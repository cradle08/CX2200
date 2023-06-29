#ifndef __BSP_COUNT_SIGNAL_H__
#define __BSP_COUNT_SIGNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

  
void Count_Signal_Init(void);
void Count_Signal_Start(void);
void Count_Signal_Stop(void);
void Count_Signal_SetFreq(float fRatio);
  
  

  
  
  
  
  
  
  
  
  
  
  
  
  
  
#ifdef __cplusplus
}
#endif

#endif //__BSP_SIMULATION_H__


