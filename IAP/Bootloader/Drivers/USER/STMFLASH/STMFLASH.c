#include "STMFLASH.h"

/*	写FLASH操作如下
	1.解锁 Flash： 通过 HAL_FLASH_Unlock() 解锁 Flash，以便进行写入操作。
	2.擦除 Flash 页面： 使用 FLASH_EraseInitTypeDef 结构配置并擦除指定的 Flash 页面。
	3.写入数据： 循环遍历 buf 数组，将数据以双字（8 字节）为单位写入到 Flash。
	4.锁定 Flash： 写入操作完成后，调用 HAL_FLASH_Lock() 锁定 Flash，确保安全性。
 */



/**
 *@功能：从内部Flash读取指定字节数据
 *@参数1：ReadAddress：数据起始地址
 *@参数2：*data：      读取到的数据缓存首地址
 *@参数3：length：     读取字节个数
 */
void STMFLASH_Read(uint32_t ReadAddress, uint8_t *data, uint32_t length)
{
    for(uint32_t i=0;i<length;i++)
    {
        data[i]=*(uint8_t*)(ReadAddress+i); //读取数据
    }
}


//读取指定地址的字(32位数据)
//faddr:读地址
//返回值:对应数据.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
    return *(uint32_t*)faddr;
}

/*
 * 获取某个地址所在的flash扇区
 * addr:flash地址
 *返回值:0~11,即addr所在的扇区
*/
uint8_t STMFLASH_GetFlashSector(uint32_t addr)
{
    if(addr<ADDR_FLASH_SECTOR_1)		return FLASH_SECTOR_0;
    else if(addr<ADDR_FLASH_SECTOR_2)	return FLASH_SECTOR_1;
    else if(addr<ADDR_FLASH_SECTOR_3)	return FLASH_SECTOR_2;
    else if(addr<ADDR_FLASH_SECTOR_4)	return FLASH_SECTOR_3;
    else if(addr<ADDR_FLASH_SECTOR_5)	return FLASH_SECTOR_4;
    else if(addr<ADDR_FLASH_SECTOR_6)	return FLASH_SECTOR_5;
    else if(addr<ADDR_FLASH_SECTOR_7)	return FLASH_SECTOR_6;
    else if(addr<ADDR_FLASH_SECTOR_8)	return FLASH_SECTOR_7;
    else if(addr<ADDR_FLASH_SECTOR_9)	return FLASH_SECTOR_8;
    else if(addr<ADDR_FLASH_SECTOR_10)	return FLASH_SECTOR_9;
    else if(addr<ADDR_FLASH_SECTOR_11)	return FLASH_SECTOR_10;
    return FLASH_SECTOR_11;
}
/**
 * @brief  获取指定扇区的大小（单位：字节）
 * @param  sector: 扇区号（0~11）
 * @retval 扇区大小（单位：字节）
 */
uint32_t STMFLASH_GetSectorSize(uint32_t sector)
{
    if (sector == FLASH_SECTOR_0 || sector == FLASH_SECTOR_1 ||
        sector == FLASH_SECTOR_2 || sector == FLASH_SECTOR_3)
        return 16 * 1024;  // 16KB

    else if (sector == FLASH_SECTOR_4)
        return 64 * 1024;  // 64KB

    else if (sector <= FLASH_SECTOR_11)
        return 128 * 1024; // 128KB
    else
        return 0; // 错误扇区号
}
/** 注意：直接擦除指定地址的扇区，擦除前须小心
 * @brief 直接擦除startAddr-----startAddr+length内的扇区
 */
void STMFLASH_EraseArea(uint32_t startAddr, uint32_t length)
{
    HAL_FLASH_Unlock();
    uint32_t addr = startAddr;
    uint32_t endAddr = startAddr + length;
    uint32_t sectorError;

    FLASH_EraseInitTypeDef FlashEraseInit;
    FlashEraseInit.TypeErase    = FLASH_TYPEERASE_SECTORS;
    FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    FlashEraseInit.NbSectors    = 1;

    while (addr < endAddr)
    {
        FlashEraseInit.Sector = STMFLASH_GetFlashSector(addr);
        HAL_FLASHEx_Erase(&FlashEraseInit, &sectorError);
        addr += STMFLASH_GetSectorSize(FlashEraseInit.Sector);
    }
    HAL_FLASH_Lock();
}


/**
 * @brief 向STM32 Flash写入数据（不带擦除）
 * @param WriteAddress 写入起始地址
 * @param data         要写入的数据指针（字节）
 * @param length       写入长度（字节数）
 * @note Flash必须事先擦除，否则写入可能失败！
 */
void STMFLASH_Write_NoErase(uint32_t WriteAddress, const uint8_t *data, uint32_t length)
{
    if (data == NULL || length == 0) return;

    HAL_FLASH_Unlock();  // 解锁Flash

    for (uint32_t i = 0; i < length; i++)
    {
        // 写入前检查目标地址是否为0xFF，否则写入失败
        if (*(uint8_t*)WriteAddress != 0xFF)
        {
            // 此处可以选择报错或跳过，当前选择跳过该字节
            WriteAddress++;
            continue;
        }

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, WriteAddress, data[i]) != HAL_OK)
        {
            // 写入失败，退出
            break;
        }
        WriteAddress++;
    }
    HAL_FLASH_Lock();  // 上锁Flash
}


/** 注意：每次都会检测是否有不为0xFF的，所以需要先保留好必要的数据，再调用此函数
 * @brief 在FLASH 指定位置写入指定长度的数据（自动擦除，4字节对齐方式）
 * @param WriteAddress 要写入的起始地址（必须4字节对齐）
 * @param data 要写入的数据指针
 * @param length 要写入的字节数（不必4字节对齐，内部自动补齐）
 */
void STMFLASH_Write(uint32_t WriteAddress, uint8_t *data, uint32_t length)
{
    FLASH_EraseInitTypeDef FlashEraseInit;
    HAL_StatusTypeDef FlashStatus=HAL_OK;
    uint32_t SectorError=0;
    uint32_t addrx=0;
    uint32_t endaddr=0;

    if( (WriteAddress < STM_FLASH_BASE) || ( WriteAddress + length >= STM_FLASH_END) || (length <= 0) )
    return;

    HAL_FLASH_Unlock();              //解锁
    addrx = WriteAddress;            //写入的起始地址
    endaddr = WriteAddress+length;   //写入的结束地址

	while(addrx<endaddr)  //扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
	{
		if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
		{
			FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //擦除类型，扇区擦除
			FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);   //要擦除的扇区
			FlashEraseInit.NbSectors=1;                             //一次只擦除一个扇区
			FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //电压范围，VCC=2.7~3.6V之间!!
			if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK)
			{
				break;//发生错误了
			}
		}
		else
		{
			addrx+=4;
		}
		FLASH_WaitForLastOperation(FLASH_WAITETIME);                //等待上次操作完成
	}//读取要擦除扇区是否存在有效数据

    FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);            //等待上次操作完成
    if(FlashStatus==HAL_OK)
    {
         while(WriteAddress<endaddr)//写数据
         {
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,WriteAddress,*data)!=HAL_OK)//写入数据
            {
                break;	//写入异常
            }
            WriteAddress+=1;
            data+=1;
        }
    }
    HAL_FLASH_Lock();           //上锁
}






