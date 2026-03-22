cmake_minimum_required(VERSION 3.22)

project(pancake-making-machine C ASM)

# ==================== Compiler & Target Settings ==========================

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

# STM32F446RE target
set(STM32_DEVICE STM32F446RETx)
set(STM32_FAMILY STM32F4xx)
set(MCU_ARCH cortex-m4)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -mcpu=${MCU_ARCH} \
    -mthumb \
    -mfpu=fpv4-sp-d16 \
    -mfloat-abi=hard \
    -fdata-sections \
    -ffunction-sections \
    -DUSE_HAL_DRIVER \
    -DSTM32F446xx \
    -Wall \
    -Wextra")

set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")
set(CMAKE_C_FLAGS_RELEASE "-O2")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
    -mcpu=${MCU_ARCH} \
    -mthumb \
    -mfpu=fpv4-sp-d16 \
    -mfloat-abi=hard \
    --specs=nano.specs \
    -lc -lm \
    -Wl,-Map=${PROJECT_NAME}.map \
    -Wl,--gc-sections")

# ==================== HAL & Middleware Paths ============================

set(DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Drivers)
set(CORE_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Inc)
set(CORE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Src)
set(FREERTOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../FreeRTOS/FreeRTOS)

# ==================== Include Directories ===============================

include_directories(
    ${CORE_INC_DIR}
    ${DRIVERS_DIR}/STM32F4xx_HAL_Driver/Inc
    ${DRIVERS_DIR}/CMSIS/Include
    ${DRIVERS_DIR}/CMSIS/Device/ST/STM32F4xx/Include
    ${FREERTOS_DIR}/Source/include
    ${FREERTOS_DIR}/Source/portable/GCC/ARM_CM4F
)

# ==================== Source Files ======================================

# Application source files
set(APP_SOURCES
    ${CORE_SRC_DIR}/main.c
    ${CORE_SRC_DIR}/control_system.c
    ${CORE_SRC_DIR}/thermal_control.c
    ${CORE_SRC_DIR}/motion_control.c
    ${CORE_SRC_DIR}/pump_control.c
    ${CORE_SRC_DIR}/safety_control.c
    ${CORE_SRC_DIR}/hmi.c
    ${CORE_SRC_DIR}/logging.c
)

# HAL source files (from Drivers)
file(GLOB HAL_SOURCES
    ${DRIVERS_DIR}/STM32F4xx_HAL_Driver/Src/*.c
)

# FreeRTOS source files
set(FREERTOS_SOURCES
    ${FREERTOS_DIR}/Source/tasks.c
    ${FREERTOS_DIR}/Source/queue.c
    ${FREERTOS_DIR}/Source/list.c
    ${FREERTOS_DIR}/Source/timers.c
    ${FREERTOS_DIR}/Source/portable/MemMang/heap_4.c
    ${FREERTOS_DIR}/Source/portable/GCC/ARM_CM4F/port.c
)

# Startup & system files
set(STARTUP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Startup/startup_stm32f446retx.s)
set(SYSTEM_FILE ${CORE_SRC_DIR}/system_stm32f4xx.c)

# Combine all sources
set(ALL_SOURCES
    ${APP_SOURCES}
    ${HAL_SOURCES}
    ${FREERTOS_SOURCES}
    ${SYSTEM_FILE}
)

# ==================== Executable Target ==================================

add_executable(${PROJECT_NAME}.elf
    ${ALL_SOURCES}
    ${STARTUP_FILE}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}.elf
    c
    m
)

# ==================== Post-Build Commands ===============================

# Generate .bin and .hex files
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND arm-none-eabi-objcopy -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMENT "Generating .bin and .hex files..."
)

# Print firmware size
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND arm-none-eabi-size --format=berkeley ${PROJECT_NAME}.elf
    COMMENT "Firmware size:"
)

# ==================== Default Build Type ================================

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Target: STM32F446RE")
message(STATUS "Compiler: arm-none-eabi-gcc")
