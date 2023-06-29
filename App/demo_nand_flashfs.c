/*
*********************************************************************************************************
*
*	ģ������ : FlashFS�ļ�ϵͳ��ʾģ�顣
*	�ļ����� : demo_nand_flashfs.c
*	��    �� : V1.0
*	˵    �� : ��������ֲFlashFS�ļ�ϵͳ����ҪѧϰFlashFS���ۺϲ�����
*
*	�޸ļ�¼ :
*		�汾��    ����         ����            ˵��
*       V1.0    2019-06-19   Eric2013    1. RL-FlashFS���ļ����汾V6.x
*
*	Copyright (C), 2019-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
//#include "includes.h"
#include "main.h"
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>

#include "main.h"
#include "cmsis_os2.h"
#include "rl_fs.h"
#include "demo_nand_flashfs.h"
#include "ui_files.h"
#include "file_operate.h"

/* ���ڲ��Զ�д�ٶ� */
#define TEST_FILE_LEN			(2*1024*1024)	/* ���ڲ��Ե��ļ����� */
#define BUF_SIZE				(4*1024)		/* ÿ�ζ�дSD����������ݳ��� */

/* �������ļ��ڵ��õĺ������� */
static void ViewSDCapacity(void);
static void ViewRootDir(void);
static void CreateNewFile(void);
static void ReadFileData(void);
static void SeekFileData(void);
static void DeleteDirFile(void);
static void WriteFileTest(void);
static void WriteCSVFile(void);
static void ReadCSVFileData(void);
static void DispMenu(void);
static void DotFormat(uint64_t _ullVal, char *_sp);

/* FlashFS API�ķ���ֵ */
static const char * ReVal_Table[]= 
{
	"fsOK���ɹ�",				                        
	"fsError��δָ���Ĵ���",
	"fsUnsupported��������֧��",
	"fsAccessDenied����Դ���ʱ��ܾ�",
	
	"fsInvalidParameter��������Ч",
	"fsInvalidDrive��������Ч������������",
	"fsInvalidPath��·����Ч",
	"fsUninitializedDrive������δ��ʼ�� ",

	"fsDriverError����д����",
	"fsMediaError��ý�����",
	"fsNoMedia��ý�鲻���ڣ�����δ��ʼ��",
	"fsNoFileSystem���ļ�ϵͳδ��ʽ��",

	"fsNoFreeSpace��û�п��ÿռ�",
	"fsFileNotFound���ļ�δ�ҵ�",
	"fsDirNotEmpty���ļ��зǿ�",
	"fsTooManyOpenFiles�����ļ�̫��",
};

//ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint8_t g_TestBuf[BUF_SIZE]);
//ALIGN_32BYTES(uint8_t g_TestBuf[BUF_SIZE]);
uint8_t g_TestBuf[BUF_SIZE];
/*
*********************************************************************************************************
*	�� �� ��: DemoFlashFS
*	����˵��: FlashFS��ϵͳ��ʾ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DemoFlashFS(uint8_t ucCmd)
{
	uint8_t cmd = ucCmd;
	uint8_t result;

	
	result = finit("N0:");
	if(result != NULL)
	{
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("�����ļ�ϵͳ�ɹ� (%s)\r\n", ReVal_Table[result]);
	}
	
//	while(1)
//	{
//		if (comGetChar(COM1, &cmd))	/* �Ӵ��ڶ���һ���ַ�(��������ʽ) */
//		{
			printf("\r\n");
			switch (cmd)
			{
				case 1:
					printf("��1 - ViewSDCapacity��\r\n");
					ViewSDCapacity();		/* SD���Ĺ��أ�ж�ؼ���������ʾ */
					break;
				
				case 2:
					printf("��2 - ViewRootDir��\r\n");
					ViewRootDir();		    /* ��ʾSD����Ŀ¼�µ��ļ��� */
					break;
				
				case 3:
					printf("��3 - CreateNewFile��\r\n");
					CreateNewFile();	    /* ��������text�ı���ʹ�ò�ͬ����д������ */
					break;
				
				case 4:
					printf("��4 - ReadFileData��\r\n");
					ReadFileData();	        /* ʹ��������ͬ������ȡ�ı�  */
					break;

				case 5:
					printf("��5 - SeekFileData��\r\n");
					SeekFileData();	        /* ����һ��text�ı�����ָ��λ�ö�����ж�д����  */
					break;
				
				case 6:
					printf("��6 - DeleteDirFile��\r\n");
					DeleteDirFile();	    /* ɾ���ļ��к��ļ�  */
					break;
				
				case 7:
					printf("��7 - WriteFileTest��\r\n");
					WriteFileTest();	    /* �����ļ���д�ٶ�  */
					break;
				
				case 8:
					printf("��8 - WriteCSVFile��\r\n");
					WriteCSVFile();	        /* д���ݵ�CSV�ļ���  */
					break;
				
				case 9:
					printf("��9 - ReadCSVFileData��\r\n");
					ReadCSVFileData();	    /* ��CSV�ļ��ж�����  */
					break;
				case 10:
                {
                   /* ��ӡ�����б��û�����ͨ�����ڲ���ָ�� */
                    DispMenu();
                }
                break;
				default:
					DispMenu();
					break;
			//}
		//}
		
		osDelay(10);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: DispMenu
*	����˵��: ��ʾ������ʾ�˵�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispMenu(void)
{
	printf("\r\n------------------------------------------------\r\n");
	printf("��ѡ�񴮿ڲ���������Լ��̴�ӡ���ּ���:\r\n");
	printf("�״�ʹ�ã���ص��ô�������0������NAND Flash�ĵͼ���ʽ�����ļ�ϵͳ��ʽ��\r\n");	
	printf("1 - ��ʾNAND������ʣ������\r\n");
	printf("2 - ��ʾNAND��Ŀ¼�µ��ļ�\r\n");
	printf("3 - ��������text�ı���ʹ�ò�ͬ����д������\r\n");
	printf("4 - ʹ��������ͬ������ȡ�ı�\r\n");
	printf("5 - ����һ��text�ı���ָ��һ��λ�ö�����ж�д����\r\n");
	printf("6 - ɾ���ļ��к��ļ�\r\n");
	printf("7 - �����ļ���д�ٶ�\r\n");
	printf("8 - д���ݵ�CSV�ļ���\r\n");
	printf("9 - ��CSV�ļ��ж�����\r\n");
	printf("a - ��NANDģ��U�̣�ע����Сд��ĸa\r\n");
	printf("b - �ر�NANDģ��U�̣�ע����Сд��ĸb\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: DotFormatjavascript:;
*	����˵��: �����ݹ淶����ʾ�������û��鿴
*             ����
*             2345678   ---->  2.345.678
*             334426783 ---->  334.426.783
*             ��������Ϊ��λ������ʾ
*	��    ��: _ullVal   ��Ҫ�淶��ʾ����ֵ
*             _sp       �淶��ʾ�����ݴ洢��buf��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DotFormat(uint64_t _ullVal, char *_sp) 
{
	/* ��ֵ���ڵ���10^9 */
	if (_ullVal >= (uint64_t)1e9) 
	{
		_sp += sprintf (_sp, "%d.", (uint32_t)(_ullVal / (uint64_t)1e9));
		_ullVal %= (uint64_t)1e9;
		_sp += sprintf (_sp, "%03d.", (uint32_t)(_ullVal / (uint64_t)1e6));
		_ullVal %= (uint64_t)1e6;
		sprintf (_sp, "%03d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* ��ֵ���ڵ���10^6 */
	if (_ullVal >= (uint64_t)1e6) 
	{
		_sp += sprintf (_sp,"%d.", (uint32_t)(_ullVal / (uint64_t)1e6));
		_ullVal %= (uint64_t)1e6;
		sprintf (_sp,"%03d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* ��ֵ���ڵ���10^3 */
	if (_ullVal >= 1000) 
	{
		sprintf (_sp, "%d.%03d", (uint32_t)(_ullVal / 1000), (uint32_t)(_ullVal % 1000));
		return;
	}
	
	/* ������ֵ */
	sprintf (_sp,"%d",(uint32_t)(_ullVal));
}

/*
*********************************************************************************************************
*	�� �� ��: ViewSDCapacity
*	����˵��: SD���Ĺ��أ�ж�ؼ���������ʾ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void View_Nand_Capacity(void)
{
    extern const char* gc_pcDrv;
	uint8_t result;
	fsMediaInfo info;
	uint64_t ullSdCapacity;
	int32_t id;  
	uint8_t buf[15];
	uint32_t ser_num;
	fsStatus restatus;
	char *opt;
	
	/* ����SD�� */
	result = fmount(gc_pcDrv);
	if(result != NULL)
	{
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", ReVal_Table[result]);
	}
	else
	{
		printf("�����ļ�ϵͳ�ɹ� (%s)\r\n", ReVal_Table[result]);
	}
	
//	/* ��ʽ�� */
//	printf("�ļ�ϵͳ��ʽ��......\r\n");
//	result = fformat ("N0:", gc_pcOpt);
//	printf("�ļ�ϵͳ��ʽ�� (%s)\r\n", ReVal_Table[result]);
	
	printf("------------------------------------------------------------------\r\n");
	
	/* ��ȡvolume label */
	if (fvol (gc_pcDrv, (char *)buf, &ser_num) == 0) 
	{
		if (buf[0]) 
		{
			printf ("NAND��volume label�� %s\r\n", buf);
		}
		else 
		{
			printf ("NANDû��volume label\r\n");
		}
		
		printf ("NAND��volume serial number�� %d\r\n", ser_num);
	}
	else 
	{
		printf ("Volume���ʴ���\r\n");
	}

	/* ��ȡSD��ʣ������ */
	ullSdCapacity = ffree(gc_pcDrv);
	DotFormat(ullSdCapacity, (char *)buf);
	printf("NANDʣ������ = %10s�ֽ�\r\n", buf);
	
	/* ��ȡ��Ӧ�洢�豸�ľ����ע����غ���������IO���Ʋ�����Ч�� */
	id = fs_ioc_get_id(gc_pcDrv);          
   
	/* ���ʵ�ʱ��Ҫ������ */
	fs_ioc_lock (id);
	
	/* ��ʼ��FAT�ļ�ϵͳ��ʽ�Ĵ洢�豸 */

	/* ��ȡ�洢�豸��������Ϣ */
	restatus = fs_ioc_read_info (id, &info);
	if(restatus == fsOK)
	{
		/* �ܵ������� * ������С��SD����������С��512�ֽ� */
		ullSdCapacity = (uint64_t)info.block_cnt << 9;
		DotFormat(ullSdCapacity, (char *)buf);
		printf("NAND������ = %10s�ֽ�\r\nSD������������ = %d \r\n", buf, info.block_cnt);
	}
	else
	{
		printf("��ȡ������Ϣʧ�� %s\r\n", ReVal_Table[restatus]);
	}
	
	/* ���ʽ���Ҫ���� */
	fs_ioc_unlock (id);

	printf("NAND��������С = %d�ֽ�\r\n", info.read_blen);
	printf("NANDд������С = %d�ֽ�\r\n", info.write_blen);
	
	/* ж��SD�� */
	result = funmount(gc_pcDrv);
	if(result != NULL)
	{
		printf("ж���ļ�ϵͳʧ��\r\n");
	}
	else
	{
		printf("ж���ļ�ϵͳ�ɹ�\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
}


/*
*********************************************************************************************************
*	�� �� ��: CreateNewFile
*	����˵��: ��SD����������text�ı����ֱ�ʹ��fwrite��fprintf��fputsд���ݡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void CreateNewFile(void)
{
	const uint8_t WriteText[] = {"TEST 000\r\n"};
	const uint8_t WriteText1[] = {"TEST 111\r"};
	FILE *fout;
	uint32_t bw;
	uint32_t i = 2;
	uint8_t result;
    char cDir[128] = {0};

	/* ����SD�� */
	result = fmount(gc_pcDrv);
	if(result != NULL)
	{
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("�����ļ�ϵͳ�ɹ� (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* ���ļ���test�е��ļ�test1.txt�����û�����ļ��к�txt�ļ����Զ�����*/
    sprintf(cDir, "%s%s", gc_pcDrv, "\\test\\test1.txt");
    printf("%s\r\n", cDir);
    
	fout = fopen (cDir, "w"); 
	if (fout != NULL) 
	{
		printf("���ļ�N0:\\test\\test1.txt�ɹ������û�����ļ��к�txt�ļ����Զ�����\r\n");
		/* д���� */
		bw = fwrite (WriteText, sizeof(uint8_t), sizeof(WriteText)/sizeof(uint8_t), fout);
		if(bw == sizeof(WriteText)/sizeof(uint8_t))
		{
			printf("д�����ݳɹ�\r\n");
		}
		else
		{ 
			printf("д������ʧ��\r\n");
		}
		
		/* �ر��ļ� */
		fclose(fout);
	}
	else
	{
		printf("���ļ�N0:\\test\\test.txtʧ��\r\n");
	}
	
	/**********************************************************************************************************/
	/* ���ļ���test�е��ļ�test2.txt�����û�����ļ��к�txt�ļ����Զ�����*/
    memset(cDir, 0, sizeof(cDir));
    sprintf(cDir, "%s%s", gc_pcDrv, "\\test\\test2.txt");
    printf("%s\r\n", cDir);
    
	fout = fopen (cDir, "w"); 
	if (fout != NULL) 
	{
		printf("���ļ�N0:\\test\\test2.txt�ɹ������û�����ļ��к�txt�ļ����Զ�����\r\n");
		
		/* д���� */
		bw = fprintf (fout, "%d %d %f\r\n", i, i*5, i*5.55f);
		
		/* ʹ�ú���ferror����Ƿ��������� */
		if (ferror(fout) != NULL)  
		{
			printf("д������ʧ��\r\n");
		}
		else
		{
			printf("д�����ݳɹ�\r\n");	
		}
		
		/* �ر��ļ� */
		fclose(fout);
	}
	else
	{
		printf("���ļ�M0:\\test\\test.txtʧ��\r\n");
	}
	
	/**********************************************************************************************************/
	/* ���ļ���test�е��ļ�test3.txt�����û�����ļ��к�txt�ļ����Զ�����*/
    memset(cDir, 0, sizeof(cDir));
    sprintf(cDir, "%s%s", gc_pcDrv, "\\test\\test3.txt");
    printf("%s\r\n", cDir);
    
	fout = fopen (cDir, "w"); 
	if (fout != NULL) 
	{
		printf("���ļ�N0:\\test\\test3.txt�ɹ������û�����ļ��к�txt�ļ����Զ�����\r\n");
		
		/* д���� */
		fputs((const char *)WriteText1, fout);
		
		/* ʹ�ú���ferror����Ƿ��������� */
		if (ferror(fout) != NULL)  
		{
			printf("д������ʧ��\r\n");
		}
		else
		{
			printf("д�����ݳɹ�\r\n");	
		}
		
		/* �ر��ļ� */
		fclose(fout);
	}
	else
	{
		printf("���ļ�N0:\\test\\test.txtʧ��\r\n");
	}

access_fail:	
	/* ж��SD�� */
	result = funmount(gc_pcDrv);
	if(result != NULL)
	{
		printf("ж���ļ�ϵͳʧ��\r\n");
	}
	else
	{
		printf("ж���ļ�ϵͳ�ɹ�\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: ReadFileData
*	����˵��: �ֱ�ʹ��fread��fscan��fgets��ȡ������ͬ��txt�ļ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ReadFileData(void)
{
	const uint8_t WriteText[] = {"�人�������������޹�˾\r\n2015-09-06\r\nwww.armfly.com\r\nWWW.ARMFLY.COM"};
	uint8_t Readbuf[100];
	FILE *fin;
	uint32_t bw;
	uint32_t index1, index2;
	float  count = 0.0f;
	uint8_t result;

	/* ����SD�� */
	result = fmount(gc_pcDrv);
	if(result != NULL)
	{
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("�����ļ�ϵͳ�ɹ� (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* ���ļ���test�е��ļ�test1.txt */
	fin = fopen ("N0:\\test\\test1.txt", "r"); 
	if (fin != NULL) 
	{
		printf("<1>���ļ�M0:\\test\\test1.txt�ɹ�\r\n");
		
		/* ��ֹ���� */
		(void) WriteText;
		
		/* ������ */
		bw = fread(Readbuf, sizeof(uint8_t), sizeof(WriteText)/sizeof(uint8_t), fin);
		if(bw == sizeof(WriteText)/sizeof(uint8_t))
		{
			Readbuf[bw] = NULL;
			printf("test1.txt �ļ����� : \r\n%s\r\n", Readbuf);
		}
		else
		{ 
			printf("������ʧ��\r\n");
		}
		
		/* �ر��ļ� */
		fclose(fin);
	}
	else
	{
		printf("���ļ�N0:\\test\\test.txtʧ��, �����ļ�������\r\n");
	}
	
	/**********************************************************************************************************/
	/* ���ļ���test�е��ļ�test2.txt */
	fin = fopen ("N0:\\test\\test2.txt", "r"); 
	if (fin != NULL) 
	{
		printf("\r\n<2>���ļ�M0:\\test\\test2.txt�ɹ�\r\n");
		
		bw = fscanf(fin, "%d %d %f", &index1, &index2, &count);

		/* 3�������������� */
		if (bw == 3)  
		{
			printf("��������ֵ\r\nindex1 = %d index2 = %d count = %f\r\n", index1, index2, count);
		}
		else
		{
			printf("������ʧ��\r\n");	
		}
		
		/* �ر��ļ� */
		fclose(fin);
	}
	else
	{
		printf("���ļ�N0:\\test\\test.txtʧ��(%s)\r\n", ReVal_Table[result]);
	}
	
	/**********************************************************************************************************/
	/* ���ļ���test�е��ļ�test3.txt�����û�����ļ��к�txt�ļ����Զ�����*/
	fin = fopen ("N0:\\test\\test3.txt", "r"); 
	if (fin != NULL) 
	{
		printf("\r\n<3>���ļ�N0:\\test\\test3.txt�ɹ�\r\n");
		
		/* ������ */
		if(fgets((char *)Readbuf, sizeof(Readbuf), fin) != NULL)
		{
			printf("test3.txt �ļ����� : \r\n%s\r\n", Readbuf);
		}
		else
		{
			printf("������ʧ��\r\n");
		}
		
		/* �ر��ļ� */
		fclose(fin);
	}
	else
	{
		printf("���ļ�N0:\\test\\test.txtʧ��\r\n");
	}
	
access_fail:
	/* ж��SD�� */
	result = funmount(gc_pcDrv);
	if(result != NULL)
	{
		printf("ж���ļ�ϵͳʧ��\r\n");
	}
	else
	{
		printf("ж���ļ�ϵͳ�ɹ�\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: SeekFileData
*	����˵��: ����һ��text�ı�����ָ��λ�ö�����ж�д������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void SeekFileData(void)
{
	const uint8_t WriteText[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};
	FILE *fin, *fout;
	uint32_t bw;
	uint32_t uiPos;
	uint8_t ucChar;
	uint8_t result;

	/* ����SD�� */
	result = fmount(gc_pcDrv);
	if(result != NULL)
	{
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("�����ļ�ϵͳ�ɹ� (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* ���ļ���test�е��ļ�test1.txt�����û�����ļ��к�txt�ļ����Զ�����*/
	fout = fopen ("N0:\\test.txt", "w"); 
	if (fout != NULL) 
	{
		printf("���ļ�N0:\\test.txt�ɹ������û��txt�ļ����Զ�����\r\n");
		/* д���� */
		bw = fwrite (WriteText, sizeof(uint8_t), sizeof(WriteText)/sizeof(uint8_t), fout);
		if(bw == sizeof(WriteText)/sizeof(uint8_t))
		{
			printf("д�����ݳɹ�\r\n");
		}
		else
		{ 
			printf("д������ʧ��\r\n");
		}
			
		/* �ر��ļ� */
		fclose(fout);
	}
	else
	{
		printf("���ļ�N0:\\test.txtʧ��\r\n");
	}
	
	/***********************************************/
	fin = fopen ("N0:\\test.txt","r");
	if (fin != NULL)  
	{
		printf("\r\n���ļ�N0:\\test.txt�ɹ�\r\n");
		
		/* ��ȡ�ļ�test.txt��λ��0���ַ� */
		fseek (fin, 0L, SEEK_SET);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);		
		printf("�ļ�test.txt��ǰ��ȡλ�ã�%02d���ַ���%c\r\n", uiPos, ucChar);
		
		/* ��ȡ�ļ�test.txt��λ��1���ַ� */
		fseek (fin, 1L, SEEK_SET);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);		
		printf("�ļ�test.txt��ǰ��ȡλ�ã�%02d���ַ���%c\r\n", uiPos, ucChar);

		/* ��ȡ�ļ�test.txt��λ��25���ַ� */
		fseek (fin, 25L, SEEK_SET);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);		
		printf("�ļ�test.txt��ǰ��ȡλ�ã�%02d���ַ���%c\r\n", uiPos, ucChar);
		
		/* ͨ�����溯���Ĳ�������ǰ��дλ����26
		   ���溯�����ڵ�ǰλ�õĻ����Ϻ���2��λ�ã�Ҳ����24�����ú���fgetc��λ�þ���25
		 */
		fseek (fin, -2L, SEEK_CUR);
		uiPos = ftell(fin); 	
		ucChar = fgetc (fin);		
		printf("�ļ�test.txt��ǰ��ȡλ�ã�%02d���ַ���%c\r\n", uiPos, ucChar);
		
		/* ��ȡ�ļ�test.txt�ĵ�����2���ַ�, ���һ����'\0' */
		fseek (fin, -2L, SEEK_END); 
		uiPos = ftell(fin); 
		ucChar = fgetc (fin);
		printf("�ļ�test.txt��ǰ��ȡλ�ã�%02d���ַ���%c\r\n", uiPos, ucChar);
		
		/* ����ȡλ�����¶�λ���ļ���ͷ */
		rewind(fin);  
		uiPos = ftell(fin); 
		ucChar = fgetc (fin);
		printf("�ļ�test.txt��ǰ��ȡλ�ã�%02d���ַ���%c\r\n", uiPos, ucChar);	
		
		/* 
		   ��������ʾһ��ungetc�����ã��˺������ǽ���ǰ�Ķ�ȡλ��ƫ�ƻ�һ���ַ���
		   ��fgetc���ú�λ������һ���ַ���
		 */
		fseek (fin, 0L, SEEK_SET);
		ucChar = fgetc (fin);
		uiPos = ftell(fin); 
		printf("�ļ�test.txt��ǰ��ȡλ�ã�%02d���ַ���%c\r\n", uiPos, ucChar);
		ungetc(ucChar, fin); 
		uiPos = ftell(fin); 
		printf("�ļ�test.txt��ǰ��ȡλ�ã�%02d���ַ���%c\r\n", uiPos, ucChar);
		
		/* �ر�*/
		fclose (fin);
	}
	else
	{
		printf("���ļ�N0:\\test.txtʧ��\r\n");
	}
	
access_fail:	
	/* ж��SD�� */
	result = funmount(gc_pcDrv);
	if(result != NULL)
	{
		printf("ж���ļ�ϵͳʧ��\r\n");
	}
	else
	{
		printf("ж���ļ�ϵͳ�ɹ�\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
	
}

/*
*********************************************************************************************************
*	�� �� ��: DeleteDirFile
*	����˵��: ɾ���ļ��к��ļ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DeleteDirFile(void)
{
	uint8_t result;
	uint8_t i;
	fsStatus restatus;
	char FileName[50];
	
	/* ����SD�� */
	result = fmount(gc_pcDrv);
	if(result != NULL)
	{
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("�����ļ�ϵͳ�ɹ� (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/* ɾ���ļ� speed1.txt */
	for (i = 0; i < 20; i++)
	{
		sprintf(FileName, "N0:\\Speed%02d.txt", i);		/* ÿд1�Σ���ŵ��� */
		restatus = fdelete (FileName, NULL);
		if (restatus != NULL) 
		{
			printf("%s�ļ������ڣ�����ֵ��%d��\r\n", FileName, result);
		}
		else
		{
			printf("ɾ��%s�ļ��ɹ�\r\n", FileName);
		}
	}
	
	for(i = 1; i < 4; i++)
	{
		sprintf(FileName, "N0:\\test\\test%01d.txt", i);   /* ÿд1�Σ���ŵ��� */
		restatus = fdelete (FileName, NULL);
		if (restatus != NULL) 
		{
			printf("%s�ļ������ڣ�����ֵ��%d��\r\n", FileName, result);
		}
		else
		{
			printf("ɾ��%s�ļ��ɹ�\r\n", FileName);
		}	
	}
	
	/* ɾ���ļ���test */
	restatus = fdelete ("N0:\\test\\", NULL);
	if (restatus != NULL) 
	{
		printf("test�ļ��зǿջ򲻴��ڣ�����ֵ��%d��\r\n", result);
	}
	else
	{
		printf("ɾ��test�ļ��гɹ�\r\n");
	}
	
	/* ɾ��csv�ļ� */
	restatus = fdelete ("N0:\\record.csv", NULL);
	if (restatus != NULL) 
	{
		printf("record.csv�ļ������ڣ�����ֵ��%d��\r\n", result);
	}
	else
	{
		printf("ɾ��record.csv���ɹ�\r\n");
	}
	
access_fail:
	/* ж��SD�� */
	result = funmount(gc_pcDrv);
	if(result != NULL)
	{
		printf("ж���ļ�ϵͳʧ��\r\n");
	}
	else
	{
		printf("ж���ļ�ϵͳ�ɹ�\r\n");
	}
		
	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: WriteFileTest
*	����˵��: �����ļ���д�ٶ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void WriteFileTest(void)
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

	/* ����SD�� -----------------------------------------*/
	result = fmount(gc_pcDrv);
	if(result != NULL)
	{
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("�����ļ�ϵͳ�ɹ� (%s)\r\n", ReVal_Table[result]);
	}
	
	printf("------------------------------------------------------------------\r\n");

	/* ���ļ� -----------------------------------------*/
	sprintf(TestFileName, "Speed%02d.txt", s_ucTestSn++);		/* ÿд1�Σ���ŵ��� */
	fout = fopen (TestFileName, "w");
	if (fout ==  NULL)
	{
		printf("�ļ�: %s����ʧ��\r\n", TestFileName);
		goto access_fail;
	}
	
	/* дһ������ ����д�ٶ�-----------------------------------------*/
	printf("��ʼд�ļ�%s %dKB ...\r\n", TestFileName, TEST_FILE_LEN / 1024);
	runtime1 = osKernelGetTickCount();	/* ��ȡϵͳ����ʱ�� */
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
			printf("%s�ļ�дʧ��\r\n", TestFileName);
			break;
		}
	}
	runtime2 = osKernelGetTickCount();	/* ��ȡϵͳ����ʱ�� */

	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("\r\n  д��ʱ : %dms   ƽ��д�ٶ� : %dB/S (%dKB/S)\r\n",
			timelen,
			(TEST_FILE_LEN * 1000) / timelen,
			((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}
	
	/* ʹ�ú���ferror�ٴμ���Ƿ��������� -----------------------*/
	if (ferror (fout))  
	{
		printf ("д���ݹ����г��ֹ�����\r\n\n");
    }
	else
	{
		printf ("д���ݹ�����δ���ֹ�����\r\n\n");		
	}

	fclose (fout);	/* �ر��ļ�*/


	/* ��ʼ���ļ������Զ��ٶ� ---------------------------------------*/
	fout = fopen (TestFileName, "r");
	if (fout ==  NULL)
	{
		printf("�ļ�: %s��ȡʧ��\r\n", TestFileName);
		goto access_fail;
	}

	printf("��ʼ���ļ� %dKB ...\r\n", TEST_FILE_LEN / 1024);
	runtime1 = osKernelGetTickCount();	/* ��ȡϵͳ����ʱ�� */
	for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
	{
		bw = fread(g_TestBuf, sizeof(uint8_t), BUF_SIZE, fout);   
		if (bw == BUF_SIZE)
		{
			if (((i + 1) % 8) == 0)
			{
				printf(".");
			}

			/* �Ƚ�д��������Ƿ���ȷ�������ᵼ�¶����ٶȽ������ */
			for (k = 0; k < sizeof(g_TestBuf); k++)
			{
				if (g_TestBuf[k] != (k / 512) + '0')
				{
				  	err = 1;
					printf("Speed1.txt �ļ����ɹ����������ݳ���\r\n");
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
			printf("Speed1.txt �ļ���ʧ��\r\n");
			break;
		}
	}
	runtime2 = osKernelGetTickCount();	/* ��ȡϵͳ����ʱ�� */

	if (err == 0)
	{
		timelen = (runtime2 - runtime1);
		printf("\r\n  ����ʱ : %dms   ƽ�����ٶ� : %dB/S (%dKB/S)\r\n", timelen,
			(TEST_FILE_LEN * 1000) / timelen, ((TEST_FILE_LEN / 1024) * 1000) / timelen);
	}
	
	/* ʹ�ú���ferror�ٴμ���Ƿ��������� -----------------------*/
	if (ferror (fout))  
	{
		printf ("�����ݹ����г��ֹ�����\r\n");
    }
	else
	{
		printf ("�����ݹ�����δ���ֹ�����\r\n");		
	}

	fclose (fout);	/* �ر��ļ�*/
	
access_fail:
	/* ж��SD�� */
	result = funmount(gc_pcDrv);
	if(result != NULL)
	{
		printf("ж���ļ�ϵͳʧ��\r\n");
	}
	else
	{
		printf("ж���ļ�ϵͳ�ɹ�\r\n");
	}
	
	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: WriteCSVFile
*	����˵��: д���ݵ�CSV�ļ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void WriteCSVFile(void)
{
	const uint8_t WriteText[] = {"��¼ʱ��,���,��¼��ֵһ,��¼��ֵ��\r\n"};
	FILE *fout;
	uint8_t result;
	static uint16_t i = 1;
	static uint8_t ucFirstRunFlag = 0;

	/* ����SD�� */
	result = fmount(gc_pcDrv);
	if(result != NULL)
	{
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("�����ļ�ϵͳ�ɹ� (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* 
	  1. ���ļ�record.csv�����û�д��ļ����Զ�������
	  2. �ڶ���������ʾ����ļ�д���ݶ���β����ʼ��ӡ�
	*/
	fout = fopen ("N0:\\record.csv", "a"); 
	if (fout != NULL) 
	{
		printf("���ļ�N0:\\record.csvt�ɹ������û�д��ļ����Զ�����\r\n");
		
		/* д���ݣ�����ǵ�һ��д���ݣ���дCSV�ļ��ı������Ŀ���Ժ�д���ݲ�����Ҫд�˱��� */
		if(ucFirstRunFlag == 0)
		{
			fprintf(fout, (char *)WriteText);
			ucFirstRunFlag = 1;
		}
		
		/* ����д5������ */
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
		
		/* ʹ�ú���ferror����Ƿ��������� */
		if (ferror(fout) != NULL)  
		{
			printf("д������ʧ��\r\n");
		}
		else
		{
			printf("д�����ݳɹ�\r\n");	
		}
		
		/* �ر��ļ� */
		fclose(fout);
	}
	else
	{
		printf("���ļ�M0:\\record.csvʧ��\r\n");
	}

access_fail:	
	/* ж��SD�� */
	result = funmount(gc_pcDrv);
	if(result != NULL)
	{
		printf("ж���ļ�ϵͳʧ��\r\n");
	}
	else
	{
		printf("ж���ļ�ϵͳ�ɹ�\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: ReadCSVFileData
*	����˵��: ��ȡCSV�ļ��е����ݡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ReadCSVFileData(void)
{
	uint8_t Readbuf[50];
	FILE *fin;
	uint8_t result;

	/* ����SD�� */
	result = fmount(gc_pcDrv);
	if(result != NULL)
	{
		/* �������ʧ�ܣ���ز�Ҫ�ٵ���FlashFS������API��������ֹ����Ӳ���쳣 */
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", ReVal_Table[result]);
		goto access_fail;
	}
	else
	{
		printf("�����ļ�ϵͳ�ɹ� (%s)\r\n", ReVal_Table[result]);
	}
	printf("------------------------------------------------------------------\r\n");
	
	/**********************************************************************************************************/
	/* ��record.csv�ļ� */
	fin = fopen ("N0:\\record.csv", "r"); 
	if (fin != NULL) 
	{
		printf("���ļ�N0:\\record.csv�ɹ�\r\n");
		
		/* ���������ݶ���������ʹ��fgets�����ĺô����ǿ���һ��һ�еĶ�ȡ����Ϊ�˺�������'\n'���з��ͻ�ֹͣ��ȡ */
		while (fgets((char *)Readbuf, sizeof(Readbuf), fin) != NULL)  
		{
			printf("%s", Readbuf);	
		}
		
		/* ʹ�ú���ferror����Ƿ��������� */
		if (ferror(fin) != NULL)  
		{
			printf("��������ʧ��\r\n");
		}
		else
		{
			printf("record.csv���������ȫ����ȷ����\r\n");	
		}
		
		/* �ر��ļ� */
		fclose(fin);
	}
	else
	{
		printf("���ļ�N0:\\record.csvʧ��\r\n");
	}
	
access_fail:
	/* ж��SD�� */
	result = funmount(gc_pcDrv);
	if(result != NULL)
	{
		printf("ж���ļ�ϵͳʧ��\r\n");
	}
	else
	{
		printf("ж���ļ�ϵͳ�ɹ�\r\n");
	}

	printf("------------------------------------------------------------------\r\n");
}



