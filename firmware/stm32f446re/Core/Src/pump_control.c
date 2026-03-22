/**
 * @file pump_control.c
 * @brief Pump PWM control implementation
 */

#include "main.h"
#include "pump_control.h"
#include "control_system.h"
#include <string.h>

/* ============================================================================
 * PRIVATE VARIABLES
 * ========================================================================= */

static pump_control_t g_pump = {0};
static pump_trim_t g_pump_trim = {0};

/* Flow target by ring (mL/s) - proportional to radius */
static const float ring_flow_targets[NUM_NOZZLES] = {
    RING1_FLOW_TARGET,
    RING2_FLOW_TARGET,
    RING3_FLOW_TARGET,
    RING4_FLOW_TARGET,
    RING5_FLOW_TARGET,
    RING6_FLOW_TARGET,
    RING7_FLOW_TARGET
};

/* ============================================================================
 * FUNCTION IMPLEMENTATIONS
 * ========================================================================= */

/**
 * @brief Initialize pump control module
 */
void pump_init(void)
{
    memset(&g_pump, 0, sizeof(g_pump));
    memset(&g_pump_trim, 0, sizeof(g_pump_trim));
    
    g_pump.pwm_period = PUMP_TIMER_PERIOD;
    g_pump.pwm_pulse = 0;
    g_pump.is_enabled = false;
    g_pump.duty_cycle_percent = 0.0f;
    
    /* Initialize trim values to 100% (no trim) */
    for (int i = 0; i < NUM_NOZZLES; i++) {
        g_pump_trim.trim_percent[i] = 100;
        g_pump_trim.flow_setpoint_ml_s[i] = ring_flow_targets[i];
        g_pump_trim.measured_flow_ml_s[i] = 0.0f;
    }
    
    pump_enable(false);
}

/**
 * @brief Enable or disable pump
 * @param enable true to enable, false to disable
 */
void pump_enable(bool enable)
{
    extern TIM_HandleTypeDef htim1;

    g_pump.is_enabled = enable;

    if (enable) {
        /* Start PWM on TIM1 CH2 (assumed pump PWM pin) */
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    } else {
        /* Stop PWM */
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
        g_pump.duty_cycle_percent = 0.0f;
    }
}

/**
 * @brief Set pump duty cycle
 * @param percent Duty cycle 0-100%
 */
void pump_set_duty_cycle(float percent)
{
    extern TIM_HandleTypeDef htim1;
    
    /* Clamp to valid range */
    if (percent < PUMP_DUTY_MIN_PCT) {
        percent = PUMP_DUTY_MIN_PCT;
    }
    if (percent > PUMP_DUTY_MAX_PCT) {
        percent = PUMP_DUTY_MAX_PCT;
    }
    
    g_pump.duty_cycle_percent = percent;
    
    /* Calculate PWM pulse width */
    g_pump.pwm_pulse = (g_pump.pwm_period * percent) / 100.0f;
    
    /* Update PWM */
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint32_t)g_pump.pwm_pulse);
}

/**
 * @brief Get current pump duty cycle
 * @return Duty cycle 0-100%
 */
float pump_get_duty_cycle(void)
{
    return g_pump.duty_cycle_percent;
}

/**
 * @brief Set trim value for a specific nozzle
 * @param nozzle Nozzle index 0-6
 * @param trim_percent Trim percentage 0-100 (100 = full flow, 0 = blocked)
 */
void pump_set_trim(uint8_t nozzle, uint8_t trim_percent)
{
    if (nozzle >= NUM_NOZZLES) {
        return;
    }
    
    if (trim_percent > 100) {
        trim_percent = 100;
    }
    
    g_pump_trim.trim_percent[nozzle] = trim_percent;
    
    /* Adjust flow setpoint based on trim */
    g_pump_trim.flow_setpoint_ml_s[nozzle] = 
        (ring_flow_targets[nozzle] * trim_percent) / 100.0f;
}

/**
 * @brief Get trim value for a specific nozzle
 * @param nozzle Nozzle index 0-6
 * @return Trim percentage 0-100
 */
uint8_t pump_get_trim(uint8_t nozzle)
{
    if (nozzle >= NUM_NOZZLES) {
        return 0;
    }
    return g_pump_trim.trim_percent[nozzle];
}

/**
 * @brief Check if pump is enabled
 * @return true if enabled, false otherwise
 */
bool pump_is_enabled(void)
{
    return g_pump.is_enabled;
}

/**
 * @brief Update pump control (called at 10 Hz)
 * 
 * This would:
 * - Apply dynamic duty cycle adjustments
 * - Monitor flow sensors (future)
 * - Adjust based on dispense profile
 */
void pump_update(void)
{
    static uint32_t last_update_ms = 0;
    uint32_t now_ms = HAL_GetTick();
    
    if ((now_ms - last_update_ms) < T_SAMPLE_PUMP) {
        return;
    }
    last_update_ms = now_ms;
    
    if (!g_pump.is_enabled) {
        return;
    }
    
    /* For now, maintain constant duty cycle */
    /* Future: implement adaptive control based on flow feedback */
}
