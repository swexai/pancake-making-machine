################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/control_system.c \
../Core/Src/freertos.c \
../Core/Src/hmi.c \
../Core/Src/logging.c \
../Core/Src/main.c \
../Core/Src/motion_control.c \
../Core/Src/pump_control.c \
../Core/Src/safety_control.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_hal_timebase_tim.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/thermal_control.c 

OBJS += \
./Core/Src/control_system.o \
./Core/Src/freertos.o \
./Core/Src/hmi.o \
./Core/Src/logging.o \
./Core/Src/main.o \
./Core/Src/motion_control.o \
./Core/Src/pump_control.o \
./Core/Src/safety_control.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_hal_timebase_tim.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/thermal_control.o 

C_DEPS += \
./Core/Src/control_system.d \
./Core/Src/freertos.d \
./Core/Src/hmi.d \
./Core/Src/logging.d \
./Core/Src/main.d \
./Core/Src/motion_control.d \
./Core/Src/pump_control.d \
./Core/Src/safety_control.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_hal_timebase_tim.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/thermal_control.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FreeRTOS/FreeRTOS/Source/include -I../FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/control_system.cyclo ./Core/Src/control_system.d ./Core/Src/control_system.o ./Core/Src/control_system.su ./Core/Src/freertos.cyclo ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/hmi.cyclo ./Core/Src/hmi.d ./Core/Src/hmi.o ./Core/Src/hmi.su ./Core/Src/logging.cyclo ./Core/Src/logging.d ./Core/Src/logging.o ./Core/Src/logging.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/motion_control.cyclo ./Core/Src/motion_control.d ./Core/Src/motion_control.o ./Core/Src/motion_control.su ./Core/Src/pump_control.cyclo ./Core/Src/pump_control.d ./Core/Src/pump_control.o ./Core/Src/pump_control.su ./Core/Src/safety_control.cyclo ./Core/Src/safety_control.d ./Core/Src/safety_control.o ./Core/Src/safety_control.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_hal_timebase_tim.cyclo ./Core/Src/stm32f4xx_hal_timebase_tim.d ./Core/Src/stm32f4xx_hal_timebase_tim.o ./Core/Src/stm32f4xx_hal_timebase_tim.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/thermal_control.cyclo ./Core/Src/thermal_control.d ./Core/Src/thermal_control.o ./Core/Src/thermal_control.su

.PHONY: clean-Core-2f-Src

