/**
 * @file safety_control.c
 * @brief Safety monitoring and emergency stop control
 */

#include "main.h"
#include "safety_control.h"
#include "control_system.h"
#include "thermal_control.h"
#include <string.h>

/* ============================================================================
 * PRIVATE VARIABLES
 * ========================================================================= */

static safety_inputs_t g_safety_inputs = {0};
static safety_fault_status_t g_fault_status = {0};

/* ============================================================================
 * PRIVATE FUNCTIONS
 * ========================================================================= */

/**
 * @brief Disable all system outputs (heater, motor, pump)
 */
static void disable_all_outputs(void)
{
    thermal_ssr_enable(false);

    HAL_GPIO_WritePin(EN_THETA_GPIO_Port, EN_THETA_Pin, GPIO_PIN_RESET);

    extern TIM_HandleTypeDef htim1;
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);  /* Pump PWM */
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);  /* SSR */
}

/* ============================================================================
 * FUNCTION IMPLEMENTATIONS
 * ========================================================================= */

/**
 * @brief Initialize safety module
 */
void safety_init(void)
{
    memset(&g_safety_inputs, 0, sizeof(g_safety_inputs));
    memset(&g_fault_status, 0, sizeof(g_fault_status));
    
    g_fault_status.fault_flags = FAULT_NONE;
    g_fault_status.immediate_shutdown_required = false;
}

/**
 * @brief Check E-stop input with debouncing
 * E-stop is active LOW (NC switch) - pressed = 0, released = 1
 */
void safety_estop_check(void)
{
    GPIO_PinState estop_pin = HAL_GPIO_ReadPin(ESTOP_GPIO_Port, ESTOP_Pin);
    
    if (estop_pin == GPIO_PIN_RESET) {
        /* E-stop pressed (active low) */
        g_safety_inputs.estop_debounce_counter++;
        
        if (g_safety_inputs.estop_debounce_counter >= (ESTOP_DEBOUNCE_MS / 1)) {
            if (!g_safety_inputs.estop_pressed) {
                /* Transition from released to pressed */
                g_safety_inputs.estop_pressed = true;
                safety_set_fault(FAULT_ESTOP);
                g_fault_status.immediate_shutdown_required = true;
            }
        }
    } else {
        /* E-stop released */
        g_safety_inputs.estop_debounce_counter = 0;
        if (g_safety_inputs.estop_pressed) {
            g_safety_inputs.estop_pressed = false;
            safety_clear_fault(FAULT_ESTOP);
        }
    }
}

/**
 * @brief Check door/cover interlock with debouncing
 * Cover switch is active LOW (NC) - closed = 1, open = 0
 */
void safety_cover_check(void)
{
    GPIO_PinState cover_pin = HAL_GPIO_ReadPin(NC_Switch_GPIO_Port, NC_Switch_Pin);
    
    if (cover_pin == GPIO_PIN_RESET) {
        /* Cover open */
        g_safety_inputs.cover_debounce_counter++;
        
        if (g_safety_inputs.cover_debounce_counter >= (COVER_DEBOUNCE_MS / 1)) {
            if (!g_safety_inputs.cover_open) {
                g_safety_inputs.cover_open = true;
                safety_set_fault(FAULT_COVER_OPEN);
                g_fault_status.immediate_shutdown_required = true;
            }
        }
    } else {
        /* Cover closed */
        g_safety_inputs.cover_debounce_counter = 0;
        if (g_safety_inputs.cover_open) {
            g_safety_inputs.cover_open = false;
            safety_clear_fault(FAULT_COVER_OPEN);
        }
    }
}

/**
 * @brief Check thermal cutoff condition
 */
void safety_thermal_check(void)
{
    float current_temp = thermal_get_temperature();
    
    if (current_temp >= THERMAL_CUTOFF_THRESHOLD_C) {
        if (!g_safety_inputs.thermal_cutoff_active) {
            g_safety_inputs.thermal_cutoff_active = true;
            safety_set_fault(FAULT_THERMAL_CUTOFF);
            g_fault_status.immediate_shutdown_required = true;
        }
    } else if (current_temp < (THERMAL_CUTOFF_THRESHOLD_C - 10.0f)) {
        /* Hysteresis: clear fault only after dropping 10°C below threshold */
        if (g_safety_inputs.thermal_cutoff_active) {
            g_safety_inputs.thermal_cutoff_active = false;
            safety_clear_fault(FAULT_THERMAL_CUTOFF);
        }
    }
}

/**
 * @brief Run complete safety monitoring (1 kHz rate)
 */
void safety_monitor(void)
{
    static uint32_t last_check_ms = 0;
    uint32_t now_ms = HAL_GetTick();
    
    if ((now_ms - last_check_ms) < 1) {  /* 1 ms = 1 kHz */
        return;
    }
    last_check_ms = now_ms;
    
    /* Check all safety inputs */
    safety_estop_check();
    safety_cover_check();
    safety_thermal_check();
    
    /* If any critical fault, disable all outputs */
    if (g_fault_status.immediate_shutdown_required) {
        safety_shutdown();
    }
}

/**
 * @brief Check if all safety systems are OK
 * @return true if no faults, false if any fault active
 */
bool safety_all_systems_ok(void)
{
    return (g_fault_status.fault_flags == FAULT_NONE);
}

/**
 * @brief Perform emergency shutdown
 * Hard-cuts all outputs regardless of firmware state
 */
void safety_shutdown(void)
{
    disable_all_outputs();
    g_system_state.current_mode = MODE_ESTOP;
}

/**
 * @brief Disable all system outputs
 */
void safety_disable_all_outputs(void)
{
    disable_all_outputs();
}

/**
 * @brief Set a fault flag
 * @param fault Fault code to set
 */
void safety_set_fault(safety_fault_t fault)
{
    g_fault_status.fault_flags |= (uint32_t)fault;
    g_fault_status.fault_timestamp_ms = HAL_GetTick();
}

/**
 * @brief Clear a fault flag
 * @param fault Fault code to clear
 */
void safety_clear_fault(safety_fault_t fault)
{
    g_fault_status.fault_flags &= ~((uint32_t)fault);
}

/**
 * @brief Check if a specific fault is active
 * @param fault Fault code to check
 * @return true if fault is set, false otherwise
 */
bool safety_has_fault(safety_fault_t fault)
{
    return ((g_fault_status.fault_flags & (uint32_t)fault) != 0);
}

/**
 * @brief Get human-readable fault description
 * @param fault Fault code
 * @return Pointer to fault description string
 */
const char* safety_fault_description(safety_fault_t fault)
{
    switch (fault) {
        case FAULT_NONE:
            return "No fault";
        case FAULT_ESTOP:
            return "Emergency stop active";
        case FAULT_COVER_OPEN:
            return "Door/cover open";
        case FAULT_THERMAL_CUTOFF:
            return "Thermal cutoff triggered";
        case FAULT_SENSOR_LOSS:
            return "Sensor communication lost";
        case FAULT_MOTOR_STALL:
            return "Motor stall detected";
        case FAULT_PUMP_LOSS:
            return "Pump pressure loss";
        case FAULT_OVERCURRENT:
            return "Over-current detected";
        case FAULT_SYSTEM_WATCHDOG:
            return "System watchdog timeout";
        default:
            return "Unknown fault";
    }
}
