/*****************************************************************************
FatFS文件系统应用示例，测试的时候启动文件上写的栈空间和堆空间需要注意，防止溢出导致硬件中断。

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



/*宏定义区********************************************************************/
#define BOOT_FILE_FLAG							0x5AA50FF0								//BOOT程序文件标志
#define MAIN_FILE_FLAG							0xA55AF00F								//MAIN程序文件标志
#define UI_FILE_FLAG							0x0FF0A55A								//UI资源文件标志

#define BOOT_PATH								"0:/CX-2200_Boot.bin"					//定义BOOT程序文件在U盘中的路径
#define MAIN_PATH								"0:/CX-2200_Main.bin"					//定义Main程序文件在U盘中的路径
#define UI_PATH									"0:/UI"									//定义UI文件在U盘中的根目录

#define SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET		30*1024*1024							//定义用于在SPI_FLASH中存放升级包数据的起始偏移量


/*全局变量定义区***************************************************************/





/*外部函数区*******************************************************************/
/***
*挂载文件系统到设备上。文件系统挂载时会对SPI设备初始化
*参数：
	Label：要挂载文件系统的设备卷标号字符串，如：0:
	FS：挂载的设备全局句柄
	MkfsFlag：是否需要强制格式化标志
*返回值：文件操作的返回结果
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
	
	//挂载设备
	ResFatFs = f_mount(FS,Label,0);
	
	if(MkfsFlag == FORCE_MKFS){			//强制执行格式化操作
		//格式化设备
		ResFatFs = f_mkfs(Label,FM_ANY,0,Work,sizeof(Work));
		if(ResFatFs == FR_OK){
			printf("%s：格式化成功\r\n",Label);
			
			//格式化后，先取消挂载，然后重新挂载即可正常使用文件系统
			f_mount(NULL,Label,0);
			ResFatFs = f_mount(FS,Label,0);
		}else{
			printf("%s：格式化失败\r\n",Label);
			f_mount(NULL,Label,0);
			return FR_MKFS_ABORTED;
		}
	}else if(ResFatFs == FR_NO_FILESYSTEM){
		//因为涉及格式化操作，所以重新读取挂载，以便确认是否真的没有文件系统，防止误操作导致数据丢失
		if(f_mount(FS,Label,0) == FR_NO_FILESYSTEM){
			printf("%s：还没有文件系统，即将进行格式化...\r\n",Label);
		
			//格式化设备
			ResFatFs = f_mkfs(Label,0,0,Work,sizeof(Work));
			if(ResFatFs == FR_OK){
				printf("%s：格式化成功\r\n",Label);
				
				//格式化后，先取消挂载，然后重新挂载即可正常使用文件系统
				f_mount(NULL,Label,0);
				ResFatFs = f_mount(FS,Label,0);
			}else{
				printf("%s：格式化失败\r\n",Label);
				f_mount(NULL,Label,0);
				return FR_MKFS_ABORTED;
			}
		}else{
			//重启设备
			printf("%s：两次判断是否需要格式化操作不一致，即将重启\r\n",Label);
			
			__set_PRIMASK(1);
			NVIC_SystemReset();
		}
	}else if(ResFatFs != FR_OK){
		printf("%s：挂载文件系统失败。(%d)\r\n",Label,ResFatFs);
		printf("可能原因：设备初始化不成功。\r\n");
		return ResFatFs;
	}else{
		printf("%s：文件系统挂载成功\r\n",Label);
	}
	
	return ResFatFs;
}




/***
*获取设备容量信息
*参数：
	Label：要挂载文件系统的设备卷标号字符串，如：0:
	FreePlaceByte：将返回设备空闲空间字节数
	TotalPlaceByte：将返回设备总字节数
*返回值：文件操作的返回结果
***/
FRESULT FatFs_GetDiskInfo(char* Label,uint64_t* FreePlaceByte,uint64_t* TotalPlaceByte)
{
	unsigned long FreeClstNum = 0;				//用来保存可用的空闲簇数
	FATFS* Fs; 								//用来保存指定驱动器号对应的FatFS句柄参数，这里用来得出总簇数和每个簇包含的扇区数
	uint64_t FreeSectorNum = 0;				//空闲扇区数
	uint64_t TotalSectorNum = 0;			//总扇区数
	FRESULT ResFatFs = FR_OK;
	MSC_LUNTypeDef info;
	extern USBH_HandleTypeDef hUsbHostFS;
	
	if(Label == NULL){
		return FR_NO_PATH;
	}
	
	//读取U盘信息
	USBH_MSC_GetLUNInfo(&hUSB_Host, 0, &info);
	
	*FreePlaceByte = 0;
	*TotalPlaceByte = 0;
	
	//获取设备容量
	ResFatFs = f_getfree(Label,&FreeClstNum,&Fs);
	
	if(ResFatFs != FR_OK){
		return ResFatFs;
	}
	
	FreeSectorNum = FreeClstNum * Fs->csize;
	TotalSectorNum = (Fs->n_fatent - 2)*Fs->csize;
	
	//根据不同的设备设置的扇区大小得出对应的空闲字节数和总字节数,这里要乘以diskio.c中设置的GET_SECTOR_SIZE中设置的扇区大小值才是字节数
	if(strcmp(Label,U_PAN_LABEL) == 0){
		*FreePlaceByte = FreeSectorNum * info.capacity.block_size;
		*TotalPlaceByte = TotalSectorNum * info.capacity.block_size;
	}
	
	return ResFatFs;
}



/***
*创建多级目录
*参数：
	Path：要创建目录的路径，需要注意下路径不能太长，防止溢出，并且这个路径必须是一个完整的到文件结尾的路径，如：0:/Dir1/Dir2/Test.txt
*返回值：文件操作的返回结果
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
	
	//逐级创建目录
	StrP1 = strtok(DirTempPath1,"/");
	StrP2 = strtok(NULL,"/");
	if(StrP2 == NULL){
		printf("没有目录层级\r\n");
		return FR_INVALID_NAME;
	}
	
	Len += sprintf(DirTempPath2+Len,"%s",StrP1);			//这里获取到的是根目录，即0:
	
	while(1){
		StrP1 = strtok(NULL,"/");
		if(StrP1 == NULL){
			break;
		}
		
		Len += sprintf(DirTempPath2+Len,"/%s",StrP2);
		if(f_opendir(&dp,DirTempPath2) == FR_NO_PATH){
			//创建目录
			ResFatFs = f_mkdir(DirTempPath2);
			if(ResFatFs != FR_OK){
				printf("创建目录失败，退出\r\n");
				return ResFatFs;
			}
		}else{
			//关闭目录
			f_closedir(&dp);
		}
		
		StrP2 = strtok(NULL,"/");
		if(StrP2 == NULL){
			break;
		}
		Len += sprintf(DirTempPath2+Len,"/%s",StrP1);
		if(f_opendir(&dp,DirTempPath2) == FR_NO_PATH){
			//创建目录
			ResFatFs = f_mkdir(DirTempPath2);
			if(ResFatFs != FR_OK){
				printf("创建目录失败，退出\r\n");
				return ResFatFs;
			}
		}else{
			//关闭目录
			f_closedir(&dp);
		}
	}
	
	return FR_OK;
}




/***
*删除指定目录下的所有文件和子目录
*参数：
	Path：路径，这个路径只能为目录路径，不能带文件路径，如：0:/Dir1
*返回值：文件操作的返回结果
***/
FRESULT FatFs_UpanDelFileSubDir(char* Path)
{
	FRESULT ResFatFs = FR_OK;
	DIR dp;
	FILINFO fno;
	char StrTemp[100] = {0};
	
	//判断下是否是空路径
	if(Path == NULL){
		return FR_NO_PATH;
	}
	
	//判断路径形式是否正确
	if(strstr(Path,".") != NULL){
		return FR_INVALID_NAME;
	}
	
	//打开目录
	ResFatFs = f_opendir(&dp,Path);
	if(ResFatFs != FR_OK){
		printf("打开目录失败：%u\r\n",ResFatFs);
		return ResFatFs;
	}
	
	//遍历删除
	while(1){
		//读取目录信息
		ResFatFs = f_readdir(&dp,&fno);
		if(ResFatFs != FR_OK){
			f_closedir(&dp);
			printf("读取目录项失败：%u\r\n",ResFatFs);
			return ResFatFs;
		}
		
		//由于名称只采用ASCII，所以字符串对应的十六进制肯定小于0x80，否则可能是因为写入过程中被中断导致文件故障，从而读取异常
		if(fno.fname[0] > 0x21 && fno.fname[0] <= 0x7a){
			if((fno.fattrib & AM_DIR) != 0){
				//子目录，递归删除里面文件，然后再删除子目录
				memset(StrTemp,0,100);
				sprintf(StrTemp,"%s/%s",Path,fno.fname);
				ResFatFs = FatFs_UpanDelFileSubDir(StrTemp);
				if(ResFatFs != FR_OK){
					f_closedir(&dp);
					printf("递归删除子目录内文件失败：%u\r\n",ResFatFs);
					return ResFatFs;
				}
				
				printf("删除子目录：%s\r\n",StrTemp);
				ResFatFs = f_unlink(StrTemp);				//删除已经为空的子目录
				if(ResFatFs != FR_OK){
					f_closedir(&dp);
					printf("删除子目录失败：%u\r\n",ResFatFs);
					return ResFatFs;
				}
				
				//删除完子目录后，需要跳过此项检索，否则会返回FR_INVALID_OBJECT
				f_readdir(&dp,&fno);
			}else if((fno.fattrib & AM_ARC) != 0){
				//文件
				//先判断文件名格式字符串是否正确
				if(strstr(fno.fname,".") == NULL){
					printf("文件损坏，跳过\r\n");
					continue;
				}
				
				memset(StrTemp,0,100);
				sprintf(StrTemp,"%s/%s",Path,fno.fname);
				printf("删除文件：%s\r\n",StrTemp);
				
				//删除文件
				ResFatFs = f_unlink(StrTemp);				
				if(ResFatFs != FR_OK){
					f_closedir(&dp);
					printf("删除文件失败：%u\r\n",ResFatFs);
					return ResFatFs;
				}
			}else{
				printf("文件或目录损坏，跳过\r\n");
			}
		}else if(fno.fname[0] == 0){
			//删除完毕，退出
			f_closedir(&dp);
			return FR_OK;
		}else{
			printf("文件或目录损坏，跳过\r\n");
		}
	}
}



/***
*U盘升级BOOT
*返回值：文件操作的返回结果：
	FR_OK：表示文件正常写入
	FR_NO_PATH：表示没有文件，不需要升级，或者文件异常
	FR_INVALID_OBJECT：表示文件校验失败
	FR_INVALID_PARAMETER：表示文件检验通过，但写入失败
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
		//文件打开失败，不存在或者文件异常
		return FR_NO_PATH;
	}
	
	//读取数据
	Restatus = f_read(&UpanFL,BootBuf,1024*1024,&Len);
	if(Restatus != FR_OK){
		f_close(&UpanFL);
		
		//文件读取失败
		return Restatus;
	}
	
	f_close(&UpanFL);
	
	//校验文件
	if(Len >= 1024*1024){
		//文件大小超限
		return FR_INVALID_OBJECT;
	}
	
	//进行CRC校验
	CRC16Value = BootBuf[5]<<8 | BootBuf[4];
	if(CRC16Value != CRC16(BootBuf+8,Len-8)){
		//文件校验错误
		return FR_INVALID_OBJECT;
	}
	
	//判断文件标志是否正确
	FileFlag = BootBuf[3]<<24 | BootBuf[2]<<16 | BootBuf[1]<<8 | BootBuf[0];
	if(FileFlag != (BOOT_FILE_FLAG ^ CRC16Value)){
		//文件标志错误
		return FR_INVALID_OBJECT;
	}
	
	//文件验证通过，开始写入
	if(FLASH_WriteData_8(FLASH_BASE,BootBuf+8,Len-8) != HAL_OK){
		//再写入一次
		if(FLASH_WriteData_8(FLASH_BASE,BootBuf+8,Len-8) != HAL_OK){
			return FR_INVALID_PARAMETER;
		}else{
			//校验写入到FLASH中的数据
			if(CRC16((uint8_t*)FLASH_BASE,Len-8) != CRC16Value){
				//校验失败，再重新写入一次
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
		//校验写入到FLASH中的数据
		if(CRC16((uint8_t*)FLASH_BASE,Len-8) != CRC16Value){
			//校验失败，再重新写入一次
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
*U盘升级Main
*返回值：文件操作的返回结果：
	FR_OK：表示文件正常写入
	FR_NO_PATH：表示没有文件，不需要升级，或者文件异常
	FR_INVALID_OBJECT：表示文件校验失败
	FR_INVALID_PARAMETER：表示文件检验通过，但写入失败
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
		//文件打开失败，不存在或者文件异常
		return FR_NO_PATH;
	}
	
	//读取数据
	Restatus = f_read(&UpanFL,FTPDownLoadData->tempData,FTP_TEMP_DOWNLOAD_BUF_LEN,&FTPDownLoadData->tempTotalLen);
	if(Restatus != FR_OK){
		f_close(&UpanFL);
		
		//文件读取失败
		return Restatus;
	}
	
	f_close(&UpanFL);
	
	//校验文件
	if(FTPDownLoadData->tempTotalLen >= FTP_TEMP_DOWNLOAD_BUF_LEN){
		//文件大小超限
		return FR_INVALID_OBJECT;
	}
	
	//进行CRC校验
	CRC16Value = FTPDownLoadData->tempData[5]<<8 | FTPDownLoadData->tempData[4];
	if(CRC16Value != CRC16(FTPDownLoadData->tempData+8,FTPDownLoadData->tempTotalLen-8)){
		//文件校验错误
		return FR_INVALID_OBJECT;
	}
	
	//判断文件标志是否正确
	FileFlag = FTPDownLoadData->tempData[3]<<24 | FTPDownLoadData->tempData[2]<<16 | FTPDownLoadData->tempData[1]<<8 | FTPDownLoadData->tempData[0];
	if(FileFlag != (MAIN_FILE_FLAG ^ CRC16Value)){
		//文件标志错误
		return FR_INVALID_OBJECT;
	}
	
	//文件验证通过，将数据整理好
	FTPDownLoadData->flag = 0xA55A0FF0;
	FTPDownLoadData->crc16 = CRC16Value;
	FTPDownLoadData->totalLen = FTPDownLoadData->tempTotalLen-8;
	
	for(i=0;i<FTPDownLoadData->totalLen;i++){
		FTPDownLoadData->data[i] = FTPDownLoadData->tempData[8+i];
	}
	
	//写入SPI FLASH
	SPI4_ChangeModeTo(SPI_MODE_FLASH);
	
	if(SPIFlash_WriteData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)FTPDownLoadData,FTPDownLoadData->totalLen+12) == SUCCESS){
		//写入成功，重新读取出来，判断CRC是否一致
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
		//写入失败，重写一次
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
	
	//往EEPROM做好需要BOOT升级MAIN的标志
	MachRunPara.update.needBootToUpdateMainFlag = UPDATE_MAIN_SOFTWARE_FLAG;
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_UPDATE_MAIN_SOFTWARE_FLAG,(uint8_t*)&MachRunPara.update.needBootToUpdateMainFlag,sizeof(MachRunPara.update.needBootToUpdateMainFlag));
	
	return FR_OK;
}



/***
*路径转换，将路径中的/转换成\\，注意不要内存溢出了
*参数：
	DesPath：目标路径
	SrcPath：源路径
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
*U盘升级UI--读取并拷贝资源到NAND FLASH中对应路径下，策略：替换/新建方式
参数：
	Path：这里必须是文件路径
返回值：
	FR_INVALID_NAME：参数不合法
	FR_INVALID_OBJECT：文件或者目录异常
	FR_DENIED：文件校验通过，但写入目标设备异常
	FR_OK：正常
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
	
	
	//判断下是否是空路径
	if(Path == NULL){
		return FR_INVALID_NAME;
	}
	
	//判断路径是否是文件路径
	if(strstr(Path,".") == NULL){
		return FR_INVALID_NAME;
	}
	
	Restatus = f_open(&UpanFL,Path,FA_READ|FA_OPEN_EXISTING);
	if(Restatus != FR_OK){
		//文件打开失败，不存在或者文件异常
		return FR_INVALID_OBJECT;
	}
	
	//清空BUF
	memset(UIBuf,0,3*1024*1024);
	
	//读取数据
	Restatus = f_read(&UpanFL,UIBuf,3*1024*1024,&Len);
	if(Restatus != FR_OK){
		f_close(&UpanFL);
		
		//文件读取失败
		return FR_INVALID_OBJECT;
	}
	
	f_close(&UpanFL);
	
	//校验文件
	if(Len >= 3*1024*1024){
		//文件大小超限
		return FR_INVALID_OBJECT;
	}
	
	//进行CRC校验
	CRC16Value = UIBuf[5]<<8 | UIBuf[4];
	if(CRC16Value != CRC16(UIBuf+8,Len-8)){
		//文件校验错误
		return FR_INVALID_OBJECT;
	}
	
	//判断文件标志是否正确
	FileFlag = UIBuf[3]<<24 | UIBuf[2]<<16 | UIBuf[1]<<8 | UIBuf[0];
	if(FileFlag != (UI_FILE_FLAG ^ CRC16Value)){
		//文件标志错误
		return FR_INVALID_OBJECT;
	}

	//文件校验通过，写入NAND对应路径下
	//路径转换，并拼接成RL_FS文件系统的路径
	sprintf(NandPathTemp,"N0:\\%s",(Path+6));
	PathChange(NandPath,NandPathTemp);
	
	if(FatFs_WriteNewFile((uint8_t*)NandPath,UIBuf+8,Len-8) == FEED_BACK_SUCCESS){
		//写入成功，重新读取出来，判断CRC是否一致
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
		//写入失败，再写一次
		if(FatFs_WriteNewFile((uint8_t*)NandPath,UIBuf+8,Len-8) == FEED_BACK_SUCCESS){
			//写入成功，重新读取出来，判断CRC是否一致
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
*U盘升级UI--遍历搜寻U盘中指定目录下的UI资源文件
*参数：
	Path：路径，这个路径只能为目录路径，如：0:/Dir1，不能为文件路径
返回值：
	FR_INVALID_NAME：参数不合法
	FR_INVALID_OBJECT：文件或者目录异常
	FR_DENIED：文件校验通过，但写入目标设备异常
	FR_OK：正常
	FR_NO_PATH：U盘中UI资源目录不存在，即不需要升级操作
***/
static FRESULT Upan_UISearchFile(char* Path)
{
	FRESULT Restatus = FR_OK;
	DIR dp;
	FILINFO fno;
	char StrTemp[100] = {0};
	
	
	//判断下是否是空路径
	if(Path == NULL){
		return FR_INVALID_NAME;
	}
	
	//判断目录路径形式是否正确
	if(strstr(Path,".") != NULL){
		return FR_INVALID_NAME;
	}
	
	//判断目录是否存在
	Restatus = f_opendir(&dp,Path);
	if(Restatus != FR_OK){
		printf("打开目录失败：%u\r\n",Restatus);
		return FR_NO_PATH;
	}
	
	//遍历
	while(1){
		//读取目录信息
		Restatus = f_readdir(&dp,&fno);
		if(Restatus != FR_OK){
			printf("读取目录项失败：%u\r\n",Restatus);
			f_closedir(&dp);
			return FR_INVALID_OBJECT;
		}
		
		//由于名称只采用ASCII，所以字符串对应的十六进制肯定小于0x80，否则可能是因为写入过程中被中断导致文件故障，从而读取异常
		if(fno.fname[0] > 0x21 && fno.fname[0] <= 0x7a){
			if((fno.fattrib & AM_DIR) != 0){
				//子目录
				memset(StrTemp,0,100);
				sprintf(StrTemp,"%s/%s",Path,fno.fname);
				
				//递归遍历
				Restatus = Upan_UISearchFile(StrTemp);
				if(Restatus != FR_OK){
					printf("递归子目录内文件失败：%u\r\n",Restatus);
					f_closedir(&dp);
					return FR_INVALID_OBJECT;
				}
			}else if((fno.fattrib & AM_ARC) != 0){
				//文件
				//先判断文件名格式字符串是否正确
				if(strstr(fno.fname,".") == NULL){
					printf("文件损坏\r\n");
					f_closedir(&dp);
					return FR_INVALID_OBJECT;
				}
				
				memset(StrTemp,0,100);
				sprintf(StrTemp,"%s/%s",Path,fno.fname);
				printf("拷贝文件：%s\r\n",StrTemp);
				
				//读取文件，并拷贝到NAND FLASH中对应路径下
				Restatus = Upan_UIReadFile2Nand(StrTemp);
				if(Restatus != FR_OK){
					return Restatus;
				}
			}else{
				f_closedir(&dp);
				printf("文件或目录损坏\r\n");
				return FR_INVALID_OBJECT;
			}
		}else if(fno.fname[0] == 0){
			//当前目录遍历完毕，退出
			f_closedir(&dp);
			return FR_OK;
		}else{
			f_closedir(&dp);
			printf("文件或目录损坏：%s\r\n",Path);
			return FR_INVALID_OBJECT;
		}
	}
}



/***
*U盘升级更新UI
返回值：
	FR_INVALID_NAME：参数不合法
	FR_INVALID_OBJECT：文件或者目录异常
	FR_DENIED：文件校验通过，但写入目标设备异常
	FR_OK：正常
	FR_NO_PATH：U盘中UI资源目录不存在，即不需要升级操作
***/
FRESULT Upan_UpdateUI(void)
{
	return Upan_UISearchFile(UI_PATH);
}











