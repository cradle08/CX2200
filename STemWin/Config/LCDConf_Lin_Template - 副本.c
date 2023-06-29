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

#include "LCDConf_Lin_Template.h"
#include "stm32f4xx_hal_ltdc.h"
#include "stm32f429xx.h"
// Physical display size

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

//typedef struct {
//	LTDC_Layer_TypeDef * pstLTDC_Layer[2];
//	uint32_t 						 ulFrameAddr[2];
//	uint32_t						 ulPendingBuffer[GUI_NUM_LAYERS];
//	uint32_t						 ulBufferIndex[GUI_NUM_LAYERS];
//	uint32_t						 ulXSize[GUI_NUM_LAYERS];
//	uint32_t						 ulYSize[GUI_NUM_LAYERS];
//	uint32_t						 ulBytesPrePixels[GUI_NUM_LAYERS];
//} LCD_t;


LCDData_t g_stLCDData;

static const LCD_API_COLOR_CONV * _apColorConvAPI[] = {
  COLOR_CONVERSION_0,
#if GUI_NUM_LAYERS > 1
  COLOR_CONVERSION_1,
#endif
};





void LCDData_Init(void)
{
	char i = 0;
	
	g_stLCDData.pstLTDC_Layer[0] = LTDC_Layer1;
	g_stLCDData.pstLTDC_Layer[1] = LTDC_Layer2;
	g_stLCDData.ulFrameAddr[0] 	 = GUI_FRAME1_BUFFER_ADDR;
	g_stLCDData.ulFrameAddr[1]	 = GUI_FRAME2_BUFFER_ADDR;
	
	for(i = 0; i < GUI_NUM_LAYERS; i++)
	{
		g_stLCDData.ulPendingBuffer[i]  = 0;
		g_stLCDData.ulBufferIndex[i]    = 0;
		g_stLCDData.ulXSize[i]          = XSIZE_PHYS;
		g_stLCDData.ulYSize[i]					= YSIZE_PHYS;
		g_stLCDData.ulBytesPrePixels[i] = 2;
	}
}


//int Bytes_Pre_Pixel(int ulColorFormat)
//{
//	uint8_t ucByteNo = 0;
//	switch(ulColorFormat)
//	{
//		case _CM_ARGB8888:
//		{
//			ucByteNo = 4;
//		}
//		break;
//		
//		case _CM_RGB888:
//		{
//			ucByteNo = 4;
//		}
//		break;		
//	
//		case _CM_RGB565:
//		{
//			ucByteNo = 2;
//		}
//		break;

//		case _CM_ARGB1555:
//		{
//			ucByteNo = 4;
//		}
//		break;

//		case _CM_ARGB4444:
//		{
//			ucByteNo = 4;
//		}
//		break;

//		case _CM_L8:
//		{
//			ucByteNo = 4;
//		}
//		break;

//		case _CM_AL44:
//		{
//			ucByteNo = 4;
//		}
//		break;		
//		
//		case _CM_AL88:
//		{
//			ucByteNo = 4;
//		}
//		break;
//		default:break;
//	}
//}

//int Frame_Buffer_Size(int Layer)
//{
//	return Bytes_Pre_Pixel(COLOR_MODE_0);
//}




/*
*
*       _GetPixelformat
*/
static U32 _GetPixelformat(int LayerIndex) {
  const LCD_API_COLOR_CONV * pColorConvAPI;

  if (LayerIndex >= GUI_COUNTOF(_apColorConvAPI)) {
    return 0;
  }
  pColorConvAPI = _apColorConvAPI[LayerIndex];
  if        (pColorConvAPI == GUICC_M8888I) {
    return LTDC_PIXEL_FORMAT_ARGB8888;
  } else if (pColorConvAPI == GUICC_M888) {
    return LTDC_PIXEL_FORMAT_RGB888;
  } else if (pColorConvAPI == GUICC_M565) {
    return LTDC_PIXEL_FORMAT_RGB565;
  } else if (pColorConvAPI == GUICC_M1555I) {
    return LTDC_PIXEL_FORMAT_ARGB1555;
  } else if (pColorConvAPI == GUICC_M4444I) {
    return LTDC_PIXEL_FORMAT_ARGB4444;
  } else if (pColorConvAPI == GUICC_8666) {
    return LTDC_PIXEL_FORMAT_L8;
  } else if (pColorConvAPI == GUICC_1616I) {
    return LTDC_PIXEL_FORMAT_AL44;
  } else if (pColorConvAPI == GUICC_88666I) {
    return LTDC_PIXEL_FORMAT_AL88;
  }
  while (1); // Error
}


/*
*
*       _DMA_Copy
*/
static void _DMA_Copy(int LayerIndex, void * pSrc, void * pDst, int xSize, int ySize, int OffLineSrc, int OffLineDst) {
  U32 PixelFormat;

  PixelFormat = _GetPixelformat(LayerIndex);
  DMA2D->CR      = 0x00000000UL | (1 << 9);         // Control Register (Memory to memory and TCIE)
  DMA2D->FGMAR   = (U32)pSrc;                       // Foreground Memory Address Register (Source address)
  DMA2D->OMAR    = (U32)pDst;                       // Output Memory Address Register (Destination address)
  DMA2D->FGOR    = OffLineSrc;                      // Foreground Offset Register (Source line offset)
  DMA2D->OOR     = OffLineDst;                      // Output Offset Register (Destination line offset)
  DMA2D->FGPFCCR = PixelFormat;                     // Foreground PFC Control Register (Defines the input pixel format)
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; // Number of Line Register (Size configuration of area to be transfered)
  DMA2D->CR     |= 1;                               // Start operation
  //
  // Wait until transfer is done
  //
  while (DMA2D->CR & DMA2D_CR_START) {
    //__WFI();                                        // Sleep until next interrupt
  }
}



/*********************************************************************
*
*       _DMA_DrawBitmapL8
*/
static void _DMA_DrawBitmapL8(void * pSrc, void * pDst,  U32 OffSrc, U32 OffDst, U32 PixelFormatDst, U32 xSize, U32 ySize) {
  //
  // Set up mode
  //
  DMA2D->CR      = 0x00010000UL | (1 << 9);         // Control Register (Memory to memory with pixel format conversion and TCIE)
  //
  // Set up pointers
  //
  DMA2D->FGMAR   = (U32)pSrc;                       // Foreground Memory Address Register (Source address)
  DMA2D->OMAR    = (U32)pDst;                       // Output Memory Address Register (Destination address)
  //
  // Set up offsets
  //
  DMA2D->FGOR    = OffSrc;                          // Foreground Offset Register (Source line offset)
  DMA2D->OOR     = OffDst;                          // Output Offset Register (Destination line offset)
  //
  // Set up pixel format
  //
  DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_L8;             // Foreground PFC Control Register (Defines the input pixel format)
  DMA2D->OPFCCR  = PixelFormatDst;                  // Output PFC Control Register (Defines the output pixel format)
  //
  // Set up size
  //
  DMA2D->NLR     = (U32)(xSize << 16) | ySize;      // Number of Line Register (Size configuration of area to be transfered)
  //
  // Execute operation
  //
  DMA2D->CR     |= 1;                               // Start operation
  //
  // Wait until transfer is done
  //
  while (DMA2D->CR & DMA2D_CR_START) {
    //__WFI();                                        // Sleep until next interrupt
  }
}


/*
*
*/
U32 GetBufferSize(LayerIndex)
{
	return (g_stLCDData.ulXSize[LayerIndex] * g_stLCDData.ulYSize[LayerIndex] * g_stLCDData.ulBytesPrePixels[LayerIndex]);

}

/*
*
*/
void CUSTOM_LCD_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {
  U32 BufferSize, AddrSrc, AddrDst;

  BufferSize = GetBufferSize(LayerIndex);
	
  AddrSrc    = g_stLCDData.ulFrameAddr[LayerIndex] + BufferSize * IndexSrc;
  AddrDst    = g_stLCDData.ulFrameAddr[LayerIndex] + BufferSize * IndexDst;
  _DMA_Copy(LayerIndex, (void *)AddrSrc, (void *)AddrDst, g_stLCDData.ulXSize[LayerIndex], g_stLCDData.ulYSize[LayerIndex], 0, 0);
	g_stLCDData.ulBufferIndex[LayerIndex] = IndexDst;  // After this function has been called all drawing operations are routed to Buffer[IndexDst]!
}


/*
*
*       _DMA_Fill
*/
static void _DMA_Fill(int LayerIndex, void * pDst, int xSize, int ySize, int OffLine, U32 ColorIndex) {
  U32 PixelFormat;

  PixelFormat = _GetPixelformat(LayerIndex);
  DMA2D->CR      = 0x00030000UL | (1 << 9);         // Register to memory and TCIE
  DMA2D->OCOLR   = ColorIndex;                      // Color to be used
  DMA2D->OMAR    = (U32)pDst;                       // Destination address
  DMA2D->OOR     = OffLine;                         // Destination line offset
  DMA2D->OPFCCR  = PixelFormat;                     // Defines the number of pixels to be transfered
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; // Size configuration of area to be transfered
  DMA2D->CR     |= 1;                               // Start operation
  //
  // Wait until transfer is done
  //
  while (DMA2D->CR & DMA2D_CR_START) {
    //__WFI();                                        // Sleep until next interrupt
  }
}


/*
*	
*/
void CUSTOM_LCD_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize) {
  U32 BufferSize, AddrSrc, AddrDst;

  BufferSize = GetBufferSize(LayerIndex);
  AddrSrc = g_stLCDData.ulFrameAddr[LayerIndex] + BufferSize * g_stLCDData.ulBufferIndex[LayerIndex] + (y0 * g_stLCDData.ulXSize[LayerIndex] + x0) * g_stLCDData.ulBytesPrePixels[LayerIndex];
  AddrDst = g_stLCDData.ulFrameAddr[LayerIndex] + BufferSize * g_stLCDData.ulBufferIndex[LayerIndex] + (y1 * g_stLCDData.ulXSize[LayerIndex] + x1) * g_stLCDData.ulBytesPrePixels[LayerIndex];
	_DMA_Copy(LayerIndex, (void *)AddrSrc, (void *)AddrDst, xSize, ySize, g_stLCDData.ulXSize[LayerIndex] - xSize, g_stLCDData.ulXSize[LayerIndex] - xSize);
}


/*
*	
*/
void CUSTOM_LCD_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) {
  U32 BufferSize, AddrDst;
  int xSize, ySize;

  if (GUI_GetDrawMode() == GUI_DM_XOR) {
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, NULL);
    LCD_FillRect(x0, y0, x1, y1);
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, (void(*)(void))CUSTOM_LCD_FillRect);
  } else {
    xSize = x1 - x0 + 1;
    ySize = y1 - y0 + 1;
    BufferSize = GetBufferSize(LayerIndex);
    AddrDst = g_stLCDData.ulFrameAddr[LayerIndex] + BufferSize * g_stLCDData.ulBufferIndex[LayerIndex] + (y0 * g_stLCDData.ulXSize[LayerIndex] + x0) * g_stLCDData.ulBytesPrePixels[LayerIndex];
    _DMA_Fill(LayerIndex, (void *)AddrDst, xSize, ySize, g_stLCDData.ulXSize[LayerIndex] - xSize, PixelIndex);
  }
}


void CUSTOM_LCD_DrawBitmap8bpp(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine) {
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;
  U32 PixelFormat;

  BufferSize = GetBufferSize(LayerIndex);
  AddrDst = g_stLCDData.ulFrameAddr[LayerIndex] + BufferSize * g_stLCDData.ulBufferIndex[LayerIndex] + (y * g_stLCDData.ulXSize[LayerIndex] + x) * g_stLCDData.ulBytesPrePixels[LayerIndex];
  OffLineSrc = BytesPerLine - xSize;
  OffLineDst = g_stLCDData.ulXSize[LayerIndex] - xSize;
  PixelFormat = _GetPixelformat(LayerIndex);
  _DMA_DrawBitmapL8((void *)p, (void *)AddrDst, OffLineSrc, OffLineDst, PixelFormat, xSize, ySize);
}


void CUSTOM_LCD_DrawBitmap16bpp(int LayerIndex, int x, int y, U16 const * p, int xSize, int ySize, int BytesPerLine) {
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;

  BufferSize = GetBufferSize(LayerIndex);
  AddrDst = g_stLCDData.ulFrameAddr[LayerIndex] + BufferSize * g_stLCDData.ulBufferIndex[LayerIndex] + (y * g_stLCDData.ulXSize[LayerIndex] + x) * g_stLCDData.ulBytesPrePixels[LayerIndex];
  OffLineSrc = (BytesPerLine / 2) - xSize;
  OffLineDst = g_stLCDData.ulXSize[LayerIndex] - xSize;
  _DMA_Copy(LayerIndex, (void *)p, (void *)AddrDst, xSize, ySize, OffLineSrc, OffLineDst);
}


/*
*
*/
static void _LTDC_SetLUTEntry(int LayerIndex, U32 Color, int Pos) {
  U32 r, g, b, a;

  r = Color & (0xff << 16);
  g = Color & (0xff <<  8);
  b = Color &  0xff;
  a = Pos << 24;
  g_stLCDData.pstLTDC_Layer[LayerIndex]->CLUTWR &= ~(LTDC_LxCLUTWR_BLUE | LTDC_LxCLUTWR_GREEN | LTDC_LxCLUTWR_RED | LTDC_LxCLUTWR_CLUTADD);
  g_stLCDData.pstLTDC_Layer[LayerIndex]->CLUTWR  = r | g | b | a;
  //
  // Reload configuration
  //
  HAL_LTDC_Reload(&hltdc, LTDC_SRCR_IMR);
}


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
  //
  // At first initialize use of multiple buffers on demand
  //
  #if (NUM_BUFFERS > 1)
    GUI_MULTIBUF_Config(NUM_BUFFERS);
  #endif
  //
  // Set display driver and color conversion for 1st layer
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_0, COLOR_CONVERSION_0, 0, 0);
  //
  // Display driver configuration, required for Lin-driver
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
  }
	LCD_SetVRAMAddrEx(0, (void*)GUI_FRAME1_BUFFER_ADDR);
  //LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR);
  
  // Set user palette data (only required if no fixed palette is used)
  //
  #if defined(PALETTE)
    LCD_SetLUTEx(0, PALETTE);
  #endif
  
  //
  // Set custom functions for several operations to optimize native processes
  //
  LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))CUSTOM_LCD_CopyBuffer);
  LCD_SetDevFunc(0, LCD_DEVFUNC_COPYRECT,   (void(*)(void))CUSTOM_LCD_CopyRect);
  LCD_SetDevFunc(0, LCD_DEVFUNC_FILLRECT, (void(*)(void))CUSTOM_LCD_FillRect);
  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_8BPP, (void(*)(void))CUSTOM_LCD_DrawBitmap8bpp); 
  LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_16BPP, (void(*)(void))CUSTOM_LCD_DrawBitmap16bpp);
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
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  switch (Cmd) {
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    // ...
    return 0;
  }
  case LCD_X_SETVRAMADDR: {
    //
    // Required for setting the address of the video RAM for drivers
    // with memory mapped video RAM which is passed in the 'pVRAM' element of p
    //
    LCD_X_SETVRAMADDR_INFO * p;
    p = (LCD_X_SETVRAMADDR_INFO *)pData;
    //...
    return 0;
  }
  case LCD_X_SETORG: {
    //
    // Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
    //
    LCD_X_SETORG_INFO * p;
    p = (LCD_X_SETORG_INFO *)pData;
    //...
    return 0;
  }
  case LCD_X_SHOWBUFFER: {
    //
    // Required if multiple buffers are used. The 'Index' element of p contains the buffer index.
    //
    LCD_X_SHOWBUFFER_INFO * p;
    p = (LCD_X_SHOWBUFFER_INFO *)pData;
    g_stLCDData.ulPendingBuffer[LayerIndex] = p->Index;
		//...
		
    return 0;
  }
  case LCD_X_SETLUTENTRY: {
    //
    // Required for setting a lookup table entry which is passed in the 'Pos' and 'Color' element of p
    //
    LCD_X_SETLUTENTRY_INFO * p;
    p = (LCD_X_SETLUTENTRY_INFO *)pData;
		p = (LCD_X_SETLUTENTRY_INFO *)pData;
    _LTDC_SetLUTEntry(LayerIndex, p->Color, p->Pos);
    //...
    return 0;
  }
  case LCD_X_ON: {
    //
    // Required if the display controller should support switching on and off
    //
		//__HAL_LTDC_ENABLE(ENABLE);
    return 0;
  }
  case LCD_X_OFF: {
    //
    // Required if the display controller should support switching on and off
    //
    // ...
		//__HAL_LTDC_ENABLE(DISABLE);
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
