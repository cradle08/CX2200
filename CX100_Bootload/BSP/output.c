/*************************************************************
IO输出
**************************************************************/
#include "output.h"




/*宏定义区****************************************************/




/*内部函数区**************************************************/




/*外部函数区**************************************************/
/***
*配置输出IO引脚，配置其他尚未使用的输出引脚，防止干扰
***/
void Output_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//配置IO输出端口时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIOG->BSRRL |= GPIO_Pin_10;					//电阻屏EN
	GPIOG->BSRRL |= GPIO_Pin_12;					//电阻屏CS
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIOE->BSRRL |= GPIO_Pin_3;						//数字电位器CS
	GPIOE->BSRRL |= GPIO_Pin_4;						//数字电位器CS
	
}






























