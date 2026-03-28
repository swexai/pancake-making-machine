/**
 * @file motion_control.c
 * @brief Stepper motor (theta-axis) control implementation
 */

#include "main.h"
#include "motion_control.h"
#include "control_system.h"
#include <string.h>
#include <math.h>

/* ============================================================================
 * PRIVATE VARIABLES
 * ========================================================================= */

static stepper_motor_t g_stepper = {0};
static motion_profile_t g_profile = {0};
static bool g_home_in_progress = false;

/* ============================================================================
 * FUNCTION IMPLEMENTATIONS
 * ========================================================================= */

/**
 * @brief Initialize motion control module
 */
void motion_init(void)
{
    memset(&g_stepper, 0, sizeof(g_stepper));
    memset(&g_profile, 0, sizeof(g_profile));
    
    g_stepper.target_rpm = NOMINAL_SPEED_RPM;
    g_stepper.is_enabled = false;
    g_stepper.is_homed = false;
    
    g_profile.max_acceleration = 2.0f;  /* rev/s^2 - conservative for smooth motion */
    
    motion_enable(false);
}

/**
 * @brief Home the theta axis using hub sensor
 * 
 * Procedure:
 * 1. Slow ramp to HOMING_SPEED_RPM
 * 2. Wait for home switch to trigger
 * 3. Back off and latch position as zero
 * 4. Ramp to nominal speed
 */
void motion_home_axis(void)
{
    g_home_in_progress = true;
    g_stepper.is_homed = false;
    
    /* Start slow approach */
    motion_enable(true);
    motion_set_target_speed(HOMING_SPEED_RPM);
    
    /* Wait for home switch (this would be polled in main loop) */
    /* Timeout after 10 seconds */
    uint32_t timeout_ms = HAL_GetTick() + 10000;
    
    while (HAL_GetTick() < timeout_ms && !g_stepper.is_homed) {
        /* Poll home switch - assuming it's on a GPIO */
        /* This is pseudo-code; actual implementation depends on hardware */
        if (HAL_GPIO_ReadPin(NC_Switch_GPIO_Port, NC_Switch_Pin) == GPIO_PIN_SET) {  /* Home switch active */
            g_stepper.position_revolutions = 0.0f;
            g_stepper.step_counter = 0;
            g_stepper.is_homed = true;
            break;
        }
    }
    
    if (!g_stepper.is_homed) {
        /* Homing failed - disable motor */
        motion_enable(false);
    } else {
        /* Ramp to nominal speed */
        motion_set_target_speed(NOMINAL_SPEED_RPM);
    }
    
    g_home_in_progress = false;
}

/**
 * @brief Enable/disable stepper motor
 * @param enable true to enable, false to disable
 */
void motion_enable(bool enable)
{
    g_stepper.is_enabled = enable;

    if (enable) {
        /* Set EN_THETA high to enable driver */
        HAL_GPIO_WritePin(EN_THETA_GPIO_Port, EN_THETA_Pin, GPIO_PIN_SET);
    } else {
        /* Set EN_THETA low to disable driver */
        HAL_GPIO_WritePin(EN_THETA_GPIO_Port, EN_THETA_Pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief Set target rotational speed
 * @param rpm Target speed in revolutions per minute
 */
void motion_set_target_speed(float rpm)
{
    g_stepper.target_rpm = rpm;
    g_profile.target_velocity = rpm / 60.0f;  /* Convert to rev/s */
}

void motion_set_target_rpm(float rpm)
{
    motion_set_target_speed(rpm);
}

/**
 * @brief Generate step pulse to stepper driver
 * 
 * TB6600 expects:
 * - STEP input: pulse to advance by one microstep
 * - DIR input: direction control (high = CW, low = CCW)
 * - EN input: enable (high = enabled, low = disabled)
 */
void motion_step(void)
{
    /* Generate step pulse (assuming connected to timer output/PWM) */
    /* Pulse width: typically 1-2 microseconds */
    
    /* Set direction (assuming CW rotation) */
    HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET);
    
    /* Toggle STEP pin (this would be done via timer or GPIO) */
    extern TIM_HandleTypeDef htim2;
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    
    g_stepper.step_counter++;
}

/**
 * @brief Get current axis position in revolutions
 * @return Position in revolutions (0 at home)
 */
float motion_get_position(void)
{
    return g_stepper.position_revolutions;
}

/**
 * @brief Get current rotational speed
 * @return Speed in RPM
 */
float motion_get_rpm(void)
{
    return g_stepper.current_rpm;
}

/**
 * @brief Check if axis is homed
 * @return true if homing successful, false otherwise
 */
bool motion_is_homed(void)
{
    return g_stepper.is_homed;
}

/**
 * @brief Check if stepper is enabled
 * @return true if enabled, false otherwise
 */
bool motion_is_enabled(void)
{
    return g_stepper.is_enabled;
}

/**
 * @brief Update motion profile and generate steps
 * Called periodically (typically 1-10 kHz depending on speed)
 * 
 * This handles:
 * - Smooth acceleration/deceleration ramps
 * - Step pulse generation at proper intervals
 * - Position tracking
 */
void motion_update(void)
{
    static uint32_t last_step_time_us = 0;
    uint32_t now_us = HAL_GetMicrosecond();  /* Pseudo-function */
    
    if (!g_stepper.is_enabled) {
        return;
    }
    
    /* Acceleration/deceleration profile */
    float vel_error = g_profile.target_velocity - g_profile.current_velocity;
    float max_vel_change = g_profile.max_acceleration * 0.001f;  /* Per 1ms */
    
    if (vel_error > max_vel_change) {
        g_profile.current_velocity += max_vel_change;
    } else if (vel_error < -max_vel_change) {
        g_profile.current_velocity -= max_vel_change;
    } else {
        g_profile.current_velocity = g_profile.target_velocity;
    }
    
    g_stepper.current_rpm = g_profile.current_velocity * 60.0f;
    
    /* Calculate step interval based on velocity */
    float steps_per_second = g_profile.current_velocity * MOTOR_STEPS_PER_REV;
    float step_interval_us = (steps_per_second > 0) ? (1000000.0f / steps_per_second) : 0;
    
    /* Generate step pulses when time is right */
    if ((now_us - last_step_time_us) >= step_interval_us) {
        motion_step();
        last_step_time_us = now_us;
        
        /* Update position counter */
        g_stepper.position_revolutions += 1.0f / MOTOR_STEPS_PER_REV;
    }
}
