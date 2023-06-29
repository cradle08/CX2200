/*************************************************************************
*PCBA检验执行项
**************************************************************************/
#include "pcba.h"
#include "bsp_rtc.h"
#include "WM.h"
#include "Public_menuDLG.h"
#include "DIALOG.h"
#include "bsp_outin.h"
#include "math.h"
#include "monitorV.h"
#include "cmsis_os2.h"
#include "CAT_iot.h"
#include "Printer.h"
#include "bsp_spi.h"
#include "bsp_eeprom.h"
#include "bsp_beep.h"
#include "bsp_pump.h"
#include "bsp_press.h"
#include "bsp_motor.h"
#include "bsp_countsignal.h"
#include "process.h"
#include "cmsis_os.h"
#include "bsp_light.h"
#include "bsp_resistance.h"
#include "GUI_Touch.h"
#include "xpt2046.h"
#include "digPressure.h"
#include "smpdatadefs.h"
#include "algpulse.h"
#include "usart1CMD.h"



/*宏定义区***************************************************************/
#define MULTIEDIT_PCBA_ID				 	(GUI_WINDOW_PCBA_PAGE_ID + 0x0A)				//PCBA检验页面多行文本显示控件ID
#define EDIT_PCBA_ID						(GUI_WINDOW_PCBA_PAGE_ID + 0x0C)				//PCBA检验页面编辑框控件ID
#define CONFIM_BUTT_PCBA_ID					(GUI_WINDOW_PCBA_PAGE_ID + 0x0D)				//PCBA检验页面确认按钮控件ID
#define NO_BUTT_PCBA_ID 					(GUI_WINDOW_PCBA_PAGE_ID + 0x12)				//PCBA检验页面NO按钮控件ID
#define YES_BUTT_PCBA_ID 					(GUI_WINDOW_PCBA_PAGE_ID + 0x13)				//PCBA检验页面YES按钮控件ID



/*全局变量定义区**********************************************************/
osEventFlagsId_t PCBAEventGroupHandle = NULL;							//PCBA检验需要用到的事件组






/*函数区*****************************************************************/
/***
*主控板PCBA检验
***/
void PCBA_MainBoard(void)
{
#if PCBA_CHECK_MAIN_BOARD
	extern __IO WM_HWIN     g_hActiveWin;
	extern UART_HandleTypeDef huart4;
	extern UART_HandleTypeDef huart5;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern volatile CAT_ReceBuf_s CATReceBuf;
	extern osSemaphoreId_t SemphrBinPrinterRxHandle;
	extern PrinterReceBuf_s PrinterReceBuf;
	extern osEventFlagsId_t PCBAEventGroupHandle;
	extern  MachInfo_s MachInfo;
	extern __IO BC_Data_t   g_stBC_Data[BC_END];
	extern osSemaphoreId xSema_OutIn_Key;
	extern Usart1ReceBuf_s Usart1ReceBuf;
	
	extern osThreadId_t Usart1CMDHandle;
	extern osThreadId_t CATTaskHandle;
	extern osThreadId_t CATUpdateStatusTaskHandle;
	extern osThreadId_t PrintHandle;
	extern osThreadId_t PrintStatusHandle;
	extern osThreadId_t AlgoHandle;
	extern osThreadId_t PeriodScanHandle;
	
	WM_HWIN hItem;
	uint32_t TimeStampStart = 0;							//用来记录RTC计时是否准确
	RTC_HYM8563Info_s RTC_Hym8563Info = {0};
	char Str[100] = {0};
	char Str2[10] = {0};
	
	uint32_t Temp1 = 0;
	int32_t STemp1 = 0;
	int32_t STemp2 = 0;
	
	uint32_t i = 0;
	uint32_t j = 0;
	
	//用来表示检测是否通过
	uint8_t FailFlag = 0;
	uint8_t PressCheckFlag = 0;
	uint8_t MotoCheckFlag = 0;
	
	
	//挂起不相关任务
	osThreadSuspend(CATTaskHandle);
	osThreadSuspend(CATUpdateStatusTaskHandle);
	osThreadSuspend(PrintHandle);
	osThreadSuspend(PrintStatusHandle);
	osThreadSuspend(AlgoHandle);
	osThreadSuspend(Usart1CMDHandle);
	osThreadSuspend(PeriodScanHandle);
	
	//开启恒流源
	BC_CUR_SW_ON;
	
	//设置RTC起始时间
	RTC_Hym8563Info.year.bcd_h = 0;
	RTC_Hym8563Info.year.bcd_l = 0;
	RTC_Hym8563Info.month.bcd_h = 0;
	RTC_Hym8563Info.month.bcd_l = 1;
	RTC_Hym8563Info.day.bcd_h = 0;
	RTC_Hym8563Info.day.bcd_l = 1;
	RTC_Hym8563Info.hour.bcd_h = 0;
	RTC_Hym8563Info.hour.bcd_l = 0;
	RTC_Hym8563Info.min.bcd_h = 0;
	RTC_Hym8563Info.min.bcd_l = 0;
	RTC_Hym8563Info.sec.bcd_h = 0;
	RTC_Hym8563Info.sec.bcd_l = 0;
	
	RTC_HYM8563SetTime(&RTC_Hym8563Info);
	TimeStampStart = HAL_GetTick();
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	MULTIEDIT_SetText(hItem,"");
	MULTIEDIT_AddText(hItem,"主控板PCBA检验开始...\n");
	
	//IO输入输出检测
	MENU_GREEN_LED_ON;
	MENU_RED_LED_ON;
	CAT_PWR_ON;
	CAT_RESET_ON;
	COUNT_CELL_SW_ON;
	COUNT_CELL_TYPE_SW_ON;
	osDelay(100);
	
	if(OUTIN_KEY_STATUS != RESET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"面板红灯--进出仓按键：【失败】\n");
	}
	
	if(COUNT_KEY_STATUS != RESET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"面板绿灯--计数按键：【失败】\n");
	}
	
	if(ELEC_STATUS != SET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"CAT_PWR--定量电极：【失败】\n");
	}
	
	if(CAT_START_STATUS != RESET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"CAT_RST--CAT_STATUS：【失败】\n");
	}
	
	if(CHECK_JSC_EXIST_STATUS != JSC_EXIST_STATUS_ACTIVE){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"计数池上对射板输入输出：【失败】\n");
	}
	
	MENU_GREEN_LED_OFF;
	MENU_RED_LED_OFF;
	CAT_PWR_OFF;
	CAT_RESET_OFF;
	COUNT_CELL_SW_OFF;
	osDelay(100);
	
	if(OUTIN_KEY_STATUS != SET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"面板红灯--进出仓按键：【失败】\n");
	}
	
	if(COUNT_KEY_STATUS != SET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"面板绿灯--计数按键：【失败】\n");
	}
	
	if(ELEC_STATUS != RESET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"CAT_PWR--定量电极：【失败】\n");
	}
	
	if(CAT_START_STATUS != SET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"CAT_RST--CAT_STATUS：【失败】\n");
	}
	
	if(CHECK_JSC_EXIST_STATUS == JSC_EXIST_STATUS_ACTIVE){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"计数池上对射板输入输出：【失败】\n");
	}
	
	osThreadResume(PeriodScanHandle);
	
	if(FailFlag == 0){
		MULTIEDIT_AddText(hItem,"IO输入输出检测：【成功】\n");
	}
	
	//判断恒流源是否正常
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG);
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_YES_BUTTON_FLAG);
	MULTIEDIT_AddText(hItem,">>>请确认恒流源是否正常\n");
	if(osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG|PCBA_EVENT_YES_BUTTON_FLAG,0,osWaitForever) & PCBA_EVENT_NO_BUTTON_FLAG){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"恒流源检测：【失败】\n");
	}else{
		MULTIEDIT_AddText(hItem,"恒流源检测：【成功】\n");
	}
	
	//判断是否已经拔掉探针线
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG);
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_YES_BUTTON_FLAG);
	MULTIEDIT_AddText(hItem,">>>请确认是否已经拔出探针连接线\n");
	osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG|PCBA_EVENT_YES_BUTTON_FLAG,0,osWaitForever);
	
	//检验SPI FLASH
	//更改SPI模式
	SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
	memset(Str,0,100);
	strcpy(Str,"PCBA_MAIN_CHECK");
	if(SPIFlash_WriteData(30*1024*1024,(uint8_t*)Str,100) != SUCCESS){
		SPI4_ExitModeToDefault();
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"SPI FLASH写入：【失败】\n");
	}else{
		memset(Str,0,100);
		if(SPIFlash_ReadData(30*1024*1024,(uint8_t*)Str,100) != SUCCESS){
			SPI4_ExitModeToDefault();
			FailFlag = 1;
			MULTIEDIT_AddText(hItem,"SPI FLASH读取：【失败】\n");
		}else{
			SPI4_ExitModeToDefault();
			
			if(strstr(Str,"PCBA_MAIN_CHECK") == NULL){
				FailFlag = 1;
				MULTIEDIT_AddText(hItem,"SPI FLASH检测：【失败】\n");
			}else{
				MULTIEDIT_AddText(hItem,"SPI FLASH检测：【成功】\n");
			}
		}
	}
	
	MULTIEDIT_AddText(hItem,"NAND FLASH检测：【成功】\n");
	MULTIEDIT_AddText(hItem,"SDRAM检测：【成功】\n");

	//检验EEPROM
	MULTIEDIT_AddText(hItem,">>>请输入硬件版本号，并点确认\n");

PCBA_MAIN_CHECK_EEPROM_TAG:
	//先清除标志位
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG);
	
	//等待点击确认按钮
	osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG,0,osWaitForever);
	
	//判断录入的硬件版本号格式是否正确
	memset(Str,0,6);
	hItem = WM_GetDialogItem(g_hActiveWin, EDIT_PCBA_ID);
	EDIT_GetText(hItem,Str,5);
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	if(strlen(Str) < 4){
		MULTIEDIT_AddText(hItem,">>>输入硬件版本号错误，重新输入\n");
		goto PCBA_MAIN_CHECK_EEPROM_TAG;
	}
	
	if(Str[0] < 'A' || Str[0] > 'Z'){
		MULTIEDIT_AddText(hItem,">>>输入硬件版本号错误，重新输入\n");
		goto PCBA_MAIN_CHECK_EEPROM_TAG;
	}
	
	if(Str[1] != '.'){
		MULTIEDIT_AddText(hItem,">>>输入硬件版本号错误，重新输入\n");
		goto PCBA_MAIN_CHECK_EEPROM_TAG;
	}
	
	if(Str[2] < '0' || Str[2] > '9'){
		MULTIEDIT_AddText(hItem,">>>输入硬件版本号错误，重新输入\n");
		goto PCBA_MAIN_CHECK_EEPROM_TAG;
	}
	
	if(Str[3] < '0' || Str[3] > '9'){
		MULTIEDIT_AddText(hItem,">>>输入硬件版本号错误，重新输入\n");
		goto PCBA_MAIN_CHECK_EEPROM_TAG;
	}
	
	//硬件版本号检验正确，准备写入EEPROM
	if(EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_MAIN_BORD_HW_VERSION,(uint8_t*)Str,5) != HAL_OK){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"主控板EEPROM检测：【失败】\n");
	}else{
		memset(Str2,0,10);
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_MAIN_BORD_HW_VERSION,(uint8_t*)Str2,5);
		if(strcmp(Str2,Str) != 0){
			FailFlag = 1;
			MULTIEDIT_AddText(hItem,"主控板EEPROM检测：【失败】\n");
		}else{
			MULTIEDIT_AddText(hItem,"主控板EEPROM检测：【成功】\n");
		}
	}
	
	//检验是否可以读取转接板上的EEPROM
	memset(Str,0,6);
	EEPROMReadData(EEPROM_24C04,CONN_BORD_EEPROM_24C04,EEPROM_ADR_ZJ_BORD_HW_VERSION,(uint8_t*)Str,5);
	if(strstr(Str,"A.00") == NULL){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"读取转接板EEPROM：【失败】\n");
	}else{
		MULTIEDIT_AddText(hItem,"读取转接板EEPROM：【成功】\n");
	}
	
	//判断蜂鸣器
	Beep_Start();
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG);
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_YES_BUTTON_FLAG);
	MULTIEDIT_AddText(hItem,">>>蜂鸣器是否有效\n");
	if(osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG|PCBA_EVENT_YES_BUTTON_FLAG,0,osWaitForever) & PCBA_EVENT_NO_BUTTON_FLAG){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"蜂鸣器检测：【失败】\n");
	}else{
		MULTIEDIT_AddText(hItem,"蜂鸣器检测：【成功】\n");
	}
	Beep_Stop();
	
	//建压-30Kpa
	MachInfo.paramSet.ucPump_Scale = 100;
	MachInfo.paramSet.fPress_Adjust = 0;
	
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	AIR_WAVE_CLOSE;
	
	if(abs(Press_Value()) > 5000){
		FailFlag = 1;
		PressCheckFlag = 1;
		MULTIEDIT_AddText(hItem,"0Kpa,压力组件检测：【失败】\n");
	}
	
	//建压-30Kpa
	if(PressCheckFlag==0){
		if(Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE) != ERROR_CODE_SUCCESS){
			FailFlag = 1;
			PressCheckFlag = 1;
			MULTIEDIT_AddText(hItem,"-30Kpa,压力组件检测：【失败】\n");
		}
	}
	
	osDelay(1000);
	
	if(PressCheckFlag==0){
		if(Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE) != ERROR_CODE_SUCCESS){
			FailFlag = 1;
			PressCheckFlag = 1;
			MULTIEDIT_AddText(hItem,"-30Kpa,压力组件检测：【失败】\n");
		}
	}
	
	osDelay(1000);
	
	if(PressCheckFlag==0){
		if(Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE) != ERROR_CODE_SUCCESS){
			FailFlag = 1;
			PressCheckFlag = 1;
			MULTIEDIT_AddText(hItem,"-30Kpa,压力组件检测：【失败】\n");
		}
	}
	
	osDelay(2000);
	
	if(PressCheckFlag==0){
		osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG);
		osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_YES_BUTTON_FLAG);
		MULTIEDIT_AddText(hItem,">>>是否能建压到-30Kpa?\n");
		if(osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG|PCBA_EVENT_YES_BUTTON_FLAG,0,osWaitForever) & PCBA_EVENT_NO_BUTTON_FLAG){
			FailFlag = 1;
			PressCheckFlag = 1;
			MULTIEDIT_AddText(hItem,"压力组件检测：【失败】\n");
		}
	}
	
	if(PressCheckFlag==0){
		if(abs(Press_Value()) < 20000){
			FailFlag = 1;
			PressCheckFlag = 1;
			MULTIEDIT_AddText(hItem,"保压2s压力组件检测：【失败】\n");
		}
	}
	
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	if(PressCheckFlag==0){
		if(abs(Press_Value()) > 5000){
			FailFlag = 1;
			PressCheckFlag = 1;
			MULTIEDIT_AddText(hItem,"0Kpa,压力组件检测：【失败】\n");
		}
	}
	
	if(PressCheckFlag == 0){
		MULTIEDIT_AddText(hItem,"压力组件检测：【成功】\n");
	}
	

	
	
	//判断电压
	STemp1 = Power_12VP_V();
	memset(Str,0,100);
	if(abs(STemp1 - 12000) > 500){
		FailFlag = 1;
		sprintf(Str,"P12V检测：%d：【失败】\n",STemp1);
		MULTIEDIT_AddText(hItem,Str);
	}else{
		MULTIEDIT_AddText(hItem,"P12V检测：【成功】\n");
	}
	
	STemp1 = Power_12VN_V();
	memset(Str,0,100);
	if(abs(STemp1 - (-12000)) > 500){
		FailFlag = 1;
		sprintf(Str,"N12V检测：%d：【失败】\n",STemp1);
		MULTIEDIT_AddText(hItem,Str);
	}else{
		MULTIEDIT_AddText(hItem,"N12V检测：【成功】\n");
	}
	
	STemp1 = BC_Current_V();
	memset(Str,0,100);
	if(abs(STemp1 - 55000) > 2000){
		FailFlag = 1;
		sprintf(Str,"55V检测：%d：【失败】\n",STemp1);
		MULTIEDIT_AddText(hItem,Str);
	}else{
		MULTIEDIT_AddText(hItem,"55V检测：【成功】\n");
	}
	
	STemp1 = XK_V();
	memset(Str,0,100);
	if(abs(STemp1 - 2500) > 100){
		FailFlag = 1;
		sprintf(Str,"小孔电压检测：%d：【失败】\n",STemp1);
		MULTIEDIT_AddText(hItem,Str);
	}else{
		MULTIEDIT_AddText(hItem,"小孔电压检测：【成功】\n");
	}
	
	//判断串口1--debug
	Usart1ReceBuf.readIndex = Usart1ReceBuf.writeIndex;
	printf("PCBA MAIN CHECK\r\n");
	osDelay(100);
	while(Usart1ReceBuf.readIndex != Usart1ReceBuf.writeIndex){
		if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] == 'P'){
			Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
			if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] == 'C'){
				Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
				if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] == 'B'){
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] == 'A'){
						MULTIEDIT_AddText(hItem,"调试串口检测：【成功】\n");
						break;
					}
				}
			}
		}else{
			Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
		}
	}
	if(Usart1ReceBuf.readIndex == Usart1ReceBuf.writeIndex){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"调试串口检测：【失败】\n");
	}
	
	//判断串口4--CAT
	memset(Str,0,100);
	strcpy(Str,"PCBA MAIN CHECK MAIN CHECK MAIN CHECK");
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)Str,strlen(Str),100);
	osKernelUnlock();
	osDelay(10);
	if(strstr((char*)(CATReceBuf.buf+CATReceBuf.readIndex),"MAIN CHECK") == NULL){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"CAT串口检测：【失败】\n");
	}else{
		MULTIEDIT_AddText(hItem,"CAT串口检测：【成功】\n");
	}
	
	//判断串口5--打印机
	memset(Str,0,100);
	strcpy(Str,"PCBAPCBAPCBA");
	PrinterReceBuf.readIndex = PrinterReceBuf.writeIndex;
	osKernelLock();
	HAL_UART_Transmit(&huart5,(uint8_t*)Str,strlen(Str),100);
	osKernelUnlock();
	osDelay(10);
	if(strstr((char*)(PrinterReceBuf.buf),"PCBA") == NULL){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"打印机串口检测：【失败】\n");
	}else{
		MULTIEDIT_AddText(hItem,"打印机串口检测：【成功】\n");
	}
	
	//检验进出仓组件
	//进仓
	if(OutIn_Module_In(MOTOR_MODE_NORMAL) != ERROR_CODE_SUCCESS){
		FailFlag = 1;
		MotoCheckFlag = 1;
		MULTIEDIT_AddText(hItem,"进出仓组件检测：【失败】\n");
	}
	if(MotoCheckFlag == 0){
		if(OUTIN_IN_OC_STATUS != SET || OUTIN_OUT_OC_STATUS != RESET || CLAMP_IN_OC_STATUS != RESET || CLAMP_OUT_OC_STATUS != SET){
			FailFlag = 1;
			MotoCheckFlag = 1;
			MULTIEDIT_AddText(hItem,"进出仓组件检测：【失败】\n");
		}
	}
	if(MotoCheckFlag == 0){
		if(OutIn_Module_Out(MOTOR_MODE_NORMAL) != ERROR_CODE_SUCCESS){
			FailFlag = 1;
			MotoCheckFlag = 1;
			MULTIEDIT_AddText(hItem,"进出仓组件检测：【失败】\n");
		}
	}
	if(MotoCheckFlag == 0){
		if(OUTIN_IN_OC_STATUS != RESET || OUTIN_OUT_OC_STATUS != SET || CLAMP_IN_OC_STATUS != SET || CLAMP_OUT_OC_STATUS != RESET){
			FailFlag = 1;
			MotoCheckFlag = 1;
			MULTIEDIT_AddText(hItem,"进出仓组件检测：【失败】\n");
		}
	}
	
	if(MotoCheckFlag == 0){
		MULTIEDIT_AddText(hItem,"进出仓组件检测：【成功】\n");
	}
	
	//红绿蓝白黑灰纯色显示
	WM_SetCallback(WM_HBKWIN,None_cbBkWin);
	WM_HideWindow(g_hActiveWin);
	osDelay(100);
	
	GUI_SetBkColor(GUI_RED);
    GUI_Clear();
	osDelay(1000);
	GUI_SetBkColor(GUI_GREEN);
    GUI_Clear();
	osDelay(1000);
	GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
	osDelay(1000);
	GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
	osDelay(1000);
	GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
	osDelay(1000);
	GUI_SetBkColor(GUI_GRAY);
    GUI_Clear();
	osDelay(1000);
	
	WM_SetCallback(WM_HBKWIN, Public_cbBkWin);
	WM_ShowWindow(g_hActiveWin);
	
	//HGB检验
	MachInfo.paramSet.current535 = 1200;
	MachInfo.paramSet.current415 = 1400;
	MachInfo.paramSet.ucHGB_Gain = 150;
	HGB_Gran_Set(HGB_DR_CHANNEL,MachInfo.paramSet.ucHGB_Gain);
	
	Light_Start(MachInfo.paramSet.current535);
	osDelay(3000);
	
	STemp1 = HGB_ADC();
	
	memset(Str,0,100);
	if(abs(STemp1-3000) > 1000){
		FailFlag = 1;
		sprintf(Str,"535波长HGB收发检测：%u：【失败】\n",STemp1);
		MULTIEDIT_AddText(hItem,Str);
	}else{
		MULTIEDIT_AddText(hItem,"535波长HGB收发检测：【成功】\n");
	}
	
	Light_Start(MachInfo.paramSet.current415);
	osDelay(2000);
	
	STemp1 = HGB_ADC();
	
	memset(Str,0,100);
	if(abs(STemp1-3000) > 1000){
		FailFlag = 1;
		sprintf(Str,"415波长HGB收发检测：%u：【失败】\n",STemp1);
		MULTIEDIT_AddText(hItem,Str);
	}else{
		MULTIEDIT_AddText(hItem,"415波长HGB收发检测：【成功】\n");
	}
	
	Light_Stop();
	
	MachInfo.paramSet.ucHGB_Gain = 20;
	HGB_Gran_Set(HGB_DR_CHANNEL,MachInfo.paramSet.ucHGB_Gain);
	osDelay(10);
	
	Light_Start(MachInfo.paramSet.current415);
	osDelay(3000);
	
	STemp2 = HGB_ADC();
	
	if(abs(STemp1-STemp2) < 500){
		FailFlag = 1;
		sprintf(Str,"修改HGB增益检测：%u：【失败】\n",STemp2);
		MULTIEDIT_AddText(hItem,Str);
	}else{
		MULTIEDIT_AddText(hItem,"修改HGB增益检测：【成功】\n");
	}
	
	Light_Stop();
	
	MachInfo.paramSet.ucHGB_Gain = 150;
	HGB_Gran_Set(HGB_DR_CHANNEL,MachInfo.paramSet.ucHGB_Gain);
	osDelay(10);
	
	MachInfo.paramSet.current415 = 500;
	
	Light_Start(MachInfo.paramSet.current415);
	osDelay(3000);
	
	STemp2 = HGB_ADC();
	
	if(abs(STemp1-STemp2) < 500){
		FailFlag = 1;
		sprintf(Str,"修改HGB驱动电流检测：%u：【失败】\n",STemp2);
		MULTIEDIT_AddText(hItem,Str);
	}else{
		MULTIEDIT_AddText(hItem,"修改HGB驱动电流检测：【成功】\n");
	}
	
	Light_Stop();
	
	//WBC噪声检测
	//开启WBC采集
	BC_Enable(BC_WBC);
	
	Temp1 = 0;
	for(i=0;j<=20;j++){
		osDelay(100);
		
		for(i=0;i<500;i++){
			if(Temp1 < g_stBC_Data[BC_WBC].ucBuf[i]){
				Temp1 = g_stBC_Data[BC_WBC].ucBuf[i];
			}
		}
	}
	
	//关闭WBC采集
	BC_Disable(BC_WBC);
	g_stBC_Data[BC_WBC].ucFlag = BC_ADC_PART_NONE;
	
	memset(Str,0,100);
	if(((Temp1*3300)>>12) > 200){
		FailFlag = 1;
		sprintf(Str,"WBC噪声检测：%d：【失败】\n",((Temp1*3300)>>12));
		MULTIEDIT_AddText(hItem,Str);
	}else{
		MULTIEDIT_AddText(hItem,"WBC噪声检测：【成功】\n");
	}
	
	//内部工装信号检测
{
	pulse_info_i_t *pulseInfo;  //脉冲信息buffer
    uint32 pulseNum = 0;        //本次测试脉冲个数
	uint16_t Count = 0;
	double64 smpRate = BC_SAMPLE_RATE;   //数据采样频率，单位Hz
	pulse_mid_feature midFeature = {CS_IDLE, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0, 0,	0, smpRate, 0};
	
	//开辟算法内存空间
	pulseInfo = (pulse_info_i_t*)SDRAM_ALGO_ADDR;
    memset((void*)pulseInfo, 0, SDRAM_ALGO_LEN);	
		
	//发送内部工装触发信号
	COUNT_SIGNAL_SW_ON;
	Count_Signal_SetFreq(0.5);
	Count_Signal_Start();
	
	osDelay(500);
		
	//关闭调度器
	osKernelLock();
	
	//采集数据
	for(Count=0;Count<750;Count++){
		//使能血细胞信号采集
		BC_Enable(BC_WBC);
		
		//等待采集到1024字节数据
		while(1){
			if(BC_ADC_PART_1 == g_stBC_Data[BC_WBC].ucFlag){
				//关闭采集
				BC_Disable(BC_WBC);
				break;
			}
		}
		
		//复位标志
		g_stBC_Data[BC_WBC].ucFlag = BC_ADC_PART_NONE;
		
		//计算脉冲个数
		wbcPulseIdentify(pulseInfo, &pulseNum, (const uint8 *)&g_stBC_Data[BC_WBC].ucBuf[0], DATA_UINT_SIZE, &midFeature);
	}
	
	//恢复调度器
	osKernelUnlock();

	//关闭板载工装信号
	COUNT_SIGNAL_SW_OFF;
	//关闭信号
	Count_Signal_Stop();
	
	if(pulseNum < 0x0060 || pulseNum > 0x00FF){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"内部工装检测：【失败】\n");
	}else{
		MULTIEDIT_AddText(hItem,"内部工装检测：【成功】\n");
	}
}
	
	//验证RTC
	memset((uint8_t*)&RTC_Hym8563Info,0,sizeof(RTC_HYM8563Info_s));
	RTC_HYM8563GetTime(&RTC_Hym8563Info);
	TimeStampStart = HAL_GetTick() - TimeStampStart;
	if(RTC_Hym8563Info.day.bcd_h == 0 && RTC_Hym8563Info.day.bcd_l == 1){
		//判断RTC走时是否准确
		if(abs(((RTC_Hym8563Info.hour.bcd_h*10+RTC_Hym8563Info.hour.bcd_l)*3600 + (RTC_Hym8563Info.min.bcd_h*10+RTC_Hym8563Info.min.bcd_l)*60 + (RTC_Hym8563Info.sec.bcd_h*10+RTC_Hym8563Info.sec.bcd_l)) - (int)(TimeStampStart/1000)) > 2){
			FailFlag = 1;	
			MULTIEDIT_AddText(hItem,"RTC检测：【失败】\n");
		}else{
			MULTIEDIT_AddText(hItem,"RTC检测：【成功】\n");
		}
	}else{
		FailFlag = 1;	
		MULTIEDIT_AddText(hItem,"RTC检测：【失败】\n");
	}
	
	//清除EEPROM中的参数
	MachInfo.initFlag = 0;
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_INIT_FLAG,(uint8_t*)&MachInfo.initFlag,sizeof(MachInfo.initFlag));
	
	if(FailFlag == 1){
		MULTIEDIT_AddText(hItem,"主控板PCBA检验：【失败】\n");
	}else{
		MULTIEDIT_AddText(hItem,"主控板PCBA检验：【成功】\n");
	}
	
	//向界面发送重新执行消息
	WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
#endif
}



/***
*转接板PCBA检验
***/
void PCBA_ZJBoard(void)
{
#if PCBA_CHECK_ZJ_BOARD
	extern __IO WM_HWIN     g_hActiveWin;
	
	extern UART_HandleTypeDef huart5;
	extern osSemaphoreId_t SemphrBinPrinterRxHandle;
	extern PrinterReceBuf_s PrinterReceBuf;
	extern osEventFlagsId_t PCBAEventGroupHandle;
	extern  MachInfo_s MachInfo;
	
	extern osThreadId_t Usart1CMDHandle;
	extern osThreadId_t CATTaskHandle;
	extern osThreadId_t CATUpdateStatusTaskHandle;
	extern osThreadId_t PrintHandle;
	extern osThreadId_t PrintStatusHandle;
	extern osThreadId_t AlgoHandle;
	extern osThreadId_t PeriodScanHandle;
	
	WM_HWIN hItem;
	char Str[100] = {0};
	char Str2[10] = {0};
	
	//用来表示检测是否通过
	uint8_t FailFlag = 0;
	
	//挂起不相关任务
	osThreadSuspend(CATTaskHandle);
	osThreadSuspend(CATUpdateStatusTaskHandle);
	osThreadSuspend(PrintHandle);
	osThreadSuspend(PrintStatusHandle);
	osThreadSuspend(AlgoHandle);
	osThreadSuspend(Usart1CMDHandle);
	osThreadSuspend(PeriodScanHandle);
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	MULTIEDIT_SetText(hItem,"");
	MULTIEDIT_AddText(hItem,"显示转接板PCBA检验开始...\n");
	
	//IO输入输出检测
	MENU_GREEN_LED_ON;
	MENU_RED_LED_ON;
	osDelay(100);
	
	if(OUTIN_KEY_STATUS != RESET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"面板红灯--进出仓按键：【失败】\n");
	}
	
	if(COUNT_KEY_STATUS != RESET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"面板绿灯--计数按键：【失败】\n");
	}
	
	MENU_GREEN_LED_OFF;
	MENU_RED_LED_OFF;
	osDelay(100);
	
	if(OUTIN_KEY_STATUS != SET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"面板红灯--进出仓按键：【失败】\n");
	}
	
	if(COUNT_KEY_STATUS != SET){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"面板绿灯--计数按键：【失败】\n");
	}
	
	osThreadResume(PeriodScanHandle);
	
	if(FailFlag == 0){
		MULTIEDIT_AddText(hItem,"IO输入输出检测：【成功】\n");
	}
	
	//避免界面公共部分刷新
	WM_SetCallback(WM_HBKWIN,None_cbBkWin);
	WM_HideWindow(g_hActiveWin);
	osDelay(100);
		
	//显示触摸校验画板
	GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(GUI_BLACK);
    GUI_Clear();
	
	GUI_TouchCalibrate(0,0);

	//红绿蓝白黑灰纯色显示
	GUI_SetBkColor(GUI_RED);
    GUI_Clear();
	osDelay(1000);
	GUI_SetBkColor(GUI_GREEN);
    GUI_Clear();
	osDelay(1000);
	GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
	osDelay(1000);
	GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
	osDelay(1000);
	GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
	osDelay(1000);
	GUI_SetBkColor(GUI_GRAY);
    GUI_Clear();
	osDelay(1000);
	
	WM_SetCallback(WM_HBKWIN, Public_cbBkWin);
	WM_ShowWindow(g_hActiveWin);
	
	//判断串口5--打印机
	memset(Str,0,100);
	strcpy(Str,"PCBAPCBAPCBA");
	PrinterReceBuf.readIndex = PrinterReceBuf.writeIndex;
	osKernelLock();
	HAL_UART_Transmit(&huart5,(uint8_t*)Str,strlen(Str),100);
	osKernelUnlock();
	osDelay(10);
	if(strstr((char*)(PrinterReceBuf.buf),"PCBA") == NULL){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"打印机串口检测：【失败】\n");
	}else{
		MULTIEDIT_AddText(hItem,"打印机串口检测：【成功】\n");
	}
	
	//检验EEPROM
	MULTIEDIT_AddText(hItem,">>>请输入硬件版本号，并点确认\n");

PCBA_ZJBoard_EEPROM_TAG:
	//先清除标志位
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG);
	
	//等待点击确认按钮
	osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG,0,osWaitForever);
	
	//判断录入的硬件版本号格式是否正确
	memset(Str,0,6);
	hItem = WM_GetDialogItem(g_hActiveWin, EDIT_PCBA_ID);
	EDIT_GetText(hItem,Str,5);
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	if(strlen(Str) < 4){
		MULTIEDIT_AddText(hItem,">>>输入硬件版本号错误，重新输入\n");
		goto PCBA_ZJBoard_EEPROM_TAG;
	}
	
	if(Str[0] < 'A' || Str[0] > 'Z'){
		MULTIEDIT_AddText(hItem,">>>输入硬件版本号错误，重新输入\n");
		goto PCBA_ZJBoard_EEPROM_TAG;
	}
	
	if(Str[1] != '.'){
		MULTIEDIT_AddText(hItem,">>>输入硬件版本号错误，重新输入\n");
		goto PCBA_ZJBoard_EEPROM_TAG;
	}
	
	if(Str[2] < '0' || Str[2] > '9'){
		MULTIEDIT_AddText(hItem,">>>输入硬件版本号错误，重新输入\n");
		goto PCBA_ZJBoard_EEPROM_TAG;
	}
	
	if(Str[3] < '0' || Str[3] > '9'){
		MULTIEDIT_AddText(hItem,">>>输入硬件版本号错误，重新输入\n");
		goto PCBA_ZJBoard_EEPROM_TAG;
	}
	
	//硬件版本号检验正确，准备写入EEPROM
	if(EEPROMWriteData(EEPROM_24C04,CONN_BORD_EEPROM_24C04,EEPROM_ADR_ZJ_BORD_HW_VERSION,(uint8_t*)Str,5) != HAL_OK){
		FailFlag = 1;
		MULTIEDIT_AddText(hItem,"显示转接板EEPROM检测：【失败】\n");
	}else{
		memset(Str2,0,10);
		
		//读取出版本号，并检验是否正确
		EEPROMReadData(EEPROM_24C04,CONN_BORD_EEPROM_24C04,EEPROM_ADR_ZJ_BORD_HW_VERSION,(uint8_t*)Str2,5);
		
		if(strcmp(Str,Str2) != 0){
			FailFlag = 1;
			MULTIEDIT_AddText(hItem,"显示转接板EEPROM检测：【失败】\n");
		}else{
			MULTIEDIT_AddText(hItem,"显示转接板EEPROM检测：【成功】\n");
		}
	}
	
	if(FailFlag == 1){
		MULTIEDIT_AddText(hItem,"显示转接板PCBA检验：【失败】\n");
	}else{
		MULTIEDIT_AddText(hItem,"显示转接板PCBA检验：【成功】\n");
	}
	
	//向界面发送重新执行消息
	WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
#endif
}




/***
*CAT模块PCBA检验
***/
void PCBA_CAT(void)
{
#if PCBA_CHECK_CAT
	extern __IO WM_HWIN     g_hActiveWin;
	extern  MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osThreadId_t CATUpdateStatusTaskHandle;
	extern osThreadId_t CATTaskHandle;
	extern const osThreadAttr_t CATUpdateStatus_attributes;
	extern const osThreadAttr_t CATTask_attributes;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	
	WM_HWIN hItem;
	char Str[100] = {0};
	uint32_t Temp1 = 0;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	MULTIEDIT_SetText(hItem,"");
	MULTIEDIT_AddText(hItem,"CAT1模块PCBA检验开始...\n");
	
	//重置CAT相关的全局变量
	memset(&CATGlobalStatus,0,sizeof(CATGlobalStatus_s));
	//将版本号临时提升到最大，防止程序被网络自动升级
	CATGlobalStatus.localVersion.mainVersion = 999;
	CATGlobalStatus.localVersion.bootloadVersion = 999;
	CATGlobalStatus.localVersion.uiVersion = 999;
	
	//将CAT相关的任务删除，然后再重新建立
	osThreadTerminate(CATUpdateStatusTaskHandle);
	osThreadTerminate(CATTaskHandle);
	osDelay(50);
	osSemaphoreRelease(CATSemphrCountTxHandle);
	CATUpdateStatusTaskHandle = osThreadNew(CATUpdateStatusTask, NULL, &CATUpdateStatus_attributes);
	CATTaskHandle = osThreadNew(CATTask, NULL, &CATTask_attributes);
	
	Temp1 = 0;
	while(CATGlobalStatus.flag.powOn == 0){
		Temp1++;
		if(Temp1>300){
			MULTIEDIT_AddText(hItem,"CAT1模块上电：失败\n");
			MULTIEDIT_AddText(hItem,"CAT1模块检测结果：【失败】\n");
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
		osDelay(100);
	}
	MULTIEDIT_AddText(hItem,"CAT1模块上电【成功】\n");
	
	Temp1 = 0;
	while(CATGlobalStatus.flag.conn == 0){
		Temp1++;
		if(Temp1>300){
			MULTIEDIT_AddText(hItem,"CAT1模块与MCU通讯：失败\n");
			MULTIEDIT_AddText(hItem,"CAT1模块检测结果：【失败】\n");
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
		osDelay(100);
	}
	MULTIEDIT_AddText(hItem,"CAT1模块与MCU通讯【成功】\n");
	
	Temp1 = 0;
	while(CATGlobalStatus.flag.sim == 0){
		Temp1++;
		if(Temp1>300){
			MULTIEDIT_AddText(hItem,"CAT1模块读卡：失败\n");
			MULTIEDIT_AddText(hItem,"CAT1模块检测结果：【失败】\n");
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
		osDelay(100);
	}
	MULTIEDIT_AddText(hItem,"CAT1模块读卡【成功】\n");
	
	Temp1 = 0;
	while(CATGlobalStatus.signalStrength < 20){
		Temp1++;
		if(Temp1>300){
			MULTIEDIT_AddText(hItem,"CAT1模块信号：弱\n");
			MULTIEDIT_AddText(hItem,"CAT1模块检测结果：【失败】\n");
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
		osDelay(100);
	}
	memset(Str,0,100);
	sprintf(Str,"信号强度:%u\n",CATGlobalStatus.signalStrength);
	MULTIEDIT_AddText(hItem,Str);
	
	Temp1 = 0;
	while(CATGlobalStatus.registerNet == CAT_REGISTNET_UNKNOW){
		Temp1++;
		if(Temp1>200){
			MULTIEDIT_AddText(hItem,"CAT1模块网络未知\n");
			MULTIEDIT_AddText(hItem,"CAT1模块检测结果：【失败】\n");
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
		osDelay(100);
	}
	Temp1 = 0;
	while(CATGlobalStatus.registerNet == CAT_REGISTNET_CHECKING){
		Temp1++;
		if(Temp1>300){
			MULTIEDIT_AddText(hItem,"CAT1模块搜网超时\n");
			MULTIEDIT_AddText(hItem,"CAT1模块检测结果：【失败】\n");
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
		osDelay(100);
	}
	Temp1 = 0;
	while(CATGlobalStatus.registerNet == CAT_REGISTNET_SIM_OVERDUE){
		Temp1++;
		if(Temp1>300){
			MULTIEDIT_AddText(hItem,"SIM卡过期\n");
			MULTIEDIT_AddText(hItem,"CAT1模块检测结果：【失败】\n");
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
		osDelay(100);
	}
	
	MULTIEDIT_AddText(hItem,"CAT1模块注网【成功】\n");
	
	if(CATGlobalStatus.operatorNum == 0){
		MULTIEDIT_AddText(hItem,"当前使用的是电信卡\n");
	}else{
		MULTIEDIT_AddText(hItem,"当前使用的是移动卡\n");
	}
	
	memset(Str,0,100);
	sprintf(Str,"IMEI:%s\n",CATGlobalStatus.imei);
	MULTIEDIT_AddText(hItem,Str);
	
	memset(Str,0,100);
	sprintf(Str,"ICCID:%s\n",CATGlobalStatus.iccid);
	MULTIEDIT_AddText(hItem,Str);
	
	memset(Str,0,100);
	sprintf(Str,"位置信息:%s\n",CATGlobalStatus.position);
	MULTIEDIT_AddText(hItem,Str);
	
	Temp1 = 0;
	while(CATGlobalStatus.serviceVersion.PMainVersion == 0){
		Temp1++;
		if(Temp1>300){
			MULTIEDIT_AddText(hItem,"CAT1模块连接服务器：失败\n");
			MULTIEDIT_AddText(hItem,"CAT1模块检测结果：【失败】\n");
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
		osDelay(100);
	}
	
	MULTIEDIT_AddText(hItem,"CAT1模块连接服务端【成功】\n");
	MULTIEDIT_AddText(hItem,"CAT1模块检验结果：【成功】\n");
	
	WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
#endif
}




/***
*触摸屏物料检测
***/
void PCBA_TouchScreen(void)
{
#if PCBA_CHECK_TOUCH_SCREEN
	extern __IO WM_HWIN     g_hActiveWin;
	extern osEventFlagsId_t PCBAEventGroupHandle;
	extern  MachInfo_s MachInfo;
	
	WM_HWIN hItem;
	
	//避免界面公共部分刷新
	WM_SetCallback(WM_HBKWIN,None_cbBkWin);
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	MULTIEDIT_SetText(hItem,"");
	MULTIEDIT_AddText(hItem,"触摸屏显示模组检验开始...\n");
	
	WM_HideWindow(g_hActiveWin);
	osDelay(100);
		
	//显示触摸校验画板
	GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(GUI_BLACK);
    GUI_Clear();
	
	GUI_TouchCalibrate(0,0);
	
	//红绿蓝白黑灰纯色显示
	GUI_SetBkColor(GUI_RED);
    GUI_Clear();
	osDelay(500);
	while(PenIRQ_GetState() != PENIRQ_ACTIVE);
	osDelay(100);
	while(PenIRQ_GetState() == PENIRQ_ACTIVE);
	
	GUI_SetBkColor(GUI_GREEN);
    GUI_Clear();
	while(PenIRQ_GetState() != PENIRQ_ACTIVE);
	osDelay(100);
	while(PenIRQ_GetState() == PENIRQ_ACTIVE);
	GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
	while(PenIRQ_GetState() != PENIRQ_ACTIVE);
	osDelay(100);
	while(PenIRQ_GetState() == PENIRQ_ACTIVE);
	GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
	while(PenIRQ_GetState() != PENIRQ_ACTIVE);
	osDelay(100);
	while(PenIRQ_GetState() == PENIRQ_ACTIVE);
	GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
	while(PenIRQ_GetState() != PENIRQ_ACTIVE);
	osDelay(100);
	while(PenIRQ_GetState() == PENIRQ_ACTIVE);
	GUI_SetBkColor(GUI_GRAY);
    GUI_Clear();
	while(PenIRQ_GetState() != PENIRQ_ACTIVE);
	osDelay(100);
	while(PenIRQ_GetState() == PENIRQ_ACTIVE);
	
	//显示边框
	GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
	GUI_SetColor(GUI_RED);
	GUI_SetPenSize(5);
	GUI_DrawLine(0,0,799,0);
	GUI_DrawLine(799,0,799,479);
	GUI_DrawLine(799,479,0,479);
	GUI_DrawLine(0,479,0,0);
	
	while(PenIRQ_GetState() != PENIRQ_ACTIVE);
	osDelay(100);
	while(PenIRQ_GetState() == PENIRQ_ACTIVE);
	
	//界面公共部分刷新
	WM_SetCallback(WM_HBKWIN, Public_cbBkWin);
	WM_ShowWindow(g_hActiveWin);
	
	MULTIEDIT_AddText(hItem,"触摸屏显示模组检验完成\n");
	
	//向界面发送重新执行消息
	WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
#endif
}



/***
*PCBA压力校准
***/
void PCBA_PressureAdjust(void)
{
	extern __IO WM_HWIN     g_hActiveWin;
	extern  MachInfo_s MachInfo;
	WM_HWIN hItem;

#if PCBA_CHECK_PRESSURE_MONITOR
	char Str[100] = {0};
	uint8_t Len = 0;
	uint32_t Temp1 = 0;
	uint32_t Temp2 = 0;
	uint32_t Temp3 = 0;
	int32_t STemp1 = 0;
	int32_t STemp2 = 0;
	uint32_t i = 0;
	uint32_t j = 0;
	int32_t DigPressure[6];
	int32_t CurrPressure[6];
	uint32_t Time = 0;
	uint32_t EventBit = 0;
	
	hItem = WM_GetDialogItem(g_hActiveWin, EDIT_PCBA_ID);
	WM_EnableWindow(hItem);
	hItem = WM_GetDialogItem(g_hActiveWin, CONFIM_BUTT_PCBA_ID);
	WM_EnableWindow(hItem);
	hItem = WM_GetDialogItem(g_hActiveWin, NO_BUTT_PCBA_ID);
	WM_EnableWindow(hItem);
	hItem = WM_GetDialogItem(g_hActiveWin, YES_BUTT_PCBA_ID);
	WM_EnableWindow(hItem);
	
	MachInfo.paramSet.ucPump_Scale = 100;
	
	
PCBA_PressureAdjustTag:

	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	MULTIEDIT_SetText(hItem,"");
	MULTIEDIT_AddText(hItem,"工装压力校准开始...\n");
	
	Pump_Stop();
	LIQUID_WAVE_CLOSE;
	AIR_WAVE_CLOSE;
	
	//开阀泄压
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	memset((uint8_t*)&MachInfo.paramSet.digSensor[0],0,sizeof(MachInfo.paramSet.digSensor));
	
	//先清除标志位
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG);
	MULTIEDIT_AddText(hItem,"[0Kpa]请输入当前压力计压力值\n");
	//等待点击确认按钮
	osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG,0,osWaitForever);
	
	memset(Str,0,20);
	hItem = WM_GetDialogItem(g_hActiveWin, EDIT_PCBA_ID);
	EDIT_GetText(hItem,Str,10);
	CurrPressure[0] = atoi(Str);
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	
	//读取0kpa时的压力值
	DigPressureStart(AVG4);
	osDelay(30);
	DigPressure_GetRawValue(&Temp1);
	DigPressure[0] = DigPressure_ConvertValue(Temp1);
	
	//建压到-10kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		
		if(STemp2 <= -10000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-10Kpa超时\n");
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			AIR_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	//先清除标志位
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG);
	MULTIEDIT_AddText(hItem,"[-10Kpa]请输入当前压力计压力值\n");
	//等待点击确认按钮
	osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG,0,osWaitForever);
	
	memset(Str,0,20);
	hItem = WM_GetDialogItem(g_hActiveWin, EDIT_PCBA_ID);
	EDIT_GetText(hItem,Str,10);
	CurrPressure[1] = atoi(Str);
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	
	//读取-10kpa时的压力值
	DigPressureStart(AVG4);
	osDelay(30);
	DigPressure_GetRawValue(&Temp1);
	DigPressure[1] = DigPressure_ConvertValue(Temp1);
	
	//建压到-20kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		
		if(STemp2 <= -20000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-20Kpa超时\n");
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			AIR_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	//先清除标志位
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG);
	MULTIEDIT_AddText(hItem,"[-20Kpa]请输入当前压力计压力值\n");
	//等待点击确认按钮
	osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG,0,osWaitForever);
	
	memset(Str,0,20);
	hItem = WM_GetDialogItem(g_hActiveWin, EDIT_PCBA_ID);
	EDIT_GetText(hItem,Str,10);
	CurrPressure[2] = atoi(Str);
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	
	//读取-20kpa时的压力值
	DigPressureStart(AVG4);
	osDelay(30);
	DigPressure_GetRawValue(&Temp1);
	DigPressure[2] = DigPressure_ConvertValue(Temp1);
	
	//建压到-30kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		
		if(STemp2 <= -30000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-30Kpa超时\n");
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			AIR_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	//先清除标志位
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG);
	MULTIEDIT_AddText(hItem,"[-30Kpa]请输入当前压力计压力值\n");
	//等待点击确认按钮
	osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG,0,osWaitForever);
	
	memset(Str,0,20);
	hItem = WM_GetDialogItem(g_hActiveWin, EDIT_PCBA_ID);
	EDIT_GetText(hItem,Str,10);
	CurrPressure[3] = atoi(Str);
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	
	//读取-30kpa时的压力值
	DigPressureStart(AVG4);
	osDelay(30);
	DigPressure_GetRawValue(&Temp1);
	DigPressure[3] = DigPressure_ConvertValue(Temp1);
	
	//建压到-40kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		
		if(STemp2 <= -40000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-40Kpa超时\n");
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			AIR_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	//先清除标志位
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG);
	MULTIEDIT_AddText(hItem,"[-40Kpa]请输入当前压力计压力值\n");
	//等待点击确认按钮
	osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG,0,osWaitForever);
	
	memset(Str,0,20);
	hItem = WM_GetDialogItem(g_hActiveWin, EDIT_PCBA_ID);
	EDIT_GetText(hItem,Str,10);
	CurrPressure[4] = atoi(Str);
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	
	//读取-40kpa时的压力值
	DigPressureStart(AVG4);
	osDelay(30);
	DigPressure_GetRawValue(&Temp1);
	DigPressure[4] = DigPressure_ConvertValue(Temp1);
	
	//===计算补偿值===
	for(i=0;i<5;i++){
		MachInfo.paramSet.digSensor[i] = DigPressure[i] - CurrPressure[i];
	}
	
	MULTIEDIT_AddText(hItem,"核对补偿后的压力值开始...\n");
	
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	osDelay(500);
	
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG);
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_YES_BUTTON_FLAG);
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		Len = sprintf(Str,"当前补偿后的压力值为：%d Pa\n",DigPressure_ConvertValue(Temp1));
		Str[Len] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		EventBit = osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG|PCBA_EVENT_YES_BUTTON_FLAG,0,500);
		if(EventBit==osErrorTimeout || EventBit==osErrorResource || EventBit==osErrorParameter){
			continue;
		}else if(EventBit & PCBA_EVENT_NO_BUTTON_FLAG){
			MULTIEDIT_AddText(hItem,"校验失败，即将重新校验...\n");
			osDelay(1000);
			goto PCBA_PressureAdjustTag;
		}else if(EventBit & PCBA_EVENT_YES_BUTTON_FLAG){
			break;
		}
	}
		
	
	//建压到-10kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		
		if(STemp2 <= -10000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-10Kpa超时\n");
			AIR_WAVE_CLOSE;
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG);
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_YES_BUTTON_FLAG);
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		Len = sprintf(Str,"当前补偿后的压力值为：%d Pa\n",DigPressure_ConvertValue(Temp1));
		Str[Len] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		EventBit = osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG|PCBA_EVENT_YES_BUTTON_FLAG,0,500);
		if(EventBit==osErrorTimeout || EventBit==osErrorResource || EventBit==osErrorParameter){
			continue;
		}else if(EventBit & PCBA_EVENT_NO_BUTTON_FLAG){
			MULTIEDIT_AddText(hItem,"校验失败，即将重新校验...\n");
			osDelay(1000);
			goto PCBA_PressureAdjustTag;
		}else if(EventBit & PCBA_EVENT_YES_BUTTON_FLAG){
			break;
		}
	}
	
	//建压到-20kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		
		if(STemp2 <= -20000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-20Kpa超时\n");
			AIR_WAVE_CLOSE;
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG);
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_YES_BUTTON_FLAG);
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		Len = sprintf(Str,"当前补偿后的压力值为：%d Pa\n",DigPressure_ConvertValue(Temp1));
		Str[Len] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		EventBit = osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG|PCBA_EVENT_YES_BUTTON_FLAG,0,500);
		if(EventBit==osErrorTimeout || EventBit==osErrorResource || EventBit==osErrorParameter){
			continue;
		}else if(EventBit & PCBA_EVENT_NO_BUTTON_FLAG){
			MULTIEDIT_AddText(hItem,"校验失败，即将重新校验...\n");
			osDelay(1000);
			goto PCBA_PressureAdjustTag;
		}else if(EventBit & PCBA_EVENT_YES_BUTTON_FLAG){
			break;
		}
	}
	
	//建压到-30kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		
		if(STemp2 <= -30000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-30Kpa超时\n");
			AIR_WAVE_CLOSE;
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG);
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_YES_BUTTON_FLAG);
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		Len = sprintf(Str,"当前补偿后的压力值为：%d Pa\n",DigPressure_ConvertValue(Temp1));
		Str[Len] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		EventBit = osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG|PCBA_EVENT_YES_BUTTON_FLAG,0,500);
		if(EventBit==osErrorTimeout || EventBit==osErrorResource || EventBit==osErrorParameter){
			continue;
		}else if(EventBit & PCBA_EVENT_NO_BUTTON_FLAG){
			MULTIEDIT_AddText(hItem,"校验失败，即将重新校验...\n");
			osDelay(1000);
			goto PCBA_PressureAdjustTag;
		}else if(EventBit & PCBA_EVENT_YES_BUTTON_FLAG){
			break;
		}
	}
	
	//建压到-40kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		
		if(STemp2 <= -40000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-40Kpa超时\n");
			AIR_WAVE_CLOSE;
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG);
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_YES_BUTTON_FLAG);
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		Len = sprintf(Str,"当前补偿后的压力值为：%d Pa\n",DigPressure_ConvertValue(Temp1));
		Str[Len] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		EventBit = osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG|PCBA_EVENT_YES_BUTTON_FLAG,0,500);
		if(EventBit==osErrorTimeout || EventBit==osErrorResource || EventBit==osErrorParameter){
			continue;
		}else if(EventBit & PCBA_EVENT_NO_BUTTON_FLAG){
			MULTIEDIT_AddText(hItem,"校验失败，即将重新校验...\n");
			osDelay(1000);
			goto PCBA_PressureAdjustTag;
		}else if(EventBit & PCBA_EVENT_YES_BUTTON_FLAG){
			break;
		}
	}
	
	//建压到-50kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		
		if(STemp2 <= -50000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-50Kpa超时\n");
			AIR_WAVE_CLOSE;
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG);
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_YES_BUTTON_FLAG);
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		Len = sprintf(Str,"当前补偿后的压力值为：%d Pa\n",DigPressure_ConvertValue(Temp1));
		Str[Len] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		EventBit = osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG|PCBA_EVENT_YES_BUTTON_FLAG,0,500);
		if(EventBit==osErrorTimeout || EventBit==osErrorResource || EventBit==osErrorParameter){
			continue;
		}else if(EventBit & PCBA_EVENT_NO_BUTTON_FLAG){
			MULTIEDIT_AddText(hItem,"校验失败，即将重新校验...\n");
			osDelay(1000);
			goto PCBA_PressureAdjustTag;
		}else if(EventBit & PCBA_EVENT_YES_BUTTON_FLAG){
			break;
		}
	}
	
	//写入EEPOM
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.digSensor-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.digSensor,sizeof(MachInfo.paramSet.digSensor));
#elif PCBA_CHECK_VALVE_PUMP
	char Str[100] = {0};
	uint8_t Len = 0;
	uint32_t Temp1 = 0;
	uint32_t Temp2 = 0;
	uint32_t Temp3 = 0;
	int32_t STemp1 = 0;
	int32_t STemp2 = 0;
	uint32_t i = 0;
	uint32_t j = 0;
	int32_t DigPressure[6];
	int32_t CurrPressure[6];
	uint32_t Time = 0;
	uint32_t EventBit = 0;
	
	hItem = WM_GetDialogItem(g_hActiveWin, EDIT_PCBA_ID);
	WM_EnableWindow(hItem);
	hItem = WM_GetDialogItem(g_hActiveWin, CONFIM_BUTT_PCBA_ID);
	WM_EnableWindow(hItem);
	hItem = WM_GetDialogItem(g_hActiveWin, NO_BUTT_PCBA_ID);
	WM_EnableWindow(hItem);
	hItem = WM_GetDialogItem(g_hActiveWin, YES_BUTT_PCBA_ID);
	WM_EnableWindow(hItem);
	
	MachInfo.paramSet.ucPump_Scale = 100;
	
	
PCBA_PressureAdjustTag:

	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	MULTIEDIT_SetText(hItem,"");
	MULTIEDIT_AddText(hItem,"工装压力校准开始...\n");
	
	Pump_Stop();
	LIQUID_WAVE_CLOSE;
	AIR_WAVE_CLOSE;
	
	//开阀泄压
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;

	//清空补偿值
	MachInfo.paramSet.fPress_Adjust = 0;

	//建压-30Kpa
	if(Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE) != ERROR_CODE_SUCCESS){
		MULTIEDIT_AddText(hItem,"建压到-30Kpa失败\n");
		
		LIQUID_WAVE_OPEN;
		osDelay(1500);
		LIQUID_WAVE_CLOSE;
		
		//向界面发送重新执行消息
		WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
		return;
	}
	
	osDelay(500);
	
	//先清除标志位
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG);
	MULTIEDIT_AddText(hItem,"[-30Kpa]请输入当前压力计压力值\n");
	//等待点击确认按钮
	osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_CONFIM_BUTTON_FLAG,0,osWaitForever);
	
	memset(Str,0,20);
	hItem = WM_GetDialogItem(g_hActiveWin, EDIT_PCBA_ID);
	EDIT_GetText(hItem,Str,10);
	CurrPressure[0] = atoi(Str);
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	
	CurrPressure[1] = Press_Value();
	
	MachInfo.paramSet.fPress_Adjust = (CurrPressure[1] - CurrPressure[0])/1000.0;
	
	//===
	MULTIEDIT_AddText(hItem,"核对补偿后的压力值开始...\n");
	
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	//建压-30Kpa
	if(Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE) != ERROR_CODE_SUCCESS){
		MULTIEDIT_AddText(hItem,"建压到-30Kpa失败\n");
		
		LIQUID_WAVE_OPEN;
		osDelay(1500);
		LIQUID_WAVE_CLOSE;
		
		//向界面发送重新执行消息
		WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
		return;
	}
	
	osDelay(500);
	
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG);
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_YES_BUTTON_FLAG);
	while(1){
		Len = sprintf(Str,"当前补偿后的压力值为：%d Pa\n",Press_Value());
		Str[Len] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		EventBit = osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_NO_BUTTON_FLAG|PCBA_EVENT_YES_BUTTON_FLAG,0,500);
		if(EventBit==osErrorTimeout || EventBit==osErrorResource || EventBit==osErrorParameter){
			continue;
		}else if(EventBit & PCBA_EVENT_NO_BUTTON_FLAG){
			MULTIEDIT_AddText(hItem,"校验失败，即将重新校验...\n");
			osDelay(1000);
			goto PCBA_PressureAdjustTag;
		}else if(EventBit & PCBA_EVENT_YES_BUTTON_FLAG){
			break;
		}
	}
	
	//写入EEPOM
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.fPress_Adjust-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.fPress_Adjust,sizeof(MachInfo.paramSet.fPress_Adjust));
#endif
	
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	MULTIEDIT_SetText(hItem,"压力传感器校验完成\n");
	
	hItem = WM_GetDialogItem(g_hActiveWin, EDIT_PCBA_ID);
	WM_DisableWindow(hItem);
	hItem = WM_GetDialogItem(g_hActiveWin, CONFIM_BUTT_PCBA_ID);
	WM_DisableWindow(hItem);
	hItem = WM_GetDialogItem(g_hActiveWin, NO_BUTT_PCBA_ID);
	WM_DisableWindow(hItem);
	hItem = WM_GetDialogItem(g_hActiveWin, YES_BUTT_PCBA_ID);
	WM_DisableWindow(hItem);
	
	
	//向界面发送重新执行消息
	WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
	return;
}




/***
*压力传感器检验
***/
void PCBA_Pressure(uint8_t SensorType)
{
#if PCBA_CHECK_PRESSURE_MONITOR
	extern __IO WM_HWIN     g_hActiveWin;
	extern  MachInfo_s MachInfo;
	
	WM_HWIN hItem;
	char Str[100] = {0};
	
	uint32_t Temp1 = 0;
	
	int32_t STemp2 = 0;
	
	uint32_t i = 0;
	
	int32_t DigPressure[5];
	int32_t MoniPressure[5];
	int32_t DiffValue[5];
	
	uint32_t Time = 0;
	
	
	//执行不同的压力传感器检验
	MachInfo.paramSet.pressSensor = SensorType;
	
	MachInfo.paramSet.ucPump_Scale = 100;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	MULTIEDIT_SetText(hItem,"");
	MULTIEDIT_AddText(hItem,"模拟压力传感器检验开始...\n");
	
	Pump_Stop();
	LIQUID_WAVE_CLOSE;
	AIR_WAVE_CLOSE;
	
	//开阀泄压
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	//读取0kpa时的压力值
	DigPressureStart(AVG4);
	osDelay(30);
	DigPressure_GetRawValue(&Temp1);
	DigPressure[0] = DigPressure_ConvertValue(Temp1);
	
	MoniPressure[0] = Press_Value();
	DiffValue[0] = DigPressure[0]-MoniPressure[0];
	
	memset(Str,0,100);
	sprintf(Str,"0kpa时,数字:%d/模拟:%d/差值:%d\n",DigPressure[0],MoniPressure[0],DiffValue[0]);
	MULTIEDIT_AddText(hItem,Str);
	
	//建压到10kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		printf("Raw:%u,Value:%d\r\n",Temp1,STemp2);
		
		if(STemp2 <= -10000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-10Kpa超时\n");
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			AIR_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	//读取10kpa时的压力值
	DigPressureStart(AVG4);
	osDelay(30);
	DigPressure_GetRawValue(&Temp1);
	DigPressure[1] = DigPressure_ConvertValue(Temp1);
	
	MoniPressure[1] = Press_Value();
	DiffValue[1] = DigPressure[1]-MoniPressure[1];
	
	memset(Str,0,100);
	sprintf(Str,"10kpa时,数字:%d/模拟:%d/差值:%d\n",DigPressure[1],MoniPressure[1],DiffValue[1]);
	MULTIEDIT_AddText(hItem,Str);
	
	//建压到20kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		printf("Raw:%u,Value:%d\r\n",Temp1,STemp2);
		
		if(STemp2 <= -20000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-20Kpa超时\n");
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			AIR_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	//读取20kpa时的压力值
	DigPressureStart(AVG4);
	osDelay(30);
	DigPressure_GetRawValue(&Temp1);
	DigPressure[2] = DigPressure_ConvertValue(Temp1);
	
	MoniPressure[2] = Press_Value();
	DiffValue[2] = DigPressure[2]-MoniPressure[2];
	
	memset(Str,0,100);
	sprintf(Str,"20kpa时,数字:%d/模拟:%d/差值:%d\n",DigPressure[2],MoniPressure[2],DiffValue[2]);
	MULTIEDIT_AddText(hItem,Str);
	
	//建压到30kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		
		if(STemp2 <= -30000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-30Kpa超时\n");
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			AIR_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	//读取30kpa时的压力值
	DigPressureStart(AVG4);
	osDelay(30);
	DigPressure_GetRawValue(&Temp1);
	DigPressure[3] = DigPressure_ConvertValue(Temp1);
	
	MoniPressure[3] = Press_Value();
	DiffValue[3] = DigPressure[3]-MoniPressure[3];
	
	memset(Str,0,100);
	sprintf(Str,"30kpa时,数字:%d/模拟:%d/差值:%d\n",DigPressure[3],MoniPressure[3],DiffValue[3]);
	MULTIEDIT_AddText(hItem,Str);
	
	//建压到40kpa
	Pump_Start();
	AIR_WAVE_OPEN;
	
	Time = HAL_GetTick();
	
	while(1){
		DigPressureStart(AVG4);
		osDelay(30);
		DigPressure_GetRawValue(&Temp1);
		STemp2 = DigPressure_ConvertValue(Temp1);
		
		if(STemp2 <= -40000){
			AIR_WAVE_CLOSE;
			Pump_Stop();
			break;
		}
		
		if(HAL_GetTick() - Time > 10000){
			MULTIEDIT_AddText(hItem,"建压到-40Kpa超时\n");
			Pump_Stop();
			
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			AIR_WAVE_CLOSE;
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	osDelay(500);
	
	//读取40kpa时的压力值
	DigPressureStart(AVG4);
	osDelay(30);
	DigPressure_GetRawValue(&Temp1);
	DigPressure[4] = DigPressure_ConvertValue(Temp1);
	
	MoniPressure[4] = Press_Value();
	DiffValue[4] = DigPressure[4]-MoniPressure[4];
	
	memset(Str,0,100);
	sprintf(Str,"40kpa时,数字:%d/模拟:%d/差值:%d\n",DigPressure[4],MoniPressure[4],DiffValue[4]);
	MULTIEDIT_AddText(hItem,Str);
	
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	//判断差值是否相关
	for(i=0;i<5;i++){
		if(abs(DiffValue[i]) > 1500){
			MULTIEDIT_AddText(hItem,"压力传感器线性偏差过大\n");
			MULTIEDIT_AddText(hItem,"压力传感器检验结果：【失败】\n");
			
			//向界面发送重新执行消息
			WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
			return;
		}
	}
	
	MULTIEDIT_AddText(hItem,"压力传感器检验结果：【成功】\n");
	
	//向界面发送重新执行消息
	WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
	return;
#endif
}


/***
*阀检测
***/
void PCBA_Valve(void)
{
#if PCBA_CHECK_VALVE_PUMP
	extern __IO WM_HWIN     g_hActiveWin;
	extern osEventFlagsId_t PCBAEventGroupHandle;
	extern  MachInfo_s MachInfo;
	
	WM_HWIN hItem;
	int Pressure1 = 0;
	
	MachInfo.paramSet.ucPump_Scale = 100;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	MULTIEDIT_SetText(hItem,"");
	MULTIEDIT_AddText(hItem,"阀检验开始...\n");
	
	AIR_WAVE_CLOSE;
	LIQUID_WAVE_CLOSE;
	
	//建压-40Kpa
	if(Build_Press(RUN_MODE_NORMAL, AGING_TEST_PRESS) != ERROR_CODE_SUCCESS){
		MULTIEDIT_AddText(hItem,"建压到-40Kpa失败\n");
		
		LIQUID_WAVE_OPEN;
		osDelay(1500);
		LIQUID_WAVE_CLOSE;
		
		//向界面发送重新执行消息
		WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
		return;
	}
	
	MULTIEDIT_AddText(hItem,"阀保压性检测中，请等待...\n");
	
	osDelay(1500);
	Pressure1 = Press_Value();
	osDelay(20000);
	
	if(abs(Pressure1 - Press_Value()) > 3000 || Press_Value() > BC_TEST_PRESS_VALUE){
		MULTIEDIT_AddText(hItem,"20s内，阀泄漏量大于3Kpa\n");
		MULTIEDIT_AddText(hItem,"阀检验结果：【失败】\n");
		
		LIQUID_WAVE_OPEN;
		osDelay(1500);
		LIQUID_WAVE_CLOSE;
		
		//向界面发送重新执行消息
		WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
		return;
	}else{
		MULTIEDIT_AddText(hItem,"阀检验结果：【合格】\n");
	}
	
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	//向界面发送重新执行消息
	WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
#endif
}



/***
*泵检测
***/
void PCBA_Pump(void)
{
#if PCBA_CHECK_VALVE_PUMP
	extern __IO WM_HWIN     g_hActiveWin;
	extern osEventFlagsId_t PCBAEventGroupHandle;
	extern  MachInfo_s MachInfo;
	
	WM_HWIN hItem;
	char Str[100] = {0};
	uint8_t Len = 0;
	uint16_t PumpScale = 0;
	uint32_t Time = 0;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	MULTIEDIT_SetText(hItem,"");
	MULTIEDIT_AddText(hItem,"泵检验开始...\n");
	
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	AIR_WAVE_CLOSE;
	LIQUID_WAVE_CLOSE;
	
	//临时更改
	PumpScale = MachInfo.paramSet.ucPump_Scale;
	MachInfo.paramSet.ucPump_Scale = 60;
	Time = HAL_GetTick();
	
	//建压-30kPa
	if(Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE) != ERROR_CODE_SUCCESS){
		MULTIEDIT_AddText(hItem,"建压到-30Kpa失败\n");
		MULTIEDIT_AddText(hItem,"泵检测结果：【失败】\n");
		
		LIQUID_WAVE_OPEN;
		osDelay(1500);
		LIQUID_WAVE_CLOSE;
		MachInfo.paramSet.ucPump_Scale = PumpScale;
		
		//向界面发送重新执行消息
		WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
		
		return;
	}
	
	if((HAL_GetTick() - Time > 6000)){
		Len = sprintf(Str,"建压到-30Kpa时间不在6s范围内:%u ms\n",(HAL_GetTick() - Time));
		Str[Len] = '\0';
		MULTIEDIT_AddText(hItem,Str);
		MULTIEDIT_AddText(hItem,"泵检测结果：【失败】\n");
		
		LIQUID_WAVE_OPEN;
		osDelay(1500);
		LIQUID_WAVE_CLOSE;
		MachInfo.paramSet.ucPump_Scale = PumpScale;
		
		//向界面发送重新执行消息
		WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
		
		return;
	}
	
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	//建压-40kPa
	if(Build_Press(RUN_MODE_NORMAL, AGING_TEST_PRESS) != ERROR_CODE_SUCCESS){
		MULTIEDIT_AddText(hItem,"建压到-40Kpa失败\n");
		MULTIEDIT_AddText(hItem,"泵检测结果：【失败】\n");
		
		LIQUID_WAVE_OPEN;
		osDelay(1500);
		LIQUID_WAVE_CLOSE;
		MachInfo.paramSet.ucPump_Scale = PumpScale;
		
		//向界面发送重新执行消息
		WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
		
		return;
	}
	
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	MachInfo.paramSet.ucPump_Scale = PumpScale;
	
	MULTIEDIT_AddText(hItem,"泵检验结果：【合格】\n");
	
	//向界面发送重新执行消息
	WM_SendMessageNoPara(g_hActiveWin,WM_PCBA_TEST_BE_OVER);
#endif
}




/***
*电机检测
***/
void PCBA_Moto(void)
{
#if PCBA_CHECK_MOTO
	extern __IO WM_HWIN     g_hActiveWin;
	extern osEventFlagsId_t PCBAEventGroupHandle;
	extern  MachInfo_s MachInfo;
	
	WM_HWIN hItem;
	uint32_t EventBit = 0;
	uint16_t i = 0;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_PCBA_ID);
	MULTIEDIT_SetText(hItem,"");
	MULTIEDIT_AddText(hItem,"直线步进电机检验开始...\n");
	
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_MOTO_TUILI_BUTTON_FLAG);
	osEventFlagsClear(PCBAEventGroupHandle,PCBA_EVENT_MOTO_TIAODONG_BUTTON_FLAG);
	
	while(1){
		EventBit = osEventFlagsWait(PCBAEventGroupHandle,PCBA_EVENT_MOTO_TUILI_BUTTON_FLAG|PCBA_EVENT_MOTO_TIAODONG_BUTTON_FLAG,0,osWaitForever);
		
		MULTIEDIT_SetText(hItem,"");
		MULTIEDIT_AddText(hItem,"直线步进电机检验开始...\n");
		
		if(EventBit & PCBA_EVENT_MOTO_TUILI_BUTTON_FLAG){
			//先出仓到位
			OutIn_Motor_Out(MOTOR_MODE_NORMAL,MOTOR_OUTIN_MAX_STEP,0);
			
			for(i=0;i<2;i++){
				OutIn_Motor_In(MOTOR_MODE_NORMAL,MOTOR_OUTIN_MAX_STEP,0);
				OutIn_Motor_Out(MOTOR_MODE_NORMAL,MOTOR_OUTIN_MAX_STEP,0);
			}
			
			MULTIEDIT_AddText(hItem,"电机推力检测完毕\n");
		}else if(EventBit & PCBA_EVENT_MOTO_TIAODONG_BUTTON_FLAG){
			MV_InitPara(MOTOR_CLAMP, 1920, 2000, 100, 10);
			MV_Move(MOTOR_CLAMP, 10000, MOTOR_DIR_OUT); 
			MTx_IoMinitor_Disnable(MOTOR_CLAMP);
			
			while(TRUE != MV_IsFinished(MOTOR_CLAMP));
			MV_Stop(MOTOR_CLAMP);
			
			MULTIEDIT_AddText(hItem,"电机跳动检测完毕\n");
		}
		
		MULTIEDIT_AddText(hItem,"请按相关检验标准进行检验\n");
	}
#endif
}









//登录界面控件ID值
#define ID_LOGIN_PAGE_EDIT_PASSWORD           (GUI_LOGIN_ID + 0x02)
#define ID_LOGIN_PAGE_BUTTON_LOGIN            (GUI_LOGIN_ID + 0x03)


/***
*自动重复开关机任务
***/
void TempTask_AutoPowerOnoffCheck(void *argument)
{
#if AUTO_POWER_ONOFF_SELF_CHECK
	extern __IO WM_HWIN     g_hActiveWin;
	extern  MachInfo_s MachInfo;
	extern osSemaphoreId xSema_Count_Key;
	extern __IO MachRunPara_s MachRunPara;
	extern char gs_cPasswdBuffer[ACCOUNT_PSW_MAX_LEN];
	WM_HWIN hItem;
	GUI_PID_STATE TouchState;
	GUI_RECT Rect;
	
	
	//填入登录密码
	hItem = WM_GetDialogItem(g_hActiveWin, ID_LOGIN_PAGE_EDIT_PASSWORD);
	EDIT_SetText(hItem,"001");
	memset(gs_cPasswdBuffer,0,sizeof(gs_cPasswdBuffer));
	strcpy(gs_cPasswdBuffer,"001");
	
	osDelay(300);
	
	//模拟触发登录按钮点击事件
	hItem = WM_GetDialogItem(g_hActiveWin, ID_LOGIN_PAGE_BUTTON_LOGIN);
	WM_GetWindowRectEx(hItem,&Rect);
	TouchState.x = Rect.x0+50;
	TouchState.y = Rect.y0+20;
	TouchState.Pressed = 1;
	GUI_TOUCH_StoreStateEx(&TouchState);
	osDelay(100);
	TouchState.Pressed = 0;
	GUI_TOUCH_StoreStateEx(&TouchState);
	
	//等待自检完毕跳转到样本分析界面
	while(Get_Cur_MenuID() != GUI_WINDOW_ANALYSIS_ID){
		osDelay(1000);
	}
	
	//发送内部工装触发信号
	//接通板载工装信号
	COUNT_SIGNAL_SW_ON;
	//设置板载工装信号的*空比，启动信号
	Count_Signal_SetFreq(0.5);
	Count_Signal_Start();
	
	osDelay(500);
	
	Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO;
    osSemaphoreRelease(xSema_Count_Key);
	osDelay(500);
	Get_CountMode_Info()->ulCountMode = COUNT_MODE_INVALIDE;
	
	//关闭板载工装信号
	COUNT_SIGNAL_SW_OFF;
	//关闭信号
	Count_Signal_Stop();
	
	osDelay(10000);
	Get_CountMode_Info()->ulCountMode = COUNT_MODE_NORMAL;
	
	while(1){
		osDelay(10000);
	}
#endif
}












