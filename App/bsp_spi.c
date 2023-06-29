/**************************************************************************
SPI FLASH(��W25Q256Ϊ��)�������̣�ʹ��SPI4
���飺ֻʹ��DMA�����ж�д���ݣ������ĵȴ�д����ɶ�ʹ����ͨ��ʽ
***************************************************************************/
#include "bsp_spi.h"
#include "main.h"
#include "bsp_outin.h"


/*�궨����******************************************************************/
#define FLASH_CS_HIGH									SPI_CS_Disable(SPI_CS_FLASH)  //FLASH_SPI_CS_GPIO_Port->BSRR |= FLASH_SPI_CS_Pin    //    
#define FLASH_CS_LOW									SPI_CS_Enable(SPI_CS_FLASH)   //FLASH_SPI_CS_GPIO_Port->BSRR |= (uint32_t)FLASH_SPI_CS_Pin<<16   


		
		
#define SPIFLASH_MAX_TIMEOUT							500000						//����SPI��дһ���ֽ����ĳ�ʱʱ��
#define SPIFLASH_WRITE_MAX_TIMEOUT						50							//����SPIFLASHдһҳ���ĳ�ʱʱ��
#define SPIFLASH_SECTION_ERASE_MAX_TIMEOUT				1000						//����SPIFLASH�����������ĳ�ʱʱ��
#define SPIFLASH_BLOCK_ERASE_MAX_TIMEOUT				5000						//����SPIFLASH��������ĳ�ʱʱ��
#define SPIFLASH_CHIP_ERASE_MAX_TIMEOUT					500000						//����SPIFLASH��Ƭ�������ĳ�ʱʱ��






//ָ����
#define SPIFLASH_CMD_READ_ID							0x9F						//���ڶ�ȡflash��IDֵ��ָ����
#define SPIFLASH_CMD_ENTER_4_ADR_MODE					0xB7						//���ڿ��ٸ���flashΪ4�ֽڵ�ַģʽ��ָ����
#define SPIFLASH_CMD_WRITE_ENABLE						0x06						//����flashдʹ�ܵ�ָ����
#define SPIFLASH_CMD_READ_STAT_REG1						0x05						//���ڶ�ȡflash��״̬�Ĵ���1��ָ����
#define SPIFLASH_CMD_READ_STAT_REG2						0x35						//���ڶ�ȡflash��״̬�Ĵ���2��ָ����
#define SPIFLASH_CMD_READ_STAT_REG3						0x15						//���ڶ�ȡflash��״̬�Ĵ���3��ָ����
#define SPIFLASH_CMD_WRITE_STAT_REG1					0x05						//����д��flash��״̬�Ĵ���1��ָ����
#define SPIFLASH_CMD_WRITE_STAT_REG2					0x35						//����д��flash��״̬�Ĵ���2��ָ����
#define SPIFLASH_CMD_WRITE_STAT_REG3					0x15						//����д��flash��״̬�Ĵ���3��ָ����
#define SPIFLASH_CMD_RESET_ENABLE						0x66						//����flash����ʹ�ܵ�ָ����
#define SPIFLASH_CMD_RESET								0x99						//����flash������ָ����
#define SPIFLASH_CMD_READ_DATA							0x0C						//����flash���ٶ�ȡ���ݵ�ָ����
#define SPIFLASH_CMD_SECTION_ERASE						0x21						//����flash����������ָ����
#define SPIFLASH_CMD_BLOCK_ERASE						0xDC						//����flash�������ָ����
#define SPIFLASH_CMD_CHIP_ERASE							0xC7						//����flash��Ƭ������ָ����
#define SPIFLASH_CMD_WRITE_DATA							0x12						//����flashд�����ݵ�ָ����

#define SPI_SPEED_2										0x0038						//&�ϴ�ֵ�ķ�ֵ����SPI�����ʸ���Ϊ2��Ƶ
#define SPI_SPEED_256									0x0038						//|�ϴ�ֵ����SPI�����ʸ���Ϊ256��Ƶ




/*ȫ�ֱ���������***********************************************************/
uint8_t InvalidValue = 0xFF;														//������Чֵ
__IO uint8_t SPIFlashDmaTCFlag = 0;													//���ڱ�ʾDMA���ͻ��߽����Ƿ���ɱ�־��Ϊ1��ʾ�Ѿ����
uint8_t SPI4NextModeFlag = SPI_MODE_END;											//���ڱ�ʾSPI4��һ��ͨѶ��Ҫ����ʲôģʽ״̬��Ϊ0��ʾ��Ҫ������XPT2046ͨѶ��״̬��Ϊ1��ʾ��Ҫ������SPI FLASHͨѶ״̬��Ϊ2��ʾ��Ҫ������AD���ֵ�λ��ͨѶ״̬
uint8_t SPI4ResourceNum = 1;														//���ڱ�ʾ��ǰSPI4�Ƿ�ռ���У�Ϊ0��ʾ���ڱ�ռ���У�Ϊ1��ʾû�б�ռ��
uint32_t SPI4ResurceUsed = 0;


/*�ڲ�������****************************************************************/
/***
*��дһ���ֽڣ�����д��һ���ֽڣ����߶���һ���ֽ�
*������
	Data��Ҫд������ݣ�����Ϊ�˶���һ���ֽڶ�ʹ�õ���Ч�ֽ���������SPI����ʱ��
*����ֵ����Ҫ���ص�һ���ֽڣ���������д��һ���ֽڴӶ��ӻ����ص���Ч����
***/
static uint8_t _SPI_RW(uint8_t Data)
{
	uint32_t Count = 0;
	
	//�ж�TXE�Ƿ�Ϊ1��������ָ������ݵ�����
	while((SPI4->SR & SPI_FLAG_TXE) == 0){
		Count++;
		if(Count>=SPIFLASH_MAX_TIMEOUT){
			return 0;
		}
	}
	SPI4->DR = Data;
	
	//�ж�RXNE�Ƿ�Ϊ1���Ӷ���֪�����Ƿ��Ѿ��������,��RXNEΪ1ʱ������һ�������շ�����ˣ����԰���Ҫ��NSS������
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
*��ȡJEDEC ID��������������ж�FLASH�Ƿ����������������ͺ��Ƿ�һ��
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
*����flashΪ4�ֽڵ�ַģʽ
***/
static void _SPIFlash_Set4AdrMode(void)
{
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_ENTER_4_ADR_MODE);
	
	FLASH_CS_HIGH;
}



/***
*дʹ��
***/
static void _SPIFlash_WriteEnable(void)
{
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_WRITE_ENABLE);
	
	FLASH_CS_HIGH;
}


///***
//*��ȡ״̬�Ĵ���1
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
//*��ȡ״̬�Ĵ���2
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
*��ȡ״̬�Ĵ���3
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
//*д��״̬�Ĵ���1
//***/
//static void _SPIFlash_WriteStatReg1(uint8_t RegValue)
//{
//	uint32_t Count = 0;
//	
//	//дʹ��
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
//	//�ж�д���Ƿ����,BUSYλ==0
//	while((_SPIFlash_ReadStatReg1() & 0x01) != 0){
//		Count++;
//		if(Count > SPIFLASH_MAX_TIMEOUT){
//			break;
//		}
//	}
//}


///***
//*д��״̬�Ĵ���2
//***/
//static void _SPIFlash_WriteStatReg2(uint8_t RegValue)
//{
//	uint32_t Count = 0;
//	
//	//дʹ��
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
//	//�ж�д���Ƿ����,BUSYλ==0
//	while((_SPIFlash_ReadStatReg1() & 0x01) != 0){
//		Count++;
//		if(Count > SPIFLASH_MAX_TIMEOUT){
//			break;
//		}
//	}
//}



///***
//*д��״̬�Ĵ���3
//***/
//static void _SPIFlash_WriteStatReg3(uint8_t RegValue)
//{
//	uint32_t Count = 0;
//	
//	//дʹ��
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
//	//�ж�д���Ƿ����,BUSYλ==0
//	while((_SPIFlash_ReadStatReg1() & 0x01) != 0){
//		Count++;
//		if(Count > SPIFLASH_MAX_TIMEOUT){
//			break;
//		}
//	}
//}



/***
*�ȴ�BUSYλΪ0,��DMA��ʽѭ����ȡ״̬�Ĵ���1��ֵ
***/
static ErrorStatus _SPIFlash_WaitBusy(uint32_t MaxTimeout)
{
	uint32_t Timestamp = HAL_GetTick();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_READ_STAT_REG1);
	
	//�ж�д���Ƿ����,BUSYλ==0
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
//*�ȴ�BUSYλΪ0,ʹ��DMA��ʽѭ����ȡ״̬�Ĵ���1��ֵ��������ʹ��
//***/
//static ErrorStatus _SPIFlash_DMA_WaitBusy(uint32_t MaxTimeout)
//{
//	uint8_t StatReg1 = 0xFF;
//	uint32_t Timestamp = Get_Stamp();
//	
//	//�����Ƚ�DMA���ر�
//	DMA2_Stream3->CR &= (~DMA_SxCR_EN);                 	//�ȹر�DMA2��������5
//	DMA2_Stream4->CR &= (~DMA_SxCR_EN);                 	//�ȹر�DMA2��������6
//	
//	FLASH_CS_LOW;
//	
//	_SPI_RW(SPIFLASH_CMD_READ_STAT_REG1);
//	
//	//�����־
//	SPIFlashDmaTCFlag = 0;
//	StatReg1 = _SPI_RW(0xFF);
//	
//	//�����ý�������DMA2��S5C7
//	while((DMA2_Stream3->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
//	DMA2->LIFCR |= 0x0F400000;								//������Ӧ���¼���־ȫ������
//	DMA2_Stream3->M0AR = (uint32_t)&StatReg1;				//�����ڴ��ַ
//	DMA2_Stream3->NDTR = 1;									//�������ý�Ҫ������ֽڳ���
//	DMA2_Stream3->CR &= ~DMA_SxCR_MINC;						//�ڴ��ַ�̶�ģʽ
//	DMA2_Stream3->CR |= DMA_SxCR_CIRC;						//ѭ��ģʽ
//	DMA2_Stream3->CR &= ~DMA_SxCR_TCIE;						//�ر�TC�ж�
//	DMA2_Stream3->CR |= DMA_SxCR_EN;						//����DMA2��������5
//	
//	//����Ϊ�˴���ʱ�ӵķ���DMA2��S6C7
//	while((DMA2_Stream4->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
//	DMA2->HIFCR |= 0x0000003D;								//������Ӧ���¼���־ȫ������
//	DMA2_Stream4->M0AR = (uint32_t)&InvalidValue;			//�����ڴ��ַ
//	DMA2_Stream4->NDTR = 1;									//�������ý�Ҫ������ֽڳ���
//	DMA2_Stream4->CR &= ~DMA_SxCR_MINC;						//�ڴ��ַ�̶�ģʽ
//	DMA2_Stream4->CR |= DMA_SxCR_CIRC;						//ѭ��ģʽ
//	DMA2_Stream4->CR &= ~DMA_SxCR_TCIE;						//�ر�TC�ж�
//	DMA2_Stream4->CR |= DMA_SxCR_EN;						//����DMA2��������6
//	
//	while((StatReg1 & 0x01) != 0){
//		if(Time_MS(Timestamp,MaxTimeout)){
//			DMA2_Stream4->CR &= ~DMA_SxCR_EN;				//�ر�DMA2��������6
//			DMA2_Stream3->CR &= ~DMA_SxCR_EN;				//�ر�DMA2��������5
//			
//			//����������ֱ���ж�ѭ��ģʽ��������Ҫ�ȴ�DMA�ر����
//			while(DMA2_Stream4->CR & DMA_SxCR_EN);
//			while(DMA2_Stream3->CR & DMA_SxCR_EN);
//			
//			FLASH_CS_HIGH;
//			
//			return ERROR;
//		}
//	}
//	
//	DMA2_Stream4->CR &= ~DMA_SxCR_EN;						//�ر�DMA2��������6
//	DMA2_Stream3->CR &= ~DMA_SxCR_EN;						//�ر�DMA2��������5
//	
//	//����������ֱ���ж�ѭ��ģʽ��������Ҫ�ȴ�DMA�ر����
//	while(DMA2_Stream4->CR & DMA_SxCR_EN);
//	while(DMA2_Stream3->CR & DMA_SxCR_EN);
//	
//	FLASH_CS_HIGH;
//	
//	return SUCCESS;
//}



/***
*д��һҳ���ݣ���DMA��ʽ��ע�⣬����ֻ��д�룬�����жϲ����Ƿ�Ϸ����Լ��Ƿ��
Խҳ��
***/
static void _SPIFlash_WritePageData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	//дʹ��
	_SPIFlash_WriteEnable();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_WRITE_DATA);
	
	//���͵�ַ
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
*д��һҳ���ݣ�DMA��ʽ��ע�⣬����ֻ��д�룬�����жϲ����Ƿ�Ϸ����Լ��Ƿ��
Խҳ��
***/
static void _SPIFlash_DMA_WritePageData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	osKernelLock();
	
	//дʹ��
	_SPIFlash_WriteEnable();
	
	//�����Ƚ�DMA���ر�
	DMA2_Stream3->CR &= (~DMA_SxCR_EN);                 	//�ȹر�DMA2��������5
	DMA2_Stream4->CR &= (~DMA_SxCR_EN);                 	//�ȹر�DMA2��������6
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_WRITE_DATA);
	
	//���͵�ַ
	_SPI_RW((Adr>>24));
	_SPI_RW((Adr>>16));
	_SPI_RW((Adr>>8));
	_SPI_RW((Adr & 0x000000FF));
	
	//�����־
	SPIFlashDmaTCFlag = 0;
	
	//����ȷ����SPI��RXNEλ���㣬��ֹ����һ����DMA����ͨ���Ͱ�֮ǰ�����ݽ��ս����ˣ��������ݴ�λ
	SPI4->DR;
	
	//�����ý�������DMA2��S3C5
	while((DMA2_Stream3->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
	DMA2->LIFCR |= 0x0F400000;								//������Ӧ���¼���־ȫ������
	DMA2_Stream3->CR |= DMA_IT_TC;							//����TC�ж�
	DMA2_Stream3->M0AR = (uint32_t)&InvalidValue;			//�����ڴ��ַ
	DMA2_Stream3->NDTR = Len;								//�������ý�Ҫ������ֽڳ���
	DMA2_Stream3->CR &= ~DMA_SxCR_MINC;						//�ڴ��ַ�̶�ģʽ
	DMA2_Stream3->CR |= DMA_SxCR_EN;						//����DMA2��������5
	
	//����Ϊ�˴���ʱ�ӵķ���DMA2��S4C5
	while((DMA2_Stream4->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
	DMA2->HIFCR |= 0x0000003D;								//������Ӧ���¼���־ȫ������
	DMA2_Stream4->CR &= ~DMA_IT_TC;							//�ر�TC�ж�
	DMA2_Stream4->M0AR = (uint32_t)Buf;						//�����ڴ��ַ
	DMA2_Stream4->NDTR = Len;								//�������ý�Ҫ������ֽڳ���
	DMA2_Stream4->CR |= DMA_SxCR_MINC;						//�ڴ��ַ����ģʽ
	DMA2_Stream4->CR |= DMA_SxCR_EN;						//����DMA2��������6
	
	//�ȴ��������
	while(SPIFlashDmaTCFlag == 0);
	
	FLASH_CS_HIGH;
	
	osKernelUnlock();
}



 
//HAL_StatusTypeDef

/*
*   �л�SPI4��ģʽ��
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
    
    //����SPI����
	hspi4.Instance->CR1 &= ~SPI_CR1_SPE;
    switch(eMode)
    {
        case SPI_MODE_TP:
        case SPI_MODE_DR:
        {
        	//��Ƶ��Ϊ256
            hspi4.Instance->CR1 |= SPI_SPEED_256;
            //����SPI
            hspi4.Instance->CR1 |= SPI_CR1_SPE;
        }
        break;
        case SPI_MODE_FLASH:
        {
            //��Ƶ��Ϊ4
            hspi4.Instance->CR1 &= ~SPI_SPEED_2;
            hspi4.Instance->CR1 |= 0x0008;
            //����SPI
            hspi4.Instance->CR1 |= SPI_CR1_SPE;
        }
        break;
        default:break;  
    }
    SPI4NextModeFlag = eMode;
    
    return eMode;
}


/*
*   �л�SPI4��ģʽ��Ĭ��TPģʽ��
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
    
    //Ĭ�ϴ��ڴ���ģʽ��
    SPI4NextModeFlag = SPI_MODE_TP;
    
    //�ȹر�SPI 
    hspi4.Instance->CR1 &= ~SPI_CR1_SPE;
    //��Ƶ��Ϊ256
    hspi4.Instance->CR1 |= SPI_SPEED_256;
    //����SPI
    hspi4.Instance->CR1 |= SPI_CR1_SPE;
    
    osKernelUnlock();
}



/*�ⲿ������****************************************************************/
/***
*�л�SPI4ģʽ��SPI FLASH����ͨѶ����Ƶ2��ģʽ��0��0
***/
void SPI4_ChangeModeToFlash(void)
{
	extern SPI_HandleTypeDef hspi4;
	extern uint8_t SPI4NextModeFlag;
	extern uint8_t SPI4ResourceNum;
	
	//���ж�SPI4�Ƿ��Ѿ����ڿ���
	while(SPI4ResourceNum == 0){
		SPI4NextModeFlag = SPI_MODE_FLASH;
		osDelay(2);
	}
	
	//��FLASH��ȡSPI4����Ȩ
	SPI4ResourceNum = 0;
	

}


/***
*�л�SPI4ģʽ�����ֵ�λ������ͨѶ����Ƶ256��ģʽ��0��0
***/
void SPI4_ChangeModeToDR(void)
{
	extern SPI_HandleTypeDef hspi4;
	extern uint8_t SPI4NextModeFlag;
	extern uint8_t SPI4ResourceNum;
	
	//���ж�SPI4�Ƿ��Ѿ����ڿ���
	while(SPI4ResourceNum == 0){
		SPI4NextModeFlag = SPI_MODE_DR;
		osDelay(2);
	}
	
	//��FLASH��ȡSPI4����Ȩ
	SPI4ResourceNum = 0;
	
	//�ȹر�SPI 
	hspi4.Instance->CR1 &= ~SPI_CR1_SPE;
	
	//��Ƶ��Ϊ256
	hspi4.Instance->CR1 |= SPI_SPEED_256;
	
	//����SPI
	hspi4.Instance->CR1 |= SPI_CR1_SPE;
}




/***
*�л�SPI4��ԴΪ����״̬��һ��FLASH��AD���ֵ�λ���������Ҫ�л�Ϊ����״̬���������������Ҫ�л�Ϊ����״̬
***/
void SPI4_ChangeModeToIDLE(void)
{
	extern uint8_t SPI4NextModeFlag;
	extern uint8_t SPI4ResourceNum;
	
	SPI4NextModeFlag = SPI_MODE_END;
	SPI4ResourceNum = 1;
}



/***
*�л�SPI4ģʽ�����������ͨѶ����Ƶ256��ģʽ��0��0
***/
HAL_StatusTypeDef SPI4_ChangeModeToXPT2046(void)
{
	extern SPI_HandleTypeDef hspi4;
	extern uint8_t SPI4NextModeFlag;
	extern uint8_t SPI4ResourceNum;
	
	//�ж�SPI4�Ƿ��Ѿ���ռ����
	if(SPI4ResourceNum == 0){
		return HAL_ERROR;
	}
	
	//�ж���һ��SPI4�Ƿ�Ҫ��FLASH�������ֵ�λ��ռ�ã��ǣ�������ͣʹ��
	if(SPI4NextModeFlag == SPI_MODE_FLASH || SPI4NextModeFlag == SPI_MODE_DR){
		return HAL_ERROR;
	}
	
	//�жϵ�ǰSPI4�Ƿ��Ѿ�������ռ���У��ǣ���ֱ�ӷ��سɹ�����
	if(SPI4NextModeFlag == SPI_MODE_TP){
		SPI4ResourceNum = 0;
		return HAL_OK;
	}
	
	//����ռ��SPI4
	SPI4ResourceNum = 0;
	SPI4NextModeFlag = SPI_MODE_TP;
	
	//�ȹر�SPI 
	hspi4.Instance->CR1 &= ~SPI_CR1_SPE;
	
	//��Ƶ��Ϊ256
	hspi4.Instance->CR1 |= SPI_SPEED_256;
	
	//����SPI
	hspi4.Instance->CR1 |= SPI_CR1_SPE;

	return HAL_OK;
}





/***
*��ʼ��SPI FLASH
***/
ErrorStatus SPIFlash_Init(void)
{
    uint8_t ucNum = 3;
    
	//����SPIģʽ
	SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
	
	//��ȡIDֵ
    do{
        if(_SPIFlash_ReadId() != SPIFLASH_JEDEC_ID){
            //����SPIģʽ, Ϊ����ģʽ
            LOG_Error("SPI Flash Conn Fail");
            //return ERROR;
        }    
    }while(ucNum--);
    
	
	//��ȡ״̬�Ĵ���3��ֵ���ж��Ƿ��Ѿ�����4�ֽڵ�ַģʽ
	if((_SPIFlash_ReadStatReg3() & 0x01) == 0){
		//����Ϊ4�ֽڵ�ַģʽ
		_SPIFlash_Set4AdrMode();
	}
	
	//����SPIģʽ
	SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
	
	return SUCCESS;
}



/***
*�����ж�SPI FLASH�Ƿ��Ѿ�׼�����
***/
ErrorStatus SPIFlash_IsReady(void)
{
	//��ȡIDֵ
	if(_SPIFlash_ReadId() != SPIFLASH_JEDEC_ID){
		return ERROR;
	}else{
		return SUCCESS;
	}
}



/***
*����FLASH
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
*��ȡ���ݣ���DMA��ȡģʽ
***/
ErrorStatus SPIFlash_ReadData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	if(Len == 0 || Adr > SPIFLASH_VOLUME_SIZE){
		return ERROR;
	}
	
	//���ֻ�ܶ�ȡ��оƬ��β������������»ص���ͷ����ȡ
	if(Adr+Len > SPIFLASH_VOLUME_SIZE){
		Len = SPIFLASH_VOLUME_SIZE - Adr;
	}
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_READ_DATA);
	
	//���͵�ַ
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
*��ȡ���ݣ�DMA��ʽ��ȡģʽ
***/
ErrorStatus SPIFlash_DMA_ReadData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	extern __IO uint8_t SPIFlashDmaTCFlag;
	uint16_t TempLen = 0;
	InvalidValue = 0xff;
	
	if(Len == 0 || Adr > SPIFLASH_VOLUME_SIZE){
		return ERROR;
	}
	
	//���ֻ�ܶ�ȡ��оƬ��β������������»ص���ͷ����ȡ
	if(Adr+Len > SPIFLASH_VOLUME_SIZE){
		return ERROR;
	}
	
	osKernelLock();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_READ_DATA);
	
	//���͵�ַ
	_SPI_RW((Adr>>24));
	_SPI_RW((Adr>>16));
	_SPI_RW((Adr>>8));
	_SPI_RW((Adr & 0x000000FF));
	
	_SPI_RW(InvalidValue);
	
	//����ȷ����SPI��RXNEλ���㣬��ֹ����һ����DMA����ͨ���Ͱ�֮ǰ�����ݽ��ս����ˣ��������ݴ�λ
	SPI4->DR;
	
	while(Len){
		//�����־
		SPIFlashDmaTCFlag = 0;
		
		//�����Ƚ�DMA���ر�
		DMA2_Stream3->CR &= (~DMA_SxCR_EN);                 	//�ȹر�DMA2��������5
		DMA2_Stream4->CR &= (~DMA_SxCR_EN);                 	//�ȹر�DMA2��������6
	
		if(Len > 65535){
			TempLen = 65535;
		}else{
			TempLen = Len;
		}
		Len -= TempLen;
		
		//�����ý�������DMA2��S3C5
		while((DMA2_Stream3->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
		DMA2->LIFCR |= 0x0F400000;								//������Ӧ���¼���־ȫ������
		DMA2_Stream3->CR |= DMA_IT_TC;							//����TC�ж�
		DMA2_Stream3->M0AR = (uint32_t)Buf;						//�����ڴ��ַ
		DMA2_Stream3->NDTR = TempLen;								//�������ý�Ҫ������ֽڳ���
		DMA2_Stream3->CR |= DMA_SxCR_MINC;						//�ڴ��ַ����ģʽ
		DMA2_Stream3->CR |= DMA_SxCR_EN;						//����DMA2��������5
		
		//����Ϊ�˴���ʱ�ӵķ���DMA2��S4C5
		while((DMA2_Stream4->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
		DMA2->HIFCR |= 0x0000003D;								//������Ӧ���¼���־ȫ������
		DMA2_Stream4->CR &= ~DMA_IT_TC;							//�ر�TC�ж�
		DMA2_Stream4->M0AR = (uint32_t)&InvalidValue;			//�����ڴ��ַ
		DMA2_Stream4->NDTR = TempLen;								//�������ý�Ҫ������ֽڳ���
		DMA2_Stream4->CR &= ~DMA_SxCR_MINC;						//�ڴ��ַ�̶�ģʽ
		DMA2_Stream4->CR |= DMA_SxCR_EN;						//����DMA2��������6
		
		//�ȴ��������
		while(SPIFlashDmaTCFlag == 0);
		
		//����Buf��ַ
		Buf += TempLen;
	}
	
	FLASH_CS_HIGH;
	
	osKernelUnlock();
	
	return SUCCESS;
}



/***
*����Ϊ��λ��ȡ���ݣ���FatFs�ļ�ϵͳʹ��
***/
ErrorStatus SPIFlash_DMA_SectorReadData(uint8_t* Buf,uint32_t SectorAdr,uint32_t SectorNum)
{
	return SPIFlash_DMA_ReadData(SectorAdr*SPIFLASH_SECTION_SIZE,Buf,SectorNum*SPIFLASH_SECTION_SIZE);
}






/***
*������������ַ��Ҫ����4KB�߽�
***/
void SPIFlash_EraseSection(uint32_t Adr)
{
	//�Ƚ���ַ���뵽4KB�߽�
	Adr -= (Adr % (SPIFLASH_SECTION_SIZE));
	
	//дʹ��
	_SPIFlash_WriteEnable();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_SECTION_ERASE);
	
	//���͵�ַ
	_SPI_RW((Adr>>24));
	_SPI_RW((Adr>>16));
	_SPI_RW((Adr>>8));
	_SPI_RW((Adr & 0x000000FF));
	
	FLASH_CS_HIGH;
	
	//�ȴ�ִ�����
	_SPIFlash_WaitBusy(SPIFLASH_SECTION_ERASE_MAX_TIMEOUT);
}



/***
*���������ַ��Ҫ����64KB�߽�
***/
void SPIFlash_EraseBlock(uint32_t Adr)
{
	//�Ƚ���ַ���뵽64KB�߽�
	Adr -= (Adr % SPIFLASH_BLOCK_SIZE);
	
	//дʹ��
	_SPIFlash_WriteEnable();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_BLOCK_ERASE);
	
	//���͵�ַ
	_SPI_RW((Adr>>24));
	_SPI_RW((Adr>>16));
	_SPI_RW((Adr>>8));
	_SPI_RW((Adr & 0x000000FF));
	
	FLASH_CS_HIGH;
	
	//�ȴ�ִ�����
	_SPIFlash_WaitBusy(SPIFLASH_BLOCK_ERASE_MAX_TIMEOUT);
}



/***
*��Ƭ����
***/
void SPIFlash_EraseChip(void)
{
	//дʹ��
	_SPIFlash_WriteEnable();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_CHIP_ERASE);
	
	FLASH_CS_HIGH;
	
	//�ȴ�ִ�����
	_SPIFlash_WaitBusy(SPIFLASH_CHIP_ERASE_MAX_TIMEOUT);
}




/***
*д�����ݣ�DMA��ʽд�롣
*ע�ⲻ�ܿ�ҳд�룬ÿҳ��СΪ256�ֽڣ�����ע�⣬����ַΪһ���������׵�ַʱ��
���Ȳ�����������д�����ݣ�����ַ�����������׵�ַʱ����Ĭ�ϲ��ò���ֱ��д�����ݼ���
***/
ErrorStatus SPIFlash_DMA_WriteData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	uint16_t SectionNum = 0;
	uint32_t Temp = 0;
	
	//���жϲ����Ƿ�Ϸ�
	if(Len == 0 || Adr > SPIFLASH_VOLUME_SIZE){
		return ERROR;
	}
	
	if(Adr + Len > SPIFLASH_VOLUME_SIZE){
		Len = SPIFLASH_VOLUME_SIZE - Adr;
	}
	
	//��ǰ����Ҫд�����ݵ���������
	if((Adr % SPIFLASH_SECTION_SIZE) == 0){				//д����ʼ��ַ�պö�������
		//����һ����Ҫ����������д�룬�Ա���ǰ������������
		SectionNum = Len / SPIFLASH_SECTION_SIZE;
		SectionNum += (Len % SPIFLASH_SECTION_SIZE)==0?0:1;
		
		Temp = Adr;
		
		//��������
		while(SectionNum--){
			SPIFlash_EraseSection(Temp);
			Temp += SPIFLASH_SECTION_SIZE;
		}
	}else{												//д����ʼ��ַû�ж�������
		Temp = Adr % SPIFLASH_SECTION_SIZE;
		if(Temp+Len > SPIFLASH_SECTION_SIZE){
			Temp = Temp+Len - SPIFLASH_SECTION_SIZE;
			
			SectionNum = Temp / SPIFLASH_SECTION_SIZE;
			SectionNum += (Temp % SPIFLASH_SECTION_SIZE)==0?0:1;
			
			Temp = ((Adr/SPIFLASH_SECTION_SIZE)+1)*SPIFLASH_SECTION_SIZE;
			
			//��������
			while(SectionNum--){
				SPIFlash_EraseSection(Temp);
				Temp += SPIFLASH_SECTION_SIZE;
			}
		}
	}
	
	//��ʼд������
	while(Len){
		if((Adr % SPIFLASH_PAGE_SIZE) == 0){	//д���ַ����ҳ��
			if(Len > SPIFLASH_PAGE_SIZE){				//ʣ�����ݴ���һҳ
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);					//�ȴ���һ��д�����
				_SPIFlash_DMA_WritePageData(Adr,Buf,SPIFLASH_PAGE_SIZE);
				Adr += SPIFLASH_PAGE_SIZE;
				Buf += SPIFLASH_PAGE_SIZE;
				Len -= SPIFLASH_PAGE_SIZE;
			}else{								//ʣ������С�ڵ���һҳ
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);					//�ȴ���һ��д�����
				_SPIFlash_DMA_WritePageData(Adr,Buf,Len);
				Adr += Len;
				Buf += Len;
				Len = 0;
			}
		}else{											//д���ַû�ж���ҳ��
			Temp = Adr % SPIFLASH_PAGE_SIZE;
			Temp = SPIFLASH_PAGE_SIZE - Temp;
			
			if(Len > Temp){
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);						//�ȴ���һ��д�����
				_SPIFlash_DMA_WritePageData(Adr,Buf,Temp);
				Adr += Temp;
				Buf += Temp;
				Len -= Temp;
			}else{
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);						//�ȴ���һ��д�����
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
*д�����ݣ���DMA��ʽд�롣
*ע�ⲻ�ܿ�ҳд�룬ÿҳ��СΪ256�ֽڣ�����ע�⣬����ַΪһ���������׵�ַʱ��
���Ȳ�����������д�����ݣ�����ַ�����������׵�ַʱ����Ĭ�ϲ��ò���ֱ��д�����ݼ���
***/
ErrorStatus SPIFlash_WriteData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	uint16_t SectionNum = 0;
	uint32_t Temp = 0;
	
	//���жϲ����Ƿ�Ϸ�
	if(Len == 0){
		return ERROR;
	}
	
	if(Adr + Len > SPIFLASH_VOLUME_SIZE){
		Len = SPIFLASH_VOLUME_SIZE - Adr;
	}
	
	//��ǰ����Ҫд�����ݵ���������
	if((Adr % SPIFLASH_SECTION_SIZE) == 0){				//д����ʼ��ַ�պö�������
		//����һ����Ҫ����������д�룬�Ա���ǰ������������
		SectionNum = Len / SPIFLASH_SECTION_SIZE;
		SectionNum += (Len % SPIFLASH_SECTION_SIZE)==0?0:1;
		
		Temp = Adr;
		
		//��������
		while(SectionNum--){
			SPIFlash_EraseSection(Temp);
			Temp += SPIFLASH_SECTION_SIZE;
		}
	}else{												//д����ʼ��ַû�ж�������
		Temp = Adr % SPIFLASH_SECTION_SIZE;
		if(Temp+Len > SPIFLASH_SECTION_SIZE){
			Temp = Temp+Len - SPIFLASH_SECTION_SIZE;
			
			SectionNum = Temp / SPIFLASH_SECTION_SIZE;
			SectionNum += (Temp % SPIFLASH_SECTION_SIZE)==0?0:1;
			
			Temp = ((Adr/SPIFLASH_SECTION_SIZE)+1)*SPIFLASH_SECTION_SIZE;
			
			//��������
			while(SectionNum--){
				SPIFlash_EraseSection(Temp);
				Temp += SPIFLASH_SECTION_SIZE;
			}
		}
	}
	
	//��ʼд������
	while(Len){
		if((Adr % SPIFLASH_PAGE_SIZE) == 0){	//д���ַ����ҳ��
			if(Len > SPIFLASH_PAGE_SIZE){				//ʣ�����ݴ���һҳ
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);					//�ȴ���һ��д�����
				_SPIFlash_WritePageData(Adr,Buf,SPIFLASH_PAGE_SIZE);
				Adr += SPIFLASH_PAGE_SIZE;
				Buf += SPIFLASH_PAGE_SIZE;
				Len -= SPIFLASH_PAGE_SIZE;
			}else{								//ʣ������С�ڵ���һҳ
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);					//�ȴ���һ��д�����
				_SPIFlash_WritePageData(Adr,Buf,Len);
				Adr += Len;
				Buf += Len;
				Len = 0;
			}
		}else{											//д���ַû�ж���ҳ��
			Temp = Adr % SPIFLASH_PAGE_SIZE;
			Temp = SPIFLASH_PAGE_SIZE - Temp;
			
			if(Len > Temp){
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);						//�ȴ���һ��д�����
				_SPIFlash_WritePageData(Adr,Buf,Temp);
				Adr += Temp;
				Buf += Temp;
				Len -= Temp;
			}else{
				_SPIFlash_WaitBusy(SPIFLASH_WRITE_MAX_TIMEOUT);						//�ȴ���һ��д�����
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
*����Ϊ��λд�����ݣ���FatFSд������ʹ��
***/
ErrorStatus SPIFlash_SectorWriteData(uint8_t* Buf,uint32_t SectorAdr,uint32_t SectorNum)
{
	return SPIFlash_WriteData(SectorAdr*SPIFLASH_SECTION_SIZE,Buf,SectorNum*SPIFLASH_SECTION_SIZE);
}
























