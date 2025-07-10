################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/USER/STMFLASH/STMFLASH.c 

OBJS += \
./Drivers/USER/STMFLASH/STMFLASH.o 

C_DEPS += \
./Drivers/USER/STMFLASH/STMFLASH.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/USER/STMFLASH/%.o Drivers/USER/STMFLASH/%.su Drivers/USER/STMFLASH/%.cyclo: ../Drivers/USER/STMFLASH/%.c Drivers/USER/STMFLASH/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/USER/USART_DMA -I../Drivers/USER/Ymodem -I../Drivers/USER/STMFLASH -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-USER-2f-STMFLASH

clean-Drivers-2f-USER-2f-STMFLASH:
	-$(RM) ./Drivers/USER/STMFLASH/STMFLASH.cyclo ./Drivers/USER/STMFLASH/STMFLASH.d ./Drivers/USER/STMFLASH/STMFLASH.o ./Drivers/USER/STMFLASH/STMFLASH.su

.PHONY: clean-Drivers-2f-USER-2f-STMFLASH

