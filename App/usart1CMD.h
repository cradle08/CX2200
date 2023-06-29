#ifndef _USART1CMD_H
#define _USART1CMD_H



#include "stm32f4xx_hal.h"





#define USART1_RECE_BUF_LEN										31

/***
*�����USART1��ѭ���������ݵĶ���
***/
typedef struct _Usart1ReceBuf_s{
	char buf[USART1_RECE_BUF_LEN+1];							//ע��������Ҫ+1
	uint8_t readIndex;
	uint8_t writeIndex;
}Usart1ReceBuf_s;
















void Usart1CMDTask(void *argument);




#endif
