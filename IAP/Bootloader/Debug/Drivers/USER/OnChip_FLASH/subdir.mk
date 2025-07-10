################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/USER/OnChip_FLASH/FLASH_Operate.c 

OBJS += \
./Drivers/USER/OnChip_FLASH/FLASH_Operate.o 

C_DEPS += \
./Drivers/USER/OnChip_FLASH/FLASH_Operate.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/USER/OnChip_FLASH/%.o Drivers/USER/OnChip_FLASH/%.su Drivers/USER/OnChip_FLASH/%.cyclo: ../Drivers/USER/OnChip_FLASH/%.c Drivers/USER/OnChip_FLASH/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/USER/USART_DMA -I../Drivers/USER/Ymodem -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-USER-2f-OnChip_FLASH

clean-Drivers-2f-USER-2f-OnChip_FLASH:
	-$(RM) ./Drivers/USER/OnChip_FLASH/FLASH_Operate.cyclo ./Drivers/USER/OnChip_FLASH/FLASH_Operate.d ./Drivers/USER/OnChip_FLASH/FLASH_Operate.o ./Drivers/USER/OnChip_FLASH/FLASH_Operate.su

.PHONY: clean-Drivers-2f-USER-2f-OnChip_FLASH

