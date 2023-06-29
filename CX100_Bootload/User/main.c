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





//定义版本号相对于程序起始地址的偏移量
#define VERSION_AT_FLASH_OFFSET			0x3C00

//定义引导程序版本号
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
	printf("BootLoader初始化成功，程序编译时间：%s,%s\r\n",__DATE__,__TIME__);
	
	//先判断EEPROM中升级标识是否有效，是，则开始从SPI_FLASH中读取出主程序然后升级即可，否，则直接跳转到主程序执行
	ReturnState = EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_UPDATE_MAIN_SOFTWARE,(uint8_t*)&UpdateMainSoftwareFlag,sizeof(UpdateMainSoftwareFlag));
	
	//当从EEPROM中读取数据失败时，则直接跳转
	if(UpdateMainSoftwareFlag != UPDATE_MAIN_SOFTWARE_FLAG || ReturnState != SUCCESS){
		//直接跳转到主程序即可
		if(IapJumpIntoApp() == ERROR){
			//程序跳转失败
			FMC_SDRAMConfig();
			Output_Config();
			LCD_Init();
			
			printf("程序跳转失败\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("APP JUMP ERROR,STOP!");
			
			while(1);
		}
	}
	
	//需要升级主程序，进入升级流程
	FMC_SDRAMConfig();
	Output_Config();
	LCD_Init();
	
	LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
	LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,13);
	LCD_DisplayString("UPDATING,PLEASE WAIT...");
	
	if(SPIFlash_Init() != SUCCESS){
		//SPI FLASH初始化失败，直接跳转主程序
		printf("SPI FLASH初始化失败，跳转主程序中...\r\n");
		LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
		LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,12);
		LCD_DisplayString("FLASH INIT ERR,JUMPING...");
		
		Delay_MS(1000);
		
		if(IapJumpIntoApp() == ERROR){
			printf("程序跳转失败\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("APP JUMP ERROR,STOP!");
			
			while(1);
		}
	}
	
	printf("SPI FLASH初始化成功，读取主程序升级包\r\n");
	LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
	LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,17);
	LCD_DisplayString("READING DATA...");
	
	//从SDRAM偏移10M处作为接收升级包的缓存
	UpdateMainSoftwareDataBuf = (UpdateMainSoftwareData_s*)(FMC_SDRAM_BANK1_ADR_START+SDRAM_SAVE_MAIN_SOFTWARE_OFFSET);
	memset(UpdateMainSoftwareDataBuf,0,2*1024*1024);
	
	//从SPI_FLASH中读取升级包头数据
	SPIFlash_DMA_ReadData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)UpdateMainSoftwareDataBuf,sizeof(UpdateMainSoftwareData_s));
	
	//验证数据是否正确
	if(UpdateMainSoftwareDataBuf->flag != UPDATE_MAIN_SOFTWARE_FLAG || UpdateMainSoftwareDataBuf->totalLen < 100*1024 || UpdateMainSoftwareDataBuf->totalLen > 1024*1024){
		//SPI_FLASH中数据错误，直接跳转到主程序中
		printf("SPI FLASH中升级包数据异常，跳转主程序中...\r\n");
		LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
		LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
		LCD_DisplayString("DATA ERR,JUMPING...");
		
		Delay_MS(1000);
		
		if(IapJumpIntoApp() == ERROR){
			printf("程序跳转失败\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("APP JUMP ERROR,STOP!");
			
			while(1);
		}
	}
	
	//从SPI_FLASH中读取升级包完整数据
	SPIFlash_DMA_ReadData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)UpdateMainSoftwareDataBuf,sizeof(UpdateMainSoftwareData_s)+UpdateMainSoftwareDataBuf->totalLen);
	
	//校验数据
	if(CRC16((uint8_t*)UpdateMainSoftwareDataBuf->data,UpdateMainSoftwareDataBuf->totalLen) != UpdateMainSoftwareDataBuf->crc16){
		//校验数据错误，重新读取校验一次
		SPIFlash_DMA_ReadData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)UpdateMainSoftwareDataBuf,sizeof(UpdateMainSoftwareData_s)+UpdateMainSoftwareDataBuf->totalLen);
		if(CRC16((uint8_t*)UpdateMainSoftwareDataBuf->data,UpdateMainSoftwareDataBuf->totalLen) != UpdateMainSoftwareDataBuf->crc16){
			//SPI_FLASH中数据错误，直接跳转到主程序中
			printf("SPI FLASH中升级包数据CRC异常，跳转主程序中...\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,13);
			LCD_DisplayString("DATA CRC ERR,JUMPING...");
			
			Delay_MS(1000);
			
			if(IapJumpIntoApp() == ERROR){
				printf("程序跳转失败\r\n");
				LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
				LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
				LCD_DisplayString("APP JUMP ERROR,STOP!");
				
				while(1);
			}
		}
	}
	
	//数据校验成功，准备升级
	printf("数据校验成功，准备升级\r\n");
	LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
	LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,13);
	LCD_DisplayString("DATA IS OK,WRITE DATA...");
	
	//写入数据到FLASH
	FlashStatus = FLASH_WriteData_8(FLASH_BASE+MCU_FLASH_MAIN_SOFTWARE_START_ADR,UpdateMainSoftwareDataBuf->data,UpdateMainSoftwareDataBuf->totalLen);
	if(FlashStatus != FLASH_COMPLETE){
		//再写入一次
		FlashStatus = FLASH_WriteData_8(FLASH_BASE+MCU_FLASH_MAIN_SOFTWARE_START_ADR,UpdateMainSoftwareDataBuf->data,UpdateMainSoftwareDataBuf->totalLen);
		if(FlashStatus != FLASH_COMPLETE){
			printf("数据写入失败\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("DATA WRITE ERR,STOP!");
			
			while(1);
		}
	}
	
	//校验写入到FLASH中的数据
	if(CRC16((uint8_t*)(FLASH_BASE+MCU_FLASH_MAIN_SOFTWARE_START_ADR),UpdateMainSoftwareDataBuf->totalLen) != UpdateMainSoftwareDataBuf->crc16){
		//再写入一次
		FlashStatus = FLASH_WriteData_8(FLASH_BASE+MCU_FLASH_MAIN_SOFTWARE_START_ADR,UpdateMainSoftwareDataBuf->data,UpdateMainSoftwareDataBuf->totalLen);
		if(FlashStatus != FLASH_COMPLETE){
			printf("数据写入失败\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("DATA WRITE ERR,STOP!");
			
			while(1);
		}
		
		//校验写入的数据
		if(CRC16((uint8_t*)(FLASH_BASE+MCU_FLASH_MAIN_SOFTWARE_START_ADR),UpdateMainSoftwareDataBuf->totalLen) != UpdateMainSoftwareDataBuf->crc16){
			printf("数据写入失败\r\n");
			LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
			LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
			LCD_DisplayString("DATA WRITE ERR,STOP!");
			
			while(1);
		}
	}
	
	//升级成功，准备跳转
	printf("升级成功\r\n");
	LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
	LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
	LCD_DisplayString("UPDATE SUCCESS!JUMPING...");
	
	Delay_MS(500);
	
	if(IapJumpIntoApp() == ERROR){
		printf("程序跳转失败\r\n");
		LCD_ClearAll(LCD_LAYER_B,LCD_COLOR_WHITE);
		LCD_SetPara(LCD_LAYER_B,LCD_COLOR_BLACK,LCD_COLOR_WHITE,&Font16x24,10,15);
		LCD_DisplayString("APP JUMP ERROR,STOP!");
		
		while(1);
	}
	
	
	while(1){
		
	}
}

