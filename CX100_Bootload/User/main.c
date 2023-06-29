#include "stm32f4xx.h"
#include "systick.h"
#include "usart.h"
#include "fmc_sdram.h"
#include "i2cHardPolling.h"
#include "flash.h"
#include "LCD.h"
#include "SPI_Flash.h"
#include "eeprom.h"
#include "string.h"
#include "output.h"
#include "global.h"





//����汾������ڳ�����ʼ��ַ��ƫ����
#define VERSION_AT_FLASH_OFFSET			0x3C00

//������������汾��
const uint16_t BootloaderSoftwareVersion __attribute__((at(FLASH_BASE+VERSION_AT_FLASH_OFFSET))) = 100;



int main(void)
{
	uint32_t UpdateMainSoftwareFlag = 0;
	UpdateMainSoftwareData_s* UpdateMainSoftwareDataBuf = 0;
	ErrorStatus ReturnState = SUCCESS;
	FLASH_Status FlashStatus = FLASH_COMPLETE;
	
	Systick_InitConfig();
	USART_Printf_Config();
	I2C_Config();
	printf("BootLoader��ʼ���ɹ����������ʱ�䣺%s,%s\r\n",__DATE__,__TIME__);
	
	//���ж�EEPROM��������ʶ�Ƿ���Ч���ǣ���ʼ��SPI_FLASH�ж�ȡ��������Ȼ���������ɣ�����ֱ����ת��������ִ��
	ReturnState = EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_UPDATE_MAIN_SOFTWARE,(uint8_t*)&UpdateMainSoftwareFlag,sizeof(UpdateMainSoftwareFlag));
	
	//����EEPROM�ж�ȡ����ʧ��ʱ����ֱ����ת
	if(UpdateMainSoftwareFlag != UPDATE_MAIN_SOFTWARE_FLAG || ReturnState != SUCCESS){
		//ֱ����ת�������򼴿�
		if(IapJumpIntoApp() == ERROR){
			//������תʧ��
			FMC_SDRAMConfig();
			Output_Config();
			LCD_Init();
			
			printf("������תʧ��\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("APP JUMP ERROR,STOP!");
			
			while(1);
		}
	}
	
	//��Ҫ���������򣬽�����������
	FMC_SDRAMConfig();
	Output_Config();
	LCD_Init();
	
	LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
	LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,13);
	LCD_DisplayString("UPDATING,PLEASE WAIT...");
	
	if(SPIFlash_Init() != SUCCESS){
		//SPI FLASH��ʼ��ʧ�ܣ�ֱ����ת������
		printf("SPI FLASH��ʼ��ʧ�ܣ���ת��������...\r\n");
		LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
		LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,12);
		LCD_DisplayString("FLASH INIT ERR,JUMPING...");
		
		Delay_MS(1000);
		
		if(IapJumpIntoApp() == ERROR){
			printf("������תʧ��\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("APP JUMP ERROR,STOP!");
			
			while(1);
		}
	}
	
	printf("SPI FLASH��ʼ���ɹ�����ȡ������������\r\n");
	LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
	LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,17);
	LCD_DisplayString("READING DATA...");
	
	//��SDRAMƫ��10M����Ϊ�����������Ļ���
	UpdateMainSoftwareDataBuf = (UpdateMainSoftwareData_s*)(FMC_SDRAM_BANK1_ADR_START+SDRAM_SAVE_MAIN_SOFTWARE_OFFSET);
	memset(UpdateMainSoftwareDataBuf,0,2*1024*1024);
	
	//��SPI_FLASH�ж�ȡ������ͷ����
	SPIFlash_DMA_ReadData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)UpdateMainSoftwareDataBuf,sizeof(UpdateMainSoftwareData_s));
	
	//��֤�����Ƿ���ȷ
	if(UpdateMainSoftwareDataBuf->flag != UPDATE_MAIN_SOFTWARE_FLAG || UpdateMainSoftwareDataBuf->totalLen < 100*1024 || UpdateMainSoftwareDataBuf->totalLen > 1024*1024){
		//SPI_FLASH�����ݴ���ֱ����ת����������
		printf("SPI FLASH�������������쳣����ת��������...\r\n");
		LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
		LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
		LCD_DisplayString("DATA ERR,JUMPING...");
		
		Delay_MS(1000);
		
		if(IapJumpIntoApp() == ERROR){
			printf("������תʧ��\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("APP JUMP ERROR,STOP!");
			
			while(1);
		}
	}
	
	//��SPI_FLASH�ж�ȡ��������������
	SPIFlash_DMA_ReadData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)UpdateMainSoftwareDataBuf,sizeof(UpdateMainSoftwareData_s)+UpdateMainSoftwareDataBuf->totalLen);
	
	//У������
	if(CRC16((uint8_t*)UpdateMainSoftwareDataBuf->data,UpdateMainSoftwareDataBuf->totalLen) != UpdateMainSoftwareDataBuf->crc16){
		//У�����ݴ������¶�ȡУ��һ��
		SPIFlash_DMA_ReadData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)UpdateMainSoftwareDataBuf,sizeof(UpdateMainSoftwareData_s)+UpdateMainSoftwareDataBuf->totalLen);
		if(CRC16((uint8_t*)UpdateMainSoftwareDataBuf->data,UpdateMainSoftwareDataBuf->totalLen) != UpdateMainSoftwareDataBuf->crc16){
			//SPI_FLASH�����ݴ���ֱ����ת����������
			printf("SPI FLASH������������CRC�쳣����ת��������...\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,13);
			LCD_DisplayString("DATA CRC ERR,JUMPING...");
			
			Delay_MS(1000);
			
			if(IapJumpIntoApp() == ERROR){
				printf("������תʧ��\r\n");
				LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
				LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
				LCD_DisplayString("APP JUMP ERROR,STOP!");
				
				while(1);
			}
		}
	}
	
	//����У��ɹ���׼������
	printf("����У��ɹ���׼������\r\n");
	LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
	LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,13);
	LCD_DisplayString("DATA IS OK,WRITE DATA...");
	
	//д�����ݵ�FLASH
	FlashStatus = FLASH_WriteData_8(FLASH_BASE+MCU_FLASH_MAIN_SOFTWARE_START_ADR,UpdateMainSoftwareDataBuf->data,UpdateMainSoftwareDataBuf->totalLen);
	if(FlashStatus != FLASH_COMPLETE){
		//��д��һ��
		FlashStatus = FLASH_WriteData_8(FLASH_BASE+MCU_FLASH_MAIN_SOFTWARE_START_ADR,UpdateMainSoftwareDataBuf->data,UpdateMainSoftwareDataBuf->totalLen);
		if(FlashStatus != FLASH_COMPLETE){
			printf("����д��ʧ��\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("DATA WRITE ERR,STOP!");
			
			while(1);
		}
	}
	
	//У��д�뵽FLASH�е�����
	if(CRC16((uint8_t*)(FLASH_BASE+MCU_FLASH_MAIN_SOFTWARE_START_ADR),UpdateMainSoftwareDataBuf->totalLen) != UpdateMainSoftwareDataBuf->crc16){
		//��д��һ��
		FlashStatus = FLASH_WriteData_8(FLASH_BASE+MCU_FLASH_MAIN_SOFTWARE_START_ADR,UpdateMainSoftwareDataBuf->data,UpdateMainSoftwareDataBuf->totalLen);
		if(FlashStatus != FLASH_COMPLETE){
			printf("����д��ʧ��\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("DATA WRITE ERR,STOP!");
			
			while(1);
		}
		
		//У��д�������
		if(CRC16((uint8_t*)(FLASH_BASE+MCU_FLASH_MAIN_SOFTWARE_START_ADR),UpdateMainSoftwareDataBuf->totalLen) != UpdateMainSoftwareDataBuf->crc16){
			printf("����д��ʧ��\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("DATA WRITE ERR,STOP!");
			
			while(1);
		}
	}
	
	//�����ɹ���׼����ת
	printf("�����ɹ�\r\n");
	LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
	LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
	LCD_DisplayString("UPDATE SUCCESS!JUMPING...");
	
	Delay_MS(500);
	
	if(IapJumpIntoApp() == ERROR){
		printf("������תʧ��\r\n");
		LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
		LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
		LCD_DisplayString("APP JUMP ERROR,STOP!");
		
		while(1);
	}
	
	
	while(1){
		
	}
}

