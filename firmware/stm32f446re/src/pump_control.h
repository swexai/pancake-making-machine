/**
 * @file pump_control.h
 * @brief Dispensing subsystem (pump PWM + manifold + 7 nozzles)
 */

#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include <stdint.h>
#include <stddef.h>

/* ==================== Type Definitions ================================ */

typedef enum {
    PUMP_MODE_RUN = 0,  /* Dispense (pump → 7 nozzles) */
    PUMP_MODE_CIP = 1   /* Clean-in-place (pump → recirculate) */
} PumpMode_e;

typedef struct {
    PumpMode_e mode;           /* Current pump mode (RUN/CIP) */
    uint16_t pwm_duty;         /* PWM duty cycle (0–1999 for 10 Hz) */
    float flow_rate_ml_s;      /* Estimated flow rate (mL/s) */
    uint32_t dispense_volume_ml; /* Target dispense volume (e.g., 141 mL = ~5 s @ 28.3 mL/s) */
    uint32_t dispense_time_ms;  /* Dispense duration (e.g., 5000 ms = 5 s) */
    uint8_t is_dispensing;      /* 1 = active, 0 = idle */
} PumpControl_t;

/* ==================== Function Prototypes ============================ */

/**
 * @brief Initialize pump subsystem (TIM4 PWM on PB6, manifold GPIO)
 */
void Pump_Init(void);

/**
 * @brief Set pump mode (RUN or CIP)
 * @param[in] mode: PUMP_MODE_RUN or PUMP_MODE_CIP
 */
void Pump_SetMode(PumpMode_e mode);

/**
 * @brief Set pump PWM duty cycle
 * @param[in] duty: 0–1999 (0–100% at 10 Hz)
 */
void Pump_SetDuty(uint16_t duty);

/**
 * @brief Start dispensing for a fixed duration
 * @param[in] duration_ms: how long to dispense (e.g., 5000 ms)
 * @param[in] flow_percent: pump speed 0–100
 */
void Pump_Dispense(uint32_t duration_ms, uint8_t flow_percent);

/**
 * @brief Stop dispensing (pump PWM → 0)
 */
void Pump_Stop(void);

/**
 * @brief Get current pump state
 */
PumpControl_t* Pump_GetState(void);

/**
 * @brief Check if dispensing is active
 */
uint8_t Pump_IsDispensing(void);

/**
 * @brief FreeRTOS task: Pump control (10 Hz PWM monitoring & timing)
 */
void Pump_ControlTask(void *pvParameters);

#endif /* PUMP_CONTROL_H */
