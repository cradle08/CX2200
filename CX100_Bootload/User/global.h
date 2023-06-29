#ifndef _GLOBAL_H
#define _GLOBAL_H



#include "stm32f4xx.h"





/*宏定义区*********************************************************************************/
#define EEPROM_ADR_UPDATE_MAIN_SOFTWARE			20										//定义EEPROM中用来保存是否升级主程序的标志地址，4字节，为0xA55A0FF0时，表示需要引导程序升级主程序，其他值则不需要
#define UPDATE_MAIN_SOFTWARE_FLAG				0xA55A0FF0								//定义用于确认需要引导程序升级主程序的标志
	
#define SDRAM_SAVE_MAIN_SOFTWARE_OFFSET			10*1024*1024							//定义用于在SDRAM中存放升级包数据的起始偏移量
#define SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET		30*1024*1024							//定义用于在SPI_FLASH中存放升级包数据的起始偏移量

#define FLASH_ADR_ALIGN							512										//定义对齐边界值，这个根据所有的中断向量个数扩大2的整数次幂后*4得出
#define MCU_FLASH_MAIN_SOFTWARE_START_ADR		32*1024									//定义主程序存放在MCU的FLASH中的起始位置的偏移量
#define SRAM_END_ADR							0x20030000								//定义MCU内部的SRAM结束地址，F429内部SRAM除去CCM的容量为192K


/***
*定义保存在SPI FLASH中的主程序升级包数据结构体
***/
typedef struct _UpdateMainSoftwareData_s{
	uint32_t flag;								//用于定义SPI flash中保存的数据是否有效，当为0xA55A0FF0时，表示数据有效，可以往下校验数据，其他值时则直接表示数据无效
	uint32_t totalLen;							//文件总大小
	uint16_t crc16;								//文件CRC校验值
	
	uint8_t data[];								//可变长数组，用于之后存放升级包数据
}UpdateMainSoftwareData_s;










uint16_t CRC16(uint8_t* Buf,uint32_t Len);
ErrorStatus IapJumpIntoApp(void);



#endif
