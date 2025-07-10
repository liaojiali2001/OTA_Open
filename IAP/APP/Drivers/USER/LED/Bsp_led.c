#include "Bsp_led.h"


#ifdef LED0_GPIO_Port
	void pfLED0_Set_PinLevel(uint8_t level)/* 函数功能：引脚电平设置函数 */
	{
		if (level != GPIO_PIN_RESET) LED0_GPIO_Port->BSRR = LED0_Pin;
		else	LED0_GPIO_Port->BSRR = (uint32_t) LED0_Pin << 16U;
	}
#endif

#ifdef LED1_GPIO_Port
	void pfLED1_Set_PinLevel(uint8_t level) /* 函数功能：引脚电平设置函数 */
	{
		if (level != GPIO_PIN_RESET) LED1_GPIO_Port->BSRR = LED1_Pin;
		else	LED1_GPIO_Port->BSRR = (uint32_t) LED1_Pin << 16U;
	}
#endif



