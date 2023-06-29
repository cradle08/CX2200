/*********************************************************************
*HGB相关处理动作区
**********************************************************************/
#include "HGB_Deal.h"
#include "WM.h"
#include "Public_menuDLG.h"
#include "DIALOG.h"
#include "bsp_light.h"
#include "monitorV.h"
#include "math.h"
#include "Common.h"
#include "bsp_eeprom.h"
#include "alg.h"
#include "modedefs.h"
#include "algpulse.h"






/*宏定义区***********************************************************************/
//HGB调校页面控件ID
#define ID_MULTIEDIT_0							(HGB_SELF_CHECK_WINDOW_ID + 0x14)
#define ID_BUTTON_0 							(HGB_SELF_CHECK_WINDOW_ID + 0x0B)
#define ID_BUTTON_1 							(HGB_SELF_CHECK_WINDOW_ID + 0x0C)
#define ID_BUTTON_2 							(HGB_SELF_CHECK_WINDOW_ID + 0x13)
#define ID_BUTTON_3 							(HGB_SELF_CHECK_WINDOW_ID + 0x15)
#define ID_BUTTON_4 							(HGB_SELF_CHECK_WINDOW_ID + 0x16)
#define ID_BUTTON_5 							(HGB_SELF_CHECK_WINDOW_ID + 0x17)
#define ID_BUTTON_6 							(HGB_SELF_CHECK_WINDOW_ID + 0x18)
#define ID_EDIT_4 								(HGB_SELF_CHECK_WINDOW_ID + 0x0F)
#define ID_EDIT_5 								(HGB_SELF_CHECK_WINDOW_ID + 0x10)
#define ID_EDIT_6 								(HGB_SELF_CHECK_WINDOW_ID + 0x11)











/*函数区*****************************************************************/
/***
*计算当前HGB的平均ADC值，采用连续采10个点，去除头尾各3个点后求均值方式
***/
uint16_t Get_HGBAvgADC(uint16_t Time)
{
	uint16_t ADCValue[10] = {0};
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t Temp = 0;
	uint32_t Sum = 0;
	
	//将面板指示灯熄灭
	g_ePanel_LED_Status = PANEL_LED_STATUS_OFF;
	osDelay(60);
	
	for(i=0;i<10;i++){
		ADCValue[i] = HGB_ADC();
		osDelay(Time);
	}
	
	//将面板指示灯恢复正常
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//冒泡排序
	for(i=0;i<10;i++){
		for(j=0;j<9-i;j++){
			if(ADCValue[j] < ADCValue[j+1]){
				Temp = ADCValue[j];
				ADCValue[j] = ADCValue[j+1];
				ADCValue[j+1] = Temp;
			}
		}
	}
	
	//求平均
	Sum = 0;
	for(i=3;i<7;i++){
		Sum += ADCValue[i];
	}
	
	return Sum/4;
}





/***
*HGB调校界面中的HGB测试，临时方案
***/
void HGBAdjust_HGBTest(void)
{
	extern __IO WM_HWIN g_hActiveWin;					//当前显示窗口的句柄
	extern MachInfo_s MachInfo;
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern __IO uint8_t g_CountHandle_Status;
	WM_HWIN hItem;
	char Str[50] = {0};
	uint16_t Avg535ADC = 0;
	uint16_t Avg415ADC = 0;
	float HGBValue = 0.0f;
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
	MULTIEDIT_SetText(hItem,"HGB测试中，请等待...");
	
	//先开535灯
	Light_Start(HGBLED_535,MachInfo.paramSet.current535);
	osDelay(5000);
	
	Avg535ADC = Get_HGBAvgADC(100);
	
	Light_Stop();
	
	osDelay(50);
	
	//开415灯
	Light_Start(HGBLED_415,MachInfo.paramSet.current415);
	osDelay(3000);
	
	Avg415ADC = Get_HGBAvgADC(100);
	
	Light_Stop();
	
	HGBValue = (log10(3500.0f/Avg415ADC) - log10(3500.0f/Avg535ADC))*HGB_CURVE_FIT_K2;
	
	memset(Str,0,50);
	sprintf(Str,"415ADC:%u,,,535ADC:%u,,,HGB:%lf\r\n",Avg415ADC,Avg535ADC,HGBValue);
	MULTIEDIT_SetText(hItem,"HGB测试完毕\r\n");
	MULTIEDIT_AddText(hItem,Str);
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//HGB调校界面按钮恢复
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_0));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_1));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_2));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_3));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_4));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_5));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_6));
}









/***
*HGB调校界面中的开415灯
***/
void HGBAdjust_Open415(void)
{
	extern __IO WM_HWIN g_hActiveWin;					//当前显示窗口的句柄
	extern MachInfo_s MachInfo;
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern __IO uint8_t g_CountHandle_Status;
	extern osEventFlagsId_t GlobalEventOtherGroupHandle;
	WM_HWIN hItem;
	char Str[50] = {0};
	uint16_t Avg415ADC = 0;
	uint32_t EventBits = 0;
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
	MULTIEDIT_SetText(hItem,"415灯开启中，请等待...");
	
	//开415灯
	Light_Start(HGBLED_415,MachInfo.paramSet.current415);
	osDelay(5000);
	
	while(1){
		EventBits = osEventFlagsWait(GlobalEventOtherGroupHandle,GLOBAL_EVENT_OTHER_HGB_ADJUST_CLOSE,osFlagsWaitAny,0);
		if(EventBits != osFlagsErrorTimeout && EventBits != osFlagsErrorResource && EventBits != osFlagsErrorParameter &&(EventBits & GLOBAL_EVENT_OTHER_HGB_ADJUST_CLOSE) == GLOBAL_EVENT_OTHER_HGB_ADJUST_CLOSE){
			//关灯
			break;
		}
		
		Avg415ADC = Get_HGBAvgADC(100);
		
		memset(Str,0,50);
		sprintf(Str,"415ADC:%u\r\n",Avg415ADC);
		MULTIEDIT_SetText(hItem,Str);
	}
	
	Light_Stop();
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//HGB调校界面按钮恢复
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_0));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_1));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_2));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_3));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_4));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_5));
}




/***
*HGB调校界面中的开535灯
***/
void HGBAdjust_Open535(void)
{
	extern __IO WM_HWIN g_hActiveWin;					//当前显示窗口的句柄
	extern MachInfo_s MachInfo;
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern __IO uint8_t g_CountHandle_Status;
	extern osEventFlagsId_t GlobalEventOtherGroupHandle;
	WM_HWIN hItem;
	char Str[50] = {0};
	uint16_t Avg535ADC = 0;
	uint32_t EventBits = 0;
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
	MULTIEDIT_SetText(hItem,"535灯开启中，请等待...");
	
	//开535灯
	Light_Start(HGBLED_535,MachInfo.paramSet.current535);
	osDelay(5000);
	
	while(1){
		EventBits = osEventFlagsWait(GlobalEventOtherGroupHandle,GLOBAL_EVENT_OTHER_HGB_ADJUST_CLOSE,osFlagsWaitAny,0);
		if(EventBits != osFlagsErrorTimeout && EventBits != osFlagsErrorResource && EventBits != osFlagsErrorParameter &&(EventBits & GLOBAL_EVENT_OTHER_HGB_ADJUST_CLOSE) == GLOBAL_EVENT_OTHER_HGB_ADJUST_CLOSE){
			//关灯
			break;
		}
		
		Avg535ADC = Get_HGBAvgADC(100);
		
		memset(Str,0,50);
		sprintf(Str,"535ADC:%u\r\n",Avg535ADC);
		MULTIEDIT_SetText(hItem,Str);
	}
	
	Light_Stop();
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//HGB调校界面按钮恢复
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_0));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_1));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_2));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_3));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_4));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_5));
}



/***
*HGB调校界面中的空气校准
***/
void HGBAdjust_KQAuto(void)
{
	extern __IO WM_HWIN g_hActiveWin;					//当前显示窗口的句柄
	extern MachInfo_s MachInfo;
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern __IO uint8_t g_CountHandle_Status;
	
	WM_HWIN hItem;
	char Str[1024] = {0};
	char StrTemp[20] = {0};
	uint16_t Len = 0;
	uint16_t TempLen = 0;
	uint16_t Avg1 = 0;
	uint16_t Avg2 = 0;
	float K = 0.0f;
	float B = 0.0f;
	uint16_t Current415 = 0;
	uint16_t Current535 = 0;
	uint8_t Flag1 = 0;
	uint8_t Flag2 = 0;
	uint8_t RetryCount = 0;
	
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
	Len += sprintf(Str+Len,"空气校准\r\n");
	Str[Len] = '\0';
	MULTIEDIT_SetText(hItem,Str);
	
	//===校准415
	TempLen += sprintf(Str+Len,"校准415中：\r\n");
	Str[Len+TempLen] = '\0';
	MULTIEDIT_SetText(hItem,Str);
	
	//获取2个线性点
	Light_Start(HGBLED_415,MachInfo.paramSet.point1Curr415);
	osDelay(5000);
	Avg1 = Get_HGBAvgADC(100);
	
	Light_Start(HGBLED_415,MachInfo.paramSet.point2Curr415);
	osDelay(3000);
	Avg2 = Get_HGBAvgADC(100);
	
	//计算K,B
	K = 1.0f*(Avg2-Avg1)/(MachInfo.paramSet.point2Curr415-MachInfo.paramSet.point1Curr415);
	B = 1.0f*Avg1 - K*MachInfo.paramSet.point1Curr415;
	
	//计算预估电流值
	Current415 = 1.0f*(MachInfo.paramSet.KQADC415 - B)/K;
	
	while(1){
		Light_Start(HGBLED_415,Current415);
		osDelay(2000);
		Avg1 = Get_HGBAvgADC(100);
		
		//显示当前电流值和对应ADC值
		TempLen += sprintf(Str+Len,"校准415中，电流：%u，ADC：%u\r\n",Current415,Avg1);
		Str[Len+TempLen] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		if(Avg1 > MachInfo.paramSet.KQADC415+2){
			if(Flag2 == 1){
				//表示步进为1开始调整
				Flag1 = 2;
			}else if(Flag2 == 2 && Flag1 == 1){
				//表示无法校准成功
				RetryCount++;
				
				Len += sprintf(Str+Len,"校准415【失败】%u次，电流：%u，ADC：%u\r\n",RetryCount,Current415,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				if(RetryCount >= 3){
					break;
				}else{
					//给3次重试机会
					Flag1 = 0;
					Flag2 = 0;
				}
			}else if(Flag2 == 0){
				Flag1 = 1;
			}
			
			if(Flag1 == 1){
				Current415 -= 1.0f*(Avg1 - MachInfo.paramSet.KQADC415)/K;
			}else if(Flag1 == 2){
				Current415 -= 1;
			}
			
			//判断电流是否已经超限，是，则直接报校准异常
			if(Current415 > 4000){
				//表示无法校准成功
				Len += sprintf(Str+Len,"校准415【失败】，电流：%u，ADC：%u\r\n",Current415,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				break;
			}
		}else if(Avg1 < MachInfo.paramSet.KQADC415-2){
			if(Flag1 == 1){
				//表示步进为1开始调整
				Flag2 = 2;
			}else if(Flag1 == 2 && Flag2 == 1){
				//表示无法校准成功
				RetryCount++;
				
				Len += sprintf(Str+Len,"校准415【失败】%u次，电流：%u，ADC：%u\r\n",RetryCount,Current415,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				if(RetryCount >= 3){
					break;
				}else{
					//给3次重试机会
					Flag1 = 0;
					Flag2 = 0;
				}
			}else if(Flag1 == 0){
				Flag2 = 1;
			}
			
			if(Flag2 == 1){
				Current415 += 1.0f*(MachInfo.paramSet.KQADC415 - Avg1)/K;
			}else if(Flag2 == 2){
				Current415 += 1;
			}
			
			//判断电流是否已经超限，是，则直接报校准异常
			if(Current415 > 4000){
				//表示无法校准成功
				Len += sprintf(Str+Len,"校准415【失败】，电流：%u，ADC：%u\r\n",Current415,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				break;
			}
		}else{
			//表示校准成功
			Len += sprintf(Str+Len,"校准415【成功】，电流：%u，ADC：%u\r\n",Current415,Avg1);
			Str[Len] = '\0';
			MULTIEDIT_SetText(hItem,Str);
			
			MachInfo.paramSet.current415 = Current415;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.current415-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.current415,sizeof(MachInfo.paramSet.current415));
			
			//更新界面显示
			hItem = WM_GetDialogItem(g_hActiveWin, ID_EDIT_4);
			memset(StrTemp,0,20);
			sprintf(StrTemp,"%u",MachInfo.paramSet.current415);
			EDIT_SetText(hItem,StrTemp);
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			
			break;
		}
	}
	
	Light_Stop();
	osDelay(50);
	
	Flag1 = 0;
	Flag2 = 0;
	RetryCount = 0;
	
	//===校准535
	TempLen += sprintf(Str+Len,"校准535中：\r\n");
	Str[Len+TempLen] = '\0';
	MULTIEDIT_SetText(hItem,Str);
	
	//获取2个线性点
	Light_Start(HGBLED_535,MachInfo.paramSet.point1Curr535);
	osDelay(5000);
	Avg1 = Get_HGBAvgADC(100);
	
	Light_Start(HGBLED_535,MachInfo.paramSet.point2Curr535);
	osDelay(3000);
	Avg2 = Get_HGBAvgADC(100);
	
	//计算K,B
	K = 1.0f*(Avg2-Avg1)/(MachInfo.paramSet.point2Curr535-MachInfo.paramSet.point1Curr535);
	B = 1.0f*Avg1 - K*MachInfo.paramSet.point1Curr535;
	
	//计算预估电流值
	Current535 = 1.0f*(MachInfo.paramSet.KQADC535 - B)/K;
	
	while(1){
		Light_Start(HGBLED_535,Current535);
		osDelay(2000);
		Avg1 = Get_HGBAvgADC(100);
		
		//显示当前电流值和对应ADC值
		TempLen += sprintf(Str+Len,"校准535中，电流：%u，ADC：%u\r\n",Current535,Avg1);
		Str[Len+TempLen] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		if(Avg1 > MachInfo.paramSet.KQADC535+2){
			if(Flag2 == 1){
				//表示步进为1开始调整
				Flag1 = 2;
			}else if(Flag2 == 2 && Flag1 == 1){
				//表示无法校准成功
				RetryCount++;
				
				Len += sprintf(Str+Len,"校准535【失败】%u次，电流：%u，ADC：%u\r\n",RetryCount,Current535,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				if(RetryCount >= 3){
					break;
				}else{
					//给3次重试机会
					Flag1 = 0;
					Flag2 = 0;
				}
			}else if(Flag2 == 0){
				Flag1 = 1;
			}
			
			if(Flag1 == 1){
				Current535 -= 1.0f*(Avg1 - MachInfo.paramSet.KQADC535)/K;
			}else if(Flag1 == 2){
				Current535 -= 1;
			}
			
			//判断电流是否已经超限，是，则直接报校准异常
			if(Current535 > 4000){
				//表示无法校准成功
				Len += sprintf(Str+Len,"校准535【失败】，电流：%u，ADC：%u\r\n",Current535,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				break;
			}
		}else if(Avg1 < MachInfo.paramSet.KQADC535-2){
			if(Flag1 == 1){
				//表示步进为1开始调整
				Flag2 = 2;
			}else if(Flag1 == 2 && Flag2 == 1){
				//表示无法校准成功
				RetryCount++;
				
				Len += sprintf(Str+Len,"校准535【失败】%u次，电流：%u，ADC：%u\r\n",RetryCount,Current535,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				if(RetryCount >= 3){
					break;
				}else{
					//给3次重试机会
					Flag1 = 0;
					Flag2 = 0;
				}
			}else if(Flag1 == 0){
				Flag2 = 1;
			}
			
			if(Flag2 == 1){
				Current535 += 1.0f*(MachInfo.paramSet.KQADC535 - Avg1)/K;
			}else if(Flag2 == 2){
				Current535 += 1;
			}
			
			//判断电流是否已经超限，是，则直接报校准异常
			if(Current535 > 4000){
				//表示无法校准成功
				Len += sprintf(Str+Len,"校准535【失败】，电流：%u，ADC：%u\r\n",Current535,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				break;
			}
		}else{
			//表示校准成功
			Len += sprintf(Str+Len,"校准535【成功】，电流：%u，ADC：%u\r\n",Current535,Avg1);
			Str[Len] = '\0';
			MULTIEDIT_SetText(hItem,Str);
			
			MachInfo.paramSet.current535 = Current535;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.current535-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.current535,sizeof(MachInfo.paramSet.current535));
			
			//更新界面显示
			hItem = WM_GetDialogItem(g_hActiveWin, ID_EDIT_5);
			memset(StrTemp,0,20);
			sprintf(StrTemp,"%u",MachInfo.paramSet.current535);
			EDIT_SetText(hItem,StrTemp);
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			
			break;
		}
	}
	
	Light_Stop();
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//HGB调校界面按钮恢复
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_0));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_1));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_2));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_3));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_4));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_5));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_6));
}



/***
*HGB调校界面中的空白校准
***/
void HGBAdjust_KBAuto(void)
{
	extern __IO WM_HWIN g_hActiveWin;					//当前显示窗口的句柄
	extern MachInfo_s MachInfo;
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern __IO uint8_t g_CountHandle_Status;
	WM_HWIN hItem;
	char Str[1024] = {0};
	char StrTemp[20] = {0};
	uint16_t Len = 0;
	uint16_t TempLen = 0;
	uint16_t Avg1 = 0;
	uint16_t Avg2 = 0;
	float K = 0.0f;
	float B = 0.0f;
	uint16_t Current415 = 0;
	uint16_t Current535 = 0;
	uint8_t Flag1 = 0;
	uint8_t Flag2 = 0;
	uint8_t RetryCount = 0;
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
	Len += sprintf(Str+Len,"空白校准\r\n");
	Str[Len] = '\0';
	MULTIEDIT_SetText(hItem,Str);
	
	//===校准415
	TempLen += sprintf(Str+Len,"校准415中：\r\n");
	Str[Len+TempLen] = '\0';
	MULTIEDIT_SetText(hItem,Str);
	
	//获取2个线性点
	Light_Start(HGBLED_415,MachInfo.paramSet.point1Curr415);
	osDelay(5000);
	Avg1 = Get_HGBAvgADC(100);
	
	Light_Start(HGBLED_415,MachInfo.paramSet.point2Curr415);
	osDelay(3000);
	Avg2 = Get_HGBAvgADC(100);
	
	//计算K,B
	K = 1.0f*(Avg2-Avg1)/(MachInfo.paramSet.point2Curr415-MachInfo.paramSet.point1Curr415);
	B = 1.0f*Avg1 - K*MachInfo.paramSet.point1Curr415;
	
	//计算预估电流值
	Current415 = 1.0f*(MachInfo.paramSet.KBADC415 - B)/K;
	
	while(1){
		Light_Start(HGBLED_415,Current415);
		osDelay(2000);
		Avg1 = Get_HGBAvgADC(100);
		
		//显示当前电流值和对应ADC值
		TempLen += sprintf(Str+Len,"校准415中，电流：%u，ADC：%u\r\n",Current415,Avg1);
		Str[Len+TempLen] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		if(Avg1 > MachInfo.paramSet.KBADC415+2){
			if(Flag2 == 1){
				//表示步进为1开始调整
				Flag1 = 2;
			}else if(Flag2 == 2 && Flag1 == 1){
				//表示无法校准成功
				RetryCount++;
				
				Len += sprintf(Str+Len,"校准415【失败】%u次，电流：%u，ADC：%u\r\n",RetryCount,Current415,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				if(RetryCount >= 3){
					break;
				}else{
					//给3次重试机会
					Flag1 = 0;
					Flag2 = 0;
				}
			}else if(Flag2 == 0){
				Flag1 = 1;
			}
			
			if(Flag1 == 1){
				Current415 -= 1.0f*(Avg1 - MachInfo.paramSet.KBADC415)/K;
			}else if(Flag1 == 2){
				Current415 -= 1;
			}
			
			//判断电流是否已经超限，是，则直接报校准异常
			if(Current415 > 4000){
				//表示无法校准成功
				//显示当前电流值和对应ADC值
				Len += sprintf(Str+Len,"校准415【失败】，电流：%u，ADC：%u\r\n",Current415,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				break;
			}
		}else if(Avg1 < MachInfo.paramSet.KBADC415-2){
			if(Flag1 == 1){
				//表示步进为1开始调整
				Flag2 = 2;
			}else if(Flag1 == 2 && Flag2 == 1){
				//表示无法校准成功
				RetryCount++;
				
				//表示无法校准成功
				Len += sprintf(Str+Len,"校准415【失败】%u次，电流：%u，ADC：%u\r\n",RetryCount,Current415,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				if(RetryCount >= 3){
					break;
				}else{
					//给3次重试机会
					Flag1 = 0;
					Flag2 = 0;
				}
			}else if(Flag1 == 0){
				Flag2 = 1;
			}
			
			if(Flag2 == 1){
				Current415 += 1.0f*(MachInfo.paramSet.KBADC415 - Avg1)/K;
			}else if(Flag2 == 2){
				Current415 += 1;
			}
			
			//判断电流是否已经超限，是，则直接报校准异常
			if(Current415 > 4000){
				//表示无法校准成功
				//显示当前电流值和对应ADC值
				Len += sprintf(Str+Len,"校准415【失败】，电流：%u，ADC：%u\r\n",Current415,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				break;
			}
		}else{
			//表示校准成功
			Len += sprintf(Str+Len,"校准415成功，电流：%u，ADC：%u\r\n",Current415,Avg1);
			Str[Len] = '\0';
			MULTIEDIT_SetText(hItem,Str);
			
			MachInfo.paramSet.current415 = Current415;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.current415-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.current415,sizeof(MachInfo.paramSet.current415));
			
			//更新界面显示
			hItem = WM_GetDialogItem(g_hActiveWin, ID_EDIT_4);
			memset(StrTemp,0,20);
			sprintf(StrTemp,"%u",MachInfo.paramSet.current415);
			EDIT_SetText(hItem,StrTemp);
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			
			break;
		}
	}
	
	Light_Stop();
	osDelay(50);
	
	Flag1 = 0;
	Flag2 = 0;
	RetryCount = 0;
	
	//===校准535
	TempLen += sprintf(Str+Len,"校准535中：\r\n");
	Str[Len+TempLen] = '\0';
	MULTIEDIT_SetText(hItem,Str);
	
	//获取2个线性点
	Light_Start(HGBLED_535,MachInfo.paramSet.point1Curr535);
	osDelay(5000);
	Avg1 = Get_HGBAvgADC(100);
	
	Light_Start(HGBLED_535,MachInfo.paramSet.point2Curr535);
	osDelay(3000);
	Avg2 = Get_HGBAvgADC(100);
	
	//计算K,B
	K = 1.0f*(Avg2-Avg1)/(MachInfo.paramSet.point2Curr535-MachInfo.paramSet.point1Curr535);
	B = 1.0f*Avg1 - K*MachInfo.paramSet.point1Curr535;
	
	//计算预估电流值
	Current535 = 1.0f*(MachInfo.paramSet.KBADC535 - B)/K;
	
	while(1){
		Light_Start(HGBLED_535,Current535);
		osDelay(2000);
		Avg1 = Get_HGBAvgADC(100);
		
		//显示当前电流值和对应ADC值
		TempLen += sprintf(Str+Len,"校准535中，电流：%u，ADC：%u\r\n",Current535,Avg1);
		Str[Len+TempLen] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		if(Avg1 > MachInfo.paramSet.KBADC535+2){
			if(Flag2 == 1){
				//表示步进为1开始调整
				Flag1 = 2;
			}else if(Flag2 == 2 && Flag1 == 1){
				//表示无法校准成功
				RetryCount++;
				
				Len += sprintf(Str+Len,"校准535【失败】%u次，电流：%u，ADC：%u\r\n",RetryCount,Current535,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				if(RetryCount >= 3){
					break;
				}else{
					//给3次重试机会
					Flag1 = 0;
					Flag2 = 0;
				}
			}else if(Flag2 == 0){
				Flag1 = 1;
			}
			
			if(Flag1 == 1){
				Current535 -= 1.0f*(Avg1 - MachInfo.paramSet.KBADC535)/K;
			}else if(Flag1 == 2){
				Current535 -= 1;
			}
			
			//判断电流是否已经超限，是，则直接报校准异常
			if(Current535 > 4000){
				//表示无法校准成功
				//显示当前电流值和对应ADC值
				Len += sprintf(Str+Len,"校准535【失败】，电流：%u，ADC：%u\r\n",Current535,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				break;
			}
		}else if(Avg1 < MachInfo.paramSet.KBADC535-2){
			if(Flag1 == 1){
				//表示步进为1开始调整
				Flag2 = 2;
			}else if(Flag1 == 2 && Flag2 == 1){
				//表示无法校准成功
				RetryCount++;
				
				Len += sprintf(Str+Len,"校准535【失败】%u次，电流：%u，ADC：%u\r\n",RetryCount,Current535,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				if(RetryCount >= 3){
					break;
				}else{
					//给3次重试机会
					Flag1 = 0;
					Flag2 = 0;
				}
			}else if(Flag1 == 0){
				Flag2 = 1;
			}
			
			if(Flag2 == 1){
				Current535 += 1.0f*(MachInfo.paramSet.KBADC535 - Avg1)/K;
			}else if(Flag2 == 2){
				Current535 += 1;
			}
			
			//判断电流是否已经超限，是，则直接报校准异常
			if(Current535 > 4000){
				//表示无法校准成功
				//显示当前电流值和对应ADC值
				Len += sprintf(Str+Len,"校准535【失败】，电流：%u，ADC：%u\r\n",Current535,Avg1);
				Str[Len] = '\0';
				MULTIEDIT_SetText(hItem,Str);
				
				break;
			}
		}else{
			//表示校准成功
			Len += sprintf(Str+Len,"校准535成功，电流：%u，ADC：%u\r\n",Current535,Avg1);
			Str[Len] = '\0';
			MULTIEDIT_SetText(hItem,Str);
			
			MachInfo.paramSet.current535 = Current535;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.current535-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.current535,sizeof(MachInfo.paramSet.current535));
			
			//更新界面显示
			hItem = WM_GetDialogItem(g_hActiveWin, ID_EDIT_5);
			memset(StrTemp,0,20);
			sprintf(StrTemp,"%u",MachInfo.paramSet.current535);
			EDIT_SetText(hItem,StrTemp);
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			
			break;
		}
	}
	
	Light_Stop();
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//HGB调校界面按钮恢复
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_0));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_1));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_2));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_3));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_4));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_5));
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_6));
}









