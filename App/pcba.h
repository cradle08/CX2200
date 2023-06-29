#ifndef _PCBA_H
#define _PCBA_H



#include "stm32f4xx_hal.h"




#define PCBA_EVENT_CONFIM_BUTTON_FLAG					1<<0				//PCBA�����¼����е����ȷ�ϰ�ť�¼�
#define PCBA_EVENT_NO_BUTTON_FLAG						1<<1				//PCBA�����¼����е����NO��ť�¼�
#define PCBA_EVENT_YES_BUTTON_FLAG						1<<2				//PCBA�����¼����е����YES��ť�¼�
#define PCBA_EVENT_MOTO_TUILI_BUTTON_FLAG				1<<3				//PCBA�����¼����е���˵��������ť�¼�
#define PCBA_EVENT_MOTO_TIAODONG_BUTTON_FLAG			1<<4				//PCBA�����¼����е���˵��������ť�¼�














void PCBA_MainBoard(void);
void PCBA_ZJBoard(void);
void PCBA_CAT(void);
void PCBA_TouchScreen(void);
void PCBA_PressureAdjust(void);
void PCBA_Pressure(uint8_t SensorType);
void PCBA_Valve(void);
void PCBA_Pump(void);
void PCBA_Moto(void);

void TempTask_AutoPowerOnoffCheck(void *argument);



#endif
