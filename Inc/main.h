/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "cmsis_os2.h"
#include "bsp_outin.h"


//版本号：规则：数值101对应的显示版本为：V1.0.1；硬件版本规则：字符串A.01
#define RELEASE_VERSION        				"V1"						//发布版本号
#define FULL_VERSION        				atoi("102")					//完整版本号
#define MAIN_VERSION        				atoi("102")					//主程序版本号
#define BOOTLOAD_VERSION_ADR				(FLASH_BASE+0x3C00)			//BOOTLOADER程序在MCU内部FLASH中的存放地址，这里不要修改此地址，要想修改引导程序版本，需要进入引导程序代码中修改


//PCBA检验程序编译项，正常使用时，下面几项都为0即可
#define PCBA_CHECK_MAIN_BOARD				0							//主控板PCBA检验
#define PCBA_CHECK_ZJ_BOARD					0							//转接板PCBA检验
#define PCBA_CHECK_CAT						0							//CAT模块检验
#define PCBA_CHECK_TOUCH_SCREEN				0							//触摸屏检验
#define PCBA_CHECK_MOTO						0							//步进电机检验
#define PCBA_CHECK_PRESSURE_MONITOR			0							//压力传感器检验
#define PCBA_CHECK_VALVE_PUMP				0							//阀和泵检验
#define AUTO_POWER_ONOFF_SELF_CHECK			0							//自动重复开关机自检



//1:数据和日志写入SPI Flash（直接存储方式）， 0：数据日志写入Nand（通过文件系统的方式）
#define DATA_LOG_SAVE_TO_SPI_FLASH   1

// 1:使用RL-FlashFS文件系统, 0:使用FATFS文件系统（FATFS）
#define USE_RL_FLASH_FS     1


extern Panel_LED_Status_e g_ePanel_LED_Status;         //面板指示灯运行状态，


/*
*   定义函数状态返回
*/
typedef enum {
   FEED_BACK_SUCCESS  = 0,   
   FEED_BACK_FAIL     = 1,   
   FEED_BACK_ERROR    = 2,   
   FEED_BACK_TIMEOUT  = 3,  
   FEED_BACK_NONE     = 4,   // don't need to feedback any frame

} FeedBack_e;
    

/*
*   布尔定义
*/
typedef enum {
    FALSE   = 0,
    TRUE    = !FALSE,

} Bool;



#define CX_DEBUG
/*
*   串口打印日志输出级别
*/
typedef enum {
    LOG_LEVEL_INFO  =  0,
    LOG_LEVEL_DEBUG =  1,
    LOG_LEVEL_WARM  =  2,
    LOG_LEVEL_ERROR =  3,
    LOG_LEVEL_END   =  4,
    
}LogLevel_e;
    


//
extern __IO LogLevel_e  g_eLogLevel;
#define LOG_Info(fmt, ...)		{if(g_eLogLevel <= LOG_LEVEL_INFO) printf(fmt"\r\n", ##__VA_ARGS__);}
#define LOG_Debug(fmt, ...)		{if(g_eLogLevel <= LOG_LEVEL_DEBUG) printf("%s,%d:"fmt"\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);}
#define LOG_Warm(fmt, ...)   	{if(g_eLogLevel <= LOG_LEVEL_WARM) printf("Waring: %s,%d: "fmt"\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);}
#define LOG_Error(fmt, ...)   	{if(g_eLogLevel <= LOG_LEVEL_ERROR) printf("Error: %s,%d: "fmt"\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);}

//
extern __IO osMutexId_t   xMutex_Com;       //公共互斥锁
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
void Delay_US(uint32_t ulUs);
Bool Delay_Until(uint32_t ulTime);
void App_Delay(RunMode_e eRunMode, uint32_t ulTime);
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void Disable_Motor_Task(void);
void Enable_Motor_Task(Bool eFlag);
void Disable_Algo_Task(void);
void Enable_Algo_Task(Bool eFlag);
void Disable_FrontEnd_Task(void);
void Enable_FrontEnd_Task(void);
void Soft_Reboot(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LTDC_LAYER_START_ADR 0xC0000000
#define DR0_SPI_CS_Pin GPIO_PIN_3
#define DR0_SPI_CS_GPIO_Port GPIOE
#define DR1_SPI_CS_Pin GPIO_PIN_4
#define DR1_SPI_CS_GPIO_Port GPIOE
#define OUTIN_KEY_Pin GPIO_PIN_8
#define OUTIN_KEY_GPIO_Port GPIOI
#define OUTIN_KEY_EXTI_IRQn EXTI9_5_IRQn
#define BC_COUNT_KEY_Pin GPIO_PIN_13
#define BC_COUNT_KEY_GPIO_Port GPIOC
#define BC_COUNT_KEY_EXTI_IRQn EXTI15_10_IRQn
#define MENU_GREEN_Pin GPIO_PIN_14
#define MENU_GREEN_GPIO_Port GPIOC
#define MENU_RED_Pin GPIO_PIN_15
#define MENU_RED_GPIO_Port GPIOC
#define NAND_WP_Pin GPIO_PIN_11
#define NAND_WP_GPIO_Port GPIOI
#define ELEC_AIN_Pin GPIO_PIN_6
#define ELEC_AIN_GPIO_Port GPIOF
#define BC_CURRENT_SW_Pin GPIO_PIN_7
#define BC_CURRENT_SW_GPIO_Port GPIOF
#define NTC_AIN_Pin GPIO_PIN_8
#define NTC_AIN_GPIO_Port GPIOF
#define BC_CUR_AIN_Pin GPIO_PIN_9
#define BC_CUR_AIN_GPIO_Port GPIOF
#define XK_AIN_Pin GPIO_PIN_1
#define XK_AIN_GPIO_Port GPIOC
#define P12V_AIN_Pin GPIO_PIN_2
#define P12V_AIN_GPIO_Port GPIOC
#define N12V_AIN_Pin GPIO_PIN_3
#define N12V_AIN_GPIO_Port GPIOC
#define PRESS_AIN_Pin GPIO_PIN_0
#define PRESS_AIN_GPIO_Port GPIOA
#define BEEP_PWM_Pin GPIO_PIN_1
#define BEEP_PWM_GPIO_Port GPIOA
#define HGB_AIN_Pin GPIO_PIN_2
#define HGB_AIN_GPIO_Port GPIOA
#define RBC_PLT_Pin GPIO_PIN_3
#define RBC_PLT_GPIO_Port GPIOA
#define LIGHT_CUR_DAC_Pin GPIO_PIN_4
#define LIGHT_CUR_DAC_GPIO_Port GPIOA
#define MOTOR_CUR_DAC_Pin GPIO_PIN_5
#define MOTOR_CUR_DAC_GPIO_Port GPIOA
#define WBC_AIN_Pin GPIO_PIN_6
#define WBC_AIN_GPIO_Port GPIOA
#define LIQUID_VALVE_Pin GPIO_PIN_7
#define LIQUID_VALVE_GPIO_Port GPIOA
#define CAT_STATUS_Pin GPIO_PIN_4
#define CAT_STATUS_GPIO_Port GPIOC
#define AIR_VALVE_Pin GPIO_PIN_0
#define AIR_VALVE_GPIO_Port GPIOB
#define LED0_Pin GPIO_PIN_1
#define LED0_GPIO_Port GPIOB
#define CAT_PWRKEY_Pin GPIO_PIN_10
#define CAT_PWRKEY_GPIO_Port GPIOB
#define CAT_RESET_Pin GPIO_PIN_11
#define CAT_RESET_GPIO_Port GPIOB
#define LIGHT_SW_Pin GPIO_PIN_6
#define LIGHT_SW_GPIO_Port GPIOH
#define I2C3_SCL_Pin GPIO_PIN_7
#define I2C3_SCL_GPIO_Port GPIOH
#define I2C_SDA_Pin GPIO_PIN_8
#define I2C_SDA_GPIO_Port GPIOH
#define COUNT_CELL_CHECK_Pin GPIO_PIN_12
#define COUNT_CELL_CHECK_GPIO_Port GPIOB
#define COUNT_CELL_TYPE_CHECK_Pin GPIO_PIN_13
#define COUNT_CELL_TYPE_CHECK_GPIO_Port GPIOB
#define FLASH_SPI_CS_Pin GPIO_PIN_15
#define FLASH_SPI_CS_GPIO_Port GPIOB
#define COUNT_CELL_SW_Pin GPIO_PIN_13
#define COUNT_CELL_SW_GPIO_Port GPIOD
#define COUNT_CELL_TYPE_SW_Pin GPIO_PIN_3
#define COUNT_CELL_TYPE_SW_GPIO_Port GPIOG
#define OUTIN_MOTOR_PWM_Pin GPIO_PIN_6
#define OUTIN_MOTOR_PWM_GPIO_Port GPIOC
#define CLAMP_OUT_OC_Pin GPIO_PIN_7
#define CLAMP_OUT_OC_GPIO_Port GPIOC
#define CLAMP_IN_OC_Pin GPIO_PIN_8
#define CLAMP_IN_OC_GPIO_Port GPIOC
#define OUTIN_OUT_OC_Pin GPIO_PIN_9
#define OUTIN_OUT_OC_GPIO_Port GPIOC
#define OUTIN_IN_OC_Pin GPIO_PIN_8
#define OUTIN_IN_OC_GPIO_Port GPIOA
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA
#define ELEC_Pin GPIO_PIN_3
#define ELEC_GPIO_Port GPIOI
#define CLAMP_MOTOR_PWM_Pin GPIO_PIN_15
#define CLAMP_MOTOR_PWM_GPIO_Port GPIOA
#define PRINT_UART5_TX_Pin GPIO_PIN_12
#define PRINT_UART5_TX_GPIO_Port GPIOC
#define PRINT_UART5_RX_Pin GPIO_PIN_2
#define PRINT_UART5_RX_GPIO_Port GPIOD
#define COUNT_SIGNAL_SW_Pin GPIO_PIN_3
#define COUNT_SIGNAL_SW_GPIO_Port GPIOD
#define LCD_RST_Pin GPIO_PIN_7
#define LCD_RST_GPIO_Port GPIOD
#define XPT_PENIRQ_Pin GPIO_PIN_10
#define XPT_PENIRQ_GPIO_Port GPIOG
#define TP_SPI_CS_Pin GPIO_PIN_12
#define TP_SPI_CS_GPIO_Port GPIOG
#define OUTIN_MOTOR_EN_Pin GPIO_PIN_3
#define OUTIN_MOTOR_EN_GPIO_Port GPIOB
#define OUTIN_MOTOR_DIR_Pin GPIO_PIN_4
#define OUTIN_MOTOR_DIR_GPIO_Port GPIOB
#define LCD_BL_Pin GPIO_PIN_5
#define LCD_BL_GPIO_Port GPIOB
#define PUMP_PWM_Pin GPIO_PIN_6
#define PUMP_PWM_GPIO_Port GPIOB
#define LCD_PWR_Pin GPIO_PIN_7
#define LCD_PWR_GPIO_Port GPIOB
#define CLAMP_MOTOR_EN_Pin GPIO_PIN_8
#define CLAMP_MOTOR_EN_GPIO_Port GPIOB
#define CLAMP_MOTOR_DIR_Pin GPIO_PIN_9
#define CLAMP_MOTOR_DIR_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
