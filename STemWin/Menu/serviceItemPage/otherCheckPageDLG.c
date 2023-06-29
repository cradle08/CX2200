/*********************************************************************
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
#include "monitorV.h"
#include "bsp_outin.h"
#include "bsp_pump.h"
#include "bsp_beep.h"
#include "bsp_light.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0 (OTHER_CHECK_PAGE_ID + 0x00)
#define ID_TEXT_0 (OTHER_CHECK_PAGE_ID + 0x01)
#define ID_TEXT_1 (OTHER_CHECK_PAGE_ID + 0x02)
#define ID_BUTTON_0 (OTHER_CHECK_PAGE_ID + 0x03)
#define ID_BUTTON_1 (OTHER_CHECK_PAGE_ID + 0x04)
#define ID_BUTTON_2 (OTHER_CHECK_PAGE_ID + 0x05)
#define ID_BUTTON_3 (OTHER_CHECK_PAGE_ID + 0x06)
#define ID_BUTTON_4 (OTHER_CHECK_PAGE_ID + 0x07)
#define ID_BUTTON_5 (OTHER_CHECK_PAGE_ID + 0x08)
#define ID_BUTTON_6 (OTHER_CHECK_PAGE_ID + 0x09)


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
  { WINDOW_CreateIndirect, "otherCheckPage", ID_WINDOW_0, 0, 63, 800, 395, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "SERVE", ID_TEXT_0, 20, 13, 85, 30, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "> OTHER CHECK", ID_TEXT_1, 105, 20, 140, 20, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "CEL", ID_BUTTON_0, 678, 11, 100, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "xk_b", ID_BUTTON_1, 174, 113, 200, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "valveAir_b", ID_BUTTON_2, 426, 113, 200, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "pump_b", ID_BUTTON_3, 174, 173, 200, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "valveYe_b", ID_BUTTON_4, 426, 173, 200, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "beep_b", ID_BUTTON_5, 174, 233, 200, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "yuliu1_b", ID_BUTTON_6, 426, 233, 200, 40, 0, 0x0, 0 },
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
static int _Button_DrawSkin(const WIDGET_ITEM_DRAW_INFO* pDrawItemInfo)
{
	GUI_RECT Rect;
    WM_GetClientRect(&Rect);
	
	switch(pDrawItemInfo->Cmd){
		case WIDGET_ITEM_DRAW_BACKGROUND:
			if(BUTTON_IsPressed(pDrawItemInfo->hWin)){		//按钮按下时皮肤色
				GUI_SetColor(MachInfo.companyInfo.skin.buttPress);
				GUI_AA_FillRoundedRectEx(&Rect,20);
			}else{											//按钮没有按下时皮肤色
				GUI_SetColor(MachInfo.companyInfo.skin.buttNotPress);
				GUI_AA_FillRoundedRectEx(&Rect,20);
				
				GUI_SetColor(MachInfo.companyInfo.skin.buttBoard);
				GUI_AA_DrawRoundedRectEx(&Rect,19);
			}
		break;
		default:
			return BUTTON_SKIN_FLEX(pDrawItemInfo);
	}
	return 0;
}
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
	WM_HWIN hItem;
  int NCode;
  int Id;
  GUI_RECT Rect;
    
  // USER START (Optionally insert additional variables)
	extern WM_HWIN CreateDialogPage(void);
	extern WM_HWIN CreateotherCheckPage(void);
	
	extern char* BKBmaDataBuf;
	extern MachInfo_s MachInfo;
	
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
		TEXT_SetText(hItem,g_ucaLng_Serve_Serve[MachInfo.systemSet.eLanguage]);
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
		if(MachInfo.systemSet.eLanguage == LANGUAGE_CHINESE){
			WM_SetWindowPos(hItem,68, 20+63, 140, 20);
		}else{
			WM_SetWindowPos(hItem,104, 20+63, 140, 20);
		}
		TEXT_SetTextAlign(hItem,TEXT_CF_LEFT|TEXT_CF_VCENTER);
		TEXT_SetText(hItem,g_ucaLng_Serve_Other[MachInfo.systemSet.eLanguage]);
	
		//设置按钮皮肤
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
		BUTTON_SetFont(hItem,&HZ_SONGTI_16);
		BUTTON_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
		BUTTON_SetTextColor(hItem,BUTTON_CI_DISABLED,GUI_GRAY);
		BUTTON_SetTextColor(hItem,BUTTON_CI_PRESSED,MachInfo.companyInfo.skin.buttPressFont);
		BUTTON_SetTextColor(hItem,BUTTON_CI_UNPRESSED,MachInfo.companyInfo.skin.buttNotPressFont);
		BUTTON_SetText(hItem,g_ucaLng_Return[MachInfo.systemSet.eLanguage]);
		BUTTON_SetSkin(hItem,_Button_DrawSkin);									//设置皮肤回调函数
	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
		BUTTON_SetFont(hItem,&HZ_SONGTI_16);
		BUTTON_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
		BUTTON_SetTextColor(hItem,BUTTON_CI_DISABLED,GUI_GRAY);
		BUTTON_SetTextColor(hItem,BUTTON_CI_PRESSED,MachInfo.companyInfo.skin.buttPressFont);
		BUTTON_SetTextColor(hItem,BUTTON_CI_UNPRESSED,MachInfo.companyInfo.skin.buttNotPressFont);
		BUTTON_SetText(hItem,g_ucaLng_Serve_XK[MachInfo.systemSet.eLanguage]);
		BUTTON_SetSkin(hItem,_Button_DrawSkin);									//设置皮肤回调函数
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
		BUTTON_SetFont(hItem,&HZ_SONGTI_16);
		BUTTON_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
		BUTTON_SetTextColor(hItem,BUTTON_CI_DISABLED,GUI_GRAY);
		BUTTON_SetTextColor(hItem,BUTTON_CI_PRESSED,MachInfo.companyInfo.skin.buttPressFont);
		BUTTON_SetTextColor(hItem,BUTTON_CI_UNPRESSED,MachInfo.companyInfo.skin.buttNotPressFont);
		BUTTON_SetText(hItem,g_ucaLng_Serve_AirValve[MachInfo.systemSet.eLanguage]);
		BUTTON_SetSkin(hItem,_Button_DrawSkin);									//设置皮肤回调函数
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
		BUTTON_SetFont(hItem,&HZ_SONGTI_16);
		BUTTON_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
		BUTTON_SetTextColor(hItem,BUTTON_CI_DISABLED,GUI_GRAY);
		BUTTON_SetTextColor(hItem,BUTTON_CI_PRESSED,MachInfo.companyInfo.skin.buttPressFont);
		BUTTON_SetTextColor(hItem,BUTTON_CI_UNPRESSED,MachInfo.companyInfo.skin.buttNotPressFont);
		BUTTON_SetText(hItem,g_ucaLng_Serve_Pump[MachInfo.systemSet.eLanguage]);
		BUTTON_SetSkin(hItem,_Button_DrawSkin);									//设置皮肤回调函数
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
		BUTTON_SetFont(hItem,&HZ_SONGTI_16);
		BUTTON_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
		BUTTON_SetTextColor(hItem,BUTTON_CI_DISABLED,GUI_GRAY);
		BUTTON_SetTextColor(hItem,BUTTON_CI_PRESSED,MachInfo.companyInfo.skin.buttPressFont);
		BUTTON_SetTextColor(hItem,BUTTON_CI_UNPRESSED,MachInfo.companyInfo.skin.buttNotPressFont);
		BUTTON_SetText(hItem,g_ucaLng_Serve_LiquidValve[MachInfo.systemSet.eLanguage]);
		BUTTON_SetSkin(hItem,_Button_DrawSkin);									//设置皮肤回调函数
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
		BUTTON_SetFont(hItem,&HZ_SONGTI_16);
		BUTTON_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
		BUTTON_SetTextColor(hItem,BUTTON_CI_DISABLED,GUI_GRAY);
		BUTTON_SetTextColor(hItem,BUTTON_CI_PRESSED,MachInfo.companyInfo.skin.buttPressFont);
		BUTTON_SetTextColor(hItem,BUTTON_CI_UNPRESSED,MachInfo.companyInfo.skin.buttNotPressFont);
		BUTTON_SetText(hItem,g_ucaLng_Serve_Beep[MachInfo.systemSet.eLanguage]);
		BUTTON_SetSkin(hItem,_Button_DrawSkin);									//设置皮肤回调函数
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
		BUTTON_SetFont(hItem,&HZ_SONGTI_16);
		BUTTON_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
		BUTTON_SetTextColor(hItem,BUTTON_CI_DISABLED,GUI_GRAY);
		BUTTON_SetTextColor(hItem,BUTTON_CI_PRESSED,MachInfo.companyInfo.skin.buttPressFont);
		BUTTON_SetTextColor(hItem,BUTTON_CI_UNPRESSED,MachInfo.companyInfo.skin.buttNotPressFont);
		BUTTON_SetText(hItem,g_ucaLng_Serve_HGB[MachInfo.systemSet.eLanguage]);
//        BUTTON_SetText(hItem,g_ucaLng_Serve_MotoCheck[MachInfo.systemSet.eLanguage]);
		BUTTON_SetSkin(hItem,_Button_DrawSkin);									//设置皮肤回调函数
        
        //设置窗口颜色和背景颜色一致
        WINDOW_SetBkColor(pMsg->hWin, MachInfo.companyInfo.skin.bg);
	break;
	  
	case WM_PAINT:
		//画背景BMP图
		//GUI_BMP_Draw(BKBmaDataBuf,1,0);
        //Show_SDRAM_BMP(SDRAM_BMP_Frame, 1, 0);     
        WM_GetClientRect(&Rect); 
        GUI_SetColor(GUI_WHITE);
        GUI_AA_FillRoundedRect(Rect.x0+10, Rect.y0, Rect.x1-10, Rect.y1+10, 10);

		//画线
		GUI_SetColor(0x009ea0a2);
		GUI_DrawHLine(54,8,790);
	break;
	  
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'return_b'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		GUI_EndDialog(pMsg->hWin, 0);
        g_hActiveWin = CreateservicePage();
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_1: // Notifications sent by 'xk_b'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//开启恒流源
		BC_CUR_SW_ON;
		HAL_Delay(500);
	  
		if(XK_V() > 2300){
			//小孔电压自检成功
			GUI_EndDialog(pMsg->hWin, 0);
			g_hActiveWin = CreateDialogPage();
		  
			//保存对话框内容
			DialogPageData.bmpType = BMP_OK;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = CreateotherCheckPage;
			strcpy(DialogPageData.str,g_ucaLng_Serve_XKSuccess[MachInfo.systemSet.eLanguage]);
			
			pMsg->MsgId = WM_USER_DATA;
			pMsg->Data.p = &DialogPageData;
			WM_SendMessage(g_hActiveWin,pMsg);
		}else{
			//小孔电压自检失败
			GUI_EndDialog(pMsg->hWin, 0);
			g_hActiveWin = CreateDialogPage();
		  
			//保存对话框内容
			DialogPageData.bmpType = BMP_ALARM;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = CreateotherCheckPage;
			strcpy(DialogPageData.str,g_ucaLng_Serve_XKFail[MachInfo.systemSet.eLanguage]);
			
			pMsg->MsgId = WM_USER_DATA;
			pMsg->Data.p = &DialogPageData;
			WM_SendMessage(g_hActiveWin,pMsg);
		}
		
		BC_CUR_SW_OFF;
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_2: // Notifications sent by 'valveAir_b'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//往后台发送消息
		BackendMsg.usCmd = CMD_SERVER_OTHER_CHECK_AIR_VALVE;
        UI_Put_Msg((uint8_t*)&BackendMsg);
	  
		GUI_EndDialog(pMsg->hWin, 0);
		g_hActiveWin = CreateDialogPage();
	  
		//保存对话框内容
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = NO_CONFIM_BUTTON;
		DialogPageData.fun = CreateotherCheckPage;
		strcpy(DialogPageData.str,g_ucaLng_Serve_AirValveing[MachInfo.systemSet.eLanguage]);
		
		pMsg->MsgId = WM_USER_DATA;
		pMsg->Data.p = &DialogPageData;
		WM_SendMessage(g_hActiveWin,pMsg);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_3: // Notifications sent by 'pump_b'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//往后台发送消息
		BackendMsg.usCmd = CMD_SERVER_OTHER_CHECK_PUMP;
        UI_Put_Msg((uint8_t*)&BackendMsg);
	  
		GUI_EndDialog(pMsg->hWin, 0);
		g_hActiveWin = CreateDialogPage();
	  
		//保存对话框内容
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = NO_CONFIM_BUTTON;
		DialogPageData.fun = CreateotherCheckPage;
		strcpy(DialogPageData.str,g_ucaLng_Serve_PumpChecking[MachInfo.systemSet.eLanguage]);
		
		pMsg->MsgId = WM_USER_DATA;
		pMsg->Data.p = &DialogPageData;
		WM_SendMessage(g_hActiveWin,pMsg);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_4: // Notifications sent by 'valveYe_b'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//往后台发送消息
		BackendMsg.usCmd = CMD_SERVER_OTHER_CHECK_YE_VALVE;
        UI_Put_Msg((uint8_t*)&BackendMsg);
	  
		GUI_EndDialog(pMsg->hWin, 0);
		g_hActiveWin = CreateDialogPage();
	  
		//保存对话框内容
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = NO_CONFIM_BUTTON;
		DialogPageData.fun = CreateotherCheckPage;
		strcpy(DialogPageData.str,g_ucaLng_Serve_LiquidValveing[MachInfo.systemSet.eLanguage]);
		
		pMsg->MsgId = WM_USER_DATA;
		pMsg->Data.p = &DialogPageData;
		WM_SendMessage(g_hActiveWin,pMsg);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_5: // Notifications sent by 'beep_b'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		//往后台发送消息
		BackendMsg.usCmd = CMD_SERVER_OTHER_CHECK_BEEP;
        UI_Put_Msg((uint8_t*)&BackendMsg);
	  
		GUI_EndDialog(pMsg->hWin, 0);
		g_hActiveWin = CreateDialogPage();
	  
		//保存对话框内容
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = NO_CONFIM_BUTTON;
		DialogPageData.fun = CreateotherCheckPage;
		strcpy(DialogPageData.str,g_ucaLng_Serve_BeepChecking[MachInfo.systemSet.eLanguage]);
		
		pMsg->MsgId = WM_USER_DATA;
		pMsg->Data.p = &DialogPageData;
		WM_SendMessage(g_hActiveWin,pMsg);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
      case ID_BUTTON_6: // Notifications sent by    //'HGB自检'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
      
		//往后台发送消息
		BackendMsg.usCmd = CMD_SERVER_OTHER_CHECK_HGB;
        UI_Put_Msg((uint8_t*)&BackendMsg);
	  
		GUI_EndDialog(pMsg->hWin, 0);
		g_hActiveWin = CreateDialogPage();
	  
		//保存对话框内容
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = NO_CONFIM_BUTTON;
		DialogPageData.fun = CreateotherCheckPage;
		strcpy(DialogPageData.str,g_ucaLng_Serve_HGBChecking[MachInfo.systemSet.eLanguage]);
		
		pMsg->MsgId = WM_USER_DATA;
		pMsg->Data.p = &DialogPageData;
		WM_SendMessage(g_hActiveWin,pMsg);
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
*       CreateotherCheckPage
*/
WM_HWIN CreateotherCheckPage(void);
WM_HWIN CreateotherCheckPage(void) {
    WM_HWIN hWin;
	
    WM_SetCallback(WM_HBKWIN, Public_cbBkWin);
	Public_Show_Widget();
	
	Disable_Algo_Task();

    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/