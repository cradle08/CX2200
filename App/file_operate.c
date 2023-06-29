
#include "main.h"
#include "file_operate.h"
#include "backend_msg.h"
#include "bsp_rtc.h"
#include "bsp_eeprom.h"
#include "printerTask.h"
#include "process.h"
#include "stdarg.h"
#include "time.h"
#include "msg_def.h"
#include "parameter.h"
#include "bsp_spi.h"
#include "crc16.h"
#include "Printer.h"
#include "Common.h"

#if USE_RL_FLASH_FS
#include "rl_fs.h"
#include "rl_fs_lib.h"   
#else
    #include "ff.h"
    #include "fatfs.h"
#endif


//开启标志后，如果出现数据校验失败，则会停止打印信息（仅做测试用）
#define SPI_FLASH_DATA_ERR_DEBUG    0




/* 
*   listview widget行序号0-6， 对应的数据下标号0-6，不一定是一一对应，中间相差无效数据个数。
*   按照 g_ucaListReview_DataTable[] 存放taListReview_data[]的下标，按照SN号大小递减排序。
*   listreview控件按照SN号降序显示有效数据（不显示无效数据）。
*/
uint8_t g_ucaListReview_DataTable[LIST_REVIEW_ROW_NUM] = {0}; //该数组存储的为 MachRunPara.taListReview_data[]数字的下标（按照SN号从大到小的下标）



/*
*  获取WBC HGB公共头 地址（spi存储方式）
*/
uint32_t Get_WBCHGB_Head_SPI_Addr(void)
{
    extern MachInfo_s MachInfo;
    uint32_t ulAddr = 0;
    
    if(ACCOUNT_DATA_TYPE_OUTSIDE == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType)
    {
        ulAddr = OUTSIDE_WBCHGB_HEAD_SPIFLASH_START_ADDR;
    }else{
        ulAddr = INNER_WBCHGB_HEAD_SPIFLASH_START_ADDR;
    }
    
    return ulAddr;    
}


/*
*   获取WBC HGB数据地址（spi存储方式）
*/
uint32_t Get_WBCHGB_Data_SPI_Addr(void)
{
    extern MachInfo_s MachInfo;
    uint32_t ulAddr = 0;
    
    if(ACCOUNT_DATA_TYPE_OUTSIDE == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType)
    {
        ulAddr = OUTSIDE_WBCHGB_DATA_SPIFLASH_START_ADDR;
    }else{
        ulAddr = INNER_WBCHGB_DATA_SPIFLASH_START_ADDR;
    }
    
    return ulAddr;
}


/*
*   获取计数日志地址（spi存储方式）
*/
uint32_t Get_WBCHGB_Log_SPI_Addr(void)
{
    extern MachInfo_s MachInfo;
    uint32_t ulAddr = 0;
    
    if(ACCOUNT_DATA_TYPE_OUTSIDE == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType)
    {
        ulAddr = OUTSIDE_WBCHGB_LOG_SPIFLASH_START_ADDR;
    }else{
        ulAddr = INNER_WBCHGB_LOG_SPIFLASH_START_ADDR;
    }
    
    return ulAddr;
}


/*
*   获取质控数据地址（spi存储方式）
*/
uint32_t Get_QC_WBCHGB_Data_SPI_Addr(QC_FILENUM_INDEX_e eFileNumIndex)
{
    uint32_t ulOff = 0;
    //偏移量
    ulOff = eFileNumIndex*QC_PER_FILE_NUM_MAX_CAPACITY*sizeof(QC_WBCHGB_TestInfo_t);
    
    return QC_DATA_SPIFLASH_START_ADDR+ulOff;
}

/*
*   获取运行日志地址（spi存储方式）
*/
uint32_t Get_Run_Log_SPI_Addr(void)
{
    return RUN_LOG_SPIFLASH_START_ADDR;
}




/* 用于测试读写速度 */
#define TEST_FILE_LEN			(4*1024)	/* 用于测试的文件长度 */
#define BUF_SIZE				(1024)		/* 每次读写S卡的最大数据长度 */


#if USE_RL_FLASH_FS
/* FlashFS API的返回值 */
const char * ReVal_Table[]= 
{
	"fsOK:Success",				                        
	"fsError：Not Specified Error",
	"fsUnsupported：Operate Not Suppert",
	"fsAccessDenied：Resure Access Rejuct",
	
	"fsInvalidParameter",
	"fsInvalidDrive: Dirver Invalid Or Not Exist",
	"fsInvalidPath: Path Invalid",
	"fsUninitializedDrive：UnInit Driver",

	"fsDriverError：Read/Write Error",
	"fsMediaError：Media Error",
	"fsNoMedia：Media Not Exist Or Not Init",
	"fsNoFileSystem：FS Not Format",

	"fsNoFreeSpace：Not Free Capacity",
	"fsFileNotFound",
	"fsDirNotEmpty",
	"fsTooManyOpenFiles",
};
#endif //USE_RL_FLASH_FS



/***
*文件系统初始化
***/
void FS_LowLevelInit(void)
{
	fsStatus res = fsOK;
	
	res = finit(DRV_PATH);
    if(res != NULL)
    {
        res = finit(DRV_PATH);
		
		if(res != NULL){
			//初始化失败，锁定程序
			while(1){
				LOG_Info("finit failure res=%d, %s", res, ReVal_Table[res]);
				HAL_Delay(1000);
			}
		}
    } 
}




/*
*   挂载文件系统
*/
FeedBack_e FS_Mount(FSMountLock_e FSMountLockState)
{
    fsStatus res = fsOK;

    res = fmount(DRV_PATH);
    if(fsOK != res)
    {
        osDelay(150);
        res = funmount(DRV_PATH);
        osDelay(150);
        res = fmount(DRV_PATH);
        if(fsOK != res)
        {
//			res = fformat(DRV_PATH, RL_FLASH_FS_NAND_FORMAT);
//			printf("文件系统格式化 res =%d, (%s)\r\n", res, ReVal_Table[res]);
//			
//			res = fmount(DRV_PATH);
//			if(fsOK != res)
//			{
//			   LOG_Info("fmount failure res=%d, %s", res, ReVal_Table[res]);
//			}else{
//			   LOG_Info("fmount success res=%d, %s", res, ReVal_Table[res]);
//			}
			
			LOG_Info("funmount failure res=%d, %s", res, ReVal_Table[res]);
			
			if(FSMountLockState == FS_MOUNT_LOCK){
				//挂载失败，锁定程序
				while(1){
					HAL_Delay(1000);
					LOG_Info("funmount failure res=%d, %s", res, ReVal_Table[res]);
				}
			}else{
				funmount(DRV_PATH);
				return FEED_BACK_ERROR;
			}
        }
    }
    return FEED_BACK_SUCCESS;
}


/***
*卸载文件系统
***/
FeedBack_e FS_Unmount(void)
{
	if(funmount(DRV_PATH) != NULL)
	{
		LOG_Error("funmount fail");
		return FEED_BACK_ERROR;
	}else{
		return FEED_BACK_SUCCESS;
	}
}



/*
*   文件系统格式化
*/
FeedBack_e FS_Format(void)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    fsStatus res = fsOK;
	
    res = finit(DRV_PATH);
    if(fsOK != res)
    {
       LOG_Info("finit failure res=%d, %s", res, ReVal_Table[res]);
    }else{
       LOG_Info("finit success res=%d, %s", res, ReVal_Table[res]);
    }
    //
    res = fformat(DRV_PATH, RL_FLASH_FS_NAND_FORMAT);
    if(fsOK != res)
    {
        eFeedBack = FEED_BACK_FAIL;
        printf("FS format res =%d, (%s)\r\n", res, ReVal_Table[res]);
    }
    //
    res = fmount(DRV_PATH);
    if(fsOK != res)
    {
       LOG_Info("fmount failure res=%d, %s", res, ReVal_Table[res]);
    }else{
       LOG_Info("fmount success res=%d, %s", res, ReVal_Table[res]);
    }    
    
    return eFeedBack;

}




/*
*********************************************************************************************************
*	函 数 名: DotFormatjavascript:;
*	功能说明: 将数据规范化显示，方便用户查看
*             比如
*             2345678   ---->  2.345.678
*             334426783 ---->  334.426.783
*             以三个数为单位进行显示
*	形    参: _ullVal   需要规范显示的数值
*             _sp       规范显示后数据存储的buf。
*	返 回 值: 无
*********************************************************************************************************
*/
static void DotFormat(uint64_t _ullVal, char *_sp) 
{
#if USE_RL_FLASH_FS
	/* 数值大于等于10^9 */
	if (_ullVal >= (uint64_t)1e9) 
	{
		_sp += sprintf (_sp, "%d.", (uint32_t)(_ullVal / (uint64_t)1e9));
		_ullVal %= (uint64_t)1e9;
		_sp += sprintf (_sp, "%03d.", (uint32_t)(_ullVal / (uint64_t)1e6));
		_ullVal %= (uint64_t)1e6;
		sprintf (_sp, "%03d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* 数值大于等于10^6 */
	if (_ullVal >= (uint64_t)1e6) 
	{
		_sp += sprintf (_sp,"%d.", (uint32_t)(_ullVal / (uint64_t)1e6));
		_ullVal %= (uint64_t)1e6;
		sprintf (_sp,"%03d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* 数值大于等于10^3 */
	if (_ullVal >= 1000) 
	{
		sprintf (_sp, "%d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* 其它数值 */
	sprintf (_sp,"%d",(uint32_t)(_ullVal));
#endif
}

/*
*	函 数 名: ViewDir,功能说明: 显示目录下的文件名
*/
static void ViewDir(char *pPath)
{
#if USE_RL_FLASH_FS
    
	uint8_t result;
	fsFileInfo info;
	uint8_t buf[15];
    char cDir[128] = {0};
	
    info.fileID = 0;   /* 每次使用ffind函数前，info.fileID必须初始化为0 */

	/* 加载SD卡 */
	result = fmount(DRV_PATH);
	
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("fmount Fail (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("fmount Success (%s)\r\n", ReVal_Table[result]);
	}
	
	printf("------------------------------------------------------------------\r\n");
	printf("FileName                 |  FileSize     | ID  | Attribute      |Date\r\n");
	
	/* 
	   将根目录下的所有文件列出来。
	   1. "*" 或者 "*.*" 搜索指定路径下的所有文件
	   2. "abc*"         搜索指定路径下以abc开头的所有文件
	   3. "*.jpg"        搜索指定路径下以.jpg结尾的所有文件
	   4. "abc*.jpg"     搜索指定路径下以abc开头和.jpg结尾的所有文件
	
	   以下是实现搜索根目录下所有文件
	*/
    sprintf(cDir, "%s*", pPath);
    printf("Dir:%s\r\n", cDir);
	while(ffind (cDir, &info) == 0)  
	{ 
		/* 调整文件显示大小格式 */
		DotFormat(info.size, (char *)buf);
		
		/* 打印根目录下的所有文件 */
		printf ("%-20s %12s bytes, ID: %04d  ",
				info.name,
				buf,
				info.fileID);
		
		/* 判断是文件还是子目录 */
		if (info.attrib & FS_FAT_ATTR_DIRECTORY)
		{
			printf("(0x%02x)Dir", info.attrib);
		}
		else
		{
			printf("(0x%02x)File", info.attrib);
		}
		
		/* 显示文件日期 */
		printf ("  %04d.%02d.%02d  %02d:%02d\r\n",
                 info.time.year, info.time.mon, info.time.day,
               info.time.hr, info.time.min);
    }
	
	if (info.fileID == 0)  
	{
		printf ("Not File\r\n");
	}

access_fail:
	/* 卸载SD卡 */
	result = funmount(DRV_PATH);
	if(result != NULL)
	{
		printf("funmount Fail\r\n");
	}
	else
	{
		printf("funmount Success\r\n");
	}
	
	printf("------------------------------------------------------------------\r\n");

#else //FATFS
    
#endif
    
}




/*
*   nand其容量显示
*/
static void View_Nand_Capacity(void)
{
#if USE_RL_FLASH_FS
    
	uint8_t result;
	fsMediaInfo info;
	uint64_t ullSdCapacity;
	int32_t id;  
	uint8_t buf[15];
	uint32_t ser_num;
	fsStatus restatus;
	
	/* 加载SD卡 */
	result = fmount(DRV_PATH);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("fmount Fail (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("fmount Success (%s)\r\n", ReVal_Table[result]);
	}
	
//	/* 格式化 */
//	printf("文件系统格式中......\r\n");
//	result = fformat ("N0:", RL_FLASH_FS_NAND_FORMAT);
//	printf("文件系统格式化 (%s)\r\n", ReVal_Table[result]);
	
	printf("------------------------------------------------------------------\r\n");
	
	/* 获取volume label */
	if (fvol (DRV_PATH, (char *)buf, &ser_num) == 0) 
	{
		if (buf[0]) 
		{
			printf ("NAND volume label %s\r\n", buf);
		}
		else 
		{
			printf ("NAND no volume label\r\n");
		}
		
		printf ("NAND volume serial number is %d\r\n", ser_num);
	}
	else 
	{
		printf ("Volume Access Error\r\n");
	}

	/* 获取SD卡剩余容量 */
	ullSdCapacity = ffree(DRV_PATH);
	DotFormat(ullSdCapacity, (char *)buf);
	printf("NAND Capacity = %10sByte\r\n", buf);
	
	/* 获取相应存储设备的句柄，注意挂载后操作下面的IO控制才是有效的 */
	id = fs_ioc_get_id(DRV_PATH);          
   
	/* 访问的时候要加上锁 */
	fs_ioc_lock (id);
	
	/* 初始化FAT文件系统格式的存储设备 */

	/* 获取存储设备的扇区信息 */
	restatus = fs_ioc_read_info (id, &info);
	if(restatus == fsOK)
	{
		/* 总的扇区数 * 扇区大小，SD卡的扇区大小是512字节 */
		ullSdCapacity = (uint64_t)info.block_cnt << 9;
		DotFormat(ullSdCapacity, (char *)buf);
		printf("NAND Capacity = %10sByte\r\nSD Total Sector = %d \r\n", buf, info.block_cnt);
	}
	else
	{
		printf("Get CFG Info %s\r\n", ReVal_Table[restatus]);
	}
	
	/* 访问结束要解锁 */
	fs_ioc_unlock (id);

	printf("NAND Sector Size = %dByte\r\n", info.read_blen);
	printf("NAND Sector Size = %dByte\r\n", info.write_blen);
	
	/* 卸载SD卡 */
	result = funmount(DRV_PATH);
	if(result != NULL)
	{
		printf("funmount Fail\r\n");
	}
	else
	{
		printf("funmount Success\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
#else //FATFS
    
#endif
    
}


/*
*	函 数 名: CreateNewFile
*	功能说明: 在SD卡创建三个text文本，分别使用fwrite，fprintf和fputs写数据。
*/
static void CreateNewFile(void)
{
#if USE_RL_FLASH_FS
    const uint8_t WriteText1[] = {"1111\r\n222\r\n33333\r\n44444"};
    const uint8_t WriteText[] = {"5555\r\n66666\r\n77777\r\n88888"};

	FILE *fout;
	uint32_t bw;
	uint32_t i = 2;
	uint8_t result;
    char cDir[128] = {0};

	/* 加载SD卡 */
	result = fmount(DRV_PATH);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("fmount Fail (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("fmount success (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test1.txt，如果没有子文件夹和txt文件会自动创建*/
    sprintf(cDir, "%s%s", DRV_PATH, "\\test\\test1.txt");
    printf("%s\r\n", cDir);
    
	fout = fopen (cDir, "w"); 
	if (fout != NULL) 
	{
		printf("open N0:\\test\\test1.txt success，if not exist subdir or txt file while will be create\r\n");
		/* 写数据 */
		bw = fwrite (WriteText, sizeof(uint8_t), sizeof(WriteText)/sizeof(uint8_t), fout);
		if(bw == sizeof(WriteText)/sizeof(uint8_t))
		{
			printf("write success\r\n");
		}
		else
		{ 
			printf("write fail\r\n");
		}
		
		/* 关闭文件 */
		fclose(fout);
	}
	else
	{
		printf("Open File N0:\\test\\test1.txt Fail\r\n");
	}
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test2.txt，如果没有子文件夹和txt文件会自动创建*/
    memset(cDir, 0, sizeof(cDir));
    sprintf(cDir, "%s%s", DRV_PATH, "\\test\\test2.txt");
    printf("%s\r\n", cDir);
    
	fout = fopen (cDir, "w"); 
	if (fout != NULL) 
	{
		printf("Open File N0:\\test\\test2.txt Success，If not exist dir or file will be create\r\n");
		
		/* 写数据 */
		bw = fprintf (fout, "%d %d %f\r\n", i, i*5, i*5.55f);
		
		/* 使用函数ferror检测是否发生过错误 */
		if (ferror(fout) != NULL)  
		{
			printf("write fail\r\n");
		}
		else
		{
			printf("write success\r\n");	
		}
		
		/* 关闭文件 */
		fclose(fout);
	}
	else
	{
		printf("open file N0:\\test\\test2.txt fail\r\n");
	}
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test3.txt，如果没有子文件夹和txt文件会自动创建*/
    memset(cDir, 0, sizeof(cDir));
    sprintf(cDir, "%s%s", DRV_PATH, "\\test\\test3.txt");
    printf("%s\r\n", cDir);
    
	fout = fopen (cDir, "w"); 
	if (fout != NULL) 
	{
		printf("open file N0:\\test\\test3.txt success，if not exist sub_dir or file will be create\r\n");
		
		/* 写数据 */
		fputs((const char *)WriteText1, fout);
		
		/* 使用函数ferror检测是否发生过错误 */
		if (ferror(fout) != NULL)  
		{
			printf("write fail\r\n");
		}
		else
		{
			printf("write success\r\n");	
		}
		
		/* 关闭文件 */
		fclose(fout);
	}
	else
	{
		printf("open file N0:\\test\\test3.txt fail\r\n");
	}

access_fail:	
	/* 卸载SD卡 */
	result = funmount(DRV_PATH);
	if(result != NULL)
	{
		printf("funmount fail\r\n");
	}
	else
	{
		printf("funmount success\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
    
#else //FATFS
    
#endif
}


/*
*	函 数 名: ReadFileData
*	功能说明: 分别使用fread，fscan和fgets读取三个不同的txt文件。
*	形    参: 无
*	返 回 值: 无
*/
static void ReadFileData(void)
{
#if USE_RL_FLASH_FS
	const uint8_t WriteText[] = {"1111\r\n222\r\33333\r\n44444"};
	uint8_t Readbuf[100];
	FILE *fin;
	uint32_t bw;
	uint32_t index1, index2;
	float  count = 0.0f;
	uint8_t result;
    char cDir[128] = {0};

	/* 加载SD卡 */
	result = fmount(DRV_PATH);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("fmount Fail (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("fmount Success (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test1.txt */

    sprintf(cDir, "%s%s", DRV_PATH, "\\test\\test1.txt");
    printf("%s\r\n", cDir);
	fin = fopen (cDir, "r"); 
	if (fin != NULL) 
	{
		printf("open file N0:\\test\\test1.txt success\r\n");
		
		/* 防止警告 */
		(void) WriteText;
		
		/* 读数据 */
		bw = fread(Readbuf, sizeof(uint8_t), sizeof(WriteText)/sizeof(uint8_t), fin);
		if(bw == sizeof(WriteText)/sizeof(uint8_t))
		{
			Readbuf[bw] = NULL;
			printf("test1.txt context: \r\n%s\r\n", Readbuf);
		}
		else
		{ 
			printf("read fail\r\n");
		}
		
		/* 关闭文件 */
		fclose(fin);
	}
	else
	{
		printf("open file N0:\\test\\test.txt fail, Maybe file not exist\n");
	}
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test2.txt */
    sprintf(cDir, "%s%s", DRV_PATH, "\\test\\test2.txt");
    printf("%s\r\n", cDir);
	fin = fopen (cDir, "r"); 
	if (fin != NULL) 
	{
		printf("\r\n Open file N0:\\test\\test2.txt success\r\n");
		
		bw = fscanf(fin, "%d %d %f", &index1, &index2, &count);

		/* 3参数都读出来了 */
		if (bw == 3)  
		{
			printf("Read Num\r\nindex1 = %d index2 = %d count = %f\r\n", index1, index2, count);
		}
		else
		{
			printf("read fail\r\n");	
		}
		
		/* 关闭文件 */
		fclose(fin);
	}
	else
	{
		printf("open file N0:\\test\\test.txt fail(%s)\r\n", ReVal_Table[result]);
	}
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test3.txt，如果没有子文件夹和txt文件会自动创建*/
    sprintf(cDir, "%s%s", DRV_PATH, "\\test\\test3.txt");
    printf("%s\r\n", cDir);
	fin = fopen (cDir, "r");
	if (fin != NULL) 
	{
		printf("\r\nopen file N0:\\test\\test3.txt success\r\n");
		
		/* 读数据 */
		if(fgets((char *)Readbuf, sizeof(Readbuf), fin) != NULL)
		{
			printf("test3.txt context: \r\n%s\r\n", Readbuf);
		}
		else
		{
			printf("Read Context Fail\r\n");
		}
		
		/* 关闭文件 */
		fclose(fin);
	}
	else
	{
		printf("Open File N0:\\test\\test.txt Fail\r\n");
	}
	
access_fail:
	/* 卸载SD卡 */
	result = funmount(DRV_PATH);
	if(result != NULL)
	{
		printf("funmount fail\r\n");
	}
	else
	{
		printf("funmount success\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
    
#else //FATFS
    
#endif
    
}



void Read_Test(uint32_t ulTestNum)
{

}

/*
*
*/
#define RW_WHILE_TEST_DATA_LEN  2048
void RW_While_Test(uint32_t ulTestNum)
{
#if USE_RL_FLASH_FS
    uint32_t ulIndex = 0, ulWFail_Num = 0, ulDataFail_Num = 0;
    uint16_t i = 0, result = 0, usLen = 0;
    FILE *Fin = NULL;
    uint8_t ucWriteBuffer[RW_WHILE_TEST_DATA_LEN] = {0};
    uint8_t ucReadBuffer[RW_WHILE_TEST_DATA_LEN] = {0};
    char cDir[36] = {0};
    
    //read write test
    for(ulIndex = 0; ulIndex < ulTestNum; ulIndex++)
    {    
        //create file
        if(fmount(DRV_PATH) == NULL)
        {
            LOG_Info("fmount ok");
        }else{
            LOG_Info("fmount fail");
            goto access_fail;
        }
        //create file
        sprintf(cDir, "%s%s", DRV_PATH, "\\Data\\Data_Text.txt");
        Fin = fopen(cDir, "r");
        if(Fin != NULL)
        {
            LOG_Info("Open %s Ok", cDir);
            Fin = fopen(cDir, "w");
            if(Fin == NULL) 
            {
                LOG_Info("create %s fail, exit", cDir);
            }      
        }else{
            LOG_Info("Open %s Fail", cDir);
        }
    
        //create data
        for(i = 0; i < RW_WHILE_TEST_DATA_LEN; i++)
        {
            ucWriteBuffer[i++] = rand()%256;
        }
        
        //write file
        usLen = fwrite(ucWriteBuffer, sizeof(uint8_t), sizeof(ucWriteBuffer), Fin);
        if(usLen != sizeof(ucWriteBuffer))
        {
            ulWFail_Num++;
            //ulOK_Num = 0, ulWFail_Num = 0, ulRFail_Num, ulDataFail_Num = 0;
        }else{
            LOG_Info("Write File Fail");
            goto access_fail;
        }
        
        fflush(Fin);
        fclose(Fin);
        
        HAL_Delay(20);
        
        //create file
        Fin = fopen(cDir, "r");
        if(Fin != NULL)
        {
            LOG_Info("Open %s Ok", cDir);
   
        }else{
            LOG_Info("Open %s Fail", cDir);
            goto access_fail;
        }
        
        //read
        usLen = fread(ucReadBuffer, sizeof(uint8_t), sizeof(ucReadBuffer), Fin);
        
        //compare Data
        for(i = 0; i < RW_WHILE_TEST_DATA_LEN; i++)
        {
            if(ucWriteBuffer[i] != ucReadBuffer[i])
            {
                ulDataFail_Num++;
            }
        }
        
        //
        fclose(Fin);
    }
    
access_fail:
	/* 卸载SD卡 */
	result = funmount(DRV_PATH);
	if(result != NULL)
	{
		printf("funmount fail\r\n");
	}
	else
	{
		printf("funmount success\r\n");
	} 
    
#else //FATFS
    
#endif
}





/*
*********************************************************************************************************
*	函 数 名: SeekFileData
*	功能说明: 创建一个text文本，在指定位置对其进行读写操作。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void SeekFileData(void)
{
#if USE_RL_FLASH_FS
	const uint8_t WriteText[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};
	FILE *fin, *fout;
	uint32_t bw;
	uint8_t ucChar;
	uint8_t result;

	/* 加载SD卡 */
	result = fmount(DRV_PATH);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("fmount Fail (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("Fmount Success (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test1.txt，如果没有子文件夹和txt文件会自动创建*/
	fout = fopen ("N0:\\test.txt", "w"); 
	if (fout != NULL) 
	{
		printf("Open File N0:\\test.txt Success，If txt Is Not Exit Will Be Create\r\n");
		/* 写数据 */
		bw = fwrite (WriteText, sizeof(uint8_t), sizeof(WriteText)/sizeof(uint8_t), fout);
		if(bw == sizeof(WriteText)/sizeof(uint8_t))
		{
			printf("Write Success\r\n");
		}
		else
		{ 
			printf("Write Fail\r\n");
		}
			
		/* 关闭文件 */
		fclose(fout);
	}
	else
	{
		printf("Open File N0:\\test.txt Fail\r\n");
	}
	
	/***********************************************/
	fin = fopen ("N0:\\test.txt","r");
	if (fin != NULL)  
	{
		printf("\r\nOpen File N0:\\test.txt Success\r\n");
		
		/* 读取文件test.txt的位置0的字符 */
		fseek (fin, 0L, SEEK_SET);
		ftell(fin); 	
		ucChar = fgetc (fin);		
//		printf("File test.txt Current Position：%02d CHAR %c\r\n", uiPos, ucChar);
		
		/* 读取文件test.txt的位置1的字符 */
		fseek (fin, 1L, SEEK_SET);
		ftell(fin); 	
		ucChar = fgetc (fin);		
//		printf("File test.txt Current Position %02d CHAR %c\r\n", uiPos, ucChar);

		/* 读取文件test.txt的位置25的字符 */
		fseek (fin, 25L, SEEK_SET);
		ftell(fin); 	
		ucChar = fgetc (fin);		
//		printf("File test.txt Current Position %02d CHAR %c\r\n", uiPos, ucChar);
		
		/* 通过上面函数的操作，当前读写位置是26
		   下面函数是在当前位置的基础上后退2个位置，也就是24，调用函数fgetc后，位置就是25
		 */
		fseek (fin, -2L, SEEK_CUR);
		ftell(fin); 	
		ucChar = fgetc (fin);		
//		printf("File test.txt Current Position %02d CHAR %c\r\n", uiPos, ucChar);
		
		/* 读取文件test.txt的倒数第2个字符, 最后一个是'\0' */
		fseek (fin, -2L, SEEK_END); 
		ftell(fin); 
		ucChar = fgetc (fin);
//		printf("File test.txt Current Position %02d CHAR %c\r\n", uiPos, ucChar);
		
		/* 将读取位置重新定位到文件开头 */
		rewind(fin);  
		ftell(fin); 
		ucChar = fgetc (fin);
//		printf("File test.txt Current Position %02d CHAR %c\r\n", uiPos, ucChar);	
		
		/* 
		   这里是演示一下ungetc的作用，此函数就是将当前的读取位置偏移回一个字符，
		   而fgetc调用后位置增加一个字符。
		 */
		fseek (fin, 0L, SEEK_SET);
		ucChar = fgetc (fin);
		ftell(fin); 
//		printf("File test.txt Current Position %02d CHAR %c\r\n", uiPos, ucChar);
		ungetc(ucChar, fin); 
		ftell(fin); 
//		printf("File test.txt Current Position %02d CHAR %c\r\n", uiPos, ucChar);
		
		/* 关闭*/
		fclose (fin);
	}
	else
	{
		printf("Open File N0:\\test.txt Fail\r\n");
	}
	
access_fail:	
	/* 卸载SD卡 */
	result = funmount(DRV_PATH);
	if(result != NULL)
	{
		printf("funmount fail\r\n");
	}
	else
	{
		printf("funmount success\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
    
#else //FATFS
    
#endif
	
}

/*
*********************************************************************************************************
*	函 数 名: DeleteDirFile
*	功能说明: 删除文件夹和文件
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DeleteDirFile(void)
{
#if USE_RL_FLASH_FS
	uint8_t result;
	uint8_t i;
	fsStatus restatus;
	char FileName[FILE_NAME_MAX_LEN];
	
	/* 加载SD卡 */
	result = fmount(DRV_PATH);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("fmount Fail (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("Fmount Success  (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/* 删除文件 speed1.txt */
	for (i = 0; i < 20; i++)
	{
		sprintf(FileName, "N0:\\Speed%02d.txt", i);		/* 每写1次，序号递增 */
		restatus = fdelete (FileName, NULL);
		if (restatus != NULL) 
		{
			printf("%s file not exist(Return Value %d)\r\n", FileName, result);
		}
		else
		{
			printf("Delete %s File Success\r\n", FileName);
		}
	}
	
	for(i = 1; i < 4; i++)
	{
		sprintf(FileName, "N0:\\test\\test%01d.txt", i);   /* 每写1次，序号递增 */
		restatus = fdelete (FileName, NULL);
		if (restatus != NULL) 
		{
			printf("%s file not exist(Return Value %d)\r\n", FileName, result);
		}
		else
		{
			printf("Delete %s File Success\r\n", FileName);
		}	
	}
	
	/* 删除文件夹test */
	restatus = fdelete ("N0:\\test\\", NULL);
	if (restatus != NULL) 
	{
		printf("test File Not Exist Or Empty(Return Value:%d)\r\n", result);
	}
	else
	{
		printf("Delete Test Dir Success\r\n");
	}
	
	/* 删除csv文件 */
	restatus = fdelete ("N0:\\record.csv", NULL);
	if (restatus != NULL) 
	{
		printf("record.csv File Not Exsit(Reture Value %d)\r\n", result);
	}
	else
	{
		printf("Delete record.csv Success\r\n");
	}
	
access_fail:
	/* 卸载SD卡 */
	result = funmount(DRV_PATH);
	if(result != NULL)
	{
		printf("funmount fail\r\n");
	}
	else
	{
		printf("funmount success\r\n");
	}
		
	printf("------------------------------------------------------------------\r\n");
#else //FATFS
    
#endif
}



/*
*********************************************************************************************************
*	函 数 名: WriteCSVFile
*	功能说明: 写数据到CSV文件中
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void WriteCSVFile(void)
{
#if USE_RL_FLASH_FS
	const uint8_t WriteText[] = {"record time, flag , value1, value2\r\n"};
	FILE *fout;
	uint8_t result;
	static uint16_t i = 1;
	static uint8_t ucFirstRunFlag = 0;

	/* 加载SD卡 */
	result = fmount(DRV_PATH);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("fmount Fail (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("Fmount Success  (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* 
	  1. 打开文件record.csv，如果没有此文件会自动创建。
	  2. 第二个参数表示向此文件写数据都从尾部开始添加。
	*/
	fout = fopen ("N0:\\record.csv", "a"); 
	if (fout != NULL) 
	{
		printf("open file N0:\\record.csv success，if file not exist will be create\r\n");
		
		/* 写数据，如果是第一次写数据，先写CSV文件的表项的题目，以后写数据不再需要写此表项 */
		if(ucFirstRunFlag == 0)
		{
			fprintf(fout, (char *)WriteText);
			ucFirstRunFlag = 1;
		}
		
		/* 依次写5行数据 */
		fprintf (fout, "%d, %d, %f\r\n", i, i*5, i*5.55f);
		
		i++;
		fprintf (fout, "%d, %d, %f\r\n", i, i*5, i*5.55f);
		
		i++;
		fprintf (fout, "%d, %d, %f\r\n", i, i*5, i*5.55f);
		
		i++;
		fprintf (fout, "%d, %d, %f\r\n", i, i*5, i*5.55f);
		
		i++;
		fprintf (fout, "%d, %d, %f\r\n", i, i*5, i*5.55f);
		i++;
		
		/* 使用函数ferror检测是否发生过错误 */
		if (ferror(fout) != NULL)  
		{
			printf("write fail\r\n");
		}
		else
		{
			printf("write success\r\n");	
		}
		
		/* 关闭文件 */
		fclose(fout);
	}
	else
	{
		printf("open file:\\record.csv failure\r\n");
	}

access_fail:	
	/* 卸载SD卡 */
	result = funmount(DRV_PATH);
	if(result != NULL)
	{
		printf("funmount fail\r\n");
	}
	else
	{
		printf("funmount success\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
#else //FATFS
    
#endif
}

/*
*********************************************************************************************************
*	函 数 名: ReadCSVFileData
*	功能说明: 读取CSV文件中的数据。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ReadCSVFileData(void)
{
#if USE_RL_FLASH_FS
	uint8_t Readbuf[50];
	FILE *fin;
	uint8_t result;

	/* 加载SD卡 */
	result = fmount(DRV_PATH);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("fmount Fail (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("Fmount Success  (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* 打开record.csv文件 */
	fin = fopen ("N0:\\record.csv", "r"); 
	if (fin != NULL) 
	{
		printf("open file N0:\\record.csv success\r\n");
		
		/* 将所有数据都读出来，使用fgets函数的好处就是可以一行一行的读取，因为此函数遇到'\n'换行符就会停止读取 */
		while (fgets((char *)Readbuf, sizeof(Readbuf), fin) != NULL)  
		{
			printf("%s", Readbuf);	
		}
		
		/* 使用函数ferror检测是否发生过错误 */
		if (ferror(fin) != NULL)  
		{
			printf("read fail\r\n");
		}
		else
		{
			printf("record.csv read success\r\n");	
		}
		
		/* 关闭文件 */
		fclose(fin);
	}
	else
	{
		printf("open file N0:\\record.csv fail\r\n");
	}
	
access_fail:
	/* 卸载SD卡 */
	result = funmount(DRV_PATH);
	if(result != NULL)
	{
		printf("funmount fail\r\n");
	}
	else
	{
		printf("funmount success\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
#else //FATFS
    
#endif

}






static void DispMenu(void)
{
	LOG_Info("\r\n------------------------------------------------");
	LOG_Info("CMD LIST");
	LOG_Info("0:Format");	
	LOG_Info("1: Capatity");
	LOG_Info("2 Dir /");
	LOG_Info("3 - creat 3file and write context");
	LOG_Info("4 - use 3 dif api read file\r\n");
	LOG_Info("5 - create file, fseek read/write");
	LOG_Info("6 - delete dir/file");
//	LOG_Info("7 - test read/write speed");
	LOG_Info("8 - write date to CSV");
	LOG_Info("9 - read CSV");
    LOG_Info("10 - read all files at Head/,Data/,Log/");
    LOG_Info("11 - dele all file at Head/,Data/,Log/");
    LOG_Info("12 - display cmd list");
}


/*
*********************************************************************************************************
*	函 数 名: DemoFlashFS
*	功能说明: FlashFS件系统演示主程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DemoFlashFS(uint8_t ucCmd)
{
#if USE_RL_FLASH_FS
	uint8_t cmd = ucCmd;
	uint8_t result;

	
	result = finit("N0:");
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("fmount Fail (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("fount fs success (%s)\r\n", ReVal_Table[result]);
	}
	
//	while(1)
//	{
//		if (comGetChar(COM1, &cmd))	/* 从串口读入一个字符(非阻塞方式) */
//		{
			printf("\r\n");
			switch (cmd)
			{
				case 1:
					printf("[1 - ViewSDCapacity]\r\n");
					View_Nand_Capacity();		/* SD卡的挂载，卸载及其容量显示 */
					break;
				
				case 2:
					printf("[2 - ViewRootDir]\r\n");
					ViewDir(".");		    /* 显示SD卡根目录下的文件名 */
					break;
				
				case 3:
					printf("[3 - CreateNewFile]\r\n");
					CreateNewFile();	    /* 创建三个text文本并使用不同函数写入内容 */
					break;
				
				case 4:
					printf("[4 - ReadFileData]\r\n");
					ReadFileData();	        /* 使用三个不同函数读取文本  */
					break;

				case 5:
					printf("[5 - SeekFileData]\r\n");
					SeekFileData();	        /* 创建一个text文本，在指定位置对其进行读写操作  */
					break;
				
				case 6:
					printf("[6 - DeleteDirFile]\r\n");
					DeleteDirFile();	    /* 删除文件夹和文件  */
					break;
				
				case 7:
					printf("[7 - WriteFileTest]\r\n");
					//WriteFileTest();	    /* 测试文件读写速度  */
					break;
				
				case 8:
					printf("[8 - WriteCSVFile]\r\n");
					WriteCSVFile();	        /* 写数据到CSV文件中  */
					break;
				
				case 9:
					printf("[9 - ReadCSVFileData]\r\n");
					ReadCSVFileData();	    /* 从CSV文件中读数据  */
					break;
                
                case 10:
                    Print_Dir("N0:\\0\\Data\\");
                    Print_Dir("N0:\\0\\Log\\");
                    Print_Dir("N0:\\0\\Head\\");
                    //
                    Print_Dir("N0:\\1\\Data\\");
                    Print_Dir("N0:\\1\\Log\\");
                    Print_Dir("N0:\\1\\Head\\");
                    break;
                
                case 11:
                    Del_Account_DataFile(ACCOUNT_DATA_TYPE_INNER);
                    Del_Account_DataFile(ACCOUNT_DATA_TYPE_OUTSIDE);
                    break;
                                
				case 12:
                {
                   /* 打印命令列表，用户可以通过串口操作指令 */
                    DispMenu();
                }
                break;
                case 13:
                   // Read_Test();
                    break;
				default:
					DispMenu();
					break;
			//}
		//}
	}
#else //FATFS
    
#endif
}





/********************************************************************************************************************************
********************************************************************************************************************************/

/*
*   删除不同类型账号对于的 数据文件
*/
FeedBack_e Del_Account_DataFile(AccountDataType_e eAccountDataType)
{
    char ucaFileName[FILE_NAME_MAX_LEN] = {0};
    
    if(ACCOUNT_DATA_TYPE_OUTSIDE == eAccountDataType)
    {
        //删除外部部账号（001/admin）产生的文件
        //delete Head
        memset(ucaFileName, 0, sizeof(ucaFileName));
        sprintf((char*)ucaFileName, "%s%s%s", (char*)DRV_PATH, ACCOUNT_DATA_TYPE_OUTSIDE_PATH, HEAD_DIR_PATH);
        Del_Dir_Files(ucaFileName);
        
        //delete Data
        memset(ucaFileName, 0, sizeof(ucaFileName));
        sprintf((char*)ucaFileName, "%s%s%s", (char*)DRV_PATH, ACCOUNT_DATA_TYPE_OUTSIDE_PATH, DATA_DIR_PATH);
        Del_Dir_Files(ucaFileName);
        
        //delete Log
        memset(ucaFileName, 0, sizeof(ucaFileName));
        sprintf((char*)ucaFileName, "%s%s%s", (char*)DRV_PATH, ACCOUNT_DATA_TYPE_OUTSIDE_PATH, LOG_DIR_PATH);
        Del_Dir_Files(ucaFileName); 
        
    }else{        
        //删除内部账号（工程师/测试账号）产生的文件
        //delete Head
        memset(ucaFileName, 0, sizeof(ucaFileName));
        sprintf((char*)ucaFileName, "%s%s%s", (char*)DRV_PATH, ACCOUNT_DATA_TYPE_INNER_PATH, HEAD_DIR_PATH);
        Del_Dir_Files(ucaFileName);
        
        //delete Data
        memset(ucaFileName, 0, sizeof(ucaFileName));
        sprintf((char*)ucaFileName, "%s%s%s", (char*)DRV_PATH, ACCOUNT_DATA_TYPE_INNER_PATH, DATA_DIR_PATH);
        Del_Dir_Files(ucaFileName);
        
        //delete Log
        memset(ucaFileName, 0, sizeof(ucaFileName));
        sprintf((char*)ucaFileName, "%s%s%s", (char*)DRV_PATH, ACCOUNT_DATA_TYPE_INNER_PATH, LOG_DIR_PATH);
        Del_Dir_Files(ucaFileName); 
    
    }
    
    return FEED_BACK_SUCCESS;
}




/*
*   查看指定目录下的文件
*/
void Print_Dir(char *pucaDir)
{
#if USE_RL_FLASH_FS    
    
    ViewDir(pucaDir);

#else

    FRESULT res;
    DIR dir;
    FILINFO fno;
    
    if(pucaDir == NULL)
    {
        LOG_Error("Input pucaDir is NULL");
        return;
    }
    
    //
    LOG_Info("ReadDir: %s", pucaDir);
    res = f_opendir(&dir, (char*) pucaDir);
    if(res == FR_OK)
    {
        while(1)
        {
            res = f_readdir(&dir, &fno);
            if(res != FR_OK || fno.fname[0] == 0) break;
            if(fno.fattrib & AM_DIR){
                //为子目录
    //            i = strlen(pucaDir);
    //            sprintf(&pucaDir[i], "/%s", fno.fname);
    //            res = Print_Dir(pucaDir)
            }else{
                LOG_Info("%s%s", pucaDir, fno.fname);
            }
        }
    }
    //
    f_closedir(&dir);
    
#endif
    
}



/*
*  删除 指定文件, pcFile为：路径和文件名，如：N0:\\Software\\MCU_V1.0.0_20210429.bin 或 0:/Software/MCU_V1.0.0_20210429.bin
*/
FeedBack_e Del_File(char *pcFile)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    
#if USE_RL_FLASH_FS
    uint8_t result = 0;  
    result = fmount(DRV_PATH);
    if(result != NULL)
    {
        eFeedBack = FEED_BACK_FAIL;
        LOG_Error("fmount Fail")
        result = funmount(DRV_PATH);
        if(result != NULL)
        {
            eFeedBack = FEED_BACK_FAIL;
            LOG_Error("funmount fail")
        }
    }
    
    //删除文件夹中所有文件
    if(fdelete(pcFile, NULL) != fsOK)
    {
        eFeedBack = FEED_BACK_FAIL;
        LOG_Error("fdelete %s Fail", pcFile);
        result = funmount(DRV_PATH);
        if(result != NULL)
        {
            eFeedBack = FEED_BACK_FAIL;
            LOG_Error("funmount fail")
        }
    }else{
        LOG_Info("Delete %s Success", pcFile);
    }
#else
    FRESULT result = FR_OK;
    
    if(pcFile != NULL)
    {
        result = f_unlink(pcFile);
        if(result != FR_OK)
        {
            LOG_Error("Delete %s Fail", pcFile);
            eFeedBack = FEED_BACK_FAIL;
        }
    }else{
        LOG_Error("File Is NULL");
        eFeedBack = FEED_BACK_FAIL;
    }
    
#endif
    return eFeedBack;
}




/*
*  删除 指定目录下的所有文件(不包括子文件夹). pucaDir为路径, 如：N0:\\Data\\ 或 0:/Data/
*/
FeedBack_e Del_Dir_Files(char *pcaDir)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    

#if USE_RL_FLASH_FS
    
//    uint8_t result = 0;
    
//    result = fmount(DRV_PATH);
//    if(result != NULL)
//    {
//        LOG_Error("fmount Fail")
//        eFeedBack = FEED_BACK_FAIL;
//        goto access_fail;
//    }
    
    //删除文件夹中所有文件
    if(fdelete(pcaDir, NULL) != fsOK)
    {
        eFeedBack = FEED_BACK_FAIL;
        LOG_Error("fdelete %s Fail", pcaDir);
//        goto access_fail;
    }else{
        LOG_Info("Delete %s Success", pcaDir);
    }
    
    
//access_fail:
//    result = funmount(DRV_PATH);
//    if(result != NULL)
//    {
//        eFeedBack = FEED_BACK_FAIL;
//        LOG_Error("funmount fail")
//    }
#else
    FRESULT res;
    DIR dir;
    FILINFO fno;
    
    //
    if(pcaDir == NULL)
    {
        LOG_Error("Input pucaDir is NULL");
        return FEED_BACK_FAIL;
    }
    
    //
    LOG_Info("ReadDir: %s", pcaDir);
    res = f_opendir(&dir, (char*) pcaDir);
    if(res == FR_OK)
    {
        while(1)
        {
            res = f_readdir(&dir, &fno);
            if(res != FR_OK || fno.fname[0] == 0) break;
            if(fno.fattrib & AM_DIR){
                //为子目录
            }else{
                f_unlink(fno.fname);
                LOG_Info("Delete %s Success", fno.fname);
            }
        }
    }
    //
    f_closedir(&dir);

#endif
    
    return eFeedBack;
}




/***
*删除指定目录下的所有文件和子目录
***/
FeedBack_e FatFs_DelFileSubDir(char* Path)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    
#if USE_RL_FLASH_FS
    
    if(fdelete(Path, "/S") == fsOK) 
    {      
        eFeedBack = FEED_BACK_FAIL;
        LOG_Error("Delete Dir %s Fail", Path);
    }

#else
    
	FRESULT FsReturn = FR_OK;
	DIR dp;
	FILINFO fno;
	char StrTemp[100] = {0};
	
	//判断下是否是空路径
	if(Path == NULL){
		eFeedBack = FEED_BACK_FAIL;;
	}
	
	//打开目录
	FsReturn = f_opendir(&dp,Path);
	if(FsReturn != FR_OK){
		eFeedBack = FEED_BACK_FAIL;
	}
	
	//遍历删除
	while(1){
		FsReturn = f_readdir(&dp,&fno);
		if(FsReturn != FR_OK){
			f_closedir(&dp);
			eFeedBack = FEED_BACK_FAIL;
		}
		
		if(fno.fname[0] != 0){
			if((fno.fattrib & AM_DIR) != 0){
				//子目录，递归删除里面文件
				memset(StrTemp,0,100);
				sprintf(StrTemp,"%s/%s",Path,fno.fname);
				eFeedBack = FatFs_DelFileSubDir(StrTemp);
				if(eFeedBack != FEED_BACK_SUCCESS){
					f_closedir(&dp);
					return eFeedBack;
				}
			}else{
				//文件
				memset(StrTemp,0,100);
				sprintf(StrTemp,"%s/%s",Path,fno.fname);
				FsReturn = f_unlink(StrTemp);
				if(FsReturn != FR_OK){
					f_closedir(&dp);
					eFeedBack = FEED_BACK_FAIL;
				}
			}
		}else{
			//删除完毕，退出
			f_closedir(&dp);
		}
	}
#endif
    
    return eFeedBack;
}






/*
*   Nand的容量信息, Nand的容量信息, pulTotalSect：总的扇区数，pulFreeSect：可用扇区数
*/
FeedBack_e Nand_Volume_Info(uint32_t *pulTotalSect, uint32_t *pulFreeSect)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    
#if USE_RL_FLASH_FS
    fsMediaInfo info;
    fsStatus result = 0;   
    uint32_t ulFreeBytes = 0;
    int32_t id;  
    
    result = fmount(DRV_PATH);
    if(result != fsOK)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("fmount Fail(%s)\r\n", ReVal_Table[result]);
        funmount(DRV_PATH);
        eFeedBack = FEED_BACK_FAIL;
	}
    
    //
    ulFreeBytes = ffree(DRV_PATH);
    id = fs_ioc_get_id(DRV_PATH); 
    fs_ioc_lock (id);
    result = fs_ioc_read_info (id, &info);
    fs_ioc_unlock (id);
    if(result == fsOK)
    {
        *pulTotalSect = info.block_cnt;
        *pulFreeSect  = ulFreeBytes/info.read_blen;
        LOG_Info("Nand TotalSector=%d, FreeSector=%d", *pulTotalSect, *pulFreeSect);
    }else{
        eFeedBack = FEED_BACK_FAIL;
        LOG_Error("Get info fail");
    }
    
    //
    result = funmount(DRV_PATH);
	if(result != NULL)
	{
		printf("funmount Fail\r\n");
	}
	else
	{
		printf("funmount Success\r\n");
	}
#else
    FATFS *pFile;
    DWORD clust, free_sect, tot_sect;

    /* 获取设备信息和空簇大小 */
	if(FR_OK != f_getfree(USERPath, &clust, &pFile))
    {
        LOG_Error(" f_getfree Error");
        eFeedBack = FEED_BACK_FAIL;
    }

    /* 计算得到总的扇区个数和空扇区个数 */
    tot_sect = (pFile->n_fatent - 2) * pFile->csize;
    free_sect = clust * pFile->csize;
    
    *pulTotalSect = tot_sect;
    *pulFreeSect = free_sect;
#endif
    
    return eFeedBack;
}






/***
*以新建的方式往指定路径写入指定buf的文件，当文件已经存在时，则覆盖旧文件
***/
FeedBack_e FatFs_WriteNewFile(uint8_t *pFileName, uint8_t *pWriteBuffer, uint32_t ulWriteLen)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    
    //nand写保护功能，关闭
    NAND_WP_OFF;
	HAL_Delay(2);
	
    if(pFileName != NULL && pWriteBuffer != NULL)
    {
        eFeedBack = FatFs_Write_File((uint8_t*)pFileName, 0, pWriteBuffer, ulWriteLen);
    }
	
	//nand写保护功能，开启
    NAND_WP_ON;
    return eFeedBack;
}




/***
*直接读取整个文件大小
***/
FeedBack_e FatFs_ReadAllFile(uint8_t *pFileName, uint8_t *pReadBuffer, uint32_t *pulReadLen)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    uint32_t ulLen = 0;
    uint32_t ulReadLen = 0;
    FILE *ptFile;
    fsFileInfo info;
    info.fileID = 0;
    
    if(FS_Mount(FS_MOUNT_NOT_LOCK) != FEED_BACK_SUCCESS){
		return FEED_BACK_FAIL;
	}
        
	//打开文件
    ptFile = fopen((char*)pFileName, "r"); //FA_OPEN_EXISTING，如果文件不存在，则会返回错误
	if(ptFile == NULL)
	{
        LOG_Error("Open %s Failure Or File Not Exist", pFileName);
        eFeedBack = FEED_BACK_FAIL;
        goto FatFs_ReadAllFileTag;
	}
	
    if(ffind((const char*)pFileName, &info) != fsOK)
    {
        LOG_Error("File %s Is Not Exist", pFileName);
        eFeedBack = FEED_BACK_FAIL;
		
		//关闭文件
		fclose(ptFile);
        goto FatFs_ReadAllFileTag;
    }  
    ulReadLen = info.size;
 
    
	//读取数据
	ulLen = fread(pReadBuffer, sizeof(uint8_t), ulReadLen, ptFile);
    if(ulReadLen != ulLen)
    {
        LOG_Error("Read %s Fail, Len=%d", pFileName, ulLen);
		
		//关闭文件
		fclose(ptFile);
        goto FatFs_ReadAllFileTag;
    }
    
    //读取长度
    *pulReadLen = ulLen;
	
	//关闭文件
	fclose(ptFile);
	
 FatFs_ReadAllFileTag:
    FS_Unmount();
    
    return eFeedBack;
}



/***
*判断指定路径下是否已经存在指定的文件
***/
FeedBack_e FatFs_IsExistFile(char* Path)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    fsFileInfo info;
	
    info.fileID = 0;
    
	//挂载
    if(FS_Mount(FS_MOUNT_NOT_LOCK) != FEED_BACK_SUCCESS){
		return FEED_BACK_ERROR;
	}
	
    //
    if(ffind(Path, &info) != fsOK)
    {
        LOG_Error("File %s Is Not Exist", Path);
       eFeedBack = FEED_BACK_FAIL;
    }  

    //卸载
    FS_Unmount();
    
    return eFeedBack;
}




/*
*	写文件
*/
FeedBack_e FatFs_Write_File(uint8_t *pFileName, uint32_t ulWritePos, uint8_t *pWriteBuffer, uint32_t ulWriteLen)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
	uint32_t ulLen = 0;
    FILE *ptFile;
    
	//挂载
    if(FS_Mount(FS_MOUNT_NOT_LOCK) != FEED_BACK_SUCCESS){
		return FEED_BACK_FAIL;
	}
    
//    ptFile = fopen((char*)pFileName, "r+");
//    if(ptFile == NULL)
//    {
//        ptFile = fopen((char*)pFileName, "w"); // 如果文件不存在，则创建文件
//    }
//    if(ptFile == NULL)
//    {
//        LOG_Info("Open %s Failure", pFileName);
//        eFeedBack = FEED_BACK_FAIL;
//        goto FatFs_Write_FileTag;
//    }
    
//    //写入位置
//    fseek(ptFile, ulWritePos, SEEK_SET);
	
	//直接以覆盖的方式打开
	ptFile = fopen((char*)pFileName, "wb+");
	if(ptFile == NULL){
		LOG_Info("Open %s Failure", pFileName);
        eFeedBack = FEED_BACK_FAIL;
        goto FatFs_Write_FileTag;
	}
    
    //写入结果数据
    ulLen = fwrite(pWriteBuffer, sizeof(uint8_t), ulWriteLen, ptFile);
   
	//结果
	if(ulWriteLen != ulLen)
	{
        LOG_Error("Write %s, Len %d != DataLen %d", pFileName, ulWriteLen, ulLen);
        eFeedBack = FEED_BACK_FAIL;
	}
		
	//关闭文件
    fflush(ptFile);
    fclose(ptFile);
    
FatFs_Write_FileTag:
    FS_Unmount();

    return eFeedBack;
}




/*
*	保存计数头信息（控制/状态信息）
*/
FeedBack_e FatFs_Write_CountHead(uint32_t ulIndex, CountHead_t *ptCountHead)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    uint16_t usLen = 0, index = 0, usWriteCrc = 0, usReadCrc = 0;
    uint32_t ulAddr = 0, ulTempAddr = 0;
	uint32_t BufSize = 20*1024;
    uint8_t *pSectorBuffer = (uint8_t*)SDRAM_ApplyCommonBuf(BufSize);
	uint8_t ucNum = 0;    
    
    usWriteCrc = ptCountHead->usCrc;
    //osKernelLock();
    SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
    do{
        usLen = 0, index = 0;
        ulAddr = 0, ulTempAddr = 0;
        //
        ulAddr = Get_WBCHGB_Head_SPI_Addr();
        ulAddr += ulIndex*sizeof(CountHead_t);
         //
        if((ulAddr+sizeof(CountHead_t))/SPIFLASH_SECTION_SIZE - (ulAddr/SPIFLASH_SECTION_SIZE))
        {
            //写入的数据跨越两个sector
            usLen = SPIFLASH_SECTION_SIZE*2;
        }else{
            //写入的数据在当前一个sector
            usLen = SPIFLASH_SECTION_SIZE;
        }
                
        //读取当前sector
        memset(pSectorBuffer, 0, BufSize);
        ulTempAddr = ulAddr - ulAddr%SPIFLASH_SECTION_SIZE;
        SPIFlash_DMA_ReadData(ulTempAddr, pSectorBuffer, usLen);
        //替换其中需写入的数据
        index = ulAddr%SPIFLASH_SECTION_SIZE;
        memmove((char*)&pSectorBuffer[index], (char*)ptCountHead, sizeof(CountHead_t));
        //回写数据 index
        //SPIFlash_WriteData(ulTempAddr, pSectorBuffer, usLen); 
        SPIFlash_DMA_WriteData(ulTempAddr, pSectorBuffer, usLen);    
        
         //读取刚刚写入的数据，重新计算对比CRC
        memset(pSectorBuffer, 0, BufSize);
        SPIFlash_DMA_ReadData(ulAddr, pSectorBuffer, sizeof(CountHead_t));
        CountHead_t *ptHead = (CountHead_t*)pSectorBuffer;
        
        usReadCrc = CRC16((uint8_t*)ptHead, sizeof(CountHead_t)-4);
        if(usWriteCrc == usReadCrc && usWriteCrc == ptHead->usCrc)
        {
            break;
        }else{
            HAL_Delay(3);
            LOG_Error("CRC Failure, ulIndex=%d, ncNum=%d, usWriteCrc=%d, usReadCrc=%d", ulIndex, ucNum, usWriteCrc, usReadCrc);
            eFeedBack = FEED_BACK_ERROR;
        } 
        //
        ucNum++;
    }while(ucNum < 3);
    SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
    //
    if(ucNum == 1 || ucNum == 2){
        //数据校验出错，但最终成功
        eFeedBack = FEED_BACK_ERROR;
        
    }else if(ucNum == 3){
        //数据校验出错，失败
        eFeedBack = FEED_BACK_FAIL; 
    }

    //用于内部工装测试，当时数据校验失败时，停止并打印信息
    #if SPI_FLASH_DATA_ERR_DEBUG
    if(ucNum >= 1)
    {
        while(1)
        {
            Print_Count_Head(ptCountHead);
            LOG_Info("---")
            Print_Count_Head((CountHead_t*)&pSectorBuffer[index]);
            osDelay(5000);
        }
    }
    #endif

    return eFeedBack;
}




/*
*	保存计数结果数据
*/
FeedBack_e FatFs_Write_WBCHGB_Info(uint32_t ulIndex, __IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    uint16_t usLen = 0, index = 0, usWriteCrc = 0, usReadCrc = 0;
    uint32_t ulAddr = 0, ulTempAddr = 0;
	uint32_t BufSize = 20*1024;
    uint8_t *pSectorBuffer = (uint8_t*)SDRAM_ApplyCommonBuf(BufSize);
	uint8_t ucNum = 0; 
    
    usWriteCrc = ptWBCHGB_TestInfo->usCrc;   
    SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
    do{
        usLen = 0, index = 0, usReadCrc = 0;
        ulAddr = 0, ulTempAddr = 0;
        //
        ulAddr = Get_WBCHGB_Data_SPI_Addr();
        ulAddr += ulIndex*sizeof(WBCHGB_TestInfo_t);
                 
        if((ulAddr+sizeof(WBCHGB_TestInfo_t))/SPIFLASH_SECTION_SIZE - (ulAddr/SPIFLASH_SECTION_SIZE))
        {
            //写入的数据跨越两个sector
            usLen = SPIFLASH_SECTION_SIZE*2;
        }else{
            //写入的数据在当前一个sector
            usLen = SPIFLASH_SECTION_SIZE;
        }
           
        //读取当前sector
        memset(pSectorBuffer, 0, BufSize);
        ulTempAddr = ulAddr - ulAddr%SPIFLASH_SECTION_SIZE; //4k对齐地址
        SPIFlash_DMA_ReadData(ulTempAddr, pSectorBuffer, usLen); //读取扇区
        //替换其中需写入的数据
        index = ulAddr%SPIFLASH_SECTION_SIZE;
        memmove((char*)&pSectorBuffer[index], (char*)ptWBCHGB_TestInfo, sizeof(WBCHGB_TestInfo_t));
        //回写数据 index
        //SPIFlash_WriteData(ulTempAddr, pSectorBuffer, usLen); 
        SPIFlash_DMA_WriteData(ulTempAddr, pSectorBuffer, usLen);     
        
        //读取刚刚写入的数据，重新计算对比CRC
        memset(pSectorBuffer, 0, BufSize);
        SPIFlash_DMA_ReadData(ulAddr, pSectorBuffer, sizeof(WBCHGB_TestInfo_t));
        WBCHGB_TestInfo_t *ptWBCHGB_Data = (WBCHGB_TestInfo_t*)pSectorBuffer;
        usReadCrc = CRC16((uint8_t*)&ptWBCHGB_Data->tMsgHead, sizeof(WBCHGB_TestInfo_t)-4);
        if(usWriteCrc == usReadCrc && usWriteCrc == ptWBCHGB_Data->usCrc)
        {
            break;
        }else{
            HAL_Delay(3);
            LOG_Error("CRC Failure, ulIndex=%d, ncNum=%d, usWriteCrc=%d, usReadCrc=%d", ulIndex, ucNum, usWriteCrc, usReadCrc);
        }
        ucNum++;
    }while(ucNum < 3);
    SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
    
    //
    if(ucNum == 1 || ucNum == 2){
        //数据校验出错，但最终成功
        eFeedBack = FEED_BACK_ERROR;
        
    }else if(ucNum == 3){
        //数据校验出错，失败
        eFeedBack = FEED_BACK_FAIL; 
    }

    //用于内部工装测试，当时数据校验失败时，停止并打印信息
#if SPI_FLASH_DATA_ERR_DEBUG
    if(ucNum >= 1)
    {
        while(1)
        {
            Print_WBCHGB_TestInfo(ptWBCHGB_TestInfo);
            LOG_Info("---")
            Print_WBCHGB_TestInfo((WBCHGB_TestInfo_t*)pSectorBuffer);
            osDelay(5000);
        }
    }
#endif

    return eFeedBack;
}



/*
*	保存质控 计数结果数据
*/
FeedBack_e FatFs_Write_QC_WBCHGB_Info(QC_FILENUM_INDEX_e eFileNumIndex, uint32_t ulIndex, __IO QC_WBCHGB_TestInfo_t *ptWBCHGB_TestInfo)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    
#if DATA_LOG_SAVE_TO_SPI_FLASH
    
    uint16_t usLen = 0, index = 0;
    uint32_t ulAddr = 0, ulTempAddr = 0, ulStartTick = 0;
	uint32_t BufSize = 20*1024;
    uint8_t *pSectorBuffer = (uint8_t*)SDRAM_ApplyCommonBuf(BufSize);
    
    //osKernelLock();
    SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
    ulAddr = Get_QC_WBCHGB_Data_SPI_Addr(eFileNumIndex);
    ulAddr += ulIndex*sizeof(WBCHGB_TestInfo_t);
     
    ulStartTick = HAL_GetTick();
    if((ulAddr+sizeof(WBCHGB_TestInfo_t))/SPIFLASH_SECTION_SIZE - (ulAddr/SPIFLASH_SECTION_SIZE))
    {
        //写入的数据跨越两个sector
        usLen = SPIFLASH_SECTION_SIZE*2;
    }else{
        //写入的数据在当前一个sector
        usLen = SPIFLASH_SECTION_SIZE;
    }
            
    //读取当前sector
    memset(pSectorBuffer, 0, BufSize);
    ulTempAddr = ulAddr - ulAddr%SPIFLASH_SECTION_SIZE; //4k对齐地址
    SPIFlash_DMA_ReadData(ulTempAddr, pSectorBuffer, usLen); //读取扇区
    //替换其中需写入的数据
    index = ulAddr%SPIFLASH_SECTION_SIZE;
    memmove((char*)&pSectorBuffer[index], (char*)ptWBCHGB_TestInfo, sizeof(WBCHGB_TestInfo_t));
    //回写数据 index
    //SPIFlash_WriteData(ulTempAddr, pSectorBuffer, usLen); 
    SPIFlash_DMA_WriteData(ulTempAddr, pSectorBuffer, usLen);     
    
    //
    SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
    //osKernelUnlock();
    LOG_Debug(" Index=%d, T=%d", ulIndex, HAL_GetTick() - ulStartTick);
   
    
#else //DATA_LOG_SAVE_TO_SPI_FLASH

    
    extern __IO MachRunPara_s MachRunPara;
    uint8_t ucaFileName[FILE_NAME_MAX_LEN] = {0};
    
	uint32_t ulFileSN = 0, ulWritePos = 0, ulSN = 0;
    //uint32_t ulStartTick = 0, ulEndTick = 0;
//    uint16_t ulWriteLen = 0;

    //
    ulSN = ulIndex;
    if(ulSN == INVALID_DATA_SN)
    {
        LOG_Error("Pameter ulSN is %X", ulSN);
        return FEED_BACK_FAIL;
    }

	//获取文件名
	ulFileSN= ulSN / DATA_FILE_RECORD_NUM;  //sn/250
    if(ACCOUNT_DATA_TYPE_OUTSIDE == MachRunPara.tAccountManage.eAccountDataType)
    {
        sprintf((char*)ucaFileName, "%s%s%s%s%04d%s", (char*)DRV_PATH, ACCOUNT_DATA_TYPE_OUTSIDE_PATH, DATA_DIR_PATH, DATA_FILE_PREFIX, ulFileSN, DATA_FILE_SUFFIX);	
    }else if(ACCOUNT_DATA_TYPE_INNER == MachRunPara.tAccountManage.eAccountDataType){
        sprintf((char*)ucaFileName, "%s%s%s%s%04d%s", (char*)DRV_PATH, ACCOUNT_DATA_TYPE_INNER_PATH, DATA_DIR_PATH, DATA_FILE_PREFIX, ulFileSN, DATA_FILE_SUFFIX);	
    }else{
        LOG_Error("NuKnown Account Data Type");
        return FEED_BACK_FAIL;
    }
    //LOG_Info("FileName:%s", (char*)ucaFileName);
   
    //ulWritePos
    ulWritePos = (ulSN % DATA_FILE_RECORD_NUM)* WBCHGB_TEST_INFO_LEN;

    //Write File
    eFeedBack = FatFs_Write_File(ucaFileName, ulWritePos, (uint8_t*)ptWBCHGB_TestInfo, WBCHGB_TEST_INFO_LEN);

#endif //DATA_LOG_SAVE_TO_SPI_FLASH

    return eFeedBack;

}




/*
*   写计数日志, 默认写入
*/
FeedBack_e FatFs_Write_ConutLog(uint32_t ulIndex, CountLog_t *ptCountLog, uint16_t usLogLen)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    uint16_t usLen = 0;
    uint32_t ulAddr = 0, ulTempAddr = 0;
	uint32_t BufSize = 20*1024;
    CountLog_t *ptRead_CountLog = (CountLog_t*)SDRAM_ApplyCommonBuf(BufSize);    
    uint16_t usReadCrc = 0;
    uint8_t ucNum = 0;    
   
    SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
    do{
        
        ulAddr = Get_WBCHGB_Log_SPI_Addr();
        ulAddr += ulIndex*COUNT_LOG_DEFAULT_MAX_LEN;
        usLen = SPIFLASH_SECTION_SIZE;
        ulTempAddr = ulAddr;
        SPIFlash_DMA_WriteData(ulTempAddr, (uint8_t*)ptCountLog, usLen);  
        
        //读取刚刚写入的数据，并校验 
        if(SUCCESS != SPIFlash_DMA_ReadData(ulAddr, (uint8_t*)ptRead_CountLog, usLen))
        {
            LOG_Error("SPI Read Count Log Error, ulIndex=%d", ulIndex);
            eFeedBack = FEED_BACK_FAIL;
        }
        //校验CRC
        usReadCrc = CRC16((uint8_t*)ptRead_CountLog->ucaLogBuffer, ptRead_CountLog->usLogLen);
        if(usReadCrc == ptRead_CountLog->usCrc)
        {
            break;
        }else{
            HAL_Delay(3);
            LOG_Error("CRC Failure, ulIndex=%d, ncNum=%d, DataCrc=%d, ReadCrc=%d", ulIndex, ucNum, ptRead_CountLog->usCrc, usReadCrc);
        }
        
        ucNum++;
        
    }while(ucNum < 3);
    SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();

    //
    if(ucNum == 1 || ucNum == 2){
        //数据校验出错，但最终成功
        eFeedBack = FEED_BACK_ERROR;
        
    }else if(ucNum == 3){
        //数据校验出错，失败
        eFeedBack = FEED_BACK_FAIL; 
    }
    
    //用于内部工装测试，当时数据校验失败时，停止并打印信息
    #if SPI_FLASH_DATA_ERR_DEBUG
    if(ucNum >= 1)
    {
        while(1)
        {
            LOG_Info("usReadCrc=%x", usReadCrc);
            Print_Count_Head(ptCountHead);
            osDelay(5000);
        }
    }
    #endif

    return eFeedBack;
}


/*
*  从文件(pFileName)的指定位置(ulReadPos)，读取指定长度(ulReadLen)的内容(pReadBuffer)
*/
FeedBack_e FatFs_Read_File(uint8_t *pFileName, uint32_t ulReadPos, uint8_t *pReadBuffer, uint32_t ulReadLen)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
     
 #if USE_RL_FLASH_FS
     uint32_t ulLen = 0, ulStartTick = 0, ulEndTick = 0;
    uint8_t ucNum = 3;
    FILE *ptFile;
    fsStatus result = fsOK;

//    Disable_FrontEnd_Task();
    
    do{
        /* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
        result = fmount(DRV_PATH);
        if(result != fsOK)
        {
            LOG_Error("fmount Fail(%s)\r\n", ReVal_Table[result]);
            funmount(DRV_PATH);
            funinit(DRV_PATH);
            osDelay(3);
            finit(DRV_PATH);
            if(result != fsOK)
            {
                LOG_Error("finit failure %s", ReVal_Table[result]);
                
            }
            if(result != fsOK && ucNum == 1)
            {
                LOG_Error("finit failure %s", ReVal_Table[result]);
//                Enable_FrontEnd_Task();
                return FEED_BACK_FAIL;
            }
        }else{
            break;
        }   
    }while(ucNum--);
        
//    //
    result = fmount(DRV_PATH);
    if(result != fsOK)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		LOG_Error("fmount (%s)\r\n", ReVal_Table[result]);
        eFeedBack = FEED_BACK_FAIL;
//        goto access_fail;
        
	}
    
	//打开文件
    ptFile = fopen((char*)pFileName, "r"); //FA_OPEN_EXISTING，如果文件不存在，则会返回错误
	if(ptFile == NULL)
	{
        LOG_Error("Open %s Failure Or File Not Exist", pFileName);
        eFeedBack = FEED_BACK_FAIL;
        goto access_fail;
	}
	
    //设置读取位置
    fseek(ptFile, ulReadPos, SEEK_SET);
    
	//读取数据
    ulStartTick = HAL_GetTick();
	ulLen = fread(pReadBuffer, sizeof(uint8_t), ulReadLen, ptFile);
    ulEndTick = HAL_GetTick();
    if(ulReadLen != ulLen)
    {
        LOG_Error("Read %s, Pos=%d, Len=%d, T=%d", pFileName, ulReadPos, ulLen, ulEndTick - ulStartTick);
    } else {
        LOG_Info("Read %s, Pos=%d, Len=%d, T=%d", pFileName, ulReadPos, ulLen, ulEndTick - ulStartTick);
    }
    
	
	//关闭文件
	fclose(ptFile);

 access_fail:
    if(funmount(DRV_PATH) != NULL)
    {
        LOG_Error("funmount fail");
    }   
		
//    Enable_FrontEnd_Task();
    
#else
    FIL tFile;
    FRESULT result = FR_OK;
    uint32_t ulLen = 0, ulStartTick = 0, ulEndTick = 0;
        
	//打开文件
    result = f_open(&tFile, (char*)pFileName, FA_OPEN_EXISTING | FA_WRITE | FA_READ); //FA_OPEN_EXISTING，如果文件不存在，则会返回错误
	if(FR_OK != result)
	{
		LOG_Info("Open %s Failure Or File Not Exist", pFileName);
        f_close(&tFile);
		return FEED_BACK_FAIL;
	}
	
    //设置读取位置
    f_lseek(&tFile, ulReadPos);
    
	//读取数据
    ulStartTick = HAL_GetTick();
	result = f_read(&tFile, pReadBuffer, ulReadLen, &ulLen);
    ulEndTick = HAL_GetTick();
    LOG_Info("Read %s, T=%d", pFileName, ulEndTick - ulStartTick);
	if(FR_OK != result)
	{
		LOG_Info("Read %s Failure, Pos=%d, Len=%d", pFileName, ulReadPos, ulLen);
        f_close(&tFile);
		return FEED_BACK_FAIL;
	}
	
	//关闭文件
	f_close(&tFile);
		
#endif
	return eFeedBack;
    
}




/*
*   缓冲日志（缓存日志到buffer中）, 返回值=增加的长度
*   pLogBuffer=日志缓存，usIndex=新日志起点
*/
#define MAX_LINE_LOG_LEN    512
uint16_t Count_Log_Output(LogType_e eLogType, __IO uint8_t *pLogBuffer, uint16_t usIndex, const char *format, ...)
{
    va_list args;
    int fmt_res = 0;
    char cLineBuffer[MAX_LINE_LOG_LEN] = {0};
    uint16_t i = 0, usMaxLen = 0;
    
    //获取一行格式化日志
    va_start(args, format);
    fmt_res =vsnprintf(cLineBuffer, MAX_LINE_LOG_LEN, format, args);
    if((fmt_res > -1) && (fmt_res <= LOG_BUFFER_MAX_LEN-1))
    {
        
    }else{
        fmt_res = MAX_LINE_LOG_LEN;
    }
    va_end(args);
    
    //最大长度限制
    if(LOG_TYPE_COUNT_LOG == eLogType || LOG_TYPE_RUN == eLogType || LOG_TYPE_OPERATE == eLogType || LOG_TYPE_PARAM_SET == eLogType)
    {
        usMaxLen = LOG_BUFFER_MAX_LEN;//
    }else{
        return 0;
    }
    
    //
    if(usIndex + fmt_res > usMaxLen)
    {
        LOG_Error("More Than Max Log Length");
        return 0;
    }
    
    //将一行日志，复制到日志buffer中
    if(fmt_res > 0)
    {
        for(i = 0; i < fmt_res; i++)
        {
            *(pLogBuffer+usIndex+i) = cLineBuffer[i];
        }
    }

    return fmt_res;
}



///*
//*   缓冲日志（缓存日志到buffer中）, 返回值=增加的长度
//*   pLogBuffer=日志缓存，usIndex=新日志起点
//*/
//#define MAX_LINE_RRROR_LOG_LEN    256
//uint16_t Error_Log_Output(uint8_t *pLogBuffer, uint16_t usIndex, const char *format, ...)
//{
//    va_list args;
//    int fmt_res = 0;
//    int len = 0;
//    char cLineBuffer[MAX_LINE_RRROR_LOG_LEN] = {0};
//    uint16_t i = 0;
//    uint32_t ulMaxLen = 0;
//    
//    //获取一行格式化日志
//    va_start(args, format);
//    fmt_res =vsnprintf(cLineBuffer, MAX_LINE_RRROR_LOG_LEN, format, args);
//    if((fmt_res > -1) && (fmt_res <= MAX_LINE_RRROR_LOG_LEN-1))
//    {
//        len = fmt_res;
//    }else{
//        fmt_res = MAX_LINE_RRROR_LOG_LEN;
//    }
//    va_end(args);
//    
//    //最大长度限制
////    if(LOG_TYPE_COUNT_LOG == eLogType || LOG_TYPE_RUN == eLogType || LOG_TYPE_OPERATE == eLogType || LOG_TYPE_PARAM_SET == eLogType)
////    {
//        ulMaxLen = ERROR_INFO_LEN;//
////    }else{
////        return 0;
////    }
//    
//    //
//    if(usIndex + fmt_res > ulMaxLen)
//    {
//        LOG_Error("More Than Max Log Length");
//        return 0;
//    }
//    
//    //将一行日志，复制到日志buffer中
//    if(fmt_res > 0)
//    {
//        for(i = 0; i < fmt_res; i++)
//        {
//            *(pLogBuffer+usIndex+i) = cLineBuffer[i];
//        }
//    }

//    return fmt_res;
//}


/*
*	 读取指定的系列号的测试结果（WBC HGB）, 每250条测试记录保存到一个文件中
*	0-249 :   保存到Data_0000.txt
*	250-499 : 保存到Data_0001.txt
*	500-749 : 保存到Data_0002.txt
*	750-999 : 保存到Data_0003.txt
*   ...
*/
FeedBack_e FatFs_Read_QC_WBCHGB_Info(QC_FILENUM_INDEX_e eFileNumIndex, uint32_t ulIndex, __IO QC_WBCHGB_TestInfo_t *ptWBCHGB_TestInfo)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
#if DATA_LOG_SAVE_TO_SPI_FLASH
    uint32_t ulAddr = 0, ulStartTick = 0;  
    
    //osKernelLock();
    SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
    ulAddr = Get_QC_WBCHGB_Data_SPI_Addr(eFileNumIndex);
    ulAddr = ulAddr + ulIndex*sizeof(WBCHGB_TestInfo_t);
    
    ulStartTick = HAL_GetTick();
    if(SUCCESS != SPIFlash_DMA_ReadData(ulAddr, (uint8_t*)ptWBCHGB_TestInfo, sizeof(WBCHGB_TestInfo_t)))
    {
        LOG_Error("SPI Read WBCHGB Error, ulIndex=%d", ulIndex);
        eFeedBack = FEED_BACK_FAIL;
    }
    SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
    //osKernelUnlock();
    LOG_Debug("T=%d", HAL_GetTick()-ulStartTick);
    
#else
    extern __IO MachRunPara_s MachRunPara;
	//
	uint32_t ulFileSN = 0, ulPos = 0, ulSN = 0;
	uint8_t ucaDataFileName[FILE_NAME_MAX_LEN] = {0};
//    uint32_t ulFirstSN = INVALID_DATA_SN, ulLastestSN = INVALID_DATA_SN, ulLastestValidSN = INVALID_DATA_SN;

//    ulFirstSN = Get_FirstSN();
//    ulLastestSN = Get_LastestSN();
//    ulLastestValidSN = Get_LastestValidSN();
//	
//	//检测SN
//	if((ulFirstSN > ulSN &&  ulLastestSN < ulSN) || ulLastestValidSN == INVALID_DATA_SN)
//	{
//        LOG_Error("no WBCHGB test info vailable");
//        return FEED_BACK_FAIL;
//	}
    
    //
    ulSN = ulIndex;
    if(ulSN == INVALID_DATA_SN)
    {
        LOG_Error("Pameter ulSN is %X", ulSN);
        return FEED_BACK_FAIL;
    }


	//获取文件名
	//LOG_Info("ulDataSN=%d", ulSN);
	ulFileSN= ulSN / DATA_FILE_RECORD_NUM; //sn/250
    
    if(ACCOUNT_DATA_TYPE_OUTSIDE == MachRunPara.tAccountManage.eAccountDataType)
    {
        sprintf((char*)ucaDataFileName, "%s%s%s%s%04d%s", (char*)DRV_PATH, ACCOUNT_DATA_TYPE_OUTSIDE_PATH, DATA_DIR_PATH, DATA_FILE_PREFIX, ulFileSN, DATA_FILE_SUFFIX);        
    }else if(ACCOUNT_DATA_TYPE_INNER == MachRunPara.tAccountManage.eAccountDataType){
        sprintf((char*)ucaDataFileName, "%s%s%s%s%04d%s", (char*)DRV_PATH, ACCOUNT_DATA_TYPE_INNER_PATH, DATA_DIR_PATH, DATA_FILE_PREFIX, ulFileSN, DATA_FILE_SUFFIX);
    }
    //LOG_Info(" FileName:%s", (char*)ucaDataFileName);   
	
	//找到数据的起始位置
	ulPos = (ulSN % DATA_FILE_RECORD_NUM)*WBCHGB_TEST_INFO_LEN; //数据定长
    
    //
    eFeedBack = FatFs_Read_File(ucaDataFileName, ulPos, (uint8_t*)ptWBCHGB_TestInfo, sizeof(WBCHGB_TestInfo_t));
#endif
    
    return eFeedBack;

}

/*
*	 读取指定的系列号的测试结果（WBC HGB）, 每250条测试记录保存到一个文件中
*	0-249 :   保存到Data_0000.txt
*	250-499 : 保存到Data_0001.txt
*	500-749 : 保存到Data_0002.txt
*	750-999 : 保存到Data_0003.txt
*   ...
*/
FeedBack_e FatFs_Read_WBCHGB_Info(uint32_t ulIndex, __IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    uint16_t usReadCrc = 0;
    uint32_t ulAddr = 0;  
    uint8_t ucNum = 0;
    
    SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
    do{
        ulAddr = Get_WBCHGB_Data_SPI_Addr();
        ulAddr = ulAddr + ulIndex*sizeof(WBCHGB_TestInfo_t);    
        //
        if(SUCCESS != SPIFlash_DMA_ReadData(ulAddr, (uint8_t*)ptWBCHGB_TestInfo, sizeof(WBCHGB_TestInfo_t)))
        {
            LOG_Error("SPI Read WBCHGB Error, ulIndex=%d", ulIndex);
            eFeedBack = FEED_BACK_FAIL;
        }
        
        //校验CRC
        usReadCrc = CRC16((uint8_t*)&ptWBCHGB_TestInfo->tMsgHead, sizeof(WBCHGB_TestInfo_t)-4);
        if(usReadCrc == ptWBCHGB_TestInfo->usCrc)
        {
            break;
        }else{
            HAL_Delay(3);
            LOG_Error("CRC Failure, ulIndex=%d, ncNum=%d, DataCrc=%d, ReadCrc=%d", ulIndex, ucNum, ptWBCHGB_TestInfo->usCrc, usReadCrc);
        }
        ucNum++;   
    }while(ucNum < 3);
    SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
    
    //
    if(ucNum == 1 || ucNum == 2){
        //数据校验出错，但最终成功
        eFeedBack = FEED_BACK_ERROR;
        
    }else if(ucNum == 3){
        //数据校验出错，失败
        eFeedBack = FEED_BACK_FAIL; 
    }
    
    //用于内部工装测试，当时数据校验失败时，停止并打印信息
    #if SPI_FLASH_DATA_ERR_DEBUG
    if(ucNum >= 1)
    {
        while(1)
        {
            LOG_Info("usReadCrc=%x", usReadCrc);
            Print_WBCHGB_TestInfo(ptWBCHGB_TestInfo);
            osDelay(5000);
        } 
    }
    #endif
    
    return eFeedBack;
}






/*
*	 读取指定的系列号的测试控制信息, 每250条测试记录保存到一个文件中
*	0-249 :   保存到Head_0000.txt
*	250-499 : 保存到Hata_0001.txt
*	500-749 : 保存到Hata_0002.txt
*	750-999 : 保存到Hata_0003.txt
*   ...
*/
FeedBack_e FatFs_Read_ConutHead(uint32_t ulIndex, CountHead_t *ptCountHead)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    uint32_t ulAddr = 0;
    uint16_t usReadCrc = 0;
    uint8_t ucNum = 0;
    
    SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
    do{
        ulAddr = Get_WBCHGB_Head_SPI_Addr();
        ulAddr = ulAddr + ulIndex*sizeof(CountHead_t);
        if(SUCCESS != SPIFlash_DMA_ReadData(ulAddr, (uint8_t*)ptCountHead, sizeof(CountHead_t)))
        {
            LOG_Error("SPI Read Count Log Error, ulIndex=%d", ulIndex);
            eFeedBack = FEED_BACK_FAIL;
        }
        //校验CRC
        usReadCrc = CRC16((uint8_t*)ptCountHead, sizeof(CountHead_t)-4);
        if(usReadCrc == ptCountHead->usCrc)
        {
            break;
        }else{
            HAL_Delay(3);
            LOG_Error("CRC Failure, ulIndex=%d, ncNum=%d, DataCrc=%d, ReadCrc=%d", ulIndex, ucNum, ptCountHead->usCrc, usReadCrc);
        }
        
        ucNum++;
    }while(ucNum < 3);
    SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();

    if(ucNum == 1 || ucNum == 2){
        //数据校验出错，但最终成功
        eFeedBack = FEED_BACK_ERROR;
        
    }else if(ucNum == 3){
        //数据校验出错，失败
        eFeedBack = FEED_BACK_FAIL; 
    }
    
    //用于内部工装测试，当时数据校验失败时，停止并打印信息
    #if SPI_FLASH_DATA_ERR_DEBUG
    if(ucNum >= 1)
    {
        while(1)
        {
            LOG_Info("usReadCrc=%x", usReadCrc);
            Print_Count_Head(ptCountHead);
            osDelay(5000);
        }
    }
    #endif

    return eFeedBack;
}




/*
*	 读取指定的系列号的日志信息, 每250条测试记录保存到一个文件中
*	0-249 :   保存到Log_0000.txt
*	250-499 : 保存到Log_0001.txt

ulSN:日志系列号，ulPos:当前日志在文件中的位置， pLogBuffer：读取日志的缓存，usLen:读取日志的长度
*   ...
*/
FeedBack_e FatFs_Read_ConutLog(uint32_t ulIndex, __IO CountLog_t *ptCountLog, uint16_t usLen)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    uint32_t ulAddr = 0;
    uint16_t usReadCrc = 0;
    uint8_t ucNum = 0;
    
    SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
    do{
        ulAddr = Get_WBCHGB_Log_SPI_Addr();
        ulAddr = ulAddr + ulIndex*COUNT_LOG_DEFAULT_MAX_LEN;
        
        if(SUCCESS != SPIFlash_DMA_ReadData(ulAddr, (uint8_t*)ptCountLog, usLen))
        {
            LOG_Error("SPI Read Count Log Error, ulIndex=%d", ulIndex);
        }
        //校验CRC
        usReadCrc = CRC16((uint8_t*)ptCountLog->ucaLogBuffer, ptCountLog->usLogLen);
        if(usReadCrc == ptCountLog->usCrc)
        {
            break;
        }else{
            HAL_Delay(3);
            LOG_Error("CRC Failure, ulIndex=%d, ncNum=%d, DataCrc=%d, ReadCrc=%d", ulIndex, ucNum, ptCountLog->usCrc, usReadCrc);
        }
        
        ucNum++;
        
    }while(ucNum < 3);
    SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();

    //
    if(ucNum == 1 || ucNum == 2){
        //数据校验出错，但最终成功
        eFeedBack = FEED_BACK_ERROR;
        
    }else if(ucNum == 3){
        //数据校验出错，失败
        eFeedBack = FEED_BACK_FAIL; 
    }
    
    //用于内部工装测试，当时数据校验失败时，停止并打印信息
    #if SPI_FLASH_DATA_ERR_DEBUG
    if(ucNum >= 1)
    {
        while(1)
        {
            LOG_Info("usReadCrc=%x", usReadCrc);
            Print_Count_Head(ptCountHead);
            osDelay(5000);
        }
    }
    #endif

    return eFeedBack;
}



/*
读取运行日志头和字符串内容
*/
FeedBack_e FatFs_Read_RunLog(uint32_t ulIndex, RunLog_s* RunLog)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS; 
#if DATA_LOG_SAVE_TO_SPI_FLASH
    uint32_t ulAddr = 0, ulStartTick = 0;
    uint16_t usReadCrc = 0;
    uint8_t ucNum = 0;
    
    ulStartTick = HAL_GetTick();
    SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
    do{
        
        ulAddr = Get_Run_Log_SPI_Addr();
        ulAddr = ulAddr + ulIndex*sizeof(RunLog_s);
        if(SUCCESS != SPIFlash_DMA_ReadData(ulAddr, (uint8_t*)RunLog, sizeof(RunLog_s)))
        {
            LOG_Error("SPI Read Run Log Error, ulIndex=%d", ulIndex);
//            eFeedBack = FEED_BACK_FAIL;
        }
        //校验CRC
        usReadCrc = CRC16((uint8_t*)RunLog, sizeof(RunLog_s)-2);
        if(usReadCrc == RunLog->usCrc)
        {
            break;
        }else{
            HAL_Delay(3);
            LOG_Error("CRC Failure, ulIndex=%d, ncNum=%d, DataCrc=%d, ReadCrc=%d", ulIndex, ucNum, RunLog->usCrc, usReadCrc);
        }
        
        ucNum++;
    }while(ucNum < 3);
    SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
    LOG_Debug(" Index=%d, T=%d", ulIndex, HAL_GetTick() - ulStartTick);

    //
    if(ucNum == 1 || ucNum == 2){
        //数据校验出错，但最终成功
        eFeedBack = FEED_BACK_ERROR;
        
    }else if(ucNum == 3){
        //数据校验出错，失败
        eFeedBack = FEED_BACK_FAIL; 
    }
    
    //用于内部工装测试，当时数据校验失败时，停止并打印信息
    #if SPI_FLASH_DATA_ERR_DEBUG
    if(ucNum >= 1)
    {
        while(1)
        {
            LOG_Info("usReadCrc=%x", usReadCrc);
            Print_Count_Head(ptCountHead);
            osDelay(5000);
        }
    }
    #endif
  
#else 
	uint32_t ulPos = 0, ulSN = 0;
	uint8_t ucaFileName[FILE_NAME_MAX_LEN] = {0};

    //
    ulSN = ulIndex;
    if(ulSN == INVALID_DATA_SN)
    {
        LOG_Error("Pameter ulSN is %X", ulSN);
        return FEED_BACK_FAIL;
    }
    
    ulPos = ulSN * RUN_LOG_MAX_LEN;  //sn/250
    //获取运行日志文件名
    sprintf((char*)ucaFileName, "%s%s%s%s", (char*)DRV_PATH, OTHERS_DIR_PATH, LOG_RUN_HEAD_FILE_PREFIX, LOG_RUN_STR_FILE_PREFIX);
    LOG_Info("ucaFileName:%s", (char*)ucaFileName);
            
    //
    eFeedBack = FatFs_Read_File(ucaFileName, ulPos, (uint8_t*)RunLog, sizeof(RunLog_s));

#endif //DATA_LOG_SAVE_TO_SPI_FLASH
    return eFeedBack;
}







/***
*保存运行日志
***/
FeedBack_e Fatfs_Write_RunLog(uint32_t ulIndex, RunLog_s* RunLog)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    
    uint16_t usLen = 0, index = 0;
    uint32_t ulAddr = 0, ulTempAddr = 0;
	uint32_t BufSize = 20*1024;
    uint8_t *pSectorBuffer = (uint8_t*)SDRAM_ApplyCommonBuf(BufSize);    
    uint16_t usReadCrc = 0;
    uint8_t ucNum = 0;
    
    SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
    do{
        ulAddr = Get_Run_Log_SPI_Addr();
        ulAddr += ulIndex*sizeof(RunLog_s);
           
        if((ulAddr+sizeof(RunLog_s))/SPIFLASH_SECTION_SIZE - (ulAddr/SPIFLASH_SECTION_SIZE))
        {
            //写入的数据跨越两个sector
            usLen = SPIFLASH_SECTION_SIZE*2;
        }else{
            //写入的数据在当前一个sector
            usLen = SPIFLASH_SECTION_SIZE;
        }
                
        //读取当前sector
        memset(pSectorBuffer, 0, BufSize);
        ulTempAddr = ulAddr - ulAddr%SPIFLASH_SECTION_SIZE;
        SPIFlash_DMA_ReadData(ulTempAddr, pSectorBuffer, usLen);
        //替换其中需写入的数据
        index = ulAddr%SPIFLASH_SECTION_SIZE;
        memmove((char*)&pSectorBuffer[index], (char*)RunLog, sizeof(RunLog_s));
        //回写数据 index
        //SPIFlash_WriteData(ulTempAddr, pSectorBuffer, usLen); 
        SPIFlash_DMA_WriteData(ulTempAddr, pSectorBuffer, usLen); 
        
        //读取刚刚写入的数据
        memset(pSectorBuffer, 0, BufSize);
        SPIFlash_DMA_ReadData(ulAddr, pSectorBuffer, sizeof(RunLog_s));
        usReadCrc = CRC16(pSectorBuffer, sizeof(RunLog_s) - 2);
        if(usReadCrc == RunLog->usCrc)
        {
            break;
        }else{
            HAL_Delay(3);
            LOG_Error("CRC Failure, ulIndex=%d, ncNum=%d, DataCrc=%d, ReadCrc=%d", ulIndex, ucNum, RunLog->usCrc, usReadCrc);
        }
        ucNum++;
    }while(ucNum < 3);
    //
    SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
    
        //
    if(ucNum == 1 || ucNum == 2){
        //数据校验出错，但最终成功
        eFeedBack = FEED_BACK_ERROR;
        
    }else if(ucNum == 3){
        //数据校验出错，失败
        eFeedBack = FEED_BACK_FAIL; 
    }
    
    //用于内部工装测试，当时数据校验失败时，停止并打印信息
    #if SPI_FLASH_DATA_ERR_DEBUG
    if(ucNum >= 1)
    {
        while(1)
        {
            LOG_Info("usReadCrc=%x", usReadCrc);
            Print_Count_Head(ptCountHead);
            osDelay(5000);
        }
    }
    #endif
    return eFeedBack;
}






/*
*   更新g_ucListReview_DataTab[]，存储对应的taListView_Data[]下标号（按有效的SN系列号的降序，对应的下标号）
*/
void Update_ListReview_DataTable(void)
{
    extern __IO MachRunPara_s MachRunPara;
    int i = 0, j = 0, m = 0;
    char buffer[LIST_REVIEW_ROW_NUM];
    uint32_t ulSNBuffer[LIST_REVIEW_ROW_NUM], ulTempSN = 0;
//    WM_HWIN hItem;
    
    //init
    for(i = 0; i < LIST_REVIEW_ROW_NUM; i++)
    {
        buffer[i]     = i;
        ulSNBuffer[i] = MachRunPara.taListView_Data[i].ulCurDataSN;
    }
    //排序，按SN号大小递减
    for(i = 0; i < LIST_REVIEW_ROW_NUM-1; i++)
    {
        for(j = i+1; j < LIST_REVIEW_ROW_NUM; j++)
        {
            if(ulSNBuffer[i] <= ulSNBuffer[j]) //降序
            {
                ulTempSN = ulSNBuffer[i];
                ulSNBuffer[i] = ulSNBuffer[j];
                ulSNBuffer[j] = ulTempSN;
                
                m = buffer[i];
                buffer[i] = buffer[j];
                buffer[j] = m;
            }else{
                if((ulSNBuffer[j] != INVALID_DATA_SN) && (ulSNBuffer[i] == ulSNBuffer[j]))
                {   //当出现SN相等情况时，异常处理
                    if(MachRunPara.taListView_Data[ulSNBuffer[i]].timestamp <  MachRunPara.taListView_Data[ulSNBuffer[j]].timestamp)
                    {
                        ulTempSN = ulSNBuffer[i];
                        ulSNBuffer[i] = ulSNBuffer[j];
                        ulSNBuffer[j] = ulTempSN;
                        
                        m = buffer[i];
                        buffer[i] = buffer[j];
                        buffer[j] = m;
                    } 
                }                
            }
        }
    }
    //把临时表给全局表赋值
    for(i = 0; i < LIST_REVIEW_ROW_NUM; i++)
    {
        g_ucaListReview_DataTable[i] = buffer[i];
    }      
}








/*
*   查找g_ucaListReview_DataTable是否存在Init化的数据，存在，
*   则返回第一个为初始化数据的g_ucaListReview_DataTable数据表中的下标，不存在，返回LIST_REVIEW_ROW_NUM
*/
uint8_t ListReview_InitData_Index(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern  uint8_t g_ucaListReview_DataTable[LIST_REVIEW_ROW_NUM];
    
    uint8_t i = 0;
    
    for(i = 0; i < LIST_REVIEW_ROW_NUM; i++)
    {
        if(DATA_STATUS_INIT == MachRunPara.taListView_Data[g_ucaListReview_DataTable[i]].eDataStatus && \
            INVALID_DATA_SN == MachRunPara.taListView_Data[g_ucaListReview_DataTable[i]].ulCurDataSN)
        {
            return i;
        }
    }
    
    return LIST_REVIEW_ROW_NUM;
}


/*
*   获取list数据中，有效数数据的个数
*/
uint8_t ListReview_ValidData_Num(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern  uint8_t g_ucaListReview_DataTable[LIST_REVIEW_ROW_NUM];
    
    uint8_t i = 0, index = 0;
    
    for(i = 0; i < LIST_REVIEW_ROW_NUM; i++)
    {
        if(DATA_STATUS_INIT != MachRunPara.taListView_Data[g_ucaListReview_DataTable[i]].eDataStatus && \
            INVALID_DATA_SN != MachRunPara.taListView_Data[g_ucaListReview_DataTable[i]].ulCurDataSN)
        {
             index++;
        }
    }
    return index;
}




/*
*   返回 g_ucaListView_DataTable[]中，有效数据中最旧（小）SN号 对于的下标号, 如果不存在有效数据，则返回 LIST_REVIEW_ROW_NUM
*                                            
*/
uint8_t ListReview_DataTable_MinSN_Index(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern uint8_t g_ucaListReview_DataTable[LIST_REVIEW_ROW_NUM];
    
    //g_ucaListView_DataTable[]中的数据已按照SN降序, 一般来说最后一个数据为最小SN
    if(MachRunPara.taListView_Data[g_ucaListReview_DataTable[LIST_REVIEW_ROW_NUM-1]].ulCurDataSN == INVALID_DATA_SN)
    {
        //如果最后一个数据为无效数据(则所有数据均为无效数据)
        return LIST_REVIEW_ROW_NUM;
    }else{
        //最后一个数据为正常数据
        return LIST_REVIEW_ROW_NUM - 1;
    }
}


/*
*   返回 g_ucaListView_DataTable[]中，有效数据中最新（大）SN号 对于的下标号, 如果不存在有效数据，则返回 LIST_REVIEW_ROW_NUM
*                                            
*/
uint8_t ListReview_DataTable_MaxSN_Index(void)
{
    extern __IO MachRunPara_s MachRunPara;
    extern uint8_t g_ucaListReview_DataTable[LIST_REVIEW_ROW_NUM];
    uint8_t i = 0;
    
    //g_ucaListView_DataTable[]中的数据已按照SN降序, 一般来说第一个数据为最小SN
    for(i = 0; i < LIST_REVIEW_ROW_NUM; i++)
    {
        if(MachRunPara.taListView_Data[g_ucaListReview_DataTable[i]].ulCurDataSN != INVALID_DATA_SN)
        {
            //查找第一个有效数据
            return i;
        }
    }
    
    //
    return LIST_REVIEW_ROW_NUM;
}



   


/*
*	UI数据（信息）结构体  ，usCmd 命令，ucIndex: 列表回顾界面，列表控件下标（列表回顾数据数组下标）
*/
FeedBack_e Update_UI_ListView_Data(uint16_t usCmd, uint8_t ucIndex)
{
    extern MachInfo_s MachInfo;
    extern uint8_t g_ucaListReview_DataTable[LIST_REVIEW_ROW_NUM];
    extern __IO DataManage_t g_tDataManage;
    extern __IO MachRunPara_s MachRunPara;
	extern CATGlobalStatus_s CATGlobalStatus;
    
    uint8_t i = 0, j = 0, index = 0, ucNum = 0;
    uint32_t ulIndex = 0, ulTempIndex = 0;
    uint32_t ulHeadIndex = 0, ulTailIndex = 0, ulBeforeTailIndex = 0, ulQueueMaxNum = 0, ulLastestSN = 0, ulStartTick = 0;
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    
    ulHeadIndex = Get_WBCHGB_Head();
    ulTailIndex = Get_WBCHGB_Tail();
    ulBeforeTailIndex = Get_WBCHGB_BeforeTail();  
    ulLastestSN = Get_WBCHGB_LastestSN();
    ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
   
    //无记录, 或者ulLastestSN为0
	if((ulHeadIndex == ulTailIndex) || ulLastestSN == INVALID_DATA_SN)
	{
		LOG_Error("No WBC HGB Data, DataManage: ulHeadIndex=%d, ulTailIndex=%d, ulLastestSN=%d", ulHeadIndex, ulTailIndex, ulLastestSN);
		return FEED_BACK_FAIL;
	}
    
    //检测，参数
    if(ucIndex >= LIST_REVIEW_ROW_NUM)
    {
        LOG_Error("ucIndex(%d) >=(%d)", ucIndex, LIST_REVIEW_ROW_NUM);
        return FEED_BACK_FAIL;
    }
    
    //编辑、打印、删除时，检测 Index
    ulIndex = MachRunPara.taListView_Data[g_ucaListReview_DataTable[ucIndex]].ulCurDataIndex;
    if((usCmd == CMD_LIST_REVIEW_EDIT || usCmd == CMD_LIST_REVIEW_DELETE || usCmd == CMD_LIST_REVIEW_PRINT) && (ulIndex == INVALID_DATA_SN))
    {
        LOG_Error("ulIndex(%d)), Cmd=%d", ulIndex, usCmd);
        return FEED_BACK_FAIL;        
    }
    
    ulStartTick = HAL_GetTick();
    
    //
    //Disable_FrontEnd_Task();
    /*** 关闭系统调度 ***/
//    osKernelLock();
    //处理命令
    switch(usCmd)
    {
        case CMD_LIST_REVIEW_INIT:   //初始化，列表回顾显示数据
        case CMD_LIST_REVIEW_MENU_OUT: //当退出列表回顾界面时，检查并更新所有列表回顾显示数据
        {           
            //从最新的一条数据开始读取
            ulIndex = ulBeforeTailIndex; 
            CountHead_t* tTempCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
            //不到LIST_REVIEW_ROW_NUM个，同时没有回走到ulHeadIndex
            for(i = 0; i < LIST_REVIEW_ROW_NUM; i++)
            {
                //读取CountHead
                eFeedBack = FatFs_Read_ConutHead(ulIndex, tTempCountHead);
                if((tTempCountHead->ulCurIndex != INVALID_DATA_SN) && (DATA_STATUS_DELETE != tTempCountHead->eStatus))
                {
                    eFeedBack = FatFs_Read_WBCHGB_Info(ulIndex, &MachRunPara.taListView_Data[i]);
                    //读取数据失败。则设置为默认值
                    if(eFeedBack == FEED_BACK_FAIL) Set_Default_WBCHGB_Test_Info(&MachRunPara.taListView_Data[i]); 
                    ulIndex = tTempCountHead->ulLastValidIndex;
                    if(INVALID_DATA_SN == ulIndex){
                        i++;
                        break;
                    }
                }
            }
                             
            //不到LIST_REVIEW_ROW_NUM（7）条的，后面的SN置为无效值
            for(; i < LIST_REVIEW_ROW_NUM; i++)
            {
                MachRunPara.taListView_Data[i].ulCurDataSN    = INVALID_DATA_SN;
                MachRunPara.taListView_Data[i].ulCurDataIndex = INVALID_DATA_SN;
                MachRunPara.taListView_Data[i].eDataStatus    = DATA_STATUS_INIT;
            }
            
            //更新g_ucListReview_DataTab[]下标号0-6，对应的taListView_Data[]下标号（按SN降序的下标号）
            Update_ListReview_DataTable();
        }
        break;
        case CMD_LIST_REVIEW_SYNC:  //当计数后，生成的新的数据同步到列表回顾数据
        {
            // 查找初始化数据。如果存在，获取下标号，优先替换
            index = ListReview_InitData_Index();
            if(index >= LIST_REVIEW_ROW_NUM) //均为有效数据，
            {                 
                //选中最后一个（最旧的），SN降序
                index = LIST_REVIEW_ROW_NUM - 1;
                i     = 0;  //为当前列表回顾中最新的，SN号的index
                if(MachRunPara.tWBCHGB_TestInfo.ulCurDataSN == MachRunPara.taListView_Data[g_ucaListReview_DataTable[i]].ulCurDataSN+1)
                {
                    //当列表回顾中当前数据，和新产生的数据连续时，更新到列表回顾，（防止当列表回顾中的数据已翻页，在进行样本分析，同步后，列表回顾中数据显示不是连续顺序）
                    memmove((void*)&MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]], (const void*)&MachRunPara.tWBCHGB_TestInfo, sizeof(WBCHGB_TestInfo_t));
                }
            }else{
                //替换其中无效数据
                memmove((void*)&MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]], (const void*)&MachRunPara.tWBCHGB_TestInfo, sizeof(WBCHGB_TestInfo_t));
            }

            //更新数据映射表
            Update_ListReview_DataTable();
            //
            //return FEED_BACK_SUCCESS;     
        }
        break;
        case CMD_LIST_REVIEW_EDIT: //已编辑一条数据，需要回写到文件
        {              
            if(ucIndex < LIST_REVIEW_ROW_NUM)
            {
                MachRunPara.taListView_Data[g_ucaListReview_DataTable[ucIndex]].usCrc = CRC16((uint8_t*)&MachRunPara.taListView_Data[g_ucaListReview_DataTable[ucIndex]].tMsgHead, sizeof(WBCHGB_TestInfo_t)-4);              
                if(MachRunPara.tWBCHGB_TestInfo.ulCurDataIndex == MachRunPara.taListView_Data[g_ucaListReview_DataTable[ucIndex]].ulCurDataIndex)
                {
                    //编辑的数据和样本分析界面为同一条数据，需同步
                    memmove((void*)&MachRunPara.tWBCHGB_TestInfo, (void*)&MachRunPara.taListView_Data[g_ucaListReview_DataTable[ucIndex]], sizeof(WBCHGB_TestInfo_t));
                }
                //回写到文件
                eFeedBack = FatFs_Write_WBCHGB_Info(ulIndex, &MachRunPara.taListView_Data[g_ucaListReview_DataTable[ucIndex]]); 
                
                //修改Head数据状态位，标记修改已修改
                CountHead_t* tCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
                FatFs_Read_ConutHead(ulIndex, tCountHead);
                tCountHead->eStatus = DATA_STATUS_CHANGE;
                tCountHead->usCrc = CRC16((uint8_t*)tCountHead, sizeof(CountHead_t)-4);
                FatFs_Write_CountHead(ulIndex, tCountHead);
                
                //修改EEPROM标记，以便cat1 检测发送修改数据
                if(ACCOUNT_DATA_TYPE_INNER == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType)
                {
                    MachInfo.catMastSynServEvent.bit.innerChangeSampleResult = 1;
					
					if(CATGlobalStatus.flag.noOtherServer == 0){
						MachInfo.catMastSynServEvent.bit.otherInnerChangeSampleResult = 1;
					}
                }else if(ACCOUNT_DATA_TYPE_OUTSIDE == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType){
                    MachInfo.catMastSynServEvent.bit.outtorChangeSampleResult = 1;
					
					if(CATGlobalStatus.flag.noOtherServer == 0){
						MachInfo.catMastSynServEvent.bit.otherOuttorChangeSampleResult = 1;
					}
                }
                EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
              }
        }
        break;
        case CMD_LIST_REVIEW_DELETE: //删除一条数据
        {
            __IO uint8_t ucSwitchFalg = 0;
            CountHead_t* tLastHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));    
			CountHead_t* tCurHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));  
			CountHead_t* tNextHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t)); 
            if(ulHeadIndex == ulTailIndex)
            {
                //queue空
                break;
            }
            
            //获取该数据的头，
            eFeedBack = FatFs_Read_ConutHead(ulIndex, tCurHead);
            //获取上一条有效数据
            if(tCurHead->ulLastValidIndex != INVALID_DATA_SN && tCurHead->ulCurIndex != ulHeadIndex)
            {
                //存在有效的上一条数据(较旧)
                ucSwitchFalg += 1;
                eFeedBack = FatFs_Read_ConutHead(tCurHead->ulLastValidIndex, tLastHead);
            }
            
            //获取下一条有效数据
            if(tCurHead->ulNextValidIndex != INVALID_DATA_SN && (tCurHead->ulCurIndex+1)%ulQueueMaxNum != ulTailIndex)
            {
                //存在有效的下一条数据(较新)
                ucSwitchFalg += 2;
                eFeedBack = FatFs_Read_ConutHead(tCurHead->ulNextValidIndex, tNextHead);
            }
            LOG_Info("Head Read T=%d", HAL_GetTick()-ulStartTick);
            LOG_Info("CurIndex=%d, LastIndex=%d, NextIndex=%d", tCurHead->ulCurIndex, tLastHead->ulCurIndex, tNextHead->ulCurIndex);
            //
            switch(ucSwitchFalg)
            {
                case 0:
                {
                    //不存在上下条数据（只剩下最后一条有效数据）
                    g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_HeadIndex = \
                            g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_TailIndex;             
					
					Set_WBCHGB_SendIndex(SERVICE_CC,g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_TailIndex);
                    //
					if(CATGlobalStatus.flag.noOtherServer == 0){
						Set_WBCHGB_SendIndex(SERVICE_OTHER,g_tDataManage.tDataManage_WBCHGB[MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType].ulQueue_TailIndex);
					}
                }
                break;
                case 1:
                {
                    //删除为最新的数据
                    tCurHead->ulLastValidIndex = INVALID_DATA_SN;
                    tLastHead->ulNextValidIndex = INVALID_DATA_SN;
                    //回写文件
                    tLastHead->usCrc = CRC16((uint8_t*)tLastHead, sizeof(CountHead_t)-4);
                    eFeedBack = FatFs_Write_CountHead(tLastHead->ulCurIndex, tLastHead);
                    
                    //更新TailIndex为上一个有效数据的index+1, TailIndex为最新有效数据的下一个index
                    Set_WBCHGB_Tail(tLastHead->ulCurIndex+1);
                }
                break;
                case 2:
                {
                    //删除最旧的数
                    tCurHead->ulNextValidIndex = INVALID_DATA_SN;
                    tNextHead->ulLastValidIndex = INVALID_DATA_SN;  
                    
                    //回写文件
                    tNextHead->usCrc = CRC16((uint8_t*)tNextHead, sizeof(CountHead_t)-4);
                    eFeedBack = FatFs_Write_CountHead(tNextHead->ulCurIndex, tNextHead);
                    
                    //更新SendIndex, Head
                    if(TRUE == WBCHGB_SendIndex_Equal_HeadIndex())
                    {
                        Set_WBCHGB_SendIndex(SERVICE_CC,tNextHead->ulCurIndex);
						
						if(CATGlobalStatus.flag.noOtherServer == 0){
							Set_WBCHGB_SendIndex(SERVICE_OTHER,tNextHead->ulCurIndex);
						}
                    }
                    //更新index,为下一个有效数据的index
                    Set_WBCHGB_Head(tNextHead->ulCurIndex);
                
                }
                break;
                case 3:
                {
                    //存在上下条数据，更新索引
                    tLastHead->ulNextValidIndex = tNextHead->ulCurIndex;
                    tNextHead->ulLastValidIndex = tLastHead->ulCurIndex;
                    //回写文件
                    tLastHead->usCrc = CRC16((uint8_t*)tLastHead, sizeof(CountHead_t)-4);
                    eFeedBack = FatFs_Write_CountHead(tLastHead->ulCurIndex, tLastHead);
                    //osDelay(30);
                    tNextHead->usCrc = CRC16((uint8_t*)tNextHead, sizeof(CountHead_t)-4);
                    eFeedBack = FatFs_Write_CountHead(tNextHead->ulCurIndex,tNextHead);
                
                }
                break;
                default:
                {
                    Enable_FrontEnd_Task();
                    return FEED_BACK_FAIL;
                }
            }
            
            //有效个数减一
            Dec_WBCHGB_ValidNum();
            //保存数据管理信息
            Save_DataMange_Info(&g_tDataManage);
            
            //osDelay(30);
            //把当前需删除的数据标记为删除状态，并回写
            tCurHead->eStatus = DATA_STATUS_DELETE;
            tCurHead->ulCurIndex = INVALID_DATA_SN;
            tCurHead->ulLastValidIndex = INVALID_DATA_SN;
            tCurHead->ulNextValidIndex = INVALID_DATA_SN;
            tCurHead->usCrc = CRC16((uint8_t*)tCurHead, sizeof(CountHead_t)-4);
            eFeedBack = FatFs_Write_CountHead(ulIndex, tCurHead);
            LOG_Info("Head Write T=%d", HAL_GetTick()-ulStartTick);
            
            
            //将该条删除的列表回顾数据无效化
            memset((void*)&MachRunPara.taListView_Data[g_ucaListReview_DataTable[ucIndex]], 0, sizeof(WBCHGB_TestInfo_t));
            MachRunPara.taListView_Data[g_ucaListReview_DataTable[ucIndex]].eDataStatus = DATA_STATUS_INIT;
            MachRunPara.taListView_Data[g_ucaListReview_DataTable[ucIndex]].ulCurDataSN = INVALID_DATA_SN;
            MachRunPara.taListView_Data[g_ucaListReview_DataTable[ucIndex]].ulCurDataIndex = INVALID_DATA_SN;
                    
            //删除数据数据前，列表回顾数据个数是LIST_REVIEW_ROW_NUM（整页），才会去加载下一个，以替换当前被删除的
            if(ListReview_ValidData_Num() >= LIST_REVIEW_ROW_NUM-1)
            {
                //更新数据映射表, 获取列表回顾数据中最小SN号，读取其头文件，获取上一个（较旧）有效的数据，覆盖删除的数据
                Update_ListReview_DataTable();
                index = ListReview_DataTable_MinSN_Index();
                if(LIST_REVIEW_ROW_NUM != index)
                {
                    //列表回顾中还有数据
                    ulTempIndex = MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]].ulCurDataIndex;
                    if(ulTempIndex != ulHeadIndex)
                    {  
                        //未到头
                        eFeedBack = FatFs_Read_ConutHead(ulTempIndex, tCurHead);
                        if(tCurHead->ulLastValidIndex != INVALID_DATA_SN && DATA_STATUS_DELETE != tCurHead->eStatus)
                        {
                            eFeedBack = FatFs_Read_WBCHGB_Info(tCurHead->ulLastValidIndex, &MachRunPara.taListView_Data[g_ucaListReview_DataTable[0]]); //重新更新查询表后，按SN降序，第一个为无效化数据
                        }
                    }
                }else{
                    //列表回顾中，没有显示数据, 在前面已做无效后处理
                }
            }
            //更新数据映射表
            Update_ListReview_DataTable();   
            
                        
            //如果当前删除和样本分界面为同一个数据，则也需要更新它（从列表回顾中获取）
            if(ulIndex == MachRunPara.tWBCHGB_TestInfo.ulCurDataIndex)
            {
                //从列表回顾中，找到最大有效SN的数据
                index = ListReview_DataTable_MaxSN_Index();
                if(index >= LIST_REVIEW_ROW_NUM)
                {
                    //不存在，有效数据, 将样本分析中数据无效化
                    memset((void*)&MachRunPara.tWBCHGB_TestInfo, 0, sizeof(WBCHGB_TestInfo_t));
                    MachRunPara.tWBCHGB_TestInfo.ulCurDataSN = INVALID_DATA_SN;
                    MachRunPara.tWBCHGB_TestInfo.ulCurDataIndex = INVALID_DATA_SN;
                    MachRunPara.tWBCHGB_TestInfo.eDataStatus = DATA_STATUS_INIT;
                }else{
                    memmove((void*)&MachRunPara.tWBCHGB_TestInfo, (void*)&MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]], sizeof(WBCHGB_TestInfo_t));
                }            
            }
            LOG_Info("Delete End T=%d", HAL_GetTick()-ulStartTick);
        }
        break;
        case CMD_LIST_REVIEW_PRINT: //打印数据
        {
            extern PrinterCurrentState_s PrinterCurrentState;
            Msg_Head_t tMsgHead = {0};
    
            //检查打印机状态
            if(PrinterCurrentState.readyState == 0x55){
                //无打印纸
                LOG_Error("Printer No Paper");
                tMsgHead.usCmd = CMD_OTHERS_PRITER_NO_PAPER;
                BackEnd_Put_Msg((uint8_t*)&tMsgHead);
                break;
            }else if(PrinterCurrentState.readyState != 0x55 && PrinterCurrentState.readyState != 0x04){
                //打印机连接异常
                LOG_Error("Printer Conn Error");
                tMsgHead.usCmd = CMD_OTHERS_PRITER_CONN_ERR;
                BackEnd_Put_Msg((uint8_t*)&tMsgHead);
                break;
            }
            //打印
           CreateAndSend_WBCHGB_PrintData(&MachRunPara.taListView_Data[g_ucaListReview_DataTable[ucIndex]]);
        }
        break;
        case CMD_LIST_REVIEW_LAST_RECORD: //上一条数据, 加载较新的一条数据 (注意和样本分析中相反，样本分析中上一条数据为较旧的)
        {   
            CountHead_t* tTempCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
            //获取有效数据的个数
            ucNum = ListReview_ValidData_Num();
            if(LIST_REVIEW_ROW_NUM == ucNum)
            {
                //满页，加载上一个数据（较新的数据）
                
                //从列表回顾中，找到最大有效SN的数据
                index = ListReview_DataTable_MaxSN_Index();
                if(index >= LIST_REVIEW_ROW_NUM)
                {
                    //均为无效数据
                }else{
                    ulTempIndex = MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]].ulCurDataIndex;
                    //已是最新的一条，退出
                    if((ulTempIndex+1)%ulQueueMaxNum == ulTailIndex) break;
                    FatFs_Read_ConutHead(ulTempIndex, tTempCountHead);
                    if(tTempCountHead->ulNextValidIndex != INVALID_DATA_SN && DATA_STATUS_DELETE != tTempCountHead->eStatus)
                    {
                        //获取最旧（小）有效数据的下标， 需要替换该条数据
                        i = ListReview_DataTable_MinSN_Index(); 
                        //读取，替换
                        eFeedBack = FatFs_Read_WBCHGB_Info(tTempCountHead->ulNextValidIndex, &MachRunPara.taListView_Data[g_ucaListReview_DataTable[i]]);
                    }
                }
                
            }else{
                //当前不够一页数据， 判断是否、存在更新的数据
                
                //当前列表中，找到最大有效SN的数据
                index = ListReview_DataTable_MaxSN_Index();
                if(index != LIST_REVIEW_ROW_NUM)
                {
                    //存在有效数据
                    
                    ulTempIndex = MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]].ulCurDataIndex;
                    //已是最新的一条，退出
                    if((ulTempIndex+1)%ulQueueMaxNum == ulTailIndex) break;
                    //
                    eFeedBack = FatFs_Read_ConutHead(ulTempIndex, tTempCountHead);
                    if(tTempCountHead->ulNextValidIndex != INVALID_DATA_SN && DATA_STATUS_DELETE != tTempCountHead->eStatus)
                    {
                        //读取，替换第一个无效数据
                        eFeedBack = FatFs_Read_WBCHGB_Info(tTempCountHead->ulNextValidIndex, &MachRunPara.taListView_Data[g_ucaListReview_DataTable[0]]);
                    }
                }
            }
            //更新序号和数据映射表（按SN降序）
            Update_ListReview_DataTable();
            //LOG_Info("LastRecord End T=%d", HAL_GetTick()-ulStartTick);
        }
        break;
        case CMD_LIST_REVIEW_NEXT_RECORD: //下一条数据，加载较旧的一条
        {
            CountHead_t* tTempCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
            //当前列表回顾数据有整页，加载下一个
            ucNum = ListReview_ValidData_Num();
            if(LIST_REVIEW_ROW_NUM == ucNum)    //当前列表回顾中有效数据的个数
            {              
                //从列表回顾中，找到最旧（小）有效SN的数据
                index = ListReview_DataTable_MinSN_Index();
                if(index >= LIST_REVIEW_ROW_NUM)
                {
                    //均为无效数据
                }else{
                    ulTempIndex = MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]].ulCurDataIndex;
                    //已是第一条，退出
                    if(ulTempIndex == ulHeadIndex) break; 
                    eFeedBack = FatFs_Read_ConutHead(ulTempIndex, tTempCountHead);
                    if(tTempCountHead->ulLastValidIndex != INVALID_DATA_SN && DATA_STATUS_DELETE != tTempCountHead->eStatus)
                    {
                        //获取最新（大）有效数据的下标， 需要替换该条数据
                        i = ListReview_DataTable_MaxSN_Index(); 
                        //读取，替换
                        eFeedBack = FatFs_Read_WBCHGB_Info(tTempCountHead->ulLastValidIndex, &MachRunPara.taListView_Data[g_ucaListReview_DataTable[i]]);
                    }
                }
            }else{
                //不够一页，判断是否、存在更旧的数据
                
                //从列表回顾中，找到最小有效SN的数据
                index = ListReview_DataTable_MinSN_Index();
                if(index != LIST_REVIEW_ROW_NUM)
                {
                    //当前列表中存在有效数据
                    
                    ulTempIndex = MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]].ulCurDataIndex;
                    //已是最旧的一条，退出
                    if(ulTempIndex == ulHeadIndex) break;
                    //
                    FatFs_Read_ConutHead(ulTempIndex, tTempCountHead);
                    if(tTempCountHead->ulLastValidIndex != INVALID_DATA_SN && DATA_STATUS_DELETE != tTempCountHead->eStatus)
                    {
                        //读取，替换第一个无效数据
                        eFeedBack = FatFs_Read_WBCHGB_Info(tTempCountHead->ulNextValidIndex, &MachRunPara.taListView_Data[g_ucaListReview_DataTable[0]]);
                    }
                }
            }
            //更新序号和数据映射表（按SN降序）
            Update_ListReview_DataTable();
            LOG_Info("NextRecord End T=%d", HAL_GetTick()-ulStartTick);         
        }
        break;
        case CMD_LIST_REVIEW_LAST_PAGE: //上一页数据, 较新的一页数据
        {
            CountHead_t* tTempCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
            for(j = 0; j < LIST_REVIEW_ROW_NUM; j++)
            { 
                //获取有效数据的个数
                ucNum = ListReview_ValidData_Num();
                if(LIST_REVIEW_ROW_NUM == ucNum)
                {
                    //满页，加载上一个数据（较新的数据）
                    
                    //从列表回顾中，找到最大有效SN的数据
                    index = ListReview_DataTable_MaxSN_Index();
                    if(index >= LIST_REVIEW_ROW_NUM)
                    {
                        //均为无效数据
                    }else{
                        ulTempIndex = MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]].ulCurDataIndex;
                        //已是最新的一条，退出
                        if((ulTempIndex+1)%ulQueueMaxNum == ulTailIndex) break;
                        eFeedBack = FatFs_Read_ConutHead(ulTempIndex, tTempCountHead);
                        if(tTempCountHead->ulNextValidIndex != INVALID_DATA_SN && DATA_STATUS_DELETE != tTempCountHead->eStatus)
                        {
                            //获取最旧（小）有效数据的下标， 需要替换该条数据
                            i = ListReview_DataTable_MinSN_Index(); 
                            //读取，替换
                            eFeedBack = FatFs_Read_WBCHGB_Info(tTempCountHead->ulNextValidIndex, &MachRunPara.taListView_Data[g_ucaListReview_DataTable[i]]);
                        }
                    }
                    
                }else{
                    //当前不够一页数据， 判断是否、存在更新的数据
                    
                    //当前列表中，找到最大有效SN的数据
                    index = ListReview_DataTable_MaxSN_Index();
                    if(index != LIST_REVIEW_ROW_NUM)
                    {
                        //存在有效数据
                        
                        ulTempIndex = MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]].ulCurDataIndex;
                        //已是最新的一条，退出
                        if((ulTempIndex+1)%ulQueueMaxNum == ulTailIndex) break;
                        //
                        eFeedBack = FatFs_Read_ConutHead(ulTempIndex, tTempCountHead);
                        if(tTempCountHead->ulNextValidIndex != INVALID_DATA_SN && DATA_STATUS_DELETE != tTempCountHead->eStatus)
                        {
                            //读取，替换第一个无效数据
                            eFeedBack = FatFs_Read_WBCHGB_Info(tTempCountHead->ulNextValidIndex, &MachRunPara.taListView_Data[g_ucaListReview_DataTable[0]]);
                        }
                    }
                }
                //更新序号和数据映射表（按SN降序）
                Update_ListReview_DataTable();
            }
        }
        break;   
        case CMD_LIST_REVIEW_NEXT_PAGE: //下一页数据，较旧的一页数据
        {    
            CountHead_t* tTempCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
            for(j = 0; j < LIST_REVIEW_ROW_NUM; j++)
            { 
                //当前列表回顾数据有整页，加载下一个
                ucNum = ListReview_ValidData_Num();
                if(LIST_REVIEW_ROW_NUM == ucNum)    //当前列表回顾中有效数据的个数
                {              
                    //从列表回顾中，找到最旧（小）有效SN的数据
                    index = ListReview_DataTable_MinSN_Index();
                    if(index >= LIST_REVIEW_ROW_NUM)
                    {
                        //均为无效数据
                    }else{
                        ulTempIndex = MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]].ulCurDataIndex;
                        //已是第一条，退出
                        if(ulTempIndex == ulHeadIndex) break; 
                        memset((void*)tTempCountHead,  0, sizeof(CountHead_t));
                        eFeedBack = FatFs_Read_ConutHead(ulTempIndex, tTempCountHead);
                        if(tTempCountHead->ulLastValidIndex != INVALID_DATA_SN && DATA_STATUS_DELETE != tTempCountHead->eStatus)
                        {
                            //获取最新（大）有效数据的下标， 需要替换该条数据
                            i = ListReview_DataTable_MaxSN_Index(); 
                            //读取，替换
                            eFeedBack = FatFs_Read_WBCHGB_Info(tTempCountHead->ulLastValidIndex, &MachRunPara.taListView_Data[g_ucaListReview_DataTable[i]]);
                        }
                    }
                }else{
                    //不够一页，判断是否、存在更旧的数据
                    
                    //从列表回顾中，找到最小有效SN的数据
                    index = ListReview_DataTable_MinSN_Index();
                    if(index != LIST_REVIEW_ROW_NUM)
                    {
                        //当前列表中存在有效数据
                        
                        ulTempIndex = MachRunPara.taListView_Data[g_ucaListReview_DataTable[index]].ulCurDataIndex;
                        //已是最旧的一条，退出
                        if(ulTempIndex == ulHeadIndex) break;
                        //
                        eFeedBack = FatFs_Read_ConutHead(ulTempIndex, tTempCountHead);
                        if(tTempCountHead->ulLastValidIndex != INVALID_DATA_SN && DATA_STATUS_DELETE != tTempCountHead->eStatus)
                        {
                            //读取，替换第一个无效数据
                            eFeedBack = FatFs_Read_WBCHGB_Info(tTempCountHead->ulNextValidIndex, &MachRunPara.taListView_Data[g_ucaListReview_DataTable[0]]);
                        }
                    }
                }
                //更新序号和数据映射表（按SN降序）
                Update_ListReview_DataTable();
            }
        }
        break;
        default:
            LOG_Error("Error Cmd=%d", usCmd);
         break;
    }
    
    //构造返回消息//
    if(usCmd != CMD_LIST_REVIEW_INIT || usCmd != CMD_LIST_REVIEW_MENU_OUT || usCmd != CMD_LIST_REVIEW_SYNC) //这两种命令不需要回复
    {
        Msg_Head_t tMsgHead;
        tMsgHead.eErrorCode      = ERROR_CODE_SUCCESS;
        tMsgHead.usMsgLen        = MSG_HEAD_LEN;
        tMsgHead.usCmd           = usCmd;
        //
        BackEnd_Put_Msg((uint8_t*)&tMsgHead);
    }  

    return eFeedBack;
    
}






/*
*	初始化，UI数据（信息）结构体  LIST_REVIEW_ROW_NUM
*/
FeedBack_e UI_ListView_Data_Init(__IO DataManage_t *ptDataManage)
{
   FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
   uint32_t ulHeadIndex = INVALID_DATA_SN, ulTailIndex = INVALID_DATA_SN, ulLastestSN = INVALID_DATA_SN;
    
    ulHeadIndex = Get_WBCHGB_Head();
    ulTailIndex = Get_WBCHGB_Tail(); 
    ulLastestSN = Get_WBCHGB_LastestSN();
    
    //无记录, 或者ulLastestSN为0
	if((ulHeadIndex ==  ulTailIndex) || ulLastestSN == INVALID_DATA_SN)
	{
		LOG_Error("No WBC HGB Data, DataManage: ulHeadIndex=%d, ulTailIndex=%d, ulLastestSN=%d", ulHeadIndex, ulTailIndex, ulLastestSN);
        Set_Default_ListView_Data();
        
		return FEED_BACK_SUCCESS;
	}
    
    eFeedBack = Update_UI_ListView_Data(CMD_LIST_REVIEW_INIT, 0);
    return eFeedBack;
}









