#ifndef __PROCESS_H__
#define __PROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"
#include "msg_def.h"
#include "bc.h"



#define BC_PRE_SAMPLE_TIME      10000     //Ԥ����ʱ�� ms
#define BC_STATIC_TIME          3000      //Ԥ�����󣬾�ֹʱ�� ms
#define MENU_LED_LIGHT_SHPERIOD 500       //���LED��ҫ��� ms


/*
*   ����ģʽ����Ϣ�ṹ��
*/
typedef __packed struct {
    uint32_t ulCountMode;          //����ģʽ
    uint32_t ulOutGZ_CountNum;     //ִ���ⲿ��װ���ԵĲ��ԡ�1�����ι�װ���ԣ������潻������>1:��ι�װ���ԣ��������潻����
    uint32_t ulInGZ_CountNum;      //ִ���ڲ���װ���ԵĲ��ԡ�1�����ι�װ���ԣ������潻������>1:��ι�װ���ԣ��������潻����
    
    uint32_t ulReUseMode_Delay;    /*�������ظ�ʹ��ģʽ�£�ִ�м���ǰ��ʱ����λ�룬��������ʱ����Ҫԭ���ǣ�
                                        �ߵ�ѹ���Ե�ʱ���ã��Ž��豸֮����Ҫ����ѹʱ�䣬Ҫ��ѹǿ����ָ��ֵ���ٽ��м�������Ч��������ѹ��Ҫʱ�䣬�ڼ䲻����Ϊ������
                                        ����ʱΪ0ʱ��������Ͻ��м��������������ļ������ظ�ʹ�á�
                                   */
}CountMode_t;


void CountMode_Init(void);
__IO CountMode_t* Get_CountMode_Info(void);
CountMode_e Set_CountMode(CountMode_e eCountMode);


ErrorCode_e Count_Exec(TestProject_e eTestProject, __IO CountMode_e eCountMode);
ErrorCode_e QC_Count_Exec(TestProject_e eTestProject, __IO CountMode_e eCountMode);
ErrorCode_e Count_Process(TestProject_e eTestProject, __IO CountMode_e eCountMode, __IO CountInfo_t *pCountInfo, __IO uint8_t *pDataBuffer, uint16_t usBufferLen);
ErrorCode_e HGB_Process(void);

ErrorCode_e Air_Bleed(void);
FeedBack_e Calibration_Msg_Handler(uint8_t *pMsg);
void PowerOff_Exec(uint8_t *pMsg);
FeedBack_e Login_SelfCheck(uint16_t usCmd);
ErrorType_e ErrorCode_Type(ErrorCode_e eErrorCode);
FeedBack_e Debug_Msg_Handler(uint8_t *pMsg);

void Load_Account_Data(void);
ErrorCode_e HGB_Process_Test(void);












#ifdef CX_DEBUG


#endif





  
#ifdef __cplusplus
}
#endif

#endif //__PROCESS_H__


