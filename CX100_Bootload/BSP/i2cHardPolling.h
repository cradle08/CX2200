#ifndef _I2CHARDPOLLING_H
#define _I2CHARDPOLLING_H



#include "stm32f4xx.h"







void I2C_Config(void);
void I2C_Reset(void);
ErrorStatus I2CWriteData(uint8_t SlaveAdr,uint16_t Reg,uint8_t RegLen,uint8_t* Buf,uint16_t Len);
ErrorStatus I2CReadData(uint8_t SlaveAdr,uint16_t Reg,uint8_t RegLen,uint8_t* Buf,uint16_t Len);
ErrorStatus RequestEEPROMAck(uint8_t SlaveAdr);



#endif
