/*******************************************************************************
STM32F4的FLASH读写
********************************************************************************/
#include "flash.h"




/*宏定义区**************************************************************/





/*内部函数区************************************************************/
/***
*将地址转换为所处的扇区号
***/
static uint32_t GetSector(uint32_t Address)
{
	uint32_t sector = 0;

	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_Sector_0;  
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_Sector_1;  
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_Sector_2;  
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_Sector_3;  
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_Sector_4;  
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_Sector_5;  
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_Sector_6;  
	}
	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
	{
		sector = FLASH_Sector_7;  
	}
	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
	{
		sector = FLASH_Sector_8;  
	}
	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
	{
		sector = FLASH_Sector_9;  
	}
	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
	{
		sector = FLASH_Sector_10;  
	}
	else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
	{
		sector = FLASH_Sector_11;  
	}
	else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
	{
		sector = FLASH_Sector_12;  
	}
	else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
	{
		sector = FLASH_Sector_13;  
	}
	else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
	{
		sector = FLASH_Sector_14;  
	}
	else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
	{
		sector = FLASH_Sector_15;  
	}
	else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
	{
		sector = FLASH_Sector_16;  
	}
	else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
	{
		sector = FLASH_Sector_17;  
	}
	else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
	{
		sector = FLASH_Sector_18;  
	}
	else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
	{
		sector = FLASH_Sector_19;  
	}
	else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
	{
		sector = FLASH_Sector_20;  
	} 
	else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
	{
		sector = FLASH_Sector_21;  
	}
	else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
	{
		sector = FLASH_Sector_22;  
	}
	else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
	{
		sector = FLASH_Sector_23;  
	}
	
	return sector;
}



/***
*判断地址是否为各扇区的起始地址
*参数：
	Address：要判断的32位FLASH地址
*返回值：0表示不是扇区起始地址，返回1表示是扇区起始地址
***/
static uint8_t IsSectorStartAdr(uint32_t Address)
{
	switch(Address){
		case ADDR_FLASH_SECTOR_0:return 1;
		case ADDR_FLASH_SECTOR_1:return 1;
		case ADDR_FLASH_SECTOR_2:return 1;
		case ADDR_FLASH_SECTOR_3:return 1;
		case ADDR_FLASH_SECTOR_4:return 1;
		case ADDR_FLASH_SECTOR_5:return 1;
		case ADDR_FLASH_SECTOR_6:return 1;
		case ADDR_FLASH_SECTOR_7:return 1;
		case ADDR_FLASH_SECTOR_8:return 1;
		case ADDR_FLASH_SECTOR_9:return 1;
		case ADDR_FLASH_SECTOR_10:return 1;
		case ADDR_FLASH_SECTOR_11:return 1;
		case ADDR_FLASH_SECTOR_12:return 1;
		case ADDR_FLASH_SECTOR_13:return 1;
		case ADDR_FLASH_SECTOR_14:return 1;
		case ADDR_FLASH_SECTOR_15:return 1;
		case ADDR_FLASH_SECTOR_16:return 1;
		case ADDR_FLASH_SECTOR_17:return 1;
		case ADDR_FLASH_SECTOR_18:return 1;
		case ADDR_FLASH_SECTOR_19:return 1;
		case ADDR_FLASH_SECTOR_20:return 1;
		case ADDR_FLASH_SECTOR_21:return 1;
		case ADDR_FLASH_SECTOR_22:return 1;
		case ADDR_FLASH_SECTOR_23:return 1;
		default:return 0;
	}
}








/*外部函数区***********************************************************/
/***
*FLASH写入数据函数，按字节写入
*参数：
	StartAddress：要写入FLASH的地址，每次根据是否是扇区的起始地址来判断是否需要擦除操作
	Data：要写入的数据的首地址,这里必须是8位的地址指针
	Len：写入长度，注意F4的FLASH可以是8位/16位/32位/64位写入，所以这里的长度是表示一个字节/半字/字/双字为单位的长度
*返回值：返回此次操作FLASH的状态值
***/
FLASH_Status FLASH_WriteData_8(uint32_t StartAddress,uint8_t *Data,uint32_t Len)
{
	uint32_t EndAdress = 0;
	uint32_t StartSector = 0;
	uint32_t EndSector = 0;
	uint32_t SectorCount = 0;
	FLASH_Status FlashStatus = FLASH_COMPLETE;
	
	if(Len == 0){
		return FlashStatus;
	}
	
	//解锁FLASH，以便可以写入数据
	FLASH_Unlock();
	
	//清除FLASH标志
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	//判断是否是扇区的起始地址，是，则执行扇区擦除操作
	if(IsSectorStartAdr(StartAddress) == 1){
		//获取此次写入的结束地址
		EndAdress = StartAddress + Len;
		
		//获取扇区号
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//开始擦除
		while(SectorCount <= EndSector){
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//上锁FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//扇区号增加
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
		}
	}else{
		//非扇区起始地址时，则还要判断是否会跨扇区
		//获取此次写入的结束地址
		EndAdress = StartAddress + Len;
		
		//获取扇区号
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//扇区号先增加一次
		if (SectorCount == FLASH_Sector_11){
			SectorCount += 40;
		}else{
			SectorCount += 8;
		}
		
		//开始擦除
		while(SectorCount <= EndSector){
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//上锁FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//扇区号增加
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
		}
	}
	
	
	//写入数据
	while(Len--){
		FlashStatus = FLASH_ProgramByte(StartAddress,*Data);
		StartAddress += 1;
		Data += 1;
		
		if (FlashStatus != FLASH_COMPLETE){
			//上锁FLASH
			FLASH_Lock();
			return FlashStatus;
		}
	}
	
	//上锁FLASH
	FLASH_Lock();
	return FlashStatus;
}


/***
*FLASH写入数据函数，按半字写入
*参数：
	StartAddress：要写入FLASH的地址，每次根据是否是扇区的起始地址来判断是否需要擦除操作
	AdrAddNum：为保持写入地址和每次写入的字节数保持字节对齐，从而增加的空地址值，这个是返回给调用者使用的
	Data：要写入的数据的首地址,这里必须是16位的地址指针
	Len：写入长度，注意F4的FLASH可以是8位/16位/32位/64位写入，所以这里的长度是表示一个字节/半字/字/双字为单位的长度
*返回值：返回此次操作FLASH的状态值
***/
FLASH_Status FLASH_WriteData_16(uint32_t StartAddress,uint16_t* AdrAddNum,uint16_t *Data,uint16_t Len)
{
	uint32_t EndAdress = 0;
	uint32_t StartSector = 0;
	uint32_t EndSector = 0;
	uint32_t SectorCount = 0;
	uint8_t AdrAddNumTemp = 0;
	FLASH_Status FlashStatus = FLASH_COMPLETE;
	
	if(Len == 0){
		return FlashStatus;
	}
	
	//解锁FLASH，以便可以写入数据
	FLASH_Unlock();
	
	//清除FLASH标志
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	//判断是否是扇区的起始地址，是，则执行扇区擦除操作
	if(IsSectorStartAdr(StartAddress) == 1){
		//获取此次写入的结束地址
		EndAdress = StartAddress + Len*2;
		
		//获取扇区号
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//开始擦除
		while(SectorCount <= EndSector){
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//上锁FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//扇区号增加
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
		}
	}else{
		//非扇区起始地址时，则还要判断是否会跨扇区
		//获取此次写入的结束地址
		EndAdress = StartAddress + Len*2;
		
		//获取扇区号
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//扇区号先增加一次
		if (SectorCount == FLASH_Sector_11){
			SectorCount += 40;
		}else{
			SectorCount += 8;
		}
		
		//开始擦除
		while(SectorCount <= EndSector){
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//上锁FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//扇区号增加
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
		}
	}
	
	//判断是否字节对齐
	AdrAddNumTemp = StartAddress % 2;
	if(AdrAddNumTemp != 0){
		StartAddress += (2 - AdrAddNumTemp);
	}
	
	//写入数据
	while(Len--){
		FlashStatus = FLASH_ProgramHalfWord(StartAddress,*Data);
		StartAddress += 2;
		Data += 1;
		
		if (FlashStatus != FLASH_COMPLETE){
			//上锁FLASH
			FLASH_Lock();
			return FlashStatus;
		}
	}
	
	//上锁FLASH
	FLASH_Lock();
	
	if(AdrAddNumTemp != 0){
		*AdrAddNum += (2 - AdrAddNumTemp);
	}
	
	return FlashStatus;
}


/***
*FLASH写入数据函数，按字写入
*参数：
	StartAddress：要写入FLASH的地址，每次根据是否是扇区的起始地址来判断是否需要擦除操作
	AdrAddNum：为保持写入地址和每次写入的字节数保持字节对齐，从而增加的空地址值，这个是返回给调用者使用的
	Data：要写入的数据的首地址,这里必须是32位的地址指针
	Len：写入长度，注意F4的FLASH可以是8位/16位/32位/64位写入，所以这里的长度是表示一个字节/半字/字/双字为单位的长度
*返回值：返回此次操作FLASH的状态值
***/
FLASH_Status FLASH_WriteData_32(uint32_t StartAddress,uint16_t* AdrAddNum,uint32_t *Data,uint16_t Len)
{
	uint32_t EndAdress = 0;
	uint32_t StartSector = 0;
	uint32_t EndSector = 0;
	uint32_t SectorCount = 0;
	uint8_t AdrAddNumTemp = 0;
	FLASH_Status FlashStatus = FLASH_COMPLETE;
	
	if(Len == 0){
		return FlashStatus;
	}
	
	//解锁FLASH，以便可以写入数据
	FLASH_Unlock();
	
	//清除FLASH标志
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	//判断是否是扇区的起始地址，是，则执行扇区擦除操作
	if(IsSectorStartAdr(StartAddress) == 1){
		//获取此次写入的结束地址
		EndAdress = StartAddress + Len*4;
		
		//获取扇区号
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//开始擦除
		while(SectorCount <= EndSector){
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//上锁FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//扇区号增加
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
		}
	}else{
		//非扇区起始地址时，则还要判断是否会跨扇区
		//获取此次写入的结束地址
		EndAdress = StartAddress + Len*4;
		
		//获取扇区号
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//扇区号先增加一次
		if (SectorCount == FLASH_Sector_11){
			SectorCount += 40;
		}else{
			SectorCount += 8;
		}
		
		//开始擦除
		while(SectorCount <= EndSector){
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//上锁FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//扇区号增加
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
		}
	}
	
	//判断是否字节对齐
	AdrAddNumTemp = StartAddress % 4;
	if(AdrAddNumTemp != 0){
		StartAddress += (4 - AdrAddNumTemp);
	}
	
	
	//写入数据
	while(Len--){
		FlashStatus = FLASH_ProgramWord(StartAddress,*Data);
		StartAddress += 4;
		Data += 1;
		
		if (FlashStatus != FLASH_COMPLETE){
			//上锁FLASH
			FLASH_Lock();
			return FlashStatus;
		}
	}
	
	//上锁FLASH
	FLASH_Lock();
	
	if(AdrAddNumTemp != 0){
		*AdrAddNum += (4 - AdrAddNumTemp);
	}
	
	return FlashStatus;
}


/***
*从FLASH读取数据函数，按字节读取
*参数：
	StartAddress：要读取数据的Flash地址
	Data：读取到的数据存放区域首地址
	Len：读取单元数据长度
***/
void FLASH_ReadData_8(uint32_t StartAddress,uint8_t *Data,uint16_t Len)
{
	while(Len--){
		*Data = *((uint8_t*)StartAddress);
		Data += 1;
		StartAddress += 1;
	}
}


/***
*从FLASH读取数据函数，按半字读取
*参数：
	StartAddress：要读取数据的Flash地址
	Data：读取到的数据存放区域首地址
	Len：读取单元数据长度
***/
void FLASH_ReadData_16(uint32_t StartAddress,uint16_t *Data,uint16_t Len)
{
	uint8_t AdrAddNumTemp = 0;
	
	//判断是否字节对齐
	AdrAddNumTemp = StartAddress % 2;
	if(AdrAddNumTemp != 0){
		StartAddress += (2 - AdrAddNumTemp);
	}
	
	while(Len--){
		*Data = *((uint16_t*)StartAddress);
		Data += 1;
		StartAddress += 2;
	}
}


/***
*从FLASH读取数据函数，按字读取
*参数：
	StartAddress：要读取数据的Flash地址
	Data：读取到的数据存放区域首地址
	Len：读取单元数据长度
***/
void FLASH_ReadData_32(uint32_t StartAddress,uint32_t *Data,uint16_t Len)
{
	uint8_t AdrAddNumTemp = 0;
	
	//判断是否字节对齐
	AdrAddNumTemp = StartAddress % 4;
	if(AdrAddNumTemp != 0){
		StartAddress += (4 - AdrAddNumTemp);
	}
	
	while(Len--){
		*Data = *((uint32_t*)StartAddress);
		Data += 1;
		StartAddress += 4;
	}
}





/***
*开启或者清除写保护，注意，WRP寄存器中的位为1时表示对应的扇区没有开启写保护，位为0时表示开启了写保护。
*参数：
	WRPSectors:需要设置写保护或者清除写保护的扇区对应的位构成的32位数值，比如，OB_WRP_Sector_0|OB_WRP_Sector_1
	State：ENABLE时表示设置写保护，DISABLE时表示清除写保护
*返回值 ：FLASH操作状态标志
***/
FLASH_Status FLASH_WriteProtect(uint32_t WRPSectors,FunctionalState State)
{
	FLASH_Status FlashStatus = FLASH_COMPLETE;
	uint16_t LastWRP = 0;
	
	//先读回当前选项字中的写保护设置，避免由于开关机导致重复执行此操作，缩短FLASH寿命
	LastWRP = FLASH_OB_GetWRP();
	
	//由于WRP寄存器对应的位为1表示未激活写保护，0表示激活写保护，所以为和下面逻辑一致，这里取反
	LastWRP = ~LastWRP;
	
	//判断开启还是关闭扇区写保护
	if(State == ENABLE){
		//判断是否已经开启指定写保护了，是，则不用执行下面操作了
		if((LastWRP & WRPSectors) == WRPSectors){
			return FlashStatus;
		}
		
		//解锁选项字
		FLASH_OB_Unlock();

		//开启指定扇区写保护
		FLASH_OB_WRPConfig(WRPSectors, ENABLE); 

		//将改动写入选项字
		FlashStatus = FLASH_OB_Launch();

		//锁定选项字
		FLASH_OB_Lock();
	}else{
		//判断是否已经关闭指定写保护了，是，则不用执行下面操作了
		if((LastWRP & WRPSectors) == 0){
			return FlashStatus;
		}
		
		//解锁选项字
		FLASH_OB_Unlock();

		//关闭指定扇区写保护
		FLASH_OB_WRPConfig(WRPSectors, DISABLE); 

		//将改动写入选项字
		FlashStatus = FLASH_OB_Launch();

		//锁定选项字
		FLASH_OB_Lock();
	}
	
	return FlashStatus;
}


/***
*开启或者清除读保护（级别1），注意，降级会擦除FLASH，开启了读保护后需要上电重启
*参数：
	State:开启/关闭读保护
*返回值：返回此次FLASH操作的结果
***/
FLASH_Status FLASH_ReadProtect(FunctionalState State)
{
	FLASH_Status FlashState = FLASH_COMPLETE;
	uint8_t RDPAlreadyFlag = 0;
	
	//读回当前是否已经设置读保护，防止重复设置
	RDPAlreadyFlag = FLASH_OB_GetRDP();
	if(State == ENABLE){
		if(RDPAlreadyFlag == SET){
			return FlashState;
		}
	}else{
		if(RDPAlreadyFlag == RESET){
			return FlashState;
		}
	}
	
	FLASH_OB_Unlock();
	
	//清除FLASH标志
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	if(State == ENABLE){
		//开启读保护
		FLASH_OB_RDPConfig(OB_RDP_Level_1);
		FlashState = FLASH_OB_Launch();
	}else{
		//关闭读保护
		FLASH_OB_RDPConfig(OB_RDP_Level_0);
		FlashState = FLASH_OB_Launch();
	}
	
	//锁定选项字
	FLASH_OB_Lock();
	
	return FlashState;
}








