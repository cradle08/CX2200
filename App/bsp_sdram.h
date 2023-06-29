#ifndef __BSP_SDRAM_H_
#define __BSP_SDRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"


/**********************SDRAM*********************
*
*  SDRAM1:0xC000 0000 - 0xCFFF FFFF, (32M:0x0200 0000)
*  //SDRAM2:0xD000 0000 - 0xDFFF FFFF
*/
#define Bank5_SDRAM_ADDR    ((uint32_t)(0XC0000000)) //SDRAM开始地址
//SDRAM配置参数
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)
  
  
void Bsp_SDRAM_Init(void);
  
  
#ifdef __cplusplus
}
#endif

#endif //__BSP_SDRAM_H_

