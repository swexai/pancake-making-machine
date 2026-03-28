################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/build/CMakeFiles/3.28.3/CompilerIdC/CMakeCCompilerId.c 

OBJS += \
./Core/build/CMakeFiles/3.28.3/CompilerIdC/CMakeCCompilerId.o 

C_DEPS += \
./Core/build/CMakeFiles/3.28.3/CompilerIdC/CMakeCCompilerId.d 


# Each subdirectory must supply rules for building sources it contributes
Core/build/CMakeFiles/3.28.3/CompilerIdC/%.o Core/build/CMakeFiles/3.28.3/CompilerIdC/%.su Core/build/CMakeFiles/3.28.3/CompilerIdC/%.cyclo: ../Core/build/CMakeFiles/3.28.3/CompilerIdC/%.c Core/build/CMakeFiles/3.28.3/CompilerIdC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FreeRTOS/FreeRTOS/Source/include -I../FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-build-2f-CMakeFiles-2f-3-2e-28-2e-3-2f-CompilerIdC

clean-Core-2f-build-2f-CMakeFiles-2f-3-2e-28-2e-3-2f-CompilerIdC:
	-$(RM) ./Core/build/CMakeFiles/3.28.3/CompilerIdC/CMakeCCompilerId.cyclo ./Core/build/CMakeFiles/3.28.3/CompilerIdC/CMakeCCompilerId.d ./Core/build/CMakeFiles/3.28.3/CompilerIdC/CMakeCCompilerId.o ./Core/build/CMakeFiles/3.28.3/CompilerIdC/CMakeCCompilerId.su

.PHONY: clean-Core-2f-build-2f-CMakeFiles-2f-3-2e-28-2e-3-2f-CompilerIdC

