################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/USER/Ymodem/Ymodem.c 

OBJS += \
./Drivers/USER/Ymodem/Ymodem.o 

C_DEPS += \
./Drivers/USER/Ymodem/Ymodem.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/USER/Ymodem/%.o Drivers/USER/Ymodem/%.su Drivers/USER/Ymodem/%.cyclo: ../Drivers/USER/Ymodem/%.c Drivers/USER/Ymodem/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/USER/USART_DMA -I../Drivers/USER/Ymodem -I../Drivers/USER/STMFLASH -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-USER-2f-Ymodem

clean-Drivers-2f-USER-2f-Ymodem:
	-$(RM) ./Drivers/USER/Ymodem/Ymodem.cyclo ./Drivers/USER/Ymodem/Ymodem.d ./Drivers/USER/Ymodem/Ymodem.o ./Drivers/USER/Ymodem/Ymodem.su

.PHONY: clean-Drivers-2f-USER-2f-Ymodem

