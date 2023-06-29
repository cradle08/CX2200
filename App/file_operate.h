#ifndef __FILE_OPERATE_H__
#define __FILE_OPERATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "msg_def.h"
#include "bc.h"
//#include "ff.h"


    
/****************************
*
*   1��WBC�������ݡ���־������Headͳһ��������˫��ѭ������
*   2��������־������˫��ѭ������
*   3���ʿ����ݣ���������־��������˫��������ѭ���������洢������ʾ�쳣����ɾ�����һ��������ɾ���ļ��ŵķ�ʽ��Ӧȫ�����ݣ�
*
*
****************************/

//���������ݴ洢���������30M��ʣ�����2M����������
#define SPI_FLASH_DATA_MAX_LEN      0x01E00000  //30M
#define SPACE_SPIFLASH_LEN          0x19000     //100k
#define COUNT_LOG_DEFAULT_MAX_LEN   LOG_INFO_MAX_LEN      //4k



//��ǰWBCHGB���ݴ�С:1541B, ͷ��58B
/************************ outside wbc hgb (��0M��ʼ)*************************************/
//wbc hgb ������Ϣͷ
#define OUTSIDE_WBCHGB_HEAD_SPIFLASH_START_ADDR   0x0        
#define OUTSIDE_WBCHGB_HEAD_SPIFLASH_MAX_LEN      0x0004B000 //300k      //58*4500=254k=(4500��/254k)  //, 

//wbc hgb����
#define OUTSIDE_WBCHGB_DATA_SPIFLASH_START_ADDR   OUTSIDE_WBCHGB_HEAD_SPIFLASH_MAX_LEN  //0x0        
#define OUTSIDE_WBCHGB_DATA_SPIFLASH_MAX_LEN      0x6C0000   //1550*4500=6975000=0x6A7000(6.65M)   //����4K���� +0x19000=100k

//wbc hgb��־
#define OUTSIDE_WBCHGB_LOG_SPIFLASH_START_ADDR    (OUTSIDE_WBCHGB_HEAD_SPIFLASH_MAX_LEN+OUTSIDE_WBCHGB_DATA_SPIFLASH_MAX_LEN+SPACE_SPIFLASH_LEN)        
#define OUTSIDE_WBCHGB_LOG_SPIFLASH_MAX_LEN       0x01194000   //4500*4096=18000k=17M=() //����4K����

//End Addr 0x18B8000=25312k

/************************ inner wbc hgb (��25600k=25.6M-=0x1900000)��ʼ) *************************************/
//wbc hgb ������Ϣͷ
#define INNER_WBCHGB_HEAD_SPIFLASH_START_ADDR     0x1900000         
#define INNER_WBCHGB_HEAD_SPIFLASH_MAX_LEN        0x0000D000//0x0000C800 //50k   //58*500=29000=28k=0x0000639C(500��/28k)    

//wbc hgb����
#define INNER_WBCHGB_DATA_SPIFLASH_START_ADDR     (INNER_WBCHGB_HEAD_SPIFLASH_START_ADDR+INNER_WBCHGB_HEAD_SPIFLASH_MAX_LEN)         
#define INNER_WBCHGB_DATA_SPIFLASH_MAX_LEN        0x000C6000   //1550*500=775000=0xBD358=756K=(500��/756k)    //����4K���� +0x8000=32k

//wbc hgb��־
#define INNER_WBCHGB_LOG_SPIFLASH_START_ADDR      (INNER_WBCHGB_DATA_SPIFLASH_START_ADDR+INNER_WBCHGB_DATA_SPIFLASH_MAX_LEN+SPACE_SPIFLASH_LEN)        //+100k 
#define INNER_WBCHGB_LOG_SPIFLASH_MAX_LEN         0x001F4000   //4096*500=2000k=0x1F4000=(500��/1.96M)   //����4K����

//End Addr 0x1BD F800 = 28542k

/************************ ������־ (��28672k=0x1C00000��ʼ) *************************************/
//������־
#define RUN_LOG_SPIFLASH_START_ADDR               0x1C00000    //0x1CC3000    //0x1CC3000//0x01D7D000      
#define RUN_LOG_SPIFLASH_MAX_LEN                  0x0004B000 //300k   //582*500=284k=0x000470B8(500��/284k) // 300k

//End Addr 0x1C0 4B00 = 28972k

/************************ QC (��29M=��ʼ) *************************************/
//QC_WBCHGB_TestInfo_t:1447
//QC��Ϣ
#define QC_DATA_SPIFLASH_START_ADDR               0x1D00000       
#define QC_DATA_SPIFLASH_MAX_LEN                  0x000E4000   //1550*600=930000=0x000E4000(500��/912k)    //����4K����



//
uint32_t Get_WBCHGB_Head_SPI_Addr(void);
uint32_t Get_WBCHGB_Data_SPI_Addr(void);
uint32_t Get_WBCHGB_Log_SPI_Addr(void);
uint32_t Get_QC_WBCHGB_Data_SPI_Addr(QC_FILENUM_INDEX_e eFileNumIndex);
uint32_t Get_Run_Log_SPI_Addr(void);



extern const char * ReVal_Table[];


#if USE_RL_FLASH_FS 
#define DRV_PATH    RL_FLASH_FS_NAND_DRV_PATH
#else
#define DRV_PATH USERPath
#endif


// �ļ�����
typedef enum {
	FILE_TYPE_COUNT_DATA 			= 0,		//�����ļ�����
    FILE_TYPE_CONFIG				= 1,		//�����ļ�����
    FILE_TYPE_COUNT_LOG          	= 2,		//������־�ļ�����
    FILE_TYPE_COUNT_HEAD          	= 3,		//������־�ļ�����
    
//	FILE_TYPE_COUNT_LOG_HEAD     	= 6,		//������־ͷ�ļ�����
//	LOG_TYPE_COUNT_LOG     	        = 7,		//������־�ַ����ļ�����
//    FILE_TYPE_OPERATE_LOG   		= 8,		//������־�ļ�����
//    FILE_TYPE_PARAM_SET_LOG 		= 9,		//����������־�ļ�����
	
}FileType_e;


// ��־����
typedef enum {

	LOG_TYPE_COUNT_LOG     	= 6,		//������־
	LOG_TYPE_RUN     	    = 7,		//������־
    LOG_TYPE_OPERATE   		= 8,		//������־
    LOG_TYPE_PARAM_SET 		= 9,		//����������־	

}LogType_e;



/***
*�����ļ�ϵͳʧ���Ƿ���Ҫ��������ö��
***/
typedef enum _FSMountLock_e{
	FS_MOUNT_LOCK = 0,
	FS_MOUNT_NOT_LOCK
}FSMountLock_e;






//
uint16_t Count_Log_Output(LogType_e eLogType, __IO uint8_t *pLogBuffer, uint16_t usIndex, const char *format, ...);
//uint16_t Error_Log_Output(uint8_t *pLogBuffer, uint16_t usIndex, const char *format, ...);
//
void Update_ListReview_DataTable(void);
FeedBack_e Update_UI_ListView_Data(uint16_t usCmd, uint8_t ucIndex);
FeedBack_e UI_ListView_Data_Init(__IO DataManage_t *ptDataManage);

//
void Machine_Factory_Init(void);
FeedBack_e Del_Account_DataFile(AccountDataType_e eAccountDataType); //ɾ���ڲ����ⲿ�˺����ɵ������ļ���head/data/log��
void DemoFlashFS(uint8_t ucCmd);


//
void FS_LowLevelInit(void);
FeedBack_e FS_Mount(FSMountLock_e FSMountLockState);
FeedBack_e FS_Unmount(void);


FeedBack_e FS_Format(void);
void Print_Dir(char *pcaDir);   //��ʾָ��·���������ļ�
FeedBack_e Del_File(char *pcFile);       //ɾ�������ļ�
FeedBack_e Del_Dir_Files(char *pcaDir);  //ɾ�� ָ��Ŀ¼�µ������ļ�(���������ļ���)
FeedBack_e FatFs_DelFileSubDir(char* Path); //ɾ��ָ��·���������ļ�����Ŀ¼
FeedBack_e Nand_Volume_Info(uint32_t *pulTotalSect, uint32_t *pulFreeSect);
FeedBack_e FatFs_WriteNewFile(uint8_t *pFileName, uint8_t *pWriteBuffer, uint32_t ulWriteLen);  //д�ļ�
FeedBack_e FatFs_ReadAllFile(uint8_t *pFileName, uint8_t *pReadBuffer, uint32_t *pulReadLen);   //��ȡ�����ļ�
FeedBack_e FatFs_IsExistFile(char* Path);   //����ļ��Ƿ����
FeedBack_e FatFs_Write_File(uint8_t *pFileName, uint32_t ulWritePos, uint8_t *pWriteBuffer, uint32_t ulWriteLen);
FeedBack_e FatFs_Read_File(uint8_t *pFileName, uint32_t ulReadPos, uint8_t *pReadBuffer, uint32_t ulReadLen);

//FeedBack_e FatFs_Write_ErrorLog(uint8_t *pLogBuffer, uint16_t usLogLen);
//FeedBack_e FatFs_Read_ErrorLog(uint8_t *pLogBuffer, uint32_t ulMaxReadLen, uint32_t*pulReadLen);


/*** ������ʷԭ�����½ӿ������� FatFs_***��ͷ�����Ǻ�fatfsû��ϵ��Ҳδʹ��Fat�ļ�ϵͳ������ʹ�ã���δʹ�ã�  ***/
//write
FeedBack_e Fatfs_Write_RunLog(uint32_t ulIndex, RunLog_s* RunLog);
FeedBack_e FatFs_Write_CountHead(uint32_t ulIndex, CountHead_t *ptCountHead);
//FeedBack_e FatFs_Write_ConutLog(uint32_t ulIndex, uint32_t ulWritePos, uint8_t *pLogBuffer, uint16_t usLogLen);
FeedBack_e FatFs_Write_ConutLog(uint32_t ulIndex, CountLog_t *ptCountLog, uint16_t usLogLen);
FeedBack_e FatFs_Write_WBCHGB_Info(uint32_t ulIndex, __IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);
FeedBack_e FatFs_Write_QC_WBCHGB_Info(QC_FILENUM_INDEX_e eFileNumIndex, uint32_t ulIndex, __IO QC_WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);
//


//read
FeedBack_e FatFs_Read_RunLog(uint32_t ulIndex, RunLog_s* RunLog);
FeedBack_e UI_Get_Lastet_WBCHGB_TestInfo(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo, __IO DataManage_t *ptDataManage);
FeedBack_e FatFs_Read_ConutHead(uint32_t ulIndex, CountHead_t *ptCountHead);
//FeedBack_e FatFs_Read_ConutLog(uint32_t ulIndex, uint32_t ulPos, uint8_t *pLogBuffer, uint16_t usLen);
FeedBack_e FatFs_Read_ConutLog(uint32_t ulIndex, __IO CountLog_t *ptCountLog, uint16_t usLen);
FeedBack_e FatFs_Read_WBCHGB_Info(uint32_t ulIndex, __IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);
FeedBack_e FatFs_Read_QC_WBCHGB_Info(QC_FILENUM_INDEX_e eFileNumIndex, uint32_t ulIndex, __IO QC_WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);


#ifdef __cplusplus
}
#endif

#endif //__FILE_OPERATE_H__


