#include "msg_def.h"
#include "ui_msg.h"
#include "bc.h"
#include "main.h"
#include "cmsis_os2.h"
#include "WM.h"
#include "cx_menu.h"
#include "Public_menuDLG.h"
#include "process.h"
#include "file_operate.h"
#include "parameter.h"
#include "Common.h"




osMessageQueueId_t xUIQueue = NULL; //UI前端消息队列，UI往其中发送消息，后端处理消息



                                                

/*
*		UI发送消息，后端将获取
*/
int UI_Put_Msg(uint8_t *pMsg)
{
	extern osMessageQueueId_t xUIQueue;
	int status;
	
	status = osMessageQueuePut(xUIQueue, (void*)pMsg, 0, 0x00);
    if(osOK != status)
    {
        LOG_Error("Put Msg Error");
    }
	return status;
}



/*
*		UI获取后端发送的消息
*/
int UI_Get_Msg(uint8_t *pMsg)
{
	extern osMessageQueueId_t xBackEndQueue;
	int status;
	
	status = osMessageQueueGet(xBackEndQueue, pMsg, 0, 0x00);
	return status;
}



/*
*   样本分析消息处理
*/
static void UI_AnalysisMsg_Handler(uint8_t *pMsg)
{	
    extern __IO MachRunPara_s	MachRunPara;
    extern __IO WM_HWIN     g_hActiveWin;
	Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
    switch(pMsgHead->usCmd)
    {
        case CMD_ANALYSIS_LAST_RECORD:  //上一条记录
        {
			WBCHGB_TestInfo_t *pstWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)pMsg;
            if(pstWBCHGB_TestInfo->tMsgHead.eErrorCode == ERROR_CODE_FAILURE)
            {
               //@todo (none)
            }else{
                if(Is_Display_Data(pstWBCHGB_TestInfo))
                {
                    memmove((void*)&MachRunPara.tWBCHGB_TestInfo, pMsg, sizeof(WBCHGB_TestInfo_t));
                }	
            }
        }
        break;
        case CMD_ANALYSIS_NEXT_RECORD: //下一条记录
        {
//            WBCHGB_TestInfo_t *pstWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)pMsg;
            //@todo load data error prompt
            memmove((void*)&MachRunPara.tWBCHGB_TestInfo, pMsg, sizeof(WBCHGB_TestInfo_t));
        }
        break;
        case CMD_ANALYSIS_PRINT:
        {
        
        }
        break;
        case CMD_ANALYSIS_LOAT_DATA_FAIL:
        {
            //提示数据加载失败
            WM_SendMessageNoPara(g_hActiveWin, WM_ANALYSIS_LOAT_DATA_FAIL);
        }
        break;
        case CMD_ANALYSIS_PROMPT_COUNTING:
        {
            //隐藏公共UI按键（样本分析，列表回顾等）
            Public_Hide_Widget();
            
            WM_SetCallback(WM_HBKWIN, AlphaBk_cbBkWin); 
            GUI_EndDialog(g_hActiveWin, 0);
            g_hActiveWin = CountingPrompt_CreateWindow();
			GUI_Exec();
        }
        break;
        case CMD_ANALYSIS_COUNT:
        case CMD_ANALYSIS_COUNT_MODE_AUTO:
        {  
			//计数消息, 无论数据如何，更新
            WBCHGB_TestInfo_t *pstWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)pMsg;
            //赋值数据到样本分析，列表回顾
            memmove((void*)&MachRunPara.tWBCHGB_TestInfo, pMsg, sizeof(WBCHGB_TestInfo_t));
            Update_UI_ListView_Data(CMD_LIST_REVIEW_SYNC, 0);
            
            if(CMD_ANALYSIS_COUNT == pMsgHead->usCmd)
            {
                //计数过程错误发生,并且计数数据无效
                if(pstWBCHGB_TestInfo->tMsgHead.eErrorCode != ERROR_CODE_SUCCESS && pstWBCHGB_TestInfo->eDataStatus == DATA_STATUS_COUNT_INVALID)
                {
                    //有错误发生,给界面发送消息（有界面交互）
                    WM_MESSAGE tMsg;
                    tMsg.hWin   = g_hActiveWin;
                    tMsg.MsgId  = WM_ANALYSIS_ERROR_PROMPT;
                    tMsg.Data.v = pstWBCHGB_TestInfo->tMsgHead.eErrorCode;
                    //
                    WM_SendMessage(g_hActiveWin, &tMsg);
                }
                
                //计数过程无错误,并且计数数据有效
                if(pstWBCHGB_TestInfo->tMsgHead.eErrorCode == ERROR_CODE_SUCCESS && pstWBCHGB_TestInfo->eDataStatus == DATA_STATUS_NORMAL)
                {
                    //无错误发生,给界面发送消息（有界面交互）
                    WM_MESSAGE tMsg;
                    tMsg.hWin   = g_hActiveWin;
                    tMsg.MsgId  = WM_ANALYSIS_NORMAL_PROMPT;
                    tMsg.Data.v = pstWBCHGB_TestInfo->tMsgHead.eErrorCode;
                    //
                    WM_SendMessage(g_hActiveWin, &tMsg);
                }
            }else if(CMD_ANALYSIS_COUNT_MODE_AUTO == pMsgHead->usCmd){
                //外部工装信号仿真模拟测试,纯信号,自动执行。 只更新样本分析界面数据，不做界面交互切换
                    WM_MESSAGE tMsg;
                    tMsg.hWin   = g_hActiveWin;
                    tMsg.MsgId  = WM_ANALYSIS_GZ_AUTO_MODE_UPDATE_DATA;
                    tMsg.Data.v = pstWBCHGB_TestInfo->tMsgHead.eErrorCode;
                    //
                    WM_SendMessage(g_hActiveWin, &tMsg); 
            }
        }
        break;
        case CMD_ANALYSIS_MOTOR_OUTIN:
        {
        
        }
        break;
		
		//弹出输入患者编码界面
		case CMD_ANALYSIS_INPUT_USER_NUM:
        {
			extern WM_HWIN CreateInputUserNumPage(void);
			
			switch(MachInfo.companyInfo.company){
				case COMPANY_YSB:
				{
					//药师帮
					extern osEventFlagsId_t GlobalEventCatGroupHandle;
					extern osMessageQueueId_t CATQueueOtherHandle;
					CommonDialogPageData_s DialogPageData = {0};
					Msg_Head_t BackendMsg = {0};
					CATQueueInfoBuf_s CATQueueInfoBuf = {0};
					WM_MESSAGE pMsg;
					
					GUI_EndDialog(g_hActiveWin, 0);
					g_hActiveWin = CreateDialogPage();
					
					//保存对话框内容
					DialogPageData.bmpType = BMP_ALARM;
					DialogPageData.confimButt = NO_CONFIM_BUTTON;
					DialogPageData.fun = Analysis_Menu;
					strcpy(DialogPageData.str,g_ucaLng_Analyse_RequestParientInfo[MachInfo.systemSet.eLanguage]);
					
					pMsg.MsgId = WM_USER_DATA;
					pMsg.Data.p = &DialogPageData;
					WM_SendMessage(g_hActiveWin,&pMsg);
					
					//先清除网络异常事件标志
					osEventFlagsClear(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR);
					
					//往后台发送消息
					BackendMsg.usCmd = CMD_ANALYSIS_REQUEST_USER_NUM;
					UI_Put_Msg((uint8_t*)&BackendMsg);
					
					memset((uint8_t*)&MachRunPara.patientList,0,sizeof(MachRunPara.patientList));
					MachRunPara.patientList.pageNum = 1;
					
					CATQueueInfoBuf.msgType = REQUEST_PATIENT_ID_CAT_MSG_TYPE;
					CATQueueInfoBuf.para.TempU8 = MachRunPara.patientList.pageNum;				//请求第1页的患者信息
					
					//向第三方服务端请求患者信息
					osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0, 0);
				}break;
				
				case COMPANY_HY:
				{
					//和映
					GUI_EndDialog(g_hActiveWin, 0);
					g_hActiveWin = CreateInputUserNumPage();
				}break;
				
				default :break;
			}
        }break;
		
        default:break;
    }
}



/*
*   列表回顾消息处理
*/
static void UI_ListMsg_Handler(uint8_t *pMsg)
{
	Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
    switch(pMsgHead->usCmd)
    {
        case CMD_LIST_REVIEW_LOAT_DATA_FAIL:
        {
            WM_SendMessageNoPara(g_hActiveWin, WM_LIST_REVIEW_LOAT_DATA_FAIL);
        }
        break;
        default:break;
    }
}


/*
*   设置消息处理
*/
static void UI_SetMsg_Handler(uint8_t *pMsg)
{
}



/*
*   服务消息处理
*/
static void UI_ServiceMsg_Handler(uint8_t *pMsg)
{
	Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
    switch(pMsgHead->usCmd)
    {
        case CMD_SET_HGB_TEST:
        {
            WM_MESSAGE Msg = {0};
            HGB_Data_t tHGB_Data = {0};
            memmove((void*)&tHGB_Data, (void*)pMsg, sizeof(HGB_Data_t));
            
            Msg.MsgId  = WM_SET_HGB_TEST;
            Msg.hWin   = g_hActiveWin;
            Msg.Data.p = &tHGB_Data;
            WM_SendMessage(g_hActiveWin, &Msg);
        }
        break;
        case CMD_SET_HGB_GZ_AUTO_ADJUST:
        case CMD_SET_HGB_BLACK_AUTO_ADJUST:
        {
            WM_MESSAGE Msg = {0};
            HGB_AutoAdjust_t tHGB_AutoAjust = {0};
            memmove((void*)&tHGB_AutoAjust, (void*)pMsg, sizeof(HGB_AutoAdjust_t));
            
            //
            if(CMD_SET_HGB_GZ_AUTO_ADJUST == pMsgHead->usCmd)
            {
                Msg.MsgId  = WM_SET_HGB_GZ_AUTO_ADJUST;;
            }else{
                Msg.MsgId  = WM_SET_HGB_BLACK_AUTO_ADJUST;
            }
            Msg.hWin   = g_hActiveWin;
            Msg.Data.p = &tHGB_AutoAjust;
            WM_SendMessage(g_hActiveWin, &Msg);
        }
        break;

        default:break;
    }
}



/*
*   登录消息处理
*/
static void UI_Login_Handler(uint8_t *pMsg)
{
    extern __IO MachRunPara_s MachRunPara;
	Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
    switch(pMsgHead->usCmd)
    {
        case CMD_LOGIN_SELFCHECK:
        {
            memmove((void*)&MachRunPara.tLogin_SelfCheck, pMsg, sizeof(Login_SelfCheck_t));
        }
        break;
        case CMD_LOGIN_ERROR_CLEAR:
        {
            uint8_t i = 0; 
            WM_MESSAGE UserMsg = {0};
            CommonDialogPageData_s DialogPageData = {0};
            memmove((void*)&MachRunPara.tLogin_SelfCheck, pMsg, sizeof(Login_SelfCheck_t));
            
            //检测结束
            if(100 == MachRunPara.tLogin_SelfCheck.ucProgress)
            {
                if(MachRunPara.tLogin_SelfCheck.ucErrorNum > 0)
                {
                    //有故障
                    DialogPageData.bmpType = BMP_ALARM;
                    strcpy(DialogPageData.str, g_ucaLng_Failure[MachInfo.systemSet.eLanguage]);
                }else if(MachRunPara.tLogin_SelfCheck.ucErrorNum == 0){
                    //成功
                    DialogPageData.bmpType = BMP_OK;
                    strcpy(DialogPageData.str, g_ucaLng_Success[MachInfo.systemSet.eLanguage]);
                }
                
                //有故障，更新LED灯显示状态
                Panel_LED_Status_e eStatus = PANEL_LED_STATUS_NORMAL;
                if(MachRunPara.tLogin_SelfCheck.ucErrorNum >= 1)  eStatus = PANEL_LED_STATUS_WARN;
                for(i = 0; i < MachRunPara.tLogin_SelfCheck.ucErrorNum; i++)
                {
                    if(Panel_LED_Status_By_ErrorCode(MachRunPara.tLogin_SelfCheck.eaErrorCodeBuffer[i]) == PANEL_LED_STATUS_ERROR)
                    {
                        eStatus = PANEL_LED_STATUS_ERROR;     
                        break;
                    }                
                }
                g_ePanel_LED_Status = eStatus;
                Panel_Led_Status(RUN_MODE_NORMAL, g_ePanel_LED_Status);  
                    
                //
                DialogPageData.confimButt = HAS_CONFIM_BUTTON;
                DialogPageData.fun = ErrorInfo_CreateWindow;
                //
                UserMsg.MsgId = WM_USER_DATA;
                UserMsg.Data.p = &DialogPageData;
                WM_SendMessage(g_hActiveWin, &UserMsg);
            }            
        }
        break;
        default:break;
    }
}




/*
*   其他消息处理
*/
static void UI_Others_Handler(uint8_t *pMsg)
{
    extern __IO WM_HWIN g_hActiveWin;
    
	Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
    switch(pMsgHead->usCmd)
    {
        case CMD_OTHERS_PRITER_NO_PAPER:
        {
            //打印机缺纸
            WM_SendMessageNoPara(g_hActiveWin, WM_PRINTER_NO_PAPER);      
        }
        break;
        case CMD_OTHERS_PRITER_CONN_ERR:
        {
            //打印连接错误
            WM_SendMessageNoPara(g_hActiveWin, WM_PRINTER_CONN_ERR);
        }
        break;
        default:
		{
			LOG_Error("CMD = %d", pMsgHead->usCmd);
		}
        break;
    }
}





/*
*   UI处理从后端发送过来的质控消息
*/
static void UI_QC_Handler(uint8_t *pMsg)
{ 
    Msg_Head_t *pMsgHead = (Msg_Head_t*)pMsg;
    switch(pMsgHead->usCmd)
    {     
        case CMD_QC_ANALYSIS_COUNT:
        case CMD_QC_ANALYSIS_COUNT_MODE_AUTO:
        {
            /*** 质控模式下，不进行界面交互 ***/
            
			//计数消息, 无论数据如何，更新
            QC_WBCHGB_TestInfo_t *pstWBCHGB_TestInfo = (QC_WBCHGB_TestInfo_t*)pMsg;
            
            //赋值数据到质控样本分析，质控列表回顾
            memmove((void*)&MachRunPara.tQC_WBCHGB_TestInfo, pMsg, sizeof(QC_WBCHGB_TestInfo_t));
            //当前列表回顾显示的文件号
            if(pstWBCHGB_TestInfo->eFileNumIndex == MachRunPara.eQC_List_FileNum_Index)
            {
                memmove((void*)&MachRunPara.tQC_ListView_Data, (void*)&MachRunPara.tQC_WBCHGB_TestInfo, sizeof(QC_WBCHGB_TestInfo_t));
            }
            //
            WM_MESSAGE tMsg;
            tMsg.hWin   = g_hActiveWin;
            tMsg.MsgId  = WM_ANALYSIS_GZ_AUTO_MODE_UPDATE_DATA;
            tMsg.Data.v = pstWBCHGB_TestInfo->tMsgHead.eErrorCode;
            //
            WM_SendMessage(g_hActiveWin, &tMsg);            
        }
        break;
        case CMD_QC_LOAD_ANALYSIS_LIST_DATA:
        {
            //加载质控，样本分析/列表显示数据
            memmove((void*)&MachRunPara.tQC_WBCHGB_TestInfo, pMsg, sizeof(QC_WBCHGB_TestInfo_t));
            memmove((void*)&MachRunPara.tQC_ListView_Data, pMsg, sizeof(QC_WBCHGB_TestInfo_t));
        }
        break;
        case CMD_QC_UPDATE_LIST_DATA:
        {
            //切换文件号时，需更新列表显示数据
            memmove((void*)&MachRunPara.tQC_ListView_Data, pMsg, sizeof(QC_WBCHGB_TestInfo_t));
        }
        break;
        case CMD_QC_UPDATE_ANALYSIS_DATA:
        {
            //切换文件号时，需更新列表显示数据
            memmove((void*)&MachRunPara.tQC_WBCHGB_TestInfo, pMsg, sizeof(QC_WBCHGB_TestInfo_t));
        }
        break;
//        case CMD_QC_VALID_FILE_NUM:
//        {
//            //提示无效的文件号
//            WM_MESSAGE tMsg;
//            tMsg.hWin   = g_hActiveWin;
//            tMsg.MsgId  = WM_QC_ERROR_PROMPT_VALID_FILE_NUM;
//            //
//            WM_SendMessage(g_hActiveWin, &tMsg); 
//            
//        }
//        break;
//        case CMD_QC_FILENUM_CAPACITY_IS_FULL:
//        {
//            //提示文件号下，数据容量已满，需删除后在测试
//            WM_MESSAGE tMsg;
//            tMsg.hWin   = g_hActiveWin;
//            tMsg.MsgId  = WM_QC_ERROR_FILENUM_CAPACITY_IS_FULL;
//            //
//            WM_SendMessage(g_hActiveWin, &tMsg); 
//        }
//        break;
        case CMD_QC_LIST_NEXT_RECORD:
        case CMD_QC_LIST_LAST_RECORD:
        case CMD_QC_LIST_DELETE:
        {
           //质控分析、上一记录           
           //更新，质控列表显示数据
           memmove((void*)&MachRunPara.tQC_ListView_Data, pMsg, sizeof(QC_WBCHGB_TestInfo_t));
        }
        break;
        case CMD_QC_PRINT:
        {
            //质控分析、打印
        
        }
        break;

        default:break;
    }   
}





/*
*   界面消息处理， 处理后端发送过来的消息,
*/ 
void UI_Msg_Handler(void)
{
	extern __IO WM_HWIN g_hActiveWin;
    extern osMessageQueueId_t xBackEndQueue;
    uint8_t ucMsgType = 0;
    uint8_t ucMsgBuffer[BACKEND_TO_UI_MSG_BUFFER_LEN] = {0};
    
	memset((void*)ucMsgBuffer, 0, (uint32_t)sizeof(ucMsgBuffer));
	if(osOK == UI_Get_Msg(ucMsgBuffer))
    {
        Msg_Head_t *pMsgHead = (Msg_Head_t*)ucMsgBuffer;
		ucMsgType = pMsgHead->usCmd >> 12;
        //
        switch(ucMsgType)
        {
            case CMD_ANALYSIS:
            {
                UI_AnalysisMsg_Handler(ucMsgBuffer);
            }
            break;
            case CMD_LIST:
            {
                UI_ListMsg_Handler(ucMsgBuffer);
            }
            break;
            case CMD_SET:
            {
                UI_SetMsg_Handler(ucMsgBuffer);
            }
            break;
            case CMD_SERVER:
            {
                UI_ServiceMsg_Handler(ucMsgBuffer);
            }
            break;
            case CMD_LOGIN:
            {
                UI_Login_Handler(ucMsgBuffer);
            }
            break;
            case CMD_OTHERS:
            {
                UI_Others_Handler(ucMsgBuffer);
            }
            break;
            case CMD_QC:
            {
                UI_QC_Handler(ucMsgBuffer);
            }
            break;
            default:
			{
				LOG_Info("UI_Msg_Handler: Error CMD Type = %d", ucMsgType);
			}	
			break;
        }
        //刷新当前显示界面
        WM_InvalidateWindow(g_hActiveWin);       
        
		//给界面发送刷新消息
		//WM_MESSAGE Msg = {0};
		//Msg.MsgId = WM_PUBLIC_UPDATE;
		//WM_BroadcastMessage(&Msg);
        //WM_SendMessageNoPara(WM_HBKWIN, WM_MENU_UPDATE);        
        
        //刷新背景（指定区域，编号，用户，时间） 
        WM_SendMessageNoPara(WM_HBKWIN, WM_PUBLIC_UPDATE);        	
    }
}

