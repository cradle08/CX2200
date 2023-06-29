/**********************************************************************
*串口打印配置，即初始化USART_Printf_Config()即可，也可以用
DMA+空闲中断数据传输配置方式。
**************************************************************************/
#include "usart.h"




/*宏定义区**********************************************************/
#define BAUDRATE_USART1					115200				//定义USART1收发数据包的波特率
#define BAUDRATE_USART2					115200				//定义USART2收发数据包的波特率
#define BAUDRATE_USART3					115200				//定义USART3收发数据包的波特率
#define BAUDRATE_USART4					115200				//定义USART4收发数据包的波特率
#define BAUDRATE_USART5					115200				//定义USART5收发数据包的波特率
#define BAUDRATE_USART6					115200				//定义USART6收发数据包的波特率


//打印模块相关
#define TEST_USART1_PRINTF				1					//使用USART1作为打印信息
#define TEST_USART2_PRINTF				0					//使用USART2作为打印信息
#define TEST_USART3_PRINTF				0					//使用USART3作为打印信息
#define TEST_USART4_PRINTF				0					//使用USART4作为打印信息
#define TEST_USART5_PRINTF				0					//使用USART5作为打印信息
#define TEST_USART6_PRINTF				0					//使用USART6作为打印信息


#define USE_USART1_PORT					GPIOA
#define USE_USART1_TX_PIN				GPIO_Pin_9							
#define USE_USART1_RX_PIN				GPIO_Pin_10

#define USE_USART2_PORT					GPIOA
#define USE_USART2_TX_PIN				GPIO_Pin_2
#define USE_USART2_RX_PIN				GPIO_Pin_3

#define USE_USART3_PORT					GPIOB
#define USE_USART3_TX_PIN				GPIO_Pin_10
#define USE_USART3_RX_PIN				GPIO_Pin_11

#define USE_USART4_PORT					GPIOC
#define USE_USART4_TX_PIN				GPIO_Pin_10
#define USE_USART4_RX_PIN				GPIO_Pin_11

#define USE_USART5_PORT1				GPIOD
#define USE_USART5_RX_PIN				GPIO_Pin_2
#define USE_USART5_PORT2				GPIOC
#define USE_USART5_TX_PIN				GPIO_Pin_12

#define USE_USART6_PORT					GPIOC
#define USE_USART6_TX_PIN				GPIO_Pin_6
#define USE_USART6_RX_PIN				GPIO_Pin_7




/*全局变量定义区**********************************************************/
Usart1_SendBuf_s Usart1SendBuf = {0};
Usart1_Rece_Query_s Usart1ReceQuery = {0};
Usart2_SendBuf_s Usart2SendBuf = {0};
Usart2_Rece_Query_s Usart2ReceQuery = {0};
Usart3_SendBuf_s Usart3SendBuf = {0};
Usart3_Rece_Query_s Usart3ReceQuery = {0};
Usart4_SendBuf_s Usart4SendBuf = {0};
Usart4_Rece_Query_s Usart4ReceQuery = {0};
Usart5_SendBuf_s Usart5SendBuf = {0};
Usart5_Rece_Query_s Usart5ReceQuery = {0};
Usart6_SendBuf_s Usart6SendBuf = {0};
Usart6_Rece_Query_s Usart6ReceQuery = {0};




/*内部函数区*************************************************************/
/***
*配置DMA2的数据流7的通道4，给USART1的TX端提供服务
***/
static void _DMA2_S7CH4_USART1_TX_Config(void)
{
	extern Usart1_SendBuf_s Usart1SendBuf;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA2时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);	
	
    //配置DMA
//    DMA_InitStructure.DMA_BufferSize = 10 ;                                   //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART1->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart1SendBuf ;           //内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA2_Stream7,&DMA_InitStructure);                                  //初始化数据流7相关操作
    
    //先关闭数据流7，等需要发送数据的时候再开启
    DMA_Cmd(DMA2_Stream7,DISABLE);
}


/***
*配置DMA2的数据流5的通道4，给USART1的RX端提供服务
***/
static void _DMA2_S5CH4_USART1_RX_Config(void)
{
	extern Usart1_Rece_Query_s Usart1ReceQuery;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA2时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
    //配置DMA
    DMA_InitStructure.DMA_BufferSize = USART1_RECE_QUERY_LEN+1;                 //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART1->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart1ReceQuery.buf ;     //内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA2_Stream5,&DMA_InitStructure);                                  //初始化数据流5相关操作
	
	//打开数据流5，等待USART1请求接收数据
    DMA_Cmd(DMA2_Stream5,ENABLE);
}


/***
*配置DMA1的数据流6的通道4，给USART2的TX端提供服务
***/
static void _DMA1_S6CH4_USART2_TX_Config(void)
{
	extern Usart2_SendBuf_s Usart2SendBuf;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA1时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
	
    //配置DMA
//    DMA_InitStructure.DMA_BufferSize = 10 ;                                   //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART2->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart2SendBuf ;           //内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA1_Stream6,&DMA_InitStructure);                                  //初始化数据流6相关操作
    
    //先关闭数据流6，等需要发送数据的时候再开启
    DMA_Cmd(DMA1_Stream6,DISABLE);
}


/***
*配置DMA1的数据流5的通道4，给USART2的RX端提供服务
***/
static void _DMA1_S5CH4_USART2_RX_Config(void)
{
	extern Usart2_Rece_Query_s Usart2eceQuery;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA1时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
    //配置DMA
    DMA_InitStructure.DMA_BufferSize = USART2_RECE_QUERY_LEN+1;                 //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART2->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart2ReceQuery.buf ;     //内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA1_Stream5,&DMA_InitStructure);                                  //初始化数据流5相关操作
	
	//打开数据流5，等待USART2请求接收数据
    DMA_Cmd(DMA1_Stream5,ENABLE);
}


/***
*配置DMA1的数据流3的通道4，给USART3的TX端提供服务
***/
static void _DMA1_S3CH4_USART3_TX_Config(void)
{
	extern Usart3_SendBuf_s Usart3SendBuf;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA1时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
	
    //配置DMA
//    DMA_InitStructure.DMA_BufferSize = 10 ;                                   //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART3->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart3SendBuf ;        	//内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA1_Stream3,&DMA_InitStructure);                                  //初始化数据流3相关操作
    
    //先关闭数据流3，等需要发送数据的时候再开启
    DMA_Cmd(DMA1_Stream3,DISABLE);
}


/***
*配置DMA1的数据流1的通道4，给USART3的RX端提供服务
***/
static void _DMA1_S1CH4_USART3_RX_Config(void)
{
	extern Usart3_Rece_Query_s Usart3eceQuery;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA1时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
    //配置DMA
    DMA_InitStructure.DMA_BufferSize = USART3_RECE_QUERY_LEN+1;                 //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART3->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart3ReceQuery.buf ;     //内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA1_Stream1,&DMA_InitStructure);                                  //初始化数据流1相关操作
	
	//打开数据流1，等待USART3请求接收数据
    DMA_Cmd(DMA1_Stream1,ENABLE);
}


/***
*配置DMA1的数据流4的通道4，给UART4的TX端提供服务
***/
static void _DMA1_S4CH4_UART4_TX_Config(void)
{
	extern Usart4_SendBuf_s Usart4SendBuf;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA1时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
	
    //配置DMA
//    DMA_InitStructure.DMA_BufferSize = 10 ;                                   //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(UART4->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart4SendBuf ;           //内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA1_Stream4,&DMA_InitStructure);                                  //初始化数据流4相关操作
    
    //先关闭数据流4，等需要发送数据的时候再开启
    DMA_Cmd(DMA1_Stream4,DISABLE);
}


/***
*配置DMA1的数据流2的通道4，给UART4的RX端提供服务
***/
static void _DMA1_S2CH4_UART4_RX_Config(void)
{
	extern Usart4_Rece_Query_s Usart4eceQuery;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA1时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
    //配置DMA
    DMA_InitStructure.DMA_BufferSize = USART4_RECE_QUERY_LEN+1;                 //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(UART4->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart4ReceQuery.buf ;     //内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA1_Stream2,&DMA_InitStructure);                                  //初始化数据流2相关操作
	
	//打开数据流2，等待USART4请求接收数据
    DMA_Cmd(DMA1_Stream2,ENABLE);
}


/***
*配置DMA1的数据流7的通道4，给UART5的TX端提供服务
***/
static void _DMA1_S7CH4_UART5_TX_Config(void)
{
	extern Usart5_SendBuf_s Usart5SendBuf;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA1时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
	
    //配置DMA
//    DMA_InitStructure.DMA_BufferSize = 10 ;                                   //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(UART5->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart5SendBuf ;           //内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA1_Stream7,&DMA_InitStructure);                                  //初始化数据流7相关操作
    
    //先关闭数据流7，等需要发送数据的时候再开启
    DMA_Cmd(DMA1_Stream7,DISABLE);
}


/***
*配置DMA1的数据流0的通道4，给UART5的RX端提供服务
***/
static void _DMA1_S0CH4_UART5_RX_Config(void)
{
	extern Usart5_Rece_Query_s Usart5eceQuery;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA1时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
    //配置DMA
    DMA_InitStructure.DMA_BufferSize = USART5_RECE_QUERY_LEN+1;                 //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(UART5->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart5ReceQuery.buf ;     //内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA1_Stream0,&DMA_InitStructure);                                  //初始化数据流0相关操作
	
	//打开数据流0，等待USART5请求接收数据
    DMA_Cmd(DMA1_Stream0,ENABLE);
}



/***
*配置DMA2的数据流6的通道5，给USART6的TX端提供服务
***/
static void _DMA2_S6CH5_USART6_TX_Config(void)
{
	extern Usart6_SendBuf_s Usart6SendBuf;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA2时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);	
	
    //配置DMA
//    DMA_InitStructure.DMA_BufferSize = 10 ;                                   //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_5;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART6->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart6SendBuf ;           //内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA2_Stream6,&DMA_InitStructure);                                  //初始化数据流6相关操作
    
    //先关闭数据流6，等需要发送数据的时候再开启
    DMA_Cmd(DMA2_Stream6,DISABLE);
}


/***
*配置DMA2的数据流1的通道5，给USART6的RX端提供服务
***/
static void _DMA2_S1CH5_USART6_RX_Config(void)
{
	extern Usart6_Rece_Query_s Usart6ReceQuery;
	DMA_InitTypeDef  DMA_InitStructure;
	
    //使能DMA2时钟，AHB1总线上
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
    //配置DMA
    DMA_InitStructure.DMA_BufferSize = USART6_RECE_QUERY_LEN+1;                 //传输的数据长度
    DMA_InitStructure.DMA_Channel = DMA_Channel_5;                              //选择通道，一个数据流只能配置给一个通道服务
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             //普通模式还是循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART6->DR)) ;      //外设地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;                    //传输方向
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)&Usart6ReceQuery.buf ;     //内存地址
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;                     //不开启FIFO模式，选择直接传输数据模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;      //FIFO触发的阈值，1/4，1/2,3/4,1/1
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;                //内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存每次传输的字节长度
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设是单次传输还是突发传输
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设每次传输的字节长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址固定
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //此DMA请求的优先级
    
    DMA_Init(DMA2_Stream1,&DMA_InitStructure);                                  //初始化数据流1相关操作
	
	//打开数据流1，等待USART6请求接收数据
    DMA_Cmd(DMA2_Stream1,ENABLE);
}





/***
*配置USART1的引脚
***/
static void _USART1_Pin_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    //配置外设引脚为复用模式
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);		//设置PA9为AF模式，给USART1复用
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);		//设置PA10为AF模式，给USART1复用

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/***
*内部函数,配置USART1
***/
static void _USART1_Config(void)
{
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    //配置USART
    USART_InitStructure.USART_BaudRate = BAUDRATE_USART1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1,&USART_InitStructure);
    
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);			//开启USART的TX端的DMA发送请求
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);			//开启USART的RX端的DMA接收请求
	
	NVIC_EnableIRQ(USART1_IRQn);
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
    
    USART_Cmd(USART1,ENABLE);
}


/***
*配置USART2的引脚
***/
static void _USART2_Pin_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    //配置外设引脚为复用模式
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);		//设置PA2为AF模式，给USART2复用
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);		//设置PA3为AF模式，给USART2复用

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/***
*内部函数,配置USART2
***/
static void _USART2_Config(void)
{
    USART_InitTypeDef USART_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //配置USART
    USART_InitStructure.USART_BaudRate = BAUDRATE_USART2;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2,&USART_InitStructure);
    
    USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);			//开启USART的TX端的DMA发送请求
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);			//开启USART的RX端的DMA接收请求
	
	NVIC_EnableIRQ(USART2_IRQn);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
    
    USART_Cmd(USART2,ENABLE);
}


/***
*配置USART3的引脚
***/
static void _USART3_Pin_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    //配置外设引脚为复用模式
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);		//设置PB10为AF模式，给USART3复用
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);		//设置PB11为AF模式，给USART3复用

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/***
*内部函数,配置USART3
***/
static void _USART3_Config(void)
{
    USART_InitTypeDef USART_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    //配置USART
    USART_InitStructure.USART_BaudRate = BAUDRATE_USART3;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3,&USART_InitStructure);
    
    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);			//开启USART的TX端的DMA发送请求
	USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);			//开启USART的RX端的DMA接收请求
	
	NVIC_EnableIRQ(USART3_IRQn);
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
    
    USART_Cmd(USART3,ENABLE);
}



/***
*配置UART4的引脚
***/
static void _UART4_Pin_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    
    //配置外设引脚为复用模式
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);		//设置PC10为AF模式，给UART4复用
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);		//设置PC11为AF模式，给UART4复用

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/***
*内部函数,配置UART4
***/
static void _UART4_Config(void)
{
    USART_InitTypeDef USART_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    //配置USART
    USART_InitStructure.USART_BaudRate = BAUDRATE_USART4;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4,&USART_InitStructure);
    
    USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);			//开启UART的TX端的DMA发送请求
	USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);			//开启UART的RX端的DMA接收请求
	
	NVIC_EnableIRQ(UART4_IRQn);
	USART_ITConfig(UART4,USART_IT_IDLE,ENABLE);
    
    USART_Cmd(UART4,ENABLE);
}


/***
*配置UART5的引脚
***/
static void _UART5_Pin_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    //配置外设引脚为复用模式
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);		//设置PC12为AF模式，给UART5复用
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);		//设置PD2为AF模式，给UART5复用

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}


/***
*内部函数,配置UART5
***/
static void _UART5_Config(void)
{
    USART_InitTypeDef USART_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

    //配置USART
    USART_InitStructure.USART_BaudRate = BAUDRATE_USART5;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART5,&USART_InitStructure);
    
    USART_DMACmd(UART5,USART_DMAReq_Tx,ENABLE);			//开启UART的TX端的DMA发送请求
	USART_DMACmd(UART5,USART_DMAReq_Rx,ENABLE);			//开启UART的RX端的DMA接收请求
	
	NVIC_EnableIRQ(UART5_IRQn);
	USART_ITConfig(UART5,USART_IT_IDLE,ENABLE);
    
    USART_Cmd(UART5,ENABLE);
}



/***
*配置USART6的引脚
***/
static void _USART6_Pin_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    
    //配置外设引脚为复用模式
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);		//设置PC6为AF模式，给USART6复用
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);		//设置PC7为AF模式，给USART6复用

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/***
*内部函数,配置USART6
***/
static void _USART6_Config(void)
{
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    //配置USART
    USART_InitStructure.USART_BaudRate = BAUDRATE_USART6;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART6,&USART_InitStructure);
    
    USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);			//开启USART的TX端的DMA发送请求
	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);			//开启USART的RX端的DMA接收请求
	
	NVIC_EnableIRQ(USART6_IRQn);
	USART_ITConfig(USART6,USART_IT_IDLE,ENABLE);
    
    USART_Cmd(USART6,ENABLE);
}







/*外部函数区**************************************************************/
/***
*配置串口1，并且通过DMA2的数据流7通道4来辅助发送数据，以及通过使用空闲中断
和DMA2的数据流5通道4来辅助接受数据
***/
void USART1_Dma2_Config(void)
{
	_USART1_Pin_Config();
	_DMA2_S7CH4_USART1_TX_Config();
	_DMA2_S5CH4_USART1_RX_Config();
	_USART1_Config();
}


/***
*配置串口2，并且通过DMA1的数据流6通道4来辅助发送数据，以及通过使用空闲中断
和DMA1的数据流5通道4来辅助接受数据
***/
void USART2_Dma1_Config(void)
{
	_USART2_Pin_Config();
	_DMA1_S6CH4_USART2_TX_Config();
	_DMA1_S5CH4_USART2_RX_Config();
	_USART2_Config();
}


/***
*配置串口3，并且通过DMA1的数据流3通道4来辅助发送数据，以及通过使用空闲中断
和DMA1的数据流1通道4来辅助接受数据
***/
void USART3_Dma1_Config(void)
{
	_USART3_Pin_Config();
	_DMA1_S3CH4_USART3_TX_Config();
	_DMA1_S1CH4_USART3_RX_Config();
	_USART3_Config();
}


/***
*配置串口4，并且通过DMA1的数据流3通道4来辅助发送数据，以及通过使用空闲中断
和DMA1的数据流1通道4来辅助接受数据
***/
void UART4_Dma1_Config(void)
{
	_UART4_Pin_Config();
	_DMA1_S4CH4_UART4_TX_Config();
	_DMA1_S2CH4_UART4_RX_Config();
	_UART4_Config();
}


/***
*配置串口5，并且通过DMA1的数据流7通道4来辅助发送数据，以及通过使用空闲中断
和DMA1的数据流0通道4来辅助接受数据
***/
void UART5_Dma1_Config(void)
{
	_UART5_Pin_Config();
	_DMA1_S7CH4_UART5_TX_Config();
	_DMA1_S0CH4_UART5_RX_Config();
	_UART5_Config();
}



/***
*配置串口6，并且通过DMA2的数据流6通道5来辅助发送数据，以及通过使用空闲中断
和DMA2的数据流1通道5来辅助接受数据
***/
void USART6_Dma2_Config(void)
{
	_USART6_Pin_Config();
	_DMA2_S6CH5_USART6_TX_Config();
	_DMA2_S1CH5_USART6_RX_Config();
	_USART6_Config();
}





//=======
/***
*开始通过DMA发送指定的数据到USART1的TX端
*参数：
	Len：本次传输的数据长度，也就是此数据帧的长度
***/
void DMA_Usart1SendData(u16 Len)
{
	extern Usart1_SendBuf_s Usart1SendBuf;
	
    DMA2_Stream7->CR &= (~DMA_SxCR_EN);                 //先关闭DMA2的数据流7
	DMA2->HIFCR = 0x0f400000;							//将流对应的事件标志清零
	DMA2_Stream7->NDTR = Len;							//重新配置将要传输的字节长度
	DMA2_Stream7->CR |= DMA_SxCR_EN;					//开启DMA2的数据流7，准备传输数据到USART1的TX端
}

/***
*开始通过DMA发送指定的数据到USART2的TX端
*参数：
	Len：本次传输的数据长度，也就是此数据帧的长度
***/
void DMA_Usart2SendData(u16 Len)
{
    DMA1_Stream6->CR &= (~DMA_SxCR_EN);                 //先关闭DMA1的数据流6
	DMA1->HIFCR = 0x003D0000;							//将流对应的事件标志清零
	DMA1_Stream6->NDTR = Len;							//重新配置将要传输的字节长度
	DMA1_Stream6->CR |= DMA_SxCR_EN;					//开启DMA1的数据流6，准备传输数据到USART2的TX端
}

/***
*开始通过DMA发送指定的数据到USART3的TX端
*参数：
	Len：本次传输的数据长度，也就是此数据帧的长度
***/
void DMA_Usart3SendData(u16 Len)
{
    DMA1_Stream3->CR &= (~DMA_SxCR_EN);                 //先关闭DMA1的数据流3
	DMA1->LIFCR = 0x0f400000;							//将流对应的事件标志清零
	DMA1_Stream3->NDTR = Len;							//重新配置将要传输的字节长度
	DMA1_Stream3->CR |= DMA_SxCR_EN;					//开启DMA1的数据流3，准备传输数据到USART3的TX端
}


/***
*开始通过DMA发送指定的数据到UART4的TX端
*参数：
	Len：本次传输的数据长度，也就是此数据帧的长度
***/
void DMA_Uart4SendData(u16 Len)
{
    DMA1_Stream4->CR &= (~DMA_SxCR_EN);                 //先关闭DMA1的数据流4
	DMA1->HIFCR = 0x0000003D;							//将流对应的事件标志清零
	DMA1_Stream4->NDTR = Len;							//重新配置将要传输的字节长度
	DMA1_Stream4->CR |= DMA_SxCR_EN;					//开启DMA1的数据流4，准备传输数据到UART4的TX端
}


/***
*开始通过DMA发送指定的数据到UART5的TX端
*参数：
	Len：本次传输的数据长度，也就是此数据帧的长度
***/
void DMA_Uart5SendData(u16 Len)
{
    DMA1_Stream7->CR &= (~DMA_SxCR_EN);                 //先关闭DMA1的数据流7
	DMA1->HIFCR = 0x0f400000;							//将流对应的事件标志清零
	DMA1_Stream7->NDTR = Len;							//重新配置将要传输的字节长度
	DMA1_Stream7->CR |= DMA_SxCR_EN;					//开启DMA1的数据流7，准备传输数据到UART5的TX端
}


/***
*开始通过DMA发送指定的数据到USART6的TX端
*参数：
	Len：本次传输的数据长度，也就是此数据帧的长度
***/
void DMA_Usart6SendData(u16 Len)
{
    DMA2_Stream6->CR &= (~DMA_SxCR_EN);                 //先关闭DMA2的数据流6
	DMA2->HIFCR = 0x003D0000;							//将流对应的事件标志清零
	DMA2_Stream6->NDTR = Len;							//重新配置将要传输的字节长度
	DMA2_Stream6->CR |= DMA_SxCR_EN;					//开启DMA2的数据流6，准备传输数据到USART6的TX端
}




//=======
/***
*指示当前串口对应的队列的写指针指示的位置
***/
u8 GetUsart1_ReceDataLen(void)
{
	return (USART1_RECE_QUERY_LEN+1 - DMA2_Stream5->NDTR);
}


/***
*指示当前串口对应的队列的写指针指示的位置
***/
u8 GetUsart2_ReceDataLen(void)
{
	return (USART2_RECE_QUERY_LEN+1 - DMA1_Stream5->NDTR);
}


/***
*指示当前串口对应的队列的写指针指示的位置
***/
u8 GetUsart3_ReceDataLen(void)
{
	return (USART3_RECE_QUERY_LEN+1 - DMA1_Stream1->NDTR);
}


/***
*指示当前串口对应的队列的写指针指示的位置
***/
u8 GetUart4_ReceDataLen(void)
{
	return (USART4_RECE_QUERY_LEN+1 - DMA1_Stream2->NDTR);
}


/***
*指示当前串口对应的队列的写指针指示的位置
***/
u8 GetUart5_ReceDataLen(void)
{
	return (USART5_RECE_QUERY_LEN+1 - DMA1_Stream0->NDTR);
}


/***
*指示当前串口对应的队列的写指针指示的位置
***/
u8 GetUsart6_ReceDataLen(void)
{
	return (USART6_RECE_QUERY_LEN+1 - DMA2_Stream1->NDTR);
}





/***
*使用串口进行打印数据
***/
void USART_Printf_Config(void)
{
	#if(TEST_USART1_PRINTF)
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		
		//配置外设引脚为复用模式
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);		//设置PA9为AF模式，给USART1复用
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);		//设置PA10为AF模式，给USART1复用
	
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		//配置USART
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART1,&USART_InitStructure);
		
		USART_Cmd(USART1,ENABLE);
	#elif(TEST_USART2_PRINTF)
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		
		//配置外设引脚为复用模式
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);		//设置PA2为AF模式，给USART2复用
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);		//设置PA3为AF模式，给USART2复用
	
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		//配置USART
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART2,&USART_InitStructure);
		
		USART_Cmd(USART2,ENABLE);
	#elif(TEST_USART3_PRINTF)
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		
		//配置外设引脚为复用模式
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);		//设置PB10为AF模式，给USART3复用
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);		//设置PB11为AF模式，给USART3复用
	
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		//配置USART
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART3,&USART_InitStructure);
		
		USART_Cmd(USART3,ENABLE);
	#elif(TEST_USART4_PRINTF)
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		
		//配置外设引脚为复用模式
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);		//设置PC10为AF模式，给USART4复用
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);		//设置PC11为AF模式，给USART4复用
	
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	
		//配置USART
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(UART4,&USART_InitStructure);
		
		USART_Cmd(UART4,ENABLE);
	#elif(TEST_USART5_PRINTF)
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
		
		//配置外设引脚为复用模式
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);		//设置PC12为AF模式，给USART5复用
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);		//设置PD2为AF模式，给USART5复用
	
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	
		//配置USART
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(UART5,&USART_InitStructure);
		
		USART_Cmd(UART5,ENABLE);
	#elif(TEST_USART6_PRINTF)
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
		
		//配置外设引脚为复用模式
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);		//设置PC6为AF模式，给USART6复用
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);		//设置PC7为AF模式，给USART6复用
	
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//复用模式
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	
		//配置USART
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART6,&USART_InitStructure);
		
		USART_Cmd(USART6,ENABLE);
	#endif
}



/***
*内部调用函数，注意要勾选OPTIONS中的USE Micro LIB选项
***/
int fputc(int ch,FILE *f)
{
	#if(TEST_USART1_PRINTF)
		USART_SendData(USART1,(u8)ch);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	#elif(TEST_USART2_PRINTF)
		USART_SendData(USART2,(u8)ch);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
	#elif(TEST_USART3_PRINTF)
		USART_SendData(USART3,(u8)ch);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
	#elif(TEST_USART4_PRINTF)
		USART_SendData(UART4,(u8)ch);
		while(USART_GetFlagStatus(UART4,USART_FLAG_TXE)==RESET);
	#elif(TEST_USART5_PRINTF)
		USART_SendData(UART5,(u8)ch);
		while(USART_GetFlagStatus(UART5,USART_FLAG_TXE)==RESET);
	#elif(TEST_USART6_PRINTF)
		USART_SendData(USART6,(u8)ch);
		while(USART_GetFlagStatus(USART6,USART_FLAG_TXE)==RESET);
	#endif
	
	return ch;
}











/*中断函数处理区**********************************************************************/
/***
*USART1的空闲中断处理函数，表明接收到了数据，并且存储到内存队列中了
***/
//void USART1_IRQHandler(void)
//{
//	extern Usart1_Rece_Query_s Usart1ReceQuery;
//	
//	//空闲中断处理
//	if(USART_GetITStatus(USART1,USART_IT_IDLE)==SET){
//		Usart1ReceQuery.write = GetUsart1_ReceDataLen();		//获得DMA当前已经传输的数据的个数，更新写指针
//		USART1->DR;												//清除IDLE中断标志，必须先读SR寄存器，然后再读DR寄存器即可自动清除空闲标志
//	}
//}


/***
*USART2的空闲中断处理函数，表明接收到了数据，并且存储到内存队列中了
***/
void USART2_IRQHandler(void)
{
	extern Usart2_Rece_Query_s Usart2ReceQuery;
	
	//空闲中断处理
	if(USART_GetITStatus(USART2,USART_IT_IDLE)==SET){
		Usart2ReceQuery.write = GetUsart2_ReceDataLen();		//获得DMA当前已经传输的数据的个数，更新写指针
		USART2->DR;												//清除IDLE中断标志，必须先读SR寄存器，然后再读DR寄存器即可自动清除空闲标志
	}
	
	
	/***
		由于DMA的TC中断触发时，只是表示已经将最后一个数据移入USART的DR中了，
		所以若立即将485的方向改为接收模式，则会使最后两个字节的数据丢失掉。
		所以需要先判断下USART的TC标志位，是否已经将最后一个数据发送出去了，
		若是，再切换485为接收模式
	***/
	if(USART_GetITStatus(USART2,USART_IT_TC)==SET){
		//数据发送完毕，将485重新处于接收模式
//		Ctrl_485_Send_Rece(USART2_485_PORT,USART2_485_PIN,USART_RECE);
		USART_ClearFlag(USART2,USART_FLAG_TC);
	}
}


/***
*USART3的空闲中断处理函数，表明接收到了数据，并且存储到内存队列中了
***/
void USART3_IRQHandler(void)
{
	extern Usart3_Rece_Query_s Usart3ReceQuery;
	
	//空闲中断处理
	if(USART_GetITStatus(USART3,USART_IT_IDLE)==SET){
		Usart3ReceQuery.write = GetUsart3_ReceDataLen();		//获得DMA当前已经传输的数据的个数，更新写指针
		USART3->DR;												//清除IDLE中断标志，必须先读SR寄存器，然后再读DR寄存器即可自动清除空闲标志
	}
}


/***
*UART4的空闲中断处理函数，表明接收到了数据，并且存储到内存队列中了
***/
void UART4_IRQHandler(void)
{
	extern Usart4_Rece_Query_s Usart4ReceQuery;
	
	//空闲中断处理
	if(USART_GetITStatus(UART4,USART_IT_IDLE)==SET){
		Usart4ReceQuery.write = GetUart4_ReceDataLen();			//获得DMA当前已经传输的数据的个数，更新写指针
		UART4->DR;												//清除IDLE中断标志，必须先读SR寄存器，然后再读DR寄存器即可自动清除空闲标志
	}
}


/***
*UART5的空闲中断处理函数，表明接收到了数据，并且存储到内存队列中了
***/
void UART5_IRQHandler(void)
{
	extern Usart5_Rece_Query_s Usart5ReceQuery;
	
	//空闲中断处理
	if(USART_GetITStatus(UART5,USART_IT_IDLE)==SET){
		Usart5ReceQuery.write = GetUart5_ReceDataLen();			//获得DMA当前已经传输的数据的个数，更新写指针
		UART5->DR;												//清除IDLE中断标志，必须先读SR寄存器，然后再读DR寄存器即可自动清除空闲标志
	}
}



/***
*USART6的空闲中断处理函数，表明接收到了数据，并且存储到内存队列中了
***/
void USART6_IRQHandler(void)
{
	extern Usart6_Rece_Query_s Usart6ReceQuery;
	
	//空闲中断处理
	if(USART_GetITStatus(USART6,USART_IT_IDLE)==SET){
		Usart6ReceQuery.write = GetUsart6_ReceDataLen();		//获得DMA当前已经传输的数据的个数，更新写指针
		USART6->DR;												//清除IDLE中断标志，必须先读SR寄存器，然后再读DR寄存器即可自动清除空闲标志
	}
}









