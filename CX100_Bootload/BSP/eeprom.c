/********************************************************************
*EEPROM��д������أ�EEPROMһ�㶼��I2C��ͨѶ��ͨѶ����Ϊ400khz������
24Cxxϵ������EEPROM
*********************************************************************/
#include "eeprom.h"
#include "i2cHardPolling.h"
#include "systick.h"




/*�ڲ�������********************************************************/
/***
*�ж�EEPROM�Ƿ��Ѿ�д�����
*������
	SlaveAdr��EEPROM�����ַ
*����ֵ������SUCCESSʱ����д����ϣ�ERRORΪ�������������޸��Ĵ���,EEPROM
��10ms�ھ���Ӧ��
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




/*�ⲿ������*********************************************************/
/***
*24Cxxд������
*������
	EEPROMType��EEPROM����
	SlaveAdr��EEPROM�����ַ
	Reg��Ҫ��ʼд�����ݵ���ʼ��ַ
	Buf��д������ݻ�������ʼ��ַ
	Len��һ��Ҫд������ݳ���
*����ֵ��HAL_OKΪ������ȡ������Ϊ�쳣
***/
ErrorStatus EEPROMWriteData(EEPROM_Type_e EEPROMType,uint8_t SlaveAdr,uint16_t Reg,uint8_t* Buf,uint16_t Len)
{
	uint16_t WriteBaseAdr = 0;			//���浱ǰд������ݵ����ҳ�׵Ļ���ַ
	uint16_t WritePageAdr = 0;			//���浱ǰд���ҳ��ַ
	uint8_t TempLen = 0;
	ErrorStatus ReState = SUCCESS;
	
	uint8_t PageBytes = 0;				//ÿҳ�ֽ���
	uint16_t TotalBytes = 0;			//���ֽ���
	uint16_t RegAdrTemp = 0;			//д�����ݵļĴ�����ַ
	
	
	//���ݲ�ͬEEPROM���ͼ����������ֵ
	switch(EEPROMType){
		case EEPROM_24C02:
			SlaveAdr = (SlaveAdr & 0xFE);
			PageBytes = 8;
			TotalBytes = 256;
		
			//�����ҳ��ַ�ͻ���ַ
			WriteBaseAdr = Reg & 0x0007;
			WritePageAdr = Reg & 0xFFF8;
		break;
		case EEPROM_24C04:
			SlaveAdr = (SlaveAdr & 0xFC);
			PageBytes = 16;
			TotalBytes = 512;
		
			//�����ҳ��ַ�ͻ���ַ
			WriteBaseAdr = Reg & 0x000F;
			WritePageAdr = Reg & 0xFFF0;
		break;
		case EEPROM_24C08:
			SlaveAdr = (SlaveAdr & 0xF8);
			PageBytes = 16;
			TotalBytes = 1024;
		
			//�����ҳ��ַ�ͻ���ַ
			WriteBaseAdr = Reg & 0x000F;
			WritePageAdr = Reg & 0xFFF0;
		break;
		case EEPROM_24C16:
			SlaveAdr = (SlaveAdr & 0xF0);
			PageBytes = 16;
			TotalBytes = 2048;
		
			//�����ҳ��ַ�ͻ���ַ
			WriteBaseAdr = Reg & 0x000F;
			WritePageAdr = Reg & 0xFFF0;
		break;
		case EEPROM_24C32:
			SlaveAdr = (SlaveAdr & 0xFE);
			PageBytes = 32;
			TotalBytes = 4096;
		
			//�����ҳ��ַ�ͻ���ַ
			WriteBaseAdr = Reg & 0x001F;
			WritePageAdr = Reg & 0xFFE0;
		break;
		case EEPROM_24C64:
			SlaveAdr = (SlaveAdr & 0xFE);
			PageBytes = 32;
			TotalBytes = 8192;
		
			//�����ҳ��ַ�ͻ���ַ
			WriteBaseAdr = Reg & 0x001F;
			WritePageAdr = Reg & 0xFFE0;
		break;
		
		default :break;
	}
	
	//���жϲ����Ƿ�Ϸ�
	if(Reg+Len > TotalBytes){
		return ERROR;
	}
	
	//��ʼд������
	while(Len){
		//����˴�д������ݵĳ���
		if(Len > (PageBytes - WriteBaseAdr)){
			TempLen = PageBytes - WriteBaseAdr;
		}else{
			TempLen = Len;
		}
		
		RegAdrTemp = WritePageAdr|WriteBaseAdr;
		
		//���ݲ�ͬEEPROM���ͼ���ӻ���ֵַ
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
		
		//�ж�EEPROMд���Ƿ����
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
		
		//�ٸ�һ�λ���д��
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
		
		//д��ɹ������¸�����ֵ
		WritePageAdr += PageBytes;
		WriteBaseAdr = 0;
		Buf += TempLen;
		Len -= TempLen;
	}
	
	//�ж�EEPROMд���Ƿ����
	ReState = _IsEEPROMReady(SlaveAdr);
	if(ReState != SUCCESS){
		return ReState;
	}
	
	return ReState;
}



/***
*24Cxx��ȡ����
*������
	EEPROMType��EEPROM����
	SlaveAdr��EEPROM�����ַ
	Reg��Ҫ��ʼ��ȡ���ݵ���ʼ��ַ
	Buf����Ŷ�ȡ�����ݻ�������ʼ��ַ
	Len��һ��Ҫ��ȡ�����ݳ���
*����ֵ��HAL_OKΪ������ȡ������Ϊ�쳣
***/
ErrorStatus EEPROMReadData(EEPROM_Type_e EEPROMType,uint8_t SlaveAdr,uint16_t Reg,uint8_t* Buf,uint16_t Len)
{
	uint16_t TotalBytes = 0;			//���ֽ���
	ErrorStatus ReturnStatus = SUCCESS;
	
	
	//���ݲ�ͬEEPROM���ͼ����������ֵ
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
	
	//���жϲ����Ƿ�Ϸ�
	if(Reg+Len > TotalBytes){
		return ERROR;
	}
	
	
	switch(EEPROMType){
		case EEPROM_24C32:
			//������Ҫ����һ�λ��ᣬ��ֹhal�⺯����Ӳ��I2C���¶�ȡʧ�ܣ����������ȡʧ��
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





