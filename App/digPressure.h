#ifndef _DIGPRESSURE_H
#define _DIGPRESSURE_H



#include "stm32f4xx_hal.h"





/***
*用于标识数字式压力计启动单次还是多次求平均的采集方式的枚举
***/
typedef enum _DigPresureSampleMode_e{
	SINGLE = 0,
	AVG2,
	AVG4,
	AVG8,
	AVG16
}DigPresureSampleMode_e;








uint8_t DigPressure_GetStatus(void);
HAL_StatusTypeDef DigPressureStart(DigPresureSampleMode_e SampleMode);
HAL_StatusTypeDef DigPressure_GetRawValue(uint32_t* RawPressure);
int32_t DigPressure_ConvertValue(uint32_t RawPressure);





#endif
