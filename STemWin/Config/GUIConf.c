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
File        : GUIConf.c
Purpose     : Display controller initialization
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

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#include "main.h"
#include "GUIConf.h"
#include "Common.h"


// Physical display size
//
#define XSIZE_PHYS 	LCD_WIDTH_PIXEL //800
#define YSIZE_PHYS  LCD_HIGHT_PIXEL	//480



//
// Define the available number of bytes available for the GUI
//
//#define GUI_NUMBYTES  0x200000

/*
* ��ֲ����
*/

#define EX_SRAM		        1
#define EMWIN_RAM_ADDR 		SDRAM_GUI_ADDR
#define EMWIN_RAM_LEN		(SDRAM_GUI_LEN)

#if EX_SRAM
#define GUI_NUMBYTES		EMWIN_RAM_LEN	
#else
#define GUI_NUMBYTES	0xC800	//50k
#endif



/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/
void GUI_X_Config(void) {

#if EX_SRAM	
	//
  // 32 bit aligned memory area
  //
	static U32 *aMemory;
	aMemory = (U32*)EMWIN_RAM_ADDR;
	
	GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
	//GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
#else
	
  static U32 aMemory[GUI_NUMBYTES / 4];
  //
  // Assign memory to emWin
  //
  GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
  //
  // Set default font
  //
#endif
  GUI_SetDefaultFont(GUI_FONT_8X16_ASCII);  //GUI_FONT_6X8
}

/*************************** End of file ****************************/
