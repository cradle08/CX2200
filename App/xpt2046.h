#ifndef _XPT2046_H
#define _XPT2046_H


#include "stm32f4xx_hal.h"







//����PENIRQ���뼤��״̬�Ϳ���״ֵ̬
#define PENIRQ_ACTIVE							0
#define PENIRQ_IDLE								1


/***
*У׼ת������
***/
typedef struct _TouchAdjustPara_s{
	uint8_t flag;
	float a1;
	float b1;
	float c1;
	
	float a2;
	float b2;
	float c2;
    
}TouchAdjustPara_s;


/***
*����ṹ��
***/
typedef struct _TouchPos_s{
	int16_t x;
	int16_t y;
}TouchPos_s;













void PenIRQInputStateScanning(void);
uint8_t PenIRQ_GetState(void);

void XPT2046_Init(void);
ErrorStatus XPT2046_GetRawPosValue(TouchPos_s* TouchRawPos);
ErrorStatus Touch_GetPixelPosValue(TouchPos_s* TouchPixelPos);




#endif

