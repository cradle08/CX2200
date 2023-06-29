#ifndef _USART1CMD_H
#define _USART1CMD_H



#include "stm32f4xx_hal.h"





#define USART1_RECE_BUF_LEN										31

/***
*定义从USART1中循环接收数据的队列
***/
typedef struct _Usart1ReceBuf_s{
	char buf[USART1_RECE_BUF_LEN+1];							//注意这里需要+1
	uint8_t readIndex;
	uint8_t writeIndex;
}Usart1ReceBuf_s;
















void Usart1CMDTask(void *argument);




#endif
