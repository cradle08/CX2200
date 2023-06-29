
#include "file_operate.h"
#include "demo_flashfs.h"





/* 仅允许本文件内调用的函数声明 */
static void ViewSDCapacity(char *_path);
static void ViewRootDir(char *_path);
static void CreateNewFile(char *_path);
static void ReadFileData(char *_path);
static void SeekFileData(char *_path);
static void DeleteDirFile(char *_path);
static void WriteFileTest(char *_path);
static void WriteCSVFile(char *_path);
static void ReadCSVFileData(char *_path);
static void DispMenu(void);
static void StartFTP(char *_path);
static void StopFTP(char *_path);
static void DotFormat(uint64_t _ullVal, char *_sp);



//static char * DevicePath[]= 
//{
//	"M0:", /* SD卡 */			                        
//	"U0:", /* U盘  */
//	"N0:", /* NAND */
//};


/*
*********************************************************************************************************
*	函 数 名: DemoFlashFS
*	功能说明: FlashFS件系统演示主程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DemoFlashFS(uint8_t opt)
{
    char *_path = RL_FLASH_FS_NAND_DRV_PATH;

	/* 打印命令列表，用户可以通过串口操作指令 */
	DispMenu();
	
//	while(1)
//	{
//		if (comGetChar(COM1, &cmd))	/* 从串口读入一个字符(非阻塞方式) */
//		{
			printf("\r\n");
			switch (opt)
			{
				case 1:
					printf("1 - ViewSDCapacity\r\n");
					ViewSDCapacity(_path);		/* 挂载，卸载及其容量显示 */
					break;
				
				case 2:
					printf("2 - ViewRootDir\r\n");
					ViewRootDir(_path);		    /* 显示SD卡根目录下的文件名 */
					break;
				
				case 3:
					printf("3 - CreateNewFile\r\n");
					CreateNewFile(_path);	    /* 创建三个text文本并使用不同函数写入内容 */
					break;
				
				case 4:
					printf("4 - ReadFileData\r\n");
					ReadFileData(_path);	        /* 使用三个不同函数读取文本  */
					break;

				case 5:
					printf("5 - SeekFileData\r\n");
					SeekFileData(_path);	        /* 创建一个text文本，在指定位置对其进行读写操作  */
					break;
				
				case 6:
					printf("6 - DeleteDirFile\r\n");
					DeleteDirFile(_path);	    /* 删除文件夹和文件  */
					break;
				
				case 7:
					printf("7 - WriteFileTest\r\n");
					WriteFileTest(_path);	    /* 测试文件读写速度  */
					break;
				
				case 8:
					printf("8 - WriteCSVFile\r\n");
					WriteCSVFile(_path);	        /* 写数据到CSV文件中  */
					break;
				
				case 9:
					printf("9 - ReadCSVFileData\r\n");
					ReadCSVFileData(_path);	    /* 从CSV文件中读数据  */
					break;
                
                case 10:
                    DispMenu();
                    break;
				
//				case 'M':                       /* 打开SD卡，操作SD卡功能  */
//                case 'm':
//					printf("【m - OpenSDCard】\r\n");
//                    if(strncmp("U0:", _path, 3) == 0)
//                    {
//                        USBH_MSC_Uninitialize(0);
//                        USBH_Uninitialize(0U);
//                    }
//					_path = DevicePath[0];
//					break;

//				case 'U':                       /* 挂载外置U盘，操作U盘功能 */
//                case 'u':
//					printf("【u - OpenUsbDevice】\r\n初始化枚举U盘中...\r\n");
//                    osDelay(100);
//                    USBH_Initialize(0U);                 
//                    USBH_MSC_Initialize(0U);
//                    while (USBH_MSC_GetStatus (0) != usbOK)
//                    {
//                        osDelay(500);
//                        printf(".");
//                    }
//                    printf("U盘挂载完成\r\n");             
//					_path = DevicePath[1];
//					break;	

//				case 'N':                     /* 打开NAND，操作NAND功能 */
//                case 'n':
//					printf("【n - OpenNand】\r\n");
//                    if(strncmp("U0:", _path, 3) == 0)
//                    {
//                        USBH_MSC_Uninitialize(0);
//                        USBH_Uninitialize(0U);
//                    }
//					_path = DevicePath[2];
//					break;                    
                
//                case 'A':                    /* 打开SD卡和NAND模拟U盘功能 */
//				case 'a':
//					printf("【a - OpenUSB Device】\r\n");
//					printf("注意，设备卡挂载期间，不可操作其它命令，要卸载后才可以操作！！\r\n");
//					USBD_Initialize(0U);                 
//					USBD_Connect   (0U);      
//					break;	

//                case 'B':                   /* 关闭SD卡和NAND模拟U盘功能 */
//				case 'b':
//					printf("【b - CloseUSB Device】\r\n");
//					USBD_Disconnect(0);
//					USBD_Uninitialize(0);
//					break;
//                
//                case 'F':                   /* 启动FTP服务器的存储设备，这里用的SD卡 */
//				case 'f':
//					printf("【f - StartFTP】\r\n");
//                    if(strncmp("U0:", _path, 3) == 0)
//                    {
//                        USBH_MSC_Uninitialize(0);
//                        USBH_Uninitialize(0U);
//                    }
//					_path = DevicePath[0];
//					StartFTP(_path);
//					break;
//                
//                case 'T':                 /* 关闭FTP服务器的存储设备*/
//				case 't':
//					printf("【t - StopFTP】\r\n");
//                    if(strncmp("U0:", _path, 3) == 0)
//                    {
//                        USBH_MSC_Uninitialize(0);
//                        USBH_Uninitialize(0U);
//                    }
//					_path = DevicePath[0];
//					StopFTP(_path);
//					break;

				default:
					DispMenu();
					break;
			}
//		}
		
		//osDelay(10);
	//}
}

/*
*********************************************************************************************************
*	函 数 名: DispMenu
*	功能说明: 显示操作提示菜单
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispMenu(void)
{
	printf("\r\n------------------------------------------------\r\n");
	printf("请选择串口操作命令，电脑键盘打印数字即可:\r\n");
	printf("首次使用，如果操作NAND，务必调用串口命令1，进行NAND Flash的低级格式化和文件系统格式化\r\n");	
	printf("1 - 显示卡容量和剩余容量\r\n");
	printf("2 - 显示卡根目录下的文件\r\n");
	printf("3 - 创建三个text文本并使用不同函数写入内容\r\n");
	printf("4 - 使用三个不同函数读取文本\r\n");
	printf("5 - 创建一个text文本并指定一个位置对其进行读写操作\r\n");
	printf("6 - 删除文件夹和文件\r\n");
	printf("7 - 测试文件读写速度\r\n");
	printf("8 - 写数据到CSV文件中\r\n");
	printf("9 - 从CSV文件中读数据\r\n");
//	printf("m - 所有操作是针对SD卡命令\r\n");
//	printf("u - 所有操作针对U盘命令\r\n");
//	printf("n - 所有操作针对NAND命令\r\n");   
//	printf("a - 打开SD和NAND模拟U盘\r\n");
//	printf("b - 关闭SD和NAND模拟U盘\r\n");
//    printf("f - 启动FTP前，挂载SD卡\r\n");
//	printf("t - 关闭FTP后，卸载SD卡\r\n");
    
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
}

/*
*********************************************************************************************************
*	函 数 名: ViewSDCapacity
*	功能说明: 设备的挂载，卸载及其容量显示
*	形    参: _path，U盘，SD卡和NAND
*	返 回 值: 无
*********************************************************************************************************
*/
static void ViewSDCapacity(char *_path)
{
	uint8_t result;
	uint64_t ullSdCapacity;
	uint8_t buf[15];
	uint32_t ser_num;
	char *opt;
    
    result = finit(_path);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("初始化文件系统失败 (%s)\r\n", ReVal_Table[result]);
 		return;       
	}
	else
	{
		printf("初始化文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	/* 加载设备 */
	result = fmount(_path);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("挂载文件系统失败 (%s)\r\n", ReVal_Table[result]);
        
        /* 格式化 */
        if(strncmp("N0:", _path, 3) == 0)
        {
            opt = "/LL /L nand /FAT16";
            printf("文件系统格式中......\r\n");
            result = fformat (_path, opt);
            printf("文件系统格式化 (%s)\r\n", ReVal_Table[result]);
            if(result != NULL)
            {
                return;
            }
        }
        else
        {
            return;
        }
	}
	else
	{
		printf("挂载文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
    
	printf("------------------------------------------------------------------\r\n");
	
	/* 获取volume label */
	if (fvol (_path, (char *)buf, &ser_num) == 0) 
	{
		if (buf[0]) 
		{
			printf ("volume label是 %s\r\n", buf);
		}
		else 
		{
			printf ("没有volume label\r\n");
		}
		
		printf ("volume serial number是 %d\r\n", ser_num);
	}
	else 
	{
		printf ("Volume访问错误\r\n");
	}

	/* 获取剩余容量 */
	ullSdCapacity = ffree(_path);
	DotFormat(ullSdCapacity, (char *)buf);
	printf("剩余容量 = %10s字节\r\n", buf);

	/* 卸载设备 */
	result = funmount(_path);
	if(result != NULL)
	{
		printf("卸载文件系统失败\r\n");
	}
	else
	{
		printf("卸载文件系统成功\r\n");
	}
    
    result = funinit(_path);
	if(result != NULL)
	{
		printf("复位文件系统失败 (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("复位文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}

	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: ViewRootDir
*	功能说明: 显示根目录下的文件名
*	形    参: _path，U盘，SD卡和NAND
*	返 回 值: 无
*********************************************************************************************************
*/
static void ViewRootDir(char *_path)
{
	uint8_t result;
	fsFileInfo info;
	uint8_t buf[15];
    char pathbuf[20];
	
    info.fileID = 0;   /* 每次使用ffind函数前，info.fileID必须初始化为0 */

    result = finit(_path);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("初始化文件系统失败 (%s)\r\n", ReVal_Table[result]);
 		return;       
	}
	else
	{
		printf("初始化文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	/* 加载 */
	result = fmount(_path);
	
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("挂载文件系统失败 (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("挂载文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
	
	printf("------------------------------------------------------------------\r\n");
	printf("文件名                 |  文件大小     | 文件ID  | 属性      |日期\r\n");
	
	/* 
	   将根目录下的所有文件列出来。
	   1. "*" 或者 "*.*" 搜索指定路径下的所有文件
	   2. "abc*"         搜索指定路径下以abc开头的所有文件
	   3. "*.jpg"        搜索指定路径下以.jpg结尾的所有文件
	   4. "abc*.jpg"     搜索指定路径下以abc开头和.jpg结尾的所有文件
	
	   以下是实现搜索根目录下所有文件
	*/
    sprintf(pathbuf, "%s*.*", _path);
	while(ffind (pathbuf, &info) == 0)  
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
			printf("(0x%02x)目录", info.attrib);
		}
		else
		{
			printf("(0x%02x)文件", info.attrib);
		}
		
		/* 显示文件日期 */
		printf ("  %04d.%02d.%02d  %02d:%02d\r\n",
                 info.time.year, info.time.mon, info.time.day,
               info.time.hr, info.time.min);
    }
	
	if (info.fileID == 0)  
	{
		printf ("卡中没有存放文件\r\n");
	}

access_fail:
	/* 卸载 */
	result = funmount(_path);
	if(result != NULL)
	{
		printf("卸载文件系统失败\r\n");
	}
	else
	{
		printf("卸载文件系统成功\r\n");
	}
    
    result = funinit(_path);
	if(result != NULL)
	{
		printf("复位文件系统失败 (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("复位文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
	
	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: CreateNewFile
*	功能说明: 创建三个text文本，分别使用fwrite，fprintf和fputs写数据。
*	形    参: _path，U盘，SD卡和NAND
*	返 回 值: 无
*********************************************************************************************************
*/
static void CreateNewFile(char *_path)
{
	const uint8_t WriteText[] = {"武汉安富莱电子有限公司\r\n2015-09-06\r\nwww.armfly.com\r\nWWW.ARMFLY.COM"};
	const uint8_t WriteText1[] = {"武汉安富莱电子有限公司\r"};
	FILE *fout;
	uint32_t bw;
	uint32_t i = 2;
	uint8_t result;
    char namepath[30];

    
    result = finit(_path);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("初始化文件系统失败 (%s)\r\n", ReVal_Table[result]);
 		return;       
	}
	else
	{
		printf("初始化文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	/* 加载 */
	result = fmount(_path);
	
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("挂载文件系统失败 (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("挂载文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test1.txt，如果没有子文件夹和txt文件会自动创建*/
    sprintf(namepath, "%s\\test\\test1.txt", _path);
	fout = fopen (namepath, "w"); 
	if (fout != NULL) 
	{
		printf("打开文件\\test\\test1.txt成功，如果没有子文件夹和txt文件会自动创建\r\n");
		/* 写数据 */
		bw = fwrite (WriteText, sizeof(uint8_t), sizeof(WriteText)/sizeof(uint8_t), fout);
		if(bw == sizeof(WriteText)/sizeof(uint8_t))
		{
			printf("写入内容成功\r\n");
		}
		else
		{ 
			printf("写入内容失败\r\n");
		}
		
		/* 关闭文件 */
		fclose(fout);
	}
	else
	{
		printf("打开文件\\test\\test.txt失败\r\n");
	}
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test2.txt，如果没有子文件夹和txt文件会自动创建*/
    sprintf(namepath, "%s\\test\\test2.txt", _path);
	fout = fopen (namepath, "w"); 
	if (fout != NULL) 
	{
		printf("打开文件\\test\\test2.txt成功，如果没有子文件夹和txt文件会自动创建\r\n");
		
		/* 写数据 */
		bw = fprintf (fout, "%d %d %f\r\n", i, i*5, i*5.55f);
		
		/* 使用函数ferror检测是否发生过错误 */
		if (ferror(fout) != NULL)  
		{
			printf("写入内容失败\r\n");
		}
		else
		{
			printf("写入内容成功\r\n");	
		}
		
		/* 关闭文件 */
		fclose(fout);
	}
	else
	{
		printf("打开文件\\test\\test.txt失败\r\n");
	}
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test3.txt，如果没有子文件夹和txt文件会自动创建*/
 	sprintf(namepath, "%s\\test\\test3.txt", _path);   
	fout = fopen (namepath, "w"); 
	if (fout != NULL) 
	{
		printf("打开文件\\test\\test3.txt成功，如果没有子文件夹和txt文件会自动创建\r\n");
		
		/* 写数据 */
		fputs((const char *)WriteText1, fout);
		
		/* 使用函数ferror检测是否发生过错误 */
		if (ferror(fout) != NULL)  
		{
			printf("写入内容失败\r\n");
		}
		else
		{
			printf("写入内容成功\r\n");	
		}
		
		/* 关闭文件 */
		fclose(fout);
	}
	else
	{
		printf("打开文件\\test\\test.txt失败\r\n");
	}

access_fail:	
	/* 卸载 */
	result = funmount(_path);
	if(result != NULL)
	{
		printf("卸载文件系统失败\r\n");
	}
	else
	{
		printf("卸载文件系统成功\r\n");
	}
    
    result = funinit(_path);
	if(result != NULL)
	{
		printf("复位文件系统失败 (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("复位文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}

	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: ReadFileData
*	功能说明: 分别使用fread，fscan和fgets读取三个不同的txt文件。
*	形    参: _path，U盘，SD卡和NAND
*	返 回 值: 无
*********************************************************************************************************
*/
static void ReadFileData(char *_path)
{
	const uint8_t WriteText[] = {"武汉安富莱电子有限公司\r\n2015-09-06\r\nwww.armfly.com\r\nWWW.ARMFLY.COM"};
	uint8_t Readbuf[50];
	FILE *fin;
	uint32_t bw;
	uint32_t index1, index2;
	float  count = 0.0f;
	uint8_t result;
	char namepath[30];

    result = finit(_path);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("初始化文件系统失败 (%s)\r\n", ReVal_Table[result]);
 		return;       
	}
	else
	{
		printf("初始化文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	/* 加载 */
	result = fmount(_path);
	
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("挂载文件系统失败 (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("挂载文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test1.txt */
	sprintf(namepath, "%s\\test\\test1.txt", _path);
	fin = fopen (namepath, "r"); 
	if (fin != NULL) 
	{
		printf("<1>打开文件\\test\\test1.txt成功\r\n");
		
		/* 防止警告 */
		(void) WriteText;
		
		/* 读数据 */
		bw = fread(Readbuf, sizeof(uint8_t), sizeof(WriteText)/sizeof(uint8_t), fin);
		if(bw == sizeof(WriteText)/sizeof(uint8_t))
		{
			Readbuf[bw] = NULL;
			printf("test1.txt 文件内容 : \r\n%s\r\n", Readbuf);
		}
		else
		{ 
			printf("读内容失败\r\n");
		}
		
		/* 关闭文件 */
		fclose(fin);
	}
	else
	{
		printf("打开文件\\test\\test.txt失败, 可能文件不存在\r\n");
	}
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test2.txt */
	sprintf(namepath, "%s\\test\\test2.txt", _path);
	fin = fopen (namepath, "r"); 
	if (fin != NULL) 
	{
		printf("\r\n<2>打开文件\\test\\test2.txt成功\r\n");
		
		bw = fscanf(fin, "%d %d %f", &index1, &index2, &count);

		/* 3参数都读出来了 */
		if (bw == 3)  
		{
			printf("读出的数值\r\nindex1 = %d index2 = %d count = %f\r\n", index1, index2, count);
		}
		else
		{
			printf("读内容失败\r\n");	
		}
		
		/* 关闭文件 */
		fclose(fin);
	}
	else
	{
		printf("打开文件\\test\\test.txt失败(%s)\r\n", ReVal_Table[result]);
	}
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test3.txt，如果没有子文件夹和txt文件会自动创建*/
	sprintf(namepath, "%s\\test\\test3.txt", _path);
	fin = fopen (namepath, "r"); 
	if (fin != NULL) 
	{
		printf("\r\n<3>打开文件\\test\\test3.txt成功\r\n");
		
		/* 读数据 */
		if(fgets((char *)Readbuf, sizeof(Readbuf), fin) != NULL)
		{
			printf("test3.txt 文件内容 : \r\n%s\r\n", Readbuf);
		}
		else
		{
			printf("读内容失败\r\n");
		}
		
		/* 关闭文件 */
		fclose(fin);
	}
	else
	{
		printf("打开文件\\test\\test.txt失败\r\n");
	}
	
access_fail:
	/* 卸载 */
	result = funmount(_path);
	if(result != NULL)
	{
		printf("卸载文件系统失败\r\n");
	}
	else
	{
		printf("卸载文件系统成功\r\n");
	}
    
    result = funinit(_path);
	if(result != NULL)
	{
		printf("复位文件系统失败 (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("复位文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}

	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: SeekFileData
*	功能说明: 创建一个text文本，在指定位置对其进行读写操作。
*	形    参: _path，U盘，SD卡和NAND
*	返 回 值: 无
*********************************************************************************************************
*/
static void SeekFileData(char *_path)
{
	const uint8_t WriteText[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};
	FILE *fin, *fout;
	uint32_t bw;
	uint32_t uiPos;
	uint8_t ucChar;
	uint8_t result;
	char namepath[30];
    

    result = finit(_path);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("初始化文件系统失败 (%s)\r\n", ReVal_Table[result]);
 		return;       
	}
	else
	{
		printf("初始化文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	/* 加载 */
	result = fmount(_path);
	
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("挂载文件系统失败 (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("挂载文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* 打开文件夹test中的文件test1.txt，如果没有子文件夹和txt文件会自动创建*/
	sprintf(namepath, "%s\\test.txt", _path);
	fout = fopen (namepath, "w"); 
	if (fout != NULL) 
	{
		printf("打开文件\\test.txt成功，如果没有txt文件会自动创建\r\n");
		/* 写数据 */
		bw = fwrite (WriteText, sizeof(uint8_t), sizeof(WriteText)/sizeof(uint8_t), fout);
		if(bw == sizeof(WriteText)/sizeof(uint8_t))
		{
			printf("写入内容成功\r\n");
		}
		else
		{ 
			printf("写入内容失败\r\n");
		}
			
		/* 关闭文件 */
		fclose(fout);
	}
	else
	{
		printf("打开文件\\test.txt失败\r\n");
	}
	
	/***********************************************/
	sprintf(namepath, "%s\\test.txt", _path);
	fin = fopen (namepath,"r");
	if (fin != NULL)  
	{
		printf("\r\n打开文件\\test.txt成功\r\n");
		
		/* 读取文件test.txt的位置0的字符 */
		fseek (fin, 0L, SEEK_SET);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);		
		printf("文件test.txt当前读取位置：%02d，字符：%c\r\n", uiPos, ucChar);
		
		/* 读取文件test.txt的位置1的字符 */
		fseek (fin, 1L, SEEK_SET);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);		
		printf("文件test.txt当前读取位置：%02d，字符：%c\r\n", uiPos, ucChar);

		/* 读取文件test.txt的位置25的字符 */
		fseek (fin, 25L, SEEK_SET);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);		
		printf("文件test.txt当前读取位置：%02d，字符：%c\r\n", uiPos, ucChar);
		
		/* 通过上面函数的操作，当前读写位置是26
		   下面函数是在当前位置的基础上后退2个位置，也就是24，调用函数fgetc后，位置就是25
		 */
		fseek (fin, -2L, SEEK_CUR);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);		
		printf("文件test.txt当前读取位置：%02d，字符：%c\r\n", uiPos, ucChar);
		
		/* 读取文件test.txt的倒数第2个字符, 最后一个是'\0' */
		fseek (fin, -2L, SEEK_END); 
		uiPos = ftell(fin); 
		ucChar = fgetc (fin);
		printf("文件test.txt当前读取位置：%02d，字符：%c\r\n", uiPos, ucChar);
		
		/* 将读取位置重新定位到文件开头 */
		rewind(fin);  
		uiPos = ftell(fin); 
		ucChar = fgetc (fin);
		printf("文件test.txt当前读取位置：%02d，字符：%c\r\n", uiPos, ucChar);	
		
		/* 
		   这里是演示一下ungetc的作用，此函数就是将当前的读取位置偏移回一个字符，
		   而fgetc调用后位置增加一个字符。
		 */
		fseek (fin, 0L, SEEK_SET);
		ucChar = fgetc (fin);
		uiPos = ftell(fin); 
		printf("文件test.txt当前读取位置：%02d，字符：%c\r\n", uiPos, ucChar);
		ungetc(ucChar, fin); 
		uiPos = ftell(fin); 
		printf("文件test.txt当前读取位置：%02d，字符：%c\r\n", uiPos, ucChar);
		
		/* 关闭*/
		fclose (fin);
	}
	else
	{
		printf("打开文件\\test.txt失败\r\n");
	}
	
access_fail:	
	/* 卸载 */
	result = funmount(_path);
	if(result != NULL)
	{
		printf("卸载文件系统失败\r\n");
	}
	else
	{
		printf("卸载文件系统成功\r\n");
	}
    
    result = funinit(_path);
	if(result != NULL)
	{
		printf("复位文件系统失败 (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("复位文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}

	printf("------------------------------------------------------------------\r\n");
	
}

/*
*********************************************************************************************************
*	函 数 名: DeleteDirFile
*	功能说明: 删除文件夹和文件
*	形    参: _path，U盘，SD卡和NAND
*	返 回 值: 无
*********************************************************************************************************
*/
static void DeleteDirFile(char *_path)
{
	uint8_t result;
	uint8_t i;
	fsStatus restatus;
	char FileName[50];
	
    result = finit(_path);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("初始化文件系统失败 (%s)\r\n", ReVal_Table[result]);
 		return;       
	}
	else
	{
		printf("初始化文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	/* 加载 */
	result = fmount(_path);
	
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("挂载文件系统失败 (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("挂载文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/* 删除文件 speed1.txt */
	for (i = 0; i < 20; i++)
	{
        sprintf(FileName, "%s\\Speed%02d.txt", _path, i);/* 每写1次，序号递增 */	
		restatus = fdelete (FileName, NULL);
		if (restatus != NULL) 
		{
			printf("%s文件不存在（返回值：%d）\r\n", FileName, result);
		}
		else
		{
			printf("删除%s文件成功\r\n", FileName);
		}
	}
	
	for(i = 1; i < 4; i++)
	{
        sprintf(FileName, "%s\\test\\test%01d.txt", _path, i);   /* 每写1次，序号递增 */
		restatus = fdelete (FileName, NULL);
		if (restatus != NULL) 
		{
			printf("%s文件不存在（返回值：%d）\r\n", FileName, result);
		}
		else
		{
			printf("删除%s文件成功\r\n", FileName);
		}	
	}
	
	/* 删除文件夹test */
    sprintf(FileName, "%s\\test\\", _path);
	restatus = fdelete (FileName, NULL);   
	if (restatus != NULL) 
	{
		printf("test文件夹非空或不存在（返回值：%d）\r\n", result);
	}
	else
	{
		printf("删除test文件夹成功\r\n");
	}
	
	/* 删除csv文件 */

    sprintf(FileName, "%s\\record.csv", _path);
	restatus = fdelete (FileName, NULL);  
	if (restatus != NULL) 
	{
		printf("record.csv文件不存在（返回值：%d）\r\n", result);
	}
	else
	{
		printf("删除record.csv件成功\r\n");
	}
	
access_fail:
	/* 卸载 */
	result = funmount(_path);
	if(result != NULL)
	{
		printf("卸载文件系统失败\r\n");
	}
	else
	{
		printf("卸载文件系统成功\r\n");
	}
    
    result = funinit(_path);
	if(result != NULL)
	{
		printf("复位文件系统失败 (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("复位文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
		
	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: WriteFileTest
*	功能说明: 测试文件读写速度
*	形    参: _path，U盘，SD卡和NAND
*	返 回 值: 无
*********************************************************************************************************
*/
/* 用于测试读写速度 */
#define TEST_FILE_LEN			(2*1024)//(2*1024*1024)	/* 用于测试的文件长度 */ 
#define BUF_SIZE				(1*1024)//(4*1024)		/* 每次读写SD卡的最大数据长度 */
uint8_t g_TestBuf[BUF_SIZE];

static void WriteFileTest(char *_path)
{
	FILE *fout;
	uint32_t bw;
	uint32_t i,k;
	uint32_t runtime1,runtime2,timelen;
	uint8_t err = 0, result;
	char TestFileName[13];
	static uint8_t s_ucTestSn = 0;

	for (i = 0; i < sizeof(g_TestBuf); i++)
	{
		g_TestBuf[i] = (i / 512) + '0';
	}

    result = finit(_path);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("初始化文件系统失败 (%s)\r\n", ReVal_Table[result]);
 		return;       
	}
	else
	{
		printf("初始化文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	/* 加载 */
	result = fmount(_path);
	
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("挂载文件系统失败 (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("挂载文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
	
	printf("------------------------------------------------------------------\r\n");

	/* 打开文件 -----------------------------------------*/
	sprintf(TestFileName, "%s\\Speed%02d.txt", _path, s_ucTestSn++);		/* 每写1次，序号递增 */
	fout = fopen (TestFileName, "w");
	if (fout ==  NULL)
	{
		printf("文件: %s创建失败\r\n", TestFileName);
		goto access_fail;
	}
	
	/* 写一串数据 测试写速度-----------------------------------------*/
	printf("开始写文件%s %dKB ...\r\n", TestFileName, TEST_FILE_LEN / 1024);
	runtime1 = osKernelGetTickCount();	/* 读取系统运行时间 */
	for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
		bw = fwrite (g_TestBuf, sizeof(uint8_t), BUF_SIZE, fout);   
		if (bw == BUF_SIZE)
		{
			if (((i + 1) % 8) == 0)
			{
				printf(".");
			}
		}
		else
		{
			err = 1;
			printf("%s文件写失败\r\n", TestFileName);
			break;
		}
	}
	runtime2 = osKernelGetTickCount();	/* 读取系统运行时间 */

	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("\r\n  写耗时 : %dms   平均写速度 : %dB/S (%dKB/S)\r\n",
			timelen,
			(TEST_FILE_LEN * 1000) / timelen,
			((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}
	
	/* 使用函数ferror再次检测是否发生过错误 -----------------------*/
	if (ferror (fout))  
	{
		printf ("写数据过程中出现过错误\r\n\n");
    }
	else
	{
		printf ("写数据过程中未出现过错误\r\n\n");		
	}

	fclose (fout);	/* 关闭文件*/


	/* 开始读文件，测试读速度 ---------------------------------------*/
	fout = fopen (TestFileName, "r");
	if (fout ==  NULL)
	{
		printf("文件: %s读取失败\r\n", TestFileName);
		goto access_fail;
	}

	printf("开始读文件 %dKB ...\r\n", TEST_FILE_LEN / 1024);
	runtime1 = osKernelGetTickCount();	/* 读取系统运行时间 */
	for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
		bw = fread(g_TestBuf, sizeof(uint8_t), BUF_SIZE, fout);   
		if (bw == BUF_SIZE)
		{
			if (((i + 1) % 8) == 0)
			{
				printf(".");
			}

			/* 比较写入的数据是否正确，此语句会导致读卡速度结果降低 */
			for (k = 0; k < sizeof(g_TestBuf); k++)
			{
				if (g_TestBuf[k] != (k / 512) + '0')
				{
				  	err = 1;
					printf("Speed1.txt 文件读成功，但是数据出错\r\n");
					break;
				}
			}
			if (err == 1)
			{
				break;
			}
		}
		else
		{
			err = 1;
			printf("Speed1.txt 文件读失败\r\n");
			break;
		}
	}
	runtime2 = osKernelGetTickCount();	/* 读取系统运行时间 */

	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("\r\n  读耗时 : %dms   平均读速度 : %dB/S (%dKB/S)\r\n", timelen,
			(TEST_FILE_LEN * 1000) / timelen, ((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}
	
	/* 使用函数ferror再次检测是否发生过错误 -----------------------*/
	if (ferror (fout))  
	{
		printf ("读数据过程中出现过错误\r\n");
    }
	else
	{
		printf ("读数据过程中未出现过错误\r\n");		
	}

	fclose (fout);	/* 关闭文件*/
	
access_fail:
	/* 卸载 */
	result = funmount(_path);
	if(result != NULL)
	{
		printf("卸载文件系统失败\r\n");
	}
	else
	{
		printf("卸载文件系统成功\r\n");
	}
    
    result = funinit(_path);
	if(result != NULL)
	{
		printf("复位文件系统失败 (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("复位文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
	
	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: WriteCSVFile
*	功能说明: 写数据到CSV文件中
*	形    参: _path，U盘，SD卡和NAND
*	返 回 值: 无
*********************************************************************************************************
*/
static void WriteCSVFile(char *_path)
{
	const uint8_t WriteText[] = {"记录时间,标号,记录数值一,记录数值二\r\n"};
	FILE *fout;
	uint8_t result;
	static uint16_t i = 1;
	static uint8_t ucFirstRunFlag = 0;
	char namepath[30];

    result = finit(_path);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("初始化文件系统失败 (%s)\r\n", ReVal_Table[result]);
 		return;       
	}
	else
	{
		printf("初始化文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	/* 加载 */
	result = fmount(_path);
	
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("挂载文件系统失败 (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("挂载文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* 
	  1. 打开文件record.csv，如果没有此文件会自动创建。
	  2. 第二个参数表示向此文件写数据都从尾部开始添加。
	*/
	sprintf(namepath, "%s\\record.csv", _path);
	fout = fopen (namepath, "a");
	if (fout != NULL) 
	{
		printf("打开文件M0:\\record.csvt成功，如果没有此文件会自动创建\r\n");
		
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
			printf("写入内容失败\r\n");
		}
		else
		{
			printf("写入内容成功\r\n");	
		}
		
		/* 关闭文件 */
		fclose(fout);
	}
	else
	{
		printf("打开文件M0:\\record.csv失败\r\n");
	}

access_fail:	
	/* 卸载 */
	result = funmount(_path);
	if(result != NULL)
	{
		printf("卸载文件系统失败\r\n");
	}
	else
	{
		printf("卸载文件系统成功\r\n");
	}
    
    result = funinit(_path);
	if(result != NULL)
	{
		printf("复位文件系统失败 (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("复位文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}

	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: ReadCSVFileData
*	功能说明: 读取CSV文件中的数据。
*	形    参: _path，U盘，SD卡和NAND
*	返 回 值: 无
*********************************************************************************************************
*/
static void ReadCSVFileData(char *_path)
{
	uint8_t Readbuf[50];
	FILE *fin;
	uint8_t result;
	char namepath[30];

    result = finit(_path);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("初始化文件系统失败 (%s)\r\n", ReVal_Table[result]);
 		return;       
	}
	else
	{
		printf("初始化文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	/* 加载 */
	result = fmount(_path);
	
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("挂载文件系统失败 (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("挂载文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* 打开record.csv文件 */
	sprintf(namepath, "%s\\record.csv", _path);
	fin = fopen (namepath, "r");
	if (fin != NULL) 
	{
		printf("打开文件M0:\\record.csv成功\r\n");
		
		/* 将所有数据都读出来，使用fgets函数的好处就是可以一行一行的读取，因为此函数遇到'\n'换行符就会停止读取 */
		while (fgets((char *)Readbuf, sizeof(Readbuf), fin) != NULL)  
		{
			printf("%s", Readbuf);	
		}
		
		/* 使用函数ferror检测是否发生过错误 */
		if (ferror(fin) != NULL)  
		{
			printf("读出内容失败\r\n");
		}
		else
		{
			printf("record.csv里面的内容全部正确读出\r\n");	
		}
		
		/* 关闭文件 */
		fclose(fin);
	}
	else
	{
		printf("打开文件M0:\\record.csv失败\r\n");
	}
	
access_fail:
	/* 卸载 */
	result = funmount(_path);
	if(result != NULL)
	{
		printf("卸载文件系统失败\r\n");
	}
	else
	{
		printf("卸载文件系统成功\r\n");
	}
    
    result = funinit(_path);
	if(result != NULL)
	{
		printf("复位文件系统失败 (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("复位文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}

	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	函 数 名: StartFTP
*	功能说明: 启动FTP前，挂载设备。
*	形    参: _path，SD卡。
*	返 回 值: 无
*********************************************************************************************************
*/
static void StartFTP(char *_path)
{
	uint8_t result;

    result = finit(_path);
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("初始化文件系统失败 (%s)\r\n", ReVal_Table[result]);
 		return;       
	}
	else
	{
		printf("初始化文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
    
	/* 加载 */
	result = fmount(_path);
	
	if(result != NULL)
	{
		/* 如果挂载失败，务必不要再调用FlashFS的其它API函数，防止进入硬件异常 */
		printf("挂载文件系统失败 (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("挂载文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
	
	printf("------------------------------------------------------------------\r\n");
}

static void StopFTP(char *_path)
{
	uint8_t result;

	/* 卸载 */
	result = funmount(_path);
	if(result != NULL)
	{
		printf("卸载文件系统失败\r\n");
	}
	else
	{
		printf("卸载文件系统成功\r\n");
	}
    
    result = funinit(_path);
	if(result != NULL)
	{
		printf("复位文件系统失败 (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("复位文件系统成功 (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
