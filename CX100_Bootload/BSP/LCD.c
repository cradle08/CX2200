/***********************************************************************
LCD显示，RGB565模式，使用SDRAM-->FMC-->DMA2D-->LTDC-->RGB-->屏幕显示方式
************************************************************************/
#include "LCD.h"
#include "fmc_sdram.h"
#include "string.h"




/*宏定义区***************************************************************/
//红色数据线
//#define LTDC_R0_GPIO_PORT        	GPIOH
//#define LTDC_R0_GPIO_CLK         	RCC_AHB1Periph_GPIOH
//#define LTDC_R0_GPIO_PIN         	GPIO_Pin_2
//#define LTDC_R0_PINSOURCE        	GPIO_PinSource2

//#define LTDC_R1_GPIO_PORT        	GPIOH
//#define LTDC_R1_GPIO_CLK         	RCC_AHB1Periph_GPIOH
//#define LTDC_R1_GPIO_PIN         	GPIO_Pin_3
//#define LTDC_R1_PINSOURCE        	GPIO_PinSource3

//#define LTDC_R2_GPIO_PORT        	GPIOH
//#define LTDC_R2_GPIO_CLK         	RCC_AHB1Periph_GPIOH
//#define LTDC_R2_GPIO_PIN         	GPIO_Pin_8
//#define LTDC_R2_PINSOURCE        	GPIO_PinSource8

#define LTDC_R3_GPIO_PORT        	GPIOH
#define LTDC_R3_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_R3_GPIO_PIN         	GPIO_Pin_9
#define LTDC_R3_PINSOURCE        	GPIO_PinSource9
#define LTDC_R3_GPIO_AF_MODE		GPIO_AF9_LTDC

#define LTDC_R4_GPIO_PORT        	GPIOH
#define LTDC_R4_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_R4_GPIO_PIN         	GPIO_Pin_10
#define LTDC_R4_PINSOURCE        	GPIO_PinSource10

#define LTDC_R5_GPIO_PORT        	GPIOH
#define LTDC_R5_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_R5_GPIO_PIN         	GPIO_Pin_11
#define LTDC_R5_PINSOURCE        	GPIO_PinSource11

#define LTDC_R6_GPIO_PORT        	GPIOH
#define LTDC_R6_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_R6_GPIO_PIN         	GPIO_Pin_12
#define LTDC_R6_PINSOURCE        	GPIO_PinSource12
#define LTDC_R6_GPIO_AF_MODE		GPIO_AF9_LTDC

#define LTDC_R7_GPIO_PORT        	GPIOG
#define LTDC_R7_GPIO_CLK         	RCC_AHB1Periph_GPIOG
#define LTDC_R7_GPIO_PIN         	GPIO_Pin_6
#define LTDC_R7_PINSOURCE        	GPIO_PinSource6
//绿色数据线
//#define LTDC_G0_GPIO_PORT        	GPIOE
//#define LTDC_G0_GPIO_CLK         	RCC_AHB1Periph_GPIOE
//#define LTDC_G0_GPIO_PIN         	GPIO_Pin_5
//#define LTDC_G0_PINSOURCE        	GPIO_PinSource5

//#define LTDC_G1_GPIO_PORT        	GPIOE
//#define LTDC_G1_GPIO_CLK         	RCC_AHB1Periph_GPIOE
//#define LTDC_G1_GPIO_PIN         	GPIO_Pin_6
//#define LTDC_G1_PINSOURCE        	GPIO_PinSource6

#define LTDC_G2_GPIO_PORT        	GPIOH
#define LTDC_G2_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_G2_GPIO_PIN         	GPIO_Pin_13
#define LTDC_G2_PINSOURCE        	GPIO_PinSource13

#define LTDC_G3_GPIO_PORT        	GPIOH
#define LTDC_G3_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_G3_GPIO_PIN         	GPIO_Pin_14
#define LTDC_G3_PINSOURCE        	GPIO_PinSource14
#define LTDC_G3_GPIO_AF_MODE		GPIO_AF9_LTDC

#define LTDC_G4_GPIO_PORT        	GPIOH
#define LTDC_G4_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_G4_GPIO_PIN         	GPIO_Pin_15
#define LTDC_G4_PINSOURCE        	GPIO_PinSource15

#define LTDC_G5_GPIO_PORT        	GPIOI
#define LTDC_G5_GPIO_CLK         	RCC_AHB1Periph_GPIOI
#define LTDC_G5_GPIO_PIN         	GPIO_Pin_0
#define LTDC_G5_PINSOURCE        	GPIO_PinSource0

#define LTDC_G6_GPIO_PORT        	GPIOI
#define LTDC_G6_GPIO_CLK         	RCC_AHB1Periph_GPIOI
#define LTDC_G6_GPIO_PIN         	GPIO_Pin_1
#define LTDC_G6_PINSOURCE        	GPIO_PinSource1

#define LTDC_G7_GPIO_PORT        	GPIOI
#define LTDC_G7_GPIO_CLK         	RCC_AHB1Periph_GPIOI
#define LTDC_G7_GPIO_PIN         	GPIO_Pin_2
#define LTDC_G7_PINSOURCE        	GPIO_PinSource2

//蓝色数据线
//#define LTDC_B0_GPIO_PORT        	GPIOE
//#define LTDC_B0_GPIO_CLK         	RCC_AHB1Periph_GPIOE
//#define LTDC_B0_GPIO_PIN         	GPIO_Pin_4
//#define LTDC_B0_PINSOURCE        	GPIO_PinSource4

//#define LTDC_B1_GPIO_PORT        	GPIOG
//#define LTDC_B1_GPIO_CLK         	RCC_AHB1Periph_GPIOG
//#define LTDC_B1_GPIO_PIN         	GPIO_Pin_12
//#define LTDC_B1_PINSOURCE        	GPIO_PinSource12

//#define LTDC_B2_GPIO_PORT        	GPIOD
//#define LTDC_B2_GPIO_CLK         	RCC_AHB1Periph_GPIOD
//#define LTDC_B2_GPIO_PIN         	GPIO_Pin_6
//#define LTDC_B2_PINSOURCE        	GPIO_PinSource6

#define LTDC_B3_GPIO_PORT        	GPIOG
#define LTDC_B3_GPIO_CLK         	RCC_AHB1Periph_GPIOG
#define LTDC_B3_GPIO_PIN         	GPIO_Pin_11
#define LTDC_B3_PINSOURCE        	GPIO_PinSource11

#define LTDC_B4_GPIO_PORT        	GPIOI
#define LTDC_B4_GPIO_CLK         	RCC_AHB1Periph_GPIOI
#define LTDC_B4_GPIO_PIN         	GPIO_Pin_4
#define LTDC_B4_PINSOURCE        	GPIO_PinSource4
#define LTDC_B4_GPIO_AF_MODE		GPIO_AF_LTDC

#define LTDC_B5_GPIO_PORT        	GPIOI
#define LTDC_B5_GPIO_CLK         	RCC_AHB1Periph_GPIOI
#define LTDC_B5_GPIO_PIN         	GPIO_Pin_5
#define LTDC_B5_PINSOURCE        	GPIO_PinSource5

#define LTDC_B6_GPIO_PORT        	GPIOI
#define LTDC_B6_GPIO_CLK         	RCC_AHB1Periph_GPIOI
#define LTDC_B6_GPIO_PIN         	GPIO_Pin_6
#define LTDC_B6_PINSOURCE        	GPIO_PinSource6

#define LTDC_B7_GPIO_PORT        	GPIOI
#define LTDC_B7_GPIO_CLK         	RCC_AHB1Periph_GPIOI
#define LTDC_B7_GPIO_PIN         	GPIO_Pin_7
#define LTDC_B7_PINSOURCE        	GPIO_PinSource7

//控制信号线
#define LTDC_CLK_GPIO_PORT        	GPIOG
#define LTDC_CLK_GPIO_CLK         	RCC_AHB1Periph_GPIOG
#define LTDC_CLK_GPIO_PIN         	GPIO_Pin_7
#define LTDC_CLK_PINSOURCE        	GPIO_PinSource7
	
#define LTDC_HSYNC_GPIO_PORT      	GPIOI
#define LTDC_HSYNC_GPIO_CLK       	RCC_AHB1Periph_GPIOI
#define LTDC_HSYNC_GPIO_PIN       	GPIO_Pin_10
#define LTDC_HSYNC_PINSOURCE      	GPIO_PinSource10
	
#define LTDC_VSYNC_GPIO_PORT      	GPIOI
#define LTDC_VSYNC_GPIO_CLK       	RCC_AHB1Periph_GPIOI
#define LTDC_VSYNC_GPIO_PIN       	GPIO_Pin_9
#define LTDC_VSYNC_PINSOURCE      	GPIO_PinSource9
	
	
#define LTDC_DE_GPIO_PORT         	GPIOF
#define LTDC_DE_GPIO_CLK          	RCC_AHB1Periph_GPIOF
#define LTDC_DE_GPIO_PIN          	GPIO_Pin_10
#define LTDC_DE_PINSOURCE         	GPIO_PinSource10
	
#define LTDC_DISP_GPIO_PORT       	GPIOB
#define LTDC_DISP_GPIO_CLK        	RCC_AHB1Periph_GPIOB
#define LTDC_DISP_GPIO_PIN        	GPIO_Pin_7
	
#define LTDC_BL_GPIO_PORT         	GPIOB
#define LTDC_BL_GPIO_CLK          	RCC_AHB1Periph_GPIOB
#define LTDC_BL_GPIO_PIN          	GPIO_Pin_5



//===时间参数=需根据使用的液晶数据手册配置，CLK=24MHZ时，则1个周期为：41.6ns==
#define HSW   1										//HSYNC信号宽度，单位CLK时钟周期
#define VSW   1										//VSYNC信号宽度，单位CLK时钟周期

#define HBP  46										//HSYNC后的无效像素
#define HFP  20										//HSYNC前的无效像素

#define VBP  23										//VSYNC后的无效行数
#define VFP  22										//VSYNC前的无效行数


#define LCD_PIXEL_WIDTH       ((uint16_t)800)		//屏幕有效宽度，像素为单位
#define LCD_PIXEL_HEIGHT      ((uint16_t)480)		//屏幕有效高度，像素为单位

#define LCD_FRAME_BUFFER      0xC0000000			//Layer1层显示数据的内存首地址
#define BUFFER_OFFSET         800*480*2				//一帧图像数据内存大小




/*全局变量定义区***********************************************************/
LCD_CurrStates_s LCDCurrStates = {0};				//当前操作LCD相关参数值



/*内部函数区*************************************************************/
/***
*配置LCD驱动使用到的引脚
***/
static void _LTDC_PinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* 使能LCD使用到的RGB引脚时钟 */
						 //红色数据线
	RCC_AHB1PeriphClockCmd(/*LTDC_R0_GPIO_CLK | LTDC_R1_GPIO_CLK | LTDC_R2_GPIO_CLK|*/
							LTDC_R3_GPIO_CLK | LTDC_R4_GPIO_CLK | LTDC_R5_GPIO_CLK|
							LTDC_R6_GPIO_CLK | LTDC_R7_GPIO_CLK |
							//绿色数据线
							/*LTDC_G0_GPIO_CLK|LTDC_G1_GPIO_CLK|*/LTDC_G2_GPIO_CLK|
							LTDC_G3_GPIO_CLK|LTDC_G4_GPIO_CLK|LTDC_G5_GPIO_CLK|
							LTDC_G6_GPIO_CLK|LTDC_G7_GPIO_CLK|
							//蓝色数据线
							/*LTDC_B0_GPIO_CLK|LTDC_B1_GPIO_CLK|LTDC_B2_GPIO_CLK|*/
							LTDC_B3_GPIO_CLK|LTDC_B4_GPIO_CLK|LTDC_B5_GPIO_CLK|
							LTDC_B6_GPIO_CLK|LTDC_B7_GPIO_CLK|
							//控制信号线
							LTDC_CLK_GPIO_CLK | LTDC_HSYNC_GPIO_CLK |LTDC_VSYNC_GPIO_CLK|
							LTDC_DE_GPIO_CLK  | LTDC_BL_GPIO_CLK    |LTDC_DISP_GPIO_CLK ,ENABLE);


	/* GPIO配置 */
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	/* 红色数据线 */
//	GPIO_InitStruct.GPIO_Pin = LTDC_R0_GPIO_PIN;
//	GPIO_Init(LTDC_R0_GPIO_PORT, &GPIO_InitStruct);
//	GPIO_PinAFConfig(LTDC_R0_GPIO_PORT, LTDC_R0_PINSOURCE, GPIO_AF_LTDC);

//	GPIO_InitStruct.GPIO_Pin = LTDC_R1_GPIO_PIN;
//	GPIO_Init(LTDC_R1_GPIO_PORT, &GPIO_InitStruct);
//	GPIO_PinAFConfig(LTDC_R1_GPIO_PORT, LTDC_R1_PINSOURCE, GPIO_AF_LTDC);

//	GPIO_InitStruct.GPIO_Pin = LTDC_R2_GPIO_PIN;
//	GPIO_Init(LTDC_R2_GPIO_PORT, &GPIO_InitStruct);
//	GPIO_PinAFConfig(LTDC_R2_GPIO_PORT, LTDC_R2_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_R3_GPIO_PIN;
	GPIO_Init(LTDC_R3_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_R3_GPIO_PORT, LTDC_R3_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_R4_GPIO_PIN;
	GPIO_Init(LTDC_R4_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_R4_GPIO_PORT, LTDC_R4_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_R5_GPIO_PIN;
	GPIO_Init(LTDC_R5_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_R5_GPIO_PORT, LTDC_R5_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_R6_GPIO_PIN;
	GPIO_Init(LTDC_R6_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_R6_GPIO_PORT, LTDC_R6_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_R7_GPIO_PIN;
	GPIO_Init(LTDC_R7_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_R7_GPIO_PORT, LTDC_R7_PINSOURCE, GPIO_AF_LTDC);

	//绿色数据线
//	GPIO_InitStruct.GPIO_Pin = LTDC_G0_GPIO_PIN;
//	GPIO_Init(LTDC_G0_GPIO_PORT, &GPIO_InitStruct);
//	GPIO_PinAFConfig(LTDC_G0_GPIO_PORT, LTDC_G0_PINSOURCE, GPIO_AF_LTDC);

//	GPIO_InitStruct.GPIO_Pin = LTDC_G1_GPIO_PIN;
//	GPIO_Init(LTDC_G1_GPIO_PORT, &GPIO_InitStruct);
//	GPIO_PinAFConfig(LTDC_G1_GPIO_PORT, LTDC_G1_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_G2_GPIO_PIN;
	GPIO_Init(LTDC_G2_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_G2_GPIO_PORT, LTDC_G2_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_G3_GPIO_PIN;
	GPIO_Init(LTDC_G3_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_G3_GPIO_PORT, LTDC_G3_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_G4_GPIO_PIN;
	GPIO_Init(LTDC_G4_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_G4_GPIO_PORT, LTDC_G4_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_G5_GPIO_PIN;
	GPIO_Init(LTDC_G5_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_G5_GPIO_PORT, LTDC_G5_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_G6_GPIO_PIN;
	GPIO_Init(LTDC_G6_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_G6_GPIO_PORT, LTDC_G6_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_G7_GPIO_PIN;
	GPIO_Init(LTDC_G7_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_G7_GPIO_PORT, LTDC_G7_PINSOURCE, GPIO_AF_LTDC);

	//蓝色数据线
//	GPIO_InitStruct.GPIO_Pin = LTDC_B0_GPIO_PIN;
//	GPIO_Init(LTDC_B0_GPIO_PORT, &GPIO_InitStruct);
//	GPIO_PinAFConfig(LTDC_B0_GPIO_PORT, LTDC_B0_PINSOURCE, GPIO_AF_LTDC);

//	GPIO_InitStruct.GPIO_Pin = LTDC_B1_GPIO_PIN;
//	GPIO_Init(LTDC_B1_GPIO_PORT, &GPIO_InitStruct);
//	GPIO_PinAFConfig(LTDC_B1_GPIO_PORT, LTDC_B1_PINSOURCE, GPIO_AF_LTDC);

//	GPIO_InitStruct.GPIO_Pin = LTDC_B2_GPIO_PIN;
//	GPIO_Init(LTDC_B2_GPIO_PORT, &GPIO_InitStruct);
//	GPIO_PinAFConfig(LTDC_B2_GPIO_PORT, LTDC_B2_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_B3_GPIO_PIN;
	GPIO_Init(LTDC_B3_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_B3_GPIO_PORT, LTDC_B3_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_B4_GPIO_PIN;
	GPIO_Init(LTDC_B4_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_B4_GPIO_PORT, LTDC_B4_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_B5_GPIO_PIN;
	GPIO_Init(LTDC_B5_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_B5_GPIO_PORT, LTDC_B5_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_B6_GPIO_PIN;
	GPIO_Init(LTDC_B6_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_B6_GPIO_PORT, LTDC_B6_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_B7_GPIO_PIN;
	GPIO_Init(LTDC_B7_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_B7_GPIO_PORT, LTDC_B7_PINSOURCE, GPIO_AF_LTDC);

	//控制信号线
	GPIO_InitStruct.GPIO_Pin = LTDC_CLK_GPIO_PIN;
	GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_CLK_GPIO_PORT, LTDC_CLK_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_HSYNC_GPIO_PIN;
	GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_HSYNC_GPIO_PORT, LTDC_HSYNC_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_VSYNC_GPIO_PIN;
	GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_VSYNC_GPIO_PORT, LTDC_VSYNC_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_DE_GPIO_PIN;		//数据有效信号线
	GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_DE_GPIO_PORT, LTDC_DE_PINSOURCE, GPIO_AF_LTDC);

	//BL DISP==通用推完输出，默认上拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStruct.GPIO_Pin = LTDC_DISP_GPIO_PIN;			//屏幕显示开关，高电平表示开启显示，低电平表示关闭显示
	GPIO_Init(LTDC_DISP_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = LTDC_BL_GPIO_PIN;			//背光控制，高电平表示开启背光，低电平表示关闭背光
	GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);

	//拉高使能lcd
	LTDC_DISP_GPIO_PORT->BSRRL = LTDC_DISP_GPIO_PIN;
	LTDC_BL_GPIO_PORT->BSRRL = LTDC_BL_GPIO_PIN;
}






/***
*配置LTDC
***/
static void _LTDC_Config(void)
{
	LTDC_InitTypeDef LTDC_InitStruct;

	/* Enable the LTDC Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);

	/* LTDC Configuration *********************************************************/
	//电平极性配置，需要根据实际使用的屏幕参数而定
	LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;					//水平同步信号有效电平为低电平
	LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;					//垂直同步信号有效电平为低电平
	LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;					//数据有效信号有效电平为低电平
	LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;					//像素时钟极性配置，上升沿有效

	//背景层RGB颜色分量值，在Layer1和Layer2没有配置时，则背景色就会显示出来
	LTDC_InitStruct.LTDC_BackgroundRedValue = 0;
	LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
	LTDC_InitStruct.LTDC_BackgroundBlueValue = 255;

	/* 配置 PLLSAI 分频器，它的输出作为像素同步时钟CLK，一般使用RGB888时，一般配置成8.75MHZ，使用RGB565时，可配置成24MHZ*/
	/* PLLSAI_VCO 输入时钟 = HSE_VALUE/PLL_M = 1 Mhz */
	/* PLLSAI_VCO 输出时钟 = PLLSAI_VCO输入 * PLLSAI_N = 384 Mhz */
	/* PLLLCDCLK = PLLSAI_VCO 输出/PLLSAI_R = 384/6  Mhz */
	/* LTDC 时钟频率 = PLLLCDCLK / DIV = 384/4/4 = 24 Mhz */
	/* LTDC时钟太高会导花屏，若对刷屏速度要求不高，降低时钟频率可减少花屏现象*/
	/* 以下函数三个参数分别为：PLLSAIN,PLLSAIQ,PLLSAIR，其中PLLSAIQ与LTDC无关*/
	RCC_PLLSAIConfig(384, 7, 4);
	RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div4);								//参数为DIV值
	RCC_PLLSAICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET);

	//时间参数配置，需根据使用的屏幕参数而定
	LTDC_InitStruct.LTDC_HorizontalSync =HSW-1;								// 配置行同步信号宽度(HSW-1)
	LTDC_InitStruct.LTDC_VerticalSync = VSW-1;								// 配置垂直同步信号宽度(VSW-1)
	LTDC_InitStruct.LTDC_AccumulatedHBP =HSW+HBP-1;							// 配置(HSW+HBP-1) 
	LTDC_InitStruct.LTDC_AccumulatedVBP = VSW+VBP-1;						// 配置(VSW+VBP-1) 
	LTDC_InitStruct.LTDC_AccumulatedActiveW = HSW+HBP+LCD_PIXEL_WIDTH-1;	// 配置(HSW+HBP+有效像素宽度-1)
	LTDC_InitStruct.LTDC_AccumulatedActiveH = VSW+VBP+LCD_PIXEL_HEIGHT-1;	// 配置(VSW+VBP+有效像素高度-1)
	LTDC_InitStruct.LTDC_TotalWidth =HSW+ HBP+LCD_PIXEL_WIDTH + HFP-1; 		// 配置总宽度(HSW+HBP+有效像素宽度+HFP-1)
	LTDC_InitStruct.LTDC_TotalHeigh =VSW+ VBP+LCD_PIXEL_HEIGHT + VFP-1;		// 配置总高度(VSW+VBP+有效像素高度+VFP-1)
	LTDC_Init(&LTDC_InitStruct);
}




/***
*配置LTDC显示层和窗口
***/
static void _LTDC_LayerConfig(void)
{
	LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;

	/* 层窗口配置 */
	/* 配置本层的窗口边界，注意这些参数是包含HBP HSW VBP VSW的，注意，这里和cubemx中配置的参数意义（像素坐标）不一样 */    
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = HSW+HBP;					//窗口行的第一个有效的时钟信号，该成员值应用为 (LTDC_InitStruct.LTDC_AccumulatedHBP+1)的值
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = HSW+HBP+LCD_PIXEL_WIDTH-1;	//窗口行的最后一个有效时钟信号，该成员值应用为 (LTDC_InitStruct.LTDC_AccumulatedActiveW)的值
	LTDC_Layer_InitStruct.LTDC_VerticalStart = VSW+VBP;						//窗口列的第一个有效的列时钟信号，该成员值应用为 (LTDC_InitStruct.LTDC_AccumulatedVBP+1)的值
	LTDC_Layer_InitStruct.LTDC_VerticalStop = VSW+VBP+LCD_PIXEL_HEIGHT-1;	//窗口列的最后一个有效的列时钟信号，该成员值应用为 (LTDC_InitStruct.LTDC_AccumulatedActiveH)的值

	LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;		//当前层像素输入格式，这里决定了该层输入源像素格式
	LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;							//透明度常量
	
	//该层默认颜色值，在该层定义的窗口外或者在该层禁止时使用
	LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;						//此层默认蓝色分量值
	LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 255;						//此层默认绿色分量值
	LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;							//此层默认红色分量值
	LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;						//此层默认透明度分量值
	
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;	//配置混合器1，只使用透明度常量值参与运算
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;	//配置混合器2，只使用透明度常量值参与运算

	LTDC_Layer_InitStruct.LTDC_CFBLineLength = (LCD_PIXEL_WIDTH * 2) + 3;	//配置行数据长度 = 行有效像素个数 * 每个像素的字节数（根据像素格式而定）+ 3，单位字节，注意，这里和cubemx中对应的参数配置不一样的意义
	LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;			//配置当前层的总行数，注意，这里和cubemx中对应的参数配置不一样的意义
	LTDC_Layer_InitStruct.LTDC_CFBPitch = (LCD_PIXEL_WIDTH * 2);			//类似于DMA2D的行偏移，从行起始处到下一行像素起始处的增量，一般就直接等于行的有效像素个数 * 每个像素的字节数，注意，这里和cubemx中对应的参数配置不一样的意义

	/*
	向这些空间写入的数据会被解释成像素数据，LTDC 会把这些数据传输到液晶屏上，
	所以我们要控制液晶屏的输出，只要修改这些空间的数据即可，包括变量操作、
	指针操作、DMA 操作以及 DMA2D 操作等一切可修	改 SDRAM 内容的操作都支持。
	*/
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER;			//配置当前层的显存起始位置

	//初始化Layer1
	LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

	//Layer2层配置
//	LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;		//该层像素输入格式，这里决定了该层输入源像素格式

//	//在Layer1层内存地址基础上+一帧图像数据需要的空间=Layer2层内存数据地址
//	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER + BUFFER_OFFSET;

//	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;	//配置混合器1，使用透明度常量值和像素本身透明度值参与运算
//	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor1_CA;	//配置混合器1，使用透明度常量值和像素本身透明度值参与运算

	///初始化Layer2
//	LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);

	//配置重载LTDC设置方式
	LTDC_ReloadConfig(LTDC_IMReload);			//这里为立即重载，LTDC_VBReload为渐消式重载

	//使能Layer1和Layer2层，失能时，则显示层窗口配置中的默认颜色值
	LTDC_LayerCmd(LTDC_Layer1, ENABLE);
//	LTDC_LayerCmd(LTDC_Layer2, ENABLE);

	/* LTDC configuration reload */
	LTDC_ReloadConfig(LTDC_IMReload);


	//使能抖动功能
	LTDC_DitherCmd(ENABLE);
}









/*外部函数区**************************************************************/
/***
*初始化LCD相关
***/
void LCD_Init(void)
{
	/* Enable the DMA2D Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);
	
	//配置FMC+SDRAM，用作显存
//	FMC_SDRAMConfig();
	
	//配置LTDC
	_LTDC_PinConfig();
	_LTDC_Config();
	_LTDC_LayerConfig();		//层级和窗口配置
	LTDC_Cmd(ENABLE);			//使能LTDC
}






/***
*使用DMA2D方式清屏指定层--使用纯色填充模式
参数：
	CurrentLayer：要操作的层
	Color：用来清屏的颜色RGB值，这里16位表示只支持RGB565或者RGB1555模式
***/
void LCD_ClearAll(CurrentLayer_e CurrentLayer,uint16_t Color)
{
	extern LCD_CurrStates_s LCDCurrStates;
	DMA2D_InitTypeDef DMA2D_InitStruct;
	uint16_t RedValue = 0, GreenValue = 0, BlueValue = 0;
	
	//保存当前操作参数值
	LCDCurrStates.ramAdrStart = CurrentLayer==LCD_LAYER_B?LCD_FRAME_BUFFER:(LCD_FRAME_BUFFER+BUFFER_OFFSET);
	LCDCurrStates.currPixelIndex = 0;

	//获取R,G,B各颜色分量值
	RedValue = (0xF800 & Color) >> 11;
	BlueValue = 0x001F & Color;
	GreenValue = (0x07E0 & Color) >> 5;

	//配置DMA2D
	DMA2D_DeInit();														//必须先重置，避免异常
	DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;							//设置DMA2D模式
	DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;						//输出颜色模式，这个要和参数color匹配，同时还需要屏幕支持此颜色模式才行
	DMA2D_InitStruct.DMA2D_OutputGreen = GreenValue;					//配置要输出的绿色分量
	DMA2D_InitStruct.DMA2D_OutputBlue = BlueValue;						//配置要输出的蓝色分量
	DMA2D_InitStruct.DMA2D_OutputRed = RedValue;						//配置要输出的红色分量
	DMA2D_InitStruct.DMA2D_OutputAlpha = (Color&0x8000)?0xFF:0x00;		//设置透明度
	DMA2D_InitStruct.DMA2D_OutputMemoryAdd = LCDCurrStates.ramAdrStart;	//设置输出数据的显存地址
	DMA2D_InitStruct.DMA2D_OutputOffset = 0;							//行偏移量
	DMA2D_InitStruct.DMA2D_NumberOfLine = LCD_PIXEL_HEIGHT;				//配置要传输多少行
	DMA2D_InitStruct.DMA2D_PixelPerLine = LCD_PIXEL_WIDTH;				//配置每行有多少像素点
	DMA2D_Init(&DMA2D_InitStruct);

	//开始传输
	DMA2D_StartTransfer();

	//等待执行完毕
	while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET);
}



/***
*使用DMA2D方式清除指定层的指定区域--使用纯色填充模式
参数：
	CurrentLayer：要操作的层
	Color：用来清屏的颜色RGB值，这里16位表示只支持RGB565或者RGB1555模式
	StartX：起始点X的像素坐标，从0开始取值
	StartY：起始点Y的像素坐标，从0开始取值
	EndX：终点X的像素坐标，最大为：LCD_PIXEL_WIDTH
	EndY：终点Y的像素坐标，最大为；LCD_PIXEL_HEIGHT
***/
void LCD_Clear(CurrentLayer_e CurrentLayer,uint16_t Color,u16 StartX,u16 StartY,u16 EndX,u16 EndY)
{
	extern LCD_CurrStates_s LCDCurrStates;
	DMA2D_InitTypeDef DMA2D_InitStruct;
	uint16_t RedValue = 0, GreenValue = 0, BlueValue = 0;
	u16 Temp = 0;
	
	//参数合法性判断
	StartX = StartX>=LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH-1:StartX;
	StartY = StartY>=LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT-1:StartY;
	EndX = EndX>=LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH:EndX;
	EndY = EndY>=LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT:EndY;
	
	if(StartX > EndX){
		Temp = StartX;
		StartX = EndX;
		EndX = Temp;
	}
	
	if(StartY > EndY){
		Temp = StartY;
		StartY = EndY;
		EndY = Temp;
	}
	
	//保存当前操作参数值
	LCDCurrStates.ramAdrStart = CurrentLayer==LCD_LAYER_B?LCD_FRAME_BUFFER:(LCD_FRAME_BUFFER+BUFFER_OFFSET);
	LCDCurrStates.currPixelIndex = StartY*LCD_PIXEL_WIDTH + StartX;

	//获取R,G,B各颜色分量值
	RedValue = (0xF800 & Color) >> 11;
	BlueValue = 0x001F & Color;
	GreenValue = (0x07E0 & Color) >> 5;

	//配置DMA2D
	DMA2D_DeInit();
	DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;							//设置DMA2D模式
	DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;						//输出颜色模式，这个要和参数color匹配，同时还需要屏幕支持此颜色模式才行
	DMA2D_InitStruct.DMA2D_OutputGreen = GreenValue;					//配置要输出的绿色分量
	DMA2D_InitStruct.DMA2D_OutputBlue = BlueValue;						//配置要输出的蓝色分量
	DMA2D_InitStruct.DMA2D_OutputRed = RedValue;						//配置要输出的红色分量
	DMA2D_InitStruct.DMA2D_OutputAlpha = (Color&0x8000)?0xFF:0x00;		//设置透明度
	DMA2D_InitStruct.DMA2D_OutputMemoryAdd = LCDCurrStates.ramAdrStart+LCDCurrStates.currPixelIndex*2;	//设置输出数据的显存地址
	DMA2D_InitStruct.DMA2D_OutputOffset = LCD_PIXEL_WIDTH-(EndX-StartX);								//行偏移量
	DMA2D_InitStruct.DMA2D_NumberOfLine = EndY-StartY;					//配置要传输多少行
	DMA2D_InitStruct.DMA2D_PixelPerLine = EndX-StartX;					//配置每行有多少像素点
	DMA2D_Init(&DMA2D_InitStruct);

	//开始传输
	DMA2D_StartTransfer();

	//等待执行完毕
	while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET);
}




/***
设置当前层透明度
参数：
	CurrentLayer：要操作的层
	Transparency：透明度
***/
void LCD_SetTransparency(CurrentLayer_e CurrentLayer,uint8_t Transparency)
{
	if (CurrentLayer == LCD_LAYER_B){
		LTDC_LayerAlpha(LTDC_Layer1, Transparency);
	}
	else{
		LTDC_LayerAlpha(LTDC_Layer2, Transparency);
	}
	
	LTDC_ReloadConfig(LTDC_IMReload);
}



/***
*设置LCD当前运行的各项参数，相当于设置当前画笔的运行参数
*参数：
	CurrentLayer：要操作的层
	TextColor：文本颜色
	TextBackColor：文本背景色
	Fonts：使用的字体
	Row：要显示的起始行号，这个跟使用的字体和分辨率有关，从0开始计数
	Col：要显示的起始列号，这个跟使用的字体和分辨率有关，从0开始计数
***/
void LCD_SetPara(CurrentLayer_e CurrentLayer,uint16_t TextColor, uint16_t TextBackColor,sFONT* Fonts,uint16_t Row,uint16_t Col)
{
	extern LCD_CurrStates_s LCDCurrStates;
	
	LCDCurrStates.ramAdrStart = CurrentLayer==LCD_LAYER_B?LCD_FRAME_BUFFER:(LCD_FRAME_BUFFER+BUFFER_OFFSET);
	LCDCurrStates.fonts = Fonts==NULL?&Font16x24:Fonts;
	LCDCurrStates.textBackColor = TextBackColor;
	LCDCurrStates.textColor = TextColor;
	
	//根据指定要显示的行号和列号计算要写入字符的位置的左上角像素位置
	Row += Col / (LCD_PIXEL_WIDTH/LCDCurrStates.fonts->Width);
	Col = Col % (LCD_PIXEL_WIDTH/LCDCurrStates.fonts->Width);
	LCDCurrStates.currPixelIndex = Row*LCDCurrStates.fonts->Height*LCD_PIXEL_WIDTH + Col*LCDCurrStates.fonts->Width;
	
	//当像素索引溢出时，则直接重置为0
	LCDCurrStates.currPixelIndex = LCDCurrStates.currPixelIndex >= (LCD_PIXEL_WIDTH*LCD_PIXEL_HEIGHT)?0:LCDCurrStates.currPixelIndex;
}




/***
*显示英文字符
	Ptr：字符串起始地址
*返回值：SUCCESS表示操作成功，ERROR表示操作失败
***/
ErrorStatus LCD_DisplayString(char* Ptr)
{
	extern LCD_CurrStates_s LCDCurrStates;
	
	u8 i=0,j=0,k=8;
	u32 FontIndex = 0;						//定义要显示的字符对应指定字库数组中的索引
	u8 FontTableValue = 0;					//字符点阵列表中的字节
	u16 PixelOffset = 0;					//定义要操作的字符点阵中相对于当前左上角像素点位置偏移量
	u16 LineAdd = 0;						//用于表示当前字符串所在位置像素的行号
	u32 Temp = 0,Temp2 = 0;
	
	//判断参数是否合法
	if(Ptr == NULL){
		return ERROR;
	}
	
	LineAdd = LCDCurrStates.currPixelIndex / LCD_PIXEL_WIDTH+1;
	
	//根据每个字符点阵数据，将像素设置为指定的前景色还是背景色即可显示字符
	while(*Ptr != '\0'){
		//判断是否是回车换行符
		if(*Ptr == '\r'){
			Ptr++;
			continue;
		}
		
		if(*Ptr == '\n'){
			Ptr++;
			//换行
			LCDCurrStates.currPixelIndex = (LCDCurrStates.currPixelIndex / LCD_PIXEL_WIDTH + LCDCurrStates.fonts->Height)*LCD_PIXEL_WIDTH;
			
			//判断是否已经满屏了
			if(LCDCurrStates.currPixelIndex >= LCD_PIXEL_HEIGHT*LCD_PIXEL_WIDTH){
				LCDCurrStates.currPixelIndex = 0;
				LineAdd = 1;
			}else{
				LineAdd += LCDCurrStates.fonts->Height;
			}
			continue;
		}
		
		FontIndex = (*Ptr - 32)*(LCDCurrStates.fonts->Height*LCDCurrStates.fonts->Width/8);
		PixelOffset = 0;
		
		for(i=0;i<LCDCurrStates.fonts->Height;i++){
			for(j=0;j<LCDCurrStates.fonts->Width;j++){
				if(k > 7){
					k = 0;
					FontTableValue = *(((u8*)LCDCurrStates.fonts->table)+FontIndex);
					FontIndex++;
				}
				
				if(((FontTableValue<<k)&0x80) == 0){
					//字符背景色填充当前像素
					*(((u16*)LCDCurrStates.ramAdrStart+LCDCurrStates.currPixelIndex+PixelOffset)) = LCDCurrStates.textBackColor;
				}else{
					//字符前景色填充当前像素
					*(((u16*)LCDCurrStates.ramAdrStart+LCDCurrStates.currPixelIndex+PixelOffset)) = LCDCurrStates.textColor;
				}
				
				k++;
				
				//移动像素点阵中的像素指针位置到下一列
				PixelOffset++;
			}
			
			//移动像素点阵中的像素指针位置到下一行
			PixelOffset += LCD_PIXEL_WIDTH - LCDCurrStates.fonts->Width;
		}
		
		Ptr++;
		
		//判断一行是否已写满需要换行
		Temp2 = LCDCurrStates.currPixelIndex + LCDCurrStates.fonts->Width;
		Temp = (Temp2)%(LCD_PIXEL_WIDTH*LineAdd);
		if( Temp != Temp2){
			LCDCurrStates.currPixelIndex += LCDCurrStates.fonts->Width - Temp;
			LCDCurrStates.currPixelIndex += (LCDCurrStates.fonts->Height-1)*LCD_PIXEL_WIDTH;
			
			//判断是否已经满屏了
			if(LCDCurrStates.currPixelIndex >= LCD_PIXEL_HEIGHT*LCD_PIXEL_WIDTH){
				LCDCurrStates.currPixelIndex = 0;
				LineAdd = 1;
			}else{
				LineAdd += LCDCurrStates.fonts->Height;
			}
		}else{
			LCDCurrStates.currPixelIndex += LCDCurrStates.fonts->Width;
		}
	}
	
	return SUCCESS;
}


/***
*画任意角度的直线
*参数：
	StartX：起始点X的像素坐标
	StartY：起始点Y的像素坐标
	EndX：终点X的像素坐标
	EndY：终点Y的像素坐标
***/
void LCD_DrawLine(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
{
	extern LCD_CurrStates_s LCDCurrStates;
	u16 Dy = 0;
	u16 Dx = 0;
	u16 X = 0;
	u16 Y = 0;
	s8 Xadd1 = 0;
	s8 Xadd2 = 0;
	s8 Yadd1 = 0;
	s8 Yadd2 = 0;
	u16 Count = 0;
	u16 OverFlagValue = 0;
	u16 Sum = 0;
	u16 OffsetAdd = 0;
	u16 i = 0;
	
	
	//参数合法性判断
	StartX = StartX>=LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH-1:StartX;
	StartY = StartY>=LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT-1:StartY;
	EndX = EndX>=LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH-1:EndX;
	EndY = EndY>=LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT-1:EndY;
	
	
	Dy = EndY>StartY?(EndY-StartY):(StartY-EndY);
	Dx = EndX>StartX?(EndX-StartX):(StartX-EndX);
	X = StartX;
	Y = StartY;
	
	//判断x轴和y轴的增减方向
	if(EndX >= StartX){
		Xadd1 = 1;
		Xadd2 = 1;
	}else{
		Xadd1 = -1;
		Xadd2 = -1;
	}
	
	if(EndY >= StartY){
		Yadd1 = 1;
		Yadd2 = 1;
	}else{
		Yadd1 = -1;
		Yadd2 = -1;
	}
	
	//判断以哪个轴作为步进轴
	if(Dx >= Dy){
		Xadd1 = 0;
		Yadd2 = 0;
		
		Count = Dx;
		OverFlagValue = Dx;
		Sum = Dx>>1;
		OffsetAdd = Dy;
	}else{
		Xadd2 = 0;
		Yadd1 = 0;
		
		Count = Dy;
		OverFlagValue = Dy;
		Sum = Dy>>1;
		OffsetAdd = Dx;
	}
	
	for(i=0;i<=Count;i++){
		LCDCurrStates.currPixelIndex = Y*LCD_PIXEL_WIDTH + X;
		*(((u16*)LCDCurrStates.ramAdrStart)+LCDCurrStates.currPixelIndex) = LCDCurrStates.textColor;
		
		Sum += OffsetAdd;
		if(Sum >= OverFlagValue){
			Sum -= OverFlagValue;
			
			X += Xadd1;
			Y += Yadd1;
		}
		
		X += Xadd2;
		Y += Yadd2;
	}
}



/***
*画空心圆
*参数：
	Xpos，Ypos：圆心像素坐标
	Radius：半径
***/
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int x = -Radius, y = 0, err = 2-2*Radius, e2;
	
	do{
		*(((u16*)LCDCurrStates.ramAdrStart)+((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos+y))) = LCDCurrStates.textColor;
		*(((u16*)LCDCurrStates.ramAdrStart)+((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos+y))) = LCDCurrStates.textColor;
		*(((u16*)LCDCurrStates.ramAdrStart)+((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos-y))) = LCDCurrStates.textColor;
		*(((u16*)LCDCurrStates.ramAdrStart)+((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos-y))) = LCDCurrStates.textColor;

		e2 = err;
		if (e2 <= y) {
		   err += ++y*2+1;
		   if (-x == y && e2 <= x) e2 = 0;
		}
		if (e2 > x) err += ++x*2+1;
	}while (x <= 0);
}



/***
*刷图
***/
void LCD_DrawPicture(CurrentLayer_e CurrentLayer,u32* PictureAdr)
{
	extern LCD_CurrStates_s LCDCurrStates;
	DMA2D_InitTypeDef DMA2D_InitStruct;
	DMA2D_FG_InitTypeDef DMA2D_FG_InitStruct; 
	
	//保存当前操作参数值
	LCDCurrStates.ramAdrStart = CurrentLayer==LCD_LAYER_B?LCD_FRAME_BUFFER:(LCD_FRAME_BUFFER+BUFFER_OFFSET);
	LCDCurrStates.currPixelIndex = 0;

	//配置DMA2D
	DMA2D_DeInit();
	DMA2D_InitStruct.DMA2D_Mode = DMA2D_M2M;							//设置DMA2D模式
	DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;						//输出颜色模式，这个要和图片像素格式匹配
	DMA2D_InitStruct.DMA2D_OutputGreen = 0;								//配置要输出的绿色分量
	DMA2D_InitStruct.DMA2D_OutputBlue = 255;							//配置要输出的蓝色分量
	DMA2D_InitStruct.DMA2D_OutputRed = 0;								//配置要输出的红色分量
	DMA2D_InitStruct.DMA2D_OutputAlpha = 255;							//设置透明度
	DMA2D_InitStruct.DMA2D_OutputMemoryAdd = LCDCurrStates.ramAdrStart;	//设置输出数据的显存地址
	DMA2D_InitStruct.DMA2D_OutputOffset = 0;							//行偏移量
	DMA2D_InitStruct.DMA2D_NumberOfLine = LCD_PIXEL_HEIGHT;				//配置要传输多少行
	DMA2D_InitStruct.DMA2D_PixelPerLine = LCD_PIXEL_WIDTH;				//配置每行有多少像素点
	DMA2D_Init(&DMA2D_InitStruct);
	
	/* Configure default values for foreground */
	DMA2D_FG_StructInit(&DMA2D_FG_InitStruct);

	/* Configure DMA2D foreground color mode */
	DMA2D_FG_InitStruct.DMA2D_FGCM = CM_RGB565;

	/* Configure Input Address */
	DMA2D_FG_InitStruct.DMA2D_FGMA = (uint32_t)PictureAdr;					

	/* Initialize foreground */ 
	DMA2D_FGConfig(&DMA2D_FG_InitStruct);

	//开始传输
	DMA2D_StartTransfer();

	//等待执行完毕
	while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET);
}



/***
*画点函数
*参数：
	PixelX：像素X坐标
	PixelY：像素Y坐标
***/
void LCD_DrawPoint(u16 PixelX,u16 PixelY)
{
	extern LCD_CurrStates_s LCDCurrStates;
	
	//参数合法性判断
	PixelX = PixelX>LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH-1:PixelX;
	PixelY = PixelY>LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT-1:PixelY;
	
	//画一个点
	LCDCurrStates.currPixelIndex = (PixelY-1)*LCD_PIXEL_WIDTH + PixelX;
	*(((u16*)LCDCurrStates.ramAdrStart)+LCDCurrStates.currPixelIndex) = LCDCurrStates.textColor;
}





