################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/USER/LED/Api_led.c \
../Drivers/USER/LED/Bsp_led.c \
../Drivers/USER/LED/Dri_led.c 

OBJS += \
./Drivers/USER/LED/Api_led.o \
./Drivers/USER/LED/Bsp_led.o \
./Drivers/USER/LED/Dri_led.o 

C_DEPS += \
./Drivers/USER/LED/Api_led.d \
./Drivers/USER/LED/Bsp_led.d \
./Drivers/USER/LED/Dri_led.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/USER/LED/%.o Drivers/USER/LED/%.su Drivers/USER/LED/%.cyclo: ../Drivers/USER/LED/%.c Drivers/USER/LED/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/USER/LED -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-USER-2f-LED

clean-Drivers-2f-USER-2f-LED:
	-$(RM) ./Drivers/USER/LED/Api_led.cyclo ./Drivers/USER/LED/Api_led.d ./Drivers/USER/LED/Api_led.o ./Drivers/USER/LED/Api_led.su ./Drivers/USER/LED/Bsp_led.cyclo ./Drivers/USER/LED/Bsp_led.d ./Drivers/USER/LED/Bsp_led.o ./Drivers/USER/LED/Bsp_led.su ./Drivers/USER/LED/Dri_led.cyclo ./Drivers/USER/LED/Dri_led.d ./Drivers/USER/LED/Dri_led.o ./Drivers/USER/LED/Dri_led.su

.PHONY: clean-Drivers-2f-USER-2f-LED

