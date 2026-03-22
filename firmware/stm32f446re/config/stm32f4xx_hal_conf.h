/**
 * @file stm32f4xx_hal_conf.h
 * @brief HAL configuration file for STM32F446RE
 */

#ifndef STM32F4XX_HAL_CONF_H
#define STM32F4XX_HAL_CONF_H

#include "stm32f4xx.h"

/* ========================== Module Selection ============================ */

#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_CRC_MODULE_ENABLED
#define HAL_CRYP_MODULE_ENABLED
#define HAL_DAC_MODULE_ENABLED
#define HAL_DCMI_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_DMA2D_MODULE_ENABLED
#define HAL_ETH_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_NAND_MODULE_ENABLED
#define HAL_NOR_MODULE_ENABLED
#define HAL_PCCARD_MODULE_ENABLED
#define HAL_SRAM_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_I2S_MODULE_ENABLED
#define HAL_IWDG_MODULE_ENABLED
#define HAL_LTDC_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_RNG_MODULE_ENABLED
#define HAL_RTC_MODULE_ENABLED
#define HAL_SAI_MODULE_ENABLED
#define HAL_SD_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_SRAM_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_USART_MODULE_ENABLED
#define HAL_IRDA_MODULE_ENABLED
#define HAL_SMARTCARD_MODULE_ENABLED
#define HAL_WWDG_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_PCD_MODULE_ENABLED
#define HAL_HCD_MODULE_ENABLED

/* ======================== HSE Configuration ============================= */

#define HSE_VALUE ((uint32_t) 8000000U)
#define HSE_STARTUP_TIMEOUT ((uint32_t) 100U)

/* ======================== LSE Configuration ============================= */

#define LSE_VALUE ((uint32_t) 32768U)
#define LSE_STARTUP_TIMEOUT ((uint32_t) 5000U)

/* ==================== Internal Clock Selection ========================= */

#define HSI_VALUE ((uint32_t) 16000000U)
#define LSI_VALUE ((uint32_t) 32000U)

/* ======================= Oscillator Configuration ====================== */

#define RCC_OSCILLATORTYPE_HSE  0x00000001U
#define RCC_OSCILLATORTYPE_HSI  0x00000002U
#define RCC_OSCILLATORTYPE_LSE  0x00000004U
#define RCC_OSCILLATORTYPE_LSI  0x00000008U

/* ======================== System Clock Source ========================== */

/* PLL: HSE 8MHz → 180MHz */
#define VECT_TAB_OFFSET 0x00000000U

/* ==================== Timeout Values (ms) ============================== */

#define HSE_STARTUP_TIMEOUT     ((uint32_t) 100U)
#define LSE_STARTUP_TIMEOUT     ((uint32_t) 5000U)

/* ======================== Ethernet Configuration ======================= */

#define MAC_ADDR0   2U
#define MAC_ADDR1   0U
#define MAC_ADDR2   0U
#define MAC_ADDR3   0U
#define MAC_ADDR4   0U
#define MAC_ADDR5   0U

/* ==================== DMA Configuration =============================== */

#define DMA_MAX_NUMBER_OF_CHANNELS    8U

/* ==================== ADC/DAC/Comparator Configuration ================= */

#define ADC_CLOCK_SYNC_PCLK_DIV2    0x00000000U
#define ADC_CLOCK_SYNC_PCLK_DIV4    0x00000004U
#define ADC_CLOCK_SYNC_PCLK_DIV6    0x00000008U
#define ADC_CLOCK_SYNC_PCLK_DIV8    0x0000000CU

#define ADC_PRESCALER_DIV2   ADC_CLOCK_SYNC_PCLK_DIV2
#define ADC_PRESCALER_DIV4   ADC_CLOCK_SYNC_PCLK_DIV4
#define ADC_PRESCALER_DIV6   ADC_CLOCK_SYNC_PCLK_DIV6
#define ADC_PRESCALER_DIV8   ADC_CLOCK_SYNC_PCLK_DIV8

/* ==================== Asserting Macro =================================== */

#define assert_param(expr) ((void)0U)

#endif /* STM32F4XX_HAL_CONF_H */
