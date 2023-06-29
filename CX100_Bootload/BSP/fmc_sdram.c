/************************************************************************
*ʹ��FMC��дSDRAM
*************************************************************************/
#include "fmc_sdram.h"




/*�궨����***************************************************************/
//===���Ŷ���===
/*A���е�ַ�ź���*/    
#define FMC_A0_GPIO_PORT        GPIOF
#define FMC_A0_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_A0_GPIO_PIN         GPIO_Pin_0
#define FMC_A0_PINSOURCE        GPIO_PinSource0

#define FMC_A1_GPIO_PORT        GPIOF
#define FMC_A1_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_A1_GPIO_PIN         GPIO_Pin_1
#define FMC_A1_PINSOURCE        GPIO_PinSource1

#define FMC_A2_GPIO_PORT        GPIOF
#define FMC_A2_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_A2_GPIO_PIN         GPIO_Pin_2
#define FMC_A2_PINSOURCE        GPIO_PinSource2

#define FMC_A3_GPIO_PORT        GPIOF
#define FMC_A3_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_A3_GPIO_PIN         GPIO_Pin_3
#define FMC_A3_PINSOURCE        GPIO_PinSource3

#define FMC_A4_GPIO_PORT        GPIOF
#define FMC_A4_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_A4_GPIO_PIN         GPIO_Pin_4
#define FMC_A4_PINSOURCE        GPIO_PinSource4

#define FMC_A5_GPIO_PORT        GPIOF
#define FMC_A5_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_A5_GPIO_PIN         GPIO_Pin_5
#define FMC_A5_PINSOURCE        GPIO_PinSource5

#define FMC_A6_GPIO_PORT        GPIOF
#define FMC_A6_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_A6_GPIO_PIN         GPIO_Pin_12
#define FMC_A6_PINSOURCE        GPIO_PinSource12

#define FMC_A7_GPIO_PORT        GPIOF
#define FMC_A7_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_A7_GPIO_PIN         GPIO_Pin_13
#define FMC_A7_PINSOURCE        GPIO_PinSource13

#define FMC_A8_GPIO_PORT        GPIOF
#define FMC_A8_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_A8_GPIO_PIN         GPIO_Pin_14
#define FMC_A8_PINSOURCE        GPIO_PinSource14

#define FMC_A9_GPIO_PORT        GPIOF
#define FMC_A9_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_A9_GPIO_PIN         GPIO_Pin_15
#define FMC_A9_PINSOURCE        GPIO_PinSource15


#define FMC_A10_GPIO_PORT        GPIOG
#define FMC_A10_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define FMC_A10_GPIO_PIN         GPIO_Pin_0
#define FMC_A10_PINSOURCE        GPIO_PinSource0


#define FMC_A11_GPIO_PORT        GPIOG
#define FMC_A11_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define FMC_A11_GPIO_PIN         GPIO_Pin_1
#define FMC_A11_PINSOURCE        GPIO_PinSource1

#define FMC_A12_GPIO_PORT        GPIOG
#define FMC_A12_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define FMC_A12_GPIO_PIN         GPIO_Pin_2
#define FMC_A12_PINSOURCE        GPIO_PinSource2

/*BA��ַ��*/
#define FMC_BA0_GPIO_PORT        GPIOG
#define FMC_BA0_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define FMC_BA0_GPIO_PIN         GPIO_Pin_4
#define FMC_BA0_PINSOURCE        GPIO_PinSource4

#define FMC_BA1_GPIO_PORT        GPIOG
#define FMC_BA1_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define FMC_BA1_GPIO_PIN         GPIO_Pin_5
#define FMC_BA1_PINSOURCE        GPIO_PinSource5

/*DQ�����ź���*/
#define FMC_D0_GPIO_PORT        GPIOD
#define FMC_D0_GPIO_CLK         RCC_AHB1Periph_GPIOD
#define FMC_D0_GPIO_PIN         GPIO_Pin_14
#define FMC_D0_PINSOURCE        GPIO_PinSource14

#define FMC_D1_GPIO_PORT        GPIOD
#define FMC_D1_GPIO_CLK         RCC_AHB1Periph_GPIOD
#define FMC_D1_GPIO_PIN         GPIO_Pin_15
#define FMC_D1_PINSOURCE        GPIO_PinSource15

#define FMC_D2_GPIO_PORT        GPIOD
#define FMC_D2_GPIO_CLK         RCC_AHB1Periph_GPIOD
#define FMC_D2_GPIO_PIN         GPIO_Pin_0
#define FMC_D2_PINSOURCE        GPIO_PinSource0

#define FMC_D3_GPIO_PORT        GPIOD
#define FMC_D3_GPIO_CLK         RCC_AHB1Periph_GPIOD
#define FMC_D3_GPIO_PIN         GPIO_Pin_1
#define FMC_D3_PINSOURCE        GPIO_PinSource1

#define FMC_D4_GPIO_PORT        GPIOE
#define FMC_D4_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_D4_GPIO_PIN         GPIO_Pin_7
#define FMC_D4_PINSOURCE        GPIO_PinSource7

#define FMC_D5_GPIO_PORT        GPIOE
#define FMC_D5_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_D5_GPIO_PIN         GPIO_Pin_8
#define FMC_D5_PINSOURCE        GPIO_PinSource8

#define FMC_D6_GPIO_PORT        GPIOE
#define FMC_D6_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_D6_GPIO_PIN         GPIO_Pin_9
#define FMC_D6_PINSOURCE        GPIO_PinSource9

#define FMC_D7_GPIO_PORT        GPIOE
#define FMC_D7_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_D7_GPIO_PIN         GPIO_Pin_10
#define FMC_D7_PINSOURCE        GPIO_PinSource10

#define FMC_D8_GPIO_PORT        GPIOE
#define FMC_D8_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_D8_GPIO_PIN         GPIO_Pin_11
#define FMC_D8_PINSOURCE        GPIO_PinSource11

#define FMC_D9_GPIO_PORT        GPIOE
#define FMC_D9_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_D9_GPIO_PIN         GPIO_Pin_12
#define FMC_D9_PINSOURCE        GPIO_PinSource12

#define FMC_D10_GPIO_PORT        GPIOE
#define FMC_D10_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_D10_GPIO_PIN         GPIO_Pin_13
#define FMC_D10_PINSOURCE        GPIO_PinSource13

#define FMC_D11_GPIO_PORT        GPIOE
#define FMC_D11_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_D11_GPIO_PIN         GPIO_Pin_14
#define FMC_D11_PINSOURCE        GPIO_PinSource14

#define FMC_D12_GPIO_PORT        GPIOE
#define FMC_D12_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_D12_GPIO_PIN         GPIO_Pin_15
#define FMC_D12_PINSOURCE        GPIO_PinSource15

#define FMC_D13_GPIO_PORT        GPIOD
#define FMC_D13_GPIO_CLK         RCC_AHB1Periph_GPIOD
#define FMC_D13_GPIO_PIN         GPIO_Pin_8
#define FMC_D13_PINSOURCE        GPIO_PinSource8

#define FMC_D14_GPIO_PORT        GPIOD
#define FMC_D14_GPIO_CLK         RCC_AHB1Periph_GPIOD
#define FMC_D14_GPIO_PIN         GPIO_Pin_9
#define FMC_D14_PINSOURCE        GPIO_PinSource9

#define FMC_D15_GPIO_PORT        GPIOD
#define FMC_D15_GPIO_CLK         RCC_AHB1Periph_GPIOD
#define FMC_D15_GPIO_PIN         GPIO_Pin_10
#define FMC_D15_PINSOURCE        GPIO_PinSource10


/*�����ź���*/  
/*CSƬѡ*/
#define FMC_CS_GPIO_PORT        GPIOH
#define FMC_CS_GPIO_CLK         RCC_AHB1Periph_GPIOH
#define FMC_CS_GPIO_PIN         GPIO_Pin_3
#define FMC_CS_PINSOURCE        GPIO_PinSource3

/*WEдʹ��*/
#define FMC_WE_GPIO_PORT        GPIOC
#define FMC_WE_GPIO_CLK         RCC_AHB1Periph_GPIOC
#define FMC_WE_GPIO_PIN         GPIO_Pin_0
#define FMC_WE_PINSOURCE        GPIO_PinSource0
/*RAS��ѡͨ*/
#define FMC_RAS_GPIO_PORT        GPIOF
#define FMC_RAS_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_RAS_GPIO_PIN         GPIO_Pin_11
#define FMC_RAS_PINSOURCE        GPIO_PinSource11
/*CAS��ѡͨ*/
#define FMC_CAS_GPIO_PORT        GPIOG
#define FMC_CAS_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define FMC_CAS_GPIO_PIN         GPIO_Pin_15
#define FMC_CAS_PINSOURCE        GPIO_PinSource15
/*CLKͬ��ʱ�ӣ��洢����2*/
#define FMC_CLK_GPIO_PORT        GPIOG
#define FMC_CLK_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define FMC_CLK_GPIO_PIN         GPIO_Pin_8
#define FMC_CLK_PINSOURCE        GPIO_PinSource8
/*CKEʱ��ʹ�ܣ��洢����2*/
#define FMC_CKE_GPIO_PORT        GPIOH
#define FMC_CKE_GPIO_CLK         RCC_AHB1Periph_GPIOH
#define FMC_CKE_GPIO_PIN         GPIO_Pin_2
#define FMC_CKE_PINSOURCE        GPIO_PinSource2

/*DQM1��������*/
#define FMC_UDQM_GPIO_PORT        GPIOE
#define FMC_UDQM_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_UDQM_GPIO_PIN         GPIO_Pin_1
#define FMC_UDQM_PINSOURCE        GPIO_PinSource1
/*DQM0��������*/
#define FMC_LDQM_GPIO_PORT        GPIOE
#define FMC_LDQM_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_LDQM_GPIO_PIN         GPIO_Pin_0
#define FMC_LDQM_PINSOURCE        GPIO_PinSource0



//===SDRAM ģʽ���õļĴ�����ض���===
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200) 






/*�ڲ�������************************************************************/
/**
  * @brief  �ӳ�һ��ʱ��
  * @param  �ӳٵ�ʱ�䳤��
  * @retval None
  */
static void SDRAM_delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
}




/***
*FMC��������
***/
static void _FMC_PinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

						/*��ַ�ź���*/
	RCC_AHB1PeriphClockCmd(FMC_A0_GPIO_CLK | FMC_A1_GPIO_CLK | FMC_A2_GPIO_CLK | 
						 FMC_A3_GPIO_CLK | FMC_A4_GPIO_CLK | FMC_A5_GPIO_CLK |
						 FMC_A6_GPIO_CLK | FMC_A7_GPIO_CLK | FMC_A8_GPIO_CLK |
						 FMC_A9_GPIO_CLK | FMC_A10_GPIO_CLK| FMC_A11_GPIO_CLK|FMC_A12_GPIO_CLK|   
						 FMC_BA0_GPIO_CLK| FMC_BA1_GPIO_CLK|
						 /*�����ź���*/
						 FMC_D0_GPIO_CLK | FMC_D1_GPIO_CLK | FMC_D2_GPIO_CLK | 
						 FMC_D3_GPIO_CLK | FMC_D4_GPIO_CLK | FMC_D5_GPIO_CLK |
						 FMC_D6_GPIO_CLK | FMC_D7_GPIO_CLK | FMC_D8_GPIO_CLK |
						 FMC_D9_GPIO_CLK | FMC_D10_GPIO_CLK| FMC_D11_GPIO_CLK|
						 FMC_D12_GPIO_CLK| FMC_D13_GPIO_CLK| FMC_D14_GPIO_CLK|
						 FMC_D15_GPIO_CLK|  
						 /*�����ź���*/
						 FMC_CS_GPIO_CLK  | FMC_WE_GPIO_CLK | FMC_RAS_GPIO_CLK |
						 FMC_CAS_GPIO_CLK |FMC_CLK_GPIO_CLK | FMC_CKE_GPIO_CLK |
						 FMC_UDQM_GPIO_CLK|FMC_LDQM_GPIO_CLK, ENABLE);
					 

	/* ͨ�� GPIO ���� */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;       //����Ϊ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //�������
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;   


	/*A���е�ַ�ź��� �����������*/
	GPIO_InitStructure.GPIO_Pin = FMC_A0_GPIO_PIN; 
	GPIO_Init(FMC_A0_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A0_GPIO_PORT, FMC_A0_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A1_GPIO_PIN; 
	GPIO_Init(FMC_A1_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A1_GPIO_PORT, FMC_A1_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A2_GPIO_PIN; 
	GPIO_Init(FMC_A2_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A2_GPIO_PORT, FMC_A2_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A3_GPIO_PIN; 
	GPIO_Init(FMC_A3_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A3_GPIO_PORT, FMC_A3_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A4_GPIO_PIN; 
	GPIO_Init(FMC_A4_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A4_GPIO_PORT, FMC_A4_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A5_GPIO_PIN; 
	GPIO_Init(FMC_A5_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A5_GPIO_PORT, FMC_A5_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A6_GPIO_PIN; 
	GPIO_Init(FMC_A6_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A6_GPIO_PORT, FMC_A6_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A7_GPIO_PIN; 
	GPIO_Init(FMC_A7_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A7_GPIO_PORT, FMC_A7_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A8_GPIO_PIN; 
	GPIO_Init(FMC_A8_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A8_GPIO_PORT, FMC_A8_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A9_GPIO_PIN; 
	GPIO_Init(FMC_A9_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A9_GPIO_PORT, FMC_A9_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A10_GPIO_PIN; 
	GPIO_Init(FMC_A10_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A10_GPIO_PORT, FMC_A10_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A11_GPIO_PIN; 
	GPIO_Init(FMC_A11_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A11_GPIO_PORT, FMC_A11_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_A12_GPIO_PIN; 
	GPIO_Init(FMC_A12_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_A12_GPIO_PORT, FMC_A12_PINSOURCE , GPIO_AF_FMC);

	/*BA��ַ�ź���*/
	GPIO_InitStructure.GPIO_Pin = FMC_BA0_GPIO_PIN; 
	GPIO_Init(FMC_BA0_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_BA0_GPIO_PORT, FMC_BA0_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_BA1_GPIO_PIN;
	GPIO_Init(FMC_BA1_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_BA1_GPIO_PORT, FMC_BA1_PINSOURCE , GPIO_AF_FMC);


	/*DQ�����ź��� �����������*/
	GPIO_InitStructure.GPIO_Pin = FMC_D0_GPIO_PIN; 
	GPIO_Init(FMC_D0_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D0_GPIO_PORT, FMC_D0_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D1_GPIO_PIN; 
	GPIO_Init(FMC_D1_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D1_GPIO_PORT, FMC_D1_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D2_GPIO_PIN; 
	GPIO_Init(FMC_D2_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D2_GPIO_PORT, FMC_D2_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D3_GPIO_PIN; 
	GPIO_Init(FMC_D3_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D3_GPIO_PORT, FMC_D3_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D4_GPIO_PIN; 
	GPIO_Init(FMC_D4_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D4_GPIO_PORT, FMC_D4_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D5_GPIO_PIN; 
	GPIO_Init(FMC_D5_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D5_GPIO_PORT, FMC_D5_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D6_GPIO_PIN; 
	GPIO_Init(FMC_D6_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D6_GPIO_PORT, FMC_D6_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D7_GPIO_PIN; 
	GPIO_Init(FMC_D7_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D7_GPIO_PORT, FMC_D7_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D8_GPIO_PIN; 
	GPIO_Init(FMC_D8_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D8_GPIO_PORT, FMC_D8_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D9_GPIO_PIN; 
	GPIO_Init(FMC_D9_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D9_GPIO_PORT, FMC_D9_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D10_GPIO_PIN; 
	GPIO_Init(FMC_D10_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D10_GPIO_PORT, FMC_D10_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D11_GPIO_PIN; 
	GPIO_Init(FMC_D11_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D11_GPIO_PORT, FMC_D11_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D12_GPIO_PIN; 
	GPIO_Init(FMC_D12_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D12_GPIO_PORT, FMC_D12_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D13_GPIO_PIN; 
	GPIO_Init(FMC_D13_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D13_GPIO_PORT, FMC_D13_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D14_GPIO_PIN; 
	GPIO_Init(FMC_D14_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D14_GPIO_PORT, FMC_D14_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_D15_GPIO_PIN; 
	GPIO_Init(FMC_D15_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_D15_GPIO_PORT, FMC_D15_PINSOURCE , GPIO_AF_FMC);

	/*�����ź���*/
	GPIO_InitStructure.GPIO_Pin = FMC_CS_GPIO_PIN; 
	GPIO_Init(FMC_CS_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_CS_GPIO_PORT, FMC_CS_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_WE_GPIO_PIN; 
	GPIO_Init(FMC_WE_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_WE_GPIO_PORT, FMC_WE_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_RAS_GPIO_PIN; 
	GPIO_Init(FMC_RAS_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_RAS_GPIO_PORT, FMC_RAS_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_CAS_GPIO_PIN; 
	GPIO_Init(FMC_CAS_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_CAS_GPIO_PORT, FMC_CAS_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_CLK_GPIO_PIN; 
	GPIO_Init(FMC_CLK_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_CLK_GPIO_PORT, FMC_CLK_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_CKE_GPIO_PIN; 
	GPIO_Init(FMC_CKE_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_CKE_GPIO_PORT, FMC_CKE_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_UDQM_GPIO_PIN; 
	GPIO_Init(FMC_UDQM_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_UDQM_GPIO_PORT, FMC_UDQM_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_LDQM_GPIO_PIN; 
	GPIO_Init(FMC_LDQM_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_LDQM_GPIO_PORT, FMC_LDQM_PINSOURCE , GPIO_AF_FMC);
}



/***
*����FMC���ƼĴ�����ʱ��Ĵ���
***/
static void _FMC_Config(void)
{
	FMC_SDRAMInitTypeDef  FMC_SDRAMInitStructure;
	FMC_SDRAMTimingInitTypeDef  FMC_SDRAMTimingInitStructure; 

	/* ʹ�� FMC ʱ��--AHB3*/
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);

	/* SDRAMʱ��ṹ�壬����SDRAMоƬ����������----------------*/
	/* SDCLK�� 90 Mhz (HCLK/2 :180Mhz/2) 1��ʱ������Tsdclk =1/90MHz=11.11ns*/
	FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay    = 2;      				// TMRD: 2 Clock cycles ����ģʽ�Ĵ����������ӳ�
	FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 7;						// TXSR: min=72ns (7x11.11ns) �˳�����ˢ���������ӳ�
	FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime      = 4;						// TRAS: min=42ns (4x11.11ns) max=120 (ns) �Զ�ˢ������
	FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay        = 7;         				// TRC:  min=70 (6x11.11ns) ��������Ч����֮����ӳ٣��Լ���������ˢ������֮����ӳ�
	FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime    = 2;      				// TWR:  2CLK д�����Ԥ�������֮����ӳ٣��ȴ����ʱ���ſ�ʼִ��Ԥ�������
	FMC_SDRAMTimingInitStructure.FMC_RPDelay              = 2;         				// TRP:  15ns => 2x11.11ns Ԥ�����ӳ�   
	FMC_SDRAMTimingInitStructure.FMC_RCDDelay             = 2;						// TRCD: 15ns => 2x11.11ns ����Ч����ж�д����֮����ӳ�
	
	/* FMC SDRAM �������� */	
	FMC_SDRAMInitStructure.FMC_Bank = FMC_Bank1_SDRAM;								//ѡ��洢���򣬸���Ӳ�����߾�����ʹ�õ���CKE0+NE0����CKE1+NE1
	FMC_SDRAMInitStructure.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_9b;			// �е�ַ�߿��: [8:0]
	FMC_SDRAMInitStructure.FMC_RowBitsNumber = FMC_RowBits_Number_13b;				// �е�ַ�߿��: [12:0]
	FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth = FMC_SDMemory_Width_16b; 			// �����߿��
	FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;		// SDRAM�ڲ�bank����
	FMC_SDRAMInitStructure.FMC_CASLatency = FMC_CAS_Latency_2; 						// CL ��ʱ����Ŀ
	FMC_SDRAMInitStructure.FMC_WriteProtection = FMC_Write_Protection_Disable;		// ��ֹд���������ʹ����д���������� SDRAM д�����ݣ�����ʹ�ö��ǽ�ֹд�����ġ�
	FMC_SDRAMInitStructure.FMC_SDClockPeriod = FMC_SDClock_Period_2; 				// SDCLKʱ�ӷ�Ƶ���ӣ�SDCLK = HCLK/SDCLOCK_PERIOD
	FMC_SDRAMInitStructure.FMC_ReadBurst = FMC_Read_Burst_Enable;					// ͻ����ģʽ���ã���ֹʱ��Ч�� BL=1
	FMC_SDRAMInitStructure.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;				// ���ӳ����ã����������� CASLatency ��ʱ�����ں��ٵȴ����ٸ� HCLK ʱ�����ڲŽ������ݲ�������ȷ����ȷ��ǰ���£����ֵ����ΪԽ��Խ��
	FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStructure;	// SDRAMʱ�����

	/* ���ó�ʼ����������Ĵ���д������ */
	FMC_SDRAMInit(&FMC_SDRAMInitStructure); 
}



/***
*��ʼ��SDRAM����ʱ��,��õȴ�ִ�����ʱ����Ҫ��ѭ�������볬ʱ����
***/
static void _SDRAM_InitSequence(void)
{
	FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;
	uint32_t tmpr = 0;

	/* ������������ṩ��SDRAM��ʱ�� */
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 0;						//��ʱ��Ҫ�������Ͷ�� ���Զ�ˢ�¡� (Auto Refresh)����ʱ�����ñ���Ա���ɿ��������Ͷ��ٴΣ����������ֵΪ 1-16�������͵����������������ֵ��Ч
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;					//���� SDRAM ���ͼ���ģʽ�Ĵ�������ʱ������ṹ���Ա��ֵ��ͨ����ַ�߷��͵�SDRAM ��ģʽ�Ĵ����У������Աֵ����Ϊ 13 λ������λһһ��Ӧ SDRAM ��ģʽ�Ĵ���
	
	/* ���SDRAM��־���ȴ���SDRAM���� */ 
	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET);

	/* ������������*/
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	/* ��ʱ */
	SDRAM_delay(10);

	/* ������������е�bankԤ��� */ 
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 0;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
	
	/* ���SDRAM��־���ȴ���SDRAM���� */ 
	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET);
	
	/* ������������*/
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	/* ��������Զ�ˢ�� */   
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 2;      					//2���Զ�ˢ������
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
	
	/* ���SDRAM��־���ȴ���SDRAM���� */ 
	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET);

	/* �����Զ�ˢ������*/
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	/* ����sdram�Ĵ������� */
	tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_4        |						//ͻ������
				   SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |						//ͻ��ģʽΪ������ģʽ
				   SDRAM_MODEREG_CAS_LATENCY_2           |						//��Ҫ������FMC�Ĵ�����CLֵ��ͬ
				   SDRAM_MODEREG_OPERATING_MODE_STANDARD |						//��׼ģʽ
				   SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;						//д������ͻ��

	/* �����������SDRAM�Ĵ��� */
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_LoadMode;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;						//������Ҫ�����Զ�ˢ������
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;
	
	/* ���SDRAM��־���ȴ���SDRAM���� */ 
	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET);

	/* ������������*/
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	/* ����ˢ�¼����� */
	/*ˢ������ = (COUNT + 1) x SDRAM Ƶ��ʱ��
	COUNT =�� SDRAM ˢ������/����) - 20*/
	/* 64ms/8192=7.813us  (7.813 us x FSDCLK) - 20 =683 */
	FMC_SetRefreshCount(683);
	
	/* ���SDRAM��־���ȴ���SDRAM���� */ 
	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET);
}







/*�ⲿ������************************************************************/
/***
*����FMC+SDRAM
***/
void FMC_SDRAMConfig(void)
{
	_FMC_PinConfig();
	_FMC_Config();
	_SDRAM_InitSequence();
}















