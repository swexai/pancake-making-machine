/**
 * @file control_system.c
 * @brief Main control system integration
 */

#include "main.h"
#include "control_system.h"
#include "thermal_control.h"
#include "motion_control.h"
#include "pump_control.h"
#include "safety_control.h"
#include "hmi.h"
#include "logging.h"
#include <string.h>

/* ============================================================================
 * GLOBAL VARIABLES
 * ========================================================================= */

system_state_t g_system_state = {0};

/* ============================================================================
 * PRIVATE STATE
 * ========================================================================= */

static const char *g_mode_names[] = {
    "IDLE", "WARMUP", "READY", "DISPENSE", "COOLDOWN", "CIP", "ERROR", "ESTOP"
};

/* ============================================================================
 * FUNCTION IMPLEMENTATIONS
 * ========================================================================= */

/**
 * @brief Initialize entire control system
 */
void control_system_init(void)
{
    /* Initialize global state */
    memset(&g_system_state, 0, sizeof(g_system_state));
    g_system_state.current_mode = MODE_IDLE;
    g_system_state.thermal.setpoint_c = TEMP_SETPOINT_DEFAULT_C;
    
    /* Initialize modules */
    safety_init();      /* FIRST - safety is critical */
    thermal_init();
    motion_init();
    pump_init();
    hmi_init();
    logging_init();
    
    uart_printf("\r\nControl System Initialized\r\n");
}

/**
 * @brief Get current machine mode name
 * @return Pointer to mode name string
 */
const char* control_system_get_mode_name(void)
{
    if (g_system_state.current_mode < 8) {
        return g_mode_names[g_system_state.current_mode];
    }
    return "UNKNOWN";
}

/**
 * @brief State machine for main control flow
 */
void control_system_state_machine(void)
{
    machine_mode_t next_mode = g_system_state.current_mode;
    
    switch (g_system_state.current_mode) {
        case MODE_IDLE:
            /* Idle state: wait for command or sensor input */
            if (g_system_state.requested_mode == MODE_WARMUP) {
                next_mode = MODE_WARMUP;
                thermal_ssr_enable(true);
                g_system_state.stats.cycle_count++;
            }
            break;
            
        case MODE_WARMUP:
            /* Heat plate to setpoint (210-230°C) */
            thermal_pid_update();
            
            if (thermal_is_stable()) {
                next_mode = MODE_READY;
                uart_printf("Plate stable at setpoint\r\n");
            }
            break;
            
        case MODE_READY:
            /* Ready to dispense - maintain temperature */
            thermal_pid_update();
            
            if (g_system_state.requested_mode == MODE_DISPENSE) {
                next_mode = MODE_DISPENSE;
                
                /* Verify safe conditions before starting dispense */
                if (!motion_is_homed()) {
                    uart_printf("Error: Theta not homed!\r\n");
                    next_mode = MODE_ERROR;
                    break;
                }
                
                /* Start dispense cycle */
                motion_enable(true);
                pump_enable(true);
                motion_set_target_rpm((float)THETA_TARGET_RPM);
                pump_set_duty_cycle(85.0f);  /* TODO: calibrate based on flow requirements */
            }
            break;
            
        case MODE_DISPENSE:
            /* Active dispense: rotate plate, pump batter, monitor */
            thermal_pid_update();
            motion_update();
            pump_update();
            
            /* Check if dispense cycle complete (~5 seconds) */
            static uint32_t dispense_start_ms = 0;
            if (dispense_start_ms == 0) {
                dispense_start_ms = HAL_GetTick();
            }
            
            if ((HAL_GetTick() - dispense_start_ms) >= DISPENSE_DURATION_MS) {
                /* End dispense */
                pump_enable(false);
                motion_enable(false);
                dispense_start_ms = 0;
                
                next_mode = MODE_COOLDOWN;
                uart_printf("Dispense complete\r\n");
            }
            break;
            
        case MODE_COOLDOWN:
            /* Cool plate after dispense */
            thermal_pid_update();
            
            /* Stay here until temperature below safe handling level (~100°C) */
            if (g_system_state.thermal.current_c < 100.0f) {
                thermal_ssr_enable(false);
                next_mode = MODE_IDLE;
            }
            
            /* Or return to READY if operator immediately wants another cycle */
            if (g_system_state.requested_mode == MODE_WARMUP) {
                next_mode = MODE_WARMUP;
            }
            break;
            
        case MODE_CIP:
            /* Cleaning-in-place: run pump without heat */
            pump_enable(true);
            pump_set_duty_cycle(100.0f);
            thermal_ssr_enable(false);
            
            /* CIP runs for fixed duration (~30 seconds) or until manual stop */
            if (g_system_state.requested_mode == MODE_IDLE) {
                pump_enable(false);
                g_system_state.stats.cip_count++;
                next_mode = MODE_IDLE;
            }
            break;
            
        case MODE_ERROR:
            /* Error state: requires manual reset */
            safety_disable_all_outputs();
            /* Stay in error until acknowledged */
            if (g_system_state.requested_mode == MODE_IDLE) {
                next_mode = MODE_IDLE;
            }
            break;
            
        case MODE_ESTOP:
            /* Emergency stop: outputs already disabled by safety */
            /* Require system reset to recover */
            break;
            
        default:
            next_mode = MODE_ERROR;
    }
    
    /* Transition to next state */
    if (next_mode != g_system_state.current_mode) {
        g_system_state.current_mode = next_mode;
    }
    
    /* Clear requested mode after processing */
    if (g_system_state.requested_mode != next_mode) {
        g_system_state.requested_mode = g_system_state.current_mode;
    }
}

/**
 * @brief Main control loop update (called from FreeRTOS task)
 * Runs at highest priority to ensure safety monitoring
 */
void control_system_update(void)
{
    /* ALWAYS check safety first */
    safety_monitor();
    
    /* Update state machine */
    control_system_state_machine();
    
    /* Update system state from module states */
    g_system_state.thermal.current_c = thermal_get_temperature();
    g_system_state.motion.current_rpm = motion_get_rpm();
    g_system_state.motion.position_revs = motion_get_position();
    g_system_state.motion.homed = motion_is_homed();
    g_system_state.pump.duty_percent = pump_get_duty_cycle();
    
    /* Check for system faults */
    if (safety_has_fault(FAULT_ESTOP) || safety_has_fault(FAULT_COVER_OPEN)) {
        g_system_state.current_mode = MODE_ESTOP;
    }
}
