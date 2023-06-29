#ifndef _COMMON_H
#define _COMMON_H



#include "stm32f4xx_hal.h"
#include "GUI.h"
#include "main.h"
#include "msg_def.h"





/***
*定义要绘制的三角形类别枚举
***/
typedef enum _SanJiaoType_e{
	UP_SAN_JIAO = 0,
	UUP_SAN_JIAO,
	DOWN_SAN_JIAO,
	DDOWN_SAN_JIAO,
	
	ZUO_SAN_JIAO,
	YOU_SAN_JIAO
}SanJiaoType_e;



//===全局事件组标志=================================================================================
//打印机相关
#define GLOBAL_EVENT_PRINTER_CONN_ERR						1<<0									//定义全局事件组中打印机通讯异常事件
#define GLOBAL_EVENT_PRINTER_NO_PAPER						1<<1									//定义全局事件组中打印机缺纸事件
#define GLOBAL_EVENT_PRINTER_RECOVER						1<<2									//定义全局事件组中打印机重新恢复正常事件

//CAT相关
#define GLOBAL_EVENT_CAT_START_ERR							1<<0									//定义全局事件组中CAT模块启动失败事件 
#define GLOBAL_EVENT_CAT_INIT_SUCCESS						1<<1									//定义全局事件组中CAT模块初始化完成事件
#define GLOBAL_EVENT_CAT_SERIAL_ERR							1<<2									//定义全局事件组中串口与CAT模块通讯失败事件
#define GLOBAL_EVENT_CAT_NO_SIM_ERR							1<<3									//定义全局事件组中CAT模块无SIM卡异常事件
#define GLOBAL_EVENT_CAT_SIM_UNACTIVE_ERR					1<<4									//定义全局事件组中CAT模块SIM卡未激活异常事件
#define GLOBAL_EVENT_CAT_REGIST_NET_ERR						1<<5									//定义全局事件组中CAT模块注册网络异常事件
#define GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS					1<<6									//定义全局事件组中CAT模块注册网络成功事件
#define GLOBAL_EVENT_CAT_CONN_NET_ERR						1<<7									//定义全局事件组中CAT模块连接服务端异常事件
#define GLOBAL_EVENT_CAT_FACTORY_INFO_EXIST					1<<8									//定义全局事件组中CAT模块发送仪器出厂信息到服务端时，响应为信息已存在异常
#define GLOBAL_EVENT_CAT_FACTORY_INFO_OK					1<<9									//定义全局事件组中CAT模块发送仪器出厂信息到服务端时，正常响应录入成功事件
#define GLOBAL_EVENT_CAT_MACHINE_PARA_ILLEGAL				1<<10									//定义全局事件组中CAT模块发送给服务端的参数非法异常
#define GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_OK				1<<11									//定义全局事件组中CAT模块发送给服务端的修改仪器信息的指令发送成功事件
#define GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_ERR			1<<12									//定义全局事件组中CAT模块发送给服务端的修改仪器信息的指令发送失败事件
#define GLOBAL_EVENT_CAT_RECOVER_FACTORY_OK					1<<13									//定义全局事件组中CAT模块发送给服务端的恢复出厂设置的指令发送成功事件
#define GLOBAL_EVENT_CAT_RECOVER_FACTORY_ERR				1<<14									//定义全局事件组中CAT模块发送给服务端的恢复出厂设置的指令发送失败事件
#define GLOBAL_EVENT_CAT_OTHER_SER_START					1<<15									//定义全局事件组中CAT模块通知与第三方服务端通讯的任务可以启动事件
#define GLOBAL_EVENT_CAT_OTHER_SER_PATIENT_INFO_OK			1<<16									//定义全局事件组中第三方服务端响应指定患者编号的患者信息下发成功事件
#define GLOBAL_EVENT_CAT_OTHER_SER_NO_PATIENT_INFO			1<<17									//定义全局事件组中第三方服务端响应指定患者编号的患者信息不存在事件
#define GLOBAL_EVENT_CAT_REFRESH_NET						1<<18									//定义全局事件组中CAT手动刷新网络的事件
#define GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR					1<<19									//定义全局事件组中CAT模块连接第三方服务端异常事件
            
//其他相关			
#define GLOBAL_EVENT_OTHER_HGB_ADJUST_CLOSE					1<<0									//定义全局事件组其他中HGB校准界面中关灯按键触发的事件




//===SDRAM地址分配=================================================================
//===底层硬件需求内存区======
//在LTDC配置中使用了该地址，一屏缓存大小：800*480*2=768000/1024=750KB，若开启多缓冲机制，则需要留取对应的内存出来，这里开启3级缓冲，所以需要预留>=750*3KB
#define SDRAM_LTDC_ADDR		    						LTDC_LAYER_START_ADR  //0xC0000000，这里实际只是起到提示作用，实际起作用的是LTDC_LAYER_START_ADR
#define SDRAM_LTDC_LEN		    						0x258000	//2400K
		
//打印机打印位图使用		
#define SDRAM_PRINTER_ADDR	        					(SDRAM_LTDC_ADDR+SDRAM_LTDC_LEN)
#define SDRAM_PRINTER_LEN	        					0xC8000		//800K
		
//GUI缓存地址，大小		
#define SDRAM_GUI_ADDR		        					(SDRAM_PRINTER_ADDR+SDRAM_PRINTER_LEN)
#define SDRAM_GUI_LEN		        					0x600000	//6144K
		
		
//===变量内存区======		
//MachRunPara变量		
#define SDRAM_MACH_RUN_PARA_ADDR	        			(SDRAM_GUI_ADDR+SDRAM_GUI_LEN)
#define SDRAM_MACH_RUN_PARA_LEN		    				0x40000	    //256K
		
//MachInfo变量		
#define SDRAM_MACHINE_INFO_ADDR	    					(SDRAM_MACH_RUN_PARA_ADDR+SDRAM_MACH_RUN_PARA_LEN)
#define SDRAM_MACHINE_INFO_LEN							0x40000	    //256K
		
//公共缓存区		
#define SDRAM_COMMON_BUF_ADDR	    					(SDRAM_MACHINE_INFO_ADDR+SDRAM_MACHINE_INFO_LEN)
#define SDRAM_COMMON_BUF_LEN							0x700000	//7168K
		
		
//===UI资源区======		
//UNI2GBK存储地址		
#define SDRAM_UNI2GBK_ADDR								(SDRAM_COMMON_BUF_ADDR+SDRAM_COMMON_BUF_LEN)
#define SDRAM_UNI2GBK_LEN								0x20000		//128K
		
//字体12号存储地址，大小(1009K)		
#define SDRAM_FONT12_ADDR		    					(SDRAM_UNI2GBK_ADDR+SDRAM_UNI2GBK_LEN)
#define SDRAM_FONT12_LEN		    					0x180000	//1536K
		
//字体16号存储地址，大小(1233K)		
#define SDRAM_FONT16_ADDR		    					(SDRAM_FONT12_ADDR+SDRAM_FONT12_LEN)
#define SDRAM_FONT16_LEN		    					0x180000	//1536K
		
//开关机画面		
#define SDRAM_BK_PICTURE_PWOER_ON_OFF_ADDR      		(SDRAM_FONT16_ADDR+SDRAM_FONT16_LEN)
#define SDRAM_BK_PICTURE_PWOER_ON_OFF_LEN       		 0x12C000  	//1200k
		
//登录界面		
#define SDRAM_BK_PICTURE_LOGIN_ADDR             		(SDRAM_BK_PICTURE_PWOER_ON_OFF_ADDR+SDRAM_BK_PICTURE_PWOER_ON_OFF_LEN)  	
#define SDRAM_BK_PICTURE_LOGIN_LEN              		 0x12C000  	//1200k
		
//登录自检界面		
#define SDRAM_BK_PICTURE_SELF_CHECK_ADDR        		(SDRAM_BK_PICTURE_LOGIN_ADDR+SDRAM_BK_PICTURE_LOGIN_LEN)  	
#define SDRAM_BK_PICTURE_SELF_CHECK_LEN         		 0x12C000  	//1200k
		
//设置界面ICON背景框		
#define SDRAM_BK_PICTURE_ICONBKBIG_ADDR	        		(SDRAM_BK_PICTURE_SELF_CHECK_ADDR+SDRAM_BK_PICTURE_SELF_CHECK_LEN)
#define SDRAM_BK_PICTURE_ICONBKBIG_LEN	        		0xA000	    //40K
		
//服务界面ICON背景框		
#define SDRAM_BK_PICTURE_ICONBKSMALL_ADDR	    		(SDRAM_BK_PICTURE_ICONBKBIG_ADDR+SDRAM_BK_PICTURE_ICONBKBIG_LEN)
#define SDRAM_BK_PICTURE_ICONBKSAMLL_LEN	    		0xA000	    //40K


//服务--组件老化ICON
#define SDRAM_BK_PICTURE_AgingTestActive_ADDR        	(SDRAM_BK_PICTURE_ICONBKSMALL_ADDR+SDRAM_BK_PICTURE_ICONBKSAMLL_LEN) 
#define SDRAM_BK_PICTURE_AgingTestActive_LEN         	0x2800  	//10k

//服务--组件老化ICON灰
#define SDRAM_BK_PICTURE_AgingTestUnactive_ADDR        	(SDRAM_BK_PICTURE_AgingTestActive_ADDR+SDRAM_BK_PICTURE_AgingTestActive_LEN)
#define SDRAM_BK_PICTURE_AgingTestUnactive_LEN         	0x2800  	//10k

//服务--气密性自检ICON
#define SDRAM_BK_PICTURE_AirLight_ADDR               	(SDRAM_BK_PICTURE_AgingTestUnactive_ADDR+SDRAM_BK_PICTURE_AgingTestUnactive_LEN)
#define SDRAM_BK_PICTURE_AirLight_LEN                	0x2800  	//10k

//服务--测试日志ICON
#define SDRAM_BK_PICTURE_AllLog_ADDR                 	(SDRAM_BK_PICTURE_AirLight_ADDR+SDRAM_BK_PICTURE_AirLight_LEN)
#define SDRAM_BK_PICTURE_AllLog_LEN                  	0x2800  	//10k

//服务--存储信息ICON
#define SDRAM_BK_PICTURE_Disk_ADDR                   	(SDRAM_BK_PICTURE_AllLog_ADDR+SDRAM_BK_PICTURE_AllLog_LEN)
#define SDRAM_BK_PICTURE_Disk_LEN                    	0x2800  	//10k

//服务--故障日志ICON
#define SDRAM_BK_PICTURE_ErrLog_ADDR                 	(SDRAM_BK_PICTURE_Disk_ADDR+SDRAM_BK_PICTURE_Disk_LEN)
#define SDRAM_BK_PICTURE_ErrLog_LEN                  	0x2800  	//10k

//服务--交互自检ICON
#define SDRAM_BK_PICTURE_InterActionActive_ADDR      	(SDRAM_BK_PICTURE_ErrLog_ADDR+SDRAM_BK_PICTURE_ErrLog_LEN)
#define SDRAM_BK_PICTURE_InterActionActive_LEN       	0x2800  	//10k

//服务--交互自检ICON灰
#define SDRAM_BK_PICTURE_InterActionUnactive_ADDR    	(SDRAM_BK_PICTURE_InterActionActive_ADDR+SDRAM_BK_PICTURE_InterActionActive_LEN)
#define SDRAM_BK_PICTURE_InterActionUnactive_LEN     	0x2800  	//10k

//服务--进出仓自检ICON
#define SDRAM_BK_PICTURE_Moto_ADDR                   	(SDRAM_BK_PICTURE_InterActionUnactive_ADDR+SDRAM_BK_PICTURE_InterActionUnactive_LEN)
#define SDRAM_BK_PICTURE_Moto_LEN                    	0x2800  	//10k

//服务--其他自检ICON
#define SDRAM_BK_PICTURE_Others_ADDR                 	(SDRAM_BK_PICTURE_Moto_ADDR+SDRAM_BK_PICTURE_Moto_LEN)
#define SDRAM_BK_PICTURE_Others_LEN                  	0x2800  	//10k

//服务--运行日志ICON
#define SDRAM_BK_PICTURE_ParaLog_ADDR                	(SDRAM_BK_PICTURE_Others_ADDR+SDRAM_BK_PICTURE_Others_LEN)
#define SDRAM_BK_PICTURE_ParaLog_LEN                 	0x2800  	//10k

//服务--负压自检ICON
#define SDRAM_BK_PICTURE_Pressure_ADDR               	(SDRAM_BK_PICTURE_ParaLog_ADDR+SDRAM_BK_PICTURE_ParaLog_LEN)
#define SDRAM_BK_PICTURE_Pressure_LEN                	0x2800  	//10k

//服务--运行状态ICON
#define SDRAM_BK_PICTURE_StatusActive_ADDR           	(SDRAM_BK_PICTURE_Pressure_ADDR+SDRAM_BK_PICTURE_Pressure_LEN)
#define SDRAM_BK_PICTURE_StatusActive_LEN            	0x2800  	//10k

//服务--运行状态ICON灰
#define SDRAM_BK_PICTURE_StatusUnactive_ADDR         	(SDRAM_BK_PICTURE_StatusActive_ADDR+SDRAM_BK_PICTURE_StatusActive_LEN)
#define SDRAM_BK_PICTURE_StatusUnactive_LEN           	0x2800  	//10k

//服务--版本信息ICON
#define SDRAM_BK_PICTURE_Version_ADDR                	(SDRAM_BK_PICTURE_StatusUnactive_ADDR+SDRAM_BK_PICTURE_StatusUnactive_LEN)
#define SDRAM_BK_PICTURE_Version_LEN                 	0x2800  	//10k

//服务--数据管理ICON
#define SDRAM_BK_PICTURE_DataActive_ADDR             	(SDRAM_BK_PICTURE_Version_ADDR+SDRAM_BK_PICTURE_Version_LEN)
#define SDRAM_BK_PICTURE_DataActive_LEN              	0x2800  	//10k

//服务--数据管理ICON灰
#define SDRAM_BK_PICTURE_DataUnActive_ADDR           	(SDRAM_BK_PICTURE_DataActive_ADDR+SDRAM_BK_PICTURE_DataActive_LEN)
#define SDRAM_BK_PICTURE_DataUnActive_LEN            	0x2800  	//10k

//服务--警告弹窗ICON
#define SDRAM_BK_PICTURE_Err_ADDR                   	(SDRAM_BK_PICTURE_DataUnActive_ADDR+SDRAM_BK_PICTURE_DataUnActive_LEN)
#define SDRAM_BK_PICTURE_Err_LEN                     	0x3C00  	//15k

//服务--OK弹窗ICON
#define SDRAM_BK_PICTURE_Ok_ADDR                    	(SDRAM_BK_PICTURE_Err_ADDR+SDRAM_BK_PICTURE_Err_LEN)
#define SDRAM_BK_PICTURE_Ok_LEN                      	0x3C00  	//15k

//服务--自检ICON
#define SDRAM_BK_PICTURE_SelfCheck_ADDR             	(SDRAM_BK_PICTURE_Ok_ADDR+SDRAM_BK_PICTURE_Ok_LEN)
#define SDRAM_BK_PICTURE_SelfCheck_LEN              	0x6400  	//25k

//服务--测试日志ICON灰
#define SDRAM_BK_PICTURE_AllLogUnactive_ADDR        	(SDRAM_BK_PICTURE_SelfCheck_ADDR+SDRAM_BK_PICTURE_SelfCheck_LEN)
#define SDRAM_BK_PICTURE_AllLogUnactive_LEN          	0x2800  	//10k

//服务--故障日志ICON灰
#define SDRAM_BK_PICTURE_ErrLogUnactive_ADDR         	(SDRAM_BK_PICTURE_AllLogUnactive_ADDR+SDRAM_BK_PICTURE_AllLogUnactive_LEN)
#define SDRAM_BK_PICTURE_ErrLogUnactive_LEN          	0x2800  	//10k

//服务--运行日志ICON灰
#define SDRAM_BK_PICTURE_ParaLogUnactive_ADDR       	(SDRAM_BK_PICTURE_ErrLogUnactive_ADDR+SDRAM_BK_PICTURE_ErrLogUnactive_LEN)
#define SDRAM_BK_PICTURE_ParaLogUnactive_LEN        	0x2800  	//10k


//设置--辅助ICON
#define SDRAM_BK_PICTURE_FuZhu_ADDR                 	(SDRAM_BK_PICTURE_ParaLogUnactive_ADDR+SDRAM_BK_PICTURE_ParaLogUnactive_LEN)
#define SDRAM_BK_PICTURE_FuZhu_LEN                  	0x2800  	//10k

//设置--辅助ICON灰	
#define SDRAM_BK_PICTURE_FuZhuUnactive_ADDR         	(SDRAM_BK_PICTURE_FuZhu_ADDR+SDRAM_BK_PICTURE_FuZhu_LEN)
#define SDRAM_BK_PICTURE_FuZhuUnactive_LEN          	0x2800  	//10k

//设置--校准ICON	
#define SDRAM_BK_PICTURE_JiaoZhun_ADDR              	(SDRAM_BK_PICTURE_FuZhuUnactive_ADDR+SDRAM_BK_PICTURE_FuZhuUnactive_LEN)
#define SDRAM_BK_PICTURE_JiaoZhun_LEN               	0x2800  	//10k

//设置--校准ICON灰	
#define SDRAM_BK_PICTURE_JiaoZhunGray_ADDR          	(SDRAM_BK_PICTURE_JiaoZhun_ADDR+SDRAM_BK_PICTURE_JiaoZhun_LEN)
#define SDRAM_BK_PICTURE_JiaoZhunGray_LEN           	0x2800  	//10k

//设置--实验室信息ICON	
#define SDRAM_BK_PICTURE_LabInfo_ADDR               	(SDRAM_BK_PICTURE_JiaoZhunGray_ADDR+SDRAM_BK_PICTURE_JiaoZhunGray_LEN)
#define SDRAM_BK_PICTURE_LabInfo_LEN                	0x2800  	//10k

//设置--参数设置ICON	
#define SDRAM_BK_PICTURE_Para_ADDR                  	(SDRAM_BK_PICTURE_LabInfo_ADDR+SDRAM_BK_PICTURE_LabInfo_LEN)
#define SDRAM_BK_PICTURE_Para_LEN                   	0x2800  	//10k

//设置--参数设置ICON灰
#define SDRAM_BK_PICTURE_ParaGray_ADDR              	(SDRAM_BK_PICTURE_Para_ADDR+SDRAM_BK_PICTURE_Para_LEN)
#define SDRAM_BK_PICTURE_ParaGray_LEN               	0x2800  	//10k

//设置--打印设置ICON	
#define SDRAM_BK_PICTURE_Printer_ADDR               	(SDRAM_BK_PICTURE_ParaGray_ADDR+SDRAM_BK_PICTURE_ParaGray_LEN)
#define SDRAM_BK_PICTURE_Printer_LEN                	0x2800  	//10k

//设置--系统设置ICON	
#define SDRAM_BK_PICTURE_Sys_ADDR                   	(SDRAM_BK_PICTURE_Printer_ADDR+SDRAM_BK_PICTURE_Printer_LEN)
#define SDRAM_BK_PICTURE_Sys_LEN                    	0x2800  	//10k

//设置--调试功能ICON	
#define SDRAM_BK_PICTURE_Test_ADDR                  	(SDRAM_BK_PICTURE_Sys_ADDR+SDRAM_BK_PICTURE_Sys_LEN)
#define SDRAM_BK_PICTURE_Test_LEN                   	0x2800  	//10k

//设置--调试功能ICON灰	
#define SDRAM_BK_PICTURE_TestGray_ADDR              	(SDRAM_BK_PICTURE_Test_ADDR+SDRAM_BK_PICTURE_Test_LEN)
#define SDRAM_BK_PICTURE_TestGray_LEN               	0x2800  	//10k

//设置--用户管理ICON
#define SDRAM_BK_PICTURE_User_ADDR                  	(SDRAM_BK_PICTURE_TestGray_ADDR+SDRAM_BK_PICTURE_TestGray_LEN)
#define SDRAM_BK_PICTURE_User_LEN                   	0x2800  	//10k

//设置--开关切换关ICON
#define SDRAM_BK_PICTURE_SwitchClose_ADDR           	(SDRAM_BK_PICTURE_User_ADDR+SDRAM_BK_PICTURE_User_LEN)
#define SDRAM_BK_PICTURE_SwitchClose_LEN            	0x2800  	//10k

//设置--开关切换开ICON	
#define SDRAM_BK_PICTURE_SwitchOpen_ADDR            	(SDRAM_BK_PICTURE_SwitchClose_ADDR+SDRAM_BK_PICTURE_SwitchClose_LEN)
#define SDRAM_BK_PICTURE_SwitchOpen_LEN             	0x2800  	//10k


//质控--设置
#define SDRAM_BK_PICTURE_QC_SET_ADDR                	(SDRAM_BK_PICTURE_SwitchOpen_ADDR+SDRAM_BK_PICTURE_SwitchOpen_LEN)
#define SDRAM_BK_PICTURE_QC_SET_LEN                 	0x2800  	//10k

//质控--列表	
#define SDRAM_BK_PICTURE_QC_LIST_ADDR               	(SDRAM_BK_PICTURE_QC_SET_ADDR+SDRAM_BK_PICTURE_QC_SET_LEN)
#define SDRAM_BK_PICTURE_QC_LIST_LEN                	0x2800  	//10k

//质控--分析	
#define SDRAM_BK_PICTURE_QC_ANALYSIS_ADDR           	(SDRAM_BK_PICTURE_QC_LIST_ADDR+SDRAM_BK_PICTURE_QC_LIST_LEN)
#define SDRAM_BK_PICTURE_QC_ANALYSIS_LEN            	0x2800  	//10k


//样本分析--警告蓝底
#define SDRAM_BK_PICTURE_WARN_BLUE_ADDR              	(SDRAM_BK_PICTURE_QC_ANALYSIS_ADDR+SDRAM_BK_PICTURE_QC_ANALYSIS_LEN)
#define SDRAM_BK_PICTURE_WARN_BLUE_LEN               	0x5000  	//20k

//样本分析--警告白底	
#define SDRAM_BK_PICTURE_WARN_WHITE_ADDR             	(SDRAM_BK_PICTURE_WARN_BLUE_ADDR+SDRAM_BK_PICTURE_WARN_BLUE_LEN)
#define SDRAM_BK_PICTURE_WARN_WHITE_LEN              	0x5000  	//20k

//样本分析--计数失败	
#define SDRAM_BK_PICTURE_COUNT_FAIL_ADDR             	(SDRAM_BK_PICTURE_WARN_WHITE_ADDR+SDRAM_BK_PICTURE_WARN_WHITE_LEN)
#define SDRAM_BK_PICTURE_COUNT_FAIL_LEN              	0x64000  	//400k

//===ICON===
//样本分析点击图标
#define SDRAM_BK_ICON_ANALYSIS_BTN_ADDR             	(SDRAM_BK_PICTURE_COUNT_FAIL_ADDR+SDRAM_BK_PICTURE_COUNT_FAIL_LEN)
#define SDRAM_BK_ICON_ANALYSIS_BTN_LEN              	0x1000  //4k

//样本分析未点击图标	
#define SDRAM_BK_ICON_ANALYSIS_BTN_UN_ADDR          	(SDRAM_BK_ICON_ANALYSIS_BTN_ADDR+SDRAM_BK_ICON_ANALYSIS_BTN_LEN)
#define SDRAM_BK_ICON_ANALYSIS_BTN_UN_LEN           	0x1000  //4k

//列表回顾点击图标	
#define SDRAM_BK_ICON_LIST_BTN_ADDR           			(SDRAM_BK_ICON_ANALYSIS_BTN_UN_ADDR+SDRAM_BK_ICON_ANALYSIS_BTN_UN_LEN)
#define SDRAM_BK_ICON_LIST_BTN_LEN            			0x1000  //4k

//列表回顾未点击图标	
#define SDRAM_BK_ICON_LIST_BTN_UN_ADDR           		(SDRAM_BK_ICON_LIST_BTN_ADDR+SDRAM_BK_ICON_LIST_BTN_LEN)
#define SDRAM_BK_ICON_LIST_BTN_UN_LEN            		0x1000  //4k

//错误提示圆圈图标	
#define SDRAM_BK_ICON_ERR_YUAN_BTN_ADDR           		(SDRAM_BK_ICON_LIST_BTN_UN_ADDR+SDRAM_BK_ICON_LIST_BTN_UN_LEN)
#define SDRAM_BK_ICON_ERR_YUAN_BTN_LEN            		0x800  //2k

//错误提示三角图标	
#define SDRAM_BK_ICON_ERR_SJ_BTN_ADDR           		(SDRAM_BK_ICON_ERR_YUAN_BTN_ADDR+SDRAM_BK_ICON_ERR_YUAN_BTN_LEN)
#define SDRAM_BK_ICON_ERR_SJ_BTN_LEN            		0x800  //2k

//菜单点击图标	
#define SDRAM_BK_ICON_MENU_BTN_ADDR           			(SDRAM_BK_ICON_ERR_SJ_BTN_ADDR+SDRAM_BK_ICON_ERR_SJ_BTN_LEN)
#define SDRAM_BK_ICON_MENU_BTN_LEN            			0x800  //2k

//菜单未点击图标	
#define SDRAM_BK_ICON_MENU_BTN_UN_ADDR           		(SDRAM_BK_ICON_MENU_BTN_ADDR+SDRAM_BK_ICON_MENU_BTN_LEN)
#define SDRAM_BK_ICON_MENU_BTN_UN_LEN            		0x800  //2k

//菜单界面内样本分析图标	
#define SDRAM_BK_ICON_MENU_ANALYSIS_BTN_ADDR        	(SDRAM_BK_ICON_MENU_BTN_UN_ADDR+SDRAM_BK_ICON_MENU_BTN_UN_LEN)
#define SDRAM_BK_ICON_MENU_ANALYSIS_BTN_LEN         	0x7800  //30k

//菜单界面内列表回顾图标	
#define SDRAM_BK_ICON_MENU_LIST_BTN_ADDR          		(SDRAM_BK_ICON_MENU_ANALYSIS_BTN_ADDR+SDRAM_BK_ICON_MENU_ANALYSIS_BTN_LEN)
#define SDRAM_BK_ICON_MENU_LIST_BTN_LEN           		0x7800  //30k

//菜单界面内注销图标	
#define SDRAM_BK_ICON_MENU_LOGOUT_BTN_ADDR          	(SDRAM_BK_ICON_MENU_LIST_BTN_ADDR+SDRAM_BK_ICON_MENU_LIST_BTN_LEN)
#define SDRAM_BK_ICON_MENU_LOGOUT_BTN_LEN           	0x7800  //30k

//菜单界面内关机图标	
#define SDRAM_BK_ICON_MENU_OFF_BTN_ADDR          		(SDRAM_BK_ICON_MENU_LOGOUT_BTN_ADDR+SDRAM_BK_ICON_MENU_LOGOUT_BTN_LEN)
#define SDRAM_BK_ICON_MENU_OFF_BTN_LEN           		0x7800  //30k

//菜单界面内质控图标	
#define SDRAM_BK_ICON_MENU_QC_BTN_ADDR          		(SDRAM_BK_ICON_MENU_OFF_BTN_ADDR+SDRAM_BK_ICON_MENU_OFF_BTN_LEN)
#define SDRAM_BK_ICON_MENU_QC_BTN_LEN           		0x7800  //30k

//菜单界面内服务图标	
#define SDRAM_BK_ICON_MENU_SERVICE_BTN_ADDR         	(SDRAM_BK_ICON_MENU_QC_BTN_ADDR+SDRAM_BK_ICON_MENU_QC_BTN_LEN)
#define SDRAM_BK_ICON_MENU_SERVICE_BTN_LEN          	0x7800  //30k

//菜单界面内设置图标	
#define SDRAM_BK_ICON_MENU_SET_BTN_ADDR          		(SDRAM_BK_ICON_MENU_SERVICE_BTN_ADDR+SDRAM_BK_ICON_MENU_SERVICE_BTN_LEN)
#define SDRAM_BK_ICON_MENU_SET_BTN_LEN           		0x7800  //30k

//菜单界面内用户头像图标	
#define SDRAM_BK_ICON_MENU_USER_BTN_ADDR          		(SDRAM_BK_ICON_MENU_SET_BTN_ADDR+SDRAM_BK_ICON_MENU_SET_BTN_LEN)
#define SDRAM_BK_ICON_MENU_USER_BTN_LEN           		0x1800  //6k

//下拉箭头图标	
#define SDRAM_BK_ICON_XJT_BTN_ADDR          			(SDRAM_BK_ICON_MENU_USER_BTN_ADDR+SDRAM_BK_ICON_MENU_USER_BTN_LEN)
#define SDRAM_BK_ICON_XJT_BTN_LEN           			0x400  //1k



//===算法===
#define SDRAM_ALGO_ADDR		        					(SDRAM_BK_ICON_XJT_BTN_ADDR+SDRAM_BK_ICON_XJT_BTN_LEN)
#define SDRAM_ALGO_LEN		        					0x800000	//8M



//===结尾地址===
#define SDRAM_END_ADDR									SDRAM_ALGO_ADDR+SDRAM_ALGO_LEN




















uint32_t SDRAM_ApplyCommonBuf(uint32_t Size);
void UI_DrawSanJiao(SanJiaoType_e SJType,uint16_t CenterX,uint16_t CenterY,GUI_COLOR Color);
void Company_ConfigInfo(Company_e Company);
ReferGroup_e AgeToRefGroup(uint8_t Age,AgeUnit_e AgeUnit,SexType_e Sex);




#endif
