#ifndef _FATFS_UPANAPP_H
#define _FATFS_UPANAPP_H


#include "stm32f4xx_hal.h"
#include "ff.h"





/*�궨����*********************************************************/
//�����豸��꣬ע�����ﶨ���˳����Ҫ��diskio.c�ж�����豸�����Ŷ�Ӧ��
#define U_PAN_LABEL								"0:"				//����U���豸�ľ��


/***
*�Ƿ�ǿ�Ƹ�ʽ����־
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
