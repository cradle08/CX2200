#ifndef __UNICODE2GBK_H__
#define __UNICODE2GBK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"

 
void UTFToUnicode(uint16_t *pUncodeBuffer, uint8_t *pUTF8Buffer);
void UnicodeToGBK(uint16_t *pGBK, uint16_t *pUncodeBuffer);

  
#ifdef __cplusplus
}
#endif




#endif //__UNICODE2GBK_H__


