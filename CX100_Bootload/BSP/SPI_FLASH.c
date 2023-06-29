/**************************************************************************
SPI FLASH(以W25Q256为例)驱动例程，使用SPI4,PF7~PF9，CS-->PF6
建议：只使用DMA来进行读写数据，其他的等待写入完成都使用普通方式
***************************************************************************/
#include "SPI_FLASH.h"
#include "systick.h"





/*宏定义区******************************************************************/
#define FLASH_CS_HIGH									GPIOB->BSRRL |= GPIO_Pin_15
#define FLASH_CS_LOW									GPIOB->BSRRH |= GPIO_Pin_15
		
		
#define SPIFLASH_MAX_TIMEOUT							500000						//定义SPI读写一个字节最大的超时时间
#define SPIFLASH_WRITE_MAX_TIMEOUT						50							//定义SPIFLASH写一页最大的超时时间
#define SPIFLASH_SECTION_ERASE_MAX_TIMEOUT				1000						//定义SPIFLASH扇区擦除最大的超时时间
#define SPIFLASH_BLOCK_ERASE_MAX_TIMEOUT				5000						//定义SPIFLASH块擦除最大的超时时间
#define SPIFLASH_CHIP_ERASE_MAX_TIMEOUT					500000						//定义SPIFLASH整片擦除最大的超时时间

#define SPIFLASH_JEDEC_ID								0x00EF4019					//定义W25Q256的ID，这个是固定值，不同型号不一样




//指令码
#define SPIFLASH_CMD_READ_ID							0x9F						//用于读取flash的ID值的指令码
#define SPIFLASH_CMD_ENTER_4_ADR_MODE					0xB7						//用于快速更改flash为4字节地址模式的指令码
#define SPIFLASH_CMD_WRITE_ENABLE						0x06						//用于flash写使能的指令码
#define SPIFLASH_CMD_READ_STAT_REG1						0x05						//用于读取flash的状态寄存器1的指令码
#define SPIFLASH_CMD_READ_STAT_REG2						0x35						//用于读取flash的状态寄存器2的指令码
#define SPIFLASH_CMD_READ_STAT_REG3						0x15						//用于读取flash的状态寄存器3的指令码
#define SPIFLASH_CMD_WRITE_STAT_REG1					0x05						//用于写入flash的状态寄存器1的指令码
#define SPIFLASH_CMD_WRITE_STAT_REG2					0x35						//用于写入flash的状态寄存器2的指令码
#define SPIFLASH_CMD_WRITE_STAT_REG3					0x15						//用于写入flash的状态寄存器3的指令码
#define SPIFLASH_CMD_RESET_ENABLE						0x66						//用于flash重启使能的指令码
#define SPIFLASH_CMD_RESET								0x99						//用于flash重启的指令码
#define SPIFLASH_CMD_READ_DATA							0x0C						//用于flash快速读取数据的指令码
#define SPIFLASH_CMD_SECTION_ERASE						0x21						//用于flash扇区擦除的指令码
#define SPIFLASH_CMD_BLOCK_ERASE						0xDC						//用于flash块擦除的指令码
#define SPIFLASH_CMD_CHIP_ERASE							0xC7						//用于flash整片擦除的指令码
#define SPIFLASH_CMD_WRITE_DATA							0x12						//用于flash写入数据的指令码



/*全局变量定义区***********************************************************/
uint8_t InvalidValue = 0xFF;					//定义无效值
uint8_t SPIFlashDmaTCFlag = 0;					//用于表示DMA发送或者接收是否完成标志，为1表示已经完成




/*内部函数区****************************************************************/
/***
*配置主机spi发送TX对应的DMA2数据流4通道5
***/
static void _SPI_Tx_Dma2S4C5_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	//使能DMA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
//	DMA_InitStructure.DMA_BufferSize = 10;	            										//每次传输的数据的个数，传输完时触发中断
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t);   										//内存地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI4->DR;                 			//外设地址
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;												//选择通道，一个数据流只能配置给一个通道服务
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                         			//传输方向
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                			//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                         			//内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     				//外设每次传输的字节长度
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             				//内存每次传输的字节长度
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                 				//普通模式还是循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                             			//此DMA请求的优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                          			//不开启FIFO模式，选择直接传输数据模式
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;           			//FIFO触发的阈值，1/4，1/2,3/4,1/1
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                     			//内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;             			//外设是单次传输还是突发传输
	DMA_Init(DMA2_Stream4, &DMA_InitStructure);			
				
	NVIC_EnableIRQ(DMA2_Stream4_IRQn);															//打开NVIC中对应的DMA数据流4的中断通道
	DMA_ITConfig(DMA2_Stream4, DMA_IT_TC, DISABLE);												//需要时再打开DMA数据流4的数据传输完成中断
	
//	//开启DMA2的数据流4，需要开启的时候再开启
//	DMA_Cmd(DMA2_Stream4, ENABLE);
}


/***
*配置主机spi接收RX对应的DMA2数据流3通道5
***/
static void _SPI_Rx_Dma2S3C5_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	//使能DMA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
//	DMA_InitStructure.DMA_BufferSize = 10;	            										//每次传输的数据的个数，传输完时触发中断
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t);   										//内存地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI4->DR;                 			//外设地址
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;												//选择通道，一个数据流只能配置给一个通道服务
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                         			//传输方向
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                			//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                         			//内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     				//外设每次传输的字节长度
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             				//内存每次传输的字节长度
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                 				//普通模式还是循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                             			//此DMA请求的优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                          			//不开启FIFO模式，选择直接传输数据模式
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;           			//FIFO触发的阈值，1/4，1/2,3/4,1/1
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                     			//内存是单次传输和突发传输，突发传输分为：4,8,16次连续传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;             			//外设是单次传输还是突发传输
	DMA_Init(DMA2_Stream3, &DMA_InitStructure);			
				
	NVIC_EnableIRQ(DMA2_Stream3_IRQn);															//打开NVIC中对应的DMA数据流3的中断通道
	DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, DISABLE);												//需要时再打开DMA数据流3的数据传输完成中断
	
//	//开启DMA2的数据流3，需要开启的时候再开启
//	DMA_Cmd(DMA2_Stream3, ENABLE);
}





/***
*配置SPI的MISO,MOSI,SCK引脚为AF_PP即可，配置CS脚为OUT_PP模式
***/
static void _SPI_Pin_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource2, GPIO_AF_SPI4);		
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_SPI4);   	
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_SPI4);   	
    
	//配置SPI的SCK，MISO和MOSI引脚为复用推挽模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	//配置输出控制从机CS引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//先让控制从机1的NSS脚拉高
	FLASH_CS_HIGH;
	
}


/***
*配置SPI
***/
static void _SPI_Config(void)
{
	SPI_InitTypeDef   SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4,ENABLE);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//双线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						//SPI为主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					//传输的数据长度为8位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;							//SPI总线空闲时，sck线为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						//在SCK的奇数边沿采样数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							//主机的NSS软件控制模式
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;	//时钟分频值
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					//数据高位先行
	SPI_InitStructure.SPI_CRCPolynomial = 7;							//CRC校验多项式设置，数据传输的最后一个字节出加上此处的7，以完成CRC校验
	SPI_Init(SPI4, &SPI_InitStructure);
	
	//开启SPI的DMA请求
	SPI_I2S_DMACmd(SPI4,SPI_I2S_DMAReq_Tx,ENABLE);
	SPI_I2S_DMACmd(SPI4,SPI_I2S_DMAReq_Rx,ENABLE);
	
	SPI_Cmd(SPI4, ENABLE);
}




/***
*读写一个字节，用于写入一个字节，或者读出一个字节
*参数：
	Data：要写入的数据，或者为了读出一个字节而使用的无效字节驱动主机SPI发出时钟
*返回值：需要读回的一个字节，或者由于写入一个字节从而从机返回的无效数据
***/
static uint8_t _SPI_RW(uint8_t Data)
{
	uint32_t Count = 0;
	
	//判断TXE是否为1，避免出现覆盖数据的现象
	while((SPI4->SR & SPI_I2S_FLAG_TXE) == 0){
		Count++;
		if(Count>=SPIFLASH_MAX_TIMEOUT){
			return 0;
		}
	}
	SPI4->DR = Data;
	
	//判断RXNE是否为1，从而得知数据是否已经接收完成,当RXNE为1时，表明一轮数据收发完成了，可以按需要将NSS拉高了
	Count = 0;
	while((SPI4->SR & SPI_I2S_FLAG_RXNE) == 0){
		Count++;
		if(Count>=SPIFLASH_MAX_TIMEOUT){
			return 0;
		}
	}
	return SPI4->DR;
}




/***
*读取JEDEC ID，这个可以用于判断FLASH是否启动正常，并且型号是否一致
***/
static uint32_t _SPIFlash_ReadId(void)
{
	uint32_t Id = 0;
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_READ_ID);
	
	Id = _SPI_RW(InvalidValue)<<16;
	Id |= _SPI_RW(InvalidValue)<<8;
	Id |= _SPI_RW(InvalidValue);
	
	FLASH_CS_HIGH;
	return Id;
}




/***
*更改flash为4字节地址模式
***/
static void _SPIFlash_Set4AdrMode(void)
{
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_ENTER_4_ADR_MODE);
	
	FLASH_CS_HIGH;
}



/***
*写使能
***/
static void _SPIFlash_WriteEnable(void)
{
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_WRITE_ENABLE);
	
	FLASH_CS_HIGH;
}


///***
//*读取状态寄存器1
//***/
//static uint8_t _SPIFlash_ReadStatReg1(void)
//{
//	uint8_t StatReg1 = 0;
//	
//	FLASH_CS_LOW;
//	
//	_SPI_RW(SPIFLASH_CMD_READ_STAT_REG1);
//	
//	StatReg1 = _SPI_RW(InvalidValue);
//	
//	FLASH_CS_HIGH;
//	return StatReg1;
//}




///***
//*读取状态寄存器2
//***/
//static uint8_t _SPIFlash_ReadStatReg2(void)
//{
//	uint8_t StatReg2 = 0;
//	
//	FLASH_CS_LOW;
//	
//	_SPI_RW(SPIFLASH_CMD_READ_STAT_REG2);
//	
//	StatReg2 = _SPI_RW(InvalidValue);
//	
//	FLASH_CS_HIGH;
//	return StatReg2;
//}



/***
*读取状态寄存器3
***/
static uint8_t _SPIFlash_ReadStatReg3(void)
{
	uint8_t StatReg3 = 0;
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_READ_STAT_REG3);
	
	StatReg3 = _SPI_RW(InvalidValue);
	
	FLASH_CS_HIGH;
	return StatReg3;
}



///***
//*写入状态寄存器1
//***/
//static void _SPIFlash_WriteStatReg1(uint8_t RegValue)
//{
//	uint32_t Count = 0;
//	
//	//写使能
//	_SPIFlash_WriteEnable();
//	
//	FLASH_CS_LOW;
//	
//	_SPI_RW(SPIFLASH_CMD_WRITE_STAT_REG1);
//	
//	_SPI_RW(RegValue);
//	
//	FLASH_CS_HIGH;
//	
//	//判断写入是否完成,BUSY位==0
//	while((_SPIFlash_ReadStatReg1() & 0x01) != 0){
//		Count++;
//		if(Count > SPIFLASH_MAX_TIMEOUT){
//			break;
//		}
//	}
//}


///***
//*写入状态寄存器2
//***/
//static void _SPIFlash_WriteStatReg2(uint8_t RegValue)
//{
//	uint32_t Count = 0;
//	
//	//写使能
//	_SPIFlash_WriteEnable();
//	
//	FLASH_CS_LOW;
//	
//	_SPI_RW(SPIFLASH_CMD_WRITE_STAT_REG2);
//	
//	_SPI_RW(RegValue);
//	
//	FLASH_CS_HIGH;
//	
//	//判断写入是否完成,BUSY位==0
//	while((_SPIFlash_ReadStatReg1() & 0x01) != 0){
//		Count++;
//		if(Count > SPIFLASH_MAX_TIMEOUT){
//			break;
//		}
//	}
//}



///***
//*写入状态寄存器3
//***/
//static void _SPIFlash_WriteStatReg3(uint8_t RegValue)
//{
//	uint32_t Count = 0;
//	
//	//写使能
//	_SPIFlash_WriteEnable();
//	
//	FLASH_CS_LOW;
//	
//	_SPI_RW(SPIFLASH_CMD_WRITE_STAT_REG3);
//	
//	_SPI_RW(RegValue);
//	
//	FLASH_CS_HIGH;
//	
//	//判断写入是否完成,BUSY位==0
//	while((_SPIFlash_ReadStatReg1() & 0x01) != 0){
//		Count++;
//		if(Count > SPIFLASH_MAX_TIMEOUT){
//			break;
//		}
//	}
//}



/***
*等待BUSY位为0,非DMA方式循环读取状态寄存器1的值
***/
static ErrorStatus _SPIFlash_WaitBusy(uint32_t MaxTimeout)
{
	uint32_t Timestamp = Get_Stamp();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_READ_STAT_REG1);
	
	//判断写入是否完成,BUSY位==0
	while((_SPI_RW(InvalidValue) & 0x01) != 0){
		if(Time_MS(Timestamp,MaxTimeout)){
			FLASH_CS_HIGH;
			return ERROR;
		}
	}
	
	FLASH_CS_HIGH;
	return SUCCESS;
}





///***
//*等待BUSY位为0,使用DMA方式循环读取状态寄存器1的值，不建议使用
//***/
//static ErrorStatus _SPIFlash_DMA_WaitBusy(uint32_t MaxTimeout)
//{
//	uint8_t StatReg1 = 0xFF;
//	uint32_t Timestamp = Get_Stamp();
//	
//	//这里先将DMA流关闭
//	DMA2_Stream5->CR &= (~DMA_SxCR_EN);                 	//先关闭DMA2的数据流5
//	DMA2_Stream6->CR &= (~DMA_SxCR_EN);                 	//先关闭DMA2的数据流6
//	
//	FLASH_CS_LOW;
//	
//	_SPI_RW(SPIFLASH_CMD_READ_STAT_REG1);
//	
//	//清除标志
//	SPIFlashDmaTCFlag = 0;
//	StatReg1 = _SPI_RW(0xFF);
//	
//	//先配置接受数据DMA2流S5C7
//	while((DMA2_Stream5->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
//	DMA2->HIFCR |= 0x00000f40;								//将流对应的事件标志全部清零
//	DMA2_Stream5->M0AR = (uint32_t)&StatReg1;				//配置内存地址
//	DMA2_Stream5->NDTR = 1;									//重新配置将要传输的字节长度
//	DMA2_Stream5->CR &= ~DMA_SxCR_MINC;						//内存地址固定模式
//	DMA2_Stream5->CR |= DMA_SxCR_CIRC;						//循环模式
//	DMA2_Stream5->CR &= ~DMA_SxCR_TCIE;						//关闭TC中断
//	DMA2_Stream5->CR |= DMA_SxCR_EN;						//开启DMA2的数据流5
//	
//	//配置为了触发时钟的发送DMA2的S6C7
//	while((DMA2_Stream6->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
//	DMA2->HIFCR |= 0x003D0000;								//将流对应的事件标志全部清零
//	DMA2_Stream6->M0AR = (uint32_t)&InvalidValue;			//配置内存地址
//	DMA2_Stream6->NDTR = 1;									//重新配置将要传输的字节长度
//	DMA2_Stream6->CR &= ~DMA_SxCR_MINC;						//内存地址固定模式
//	DMA2_Stream6->CR |= DMA_SxCR_CIRC;						//循环模式
//	DMA2_Stream6->CR &= ~DMA_SxCR_TCIE;						//关闭TC中断
//	DMA2_Stream6->CR |= DMA_SxCR_EN;						//开启DMA2的数据流6
//	
//	while((StatReg1 & 0x01) != 0){
//		if(Time_MS(Timestamp,MaxTimeout)){
//			DMA2_Stream6->CR &= ~DMA_SxCR_EN;				//关闭DMA2的数据流6
//			DMA2_Stream5->CR &= ~DMA_SxCR_EN;				//关闭DMA2的数据流5
//			
//			//由于这里是直接中断循环模式，所以需要等待DMA关闭完成
//			while(DMA2_Stream6->CR & DMA_SxCR_EN);
//			while(DMA2_Stream5->CR & DMA_SxCR_EN);
//			
//			FLASH_CS_HIGH;
//			
//			return ERROR;
//		}
//	}
//	
//	DMA2_Stream6->CR &= ~DMA_SxCR_EN;						//关闭DMA2的数据流6
//	DMA2_Stream5->CR &= ~DMA_SxCR_EN;						//关闭DMA2的数据流5
//	
//	//由于这里是直接中断循环模式，所以需要等待DMA关闭完成
//	while(DMA2_Stream6->CR & DMA_SxCR_EN);
//	while(DMA2_Stream5->CR & DMA_SxCR_EN);
//	
//	FLASH_CS_HIGH;
//	
//	return SUCCESS;
//}



///***
//*写入一页数据，非DMA方式，注意，这里只管写入，不会判断参数是否合法，以及是否会
//越页界
//***/
//static void _SPIFlash_WritePageData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
//{
//	//写使能
//	_SPIFlash_WriteEnable();
//	
//	FLASH_CS_LOW;
//	
//	_SPI_RW(SPIFLASH_CMD_WRITE_DATA);
//	
//	//发送地址
//	_SPI_RW((Adr>>24));
//	_SPI_RW((Adr>>16));
//	_SPI_RW((Adr>>8));
//	_SPI_RW((Adr & 0x000000FF));
//	
//	while(Len--){
//		_SPI_RW(*Buf++);
//	}
//	
//	FLASH_CS_HIGH;
//}



/***
*写入一页数据，DMA方式，注意，这里只管写入，不会判断参数是否合法，以及是否会
越页界
***/
static void _SPIFlash_DMA_WritePageData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	//写使能
	_SPIFlash_WriteEnable();
	
	//这里先将DMA流关闭
	DMA2_Stream3->CR &= (~DMA_SxCR_EN);                 	//先关闭DMA2的数据流3
	DMA2_Stream4->CR &= (~DMA_SxCR_EN);                 	//先关闭DMA2的数据流4
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_WRITE_DATA);
	
	//发送地址
	_SPI_RW((Adr>>24));
	_SPI_RW((Adr>>16));
	_SPI_RW((Adr>>8));
	_SPI_RW((Adr & 0x000000FF));
	
	//清除标志
	SPIFlashDmaTCFlag = 0;
	
	//这里确保将SPI的RXNE位清零，防止下面一开启DMA接收通道就把之前的数据接收进来了，导致数据错位
	SPI4->DR;
	
	//先配置接受数据DMA2流S5C7
	while((DMA2_Stream3->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
	DMA2->LIFCR |= 0x0f400000;								//将流对应的事件标志全部清零
	DMA2_Stream3->CR |= DMA_IT_TC;							//开启TC中断
	DMA2_Stream3->M0AR = (uint32_t)&InvalidValue;			//配置内存地址
	DMA2_Stream3->NDTR = Len;								//重新配置将要传输的字节长度
	DMA2_Stream3->CR &= ~DMA_SxCR_MINC;						//内存地址增加模式
	DMA2_Stream3->CR |= DMA_SxCR_EN;						//开启DMA2的数据流5
	
	//配置为了触发时钟的发送DMA2的S6C7
	while((DMA2_Stream4->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
	DMA2->HIFCR |= 0x0000003D;								//将流对应的事件标志全部清零
	DMA2_Stream4->CR &= ~DMA_IT_TC;							//关闭TC中断
	DMA2_Stream4->M0AR = (uint32_t)Buf;						//配置内存地址
	DMA2_Stream4->NDTR = Len;								//重新配置将要传输的字节长度
	DMA2_Stream4->CR |= DMA_SxCR_MINC;						//内存地址固定模式
	DMA2_Stream4->CR |= DMA_SxCR_EN;						//开启DMA2的数据流6
	
	//等待传输完成
	while(SPIFlashDmaTCFlag == 0);
	
	FLASH_CS_HIGH;
}













/*外部函数区****************************************************************/
/***
*初始化SPI FLASH
***/
ErrorStatus SPIFlash_Init(void)
{
	_SPI_Tx_Dma2S4C5_Config();
	_SPI_Rx_Dma2S3C5_Config();
	_SPI_Pin_Config();
	_SPI_Config();
	
	//读取ID值
	if(_SPIFlash_ReadId() != SPIFLASH_JEDEC_ID){
		return ERROR;
	}
	
	//读取状态寄存器3的值，判断是否已经处于4字节地址模式
	if((_SPIFlash_ReadStatReg3() & 0x01) == 0){
		//更改为4字节地址模式
		_SPIFlash_Set4AdrMode();
	}
	
	return SUCCESS;
}



/***
*用于判断SPI FLASH是否已经准备完成
***/
ErrorStatus SPIFlash_IsReady(void)
{
	//读取ID值
	if(_SPIFlash_ReadId() != SPIFLASH_JEDEC_ID){
		return ERROR;
	}else{
		return SUCCESS;
	}
}



/***
*重启FLASH
***/
void SPIFlash_Reset(void)
{
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_RESET_ENABLE);
	
	FLASH_CS_HIGH;
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_RESET);
	
	FLASH_CS_HIGH;
}



/***
*读取数据，非DMA读取模式
***/
ErrorStatus SPIFlash_ReadData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	if(Len == 0){
		return ERROR;
	}
	
	//最大只能读取到芯片结尾处，否则会重新回到开头处读取
	if(Adr+Len > SPIFLASH_VOLUME_SIZE){
		Len = SPIFLASH_VOLUME_SIZE - Adr;
	}
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_READ_DATA);
	
	//发送地址
	_SPI_RW((Adr>>24));
	_SPI_RW((Adr>>16));
	_SPI_RW((Adr>>8));
	_SPI_RW((Adr & 0x000000FF));
	
	_SPI_RW(InvalidValue);
	
	while(Len--){
		*Buf++ = _SPI_RW(InvalidValue);
	}
	
	FLASH_CS_HIGH;
	
	return SUCCESS;
}



/***
*读取数据，DMA方式读取模式
***/
ErrorStatus SPIFlash_DMA_ReadData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	extern uint8_t SPIFlashDmaTCFlag;
	uint16_t TempLen = 0;
	InvalidValue = 0xff;
	
	if(Len == 0){
		return ERROR;
	}
	
	//最大只能读取到芯片结尾处，否则会重新回到开头处读取
	if(Adr+Len > SPIFLASH_VOLUME_SIZE){
		Len = SPIFLASH_VOLUME_SIZE - Adr;
	}
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_READ_DATA);
	
	//发送地址
	_SPI_RW((Adr>>24));
	_SPI_RW((Adr>>16));
	_SPI_RW((Adr>>8));
	_SPI_RW((Adr & 0x000000FF));
	
	_SPI_RW(InvalidValue);
	
	//这里确保将SPI的RXNE位清零，防止下面一开启DMA接收通道就把之前的数据接收进来了，导致数据错位
	SPI4->DR;
	
	while(Len){
		//清除标志
		SPIFlashDmaTCFlag = 0;
		
		//这里先将DMA流关闭
		DMA2_Stream3->CR &= (~DMA_SxCR_EN);                 	//先关闭DMA2的数据流5
		DMA2_Stream4->CR &= (~DMA_SxCR_EN);                 	//先关闭DMA2的数据流6
	
		if(Len > 65535){
			TempLen = 65535;
		}else{
			TempLen = Len;
		}
		Len -= TempLen;
		
		//先配置接受数据DMA2流S5C7
		while((DMA2_Stream3->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
		DMA2->LIFCR |= 0x0f400000;								//将流对应的事件标志全部清零
		DMA2_Stream3->CR |= DMA_IT_TC;							//开启TC中断
		DMA2_Stream3->M0AR = (uint32_t)Buf;						//配置内存地址
		DMA2_Stream3->NDTR = TempLen;							//重新配置将要传输的字节长度
		DMA2_Stream3->CR |= DMA_SxCR_MINC;						//内存地址增加模式
		DMA2_Stream3->CR |= DMA_SxCR_EN;						//开启DMA2的数据流5
		
		//配置为了触发时钟的发送DMA2的S6C7
		while((DMA2_Stream4->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
		DMA2->HIFCR |= 0x0000003D;								//将流对应的事件标志全部清零
		DMA2_Stream4->CR &= ~DMA_IT_TC;							//关闭TC中断
		DMA2_Stream4->M0AR = (uint32_t)&InvalidValue;			//配置内存地址
		DMA2_Stream4->NDTR = TempLen;							//重新配置将要传输的字节长度
		DMA2_Stream4->CR &= ~DMA_SxCR_MINC;						//内存地址固定模式
		DMA2_Stream4->CR |= DMA_SxCR_EN;						//开启DMA2的数据流6
		
		//等待传输完成
		while(SPIFlashDmaTCFlag == 0);
		
		//更新Buf地址
		Buf += TempLen;
	}
	
	FLASH_CS_HIGH;
	
	return SUCCESS;
}



/***
*扇区为单位读取数据，给FatFs文件系统使用
***/
ErrorStatus SPIFlash_DMA_SectorReadData(uint8_t* Buf,uint32_t SectorAdr,uint32_t SectorNum)
{
	return SPIFlash_DMA_ReadData(SectorAdr*SPIFLASH_SECTION_SIZE,Buf,SectorNum*SPIFLASH_SECTION_SIZE);
}






/***
*扇区擦除，地址需要对齐4KB边界
***/
void SPIFlash_EraseSection(uint32_t Adr)
{
	//先将地址对齐到4KB边界
	Adr -= (Adr % (SPIFLASH_SECTION_SIZE));
	
	//写使能
	_SPIFlash_WriteEnable();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_SECTION_ERASE);
	
	//发送地址
	_SPI_RW((Adr>>24));
	_SPI_RW((Adr>>16));
	_SPI_RW((Adr>>8));
	_SPI_RW((Adr & 0x000000FF));
	
	FLASH_CS_HIGH;
	
	//等待执行完毕
	_SPIFlash_WaitBusy(SPIFLASH_SECTION_ERASE_MAX_TIMEOUT);
}



/***
*块擦除，地址需要对齐64KB边界
***/
void SPIFlash_EraseBlock(uint32_t Adr)
{
	//先将地址对齐到64KB边界
	Adr -= (Adr % SPIFLASH_BLOCK_SIZE);
	
	//写使能
	_SPIFlash_WriteEnable();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_BLOCK_ERASE);
	
	//发送地址
	_SPI_RW((Adr>>24));
	_SPI_RW((Adr>>16));
	_SPI_RW((Adr>>8));
	_SPI_RW((Adr & 0x000000FF));
	
	FLASH_CS_HIGH;
	
	//等待执行完毕
	_SPIFlash_WaitBusy(SPIFLASH_BLOCK_ERASE_MAX_TIMEOUT);
}



/***
*整片擦除
***/
void SPIFlash_EraseChip(void)
{
	//写使能
	_SPIFlash_WriteEnable();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_CHIP_ERASE);
	
	FLASH_CS_HIGH;
	
	//等待执行完毕
	_SPIFlash_WaitBusy(SPIFLASH_CHIP_ERASE_MAX_TIMEOUT);
}




/***
*写入数据，非DMA方式写入。
*注意不能跨页写入，每页大小为256字节，并且注意，当地址为一个扇区的首地址时，
则先擦除该扇区再写入数据，当地址不是扇区的首地址时，则默认不用擦除直接写入数据即可
***/
ErrorStatus SPIFlash_WriteData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	uint16_t SectionNum = 0;
	uint32_t Temp = 0;
	
	//先判断参数是否合法
	if(Len == 0){
		return ERROR;
	}
	
	if(Adr + Len > SPIFLASH_VOLUME_SIZE){
		Len = SPIFLASH_VOLUME_SIZE - Adr;
	}
	
	//提前将需要写入数据的扇区擦除
	if((Adr % SPIFLASH_SECTION_SIZE) == 0){				//写入起始地址刚好对齐扇区
		//计算一共需要多少扇区来写入，以便提前将扇区擦除好
		SectionNum = Len / SPIFLASH_SECTION_SIZE;
		SectionNum += (Len % SPIFLASH_SECTION_SIZE)==0?0:1;
		
		Temp = Adr;
		
		//擦除扇区
		while(SectionNum--){
			SPIFlash_EraseSection(Temp);
			Temp += SPIFLASH_SECTION_SIZE;
		}
	}else{												//写入起始地址没有对齐扇区
		Temp = Adr % SPIFLASH_SECTION_SIZE;
		if(Temp+Len > SPIFLASH_SECTION_SIZE){
			Temp = Temp+Len - SPIFLASH_SECTION_SIZE;
			
			SectionNum = Temp / SPIFLASH_SECTION_SIZE;
			SectionNum += (Temp % SPIFLASH_SECTION_SIZE)==0?0:1;
			
			Temp = ((Adr/SPIFLASH_SECTION_SIZE)+1)*SPIFLASH_SECTION_SIZE;
			
			//擦除扇区
			while(SectionNum--){
				SPIFlash_EraseSection(Temp);
				Temp += SPIFLASH_SECTION_SIZE;
			}
		}
	}
	
	//开始写入数据
	while(Len){
		if((Adr % SPIFLASH_PAGE_SIZE) == 0){	//写入地址对齐页首
			if(Len > SPIFLASH_PAGE_SIZE){				//剩余数据大于一页
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);					//等待上一次写入完成
				_SPIFlash_DMA_WritePageData(Adr,Buf,SPIFLASH_PAGE_SIZE);
				Adr += SPIFLASH_PAGE_SIZE;
				Buf += SPIFLASH_PAGE_SIZE;
				Len -= SPIFLASH_PAGE_SIZE;
			}else{								//剩余数据小于等于一页
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);					//等待上一次写入完成
				_SPIFlash_DMA_WritePageData(Adr,Buf,Len);
				Adr += Len;
				Buf += Len;
				Len = 0;
			}
		}else{											//写入地址没有对齐页首
			Temp = Adr % SPIFLASH_PAGE_SIZE;
			Temp = SPIFLASH_PAGE_SIZE - Temp;
			
			if(Len > Temp){
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);						//等待上一次写入完成
				_SPIFlash_DMA_WritePageData(Adr,Buf,Temp);
				Adr += Temp;
				Buf += Temp;
				Len -= Temp;
			}else{
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);						//等待上一次写入完成
				_SPIFlash_DMA_WritePageData(Adr,Buf,Len);
				Adr += Temp;
				Buf += Temp;
				Len = 0;
			}
		}
	}
	
	_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);
	return SUCCESS;
}




/***
*扇区为单位写入数据，给FatFS写入数据使用
***/
ErrorStatus SPIFlash_SectorWriteData(uint8_t* Buf,uint32_t SectorAdr,uint32_t SectorNum)
{
	return SPIFlash_WriteData(SectorAdr*SPIFLASH_SECTION_SIZE,Buf,SectorNum*SPIFLASH_SECTION_SIZE);
}







