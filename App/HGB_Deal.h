#ifndef _HGB_DEAL_H
#define _HGB_DEAL_H




#include "stm32f4xx_hal.h"





















uint16_t Get_HGBAvgADC(uint16_t Time);

void HGBAdjust_HGBTest(void);
void HGBAdjust_Open415(void);
void HGBAdjust_Open535(void);
void HGBAdjust_KQAuto(void);
void HGBAdjust_KBAuto(void);




#endif
