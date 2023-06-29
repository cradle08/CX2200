#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__



#include "stm32f4xx_hal.h"




/***
*����ö��
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
*����ʱ�������ؽṹ��
***/
typedef __packed struct _RTC_HYM8563Info_s{
	__packed struct{
		uint8_t bcd_l:			4;				//��BCD���λ
		uint8_t bcd_h:			3;				//��BCD���λ
		uint8_t vl:				1;				//Ϊ0��ʾRTCоƬ��ʱ׼ȷ��Ϊ1��ʾ��ʱ������
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
		uint8_t c:				1;				//Ϊ0��ʾ��ǰΪ20xx�꣬Ϊ1��ʾ��ǰΪ19xx��
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
