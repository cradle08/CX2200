#ifndef __BSP_LIGHT_H__
#define __BSP_LIGHT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "msg_def.h"


#define HGB_LIGHT_DAC       1400    //�޸�ʱ��20210721��v0.1.5   //��ǰDAC1��Ĭ���趨ֵ������HGB����Դ��DACֵԽ������ĺ���ԴԽ��
#define HGB_LIGHT_DAC_MAX   2000     //��ǰDAC1��Ĭ���趨ֵ�� ���ֵ������HGB����Դ��DACֵԽ������ĺ���ԴԽ��



/***
*����Ҫ������HGB������ö��
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


