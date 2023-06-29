#include "main.h"
#include "tim.h"
#include "bsp_pump.h"
#include "msg_def.h"
#include "bsp_outin.h"
#include "bsp_motor.h"
#include "bsp_press.h"
#include "msg_def.h"
#include "cmsis_os.h"
#include "bsp_eeprom.h"
#include "file_operate.h"
#include "parameter.h"
#include "crc16.h"

/*
* 泵初始化
*/
void Pump_Init(void)
{
  MX_TIM4_Init();
}  


/*
* 泵初开启
*/
void Pump_Start(void)
{
	extern  MachInfo_s MachInfo;
	
	Pump_SetRatio(MachInfo.paramSet.ucPump_Scale);  //当前默认为100
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
}

/*
* 泵初关闭
*/
void Pump_Stop(void)
{
  HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
}


/*
* 泵初设置占空比(0-100), ucRatio, 当前泵使用的是TIM4，驱动频率为800HZ，占空比宽度：6250
*/
#define PUMP_PWM_ZKB    6250
void Pump_SetRatio(uint16_t ucRatio) 
{
  TIM_OC_InitTypeDef sConfigOC = {0};
  uint8_t ucRatioTemp = ucRatio;
  uint16_t usPulse = 0;
  
  //获取占空比，数值
  if(ucRatioTemp > 100)
  {
    ucRatioTemp = 100;
  }
  usPulse = PUMP_PWM_ZKB*ucRatioTemp/100;

  //设置
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = usPulse;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  
  //HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
}
  
  

/*
* 泵自检, 开阀3s
*/
void Pump_SelfCheck(void)
{
    __IO uint32_t ulStartTick = 0, ulEndTick = 0;
    
    ulStartTick = HAL_GetTick();
    ulEndTick   = ulStartTick;
    
    Pump_Start();
    while(ulEndTick - ulStartTick > 3000) //3s
    {
        ulEndTick = HAL_GetTick();
    }
    Pump_Stop();
}




/***
*自适应泵占空比值
*返回值：返回HAL_OK时表示正常执行成功，返回HAL_ERROR时表示执行失败了
***/
HAL_StatusTypeDef ParaSet_AutoPumpAdjust(uint8_t* PumpResult)
{
    extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
	extern __IO MachRunPara_s MachRunPara;
	extern __IO OutIn_Module_Position_e g_eOutIn_Key;
	extern osSemaphoreId xSema_OutIn_Key;
	int32_t Temp;
	uint8_t PumpRaw = MachInfo.paramSet.ucPump_Scale;
	
	RTC_HYM8563Info_s RTCInfo = {0};
	
	//运行日志缓存区
	RunLog_s RunLog = {0};
	
    uint32_t BuildPresureTime = 0;
	uint8_t AutoPluseSetFlag = 0;					//为0表示自动适应占空比已经成功，为1表示失败
	uint8_t PluseAddDir = 0;						//用于表示当前是否需要增减占空比，为0时表示不需要，为1时表示需要增加，为2时表示减少
	
	
	AIR_WAVE_CLOSE;
	LIQUID_WAVE_CLOSE;
	Pump_Stop();
	
	//在进仓位置先出仓一段距离，然后放气，确保罐体中无压力，然后再进仓
	OutIn_Module_In(MOTOR_MODE_NORMAL);
	
	//出仓一段距离
	if(Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_OUTIN,MOTOR_DIR_OUT,8000) != FEED_BACK_SUCCESS){
		return HAL_ERROR;
	}
	if(Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP,MOTOR_DIR_IN,3500) != FEED_BACK_SUCCESS){
		return HAL_ERROR;
	}
	
	
	
	//开液阀,放气
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	//开始建压
	BuildPressTime(BC_TEST_PRESS_VALUE,&BuildPresureTime);
	
	if(BuildPresureTime < 4000){
		PluseAddDir = 2;
		AutoPluseSetFlag = 1;
	}else if(BuildPresureTime > 6000){
		PluseAddDir = 1;
		AutoPluseSetFlag = 1;
	}else{
		//已经满足要求了
		AutoPluseSetFlag = 0;
	}
	
	while(AutoPluseSetFlag != 0){
		if(PluseAddDir == 1){
			//增加占空比
			MachInfo.paramSet.ucPump_Scale += 10;
			
			//开液阀,放气
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;  	
			
			//建压
			BuildPressTime(BC_TEST_PRESS_VALUE,&BuildPresureTime);
			
			//判断当前建压时间是否已经满足要求了
			if((BuildPresureTime >= 4000) && (BuildPresureTime <= 6000)){
				AutoPluseSetFlag = 0;
				PluseAddDir = 0;
				break;
			}else if(BuildPresureTime < 4000){
				PluseAddDir = 2;
			}else{
				PluseAddDir = 1;
			}
			
			if(MachInfo.paramSet.ucPump_Scale >=100){
				MachInfo.paramSet.ucPump_Scale = 100;
				PluseAddDir = 0;
				break;
			}
		}else if(PluseAddDir == 2){
			//减小占空比
			MachInfo.paramSet.ucPump_Scale -= 10;
			
			//开液阀,放气
			LIQUID_WAVE_OPEN;
			osDelay(1500);
			LIQUID_WAVE_CLOSE;    	
			
			//建压
			BuildPressTime(BC_TEST_PRESS_VALUE,&BuildPresureTime);
			
			//判断当前建压时间是否已经满足要求了
			if((BuildPresureTime >= 4000) && (BuildPresureTime <= 6000)){
				AutoPluseSetFlag = 0;
				PluseAddDir = 0;
				break;
			}else if(BuildPresureTime < 4000){
				PluseAddDir = 2;
			}else{
				PluseAddDir = 1;
			}
			
			if(MachInfo.paramSet.ucPump_Scale <=10){
				MachInfo.paramSet.ucPump_Scale = 10;
				PluseAddDir = 0;
				break;
			}
		}
	}
	
	//开液阀,放气
	LIQUID_WAVE_OPEN;
	osDelay(1500);
	LIQUID_WAVE_CLOSE;
	
	//进出仓还原回默认位置
	OutIn_Module_In(MOTOR_MODE_NORMAL);
	
	*PumpResult = MachInfo.paramSet.ucPump_Scale;
	
	//判断自适应是否成功
	if(AutoPluseSetFlag == 0){
        
		RunLog.logHead.logLen += sprintf(RunLog.Str+RunLog.logHead.logLen,"Adaptive adjustment of pump is successful:\r\nPumpPluse:%u-->%u",PumpRaw,MachInfo.paramSet.ucPump_Scale);
		
		//保存运行日志
		RunLog.Str[RunLog.logHead.logLen] = '\0';
		RunLog.logHead.num = g_tDataManage.ulRunLog_SN;
		RunLog.logHead.eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
		RTC_HYM8563GetTime(&RTCInfo);
		sprintf(RunLog.logHead.dataTime,"20%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
						RTCInfo.year.bcd_h,RTCInfo.year.bcd_l,
						RTCInfo.month.bcd_h,RTCInfo.month.bcd_l,
						RTCInfo.day.bcd_h,RTCInfo.day.bcd_l,
						RTCInfo.hour.bcd_h,RTCInfo.hour.bcd_l,
						RTCInfo.min.bcd_h,RTCInfo.min.bcd_l,
						RTCInfo.sec.bcd_h,RTCInfo.sec.bcd_l);
		Temp = sprintf(RunLog.logHead.shortStr,"Parameters Change");
		RunLog.logHead.shortStr[Temp] = '\0';
		RunLog.logHead.timestamp = GetCurrentTimestamp(RTCInfo);
        RunLog.usCrc = CRC16((uint8_t*)&RunLog, sizeof(RunLog_s)-2);
        
        Update_RunLog_Tail();
		Fatfs_Write_RunLog(RunLog.logHead.num, &RunLog);
		
		g_tDataManage.ulRunLog_SN++;//g_tDataManage.lastRunLogNum++;
        Add_RunLog_Tail();
		Save_DataMange_Info(&g_tDataManage);
		
		//标记CAT必发项
		if(MachInfo.catMastSynServEvent.bit.sendRunLog != 1){
			MachInfo.catMastSynServEvent.bit.sendRunLog = 1;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
		}
		
		EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET,(uint8_t*)&MachInfo.paramSet,sizeof(MachInfo.paramSet));
		return HAL_OK;
	}else{
		RunLog.logHead.logLen += sprintf(RunLog.Str+RunLog.logHead.logLen,"Adaptive adjustment of pump is failed:\r\nOld PumpPluse:%u",PumpRaw);
		
		//保存运行日志
		RunLog.Str[RunLog.logHead.logLen] = '\0';
		RunLog.logHead.num = g_tDataManage.ulRunLog_SN;//g_tDataManage.lastRunLogNum;
		RunLog.logHead.eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
		RTC_HYM8563GetTime(&RTCInfo);
		sprintf(RunLog.logHead.dataTime,"20%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
						RTCInfo.year.bcd_h,RTCInfo.year.bcd_l,
						RTCInfo.month.bcd_h,RTCInfo.month.bcd_l,
						RTCInfo.day.bcd_h,RTCInfo.day.bcd_l,
						RTCInfo.hour.bcd_h,RTCInfo.hour.bcd_l,
						RTCInfo.min.bcd_h,RTCInfo.min.bcd_l,
						RTCInfo.sec.bcd_h,RTCInfo.sec.bcd_l);
		Temp = sprintf(RunLog.logHead.shortStr,"Parameters Change");
		RunLog.logHead.shortStr[Temp] = '\0';
		RunLog.logHead.timestamp = GetCurrentTimestamp(RTCInfo);
        RunLog.usCrc = CRC16((uint8_t*)&RunLog, sizeof(RunLog_s)-2);
        
        Update_RunLog_Tail();
		Fatfs_Write_RunLog(RunLog.logHead.num, &RunLog);
		
        g_tDataManage.ulRunLog_SN;//g_tDataManage.lastRunLogNum++;
		Add_RunLog_Tail();
		Save_DataMange_Info(&g_tDataManage);
		
		//标记CAT必发项
		if(MachInfo.catMastSynServEvent.bit.sendRunLog != 1){
			MachInfo.catMastSynServEvent.bit.sendRunLog = 1;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
		}
		
		MachInfo.paramSet.ucPump_Scale = PumpRaw;
		return HAL_ERROR;
	}
}










