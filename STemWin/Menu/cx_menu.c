#include <stddef.h>
#include <string.h>
#include "WM.h"
#include "GUI.h"
#include "FRAMEWIN.h"
#include "BUTTON.h"
#include "LISTVIEW.h"
#include "EDIT.h"
#include "DROPDOWN.h"
#include "TEXT.h"
#include "GUI_Touch.h"

#include "cx_menu.h"
#include "msg_def.h"
#include "ui_msg.h"
#include "Public_menuDLG.h"
#include "bsp_outin.h"
#include "ui_files.h"
#include "file_operate.h"
#include "bsp_eeprom.h"
#include "pcba.h"
#include "xpt2046.h"





//#define GUI_SIMULATION
#ifndef GUI_SIMULATION
//
#endif



/*
*   影藏UI
*/
void UI_Hide(void)
{
    Public_Hide_Widget();
    WM_HideWindow(g_hActiveWin);
    
    GUI_SetColor(GUI_BLACK);
    GUI_Clear();
}



/*
*   显示UI
*/
void UI_Show(void)
{
    Public_Show_Widget();
    WM_InvalidateWindow(WM_HBKWIN);
    WM_ShowWindow(g_hActiveWin);
}



//#define RECOMMENDED_MEMORY (1024L * 50)


