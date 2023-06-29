/********************************************************************
*EEPROM读写数据相关，EEPROM一般都用I2C来通讯，通讯速率为400khz，兼容
24Cxx系列所有EEPROM
*********************************************************************/
#include "eeprom.h"
#include "i2cHardPolling.h"
#include "systick.h"




/*内部函数区********************************************************/
/***
*判断EEPROM是否已经写入完毕
*参数：
	SlaveAdr：EEPROM物理地址
*返回值：返回SUCCESS时表明写入完毕，ERROR为出现其他不可修复的错误,EEPROM
在10ms内均无应答
***/
static ErrorStatus _IsEEPROMReady(u8 SlaveAdr)
{
	uint32_t I2CWaitCount = 0;
	
	while(I2CWaitCount < 1000){
		I2CWaitCount++;
		if(RequestEEPROMAck(SlaveAdr) == SUCCESS){
			return SUCCESS;
		}
	}
	
	return ERROR;
}




/*外部函数区*********************************************************/
/***
*24Cxx写入数据
*参数：
	EEPROMType：EEPROM类型
	SlaveAdr：EEPROM物理地址
	Reg：要开始写入数据的起始地址
	Buf：写入的数据缓冲区起始地址
	Len：一共要写入的数据长度
*返回值：HAL_OK为正常读取，其他为异常
***/
ErrorStatus EEPROMWriteData(EEPROM_Type_e EEPROMType,uint8_t SlaveAdr,uint16_t Reg,uint8_t* Buf,uint16_t Len)
{
	uint16_t WriteBaseAdr = 0;			//保存当前写入的数据的相对页首的基地址
	uint16_t WritePageAdr = 0;			//保存当前写入的页地址
	uint8_t TempLen = 0;
	ErrorStatus ReState = SUCCESS;
	
	uint8_t PageBytes = 0;				//每页字节数
	uint16_t TotalBytes = 0;			//总字节数
	uint16_t RegAdrTemp = 0;			//写入数据的寄存器地址
	
	
	//根据不同EEPROM类型计算所需参数值
	switch(EEPROMType){
		case EEPROM_24C02:
			SlaveAdr = (SlaveAdr & 0xFE);
			PageBytes = 8;
			TotalBytes = 256;
		
			//分离出页地址和基地址
			WriteBaseAdr = Reg & 0x0007;
			WritePageAdr = Reg & 0xFFF8;
		break;
		case EEPROM_24C04:
			SlaveAdr = (SlaveAdr & 0xFC);
			PageBytes = 16;
			TotalBytes = 512;
		
			//分离出页地址和基地址
			WriteBaseAdr = Reg & 0x000F;
			WritePageAdr = Reg & 0xFFF0;
		break;
		case EEPROM_24C08:
			SlaveAdr = (SlaveAdr & 0xF8);
			PageBytes = 16;
			TotalBytes = 1024;
		
			//分离出页地址和基地址
			WriteBaseAdr = Reg & 0x000F;
			WritePageAdr = Reg & 0xFFF0;
		break;
		case EEPROM_24C16:
			SlaveAdr = (SlaveAdr & 0xF0);
			PageBytes = 16;
			TotalBytes = 2048;
		
			//分离出页地址和基地址
			WriteBaseAdr = Reg & 0x000F;
			WritePageAdr = Reg & 0xFFF0;
		break;
		case EEPROM_24C32:
			SlaveAdr = (SlaveAdr & 0xFE);
			PageBytes = 32;
			TotalBytes = 4096;
		
			//分离出页地址和基地址
			WriteBaseAdr = Reg & 0x001F;
			WritePageAdr = Reg & 0xFFE0;
		break;
		case EEPROM_24C64:
			SlaveAdr = (SlaveAdr & 0xFE);
			PageBytes = 32;
			TotalBytes = 8192;
		
			//分离出页地址和基地址
			WriteBaseAdr = Reg & 0x001F;
			WritePageAdr = Reg & 0xFFE0;
		break;
		
		default :break;
	}
	
	//先判断参数是否合法
	if(Reg+Len > TotalBytes){
		return ERROR;
	}
	
	//开始写入数据
	while(Len){
		//计算此次写入的数据的长度
		if(Len > (PageBytes - WriteBaseAdr)){
			TempLen = PageBytes - WriteBaseAdr;
		}else{
			TempLen = Len;
		}
		
		RegAdrTemp = WritePageAdr|WriteBaseAdr;
		
		//根据不同EEPROM类型计算从机地址值
		switch(EEPROMType){
			case EEPROM_24C02:
				SlaveAdr = SlaveAdr;
			break;
			case EEPROM_24C04:
				SlaveAdr = ((SlaveAdr & 0xFC)|((RegAdrTemp&0x0100)>>7));
			break;
			case EEPROM_24C08:
				SlaveAdr = ((SlaveAdr & 0xF8)|((RegAdrTemp&0x0300)>>7));
			break;
			case EEPROM_24C16:
				SlaveAdr = ((SlaveAdr & 0xF0)|((RegAdrTemp&0x0700)>>7));
			break;
			case EEPROM_24C32:
				SlaveAdr = SlaveAdr;
			break;
			case EEPROM_24C64:
				SlaveAdr = SlaveAdr;
			break;
			default :break;
		}
		
		//判断EEPROM写入是否完成
		ReState = _IsEEPROMReady(SlaveAdr);
		if(ReState != SUCCESS){
			return ReState;
		}
		
		switch(EEPROMType){
			case EEPROM_24C32:
				ReState = I2CWriteData(SlaveAdr,RegAdrTemp&0x0FFF,2,Buf,TempLen);
			break;
			case EEPROM_24C64:
				ReState = I2CWriteData(SlaveAdr,RegAdrTemp&0x1FFF,2,Buf,TempLen);
			break;
			default :
				ReState = I2CWriteData(SlaveAdr,RegAdrTemp&0x00ff,1,Buf,TempLen);
			break;
		}
		
		//再给一次机会写入
		if(ReState != SUCCESS){
			I2C3->CR1 |= I2C_CR1_SWRST;
			I2C_Config();
			
			switch(EEPROMType){
				case EEPROM_24C32:
					return I2CWriteData(SlaveAdr,RegAdrTemp&0x0FFF,2,Buf,TempLen);
				case EEPROM_24C64:
					return I2CWriteData(SlaveAdr,RegAdrTemp&0x1FFF,2,Buf,TempLen);
				default :
					return I2CWriteData(SlaveAdr,RegAdrTemp&0x00ff,1,Buf,TempLen);
			}
		}
		
		//写入成功，更新各参数值
		WritePageAdr += PageBytes;
		WriteBaseAdr = 0;
		Buf += TempLen;
		Len -= TempLen;
	}
	
	//判断EEPROM写入是否完成
	ReState = _IsEEPROMReady(SlaveAdr);
	if(ReState != SUCCESS){
		return ReState;
	}
	
	return ReState;
}



/***
*24Cxx读取数据
*参数：
	EEPROMType：EEPROM类型
	SlaveAdr：EEPROM物理地址
	Reg：要开始读取数据的起始地址
	Buf：存放读取的数据缓冲区起始地址
	Len：一共要读取的数据长度
*返回值：HAL_OK为正常读取，其他为异常
***/
ErrorStatus EEPROMReadData(EEPROM_Type_e EEPROMType,uint8_t SlaveAdr,uint16_t Reg,uint8_t* Buf,uint16_t Len)
{
	uint16_t TotalBytes = 0;			//总字节数
	ErrorStatus ReturnStatus = SUCCESS;
	
	
	//根据不同EEPROM类型计算所需参数值
	switch(EEPROMType){
		case EEPROM_24C02:
			SlaveAdr = (SlaveAdr & 0xFE);
			TotalBytes = 256;
		break;
		case EEPROM_24C04:
			SlaveAdr = ((SlaveAdr & 0xFC)|((Reg&0x0100)>>7));
			TotalBytes = 512;
		break;
		case EEPROM_24C08:
			SlaveAdr = ((SlaveAdr & 0xF8)|((Reg&0x0300)>>7));
			TotalBytes = 1024;
		break;
		case EEPROM_24C16:
			SlaveAdr = ((SlaveAdr & 0xF0)|((Reg&0x0700)>>7));
			TotalBytes = 2048;
		break;
		case EEPROM_24C32:
			SlaveAdr = (SlaveAdr & 0xFE);
			TotalBytes = 4096;
		break;
		case EEPROM_24C64:
			SlaveAdr = (SlaveAdr & 0xFE);
			TotalBytes = 8192;
		break;
		default :break;
	}
	
	//先判断参数是否合法
	if(Reg+Len > TotalBytes){
		return ERROR;
	}
	
	
	switch(EEPROMType){
		case EEPROM_24C32:
			//这里需要增加一次机会，防止hal库函数的硬件I2C导致读取失败，尽量避免读取失败
			ReturnStatus = I2CReadData(SlaveAdr,Reg&0x0FFF,2,Buf,Len);
			if(ReturnStatus != SUCCESS){
				I2C3->CR1 |= I2C_CR1_SWRST;
				I2C_Config();
				
				return I2CReadData(SlaveAdr,Reg&0x0FFF,2,Buf,Len);
			}
			return ReturnStatus;
		case EEPROM_24C64:
			ReturnStatus = I2CReadData(SlaveAdr,Reg&0x1FFF,2,Buf,Len);
			if(ReturnStatus != SUCCESS){
				I2C3->CR1 |= I2C_CR1_SWRST;
				I2C_Config();
				
				return I2CReadData(SlaveAdr,Reg&0x1FFF,2,Buf,Len);
			}
			return ReturnStatus;
		default :
			ReturnStatus = I2CReadData(SlaveAdr,Reg&0x00ff,1,Buf,Len);
			if(ReturnStatus != SUCCESS){
				I2C3->CR1 |= I2C_CR1_SWRST;
				I2C_Config();
				
				return I2CReadData(SlaveAdr,Reg&0x00ff,1,Buf,Len);
			}
			return ReturnStatus;
	}
}





