#ifndef _LCD_H
#define _LCD_H



#include "stm32f4xx.h"
#include "fonts.h"






/***
*定义操作LCD相关的实时全局变量结构体，这里针对的是RGB565或者RGB1555模式
***/
typedef struct _LCD_CurrStates_s{
	u32 ramAdrStart;					//根据当前要操作的层来决定要操作的显存地址
	u32 currPixelIndex;					//用于表示当前指向的像素索引值
	sFONT* fonts;						//要使用的字体
	uint16_t textColor;					//字体前景色
	uint16_t textBackColor;				//字体背景色
}LCD_CurrStates_s;




/***
*定义当前操作的图层枚举
***/
typedef enum _CurrentLayer_e{
	LCD_LAYER_F = 0,				//前景层（Layer2）
	LCD_LAYER_B						//背景层（Layer1）
}CurrentLayer_e;




/**
  * @brief  LCD color
  */
/*
 *   使用以下两组颜色必须跟LCD_LayerInit()函数中对每层的设置对应，不然会导致颜色显示错误。
 * 特别为实现两层同时正常显示需要把上层(LCD_FOREGROUND_LAYER)设置为LTDC_Pixelformat_ARGB1555
 * 格式，对应的必须使用LCD_COLOR1555_XXX颜色，该格式在1位透明控制位，可以实现没有实际颜色填
 * 充的像素点通透显示,从而使得显示下层(LCD_BACKGROUND_LAYER)成为可能。如果上层是使用
 * LTDC_Pixelformat_ARGB565格式设置是无法正常显示双层的。
 *   当然，可以把下层设置为LTDC_Pixelformat_ARGB1555格式，使用用LCD_COLOR1555_XXX也是可以正
 * 常显示的。
 * 特别注意：ARGB1555其中的A对应开发板液晶来说设置为1表示不透明，设置为0表透明。
*/
#define LCD_COLOR565_WHITE           0xFFFF        //11111 111111 11111
#define LCD_COLOR565_BLACK           0x0000        //00000 000000 00000
#define LCD_COLOR565_GREY            0xF7DE        //11110 111110 11110
#define LCD_COLOR565_BLUE            0x001F        //00000 000000 11111
#define LCD_COLOR565_BLUE2           0x051F        //00000 101000 11111
#define LCD_COLOR565_RED             0xF800        //11111 000000 00000
#define LCD_COLOR565_MAGENTA         0xF81F        //11111 000000 11111
#define LCD_COLOR565_GREEN           0x07E0        //00000 111111 00000
#define LCD_COLOR565_CYAN            0x7FFF        //01111 111111 11111
#define LCD_COLOR565_YELLOW          0xFFE0        //11111 111111 00000

                                                   //ARGB1555
#define LCD_COLOR1555_WHITE          0xFFFF        //1 11111 11111 11111
#define LCD_COLOR1555_BLACK          0x8000        //1 00000 00000 00000
#define LCD_COLOR1555_GREY           0xFBFE        //1 11110 11111 11110
#define LCD_COLOR1555_BLUE           0x801F        //1 00000 00000 11111
#define LCD_COLOR1555_BLUE2          0x829F        //1 00000 10100 11111
#define LCD_COLOR1555_RED            0xFC00        //1 11111 00000 00000
#define LCD_COLOR1555_MAGENTA        0xFC1F        //1 11111 00000 11111
#define LCD_COLOR1555_GREEN          0x83E0        //1 00000 11111 00000
#define LCD_COLOR1555_CYAN           0xBFFF        //1 01111 11111 11111
#define LCD_COLOR1555_YELLOW         0xFFE0        //1 11111 11111 00000
#define LCD_COLOR1555_CLEAR               0


/*定义默认常用纯色对应的RGB（或者ARGB）数据，RGB565与RGB888通用接口*/
#define LCD_COLOR_WHITE          	LCD_COLOR565_WHITE
#define LCD_COLOR_BLACK          	LCD_COLOR565_BLACK
#define LCD_COLOR_GREY           	LCD_COLOR565_GREY
#define LCD_COLOR_BLUE           	LCD_COLOR565_BLUE
#define LCD_COLOR_BLUE2          	LCD_COLOR565_BLUE2
#define LCD_COLOR_RED            	LCD_COLOR565_RED
#define LCD_COLOR_MAGENTA        	LCD_COLOR565_MAGENTA
#define LCD_COLOR_GREEN          	LCD_COLOR565_GREEN
#define LCD_COLOR_CYAN           	LCD_COLOR565_CYAN
#define LCD_COLOR_YELLOW         	LCD_COLOR565_YELLOW

#define TRANSPARENCY 				0x7FFF			//透明







void LCD_Init(void);

void LCD_ClearAll(CurrentLayer_e CurrentLayer,uint16_t Color);
void LCD_Clear(CurrentLayer_e CurrentLayer,uint16_t Color,u16 StartX,u16 StartY,u16 EndX,u16 EndY);

void LCD_SetTransparency(CurrentLayer_e CurrentLayer,uint8_t Transparency);
void LCD_SetPara(CurrentLayer_e CurrentLayer,uint16_t TextColor, uint16_t TextBackColor,sFONT* Fonts,uint16_t Row,uint16_t Col);
ErrorStatus LCD_DisplayString(char* Ptr);

void LCD_DrawLine(u16 StartX,u16 StartY,u16 EndX,u16 EndY);
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);

void LCD_DrawPicture(CurrentLayer_e CurrentLayer,u32* PictureAdr);
void LCD_DrawPoint(u16 PixelX,u16 PixelY);




#endif
