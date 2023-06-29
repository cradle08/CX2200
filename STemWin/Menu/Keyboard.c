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
//
#include "cx_menu.h"
#include "msg_def.h"
#include "ui_msg.h"
#include "Public_menuDLG.h"
#include "ui_files.h"
#include "Keyboard.h"
#include "pysearch.h"
#include "Public_menuDLG.h"



static __IO WM_HWIN gs_hKeyBoardWin = 0xFFFFFFFF;  //键盘活动窗口句柄

/*
*********************************************************************************************************
*	                         宏定义和结构体
*********************************************************************************************************
*/
#define COLOR_BORDER           0x00E4E4E4  //0x00EEDFB2//0x444444
#define COLOR_KEYPAD0          0xAAAAAA
#define COLOR_KEYPAD1          0x555555

#define BUTTON_PRESSED_COLOR    0x00A9A9A9   //0x00A9A9A9
#define BUTTON_ENABLE_COLOR     0x00F5F5F5
#define BUTTON_SKINFLEX_RADIUS  3

#define APP_INIT_LOWERCASE     (WM_USER + 0)


#define KEYBOARD_RESULT_BUFFER_LEN	5
__IO char g_caKeyboard_Buffer[KEYBOARD_RESULT_BUFFER_LEN];


typedef struct {
  int          xPos;
  int          yPos;
  int          xSize;
  int          ySize;
  const char * acCharSmall;   	// 按钮对应的小写字符 
  const char * acCharBig;  		// 按钮对应的大写字符 
  const char * acChar123;  		// 按钮对应的数值，符号字符 
  char         cControl;   	// 按钮对应的控制位，1表示功能按钮，0表示普通按钮, 2:不需要处理
} BUTTON_DATA;


/*
*	大小写标志
*/
typedef enum{
	EN_BIG		= 0,	//大写
	EN_SMALL	= 1,	//小写
}Big_Small_e;


/*
*	中英文标志
*/
typedef enum{
	EN_CHINESE	= 0,	//中文
	EN_ENGLISH	= 1,	//英文
	
}Chinese_English_e;


/*
*	消息类型标志
*/
typedef enum{
	MSG_TYPE_CHINESE	= 0,	//中文
	MSG_TYPE_OTHERS 	= 1,	//英文,数字，符号等
	
}MsgType_e;



/*
*	字母，字符数字，显示形式
*/
typedef enum{
	EN_CHAR			= 0,	//字符
	EN_SYMBOL_NUM	= 1,	//符号数字
	
}CharSymbolNum_e;



/*
*	中文提示，显示状态
*/
typedef enum{
	EN_HZ_HIDE		= 0,	//隐藏
	EN_HZ_HIDE_KEEP	= 1,	//保持隐藏状态
	EN_HZ_SHOW		= 2,	//显示
	EN_HZ_SHOW_KEEP	= 3,	//保持显示状态	
}HZ_Hide_e;




//键盘状态结构体
#define PROMPT_HZ_NUM_PER_LINE  7           //提示汉字，一行最大7个
#define INPUT_CHAR_MAX_LEN      10          //输入拼音最多10个
typedef struct{
	Big_Small_e 		eBigSmall;		    //大小写标志
	Chinese_English_e 	eCE;			    //中英文标志
	CharSymbolNum_e		eChar_SymbolNum;    //字母，字符数字
	unsigned char		caInputBuffer[INPUT_CHAR_MAX_LEN]; //输入的索引拼音
	unsigned char		cInputIndex;	    //输入索引拼音的下标
	char				*pcHzBuffer;	    //根据输入拼音索引到的所有汉字
	unsigned int 		ulHzNum; 		    //根据输入拼音索引到的所有汉字个数
    unsigned int 		ulPageNum; 		    //索引到的汉字，分页显示数
    unsigned int 		ulPageIndex;        //索引到的汉字，当前显示分页标号，即：ulHzNum/PROMPT_HZ_NUM_PER_LINE
    HZ_Hide_e           HzHideFlag;         //汉字提示行，显示或隐藏标志， 0：隐藏，1：显示

} KeyboardStatus_t;
static __IO KeyboardStatus_t g_tKeyBoardStats = {0};
//static WM_HWIN   hMulti;


//键大小，分格位置
#define BUTTON_SPLIT_X  	2	//每行键的间隔
#define BUTTON_SPLIT_Y  	2	//每列键的间隔
#define BUTTON_WHITH   		78	//每个普通键的宽度，如：a
#define BUTTON_WHITH_L   	117	//每个长键的宽度，如：完成
#define BUTTON_WHITH_BLACK	(BUTTON_WHITH*6+BUTTON_SPLIT_X*5)	//空格键长度
#define BUTTON_HIGHT   		40	//每个键的高度




//

//BUTTON Data 数据下标
typedef enum{
    BUTTON_DATA_InputChar   = 0,
    BUTTON_DATA_HZ_1        = 1,
    BUTTON_DATA_HZ_2        = 2,
    BUTTON_DATA_HZ_3        = 3,
    BUTTON_DATA_HZ_4        = 4,
    BUTTON_DATA_HZ_5        = 5,
    BUTTON_DATA_HZ_6        = 6,
    BUTTON_DATA_HZ_7        = 7,
    BUTTON_DATA_LAST_PAGE   = 8,
    BUTTON_DATA_NEXT_PAGE   = 9,
    //
	BUTTON_DATA_qQ			= 10,
	BUTTON_DATA_wW			= 11,
	BUTTON_DATA_eE			= 12,
	BUTTON_DATA_rR			= 13,
	BUTTON_DATA_tT			= 14,
	BUTTON_DATA_yY			= 15,
	BUTTON_DATA_uU			= 16,
	BUTTON_DATA_iI			= 17,
	BUTTON_DATA_oO			= 18,
	BUTTON_DATA_pP			= 19,
	BUTTON_DATA_aA			= 20,
	BUTTON_DATA_sS			= 21,
	BUTTON_DATA_dD			= 22,
	BUTTON_DATA_fF			= 23,
	BUTTON_DATA_gG			= 24,
	BUTTON_DATA_hH			= 25,
	BUTTON_DATA_jJ			= 26,
	BUTTON_DATA_kK			= 27,
	BUTTON_DATA_lL			= 28,
	BUTTON_DATA_CapsLock	= 29,
	BUTTON_DATA_zZ			= 30,
	BUTTON_DATA_xX			= 31,
	BUTTON_DATA_cC			= 32,
	BUTTON_DATA_vV			= 33,
	BUTTON_DATA_bB			= 34,
	BUTTON_DATA_nN			= 35,
	BUTTON_DATA_mM			= 36,
	BUTTON_DATA_Del			= 37,
	BUTTON_DATA_ABC123		= 38,
	BUTTON_DATA_Blank		= 39,
	BUTTON_DATA_Shift		= 40,
	BUTTON_DATA_Enter		= 41,	
	BUTTON_DATA_END			= 42,
    
}Button_Data_e;


//
static const BUTTON_DATA _aButtonData[BUTTON_DATA_END] = 
{
    /* 第0排按钮 */
	{ BUTTON_SPLIT_X*1,      			 BUTTON_SPLIT_Y,  BUTTON_WHITH, BUTTON_HIGHT, " ", " ", " ", 2},
	{ BUTTON_SPLIT_X*2+BUTTON_WHITH,     BUTTON_SPLIT_Y,  BUTTON_WHITH, BUTTON_HIGHT, " ", " ", " ", 1},
	{ BUTTON_SPLIT_X*3+BUTTON_WHITH*2,   BUTTON_SPLIT_Y,  BUTTON_WHITH, BUTTON_HIGHT, " ", " ", " ", 1},
	{ BUTTON_SPLIT_X*4+BUTTON_WHITH*3,   BUTTON_SPLIT_Y,  BUTTON_WHITH, BUTTON_HIGHT, " ", " ", " ", 1},
	{ BUTTON_SPLIT_X*5+BUTTON_WHITH*4,   BUTTON_SPLIT_Y,  BUTTON_WHITH, BUTTON_HIGHT, " ", " ", " ", 1},
	{ BUTTON_SPLIT_X*6+BUTTON_WHITH*5,   BUTTON_SPLIT_Y,  BUTTON_WHITH, BUTTON_HIGHT, " ", " ", " ", 1},
	{ BUTTON_SPLIT_X*7+BUTTON_WHITH*6,   BUTTON_SPLIT_Y,  BUTTON_WHITH, BUTTON_HIGHT, " ", " ", " ", 1},
	{ BUTTON_SPLIT_X*8+BUTTON_WHITH*7,   BUTTON_SPLIT_Y,  BUTTON_WHITH, BUTTON_HIGHT, " ", " ", " ", 1},
	{ BUTTON_SPLIT_X*9+BUTTON_WHITH*8,   BUTTON_SPLIT_Y,  BUTTON_WHITH, BUTTON_HIGHT, "<", " ", " ", 1},
	{ BUTTON_SPLIT_X*10+BUTTON_WHITH*9,  BUTTON_SPLIT_Y,  BUTTON_WHITH, BUTTON_HIGHT, ">", " ", " ", 1},

	/* 第1排按钮 */
	{ BUTTON_SPLIT_X*1,      			 BUTTON_SPLIT_Y*2+BUTTON_HIGHT,  BUTTON_WHITH, BUTTON_HIGHT, "q", "Q", "1", 0},
	{ BUTTON_SPLIT_X*2+BUTTON_WHITH,     BUTTON_SPLIT_Y*2+BUTTON_HIGHT,  BUTTON_WHITH, BUTTON_HIGHT, "w", "W", "2", 0},
	{ BUTTON_SPLIT_X*3+BUTTON_WHITH*2,   BUTTON_SPLIT_Y*2+BUTTON_HIGHT,  BUTTON_WHITH, BUTTON_HIGHT, "e", "E", "3", 0},
	{ BUTTON_SPLIT_X*4+BUTTON_WHITH*3,   BUTTON_SPLIT_Y*2+BUTTON_HIGHT,  BUTTON_WHITH, BUTTON_HIGHT, "r", "R", "4", 0},
	{ BUTTON_SPLIT_X*5+BUTTON_WHITH*4,   BUTTON_SPLIT_Y*2+BUTTON_HIGHT,  BUTTON_WHITH, BUTTON_HIGHT, "t", "T", "5", 0},
	{ BUTTON_SPLIT_X*6+BUTTON_WHITH*5,   BUTTON_SPLIT_Y*2+BUTTON_HIGHT,  BUTTON_WHITH, BUTTON_HIGHT, "y", "Y", "6", 0},
	{ BUTTON_SPLIT_X*7+BUTTON_WHITH*6,   BUTTON_SPLIT_Y*2+BUTTON_HIGHT,  BUTTON_WHITH, BUTTON_HIGHT, "u", "U", "7", 0},
	{ BUTTON_SPLIT_X*8+BUTTON_WHITH*7,   BUTTON_SPLIT_Y*2+BUTTON_HIGHT,  BUTTON_WHITH, BUTTON_HIGHT, "i", "I", "8", 0},
	{ BUTTON_SPLIT_X*9+BUTTON_WHITH*8,   BUTTON_SPLIT_Y*2+BUTTON_HIGHT,  BUTTON_WHITH, BUTTON_HIGHT, "o", "O", "9", 0},
	{ BUTTON_SPLIT_X*10+BUTTON_WHITH*9,  BUTTON_SPLIT_Y*2+BUTTON_HIGHT,  BUTTON_WHITH, BUTTON_HIGHT, "p", "P", "0", 0},

	/* 第2排按钮 */
	{ 25+BUTTON_SPLIT_X*1,      		 	BUTTON_SPLIT_Y*3+BUTTON_HIGHT*2,  BUTTON_WHITH, BUTTON_HIGHT, "a", "A", "+",  0},
	{ 25+BUTTON_SPLIT_X*2+BUTTON_WHITH,     BUTTON_SPLIT_Y*3+BUTTON_HIGHT*2,  BUTTON_WHITH, BUTTON_HIGHT, "s", "S", "-", 0},
	{ 25+BUTTON_SPLIT_X*3+BUTTON_WHITH*2,   BUTTON_SPLIT_Y*3+BUTTON_HIGHT*2,  BUTTON_WHITH, BUTTON_HIGHT, "d", "D", "*", 0},
	{ 25+BUTTON_SPLIT_X*4+BUTTON_WHITH*3,   BUTTON_SPLIT_Y*3+BUTTON_HIGHT*2,  BUTTON_WHITH, BUTTON_HIGHT, "f", "F", "/", 0},
	{ 25+BUTTON_SPLIT_X*5+BUTTON_WHITH*4,   BUTTON_SPLIT_Y*3+BUTTON_HIGHT*2,  BUTTON_WHITH, BUTTON_HIGHT, "g", "G", "=", 0},
	{ 25+BUTTON_SPLIT_X*6+BUTTON_WHITH*5,   BUTTON_SPLIT_Y*3+BUTTON_HIGHT*2,  BUTTON_WHITH, BUTTON_HIGHT, "h", "H", "%", 0},
	{ 25+BUTTON_SPLIT_X*7+BUTTON_WHITH*6,   BUTTON_SPLIT_Y*3+BUTTON_HIGHT*2,  BUTTON_WHITH, BUTTON_HIGHT, "j", "J", "_", 0},
	{ 25+BUTTON_SPLIT_X*8+BUTTON_WHITH*7,   BUTTON_SPLIT_Y*3+BUTTON_HIGHT*2,  BUTTON_WHITH, BUTTON_HIGHT, "k", "K", "@", 0},
	{ 25+BUTTON_SPLIT_X*9+BUTTON_WHITH*8,   BUTTON_SPLIT_Y*3+BUTTON_HIGHT*2,  BUTTON_WHITH, BUTTON_HIGHT, "l", "L", ".", 0},

	/* 第3排按钮 */
	{ BUTTON_SPLIT_X*1,      		 	 				BUTTON_SPLIT_Y*4+BUTTON_HIGHT*3,  BUTTON_WHITH_L, 	BUTTON_HIGHT, "大小写",  "大小写", "Shift", 1},
	{ BUTTON_SPLIT_X*2+BUTTON_WHITH_L,     				BUTTON_SPLIT_Y*4+BUTTON_HIGHT*3,  BUTTON_WHITH, 	BUTTON_HIGHT, "z",    "Z", 	 ",", 0},
	{ BUTTON_SPLIT_X*3+BUTTON_WHITH_L+BUTTON_WHITH*1,   BUTTON_SPLIT_Y*4+BUTTON_HIGHT*3,  BUTTON_WHITH, 	BUTTON_HIGHT, "x",    "X",   "'", 0},
	{ BUTTON_SPLIT_X*4+BUTTON_WHITH_L+BUTTON_WHITH*2,   BUTTON_SPLIT_Y*4+BUTTON_HIGHT*3,  BUTTON_WHITH, 	BUTTON_HIGHT, "c",    "C",   "\"", 0},
	{ BUTTON_SPLIT_X*5+BUTTON_WHITH_L+BUTTON_WHITH*3,   BUTTON_SPLIT_Y*4+BUTTON_HIGHT*3,  BUTTON_WHITH, 	BUTTON_HIGHT, "v",    "V",   "(", 0},
	{ BUTTON_SPLIT_X*6+BUTTON_WHITH_L+BUTTON_WHITH*4,   BUTTON_SPLIT_Y*4+BUTTON_HIGHT*3,  BUTTON_WHITH, 	BUTTON_HIGHT, "b",    "B",   ")", 0},
	{ BUTTON_SPLIT_X*7+BUTTON_WHITH_L+BUTTON_WHITH*5,   BUTTON_SPLIT_Y*4+BUTTON_HIGHT*3,  BUTTON_WHITH, 	BUTTON_HIGHT, "n",    "N",   "!", 0},
	{ BUTTON_SPLIT_X*8+BUTTON_WHITH_L+BUTTON_WHITH*6,   BUTTON_SPLIT_Y*4+BUTTON_HIGHT*3,  BUTTON_WHITH, 	BUTTON_HIGHT, "m",    "M",   "?", 0},
	{ BUTTON_SPLIT_X*9+BUTTON_WHITH_L+BUTTON_WHITH*7,   BUTTON_SPLIT_Y*4+BUTTON_HIGHT*3,  BUTTON_WHITH_L, 	BUTTON_HIGHT, "删除",  "删除", "Del", 1},

	/* 第4排按钮 */
	{ BUTTON_SPLIT_X*1,      		 	 								BUTTON_SPLIT_Y*5+BUTTON_HIGHT*4,  BUTTON_WHITH_L,  	  BUTTON_HIGHT, "123",  "ABC",    "123", 1},
	{ BUTTON_SPLIT_X*2+BUTTON_WHITH_L,   								BUTTON_SPLIT_Y*5+BUTTON_HIGHT*4,  BUTTON_WHITH_BLACK, BUTTON_HIGHT, " ",     " ",      " ",  0},
	{ BUTTON_SPLIT_X*3+BUTTON_WHITH_L+BUTTON_WHITH_BLACK,   			BUTTON_SPLIT_Y*5+BUTTON_HIGHT*4,  BUTTON_WHITH, 	  BUTTON_HIGHT, "中/En", "中/En",    "En/中", 1},
	{ BUTTON_SPLIT_X*4+BUTTON_WHITH+BUTTON_WHITH_L+BUTTON_WHITH_BLACK,  BUTTON_SPLIT_Y*5+BUTTON_HIGHT*4,  BUTTON_WHITH_L, 	  BUTTON_HIGHT, "完成", "完成", "Enter", 1}

};




/*
*	初始化，索引汉字状态
*/
static void Clear_Keyboard_HZ_Status(__IO KeyboardStatus_t *ptKeyBoardStatus)
{
    memset((void*)ptKeyBoardStatus->caInputBuffer, 0, INPUT_CHAR_MAX_LEN);
    ptKeyBoardStatus->cInputIndex		= 0;		   
	ptKeyBoardStatus->pcHzBuffer		= 0;	       
	ptKeyBoardStatus->ulHzNum			= 0; 		    
    ptKeyBoardStatus->ulPageNum 		= 0;            
    ptKeyBoardStatus->ulPageIndex       = 0;   
}



/*
*	初始化键盘状态
*/
static void KeyboardStatus_Init(__IO KeyboardStatus_t *ptKeyBoardStatus)
{
    extern __IO MachRunPara_s MachRunPara;
	extern  MachInfo_s MachInfo;
	
	//默认值
	ptKeyBoardStatus->eCE = EN_ENGLISH;
	ptKeyBoardStatus->eChar_SymbolNum = EN_CHAR;
	
	switch(MachRunPara.keyboardInitJM){
		//显示在汉字输入界面
		case KEYBOARD_HZ:
		{
			if(MachInfo.systemSet.eLanguage == LANGUAGE_CHINESE){
				ptKeyBoardStatus->eCE 				= EN_CHINESE;
			}else{
				ptKeyBoardStatus->eCE 				= EN_ENGLISH;
			}
			
			ptKeyBoardStatus->eChar_SymbolNum = EN_CHAR;
		}break;
		
		//显示在字母输入界面
		case KEYBOARD_CHAR:
		{
			ptKeyBoardStatus->eCE = EN_ENGLISH;
			ptKeyBoardStatus->eChar_SymbolNum = EN_CHAR;
		}break;
		
		//显示在数字输入界面
		case KEYBOARD_NUM:
		{
			ptKeyBoardStatus->eCE = EN_ENGLISH;
			ptKeyBoardStatus->eChar_SymbolNum = EN_SYMBOL_NUM;
		}break;
		default :break;
	}
    
    ptKeyBoardStatus->eBigSmall 		= EN_SMALL;
	memset((void*)ptKeyBoardStatus->caInputBuffer, 0, INPUT_CHAR_MAX_LEN);
	ptKeyBoardStatus->cInputIndex		= 0;		    
	ptKeyBoardStatus->pcHzBuffer		= 0;	        
	ptKeyBoardStatus->ulHzNum			= 0; 		    
    ptKeyBoardStatus->ulPageNum 		= 0;           
    ptKeyBoardStatus->ulPageIndex       = 0;          
    ptKeyBoardStatus->HzHideFlag        = EN_HZ_HIDE;  
    
}





/*
*   汉字提示按键皮肤回调
*/
static int _HZ_Button_DrawSkin(const WIDGET_ITEM_DRAW_INFO* pDrawItemInfo)
{
	GUI_RECT Rect;
    WM_GetClientRect(&Rect);
	
	switch(pDrawItemInfo->Cmd){
		case WIDGET_ITEM_DRAW_BACKGROUND:
        {
            GUI_SetColor(COLOR_KEYPAD1);
            GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y0);
        }
		break;
		default:
			return BUTTON_SKIN_FLEX(pDrawItemInfo);
	}
	return 0;
}



/*
*	键盘绘制函数，控制按键位为中文，其他字符按键设置为英文
*/
static void Keyboard_Msg_Init(WM_MESSAGE *pMsg)
{
	WM_HWIN    hWin;
	WM_HWIN    hButton;
    
 #if USE_EXTERN_FONT
    extern GUI_FONT HZ_SONGTI_16;
#else
	extern GUI_CONST_STORAGE GUI_FONT GUI_F16_GB2312;
#endif   

	int i = 0;
	hWin = pMsg->hWin;
	
    //索引中文显示，只有在中文输入的时候显示
    for (i = BUTTON_DATA_InputChar; i < BUTTON_DATA_qQ; i++) 
    {
        if(BUTTON_DATA_InputChar == i)
        {
            //第一个，其实是TEXT，只是采用Button的定义数据格式，为输入信息
            hButton = TEXT_CreateEx(_aButtonData[i].xPos, _aButtonData[i].yPos, _aButtonData[i].xSize, _aButtonData[i].ySize, hWin, WM_CF_SHOW, 0, KEYBOARD_ID+i, NULL);
			TEXT_SetTextAlign(hButton, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetFont(hButton, &GUI_Font20_ASCII); //
            TEXT_SetTextColor(hButton, GUI_RED);
			TEXT_SetBkColor(hButton, GUI_LIGHTBLUE);
        }else{
            hButton = BUTTON_CreateEx(_aButtonData[i].xPos, _aButtonData[i].yPos, _aButtonData[i].xSize, _aButtonData[i].ySize, hWin, WM_CF_SHOW, 0, KEYBOARD_ID+i);
            BUTTON_SetSkin(hButton, _HZ_Button_DrawSkin);
            
            BUTTON_SetFont(hButton, &HZ_SONGTI_16); //中文
            BUTTON_SetText(hButton, _aButtonData[i].acCharSmall);
            BUTTON_SetBkColor(hButton, BUTTON_CI_PRESSED, COLOR_BORDER);
            BUTTON_SetBkColor(hButton, BUTTON_CI_UNPRESSED, COLOR_BORDER);
            BUTTON_SetBkColor(hButton, BUTTON_CI_DISABLED, COLOR_BORDER);
            //
            BUTTON_SetTextColor(hButton, BUTTON_CI_PRESSED, GUI_RED);
            BUTTON_SetTextColor(hButton, BUTTON_CI_UNPRESSED, GUI_RED);
            
            BUTTON_SetFrameColor(hButton, COLOR_BORDER);
            //
            BUTTON_SetFocussable(hButton, 0);	//失能，聚焦功能
        }
        WM_HideWindow(hButton);
    }
    g_tKeyBoardStats.HzHideFlag = EN_HZ_HIDE_KEEP; //隐藏
    
    //键盘
    BUTTON_SKINFLEX_PROPS Props;
	for (i = BUTTON_DATA_qQ; i < BUTTON_DATA_END; i++) 
	{
		hButton = BUTTON_CreateEx(_aButtonData[i].xPos, _aButtonData[i].yPos, _aButtonData[i].xSize, _aButtonData[i].ySize, hWin, WM_CF_SHOW, 0, KEYBOARD_ID+i);
//      BUTTON_SetSkin(hButton, _Button_DrawSkin);        
         //
         BUTTON_GetSkinFlexProps(&Props, BUTTON_SKINFLEX_PI_PRESSED);
         Props.aColorFrame[0] = BUTTON_PRESSED_COLOR;
         Props.aColorFrame[1] = BUTTON_PRESSED_COLOR;
         Props.aColorFrame[2] = BUTTON_PRESSED_COLOR;
         Props.aColorLower[0] = BUTTON_PRESSED_COLOR;
         Props.aColorLower[1] = BUTTON_PRESSED_COLOR;
         Props.aColorUpper[0] = BUTTON_PRESSED_COLOR;
         Props.aColorUpper[1] = BUTTON_PRESSED_COLOR;
         Props.Radius = 3;
         BUTTON_SetSkinFlexProps(&Props, BUTTON_SKINFLEX_PI_PRESSED); 
         //
         BUTTON_GetSkinFlexProps(&Props, BUTTON_SKINFLEX_PI_ENABLED);
         Props.aColorFrame[0] = BUTTON_ENABLE_COLOR;
         Props.aColorFrame[1] = BUTTON_ENABLE_COLOR;
         Props.aColorFrame[2] = BUTTON_ENABLE_COLOR;
         Props.aColorLower[0] = BUTTON_ENABLE_COLOR;
         Props.aColorLower[1] = BUTTON_ENABLE_COLOR;
         Props.aColorUpper[0] = BUTTON_ENABLE_COLOR;
         Props.aColorUpper[1] = BUTTON_ENABLE_COLOR;
         Props.Radius = 3;
         BUTTON_SetSkinFlexProps(&Props, BUTTON_SKINFLEX_PI_ENABLED); 
         WM_InvalidateWindow(hButton);
        
		//设置按键字库，
		if(1 == _aButtonData[i].cControl) //控制键
		{
			BUTTON_SetFont(hButton, &HZ_SONGTI_16); //中文
		}else{	//通用字符
			BUTTON_SetFont(hButton, &GUI_Font24_ASCII);
		}
	
		//设置按键显示内容
		if(EN_SYMBOL_NUM == g_tKeyBoardStats.eChar_SymbolNum)//符号数字
		{
			BUTTON_SetText(hButton, _aButtonData[i].acChar123);       			
		}else{ //字母
			if(EN_SMALL == g_tKeyBoardStats.eBigSmall) //小写
			{
				BUTTON_SetText(hButton, _aButtonData[i].acCharSmall);
			}else{
				BUTTON_SetText(hButton, _aButtonData[i].acCharBig);	//大写
			}
		}
        
        //
        if(BUTTON_DATA_Shift == i)
        {
			if(g_tKeyBoardStats.eCE == EN_CHINESE)
            {
                BUTTON_SetText(hButton, _aButtonData[i].acCharSmall); 
            }else{
                BUTTON_SetText(hButton, _aButtonData[i].acChar123); 
            }
			
			//在英文模式下，强制将中英文切换键失效
			if(MachInfo.systemSet.eLanguage == LANGUAGE_ENGLISH){
				WM_DisableWindow(hButton);
				BUTTON_SetText(hButton, "En");
			}
        }
		
		if(BUTTON_DATA_CapsLock == i)
        {
            if(g_tKeyBoardStats.eCE == EN_CHINESE)
            {
                BUTTON_SetText(hButton, _aButtonData[i].acCharSmall); 
            }else{
                BUTTON_SetText(hButton, _aButtonData[i].acChar123); 
            }
        }
		
		if(BUTTON_DATA_Del == i)
        {
            if(g_tKeyBoardStats.eCE == EN_CHINESE)
            {
                BUTTON_SetText(hButton, _aButtonData[i].acCharSmall); 
            }else{
                BUTTON_SetText(hButton, _aButtonData[i].acChar123); 
            }
        }
		
		if(BUTTON_DATA_Enter == i)
        {
            if(g_tKeyBoardStats.eCE == EN_CHINESE)
            {
                BUTTON_SetText(hButton, _aButtonData[i].acCharSmall); 
            }else{
                BUTTON_SetText(hButton, _aButtonData[i].acChar123); 
            }
        }
        
		BUTTON_SetFocussable(hButton, 0);	//失能，聚焦功能
	}
}


/*
*   更新键盘显示字符
*/
static void Keyboard_Update_Char(WM_MESSAGE *pMsg)
{
    WM_HWIN hButton;
    int i = 0;

    //
    for(i = BUTTON_DATA_qQ; i < BUTTON_DATA_END; i++)
    {
        hButton = WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i);
        //设置字符按键显示内容
        if(0 == _aButtonData[i].cControl)
        {
            if(EN_SYMBOL_NUM == g_tKeyBoardStats.eChar_SymbolNum)//符号数字
            {
                BUTTON_SetText(hButton, _aButtonData[i].acChar123);
                
            }else{ //字母
                if(EN_SMALL == g_tKeyBoardStats.eBigSmall) //小写
                {
                    BUTTON_SetText(hButton, _aButtonData[i].acCharSmall);
                }else{
                    BUTTON_SetText(hButton, _aButtonData[i].acCharBig);	//大写
                }
            }
        }
    }
}



/*
*   显示的BUTTON
*/
static void Show_HZ_Prompt(WM_MESSAGE *pMsg)
{
    int i = 0;
    
    //显示索引中文
    if(g_tKeyBoardStats.HzHideFlag != EN_HZ_SHOW_KEEP)
    {
        for (i = BUTTON_DATA_InputChar; i < BUTTON_DATA_qQ; i++) 
        {
            if(BUTTON_DATA_InputChar == i)
            {
                TEXT_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i), (char*)g_tKeyBoardStats.caInputBuffer);
            }
            WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i));
        }
        g_tKeyBoardStats.HzHideFlag = EN_HZ_SHOW_KEEP; //
    }
    
    //处理翻页
    if(g_tKeyBoardStats.ulPageIndex == 0)//第一页汉字
    {
        WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, BUTTON_DATA_LAST_PAGE));
        
    }else if(g_tKeyBoardStats.ulPageIndex == g_tKeyBoardStats.ulPageNum - 1){ //最后一行汉字
        
        WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, BUTTON_DATA_NEXT_PAGE));
    }else{ //中间行
        WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, BUTTON_DATA_LAST_PAGE));
        WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, BUTTON_DATA_NEXT_PAGE));
    }      
}



/*
*   隐藏提示汉字，并清除内容
*/
static void Hide_HZ_Prompt(WM_MESSAGE *pMsg)
{
     int i = 0;
    
    //隐藏索引中文
    if(g_tKeyBoardStats.HzHideFlag != EN_HZ_HIDE_KEEP)
    {
        for (i = BUTTON_DATA_InputChar; i < BUTTON_DATA_qQ; i++) 
        {  
            WM_HideWindow(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i));
        }
        g_tKeyBoardStats.HzHideFlag = EN_HZ_HIDE_KEEP; //
    }   
}



/*
*   动态刷新索引到的汉字,并显示
*/
static void Keyboard_Update_Prompt_HZ(WM_MESSAGE *pMsg)
{
#if USE_EXTERN_FONT
    extern GUI_FONT HZ_SONGTI_16;
#else
	extern GUI_CONST_STORAGE GUI_FONT GUI_F16_GB2312;
#endif
    
    unsigned int i = 0, ulIndex = 0, ulNum = 0;
    char buffer[5] = {0};
    
    //显示汉字BUTTON控件
    Show_HZ_Prompt(pMsg);    
    
    //显示索引到的汉字内容
    if(g_tKeyBoardStats.pcHzBuffer != '\0')
	{
		printf("num:%d, HZ=%s\r\n", g_tKeyBoardStats.ulHzNum, g_tKeyBoardStats.pcHzBuffer);
        
        //当前页在所有汉字中的标号
        ulIndex  = (g_tKeyBoardStats.ulPageIndex)*PROMPT_HZ_NUM_PER_LINE*3; //一个汉字三个字节
        
        //当前行显示汉字的个数
        if(g_tKeyBoardStats.ulPageIndex == 0 && 1 == g_tKeyBoardStats.ulPageNum)
        {
            //只有一行(第一行)
            ulNum = g_tKeyBoardStats.ulHzNum;
        }else if(g_tKeyBoardStats.ulPageIndex < g_tKeyBoardStats.ulPageNum - 1){
            //前面几行行（多行）  
            ulNum = PROMPT_HZ_NUM_PER_LINE;
        }else if(g_tKeyBoardStats.ulPageIndex == g_tKeyBoardStats.ulPageNum - 1){
            //最后一行（多行）
            ulNum = g_tKeyBoardStats.ulHzNum%PROMPT_HZ_NUM_PER_LINE;  
        }
        
		//显示一行索引到的汉字
		for(i = BUTTON_DATA_HZ_1; i <= ulNum; i++)
		{  
			memset(buffer,0, 5);
			buffer[0] = *(g_tKeyBoardStats.pcHzBuffer + ulIndex + (i-1)*3 + 0);
			buffer[1] = *(g_tKeyBoardStats.pcHzBuffer + ulIndex + (i-1)*3 + 1);
			buffer[2] = *(g_tKeyBoardStats.pcHzBuffer + ulIndex + (i-1)*3 + 2);

			printf("i=%d,b0=%X,b1=%X,b2=%X,buffer=%s\r\n", i, buffer[0], buffer[1], buffer[2],buffer);
			BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i), &HZ_SONGTI_16);
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i), buffer);
		}
		
		//last page and num less than PROMPT_HZ_NUM_PER_LINE, clear less buttion to blank
		if(ulNum != PROMPT_HZ_NUM_PER_LINE)
		{
			for(i = ulNum + 1; i <= BUTTON_DATA_HZ_7; i++)
			{
				BUTTON_SetFont(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i), &HZ_SONGTI_16);
				BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i), "");
			}
		}
		
	}else{
		for (i = BUTTON_DATA_HZ_1; i <= BUTTON_DATA_HZ_7; i++) 
        {  
            BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i), " ");
        }
	}
}



/*
*   根据输入，动态刷新索引到的汉字,并显示
*/
static void Keyboard_SearchAndShow_Prompt_HZ(WM_MESSAGE *pMsg)
{
#if USE_EXTERN_FONT
    extern GUI_FONT HZ_SONGTI_16;
#else
	extern GUI_CONST_STORAGE GUI_FONT GUI_F16_GB2312;
#endif
    //WM_HWIN hButton;
    
    //显示输入的拼音
	printf("Input:%s\r\n", g_tKeyBoardStats.caInputBuffer);
    
    //索引汉字，并更新相关信息
    if(g_tKeyBoardStats.caInputBuffer[0] > 0)
    {
        g_tKeyBoardStats.pcHzBuffer  = PYSearch((unsigned char*)g_tKeyBoardStats.caInputBuffer, (unsigned int*)&g_tKeyBoardStats.ulHzNum); //索引中文
        g_tKeyBoardStats.ulPageIndex = 0; //start form 0
        g_tKeyBoardStats.ulPageNum   = (g_tKeyBoardStats.ulHzNum + PROMPT_HZ_NUM_PER_LINE - 1)/PROMPT_HZ_NUM_PER_LINE;
    
        //更新显示
        Keyboard_Update_Prompt_HZ(pMsg);
    }
    //
    GUI_SetColor(GUI_RED);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+BUTTON_DATA_InputChar), (char*)g_tKeyBoardStats.caInputBuffer);
}


/*
* 发送键盘输入数据到当前活动界面
*/
static void Send_Keyboard_Msg(const char *pcResult, MsgType_e eType) // cFlag: 0:字符，数字，符号等，1：汉字
{    
    //
    WM_MESSAGE		Msg;
    
    //
    memset((char*)g_caKeyboard_Buffer, 0, KEYBOARD_RESULT_BUFFER_LEN);
    if(MSG_TYPE_CHINESE == eType)
    {
        g_caKeyboard_Buffer[0] = pcResult[0];   //汉字，三个字节
        g_caKeyboard_Buffer[1] = pcResult[1];
        g_caKeyboard_Buffer[2] = pcResult[2];
        
    }else if(MSG_TYPE_OTHERS == eType){
        g_caKeyboard_Buffer[0] = pcResult[0]; //字符，数字，符号，一个字节
    }else{
        LOG_Error("Keyboard Msg Type(%d) Error", eType);
        return;
    }
    //
    Msg.MsgId  = WM_KEYBOARD_UPDATE;
    Msg.hWin   = g_hActiveWin;
    Msg.Data.p = (char*)g_caKeyboard_Buffer;
    WM_SendMessage(g_hActiveWin, &Msg);
}

/*
*********************************************************************************************************
*	函 数 名: Keyboard_Callback
*	功能说明: 回调函数
*	形    参：pMsg  指针参数
*	返 回 值: 无
*********************************************************************************************************
*/
static void Keyboard_Callback(WM_MESSAGE * pMsg) 
{
#if USE_EXTERN_FONT
    extern GUI_FONT HZ_SONGTI_16;
#else
	extern GUI_CONST_STORAGE GUI_FONT GUI_F16_GB2312;
#endif

	WM_HWIN         hWin;
	int             Id;
	int             NCode;
	unsigned int    xSize;
	unsigned int    ySize;
	unsigned int    i;
    unsigned int    ulIndex;
    char            buffer[KEYBOARD_RESULT_BUFFER_LEN] = {0};
    WM_MESSAGE      Msg;
	
	hWin = pMsg->hWin;
	switch(pMsg->MsgId)
	{
		case WM_CREATE: //初始化
		{
			Keyboard_Msg_Init(pMsg);
		}
		break;
		case WM_PAINT: //重绘
		{
			Keyboard_Update_Char(pMsg);
			xSize = WM_GetWindowSizeX(hWin);
			ySize = WM_GetWindowSizeY(hWin);
			GUI_SetColor(GUI_WHITE);
            GUI_DrawRect(0, 0, xSize, ySize);
            GUI_DrawGradientV(1, 1, xSize - 2, ySize - 2, COLOR_KEYPAD0, COLOR_KEYPAD0);
		}
		break;
		case WM_NOTIFY_PARENT: //处理子控件相应
		{
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (NCode) 
			{
				case WM_NOTIFICATION_RELEASED: //按键释放消息
				{
					i = Id - KEYBOARD_ID;
					//通用字符按键
					if(_aButtonData[i].cControl == 0) 
					{
						if(EN_ENGLISH == g_tKeyBoardStats.eCE) //英文
						{
							if(EN_SYMBOL_NUM == g_tKeyBoardStats.eChar_SymbolNum)//符号数字
							{
                                Send_Keyboard_Msg(_aButtonData[i].acChar123, MSG_TYPE_OTHERS); //发送输入字符信息，给当前激活界面
								
							}else{ //字母
								if(EN_SMALL == g_tKeyBoardStats.eBigSmall) //小写
								{
                                    Send_Keyboard_Msg(_aButtonData[i].acCharSmall, MSG_TYPE_OTHERS); 
								}else{
                                    Send_Keyboard_Msg(_aButtonData[i].acCharBig, MSG_TYPE_OTHERS);
								}
							}
						}else if(EN_CHINESE == g_tKeyBoardStats.eCE){ //中文
							if(EN_SYMBOL_NUM == g_tKeyBoardStats.eChar_SymbolNum)//符号数字
							{
                                Send_Keyboard_Msg(_aButtonData[i].acChar123, MSG_TYPE_OTHERS); //发送输入字符信息，给当前激活界面
								
							}else{ //字母
								if(EN_SMALL == g_tKeyBoardStats.eBigSmall) //小写（只有在小写，中文时，才会启动拼音索引）
								{
                                    //中文索引汉字，并显示
									if(g_tKeyBoardStats.cInputIndex >= INPUT_CHAR_MAX_LEN)
									{
										g_tKeyBoardStats.cInputIndex = INPUT_CHAR_MAX_LEN - 1;
									}
                                    g_tKeyBoardStats.caInputBuffer[g_tKeyBoardStats.cInputIndex++] = _aButtonData[i].acCharSmall[0];
                                    Keyboard_SearchAndShow_Prompt_HZ(pMsg);
								}else{ //大写
                                    Send_Keyboard_Msg(_aButtonData[i].acCharBig, MSG_TYPE_OTHERS);
								}
							}
						}
					}else if(_aButtonData[i].cControl == 1){ 
                        //功能按键
						switch(i)
						{
							case BUTTON_DATA_Del:	//删除输入的字符或汉字
							{
                                if(EN_CHINESE == g_tKeyBoardStats.eCE && EN_SMALL == g_tKeyBoardStats.eBigSmall){ //中文,小写 =》拼音输入
                                    if(g_tKeyBoardStats.cInputIndex > 0)
                                    {
                                        g_tKeyBoardStats.caInputBuffer[--g_tKeyBoardStats.cInputIndex] = 0;
                                        //更新显示索引到的汉字
                                        Keyboard_SearchAndShow_Prompt_HZ(pMsg);
                                        //
                                        if(g_tKeyBoardStats.cInputIndex == 0) //已没有输入字符
                                        {
                                            //隐藏中文提示
                                            Hide_HZ_Prompt(pMsg);
                                            Clear_Keyboard_HZ_Status(&g_tKeyBoardStats);
                                        }
                                    }else{
                                        GUI_StoreKeyMsg(GUI_KEY_BACKSPACE, 1);
                                    }
                                }else{
                                    GUI_StoreKeyMsg(GUI_KEY_BACKSPACE, 1);
                                }	
                                //发送消息给当前活动界面，输入键盘关闭
                                Msg.MsgId  = WM_KEYBOARD_DELETE;
                                Msg.hWin   = g_hActiveWin;
                                WM_SendMessage(g_hActiveWin, &Msg);                                
							}
							break;
                            case BUTTON_DATA_CapsLock: //大小写切换
							{
								if(EN_BIG == g_tKeyBoardStats.eBigSmall)
								{
									g_tKeyBoardStats.eBigSmall = EN_SMALL;		
									
								}else{
									g_tKeyBoardStats.eBigSmall = EN_BIG;
								}
                                
                                //当发送切换时，清除输入buffer
                                g_tKeyBoardStats.cInputIndex = 0;
								memset((void*)g_tKeyBoardStats.caInputBuffer, 0, INPUT_CHAR_MAX_LEN);
                                //隐藏中文提示
                                Hide_HZ_Prompt(pMsg);
                                Clear_Keyboard_HZ_Status(&g_tKeyBoardStats);
							}
							break;
							case BUTTON_DATA_Shift:	//中英文切换
							{
								if(EN_CHINESE == g_tKeyBoardStats.eCE)
								{
									g_tKeyBoardStats.eCE = EN_ENGLISH;
                                    //刷新中英文显示按键, 显示为：英/中
                                    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i), _aButtonData[i].acChar123);
                                    //对应的提示字符，需改为英文
                                    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+BUTTON_DATA_Del), _aButtonData[BUTTON_DATA_Del].acChar123);
                                    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+BUTTON_DATA_CapsLock), _aButtonData[BUTTON_DATA_CapsLock].acChar123);
                                    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+BUTTON_DATA_Enter), _aButtonData[BUTTON_DATA_Enter].acChar123);
								}else{
									g_tKeyBoardStats.eCE = EN_CHINESE;
                                    g_tKeyBoardStats.eBigSmall = EN_SMALL; /////中文模式下,字母模式下，需为：小写 
									if(EN_CHAR == g_tKeyBoardStats.eChar_SymbolNum)
									{
										g_tKeyBoardStats.eBigSmall = EN_SMALL;
									}
                                    //刷新中英文显示按键, 显示为：中/英
                                    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i), _aButtonData[i].acCharSmall);
                                    //对应的提示字符，需改为中文
                                    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+BUTTON_DATA_Del), _aButtonData[BUTTON_DATA_Del].acCharSmall);
                                    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+BUTTON_DATA_CapsLock), _aButtonData[BUTTON_DATA_CapsLock].acCharSmall);
                                    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+BUTTON_DATA_Enter), _aButtonData[BUTTON_DATA_Enter].acCharSmall);
								}   
                                
                                //当发送切换时，清除输入buffer
                                g_tKeyBoardStats.cInputIndex = 0;
								memset((void*)g_tKeyBoardStats.caInputBuffer, 0, INPUT_CHAR_MAX_LEN);                                
                                //隐藏中文提示
                                Hide_HZ_Prompt(pMsg);
                                Clear_Keyboard_HZ_Status(&g_tKeyBoardStats);
							}	
							break;
							case BUTTON_DATA_ABC123: //字母，符号数字切换
							{
								if(EN_CHAR == g_tKeyBoardStats.eChar_SymbolNum) 
								{
                                    //切换为：数值符号
									g_tKeyBoardStats.eChar_SymbolNum = EN_SYMBOL_NUM;
									//刷新字符数字显示按键, 显示为：123
                                    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i), _aButtonData[i].acCharBig);//acChar123
								}else{ 
                                    //切换为：ASCII字符                                  
									g_tKeyBoardStats.eChar_SymbolNum = EN_CHAR;
									//刷新字符数字显示按键, 显示为：ABC
                                    BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, KEYBOARD_ID+i), _aButtonData[i].acChar123);//acCharSmall
								}
                                
                                //当发送切换时，清除输入buffer
                                g_tKeyBoardStats.cInputIndex = 0;
								memset((void*)g_tKeyBoardStats.caInputBuffer, 0, INPUT_CHAR_MAX_LEN);
                                //隐藏中文提示
                                Hide_HZ_Prompt(pMsg);
                                Clear_Keyboard_HZ_Status(&g_tKeyBoardStats);
							}
                            break;
                            case BUTTON_DATA_LAST_PAGE: // 提示汉字，上一页
							{
                                if(g_tKeyBoardStats.ulPageIndex > 0)
                                {
                                    g_tKeyBoardStats.ulPageIndex--;
                                    Keyboard_Update_Prompt_HZ(pMsg);
                                }
							}
							break;	
							case BUTTON_DATA_NEXT_PAGE: // // 提示汉字，下一页
							{
                                if(g_tKeyBoardStats.ulPageIndex < g_tKeyBoardStats.ulPageNum - 1)
                                {
                                    g_tKeyBoardStats.ulPageIndex++;
                                    Keyboard_Update_Prompt_HZ(pMsg);
                                }	
							}
							break;
							case BUTTON_DATA_HZ_1: //选中所需要输入的的提示汉字
                            case BUTTON_DATA_HZ_2:
                            case BUTTON_DATA_HZ_3:
                            case BUTTON_DATA_HZ_4:
                            case BUTTON_DATA_HZ_5:
                            case BUTTON_DATA_HZ_6:
                            case BUTTON_DATA_HZ_7:
							{ 
                                //根据提示选择汉字，这里响应用户选择的汉字，做处理
                                if(g_tKeyBoardStats.ulPageIndex <= g_tKeyBoardStats.ulPageNum - 1)
                                {
                                    memset(buffer, 0, 5);
                                    
                                    //当时汉字不足一行或最后一行，当剩余汉字少于一行，且点击“空”的汉字提示按钮，应做无反应响应
                                    if(g_tKeyBoardStats.ulPageIndex == g_tKeyBoardStats.ulPageNum - 1)
                                    {
                                        if(i > (g_tKeyBoardStats.ulHzNum%PROMPT_HZ_NUM_PER_LINE) && g_tKeyBoardStats.ulHzNum%PROMPT_HZ_NUM_PER_LINE != 0)
                                        {
                                            //退出
                                            break;
                                        }
                                    }   
                                    //
                                    ulIndex = g_tKeyBoardStats.ulPageIndex*PROMPT_HZ_NUM_PER_LINE*3 + (i-1)*3 ;  //采用utf8-BOM编码，一个汉字三个字节
                                    buffer[0] = g_tKeyBoardStats.pcHzBuffer[ulIndex++];
                                    buffer[1] = g_tKeyBoardStats.pcHzBuffer[ulIndex++];
                                    buffer[2] = g_tKeyBoardStats.pcHzBuffer[ulIndex++];    
                                    
                                    //发送HZ到界面输入框等
                                    Send_Keyboard_Msg(buffer, MSG_TYPE_CHINESE); //把选择的汉字编码发送给当前活动界面，界面接受消息后，会有相应的处理
                                    printf("HZ Select: i=%d, 0=%X, 1=%X, 2=%X,%s\r\n", i, buffer[0], buffer[1], buffer[2], buffer);                                                                  
                                }
//                                
                                //清除输入缓存，隐藏汉字BTN
                                g_tKeyBoardStats.cInputIndex = 0;
								memset((void*)g_tKeyBoardStats.caInputBuffer, 0, INPUT_CHAR_MAX_LEN);                                
                                //隐藏中文提示, 重置状态
                                Hide_HZ_Prompt(pMsg);
                                Clear_Keyboard_HZ_Status(&g_tKeyBoardStats);
							}    
							break;							
							case BUTTON_DATA_Enter: //完成
							{ 
                                //发送消息给当前活动界面，输入键盘关闭
                                Msg.MsgId  = WM_KEYBOARD_END;
                                Msg.hWin   = g_hActiveWin;
                                WM_SendMessage(g_hActiveWin, &Msg);
                                //关闭键盘
								GUI_EndDialog(pMsg->hWin, 0);
                                gs_hKeyBoardWin = 0xFFFFFFFF;
                                
							}
							break;							
						}					
					}
				}
				break;
			}
		}
		break;
	}
}





//
WM_HWIN CreateKeyboard_Window(void);
WM_HWIN CreateKeyboard_Window(void) {

	KeyboardStatus_Init(&g_tKeyBoardStats);

    if(0xFFFFFFFF == gs_hKeyBoardWin)
    {
        //当多次调用键盘时，只创建一次
        gs_hKeyBoardWin = WM_CreateWindowAsChild(0, 260, 800, 220, WM_HBKWIN, WM_CF_SHOW | WM_CF_STAYONTOP, Keyboard_Callback, 0);
    }else{
		GUI_EndDialog(gs_hKeyBoardWin,0);
		gs_hKeyBoardWin = WM_CreateWindowAsChild(0, 260, 800, 220, WM_HBKWIN, WM_CF_SHOW | WM_CF_STAYONTOP, Keyboard_Callback, 0);
	}
	return gs_hKeyBoardWin;
}








