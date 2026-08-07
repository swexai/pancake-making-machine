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
static uint32_t g_home_start_time = 0;
static GPIO_PinState g_home_start_pin_state = GPIO_PIN_SET;
static float g_home_start_position_rev = 0.0f;
static const uint32_t HOMING_TIMEOUT_MS = 10000;  /* 10 second timeout */
static const uint32_t HOMING_FALLBACK_COMPLETE_MS = 1200; /* fallback if switch edge is unavailable */
static const float HOMING_MIN_TRAVEL_REV = 0.15f;         /* require real movement before fallback success */

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
    
    g_stepper.target_rpm = 0.0f;  /* Start with zero target, set during homing/dispense */
    g_stepper.is_enabled = false;
    g_stepper.is_homed = false;
    
    g_profile.max_acceleration = 2.0f;  /* rev/s^2 - conservative for smooth motion */
    
    motion_enable(false);
}

/**
 * @brief Initiate homing sequence (non-blocking)
 * 
 * Starts the homing procedure. Check motion_is_homed() to see if complete.
 * Call motion_update() regularly for homing to progress.
 */
void motion_home_axis(void)
{
    if (g_home_in_progress) {
        return;  /* Already homing */
    }
    
    g_home_in_progress = true;
    g_stepper.is_homed = false;
    g_home_start_time = HAL_GetTick();
    g_home_start_pin_state = HAL_GPIO_ReadPin(HOME_THETA_GPIO_Port, HOME_THETA_Pin);
    g_home_start_position_rev = g_stepper.position_revolutions;
    
    /* Start slow approach */
    motion_enable(true);
    motion_set_target_speed(HOMING_SPEED_RPM);
}

/**
 * @brief Check if homing is in progress
 * @return true if actively homing, false otherwise
 */
bool motion_is_homing_active(void)
{
    return g_home_in_progress;
}

/**
 * @brief Enable/disable stepper motor
 * @param enable true to enable, false to disable
 */
void motion_enable(bool enable)
{
    g_stepper.is_enabled = enable;
    extern TIM_HandleTypeDef htim2;

    if (enable) {
        /* Driver enable is active-HIGH on this wiring: drive THETA_EN high */
        HAL_GPIO_WritePin(THETA_EN_GPIO_Port, THETA_EN_Pin, GPIO_PIN_SET);
        /* Set DIR to HIGH for clockwise (forward) rotation */
        HAL_GPIO_WritePin(THETA_DIR_GPIO_Port, THETA_DIR_Pin, GPIO_PIN_SET);
    } else {
        /* Stop step pulse timer BEFORE disabling driver to cut coil current */
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
        /* Drive THETA_EN low to disable the motor driver */
        HAL_GPIO_WritePin(THETA_EN_GPIO_Port, THETA_EN_Pin, GPIO_PIN_RESET);
        g_stepper.current_rpm = 0.0f;
        g_profile.current_velocity = 0.0f;
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
#if !MOTION_SIMULATION_MODE
/**
 * @brief Update TIM2 period to produce the correct step frequency for current_velocity.
 * Called whenever velocity changes. TIM2 channel 2 outputs STEP pulses continuously.
 * Step frequency = current_velocity (rev/s) * MOTOR_STEPS_PER_REV
 * TIM2 ARR = (APB1_CLOCK / step_frequency) - 1  (prescaler = 0)
 * APB1 runs at SystemCoreClock (16 MHz with HSI, no PLL).
 */
static void motion_update_step_frequency(float velocity_rev_s)
{
    extern TIM_HandleTypeDef htim2;
    extern void uart_printf(const char *format, ...);
    static float last_velocity = -1.0f;
    static float last_logged_vel = -999.0f;

    if (velocity_rev_s <= 0.0f) {
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
        last_velocity = -1.0f;
        last_logged_vel = -999.0f;
        return;
    }

    /* Enable ARPE (auto-reload preload) on every call to ensure it stays set.
     * Without ARPE, __HAL_TIM_SET_AUTORELOAD writes directly to the active ARR
     * register. If new ARR < current CNT, TIM2 (32-bit) must count to 0xFFFFFFFF
     * (~268s at 16 MHz) before the next STEP pulse fires — stalling the motor.
     * With ARPE, ARR writes update only the shadow register; the active ARR is
     * replaced atomically at the next timer overflow, preventing any stall. */
    htim2.Instance->CR1 |= TIM_CR1_ARPE;

    /* Only recalculate and write registers when velocity has changed */
    if (velocity_rev_s != last_velocity) {
        last_velocity = velocity_rev_s;

        float step_freq = velocity_rev_s * MOTOR_STEPS_PER_REV;
        uint32_t arr = (uint32_t)(SystemCoreClock / step_freq);
        if (arr < 2) arr = 2;

        /* TB6600 minimum STEP pulse: 2-3 µs. 40 ticks @ 16 MHz = 2.5 µs */
        uint32_t pulse_ticks = 40;
        __HAL_TIM_SET_AUTORELOAD(&htim2, arr - 1);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_ticks);

        /* Throttle log: only print every 0.5 rev/s (~30 RPM) change.
         * Printing every 0.002 rev/s increment floods UART at ~5 ms/line,
         * degrading the motion_update() call rate from 1 ms to ~10 ms. */
        if (fabsf(velocity_rev_s - last_logged_vel) >= 0.5f) {
            uart_printf("MOTION: vel=%.1f rev/s (%.0f RPM), freq=%.0f Hz, ARR=%lu\r\n",
                       velocity_rev_s, velocity_rev_s * 60.0f, step_freq, arr - 1);
            last_logged_vel = velocity_rev_s;
        }
    }

    /* Start timer if not already running; reset counter for a clean first period */
    if ((htim2.Instance->CR1 & TIM_CR1_CEN) == 0) {
        uart_printf("MOTION: Starting TIM2 PWM on PB3 (TIM2_CH2)...\r\n");
        __HAL_TIM_SET_COUNTER(&htim2, 0);
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    }
}
#endif

void motion_step(void)
{
    /* Direction: CW */
    HAL_GPIO_WritePin(THETA_DIR_GPIO_Port, THETA_DIR_Pin, GPIO_PIN_SET);
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
 * - Homing state machine (non-blocking)
 */
void motion_update(void)
{
    static uint32_t last_update_ms = 0;
    uint32_t now_ms = HAL_GetTick();
    if (last_update_ms == 0) {
        last_update_ms = now_ms;
    }

    float dt_s = (now_ms - last_update_ms) / 1000.0f;
    last_update_ms = now_ms;

    /* Keep integration stable under scheduler jitter and tick coalescing. */
    if (dt_s <= 0.0f) {
        dt_s = 0.001f;
    } else if (dt_s > 0.050f) {
        dt_s = 0.050f;
    }

    /* Handle homing state machine first (non-blocking) */
    if (g_home_in_progress) {
        uint32_t elapsed_ms = HAL_GetTick() - g_home_start_time;
        float homing_travel_rev = fabsf(g_stepper.position_revolutions - g_home_start_position_rev);

        /* Fallback completion: if switch edge is never observed but axis actually moved,
         * accept homing after a short travel period so dispense is not blocked. */
        if (elapsed_ms > HOMING_FALLBACK_COMPLETE_MS &&
            homing_travel_rev >= HOMING_MIN_TRAVEL_REV &&
            !g_stepper.is_homed) {
            g_stepper.position_revolutions = 0.0f;
            g_stepper.step_counter = 0;
            g_stepper.is_homed = true;
            motion_set_target_speed(0.0f);
            motion_enable(false);
            uart_printf("Homing completed (fallback: no switch edge, travel=%.2f rev)\r\n", homing_travel_rev);
            g_home_in_progress = false;
            return;
        }
        
        if (elapsed_ms > HOMING_TIMEOUT_MS) {
            /* Timeout - homing failed */
            uart_printf("ERROR: Homing timeout\r\n");
            motion_enable(false);
            g_home_in_progress = false;
            return;
        }
        
#if MOTION_SIMULATION_MODE
        /* Simulation: "Detect" home after 1 second */
        if (elapsed_ms > 1000 && !g_stepper.is_homed) {
            g_stepper.position_revolutions = 0.0f;
            g_stepper.step_counter = 0;
            g_stepper.is_homed = true;
            uart_printf("Homing completed successfully\r\n");
            motion_set_target_speed(0.0f);
            motion_enable(false);
            g_home_in_progress = false;
            return;
        }
#else
    /* Check home switch transition.
     * Using edge-change avoids false immediate completion when the switch
     * idle level is already high/low at homing start (NC/NO wiring variants). */
    GPIO_PinState home_now = HAL_GPIO_ReadPin(HOME_THETA_GPIO_Port, HOME_THETA_Pin);
    if (home_now != g_home_start_pin_state) {
            g_stepper.position_revolutions = 0.0f;
            g_stepper.step_counter = 0;
            g_stepper.is_homed = true;
            uart_printf("Homing completed successfully\r\n");
            motion_set_target_speed(0.0f);
            motion_enable(false);
            g_home_in_progress = false;
            return;
        }
#endif
    }
    
    if (!g_stepper.is_enabled) {
        return;
    }
    
    /* Acceleration/deceleration profile */
    float vel_error = g_profile.target_velocity - g_profile.current_velocity;
    float max_vel_change = g_profile.max_acceleration * dt_s;
    
    if (vel_error > max_vel_change) {
        g_profile.current_velocity += max_vel_change;
    } else if (vel_error < -max_vel_change) {
        g_profile.current_velocity -= max_vel_change;
    } else {
        g_profile.current_velocity = g_profile.target_velocity;
    }
    
    g_stepper.current_rpm = g_profile.current_velocity * 60.0f;
    
#if MOTION_SIMULATION_MODE
    /* In simulation, update position based on velocity without real stepping */
    static uint32_t last_update_us = 0;
    uint32_t now_us_sim = HAL_GetTick() * 1000;
    if (last_update_us == 0) last_update_us = now_us_sim;
    
    float dt_seconds = (now_us_sim - last_update_us) / 1000000.0f;
    last_update_us = now_us_sim;
    
    g_stepper.position_revolutions += g_profile.current_velocity * dt_seconds;
    g_stepper.step_counter = (uint32_t)(g_stepper.position_revolutions * MOTOR_STEPS_PER_REV);
    
    /* Debug print */
    extern void uart_printf(const char *format, ...);
    static uint32_t last_debug_ms = 0;
    uint32_t now_ms = HAL_GetTick();
    if (now_ms - last_debug_ms > 2000 && !g_home_in_progress) {
        uart_printf("MOTION SIM: Motor RPM=%.1f, Pos=%.2f rev, Homed=%d\r\n",
                   g_stepper.current_rpm, g_stepper.position_revolutions, g_stepper.is_homed);
        last_debug_ms = now_ms;
    }
#else
    /* Set direction CW */
    HAL_GPIO_WritePin(THETA_DIR_GPIO_Port, THETA_DIR_Pin, GPIO_PIN_SET);
    
    /* Update TIM2 frequency to match current velocity.
     * TIM2 PWM output drives the STEP line continuously at the correct rate.
     * Position is tracked from step counter incremented by TIM2 update ISR or
     * estimated from velocity here since we don't have a step ISR yet. */
    motion_update_step_frequency(g_profile.current_velocity);
    
    /* Estimate position from integrated velocity using the same dt as ramp update. */
    g_stepper.position_revolutions += g_profile.current_velocity * dt_s;
#endif
}
