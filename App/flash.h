#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f4xx_hal.h"
#include "msg_def.h"

 //扇区2（16k）偏移0x1000作为保存参数起始地址, 即：0x08008000-0x0800c000
#define PARAM_FLASH_BASE 			(0x08008000+0x1000)
#define PARAM_FLASH_MAX_LEN		0x3000							//扇区剩余部分（12k）的	作为保存数据的最大长度
#define FLASH_WAITETIME  			50000          			//FLASH等待超时时间

 

 
uint32_t FLASH_ReadWord(uint32_t ulAddr);		  	//读出字  
void FLASH_Write(uint8_t *pParam, uint32_t ulWriteLen);		//从指定地址开始写入指定长度的数据
void FLASH_Read(uint8_t *pParam, uint32_t ulReadLen);   		//从指定地址开始读出指定长度的数据





//
void Test_Write(uint32_t WriteAddr,uint32_t WriteData);	




HAL_StatusTypeDef FLASH_WriteData_8(uint32_t StartAddress,uint8_t *Data,uint32_t Len);
void FLASH_ReadData_8(uint32_t StartAddress,uint8_t *Data,uint16_t Len);


#endif



