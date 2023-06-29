#ifndef _CAT_IOTTASK_H
#define _CAT_IOTTASK_H


#include "stm32f4xx_hal.h"






//����CAT���ӷ���˲��ɹ��������ӳ�ʱ��
#define CAT_RECONNECT_SERVICE_TIME							20000




/***
*���������Լ�����״ֵ̬
***/
typedef struct _CAT_MachSelfCheckResult_s{
	uint8_t n12vFlag;							//��12V�Լ��Ƿ��쳣��Ϊ0��ʾ������Ϊ1��ʾ�쳣
	uint8_t p12vFlag;							//��12V�Լ��Ƿ��쳣
	uint8_t outInMotoOut;						//�����ֵ�������Ƿ�����
	uint8_t campleMotoSf;						//̽�����ͷ��Ƿ�����
	uint8_t xk;									//С�׵�ѹ�Ƿ�����
	uint8_t builePress;							//��ѹ�Ƿ�����
	uint8_t outInMotoIn;						//�����ֽ����Ƿ�����
	uint8_t campleMotoGd;						//̽�����̶��Ƿ�����
	uint8_t cat;								//CATģ�������Ƿ�����
	uint8_t hgb;								//HGB�Ƿ�����
	uint8_t ocInOut;							//�����ֹ����Ƿ�����
	uint8_t ocTanzhen;							//̽������Ƿ�����
	uint8_t jscCheck;							//�����ؼ��ģ���Ƿ�����
	uint8_t mbxCheck;							//�ܱ����Լ��Ƿ�����
	uint8_t eepromFlag;							//EEPROM�Ƿ�����
	uint8_t spiFlag;							//SPI�Ƿ�����
}CAT_MachSelfCheckResult_s;





typedef enum _CatMsgType_e{
	AGING_TEST_CAT_MSG_TYPE = 0,								//CATģ���ϻ����Ե���Ϣ����
	TIME_UP_CAT_MSG_TYPE,										//CATģ�鷢����Ϣǰ������60���ӵ�ʱ��ʱ����Ϣ����
	SAMPLE_ANALYSE_CAT_MSG_TYPE,								//����������Ϻ���CAT������Ϣ����Ϣ����
	CHANGE_SAMPLE_ANALYSE_CAT_MSG_TYPE,							//�޸��������������ֶκ���CAT������Ϣ����Ϣ���ͣ�ÿ�ο����ŷ���һ�μ���
	SET_PARA_OVER_CAT_MSG_TYPE,									//�������ú���CAT������Ϣ����Ϣ����
	CHANGE_MACHINE_INFO_CAT_MSG_TYPGE,							//�ı�������Ϣ����CAT����Ϣ����
	SEND_RUN_LOG_CAT_MSG_TYPGE,									//����������־
	SELF_CHECK_RESULT_CAT_MSG_TYPE,								//���������Լ���
	ACTIVE_UPDATE_CAT_MSG_TYPE,									//���������������̣���������������߲��԰�
	
	REFRESH_NET_CAT_MSG_TYPE,									//�ֶ�ˢ���������Ϣ����
	
	REQUEST_PATIENT_ID_CAT_MSG_TYPE,							//����������ָ�����߱�ŵĻ�����Ϣ
	SEND_PATIENT_ORDER_NUM_CAT_MSG_TYPE,						//�����˷��ͼ������в��ԵĻ��߶��������Ϣ
	NET_RECOVER_CAT_MSG_TYPE,									//�������»ָ�����Ϣ����
}CatMsgType_e;




/***
*���ڴ��ݸ�CATģ�鷢������Ϣ�������ݽṹ��
***/
typedef struct _CATQueueInfoBuf_s{
	CatMsgType_e msgType;									//��Ϣ���ͱ�־
	
	//��������Ϣ��������
	union{
		uint8_t TempU8;
		uint32_t TempU32;
		char Str[60];
		
		//�ı������ź�ָ���Ӧ�����ݽṹ��
		struct{
			uint8_t changeType;								//Ҫ�޸ĵ���Ϣ���ͣ���Ӧָ��2.2.8
			uint8_t info[50];								//��Ϣ����
		}changeMachineInfo;
		
		CAT_MachSelfCheckResult_s CATMachSelfCheckResult;	//�����Լ����ϴ�
		
		//ǿ��������ز���
		struct{
			uint8_t versionType;							//Ϊ0��ʾ�����������棬Ϊ1��ʾ���������԰�
			uint8_t updateType;								//Ϊ0��ʾ����������Ϊ1��ʾ������������Ϊ2��ʾ����UI
		}forceUpdate;
	}para;
}CATQueueInfoBuf_s;




#define FTP_TEMP_DOWNLOAD_BUF_LEN							2621440			//��ʱBUF���2.5MB

/***
*�����FTP�����ص��ļ����ݽṹ��
***/
typedef struct _FTPDownloadData_s{
	uint32_t flag;											//���ڶ���SPI flash�б���������Ƿ���Ч����Ϊ0xA55A0FF0ʱ����ʾ������Ч����������У�����ݣ�����ֵʱ��ֱ�ӱ�ʾ������Ч
	uint32_t totalLen;										//�ļ��ܴ�С
	uint16_t crc16;											//�ļ�CRCУ��ֵ
	uint8_t data[100];										//����֮������ȡ����������ݣ����������Сֻ�����ں�������ʱbuf����λ���ã���������ȡ���ݵ�ʱ����ȡ���������ݻ��𽥸�����ʱBUF�ѱ���ȡ���ݵ��ڴ�λ��
	
	uint32_t tempTotalLen;									//���������FTP���ص������ַ�����
	uint8_t tempData[FTP_TEMP_DOWNLOAD_BUF_LEN];			//������ʱ�����FTP���ص������ַ����������1740KB
}FTPDownloadData_s;










uint16_t VersionStr2Int(char* StrBuf);
void VersionInt2Str(uint16_t Version,char* VersionStr);


void CATUpdateStatusTask(void *argument);
void CATTask(void *argument);

void CAT_OtherTask(void *argument);


#endif
