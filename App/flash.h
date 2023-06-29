#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f4xx_hal.h"
#include "msg_def.h"

 //����2��16k��ƫ��0x1000��Ϊ���������ʼ��ַ, ����0x08008000-0x0800c000
#define PARAM_FLASH_BASE 			(0x08008000+0x1000)
#define PARAM_FLASH_MAX_LEN		0x3000							//����ʣ�ಿ�֣�12k����	��Ϊ�������ݵ���󳤶�
#define FLASH_WAITETIME  			50000          			//FLASH�ȴ���ʱʱ��

 

 
uint32_t FLASH_ReadWord(uint32_t ulAddr);		  	//������  
void FLASH_Write(uint8_t *pParam, uint32_t ulWriteLen);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void FLASH_Read(uint8_t *pParam, uint32_t ulReadLen);   		//��ָ����ַ��ʼ����ָ�����ȵ�����





//
void Test_Write(uint32_t WriteAddr,uint32_t WriteData);	




HAL_StatusTypeDef FLASH_WriteData_8(uint32_t StartAddress,uint8_t *Data,uint32_t Len);
void FLASH_ReadData_8(uint32_t StartAddress,uint8_t *Data,uint16_t Len);


#endif



