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
__IO osSemaphoreId_t xSema_OutIn_Key   = NULL;	//������
__IO osSemaphoreId_t xSema_Count_Key   = NULL;	//�����ź���
__IO osMutexId_t     xMutex_Com        = NULL;  //����������

__IO uint8_t g_OutIn_Module_Status     = 0;    //����ģ�飬�������е�״̬�� 0���޶�����1������
__IO uint8_t g_CountHandle_Status      = 0;    //����ģ�飬�������е�״̬�� 0���޶�����1�����ڴ����У����ڿ���ָʾ�Ƶľ�����ҫ��

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
*   ���й�����ջ����Сֵ
*/
void Printf_Free_StackSize(void)
{
    LOG_Info("Algo:%d", osThreadGetStackSpace(AlgoHandle));
    LOG_Info("UI_F:%d", osThreadGetStackSpace(UI_FrontEndHandle));
    LOG_Info("UI_B:%d", osThreadGetStackSpace(UI_BackEndHandle));    //���ö�ջ�ռ�
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
    //�ź���
	xSema_OutIn_Key = osSemaphoreNew(1, 0, NULL);
	xSema_Count_Key = osSemaphoreNew(1, 0, NULL);
    //��Ϣ����
	xUIQueue        = osMessageQueueNew(3, UI_TO_BACKEND_MSG_BUFFER_LEN, NULL);
	xBackEndQueue   = osMessageQueueNew(2, BACKEND_TO_UI_MSG_BUFFER_LEN, NULL);
    //������
    xMutex_Com      = osMutexNew(NULL);
    
    
    SemphrCountPrinterTxHandle   = osSemaphoreNew(1, 1, NULL);
    SemphrBinPrinterRxHandle     = osSemaphoreNew(1, 0, NULL);
    PrinterInfoQueueHandle       = osMessageQueueNew(1, sizeof(PrinterInfoData_s), NULL);
	
	
	//===ȫ���¼������===
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
//===��ӡ�����==========================================================
  PrintHandle = osThreadNew(PrinterTask, NULL, &Print_attributes);
  PrintStatusHandle = osThreadNew(PrinterIsOKTask, NULL, &PrintStatus_attributes);

//===CATģ�����=========================================================
	//��ѯCAT�Ƿ�OK���Լ�״̬��ʼ���͸�������
	CATUpdateStatusTaskHandle = osThreadNew(CATUpdateStatusTask, NULL, &CATUpdateStatus_attributes);
	CATTaskHandle = osThreadNew(CATTask, NULL, &CATTask_attributes);
	
	//����CATģ�鷢��������ؼ����ź���
	CATSemphrCountTxHandle = osSemaphoreNew(1, 1, NULL);
	
	//����CATģ�������ض�ֵ�ź���
	CATSemphrBinRxHandle = osSemaphoreNew(1, 0, NULL);
	
	//�������ڴ���CAT������Ϣ����Ϣ����
	CATQueueHandle = osMessageQueueNew(10, sizeof(CATQueueInfoBuf_s), NULL);
	
	//===�����������ͨѶ���===
	CATOtherTaskHandle = osThreadNew(CAT_OtherTask, NULL, &CATOtherTask_attributes);
	
	//�������ڴ���CAT������Ϣ����Ϣ����
	CATQueueOtherHandle = osMessageQueueNew(5, sizeof(CATQueueInfoBuf_s), NULL);
	
	//���ⴴ���͵���������˵ļ����ź���
	CATSemMutexHandle = osSemaphoreNew(1, 1, NULL);


//===USART1�����������=========================================================
	Usart1CMDHandle = osThreadNew(Usart1CMDTask, NULL, &Usart1CMD_attributes);
	
	//��������1������ض�ֵ�ź���
	Usart1RxSempBinHandle = osSemaphoreNew(1, 0, NULL);
	
//===ʧ�ܵ�����㷨����==========================
	Disable_Motor_Task();
	Disable_Algo_Task();
	
//===��λϵͳ��������־===================================================================
	SystemStartFlag = HAL_OK;
	
	//����״̬����(���ݱ�����eeprom)
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
*   ����motor����   
*/
void Disable_Motor_Task(void)
{
	uint16_t Count = 0;
	
	//�������������δִ�����ʱ���ȴ������
    while(1 == g_OutIn_Module_Status)
    {
        //δִ�����
        osDelay(50);
        if(0 == g_OutIn_Module_Status || ++Count > 220) //������ģ��ִ����ɣ��򳬹�11s
        {
            break;
        }
    }
	g_OutIn_Module_Status = 0;
	
    osSemaphoreAcquire(xSema_OutIn_Key, 0); //����ǰ����ź���
    osThreadSuspend(MotorHandle);
}

/*
*   �ָ�motor����   
*/
void Enable_Motor_Task(Bool eFlag)
{
	extern __IO OutIn_Module_Position_e g_eOutIn_Key;
	
    //true���ź�����false�����
    if(TRUE == eFlag)
    {
        osSemaphoreAcquire(xSema_OutIn_Key, 0); //����ǰ����ź���   
    }
	
	//�����λ��
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
    
    //�������ʼλ��
    g_eOutIn_Key = OutIn_Module_Positon();

  /* Infinite loop */
	for(;;)
	{
		osSemaphoreAcquire(xSema_OutIn_Key, osWaitForever);
		
		g_eOutIn_Key = OutIn_Module_Positon();
		g_eOutIn_Key = !g_eOutIn_Key;
		
        //ִ�ж���
        g_OutIn_Module_Status = 1;
        eErrorCode = OutIn_Module_Out_Or_In(MOTOR_MODE_NORMAL);

        if(OUTIN_MODULE_POSITION_IN == g_eOutIn_Key && ERROR_CODE_CLAMP_OUT == eErrorCode)
        {
            //��������쳣������Ҫ�ɿ�����
            if(FEED_BACK_SUCCESS != Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP, MOTOR_DIR_IN, MachInfo.paramSet.ulClamp_In_AddStep))
            {
                eErrorCode = ERROR_CODE_CLAMP_IN;
            }
        }
		
		//���½���λ����Ϣ����ֹż��δ���ֵ����
        g_eOutIn_Key = OutIn_Module_Positon();  
		
        if(OUTIN_MODULE_POSITION_NUKNOWN == g_eOutIn_Key || FEED_BACK_SUCCESS != eErrorCode) 
        {
            //�����쳣���ݶ���ֻ����������������ִ�н����֣���ʾ�澯��Ϣ��
            WM_MESSAGE tMsg = {0};
            tMsg.hWin  = g_hActiveWin;
            tMsg.MsgId = WM_MOTOR_ERROR_PROMPT;
            tMsg.Data.v = eErrorCode;
            WM_SendMessage(g_hActiveWin, &tMsg);
			
			//����������־
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

		//�����ж�������δ���ʱ��ȥ�����һ����Ϣ����
        osSemaphoreAcquire(xSema_OutIn_Key, 0);
		
        //�������
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
            //����ʱ��,UI��ʾʱ��
            RTC_Read((uint8_t*)MachRunPara.ucaLastestDateTime, DATE_TIME_LEN);
        }

        //
        if(i - ulTemp4 >= 25) //500ms
        {
            ulTemp4 = i;
            if(1 == g_CountHandle_Status)
            {
                Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status); //���Ƽ���ģʽ����ҫ 
            }else{
                Panel_Led_Status(RUN_MODE_NORMAL, g_ePanel_LED_Status);  //��������ģʽ������������ҫ 
            }
        }        
        
		//����ɨ��
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
*   ���� FrontEnd����   
*/
void Disable_FrontEnd_Task(void)
{
    osThreadSuspend(MotorHandle);
}

/*
*   �ָ�FrontEnd����   
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
	
	
    
    //STemWin��ʼ��
	GUI_Init();
	WM_MULTIBUF_Enable(1);
	g_hActiveWin = WM_HBKWIN;
	
//    //��ʾ�ֱ�
//	GUI_CURSOR_SetPosition(200, 200);
//	GUI_CURSOR_Show();
    
	GUI_SetColor(GUI_BLACK);
	GUI_Clear();
    

	//�ж��Ƿ��Ѿ�У׼��ϵ����
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
		
		//¼��У׼����
		GUI_TOUCH_CalcCoefficients(NUM_CALIB_POINTS,PixelX,PixelY,SamX,SamY, LCD_WIDTH_PIXEL, LCD_HIGHT_PIXEL);
	}else{
        //��������ϵУ׼ת����ϵ
        Touch_Adjust();
	}
	
	//��ʼ���ļ�ϵͳ�ײ㣬��ʧ�ܣ���ֱ����������
	FS_LowLevelInit();
	
	//�����ļ�ϵͳ��������ʧ�ܣ���ֱ����������
//    FS_Mount(FS_MOUNT_LOCK);
	
	//���ؿ�������ͼƬ��SDRAM(���ػ��͵�¼���棬����ʹ�ü��ص�SDRAM�������л�������)
    LoadBMP_To_SDRAM(BK_PICTURE_OTHERS_POWER_ON_OFF);
	
	//����ICON��SDRAM
	LoadAllICONToSDRAM();
	
    //��ʾ��������
    Show_BK_BMP(BK_SDRAM,BK_PICTURE_OTHERS_POWER_ON_OFF, 0, 0);
	
	osDelay(20);
	
    //��LCD����
    LCD_BL_ON;
	
//===�����ֿ�===
    SIF_Font_Init();
	
    //ʹ��UTF-8�����ʹ��SIF����
    GUI_UC_SetEncodeUTF8();
    
	//���ÿؼ�ȫ������
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
	
		//��PCBA�������
		g_hActiveWin = CreatePCBA_Page();
        LCD_BL_ON;
#else
    //��������������ͼ��SDRAM
    LoadPartBMP_To_SDRAM();
	
    //�򿪵�¼����
	g_hActiveWin = CreateLogin_Window();
	
	//�ж��Ƿ�������������
	if((MachRunPara.update.nextOpenToStartUpdateFlag == NEXT_OPEN_TO_START_UPDATE_FLAG) && (strlen((char*)MachInfo.labInfo.ucaMachineSN)>=12)){						//�ж��Ƿ�������������
		if(DialogWin == NULL){
			WM_HideWindow(g_hActiveWin);
			DialogWin = CreateDialogPage();
		}
		
		//������ʾ
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = NO_CONFIM_BUTTON;
		DialogPageData.fun = NULL;
		strcpy(DialogPageData.str,g_ucaLng_START_UPDATE[MachInfo.systemSet.eLanguage]);

		pMsg.MsgId = WM_USER_DATA;
		pMsg.Data.p = &DialogPageData;
		WM_SendMessage(DialogWin,&pMsg);		
	}else if((MachInfo.flag.bit.lock == 1) && (strlen((char*)MachInfo.labInfo.ucaMachineSN)>=12)){					//�ж��Ƿ���Ҫ����
		if(DialogWin == NULL){
			WM_HideWindow(g_hActiveWin);
			DialogWin = CreateDialogPage();
		}
		
		//������ʾ
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = NO_CONFIM_BUTTON;
		DialogPageData.fun = NULL;
		strcpy(DialogPageData.str,g_ucaLng_LOCK_MACHINE[MachInfo.systemSet.eLanguage]);

		pMsg.MsgId = WM_USER_DATA;
		pMsg.Data.p = &DialogPageData;
		WM_SendMessage(DialogWin,&pMsg);
	}
 
#endif
	
	//�ظ����ػ���ʱ����
#if AUTO_POWER_ONOFF_SELF_CHECK
	{
		osThreadId_t TempTask_AutoPowerOnoffCheckHandle;
		const osThreadAttr_t TempTask_AutoPowerOnoffCheck_attributes = {
		  .name = "TempTask_AutoPowerOnoffCheck",
		  .priority = (osPriority_t) osPriorityLow6,
		  .stack_size = 128*6
		};
		
		//��ʱ����һ��������ִ���Զ��Ŀ��ػ�����
		TempTask_AutoPowerOnoffCheckHandle = osThreadNew(TempTask_AutoPowerOnoffCheck, NULL, &TempTask_AutoPowerOnoffCheck_attributes);
	}
#endif

	//ж���ļ�ϵͳ
//	FS_Unmount();
	
	ulTick = HAL_GetTick();
	
    for(;;)
    {  
        UI_Msg_Handler();

        if(HAL_GetTick() - ulTick > 20000)//20s
        {
            ulTick = HAL_GetTick();
            if(WM_GetCallback(WM_HBKWIN) == Public_cbBkWin) //��WM_HBKWIN�Ļص�����δPublic_cbBkWinʱ���ŷ��͸�����Ϣ������״̬����
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
    
    //����HGB���ֵ�λ��ֵ
    HGB_Gran_Set(HGB_DR_CHANNEL,MachInfo.paramSet.ucHGB_Gain);

    //WBC���ֵ�λ��ֵ
    WBC_Gran_Set(WBC_DR_CHANNEL, MachInfo.paramSet.ucWBC_Gain);
	
	//������Ϣ�ṹ���ʼ��
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
*   ����algo����   
*/
void Disable_Algo_Task(void)
{
//	printf("suspend algo\r\n");
    osSemaphoreAcquire(xSema_Count_Key, 0); //����ǰ����ź��� 
    osThreadSuspend(AlgoHandle);
}

/*
*   �ָ�algo����   
*/
void Enable_Algo_Task(Bool eFlag)
{
//	printf("resume algo\r\n");
	
    //true���ź�����false�����
    if(TRUE == eFlag)
    {
        osSemaphoreAcquire(xSema_Count_Key, 0); //����ǰ����ź���
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
            //�����Լ���ڹ��ϣ�Ӱ�������������������ϣ��ټ���
            WM_SendMessageNoPara(g_hActiveWin, WM_ANALYSISI_ERROR_COUNT_NOT_ALLOW);
            ulCountMode = COUNT_MODE_INVALIDE;
        }else{
            //������ʼ
            g_CountHandle_Status = 1;
        }
        
        //
        switch(ulCountMode)
        {                            
            case COUNT_MODE_NORMAL:
            {
                //������������
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_NORMAL);
            }
            break;
            case COUNT_MODE_QC_NORMAL:
            {
                //�ʿء�������������
                QC_Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_QC_NORMAL);
            }
            break;
            case COUNT_MODE_QC_OUTSIDE:
            {
                QC_Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_QC_OUTSIDE);
                osSemaphoreRelease(xSema_Count_Key); //ִ����һ��
            }
            break;
            case COUNT_MODE_QC_INNER_AUTO:
            {
                QC_Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_QC_INNER_AUTO);
                osSemaphoreRelease(xSema_Count_Key); //ִ����һ��
            }
            break;
            case COUNT_MODE_RE_USE:
            {
                //����ģʽ����ʱ����������ⶨ���缫����,�����������ظ�ʹ��ģʽ��
                osDelay(Get_CountMode_Info()->ulReUseMode_Delay*1000);
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_RE_USE);
            }
            break;
            case COUNT_MODE_SIGNAL_SRC:
            {
                //�ź�Դģʽ
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_SIGNAL_SRC); 
            }
            break;    
            case COUNT_MODE_SIMULATION_SIGNAL_INNER: 
            {
                //��װ�źŷ���ģ�����,-->���β������ɼ��ڲ���װ�źš���ͨ���㷨���㡣�����Ӧ������־��ˢ�����ݵ������������档���������ʾ
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_SIMULATION_SIGNAL_INNER);//COUNT_MODE_SIMULATION); 
                //�Ͽ����ع�װ�ź�
                COUNT_SIGNAL_SW_OFF;
                //�ر��ź�
                Count_Signal_Stop();
            }
            break; 
            case COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO: 
            {
                //�ڲ���װ�źŷ���ģ��ȫ���̲���-->�ظ��������ɼ��ڲ���װ�źš���ͨ���㷨���㡣�����Ӧ������־��ˢ�����ݵ������������档���޽��������ʾ��
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO); 
                //�Ͽ����ع�װ�ź�
                //COUNT_SIGNAL_SW_OFF;
                //�ر��ź�
                //Count_Signal_Stop();
                osSemaphoreRelease(xSema_Count_Key); //ִ����һ��
            }
            break;
            case COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE:
            {
                //�ⲿ��װ�źŷ���ģ�����-->���β������ɼ��ⲿ��װ�źš���ͨ���㷨���㡣�����Ӧ������־��ˢ�����ݵ������������档���������ʾ
                Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE); 
            }
            break;
            case COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO:
            {
                if(Get_CountMode_Info()->ulOutGZ_CountNum > 0)
                {
                    //�ⲿ��װ�źŷ���ģ�����,-->�ظ��������ɼ��ⲿ��װ�źš���ͨ���㷨���㡣�����Ӧ������־��ˢ�����ݵ������������档���޽��������ʾ��
                    Count_Exec(TEST_PROJECT_WBC_HGB, COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO); 
                    Get_CountMode_Info()->ulOutGZ_CountNum--;
                    osSemaphoreRelease(xSema_Count_Key); //ִ����һ��
                }
            }
            break;
			
			//�ʼ�--WBC����
			case COUNT_MODE_ZJ_WBC_TEST:
			{
				ZJ_WBCGZTest();
			}break;
			
			//�ʼ�--HGB�հײ���
			case COUNT_MODE_ZJ_HGB_KB:
			{
				ZJ_HGB_KB();
			}break;
			
			//�ʼ�--HGB���ܶȲ���
			case COUNT_MODE_ZJ_HGB_JMD:
			{
				ZJ_HGB_JMD();
			}break;
			
			//�ʼ�--HGBУ׼1
			case COUNT_MODE_ZJ_HGB_JZ_1:
			{
				ZJ_HGB_JZ(ZJ_HGBJZ_1);
			}break;
			
			//�ʼ�--HGBУ׼2
			case COUNT_MODE_ZJ_HGB_JZ_2:
			{
				ZJ_HGB_JZ(ZJ_HGBJZ_2);
			}break;
			
			//�ʼ�--HGBУ׼3
			case COUNT_MODE_ZJ_HGB_JZ_3:
			{
				ZJ_HGB_JZ(ZJ_HGBJZ_3);
			}break;
			
			//�ʼ�--HGB����1
			case COUNT_MODE_ZJ_HGB_XX_1:
			{
				ZJ_HGB_XX(ZJ_HGBXX_1);
			}break;
			
			//�ʼ�--HGB����2
			case COUNT_MODE_ZJ_HGB_XX_2:
			{
				ZJ_HGB_XX(ZJ_HGBXX_2);
			}break;
			
			//�ʼ�--HGB����3
			case COUNT_MODE_ZJ_HGB_XX_3:
			{
				ZJ_HGB_XX(ZJ_HGBXX_3);
			}break;
			
			//�ʼ�--HGB����4
			case COUNT_MODE_ZJ_HGB_XX_4:
			{
				ZJ_HGB_XX(ZJ_HGBXX_4);
			}break;
			
			//�ʼ�--HGB����5
			case COUNT_MODE_ZJ_HGB_XX_5:
			{
				ZJ_HGB_XX(ZJ_HGBXX_5);
			}break;
			
			//�ʼ�--׼ȷ��1
			case COUNT_MODE_ZJ_HGB_ZQD_1:
			{
				ZJ_HGB_ZQD(ZJ_HGBZQD_1);
			}break;
			
			//�ʼ�--׼ȷ��2
			case COUNT_MODE_ZJ_HGB_ZQD_2:
			{
				ZJ_HGB_ZQD(ZJ_HGBZQD_2);
			}break;
			
			//�ʼ�--׼ȷ��3
			case COUNT_MODE_ZJ_HGB_ZQD_3:
			{
				ZJ_HGB_ZQD(ZJ_HGBZQD_3);
			}break;
			
            default:break;
        }
        
        //����������ָʾ��ֹͣ��ҫ
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
