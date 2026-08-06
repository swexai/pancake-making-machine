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
#define COVER_SW_Pin GPIO_PIN_1
#define COVER_SW_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define SPI1_CLK_Pin GPIO_PIN_5
#define SPI1_CLK_GPIO_Port GPIOA
#define START_BTN_Pin GPIO_PIN_4
#define START_BTN_GPIO_Port GPIOC
#define STOP_BTN_Pin GPIO_PIN_5
#define STOP_BTN_GPIO_Port GPIOC
#define HOME_THETA_Pin GPIO_PIN_6
#define HOME_THETA_GPIO_Port GPIOC
#define HOME_RADIAL_Pin GPIO_PIN_7
#define HOME_RADIAL_GPIO_Port GPIOC
#define RADIAL_STEP_Pin GPIO_PIN_8
#define RADIAL_STEP_GPIO_Port GPIOA
#define BATTER_LOW_Pin GPIO_PIN_10
#define BATTER_LOW_GPIO_Port GPIOC
#define SPARE_DI_Pin GPIO_PIN_11
#define SPARE_DI_GPIO_Port GPIOC
#define THETA_STEP_Pin GPIO_PIN_3
#define THETA_STEP_GPIO_Port GPIOB
#define THETA_DIR_Pin GPIO_PIN_4
#define THETA_DIR_GPIO_Port GPIOB
#define THETA_EN_Pin GPIO_PIN_5
#define THETA_EN_GPIO_Port GPIOB
#define PWM_PUMP_Pin GPIO_PIN_6
#define PWM_PUMP_GPIO_Port GPIOB
#define Power_Indicator_Pin GPIO_PIN_7
#define Power_Indicator_GPIO_Port GPIOB
#define RADIAL_DIR_Pin GPIO_PIN_8
#define RADIAL_DIR_GPIO_Port GPIOB
#define RADIAL_EN_Pin GPIO_PIN_9
#define RADIAL_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* Simulation mode for testing without hardware */
#define SIMULATION_MODE 1  /* Set to 1 for simulation, 0 for real hardware */

/* Custom utility functions */
uint32_t HAL_GetMicrosecond(void);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
