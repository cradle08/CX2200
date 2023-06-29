#ifndef _CAT_IOTTASK_H
#define _CAT_IOTTASK_H


#include "stm32f4xx_hal.h"






//定义CAT连接服务端不成功后重连延迟时间
#define CAT_RECONNECT_SERVICE_TIME							20000




/***
*定义仪器自检结果各状态值
***/
typedef struct _CAT_MachSelfCheckResult_s{
	uint8_t n12vFlag;							//负12V自检是否异常，为0表示正常，为1表示异常
	uint8_t p12vFlag;							//正12V自检是否异常
	uint8_t outInMotoOut;						//进出仓电机出仓是否正常
	uint8_t campleMotoSf;						//探针电机释放是否正常
	uint8_t xk;									//小孔电压是否正常
	uint8_t builePress;							//建压是否正常
	uint8_t outInMotoIn;						//进出仓进仓是否正常
	uint8_t campleMotoGd;						//探针电机固定是否正常
	uint8_t cat;								//CAT模块连接是否正常
	uint8_t hgb;								//HGB是否正常
	uint8_t ocInOut;							//进出仓光耦是否正常
	uint8_t ocTanzhen;							//探针光耦是否正常
	uint8_t jscCheck;							//计数池检测模块是否正常
	uint8_t mbxCheck;							//密闭性自检是否正常
	uint8_t eepromFlag;							//EEPROM是否正常
	uint8_t spiFlag;							//SPI是否正常
}CAT_MachSelfCheckResult_s;





typedef enum _CatMsgType_e{
	AGING_TEST_CAT_MSG_TYPE = 0,								//CAT模块老化测试的消息类型
	TIME_UP_CAT_MSG_TYPE,										//CAT模块发送消息前后间隔了60分钟的时间时的消息类型
	SAMPLE_ANALYSE_CAT_MSG_TYPE,								//样本分析完毕后唤醒CAT发送消息的消息类型
	CHANGE_SAMPLE_ANALYSE_CAT_MSG_TYPE,							//修改样本分析部分字段后唤醒CAT发送消息的消息类型，每次开机才发起一次检索
	SET_PARA_OVER_CAT_MSG_TYPE,									//参数设置后唤醒CAT发送消息的消息类型
	CHANGE_MACHINE_INFO_CAT_MSG_TYPGE,							//改变仪器信息后唤醒CAT的消息类型
	SEND_RUN_LOG_CAT_MSG_TYPGE,									//发送运行日志
	SELF_CHECK_RESULT_CAT_MSG_TYPE,								//发送仪器自检结果
	ACTIVE_UPDATE_CAT_MSG_TYPE,									//激活主动升级流程，升级到发布版或者测试版
	
	REFRESH_NET_CAT_MSG_TYPE,									//手动刷新网络的消息类型
	
	REQUEST_PATIENT_ID_CAT_MSG_TYPE,							//向服务端请求指定患者编号的患者信息
	SEND_PATIENT_ORDER_NUM_CAT_MSG_TYPE,						//向服务端发送即将进行测试的患者订单编号信息
	NET_RECOVER_CAT_MSG_TYPE,									//网络重新恢复的消息类型
}CatMsgType_e;




/***
*用于传递给CAT模块发布的消息队列数据结构体
***/
typedef struct _CATQueueInfoBuf_s{
	CatMsgType_e msgType;									//消息类型标志
	
	//附带的消息队列内容
	union{
		uint8_t TempU8;
		uint32_t TempU32;
		char Str[60];
		
		//改变仪器信号指令对应的内容结构体
		struct{
			uint8_t changeType;								//要修改的信息类型，对应指令2.2.8
			uint8_t info[50];								//信息内容
		}changeMachineInfo;
		
		CAT_MachSelfCheckResult_s CATMachSelfCheckResult;	//仪器自检结果上传
		
		//强制升级相关参数
		struct{
			uint8_t versionType;							//为0表示升级到发布版，为1表示升级到测试版
			uint8_t updateType;								//为0表示升级主程序，为1表示升级引导程序，为2表示升级UI
		}forceUpdate;
	}para;
}CATQueueInfoBuf_s;




#define FTP_TEMP_DOWNLOAD_BUF_LEN							2621440			//临时BUF最大2.5MB

/***
*定义从FTP中下载的文件数据结构体
***/
typedef struct _FTPDownloadData_s{
	uint32_t flag;											//用于定义SPI flash中保存的数据是否有效，当为0xA55A0FF0时，表示数据有效，可以往下校验数据，其他值时则直接表示数据无效
	uint32_t totalLen;										//文件总大小
	uint16_t crc16;											//文件CRC校验值
	uint8_t data[100];										//用于之后存放提取后的有用数据，这里数组大小只是用于和下面临时buf隔开位置用，后续在提取数据的时候，提取的有用数据会逐渐覆盖临时BUF已被提取数据的内存位置
	
	uint32_t tempTotalLen;									//用来保存从FTP下载的所有字符长度
	uint8_t tempData[FTP_TEMP_DOWNLOAD_BUF_LEN];			//用来临时保存从FTP下载的所有字符，这里最大1740KB
}FTPDownloadData_s;










uint16_t VersionStr2Int(char* StrBuf);
void VersionInt2Str(uint16_t Version,char* VersionStr);


void CATUpdateStatusTask(void *argument);
void CATTask(void *argument);

void CAT_OtherTask(void *argument);


#endif
