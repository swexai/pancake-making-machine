/**
 * @file pump_control.h
 * @brief Pump PWM control module with per-nozzle trimming
 */

#ifndef PUMP_CONTROL_H
#define PUMP_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_NOZZLES             7
#define PUMP_PWM_FREQUENCY_HZ   10000
#define PUMP_TIMER_PERIOD       (SYSTEM_CLOCK_HZ / PUMP_PWM_FREQUENCY_HZ)

/* Flow rates (mL/s) for each concentric ring */
/* Target flow proportional to ring radius: 28.3 mL/s total */
#define RING1_FLOW_TARGET       0.8f   /* center ring, smallest */
#define RING2_FLOW_TARGET       1.2f
#define RING3_FLOW_TARGET       1.6f
#define RING4_FLOW_TARGET       2.0f
#define RING5_FLOW_TARGET       2.4f
#define RING6_FLOW_TARGET       2.8f
#define RING7_FLOW_TARGET       18.7f  /* outer ring - rest of flow */

typedef struct {
    float duty_cycle_percent;
    float current_flow_ml_s;
    bool is_enabled;
    uint32_t pwm_period;
    uint32_t pwm_pulse;
} pump_control_t;

typedef struct {
    uint8_t trim_percent[NUM_NOZZLES];  /* 0-100% trim per nozzle */
    float flow_setpoint_ml_s[NUM_NOZZLES];
    float measured_flow_ml_s[NUM_NOZZLES];
} pump_trim_t;

void pump_init(void);
void pump_enable(bool enable);
void pump_set_duty_cycle(float percent);
float pump_get_duty_cycle(void);
void pump_set_trim(uint8_t nozzle, uint8_t trim_percent);
uint8_t pump_get_trim(uint8_t nozzle);
void pump_update(void);
bool pump_is_enabled(void);

#endif /* PUMP_CONTROL_H */
