#ifndef _GUI_TOUCH_H
#define _GUI_TOUCH_H



#include "stm32f4xx_hal.h"




















void GUI_TOUCH_X_ActivateX(void);
void GUI_TOUCH_X_ActivateY(void);
int GUI_TOUCH_X_MeasureX(void);
int GUI_TOUCH_X_MeasureY(void);

void GUI_TouchCalibrate(uint8_t SaveFlag,uint8_t CalibrateFlag);


void Touch_Adjust(void);
void Touch_Scan(void);


#endif
