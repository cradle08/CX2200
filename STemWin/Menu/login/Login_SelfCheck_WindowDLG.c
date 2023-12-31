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
/*
*
*   登录自检界面，显示自检进度（%）
*
*/
// USER END

#include "DIALOG.h"


#include "ui_files.h"
#include "Public_menuDLG.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_LOGIN_SELFCHECK   (GUI_LOGIN_SELFCHECK_ID + 0x00)
#define ID_PROGBAR_0                (GUI_LOGIN_SELFCHECK_ID + 0x01)
#define ID_TEXT_0                   (GUI_LOGIN_SELFCHECK_ID + 0x02)


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
static const GUI_WIDGET_CREATE_INFO Login_SelfCheck_aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_LOGIN_SELFCHECK, 0, 0, 800, 480, 0, 0x0, 0 },
  { PROGBAR_CreateIndirect, "",      ID_PROGBAR_0,          190, 230, 420, 20, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "",         ID_TEXT_0,             190, 270, 420, 40, 0, 0x0, 0 },
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




/*********************************************************************
*
*       _cbDialog
*/
#define GUI_LOGIN_PROGBAR_COLOR_0		GUI_BLUE
#define GUI_LOGIN_PROGBAR_COLOR_1		GUI_WHITE
static void Login_SelfCheck_cbDialog(WM_MESSAGE * pMsg) {
  // USER START (Optionally insert additional variables)
  // USER END
   extern __IO MachRunPara_s MachRunPara;
   WM_HWIN hItem;
	char buffer[128] = {0};

  switch (pMsg->MsgId) {
  // USER START (Optionally insert additional message handling)
  // USER END
  case WM_INIT_DIALOG:
  {
      //progbar
      PROGBAR_SKINFLEX_PROPS Props;
      hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
      PROGBAR_SetMinMax(hItem, 0, 100);
      //
      PROGBAR_GetSkinFlexProps(&Props, 0);
      Props.aColorUpperL[0] = MachInfo.companyInfo.skin.highlightKJNotSelect;
      Props.aColorUpperL[1] = MachInfo.companyInfo.skin.highlightKJNotSelect;
      Props.aColorLowerL[0] = MachInfo.companyInfo.skin.highlightKJNotSelect;
      Props.aColorLowerL[1] = MachInfo.companyInfo.skin.highlightKJNotSelect;
      //
      Props.aColorUpperR[0] = 0x00f7f7f7;
      Props.aColorUpperR[1] = 0x00f7f7f7;
      Props.aColorLowerR[0] = 0x00f7f7f7;
      Props.aColorLowerR[0] = 0x00f7f7f7;
      //
      Props.ColorFrame  = MachInfo.companyInfo.skin.buttBoard;
      Props.ColorText   = GUI_BLACK;
      PROGBAR_SetSkinFlexProps(&Props, 0);
      WM_InvalidateWindow(hItem);
      
      //text
      hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
      TEXT_SetTextColor(hItem, GUI_WHITE);
      TEXT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_HCENTER);
	  sprintf(buffer, "%s...", g_ucaLng_Login_SelfCheck[MachInfo.systemSet.eLanguage]);
      TEXT_SetText(hItem, buffer);
      
      //清空开机自检信息，防止对重复多次开机自检的影响
      memset((void*)&MachRunPara.tLogin_SelfCheck, 0, sizeof(MachRunPara.tLogin_SelfCheck));
  }
  break;
  case WM_PAINT:
  {
      
      GUI_SetColor(GUI_LIGHTBLUE);
      GUI_Clear();
      
      //bk bmp
      Show_BK_BMP(BK_SDRAM, BK_PICTURE_OTHERS_SELF_CHECK, 0, 0);//Show_BK_BMP(BK_PICTURE_OTHERS_SELF_CHECK, 0, 0);
      
      //progbar
      hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
      if(MachRunPara.tLogin_SelfCheck.ucProgress > 100) MachRunPara.tLogin_SelfCheck.ucProgress = 100;
      PROGBAR_SetValue(hItem, MachRunPara.tLogin_SelfCheck.ucProgress);
      
      //@todo 需要做一个异常处理，当没收返回ucProgress==100的消息时，应设置超时后，自动进入样本分析界面
      if(100 == MachRunPara.tLogin_SelfCheck.ucProgress)
      {
          //给自己发消息，自检完成
          WM_SendMessageNoPara(pMsg->hWin, WM_LOGIN_SELFCHECK_FINISH); 
          MachRunPara.tLogin_SelfCheck.ucProgress = 0;          
      }
  }
  break;
  case WM_LOGIN_SELFCHECK_FINISH:
  {
      uint8_t i = 0; 
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
          
     //自检完成，转入样本分析界面
      GUI_EndDialog(pMsg->hWin, 0);
//      GUI_Exec();
      g_hActiveWin = WM_HBKWIN;
      Public_Msg_Init();
      g_hActiveWin = Analysis_Menu();
      
      //发送消息给你后端，加载用户数据
      Msg_Head_t tMsgHead;
      tMsgHead.usCmd   = CMD_LOGIN_OK_LOAD_USER_DATA;
      UI_Put_Msg((uint8_t*)&tMsgHead);
       
  }
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
WM_HWIN Login_SelfCheck_CreateWindow(void);
WM_HWIN Login_SelfCheck_CreateWindow(void) {
  WM_HWIN hWin;
	
	Disable_Algo_Task();
	
  hWin = GUI_CreateDialogBox(Login_SelfCheck_aDialogCreate, GUI_COUNTOF(Login_SelfCheck_aDialogCreate), Login_SelfCheck_cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
