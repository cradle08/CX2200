#ifndef _EEPROM_H
#define _EEPROM_H


#include "stm32f4xx.h"



#define I2C_SLAVE_ADR_EE24C32					0x50						//定义24C32的从机物理地址
#define I2C_SLAVE_ADR_EE24C04					0x54						//定义24C04的从机物理地址

#define MAIN_BORD_EEPROM_24C32					I2C_SLAVE_ADR_EE24C32		//主控板使用的EEPROM型号
#define CONN_BORD_EEPROM_24C04					I2C_SLAVE_ADR_EE24C04		//显示转接板使用的EEPROM型号


/***
*定义EEPROM类型枚举
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
