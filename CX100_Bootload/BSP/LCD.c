/***********************************************************************
LCD��ʾ��RGB565ģʽ��ʹ��SDRAM-->FMC-->DMA2D-->LTDC-->RGB-->��Ļ��ʾ��ʽ
************************************************************************/
#include "LCD.h"
#include "fmc_sdram.h"
#include "string.h"




/*�궨����***************************************************************/
//��ɫ������
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
//��ɫ������
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

//��ɫ������
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

//�����ź���
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



//===ʱ�����=�����ʹ�õ�Һ�������ֲ����ã�CLK=24MHZʱ����1������Ϊ��41.6ns==
#define HSW   1										//HSYNC�źſ�ȣ���λCLKʱ������
#define VSW   1										//VSYNC�źſ�ȣ���λCLKʱ������

#define HBP  46										//HSYNC�����Ч����
#define HFP  20										//HSYNCǰ����Ч����

#define VBP  23										//VSYNC�����Ч����
#define VFP  22										//VSYNCǰ����Ч����


#define LCD_PIXEL_WIDTH       ((uint16_t)800)		//��Ļ��Ч��ȣ�����Ϊ��λ
#define LCD_PIXEL_HEIGHT      ((uint16_t)480)		//��Ļ��Ч�߶ȣ�����Ϊ��λ

#define LCD_FRAME_BUFFER      0xC0000000			//Layer1����ʾ���ݵ��ڴ��׵�ַ
#define BUFFER_OFFSET         800*480*2				//һ֡ͼ�������ڴ��С




/*ȫ�ֱ���������***********************************************************/
LCD_CurrStates_s LCDCurrStates = {0};				//��ǰ����LCD��ز���ֵ



/*�ڲ�������*************************************************************/
/***
*����LCD����ʹ�õ�������
***/
static void _LTDC_PinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* ʹ��LCDʹ�õ���RGB����ʱ�� */
						 //��ɫ������
	RCC_AHB1PeriphClockCmd(/*LTDC_R0_GPIO_CLK | LTDC_R1_GPIO_CLK | LTDC_R2_GPIO_CLK|*/
							LTDC_R3_GPIO_CLK | LTDC_R4_GPIO_CLK | LTDC_R5_GPIO_CLK|
							LTDC_R6_GPIO_CLK | LTDC_R7_GPIO_CLK |
							//��ɫ������
							/*LTDC_G0_GPIO_CLK|LTDC_G1_GPIO_CLK|*/LTDC_G2_GPIO_CLK|
							LTDC_G3_GPIO_CLK|LTDC_G4_GPIO_CLK|LTDC_G5_GPIO_CLK|
							LTDC_G6_GPIO_CLK|LTDC_G7_GPIO_CLK|
							//��ɫ������
							/*LTDC_B0_GPIO_CLK|LTDC_B1_GPIO_CLK|LTDC_B2_GPIO_CLK|*/
							LTDC_B3_GPIO_CLK|LTDC_B4_GPIO_CLK|LTDC_B5_GPIO_CLK|
							LTDC_B6_GPIO_CLK|LTDC_B7_GPIO_CLK|
							//�����ź���
							LTDC_CLK_GPIO_CLK | LTDC_HSYNC_GPIO_CLK |LTDC_VSYNC_GPIO_CLK|
							LTDC_DE_GPIO_CLK  | LTDC_BL_GPIO_CLK    |LTDC_DISP_GPIO_CLK ,ENABLE);


	/* GPIO���� */
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	/* ��ɫ������ */
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

	//��ɫ������
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

	//��ɫ������
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

	//�����ź���
	GPIO_InitStruct.GPIO_Pin = LTDC_CLK_GPIO_PIN;
	GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_CLK_GPIO_PORT, LTDC_CLK_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_HSYNC_GPIO_PIN;
	GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_HSYNC_GPIO_PORT, LTDC_HSYNC_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_VSYNC_GPIO_PIN;
	GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_VSYNC_GPIO_PORT, LTDC_VSYNC_PINSOURCE, GPIO_AF_LTDC);

	GPIO_InitStruct.GPIO_Pin = LTDC_DE_GPIO_PIN;		//������Ч�ź���
	GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);
	GPIO_PinAFConfig(LTDC_DE_GPIO_PORT, LTDC_DE_PINSOURCE, GPIO_AF_LTDC);

	//BL DISP==ͨ�����������Ĭ������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStruct.GPIO_Pin = LTDC_DISP_GPIO_PIN;			//��Ļ��ʾ���أ��ߵ�ƽ��ʾ������ʾ���͵�ƽ��ʾ�ر���ʾ
	GPIO_Init(LTDC_DISP_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = LTDC_BL_GPIO_PIN;			//������ƣ��ߵ�ƽ��ʾ�������⣬�͵�ƽ��ʾ�رձ���
	GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);

	//����ʹ��lcd
	LTDC_DISP_GPIO_PORT->BSRRL = LTDC_DISP_GPIO_PIN;
	LTDC_BL_GPIO_PORT->BSRRL = LTDC_BL_GPIO_PIN;
}






/***
*����LTDC
***/
static void _LTDC_Config(void)
{
	LTDC_InitTypeDef LTDC_InitStruct;

	/* Enable the LTDC Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);

	/* LTDC Configuration *********************************************************/
	//��ƽ�������ã���Ҫ����ʵ��ʹ�õ���Ļ��������
	LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;					//ˮƽͬ���ź���Ч��ƽΪ�͵�ƽ
	LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;					//��ֱͬ���ź���Ч��ƽΪ�͵�ƽ
	LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;					//������Ч�ź���Ч��ƽΪ�͵�ƽ
	LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;					//����ʱ�Ӽ������ã���������Ч

	//������RGB��ɫ����ֵ����Layer1��Layer2û������ʱ���򱳾�ɫ�ͻ���ʾ����
	LTDC_InitStruct.LTDC_BackgroundRedValue = 0;
	LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
	LTDC_InitStruct.LTDC_BackgroundBlueValue = 255;

	/* ���� PLLSAI ��Ƶ�������������Ϊ����ͬ��ʱ��CLK��һ��ʹ��RGB888ʱ��һ�����ó�8.75MHZ��ʹ��RGB565ʱ�������ó�24MHZ*/
	/* PLLSAI_VCO ����ʱ�� = HSE_VALUE/PLL_M = 1 Mhz */
	/* PLLSAI_VCO ���ʱ�� = PLLSAI_VCO���� * PLLSAI_N = 384 Mhz */
	/* PLLLCDCLK = PLLSAI_VCO ���/PLLSAI_R = 384/6  Mhz */
	/* LTDC ʱ��Ƶ�� = PLLLCDCLK / DIV = 384/4/4 = 24 Mhz */
	/* LTDCʱ��̫�߻ᵼ����������ˢ���ٶ�Ҫ�󲻸ߣ�����ʱ��Ƶ�ʿɼ��ٻ�������*/
	/* ���º������������ֱ�Ϊ��PLLSAIN,PLLSAIQ,PLLSAIR������PLLSAIQ��LTDC�޹�*/
	RCC_PLLSAIConfig(384, 7, 4);
	RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div4);								//����ΪDIVֵ
	RCC_PLLSAICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET);

	//ʱ��������ã������ʹ�õ���Ļ��������
	LTDC_InitStruct.LTDC_HorizontalSync =HSW-1;								// ������ͬ���źſ��(HSW-1)
	LTDC_InitStruct.LTDC_VerticalSync = VSW-1;								// ���ô�ֱͬ���źſ��(VSW-1)
	LTDC_InitStruct.LTDC_AccumulatedHBP =HSW+HBP-1;							// ����(HSW+HBP-1) 
	LTDC_InitStruct.LTDC_AccumulatedVBP = VSW+VBP-1;						// ����(VSW+VBP-1) 
	LTDC_InitStruct.LTDC_AccumulatedActiveW = HSW+HBP+LCD_PIXEL_WIDTH-1;	// ����(HSW+HBP+��Ч���ؿ��-1)
	LTDC_InitStruct.LTDC_AccumulatedActiveH = VSW+VBP+LCD_PIXEL_HEIGHT-1;	// ����(VSW+VBP+��Ч���ظ߶�-1)
	LTDC_InitStruct.LTDC_TotalWidth =HSW+ HBP+LCD_PIXEL_WIDTH + HFP-1; 		// �����ܿ��(HSW+HBP+��Ч���ؿ��+HFP-1)
	LTDC_InitStruct.LTDC_TotalHeigh =VSW+ VBP+LCD_PIXEL_HEIGHT + VFP-1;		// �����ܸ߶�(VSW+VBP+��Ч���ظ߶�+VFP-1)
	LTDC_Init(&LTDC_InitStruct);
}




/***
*����LTDC��ʾ��ʹ���
***/
static void _LTDC_LayerConfig(void)
{
	LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;

	/* �㴰������ */
	/* ���ñ���Ĵ��ڱ߽磬ע����Щ�����ǰ���HBP HSW VBP VSW�ģ�ע�⣬�����cubemx�����õĲ������壨�������꣩��һ�� */    
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = HSW+HBP;					//�����еĵ�һ����Ч��ʱ���źţ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedHBP+1)��ֵ
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = HSW+HBP+LCD_PIXEL_WIDTH-1;	//�����е����һ����Чʱ���źţ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedActiveW)��ֵ
	LTDC_Layer_InitStruct.LTDC_VerticalStart = VSW+VBP;						//�����еĵ�һ����Ч����ʱ���źţ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedVBP+1)��ֵ
	LTDC_Layer_InitStruct.LTDC_VerticalStop = VSW+VBP+LCD_PIXEL_HEIGHT-1;	//�����е����һ����Ч����ʱ���źţ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedActiveH)��ֵ

	LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;		//��ǰ�����������ʽ����������˸ò�����Դ���ظ�ʽ
	LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;							//͸���ȳ���
	
	//�ò�Ĭ����ɫֵ���ڸò㶨��Ĵ���������ڸò��ֹʱʹ��
	LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;						//�˲�Ĭ����ɫ����ֵ
	LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 255;						//�˲�Ĭ����ɫ����ֵ
	LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;							//�˲�Ĭ�Ϻ�ɫ����ֵ
	LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;						//�˲�Ĭ��͸���ȷ���ֵ
	
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;	//���û����1��ֻʹ��͸���ȳ���ֵ��������
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;	//���û����2��ֻʹ��͸���ȳ���ֵ��������

	LTDC_Layer_InitStruct.LTDC_CFBLineLength = (LCD_PIXEL_WIDTH * 2) + 3;	//���������ݳ��� = ����Ч���ظ��� * ÿ�����ص��ֽ������������ظ�ʽ������+ 3����λ�ֽڣ�ע�⣬�����cubemx�ж�Ӧ�Ĳ������ò�һ��������
	LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;			//���õ�ǰ�����������ע�⣬�����cubemx�ж�Ӧ�Ĳ������ò�һ��������
	LTDC_Layer_InitStruct.LTDC_CFBPitch = (LCD_PIXEL_WIDTH * 2);			//������DMA2D����ƫ�ƣ�������ʼ������һ��������ʼ����������һ���ֱ�ӵ����е���Ч���ظ��� * ÿ�����ص��ֽ�����ע�⣬�����cubemx�ж�Ӧ�Ĳ������ò�һ��������

	/*
	����Щ�ռ�д������ݻᱻ���ͳ��������ݣ�LTDC �����Щ���ݴ��䵽Һ�����ϣ�
	��������Ҫ����Һ�����������ֻҪ�޸���Щ�ռ�����ݼ��ɣ���������������
	ָ�������DMA �����Լ� DMA2D ������һ�п���	�� SDRAM ���ݵĲ�����֧�֡�
	*/
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER;			//���õ�ǰ����Դ���ʼλ��

	//��ʼ��Layer1
	LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

	//Layer2������
//	LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;		//�ò����������ʽ����������˸ò�����Դ���ظ�ʽ

//	//��Layer1���ڴ��ַ������+һ֡ͼ��������Ҫ�Ŀռ�=Layer2���ڴ����ݵ�ַ
//	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER + BUFFER_OFFSET;

//	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;	//���û����1��ʹ��͸���ȳ���ֵ�����ر���͸����ֵ��������
//	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor1_CA;	//���û����1��ʹ��͸���ȳ���ֵ�����ر���͸����ֵ��������

	///��ʼ��Layer2
//	LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);

	//��������LTDC���÷�ʽ
	LTDC_ReloadConfig(LTDC_IMReload);			//����Ϊ�������أ�LTDC_VBReloadΪ����ʽ����

	//ʹ��Layer1��Layer2�㣬ʧ��ʱ������ʾ�㴰�������е�Ĭ����ɫֵ
	LTDC_LayerCmd(LTDC_Layer1, ENABLE);
//	LTDC_LayerCmd(LTDC_Layer2, ENABLE);

	/* LTDC configuration reload */
	LTDC_ReloadConfig(LTDC_IMReload);


	//ʹ�ܶ�������
	LTDC_DitherCmd(ENABLE);
}









/*�ⲿ������**************************************************************/
/***
*��ʼ��LCD���
***/
void LCD_Init(void)
{
	/* Enable the DMA2D Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);
	
	//����FMC+SDRAM�������Դ�
//	FMC_SDRAMConfig();
	
	//����LTDC
	_LTDC_PinConfig();
	_LTDC_Config();
	_LTDC_LayerConfig();		//�㼶�ʹ�������
	LTDC_Cmd(ENABLE);			//ʹ��LTDC
}






/***
*ʹ��DMA2D��ʽ����ָ����--ʹ�ô�ɫ���ģʽ
������
	CurrentLayer��Ҫ�����Ĳ�
	Color��������������ɫRGBֵ������16λ��ʾֻ֧��RGB565����RGB1555ģʽ
***/
void LCD_ClearAll(CurrentLayer_e CurrentLayer,uint16_t Color)
{
	extern LCD_CurrStates_s LCDCurrStates;
	DMA2D_InitTypeDef DMA2D_InitStruct;
	uint16_t RedValue = 0, GreenValue = 0, BlueValue = 0;
	
	//���浱ǰ��������ֵ
	LCDCurrStates.ramAdrStart = CurrentLayer==LCD_LAYER_B?LCD_FRAME_BUFFER:(LCD_FRAME_BUFFER+BUFFER_OFFSET);
	LCDCurrStates.currPixelIndex = 0;

	//��ȡR,G,B����ɫ����ֵ
	RedValue = (0xF800 & Color) >> 11;
	BlueValue = 0x001F & Color;
	GreenValue = (0x07E0 & Color) >> 5;

	//����DMA2D
	DMA2D_DeInit();														//���������ã������쳣
	DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;							//����DMA2Dģʽ
	DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;						//�����ɫģʽ�����Ҫ�Ͳ���colorƥ�䣬ͬʱ����Ҫ��Ļ֧�ִ���ɫģʽ����
	DMA2D_InitStruct.DMA2D_OutputGreen = GreenValue;					//����Ҫ�������ɫ����
	DMA2D_InitStruct.DMA2D_OutputBlue = BlueValue;						//����Ҫ�������ɫ����
	DMA2D_InitStruct.DMA2D_OutputRed = RedValue;						//����Ҫ����ĺ�ɫ����
	DMA2D_InitStruct.DMA2D_OutputAlpha = (Color&0x8000)?0xFF:0x00;		//����͸����
	DMA2D_InitStruct.DMA2D_OutputMemoryAdd = LCDCurrStates.ramAdrStart;	//����������ݵ��Դ��ַ
	DMA2D_InitStruct.DMA2D_OutputOffset = 0;							//��ƫ����
	DMA2D_InitStruct.DMA2D_NumberOfLine = LCD_PIXEL_HEIGHT;				//����Ҫ���������
	DMA2D_InitStruct.DMA2D_PixelPerLine = LCD_PIXEL_WIDTH;				//����ÿ���ж������ص�
	DMA2D_Init(&DMA2D_InitStruct);

	//��ʼ����
	DMA2D_StartTransfer();

	//�ȴ�ִ�����
	while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET);
}



/***
*ʹ��DMA2D��ʽ���ָ�����ָ������--ʹ�ô�ɫ���ģʽ
������
	CurrentLayer��Ҫ�����Ĳ�
	Color��������������ɫRGBֵ������16λ��ʾֻ֧��RGB565����RGB1555ģʽ
	StartX����ʼ��X���������꣬��0��ʼȡֵ
	StartY����ʼ��Y���������꣬��0��ʼȡֵ
	EndX���յ�X���������꣬���Ϊ��LCD_PIXEL_WIDTH
	EndY���յ�Y���������꣬���Ϊ��LCD_PIXEL_HEIGHT
***/
void LCD_Clear(CurrentLayer_e CurrentLayer,uint16_t Color,u16 StartX,u16 StartY,u16 EndX,u16 EndY)
{
	extern LCD_CurrStates_s LCDCurrStates;
	DMA2D_InitTypeDef DMA2D_InitStruct;
	uint16_t RedValue = 0, GreenValue = 0, BlueValue = 0;
	u16 Temp = 0;
	
	//�����Ϸ����ж�
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
	
	//���浱ǰ��������ֵ
	LCDCurrStates.ramAdrStart = CurrentLayer==LCD_LAYER_B?LCD_FRAME_BUFFER:(LCD_FRAME_BUFFER+BUFFER_OFFSET);
	LCDCurrStates.currPixelIndex = StartY*LCD_PIXEL_WIDTH + StartX;

	//��ȡR,G,B����ɫ����ֵ
	RedValue = (0xF800 & Color) >> 11;
	BlueValue = 0x001F & Color;
	GreenValue = (0x07E0 & Color) >> 5;

	//����DMA2D
	DMA2D_DeInit();
	DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;							//����DMA2Dģʽ
	DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;						//�����ɫģʽ�����Ҫ�Ͳ���colorƥ�䣬ͬʱ����Ҫ��Ļ֧�ִ���ɫģʽ����
	DMA2D_InitStruct.DMA2D_OutputGreen = GreenValue;					//����Ҫ�������ɫ����
	DMA2D_InitStruct.DMA2D_OutputBlue = BlueValue;						//����Ҫ�������ɫ����
	DMA2D_InitStruct.DMA2D_OutputRed = RedValue;						//����Ҫ����ĺ�ɫ����
	DMA2D_InitStruct.DMA2D_OutputAlpha = (Color&0x8000)?0xFF:0x00;		//����͸����
	DMA2D_InitStruct.DMA2D_OutputMemoryAdd = LCDCurrStates.ramAdrStart+LCDCurrStates.currPixelIndex*2;	//����������ݵ��Դ��ַ
	DMA2D_InitStruct.DMA2D_OutputOffset = LCD_PIXEL_WIDTH-(EndX-StartX);								//��ƫ����
	DMA2D_InitStruct.DMA2D_NumberOfLine = EndY-StartY;					//����Ҫ���������
	DMA2D_InitStruct.DMA2D_PixelPerLine = EndX-StartX;					//����ÿ���ж������ص�
	DMA2D_Init(&DMA2D_InitStruct);

	//��ʼ����
	DMA2D_StartTransfer();

	//�ȴ�ִ�����
	while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET);
}




/***
���õ�ǰ��͸����
������
	CurrentLayer��Ҫ�����Ĳ�
	Transparency��͸����
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
*����LCD��ǰ���еĸ���������൱�����õ�ǰ���ʵ����в���
*������
	CurrentLayer��Ҫ�����Ĳ�
	TextColor���ı���ɫ
	TextBackColor���ı�����ɫ
	Fonts��ʹ�õ�����
	Row��Ҫ��ʾ����ʼ�кţ������ʹ�õ�����ͷֱ����йأ���0��ʼ����
	Col��Ҫ��ʾ����ʼ�кţ������ʹ�õ�����ͷֱ����йأ���0��ʼ����
***/
void LCD_SetPara(CurrentLayer_e CurrentLayer,uint16_t TextColor, uint16_t TextBackColor,sFONT* Fonts,uint16_t Row,uint16_t Col)
{
	extern LCD_CurrStates_s LCDCurrStates;
	
	LCDCurrStates.ramAdrStart = CurrentLayer==LCD_LAYER_B?LCD_FRAME_BUFFER:(LCD_FRAME_BUFFER+BUFFER_OFFSET);
	LCDCurrStates.fonts = Fonts==NULL?&Font16x24:Fonts;
	LCDCurrStates.textBackColor = TextBackColor;
	LCDCurrStates.textColor = TextColor;
	
	//����ָ��Ҫ��ʾ���кź��кż���Ҫд���ַ���λ�õ����Ͻ�����λ��
	Row += Col / (LCD_PIXEL_WIDTH/LCDCurrStates.fonts->Width);
	Col = Col % (LCD_PIXEL_WIDTH/LCDCurrStates.fonts->Width);
	LCDCurrStates.currPixelIndex = Row*LCDCurrStates.fonts->Height*LCD_PIXEL_WIDTH + Col*LCDCurrStates.fonts->Width;
	
	//�������������ʱ����ֱ������Ϊ0
	LCDCurrStates.currPixelIndex = LCDCurrStates.currPixelIndex >= (LCD_PIXEL_WIDTH*LCD_PIXEL_HEIGHT)?0:LCDCurrStates.currPixelIndex;
}




/***
*��ʾӢ���ַ�
	Ptr���ַ�����ʼ��ַ
*����ֵ��SUCCESS��ʾ�����ɹ���ERROR��ʾ����ʧ��
***/
ErrorStatus LCD_DisplayString(char* Ptr)
{
	extern LCD_CurrStates_s LCDCurrStates;
	
	u8 i=0,j=0,k=8;
	u32 FontIndex = 0;						//����Ҫ��ʾ���ַ���Ӧָ���ֿ������е�����
	u8 FontTableValue = 0;					//�ַ������б��е��ֽ�
	u16 PixelOffset = 0;					//����Ҫ�������ַ�����������ڵ�ǰ���Ͻ����ص�λ��ƫ����
	u16 LineAdd = 0;						//���ڱ�ʾ��ǰ�ַ�������λ�����ص��к�
	u32 Temp = 0,Temp2 = 0;
	
	//�жϲ����Ƿ�Ϸ�
	if(Ptr == NULL){
		return ERROR;
	}
	
	LineAdd = LCDCurrStates.currPixelIndex / LCD_PIXEL_WIDTH+1;
	
	//����ÿ���ַ��������ݣ�����������Ϊָ����ǰ��ɫ���Ǳ���ɫ������ʾ�ַ�
	while(*Ptr != '\0'){
		//�ж��Ƿ��ǻس����з�
		if(*Ptr == '\r'){
			Ptr++;
			continue;
		}
		
		if(*Ptr == '\n'){
			Ptr++;
			//����
			LCDCurrStates.currPixelIndex = (LCDCurrStates.currPixelIndex / LCD_PIXEL_WIDTH + LCDCurrStates.fonts->Height)*LCD_PIXEL_WIDTH;
			
			//�ж��Ƿ��Ѿ�������
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
					//�ַ�����ɫ��䵱ǰ����
					*(((u16*)LCDCurrStates.ramAdrStart+LCDCurrStates.currPixelIndex+PixelOffset)) = LCDCurrStates.textBackColor;
				}else{
					//�ַ�ǰ��ɫ��䵱ǰ����
					*(((u16*)LCDCurrStates.ramAdrStart+LCDCurrStates.currPixelIndex+PixelOffset)) = LCDCurrStates.textColor;
				}
				
				k++;
				
				//�ƶ����ص����е�����ָ��λ�õ���һ��
				PixelOffset++;
			}
			
			//�ƶ����ص����е�����ָ��λ�õ���һ��
			PixelOffset += LCD_PIXEL_WIDTH - LCDCurrStates.fonts->Width;
		}
		
		Ptr++;
		
		//�ж�һ���Ƿ���д����Ҫ����
		Temp2 = LCDCurrStates.currPixelIndex + LCDCurrStates.fonts->Width;
		Temp = (Temp2)%(LCD_PIXEL_WIDTH*LineAdd);
		if( Temp != Temp2){
			LCDCurrStates.currPixelIndex += LCDCurrStates.fonts->Width - Temp;
			LCDCurrStates.currPixelIndex += (LCDCurrStates.fonts->Height-1)*LCD_PIXEL_WIDTH;
			
			//�ж��Ƿ��Ѿ�������
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
*������Ƕȵ�ֱ��
*������
	StartX����ʼ��X����������
	StartY����ʼ��Y����������
	EndX���յ�X����������
	EndY���յ�Y����������
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
	
	
	//�����Ϸ����ж�
	StartX = StartX>=LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH-1:StartX;
	StartY = StartY>=LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT-1:StartY;
	EndX = EndX>=LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH-1:EndX;
	EndY = EndY>=LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT-1:EndY;
	
	
	Dy = EndY>StartY?(EndY-StartY):(StartY-EndY);
	Dx = EndX>StartX?(EndX-StartX):(StartX-EndX);
	X = StartX;
	Y = StartY;
	
	//�ж�x���y�����������
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
	
	//�ж����ĸ�����Ϊ������
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
*������Բ
*������
	Xpos��Ypos��Բ����������
	Radius���뾶
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
*ˢͼ
***/
void LCD_DrawPicture(CurrentLayer_e CurrentLayer,u32* PictureAdr)
{
	extern LCD_CurrStates_s LCDCurrStates;
	DMA2D_InitTypeDef DMA2D_InitStruct;
	DMA2D_FG_InitTypeDef DMA2D_FG_InitStruct; 
	
	//���浱ǰ��������ֵ
	LCDCurrStates.ramAdrStart = CurrentLayer==LCD_LAYER_B?LCD_FRAME_BUFFER:(LCD_FRAME_BUFFER+BUFFER_OFFSET);
	LCDCurrStates.currPixelIndex = 0;

	//����DMA2D
	DMA2D_DeInit();
	DMA2D_InitStruct.DMA2D_Mode = DMA2D_M2M;							//����DMA2Dģʽ
	DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;						//�����ɫģʽ�����Ҫ��ͼƬ���ظ�ʽƥ��
	DMA2D_InitStruct.DMA2D_OutputGreen = 0;								//����Ҫ�������ɫ����
	DMA2D_InitStruct.DMA2D_OutputBlue = 255;							//����Ҫ�������ɫ����
	DMA2D_InitStruct.DMA2D_OutputRed = 0;								//����Ҫ����ĺ�ɫ����
	DMA2D_InitStruct.DMA2D_OutputAlpha = 255;							//����͸����
	DMA2D_InitStruct.DMA2D_OutputMemoryAdd = LCDCurrStates.ramAdrStart;	//����������ݵ��Դ��ַ
	DMA2D_InitStruct.DMA2D_OutputOffset = 0;							//��ƫ����
	DMA2D_InitStruct.DMA2D_NumberOfLine = LCD_PIXEL_HEIGHT;				//����Ҫ���������
	DMA2D_InitStruct.DMA2D_PixelPerLine = LCD_PIXEL_WIDTH;				//����ÿ���ж������ص�
	DMA2D_Init(&DMA2D_InitStruct);
	
	/* Configure default values for foreground */
	DMA2D_FG_StructInit(&DMA2D_FG_InitStruct);

	/* Configure DMA2D foreground color mode */
	DMA2D_FG_InitStruct.DMA2D_FGCM = CM_RGB565;

	/* Configure Input Address */
	DMA2D_FG_InitStruct.DMA2D_FGMA = (uint32_t)PictureAdr;					

	/* Initialize foreground */ 
	DMA2D_FGConfig(&DMA2D_FG_InitStruct);

	//��ʼ����
	DMA2D_StartTransfer();

	//�ȴ�ִ�����
	while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET);
}



/***
*���㺯��
*������
	PixelX������X����
	PixelY������Y����
***/
void LCD_DrawPoint(u16 PixelX,u16 PixelY)
{
	extern LCD_CurrStates_s LCDCurrStates;
	
	//�����Ϸ����ж�
	PixelX = PixelX>LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH-1:PixelX;
	PixelY = PixelY>LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT-1:PixelY;
	
	//��һ����
	LCDCurrStates.currPixelIndex = (PixelY-1)*LCD_PIXEL_WIDTH + PixelX;
	*(((u16*)LCDCurrStates.ramAdrStart)+LCDCurrStates.currPixelIndex) = LCDCurrStates.textColor;
}





