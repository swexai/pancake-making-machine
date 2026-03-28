/**
 * @file thermal_control.c
 * @brief Temperature control implementation with PID loop
 */

#include "main.h"
#include "thermal_control.h"
#include "control_system.h"
#include <math.h>
#include <string.h>

/* ============================================================================
 * PRIVATE VARIABLES
 * ========================================================================= */

static temperature_sensor_t g_rtd_sensor = {0};
static thermal_pid_t g_thermal_pid = {0};

/* PID Tuning parameters */
/* These values should be tuned experimentally; starting with conservative gains */
static const float PID_KP = 50.0f;    /* Proportional gain */
static const float PID_KI = 15.0f;    /* Integral gain */
static const float PID_KD = 5.0f;     /* Derivative gain */
static const float PID_INTEGRAL_MAX = 100.0f;  /* Anti-windup limit */
static const float SSR_DUTY_MIN = 5.0f;    /* Minimum SSR PWM duty (%) */
static const float SSR_DUTY_MAX = 90.0f;   /* Maximum SSR PWM duty (%) */

/* ============================================================================
 * FUNCTION IMPLEMENTATIONS
 * ========================================================================= */

/**
 * @brief Initialize thermal control module
 */
void thermal_init(void)
{
    memset(&g_rtd_sensor, 0, sizeof(g_rtd_sensor));
    memset(&g_thermal_pid, 0, sizeof(g_thermal_pid));
    
    g_thermal_pid.setpoint_c = TEMP_SETPOINT_DEFAULT_C;
    g_thermal_pid.kp = PID_KP;
    g_thermal_pid.ki = PID_KI;
    g_thermal_pid.kd = PID_KD;
    
    thermal_ssr_enable(false);
}

/**
 * @brief Read PT100 RTD temperature via MAX31865 over SPI
 * In simulation mode, simulates temperature ramping up over time
 * 
 * The MAX31865 is a 2-wire RTD-to-digital converter.
 * Register map:
 *   0x00: Configuration
 *   0x01: RTD MSB (15-8)
 *   0x02: RTD LSB (7-0)
 * 
 * @return true if read successful, false otherwise
 */
bool thermal_read_rtd_spi(void)
{
#if SIMULATION_MODE
    /* Simulation: Temperature starts at 25°C and ramps up when heating is enabled */
    static float sim_temp = 25.0f;
    static uint32_t last_sim_time = 0;
    uint32_t now = HAL_GetTick();
    
    if (last_sim_time == 0) last_sim_time = now;
    
    float dt_seconds = (now - last_sim_time) / 1000.0f;
    last_sim_time = now;
    
    /* Check if SSR is enabled (heating on) */
    extern TIM_HandleTypeDef htim1;
    bool heating_on = (htim1.Instance != NULL && HAL_TIM_PWM_GetState(&htim1) == HAL_TIM_STATE_BUSY);
    
    if (heating_on) {
        /* Ramp up temperature at 2°C per second when heating */
        sim_temp += 2.0f * dt_seconds;
        if (sim_temp > 250.0f) sim_temp = 250.0f;  /* Safety limit */
    } else {
        /* Cool down slowly */
        sim_temp -= 0.5f * dt_seconds;
        if (sim_temp < 20.0f) sim_temp = 20.0f;
    }
    
    g_rtd_sensor.temperature_c = sim_temp;
    g_rtd_sensor.raw_adc = (sim_temp / 300.0f) * 32768.0f;  /* Fake ADC value */
    g_rtd_sensor.last_read_ms = now;
    
    return true;
#else
    uint8_t tx_buf[3] = {0x00, 0x00, 0x00};  /* Read config+rtd */
    uint8_t rx_buf[3] = {0x00, 0x00, 0x00};
    
    extern SPI_HandleTypeDef hspi1;
    
    /* Read RTD value from MAX31865 */
    tx_buf[0] = 0x01;  /* RTD MSB register */
    
    if (HAL_SPI_TransmitReceive(&hspi1, tx_buf, rx_buf, 3, 100) != HAL_OK) {
        return false;
    }
    
    /* Extract RTD raw value (15-bit) */
    uint16_t rtd_raw = ((rx_buf[1] << 8) | rx_buf[2]) >> 1;
    g_rtd_sensor.raw_adc = (float)rtd_raw;
    
    /* Convert RTD raw value to resistance (Ohms) */
    float rtd_resistance = (rtd_raw / 32768.0f) * RTD_REFERENCE_OHM;
    
    /* Solve Callendar-Van Dusen equation for temperature
     * R(T) = R0[1 + A*T + B*T^2 + C*T^3*(T-100)]  for T < 0
     * R(T) = R0[1 + A*T + B*T^2]                   for T >= 0
     * 
     * Simplified quadratic solution for common case (T >= 0):
     */
    float a = RTD_A;
    float b = RTD_B;
    float c = -(rtd_resistance / RTD_NOMINAL_OHM - 1.0f);
    
    float discriminant = a*a - 4.0f*b*c;
    if (discriminant < 0.0f) {
        return false;
    }
    
    float temp_c = (-a + sqrtf(discriminant)) / (2.0f * b);
    
    /* Sanity check */
    if (temp_c < 0.0f || temp_c > 300.0f) {
        return false;
    }
    
    g_rtd_sensor.temperature_c = temp_c;
    g_rtd_sensor.last_read_ms = HAL_GetTick();
    
    return true;
#endif
}

/**
 * @brief Get current measured temperature in Celsius
 * @return Temperature in °C
 */
float thermal_get_temperature(void)
{
    return g_rtd_sensor.temperature_c;
}

/**
 * @brief Reset PID controller (clear integral term, etc.)
 */
void thermal_pid_reset(void)
{
    g_thermal_pid.integral = 0.0f;
    g_thermal_pid.previous_error = 0.0f;
    g_thermal_pid.output = 0.0f;
}

/**
 * @brief Compute PID output given setpoint and measurement
 * 
 * @param setpoint Target temperature (°C)
 * @param measurement Current temperature (°C)
 * @return PID output (0-100 for SSR duty %)
 */
float thermal_pid_compute(float setpoint, float measurement)
{
    float error = setpoint - measurement;
    float dt = T_SAMPLE_THERMALS / 1000.0f;  /* Convert ms to seconds */
    
    /* Proportional term */
    float p_term = g_thermal_pid.kp * error;
    
    /* Integral term (with anti-windup) */
    g_thermal_pid.integral += g_thermal_pid.ki * error * dt;
    if (g_thermal_pid.integral > PID_INTEGRAL_MAX) {
        g_thermal_pid.integral = PID_INTEGRAL_MAX;
    }
    if (g_thermal_pid.integral < 0.0f) {
        g_thermal_pid.integral = 0.0f;
    }
    float i_term = g_thermal_pid.integral;
    
    /* Derivative term (rate of change) */
    float d_error = (error - g_thermal_pid.previous_error) / dt;
    float d_term = g_thermal_pid.kd * d_error;
    g_thermal_pid.previous_error = error;
    
    /* Compute output */
    float output = p_term + i_term + d_term;
    
    /* Clamp to SSR duty limits */
    if (output < SSR_DUTY_MIN) {
        output = SSR_DUTY_MIN;
    }
    if (output > SSR_DUTY_MAX) {
        output = SSR_DUTY_MAX;
    }
    
    g_thermal_pid.output = output;
    return output;
}

/**
 * @brief Update thermal control loop
 * Called at 25 Hz (40 ms intervals)
 */
void thermal_pid_update(void)
{
    static uint32_t last_update_ms = 0;
    uint32_t now_ms = HAL_GetTick();
    
    /* Check if enough time has passed */
    if ((now_ms - last_update_ms) < T_SAMPLE_THERMALS) {
        return;
    }
    last_update_ms = now_ms;
    
    /* Read temperature sensor */
    if (!thermal_read_rtd_spi()) {
        /* Sensor read failed - reduce heating as safety measure */
        thermal_ssr_enable(false);
        return;
    }
    
    /* Compute PID output */
    float ssr_duty = thermal_pid_compute(g_thermal_pid.setpoint_c, 
                                         g_rtd_sensor.temperature_c);
    
    /* Apply SSR control at reduced frequency (10 Hz vs 25 Hz sensor sampling) */
    static uint32_t ssr_update_counter = 0;
    if ((ssr_update_counter++ % 3) == 0) {  /* Every 3rd call = ~10 Hz */
        /* Set SSR PWM duty via TIM */
        extern TIM_HandleTypeDef htim1;
        uint32_t period = htim1.Init.Period + 1;
        uint32_t pulse = (period * (uint32_t)ssr_duty) / 100;
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse);
        
#if SIMULATION_MODE
        /* Debug print in simulation */
        extern void uart_printf(const char *format, ...);
        static uint32_t last_debug_ms = 0;
        if (now_ms - last_debug_ms > 1000) {  /* Print every second */
            uart_printf("SIM: Temp=%.1f C, Setpoint=%.1f C, Duty=%.1f%%\r\n",
                       g_rtd_sensor.temperature_c, g_thermal_pid.setpoint_c, ssr_duty);
            last_debug_ms = now_ms;
        }
#endif
    }
}

/**
 * @brief Enable/disable SSR heater control
 * @param enable true to enable heating, false to disable
 */
void thermal_ssr_enable(bool enable)
{
    extern TIM_HandleTypeDef htim1;
    
    if (enable) {
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    } else {
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    }
}

/**
 * @brief Check if temperature is stable within tolerance
 * @return true if within ±3°C of setpoint, false otherwise
 */
bool thermal_is_stable(void)
{
    float error = fabsf(g_thermal_pid.setpoint_c - g_rtd_sensor.temperature_c);
    return (error <= TEMP_TOLERANCE_C);
}
