#ifndef __BSP_BEEP_H_
#define __BSP_BEEP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"
  
void Beep_Init(void);
void Beep_Start(void);
void Beep_Stop(void);
void Beep_SetFreq(uint16_t usPsc, uint16_t usArr);
void Beep_Period(uint8_t ucNum, uint32_t ulPeriod);
void Beep_Exec(Bool eFlag);
  
  
  
  
  
  
  
  
  
  
  
  
  
  
#ifdef __cplusplus
}
#endif

#endif //__BSP_PUMP_H_


