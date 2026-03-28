/**
 * @file logging.c
 * @brief Data logging and telemetry implementation
 */

#include "main.h"
#include "logging.h"
#include "control_system.h"
#include <string.h>
#include <stdio.h>

/* ============================================================================
 * PRIVATE VARIABLES
 * ========================================================================= */

static log_buffer_t g_log_buffer = {0};
static uint32_t g_last_log_ms = 0;

/* ============================================================================
 * FUNCTION IMPLEMENTATIONS
 * ========================================================================= */

/**
 * @brief Initialize logging system
 */
void logging_init(void)
{
    memset(&g_log_buffer, 0, sizeof(g_log_buffer));
    g_log_buffer.write_index = 0;
    g_log_buffer.entry_count = 0;
    g_log_buffer.is_full = false;
    g_last_log_ms = 0;
}

/**
 * @brief Add current system state to log
 * Called at 1 Hz intervals
 */
void logging_add_entry(void)
{
    uint32_t now_ms = HAL_GetTick();
    
    /* Check if enough time has passed for next log entry */
    if ((now_ms - g_last_log_ms) < T_LOG_INTERVAL) {
        return;
    }
    g_last_log_ms = now_ms;
    
    /* Create log entry with current system state */
    log_entry_t *entry = &g_log_buffer.entries[g_log_buffer.write_index];
    
    entry->timestamp_ms = now_ms;
    entry->plate_temperature_c = g_system_state.thermal.current_c;
    entry->pump_duty_percent = g_system_state.pump.duty_percent;
    entry->theta_rpm = g_system_state.motion.current_rpm;
    entry->theta_position_revs = g_system_state.motion.position_revs;
    entry->estop_active = g_system_state.safety.estop_pressed;
    entry->cover_open = g_system_state.safety.cover_open;
    entry->pump_enabled = g_system_state.pump.enabled;
    entry->motion_enabled = g_system_state.motion.enabled;
    entry->machine_mode = (uint8_t)g_system_state.current_mode;
    entry->fault_code = 0;  /* Could encode fault_flags here */
    entry->cycle_number = g_system_state.stats.cycle_count;
    
    /* Advance write pointer */
    g_log_buffer.write_index++;
    if (g_log_buffer.write_index >= LOG_BUFFER_SIZE) {
        g_log_buffer.write_index = 0;
        g_log_buffer.is_full = true;
    }
    
    /* Update entry count */
    if (!g_log_buffer.is_full) {
        g_log_buffer.entry_count++;
    }
}

/**
 * @brief Transmit a log entry via UART
 * @param entry Pointer to log entry to transmit
 */
void logging_transmit_entry(log_entry_t *entry)
{
    extern UART_HandleTypeDef huart2;
    
    char buffer[128];
    uint16_t length = snprintf(buffer, sizeof(buffer),
        "[%06lu] T=%6.1fC P=%3.0f%% RPM=%6.1f E=%d C=%d M\r\n",
        entry->timestamp_ms,
        entry->plate_temperature_c,
        entry->pump_duty_percent,
        entry->theta_rpm,
        entry->estop_active,
        entry->cover_open
    );
    
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, length, 100);
}

/**
 * @brief Dump entire log buffer to UART
 */
void logging_dump_buffer(void)
{
    extern UART_HandleTypeDef huart2;
    
    char header[] = "=== LOG DUMP ===\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)header, strlen(header), 100);
    
    uint16_t count = g_log_buffer.is_full ? LOG_BUFFER_SIZE : g_log_buffer.entry_count;
    
    for (uint16_t i = 0; i < count; i++) {
        logging_transmit_entry(&g_log_buffer.entries[i]);
    }
    
    char footer[] = "=== END LOG ===\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)footer, strlen(footer), 100);
}

/**
 * @brief Clear the log buffer
 */
void logging_clear_buffer(void)
{
    memset(&g_log_buffer, 0, sizeof(g_log_buffer));
    g_log_buffer.write_index = 0;
    g_log_buffer.entry_count = 0;
    g_log_buffer.is_full = false;
}

/**
 * @brief Get current buffer entry count
 * @return Number of entries in buffer
 */
uint16_t logging_get_entry_count(void)
{
    return g_log_buffer.entry_count;
}

/**
 * @brief Check if log buffer is full
 * @return true if buffer is full, false otherwise
 */
bool logging_is_buffer_full(void)
{
    return g_log_buffer.is_full;
}

/**
 * @brief Get a specific log entry
 * @param index Entry index
 * @return Log entry (or zero-filled if index out of range)
 */
log_entry_t logging_get_entry(uint16_t index)
{
    log_entry_t entry = {0};
    
    if (index < LOG_BUFFER_SIZE) {
        entry = g_log_buffer.entries[index];
    }
    
    return entry;
}
