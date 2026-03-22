/**
 * @file thermal_control.h
 * @brief Temperature control module with PID loop
 */

#ifndef THERMAL_CONTROL_H
#define THERMAL_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

/* PT100 RTD over MAX31865 */
#define RTD_NOMINAL_OHM         100.0f
#define RTD_REFERENCE_OHM       430.0f
#define RTD_A                   3.9083e-3f
#define RTD_B                   -5.775e-7f

typedef struct {
    float temperature_c;
    float raw_adc;
    uint32_t last_read_ms;
} temperature_sensor_t;

typedef struct {
    float setpoint_c;
    float kp;
    float ki;
    float kd;
    float integral;
    float previous_error;
    float output;
    uint32_t last_update_ms;
} thermal_pid_t;

void thermal_init(void);
float thermal_get_temperature(void);
bool thermal_read_rtd_spi(void);
void thermal_pid_reset(void);
void thermal_pid_update(void);
float thermal_pid_compute(float setpoint, float measurement);
void thermal_ssr_enable(bool enable);
bool thermal_is_stable(void);

#endif /* THERMAL_CONTROL_H */
