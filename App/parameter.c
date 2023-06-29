
#include "main.h"
#include "parameter.h"
#include "file_operate.h"
#include "backend_msg.h"
#include "bsp_rtc.h"
#include "bsp_eeprom.h"
#include "printerTask.h"
#include "process.h"
#include "stdarg.h"
#include "time.h"
#include "bsp_light.h"
#include "CAT_iot.h"
#include "bsp_resistance.h"
#include "Common.h"

/*
*   是否，是可以显示的数据(用于样本分析、列表回顾界面)
*/
Bool Is_Display_Data(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo)
{
    Bool eBool = FALSE;
    //正常、测试无效的数据
    if(ptWBCHGB_TestInfo->eDataStatus == DATA_STATUS_NORMAL || ptWBCHGB_TestInfo->eDataStatus == DATA_STATUS_COUNT_INVALID)
    {
        eBool = TRUE;
    }else{
        eBool = FALSE;
    }
    
    //
    return eBool;
}




/*
*   根据账号，获取账号数据类型
*/
AccountDataType_e Get_AccountDataType(AccountType_e eAccountType)
{
    AccountDataType_e eAccountDataType;
    
    if(ACCOUNT_TYPE_SUPER == eAccountType)
    {
        eAccountDataType = ACCOUNT_DATA_TYPE_INNER;
    }else{
        eAccountDataType = ACCOUNT_DATA_TYPE_OUTSIDE;
    }
    
    return eAccountDataType;
}






/*
*	 数据管理信息 信息
*/
void Set_Default_DataManage(__IO DataManage_t *ptDataManage)   	
{
    QC_FILENUM_INDEX_e eFileNumIndex = 0;
	memset((void*)ptDataManage, 0, sizeof(DataManage_t));
	
	printf("DataManage_t size:%u\r\n",sizeof(DataManage_t));
	
    //WBC HGB,外部账号数据管理
    ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].ulLastestSN 	  = INVALID_DATA_SN;	  //第一条记录（样本测试结果）系列号
	ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].ulQueue_HeadIndex = 0;//INVALID_DATA_SN;	  //最后一条记录（样本测试结果）系列号
    ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].ulQueue_TailIndex = 0;//INVALID_DATA_SN;	  //最后一条记录（样本测试结果）系列号
    ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].ccSendIndex       = 0;//INVALID_DATA_SN;	  //最后一条记录（样本测试结果）系列号
    ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].ulValidNum        = 0;
    
    //WBC HGB,外部账号数据管理
    ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ulLastestSN 	    = INVALID_DATA_SN;	  //第一条记录（样本测试结果）系列号
	ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ulQueue_HeadIndex = 0;//INVALID_DATA_SN;	  //最后一条记录（样本测试结果）系列号
    ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ulQueue_TailIndex = 0;//INVALID_DATA_SN;	  //最后一条记录（样本测试结果）系列号
    ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ccSendIndex       = 0;//INVALID_DATA_SN;	  //最后一条记录（样本测试结果）系列号
    ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ulValidNum        = 0;
 
    //运行日志
    ptDataManage->ulRunLog_SN                  = 0;    //运行日志的系列号,从零开始递增           
    ptDataManage->ulRunLog_Queue_HeadIndex     = 0;    //循环队列头指针
    ptDataManage->ulRunLog_Queue_TailIndex     = 0;    //循环队列尾指针
    ptDataManage->ulRunLog_SendIndex           = 0;    //运行日志队列，下一条需要发送日志的索引号
    //质控数据管理
    for(eFileNumIndex = QC_FILENUM_INDEX_0; eFileNumIndex < QC_FILENUM_INDEX_END; eFileNumIndex++)
    {
        ptDataManage->tDataManage_QC[eFileNumIndex].ulQC_SN              = INVALID_DATA_SN;
        ptDataManage->tDataManage_QC[eFileNumIndex].ulQC_Queue_HeadIndex = 0;
        ptDataManage->tDataManage_QC[eFileNumIndex].ulQC_Queue_TailIndex = 0;
        ptDataManage->tDataManage_QC[eFileNumIndex].ulValidNum           = 0;
    }
  
    //设置初始时间
    RTC_Read((uint8_t*)ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ucaLastestDateTime, DATE_TIME_LEN);
    strncpy((char*)ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].ucaLastestDateTime, (char*)ptDataManage->tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ucaLastestDateTime, DATE_TIME_LEN);
}




/*
*   打印计数信息头
*/
void Print_Count_Head(CountHead_t *ptCountHead)
{
    LOG_Info("SN=%s, Data=%s \r\n ErrCode=%d, HideErrCode=%d, ErrType=%d, DataStatus=%d, Account=%d", ptCountHead->ucaSampleSN, ptCountHead->ucaDateTime,\
                              ptCountHead->eErrorCode, ptCountHead->eHideErrorCode, ptCountHead->errType, ptCountHead->eStatus, ptCountHead->eAccountType);
    LOG_Info("CurSN=%d, CurIndex=%d, LastValidIndex=%d, NextValidIndex=%d, CRC=%x", ptCountHead->ulCurSN,ptCountHead->ulCurIndex,\
                                     ptCountHead->ulLastValidIndex, ptCountHead->ulNextValidIndex, ptCountHead->usCrc);

}




/*
*	 设置默认WBC HGB测试结果信息
*/
void Set_Default_WBCHGB_Test_Info(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo)   	
{
	memset((void*)ptWBCHGB_TestInfo, 0, sizeof(WBCHGB_TestInfo_t));
    ptWBCHGB_TestInfo->tMsgHead.eErrorCode      = ERROR_CODE_SUCCESS;
    
    ptWBCHGB_TestInfo->eDataStatus              = DATA_STATUS_INIT;
    ptWBCHGB_TestInfo->ulCurDataSN              = INVALID_DATA_SN;
    ptWBCHGB_TestInfo->ucaEndFlag[0]            = '\r';
    ptWBCHGB_TestInfo->ucaEndFlag[1]            = '\n';
}



/*
*   打印 WBC HGB测试结果信息
*/
void Print_WBCHGB_TestInfo(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo)
{
    uint16_t i = 0;
    
    LOG_Info("WBCHGB_TestInfo: \r\nMsgStatus=%d, MsgStatus=%d, MsgLen=%d,\
               eDataStatus=%d, ulCurDataSN=%d, ucaSampleSN=%s,\
               ucaName=%s, ucaBirthDay=%s, eSex=%d,\
               ucAge=%d, eAgeUnit=%d, eReferGroup=s%d,\
               eTestMode=%d, ucaDateTime=%s, Crc=%x\
                ucaRemark=%s", ptWBCHGB_TestInfo->tMsgHead.usCmd, ptWBCHGB_TestInfo->tMsgHead.eErrorCode, ptWBCHGB_TestInfo->tMsgHead.usMsgLen,\
        ptWBCHGB_TestInfo->eDataStatus, ptWBCHGB_TestInfo->ulCurDataSN, ptWBCHGB_TestInfo->ucaSampleSN,\
        ptWBCHGB_TestInfo->ucaName, ptWBCHGB_TestInfo->ucaBirthDay, ptWBCHGB_TestInfo->eSex,\
        ptWBCHGB_TestInfo->ucAge, ptWBCHGB_TestInfo->eAgeUnit, ptWBCHGB_TestInfo->eReferGroup,\
        ptWBCHGB_TestInfo->eTestMode, ptWBCHGB_TestInfo->ucaDateTime,\
        ptWBCHGB_TestInfo->usCrc, ptWBCHGB_TestInfo->ucaRemark);

    //直方图
    for(i = 0; i < GRAPH_DATA_LEN; i++)
    {
        printf("[%03d]=%0.2f\t", i, ptWBCHGB_TestInfo->fWBCHGB_RstData[i]);
        if(i%4 == 0) LOG_Info("");
    }
    LOG_Info("");
    
    //WBC HGB结果信息
     LOG_Info("WBCHGB:");
    for(i = 0; i < WBCHGB_RST_END; i++)
    {
        LOG_Info("%0.2f\t",  ptWBCHGB_TestInfo->fWBCHGB_RstData[i]);
    }
    LOG_Info("");
    
    //分隔线
    LOG_Info("Splite Line:");
    for(i = 0; i < 3; i++)
    {
        LOG_Info("%0.2f\t",  ptWBCHGB_TestInfo->fLines[i]);
    }
    LOG_Info("MaxPos=%0.2f", ptWBCHGB_TestInfo->fWbcMaxPos);
    //提示信息
    LOG_Info("ucaWBC_ErrorPrompt=%s", ptWBCHGB_TestInfo->ucaWBC_ErrorPrompt);
    LOG_Info("ucaHGB_ErrorPrompt=%s", ptWBCHGB_TestInfo->ucaHGB_ErrorPrompt);
    //end flag
    LOG_Info("%s", ptWBCHGB_TestInfo->ucaEndFlag);
}



/*
*	 打印，下一样本信息
*/
void Print_NextSample(__IO NextSample_t *ptNextSample)
{		
    LOG_Info("NextSample: ulNextDataSN=%d, ucaNextSampleSN=%s, ucaName=%s,\
        eSex=%d, ucAge=%d, eAgeUnit=%d,\
        ucaBirthDay=%s, eReferGroup=%d, ucaRemark=%s", \
    ptNextSample->ulNextDataSN, ptNextSample->ucaNextSampleSN, ptNextSample->ucaName,\
    ptNextSample->eSex, ptNextSample->ucAge, ptNextSample->eAgeUnit,\
    ptNextSample->ucaBirthDay, ptNextSample->eReferGroup, ptNextSample->ucaRemark);
}





/*
*	 获取样本编号，自动曾一, "CBC-00001" 该编号从1开始
*/
void Get_SampleSN(__IO uint8_t *pucNextSampleSN, uint32_t ulDataSN)
{
   sprintf((char*)pucNextSampleSN, "%s%05d", SAMPLE_SN_PREFIX, ulDataSN);
}



/*
*	初始化下一样本信息，该函数上电后调用，根据当前 DataManage_t 信息生成，
*/
void NextSample_Info_Init(__IO NextSample_t *ptNextSample, __IO DataManage_t *ptDataManage)
{
    uint32_t ulLastestSN = INVALID_DATA_SN;
    
    //
    memset((void*)ptNextSample, 0, sizeof(NextSample_t));
    
    //
    ulLastestSN = Get_WBCHGB_LastestSN();
    if(ulLastestSN == INVALID_DATA_SN)
    {
        //无测试数据, 
        ptNextSample->ulNextDataSN = 0;	 //数据系列号从0开始
        Get_SampleSN(ptNextSample->ucaNextSampleSN, ptNextSample->ulNextDataSN + 1); //样本系列号从CBC-00001开始，数据系列号从0开始
        return;        
    }
    
	//样本系列号从CBC-00001开始，数据系列号从0开始
	ptNextSample->ulNextDataSN = ulLastestSN;
	//生成下一样本编号
    Get_SampleSN(ptNextSample->ucaNextSampleSN, ptNextSample->ulNextDataSN + 1); //样本系列号从CBC-00001开始，数据系列号从1开始
}



/*
* 根据操作流程，样本测试完成后，根据当前样本信息，自动生成下一样本信息（这里只更新，样本编号和数据系列号）
*
*   根据用户是否重新测试：eBool:TRUE-清理下一样本信息，FALSE-保持下一样本的输入信息（但是会更新编号）
*/
void Update_NexSample_Info(Bool eBool, __IO NextSample_t *ptNextSample, __IO DataManage_t *ptDataManage)
{
    uint32_t ulLastestSN = INVALID_DATA_SN;
    
    if(TRUE == eBool)
    {
        //清零下一样本信息
        memset((void*)ptNextSample, 0, sizeof(NextSample_t));
    }
    
	//数据系列号
    ulLastestSN = Get_WBCHGB_LastestSN();
	ptNextSample->ulNextDataSN = ulLastestSN; //数据系列号从0开始
	//生成下一样本编号 
	Get_SampleSN(ptNextSample->ucaNextSampleSN, ptNextSample->ulNextDataSN + 1);    //样本系列号从1开始（CBC-00001）
}



/*
*	获取下一样本信息，赋值给WBCHGB结果信息结构体
*/
void Get_NexSample_Info(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo, __IO NextSample_t *ptNextSample)
{

    //更新下一个数据系列号，通过数据管理来赋值
    ptWBCHGB_TestInfo->ulCurDataSN = ptNextSample->ulNextDataSN;
    strncpy((char*)ptWBCHGB_TestInfo->ucaSampleSN, (char*)ptNextSample->ucaNextSampleSN, SAMPLE_SN_LEN);
  
    //赋值下一样本信息
    strncpy((char*)ptWBCHGB_TestInfo->ucaName,	(char*)ptNextSample->ucaName, PATIENT_NAME_LEN);
	strcpy((char*)ptWBCHGB_TestInfo->patientID,(char*)ptNextSample->patientID);
	strcpy((char*)ptWBCHGB_TestInfo->orderNum,(char*)ptNextSample->orderNum);
    
    ptWBCHGB_TestInfo->ucAge 				= ptNextSample->ucAge;
    ptWBCHGB_TestInfo->eAgeUnit 		    = ptNextSample->eAgeUnit;
    ptWBCHGB_TestInfo->eSex 				= ptNextSample->eSex;
    //
    strncpy((char*)ptWBCHGB_TestInfo->ucaBirthDay, (char*)ptNextSample->ucaBirthDay, PATIENT_BIRTHE_DAY_MAX_LEN);
    ptWBCHGB_TestInfo->eReferGroup 		    = ptNextSample->eReferGroup;
    strncpy((char*)ptWBCHGB_TestInfo->ucaRemark, (char*)ptNextSample->ucaRemark, REMARK_LEN);
	ptWBCHGB_TestInfo->eTestMode 			= ptNextSample->eTestMode;
}



/*
*   列表回顾数据
*/
void Set_Default_ListView_Data(void)
{
    extern __IO MachRunPara_s MachRunPara;
    uint8_t i= 0;
    
    for(i = 0; i < LIST_REVIEW_ROW_NUM; i++)
    {
        memset((void*)&MachRunPara.taListView_Data[i], 0, sizeof(WBCHGB_TestInfo_t));
        MachRunPara.taListView_Data[i].ulCurDataSN = INVALID_DATA_SN;
        MachRunPara.taListView_Data[i].ulCurDataIndex = INVALID_DATA_SN;
        MachRunPara.taListView_Data[i].eDataStatus = DATA_STATUS_INIT;
    }   
}



/*
*   质控、界面数据
*/
void Set_Default_QC_WBCHGB_Data(void)
{
    extern __IO MachRunPara_s MachRunPara;
    
    //质控、样本分析
    memset((void*)&MachRunPara.tQC_WBCHGB_TestInfo, 0, sizeof(QC_WBCHGB_TestInfo_t));
    MachRunPara.tQC_WBCHGB_TestInfo.ulCurDataSN = INVALID_DATA_SN;
    MachRunPara.tQC_WBCHGB_TestInfo.ulCurDataIndex = INVALID_DATA_SN;
    MachRunPara.tQC_WBCHGB_TestInfo.eDataStatus = DATA_STATUS_INIT;
    
    //质控列表回顾
    memset((void*)&MachRunPara.tQC_ListView_Data, 0, sizeof(QC_WBCHGB_TestInfo_t));
    MachRunPara.tQC_ListView_Data.ulCurDataSN = INVALID_DATA_SN;
    MachRunPara.tQC_ListView_Data.ulCurDataIndex = INVALID_DATA_SN;
    MachRunPara.tQC_ListView_Data.eDataStatus = DATA_STATUS_INIT;
}



/*
*   更新、填充样本测试时间, MachRunPara.ucaLastestDateTime在 PeriodScanTask任务中定时更新
*/
void Update_WBCHGB_DataManage_LastestDateTime(void)
{
    extern MachInfo_s MachInfo;
	extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
    
    strncpy((char*)g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ucaLastestDateTime, (char*)MachRunPara.ucaLastestDateTime, DATE_TIME_LEN); 
}



/*
*   WBCHGB index索引是否正常
*/
Bool WBCHGB_Index_Is_Normal(uint32_t ulIndex)
{
    extern __IO MachRunPara_s MachRunPara;
	extern MachInfo_s MachInfo;
	extern OutSideDataNum_s OutSideDataNum;
    
    if(ACCOUNT_DATA_TYPE_OUTSIDE == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType && ulIndex < OutSideDataNum.dataNum[OutSideDataNum.index])
    {
        return TRUE;
    }else if(ACCOUNT_DATA_TYPE_INNER == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType && ulIndex < INNER_ACCOUNT_COUNT_MAX_NUM){
        return TRUE;
    }
    
    LOG_Error("Index(%d) > MaxIndex", ulIndex);
    return FALSE;
}

/*
*   获取WBC HGB数据管理, 头
*/
uint32_t Get_WBCHGB_Head(void)
{
    extern __IO MachRunPara_s MachRunPara;
	extern MachInfo_s MachInfo;
    extern __IO DataManage_t g_tDataManage;
    uint32_t ulIndex = 0;
    
    ulIndex = g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_HeadIndex;
    return ulIndex;
}


/*
*   设置WBC HGB数据管理, 头
*/
uint32_t Set_WBCHGB_Head(uint32_t ulIndex)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulQueueMaxNum = 0;
    
    ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
    g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_HeadIndex = ulIndex%ulQueueMaxNum;
    return ulIndex;
}



/*
*   获取WBC HGB数据管理, 尾
*/
uint32_t Get_WBCHGB_Tail(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulIndex = 0;
    
    ulIndex = g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_TailIndex;
    return ulIndex;
}


/*
*   设置WBC HGB数据管理, 尾
*/
uint32_t Set_WBCHGB_Tail(uint32_t ulIndex)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulQueueMaxNum = 0;
    
    ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
    g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_TailIndex = ulIndex%ulQueueMaxNum;
    return ulIndex;
}


/*
*   获取WBC HGB数据管理, 尾的上一个有效数据
*/
uint32_t Get_WBCHGB_BeforeTail(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
    
    uint32_t ulIndex = 0, ulHeadIndex = 0, ulTailIndex = 0, ulQueueMaxNum = 0;
    
    //
    ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
    ulHeadIndex   = Get_WBCHGB_Head();
    ulTailIndex   = Get_WBCHGB_Tail();
    
    //
    if(ulHeadIndex == ulTailIndex)
    {
        //Queue为空
        ulIndex = INVALID_DATA_SN;
    }else{
        //不为空，获取最后一个有效数据index
        ulIndex = (ulTailIndex == 0)? ulQueueMaxNum-1: ulTailIndex-1;
    }

    return ulIndex;
}


/*
*   更新WBC HGB数据管理, 头
*/
uint32_t Update_WBCHGB_Head(void)
{
    uint32_t ulIndex = 0;
    
    return ulIndex;
}




/*
*   更新WBC HGB数据管理, 尾
*/
uint32_t Update_WBCHGB_Tail(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern  __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulHeadIndex = 0, ulTailIndex = 0, ccSendIndex = 0, ulQueueMaxNum = 0;
    uint8_t ucFlag = 0;
    
    ulHeadIndex = g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_HeadIndex;//Get_WBCHGB_Head();
    ulTailIndex = g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_TailIndex;//Get_WBCHGB_Tail();
    ccSendIndex = g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ccSendIndex;//Get_WBCHGB_SendIndex();
    ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
    
    
   if((ulTailIndex +1)%ulQueueMaxNum == ulHeadIndex)
   {
       /** 队列已满 **/
       
       //当前需要发送index和 head相等，
       if(ulHeadIndex == ccSendIndex) ucFlag = 1;
       //队列已满, 头指针指向下一个       
       g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_HeadIndex = (ulHeadIndex + 1)%ulQueueMaxNum;
       //同时需要更新send index
       if(1 == ucFlag)
       {
            g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ccSendIndex = \
                g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_HeadIndex; 
       }
   }
   //
   return g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_TailIndex;
}


/*
*   WBC HGB数据管理, 头递增
*/
uint32_t Add_WBCHGB_Head(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulQueueMaxNum = 0;
    
    ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
    g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_HeadIndex = \
        (g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_HeadIndex+1)%ulQueueMaxNum;
    
    return g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_HeadIndex;
}


/*
*   WBC HGB数据管理, 尾递增
*/
uint32_t Add_WBCHGB_Tail(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulQueueMaxNum = 0;
    
    ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
    g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_TailIndex = \
        (g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_TailIndex+1)%ulQueueMaxNum;
    
    return g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_TailIndex;

}


/*
*   WBC HGB数据管理, 有效数据个数
*/
uint32_t Get_WBCHGB_ValidNum(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    
    return g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulValidNum;
}


/*
*   WBC HGB数据管理, 有效数据个数递减
*/
uint32_t Dec_WBCHGB_ValidNum(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    
    if(g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulValidNum > 0)
    {
        g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulValidNum--;
    }
    //
    return g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulValidNum;
}



/*
*   WBC HGB数据管理, 有效数据个数递增
*/
uint32_t Add_WBCHGB_ValidNum(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulQueueMaxNum = 0;
    
    ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
    g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulValidNum = \
        (g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulValidNum+1)%ulQueueMaxNum;
    
    return g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulValidNum;

}

/*
*   WBC HGB数据管理, 获取当前账号（内部或外部账号）， 对队列最大个数
*/
uint32_t Get_WBCHGB_Queue_MaxNum(void)
{
    extern __IO MachRunPara_s MachRunPara;
	extern MachInfo_s MachInfo;
	extern OutSideDataNum_s OutSideDataNum;
    uint32_t ulQueueMaxNum = 0;
    
    if(ACCOUNT_DATA_TYPE_OUTSIDE == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType)
    {
        //外部账号
        ulQueueMaxNum = OutSideDataNum.dataNum[OutSideDataNum.index];
    
    }else{
        //内部账号
        ulQueueMaxNum = INNER_ACCOUNT_COUNT_MAX_NUM;
    }    
    return ulQueueMaxNum;
}
    
    
/*
*   获取数据管理中的LastestSN
*/
uint32_t Get_WBCHGB_LastestSN(void)
{  
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulSN = 0;
    
    ulSN = g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulLastestSN;
    
    return ulSN;  
}   



/*
*   数据管理中的LastestSN，递增
*/
uint32_t Add_WBCHGB_LastestSN(void)
{  
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulSN = 0;
    
    ulSN = ++g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulLastestSN;
    
    return ulSN;  
}   



/*
*   递增，WBC HGB数据管理中的LastestSN (用于样本测试的更新)
*/
void Update_WBCHGB_DataManage(void)
{  
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
	extern OutSideDataNum_s OutSideDataNum;
    uint32_t ulLastestSN = INVALID_DATA_SN;
    uint32_t ulQueueMaxNum = 0;
    
    //
    AccountDataType_e eAccountDataType = ACCOUNT_DATA_TYPE_END;
    eAccountDataType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType;

    //ulLastestSN = Get_WBCHGB_LastestSN();
    ulLastestSN = g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulLastestSN;
    if(INVALID_DATA_SN != ulLastestSN)
    {
        //第N次
        g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulLastestSN++;
        if(ACCOUNT_DATA_TYPE_OUTSIDE == eAccountDataType)
        {
            //外部账号，最大存储数据数
            ulQueueMaxNum = OutSideDataNum.dataNum[OutSideDataNum.index];
        }else{
            //内部账号，最大存储数据数
            ulQueueMaxNum = INNER_ACCOUNT_COUNT_MAX_NUM;
        }
        
        //TailIndex递增
        g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulQueue_TailIndex = (g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulQueue_TailIndex+1)%ulQueueMaxNum;
        //有效个数递增
        g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulValidNum++;
        if(g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulValidNum >= ulQueueMaxNum-1) 
        {
            g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulValidNum = ulQueueMaxNum-1;
        }
        //g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulValidNum = (g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulValidNum+1)%ulQueueMaxNum;
    
    }else{
        //第一次测试
        g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulLastestSN = 1;
        g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulQueue_HeadIndex = 0;
        g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulQueue_TailIndex = 1;
        g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ulValidNum = 1;
        g_tDataManage.tDataManage_WBCHGB[eAccountDataType].ccSendIndex = 0;
    
    } 
}



/*****************************************************************************/

/*
*  运行日志管理数据初始化
*/
void RunLog_DataManage_Init(void)
{
    extern  __IO DataManage_t g_tDataManage; 
    //
    g_tDataManage.ulRunLog_Queue_HeadIndex = 0;
    g_tDataManage.ulRunLog_Queue_TailIndex = 0;
    g_tDataManage.ulRunLog_SendIndex  = 0;
    g_tDataManage.ulRunLog_SN         = 0;
}



/*
*  运行日志, 队列中日志的个数
*/
uint32_t RunLog_Num(void)
{
    extern  __IO DataManage_t g_tDataManage; 
    //
    return (g_tDataManage.ulRunLog_Queue_TailIndex - g_tDataManage.ulRunLog_Queue_HeadIndex + RUN_LOG_MAX_NUM)%RUN_LOG_MAX_NUM;
}




/*
*  获取运行日志，Head (index)
*/
uint32_t Get_RunLog_Head(void)
{ 
    extern  __IO DataManage_t g_tDataManage;    
    return g_tDataManage.ulRunLog_Queue_HeadIndex;
}

/*
*  获取运行日志，Tail (index)
*/
uint32_t Get_RunLog_Tail(void)
{
    extern  __IO DataManage_t g_tDataManage;    
    return g_tDataManage.ulRunLog_Queue_TailIndex;
}



/*
*  获取运行日志，TailSN（xu）
*/
uint32_t Update_RunLog_Tail(void)
{
   extern  __IO DataManage_t g_tDataManage;
   uint8_t ucFlag = 0;
    
   if((g_tDataManage.ulRunLog_Queue_TailIndex +1)%RUN_LOG_MAX_NUM == g_tDataManage.ulRunLog_Queue_HeadIndex)
   {
       /** 队列已满 **/
       
       //当前需要发送index和 head相等，
       if(g_tDataManage.ulRunLog_Queue_HeadIndex == g_tDataManage.ulRunLog_SendIndex) ucFlag = 1;
       //队列已满, 头指针指向下一个
       g_tDataManage.ulRunLog_Queue_HeadIndex = (g_tDataManage.ulRunLog_Queue_HeadIndex + 1)%RUN_LOG_MAX_NUM;
       
       //同时需要更新send index
       if(1 == ucFlag)
       {
            g_tDataManage.ulRunLog_SendIndex = g_tDataManage.ulRunLog_Queue_HeadIndex;  
       }
   }
   //
   return g_tDataManage.ulRunLog_Queue_TailIndex;
}


/*
*   更新运行日志，TailSN
*/
uint32_t Add_RunLog_Tail(void)
{
    extern  __IO DataManage_t g_tDataManage;
    
    //TailSN加一
    g_tDataManage.ulRunLog_Queue_TailIndex = (g_tDataManage.ulRunLog_Queue_TailIndex + 1)%RUN_LOG_MAX_NUM;
   
    //
    return g_tDataManage.ulRunLog_Queue_TailIndex;
}



/*
*  获取运行日志，发送index
*/
uint32_t Update_RunLog_SendIndex(void)
{
    extern  __IO DataManage_t g_tDataManage;
    
    if(g_tDataManage.ulRunLog_SendIndex == g_tDataManage.ulRunLog_Queue_TailIndex) return INVALID_DATA_SN;  
    else return g_tDataManage.ulRunLog_SendIndex;
}
    

/*
*  更新运行日志，发送index
*/
uint32_t Add_RunLog_SendIndex(void)
{
    extern  __IO DataManage_t g_tDataManage;
    g_tDataManage.ulRunLog_SendIndex = (g_tDataManage.ulRunLog_SendIndex+1)%RUN_LOG_MAX_NUM;
    return g_tDataManage.ulRunLog_SendIndex;
}
    


    
/****************************************************************************************/
/*
*   有效个数
*/
uint32_t Get_QC_ValidNum(QC_FILENUM_INDEX_e eFileNumIndex)
{
    extern __IO DataManage_t g_tDataManage;
    //
    return g_tDataManage.tDataManage_QC[eFileNumIndex].ulValidNum;
}


/*
*   获取上一个索引号，
*/
uint32_t Get_QC_LastIndex(QC_FILENUM_INDEX_e eFileNumIndex, uint32_t ulCurIndex)
{
    extern __IO DataManage_t g_tDataManage;
    uint32_t ulTempIndex = 0;

    ulTempIndex = (ulCurIndex == 0)? QC_PER_FILE_NUM_MAX_CAPACITY - 1: g_tDataManage.tDataManage_QC[eFileNumIndex].ulQC_Queue_TailIndex -1;
    return ulTempIndex;
}


/*
*   获取尾索引号，
*/
uint32_t Get_QC_TailIndex(QC_FILENUM_INDEX_e eFileNumIndex)
{
    extern __IO DataManage_t g_tDataManage;
    //
    return g_tDataManage.tDataManage_QC[eFileNumIndex].ulQC_Queue_TailIndex;

}


/*
*   尾索引号，自增一
*/
uint32_t Add_QC_TailIndex(QC_FILENUM_INDEX_e eFileNumIndex)
{
    extern __IO DataManage_t g_tDataManage;
    //
    g_tDataManage.tDataManage_QC[eFileNumIndex].ulQC_Queue_TailIndex = \
        (g_tDataManage.tDataManage_QC[eFileNumIndex].ulQC_Queue_TailIndex + 1)%QC_PER_FILE_NUM_MAX_CAPACITY;
    
    return g_tDataManage.tDataManage_QC[eFileNumIndex].ulQC_Queue_TailIndex;
}


/*
*   设置、尾索引号
*/
uint32_t Set_QC_TailIndex(QC_FILENUM_INDEX_e eFileNumIndex, uint32_t ulIndex)
{
    extern __IO DataManage_t g_tDataManage;
    
    if(ulIndex < QC_PER_FILE_NUM_MAX_CAPACITY)
    {
    //
        g_tDataManage.tDataManage_QC[eFileNumIndex].ulQC_Queue_TailIndex = ulIndex;
    }else{
        LOG_Error("ulIndex=%d", ulIndex);
    }
    
    return g_tDataManage.tDataManage_QC[eFileNumIndex].ulQC_Queue_TailIndex;
}

/*
*   有效个数，自增一
*/
uint32_t Add_QC_ValidNum(QC_FILENUM_INDEX_e eFileNumIndex)
{
    extern __IO DataManage_t g_tDataManage;
    //
    g_tDataManage.tDataManage_QC[eFileNumIndex].ulValidNum = \
        (g_tDataManage.tDataManage_QC[eFileNumIndex].ulValidNum + 1)%QC_PER_FILE_NUM_MAX_CAPACITY;
    
    return g_tDataManage.tDataManage_QC[eFileNumIndex].ulValidNum;
}



Bool WBCHGB_SendIndex_Equal_HeadIndex(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    Bool eBool = FALSE;
    //
    if(g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ccSendIndex == \
        g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_HeadIndex)
    {
        eBool = TRUE;
    }else{
        eBool = FALSE;
    }
    
    return eBool;  
}


/*
*   获取数据管理中的 ulLastSendSN
*/
uint32_t Get_WBCHGB_SendIndex(CAT_SERVICE_e CatService)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulIndex = 0;
    //
	if(CatService == SERVICE_CC){
		ulIndex = g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ccSendIndex;
	}else{
		ulIndex = g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].otherSendIndex;
	}
    
    return ulIndex;  
}


/*
*   设置数据管理中的 ulLastSendSN
*/
uint32_t Set_WBCHGB_SendIndex(CAT_SERVICE_e CatService,uint32_t ulIndex)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulQueueMaxNum = 0;
    
    ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
	
	if(CatService == SERVICE_CC){
		g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ccSendIndex = ulIndex%ulQueueMaxNum;
	}else{
		g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].otherSendIndex = ulIndex%ulQueueMaxNum;
	}
    
    return ulIndex; 

}


/*
*   设置数据管理中的 ulLastSendSN 递增
*/
uint32_t Add_WBCHGB_SendIndex(CAT_SERVICE_e CatService)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    uint32_t ulIndex = 0;
    uint32_t ulQueueMaxNum = 0;
    
    ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
    //
	if(CatService == SERVICE_CC){
		ulIndex = (++g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ccSendIndex)%ulQueueMaxNum;
	}else{
		ulIndex = (++g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].otherSendIndex)%ulQueueMaxNum;
	}
    
    return ulIndex;  
}

/*
*   设置数据管理中的 ulLastSendSN
*/
uint32_t Set_LastSendSN(uint32_t ulSN)
{
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
    
    g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ccSendIndex = ulSN;
    
    return ulSN;  
}



/*
*	 获取最后一条有效的数据（如果存在的话）,在上电之后
*/
FeedBack_e UI_Get_Lastet_WBCHGB_TestInfo(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo, __IO DataManage_t *ptDataManage)	
{
    extern __IO MachRunPara_s MachRunPara;
    uint32_t ulHeadIndex = INVALID_DATA_SN, ulTailIndex = INVALID_DATA_SN, ulLastestSN = INVALID_DATA_SN, ulBeforeTailIndex = INVALID_DATA_SN;
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    
    ulHeadIndex = Get_WBCHGB_Head();
    ulTailIndex = Get_WBCHGB_Tail();
    ulLastestSN = Get_WBCHGB_LastestSN(); 
  
    //无记录, 或者ulLastestSN为0
    if((ulHeadIndex == ulTailIndex) || ulLastestSN == INVALID_DATA_SN)
    {
        LOG_Error("No WBC HGB Data, DataManage: ulHeadIndex=%d, ulTailIndex=%d, ulLastestSN=%d", ulHeadIndex, ulTailIndex, ulLastestSN);
        Set_Default_WBCHGB_Test_Info(ptWBCHGB_TestInfo);
        
        return FEED_BACK_SUCCESS;
    }
    
    //
    ulBeforeTailIndex = Get_WBCHGB_BeforeTail();
    if(ulBeforeTailIndex != INVALID_DATA_SN)
    { 
        //有测试记录，并且存在最新有效记录
        eFeedBack = FatFs_Read_WBCHGB_Info(ulBeforeTailIndex, ptWBCHGB_TestInfo);
        if(eFeedBack != FEED_BACK_SUCCESS)
        {
            LOG_Error(" Data Err=%d", eFeedBack);
            Set_Default_WBCHGB_Test_Info(ptWBCHGB_TestInfo);
        }
    }else{
        LOG_Info("Not Exist Valid Data");
        Set_Default_WBCHGB_Test_Info(ptWBCHGB_TestInfo);
    }

    //
    return eFeedBack;
}



/*
*	初始化，UI数据（信息）结构体
*/
void UI_Info_Init(__IO MachRunPara_s *ptUI_Info,  MachInfo_s *ptMachineInfo, __IO DataManage_t *ptDataManage)
{
    extern __IO MachRunPara_s MachRunPara;
    memset((void*)ptUI_Info, 0, sizeof(MachRunPara_s));
	
    memmove((void*)ptUI_Info->taQC_SetInfo,    (const void*)ptMachineInfo->qcInfo.set,     sizeof(QC_SetInfo_s)*QC_FILENUM_INDEX_END);
}



/*
*	初始化机器信息
*/
void Machine_Info_Init(MachInfo_s *ptMachineInfo)	
{
	extern  MachInfo_s	MachInfo;
    extern  __IO MachRunPara_s      MachRunPara;
	extern osMessageQueueId_t CATQueueHandle;
	extern osMessageQueueId_t CATQueueOtherHandle;
	extern CATGlobalStatus_s CATGlobalStatus;
	CATQueueInfoBuf_s CATQueueInfoBuf;
	RTC_HYM8563Info_s RTCTime = {0};
	extern OutSideDataNum_s OutSideDataNum;
	uint16_t i = 0,Temp = 0;
	
	memset((void*)&MachInfo, 0, sizeof(MachInfo_s));
    memset((void*)&g_tDataManage, 0, sizeof(DataManage_t));
	
	//获取仪器参数，根据ucInitFlag判断是否初始化
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_INIT_FLAG,(uint8_t*)&MachInfo.initFlag,sizeof(MachInfo.initFlag));
	
    //判断是否初始化
	if(MachInfo.initFlag == MACHINFO_INIT_FLAG)
	{                
        //已初始化
        LOG_Info("Machine Para Already Init");
		
		//读取仪器参数
		EEPROM_Read_MachInfo_touch();
		EEPROM_Read_MachInfo_flag();
		EEPROM_Read_MachInfo_companyInfo();
		EEPROM_Read_MachInfo_catMastSynServEvent();
		EEPROM_Read_MachInfo_paramSet();
		EEPROM_Read_MachInfo_calibration();
		EEPROM_Read_MachInfo_systemSet();
		EEPROM_Read_MachInfo_accountMM();
		EEPROM_Read_MachInfo_printSet();
		EEPROM_Read_MachInfo_labInfo();
		EEPROM_Read_MachInfo_qcInfo();
		EEPROM_Read_MachInfo_other();
		
        //获取数据管理信息
        Get_DataMange_Info(&g_tDataManage);
        
        //初始化UI信息结构体
        UI_Info_Init(&MachRunPara, &MachInfo, &g_tDataManage);
	}else{ 
		//重新读取一次EEPROM信息，防止被干扰导致误判
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_INIT_FLAG,(uint8_t*)&MachInfo.initFlag,sizeof(MachInfo.initFlag));
		
		if(MachInfo.initFlag == MACHINFO_INIT_FLAG){
			//已初始化
			LOG_Info("Machine Para Already Init");
			
			//读取仪器参数
			EEPROM_Read_MachInfo_touch();
			EEPROM_Read_MachInfo_flag();
			EEPROM_Read_MachInfo_companyInfo();
			EEPROM_Read_MachInfo_catMastSynServEvent();
			EEPROM_Read_MachInfo_paramSet();
			EEPROM_Read_MachInfo_calibration();
			EEPROM_Read_MachInfo_systemSet();
			EEPROM_Read_MachInfo_accountMM();
			EEPROM_Read_MachInfo_printSet();
			EEPROM_Read_MachInfo_labInfo();
			EEPROM_Read_MachInfo_qcInfo();
			EEPROM_Read_MachInfo_other();
			
			//获取数据管理信息
			Get_DataMange_Info(&g_tDataManage);
			
			//初始化UI信息结构体
			UI_Info_Init(&MachRunPara, &MachInfo, &g_tDataManage);
		}else{
			//初始化参数
			LOG_Info("Machine Para Initing...");
			
			//先将EEPROM全片擦除，即写入0xFF
			uint8_t* EEPROMInitValue = (uint8_t*)SDRAM_ApplyCommonBuf(4096);
			memset(EEPROMInitValue,0xFF,4096);
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_NEXT_UPDATE_FLAG,EEPROMInitValue,(4096-MAIN_BORD_HW_VERSION_LEN));
			
			//初始化仪器参数
			MachInfo.initFlag = MACHINFO_INIT_FLAG;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_INIT_FLAG,(uint8_t*)&MachInfo.initFlag,sizeof(MachInfo.initFlag));
			EEPROM_Init_MachInfo_touch();
			Machine_InitPara();
			
			//设置默认数据管理信息
			Set_Default_DataManage(&g_tDataManage);
			
			//保存数据管理信息
			Save_DataMange_Info(&g_tDataManage);
					
			//初始化UI信息结构体
			UI_Info_Init(&MachRunPara, &MachInfo, &g_tDataManage);
		}
    }
	
	//读取硬件版本
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_MAIN_BORD_HW_VERSION,(uint8_t*)MachRunPara.hardVer.main,9);
	EEPROMReadData(EEPROM_24C04,CONN_BORD_EEPROM_24C04,EEPROM_ADR_ZJ_BORD_HW_VERSION,(uint8_t*)MachRunPara.hardVer.zjb,9);
	if(strstr((char*)MachRunPara.hardVer.main,".") == NULL || strlen((char*)MachRunPara.hardVer.main) > 4){
		memset((char*)MachRunPara.hardVer.main,0,sizeof(MachRunPara.hardVer.main));
		strcpy((char*)MachRunPara.hardVer.main,"A.00");
	}
	if(strstr((char*)MachRunPara.hardVer.zjb,".") == NULL || strlen((char*)MachRunPara.hardVer.zjb) > 4){
		memset((char*)MachRunPara.hardVer.zjb,0,sizeof(MachRunPara.hardVer.zjb));
		strcpy((char*)MachRunPara.hardVer.zjb,"A.00");
	}
	
	//读取是否开启升级流程的标志
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_NEXT_UPDATE_FLAG,(uint8_t*)&MachRunPara.update.nextOpenToStartUpdateFlag,sizeof(MachRunPara.update.nextOpenToStartUpdateFlag));
	
	//当程序运行到这里则说明主程序已经更新写入完毕了，这里需要将标志重置掉
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_UPDATE_MAIN_SOFTWARE_FLAG,(uint8_t*)&MachRunPara.update.needBootToUpdateMainFlag,sizeof(MachRunPara.update.needBootToUpdateMainFlag));
	if(MachRunPara.update.needBootToUpdateMainFlag == UPDATE_MAIN_SOFTWARE_FLAG){
		MachRunPara.update.needBootToUpdateMainFlag = 0;
		EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_UPDATE_MAIN_SOFTWARE_FLAG,(uint8_t*)&MachRunPara.update.needBootToUpdateMainFlag,sizeof(MachRunPara.update.needBootToUpdateMainFlag));
	}
	
	//初始化用户首次开机时间戳
	if(MachInfo.flag.bit.connNet == 1){
		if(MachInfo.flag.bit.userTimestampInit != 1){
			MachInfo.flag.bit.userTimestampInit = 1;
			
			//获取RTC时间
			RTC_HYM8563GetTime(&RTCTime);
			
			MachInfo.other.userTimestamp = GetCurrentTimestamp(RTCTime);
			
			//通过CAT模块发送参数修改信息到后台
			//标记CAT必发事件标记
			MachInfo.catMastSynServEvent.bit.paraSet = 1;
			
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_FLAG,(uint8_t*)&MachInfo.flag,sizeof(MachInfo.flag));
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_OTHER+((uint8_t*)&MachInfo.other.userTimestamp - (uint8_t*)&MachInfo.other),(uint8_t*)&MachInfo.other.userTimestamp,sizeof(MachInfo.other.userTimestamp));
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
		}
	}
	
	//给CAT判断是否需要升级的各版本赋值
	CATGlobalStatus.localVersion.mainVersion = MAIN_VERSION;
	CATGlobalStatus.localVersion.bootloadVersion = *((uint16_t*)BOOTLOAD_VERSION_ADR);
	CATGlobalStatus.localVersion.uiVersion = MachInfo.other.uiVersion;
	
	//查询是否有必发项还没有发送给服务端
	if(MachInfo.catMastSynServEvent.bit.paraSet == 1){					//部分参数设置必发项
		CATQueueInfoBuf.msgType = SET_PARA_OVER_CAT_MSG_TYPE;
		osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
	}
	if(MachInfo.catMastSynServEvent.bit.innerSampleResult == 1 || MachInfo.catMastSynServEvent.bit.outtorSampleResult == 1){				//样本分析结果必发项
		CATQueueInfoBuf.msgType = SAMPLE_ANALYSE_CAT_MSG_TYPE;
		osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
	}
	if(MachInfo.catMastSynServEvent.bit.otherInnerSampleResult == 1 || MachInfo.catMastSynServEvent.bit.otherOuttorSampleResult == 1){	//样本分析结果必发项，第三方服务端
		CATQueueInfoBuf.msgType = SAMPLE_ANALYSE_CAT_MSG_TYPE;
		osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0,0);
	}
	if(MachInfo.catMastSynServEvent.bit.sendRunLog == 1){					//运行日志必发项
		CATQueueInfoBuf.msgType = SEND_RUN_LOG_CAT_MSG_TYPGE;
		osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
	}
	if(MachInfo.catMastSynServEvent.bit.delTestData == 1){
		CATQueueInfoBuf.msgType = CHANGE_MACHINE_INFO_CAT_MSG_TYPGE;
		CATQueueInfoBuf.para.changeMachineInfo.changeType = 1;					//1表示通知服务端删除了测试数据
		osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
	}
	if(MachInfo.catMastSynServEvent.bit.delUserData == 1){
		CATQueueInfoBuf.msgType = CHANGE_MACHINE_INFO_CAT_MSG_TYPGE;
		CATQueueInfoBuf.para.changeMachineInfo.changeType = 2;					//2表示通知服务端删除了用户数据
		osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
	}
	if(MachInfo.catMastSynServEvent.bit.resetUserOpenTimestamp == 1){
		CATQueueInfoBuf.msgType = CHANGE_MACHINE_INFO_CAT_MSG_TYPGE;
		CATQueueInfoBuf.para.changeMachineInfo.changeType = 4;					//4表示通知服务端重置用户开机时间
		osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
	}
	if(MachInfo.catMastSynServEvent.bit.innerChangeSampleResult == 1 || MachInfo.catMastSynServEvent.bit.outtorChangeSampleResult == 1){	//修改样本分析结果必发项
		CATQueueInfoBuf.msgType = CHANGE_SAMPLE_ANALYSE_CAT_MSG_TYPE;
		osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
	}
	
	
	//读取外部账号样本分析数据容量
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_DATA_OUTSIDE_NUM,(uint8_t*)&OutSideDataNum,sizeof(OutSideDataNum_s));
	if(OutSideDataNum.initFlag != OUTSIDE_DATA_NUM_INIT_FLAG){
		//再读取一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_DATA_OUTSIDE_NUM,(uint8_t*)&OutSideDataNum,sizeof(OutSideDataNum_s));
		if(OutSideDataNum.initFlag != OUTSIDE_DATA_NUM_INIT_FLAG){
			OutSideDataNum.initFlag = OUTSIDE_DATA_NUM_INIT_FLAG;
			OutSideDataNum.index = 0;
			
			Temp = OUTSIDE_DATA_MAX_NUM;
			
			for(i=0;i<OUTSIDE_DATA_NUM_MODE_NUM;i++){
				OutSideDataNum.dataNum[i] = Temp;
				Temp -= 500;
			}
			
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_DATA_OUTSIDE_NUM,(uint8_t*)&OutSideDataNum,sizeof(OutSideDataNum_s));
		}
	}else{
		//判断当前数据容量值是否超过最大允许数据容量值，是，则重新初始化
		if(OutSideDataNum.dataNum[OutSideDataNum.index] > OUTSIDE_DATA_MAX_NUM){
			//再读取一次
			EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_DATA_OUTSIDE_NUM,(uint8_t*)&OutSideDataNum,sizeof(OutSideDataNum_s));
			if(OutSideDataNum.dataNum[OutSideDataNum.index] > OUTSIDE_DATA_MAX_NUM){
				OutSideDataNum.initFlag = OUTSIDE_DATA_NUM_INIT_FLAG;
				OutSideDataNum.index = 0;
				
				Temp = OUTSIDE_DATA_MAX_NUM;
				
				for(i=0;i<OUTSIDE_DATA_NUM_MODE_NUM;i++){
					OutSideDataNum.dataNum[i] = Temp;
					Temp -= 500;
				}
				
				EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_DATA_OUTSIDE_NUM,(uint8_t*)&OutSideDataNum,sizeof(OutSideDataNum_s));
			}
		}
	}
	printf("OutSideDataNum_s size:%u\r\n",sizeof(OutSideDataNum_s));
	
	//开机总次数自增，这里写入EEPROM时，只更新该字段的内容，不全部更新
	MachInfo.other.totalOpenCount++;
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_OTHER+((uint8_t*)&MachInfo.other.totalOpenCount-(uint8_t*)&MachInfo.other),(uint8_t*)&MachInfo.other.totalOpenCount,sizeof(MachInfo.other.totalOpenCount));

	//更新时间,UI显示时间
	RTC_Read((uint8_t*)MachRunPara.ucaLastestDateTime, DATE_TIME_LEN);
}



    






