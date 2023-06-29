#ifndef _COMMON_H
#define _COMMON_H



#include "stm32f4xx_hal.h"
#include "GUI.h"
#include "main.h"
#include "msg_def.h"





/***
*����Ҫ���Ƶ����������ö��
***/
typedef enum _SanJiaoType_e{
	UP_SAN_JIAO = 0,
	UUP_SAN_JIAO,
	DOWN_SAN_JIAO,
	DDOWN_SAN_JIAO,
	
	ZUO_SAN_JIAO,
	YOU_SAN_JIAO
}SanJiaoType_e;



//===ȫ���¼����־=================================================================================
//��ӡ�����
#define GLOBAL_EVENT_PRINTER_CONN_ERR						1<<0									//����ȫ���¼����д�ӡ��ͨѶ�쳣�¼�
#define GLOBAL_EVENT_PRINTER_NO_PAPER						1<<1									//����ȫ���¼����д�ӡ��ȱֽ�¼�
#define GLOBAL_EVENT_PRINTER_RECOVER						1<<2									//����ȫ���¼����д�ӡ�����»ָ������¼�

//CAT���
#define GLOBAL_EVENT_CAT_START_ERR							1<<0									//����ȫ���¼�����CATģ������ʧ���¼� 
#define GLOBAL_EVENT_CAT_INIT_SUCCESS						1<<1									//����ȫ���¼�����CATģ���ʼ������¼�
#define GLOBAL_EVENT_CAT_SERIAL_ERR							1<<2									//����ȫ���¼����д�����CATģ��ͨѶʧ���¼�
#define GLOBAL_EVENT_CAT_NO_SIM_ERR							1<<3									//����ȫ���¼�����CATģ����SIM���쳣�¼�
#define GLOBAL_EVENT_CAT_SIM_UNACTIVE_ERR					1<<4									//����ȫ���¼�����CATģ��SIM��δ�����쳣�¼�
#define GLOBAL_EVENT_CAT_REGIST_NET_ERR						1<<5									//����ȫ���¼�����CATģ��ע�������쳣�¼�
#define GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS					1<<6									//����ȫ���¼�����CATģ��ע������ɹ��¼�
#define GLOBAL_EVENT_CAT_CONN_NET_ERR						1<<7									//����ȫ���¼�����CATģ�����ӷ�����쳣�¼�
#define GLOBAL_EVENT_CAT_FACTORY_INFO_EXIST					1<<8									//����ȫ���¼�����CATģ�鷢������������Ϣ�������ʱ����ӦΪ��Ϣ�Ѵ����쳣
#define GLOBAL_EVENT_CAT_FACTORY_INFO_OK					1<<9									//����ȫ���¼�����CATģ�鷢������������Ϣ�������ʱ��������Ӧ¼��ɹ��¼�
#define GLOBAL_EVENT_CAT_MACHINE_PARA_ILLEGAL				1<<10									//����ȫ���¼�����CATģ�鷢�͸�����˵Ĳ����Ƿ��쳣
#define GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_OK				1<<11									//����ȫ���¼�����CATģ�鷢�͸�����˵��޸�������Ϣ��ָ��ͳɹ��¼�
#define GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_ERR			1<<12									//����ȫ���¼�����CATģ�鷢�͸�����˵��޸�������Ϣ��ָ���ʧ���¼�
#define GLOBAL_EVENT_CAT_RECOVER_FACTORY_OK					1<<13									//����ȫ���¼�����CATģ�鷢�͸�����˵Ļָ��������õ�ָ��ͳɹ��¼�
#define GLOBAL_EVENT_CAT_RECOVER_FACTORY_ERR				1<<14									//����ȫ���¼�����CATģ�鷢�͸�����˵Ļָ��������õ�ָ���ʧ���¼�
#define GLOBAL_EVENT_CAT_OTHER_SER_START					1<<15									//����ȫ���¼�����CATģ��֪ͨ������������ͨѶ��������������¼�
#define GLOBAL_EVENT_CAT_OTHER_SER_PATIENT_INFO_OK			1<<16									//����ȫ���¼����е������������Ӧָ�����߱�ŵĻ�����Ϣ�·��ɹ��¼�
#define GLOBAL_EVENT_CAT_OTHER_SER_NO_PATIENT_INFO			1<<17									//����ȫ���¼����е������������Ӧָ�����߱�ŵĻ�����Ϣ�������¼�
#define GLOBAL_EVENT_CAT_REFRESH_NET						1<<18									//����ȫ���¼�����CAT�ֶ�ˢ��������¼�
#define GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR					1<<19									//����ȫ���¼�����CATģ�����ӵ�����������쳣�¼�
            
//�������			
#define GLOBAL_EVENT_OTHER_HGB_ADJUST_CLOSE					1<<0									//����ȫ���¼���������HGBУ׼�����йصư����������¼�




//===SDRAM��ַ����=================================================================
//===�ײ�Ӳ�������ڴ���======
//��LTDC������ʹ���˸õ�ַ��һ�������С��800*480*2=768000/1024=750KB���������໺����ƣ�����Ҫ��ȡ��Ӧ���ڴ���������￪��3�����壬������ҪԤ��>=750*3KB
#define SDRAM_LTDC_ADDR		    						LTDC_LAYER_START_ADR  //0xC0000000������ʵ��ֻ������ʾ���ã�ʵ�������õ���LTDC_LAYER_START_ADR
#define SDRAM_LTDC_LEN		    						0x258000	//2400K
		
//��ӡ����ӡλͼʹ��		
#define SDRAM_PRINTER_ADDR	        					(SDRAM_LTDC_ADDR+SDRAM_LTDC_LEN)
#define SDRAM_PRINTER_LEN	        					0xC8000		//800K
		
//GUI�����ַ����С		
#define SDRAM_GUI_ADDR		        					(SDRAM_PRINTER_ADDR+SDRAM_PRINTER_LEN)
#define SDRAM_GUI_LEN		        					0x600000	//6144K
		
		
//===�����ڴ���======		
//MachRunPara����		
#define SDRAM_MACH_RUN_PARA_ADDR	        			(SDRAM_GUI_ADDR+SDRAM_GUI_LEN)
#define SDRAM_MACH_RUN_PARA_LEN		    				0x40000	    //256K
		
//MachInfo����		
#define SDRAM_MACHINE_INFO_ADDR	    					(SDRAM_MACH_RUN_PARA_ADDR+SDRAM_MACH_RUN_PARA_LEN)
#define SDRAM_MACHINE_INFO_LEN							0x40000	    //256K
		
//����������		
#define SDRAM_COMMON_BUF_ADDR	    					(SDRAM_MACHINE_INFO_ADDR+SDRAM_MACHINE_INFO_LEN)
#define SDRAM_COMMON_BUF_LEN							0x700000	//7168K
		
		
//===UI��Դ��======		
//UNI2GBK�洢��ַ		
#define SDRAM_UNI2GBK_ADDR								(SDRAM_COMMON_BUF_ADDR+SDRAM_COMMON_BUF_LEN)
#define SDRAM_UNI2GBK_LEN								0x20000		//128K
		
//����12�Ŵ洢��ַ����С(1009K)		
#define SDRAM_FONT12_ADDR		    					(SDRAM_UNI2GBK_ADDR+SDRAM_UNI2GBK_LEN)
#define SDRAM_FONT12_LEN		    					0x180000	//1536K
		
//����16�Ŵ洢��ַ����С(1233K)		
#define SDRAM_FONT16_ADDR		    					(SDRAM_FONT12_ADDR+SDRAM_FONT12_LEN)
#define SDRAM_FONT16_LEN		    					0x180000	//1536K
		
//���ػ�����		
#define SDRAM_BK_PICTURE_PWOER_ON_OFF_ADDR      		(SDRAM_FONT16_ADDR+SDRAM_FONT16_LEN)
#define SDRAM_BK_PICTURE_PWOER_ON_OFF_LEN       		 0x12C000  	//1200k
		
//��¼����		
#define SDRAM_BK_PICTURE_LOGIN_ADDR             		(SDRAM_BK_PICTURE_PWOER_ON_OFF_ADDR+SDRAM_BK_PICTURE_PWOER_ON_OFF_LEN)  	
#define SDRAM_BK_PICTURE_LOGIN_LEN              		 0x12C000  	//1200k
		
//��¼�Լ����		
#define SDRAM_BK_PICTURE_SELF_CHECK_ADDR        		(SDRAM_BK_PICTURE_LOGIN_ADDR+SDRAM_BK_PICTURE_LOGIN_LEN)  	
#define SDRAM_BK_PICTURE_SELF_CHECK_LEN         		 0x12C000  	//1200k
		
//���ý���ICON������		
#define SDRAM_BK_PICTURE_ICONBKBIG_ADDR	        		(SDRAM_BK_PICTURE_SELF_CHECK_ADDR+SDRAM_BK_PICTURE_SELF_CHECK_LEN)
#define SDRAM_BK_PICTURE_ICONBKBIG_LEN	        		0xA000	    //40K
		
//�������ICON������		
#define SDRAM_BK_PICTURE_ICONBKSMALL_ADDR	    		(SDRAM_BK_PICTURE_ICONBKBIG_ADDR+SDRAM_BK_PICTURE_ICONBKBIG_LEN)
#define SDRAM_BK_PICTURE_ICONBKSAMLL_LEN	    		0xA000	    //40K


//����--����ϻ�ICON
#define SDRAM_BK_PICTURE_AgingTestActive_ADDR        	(SDRAM_BK_PICTURE_ICONBKSMALL_ADDR+SDRAM_BK_PICTURE_ICONBKSAMLL_LEN) 
#define SDRAM_BK_PICTURE_AgingTestActive_LEN         	0x2800  	//10k

//����--����ϻ�ICON��
#define SDRAM_BK_PICTURE_AgingTestUnactive_ADDR        	(SDRAM_BK_PICTURE_AgingTestActive_ADDR+SDRAM_BK_PICTURE_AgingTestActive_LEN)
#define SDRAM_BK_PICTURE_AgingTestUnactive_LEN         	0x2800  	//10k

//����--�������Լ�ICON
#define SDRAM_BK_PICTURE_AirLight_ADDR               	(SDRAM_BK_PICTURE_AgingTestUnactive_ADDR+SDRAM_BK_PICTURE_AgingTestUnactive_LEN)
#define SDRAM_BK_PICTURE_AirLight_LEN                	0x2800  	//10k

//����--������־ICON
#define SDRAM_BK_PICTURE_AllLog_ADDR                 	(SDRAM_BK_PICTURE_AirLight_ADDR+SDRAM_BK_PICTURE_AirLight_LEN)
#define SDRAM_BK_PICTURE_AllLog_LEN                  	0x2800  	//10k

//����--�洢��ϢICON
#define SDRAM_BK_PICTURE_Disk_ADDR                   	(SDRAM_BK_PICTURE_AllLog_ADDR+SDRAM_BK_PICTURE_AllLog_LEN)
#define SDRAM_BK_PICTURE_Disk_LEN                    	0x2800  	//10k

//����--������־ICON
#define SDRAM_BK_PICTURE_ErrLog_ADDR                 	(SDRAM_BK_PICTURE_Disk_ADDR+SDRAM_BK_PICTURE_Disk_LEN)
#define SDRAM_BK_PICTURE_ErrLog_LEN                  	0x2800  	//10k

//����--�����Լ�ICON
#define SDRAM_BK_PICTURE_InterActionActive_ADDR      	(SDRAM_BK_PICTURE_ErrLog_ADDR+SDRAM_BK_PICTURE_ErrLog_LEN)
#define SDRAM_BK_PICTURE_InterActionActive_LEN       	0x2800  	//10k

//����--�����Լ�ICON��
#define SDRAM_BK_PICTURE_InterActionUnactive_ADDR    	(SDRAM_BK_PICTURE_InterActionActive_ADDR+SDRAM_BK_PICTURE_InterActionActive_LEN)
#define SDRAM_BK_PICTURE_InterActionUnactive_LEN     	0x2800  	//10k

//����--�������Լ�ICON
#define SDRAM_BK_PICTURE_Moto_ADDR                   	(SDRAM_BK_PICTURE_InterActionUnactive_ADDR+SDRAM_BK_PICTURE_InterActionUnactive_LEN)
#define SDRAM_BK_PICTURE_Moto_LEN                    	0x2800  	//10k

//����--�����Լ�ICON
#define SDRAM_BK_PICTURE_Others_ADDR                 	(SDRAM_BK_PICTURE_Moto_ADDR+SDRAM_BK_PICTURE_Moto_LEN)
#define SDRAM_BK_PICTURE_Others_LEN                  	0x2800  	//10k

//����--������־ICON
#define SDRAM_BK_PICTURE_ParaLog_ADDR                	(SDRAM_BK_PICTURE_Others_ADDR+SDRAM_BK_PICTURE_Others_LEN)
#define SDRAM_BK_PICTURE_ParaLog_LEN                 	0x2800  	//10k

//����--��ѹ�Լ�ICON
#define SDRAM_BK_PICTURE_Pressure_ADDR               	(SDRAM_BK_PICTURE_ParaLog_ADDR+SDRAM_BK_PICTURE_ParaLog_LEN)
#define SDRAM_BK_PICTURE_Pressure_LEN                	0x2800  	//10k

//����--����״̬ICON
#define SDRAM_BK_PICTURE_StatusActive_ADDR           	(SDRAM_BK_PICTURE_Pressure_ADDR+SDRAM_BK_PICTURE_Pressure_LEN)
#define SDRAM_BK_PICTURE_StatusActive_LEN            	0x2800  	//10k

//����--����״̬ICON��
#define SDRAM_BK_PICTURE_StatusUnactive_ADDR         	(SDRAM_BK_PICTURE_StatusActive_ADDR+SDRAM_BK_PICTURE_StatusActive_LEN)
#define SDRAM_BK_PICTURE_StatusUnactive_LEN           	0x2800  	//10k

//����--�汾��ϢICON
#define SDRAM_BK_PICTURE_Version_ADDR                	(SDRAM_BK_PICTURE_StatusUnactive_ADDR+SDRAM_BK_PICTURE_StatusUnactive_LEN)
#define SDRAM_BK_PICTURE_Version_LEN                 	0x2800  	//10k

//����--���ݹ���ICON
#define SDRAM_BK_PICTURE_DataActive_ADDR             	(SDRAM_BK_PICTURE_Version_ADDR+SDRAM_BK_PICTURE_Version_LEN)
#define SDRAM_BK_PICTURE_DataActive_LEN              	0x2800  	//10k

//����--���ݹ���ICON��
#define SDRAM_BK_PICTURE_DataUnActive_ADDR           	(SDRAM_BK_PICTURE_DataActive_ADDR+SDRAM_BK_PICTURE_DataActive_LEN)
#define SDRAM_BK_PICTURE_DataUnActive_LEN            	0x2800  	//10k

//����--���浯��ICON
#define SDRAM_BK_PICTURE_Err_ADDR                   	(SDRAM_BK_PICTURE_DataUnActive_ADDR+SDRAM_BK_PICTURE_DataUnActive_LEN)
#define SDRAM_BK_PICTURE_Err_LEN                     	0x3C00  	//15k

//����--OK����ICON
#define SDRAM_BK_PICTURE_Ok_ADDR                    	(SDRAM_BK_PICTURE_Err_ADDR+SDRAM_BK_PICTURE_Err_LEN)
#define SDRAM_BK_PICTURE_Ok_LEN                      	0x3C00  	//15k

//����--�Լ�ICON
#define SDRAM_BK_PICTURE_SelfCheck_ADDR             	(SDRAM_BK_PICTURE_Ok_ADDR+SDRAM_BK_PICTURE_Ok_LEN)
#define SDRAM_BK_PICTURE_SelfCheck_LEN              	0x6400  	//25k

//����--������־ICON��
#define SDRAM_BK_PICTURE_AllLogUnactive_ADDR        	(SDRAM_BK_PICTURE_SelfCheck_ADDR+SDRAM_BK_PICTURE_SelfCheck_LEN)
#define SDRAM_BK_PICTURE_AllLogUnactive_LEN          	0x2800  	//10k

//����--������־ICON��
#define SDRAM_BK_PICTURE_ErrLogUnactive_ADDR         	(SDRAM_BK_PICTURE_AllLogUnactive_ADDR+SDRAM_BK_PICTURE_AllLogUnactive_LEN)
#define SDRAM_BK_PICTURE_ErrLogUnactive_LEN          	0x2800  	//10k

//����--������־ICON��
#define SDRAM_BK_PICTURE_ParaLogUnactive_ADDR       	(SDRAM_BK_PICTURE_ErrLogUnactive_ADDR+SDRAM_BK_PICTURE_ErrLogUnactive_LEN)
#define SDRAM_BK_PICTURE_ParaLogUnactive_LEN        	0x2800  	//10k


//����--����ICON
#define SDRAM_BK_PICTURE_FuZhu_ADDR                 	(SDRAM_BK_PICTURE_ParaLogUnactive_ADDR+SDRAM_BK_PICTURE_ParaLogUnactive_LEN)
#define SDRAM_BK_PICTURE_FuZhu_LEN                  	0x2800  	//10k

//����--����ICON��	
#define SDRAM_BK_PICTURE_FuZhuUnactive_ADDR         	(SDRAM_BK_PICTURE_FuZhu_ADDR+SDRAM_BK_PICTURE_FuZhu_LEN)
#define SDRAM_BK_PICTURE_FuZhuUnactive_LEN          	0x2800  	//10k

//����--У׼ICON	
#define SDRAM_BK_PICTURE_JiaoZhun_ADDR              	(SDRAM_BK_PICTURE_FuZhuUnactive_ADDR+SDRAM_BK_PICTURE_FuZhuUnactive_LEN)
#define SDRAM_BK_PICTURE_JiaoZhun_LEN               	0x2800  	//10k

//����--У׼ICON��	
#define SDRAM_BK_PICTURE_JiaoZhunGray_ADDR          	(SDRAM_BK_PICTURE_JiaoZhun_ADDR+SDRAM_BK_PICTURE_JiaoZhun_LEN)
#define SDRAM_BK_PICTURE_JiaoZhunGray_LEN           	0x2800  	//10k

//����--ʵ������ϢICON	
#define SDRAM_BK_PICTURE_LabInfo_ADDR               	(SDRAM_BK_PICTURE_JiaoZhunGray_ADDR+SDRAM_BK_PICTURE_JiaoZhunGray_LEN)
#define SDRAM_BK_PICTURE_LabInfo_LEN                	0x2800  	//10k

//����--��������ICON	
#define SDRAM_BK_PICTURE_Para_ADDR                  	(SDRAM_BK_PICTURE_LabInfo_ADDR+SDRAM_BK_PICTURE_LabInfo_LEN)
#define SDRAM_BK_PICTURE_Para_LEN                   	0x2800  	//10k

//����--��������ICON��
#define SDRAM_BK_PICTURE_ParaGray_ADDR              	(SDRAM_BK_PICTURE_Para_ADDR+SDRAM_BK_PICTURE_Para_LEN)
#define SDRAM_BK_PICTURE_ParaGray_LEN               	0x2800  	//10k

//����--��ӡ����ICON	
#define SDRAM_BK_PICTURE_Printer_ADDR               	(SDRAM_BK_PICTURE_ParaGray_ADDR+SDRAM_BK_PICTURE_ParaGray_LEN)
#define SDRAM_BK_PICTURE_Printer_LEN                	0x2800  	//10k

//����--ϵͳ����ICON	
#define SDRAM_BK_PICTURE_Sys_ADDR                   	(SDRAM_BK_PICTURE_Printer_ADDR+SDRAM_BK_PICTURE_Printer_LEN)
#define SDRAM_BK_PICTURE_Sys_LEN                    	0x2800  	//10k

//����--���Թ���ICON	
#define SDRAM_BK_PICTURE_Test_ADDR                  	(SDRAM_BK_PICTURE_Sys_ADDR+SDRAM_BK_PICTURE_Sys_LEN)
#define SDRAM_BK_PICTURE_Test_LEN                   	0x2800  	//10k

//����--���Թ���ICON��	
#define SDRAM_BK_PICTURE_TestGray_ADDR              	(SDRAM_BK_PICTURE_Test_ADDR+SDRAM_BK_PICTURE_Test_LEN)
#define SDRAM_BK_PICTURE_TestGray_LEN               	0x2800  	//10k

//����--�û�����ICON
#define SDRAM_BK_PICTURE_User_ADDR                  	(SDRAM_BK_PICTURE_TestGray_ADDR+SDRAM_BK_PICTURE_TestGray_LEN)
#define SDRAM_BK_PICTURE_User_LEN                   	0x2800  	//10k

//����--�����л���ICON
#define SDRAM_BK_PICTURE_SwitchClose_ADDR           	(SDRAM_BK_PICTURE_User_ADDR+SDRAM_BK_PICTURE_User_LEN)
#define SDRAM_BK_PICTURE_SwitchClose_LEN            	0x2800  	//10k

//����--�����л���ICON	
#define SDRAM_BK_PICTURE_SwitchOpen_ADDR            	(SDRAM_BK_PICTURE_SwitchClose_ADDR+SDRAM_BK_PICTURE_SwitchClose_LEN)
#define SDRAM_BK_PICTURE_SwitchOpen_LEN             	0x2800  	//10k


//�ʿ�--����
#define SDRAM_BK_PICTURE_QC_SET_ADDR                	(SDRAM_BK_PICTURE_SwitchOpen_ADDR+SDRAM_BK_PICTURE_SwitchOpen_LEN)
#define SDRAM_BK_PICTURE_QC_SET_LEN                 	0x2800  	//10k

//�ʿ�--�б�	
#define SDRAM_BK_PICTURE_QC_LIST_ADDR               	(SDRAM_BK_PICTURE_QC_SET_ADDR+SDRAM_BK_PICTURE_QC_SET_LEN)
#define SDRAM_BK_PICTURE_QC_LIST_LEN                	0x2800  	//10k

//�ʿ�--����	
#define SDRAM_BK_PICTURE_QC_ANALYSIS_ADDR           	(SDRAM_BK_PICTURE_QC_LIST_ADDR+SDRAM_BK_PICTURE_QC_LIST_LEN)
#define SDRAM_BK_PICTURE_QC_ANALYSIS_LEN            	0x2800  	//10k


//��������--��������
#define SDRAM_BK_PICTURE_WARN_BLUE_ADDR              	(SDRAM_BK_PICTURE_QC_ANALYSIS_ADDR+SDRAM_BK_PICTURE_QC_ANALYSIS_LEN)
#define SDRAM_BK_PICTURE_WARN_BLUE_LEN               	0x5000  	//20k

//��������--����׵�	
#define SDRAM_BK_PICTURE_WARN_WHITE_ADDR             	(SDRAM_BK_PICTURE_WARN_BLUE_ADDR+SDRAM_BK_PICTURE_WARN_BLUE_LEN)
#define SDRAM_BK_PICTURE_WARN_WHITE_LEN              	0x5000  	//20k

//��������--����ʧ��	
#define SDRAM_BK_PICTURE_COUNT_FAIL_ADDR             	(SDRAM_BK_PICTURE_WARN_WHITE_ADDR+SDRAM_BK_PICTURE_WARN_WHITE_LEN)
#define SDRAM_BK_PICTURE_COUNT_FAIL_LEN              	0x64000  	//400k

//===ICON===
//�����������ͼ��
#define SDRAM_BK_ICON_ANALYSIS_BTN_ADDR             	(SDRAM_BK_PICTURE_COUNT_FAIL_ADDR+SDRAM_BK_PICTURE_COUNT_FAIL_LEN)
#define SDRAM_BK_ICON_ANALYSIS_BTN_LEN              	0x1000  //4k

//��������δ���ͼ��	
#define SDRAM_BK_ICON_ANALYSIS_BTN_UN_ADDR          	(SDRAM_BK_ICON_ANALYSIS_BTN_ADDR+SDRAM_BK_ICON_ANALYSIS_BTN_LEN)
#define SDRAM_BK_ICON_ANALYSIS_BTN_UN_LEN           	0x1000  //4k

//�б�ع˵��ͼ��	
#define SDRAM_BK_ICON_LIST_BTN_ADDR           			(SDRAM_BK_ICON_ANALYSIS_BTN_UN_ADDR+SDRAM_BK_ICON_ANALYSIS_BTN_UN_LEN)
#define SDRAM_BK_ICON_LIST_BTN_LEN            			0x1000  //4k

//�б�ع�δ���ͼ��	
#define SDRAM_BK_ICON_LIST_BTN_UN_ADDR           		(SDRAM_BK_ICON_LIST_BTN_ADDR+SDRAM_BK_ICON_LIST_BTN_LEN)
#define SDRAM_BK_ICON_LIST_BTN_UN_LEN            		0x1000  //4k

//������ʾԲȦͼ��	
#define SDRAM_BK_ICON_ERR_YUAN_BTN_ADDR           		(SDRAM_BK_ICON_LIST_BTN_UN_ADDR+SDRAM_BK_ICON_LIST_BTN_UN_LEN)
#define SDRAM_BK_ICON_ERR_YUAN_BTN_LEN            		0x800  //2k

//������ʾ����ͼ��	
#define SDRAM_BK_ICON_ERR_SJ_BTN_ADDR           		(SDRAM_BK_ICON_ERR_YUAN_BTN_ADDR+SDRAM_BK_ICON_ERR_YUAN_BTN_LEN)
#define SDRAM_BK_ICON_ERR_SJ_BTN_LEN            		0x800  //2k

//�˵����ͼ��	
#define SDRAM_BK_ICON_MENU_BTN_ADDR           			(SDRAM_BK_ICON_ERR_SJ_BTN_ADDR+SDRAM_BK_ICON_ERR_SJ_BTN_LEN)
#define SDRAM_BK_ICON_MENU_BTN_LEN            			0x800  //2k

//�˵�δ���ͼ��	
#define SDRAM_BK_ICON_MENU_BTN_UN_ADDR           		(SDRAM_BK_ICON_MENU_BTN_ADDR+SDRAM_BK_ICON_MENU_BTN_LEN)
#define SDRAM_BK_ICON_MENU_BTN_UN_LEN            		0x800  //2k

//�˵���������������ͼ��	
#define SDRAM_BK_ICON_MENU_ANALYSIS_BTN_ADDR        	(SDRAM_BK_ICON_MENU_BTN_UN_ADDR+SDRAM_BK_ICON_MENU_BTN_UN_LEN)
#define SDRAM_BK_ICON_MENU_ANALYSIS_BTN_LEN         	0x7800  //30k

//�˵��������б�ع�ͼ��	
#define SDRAM_BK_ICON_MENU_LIST_BTN_ADDR          		(SDRAM_BK_ICON_MENU_ANALYSIS_BTN_ADDR+SDRAM_BK_ICON_MENU_ANALYSIS_BTN_LEN)
#define SDRAM_BK_ICON_MENU_LIST_BTN_LEN           		0x7800  //30k

//�˵�������ע��ͼ��	
#define SDRAM_BK_ICON_MENU_LOGOUT_BTN_ADDR          	(SDRAM_BK_ICON_MENU_LIST_BTN_ADDR+SDRAM_BK_ICON_MENU_LIST_BTN_LEN)
#define SDRAM_BK_ICON_MENU_LOGOUT_BTN_LEN           	0x7800  //30k

//�˵������ڹػ�ͼ��	
#define SDRAM_BK_ICON_MENU_OFF_BTN_ADDR          		(SDRAM_BK_ICON_MENU_LOGOUT_BTN_ADDR+SDRAM_BK_ICON_MENU_LOGOUT_BTN_LEN)
#define SDRAM_BK_ICON_MENU_OFF_BTN_LEN           		0x7800  //30k

//�˵��������ʿ�ͼ��	
#define SDRAM_BK_ICON_MENU_QC_BTN_ADDR          		(SDRAM_BK_ICON_MENU_OFF_BTN_ADDR+SDRAM_BK_ICON_MENU_OFF_BTN_LEN)
#define SDRAM_BK_ICON_MENU_QC_BTN_LEN           		0x7800  //30k

//�˵������ڷ���ͼ��	
#define SDRAM_BK_ICON_MENU_SERVICE_BTN_ADDR         	(SDRAM_BK_ICON_MENU_QC_BTN_ADDR+SDRAM_BK_ICON_MENU_QC_BTN_LEN)
#define SDRAM_BK_ICON_MENU_SERVICE_BTN_LEN          	0x7800  //30k

//�˵�����������ͼ��	
#define SDRAM_BK_ICON_MENU_SET_BTN_ADDR          		(SDRAM_BK_ICON_MENU_SERVICE_BTN_ADDR+SDRAM_BK_ICON_MENU_SERVICE_BTN_LEN)
#define SDRAM_BK_ICON_MENU_SET_BTN_LEN           		0x7800  //30k

//�˵��������û�ͷ��ͼ��	
#define SDRAM_BK_ICON_MENU_USER_BTN_ADDR          		(SDRAM_BK_ICON_MENU_SET_BTN_ADDR+SDRAM_BK_ICON_MENU_SET_BTN_LEN)
#define SDRAM_BK_ICON_MENU_USER_BTN_LEN           		0x1800  //6k

//������ͷͼ��	
#define SDRAM_BK_ICON_XJT_BTN_ADDR          			(SDRAM_BK_ICON_MENU_USER_BTN_ADDR+SDRAM_BK_ICON_MENU_USER_BTN_LEN)
#define SDRAM_BK_ICON_XJT_BTN_LEN           			0x400  //1k



//===�㷨===
#define SDRAM_ALGO_ADDR		        					(SDRAM_BK_ICON_XJT_BTN_ADDR+SDRAM_BK_ICON_XJT_BTN_LEN)
#define SDRAM_ALGO_LEN		        					0x800000	//8M



//===��β��ַ===
#define SDRAM_END_ADDR									SDRAM_ALGO_ADDR+SDRAM_ALGO_LEN




















uint32_t SDRAM_ApplyCommonBuf(uint32_t Size);
void UI_DrawSanJiao(SanJiaoType_e SJType,uint16_t CenterX,uint16_t CenterY,GUI_COLOR Color);
void Company_ConfigInfo(Company_e Company);
ReferGroup_e AgeToRefGroup(uint8_t Age,AgeUnit_e AgeUnit,SexType_e Sex);




#endif
