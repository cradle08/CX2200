#ifndef __BACKEND_MSG_H__
#define __BACKEND_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "msg_def.h"
#include "stm32f4xx_hal.h"


/***
*�����������ǰ�˻ش�����ϢԴö��
***/
typedef enum _ServiceBToUIDataSource_e{
	PRESSURE_SELF_CHECK = 0,
	AIR_LIGHT_SELF_CHECK,
	MOTO_SELF_CHECK,
}ServiceBToUIDataSource_e;



/***
*��������������ǰ�˴��ݵ���Ϣ�ṹ��
***/
typedef struct _ServiceBToUIUserData_s{
	ServiceBToUIDataSource_e dataSource;
	uint8_t result;								//Ϊ0��ʾ�ɹ�������ֵ��ʾʧ��
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


