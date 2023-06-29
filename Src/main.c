/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dac.h"
#include "dma.h"
#include "dma2d.h"
#include "fatfs.h"
#include "i2c.h"
#include "ltdc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_nand.h"
#include "ftl.h"
#include "bsp_sdram.h"
#include "portable.h"
#include "bsp_outin.h"
#include "ltdc.h"
//#include "bsp_touch.h"
#include "bsp_adc.h"
#include "bsp_pump.h"
#include "bsp_outin.h"
#include "bsp_motor.h"
#include "cmsis_os2.h"
#include "GUI.h"
#include "flash.h"
#include "backend_msg.h"
#include "bsp_light.h"
#include "bsp_resistance.h"
#include "bsp_beep.h"
#include "file_operate.h"
//#include "cm_backtrace.h"
#include "file_operate.h"
#include "CAT_iot.h"
#include "bsp_spi.h"
#include "parameter.h"
#include "process.h"
#include "Printer.h"
#include "usart1CMD.h"
#include "Common.h"
#include "bsp_eeprom.h"


#if !USE_RL_FLASH_FS
#include "fatfs.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//������Ϣ
MachInfo_s MachInfo __attribute__ ((at(SDRAM_MACHINE_INFO_ADDR))) = {0}; 

//UI ���ݽṹ��
__IO MachRunPara_s MachRunPara __attribute__((at(SDRAM_MACH_RUN_PARA_ADDR))) = {0};

//�ⲿ�˺�����������������
OutSideDataNum_s OutSideDataNum = {0};

//
Panel_LED_Status_e g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;         		//���ָʾ������״̬��
__IO OutIn_Module_Position_e g_eOutIn_Key = OUTIN_MODULE_POSITION_NUKNOWN; 		//������ģ��λ�ã������ַ������
__IO uint8_t g_ucOutInKey = 0;                                             		//�����ְ���״̬,�����˳�GIF
__IO uint8_t g_Count_Key = RESET;               								//����������־
__IO DataManage_t g_tDataManage = {0};          								//���ݹ���ȫ�ֽṹ��
__IO LogLevel_e g_eLogLevel = LOG_LEVEL_INFO;   								//LOG_LEVEL_INFO; //��ǰĬ����־�������
HAL_StatusTypeDef SystemStartFlag = HAL_ERROR;									//�������ϵͳ�Ƿ��Ѿ������ı�־��HAL_OK��ʾ�Ѿ�������ɣ�������ҪĿ���Ƿ�ֹϵͳ��δ�����ʹ����жϣ������ж���ʹ����ϵͳ��API���Ӷ������쳣����
#define MCU_FLASH_MAIN_SOFTWARE_START_ADR		32*1024							//��������������MCU��FLASH�е���ʼλ�õ�ƫ����

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */



/***************************************************************
*
*   ��������ջ(30k)+���е�(8k)+�㷨��50k��= 88k��ϵͳ��ջ��88+20(free)=108�� STM32F429igt6:256k�� ʣ�ࣺ256-108-64=84
*
***************************************************************/

const HeapRegion_t xHeapRegions[] =
{  
	{ ( uint8_t * ) 0x10000000UL, 0x10000 },        //64k
    { ( uint8_t * ) 0x20000000UL, 0x10000 },        //64k
 	{ NULL, 0 }
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
static void Bsp_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/*
*	�����ʼ������
*/
static void Bsp_Init(void)
{
    extern __IO OutIn_Module_Position_e  g_eOutIn_Key;   
	extern volatile CAT_ReceBuf_s CATReceBuf;
	extern PrinterReceBuf_s PrinterReceBuf;
	extern Usart1ReceBuf_s Usart1ReceBuf;
	extern  MachInfo_s    MachInfo;
    
	// SDRAM ���ò���
	Bsp_SDRAM_Init();
	
    //nandд�������ܣ�off
    NAND_WP_ON;
    
    //���õ��Ĭ����������
    Motor_Set_Current(MOTOR_CURRENT_VALUE_START);
    
    //�رչ�·���������HGB�ƣ�
    Light_Stop();
    
    //�ϵ�� ���ָʾ�ƣ�����״̬
    Panel_Led_Status(RUN_MODE_NORMAL, PANEL_LED_STATUS_NORMAL);
    
    //LCD����
    LCD_BL_OFF;
    
	//WBC����Դ��
	BC_CUR_SW_OFF;
    
	#if USE_RL_FLASH_FS
		NAND_Reset();
    #endif
    
    //�����Ӧ��ʱ����
    MTx_Timer_Init();
    
    //����ADC3��������ص�ѹ
	ADC3_Start();
    
    //�ط����ر�
	Pump_Stop();
	LIQUID_WAVE_CLOSE;
	AIR_WAVE_CLOSE;
    
    //ʧ������spi����, ����Ҫʱ�ٿ���
    SPI_CS_Disable_All();
   
    //����SPI�����ڵ��败��xpt2046����ֵ��λ����
   __HAL_SPI_ENABLE(&hspi4);
   
   //����IIC������EEPROM��RTC
   __HAL_I2C_ENABLE(&hi2c3);
   
   //��������ʼ��
   Beep_Init();
    
    //����ģʽ��ʼ��
   CountMode_Init();
   
   //����Usart1�Ŀ����ж�
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
	
	//����USART1��RX--DMAѭ��ģʽ
	HAL_UART_Receive_DMA(&huart1,(uint8_t*)Usart1ReceBuf.buf,USART1_RECE_BUF_LEN+1);
   
   //����Uart4�Ŀ����ж�
	__HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE);
	
	//����UART4��RX--DMAѭ��ģʽ
	HAL_UART_Receive_DMA(&huart4,(uint8_t*)CATReceBuf.buf,CAT_RECE_BUF_SIZE+1);
	
	//����Uart5�Ŀ����ж�
	__HAL_UART_ENABLE_IT(&huart5,UART_IT_IDLE);
	
	//����UART5��RX--DMAѭ��ģʽ
	HAL_UART_Receive_DMA(&huart5,(uint8_t*)PrinterReceBuf.buf,PRINTER_RECE_BUF_LEN+1);
	
	//��ʼ��SPI4��DMA����
	hspi4.Instance->CR2 |= SPI_CR2_RXDMAEN;
	hspi4.Instance->CR2 |= SPI_CR2_TXDMAEN;
	DMA2_Stream3->PAR = (uint32_t)&SPI4->DR;							
	DMA2_Stream4->PAR = (uint32_t)&SPI4->DR;
	
	//��ʼ��SPI FLASH
	SPIFlash_Init();
	
	//��ʾ�汾��Ϣ������ʱ��
	LOG_Info("MCU Version=%d, BuildDate=%s %s��SDRAM_ADR_END:%x��EEPROM_ADR_END:%u\r\n", MAIN_VERSION, __DATE__, __TIME__,SDRAM_END_ADDR,EEPROM_ADD_END);
}


/*
*	΢����ʱ����
*/
void Delay_US(uint32_t us)
{
    uint32_t start, now, delta, reload, us_tick;
    start = SysTick->VAL;
    reload = SysTick->LOAD;
    us_tick = SystemCoreClock / 1000000UL;
    do {
        now = SysTick->VAL;
        delta = start > now ? start - now : reload + start - now;
    } while(delta < us_tick * us);
}


/*
*   ��ʱulTime����, ����ָ��ʱ��ʱ������TRUE���Ƕ���ִ��
*/
Bool Delay_Until(uint32_t ulTime)
{
    static uint32_t sulEndTime = 0;
    static Bool eStatus = FALSE;
    
    if(eStatus == FALSE)
    {
        //sulEndTime = osKernelGetTickCount() + ulTime; 
        sulEndTime = HAL_GetTick() + ulTime;
        eStatus = TRUE;
        return FALSE;
    }
    //
    if(eStatus == TRUE)
    {
        if(HAL_GetTick() >= sulEndTime)
        {
            sulEndTime = 0;
            eStatus = FALSE;
            return TRUE;
        }else{
            return FALSE;
        }
    }
    //
    return FALSE;
}


/*
*   ������ʱ����������ģ�鲻ͬ�����з�ʽ��ѡ��ͬ��ʱ��ʽ,  
*/
void App_Delay(RunMode_e eRunMode, uint32_t ulTime)
{   
    if(RUN_MODE_NORMAL == eRunMode)
    {
        //�Ƕ�����ʱ��
        osDelay(ulTime);
    }else if(RUN_MODE_COUNT == eRunMode){
        //������ʱ
        HAL_Delay(ulTime);
    }
}





/*
* �������
*/
void Soft_Reboot(void)
{
    __disable_irq();
    NVIC_SystemReset();  
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  extern __IO MachRunPara_s MachRunPara;
	
	/***
	����APP������ж�������ƫ���������򣬻ᵼ��APP�ϲ������жϻ����IAP���жϴ����������ƫ������Ҫ����APP��FLASH�е���ʼ��ַƫ�������õ���
	ע����Ҫ�ֶ�����IROM1��ʵ�ʵ�FLASH��ŵ���ʼ��ַһ��
	***/
	SCB->VTOR = FLASH_BASE | MCU_FLASH_MAIN_SOFTWARE_START_ADR;
	__enable_irq();
	
	//�������ж�
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
    /**********************************************************************
    *   TIM2=MOTOR_CLAMP,   TIM3=OUTIN_MOTOR,       TIM10=HAL_Ticks
    *   TIM4=PUMP,          TIM12=�ڲ���װ�ź�,
    *
    ************************************************************************/
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_FMC_Init();
  MX_CRC_Init();
  MX_DMA2D_Init();
  MX_LTDC_Init();
  MX_ADC3_Init();
  MX_TIM4_Init();
  MX_ADC2_Init();
  MX_DAC_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_SPI4_Init();
  MX_I2C3_Init();
  MX_UART5_Init();
  MX_TIM12_Init();
  MX_UART4_Init();
  MX_TIM5_Init();
  MX_I2C2_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  Bsp_Init();

  vPortDefineHeapRegions(xHeapRegions);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 396;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 3;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM10 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM10) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    LOG_Info("Error_Handler: %s, %d", __FUNCTION__, __LINE__);
    while(1);
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
