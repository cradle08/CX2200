#ifndef __BACKEND_MSG_H__
#define __BACKEND_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "msg_def.h"
#include "stm32f4xx_hal.h"


/***
*服务界面后端向前端回传的消息源枚举
***/
typedef enum _ServiceBToUIDataSource_e{
	PRESSURE_SELF_CHECK = 0,
	AIR_LIGHT_SELF_CHECK,
	MOTO_SELF_CHECK,
}ServiceBToUIDataSource_e;



/***
*定义服务界面后端向前端传递的消息结构体
***/
typedef struct _ServiceBToUIUserData_s{
	ServiceBToUIDataSource_e dataSource;
	uint8_t result;								//为0表示成功，其他值表示失败
}ServiceBToUIUserData_s;








void Init_NextTest_Info(void);
void Set_NextTest_Info(void);
void Get_NextTest_Info(WBCHGB_TestInfo_t *pMsg);

int BackEnd_Put_Msg(uint8_t *pMsg);
int BackEnd_Get_Msg(uint8_t *pMsg);

static void BackEnd_AnalysisMsg_Handler(uint8_t *pMsg);
static void BackEnd_ListMsg_Handler(uint8_t *pMsg);    
static void BackEnd_SetMsg_Handler(uint8_t *pMsg);
static void BackEnd_ServerMsg_Handler(uint8_t *pMsg);

void BackEnd_Msg_Handler(void);

void Load_QC_WBCHGB_Data(void);



#ifdef __cpluplus
}
#endif

#endif


