#include "DIALOG.h"
#include "ui_files.h"
#include "Public_menuDLG.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_POWEROFF              (GUI_WINDOW_POWEROFF_ID + 0x00)
#define ID_POWER_OFF_PROMPT_TEXT        (ID_WINDOW_POWEROFF + 0x02)

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
static const GUI_WIDGET_CREATE_INFO PowerOff_aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_POWEROFF,    0, 0, 800, 480, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "",         ID_POWER_OFF_PROMPT_TEXT, 200, 210, 400, 60, 0, 0x0, 0 },
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
static void PowerOff_cbDialog(WM_MESSAGE * pMsg) {
  // USER START (Optionally insert additional variables)
  // USER END
  // extern __IO MachRunPara_s MachRunPara;
   WM_HWIN hItem;
	
	//为0表示显示关机画面中，为1表示显示关闭电源提示符中
	static uint8_t CloseStatusFlag = 0;

  switch (pMsg->MsgId) {
  // USER START (Optionally insert additional message handling)
  // USER END
 
  case WM_INIT_DIALOG:
  {
      //text, power off prompt
      hItem = WM_GetDialogItem(pMsg->hWin, ID_POWER_OFF_PROMPT_TEXT);
      TEXT_SetTextColor(hItem, GUI_WHITE);
      TEXT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_HCENTER);
	  if(MachInfo.systemSet.eLanguage == LANGUAGE_CHINESE){
		TEXT_SetFont(hItem, &HZ_SONGTI_24);
	  }else{
		TEXT_SetFont(hItem, &GUI_Font32_ASCII);
	  }
	  TEXT_SetText(hItem, "");
      
      //WBC恒流源关
      BC_CUR_SW_OFF;
	  
	  CloseStatusFlag = 0;
  }
  break;
  
  case WM_PAINT:
  {
	  if(CloseStatusFlag == 0){
		Show_BK_BMP(BK_SDRAM,BK_PICTURE_OTHERS_POWER_ON_OFF,0,0);
	  }else{
		GUI_RECT Rect;
		WM_GetClientRect(&Rect);
		  
		GUI_SetColor(GUI_BLACK);
		GUI_FillRectEx(&Rect);
	  }
  }
  break;
  
  case WM_USER_DATA:
  {
	CloseStatusFlag = 1;

	//无效化此界面
	WM_InvalidateWindow(pMsg->hWin);
	  
	hItem = WM_GetDialogItem(pMsg->hWin, ID_POWER_OFF_PROMPT_TEXT);
	if(MachInfo.systemSet.eLanguage == LANGUAGE_CHINESE){
		TEXT_SetText(hItem, "请关闭电源");
	}else{
		TEXT_SetText(hItem, "Please Turn Off The Power");
	}
  }break;
  
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
WM_HWIN PowerOff_CreateWindow(void);
WM_HWIN PowerOff_CreateWindow(void) {
  WM_HWIN hWin;
	
	Disable_Algo_Task();
    
  hWin = GUI_CreateDialogBox(PowerOff_aDialogCreate, GUI_COUNTOF(PowerOff_aDialogCreate), PowerOff_cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
