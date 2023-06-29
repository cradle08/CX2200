/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include "cmsis_os2.h"
//#include "lwip/netbuf.h"
//#include "lwip.h"
#include "fmc.h"
#include "lcd.h"
#include "ltdc.h"
//#include "bsp_touch.h"
#include "cx_menu.h"
#include "bc.h"
#include "usart.h"
#include "process.h"
#include "bsp_outin.h"
#include "bsp_pump.h"
#include "bsp_light.h"
#include "bsp_motor.h"
#include "bsp_press.h"
#include "ftl.h"
#include "file_operate.h"
#include "bsp_nand.h"
#include "ui_msg.h"
#include "msg_def.h"
#include "backend_msg.h"
#include "cx_menu.h"
#include "monitorV.h"
#include "tp.h"
//#include "inputTask.h"
#include "xpt2046.h"
#include "bsp_rtc.h"
#include "bsp_eeprom.h"
#include "parameter.h"
#include "Public_menuDLG.h"
#include "Printer.h"
#include "PrinterTask.h"
#include "GUI_Touch.h"
#include "bsp_countsignal.h"
#include "bsp_resistance.h"
#include "CAT_iotTask.h"
#include "PrinterTask.h"
#include "usart1CMD.h"
#include "LISTVIEW.h"
#include "EDIT.h"
#include "DROPDOWN.h"
#include "TEXT.h"
#include "pcba.h"
#include "ZhiJian.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
__IO osSemaphoreId_t xSema_OutIn_Key   = NULL;	//进出仓
__IO osSemaphoreId_t xSema_Count_Key   = NULL;	//计数信号量
__IO osMutexId_t     xMutex_Com        = NULL;  //公共互斥锁

__IO uint8_t g_OutIn_Module_Status     = 0;    //进仓模块，在任务中的状态， 0：无动作，1：动作
__IO uint8_t g_CountHandle_Status      = 0;    //计数模块，在任务中的状态， 0：无动作，1：正在处理中（用于控制指示灯的均匀闪耀）

/* USER CODE END Variables */
/* Definitions for Motor */
osThreadId_t MotorHandle;
const osThreadAttr_t Motor_attributes = {
  .name = "Motor",
  .priority = (osPriority_t) osPriorityLow3,
  .stack_size = 1024 * 4
};
/* Definitions for PeriodScan */
osThreadId_t PeriodScanHandle;
const osThreadAttr_t PeriodScan_attributes = {
  .name = "PeriodScan",
  .priority = (osPriority_t) osPriorityLow6,
  .stack_size = 600 * 4
};
/* Definitions for UI_FrontEnd */
osThreadId_t UI_FrontEndHandle;
const osThreadAttr_t UI_FrontEnd_attributes = {
  .name = "UI_FrontEnd",
  .priority = (osPriority_t) osPriorityLow7,
  .stack_size = 2048 * 4
};
/* Definitions for UI_BackEnd */
osThreadId_t UI_BackEndHandle;
const osThreadAttr_t UI_BackEnd_attributes = {
  .name = "UI_BackEnd",
  .priority = (osPriority_t) osPriorityLow6,
  .stack_size = 2048 * 4
};
/* Definitions for Algo */
osThreadId_t AlgoHandle;
const osThreadAttr_t Algo_attributes = {
  .name = "Algo",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 12288 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
//osPriorityLow: 3 5 6 6 7  
osThreadId_t PrintHandle;
const osThreadAttr_t Print_attributes = {
  .name = "Print",
  .priority = (osPriority_t) osPriorityLow3,
  .stack_size = 1024 * 4
};

osThreadId_t PrintStatusHandle;
const osThreadAttr_t PrintStatus_attributes = {
  .name = "PrintStatus",
  .priority = (osPriority_t) osPriorityLow2,
  .stack_size = 512 * 4
};


osThreadId_t CATUpdateStatusTaskHandle;
const osThreadAttr_t CATUpdateStatus_attributes = {
  .name = "CATUpdateStatusTask",
  .priority = (osPriority_t) osPriorityLow4,
  .stack_size = 640*4
};

osThreadId_t CATTaskHandle;
const osThreadAttr_t CATTask_attributes = {
  .name = "CATTask",
  .priority = (osPriority_t) osPriorityLow4,
  .stack_size = 1280 * 4
};


osThreadId_t CATOtherTaskHandle;
const osThreadAttr_t CATOtherTask_attributes = {
  .name = "CAT_OtherTask",
  .priority = (osPriority_t) osPriorityLow5,
  .stack_size = 512 * 4
};


osThreadId_t Usart1CMDHandle;
const osThreadAttr_t Usart1CMD_attributes = {
  .name = "Usart1CMD",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 4
};


/*
*   运行过程中栈的最小值
*/
void Printf_Free_StackSize(void)
{
    LOG_Info("Algo:%d", osThreadGetStackSpace(AlgoHandle));
    LOG_Info("UI_F:%d", osThreadGetStackSpace(UI_FrontEndHandle));
    LOG_Info("UI_B:%d", osThreadGetStackSpace(UI_BackEndHandle));    //可用堆栈空间
    LOG_Info("Motor:%d", osThreadGetStackSpace(MotorHandle));
    LOG_Info("Print:%d", osThreadGetStackSpace(PrintHandle));
	LOG_Info("Print_S:%d", osThreadGetStackSpace(PrintStatusHandle));
    LOG_Info("Usart1CMD:%d", osThreadGetStackSpace(Usart1CMDHandle));
    LOG_Info("CATTask:%d", osThreadGetStackSpace(CATTaskHandle));
	LOG_Info("CATTas_S:%d", osThreadGetStackSpace(CATUpdateStatusTaskHandle));
	LOG_Info("OtherCatTask:%d", osThreadGetStackSpace(CATOtherTaskHandle));
	LOG_Info("Scan:%d", osThreadGetStackSpace(PeriodScanHandle));
	LOG_Info("CurFreeHeap:%u", xPortGetFreeHeapSize());
	LOG_Info("MinFreeHeap:%u", xPortGetMinimumEverFreeHeapSize());
}


/* USER CODE END FunctionPrototypes */

void MotorTask(void *argument);
void PeriodScanTask(void *argument);
void UI_FrontEndTask(void *argument);
void UI_BackEndTask(void *argument);
void AlgoTask(void *argument);

extern void MX_USB_HOST_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}

void GUI_TouchTask(void *argument);


/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
   
    extern osMessageQueueId_t xUIQueue;
    extern osMessageQueueId_t xBackEndQueue;
    extern __IO osMutexId_t   xMutex_Com;
    extern osEventFlagsId_t GlobalEventOtherGroupHandle;  
	extern osEventFlagsId_t GlobalEventPrintGroupHandle;	
	extern osEventFlagsId_t GlobalEventCatGroupHandle;		
    extern osSemaphoreId_t  SemphrCountPrinterTxHandle;
    extern osSemaphoreId_t  SemphrBinPrinterRxHandle;
    extern osMessageQueueId_t PrinterInfoQueueHandle;
	extern osMessageQueueId_t CATQueueHandle;
	extern HAL_StatusTypeDef SystemStartFlag;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t Usart1RxSempBinHandle;
	
	extern osSemaphoreId_t CATSemMutexHandle;
	extern osMessageQueueId_t CATQueueOtherHandle;

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
    //信号量
	xSema_OutIn_Key = osSemaphoreNew(1, 0, NULL);
	xSema_Count_Key = osSemaphoreNew(1, 0, NULL);
    //消息队列
	xUIQueue        = osMessageQueueNew(3, UI_TO_BACKEND_MSG_BUFFER_LEN, NULL);
	xBackEndQueue   = osMessageQueueNew(2, BACKEND_TO_UI_MSG_BUFFER_LEN, NULL);
    //互斥锁
    xMutex_Com      = osMutexNew(NULL);
    
    
    SemphrCountPrinterTxHandle   = osSemaphoreNew(1, 1, NULL);
    SemphrBinPrinterRxHandle     = osSemaphoreNew(1, 0, NULL);
    PrinterInfoQueueHandle       = osMessageQueueNew(1, sizeof(PrinterInfoData_s), NULL);
	
	
	//===全局事件组相关===
    GlobalEventOtherGroupHandle  = osEventFlagsNew(NULL);
	GlobalEventPrintGroupHandle  = osEventFlagsNew(NULL);
	GlobalEventCatGroupHandle    = osEventFlagsNew(NULL);
	
//    xEvent_BC = osEventFlagsNew(NULL);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Motor */
  MotorHandle = osThreadNew(MotorTask, NULL, &Motor_attributes);

  /* creation of PeriodScan */
  PeriodScanHandle = osThreadNew(PeriodScanTask, NULL, &PeriodScan_attributes);

  /* creation of UI_FrontEnd */
  UI_FrontEndHandle = osThreadNew(UI_FrontEndTask, NULL, &UI_FrontEnd_attributes);

  /* creation of UI_BackEnd */
  UI_BackEndHandle = osThreadNew(UI_BackEndTask, NULL, &UI_BackEnd_attributes);

  /* creation of Algo */
  AlgoHandle = osThreadNew(AlgoTask, NULL, &Algo_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
//===打印机相关==========================================================
  PrintHandle = osThreadNew(PrinterTask, NULL, &Print_attributes);
  PrintStatusHandle = osThreadNew(PrinterIsOKTask, NULL, &PrintStatus_attributes);

//===CAT模块相关=========================================================
	//查询CAT是否OK，以及状态初始化和更新任务
	CATUpdateStatusTaskHandle = osThreadNew(CATUpdateStatusTask, NULL, &CATUpdateStatus_attributes);
	CATTaskHandle = osThreadNew(CATTask, NULL, &CATTask_attributes);
	
	//创建CAT模块发送数据相关计数信号量
	CATSemphrCountTxHandle = osSemaphoreNew(1, 1, NULL);
	
	//创建CAT模块接收相关二值信号量
	CATSemphrBinRxHandle = osSemaphoreNew(1, 0, NULL);
	
	//创建用于传递CAT发布消息的消息队列
	CATQueueHandle = osMessageQueueNew(10, sizeof(CATQueueInfoBuf_s), NULL);
	
	//===第三方服务端通讯相关===
	CATOtherTaskHandle = osThreadNew(CAT_OtherTask, NULL, &CATOtherTask_attributes);
	
	//创建用于传递CAT发布消息的消息队列
	CATQueueOtherHandle = osMessageQueueNew(5, sizeof(CATQueueInfoBuf_s), NULL);
	
	//互斥创怀和第三方服务端的计数信号量
	CATSemMutexHandle = osSemaphoreNew(1, 1, NULL);


//===USART1调试命令相关=========================================================
	Usart1CMDHandle = osThreadNew(Usart1CMDTask, NULL, &Usart1CMD_attributes);
	
	//创建串口1接收相关二值信号量
	Usart1RxSempBinHandle = osSemaphoreNew(1, 0, NULL);
	
//===失能电机和算法任务==========================
	Disable_Motor_Task();
	Disable_Algo_Task();
	
//===置位系统已启动标志===================================================================
	SystemStartFlag = HAL_OK;
	
	//机器状态参数(数据保存在eeprom)
    LOG_Info("Machine Info Size=%d, WBCHGB Data Size=%d", sizeof(MachInfo_s), sizeof(WBCHGB_TestInfo_t));
    Machine_Info_Init(&MachInfo);
	

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_MotorTask */
/**
  * @brief  Function implementing the Motor thread.
  * @param  argument: Not used
  * @retval None
  */

/*
*   挂起motor任务   
*/
void Disable_Motor_Task(void)
{
	uint16_t Count = 0;
	
	//当进出电机，还未执行完成时，等待其完成
    while(1 == g_OutIn_Module_Status)
    {
        //未执行完成
        osDelay(50);
        if(0 == g_OutIn_Module_Status || ++Count > 220) //进出仓模块执行完成，或超过11s
        {
            break;
        }
    }
	g_OutIn_Module_Status = 0;
	
    osSemaphoreAcquire(xSema_OutIn_Key, 0); //挂起前清除信号量
    osThreadSuspend(MotorHandle);
}

/*
*   恢复motor任务   
*/
void Enable_Motor_Task(Bool eFlag)
{
	extern __IO OutIn_Module_Position_e g_eOutIn_Key;
	
    //true清信号量，false不清除
    if(TRUE == eFlag)
    {
        osSemaphoreAcquire(xSema_OutIn_Key, 0); //挂起前清除信号量   
    }
	
	//检测电机位置
    g_eOutIn_Key = OutIn_Module_Positon();
	
    osThreadResume(MotorHandle);
}

/* USER CODE END Header_MotorTask */
void MotorTask(void *argument)
{
  /* init code for USB_HOST */
//  MX_USB_HOST_Init();
  /* USER CODE BEGIN MotorTask */
  /* Infinite loop */
    extern MachInfo_s MachInfo;
    extern __IO OutIn_Module_Position_e g_eOutIn_Key;
	extern __IO osSemaphoreId xSema_OutIn_Key;
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
	RunLog_s RunLog = {0};
	RTC_HYM8563Info_s RTCInfo = {0};
	uint32_t ulIndex = 0;
    
    //检测电机初始位置
    g_eOutIn_Key = OutIn_Module_Positon();

  /* Infinite loop */
	for(;;)
	{
		osSemaphoreAcquire(xSema_OutIn_Key, osWaitForever);
		
		g_eOutIn_Key = OutIn_Module_Positon();
		g_eOutIn_Key = !g_eOutIn_Key;
		
        //执行动作
        g_OutIn_Module_Status = 1;
        eErrorCode = OutIn_Module_Out_Or_In(MOTOR_MODE_NORMAL);

        if(OUTIN_MODULE_POSITION_IN == g_eOutIn_Key && ERROR_CODE_CLAMP_OUT == eErrorCode)
        {
            //如果进仓异常，则需要松开夹子
            if(FEED_BACK_SUCCESS != Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP, MOTOR_DIR_IN, MachInfo.paramSet.ulClamp_In_AddStep))
            {
                eErrorCode = ERROR_CODE_CLAMP_IN;
            }
        }
		
		//更新进仓位置信息，防止偶发未进仓的情况
        g_eOutIn_Key = OutIn_Module_Positon();  
		
        if(OUTIN_MODULE_POSITION_NUKNOWN == g_eOutIn_Key || FEED_BACK_SUCCESS != eErrorCode) 
        {
            //进仓异常（暂定于只有在样本分析界面执行进出仓，提示告警信息）
            WM_MESSAGE tMsg = {0};
            tMsg.hWin  = g_hActiveWin;
            tMsg.MsgId = WM_MOTOR_ERROR_PROMPT;
            tMsg.Data.v = eErrorCode;
            WM_SendMessage(g_hActiveWin, &tMsg);
			
			//保存运行日志
			RunLog.logHead.logLen = 0;
			RunLog.logHead.logLen += sprintf(RunLog.Str+RunLog.logHead.logLen,"Movement Failure\r\n");
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
			ulIndex = sprintf(RunLog.logHead.shortStr,"Abnormal");
			RunLog.logHead.shortStr[ulIndex] = '\0';
			RunLog.logHead.timestamp = GetCurrentTimestamp(RTCInfo);
            RunLog.usCrc = CRC16((uint8_t*)&RunLog, sizeof(RunLog_s)-2);
            
            ulIndex = Update_RunLog_Tail();
			Fatfs_Write_RunLog(ulIndex, &RunLog);
			g_tDataManage.ulRunLog_SN++;//g_tDataManage.lastRunLogNum++;
            Add_RunLog_Tail();
			Save_DataMange_Info(&g_tDataManage);
        }

		//按键有抖动，多次触发时，去掉最后一次消息触发
        osSemaphoreAcquire(xSema_OutIn_Key, 0);
		
        //动作完成
        g_OutIn_Module_Status = 0;
		
		osDelay(1);
    }
  /* USER CODE END MotorTask */
}

/* USER CODE BEGIN Header_PeriodScanTask */
/**
* @brief Function implementing the PeriodScan thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_PeriodScanTask */
void PeriodScanTask(void *argument)
{
  /* USER CODE BEGIN PeriodScanTask */
  /* Infinite loop */
  
    extern MachInfo_s MachInfo;
    extern __IO MachRunPara_s MachRunPara;
    uint32_t ulTemp1 = 0, ulTemp2 = 0, i = 0, ulTemp4 = 0;
    
    for(;;)
    {
        i++;
        if(i - ulTemp1 >= 50) // 1S
        {
            ulTemp1 = i;
            LED0_TRIGGLE;
			
//			Printf_Free_StackSize();
        }
        //
        if(i - ulTemp2 >= 250) // 5S
        {
            ulTemp2 = i;
            //更新时间,UI显示时间
            RTC_Read((uint8_t*)MachRunPara.ucaLastestDateTime, DATE_TIME_LEN);
        }

        //
        if(i - ulTemp4 >= 25) //500ms
        {
            ulTemp4 = i;
            if(1 == g_CountHandle_Status)
            {
                Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status); //面板灯计数模式，闪耀 
            }else{
                Panel_Led_Status(RUN_MODE_NORMAL, g_ePanel_LED_Status);  //面板灯正常模式，常亮，不闪耀 
            }
        }        
        
		//触摸扫描
        Touch_Scan();
		
        osDelay(20);
    }
  /* USER CODE END PeriodScanTask */
}

/* USER CODE BEGIN Header_UI_FrontEndTask */
/**
* @brief Function implementing the UI_FrontEnd thread.
* @param argument: Not used
* @retval None
*/

/*
*   挂起 FrontEnd任务   
*/
void Disable_FrontEnd_Task(void)
{
    osThreadSuspend(MotorHandle);
}

/*
*   恢复FrontEnd任务   
*/
void Enable_FrontEnd_Task(void)
{
   osThreadResume(UI_FrontEndHandle); 
}


/* USER CODE END Header_UI_FrontEndTask */
void UI_FrontEndTask(void *argument)
{
  /* USER CODE BEGIN UI_FrontEndTask */
    /* Infinite loop */
	extern WM_HWIN DialogWin;
	extern MachInfo_s MachInfo;
	extern WM_HWIN CreateDialogPage(void);
	extern WM_HWIN CreatePCBA_Page(void);
	CommonDialogPageData_s DialogPageData = {0};
	WM_MESSAGE pMsg;
	uint32_t ulTick = 0;
	
	
    
    //STemWin初始化
	GUI_Init();
	WM_MULTIBUF_Enable(1);
	g_hActiveWin = WM_HBKWIN;
	
//    //显示手标
//	GUI_CURSOR_SetPosition(200, 200);
//	GUI_CURSOR_Show();
    
	GUI_SetColor(GUI_BLACK);
	GUI_Clear();
    

	//判断是否已经校准过系数了
	if(MachInfo.touch.flag == TOUCH_INIT_FLAG){
		int PixelX[NUM_CALIB_POINTS];
		int PixelY[NUM_CALIB_POINTS];
		int SamX[NUM_CALIB_POINTS];	
		int SamY[NUM_CALIB_POINTS];
		uint8_t i = 0;
		
		for(i=0;i<NUM_CALIB_POINTS;i++){
			PixelX[i] = MachInfo.touch.pixelX[i];
		}
		for(i=0;i<NUM_CALIB_POINTS;i++){
			PixelY[i] = MachInfo.touch.pixelY[i];
		}
		for(i=0;i<NUM_CALIB_POINTS;i++){
			SamX[i] = MachInfo.touch.samX[i];
		}
		for(i=0;i<NUM_CALIB_POINTS;i++){
			SamY[i] = MachInfo.touch.samY[i];
		}
		
		//录入校准参数
		GUI_TOUCH_CalcCoefficients(NUM_CALIB_POINTS,PixelX,PixelY,SamX,SamY, LCD_WIDTH_PIXEL, LCD_HIGHT_PIXEL);
	}else{
        //建立坐标系校准转换关系
        Touch_Adjust();
	}
	
	//初始化文件系统底层，若失败，则直接锁定程序
	FS_LowLevelInit();
	
	//挂载文件系统，若挂载失败，则直接锁定程序
//    FS_Mount(FS_MOUNT_LOCK);
	
	//加载开机背景图片到SDRAM(开关机和登录画面，建议使用加载到SDRAM，画面切换更流畅)
    LoadBMP_To_SDRAM(BK_PICTURE_OTHERS_POWER_ON_OFF);
	
	//加载ICON到SDRAM
	LoadAllICONToSDRAM();
	
    //显示开机画面
    Show_BK_BMP(BK_SDRAM,BK_PICTURE_OTHERS_POWER_ON_OFF, 0, 0);
	
	osDelay(20);
	
    //开LCD背光
    LCD_BL_ON;
	
//===加载字库===
    SIF_Font_Init();
	
    //使能UTF-8，便可使用SIF字体
    GUI_UC_SetEncodeUTF8();
    
	//设置控件全局属性
    GUI_SetDefaultFont(&HZ_SONGTI_16);
	LISTVIEW_SetDefaultFont(&HZ_SONGTI_16);
    BUTTON_SetDefaultFont(&HZ_SONGTI_16);
	HEADER_SetDefaultFont(&HZ_SONGTI_16);
    TEXT_SetDefaultFont(&HZ_SONGTI_16);
    EDIT_SetDefaultFont(&HZ_SONGTI_16);
    DROPDOWN_SetDefaultFont(&HZ_SONGTI_16);
//===============

#if PCBA_CHECK_MAIN_BOARD || PCBA_CHECK_ZJ_BOARD || PCBA_CHECK_CAT || PCBA_CHECK_TOUCH_SCREEN || PCBA_CHECK_MOTO || PCBA_CHECK_PRESSURE_MONITOR || PCBA_CHECK_VALVE_PUMP
		Public_Msg_Init();
	
		//打开PCBA检验界面
		g_hActiveWin = CreatePCBA_Page();
        LCD_BL_ON;
#else
    //加载其他部分切图到SDRAM
    LoadPartBMP_To_SDRAM();
	
    //打开登录界面
	g_hActiveWin = CreateLogin_Window();
	
	//判断是否启动升级流程
	if((MachRunPara.update.nextOpenToStartUpdateFlag == NEXT_OPEN_TO_START_UPDATE_FLAG) && (strlen((char*)MachInfo.labInfo.ucaMachineSN)>=12)){						//判断是否启动升级流程
		if(DialogWin == NULL){
			WM_HideWindow(g_hActiveWin);
			DialogWin = CreateDialogPage();
		}
		
		//弹窗提示
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = NO_CONFIM_BUTTON;
		DialogPageData.fun = NULL;
		strcpy(DialogPageData.str,g_ucaLng_START_UPDATE[MachInfo.systemSet.eLanguage]);

		pMsg.MsgId = WM_USER_DATA;
		pMsg.Data.p = &DialogPageData;
		WM_SendMessage(DialogWin,&pMsg);		
	}else if((MachInfo.flag.bit.lock == 1) && (strlen((char*)MachInfo.labInfo.ucaMachineSN)>=12)){					//判断是否需要锁机
		if(DialogWin == NULL){
			WM_HideWindow(g_hActiveWin);
			DialogWin = CreateDialogPage();
		}
		
		//弹窗提示
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = NO_CONFIM_BUTTON;
		DialogPageData.fun = NULL;
		strcpy(DialogPageData.str,g_ucaLng_LOCK_MACHINE[MachInfo.systemSet.eLanguage]);

		pMsg.MsgId = WM_USER_DATA;
		pMsg.Data.p = &DialogPageData;
		WM_SendMessage(DialogWin,&pMsg);
	}
 
#endif
	
	//重复开关机临时任务
#if AUTO_POWER_ONOFF_SELF_CHECK
	{
		osThreadId_t TempTask_AutoPowerOnoffCheckHandle;
		const osThreadAttr_t TempTask_AutoPowerOnoffCheck_attributes = {
		  .name = "TempTask_AutoPowerOnoffCheck",
		  .priority = (osPriority_t) osPriorityLow6,
		  .stack_size = 128*6
		};
		
		//临时创建一个任务来执行自动的开关机任务
		TempTask_AutoPowerOnoffCheckHandle = osThreadNew(TempTask_AutoPowerOnoffCheck, NULL, &TempTask_AutoPowerOnoffCheck_attributes);
	}
#endif

	//卸载文件系统
//	FS_Unmount();
	
	ulTick = HAL_GetTick();
	
    for(;;)
    {  
        UI_Msg_Handler();

        if(HAL_GetTick() - ulTick > 20000)//20s
        {
            ulTick = HAL_GetTick();
            if(WM_GetCallback(WM_HBKWIN) == Public_cbBkWin) //当WM_HBKWIN的回调函数未Public_cbBkWin时，才发送更新消息（更新状态栏）
            {
                WM_SendMessageNoPara(WM_HBKWIN, WM_PUBLIC_UPDATE);
            } 
        }
        GUI_Delay(20);
    }
  /* USER CODE END UI_FrontEndTask */
}

/* USER CODE BEGIN Header_UI_BackEndTask */
/**
* @brief Function implementing the UI_BackEnd thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_UI_BackEndTask */
void UI_BackEndTask(void *argument)
{
  /* USER CODE BEGIN UI_BackEndTask */
    /* Infinite loop */
	extern MachInfo_s MachInfo;
    
    //设置HGB数字电位器值
    HGB_Gran_Set(HGB_DR_CHANNEL,MachInfo.paramSet.ucHGB_Gain);

    //WBC数字电位器值
    WBC_Gran_Set(WBC_DR_CHANNEL, MachInfo.paramSet.ucWBC_Gain);
	
	//病人信息结构体初始化
	Patient_Info_Init();
	
    for(;;)
    {
        BackEnd_Msg_Handler();
        osDelay(1);
    }
  /* USER CODE END UI_BackEndTask */
}

/* USER CODE BEGIN Header_AlgoTask */
/**
* @brief Function implementing the Algo thread.
* @param argument: Not used
* @retval None
*/

/*
*   挂起algo任务   
*/
void Disable_Algo_Task(void)
{
//	printf("suspend algo\r\n");
    osSemaphoreAcquire(xSema_Count_Key, 0); //挂起前清除信号量 
    osThreadSuspend(AlgoHandle);
}

/*
*   恢复algo任务   
*/
void Enable_Algo_Task(Bool eFlag)
{
//	printf("resume algo\r\n");
	
    //true清信号量，false不清除
    if(TRUE == eFlag)
    {
        osSemaphoreAcquire(xSema_Count_Key, 0); //挂起前清除信号量
    }
    osThreadResume(AlgoHandle);
}

/* USER CODE END Header_AlgoTask */
void AlgoTask(void *argument)
{
  /* USER CODE BEGIN AlgoTask */
  
  extern MachInfo_s MachInfo;
  extern __IO uint8_t g_Count_Key;
  extern __IO osSemaphoreId_t xSema_Count_Key;
  extern __IO Bool g_bAlgoTask_Flag;
	
  __IO uint32_t ulCountMode;
   
    for(;;)
    {
        osSemaphoreAcquire(xSema_Count_Key, osWaitForever);
		
        ulCountMode = Get_CountMode_Info()->ulCountMode;
		
        if(PANEL_LED_STATUS_ERROR ==  g_ePanel_LED_Status)
        {
            //开机自检存在故障（影响计数），需先清除故障，再计数
            WM_SendMessageNoPara(g_hActiveWin, WM_ANALYSISI_ERROR_COUNT_NOT_ALLOW);
            ulCountMode = COUNT_MODE_INVALIDE;
        }else{
            //计数开始
            g_CountHandle_Status = 1;
        }
        
        //
        switch(ulCountMode)
        {                            
            case COUNT_MODE_NORMAL:
            {
                //正常计数流程
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_NORMAL);
            }
            break;
            case COUNT_MODE_QC_NORMAL:
            {
                //质控、正常计数流程
                QC_Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_QC_NORMAL);
            }
            break;
            case COUNT_MODE_QC_OUTSIDE:
            {
                QC_Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_QC_OUTSIDE);
                osSemaphoreRelease(xSema_Count_Key); //执行下一次
            }
            break;
            case COUNT_MODE_QC_INNER_AUTO:
            {
                QC_Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_QC_INNER_AUTO);
                osSemaphoreRelease(xSema_Count_Key); //执行下一次
            }
            break;
            case COUNT_MODE_RE_USE:
            {
                //测试模式（定时采样，不检测定量电极触发,即：计数池重复使用模式）
                osDelay(Get_CountMode_Info()->ulReUseMode_Delay*1000);
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_RE_USE);
            }
            break;
            case COUNT_MODE_SIGNAL_SRC:
            {
                //信号源模式
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_SIGNAL_SRC); 
            }
            break;    
            case COUNT_MODE_SIMULATION_SIGNAL_INNER: 
            {
                //工装信号仿真模拟测试,-->单次操作：采集内部工装信号。并通过算法计算。保存对应数据日志。刷新数据到样本分析界面。界面操作提示
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_SIMULATION_SIGNAL_INNER);//COUNT_MODE_SIMULATION); 
                //断开板载工装信号
                COUNT_SIGNAL_SW_OFF;
                //关闭信号
                Count_Signal_Stop();
            }
            break; 
            case COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO: 
            {
                //内部工装信号仿真模拟全流程测试-->重复操作：采集内部工装信号。并通过算法计算。保存对应数据日志。刷新数据到样本分析界面。（无界面操作提示）
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO); 
                //断开板载工装信号
                //COUNT_SIGNAL_SW_OFF;
                //关闭信号
                //Count_Signal_Stop();
                osSemaphoreRelease(xSema_Count_Key); //执行下一次
            }
            break;
            case COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE:
            {
                //外部工装信号仿真模拟测试-->单次操作：采集外部工装信号。并通过算法计算。保存对应数据日志。刷新数据到样本分析界面。界面操作提示
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE); 
            }
            break;
            case COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO:
            {
                if(Get_CountMode_Info()->ulOutGZ_CountNum > 0)
                {
                    //外部工装信号仿真模拟测试,-->重复操作：采集外部工装信号。并通过算法计算。保存对应数据日志。刷新数据到样本分析界面。（无界面操作提示）
                    Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO); 
                    Get_CountMode_Info()->ulOutGZ_CountNum--;
                    osSemaphoreRelease(xSema_Count_Key); //执行下一次
                }
            }
            break;
			
			//质检--WBC测试
			case COUNT_MODE_ZJ_WBC_TEST:
			{
				ZJ_WBCGZTest();
			}break;
			
			//质检--HGB空白测试
			case COUNT_MODE_ZJ_HGB_KB:
			{
				ZJ_HGB_KB();
			}break;
			
			//质检--HGB精密度测试
			case COUNT_MODE_ZJ_HGB_JMD:
			{
				ZJ_HGB_JMD();
			}break;
			
			//质检--HGB校准1
			case COUNT_MODE_ZJ_HGB_JZ_1:
			{
				ZJ_HGB_JZ(ZJ_HGBJZ_1);
			}break;
			
			//质检--HGB校准2
			case COUNT_MODE_ZJ_HGB_JZ_2:
			{
				ZJ_HGB_JZ(ZJ_HGBJZ_2);
			}break;
			
			//质检--HGB校准3
			case COUNT_MODE_ZJ_HGB_JZ_3:
			{
				ZJ_HGB_JZ(ZJ_HGBJZ_3);
			}break;
			
			//质检--HGB线性1
			case COUNT_MODE_ZJ_HGB_XX_1:
			{
				ZJ_HGB_XX(ZJ_HGBXX_1);
			}break;
			
			//质检--HGB线性2
			case COUNT_MODE_ZJ_HGB_XX_2:
			{
				ZJ_HGB_XX(ZJ_HGBXX_2);
			}break;
			
			//质检--HGB线性3
			case COUNT_MODE_ZJ_HGB_XX_3:
			{
				ZJ_HGB_XX(ZJ_HGBXX_3);
			}break;
			
			//质检--HGB线性4
			case COUNT_MODE_ZJ_HGB_XX_4:
			{
				ZJ_HGB_XX(ZJ_HGBXX_4);
			}break;
			
			//质检--HGB线性5
			case COUNT_MODE_ZJ_HGB_XX_5:
			{
				ZJ_HGB_XX(ZJ_HGBXX_5);
			}break;
			
			//质检--准确度1
			case COUNT_MODE_ZJ_HGB_ZQD_1:
			{
				ZJ_HGB_ZQD(ZJ_HGBZQD_1);
			}break;
			
			//质检--准确度2
			case COUNT_MODE_ZJ_HGB_ZQD_2:
			{
				ZJ_HGB_ZQD(ZJ_HGBZQD_2);
			}break;
			
			//质检--准确度3
			case COUNT_MODE_ZJ_HGB_ZQD_3:
			{
				ZJ_HGB_ZQD(ZJ_HGBZQD_3);
			}break;
			
            default:break;
        }
        
        //计数结束，指示灯停止闪耀
        g_CountHandle_Status = 0;
        //
        if(COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO != ulCountMode && COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO != ulCountMode && COUNT_MODE_QC_INNER_AUTO != ulCountMode && COUNT_MODE_QC_NORMAL != ulCountMode)
        {
			Disable_Algo_Task();
        }
        
        osDelay(1);
    }
  /* USER CODE END AlgoTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
