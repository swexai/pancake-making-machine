/**
 * @file control_system.h
 * @brief Main control system header for Injera machine
 * @version 1.0
 */

#ifndef CONTROL_SYSTEM_H
#define CONTROL_SYSTEM_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * SYSTEM CONSTANTS
 * ========================================================================= */

#define SYSTEM_CLOCK_HZ         180000000UL
#define T_SAMPLE_THERMALS       1000.0f / 25.0f  /* 25 Hz -> 40 ms */
#define T_SAMPLE_PUMP           1000.0f / 10.0f  /* 10 Hz -> 100 ms */
#define T_SAMPLE_THETA          1000.0f / 25.0f  /* 25 Hz -> 40 ms */
#define T_SAMPLE_SAFETY         1.0f             /* 1 kHz -> 1 ms */
#define T_SAMPLE_HMI            1000.0f / 2.0f   /* 2 Hz -> 500 ms */
#define T_LOG_INTERVAL          1000UL            /* 1 Hz -> 1000 ms */

/* Dispense/Motion Parameters */
#define THETA_TARGET_RPM        60.0f             /* 1.0 rev/s */
#define THETA_TARGET_RPS        1.0f              /* revolutions per second */
#define DISPENSE_DURATION_MS    5000              /* 5 seconds */
#define TOTAL_FLOW_ML_S         28.3f             /* mL/s */
#define TOTAL_BATTER_ML         141.5f            /* mL total */
#define NUM_NOZZLES             7

/* Temperature Control */
#define TEMP_SETPOINT_MIN_C     210.0f
#define TEMP_SETPOINT_MAX_C     230.0f
#define TEMP_SETPOINT_DEFAULT_C 220.0f
#define TEMP_TOLERANCE_C        3.0f              /* ±3°C */
#define TEMP_SENSOR_RATE_HZ     25                /* PT100 sample rate */

/* Pump Control */
#define PUMP_PWM_FREQ_HZ        10000             /* 10 kHz PWM */
#define PUMP_DUTY_MIN_PCT       0.0f
#define PUMP_DUTY_MAX_PCT       100.0f

/* ============================================================================
 * DATA STRUCTURES
 * ========================================================================= */

typedef struct {
    float setpoint_c;
    float current_c;
    float setpoint_f;
    float current_f;
} thermal_state_t;

typedef struct {
    float kp;
    float ki;
    float kd;
    float integral;
    float previous_error;
} pid_controller_t;

typedef struct {
    float target_rpm;
    float current_rpm;
    float position_revs;
    bool homed;
    bool enabled;
} motion_state_t;

typedef struct {
    float duty_percent;
    float target_flow_ml_s;
    bool enabled;
    uint8_t trim_values[NUM_NOZZLES];  /* 0-255 trim per nozzle */
} pump_state_t;

typedef struct {
    bool estop_pressed;
    bool cover_open;
    bool thermal_cutoff;
    bool pump_fault;
    bool motion_fault;
    uint32_t fault_flags;
} safety_state_t;

typedef struct {
    uint32_t cycle_count;
    uint32_t fault_count;
    uint32_t cip_count;
    uint32_t total_runtime_seconds;
} machine_stats_t;

/* log_entry_t is defined in logging.h to avoid duplicate typedef conflicts across modules */

typedef enum {
    MODE_IDLE = 0,
    MODE_WARMUP = 1,
    MODE_READY = 2,
    MODE_DISPENSE = 3,
    MODE_COOLDOWN = 4,
    MODE_CIP = 5,
    MODE_ERROR = 6,
    MODE_ESTOP = 7
} machine_mode_t;

typedef struct {
    machine_mode_t current_mode;
    machine_mode_t requested_mode;
    thermal_state_t thermal;
    motion_state_t motion;
    pump_state_t pump;
    safety_state_t safety;
    machine_stats_t stats;
} system_state_t;

/* ============================================================================
 * GLOBAL STATE
 * ========================================================================= */

extern system_state_t g_system_state;

/* ============================================================================
 * FUNCTION PROTOTYPES - INITIALIZATION
 * ========================================================================= */

void control_system_init(void);
void control_system_update(void);

/* ============================================================================
 * FUNCTION PROTOTYPES - THERMAL CONTROL
 * ========================================================================= */

void thermal_control_init(void);
void thermal_control_update(void);
float thermal_read_temperature(void);
void thermal_set_setpoint(float temp_c);
void thermal_ssr_set(bool on);

/* ============================================================================
 * FUNCTION PROTOTYPES - MOTION CONTROL
 * ========================================================================= */

void motion_control_init(void);
void motion_control_update(void);
void motion_home(void);
void motion_enable(bool enabled);
void motion_set_target_rpm(float rpm);
void motion_step_pulse(void);
float motion_get_current_rpm(void);
bool motion_is_homed(void);

/* ============================================================================
 * FUNCTION PROTOTYPES - PUMP CONTROL
 * ========================================================================= */

void pump_control_init(void);
void pump_control_update(void);
void pump_enable(bool enabled);
void pump_set_duty_cycle(float percent);
void pump_set_trim(uint8_t nozzle, uint8_t trim_value);

/* ============================================================================
 * FUNCTION PROTOTYPES - SAFETY
 * ========================================================================= */

void safety_init(void);
void safety_monitor(void);
bool safety_all_ok(void);
void safety_estop_scan(void);
void safety_shutdown(void);

/* ============================================================================
 * FUNCTION PROTOTYPES - HMI & LOGGING
 * ========================================================================= */

void hmi_init(void);
void hmi_update(void);
void logging_init(void);
void logging_update(void);
void log_add_entry(void);
void uart_printf(const char *fmt, ...);

#endif /* CONTROL_SYSTEM_H */
