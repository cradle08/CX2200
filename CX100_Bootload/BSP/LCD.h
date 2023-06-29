#ifndef _LCD_H
#define _LCD_H



#include "stm32f4xx.h"
#include "fonts.h"






/***
*�������LCD��ص�ʵʱȫ�ֱ����ṹ�壬������Ե���RGB565����RGB1555ģʽ
***/
typedef struct _LCD_CurrStates_s{
	u32 ramAdrStart;					//���ݵ�ǰҪ�����Ĳ�������Ҫ�������Դ��ַ
	u32 currPixelIndex;					//���ڱ�ʾ��ǰָ�����������ֵ
	sFONT* fonts;						//Ҫʹ�õ�����
	uint16_t textColor;					//����ǰ��ɫ
	uint16_t textBackColor;				//���屳��ɫ
}LCD_CurrStates_s;




/***
*���嵱ǰ������ͼ��ö��
***/
typedef enum _CurrentLayer_e{
	LCD_LAYER_F = 0,				//ǰ���㣨Layer2��
	LCD_LAYER_B						//�����㣨Layer1��
}CurrentLayer_e;




/**
  * @brief  LCD color
  */
/*
 *   ʹ������������ɫ�����LCD_LayerInit()�����ж�ÿ������ö�Ӧ����Ȼ�ᵼ����ɫ��ʾ����
 * �ر�Ϊʵ������ͬʱ������ʾ��Ҫ���ϲ�(LCD_FOREGROUND_LAYER)����ΪLTDC_Pixelformat_ARGB1555
 * ��ʽ����Ӧ�ı���ʹ��LCD_COLOR1555_XXX��ɫ���ø�ʽ��1λ͸������λ������ʵ��û��ʵ����ɫ��
 * ������ص�ͨ͸��ʾ,�Ӷ�ʹ����ʾ�²�(LCD_BACKGROUND_LAYER)��Ϊ���ܡ�����ϲ���ʹ��
 * LTDC_Pixelformat_ARGB565��ʽ�������޷�������ʾ˫��ġ�
 *   ��Ȼ�����԰��²�����ΪLTDC_Pixelformat_ARGB1555��ʽ��ʹ����LCD_COLOR1555_XXXҲ�ǿ�����
 * ����ʾ�ġ�
 * �ر�ע�⣺ARGB1555���е�A��Ӧ������Һ����˵����Ϊ1��ʾ��͸��������Ϊ0��͸����
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


/*����Ĭ�ϳ��ô�ɫ��Ӧ��RGB������ARGB�����ݣ�RGB565��RGB888ͨ�ýӿ�*/
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

#define TRANSPARENCY 				0x7FFF			//͸��







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
