#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "msg_def.h"
#include "CAT_iot.h"


Bool Is_Display_Data(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);


//
void Set_Default_DataManage(__IO DataManage_t *ptDataManage);
void Set_Default_WBCHGB_Test_Info(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);
void Set_Default_ListView_Data(void);
void Set_Default_QC_WBCHGB_Data(void);


//
void Print_WBCHGB_TestInfo(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);
void Print_NextSample(__IO NextSample_t *ptNextSample);
void Print_Count_Head(CountHead_t *ptCountHead);
//
AccountDataType_e Get_AccountDataType(AccountType_e eAccountType);
void Update_WBCHGB_DataManage_LastestDateTime(void);

Bool WBCHGB_Index_Is_Normal(uint32_t ulIndex);
//样本数据
uint32_t Get_WBCHGB_Head(void);
uint32_t Set_WBCHGB_Head(uint32_t ulIndex);
uint32_t Add_WBCHGB_Head(void);
uint32_t Update_WBCHGB_Head(void);
//
uint32_t Get_WBCHGB_Tail(void);
uint32_t Set_WBCHGB_Tail(uint32_t ulIndex);
uint32_t Add_WBCHGB_Tail(void);
uint32_t Update_WBCHGB_Tail(void);
uint32_t Get_WBCHGB_BeforeTail(void);
//
Bool WBCHGB_SendIndex_Equal_HeadIndex(void);
uint32_t Get_WBCHGB_SendIndex(CAT_SERVICE_e CatService);
uint32_t Add_WBCHGB_SendIndex(CAT_SERVICE_e CatService);
uint32_t Set_WBCHGB_SendIndex(CAT_SERVICE_e CatService,uint32_t ulIndex);
//
uint32_t Get_WBCHGB_LastestSN(void);
uint32_t Add_WBCHGB_LastestSN(void);
//
void Update_WBCHGB_DataManage(void);
uint32_t Get_WBCHGB_ValidNum(void);
uint32_t Dec_WBCHGB_ValidNum(void);
uint32_t Add_WBCHGB_ValidNum(void);
uint32_t Get_WBCHGB_Queue_MaxNum(void);


//运行日志
void RunLog_DataManage_Init(void);
uint32_t Get_RunLog_Head(void);
uint32_t Get_RunLog_Tail(void);
uint32_t Update_RunLog_Tail(void);
uint32_t Add_RunLog_Tail(void);
uint32_t Update_RunLog_SendIndex(void);
uint32_t Add_RunLog_SendIndex(void);
uint32_t RunLog_Num(void);

//质控,(双向链表，循环)
uint32_t Get_QC_ValidNum(QC_FILENUM_INDEX_e eFileNumIndex);
uint32_t Get_QC_LastIndex(QC_FILENUM_INDEX_e eFileNumIndex, uint32_t ulCurIndex);
uint32_t Get_QC_TailIndex(QC_FILENUM_INDEX_e eFileNumIndex);
uint32_t Set_QC_TailIndex(QC_FILENUM_INDEX_e eFileNumIndex, uint32_t ulIndex);
uint32_t Add_QC_TailIndex(QC_FILENUM_INDEX_e eFileNumIndex);
uint32_t Add_QC_ValidNum(QC_FILENUM_INDEX_e eFileNumIndex);
//
void Machine_Info_Init(MachInfo_s *ptMachineInfo);
void UI_Info_Init(__IO MachRunPara_s *ptUI_Info,MachInfo_s *ptMachineInfo, __IO DataManage_t *pDataMange);
void NextSample_Info_Init(__IO NextSample_t *ptNextSample, __IO DataManage_t *ptDataManage);
void Update_NexSample_Info(Bool eBool,__IO NextSample_t *ptNextSample, __IO DataManage_t *ptDataManage);
void Set_NexSample_Info(NextSample_t *ptNextSample, NextSample_t *ptNextSample_UI);
void Get_NexSample_Info(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo, __IO NextSample_t *ptNextSample);



#ifdef __cplusplus
}
#endif

#endif //__FILE_OPERATE_H__

