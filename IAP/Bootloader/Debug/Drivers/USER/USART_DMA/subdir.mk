################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/USER/USART_DMA/UARTConfig.c 

OBJS += \
./Drivers/USER/USART_DMA/UARTConfig.o 

C_DEPS += \
./Drivers/USER/USART_DMA/UARTConfig.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/USER/USART_DMA/%.o Drivers/USER/USART_DMA/%.su Drivers/USER/USART_DMA/%.cyclo: ../Drivers/USER/USART_DMA/%.c Drivers/USER/USART_DMA/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/USER/USART_DMA -I../Drivers/USER/Ymodem -I../Drivers/USER/STMFLASH -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-USER-2f-USART_DMA

clean-Drivers-2f-USER-2f-USART_DMA:
	-$(RM) ./Drivers/USER/USART_DMA/UARTConfig.cyclo ./Drivers/USER/USART_DMA/UARTConfig.d ./Drivers/USER/USART_DMA/UARTConfig.o ./Drivers/USER/USART_DMA/UARTConfig.su

.PHONY: clean-Drivers-2f-USER-2f-USART_DMA

