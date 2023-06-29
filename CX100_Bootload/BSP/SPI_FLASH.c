/**************************************************************************
SPI FLASH(��W25Q256Ϊ��)�������̣�ʹ��SPI4,PF7~PF9��CS-->PF6
���飺ֻʹ��DMA�����ж�д���ݣ������ĵȴ�д����ɶ�ʹ����ͨ��ʽ
***************************************************************************/
#include "SPI_FLASH.h"
#include "systick.h"





/*�궨����******************************************************************/
#define FLASH_CS_HIGH									GPIOB->BSRRL |= GPIO_Pin_15
#define FLASH_CS_LOW									GPIOB->BSRRH |= GPIO_Pin_15
		
		
#define SPIFLASH_MAX_TIMEOUT							500000						//����SPI��дһ���ֽ����ĳ�ʱʱ��
#define SPIFLASH_WRITE_MAX_TIMEOUT						50							//����SPIFLASHдһҳ���ĳ�ʱʱ��
#define SPIFLASH_SECTION_ERASE_MAX_TIMEOUT				1000						//����SPIFLASH�����������ĳ�ʱʱ��
#define SPIFLASH_BLOCK_ERASE_MAX_TIMEOUT				5000						//����SPIFLASH��������ĳ�ʱʱ��
#define SPIFLASH_CHIP_ERASE_MAX_TIMEOUT					500000						//����SPIFLASH��Ƭ�������ĳ�ʱʱ��

#define SPIFLASH_JEDEC_ID								0x00EF4019					//����W25Q256��ID������ǹ̶�ֵ����ͬ�ͺŲ�һ��




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



/*ȫ�ֱ���������***********************************************************/
uint8_t InvalidValue = 0xFF;					//������Чֵ
uint8_t SPIFlashDmaTCFlag = 0;					//���ڱ�ʾDMA���ͻ��߽����Ƿ���ɱ�־��Ϊ1��ʾ�Ѿ����




/*�ڲ�������****************************************************************/
/***
*��������spi����TX��Ӧ��DMA2������4ͨ��5
***/
static void _SPI_Tx_Dma2S4C5_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	//ʹ��DMAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
//	DMA_InitStructure.DMA_BufferSize = 10;	            										//ÿ�δ�������ݵĸ�����������ʱ�����ж�
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t);   										//�ڴ��ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI4->DR;                 			//�����ַ
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;												//ѡ��ͨ����һ��������ֻ�����ø�һ��ͨ������
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                         			//���䷽��
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                			//�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                         			//�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     				//����ÿ�δ�����ֽڳ���
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             				//�ڴ�ÿ�δ�����ֽڳ���
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                 				//��ͨģʽ����ѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                             			//��DMA��������ȼ�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                          			//������FIFOģʽ��ѡ��ֱ�Ӵ�������ģʽ
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;           			//FIFO��������ֵ��1/4��1/2,3/4,1/1
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                     			//�ڴ��ǵ��δ����ͻ�����䣬ͻ�������Ϊ��4,8,16����������
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;             			//�����ǵ��δ��仹��ͻ������
	DMA_Init(DMA2_Stream4, &DMA_InitStructure);			
				
	NVIC_EnableIRQ(DMA2_Stream4_IRQn);															//��NVIC�ж�Ӧ��DMA������4���ж�ͨ��
	DMA_ITConfig(DMA2_Stream4, DMA_IT_TC, DISABLE);												//��Ҫʱ�ٴ�DMA������4�����ݴ�������ж�
	
//	//����DMA2��������4����Ҫ������ʱ���ٿ���
//	DMA_Cmd(DMA2_Stream4, ENABLE);
}


/***
*��������spi����RX��Ӧ��DMA2������3ͨ��5
***/
static void _SPI_Rx_Dma2S3C5_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	//ʹ��DMAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
//	DMA_InitStructure.DMA_BufferSize = 10;	            										//ÿ�δ�������ݵĸ�����������ʱ�����ж�
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t);   										//�ڴ��ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI4->DR;                 			//�����ַ
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;												//ѡ��ͨ����һ��������ֻ�����ø�һ��ͨ������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                         			//���䷽��
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                			//�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                         			//�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     				//����ÿ�δ�����ֽڳ���
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             				//�ڴ�ÿ�δ�����ֽڳ���
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                 				//��ͨģʽ����ѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                             			//��DMA��������ȼ�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                          			//������FIFOģʽ��ѡ��ֱ�Ӵ�������ģʽ
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;           			//FIFO��������ֵ��1/4��1/2,3/4,1/1
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                     			//�ڴ��ǵ��δ����ͻ�����䣬ͻ�������Ϊ��4,8,16����������
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;             			//�����ǵ��δ��仹��ͻ������
	DMA_Init(DMA2_Stream3, &DMA_InitStructure);			
				
	NVIC_EnableIRQ(DMA2_Stream3_IRQn);															//��NVIC�ж�Ӧ��DMA������3���ж�ͨ��
	DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, DISABLE);												//��Ҫʱ�ٴ�DMA������3�����ݴ�������ж�
	
//	//����DMA2��������3����Ҫ������ʱ���ٿ���
//	DMA_Cmd(DMA2_Stream3, ENABLE);
}





/***
*����SPI��MISO,MOSI,SCK����ΪAF_PP���ɣ�����CS��ΪOUT_PPģʽ
***/
static void _SPI_Pin_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource2, GPIO_AF_SPI4);		
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_SPI4);   	
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_SPI4);   	
    
	//����SPI��SCK��MISO��MOSI����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	//����������ƴӻ�CS����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//���ÿ��ƴӻ�1��NSS������
	FLASH_CS_HIGH;
	
}


/***
*����SPI
***/
static void _SPI_Config(void)
{
	SPI_InitTypeDef   SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4,ENABLE);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						//SPIΪ����ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					//��������ݳ���Ϊ8λ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;							//SPI���߿���ʱ��sck��Ϊ�͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						//��SCK���������ز�������
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							//������NSS�������ģʽ
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;	//ʱ�ӷ�Ƶֵ
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					//���ݸ�λ����
	SPI_InitStructure.SPI_CRCPolynomial = 7;							//CRCУ�����ʽ���ã����ݴ�������һ���ֽڳ����ϴ˴���7�������CRCУ��
	SPI_Init(SPI4, &SPI_InitStructure);
	
	//����SPI��DMA����
	SPI_I2S_DMACmd(SPI4,SPI_I2S_DMAReq_Tx,ENABLE);
	SPI_I2S_DMACmd(SPI4,SPI_I2S_DMAReq_Rx,ENABLE);
	
	SPI_Cmd(SPI4, ENABLE);
}




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
	while((SPI4->SR & SPI_I2S_FLAG_TXE) == 0){
		Count++;
		if(Count>=SPIFLASH_MAX_TIMEOUT){
			return 0;
		}
	}
	SPI4->DR = Data;
	
	//�ж�RXNE�Ƿ�Ϊ1���Ӷ���֪�����Ƿ��Ѿ��������,��RXNEΪ1ʱ������һ�������շ�����ˣ����԰���Ҫ��NSS������
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
*��ȡJEDEC ID��������������ж�FLASH�Ƿ����������������ͺ��Ƿ�һ��
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
	uint32_t Timestamp = Get_Stamp();
	
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_READ_STAT_REG1);
	
	//�ж�д���Ƿ����,BUSYλ==0
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
//*�ȴ�BUSYλΪ0,ʹ��DMA��ʽѭ����ȡ״̬�Ĵ���1��ֵ��������ʹ��
//***/
//static ErrorStatus _SPIFlash_DMA_WaitBusy(uint32_t MaxTimeout)
//{
//	uint8_t StatReg1 = 0xFF;
//	uint32_t Timestamp = Get_Stamp();
//	
//	//�����Ƚ�DMA���ر�
//	DMA2_Stream5->CR &= (~DMA_SxCR_EN);                 	//�ȹر�DMA2��������5
//	DMA2_Stream6->CR &= (~DMA_SxCR_EN);                 	//�ȹر�DMA2��������6
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
//	while((DMA2_Stream5->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
//	DMA2->HIFCR |= 0x00000f40;								//������Ӧ���¼���־ȫ������
//	DMA2_Stream5->M0AR = (uint32_t)&StatReg1;				//�����ڴ��ַ
//	DMA2_Stream5->NDTR = 1;									//�������ý�Ҫ������ֽڳ���
//	DMA2_Stream5->CR &= ~DMA_SxCR_MINC;						//�ڴ��ַ�̶�ģʽ
//	DMA2_Stream5->CR |= DMA_SxCR_CIRC;						//ѭ��ģʽ
//	DMA2_Stream5->CR &= ~DMA_SxCR_TCIE;						//�ر�TC�ж�
//	DMA2_Stream5->CR |= DMA_SxCR_EN;						//����DMA2��������5
//	
//	//����Ϊ�˴���ʱ�ӵķ���DMA2��S6C7
//	while((DMA2_Stream6->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
//	DMA2->HIFCR |= 0x003D0000;								//������Ӧ���¼���־ȫ������
//	DMA2_Stream6->M0AR = (uint32_t)&InvalidValue;			//�����ڴ��ַ
//	DMA2_Stream6->NDTR = 1;									//�������ý�Ҫ������ֽڳ���
//	DMA2_Stream6->CR &= ~DMA_SxCR_MINC;						//�ڴ��ַ�̶�ģʽ
//	DMA2_Stream6->CR |= DMA_SxCR_CIRC;						//ѭ��ģʽ
//	DMA2_Stream6->CR &= ~DMA_SxCR_TCIE;						//�ر�TC�ж�
//	DMA2_Stream6->CR |= DMA_SxCR_EN;						//����DMA2��������6
//	
//	while((StatReg1 & 0x01) != 0){
//		if(Time_MS(Timestamp,MaxTimeout)){
//			DMA2_Stream6->CR &= ~DMA_SxCR_EN;				//�ر�DMA2��������6
//			DMA2_Stream5->CR &= ~DMA_SxCR_EN;				//�ر�DMA2��������5
//			
//			//����������ֱ���ж�ѭ��ģʽ��������Ҫ�ȴ�DMA�ر����
//			while(DMA2_Stream6->CR & DMA_SxCR_EN);
//			while(DMA2_Stream5->CR & DMA_SxCR_EN);
//			
//			FLASH_CS_HIGH;
//			
//			return ERROR;
//		}
//	}
//	
//	DMA2_Stream6->CR &= ~DMA_SxCR_EN;						//�ر�DMA2��������6
//	DMA2_Stream5->CR &= ~DMA_SxCR_EN;						//�ر�DMA2��������5
//	
//	//����������ֱ���ж�ѭ��ģʽ��������Ҫ�ȴ�DMA�ر����
//	while(DMA2_Stream6->CR & DMA_SxCR_EN);
//	while(DMA2_Stream5->CR & DMA_SxCR_EN);
//	
//	FLASH_CS_HIGH;
//	
//	return SUCCESS;
//}



///***
//*д��һҳ���ݣ���DMA��ʽ��ע�⣬����ֻ��д�룬�����жϲ����Ƿ�Ϸ����Լ��Ƿ��
//Խҳ��
//***/
//static void _SPIFlash_WritePageData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
//{
//	//дʹ��
//	_SPIFlash_WriteEnable();
//	
//	FLASH_CS_LOW;
//	
//	_SPI_RW(SPIFLASH_CMD_WRITE_DATA);
//	
//	//���͵�ַ
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
*д��һҳ���ݣ�DMA��ʽ��ע�⣬����ֻ��д�룬�����жϲ����Ƿ�Ϸ����Լ��Ƿ��
Խҳ��
***/
static void _SPIFlash_DMA_WritePageData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	//дʹ��
	_SPIFlash_WriteEnable();
	
	//�����Ƚ�DMA���ر�
	DMA2_Stream3->CR &= (~DMA_SxCR_EN);                 	//�ȹر�DMA2��������3
	DMA2_Stream4->CR &= (~DMA_SxCR_EN);                 	//�ȹر�DMA2��������4
	
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
	
	//�����ý�������DMA2��S5C7
	while((DMA2_Stream3->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
	DMA2->LIFCR |= 0x0f400000;								//������Ӧ���¼���־ȫ������
	DMA2_Stream3->CR |= DMA_IT_TC;							//����TC�ж�
	DMA2_Stream3->M0AR = (uint32_t)&InvalidValue;			//�����ڴ��ַ
	DMA2_Stream3->NDTR = Len;								//�������ý�Ҫ������ֽڳ���
	DMA2_Stream3->CR &= ~DMA_SxCR_MINC;						//�ڴ��ַ����ģʽ
	DMA2_Stream3->CR |= DMA_SxCR_EN;						//����DMA2��������5
	
	//����Ϊ�˴���ʱ�ӵķ���DMA2��S6C7
	while((DMA2_Stream4->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
	DMA2->HIFCR |= 0x0000003D;								//������Ӧ���¼���־ȫ������
	DMA2_Stream4->CR &= ~DMA_IT_TC;							//�ر�TC�ж�
	DMA2_Stream4->M0AR = (uint32_t)Buf;						//�����ڴ��ַ
	DMA2_Stream4->NDTR = Len;								//�������ý�Ҫ������ֽڳ���
	DMA2_Stream4->CR |= DMA_SxCR_MINC;						//�ڴ��ַ�̶�ģʽ
	DMA2_Stream4->CR |= DMA_SxCR_EN;						//����DMA2��������6
	
	//�ȴ��������
	while(SPIFlashDmaTCFlag == 0);
	
	FLASH_CS_HIGH;
}













/*�ⲿ������****************************************************************/
/***
*��ʼ��SPI FLASH
***/
ErrorStatus SPIFlash_Init(void)
{
	_SPI_Tx_Dma2S4C5_Config();
	_SPI_Rx_Dma2S3C5_Config();
	_SPI_Pin_Config();
	_SPI_Config();
	
	//��ȡIDֵ
	if(_SPIFlash_ReadId() != SPIFLASH_JEDEC_ID){
		return ERROR;
	}
	
	//��ȡ״̬�Ĵ���3��ֵ���ж��Ƿ��Ѿ�����4�ֽڵ�ַģʽ
	if((_SPIFlash_ReadStatReg3() & 0x01) == 0){
		//����Ϊ4�ֽڵ�ַģʽ
		_SPIFlash_Set4AdrMode();
	}
	
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
	FLASH_CS_LOW;
	
	_SPI_RW(SPIFLASH_CMD_RESET);
	
	FLASH_CS_HIGH;
}



/***
*��ȡ���ݣ���DMA��ȡģʽ
***/
ErrorStatus SPIFlash_ReadData(uint32_t Adr,uint8_t* Buf,uint32_t Len)
{
	if(Len == 0){
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
	extern uint8_t SPIFlashDmaTCFlag;
	uint16_t TempLen = 0;
	InvalidValue = 0xff;
	
	if(Len == 0){
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
		
		//�����ý�������DMA2��S5C7
		while((DMA2_Stream3->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
		DMA2->LIFCR |= 0x0f400000;								//������Ӧ���¼���־ȫ������
		DMA2_Stream3->CR |= DMA_IT_TC;							//����TC�ж�
		DMA2_Stream3->M0AR = (uint32_t)Buf;						//�����ڴ��ַ
		DMA2_Stream3->NDTR = TempLen;							//�������ý�Ҫ������ֽڳ���
		DMA2_Stream3->CR |= DMA_SxCR_MINC;						//�ڴ��ַ����ģʽ
		DMA2_Stream3->CR |= DMA_SxCR_EN;						//����DMA2��������5
		
		//����Ϊ�˴���ʱ�ӵķ���DMA2��S6C7
		while((DMA2_Stream4->CR & DMA_SxCR_EN) != 0);			//�ȴ�ENλΪ0�ſ��Կ�ʼ����DMA
		DMA2->HIFCR |= 0x0000003D;								//������Ӧ���¼���־ȫ������
		DMA2_Stream4->CR &= ~DMA_IT_TC;							//�ر�TC�ж�
		DMA2_Stream4->M0AR = (uint32_t)&InvalidValue;			//�����ڴ��ַ
		DMA2_Stream4->NDTR = TempLen;							//�������ý�Ҫ������ֽڳ���
		DMA2_Stream4->CR &= ~DMA_SxCR_MINC;						//�ڴ��ַ�̶�ģʽ
		DMA2_Stream4->CR |= DMA_SxCR_EN;						//����DMA2��������6
		
		//�ȴ��������
		while(SPIFlashDmaTCFlag == 0);
		
		//����Buf��ַ
		Buf += TempLen;
	}
	
	FLASH_CS_HIGH;
	
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
*����Ϊ��λд�����ݣ���FatFSд������ʹ��
***/
ErrorStatus SPIFlash_SectorWriteData(uint8_t* Buf,uint32_t SectorAdr,uint32_t SectorNum)
{
	return SPIFlash_WriteData(SectorAdr*SPIFLASH_SECTION_SIZE,Buf,SectorNum*SPIFLASH_SECTION_SIZE);
}







