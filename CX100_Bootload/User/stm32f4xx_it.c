#include "stm32f4xx.h"





/***
*DMA2S5�жϴ����������ڱ�ʾSPI���ݽ������
***/
void DMA2_Stream3_IRQHandler(void)
{
	extern uint8_t SPIFlashDmaTCFlag;
	
	//��������ж�
	if(DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF3))
	{
		//���������TC�жϱ�־λ
		DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3);
		
		SPIFlashDmaTCFlag = 1;
	}
}




/***
*DMA2S6�жϴ����������ڱ�ʾSPI���ݷ������
***/
void DMA2_Stream4_IRQHandler(void)
{
	extern uint8_t SPIFlashDmaTCFlag;
	
	//��������ж�
	if(DMA_GetITStatus(DMA2_Stream4, DMA_IT_TCIF4))
	{
		//���������TC�жϱ�־λ
		DMA_ClearITPendingBit(DMA2_Stream4, DMA_IT_TCIF4);
		
		SPIFlashDmaTCFlag = 1;
	}
}



