/********************************************************************
*EEPROM读写数据相关，EEPROM一般都用I2C来通讯，通讯速率为400khz，兼容
24Cxx系列所有EEPROM

注意事项：
1.常用型号：
	1)24C02:容量为：2Kbit，可按字节写入，也可以按页写入（每页大小为8字节），写入数据的地址位数：8位，从机物理地址为(7位)：1010 A2 A1 A0，其中A2,A1,A0为对应的引脚状态
*********************************************************************/
#include "bsp_eeprom.h"
#include "i2c.h"
#include "msg_def.h"
#include "file_operate.h"
#include "Common.h"




/*宏定义区***********************************************************/
#define I2C_SR2_BUSY_FLAG			0x0002						//定义BUSY对应的判断标志位(SR2)
#define I2C_SR1_BERR_AF_FLAG		0x0500						//定义BERR和AF对应的判断标志位(SR1)
#define I2C_SR1_TXE_FLAG			0x0080						//定义TXE对应的判断标志位(SR1)
#define I2C_SR2_TRA_BUSY_MSL_FLAG	0x0007						//定义TRA,BUSY,MSL对应的判断标志位(SR2)
#define I2C_CR1_ACK_FLAG			0x0400						//定义ACK位
#define I2C_CR1_POS_FLAG			0x0800						//定义POS位
#define I2C_CR1_STOP_FLAG			0x0200						//定义STOP位
#define I2C_SR2_BUSY_MSL_FLAG		0x0003						//定义BUSY,MSL对应的判断标志位(SR2)
#define I2C_SR1_RXNE_BTF_FLAG		0x0044						//定义RXNE,BTF位对应的判断标志位(SR1)
#define I2C_SR1_RXNE_FLAG			0x0040						//定义RXNE位对应的判断标志位(SR1)

#define  I2C_EVENT_MASTER_MODE_SELECT                      ((uint32_t)0x00030001)
#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)


/*内部函数区********************************************************/
/***
*判断I2C总线是否被从机占用，总线出错，总线应答错误等情况，若是则解除
***/
static void _I2C_ClearErr(void)
{
	extern I2C_HandleTypeDef hi2c3;
	
	if((hi2c3.Instance->SR2 & I2C_FLAG_BUSY) || (hi2c3.Instance->SR1 & I2C_FLAG_AF)){
		GPIO_InitTypeDef GPIO_InitStruct;
		uint16_t i,j;
		
		//必须先将I2C的引脚配置成通用开漏模式，才能手动控制SCL的状态
		GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
		
		//产生一个STOP条件，清除AF（应答错误）产生的错误
		GPIOH->BSRR = GPIO_PIN_7<<16;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_8<<16;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_7;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_8;
		for(j=0;j<50000;j++);
		
		//循环让SCL输出脉冲，让从机释放I2C总线的控制权，清除BUSY状态
		for(i=0;i<9;i++){
			GPIOH->BSRR = GPIO_PIN_7<<16;
			for(j=0;j<50000;j++);
			GPIOH->BSRR = GPIO_PIN_7;
			for(j=0;j<50000;j++);
		}
		
		//先重置I2C，再重新配置I2C引脚模式和I2C模式，即可解除总线异常状态标志
		hi2c3.Instance->CR1 |= I2C_CR1_SWRST;
		hi2c3.State = HAL_I2C_STATE_RESET;
		MX_I2C3_Init();
	}
}



/***
*专门给EEPROM用的，测试是否已经将数据写入完毕的函数
*参数：
	SlaveAdr：EEPROM物理地址
*返回值：SUCCESS表示EEPROM有响应，ERROR表示无响应
***/
HAL_StatusTypeDef RequestEEPROMAck(I2C_HandleTypeDef* hi2c,uint8_t SlaveAdr)
{
	uint32_t Count = 0;					//防止超时卡死现象的计数值
	uint32_t flag1 = 0, flag2 = 0;
	uint32_t SRValue = 0;
	
	//避免两次写入之间间隔太短，导致从机尚未完全释放总线，从而要进入_I2C_ClearErr函数来释放
	for(Count=0;Count<10000;Count++){
		if(!(hi2c->Instance->SR2 & I2C_SR2_BUSY_FLAG)){
			break;
		}
	}
	Count = 0;
	
	//判断总线是否异常，若是，则解除总线异常
	_I2C_ClearErr();
	
	//产生起始条件，当起始信号没有响应，START位没有被硬件清除时，需要复位下I2C
	hi2c->Instance->CR1 |= I2C_CR1_START;
	Count = 0;
    do{
		flag1 = hi2c->Instance->SR1;
		flag2 = hi2c->Instance->SR2;
		flag2 = flag2 << 16;
		SRValue = (flag1 | flag2) & 0x00FFFFFF;
		
		if((SRValue & I2C_EVENT_MASTER_MODE_SELECT) == I2C_EVENT_MASTER_MODE_SELECT){
			break;
		}
		
		Count++;
		if(Count>=10000){
			hi2c->Instance->CR1 |= I2C_CR1_SWRST;
			hi2c->State = HAL_I2C_STATE_RESET;
			MX_I2C3_Init();
			return HAL_TIMEOUT;
		}
	}while(1);
	
	//发送从机地址，等待响应，若超时没有响应，则表明EEPROM还没有写入完毕，需要生成停止信号，并清除AF标志位
	hi2c->Instance->DR = ((uint8_t)((SlaveAdr) & (uint8_t)(~I2C_OAR1_ADD0)));
	Count = 0;
	do{
		flag1 = hi2c->Instance->SR1;
		flag2 = hi2c->Instance->SR2;
		flag2 = flag2 << 16;
		SRValue = (flag1 | flag2) & 0x00FFFFFF;
		
		if((SRValue & I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED){
			hi2c->Instance->CR1 |= I2C_CR1_STOP;
			__HAL_I2C_CLEAR_ADDRFLAG(hi2c);						//这里需要清除ADDR标志位
			return HAL_OK;
		}
		
		Count++;
		if(Count>=1000){
			hi2c->Instance->CR1 |= I2C_CR1_STOP;
			__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);			//这里需要清除掉AF标志
			return HAL_TIMEOUT;
		}
	}while(1);
}




/***
*判断EEPROM是否已经写入完毕
*参数：
	SlaveAdr：EEPROM物理地址
*返回值：返回SUCCESS时表明写入完毕，ERROR为出现其他不可修复的错误,EEPROM
在10ms内均无应答
***/
static HAL_StatusTypeDef _IsEEPROMReady(I2C_HandleTypeDef* hi2c,uint8_t SlaveAdr)
{
	uint32_t I2CWaitCount = 0;
	
	while(I2CWaitCount < 1000){
		I2CWaitCount++;
		if(RequestEEPROMAck(hi2c,SlaveAdr) == SUCCESS){
			return HAL_OK;
		}
	}
	
	return HAL_TIMEOUT;
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
HAL_StatusTypeDef EEPROMWriteData(EEPROM_Type_e EEPROMType,uint8_t SlaveAdr,uint16_t Reg,uint8_t* Buf,uint16_t Len)
{
	extern I2C_HandleTypeDef hi2c3;
	uint16_t WriteBaseAdr = 0;			//保存当前写入的数据的相对页首的基地址
	uint16_t WritePageAdr = 0;			//保存当前写入的页地址
	uint8_t TempLen = 0;
	HAL_StatusTypeDef ReState = HAL_OK;
	uint32_t Count = 0;	
	
	uint8_t PageBytes = 0;				//每页字节数
	uint16_t TotalBytes = 0;			//总字节数
	uint16_t RegAdrTemp = 0;			//写入数据的寄存器地址
	
	osKernelLock();
	
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
		osKernelUnlock();
		return HAL_ERROR;
	}
	
	//避免两次读取之间间隔太短，导致从机尚未释放总线完全，从而要进入_I2C_ClearErr函数来释放
	for(Count=0;Count<10000;Count++){
		if((hi2c3.Instance->SR2 & I2C_FLAG_BUSY) == RESET){
			break;
		}
	}
	Count = 0;
	
	//判断I2C总线是否处于BUSY，是，则先通过手动方式产生9个时钟周期让从机释放总线
	_I2C_ClearErr();
	
	
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
		ReState = _IsEEPROMReady(&hi2c3,SlaveAdr);
		if(ReState != HAL_OK){
			osKernelUnlock();
			return ReState;
		}
		
		switch(EEPROMType){
			case EEPROM_24C32:
				ReState = HAL_I2C_Mem_Write(&hi2c3,SlaveAdr,RegAdrTemp&0x0FFF,2,Buf,TempLen,50);
			break;
			case EEPROM_24C64:
				ReState = HAL_I2C_Mem_Write(&hi2c3,SlaveAdr,RegAdrTemp&0x1FFF,2,Buf,TempLen,50);
			break;
			default :
				ReState = HAL_I2C_Mem_Write(&hi2c3,SlaveAdr,RegAdrTemp&0x00ff,1,Buf,TempLen,50);
			break;
		}
		
		//再给一次机会写入
		if(ReState != HAL_OK){
			hi2c3.Instance->CR1 |= I2C_CR1_SWRST;
			hi2c3.State = HAL_I2C_STATE_RESET;
			MX_I2C3_Init();
			
			switch(EEPROMType){
				case EEPROM_24C32:
					ReState = HAL_I2C_Mem_Write(&hi2c3,SlaveAdr,RegAdrTemp&0x0FFF,2,Buf,TempLen,50);
					break;
				case EEPROM_24C64:
					ReState = HAL_I2C_Mem_Write(&hi2c3,SlaveAdr,RegAdrTemp&0x1FFF,2,Buf,TempLen,50);
					break;
				default :
					ReState = HAL_I2C_Mem_Write(&hi2c3,SlaveAdr,RegAdrTemp&0x00ff,1,Buf,TempLen,50);
					break;
			}
			
			osKernelUnlock();
			return ReState;
		}
		
		//写入成功，更新各参数值
		WritePageAdr += PageBytes;
		WriteBaseAdr = 0;
		Buf += TempLen;
		Len -= TempLen;
	}
	
	//判断EEPROM写入是否完成
	ReState = _IsEEPROMReady(&hi2c3,SlaveAdr);
	
	osKernelUnlock();
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
HAL_StatusTypeDef EEPROMReadData(EEPROM_Type_e EEPROMType,uint8_t SlaveAdr,uint16_t Reg,uint8_t* Buf,uint16_t Len)
{
	extern I2C_HandleTypeDef hi2c3;
	uint32_t Count = 0;	
	uint16_t TotalBytes = 0;			//总字节数
	HAL_StatusTypeDef ReturnStatus = HAL_OK;
	
	osKernelLock();
	
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
		osKernelUnlock();
		return HAL_ERROR;
	}
	
	
	//避免两次读取之间间隔太短，导致从机尚未释放总线完全，从而要进入_I2C_ClearErr函数来释放
	for(Count=0;Count<10000;Count++){
		if((hi2c3.Instance->SR2 & I2C_FLAG_BUSY) == RESET){
			break;
		}
	}
	Count = 0;
	
	//判断I2C总线是否处于BUSY，是，则先通过手动方式产生9个时钟周期让从机释放总线
	_I2C_ClearErr();
	
	switch(EEPROMType){
		case EEPROM_24C32:
			//这里需要增加一次机会，防止hal库函数的硬件I2C导致读取失败，尽量避免读取失败
			ReturnStatus = HAL_I2C_Mem_Read(&hi2c3,SlaveAdr,Reg&0x0FFF,2,Buf,Len,500);
			if(ReturnStatus != HAL_OK){
				hi2c3.Instance->CR1 |= I2C_CR1_SWRST;
				hi2c3.State = HAL_I2C_STATE_RESET;
				MX_I2C3_Init();
				
				ReturnStatus =  HAL_I2C_Mem_Read(&hi2c3,SlaveAdr,Reg&0x0FFF,2,Buf,Len,500);
			}
		break;
		case EEPROM_24C64:
			ReturnStatus = HAL_I2C_Mem_Read(&hi2c3,SlaveAdr,Reg&0x1FFF,2,Buf,Len,500);
			if(ReturnStatus != HAL_OK){
				hi2c3.Instance->CR1 |= I2C_CR1_SWRST;
				hi2c3.State = HAL_I2C_STATE_RESET;
				MX_I2C3_Init();
				
				ReturnStatus = HAL_I2C_Mem_Read(&hi2c3,SlaveAdr,Reg&0x1FFF,2,Buf,Len,500);
				
			}
		break;
		default :
			ReturnStatus = HAL_I2C_Mem_Read(&hi2c3,SlaveAdr,Reg&0x00ff,1,Buf,Len,500);
			if(ReturnStatus != HAL_OK){
				hi2c3.Instance->CR1 |= I2C_CR1_SWRST;
				hi2c3.State = HAL_I2C_STATE_RESET;
				MX_I2C3_Init();
				
				ReturnStatus = HAL_I2C_Mem_Read(&hi2c3,SlaveAdr,Reg&0x00ff,1,Buf,Len,500);
			}
		break;
	}
	
	osKernelUnlock();
	return ReturnStatus;
}




/******************************************************************************/
/*
*   读取数据管理信息参数
*/
HAL_StatusTypeDef Get_DataMange_Info(__IO DataManage_t *ptDataManage)
{
     return EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32, EPPROM_ADDR_DATA_MANAGE, (uint8_t*)ptDataManage, sizeof(DataManage_t));
}



/*
*   更新数据管理信息参数
*/
HAL_StatusTypeDef Save_DataMange_Info(__IO DataManage_t *ptDataManage)
{
    uint8_t i = 0;

    for(i = 0;  i < 3; i++)
    {
        if(HAL_OK == EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32, EPPROM_ADDR_DATA_MANAGE, (uint8_t*)ptDataManage, sizeof(DataManage_t)))
        {
            return HAL_OK;
        }
    }
    
    return HAL_ERROR;
}


//===仪器参数相关==================================================================
/***
*初始化仪器所有参数，注意这里没有包含触摸参数初始化
***/
void Machine_InitPara(void)
{
	EEPROM_Init_MachInfo_flag();
	EEPROM_Init_MachInfo_companyInfo();
	EEPROM_Init_MachInfo_catMastSynServEvent();
	EEPROM_Init_MachInfo_paramSet();
	EEPROM_Init_MachInfo_calibration();
	EEPROM_Init_MachInfo_systemSet();
	EEPROM_Init_MachInfo_accountMM();
	EEPROM_Init_MachInfo_printSet();
	EEPROM_Init_MachInfo_labInfo();
	EEPROM_Init_MachInfo_qcInfo();
	EEPROM_Init_MachInfo_other();
}





/***
*初始化MachInfo.touch
***/
void EEPROM_Init_MachInfo_touch(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.touch.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.touch.size = sizeof(MachInfo.touch);
	
	MachInfo.touch.flag = TOUCH_INIT_FLAG;
		
	MachInfo.touch.pixelX[0] = 40;
	MachInfo.touch.pixelX[1] = 760;
	MachInfo.touch.pixelX[2] = 760;
	MachInfo.touch.pixelX[3] = 40;
	MachInfo.touch.pixelX[4] = 400;
	
	MachInfo.touch.pixelY[0] = 24;
	MachInfo.touch.pixelY[1] = 24;
	MachInfo.touch.pixelY[2] = 456;
	MachInfo.touch.pixelY[3] = 456;
	MachInfo.touch.pixelY[4] = 240;
	
	MachInfo.touch.samX[0] = 293;
	MachInfo.touch.samX[1] = 3802;
	MachInfo.touch.samX[2] = 3824;
	MachInfo.touch.samX[3] = 296;
	MachInfo.touch.samX[4] = 2038;
	
	MachInfo.touch.samY[0] = 488;
	MachInfo.touch.samY[1] = 460;
	MachInfo.touch.samY[2] = 3791;
	MachInfo.touch.samY[3] = 3807;
	MachInfo.touch.samY[4] = 2143;
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_MACHINFO_INIT_TOUCH,(uint8_t*)&MachInfo.touch,sizeof(MachInfo.touch));
	
	printf("MachInfo.touch size:%u\r\n",sizeof(MachInfo.touch));
}



/***
*查找MachInfo.touch中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_touch(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.touch.size = sizeof(MachInfo.touch);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_MACHINFO_INIT_TOUCH,(uint8_t*)&MachInfo.touch,sizeof(MachInfo.touch));
}




/***
*读取MachInfo.touch，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_touch(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_MACHINFO_INIT_TOUCH,(uint8_t*)&MachInfo.touch,sizeof(MachInfo.touch));
	
	//判断起始标志是否异常
	if(MachInfo.touch.startFlag != STRUCT_START_INIT_FLAG && MachInfo.touch.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_MACHINFO_INIT_TOUCH,(uint8_t*)&MachInfo.touch,sizeof(MachInfo.touch));
		
		if(MachInfo.touch.startFlag != STRUCT_START_INIT_FLAG && MachInfo.touch.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.touch.startFlag == STRUCT_START_INIT_FLAG && MachInfo.touch.size == sizeof(MachInfo.touch)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.touch.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_touch();
	}else if(MachInfo.touch.size != sizeof(MachInfo.touch)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_touch();
	}
}



//===
/***
*初始化MachInfo.flag
***/
void EEPROM_Init_MachInfo_flag(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.flag.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.flag.size = sizeof(MachInfo.flag);
	
	*((uint64_t*)(&MachInfo.flag.bit)) = 0;
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_FLAG,(uint8_t*)&MachInfo.flag,sizeof(MachInfo.flag));
	
	printf("MachInfo.flag size:%u\r\n",sizeof(MachInfo.flag));
}



/***
*查找MachInfo.flag中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_flag(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.flag.size = sizeof(MachInfo.flag);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_FLAG,(uint8_t*)&MachInfo.flag,sizeof(MachInfo.flag));
}




/***
*读取MachInfo.flag，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_flag(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_FLAG,(uint8_t*)&MachInfo.flag,sizeof(MachInfo.flag));
	
	//判断起始标志是否异常
	if(MachInfo.flag.startFlag != STRUCT_START_INIT_FLAG && MachInfo.flag.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_FLAG,(uint8_t*)&MachInfo.flag,sizeof(MachInfo.flag));
		
		if(MachInfo.flag.startFlag != STRUCT_START_INIT_FLAG && MachInfo.flag.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.flag.startFlag == STRUCT_START_INIT_FLAG && MachInfo.flag.size == sizeof(MachInfo.flag)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.flag.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_flag();
	}else if(MachInfo.flag.size != sizeof(MachInfo.flag)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_flag();
	}
}



//===
/***
*初始化MachInfo.companyInfo
***/
void EEPROM_Init_MachInfo_companyInfo(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.companyInfo.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.companyInfo.size = sizeof(MachInfo.companyInfo);
	
	Company_ConfigInfo(COMPANY_CREATE_CARE);
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_COMPANY,(uint8_t*)&MachInfo.companyInfo,sizeof(MachInfo.companyInfo));
	
	printf("MachInfo.companyInfo size:%u\r\n",sizeof(MachInfo.companyInfo));
}



/***
*查找MachInfo.companyInfo中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_companyInfo(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.companyInfo.size = sizeof(MachInfo.companyInfo);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_COMPANY,(uint8_t*)&MachInfo.companyInfo,sizeof(MachInfo.companyInfo));
}




/***
*读取MachInfo.companyInfo，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_companyInfo(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_COMPANY,(uint8_t*)&MachInfo.companyInfo,sizeof(MachInfo.companyInfo));
	
	//判断起始标志是否异常
	if(MachInfo.companyInfo.startFlag != STRUCT_START_INIT_FLAG && MachInfo.companyInfo.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_COMPANY,(uint8_t*)&MachInfo.companyInfo,sizeof(MachInfo.companyInfo));
		
		if(MachInfo.companyInfo.startFlag != STRUCT_START_INIT_FLAG && MachInfo.companyInfo.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.companyInfo.startFlag == STRUCT_START_INIT_FLAG && MachInfo.companyInfo.size == sizeof(MachInfo.companyInfo)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.companyInfo.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_companyInfo();
	}else if(MachInfo.companyInfo.size != sizeof(MachInfo.companyInfo)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_companyInfo();
	}
}



//===
/***
*初始化MachInfo.catMastSynServEvent
***/
void EEPROM_Init_MachInfo_catMastSynServEvent(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.catMastSynServEvent.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.catMastSynServEvent.size = sizeof(MachInfo.catMastSynServEvent);
	
	*((uint64_t*)(&MachInfo.catMastSynServEvent.bit)) = 0;
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
	
	printf("MachInfo.catMastSynServEvent size:%u\r\n",sizeof(MachInfo.catMastSynServEvent));
}



/***
*查找MachInfo.catMastSynServEvent中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_catMastSynServEvent(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.catMastSynServEvent.size = sizeof(MachInfo.catMastSynServEvent);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
}




/***
*读取MachInfo.catMastSynServEvent，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_catMastSynServEvent(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
	
	//判断起始标志是否异常
	if(MachInfo.catMastSynServEvent.startFlag != STRUCT_START_INIT_FLAG && MachInfo.catMastSynServEvent.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
		
		if(MachInfo.catMastSynServEvent.startFlag != STRUCT_START_INIT_FLAG && MachInfo.catMastSynServEvent.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.catMastSynServEvent.startFlag == STRUCT_START_INIT_FLAG && MachInfo.catMastSynServEvent.size == sizeof(MachInfo.catMastSynServEvent)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.catMastSynServEvent.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_catMastSynServEvent();
	}else if(MachInfo.catMastSynServEvent.size != sizeof(MachInfo.catMastSynServEvent)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_catMastSynServEvent();
	}
}



//===
/***
*初始化MachInfo.paramSet
***/
void EEPROM_Init_MachInfo_paramSet(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.paramSet.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.paramSet.size = sizeof(MachInfo.paramSet);
	
	MachInfo.paramSet.ucWBC_Gain = 135;		                    //WBC增益
	MachInfo.paramSet.ucHGB_Gain = 150;		    				//HGB增益
	
    MachInfo.paramSet.current535 = 900;		        			//535灯电流
	MachInfo.paramSet.current415 = 900;		        			//415灯电流
	
    MachInfo.paramSet.KQADC415 = 3026;     						//空气校准415ADC
	MachInfo.paramSet.KQADC535 = 3006;                         	//空气校准535ADC
    MachInfo.paramSet.KBADC415 = 3500;                         	//空白校准415ADC
    MachInfo.paramSet.KBADC535 = 3500;                         	//空白校准535ADC
	
	MachInfo.paramSet.point1Curr415 = 900;     					//415波长线性点1电流值
	MachInfo.paramSet.point1Curr535 = 900;                      //535波长线性点1电流值
    MachInfo.paramSet.point2Curr415 = 1100;                     //415波长线性点2电流值
    MachInfo.paramSet.point2Curr535 = 1100;                     //535波长线性点2电流值
    
    MachInfo.paramSet.ucPump_Scale = 100;						//泵占空比  
    MachInfo.paramSet.usNsec_BeepWaring = 30;       			//样本分析后, N=30秒后进出仓未s进仓，则蜂鸣器报警
    //						
    MachInfo.paramSet.ulOutIn_In_AddStep = 500;					//进出电机，往里走，光耦补充 0
	MachInfo.paramSet.ulOutIn_Out_AddStep = 1500;	    		//进出电机，往外走，光耦补充 1400
	MachInfo.paramSet.ulClamp_In_AddStep = 1500;				//夹子电机，往里走，光耦补充 1400
    MachInfo.paramSet.ulClamp_Out_AddStep = 650;      			//夹子电机，往外走，光耦补充  1600
	MachInfo.paramSet.fPress_Adjust = 0;						//气压校准值
	MachInfo.paramSet.pressSensor = 1;							//压力传感器
	
	//配置数字压力传感器，HONEYWELL，IT110传感器的压力补偿值
    MachInfo.paramSet.digSensor[0] = -62;
	MachInfo.paramSet.digSensor[1] = -319;
	MachInfo.paramSet.digSensor[2] = -503;
	MachInfo.paramSet.digSensor[3] = -477;
	MachInfo.paramSet.digSensor[4] = -589;
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET,(uint8_t*)&MachInfo.paramSet,sizeof(MachInfo.paramSet));
	
	printf("MachInfo.paramSet size:%u\r\n",sizeof(MachInfo.paramSet));
}



/***
*查找MachInfo.paramSet中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_paramSet(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.paramSet.size = sizeof(MachInfo.paramSet);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET,(uint8_t*)&MachInfo.paramSet,sizeof(MachInfo.paramSet));
}




/***
*读取MachInfo.paramSet，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_paramSet(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET,(uint8_t*)&MachInfo.paramSet,sizeof(MachInfo.paramSet));
	
	//判断起始标志是否异常
	if(MachInfo.paramSet.startFlag != STRUCT_START_INIT_FLAG && MachInfo.paramSet.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET,(uint8_t*)&MachInfo.paramSet,sizeof(MachInfo.paramSet));
		
		if(MachInfo.paramSet.startFlag != STRUCT_START_INIT_FLAG && MachInfo.paramSet.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.paramSet.startFlag == STRUCT_START_INIT_FLAG && MachInfo.paramSet.size == sizeof(MachInfo.paramSet)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.paramSet.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_paramSet();
	}else if(MachInfo.paramSet.size != sizeof(MachInfo.paramSet)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_paramSet();
	}
}



//===
/***
*初始化MachInfo.calibration
***/
void EEPROM_Init_MachInfo_calibration(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.calibration.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.calibration.size = sizeof(MachInfo.calibration);
	
	memset(MachInfo.calibration.ucaWBC_CalibrationDate,0,sizeof(MachInfo.calibration.ucaWBC_CalibrationDate));
	memset(MachInfo.calibration.ucaHGB_CalibrationDate,0,sizeof(MachInfo.calibration.ucaHGB_CalibrationDate));
	
	MachInfo.calibration.fWBC 	= 1.00;		//WBC校准系数
    MachInfo.calibration.fHGB 	= 1.00;		//HGB校准系数
    strcpy((char*)MachInfo.calibration.ucaWBC_CalibrationDate, "2021/01/01");
    strcpy((char*)MachInfo.calibration.ucaHGB_CalibrationDate, "2021/01/01");
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CALIBRATION,(uint8_t*)&MachInfo.calibration,sizeof(MachInfo.calibration));
	
	printf("MachInfo.calibration size:%u\r\n",sizeof(MachInfo.calibration));
}



/***
*查找MachInfo.calibration中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_calibration(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.calibration.size = sizeof(MachInfo.calibration);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CALIBRATION,(uint8_t*)&MachInfo.calibration,sizeof(MachInfo.calibration));
}




/***
*读取MachInfo.calibration，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_calibration(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CALIBRATION,(uint8_t*)&MachInfo.calibration,sizeof(MachInfo.calibration));
	
	//判断起始标志是否异常
	if(MachInfo.calibration.startFlag != STRUCT_START_INIT_FLAG && MachInfo.calibration.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CALIBRATION,(uint8_t*)&MachInfo.calibration,sizeof(MachInfo.calibration));
		
		if(MachInfo.calibration.startFlag != STRUCT_START_INIT_FLAG && MachInfo.calibration.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.calibration.startFlag == STRUCT_START_INIT_FLAG && MachInfo.calibration.size == sizeof(MachInfo.calibration)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.calibration.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_calibration();
	}else if(MachInfo.calibration.size != sizeof(MachInfo.calibration)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_calibration();
	}
}




//===
/***
*初始化MachInfo.systemSet
***/
void EEPROM_Init_MachInfo_systemSet(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.systemSet.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.systemSet.size = sizeof(MachInfo.systemSet);
	
	memset(MachInfo.systemSet.ucaDate,0,sizeof(MachInfo.systemSet.ucaDate));
	memset(MachInfo.systemSet.ucaTime,0,sizeof(MachInfo.systemSet.ucaTime));
	
	strcpy((void*)MachInfo.systemSet.ucaDate, "2021/01/01");		//日期
	strcpy((void*)MachInfo.systemSet.ucaTime, "00:00:00");			//时间
	MachInfo.systemSet.eLanguage = LANGUAGE_CHINESE;
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_SYS_SET,(uint8_t*)&MachInfo.systemSet,sizeof(MachInfo.systemSet));
	
	printf("MachInfo.systemSet size:%u\r\n",sizeof(MachInfo.systemSet));
}



/***
*查找MachInfo.systemSet中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_systemSet(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.systemSet.size = sizeof(MachInfo.systemSet);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_SYS_SET,(uint8_t*)&MachInfo.systemSet,sizeof(MachInfo.systemSet));
}




/***
*读取MachInfo.systemSet，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_systemSet(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_SYS_SET,(uint8_t*)&MachInfo.systemSet,sizeof(MachInfo.systemSet));
	
	//判断起始标志是否异常
	if(MachInfo.systemSet.startFlag != STRUCT_START_INIT_FLAG && MachInfo.systemSet.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_SYS_SET,(uint8_t*)&MachInfo.systemSet,sizeof(MachInfo.systemSet));
		
		if(MachInfo.systemSet.startFlag != STRUCT_START_INIT_FLAG && MachInfo.systemSet.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.systemSet.startFlag == STRUCT_START_INIT_FLAG && MachInfo.systemSet.size == sizeof(MachInfo.systemSet)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.systemSet.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_systemSet();
	}else if(MachInfo.systemSet.size != sizeof(MachInfo.systemSet)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_systemSet();
	}
}



//===
/***
*初始化MachInfo.accountMM
***/
void EEPROM_Init_MachInfo_accountMM(void)
{
	extern MachInfo_s MachInfo;
	
	memset((void*)&MachInfo.accountMM, 0, sizeof(MachInfo.accountMM));
	
	MachInfo.accountMM.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.accountMM.size = sizeof(MachInfo.accountMM);
	
	//超级管理员
	strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].user,"YongFu");
	strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].psw,ACCOUNT_SUPER_DEFAULT_PSW);
	MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].authority = ACCOUNT_SUPER_AUTHORITY;
	MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].type = ACCOUNT_TYPE_SUPER;
	MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].dataType = ACCOUNT_DATA_TYPE_INNER;
	MachInfo.accountMM.endIndex++;
	
	//管理员
	strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].user,"admin");
	strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,ACCOUNT_ADMIN_DEFAULT_PSW);
	MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].authority = ACCOUNT_ADMIN_AUTHORITY;
	MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].type = ACCOUNT_TYPE_ADMIN;
	MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].dataType = ACCOUNT_DATA_TYPE_OUTSIDE;
	MachInfo.accountMM.endIndex++;
	
	//普通账户
	strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_NORMAL].user,"001");
	strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_NORMAL].psw,ACCOUNT_NORMAL_DEFAULT_PSW);
	MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_NORMAL].authority = ACCOUNT_NORMAL_AUTHORITY;
	MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_NORMAL].type = ACCOUNT_TYPE_NORMAL;
	MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_NORMAL].dataType = ACCOUNT_DATA_TYPE_OUTSIDE;
	MachInfo.accountMM.endIndex++;
	
	//设置默认账户
	MachInfo.accountMM.defaultIndex = ACCOUNT_TYPE_NORMAL;
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_ACCOUNT_MM,(uint8_t*)&MachInfo.accountMM,sizeof(MachInfo.accountMM));
	
	printf("MachInfo.accountMM size:%u\r\n",sizeof(MachInfo.accountMM));
}



/***
*查找MachInfo.accountMM中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_accountMM(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.accountMM.size = sizeof(MachInfo.accountMM);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_ACCOUNT_MM,(uint8_t*)&MachInfo.accountMM,sizeof(MachInfo.accountMM));
}




/***
*读取MachInfo.accountMM，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_accountMM(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_ACCOUNT_MM,(uint8_t*)&MachInfo.accountMM,sizeof(MachInfo.accountMM));
	
	//判断起始标志是否异常
	if(MachInfo.accountMM.startFlag != STRUCT_START_INIT_FLAG && MachInfo.accountMM.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_ACCOUNT_MM,(uint8_t*)&MachInfo.accountMM,sizeof(MachInfo.accountMM));
		
		if(MachInfo.accountMM.startFlag != STRUCT_START_INIT_FLAG && MachInfo.accountMM.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.accountMM.startFlag == STRUCT_START_INIT_FLAG && MachInfo.accountMM.size == sizeof(MachInfo.accountMM)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.accountMM.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_accountMM();
	}else if(MachInfo.accountMM.size != sizeof(MachInfo.accountMM)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_accountMM();
	}
}


//===
/***
*初始化MachInfo.printSet
***/
void EEPROM_Init_MachInfo_printSet(void)
{
	extern MachInfo_s MachInfo;
	
	memset((void*)&MachInfo.printSet, 0, sizeof(MachInfo.printSet));
	
	MachInfo.printSet.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.printSet.size = sizeof(MachInfo.printSet);
	
	strcpy((char*)MachInfo.printSet.ucaPrintTitle, "血细胞检验报告单");			//打印标题
	MachInfo.printSet.ePrintWay = PRINTF_WAY_AUTO;
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PRINT_SET,(uint8_t*)&MachInfo.printSet,sizeof(MachInfo.printSet));
	
	printf("MachInfo.printSet size:%u\r\n",sizeof(MachInfo.printSet));
}



/***
*查找MachInfo.printSet中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_printSet(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.printSet.size = sizeof(MachInfo.printSet);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PRINT_SET,(uint8_t*)&MachInfo.printSet,sizeof(MachInfo.printSet));
}




/***
*读取MachInfo.printSet，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_printSet(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PRINT_SET,(uint8_t*)&MachInfo.printSet,sizeof(MachInfo.printSet));
	
	//判断起始标志是否异常
	if(MachInfo.printSet.startFlag != STRUCT_START_INIT_FLAG && MachInfo.printSet.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PRINT_SET,(uint8_t*)&MachInfo.printSet,sizeof(MachInfo.printSet));
		
		if(MachInfo.printSet.startFlag != STRUCT_START_INIT_FLAG && MachInfo.printSet.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.printSet.startFlag == STRUCT_START_INIT_FLAG && MachInfo.printSet.size == sizeof(MachInfo.printSet)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.printSet.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_printSet();
	}else if(MachInfo.printSet.size != sizeof(MachInfo.printSet)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_printSet();
	}
}



//===
/***
*初始化MachInfo.labInfo
***/
void EEPROM_Init_MachInfo_labInfo(void)
{
	extern MachInfo_s MachInfo;
	
	memset((void*)&MachInfo.labInfo, 0, sizeof(MachInfo.labInfo));
	
	MachInfo.labInfo.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.labInfo.size = sizeof(MachInfo.labInfo);
	
	strcpy((char*)MachInfo.labInfo.ucaMachineName, 	"半自动血细胞分析仪");	//机器名称
	strcpy((char*)MachInfo.labInfo.ucaMachineType, 	"CX-2200");				//机器类型
	strcpy((char*)MachInfo.labInfo.ucaMachineSN, 	"");					//机器系列号
	strcpy((char*)MachInfo.labInfo.ucaClinicName, 	"");					//诊所名称
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_LAB_INFO,(uint8_t*)&MachInfo.labInfo,sizeof(MachInfo.labInfo));
	
	printf("MachInfo.labInfo size:%u\r\n",sizeof(MachInfo.labInfo));
}



/***
*查找MachInfo.labInfo中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_labInfo(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.labInfo.size = sizeof(MachInfo.labInfo);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_LAB_INFO,(uint8_t*)&MachInfo.labInfo,sizeof(MachInfo.labInfo));
}




/***
*读取MachInfo.labInfo，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_labInfo(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_LAB_INFO,(uint8_t*)&MachInfo.labInfo,sizeof(MachInfo.labInfo));
	
	//判断起始标志是否异常
	if(MachInfo.labInfo.startFlag != STRUCT_START_INIT_FLAG && MachInfo.labInfo.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_LAB_INFO,(uint8_t*)&MachInfo.labInfo,sizeof(MachInfo.labInfo));
		
		if(MachInfo.labInfo.startFlag != STRUCT_START_INIT_FLAG && MachInfo.labInfo.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.labInfo.startFlag == STRUCT_START_INIT_FLAG && MachInfo.labInfo.size == sizeof(MachInfo.labInfo)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.labInfo.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_labInfo();
	}else if(MachInfo.labInfo.size != sizeof(MachInfo.labInfo)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_labInfo();
	}
}



//===
/***
*初始化MachInfo.qcInfo
***/
void EEPROM_Init_MachInfo_qcInfo(void)
{
	extern MachInfo_s MachInfo;
	extern MachRunPara_s MachRunPara;
	
	MachInfo.qcInfo.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.qcInfo.size = sizeof(MachInfo.qcInfo);
	
	//质控、设定信息
    QC_FILENUM_INDEX_e eQC_Index = QC_FILENUM_INDEX_0;
    for(; eQC_Index < QC_FILENUM_INDEX_END; eQC_Index++)
    {
       memset((void*)&MachInfo.qcInfo.set[eQC_Index], 0, sizeof(MachInfo.qcInfo.set));
        //
        MachInfo.qcInfo.set[eQC_Index].eFileNumIndex     = QC_FILENUM_INDEX_END;
        MachInfo.qcInfo.set[eQC_Index].usTestCapacity    = QC_PER_FILE_NUM_MAX_CAPACITY;
    }
    //
    MachRunPara.eQC_Analysis_FileNum_Index = QC_FILENUM_INDEX_END;
    MachRunPara.eQC_List_FileNum_Index     = QC_FILENUM_INDEX_END;
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_QC_INFO,(uint8_t*)&MachInfo.qcInfo,sizeof(MachInfo.qcInfo));
	
	printf("MachInfo.qcInfo size:%u\r\n",sizeof(MachInfo.qcInfo));
}



/***
*查找MachInfo.qcInfo中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_qcInfo(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.qcInfo.size = sizeof(MachInfo.qcInfo);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_QC_INFO,(uint8_t*)&MachInfo.qcInfo,sizeof(MachInfo.qcInfo));
}




/***
*读取MachInfo.qcInfo，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_qcInfo(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_QC_INFO,(uint8_t*)&MachInfo.qcInfo,sizeof(MachInfo.qcInfo));
	
	//判断起始标志是否异常
	if(MachInfo.qcInfo.startFlag != STRUCT_START_INIT_FLAG && MachInfo.qcInfo.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_QC_INFO,(uint8_t*)&MachInfo.qcInfo,sizeof(MachInfo.qcInfo));
		
		if(MachInfo.qcInfo.startFlag != STRUCT_START_INIT_FLAG && MachInfo.qcInfo.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.qcInfo.startFlag == STRUCT_START_INIT_FLAG && MachInfo.qcInfo.size == sizeof(MachInfo.qcInfo)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.qcInfo.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_qcInfo();
	}else if(MachInfo.qcInfo.size != sizeof(MachInfo.qcInfo)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_qcInfo();
	}
}


//===
/***
*初始化MachInfo.other
***/
void EEPROM_Init_MachInfo_other(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.other.startFlag = STRUCT_START_INIT_FLAG;
	MachInfo.other.size = sizeof(MachInfo.other);
	
	//所有的值在原来的基础上*100，例如：3.5* -> 3.5*100 = 350
    //通用
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usWBCH         = 950;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usWBCL         = 350;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usGranH        = 630;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usGranL        = 180;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usMidH         = 100;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usMidL         = 10;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usLymH         = 320;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usLymL         = 110;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usGranPercentH = 7500;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usGranPercentL = 4000;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usMidPercentH  = 1000;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usMidPercentL  = 300;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usLymPercentH  = 5000;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usLymPercentL  = 2000;
	MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usHGBH  		= 16000;
    MachInfo.other.refGroups[REFER_GROUP_CMOMMON].usHGBL  		= 11000;
    
    //成男
    MachInfo.other.refGroups[REFER_GROUP_MALE].usWBCH         = 950;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usWBCL         = 350;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usGranH        = 630;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usGranL        = 180;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usMidH         = 100;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usMidL         = 10;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usLymH         = 320;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usLymL         = 110;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usGranPercentH = 7500;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usGranPercentL = 4000;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usMidPercentH  = 1000;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usMidPercentL  = 300;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usLymPercentH  = 5000;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usLymPercentL  = 2000;
	MachInfo.other.refGroups[REFER_GROUP_MALE].usHGBH  		= 17500;
    MachInfo.other.refGroups[REFER_GROUP_MALE].usHGBL  		= 13000;
    
    //成女
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usWBCH         = 950;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usWBCL         = 350;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usGranH        = 630;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usGranL        = 180;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usMidH         = 100;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usMidL         = 10;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usLymH         = 320;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usLymL         = 110;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usGranPercentH = 7500;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usGranPercentL = 4000;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usMidPercentH  = 1000;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usMidPercentL  = 300;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usLymPercentH  = 5000;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usLymPercentL  = 2000;
	MachInfo.other.refGroups[REFER_GROUP_FEMALE].usHGBH  		= 15000;
    MachInfo.other.refGroups[REFER_GROUP_FEMALE].usHGBL  		= 11500;
    
    //儿童
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usWBCH         = 1200;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usWBCL         = 500;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usGranH        = 700;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usGranL        = 200;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usMidH         = 100;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usMidL         = 5;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usLymH         = 400;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usLymL         = 80;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usGranPercentH = 7000;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usGranPercentL = 5000;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usMidPercentH  = 1000;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usMidPercentL  = 150;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usLymPercentH  = 4000;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usLymPercentL  = 2000;
	MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usHGBH  		= 15500;
    MachInfo.other.refGroups[REFER_GROUP_CHILDREN].usHGBL  		= 12000;
	
    //新生儿
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usWBCH         = 2000;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usWBCL         = 1500;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usGranH        = 700;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usGranL        = 200;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usMidH         = 100;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usMidL         = 5;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usLymH         = 400;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usLymL         = 80;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usGranPercentH = 4000;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usGranPercentL = 3100;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usMidPercentH  = 1000;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usMidPercentL  = 150;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usLymPercentH  = 6000;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usLymPercentL  = 4000;
	MachInfo.other.refGroups[REFER_GROUP_BOBY].usHGBH  		= 20000;
    MachInfo.other.refGroups[REFER_GROUP_BOBY].usHGBL  		= 17000;
	
	MachInfo.other.factoryTimestamp = 0;
	MachInfo.other.userTimestamp = 0;
	MachInfo.other.totalOpenCount = 0;
	MachInfo.other.uiVersion = 100;
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_OTHER,(uint8_t*)&MachInfo.other,sizeof(MachInfo.other));
	
	printf("MachInfo.other size:%u\r\n",sizeof(MachInfo.other));
}



/***
*查找MachInfo.other中新增变量并对其初始化
***/
void EEPROM_AddValue_MachInfo_other(void)
{
	extern MachInfo_s MachInfo;
	
	MachInfo.other.size = sizeof(MachInfo.other);
	
	//相对于初始版本（即尚未正式发货前的版本），后续新增的变量需要在这里对比下是否为0xFF(0xFFFF...)，是，则将该变量进行初始化，否，则保持原样
	
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_OTHER,(uint8_t*)&MachInfo.other,sizeof(MachInfo.other));
}




/***
*读取MachInfo.other，并判断结构体起始标志和结构体字节长度字段
***/
void EEPROM_Read_MachInfo_other(void)
{
	extern MachInfo_s MachInfo;
	
	EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_OTHER,(uint8_t*)&MachInfo.other,sizeof(MachInfo.other));
	
	//判断起始标志是否异常
	if(MachInfo.other.startFlag != STRUCT_START_INIT_FLAG && MachInfo.other.startFlag != 0xFFFFFFFF){
		//重读一次
		EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_OTHER,(uint8_t*)&MachInfo.other,sizeof(MachInfo.other));
		
		if(MachInfo.other.startFlag != STRUCT_START_INIT_FLAG && MachInfo.other.startFlag != 0xFFFFFFFF){
			//数据被覆盖，锁定程序
			LOG_Error("数据被覆盖");
			while(1);
		}
	}
	
	if(MachInfo.other.startFlag == STRUCT_START_INIT_FLAG && MachInfo.other.size == sizeof(MachInfo.other)){
		//数据正常，直接退出即可
		return;
	}else if(MachInfo.other.startFlag == 0xFFFFFFFF){
		//新增的结构体，直接进行初始化操作即可
		EEPROM_Init_MachInfo_other();
	}else if(MachInfo.other.size != sizeof(MachInfo.other)){
		//结构体中有新增的变量，需要对新增变量进行初始化
		EEPROM_AddValue_MachInfo_other();
	}
}











