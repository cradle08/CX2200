#include "Common.h"
#include "bsp_eeprom.h"





/*全局变量定义区********************************************************/
//全局事件组
osEventFlagsId_t GlobalEventOtherGroupHandle;						//全局事件组--其他相关
osEventFlagsId_t GlobalEventPrintGroupHandle;						//全局事件组--打印机相关
osEventFlagsId_t GlobalEventCatGroupHandle;							//全局事件组--CAT相关





/*函数区**************************************************************/
/***
*从SDRAM中的公共缓存区中申请指定大小的线性内存空间
*参数：
	Size：要申请的内存空间大小，单位字节
*返回值：申请到的SDRAM中的内存地址
***/
uint32_t SDRAM_ApplyCommonBuf(uint32_t Size)
{
	static uint32_t Offset = 0;						//内存偏移量
	uint32_t Adr = 0;
	uint16_t Align = 0;
	
//	printf("Offset0:%u\r\n",Offset);
	
	//4字节对齐地址
	Align = Offset & 0x03;
	Offset += (4-Align);
	
//	printf("Offset1:%u\r\n",Offset);
	
	if(Offset+Size > SDRAM_COMMON_BUF_LEN){
		Adr = SDRAM_COMMON_BUF_ADDR;
		Offset = Size;
	}else{
		Adr = SDRAM_COMMON_BUF_ADDR+Offset;
		Offset += Size;
	}
	
	//初始化区域
	memset((uint8_t*)Adr,0,Size);
	
//	printf("Offset2:%u\r\n",Offset);
	
	return Adr;
}






/***
*绘制上三角形
***/
void UI_DrawSanJiao(SanJiaoType_e SJType,uint16_t CenterX,uint16_t CenterY,GUI_COLOR Color)
{
	//上三角形顶点数组，以三角形内部中心点为原点得出各顶点坐标
	GUI_POINT UpSanJiaoPoints[] = {
		{0,-8},
		{-7,4},
		{7,4},
	};
	//上上三角形顶点数组，以中心点为原点得出各顶点坐标
	GUI_POINT UUpSanJiaoPoints[] = {
		{0,0},
		{-7,0},
		{0,-12},
		{7,0},
		{0,0},
		{7,12},
		{-7,12}
	};
	//下三角形顶点数组，以三角形内部中心点为原点得出各顶点坐标
	GUI_POINT DownSanJiaoPoints[] = {
		{0,8},
		{-7,-4},
		{7,-4},
	};
	//下下三角形顶点数组，以中心点为原点得出各顶点坐标
	GUI_POINT DDownSanJiaoPoints[] = {
		{0,0},
		{-7,0},
		{0,12},
		{7,0},
		{0,0},
		{7,-12},
		{-7,-12}
	};
	//右三角形顶点数组，以三角形左上角为原点得出各顶点坐标
	GUI_POINT YouSanJiaoPoints[] = {
		{0,0},
		{0,12},
		{10,6},
	};
	//左三角形顶点数组，以三角形左上角为原点得出各顶点坐标
	GUI_POINT ZuoSanJiaoPoints[] = {
		{0,6},
		{10,0},
		{10,12},
	};
	
	
	GUI_SetColor(Color);
	
	switch(SJType){
		case UP_SAN_JIAO:
		{
			GUI_AA_FillPolygon(UpSanJiaoPoints,3,CenterX,CenterY);
		}break;
		case UUP_SAN_JIAO:
		{
			GUI_AA_FillPolygon(UUpSanJiaoPoints,7,CenterX,CenterY);
		}break;
		case DOWN_SAN_JIAO:
		{
			GUI_AA_FillPolygon(DownSanJiaoPoints,3,CenterX,CenterY);
		}break;
		case DDOWN_SAN_JIAO:
		{
			GUI_AA_FillPolygon(DDownSanJiaoPoints,7,CenterX,CenterY);
		}break;
		case ZUO_SAN_JIAO:
		{
			GUI_AA_FillPolygon(ZuoSanJiaoPoints,3,CenterX,CenterY);
		}break;
		case YOU_SAN_JIAO:
		{
			GUI_AA_FillPolygon(YouSanJiaoPoints,3,CenterX,CenterY);
		}break;
		default :break;
	}
}




/***
*配置不同公司的特性参数信息
***/
void Company_ConfigInfo(Company_e Company)
{
	switch(Company){
		//配置锦瑞信息
		case COMPANY_JIN_RUI:
		{
			//MQTT
			strcpy(MachInfo.companyInfo.mqtt.ip,"");
			strcpy(MachInfo.companyInfo.mqtt.name,"");
			strcpy(MachInfo.companyInfo.mqtt.psw,"");
			
			//皮肤
			MachInfo.companyInfo.skin.bg = 0x00e8f4e0;
			MachInfo.companyInfo.skin.buttNotPress = 0x00f7f7f7;
			MachInfo.companyInfo.skin.buttNotPressFont = 0x00000000;
			MachInfo.companyInfo.skin.buttPress = 0x0098cc60;
			MachInfo.companyInfo.skin.buttPressFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.buttBoard = 0x00cccccc;
			MachInfo.companyInfo.skin.highlightKJNotSelect = 0x006d9823;
			MachInfo.companyInfo.skin.highlightKHNotSelectFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.highlightKJSelect = 0x0044640a;
			MachInfo.companyInfo.skin.highlightKJSelectFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.title = 0x00e8f4e0;
			MachInfo.companyInfo.skin.titleFont = 0x00000000;
			
			MachInfo.printSet.ePrintWay = PRINTF_WAY_AUTO;
			
			memset(MachInfo.labInfo.ucaClinicName,0,CLINIC_NAME_MAX_LEN);
			strcpy((char*)MachInfo.labInfo.ucaClinicName,"");
			
			memset(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,0,ACCOUNT_PSW_MAX_LEN);
			strcpy((char*)MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,"admin");
		}break;
		
		//配置药师帮信息
		case COMPANY_YSB:
		{
			//MQTT
			strcpy(MachInfo.companyInfo.mqtt.ip,"ch-mqtt.guangpuyun.cn");
			strcpy(MachInfo.companyInfo.mqtt.name,(char*)MachInfo.labInfo.ucaMachineSN);
			strcpy(MachInfo.companyInfo.mqtt.psw,"");
			
			//皮肤
			MachInfo.companyInfo.skin.bg = 0x00f5e6c5;
			MachInfo.companyInfo.skin.buttNotPress = 0x00f7f7f7;
			MachInfo.companyInfo.skin.buttNotPressFont = 0x00000000;
			MachInfo.companyInfo.skin.buttPress = 0x00c68803;
			MachInfo.companyInfo.skin.buttPressFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.buttBoard = 0x00cccccc;
			MachInfo.companyInfo.skin.highlightKJNotSelect = 0x00c68803;
			MachInfo.companyInfo.skin.highlightKHNotSelectFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.highlightKJSelect = 0x00664602;
			MachInfo.companyInfo.skin.highlightKJSelectFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.title = 0x00f1e7d2;
			MachInfo.companyInfo.skin.titleFont = 0x00000000;
			
			MachInfo.printSet.ePrintWay = PRINTF_WAY_MANUAL;
			
			memset(MachInfo.labInfo.ucaClinicName,0,CLINIC_NAME_MAX_LEN);
			strcpy((char*)MachInfo.labInfo.ucaClinicName,"广州阅微");
			
			memset(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,0,ACCOUNT_PSW_MAX_LEN);
			strcpy((char*)MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,"HtCgG1nzh");
		}break;
		
		//配置和映信息
		case COMPANY_HY:
		{
			//MQTT
			strcpy(MachInfo.companyInfo.mqtt.ip,"");
			strcpy(MachInfo.companyInfo.mqtt.name,"");
			strcpy(MachInfo.companyInfo.mqtt.psw,"");
			
			//皮肤
			MachInfo.companyInfo.skin.bg = 0x00f5e6c5;
			MachInfo.companyInfo.skin.buttNotPress = 0x00f7f7f7;
			MachInfo.companyInfo.skin.buttNotPressFont = 0x00000000;
			MachInfo.companyInfo.skin.buttPress = 0x00c68803;
			MachInfo.companyInfo.skin.buttPressFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.buttBoard = 0x00cccccc;
			MachInfo.companyInfo.skin.highlightKJNotSelect = 0x00c68803;
			MachInfo.companyInfo.skin.highlightKHNotSelectFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.highlightKJSelect = 0x00664602;
			MachInfo.companyInfo.skin.highlightKJSelectFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.title = 0x00f1e7d2;
			MachInfo.companyInfo.skin.titleFont = 0x00000000;
			
			MachInfo.printSet.ePrintWay = PRINTF_WAY_AUTO;
			
			memset(MachInfo.labInfo.ucaClinicName,0,CLINIC_NAME_MAX_LEN);
			strcpy((char*)MachInfo.labInfo.ucaClinicName,"");
			
			memset(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,0,ACCOUNT_PSW_MAX_LEN);
			strcpy((char*)MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,"admin");
		}break;
		
		//默认创怀信息
		default :
		{
			//MQTT
			strcpy(MachInfo.companyInfo.mqtt.ip,"");
			strcpy(MachInfo.companyInfo.mqtt.name,"");
			strcpy(MachInfo.companyInfo.mqtt.psw,"");
			
			//皮肤
			MachInfo.companyInfo.skin.bg = 0x00f5e6c5;
			MachInfo.companyInfo.skin.buttNotPress = 0x00f7f7f7;
			MachInfo.companyInfo.skin.buttNotPressFont = 0x00000000;
			MachInfo.companyInfo.skin.buttPress = 0x00c68803;
			MachInfo.companyInfo.skin.buttPressFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.buttBoard = 0x00cccccc;
			MachInfo.companyInfo.skin.highlightKJNotSelect = 0x00c68803;
			MachInfo.companyInfo.skin.highlightKHNotSelectFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.highlightKJSelect = 0x00664602;
			MachInfo.companyInfo.skin.highlightKJSelectFont = 0x00FFFFFF;
			MachInfo.companyInfo.skin.title = 0x00f1e7d2;
			MachInfo.companyInfo.skin.titleFont = 0x00000000;
			
			MachInfo.printSet.ePrintWay = PRINTF_WAY_AUTO;
			
			memset(MachInfo.labInfo.ucaClinicName,0,CLINIC_NAME_MAX_LEN);
			strcpy((char*)MachInfo.labInfo.ucaClinicName,"");
			
			memset(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,0,ACCOUNT_PSW_MAX_LEN);
			strcpy((char*)MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,"admin");
		}break;
	}
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_COMPANY,(uint8_t*)&MachInfo.companyInfo,sizeof(MachInfo.companyInfo));
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PRINT_SET+((uint8_t*)&MachInfo.printSet.ePrintWay-(uint8_t*)&MachInfo.printSet),(uint8_t*)&MachInfo.printSet.ePrintWay,sizeof(MachInfo.printSet.ePrintWay));
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_LAB_INFO,(uint8_t*)&MachInfo.labInfo,sizeof(MachInfo.labInfo));
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_ACCOUNT_MM,(uint8_t*)&MachInfo.accountMM,sizeof(MachInfo.accountMM));
}



/***
*根据年龄数值和单位，以及性别自动选择对应的参考组
***/
ReferGroup_e AgeToRefGroup(uint8_t Age,AgeUnit_e AgeUnit,SexType_e Sex)
{
	extern __IO MachRunPara_s MachRunPara;
	
	switch(MachRunPara.tNextSample.eAgeUnit){
		case AGE_UNIT_DAY:
		{
			if(Age <= 28){
				//新生儿
				return REFER_GROUP_BOBY;
			}else{
				//儿童
				return REFER_GROUP_CHILDREN;
			}
		}
		case AGE_UNIT_WEEK:
		{
			if(Age*7 <= 28){
				//新生儿
				return REFER_GROUP_BOBY;
			}else{
				//儿童
				return REFER_GROUP_CHILDREN;
			}
		}
		case AGE_UNIT_MONTH:
		{
			if(Age <= 168){
				//儿童
				return REFER_GROUP_CHILDREN;
			}else{
				if(Sex == SEX_TYPE_MALE){
					//成男
					return REFER_GROUP_MALE;
				}else if(Sex == SEX_TYPE_FEMALE){
					//成女
					return REFER_GROUP_FEMALE;
				}else{
					//通用
					return REFER_GROUP_CMOMMON;
				}
			}
		}
		case AGE_UNIT_YEAR:
		{
			if(Age <= 14){
				//儿童
				return REFER_GROUP_CHILDREN;
			}else{
				if(Sex == SEX_TYPE_MALE){
					//成男
					return REFER_GROUP_MALE;
				}else if(Sex == SEX_TYPE_FEMALE){
					//成女
					return REFER_GROUP_FEMALE;
				}else{
					//通用
					return REFER_GROUP_CMOMMON;
				}
			}
		}
		default :return REFER_GROUP_CMOMMON;
	}
}



















