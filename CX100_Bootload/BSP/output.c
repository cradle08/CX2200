/*************************************************************
IO���
**************************************************************/
#include "output.h"




/*�궨����****************************************************/




/*�ڲ�������**************************************************/




/*�ⲿ������**************************************************/
/***
*�������IO���ţ�����������δʹ�õ�������ţ���ֹ����
***/
void Output_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//����IO����˿�ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIOG->BSRRL |= GPIO_Pin_10;					//������EN
	GPIOG->BSRRL |= GPIO_Pin_12;					//������CS
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIOE->BSRRL |= GPIO_Pin_3;						//���ֵ�λ��CS
	GPIOE->BSRRL |= GPIO_Pin_4;						//���ֵ�λ��CS
	
}






























