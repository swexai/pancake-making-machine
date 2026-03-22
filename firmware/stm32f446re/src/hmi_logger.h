/**
 * @file hmi_logger.h
 * @brief HMI (Human–Machine Interface) & data logging subsystem
 */

#ifndef HMI_LOGGER_H
#define HMI_LOGGER_H

#include <stdint.h>
#include <stddef.h>

/* ==================== Type Definitions ================================ */

typedef enum {
    CMD_NONE = 0,
    CMD_START_CYCLE,   /* Begin one pancake cycle */
    CMD_STOP,          /* Stop immediately */
    CMD_HOME_MOTOR,    /* Homing run */
    CMD_SET_TEMP,      /* Set target temperature */
    CMD_QUERY_STATUS,  /* Request system status */
    CMD_RESET_FAULT    /* Clear error state */
} HMI_Command_e;

typedef struct {
    HMI_Command_e cmd;          /* Parsed command from HMI */
    float temp_setpoint;        /* Temperature setpoint (if CMD_SET_TEMP) */
} HMI_Packet_t;

typedef struct {
    /* Thermal state */
    float temperature_deg_c;
    uint16_t ssr_duty;
    
    /* Motion state */
    int32_t motor_position_steps;
    uint8_t motor_is_homed;
    
    /* Pump state */
    uint8_t pump_is_dispensing;
    uint16_t pump_duty;
    
    /* Safety state */
    uint8_t safety_state;
    
    /* Timestamp */
    uint32_t timestamp_ms;
} SystemState_t;

/* ==================== Function Prototypes ============================ */

/**
 * @brief Initialize HMI/Logger (USART2: 115200 baud)
 */
void HMI_Init(void);

/**
 * @brief Parse incoming HMI command from UART RX buffer
 * @param[out] packet: parsed command & parameters
 * @return: 1 if valid command received, 0 if incomplete
 */
uint8_t HMI_ParseCommand(HMI_Packet_t *packet);

/**
 * @brief Print status message to console (UART TX)
 */
void HMI_PrintStatus(SystemState_t *state);

/**
 * @brief Log event to console (e.g., "HOMING START", "DISPENSE COMPLETE")
 */
void HMI_LogEvent(const char *event_str, uint32_t timestamp_ms);

/**
 * @brief Log formatted message (printf-style)
 */
void HMI_Printf(const char *fmt, ...);

/**
 * @brief FreeRTOS task: HMI command handler & logging (1–2 Hz)
 */
void HMI_Task(void *pvParameters);

#endif /* HMI_LOGGER_H */
