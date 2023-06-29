#ifndef __TP_H__
#define __TP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"

 


  
void TP_Init(void);
void TP_Scan(void);



int TP_Read_X(void);
int TP_Read_Y(void);





  
#ifdef __cplusplus
}
#endif




#endif //__TP_H__


