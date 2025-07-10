#include "UARTConfig.h"
#include "Ymodem.h"

void uartYmodemInit(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(ymodem_UARTHANDLE, ymodem_Info.fileBuffer, sizeof(ymodem_Info.fileBuffer));
    __HAL_DMA_DISABLE_IT(ymodem_hdmarx, DMA_IT_HT);
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart->Instance == USART1)  		//串口IAP
    {
    	ymodem_Info.currentBlockSize =Size;
    	ymodem_Info.uartRecFlag = 1;
        HAL_UARTEx_ReceiveToIdle_DMA(ymodem_UARTHANDLE, ymodem_Info.fileBuffer, sizeof(ymodem_Info.fileBuffer));
        __HAL_DMA_DISABLE_IT(ymodem_hdmarx, DMA_IT_HT);
    }
}


