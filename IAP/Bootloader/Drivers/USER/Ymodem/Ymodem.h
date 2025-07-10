#ifndef __YMODEM_H_
#define __YMODEM_H_
#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
//符号    数值    含义
#define	ymodem_SOH     0X01    	//128字节数据包
#define ymodem_STX     0X02    	//1024字节数据包
#define ymodem_EOT     0X04    	//结束传输
#define ymodem_ACK     0X06    	//正确接收回应
#define ymodem_NAK     0X15    	//错误接受回应
#define ymodem_CAN     0X18    	//传输中止
#define ymodem_C       0X43    	//请求数据
#define ymodem_Len128  	128    	//128 Bytes
#define ymodem_Len1024	1024    //1024 Bytes


#define APP_ADDR	0X08020000	//写APP的FLASH位置,bootloader预留128kB
#define FlashWrite_Once	256		//FLASH单次操作写入的字节数量
#define MAX_RETRY	20			//数据传输中错误允许最大次数

#define TERMINAL_SUPPORTS_CARRIAGE_RETURN 0//如果串口终端支持 \r 则定义为 1，否则为 0
#define ymodem_UseSecureCRT		//是否使用SecureCRT传输数据---------其和标准不同-这里仅支持128传输，1024的传输和标准协议不同，不做编写
#define ymodem_UARTHANDLE 	&huart1
#define ymodem_hdmatx 		&hdma_usart1_tx
#define ymodem_hdmarx 		&hdma_usart1_rx
#define My_mDelay(x)		HAL_Delay(x)

typedef enum
{
	ymodem_STATE_IDLE,		//空闲状态
	ymodem_STATE_RECEIVING,	//接收状态
	ymodem_STATE_FINISHED,	//传输结束
	ymodem_STATE_ERROR,		//传输错误

	ymodem_STATE_UPDATE		//更新标志-可以跳转到APP
}YMODEM_State;//整个传输过程的状态标志位

typedef struct
{
    uint8_t currentBlockNum;    //当前块号
    uint16_t currentBlockSize;	//当前包大小
    uint16_t totalBlockSize;	//当前包总大小
    uint8_t fileBuffer[1024+5];	//单包缓冲	128+5/1024+5
    uint8_t fileName[128];      //文件名--------------只有头包数据有
    uint8_t fileSizeHex[128];	//文件大小--接收的数据
    uint32_t fileSize;          //文件大小
    YMODEM_State state;       	//当前接收状态

    uint8_t rec_EOTcount;		//接收EOT的次数
    volatile uint8_t uartRecFlag;//串口接收完成标志位  0：空闲	1：接收完成
    uint32_t writeIndex;		//已经写入的字节大小
    uint8_t writeData[FlashWrite_Once];//将要写入的实际数据-页/块写入
    uint8_t retryCount;			//记录数据传输过程中错误次数，要求重发并记录重发次数
    uint32_t timeOut;			//记录空闲状态下，发送请求的时间

//    void (*ymodem_memoryWrite)(uint32_t addr,uint32_t len,uint8_t *writeBuffer);//绑定写入FLASH的函数
}YMODEM_Data;

extern YMODEM_Data ymodem_Info;

void update_JumpToApplication(void);//跳转到APP地址
void ymodem_PrintProgress(uint32_t receivedSize, uint32_t totalSize);
void ymodem_InfoAnalysis(void);
void ymodem_SendCMD(uint8_t CMD);

bool ymodem_InitialFrameAnalysis(const uint8_t *pdata,uint16_t len);//初始帧解析
bool ymodem_DataFrameAnalysis(const uint8_t *pdata,uint16_t len);//数据帧解析
bool HandleSOH_STX(const uint8_t *pdata,uint16_t len,uint16_t DataNum);//数据帧解析
bool  ymodem_EndFrameAnalysis(const uint8_t *pdata,uint16_t len);//结尾帧解析
uint16_t ymodem_Crc16(const uint8_t *data, uint16_t length);
#endif
