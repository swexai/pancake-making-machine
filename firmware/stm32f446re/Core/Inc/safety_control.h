/**
 * @file safety_control.h
 * @brief Safety monitoring and emergency stop control
 */

#ifndef SAFETY_CONTROL_H
#define SAFETY_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

/* Safety input pins (active low for NC switches) */
#define ESTOP_DEBOUNCE_MS       50
#define COVER_DEBOUNCE_MS       100
#define THERMAL_CUTOFF_THRESHOLD_C  250  /* Thermal cutoff setpoint */

typedef enum {
    FAULT_NONE = 0x00,
    FAULT_ESTOP = 0x01,
    FAULT_COVER_OPEN = 0x02,
    FAULT_THERMAL_CUTOFF = 0x04,
    FAULT_SENSOR_LOSS = 0x08,
    FAULT_MOTOR_STALL = 0x10,
    FAULT_PUMP_LOSS = 0x20,
    FAULT_OVERCURRENT = 0x40,
    FAULT_SYSTEM_WATCHDOG = 0x80
} safety_fault_t;

typedef struct {
    bool estop_pressed;
    bool cover_open;
    bool thermal_cutoff_active;
    uint32_t estop_debounce_counter;
    uint32_t cover_debounce_counter;
    uint32_t last_check_ms;
} safety_inputs_t;

typedef struct {
    uint32_t fault_flags;
    bool immediate_shutdown_required;
    uint32_t fault_timestamp_ms;
    char fault_description[64];
} safety_fault_status_t;

void safety_init(void);
void safety_monitor(void);
void safety_estop_check(void);
void safety_cover_check(void);
void safety_thermal_check(void);
bool safety_all_systems_ok(void);
void safety_shutdown(void);
void safety_disable_all_outputs(void);
void safety_set_fault(safety_fault_t fault);
void safety_clear_fault(safety_fault_t fault);
bool safety_has_fault(safety_fault_t fault);
const char* safety_fault_description(safety_fault_t fault);

#endif /* SAFETY_CONTROL_H */
