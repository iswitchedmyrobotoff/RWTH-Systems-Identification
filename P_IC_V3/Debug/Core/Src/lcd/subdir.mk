################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/lcd/lcd.c 

C_DEPS += \
./Core/Src/lcd/lcd.d 

OBJS += \
./Core/Src/lcd/lcd.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/lcd/%.o Core/Src/lcd/%.su Core/Src/lcd/%.cyclo: ../Core/Src/lcd/%.c Core/Src/lcd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-lcd

clean-Core-2f-Src-2f-lcd:
	-$(RM) ./Core/Src/lcd/lcd.cyclo ./Core/Src/lcd/lcd.d ./Core/Src/lcd/lcd.o ./Core/Src/lcd/lcd.su

.PHONY: clean-Core-2f-Src-2f-lcd

