#ifndef __PROCESS_H__
#define __PROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"
#include "msg_def.h"
#include "bc.h"



#define BC_PRE_SAMPLE_TIME      10000     //预抽样时间 ms
#define BC_STATIC_TIME          3000      //预抽样后，静止时间 ms
#define MENU_LED_LIGHT_SHPERIOD 500       //面板LED闪耀间隔 ms


/*
*   计数模式，信息结构体
*/
typedef __packed struct {
    uint32_t ulCountMode;          //计数模式
    uint32_t ulOutGZ_CountNum;     //执行外部工装测试的测试。1：单次工装测试（带界面交互），>1:多次工装测试（不带界面交互）
    uint32_t ulInGZ_CountNum;      //执行内部工装测试的测试。1：单次工装测试（带界面交互），>1:多次工装测试（不带界面交互）
    
    uint32_t ulReUseMode_Delay;    /*计数池重复使用模式下，执行计数前延时，单位秒，添加这个延时的主要原因是：
                                        高低压测试的时候用，放进设备之后需要有增压时间，要在压强到达指定值后再进行计数才有效，但是增压需要时间，期间不能人为操作。
                                        当延时为0时，则可马上进行计数，用于正常的计数池重复使用。
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


