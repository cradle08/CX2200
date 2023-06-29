#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__



#include "stm32f4xx_hal.h"




/***
*星期枚举
***/
typedef enum _Week_e{
	SUN = 0,
	MON,
	TUE,
	WED,
	THU,
	FRI,
	SAT
}Week_e;




/***
*定义时间参数相关结构体
***/
typedef __packed struct _RTC_HYM8563Info_s{
	__packed struct{
		uint8_t bcd_l:			4;				//秒BCD码低位
		uint8_t bcd_h:			3;				//秒BCD码高位
		uint8_t vl:				1;				//为0表示RTC芯片计时准确，为1表示计时不可信
	}sec;
	
	__packed struct{
		uint8_t bcd_l:			4;
		uint8_t bcd_h:			3;
	}min;
	
	__packed struct{
		uint8_t bcd_l:			4;
		uint8_t bcd_h:			2;
	}hour;
	
	__packed struct{
		uint8_t bcd_l:			4;
		uint8_t bcd_h:			2;
	}day;
	
	Week_e week;
	
	__packed struct{
		uint8_t bcd_l:			4;				
		uint8_t bcd_h:			1;
		uint8_t :				2;
		uint8_t c:				1;				//为0表示当前为20xx年，为1表示当前为19xx年
	}month;
	
	__packed struct{
		uint8_t bcd_l:			4;
		uint8_t bcd_h:			4;
	}year;
}RTC_HYM8563Info_s;






HAL_StatusTypeDef RTC_HYM8563SetTime(RTC_HYM8563Info_s *ptRTC_Hym8563Info);
HAL_StatusTypeDef RTC_HYM8563GetTime(RTC_HYM8563Info_s *ptRTC_Hym8563Info);



HAL_StatusTypeDef RTC_Read(uint8_t *pucaDateTime, uint8_t ucLen);
HAL_StatusTypeDef RTC_Write(uint8_t *pucaDateTime, uint8_t ucLen);

long GetCurrentTimestamp(RTC_HYM8563Info_s RTCHym8563Info);


#endif
