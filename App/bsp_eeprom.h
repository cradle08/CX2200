#ifndef _BSP_EEPROM_H
#define _BSP_EEPROM_H


#include "stm32f4xx_hal.h"
#include "msg_def.h"



#define I2C_SLAVE_ADR_EE24C32					0xA0						//定义24C32的从机物理地址
#define I2C_SLAVE_ADR_EE24C04					0xA8						//定义24C04的从机物理地址

#define MAIN_BORD_EEPROM_24C32					I2C_SLAVE_ADR_EE24C32		//主控板使用的EEPROM型号， 4K
#define CONN_BORD_EEPROM_24C04					I2C_SLAVE_ADR_EE24C04		//显示转接板使用的EEPROM型号, 512Byte

/***
*定义EEPROM类型枚举
***/
typedef enum _EEPROM_Type_e{
	EEPROM_24C02 = 0,
	EEPROM_24C04,
	EEPROM_24C08,
	EEPROM_24C16,
	EEPROM_24C32,   //最大4KB
	EEPROM_24C64
}EEPROM_Type_e;



#define UPDATE_MAIN_SOFTWARE_FLAG					0xA55A0FF0															//定义用于确认需要引导程序升级主程序的标志
#define NEXT_OPEN_TO_START_UPDATE_FLAG				0x5AA50FF0															//定义下一次开机将进入在线升级流程

/*EEPROM地址分配****************************************************************/
//===显示转接板中的EEPROM===
#define EEPROM_ADR_ZJ_BORD_HW_VERSION				0																	//定义转接板上的EEPROM中保存对应的硬件版本号的起始地址

//===主控板中的EEPROM===
#define EEPROM_ADR_MAIN_BORD_HW_VERSION				0																	//定义主控板上的EEPROM中保存对应的硬件版本号的起始地址
#define MAIN_BORD_HW_VERSION_LEN					16																	//16字节

#define EEPROM_ADR_NEXT_UPDATE_FLAG					(EEPROM_ADR_MAIN_BORD_HW_VERSION+MAIN_BORD_HW_VERSION_LEN)			//定义是否需要启动升级更新流程的标志，为0xA55A0FF0时表示开机启动升级流程
#define NEXT_UPDATE_FLAG_LEN						4

#define EEPROM_ADR_UPDATE_MAIN_SOFTWARE_FLAG		(EEPROM_ADR_NEXT_UPDATE_FLAG+NEXT_UPDATE_FLAG_LEN)					//定义EEPROM中用来保存是否升级主程序的标志地址，4字节，为0xA55A0FF0时，表示需要引导程序升级主程序
#define UPDATE_MAIN_SOFTWARE_LEN					4

#define EEPROM_ADDR_AGINGTEST	            		(EEPROM_ADR_UPDATE_MAIN_SOFTWARE_FLAG+UPDATE_MAIN_SOFTWARE_LEN)		//老化测试用到的地址（适用用于主板上和连接板上的EPPROM）
#define AGINGTEST_LEN								1
//===这里预留了100-24=76字节的空间

//===
#define EPPROM_ADDR_DATA_MANAGE             		100 																//数据管理信息地址 
#define DATA_MANAGE_LEN          					400
      		                
#define EPPROM_ADDR_DATA_OUTSIDE_NUM   				(EPPROM_ADDR_DATA_MANAGE+DATA_MANAGE_LEN) 							//外部账号样本分析数据容量信息保存地址
#define DATA_OUTSIDE_NUM_LEN       					60

//===仪器信息                            
#define EPPROM_ADDR_MACHINFO_INIT_FLAG          	(EPPROM_ADDR_DATA_OUTSIDE_NUM+DATA_OUTSIDE_NUM_LEN) 				//MachInfo.initFlag
#define MACHINFO_INIT_FLAG_LEN         				4

#define EEPROM_ADDR_MACHINFO_INIT_TOUCH        	    (EPPROM_ADDR_MACHINFO_INIT_FLAG+MACHINFO_INIT_FLAG_LEN)				//MachInfo.touch
#define MACHINFO_INIT_TOUCH_LEN        	            100

#define EPPROM_ADDR_MACHINFO_FLAG          			(EEPROM_ADDR_MACHINFO_INIT_TOUCH+MACHINFO_INIT_TOUCH_LEN) 			//MachInfo.flag
#define MACHINFO_FLAG_LEN         					32

#define EPPROM_ADDR_MACHINFO_COMPANY       			(EPPROM_ADDR_MACHINFO_FLAG+MACHINFO_FLAG_LEN) 						//MachInfo.companyInfo
#define MACHINFO_COMPANY_LEN         				250

#define EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT		(EPPROM_ADDR_MACHINFO_COMPANY+MACHINFO_COMPANY_LEN) 				//MachInfo.catMastSynServEvent
#define MACHINFO_CAT_SYNC_SER_EVENT_LEN         	32

#define EPPROM_ADDR_MACHINFO_PARAMSET				(EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT+MACHINFO_CAT_SYNC_SER_EVENT_LEN) 	//MachInfo.paramSet
#define MACHINFO_PARAMSET_LEN         				160

#define EPPROM_ADDR_MACHINFO_CALIBRATION			(EPPROM_ADDR_MACHINFO_PARAMSET+MACHINFO_PARAMSET_LEN) 				//MachInfo.calibration
#define MACHINFO_CALIBRATION_LEN         			100

#define EPPROM_ADDR_MACHINFO_SYS_SET				(EPPROM_ADDR_MACHINFO_CALIBRATION+MACHINFO_CALIBRATION_LEN) 		//MachInfo.systemSet
#define MACHINFO_SYS_SET_LEN         				100

#define EPPROM_ADDR_MACHINFO_ACCOUNT_MM				(EPPROM_ADDR_MACHINFO_SYS_SET+MACHINFO_SYS_SET_LEN) 				//MachInfo.accountMM
#define MACHINFO_ACCOUNT_MM_LEN         			750

#define EPPROM_ADDR_MACHINFO_PRINT_SET				(EPPROM_ADDR_MACHINFO_ACCOUNT_MM+MACHINFO_ACCOUNT_MM_LEN) 			//MachInfo.printSet
#define MACHINFO_PRINT_SET_LEN         				80

#define EPPROM_ADDR_MACHINFO_LAB_INFO				(EPPROM_ADDR_MACHINFO_PRINT_SET+MACHINFO_PRINT_SET_LEN) 			//MachInfo.labInfo
#define MACHINFO_LAB_INFO_LEN         				160

#define EPPROM_ADDR_MACHINFO_QC_INFO				(EPPROM_ADDR_MACHINFO_LAB_INFO+MACHINFO_LAB_INFO_LEN) 				//MachInfo.qcInfo
#define MACHINFO_QC_INFO_LEN         				800

#define EPPROM_ADDR_MACHINFO_OTHER					(EPPROM_ADDR_MACHINFO_QC_INFO+MACHINFO_QC_INFO_LEN) 				//MachInfo.other
#define MACHINFO_OTHER_LEN         					600


#define EEPROM_ADD_END								(EPPROM_ADDR_MACHINFO_OTHER+MACHINFO_OTHER_LEN)						//EEPROM结束地址













extern __IO DataManage_t g_tDataManage;


HAL_StatusTypeDef EEPROMWriteData(EEPROM_Type_e EEPROMType,uint8_t SlaveAdr,uint16_t Reg,uint8_t* Buf,uint16_t Len);
HAL_StatusTypeDef EEPROMReadData(EEPROM_Type_e EEPROMType,uint8_t SlaveAdr,uint16_t Reg,uint8_t* Buf,uint16_t Len);


//数据管理参数
HAL_StatusTypeDef Get_DataMange_Info(__IO DataManage_t *ptDataManage);
HAL_StatusTypeDef Save_DataMange_Info(__IO DataManage_t *ptDataManage);

void Machine_InitPara(void);

void EEPROM_Init_MachInfo_touch(void);
void EEPROM_AddValue_MachInfo_touch(void);
void EEPROM_Read_MachInfo_touch(void);

void EEPROM_Init_MachInfo_flag(void);
void EEPROM_AddValue_MachInfo_flag(void);
void EEPROM_Read_MachInfo_flag(void);

void EEPROM_Init_MachInfo_companyInfo(void);
void EEPROM_AddValue_MachInfo_companyInfo(void);
void EEPROM_Read_MachInfo_companyInfo(void);

void EEPROM_Init_MachInfo_catMastSynServEvent(void);
void EEPROM_AddValue_MachInfo_catMastSynServEvent(void);
void EEPROM_Read_MachInfo_catMastSynServEvent(void);

void EEPROM_Init_MachInfo_paramSet(void);
void EEPROM_AddValue_MachInfo_paramSet(void);
void EEPROM_Read_MachInfo_paramSet(void);

void EEPROM_Init_MachInfo_calibration(void);
void EEPROM_AddValue_MachInfo_calibration(void);
void EEPROM_Read_MachInfo_calibration(void);

void EEPROM_Init_MachInfo_systemSet(void);
void EEPROM_AddValue_MachInfo_systemSet(void);
void EEPROM_Read_MachInfo_systemSet(void);

void EEPROM_Init_MachInfo_accountMM(void);
void EEPROM_AddValue_MachInfo_accountMM(void);
void EEPROM_Read_MachInfo_accountMM(void);

void EEPROM_Init_MachInfo_printSet(void);
void EEPROM_AddValue_MachInfo_printSet(void);
void EEPROM_Read_MachInfo_printSet(void);

void EEPROM_Init_MachInfo_labInfo(void);
void EEPROM_AddValue_MachInfo_labInfo(void);
void EEPROM_Read_MachInfo_labInfo(void);

void EEPROM_Init_MachInfo_qcInfo(void);
void EEPROM_AddValue_MachInfo_qcInfo(void);
void EEPROM_Read_MachInfo_qcInfo(void);

void EEPROM_Init_MachInfo_other(void);
void EEPROM_AddValue_MachInfo_other(void);
void EEPROM_Read_MachInfo_other(void);






#endif
