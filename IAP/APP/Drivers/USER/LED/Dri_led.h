#ifndef _DRIVER_LED_H_
#define _DRIVER_LED_H_

#include "stdint.h"
#include "string.h"


typedef struct s_LED
{
    uint8_t  LED_Set_LevelOn 		:1;	//灯亮电平状态
    uint8_t  LED_Read_LevelNow     	:1;	//当前电平状态
	uint8_t  LED_Read_LevelOld     	:1;	//上次电平状态
	uint8_t  LED_Read_State         :5;	//灯亮灭，间隔期间的标志
    uint8_t  LED_Set_FlickerNum;		//设置闪烁次数
    uint8_t  LED_Read_FlickerNum;		//闪烁次数计数

	uint16_t LED_Set_LevelOn_Time;		//每次闪烁灯亮持续时间
	uint16_t LED_Set_LevelOff_Time;		//每次闪烁灯灭持续时间

	uint16_t LED_Set_Interval_Time;		//闪烁LED_Set_FlickerNum次算一个周期，LED_Set_Interval_Time为两个周期的间隔时间
	uint16_t LED_Read_Time;				//不同状态的时间计时

	void  (*LED_Set_LEDLevel)(uint8_t level);
    struct s_LED* next;
}s_LED;




void Dri_Led_Init(s_LED* sLEDx, uint8_t LED_Set_LevelOn,void(*pfLED_Set_PinLevel)(uint8_t));
void Dri_Led_Set(s_LED* sLEDx,uint16_t LED_Set_LevelOn_Time,uint16_t LED_Set_LevelOff_Time,uint8_t LED_Set_FlickerNum,uint16_t LED_Set_Interval_Time);
int  Dri_Led_Link(s_LED* sLEDx);
void Dri_Led_RemoveLink(s_LED* sLEDx);
void Dri_Led_handler(s_LED* s_LEDx);
void Dri_Led_Poll(void);




#endif


