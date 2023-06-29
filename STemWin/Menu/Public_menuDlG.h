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



extern __IO WM_HWIN g_hActiveWin;  //��ǰ����ھ��


/********************** ���� ***************************/
extern GUI_FONT SIF_HZ12_SONGTI; 
extern GUI_FONT SIF_HZ16_SONGTI; 
extern GUI_FONT SIF_HZ24_SONGTI; 

extern GUI_CONST_STORAGE GUI_FONT GUI_FontHZ_SongTi_24_4A;
//extern GUI_CONST_STORAGE GUI_FONT GUI_FontHZ_SongTi_12_4A;

#define HZ_SONGTI_12    SIF_HZ12_SONGTI 
#define HZ_SONGTI_16    SIF_HZ16_SONGTI 
#define HZ_SONGTI_24    GUI_FontHZ_SongTi_24_4A 

/********************** ����ID ***************************/
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
#define GUI_WINDOW_COUNTING_PROMPT        (GUI_ID_USER + 0x0C0) //�������ڷ�����
#define GUI_WINDOW_COUNT_FAIL_ID          (GUI_ID_USER + 0x0D0) //����ʧ����ʾ����
#define GUI_WINDOW_N_SEC_OUTIN_IN_ID      (GUI_ID_USER + 0x0E0) //N����ʾ�Զ�����
#define GUI_WINDOW_PRINTER_ERROR_PORMPT   (GUI_ID_USER + 0x0F0) //��ӡ������ʾ����
#define GUI_WINDOW_CONFIRMT_TAKEOUT_CELL  (GUI_ID_USER + 0x100) //��ȷ�ϼ��ģ���Ƿ�ȡ������
#define GUI_WINDOW_SELECT_YES_NO_ID       (GUI_ID_USER + 0x110) //yes or no ѡ����ʾ����
#define PAGE_ID_INPUT_USER_NUM		       (GUI_ID_USER + 0x120) //���뻼�߱�Ž���
#define PAGE_ID_PATIENT_LIST		       (GUI_ID_USER + 0x130) //���������б����

//list review 
#define GUI_WINDOW_LIST_REVIEW_ID         (GUI_ID_USER + 0x140)
#define GUI_WINDOW_RESULT_INFO_ID         (GUI_ID_USER + 0x150)   //�б�ع��н���������ID
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

/************  WM_MESSAGE, �Զ�����Ϣ  *************************************/
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
#define WM_FAIL_OUTPUT_GUIDE_GIF                (WM_USER + 0x19)   //����GIF�;�̬����
#define WM_FAIL_OUTPUT_GUIDE_GIF2               (WM_USER + 0x20)  //ֻ����GIF����

//����������ʾ
#define WM_ANALYSIS_ERROR_PROMPT                (WM_USER + 0x30)
#define WM_ANALYSIS_NORMAL_PROMPT               (WM_USER + 0x31)
#define WM_ANALYSIS_GZ_AUTO_MODE_UPDATE_DATA    (WM_USER + 0x32) //��װ�źţ��Զ�����ģʽ
#define WM_ANALYSIS_GIF_OPERATE_OUTIN_KEY       (WM_USER + 0x33) //�ɽ����ּ��������˳���������GIF
#define WM_ANALYSIS_GIF_OPERATE_COUNT_KEY       (WM_USER + 0x34) //�ɼ������������˳���������GIF
#define WM_ANALYSISI_ERROR_COUNT_NOT_ALLOW      (WM_USER + 0x35) //�����Լ���ڹ��ϣ�Ӱ�������������������ϣ��ټ���
#define WM_ANALYSIS_LOAT_DATA_FAIL              (WM_USER + 0x36)
//����ˢ��
#define WM_MENU_UPDATE                          (WM_USER + 0x45)

//�����̨֪ͨǰ̨����������ݸ�����ʾ��WM MESSAGE ID
#define SERVICE_AGING_USER_ID                   (WM_USER+1)                //����-->����ϻ������û�����IDֵ

//��¼�Լ�
#define WM_LOGIN_SELFCHECK_FINISH               (WM_USER + 0x50)

//��ӡ��
#define WM_PRINTER_NO_PAPER                     (WM_USER + 0x60)    //��ӡ��ȱֽ
#define WM_PRINTER_CONN_ERR                     (WM_USER + 0x61)    //��ӡ�������쳣

//����--�ϻ�����
#define WM_SERVICE_AGING_TEST_BE_OVER           (WM_USER + 0x70)    //�����ϻ����ִ����Ϻ�֪ͨ������Ϣ

//PCBA--ִ����ϣ�����ִ����һ�ֲ�����Ϣ
#define WM_PCBA_TEST_BE_OVER           			(WM_USER + 0x71)    //PCBA���Ժ��ִ����ϣ�����ִ����һ�ֲ��Ե�֪ͨ������Ϣ

//���� HGB Debug
#define WM_SET_HGB_BLACK_AUTO_ADJUST            (WM_USER + 0x80)    //HGB�հ��Զ�У׼
#define WM_SET_HGB_GZ_AUTO_ADJUST               (WM_USER + 0x81)    //HGB��װ�Զ�У׼
#define WM_SET_HGB_TEST                         (WM_USER + 0x82)    //HGB����

//�ʼ����
#define WM_ZJ_TEST_OVER	                         (WM_USER + 0x90)    //�ʼ�����ز������



//**************************���͸����öԻ�������ݽṹ��*********************************
typedef WM_HWIN WM_WINDOW_CREATE(void);
void Motor_Error_Prompt(WM_MESSAGE *pMsg, WM_WINDOW_CREATE *pWindowCreate);
/***
*���ڱ�ʾ�����Ի�����Ҫ���ص�ͼƬö��
***/
typedef enum _DialogBmpType_e{
	BMP_ALARM = 0,						//���أ�ͼ�꣬��ɫ����
	BMP_OK,								//���ء�
	BMP_SELF_CHECK,						//�����Լ���ͼ��
    
}DialogBmpType_e;


/***
*���ڱ�ʾ�����Ի������Ƿ���ʾȷ�ϰ���
***/
typedef enum _DialogConfimButt_e{
	NO_CONFIM_BUTTON = 0,
	HAS_CONFIM_BUTTON,
}DialogConfimButt_e;


/***
*���͸������Ի�������ݽṹ��
***/
typedef struct _CommonDialogPageData_s{
	DialogBmpType_e bmpType;					
	DialogConfimButt_e confimButt;
	WM_HWIN (*fun)(void);									//���ڱ�����öԻ����Դ���ڵĴ���������ڣ�����ָ��
	
	char str[100];
}CommonDialogPageData_s;


/***
*���ڹ���ѡ��Ի������ֵ��ȷ�ϼ���Ҫִ�е���Ŀö��
***/
typedef enum _SwitchDialogItem_e{
	CONFIG_USERMANAGE_RECOVER_FACTORY_PSW = 0,								//����--�û�����--�ָ���������
	CONFIG_LAB_INFO_CHANGE_SN,												//����--ʵ������Ϣ--�޸��������к�
	CONFIG_DEL_USER,														//����--�û�����--ɾ���û�
	CONFIG_CHANGE_DATA_NUM,													//����--ϵͳ����--�л�������������
	CONFIG_U_UPDATE,														//����--��������--U������
	
	SERVICE_DATAMANAGE_RESET_PARA,											//����--���ݹ���--������������
	SERVICE_DATAMANAGE_DEL_TEST_DATA,										//����--���ݹ���--ɾ����������
	SERVICE_DATAMANAGE_DEL_USER_DATA,										//����--���ݹ���--ɾ���û�����
	SERVICE_DATAMANAGE_RECOVER_FACTORY,										//����--���ݹ���--�ָ���������
    //
    LISTVIEW_DELETE_DATA,                                                   //�б�ع˺ͽ�����飬��ɾ������
    MENU_POWER_OFF_PROMPT,                                                  //�˵����棬�Ƿ�ػ���ʾ
    MENU_POWER_OFF_TAKEOUT_CELL,                                            //�˵����棬ִ���Ƿ�ػ�ǰ����ʾ����ȡ��Ѫϸ������ģ��
    MENU_LOGOUT_PROMPT,                                                     //�˵����棬�Ƿ�ע����ʾ
    
	LOGIN_RESET_MACHINE_PARA,												//��¼�����У�������¼��������������������
	
	//��������
	ANALYSE_NET_ERR_MUNAUL_INPUT_INFO,										//������������������������Ϣʱ�����쳣��ѡ���Ƿ��ֶ�¼�뻼����Ϣ
}SwitchDialogItem_e;



//**************************���͸�����ѡ��Ի�������ݽṹ��*********************************
typedef struct _CommonSwitchDialogPageData_s{
	SwitchDialogItem_e switchDialogItem;
	CommonDialogPageData_s dialogData;
	
	uint32_t u32;
}CommonSwitchDialogPageData_s;



/************  Color����ɫ  *************************************/
//button
// 
#define GUI_BUTTON_ROUND_DISABLED_COLOR		        0x00f3f1ec
//
//
#define BTN_RELEASE_GRAY    0x00c9c9c9
#define BTN_PRESS_GRAY    0x00b4b4b4
#define BTN_CHAR_COLOR    GUI_WHITE
#define BTN_FRAME_COLOR   0X00BEBEBE              //��ť�߿���ɫ

#define GUI_BTN_DISABLE_COLOR       0x00f3f1ec
#define GUI_BTN_PRESS_CHAR_COLOR    0x00ffffff
#define GUI_BTN_RELEASE_CHAR_COLOR  0x00333333
//Text
#define GUI_TEXT_FRAME_COLOR        0x00cbcbcb //���hui
#define GUI_TEXT_FRAME_COLOR        0x00cbcbcb
//edit
#define EDIT_FRAME_COLOR            0x00a2a09e
#define EDIT_ROUNDED_RADIUS          4              //�༭���Բ�ǰ뾶
//
#define GUI_SPLINE_LINE_COLOR       0x00efefef    //�ָ�����ɫ
//downdrop
#define DROPDOWN_FRAME_COLOR        0x00a2a09e    //�����߿��ʼ����ɫ
#define DROPDOWN_UNSEL_COLOR        GUI_BLACK     //ûѡ��Ԫ�ص���ɫ
#define DROPDOWN_SELFOCUS_COLOR     0x00EEDFB2    //ѡ��Ԫ�ص���ɫ0X00BEBEBE
#define DROPDOWN_DEFAULT_COLOR      GUI_WHITE    //ѡ��Ԫ�ص���ɫ
#define DROPDOWN_ROUNDED_RADIO      3            //Բ����
#define DROPDOWN_SPACE_LEN          5           //Ԫ�ؼ�ļ��
// dropdown color
#define NEXT_SAMPLE_DROPDOWN_UNSEL_COLOR        GUI_BLACK               
#define NEXT_SAMPLE_DROPDOWN_SELFOCUS_COLOR     0x00EEDFB2
#define NEXT_SAMPLE_DROPDOWN_FRAME_INIT_COLOR   0X00a2a09e               //�����߿��ʼ����ɫ
#define NEXT_SAMPLE_DROPDOWN_FRAME_CLICK_COLOR  0x00EEDFB2               //�����߿�����ɫ


/************* ͼ�� ****************************/
//
#define USE_BMP_PICTURE     1 
#if USE_BMP_PICTURE
/***** BMP *****/
extern GUI_CONST_STORAGE GUI_BITMAP bm_logo_x1;                         // ��˾��logo
//extern const unsigned short _aclogo_x2[12018UL + 1];                  // ��˾��logo*2 �����Ŵ�

//
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_prev_btn_black;  //��һ��¼����ͷͼ�꣬��ɫ
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_prev_btn_gray;   //��һ��¼����ͷͼ�꣬��ɫ
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_prev_btn_Blue;   //��һ��¼����ͷͼ�꣨���ף�
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_next_btn_black;  //��һ��¼����ͷͼ��,��ɫ
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_next_btn_gray;   //��һ��¼����ͷͼ��,��ɫ
extern GUI_CONST_STORAGE GUI_BITMAP bm_analysis_record_next_btn_Blue;   //��һ��¼����ͷͼ��(����)
extern GUI_CONST_STORAGE GUI_BITMAP bm_PullDown_ARROW;                  //������ͷ��������һ�����ȣ�dropdown
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_btn_active;                 //�˵�������״��
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_btn_idle;                   //�˵�������״̬
//
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_analysis;               //�˵�����������
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_list;                   //�˵����б�ع�
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_qc;                     //�˵����ʿ�
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_service;                //�˵�������ͼ��
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_set;                    //�˵�������ͼ��
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_logout;                    //�˵���ע��
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_poweroff;               //�˵����ػ�
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_user;                   //�˵����û�ͷ��
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_btn_arrow;              //�˵�����ͷ
extern GUI_CONST_STORAGE GUI_BITMAP bm_menu_btn_close;                //�˵����˳��˵�
//
extern GUI_CONST_STORAGE GUI_BITMAP bm_list_btn_idle;                //�б�عˣ�����״̬
extern GUI_CONST_STORAGE GUI_BITMAP bm_list_btn_active;                //�б�عˣ�����״̬
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_Up;                //�б�عˣ����ϵ���ͷ
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_Up_Blue;				//�б�عˣ����ϵ���ͷ�����ף�
extern GUI_CONST_STORAGE GUI_BITMAP bmListReview_Up_Gray;				//�б�عˣ����ϵ���ͷ���ҵף�
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_UpUp;                //�б�عˣ�����˫��ͷ
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_UpUp_Blue;			//�б�عˣ�����˫��ͷ(����)
extern GUI_CONST_STORAGE GUI_BITMAP bmListReview_UpUp_Gray;				//�б�عˣ�����˫��ͷ(�ҵ�)
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_Down;                //�б�عˣ����µ���ͷ
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_Down_Blue;        //�б�عˣ����µ���ͷ(����)
extern GUI_CONST_STORAGE GUI_BITMAP bmListReview_Down_Gray;			//�б�عˣ����µ���ͷ(�ҵ�)
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_DownDown;         //�б�عˣ�����˫��ͷ
extern GUI_CONST_STORAGE GUI_BITMAP bm_ListReview_DownDown_Blue;    //�б�عˣ�����˫��ͷ(����)
extern GUI_CONST_STORAGE GUI_BITMAP bmListReview_DD_Gray;			//�б�عˣ�����˫��ͷ(�ҵ�)

//extern const unsigned short _acErrorPrompt_OK[2056UL + 1];        //������ʾ���Ժ�
extern GUI_CONST_STORAGE GUI_BITMAP bm_circle_error_prompt;         //������ʾ����̾��, Բ��
extern GUI_CONST_STORAGE GUI_BITMAP bm_triangle_error_prompt;       //������ʾ����̾�ţ�������

#else

/***** PNG *****/
extern const unsigned char _aclogo[2560UL + 1];                    // ��˾��logo
extern const unsigned char _aclogo_x2[12018UL + 1];       // ��˾��logo*2 �����Ŵ�
extern const unsigned char _acPullDown_ARROW[1062UL + 1];        //������ͷ������dropdown
//
extern const unsigned char _acanalysis_btn_active[381UL + 1];    //��������������״̬
extern const unsigned char _acanalysis_btn_idle[1289UL + 1];    //��������������״̬
extern const unsigned char _aclist_btn_active[1248UL + 1];        //��������������״̬
extern const unsigned char _acanalysis_fault[1136UL + 1];        //����������������ʾ������ͼ��
extern const unsigned char _acanalysis_record_next_btn[1033UL + 1];    //��һ��¼����ͷͼ��
extern const unsigned char _acanalysis_record_prev_btn[1039UL + 1];    //��һ��¼����ͷͼ��
//
extern const unsigned char _acmenu_btn_active[1039UL + 1];        //�˵�������״��
extern const unsigned char _acmenu_btn_idle[1070UL + 1];        //�˵�������״��
extern const unsigned char _acmenu_setup[1289UL + 1];            //�˵�������ͼ��
extern const unsigned char _acmenu_service[1036UL + 1];            //�˵�������ͼ��
extern const unsigned char _acmenu_setup[1289UL + 1];            //�˵�������ͼ��
extern const unsigned char _acmenu_power[1379UL + 1];            //�˵�����Դ
extern const unsigned char _acmenu_logout[1204UL + 1];            //�˵����˳�
//
extern const unsigned char _aclist_btn_active[1248UL + 1];        //�б�عˣ�����״̬
extern const unsigned char _aclist_btn_idle[1303UL + 1];        //�б�عˣ�����״̬
extern const unsigned char _ac_ListReview_Up[1066UL + 1];        //�б�عˣ����ϵ���ͷ
extern const unsigned char _ac_ListReview_UpUp[1087UL + 1];        //�б�عˣ�����˫��ͷ
extern const unsigned char _ac_ListReview_Down[1355UL + 1];        //�б�عˣ����µ���ͷ
extern const unsigned char _ac_ListReview_DownDown[1414UL + 1]; //�б�عˣ�����˫��ͷ
//
extern const unsigned char _acErrorPrompt_OK[2056UL + 1];       //������ʾ���Ժ�
extern const unsigned char _acErrorPrompt_Fail_c[2236UL + 1];   //������ʾ����̾��, Բ��
extern const unsigned char _acErrorPrompt_Fail_s[1201UL + 1];    //������ʾ����̾�ţ�������

#endif

typedef enum{
    //��������
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

//    //�˵�
//    LOGO_menu_analysis                  = 0x21,
//    LOGO_menu_list                      = 0x22,
//    LOGO_menu_qc                        = 0x23,
//    LOGO_menu_service                   = 0x24,
//    LOGO_menu_set                       = 0x25,
//    LOGO_menu_btn_logout                = 0x26,
//    LOGO_menu_poweroff                  = 0x27,
//    LOGO_menu_btn_user                  = 0x2a,     //�û�ͷ��
//    LOGO_menu_btn_close                 = 0x2b,     //�رղ˵�����
//    LOGO_menu_arrow                     = 0x2c,     //�رղ˵�����

//    //�б�ع�
//    LOGO_list_btn_idle                  = 0x40,
//    LOGO_list_btn_active                = 0x41,
//    LOGO_circle_error_prompt            = 0x4e,
//    LOGO_riangle_error_prompt           = 0x4f,
	AAA,
}Logo_e;

//��ʾ��תΪ���ݵ�Сͼ��
void Draw_Const_Logos(Logo_e eLogo, uint16_t usX, uint16_t usY);



/*
*   ��ӡ����������
*/
typedef enum {
    PRINTER_NO_PAPER    = 1,
    PRINTER_CONN_ERROR  = 2,
    PRINTER_END         = 3,
} PrinterErrorType_e;


/************* ��������Ӣ�Ķ��� ****************************/
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

//��������
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

/***********�б�ع�*****************************************************/
extern const char g_ucaLng_ResultInfo[LANGUAGE_END][13];
extern const char g_ucaLng_ReFreshNet[LANGUAGE_END][20];
extern const char g_ucaLng_Edit[LANGUAGE_END][7];
extern const char g_ucaLng_Return[LANGUAGE_END][7];
extern const char g_ucaLng_IfDeleteData[LANGUAGE_END][16];
extern const char g_ucaLng_WaitForRefresh[LANGUAGE_END][30];
extern const char g_ucaLng_RefreshNetOK[LANGUAGE_END][30];
extern const char g_ucaLng_RefreshNetErr[LANGUAGE_END][35];

/***********����*********************************************************/
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

/***********����*********************************************************/
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





/***********��¼/�ػ�*********************************************************/
extern const char g_ucaLng_Login_SelfCheck[LANGUAGE_END][35];
extern const char g_ucaLng_PowerOff_Prompt[LANGUAGE_END][50];
extern const char g_ucaLng_Selfcheck_Err[LANGUAGE_END][25];
    
/***********����*********************************************************/
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

/*********** �ʿ� *********************************************************/
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
/***********�������Ӧ������*********************************************************/

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


#define PUBLIC_ALPHA_COLOR                 0x00525a4e              //������ɫ
#define PUBLIC_ALPHA_VALUE                 30                      //͸��ֵ

//����API    
/******************** Public menu *****************************/
void Public_cbBkWin(WM_MESSAGE* pMsg);
void Public_Msg_Init(void);
void Public_Hide_Widget(void);
void Public_Show_Widget(void);
void Public_Dropdown_Callback(WM_MESSAGE *pMsg);
void Public_Edit_Callback(WM_MESSAGE *pMsg);
void Public_FrameBK(GUI_RECT *ptRect, uint16_t usX, uint16_t usY);
void Window_Alpha(WM_MESSAGE* pMsg, GUI_RECT *ptRect);  //����͸��������
void AlphaBk_cbBkWin(WM_MESSAGE* pMsg);     //��͸������
void NotAlphaBk_cbBkWin(WM_MESSAGE* pMsg);  //��͸������
void None_cbBkWin(WM_MESSAGE* pMsg);        //��������
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
*  ������������������"����״̬"����ʾЧ������
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

