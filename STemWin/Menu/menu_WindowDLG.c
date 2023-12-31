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
*   菜单界面
*
*/
// USER END

#include "WM.h"
#include "GUI.h"
#include "FRAMEWIN.h"
#include "BUTTON.h"
#include "DIALOG.h"
#include "EDIT.h"
#include "DROPDOWN.h"
#include "TEXT.h"

#include "msg_def.h"
#include "Public_menuDLG.h"




/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//bk color
#define GUI_MENU_ALPHA_COLOR                 0x00525a4e              //背景颜色
#define GUI_MENU_ALPHA                          30                   //透明值
// text
#define GUI_MENU_TEXT_COLOR                 GUI_WHITE              //标题颜色
//button color
#define GUI_MENU_BTN_PRESS_COLOR             0x00e27632              //按钮按下颜色
#define GUI_MENU_BTN_RELEASE_COLOR           0x00cc941d //0x00e27632               //按钮松开颜色
#define GUI_MENU_BTN_FRAME_COLOR             0X00BEBEBE              //按钮边框颜色
#define GUI_MENU_BTN_CHAR_COLOR              GUI_WHITE                //按钮松文本颜色




//ID
#define ID_WINDOW_MENU_WIN          (GUI_MENU_ID + 0x01)
#define ID_BUTTON_MENU_ANALYSIS     (GUI_MENU_ID + 0x02)
#define ID_BUTTON_MENU_LIST_REVIEW  (GUI_MENU_ID + 0x03)
#define ID_BUTTON_MENU_QC           (GUI_MENU_ID + 0x04)
#define ID_BUTTON_MENU_SERVICE      (GUI_MENU_ID + 0x05)
#define ID_BUTTON_MENU_SET          (GUI_MENU_ID + 0x06)
#define ID_BUTTON_MENU_LOGOUT       (GUI_MENU_ID + 0x07)
#define ID_BUTTON_MENU_POWEROFF     (GUI_MENU_ID + 0x08)
#define ID_BUTTON_MENU_CLOSE        (GUI_MENU_ID + 0x09)
#define ID_TEXT_MENU_USER           (GUI_MENU_ID + 0x0a)


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
static const GUI_WIDGET_CREATE_INFO Menu_aDialogCreate[] = {
  { WINDOW_CreateIndirect,  "", ID_WINDOW_MENU_WIN,               0, 0, 200, 480, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,  "",  ID_BUTTON_MENU_ANALYSIS,         0, 90, 200, 50, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,  "", ID_BUTTON_MENU_LIST_REVIEW,       0, 145, 200, 50, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,  "", ID_BUTTON_MENU_QC,                0, 200, 200, 50, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,  "", ID_BUTTON_MENU_SERVICE,           0, 255, 200, 50, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,  "", ID_BUTTON_MENU_SET,               0, 310, 200, 50, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,  "", ID_BUTTON_MENU_LOGOUT,            0, 365, 200, 50, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,  "", ID_BUTTON_MENU_POWEROFF,          0, 420, 200, 50, 0, 0x0, 0 },
  { TEXT_CreateIndirect,    "", ID_TEXT_MENU_USER,                0, 0, 150, 85, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect,  "", ID_BUTTON_MENU_CLOSE,             150, 0, 50, 50, 0, 0x0, 0 },
 
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/


/*
* menu btn callback, 495,15(200, 30)
*/
#define MENU_BTN_PEN_SIZE   20
static void Menu_Btn_Callback(WM_MESSAGE* pMsg)
{
    int Id;
//    const unsigned char RoundRadius = 0; //显示区域圆角半径

    switch (pMsg->MsgId)
    {
    case WM_PAINT:
    {        
        GUI_RECT Rect;

        Id = WM_GetId(pMsg->hWin);
        WM_GetClientRect(&Rect);
        //495,15
        if (BUTTON_IsPressed(pMsg->hWin)) {
			
        } else {
			//设置文本显示模式为透明
			GUI_SetTextMode(GUI_TEXTMODE_TRANS);
			
            if (ID_BUTTON_MENU_ANALYSIS == Id)
            {
                //样本分析
				Show_BK_BMP(BK_SDRAM,ICON_MENU_ANALYSIS_BTN, 0, 0);
				
				GUI_DispStringAt(g_ucaLng_SampleAnalysis[MachInfo.systemSet.eLanguage], 50, 17);
            }
            else if (ID_BUTTON_MENU_LIST_REVIEW == Id) {
                //列表回顾
				Show_BK_BMP(BK_SDRAM,ICON_MENU_LIST_BTN, 0, 0);
				
				GUI_DispStringAt(g_ucaLng_ListReview[MachInfo.systemSet.eLanguage], 50, 17);
            }
            else if (ID_BUTTON_MENU_QC == Id) {
                //质控
				Show_BK_BMP(BK_SDRAM,ICON_MENU_QC_BTN, 0, 0);
				
				GUI_DispStringAt(g_ucaLng_QC[MachInfo.systemSet.eLanguage], 50, 17);
            }
            else if (ID_BUTTON_MENU_SERVICE == Id) {
                //服务
				Show_BK_BMP(BK_SDRAM,ICON_MENU_SERVICE_BTN, 0, 0);
				
				GUI_DispStringAt(g_ucaLng_Service[MachInfo.systemSet.eLanguage], 50, 17);
            }
            else if (ID_BUTTON_MENU_SET == Id) {
                //设置
				Show_BK_BMP(BK_SDRAM,ICON_MENU_SET_BTN, 0, 0);
				
				GUI_DispStringAt(g_ucaLng_Set[MachInfo.systemSet.eLanguage], 50, 17);
            }
            else if (ID_BUTTON_MENU_LOGOUT == Id) {
                //注销
				Show_BK_BMP(BK_SDRAM,ICON_MENU_LOGOUT_BTN, 0, 0);
				
				GUI_DispStringAt(g_ucaLng_Logout[MachInfo.systemSet.eLanguage], 50, 17);
            }
            else if (ID_BUTTON_MENU_POWEROFF == Id) {
                //关机
				Show_BK_BMP(BK_SDRAM,ICON_MENU_OFF_BTN, 0, 0);
				
				GUI_DispStringAt(g_ucaLng_PowerOff[MachInfo.systemSet.eLanguage], 50, 17);
            }
			
			//设置文本显示模式正常模式
			GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
        }
    }
    break;
    default:
        BUTTON_Callback(pMsg);
        break;
    }
}


//menu user text callback
static void  Menu_Text_CallBack(WM_MESSAGE* pMsg)
{
    extern __IO MachRunPara_s MachRunPara;
	extern  MachInfo_s	MachInfo;
    const short xAxis = 78, yAxis = 35;
    switch (pMsg->MsgId)
    {
        case WM_PAINT:
        {
            GUI_RECT Rect;
            WM_GetClientRect(&Rect);

            GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
            GUI_FillRectEx(&Rect);
            //@todo
            //GUI_PNG_Draw(__userlogo, GUI_COUNTOF(__userlogo), 21, 36);

            //TEXT_SetBkColor(pMsg->hWin, GUI_MENU_TEXT_BK_COLOR);
            //TEXT_SetTextColor(pMsg->hWin, GUI_WHITE);

            GUI_SetColor(MachInfo.companyInfo.skin.highlightKHNotSelectFont);
            GUI_SetBkColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
            
            //
			Show_BK_BMP(BK_SDRAM,ICON_MENU_USER_BTN, 20, 20);
            
            
            TEXT_SetFont(pMsg->hWin, &GUI_Font16_1);
            GUI_DispStringAtCEOL(MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].user, xAxis, yAxis);
        }
        break;
        default:
            TEXT_Callback(pMsg);
            break;
    }
}


// USER START (Optionally insert additional static code)
// USER END
//ID
/*
*   界面，初始化
*/
static void Menu_Msg_Init(WM_MESSAGE* pMsg)
{
    WM_HWIN hItem;
    //GUI_RECT Rect;

//    GUI_SetColor(GUI_MENU_BK_COLOR);
//    GUI_FillRect(0, 0, 200, 800);
    
    //menu analysis
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MENU_ANALYSIS);
    WM_SetCallback(hItem, Menu_Btn_Callback);
    
    //menu list review
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MENU_LIST_REVIEW);
    WM_SetCallback(hItem, Menu_Btn_Callback);

    //menu qc
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MENU_QC);
    WM_SetCallback(hItem, Menu_Btn_Callback);

    //menu service
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MENU_SERVICE);
    WM_SetCallback(hItem, Menu_Btn_Callback);

    //menu set
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MENU_SET);
    WM_SetCallback(hItem, Menu_Btn_Callback);

    //menu logout
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MENU_LOGOUT);
    WM_SetCallback(hItem, Menu_Btn_Callback);

    //menu power
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MENU_POWEROFF);
    WM_SetCallback(hItem, Menu_Btn_Callback);

    //menu exit
//    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MENU_CLOSE);
//    WM_SetCallback(hItem, Menu_Btn_Callback);

    //text user
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_MENU_USER);
    WM_SetCallback(hItem, Menu_Text_CallBack);
    
}


static void Menu_Msg_Paint(WM_MESSAGE* pMsg)
{
    GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect); 
    GUI_FillRect(0, 0, 200, 800);
}


/*********************************************************************
*
*       _cbDialog
*/
static void Menu_cbDialog(WM_MESSAGE * pMsg) {
  int NCode;
  int Id;
  // USER START (Optionally insert additional variables)
	extern WM_HWIN CreateSetPage(void);
	
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
  {   
      Menu_Msg_Init(pMsg);
  }
  break;
  case WM_PAINT:
  {
      //Menu_Msg_Paint(pMsg);
      Menu_Msg_Paint(pMsg);
  }
  break;
  case WM_MOTOR_ERROR_PROMPT:
  {
      Motor_Error_Prompt(pMsg, Createmenu_Window);   
  }
  break; 
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_MENU_ANALYSIS: // Notifications sent by 'analysis '
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
          GUI_EndDialog(pMsg->hWin, 0);
          g_hActiveWin = Analysis_Menu();  
      // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
 
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_MENU_LIST_REVIEW: // Notifications sent by 'list review'
        switch (NCode) {
        case WM_NOTIFICATION_CLICKED:
            // USER START (Optionally insert code for reacting on notification message)
            GUI_EndDialog(pMsg->hWin, 0);
            g_hActiveWin = CreateListReview_Window();
            // USER END
            break;
        case WM_NOTIFICATION_RELEASED:
            // USER START (Optionally insert code for reacting on notification message)
            GUI_EndDialog(pMsg->hWin, 0);
            g_hActiveWin = CreateListReview_Window();
            // USER END
            break;
            // USER START (Optionally insert additional code for further notification handling)
            // USER END
        }
        break;
     case ID_BUTTON_MENU_QC: // Notifications sent by 'qc'
        switch (NCode) {
        case WM_NOTIFICATION_CLICKED:
            // USER START (Optionally insert code for reacting on notification message)
            
            GUI_EndDialog(pMsg->hWin, 0);
            g_hActiveWin = QC_WindowDlG();
        
            // USER END
            break;
        case WM_NOTIFICATION_RELEASED:
            // USER START (Optionally insert code for reacting on notification message)
            // USER END
            break;
            // USER START (Optionally insert additional code for further notification handling)
            // USER END
    }
    break;
    case ID_BUTTON_MENU_SERVICE: // Notifications sent by 'Service_Btn'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        GUI_EndDialog(pMsg->hWin, 0);
        g_hActiveWin = CreateservicePage();
      // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)

        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_MENU_SET: // Notifications sent by 'Set_Btn'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
       	GUI_EndDialog(pMsg->hWin, 0);
        g_hActiveWin = CreateSetPage();
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)

        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_MENU_LOGOUT: // Notifications sent by 'LogOut_Btn'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
      {
			extern WM_HWIN Analysis_Menu(void);
		  
            //是否关机提示
            GUI_EndDialog(pMsg->hWin, 0);
            g_hActiveWin = CreateSwitchDialogPage();
            //
            CommonSwitchDialogPageData_s SwitchDialogData = {0};
            SwitchDialogData.switchDialogItem = MENU_LOGOUT_PROMPT;
            SwitchDialogData.u32 = 0;
            SwitchDialogData.dialogData.bmpType = BMP_ALARM;
            SwitchDialogData.dialogData.confimButt = HAS_CONFIM_BUTTON;
            SwitchDialogData.dialogData.fun = Analysis_Menu;					
            strcpy(SwitchDialogData.dialogData.str, g_ucaLng_Ensure_Logout[MachInfo.systemSet.eLanguage]);

            pMsg->MsgId = WM_USER_DATA;
            pMsg->Data.p = &SwitchDialogData;
            WM_SendMessage(g_hActiveWin,pMsg);
      }
      break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)

        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_MENU_POWEROFF: // Notifications sent by 'PowerOff_Btn'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
      {
		extern WM_HWIN Analysis_Menu(void);
          
        //是否关机提示
        GUI_EndDialog(pMsg->hWin, 0);
	    g_hActiveWin = CreateSwitchDialogPage();
        //
        CommonSwitchDialogPageData_s SwitchDialogData = {0};
        SwitchDialogData.switchDialogItem = MENU_POWER_OFF_PROMPT;
        SwitchDialogData.u32 = 0;
        SwitchDialogData.dialogData.bmpType = BMP_ALARM;
        SwitchDialogData.dialogData.confimButt = HAS_CONFIM_BUTTON;
        SwitchDialogData.dialogData.fun = Analysis_Menu;					//保存进入对话框前的界面创建函数
        strcpy(SwitchDialogData.dialogData.str, g_ucaLng_Ensure_PowerOff[MachInfo.systemSet.eLanguage]);
        
        pMsg->MsgId = WM_USER_DATA;
        pMsg->Data.p = &SwitchDialogData;
        WM_SendMessage(g_hActiveWin,pMsg);
            
      }
      break;
      case WM_NOTIFICATION_RELEASED:
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



//
static void Menu_cbBkWin(WM_MESSAGE* pMsg)
{
    switch (pMsg->MsgId)
    {
        case WM_PAINT:
        {
            GUI_SetAlpha(GUI_MENU_ALPHA);
            GUI_SetColor(GUI_MENU_ALPHA_COLOR);
            GUI_FillRect(200, 0, 800, 480);
            GUI_SetAlpha(0);

        }
        break;
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
*       Createmenu_Window
*/
WM_HWIN Createmenu_Window(void);
WM_HWIN Createmenu_Window(void) {
  WM_HWIN hWin;

  Public_Hide_Widget(); //隐藏公共部分控件
  WM_SetCallback(WM_HBKWIN, Menu_cbBkWin);
	
	Disable_Algo_Task();
    
  Set_Cur_MenuID(GUI_MENU_ID);
  //菜单按键背景高亮
  Public_Reset_Btn_Status(BTN_MENU);
	
  hWin = GUI_CreateDialogBox(Menu_aDialogCreate, GUI_COUNTOF(Menu_aDialogCreate), Menu_cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
