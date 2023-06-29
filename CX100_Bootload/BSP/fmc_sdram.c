/************************************************************************
*使用FMC读写SDRAM
*************************************************************************/
#include "fmc_sdram.h"




/*宏定义区***************************************************************/
//===引脚定义===
/*A行列地址信号线*/    
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

/*BA地址线*/
#define FMC_BA0_GPIO_PORT        GPIOG
#define FMC_BA0_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define FMC_BA0_GPIO_PIN         GPIO_Pin_4
#define FMC_BA0_PINSOURCE        GPIO_PinSource4

#define FMC_BA1_GPIO_PORT        GPIOG
#define FMC_BA1_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define FMC_BA1_GPIO_PIN         GPIO_Pin_5
#define FMC_BA1_PINSOURCE        GPIO_PinSource5

/*DQ数据信号线*/
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


/*控制信号线*/  
/*CS片选*/
#define FMC_CS_GPIO_PORT        GPIOH
#define FMC_CS_GPIO_CLK         RCC_AHB1Periph_GPIOH
#define FMC_CS_GPIO_PIN         GPIO_Pin_3
#define FMC_CS_PINSOURCE        GPIO_PinSource3

/*WE写使能*/
#define FMC_WE_GPIO_PORT        GPIOC
#define FMC_WE_GPIO_CLK         RCC_AHB1Periph_GPIOC
#define FMC_WE_GPIO_PIN         GPIO_Pin_0
#define FMC_WE_PINSOURCE        GPIO_PinSource0
/*RAS行选通*/
#define FMC_RAS_GPIO_PORT        GPIOF
#define FMC_RAS_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define FMC_RAS_GPIO_PIN         GPIO_Pin_11
#define FMC_RAS_PINSOURCE        GPIO_PinSource11
/*CAS列选通*/
#define FMC_CAS_GPIO_PORT        GPIOG
#define FMC_CAS_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define FMC_CAS_GPIO_PIN         GPIO_Pin_15
#define FMC_CAS_PINSOURCE        GPIO_PinSource15
/*CLK同步时钟，存储区域2*/
#define FMC_CLK_GPIO_PORT        GPIOG
#define FMC_CLK_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define FMC_CLK_GPIO_PIN         GPIO_Pin_8
#define FMC_CLK_PINSOURCE        GPIO_PinSource8
/*CKE时钟使能，存储区域2*/
#define FMC_CKE_GPIO_PORT        GPIOH
#define FMC_CKE_GPIO_CLK         RCC_AHB1Periph_GPIOH
#define FMC_CKE_GPIO_PIN         GPIO_Pin_2
#define FMC_CKE_PINSOURCE        GPIO_PinSource2

/*DQM1数据掩码*/
#define FMC_UDQM_GPIO_PORT        GPIOE
#define FMC_UDQM_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_UDQM_GPIO_PIN         GPIO_Pin_1
#define FMC_UDQM_PINSOURCE        GPIO_PinSource1
/*DQM0数据掩码*/
#define FMC_LDQM_GPIO_PORT        GPIOE
#define FMC_LDQM_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define FMC_LDQM_GPIO_PIN         GPIO_Pin_0
#define FMC_LDQM_PINSOURCE        GPIO_PinSource0



//===SDRAM 模式配置的寄存器相关定义===
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






/*内部函数区************************************************************/
/**
  * @brief  延迟一段时间
  * @param  延迟的时间长度
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
*FMC引脚配置
***/
static void _FMC_PinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

						/*地址信号线*/
	RCC_AHB1PeriphClockCmd(FMC_A0_GPIO_CLK | FMC_A1_GPIO_CLK | FMC_A2_GPIO_CLK | 
						 FMC_A3_GPIO_CLK | FMC_A4_GPIO_CLK | FMC_A5_GPIO_CLK |
						 FMC_A6_GPIO_CLK | FMC_A7_GPIO_CLK | FMC_A8_GPIO_CLK |
						 FMC_A9_GPIO_CLK | FMC_A10_GPIO_CLK| FMC_A11_GPIO_CLK|FMC_A12_GPIO_CLK|   
						 FMC_BA0_GPIO_CLK| FMC_BA1_GPIO_CLK|
						 /*数据信号线*/
						 FMC_D0_GPIO_CLK | FMC_D1_GPIO_CLK | FMC_D2_GPIO_CLK | 
						 FMC_D3_GPIO_CLK | FMC_D4_GPIO_CLK | FMC_D5_GPIO_CLK |
						 FMC_D6_GPIO_CLK | FMC_D7_GPIO_CLK | FMC_D8_GPIO_CLK |
						 FMC_D9_GPIO_CLK | FMC_D10_GPIO_CLK| FMC_D11_GPIO_CLK|
						 FMC_D12_GPIO_CLK| FMC_D13_GPIO_CLK| FMC_D14_GPIO_CLK|
						 FMC_D15_GPIO_CLK|  
						 /*控制信号线*/
						 FMC_CS_GPIO_CLK  | FMC_WE_GPIO_CLK | FMC_RAS_GPIO_CLK |
						 FMC_CAS_GPIO_CLK |FMC_CLK_GPIO_CLK | FMC_CKE_GPIO_CLK |
						 FMC_UDQM_GPIO_CLK|FMC_LDQM_GPIO_CLK, ENABLE);
					 

	/* 通用 GPIO 配置 */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;       //配置为复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;   


	/*A行列地址信号线 针对引脚配置*/
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

	/*BA地址信号线*/
	GPIO_InitStructure.GPIO_Pin = FMC_BA0_GPIO_PIN; 
	GPIO_Init(FMC_BA0_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_BA0_GPIO_PORT, FMC_BA0_PINSOURCE , GPIO_AF_FMC);

	GPIO_InitStructure.GPIO_Pin = FMC_BA1_GPIO_PIN;
	GPIO_Init(FMC_BA1_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(FMC_BA1_GPIO_PORT, FMC_BA1_PINSOURCE , GPIO_AF_FMC);


	/*DQ数据信号线 针对引脚配置*/
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

	/*控制信号线*/
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
*配置FMC控制寄存器和时序寄存器
***/
static void _FMC_Config(void)
{
	FMC_SDRAMInitTypeDef  FMC_SDRAMInitStructure;
	FMC_SDRAMTimingInitTypeDef  FMC_SDRAMTimingInitStructure; 

	/* 使能 FMC 时钟--AHB3*/
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);

	/* SDRAM时序结构体，根据SDRAM芯片参数表配置----------------*/
	/* SDCLK： 90 Mhz (HCLK/2 :180Mhz/2) 1个时钟周期Tsdclk =1/90MHz=11.11ns*/
	FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay    = 2;      				// TMRD: 2 Clock cycles 加载模式寄存器命令后的延迟
	FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 7;						// TXSR: min=72ns (7x11.11ns) 退出自我刷新命令后的延迟
	FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime      = 4;						// TRAS: min=42ns (4x11.11ns) max=120 (ns) 自动刷新周期
	FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay        = 7;         				// TRC:  min=70 (6x11.11ns) 两个行有效命令之间的延迟，以及两个相邻刷新命令之间的延迟
	FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime    = 2;      				// TWR:  2CLK 写命令和预充电命令之间的延迟，等待这段时间后才开始执行预充电命令
	FMC_SDRAMTimingInitStructure.FMC_RPDelay              = 2;         				// TRP:  15ns => 2x11.11ns 预充电后延迟   
	FMC_SDRAMTimingInitStructure.FMC_RCDDelay             = 2;						// TRCD: 15ns => 2x11.11ns 行有效命令到列读写命令之间的延迟
	
	/* FMC SDRAM 控制配置 */	
	FMC_SDRAMInitStructure.FMC_Bank = FMC_Bank1_SDRAM;								//选择存储区域，根据硬件接线决定，使用的是CKE0+NE0还是CKE1+NE1
	FMC_SDRAMInitStructure.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_9b;			// 列地址线宽度: [8:0]
	FMC_SDRAMInitStructure.FMC_RowBitsNumber = FMC_RowBits_Number_13b;				// 行地址线宽度: [12:0]
	FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth = FMC_SDMemory_Width_16b; 			// 数据线宽度
	FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;		// SDRAM内部bank数量
	FMC_SDRAMInitStructure.FMC_CASLatency = FMC_CAS_Latency_2; 						// CL 的时钟数目
	FMC_SDRAMInitStructure.FMC_WriteProtection = FMC_Write_Protection_Disable;		// 禁止写保护，如果使能了写保护则不能向 SDRAM 写入数据，正常使用都是禁止写保护的。
	FMC_SDRAMInitStructure.FMC_SDClockPeriod = FMC_SDClock_Period_2; 				// SDCLK时钟分频因子，SDCLK = HCLK/SDCLOCK_PERIOD
	FMC_SDRAMInitStructure.FMC_ReadBurst = FMC_Read_Burst_Enable;					// 突发读模式设置，禁止时等效于 BL=1
	FMC_SDRAMInitStructure.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;				// 读延迟配置，用于配置在 CASLatency 个时钟周期后，再等待多少个 HCLK 时钟周期才进行数据采样，在确保正确的前提下，这个值设置为越短越好
	FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStructure;	// SDRAM时序参数

	/* 调用初始化函数，向寄存器写入配置 */
	FMC_SDRAMInit(&FMC_SDRAMInitStructure); 
}



/***
*初始化SDRAM命令时序,最好等待执行完毕时，不要死循环，加入超时机制
***/
static void _SDRAM_InitSequence(void)
{
	FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;
	uint32_t tmpr = 0;

	/* 配置命令：开启提供给SDRAM的时钟 */
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 0;						//有时需要连续发送多个 “自动刷新” (Auto Refresh)命令时，配置本成员即可控制它发送多少次，可输入参数值为 1-16，若发送的是其它命令，本参数值无效
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;					//当向 SDRAM 发送加载模式寄存器命令时，这个结构体成员的值将通过地址线发送到SDRAM 的模式寄存器中，这个成员值长度为 13 位，各个位一一对应 SDRAM 的模式寄存器
	
	/* 检查SDRAM标志，等待至SDRAM空闲 */ 
	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET);

	/* 发送上述命令*/
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	/* 延时 */
	SDRAM_delay(10);

	/* 配置命令：对所有的bank预充电 */ 
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 0;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
	
	/* 检查SDRAM标志，等待至SDRAM空闲 */ 
	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET);
	
	/* 发送上述命令*/
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	/* 配置命令：自动刷新 */   
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 2;      					//2个自动刷新命令
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
	
	/* 检查SDRAM标志，等待至SDRAM空闲 */ 
	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET);

	/* 发送自动刷新命令*/
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	/* 设置sdram寄存器配置 */
	tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_4        |						//突发长度
				   SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |						//突发模式为：连续模式
				   SDRAM_MODEREG_CAS_LATENCY_2           |						//需要和配置FMC寄存器的CL值相同
				   SDRAM_MODEREG_OPERATING_MODE_STANDARD |						//标准模式
				   SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;						//写操作无突发

	/* 配置命令：设置SDRAM寄存器 */
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_LoadMode;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;						//这里需要加入自动刷新周期
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;
	
	/* 检查SDRAM标志，等待至SDRAM空闲 */ 
	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET);

	/* 发送上述命令*/
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	/* 设置刷新计数器 */
	/*刷新速率 = (COUNT + 1) x SDRAM 频率时钟
	COUNT =（ SDRAM 刷新周期/行数) - 20*/
	/* 64ms/8192=7.813us  (7.813 us x FSDCLK) - 20 =683 */
	FMC_SetRefreshCount(683);
	
	/* 检查SDRAM标志，等待至SDRAM空闲 */ 
	while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET);
}







/*外部函数区************************************************************/
/***
*配置FMC+SDRAM
***/
void FMC_SDRAMConfig(void)
{
	_FMC_PinConfig();
	_FMC_Config();
	_SDRAM_InitSequence();
}















