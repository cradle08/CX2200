#ifndef __BSP_LIGHT_H__
#define __BSP_LIGHT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "msg_def.h"


#define HGB_LIGHT_DAC       1400    //修改时间20210721，v0.1.5   //当前DAC1的默认设定值，驱动HGB恒流源，DAC值越大，输出的恒流源越大
#define HGB_LIGHT_DAC_MAX   2000     //当前DAC1的默认设定值的 最大值，驱动HGB恒流源，DAC值越大，输出的恒流源越大



/***
*定义要开启的HGB灯类型枚举
***/
typedef enum _HGBLed_e{
	HGBLED_535 = 0,
	HGBLED_415
}HGBLed_e;
















void Light_Set_DAC(uint16_t usDAC);
void Light_Start(HGBLed_e HGBLed,uint16_t DACValue);
void Light_Stop(void);
void Light_Set_Current(uint16_t usCurrent);
HAL_StatusTypeDef Light_OpenIsTimeup(uint32_t Time);
  
  
ErrorCode_e HGB_Gran_Auto_Adjust(uint16_t usDstADC, uint8_t *pDR, uint16_t *pusHGB_ADC_Ave);
ErrorCode_e HGB_Current_Auto_Adjust(uint16_t usDstADC, uint8_t *pDR, uint16_t *pusHGBLED_Current, uint16_t *pusHGB_ADC_Ave);
#ifdef __cplusplus
}
#endif

#endif //


