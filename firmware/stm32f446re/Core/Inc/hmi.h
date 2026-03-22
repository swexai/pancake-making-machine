/**
 * @file hmi.h
 * @brief Human-Machine Interface and serial communication
 */

#ifndef HMI_H
#define HMI_H

#include <stdint.h>
#include <stdbool.h>

#define UART_RX_BUFFER_SIZE     128
#define UART_TX_BUFFER_SIZE     256

typedef enum {
    CMD_START = 0x01,
    CMD_STOP = 0x02,
    CMD_EMERGENCY_STOP = 0x03,
    CMD_SET_TEMP = 0x04,
    CMD_READ_STATUS = 0x05,
    CMD_CIP_MODE = 0x06,
    CMD_HOME = 0x07,
    CMD_TRIM_NOZZLE = 0x08,
    CMD_GET_LOGS = 0x09,
    CMD_RESET_STATS = 0x0A
} hmi_command_t;

typedef struct {
    uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
    uint16_t rx_index;
    uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
    uint16_t tx_index;
    bool command_ready;
    hmi_command_t last_command;
} uart_buffer_t;

/* HMI initialization */
void hmi_init(void);

/* Update HMI state */
void hmi_update(void);

/* UART interrupt handler (called from HAL) */
void uart_rx_callback(uint8_t byte);

/* Send status string via UART */
void hmi_send_status(void);

/* Send temperature value */
void hmi_send_temperature(float temp_c);

/* Send cycle statistics */
void hmi_send_statistics(void);

/* Parse and execute HMI commands */
void hmi_process_command(uint8_t *buffer, uint16_t length);

/* Simple UART printf */
void uart_printf(const char *format, ...);

#endif /* HMI_H */
