#include "Ymodem.h"
#include "STMFLASH.h"

YMODEM_Data ymodem_Info={0};

/*******************************************************************************/
//跳转指令
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;

void update_JumpToApplication(void)//跳转到APP地址
{
	uint32_t JumpAddress;
    //if (((*(__IO uint32_t*)APP_ADDR) & 0x2FFE0000 ) == 0x20000000)
    if (((*(__IO uint32_t*)APP_ADDR) & 0x2FFE0000 ) == 0x20020000)//建议下载后连接stlink-unity或者读一下该地址第一个word是什么
    {
/****************************************使能了哪些中断就关掉那些*********************************************/
    	//对于有中断的外设，尽量先停掉，然后再关中断，不然跳转到app会卡死
    	HAL_UART_DMAStop(&huart1);          //停止DMA传输
    	HAL_DMA_Abort(&hdma_usart1_rx);		//先停掉dma的传输
    	HAL_NVIC_DisableIRQ(USART1_IRQn);
    	HAL_NVIC_DisableIRQ(DMA2_Stream2_IRQn);
/****************************************使能了哪些中断就关掉那些*********************************************/

//        HAL_RCC_DeInit();//有的论坛中说明需要此行，但是实际测试不影响

//跳转
		SCB->VTOR = APP_ADDR;  // 重定位向量表
		__set_MSP(*(__IO uint32_t*) APP_ADDR);//设置栈顶指针
		JumpAddress = *(__IO uint32_t*) (APP_ADDR + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		Jump_To_Application();
    }
}

/*******************************************************************************/
/**
 * @brief  传输数据时的串口显示
 * @param  receivedSize	已经接收的数据量
 *		   totalSize	目标接受量
 * @return 空
 * @explain	TERMINAL_SUPPORTS_CARRIAGE_RETURN
 * 			0：不支持\r,例如ssom，串口助手等，靠打印百分比多行显示
 * 			1：支持\r,例如secureCRT,Putty等可以实现一行的动态进度条
 */
#if TERMINAL_SUPPORTS_CARRIAGE_RETURN
	void ymodem_PrintProgress(uint32_t receivedSize, uint32_t totalSize)
	{
		const int barWidth = 50;
		float progress = (float)receivedSize / totalSize;
		int pos = (int)(barWidth * progress);

		printf("\r[");
		for (int i = 0; i < barWidth; ++i) {
			if (i < pos) printf("=");
			else if (i == pos) printf(">");
			else printf(" ");
		}
		printf("] %3d%% (%lu/%lu)", (int)(progress * 100), receivedSize, totalSize);
		fflush(stdout); // 嵌入式平台可视情况决定是否使用
	}
#else
	void ymodem_PrintProgress(uint32_t receivedSize, uint32_t totalSize)
	{
		static int lastPercent = -1;  // 上一次打印的进度百分比
		int percent = (int)(((float)receivedSize / totalSize) * 100);

		if (percent != lastPercent)  // 只在百分比变化时打印
		{
			printf("Progress: %3d%% (%lu/%lu bytes)\n", percent, receivedSize, totalSize);
			lastPercent = percent;
		}
	}
#endif
/**
 * @brief  传输数据时的数据交互发送函数
 * @param  CMD：ymodem_ACK	正确接收回应
 * 				ymodem_C	请求数据
 * 				ymodem_NAK	错误接受回应
 * 				ymodem_CAN	传输中止
 * @return 空
 */
void ymodem_SendCMD(uint8_t CMD)
{
	uint8_t cmd = CMD;
	HAL_UART_Transmit_IT(ymodem_UARTHANDLE, &cmd, 1);
	while(__HAL_UART_GET_FLAG(ymodem_UARTHANDLE,UART_FLAG_TC) == RESET);
}
/**
 * @brief  接收数据，解析数据，处理数据-----此处用于IAP串口升级，OTA升级在app程序里面接收数据
 * @param  空
 * @explain	起始帧：包含文件名，文件大小等信息
 * 			数据帧：包含具体的bin文件数据
 * 			结束帧：包含最后一次的SOH 00 FF 00-----00 00 00
 * 			命令帧：单字节数据交互
 * @return 空
 */
void ymodem_InfoAnalysis(void)
{
	switch(ymodem_Info.state)
	{
		case ymodem_STATE_IDLE:		//空闲状态--分析起始帧(一直发送请求帧，直到有数据)
			if(ymodem_Info.uartRecFlag == 1)//串口接收完成
			{
				//把起始帧解析，包含文件名，文件大小
				if(ymodem_InitialFrameAnalysis(ymodem_Info.fileBuffer,ymodem_Info.currentBlockSize))
				{
					ymodem_SendCMD(ymodem_ACK);
					ymodem_Info.state=ymodem_STATE_RECEIVING;
					STMFLASH_EraseArea(APP_ADDR, ymodem_Info.fileSize);//开启正式传输后，清空app区域，方便后续写入
					printf("Received upgrade data, waiting for data reception to complete\r\n");
				}
				else
				{
					ymodem_SendCMD(ymodem_NAK);  // 解析失败则重传
					ymodem_Info.state = ymodem_STATE_ERROR;
				}
				ymodem_Info.uartRecFlag = 0;
			}
			ymodem_SendCMD(ymodem_C);
			My_mDelay(1000);//开始时,1s发一次请求帧
		break;
		case ymodem_STATE_RECEIVING://接收状态--分析数据帧
			if(ymodem_Info.uartRecFlag == 1)	//判断是数据帧
			{
				if(ymodem_DataFrameAnalysis(ymodem_Info.fileBuffer,ymodem_Info.currentBlockSize))
				{
					//这里的进度显示的接收字节不一定等于文件大小，因为文件大小不一定是128/1024的倍数
					ymodem_PrintProgress(ymodem_Info.totalBlockSize, ymodem_Info.fileSize);//打印进度
					ymodem_SendCMD(ymodem_ACK);
					if(ymodem_Info.totalBlockSize >= ymodem_Info.fileSize)//已经接收完全部数据
					{
						ymodem_Info.state=ymodem_STATE_FINISHED;
						ymodem_Info.writeIndex = 0; // 清零用于下一次接收
					}
				}
				else
				{
					ymodem_SendCMD(ymodem_NAK);  // 数据帧解析失败
					if(++ymodem_Info.retryCount > MAX_RETRY)
					{
						ymodem_Info.state = ymodem_STATE_ERROR;
					}
				}
				ymodem_Info.uartRecFlag = 0;
			}
		break;
		case ymodem_STATE_FINISHED:	//第一次接收到结束帧EOT
			if(ymodem_Info.uartRecFlag == 1)
			{
				switch(ymodem_Info.rec_EOTcount)
				{
					case 0:
						if(ymodem_Info.fileBuffer[0]==ymodem_EOT && ymodem_Info.currentBlockSize==1)
						{
							ymodem_SendCMD(ymodem_NAK);//响应结束命令
							ymodem_Info.rec_EOTcount++;
						}
					break;
					case 1:
						if(ymodem_Info.fileBuffer[0]==ymodem_EOT && ymodem_Info.currentBlockSize==1)
						{
							ymodem_SendCMD(ymodem_ACK);//响应结束命令
							My_mDelay(5);
							ymodem_SendCMD(ymodem_C);//请求结束帧
							ymodem_Info.rec_EOTcount++;
						}
					break;
					case 2:
						if(ymodem_EndFrameAnalysis(ymodem_Info.fileBuffer, ymodem_Info.currentBlockSize))
						{
							ymodem_SendCMD(ymodem_ACK);
							memset(&ymodem_Info,0,sizeof(ymodem_Info));
							ymodem_Info.state = ymodem_STATE_UPDATE;
							printf("Data reception completed, ready to upgrade\r\n");
						}
					break;
					default:break;
				}
				ymodem_Info.uartRecFlag = 0;
			}
		break;
		case ymodem_STATE_ERROR:
			ymodem_SendCMD(ymodem_CAN);  //终止传输
			ymodem_SendCMD(ymodem_CAN);  //终止传输	发送两次代表结束(secureCRT不会结束，会继续等待ACK回应)
			My_mDelay(10);
			memset(&ymodem_Info,0,sizeof(ymodem_Info));
			printf("Transmission process has too many errors,end\r\n");
		break;
		default:break;
	}
}


bool HandleSOH_STX(const uint8_t *pdata,uint16_t len,uint16_t DataNum)
{
    if(pdata == NULL || len < 3 + DataNum + 2) return false; // 帧头+帧序+反码+1024字节数据+2字节CRC
    // 帧结构: SOH(1) + 帧编号(1) + 反码(1) + DataNum字节数据 + CRC16(2)
    if (pdata[1] != (uint8_t)(~pdata[2])) return false;
    uint16_t crc16 = ymodem_Crc16(&pdata[3], DataNum);
    if (crc16 != ((pdata[DataNum+3] << 8) | pdata[DataNum+4])) return false;
    ymodem_Info.currentBlockNum = pdata[1];//记录当前块儿号
    for(uint16_t i=0;i<DataNum;i++)
    {
    	if (ymodem_Info.writeIndex >= ymodem_Info.fileSize) break;

    	ymodem_Info.writeData[(ymodem_Info.writeIndex++)&(FlashWrite_Once-1)]=pdata[3+i];
    	if(ymodem_Info.writeIndex%FlashWrite_Once == 0)//存够一帧写入FLASH的数据
    	{
        	//此处可以把数据写入FLASH
    		STMFLASH_Write_NoErase(APP_ADDR+ymodem_Info.writeIndex- FlashWrite_Once, ymodem_Info.writeData,FlashWrite_Once);
    	}
    }
    //如果最后一帧，没写满，只需要再ymodem_Info.writeData后面补上0xFF
    if (ymodem_Info.writeIndex == ymodem_Info.fileSize &&
        (ymodem_Info.writeIndex % FlashWrite_Once) != 0)
    {
    	uint16_t remain = ymodem_Info.writeIndex % FlashWrite_Once;
		for (uint16_t i = remain; i < FlashWrite_Once; i++)
			ymodem_Info.writeData[i] = 0xFF; // 或 0x00，根据Flash要求
		//此处可以把数据写入FLASH
		STMFLASH_Write_NoErase(APP_ADDR+ymodem_Info.writeIndex- remain, ymodem_Info.writeData,FlashWrite_Once);
    }
    ymodem_Info.totalBlockSize += DataNum;
    return true;
}

/**
 * @brief  解析数据帧
 * @param  pdata：要解析的数据	len：数据大小
 * @return 空
 */
bool ymodem_DataFrameAnalysis(const uint8_t *pdata,uint16_t len)
{
	uint16_t DataNum=(pdata[0]==ymodem_SOH)?128:1024;
	return HandleSOH_STX(pdata,len,DataNum);
}

/**
 * @brief  解析初始帧，记录文件名，文件大小等信息
 * @param  pdata：要解析的数据	len：数据大小
 * @return 空
 */
bool ymodem_InitialFrameAnalysis(const uint8_t *pdata,uint16_t len)
{
    if(pdata == NULL || len < 3 + 128 + 2) return false; // 帧头+帧序+反码+128字节数据+2字节CRC

    // 校验帧头是否为 SOH，是否帧号与帧号反码匹配
    if ((pdata[0] != ymodem_SOH)||(pdata[1]!=0x00)||(pdata[2]!=0xFF) ) return false;
    uint16_t crc16 = ymodem_Crc16(&pdata[3],ymodem_Len128);

    if( crc16 != (pdata[ymodem_Len128+3]<<8|pdata[ymodem_Len128+4]) )//判断CRC是否一致
    {
    	return false;
    }

// 1. 提取文件名（以 0x00 结束）
	uint8_t i=0;
    const uint8_t *dataPtr = &pdata[3]; // 数据段（128字节）开始
    while(i < ymodem_Len128 && dataPtr[i] != '\0')
    {
    	ymodem_Info.fileName[i] = dataPtr[i];
    	i++;
    }
    ymodem_Info.fileName[i] ='\0';//记录结束
    i++; // 跳过文件名结束的 0x00
// 2. 提取文件大小
    uint8_t j=0;
    while ((i < 128) && (dataPtr[i] != ' '|| dataPtr[i] != '\0'))
    {
    	ymodem_Info.fileSizeHex[j++] = dataPtr[i++];
    }
    ymodem_Info.fileSizeHex[j] = '\0'; // 字符串结束符

    // 3. 转换 ASCII 十进制字符串为整数
    ymodem_Info.fileSize = strtol((char *)ymodem_Info.fileSizeHex, NULL, 10);

    //printf("filename:%s,filesize:%d\r\n",ymodem_Info.fileName,ymodem_Info.fileSize);
    return true;
}

bool  ymodem_EndFrameAnalysis(const uint8_t *pdata,uint16_t len)
{
    if(pdata == NULL || len < 3 + ymodem_Len128 + 2) return false; // 帧头+帧序+反码+128字节数据+2字节CRC
    // 校验帧头是否为 SOH，是否帧号与帧号反码匹配
    if ((pdata[0] != ymodem_SOH)||(pdata[1]!=0x00)||(pdata[2]!=0xFF) ) return false;
    // 检查是否是全 0 的数据段

/**************************************************************************************************/
//经过测试，用secureCRT发送时，结束帧不全为0，因此额外判断
#ifdef ymodem_UseSecureCRT
    if((pdata[3] == 0x00)||(pdata[4] == 0x30)||(pdata[5] == 0x20)||
       (pdata[6] == 0x30)||(pdata[7] == 0x20)||(pdata[8] == 0x30)
	  )
    {
    	return true;
    }
#endif
/**************************************************************************************************/
    for (uint16_t i = 3; i < ymodem_Len128+3; i++)
    {
        if (pdata[i] != 0x00) return false;
    }
    // CRC字段是0x0000（可选判断）
    if (pdata[ymodem_Len128+3] != 0x00 || pdata[ymodem_Len128+4] != 0x00)
        return false;

    return true;
}

/**
 * @brief  ymodem的CRC校验计算
 * @param  pdata：要解析的数据(从此处开始计算)	len：数据大小
 * @return 返回CRC-16 的值
 */
uint16_t ymodem_Crc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0x0000;  // YMODEM初始值通常为0x0000
    uint16_t i;
    while (length--)
    {
        crc ^= (*data++) << 8;
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}
