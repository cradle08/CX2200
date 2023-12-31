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
// USER END
 
#include "DIALOG.h"
#include "Public_menuDLG.h"
#include "bsp_eeprom.h"
#include "CAT_iot.h"
#include "bsp_rtc.h"
#include "time.h"
#include "Common.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0 (CONFIG_LAB_INFO_PAGE_ID + 0x00)
#define ID_TEXT_0 (CONFIG_LAB_INFO_PAGE_ID + 0x01)
#define ID_TEXT_1 (CONFIG_LAB_INFO_PAGE_ID + 0x02)
#define ID_TEXT_2 (CONFIG_LAB_INFO_PAGE_ID + 0x03)
#define ID_TEXT_3 (CONFIG_LAB_INFO_PAGE_ID + 0x04)
#define ID_TEXT_4 (CONFIG_LAB_INFO_PAGE_ID + 0x05)
#define ID_TEXT_5 (CONFIG_LAB_INFO_PAGE_ID + 0x06)
#define ID_EDIT_0 (CONFIG_LAB_INFO_PAGE_ID + 0x07)
#define ID_EDIT_1 (CONFIG_LAB_INFO_PAGE_ID + 0x08)
#define ID_EDIT_2 (CONFIG_LAB_INFO_PAGE_ID + 0x09)
#define ID_EDIT_3 (CONFIG_LAB_INFO_PAGE_ID + 0x0A)
#define ID_BUTTON_0 (CONFIG_LAB_INFO_PAGE_ID + 0x0B)
#define ID_BUTTON_1 (CONFIG_LAB_INFO_PAGE_ID + 0x0C)
#define ID_BUTTON_2 (CONFIG_LAB_INFO_PAGE_ID + 0x0D)

#define ID_TEXT_6 (CONFIG_LAB_INFO_PAGE_ID + 0x0E)
#define ID_TEXT_7 (CONFIG_LAB_INFO_PAGE_ID + 0x0F)
#define ID_TEXT_8 (CONFIG_LAB_INFO_PAGE_ID + 0x10)
#define ID_EDIT_4 (CONFIG_LAB_INFO_PAGE_ID + 0x11)
#define ID_EDIT_5 (CONFIG_LAB_INFO_PAGE_ID + 0x12)
#define ID_EDIT_6 (CONFIG_LAB_INFO_PAGE_ID + 0x13)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "lab_infoPage", ID_WINDOW_0, 0, 63, 800, 395, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "SET", ID_TEXT_0, 30, 15, 95, 30, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "> LAB INFO", ID_TEXT_1, 125, 20, 140, 20, 0, 0x0, 0 },
  
  { TEXT_CreateIndirect, "MachName", ID_TEXT_2, 20, 104, 100, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "MachType", ID_TEXT_3, 20, 157, 100, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "MachSN", ID_BUTTON_2, 20, 208, 100, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Clinic", ID_TEXT_5, 20, 260, 100, 20, 0, 0x0, 0 },
  
  { EDIT_CreateIndirect, "machName_e", ID_EDIT_0, 130, 94, 260, 32, 0, 0x28, 0 },
  { EDIT_CreateIndirect, "machType_e", ID_EDIT_1, 130, 146, 260, 32, 0, 0x28, 0 },
  { EDIT_CreateIndirect, "machSN_e", ID_EDIT_2, 130, 198, 260, 32, 0, 20, 0 },
  { EDIT_CreateIndirect, "zhensuoName_e", ID_EDIT_3, 130, 250, 260, 32, 0, 50, 0 },
  
  { BUTTON_CreateIndirect, "Cel", ID_BUTTON_0, 265, 330, 120, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Confim", ID_BUTTON_1, 415, 330, 120, 40, 0, 0x0, 0 },
  
  { TEXT_CreateIndirect, "MQTT-IP", ID_TEXT_6, 390, 104, 100, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "MQTT-NAME", ID_TEXT_7, 390, 157, 100, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "MQTT-PSW", ID_TEXT_8, 390, 208, 100, 20, 0, 0x0, 0 },
  
  { EDIT_CreateIndirect, "MQTT-IP_e", ID_EDIT_4, 500, 94, 260, 32, 0, 0x28, 0 },
  { EDIT_CreateIndirect, "MQTT-NAME_e", ID_EDIT_5, 500, 146, 260, 32, 0, 0x28, 0 },
  { EDIT_CreateIndirect, "MQTT-PSW_e", ID_EDIT_6, 500, 198, 260, 32, 0, 0x28, 0 },
  
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
static void _LabInfoPageRedrawWidgetType_Callback(WM_MESSAGE* pMsg)
{
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
    {
        GUI_RECT Rect;
        WM_GetClientRect(&Rect);

        if (BUTTON_IsPressed(pMsg->hWin)) {
			switch(WM_GetId(pMsg->hWin)){
				case ID_BUTTON_0:
					//填充
					GUI_SetColor(MachInfo.companyInfo.skin.buttPress);
					GUI_AA_FillRoundedRectEx(&Rect,20);
					
					//设置字符前景和背景色
					GUI_SetColor(MachInfo.companyInfo.skin.buttPressFont);
					GUI_SetBkColor(MachInfo.companyInfo.skin.buttPress);
				
					GUI_DispStringInRect(g_ucaLng_Cancel[MachInfo.systemSet.eLanguage],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);
				break;
				
				case ID_BUTTON_1:
					//填充
					GUI_SetColor(MachInfo.companyInfo.skin.buttPress);
					GUI_AA_FillRoundedRectEx(&Rect,20);
					
					//设置字符前景和背景色
					GUI_SetColor(MachInfo.companyInfo.skin.buttPressFont);
					GUI_SetBkColor(MachInfo.companyInfo.skin.buttPress);
				
					GUI_DispStringInRect(g_ucaLng_Sure[MachInfo.systemSet.eLanguage],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);
				break;
				
				case ID_BUTTON_2:
					GUI_SetColor(GUI_WHITE);
					GUI_FillRectEx(&Rect);
					
					//设置字符前景和背景色
					GUI_SetColor(GUI_BLACK);
					GUI_SetBkColor(GUI_WHITE);
				
					GUI_DispStringInRect(g_ucaLng_SET_MachSN[MachInfo.systemSet.eLanguage],&Rect,GUI_TA_RIGHT|GUI_TA_VCENTER);
				break;
				
				default :break;
			}
        }
        else{
			switch(WM_GetId(pMsg->hWin)){
				case ID_BUTTON_0:
					//填充
					GUI_SetColor(MachInfo.companyInfo.skin.buttNotPress);
					GUI_AA_FillRoundedRectEx(&Rect,20);
					
					GUI_SetColor(MachInfo.companyInfo.skin.buttBoard);
					GUI_AA_DrawRoundedRectEx(&Rect,19);
				
					//设置字符前景和背景色
					GUI_SetColor(MachInfo.companyInfo.skin.buttNotPressFont);
					GUI_SetBkColor(MachInfo.companyInfo.skin.buttNotPress);
				
					GUI_DispStringInRect(g_ucaLng_Cancel[MachInfo.systemSet.eLanguage],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);
				break;
				
				case ID_BUTTON_1:
					//填充
					GUI_SetColor(MachInfo.companyInfo.skin.buttNotPress);
					GUI_AA_FillRoundedRectEx(&Rect,20);
				
					GUI_SetColor(MachInfo.companyInfo.skin.buttBoard);
					GUI_AA_DrawRoundedRectEx(&Rect,19);
					
					//设置字符前景和背景色
					GUI_SetColor(MachInfo.companyInfo.skin.buttNotPressFont);
					GUI_SetBkColor(MachInfo.companyInfo.skin.buttNotPress);
				
					GUI_DispStringInRect(g_ucaLng_Sure[MachInfo.systemSet.eLanguage],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);
				break;
				
				case ID_BUTTON_2:
					GUI_SetColor(GUI_WHITE);
					GUI_FillRectEx(&Rect);
					
					//设置字符前景和背景色
					GUI_SetColor(GUI_BLACK);
					GUI_SetBkColor(GUI_WHITE);
				
					GUI_DispStringInRect(g_ucaLng_SET_MachSN[MachInfo.systemSet.eLanguage],&Rect,GUI_TA_RIGHT|GUI_TA_VCENTER);
				break;
				
				default :break;
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
* 设置控件回调函数，以及重绘控件
*/
static void _LabInfoPageRedrawWidgetType(WM_MESSAGE* pMsg)
{
    WM_HWIN hItem;

    //取消按钮
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);  
    WM_SetCallback(hItem, _LabInfoPageRedrawWidgetType_Callback);
    WM_InvalidateWindow(hItem);
	
	//确定按钮
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);  
    WM_SetCallback(hItem, _LabInfoPageRedrawWidgetType_Callback);
    WM_InvalidateWindow(hItem);
	
	//序列号标签按钮
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);  
    WM_SetCallback(hItem, _LabInfoPageRedrawWidgetType_Callback);
    WM_InvalidateWindow(hItem);
}





// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  GUI_RECT Rect;
    
  // USER START (Optionally insert additional variables)
	extern WM_HWIN CreateSetPage(void);

	extern char* BKBmaDataBuf;
	extern __IO MachRunPara_s	MachRunPara;
	extern MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osThreadId_t CATTaskHandle;
	extern osMessageQueueId_t CATQueueHandle;
	extern OutSideDataNum_s OutSideDataNum;
	
    static uint8_t MoveWindowFlag = 0;						//窗口移动标志
	
	char StrTemp[60];
	RTC_HYM8563Info_s RTCTime = {0};
	
	CATQueueInfoBuf_s CATQueueInfoBuf;
	CommonDialogPageData_s DialogPageData = {0};
	Msg_Head_t BackendMsg = {0};
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
	if(MachInfo.systemSet.eLanguage == LANGUAGE_CHINESE){
		TEXT_SetFont(hItem,&HZ_SONGTI_24);
		WM_SetWindowPos(hItem,20, 13+63, 48, 30);
	}else{
		TEXT_SetFont(hItem,&GUI_Font24_ASCII);
		WM_SetWindowPos(hItem,20, 13+63, 84, 30);
	}
	TEXT_SetTextAlign(hItem,TEXT_CF_HCENTER|TEXT_CF_VCENTER);
	TEXT_SetText(hItem,g_ucaLng_Set[MachInfo.systemSet.eLanguage]);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
	if(MachInfo.systemSet.eLanguage == LANGUAGE_CHINESE){
		WM_SetWindowPos(hItem,68, 20+63, 140, 20);
	}else{
		WM_SetWindowPos(hItem,104, 20+63, 140, 20);
	}
	TEXT_SetTextAlign(hItem,TEXT_CF_LEFT|TEXT_CF_VCENTER);
	TEXT_SetText(hItem,g_ucaLng_SET_LabInfo[MachInfo.systemSet.eLanguage]);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
	TEXT_SetTextAlign(hItem,GUI_TA_RIGHT|GUI_TA_VCENTER);
	TEXT_SetText(hItem,g_ucaLng_SET_MachName[MachInfo.systemSet.eLanguage]);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
	TEXT_SetTextAlign(hItem,GUI_TA_RIGHT|GUI_TA_VCENTER);
	TEXT_SetText(hItem,g_ucaLng_SET_MachType[MachInfo.systemSet.eLanguage]);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
	TEXT_SetTextAlign(hItem,GUI_TA_RIGHT|GUI_TA_VCENTER);
	TEXT_SetText(hItem,g_ucaLng_SET_ZhensuoName[MachInfo.systemSet.eLanguage]);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
	TEXT_SetTextAlign(hItem,GUI_TA_RIGHT|GUI_TA_VCENTER);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
	TEXT_SetTextAlign(hItem,GUI_TA_RIGHT|GUI_TA_VCENTER);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
	TEXT_SetTextAlign(hItem,GUI_TA_RIGHT|GUI_TA_VCENTER);
  
    //
    // Initialization of 'machName_e'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
	EDIT_SetTextAlign(hItem,GUI_TA_LEFT|GUI_TA_VCENTER);
	//不同公司显示不同仪器名称
	switch(MachInfo.companyInfo.company){
		case COMPANY_JIN_RUI:
		{
			EDIT_SetText(hItem, g_ucaLng_SET_MachNameInfoYL1[MachInfo.systemSet.eLanguage]);
		}break;
		
		default :
		{
			EDIT_SetText(hItem, g_ucaLng_SET_MachNameInfo[MachInfo.systemSet.eLanguage]);
		}break;
	}
	WM_DisableWindow(hItem);								
    //
    // Initialization of 'machType_e'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_1);
	EDIT_SetTextAlign(hItem,GUI_TA_LEFT|GUI_TA_VCENTER);
	//不同公司显示不同仪器类型
	switch(MachInfo.companyInfo.company){
		case COMPANY_JIN_RUI:
		{
			if(OutSideDataNum.index == 0){
				EDIT_SetText(hItem, "KT-10");
			}else if(OutSideDataNum.index == 1){
				EDIT_SetText(hItem, "KT-11");
			}else{
				EDIT_SetText(hItem, "KT-10");
			}
		}break;
		
		default :
		{
			EDIT_SetText(hItem, "CX-2200");
		}break;
	}
    
	WM_DisableWindow(hItem);								
    //
    // Initialization of 'machSN_e'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_2);
	EDIT_SetTextAlign(hItem,GUI_TA_LEFT|GUI_TA_VCENTER);
	EDIT_EnableBlink(hItem,500,1);
	
	//判断保存的SN是否符合要求
	if(strlen((char*)MachInfo.labInfo.ucaMachineSN) == MACHINE_SN_LEN){
		//判断SN是否符合规定要求
		if(MachInfo.labInfo.ucaMachineSN[0]!='B' || MachInfo.labInfo.ucaMachineSN[1]!='B' || MachInfo.labInfo.ucaMachineSN[2]!='4'){
			//获取RTC时间
			RTC_HYM8563GetTime(&RTCTime);
			memset((uint8_t*)MachInfo.labInfo.ucaMachineSN,0,sizeof(MachInfo.labInfo.ucaMachineSN));
			sprintf((char*)MachInfo.labInfo.ucaMachineSN,"BB4%u%u%u%u",RTCTime.year.bcd_h,RTCTime.year.bcd_l,RTCTime.month.bcd_h,RTCTime.month.bcd_l);
			
			MachInfo.flag.bit.connNet = 0;
		}
	}else{
		//获取RTC时间
		RTC_HYM8563GetTime(&RTCTime);
		memset((uint8_t*)MachInfo.labInfo.ucaMachineSN,0,sizeof(MachInfo.labInfo.ucaMachineSN));
		sprintf((char*)MachInfo.labInfo.ucaMachineSN,"BB4%u%u%u%u",RTCTime.year.bcd_h,RTCTime.year.bcd_l,RTCTime.month.bcd_h,RTCTime.month.bcd_l);
		
		MachInfo.flag.bit.connNet = 0;
	}
	EDIT_SetText(hItem, (char*)MachInfo.labInfo.ucaMachineSN);
	
	//判断是否已经在服务端注册过了
	if(MachInfo.flag.bit.connNet != 1){
		//根据不同的用户权限，开放编辑框
		if(MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type != ACCOUNT_TYPE_SUPER){
			WM_DisableWindow(hItem);							//失能编辑框
		}else{
			if(CATGlobalStatus.registerNet == CAT_REGISTNET_SUCCESS){
				WM_EnableWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_2));
			}else{
				WM_DisableWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_2));
			}
		}
	}else{
		WM_DisableWindow(hItem);							//失能编辑框
	}
    //
    // Initialization of 'zhensuoName_e'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_3);
	EDIT_SetTextAlign(hItem,GUI_TA_LEFT|GUI_TA_VCENTER);
	EDIT_EnableBlink(hItem,500,1);
	switch(MachInfo.companyInfo.company){
		case COMPANY_YSB:
		{
			//药师帮不能编辑诊所名，且固定为“广州阅微”
			if(strcmp((char*)MachInfo.labInfo.ucaClinicName,"广州阅微") != 0){
				memset(MachInfo.labInfo.ucaClinicName,0,CLINIC_NAME_MAX_LEN);
				strcpy((char*)MachInfo.labInfo.ucaClinicName,"广州阅微");
				EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_LAB_INFO,(uint8_t*)&MachInfo.labInfo,sizeof(MachInfo.labInfo));
			}
			WM_DisableWindow(hItem);
		}break;
		
		default :
		{
			WM_EnableWindow(hItem);
		}break;
	}
	EDIT_SetText(hItem, (char*)MachInfo.labInfo.ucaClinicName);
	
	//MQTT-IP EDIT
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_4);
	EDIT_SetTextAlign(hItem,GUI_TA_LEFT|GUI_TA_VCENTER);
	EDIT_EnableBlink(hItem,500,1);
	EDIT_SetMaxLen(hItem,MQTT_IP_MAX_LEN);
	EDIT_SetText(hItem,MachInfo.companyInfo.mqtt.ip);
	
	//MQTT-NAME EDIT
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_5);
	EDIT_SetTextAlign(hItem,GUI_TA_LEFT|GUI_TA_VCENTER);
	EDIT_EnableBlink(hItem,500,1);
	EDIT_SetMaxLen(hItem,MQTT_NAME_MAX_LEN);
	EDIT_SetText(hItem,MachInfo.companyInfo.mqtt.name);
	
	//MQTT-PSW EDIT
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_6);
	EDIT_SetTextAlign(hItem,GUI_TA_LEFT|GUI_TA_VCENTER);
	EDIT_EnableBlink(hItem,500,1);
	EDIT_SetMaxLen(hItem,MQTT_PSW_MAX_LEN);
	EDIT_SetText(hItem,MachInfo.companyInfo.mqtt.psw);
	
	//根据所选公司决定是否需要显示MQTT相关参数
	switch(MachInfo.companyInfo.company){
		case COMPANY_YSB:
		{
			//药师帮
			if(MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].authority < ACCOUNT_ADMIN_AUTHORITY){
				//普通用户不可见IP/域名字段，用户名字段
				WM_HideWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_4));
				WM_HideWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_5));
				WM_HideWindow(WM_GetDialogItem(pMsg->hWin,ID_TEXT_6));
				WM_HideWindow(WM_GetDialogItem(pMsg->hWin,ID_TEXT_7));
				
				WM_MoveWindow(WM_GetDialogItem(pMsg->hWin,ID_TEXT_8),0,-104);
				WM_MoveWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_6),0,-104);
			}
		}break;
		
		default :
		{
			//隐藏控件
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin,ID_TEXT_6));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin,ID_TEXT_7));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin,ID_TEXT_8));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_4));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_5));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_6));
			
			//移动控件
			WM_MoveWindow(WM_GetDialogItem(pMsg->hWin,ID_TEXT_2),180,0);
			WM_MoveWindow(WM_GetDialogItem(pMsg->hWin,ID_TEXT_3),180,0);
			WM_MoveWindow(WM_GetDialogItem(pMsg->hWin,ID_BUTTON_2),180,0);
			WM_MoveWindow(WM_GetDialogItem(pMsg->hWin,ID_TEXT_5),180,0);
			
			WM_MoveWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_0),180,0);
			WM_MoveWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_1),180,0);
			WM_MoveWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_2),180,0);
			WM_MoveWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_3),180,0);
		}break;
	}
	
	//重绘按钮样式
	_LabInfoPageRedrawWidgetType(pMsg);
    // USER START (Optionally insert additional code for further widget initialization)
    //设置窗口颜色和背景颜色一致
    WINDOW_SetBkColor(pMsg->hWin, MachInfo.companyInfo.skin.bg);
    // USER END
    break;
	
	//更新键盘事件
	case WM_KEYBOARD_UPDATE: 
	{
		char c;
		char ZhensuoName[50];
		uint8_t i = strlen(pMsg->Data.p);
		hItem = WM_GetFocusedWindow();
		
		if(i == 1){								//输入ASCII码
			c = *((char*)pMsg->Data.p);
			if(hItem == WM_GetDialogItem(pMsg->hWin, ID_EDIT_2)){
				if((c>='0'&&c<='9') || (c>='a'&&c<='z') || (c>='A'&&c<='Z')){
					GUI_StoreKeyMsg(c, 1);
				}
			}else{
				GUI_StoreKeyMsg(c, 1);
			}
		}else if(i == 3){						//输入汉字
			if(hItem == WM_GetDialogItem(pMsg->hWin, ID_EDIT_3)){
				memset(ZhensuoName,0,sizeof(ZhensuoName));
				EDIT_GetText(hItem, ZhensuoName, 50);
				strcat(ZhensuoName, pMsg->Data.p);
				EDIT_SetText(hItem, ZhensuoName);
			}
		}
	}
	break;
	
	//键盘结束事件
	case WM_KEYBOARD_END:
		//恢复窗口到原来位置处
		if(MoveWindowFlag == 1){
			MoveWindowFlag = 0;
			WM_MoveWindow(pMsg->hWin, 0, 90);
		}
	
		//恢复编辑框使能
		if(CATGlobalStatus.registerNet == CAT_REGISTNET_SUCCESS && MachInfo.flag.bit.connNet != 1 && MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type == ACCOUNT_TYPE_SUPER){
			WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_2));
		}
		
		WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_3));
	break;
	
  case WM_PAINT:
	//画背景BMP图
    WM_GetClientRect(&Rect); 
    GUI_SetColor(GUI_WHITE);
    GUI_AA_FillRoundedRect(Rect.x0+10, Rect.y0, Rect.x1-10, Rect.y1+10, 10);

	//画线
	GUI_SetColor(0x009ea0a2);
	GUI_DrawHLine(54,8,790);
	
  break;
  
  //窗口定时器
  case WM_TIMER:
	//判断序列号输入框是否可以被激活了
	if(CATGlobalStatus.registerNet == CAT_REGISTNET_SUCCESS && MachInfo.flag.bit.connNet != 1 && MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type == ACCOUNT_TYPE_SUPER){
		WM_EnableWindow(WM_GetDialogItem(pMsg->hWin,ID_EDIT_2));
	}else{
		//重启计时器
		WM_RestartTimer(pMsg->Data.v,1000);
	}
  break;
  
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_EDIT_0: // Notifications sent by 'machName_e'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
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
    case ID_EDIT_1: // Notifications sent by 'machType_e'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
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
    case ID_EDIT_2: // Notifications sent by 'machSN_e'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//移动窗体，避免被键盘窗口覆盖
		if(MoveWindowFlag == 0){
			MoveWindowFlag = 1;

            WM_MoveWindow(pMsg->hWin,0,-90);
            
            //唤出键盘
            WM_SetFocus(pMsg->hWinSrc);
            WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_3));
			MachRunPara.keyboardInitJM = KEYBOARD_NUM;
            CreateKeyboard_Window();
            
		}
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
    case ID_EDIT_3: // Notifications sent by 'zhensuoName_e'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//移动窗体，避免被键盘窗口覆盖
		if(MoveWindowFlag == 0){
			MoveWindowFlag = 1;
			WM_MoveWindow(pMsg->hWin,0,-90);
			
			//唤出键盘
			WM_SetFocus(pMsg->hWinSrc);
			WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_2));
			MachRunPara.keyboardInitJM = KEYBOARD_HZ;
			CreateKeyboard_Window();
		}
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
    case ID_BUTTON_0: // Notifications sent by 'cel_b'
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
    case ID_BUTTON_1: // Notifications sent by 'confim_b'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//保存参数
		//诊所名编辑框
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_3);
		memset(StrTemp,0,sizeof(StrTemp));
		EDIT_GetText(hItem,StrTemp,50);
		if(strcmp(StrTemp,(char*)MachInfo.labInfo.ucaClinicName) != 0){
			strcpy((char*)MachInfo.labInfo.ucaClinicName,StrTemp);
			
			//标记CAT必发事件标记
			if(MachInfo.catMastSynServEvent.bit.paraSet != 1){
				MachInfo.catMastSynServEvent.bit.paraSet = 1;
				EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
			}
			
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_LAB_INFO,(uint8_t*)&MachInfo.labInfo,sizeof(MachInfo.labInfo));
			
			//通过CAT模块发送参数修改信息到后台
			CATQueueInfoBuf.msgType = SET_PARA_OVER_CAT_MSG_TYPE;
			osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
		}
	  
		//SN编辑框
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_2);
		memset(StrTemp,0,sizeof(StrTemp));
		EDIT_GetText(hItem,StrTemp,20);
		if(strcmp(StrTemp,(char*)MachInfo.labInfo.ucaMachineSN) != 0){
			//判断更改后的SN是否符合要求
			if(strlen(StrTemp) == MACHINE_SN_LEN && StrTemp[0] == 'B' && StrTemp[1] == 'B' && StrTemp[2] == '4'){
				extern osEventFlagsId_t GlobalEventCatGroupHandle;
				
				strcpy((char*)MachInfo.labInfo.ucaMachineSN,StrTemp);
				
				//获取RTC时间
				RTC_HYM8563GetTime(&RTCTime);
				
				//标记出厂时间
				MachInfo.other.factoryTimestamp = GetCurrentTimestamp(RTCTime);
				
				//弹出对话框，等待服务端响应
				GUI_EndDialog(pMsg->hWin, 0);
				g_hActiveWin = CreateDialogPage();
			  
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = NO_CONFIM_BUTTON;
				DialogPageData.fun = Createlab_infoPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_WaitForConfim[MachInfo.systemSet.eLanguage]);
				
				pMsg->MsgId = WM_USER_DATA;
				pMsg->Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,pMsg);
				
				//先清除网络异常事件标志
				osEventFlagsClear(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_NET_ERR);
				
				//往后台发送消息
				BackendMsg.usCmd = CMD_SET_SN_WAIT_CAT;
				UI_Put_Msg((uint8_t*)&BackendMsg);
				
				//唤醒CAT任务
				if(osThreadGetState(CATTaskHandle) == osThreadBlocked){
					osThreadResume(CATTaskHandle);
				}
			}else{
				//弹出对话框，SN输入错误
				GUI_EndDialog(pMsg->hWin, 0);
				g_hActiveWin = CreateDialogPage();
			  
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = Createlab_infoPage;
				strcpy(DialogPageData.str,g_ucaLng_SET_SNERR[MachInfo.systemSet.eLanguage]);
				
				pMsg->MsgId = WM_USER_DATA;
				pMsg->Data.p = &DialogPageData;
				WM_SendMessage(g_hActiveWin,pMsg);
			}
		}else{
			GUI_EndDialog(pMsg->hWin, 0);
			g_hActiveWin = CreateSetPage();
		}
		
		//MQTT-IP
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_4);
		memset(StrTemp,0,sizeof(StrTemp));
		EDIT_GetText(hItem,StrTemp,MQTT_IP_MAX_LEN);
		if(strcmp(StrTemp,MachInfo.companyInfo.mqtt.ip) != 0){
			strcpy(MachInfo.companyInfo.mqtt.ip,StrTemp);
			
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_COMPANY+((uint8_t*)&MachInfo.companyInfo.mqtt-(uint8_t*)&MachInfo.companyInfo),(uint8_t*)&MachInfo.companyInfo.mqtt,sizeof(MachInfo.companyInfo.mqtt));
		}
		
		//MQTT-NAME
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_5);
		memset(StrTemp,0,sizeof(StrTemp));
		EDIT_GetText(hItem,StrTemp,MQTT_NAME_MAX_LEN);
		if(strcmp(StrTemp,MachInfo.companyInfo.mqtt.name) != 0){
			strcpy(MachInfo.companyInfo.mqtt.name,StrTemp);
			
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_COMPANY+((uint8_t*)&MachInfo.companyInfo.mqtt-(uint8_t*)&MachInfo.companyInfo),(uint8_t*)&MachInfo.companyInfo.mqtt,sizeof(MachInfo.companyInfo.mqtt));
		}
		
		//MQTT-PSW
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_6);
		memset(StrTemp,0,sizeof(StrTemp));
		EDIT_GetText(hItem,StrTemp,MQTT_PSW_MAX_LEN);
		if(strcmp(StrTemp,MachInfo.companyInfo.mqtt.psw) != 0){
			strcpy(MachInfo.companyInfo.mqtt.psw,StrTemp);
			
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_COMPANY+((uint8_t*)&MachInfo.companyInfo.mqtt-(uint8_t*)&MachInfo.companyInfo),(uint8_t*)&MachInfo.companyInfo.mqtt,sizeof(MachInfo.companyInfo.mqtt));
		}
		
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
	  
	  case ID_BUTTON_2: // Notifications sent by '仪器序列号标签按钮'
      switch(NCode) {
		 static uint32_t PressTick = 0;
		  
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
		PressTick = HAL_GetTick();
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//按下超过5s则临时接触输入序列号编辑框的限制，以便可以更改序列号
		if(HAL_GetTick() - PressTick > 5000 && CATGlobalStatus.flag.connServerErr != 1 && CATGlobalStatus.registerNet == CAT_REGISTNET_SUCCESS){
			extern WM_HWIN CreateSwitchDialogPage(void);
			extern WM_HWIN Createlab_infoPage(void);
			CommonSwitchDialogPageData_s SwitchDialogData = {0};
			
			//弹出选择对话框
			GUI_EndDialog(pMsg->hWin, 0);
			g_hActiveWin = CreateSwitchDialogPage();
			
			//重置仪器参数
			SwitchDialogData.switchDialogItem = CONFIG_LAB_INFO_CHANGE_SN;
			SwitchDialogData.dialogData.bmpType = BMP_ALARM;
			SwitchDialogData.dialogData.confimButt = HAS_CONFIM_BUTTON;
			SwitchDialogData.dialogData.fun = Createlab_infoPage;					//保存进入对话框前的界面创建函数
			strcpy(SwitchDialogData.dialogData.str,g_ucaLng_SET_ConfimChangeSN[MachInfo.systemSet.eLanguage]);
			
			pMsg->MsgId = WM_USER_DATA;
			pMsg->Data.p = &SwitchDialogData;
			WM_SendMessage(g_hActiveWin,pMsg);
		}
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
	  
	case ID_EDIT_4: // Notifications sent by 'MQTT-IP'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//唤出键盘
		WM_SetFocus(pMsg->hWinSrc);
		MachRunPara.keyboardInitJM = KEYBOARD_CHAR;
		CreateKeyboard_Window();
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
	  
	case ID_EDIT_5: // Notifications sent by 'MQTT-NAME'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//唤出键盘
		WM_SetFocus(pMsg->hWinSrc);
		MachRunPara.keyboardInitJM = KEYBOARD_CHAR;
		CreateKeyboard_Window();
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
	  
	case ID_EDIT_6: // Notifications sent by 'MQTT-PSW'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//移动窗体，避免被键盘窗口覆盖
		if(MoveWindowFlag == 0){
			MoveWindowFlag = 1;

            WM_MoveWindow(pMsg->hWin,0,-70);
            
            //唤出键盘
            WM_SetFocus(pMsg->hWinSrc);
			MachRunPara.keyboardInitJM = KEYBOARD_CHAR;
            CreateKeyboard_Window();
		}
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
*       Createlab_infoPage
*/
WM_HWIN Createlab_infoPage(void);
WM_HWIN Createlab_infoPage(void) {
	WM_HWIN hWin;

	WM_SetCallback(WM_HBKWIN, Public_cbBkWin);
	Public_Show_Widget();
	
	Disable_Algo_Task();
	
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);

	//创建窗口定时器,1000ms触发一次
	WM_CreateTimer(hWin,0,1000,0);
	
	return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
