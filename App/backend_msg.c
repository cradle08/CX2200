#include "msg_def.h"
#include "backend_msg.h"
#include "cmsis_os2.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "main.h"
#include "file_operate.h"
#include "bsp_eeprom.h"
#include "bsp_motor.h"
#include "PrinterTask.h"
#include "process.h"
#include "agingTestPageDLG.h"
#include "WM.h"
#include "Public_menuDLG.h"
#include "bsp_press.h"
#include "bsp_outin.h"
#include "CAT_iotTask.h"
#include "time.h"
#include "Printer.h"
#include "bsp_pump.h"
#include "bsp_beep.h"
#include "bsp_light.h"
#include "monitorV.h"
#include "pcba.h"
#include "printer.h"
#include "bsp_spi.h"
#include "Crc16.h"
#include "xpt2046.h"
#include "usb_host.h"
#include "fatfs_UpanApp.h"
#include "CAT_iot.h"
#include "Common.h"
#include "HGB_Deal.h"
#include "ZhiJian.h"


osMessageQueueId_t xBackEndQueue = NULL; //UI后端消息队列，后端往其中发送消息，UI处理消息





/*
*	根据UI接受消息 设置下一样本信息,
*/
//void Set_NexSample_Info(NextSample_t *ptNextSample, NextSample_t *ptNextSample_UI)
//{
//    //清零
//	memset(ptNextSample, 0, sizeof(NextSample_t));
//    
//    //更新下一个数据系列号
//	ptNextSample->ulNextDataSN = ptNextSample_UI->ulNextDataSN;
//	
//	//strcpy((char*)ptNextSample->ucaNextSampleSN,(char*)ptNextSample_UI->ucaNextSampleSN);
//    //生成下一样本编号 
//	Get_SampleSN(ptNextSample->ucaNextSampleSN, ptNextSample->ulNextDataSN);
//    
//	strcpy((char*)ptNextSample->ucaName, 		(char*)ptNextSample_UI->ucaName);
//	strcpy((char*)ptNextSample->ucaRemark, 		(char*)ptNextSample_UI->ucaRemark);
//	
//	ptNextSample->eSex 		    = ptNextSample_UI->eSex;
//	ptNextSample->ucAge 		= ptNextSample_UI->ucAge;
//	ptNextSample->eReferGroup 	= ptNextSample_UI->eReferGroup;
//	ptNextSample->eTestMode 	= ptNextSample_UI->eTestMode;
//}





/*
*	后端发送消息，UI将获取
*/
int BackEnd_Put_Msg(uint8_t *pMsg)
{
	extern osMessageQueueId_t xBackEndQueue;
	int status;
	
	status = osMessageQueuePut(xBackEndQueue, (void*)pMsg, 0, 0x00);
    if(osOK != status)
    {
        LOG_Error("Put Msg Error");
    }
	return status;
}



/*
*	后端获取UI发送的消息
*/
int BackEnd_Get_Msg(uint8_t *pMsg)
{
	extern osMessageQueueId_t xUIQueue;
	int status;
	
	status = osMessageQueueGet(xUIQueue, (void*)pMsg, 0, osWaitForever);
	return status;
}



/*
*	上一记录消息处理
*/
static void LastRecord_Msg_Handler(LastRecord_t *pLastRecord)
{
    extern MachInfo_s MachInfo;
    
	WBCHGB_TestInfo_t* pWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
    CountHead_t* tTempCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
    uint32_t ulDataIndex = 0;

	 //获取，当前索引号
    ulDataIndex = pLastRecord->ulCurDataIndex;
    
    //读取上一条有效的数据
    FatFs_Read_ConutHead(ulDataIndex, tTempCountHead);
	
    if(tTempCountHead->ulLastValidIndex != INVALID_DATA_SN)
    {
        FatFs_Read_WBCHGB_Info(tTempCountHead->ulLastValidIndex, pWBCHGB_TestInfo);
        
        //构建返回消息头
        pWBCHGB_TestInfo->tMsgHead.usCmd        = CMD_ANALYSIS_LAST_RECORD;
        pWBCHGB_TestInfo->tMsgHead.eErrorCode   = ERROR_CODE_SUCCESS;
    }else{
        //不存在上一条数据
        LOG_Error("Not Exist Last Data, Index=%d", tTempCountHead->ulLastValidIndex);
        pWBCHGB_TestInfo->tMsgHead.usCmd        = CMD_ANALYSIS_LAST_RECORD;
        pWBCHGB_TestInfo->tMsgHead.eErrorCode   = ERROR_CODE_FAILURE;
    }
    //发送数据
    BackEnd_Put_Msg((uint8_t*)pWBCHGB_TestInfo);
}



/*
*	下一记录消息处理 (获取较新的数据)
*/
static void NextRecord_Msg_Handler(NextRecord_t *pNextRecord)
{
    extern MachInfo_s MachInfo;

    CountHead_t* tTempCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
    uint32_t ulDataIndex = 0;
	uint8_t* ucaMsgBuffer = (uint8_t*)SDRAM_ApplyCommonBuf(BACKEND_TO_UI_MSG_BUFFER_LEN); //发送信息缓存
	WBCHGB_TestInfo_t *pWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)ucaMsgBuffer;

	 //获取，当前索引号
    ulDataIndex = pNextRecord->ulCurDataIndex;

    //
    FatFs_Read_ConutHead(ulDataIndex, tTempCountHead);
    LOG_Info("ulDataIndex=%d", tTempCountHead->ulNextValidIndex);
    if(tTempCountHead->ulNextValidIndex != INVALID_DATA_SN)
    {
        FatFs_Read_WBCHGB_Info(tTempCountHead->ulNextValidIndex, pWBCHGB_TestInfo);
         //构建返回消息
        pWBCHGB_TestInfo->tMsgHead.usCmd        = CMD_ANALYSIS_NEXT_RECORD;
//        pWBCHGB_TestInfo->tMsgHead.eErrorCode   = ERROR_CODE_SUCCESS;
//        pWBCHGB_TestInfo->tMsgHead.usMsgLen     = sizeof(WBCHGB_TestInfo_t) - MSG_HEAD_LEN;

    }else{
        LOG_Error("Not Exist Last Data, Index=%d", tTempCountHead->ulNextValidIndex);
        //构建返回消息
        pWBCHGB_TestInfo->tMsgHead.usCmd        = CMD_ANALYSIS_NEXT_RECORD;
//        pWBCHGB_TestInfo->tMsgHead.eErrorCode   = ERROR_CODE_FAILURE;
        
    }
     //
    BackEnd_Put_Msg((uint8_t*)pWBCHGB_TestInfo);
}



/*
*	打印结果数据
*/
static void Print_Msg_Handler(Print_t *pPrint)
{
    //extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern PrinterCurrentState_s PrinterCurrentState;
    
    uint32_t ulIndex = 0, ulHeadIndex = 0, ulTailIndex = 0;
    WBCHGB_TestInfo_t* tWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
    
    ulIndex = pPrint->ulCurDataIndex;
    ulHeadIndex = Get_WBCHGB_Head();
    ulTailIndex = Get_WBCHGB_Tail();
    
    Msg_Head_t tMsgHead = {0};
    
    //检查打印机状态
    if(PrinterCurrentState.readyState == 0x55){
        //无打印纸
        LOG_Error("Printer No Paper");
        tMsgHead.usCmd = CMD_OTHERS_PRITER_NO_PAPER;
        BackEnd_Put_Msg((uint8_t*)&tMsgHead);
        return;
    }else if(PrinterCurrentState.readyState != 0x55 && PrinterCurrentState.readyState != 0x04){
        //打印机连接异常
        LOG_Error("Printer Conn Error");
        tMsgHead.usCmd = CMD_OTHERS_PRITER_CONN_ERR;
        BackEnd_Put_Msg((uint8_t*)&tMsgHead);
        return;
    }
    //osDelay(10);
    
    LOG_Debug("ulCurDataIndex=%d", ulIndex);
    if(INVALID_DATA_SN != ulIndex)
    {
        if(FEED_BACK_SUCCESS == FatFs_Read_WBCHGB_Info(ulIndex,  tWBCHGB_TestInfo))
        {
            CreateAndSend_WBCHGB_PrintData(tWBCHGB_TestInfo);
        }
    }else{
        LOG_Error("nuKonwn ulCurDataIndex(%d), ulHeadIndex(%d), ulTailIndex(%d)", ulIndex, ulHeadIndex, ulTailIndex);
    }
}    
    


/*
*	更新病人信息
*/
static void Update_Patient_Info(Patient_Info_t *pPatient_Info)
{
    extern MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
    uint32_t ulIndex = 0;
    WBCHGB_TestInfo_t* tWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
    CountHead_t* tCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
    
    ulIndex = pPatient_Info->ulCurDataIndex;
    
    LOG_Debug("ulCurDataIndex=%d", ulIndex);
    if(INVALID_DATA_SN != ulIndex)
    {
        //获取结果数据
        if(FEED_BACK_SUCCESS == FatFs_Read_WBCHGB_Info(ulIndex,tWBCHGB_TestInfo))
        {
            //更新数据中的病人信息
            tWBCHGB_TestInfo->eSex        = pPatient_Info->eSex;
            tWBCHGB_TestInfo->ucAge       = pPatient_Info->ucAge;
            tWBCHGB_TestInfo->eAgeUnit    = pPatient_Info->eAgeUnit;
            //tWBCHGB_TestInfo.eReferGroup = pPatient_Info->eReferGroup;
            //
            strcpy((char*)tWBCHGB_TestInfo->ucaName, (char*)pPatient_Info->ucaName);
            //strcpy((char*)tWBCHGB_TestInfo.ucaDateTime, (char*)pPatient_Info->ucaDateTime);
            strcpy((char*)tWBCHGB_TestInfo->ucaBirthDay, (char*)pPatient_Info->ucaBirthDay);
            strcpy((char*)tWBCHGB_TestInfo->ucaRemark, (char*)pPatient_Info->ucaRemark);
            //crc
            tWBCHGB_TestInfo->usCrc = CRC16((uint8_t*)&tWBCHGB_TestInfo->tMsgHead, sizeof(WBCHGB_TestInfo_t)-4);
            //回写到文件中，
            SPI4_ChangeModeTo(SPI_MODE_TP);//osKernelLock();
            FatFs_Write_WBCHGB_Info(ulIndex, tWBCHGB_TestInfo);   
            SPI4_ExitModeToDefault();osKernelUnlock();
            
            //修改Head数据状态位，标记修改已修改
            FatFs_Read_ConutHead(ulIndex, tCountHead);
            tCountHead->eStatus = DATA_STATUS_CHANGE;
            tCountHead->usCrc = CRC16((uint8_t*)tCountHead, sizeof(CountHead_t)-4);
            FatFs_Write_CountHead(ulIndex, tCountHead);
            
            //修改EEPROM标记，以便cat1 检测发送修改数据
            if(ACCOUNT_DATA_TYPE_INNER == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType)
            {
                MachInfo.catMastSynServEvent.bit.innerChangeSampleResult = 1;
				
				if(CATGlobalStatus.flag.noOtherServer == 0){
					MachInfo.catMastSynServEvent.bit.otherInnerChangeSampleResult = 1;
				}
            }else if(ACCOUNT_DATA_TYPE_OUTSIDE == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType){
                MachInfo.catMastSynServEvent.bit.outtorChangeSampleResult = 1;
				
				if(CATGlobalStatus.flag.noOtherServer == 0){
					MachInfo.catMastSynServEvent.bit.otherOuttorChangeSampleResult = 1;
				}
            }
            EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
        }
    }else{
        LOG_Error("nuKonwn ulCurDataIndex(%d)", ulIndex);
    }


}



/*
*   样本分析消息处理
*/
static void BackEnd_AnalysisMsg_Handler(uint8_t *pMsg)
{
	//uint8_t ucMsgBuffer[UI_TO_BACKEND_MSG_BUFFER_LEN] = {0}; //发送信息缓存
		
	//获取消息命令
	Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
		
    switch(pMsgHead->usCmd)
    {
        case CMD_ANALYSIS_LAST_RECORD:
        {
			//处理上一记录
			LastRecord_Msg_Handler((LastRecord_t*)pMsg);
        }
        break;
        case CMD_ANALYSIS_NEXT_RECORD:
        {
			//处理下一记录
			NextRecord_Msg_Handler((NextRecord_t*)pMsg);
        }
        break;
        case CMD_ANALYSIS_NEXT_SAMPLE:
        {
			//下一样本信息
			//Set_NexSample_Info(&MachRunPara.tNextSample, (NextSample_t*)pMsg);
        }
        break;
        case CMD_ANALYSIS_PRINT:
        {
            //打印
			Print_Msg_Handler((Print_t*)pMsg);
        }
        break;
        case CMD_ANALYSIS_UPDATE_PATIENT_INFO:
        {
            //保存修改后的病人信息
			Update_Patient_Info((Patient_Info_t*)pMsg);
        }
        break;
		
		//等待服务端返回指定的患者信息
		case CMD_ANALYSIS_REQUEST_USER_NUM:
        {
			extern WM_HWIN Analysis_Menu(void);
			extern WM_HWIN CreateNextSample_Window(void);
			extern osEventFlagsId_t GlobalEventCatGroupHandle;
			uint32_t EventBits = 0;
			CommonDialogPageData_s DialogPageData = {0};
			WM_MESSAGE UserMsg;
			CommonSwitchDialogPageData_s SwitchDialogData = {0};
			
			
			EventBits = osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_NO_PATIENT_INFO|GLOBAL_EVENT_CAT_OTHER_SER_PATIENT_INFO_OK|GLOBAL_EVENT_CAT_CONN_NET_ERR|GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR,osFlagsWaitAny,100000);
			
			MachRunPara.flag.requestingPatient = 0;
			
			if(EventBits == (uint32_t)osErrorTimeout || EventBits == (uint32_t)osErrorParameter || EventBits == (uint32_t)osErrorResource){
				//网络异常
				switch(MachInfo.companyInfo.company){
					case COMPANY_YSB:
					{
						//药师帮是直接弹窗提示后退出
						//保存对话框内容
						DialogPageData.bmpType = BMP_ALARM;
						DialogPageData.confimButt = HAS_CONFIM_BUTTON;
						DialogPageData.fun = Analysis_Menu;
						strcpy(DialogPageData.str,g_ucaLng_Analyse_NetERR[MachInfo.systemSet.eLanguage]);
						
						UserMsg.MsgId = WM_USER_DATA;
						UserMsg.Data.p = &DialogPageData;
						WM_SendMessage(g_hActiveWin,&UserMsg);
					}break;
					
					case COMPANY_HY:
					{
						//和映是进入人工确认模式，进入下一样本分析界面中
						//弹出选择对话框
						GUI_EndDialog(g_hActiveWin, 0);
						g_hActiveWin = CreateSwitchDialogPage();
						
						//重置仪器参数
						SwitchDialogData.switchDialogItem = ANALYSE_NET_ERR_MUNAUL_INPUT_INFO;
						SwitchDialogData.dialogData.bmpType = BMP_ALARM;
						SwitchDialogData.dialogData.confimButt = HAS_CONFIM_BUTTON;
						SwitchDialogData.dialogData.fun = Analysis_Menu;					
						strcpy(SwitchDialogData.dialogData.str,g_ucaLng_Analyse_InputUserNumNetERR[MachInfo.systemSet.eLanguage]);
						
						UserMsg.MsgId = WM_USER_DATA;
						UserMsg.Data.p = &SwitchDialogData;
						WM_SendMessage(g_hActiveWin,&UserMsg);
					}break;
					
					default :break;
				}
			}else if((EventBits & GLOBAL_EVENT_CAT_CONN_NET_ERR) == GLOBAL_EVENT_CAT_CONN_NET_ERR || (EventBits & GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR) == GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR){
				//网络异常
				switch(MachInfo.companyInfo.company){
					case COMPANY_YSB:
					{
						//药师帮是直接弹窗提示后退出
						//保存对话框内容
						DialogPageData.bmpType = BMP_ALARM;
						DialogPageData.confimButt = HAS_CONFIM_BUTTON;
						DialogPageData.fun = Analysis_Menu;
						strcpy(DialogPageData.str,g_ucaLng_Analyse_NetERR[MachInfo.systemSet.eLanguage]);
						
						UserMsg.MsgId = WM_USER_DATA;
						UserMsg.Data.p = &DialogPageData;
						WM_SendMessage(g_hActiveWin,&UserMsg);
					}break;
					
					case COMPANY_HY:
					{
						//和映是进入人工确认模式，进入下一样本分析界面中
						//弹出选择对话框
						GUI_EndDialog(g_hActiveWin, 0);
						g_hActiveWin = CreateSwitchDialogPage();
						
						//重置仪器参数
						SwitchDialogData.switchDialogItem = ANALYSE_NET_ERR_MUNAUL_INPUT_INFO;
						SwitchDialogData.dialogData.bmpType = BMP_ALARM;
						SwitchDialogData.dialogData.confimButt = HAS_CONFIM_BUTTON;
						SwitchDialogData.dialogData.fun = Analysis_Menu;					
						strcpy(SwitchDialogData.dialogData.str,g_ucaLng_Analyse_InputUserNumNetERR[MachInfo.systemSet.eLanguage]);
						
						UserMsg.MsgId = WM_USER_DATA;
						UserMsg.Data.p = &SwitchDialogData;
						WM_SendMessage(g_hActiveWin,&UserMsg);
					}break;
					
					default :break;
				}
			}else if((EventBits & GLOBAL_EVENT_CAT_OTHER_SER_NO_PATIENT_INFO) == GLOBAL_EVENT_CAT_OTHER_SER_NO_PATIENT_INFO){
				//不存在患者信息
				switch(MachInfo.companyInfo.company){
					case COMPANY_YSB:
					{
						//药师帮
						extern WM_HWIN CreatePatientListPage(void);
						
						GUI_EndDialog(g_hActiveWin, 0);
						g_hActiveWin = CreatePatientListPage();
					}break;
					
					case COMPANY_HY:
					{
						//和映
						DialogPageData.bmpType = BMP_ALARM;
						DialogPageData.confimButt = HAS_CONFIM_BUTTON;
						DialogPageData.fun = Analysis_Menu;
						strcpy(DialogPageData.str,g_ucaLng_Analyse_NoParientInfo[MachInfo.systemSet.eLanguage]);
						
						UserMsg.MsgId = WM_USER_DATA;
						UserMsg.Data.p = &DialogPageData;
						WM_SendMessage(g_hActiveWin,&UserMsg);
					}break;
					
					default :break;
				}
			}else if((EventBits & GLOBAL_EVENT_CAT_OTHER_SER_PATIENT_INFO_OK) == GLOBAL_EVENT_CAT_OTHER_SER_PATIENT_INFO_OK){
				//下拉患者信息成功
				switch(MachInfo.companyInfo.company){
					case COMPANY_YSB:
					{
						//药师帮
						extern WM_HWIN CreatePatientListPage(void);
						
						GUI_EndDialog(g_hActiveWin, 0);
						g_hActiveWin = CreatePatientListPage();
					}break;
					
					case COMPANY_HY:
					{
						//和映
						GUI_EndDialog(g_hActiveWin, 0);
						g_hActiveWin = CreateNextSample_Window();
					}break;
					
					default :break;
				}
			}
        }
        break;
		
        default:break;
    }
}





/*
*   列表回顾消息处理
*/
static void BackEnd_ListMsg_Handler(uint8_t *pMsg)
{
	//Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
    uint8_t ucIndex = 0;
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    __IO ListReview_t *ptListReview = (ListReview_t*)pMsg;
    
    ucIndex = ptListReview->ucIndex;
    switch(ptListReview->tMsgHead.usCmd)
    {
        case CMD_LIST_REVIEW_EDIT:
        {
            eFeedBack = Update_UI_ListView_Data(CMD_LIST_REVIEW_EDIT, ucIndex);
        }
        break;
        case CMD_LIST_REVIEW_DELETE:
        {
             eFeedBack = Update_UI_ListView_Data(CMD_LIST_REVIEW_DELETE, ucIndex);
        }
        break;
        case CMD_LIST_REVIEW_PRINT:
        {
             Update_UI_ListView_Data(CMD_LIST_REVIEW_PRINT, ucIndex);
        }
        break;
        case CMD_LIST_REVIEW_LAST_RECORD:
        {
             eFeedBack = Update_UI_ListView_Data(CMD_LIST_REVIEW_LAST_RECORD, ucIndex);
        }
        break;
        case CMD_LIST_REVIEW_NEXT_RECORD:
        {
             eFeedBack = Update_UI_ListView_Data(CMD_LIST_REVIEW_NEXT_RECORD, ucIndex);
        }
        break;
        case CMD_LIST_REVIEW_LAST_PAGE:
        {
             eFeedBack = Update_UI_ListView_Data(CMD_LIST_REVIEW_LAST_PAGE, ucIndex);
        }
        break;
        case CMD_LIST_REVIEW_NEXT_PAGE:
        {
             eFeedBack = Update_UI_ListView_Data(CMD_LIST_REVIEW_NEXT_PAGE, ucIndex);
        }
        break;
        default:break;
    }
    
    //有数据读写、加载失败
    if(FEED_BACK_FAIL == eFeedBack)
    {
        Msg_Head_t tMsgHead = {0};
        tMsgHead.usCmd = CMD_LIST_REVIEW_LOAT_DATA_FAIL;
        BackEnd_Put_Msg((uint8_t*)&tMsgHead);
    }
}



/*
*   设置消息处理
*/
static void BackEnd_SetMsg_Handler(uint8_t *pMsg)
{
	extern __IO WM_HWIN g_hActiveWin;
	extern  MachInfo_s	MachInfo;
    extern  __IO MachRunPara_s       MachRunPara;
	extern __IO DataManage_t g_tDataManage;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	
	Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
	ErrorCode_e ReStates = ERROR_CODE_SUCCESS;
//	uint32_t Temp = 0;
	CommonDialogPageData_s DialogPageData = {0};
	WM_MESSAGE UserMsg;
	extern osMessageQueueId_t CATQueueHandle;
	CATQueueInfoBuf_s CATQueueInfoBuf = {0};
	
	
    switch(pMsgHead->usCmd)
    {
        //===设置--设置SN后等待服务端响应
		case CMD_SET_SN_WAIT_CAT:
		{
			extern WM_HWIN CreateSetPage(void);
			extern WM_HWIN Createlab_infoPage(void);
			uint32_t EventBit = 0;
			
			//等待CAT返回服务端响应
			EventBit = osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_FACTORY_INFO_EXIST|GLOBAL_EVENT_CAT_FACTORY_INFO_OK|GLOBAL_EVENT_CAT_CONN_NET_ERR,osFlagsWaitAny,100000);
			
			if(EventBit == (uint32_t)osErrorTimeout || EventBit == (uint32_t)osErrorParameter || EventBit == (uint32_t)osErrorResource){
				memset((char*)MachInfo.labInfo.ucaMachineSN,0,sizeof(MachInfo.labInfo.ucaMachineSN));
				strcpy((char*)MachInfo.labInfo.ucaMachineSN,"");
				MachInfo.other.factoryTimestamp = 0;
				
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = Createlab_infoPage;
				strcpy(DialogPageData.str,"联网失败，请稍后重试");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if((EventBit & GLOBAL_EVENT_CAT_CONN_NET_ERR) == GLOBAL_EVENT_CAT_CONN_NET_ERR){
				memset((char*)MachInfo.labInfo.ucaMachineSN,0,sizeof(MachInfo.labInfo.ucaMachineSN));
				strcpy((char*)MachInfo.labInfo.ucaMachineSN,"");
				MachInfo.other.factoryTimestamp = 0;
				
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = Createlab_infoPage;
				strcpy(DialogPageData.str,"联网失败，请稍后重试");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if((EventBit & GLOBAL_EVENT_CAT_FACTORY_INFO_EXIST) == GLOBAL_EVENT_CAT_FACTORY_INFO_EXIST){
				//重置序列号
				memset((char*)MachInfo.labInfo.ucaMachineSN,0,sizeof(MachInfo.labInfo.ucaMachineSN));
				strcpy((char*)MachInfo.labInfo.ucaMachineSN,"");
				MachInfo.other.factoryTimestamp = 0;
				
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = Createlab_infoPage;
				strcpy(DialogPageData.str,"服务端中已存在此序列号，请重新输入");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if((EventBit & GLOBAL_EVENT_CAT_FACTORY_INFO_OK) == GLOBAL_EVENT_CAT_FACTORY_INFO_OK){
				//写入EEPROM
				EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_LAB_INFO,(uint8_t*)&MachInfo.labInfo,sizeof(MachInfo.labInfo));
				EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_OTHER+((uint8_t*)&MachInfo.other.factoryTimestamp-(uint8_t*)&MachInfo.other),(uint8_t*)&MachInfo.other.factoryTimestamp,sizeof(MachInfo.other.factoryTimestamp));
				
				//更新对话框内容
				DialogPageData.bmpType = BMP_OK;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateSetPage;
				strcpy(DialogPageData.str,"服务端录入成功");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}
		}
		break;
		
		//设置--修改仪器序列号
		case CMD_CHANGE_SN_WAIT_CAT:
		{
			extern WM_HWIN Createlab_infoPage(void);
			uint32_t EventBit = 0;
			
			//等待CAT发送修改SN给服务端成功
			EventBit = osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_OK|GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_ERR|GLOBAL_EVENT_CAT_CONN_NET_ERR,osFlagsWaitAny,100000);
			
			if(EventBit == (uint32_t)osErrorTimeout || EventBit == (uint32_t)osErrorParameter || EventBit == (uint32_t)osErrorResource){
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = Createlab_infoPage;
				strcpy(DialogPageData.str,"网络异常，请稍后重试");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if((EventBit & GLOBAL_EVENT_CAT_CONN_NET_ERR) == GLOBAL_EVENT_CAT_CONN_NET_ERR){
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = Createlab_infoPage;
				strcpy(DialogPageData.str,"网络异常，请稍后重试");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if((EventBit & GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_OK) == GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_OK){
				MachInfo.flag.bit.connNet = 0;
				strcpy((char*)MachInfo.labInfo.ucaMachineSN,"");
				
				EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_FLAG,(uint8_t*)&MachInfo.flag,sizeof(MachInfo.flag));
				EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_LAB_INFO+((uint8_t*)MachInfo.labInfo.ucaMachineSN-(uint8_t*)&MachInfo.labInfo),(uint8_t*)MachInfo.labInfo.ucaMachineSN,sizeof(MachInfo.labInfo.ucaMachineSN));
			
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = Createlab_infoPage;
				strcpy(DialogPageData.str,"修改仪器序列号成功，即将重启");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				
				osDelay(1000);
				
				//重新登录
				__set_PRIMASK(1);
				NVIC_SystemReset();
			}else if((EventBit & GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_ERR) == GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_ERR){
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = Createlab_infoPage;
				strcpy(DialogPageData.str,"修改仪器序列号失败，即将重启");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				
				osDelay(1000);
				
				//重新登录
				__set_PRIMASK(1);
				NVIC_SystemReset();
			}
		}
		break;
		
		//设置--辅助设置--U盘升级
		case CMD_SET_U_UPDATE:
		{
			extern WM_HWIN CreateFuzhuConfigPage(void);
			extern ApplicationTypeDef Appli_state;
			extern FATFS USBHFatFS;
			CommonDialogPageData_s DialogPageData = {0};
			uint32_t Count = 0;
			FRESULT Restatus = FR_OK;
			uint8_t BootUpdateErrFlag = 0;
			uint8_t MainUpdateErrFlag = 0;
			uint8_t UIUpdateErrFlag = 0;
			
			//配置USBH_MSC模式
			MX_USB_HOST_Init();
			
			//10s内还未正确识别U盘则退出
			while(1){
				Count++;
				
				if(Appli_state == APPLICATION_READY){
					break;
				}
				
				//超时退出，弹出提示后，重启
				if(Count > 100){
					//更新对话框内容
					DialogPageData.bmpType = BMP_ALARM;
					DialogPageData.confimButt = NO_CONFIM_BUTTON;
					DialogPageData.fun = CreateFuzhuConfigPage;
					strcpy(DialogPageData.str,g_ucaLng_SET_UpanTimeout[MachInfo.systemSet.eLanguage]);
					
					UserMsg.MsgId = WM_USER_DATA;
					UserMsg.Data.p = &DialogPageData;
					WM_SendMessage(g_hActiveWin,&UserMsg);
					
					osDelay(5000);
				
					//重新登录
					__set_PRIMASK(1);
					NVIC_SystemReset();
				}
				
				osDelay(100);
			}
			
			//挂载U盘
			Restatus = FatFs_Mount(U_PAN_LABEL,&USBHFatFS,NOT_FORCE_MKFS);
			if(Restatus != FR_OK){
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreateFuzhuConfigPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_UpanFoumtFail[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				
				osDelay(5000);
			
				//重新登录
				__set_PRIMASK(1);
				NVIC_SystemReset();
			}
			
			//===检查是否需要升级Boot===
			Restatus = Upan_UpdateBoot();
			if(Restatus == FR_INVALID_OBJECT){
				BootUpdateErrFlag = 1;
				
				//文件校验失败
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreateFuzhuConfigPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_BootUpanFileErr[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if(Restatus == FR_INVALID_PARAMETER){
				BootUpdateErrFlag = 1;
				
				//文件写入失败
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreateFuzhuConfigPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_BootUpanFileWriteFail[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if(Restatus == FR_OK || Restatus == FR_NO_PATH){
				//文件写入成功，或者文件不存在不需要升级，这里直接跳过即可，不需要处理
				BootUpdateErrFlag = 0;
			}else{
				BootUpdateErrFlag = 1;
				
				//其他文件操作异常
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreateFuzhuConfigPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_BootUpanOtherErr[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}
			
			//判断BOOT升级是否成功，否，则直接重启
			if(BootUpdateErrFlag == 1){
				osDelay(5000);
				
				//重新登录
				__set_PRIMASK(1);
				NVIC_SystemReset();
			}
			
			//===检查是否需要升级Main===
			Restatus = Upan_UpdateMain();
			if(Restatus == FR_INVALID_OBJECT){
				MainUpdateErrFlag = 1;
				
				//文件校验失败
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreateFuzhuConfigPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_MainUpanFileErr[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if(Restatus == FR_INVALID_PARAMETER){
				MainUpdateErrFlag = 1;
				
				//文件写入失败
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreateFuzhuConfigPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_MainUpanFileWriteFail[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if(Restatus == FR_OK || Restatus == FR_NO_PATH){
				//文件写入成功，或者文件不存在不需要升级，这里直接跳过即可，不需要处理
				MainUpdateErrFlag = 0;
			}else{
				MainUpdateErrFlag = 1;
				
				//其他文件操作异常
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreateFuzhuConfigPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_MainUpanOtherErr[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}
			
			//判断MAIN升级是否成功，否，则直接重启
			if(MainUpdateErrFlag == 1){
				osDelay(5000);
				
				//重新登录
				__set_PRIMASK(1);
				NVIC_SystemReset();
			}
			
			//===检查是否需要更新UI===
			Restatus = Upan_UpdateUI();
			if(Restatus == FR_INVALID_OBJECT){
				UIUpdateErrFlag = 1;
				
				//文件校验失败
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreateFuzhuConfigPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_UIUpanFileErr[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if(Restatus == FR_DENIED){
				UIUpdateErrFlag = 1;
				
				//文件写入失败
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreateFuzhuConfigPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_UIUpanFileWriteFail[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if(Restatus == FR_OK || Restatus == FR_NO_PATH){
				//文件写入成功，或者文件不存在不需要升级，这里直接跳过即可，不需要处理
				UIUpdateErrFlag = 0;
			}else{
				UIUpdateErrFlag = 1;
				
				//其他文件操作异常
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreateFuzhuConfigPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_UIUpanOtherErr[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}
			
			//判断MAIN升级是否成功，否，则直接重启
			if(UIUpdateErrFlag == 1){
				osDelay(5000);
				
				//重新登录
				__set_PRIMASK(1);
				NVIC_SystemReset();
			}
			
			
			//升级成功
			DialogPageData.bmpType = BMP_OK;
			DialogPageData.confimButt = NO_CONFIM_BUTTON;
			DialogPageData.fun = CreateFuzhuConfigPage;
			strcpy(DialogPageData.str,g_ucaLng_SET_UUpdateSuccess[MachInfo.systemSet.eLanguage]);
			
			UserMsg.MsgId = WM_USER_DATA;
			UserMsg.Data.p = &DialogPageData;
			WM_SendMessage(g_hActiveWin,&UserMsg);
			
			osDelay(5000);
		
			//重新登录
			__set_PRIMASK(1);
			NVIC_SystemReset();
		}break;
		
		//手动刷新网络
		case CMD_LIST_REVIEW_REFRESH_NET:
        {
			extern WM_HWIN CreateListReview_Window(void);
			extern osEventFlagsId_t GlobalEventCatGroupHandle;
			CommonDialogPageData_s DialogPageData = {0};
			uint32_t EventBits = 0;
			
			switch(MachInfo.companyInfo.company){
				case COMPANY_YSB:
				{
					//药师帮
					EventBits = osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REFRESH_NET|GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR,osFlagsWaitAny,100000);
				}break;
				
				default :
				{
					EventBits = osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REFRESH_NET|GLOBAL_EVENT_CAT_CONN_NET_ERR,osFlagsWaitAny,100000);
				}break;
			}
			
			if(EventBits == (uint32_t)osErrorTimeout || EventBits == (uint32_t)osErrorParameter || EventBits == (uint32_t)osErrorResource){
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateListReview_Window;
				strcpy(DialogPageData.str,g_ucaLng_Analyse_NetERR[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if((EventBits & GLOBAL_EVENT_CAT_CONN_NET_ERR) == GLOBAL_EVENT_CAT_CONN_NET_ERR){
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateListReview_Window;
				strcpy(DialogPageData.str,g_ucaLng_Analyse_NetERR[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if((EventBits & GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR) == GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR){
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateListReview_Window;
				strcpy(DialogPageData.str,g_ucaLng_Analyse_NetERR[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if((EventBits & GLOBAL_EVENT_CAT_REFRESH_NET) == GLOBAL_EVENT_CAT_REFRESH_NET){
				//刷新成功
				DialogPageData.bmpType = BMP_OK;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateListReview_Window;
				strcpy(DialogPageData.str,g_ucaLng_RefreshNetOK[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}
			
			//更新网络标识
			WM_SendMessageNoPara(WM_HBKWIN, WM_PUBLIC_UPDATE);
        }
        break;
		
        case CMD_SET_CALIBRATION:
        {
            Calibration_Msg_Handler(pMsg);
			
			//标记CAT必发事件标记
			if(MachInfo.catMastSynServEvent.bit.paraSet != 1){
				MachInfo.catMastSynServEvent.bit.paraSet = 1;
				EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
			}
			
			//通过CAT模块发送参数修改信息到后台
			CATQueueInfoBuf.msgType = SET_PARA_OVER_CAT_MSG_TYPE;
            osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
        }
        break;
        case CMD_SET_DEBUG:
        {
            Debug_Msg_Handler(pMsg);
        }
        break;
        case CMD_SET_SWITCH_LOGO:
        {
            memset((void*)SDRAM_BK_PICTURE_LOGIN_ADDR, 0, SDRAM_BK_PICTURE_LOGIN_LEN);
            LoadBMP_To_SDRAM(BK_PICTURE_OTHERS_LOGIN);
            osDelay(50);
            memset((void*)SDRAM_BK_PICTURE_PWOER_ON_OFF_ADDR, 0, SDRAM_BK_PICTURE_PWOER_ON_OFF_LEN);
            LoadBMP_To_SDRAM(BK_PICTURE_OTHERS_POWER_ON_OFF);
        }
        break;
        case CMD_SET_UPDATE_LOGIN_PICTURE:
        {
            memset((void*)SDRAM_BK_PICTURE_LOGIN_ADDR, 0, SDRAM_BK_PICTURE_LOGIN_LEN);
            LoadBMP_To_SDRAM(BK_PICTURE_OTHERS_LOGIN);
        }
        break;
		
		//压力校准中自动建压到-30Kpa
		case CMD_SET_BUILD_PRESSURE:
		{
			extern WM_HWIN CreateParaSetPage(void);
			
			//先保证处于进仓位置
			OutIn_Module_In(MOTOR_MODE_NORMAL);
		
			//出仓一段距离
			Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_OUTIN,MOTOR_DIR_OUT,8000);
			Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP,MOTOR_DIR_IN,3000);
			
			//开液阀,放气
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			
			ReStates = Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE);
			osDelay(1000);
			ReStates = Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE);
			osDelay(1000);
			ReStates = Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE);
			osDelay(1000);
			

			
			//返回执行结果给窗口
			if(ReStates != ERROR_CODE_SUCCESS){
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateParaSetPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_BuildPressFail[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else{
				//保存对话框内容
				DialogPageData.bmpType = BMP_OK;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateParaSetPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_BuildPressSuccess[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}
		}
		break;
		
        default:break;
    }
}


/*
*   服务消息处理
*/
static void BackEnd_ServerMsg_Handler(uint8_t *pMsg)
{
	extern __IO OutIn_Module_Position_e g_eOutIn_Key;
	extern ServiceAgingTestUIInfo_s ServiceAgingTestUIInfo;
	extern __IO WM_HWIN g_hActiveWin;

	extern  MachInfo_s	MachInfo;
    extern  __IO MachRunPara_s       MachRunPara;
	extern __IO DataManage_t g_tDataManage;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern WM_HWIN CreateotherCheckPage(void);
	extern WM_HWIN CreatedataManagePage(void);
	extern __IO osSemaphoreId_t xSema_OutIn_Key;
	extern __IO uint8_t g_CountHandle_Status;
	extern Panel_LED_Status_e g_ePanel_LED_Status;

	extern osMessageQueueId_t CATQueueHandle;
	CATQueueInfoBuf_s CATQueueInfoBuf = {0};
	Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;

	ErrorCode_e ReStates = ERROR_CODE_SUCCESS;
	int32_t Pressure = 0;
	uint32_t Temp = 0, ulIndex = 0;
	AgingTestUIToBackendInfo_s AgingTestUI2BInfo = {0};
	CommonDialogPageData_s DialogPageData = {0};
	WM_MESSAGE UserMsg;
	

	
	//运行日志缓存区
	RunLog_s RunLog = {0};
	RTC_HYM8563Info_s RTCInfo = {0};
	
    switch(pMsgHead->usCmd)
    {
				
		//===服务--自检后端执行===
		//负压自检
		case CMD_SERVER_PRESS:
			//先保证处于进仓位置
			OutIn_Module_In(MOTOR_MODE_NORMAL);
		
			//出仓一段距离
			Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_OUTIN,MOTOR_DIR_OUT,8000);
			Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP,MOTOR_DIR_IN,3000);
			
			//开液阀,放气
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			
			ReStates = Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE);
			
			//开液阀,放气
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			
			//进仓位置
			OutIn_Module_In(MOTOR_MODE_NORMAL);
			
			//返回执行结果给窗口
			if(ReStates != ERROR_CODE_SUCCESS){
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_BuildCheckF[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else{
				//保存对话框内容
				DialogPageData.bmpType = BMP_OK;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_BuildCheckS[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}
		break;
			
		//气密性自检
		case CMD_SERVER_AIR_LIGHT:
			//先保证处于进仓位置
			OutIn_Module_In(MOTOR_MODE_NORMAL);
		
			ReStates = Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE);
			if(ReStates != ERROR_CODE_SUCCESS){
				//出仓一段距离
				Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_OUTIN,MOTOR_DIR_OUT,8000);
				Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP,MOTOR_DIR_IN,3000);
				
				//开液阀,放气
				LIQUID_WAVE_OPEN;
				osDelay(1500);
				LIQUID_WAVE_CLOSE;
				
				//处于进仓位置
				OutIn_Module_In(MOTOR_MODE_NORMAL);
				
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_AIRTIGCheckF[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				break;
			}
			
			osDelay(1500);
			ReStates = Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE);
			if(ReStates != ERROR_CODE_SUCCESS){
				//出仓一段距离
				Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_OUTIN,MOTOR_DIR_OUT,8000);
				Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP,MOTOR_DIR_IN,3000);
				
				//开液阀,放气
				LIQUID_WAVE_OPEN;
				osDelay(1500);
				LIQUID_WAVE_CLOSE;
				
				//处于进仓位置
				OutIn_Module_In(MOTOR_MODE_NORMAL);
				
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_AIRTIGCheckF[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				break;
			}
			osDelay(1500);
			ReStates = Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE);
			if(ReStates != ERROR_CODE_SUCCESS){
				//出仓一段距离
				Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_OUTIN,MOTOR_DIR_OUT,8000);
				Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP,MOTOR_DIR_IN,3000);
				
				//开液阀,放气
				LIQUID_WAVE_OPEN;
				osDelay(1500);
				LIQUID_WAVE_CLOSE;
				
				//处于进仓位置
				OutIn_Module_In(MOTOR_MODE_NORMAL);
				
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_AIRTIGCheckF[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				break;
			}
			osDelay(1000);
			
			Pressure = Press_Value();
			osDelay(20000);
			
			//判断压力差是否大于3kpa
			if(abs(Press_Value()-Pressure) > 3000){
				//出仓一段距离
				Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_OUTIN,MOTOR_DIR_OUT,8000);
				Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP,MOTOR_DIR_IN,3000);
				
				//开液阀,放气
				LIQUID_WAVE_OPEN;
				osDelay(1500);
				LIQUID_WAVE_CLOSE;
				
				//处于进仓位置
				OutIn_Module_In(MOTOR_MODE_NORMAL);
				
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_AIRTIGCheckF[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else{
				//出仓一段距离
				Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_OUTIN,MOTOR_DIR_OUT,8000);
				Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP,MOTOR_DIR_IN,3000);
				
				//开液阀,放气
				LIQUID_WAVE_OPEN;
				osDelay(1500);
				LIQUID_WAVE_CLOSE;
				
				//处于进仓位置
				OutIn_Module_In(MOTOR_MODE_NORMAL);
				
				//保存对话框内容
				DialogPageData.bmpType = BMP_OK;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_AIRTIGCheckS[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}
		break;
			
		//电机自检
		case CMD_SERVER_MOTOR:
			if(SET == OC_Status(OC_INDEX_OUTIN_OUT) && SET == OC_Status(OC_INDEX_OUTIN_IN)){
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_MotoCheckF[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				break;
			}
			
			if(SET == OC_Status(OC_INDEX_CLAMP_IN) && SET == OC_Status(OC_INDEX_CLAMP_OUT)){
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_MotoCheckF[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				break;
			}
		
			if(OutIn_Motor_Out(MOTOR_MODE_NORMAL, MOTOR_OUTIN_MAX_STEP,0) != ERROR_CODE_SUCCESS){
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_MotoCheckF[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				break;
			}
			
			if(Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, 0) != ERROR_CODE_SUCCESS){
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_MotoCheckF[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				break;
			}
			
			if(Clamp_Motor_Out(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, 0) != ERROR_CODE_SUCCESS){
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_MotoCheckF[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				break;
			}
			
			if(OutIn_Motor_In(MOTOR_MODE_NORMAL, MOTOR_OUTIN_MAX_STEP,0) != ERROR_CODE_SUCCESS){
				//保存对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateservicePage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_MotoCheckF[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				break;
			}
			
			//保存对话框内容
			DialogPageData.bmpType = BMP_OK;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = CreateservicePage;
			strcpy(DialogPageData.str,g_ucaLng_Serve_MotoCheckS[MachInfo.systemSet.eLanguage]);
			
			UserMsg.MsgId = WM_USER_DATA;
			UserMsg.Data.p = &DialogPageData;
			WM_SendMessage(g_hActiveWin,&UserMsg);
		break;
			
		//===服务--其他自检的后端执行===
		//气阀自检
		case CMD_SERVER_OTHER_CHECK_AIR_VALVE:
			AIR_WAVE_OPEN;
			osDelay(500);
			AIR_WAVE_CLOSE;
			osDelay(500);
			AIR_WAVE_OPEN;
			osDelay(500);
			AIR_WAVE_CLOSE;
			osDelay(500);
			AIR_WAVE_OPEN;
			osDelay(500);
			AIR_WAVE_CLOSE;
			osDelay(500);
		
			//保存对话框内容
			DialogPageData.bmpType = BMP_OK;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = CreateotherCheckPage;
			strcpy(DialogPageData.str,g_ucaLng_Serve_AirValveS[MachInfo.systemSet.eLanguage]);
			
			UserMsg.MsgId = WM_USER_DATA;
			UserMsg.Data.p = &DialogPageData;
			WM_SendMessage(g_hActiveWin,&UserMsg);
		break;
		
		//泵自检
		case CMD_SERVER_OTHER_CHECK_PUMP:
			Pump_Start();
			osDelay(3000);
			Pump_Stop();
		
			//保存对话框内容
			DialogPageData.bmpType = BMP_OK;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = CreateotherCheckPage;
			strcpy(DialogPageData.str,g_ucaLng_Serve_PumpCheckS[MachInfo.systemSet.eLanguage]);
			
			UserMsg.MsgId = WM_USER_DATA;
			UserMsg.Data.p = &DialogPageData;
			WM_SendMessage(g_hActiveWin,&UserMsg);
		break;
		
		//液阀自检
		case CMD_SERVER_OTHER_CHECK_YE_VALVE:
			LIQUID_WAVE_OPEN;
			osDelay(500);
			LIQUID_WAVE_CLOSE;
			osDelay(500);
			LIQUID_WAVE_OPEN;
			osDelay(500);
			LIQUID_WAVE_CLOSE;
			osDelay(500);
			LIQUID_WAVE_OPEN;
			osDelay(500);
			LIQUID_WAVE_CLOSE;
			osDelay(500);
		
			//保存对话框内容
			DialogPageData.bmpType = BMP_OK;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = CreateotherCheckPage;
			strcpy(DialogPageData.str,g_ucaLng_Serve_LiquidValveS[MachInfo.systemSet.eLanguage]);
			
			UserMsg.MsgId = WM_USER_DATA;
			UserMsg.Data.p = &DialogPageData;
			WM_SendMessage(g_hActiveWin,&UserMsg);
		break;
		
		//蜂鸣器自检
		case CMD_SERVER_OTHER_CHECK_BEEP:
			Beep_Start();
			osDelay(500);
			Beep_Stop();
			osDelay(500);
			Beep_Start();
			osDelay(500);
			Beep_Stop();
			osDelay(500);
			Beep_Start();
			osDelay(500);
			Beep_Stop();
			osDelay(500);
		
			//保存对话框内容
			DialogPageData.bmpType = BMP_OK;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = CreateotherCheckPage;
			strcpy(DialogPageData.str,g_ucaLng_Serve_BeepCheckS[MachInfo.systemSet.eLanguage]);
			
			UserMsg.MsgId = WM_USER_DATA;
			UserMsg.Data.p = &DialogPageData;
			WM_SendMessage(g_hActiveWin,&UserMsg);
		break;
		
		//HGB自检
		case CMD_SERVER_OTHER_CHECK_HGB:
		{
			uint16_t CloseADC = HGB_ADC();
			uint16_t Open535 = 0;
			uint16_t Open415 = 0;
			
			Light_Start(HGBLED_535,MachInfo.paramSet.current535);
			osDelay(5000);
			Open535 = HGB_ADC();
			Light_Start(HGBLED_415,MachInfo.paramSet.current415);
			osDelay(3000);
			Open415 = HGB_ADC();
			
			//关闭HGB
			Light_Stop();
			
			if((Open535 < MachInfo.paramSet.KQADC535-500) || (Open535 > 3700) || (Open415 < MachInfo.paramSet.KQADC415-500) || (Open415 > 3700)){
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateotherCheckPage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_HGBCheckingERR[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else{
				//保存对话框内容
				DialogPageData.bmpType = BMP_OK;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreateotherCheckPage;
				strcpy(DialogPageData.str,g_ucaLng_Serve_HGBCheckingOK[MachInfo.systemSet.eLanguage]);
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}
		}
		break;
			
		//===服务--交互自检的后端执行===
		case CMD_SERVER_INTERACTION_PRINTER:
			TestPrinter();
		break;
		
		//屏幕边框显示
		case CMD_SERVER_INTERACTION_SCREEN_BOARD:
		{
			WM_HideWindow(g_hActiveWin);
			osDelay(100);
			
			//显示边框
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			GUI_SetColor(GUI_RED);
			GUI_SetPenSize(5);
			GUI_DrawLine(0,0,799,0);
			GUI_DrawLine(799,0,799,479);
			GUI_DrawLine(799,479,0,479);
			GUI_DrawLine(0,479,0,0);
			
			while(PenIRQ_GetState() != PENIRQ_ACTIVE);
			osDelay(100);
			while(PenIRQ_GetState() == PENIRQ_ACTIVE);
			
			WM_SetCallback(WM_HBKWIN, Public_cbBkWin);
			WM_ShowWindow(g_hActiveWin);
		}
		break;
		
		//===服务--数据管理的后端执行===
		//删除测试数据
		case CMD_SERVER_DEL_TEST_DATA:
			//保存对话框内容
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = CreatedataManagePage;
		
			if(Del_Account_DataFile(ACCOUNT_DATA_TYPE_INNER) == FEED_BACK_SUCCESS){
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ulLastestSN = INVALID_DATA_SN;
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ulQueue_TailIndex = 0;
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ulQueue_HeadIndex = 0;
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ulValidNum = 0;
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].ccSendIndex = 0;
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_INNER].otherSendIndex = 0;
				
				DialogPageData.bmpType = BMP_OK;
				strcpy(DialogPageData.str,"删除测试数据成功");
				
				//这里需要向服务端发送指令
				CATQueueInfoBuf.msgType = CHANGE_MACHINE_INFO_CAT_MSG_TYPGE;
				CATQueueInfoBuf.para.changeMachineInfo.changeType = 1;					//1表示通知服务端清除了测试数据
				osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
				
				//保存运行日志
				RunLog.logHead.num = g_tDataManage.ulRunLog_SN;//MachRunPara.tDataManage.lastRunLogNum;
				RunLog.logHead.eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
				RTC_HYM8563GetTime(&RTCInfo);
				sprintf(RunLog.logHead.dataTime,"20%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
								RTCInfo.year.bcd_h,RTCInfo.year.bcd_l,
								RTCInfo.month.bcd_h,RTCInfo.month.bcd_l,
								RTCInfo.day.bcd_h,RTCInfo.day.bcd_l,
								RTCInfo.hour.bcd_h,RTCInfo.hour.bcd_l,
								RTCInfo.min.bcd_h,RTCInfo.min.bcd_l,
								RTCInfo.sec.bcd_h,RTCInfo.sec.bcd_l);
				sprintf(RunLog.logHead.shortStr,"Deleting Test Data");
				RunLog.logHead.logLen += sprintf(RunLog.Str+RunLog.logHead.logLen,"Deleting test data succeeded");
				RunLog.Str[RunLog.logHead.logLen] = '\0';
				RunLog.logHead.timestamp = GetCurrentTimestamp(RTCInfo);
                RunLog.usCrc = CRC16((uint8_t*)&RunLog, sizeof(RunLog_s)-2);
                
                ulIndex = Update_RunLog_Tail();
				Fatfs_Write_RunLog(ulIndex, &RunLog);
                Add_RunLog_Tail();
                
                g_tDataManage.ulRunLog_SN++;
                Save_DataMange_Info(&g_tDataManage);
                
				//标记CAT必发项
				MachInfo.catMastSynServEvent.bit.delTestData = 1;
				MachInfo.catMastSynServEvent.bit.sendRunLog = 1;
				EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				
				osDelay(1000);
					
				//重新登录
				__set_PRIMASK(1);
				NVIC_SystemReset();
			}else{
				DialogPageData.bmpType = BMP_ALARM;
				strcpy(DialogPageData.str,"删除测试数据失败");
				
				//保存运行日志
				RunLog.logHead.num = g_tDataManage.ulRunLog_SN;//MachRunPara.tDataManage.lastRunLogNum;
				RunLog.logHead.eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
				RTC_HYM8563GetTime(&RTCInfo);
				sprintf(RunLog.logHead.dataTime,"20%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
								RTCInfo.year.bcd_h,RTCInfo.year.bcd_l,
								RTCInfo.month.bcd_h,RTCInfo.month.bcd_l,
								RTCInfo.day.bcd_h,RTCInfo.day.bcd_l,
								RTCInfo.hour.bcd_h,RTCInfo.hour.bcd_l,
								RTCInfo.min.bcd_h,RTCInfo.min.bcd_l,
								RTCInfo.sec.bcd_h,RTCInfo.sec.bcd_l);
				sprintf(RunLog.logHead.shortStr,"Deleting Test Data");
				RunLog.logHead.logLen += sprintf(RunLog.Str+RunLog.logHead.logLen,"Failed to delete test data");
				RunLog.Str[RunLog.logHead.logLen] = '\0';
				RunLog.logHead.timestamp = GetCurrentTimestamp(RTCInfo);
                RunLog.usCrc = CRC16((uint8_t*)&RunLog, sizeof(RunLog_s)-2);
                
                ulIndex = Update_RunLog_Tail();
				Fatfs_Write_RunLog(ulIndex, &RunLog);
                Add_RunLog_Tail();
                
                g_tDataManage.ulRunLog_SN++;
                Save_DataMange_Info(&g_tDataManage);
				
				//标记CAT必发项
				if(MachInfo.catMastSynServEvent.bit.sendRunLog != 1){
					MachInfo.catMastSynServEvent.bit.sendRunLog = 1;
					EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
				}
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}			
		break;
			
		//删除用户数据
		case CMD_SERVER_DEL_USER_DATA:
			//保存对话框内容
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = CreatedataManagePage;
		
			if(Del_Account_DataFile(ACCOUNT_DATA_TYPE_OUTSIDE) == FEED_BACK_SUCCESS){
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].ulLastestSN = INVALID_DATA_SN;
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].ulQueue_TailIndex = 0;
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].ulQueue_HeadIndex = 0;
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].ulValidNum = 0;
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].ccSendIndex = 0;
				g_tDataManage.tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_OUTSIDE].otherSendIndex = 0;
				
				DialogPageData.bmpType = BMP_OK;
				strcpy(DialogPageData.str,g_ucaLng_Serve_DEL_USERDATA_SUCCESS[MachInfo.systemSet.eLanguage]);
				
				//这里需要向服务端发送指令
				CATQueueInfoBuf.msgType = CHANGE_MACHINE_INFO_CAT_MSG_TYPGE;
				CATQueueInfoBuf.para.changeMachineInfo.changeType = 2;					//2表示通知服务端清除了用户数据
				osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
				
				//保存运行日志
				RunLog.logHead.num = g_tDataManage.ulRunLog_SN;//MachRunPara.tDataManage.lastRunLogNum;
				RunLog.logHead.eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
				RTC_HYM8563GetTime(&RTCInfo);
				sprintf(RunLog.logHead.dataTime,"20%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
								RTCInfo.year.bcd_h,RTCInfo.year.bcd_l,
								RTCInfo.month.bcd_h,RTCInfo.month.bcd_l,
								RTCInfo.day.bcd_h,RTCInfo.day.bcd_l,
								RTCInfo.hour.bcd_h,RTCInfo.hour.bcd_l,
								RTCInfo.min.bcd_h,RTCInfo.min.bcd_l,
								RTCInfo.sec.bcd_h,RTCInfo.sec.bcd_l);
				sprintf(RunLog.logHead.shortStr,"Del User Data");
				RunLog.logHead.logLen += sprintf(RunLog.Str+RunLog.logHead.logLen,"Del User Data Success");
				RunLog.Str[RunLog.logHead.logLen] = '\0';
				RunLog.logHead.timestamp = GetCurrentTimestamp(RTCInfo);
                RunLog.usCrc = CRC16((uint8_t*)&RunLog, sizeof(RunLog_s)-2);
                
                ulIndex = Update_RunLog_Tail();
				Fatfs_Write_RunLog(ulIndex, &RunLog);
                Add_RunLog_Tail();
                
                g_tDataManage.ulRunLog_SN++;
                Save_DataMange_Info(&g_tDataManage);
				//MachRunPara.tDataManage.lastRunLogNum++;
				//Save_DataMange_Info(&MachRunPara.tDataManage);
				
				//标记CAT必发项
				MachInfo.catMastSynServEvent.bit.delUserData = 1;
				MachInfo.catMastSynServEvent.bit.sendRunLog = 1;
				EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				
				osDelay(1000);
					
				//重新登录
				__set_PRIMASK(1);
				NVIC_SystemReset();
			}else{
				DialogPageData.bmpType = BMP_ALARM;
				strcpy(DialogPageData.str,g_ucaLng_Serve_DEL_USERDATA_FAIL[MachInfo.systemSet.eLanguage]);
				
				//保存运行日志
				RunLog.logHead.num = g_tDataManage.ulRunLog_SN;//MachRunPara.tDataManage.lastRunLogNum;
				RunLog.logHead.eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
				RTC_HYM8563GetTime(&RTCInfo);
				sprintf(RunLog.logHead.dataTime,"20%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
								RTCInfo.year.bcd_h,RTCInfo.year.bcd_l,
								RTCInfo.month.bcd_h,RTCInfo.month.bcd_l,
								RTCInfo.day.bcd_h,RTCInfo.day.bcd_l,
								RTCInfo.hour.bcd_h,RTCInfo.hour.bcd_l,
								RTCInfo.min.bcd_h,RTCInfo.min.bcd_l,
								RTCInfo.sec.bcd_h,RTCInfo.sec.bcd_l);
				sprintf(RunLog.logHead.shortStr,"Del User Data");
				RunLog.logHead.logLen += sprintf(RunLog.Str+RunLog.logHead.logLen,"Del User Data Fail");
				RunLog.Str[RunLog.logHead.logLen] = '\0';
				RunLog.logHead.timestamp = GetCurrentTimestamp(RTCInfo);
                RunLog.usCrc = CRC16((uint8_t*)&RunLog, sizeof(RunLog_s)-2);

                ulIndex = Update_RunLog_Tail();
				Fatfs_Write_RunLog(ulIndex, &RunLog);
                Add_RunLog_Tail();
                
                g_tDataManage.ulRunLog_SN++;
                Save_DataMange_Info(&g_tDataManage);
				//MachRunPara.tDataManage.lastRunLogNum++;
				//Save_DataMange_Info(&MachRunPara.tDataManage);
				
				//标记CAT必发项
				if(MachInfo.catMastSynServEvent.bit.sendRunLog != 1){
					MachInfo.catMastSynServEvent.bit.sendRunLog = 1;
					EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
				}
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}
		break;
        case CMD_SERVER_RECOVER_FACTORY:
        {
			extern WM_HWIN CreatedataManagePage(void);
			extern CATGlobalStatus_s CATGlobalStatus;
			uint32_t EventBit = 0;
			
			if(strlen((char*)MachInfo.labInfo.ucaMachineSN) < MACHINE_SN_LEN || CATGlobalStatus.flag.connServerErr == 1){
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_RECOVER_FACTORY_OK);
			}else{
				//先清除网络异常事件标志
				osEventFlagsClear(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_NET_ERR);
				
				//向服务端发送恢复出厂信息
				CATQueueInfoBuf.msgType = CHANGE_MACHINE_INFO_CAT_MSG_TYPGE;
				CATQueueInfoBuf.para.changeMachineInfo.changeType = 3;					//3表示通知服务端恢复了出厂设置
				osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
			}
			
			//等待响应
			EventBit = osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_RECOVER_FACTORY_OK|GLOBAL_EVENT_CAT_RECOVER_FACTORY_ERR|GLOBAL_EVENT_CAT_CONN_NET_ERR,osFlagsWaitAny,100000);
			
			if(EventBit == (uint32_t)osErrorTimeout || EventBit == (uint32_t)osErrorParameter || EventBit == (uint32_t)osErrorResource){
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreatedataManagePage;
				strcpy(DialogPageData.str,"网络异常，请稍后重试");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if((EventBit & GLOBAL_EVENT_CAT_CONN_NET_ERR) == GLOBAL_EVENT_CAT_CONN_NET_ERR){
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = CreatedataManagePage;
				strcpy(DialogPageData.str,"网络异常，请稍后重试");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
			}else if((EventBit & GLOBAL_EVENT_CAT_RECOVER_FACTORY_OK) == GLOBAL_EVENT_CAT_RECOVER_FACTORY_OK){
				//清除所有参数信息
				MachInfo.initFlag = 0;
				EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_INIT_FLAG,(uint8_t*)&MachInfo.initFlag,sizeof(MachInfo.initFlag));
				
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreatedataManagePage;
				strcpy(DialogPageData.str,"恢复出厂设置成功，即将重启");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				
				osDelay(1000);
				
				//重新登录
				__set_PRIMASK(1);
				NVIC_SystemReset();
			}else if((EventBit & GLOBAL_EVENT_CAT_RECOVER_FACTORY_ERR) == GLOBAL_EVENT_CAT_RECOVER_FACTORY_ERR){
				//更新对话框内容
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = CreatedataManagePage;
				strcpy(DialogPageData.str,"恢复出厂设置失败，即将重启");
				
				UserMsg.MsgId = WM_USER_DATA;
				UserMsg.Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,&UserMsg);
				
				osDelay(1000);
				
				//重新登录
				__set_PRIMASK(1);
				NVIC_SystemReset();
			}
        }
        break;
		
		//重启CAT模块
		case CMD_SERVER_CAT_RESTART:
        {
			extern osThreadId_t CATUpdateStatusTaskHandle;
			extern const osThreadAttr_t CATUpdateStatus_attributes;
			
			//先删除任务
			osThreadTerminate(CATUpdateStatusTaskHandle);
			
			osDelay(2);
			
			//重新创建任务
			CATUpdateStatusTaskHandle = osThreadNew(CATUpdateStatusTask, NULL, &CATUpdateStatus_attributes);
        }
        break;
		
		//===服务--老化测试的后端执行====
		//气嘴密闭性测试
		case CMD_SERVER_QIZUI_TEST:
		{
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			QizuiTest(AgingTestUI2BInfo.count);
		}
		break;
		
		//流程老化测试
		case CMD_SERVER_AGING_TEST:
        {
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			AgingTest(AgingTestUI2BInfo.count);
        }
		break;
		
		//探针老化测试
		case CMD_SERVER_TANZHEN_TEST:
        {
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			TanzhenTest(AgingTestUI2BInfo.count);
        }
        break;
		
		//气阀老化测试
		case CMD_SERVER_AIR_VALVE_TEST:
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			
			memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));
			ServiceAgingTestUIInfo.page = AIR_TEST_RESULT;
			ServiceAgingTestUIInfo.startFlag = 1;
			ServiceAgingTestUIInfo.stopFlag = 0;
			
			//读取RTC起始时间
			RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.airValveTestResultInfo.rtc.startTime);
			ServiceAgingTestUIInfo.unions.airValveTestResultInfo.setCount = AgingTestUI2BInfo.count;
			ServiceAgingTestUIInfo.unions.airValveTestResultInfo.runCount = 0;
		
			//面板指示灯闪烁
			g_CountHandle_Status = 1;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
			
			while(ServiceAgingTestUIInfo.unions.airValveTestResultInfo.runCount < ServiceAgingTestUIInfo.unions.airValveTestResultInfo.setCount){
				//判断是否提前中止探针稳定性老化
				if(ServiceAgingTestUIInfo.stopFlag == 1){
					//提前退出循环
					break;
				}
				
				ServiceAgingTestUIInfo.unions.airValveTestResultInfo.runCount++;
				
				//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
				WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
				
				AIR_WAVE_OPEN;
				osDelay(1000);
				AIR_WAVE_CLOSE;
				osDelay(1000);
			}
			
			ServiceAgingTestUIInfo.startFlag = 0;
			ServiceAgingTestUIInfo.stopFlag = 1;
			
			//读取RTC结束时间
			RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.airValveTestResultInfo.rtc.endTime);
			
			//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
			WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
			
			//出仓
			OutIn_Module_Out(MOTOR_MODE_NORMAL);
			
			//关闭面板指示灯
			g_CountHandle_Status = 0;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
			
			//向界面发送后端执行完毕消息
			WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
		break;
			
		//液阀老化测试
		case CMD_SERVER_YEFA_VALVE_TEST:
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			
			memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));
			ServiceAgingTestUIInfo.page = YEFA_TEST_RESULT;
			ServiceAgingTestUIInfo.startFlag = 1;
			ServiceAgingTestUIInfo.stopFlag = 0;
		
			//读取RTC起始时间
			RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.yeFaValveTestResultInfo.rtc.startTime);
			ServiceAgingTestUIInfo.unions.yeFaValveTestResultInfo.setCount = AgingTestUI2BInfo.count;
			ServiceAgingTestUIInfo.unions.yeFaValveTestResultInfo.runCount = 0;
			
			//面板指示灯闪烁
			g_CountHandle_Status = 1;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
		
			while(ServiceAgingTestUIInfo.unions.yeFaValveTestResultInfo.runCount < ServiceAgingTestUIInfo.unions.yeFaValveTestResultInfo.setCount){
				//判断是否提前中止探针稳定性老化
				if(ServiceAgingTestUIInfo.stopFlag == 1){
					//提前退出循环
					break;
				}
				
				ServiceAgingTestUIInfo.unions.yeFaValveTestResultInfo.runCount++;
				
				//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
				WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
				
				LIQUID_WAVE_OPEN;
				osDelay(1000);
				LIQUID_WAVE_CLOSE;
				osDelay(1000);
			}
			
			ServiceAgingTestUIInfo.startFlag = 0;
			ServiceAgingTestUIInfo.stopFlag = 1;
			
			//读取RTC结束时间
			RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.yeFaValveTestResultInfo.rtc.endTime);
			
			//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
			WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
			
			//出仓
			OutIn_Module_Out(MOTOR_MODE_NORMAL);
			
			//关闭面板指示灯
			g_CountHandle_Status = 0;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
			
			//向界面发送后端执行完毕消息
			WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
		break;
			
		//建压老化测试
		case CMD_SERVER_BUILD_PRESSURE_TEST:
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			
			memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));
			ServiceAgingTestUIInfo.page = BUILD_PRESSURE_TEST_RESULT;
			ServiceAgingTestUIInfo.startFlag = 1;
			ServiceAgingTestUIInfo.stopFlag = 0;
		
			//出仓
			OutIn_Module_Out(MOTOR_MODE_NORMAL);
			
			//读取RTC起始时间
			RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.rtc.startTime);
			ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.setCount = AgingTestUI2BInfo.count;
			ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.runCount = 0;
			
			//面板指示灯闪烁
			g_CountHandle_Status = 1;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
		
			while(ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.runCount < ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.setCount){
				//判断是否提前中止探针稳定性老化
				if(ServiceAgingTestUIInfo.stopFlag == 1){
					//提前退出循环
					break;
				}
				
				ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.runCount++;
				
				//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
				WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
				
				//开液阀,放气
				LIQUID_WAVE_OPEN;
				osDelay(1500);
				LIQUID_WAVE_CLOSE;
				
				Temp = HAL_GetTick();
				ReStates = Build_Press(RUN_MODE_NORMAL, -10000);
				ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure10.time = ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure10.time>(HAL_GetTick() - Temp)?ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure10.time:(HAL_GetTick() - Temp);
				if(ReStates != ERROR_CODE_SUCCESS){
					//开液阀,放气
					LIQUID_WAVE_OPEN;
					osDelay(1500);
					LIQUID_WAVE_CLOSE;
					
					ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure10.buildCount++;
					
					//建压成功后才可以进入下一个压力值
					continue;
				}
				
				Temp = HAL_GetTick();
				ReStates = Build_Press(RUN_MODE_NORMAL, -20000);
				ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure20.time = ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure20.time>(HAL_GetTick() - Temp)?ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure20.time:(HAL_GetTick() - Temp);
				if(ReStates != ERROR_CODE_SUCCESS){
					//开液阀,放气
					LIQUID_WAVE_OPEN;
					osDelay(1500);
					LIQUID_WAVE_CLOSE;
					
					ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure20.buildCount++;
					
					//建压成功后才可以进入下一个压力值
					continue;
				}
				
				Temp = HAL_GetTick();
				ReStates = Build_Press(RUN_MODE_NORMAL, -30000);
				ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure30.time = ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure30.time>(HAL_GetTick() - Temp)?ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure30.time:(HAL_GetTick() - Temp);
				if(ReStates != ERROR_CODE_SUCCESS){
					//开液阀,放气
					LIQUID_WAVE_OPEN;
					osDelay(1500);
					LIQUID_WAVE_CLOSE;
					
					ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure30.buildCount++;
					
					//建压成功后才可以进入下一个压力值
					continue;
				}
				
				Temp = HAL_GetTick();
				ReStates = Build_Press(RUN_MODE_NORMAL, -40000);
				ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure40.time = ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure40.time>(HAL_GetTick() - Temp)?ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure40.time:(HAL_GetTick() - Temp);
				if(ReStates != ERROR_CODE_SUCCESS){
					//开液阀,放气
					LIQUID_WAVE_OPEN;
					osDelay(1500);
					LIQUID_WAVE_CLOSE;
					
					ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure40.buildCount++;
					
					//建压成功后才可以进入下一个压力值
					continue;
				}
				
				Temp = HAL_GetTick();
				ReStates = Build_Press(RUN_MODE_NORMAL, -50000);
				ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure50.time = ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure50.time>(HAL_GetTick() - Temp)?ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure50.time:(HAL_GetTick() - Temp);
				if(ReStates != ERROR_CODE_SUCCESS){
					//开液阀,放气
					LIQUID_WAVE_OPEN;
					osDelay(1500);
					LIQUID_WAVE_CLOSE;
					
					ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.pressure50.buildCount++;
					
					//建压成功后才可以进入下一个压力值
					continue;
				}
			}
			
			ServiceAgingTestUIInfo.startFlag = 0;
			ServiceAgingTestUIInfo.stopFlag = 1;
			
			//读取RTC结束时间
			RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.buildPressureTestResultInfo.rtc.endTime);
			
			//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
			WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
			
			//出仓
			OutIn_Module_Out(MOTOR_MODE_NORMAL);
			//开液阀,放气
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			
			//关闭面板指示灯
			g_CountHandle_Status = 0;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
			
			//向界面发送后端执行完毕消息
			WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
		break;
			
		//定量电极测试，需要数字工装计数池来测试
		case CMD_SERVER_DL_ELEC_TEST:
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			
			memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));
			ServiceAgingTestUIInfo.page = DL_ELEC_TEST_RESULT;
			ServiceAgingTestUIInfo.startFlag = 1;
			ServiceAgingTestUIInfo.stopFlag = 0;
		
			//先处于出仓位置
			OutIn_Module_Out(MOTOR_MODE_NORMAL);
		
			//开启恒流源
			BC_CUR_SW_ON;
			HAL_Delay(500);
			
			//读取RTC起始时间
			RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.dlElecTestResultInfo.rtc.startTime);
			ServiceAgingTestUIInfo.unions.dlElecTestResultInfo.setCount = AgingTestUI2BInfo.count;
			ServiceAgingTestUIInfo.unions.dlElecTestResultInfo.runCount = 0;
			
			//面板指示灯闪烁
			g_CountHandle_Status = 1;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
		
			while(ServiceAgingTestUIInfo.unions.dlElecTestResultInfo.runCount < ServiceAgingTestUIInfo.unions.dlElecTestResultInfo.setCount){
				//判断是否提前中止探针稳定性老化
				if(ServiceAgingTestUIInfo.stopFlag == 1){
					//提前退出循环
					break;
				}
				
				ServiceAgingTestUIInfo.unions.dlElecTestResultInfo.runCount++;
				
				//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
				WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
				
				//固定
				Clamp_Motor_Out(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_Out_AddStep);
				
				osDelay(1000);
				
				//判断定量电极是否处于触发状态
				if(ELEC_STATUS != SET){
					ServiceAgingTestUIInfo.unions.dlElecTestResultInfo.activeStatusErrCount++;
				}
				
				//释放
				Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
				
				osDelay(1000);
				
				//判断定量电极是否处于非触发状态
				if(ELEC_STATUS == SET){
					ServiceAgingTestUIInfo.unions.dlElecTestResultInfo.idleStatusErrCount++;
				}
			}
			
			BC_CUR_SW_OFF;
			
			ServiceAgingTestUIInfo.startFlag = 0;
			ServiceAgingTestUIInfo.stopFlag = 1;
			
			//读取RTC结束时间
			RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.dlElecTestResultInfo.rtc.endTime);
			
			//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
			WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
			
			//出仓
			OutIn_Module_Out(MOTOR_MODE_NORMAL);
			
			//关闭面板指示灯
			g_CountHandle_Status = 0;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
			
			//向界面发送后端执行完毕消息
			WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
		break;
		
		
		//进出仓运动老化测试
        case CMD_SERVER_OUTIN_MODULE_OUT_IN:
        {
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			
			memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));
			ServiceAgingTestUIInfo.page = MOTOR_OUT_IN_RESULT;
			ServiceAgingTestUIInfo.startFlag = 1;
			ServiceAgingTestUIInfo.stopFlag = 0;
			
			//先处于出仓位置
			OutIn_Module_In(MOTOR_MODE_NORMAL);
			
			//读取RTC起始时间
			RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.outInTestResultInfo.rtc.startTime);
			ServiceAgingTestUIInfo.unions.outInTestResultInfo.setCount = AgingTestUI2BInfo.count;
			ServiceAgingTestUIInfo.unions.outInTestResultInfo.runCount = 0;
			
			//面板指示灯闪烁
			g_CountHandle_Status = 1;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
			
			while(ServiceAgingTestUIInfo.unions.outInTestResultInfo.runCount < ServiceAgingTestUIInfo.unions.outInTestResultInfo.setCount){
				//判断是否提前中止老化
				if(ServiceAgingTestUIInfo.stopFlag == 1){
					//提前退出循环
					break;
				}
				
				ServiceAgingTestUIInfo.unions.outInTestResultInfo.runCount++;
				
				//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
				WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
				
				if(OutIn_Motor_Out(MOTOR_MODE_NORMAL, MOTOR_OUTIN_MAX_STEP,0) != ERROR_CODE_SUCCESS){
					ServiceAgingTestUIInfo.unions.outInTestResultInfo.outErrCount++;
				}
				
				if(Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, 0) != ERROR_CODE_SUCCESS){
					ServiceAgingTestUIInfo.unions.outInTestResultInfo.sfErrCount++;
				}
				
				if(Clamp_Motor_Out(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, 0) != ERROR_CODE_SUCCESS){
					ServiceAgingTestUIInfo.unions.outInTestResultInfo.gdErrCount++;
				}
				
				if(OutIn_Motor_In(MOTOR_MODE_NORMAL, MOTOR_OUTIN_MAX_STEP,0) != ERROR_CODE_SUCCESS){
					ServiceAgingTestUIInfo.unions.outInTestResultInfo.inErrCount++;
				}
			}
			
			ServiceAgingTestUIInfo.startFlag = 0;
			ServiceAgingTestUIInfo.stopFlag = 1;
			
			//读取RTC结束时间
			RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.outInTestResultInfo.rtc.endTime);
			
			//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
			WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
			
			//出仓
			OutIn_Module_Out(MOTOR_MODE_NORMAL);
			
			//关闭面板指示灯
			g_CountHandle_Status = 0;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
			
			//向界面发送后端执行完毕消息
			WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
        }
        break;
		
		//CAT数据发送测试
		case CMD_SERVER_CAT_DATA_TEST:
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			
			CATDataTest(AgingTestUI2BInfo.count);
		
			//向界面发送后端执行完毕消息
			WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
		break;
		
		//随机点击测试
		case CMD_SERVER_RAND_TOUCH_TEST:
		{
			extern uint32_t RandomTouchTime;
			extern osThreadId_t RandonTouchTestTaskHandle;
			extern const osThreadAttr_t RandonTouchTestTask_attributes;
			
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			RandomTouchTime = AgingTestUI2BInfo.count;
			
			//临时创建一个任务来执行随机点击测试
			RandonTouchTestTaskHandle = osThreadNew(RandonTouchTestTask, NULL, &RandonTouchTestTask_attributes);
		}
		break;
		
		//生产老化测试
		case CMD_SERVER_SHENGCHAN_AGING:
		{
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			ShenChanAgingTest(AgingTestUI2BInfo.count);
		
			//向界面发送后端执行完毕消息
			WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
		}
		break;
		
		//数据存储测试
		case CMD_SERVER_DATA_SAVE_TEST:
		{
			AgingTestUI2BInfo = *((AgingTestUIToBackendInfo_s*)pMsg);
			DataSaveTest(AgingTestUI2BInfo.count);
		
			//向界面发送后端执行完毕消息
			WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
		}
		break;
		
		
		//===服务--PCBA的后端执行====
		//主控板PCBA检验
		case CMD_SERVER_PCBA_CHECK_MAIN:
		{
			PCBA_MainBoard();
		}
		break;
		
		//转接板PCBA检验
		case CMD_SERVER_PCBA_CHECK_ZJ:
		{
			PCBA_ZJBoard();
		}
		break;
		
		//CAT检验
		case CMD_SERVER_PCBA_CHECK_CAT:
		{
			PCBA_CAT();
		}
		break;
		
		//触摸屏检验
		case CMD_SERVER_PCBA_CHECK_TOUCH:
		{
			PCBA_TouchScreen();
		}
		break;
		
		//电机检验
		case CMD_SERVER_PCBA_CHECK_MOTO:
		{
			PCBA_Moto();
		}
		break;
		
		//压力传感器检验
		case CMD_SERVER_PCBA_CHECK_PRESURE:
		{
//			PCBA_Pressure();
		}
		break;
		
		//阀检验
		case CMD_SERVER_PCBA_CHECK_VALVE:
		{
			PCBA_Valve();
		}
		break;
		
		//泵检验
		case CMD_SERVER_PCBA_CHECK_PUMP:
		{
			PCBA_Pump();
		}
		break;
		
		//HONE检验
		case CMD_SERVER_PCBA_CHECK_HONE:
		{
			PCBA_Pressure(1);
		}
		break;
		
		//IT110检验
		case CMD_SERVER_PCBA_CHECK_IT110:
		{
			PCBA_Pressure(0);
		}
		break;
		
		//PCBA压力校准
		case CMD_SERVER_PCBA_PRESSURE_ADJUST:
		{
			PCBA_PressureAdjust();
		}
		break;
		
        //HGB测试
        case CMD_SET_HGB_TEST:
        {
            extern __IO osSemaphoreId_t xSema_Count_Key;
            //
            osSemaphoreRelease(xSema_Count_Key); 
        }
        break;		
		
		//HGB调校界面中的HGB测试
        case CMD_SET_HGB_ADJUST_TEST:
        {
            HGBAdjust_HGBTest();
        }
        break;	
		
		//HGB调校界面中的开415灯
        case CMD_SET_HGB_ADJUST_OPEN415:
        {
            HGBAdjust_Open415();
        }
        break;	
		
		//HGB调校界面中的开535灯
        case CMD_SET_HGB_ADJUST_OPEN535:
        {
            HGBAdjust_Open535();
        }
        break;

		//HGB调校界面中的空气校准按钮
        case CMD_SET_HGB_ADJUST_KQ_AUTO:
        {
            HGBAdjust_KQAuto();
        }
        break;	
		
		//HGB调校界面中的空白校准按钮
        case CMD_SET_HGB_ADJUST_KB_AUTO:
        {
            HGBAdjust_KBAuto();
        }
        break;
		
		
        default:break;
    }

}




/*
*   UI发送过来的登录相关消息
*/
static void BackEnd_Login_Handler(uint8_t *pMsg)
{
    extern __IO OutIn_Module_Position_e g_eOutIn_Key;
    extern __IO osSemaphoreId_t xSema_OutIn_Key;
	extern  MachInfo_s	MachInfo;
	extern osMessageQueueId_t CATQueueHandle;
	extern osMessageQueueId_t CATQueueOtherHandle;
	CATQueueInfoBuf_s CATQueueInfoBuf;
    
    Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
    switch(pMsgHead->usCmd)
    {
        //登录自检
        case CMD_LOGIN_SELFCHECK:
        case CMD_LOGIN_ERROR_CLEAR:
        {
            //自检
            Login_SelfCheck(pMsgHead->usCmd);
        }
        break;
        case CMD_LOGIN_OK_LOAD_USER_DATA:
        {
            //账号登录校验完成后，加载用户数据
            Load_Account_Data();
			
			//判断是否需要发送外部账号数据
			if(MachInfo.catMastSynServEvent.bit.outtorSampleResult == 1){				//样本分析结果必发项
				CATQueueInfoBuf.msgType = SAMPLE_ANALYSE_CAT_MSG_TYPE;
				osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
			}
			
			if(MachInfo.catMastSynServEvent.bit.otherOuttorSampleResult == 1){			//样本分析结果必发项，第三方服务端
				CATQueueInfoBuf.msgType = SAMPLE_ANALYSE_CAT_MSG_TYPE;
				osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0,0);
			}
			
			
			if(MachInfo.catMastSynServEvent.bit.outtorChangeSampleResult == 1){			//修改样本分析结果必发项
				CATQueueInfoBuf.msgType = CHANGE_SAMPLE_ANALYSE_CAT_MSG_TYPE;
				osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
			}
        }
        break;

        default:break;
    }   
}



/*
*   UI发送过来的登录相关消息
*/
static void BackEnd_PowerOff_Handler(uint8_t *pMsg)
{

    Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
    switch(pMsgHead->usCmd)
    {
        
        case CMD_POWEROFF_EXEC:
        {
            //关机， 执行进仓，关屏
           PowerOff_Exec(pMsg);
        }
        break;

        default:break;
    }   
}


/*
*   后端其他从UI发送过来的消息
*/
static void BackEnd_OthersMsg_Handler(uint8_t *pMsg)
{
    extern __IO OutIn_Module_Position_e g_eOutIn_Key;
    extern __IO osSemaphoreId_t xSema_OutIn_Key;
    uint8_t i = 0;
    
    Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
    switch(pMsgHead->usCmd)
    {
        case CMD_OTHERS_OUTIN_MODULE_OUT:
        case CMD_OTHERS_OUTIN_MODULE_IN:
        {
            extern __IO uint8_t g_OutIn_Module_Status;
            //当进出电机，还未执行完成时，等待其完成
            while(1 == g_OutIn_Module_Status)
            {
                //未执行完成
                osDelay(100);
                if(0 == g_OutIn_Module_Status || ++i > 70) //进出仓模块执行完成，或超过7s
                {
                    break;
                }
            }
            //执行进仓或出仓操作
            if(CMD_OTHERS_OUTIN_MODULE_OUT == pMsgHead->usCmd)
            {
                //进出仓模块，出仓，
                g_eOutIn_Key = OUTIN_MODULE_POSITION_OUT;
                osSemaphoreRelease(xSema_OutIn_Key);
            }else if(CMD_OTHERS_OUTIN_MODULE_IN == pMsgHead->usCmd){

                //进出仓模块，进仓，
                g_eOutIn_Key = OUTIN_MODULE_POSITION_IN;
                osSemaphoreRelease(xSema_OutIn_Key);
            }
        }break;
		
		//直接执行进仓操作
		case CMD_OTHERS_OUTIN_IN:
		{
			OutIn_Module_In(MOTOR_MODE_NORMAL);
		}break;
		
		//直接执行出仓操作
		case CMD_OTHERS_OUTIN_OUT:
		{
			OutIn_Module_Out(MOTOR_MODE_NORMAL);
		}break;
		
		//启动计数操作
		case CMD_OTHERS_START_COUNT:
		{
			extern __IO osSemaphoreId_t xSema_Count_Key;
			osSemaphoreRelease(xSema_Count_Key);
		}break;

        default:break;
    }   
}


/*
* 加载质控，分析/列表数据
*/
void Load_QC_WBCHGB_Data(void)
{
    extern MachInfo_s MachInfo;
    extern __IO MachRunPara_s MachRunPara;
    
    uint8_t ucIndex = QC_FILENUM_INDEX_END, ucMaxIndex = QC_FILENUM_INDEX_END;
    
    //获取最新（大）的文件号
    for(ucIndex = QC_FILENUM_INDEX_0; ucIndex < QC_FILENUM_INDEX_END; ucIndex++)
    {
        if(QC_FILENUM_INDEX_END != MachInfo.qcInfo.set[ucIndex].eFileNumIndex)
        {
            ucMaxIndex = MachInfo.qcInfo.set[ucIndex].eFileNumIndex;
        }else{
            break;
        }
    }
    //获取数据
    if(ucMaxIndex < QC_FILENUM_INDEX_END)
    {
        if(g_tDataManage.tDataManage_QC[ucMaxIndex].ulQC_Queue_HeadIndex == g_tDataManage.tDataManage_QC[ucMaxIndex].ulQC_Queue_TailIndex && g_tDataManage.tDataManage_QC[ucMaxIndex].ulQC_Queue_TailIndex == 0)
        {
            //当前文件号，无数据
            //
            Set_Default_QC_WBCHGB_Data();
        }else{
            //读取数据
            FatFs_Read_QC_WBCHGB_Info(ucMaxIndex, g_tDataManage.tDataManage_QC[ucMaxIndex].ulQC_Queue_TailIndex-1, &MachRunPara.tQC_WBCHGB_TestInfo);
            memmove((void*)&MachRunPara.tQC_ListView_Data, (void*)&MachRunPara.tQC_WBCHGB_TestInfo, sizeof(QC_WBCHGB_TestInfo_t));

        }
    }
}


/*
*   后端处理从UI发送过来的质控消息
*/
static void BackEnd_QC_Handler(uint8_t *pMsg)
{
    extern MachInfo_s MachInfo;
    uint8_t* ucaMsgBuffer = (uint8_t*)SDRAM_ApplyCommonBuf(BACKEND_TO_UI_MSG_BUFFER_LEN); //发送信息缓存	
    QC_WBCHGB_TestInfo_t *ptQC_WBCHGB_TestInfo = (QC_WBCHGB_TestInfo_t*)ucaMsgBuffer;
    
    Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
    switch(pMsgHead->usCmd)
    {
        case CMD_QC_UPDATE_ANALYSIS_DATA:          
        case CMD_QC_UPDATE_LIST_DATA:
        {
            //切换文件号时，需更新列表显示数据
            uint8_t ucIndex = 0;
            QC_Msg_t *ptQC_Msg = (QC_Msg_t*)pMsg;
            //QC_WBCHGB_TestInfo_t *ptQC_WBCHGB_TestInfo = (QC_WBCHGB_TestInfo_t*)ucaMsgBuffer;
            //
            ucIndex = ptQC_Msg->ulIndex;
            if(g_tDataManage.tDataManage_QC[ucIndex].ulQC_Queue_HeadIndex != g_tDataManage.tDataManage_QC[ucIndex].ulQC_Queue_TailIndex)
            {   
                //不为空
                FatFs_Read_QC_WBCHGB_Info(MachInfo.qcInfo.set[ucIndex].eFileNumIndex, g_tDataManage.tDataManage_QC[ucIndex].ulQC_Queue_TailIndex-1, ptQC_WBCHGB_TestInfo);
            }else{
                ptQC_WBCHGB_TestInfo->eFileNumIndex = QC_FILENUM_INDEX_END;
                ptQC_WBCHGB_TestInfo->ulCurDataIndex = INVALID_DATA_SN;
                ptQC_WBCHGB_TestInfo->usTestCapacity = QC_PER_FILE_NUM_MAX_CAPACITY;
            }
            //构造新消息
            ptQC_WBCHGB_TestInfo->tMsgHead.usCmd = pMsgHead->usCmd;
            BackEnd_Put_Msg(ucaMsgBuffer);
        }
        break;
        case CMD_QC_LIST_LAST_RECORD:
        {
            //质控分析、上一记录
            LastRecord_t *ptLastRecord = (LastRecord_t*)pMsg;
            //__IO QC_WBCHGB_TestInfo_t *ptQC_WBCHGB_TestInfo = (QC_WBCHGB_TestInfo_t*)ucaMsgBuffer;
            
            FatFs_Read_QC_WBCHGB_Info(ptLastRecord->eFileNumIndex, ptLastRecord->ulCurDataIndex, ptQC_WBCHGB_TestInfo);
            //
            if(INVALID_DATA_SN != ptQC_WBCHGB_TestInfo->ulLastValidIndex && ptQC_WBCHGB_TestInfo->eFileNumIndex < QC_FILENUM_INDEX_END)
            {
                FatFs_Read_QC_WBCHGB_Info(ptLastRecord->eFileNumIndex, ptQC_WBCHGB_TestInfo->ulLastValidIndex, ptQC_WBCHGB_TestInfo);
            }
            
            //发送消息/数据，给UI
            ptQC_WBCHGB_TestInfo->tMsgHead.usCmd = CMD_QC_LIST_LAST_RECORD;
            BackEnd_Put_Msg((uint8_t*)ucaMsgBuffer);

        }
        break;
        case CMD_QC_LIST_NEXT_RECORD:
        {
            //质控分析、下一记录
            //__IO QC_WBCHGB_TestInfo_t *ptQC_WBCHGB_TestInfo = (QC_WBCHGB_TestInfo_t*)ucaMsgBuffer;
            LastRecord_t *ptNextRecord = (LastRecord_t*)pMsg;
            
            FatFs_Read_QC_WBCHGB_Info(ptNextRecord->eFileNumIndex, ptNextRecord->ulCurDataIndex, ptQC_WBCHGB_TestInfo);
            //
            if(INVALID_DATA_SN != ptQC_WBCHGB_TestInfo->ulNextValidIndex && ptQC_WBCHGB_TestInfo->eFileNumIndex < QC_FILENUM_INDEX_END)
            {
                FatFs_Read_QC_WBCHGB_Info(ptNextRecord->eFileNumIndex, ptQC_WBCHGB_TestInfo->ulNextValidIndex, ptQC_WBCHGB_TestInfo);
            }
            
            //发送消息/数据，给UI
            ptQC_WBCHGB_TestInfo->tMsgHead.usCmd = CMD_QC_LIST_NEXT_RECORD;
            BackEnd_Put_Msg((uint8_t*)ucaMsgBuffer);

        }
        break;
        case CMD_QC_PRINT:
        {
            //质控分析、打印
            extern PrinterCurrentState_s PrinterCurrentState;
            Msg_Head_t tMsgHead = {0};
            //
            Print_t *ptPrint = (Print_t*)pMsg;

            //检查打印机状态
            if(PrinterCurrentState.readyState == 0x55){
                //无打印纸
                LOG_Error("Printer No Paper");
                tMsgHead.usCmd = CMD_OTHERS_PRITER_NO_PAPER;
                BackEnd_Put_Msg((uint8_t*)&tMsgHead);
                break;
            }else if(PrinterCurrentState.readyState != 0x55 && PrinterCurrentState.readyState != 0x04){
                //打印机连接异常
                LOG_Error("Printer Conn Error");
                tMsgHead.usCmd = CMD_OTHERS_PRITER_CONN_ERR;
                BackEnd_Put_Msg((uint8_t*)&tMsgHead);
                break;
            }
            if(INVALID_DATA_SN != ptPrint->ulCurDataIndex && ptPrint->ulCurDataIndex != 0 && ptPrint->eFileNumIndex < QC_FILENUM_INDEX_END)
            {
                if(FEED_BACK_SUCCESS == FatFs_Read_QC_WBCHGB_Info(ptPrint->eFileNumIndex, ptPrint->ulCurDataIndex, ptQC_WBCHGB_TestInfo))
                {
                    ptQC_WBCHGB_TestInfo->ucaLotNum[QC_LOT_NUM_LEN-1] = 0;
                    ptQC_WBCHGB_TestInfo->ucaValidDate[DATE_TIME_LEN-1] = 0;
                    CreateAndSend_QC_WBCHGB_PrintData(ptQC_WBCHGB_TestInfo);
                } 
            }
        }
        break;
        case CMD_QC_LIST_DELETE:
        {
            //QC_WBCHGB_TestInfo_t *ptQC_WBCHGB_TestInfo = (QC_WBCHGB_TestInfo_t*)ucaMsgBuffer;
            uint32_t ulNextIndex = 0, ulLastIndex = 0, ulCurIndex = 0;
            QC_WBCHGB_TestInfo_t* tData1 = (QC_WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(QC_WBCHGB_TestInfo_t));
			QC_WBCHGB_TestInfo_t* tData2 = (QC_WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(QC_WBCHGB_TestInfo_t));
            //
            QC_Record_t *ptQC_Record = (QC_Record_t*)pMsg;
            if(ptQC_Record->eFileNumIndex < QC_FILENUM_INDEX_END)
            {
                //获取当前数据
                FatFs_Read_QC_WBCHGB_Info(ptQC_Record->eFileNumIndex, ptQC_Record->ulCurDataIndex, ptQC_WBCHGB_TestInfo);
                //
                if(ptQC_WBCHGB_TestInfo->ulNextValidIndex == INVALID_DATA_SN &&  ptQC_WBCHGB_TestInfo->ulLastValidIndex == INVALID_DATA_SN)
                {
                    //只剩一条
                    ptQC_WBCHGB_TestInfo->eDataStatus  = DATA_STATUS_DELETE;
                    ptQC_WBCHGB_TestInfo->eFileNumIndex = QC_FILENUM_INDEX_END;
                    ptQC_WBCHGB_TestInfo->ulCurDataIndex = INVALID_DATA_SN;   
                    FatFs_Write_QC_WBCHGB_Info(ptQC_Record->eFileNumIndex, ptQC_Record->ulCurDataIndex, ptQC_WBCHGB_TestInfo);
                    //更新TailIndex
                    Set_QC_TailIndex(ptQC_Record->eFileNumIndex, 0);
                    Save_DataMange_Info(&g_tDataManage);
                    //发送消息给UI,更新
                    ptQC_WBCHGB_TestInfo->tMsgHead.usCmd = CMD_QC_LIST_DELETE;
                    BackEnd_Put_Msg(ucaMsgBuffer);
                    
                }else if(ptQC_WBCHGB_TestInfo->ulNextValidIndex != INVALID_DATA_SN &&  ptQC_WBCHGB_TestInfo->ulLastValidIndex == INVALID_DATA_SN){
                    /*** 删除最新的一条 ***/
                    //修改回写
                    ulNextIndex = ptQC_WBCHGB_TestInfo->ulNextValidIndex; //较旧的
                    ulCurIndex = ptQC_WBCHGB_TestInfo->ulCurDataIndex;
                    //
                    ptQC_WBCHGB_TestInfo->eDataStatus  = DATA_STATUS_DELETE;
                    ptQC_WBCHGB_TestInfo->eFileNumIndex = QC_FILENUM_INDEX_END;
                    ptQC_WBCHGB_TestInfo->ulCurDataIndex = INVALID_DATA_SN;  
                    ptQC_WBCHGB_TestInfo->ulNextValidIndex = INVALID_DATA_SN;
                    FatFs_Write_QC_WBCHGB_Info(ptQC_Record->eFileNumIndex, ulCurIndex, ptQC_WBCHGB_TestInfo);
                    //读取下一条,修改，回写
                    FatFs_Read_QC_WBCHGB_Info(ptQC_Record->eFileNumIndex, ulNextIndex, ptQC_WBCHGB_TestInfo);
                    ptQC_WBCHGB_TestInfo->ulLastValidIndex = INVALID_DATA_SN;
                    FatFs_Write_QC_WBCHGB_Info(ptQC_Record->eFileNumIndex, ptQC_WBCHGB_TestInfo->ulCurDataIndex, ptQC_WBCHGB_TestInfo);   
                    //
                    //更新TailIndex
                    Set_QC_TailIndex(ptQC_Record->eFileNumIndex, ulCurIndex-1);
                    Save_DataMange_Info(&g_tDataManage);
                    //发送消息给UI,更新
                    ptQC_WBCHGB_TestInfo->tMsgHead.usCmd = CMD_QC_LIST_DELETE;
                    BackEnd_Put_Msg(ucaMsgBuffer);
                
                }else if(ptQC_WBCHGB_TestInfo->ulNextValidIndex != INVALID_DATA_SN &&  ptQC_WBCHGB_TestInfo->ulLastValidIndex != INVALID_DATA_SN){
                    /*** 删除中间的数据 **/
                    ulNextIndex = ptQC_WBCHGB_TestInfo->ulNextValidIndex; //较旧的
                    ulCurIndex  = ptQC_WBCHGB_TestInfo->ulCurDataIndex;
                    ulLastIndex = ptQC_WBCHGB_TestInfo->ulLastValidIndex; //较新的
                    //
                    ptQC_WBCHGB_TestInfo->eDataStatus  = DATA_STATUS_DELETE;
                    ptQC_WBCHGB_TestInfo->eFileNumIndex = QC_FILENUM_INDEX_END;
                    ptQC_WBCHGB_TestInfo->ulCurDataIndex = INVALID_DATA_SN;  
                    ptQC_WBCHGB_TestInfo->ulLastValidIndex = INVALID_DATA_SN;
                    ptQC_WBCHGB_TestInfo->ulNextValidIndex = INVALID_DATA_SN;
                    FatFs_Write_QC_WBCHGB_Info(ptQC_Record->eFileNumIndex, ulCurIndex, ptQC_WBCHGB_TestInfo);
                    //读取上/下一条,修改，回写
                    FatFs_Read_QC_WBCHGB_Info(ptQC_Record->eFileNumIndex, ulLastIndex, tData2); //较新
                    FatFs_Read_QC_WBCHGB_Info(ptQC_Record->eFileNumIndex, ulNextIndex, tData1);
                    //
                    tData2->ulNextValidIndex = tData1->ulCurDataIndex;
                    tData1->ulLastValidIndex = tData2->ulCurDataIndex;
                    FatFs_Write_QC_WBCHGB_Info(ptQC_Record->eFileNumIndex, tData2->ulCurDataIndex, tData2);
                    FatFs_Write_QC_WBCHGB_Info(ptQC_Record->eFileNumIndex, tData1->ulCurDataIndex, tData1);                    
                    //
                    tData2->tMsgHead.usCmd = CMD_QC_LIST_DELETE;
                    BackEnd_Put_Msg((uint8_t*)&tData2);
                }
            }
 
        }
        break;
        case CMD_QC_LOAD_ANALYSIS_LIST_DATA:
        {
            //质控，加载质控样本分析、列表显示数据（加载当前最新的一条数据）
            //QC_WBCHGB_TestInfo_t *ptQC_WBCHGB_TestInfo = (QC_WBCHGB_TestInfo_t*)ucaMsgBuffer;           
            uint8_t ucIndex = QC_FILENUM_INDEX_END, ucMaxIndex = QC_FILENUM_INDEX_END;
 //           uint32_t ulIndex = 0;
            
            //获取最新（大）的文件号
            for(ucIndex = QC_FILENUM_INDEX_0; ucIndex < QC_FILENUM_INDEX_END; ucIndex++)
            {
                if(QC_FILENUM_INDEX_END != MachInfo.qcInfo.set[ucIndex].eFileNumIndex)
                {
                    ucMaxIndex = MachInfo.qcInfo.set[ucIndex].eFileNumIndex;
                }else{
                    break;
                }
            }
            //获取数据
            if(ucMaxIndex < QC_FILENUM_INDEX_END)
            {
                if(g_tDataManage.tDataManage_QC[ucMaxIndex].ulQC_Queue_HeadIndex == g_tDataManage.tDataManage_QC[ucMaxIndex].ulQC_Queue_TailIndex && g_tDataManage.tDataManage_QC[ucMaxIndex].ulQC_Queue_TailIndex == 0)
                {
                    //当前文件号，无数据
                    //
                    Set_Default_QC_WBCHGB_Data();
                }else{
                    //读取数据
                    FatFs_Read_QC_WBCHGB_Info(ucMaxIndex, g_tDataManage.tDataManage_QC[ucMaxIndex].ulQC_Queue_TailIndex-1, ptQC_WBCHGB_TestInfo);
                    //发送消息给UI
                    ptQC_WBCHGB_TestInfo->tMsgHead.usCmd = CMD_QC_LOAD_ANALYSIS_LIST_DATA;
                    BackEnd_Put_Msg((uint8_t*)ptQC_WBCHGB_TestInfo);
                }
            }
        }
        break;
        case CMD_QC_SET_RE_READ_QC_INFO:
        {
            //重新读取（刷新），质控设定信息
            extern __IO MachRunPara_s MachRunPara;
            EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_QC_INFO,(uint8_t*)&MachInfo.qcInfo, sizeof(MachInfo.qcInfo));
            //
            memmove((void*)&MachRunPara.taQC_SetInfo, (void*)&MachInfo.qcInfo.set, sizeof(QC_SetInfo_s)*QC_FILENUM_INDEX_END);
        }
        break;
        case CMD_QC_SET_DELETE:
        {
            //删除指定的质控设定信息
            uint8_t ucCurIndex = 0, ucMaxIndex = 0;
            
            QC_SetInfo_Msg_t *ptQC_SetInfo_Msg = (QC_SetInfo_Msg_t*)pMsg;
            ucCurIndex = ptQC_SetInfo_Msg->ucCurIndex;
            ucMaxIndex = ptQC_SetInfo_Msg->ucMaxIndex;
            
            //删除设定信息，对于的数据管理信息（数据管理信息被清除，数据将不会被索引到）
            g_tDataManage.tDataManage_QC[MachInfo.qcInfo.set[ptQC_SetInfo_Msg->ucCurIndex].eFileNumIndex].ulQC_SN = INVALID_DATA_SN;
            g_tDataManage.tDataManage_QC[MachInfo.qcInfo.set[ptQC_SetInfo_Msg->ucCurIndex].eFileNumIndex].ulQC_Queue_HeadIndex = 0;
            g_tDataManage.tDataManage_QC[MachInfo.qcInfo.set[ptQC_SetInfo_Msg->ucCurIndex].eFileNumIndex].ulQC_Queue_TailIndex = 0;
            g_tDataManage.tDataManage_QC[MachInfo.qcInfo.set[ptQC_SetInfo_Msg->ucCurIndex].eFileNumIndex].ulValidNum = 0;
            //
            Save_DataMange_Info(&g_tDataManage);
            
            //删除设定信息
            if(ucCurIndex < QC_FILENUM_INDEX_END && ucMaxIndex < QC_FILENUM_INDEX_END)
            {               
                //更新文件数组信息（索引号由低到高排列（旧到新），多余项为无效后数据）
                while(ucCurIndex < ucMaxIndex)
                {
                    /*** 删除的文件号，不是最新的一个 ***/
                    
                    //把后续项，覆盖前面项     
                    memmove((void*)&MachInfo.qcInfo.set[ucCurIndex], (void*)&MachInfo.qcInfo.set[ucCurIndex+1], sizeof(QC_SetInfo_s));
                    //MachInfo.qcInfo.set[ucCurIndex].eFileNumIndex = ucCurIndex;
                    ucCurIndex++;
                }
                //无效后，最后一项
                memset((void*)&MachInfo.qcInfo.set[ucMaxIndex], 0, sizeof(QC_SetInfo_s));
                MachInfo.qcInfo.set[ucMaxIndex].eFileNumIndex = QC_FILENUM_INDEX_END;
                MachInfo.qcInfo.set[ucMaxIndex].usFileNum = 0;   
            }
            //
            EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32, EPPROM_ADDR_MACHINFO_QC_INFO,(uint8_t*)&MachInfo.qcInfo,sizeof(MachInfo.qcInfo));
        }
        break;
        case CMD_QC_SET_SAVE:
        {
           //保存指定的质控设定信息,
            QC_SetInfo_Msg_t *ptQC_SetInfo_Msg = (QC_SetInfo_Msg_t*)pMsg;
            if(ptQC_SetInfo_Msg->ucCurIndex < QC_FILENUM_INDEX_END)
            {
                memmove((void*)&MachInfo.qcInfo.set[ptQC_SetInfo_Msg->ucCurIndex], &ptQC_SetInfo_Msg->tQC_SetInfo, sizeof(QC_SetInfo_s));
            
            }
            //
            EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32, EPPROM_ADDR_MACHINFO_QC_INFO, (uint8_t*)&MachInfo.qcInfo, sizeof(MachInfo.qcInfo));
        }
        break;

        default:break;
    }   
}




/*
*   后端消息处理， 处理UI发送过来的信息
*/
void BackEnd_Msg_Handler(void)
{
    extern osMessageQueueId_t xUIQueue;
    uint8_t ucMsgBuffer[UI_TO_BACKEND_MSG_BUFFER_LEN] = {0};
    uint8_t ucMsgType = 0;
    
    //检测后端UI是否有消息发送，一直等待, 并接受UI发送过来的信息内容
	memset((void*)ucMsgBuffer, 0, sizeof(ucMsgBuffer));
    //osMessageQueueGet(xUIQueue, ucMsgBuffer, 0, osWaitForever);
	BackEnd_Get_Msg(ucMsgBuffer);
		
	Msg_Head_t *pMsgHead = (Msg_Head_t*)ucMsgBuffer;
    ucMsgType = pMsgHead->usCmd >> 12;
    //
    switch(ucMsgType)
    {
        case CMD_ANALYSIS:
        {
            BackEnd_AnalysisMsg_Handler(ucMsgBuffer);
        }
        break;
        case CMD_LIST:
        {
            BackEnd_ListMsg_Handler(ucMsgBuffer);
        }
        break;
        case CMD_SET:
        {
            BackEnd_SetMsg_Handler(ucMsgBuffer);
        }
        break;
        case CMD_SERVER:
        {
            BackEnd_ServerMsg_Handler(ucMsgBuffer);
        }
        break;
        case CMD_LOGIN:
        {
            BackEnd_Login_Handler(ucMsgBuffer);
        }
        break;
        case CMD_POWEROFF:
        {
            BackEnd_PowerOff_Handler(ucMsgBuffer);
        }
        break;
        case CMD_OTHERS:
        {
            BackEnd_OthersMsg_Handler(ucMsgBuffer);
        }
        break;
        case CMD_QC:
        {
            BackEnd_QC_Handler(ucMsgBuffer);
        }
        break;
        default:
		{
			LOG_Info("BackEnd_Msg_Handler: Error CMD Type = %d", ucMsgType);
		}
		break;
    }
}





















