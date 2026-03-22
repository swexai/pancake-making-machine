# Build and Integration Guide

## Project Dependencies

### Required Components
1. **STM32CubeMX Project**
   - ConfiguRE peripherals: GPIO, SPI1 (MAX31865), TIM1/TIM2, UART3
   - Generate HAL code with USER CODE sections preserved

2. **FreeRTOS Kernel**
   - Version: 10.0+ (compatible with STM32 HAL)
   - Files needed:
     - `FreeRTOS/tasks.c`
     - `FreeRTOS/queue.c`
     - `FreeRTOS/list.c`
     - `FreeRTOS/timers.c`
     - `FreeRTOS/portable/MemMang/heap_4.c`
     - `FreeRTOS/portable/GCC/ARM_CM4F/port.c`

3. **ST HAL Drivers**
   - GPIO, SPI, UART, Timer (already in STM32 project)

### Optional but Recommended
- STM32F4 HAL Library documentation
- CubeMX IDE for device configuration
- ST-Link debugger
- Serial terminal (for HMI commands)

## Project Organization

```
firmware/
├── stm32f446re/
│   ├── pancake-making-machine.ioc    # CubeMX project file
│   ├── CMakeLists.txt                # CMake build configuration
│   │
│   ├── Core/
│   │   ├── Inc/
│   │   │   ├── main.h
│   │   │   ├── stm32f4xx_hal_conf.h
│   │   │   ├── stm32f4xx_it.h
│   │   │   │
│   │   │   ├── control_system.h      ← NEW
│   │   │   ├── thermal_control.h     ← NEW
│   │   │   ├── motion_control.h      ← NEW
│   │   │   ├── pump_control.h        ← NEW
│   │   │   ├── safety_control.h      ← NEW
│   │   │   ├── hmi.h                 ← NEW
│   │   │   ├── logging.h             ← NEW
│   │   │   └── FreeRTOSConfig.h      ← NEW
│   │   │
│   │   └── Src/
│   │       ├── main.c                ← MODIFIED
│   │       ├── stm32f4xx_hal_msp.c
│   │       ├── stm32f4xx_it.c
│   │       │
│   │       ├── control_system.c      ← NEW
│   │       ├── thermal_control.c     ← NEW
│   │       ├── motion_control.c      ← NEW
│   │       ├── pump_control.c        ← NEW
│   │       ├── safety_control.c      ← NEW
│   │       ├── hmi.c                 ← NEW
│   │       └── logging.c             ← NEW
│   │
│   ├── Drivers/
│   │   ├── CMSIS/
│   │   │   └── Include/     # ARM core headers
│   │   └── STM32F4xx_HAL_Driver/
│   │       └── Inc/, Src/
│   │
│   └── FreeRTOS/            ← To be added
│       ├── include/
│       ├── portable/
│       └── source/
│
└── CONTROL_SOFTWARE.md      # This documentation
```

## Build Instructions

### Using CMake (Recommended)

1. **Create build directory:**
   ```bash
   cd firmware/stm32f446re
   mkdir build && cd build
   ```

2. **Run CMake:**
   ```bash
   cmake .. -DCMAKE_TOOLCHAIN_FILE=arm-none-eabi.cmake
   ```

3. **Build:**
   ```bash
   make -j4
   ```

### Using STM32CubeIDE

1. Open `pancake-making-machine.ioc` in CubeMX
2. Verify peripheral configuration:
   - **SPI1:** Master, 8-bit, Mode 0, ~2 MHz clock
   - **TIM1:** PWM mode, 16-bit, appropriate frequency
   - **TIM2:** Output comparison for stepper steps
   - **UART3:** 115200 baud, 8N1

3. Generate code (preserves USER CODE sections)

4. Import into STM32CubeIDE

5. Add FreeRTOS source to project:
   - Right-click project → Properties
   - C/C++ General → Source locations
   - Add FreeRTOS include and source directories

6. Build project

### Using ARM GCC Directly

1. **Create Makefile:** (See CMakeLists.txt for reference)
   ```makefile
   CC = arm-none-eabi-gcc
   OBJCOPY = arm-none-eabi-objcopy
   
   CFLAGS = -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -std=c99 -Wall
   LDFLAGS = -T./STM32F446RETX_FLASH.ld
   ```

2. **Compile:**
   ```bash
   arm-none-eabi-gcc -c Core/Src/main.c -o main.o $(CFLAGS) -ICore/Inc ...
   ```

3. **Link:**
   ```bash
   arm-none-eabi-gcc *.o -o firmware.elf $(LDFLAGS)
   ```

4. **Convert to HEX:**
   ```bash
   arm-none-eabi-objcopy -O ihex firmware.elf firmware.hex
   ```

## Flashing the Firmware

### Using ST-Link Utility

```bash
st-flash write firmware.bin 0x08000000
```

### Using OpenOCD

```bash
openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg
```

Then in telnet:
```
> halt
> flash write_image erase firmware.elf
> reset
> exit
```

### Using CubeProgrammer

1. Open STM32CubeProgrammer
2. Select ST-Link debugger
3. Load `firmware.hex` or `.elf` file
4. Click **Download**

## Verification

### Serial Console Commands

After flashing, connect serial terminal at 115200 baud:

```bash
# Test basic commands
? ↵                    # Show help
Q ↵                    # Query status
T220 ↵                 # Set temp to 220°C
P75 ↵                  # Set pump to 75%
H ↵                    # Home stepper
S ↵                    # Start cycle
E ↵                    # Emergency stop
```

### Expected Startup Output

```
=== Injera Machine Control v1.0 ===
Ready for commands

--- System Status ---
Mode: IDLE
Temp: 25.0 C (setpoint: 220.0 C)
Pump: OFF (duty: 0%)
Theta: OFF (RPM: 0.0)
E-Stop: OK, Cover: CLOSED
Cycles: 0
```

### Debug Output Verification

- Observe temperature rising when T220 command sent
- Watch motor rotate when H command sent
- Check SSR/pump PWM on oscilloscope at specified frequencies

## Troubleshooting

### Compilation Errors

1. **Missing FreeRTOS headers**
   - Ensure FreeRTOSConfig.h is in Inc/ directory
   - Verify include paths in compiler settings

2. **Undefined references to SPI/UART**
   - Ensure HAL peripherals initialized in main()
   - Check MX_SPI1_Init() and MX_UART3_Init() called

3. **ARM compiler warnings**
   - Add flags: `-Wno-unused-parameter`
   - Add `-ffunction-sections -fdata-sections` for optimization

### Runtime Issues

1. **Bootloader not starting**
   - Flash main.c not reflecting changes
   - Try **Erase All** before reflashing

2. **UART not outputting**
   - Verify UART_TX/RX pins in main.h match physical pins
   - Check serial terminal baud rate (115200)
   - Use oscilloscope to verify TX signal

3. **Temperature not increasing**
   - Check TIM1 PWM output on oscilloscope
   - Verify SSR connections
   - Confirm MAX31865 SPI communication (measure MISO/MOSI)

4. **Motor not homing**
   - Verify ESTOP/EN_THETA GPIO pins
   - Check home switch wiring
   - Monitor GPIO states via debugger

5. **FreeRTOS crashes**
   - Increase configTOTAL_HEAP_SIZE in FreeRTOSConfig.h
   - Add debug logging to memory allocation hooks
   - Enable stack overflow detection (`configCHECK_FOR_STACK_OVERFLOW = 2`)

## Performance Optimization

### Memory Optimization

Current configuration allocates:
- Heap: 32 kB (from 128 kB total RAM)
- Tasks: ~2 kB each (8 tasks = ~16 kB)
- Buffers (UART, logging): ~2 kB

**For reduced-RAM variants:**
```c
#define configTOTAL_HEAP_SIZE    (16 * 1024)  /* 16 kB */
#define UART_RX_BUFFER_SIZE       64           /* 64 bytes */
#define LOG_BUFFER_SIZE           500          /* 500 entries */
```

### Execution Speed

FreeRTOS Task Times (estimated):
- Safety (1 kHz):  < 0.5 ms
- Thermal (25 Hz): < 2 ms
- Motion (25 Hz):  < 1 ms
- Pump (10 Hz):    < 0.5 ms
- HMI (2 Hz):      < 5 ms (occasionally)
- Logging (1 Hz):  < 2 ms (occasionally)

**Total CPU load:** ~10-15% average, allowing 85-90% idle for debugging/extensions

## Integration Checklist

- [ ] All 8 header files in `Core/Inc/`
- [ ] All 7 `.c` implementation files in `Core/Src/`
- [ ] FreeRTOSConfig.h configured for STM32F446
- [ ] main.c updated with FreeRTOS task creation
- [ ] FreeRTOS source files added to project
- [ ] Include paths point to FreeRTOS and HAL directories
- [ ] Linker script has sufficient RAM/FLASH allocation
- [ ] Project compiles without errors
- [ ] Firmware flashes and runs
- [ ] UART serial interface responds to commands
- [ ] Safety inputs (E-stop, cover) trigger shutdown
- [ ] Temperature control reaches setpoint
- [ ] Motor rotates with homing sequence

## Next Steps

1. **Connect test instrumentation:**
   - Thermocouple on plate for temperature validation
   - Oscilloscope on SPI, PWM, stepper signals
   - Flow meter on pump outlet

2. **Perform calibration:**
   - PID tuning for your specific plate thermal mass
   - Pump flow rate measurement per nozzle
   - Stepper motor speed verification

3. **Run acceptance tests:**
   - 10 consecutive dispense cycles
   - Temperature stability under load
   - Safety chain verification
   - Complete 8-hour runtime test

4. **Document deviations:**
   - Record actual PID gains used
   - Log pump trim values per nozzle
   - Note any hardware modifications

## Support & Documentation

- **Control Software Details:** See `CONTROL_SOFTWARE.md`
- **Hardware Architecture:** See `docs/architecture/`
- **Requirements Traceability:** See `docs/specs/traceability.csv`
- **ST HAL References:** STM32F446 HAL User Manual

---

**Document Version:** 1.0  
**Last Updated:** 2026-03-22
