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
*   1、WBC测试数据、日志：采用Head统一管理，采用双向循环链表
*   2、运行日志，采用双向循环链表
*   3、质控数据（不保存日志），采用双向链表（不循环），（存储满后，提示异常，可删除最后一条，或是删除文件号的方式对应全部数据）
*
*
****************************/

//可用于数据存储的最大容量30M（剩下最后2M用于升级）
#define SPI_FLASH_DATA_MAX_LEN      0x01E00000  //30M
#define SPACE_SPIFLASH_LEN          0x19000     //100k
#define COUNT_LOG_DEFAULT_MAX_LEN   LOG_INFO_MAX_LEN      //4k



//当前WBCHGB数据大小:1541B, 头：58B
/************************ outside wbc hgb (从0M开始)*************************************/
//wbc hgb 公共信息头
#define OUTSIDE_WBCHGB_HEAD_SPIFLASH_START_ADDR   0x0        
#define OUTSIDE_WBCHGB_HEAD_SPIFLASH_MAX_LEN      0x0004B000 //300k      //58*4500=254k=(4500条/254k)  //, 

//wbc hgb数据
#define OUTSIDE_WBCHGB_DATA_SPIFLASH_START_ADDR   OUTSIDE_WBCHGB_HEAD_SPIFLASH_MAX_LEN  //0x0        
#define OUTSIDE_WBCHGB_DATA_SPIFLASH_MAX_LEN      0x6C0000   //1550*4500=6975000=0x6A7000(6.65M)   //补齐4K对齐 +0x19000=100k

//wbc hgb日志
#define OUTSIDE_WBCHGB_LOG_SPIFLASH_START_ADDR    (OUTSIDE_WBCHGB_HEAD_SPIFLASH_MAX_LEN+OUTSIDE_WBCHGB_DATA_SPIFLASH_MAX_LEN+SPACE_SPIFLASH_LEN)        
#define OUTSIDE_WBCHGB_LOG_SPIFLASH_MAX_LEN       0x01194000   //4500*4096=18000k=17M=() //补齐4K对齐

//End Addr 0x18B8000=25312k

/************************ inner wbc hgb (从25600k=25.6M-=0x1900000)开始) *************************************/
//wbc hgb 公共信息头
#define INNER_WBCHGB_HEAD_SPIFLASH_START_ADDR     0x1900000         
#define INNER_WBCHGB_HEAD_SPIFLASH_MAX_LEN        0x0000D000//0x0000C800 //50k   //58*500=29000=28k=0x0000639C(500条/28k)    

//wbc hgb数据
#define INNER_WBCHGB_DATA_SPIFLASH_START_ADDR     (INNER_WBCHGB_HEAD_SPIFLASH_START_ADDR+INNER_WBCHGB_HEAD_SPIFLASH_MAX_LEN)         
#define INNER_WBCHGB_DATA_SPIFLASH_MAX_LEN        0x000C6000   //1550*500=775000=0xBD358=756K=(500条/756k)    //补齐4K对齐 +0x8000=32k

//wbc hgb日志
#define INNER_WBCHGB_LOG_SPIFLASH_START_ADDR      (INNER_WBCHGB_DATA_SPIFLASH_START_ADDR+INNER_WBCHGB_DATA_SPIFLASH_MAX_LEN+SPACE_SPIFLASH_LEN)        //+100k 
#define INNER_WBCHGB_LOG_SPIFLASH_MAX_LEN         0x001F4000   //4096*500=2000k=0x1F4000=(500条/1.96M)   //补齐4K对齐

//End Addr 0x1BD F800 = 28542k

/************************ 运行日志 (从28672k=0x1C00000开始) *************************************/
//运行日志
#define RUN_LOG_SPIFLASH_START_ADDR               0x1C00000    //0x1CC3000    //0x1CC3000//0x01D7D000      
#define RUN_LOG_SPIFLASH_MAX_LEN                  0x0004B000 //300k   //582*500=284k=0x000470B8(500条/284k) // 300k

//End Addr 0x1C0 4B00 = 28972k

/************************ QC (从29M=开始) *************************************/
//QC_WBCHGB_TestInfo_t:1447
//QC信息
#define QC_DATA_SPIFLASH_START_ADDR               0x1D00000       
#define QC_DATA_SPIFLASH_MAX_LEN                  0x000E4000   //1550*600=930000=0x000E4000(500条/912k)    //补齐4K对齐



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


// 文件类型
typedef enum {
	FILE_TYPE_COUNT_DATA 			= 0,		//数据文件类型
    FILE_TYPE_CONFIG				= 1,		//配置文件类型
    FILE_TYPE_COUNT_LOG          	= 2,		//计数日志文件类型
    FILE_TYPE_COUNT_HEAD          	= 3,		//计数日志文件类型
    
//	FILE_TYPE_COUNT_LOG_HEAD     	= 6,		//计数日志头文件类型
//	LOG_TYPE_COUNT_LOG     	        = 7,		//计数日志字符串文件类型
//    FILE_TYPE_OPERATE_LOG   		= 8,		//操作日志文件类型
//    FILE_TYPE_PARAM_SET_LOG 		= 9,		//参数设置日志文件类型
	
}FileType_e;


// 日志类型
typedef enum {

	LOG_TYPE_COUNT_LOG     	= 6,		//计数日志
	LOG_TYPE_RUN     	    = 7,		//运行日志
    LOG_TYPE_OPERATE   		= 8,		//操作日志
    LOG_TYPE_PARAM_SET 		= 9,		//参数设置日志	

}LogType_e;



/***
*挂载文件系统失败是否需要锁定程序枚举
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
FeedBack_e Del_Account_DataFile(AccountDataType_e eAccountDataType); //删除内部或外部账号生成的数据文件（head/data/log）
void DemoFlashFS(uint8_t ucCmd);


//
void FS_LowLevelInit(void);
FeedBack_e FS_Mount(FSMountLock_e FSMountLockState);
FeedBack_e FS_Unmount(void);


FeedBack_e FS_Format(void);
void Print_Dir(char *pcaDir);   //显示指定路径下所有文件
FeedBack_e Del_File(char *pcFile);       //删除单个文件
FeedBack_e Del_Dir_Files(char *pcaDir);  //删除 指定目录下的所有文件(不包括子文件夹)
FeedBack_e FatFs_DelFileSubDir(char* Path); //删除指定路径下所有文件和子目录
FeedBack_e Nand_Volume_Info(uint32_t *pulTotalSect, uint32_t *pulFreeSect);
FeedBack_e FatFs_WriteNewFile(uint8_t *pFileName, uint8_t *pWriteBuffer, uint32_t ulWriteLen);  //写文件
FeedBack_e FatFs_ReadAllFile(uint8_t *pFileName, uint8_t *pReadBuffer, uint32_t *pulReadLen);   //读取整个文件
FeedBack_e FatFs_IsExistFile(char* Path);   //检测文件是否存在
FeedBack_e FatFs_Write_File(uint8_t *pFileName, uint32_t ulWritePos, uint8_t *pWriteBuffer, uint32_t ulWriteLen);
FeedBack_e FatFs_Read_File(uint8_t *pFileName, uint32_t ulReadPos, uint8_t *pReadBuffer, uint32_t ulReadLen);

//FeedBack_e FatFs_Write_ErrorLog(uint8_t *pLogBuffer, uint16_t usLogLen);
//FeedBack_e FatFs_Read_ErrorLog(uint8_t *pLogBuffer, uint32_t ulMaxReadLen, uint32_t*pulReadLen);


/*** 由于历史原因，以下接口依旧以 FatFs_***开头，但是和fatfs没关系，也未使用Fat文件系统（可以使用，但未使用）  ***/
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


