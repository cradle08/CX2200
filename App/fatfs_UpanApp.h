#ifndef _FATFS_UPANAPP_H
#define _FATFS_UPANAPP_H


#include "stm32f4xx_hal.h"
#include "ff.h"





/*宏定义区*********************************************************/
//定义设备卷标，注意这里定义的顺序需要和diskio.c中定义的设备驱动号对应上
#define U_PAN_LABEL								"0:"				//定义U盘设备的卷标


/***
*是否强制格式化标志
***/
typedef enum _ForceMkfs_e{
	NOT_FORCE_MKFS = 0,
	FORCE_MKFS
}ForceMkfs_e;










FRESULT FatFs_Mount(char* Label,FATFS* FS,ForceMkfs_e MkfsFlag);
FRESULT FatFs_GetDiskInfo(char* Label,uint64_t* FreePlaceByte,uint64_t* TotalPlaceByte);
FRESULT FatFs_CreateMutiDir(char* Path);
FRESULT FatFs_UpanDelFileSubDir(char* Path);

FRESULT Upan_UpdateBoot(void);
FRESULT Upan_UpdateMain(void);
FRESULT Upan_UpdateUI(void);



#endif
