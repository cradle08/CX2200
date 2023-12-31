﻿/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.44                          *
*        Compiled Nov 10 2017, 08:53:57                              *
*        (c) 2017 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
/*
*
*   设置界面中：校准界面
*
*/
// USER END

#include "DIALOG.h"

#include "Public_menuDLG.h"
#include "bsp_eeprom.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_WBCHGB_ADJUST (GUI_CALIBRATOION_ID + 0x00)
#define ID_TEXT_SET_PROMPT   	(GUI_CALIBRATOION_ID + 0x01)
#define ID_TEXT_ADJUST_PROMPT	(GUI_CALIBRATOION_ID + 0x02)
#define ID_TEXT_CONTEXT   		(GUI_CALIBRATOION_ID + 0x03)
#define ID_BUTTON_CANCEL   		(GUI_CALIBRATOION_ID + 0x06)
#define ID_BUTTON_SURE   		(GUI_CALIBRATOION_ID + 0x07)
#define ID_EDIT_WBC_PERCENT   	(GUI_CALIBRATOION_ID + 0x08)
#define ID_EDIT_HGB_PERCENT   	(GUI_CALIBRATOION_ID + 0x09)
#define ID_TEXT_ERROR_PROMPT   	(GUI_CALIBRATOION_ID + 0x0A)

// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)

static uint8_t g_sCbr_MoveFlag = 0; //0:窗口未移动，初始状体，1：窗口已移动
#define     CBR_Y_MOVE_LEN   80      //界面Y方向移动的距离
#define     CBR_X_MOVE_LEN   0       //X方向移动的距离
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO Calibration_aDialogCreate[] = {
  { WINDOW_CreateIndirect, "",   ID_WINDOW_WBCHGB_ADJUST, 0, 60, 800, 393, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", 	 ID_TEXT_SET_PROMPT, 	  30, 15, 95, 30, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", 	 ID_TEXT_ADJUST_PROMPT,   125, 20, 120, 30, 0, 0x0, 0 },
  //{ TEXT_CreateIndirect, "",     ID_TEXT_CONTEXT, 		  165, 115, 500, 120, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "",   ID_BUTTON_CANCEL, 		  255, 300, 120, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "", 	 ID_BUTTON_SURE, 		  405, 300, 120, 40, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "",     ID_EDIT_WBC_PERCENT,     290, 160, 100, 30, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "",     ID_EDIT_HGB_PERCENT, 	  290, 200, 100, 30, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "",     ID_TEXT_ERROR_PROMPT, 	  320, 260, 180, 40, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END



/*
*  Calibration  btn callback
*/
static void Calibration_Btn_Callback(WM_MESSAGE* pMsg)
{
//    const unsigned char RoundRadius = 20; //显示区域圆角半径
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
    {
        GUI_RECT Rect;
        WM_GetClientRect(&Rect);

        if (BUTTON_IsPressed(pMsg->hWin)) {
            //填充
			GUI_SetColor(MachInfo.companyInfo.skin.buttPress);
			GUI_AA_FillRoundedRectEx(&Rect,20);
			
			//设置字符前景和背景色
			GUI_SetColor(MachInfo.companyInfo.skin.buttPressFont);
			GUI_SetBkColor(MachInfo.companyInfo.skin.buttPress);
			
            if(ID_BUTTON_SURE == WM_GetId(pMsg->hWin))
            {
                //确定
                GUI_DispStringInRect(g_ucaLng_Sure[MachInfo.systemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }else if(ID_BUTTON_CANCEL == WM_GetId(pMsg->hWin)){
            
                //取消
                GUI_DispStringInRect(g_ucaLng_Cancel[MachInfo.systemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }   
        }
        else {
			//填充
			GUI_SetColor(MachInfo.companyInfo.skin.buttNotPress);
			GUI_AA_FillRoundedRectEx(&Rect,20);
			
			GUI_SetColor(MachInfo.companyInfo.skin.buttBoard);
			GUI_AA_DrawRoundedRectEx(&Rect,19);
			
			//设置字符前景和背景色
			GUI_SetColor(MachInfo.companyInfo.skin.buttNotPressFont);
			GUI_SetBkColor(MachInfo.companyInfo.skin.buttNotPress);
			
            if(ID_BUTTON_SURE == WM_GetId(pMsg->hWin))
            {
                //确定
                GUI_DispStringInRect(g_ucaLng_Sure[MachInfo.systemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }else if(ID_BUTTON_CANCEL == WM_GetId(pMsg->hWin)){
            
                //取消
                GUI_DispStringInRect(g_ucaLng_Cancel[MachInfo.systemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            } 
        }
    }
    break;
    default:
        BUTTON_Callback(pMsg);
        break;
    }
}


/*
*   内容text回调   
*
*        100     200     200
*     40
*     40
*     40
*/
static void Draw_Context(WM_MESSAGE* pMsg)
{
    extern __IO MachRunPara_s MachRunPara;
    const unsigned char RoundRadius  = 3; //倒角弧度
    const unsigned char Hight	     = 40; //每行高度
    const unsigned short Lenght	     = 500; //每行长度

//    switch (pMsg->MsgId)
//    {
//        case WM_PAINT:
//        {
            GUI_RECT Rect, TempRect = {0};
//            WM_GetClientRect(&Rect);
            Rect.x0 = 140;
            Rect.y0 = 115;
            Rect.x1 = Rect.x0 + 500;
            Rect.y1 = Rect.y0 + 120;

            //第一行，背景
            GUI_SetColor(MachInfo.companyInfo.skin.title);
            GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x0+Lenght, Rect.y0+Hight+RoundRadius, RoundRadius);
            //字体颜色
            GUI_SetColor(MachInfo.companyInfo.skin.titleFont);
            GUI_SetBkColor(MachInfo.companyInfo.skin.title);
            //参数
            TempRect.x0 = Rect.x0;           TempRect.y0 = Rect.y0;
            TempRect.x1 = Rect.x0+100;       TempRect.y1 = Rect.y0+Hight;
            GUI_DispStringInRect(g_ucaLng_Param[MachInfo.systemSet.eLanguage], &TempRect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //校准系数(%)
            TempRect.x0 = Rect.x0+100;       TempRect.y0 = Rect.y0;
            TempRect.x1 = Rect.x0+300;       TempRect.y1 = Rect.y0+Hight;
            GUI_DispStringInRect(g_ucaLng_CalibrationFactor[MachInfo.systemSet.eLanguage], &TempRect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            //校准日期
            TempRect.x0 = Rect.x0+300;       TempRect.y0 = Rect.y0;
            TempRect.x1 = Rect.x0+500;       TempRect.y1 = Rect.y0+Hight;
            GUI_DispStringInRect(g_ucaLng_CalibrationDate[MachInfo.systemSet.eLanguage], &TempRect, GUI_TA_VCENTER | GUI_TA_HCENTER);
                       
            
            //第三行，背景
            GUI_SetColor(GUI_WHITE);
            GUI_AA_FillRoundedRect(Rect.x0, Rect.y0+Hight*2-RoundRadius, Rect.x0+Lenght, Rect.y0+Hight*3+RoundRadius, RoundRadius);
            //字体颜色
            GUI_SetColor(GUI_BLACK);
            GUI_SetBkColor(GUI_WHITE);
            //HGB
            TempRect.x0 = Rect.x0;           TempRect.y0 = Rect.y0+Hight*2;
            TempRect.x1 = Rect.x0+100;       TempRect.y1 = Rect.y0+Hight*3;
            GUI_DispStringInRect("HGB", &TempRect, GUI_TA_VCENTER | GUI_TA_HCENTER);
//            //HGB校准系数
//            sprintf(buffer, "%d", (int)MachInfo.calibration.fHGB*100);
//            EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB_PERCENT), buffer);
            //校准日期
            TempRect.x0 = Rect.x0+300;       TempRect.y0 = Rect.y0+Hight*2;
            TempRect.x1 = Rect.x0+500;       TempRect.y1 = Rect.y0+Hight*3;
            GUI_DispStringInRect((char*)MachInfo.calibration.ucaHGB_CalibrationDate, &TempRect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            
            
            //第二行，背景
            GUI_SetColor(GUI_WHITE);
            GUI_AA_FillRoundedRect(Rect.x0, Rect.y0+Hight, Rect.x0+Lenght, Rect.y0+Hight*2, RoundRadius);
            //字体颜色
            GUI_SetColor(GUI_BLACK);
            GUI_SetBkColor(GUI_WHITE);
            //WBC
            TempRect.x0 = Rect.x0;           TempRect.y0 = Rect.y0+Hight;
            TempRect.x1 = Rect.x0+100;       TempRect.y1 = Rect.y0+Hight*2;
            GUI_DispStringInRect("WBC", &TempRect, GUI_TA_VCENTER | GUI_TA_HCENTER);
//            //WBC校准系数
//            sprintf(buffer, "%d", (int)MachInfo.calibration.fWBC*100);
//            EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_WBC_PERCENT), buffer);
            //校准日期
            TempRect.x0 = Rect.x0+300;       TempRect.y0 = Rect.y0+Hight;
            TempRect.x1 = Rect.x0+500;       TempRect.y1 = Rect.y0+Hight*2;
            GUI_DispStringInRect((char*)MachInfo.calibration.ucaWBC_CalibrationDate, &TempRect, GUI_TA_VCENTER | GUI_TA_HCENTER);
           
           
            //外框
            GUI_SetColor(GUI_TEXT_FRAME_COLOR);
            GUI_AA_DrawRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, RoundRadius);
            
            //水平分割线
            GUI_DrawHLine(Rect.y0+Hight, Rect.x0, Rect.x1);
            GUI_DrawHLine(Rect.y0+Hight*2, Rect.x0, Rect.x1);
            
            //垂直分割线
            GUI_DrawVLine(Rect.x0+100, Rect.y0, Rect.y1);
            GUI_DrawVLine(Rect.x0+300, Rect.y0, Rect.y1);
           
            
            //@todo
            //GUI_PNG_Draw(__userlogo, GUI_COUNTOF(__userlogo), 21, 36);

            //TEXT_SetBkColor(pMsg->hWin, GUI_MENU_TEXT_BK_COLOR);
            //TEXT_SetTextColor(pMsg->hWin, GUI_WHITE);

//            GUI_SetColor(GUI_WHITE);
//            GUI_SetBkColor(MachInfo.companyInfo.skin.title);
//            TEXT_SetFont(pMsg->hWin, &GUI_Font16_1);
//            GUI_DispStringAtCEOL((const char*)MachRunPara.tAccountManage.ucaUser[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type], 76, 48);
//        }
//        break;
//        default:
//            TEXT_Callback(pMsg);
//            break;
//    }
}



/*
*   WBC HGB 校准界面，初始化
*/
#define BUFFER_LEN_1  32
static void WBCHGB_Calibration_Msg_Init(WM_MESSAGE* pMsg)
{
    WM_HWIN hItem;
    char buffer[BUFFER_LEN_1] = {0};
    
    //set prompt
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_SET_PROMPT);
	if(MachInfo.systemSet.eLanguage == LANGUAGE_CHINESE){
		TEXT_SetFont(hItem,&HZ_SONGTI_24);
		WM_SetWindowPos(hItem,20, 13+63, 48, 30);
	}else{
		TEXT_SetFont(hItem,&GUI_Font24_ASCII);
		WM_SetWindowPos(hItem,20, 13+63, 84, 30);
	}
	TEXT_SetTextAlign(hItem,TEXT_CF_HCENTER|TEXT_CF_VCENTER);
	TEXT_SetText(hItem,g_ucaLng_Set[MachInfo.systemSet.eLanguage]);
    
    //calibration prompt
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_ADJUST_PROMPT);
    if(MachInfo.systemSet.eLanguage == LANGUAGE_CHINESE){
		WM_SetWindowPos(hItem,68, 20+63, 140, 20);
	}else{
		WM_SetWindowPos(hItem,104, 20+63, 140, 20);
	}
	TEXT_SetTextAlign(hItem,TEXT_CF_LEFT|TEXT_CF_VCENTER);
    TEXT_SetText(hItem, g_ucaLng_ManualCalibration[MachInfo.systemSet.eLanguage]);
    
    //context
//    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_CONTEXT);
//    WM_SetCallback(hItem, Calibration_Text_CallBack);
//    WM_InvalidateWindow(hItem);
    
    
    //wbc calibration percent, 0-100
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_WBC_PERCENT);
    EDIT_SetMaxLen(hItem, 3);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //WBC校准系数
    memset(buffer, 0, BUFFER_LEN_1);
    sprintf(buffer, "%.0f", (100.0f*MachInfo.calibration.fWBC));
    EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_WBC_PERCENT), buffer);  
    EDIT_SetTextColor(hItem, EDIT_CI_CURSOR, GUI_BLACK);
    //WM_SetCallback(hItem, Calibration_Edit_Callback);
    
    //hgb calibration percent, 0-100
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB_PERCENT);
    EDIT_SetMaxLen(hItem, 3);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //HGB校准系数
    memset(buffer, 0, BUFFER_LEN_1);
    sprintf(buffer, "%.0f", (100.0f*MachInfo.calibration.fHGB));
    EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB_PERCENT), buffer);
    EDIT_SetTextColor(hItem, EDIT_CI_CURSOR, GUI_BLACK);
    //WM_SetCallback(hItem, Calibration_Edit_Callback);
    
    //sure btn
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SURE);
    WM_SetCallback(hItem, Calibration_Btn_Callback); 
    
    //canncel btn
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
    WM_SetCallback(hItem, Calibration_Btn_Callback);
    
    //
    g_sCbr_MoveFlag = 0;
}




/*
*   WBC HGB 校准界面，更新
*/
static void WBCHGB_Calibration_Msg_Paint(WM_MESSAGE* pMsg)
{
    extern char* BKBmaDataBuf;
    GUI_RECT Rect;
//    GUI_SetColor(GUI_WHITE);
//    GUI_Clear();
    
    //画背景BMP图
	//GUI_BMP_Draw(BKBmaDataBuf,1,0);
    WM_GetClientRect(&Rect); 
    GUI_SetColor(GUI_WHITE);
    GUI_AA_FillRoundedRect(Rect.x0+10, Rect.y0, Rect.x1-10, Rect.y1+10, 10);
    
    //画线
    WM_GetClientRect(&Rect);   
    GUI_SetColor(GUI_SPLINE_LINE_COLOR);
    GUI_DrawHLine(Rect.y0+55, Rect.x0+10, Rect.x1-10);
    
    //画内容
    Draw_Context(pMsg);
}

/*
*   
*/
#define WBCHGB_CALIBRATE_PARAM_LOW_VALUE    75
#define WBCHGB_CALIBRATE_PARAM_HIGH_VALUE   125
static FeedBack_e Save_Calibration_Param(WM_MESSAGE* pMsg)
{
    extern __IO MachRunPara_s MachRunPara;
    float fValue = 0;
    int   Value  = 0;
    char buffer[8] = {0}, ucModifyFlag = 0;
	float RawWBC = MachInfo.calibration.fWBC;
	float RawHGB = MachInfo.calibration.fHGB;
    //uint8_t ucaSendBuffer[UI_TO_BACKEND_MSG_BUFFER_LEN] = {0};
    CalibrationMsg_t tCalibrationMsg = {0}; 
    //构造人工校准消息
    CalibrationMsg_t *ptCalibrationMsg = (CalibrationMsg_t*)&tCalibrationMsg;
    //head
    ptCalibrationMsg->tMsgHead.usCmd       = CMD_SET_CALIBRATION;
    ptCalibrationMsg->tMsgHead.eErrorCode  =  ERROR_CODE_SUCCESS;
//    ptCalibrationMsg->tMsgHead.usMsgLen    = sizeof(CalibrationMsg_t) - MSG_HEAD_LEN;
    
    //wbc
    memset(buffer, 0, sizeof(buffer));
    EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_WBC_PERCENT), buffer, 4);
    Value  = atoi(buffer);
    
    //WBC有改动
    if(Value != (int)(MachInfo.calibration.fWBC*100))
    {
        //输入范围限制75-125
        if(Value >WBCHGB_CALIBRATE_PARAM_HIGH_VALUE || Value < WBCHGB_CALIBRATE_PARAM_LOW_VALUE) return FEED_BACK_ERROR;
        if(Value >= 100)
        {
            sprintf(buffer, "%d.%d%d", Value/100, Value%100/10, Value%10);
            fValue = (float)atof(buffer);
            ptCalibrationMsg->calibration.fWBC = fValue;
        }else if(Value >= 10 && Value <= 99){
            sprintf(buffer, "0.%d", Value%100);
            fValue = (float)atof(buffer);
            ptCalibrationMsg->calibration.fWBC = fValue;
        }else{
             sprintf(buffer, "0.0%d", Value%10);
            fValue = (float)atof(buffer);
            ptCalibrationMsg->calibration.fWBC = fValue;       
        }
        strncpy((char*)ptCalibrationMsg->calibration.ucaWBC_CalibrationDate, (char*)MachRunPara.ucaLastestDateTime, DATE_TIME_LEN);
        //
        //wbc calibration param
        MachInfo.calibration.fWBC = ptCalibrationMsg->calibration.fWBC;
        strncpy((char*)MachInfo.calibration.ucaWBC_CalibrationDate, (char*)ptCalibrationMsg->calibration.ucaWBC_CalibrationDate, DATE_TIME_LEN);

        ucModifyFlag++;
    }else{
        //没变化
        ptCalibrationMsg->calibration.fWBC = MachInfo.calibration.fWBC;
        strncpy((char*)ptCalibrationMsg->calibration.ucaWBC_CalibrationDate, (char*)MachInfo.calibration.ucaWBC_CalibrationDate, DATE_TIME_LEN);
    }

    //hgb
    memset(buffer, 0, sizeof(buffer));
    EDIT_GetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB_PERCENT), buffer, 4);
    Value  = atoi(buffer);
    //HGB有改动
    if(Value != (int)(MachInfo.calibration.fHGB*100))
    {
        //输入范围限制75-125
        if(Value >WBCHGB_CALIBRATE_PARAM_HIGH_VALUE || Value < WBCHGB_CALIBRATE_PARAM_LOW_VALUE) return FEED_BACK_ERROR;
        if(Value >= 100)
        {
            sprintf(buffer, "%d.%d%d", Value/100, Value%100/10, Value%10);
            fValue = (float)atof(buffer);
            ptCalibrationMsg->calibration.fHGB = fValue;
        }else if(Value >= 10 && Value <= 99){
            sprintf(buffer, "0.%d", Value%100);
            fValue = (float)atof(buffer);
            ptCalibrationMsg->calibration.fHGB = fValue;
        }else{
             sprintf(buffer, "0.0%d", Value%10);
            fValue = (float)atof(buffer);
            ptCalibrationMsg->calibration.fHGB = fValue;   
        }
        strncpy((char*)ptCalibrationMsg->calibration.ucaHGB_CalibrationDate, (char*)MachRunPara.ucaLastestDateTime, DATE_TIME_LEN);
        //hgb 
        MachInfo.calibration.fHGB = ptCalibrationMsg->calibration.fHGB ;
        strncpy((char*)MachInfo.calibration.ucaHGB_CalibrationDate, (char*)ptCalibrationMsg->calibration.ucaHGB_CalibrationDate, DATE_TIME_LEN);  
        //
        ucModifyFlag++;
        //
        
    }else{
        //没变化
        ptCalibrationMsg->calibration.fHGB = MachInfo.calibration.fHGB;
        strncpy((char*)ptCalibrationMsg->calibration.ucaHGB_CalibrationDate, (char*)MachInfo.calibration.ucaHGB_CalibrationDate, DATE_TIME_LEN);
    }
    
    //有修改，
    if(ucModifyFlag > 0)
    {
        //发送消息到后端
        UI_Put_Msg((uint8_t*)&tCalibrationMsg);
        
        //
        extern __IO DataManage_t g_tDataManage;
        extern MachInfo_s MachInfo;
        extern osMessageQueueId_t CATQueueHandle;
         RTC_HYM8563Info_s RTCInfo = {0};
         RunLog_s RunLog = {0};
         uint32_t ulIndex = 0;
         uint16_t Len = 0;
         CATQueueInfoBuf_s CATQueueInfoBuf;
         
         //保存运行日志
		 RunLog.logHead.logLen = sprintf(RunLog.Str+RunLog.logHead.logLen,"Modified the calibration parameters\r\nWBC:%0.2f-->%0.2f, HGB:%0.2f-->%0.2f\r\n",RawWBC,MachInfo.calibration.fWBC,RawHGB,MachInfo.calibration.fHGB);
        RunLog.Str[RunLog.logHead.logLen] = '\0';
        RunLog.logHead.num = g_tDataManage.ulRunLog_SN;//g_tDataManage.lastRunLogNum;
        RunLog.logHead.eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
        RTC_HYM8563GetTime(&RTCInfo);
        sprintf(RunLog.logHead.dataTime,"20%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
                        RTCInfo.year.bcd_h,RTCInfo.year.bcd_l,
                        RTCInfo.month.bcd_h,RTCInfo.month.bcd_l,
                        RTCInfo.day.bcd_h,RTCInfo.day.bcd_l,
                        RTCInfo.hour.bcd_h,RTCInfo.hour.bcd_l,
                        RTCInfo.min.bcd_h,RTCInfo.min.bcd_l,
                        RTCInfo.sec.bcd_h,RTCInfo.sec.bcd_l);
        Len = sprintf(RunLog.logHead.shortStr,"Calibration Parameter");
        RunLog.logHead.shortStr[Len] = '\0';
        
        RunLog.logHead.timestamp = GetCurrentTimestamp(RTCInfo);
        RunLog.usCrc = CRC16((uint8_t*)&RunLog, sizeof(RunLog_s)-2);
        
        ulIndex = Update_RunLog_Tail();
        Fatfs_Write_RunLog(ulIndex, &RunLog);
        
        g_tDataManage.ulRunLog_SN++;//g_tDataManage.lastRunLogNum++;
        Add_RunLog_Tail();
        Save_DataMange_Info(&g_tDataManage);
            
        //通过CAT模块发送参数修改信息到后台
        //标记CAT必发事件标记
        MachInfo.catMastSynServEvent.bit.paraSet = 1;
        MachInfo.catMastSynServEvent.bit.sendRunLog = 1;
		
		EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CALIBRATION,(uint8_t*)&MachInfo.calibration,sizeof(MachInfo.calibration));
		EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
                    
        CATQueueInfoBuf.msgType = SET_PARA_OVER_CAT_MSG_TYPE;
        osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
    }
    
    return FEED_BACK_SUCCESS;
}

/*********************************************************************
*
*       _cbDialog
*/
static void Calibration_cbDialog(WM_MESSAGE * pMsg) {
//  WM_HWIN hItem;
  int     NCode;
  int     Id;
  int     i = 0;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
  {
    WBCHGB_Calibration_Msg_Init(pMsg);
  }
  break;
  case WM_PAINT:
  {
     WBCHGB_Calibration_Msg_Paint(pMsg);
  }
  break;
  case WM_KEYBOARD_UPDATE: 
  {
      //清除错误提示
     TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_ERROR_PROMPT), "");
      
    i = strlen(pMsg->Data.p);
    if(i == 1) //ASCII码
    {
        char c = *((char*)pMsg->Data.p);
        if((c >= '0' && c <= '9'))
        {
            GUI_StoreKeyMsg(c, 1);
        }
    }else{
        LOG_Error("Msg Len(%d) Error", i);
    }
   }
   break;
   case WM_KEYBOARD_END: 
   {
       
//       //如果是HGB输入，则需要调整界面到原来的位置
//       if(WM_IsEnabled(WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB_PERCENT)))
//       {
//           if(g_sCbr_MoveFlag == 1)
//           {
//                WM_MoveWindow(pMsg->hWin, CBR_X_MOVE_LEN, CBR_Y_MOVE_LEN);
//                g_sCbr_MoveFlag = 0;
//           }
//       }
//       WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_WBC_PERCENT));
//       WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB_PERCENT));       
               
       if(g_sCbr_MoveFlag == 1)
       {
           WM_MoveTo(pMsg->hWin, 0, 65);
           g_sCbr_MoveFlag = 0;
       }
   }
   break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_CANCEL: // Notifications sent by 'cancel'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
         GUI_EndDialog(pMsg->hWin, 0);
         g_hActiveWin = CreateSetPage(); 
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_SURE: // Notifications sent by 'sure'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        {
        // USER START (Optionally insert code for reacting on notification message)
         //保存WBC HGB 校准参数
          if(Save_Calibration_Param(pMsg) == FEED_BACK_SUCCESS) 
          { 
                //参数输入正确
                GUI_EndDialog(pMsg->hWin, 0);
                g_hActiveWin = CreateSetPage(); 
          }else{
            // 提示异常信息
              char buffer[128] = {0};
              TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_ERROR_PROMPT), GUI_RED);
              sprintf(buffer, "%s:[75-125]", g_ucaLng_InputError[MachInfo.systemSet.eLanguage]);
              TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_ERROR_PROMPT), buffer);
              
          }              
        // USER END
        }
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_WBC_PERCENT: // Notifications sent by 'wbc_edit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        WM_SetFocus(pMsg->hWinSrc);
//        WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB_PERCENT));
		MachRunPara.keyboardInitJM = KEYBOARD_NUM;
        CreateKeyboard_Window();
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_HGB_PERCENT: // Notifications sent by 'hgb_edit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        
        //HGB输入，则需需要调整界面的位置
         if(g_sCbr_MoveFlag == 0) //防止多次点击HGB_EDIT，多次移动窗口
         {
            WM_MoveWindow(pMsg->hWin, CBR_X_MOVE_LEN, -CBR_Y_MOVE_LEN);
            g_sCbr_MoveFlag = 1;
         }
        //
        WM_SetFocus(pMsg->hWinSrc);
//        WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_WBC_PERCENT));
		 MachRunPara.keyboardInitJM = KEYBOARD_NUM;
        CreateKeyboard_Window();
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateWindow
*/
WM_HWIN Calibration_CreateWindow(void);
WM_HWIN Calibration_CreateWindow(void) {
  WM_HWIN hWin;
	
	Disable_Algo_Task();

  hWin = GUI_CreateDialogBox(Calibration_aDialogCreate, GUI_COUNTOF(Calibration_aDialogCreate), Calibration_cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
