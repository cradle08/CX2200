#ifndef _EEPROM_H
#define _EEPROM_H


#include "stm32f4xx.h"



#define I2C_SLAVE_ADR_EE24C32					0x50						//����24C32�Ĵӻ������ַ
#define I2C_SLAVE_ADR_EE24C04					0x54						//����24C04�Ĵӻ������ַ

#define MAIN_BORD_EEPROM_24C32					I2C_SLAVE_ADR_EE24C32		//���ذ�ʹ�õ�EEPROM�ͺ�
#define CONN_BORD_EEPROM_24C04					I2C_SLAVE_ADR_EE24C04		//��ʾת�Ӱ�ʹ�õ�EEPROM�ͺ�


/***
*����EEPROM����ö��
***/
typedef enum _EEPROM_Type_e{
	EEPROM_24C02 = 0,
	EEPROM_24C04,
	EEPROM_24C08,
	EEPROM_24C16,
	EEPROM_24C32,
	EEPROM_24C64
}EEPROM_Type_e;



ErrorStatus EEPROMWriteData(EEPROM_Type_e EEPROMType,uint8_t SlaveAdr,uint16_t Reg,uint8_t* Buf,uint16_t Len);
ErrorStatus EEPROMReadData(EEPROM_Type_e EEPROMType,uint8_t SlaveAdr,uint16_t Reg,uint8_t* Buf,uint16_t Len);




#endif
