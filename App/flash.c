#include "flash.h"
#include "main.h"
#include "msg_def.h"



/*
*	FLASH 扇区的起始地址及大小
*/
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//扇区7起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//扇区11起始地址,128 Kbytes 

#define ADDR_FLASH_SECTOR_12	((uint32_t)0x08100000) 	//扇区12起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_13	((uint32_t)0x08104000) 	//扇区13起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_14    ((uint32_t)0x08108000) 	//扇区14起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_15	((uint32_t)0x0810C000) 	//扇区15起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_16    ((uint32_t)0x08110000) 	//扇区16起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_17	((uint32_t)0x08120000) 	//扇区17起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_18	((uint32_t)0x08140000) 	//扇区18起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_19	((uint32_t)0x08160000) 	//扇区19起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_20    ((uint32_t)0x08180000) 	//扇区20起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_21	((uint32_t)0x081A0000) 	//扇区21起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_22    ((uint32_t)0x081C0000) 	//扇区22起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_23    ((uint32_t)0x081E0000) 	//扇区23起始地址, 128 Kbytes  


/*
*	根据地址，返回其所处的扇区（0-11）
*/
uint8_t FLASH_GetFlashSector(uint32_t addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)		return FLASH_SECTOR_0;
	else if(addr<ADDR_FLASH_SECTOR_2)	return FLASH_SECTOR_1;
	else if(addr<ADDR_FLASH_SECTOR_3)	return FLASH_SECTOR_2;
	else if(addr<ADDR_FLASH_SECTOR_4)	return FLASH_SECTOR_3;
	else if(addr<ADDR_FLASH_SECTOR_5)	return FLASH_SECTOR_4;
	else if(addr<ADDR_FLASH_SECTOR_6)	return FLASH_SECTOR_5;
	else if(addr<ADDR_FLASH_SECTOR_7)	return FLASH_SECTOR_6;
	else if(addr<ADDR_FLASH_SECTOR_8)	return FLASH_SECTOR_7;
	else if(addr<ADDR_FLASH_SECTOR_9)	return FLASH_SECTOR_8;
	else if(addr<ADDR_FLASH_SECTOR_10)	return FLASH_SECTOR_9;
	else if(addr<ADDR_FLASH_SECTOR_11)	return FLASH_SECTOR_10; 
	else if(addr<ADDR_FLASH_SECTOR_12)	return FLASH_SECTOR_11; 
	else if(addr<ADDR_FLASH_SECTOR_13)	return FLASH_SECTOR_12; 
	else if(addr<ADDR_FLASH_SECTOR_14)	return FLASH_SECTOR_13; 
	else if(addr<ADDR_FLASH_SECTOR_15)	return FLASH_SECTOR_14; 
	else if(addr<ADDR_FLASH_SECTOR_16)	return FLASH_SECTOR_15; 
	else if(addr<ADDR_FLASH_SECTOR_17)	return FLASH_SECTOR_16; 
	else if(addr<ADDR_FLASH_SECTOR_18)	return FLASH_SECTOR_17; 
	else if(addr<ADDR_FLASH_SECTOR_19)	return FLASH_SECTOR_18; 
	else if(addr<ADDR_FLASH_SECTOR_20)	return FLASH_SECTOR_19; 
	else if(addr<ADDR_FLASH_SECTOR_21)	return FLASH_SECTOR_20; 
	else if(addr<ADDR_FLASH_SECTOR_22)	return FLASH_SECTOR_21; 
	else if(addr<ADDR_FLASH_SECTOR_23)	return FLASH_SECTOR_22;  
	return FLASH_SECTOR_23;	
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



/***
*将地址转换为所处的扇区号
***/
static uint32_t GetSector(uint32_t Address)
{
	uint32_t sector = 0;

	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_SECTOR_0;  
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_SECTOR_1;  
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_SECTOR_2;  
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_SECTOR_3;  
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_SECTOR_4;  
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_SECTOR_5;  
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_SECTOR_6;  
	}
	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
	{
		sector = FLASH_SECTOR_7;  
	}
	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
	{
		sector = FLASH_SECTOR_8;  
	}
	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
	{
		sector = FLASH_SECTOR_9;  
	}
	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
	{
		sector = FLASH_SECTOR_10;  
	}
	else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
	{
		sector = FLASH_SECTOR_11;  
	}
	else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
	{
		sector = FLASH_SECTOR_12;  
	}
	else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
	{
		sector = FLASH_SECTOR_13;  
	}
	else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
	{
		sector = FLASH_SECTOR_14;  
	}
	else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
	{
		sector = FLASH_SECTOR_15;  
	}
	else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
	{
		sector = FLASH_SECTOR_16;  
	}
	else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
	{
		sector = FLASH_SECTOR_17;  
	}
	else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
	{
		sector = FLASH_SECTOR_18;  
	}
	else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
	{
		sector = FLASH_SECTOR_19;  
	}
	else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
	{
		sector = FLASH_SECTOR_20;  
	} 
	else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
	{
		sector = FLASH_SECTOR_21;  
	}
	else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
	{
		sector = FLASH_SECTOR_22;  
	}
	else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
	{
		sector = FLASH_SECTOR_23;  
	}
	
	return sector;
}



///*
//*	擦涂单个扇区 ， FLASH_Erase(FLASH_SECTOR_2);
//*/
//uint8_t FLASH_Erase(uint8_t ucSector)	
//{ 
//	FLASH_EraseInitTypeDef FlashEraseInit;
//	HAL_StatusTypeDef FlashStatus=HAL_OK;
//	uint32_t SectorError = 0;

//	//检测
//	if(ucSector > FLASH_SECTOR_11)
//	{
//		LOG_Info("Sector(%d) Out Max Value\r\n", ucSector);
//		return !HAL_OK;
//	}
//	
//	//解锁	
//	HAL_FLASH_Unlock();             
//	FlashEraseInit.TypeErase		= FLASH_TYPEERASE_SECTORS;      //擦除类型，扇区擦除 
//	FlashEraseInit.Sector			= ucSector;   					//要擦除的扇区
//	FlashEraseInit.NbSectors 		= 1;                            //一次只擦除一个扇区
//	FlashEraseInit.VoltageRange	    =	FLASH_VOLTAGE_RANGE_3;      //电压范围，VCC=2.7~3.6V之间!!
//	if(HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError)!=HAL_OK) 
//	{
//		LOG_Info("Erase Sector(%d) Error\r\n", ucSector);
//	}
//	FlashStatus= FLASH_WaitForLastOperation(FLASH_WAITETIME);        //等待上次操作完成

//	HAL_FLASH_Lock();           //上锁
//	return FlashStatus;
//}



///*
//*	往flash中写入参数数据
//*/
//void FLASH_Write(uint8_t *pParam, uint32_t ulWriteLen)	
//{ 
//	FLASH_EraseInitTypeDef FlashEraseInit;
//	HAL_StatusTypeDef FlashStatus=HAL_OK;
//	
//	uint32_t SectorError = 0;
//	uint32_t ulWriteAddr = PARAM_FLASH_BASE;	//参数起始地址
//	uint32_t i = 0;	
//    
//	//检测长度
//	if(ulWriteLen > PARAM_FLASH_MAX_LEN)
//	{
//		LOG_Info("Error: Flash Write Len more than PARAM_FLASH_MAX_LEN\r\n");
//		return;
//	}
//	
//	//解锁	
//	HAL_FLASH_Unlock();             
//	FlashEraseInit.TypeErase		= FLASH_TYPEERASE_SECTORS;       		//擦除类型，扇区擦除 
//	FlashEraseInit.Sector			= FLASH_GetFlashSector(ulWriteAddr);   	//要擦除的扇区
//	FlashEraseInit.NbSectors 		= 1;                             		//一次只擦除一个扇区
//	FlashEraseInit.VoltageRange	=	FLASH_VOLTAGE_RANGE_3;      			//电压范围，VCC=2.7~3.6V之间!!
//	if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
//	{
//		LOG_Info("Erase %d Error\r\n", FLASH_GetFlashSector(ulWriteAddr));
//	}
//	FlashStatus= FLASH_WaitForLastOperation(FLASH_WAITETIME);                //等待上次操作完成
//	if(FlashStatus==HAL_OK)
//	{
//		//写数据
//		 for(i = 0; i < ulWriteLen; i++)
//		 {
//			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, ulWriteAddr + i,*(pParam + i))!=HAL_OK)//写入数据
//			{ 
//				LOG_Info("Flash Write Error\r\n");
//				break;
//			}
//		}  
//	}
//	HAL_FLASH_Lock();  //上锁
//} 



///*
//*	读取数据	
//*/
//void FLASH_Read(uint8_t *pParam,uint32_t ulReadLen)   	
//{
//	uint32_t i = 0;
//	uint32_t ulReadAddr = PARAM_FLASH_BASE;	//参数起始地址
//	
//	//检测长度
//	if(ulReadLen > PARAM_FLASH_MAX_LEN)
//	{
//		LOG_Info("Error: Flash Read Len more than PARAM_FLASH_MAX_LEN\r\n");
//		return;
//	}
//	
//	//读数据
//	for(i = 0; i < ulReadLen; i++)
//	{
//		pParam[i]= *(__IO uint8_t*)(ulReadAddr + i);
//	}
//}


/***
*FLASH写入数据函数，按字节写入
*参数：
	StartAddress：要写入FLASH的地址，每次根据是否是扇区的起始地址来判断是否需要擦除操作
	Data：要写入的数据的首地址,这里必须是8位的地址指针
	Len：写入长度，注意F4的FLASH可以是8位/16位/32位/64位写入，所以这里的长度是表示一个字节/半字/字/双字为单位的长度
*返回值：返回此次操作FLASH的状态值
***/
HAL_StatusTypeDef FLASH_WriteData_8(uint32_t StartAddress,uint8_t *Data,uint32_t Len)
{
	uint32_t EndAdress = 0;
	uint32_t StartSector = 0;
	uint32_t EndSector = 0;
	uint32_t SectorCount = 0;
	HAL_StatusTypeDef FlashStatus = HAL_OK;
	
	if(Len == 0){
		return FlashStatus;
	}
	
	//解锁FLASH，以便可以写入数据
	FlashStatus = HAL_FLASH_Unlock();
	if(FlashStatus != HAL_OK){
		return FlashStatus;
	}
	
	//清除FLASH标志
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
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
			FLASH_Erase_Sector(SectorCount,FLASH_VOLTAGE_RANGE_3);
			
			//扇区号增加
			SectorCount++;
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
		SectorCount++;
		
		//开始擦除
		while(SectorCount <= EndSector){
			FLASH_Erase_Sector(SectorCount,FLASH_VOLTAGE_RANGE_3);
			
			//扇区号增加
			SectorCount++;
		}
	}
	
	
	//写入数据
	while(Len--){
		FlashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,StartAddress,*Data);
		StartAddress += 1;
		Data += 1;
		
		if (FlashStatus != HAL_OK){
			//上锁FLASH
			HAL_FLASH_Lock();
			return FlashStatus;
		}
	}
	
	//上锁FLASH
	HAL_FLASH_Lock();
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

