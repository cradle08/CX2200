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
*   计数仓未进仓时，弹出界面，提示N秒后 计数仓进仓
*
*/

// USER END
#include "DIALOG.h"
#include "WM.h"
#include "GUI.h"
#include "LISTBOX.h"


#include "Public_menuDLG.h"
#include "file_operate.h"
#include "PrinterTask.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/



//ID
#define ID_WINDOW_N_SEC_OUTIN_IN       (GUI_WINDOW_N_SEC_OUTIN_IN_ID + 0x00)
#define ID_N_SEC_OUTIN_IN_TEXT_SEC     (GUI_WINDOW_N_SEC_OUTIN_IN_ID + 0x01)
//#define ID_N_SEC_OUTIN_IN_BUTTON_SURE   (GUI_WINDOW_N_SEC_OUTIN_IN_ID + 0x02)
//#define ID_N_SEC_OUTIN_IN_BUTTON_NO    (GUI_WINDOW_N_SEC_OUTIN_IN_ID + 0x03)
//timer
#define N_SEC_OUTIN_IN_TIMER_ID        (GUI_WINDOW_N_SEC_OUTIN_IN_ID + 0x05)
#define N_SEC_OUTIN_IN_TIMER_PERIOD     1000    //1s


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
//最延时时间，单位秒
static  unsigned char gs_ucMaxDelayTime = 0;


// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/

static const GUI_WIDGET_CREATE_INFO  NSec_OutIn_In_aDialogCreate[] = {
  { WINDOW_CreateIndirect, "", 				 ID_WINDOW_N_SEC_OUTIN_IN,    	180, 100, 440, 225, 0, 0x0, 0 },
  { TEXT_CreateIndirect,   "",       	     ID_N_SEC_OUTIN_IN_TEXT_SEC,    68, 110, 40, 40, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "",         		 ID_N_SEC_OUTIN_IN_BUTTON_SURE,  0, 185, 440, 40, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "",         		 ID_N_SEC_OUTIN_IN_BUTTON_YES, 	220, 185, 220, 40, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO  en_NSec_OutIn_In_aDialogCreate[] = {
  { WINDOW_CreateIndirect, "", 				 ID_WINDOW_N_SEC_OUTIN_IN,    	180, 100, 440, 225, 0, 0x0, 0 },
  { TEXT_CreateIndirect,   "",       	     ID_N_SEC_OUTIN_IN_TEXT_SEC,    90, 102, 40, 40, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "",         		 ID_N_SEC_OUTIN_IN_BUTTON_SURE,  0, 185, 440, 40, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "",         		 ID_N_SEC_OUTIN_IN_BUTTON_YES, 	220, 185, 220, 40, 0, 0x0, 0 },
  
};



/*********************************************************************
*
*       _cbDialog
*/
static void NSec_OutIn_In_cbDialog(WM_MESSAGE* pMsg) {
    
    WM_HWIN hItem;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
    {
        extern __IO MachRunPara_s MachRunPara;
        
        //最大延时g_sMaxDelayTime秒
        gs_ucMaxDelayTime = 5;//MachRunPara.paramSet.ucNSec_OutIn_In; 
        
		//N Sec
		hItem = WM_GetDialogItem(pMsg->hWin, ID_N_SEC_OUTIN_IN_TEXT_SEC);
        TEXT_SetTextColor(hItem, GUI_BLUE);
        TEXT_SetFont(hItem, &GUI_Font24B_ASCII);
		TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
        TEXT_SetDec(hItem, gs_ucMaxDelayTime, 1, 0, 0, 0);
        
        //
        //timer, 周期1s, 刷新倒计时
        WM_CreateTimer(pMsg->hWin, N_SEC_OUTIN_IN_TIMER_ID, N_SEC_OUTIN_IN_TIMER_PERIOD, 0);   

    }
    break;
    case WM_PAINT:
    {
        Show_BK_BMP(BK_FILE, BK_PICTURE_N_SEC_OUTIN_IN, 100, 25);
    }
    break;
    case WM_DELETE:
    {
        gs_ucMaxDelayTime = 0; 
    }
    break;
    case WM_TIMER:
    {
        gs_ucMaxDelayTime--;
        if(gs_ucMaxDelayTime > 0)
        {
            //刷新显示时间
            hItem = WM_GetDialogItem(pMsg->hWin, ID_N_SEC_OUTIN_IN_TEXT_SEC);
            TEXT_SetDec(hItem, gs_ucMaxDelayTime, 1, 0, 0, 0);
            WM_RestartTimer(pMsg->Data.v, N_SEC_OUTIN_IN_TIMER_PERIOD);  
        }else {
			Msg_Head_t tMsgHead = {0};
			
			//计数结束
			//删除定时器
			WM_DeleteTimer(pMsg->Data.v);  

			//进仓
			tMsgHead.usCmd = CMD_OTHERS_OUTIN_IN;
			UI_Put_Msg((uint8_t*)&tMsgHead);

			//是否自动打印: 自动打印&结果正常下，自动打印
			if(PRINTF_WAY_AUTO == MachInfo.printSet.ePrintWay && MachRunPara.tWBCHGB_TestInfo.tMsgHead.eErrorCode == ERROR_CODE_SUCCESS)
			{
				//自动打印
				CreateAndSend_WBCHGB_PrintData(&MachRunPara.tWBCHGB_TestInfo);
			}

			//切换到样本分析界面
			GUI_EndDialog(pMsg->hWin, 0);
			g_hActiveWin = Analysis_Menu();
        }
    }
    break;
    case WM_NOTIFY_PARENT:
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


WM_HWIN NSec_OutIn_In_WindowDLG(void);
WM_HWIN NSec_OutIn_In_WindowDLG(void) {
    WM_HWIN hWin;
    
    Public_Hide_Widget(); //隐藏公共部分控件,
    WM_SetCallback(WM_HBKWIN, None_cbBkWin); //AlphaBk_cbBkWin
	
	Disable_Algo_Task();
	
    if(LANGUAGE_ENGLISH == MachInfo.systemSet.eLanguage)
    {
        hWin = GUI_CreateDialogBox(en_NSec_OutIn_In_aDialogCreate, GUI_COUNTOF(en_NSec_OutIn_In_aDialogCreate), NSec_OutIn_In_cbDialog, WM_HBKWIN, 0, 0);
    }else{
        hWin = GUI_CreateDialogBox(NSec_OutIn_In_aDialogCreate, GUI_COUNTOF(NSec_OutIn_In_aDialogCreate), NSec_OutIn_In_cbDialog, WM_HBKWIN, 0, 0);
    }
    return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
