/**
  ******************************************************************************
  * File Name          : LTDC.h
  * Description        : This file provides code for the configuration
  *                      of the LTDC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ltdc_H
#define __ltdc_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#define LCD_WIDTH_PIXEL	800
#define LCD_HIGHT_PIXEL 480

#define LTDC_LAYER1_ADDR 	LTDC_LAYER_START_ADR	 


/*
*	定义显示屏方向，横屏 或 竖屏
*/
typedef enum {
	LCD_DIR_V = 0,
	LCD_DIR_H = 1,
	LCD_DIR_NONE = 2,
	
} LCD_DIR_e;

/* USER CODE END Includes */

extern LTDC_HandleTypeDef hltdc;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_LTDC_Init(void);

/* USER CODE BEGIN Prototypes */
void LTDC_Custon_Define(void);
uint32_t LCD_LL_GetPixelformat(uint32_t LayerIndex);

//uint32_t LTDC_Read_Point(uint16_t usX, uint16_t usY);
//void LTDC_Draw_Point(uint16_t usX, uint16_t usY, uint16_t ulColor);
//void LTDC_Draw_VLine(uint16_t usX, uint16_t usY, uint16_t usLen, uint16_t ulColor);
//void LTDC_Draw_HLine(uint16_t usX, uint16_t usY, uint16_t usLen, uint16_t ulColor);

//void LTDC_Draw_Line(uint16_t usSX, uint16_t usSY, uint16_t usEX, uint16_t usEY, uint16_t ulColor);
//void LTDC_Draw_Rect(uint16_t usSX, uint16_t usSY, uint16_t usEX, uint16_t usEY, uint16_t ulColor);
//void LTDC_Fill_Rect(uint16_t usSX, uint16_t usSY, uint16_t usEX, uint16_t usEY, uint16_t ulColor);
//void LTDC_Clear(uint16_t ulColor);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ ltdc_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
