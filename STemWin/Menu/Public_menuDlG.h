#ifndef __PUBLIC_MENUDLG_H__
#define __PUBLIC_MENUDLG_H__

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "WM.h"
#include "FRAMEWIN.h"
#include "BUTTON.h"
#include "PROGBAR.h"

#include "msg_def.h"
#include "cx_menu.h"
#include "ui_msg.h"
#include "cmsis_os2.h"
#include "Keyboard.h"
#include "ui_files.h"
#include "parameter.h"
#include "CRC16.h"
//#define GUI_SIMULATION        



extern __IO WM_HWIN g_hActiveWin;  //当前活动窗口句柄


/********************** 字体 ***************************/
extern GUI_FONT SIF_HZ12_SONGTI; 
extern GUI_FONT SIF_HZ16_SONGTI; 
extern GUI_FONT SIF_HZ24_SONGTI; 

extern GUI_CONST_STORAGE GUI_FONT GUI_FontHZ_SongTi_24_4A;
//extern GUI_CONST_STORAGE GUI_FONT GUI_FontHZ_SongTi_12_4A;

#define HZ_SONGTI_12    SIF_HZ12_SONGTI 
#define HZ_SONGTI_16    SIF_HZ16_SONGTI 
#define HZ_SONGTI_24    GUI_FontHZ_SongTi_24_4A 

/********************** 界面ID ***************************/
//menu 
#define GUI_MENU_ID                       (GUI_ID_USER + 0x010)
//login
#define GUI_LOGIN_ID                      (GUI_ID_USER + 0x030)
#define GUI_LOGIN_SELFCHECK_ID            (GUI_ID_USER + 0x040)
//analysis
#define GUI_WINDOW_ANALYSIS_ID            (GUI_ID_USER + 0x050)
#define GUI_WINDOW_NEXT_SAMPLE_ID         (GUI_ID_USER + 0x060)
#define GUI_PATIENT_INFO_ID               (GUI_ID_USER + 0x070)
#define GUI_WINDOW_RE_COUNT_PROMPT        (GUI_ID_USER + 0x080)
#define GUI_WINDOW_OperateGuide           (GUI_ID_USER + 0x090)
#define GUI_WINDOW_ERROR_PROMPT           (GUI_ID_USER + 0x0A0)
#define GUI_ERRORINFO_ID                  (GUI_ID_USER + 0x0B0) //
#define GUI_WINDOW_COUNTING_PROMPT        (GUI_ID_USER + 0x0C0) //样本正在分析中
#define GUI_WINDOW_COUNT_FAIL_ID          (GUI_ID_USER + 0x0D0) //计数失败提示界面
#define GUI_WINDOW_N_SEC_OUTIN_IN_ID      (GUI_ID_USER + 0x0E0) //N秒提示自动进仓
#define GUI_WINDOW_PRINTER_ERROR_PORMPT   (GUI_ID_USER + 0x0F0) //打印错误提示界面
#define GUI_WINDOW_CONFIRMT_TAKEOUT_CELL  (GUI_ID_USER + 0x100) //请确认检测模块是否取出界面
#define GUI_WINDOW_SELECT_YES_NO_ID       (GUI_ID_USER + 0x110) //yes or no 选择提示界面
#define PAGE_ID_INPUT_USER_NUM		       (GUI_ID_USER + 0x120) //输入患者编号界面
#define PAGE_ID_PATIENT_LIST		       (GUI_ID_USER + 0x130) //下拉患者列表界面

//list review 
#define GUI_WINDOW_LIST_REVIEW_ID         (GUI_ID_USER + 0x140)
#define GUI_WINDOW_RESULT_INFO_ID         (GUI_ID_USER + 0x150)   //列表回顾中结果详情界面ID
#define GUI_EDIT_RESULT_ID                (GUI_ID_USER + 0x160)
#define ID_TEXT_RESULT_INFO_WBC_PROMPT    (GUI_WINDOW_RESULT_INFO_ID + 0x01) 
#define ID_TEXT_RESULT_INFO_HGB_PROMPT    (GUI_WINDOW_RESULT_INFO_ID + 0x02)
#define ID_BUTTON_RESULT_INFO_WBCHGB      (GUI_WINDOW_RESULT_INFO_ID + 0x03)
//set
#define CONFIG_MAIN_PAGE_ID               (GUI_ID_USER + 0x200)
#define CONFIG_SYS_PAGE_ID                (GUI_ID_USER + 0x210)
#define GUI_CALIBRATOION_ID               (GUI_ID_USER + 0x220)
#define CONFIG_LAB_INFO_PAGE_ID           (GUI_ID_USER + 0x230)
#define CONFIG_ParaSet_PAGE_ID            (GUI_ID_USER + 0x250)
#define CONFIG_Debug_PAGE_ID              (GUI_ID_USER + 0x280)
#define CONFIG_PRINTER_PAGE_ID            (GUI_ID_USER + 0x2A0)
#define CONFIG_FUZHU_PAGE_ID              (GUI_ID_USER + 0x2B0)
#define CONFIG_SWITCH_LOGO_PAGE_ID        (GUI_ID_USER + 0x2C0)
#define CONFIG_USER_MANAGE_PAGE_ID        (GUI_ID_USER + 0x2D0)
#define CONFIG_RESET_PSW_PAGE_ID          (GUI_ID_USER + 0x2E0)
#define CONFIG_DATE_TIME_SET_PAGE_ID      (GUI_ID_USER + 0x2F0)
#define CONFIG_SET_ADD_USER_PAGE_ID       (GUI_ID_USER + 0x300)
#define CONFIG_SET_SKIN_PAGE_ID       	  (GUI_ID_USER + 0x310)
//service
#define HGB_SELF_CHECK_WINDOW_ID   		  (GUI_ID_USER + 0x350)
#define SERVICE_ITEM_PAGE_ID              (GUI_ID_USER + 0x400)
#define AGING_TEST_PAGE_ID                (GUI_ID_USER + 0x410)
#define VERSION_PAGE_ID                	  (GUI_ID_USER + 0x420)
#define STATUS_CHECK_PAGE_ID              (GUI_ID_USER + 0x440)
#define OTHER_CHECK_PAGE_ID               (GUI_ID_USER + 0x480)
#define SELF_CHECK_POPUP_PAGE_ID          (GUI_ID_USER + 0x490)
#define CONFIM_POPUP_PAGE_ID              (GUI_ID_USER + 0x4A0)
#define DATA_MANAGE_PAGE_ID               (GUI_ID_USER + 0x4B0)
#define DATA_MANAGE_CONFIM_PAGE_ID        (GUI_ID_USER + 0x4C0)
#define JIAOHU_CHECK_PAGE_ID              (GUI_ID_USER + 0x4D0)
#define LOG_PAGE_ID            			  (GUI_ID_USER + 0x4E0)
#define DISK_PAGE_ID            		  (GUI_ID_USER + 0x4F0)


//other
#define GUI_PUBLIC_MENU_ID                (GUI_ID_USER + 0x500)
#define GUI_COM_WIDGET_ID                 (GUI_ID_USER + 0x510)
#define ID_BUTTON_ANALYSIS                (GUI_COM_WIDGET_ID + 0x01)
#define ID_BUTTON_ERROR_PROMPT            (GUI_COM_WIDGET_ID + 0x02)
#define ID_BUTTON_LIST_REVIEW             (GUI_COM_WIDGET_ID + 0x03)
#define DIALOG_PAGE_ID            		  (GUI_ID_USER + 0x540)
#define SWITCH_DIALOG_PAGE_ID             (GUI_ID_USER + 0x550)
//poweroff
#define GUI_WINDOW_POWEROFF_ID            (GUI_ID_USER + 0x570)
#define GUI_WINDOW_ENSURE_POWEROFF_ID     (GUI_ID_USER + 0x580)
//qc
#define GUI_WINDOW_QC_ID                  (GUI_ID_USER + 0x600)
#define GUI_WINDOW_QC_ANALYSIS_ID         (GUI_ID_USER + 0x610)
#define GUI_WINDOW_QC_SET_ID              (GUI_ID_USER + 0x630)
#define GUI_WINDOW_QC_LIST_ID             (GUI_ID_USER + 0x670)
//PCBA
#define GUI_WINDOW_PCBA_PAGE_ID		      (GUI_ID_USER + 0x700)

#define SERVICE_XNCHECK_PAGE_ID			  (GUI_ID_USER + 0x7A0)

/************  WM_MESSAGE, 自定义消息  *************************************/
#define WM_PUBLIC_UPDATE                  (WM_USER + 0x02) 

//analysis
#define WM_NEXT_SAMPLE_UPDATE                   (WM_USER + 0x05)
#define WM_MOTOR_ERROR_PROMPT                   (WM_USER + 0x06)
#define WM_PATIENT_INFO_UPDATE                  (WM_USER + 0x07)

//list review
#define WM_LIST_REVIEW_LOAT_DATA_FAIL           (WM_USER + 0x09)
//keyboard Message id
#define KEYBOARD_ID                             (WM_USER + 0x10) 
#define WM_KEYBOARD_UPDATE                      (WM_USER + 0x11)
#define WM_KEYBOARD_END                         (WM_USER + 0x12)
#define WM_KEYBOARD_DELETE                      (WM_USER + 0x13)

//display gif message id
#define WM_OPERATE_GUIDE_GIF                    (WM_USER + 0x15)
#define WM_INPUT_GUIDE_GIF                      (WM_USER + 0x16) 
#define WM_RE_INPUT_GUIDE_GIF                   (WM_USER + 0x17)
#define WM_OK_OUTPUT_GUIDE_GIF                  (WM_USER + 0x18)
#define WM_FAIL_OUTPUT_GUIDE_GIF                (WM_USER + 0x19)   //包括GIF和静态部分
#define WM_FAIL_OUTPUT_GUIDE_GIF2               (WM_USER + 0x20)  //只包含GIF部分

//计数错误提示
#define WM_ANALYSIS_ERROR_PROMPT                (WM_USER + 0x30)
#define WM_ANALYSIS_NORMAL_PROMPT               (WM_USER + 0x31)
#define WM_ANALYSIS_GZ_AUTO_MODE_UPDATE_DATA    (WM_USER + 0x32) //工装信号，自动测试模式
#define WM_ANALYSIS_GIF_OPERATE_OUTIN_KEY       (WM_USER + 0x33) //由进出仓键触发，退出操作引导GIF
#define WM_ANALYSIS_GIF_OPERATE_COUNT_KEY       (WM_USER + 0x34) //由计数键触发，退出操作引导GIF
#define WM_ANALYSISI_ERROR_COUNT_NOT_ALLOW      (WM_USER + 0x35) //开机自检存在故障（影响计数），需先清除故障，再计数
#define WM_ANALYSIS_LOAT_DATA_FAIL              (WM_USER + 0x36)
//界面刷新
#define WM_MENU_UPDATE                          (WM_USER + 0x45)

//定义后台通知前台界面进行数据更新显示的WM MESSAGE ID
#define SERVICE_AGING_USER_ID                   (WM_USER+1)                //服务-->组件老化界面用户数据ID值

//登录自检
#define WM_LOGIN_SELFCHECK_FINISH               (WM_USER + 0x50)

//打印机
#define WM_PRINTER_NO_PAPER                     (WM_USER + 0x60)    //打印机缺纸
#define WM_PRINTER_CONN_ERR                     (WM_USER + 0x61)    //打印机连接异常

//服务--老化界面
#define WM_SERVICE_AGING_TEST_BE_OVER           (WM_USER + 0x70)    //流程老化后端执行完毕后通知界面消息

//PCBA--执行完毕，重新执行下一轮测试消息
#define WM_PCBA_TEST_BE_OVER           			(WM_USER + 0x71)    //PCBA测试后端执行完毕，重新执行下一轮测试的通知界面消息

//设置 HGB Debug
#define WM_SET_HGB_BLACK_AUTO_ADJUST            (WM_USER + 0x80)    //HGB空白自动校准
#define WM_SET_HGB_GZ_AUTO_ADJUST               (WM_USER + 0x81)    //HGB工装自动校准
#define WM_SET_HGB_TEST                         (WM_USER + 0x82)    //HGB测试

//质检相关
#define WM_ZJ_TEST_OVER	                         (WM_USER + 0x90)    //质检中相关测试完毕



//**************************发送给公用对话框的数据结构体*********************************
typedef WM_HWIN WM_WINDOW_CREATE(void);
void Motor_Error_Prompt(WM_MESSAGE *pMsg, WM_WINDOW_CREATE *pWindowCreate);
/***
*用于表示公共对话框中要加载的图片枚举
***/
typedef enum _DialogBmpType_e{
	BMP_ALARM = 0,						//加载！图标，橙色背景
	BMP_OK,								//加载√
	BMP_SELF_CHECK,						//加载自检中图标
    
}DialogBmpType_e;


/***
*用于表示公共对话框中是否显示确认按键
***/
typedef enum _DialogConfimButt_e{
	NO_CONFIM_BUTTON = 0,
	HAS_CONFIM_BUTTON,
}DialogConfimButt_e;


/***
*发送给公共对话框的数据结构体
***/
typedef struct _CommonDialogPageData_s{
	DialogBmpType_e bmpType;					
	DialogConfimButt_e confimButt;
	WM_HWIN (*fun)(void);									//用于保存调用对话框的源窗口的创建函数入口，二级指针
	
	char str[100];
}CommonDialogPageData_s;


/***
*用于公共选择对话框区分点击确认键后要执行的项目枚举
***/
typedef enum _SwitchDialogItem_e{
	CONFIG_USERMANAGE_RECOVER_FACTORY_PSW = 0,								//设置--用户管理--恢复出厂密码
	CONFIG_LAB_INFO_CHANGE_SN,												//设置--实验室信息--修改仪器序列号
	CONFIG_DEL_USER,														//设置--用户管理--删除用户
	CONFIG_CHANGE_DATA_NUM,													//设置--系统设置--切换测试数据容量
	CONFIG_U_UPDATE,														//设置--辅助设置--U盘升级
	
	SERVICE_DATAMANAGE_RESET_PARA,											//服务--数据管理--重置仪器参数
	SERVICE_DATAMANAGE_DEL_TEST_DATA,										//服务--数据管理--删除测试数据
	SERVICE_DATAMANAGE_DEL_USER_DATA,										//服务--数据管理--删除用户数据
	SERVICE_DATAMANAGE_RECOVER_FACTORY,										//服务--数据管理--恢复出厂设置
    //
    LISTVIEW_DELETE_DATA,                                                   //列表回顾和结果详情，中删除数据
    MENU_POWER_OFF_PROMPT,                                                  //菜单界面，是否关机提示
    MENU_POWER_OFF_TAKEOUT_CELL,                                            //菜单界面，执行是否关机前，提示请先取出血细胞计数模块
    MENU_LOGOUT_PROMPT,                                                     //菜单界面，是否注销提示
    
	LOGIN_RESET_MACHINE_PARA,												//登录界面中，长按登录标题栏，重置仪器参数
	
	//样本分析
	ANALYSE_NET_ERR_MUNAUL_INPUT_INFO,										//样本分析界面中下拉患者信息时网络异常，选择是否手动录入患者信息
}SwitchDialogItem_e;



//**************************发送给公用选择对话框的数据结构体*********************************
typedef struct _CommonSwitchDialogPageData_s{
	SwitchDialogItem_e switchDialogItem;
	CommonDialogPageData_s dialogData;
	
	uint32_t u32;
}CommonSwitchDialogPageData_s;



/************  Color，颜色  *************************************/
//button
// 
#define GUI_BUTTON_ROUND_DISABLED_COLOR		        0x00f3f1ec
//
//
#define BTN_RELEASE_GRAY    0x00c9c9c9
#define BTN_PRESS_GRAY    0x00b4b4b4
#define BTN_CHAR_COLOR    GUI_WHITE
#define BTN_FRAME_COLOR   0X00BEBEBE              //按钮边框颜色

#define GUI_BTN_DISABLE_COLOR       0x00f3f1ec
#define GUI_BTN_PRESS_CHAR_COLOR    0x00ffffff
#define GUI_BTN_RELEASE_CHAR_COLOR  0x00333333
//Text
#define GUI_TEXT_FRAME_COLOR        0x00cbcbcb //深灰hui
#define GUI_TEXT_FRAME_COLOR        0x00cbcbcb
//edit
#define EDIT_FRAME_COLOR            0x00a2a09e
#define EDIT_ROUNDED_RADIUS          4              //编辑框的圆角半径
//
#define GUI_SPLINE_LINE_COLOR       0x00efefef    //分割线颜色
//downdrop
#define DROPDOWN_FRAME_COLOR        0x00a2a09e    //下拉边框初始化颜色
#define DROPDOWN_UNSEL_COLOR        GUI_BLACK     //没选中元素的颜色
#define DROPDOWN_SELFOCUS_COLOR     0x00EEDFB2    //选中元素的颜色0X00BEBEBE
#define DROPDOWN_DEFAULT_COLOR      GUI_WHITE    //选中元素的颜色
#define DROPDOWN_ROUNDED_RADIO      3            //圆角率
#define DROPDOWN_SPACE_LEN          5           //元素间的间距
// dropdown color
#define NEXT_SAMPLE_DROPDOWN_UNSEL_COLOR        GUI_BLACK               
#define NEXT_SAMPLE_DROPDOWN_SELFOCUS_COLOR     0x00EEDFB2
#define NEXT_SAMPLE_DROPDOWN_FRAME_INIT_COLOR   0X00a2a09e               //下拉边框初始化颜色
#define NEXT_SAMPLE_DROPDOWN_FRAME_CLICK_COLOR  0x00EEDFB2               //下拉边框点击颜色


/************* 图标 ****************************/
//
#define USE_BMP_PICTURE     1 
#if USE_BMP_PICTURE
/***** BMP *****/
extern GUI_CONST_STORAGE GUI_BITMAP bm_logo_x1;                         // 公司，logo
//extern const unsigned short _aclogo_x2[12018UL + 1];                  // 公司，logo*2 二倍放大

//
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_prev_btn_black;  //上一记录，箭头图标，黑色
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_prev_btn_gray;   //上一记录，箭头图标，灰色
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_prev_btn_Blue;   //上一记录，箭头图标（蓝底）
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_next_btn_black;  //下一记录，箭头图标,黑色
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_next_btn_gray;   //下一记录，箭头图标,灰色
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_next_btn_Blue;   //下一记录，箭头图标(蓝底)
extern GUI_CONST_STORAGE GUI_BITMAP bm_PullDown_ARROW;                  //下拉箭头，用于下一样本等，dropdown
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_btn_active;                 //菜单，激活状体
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_btn_idle;                   //菜单，空闲状态
//
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_analysis;               //菜单，样本分析
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_list;                   //菜单，列表回顾
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_qc;                     //菜单，质控
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_service;                //菜单，服务图标
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_set;                    //菜单，设置图标
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_logout;                    //菜单，注销
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_poweroff;               //菜单，关机
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_user;                   //菜单，用户头像
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_btn_arrow;              //菜单，箭头
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_btn_close;                //菜单，退出菜单
//
extern GUI_CONST_STORAGE GUI_BITMAP bm_list_btn_idle;                //列表回顾，空闲状态
extern GUI_CONST_STORAGE GUI_BITMAP bm_list_btn_active;                //列表回顾，激活状态
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_Up;                //列表回顾，向上单箭头
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_Up_Blue;				//列表回顾，向上单箭头（蓝底）
extern GUI_CONST_STORAGE GUI_BITMAP bmListReview_Up_Gray;				//列表回顾，向上单箭头（灰底）
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_UpUp;                //列表回顾，向上双箭头
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_UpUp_Blue;			//列表回顾，向上双箭头(蓝底)
extern GUI_CONST_STORAGE GUI_BITMAP bmListReview_UpUp_Gray;				//列表回顾，向上双箭头(灰底)
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_Down;                //列表回顾，向下单箭头
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_Down_Blue;        //列表回顾，向下单箭头(蓝底)
extern GUI_CONST_STORAGE GUI_BITMAP bmListReview_Down_Gray;			//列表回顾，向下单箭头(灰底)
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_DownDown;         //列表回顾，向下双箭头
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_DownDown_Blue;    //列表回顾，向下双箭头(蓝底)
extern GUI_CONST_STORAGE GUI_BITMAP bmListReview_DD_Gray;			//列表回顾，向下双箭头(灰底)

//extern const unsigned short _acErrorPrompt_OK[2056UL + 1];        //错误提示，对号
extern GUI_CONST_STORAGE GUI_BITMAP bm_circle_error_prompt;         //错误提示，感叹号, 圆形
extern GUI_CONST_STORAGE GUI_BITMAP bm_triangle_error_prompt;       //错误提示，感叹号，三角形

#else

/***** PNG *****/
extern const unsigned char _aclogo[2560UL + 1];                    // 公司，logo
extern const unsigned char _aclogo_x2[12018UL + 1];       // 公司，logo*2 二倍放大
extern const unsigned char _acPullDown_ARROW[1062UL + 1];        //下拉箭头，用于dropdown
//
extern const unsigned char _acanalysis_btn_active[381UL + 1];    //样本分析，激活状态
extern const unsigned char _acanalysis_btn_idle[1289UL + 1];    //样本分析，空闲状态
extern const unsigned char _aclist_btn_active[1248UL + 1];        //样本分析，激活状态
extern const unsigned char _acanalysis_fault[1136UL + 1];        //样本分析，错误提示“！”图标
extern const unsigned char _acanalysis_record_next_btn[1033UL + 1];    //上一记录，箭头图标
extern const unsigned char _acanalysis_record_prev_btn[1039UL + 1];    //下一记录，箭头图标
//
extern const unsigned char _acmenu_btn_active[1039UL + 1];        //菜单，激活状体
extern const unsigned char _acmenu_btn_idle[1070UL + 1];        //菜单，空闲状体
extern const unsigned char _acmenu_setup[1289UL + 1];            //菜单，设置图标
extern const unsigned char _acmenu_service[1036UL + 1];            //菜单，服务图标
extern const unsigned char _acmenu_setup[1289UL + 1];            //菜单，设置图标
extern const unsigned char _acmenu_power[1379UL + 1];            //菜单，电源
extern const unsigned char _acmenu_logout[1204UL + 1];            //菜单，退出
//
extern const unsigned char _aclist_btn_active[1248UL + 1];        //列表回顾，激活状态
extern const unsigned char _aclist_btn_idle[1303UL + 1];        //列表回顾，空闲状态
extern const unsigned char _ac_ListReview_Up[1066UL + 1];        //列表回顾，向上单箭头
extern const unsigned char _ac_ListReview_UpUp[1087UL + 1];        //列表回顾，向上双箭头
extern const unsigned char _ac_ListReview_Down[1355UL + 1];        //列表回顾，向下单箭头
extern const unsigned char _ac_ListReview_DownDown[1414UL + 1]; //列表回顾，向下双箭头
//
extern const unsigned char _acErrorPrompt_OK[2056UL + 1];       //错误提示，对号
extern const unsigned char _acErrorPrompt_Fail_c[2236UL + 1];   //错误提示，感叹号, 圆形
extern const unsigned char _acErrorPrompt_Fail_s[1201UL + 1];    //错误提示，感叹号，三角形

#endif

typedef enum{
    //样本分析
//    LOGO_analysis_btn_active            = 0x10,
//    LOGO_analysis_btn_idle              = 0x11,
//    LOGO_analysis_record_prev_btn_black = 0x12,
//    LOGO_analysis_record_prev_btn_gray  = 0x13,
//    LOGO_analysis_record_prev_btn_Blue  = 0x14,
//    LOGO_analysis_record_next_btn_black = 0x15,
//    LOGO_analysis_record_next_btn_gray  = 0x16,
//    LOGO_analysis_record_next_btn_Blue  = 0x17,
//    LOGO_analysis_menu_btn_active       = 0x18,
//    LOGO_analysis_menu_btn_idle         = 0x19,     
//    LOGO_PullDown_ARROW                 = 0x1a,

//    //菜单
//    LOGO_menu_analysis                  = 0x21,
//    LOGO_menu_list                      = 0x22,
//    LOGO_menu_qc                        = 0x23,
//    LOGO_menu_service                   = 0x24,
//    LOGO_menu_set                       = 0x25,
//    LOGO_menu_btn_logout                = 0x26,
//    LOGO_menu_poweroff                  = 0x27,
//    LOGO_menu_btn_user                  = 0x2a,     //用户头像
//    LOGO_menu_btn_close                 = 0x2b,     //关闭菜单界面
//    LOGO_menu_arrow                     = 0x2c,     //关闭菜单界面

//    //列表回顾
//    LOGO_list_btn_idle                  = 0x40,
//    LOGO_list_btn_active                = 0x41,
//    LOGO_circle_error_prompt            = 0x4e,
//    LOGO_riangle_error_prompt           = 0x4f,
	AAA,
}Logo_e;

//显示已转为数据的小图标
void Draw_Const_Logos(Logo_e eLogo, uint16_t usX, uint16_t usY);



/*
*   打印机错误类型
*/
typedef enum {
    PRINTER_NO_PAPER    = 1,
    PRINTER_CONN_ERROR  = 2,
    PRINTER_END         = 3,
} PrinterErrorType_e;


/************* 常量，中英文定义 ****************************/
extern const char g_ucaLng_Operator[LANGUAGE_END][10];
extern const char g_ucaLng_Login[LANGUAGE_END][7];
extern const char g_ucaLng_Name[LANGUAGE_END][7];
extern const char g_ucaLng_Sexual[LANGUAGE_END][7];
extern const char g_ucaLng_Age[LANGUAGE_END][7];
extern const char g_ucaLng_CurUser[LANGUAGE_END][13];
extern const char g_ucaLng_NextSampleSN[LANGUAGE_END][16];
extern const char g_ucaLng_SampleNum[LANGUAGE_END][13];
extern const char g_ucaLng_BirthDay[LANGUAGE_END][13];
extern const char g_ucaLng_Year[LANGUAGE_END][5];
extern const char g_ucaLng_Month[LANGUAGE_END][6];
extern const char g_ucaLng_Week[LANGUAGE_END][5];
extern const char g_ucaLng_Day[LANGUAGE_END][4];
extern const char g_ucaLng_ReferGroup[LANGUAGE_END][13];
extern const char g_ucaLng_General[LANGUAGE_END][8];
extern const char g_ucaLng_AdultMale[LANGUAGE_END][13];
extern const char g_ucaLng_AdultFemale[LANGUAGE_END][13];
extern const char g_ucaLng_Child[LANGUAGE_END][7];
extern const char g_ucaLng_Neonatus[LANGUAGE_END][10];
extern const char g_ucaLng_SexMale[LANGUAGE_END][5];
extern const char g_ucaLng_SexFemale[LANGUAGE_END][8];
extern const char g_ucaLng_Sure[LANGUAGE_END][8];
extern const char g_ucaLng_Cancel[LANGUAGE_END][8];
extern const char g_ucaLng_Yes[LANGUAGE_END][4];
extern const char g_ucaLng_No[LANGUAGE_END][4];
extern const char g_ucaLng_ReCount_Prompt[LANGUAGE_END][19];
extern const char g_ucaLng_Ensure_PowerOff[LANGUAGE_END][20];
extern const char g_ucaLng_TakeOutCell_Before_PowerOff[LANGUAGE_END][50];
extern const char g_ucaLng_Ensure_Logout[LANGUAGE_END][19];
extern const char g_ucaLng_Delete[LANGUAGE_END][8];
extern const char g_ucaLng_Refresh[LANGUAGE_END][15];
extern const char g_ucaLng_SampleAnalysis[LANGUAGE_END][13];
extern const char g_ucaLng_ListReview[LANGUAGE_END][13];
extern const char g_ucaLng_QC[LANGUAGE_END][7];
extern const char g_ucaLng_Menu[LANGUAGE_END][7];
extern const char g_ucaLng_Service[LANGUAGE_END][8];
extern const char g_ucaLng_Set[LANGUAGE_END][9];
extern const char g_ucaLng_Logout[LANGUAGE_END][7];
extern const char g_ucaLng_PowerOff[LANGUAGE_END][10];
extern const char g_ucaLng_Test[LANGUAGE_END][10];
extern const char g_ucaLng_OpenLED[LANGUAGE_END][9];
extern const char g_ucaLng_CloseLED[LANGUAGE_END][10];
extern const char g_ucaLng_BlackAutoAdjust[LANGUAGE_END][19];
extern const char g_ucaLng_GZAutoAdjust[LANGUAGE_END][19];
extern const char g_ucaLng_SET_GZ_ADC[LANGUAGE_END][16];
extern const char g_ucaLng_LoadDate_Err[LANGUAGE_END][19];
extern const char g_ucaLng_Login_ResetMachPara[LANGUAGE_END][30];
extern const char g_ucaLng_Num[LANGUAGE_END][13];

//样本分析
extern const char g_ucaLng_LastRecord[LANGUAGE_END][13];
extern const char g_ucaLng_NextRecord[LANGUAGE_END][13];    
extern const char g_ucaLng_NextSample[LANGUAGE_END][20];
extern const char g_ucaLng_ReMark[LANGUAGE_END][7];
extern const char g_ucaLng_NextSampleDownloadInfo[LANGUAGE_END][35];
//extern const char g_ucaLng_CountingPrompt[LANGUAGE_END][34];
extern const char g_ucaLng_PatientInfo[LANGUAGE_END][13];
extern const char g_ucaLng_Print[LANGUAGE_END][7];
extern const char g_ucaLng_CheckTime[LANGUAGE_END][13];
extern const char g_ucaLng_Param[LANGUAGE_END][13];
extern const char g_ucaLng_PatientInfoDownloadInfo[LANGUAGE_END][30];
extern const char g_ucaLng_Result[LANGUAGE_END][7];
extern const char g_ucaLng_Unit[LANGUAGE_END][7];
extern const char g_ucaLng_WBC[LANGUAGE_END][10];
extern const char g_ucaLng_Gan[LANGUAGE_END][20];  
extern const char g_ucaLng_Mid[LANGUAGE_END][13]; 
extern const char g_ucaLng_Lym[LANGUAGE_END][13];
extern const char g_ucaLng_HGB[LANGUAGE_END][13];
extern const char g_ucaLng_ErrorPrompt[LANGUAGE_END][18];
extern const char g_ucaLng_WBCErrorPrompt[LANGUAGE_END][22];   
extern const char g_ucaLng_HGBErrorPrompt[LANGUAGE_END][25];
extern const char g_ucaLng_LoginErrorPrompt[LANGUAGE_END][28];
extern const char g_ucaLng_OutIn_Error[LANGUAGE_END][30];
extern const char g_ucaLng_Analyse_InputUserNum[LANGUAGE_END][20];
extern const char g_ucaLng_Analyse_PatientList[LANGUAGE_END][20];
extern const char g_ucaLng_Analyse_InputUserNumNetERR[LANGUAGE_END][45];
extern const char g_ucaLng_Analyse_NetERR[LANGUAGE_END][40];
extern const char g_ucaLng_Analyse_RequestParientInfo[LANGUAGE_END][30];
extern const char g_ucaLng_Analyse_NoParientInfo[LANGUAGE_END][30];
extern const char g_ucaLng_Analyse_NOJSC[LANGUAGE_END][45];

/***********列表回顾*****************************************************/
extern const char g_ucaLng_ResultInfo[LANGUAGE_END][13];
extern const char g_ucaLng_ReFreshNet[LANGUAGE_END][20];
extern const char g_ucaLng_Edit[LANGUAGE_END][7];
extern const char g_ucaLng_Return[LANGUAGE_END][7];
extern const char g_ucaLng_IfDeleteData[LANGUAGE_END][16];
extern const char g_ucaLng_WaitForRefresh[LANGUAGE_END][30];
extern const char g_ucaLng_RefreshNetOK[LANGUAGE_END][30];
extern const char g_ucaLng_RefreshNetErr[LANGUAGE_END][35];

/***********服务*********************************************************/
extern const char g_ucaLng_VersionInfo[LANGUAGE_END][22];
extern const char g_ucaLng_VersionFull[LANGUAGE_END][19];
extern const char g_ucaLng_VersionRelease[LANGUAGE_END][19];
extern const char g_ucaLng_VersionMcu[LANGUAGE_END][13];
extern const char g_ucaLng_VersionAlgo[LANGUAGE_END][13];
extern const char g_ucaLng_VersionHardWare[LANGUAGE_END][18];
extern const char g_ucaLng_Serve_Serve[LANGUAGE_END][10];
extern const char g_ucaLng_Serve_Selfcheck[LANGUAGE_END][10];
extern const char g_ucaLng_Serve_Log[LANGUAGE_END][10];
extern const char g_ucaLng_Serve_Info[LANGUAGE_END][15]; 
extern const char g_ucaLng_Serve_BuildCheck[LANGUAGE_END][15]; 
extern const char g_ucaLng_Serve_BuildCheckS[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_BuildCheckF[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_Check[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_AIRTIGCheckS[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_AIRTIGCheckF[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_MotoCheck[LANGUAGE_END][30];  
extern const char g_ucaLng_Serve_MotoCheckS[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_MotoCheckF[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_StatusCheck[LANGUAGE_END][15];
extern const char g_ucaLng_Serve_InteractionCheck[LANGUAGE_END][15];
extern const char g_ucaLng_Serve_OtherCheck[LANGUAGE_END][15];
extern const char g_ucaLng_Serve_AgingCheck[LANGUAGE_END][15];
extern const char g_ucaLng_Serve_TestLog[LANGUAGE_END][15];
extern const char g_ucaLng_Serve_ErrLog[LANGUAGE_END][15];
extern const char g_ucaLng_Serve_RunLog[LANGUAGE_END][15]; 
extern const char g_ucaLng_Serve_Version[LANGUAGE_END][15];
extern const char g_ucaLng_Serve_Disk[LANGUAGE_END][15];
extern const char g_ucaLng_Serve_DataManage[LANGUAGE_END][15]; 
extern const char g_ucaLng_Serve_Building[LANGUAGE_END][22];
extern const char g_ucaLng_Serve_AirTighting[LANGUAGE_END][25];
extern const char g_ucaLng_Serve_MotoChecking[LANGUAGE_END][30];


extern const char g_ucaLng_Serve_Other[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_XK[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_Pump[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_Beep[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_AirValve[LANGUAGE_END][30];  
extern const char g_ucaLng_Serve_AirValveS[LANGUAGE_END][40];
extern const char g_ucaLng_Serve_LiquidValve[LANGUAGE_END][30]; 
extern const char g_ucaLng_Serve_LiquidValveS[LANGUAGE_END][40];
extern const char g_ucaLng_Serve_HGB[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_XKSuccess[LANGUAGE_END][30];   
extern const char g_ucaLng_Serve_XKFail[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_AirValveing[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_LiquidValveing[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_PumpChecking[LANGUAGE_END][30]  ;
extern const char g_ucaLng_Serve_PumpCheckS[LANGUAGE_END][40];
extern const char g_ucaLng_Serve_BeepChecking[LANGUAGE_END][30]  ;
extern const char g_ucaLng_Serve_BeepCheckS[LANGUAGE_END][40];
extern const char g_ucaLng_Serve_HGBChecking[LANGUAGE_END][30]   ;
extern const char g_ucaLng_Serve_HGBCheckingOK[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_HGBCheckingERR[LANGUAGE_END][30];



extern const char g_ucaLng_Serve_PumpChecking[LANGUAGE_END][30]  ;
extern const char g_ucaLng_Serve_BeepChecking[LANGUAGE_END][30]  ;
extern const char g_ucaLng_Serve_HGBChecking[LANGUAGE_END][30]   ;

extern const char g_ucaLng_Serve_XuHao[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_Time[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_OutlineInfo[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_Status[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_Operator[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_HaocaiErr[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_OperatorErr[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_MachErr[LANGUAGE_END][20];

extern const char g_ucaLng_Serve_ZHErr[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_Testlog[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_TestUser[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_EngineerUser[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_Errlog[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_Runlog[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_Conserr[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_Operatorerr[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_Macherr[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_ZHerr[LANGUAGE_END][20];

extern const char g_ucaLng_Serve_VersionInfo[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_FullVer[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_AlgoVer[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_MainVer[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_BootloadVer[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_HardVer[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_UIVer[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_PubVer[LANGUAGE_END][20];

extern const char g_ucaLng_Serve_DiskInfo[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_UsedPlace[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_CanPlace[LANGUAGE_END][20];
extern const char g_ucaLng_Serve_TotalPlace[LANGUAGE_END][20];

extern const char g_ucaLng_Serve_DATAMENT[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_DEL_USERDATA[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_CONFIM_DELUSERDATA[LANGUAGE_END][60];
extern const char g_ucaLng_Serve_DEL_USERDATA_SUCCESS[LANGUAGE_END][30];
extern const char g_ucaLng_Serve_DEL_USERDATA_FAIL[LANGUAGE_END][30];

/***********设置*********************************************************/
extern const char g_ucaLng_ManualCalibration[LANGUAGE_END][19];
extern const char g_ucaLng_CalibrationFactor[LANGUAGE_END][22];
extern const char g_ucaLng_CalibrationDate[LANGUAGE_END][22];
extern const char g_ucaLng_SET_XiTong[LANGUAGE_END][20];
extern const char g_ucaLng_SET_YongHu[LANGUAGE_END][20];  
extern const char g_ucaLng_SET_FuZhu[LANGUAGE_END][20];
extern const char g_ucaLng_SET_DaYing[LANGUAGE_END][20];
extern const char g_ucaLng_SET_ShiYanShi[LANGUAGE_END][20];
extern const char g_ucaLng_SET_JiaoZhun[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Canshu[LANGUAGE_END][20];
extern const char g_ucaLng_SET_TiaoShi[LANGUAGE_END][20];

extern const char g_ucaLng_SET_Sys[LANGUAGE_END][15];
extern const char g_ucaLng_SET_Date[LANGUAGE_END][10];
extern const char g_ucaLng_SET_Time[LANGUAGE_END][10];
extern const char g_ucaLng_SET_Language[LANGUAGE_END][10];
extern const char g_ucaLng_SET_Chinese[LANGUAGE_END][10];
extern const char g_ucaLng_SET_English[LANGUAGE_END][10];
extern const char g_ucaLng_SET_DataNum[LANGUAGE_END][25];
extern const char g_ucaLng_SET_ConfirmDataNum[LANGUAGE_END][80];

extern const char g_ucaLng_SET_User[LANGUAGE_END][22];
extern const char g_ucaLng_SET_NewUser[LANGUAGE_END][15]   ;
extern const char g_ucaLng_SET_DelUser[LANGUAGE_END][15]   ;
extern const char g_ucaLng_SET_SetDefault[LANGUAGE_END][25];
extern const char g_ucaLng_SET_ChangePsw[LANGUAGE_END][15];
extern const char g_ucaLng_SET_FactoryReset[LANGUAGE_END][20];  
extern const char g_ucaLng_SET_UserName[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Limit[LANGUAGE_END][10];
extern const char g_ucaLng_SET_Type[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Default[LANGUAGE_END][20];
extern const char g_ucaLng_SET_DianjiXuanxiang[LANGUAGE_END][20];
extern const char g_ucaLng_SET_NoLimit[LANGUAGE_END][20];
extern const char g_ucaLng_SET_ConfimPsw[LANGUAGE_END][35];
extern const char g_ucaLng_SET_SuperUser[LANGUAGE_END][20] ;
extern const char g_ucaLng_SET_AdminUser[LANGUAGE_END][20] ;
extern const char g_ucaLng_SET_NormalUser[LANGUAGE_END][20];
extern const char g_ucaLng_SET_NewUserN[LANGUAGE_END][20]  ;
extern const char g_ucaLng_SET_ConfimDelUser[LANGUAGE_END][30];
extern const char g_ucaLng_SET_RecoverFactory[LANGUAGE_END][70];

extern const char g_ucaLng_SET_ASSIST[LANGUAGE_END][22]    ;
extern const char g_ucaLng_SET_CompanySwitch[LANGUAGE_END][20];
extern const char g_ucaLng_SET_UUpdate[LANGUAGE_END][15]      ;
extern const char g_ucaLng_SET_SetSkin[LANGUAGE_END][15]      ;
extern const char g_ucaLng_SET_RES3[LANGUAGE_END][15]      ;
extern const char g_ucaLng_SET_UpdateMain[LANGUAGE_END][20];
extern const char g_ucaLng_SET_UpdateBoot[LANGUAGE_END][20];
extern const char g_ucaLng_SET_UpdateUI[LANGUAGE_END][20]  ;
extern const char g_ucaLng_SET_Configing[LANGUAGE_END][45] ;
extern const char g_ucaLng_SET_ConfirmUUpdate[LANGUAGE_END][80];
extern const char g_ucaLng_SET_UpanTimeout[LANGUAGE_END][40];
extern const char g_ucaLng_SET_UpanFoumtFail[LANGUAGE_END][35];
extern const char g_ucaLng_SET_UUpdateSuccess[LANGUAGE_END][35];
extern const char g_ucaLng_SET_BootUpanFileErr[LANGUAGE_END][35];
extern const char g_ucaLng_SET_BootUpanFileWriteFail[LANGUAGE_END][35];
extern const char g_ucaLng_SET_BootUpanOtherErr[LANGUAGE_END][35];
extern const char g_ucaLng_SET_MainUpanFileErr[LANGUAGE_END][35];
extern const char g_ucaLng_SET_MainUpanFileWriteFail[LANGUAGE_END][35];
extern const char g_ucaLng_SET_MainUpanOtherErr[LANGUAGE_END][35];
extern const char g_ucaLng_SET_UIUpanFileErr[LANGUAGE_END][35];
extern const char g_ucaLng_SET_UIUpanFileWriteFail[LANGUAGE_END][35];
extern const char g_ucaLng_SET_UIUpanOtherErr[LANGUAGE_END][35];


extern const char g_ucaLng_SET_Printer[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Title[LANGUAGE_END][15];
extern const char g_ucaLng_SET_PrinterMode[LANGUAGE_END][15];
extern const char g_ucaLng_SET_TitleInfo[LANGUAGE_END][30];
extern const char g_ucaLng_SET_AutoPrinter[LANGUAGE_END][30];
extern const char g_ucaLng_SET_ManualPrinter[LANGUAGE_END][32];

extern const char g_ucaLng_SET_LabInfo[LANGUAGE_END][20];
extern const char g_ucaLng_SET_MachName[LANGUAGE_END][20];
extern const char g_ucaLng_SET_MachType[LANGUAGE_END][20];
extern const char g_ucaLng_SET_MachSN[LANGUAGE_END][20];
extern const char g_ucaLng_SET_ZhensuoName[LANGUAGE_END][20]; 
extern const char g_ucaLng_SET_MachNameInfo[LANGUAGE_END][35];
extern const char g_ucaLng_SET_MachNameInfoYL1[LANGUAGE_END][35];
extern const char g_ucaLng_SET_WaitForConfim[LANGUAGE_END][35];
extern const char g_ucaLng_SET_ConfimChangeSN[LANGUAGE_END][35];
extern const char g_ucaLng_SET_SNERR[LANGUAGE_END][35];

extern const char g_ucaLng_SET_ParaSet[LANGUAGE_END][20];
extern const char g_ucaLng_SET_WBC_Gain[LANGUAGE_END][20];
extern const char g_ucaLng_SET_HGB_Gain[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Pump[LANGUAGE_END][20];
extern const char g_ucaLng_SET_535Current[LANGUAGE_END][20] ;
extern const char g_ucaLng_SET_415Current[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Pressure[LANGUAGE_END][20];
extern const char g_ucaLng_SET_OCIN[LANGUAGE_END][20];
extern const char g_ucaLng_SET_OCOUT[LANGUAGE_END][20];
extern const char g_ucaLng_SET_OCDAOTONG[LANGUAGE_END][20]; 
extern const char g_ucaLng_SET_OCDUANKAI[LANGUAGE_END][20]; 
extern const char g_ucaLng_SET_PRESSURESensor[LANGUAGE_END][20];
extern const char g_ucaLng_SET_AUTOAdjust[LANGUAGE_END][20];
extern const char g_ucaLng_SET_HGB_Debug[LANGUAGE_END][15];
extern const char g_ucaLng_SET_HGB_AjustParaSet[LANGUAGE_END][19];
extern const char g_ucaLng_SET_PumpAutoSuccess[LANGUAGE_END][35];
extern const char g_ucaLng_SET_PumpAutoFail[LANGUAGE_END][35];
extern const char g_ucaLng_SET_BuildPressing[LANGUAGE_END][35];
extern const char g_ucaLng_SET_BuildPressSuccess[LANGUAGE_END][45];
extern const char g_ucaLng_SET_BuildPressFail[LANGUAGE_END][45];

extern const char g_ucaLng_SET_Debug[LANGUAGE_END][20];
extern const char g_ucaLng_SET_CountDelay[LANGUAGE_END][20];
extern const char g_ucaLng_SET_CountReuse[LANGUAGE_END][30];
extern const char g_ucaLng_SET_DebugWBC[LANGUAGE_END][30];
extern const char g_ucaLng_SET_Count[LANGUAGE_END][20];

extern const char g_ucaLng_SET_LogoSwitch[LANGUAGE_END][20];
extern const char g_ucaLng_SET_CH[LANGUAGE_END][20] ;
extern const char g_ucaLng_SET_Reserve1[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Reserve2[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Reserve3[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Reserve4[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Reserve5[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Reserve6[LANGUAGE_END][20];
extern const char g_ucaLng_SET_Reserve7[LANGUAGE_END][20];

extern const char g_ucaLng_SET_ChangePW[LANGUAGE_END][20];
extern const char g_ucaLng_SET_OldPW[LANGUAGE_END][20];
extern const char g_ucaLng_SET_NewPW[LANGUAGE_END][20];
extern const char g_ucaLng_SET_ConfimPW[LANGUAGE_END][20]; 
extern const char g_ucaLng_SET_OldPWErr[LANGUAGE_END][50]; 
extern const char g_ucaLng_SET_PWNotMatch[LANGUAGE_END][60]; 
extern const char g_ucaLng_SET_EnterNewPW[LANGUAGE_END][40];
extern const char g_ucaLng_SET_ChangePWLogo1[LANGUAGE_END][40];
extern const char g_ucaLng_SET_ChangePWLogo2[LANGUAGE_END][40];
extern const char g_ucaLng_SET_ChangePWLogo3[LANGUAGE_END][40];
extern const char g_ucaLng_SET_ChangePWLogo4[LANGUAGE_END][40];
extern const char g_ucaLng_SET_NewUserName[LANGUAGE_END][20];
extern const char g_ucaLng_SET_AddUser[LANGUAGE_END][20];
extern const char g_ucaLng_SET_NoUserErr[LANGUAGE_END][35];
extern const char g_ucaLng_SET_UserFull[LANGUAGE_END][30];
extern const char g_ucaLng_SET_UserExists[LANGUAGE_END][35];

extern const char g_ucaLng_SET_RecoverPW[LANGUAGE_END][40];
extern const char g_ucaLng_SET_Recover001PW[LANGUAGE_END][55];  
extern const char g_ucaLng_SET_RecoverAdminPW[LANGUAGE_END][55]; 
extern const char g_ucaLng_SET_RecoverTestPW[LANGUAGE_END][55];
extern const char g_ucaLng_SET_RecoverEnginePW[LANGUAGE_END][55];
extern const char g_ucaLng_SET_ChangeSning[LANGUAGE_END][70];
extern const char g_ucaLng_SET_DelTestDataing[LANGUAGE_END][25];
extern const char g_ucaLng_SET_DelUserDataing[LANGUAGE_END][25];





/***********登录/关机*********************************************************/
extern const char g_ucaLng_Login_SelfCheck[LANGUAGE_END][35];
extern const char g_ucaLng_PowerOff_Prompt[LANGUAGE_END][50];
extern const char g_ucaLng_Selfcheck_Err[LANGUAGE_END][25];
    
/***********其他*********************************************************/
extern const char g_ucaLng_ErrorCode[LANGUAGE_END][13];
extern const char g_ucaLng_ErrorName[LANGUAGE_END][13];
extern const char g_ucaLng_ErrorHandle[LANGUAGE_END][13];   
extern const char g_ucaLng_ClearError[LANGUAGE_END][13];
extern const char g_ucaLng_ClearErrorBeforCount[LANGUAGE_END][40];
extern const char g_ucaLng_ClearErrorPrompt[LANGUAGE_END][28];
extern const char g_ucaLng_Printer_NoPaper_ErrPrompt[LANGUAGE_END][55];
extern const char g_ucaLng_LOCK_MACHINE[LANGUAGE_END][55];
extern const char g_ucaLng_UNLOCK_MACHINE[LANGUAGE_END][40];
extern const char g_ucaLng_START_UPDATE[LANGUAGE_END][50];
extern const char g_ucaLng_Success[LANGUAGE_END][8];
extern const char g_ucaLng_Failure[LANGUAGE_END][8];
extern const char g_ucaLng_InFailure[LANGUAGE_END][22];
extern const char g_ucaLng_Error[LANGUAGE_END][10];
extern const char g_ucaLng_InputError[LANGUAGE_END][13];
extern const char g_ucaLng_DataTime_InputError[LANGUAGE_END][19];

/*********** 质控 *********************************************************/
extern const char g_ucaLng_QC_Set[LANGUAGE_END][13];
extern const char g_ucaLng_QC_Analysis[LANGUAGE_END][13];
//extern const char g_ucaLng_QC_Chart[LANGUAGE_END][10];
extern const char g_ucaLng_QC_List[LANGUAGE_END][13];
extern const char g_ucaLng_QC_Type[LANGUAGE_END][7];
extern const char g_ucaLng_New[LANGUAGE_END][7];
extern const char g_ucaLng_Save[LANGUAGE_END][7];
extern const char g_ucaLng_QC_Type[LANGUAGE_END][7];
extern const char g_ucaLng_Data[LANGUAGE_END][7];
extern const char g_ucaLng_Capacity[LANGUAGE_END][9];
extern const char g_ucaLng_LoseControl[LANGUAGE_END][15];
extern const char g_ucaLng_LotNO[LANGUAGE_END][9];
extern const char g_ucaLng_UpperLimit[LANGUAGE_END][13];
extern const char g_ucaLng_LowerLimit[LANGUAGE_END][13];
extern const char g_ucaLng_Level[LANGUAGE_END][7];
extern const char g_ucaLng_High[LANGUAGE_END][5];
extern const char g_ucaLng_Middle[LANGUAGE_END][7];
extern const char g_ucaLng_Low[LANGUAGE_END][4];
extern const char g_ucaLng_FileNOIsFull[LANGUAGE_END][55];

extern const char g_ucaLng_FileNO[LANGUAGE_END][10];
extern const char g_ucaLng_Seter[LANGUAGE_END][15];
extern const char g_ucaLng_ValidDate[LANGUAGE_END][18];
extern const char g_ucaLng_QCSampleSN[LANGUAGE_END][19];
extern const char g_ucaLng_FileNOSelect[LANGUAGE_END][18];
extern const char g_ucaLng_FileNOInvalid[LANGUAGE_END][18];
extern const char g_ucaLng_ResultInCtrl[LANGUAGE_END][18];
extern const char g_ucaLng_ResultOutCtrl[LANGUAGE_END][20];
extern const char g_ucaLng_DateTime_Format[LANGUAGE_END][13];
extern const char g_ucaLng_InputParamError[LANGUAGE_END][19];
/***********错误码对应的名称*********************************************************/

extern const char g_ucaLng_ErrCodeName_BuildPress[LANGUAGE_END][25];
extern const char g_ucaLng_ErrCodeName_AirLight[LANGUAGE_END][22];
extern const char g_ucaLng_ErrCodeName_BC_Current[LANGUAGE_END][32];
extern const char g_ucaLng_ErrCodeName_12VP[LANGUAGE_END][19];
extern const char g_ucaLng_ErrCodeName_12VN[LANGUAGE_END][19];
extern const char g_ucaLng_ErrCodeName_OutIn_Out[LANGUAGE_END][32];
extern const char g_ucaLng_ErrCodeName_OutIn_In[LANGUAGE_END][32];
extern const char g_ucaLng_ErrCodeName_OutIn_OC[LANGUAGE_END][25];
extern const char g_ucaLng_ErrCodeName_TZ_Touch[LANGUAGE_END][28];
extern const char g_ucaLng_ErrCodeName_TZ_Release[LANGUAGE_END][25];
extern const char g_ucaLng_ErrCodeName_Clamp_OC[LANGUAGE_END][25];
extern const char g_ucaLng_ErrCodeName_NoAir[LANGUAGE_END][23];
extern const char g_ucaLng_ErrCodeName_Elec_Triggle[LANGUAGE_END][33];
extern const char g_ucaLng_ErrCodeName_No_Paper[LANGUAGE_END][35];
extern const char g_ucaLng_ErrCodeName_No_Periter[LANGUAGE_END][25];
extern const char g_ucaLng_ErrCodeName_4G_Cat1[LANGUAGE_END][25];
extern const char g_ucaLng_ErrCodeName_Spi_Flash[LANGUAGE_END][30];
extern const char g_ucaLng_ErrCodeName_HGB[LANGUAGE_END][20];
extern const char g_ucaLng_ErrCodeName_Cell_Check[LANGUAGE_END][46];
extern const char g_ucaLng_ErrCodeName_Disk[LANGUAGE_END][25];
extern const char g_ucaLng_Prompt_ClearErr[LANGUAGE_END][65];


#define PUBLIC_ALPHA_COLOR                 0x00525a4e              //背景颜色
#define PUBLIC_ALPHA_VALUE                 30                      //透明值

//界面API    
/******************** Public menu *****************************/
void Public_cbBkWin(WM_MESSAGE* pMsg);
void Public_Msg_Init(void);
void Public_Hide_Widget(void);
void Public_Show_Widget(void);
void Public_Dropdown_Callback(WM_MESSAGE *pMsg);
void Public_Edit_Callback(WM_MESSAGE *pMsg);
void Public_FrameBK(GUI_RECT *ptRect, uint16_t usX, uint16_t usY);
void Window_Alpha(WM_MESSAGE* pMsg, GUI_RECT *ptRect);  //窗口透明化设置
void AlphaBk_cbBkWin(WM_MESSAGE* pMsg);     //半透明背景
void NotAlphaBk_cbBkWin(WM_MESSAGE* pMsg);  //不透明背景
void None_cbBkWin(WM_MESSAGE* pMsg);        //不做处理
void Update_Public_ErrPrompt_Btn(void);
void Disable_Public_ErrPrompt_Btn(void);
void Enable_Public_ErrPrompt_Btn(void);

Panel_LED_Status_e Panel_LED_Status_By_ErrorCode(ErrorCode_e eErrorCode);
//
WM_HWIN CreateDialogPage(void);

/******************** Login menu *****************************/
WM_HWIN CreateLogin_Window(void);
WM_HWIN Login_SelfCheck_CreateWindow(void);

/******************** PowerOff menu *****************************/
WM_HWIN PowerOff_CreateWindow(void);
//WM_HWIN Ensure_PowerOff_Window(void);

/******************** Analysis menu *****************************/
void Patient_Info_Init(void);
void Display_Result_Info(char *pBuffer, char Index, __IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);
void WBCHGB_Result_Btn_Callback(WM_MESSAGE* pMsg);
void WBC_Histogram_TEXT_Callback(WM_MESSAGE* pMsg);
void WBCHGB_ErrorPrompt_Text_Callback(WM_MESSAGE* pMsg);
void TestInfo_TEXT_Callback(WM_MESSAGE* pMsg);
WM_HWIN Analysis_Menu(void);
WM_HWIN CreateNextSample_Window(void);
WM_HWIN CreatePatient_Info_Window(void);
WM_HWIN Createmenu_Window(void);
WM_HWIN ErrorInfo_CreateWindow(void);
WM_HWIN ReCount_PromptDLG(void);
WM_HWIN CountingPrompt_CreateWindow(void);
WM_HWIN CountFail_CreateWindow(void);
WM_HWIN Printer_Error_Prompt_PromptDLG(void);
WM_HWIN NSec_OutIn_In_WindowDLG(void);
WM_HWIN Confirm_Takeout_Cell_WindowDLG(void);
void Display_Result_Info(char *pBuffer, char Index, __IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);
void QC_Display_Result_Info(char *pBuffer, char Index, __IO QC_WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);

/******************** ListView menu *****************************/
WM_HWIN CreateListReview_Window(void);
WM_HWIN ResultInfo_Menu(void);
WM_HWIN EditResult_CreateWindow(void);
FeedBack_e ListView_Send_Msg(uint32_t ulCmd, WM_MESSAGE *pMsg);
extern uint8_t g_ucListView_DataIndex;
__IO WBCHGB_TestInfo_t* ListView_Current_WBCHGB_Info(void);


/******************** Set menu *********************************/
WM_HWIN CreateSetPage(void);
WM_HWIN Calibration_CreateWindow(void);
WM_HWIN Createlab_infoPage(void);
WM_HWIN CreatedataTimeSetPage(void);
WM_HWIN CreateParaSetPage(void);

/******************** Service menu *********************************/
WM_HWIN CreateservicePage(void);
WM_HWIN Version_Info_CreateWindow(void);
/********************qc menu*****************************/
WM_HWIN QC_WindowDlG(void);
WM_HWIN QC_Analysis_Menu(void);
WM_HWIN QC_List_CreateWindow(void);
WM_HWIN QC_Set_CreateWindow(void);
void Dis_Ctrol_Judge(__IO QC_WBCHGB_TestInfo_t *ptQC_WBCHGB_TestInfo);

/*
*  以下三个按键，需做"长按状态"的显示效果处理
*/
typedef enum {
    BTN_ANALYSIS    = 0,
    BTN_LISTREVIEW  = 1,
    BTN_MENU        = 2,
    BTN_END         = 3,

}BtnInde_e;
void Public_Reset_Btn_Status(BtnInde_e eIndex);

/********************others*****************************/
void Set_Cur_MenuID(uint32_t ulID);
uint32_t Get_Cur_MenuID(void);
void GIF_Guide_cbBkWin(WM_MESSAGE* pMsg);
WM_HWIN ErrorPrompt_Window(void);
//void ErrorPrompt_cbBkWin(WM_MESSAGE* pMsg);
const char* ErrorCode_Prompt_Info(ErrorCode_e eErrorCode);
WM_HWIN CreateSwitchDialogPage(void);

void AbortPublishDestopButton(void);
void RecoverPublishDestopButton(void);


#endif

