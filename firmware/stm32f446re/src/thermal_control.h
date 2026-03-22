/**
 * @file thermal_control.h
 * @brief Temperature control subsystem (PT100 + MAX31865 + SSR + Heater)
 */

#ifndef THERMAL_CONTROL_H
#define THERMAL_CONTROL_H

#include <stdint.h>
#include <stddef.h>

/* ==================== Type Definitions ================================ */

typedef struct {
    float setpoint_deg_c;      /* Target temperature (210–230 °C) */
    float measured_temp_deg_c; /* Current measured temperature */
    float error;              /* Temperature error */
    float integral;           /* PID integral term */
    float derivative;         /* PID derivative term */
    
    /* PID Coefficients */
    float kp;                 /* Proportional gain */
    float ki;                 /* Integral gain */
    float kd;                 /* Derivative gain */
    
    uint16_t ssr_duty;        /* SSR PWM duty cycle (0–1999 for 10 Hz) */
} ThermalControl_t;

/* ==================== Function Prototypes ============================ */

/**
 * @brief Initialize thermal subsystem (SPI, MAX31865, TIM1 PWM)
 */
void Thermal_Init(void);

/**
 * @brief Read temperature from MAX31865 (PT100 RTD)
 * @param[out] temp_deg_c: measured temperature in °C
 * @return: HAL status code
 */
uint8_t Thermal_ReadTemperature(float *temp_deg_c);

/**
 * @brief Update PID controller and SSR duty cycle
 * @param[in] setpoint: target temperature (°C)
 * @param[in] measured: current temperature (°C)
 * @return: new SSR duty cycle (0–1999)
 */
uint16_t Thermal_UpdatePID(float setpoint, float measured);

/**
 * @brief Set SSR duty cycle (heater power)
 * @param[in] duty: PWM duty cycle (0–1999 for 10 Hz)
 */
void Thermal_SetSSRDuty(uint16_t duty);

/**
 * @brief Get current thermal state
 */
ThermalControl_t* Thermal_GetState(void);

/**
 * @brief FreeRTOS task: Thermal control loop (25 Hz sampling, 10 Hz SSR drive)
 */
void Thermal_ControlTask(void *pvParameters);

#endif /* THERMAL_CONTROL_H */
