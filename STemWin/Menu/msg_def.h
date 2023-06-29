#ifndef __MSG_DEF_H__
#define __MSG_DEF_H__


#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "main.h"
#include "serviceItems.h"
#include "CAT_iotTask.h"
#include "bsp_outin.h"


/*
***************  analysis cmd  **************************
*/
#define CMD_ANALYSIS                   	 0x1
#define CMD_ANALYSIS_LAST_RECORD         0x1000
#define CMD_ANALYSIS_NEXT_RECORD         0x1001
#define CMD_ANALYSIS_NEXT_SAMPLE       	 0x1002
#define CMD_ANALYSIS_LAST_PAGE           0x1003
#define CMD_ANALYSIS_NEXT_PACE           0x1004
#define CMD_ANALYSIS_PRINT          	 0x1005
#define CMD_ANALYSIS_UPDATE_PATIENT_INFO 0x1006 //���¡������޸ĺ�Ĳ�����Ϣ
//
#define CMD_ANALYSIS_COUNT_MODE_AUTO     0x1019		//�������ⲿ��װ�źŷ���ģ�����,���ź�,�Զ�ִ��ģʽ
#define CMD_ANALYSIS_COUNT				 0x1020		//����
#define CMD_ANALYSIS_MOTOR_OUTIN	     0x1030		//�����ֵ��
#define CMD_ANALYSIS_MOTOR_CALAMP  		 0x1031		//���ӵ��
//
#define CMD_ANALYSIS_PROMPT_COUNTING     0x1040		//UI��ʾ�������������ڽ����У����Ե�...

#define CMD_ANALYSIS_LOAT_DATA_FAIL      0x1050     //����������������ʧ��

#define CMD_ANALYSIS_INPUT_USER_NUM      0x1060     //�������뻼�߱�Ž���
#define CMD_ANALYSIS_REQUEST_USER_NUM      0x1061   //����������ָ�����߱�ŵĻ�����Ϣ��

/*
***************  list review  **************************
*/
#define CMD_LIST                        0x3
#define CMD_LIST_REVIEW_INIT            0x3000  //��ʼ�����б�ع���ʾ����
#define CMD_LIST_REVIEW_MENU_OUT        0x3001  //���˳��б�ع�ʱ������µ�ǰ����
#define CMD_LIST_REVIEW_SYNC            0x3002  //�����������ɵ��µ�����ͬ�����б�ع�����

#define CMD_LIST_REVIEW_EDIT            0x3010  //�༭
#define CMD_LIST_REVIEW_DELETE          0x3011  //ɾ��
#define CMD_LIST_REVIEW_PRINT           0x3012  //��ӡ
#define CMD_LIST_REVIEW_LAST_RECORD     0x3013  //��һ��
#define CMD_LIST_REVIEW_NEXT_RECORD     0x3014  //��һ��
#define CMD_LIST_REVIEW_LAST_PAGE       0x3015  //��һҳ
#define CMD_LIST_REVIEW_NEXT_PAGE       0x3016  //��һҳ

//
#define CMD_LIST_REVIEW_LOAT_DATA_FAIL  0x3020     //�б�ع˼�������ʧ��

/*
***************  set  cmd **************************
*/
#define CMD_SET                         0x5
#define CMD_SET_SYSTEM                  0x5000
#define CMD_SET_USER_MANAGE             0x5100
#define CMD_SET_ASSIST                  0x5200
#define CMD_SET_PRINT                   0x5300
#define CMD_SET_LAB_INFO                0x5400
#define CMD_SET_PARAMETER               0x5500
#define CMD_SET_CALIBRATION             0x5600  //WBC HGBУ׼
#define CMD_SET_DEBUG                   0x5700  //����->����,ͨ������
#define CMD_SET_SN_WAIT_CAT				0x5800	//������SN��ȴ��������Ӧ
#define CMD_CHANGE_SN_WAIT_CAT			0x5900	//�޸�SN��ȴ��������Ӧ
#define CMD_SET_SWITCH_LOGO             0x5920  //�л�LOGO���л���ͬ�Ŀ��ػ��͵�¼ͼƬ
#define CMD_SET_BUILD_PRESSURE          0x5930  //��ѹ��-30Kpa
#define CMD_SET_UPDATE_LOGIN_PICTURE    0x5940  //ϵͳ�����У����л����Ժ󣬸��¼��ص�¼����ͼƬ��SDRAM��
#define CMD_SET_U_UPDATE			    0x5950  //U������
#define CMD_LIST_REVIEW_REFRESH_NET     0x5960  //�ֶ�ˢ������

/*
***************  service cmd **************************
*/
#define CMD_SERVER                                  0x7
#define CMD_SERVER_PRESS                            0x7000
#define CMD_SERVER_AIR_LIGHT                        0x7100
#define CMD_SERVER_MOTOR                            0x7200
#define CMD_SERVER_STATUS                           0x7300

#define CMD_SERVER_INTERACTION                      0x7400
#define CMD_SERVER_INTERACTION_PRINTER	            0x7401
#define CMD_SERVER_INTERACTION_SCREEN_BOARD         0x7402

#define CMD_SERVER_OTHERS                           0x7500
#define CMD_SERVER_COMPONENT_AGING                  0x7600
#define CMD_SERVER_LOG                              0x7700
#define CMD_SERVER_VERSION                          0x7800
#define CMD_SERVER_DISK                             0x7900

#define CMD_SERVER_DATA_MANAGE                      0x7A00
#define CMD_SERVER_DEL_TEST_DATA        			0x7A01				//ɾ����������
#define CMD_SERVER_DEL_USER_DATA        			0x7A02				//ɾ���û�����
#define CMD_SERVER_RECOVER_FACTORY        			0x7A03				//�ָ���������
#define CMD_SERVER_CAT_RESTART	        			0x7A04				//����CAT

#define CMD_SERVER_AGING		                    0x7B00
#define CMD_SERVER_TANZHEN_TEST         			0x7B01				//̽���ϻ�����
#define CMD_SERVER_AGING_TEST         				0x7B02				//�����ϻ�����
#define CMD_SERVER_OUTIN_MODULE_OUT_IN  			0x7B03  			//������ģ�飬�����˶�
#define CMD_SERVER_QIZUI_TEST			 			0x7B04  			//�����ϻ�����
#define CMD_SERVER_AIR_VALVE_TEST			 		0x7B05  			//�����ϻ�����
#define CMD_SERVER_YEFA_VALVE_TEST			 		0x7B06  			//Һ���ϻ�����
#define CMD_SERVER_BUILD_PRESSURE_TEST			 	0x7B07  			//��ѹ�ϻ�����
#define CMD_SERVER_DL_ELEC_TEST			 			0x7B08  			//�����缫�ϻ�����
#define CMD_SERVER_CAT_DATA_TEST			 		0x7B09  			//CAT���ݲ���
#define CMD_SERVER_RAND_TOUCH_TEST			 		0x7B0a  			//����������
#define CMD_SERVER_SHENGCHAN_AGING			 		0x7B0b  			//�����ϻ�����
#define CMD_SERVER_DATA_SAVE_TEST			 		0x7B0c  			//���ݴ洢����

#define CMD_SERVER_OTHER_CHECK                      0x7C00				//�����Լ���
#define CMD_SERVER_OTHER_CHECK_PUMP        			0x7C01				//���Լ���
#define CMD_SERVER_OTHER_CHECK_BEEP        			0x7C02				//�������Լ���
#define CMD_SERVER_OTHER_CHECK_AIR_VALVE   			0x7C03				//�����Լ���
#define CMD_SERVER_OTHER_CHECK_YE_VALVE    			0x7C04				//Һ���Լ���
#define CMD_SERVER_OTHER_CHECK_HGB        			0x7C05				//HGB�Լ���

#define CMD_SERVER_PCBA_CHECK                      	0x7D00				//PCBA������
#define CMD_SERVER_PCBA_CHECK_MAIN                  0x7D01				//���ذ������
#define CMD_SERVER_PCBA_CHECK_ZJ                    0x7D02				//ת�Ӱ������
#define CMD_SERVER_PCBA_CHECK_CAT                   0x7D03				//CAT������
#define CMD_SERVER_PCBA_CHECK_TOUCH                 0x7D04				//������������
#define CMD_SERVER_PCBA_CHECK_MOTO                  0x7D05				//���������
#define CMD_SERVER_PCBA_CHECK_PRESURE               0x7D06				//ѹ��������������
#define CMD_SERVER_PCBA_CHECK_VALVE		            0x7D07				//��������
#define CMD_SERVER_PCBA_CHECK_PUMP		            0x7D08				//�ü�����
#define CMD_SERVER_PCBA_CHECK_HONE		            0x7D09				//HONEYWELL������
#define CMD_SERVER_PCBA_CHECK_IT110		            0x7D0A				//IT110������
#define CMD_SERVER_PCBA_PRESSURE_ADJUST	            0x7D0B				//PCBAѹ��У׼


//
#define CMD_SET_HGB_BLACK_AUTO_ADJUST              	0x7E01				//HGB�հ������Զ�У׼
#define CMD_SET_HGB_GZ_AUTO_ADJUST              	0x7E02				//HGB��װ�����Զ�У׼
#define CMD_SET_HGB_TEST                      	    0x7E03				//HGB����
#define CMD_SET_HGB_ADJUST_TEST                     0x7E04				//HGB��У����HGB����
#define CMD_SET_HGB_ADJUST_OPEN415                  0x7E05				//HGB��У���濪415
#define CMD_SET_HGB_ADJUST_OPEN535                  0x7E06				//HGB��У���濪535
#define CMD_SET_HGB_ADJUST_CLOSE                  	0x7E07				//HGB��У����ص�
#define CMD_SET_HGB_ADJUST_KQ_AUTO                  0x7E08				//HGB��У��������Զ�У׼
#define CMD_SET_HGB_ADJUST_KB_AUTO                  0x7E09				//HGB��У����հ��Զ�У׼

/*
***************  login cmd **************************
*/
#define CMD_LOGIN                                   0x9   
#define CMD_LOGIN_SELFCHECK                         0x9000   //��¼�Լ�
#define CMD_LOGIN_ERROR_CLEAR                       0x9001   //��¼�Լ�󣬹������
#define CMD_LOGIN_OK_LOAD_USER_DATA                 0x9005   //�˺ŵ�¼У����ɺ󣬷�����Ϣ����˼����û�����
#define CMD_LOGIN_LOAD_USER_DATA_FINISH             0x9006   //�˺ŵ�¼У����ɺ󣬼����û�����

/*
***************  poweroff cmd **************************
*/
#define CMD_POWEROFF                                0xA   
#define CMD_POWEROFF_EXEC                           0xA001   //�ػ�


/*
***************  other cmd **************************
*/
//��ӡ��
#define CMD_OTHERS_PRITER_NO_PAPER                  0xC020   //��ӡ��ȱֽ
#define CMD_OTHERS_PRITER_CONN_ERR                  0xC021   //��ӡ���Ӵ���

#define CMD_OTHERS                                  0xC
#define CMD_OTHERS_OUTIN_MODULE_OUT                 0xC010   //������ģ�飬����
#define CMD_OTHERS_OUTIN_MODULE_IN                  0xC011   //������ģ�飬����
//������
#define CMD_OTHERS_OUTIN_IN			                0xC030   //����
#define CMD_OTHERS_OUTIN_OUT			            0xC031   //����
#define CMD_OTHERS_START_COUNT			            0xC032   //��������






/*
***************  QC cmd **************************
*/
#define CMD_QC                                      0xD
#define CMD_QC_ANALYSIS_COUNT			            0xD021  //�ʿؼ���
#define CMD_QC_ANALYSIS_COUNT_MODE_AUTO             0xD022  //�ʿؼ��������ź�,�Զ�ִ��ģʽ
#define CMD_QC_UPDATE_ANALYSIS_DATA                 0xD023  //�ʿط��������µ�ǰ��ʾ���б�����
//
#define CMD_QC_LIST_LAST_RECORD                     0xD002  //�ʿ��б���һ��¼
#define CMD_QC_LIST_NEXT_RECORD                     0xD003  //�ʿ��б���һ��¼
#define CMD_QC_LIST_DELETE                          0xD004  //�ʿ��б�ɾ��
#define CMD_QC_UPDATE_LIST_DATA                     0xD005  //�ʿأ����µ�ǰ��ʾ���б�����
//
#define CMD_QC_PRINT          	                    0xD008  //�ʿط�������ӡ
#define CMD_QC_SET_RE_READ_QC_INFO         	        0xD009  //�ʿ��趨�����¶�ȡ���ʿ��趨����
#define CMD_QC_SET_SAVE         	                0xD010  //�ʿ��趨�������ʿ��趨����
#define CMD_QC_SET_DELETE         	                0xD011  //�ʿ��趨��ɾ��ָ�����ʿ��趨����
//
#define CMD_QC_LOAD_ANALYSIS_LIST_DATA              0xD015  //�ʿأ������ʿ������������б�����
//#define CMD_QC_VALID_FILE_NUM           	        0xD016  //�ʿأ���������Ч���ļ���
//#define CMD_QC_FILENUM_CAPACITY_IS_FULL    	        0xD017  //�ʿأ��ļ��Ŷ��ڵ���������������

// ��С���ȶ���
#define UI_TO_BACKEND_MSG_BUFFER_LEN  	256		//UI���͸������Ϣ��С
#define BACKEND_TO_UI_MSG_BUFFER_LEN  	WBCHGB_TEST_INFO_LEN	//UI���͸������Ϣ��С
#define GRAPH_DATA_LEN  				256		//ֱ��ͼ���ݴ�С
#define DATE_TIME_LEN	  				25		//����ʱ�䳤��
#define SAMPLE_SN_LEN	  				10		//�����������
#define PATIENT_NAME_LEN	  		    22		//������������
#define PATIENT_BIRTHE_DAY_MAX_LEN	  	20		//���˳�������
#define REMARK_LEN	  					91		//��ע��Ϣ����
//#define DIAGNOSIS_INFO_LEN	  		50		//�����Ϣ����
#define WBC_PROMPT_LEN	  				161		//WBC��ʾ��Ϣ����
#define HGB_PROMPT_LEN	  				97		//HGB��ʾ��Ϣ����
#define SERIAL_NUM_LEN	  				10		//ϵ�к�תΪ���鳤��
#define QC_LOT_NUM_LEN                  15      //�ʿأ����ų���


/*
*   ������
*/
typedef enum {
    ERROR_CODE_SUCCESS                  = 0000,    //�ɹ�
    ERROR_CODE_FAILURE                  = 0001,    //ʧ��
    
    ERROR_CODE_BUILD_PRESS              = 1011,    //������ѹ�쳣
    ERROR_CODE_AIR_TIGHT                = 1021,    //ϵͳ�������쳣
//  ERROR_CODE_PRESS_TOO_LOW            = 1025,    //������ѹ������(<-22kpa)
//  ERROR_CODE_PRESS_TOO_HIGH           = 1026,    //������ѹ�����ߣ�>-33kpa�� 
    ERROR_CODE_BC_CURRENT               = 1031,    //����Դ�쳣
    ERROR_CODE_POWER_12VP               = 1032,    //��12V, �����쳣
    ERROR_CODE_POWER_12VN               = 1033,    //��12V, �����쳣
    ERROR_CODE_OUTIN_OUT                = 1041,    //�����ֵ���������쳣
    ERROR_CODE_OUTIN_IN                 = 1051,    //�����ֵ���������쳣
    ERROR_CODE_OUTIN_OC                 = 1055,    //����ģ�飬�����쳣
    //
    ERROR_CODE_CLAMP_OUT                = 1061,    //�Խӵ�����Խ��쳣
    ERROR_CODE_CLAMP_IN                 = 1071,    //�Խӵ�����ɿ��쳣  
    ERROR_CODE_CLAMP_OC                 = 1075,    //�Խ�ģ�飬�����쳣 
    //
    ERROR_CODE_NOT_AT_POS               = 1081,    //δ���ֵ��¼���ʧ��
    ERROR_CODE_ELEC_TRIGGLE             = 1091,    //��ʼ���׶Σ������缫�쳣����
    ERROR_CODE_ELEC_TRIGGLE_2           = 1092,    //Һ·�������ź��ȶ���⡢�źŲɼ��������Ⱥ����׶Σ������缫�쳣����
//  ERROR_CODE_ELEC_TRIGGLE_M1          = 1093,    //�����缫�쳣�������ȶ����쳣����1
//  ERROR_CODE_ELEC_TRIGGLE_M2          = 1094,    //�����缫�쳣�������ȶ����쳣����2
//  ERROR_CODE_ELEC_TRIGGLE_M3          = 1095,    //�����缫�쳣�������ȶ����쳣����3
//  ERROR_CODE_ELEC_TRIGGLE_M4          = 1096,    //�����缫�쳣�������ȶ����쳣����4
//  ERROR_CODE_ELEC_TRIGGLE_M5          = 1097,    //�����缫�쳣�������ȶ����쳣����5

    /*** usr operator err type ***/
    ERROR_CODE_WBC_ELEC_TOUCH           = 1101,    //�����缫�Ӵ��쳣   
    //
    ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE = 1111,    //��ʼ���׶Σ�С�׵�ѹ��ͨ
    ERROR_CODE_BSK_2S_15V               = 1112,    //Һ·�����׶Σ�С�׵�ѹС��1.5V
    ERROR_CODE_BSK_XKV_TOO_LOW          = 1113,    //С�׵�ѹС��0.60V
    //
//  ERROR_CODE_BSK                      = 1114,    //���ݲɼ������رշ�2ǰ��С�׵�ѹС��0.65V
//  ERROR_CODE_BSK                      = 1115,    //Һ·�����׶Σ�С�׵�ѹ����0.79v                                           
    ERROR_CODE_AIR_LEAK                 = 1121,    //����©��
    //
    ERROR_CODE_SAMPLE_NONE              = 1123,    //δ����������� 
    ERROR_CODE_AIR_LEAK_COUNTING_1      = 1124,    //���������У�©����ѹ������-25kpa,����ظ���ѹ3�Σ���3���ڶ����缫��������
    ERROR_CODE_AIR_LEAK_COUNTING_2      = 1125,    //���������У�©����ѹ������-25kpa������ظ���ѹ3�Σ���3���ڶ����缫û����������  
    
    /*** material err type ***/
//    ERROR_CODE_TIMEOUT_10_BEFORE        = 1131,    //��ʱ,ǰ10�루ǰʮ�뷢�����¿ף�
//    ERROR_CODE_TIMEOUT_10_AFTER         = 1132,    //��ʱ,��10�루��ʮ�뷢�����¿ף�
    ERROR_CODE_TIMEOUT                  = 1131,      //��ʱ���������ʱ�䣬δ�����¿ף�
    ERROR_CODE_T4_ERR_15S_2_MAXTIME     = 1132,      //T4ʱ���쳣��������15s, ����С��������ʱ�䣩
    ERROR_CODE_RE_CONN_2_T4_15S         = 1133,      //Һ·�����׶��ضԽӴ���>=2, ��T4>15s 
        
    /*** usr operator err type ***/
    ERROR_CODE_SAMPLE_NOT_ENOUGH        = 1141,    //�������������� , 
    ERROR_CODE_NONE_CELL                = 1151,    //δ����Ѫϸ�����ģ��
    /*** Machine error type ***/
    ERROR_CODE_TEMPERATURE              = 1161,    //�¶��쳣  
    ERROR_CODE_EEPROM                   = 1171,    //EEPROM�쳣  
    ERROR_CODE_CAT_ONE                  = 1181,    //CAT1ģ���쳣  
    ERROR_CODE_HGB_MODULE               = 1191,    //HGBģ���쳣
    ERROR_CODE_DISK_CAPACITY            = 1201,    //������������
    ERROR_CODE_SPI_FLASH_CONN_ERR       = 1202,    //SPI Flashͨ���쳣
    ERROR_CODE_FLASH_DATA_CALIBRATE_ERR = 1203,    //SPI Flash���ݶ�дУ��ʧ��
    ERROR_CODE_CELL_CHECK_MODULE        = 1205,    //������ģ���쳣  
    ERROR_CODE_RESULT_ASTERISK          = 1231,    //������Ǻ�     
    
    /*** Machine error type ***/
//    ERROR_CODE_ADD_PRESS                = 2041,    //ѹ������ֵ��һ��
//    ERROR_CODE_ADD_MOTOR_STEP           = 2051,    //�������ֵ��һ��
//    ERROR_CODE_ADD_BC_GAIN              = 2061,    //bc���油��ֵ��һ��
//    ERROR_CODE_PUMP                     = 2101,    //��ռ�ձ�ֵ��һ��
    ERROR_CODE_PRINT_NO_PAPER           = 2071,    //��ӡ��ȱֽ
//  ERROR_CODE_TANZHEN_SIGNAL           = 2121,    //̽��Ӵ��쳣
    ERROR_CODE_NO_PRINTER               = 2081,    //��ӡ��δ����
    ERROR_CODE_MONITOR_SIGNAL_ERR       = 2091,    //�����缫�Ӵ��쳣���ź��ȶ��ȼ��׶Σ�	
    //Algo    
    ERROR_CODE_ALGO_MODE                = 3000,    //�㷨�ж�����ģʽ�쳣
    ERROR_CODE_ALGO_BREAK               = 3001,    //�㷨�˳�����
    //QC Errorcode
    ERROR_CODE_INVALID_FILENUM          = 4000,     //�ʿأ���Ч���ļ���
    ERROR_CODE_FILENUM_CAPACITY_IS_FULL = 4001,     //�ʿأ��ļ����´洢��������
    
    
    //��Щ������ֻ���ڣ���/��һ��¼״̬�ķ���
//    ERROR_CODE_NO_LAST_RECORD      					= 9001,   //����һ�����ݣ��������������鿴��һ�����������ǵ�һ������������һ�����ݣ�
//    ERROR_CODE_NO_NEXT_RECORD      					= 9002,   //����һ�����ݣ��������������鿴��һ�����������ǵ�һ������������һ�����ݣ�
} ErrorCode_e;



/*
*   ���������ͣ��������ͣ�
*/
typedef enum {
    ERROR_CODE_TYPE_NORMAL              = 0,    //�޴�������
    ERROR_CODE_TYPE_USER 	            = 1,    //�û���������
	ERROR_CODE_TYPE_MACHINE             = 2,	//��������
	ERROR_CODE_TYPE_MATERIAL            = 3,	//�ĲĹ���
    ERROR_CODE_TYPE_MULTI               = 4,    //�ۺϹ���
    
//	ERROR_CODE_TYPE_USER_MACHINE		= 4,    //�û��������� �� ��������
//    ERROR_CODE_TYPE_USER_MATERIAL       = 5,    //�û��������� �� �ĲĹ���
//    ERROR_CODE_TYPE_MACHIHNE_MATERIAL   = 6,    //�������� �� �ĲĹ���
    ERROR_CODE_TYPE_END                 = 5,
} ErrorType_e;







/*
* 	��Ϣ���ͷ���
*/
typedef enum {
	INNERMSMG_DIR_UI_TO_BACKEND = 0,
	INNERMSMG_DIR_BACKEND_TO_UI = 0,
	
} InnerMsg_Dir_e;




/********************* �������� ***************************************************/

/*
* 	����ģʽ
*/
typedef enum {
	TEST_MODE_WBC_HGB = 0,
	TEST_MODE_WBC_HGB_RBC_PLT = 1,
	
} TestMode_e;


/***
*���嵱ǰʹ�������Ĺ�˾����ö�٣�ODM��˾��ר�е�UI��Ƥ��ɫ��Ӧ������ͳһ��ΪԤ��
***/
typedef enum _Company_e{
	COMPANY_CREATE_CARE = 0,			//����
	COMPANY_JIN_RUI,					//����
	COMPANY_YSB,						//ҩʦ��
	COMPANY_HY,                    		//��ӳ
	COMPANY_YU_LIU4,                    //Ԥ��4
	COMPANY_YU_LIU5,                    //Ԥ��5
	COMPANY_YU_LIU6,                    //Ԥ��6
	COMPANY_YU_LIU7,                    //Ԥ��7
	
	COMPANY_END
}Company_e;


/*
* 	�Ա�
*/
typedef enum {
    SEX_TYPE_NONE       = 0,        //��
	SEX_TYPE_MALE		= 1,		//��
	SEX_TYPE_FEMALE		= 2,		//Ů
	SEX_TYPE_END		= 3,
    
} SexType_e;

typedef enum {
	REFER_GROUP_CMOMMON		= 0,	//ͨ��
	REFER_GROUP_MALE		= 1,	//������ʿ
	REFER_GROUP_FEMALE		= 2,	//����Ůʿ
	REFER_GROUP_CHILDREN	= 3,	//��ͯ
	REFER_GROUP_BOBY		= 4,	//������
    REF_GROUPS_END          = 5,
} ReferGroup_e;








/*
*	WBC HGB �����
*/
typedef enum {
	WBCHGB_RST_WBC		= 0,
	WBCHGB_RST_GranJ	= 1,
	WBCHGB_RST_MidJ		= 2,
	WBCHGB_RST_LymJ		= 3,
	WBCHGB_RST_GranB	= 4,
	WBCHGB_RST_MidB		= 5,
	WBCHGB_RST_LymB		= 6,
	WBCHGB_RST_HGB		= 7,
	WBCHGB_RST_END		= 8,
	
} WBCHGB_Rst_e;


/*
*	 RBC �����
*/
typedef enum {
	RBC_RST_RBC		= 0,
	RBC_RST_HCT		= 1,
	RBC_RST_MCV		= 2,
	RBC_RST_MCH		= 3,
	RBC_RST_MCHC	= 4,
	RBC_RST_RDW_CV	= 5,
	RBC_RST_RDW_SD	= 6,
	RBC_RST_END		= 7,
    
} RBC_Rst_e;



/*
*	 PLT �����
*/
typedef enum {
	PLT_RST_PLT		= 0,
	PLT_RST_MPV		= 1,
	PLT_RST_PDW		= 2,
	PLT_RST_PCT		= 3,
	PLT_RST_P_LCR	= 4,
	PLT_RST_P_LCC	= 5,
	PLT_RST_END		= 6,
    
} PLT_Rst_e;


/*
	���� ��λ
*/
typedef enum {
	AGE_UNIT_YEAR	= 0,	//��
	AGE_UNIT_MONTH	= 1,	//��
	AGE_UNIT_WEEK	= 2,	//��
	AGE_UNIT_DAY	= 3,	//��
	AGE_UNIT_END	= 4,
    
} AgeUnit_e;



/*
*   �ʿأ��ļ��ű��������0-5����ǰ�����������6���ļ���ţ�
*/
typedef enum {
    QC_FILENUM_INDEX_0      = 0,
    QC_FILENUM_INDEX_1      = 1,
    QC_FILENUM_INDEX_2      = 2,
    QC_FILENUM_INDEX_3      = 3,
    QC_FILENUM_INDEX_4      = 4,
    QC_FILENUM_INDEX_5      = 5,
    QC_FILENUM_INDEX_END    = 6,   
    
} QC_FILENUM_INDEX_e;


/*
*   �ʿ�Ʒ��ˮƽ/Ũ�ȣ��ߡ��С��ͣ�
*/
typedef enum {
    QC_LEVEL_LOW    = 0,
    QC_LEVEL_MIDDLE = 1,
    QC_LEVEL_HIGH   = 2,
    QC_LEVEL_END    = 3,
    
} QC_LEVEL_e;






/*
*	�ڲ���Ϣͷ
*/
typedef __packed struct {
  //InnerMsg_Dir_e   ucDir;         //��Ϣ����0��UI������ˣ�1����˵�UI
	uint16_t  		usCmd;          //���
	ErrorCode_e  	eErrorCode;	    //��Ϣ�쳣״̬
	ErrorType_e		errType;		//�쳣����
	uint16_t  		usMsgLen;       //��Ϣ���ȣ�usMsgLen�ֶ�֮���������Ϣ�ֶγ���֮��
    
}Msg_Head_t;
#define MSG_HEAD_LEN	sizeof(Msg_Head_t)



/*
*********************  analysis msg define  ********************************
*/

/*
*	��һ��¼
*/
typedef  __packed struct{
	Msg_Head_t 	tMsgHead;						        //��Ϣͷ
	uint32_t			ulCurDataIndex;			        //��ǰ���Խ��ϵ�кţ�
    QC_FILENUM_INDEX_e  eFileNumIndex;                  //�����ʿ��б���ǰ���ļ���
//	TestMode_e 			eTestMode;						//��ǰ����ģʽ WBC+HGB
		
} LastRecord_t, NextRecord_t, QC_Record_t;


///*
//*	��һ��¼
//*/
//typedef  __packed struct {
//	Msg_Head_t 	tMsgHead;						        //��Ϣͷ
//	uint32_t			ulCurDataIndex;			        //��ǰ���Խ��ϵ�к�
//    QC_FILENUM_INDEX_e  eFileNumIndex;                  //�����ʿ��б���ǰ���ļ���
////	TestMode_e 			eTestMode;						//��ǰ����ģʽ WBC+HGB

//} NextRecord_t;




/*
*	��ӡ
*/
typedef __packed struct{
	Msg_Head_t		tMsgHead;						//��Ϣͷ
	uint32_t		ulCurDataIndex;					//��ǰ���Խ��ϵ�кţ��Զ�����,�������ݹ���
    QC_FILENUM_INDEX_e  eFileNumIndex;               //�����ʿ��б���ǰ���ļ���
} Print_t;



#define PATIENT_ID_MAX_LEN			20				//���߱���ַ�����󳤶ȣ��������'\0'
#define ORDER_NUM_MAX_LEN			20				//��������ַ�����󳤶ȣ��������'\0'


/*
*	��һ��������, ��Ϣ�ṹ�壬��λ�����Ϣλ��
*/
typedef  __packed struct{
	uint8_t 			validFlag;									//��һ��������Ϣ�Ƿ���Ч��Ϊ0��ʾ��Ч��Ϊ1��ʾ��Ч
	uint32_t			ulNextDataSN;								//��һ����ϵ�кţ��ڵ�ǰϵ�к�����һ
	uint8_t 			ucaNextSampleSN[SAMPLE_SN_LEN];				//��һ�������,�ڵ�ǰ�������һ,"CBC-0000N"
	char 				patientID[PATIENT_ID_MAX_LEN];				//���߱���ַ���
	uint8_t 			ucaName[PATIENT_NAME_LEN];					//����
	SexType_e 			eSex;										//�Ա�
	uint8_t 			ucAge;										//����
	AgeUnit_e			eAgeUnit;									//���䵥λ
	uint8_t				ucaBirthDay[PATIENT_BIRTHE_DAY_MAX_LEN];	//��������
	ReferGroup_e  		eReferGroup;								//�ο���		
	TestMode_e 			eTestMode;									//��ǰ����ģʽ WBC+HGB
	uint8_t		  		ucaRemark[REMARK_LEN];						//��ע
	char 				orderNum[ORDER_NUM_MAX_LEN];				//�������
} NextSample_t;








// WBC HGB ���Խ����Ϣ,״̬
typedef enum {
	DATA_STATUS_INIT		        = 0,    //��ʼ�������WBCHGB_TestInfo, taWBCHGB_Data��Ĭ������״̬��������Ч�����ݣ�����ʾ��UI��
    DATA_STATUS_NORMAL		        = 1,    //����״̬����ʾ��UI��
	DATA_STATUS_DELETE		        = 2,	//��ɾ����������ɾ��������ʾ��UI��
    DATA_STATUS_COUNT_INVALID       = 3,    //������Ч���ݣ�1�����������£���õ����ݣ���ʾ��UI��
	DATA_STATUS_CHANGE				= 5,	//��ʾ�����޸Ĺ�
	DATA_STATUS_END			        = 6,
	
} DataStatus_e;


//WBC HGB����������־�� ����/����ֵ�Ż����ƫ��/ƫС�жϣ���Чֵ��*
typedef enum{
    WBCHGB_ALERT_NORMAL         = 0x00,  //����
    WBCHGB_ALERT_SMALL          = 0x01,  //ƫС��
    WBCHGB_ALERT_BIG            = 0x02,  //ƫ���
    WBCHGB_ALERT_REVIEW         = 0x10,  //����ֵ ?
    WBCHGB_ALERT_REVIEW_SMALL   = 0x11,  //����ֵ ?����ƫС��
    WBCHGB_ALERT_REVIEW_BIG     = 0x12,  //����ֵ ?����ƫ���

    WBCHGB_ALERT_INVALID        = 0xFF,  //��Чֵ����* 
    WBCHGB_ALERT_END            = 0,

}WBCHGB_ALERT_e;






/*
***************  list review msg define  **************************
*/

#define LISTVIEW_MAX_ITEM_NUM			7

/*
*   �б�ع˽��棬������Ϣ�ṹ��
*/
typedef struct {
    Msg_Head_t  tMsgHead;
    uint8_t     ucIndex;     //ListReview_DataTable��Ӧ���±�
    uint32_t    ulSN;        //�б�ع��У����������ݶ��ڵ�SN��

}ListReview_t;





/*********************** ���ý��� **************************************/



/*
*	HGB �Զ�У׼��Ϣ�ṹ��
*/
typedef __packed struct {
     Msg_Head_t tMsgHead;			//��Ϣͷ
     uint8_t  ucHGB_DR;             //�Զ�У׼��HGB����ֵ
     uint16_t current535;     //�Զ�У׼��HGB��������
     uint16_t usHGB_ADC_Avg;        //HGB��ǰУ׼ʱ����ȡ��HGB ADC��ƽ��ֵ
    
} HGB_AutoAdjust_t;

 




 


 

 

 

/*
*   ����
*/
typedef enum {
	LANGUAGE_CHINESE = 0,			//����
	LANGUAGE_ENGLISH = 1,			//Ӣ��
	LANGUAGE_END     = 2,
    
} Language_e;
 
 
 
 
#define ACCOUNT_MAX_NUM									10				//����˻�����
#define ACCOUNT_USER_NAME_MAX_LEN						10				//�û����ַ���󳤶�
#define ACCOUNT_PSW_MAX_LEN								15				//�����ַ���󳤶�


/***
*��ͬ�˺Ų������������ͣ������ں�̨���豸�˶����ݹ���
****/
typedef enum _AccountDataType_e{
	ACCOUNT_DATA_TYPE_INNER = 0,		//�ڲ��˺ţ������˺ţ�����ʦ�˺ţ�����������
    ACCOUNT_DATA_TYPE_OUTSIDE,  		//�ⲿ�˺ţ���ͨ�˺ţ�����Ա�˺ţ�����������
	ACCOUNT_DATA_TYPE_END
}AccountDataType_e;


/***
*�˻�����ö��
***/
typedef enum _AccountType_e{
	ACCOUNT_TYPE_SUPER = 0,											//��������Ա�˺�
	ACCOUNT_TYPE_ADMIN,    											//����Ա�˺�
	ACCOUNT_TYPE_NORMAL,											//��ͨ�˺�
	ACCOUNT_TYPE_NEW												//�����˺�
}AccountType_e;


/***
*�˻���Ϣ�ṹ��
***/
typedef __packed struct _AccountInfo_s{
	char user[ACCOUNT_USER_NAME_MAX_LEN];							//�˻���
	char psw[ACCOUNT_PSW_MAX_LEN];									//����
	uint8_t authority;												//Ȩ��ֵ
	AccountType_e type;												//�˻�����
	AccountDataType_e dataType;										//�˻���Ӧ����������
}AccountInfo_s;






/*
*	 ������Ϣ�ṹ��
*/
typedef __packed struct{
	Msg_Head_t 			tMsgHead;								//��Ϣͷ
	uint32_t			ulCurDataIndex;							//��ǰ���Խ��ϵ�кţ��Զ�����,�������ݹ���
	uint8_t 			ucaSampleSN[SAMPLE_SN_LEN];				//��ǰ�������,�Զ�����,"CBC-0001"
	char 				patientID[PATIENT_ID_MAX_LEN];			//���߱���ַ���
	uint8_t 			ucaName[PATIENT_NAME_LEN];				//����
	SexType_e			eSex;									//�Ա�
	uint8_t 			ucAge;									//����
    AgeUnit_e			eAgeUnit;								//���䵥λ
	ReferGroup_e  		eReferGroup;							//�ο���		
	TestMode_e 			eTestMode;								//��ǰ����ģʽ WBC+HGB
	AccountType_e	    eAccountType;							//������
    uint8_t				ucaDateTime[DATE_TIME_LEN];				//ʱ��
    uint8_t			    ucaBirthDay[PATIENT_BIRTHE_DAY_MAX_LEN];//��������
	uint8_t		 		ucaRemark[REMARK_LEN];					//��ע
	char 				orderNum[ORDER_NUM_MAX_LEN];			//������ţ����ֹ�˾����ҩʦ�����Ҫ���ֶ�
} Patient_Info_t;



/*
***************  login msg define  **************************
*/

/*
*	��¼�Լ췵�أ���Ϣ
*/
#define MAX_ERROR_CODE_NUM        14                //һ�����������������������Ŀ������һ�£���
typedef __packed struct{
	Msg_Head_t		tMsgHead;						//��Ϣͷ
	//
    uint8_t         ucProgress;                     //�����Լ���ȣ�0-100��
    uint8_t         ucErrorNum;                     //����ĸ���
    ErrorCode_e     eaErrorCodeBuffer[MAX_ERROR_CODE_NUM];//�����루����Ҳ���ж��������ucErrorNum���������ң�
	
} Login_SelfCheck_t;


 
/*
*   ��ӡ��ʽ
*/
typedef enum {
	PRINTF_WAY_AUTO				= 0, 	//�Զ���ӡ
	PRINTF_WAY_MANUAL			= 1, 	//�ֶ���ӡ
	PRINTF_WAY_END				= 2,
	 
 } PrintWay_e;

 
 
 
 

 

  
 
/*********************** ������� **************************************/
 
  /*
 *	�汾��Ϣ �ṹ�壬������ֵ101��Ӧ����ʾ�汾Ϊ��V1.0.1��Ӳ���汾����
 �ַ���A01��Ӧ����ʾ�汾Ϊ���ַ���A.01
 */
 #define  CLINIC_NAME_MAX_LEN					50

#define RL_FLASH_FS_NAND_DRV_PATH               "N0:"
#define RL_FLASH_FS_NAND_FORMAT                 "/LL /L Nand /FAT16"
#define ACCOUNT_DATA_TYPE_OUTSIDE_PATH          "\\0"                 //�ⲿ�˺Ų������ݱ����·��
#define ACCOUNT_DATA_TYPE_INNER_PATH            "\\1"                 //�ڲ��˺Ų������ݱ����·��
#define DATA_DIR_PATH                           "\\Data\\"     //���ݴ洢·��
#define LOG_DIR_PATH                            "\\Log\\"       //��־�洢·��
#define HEAD_DIR_PATH                           "\\Head\\"       //��־�洢·��
/* ���� */
#define OTHERS_DIR_PATH                         "\\Others\\"      //������������־���ʿأ��洢·��

/* ͼƬ */
#define PICTURE_DIR_PATH                         "\\Picture\\"          //ͼƬ�洢·��
/* �������� ���ͼƬ*/
#define ANALYSIS_PICTURE_DIR_PATH                "\\Picture\\Analysis\\"  //��������ͼƬ�洢·��
#define OTHERS_PICTURE_DIR_PATH                 "\\Picture\\Others\\"  //����ͼƬ�洢·��
/*���*/
#define SOFTWARE_DIR_PATH                       "\\Software\\"   //����洢·��
#define FONT_DIR_PATH                           "\\Font\\"       //����洢·��
#define QC_PICTURE_DIR_PATH                     "\\Picture\\QC\\"
#define SET_PICTURE_DIR_PATH                    "\\Picture\\Set\\"
#define SERVICE_PICTURE_DIR_PATH                "\\Picture\\Service\\"


//�����ļ��洢���������
#define MAX_DATA_NUM_ONE_FILE                   250
//�ļ�������
#define FILE_NAME_MAX_LEN                       64

/*
*	 �����������ݡ���־��Ϣ���������������ݡ���־��ͨ��DataManage_t�������ݺ���־��ϵ�к�һһ��Ӧ��
*/
#define	DATA_FILE_RECORD_NUM			        MAX_DATA_NUM_ONE_FILE			//ÿ�������ļ��洢���ݵ�����
#define	DATA_INFO_LEN					        WBCHGB_TEST_INFO_LEN		//sizeof(WBCHGB_TestInfo_T)ÿ�����ݼ�¼�Ľ����С��
#define DATA_FILE_PREFIX				        "Data_"     //�����ļ���ǰ׺
#define DATA_FILE_SUFFIX				        ".txt"      //�����ļ�����׺


#define	LOG_FILE_RECORD_NUM				        MAX_DATA_NUM_ONE_FILE			//ÿ����־�ļ��洢������
#define	LOG_INFO_MAX_LEN					    4096		                    //ÿ����־���4k��
#define LOG_BUFFER_MAX_LEN                      (LOG_INFO_MAX_LEN-4)            //��־buffer��󳤶ȣ���־��Ϣ�а��������ȣ�crc��buffer������ο� CountLog_t �ṹ��
#define LOG_FILE_PREFIX					        "Log_"      //��־�ļ���ǰ׺
#define LOG_FILE_SUFFIX					        ".txt"      //��־�ļ�����׺


#define	HEAD_FILE_RECORD_NUM				    MAX_DATA_NUM_ONE_FILE	        //ÿ�����������ļ��洢������
#define	HEAD_INFO_LEN					        sizeof(CountHead_t)		    //ÿ��������Ϣ�Ĵ�С��
#define HEAD_FILE_PREFIX					    "Head_"      //��־�ļ���ǰ׺
#define HEAD_FILE_SUFFIX					    ".txt"      //��־�ļ�����׺


#define LOG_RUN_HEAD_FILE_PREFIX		        "RunLog"  //������־ͷ�ļ���ǰ׺
#define LOG_RUN_STR_FILE_PREFIX			        ".txt"  //������־�ַ����ļ���ǰ׺

#define SAMPLE_SN_PREFIX                         "CBC-"        //�������ǰ׺

/*
*   ��Щ����ͼƬ��ͼƬ�д������ֵģ�һ�㶼����Ӣ�İ汾��Ӣ�İ汾���ļ����������İ��ϼ��� "en_"��ǰ׺
*/
//��������
#define PICTURE_GIF_OPERATE                      "Operate.gif"       //������������
#define PICTURE_GIF_INPUT                        "Input.gif"         //���������
#define PICTURE_GIF_OK_OUTPUT                    "OkOutput.gif"     //�����ɹ������ó�������
#define PICTURE_GIF_FAIL_OUTPUT2                 "FailOutput2.gif"  //����ʧ�ܣ����ó������أ�ֻ�ж������֣�
//
#define ANALYSIS_PICTURE_BMP_COUNT_FAIL           "CountFail.bmp"            //����������Ч
#define ANALYSIS_PICTURE_BMP_COUNTING             "Counting.bmp"             //�������ڷ�����...
#define ANALYSIS_PICTURE_BMP_RE_COUNT             "ReCount.bmp"             //�Ƿ����²���
#define ANALYSIS_PICTURE_BMP_N_SEC_OUTIN_IN       "NSecOutinIn.bmp"       //N����Զ�����
#define ANALYSIS_PICTURE_BMP_CONFIRM_TAKEOUT      "ConfirmTakeout.bmp"     //��ȷ�ϼ��ģ���Ƿ�ȡ��
//printer
#define PICTURE_BMP_PRITER_NO_PAPER               "PrinterNoPaper.bmp"
#define PICTURE_BMP_PRITER_CONN_ERROR             "PrinterConnError.bmp"
//������������ͼ
#define PICTURE_ANALYSIS_WARN_BLUE               "WarnBlue.bmp"
#define PICTURE_ANALYSIS_WARN_WHILE              "WarnWhite.bmp"
//#define PICTURE_ANALYSIS_WAITING                 "Waiting.bmp"

/* ���� ��ͼ */
#define PICTURE_OTHERS_SELFCHECK                "SelfCheck.bmp"

extern const char gc_ucaPicture_PowerOnOff[COMPANY_END][20];
extern const char gc_ucaPicture_Login[COMPANY_END][20];
extern const char gc_ucaPicture_SelfCheck[COMPANY_END][20];


#define PICTURE_OTHERS_FRAME_BK                 "FrameBk.bmp"
#define PICTURE_OTHERS_IconBkBig                "IconBkBig.bmp"
#define PICTURE_OTHERS_IconBkSmall              "IconBkSmall.bmp"

/* �ʿذ�ť���� ��ͼ */
#define PICTURE_QC_SET                          "QcSet.bmp"
#define PICTURE_QC_ANALYSIS                     "QcAnalysis.bmp"
#define PICTURE_QC_CHART                        "QcChart.bmp"
#define PICTURE_QC_LIST                         "QcList.bmp"
#define PICTURE_QC_ARROW                        "QcArrow.bmp"

/* ���ý��棬��ر��� ��ͼ */
#define PICTURE_SET_ChangeLogo                  "ChangeLog.bmp"
#define PICTURE_SET_FuZhu                       "FuZhu.bmp"
#define PICTURE_SET_FuZhuUnactive               "FuZhuUnactive.bmp"
#define PICTURE_SET_JiaoZhen                    "JiaoZhun.bmp"
#define PICTURE_SET_JiaoZhunGray                "JiaoZhunGray.bmp"
#define PICTURE_SET_LabInfo                     "LabInfo.bmp"
#define PICTURE_SET_Para                        "Para.bmp"
#define PICTURE_SET_ParaGray                    "ParaGray.bmp"
#define PICTURE_SET_Printer                     "Printer.bmp"
#define PICTURE_SET_SwitchClose                 "SwitchClose.bmp"
#define PICTURE_SET_SwitchOpen                  "SwitchOpen.bmp"
#define PICTURE_SET_Sys                         "Sys.bmp"
#define PICTURE_SET_Test                        "Test.bmp"
#define PICTURE_SET_TestGray                    "TestGray.bmp"
#define PICTURE_SET_User                        "User.bmp"

/* ������棬��ر��� ��ͼ */
#define PICTURE_SERVICE_AgingTestActive         "AgingTestActive.bmp"
#define PICTURE_SERVICE_AgingTestUnactive       "AgingTestUnactive.bmp"
#define PICTURE_SERVICE_AirLight                "AirLight.bmp"
#define PICTURE_SERVICE_AllLog                  "AllLog.bmp"
#define PICTURE_SERVICE_AllLogUnactive          "AllLogUnactive.bmp"
//#define PICTURE_SERVICE_Data                    "Data.bmp"
#define PICTURE_SERVICE_Disk                    "Disk.bmp"
#define PICTURE_SERVICE_ErrLog                  "ErrLog.bmp"
#define PICTURE_SERVICE_ErrLogUnactive          "ErrLogUnactive.bmp"
#define PICTURE_SERVICE_InterActionActive       "InterActionActive.bmp"
#define PICTURE_SERVICE_InterActionUnactive     "InterActionUnactive.bmp"
#define PICTURE_SERVICE_Moto                    "Moto.bmp"
#define PICTURE_SERVICE_Others                  "Others.bmp"
#define PICTURE_SERVICE_ParaLog                 "ParaLog.bmp"
#define PICTURE_SERVICE_ParaLogUnactive         "ParaLogUnactive.bmp"
#define PICTURE_SERVICE_Pressure                "Pressure.bmp"
#define PICTURE_SERVICE_StatusActive            "StatusActive.bmp"
#define PICTURE_SERVICE_StatusUnactive          "StatusUnactive.bmp"
#define PICTURE_SERVICE_Version                 "Version.bmp"
#define PICTURE_SERVICE_DataActive              "DataActive.bmp"
#define PICTURE_SERVICE_DataUnActive            "DataUnactive.bmp"
#define PICTURE_SERVICE_Err                     "Err.bmp"
#define PICTURE_SERVICE_Ok                      "Ok.bmp"
#define PICTURE_SERVICE_SelfCheck               "SelfCheck.bmp"

/* �ֿ� */
#define FONT_SIF_SONGTI_12                      "songti12.sif"
#define FONT_SIF_SONGTI_16                      "songti16.sif"
#define FONT_UNI2GBK							"UNI2GBK.bin"








/*
*	HGB ���Խ����Ϣ,  
*/
typedef __packed struct{
    Msg_Head_t tMsgHead;
//    HGB_Test_e eHGB_Test;                             //HGB���Խ���ģ�������Ŀ
    float       fHGB;                                   //HGB����ֵ
    float       fSrc_HGB;                               //HGB���, ԭʼֵ
    uint16_t    usHGB_ADC_SrcBuffer[10];                //��ǰ����HGB�������Ӧ��ԭʼADCֵ
    float       fDst_HGB;                               //HGB�����У׼ֵ
    uint16_t    usHGB_ADC_DstBuffer[10];                //��ǰ����HGB�������Ӧ��У׼ADCֵ
    uint8_t     ucHGB_RstAlert;                         //HGB���������Ϣ��0��
    uint8_t     ucaHGB_ErrorPrompt[HGB_PROMPT_LEN];		//HGB������ʾ
    
} HGB_Data_t;



/*
*	WBC HGB ���Խ����Ϣ,  �ý��������Ҫ���浽�ļ���
*/
#define INVALID_DATA_SN     0xFFFFFFFF


typedef __packed struct{	
	Msg_Head_t tMsgHead;								    //��Ϣͷ
	
	//���Խ��ϵ�кţ��Զ��������ò��������ڲ�ʹ�ã��������ֲ�ͬ���
	DataStatus_e	    eDataStatus;							//����״̬,
	uint32_t		    ulCurDataSN;							//��ǰ���Խ��ϵ�кţ��Զ�����,
    uint32_t		    ulCurDataIndex;							//��ǰ���Խ�������ݹ����е�������index
    uint8_t 		    ucaSampleSN[SAMPLE_SN_LEN];				//��ǰ�������,�Զ�����,"CBC-00001"
	char				patientID[PATIENT_ID_MAX_LEN];			//���߱���ַ���
	uint8_t 		    ucaName[PATIENT_NAME_LEN];				//����
    uint8_t			    ucaBirthDay[PATIENT_BIRTHE_DAY_MAX_LEN];//��������
	SexType_e		    eSex;									//�Ա�
	uint8_t 		    ucAge;									//����
    AgeUnit_e		    eAgeUnit;								//���䵥λ
	ReferGroup_e	    eReferGroup;							//�ο���		
	TestMode_e 		    eTestMode;								//��ǰ����ģʽ WBC+HGB
	AccountType_e	    eAccountType;							//�����˺�
	uint8_t			    ucaDateTime[DATE_TIME_LEN];				//ʱ��
	long 			    timestamp;								//ʱ���
    uint8_t		  	    ucaRemark[REMARK_LEN];					//��ע
	//
	float 			    fWBC_Histogram[GRAPH_DATA_LEN];			//ֱ��ͼ����
	float 			    fWBCHGB_RstData[WBCHGB_RST_END];		//WBC HGBֵ
	uint8_t             ucaWBCHGB_RstAlert[WBCHGB_RST_END];     //WBC HGB���������Ϣ��0��
    float 			    fLines[3];								//�����ָ���
	float 			    fWbcMaxPos;								//������ֵ
	uint8_t 		    ucaWBC_ErrorPrompt[WBC_PROMPT_LEN];		//WBC������ʾ
	uint8_t 		    ucaHGB_ErrorPrompt[HGB_PROMPT_LEN];		//HGB������ʾ
	char 				orderNum[ORDER_NUM_MAX_LEN];			//�������
    uint16_t            usCrc;                                  //crc16����У��ֵ����
    uint8_t             ucaEndFlag[2];                          //������־λ���̶�Ϊ"\r\n"�����ڷָ�ÿ�����Խ��
	
} WBCHGB_TestInfo_t;
#define WBCHGB_TEST_INFO_LEN			sizeof(WBCHGB_TestInfo_t)


/*
*   �ʿأ�������ڿ�״̬
*/
typedef enum {
    QC_CTRL_STATUS_IN     = 0,   //�ʿؽ���ڿ�
    QC_CTRL_STATUS_OUT    = 1,   //�ʿؽ�����ڿ�
    QC_CTRL_STATUS_END    = 2,
    
}QC_Ctrl_Status_e;

/*
*	�ʿأ�WBC HGB ���Խ����Ϣ,
*/
typedef __packed struct{	
	Msg_Head_t tMsgHead;								    //��Ϣͷ
	
	//���Խ��ϵ�кţ��Զ��������ò��������ڲ�ʹ�ã��������ֲ�ͬ���
//    ErrorCode_e       eHideErrorCode;                       //���ع�����
	DataStatus_e	    eDataStatus;							//����״̬,
	uint32_t		    ulCurDataSN;							//��ǰ���Խ��ϵ�кţ��Զ�����,
    uint32_t		    ulCurDataIndex;							//��ǰ���Խ�������ݹ����е�������index
    uint8_t 		    ucaSampleSN[SAMPLE_SN_LEN];				//��ǰ�������,�Զ�����,"CBC-00001"
	uint8_t 		    ucaName[PATIENT_NAME_LEN];				//����
    //
    uint32_t            ulLastValidIndex;                       //��һ����Ч�����ݵ�������(���µ�)
    uint32_t            ulNextValidIndex;                       //��һ����Ч�����ݵ������ţ��Ͼɵģ�
    QC_FILENUM_INDEX_e  eFileNumIndex;                          //�����ʿأ������������ļ���
    uint16_t            usFileNum;                              //�ļ��ţ�0-65535��
    uint16_t            usTestCapacity;                         //һ���ļ����£����ɲ��Դ�������ǰΪ100��
    uint8_t             ucaLotNum[QC_LOT_NUM_LEN];              //����
    QC_LEVEL_e          eQC_Level;                              //�ʿ�Ʒ��ˮƽ/�ܶ�
    AccountType_e       eSeter;                                 //��ǰ�ļ��Ŷ��ڵ��ʿ��趨��Ϣ�ģ�������
    AccountType_e       eOperator;                              //�����˺�(������)  
    uint8_t             ucaValidDate[DATE_TIME_LEN];            //�ʿ�Ʒ����Ч��
    QC_Ctrl_Status_e    eQC_Ctrl_Status;                        //�ʿأ��ڿ�״̬
	//
	uint8_t			    ucaDateTime[DATE_TIME_LEN];				//ʱ��
	long 			    timestamp;								//ʱ���
	//
	float 			    fWBC_Histogram[GRAPH_DATA_LEN];			//ֱ��ͼ����
	float 			    fWBCHGB_RstData[WBCHGB_RST_END];		//WBC HGBֵ
	uint8_t             ucaWBCHGB_RstAlert[WBCHGB_RST_END];     //WBC HGB���������Ϣ��0��
    float 			    fLines[3];								//�����ָ���
	float 			    fWbcMaxPos;								//������ֵ
	uint8_t 		    ucaWBC_ErrorPrompt[WBC_PROMPT_LEN];		//WBC������ʾ
	uint8_t 		    ucaHGB_ErrorPrompt[HGB_PROMPT_LEN];		//HGB������ʾ
    uint16_t            usCrc;                                  //crc16����У��ֵ����
    uint8_t             ucaEndFlag[2];                          //������־λ���̶�Ϊ"\r\n"�����ڷָ�ÿ�����Խ��
	
} QC_WBCHGB_TestInfo_t;
#define QC_WBCHGB_TEST_INFO_LEN			sizeof(QC_WBCHGB_TestInfo_t)


/************************************************************************************/
/*
*   �������ݺͼ�����־��״̬������Ϣ�ṹ��
*/
typedef __packed struct _CountHead_t{   
	ErrorCode_e  	eErrorCode;	                        //����������
    ErrorCode_e  	eHideErrorCode;	                    //���������ش�����
	ErrorType_e		errType;		                    //�����쳣����
    uint8_t         ucUseAlgoFlag;                      //�����㷨��־λ�������㷨�����ݱ㲻��Ϊ0�� 0��δ���ã�1������
    DataStatus_e    eStatus;                            //�������ݺ���־״̬,��ͬʱ�ı䣩   
	uint8_t 		ucaSampleSN[SAMPLE_SN_LEN];			//��ǰ�������,�Զ�����,"CBC-00001"
	AccountType_e	eAccountType;						//�����˺�
	uint8_t			ucaDateTime[DATE_TIME_LEN];			//����ʱ��
    //����SN��
    uint32_t		ulCurSN;							//��ǰ������SN�ţ� ��ϵ�кź�WBCHGB_TestInfo�е�����ϵ�кţ�ulCurDataSN������һһ��Ӧ
    uint32_t		ulCurIndex;							//��ǰ������������
    uint32_t        ulLastValidIndex;                   //��һ����Ч�����ݵ�������(�Ͼɵ�)��������ʱ��ֵΪ��INVALID_DATA_SN
    uint32_t        ulNextValidIndex;                   //��һ����Ч�����ݵ������ţ����µģ���������ʱ��ֵΪ��INVALID_DATA_SN
	
	uint8_t 		payingStatus;						//��������״̬��Ϣ��Ϊ0��ʾ��δ���Ϊ1��ʾ�Ѿ�����
    
    uint16_t        usCrc;                                //crc16����У��ֵ
    uint8_t         ucaEndFlag[2];                        //"\r\n"
   
} CountHead_t;
#define COUNT_HEAD_SIZE     sizeof(CountHead_t)



/***
*������־�ṹ��,  һ��4k
***/
typedef __packed struct{
    uint16_t usCrc;                                 //У��        
	uint16_t usLogLen;                              //��־����
    uint8_t ucaLogBuffer[LOG_BUFFER_MAX_LEN];       //��־buffer
    
}CountLog_t;


/***
*������־�ṹ��
***/
typedef __packed struct _CountLog_s{
	CountHead_t logHead;							//������־ͷ
	char Str[4096];							    //������־�ַ���
}CountLog_s;


/*
*������־ͷ
*/
typedef __packed struct {
	uint32_t		num;								//��ǰ������־�������к�
	
	char 			shortStr[30];						//��Ҫ��Ϣ
	AccountType_e	eAccountType;						//�˺�
	char			dataTime[25];						//ʱ��
	long 			timestamp;							//ʱ���
    uint16_t        logLen;                          	//��־���泤��
    uint32_t        logBufIndex;        
    //��Ӧ��־�ַ����ļ��е�ƫ����

}RunLogHead_t;



/***
*������־
***/
typedef __packed struct _RunLog_s{
	RunLogHead_t logHead;
	char Str[512];
    uint16_t usCrc;                                 //У��
    
}RunLog_s;

#define RUN_LOG_MAX_LEN     sizeof(RunLog_s);









/***
*����--���Թ��ܽ��������ע��˽�����������浽EEPROM��������������ʧЧ
***/
typedef __packed struct ParaSetDebugInfo_s{
	uint32_t countDelayTime;					    //�����ӳ�ʱ�䣬��λ��
	uint8_t jscReuseFlag;						//�������ظ�ʹ�ñ�־
	uint8_t gzTestWBCFlag;						//��װ����WBC��ʶ
	uint32_t gzTestCount;						//��װ����WBCִ�д���
    
}ParaSetDebugInfo_s;



/***
*����--���Թ��ܲ���,ͨ�Žṹ��
***/
typedef __packed struct{
    Msg_Head_t tMsgHead;						//��Ϣͷ
	ParaSetDebugInfo_s sParaSetDebugInfo;		//���Թ��ܲ���
    
}ParaSetDebugInfo_Msg_t;




/***
*�ʿ���Ϣ�ṹ��, (����ֵ����ʱ��Ϊfloat������ͳһ*100����uint32_t�洢)
***/
typedef __packed struct{  
	QC_FILENUM_INDEX_e 	eFileNumIndex;       		//�ļ��ű��������0-5����ǰ�����������6���ļ���ţ�
	uint16_t 	usFileNum;                      	//�ļ��ţ�0-65535��
	uint16_t 	usTestCapacity;                 	//һ���ļ����£����ɲ��Դ�������ǰΪ100��
	uint8_t 	ucaLotNum[QC_LOT_NUM_LEN];      	//����
	QC_LEVEL_e 	eQC_Level;                      	//�ʿ�Ʒ��ˮƽ/�ܶ�
	AccountType_e 	eSeter; 
	uint8_t 	ucaValidDate[DATE_TIME_LEN];    	//�ʿ�Ʒ����Ч��
	float 	faUpperLimit[WBCHGB_RST_END];   		//WBC HGB ���� ����
	float 	faLowerLimit[WBCHGB_RST_END];   		//WBC HGB ���� ����
}QC_SetInfo_s;


/*
*	QC�ʿ��趨��������Ϣ�ṹ�� 
*/
typedef struct {
    Msg_Head_t      tMsgHead;
    uint8_t         ucCurIndex;      //�ļ��б�ѡ�����Ӧ���±�
    uint8_t         ucMaxIndex;      //�ļ��б��Ӧ������±�
    QC_SetInfo_s    tQC_SetInfo;  //�����ģ�QC_SetInfo
    
}QC_SetInfo_Msg_t;


/*
*	QC�ʿأ�������Ϣ�ṹ�� 
*/
typedef struct {
    Msg_Head_t      tMsgHead;
    uint32_t        ulIndex;    //�ʿ����ݶ��ڴ洢�±� ���� �ļ��ŵ��±�
}QC_Msg_t;



//===����������ض���=====================================================
#define MACHINFO_INIT_FLAG								0xA55A0FF0
#define STRUCT_START_INIT_FLAG							0xA55A0FF0
					
#define SKIN_INIT_FLAG									0xA55A0FF0
#define DOWN_LOAD_INFO_FLAG								0xA55A0FF0

#define TOUCH_INIT_FLAG									0x5AA50FF0		//��ʾ���������Ѿ���ʼ������
#define TOUCH_FOCUS_FLAG								0xF00FA55A		//��ʾҪǿ�ƽ���У׼ģʽ
					
#define MQTT_IP_MAX_LEN									30
#define MQTT_NAME_MAX_LEN								30
#define MQTT_PSW_MAX_LEN								30


//�˻�Ȩ��ֵ	
#define ACCOUNT_SUPER_AUTHORITY							255				//��������ԱȨ��ֵ
#define ACCOUNT_ADMIN_AUTHORITY							100				//��ͨ����ԱȨ��ֵ
#define ACCOUNT_NORMAL_AUTHORITY						50				//��ͨ�û�Ȩ��ֵ
#define ACCOUNT_NEW_AUTHORITY							1				//�����û�Ȩ��ֵ

//�����˻�Ĭ������
#define ACCOUNT_SUPER_DEFAULT_PSW						"YongFu123"		//��������ԱĬ������ֵ
#define ACCOUNT_ADMIN_DEFAULT_PSW						"admin"			//����ԱĬ������ֵ
#define ACCOUNT_NORMAL_DEFAULT_PSW						"001"			//��ͨ�û�Ĭ������ֵ


/***
*��־�ṹ��
***/
typedef __packed struct{
	uint32_t	startFlag;										//���˱�־��ΪSTRUCT_START_INIT_FLAGʱ���Ҳ�Ϊ0xFFFFFFFFʱ��������ýṹ�����������б����ǣ�ֱ���������򣻵�Ϊ0xFFFFFFFFʱ��������������Ľṹ�壬��ֱ�ӳ�ʼ������
	uint16_t 	size;											//������¼�˽ṹ���ֽڳ��ȣ��������ӱ���ʱ�ܱ���֪�����Ա���±������г�ʼ��
	
	__packed struct{
		uint64_t	connNet:						1;			//Ϊ0��ʾ��δ��������Ϣ���͵�����ˣ�Ϊ1��ʾ�Ѿ���������Ϣ���͵������
		uint64_t	userTimestampInit:				1;			//Ϊ1��ʾ�Ѿ���ʼ�����û��״ο���ʱ���
		uint64_t	lock:							1;			//��ʾ�Ƿ���Ҫ������ǰ������Ϊ1��ʾ������Ϊ0��ʾ������
	}bit;
}MachInfoFlag_s;


/***
*��˾��������
***/
typedef __packed struct{
	uint32_t	startFlag;
	uint16_t 	size;

	Company_e	company;										//��ǰ��˾����

	//������MQTT���������
	__packed struct{
		char	ip[MQTT_IP_MAX_LEN];							//IP������
		char	name[MQTT_NAME_MAX_LEN];						//�û���
		char	psw[MQTT_PSW_MAX_LEN];							//����
	}mqtt;

	//Ƥ��ɫ����
	__packed struct{
		uint32_t	bg;											//����ɫ
		uint32_t	title;										//������ɫ
		uint32_t	titleFont;									//���������ַ�ɫ
		uint32_t	buttNotPress;								//��ťδ���ɫ
		uint32_t	buttNotPressFont;							//��ťδ����ַ�ɫ
		uint32_t	buttPress;									//��ť���ɫ
		uint32_t	buttPressFont;								//��ť����ַ�ɫ
		uint32_t	buttBoard;									//��ťδ���ʱ�߿�ɫ
		uint32_t	highlightKJNotSelect;						//�����ؼ�δѡ��ɫ
		uint32_t	highlightKHNotSelectFont;					//�����ؼ�δѡ���ַ�ɫ
		uint32_t	highlightKJSelect;							//�����ؼ�ѡ��ɫ
		uint32_t	highlightKJSelectFont;						//�����ؼ�ѡ���ַ�ɫ
	}skin;

	__packed struct{
		uint64_t	hasOtherServer:						1;		//Ϊ1��ʾ���ڵ����������
		uint64_t	diffPowerOnOffCnEnPic:				1;		//Ϊ1��ʾ������Ӣ��ģʽ�µ�¼�͹ػ�����
		uint64_t	downLoadPatientInfo:				1;		//Ϊ1��ʾ��������������Ϣ����
	}flag;
}CompanyInfo_s;


/***
*CAT�ؼ���ͬ��������¼���־
***/
typedef __packed struct{
	uint32_t	startFlag;
	uint16_t 	size;
	
	__packed struct{
		uint64_t	paraSet:							1;			//���ò��ֲ���
		uint64_t	innerSampleResult:					1;			//�ڲ��˺ŷ������������������
		uint64_t	outtorSampleResult:					1;			//�ⲿ�˺ŷ������������������
		uint64_t	otherInnerSampleResult:				1;			//�ڲ��˺ŷ��͵�������������������������
		uint64_t	otherOuttorSampleResult:			1;			//�ⲿ�˺ŷ��͵�������������������������
		uint64_t	innerChangeSampleResult:			1;			//�޸��ڲ��˺ŷ������������������
		uint64_t	outtorChangeSampleResult:			1;			//�޸��ⲿ�˺ŷ������������������
		uint64_t	otherInnerChangeSampleResult:		1;			//�޸��ڲ��˺ŷ��͸���������������������������
		uint64_t	otherOuttorChangeSampleResult:		1;			//�޸��ⲿ�˺ŷ��͸���������������������������
		uint64_t	sendRunLog:							1;			//����������־��ÿ�ο���ʱ�ż��һ���Ƿ���������־��Ҫ���͸�����ˣ�����һ���Է���������д����͵�������־
		uint64_t	delTestData:						1;			//����˲�������
		uint64_t	delUserData:						1;			//������û�����
		uint64_t	resetUserOpenTimestamp:				1;			//�����û�����ʱ��
	}bit;
}CatMastSynServEvent_s;


/***
*�����������ýṹ��
***/
typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	uint8_t 	ucWBC_Gain;			        //WBC����
	uint8_t		ucHGB_Gain;				    //HGB����
	
	uint16_t 	current535;		    		//535��������������ֵ
	uint16_t 	current415;		    		//415��������������ֵ
	uint16_t   	KQADC415;            		//415����HGB����У׼ADCֵ
	uint16_t   	KQADC535;            		//535����HGB����У׼ADCֵ
	uint16_t   	KBADC415;            		//415����HGB�հ�У׼ADCֵ
	uint16_t   	KBADC535;            		//535����HGB�հ�У׼ADCֵ
	uint16_t   	point1Curr415;            	//415�������Ե�1����ֵ
	uint16_t   	point1Curr535;            	//535�������Ե�1����ֵ
	uint16_t   	point2Curr415;            	//415�������Ե�2����ֵ
	uint16_t   	point2Curr535;            	//535�������Ե�2����ֵ

	uint8_t		ucPump_Scale;			    //��ռ�ձ� ��0-100��%
	uint16_t   	usNsec_BeepWaring;          //����������, N��������δ���֣������������
	uint32_t 	ulOutIn_In_AddStep;		    //��������������ߣ������
	uint32_t 	ulOutIn_Out_AddStep;	    //��������������ߣ������ 
	uint32_t 	ulClamp_In_AddStep;		    //���ӵ���������ߣ������
	uint32_t 	ulClamp_Out_AddStep;        //���ӵ���������ߣ������
	float		fPress_Adjust;			    //��ѹУ׼ֵ   
	uint8_t 	pressSensor;				//ѹ��������ѡ��Ϊ0��ʾʹ��IT110A��Ϊ1��ʾʹ��HONEYWELL������

	int 		digSensor[5];				//����ѹ������������ֵ��5�㲹����0Kpa��-10Kpa��-20Kpa��-30Kpa��-40Kpa
}ParamSet_s;


/***
*�˹�У׼�ṹ��
***/
typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	float 	fWBC;										//WBCУ׼ϵ��
	uint8_t 	ucaWBC_CalibrationDate[DATE_TIME_LEN];  //У׼����
	
	float 	fHGB;										//HGBУ׼ϵ��
	uint8_t 	ucaHGB_CalibrationDate[DATE_TIME_LEN];  //У׼����
}Calibration_s;


/***
*ϵͳ���� �ṹ��
***/
typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	uint8_t 	ucaDate[DATE_TIME_LEN];					//����
	uint8_t 	ucaTime[DATE_TIME_LEN];					//ʱ��
	Language_e 	eLanguage;								//���ԣ����ģ�Ӣ��
}SystemSet_s;



/***
*�˻���Ϣ����ṹ��
***/
typedef __packed struct _AccountManagement_s{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	AccountInfo_s 	accountInfo[ACCOUNT_MAX_NUM];					//�����˻���Ϣ
	uint8_t 	endIndex;											//β������
	uint8_t 	defaultIndex;										//Ĭ���˻�����
	uint8_t 	curIndex;											//��ǰ��¼���˻�����
}AccountManagement_s;


/***
*��ӡ���� �ṹ��
***/
 #define PRINTSET_TITILE_MAX_LEN		31

typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	char 	ucaPrintTitle[PRINTSET_TITILE_MAX_LEN];	 //��ӡ����
	PrintWay_e		ePrintWay;
}PrintSet_s;


/***
*ʵ������Ϣ �ṹ��
***/
#define  CLINIC_NAME_MAX_LEN					50
#define MACHINE_SN_LEN							12

typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	uint8_t 	ucaMachineName[30];						//��������
	uint8_t 	ucaMachineType[10];						//��������
	uint8_t		ucaMachineSN[MACHINE_SN_LEN+1];			//����ϵ�к�
	uint8_t		ucaClinicName[CLINIC_NAME_MAX_LEN];		//��������	
}LabInfo_s;


/***
*�ο����������ݽṹ��
***/
typedef __packed struct _RefGroups_s{
	uint16_t usWBCH;						//���ޣ������ֵ=ʵ��ֵ*100
	uint16_t usWBCL;						//���ޣ������ֵ=ʵ��ֵ*100
	uint16_t usGranH;
	uint16_t usGranL;
	uint16_t usMidH;
	uint16_t usMidL;
	uint16_t usLymH;
	uint16_t usLymL;
	uint16_t usGranPercentH;
	uint16_t usGranPercentL;
	uint16_t usMidPercentH;
	uint16_t usMidPercentL;
	uint16_t usLymPercentH;
	uint16_t usLymPercentL;
	uint16_t usHGBH;
	uint16_t usHGBL;
}RefGroups_s;


/***
*���败������У׼����
***/
#define NUM_CALIB_POINTS  			5 
typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	uint32_t flag;							//ΪTOUCH_INIT_FLAGʱ��ʾ�Ѿ�У׼������������ΪTOUCH_FOCUS_FLAGʱ��ʾҪǿ�ƽ���У׼ģʽ
	int pixelX[NUM_CALIB_POINTS];			//�������ڱ������У׼�����������
	int pixelY[NUM_CALIB_POINTS];
	int samX[NUM_CALIB_POINTS];				//�������ڱ���ɼ���У׼���ADֵ  
	int samY[NUM_CALIB_POINTS];
}TouchPara_s;






/***
*���������ṹ�壬ע���������Ϊ__packedģʽ
***/
typedef __packed struct _MachInfo_s{	
	uint32_t 	initFlag;									//���˱�־��ΪMACHINFO_INIT_FLAGʱ������Ҫ��ȫ���������г�ʼ������
	
	TouchPara_s 	touch;									//��������
	MachInfoFlag_s 	flag;									//��־��
	CompanyInfo_s 	companyInfo;							//��˾���Բ���
	CatMastSynServEvent_s 	catMastSynServEvent;			//CAT�ؼ���ͬ��������¼���־
	ParamSet_s 	paramSet;									//������Ϣ
	Calibration_s 	calibration;							//�˹�У׼��Ϣ
	SystemSet_s		systemSet;								//ϵͳ������Ϣ
	AccountManagement_s		accountMM;						//�˻�������Ϣ
	PrintSet_s 	printSet;									//��ӡ������Ϣ
	LabInfo_s 	labInfo;									//ʵ������Ϣ
	
	__packed struct{
		uint32_t 	startFlag;
		uint16_t 	size;
		
		QC_SetInfo_s 	set[QC_FILENUM_INDEX_END]; 			//�ʿ��趨����
	}qcInfo;
	
	//===���������===
	__packed struct{
		uint32_t 	startFlag;
		uint16_t 	size;
		
		RefGroups_s 	refGroups[REF_GROUPS_END];   		//�ο����Ӧ��ȡֵ��Χ
		long 	factoryTimestamp;                 			//��������ʱ�������һ��¼���������кŵ�ʱ��
		long 	userTimestamp;								//�û��״ο���ʱ������û���һ�ο���ʹ�õ�ʱ��
		uint32_t 	totalOpenCount;							//��ǰ�����ܴ���		
		uint16_t 	uiVersion;								//UI��Դ�汾�ţ�Ĭ��100��V1.0.0
	}other;
}MachInfo_s;


extern MachInfo_s MachInfo;


/***
*������̵�����ʼ����ö��
***/
typedef enum _KeyBoardInitJM_e{
	KEYBOARD_HZ = 0,				//��ʼ��ʾ���뺺�ֽ���
	KEYBOARD_CHAR,					//��ʼ��ʾ��ĸ����
	KEYBOARD_NUM					//��ʼ��ʾ���ֽ���
}KeyBoardInitJM_e;



/*
*   wbc hgb �˹�У׼��Ϣ
*/
typedef __packed struct {
    Msg_Head_t      tMsgHead;
    Calibration_s   calibration;   
    
} CalibrationMsg_t;


typedef __packed struct _WBCGZData_s{
	float wbc;
	float fl;
}WBCGZData_s;


/*
*	����ʵʱ����״̬��Ϣ�ṹ��
*/
#define LIST_REVIEW_ROW_NUM     			7
#define HARD_VERSION_MAX_LEN				10										//Ӳ���汾���ַ�����󳤶�
	

typedef __packed struct _MachRunPara_s{
    NextSample_t            tNextSample;            					//��һ������Ϣ 
    uint8_t                 ucaLastestDateTime[DATE_TIME_LEN];          //������ʾ��ʵʱʱ��      
    Login_SelfCheck_t       tLogin_SelfCheck;        					//��¼�Լ���Ϣ�������¼�Լ�ȣ����صĴ����룩
    WBCHGB_TestInfo_t       taListView_Data[LIST_REVIEW_ROW_NUM];  		//�б�ع���ʾ���ݣ�Ϊ���������(���������ɾ��������): �������ݡ���ʼ������
    uint32_t                ulMenuID;               					//��ǰ����ID�����˵�����¼�������������б�عˣ����á�����ȣ�������ID�ŶԹ������ִ��루�磺�����������б�ع˹������棬����Ĭ����Ӣ��ʽ�ȣ������컯����
    KeyBoardInitJM_e		keyboardInitJM;								//���̳�ʼ������ʾö��
	//
    WBCHGB_TestInfo_t 		tWBCHGB_TestInfo;		                    //WBC HGB���Խ����Ϣ
	ParaSetDebugInfo_s		psDebugInfo;			                    //����--���Թ�����Ϣ
	
    //�ʿ�����
    QC_FILENUM_INDEX_e      eQC_Analysis_FileNum_Index;
    QC_FILENUM_INDEX_e      eQC_List_FileNum_Index;
    QC_SetInfo_s            taQC_SetInfo[QC_FILENUM_INDEX_END];         //�ʿ��趨��Ϣ
    QC_WBCHGB_TestInfo_t    tQC_WBCHGB_TestInfo;                        //�ʿط���������ʾ����
    QC_WBCHGB_TestInfo_t    tQC_ListView_Data;                          //�ʿ��б�ع���ʾ����   

	//Ӳ���汾��
	__packed struct{
		char main[HARD_VERSION_MAX_LEN];								//���ذ�Ӳ���汾��
		char zjb[HARD_VERSION_MAX_LEN];									//ת�Ӱ�Ӳ���汾��
	}hardVer;
	
	//�������
	__packed struct{
		uint32_t nextOpenToStartUpdateFlag;								//������ʾ�Ƿ���һ�ο���ʱ�����������������̵ı�־
		uint32_t needBootToUpdateMainFlag;								//������ʾ�������Ѿ�������ϣ���Ҫ������BOOT��������д�뵽ָ��λ�ô�
	}update;
   
	//ʵʱ��־���
	__packed struct{
		uint8_t requestingPatient;										//���ڱ�ʾ������������Ϣ�У�Ϊ1��ʾ������������Ϣ��
	}flag;
	
	//ҩʦ�����������б���Ϣ
	__packed struct{
		uint8_t pageNum;												//ҳ�ţ��ӵ�һҳ��ʼ����
		uint8_t num;													//��ǰҳ�а����Ļ�����Ϣ����
		
		__packed struct{
			char 	name[PATIENT_NAME_LEN];								//����
			uint8_t 	age;											//����
			AgeUnit_e 	eAgeUnit;										//���䵥λ
			SexType_e 	eSex;											//�Ա�
			char 	orderNum[ORDER_NUM_MAX_LEN];						//�������
		}info[5];
	}patientList;
	
	//�����ж���ָ�ʱ״̬
	__packed struct{
		uint8_t flag;													//Ϊ1��ʾ�����Ѿ��жϣ��ȴ��ָ�
		uint32_t interruptTimestamp;									//�����ж�ʱ���
		uint8_t interruptSignalStrength;								//�����ж�ʱ�ź�ǿ��
		uint32_t recoveryTimestamp;										//�����ж�ʱ���
		uint8_t recoverySignalStrength;									//�����ж�ʱ�ź�ǿ��
	}netStatus;
	
	//��ǰ�ȴ��������Ӧ�Ľ�����ݻ��߹�����־���������
	__packed struct{
		uint32_t sampleSN;
		uint32_t logSN;
	}currSN;
	
	//�ʼ��������
	__packed struct{
		__packed struct{
			WBCGZData_s kb_0_200[3];									//WBC�հ�
			WBCGZData_s jmd_8_300[10];									//WBC���ܶ�
			WBCGZData_s jz_5_300[5];									//WBCУ׼--5/300
			WBCGZData_s jz_8_300[5];									//WBCУ׼--8/300
			WBCGZData_s jz_10_300[5];									//WBCУ׼--10/300
			WBCGZData_s xxd_0_1_200[3];									//WBC���Զ�--0.1/200
			WBCGZData_s xxd_25_200[3];									//WBC���Զ�--25/200
			WBCGZData_s xxd_50_200[3];									//WBC���Զ�--50/200
			WBCGZData_s xxd_75_200[3];									//WBC���Զ�--75/200
			WBCGZData_s xxd_100_200[3];									//WBC���Զ�--100/200
		}wbcGZ;
		
		__packed struct{
			float kb[3];												//HGB�հ�
			float jmd[10];												//HBG���ܶ�
			
			//HGBУ׼
			__packed struct{
				float dest;												//��ֵ
				float value[5];											//����ֵ
			}jz[3];
			
			float xx[5][3];												//HGB����
		}hgb;
		
		//׼ȷ��
		__packed struct{
			//��ֵ
			WBCGZData_s wbcDest;
			float hgbDest;
			
			//����ֵ
			WBCGZData_s wbcValue;
			float hgbValue;
		}zqd[3];
	}zj;
}MachRunPara_s;


#define OUTSIDE_DATA_NUM_INIT_FLAG						0x5AA50FF0		//�ⲿ�˺�������������������ʼ�����
#define OUTSIDE_DATA_NUM_MODE_NUM						2				//�ⲿ�˺�����������������ģʽ������
#define OUTSIDE_DATA_MAX_NUM							4500			//�ⲿ�˺��������������������

/***
*�ⲿ�˺������������������ṹ��
***/
typedef struct _OutSideDataNum_s{
	uint32_t initFlag;													//��ʼ�����
	
	uint8_t index;
	uint32_t dataNum[OUTSIDE_DATA_NUM_MODE_NUM];
}OutSideDataNum_s;




#define RUN_LOG_MAX_NUM                                     500     //������־�������(��ǰ�洢��spiflash�У���Ҫ����500)
#define QC_DATA_MAX_NUM                                     50      //�ʿ������������
#define INNER_ACCOUNT_COUNT_MAX_NUM                         501      //�ڲ��˺���������������
#define QC_PER_FILE_NUM_MAX_CAPACITY                        100 	//ÿ���ļ��������ԵĴ���
/*
*   WBC HGB���ݹ���,����˫ѭ������
*/
typedef __packed struct{
    uint32_t        ulLastestSN;                            //��һ����ϵ�кţ����ÿһ���������ݣ���0��ʼ���������������=����ϵ�к�+1��
	uint32_t  		ulQueue_HeadIndex;					    //����ͷ����һ����Ч����
    uint32_t  		ulQueue_TailIndex;				        //����β����һ�������洢λ�ã�������һ��Ϊ���µ���Ч���ݣ�
    uint32_t        ulValidNum;                             //���� ����Ч���ݸ���
    uint8_t         ucaLastestDateTime[DATE_TIME_LEN];		//���һ�β���ʱ��
    
	uint32_t        ccSendIndex;                            //ָ��ǰ��δ�������������������������˵������������ֵ
	uint32_t        otherSendIndex;                         //ָ��ǰ��δ���������������������������˵������������ֵ
} DataManage_WBCHGB_t;


/*
*    QC���ݹ���,����˫ѭ������
*/
typedef __packed struct {
    uint32_t        ulQC_SN;                                //�ʿ����ݵ�ϵ�к�,���㿪ʼ���������ÿ����־
    uint32_t        ulQC_Queue_HeadIndex;                   //�ʿ����ݵĶ��еģ���һ����־��index
    uint32_t        ulQC_Queue_TailIndex;                   //�ʿ����ݵĶ��еģ���һ������д��index
    uint32_t        ulValidNum;                             //�ʿ����ݵĶ��еģ���Ч����

} DataManage_QC_t;



/*
*  ���ݹ���ṹ��
*/
typedef __packed struct{
       
    //�ⲿ���ڲ��˺ţ������������ݣ�WBC��HGB������
    DataManage_WBCHGB_t tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_END];   
    //�ʿ����ݹ���
    DataManage_QC_t tDataManage_QC[QC_FILENUM_INDEX_END];
    //������־����
    uint32_t        ulRunLog_SN;                            //������־��ϵ�к�,���㿪ʼ���������ÿ����־
    uint32_t        ulRunLog_Queue_HeadIndex;               //������־���еģ���һ����־��index
    uint32_t        ulRunLog_Queue_TailIndex;               //������־���еģ���һ������д��index
    uint32_t        ulRunLog_SendIndex;                     //������־���У���һ����Ҫ������־�������ţ�index��
    //
    uint16_t        usCrc;                                  //crc16����У��ֵ
} DataManage_t;








#endif



