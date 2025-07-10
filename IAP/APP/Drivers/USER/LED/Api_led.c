#include "Api_led.h"
#include "Bsp_led.h"
#include "Dri_led.h"


/* 创建api_key实例  并绑定相应的函数 -------没有新增需求，禁止改动 */
api_led ledAPI = {
    .Led_Init				= Dri_Led_Init,
    .Led_Set				= Dri_Led_Set,
    .Led_Link				= Dri_Led_Link,
    .Led_RemoveLink			= Dri_Led_RemoveLink,
    .Led_handler			= Dri_Led_handler,
	.Led_Poll				= Dri_Led_Poll
};

/************************************************
LED设备注册流程：
	1.调用初始化函数	ledAPI.Led_Init(&LED0, Valid_Low, pfLED0_Set_PinLevel);
		传入参数1	：创建的LED设备			LED0 / LED1
		传入参数2	：有效电平(高或者低)		Valid_Low / Valid_High
		传入参数3	：led电平输出函数			bsp_led.h中定义

	2.调用设置参数函数		ledAPI.Led_Set(&LED0, 1000, 1000, 5, 0);
		传入参数1	：创建的LED设备			LED0 / LED1
		传入参数2	：LED亮的时间				1000
		传入参数3	：LED灭的时间				1000
		传入参数4	：设置闪烁次数			5
		传入参数5	：一个周期结束后，到下一个周期的时间间隔		0
    
	3.调用放入链表函数		ledAPI.Led_Link(&LED0);
		传入参数1	：创建的LED设备			LED0 / LED1
	

LED设备运行流程：
	1.将注册函数初始化		LED0_Register()
	2.
		led单独检测状态
			在定时器 /滴答定时器 /线程    中使用函数	ledAPI.Led_handler检测
		led轮询检测状态
			在定时器 /滴答定时器 /线程		中使用函数	ledAPI.Led_Poll检测(使用poll的前提是led设备加入链表)
	3.可在不同情况下更改LED亮灭状态  	ledAPI.Led_Set(&LEDX, XXX, XXX, XXX, XXX);
************************************************/

/* 创建led  可在此处添加或减少led数量 -------根据需求改动 */
	s_LED LED0;
	s_LED LED1;

void LED_Init_User(void)//查找LED事件函数--添加到初始化部分
{
	LED0_Register();  //led0注册
	LED1_Register();  //led1注册

}

void LED_Handler_User(void)//查找LED运行状态函数--建议添加到1ms的定时器中
{
	//可以放在void SysTick_Handler(void)中，比较接近设置的时间
#if 0
	ledAPI.Led_handler(&LED0);
	ledAPI.Led_handler(&LED1);
#else
	ledAPI.Led_Poll();
#endif
}

void LED0_Register(void)  //led0注册
{
	ledAPI.Led_Init(&LED0, Valid_Low, pfLED0_Set_PinLevel);
	ledAPI.Led_Set(&LED0, 1000, 1000, 5, 0);
	ledAPI.Led_Link(&LED0);
}

void LED1_Register(void)  //led1注册
{
	ledAPI.Led_Init(&LED1, Valid_Low, pfLED1_Set_PinLevel); 
	ledAPI.Led_Set(&LED1, 1000, 1000, 5, 0);
	ledAPI.Led_Link(&LED1);
}

