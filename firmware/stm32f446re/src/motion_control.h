/**
 * @file motion_control.h
 * @brief Motion control subsystem (Stepper motor NEMA23 + TB6600)
 */

#ifndef MOTION_CONTROL_H
#define MOTION_CONTROL_H

#include <stdint.h>
#include <stddef.h>

/* ==================== Type Definitions ================================ */

typedef enum {
    MOTION_STOPPED = 0,
    MOTION_HOMING,      /* Return to zero (home) position */
    MOTION_RUNNING,     /* Normal rotation */
    MOTION_FAULT        /* Error state */
} MotionState_e;

typedef struct {
    int32_t position_steps;    /* Current step count (0–3200 for 1 full rev) */
    int32_t target_position;   /* Target position in steps */
    uint32_t step_frequency_hz; /* STEP pulse frequency (~500 Hz, ~0.2 rev/s) */
    
    int8_t direction;          /* -1 (CCW), 0 (stopped), +1 (CW) */
    uint8_t is_homed;          /* 1 = homed, 0 = needs homing */
    MotionState_e state;       /* Current motion state */
} MotionControl_t;

/* ==================== Function Prototypes ============================ */

/**
 * @brief Initialize motion subsystem (TIM2 STEP, GPIO DIR/EN)
 */
void Motion_Init(void);

/**
 * @brief Enable motor (assert EN pin)
 */
void Motion_Enable(void);

/**
 * @brief Disable motor (de-assert EN pin) — also controlled by safety chain
 */
void Motion_Disable(void);

/**
 * @brief Set rotation direction
 * @param[in] direction: -1 (CCW), 0 (stop), +1 (CW)
 */
void Motion_SetDirection(int8_t direction);

/**
 * @brief Start rotation at target speed (0.2 rev/s → ~500 Hz STEP)
 * @param[in] rpm: revolutions per minute (or normalized speed 0–100%)
 */
void Motion_Start(uint8_t speed_percent);

/**
 * @brief Stop rotation (jerk-limited ramp-down)
 */
void Motion_Stop(void);

/**
 * @brief Home the motor (rotate until limit switch) — blocking or async
 * @return: 0 if successful, -1 on timeout
 */
int8_t Motion_Home(void);

/**
 * @brief Get current motion state
 */
MotionControl_t* Motion_GetState(void);

/**
 * @brief Get current step position
 */
int32_t Motion_GetPosition(void);

/**
 * @brief FreeRTOS task: Motion control (timer-driven STEP, ~1 kHz monitoring)
 */
void Motion_ControlTask(void *pvParameters);

/**
 * @brief ISR callback for TIM2 update (STEP pulse counting)
 */
void Motion_StepInterrupt(void);

#endif /* MOTION_CONTROL_H */
