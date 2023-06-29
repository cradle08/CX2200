#ifndef __BSP_NAND_H__
#define __BSP_NAND_H__

#include "stm32f4xx_hal.h"
#include "file_operate.h"


#define NAND_ADDRESS			0X80000000	//nand flash�ķ��ʵ�ַ,��NCE3,��ַΪ:0X8000 0000
#define NAND_CMD				1<<16		//��������
#define NAND_ADDR				1<<17		//���͵�ַ

//NAND FLASH����
#define NAND_READID         	0X90    	//��IDָ��
#define NAND_FEATURE			0XEF    	//��������ָ��
#define NAND_RESET          	0XFF    	//��λNAND
#define NAND_READSTA        	0X70   	 	//��״̬
#define NAND_AREA_A         	0X00   
#define NAND_AREA_TRUE1     	0X30  
#define NAND_WRITE0        	 	0X80
#define NAND_WRITE_TURE1    	0X10
#define NAND_ERASE0        	 	0X60
#define NAND_ERASE1         	0XD0
#define NAND_MOVEDATA_CMD0  	0X00
#define NAND_MOVEDATA_CMD1  	0X35
#define NAND_MOVEDATA_CMD2  	0X85
#define NAND_MOVEDATA_CMD3  	0X10

//NAND FLASH״̬
#define NSTA_READY       	   	0X40		//nand�Ѿ�׼����
#define NSTA_ERROR				0X01		//nand����
#define NSTA_TIMEOUT        	0X02		//��ʱ
#define NSTA_ECC1BITERR       	0X03		//ECC 1bit����
#define NSTA_ECC2BITERR       	0X04		//ECC 2bit���ϴ���

//NAND FLASH���������ʱ����
#define NAND_TADL_DELAY				30			//tADL�ȴ��ӳ�,����70ns
#define NAND_TWHR_DELAY				25			//tWHR�ȴ��ӳ�,����60ns
#define NAND_TRHW_DELAY				35			//tRHW�ȴ��ӳ�,����100ns
#define NAND_TPROG_DELAY			200			//tPROG�ȴ��ӳ�,����ֵ200us,�����Ҫ700us
#define NAND_TBERS_DELAY			4			//tBERS�ȴ��ӳ�,����ֵ3.5ms,�����Ҫ10ms
//#define NAND_TPROG_DELAY			25000       //35*10*200 = 200us ͨ��ѭ������ʱ			
//#define NAND_TBERS_DELAY			20*NAND_TPROG_DELAY			//35*10*200*20 = 4ms ͨ��ѭ������ʱ		





#if USE_RL_FLASH_FS
#define NAND_BLOCK_NUM      4096
uint8_t NAND_Reset(void);
int32_t Driver_NAND0_GetDeviceBusy (uint32_t dev_num);
void NAND_EraseChip(void);
uint8_t NAND_EraseBlock(uint32_t BlockNum);
#else

#define NAND_MAX_LUT_SIZE           2048
#define NAND_MAX_PAGE_SIZE			4096		//����NAND FLASH������PAGE��С��������SPARE������Ĭ��4096�ֽ�
#define NAND_ECC_SECTOR_SIZE		512			//ִ��ECC����ĵ�Ԫ��С��Ĭ��512�ֽ�




//NAND���Խṹ��
typedef struct
{
    uint16_t page_totalsize;     	//ÿҳ�ܴ�С��main����spare���ܺ�
    uint16_t page_mainsize;      	//ÿҳ��main����С
    uint16_t page_sparesize;     	//ÿҳ��spare����С
    uint8_t  block_pagenum;      	//ÿ���������ҳ����
    uint16_t plane_blocknum;     	//ÿ��plane�����Ŀ�����
    uint16_t block_totalnum;     	//�ܵĿ�����
    uint16_t good_blocknum;      	//�ÿ�����    
    uint16_t valid_blocknum;     	//��Ч������(���ļ�ϵͳʹ�õĺÿ�����)
    uint32_t id;             		//NAND FLASH ID
    //uint16_t *lut;      			   	//LUT�������߼���-�����ת��
	uint16_t lut[NAND_MAX_PAGE_SIZE];    //LUT�������߼���-�����ת��
    uint32_t ecc_hard;				//Ӳ�����������ECCֵ
	uint32_t ecc_hdbuf[NAND_MAX_PAGE_SIZE/NAND_ECC_SECTOR_SIZE];//ECCӲ������ֵ������  	
	uint32_t ecc_rdbuf[NAND_MAX_PAGE_SIZE/NAND_ECC_SECTOR_SIZE];//ECC��ȡ��ֵ������
}nand_attriute;      

extern nand_attriute nand_dev;				//nand��Ҫ�����ṹ�� 

#define NAND_RB  				 HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6)//PDin(6)	//NAND Flash����/æ���� 




//NAND FLASH�ͺźͶ�Ӧ��ID��
#define MT29F4G08ABADA			0XDC909556	//MT29F4G08ABADA
#define MT29F16G08ABABA			0X48002689	//MT29F16G08ABABA

 //
void FMC_NAND_Coustom_Define(void);

uint8_t NAND_Init(void);
uint8_t NAND_ModeSet(uint8_t mode);
uint32_t NAND_ReadID(void);
uint8_t NAND_ReadStatus(void);
uint8_t NAND_WaitForReady(void);
uint8_t NAND_Reset(void);
uint8_t NAND_WaitRB(__IO uint8_t rb);
void NAND_Delay(__IO uint32_t i);
uint8_t NAND_ReadPage(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToRead);
uint8_t NAND_ReadPageComp(uint32_t PageNum,uint16_t ColNum,uint32_t CmpVal,uint16_t NumByteToRead,uint16_t *NumByteEqual);
uint8_t NAND_WritePage(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToWrite);
uint8_t NAND_WritePageConst(uint32_t PageNum,uint16_t ColNum,uint32_t cval,uint16_t NumByteToWrite);
uint8_t NAND_CopyPageWithoutWrite(uint32_t Source_PageNum,uint32_t Dest_PageNum);
uint8_t NAND_CopyPageWithWrite(uint32_t Source_PageNum,uint32_t Dest_PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToWrite);
uint8_t NAND_ReadSpare(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToRead);
uint8_t NAND_WriteSpare(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToRead);
uint8_t NAND_EraseBlock(uint32_t BlockNum);
void NAND_EraseChip(void);

uint16_t NAND_ECC_Get_OE(uint8_t oe,uint32_t eccval);
uint8_t NAND_ECC_Correction(uint8_t* data_buf,uint32_t eccrd,uint32_t ecccl);




#endif


#endif //__BSP_NAND_H__



















