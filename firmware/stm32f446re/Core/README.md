# STM32F446RE Firmware Build Guide

## Quick Start

### 1. Install STM32CubeMX
- Download from: https://www.st.com/en/development-tools/stm32cubemx.html
- Install on your system (supports Windows, macOS, Linux)

### 2. Generate IOC Project

Use the **IOC_CONFIGURATION_GUIDE.md** in this directory for step-by-step instructions on:
- Creating a new STM32F446RE project in STM32CubeMX
- Configuring all GPIO, timers, SPI, UART, and FreeRTOS settings
- Generating HAL code

### 3. Project Structure

```
stm32f446re/
├── IOC_CONFIGURATION_GUIDE.md     ← Read this first!
├── config/
│   ├── stm32f4xx_hal_conf.h       ← HAL configuration
│   ├── FreeRTOSConfig.h           ← FreeRTOS kernel config
│   └── pancake-making-machine.ioc ← STM32CubeMX project (generate)
├── hal/
│   └── [Generated HAL files from STM32CubeMX]
├── src/
│   ├── main.c                     ← Application entry point
│   ├── thermal_control.h/.c       ← Temperature control task
│   ├── motion_control.h/.c        ← Stepper motor control task
│   ├── pump_control.h/.c          ← Pump/dispense control task
│   ├── safety.h/.c                ← Safety monitoring task
│   └── hmi_logger.h/.c            ← Console I/O & logging task
└── CMakeLists.txt or Makefile    ← Build configuration
```

### 4. Clone or Copy Generated Code

After STM32CubeMX generates code:
1. Place the `.ioc` file in `config/`
2. Copy HAL driver files (`.c` and `.h`) into `hal/`
3. STM32CubeMX may generate a `Src/` folder—merge it with the skeleton code in `src/`

### 5. Build with STM32CubeIDE

- Open STM32CubeIDE → **File → Open Projects from File System**
- Import the generated project folder
- **Project → Build Project**

**Or** build with command-line tools (ARM GCC):
```bash
arm-none-eabi-gcc -mcpu=cortex-m4 -O2 -c src/main.c
# ... (see Makefile for full build)
```

### 6. Flash to STM32F446RE

Using STM32CubeProgrammer or OpenOCD:
```bash
openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg \
  -c "program build/pancake-making-machine.elf verify reset"
```

Or use ST-Link utility in STM32CubeIDE → **Run → Debug**.

---

## Peripheral Mapping Reference

For quick lookup, see **IOC_CONFIGURATION_GUIDE.md** Section 2 (Pin Mapping).

- **Safety Inputs**: PA0 (E-STOP), PA1 (Cover)
- **Thermal**: SPI1 (PA5,PA7,PA6) + PA4 (CS_MAX31865) + PA8 (SSR PWM)
- **Motion**: PB3 (STEP), PB4 (DIR), PB5 (EN)
- **Pump**: PB6 (PWM)
- **Debug Console**: USART2 (PA2 TX, PA3 RX)

---

## Task Priorities & Rates

| Task | Frequency | Priority | Notes |
|------|-----------|----------|-------|
| Safety | 1 kHz | 5 (High) | E-STOP, cover, fault detection |
| Thermal | 25 Hz | 4 | PID control, reads MAX31865 |
| Motion | ~1 kHz timer | 3 | Step generation (TIM2) |
| Pump | 10 Hz | 2 | PWM control |
| HMI | 1 Hz | 1 (Low) | Console logging |

---

## Example: Minimal main.c Targets

The provided `src/main.c` includes:
- HAL initialization (GPIO, SPI, timers, UART)
- FreeRTOS task creation with appropriate priorities
- System clock @ 180 MHz
- Hooks for stack overflow & malloc failures

### To verify after build:
1. **LED blink test** (add a heartbeat LED task)
2. **UART console** (connect serial monitor @ 115200 baud)
3. **MAX31865 SPI read** (verify temperature sensor communication)
4. **PWM verification** (oscilloscope check: SSR @ 10 Hz, pump @ 10 Hz)

---

## Next Steps

### Phase 1: HAL Integration
- [ ] Run STM32CubeMX to generate IO code
- [ ] Build & flash a minimal test (LED toggle, UART "Hello")
- [ ] Verify SPI communication with MAX31865

### Phase 2: Implement Task Stubs
- [ ] Write `thermal_control.c` (SPI read, PID algorithm)
- [ ] Write `motion_control.c` (timer ISR, step counting)
- [ ] Write `pump_control.c` (TIM4 PWM control)
- [ ] Write `safety.c` (GPIO polling, de-energization)
- [ ] Write `hmi_logger.c` (UART command parsing, logging)

### Phase 3: Integration & Testing
- [ ] Validate thermal loop with PT100 waveforms
- [ ] Test motion homing & speed ramp
- [ ] Validate pump dispense (5 s × ~28.3 mL/s = ~141 mL)
- [ ] Functional E-STOP test

---

## Useful References

- **STM32F446RE Datasheet**: [PDF](https://www.st.com/resource/en/datasheet/stm32f446re.pdf)
- **STM32F4 HAL User Manual**: [PDF](https://www.st.com/content/ccc/resource/technical/document/user_manual/group0/12/5e/33/77/f7/00/43/59/DM00135183/files/DM00135183.pdf)
- **FreeRTOS Kernel**: [Website](https://www.freertos.org/)
- **MAX31865 Datasheet**: [PDF](https://datasheets.maximintegrated.com/en/ds/MAX31865.pdf)

---

## Support

If you encounter issues:
1. Check the **IOC_CONFIGURATION_GUIDE.md** for correct CubeMX settings
2. Verify pin assignments match the schematic
3. Enable HAL debug output via `HAL_UART_Transmit()`
4. Use an oscilloscope to verify PWM frequencies and duty cycles

