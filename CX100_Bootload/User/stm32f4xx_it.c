#include "stm32f4xx.h"





/***
*DMA2S5中断处理函数，用于表示SPI数据接收完成
***/
void DMA2_Stream3_IRQHandler(void)
{
	extern uint8_t SPIFlashDmaTCFlag;
	
	//传输完成中断
	if(DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF3))
	{
		//这里先清除TC中断标志位
		DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3);
		
		SPIFlashDmaTCFlag = 1;
	}
}




/***
*DMA2S6中断处理函数，用于表示SPI数据发送完成
***/
void DMA2_Stream4_IRQHandler(void)
{
	extern uint8_t SPIFlashDmaTCFlag;
	
	//传输完成中断
	if(DMA_GetITStatus(DMA2_Stream4, DMA_IT_TCIF4))
	{
		//这里先清除TC中断标志位
		DMA_ClearITPendingBit(DMA2_Stream4, DMA_IT_TCIF4);
		
		SPIFlashDmaTCFlag = 1;
	}
}



