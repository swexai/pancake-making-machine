/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Minimal UART HAL compatibility stubs (UART module source/header not present) */
#ifndef UART_HandleTypeDef
typedef struct {
    void *Instance;
    struct {
        uint32_t BaudRate;
        uint32_t WordLength;
        uint32_t StopBits;
        uint32_t Parity;
        uint32_t Mode;
        uint32_t HwFlowCtl;
        uint32_t OverSampling;
    } Init;
} UART_HandleTypeDef;
#endif

#ifndef UART_WORDLENGTH_8B
#define UART_WORDLENGTH_8B      0x00000000U
#define UART_STOPBITS_1         0x00000000U
#define UART_PARITY_NONE        0x00000000U
#define UART_MODE_TX_RX         0x00000000U
#define UART_HWCONTROL_NONE     0x00000000U
#define UART_OVERSAMPLING_16    0x00000000U
#endif

static inline HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart) { (void)huart; return HAL_OK; }
static inline HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout) { (void)huart; (void)pData; (void)Size; (void)Timeout; return HAL_OK; }
static inline HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size) { (void)huart; (void)pData; (void)Size; return HAL_OK; }

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ESTOP_Pin GPIO_PIN_0
#define ESTOP_GPIO_Port GPIOA
#define NC_Switch_Pin GPIO_PIN_1
#define NC_Switch_GPIO_Port GPIOA
#define UART_TX_Pin GPIO_PIN_2
#define UART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define SPI1_CLK_Pin GPIO_PIN_5
#define SPI1_CLK_GPIO_Port GPIOA
#define DIR_Pin GPIO_PIN_4
#define DIR_GPIO_Port GPIOB
#define EN_THETA_Pin GPIO_PIN_5
#define EN_THETA_GPIO_Port GPIOB
#define PWM_PUMP_Pin GPIO_PIN_6
#define PWM_PUMP_GPIO_Port GPIOB
#define Power_Indicator_Pin GPIO_PIN_7
#define Power_Indicator_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* Custom utility functions */
uint32_t HAL_GetMicrosecond(void);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
