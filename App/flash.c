#include "flash.h"
#include "main.h"
#include "msg_def.h"



/*
*	FLASH ��������ʼ��ַ����С
*/
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes 

#define ADDR_FLASH_SECTOR_12	((uint32_t)0x08100000) 	//����12��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_13	((uint32_t)0x08104000) 	//����13��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_14    ((uint32_t)0x08108000) 	//����14��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_15	((uint32_t)0x0810C000) 	//����15��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_16    ((uint32_t)0x08110000) 	//����16��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_17	((uint32_t)0x08120000) 	//����17��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_18	((uint32_t)0x08140000) 	//����18��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_19	((uint32_t)0x08160000) 	//����19��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_20    ((uint32_t)0x08180000) 	//����20��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_21	((uint32_t)0x081A0000) 	//����21��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_22    ((uint32_t)0x081C0000) 	//����22��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_23    ((uint32_t)0x081E0000) 	//����23��ʼ��ַ, 128 Kbytes  


/*
*	���ݵ�ַ��������������������0-11��
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
*�жϵ�ַ�Ƿ�Ϊ����������ʼ��ַ
*������
	Address��Ҫ�жϵ�32λFLASH��ַ
*����ֵ��0��ʾ����������ʼ��ַ������1��ʾ��������ʼ��ַ
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
*����ַת��Ϊ������������
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
//*	��Ϳ�������� �� FLASH_Erase(FLASH_SECTOR_2);
//*/
//uint8_t FLASH_Erase(uint8_t ucSector)	
//{ 
//	FLASH_EraseInitTypeDef FlashEraseInit;
//	HAL_StatusTypeDef FlashStatus=HAL_OK;
//	uint32_t SectorError = 0;

//	//���
//	if(ucSector > FLASH_SECTOR_11)
//	{
//		LOG_Info("Sector(%d) Out Max Value\r\n", ucSector);
//		return !HAL_OK;
//	}
//	
//	//����	
//	HAL_FLASH_Unlock();             
//	FlashEraseInit.TypeErase		= FLASH_TYPEERASE_SECTORS;      //�������ͣ��������� 
//	FlashEraseInit.Sector			= ucSector;   					//Ҫ����������
//	FlashEraseInit.NbSectors 		= 1;                            //һ��ֻ����һ������
//	FlashEraseInit.VoltageRange	    =	FLASH_VOLTAGE_RANGE_3;      //��ѹ��Χ��VCC=2.7~3.6V֮��!!
//	if(HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError)!=HAL_OK) 
//	{
//		LOG_Info("Erase Sector(%d) Error\r\n", ucSector);
//	}
//	FlashStatus= FLASH_WaitForLastOperation(FLASH_WAITETIME);        //�ȴ��ϴβ������

//	HAL_FLASH_Lock();           //����
//	return FlashStatus;
//}



///*
//*	��flash��д���������
//*/
//void FLASH_Write(uint8_t *pParam, uint32_t ulWriteLen)	
//{ 
//	FLASH_EraseInitTypeDef FlashEraseInit;
//	HAL_StatusTypeDef FlashStatus=HAL_OK;
//	
//	uint32_t SectorError = 0;
//	uint32_t ulWriteAddr = PARAM_FLASH_BASE;	//������ʼ��ַ
//	uint32_t i = 0;	
//    
//	//��ⳤ��
//	if(ulWriteLen > PARAM_FLASH_MAX_LEN)
//	{
//		LOG_Info("Error: Flash Write Len more than PARAM_FLASH_MAX_LEN\r\n");
//		return;
//	}
//	
//	//����	
//	HAL_FLASH_Unlock();             
//	FlashEraseInit.TypeErase		= FLASH_TYPEERASE_SECTORS;       		//�������ͣ��������� 
//	FlashEraseInit.Sector			= FLASH_GetFlashSector(ulWriteAddr);   	//Ҫ����������
//	FlashEraseInit.NbSectors 		= 1;                             		//һ��ֻ����һ������
//	FlashEraseInit.VoltageRange	=	FLASH_VOLTAGE_RANGE_3;      			//��ѹ��Χ��VCC=2.7~3.6V֮��!!
//	if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
//	{
//		LOG_Info("Erase %d Error\r\n", FLASH_GetFlashSector(ulWriteAddr));
//	}
//	FlashStatus= FLASH_WaitForLastOperation(FLASH_WAITETIME);                //�ȴ��ϴβ������
//	if(FlashStatus==HAL_OK)
//	{
//		//д����
//		 for(i = 0; i < ulWriteLen; i++)
//		 {
//			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, ulWriteAddr + i,*(pParam + i))!=HAL_OK)//д������
//			{ 
//				LOG_Info("Flash Write Error\r\n");
//				break;
//			}
//		}  
//	}
//	HAL_FLASH_Lock();  //����
//} 



///*
//*	��ȡ����	
//*/
//void FLASH_Read(uint8_t *pParam,uint32_t ulReadLen)   	
//{
//	uint32_t i = 0;
//	uint32_t ulReadAddr = PARAM_FLASH_BASE;	//������ʼ��ַ
//	
//	//��ⳤ��
//	if(ulReadLen > PARAM_FLASH_MAX_LEN)
//	{
//		LOG_Info("Error: Flash Read Len more than PARAM_FLASH_MAX_LEN\r\n");
//		return;
//	}
//	
//	//������
//	for(i = 0; i < ulReadLen; i++)
//	{
//		pParam[i]= *(__IO uint8_t*)(ulReadAddr + i);
//	}
//}


/***
*FLASHд�����ݺ��������ֽ�д��
*������
	StartAddress��Ҫд��FLASH�ĵ�ַ��ÿ�θ����Ƿ�����������ʼ��ַ���ж��Ƿ���Ҫ��������
	Data��Ҫд������ݵ��׵�ַ,���������8λ�ĵ�ַָ��
	Len��д�볤�ȣ�ע��F4��FLASH������8λ/16λ/32λ/64λд�룬��������ĳ����Ǳ�ʾһ���ֽ�/����/��/˫��Ϊ��λ�ĳ���
*����ֵ�����ش˴β���FLASH��״ֵ̬
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
	
	//����FLASH���Ա����д������
	FlashStatus = HAL_FLASH_Unlock();
	if(FlashStatus != HAL_OK){
		return FlashStatus;
	}
	
	//���FLASH��־
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	//�ж��Ƿ�����������ʼ��ַ���ǣ���ִ��������������
	if(IsSectorStartAdr(StartAddress) == 1){
		//��ȡ�˴�д��Ľ�����ַ
		EndAdress = StartAddress + Len;
		
		//��ȡ������
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//��ʼ����
		while(SectorCount <= EndSector){
			FLASH_Erase_Sector(SectorCount,FLASH_VOLTAGE_RANGE_3);
			
			//����������
			SectorCount++;
		}
	}else{
		//��������ʼ��ַʱ����Ҫ�ж��Ƿ�������
		//��ȡ�˴�д��Ľ�����ַ
		EndAdress = StartAddress + Len;
		
		//��ȡ������
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//������������һ��
		SectorCount++;
		
		//��ʼ����
		while(SectorCount <= EndSector){
			FLASH_Erase_Sector(SectorCount,FLASH_VOLTAGE_RANGE_3);
			
			//����������
			SectorCount++;
		}
	}
	
	
	//д������
	while(Len--){
		FlashStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,StartAddress,*Data);
		StartAddress += 1;
		Data += 1;
		
		if (FlashStatus != HAL_OK){
			//����FLASH
			HAL_FLASH_Lock();
			return FlashStatus;
		}
	}
	
	//����FLASH
	HAL_FLASH_Lock();
	return FlashStatus;
}



/***
*��FLASH��ȡ���ݺ��������ֽڶ�ȡ
*������
	StartAddress��Ҫ��ȡ���ݵ�Flash��ַ
	Data����ȡ�������ݴ�������׵�ַ
	Len����ȡ��Ԫ���ݳ���
***/
void FLASH_ReadData_8(uint32_t StartAddress,uint8_t *Data,uint16_t Len)
{
	while(Len--){
		*Data = *((uint8_t*)StartAddress);
		Data += 1;
		StartAddress += 1;
	}
}

