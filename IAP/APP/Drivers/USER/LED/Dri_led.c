#include "Dri_led.h"

static struct s_LED* LED_Head_handle = NULL;//以链表形式创建LED设备，头指针head_handle

/* 函数功能：记录LED的初始电平及电平控制
 * sLEDx:初始化设备时传入的LED结构体变量
 * bLED_Set_LevelOn：LED灯亮时的电平,1bit(bool类型)，即0或者1
 * pfLED_Set_PinLevel:传入的函数指针，对应每个LED的电平输出控制函数
*/
void Dri_Led_Init(s_LED* sLEDx, uint8_t LED_Set_LevelOn,void(*pfLEDx_Set_PinLevel)(uint8_t))
{
	memset(sLEDx,0,sizeof(s_LED));
	sLEDx->LED_Set_LevelOn		= LED_Set_LevelOn;
	sLEDx->LED_Set_LEDLevel		= pfLEDx_Set_PinLevel;
	sLEDx->LED_Read_LevelNow	= !sLEDx->LED_Set_LevelOn;
	sLEDx->LED_Set_LEDLevel(sLEDx->LED_Read_LevelNow);
	sLEDx->LED_Read_LevelOld	= sLEDx->LED_Read_LevelNow;
}

/* 函数功能：设置LED的闪烁时间，亮灭时间
 * s_LED:初始化设备时传入的LED结构体变量
 * LED_Set_LevelOn_Time		:LED亮的时间
 * LED_Set_LevelOff_Time	:LED灭的时间
 * LED_Set_FlickerNum		:设置闪烁次数(可以用来设置某个LED用来显示是特定信息，例如通过该变量展示设备ID号)
 * LED_Set_Interval_Time	:闪烁LED_Set_FlickerNum次后，一个周期结束。LED_Set_Interval_Time代表两个周期间的间隔
*/
void Dri_Led_Set(s_LED*   sLEDx,
				 uint16_t LED_Set_LevelOn_Time,
				 uint16_t LED_Set_LevelOff_Time,
				 uint8_t  LED_Set_FlickerNum,
				 uint16_t LED_Set_Interval_Time
				 )
{
	sLEDx->LED_Set_LevelOn_Time		= LED_Set_LevelOn_Time;
	sLEDx->LED_Set_LevelOff_Time	= LED_Set_LevelOff_Time;
	sLEDx->LED_Set_FlickerNum		= LED_Set_FlickerNum;
	sLEDx->LED_Set_Interval_Time	= LED_Set_Interval_Time;
	sLEDx->LED_Read_FlickerNum		= 0;
	sLEDx->LED_Read_Time			= 0;
}

/* 函数功能：将LED设备注册到链表中，方便后续轮询查找，处理
 * s_LED:传入链表的LED设备
 * 例子：
 1.将sLED0加入链表，此时sLED0->next=NULL,head_handle=sLED0;  即LED_Head_handle(sLED0)->next=NULL
 	 LED_Head_handle(sLED0)->NULL
 2.将sLED1加入链表，即LED_Head_handle(sLED1)->next=sLED0
 	 LED_Head_handle(sLED1)->sLED0->NULL
 	 ....
*/
int Dri_Led_Link(s_LED* sLEDx)
{
    // 检查输入句柄是否为 NULLS
    if (sLEDx == NULL) {
        return -1; // 输入无效
    }
    // 初始化时间
    sLEDx->LED_Read_Time = 0;
    // 遍历链表，检查句柄是否已存在
    s_LED* current = LED_Head_handle;
    while (current != NULL)
    {
        if (current == sLEDx)
        {
            return -1; // 句柄已存在
        }
        current = current->next;
    }

    // 将新句柄添加到链表头部
    sLEDx->next = LED_Head_handle;
    LED_Head_handle = sLEDx;
    return 0; // 成功添加
}

/* 函数功能：将LED设备移出链表
 * s_LED:要移出链表的LED设备
*/
void Dri_Led_RemoveLink(s_LED* sLEDx)
{
	if (sLEDx == NULL) {
		return; // 空指针检查
	}
	// 当前指针指向头指针的地址
	s_LED **curr = &LED_Head_handle;

	// 遍历链表
	while (*curr != NULL)
	{
		s_LED *entry = *curr; // 当前节点
		// 检查句柄是否匹配
		if (sLEDx == entry)
		{
			sLEDx->LED_Set_LEDLevel(!sLEDx->LED_Set_LevelOn); // 反转LED状态
			*curr = entry->next;	// 从链表中删除当前节点
			break; // 找到目标后退出循环
		}
		// 移动到下一个节点
		curr = &entry->next;
	}
}

/* 函数功能：LED控制函数，亮灭闪烁的具体判断实现函数，建议放在周期函数里面
 * sLEDx:初始化设备时传入的LED结构体变量
*/
void Dri_Led_handler(s_LED* s_LEDx)
{
    if (s_LEDx == NULL) return; // NULL 检查以确保有效句柄
    // 递减 TIME
    if (s_LEDx->LED_Read_Time > 0)
    {
    	s_LEDx->LED_Read_Time--;
    }

    // 当 TIME 为 0 时，处理不同的状态
    if (s_LEDx->LED_Read_Time == 0)
    {
        switch (s_LEDx->LED_Read_State)
        {
            case 0: // 开灯阶段
                if (s_LEDx->LED_Set_LevelOn_Time > 0)
                {
                	s_LEDx->LED_Read_LevelNow 	= s_LEDx->LED_Set_LevelOn;
                	s_LEDx->LED_Read_Time 		= s_LEDx->LED_Set_LevelOn_Time;
                	s_LEDx->LED_Read_State = 1;
                }
                break;
            case 1: // 关灯阶段
                if (s_LEDx->LED_Set_LevelOff_Time > 0)
                {
                	s_LEDx->LED_Read_LevelNow = s_LEDx->LED_Set_LevelOn ? 0 : 1;
                	s_LEDx->LED_Read_Time = s_LEDx->LED_Set_LevelOff_Time;
                	s_LEDx->LED_Read_State = 2;
                }
                break;
            case 2: // 闪烁阶段
                if (s_LEDx->LED_Set_Interval_Time != 0)
                {
                    if (s_LEDx->LED_Set_FlickerNum > 0)
                    {
                    	s_LEDx->LED_Read_FlickerNum++;
                        // 检查是否达到闪烁次数
                        if (s_LEDx->LED_Read_FlickerNum >= s_LEDx->LED_Set_FlickerNum)
                        {
                        	s_LEDx->LED_Read_Time = s_LEDx->LED_Set_Interval_Time;
                        	s_LEDx->LED_Read_State = 3;
                        	s_LEDx->LED_Read_FlickerNum = 0; // 重置计数
                        }
                    }
                }
                // 如果没有满闪烁次数
                if (s_LEDx->LED_Read_State == 2)
                {
                	s_LEDx->LED_Read_State = 0; // 重新开始
                }
                break;
            case 3: // 闪烁结束
            	s_LEDx->LED_Read_State = 0; // 重新开始
                break;
        }
    }
    // 检查亮度状态并更新 LED
    if (s_LEDx->LED_Read_LevelOld != s_LEDx->LED_Read_LevelNow)
    {
    	s_LEDx->LED_Set_LEDLevel(s_LEDx->LED_Read_LevelNow);
    	s_LEDx->LED_Read_LevelOld = s_LEDx->LED_Read_LevelNow; // 更新旧状态
    }
}

/* 
	函数功能：轮询检测按键状态
*/
void Dri_Led_Poll(void)
{
	s_LED* target;
	for(target=LED_Head_handle; target; target=target->next) 
	{
		Dri_Led_handler(target);
	}
}
