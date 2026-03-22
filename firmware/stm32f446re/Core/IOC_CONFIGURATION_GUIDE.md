# STM32F446RE IOC Configuration Guide

## Overview
STM32CubeMX project for the Pancake-Making Machine control system. This guide documents all peripheral configurations required.

---

## 1. Project Setup in STM32CubeMX

### 1.1 Create New Project
- **Microcontroller**: STM32F446RETx
- **Target Toolchain**: STM32CubeIDE
- **Firmware Package**: STM32F4 version 1.27.0 (or latest)
- **Project Name**: `pancake-making-machine`
- **Project Location**: `firmware/stm32f446re`

### 1.2 Core Clock Configuration
- **System Clock**: 180 MHz (HSE: 8 MHz, PLL)
- **APB1**: 45 MHz (Prescaler: /4)
- **APB2**: 90 MHz (Prescaler: /2)
- **AHB**: 180 MHz (Prescaler: /1)

---

## 2. Pin Mapping & GPIO

### 2.1 Safety & Control
| Function | Pin | Port | Mode | Notes |
|----------|-----|------|------|-------|
| E-STOP CH1 | PA0 | A | GPIO Input (Pull-up) | Safety chain input |
| Cover NC Switch | PA1 | A | GPIO Input (Pull-up) | Cover open detection |
| MCB Indicator | PA2 | A | GPIO Output | Power available indicator |

### 2.2 Thermal Control (PT100 + MAX31865)
| Function | Pin | Port | Mode | Notes |
|----------|-----|------|------|-------|
| SPI1_CLK | PA5 | A | AF5 (SPI1) | 90 MHz |
| SPI1_MOSI | PA7 | A | AF5 (SPI1) | 90 MHz |
| SPI1_MISO | PA6 | A | AF5 (SPI1) | 90 MHz |
| CS_MAX31865 | PA4 | A | GPIO Output (Push-pull) | Chip Select |
| SSR_HEAT (PWM) | PA8 | A | AF1 (TIM1_CH1) | 10 Hz, ~50% duty cycle |

### 2.3 Motion Control (Stepper: NEMA23 + TB6600)
| Function | Pin | Port | Mode | Notes |
|----------|-----|------|------|-------|
| STEP | PB3 | B | GPIO Output | Timer-driven (TIM2) |
| DIR | PB4 | B | GPIO Output | Direction bit |
| EN_THETA | PB5 | B | GPIO Output | Enable (hardware safety hard-cuts) |

### 2.4 Dispense Control (Pump PWM)
| Function | Pin | Port | Mode | Notes |
|----------|-----|------|------|-------|
| PWM_PUMP | PB6 | B | AF2 (TIM4_CH1) | 10 Hz, variable duty |

### 2.5 UART (HMI / Logger / Debug)
| Function | Pin | Port | Mode | Notes |
|----------|-----|------|------|-------|
| USART2_TX | PA2 | A | AF7 (USART2) | Console output |
| USART2_RX | PA3 | A | AF7 (USART2) | Console input |
| Baud: 115200 | — | — | — | 8N1, no flow control |

---

## 3. Peripheral Configuration Details

### 3.1 SPI1 (MAX31865 Communication)
- **Mode**: Master, Full-Duplex
- **Data Size**: 8-bit
- **Clock Polarity**: CPOL=1 (Clock idles HIGH)
- **Clock Phase**: CPHA=1 (Sample on trailing edge)
- **NSS**: Software managed (using CS_MAX31865 GPIO)
- **Prescaler**: /4 (90 MHz / 4 = 22.5 MHz) — MAX31865 supports up to 5 MHz, use /16 for 5.625 MHz if needed
- **First Bit**: MSB first
- **CRC**: Disabled

### 3.2 TIM1 (SSR PWM at 10 Hz)
- **Mode**: PWM Generation on CH1 (PA8)
- **Prescaler**: 9000 - 1 (180 MHz / 9000 = 20 kHz internal)
- **Period (ARR)**: 1999 (20 kHz / 2000 = 10 Hz output)
- **Pulse (CCR1)**: 1000 (~50% duty) — adjustable for temperature control
- **Output Compare**: PWM mode 1, Preload enabled
- **Interrupts**: Disabled (polling only)

### 3.3 TIM2 (Stepper STEP Pulse Generation)
- **Mode**: Output Compare, Toggle on match
- **Prescaler**: 0 (use 180 MHz directly)
- **Period (ARR)**: 90000 (1 kHz STEP frequency ÷ 2 = 500 Hz max, ~0.2 rev/s for NEMA23)
- **Pulse (CCR1)**: 45000 (toggle=50% duty)
- **Output**: PB3 (STEP signal)
- **Update Interrupt**: Enabled (1 kHz for step counting)

### 3.4 TIM4 (Pump PWM at 10 Hz)
- **Mode**: PWM Generation on CH1 (PB6)
- **Prescaler**: 9000 - 1 (180 MHz / 9000 = 20 kHz internal)
- **Period (ARR)**: 1999 (20 kHz / 2000 = 10 Hz output)
- **Pulse (CCR1)**: 500–1500 (variable duty 25–75% for flow control)
- **Output Compare**: PWM mode 1, Preload enabled

### 3.5 USART2 (Debug / Console)
- **Mode**: Asynchronous
- **Baud Rate**: 115200
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None
- **Flow Control**: None
- **Interrupts**: RX interrupt enabled (optional for background input)

---

## 4. NVIC (Interrupt Priorities)

| Interrupt | Priority | SubPriority | Notes |
|-----------|----------|-------------|-------|
| TIM2_Update | 1 (High) | 0 | Stepper pulse generation |
| USART2_RX | 4 (Low) | 0 | Console input (FreeRTOS compatible) |
| SysTick | 15 (Lowest) | 0 | FreeRTOS scheduler tick |

---

## 5. Code Generation Settings

### 5.1 Project Options
- **IDE**: STM32CubeIDE
- **Toolchain**: Arm GNU Toolchain
- **Generate peripheral initialization as function calls**: ✓

### 5.2 HAL Configuration
- **Use LL drivers**: Uncheck (use HAL only)
- **Copy all used library files into project**: ✓
- **Generate .ioc file for SW4STM32**: Optional

### 5.3 Target Language & Framework
- **Language**: C
- **Use FreeRTOS Kernel**: ✓
  - **Kernel Version**: v10.4.x (or latest)
  - **Memory Scheme**: 1 (first fit)
  - **TOTAL_HEAP_SIZE**: 20480 bytes (20 kB)

---

## 6. Expected Generated Files

After code generation, you should have:

```
stm32f446re/
├── config/
│   ├── pancake-making-machine.ioc          [CubeMX project file]
│   ├── stm32f4xx_hal_conf.h                [HAL config header]
│   └── FreeRTOSConfig.h                    [FreeRTOS config]
├── hal/
│   ├── stm32f4xx_hal.c / .h
│   ├── stm32f4xx_hal_gpio.c / .h
│   ├── stm32f4xx_hal_spi.c / .h
│   ├── stm32f4xx_hal_uart.c / .h
│   ├── stm32f4xx_hal_tim.c / .h
│   ├── stm32f4xx_it.c / .h                 [Interrupt handlers]
│   └── system_stm32f4xx.c
├── src/
│   ├── main.c                              [Entry point — replace with FreeRTOS tasks]
│   ├── freertos.c                          [FreeRTOS initialization]
│   ├── thermal_control.c / .h
│   ├── motion_control.c / .h
│   ├── pump_control.c / .h
│   ├── safety.c / .h
│   └── hmi_logger.c / .h
└── CMakeLists.txt or Makefile              [Build system]
```

---

## 7. Step-by-Step in STM32CubeMX

1. **New Project** → Select "STM32F446RETx"
2. **System Core** tab:
   - RCC: Set HSE to 8 MHz (external oscillator)
   - RCC: Set LDO or SMPS (power regulation)
   - SYS: Set Debug to ST-Link (SWD)
3. **Clock Configuration** tab:
   - PLLCLK = 180 MHz (8 MHz × 45 ÷ 2)
4. **Connectivity** tab:
   - Add **SPI1** → Full-duplex Master → PA5,PA7,PA6 (MOSI,CLK,MISO)
   - Add **USART2** → PA2 (TX), PA3 (RX)
5. **Timer** tab:
   - Add **TIM1** for SSR PWM (PA8)
   - Add **TIM2** for stepper pulse
   - Add **TIM4** for pump PWM (PB6)
6. **GPIO** tab:
   - Configure PA4 as GPIO output (CS_MAX31865)
   - Configure PB3,PB4,PB5 as GPIO outputs (STEP, DIR, EN)
   - Configure PA0,PA1 as GPIO inputs with pull-up (safety)
7. **Middleware** tab:
   - Add **FreeRTOS Kernel** (CMSIS_V2 API)
   - Set task stack sizes (e.g., 512 words each)
8. **Project Manager** tab:
   - Target Project Structure: Create `.c/.h` pair for each peripheral
   - Generate Code

---

## 8. Verification Checklist

After generating code:

- [ ] `main()` initializes all HAL modules (GPIO, SPI, TIMERS, UART)
- [ ] ISRs in `stm32f4xx_it.c` call appropriate HAL handlers
- [ ] FreeRTOS scheduler starts in `main()` → `osKernelStart()`
- [ ] Example tasks compile and run (e.g., toggle LED at 1 Hz)
- [ ] SPI communication with MAX31865 can be tested with dummy reads
- [ ] UART output appears on serial monitor (115200 baud)
- [ ] Timers generate expected PWM frequencies (10 Hz for SSR/pump verified with oscilloscope)

---

## 9. Common Adjustments Post-Generation

**Disable unused HAL modules** in `stm32f4xx_hal_conf.h`:
```c
#define HAL_RNG_MODULE_ENABLED      0
#define HAL_CAN_MODULE_ENABLED      0
#define HAL_QSPI_MODULE_ENABLED     0
```

**Increase TOTAL_HEAP_SIZE** in `FreeRTOSConfig.h` if tasks fail to create:
```c
#define configTOTAL_HEAP_SIZE  ( 20 * 1024 ) // 20 kB
```

**Replace `main()` loop** with FreeRTOS task creation—see `src/freertos.c` generated stub.

---

## 10. Next Steps

1. Generate the .ioc project file using STM32CubeMX
2. Generate HAL code (STM32CubeIDE compatible)
3. Import project into STM32CubeIDE
4. Create application tasks in `src/` for:
   - `thermal_control_task()` — reads MAX31865, controls SSR (25 Hz)
   - `motion_control_task()` — stepper control, homing (1 kHz STEP)
   - `pump_control_task()` — PWM dispense (10 Hz)
   - `safety_task()` — monitors E-STOP, cover (1 kHz)
   - `hmi_logger_task()` — UART logging (1 Hz)
5. Build & flash onto STM32F446RE board

---

## References

- [STM32F446RE Datasheet](https://www.st.com/resource/en/datasheet/stm32f446re.pdf)
- [STM32F4 HAL User Manual](https://www.st.com/content/ccc/resource/technical/document/user_manual/group0/12/5e/33/77/f7/00/43/59/DM00135183/files/DM00135183.pdf)
- [MAX31865 Datasheet](https://datasheets.maximintegrated.com/en/ds/MAX31865.pdf)
- [FreeRTOS STM32 Integration Guide](https://www.freertos.org/STM32F1xx_STM32F4xx_Port.html)

