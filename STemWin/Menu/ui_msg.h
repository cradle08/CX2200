#ifndef __UI_MSG_H__
#define __UI_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"



static void UI_AnalysisMsg_Handler(uint8_t *pMsg);
static void UI_ListMsg_Handler(uint8_t *pMsg);
static void UI_SetMsg_Handler(uint8_t *pMsg);
void UI_Msg_Handler(void);


int UI_Put_Msg(uint8_t *pMsg);
int UI_Get_Msg(uint8_t *pMsg);



  
#ifdef __cplusplus
}
#endif

#endif //__DEVICEHANDLER_H_




