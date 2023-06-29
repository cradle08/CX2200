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
File        : LCDConf_Lin_Template.h
Purpose     : Display driver configuration file
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

#ifndef LCDCONF_H
#define LCDCONF_H

#include "main.h"
#include "ltdc.h"
#include "lcd.h"


#define XSIZE_PHYS 	LCD_WIDTH_PIXEL //1024
#define YSIZE_PHYS  LCD_HIGHT_PIXEL	//600

//
// Color conversion
//
//#define COLOR_CONVERSION GUICC_8888
//背景颜色
#define BK_COLOR	GUI_DARKBLUE		
//
// Display driver
//
//#define DISPLAY_DRIVER GUIDRV_WIN32

//
// Buffers / VScreens
//
#define NUM_BUFFERS  3 // Number of multiple buffers to be used
#define NUM_VSCREENS 1 // Number of virtual screens to be used

// 当前使用图层个数
#undef GUI_NUM_LAYERS
#define GUI_NUM_LAYERS	1


//颜色模式
#define _CM_ARGB8888 	1
#define _CM_RGB888 		2
#define _CM_RGB565 		3
#define _CM_ARGB1555 	4
#define _CM_ARGB4444 	5
#define _CM_L8 				6
#define _CM_AL44 			7
#define _CM_AL88 			8


//图层1，颜色模式和分辨率大小
#define COLOR_MODE_0	_CM_RGB565
#define XSIZE_0				XSIZE_PHYS
#define	YSIZE_0				YSIZE_PHYS


//图层2，颜色模式和分辨率大小
#if GUI_NUM_LAYERS > 1
#define COLOR_MODE_1	_CM_RGB565
#define XSIZE_1				XSIZE_PHYS
#define	YSIZE_1				YSIZE_PHYS
#endif




//单图层，颜色模式、驱动
#if (COLOR_MODE_0 == _CM_ARGB8888)
 #define COLOR_CONVERSION_0 GUICC_M8888I
 #define DISPLAY_DRIVER_0 GUIDRV_LIN_32
#elif (COLOR_MODE_0 == _CM_RGB888)
 #define COLOR_CONVERSION_0 GUICC_M888
 #define DISPLAY_DRIVER_0 GUIDRV_LIN_24
#elif (COLOR_MODE_0 == _CM_RGB565)
 #define COLOR_CONVERSION_0 GUICC_M565
 #define DISPLAY_DRIVER_0 GUIDRV_LIN_16
 //
 #define COLOR_CONVERSION	COLOR_CONVERSION_0
 #define DISPLAY_DRIVER   DISPLAY_DRIVER_0
#elif (COLOR_MODE_0 == _CM_ARGB1555)
 #define COLOR_CONVERSION_0 GUICC_M1555I
 #define DISPLAY_DRIVER_0 GUIDRV_LIN_16
#elif (COLOR_MODE_0 == _CM_ARGB4444)
 #define COLOR_CONVERSION_0 GUICC_M4444I
 #define DISPLAY_DRIVER_0 GUIDRV_LIN_16
#elif (COLOR_MODE_0 == _CM_L8)
 #define COLOR_CONVERSION_0 GUICC_8666
 #define DISPLAY_DRIVER_0 GUIDRV_LIN_8
#elif (COLOR_MODE_0 == _CM_AL44)
 #define COLOR_CONVERSION_0 GUICC_1616I
 #define DISPLAY_DRIVER_0 GUIDRV_LIN_8
#elif (COLOR_MODE_0 == _CM_AL88)
 #define COLOR_CONVERSION_0 GUICC_88666I
 #define DISPLAY_DRIVER_0 GUIDRV_LIN_16
#else
 #error Illegal color mode 0!
#endif




/* 双图层情况下，根据用户选择的颜色模式可自动选择图层 2 的 emWin 的驱动和颜色模式 */
#if (GUI_NUM_LAYERS > 1)
#if (COLOR_MODE_1 == _CM_ARGB8888)
 #define COLOR_CONVERSION_1 GUICC_M8888I
 #define DISPLAY_DRIVER_1 GUIDRV_LIN_32
#elif (COLOR_MODE_1 == _CM_RGB888)
 #define COLOR_CONVERSION_1 GUICC_M888
 #define DISPLAY_DRIVER_1 GUIDRV_LIN_24
#elif (COLOR_MODE_1 == _CM_RGB565)
 #define COLOR_CONVERSION_1 GUICC_M565
 #define DISPLAY_DRIVER_1 GUIDRV_LIN_16
#elif (COLOR_MODE_1 == _CM_ARGB1555)
 #define COLOR_CONVERSION_1 GUICC_M1555I
 #define DISPLAY_DRIVER_1 GUIDRV_LIN_16
#elif (COLOR_MODE_1 == _CM_ARGB4444)
 #define COLOR_CONVERSION_1 GUICC_M4444I
 #define DISPLAY_DRIVER_1 GUIDRV_LIN_16
#elif (COLOR_MODE_1 == _CM_L8)
 #define COLOR_CONVERSION_1 GUICC_8666
 #define DISPLAY_DRIVER_1 GUIDRV_LIN_8
#elif (COLOR_MODE_1 == _CM_AL44)
 #define COLOR_CONVERSION_1 GUICC_1616I
 #define DISPLAY_DRIVER_1 GUIDRV_LIN_8
#elif (COLOR_MODE_1 == _CM_AL88)
 #define COLOR_CONVERSION_1 GUICC_88666I
 #define DISPLAY_DRIVER_1 GUIDRV_LIN_16
#else
 #error Illegal color mode 1!
#endif
#else
//
#endif


//图层buffer
//#define SDRAM_LTDC_ADDR		0xC1000000	//GUI内存起始地址
//#define SDRAM_LTDC_LEN			0x800000		//GUI内存大小 8M

#define GUI_FRAME1_BUFFER_ADDR 	SDRAM_LTDC_ADDR	 //0xC1000000
#define GUI_FRAME2_BUFFER_ADDR 	(SDRAM_LTDC_ADDR+XSIZE_PHYS * YSIZE_PHYS*2* NUM_VSCREENS * NUM_BUFFERS)	 //2M
#define	VRAM_ADDR								GUI_FRAME1_BUFFER_ADDR


typedef struct {
	LTDC_Layer_TypeDef * pstLTDC_Layer[2];
	uint32_t 						 ulFrameAddr[2];
	uint32_t						 ulPendingBuffer[GUI_NUM_LAYERS];
	uint32_t						 ulBufferIndex[GUI_NUM_LAYERS];
	uint32_t						 ulXSize[GUI_NUM_LAYERS];
	uint32_t						 ulYSize[GUI_NUM_LAYERS];
	uint32_t						 ulBytesPrePixels[GUI_NUM_LAYERS];
} LCDData_t;










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
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
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













#endif /* LCDCONF_H */

/*************************** End of file ****************************/
