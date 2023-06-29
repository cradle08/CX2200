#ifndef _PCBA_H
#define _PCBA_H



#include "stm32f4xx_hal.h"




#define PCBA_EVENT_CONFIM_BUTTON_FLAG					1<<0				//PCBA检验事件组中点击了确认按钮事件
#define PCBA_EVENT_NO_BUTTON_FLAG						1<<1				//PCBA检验事件组中点击了NO按钮事件
#define PCBA_EVENT_YES_BUTTON_FLAG						1<<2				//PCBA检验事件组中点击了YES按钮事件
#define PCBA_EVENT_MOTO_TUILI_BUTTON_FLAG				1<<3				//PCBA检验事件组中点击了电机推力按钮事件
#define PCBA_EVENT_MOTO_TIAODONG_BUTTON_FLAG			1<<4				//PCBA检验事件组中点击了电机跳动按钮事件














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
