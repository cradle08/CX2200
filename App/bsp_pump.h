#ifndef __BSP_PUMP_H_
#define __BSP_PUMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

  
void Pump_Init(void);
void Pump_Start(void);
void Pump_Stop(void);
void Pump_SetRatio(uint16_t ucRatio);
  
HAL_StatusTypeDef ParaSet_AutoPumpAdjust(uint8_t* PumpResult);
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
#ifdef __cplusplus
}
#endif

#endif //__BSP_PUMP_H_


