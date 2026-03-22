/**
 * @file logging.h
 * @brief Data logging and telemetry module
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define LOG_BUFFER_SIZE         1000
#define LOG_ENTRY_SIZE          sizeof(log_entry_t)

typedef struct {
    uint32_t timestamp_ms;
    float plate_temperature_c;
    float pump_duty_percent;
    float theta_rpm;
    float theta_position_revs;
    bool estop_active;
    bool cover_open;
    bool pump_enabled;
    bool motion_enabled;
    uint8_t machine_mode;
    uint8_t fault_code;
    uint16_t cycle_number;
} log_entry_t;

typedef struct {
    log_entry_t entries[LOG_BUFFER_SIZE];
    uint16_t write_index;
    uint16_t entry_count;
    uint32_t last_log_time_ms;
    bool is_full;
} log_buffer_t;

/* Initialize logging system */
void logging_init(void);

/* Add an entry to the log (internal call from main loop) */
void loggingstroke_add_entry(void);

/* Write log entries to UART (debug/telemetry) */
void logging_transmit_entry(log_entry_t *entry);

/* Dump entire buffer to UART */
void logging_dump_buffer(void);

/* Clear the log buffer */
void logging_clear_buffer(void);

/* Get current buffer status */
uint16_t logging_get_entry_count(void);
bool logging_is_buffer_full(void);

/* Export a specific log entry */
log_entry_t logging_get_entry(uint16_t index);

#endif /* LOGGING_H */
