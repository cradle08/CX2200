/**
  ******************************************************************************
  * File Name          : LTDC.c
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

/* Includes ------------------------------------------------------------------*/
#include "ltdc.h"

/* USER CODE BEGIN 0 */
#include "LCDConf_Lin_Template.h"
#include "GUI.h"
/* USER CODE END 0 */

LTDC_HandleTypeDef hltdc;

/* LTDC init function */
void MX_LTDC_Init(void)
{
  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 19;
  hltdc.Init.VerticalSync = 9;
  hltdc.Init.AccumulatedHBP = 65;
  hltdc.Init.AccumulatedVBP = 32;
  hltdc.Init.AccumulatedActiveW = 865;
  hltdc.Init.AccumulatedActiveH = 512;
  hltdc.Init.TotalWidth = 1075;
  hltdc.Init.TotalHeigh = 534;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 800;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 480;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 0xff;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg.FBStartAdress = LTDC_LAYER_START_ADR;
  pLayerCfg.ImageWidth = 800;
  pLayerCfg.ImageHeight = 480;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_LTDC_MspInit(LTDC_HandleTypeDef* ltdcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(ltdcHandle->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspInit 0 */

  /* USER CODE END LTDC_MspInit 0 */
    /* LTDC clock enable */
    __HAL_RCC_LTDC_CLK_ENABLE();

    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**LTDC GPIO Configuration
    PI9     ------> LTDC_VSYNC
    PI10     ------> LTDC_HSYNC
    PF10     ------> LTDC_DE
    PH9     ------> LTDC_R3
    PH10     ------> LTDC_R4
    PH11     ------> LTDC_R5
    PH12     ------> LTDC_R6
    PG6     ------> LTDC_R7
    PG7     ------> LTDC_CLK
    PH13     ------> LTDC_G2
    PH14     ------> LTDC_G3
    PH15     ------> LTDC_G4
    PI0     ------> LTDC_G5
    PI1     ------> LTDC_G6
    PI2     ------> LTDC_G7
    PG11     ------> LTDC_B3
    PI4     ------> LTDC_B4
    PI5     ------> LTDC_B5
    PI6     ------> LTDC_B6
    PI7     ------> LTDC_B7
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* LTDC interrupt Init */
    HAL_NVIC_SetPriority(LTDC_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);
  /* USER CODE BEGIN LTDC_MspInit 1 */
//	 HAL_LTDC_ProgramLineEvent(&hltdc, 0);
//	 HAL_LTDC_EnableCLUT(&hltdc, 0);
  /* USER CODE END LTDC_MspInit 1 */
  }
}

void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* ltdcHandle)
{

  if(ltdcHandle->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspDeInit 0 */

  /* USER CODE END LTDC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LTDC_CLK_DISABLE();

    /**LTDC GPIO Configuration
    PI9     ------> LTDC_VSYNC
    PI10     ------> LTDC_HSYNC
    PF10     ------> LTDC_DE
    PH9     ------> LTDC_R3
    PH10     ------> LTDC_R4
    PH11     ------> LTDC_R5
    PH12     ------> LTDC_R6
    PG6     ------> LTDC_R7
    PG7     ------> LTDC_CLK
    PH13     ------> LTDC_G2
    PH14     ------> LTDC_G3
    PH15     ------> LTDC_G4
    PI0     ------> LTDC_G5
    PI1     ------> LTDC_G6
    PI2     ------> LTDC_G7
    PG11     ------> LTDC_B3
    PI4     ------> LTDC_B4
    PI5     ------> LTDC_B5
    PI6     ------> LTDC_B6
    PI7     ------> LTDC_B7
    */
    HAL_GPIO_DeInit(GPIOI, GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11);

    /* LTDC interrupt Deinit */
    HAL_NVIC_DisableIRQ(LTDC_IRQn);
  /* USER CODE BEGIN LTDC_MspDeInit 1 */
	
  /* Enable LTDC reset state */	//添加
  __HAL_RCC_LTDC_FORCE_RESET();
  /* Release LTDC from reset state */ 
  __HAL_RCC_LTDC_RELEASE_RESET();
	
  /* USER CODE END LTDC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/*
*	LTDC补充设置
*/
void LTDC_Custon_Define(void)
{	
//    HAL_LTDC_SetWindowPosition(&hltdc, 0, 0, 0);  //设置窗口的位置
//    HAL_LTDC_SetWindowSize(&hltdc, 1024, 600, 0);//设置窗口大小  
	
//		HAL_LTDC_SetWindowSize_NoReload(&hltdc, 1024, 600, 0);	/* 不立即更新 */
//	HAL_LTDC_SetWindowPosition(&hltdc, 0, 0, 0);		/* 立即更新 */	
}


/**
  * @brief  Line Event callback.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the specified LTDC.
  * @retval None
  */
//__IO uint8_t g_bLCDRefresh = 0;
void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc)
{
  extern LCD_LayerPropTypedef   layer_prop[GUI_NUM_LAYERS];
  U32 Addr;
  U32 layer;

 // g_bLCDRefresh = 1;
  for (layer = 0; layer < GUI_NUM_LAYERS; layer++) 
  {
    if (layer_prop[layer].pending_buffer >= 0) 
    {
      /* Calculate address of buffer to be used  as visible frame buffer */
      Addr = layer_prop[layer].address + \
             layer_prop[layer].xSize * layer_prop[layer].ySize * layer_prop[layer].pending_buffer * layer_prop[layer].BytesPerPixel;
      
      __HAL_LTDC_LAYER(hltdc, layer)->CFBAR = Addr;
     
      __HAL_LTDC_RELOAD_CONFIG(hltdc);
      
      /* Notify STemWin that buffer is used */
      GUI_MULTIBUF_ConfirmEx(layer, layer_prop[layer].pending_buffer);

      /* Clear pending buffer flag of layer */
      layer_prop[layer].pending_buffer = -1;
    }
  }
  
  HAL_LTDC_ProgramLineEvent(hltdc, 0);
//  g_bLCDRefresh = 0;
}


/**
  * @brief  Return Pixel format for a given layer
  * @param  LayerIndex : Layer Index 
  * @retval Status ( 0 : 0k , 1: error)
  */
uint32_t LCD_LL_GetPixelformat(uint32_t LayerIndex)
{
    return LTDC_PIXEL_FORMAT_RGB565;//LTDC_PIXEL_FORMAT_ARGB8888;
}

/*
*	读取像素点
*/
//uint32_t LTDC_Read_Point(uint16_t usX, uint16_t usY)
//{
//	return *(uint16_t*)(g_pulLTDC_Layer[g_stLCD.ucActiveLayer] + g_stLCD.ucColorByte*(g_stLCD.usWidth*usY + usX));
//}


///*
//*	画像素点
//*/
////void LTDC_Draw_Point(uint16_t usX, uint16_t usY, uint16_t ulColor)
////{
////	*(uint16_t*)(g_pulLTDC_Layer[g_stLCD.ucActiveLayer] + g_stLCD.ucColorByte*(g_stLCD.usWidth*usY + usX)) = ulColor;
////}



///*
//*	画线
//*/
//void LTDC_Draw_Line(uint16_t usSX, uint16_t usSY, uint16_t usEX, uint16_t usEY, uint16_t ulColor)
//{
//	
//}



///*
//*	画垂直线
//*/
//void LTDC_Draw_VLine(uint16_t usX, uint16_t usY, uint16_t usLen, uint16_t ulColor)
//{
//	uint16_t usTempLen = 0;
//	
//	//判断长度是否超过显示区域
//	if(usY + usLen <= g_stLCD.usHight)
//	{
//		usTempLen = usLen;
//	}else{
//		usTempLen = g_stLCD.usHight - usY;
//	}
//	
//	//空点，不需要绘制
//	if(usTempLen == 0) return;
//	
//	//画点
//	do{
//		LTDC_Draw_Point(usX, usY + usTempLen, ulColor);
//	}while(--usTempLen);
//}


///*
//*	画水平线
//*/
//void LTDC_Draw_HLine(uint16_t usX, uint16_t usY, uint16_t usLen, uint16_t ulColor)
//{
//	uint16_t usTempLen = 0;
//	
//	//判断长度是否超过显示区域
//	if(usX + usLen <= g_stLCD.usWidth)
//	{
//		usTempLen = usLen;
//	}else{
//		usTempLen = g_stLCD.usHight - usY;
//	}
//	
//	//长度为0，不需要绘制
//	if(usTempLen == 0) return;
//	
//	//画点
//	do{
//		LTDC_Draw_Point(usX + usTempLen, usY, ulColor);
//	}while(--usTempLen);
//}


///*
//*	画矩形
//*/
//void LTDC_Draw_Rect(uint16_t usSX, uint16_t usSY, uint16_t usEX, uint16_t usEY, uint16_t ulColor)
//{
//	uint16_t usXLen = 0, usYLen = 0;
//	
//	//两个坐标为同一个点，画点
//	if(usSX == usEX && usSY == usEY) 
//	{
//		LTDC_Draw_Point(usSX, usSY, ulColor);
//	}
//	
//	//计算两点X轴距离
//	if(usSX <= usEX)
//	{
//		usXLen = usEX - usSX;
//	}else{
//		usXLen = usSX - usEX;
//	}
//	
//	//计算两点Y周距离
//	if(usSY <= usEY)
//	{
//		usYLen = usEY - usSY;
//	}else{
//		usYLen = usSY - usEY;
//	}
//	
//	//画矩形
//	//LTDC_Draw_VLine();
//	
//}



///*
//*	填充矩形
//*/
//void LTDC_Fill_Rect(uint16_t usSX, uint16_t usSY, uint16_t usEX, uint16_t usEY, uint16_t ulColor)
//{
//	
//}

///*
//*	清屏
//*/
//void LTDC_Clear(uint16_t ulColor)
//{
//	uint32_t timeout = 0;
//	uint16_t usW = 0, usH = 0;
//	uint8_t *pData = (uint8_t*)LTDC_LAYER1_ADDR;
//	
//	__HAL_RCC_DMA2D_CLK_ENABLE();	//使能DM2D时钟
//	DMA2D->CR&=~(DMA2D_CR_START);	//先停止DMA2D
//	DMA2D->CR=DMA2D_R2M;			//寄存器到存储器模式
//	DMA2D->OPFCCR=0X02;	//设置颜色格式
//	DMA2D->OOR=0;				//设置行偏移 

//	DMA2D->OMAR=LTDC_LAYER1_ADDR;				//输出存储器地址
//	DMA2D->NLR=(600)|((1024)<<16);	//设定行数寄存器
//	DMA2D->OCOLR=0x04aaaaaa;						//设定输出颜色寄存器 
//	DMA2D->CR|=DMA2D_CR_START;				//启动DMA2D
//	while((DMA2D->ISR&(DMA2D_FLAG_TC))==0)	//等待传输完成
//	{
//		timeout++;
//		if(timeout>0X1FFFFF)break;	//超时退出
//	} 
//	DMA2D->IFCR|=DMA2D_FLAG_TC;		//清除传输完成标志 		



//}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
