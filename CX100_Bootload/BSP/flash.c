/*******************************************************************************
STM32F4��FLASH��д
********************************************************************************/
#include "flash.h"




/*�궨����**************************************************************/





/*�ڲ�������************************************************************/
/***
*����ַת��Ϊ������������
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








/*�ⲿ������***********************************************************/
/***
*FLASHд�����ݺ��������ֽ�д��
*������
	StartAddress��Ҫд��FLASH�ĵ�ַ��ÿ�θ����Ƿ�����������ʼ��ַ���ж��Ƿ���Ҫ��������
	Data��Ҫд������ݵ��׵�ַ,���������8λ�ĵ�ַָ��
	Len��д�볤�ȣ�ע��F4��FLASH������8λ/16λ/32λ/64λд�룬��������ĳ����Ǳ�ʾһ���ֽ�/����/��/˫��Ϊ��λ�ĳ���
*����ֵ�����ش˴β���FLASH��״ֵ̬
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
	
	//����FLASH���Ա����д������
	FLASH_Unlock();
	
	//���FLASH��־
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
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
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//����FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//����������
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
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
		if (SectorCount == FLASH_Sector_11){
			SectorCount += 40;
		}else{
			SectorCount += 8;
		}
		
		//��ʼ����
		while(SectorCount <= EndSector){
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//����FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//����������
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
		}
	}
	
	
	//д������
	while(Len--){
		FlashStatus = FLASH_ProgramByte(StartAddress,*Data);
		StartAddress += 1;
		Data += 1;
		
		if (FlashStatus != FLASH_COMPLETE){
			//����FLASH
			FLASH_Lock();
			return FlashStatus;
		}
	}
	
	//����FLASH
	FLASH_Lock();
	return FlashStatus;
}


/***
*FLASHд�����ݺ�����������д��
*������
	StartAddress��Ҫд��FLASH�ĵ�ַ��ÿ�θ����Ƿ�����������ʼ��ַ���ж��Ƿ���Ҫ��������
	AdrAddNum��Ϊ����д���ַ��ÿ��д����ֽ��������ֽڶ��룬�Ӷ����ӵĿյ�ֵַ������Ƿ��ظ�������ʹ�õ�
	Data��Ҫд������ݵ��׵�ַ,���������16λ�ĵ�ַָ��
	Len��д�볤�ȣ�ע��F4��FLASH������8λ/16λ/32λ/64λд�룬��������ĳ����Ǳ�ʾһ���ֽ�/����/��/˫��Ϊ��λ�ĳ���
*����ֵ�����ش˴β���FLASH��״ֵ̬
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
	
	//����FLASH���Ա����д������
	FLASH_Unlock();
	
	//���FLASH��־
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	//�ж��Ƿ�����������ʼ��ַ���ǣ���ִ��������������
	if(IsSectorStartAdr(StartAddress) == 1){
		//��ȡ�˴�д��Ľ�����ַ
		EndAdress = StartAddress + Len*2;
		
		//��ȡ������
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//��ʼ����
		while(SectorCount <= EndSector){
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//����FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//����������
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
		}
	}else{
		//��������ʼ��ַʱ����Ҫ�ж��Ƿ�������
		//��ȡ�˴�д��Ľ�����ַ
		EndAdress = StartAddress + Len*2;
		
		//��ȡ������
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//������������һ��
		if (SectorCount == FLASH_Sector_11){
			SectorCount += 40;
		}else{
			SectorCount += 8;
		}
		
		//��ʼ����
		while(SectorCount <= EndSector){
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//����FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//����������
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
		}
	}
	
	//�ж��Ƿ��ֽڶ���
	AdrAddNumTemp = StartAddress % 2;
	if(AdrAddNumTemp != 0){
		StartAddress += (2 - AdrAddNumTemp);
	}
	
	//д������
	while(Len--){
		FlashStatus = FLASH_ProgramHalfWord(StartAddress,*Data);
		StartAddress += 2;
		Data += 1;
		
		if (FlashStatus != FLASH_COMPLETE){
			//����FLASH
			FLASH_Lock();
			return FlashStatus;
		}
	}
	
	//����FLASH
	FLASH_Lock();
	
	if(AdrAddNumTemp != 0){
		*AdrAddNum += (2 - AdrAddNumTemp);
	}
	
	return FlashStatus;
}


/***
*FLASHд�����ݺ���������д��
*������
	StartAddress��Ҫд��FLASH�ĵ�ַ��ÿ�θ����Ƿ�����������ʼ��ַ���ж��Ƿ���Ҫ��������
	AdrAddNum��Ϊ����д���ַ��ÿ��д����ֽ��������ֽڶ��룬�Ӷ����ӵĿյ�ֵַ������Ƿ��ظ�������ʹ�õ�
	Data��Ҫд������ݵ��׵�ַ,���������32λ�ĵ�ַָ��
	Len��д�볤�ȣ�ע��F4��FLASH������8λ/16λ/32λ/64λд�룬��������ĳ����Ǳ�ʾһ���ֽ�/����/��/˫��Ϊ��λ�ĳ���
*����ֵ�����ش˴β���FLASH��״ֵ̬
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
	
	//����FLASH���Ա����д������
	FLASH_Unlock();
	
	//���FLASH��־
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	//�ж��Ƿ�����������ʼ��ַ���ǣ���ִ��������������
	if(IsSectorStartAdr(StartAddress) == 1){
		//��ȡ�˴�д��Ľ�����ַ
		EndAdress = StartAddress + Len*4;
		
		//��ȡ������
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//��ʼ����
		while(SectorCount <= EndSector){
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//����FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//����������
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
		}
	}else{
		//��������ʼ��ַʱ����Ҫ�ж��Ƿ�������
		//��ȡ�˴�д��Ľ�����ַ
		EndAdress = StartAddress + Len*4;
		
		//��ȡ������
		StartSector = GetSector(StartAddress);
		EndSector = GetSector(EndAdress);
		SectorCount = StartSector;
		
		//������������һ��
		if (SectorCount == FLASH_Sector_11){
			SectorCount += 40;
		}else{
			SectorCount += 8;
		}
		
		//��ʼ����
		while(SectorCount <= EndSector){
			FlashStatus = FLASH_EraseSector(SectorCount, VoltageRange_3);
			if (FlashStatus != FLASH_COMPLETE){
				//����FLASH
				FLASH_Lock();
				return FlashStatus;
			}
			
			//����������
			if (SectorCount == FLASH_Sector_11){
				SectorCount += 40;
			}else{
				SectorCount += 8;
			}
		}
	}
	
	//�ж��Ƿ��ֽڶ���
	AdrAddNumTemp = StartAddress % 4;
	if(AdrAddNumTemp != 0){
		StartAddress += (4 - AdrAddNumTemp);
	}
	
	
	//д������
	while(Len--){
		FlashStatus = FLASH_ProgramWord(StartAddress,*Data);
		StartAddress += 4;
		Data += 1;
		
		if (FlashStatus != FLASH_COMPLETE){
			//����FLASH
			FLASH_Lock();
			return FlashStatus;
		}
	}
	
	//����FLASH
	FLASH_Lock();
	
	if(AdrAddNumTemp != 0){
		*AdrAddNum += (4 - AdrAddNumTemp);
	}
	
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


/***
*��FLASH��ȡ���ݺ����������ֶ�ȡ
*������
	StartAddress��Ҫ��ȡ���ݵ�Flash��ַ
	Data����ȡ�������ݴ�������׵�ַ
	Len����ȡ��Ԫ���ݳ���
***/
void FLASH_ReadData_16(uint32_t StartAddress,uint16_t *Data,uint16_t Len)
{
	uint8_t AdrAddNumTemp = 0;
	
	//�ж��Ƿ��ֽڶ���
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
*��FLASH��ȡ���ݺ��������ֶ�ȡ
*������
	StartAddress��Ҫ��ȡ���ݵ�Flash��ַ
	Data����ȡ�������ݴ�������׵�ַ
	Len����ȡ��Ԫ���ݳ���
***/
void FLASH_ReadData_32(uint32_t StartAddress,uint32_t *Data,uint16_t Len)
{
	uint8_t AdrAddNumTemp = 0;
	
	//�ж��Ƿ��ֽڶ���
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
*�����������д������ע�⣬WRP�Ĵ����е�λΪ1ʱ��ʾ��Ӧ������û�п���д������λΪ0ʱ��ʾ������д������
*������
	WRPSectors:��Ҫ����д�����������д������������Ӧ��λ���ɵ�32λ��ֵ�����磬OB_WRP_Sector_0|OB_WRP_Sector_1
	State��ENABLEʱ��ʾ����д������DISABLEʱ��ʾ���д����
*����ֵ ��FLASH����״̬��־
***/
FLASH_Status FLASH_WriteProtect(uint32_t WRPSectors,FunctionalState State)
{
	FLASH_Status FlashStatus = FLASH_COMPLETE;
	uint16_t LastWRP = 0;
	
	//�ȶ��ص�ǰѡ�����е�д�������ã��������ڿ��ػ������ظ�ִ�д˲���������FLASH����
	LastWRP = FLASH_OB_GetWRP();
	
	//����WRP�Ĵ�����Ӧ��λΪ1��ʾδ����д������0��ʾ����д����������Ϊ�������߼�һ�£�����ȡ��
	LastWRP = ~LastWRP;
	
	//�жϿ������ǹر�����д����
	if(State == ENABLE){
		//�ж��Ƿ��Ѿ�����ָ��д�����ˣ��ǣ�����ִ�����������
		if((LastWRP & WRPSectors) == WRPSectors){
			return FlashStatus;
		}
		
		//����ѡ����
		FLASH_OB_Unlock();

		//����ָ������д����
		FLASH_OB_WRPConfig(WRPSectors, ENABLE); 

		//���Ķ�д��ѡ����
		FlashStatus = FLASH_OB_Launch();

		//����ѡ����
		FLASH_OB_Lock();
	}else{
		//�ж��Ƿ��Ѿ��ر�ָ��д�����ˣ��ǣ�����ִ�����������
		if((LastWRP & WRPSectors) == 0){
			return FlashStatus;
		}
		
		//����ѡ����
		FLASH_OB_Unlock();

		//�ر�ָ������д����
		FLASH_OB_WRPConfig(WRPSectors, DISABLE); 

		//���Ķ�д��ѡ����
		FlashStatus = FLASH_OB_Launch();

		//����ѡ����
		FLASH_OB_Lock();
	}
	
	return FlashStatus;
}


/***
*�����������������������1����ע�⣬���������FLASH�������˶���������Ҫ�ϵ�����
*������
	State:����/�رն�����
*����ֵ�����ش˴�FLASH�����Ľ��
***/
FLASH_Status FLASH_ReadProtect(FunctionalState State)
{
	FLASH_Status FlashState = FLASH_COMPLETE;
	uint8_t RDPAlreadyFlag = 0;
	
	//���ص�ǰ�Ƿ��Ѿ����ö���������ֹ�ظ�����
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
	
	//���FLASH��־
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	
	if(State == ENABLE){
		//����������
		FLASH_OB_RDPConfig(OB_RDP_Level_1);
		FlashState = FLASH_OB_Launch();
	}else{
		//�رն�����
		FLASH_OB_RDPConfig(OB_RDP_Level_0);
		FlashState = FLASH_OB_Launch();
	}
	
	//����ѡ����
	FLASH_OB_Lock();
	
	return FlashState;
}








