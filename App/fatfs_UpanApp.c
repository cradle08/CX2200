/*****************************************************************************
FatFS�ļ�ϵͳӦ��ʾ�������Ե�ʱ�������ļ���д��ջ�ռ�Ͷѿռ���Ҫע�⣬��ֹ�������Ӳ���жϡ�

******************************************************************************/
#include "fatfs_UpanApp.h"
#include "usart.h"
#include "string.h"
#include "ffconf.h"
#include "crc16.h"
#include "flash.h"
#include "bsp_spi.h"
#include "bsp_eeprom.h"
#include "file_operate.h"
#include "Common.h"



/*�궨����********************************************************************/
#define BOOT_FILE_FLAG							0x5AA50FF0								//BOOT�����ļ���־
#define MAIN_FILE_FLAG							0xA55AF00F								//MAIN�����ļ���־
#define UI_FILE_FLAG							0x0FF0A55A								//UI��Դ�ļ���־

#define BOOT_PATH								"0:/CX-2200_Boot.bin"					//����BOOT�����ļ���U���е�·��
#define MAIN_PATH								"0:/CX-2200_Main.bin"					//����Main�����ļ���U���е�·��
#define UI_PATH									"0:/UI"									//����UI�ļ���U���еĸ�Ŀ¼

#define SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET		30*1024*1024							//����������SPI_FLASH�д�����������ݵ���ʼƫ����


/*ȫ�ֱ���������***************************************************************/





/*�ⲿ������*******************************************************************/
/***
*�����ļ�ϵͳ���豸�ϡ��ļ�ϵͳ����ʱ���SPI�豸��ʼ��
*������
	Label��Ҫ�����ļ�ϵͳ���豸�����ַ������磺0:
	FS�����ص��豸ȫ�־��
	MkfsFlag���Ƿ���Ҫǿ�Ƹ�ʽ����־
*����ֵ���ļ������ķ��ؽ��
***/
FRESULT FatFs_Mount(char* Label,FATFS* FS,ForceMkfs_e MkfsFlag)
{
	FRESULT ResFatFs = FR_OK;
	uint8_t Work[_MAX_SS];
	
	if(Label == NULL){
		return FR_NO_PATH;
	}
	
	if(FS == NULL){
		return FR_INVALID_OBJECT;
	}
	
	//�����豸
	ResFatFs = f_mount(FS,Label,0);
	
	if(MkfsFlag == FORCE_MKFS){			//ǿ��ִ�и�ʽ������
		//��ʽ���豸
		ResFatFs = f_mkfs(Label,FM_ANY,0,Work,sizeof(Work));
		if(ResFatFs == FR_OK){
			printf("%s����ʽ���ɹ�\r\n",Label);
			
			//��ʽ������ȡ�����أ�Ȼ�����¹��ؼ�������ʹ���ļ�ϵͳ
			f_mount(NULL,Label,0);
			ResFatFs = f_mount(FS,Label,0);
		}else{
			printf("%s����ʽ��ʧ��\r\n",Label);
			f_mount(NULL,Label,0);
			return FR_MKFS_ABORTED;
		}
	}else if(ResFatFs == FR_NO_FILESYSTEM){
		//��Ϊ�漰��ʽ���������������¶�ȡ���أ��Ա�ȷ���Ƿ����û���ļ�ϵͳ����ֹ������������ݶ�ʧ
		if(f_mount(FS,Label,0) == FR_NO_FILESYSTEM){
			printf("%s����û���ļ�ϵͳ���������и�ʽ��...\r\n",Label);
		
			//��ʽ���豸
			ResFatFs = f_mkfs(Label,0,0,Work,sizeof(Work));
			if(ResFatFs == FR_OK){
				printf("%s����ʽ���ɹ�\r\n",Label);
				
				//��ʽ������ȡ�����أ�Ȼ�����¹��ؼ�������ʹ���ļ�ϵͳ
				f_mount(NULL,Label,0);
				ResFatFs = f_mount(FS,Label,0);
			}else{
				printf("%s����ʽ��ʧ��\r\n",Label);
				f_mount(NULL,Label,0);
				return FR_MKFS_ABORTED;
			}
		}else{
			//�����豸
			printf("%s�������ж��Ƿ���Ҫ��ʽ��������һ�£���������\r\n",Label);
			
			__set_PRIMASK(1);
			NVIC_SystemReset();
		}
	}else if(ResFatFs != FR_OK){
		printf("%s�������ļ�ϵͳʧ�ܡ�(%d)\r\n",Label,ResFatFs);
		printf("����ԭ���豸��ʼ�����ɹ���\r\n");
		return ResFatFs;
	}else{
		printf("%s���ļ�ϵͳ���سɹ�\r\n",Label);
	}
	
	return ResFatFs;
}




/***
*��ȡ�豸������Ϣ
*������
	Label��Ҫ�����ļ�ϵͳ���豸�����ַ������磺0:
	FreePlaceByte���������豸���пռ��ֽ���
	TotalPlaceByte���������豸���ֽ���
*����ֵ���ļ������ķ��ؽ��
***/
FRESULT FatFs_GetDiskInfo(char* Label,uint64_t* FreePlaceByte,uint64_t* TotalPlaceByte)
{
	unsigned long FreeClstNum = 0;				//����������õĿ��д���
	FATFS* Fs; 								//��������ָ���������Ŷ�Ӧ��FatFS������������������ó��ܴ�����ÿ���ذ�����������
	uint64_t FreeSectorNum = 0;				//����������
	uint64_t TotalSectorNum = 0;			//��������
	FRESULT ResFatFs = FR_OK;
	MSC_LUNTypeDef info;
	extern USBH_HandleTypeDef hUsbHostFS;
	
	if(Label == NULL){
		return FR_NO_PATH;
	}
	
	//��ȡU����Ϣ
	USBH_MSC_GetLUNInfo(&hUSB_Host, 0, &info);
	
	*FreePlaceByte = 0;
	*TotalPlaceByte = 0;
	
	//��ȡ�豸����
	ResFatFs = f_getfree(Label,&FreeClstNum,&Fs);
	
	if(ResFatFs != FR_OK){
		return ResFatFs;
	}
	
	FreeSectorNum = FreeClstNum * Fs->csize;
	TotalSectorNum = (Fs->n_fatent - 2)*Fs->csize;
	
	//���ݲ�ͬ���豸���õ�������С�ó���Ӧ�Ŀ����ֽ��������ֽ���,����Ҫ����diskio.c�����õ�GET_SECTOR_SIZE�����õ�������Сֵ�����ֽ���
	if(strcmp(Label,U_PAN_LABEL) == 0){
		*FreePlaceByte = FreeSectorNum * info.capacity.block_size;
		*TotalPlaceByte = TotalSectorNum * info.capacity.block_size;
	}
	
	return ResFatFs;
}



/***
*�����༶Ŀ¼
*������
	Path��Ҫ����Ŀ¼��·������Ҫע����·������̫������ֹ������������·��������һ�������ĵ��ļ���β��·�����磺0:/Dir1/Dir2/Test.txt
*����ֵ���ļ������ķ��ؽ��
***/
FRESULT FatFs_CreateMutiDir(char* Path)
{
	char DirTempPath1[50] = {0};
	char DirTempPath2[50] = {0};
	uint8_t Len = 0;
	char* StrP1 = NULL;
	char* StrP2 = NULL;
	DIR dp;
	FRESULT ResFatFs = FR_OK;
	
	
	if(Path == NULL){
		return FR_NO_PATH;
	}
	
	if(strstr(Path,".") == NULL){
		return FR_INVALID_NAME;
	}
	
	strcpy(DirTempPath1,Path);
	
	//�𼶴���Ŀ¼
	StrP1 = strtok(DirTempPath1,"/");
	StrP2 = strtok(NULL,"/");
	if(StrP2 == NULL){
		printf("û��Ŀ¼�㼶\r\n");
		return FR_INVALID_NAME;
	}
	
	Len += sprintf(DirTempPath2+Len,"%s",StrP1);			//�����ȡ�����Ǹ�Ŀ¼����0:
	
	while(1){
		StrP1 = strtok(NULL,"/");
		if(StrP1 == NULL){
			break;
		}
		
		Len += sprintf(DirTempPath2+Len,"/%s",StrP2);
		if(f_opendir(&dp,DirTempPath2) == FR_NO_PATH){
			//����Ŀ¼
			ResFatFs = f_mkdir(DirTempPath2);
			if(ResFatFs != FR_OK){
				printf("����Ŀ¼ʧ�ܣ��˳�\r\n");
				return ResFatFs;
			}
		}else{
			//�ر�Ŀ¼
			f_closedir(&dp);
		}
		
		StrP2 = strtok(NULL,"/");
		if(StrP2 == NULL){
			break;
		}
		Len += sprintf(DirTempPath2+Len,"/%s",StrP1);
		if(f_opendir(&dp,DirTempPath2) == FR_NO_PATH){
			//����Ŀ¼
			ResFatFs = f_mkdir(DirTempPath2);
			if(ResFatFs != FR_OK){
				printf("����Ŀ¼ʧ�ܣ��˳�\r\n");
				return ResFatFs;
			}
		}else{
			//�ر�Ŀ¼
			f_closedir(&dp);
		}
	}
	
	return FR_OK;
}




/***
*ɾ��ָ��Ŀ¼�µ������ļ�����Ŀ¼
*������
	Path��·�������·��ֻ��ΪĿ¼·�������ܴ��ļ�·�����磺0:/Dir1
*����ֵ���ļ������ķ��ؽ��
***/
FRESULT FatFs_UpanDelFileSubDir(char* Path)
{
	FRESULT ResFatFs = FR_OK;
	DIR dp;
	FILINFO fno;
	char StrTemp[100] = {0};
	
	//�ж����Ƿ��ǿ�·��
	if(Path == NULL){
		return FR_NO_PATH;
	}
	
	//�ж�·����ʽ�Ƿ���ȷ
	if(strstr(Path,".") != NULL){
		return FR_INVALID_NAME;
	}
	
	//��Ŀ¼
	ResFatFs = f_opendir(&dp,Path);
	if(ResFatFs != FR_OK){
		printf("��Ŀ¼ʧ�ܣ�%u\r\n",ResFatFs);
		return ResFatFs;
	}
	
	//����ɾ��
	while(1){
		//��ȡĿ¼��Ϣ
		ResFatFs = f_readdir(&dp,&fno);
		if(ResFatFs != FR_OK){
			f_closedir(&dp);
			printf("��ȡĿ¼��ʧ�ܣ�%u\r\n",ResFatFs);
			return ResFatFs;
		}
		
		//��������ֻ����ASCII�������ַ�����Ӧ��ʮ�����ƿ϶�С��0x80�������������Ϊд������б��жϵ����ļ����ϣ��Ӷ���ȡ�쳣
		if(fno.fname[0] > 0x21 && fno.fname[0] <= 0x7a){
			if((fno.fattrib & AM_DIR) != 0){
				//��Ŀ¼���ݹ�ɾ�������ļ���Ȼ����ɾ����Ŀ¼
				memset(StrTemp,0,100);
				sprintf(StrTemp,"%s/%s",Path,fno.fname);
				ResFatFs = FatFs_UpanDelFileSubDir(StrTemp);
				if(ResFatFs != FR_OK){
					f_closedir(&dp);
					printf("�ݹ�ɾ����Ŀ¼���ļ�ʧ�ܣ�%u\r\n",ResFatFs);
					return ResFatFs;
				}
				
				printf("ɾ����Ŀ¼��%s\r\n",StrTemp);
				ResFatFs = f_unlink(StrTemp);				//ɾ���Ѿ�Ϊ�յ���Ŀ¼
				if(ResFatFs != FR_OK){
					f_closedir(&dp);
					printf("ɾ����Ŀ¼ʧ�ܣ�%u\r\n",ResFatFs);
					return ResFatFs;
				}
				
				//ɾ������Ŀ¼����Ҫ�����������������᷵��FR_INVALID_OBJECT
				f_readdir(&dp,&fno);
			}else if((fno.fattrib & AM_ARC) != 0){
				//�ļ�
				//���ж��ļ�����ʽ�ַ����Ƿ���ȷ
				if(strstr(fno.fname,".") == NULL){
					printf("�ļ��𻵣�����\r\n");
					continue;
				}
				
				memset(StrTemp,0,100);
				sprintf(StrTemp,"%s/%s",Path,fno.fname);
				printf("ɾ���ļ���%s\r\n",StrTemp);
				
				//ɾ���ļ�
				ResFatFs = f_unlink(StrTemp);				
				if(ResFatFs != FR_OK){
					f_closedir(&dp);
					printf("ɾ���ļ�ʧ�ܣ�%u\r\n",ResFatFs);
					return ResFatFs;
				}
			}else{
				printf("�ļ���Ŀ¼�𻵣�����\r\n");
			}
		}else if(fno.fname[0] == 0){
			//ɾ����ϣ��˳�
			f_closedir(&dp);
			return FR_OK;
		}else{
			printf("�ļ���Ŀ¼�𻵣�����\r\n");
		}
	}
}



/***
*U������BOOT
*����ֵ���ļ������ķ��ؽ����
	FR_OK����ʾ�ļ�����д��
	FR_NO_PATH����ʾû���ļ�������Ҫ�����������ļ��쳣
	FR_INVALID_OBJECT����ʾ�ļ�У��ʧ��
	FR_INVALID_PARAMETER����ʾ�ļ�����ͨ������д��ʧ��
***/
FRESULT Upan_UpdateBoot(void)
{
	FRESULT Restatus = FR_OK;
	FIL UpanFL = {0};
	uint8_t* BootBuf = (uint8_t*)SDRAM_ApplyCommonBuf(1024*1024);
	uint32_t Len = 0;
	uint32_t FileFlag = 0;
	uint16_t CRC16Value = 0;
	
	Restatus = f_open(&UpanFL,BOOT_PATH,FA_READ|FA_OPEN_EXISTING);
	if(Restatus != FR_OK){
		//�ļ���ʧ�ܣ������ڻ����ļ��쳣
		return FR_NO_PATH;
	}
	
	//��ȡ����
	Restatus = f_read(&UpanFL,BootBuf,1024*1024,&Len);
	if(Restatus != FR_OK){
		f_close(&UpanFL);
		
		//�ļ���ȡʧ��
		return Restatus;
	}
	
	f_close(&UpanFL);
	
	//У���ļ�
	if(Len >= 1024*1024){
		//�ļ���С����
		return FR_INVALID_OBJECT;
	}
	
	//����CRCУ��
	CRC16Value = BootBuf[5]<<8 | BootBuf[4];
	if(CRC16Value != CRC16(BootBuf+8,Len-8)){
		//�ļ�У�����
		return FR_INVALID_OBJECT;
	}
	
	//�ж��ļ���־�Ƿ���ȷ
	FileFlag = BootBuf[3]<<24 | BootBuf[2]<<16 | BootBuf[1]<<8 | BootBuf[0];
	if(FileFlag != (BOOT_FILE_FLAG ^ CRC16Value)){
		//�ļ���־����
		return FR_INVALID_OBJECT;
	}
	
	//�ļ���֤ͨ������ʼд��
	if(FLASH_WriteData_8(FLASH_BASE,BootBuf+8,Len-8) != HAL_OK){
		//��д��һ��
		if(FLASH_WriteData_8(FLASH_BASE,BootBuf+8,Len-8) != HAL_OK){
			return FR_INVALID_PARAMETER;
		}else{
			//У��д�뵽FLASH�е�����
			if(CRC16((uint8_t*)FLASH_BASE,Len-8) != CRC16Value){
				//У��ʧ�ܣ�������д��һ��
				if(FLASH_WriteData_8(FLASH_BASE,BootBuf+8,Len-8) != HAL_OK){
					return FR_INVALID_PARAMETER;
				}else{
					if(CRC16((uint8_t*)FLASH_BASE,Len-8) != CRC16Value){
						return FR_INVALID_PARAMETER;
					}
				}
			}
		}
	}else{
		//У��д�뵽FLASH�е�����
		if(CRC16((uint8_t*)FLASH_BASE,Len-8) != CRC16Value){
			//У��ʧ�ܣ�������д��һ��
			if(FLASH_WriteData_8(FLASH_BASE,BootBuf+8,Len-8) != HAL_OK){
				return FR_INVALID_PARAMETER;
			}else{
				if(CRC16((uint8_t*)FLASH_BASE,Len-8) != CRC16Value){
					return FR_INVALID_PARAMETER;
				}
			}
		}
	}
	
	return FR_OK;
}



/***
*U������Main
*����ֵ���ļ������ķ��ؽ����
	FR_OK����ʾ�ļ�����д��
	FR_NO_PATH����ʾû���ļ�������Ҫ�����������ļ��쳣
	FR_INVALID_OBJECT����ʾ�ļ�У��ʧ��
	FR_INVALID_PARAMETER����ʾ�ļ�����ͨ������д��ʧ��
***/
FRESULT Upan_UpdateMain(void)
{
	extern MachRunPara_s MachRunPara;
	FRESULT Restatus = FR_OK;
	FIL UpanFL = {0};
	uint32_t FileFlag = 0;
	uint16_t CRC16Value = 0;
	FTPDownloadData_s* FTPDownLoadData = (FTPDownloadData_s*)SDRAM_ApplyCommonBuf(sizeof(FTPDownloadData_s));
	uint32_t i = 0;
	
	Restatus = f_open(&UpanFL,MAIN_PATH,FA_READ|FA_OPEN_EXISTING);
	if(Restatus != FR_OK){
		//�ļ���ʧ�ܣ������ڻ����ļ��쳣
		return FR_NO_PATH;
	}
	
	//��ȡ����
	Restatus = f_read(&UpanFL,FTPDownLoadData->tempData,FTP_TEMP_DOWNLOAD_BUF_LEN,&FTPDownLoadData->tempTotalLen);
	if(Restatus != FR_OK){
		f_close(&UpanFL);
		
		//�ļ���ȡʧ��
		return Restatus;
	}
	
	f_close(&UpanFL);
	
	//У���ļ�
	if(FTPDownLoadData->tempTotalLen >= FTP_TEMP_DOWNLOAD_BUF_LEN){
		//�ļ���С����
		return FR_INVALID_OBJECT;
	}
	
	//����CRCУ��
	CRC16Value = FTPDownLoadData->tempData[5]<<8 | FTPDownLoadData->tempData[4];
	if(CRC16Value != CRC16(FTPDownLoadData->tempData+8,FTPDownLoadData->tempTotalLen-8)){
		//�ļ�У�����
		return FR_INVALID_OBJECT;
	}
	
	//�ж��ļ���־�Ƿ���ȷ
	FileFlag = FTPDownLoadData->tempData[3]<<24 | FTPDownLoadData->tempData[2]<<16 | FTPDownLoadData->tempData[1]<<8 | FTPDownLoadData->tempData[0];
	if(FileFlag != (MAIN_FILE_FLAG ^ CRC16Value)){
		//�ļ���־����
		return FR_INVALID_OBJECT;
	}
	
	//�ļ���֤ͨ���������������
	FTPDownLoadData->flag = 0xA55A0FF0;
	FTPDownLoadData->crc16 = CRC16Value;
	FTPDownLoadData->totalLen = FTPDownLoadData->tempTotalLen-8;
	
	for(i=0;i<FTPDownLoadData->totalLen;i++){
		FTPDownLoadData->data[i] = FTPDownLoadData->tempData[8+i];
	}
	
	//д��SPI FLASH
	SPI4_ChangeModeTo(SPI_MODE_FLASH);
	
	if(SPIFlash_WriteData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)FTPDownLoadData,FTPDownLoadData->totalLen+12) == SUCCESS){
		//д��ɹ������¶�ȡ�������ж�CRC�Ƿ�һ��
		if(SPIFlash_DMA_ReadData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)FTPDownLoadData,FTPDownLoadData->totalLen+12) == SUCCESS){
			if(CRC16((uint8_t*)FTPDownLoadData->data,FTPDownLoadData->totalLen) != CRC16Value){
				SPI4_ExitModeToDefault();
				return FR_INVALID_PARAMETER;
			}
		}else{
			SPI4_ExitModeToDefault();
			return FR_INVALID_PARAMETER;
		}
	}else{
		//д��ʧ�ܣ���дһ��
		if(SPIFlash_WriteData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)FTPDownLoadData,FTPDownLoadData->totalLen+12) == SUCCESS){
			if(SPIFlash_DMA_ReadData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)FTPDownLoadData,FTPDownLoadData->totalLen+12) == SUCCESS){
				if(CRC16((uint8_t*)FTPDownLoadData->data,FTPDownLoadData->totalLen) != CRC16Value){
					SPI4_ExitModeToDefault();
					return FR_INVALID_PARAMETER;
				}
			}else{
				SPI4_ExitModeToDefault();
				return FR_INVALID_PARAMETER;
			}
		}else{
			SPI4_ExitModeToDefault();
			return FR_INVALID_PARAMETER;
		}
	}
	
	SPI4_ExitModeToDefault();
	
	//��EEPROM������ҪBOOT����MAIN�ı�־
	MachRunPara.update.needBootToUpdateMainFlag = UPDATE_MAIN_SOFTWARE_FLAG;
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_UPDATE_MAIN_SOFTWARE_FLAG,(uint8_t*)&MachRunPara.update.needBootToUpdateMainFlag,sizeof(MachRunPara.update.needBootToUpdateMainFlag));
	
	return FR_OK;
}



/***
*·��ת������·���е�/ת����\\��ע�ⲻҪ�ڴ������
*������
	DesPath��Ŀ��·��
	SrcPath��Դ·��
***/
static void PathChange(char* DesPath,char* SrcPath)
{
	uint8_t Len = 0;
	char* StrP1 = NULL;
	
	if(SrcPath == NULL){
		return;
	}
	
	StrP1 = strtok(SrcPath,"/");
	
	while(1){
		if(StrP1 == NULL){
			break;
		}
		
		Len += sprintf(DesPath+Len,"%s\\",StrP1);
		
		StrP1 = strtok(NULL,"/");
	}
	
	*(DesPath + Len - 1) = '\0';
	
	return;
}




/***
*U������UI--��ȡ��������Դ��NAND FLASH�ж�Ӧ·���£����ԣ��滻/�½���ʽ
������
	Path������������ļ�·��
����ֵ��
	FR_INVALID_NAME���������Ϸ�
	FR_INVALID_OBJECT���ļ�����Ŀ¼�쳣
	FR_DENIED���ļ�У��ͨ������д��Ŀ���豸�쳣
	FR_OK������
***/
static FRESULT Upan_UIReadFile2Nand(char* Path)
{
	FRESULT Restatus = FR_OK;
	FIL UpanFL = {0};
	uint8_t* UIBuf = (uint8_t*)SDRAM_ApplyCommonBuf(3*1024*1024);
	uint32_t Len = 0;
	uint32_t FileFlag = 0;
	uint16_t CRC16Value = 0;
	char NandPathTemp[100] = {0};
	char NandPath[100] = {0};
	
	
	//�ж����Ƿ��ǿ�·��
	if(Path == NULL){
		return FR_INVALID_NAME;
	}
	
	//�ж�·���Ƿ����ļ�·��
	if(strstr(Path,".") == NULL){
		return FR_INVALID_NAME;
	}
	
	Restatus = f_open(&UpanFL,Path,FA_READ|FA_OPEN_EXISTING);
	if(Restatus != FR_OK){
		//�ļ���ʧ�ܣ������ڻ����ļ��쳣
		return FR_INVALID_OBJECT;
	}
	
	//���BUF
	memset(UIBuf,0,3*1024*1024);
	
	//��ȡ����
	Restatus = f_read(&UpanFL,UIBuf,3*1024*1024,&Len);
	if(Restatus != FR_OK){
		f_close(&UpanFL);
		
		//�ļ���ȡʧ��
		return FR_INVALID_OBJECT;
	}
	
	f_close(&UpanFL);
	
	//У���ļ�
	if(Len >= 3*1024*1024){
		//�ļ���С����
		return FR_INVALID_OBJECT;
	}
	
	//����CRCУ��
	CRC16Value = UIBuf[5]<<8 | UIBuf[4];
	if(CRC16Value != CRC16(UIBuf+8,Len-8)){
		//�ļ�У�����
		return FR_INVALID_OBJECT;
	}
	
	//�ж��ļ���־�Ƿ���ȷ
	FileFlag = UIBuf[3]<<24 | UIBuf[2]<<16 | UIBuf[1]<<8 | UIBuf[0];
	if(FileFlag != (UI_FILE_FLAG ^ CRC16Value)){
		//�ļ���־����
		return FR_INVALID_OBJECT;
	}

	//�ļ�У��ͨ����д��NAND��Ӧ·����
	//·��ת������ƴ�ӳ�RL_FS�ļ�ϵͳ��·��
	sprintf(NandPathTemp,"N0:\\%s",(Path+6));
	PathChange(NandPath,NandPathTemp);
	
	if(FatFs_WriteNewFile((uint8_t*)NandPath,UIBuf+8,Len-8) == FEED_BACK_SUCCESS){
		//д��ɹ������¶�ȡ�������ж�CRC�Ƿ�һ��
		if(FatFs_ReadAllFile((uint8_t*)NandPath,UIBuf,&Len) == FEED_BACK_SUCCESS){
			if(CRC16(UIBuf,Len) != CRC16Value){
				return FR_DENIED;
			}else{
				return FR_OK;
			}
		}else{
			return FR_DENIED;
		}
	}else{
		//д��ʧ�ܣ���дһ��
		if(FatFs_WriteNewFile((uint8_t*)NandPath,UIBuf+8,Len-8) == FEED_BACK_SUCCESS){
			//д��ɹ������¶�ȡ�������ж�CRC�Ƿ�һ��
			if(FatFs_ReadAllFile((uint8_t*)NandPath,UIBuf,&Len) == FEED_BACK_SUCCESS){
				if(CRC16(UIBuf,Len) != CRC16Value){
					return FR_DENIED;
				}else{
					return FR_OK;
				}
			}else{
				return FR_DENIED;
			}
		}else{
			return FR_DENIED;
		}
	}
}




/***
*U������UI--������ѰU����ָ��Ŀ¼�µ�UI��Դ�ļ�
*������
	Path��·�������·��ֻ��ΪĿ¼·�����磺0:/Dir1������Ϊ�ļ�·��
����ֵ��
	FR_INVALID_NAME���������Ϸ�
	FR_INVALID_OBJECT���ļ�����Ŀ¼�쳣
	FR_DENIED���ļ�У��ͨ������д��Ŀ���豸�쳣
	FR_OK������
	FR_NO_PATH��U����UI��ԴĿ¼�����ڣ�������Ҫ��������
***/
static FRESULT Upan_UISearchFile(char* Path)
{
	FRESULT Restatus = FR_OK;
	DIR dp;
	FILINFO fno;
	char StrTemp[100] = {0};
	
	
	//�ж����Ƿ��ǿ�·��
	if(Path == NULL){
		return FR_INVALID_NAME;
	}
	
	//�ж�Ŀ¼·����ʽ�Ƿ���ȷ
	if(strstr(Path,".") != NULL){
		return FR_INVALID_NAME;
	}
	
	//�ж�Ŀ¼�Ƿ����
	Restatus = f_opendir(&dp,Path);
	if(Restatus != FR_OK){
		printf("��Ŀ¼ʧ�ܣ�%u\r\n",Restatus);
		return FR_NO_PATH;
	}
	
	//����
	while(1){
		//��ȡĿ¼��Ϣ
		Restatus = f_readdir(&dp,&fno);
		if(Restatus != FR_OK){
			printf("��ȡĿ¼��ʧ�ܣ�%u\r\n",Restatus);
			f_closedir(&dp);
			return FR_INVALID_OBJECT;
		}
		
		//��������ֻ����ASCII�������ַ�����Ӧ��ʮ�����ƿ϶�С��0x80�������������Ϊд������б��жϵ����ļ����ϣ��Ӷ���ȡ�쳣
		if(fno.fname[0] > 0x21 && fno.fname[0] <= 0x7a){
			if((fno.fattrib & AM_DIR) != 0){
				//��Ŀ¼
				memset(StrTemp,0,100);
				sprintf(StrTemp,"%s/%s",Path,fno.fname);
				
				//�ݹ����
				Restatus = Upan_UISearchFile(StrTemp);
				if(Restatus != FR_OK){
					printf("�ݹ���Ŀ¼���ļ�ʧ�ܣ�%u\r\n",Restatus);
					f_closedir(&dp);
					return FR_INVALID_OBJECT;
				}
			}else if((fno.fattrib & AM_ARC) != 0){
				//�ļ�
				//���ж��ļ�����ʽ�ַ����Ƿ���ȷ
				if(strstr(fno.fname,".") == NULL){
					printf("�ļ���\r\n");
					f_closedir(&dp);
					return FR_INVALID_OBJECT;
				}
				
				memset(StrTemp,0,100);
				sprintf(StrTemp,"%s/%s",Path,fno.fname);
				printf("�����ļ���%s\r\n",StrTemp);
				
				//��ȡ�ļ�����������NAND FLASH�ж�Ӧ·����
				Restatus = Upan_UIReadFile2Nand(StrTemp);
				if(Restatus != FR_OK){
					return Restatus;
				}
			}else{
				f_closedir(&dp);
				printf("�ļ���Ŀ¼��\r\n");
				return FR_INVALID_OBJECT;
			}
		}else if(fno.fname[0] == 0){
			//��ǰĿ¼������ϣ��˳�
			f_closedir(&dp);
			return FR_OK;
		}else{
			f_closedir(&dp);
			printf("�ļ���Ŀ¼�𻵣�%s\r\n",Path);
			return FR_INVALID_OBJECT;
		}
	}
}



/***
*U����������UI
����ֵ��
	FR_INVALID_NAME���������Ϸ�
	FR_INVALID_OBJECT���ļ�����Ŀ¼�쳣
	FR_DENIED���ļ�У��ͨ������д��Ŀ���豸�쳣
	FR_OK������
	FR_NO_PATH��U����UI��ԴĿ¼�����ڣ�������Ҫ��������
***/
FRESULT Upan_UpdateUI(void)
{
	return Upan_UISearchFile(UI_PATH);
}











