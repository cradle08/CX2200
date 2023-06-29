#ifndef _GLOBAL_H
#define _GLOBAL_H



#include "stm32f4xx.h"





/*�궨����*********************************************************************************/
#define EEPROM_ADR_UPDATE_MAIN_SOFTWARE			20										//����EEPROM�����������Ƿ�����������ı�־��ַ��4�ֽڣ�Ϊ0xA55A0FF0ʱ����ʾ��Ҫ����������������������ֵ����Ҫ
#define UPDATE_MAIN_SOFTWARE_FLAG				0xA55A0FF0								//��������ȷ����Ҫ������������������ı�־
	
#define SDRAM_SAVE_MAIN_SOFTWARE_OFFSET			10*1024*1024							//����������SDRAM�д�����������ݵ���ʼƫ����
#define SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET		30*1024*1024							//����������SPI_FLASH�д�����������ݵ���ʼƫ����

#define FLASH_ADR_ALIGN							512										//�������߽�ֵ������������е��ж�������������2���������ݺ�*4�ó�
#define MCU_FLASH_MAIN_SOFTWARE_START_ADR		32*1024									//��������������MCU��FLASH�е���ʼλ�õ�ƫ����
#define SRAM_END_ADR							0x20030000								//����MCU�ڲ���SRAM������ַ��F429�ڲ�SRAM��ȥCCM������Ϊ192K


/***
*���屣����SPI FLASH�е����������������ݽṹ��
***/
typedef struct _UpdateMainSoftwareData_s{
	uint32_t flag;								//���ڶ���SPI flash�б���������Ƿ���Ч����Ϊ0xA55A0FF0ʱ����ʾ������Ч����������У�����ݣ�����ֵʱ��ֱ�ӱ�ʾ������Ч
	uint32_t totalLen;							//�ļ��ܴ�С
	uint16_t crc16;								//�ļ�CRCУ��ֵ
	
	uint8_t data[];								//�ɱ䳤���飬����֮��������������
}UpdateMainSoftwareData_s;










uint16_t CRC16(uint8_t* Buf,uint32_t Len);
ErrorStatus IapJumpIntoApp(void);



#endif
