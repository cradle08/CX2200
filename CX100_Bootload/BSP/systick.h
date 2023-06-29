#ifndef _SYSTICK_H
#define _SYSTICK_H


#include "stm32f4xx.h"









void Systick_InitConfig(void);
void Delay_MS(uint32_t Time);
FlagStatus Time_MS(uint32_t FirstStamp,uint32_t Time);
uint32_t Get_Stamp(void);





#endif 
