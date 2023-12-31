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
*  弹出界面，提示选择“是”合”否“， 当前运用于
*	1、样本分析失败后，提示是否重新计数
*	2、列表回顾合结果详情中，删除数据时，提示是否删除数据
*
*/

// USER END
#include "DIALOG.h"
#include "WM.h"
#include "GUI.h"
#include "LISTBOX.h"


#include "Public_menuDLG.h"
#include "file_operate.h"
#include "parameter.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/



//ID
#define ID_WINDOW_SELECT_YES_NO       	 (GUI_WINDOW_SELECT_YES_NO_ID + 0x00)
#define ID_SELECT_YES_NO_PROMPT_PICTURE  (GUI_WINDOW_SELECT_YES_NO_ID + 0x01)
#define ID_SELECT_YES_NO_PROMPT_TEXT  	 (GUI_WINDOW_SELECT_YES_NO_ID + 0x02)
#define ID_SELECT_YES_NO_BUTTON_YES   	 (GUI_WINDOW_SELECT_YES_NO_ID + 0x03)
#define ID_SELECT_YES_NO_BUTTON_NO    	 (GUI_WINDOW_SELECT_YES_NO_ID + 0x04)




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

static const GUI_WIDGET_CREATE_INFO ReCount_Prompt_aDialogCreate[] = {
  { WINDOW_CreateIndirect, "",	ID_WINDOW_SELECT_YES_NO,     		180, 100, 440, 225, 0, 0x0, 0 },
  { TEXT_CreateIndirect,   "",  ID_SELECT_YES_NO_PROMPT_PICTURE,    80, 80, 140, 35, 0, 0x0, 0 },
  { TEXT_CreateIndirect,   "",  ID_SELECT_YES_NO_PROMPT_TEXT,       80, 80, 140, 135, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "",  ID_SELECT_YES_NO_BUTTON_NO,  		0, 185, 220, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "",  ID_SELECT_YES_NO_BUTTON_YES, 		220, 185, 220, 40, 0, 0x0, 0 },
  
};





/*********************************************************************
*
*       Static code
*
**********************************************************************
*/


/*
*    btn callback
*/
static void Select_YesNo_Btn_Callback(WM_MESSAGE* pMsg)
{
    const unsigned char RoundRadius = 0; //显示区域圆角半径
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
    {
        GUI_RECT Rect;
        WM_GetClientRect(&Rect);

        if (BUTTON_IsPressed(pMsg->hWin)) {
            //draw frame
            GUI_SetColor(GUI_BTN_PRESS_COLOR);
            GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, RoundRadius);
            GUI_SetColor(GUI_BTN_FRAME_COLOR);
            //GUI_AA_DrawRoundedRect(Rect.x0 + 1, Rect.y0 + 1, Rect.x1 + 1, Rect.y1 + 1, RoundRadius);
            GUI_DrawRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);;

            // display char
            GUI_SetColor(GUI_BTN_PRESS_CHAR_COLOR);
            GUI_SetBkColor(GUI_BTN_PRESS_COLOR);
            if(ID_SELECT_YES_NO_BUTTON_YES == WM_GetId(pMsg->hWin))
            {
                //是
                GUI_DispStringInRect(g_ucaLng_Yes[g_tUI_Info.tSystemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }else if(ID_SELECT_YES_NO_BUTTON_YES == WM_GetId(pMsg->hWin)){
                //否
                GUI_DispStringInRect(g_ucaLng_No[g_tUI_Info.tSystemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }                
        }
        else {
            // draw frame
            GUI_SetColor(GUI_BTN_RELEASE_COLOR);
            GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, RoundRadius);
            GUI_SetColor(GUI_BTN_FRAME_COLOR);
            //GUI_AA_DrawRoundedRect(Rect.x0 + 1, Rect.y0 + 1, Rect.x1, Rect.y1, RoundRadius);
            GUI_DrawRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);;

            // display char
            GUI_SetColor(GUI_BTN_RELEASE_CHAR_COLOR);
            GUI_SetBkColor(GUI_BTN_RELEASE_COLOR);
            if(ID_SELECT_YES_NO_BUTTON_YES == WM_GetId(pMsg->hWin))
            {
                //是
                GUI_DispStringInRect(g_ucaLng_Yes[g_tUI_Info.tSystemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }else if(ID_SELECT_YES_NO_BUTTON_NO == WM_GetId(pMsg->hWin)){
                //否
                GUI_DispStringInRect(g_ucaLng_No[g_tUI_Info.tSystemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }  
  
        }
    }
    break;
    default:
        BUTTON_Callback(pMsg);
        break;
    }
}



// image text callback
static void _Select_YesNo_Text_Image_Callback(WM_MESSAGE* pMsg, DialogBmpType_e eBmpType)
{
    switch(pMsg->MsgId)
    {
        case WM_PAINT:
        {
            GUI_RECT Rect;
            GUI_GetClientRect(&Rect);
            GUI_SetColor(GUI_WHITE);
            GUI_FillRectEx(&Rect);
            
            switch(eBmpType){
			case 0:
                Show_BK_BMP(BK_SDRAM, BK_PICTURE_ANALYSIS_WAITING, 0, 0);
			break;
			
			case 1:
                Show_BK_BMP(BK_SDRAM, BK_PICTURE_ANALYSIS_WARN_BLUE, 0, 0);
			break;
			
			case 2:
                Show_BK_BMP(BK_FILE, BK_PICTURE_ANALYSIS_WANR_WHITE, 0, 0);
			break;
            }
        }
        break;
        default:
            TEXT_Callback(pMsg);
            break;
    }

}

//iamge 
static void Select_YesNo_Text_Image_Callback(WM_MESSAGE* pMsg)
{
    _Select_YesNo_Text_Image_Callback(pMsg, SwitchDialogData.dialogData.bmpType);
}



/*********************************************************************
*
*       _cbDialog
*/
static void Select_YesNo_cbDialog(WM_MESSAGE* pMsg) {
    
    extern __IO DataManage_t g_tDataManage;
    WM_HWIN hItem;
    int     NCode;
    int     Id;
//    char buffer[50] = {0};


    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
	{
        //
        // Initialization of 'Remark_Multiedit'
        //...
		//btn NO
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SELECT_YES_NO_BUTTON_NO);
		WM_SetCallback(hItem, Select_YesNo_Btn_Callback);
        WM_InvalidateWindow(hItem);
		
		//btn Yes
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SELECT_YES_NO_BUTTON_YES);
		WM_SetCallback(hItem, Select_YesNo_Btn_Callback);
        WM_InvalidateWindow(hItem);
		
		//text image
		hItem =WM_GetDialogItem(pMsg->hWin, ID_SELECT_YES_NO_PROMPT_PICTURE);
		WM_SetCallback(hItem, Select_YesNo_Text_Image_Callback);
		
		//text 
		hItem =WM_GetDialogItem(pMsg->hWin, ID_SELECT_YES_NO_PROMPT_TEXT);
		TEXT_SetFont(hItem, );
	
	}
    break;
    case WM_PAINT:
    {
        GUI_SetAlpha(30);
		GUI_SetColor(0x00525a4e);
		GUI_FillRectEx(&Rect);
		GUI_SetAlpha(0);
	  
		//重绘弹窗为圆角矩形
		GUI_SetColor(0x00ffffff);
		GUI_AA_FillRoundedRectEx(&Rect,6);
    }
    break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_SELECT_YES_NO_BUTTON_NO: // Notifications sent by 'No BUTTON '
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                case WM_NOTIFICATION_RELEASED:
                {
					//@todo
	
                }
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
			case ID_SELECT_YES_NO_BUTTON_YES: // Notifications sent by 'Yes BUTTON '
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                case WM_NOTIFICATION_RELEASED:
                {
					//@todo

                }
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
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
*       ReCount_Prompt
*/



WM_HWIN Seletc_YesNO_Window(void);
WM_HWIN Seletc_YesNO_Window(void) {
    WM_HWIN hWin;
    
	WM_SetCallback(WM_HBKWIN, AlphaBk_cbBkWin);
	Public_Hide_Widget();
	
    //GUI_Clear();
    //Public_Hide_Widget(); //隐藏公共部分控件,
    //WM_SetCallback(WM_HBKWIN, None_cbBkWin);
    
    hWin = GUI_CreateDialogBox(Select_YesNo_aDialogCreate, GUI_COUNTOF(Select_YesNo_aDialogCreate), Select_YesNo_cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
