/**
 * @file safety.h
 * @brief Safety subsystem (E-STOP, cover switch, hardware safety chain)
 */

#ifndef SAFETY_H
#define SAFETY_H

#include <stdint.h>
#include <stddef.h>

/* ==================== Type Definitions ================================ */

typedef enum {
    SAFETY_STATE_OK = 0,       /* All systems safe */
    SAFETY_STATE_ESTOP = 1,    /* E-STOP activated (hard-cut θ-EN & SSR) */
    SAFETY_STATE_COVER_OPEN = 2, /* Cover switch NC (hard-cut θ-EN & SSR) */
    SAFETY_STATE_FAULT = 3     /* Other safety fault */
} SafetyState_e;

typedef struct {
    SafetyState_e state;       /* Current safety state */
    uint8_t estop_ch1;         /* E-STOP Channel 1 (PA0, pull-up → active low) */
    uint8_t estop_ch2;         /* E-STOP Channel 2 (redundant, if present) */
    uint8_t cover_nc;          /* Cover switch NC (PA1, pull-up → active low when open) */
    uint8_t mcb_active;        /* MCB indicator (PA2, set when power available) */
    uint32_t fault_count;      /* Counter for repeated faults */
} SafetyControl_t;

/* ==================== Function Prototypes ============================ */

/**
 * @brief Initialize safety subsystem (GPIO inputs PA0, PA1, PA2)
 */
void Safety_Init(void);

/**
 * @brief Read all safety inputs (E-STOP, cover, MCB)
 * @return: current SafetyState_e
 */
SafetyState_e Safety_ReadInputs(void);

/**
 * @brief Get current safety state
 */
SafetyControl_t* Safety_GetState(void);

/**
 * @brief Check if system is safe to operate
 * @return: 1 (safe), 0 (fault)
 */
uint8_t Safety_IsSystemSafe(void);

/**
 * @brief Emergency stop: de-energize heater & motor
 *        Hardware safety chain hard-cuts θ-EN & SSR, but firmware
 *        also stops control tasks for consistency.
 */
void Safety_EStop(void);

/**
 * @brief Reset fault state (e.g., after operator clears E-STOP)
 */
void Safety_ResetFault(void);

/**
 * @brief FreeRTOS task: Safety monitor (1 kHz scan)
 *        Polls E-STOP, cover switch, and triggers de-energization if needed.
 */
void Safety_MonitorTask(void *pvParameters);

#endif /* SAFETY_H */
