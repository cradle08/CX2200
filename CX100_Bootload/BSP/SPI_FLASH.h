#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H




#include "stm32f4xx.h"




/*宏定义区***********************************************************/
#define SPIFLASH_VOLUME_SIZE							(32*1024*1024)				//定义W25Q256的容量，单位：字节
#define SPIFLASH_SECTION_SIZE							(4*1024)					//定义W25Q256的扇区容量，单位：字节
#define SPIFLASH_BLOCK_SIZE								(64*1024)					//定义W25Q256的块容量，单位：字节
#define SPIFLASH_PAGE_SIZE								256							//定义W25Q256的页容量，单位：字节












ErrorStatus SPIFlash_Init(void);
void SPIFlash_Reset(void);
ErrorStatus SPIFlash_IsReady(void);

ErrorStatus SPIFlash_ReadData(uint32_t Adr,uint8_t* Buf,uint32_t Len);
ErrorStatus SPIFlash_DMA_ReadData(uint32_t Adr,uint8_t* Buf,uint32_t Len);
ErrorStatus SPIFlash_DMA_SectorReadData(uint8_t* Buf,uint32_t SectorAdr,uint32_t SectorNum);

void SPIFlash_EraseSection(uint32_t Adr);
void SPIFlash_EraseBlock(uint32_t Adr);
void SPIFlash_EraseChip(void);

ErrorStatus SPIFlash_WriteData(uint32_t Adr,uint8_t* Buf,uint32_t Len);
ErrorStatus SPIFlash_SectorWriteData(uint8_t* Buf,uint32_t SectorAdr,uint32_t SectorNum);



#endif
