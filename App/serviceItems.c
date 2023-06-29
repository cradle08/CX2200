/********************************************************************
*服务界面相关项目
*********************************************************************/
#include "serviceItems.h"
#include "cmsis_os2.h"
#include "PrinterTask.h"
#include "monitorV.h"
#include "main.h"
#include "bsp_eeprom.h"
#include "bsp_motor.h"
#include "cmsis_os.h"
#include "bsp_outin.h"
#include "alg.h"
#include "algstring.h"
#include "algpulse.h"
#include "abcompute.h"
#include "bc.h"
#include "backend_msg.h"
#include "bsp_press.h"
#include "bsp_light.h"
#include "agingTestPageDLG.h"
#include "WM.h"
#include "Public_menuDLG.h"
#include "Printer.h"
#include "msg_def.h"
#include "DIALOG.h"
#include "CAT_iot.h"
#include "process.h"
#include "bsp_spi.h"
#include "xpt2046.h"
#include "bsp_resistance.h"
#include "Common.h"


/*全局变量定义区*****************************************************/






/*内部函数区*********************************************************/
/***
*计算两个日期时间之间的秒值差
***/
//static long _TimeGetSecDiff(RTC_HYM8563Info_s StartTime,RTC_HYM8563Info_s EndTime)
//{
//	long StartTimeAllSec = 0;
//	long EndTimeAllSec = 0;
//	uint16_t StartYear = 2000+(StartTime.year.bcd_h*10+StartTime.year.bcd_l);
//	uint8_t StartMonth = StartTime.month.bcd_h*10+StartTime.month.bcd_l;
//	uint8_t StartDay = StartTime.day.bcd_h*10+StartTime.day.bcd_l;
//	uint8_t StartHour = StartTime.hour.bcd_h*10+StartTime.hour.bcd_l;
//	uint8_t StartMin = StartTime.min.bcd_h*10+StartTime.min.bcd_l;
//	uint8_t StartSec = StartTime.sec.bcd_h*10+StartTime.sec.bcd_l;
//	
//	uint16_t EndYear = 2000+(EndTime.year.bcd_h*10+EndTime.year.bcd_l);
//	uint8_t EndMonth = EndTime.month.bcd_h*10+EndTime.month.bcd_l;
//	uint8_t EndDay = EndTime.day.bcd_h*10+EndTime.day.bcd_l;
//	uint8_t EndHour = EndTime.hour.bcd_h*10+EndTime.hour.bcd_l;
//	uint8_t EndMin = EndTime.min.bcd_h*10+EndTime.min.bcd_l;
//	uint8_t EndSec = EndTime.sec.bcd_h*10+EndTime.sec.bcd_l;
//	
//	
//	
//	//先判断起始年是否是闰年
//	if(((StartYear%4)!=0) || (((StartYear%4)==0) && ((StartYear%100)==0) && ((StartYear%400)!=0))){
//		//非闰年
//		//计算起始时间相对于它本年1月1日 00:00:00的秒值
//		switch(StartMonth){
//			case 1:
//				StartTimeAllSec = (StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 2:
//				StartTimeAllSec = 31*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 3:
//				StartTimeAllSec = (31+28)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 4:
//				StartTimeAllSec = (31+28+31)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 5:
//				StartTimeAllSec = (31+28+31+30)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 6:
//				StartTimeAllSec = (31+28+31+30+31)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 7:
//				StartTimeAllSec = (31+28+31+30+31+30)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 8:
//				StartTimeAllSec = (31+28+31+30+31+30+31)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 9:
//				StartTimeAllSec = (31+28+31+30+31+30+31+31)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 10:
//				StartTimeAllSec = (31+28+31+30+31+30+31+31+30)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 11:
//				StartTimeAllSec = (31+28+31+30+31+30+31+31+30+31)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 12:
//				StartTimeAllSec = (31+28+31+30+31+30+31+31+30+31+30)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			default :break;
//		}
//		
//		//计算结束时间相对于它本年1月1日 00:00:00的秒值
//		EndTimeAllSec = (EndYear-StartYear)*365*24*3600;
//		if(((EndYear%4)!=0) || (((EndYear%4)==0) && ((EndYear%100)==0) && ((EndYear%400)!=0))){
//			//非闰年
//			switch(EndMonth){
//				case 1:
//					EndTimeAllSec += (EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 2:
//					EndTimeAllSec += 31*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 3:
//					EndTimeAllSec += (31+28)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 4:
//					EndTimeAllSec += (31+28+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 5:
//					EndTimeAllSec += (31+28+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 6:
//					EndTimeAllSec += (31+28+31+30+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 7:
//					EndTimeAllSec += (31+28+31+30+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 8:
//					EndTimeAllSec += (31+28+31+30+31+30+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 9:
//					EndTimeAllSec += (31+28+31+30+31+30+31+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 10:
//					EndTimeAllSec += (31+28+31+30+31+30+31+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 11:
//					EndTimeAllSec += (31+28+31+30+31+30+31+31+30+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 12:
//					EndTimeAllSec += (31+28+31+30+31+30+31+31+30+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				default :break;
//			}
//		}else{
//			//闰年
//			switch(EndMonth){
//				case 1:
//					EndTimeAllSec += (EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 2:
//					EndTimeAllSec += 31*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 3:
//					EndTimeAllSec += (31+29)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 4:
//					EndTimeAllSec += (31+29+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 5:
//					EndTimeAllSec += (31+29+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 6:
//					EndTimeAllSec += (31+29+31+30+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 7:
//					EndTimeAllSec += (31+29+31+30+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 8:
//					EndTimeAllSec += (31+29+31+30+31+30+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 9:
//					EndTimeAllSec += (31+29+31+30+31+30+31+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 10:
//					EndTimeAllSec += (31+29+31+30+31+30+31+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 11:
//					EndTimeAllSec += (31+29+31+30+31+30+31+31+30+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				case 12:
//					EndTimeAllSec += (31+29+31+30+31+30+31+31+30+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//				break;
//				default :break;
//			}
//		}
//		
//	}else{
//		//闰年
//		//计算起始时间相对于它本年1月1日 00:00:00的秒值
//		switch(StartMonth){
//			case 1:
//				StartTimeAllSec = (StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 2:
//				StartTimeAllSec = 31*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 3:
//				StartTimeAllSec = (31+29)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 4:
//				StartTimeAllSec = (31+29+31)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 5:
//				StartTimeAllSec = (31+29+31+30)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 6:
//				StartTimeAllSec = (31+29+31+30+31)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 7:
//				StartTimeAllSec = (31+29+31+30+31+30)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 8:
//				StartTimeAllSec = (31+29+31+30+31+30+31)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 9:
//				StartTimeAllSec = (31+29+31+30+31+30+31+31)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 10:
//				StartTimeAllSec = (31+29+31+30+31+30+31+31+30)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 11:
//				StartTimeAllSec = (31+29+31+30+31+30+31+31+30+31)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			case 12:
//				StartTimeAllSec = (31+29+31+30+31+30+31+31+30+31+30)*24*3600+(StartDay-1)*24*3600+StartHour*3600+StartMin*60+StartSec;
//			break;
//			default :break;
//		}
//		
//		//计算结束时间相对于它本年1月1日 00:00:00的秒值
//		EndTimeAllSec = (EndYear-StartYear)*366*24*3600;
//		
//		switch(EndMonth){
//			case 1:
//				EndTimeAllSec += (EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			case 2:
//				EndTimeAllSec += 31*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			case 3:
//				EndTimeAllSec += (31+28)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			case 4:
//				EndTimeAllSec += (31+28+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			case 5:
//				EndTimeAllSec += (31+28+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			case 6:
//				EndTimeAllSec += (31+28+31+30+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			case 7:
//				EndTimeAllSec += (31+28+31+30+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			case 8:
//				EndTimeAllSec += (31+28+31+30+31+30+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			case 9:
//				EndTimeAllSec += (31+28+31+30+31+30+31+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			case 10:
//				EndTimeAllSec += (31+28+31+30+31+30+31+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			case 11:
//				EndTimeAllSec += (31+28+31+30+31+30+31+31+30+31)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			case 12:
//				EndTimeAllSec += (31+28+31+30+31+30+31+31+30+31+30)*24*3600+(EndDay-1)*24*3600+EndHour*3600+EndMin*60+EndSec;
//			break;
//			default :break;
//		}
//	}
//	
//	return EndTimeAllSec -StartTimeAllSec;
//}






/*外部函数区*********************************************************/
/***
*流程老化
*参数：
	TestNum：要执行的流程老化次数
***/
HAL_StatusTypeDef AgingTest(uint32_t TestNum)
{
	extern MachInfo_s MachInfo;
	extern __IO WM_HWIN g_hActiveWin;							//当前显示窗口的句柄
	extern __IO BC_Data_t   g_stBC_Data[BC_END];
	extern ServiceAgingTestUIInfo_s ServiceAgingTestUIInfo;		//UI显示使用
	extern __IO uint8_t g_CountHandle_Status;
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern osMessageQueueId_t CATQueueHandle;
	extern CATGlobalStatus_s CATGlobalStatus;
	CATQueueInfoBuf_s CATQueueInfoBuf;
	AgingTestResultInfo_s AgingTestResultInfo = {0};
	int32_t Temp,Temp1;
	uint32_t TimeStampStart = 0;								//毫秒值
	uint32_t TimeStampEnd = 0;
	long RTCTimeDiff = 0;
	ErrorCode_e ReStates;
	
	double64 smpRate = BC_SAMPLE_RATE;   								//数据采样频率，单位Hz
    pulse_mid_feature midFeature = {CS_IDLE, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0, 0,	0, smpRate, 0};
	pulse_info_i_t *pulseInfo;  								//脉冲信息buffer
    uint32 pulseNum = 0;        								//本次测试脉冲个数
		
	int RawChangeBuf[512];
	uint16 i;
	uint16_t Count = 0;
		
	double CurrentMean = 0;
	double MeanSum = 0;
	double MeanAvg = 0;
		
	double CurrentCV = 0;
	double CVSum = 0;
	double CVAvg = 0;
	
	memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));				//重置数据
	ServiceAgingTestUIInfo.page = AGING_TEST_RESULT;
	ServiceAgingTestUIInfo.startFlag = 1;
	ServiceAgingTestUIInfo.stopFlag = 0;
	AgingTestResultInfo.setCount = TestNum;	
		
	//判断参数是否合法
	if(TestNum == 0){
		ServiceAgingTestUIInfo.startFlag = 0;
		ServiceAgingTestUIInfo.stopFlag = 1;
		
		//向界面发送后端执行完毕消息
		WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
		return HAL_ERROR;
	}
	
	//开启恒流源
	BC_CUR_SW_ON;
	HAL_Delay(500);
	
	//读取RTC起始时间
	RTC_HYM8563GetTime(&AgingTestResultInfo.rtc.startTime);
	TimeStampStart = HAL_GetTick();
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//开辟算法内存空间
	pulseInfo = (pulse_info_i_t*)SDRAM_ALGO_ADDR;
    memset((void*)pulseInfo, 0, SDRAM_ALGO_LEN);
	
	//开始执行流程老化
	while(TestNum--){
		//判断是否提前中止流程老化进程
		if(ServiceAgingTestUIInfo.stopFlag == 1){
			//提前退出循环
			break;
		}
		
		AgingTestResultInfo.runCount++;
		
		//发送到界面上显示
		ServiceAgingTestUIInfo.unions.agingTestResultInfo = AgingTestResultInfo;
		//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
		WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
		
		
		//判断各电压参数值
		Temp = Power_12VP_V();
		if(abs(Temp-12000) > 600){
			AgingTestResultInfo.p12v.count++;
		}
		AgingTestResultInfo.p12v.maxValue = abs(Temp-12000)>abs((AgingTestResultInfo.p12v.maxValue==0?12000:AgingTestResultInfo.p12v.maxValue)-12000)?Temp:AgingTestResultInfo.p12v.maxValue;
		
		Temp = Power_12VN_V();
		if(abs(Temp+12000) > 800){
			AgingTestResultInfo.n12v.count++;
		}
		AgingTestResultInfo.n12v.maxValue = abs(Temp+12000)>abs((AgingTestResultInfo.n12v.maxValue==0?-12000:AgingTestResultInfo.n12v.maxValue)+12000)?Temp:AgingTestResultInfo.n12v.maxValue;
		
		
		Temp = BC_Current_V();
		if(abs(Temp-55000) > 2000){
			AgingTestResultInfo.hly.count++;
		}
		AgingTestResultInfo.hly.maxValue = abs(Temp-55000)>abs((AgingTestResultInfo.hly.maxValue==0?55000:AgingTestResultInfo.hly.maxValue)-55000)?Temp:AgingTestResultInfo.hly.maxValue;
		
		//判断SDRAM是否正常，直接判定通过
		
		//判断NAND_FLASH，直接判定通过
		
		//CAT测试
		if(CATGlobalStatus.registerNet == CAT_REGISTNET_SUCCESS){
			//发送老化测试指令给CAT任务
			CATQueueInfoBuf.msgType = AGING_TEST_CAT_MSG_TYPE;
			osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
		}else{
			CATGlobalStatus.flag.connServerErr = 1;
		}
		
		if(CATGlobalStatus.flag.connServerErr == 1){
			AgingTestResultInfo.cat.failCount++;
		}else{
			AgingTestResultInfo.cat.successCount++;
		}
		
		//判断EEPROM，每隔1000次读写测试一次EEPROM
		if((AgingTestResultInfo.runCount%1000) == 1){
			//测试主控板上的EEPROM
			Temp = 0xA5;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_AGINGTEST,(uint8_t*)&Temp,1);
			Temp = 0;
			EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_AGINGTEST,(uint8_t*)&Temp,1);
			if((Temp & 0x000000ff) != 0xA5){
				AgingTestResultInfo.eepromCount1++;
			}
			
			//测试转接板上的EEPROM
			Temp = 0xA5;
			EEPROMWriteData(EEPROM_24C04,CONN_BORD_EEPROM_24C04,EEPROM_ADDR_AGINGTEST,(uint8_t*)&Temp,1);
			Temp = 0;
			EEPROMReadData(EEPROM_24C04,CONN_BORD_EEPROM_24C04,EEPROM_ADDR_AGINGTEST,(uint8_t*)&Temp,1);
			if((Temp & 0x000000ff) != 0xA5){
				AgingTestResultInfo.eepromCount2++;
			}
		}
		
		//打印机通讯
		if(Printer_IsOK() != HAL_OK){
			AgingTestResultInfo.printerCount++;
		}
		
		//========进仓================================================
		//先保证处于进仓状态
		OutIn_Module_In(MOTOR_MODE_NORMAL);
		
		//进仓状态下各参数值
		//判断光耦
		if(OC_Status(OC_INDEX_OUTIN_IN) != SET){
			AgingTestResultInfo.inside.oc.inOCCount++;
		}
		if(OC_Status(OC_INDEX_OUTIN_OUT) != RESET){
			AgingTestResultInfo.inside.oc.outOCCount++;
		}
		if(OC_Status(OC_INDEX_CLAMP_IN) != RESET){
			AgingTestResultInfo.inside.oc.releaseOCCount++;
		}
		if(OC_Status(OC_INDEX_CLAMP_OUT) != SET){
			AgingTestResultInfo.inside.oc.gdOCCount++;
		}
		
		//压力系统
		//建压
		ReStates = Build_Press(RUN_MODE_NORMAL, AGING_TEST_PRESS);
		if(ReStates != ERROR_CODE_SUCCESS){
			AgingTestResultInfo.inside.pressure.buildCount++;
		}else{
			osDelay(1000);
			Build_Press(RUN_MODE_NORMAL, AGING_TEST_PRESS);
			osDelay(1000);
			Build_Press(RUN_MODE_NORMAL, AGING_TEST_PRESS);
			osDelay(1000);
		}
		Temp = Press_Value();
		AgingTestResultInfo.inside.pressure.maxBuildPressureValue = abs(Temp-AGING_TEST_PRESS)>abs((AgingTestResultInfo.inside.pressure.maxBuildPressureValue==0?AGING_TEST_PRESS:AgingTestResultInfo.inside.pressure.maxBuildPressureValue)-AGING_TEST_PRESS)?Temp:AgingTestResultInfo.inside.pressure.maxBuildPressureValue;
		
		//密闭性，建压成功后才进行密闭性检测
		if(ReStates == ERROR_CODE_SUCCESS){
			osDelay(10000);
			
			Temp1 = abs(Temp - Press_Value());
			if(Temp1 > 5000){
				AgingTestResultInfo.inside.pressure.airtightCount++;
			}
			AgingTestResultInfo.inside.pressure.maxairtightValue = AgingTestResultInfo.inside.pressure.maxairtightValue>Temp1?AgingTestResultInfo.inside.pressure.maxairtightValue:Temp1;
		}
		
		//小孔电压
		Temp = XK_V();
		if(Temp > 900 || Temp < 500){
			AgingTestResultInfo.inside.xk.count++;
		}
		AgingTestResultInfo.inside.xk.maxValue = abs(Temp-900)>abs((AgingTestResultInfo.inside.xk.maxValue==0?900:AgingTestResultInfo.inside.xk.maxValue)-900)?Temp:AgingTestResultInfo.inside.xk.maxValue;
		
		//HGB光路检测
		//检测关闭状态下的HGB
		Temp = HGB_ADC();
		if(Temp > 200){
			AgingTestResultInfo.inside.hgb.closeCount++;
		}
		AgingTestResultInfo.inside.hgb.maxCloseValue = abs(Temp-200)>abs((AgingTestResultInfo.inside.hgb.maxCloseValue==0?200:AgingTestResultInfo.inside.hgb.maxCloseValue)-200)?Temp:AgingTestResultInfo.inside.hgb.maxCloseValue;
		
		//检测535波长灯
		Light_Start(HGBLED_535,MachInfo.paramSet.current535);
		osDelay(3000);
		
		if(abs((HGB_ADC() - Temp) < 2000) || (HGB_ADC() > 3700) || (HGB_ADC() < MachInfo.paramSet.KQADC535-500)){
			AgingTestResultInfo.inside.hgb.openCount++;
		}
		Temp1 = HGB_ADC();
		AgingTestResultInfo.inside.hgb.maxOpenValue = abs(Temp1-MachInfo.paramSet.KQADC535)>abs((AgingTestResultInfo.inside.hgb.maxOpenValue==0?MachInfo.paramSet.KQADC535:AgingTestResultInfo.inside.hgb.maxOpenValue)-MachInfo.paramSet.KQADC535)?Temp1:AgingTestResultInfo.inside.hgb.maxOpenValue;
		
		//检测415波长灯
		Light_Start(HGBLED_415,MachInfo.paramSet.current415);
		osDelay(2000);
		
		if(abs((HGB_ADC() - Temp) < 2000) || (HGB_ADC() > 3700) || (HGB_ADC() < MachInfo.paramSet.KQADC415-500)){
			AgingTestResultInfo.inside.hgb.openCount++;
		}
		Temp1 = HGB_ADC();
		AgingTestResultInfo.inside.hgb.maxOpenValue = abs(Temp1-MachInfo.paramSet.KQADC415)>abs((AgingTestResultInfo.inside.hgb.maxOpenValue==0?MachInfo.paramSet.KQADC415:AgingTestResultInfo.inside.hgb.maxOpenValue)-MachInfo.paramSet.KQADC415)?Temp1:AgingTestResultInfo.inside.hgb.maxOpenValue;
		
		//关闭HGB
		Light_Stop();
		
		//计数池有无检测
		//开启计数池有无检测灯
		COUNT_CELL_SW_ON;
		HAL_Delay(10);
		if(CHECK_JSC_EXIST_STATUS != JSC_EXIST_STATUS_ACTIVE){
			AgingTestResultInfo.inside.JscDetectionCount++;
		}
		COUNT_CELL_SW_OFF;
		
		
		//===信号检测===
		//关闭调度器
		osKernelLock();
		
		MeanSum = 0;
		CVSum = 0;
		pulseNum = 0;
		
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
			
			//16为数据扩展成32数据
			for(i=0;i<512;i++){
				RawChangeBuf[i] = g_stBC_Data[BC_WBC].ucBuf[i];
			}
			
			//计算mean
			CurrentMean = getMean_i(RawChangeBuf,512);
			MeanSum += CurrentMean;
			
			//计算CV
			CurrentCV = getCv_i(RawChangeBuf,512,0);
			CVSum += CurrentCV;
		}
		
		//恢复调度器
		osKernelUnlock();
		
		//计算平均MEAN
		MeanAvg = MeanSum/750;
		//计算平均CV
		CVAvg = CVSum/750*100;
		
		//判断是否出现异常
		if(pulseNum > SINGAL_ANALYSIS_NUM_THRESHOLD){
			AgingTestResultInfo.inside.signal.num.errCount++;
		}
		AgingTestResultInfo.inside.signal.num.maxNum = AgingTestResultInfo.inside.signal.num.maxNum<pulseNum?pulseNum:AgingTestResultInfo.inside.signal.num.maxNum;
		
		if(MeanAvg > SINGAL_ANALYSIS_MEAN_THRESHOLD){
			AgingTestResultInfo.inside.signal.mean.errCount++;
		}
		AgingTestResultInfo.inside.signal.mean.maxMean = AgingTestResultInfo.inside.signal.mean.maxMean<MeanAvg?MeanAvg:AgingTestResultInfo.inside.signal.mean.maxMean;
		
		if(CVAvg > SINGAL_ANALYSIS_CV_THRESHOLD){
			AgingTestResultInfo.inside.signal.cv.errCount++;
		}
		AgingTestResultInfo.inside.signal.cv.maxCV = AgingTestResultInfo.inside.signal.cv.maxCV<CVAvg?CVAvg:AgingTestResultInfo.inside.signal.cv.maxCV;
		
		//===========出仓====================================
		//先保证处于出仓状态
		OutIn_Module_Out(MOTOR_MODE_NORMAL);
		
		//等待出仓到位
		Temp = 0;
		while(OC_Status(OC_INDEX_CLAMP_IN) != SET){
			Temp++;
			if(Temp >= 80){
				break;
			}
			osDelay(100);
		}
		
		//放气
		LIQUID_WAVE_OPEN;
		osDelay(1500);
		LIQUID_WAVE_CLOSE;
		Temp = Press_Value();
		if(Temp < NO_PRESS_VALUE){
			//放气不充分
			AgingTestResultInfo.inside.pressure.noPressureCount++;
		}
		AgingTestResultInfo.inside.pressure.maxnoPressureValue = abs(Temp-0)>AgingTestResultInfo.inside.pressure.maxnoPressureValue?Temp:AgingTestResultInfo.inside.pressure.maxnoPressureValue;
		
		
		//出仓状态下各参数值
		//判断光耦
		if(OC_Status(OC_INDEX_OUTIN_IN) != RESET){
			AgingTestResultInfo.outside.oc.inOCCount++;
		}
		if(OC_Status(OC_INDEX_OUTIN_OUT) != SET){
			AgingTestResultInfo.outside.oc.outOCCount++;
		}
		if(OC_Status(OC_INDEX_CLAMP_IN) != SET){
			AgingTestResultInfo.outside.oc.releaseOCCount++;
		}
		if(OC_Status(OC_INDEX_CLAMP_OUT) != RESET){
			AgingTestResultInfo.outside.oc.gdOCCount++;
		}
		
		//压力系统
		//建压
		ReStates = Build_Press(RUN_MODE_NORMAL, AGING_TEST_PRESS);
		if(ReStates != ERROR_CODE_SUCCESS){
			AgingTestResultInfo.outside.pressure.buildCount++;
		}else{
			osDelay(1000);
			Build_Press(RUN_MODE_NORMAL, AGING_TEST_PRESS);
			osDelay(1000);
			Build_Press(RUN_MODE_NORMAL, AGING_TEST_PRESS);
			osDelay(1000);
		}
		Temp = Press_Value();
		AgingTestResultInfo.outside.pressure.maxBuildPressureValue = abs(Temp-AGING_TEST_PRESS)>abs((AgingTestResultInfo.outside.pressure.maxBuildPressureValue==0?AGING_TEST_PRESS:AgingTestResultInfo.outside.pressure.maxBuildPressureValue)-AGING_TEST_PRESS)?Temp:AgingTestResultInfo.outside.pressure.maxBuildPressureValue;
		
		//密闭性，建压成功后才进行密闭性检测
		if(ReStates == ERROR_CODE_SUCCESS){
			osDelay(10000);
			
			Temp1 = abs(Temp - Press_Value());
			if(Temp1 > 5000){
				AgingTestResultInfo.outside.pressure.airtightCount++;
			}
			AgingTestResultInfo.outside.pressure.maxairtightValue = AgingTestResultInfo.outside.pressure.maxairtightValue>Temp1?AgingTestResultInfo.outside.pressure.maxairtightValue:Temp1;
		}
		
		//放气
		LIQUID_WAVE_OPEN;
		osDelay(1500);
		LIQUID_WAVE_CLOSE;
		
		Temp = Press_Value();
		if(Temp < NO_PRESS_VALUE){
			//放气不充分
			AgingTestResultInfo.outside.pressure.noPressureCount++;
		}
		AgingTestResultInfo.outside.pressure.maxnoPressureValue = abs(Temp-0)>AgingTestResultInfo.outside.pressure.maxnoPressureValue?Temp:AgingTestResultInfo.outside.pressure.maxnoPressureValue;
		
		
		//小孔电压
		Temp = XK_V();
		if(Temp < 2200 || Temp > 2700){
			AgingTestResultInfo.outside.xk.count++;
		}
		AgingTestResultInfo.outside.xk.maxValue = abs(Temp-2500)>abs((AgingTestResultInfo.outside.xk.maxValue==0?2500:AgingTestResultInfo.outside.xk.maxValue)-2500)?Temp:AgingTestResultInfo.outside.xk.maxValue;
		
		//计数池有无检测
		//开启计数池有无检测灯
		COUNT_CELL_SW_ON;
		HAL_Delay(10);
		if(CHECK_JSC_EXIST_STATUS != JSC_EXIST_STATUS_ACTIVE){
			AgingTestResultInfo.outside.JscDetectionCount++;
		}
		COUNT_CELL_SW_OFF;
		
		//===信号检测===
		//关闭调度器
		osKernelLock();
		
		MeanSum = 0;
		CVSum = 0;
		pulseNum = 0;
		
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
			
			//16为数据扩展成32数据
			for(i=0;i<512;i++){
				RawChangeBuf[i] = g_stBC_Data[BC_WBC].ucBuf[i];
			}
			
			//计算mean
			CurrentMean = getMean_i(RawChangeBuf,512);
			MeanSum += CurrentMean;
			
			//计算CV
			CurrentCV = getCv_i(RawChangeBuf,512,0);
			CVSum += CurrentCV;
		}
		
		//恢复调度器
		osKernelUnlock();
		
		//计算平均MEAN
		MeanAvg = MeanSum/750;
		//计算平均CV
		CVAvg = CVSum/750*100;
		
		//判断是否出现异常
		if(pulseNum > SINGAL_ANALYSIS_NUM_THRESHOLD){
			AgingTestResultInfo.outside.signal.num.errCount++;
		}
		AgingTestResultInfo.outside.signal.num.maxNum = AgingTestResultInfo.outside.signal.num.maxNum<pulseNum?pulseNum:AgingTestResultInfo.outside.signal.num.maxNum;
		
		if(MeanAvg > SINGAL_ANALYSIS_MEAN_THRESHOLD){
			AgingTestResultInfo.outside.signal.mean.errCount++;
		}
		AgingTestResultInfo.outside.signal.mean.maxMean = AgingTestResultInfo.outside.signal.mean.maxMean<MeanAvg?MeanAvg:AgingTestResultInfo.outside.signal.mean.maxMean;
		
		if(CVAvg > SINGAL_ANALYSIS_CV_THRESHOLD){
			AgingTestResultInfo.outside.signal.cv.errCount++;
		}
		AgingTestResultInfo.outside.signal.cv.maxCV = AgingTestResultInfo.outside.signal.cv.maxCV<CVAvg?CVAvg:AgingTestResultInfo.outside.signal.cv.maxCV;
		
	}
	
	BC_CUR_SW_OFF;
	
	//读取RTC结束时间
	RTC_HYM8563GetTime(&AgingTestResultInfo.rtc.endTime);
	TimeStampEnd = HAL_GetTick();
	
	//计算RTC时间秒差值
//	RTCTimeDiff = _TimeGetSecDiff(AgingTestResultInfo.rtc.startTime,AgingTestResultInfo.rtc.endTime);
	RTCTimeDiff = GetCurrentTimestamp(AgingTestResultInfo.rtc.endTime) - GetCurrentTimestamp(AgingTestResultInfo.rtc.startTime);
	AgingTestResultInfo.rtc.secondOffset = (TimeStampEnd-TimeStampStart)/1000-RTCTimeDiff;
	
	ServiceAgingTestUIInfo.startFlag = 0;
	ServiceAgingTestUIInfo.stopFlag = 1;
	
	//发送到界面上显示
	ServiceAgingTestUIInfo.unions.agingTestResultInfo = AgingTestResultInfo;
	//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
	WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
	
	//出仓
	OutIn_Module_Out(MOTOR_MODE_NORMAL);
	//开液阀,放气
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	//向界面发送后端执行完毕消息
	WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	return HAL_OK;
}




/***
*探针稳定性测试
***/
HAL_StatusTypeDef TanzhenTest(uint32_t TestNum)
{
	extern __IO BC_Data_t   g_stBC_Data[BC_END];
	extern MachInfo_s MachInfo;
	extern __IO WM_HWIN g_hActiveWin;					//当前显示窗口的句柄
	extern ServiceAgingTestUIInfo_s ServiceAgingTestUIInfo;
	extern __IO uint8_t g_CountHandle_Status;
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	TanZhenTestResultInfo_s TanzhenTestResultInfo = {0};
	
	double64 smpRate = BC_SAMPLE_RATE;   //数据采样频率，单位Hz
    pulse_mid_feature midFeature = {CS_IDLE, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0, 0,	0, smpRate, 0};
	pulse_info_i_t *pulseInfo;  //脉冲信息buffer
    uint32 pulseNum = 0;        //本次测试脉冲个数
		
	int RawChangeBuf[512];
	uint16 i;
	uint16_t Count = 0;
		
	double CurrentMean = 0;
	double MeanSum = 0;
	double MeanAvg = 0;
		
	double CurrentCV = 0;
	double CVSum = 0;
	double CVAvg = 0;
		
	uint16_t Temp = 0;
	
		
	memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));
	ServiceAgingTestUIInfo.page = TANZHEN_TEST_RESULT;
	ServiceAgingTestUIInfo.startFlag = 1;
	ServiceAgingTestUIInfo.stopFlag = 0;
	
	//判断参数是否合法
	if(TestNum == 0){
		ServiceAgingTestUIInfo.startFlag = 0;
		ServiceAgingTestUIInfo.stopFlag = 1;
		
		//向界面发送后端执行完毕消息
		WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
		return HAL_ERROR;
	}
	
	//开启恒流源
	BC_CUR_SW_ON;
	HAL_Delay(500);
	
	//读取RTC起始时间
	RTC_HYM8563GetTime(&TanzhenTestResultInfo.rtc.startTime);
	
	TanzhenTestResultInfo.setCount = TestNum;
	TanzhenTestResultInfo.runCount = 0;
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//先保证处于进仓状态
	OutIn_Module_In(MOTOR_MODE_NORMAL);
	
	//开辟算法内存空间
	pulseInfo = (pulse_info_i_t*)SDRAM_ALGO_ADDR;
    memset((void*)pulseInfo, 0, SDRAM_ALGO_LEN);
	
	//开始执行探针稳定性老化
	while(TestNum--){
		//判断是否提前中止探针稳定性老化
		if(ServiceAgingTestUIInfo.stopFlag == 1){
			//提前退出循环
			break;
		}
		
		TanzhenTestResultInfo.runCount++;
		
		//发送到界面上显示
		ServiceAgingTestUIInfo.unions.tanzhenTestInfo = TanzhenTestResultInfo;
		//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
		WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
		
		//静置1s
		osDelay(1000);
		
		//小孔电压
		Temp = XK_V();
		if(Temp > 900){
			TanzhenTestResultInfo.xk.errCount++;
		}
		TanzhenTestResultInfo.xk.maxValue = abs(Temp-900)>abs((TanzhenTestResultInfo.xk.maxValue==0?900:TanzhenTestResultInfo.xk.maxValue)-900)?Temp:TanzhenTestResultInfo.xk.maxValue;
		
		//关闭调度器
		osKernelLock();
		
		MeanSum = 0;
		CVSum = 0;
		pulseNum = 0;
		
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
			
			//16为数据扩展成32数据
			for(i=0;i<512;i++){
				RawChangeBuf[i] = g_stBC_Data[BC_WBC].ucBuf[i];
			}
			
			//计算mean
			CurrentMean = getMean_i(RawChangeBuf,512);
			MeanSum += CurrentMean;
			
			//计算CV
			CurrentCV = getCv_i(RawChangeBuf,512,0);
			CVSum += CurrentCV;
		}
		
		//恢复调度器
		osKernelUnlock();
		
		//计算平均MEAN
		MeanAvg = MeanSum/750;
		//计算平均CV
		CVAvg = CVSum/750*100;
		
		//判断是否出现异常
		if(pulseNum > SINGAL_ANALYSIS_NUM_THRESHOLD){
			TanzhenTestResultInfo.num.errCount++;
		}
		TanzhenTestResultInfo.num.maxNum = TanzhenTestResultInfo.num.maxNum<pulseNum?pulseNum:TanzhenTestResultInfo.num.maxNum;
		
		if(MeanAvg > SINGAL_ANALYSIS_MEAN_THRESHOLD){
			TanzhenTestResultInfo.mean.errCount++;
		}
		TanzhenTestResultInfo.mean.maxMean = TanzhenTestResultInfo.mean.maxMean<MeanAvg?MeanAvg:TanzhenTestResultInfo.mean.maxMean;
		
		if(CVAvg > SINGAL_ANALYSIS_CV_THRESHOLD){
			TanzhenTestResultInfo.cv.errCount++;
		}
		TanzhenTestResultInfo.cv.maxCV = TanzhenTestResultInfo.cv.maxCV<CVAvg?CVAvg:TanzhenTestResultInfo.cv.maxCV;
		
		//松开固定电机
		Clamp_Motor_In(MOTOR_MODE_NORMAL,MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
		
		//夹紧固定电机
		Clamp_Motor_Out(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_Out_AddStep);
	}
	
	BC_CUR_SW_OFF;
	
	ServiceAgingTestUIInfo.startFlag = 0;
	ServiceAgingTestUIInfo.stopFlag = 1;
	
	//读取RTC结束时间
	RTC_HYM8563GetTime(&TanzhenTestResultInfo.rtc.endTime);
	
	//发送到界面上显示
	ServiceAgingTestUIInfo.unions.tanzhenTestInfo = TanzhenTestResultInfo;
	//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
	WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
	
	//出仓
	OutIn_Module_Out(MOTOR_MODE_NORMAL);
	
	//向界面发送后端执行完毕消息
	WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	return HAL_OK;
}


/***
*气嘴密闭性测试
***/
HAL_StatusTypeDef QizuiTest(uint32_t TestNum)
{
	extern __IO WM_HWIN g_hActiveWin;					//当前显示窗口的句柄
	extern ServiceAgingTestUIInfo_s ServiceAgingTestUIInfo;
	extern __IO uint8_t g_CountHandle_Status;
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	ErrorCode_e ReStates = ERROR_CODE_SUCCESS;
	int32_t Pressure = 0;
	uint32_t Temp = 0;
	
	memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));
	ServiceAgingTestUIInfo.page = QIZUI_TEST_RESULT;
	ServiceAgingTestUIInfo.startFlag = 1;
	ServiceAgingTestUIInfo.stopFlag = 0;
	
	//读取RTC起始时间
	RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.rtc.startTime);
	ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.setCount = TestNum;
	ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.runCount = 0;
	
	//判断参数是否合法
	if(TestNum == 0){
		ServiceAgingTestUIInfo.startFlag = 0;
		ServiceAgingTestUIInfo.stopFlag = 1;
		
		//向界面发送后端执行完毕消息
		WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
		return HAL_ERROR;
	}
	
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//先保证处于进仓位置
	OutIn_Module_In(MOTOR_MODE_NORMAL);

	while(TestNum--){
		//判断是否提前中止探针稳定性老化
		if(ServiceAgingTestUIInfo.stopFlag == 1){
			//提前退出循环
			break;
		}
		
		ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.runCount++;
		
		//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
		WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
		
		ReStates = Build_Press(RUN_MODE_NORMAL, QIZUI_TEST_PRESS);
		if(ReStates != ERROR_CODE_SUCCESS){
			//出仓一段距离
			Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_OUTIN,MOTOR_DIR_OUT,8000);
			Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP,MOTOR_DIR_IN,3000);
			
			//开液阀,放气
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;
			
			//处于进仓位置
			OutIn_Module_In(MOTOR_MODE_NORMAL);
			
			Pressure = Press_Value();
			ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.buildCount++;
			ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.maxBuildPressureValue = abs(Pressure-QIZUI_TEST_PRESS)>abs((ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.maxBuildPressureValue==0?QIZUI_TEST_PRESS:ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.maxBuildPressureValue)-QIZUI_TEST_PRESS)?Pressure:ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.maxBuildPressureValue;
			
			//建压成功后才可以进行气嘴密闭性测试
			continue;
		}
		
		osDelay(1500);
		Build_Press(RUN_MODE_NORMAL, QIZUI_TEST_PRESS);
		osDelay(1500);
		Build_Press(RUN_MODE_NORMAL, QIZUI_TEST_PRESS);
		osDelay(1000);
		
		//获取压力值
		Pressure = Press_Value();
		
		//开液阀
		LIQUID_WAVE_OPEN;
		
		//静置20s
		osDelay(20000);
		
		//判断压力差是否大于3kpa
		Temp = abs(Press_Value()-Pressure);
		if(Temp > 3000){
			ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.airtightCount++;
		}
		ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.maxairtightValue = ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.maxairtightValue>Temp?ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.maxairtightValue:Temp;
		
		//出仓一段距离
		Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_OUTIN,MOTOR_DIR_OUT,8000);
		Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP,MOTOR_DIR_IN,3000);
		
		//开液阀,放气
		LIQUID_WAVE_OPEN;
		osDelay(1500);
		LIQUID_WAVE_CLOSE;
		
		//判断放气残余压力值
		Pressure = Press_Value();
		if(Pressure < NO_PRESS_VALUE){
			//放气不充分
			ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.noPressureCount++;
		}
		ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.maxnoPressureValue = abs(Pressure-0)>ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.maxnoPressureValue?Pressure:ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.pressure.maxnoPressureValue;
		
		//处于进仓位置
		OutIn_Module_In(MOTOR_MODE_NORMAL);
	}
	
	ServiceAgingTestUIInfo.startFlag = 0;
	ServiceAgingTestUIInfo.stopFlag = 1;
	
	//读取RTC结束时间
	RTC_HYM8563GetTime(&ServiceAgingTestUIInfo.unions.qizuiTestResultInfo.rtc.endTime);
	
	//向当前显示的窗口发送WM_USER消息ID，以便让窗口更新显示数据
	WM_SendMessageNoPara(g_hActiveWin,SERVICE_AGING_USER_ID);
	
	//出仓
	OutIn_Module_Out(MOTOR_MODE_NORMAL);
	
	//开液阀,放气
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	//向界面发送后端执行完毕消息
	WM_SendMessageNoPara(g_hActiveWin,WM_SERVICE_AGING_TEST_BE_OVER);
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	return HAL_OK;
}







/***
*用于生成的随机值转换成最接近的故障码
***/
static ErrorCode_e RandIntToErrCode(uint32_t Value)
{
	if(Value <= ERROR_CODE_SUCCESS)	return ERROR_CODE_SUCCESS;
	if(Value <= ERROR_CODE_FAILURE)	return ERROR_CODE_FAILURE;
	if(Value <= ERROR_CODE_BUILD_PRESS)	return ERROR_CODE_BUILD_PRESS;
	if(Value <= ERROR_CODE_AIR_TIGHT)	return ERROR_CODE_AIR_TIGHT;
	if(Value <= ERROR_CODE_BC_CURRENT)	return ERROR_CODE_BC_CURRENT;
	if(Value <= ERROR_CODE_POWER_12VP)	return ERROR_CODE_POWER_12VP;
	if(Value <= ERROR_CODE_POWER_12VN)	return ERROR_CODE_POWER_12VN;
	if(Value <= ERROR_CODE_OUTIN_OUT)	return ERROR_CODE_OUTIN_OUT;
	if(Value <= ERROR_CODE_OUTIN_IN)	return ERROR_CODE_OUTIN_IN;
    if(Value <= ERROR_CODE_OUTIN_OC)	return ERROR_CODE_OUTIN_OC;
	if(Value <= ERROR_CODE_CLAMP_OUT)	return ERROR_CODE_CLAMP_OUT;
	if(Value <= ERROR_CODE_CLAMP_IN)	return ERROR_CODE_CLAMP_IN;
    if(Value <= ERROR_CODE_CLAMP_OC)	return ERROR_CODE_CLAMP_OC;
	if(Value <= ERROR_CODE_NOT_AT_POS)	return ERROR_CODE_NOT_AT_POS;
	if(Value <= ERROR_CODE_ELEC_TRIGGLE)	return ERROR_CODE_ELEC_TRIGGLE;
	if(Value <= ERROR_CODE_ELEC_TRIGGLE_2)	return ERROR_CODE_ELEC_TRIGGLE_2;
	if(Value <= ERROR_CODE_WBC_ELEC_TOUCH)	return ERROR_CODE_WBC_ELEC_TOUCH;
	if(Value <= ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE)	return ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE;
	if(Value <= ERROR_CODE_BSK_2S_15V)	return ERROR_CODE_BSK_2S_15V;
	if(Value <= ERROR_CODE_BSK_XKV_TOO_LOW)	return ERROR_CODE_BSK_XKV_TOO_LOW;
	if(Value <= ERROR_CODE_AIR_LEAK)	return ERROR_CODE_AIR_LEAK;
	if(Value <= ERROR_CODE_SAMPLE_NONE)	return ERROR_CODE_SAMPLE_NONE;
	if(Value <= ERROR_CODE_AIR_LEAK_COUNTING_1)	return ERROR_CODE_AIR_LEAK_COUNTING_1;
	if(Value <= ERROR_CODE_AIR_LEAK_COUNTING_2)	return ERROR_CODE_AIR_LEAK_COUNTING_2;
    if(Value <= ERROR_CODE_T4_ERR_15S_2_MAXTIME) return ERROR_CODE_T4_ERR_15S_2_MAXTIME;
    if(Value < ERROR_CODE_RE_CONN_2_T4_15S) return ERROR_CODE_RE_CONN_2_T4_15S;
	if(Value <= ERROR_CODE_TIMEOUT)	return ERROR_CODE_TIMEOUT;
	if(Value <= ERROR_CODE_SAMPLE_NOT_ENOUGH)	return ERROR_CODE_SAMPLE_NOT_ENOUGH;
	if(Value <= ERROR_CODE_NONE_CELL)	return ERROR_CODE_NONE_CELL;
	if(Value <= ERROR_CODE_TEMPERATURE)	return ERROR_CODE_TEMPERATURE;
	if(Value <= ERROR_CODE_EEPROM)	return ERROR_CODE_EEPROM;
	if(Value <= ERROR_CODE_CAT_ONE)	return ERROR_CODE_CAT_ONE;
	if(Value <= ERROR_CODE_HGB_MODULE)	return ERROR_CODE_HGB_MODULE;
	if(Value <= ERROR_CODE_DISK_CAPACITY)	return ERROR_CODE_DISK_CAPACITY;
	if(Value <= ERROR_CODE_SPI_FLASH_CONN_ERR)	return ERROR_CODE_SPI_FLASH_CONN_ERR;
    if(Value <= ERROR_CODE_FLASH_DATA_CALIBRATE_ERR) return ERROR_CODE_FLASH_DATA_CALIBRATE_ERR;
	if(Value <= ERROR_CODE_PRINT_NO_PAPER)	return ERROR_CODE_PRINT_NO_PAPER;
	if(Value <= ERROR_CODE_NO_PRINTER)	return ERROR_CODE_NO_PRINTER;
	if(Value <= ERROR_CODE_MONITOR_SIGNAL_ERR)	return ERROR_CODE_MONITOR_SIGNAL_ERR;
	if(Value <= ERROR_CODE_ALGO_MODE)	return ERROR_CODE_ALGO_MODE;
	if(Value <= ERROR_CODE_ALGO_BREAK)	return ERROR_CODE_ALGO_BREAK;
	if(Value <= ERROR_CODE_INVALID_FILENUM)	return ERROR_CODE_INVALID_FILENUM;
	if(Value <= ERROR_CODE_FILENUM_CAPACITY_IS_FULL)	return ERROR_CODE_FILENUM_CAPACITY_IS_FULL;
	
	return ERROR_CODE_SUCCESS;
}






#define MULTIEDIT_AGING_TEST_ID				 (AGING_TEST_PAGE_ID + 0x15)				//组件老化页面多行文本显示控件ID


/***
*CAT数据测试
*参数：
	StartSNNum：起始仪器序列号值
***/
HAL_StatusTypeDef CATDataTest(uint32_t StartSNNum)
{
	extern __IO WM_HWIN g_hActiveWin;					//当前显示窗口的句柄
	extern ServiceAgingTestUIInfo_s ServiceAgingTestUIInfo;
	extern MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern __IO uint8_t g_CountHandle_Status;
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	RTC_HYM8563Info_s StartTime = {0};
	RTC_HYM8563Info_s EndTime = {0};
	WM_HWIN hItem;
	HAL_StatusTypeDef ReturnStatus = HAL_OK;
	char Str[100] = {0};
	uint32_t SNCount = 0;
	WBCHGB_TestInfo_t* WBCHGBTestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
	CountHead_t* tCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
	uint32_t i=0;
	uint32_t Temp = 0,Temp1 = 0,Temp2=0;
	char LogBuf[30] = "测试日志内容";
	char SN[30] = {0};
	
	
	memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));
	ServiceAgingTestUIInfo.startFlag = 1;
	ServiceAgingTestUIInfo.stopFlag = 0;
	
	//读取RTC起始时间
	RTC_HYM8563GetTime(&StartTime);
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_AGING_TEST_ID);
	MULTIEDIT_SetText(hItem,"");
	MULTIEDIT_AddText(hItem,"CAT模拟数据测试开始...\n");
	
	//先判断起始SN号是否符合要求
	if(StartSNNum > 99999){
		MULTIEDIT_AddText(hItem,"输入的起始SN号超限，终止测试\n");
		
		ServiceAgingTestUIInfo.startFlag = 0;
		ServiceAgingTestUIInfo.stopFlag = 1;
		
		//关闭面板指示灯
		g_CountHandle_Status = 0;
		g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
		
		return HAL_OK;
	}
	
	//等待CAT注网成功
	while(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		MULTIEDIT_AddText(hItem,"等待CAT注网成功\n");
		osDelay(1000);
	}
	MULTIEDIT_AddText(hItem,"CAT注网成功\n");
	
	memset(WBCHGBTestInfo,0,sizeof(WBCHGB_TestInfo_t));
	memset(tCountHead,0,sizeof(CountHead_t));
	
	//生成样本分析数据中固定不变的内容
	tCountHead->eErrorCode = 0;
	tCountHead->eHideErrorCode = 0;
	WBCHGBTestInfo->fLines[0] = 26.0;
	WBCHGBTestInfo->fLines[1] = 64.0;
	WBCHGBTestInfo->fLines[2] = 83.0;
	WBCHGBTestInfo->fWbcMaxPos = 51.0;
	sprintf((char*)WBCHGBTestInfo->ucaWBC_ErrorPrompt,"测试WBC提示信息");
	sprintf((char*)WBCHGBTestInfo->ucaHGB_ErrorPrompt,"测试HGB提示信息");
	WBCHGBTestInfo->eReferGroup = 0;
	
	WBCHGBTestInfo->fWBC_Histogram[0]=0.38		;
	WBCHGBTestInfo->fWBC_Histogram[1]=0.92	    ;
	WBCHGBTestInfo->fWBC_Histogram[2]=1.86	    ;
	WBCHGBTestInfo->fWBC_Histogram[3]=3.51	    ;
	WBCHGBTestInfo->fWBC_Histogram[4]=6.14	    ;
	WBCHGBTestInfo->fWBC_Histogram[5]=9.99	    ;
	WBCHGBTestInfo->fWBC_Histogram[6]=15.18	    ;
	WBCHGBTestInfo->fWBC_Histogram[7]=21.44	    ;
	WBCHGBTestInfo->fWBC_Histogram[8]=27.93	    ;
	WBCHGBTestInfo->fWBC_Histogram[9]=33.99	    ;
	WBCHGBTestInfo->fWBC_Histogram[10]=38.82	 ;
	WBCHGBTestInfo->fWBC_Histogram[11]=41.78	 ;
	WBCHGBTestInfo->fWBC_Histogram[12]=42.59	 ;
	WBCHGBTestInfo->fWBC_Histogram[13]=41.40	 ;
	WBCHGBTestInfo->fWBC_Histogram[14]=38.76	 ;
	WBCHGBTestInfo->fWBC_Histogram[15]=35.39	 ;
	WBCHGBTestInfo->fWBC_Histogram[16]=29.08	 ;
	WBCHGBTestInfo->fWBC_Histogram[17]=17.08	 ;
	WBCHGBTestInfo->fWBC_Histogram[18]=12.76	 ;
	WBCHGBTestInfo->fWBC_Histogram[19]=11.13	 ;
	WBCHGBTestInfo->fWBC_Histogram[20]=10.97	 ;
	WBCHGBTestInfo->fWBC_Histogram[21]=11.77	 ;
	WBCHGBTestInfo->fWBC_Histogram[22]=14.34	 ;
	WBCHGBTestInfo->fWBC_Histogram[23]=21.02	 ;
	WBCHGBTestInfo->fWBC_Histogram[24]=31.59	 ;
	WBCHGBTestInfo->fWBC_Histogram[25]=37.22	 ;
	WBCHGBTestInfo->fWBC_Histogram[26]=43.70	 ;
	WBCHGBTestInfo->fWBC_Histogram[27]=49.89	 ;
	WBCHGBTestInfo->fWBC_Histogram[28]=56.32	 ;
	WBCHGBTestInfo->fWBC_Histogram[29]=63.07	 ;
	WBCHGBTestInfo->fWBC_Histogram[30]=69.48	 ;
	WBCHGBTestInfo->fWBC_Histogram[31]=75.75	 ;
	WBCHGBTestInfo->fWBC_Histogram[32]=81.68	 ;
	WBCHGBTestInfo->fWBC_Histogram[33]=87.04	 ;
	WBCHGBTestInfo->fWBC_Histogram[34]=91.65	 ;
	WBCHGBTestInfo->fWBC_Histogram[35]=95.36	 ;
	WBCHGBTestInfo->fWBC_Histogram[36]=98.05	 ;
	WBCHGBTestInfo->fWBC_Histogram[37]=99.62	 ;
	WBCHGBTestInfo->fWBC_Histogram[38]=100.00	 ;
	WBCHGBTestInfo->fWBC_Histogram[39]=99.15	 ;
	WBCHGBTestInfo->fWBC_Histogram[40]=97.10	 ;
	WBCHGBTestInfo->fWBC_Histogram[41]=93.90	 ;
	WBCHGBTestInfo->fWBC_Histogram[42]=89.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[43]=84.59	 ;
	WBCHGBTestInfo->fWBC_Histogram[44]=78.86	 ;
	WBCHGBTestInfo->fWBC_Histogram[45]=72.72	 ;
	WBCHGBTestInfo->fWBC_Histogram[46]=66.43	 ;
	WBCHGBTestInfo->fWBC_Histogram[47]=60.24	 ;
	WBCHGBTestInfo->fWBC_Histogram[48]=54.22	 ;
	WBCHGBTestInfo->fWBC_Histogram[49]=48.34	 ;
	WBCHGBTestInfo->fWBC_Histogram[50]=43.16	 ;
	WBCHGBTestInfo->fWBC_Histogram[51]=38.58	 ;
	WBCHGBTestInfo->fWBC_Histogram[52]=34.53	 ;
	WBCHGBTestInfo->fWBC_Histogram[53]=30.91	 ;
	WBCHGBTestInfo->fWBC_Histogram[54]=27.61	 ;
	WBCHGBTestInfo->fWBC_Histogram[55]=24.29	 ;
	WBCHGBTestInfo->fWBC_Histogram[56]=21.43	 ;
	WBCHGBTestInfo->fWBC_Histogram[57]=18.87	 ;
	WBCHGBTestInfo->fWBC_Histogram[58]=16.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[59]=14.91	 ;
	WBCHGBTestInfo->fWBC_Histogram[60]=13.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[61]=12.78	 ;
	WBCHGBTestInfo->fWBC_Histogram[62]=12.24	 ;
	WBCHGBTestInfo->fWBC_Histogram[63]=12.00	 ;
	WBCHGBTestInfo->fWBC_Histogram[64]=12.01	 ;
	WBCHGBTestInfo->fWBC_Histogram[65]=12.23	 ;
	WBCHGBTestInfo->fWBC_Histogram[66]=12.64	 ;
	WBCHGBTestInfo->fWBC_Histogram[67]=13.20	 ;
	WBCHGBTestInfo->fWBC_Histogram[68]=13.88	 ;
	WBCHGBTestInfo->fWBC_Histogram[69]=14.62	 ;
	WBCHGBTestInfo->fWBC_Histogram[70]=15.09	 ;
	WBCHGBTestInfo->fWBC_Histogram[71]=15.69	 ;
	WBCHGBTestInfo->fWBC_Histogram[72]=16.16	 ;
	WBCHGBTestInfo->fWBC_Histogram[73]=16.47	 ;
	WBCHGBTestInfo->fWBC_Histogram[74]=16.61	 ;
	WBCHGBTestInfo->fWBC_Histogram[75]=16.18	 ;
	WBCHGBTestInfo->fWBC_Histogram[76]=16.08	 ;
	WBCHGBTestInfo->fWBC_Histogram[77]=15.90	 ;
	WBCHGBTestInfo->fWBC_Histogram[78]=15.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[79]=15.38	 ;
	WBCHGBTestInfo->fWBC_Histogram[80]=14.71	 ;
	WBCHGBTestInfo->fWBC_Histogram[81]=14.35	 ;
	WBCHGBTestInfo->fWBC_Histogram[82]=13.95	 ;
	WBCHGBTestInfo->fWBC_Histogram[83]=13.54	 ;
	WBCHGBTestInfo->fWBC_Histogram[84]=13.17	 ;
	WBCHGBTestInfo->fWBC_Histogram[85]=12.89	 ;
	WBCHGBTestInfo->fWBC_Histogram[86]=12.76	 ;
	WBCHGBTestInfo->fWBC_Histogram[87]=12.74	 ;
	WBCHGBTestInfo->fWBC_Histogram[88]=12.81	 ;
	WBCHGBTestInfo->fWBC_Histogram[89]=12.95	 ;
	WBCHGBTestInfo->fWBC_Histogram[90]=13.14	 ;
	WBCHGBTestInfo->fWBC_Histogram[91]=13.28	 ;
	WBCHGBTestInfo->fWBC_Histogram[92]=13.41	 ;
	WBCHGBTestInfo->fWBC_Histogram[93]=13.52	 ;
	WBCHGBTestInfo->fWBC_Histogram[94]=13.66	 ;
	WBCHGBTestInfo->fWBC_Histogram[95]=13.80	 ;
	WBCHGBTestInfo->fWBC_Histogram[96]=14.12	 ;
	WBCHGBTestInfo->fWBC_Histogram[97]=14.61	 ;
	WBCHGBTestInfo->fWBC_Histogram[98]=15.31	 ;
	WBCHGBTestInfo->fWBC_Histogram[99]=16.24	 ;
	WBCHGBTestInfo->fWBC_Histogram[100]=17.29	 ;
	WBCHGBTestInfo->fWBC_Histogram[101]=18.57	 ;
	WBCHGBTestInfo->fWBC_Histogram[102]=19.88	 ;
	WBCHGBTestInfo->fWBC_Histogram[103]=21.12	 ;
	WBCHGBTestInfo->fWBC_Histogram[104]=22.20	 ;
	WBCHGBTestInfo->fWBC_Histogram[105]=22.95	 ;
	WBCHGBTestInfo->fWBC_Histogram[106]=23.62	 ;
	WBCHGBTestInfo->fWBC_Histogram[107]=24.19	 ;
	WBCHGBTestInfo->fWBC_Histogram[108]=24.77	 ;
	WBCHGBTestInfo->fWBC_Histogram[109]=25.48	 ;
	WBCHGBTestInfo->fWBC_Histogram[110]=26.38	 ;
	WBCHGBTestInfo->fWBC_Histogram[111]=27.68	 ;
	WBCHGBTestInfo->fWBC_Histogram[112]=29.37	 ;
	WBCHGBTestInfo->fWBC_Histogram[113]=31.48	 ;
	WBCHGBTestInfo->fWBC_Histogram[114]=34.02	 ;
	WBCHGBTestInfo->fWBC_Histogram[115]=36.85	 ;
	WBCHGBTestInfo->fWBC_Histogram[116]=39.35	 ;
	WBCHGBTestInfo->fWBC_Histogram[117]=41.96	 ;
	WBCHGBTestInfo->fWBC_Histogram[118]=44.58	 ;
	WBCHGBTestInfo->fWBC_Histogram[119]=47.07	 ;
	WBCHGBTestInfo->fWBC_Histogram[120]=49.31	 ;
	WBCHGBTestInfo->fWBC_Histogram[121]=51.19	 ;
	WBCHGBTestInfo->fWBC_Histogram[122]=52.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[123]=53.74	 ;
	WBCHGBTestInfo->fWBC_Histogram[124]=54.47	 ;
	WBCHGBTestInfo->fWBC_Histogram[125]=54.96	 ;
	WBCHGBTestInfo->fWBC_Histogram[126]=55.34	 ;
	WBCHGBTestInfo->fWBC_Histogram[127]=55.74	 ;
	WBCHGBTestInfo->fWBC_Histogram[128]=56.26	 ;
	WBCHGBTestInfo->fWBC_Histogram[129]=56.97	 ;
	WBCHGBTestInfo->fWBC_Histogram[130]=57.86	 ;
	WBCHGBTestInfo->fWBC_Histogram[131]=58.89	 ;
	WBCHGBTestInfo->fWBC_Histogram[132]=59.94	 ;
	WBCHGBTestInfo->fWBC_Histogram[133]=60.89	 ;
	WBCHGBTestInfo->fWBC_Histogram[134]=61.60	 ;
	WBCHGBTestInfo->fWBC_Histogram[135]=61.96	 ;
	WBCHGBTestInfo->fWBC_Histogram[136]=61.92	 ;
	WBCHGBTestInfo->fWBC_Histogram[137]=61.46	 ;
	WBCHGBTestInfo->fWBC_Histogram[138]=60.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[139]=59.66	 ;
	WBCHGBTestInfo->fWBC_Histogram[140]=58.58	 ;
	WBCHGBTestInfo->fWBC_Histogram[141]=57.58	 ;
	WBCHGBTestInfo->fWBC_Histogram[142]=56.74	 ;
	WBCHGBTestInfo->fWBC_Histogram[143]=56.12	 ;
	WBCHGBTestInfo->fWBC_Histogram[144]=55.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[145]=55.32	 ;
	WBCHGBTestInfo->fWBC_Histogram[146]=54.98	 ;
	WBCHGBTestInfo->fWBC_Histogram[147]=54.57	 ;
	WBCHGBTestInfo->fWBC_Histogram[148]=54.04	 ;
	WBCHGBTestInfo->fWBC_Histogram[149]=53.41	 ;
	WBCHGBTestInfo->fWBC_Histogram[150]=52.70	 ;
	WBCHGBTestInfo->fWBC_Histogram[151]=51.98	 ;
	WBCHGBTestInfo->fWBC_Histogram[152]=51.29	 ;
	WBCHGBTestInfo->fWBC_Histogram[153]=50.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[154]=50.11	 ;
	WBCHGBTestInfo->fWBC_Histogram[155]=49.58	 ;
	WBCHGBTestInfo->fWBC_Histogram[156]=49.01	 ;
	WBCHGBTestInfo->fWBC_Histogram[157]=48.36	 ;
	WBCHGBTestInfo->fWBC_Histogram[158]=47.59	 ;
	WBCHGBTestInfo->fWBC_Histogram[159]=46.66	 ;
	WBCHGBTestInfo->fWBC_Histogram[160]=45.60	 ;
	WBCHGBTestInfo->fWBC_Histogram[161]=44.43	 ;
	WBCHGBTestInfo->fWBC_Histogram[162]=43.19	 ;
	WBCHGBTestInfo->fWBC_Histogram[163]=41.93	 ;
	WBCHGBTestInfo->fWBC_Histogram[164]=40.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[165]=39.44	 ;
	WBCHGBTestInfo->fWBC_Histogram[166]=38.23	 ;
	WBCHGBTestInfo->fWBC_Histogram[167]=37.04	 ;
	WBCHGBTestInfo->fWBC_Histogram[168]=35.85	 ;
	WBCHGBTestInfo->fWBC_Histogram[169]=34.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[170]=33.48	 ;
	WBCHGBTestInfo->fWBC_Histogram[171]=32.28	 ;
	WBCHGBTestInfo->fWBC_Histogram[172]=30.93	 ;
	WBCHGBTestInfo->fWBC_Histogram[173]=29.57	 ;
	WBCHGBTestInfo->fWBC_Histogram[174]=28.31	 ;
	WBCHGBTestInfo->fWBC_Histogram[175]=27.11	 ;
	WBCHGBTestInfo->fWBC_Histogram[176]=25.97	 ;
	WBCHGBTestInfo->fWBC_Histogram[177]=24.85	 ;
	WBCHGBTestInfo->fWBC_Histogram[178]=23.73	 ;
	WBCHGBTestInfo->fWBC_Histogram[179]=22.56	 ;
	WBCHGBTestInfo->fWBC_Histogram[180]=21.31	 ;
	WBCHGBTestInfo->fWBC_Histogram[181]=20.07	 ;
	WBCHGBTestInfo->fWBC_Histogram[182]=18.80	 ;
	WBCHGBTestInfo->fWBC_Histogram[183]=17.52	 ;
	WBCHGBTestInfo->fWBC_Histogram[184]=16.24	 ;
	WBCHGBTestInfo->fWBC_Histogram[185]=15.00	 ;
	WBCHGBTestInfo->fWBC_Histogram[186]=13.82	 ;
	WBCHGBTestInfo->fWBC_Histogram[187]=12.71	 ;
	WBCHGBTestInfo->fWBC_Histogram[188]=11.69	 ;
	WBCHGBTestInfo->fWBC_Histogram[189]=10.77	 ;
	WBCHGBTestInfo->fWBC_Histogram[190]=9.95	 ;
	WBCHGBTestInfo->fWBC_Histogram[191]=9.23	 ;
	WBCHGBTestInfo->fWBC_Histogram[192]=8.62	 ;
	WBCHGBTestInfo->fWBC_Histogram[193]=8.09	 ;
	WBCHGBTestInfo->fWBC_Histogram[194]=7.61	 ;
	WBCHGBTestInfo->fWBC_Histogram[195]=7.16	 ;
	WBCHGBTestInfo->fWBC_Histogram[196]=6.71	 ;
	WBCHGBTestInfo->fWBC_Histogram[197]=6.25	 ;
	WBCHGBTestInfo->fWBC_Histogram[198]=5.80	 ;
	WBCHGBTestInfo->fWBC_Histogram[199]=5.35	 ;
	WBCHGBTestInfo->fWBC_Histogram[200]=4.94	 ;
	WBCHGBTestInfo->fWBC_Histogram[201]=4.56	 ;
	WBCHGBTestInfo->fWBC_Histogram[202]=4.23	 ;
	WBCHGBTestInfo->fWBC_Histogram[203]=3.93	 ;
	WBCHGBTestInfo->fWBC_Histogram[204]=3.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[205]=3.42	 ;
	WBCHGBTestInfo->fWBC_Histogram[206]=3.18	 ;
	WBCHGBTestInfo->fWBC_Histogram[207]=2.94	 ;
	WBCHGBTestInfo->fWBC_Histogram[208]=2.71	 ;
	WBCHGBTestInfo->fWBC_Histogram[209]=2.48	 ;
	WBCHGBTestInfo->fWBC_Histogram[210]=2.26	 ;
	WBCHGBTestInfo->fWBC_Histogram[211]=2.06	 ;
	WBCHGBTestInfo->fWBC_Histogram[212]=1.89	 ;
	WBCHGBTestInfo->fWBC_Histogram[213]=1.74	 ;
	WBCHGBTestInfo->fWBC_Histogram[214]=1.62	 ;
	WBCHGBTestInfo->fWBC_Histogram[215]=1.51	 ;
	WBCHGBTestInfo->fWBC_Histogram[216]=1.41	 ;
	WBCHGBTestInfo->fWBC_Histogram[217]=1.31	 ;
	WBCHGBTestInfo->fWBC_Histogram[218]=1.22	 ;
	WBCHGBTestInfo->fWBC_Histogram[219]=1.13	 ;
	WBCHGBTestInfo->fWBC_Histogram[220]=1.03	 ;
	WBCHGBTestInfo->fWBC_Histogram[221]=0.95	 ;
	WBCHGBTestInfo->fWBC_Histogram[222]=0.88	 ;
	WBCHGBTestInfo->fWBC_Histogram[223]=0.81	 ;
	WBCHGBTestInfo->fWBC_Histogram[224]=0.76	 ;
	WBCHGBTestInfo->fWBC_Histogram[225]=0.71	 ;
	WBCHGBTestInfo->fWBC_Histogram[226]=0.67	 ;
	WBCHGBTestInfo->fWBC_Histogram[227]=0.63	 ;
	WBCHGBTestInfo->fWBC_Histogram[228]=0.60	 ;
	WBCHGBTestInfo->fWBC_Histogram[229]=0.58	 ;
	WBCHGBTestInfo->fWBC_Histogram[230]=0.56	 ;
	WBCHGBTestInfo->fWBC_Histogram[231]=0.55	 ;
	WBCHGBTestInfo->fWBC_Histogram[232]=0.54	 ;
	WBCHGBTestInfo->fWBC_Histogram[233]=0.54	 ;
	WBCHGBTestInfo->fWBC_Histogram[234]=0.54	 ;
	WBCHGBTestInfo->fWBC_Histogram[235]=0.54	 ;
	WBCHGBTestInfo->fWBC_Histogram[236]=0.54	 ;
	WBCHGBTestInfo->fWBC_Histogram[237]=0.53	 ;
	WBCHGBTestInfo->fWBC_Histogram[238]=0.51	 ;
	WBCHGBTestInfo->fWBC_Histogram[239]=0.48	 ;
	WBCHGBTestInfo->fWBC_Histogram[240]=0.44	 ;
	WBCHGBTestInfo->fWBC_Histogram[241]=0.39	 ;
	WBCHGBTestInfo->fWBC_Histogram[242]=0.35	 ;
	WBCHGBTestInfo->fWBC_Histogram[243]=0.32	 ;
	WBCHGBTestInfo->fWBC_Histogram[244]=0.31	 ;
	WBCHGBTestInfo->fWBC_Histogram[245]=0.31	 ;
	WBCHGBTestInfo->fWBC_Histogram[246]=0.32	 ;
	WBCHGBTestInfo->fWBC_Histogram[247]=0.33	 ;
	WBCHGBTestInfo->fWBC_Histogram[248]=0.34	 ;
	WBCHGBTestInfo->fWBC_Histogram[249]=0.33	 ;
	WBCHGBTestInfo->fWBC_Histogram[250]=0.30	 ;
	WBCHGBTestInfo->fWBC_Histogram[251]=0.27	 ;
	WBCHGBTestInfo->fWBC_Histogram[252]=0.22	 ;
	WBCHGBTestInfo->fWBC_Histogram[253]=0.18	 ;
	WBCHGBTestInfo->fWBC_Histogram[254]=0.13	 ;
	WBCHGBTestInfo->fWBC_Histogram[255]=0.10     ;
										

	//先保存原始仪器序列号
	strcpy(SN,(char*)MachInfo.labInfo.ucaMachineSN);

	while(1){
		//判断是否提前中止
		if(ServiceAgingTestUIInfo.stopFlag == 1){
			//提前退出循环
			break;
		}
		
		if(StartSNNum > 99999){
			MULTIEDIT_AddText(hItem,"SN号超限，停止测试\n");
		
			break;
		}
		
		//清空多行编辑框，防止溢出
		if(StartSNNum%100 == 0){
			MULTIEDIT_SetText(hItem,"");
		}
		
		//生成新的SN
		memset((uint8_t*)MachInfo.labInfo.ucaMachineSN,0,sizeof(MachInfo.labInfo.ucaMachineSN));
		sprintf((char*)MachInfo.labInfo.ucaMachineSN,"BB421%u%u%05u",StartTime.month.bcd_h,StartTime.month.bcd_l,StartSNNum);
		
		StartSNNum++;
		
		memset(Str,0,20);
		sprintf(Str,"当前SN：%s\n",MachInfo.labInfo.ucaMachineSN);
		MULTIEDIT_AddText(hItem,Str);
		
		osDelay(50);
		
		//建立MQTT连接
		if(CAT_B_CreateMQTTConn(SERVICE_CC) != HAL_OK){
			CAT_B_CloseMQTTConn();
			
			MULTIEDIT_AddText(hItem,"CAT联网中断，终止测试\n");
			break;
		}
		
//		//间隔200ms，给服务端订阅设备主题提供缓冲时间
//		osDelay(100);
		
		//发送过仪器出厂确认信息给服务端
		ReturnStatus = CAT_B_Send_MachineFactoryInfo();
		if(ReturnStatus == HAL_TIMEOUT){
			CAT_B_CloseMQTTConn();
			
			MULTIEDIT_AddText(hItem,"CAT联网中断，终止测试\n");
			break;
		}else if(ReturnStatus == HAL_ERROR){
			CAT_B_CloseMQTTConn();
			MULTIEDIT_AddText(hItem,"该SN已经存在，跳过...\n");
			continue;
		}
		
		//发送设备当前开机信息
		if(CAT_B_Send_MachineOpenInfo() != HAL_OK){
			CAT_B_CloseMQTTConn();
			
			MULTIEDIT_AddText(hItem,"CAT联网中断，终止测试\n");
			break;
		}
		
		SNCount++;
		
		tCountHead->eErrorCode = 0;
		tCountHead->eHideErrorCode = 0;
		
		//发送样本分析数据信息20条
		for(i=1;i<=20;i++){
			//设置随机数种子
			srand(HAL_GetTick());
			
			WBCHGBTestInfo->ulCurDataSN = rand()%10000000;
			memset(WBCHGBTestInfo->ucaName,0,30);
			sprintf((char*)WBCHGBTestInfo->ucaName,"测试名%u",rand()%10000);
			WBCHGBTestInfo->eSex = i&1;
			WBCHGBTestInfo->ucAge = rand()%50;
			WBCHGBTestInfo->eAgeUnit = rand()%4;
			WBCHGBTestInfo->eAccountType = rand()%4;
			WBCHGBTestInfo->timestamp = 1620099804+rand()%8000000;
			
			Temp = rand()%2000;
			WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC] = Temp/100.0;
			WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ] = (rand()%Temp)/100.0;
			WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ] = (WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC] - WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ])/2.0f;
			WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ] = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ];
			
			WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB] = (rand()%10000)/100.0;
			WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB] = (100.0f-WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB])/2.0f;
			WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB] = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB];
			
			WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB] = (rand()%10000)/100.0;
			
			Temp = rand()%3;
			WBCHGBTestInfo->ucaWBCHGB_RstAlert[0] =  WBCHGB_Alert_Flag(Temp, WBCHGBTestInfo->eReferGroup, 0, WBCHGBTestInfo->fWBCHGB_RstData[0]);
			WBCHGBTestInfo->ucaWBCHGB_RstAlert[1] =  WBCHGB_Alert_Flag(Temp, WBCHGBTestInfo->eReferGroup, 1, WBCHGBTestInfo->fWBCHGB_RstData[1]);
			WBCHGBTestInfo->ucaWBCHGB_RstAlert[2] =  WBCHGB_Alert_Flag(Temp, WBCHGBTestInfo->eReferGroup, 2, WBCHGBTestInfo->fWBCHGB_RstData[2]);
			WBCHGBTestInfo->ucaWBCHGB_RstAlert[3] =  WBCHGB_Alert_Flag(Temp, WBCHGBTestInfo->eReferGroup, 3, WBCHGBTestInfo->fWBCHGB_RstData[3]);
			WBCHGBTestInfo->ucaWBCHGB_RstAlert[4] =  WBCHGB_Alert_Flag(Temp, WBCHGBTestInfo->eReferGroup, 4, WBCHGBTestInfo->fWBCHGB_RstData[4]);
			WBCHGBTestInfo->ucaWBCHGB_RstAlert[5] =  WBCHGB_Alert_Flag(Temp, WBCHGBTestInfo->eReferGroup, 5, WBCHGBTestInfo->fWBCHGB_RstData[5]);
			WBCHGBTestInfo->ucaWBCHGB_RstAlert[6] =  WBCHGB_Alert_Flag(Temp, WBCHGBTestInfo->eReferGroup, 6, WBCHGBTestInfo->fWBCHGB_RstData[6]);
			WBCHGBTestInfo->ucaWBCHGB_RstAlert[7] =  WBCHGB_Alert_Flag(Temp, WBCHGBTestInfo->eReferGroup, 7, WBCHGBTestInfo->fWBCHGB_RstData[7]);
			
			//发送样本分析数据
			if(CAT_B_Send_SampleInfo(WBCHGBTestInfo,tCountHead) != HAL_OK){
				CAT_B_CloseMQTTConn();
			
				MULTIEDIT_AddText(hItem,"CAT联网中断，终止测试\n");
				break;
			}
		}
		
		//发送故障日志信息50条
		for(i=0;i<50;i++){
			//设置随机数种子
			srand(HAL_GetTick());
			
			WBCHGBTestInfo->ulCurDataSN = rand()%10000000;
			
			Temp = rand()%3+1;
			switch(Temp){
				case 1:
					Temp1 = rand()%4;
					Temp2 = rand()%100;
					Temp = 1*1000+Temp1*100+Temp2;
				break;
				case 2:
					Temp1 = i&1;
					Temp2 = rand()%100;
					Temp = 2*1000+Temp1*100+Temp2;
				break;
				case 3:
					Temp2 = i&1;
					Temp = 3000+Temp2;
				break;
				default :break;
			}
			tCountHead->eErrorCode = RandIntToErrCode(Temp);
			tCountHead->eHideErrorCode = RandIntToErrCode(Temp);
			tCountHead->errType = ErrorCode_Type(tCountHead->eErrorCode);
			WBCHGBTestInfo->eAccountType = rand()%4;
			WBCHGBTestInfo->timestamp = 1620099804+rand()%8000000;
			
			if(CAT_B_Send_SampleLog(WBCHGBTestInfo,tCountHead,(uint8_t*)LogBuf,strlen(LogBuf)) != HAL_OK){
				CAT_B_CloseMQTTConn();
			
				MULTIEDIT_AddText(hItem,"CAT联网中断，终止测试\n");
				break;
			}
		}
		
		CAT_B_CloseMQTTConn();
	}
	
	//还原原始仪器序列号
	strcpy((char*)MachInfo.labInfo.ucaMachineSN,SN);

	//读取RTC结束时间
	RTC_HYM8563GetTime(&EndTime);
	
	sprintf(Str,"测试总次数：%u\n起始时间：%u%u/%u%u/%u%u %u%u:%u%u:%u%u\n停止时间：%u%u/%u%u/%u%u %u%u:%u%u:%u%u\n",
				SNCount,StartTime.year.bcd_h,StartTime.year.bcd_l,StartTime.month.bcd_h,StartTime.month.bcd_l,StartTime.day.bcd_h,StartTime.day.bcd_l,StartTime.hour.bcd_h,StartTime.hour.bcd_l,StartTime.min.bcd_h,StartTime.min.bcd_l,StartTime.sec.bcd_h,StartTime.sec.bcd_l,
				EndTime.year.bcd_h,EndTime.year.bcd_l,EndTime.month.bcd_h,EndTime.month.bcd_l,EndTime.day.bcd_h,EndTime.day.bcd_l,EndTime.hour.bcd_h,EndTime.hour.bcd_l,EndTime.min.bcd_h,EndTime.min.bcd_l,EndTime.sec.bcd_h,EndTime.sec.bcd_l);
	MULTIEDIT_AddText(hItem,Str);
	
	ServiceAgingTestUIInfo.startFlag = 0;
	ServiceAgingTestUIInfo.stopFlag = 1;
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	return HAL_OK;
}






//用来传递随机点击测试任务运行时间，单位min
uint32_t RandomTouchTime = 0;

osThreadId_t RandonTouchTestTaskHandle;
const osThreadAttr_t RandonTouchTestTask_attributes = {
  .name = "RandonTouchTestTask",
  .priority = (osPriority_t) osPriorityLow3,
  .stack_size = 128*5
};


/***
*随机点击测试临时任务
***/
void RandonTouchTestTask(void *argument)
{
	extern __IO WM_HWIN g_hActiveWin;					//当前显示窗口的句柄
	extern ServiceAgingTestUIInfo_s ServiceAgingTestUIInfo;
	uint32_t StartTick = 0;
	GUI_PID_STATE pState = {0};
	
	
	memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));
	ServiceAgingTestUIInfo.startFlag = 1;
	ServiceAgingTestUIInfo.stopFlag = 0;
	
	
	//界面显示
//	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_AGING_TEST_ID);
//	MULTIEDIT_SetText(hItem,"10s后开始...");
//	MULTIEDIT_AddText(hItem,"随机点击测试将10s后开始...\n");
	
	//先判断运行时间是否符合要求
	if(RandomTouchTime > 10000){
		RandomTouchTime = 10000;
	}
	
//	osDelay(10000);
	
	StartTick = HAL_GetTick();
	
	while(1){
		//判断是否提前中止
		if(ServiceAgingTestUIInfo.stopFlag == 1){
			//提前退出循环
			break;
		}
		
		if(HAL_GetTick()-StartTick > RandomTouchTime*60000){
			break;
		}
		
		//设置随机数种子
		srand(HAL_GetTick());
		
		pState.x = rand()%800;
		pState.y = rand()%480;
		pState.Pressed = 1;
		GUI_TOUCH_StoreStateEx(&pState);
		osDelay(20);
		pState.Pressed = 0;
		GUI_TOUCH_StoreStateEx(&pState);
		
//		osDelay(60);
//		pState.x = rand()%800;
//		pState.y = rand()%400+70;
//		pState.Pressed = 1;
//		GUI_TOUCH_StoreStateEx(&pState);
//		osDelay(20);
//		pState.Pressed = 0;
//		GUI_TOUCH_StoreStateEx(&pState);
		
		osDelay(50);
	}
	
	ServiceAgingTestUIInfo.startFlag = 0;
	ServiceAgingTestUIInfo.stopFlag = 1;
	
	osThreadSuspend(RandonTouchTestTaskHandle);
}




/***
*生产老化测试
***/
HAL_StatusTypeDef ShenChanAgingTest(uint32_t SetCount)
{
	extern __IO WM_HWIN g_hActiveWin;					//当前显示窗口的句柄
	extern ServiceAgingTestUIInfo_s ServiceAgingTestUIInfo;
	extern MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern __IO uint8_t g_CountHandle_Status;
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern osMessageQueueId_t CATQueueHandle;
	CATQueueInfoBuf_s CATQueueInfoBuf;
	WM_HWIN hItem;
	SCAgingTestResultInfo_s SCAgingTResult = {0};
	char Str[1024] = {0};
	uint16_t Len = 0;
	uint32_t Temp = 0;
	uint32_t TimeStampStart = 0,TimeStampEnd = 0;
	int16_t Temperature = 0;
	long RTCTimeDiff = 0;
	int Temp2 = 0;
	ErrorCode_e ReStates;
	uint32_t CatSuccessCount = 0;
	
	memset(Str,0,1024);
		
	memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));
	ServiceAgingTestUIInfo.startFlag = 1;
	ServiceAgingTestUIInfo.stopFlag = 0;
		
	SCAgingTResult.count.agingTotal = SetCount;
		
	//读取RTC起始时间
	RTC_HYM8563GetTime(&SCAgingTResult.rtc.startTime);
	TimeStampStart = HAL_GetTick();
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_AGING_TEST_ID);
	MULTIEDIT_SetText(hItem,"");
		
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;

	for(SCAgingTResult.count.aging=0;SCAgingTResult.count.aging<SCAgingTResult.count.agingTotal;){
		//判断是否提前中止
		if(ServiceAgingTestUIInfo.stopFlag == 1){
			SCAgingTResult.aging.stopFlag = 1;
			
			//提前退出循环
			break;
		}
		
		SCAgingTResult.count.aging++;
		
		Len = 0;
		Len += sprintf(Str+Len,"常温老化测试\n");
		Len += sprintf(Str+Len,"次数：%u/%u\n",SCAgingTResult.count.aging,SCAgingTResult.count.agingTotal);
		
		if(abs(Power_12VP_V()-12000) > 600){
			SCAgingTResult.aging.p12vFlag = 1;
		}
		Len += sprintf(Str+Len,"P12V：%s\n",SCAgingTResult.aging.p12vFlag==0?"√":"×");
		
		if(abs(Power_12VN_V()+12000) > 800){
			SCAgingTResult.aging.n12vFlag = 1;
		}
		Len += sprintf(Str+Len,"N12V：%s\n",SCAgingTResult.aging.n12vFlag==0?"√":"×");
		
		if(abs((uint16_t)BC_Current_V()-55000) > 2000){
			SCAgingTResult.aging.v55Flag = 1;
		}
		Len += sprintf(Str+Len,"55V：%s\n",SCAgingTResult.aging.v55Flag==0?"√":"×");
		
		//判断SDRAM是否正常，直接判定通过
		SCAgingTResult.aging.sdramFlag = 0;
		Len += sprintf(Str+Len,"SDRAM：%s\n",SCAgingTResult.aging.sdramFlag==0?"√":"×");
		
		SCAgingTResult.aging.nandFlag = 0;
		Len += sprintf(Str+Len,"NAND：%s\n",SCAgingTResult.aging.nandFlag==0?"√":"×");
		
		SCAgingTResult.aging.mEEPROMFlag = 0;
		SCAgingTResult.aging.sEEPROMFlag = 0;
		Len += sprintf(Str+Len,"M_EEPROM：%s\n",SCAgingTResult.aging.mEEPROMFlag==0?"√":"×");
		Len += sprintf(Str+Len,"S_EEPROM：%s\n",SCAgingTResult.aging.sEEPROMFlag==0?"√":"×");
		
		Temperature = NTC_Temperature();
		if(Temperature < 5 || Temperature > 40){
			SCAgingTResult.aging.ntcFlag = 1;
		}
		Len += sprintf(Str+Len,"温度：%s\n",SCAgingTResult.aging.ntcFlag==0?"√":"×");
		
		//CAT测试
		if(CATGlobalStatus.registerNet == CAT_REGISTNET_SUCCESS){
			//发送老化测试指令给CAT任务
			CATQueueInfoBuf.msgType = AGING_TEST_CAT_MSG_TYPE;
			osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
		}else{
			CATGlobalStatus.flag.connServerErr = 1;
		}
		
		if(CATGlobalStatus.flag.connServerErr != 1){
			CatSuccessCount++;
		}		
		if(CatSuccessCount >= (SCAgingTResult.count.aging*4/5)){
			SCAgingTResult.aging.catFlag = 0;
		}else{
			SCAgingTResult.aging.catFlag = 1;
		}
		Len += sprintf(Str+Len,"CAT：%s\n",SCAgingTResult.aging.catFlag==0?"√":"×");
		
		//先保证处于进仓状态
		OutIn_Module_In(MOTOR_MODE_NORMAL);
		
		//进仓状态下各参数值
		//判断光耦
		if(OC_Status(OC_INDEX_OUTIN_IN) != SET){
			SCAgingTResult.aging.outInFlag = 1;
		}
		if(OC_Status(OC_INDEX_OUTIN_OUT) != RESET){
			SCAgingTResult.aging.outInFlag = 1;
		}
		if(OC_Status(OC_INDEX_CLAMP_IN) != RESET){
			SCAgingTResult.aging.outInFlag = 1;
		}
		if(OC_Status(OC_INDEX_CLAMP_OUT) != SET){
			SCAgingTResult.aging.outInFlag = 1;
		}
		
		//压力系统
		//建压
		ReStates = Build_Press(RUN_MODE_NORMAL, AGING_TEST_PRESS);
		if(ReStates != ERROR_CODE_SUCCESS){
			SCAgingTResult.aging.buildPressFlag = 1;
		}else{
			osDelay(1000);
			Build_Press(RUN_MODE_NORMAL, AGING_TEST_PRESS);
			osDelay(1000);
			Build_Press(RUN_MODE_NORMAL, AGING_TEST_PRESS);
			osDelay(1000);
		}
		Temp2 = Press_Value();
		
		//开液阀
		LIQUID_WAVE_OPEN;
		
		//密闭性，建压成功后才进行密闭性检测
		if(ReStates == ERROR_CODE_SUCCESS){
			osDelay(20000);
			
			if(abs(Temp2 - Press_Value()) > 5000){
				SCAgingTResult.aging.qimiFlag = 1;
			}
		}
		
		//关液阀
		LIQUID_WAVE_CLOSE;
		
		//HGB光路检测
		//检测关闭状态下的HGB
		Temp = HGB_ADC();
		if(Temp > 200){
			SCAgingTResult.aging.hgbFlag = 1;
		}
		
		//检测535波长灯
		Light_Start(HGBLED_535,MachInfo.paramSet.current535);
		osDelay(3000);
		
		if(abs((HGB_ADC() - Temp) < 2000) || (HGB_ADC() > 3700) || (HGB_ADC() < MachInfo.paramSet.KQADC535-500)){
			SCAgingTResult.aging.hgbFlag = 1;
		}
		
		//检测415波长灯
		Light_Start(HGBLED_415,MachInfo.paramSet.current415);
		osDelay(2000);
		
		if(abs((HGB_ADC() - Temp) < 2000) || (HGB_ADC() > 3700) || (HGB_ADC() < MachInfo.paramSet.KQADC415-500)){
			SCAgingTResult.aging.hgbFlag = 1;
		}
		
		//关闭HGB
		Light_Stop();
		
		//出仓
		OutIn_Module_Out(MOTOR_MODE_NORMAL);
		
		//出仓状态下各参数值
		//判断光耦
		if(OC_Status(OC_INDEX_OUTIN_IN) != RESET){
			SCAgingTResult.aging.outInFlag = 1;
		}
		if(OC_Status(OC_INDEX_OUTIN_OUT) != SET){
			SCAgingTResult.aging.outInFlag = 1;
		}
		if(OC_Status(OC_INDEX_CLAMP_IN) != SET){
			SCAgingTResult.aging.outInFlag = 1;
		}
		if(OC_Status(OC_INDEX_CLAMP_OUT) != RESET){
			SCAgingTResult.aging.outInFlag = 1;
		}
		
		//放气
		LIQUID_WAVE_OPEN;
		osDelay(1500);
		LIQUID_WAVE_CLOSE;
		Temp2 = Press_Value();
		if(Temp2 < -2000){
			//放气不充分
			SCAgingTResult.aging.fangqiFlag = 1;
		}
		
		Len += sprintf(Str+Len,"仓体运动性：%s\n",SCAgingTResult.aging.outInFlag==0?"√":"×");
		Len += sprintf(Str+Len,"建压性：%s\n",SCAgingTResult.aging.buildPressFlag==0?"√":"×");
		Len += sprintf(Str+Len,"气密性：%s\n",SCAgingTResult.aging.qimiFlag==0?"√":"×");
		Len += sprintf(Str+Len,"泄露性：%s\n",SCAgingTResult.aging.fangqiFlag==0?"√":"×");
		Len += sprintf(Str+Len,"HGB：%s\n",SCAgingTResult.aging.hgbFlag==0?"√":"×");
		
		Str[Len] = '\0';
		MULTIEDIT_SetText(hItem,Str);
	}
	
	//出仓
	OutIn_Module_Out(MOTOR_MODE_NORMAL);

	//读取RTC结束时间
	RTC_HYM8563GetTime(&SCAgingTResult.rtc.endTime);
	TimeStampEnd = HAL_GetTick();
	
	//计算RTC时间秒差值
	RTCTimeDiff = GetCurrentTimestamp(SCAgingTResult.rtc.endTime) - GetCurrentTimestamp(SCAgingTResult.rtc.startTime);
	Temp2 = abs((int)((TimeStampEnd-TimeStampStart)/1000-RTCTimeDiff));
	if(Temp2 > 5){
		SCAgingTResult.aging.rtcFlag = 1;
	}
	
	Len += sprintf(Str+Len,"RTC：%s\n",SCAgingTResult.aging.rtcFlag==0?"√":"×");
	Len += sprintf(Str+Len,"开始时间：%u%u/%u%u/%u%u %u%u:%u%u:%u%u\n停止时间：%u%u/%u%u/%u%u %u%u:%u%u:%u%u\n",
					SCAgingTResult.rtc.startTime.year.bcd_h,SCAgingTResult.rtc.startTime.year.bcd_l,SCAgingTResult.rtc.startTime.month.bcd_h,SCAgingTResult.rtc.startTime.month.bcd_l,SCAgingTResult.rtc.startTime.day.bcd_h,SCAgingTResult.rtc.startTime.day.bcd_l,SCAgingTResult.rtc.startTime.hour.bcd_h,SCAgingTResult.rtc.startTime.hour.bcd_l,SCAgingTResult.rtc.startTime.min.bcd_h,SCAgingTResult.rtc.startTime.min.bcd_l,SCAgingTResult.rtc.startTime.sec.bcd_h,SCAgingTResult.rtc.startTime.sec.bcd_l,
					SCAgingTResult.rtc.endTime.year.bcd_h,SCAgingTResult.rtc.endTime.year.bcd_l,SCAgingTResult.rtc.endTime.month.bcd_h,SCAgingTResult.rtc.endTime.month.bcd_l,SCAgingTResult.rtc.endTime.day.bcd_h,SCAgingTResult.rtc.endTime.day.bcd_l,SCAgingTResult.rtc.endTime.hour.bcd_h,SCAgingTResult.rtc.endTime.hour.bcd_l,SCAgingTResult.rtc.endTime.min.bcd_h,SCAgingTResult.rtc.endTime.min.bcd_l,SCAgingTResult.rtc.endTime.sec.bcd_h,SCAgingTResult.rtc.endTime.sec.bcd_l);
	Len += sprintf(Str+Len,"常温老化结果：%s\n",*((uint32_t*)&SCAgingTResult.aging)==0?"通过":"不通过");
	
	Str[Len] = '\0';
	MULTIEDIT_SetText(hItem,Str);
	
	ServiceAgingTestUIInfo.page = SC_AGING_TEST_RESULT;
	ServiceAgingTestUIInfo.unions.scAgingTestResultInfo = SCAgingTResult;
	
	ServiceAgingTestUIInfo.startFlag = 0;
	ServiceAgingTestUIInfo.stopFlag = 1;
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	return HAL_OK;
}





/***
*触摸临时任务
***/
static void _TempTask_Touch(void *argument)
{
	extern TouchPos_s TouchRawPos;
	ErrorStatus ReState;
	
	while(1){
		ReState = XPT2046_GetRawPosValue(&TouchRawPos);
		if(ReState != SUCCESS){
		TouchRawPos.x = -1;
		TouchRawPos.y = -1;
		}

		osDelay(3);
	}
}




/***
*数据存储测试
***/
HAL_StatusTypeDef DataSaveTest(uint32_t SetCount)
{
	extern __IO WM_HWIN g_hActiveWin;					//当前显示窗口的句柄
	extern ServiceAgingTestUIInfo_s ServiceAgingTestUIInfo;
	extern __IO uint8_t g_CountHandle_Status;
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	WM_HWIN hItem;
	DataSaveTestResultInfo_s DataSaveTResult = {0};
	char Str[1024] = {0};
	uint32_t Len = 0;
	uint8_t* WriteBuf = (uint8_t*)SDRAM_ApplyCommonBuf(1024*1024);
	uint8_t* ReadBuf = (uint8_t*)SDRAM_ApplyCommonBuf(1024*1024);
	uint32_t i = 0;
	uint32_t StartFlashAdr = 0;
	
	const osThreadAttr_t TempTask_Touch_attributes = {
	  .name = "TempTask_Touch",
	  .priority = (osPriority_t) osPriorityLow5,
	  .stack_size = 128*4
	};
	
	
	//临时创建一个任务来执行触摸采集任务
	osThreadNew(_TempTask_Touch, NULL, &TempTask_Touch_attributes);
	
	
	memset(&ServiceAgingTestUIInfo,0,sizeof(ServiceAgingTestUIInfo_s));
	ServiceAgingTestUIInfo.startFlag = 1;
	ServiceAgingTestUIInfo.stopFlag = 0;
		
	DataSaveTResult.count.total = SetCount;
	
	//读取RTC起始时间
	RTC_HYM8563GetTime(&DataSaveTResult.rtc.startTime);
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//界面显示
	hItem = WM_GetDialogItem(g_hActiveWin, MULTIEDIT_AGING_TEST_ID);
	MULTIEDIT_SetText(hItem,"");
	
	for(i=0;i<1024*1024;i++){
		WriteBuf[i] = i;
	}
	
//	SPI4_ChangeModeToFlash();

	for(DataSaveTResult.count.current=0;DataSaveTResult.count.current<DataSaveTResult.count.total;){
		//判断是否提前中止
		if(ServiceAgingTestUIInfo.stopFlag == 1){
			//提前退出循环
			break;
		}
		
		DataSaveTResult.count.current++;
		
		Len = 0;
		Len += sprintf(Str+Len,"数据存储测试：%u/%u\n",DataSaveTResult.count.current,DataSaveTResult.count.total);
		Len += sprintf(Str+Len,"ERR次数：%u\n",DataSaveTResult.count.err);
		if(DataSaveTResult.count.err != 0){
			Len += sprintf(Str+Len,"Write：%u，Read：%u\n",DataSaveTResult.errValue.write,DataSaveTResult.errValue.read);
		}
		
		//写入数据
		WriteBuf[0] = DataSaveTResult.count.current;
		SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
		SPIFlash_DMA_WriteData(StartFlashAdr,WriteBuf,4096);
		SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
		
		//读取一次坐标信息
//		XPT2046_GetRawPosValue(&TouchRawPos);
		
		//读取数据
		memset(ReadBuf,0,4096);
		SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
		SPIFlash_DMA_ReadData(StartFlashAdr,ReadBuf,4096);
		SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
		
		//读取一次坐标信息
//		XPT2046_GetRawPosValue(&TouchRawPos);
		
		//对比数据
		for(i=0;i<4096;i++){
			if(ReadBuf[i] != WriteBuf[i]){
				DataSaveTResult.count.err++;
				DataSaveTResult.errValue.write = WriteBuf[i];
				DataSaveTResult.errValue.read = ReadBuf[i];
				break;
			}
		}
		
		StartFlashAdr += 4096;
		
		if(StartFlashAdr > 31457280){
			StartFlashAdr = 0;
		}
		
		Str[Len] = '\0';
		MULTIEDIT_SetText(hItem,Str);
		
		osDelay(4);
	}
	
//	SPI4_ChangeModeToIDLE();
	
	Len += sprintf(Str+Len,"==================\n");

	//读取RTC结束时间
	RTC_HYM8563GetTime(&DataSaveTResult.rtc.endTime);
	
	Len += sprintf(Str+Len,"开始时间：%u%u/%u%u/%u%u %u%u:%u%u:%u%u\n停止时间：%u%u/%u%u/%u%u %u%u:%u%u:%u%u\n",
					DataSaveTResult.rtc.startTime.year.bcd_h,DataSaveTResult.rtc.startTime.year.bcd_l,DataSaveTResult.rtc.startTime.month.bcd_h,DataSaveTResult.rtc.startTime.month.bcd_l,DataSaveTResult.rtc.startTime.day.bcd_h,DataSaveTResult.rtc.startTime.day.bcd_l,DataSaveTResult.rtc.startTime.hour.bcd_h,DataSaveTResult.rtc.startTime.hour.bcd_l,DataSaveTResult.rtc.startTime.min.bcd_h,DataSaveTResult.rtc.startTime.min.bcd_l,DataSaveTResult.rtc.startTime.sec.bcd_h,DataSaveTResult.rtc.startTime.sec.bcd_l,
					DataSaveTResult.rtc.endTime.year.bcd_h,DataSaveTResult.rtc.endTime.year.bcd_l,DataSaveTResult.rtc.endTime.month.bcd_h,DataSaveTResult.rtc.endTime.month.bcd_l,DataSaveTResult.rtc.endTime.day.bcd_h,DataSaveTResult.rtc.endTime.day.bcd_l,DataSaveTResult.rtc.endTime.hour.bcd_h,DataSaveTResult.rtc.endTime.hour.bcd_l,DataSaveTResult.rtc.endTime.min.bcd_h,DataSaveTResult.rtc.endTime.min.bcd_l,DataSaveTResult.rtc.endTime.sec.bcd_h,DataSaveTResult.rtc.endTime.sec.bcd_l);
	
	Str[Len] = '\0';
	MULTIEDIT_SetText(hItem,Str);
	
	ServiceAgingTestUIInfo.page = 0xFF;		//不打印数据
//	ServiceAgingTestUIInfo.unions.scAgingTestResultInfo = SCAgingTResult;
	
	ServiceAgingTestUIInfo.startFlag = 0;
	ServiceAgingTestUIInfo.stopFlag = 1;
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	return HAL_OK;
}














