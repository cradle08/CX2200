/**************************************************************************
SPI FLASH(以W25Q256为例)驱动例程，使用SPI4
建议：只使用DMA来进行读写数据，其他的等待写入完成都使用普通方式
***************************************************************************/
#include "bsp_spi.h"
#include "main.h"
#include "bsp_outin.h"


/*宏定义区******************************************************************/
#define FLASH_CS_HIGH									SPI_CS_Disable(SPI_CS_FLASH)  //FLASH_SPI_CS_GPIO_Port->BSRR |= FLASH_SPI_CS_Pin    //    
#define FLASH_CS_LOW									SPI_CS_Enable(SPI_CS_FLASH)   //FLASH_SPI_CS_GPIO_Port->BSRR |= (uint32_t)FLASH_SPI_CS_Pin<<16   


		
		
#define SPIFLASH_MAX_TIMEOUT							500000						//定义SPI读写一个字节最大的超时时间
#define SPIFLASH_WRITE_MAX_TIMEOUT						50							//定义SPIFLASH写一页最大的超时时间
#define SPIFLASH_SECTION_ERASE_MAX_TIMEOUT				1000						//定义SPIFLASH扇区擦除最大的超时时间
#define SPIFLASH_BLOCK_ERASE_MAX_TIMEOUT				5000						//定义SPIFLASH块擦除最大的超时时间
#define SPIFLASH_CHIP_ERASE_MAX_TIMEOUT					500000						//定义SPIFLASH整片擦除最大的超时时间






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

#define SPI_SPEED_2										0x0038						//&上此值的反值，则将SPI波特率更改为2分频
#define SPI_SPEED_256									0x0038						//|上此值，则将SPI波特率更改为256分频




/*全局变量定义区***********************************************************/
uint8_t InvalidValue = 0xFF;														//定义无效值
__IO uint8_t SPIFlashDmaTCFlag = 0;													//用于表示DMA发送或者接收是否完成标志，为1表示已经完成
uint8_t SPI4NextModeFlag = SPI_MODE_END;											//用于表示SPI4下一次通讯将要处于什么模式状态，为0表示将要处于与XPT2046通讯的状态，为1表示将要处于与SPI FLASH通讯状态，为2表示将要处于与AD数字电位器通讯状态
uint8_t SPI4ResourceNum = 1;														//用于表示当前SPI4是否被占用中，为0表示正在被占用中，为1表示没有被占用
uint32_t SPI4ResurceUsed = 0;


/*内部函数区****************************************************************/
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
	while((SPI4->SR & SPI_FLAG_TXE) == 0){
		Count++;
		if(Count>=SPIFLASH_MAX_TIMEOUT){
			return 0;
		}
	}
	SPI4->DR = Data;
	
	//判断RXNE是否为1，从而得知数据是否已经接收完成,当RXNE为1时，表明一轮数据收发完成了，可以按需要将NSS拉高了
	Count = 0;
	while((SPI4->SR & SPI_FLAG_RXNE) == 0){
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
uint32_t _SPIFlash_ReadId(void)
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
	uint32_t Timestamp = HAL_GetTick();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_READ_STAT_REG1);
	
	//判断写入是否完成,BUSY位==0
	while((_SPI_RW(InvalidValue) & 0x01) != 0){
		if((HAL_GetTick()-Timestamp) > MaxTimeout){
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
//	DMA2_Stream3->CR &= (~DMA_SxCR_EN);                 	//先关闭DMA2的数据流5
//	DMA2_Stream4->CR &= (~DMA_SxCR_EN);                 	//先关闭DMA2的数据流6
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
//	while((DMA2_Stream3->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
//	DMA2->LIFCR |= 0x0F400000;								//将流对应的事件标志全部清零
//	DMA2_Stream3->M0AR = (uint32_t)&StatReg1;				//配置内存地址
//	DMA2_Stream3->NDTR = 1;									//重新配置将要传输的字节长度
//	DMA2_Stream3->CR &= ~DMA_SxCR_MINC;						//内存地址固定模式
//	DMA2_Stream3->CR |= DMA_SxCR_CIRC;						//循环模式
//	DMA2_Stream3->CR &= ~DMA_SxCR_TCIE;						//关闭TC中断
//	DMA2_Stream3->CR |= DMA_SxCR_EN;						//开启DMA2的数据流5
//	
//	//配置为了触发时钟的发送DMA2的S6C7
//	while((DMA2_Stream4->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
//	DMA2->HIFCR |= 0x0000003D;								//将流对应的事件标志全部清零
//	DMA2_Stream4->M0AR = (uint32_t)&InvalidValue;			//配置内存地址
//	DMA2_Stream4->NDTR = 1;									//重新配置将要传输的字节长度
//	DMA2_Stream4->CR &= ~DMA_SxCR_MINC;						//内存地址固定模式
//	DMA2_Stream4->CR |= DMA_SxCR_CIRC;						//循环模式
//	DMA2_Stream4->CR &= ~DMA_SxCR_TCIE;						//关闭TC中断
//	DMA2_Stream4->CR |= DMA_SxCR_EN;						//开启DMA2的数据流6
//	
//	while((StatReg1 & 0x01) != 0){
//		if(Time_MS(Timestamp,MaxTimeout)){
//			DMA2_Stream4->CR &= ~DMA_SxCR_EN;				//关闭DMA2的数据流6
//			DMA2_Stream3->CR &= ~DMA_SxCR_EN;				//关闭DMA2的数据流5
//			
//			//由于这里是直接中断循环模式，所以需要等待DMA关闭完成
//			while(DMA2_Stream4->CR & DMA_SxCR_EN);
//			while(DMA2_Stream3->CR & DMA_SxCR_EN);
//			
//			FLASH_CS_HIGH;
//			
//			return ERROR;
//		}
//	}
//	
//	DMA2_Stream4->CR &= ~DMA_SxCR_EN;						//关闭DMA2的数据流6
//	DMA2_Stream3->CR &= ~DMA_SxCR_EN;						//关闭DMA2的数据流5
//	
//	//由于这里是直接中断循环模式，所以需要等待DMA关闭完成
//	while(DMA2_Stream4->CR & DMA_SxCR_EN);
//	while(DMA2_Stream3->CR & DMA_SxCR_EN);
//	
//	FLASH_CS_HIGH;
//	
//	return SUCCESS;
//}



/***
*写入一页数据，非DMA方式，注意，这里只管写入，不会判断参数是否合法，以及是否会
越页界
***/
static void _SPIFlash_WritePageData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	//写使能
	_SPIFlash_WriteEnable();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_WRITE_DATA);
	
	//发送地址
	_SPI_RW((Adr>>24));
	_SPI_RW((Adr>>16));
	_SPI_RW((Adr>>8));
	_SPI_RW((Adr & 0x000000FF));
	
	while(Len--){
		_SPI_RW(*Buf++);
	}
	
	FLASH_CS_HIGH;
}



/***
*写入一页数据，DMA方式，注意，这里只管写入，不会判断参数是否合法，以及是否会
越页界
***/
static void _SPIFlash_DMA_WritePageData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	osKernelLock();
	
	//写使能
	_SPIFlash_WriteEnable();
	
	//这里先将DMA流关闭
	DMA2_Stream3->CR &= (~DMA_SxCR_EN);                 	//先关闭DMA2的数据流5
	DMA2_Stream4->CR &= (~DMA_SxCR_EN);                 	//先关闭DMA2的数据流6
	
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
	
	//先配置接受数据DMA2流S3C5
	while((DMA2_Stream3->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
	DMA2->LIFCR |= 0x0F400000;								//将流对应的事件标志全部清零
	DMA2_Stream3->CR |= DMA_IT_TC;							//开启TC中断
	DMA2_Stream3->M0AR = (uint32_t)&InvalidValue;			//配置内存地址
	DMA2_Stream3->NDTR = Len;								//重新配置将要传输的字节长度
	DMA2_Stream3->CR &= ~DMA_SxCR_MINC;						//内存地址固定模式
	DMA2_Stream3->CR |= DMA_SxCR_EN;						//开启DMA2的数据流5
	
	//配置为了触发时钟的发送DMA2的S4C5
	while((DMA2_Stream4->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
	DMA2->HIFCR |= 0x0000003D;								//将流对应的事件标志全部清零
	DMA2_Stream4->CR &= ~DMA_IT_TC;							//关闭TC中断
	DMA2_Stream4->M0AR = (uint32_t)Buf;						//配置内存地址
	DMA2_Stream4->NDTR = Len;								//重新配置将要传输的字节长度
	DMA2_Stream4->CR |= DMA_SxCR_MINC;						//内存地址增加模式
	DMA2_Stream4->CR |= DMA_SxCR_EN;						//开启DMA2的数据流6
	
	//等待传输完成
	while(SPIFlashDmaTCFlag == 0);
	
	FLASH_CS_HIGH;
	
	osKernelUnlock();
}



 
//HAL_StatusTypeDef

/*
*   切换SPI4的模式，
*/
SPI_MODE_e SPI4_ChangeModeTo(SPI_MODE_e eMode)
{
    extern SPI_HandleTypeDef hspi4;
    extern uint8_t SPI4NextModeFlag;

    //
    osKernelLock();
    if(SPI4NextModeFlag == eMode)
    {
        return eMode;
    }
    
    //设置SPI参数
	hspi4.Instance->CR1 &= ~SPI_CR1_SPE;
    switch(eMode)
    {
        case SPI_MODE_TP:
        case SPI_MODE_DR:
        {
        	//分频改为256
            hspi4.Instance->CR1 |= SPI_SPEED_256;
            //开启SPI
            hspi4.Instance->CR1 |= SPI_CR1_SPE;
        }
        break;
        case SPI_MODE_FLASH:
        {
            //分频改为4
            hspi4.Instance->CR1 &= ~SPI_SPEED_2;
            hspi4.Instance->CR1 |= 0x0008;
            //开启SPI
            hspi4.Instance->CR1 |= SPI_CR1_SPE;
        }
        break;
        default:break;  
    }
    SPI4NextModeFlag = eMode;
    
    return eMode;
}


/*
*   切换SPI4的模式到默认TP模式，
*/
void SPI4_ExitModeToDefault(void)
{
    extern SPI_HandleTypeDef hspi4;
    extern uint8_t SPI4NextModeFlag;

    //
    if(SPI4NextModeFlag == SPI_MODE_TP)
    {
        osKernelUnlock();
        return;
    }
    
    //默认处于触摸模式下
    SPI4NextModeFlag = SPI_MODE_TP;
    
    //先关闭SPI 
    hspi4.Instance->CR1 &= ~SPI_CR1_SPE;
    //分频改为256
    hspi4.Instance->CR1 |= SPI_SPEED_256;
    //开启SPI
    hspi4.Instance->CR1 |= SPI_CR1_SPE;
    
    osKernelUnlock();
}



/*外部函数区****************************************************************/
/***
*切换SPI4模式与SPI FLASH进行通讯：分频2，模式：0，0
***/
void SPI4_ChangeModeToFlash(void)
{
	extern SPI_HandleTypeDef hspi4;
	extern uint8_t SPI4NextModeFlag;
	extern uint8_t SPI4ResourceNum;
	
	//先判断SPI4是否已经处于空闲
	while(SPI4ResourceNum == 0){
		SPI4NextModeFlag = SPI_MODE_FLASH;
		osDelay(2);
	}
	
	//让FLASH获取SPI4控制权
	SPI4ResourceNum = 0;
	

}


/***
*切换SPI4模式与数字电位器进行通讯：分频256，模式：0，0
***/
void SPI4_ChangeModeToDR(void)
{
	extern SPI_HandleTypeDef hspi4;
	extern uint8_t SPI4NextModeFlag;
	extern uint8_t SPI4ResourceNum;
	
	//先判断SPI4是否已经处于空闲
	while(SPI4ResourceNum == 0){
		SPI4NextModeFlag = SPI_MODE_DR;
		osDelay(2);
	}
	
	//让FLASH获取SPI4控制权
	SPI4ResourceNum = 0;
	
	//先关闭SPI 
	hspi4.Instance->CR1 &= ~SPI_CR1_SPE;
	
	//分频改为256
	hspi4.Instance->CR1 |= SPI_SPEED_256;
	
	//开启SPI
	hspi4.Instance->CR1 |= SPI_CR1_SPE;
}




/***
*切换SPI4资源为空闲状态，一般FLASH，AD数字电位器用完后需要切换为空闲状态，触摸用完后则不需要切换为空闲状态
***/
void SPI4_ChangeModeToIDLE(void)
{
	extern uint8_t SPI4NextModeFlag;
	extern uint8_t SPI4ResourceNum;
	
	SPI4NextModeFlag = SPI_MODE_END;
	SPI4ResourceNum = 1;
}



/***
*切换SPI4模式与电阻屏进行通讯：分频256，模式：0，0
***/
HAL_StatusTypeDef SPI4_ChangeModeToXPT2046(void)
{
	extern SPI_HandleTypeDef hspi4;
	extern uint8_t SPI4NextModeFlag;
	extern uint8_t SPI4ResourceNum;
	
	//判断SPI4是否已经被占用中
	if(SPI4ResourceNum == 0){
		return HAL_ERROR;
	}
	
	//判断下一次SPI4是否要被FLASH或者数字电位器占用，是，则触摸暂停使用
	if(SPI4NextModeFlag == SPI_MODE_FLASH || SPI4NextModeFlag == SPI_MODE_DR){
		return HAL_ERROR;
	}
	
	//判断当前SPI4是否已经被触摸占用中，是，则直接返回成功即可
	if(SPI4NextModeFlag == SPI_MODE_TP){
		SPI4ResourceNum = 0;
		return HAL_OK;
	}
	
	//触摸占用SPI4
	SPI4ResourceNum = 0;
	SPI4NextModeFlag = SPI_MODE_TP;
	
	//先关闭SPI 
	hspi4.Instance->CR1 &= ~SPI_CR1_SPE;
	
	//分频改为256
	hspi4.Instance->CR1 |= SPI_SPEED_256;
	
	//开启SPI
	hspi4.Instance->CR1 |= SPI_CR1_SPE;

	return HAL_OK;
}





/***
*初始化SPI FLASH
***/
ErrorStatus SPIFlash_Init(void)
{
    uint8_t ucNum = 3;
    
	//更改SPI模式
	SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
	
	//读取ID值
    do{
        if(_SPIFlash_ReadId() != SPIFLASH_JEDEC_ID){
            //更改SPI模式, 为低速模式
            LOG_Error("SPI Flash Conn Fail");
            //return ERROR;
        }    
    }while(ucNum--);
    
	
	//读取状态寄存器3的值，判断是否已经处于4字节地址模式
	if((_SPIFlash_ReadStatReg3() & 0x01) == 0){
		//更改为4字节地址模式
		_SPIFlash_Set4AdrMode();
	}
	
	//更改SPI模式
	SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
	
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
	
	HAL_Delay(100);
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_RESET);
	
	FLASH_CS_HIGH;
}



/***
*读取数据，非DMA读取模式
***/
ErrorStatus SPIFlash_ReadData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	if(Len == 0 || Adr > SPIFLASH_VOLUME_SIZE){
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
	extern __IO uint8_t SPIFlashDmaTCFlag;
	uint16_t TempLen = 0;
	InvalidValue = 0xff;
	
	if(Len == 0 || Adr > SPIFLASH_VOLUME_SIZE){
		return ERROR;
	}
	
	//最大只能读取到芯片结尾处，否则会重新回到开头处读取
	if(Adr+Len > SPIFLASH_VOLUME_SIZE){
		return ERROR;
	}
	
	osKernelLock();
	
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
		
		//先配置接受数据DMA2流S3C5
		while((DMA2_Stream3->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
		DMA2->LIFCR |= 0x0F400000;								//将流对应的事件标志全部清零
		DMA2_Stream3->CR |= DMA_IT_TC;							//开启TC中断
		DMA2_Stream3->M0AR = (uint32_t)Buf;						//配置内存地址
		DMA2_Stream3->NDTR = TempLen;								//重新配置将要传输的字节长度
		DMA2_Stream3->CR |= DMA_SxCR_MINC;						//内存地址增加模式
		DMA2_Stream3->CR |= DMA_SxCR_EN;						//开启DMA2的数据流5
		
		//配置为了触发时钟的发送DMA2的S4C5
		while((DMA2_Stream4->CR & DMA_SxCR_EN) != 0);			//等待EN位为0才可以开始配置DMA
		DMA2->HIFCR |= 0x0000003D;								//将流对应的事件标志全部清零
		DMA2_Stream4->CR &= ~DMA_IT_TC;							//关闭TC中断
		DMA2_Stream4->M0AR = (uint32_t)&InvalidValue;			//配置内存地址
		DMA2_Stream4->NDTR = TempLen;								//重新配置将要传输的字节长度
		DMA2_Stream4->CR &= ~DMA_SxCR_MINC;						//内存地址固定模式
		DMA2_Stream4->CR |= DMA_SxCR_EN;						//开启DMA2的数据流6
		
		//等待传输完成
		while(SPIFlashDmaTCFlag == 0);
		
		//更新Buf地址
		Buf += TempLen;
	}
	
	FLASH_CS_HIGH;
	
	osKernelUnlock();
	
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
*写入数据，DMA方式写入。
*注意不能跨页写入，每页大小为256字节，并且注意，当地址为一个扇区的首地址时，
则先擦除该扇区再写入数据，当地址不是扇区的首地址时，则默认不用擦除直接写入数据即可
***/
ErrorStatus SPIFlash_DMA_WriteData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	uint16_t SectionNum = 0;
	uint32_t Temp = 0;
	
	//先判断参数是否合法
	if(Len == 0 || Adr > SPIFLASH_VOLUME_SIZE){
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
				_SPIFlash_WritePageData(Adr,Buf,SPIFLASH_PAGE_SIZE);
				Adr += SPIFLASH_PAGE_SIZE;
				Buf += SPIFLASH_PAGE_SIZE;
				Len -= SPIFLASH_PAGE_SIZE;
			}else{								//剩余数据小于等于一页
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);					//等待上一次写入完成
				_SPIFlash_WritePageData(Adr,Buf,Len);
				Adr += Len;
				Buf += Len;
				Len = 0;
			}
		}else{											//写入地址没有对齐页首
			Temp = Adr % SPIFLASH_PAGE_SIZE;
			Temp = SPIFLASH_PAGE_SIZE - Temp;
			
			if(Len > Temp){
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);						//等待上一次写入完成
				_SPIFlash_WritePageData(Adr,Buf,Temp);
				Adr += Temp;
				Buf += Temp;
				Len -= Temp;
			}else{
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);						//等待上一次写入完成
				_SPIFlash_WritePageData(Adr,Buf,Len);
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
























