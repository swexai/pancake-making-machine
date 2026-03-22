/**
 * @file hmi.c
 * @brief Human-Machine Interface and serial communication
 */

#include "main.h"
#include "hmi.h"
#include "control_system.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* ============================================================================
 * PRIVATE VARIABLES
 * ========================================================================= */

static uart_buffer_t g_uart = {0};

/* ============================================================================
 * PRIVATE FUNCTIONS
 * ========================================================================= */

/**
 * @brief Initialize UART peripheral for HMI
 */
static void uart_init(void)
{
    extern UART_HandleTypeDef huart3;
    HAL_UART_Receive_IT(&huart3, g_uart.rx_buffer, 1);
}

/**
 * @brief Transmit data via UART
 */
static void uart_tx(const uint8_t *data, uint16_t length)
{
    extern UART_HandleTypeDef huart3;
    HAL_UART_Transmit(&huart3, (uint8_t*)data, length, 100);
}

/* ============================================================================
 * FUNCTION IMPLEMENTATIONS
 * ========================================================================= */

/**
 * @brief Initialize HMI module
 */
void hmi_init(void)
{
    memset(&g_uart, 0, sizeof(g_uart));
    uart_init();
    
    /* Send startup message */
    uart_printf("\r\n=== Injera Machine Control v1.0 ===\r\n");
    uart_printf("Ready for commands\r\n");
}

/**
 * @brief Update HMI (process pending commands)
 * Called at 2 Hz
 */
void hmi_update(void)
{
    if (g_uart.command_ready) {
        hmi_process_command(g_uart.rx_buffer, g_uart.rx_index);
        g_uart.command_ready = false;
        g_uart.rx_index = 0;
    }
}

/**
 * @brief UART RX callback (called from ISR)
 * @param byte Received byte
 */
void uart_rx_callback(uint8_t byte)
{
    if (byte == '\r' || byte == '\n') {
        /* Command complete */
        g_uart.command_ready = true;
    } else if (g_uart.rx_index < UART_RX_BUFFER_SIZE - 1) {
        g_uart.rx_buffer[g_uart.rx_index++] = byte;
    }
}

/**
 * @brief Send system status via UART
 */
void hmi_send_status(void)
{
    const char *mode_str[] = {
        "IDLE", "WARMUP", "READY", "DISPENSE", "COOLDOWN", "CIP", "ERROR", "ESTOP"
    };
    
    uart_printf("\r\n--- System Status ---\r\n");
    uart_printf("Mode: %s\r\n", mode_str[g_system_state.current_mode]);
    uart_printf("Temp: %.1f C (setpoint: %.1f C)\r\n",
        g_system_state.thermal.current_c,
        g_system_state.thermal.setpoint_c);
    uart_printf("Pump: %s (duty: %.0f%%)\r\n",
        g_system_state.pump.enabled ? "ON" : "OFF",
        g_system_state.pump.duty_percent);
    uart_printf("Theta: %s (RPM: %.1f)\r\n",
        g_system_state.motion.enabled ? "ON" : "OFF",
        g_system_state.motion.current_rpm);
    uart_printf("E-Stop: %s, Cover: %s\r\n",
        g_system_state.safety.estop_pressed ? "PRESSED" : "OK",
        g_system_state.safety.cover_open ? "OPEN" : "CLOSED");
    uart_printf("Cycles: %lu\r\n", g_system_state.stats.cycle_count);
}

/**
 * @brief Send temperature reading
 * @param temp_c Temperature in Celsius
 */
void hmi_send_temperature(float temp_c)
{
    uart_printf("Temp: %.1f C\r\n", temp_c);
}

/**
 * @brief Send cycle statistics
 */
void hmi_send_statistics(void)
{
    uart_printf("\r\n--- Statistics ---\r\n");
    uart_printf("Total cycles: %lu\r\n", g_system_state.stats.cycle_count);
    uart_printf("Faults: %lu\r\n", g_system_state.stats.fault_count);
    uart_printf("CIP runs: %lu\r\n", g_system_state.stats.cip_count);
    uart_printf("Runtime: %lu seconds\r\n", g_system_state.stats.total_runtime_seconds);
}

/**
 * @brief Parse and execute HMI command
 * @param buffer Command buffer
 * @param length Command length
 */
void hmi_process_command(uint8_t *buffer, uint16_t length)
{
    if (length == 0) {
        return;
    }
    
    char cmd = buffer[0];
    
    switch (cmd) {
        case 'S':  /* Start cycle */
        case 's':
            uart_printf("Starting cycle...\r\n");
            g_system_state.requested_mode = MODE_DISPENSE;
            break;
            
        case 'H':  /* Home theta axis */
        case 'h':
            uart_printf("Homing theta axis...\r\n");
            motion_home_axis();
            break;
            
        case 'T':  /* Set temperature (format: T220) */
        case 't':
            if (length >= 4) {
                float temp = (float)((buffer[1] - '0') * 100 + 
                                     (buffer[2] - '0') * 10 + 
                                     (buffer[3] - '0'));
                if (temp >= TEMP_SETPOINT_MIN_C && temp <= TEMP_SETPOINT_MAX_C) {
                    g_system_state.thermal.setpoint_c = temp;
                    uart_printf("Setpoint: %.0f C\r\n", temp);
                } else {
                    uart_printf("Temp out of range [210-230]\r\n");
                }
            }
            break;
            
        case 'P':  /* Set pump duty (format: P75) */
        case 'p':
            if (length >= 3) {
                float duty = (float)((buffer[1] - '0') * 10 + (buffer[2] - '0'));
                pump_set_duty_cycle(duty);
                uart_printf("Pump duty: %.0f%%\r\n", duty);
            }
            break;
            
        case 'C':  /* CIP mode */
        case 'c':
            uart_printf("Starting CIP mode...\r\n");
            g_system_state.requested_mode = MODE_CIP;
            break;
            
        case 'E':  /* Emergency stop */
        case 'e':
            uart_printf("EMERGENCY STOP\r\n");
            g_system_state.requested_mode = MODE_ESTOP;
            safety_shutdown();
            break;
            
        case 'Q':  /* Query status */
        case 'q':
            hmi_send_status();
            break;
            
        case 'L':  /* Dump logs */
        case 'l':
            logging_dump_buffer();
            break;
            
        case 'A':  /* Show statistics */
        case 'a':
            hmi_send_statistics();
            break;
            
        case '?':  /* Help */
            uart_printf("\r\n--- Available Commands ---\r\n");
            uart_printf("S - Start cycle\r\n");
            uart_printf("H - Home theta axis\r\n");
            uart_printf("Txxx - Set temp (e.g., T220)\r\n");
            uart_printf("Pxx - Set pump duty (e.g., P75)\r\n");
            uart_printf("C - Start CIP mode\r\n");
            uart_printf("E - Emergency stop\r\n");
            uart_printf("Q - Query status\r\n");
            uart_printf("L - Dump logs\r\n");
            uart_printf("A - Show statistics\r\n");
            uart_printf("? - This help\r\n");
            break;
            
        default:
            uart_printf("Unknown command: %c\r\n", cmd);
            uart_printf("Type '?' for help\r\n");
    }
}

/**
 * @brief UART printf function
 * @param format Printf-style format string
 */
void uart_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    
    char buffer[256];
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (length > 0) {
        uart_tx((uint8_t*)buffer, length);
    }
}
