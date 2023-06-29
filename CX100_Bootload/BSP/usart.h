#ifndef _USART_H
#define _USART_H


#include "stm32f4xx.h"
#include "stdio.h"


//打印信息,附带当前代码所在文件和行号
#define Myprintf(x,...)						printf("%s:%d >>> "x,__FILE__,__LINE__,##__VA_ARGS__)			



//数据接收队列长度定义相关
#define USART1_RECE_QUERY_LEN				127		//定义USART1存储接收到的数据的队列的最大长度,并且最好是2^x-1值，便于用与运算代替取模运算
#define USART2_RECE_QUERY_LEN				127		//定义USART2存储接收到的数据的队列的最大长度,并且最好是2^x-1值，便于用与运算代替取模运算
#define USART3_RECE_QUERY_LEN				127		//定义USART3存储接收到的数据的队列的最大长度,并且最好是2^x-1值，便于用与运算代替取模运算
#define USART4_RECE_QUERY_LEN				127		//定义USART4存储接收到的数据的队列的最大长度,并且最好是2^x-1值，便于用与运算代替取模运算
#define USART5_RECE_QUERY_LEN				127		//定义USART5存储接收到的数据的队列的最大长度,并且最好是2^x-1值，便于用与运算代替取模运算
#define USART6_RECE_QUERY_LEN				127		//定义USART6存储接收到的数据的队列的最大长度,并且最好是2^x-1值，便于用与运算代替取模运算



/***
*通过USART1待发送出去的数据缓存区结构体
***/
typedef struct _Usart1_SendBuf_s{
	u8 dataHead;
	u8 dataPID;
	
	u8 checksum;
	u8 dataEnd1;
	u8 dataEnd2;
}Usart1_SendBuf_s;


/***
*USART1接收队列
***/
typedef struct _Usart1_Rece_Query_s{
	u8 buf[USART1_RECE_QUERY_LEN+1];
	u8 write;
	u8 read;
}Usart1_Rece_Query_s;



/***
*UASRT1接收到数据包结构体
***/
typedef struct _Usart1_ReceBuf_s{
	u8 dataHead;
	u8 no1CustomNum;
	u8 no2CustomNum;
	u8 scNum;
	u8 carNum;
	u8 dataPID;
	
	u8 checkSum;
	u8 dataEnd1;
	u8 dataEnd2;
}Usart1_ReceBuf_s;



/***
*通过USART2待发送出去的数据缓存区结构体
***/
typedef struct _Usart2_SendBuf_s{
	u8 dataHead;
	u8 dataPID;
	
	u8 checksum;
	u8 dataEnd1;
	u8 dataEnd2;
}Usart2_SendBuf_s;


/***
*USART2接收队列
***/
typedef struct _Usart2_Rece_Query_s{
	u8 buf[USART2_RECE_QUERY_LEN+1];
	u8 write;
	u8 read;
}Usart2_Rece_Query_s;


/***
*USART2接收到的数据缓存区
***/
typedef struct _Usart2_ReceBuf_s{
	u8 dataHead;
	u8 dataPID;
	
	u8 checkSum;
	u8 dataEnd1;
	u8 dataEnd2;
}Usart2_ReceBuf_s;



/***
*通过USART3待发送出去的数据缓存区结构体
***/
typedef struct _Usart3_SendBuf_s{
	u8 dataHead;
	u8 no2CustomNum;
	u8 scNum;
	u8 carNum;
	u8 dataPID;
	
	u8 checksum;
	u8 dataEnd1;
	u8 dataEnd2;
}Usart3_SendBuf_s;


/***
*USART3接收队列
***/
typedef struct _Usart3_Rece_Query_s{
	u8 buf[USART3_RECE_QUERY_LEN+1];
	u8 write;
	u8 read;
}Usart3_Rece_Query_s;


/***
*USART3接收到的数据缓存区
***/
typedef struct _Usart3_ReceBuf_s{
	u8 dataHead;
	u8 dataPID;
	
	u8 checksum;
	u8 dataEnd1;
	u8 dataEnd2;
}Usart3_ReceBuf_s;



/***
*通过USART4待发送出去的数据缓存区结构体
***/
typedef struct _Usart4_SendBuf_s{
	u8 dataHead;
	u8 dataPID;
	
}Usart4_SendBuf_s;


/***
*USART4接收队列
***/
typedef struct _Usart4_Rece_Query_s{
	u8 buf[USART4_RECE_QUERY_LEN+1];
	u8 write;
	u8 read;
}Usart4_Rece_Query_s;


/***
*USART4接收到的数据缓存区
***/
typedef struct _Usart4_ReceBuf_s{
	u8 dataHead;
	u8 dataPID;
	
	
	u8 checksum;
	u8 dataEnd1;
	u8 dataEnd2;
}Usart4_ReceBuf_s;


/***
*通过USART5待发送出去的数据缓存区结构体
***/
typedef struct _Usart5_SendBuf_s{
	u8 dataHead;
	u8 dataPID;
	
}Usart5_SendBuf_s;


/***
*USART5接收队列
***/
typedef struct _Usart5_Rece_Query_s{
	u8 buf[USART5_RECE_QUERY_LEN+1];
	u8 write;
	u8 read;
}Usart5_Rece_Query_s;


/***
*通过USART6待发送出去的数据缓存区结构体
***/
typedef struct _Usart6_SendBuf_s{
	u8 dataHead;
	u8 dataPID;
	
}Usart6_SendBuf_s;


/***
*USART6接收队列
***/
typedef struct _Usart6_Rece_Query_s{
	u8 buf[USART6_RECE_QUERY_LEN+1];
	u8 write;
	u8 read;
}Usart6_Rece_Query_s;





void USART1_Dma2_Config(void);
void USART2_Dma1_Config(void);
void USART3_Dma1_Config(void);
void UART4_Dma1_Config(void);
void UART5_Dma1_Config(void);
void USART6_Dma2_Config(void);

void DMA_Usart1SendData(u16 Len);
void DMA_Usart2SendData(u16 Len);
void DMA_Usart3SendData(u16 Len);
void DMA_Uart4SendData(u16 Len);
void DMA_Uart5SendData(u16 Len);
void DMA_Usart6SendData(u16 Len);

u8 GetUsart1_ReceDataLen(void);
u8 GetUsart2_ReceDataLen(void);
u8 GetUsart3_ReceDataLen(void);
u8 GetUart4_ReceDataLen(void);
u8 GetUart5_ReceDataLen(void);
u8 GetUsart6_ReceDataLen(void);

void USART_Printf_Config(void);



#endif
