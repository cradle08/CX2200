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
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0 (JIAOHU_CHECK_PAGE_ID + 0x0E)
#define ID_BUTTON_0 (JIAOHU_CHECK_PAGE_ID + 0x0F)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
static uint8_t PressButtCount = 0;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "screenRGBTestPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "", ID_BUTTON_0, 0, 0, 800, 480, 0, 0x0, 0 },
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
static void _ScreenRGBTestPageRedrawWidgetType_Callback(WM_MESSAGE* pMsg)
{
	extern __IO MachRunPara_s	MachRunPara;
	
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
    {
        GUI_RECT Rect;
        WM_GetClientRect(&Rect);
		
		switch(PressButtCount){
			case 0:
				GUI_SetColor(GUI_RED);
			break;
			
			case 1:
				GUI_SetColor(GUI_GREEN);
			break;
			
			case 2:
				GUI_SetColor(GUI_BLUE);
			break;
			default :
				GUI_SetColor(GUI_RED);
			break;
		}
		
		GUI_FillRectEx(&Rect);
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
static void _ScreenRGBTestPageRedrawWidgetType(WM_MESSAGE* pMsg)
{
    WM_HWIN hItem;
	
    //返回按钮
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);  
    WM_SetCallback(hItem, _ScreenRGBTestPageRedrawWidgetType_Callback);
    WM_InvalidateWindow(hItem);
}

// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  int NCode;
  int Id;
  // USER START (Optionally insert additional variables)
	extern WM_HWIN CreatejiaoHuCheckPage(void);
  // USER END

  switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
		PressButtCount = 0;
	
		//重绘按钮样式
		_ScreenRGBTestPageRedrawWidgetType(pMsg);
	break;
		  
	  
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		PressButtCount++;
	  
		if(PressButtCount >= 3){
			GUI_EndDialog(pMsg->hWin, 0);
			g_hActiveWin = CreatejiaoHuCheckPage();
		}
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
*       CreatescreenRGBTestPage
*/
WM_HWIN CreatescreenRGBTestPage(void);
WM_HWIN CreatescreenRGBTestPage(void) {
  WM_HWIN hWin;


	WM_SetCallback(WM_HBKWIN, AlphaBk_cbBkWin);
	Public_Hide_Widget();
	
	Disable_Algo_Task();
	
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/