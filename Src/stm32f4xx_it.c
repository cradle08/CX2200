/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f4xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
#include "bc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern HCD_HandleTypeDef hhcd_USB_OTG_FS;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;
extern LTDC_HandleTypeDef hltdc;
extern DMA_HandleTypeDef hdma_spi4_rx;
extern DMA_HandleTypeDef hdma_spi4_tx;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart4_tx;
extern DMA_HandleTypeDef hdma_uart5_tx;
extern DMA_HandleTypeDef hdma_uart5_rx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim10;

/* USER CODE BEGIN EV */
#include "bsp_motor.h"
#include "usart.h"
#include "CAT_iot.h"
#include "Printer.h"
#include "usart1CMD.h"
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
////////////////////////    LOG_Info("HardFault_Handler ...");
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream0 global interrupt.
  */
void DMA1_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream0_IRQn 0 */

  /* USER CODE END DMA1_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart5_rx);
  /* USER CODE BEGIN DMA1_Stream0_IRQn 1 */

  /* USER CODE END DMA1_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream2 global interrupt.
  */
void DMA1_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream2_IRQn 0 */

  /* USER CODE END DMA1_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart4_rx);
  /* USER CODE BEGIN DMA1_Stream2_IRQn 1 */

  /* USER CODE END DMA1_Stream2_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream4 global interrupt.
  */
void DMA1_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream4_IRQn 0 */

  /* USER CODE END DMA1_Stream4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart4_tx);
  /* USER CODE BEGIN DMA1_Stream4_IRQn 1 */

  /* USER CODE END DMA1_Stream4_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */
    
    //PI8 MOTOR OUTINT KEY
    
  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  HAL_TIM_IRQHandler(&htim10);
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  /* USER CODE BEGIN TIM2_IRQn 1 */
    //探针电机
    MTx_PWM_ISR(MOTOR_CLAMP);
  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  /* USER CODE BEGIN TIM3_IRQn 1 */
    //进出仓电机
    MTx_PWM_ISR(MOTOR_OUTIN);
  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	extern UART_HandleTypeDef huart1;
	extern HAL_StatusTypeDef SystemStartFlag;
	extern osSemaphoreId_t Usart1RxSempBinHandle;
	extern Usart1ReceBuf_s Usart1ReceBuf;
	
	//判断是否是空闲中断触发
	if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE) != RESET){
		//清除空闲中断标志
		huart1.Instance->DR;
		
		if(SystemStartFlag == HAL_OK){
			Usart1ReceBuf.writeIndex = (USART1_RECE_BUF_LEN+1)-huart1.hdmarx->Instance->NDTR;
			
			//释放二值信号量
			osSemaphoreRelease(Usart1RxSempBinHandle);
		}
	}
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */
  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */
    
    // PC13 BC_COUNT_KEY
  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream7 global interrupt.
  */
void DMA1_Stream7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream7_IRQn 0 */

  /* USER CODE END DMA1_Stream7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart5_tx);
  /* USER CODE BEGIN DMA1_Stream7_IRQn 1 */

  /* USER CODE END DMA1_Stream7_IRQn 1 */
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
  /* USER CODE BEGIN UART4_IRQn 0 */
	extern UART_HandleTypeDef huart4;
	extern HAL_StatusTypeDef SystemStartFlag;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern volatile CAT_ReceBuf_s CATReceBuf;
	
	//判断是否是空闲中断触发
	if(__HAL_UART_GET_FLAG(&huart4,UART_FLAG_IDLE) != RESET){
		//清除空闲中断标志
		huart4.Instance->DR;
		
		if(SystemStartFlag == HAL_OK){
			CATReceBuf.writeIndex = (CAT_RECE_BUF_SIZE+1)-huart4.hdmarx->Instance->NDTR;
			
			//释放二值信号量
			osSemaphoreRelease(CATSemphrBinRxHandle);
		}
	}
  /* USER CODE END UART4_IRQn 0 */
  HAL_UART_IRQHandler(&huart4);
  /* USER CODE BEGIN UART4_IRQn 1 */

  /* USER CODE END UART4_IRQn 1 */
}

/**
  * @brief This function handles UART5 global interrupt.
  */
void UART5_IRQHandler(void)
{
  /* USER CODE BEGIN UART5_IRQn 0 */
	extern UART_HandleTypeDef huart5;
	extern HAL_StatusTypeDef SystemStartFlag;
	extern osSemaphoreId_t  SemphrBinPrinterRxHandle;
	extern PrinterReceBuf_s	PrinterReceBuf;
	
	//判断是否是空闲中断触发
	if(__HAL_UART_GET_FLAG(&huart5,UART_FLAG_IDLE) != RESET){
		//清除空闲中断标志
		huart5.Instance->DR;
		
		if(SystemStartFlag == HAL_OK){
			PrinterReceBuf.writeIndex = (PRINTER_RECE_BUF_LEN+1)-huart5.hdmarx->Instance->NDTR;
			
			//释放二值信号量
			osSemaphoreRelease(SemphrBinPrinterRxHandle);
		}
	}
  /* USER CODE END UART5_IRQn 0 */
  HAL_UART_IRQHandler(&huart5);
  /* USER CODE BEGIN UART5_IRQn 1 */

  /* USER CODE END UART5_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */
    
  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void DMA2_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc2);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */

  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */
	extern uint8_t SPIFlashDmaTCFlag;
	extern DMA_HandleTypeDef hdma_spi4_rx;
  /* USER CODE END DMA2_Stream3_IRQn 0 */
  /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */
	// SPI4 传输完成中断
	if(__HAL_DMA_GET_FLAG(&hdma_spi4_rx,DMA_FLAG_TCIF3_7))
	{ 
		//这里先清除TC中断标志位
		__HAL_DMA_CLEAR_FLAG(&hdma_spi4_rx,DMA_FLAG_TCIF3_7);
		
		SPIFlashDmaTCFlag = 1;
	}
  /* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream4 global interrupt.
  */
void DMA2_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream4_IRQn 0 */
	extern uint8_t SPIFlashDmaTCFlag;
	extern DMA_HandleTypeDef hdma_spi4_tx;
  /* USER CODE END DMA2_Stream4_IRQn 0 */
  /* USER CODE BEGIN DMA2_Stream4_IRQn 1 */
	//传输完成中断
	if(__HAL_DMA_GET_FLAG(&hdma_spi4_tx,DMA_FLAG_TCIF0_4))
	{ 
		//这里先清除TC中断标志位
		__HAL_DMA_CLEAR_FLAG(&hdma_spi4_tx,DMA_FLAG_TCIF0_4);
		
		SPIFlashDmaTCFlag = 1;
	}
  /* USER CODE END DMA2_Stream4_IRQn 1 */
}

/**
  * @brief This function handles USB On The Go FS global interrupt.
  */
void OTG_FS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_FS_IRQn 0 */

  /* USER CODE END OTG_FS_IRQn 0 */
  HAL_HCD_IRQHandler(&hhcd_USB_OTG_FS);
  /* USER CODE BEGIN OTG_FS_IRQn 1 */

  /* USER CODE END OTG_FS_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream5 global interrupt.
  */
void DMA2_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream5_IRQn 0 */

  /* USER CODE END DMA2_Stream5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA2_Stream5_IRQn 1 */

  /* USER CODE END DMA2_Stream5_IRQn 1 */
}

/**
  * @brief This function handles LTDC global interrupt.
  */
void LTDC_IRQHandler(void)
{
  /* USER CODE BEGIN LTDC_IRQn 0 */

  /* USER CODE END LTDC_IRQn 0 */
  HAL_LTDC_IRQHandler(&hltdc);
  /* USER CODE BEGIN LTDC_IRQn 1 */

  /* USER CODE END LTDC_IRQn 1 */
}

/* USER CODE BEGIN 1 */
/*中断回调函数区******************************************************************************/
/***
*所有串口接收完成回调函数
***/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	extern osSemaphoreId_t SemphrBinPrinterRxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern volatile CAT_ReceBuf_s CATReceBuf;
	extern PrinterReceBuf_s	PrinterReceBuf;
	extern HAL_StatusTypeDef SystemStartFlag;
	
	
	if(SystemStartFlag == HAL_OK){
		if(huart->Instance == UART5){		
			PrinterReceBuf.writeIndex = (PRINTER_RECE_BUF_LEN+1)-huart5.hdmarx->Instance->NDTR;
			osSemaphoreRelease(SemphrBinPrinterRxHandle);
		}else if(huart->Instance == UART4){
			CATReceBuf.writeIndex = (CAT_RECE_BUF_SIZE+1)-huart4.hdmarx->Instance->NDTR;
				
			osSemaphoreRelease(CATSemphrBinRxHandle);
		}
	}
}


/***
*串口接收半传输完成回调函数
***/
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	extern osSemaphoreId_t SemphrBinPrinterRxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern volatile CAT_ReceBuf_s CATReceBuf;
	extern PrinterReceBuf_s	PrinterReceBuf;
	extern HAL_StatusTypeDef SystemStartFlag;
	
	if(SystemStartFlag == HAL_OK){
		if(huart->Instance == UART4){
			CATReceBuf.writeIndex = (CAT_RECE_BUF_SIZE+1)-huart4.hdmarx->Instance->NDTR;
				
			osSemaphoreRelease(CATSemphrBinRxHandle);
		}else if(huart->Instance == UART5){
			PrinterReceBuf.writeIndex = (PRINTER_RECE_BUF_LEN+1)-huart5.hdmarx->Instance->NDTR;
			
			osSemaphoreRelease(SemphrBinPrinterRxHandle);
		}
	}
}


/***
*串口发送完成回调函数
***/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern HAL_StatusTypeDef SystemStartFlag;
	
	if(SystemStartFlag == HAL_OK){
		if(huart->Instance == UART5){			//判断是串口5的触发的发送完成回调
			//释放发送完成信号量
			osSemaphoreRelease(SemphrCountPrinterTxHandle);
		}
	}
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
