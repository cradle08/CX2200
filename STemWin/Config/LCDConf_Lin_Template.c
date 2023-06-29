/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.44 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf_Lin_Template.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license SLA0044,
  * the "License"; You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *                      http://www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "GUI.h"
#include "GUIDRV_Lin.h"

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/
#include "main.h"
#include "ltdc.h"
#include "dma2d.h"
#include "LCDConf_Lin_Template.h"
#include "GUIConf.h"

// Physical display size
//
////// define at GUIConf.h
//#define XSIZE_PHYS 	LCD_WIDTH_PIXEL //800
//#define YSIZE_PHYS  LCD_HIGHT_PIXEL	//480

//
// Color conversion
//
//#define COLOR_CONVERSION GUICC_8888

//
// Display driver
//
//#define DISPLAY_DRIVER GUIDRV_WIN32

//
// Buffers / VScreens
//
#define NUM_BUFFERS  3 // Number of multiple buffers to be used
#define NUM_VSCREENS 1 // Number of virtual screens to be used

#define BK_COLOR GUI_DARKBLUE

#undef  GUI_NUM_LAYERS
#define GUI_NUM_LAYERS 1


#define COLOR_CONVERSION_0 GUICC_M565
#define DISPLAY_DRIVER_0   GUIDRV_LIN_16


#if (GUI_NUM_LAYERS > 1)
  #define COLOR_CONVERSION_1 GUICC_M565
  #define DISPLAY_DRIVER_1   GUIDRV_LIN_16
#endif

#define LCD_LAYER0_FRAME_BUFFER  ((uint32_t)LTDC_LAYER1_ADDR)
//#define LCD_LAYER1_FRAME_BUFFER  ((uint32_t)LTDC_LAYER2_ADDR)


/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VRAM_ADDR
  #define VRAM_ADDR 0 // TBD by customer: This has to be the frame buffer start address
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
//  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
//  #error No display driver defined!
#endif
#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif


/** @defgroup STEMWIN_LIBRARY_Private_Macros
* @{
*/ 
/* Redirect bulk conversion to DMA2D routines */
#define DEFINEDMA2D_COLORCONVERSION(PFIX, PIXELFORMAT)                                                             \
static void Color2IndexBulk_##PFIX##DMA2D(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex) { \
  DMA2D_Color2IndexBulk(pColor, pIndex, NumItems, SizeOfIndex, PIXELFORMAT);                                         \
}                                                                                                                   \
static void Index2ColorBulk_##PFIX##DMA2D(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex) { \
  DMA2D_Index2ColorBulk(pIndex, pColor, NumItems, SizeOfIndex, PIXELFORMAT);  \
}

  
/** @defgroup STEMWIN_LIBRARY_Private_Variables
* @{
*/
LCD_LayerPropTypedef layer_prop[GUI_NUM_LAYERS];

static const LCD_API_COLOR_CONV * apColorConvAPI[] = 
{
  COLOR_CONVERSION_0,
#if GUI_NUM_LAYERS > 1
  COLOR_CONVERSION_1,
#endif
};

//static U32 aBufferDMA2D [XSIZE_PHYS * sizeof(U32)];  // 是否为sizeof(U16)
//static U32 aBuffer_FG   [XSIZE_PHYS * sizeof(U32)];
//static U32 aBuffer_BG   [XSIZE_PHYS * sizeof(U32)];



/** @defgroup LCD CONFIGURATION_Private_FunctionPrototypes
* @{
*/ 
static void     DMA2D_CopyBuffer         (U32 LayerIndex, void * pSrc, void * pDst, U32 xSize, U32 ySize, U32 OffLineSrc, U32 OffLineDst);
static void     DMA2D_FillBuffer(U32 LayerIndex, void * pDst, U32 xSize, U32 ySize, U32 OffLine, U32 ColorIndex);
static void     LCD_LL_Init(void); 
static void     LCD_LL_LayerInit(U32 LayerIndex); 

static void     CUSTOM_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst);
static void     CUSTOM_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize);
static void     CUSTOM_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex);
static void     CUSTOM_DrawBitmap8bpp(int LayerIndex, int x, int y, U8 const * p,  int xSize, int ySize, int BytesPerLine);
static void     CUSTOM_DrawBitmap16bpp(int LayerIndex, int x, int y, U8 const * p,  int xSize, int ySize, int BytesPerLine);
static void     CUSTOM_DrawBitmap32bpp(int LayerIndex, int x, int y, U8 const * p,  int xSize, int ySize, int BytesPerLine);
static U32      GetBufferSize(U32 LayerIndex);




/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*   
*/
void LCD_X_Config(void) {
	
	uint32_t i;
  
  LCD_LL_Init();
  //
  // At first initialize use of multiple buffers on demand
  //
//  #if (NUM_BUFFERS > 1)
//    GUI_MULTIBUF_Config(NUM_BUFFERS); 
//  #endif
  
	#if (NUM_BUFFERS > 1)
	for(i = 0; i < GUI_NUM_LAYERS; i++)
	{
		GUI_MULTIBUF_ConfigEx(i, NUM_BUFFERS); 
	}
  #endif
  //
  // Set display driver and color conversion for 1st layer
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_0, COLOR_CONVERSION_0, 0, 0);

  // Display driver configuration, required for Lin-driver
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
  }
  //LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR);
	
	#if (GUI_NUM_LAYERS > 1)
		GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_1, COLOR_CONVERSION_1, 0, 0);
	    /* Set size of 2nd layer */
    if (LCD_GetSwapXYEx(1)) {
      LCD_SetSizeEx (1, YSIZE_PHYS, XSIZE_PHYS);
      LCD_SetVSizeEx(1, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
    } else {
      LCD_SetSizeEx (1, XSIZE_PHYS, YSIZE_PHYS);
      LCD_SetVSizeEx(1, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
    }
  #endif
  //
  // Set user palette data (only required if no fixed palette is used)
  //
  #if defined(PALETTE)
    LCD_SetLUTEx(0, PALETTE);
  #endif
  
	  /*Initialize GUI Layer structure */
    layer_prop[0].address = LCD_LAYER0_FRAME_BUFFER;
    
#if (GUI_NUM_LAYERS > 1)    
    layer_prop[1].address = LCD_LAYER1_FRAME_BUFFER; 
#endif

   /* Setting up VRam address and custom functions for CopyBuffer-, CopyRect- and FillRect operations */
  for (i = 0; i < GUI_NUM_LAYERS; i++) 
  {
    layer_prop[i].pColorConvAPI = (LCD_API_COLOR_CONV *)apColorConvAPI[i];
     
    layer_prop[i].pending_buffer = -1;

    /* Set VRAM address */
    LCD_SetVRAMAddrEx(i, (void *)(layer_prop[i].address));

    /* Remember color depth for further operations */
    layer_prop[i].BytesPerPixel = 2;//LCD_GetBitsPerPixelEx(i) >> 3;

    /* Set custom functions for several operations */
    LCD_SetDevFunc(i, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))CUSTOM_CopyBuffer);
    LCD_SetDevFunc(i, LCD_DEVFUNC_COPYRECT,   (void(*)(void))CUSTOM_CopyRect);
    LCD_SetDevFunc(i, LCD_DEVFUNC_FILLRECT, (void(*)(void))CUSTOM_FillRect);

		LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_8BPP, (void(*)(void))CUSTOM_DrawBitmap8bpp); 
		LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_16BPP, (void(*)(void))CUSTOM_DrawBitmap16bpp);
		
    /* Set up drawing routine for 32bpp bitmap using DMA2D */
    if (LCD_LL_GetPixelformat(i) == LTDC_PIXEL_FORMAT_ARGB8888) {
     LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_32BPP, (void(*)(void))CUSTOM_DrawBitmap32bpp);     /* Set up drawing routine for 32bpp bitmap using DMA2D. Makes only sense with ARGB8888 */
    }    
  }
//  
//  /*电阻屏驱动校准， TOUCH_AD_RIGHT/LEFT/BOTTON/TOP为四点的AD值，每个电阻屏的值不一样*/
//  //于配置触摸屏方向
//  GUI_TOUCH_SetOrientation((GUI_MIRROR_X * LCD_GetMirrorXEx(0)) | (GUI_MIRROR_Y * LCD_GetMirrorYEx(0)) | (GUI_SWAP_XY * LCD_GetSwapXYEx (0))); 
//  //校准X轴，坐标值及对应的AD值
//  GUI_TOUCH_Calibrate(GUI_COORD_X, 0, XSIZE_PHYS-1, TOUCH_AD_RIGHT , TOUCH_AD_LEFT ); 
//  //校准Y轴，坐标值及对应的AD值
//  GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, YSIZE_PHYS-1, TOUCH_AD_BOTTOM ,  TOUCH_AD_TOP );
//  
  
  //
  // Set custom functions for several operations to optimize native processes
  //
//  LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))CUSTOM_LCD_CopyBuffer);
//  LCD_SetDevFunc(0, LCD_DEVFUNC_COPYRECT,   (void(*)(void))CUSTOM_LCD_CopyRect);
//  LCD_SetDevFunc(0, LCD_DEVFUNC_FILLRECT, (void(*)(void))CUSTOM_LCD_FillRect);
//  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_8BPP, (void(*)(void))CUSTOM_LCD_DrawBitmap8bpp); 
//  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_16BPP, (void(*)(void))CUSTOM_LCD_DrawBitmap16bpp);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if 
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
__IO uint8_t g_bLCDRefresh = 0;
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;
  U32 addr;
  int xPos, yPos;
  U32 Color;
	
  switch (Cmd) {
  case LCD_X_INITCONTROLLER: 
	{
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    // ...
		LCD_LL_LayerInit(LayerIndex);
    //return 0;
  }
	break;
  case LCD_X_SETVRAMADDR: 
	{
    //
    // Required for setting the address of the video RAM for drivers
    // with memory mapped video RAM which is passed in the 'pVRAM' element of p
    //
//    LCD_X_SETVRAMADDR_INFO * p;
//    p = (LCD_X_SETVRAMADDR_INFO *)pData;
    //...
    //return 0;
  }
	break;
  case LCD_X_SETORG: 
	{
    //
    // Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
    //
//    LCD_X_SETORG_INFO * p;
//    p = (LCD_X_SETORG_INFO *)pData;
    //...
		addr = layer_prop[LayerIndex].address + ((LCD_X_SETORG_INFO *)pData)->yPos * layer_prop[LayerIndex].xSize * layer_prop[LayerIndex].BytesPerPixel;
    HAL_LTDC_SetAddress(&hltdc, addr, LayerIndex);
    //return 0;
  }
	break;
  case LCD_X_SHOWBUFFER: 
	{
    //
    // Required if multiple buffers are used. The 'Index' element of p contains the buffer index.
    //
//    LCD_X_SHOWBUFFER_INFO * p;
//    p = (LCD_X_SHOWBUFFER_INFO *)pData;
    //...
		layer_prop[LayerIndex].pending_buffer = ((LCD_X_SHOWBUFFER_INFO *)pData)->Index;
    //return 0;
  }
	break;
  case LCD_X_SETLUTENTRY: 
	{
    //
    // Required for setting a lookup table entry which is passed in the 'Pos' and 'Color' element of p
    //
//    LCD_X_SETLUTENTRY_INFO * p;
//    p = (LCD_X_SETLUTENTRY_INFO *)pData;
    //...
		HAL_LTDC_ConfigCLUT(&hltdc, (uint32_t *)&(((LCD_X_SETLUTENTRY_INFO *)pData)->Color), 1, LayerIndex);
    //return 0;
  }
	break;
  case LCD_X_ON: 
	{
    //
    // Required if the display controller should support switching on and off
    //
		__HAL_LTDC_ENABLE(&hltdc);
    //return 0;
  }
	break;
  case LCD_X_OFF: 
	{
    //
    // Required if the display controller should support switching on and off
    //
    // ...
		 __HAL_LTDC_DISABLE(&hltdc);
    //return 0;
  }
	break;
	
	//
	case LCD_X_SETVIS:
	{
		if(((LCD_X_SETVIS_INFO *)pData)->OnOff  == ENABLE )
		{
			__HAL_LTDC_LAYER_ENABLE(&hltdc, LayerIndex); 
		}
		else
		{
			__HAL_LTDC_LAYER_DISABLE(&hltdc, LayerIndex); 
		}
		__HAL_LTDC_RELOAD_CONFIG(&hltdc); 
	}
	break;
	
	case LCD_X_SETPOS: 
		HAL_LTDC_SetWindowPosition(&hltdc, 
														 ((LCD_X_SETPOS_INFO *)pData)->xPos, 
														 ((LCD_X_SETPOS_INFO *)pData)->yPos, 
														 LayerIndex);
		break;

	case LCD_X_SETSIZE:
	{
		GUI_GetLayerPosEx(LayerIndex, &xPos, &yPos);
		layer_prop[LayerIndex].xSize = ((LCD_X_SETSIZE_INFO *)pData)->xSize;
		layer_prop[LayerIndex].ySize = ((LCD_X_SETSIZE_INFO *)pData)->ySize;
		HAL_LTDC_SetWindowPosition(&hltdc, xPos, yPos, LayerIndex);
	}
	break;

	case LCD_X_SETALPHA:
		HAL_LTDC_SetAlpha(&hltdc, ((LCD_X_SETALPHA_INFO *)pData)->Alpha, LayerIndex);
		break;

	case LCD_X_SETCHROMAMODE:
	{
		if(((LCD_X_SETCHROMAMODE_INFO *)pData)->ChromaMode != 0)
		{
			HAL_LTDC_EnableColorKeying(&hltdc, LayerIndex);
		}
		else
		{
			HAL_LTDC_DisableColorKeying(&hltdc, LayerIndex);      
		}
	}
	break;

	case LCD_X_SETCHROMA:
	{
		Color = ((((LCD_X_SETCHROMA_INFO *)pData)->ChromaMin & 0xFF0000) >> 16) |\
					 (((LCD_X_SETCHROMA_INFO *)pData)->ChromaMin & 0x00FF00) |\
					((((LCD_X_SETCHROMA_INFO *)pData)->ChromaMin & 0x0000FF) << 16);
	
		HAL_LTDC_ConfigColorKeying(&hltdc, Color, LayerIndex);
	}
	break;
	
  default:
    r = -1;
  }
  return r;
}


static void LCD_LL_LayerInit(U32 LayerIndex) 
{
  LTDC_LayerCfgTypeDef             layer_cfg;
  
  if (LayerIndex < GUI_NUM_LAYERS) 
  {
    /* Layer configuration */
    layer_cfg.WindowX0 = 0;
    layer_cfg.WindowX1 = XSIZE_PHYS;
    layer_cfg.WindowY0 = 0;
    layer_cfg.WindowY1 = YSIZE_PHYS; 
    layer_cfg.PixelFormat = LCD_LL_GetPixelformat(LayerIndex);
    layer_cfg.FBStartAdress = layer_prop[LayerIndex].address;
    layer_cfg.Alpha = 255;
    layer_cfg.Alpha0 = 0;
    layer_cfg.Backcolor.Blue = 0;
    layer_cfg.Backcolor.Green = 0;
    layer_cfg.Backcolor.Red = 0;
    layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    layer_cfg.ImageWidth = XSIZE_PHYS;
    layer_cfg.ImageHeight = YSIZE_PHYS;
    HAL_LTDC_ConfigLayer(&hltdc, &layer_cfg, LayerIndex);  
    
    /* Enable LUT on demand */
    if (LCD_GetBitsPerPixelEx(LayerIndex) <= 8) 
    {
      /* Enable usage of LUT for all modes with <= 8bpp*/
      HAL_LTDC_EnableCLUT(&hltdc, LayerIndex);
    } 
  } 
}


/**
  * @brief  Initialize the LCD Controller.
  * @param  None
  * @retval None
  */
static void LCD_LL_Init(void) 
{
  static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
      /* DeInit */
//      HAL_LTDC_DeInit(&hltdc);
      
      /* Initializaton of ILI9341 component*/
      //ili9341_Init();
      
      /* Set LCD Timings */
//			hltdc.Init.HorizontalSync = 19;
//			hltdc.Init.VerticalSync = 2;
//			hltdc.Init.AccumulatedHBP = 159;
//			hltdc.Init.AccumulatedVBP = 22;
//			hltdc.Init.AccumulatedActiveW = 1183;
//			hltdc.Init.AccumulatedActiveH = 622;
//			hltdc.Init.TotalWidth = 1343;
//			hltdc.Init.TotalHeigh = 634;
//      
//      /* background value */
//      hltdc.Init.Backcolor.Blue = 0;
//      hltdc.Init.Backcolor.Green = 0;
//      hltdc.Init.Backcolor.Red = 0;
      
      /* LCD clock configuration */
      /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 MHz */
      /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 MHz */
      /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/4 = 48 MHz */
      /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_8 = 48/8 = 6 MHz */
//    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
//    PeriphClkInitStruct.PLLSAI.PLLSAIN = 396;
//    PeriphClkInitStruct.PLLSAI.PLLSAIR = 3;
//    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
//    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
						
      /* Polarity */
//      hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
//      hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL; 
//      hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;  
//      hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
//      hltdc.Instance = LTDC;
//      
//      HAL_LTDC_Init(&hltdc);
//      HAL_LTDC_ProgramLineEvent(&hltdc, 0); 


   
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 396;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 3;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

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
    HAL_LTDC_ProgramLineEvent(&hltdc, 0);
    MX_DMA2D_Init();
        //
        
//      
//	  /* Configure the DMA2D  default mode */ 
//			hdma2d.Init.Mode         = DMA2D_R2M;
//			hdma2d.Init.ColorMode    = DMA2D_ARGB8888;
//			hdma2d.Init.OutputOffset = 0x0;     

//			hdma2d.Instance          = DMA2D; 

//			if(HAL_DMA2D_Init(&hdma2d) != HAL_OK)
//			{
//				while (1);
//			}
}

/**
  * @brief  Return Pixel format for a given layer
  * @param  LayerIndex : Layer Index 
  * @retval Status ( 0 : 0k , 1: error)
  */
static void DMA2D_CopyBuffer(U32 LayerIndex, void * pSrc, void * pDst, U32 xSize, U32 ySize, U32 OffLineSrc, U32 OffLineDst)
{
  U32 PixelFormat;

  PixelFormat = LCD_LL_GetPixelformat(LayerIndex);
  DMA2D->CR      = 0x00000000UL | (1 << 9);  
  
  /* Set up pointers */
  DMA2D->FGMAR   = (U32)pSrc;                       
  DMA2D->OMAR    = (U32)pDst;                       
  DMA2D->FGOR    = OffLineSrc;                      
  DMA2D->OOR     = OffLineDst; 
  
  /* Set up pixel format */  
  DMA2D->FGPFCCR = PixelFormat;  
  
  /*  Set up size */
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; 
  
  DMA2D->CR     |= DMA2D_CR_START;   
 
  /* Wait until transfer is done */
  while (DMA2D->CR & DMA2D_CR_START) 
  {
  }
  
}

/**
  * @brief  Fill Buffer
  * @param  LayerIndex : Layer Index
  * @param  pDst:        pointer to destination
  * @param  xSize:       X size
  * @param  ySize:       Y size
  * @param  OffLine:     offset after each line
  * @param  ColorIndex:  color to be used.           
  * @retval None
  */
static void DMA2D_FillBuffer(U32 LayerIndex, void * pDst, U32 xSize, U32 ySize, U32 OffLine, U32 ColorIndex) 
{

  U32 PixelFormat;

  PixelFormat = LCD_LL_GetPixelformat(LayerIndex);

  /* Set up mode */
  DMA2D->CR      = 0x00030000UL | (1 << 9);        
  DMA2D->OCOLR   = ColorIndex;                     

  /* Set up pointers */
  DMA2D->OMAR    = (U32)pDst;                      

  /* Set up offsets */
  DMA2D->OOR     = OffLine;                        

  /* Set up pixel format */
  DMA2D->OPFCCR  = PixelFormat;                    

  /*  Set up size */
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize;
    
  DMA2D->CR     |= DMA2D_CR_START; 
  
  /* Wait until transfer is done */
  while (DMA2D->CR & DMA2D_CR_START) 
  {
  }
}


/**
  * @brief  Get buffer size
  * @param  LayerIndex : Layer Index           
  * @retval None
  */
static U32 GetBufferSize(U32 LayerIndex) 
{
  U32 BufferSize;

  BufferSize = layer_prop[LayerIndex].xSize * layer_prop[LayerIndex].ySize * layer_prop[LayerIndex].BytesPerPixel;
  return BufferSize;
}

/**
  * @brief  Customized copy buffer
  * @param  LayerIndex : Layer Index
  * @param  IndexSrc:    index source
  * @param  IndexDst:    index destination           
  * @retval None
  */
static void CUSTOM_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {
  U32 BufferSize, AddrSrc, AddrDst;

  BufferSize = GetBufferSize(LayerIndex);
  AddrSrc    = layer_prop[LayerIndex].address + BufferSize * IndexSrc;
  AddrDst    = layer_prop[LayerIndex].address + BufferSize * IndexDst;
  DMA2D_CopyBuffer(LayerIndex, (void *)AddrSrc, (void *)AddrDst, layer_prop[LayerIndex].xSize, layer_prop[LayerIndex].ySize, 0, 0);
  layer_prop[LayerIndex].buffer_index = IndexDst;
}

/**
  * @brief  Copy rectangle
  * @param  LayerIndex : Layer Index
  * @param  x0:          X0 position
  * @param  y0:          Y0 position
  * @param  x1:          X1 position
  * @param  y1:          Y1 position
  * @param  xSize:       X size. 
  * @param  ySize:       Y size.            
  * @retval None
  */
static void CUSTOM_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize) 
{
  U32 AddrSrc, AddrDst;  

  AddrSrc = layer_prop[LayerIndex].address + (y0 * layer_prop[LayerIndex].xSize + x0) * layer_prop[LayerIndex].BytesPerPixel;
  AddrDst = layer_prop[LayerIndex].address + (y1 * layer_prop[LayerIndex].xSize + x1) * layer_prop[LayerIndex].BytesPerPixel;
  DMA2D_CopyBuffer(LayerIndex, (void *)AddrSrc, (void *)AddrDst, xSize, ySize, layer_prop[LayerIndex].xSize - xSize, layer_prop[LayerIndex].xSize - xSize);
}

/**
  * @brief  Fill rectangle
  * @param  LayerIndex : Layer Index
  * @param  x0:          X0 position
  * @param  y0:          Y0 position
  * @param  x1:          X1 position
  * @param  y1:          Y1 position
  * @param  PixelIndex:  Pixel index.             
  * @retval None
  */
static void CUSTOM_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) 
{
  U32 BufferSize, AddrDst;
  int xSize, ySize;

  if (GUI_GetDrawMode() == GUI_DM_XOR) 
  {
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, NULL);
    LCD_FillRect(x0, y0, x1, y1);
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, (void(*)(void))CUSTOM_FillRect);
  } 
  else 
  {
    xSize = x1 - x0 + 1;
    ySize = y1 - y0 + 1;
    BufferSize = GetBufferSize(LayerIndex);
    AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y0 * layer_prop[LayerIndex].xSize + x0) * layer_prop[LayerIndex].BytesPerPixel;
    DMA2D_FillBuffer(LayerIndex, (void *)AddrDst, xSize, ySize, layer_prop[LayerIndex].xSize - xSize, PixelIndex);
  }
}


/**
  * @brief  Draw indirect color bitmap
  * @param  pSrc: pointer to the source
  * @param  pDst: pointer to the destination
  * @param  OffSrc: offset source
  * @param  OffDst: offset destination
  * @param  PixelFormatDst: pixel format for destination
  * @param  xSize: X size
  * @param  ySize: Y size
  * @retval None
  */
static void CUSTOM_DrawBitmap8bpp(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine)
{
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;

  BufferSize = GetBufferSize(LayerIndex);
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y * layer_prop[LayerIndex].xSize + x) * layer_prop[LayerIndex].BytesPerPixel;
  OffLineSrc = BytesPerLine - xSize;
  OffLineDst = layer_prop[LayerIndex].xSize - xSize;
  DMA2D_CopyBuffer(LayerIndex, (void *)p, (void *)AddrDst, xSize, ySize, OffLineSrc, OffLineDst);
}


/**
  * @brief  Draw indirect color bitmap
  * @param  pSrc: pointer to the source
  * @param  pDst: pointer to the destination
  * @param  OffSrc: offset source
  * @param  OffDst: offset destination
  * @param  PixelFormatDst: pixel format for destination
  * @param  xSize: X size
  * @param  ySize: Y size
  * @retval None
  */
static void CUSTOM_DrawBitmap16bpp(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine)
{
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;

  BufferSize = GetBufferSize(LayerIndex);
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y * layer_prop[LayerIndex].xSize + x) * layer_prop[LayerIndex].BytesPerPixel;
  OffLineSrc = (BytesPerLine / 2) - xSize;
  OffLineDst = layer_prop[LayerIndex].xSize - xSize;
  DMA2D_CopyBuffer(LayerIndex, (void *)p, (void *)AddrDst, xSize, ySize, OffLineSrc, OffLineDst);
}


/**
  * @brief  Draw indirect color bitmap
  * @param  pSrc: pointer to the source
  * @param  pDst: pointer to the destination
  * @param  OffSrc: offset source
  * @param  OffDst: offset destination
  * @param  PixelFormatDst: pixel format for destination
  * @param  xSize: X size
  * @param  ySize: Y size
  * @retval None
  */
static void CUSTOM_DrawBitmap32bpp(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine)
{
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;

  BufferSize = GetBufferSize(LayerIndex);
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y * layer_prop[LayerIndex].xSize + x) * layer_prop[LayerIndex].BytesPerPixel;
  OffLineSrc = (BytesPerLine / 4) - xSize;
  OffLineDst = layer_prop[LayerIndex].xSize - xSize;
  DMA2D_CopyBuffer(LayerIndex, (void *)p, (void *)AddrDst, xSize, ySize, OffLineSrc, OffLineDst);
}





/*************************** End of file ****************************/
