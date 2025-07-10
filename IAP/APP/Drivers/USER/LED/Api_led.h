#ifndef _API_LED_H_
#define _API_LED_H_

#include "main.h"
#include "stdint.h"
#include "string.h"

#include "Dri_led.h"


#define Valid_Low 		0
#define Valid_High 		1   	//led有效电平



typedef struct api_led
{
/* 默认key的基础设置只设置一次，即Key_Set函数的参数只会在初始化的时候修改*/
/* api_function  函数api*/
	void (*Led_Init)(s_LED* sLEDx, uint8_t LED_Set_LevelOn,void(*pfLED_Set_PinLevel)(uint8_t));
	void (*Led_Set)(s_LED* sLEDx,
									uint16_t LED_Set_LevelOn_Time,
									uint16_t LED_Set_LevelOff_Time,
									uint8_t LED_Set_FlickerNum,
									uint16_t LED_Set_Interval_Time);
	int  (*Led_Link)(s_LED* sLEDx);
	void (*Led_RemoveLink)(s_LED* sLEDx);
	void (*Led_handler)(s_LED* sLEDx);
	void (*Led_Poll)(void);
}api_led;

extern s_LED LED0;
extern s_LED LED1;
extern api_led ledAPI;


void LED_Init_User(void);
void LED_Handler_User(void);


void LED0_Register(void);  //led0注册
void LED1_Register(void);  //led1注册





#endif
