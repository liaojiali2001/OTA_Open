#ifndef _BSP_LED_H_
#define _BSP_LED_H_

#include "stdint.h"
#include "string.h"
#include "main.h"

#ifdef LED0_GPIO_Port
	void pfLED0_Set_PinLevel(uint8_t level);/* 函数功能：引脚电平设置函数 */
#endif

#ifdef LED1_GPIO_Port
	void pfLED1_Set_PinLevel(uint8_t level);/* 函数功能：引脚电平设置函数 */
#endif


	
#endif

