/************************************************************
*CAT模块底层驱动相关
*************************************************************/
#include "CAT_iot.h"
#include "string.h"
#include "bsp_eeprom.h"
#include "CAT_iotTask.h"
#include "time.h"
#include "cmsis_os2.h"
#include "file_operate.h"
#include "Parameter.h"
#include "bsp_resistance.h"
#include "Public_menuDLG.h"
#include "crc16.h"
#include "algdata.h"
#include "Common.h"


/*宏定义区*********************************************************/
#define CAT_CMD_SEND_BUF_SIZE								256						//定义发送给CAT模块的指令数据缓冲区大小
#define CAT_RECE_BUF_NO_STR									0xFFFF					//定义CAT接收缓冲Buf中没有指定的字符串的标志












/***
*定义发送给CAT模块的命令缓冲区结构体
***/
typedef struct _CAT_SendCmdBuf_s{
	char buf[CAT_CMD_SEND_BUF_SIZE];
	uint16_t writeIndex;
}CAT_SendCmdBuf_s;








/*全局变量定义区**************************************************/
CATGlobalStatus_s CATGlobalStatus = {0};								//CAT模块全局状态变量保存区
MQTT_PayloadBuf_s MQTTPayloadBuf = {0};									//MQTT发布或者接收的消息内容buf
volatile CAT_ReceBuf_s CATReceBuf = {0};								//定义从CAT中接收数据的循环队列










/***
*自定义字符串比较函数，用于给CAT模块的AT指令使用
*参数：
	StartIndex：指定从哪个索引开始比较字符串，为0表示readIndex处开始
	Str：要比较的字符串
*返回值：当存在指定字符串时，则返回出现该字符串的首数组索引，当不存在时则返回0xFFFF
***/
static uint16_t _CAT_MyStrCmp(uint16_t StartIndex,char* Str)
{
	extern volatile CAT_ReceBuf_s CATReceBuf;
	uint16_t TempIndex = 0;
	char* TempStr = Str;
	uint16_t ReturnIndex = 0;
	
	//判断参数合法性
	if(Str == NULL || (CATReceBuf.readIndex == CATReceBuf.writeIndex)){
		return CAT_RECE_BUF_NO_STR;
	}
	
	if(StartIndex != 0){
		if(CATReceBuf.readIndex < CATReceBuf.writeIndex){
			if(StartIndex < CATReceBuf.readIndex || StartIndex > CATReceBuf.writeIndex){
				return CAT_RECE_BUF_NO_STR;
			}
		}else{
			if(StartIndex > CATReceBuf.writeIndex && StartIndex < CATReceBuf.readIndex){
				return CAT_RECE_BUF_NO_STR;
			}
		}
	}
	
	if(StartIndex == 0){
		TempIndex = CATReceBuf.readIndex;
	}else{
		TempIndex = StartIndex;
	}
	
	while(TempIndex != CATReceBuf.writeIndex){
		//先找到第一个字符相同的位置处
		if(CATReceBuf.buf[TempIndex] == *TempStr){
			ReturnIndex = TempIndex;
			
			//验证后续字符是否都一一对应
			while(*TempStr != '\0' && CATReceBuf.buf[TempIndex] == *TempStr && TempIndex != CATReceBuf.writeIndex){
				TempIndex = (TempIndex+1)&CAT_RECE_BUF_SIZE;
				TempStr++;
			}
			
			if(*TempStr == '\0'){
				//找到指定字符串
				return ReturnIndex;
			}else{
				//此次没有找到指定字符串，进入下一次循环
				TempStr = Str;
			}
		}else{
			TempIndex = (TempIndex+1)&CAT_RECE_BUF_SIZE;
		}
	}
	
	return CAT_RECE_BUF_NO_STR;
}



/***
*自定义字符串获取长度函数，用于给CAT模块的AT指令使用
*返回值：返回当前接收缓存队列中收到的数据长度值
***/
//static uint16_t _CAT_MyStrLen(void)
//{
//	extern volatile CAT_ReceBuf_s CATReceBuf;
//	uint16_t TempLen = 0;
//	
//	if(CATReceBuf.readIndex <= CATReceBuf.writeIndex){
//		TempLen = CATReceBuf.writeIndex - CATReceBuf.readIndex;
//	}else{
//		TempLen = (CAT_RECE_BUF_SIZE+1)- CATReceBuf.readIndex + CATReceBuf.writeIndex;
//	}
//	
//	return TempLen;
//}



/***
*自定义获取从指定索引开始处的剩余数据长度函数，用于给CAT模块的AT指令使用
*返回值：返回当前接收缓存队列中收到的剩余数据长度值
***/
//static uint16_t _CAT_MyRemainStrLen(uint16_t StartIndex)
//{
//	extern volatile CAT_ReceBuf_s CATReceBuf;
//	uint16_t TempLen = 0;
//	
//	if(CATReceBuf.readIndex <= CATReceBuf.writeIndex){
//		if(StartIndex < CATReceBuf.readIndex || StartIndex > CATReceBuf.writeIndex){
//			return 0;
//		}
//		
//		TempLen = CATReceBuf.writeIndex - StartIndex;
//	}else{
//		if(StartIndex < CATReceBuf.readIndex && StartIndex > CATReceBuf.writeIndex){
//			return 0;
//		}
//		
//		if(StartIndex >= CATReceBuf.readIndex){
//			TempLen = (CAT_RECE_BUF_SIZE+1)- StartIndex + CATReceBuf.writeIndex;
//		}else{
//			TempLen = CATReceBuf.writeIndex - StartIndex;
//		}
//	}
//	
//	return TempLen;
//}



/***
*自定义字符串复制函数，用于给CAT模块的AT指令使用
*参数：
	Index：表示从该索引出开始复制数据到指定位置处
	Buf：指定保存的缓存区
	Len：要复制的长度
*返回值：拷贝成功，则返回HAL_OK，返回HAL_ERROR则表示拷贝失败
***/
static HAL_StatusTypeDef _CAT_MyStrCpy(uint16_t Index,uint8_t* Buf,uint16_t Len)
{
	extern volatile CAT_ReceBuf_s CATReceBuf;
	
	//判断参数是否合法
	if(Index + Len > CAT_RECE_BUF_SIZE){
		if(CATReceBuf.readIndex < CATReceBuf.writeIndex){
			return HAL_ERROR;
		}else{
			if(((Index + Len)&CAT_RECE_BUF_SIZE) > CATReceBuf.writeIndex){
				return HAL_ERROR;
			}
		}
	}else{
		if(CATReceBuf.readIndex < CATReceBuf.writeIndex){
			if(Index + Len > CATReceBuf.writeIndex){
				return HAL_ERROR;
			}
		}
	}
	
	while(Len--){
		*Buf++ = CATReceBuf.buf[Index];
		Index = (Index+1)&CAT_RECE_BUF_SIZE;
	}
	
	return HAL_OK;
}






/***
*查询运营商
***/
HAL_StatusTypeDef _CAT_CheckOperator(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+COPS?\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
_CAT_CheckOperatorTag:
	TryCount++;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//判断收到的响应是否正确
			if(_CAT_MyStrCmp(0,"46011") != CAT_RECE_BUF_NO_STR){				//电信卡
				CATGlobalStatus.operatorNum = 0;
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"46000") != CAT_RECE_BUF_NO_STR){			//移动卡
				CATGlobalStatus.operatorNum = 1;
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"+COPS: 0\r\n") != CAT_RECE_BUF_NO_STR){
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SIM_UNACTIVE_ERR);
				
				CATGlobalStatus.operatorNum = 0xFF;
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_ERROR;
			}
		}else{
			//超时，与CAT通讯失败
			if(TryCount >= 10){
				CATGlobalStatus.flag.conn = 0;
			
				//发送全局事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto _CAT_CheckOperatorTag;
			}
		}
	}
}



/***
*查询IMEI号
***/
static HAL_StatusTypeDef _CAT_CheckIMEI(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint16_t ReStrIndex = 0;
	uint8_t TryCount = 0;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	//根据CAT的型号不同执行不同的指令
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CGSN\r");
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CGSN=1\r");
	}
	
	CATSendCmdBuf.writeIndex += TempLen;
	
_CAT_CheckIMEITag:
	TryCount++;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//根据不同的CAT模块类型执行不同的动作
			if(CATGlobalStatus.catType == CAT_YK){
				ReStrIndex = _CAT_MyStrCmp(0,"\r\n\"");
			
				//判断收到的响应是否正确
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					if(_CAT_MyStrCmp(ReStrIndex,"\"\r\n") != CAT_RECE_BUF_NO_STR){
						if(_CAT_MyStrCpy((ReStrIndex+3)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.imei,15) == HAL_OK){
							CATGlobalStatus.imei[15] = '\0';
							
							//此指令执行完毕，释放计数信号量
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}
				}
			}else{	//中移CAT
				ReStrIndex = _CAT_MyStrCmp(0,"+CGSN: ");
			
				//判断收到的响应是否正确
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
						if(_CAT_MyStrCpy((ReStrIndex+7)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.imei,15) == HAL_OK){
							CATGlobalStatus.imei[15] = '\0';
							
							//此指令执行完毕，释放计数信号量
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}
				}
			}
		}else{
			if(TryCount >= 5){
				//超时，与CAT通讯失败
				CATGlobalStatus.flag.conn = 0;
			
				//发送全局事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				
				return HAL_TIMEOUT;
			}else{
				goto _CAT_CheckIMEITag;
			}
		}
	}
}



/***
*查询ICCID号
***/
static HAL_StatusTypeDef _CAT_CheckICCID(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint16_t ReStrIndex = 0;
	uint8_t TryCount = 0;
	
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+ICCID\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
_CAT_CheckICCIDTag:
	TryCount++;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			ReStrIndex = _CAT_MyStrCmp(0,"+ICCID:");
			
			//判断收到的响应是否正确
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
					if(CATGlobalStatus.catType == CAT_YK){
						if(_CAT_MyStrCpy((ReStrIndex+7)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.iccid,20) == HAL_OK){
							CATGlobalStatus.iccid[20] = '\0';
							
							CATGlobalStatus.flag.sim = 1;
							
							//此指令执行完毕，释放计数信号量
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}else{
						if(_CAT_MyStrCpy((ReStrIndex+8)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.iccid,20) == HAL_OK){
							CATGlobalStatus.iccid[20] = '\0';
							
							CATGlobalStatus.flag.sim = 1;
							
							//此指令执行完毕，释放计数信号量
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}
				}
			}else if(_CAT_MyStrCmp(0,"+CME ERROR") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.sim = 0;
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_NO_SIM_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_ERROR;
			}else if(_CAT_MyStrCmp(0,"ERROR:") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.sim = 0;
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_NO_SIM_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_ERROR;
			}
		}else{
			//超时，与CAT通讯失败
			if(TryCount >= 10){
				CATGlobalStatus.flag.conn = 0;
			
				//发送全局事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto _CAT_CheckICCIDTag;
			}
		}
	}
}



/***
*查询SN号
***/
static HAL_StatusTypeDef _CAT_CheckSN(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint16_t ReStrIndex = 0;
	uint8_t TryCount = 0;

	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	//根据CAT模块型号发送不同指令
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MRD_SN?\r");
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CGSN=0\r");
	}
	
	CATSendCmdBuf.writeIndex += TempLen;

_CAT_CheckSNTag:
	TryCount++;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//根据CAT模块型号发送不同指令
			if(CATGlobalStatus.catType == CAT_YK){
				ReStrIndex = _CAT_MyStrCmp(0,"*MRD_SN:");
			
				//判断收到的响应是否正确
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
						if(_CAT_MyStrCpy((ReStrIndex+8)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.sn,14) == HAL_OK){	
							CATGlobalStatus.sn[14] = '\0';
							
							//此指令执行完毕，释放计数信号量
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}
				}
			}else{		//中移CAT
				ReStrIndex = _CAT_MyStrCmp(0,"\r\n");
			
				//判断收到的响应是否正确
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
						if(_CAT_MyStrCpy((ReStrIndex+8)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.sn,14) == HAL_OK){
							CATGlobalStatus.sn[14] = '\0';
							
							//此指令执行完毕，释放计数信号量
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}
				}
			}
		}else{
			//超时，与CAT通讯失败
			if(TryCount >= 5){
				//超时，与CAT通讯失败
				CATGlobalStatus.flag.conn = 0;
			
				//发送全局事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto _CAT_CheckSNTag;
			}
		}
	}
}









/*外部函数区****************************************************/
/***
*查询CAT模块是否注册上网络
***/
HAL_StatusTypeDef CAT_RegistNet(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	uint16_t ReStrIndex = 0;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CREG?\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_RegistNetTag:
	TryCount++;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//判断收到的响应是否正确
			ReStrIndex = _CAT_MyStrCmp(0,"+CREG: ");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
					if(CATReceBuf.buf[(ReStrIndex+9)&CAT_RECE_BUF_SIZE] == '1' || CATReceBuf.buf[(ReStrIndex+9)&CAT_RECE_BUF_SIZE] == '5'){
						if(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
							CATGlobalStatus.registerNet = CAT_REGISTNET_SUCCESS;
							
							osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS);
						}
						
						//此指令执行完毕，释放计数信号量
						osSemaphoreRelease(CATSemphrCountTxHandle);
						return HAL_OK;
					}else if(CATReceBuf.buf[(ReStrIndex+9)&CAT_RECE_BUF_SIZE] == '2'){
						if(TryCount >= 10){
							CATGlobalStatus.registerNet = CAT_REGISTNET_CHECKING;
							
							osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_ERR);
							
							//此指令执行完毕，释放计数信号量
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}else{
							goto CAT_RegistNetTag;
						}
					}else if(CATReceBuf.buf[(ReStrIndex+9)&CAT_RECE_BUF_SIZE] == '3'){
						if(TryCount >= 10){
							CATGlobalStatus.registerNet = CAT_REGISTNET_SIM_OVERDUE;
							
							osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_ERR);
							
							//此指令执行完毕，释放计数信号量
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}else{
							goto CAT_RegistNetTag;
						}
					}else{
						if(TryCount >= 10){
							CATGlobalStatus.registerNet = CAT_REGISTNET_UNKNOW;
							
							osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_ERR);
							
							//此指令执行完毕，释放计数信号量
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}else{
							goto CAT_RegistNetTag;
						}
					}
				}
			}
		}else{
			//超时，与CAT通讯失败
			if(TryCount >= 50){
				CATGlobalStatus.flag.conn = 0;
			
				//发送全局事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_RegistNetTag;
			}
		}
	}
}




/***
*查询CAT模块信号强度
***/
HAL_StatusTypeDef CAT_GetSignalStrength(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint16_t ReStrIndex = 0;
	uint8_t TryCount = 0;

	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CSQ\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_GetSignalStrengthTag:
	TryCount++;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			ReStrIndex = _CAT_MyStrCmp(0,"+CSQ: ");
			
			//判断收到的响应是否正确
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
					CATGlobalStatus.signalStrength = CATReceBuf.buf[(ReStrIndex+6)&CAT_RECE_BUF_SIZE]-'0';
				
					if(CATReceBuf.buf[(ReStrIndex+7)&CAT_RECE_BUF_SIZE] != ','){
						CATGlobalStatus.signalStrength = CATGlobalStatus.signalStrength*10+(CATReceBuf.buf[(ReStrIndex+7)&CAT_RECE_BUF_SIZE]-'0');
					}
					
					//此指令执行完毕，释放计数信号量
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}
			}
		}else{
			//超时，与CAT通讯失败
			if(TryCount >= 3){
				CATGlobalStatus.flag.conn = 0;
			
				//发送全局事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_GetSignalStrengthTag;
			}
		}
	}
}




/***
*查询网络时间，并更新RTC时钟
返回值：返回HAL_OK表示读取的网络时间可信，其他表示网络时间异常
***/
HAL_StatusTypeDef CAT_GetNetTime(RTC_HYM8563Info_s* CurrentNetTime)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	uint16_t ReStrIndex = 0;
	uint8_t TimeZoneConstant = 8;					//时区差值
	uint8_t Hour = 0;
	uint8_t Day = 0;
	uint8_t Month = 0;
	uint16_t Year = 0;
	uint8_t DayAdd = 0;
	uint8_t MonthAdd = 0;
	uint16_t Temp = 0;
	


	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CCLK?\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_GetNetTimeTag:
	TryCount++;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,1000ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			ReStrIndex = _CAT_MyStrCmp(0,"+CCLK: \"");
			
			//判断收到的响应是否正确
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
					//拼合时间，注意，从网上读取的时间需要+8小时才是中国时间
					CurrentNetTime->year.bcd_h = CATReceBuf.buf[(ReStrIndex+8)&CAT_RECE_BUF_SIZE] - '0';
					CurrentNetTime->year.bcd_l = CATReceBuf.buf[(ReStrIndex+9)&CAT_RECE_BUF_SIZE] - '0';
					CurrentNetTime->month.c = 0;							//20xx年
					
					CurrentNetTime->month.bcd_h = CATReceBuf.buf[(ReStrIndex+11)&CAT_RECE_BUF_SIZE] - '0';
					CurrentNetTime->month.bcd_l = CATReceBuf.buf[(ReStrIndex+12)&CAT_RECE_BUF_SIZE] - '0';
					
					CurrentNetTime->day.bcd_h = CATReceBuf.buf[(ReStrIndex+14)&CAT_RECE_BUF_SIZE] - '0';
					CurrentNetTime->day.bcd_l = CATReceBuf.buf[(ReStrIndex+15)&CAT_RECE_BUF_SIZE] - '0';
					
					CurrentNetTime->hour.bcd_h = CATReceBuf.buf[(ReStrIndex+17)&CAT_RECE_BUF_SIZE] - '0';
					CurrentNetTime->hour.bcd_l = CATReceBuf.buf[(ReStrIndex+18)&CAT_RECE_BUF_SIZE] - '0';
					
					CurrentNetTime->min.bcd_h = CATReceBuf.buf[(ReStrIndex+20)&CAT_RECE_BUF_SIZE] - '0';
					CurrentNetTime->min.bcd_l = CATReceBuf.buf[(ReStrIndex+21)&CAT_RECE_BUF_SIZE] - '0';
					
					CurrentNetTime->sec.bcd_h = CATReceBuf.buf[(ReStrIndex+23)&CAT_RECE_BUF_SIZE] - '0';
					CurrentNetTime->sec.bcd_l = CATReceBuf.buf[(ReStrIndex+24)&CAT_RECE_BUF_SIZE] - '0';
					CurrentNetTime->sec.vl = 0;
					
					//根据不同的CAT型号执行不同的动作，移柯的CAT才需要加8小时，中移的不需要
					if(CATGlobalStatus.catType == CAT_YK){
						Hour = CurrentNetTime->hour.bcd_h*10+CurrentNetTime->hour.bcd_l;
						Day = CurrentNetTime->day.bcd_h*10+CurrentNetTime->day.bcd_l;
						Month = CurrentNetTime->month.bcd_h*10+CurrentNetTime->month.bcd_l;
						Year = 2000+CurrentNetTime->year.bcd_h*10+CurrentNetTime->year.bcd_l;
						
						//计算+8小时后各参数的变化
						Temp = (Hour+TimeZoneConstant)%24;
						DayAdd = (Hour+TimeZoneConstant)/24;
						Hour = Temp;
						
						//判断今年是否是闰年：能被4整除，但不能被100整除，可以被400整除的年都是闰年
						if(((Year%4)!=0) || (((Year%4)==0) && ((Year%100)==0) && ((Year%400)!=0))){
							//非闰年
							switch(Month){
								case 1:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 2:
									Temp = (Day+DayAdd)%29;
									MonthAdd = (Day+DayAdd)/29;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 3:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 4:
									Temp = (Day+DayAdd)%31;
									MonthAdd = (Day+DayAdd)/31;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 5:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 6:
									Temp = (Day+DayAdd)%31;
									MonthAdd = (Day+DayAdd)/31;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 7:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 8:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 9:
									Temp = (Day+DayAdd)%31;
									MonthAdd = (Day+DayAdd)/31;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 10:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 11:
									Temp = (Day+DayAdd)%31;
									MonthAdd = (Day+DayAdd)/31;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 12:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
									
									Month = MonthAdd==0?Month:1;
									Year = MonthAdd==0?Year:(Year+1);
								break;
								default :break;
							}
						}else{
							//闰年
							switch(Month){
								case 1:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 2:
									Temp = (Day+DayAdd)%30;
									MonthAdd = (Day+DayAdd)/30;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 3:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 4:
									Temp = (Day+DayAdd)%31;
									MonthAdd = (Day+DayAdd)/31;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 5:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 6:
									Temp = (Day+DayAdd)%31;
									MonthAdd = (Day+DayAdd)/31;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 7:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 8:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 9:
									Temp = (Day+DayAdd)%31;
									MonthAdd = (Day+DayAdd)/31;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 10:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 11:
									Temp = (Day+DayAdd)%31;
									MonthAdd = (Day+DayAdd)/31;
									Day = Temp==0?1:Temp;
								
									Month += MonthAdd;
								break;
								case 12:
									Temp = (Day+DayAdd)%32;
									MonthAdd = (Day+DayAdd)/32;
									Day = Temp==0?1:Temp;
									
									Month = MonthAdd==0?Month:1;
									Year = MonthAdd==0?Year:(Year+1);
								break;
								default :break;
							}
						}
						
						//还原成BCD码到结构体中
						CurrentNetTime->year.bcd_h = (Year-2000)/10;
						CurrentNetTime->year.bcd_l = (Year-2000)%10;
						
						CurrentNetTime->month.bcd_h = Month/10;
						CurrentNetTime->month.bcd_l = Month%10;
						
						CurrentNetTime->day.bcd_h = Day/10;
						CurrentNetTime->day.bcd_l = Day%10;
						
						CurrentNetTime->hour.bcd_h = Hour/10;
						CurrentNetTime->hour.bcd_l = Hour%10;
					}
					
					//此指令执行完毕，释放计数信号量
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}
			}
		}else{
			//超时，与CAT通讯失败
			if(TryCount >= 5){
				CATGlobalStatus.flag.conn = 0;
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_GetNetTimeTag;
			}
		}
	}
}





/***
*查询并设置中移CAT定位需要使用的高德KEY
***/
static HAL_StatusTypeDef _CAT_ZY_SetGDKey(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
//	uint16_t ReStrIndex = 0;
	char TempStr[100] = {0};

	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+LBSKEY?\r");
	
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			sprintf(TempStr,"+LBSKEY:%s",GAODE_KEY_POSITION);
			
			//判断收到的响应是否正确
			if(_CAT_MyStrCmp(0,TempStr) != CAT_RECE_BUF_NO_STR){	//已存在KEY，则直接退出即可
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else{			//不存在KEY，则需要配置KEY
				memset(TempStr,0,100);
				sprintf(TempStr,"AT+LBSKEY=\"%s\"\r",GAODE_KEY_POSITION);
				
				CATSendCmdBuf.writeIndex = 0;
				TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"%s",TempStr);
				CATSendCmdBuf.writeIndex += TempLen;
				
				osKernelLock();
				HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,500);
				osKernelUnlock();
				
				osDelay(50);
				
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			//超时，与CAT通讯失败
			CATGlobalStatus.flag.conn = 0;
		
			//发送全局事件
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			
			//此指令执行完毕，释放计数信号量
			osSemaphoreRelease(CATSemphrCountTxHandle);
			return HAL_TIMEOUT;
		}
	}
}





/***
*查询经纬度位置信息
***/
HAL_StatusTypeDef CAT_GetPosition(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint16_t ReStrIndex = 0;
	uint8_t TryCount = 0;

	
	//先判断是否是中移CAT，是，则需要先配置高德KEY，然后才能正常定位，KEY配置一次就会自动保存
	if(CATGlobalStatus.catType == CAT_ZY){
		_CAT_ZY_SetGDKey();
	}
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
CAT_GetPositionTag:
	TryCount++;
	
	CATSendCmdBuf.writeIndex = 0;
	
	//开启定位
	//根据不同的CAT型号执行不同的指令
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+GTPOS=1\r");
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+QCELLLOC\r");
	}
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//不同CAT执行不同的指令
			if(CATGlobalStatus.catType == CAT_YK){
				if(_CAT_MyStrCmp(0,"CONNECT OK") != CAT_RECE_BUF_NO_STR || _CAT_MyStrCmp(0,"ALREARY CONNECTED") != CAT_RECE_BUF_NO_STR){				
					//===获取位置数据===
					CATSendCmdBuf.writeIndex = 0;
					
					//获取定位信息
					TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+GTPOS=2\r");
					CATSendCmdBuf.writeIndex += TempLen;
					
					osKernelLock();
					HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
					osKernelUnlock();
					
					while(1){
						//等待空闲中断处理中释放的二值同步信号量,20000ms超时
						xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
						
						//判断是否收到响应
						if(xReturn == osOK){
							ReStrIndex = _CAT_MyStrCmp(0,"GTPOS:");
				
							//判断收到的响应是否正确
							if(ReStrIndex != CAT_RECE_BUF_NO_STR){
								if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
									//保存位置信息
									if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") > ReStrIndex){
										_CAT_MyStrCpy((ReStrIndex+6)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.position,(_CAT_MyStrCmp(ReStrIndex,"\r\nOK")-ReStrIndex-8));
									}else{
										_CAT_MyStrCpy((ReStrIndex+6)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.position,(_CAT_MyStrCmp(ReStrIndex,"\r\nOK")+CAT_RECE_BUF_SIZE+1-ReStrIndex-8));
									}
									
									//===关闭定位===
									CATSendCmdBuf.writeIndex = 0;
									
									//关闭定位
									TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+GTPOS=0\r");
									CATSendCmdBuf.writeIndex += TempLen;
									
									osKernelLock();
									HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
									osKernelUnlock();
									
									while(1){
										//等待空闲中断处理中释放的二值同步信号量,500ms超时
										xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
										
										//判断是否收到响应
										if(xReturn == osOK){
											if(_CAT_MyStrCmp(0,"OK") != CAT_RECE_BUF_NO_STR){
												osSemaphoreRelease(CATSemphrCountTxHandle);
												return HAL_OK;
											}
										}else{
											//超时，与CAT通讯失败
											if(TryCount >= 3){
												osSemaphoreRelease(CATSemphrCountTxHandle);
												osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
												return HAL_TIMEOUT;
											}else{
												goto CAT_GetPositionTag;
											}
										}
									}
								}
							}
						}else{
							//超时，与CAT通讯失败
							if(TryCount >= 3){
								osSemaphoreRelease(CATSemphrCountTxHandle);
								osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
								return HAL_TIMEOUT;
							}else{
								goto CAT_GetPositionTag;
							}
						}
					}
				}
			}else{
				//中移CAT
				ReStrIndex = _CAT_MyStrCmp(0,"+QCELLLOC: ");
				
				//判断收到的响应是否正确
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
						//保存位置信息
						if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") > ReStrIndex){
							_CAT_MyStrCpy((ReStrIndex+11)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.position,(_CAT_MyStrCmp(ReStrIndex,"\r\nOK")-ReStrIndex-13));
						}else{
							_CAT_MyStrCpy((ReStrIndex+11)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.position,(_CAT_MyStrCmp(ReStrIndex,"\r\nOK")+CAT_RECE_BUF_SIZE+1-ReStrIndex-13));
						}
						
						//此指令执行完毕，释放计数信号量
						osSemaphoreRelease(CATSemphrCountTxHandle);
						return HAL_OK;
					}
				}
			}
		}else{
			//超时，与CAT通讯失败
			if(TryCount >= 3){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				return HAL_TIMEOUT;
			}else{
				goto CAT_GetPositionTag;
			}
		}
	}
}



/***
*打开CAT模块网络
***/
HAL_StatusTypeDef CAT_OpenNet(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	//根据不同CAT型号执行不同指令
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+NETOPEN\r");
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CGATT=1\rAT+CGACT=1\r");
	}
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_OpenNetTag:
	TryCount++;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//根据不同的CAT型号执行不同的指令
			if(CATGlobalStatus.catType == CAT_YK){
				//判断收到的响应是否正确
				if(_CAT_MyStrCmp(0,"+NETOPEN:SUCCESS") != CAT_RECE_BUF_NO_STR){
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}else if(_CAT_MyStrCmp(0,"ERROR: 902") != CAT_RECE_BUF_NO_STR){
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}else{
					if(TryCount >= 5){
						osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_ERR);
						
						//此指令执行完毕，释放计数信号量
						osSemaphoreRelease(CATSemphrCountTxHandle);
						return HAL_ERROR;
					}else{
						goto CAT_OpenNetTag;
					}
				}
			}else{
				if(_CAT_MyStrCmp(0,"\r\nOK\r\n") != CAT_RECE_BUF_NO_STR){
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}
			}
		}else{
			//超时，与CAT通讯失败
			if(TryCount >= 5){
				CATGlobalStatus.flag.conn = 0;
			
				//发送全局事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_OpenNetTag;
			}
		}
	}
}




/***
*查询CAT模块与MCU通讯是否建立完成
***/
HAL_StatusTypeDef CAT_IsOK(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;

	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_IsOKTag:
	TryCount++;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,200);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//判断收到的响应是否正确
			if(_CAT_MyStrCmp(0,"\r\nOK\r\n") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.conn = 1;
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			if(TryCount >= 20){
				//超时，与CAT通讯失败
				CATGlobalStatus.flag.conn = 0;
				
				//发送全局事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_IsOKTag;
			}
		}
	}
}




/***
*查询CAT模块型号，以便兼容移柯和中移的物联网卡
***/
HAL_StatusTypeDef CAT_ReadType(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryConut = 0;

	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CGMM\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_ReadTypeTag:
	TryConut++;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,500);
		
		//判断是否收到响应
		if(xReturn == osOK){
			if(_CAT_MyStrCmp(0,"+CGMM: LYNQ_L501") != CAT_RECE_BUF_NO_STR){
				//判断是否是移柯的模块
				CATGlobalStatus.catType = CAT_YK;
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"ML302") != CAT_RECE_BUF_NO_STR){
				//判断是否是中移的模块
				CATGlobalStatus.catType = CAT_ZY;
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			if(TryConut >= 10){
				//超时，与CAT通讯失败
				CATGlobalStatus.flag.conn = 0;
				
				//发送全局事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_ReadTypeTag;
			}
		}
	}
}





/***
*初始化配置CAT模块
***/
HAL_StatusTypeDef CAT_InitConfig(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern osThreadId_t CATUpdateStatusTaskHandle;
	uint16_t TempLen = 0;
	RTC_HYM8563Info_s CurrentNetTime;
	osStatus_t xReturn = osOK;
	uint16_t TryCount = 0;
	static uint8_t CATRestartCount = 0;					//用于记录CAT重启次数
	
	
	//当模块是移柯的，则等待模块启动完毕，即发送完Call Ready字符串后，防止模块尚未启动完毕就开始发送指令时出现异常现象，中移的模块则直接跳过此步骤
	while(CATGlobalStatus.catType == CAT_YK){
		//15s超时后则退出循环
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,15000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			if(_CAT_MyStrCmp(0,"Call Ready") != CAT_RECE_BUF_NO_STR){
				break;
			}
		}else{
			break;
		}
	}
	
	//配置功能
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	//去除指令回显功能
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"ATE0\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,500);
	osKernelUnlock();
	
	//延迟1s
	osDelay(1000);
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	
	//等待设置完毕
	if(CAT_IsOK() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//往后台发送消息，重启CAT初始化任务
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//模块串口通讯异常
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//查询IMEI号
	if(_CAT_CheckIMEI() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//往后台发送消息，重启CAT初始化任务
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//模块串口通讯异常
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//查询SN号
	if(_CAT_CheckSN() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//往后台发送消息，重启CAT初始化任务
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//模块串口通讯异常
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//查询ICCID号
	if(_CAT_CheckICCID() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//往后台发送消息，重启CAT初始化任务
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//模块启动异常
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//查询信号强度，直到信号强度超过13时才继续执行后面操作，防止误判
	TryCount = 0;
	while(1){
		CAT_GetSignalStrength();
		
		if(CATGlobalStatus.signalStrength > 10){
			break;
		}else{
			osDelay(1000);
			
			TryCount++;
			
			//60秒后，信号还未正常，往后台发送消息，重启CAT初始化任务
			if(TryCount > 60){
				CATRestartCount++;
				
				if(CATRestartCount <= 3){
					Msg_Head_t BackendMsg = {0};
				
					//往后台发送消息，重启CAT初始化任务
					BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
					UI_Put_Msg((uint8_t*)&BackendMsg);
					
					osDelay(10000);
				}else{
					//重启CAT模块3次后，若网络还未好，则挂起任务
					osThreadSuspend(CATUpdateStatusTaskHandle);
				}
			}
		}
	}
	
	//等待注册上网络
	TryCount = 0;
	while(1){
		CAT_RegistNet();
		
		if(CATGlobalStatus.registerNet == CAT_REGISTNET_SUCCESS){
			CATRestartCount = 0;
			break;
		}else if(CATGlobalStatus.registerNet == CAT_REGISTNET_SIM_OVERDUE){
			CATGlobalStatus.registerNet = CAT_REGISTNET_UNKNOW;
			
			osDelay(1000);
			TryCount++;
			
			if(TryCount > 10){
				CATRestartCount++;
				
				if(CATRestartCount <= 3){
					Msg_Head_t BackendMsg = {0};
				
					//往后台发送消息，重启CAT初始化任务
					BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
					UI_Put_Msg((uint8_t*)&BackendMsg);
					
					osDelay(10000);
				}else{
					CATGlobalStatus.registerNet = CAT_REGISTNET_SIM_OVERDUE;
					osThreadSuspend(CATUpdateStatusTaskHandle);
				}
			}
		}else{
			CATGlobalStatus.registerNet = CAT_REGISTNET_UNKNOW;
			
			osDelay(1000);
			TryCount++;
			
			//30秒后，信号还未正常，则挂起任务
			if(TryCount > 20){
				CATRestartCount++;
				
				if(CATRestartCount <= 3){
					Msg_Head_t BackendMsg = {0};
				
					//往后台发送消息，重启CAT初始化任务
					BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
					UI_Put_Msg((uint8_t*)&BackendMsg);
					
					osDelay(10000);
				}else{
					osThreadSuspend(CATUpdateStatusTaskHandle);
				}
			}
		}
	}
	
	//查询运营商
	if(_CAT_CheckOperator() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//往后台发送消息，重启CAT初始化任务
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//模块启动异常
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//打开网络
	if(CAT_OpenNet() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//往后台发送消息，重启CAT初始化任务
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//模块启动异常
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//获取网络时间
	if(CAT_GetNetTime(&CurrentNetTime) != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//往后台发送消息，重启CAT初始化任务
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//模块启动异常
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//同步RTC，HYM8563
	RTC_HYM8563SetTime(&CurrentNetTime);
	
	//获取定位信息，这里可能受基站影响，导致定位信息获取不到，只能下次开机再试
	CAT_GetPosition();
	
	return HAL_OK;
}





//===MQTT相关底层指令=为了防止流量泄漏，只要函数返回ERROR，则挂起任务============================================================
/***
*CAT，配置MQTT客户端相关信息，并且启用遗嘱消息机制，retain=0，qos=1，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
*参数：
	ClientID：客户端ID字符串，最大长度为256字节
	UserName：登录用户名字符串，可以为NULL
	Password：登录密码字符串，可以为NULL
	WillTopic：遗嘱消息主题字符串
	WillMessage：遗嘱消息字符串
***/
HAL_StatusTypeDef CAT_M_ConfigClient(CAT_SERVICE_e CatService,char* ClientID,char* UserName,char* Password,char* WillTopic,char* WillMessage)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	
	//先判断参数是否合法
	if(ClientID == NULL){
		return HAL_ERROR;
	}
	if(strlen(ClientID) > 256){
		return HAL_ERROR;
	}
	if(strlen(UserName) > 256){
		return HAL_ERROR;
	}
	if(strlen(Password) > 256){
		return HAL_ERROR;
	}
	if(strlen(WillTopic) > 256){
		return HAL_ERROR;
	}
	if(strlen(WillMessage) > 1024){
		return HAL_ERROR;
	}
	
CAT_ConfigClientTag:
	//判断网络是否连接，否，则一直等待连接成功，需要先清除事件标志
	osEventFlagsClear(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS);
	while(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS,osFlagsWaitAny,osWaitForever);
	}

	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	if(UserName == NULL){
		if(WillTopic == NULL){
			//根据不同CAT模块类型执行不同指令
			if(CATGlobalStatus.catType == CAT_YK){
				TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MCONFIG=\"%s\"\r\n",ClientID);
			}else{
				if(CatService == SERVICE_CC){
					TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTCON=0,\"%s\",%u,\"%s\",,,,,1\r\n",MQTT_SERVICE_IP,MQTT_SERVICE_PORT,ClientID);
				}else{
					TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTCON=0,\"%s\",%u,\"%s\",,,,,1\r\n",MachInfo.companyInfo.mqtt.ip,MQTT_SERVICE_PORT,ClientID);
				}
			}
		}else{
			if(CATGlobalStatus.catType == CAT_YK){
				TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MCONFIG=\"%s\",,,1,1,0,\"%s\",\"%s\"\r\n",ClientID,WillTopic,WillMessage);
			}else{
				if(CatService == SERVICE_CC){
					TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTCON=0,\"%s\",%u,\"%s\",,,,,1,1,1,0,\"%s\",\"%s\"\r\n",MQTT_SERVICE_IP,MQTT_SERVICE_PORT,ClientID,WillTopic,WillMessage);
				}else{
					TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTCON=0,\"%s\",%u,\"%s\",,,,,1,1,1,0,\"%s\",\"%s\"\r\n",MachInfo.companyInfo.mqtt.ip,MQTT_SERVICE_PORT,ClientID,WillTopic,WillMessage);
				}
			}
		}
	}else{
		if(WillTopic == NULL){
			if(CATGlobalStatus.catType == CAT_YK){
				TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MCONFIG=\"%s\",\"%s\",\"%s\"\r\n",ClientID,UserName,Password);
			}else{
				if(CatService == SERVICE_CC){
					TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTCON=0,\"%s\",%u,\"%s\",\"%s\",\"%s\",,,1\r\n",MQTT_SERVICE_IP,MQTT_SERVICE_PORT,ClientID,UserName,Password);
				}else{
					TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTCON=0,\"%s\",%u,\"%s\",\"%s\",\"%s\",,,1\r\n",MachInfo.companyInfo.mqtt.ip,MQTT_SERVICE_PORT,ClientID,UserName,Password);
				}
			}
		}else{
			if(CATGlobalStatus.catType == CAT_YK){
				TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MCONFIG=\"%s\",\"%s\",\"%s\",1,1,0,\"%s\",\"%s\"\r\n",ClientID,UserName,Password,WillTopic,WillMessage);
			}else{
				if(CatService == SERVICE_CC){
					TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTCON=0,\"%s\",%u,\"%s\",\"%s\",\"%s\",,,1,1,1,0,\"%s\",\"%s\"\r\n",MQTT_SERVICE_IP,MQTT_SERVICE_PORT,ClientID,UserName,Password,WillTopic,WillMessage);
				}else{
					TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTCON=0,\"%s\",%u,\"%s\",\"%s\",\"%s\",,,1,1,1,0,\"%s\",\"%s\"\r\n",MachInfo.companyInfo.mqtt.ip,MQTT_SERVICE_PORT,ClientID,UserName,Password,WillTopic,WillMessage);
				}
			}
		}
	}
	
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,500);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//根据不同的CAT类型执行不同的指令
			if(CATGlobalStatus.catType == CAT_YK){
				//判断收到的响应是否正确
				if(_CAT_MyStrCmp(0,"\r\nOK\r\n") != CAT_RECE_BUF_NO_STR){
					
					//此指令执行完毕，释放计数信号量
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}
			}else{
				if(_CAT_MyStrCmp(0,"+MMQTTCON: 0,0") != CAT_RECE_BUF_NO_STR){
					CATGlobalStatus.flag.tcpConn = 1;
					CATGlobalStatus.flag.mqttConn = 1;
					
					//此指令执行完毕，释放计数信号量
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}else if(_CAT_MyStrCmp(0,"+MMQTTCON: 0,1") != CAT_RECE_BUF_NO_STR){
					//此指令执行完毕，释放计数信号量
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_ERROR;
				}
			}
		}else{
			//超时，与CAT通讯失败
			//此指令执行完毕，释放计数信号量
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount == 3){
				return HAL_TIMEOUT;
			}else{
				goto CAT_ConfigClientTag;
			}
		}
	}
}




/***
*CAT，连接服务端和端口，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
*参数：
	HostName：服务端IP地址或者域名
	Port：MQTT端口
***/
HAL_StatusTypeDef CAT_M_CreateTcpConn(char* HostName,uint16_t Port)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	uint8_t RetryOnceFlag = 0;
	
	//先判断参数是否合法
	if(HostName == NULL){
		return HAL_ERROR;
	}else if(strlen(HostName) > 256){
		return HAL_ERROR;
	}
	
CAT_CreateTcpConnTag:
	//判断网络是否连接，否，则一直等待连接成功，需要先清除事件标志
	osEventFlagsClear(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS);
	while(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS,osFlagsWaitAny,osWaitForever);
	}

	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MIPSTART=\"%s\",%u\r\n",HostName,Port);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//判断收到的响应是否正确
			if(_CAT_MyStrCmp(0,"+MIPSTART: SUCCESS") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.tcpConn = 1;
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"+MIPSTART: FAILURE") != CAT_RECE_BUF_NO_STR){
				//这里可能由于重复连接而产生的错误或者其他连接错误，所以这里需要发送一个断开连接的操作，然后重新连接
				if(RetryOnceFlag == 0){
					RetryOnceFlag = 1;
					CATSendCmdBuf.writeIndex = 0;
					
					TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MIPCLOSE\r\n");
					CATSendCmdBuf.writeIndex += TempLen;
					
					osKernelLock();
					HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
					osKernelUnlock();
					
					osSemaphoreRelease(CATSemphrCountTxHandle);
					goto CAT_CreateTcpConnTag;
				}
				
				osSemaphoreRelease(CATSemphrCountTxHandle);
				CATGlobalStatus.flag.tcpConn = 0;
				
				return HAL_ERROR;
			}
		}else{
			//超时，与CAT通讯失败		
			//此指令执行完毕，释放计数信号量
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount == 3){
				//发送全局事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				return HAL_TIMEOUT;
			}else{
				goto CAT_CreateTcpConnTag;
			}
		}
	}
}




/***
*CAT，连接服务端的MQTT，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
***/
HAL_StatusTypeDef CAT_M_CreateMQTTConn(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	
	if(CATGlobalStatus.flag.tcpConn == 0){
		return HAL_ERROR;
	}
	
CAT_CreateMQTTConnTag:
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MCONNECT=1,50\r\n");
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,200);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//判断收到的响应是否正确
			if(_CAT_MyStrCmp(0,"+MCONNECT: SUCCESS") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.mqttConn = 1;
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"+MCONNECT: FAILURE") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.mqttConn = 1;
				
				//此指令执行完毕，释放计数信号量
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			//此指令执行完毕，释放计数信号量
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//超时，与CAT通讯失败
			if(TryCount == 3){
				//发送全局事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				return HAL_TIMEOUT;
			}else{
				goto CAT_CreateMQTTConnTag;
			}
		}
	}
}




/***
*CAT，关闭MQTT连接，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
***/
HAL_StatusTypeDef CAT_M_CloseMQTTConn(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	
	
CAT_CloseMQTTConnTag:
	//判断网络是否连接，否，则一直等待连接成功，需要先清除事件标志
	osEventFlagsClear(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS);
	while(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS,osFlagsWaitAny,osWaitForever);
	}
	
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	//根据不同CAT类型执行不同指令
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MDISCONNECT\r\nAT+MIPCLOSE\r\n");
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTDISCON=0\r\n");
	}
	
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,200);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//根据不同CAT类型执行不同指令
			if(CATGlobalStatus.catType == CAT_YK){
				if(_CAT_MyStrCmp(0,"+MIPCLOSE") != CAT_RECE_BUF_NO_STR){
					CATGlobalStatus.flag.tcpConn = 0;
					CATGlobalStatus.flag.mqttConn = 0;
					
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}
			}else{
				if(_CAT_MyStrCmp(0,"+MMQTTDISCON: 0,0") != CAT_RECE_BUF_NO_STR){
					CATGlobalStatus.flag.tcpConn = 0;
					CATGlobalStatus.flag.mqttConn = 0;
					
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}else if(_CAT_MyStrCmp(0,"+MMQTTDISCON: 0,1") != CAT_RECE_BUF_NO_STR){
					CATGlobalStatus.flag.tcpConn = 0;
					CATGlobalStatus.flag.mqttConn = 0;
					
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//超时，与CAT通讯失败
			if(TryCount == 3){
				return HAL_TIMEOUT;
			}else{
				goto CAT_CloseMQTTConnTag;
			}
		}
	}
}




/***
*CAT，订阅主题，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
*参数：
	Topic：订阅的主题字符串
***/
HAL_StatusTypeDef CAT_M_SubTopic(char* Topic)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	
	//先判断参数是否合法
	if(Topic == NULL){
		return HAL_ERROR;
	}
	
	//判断是否连接MQTT服务端成功
	if(CATGlobalStatus.flag.mqttConn == 0 || CATGlobalStatus.flag.tcpConn == 0){
		return HAL_ERROR;
	}
	
CAT_SubTopicTag:
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	//根据不同CAT类型执行不同的指令
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MSUB=\"%s\",2\r\n",Topic);
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTSUB=0,\"%s\",0,2\r\n",Topic);
	}
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,500ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//根据不同CAT类型执行不同的指令
			if(CATGlobalStatus.catType == CAT_YK){
				//判断收到的响应是否正确
				if(_CAT_MyStrCmp(0,"+MSUB: SUCCESS") != CAT_RECE_BUF_NO_STR){
					
					//此指令执行完毕，释放计数信号量
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}else if(_CAT_MyStrCmp(0,"+MSUB: FAILURE") != CAT_RECE_BUF_NO_STR){
					
					//此指令执行完毕，释放计数信号量
					osSemaphoreRelease(CATSemphrCountTxHandle);
					
					CATGlobalStatus.flag.mqttConn = 0;
					return HAL_ERROR;
				}
			}else{
				if(_CAT_MyStrCmp(0,"+MMQTTSUB: 0,0") != CAT_RECE_BUF_NO_STR){
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}else if(_CAT_MyStrCmp(0,"+MMQTTSUB: 0,1") != CAT_RECE_BUF_NO_STR){
					osSemaphoreRelease(CATSemphrCountTxHandle);
					
					CATGlobalStatus.flag.mqttConn = 0;
					
					return HAL_ERROR;
				}
			}
		}else{			
			//此指令执行完毕，释放计数信号量
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//超时，与CAT通讯失败
			if(TryCount == 3){
				return HAL_TIMEOUT;
			}else{
				goto CAT_SubTopicTag;
			}
		}
	}
}



/***
*CAT，发布长消息，并且当需要服务端响应消息时，则将接收缓存配置好，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
*参数：
	Topic：订阅的主题字符串
	Qos：消息传输质量
	SendBufLen：Payload总长度
	SendBuf：发布的消息内容
***/
HAL_StatusTypeDef CAT_M_PubTopic(char* Topic,uint8_t Qos,uint16_t SendBufLen,uint8_t* SendBuf)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	
	//先判断参数是否合法
	if(Topic == NULL || Qos > 2 || SendBuf == NULL || SendBufLen > 4200){
		return HAL_ERROR;
	}
	
	//判断是否连接MQTT服务端成功
	if(CATGlobalStatus.flag.mqttConn == 0 || CATGlobalStatus.flag.tcpConn == 0){
		return HAL_ERROR;
	}
	
CAT_PubTopicTag:
	TryCount++;
	
	//这里发送信号量和接收信号量都由调用者负责获取，因为发布消息后可能不需要等待服务端返回的消息，也可能需要等待服务端返回的消息
	CATSendCmdBuf.writeIndex = 0;
	
	//先发送指令，延迟25ms以上后，在发送内容
	//根据不同的CAT类型发送不同的指令
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MPUBEX=\"%s\",%u,0,%u\r\n",Topic,Qos,SendBufLen);
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTPUB=0,\"%s\",%u,0,%u,0,0\r\n",Topic,SendBufLen,Qos);
	}
	
	CATSendCmdBuf.writeIndex += TempLen;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,200);
	osKernelUnlock();
	
	osDelay(40);
	
	//通过DMA方式发送数据内容
	HAL_UART_Transmit_DMA(&huart4,SendBuf,SendBufLen);
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,30000ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//根据不同的CAT类型发送不同的指令
			if(CATGlobalStatus.catType == CAT_YK){
				if(_CAT_MyStrCmp(0,"+MPUBEX: SUCCESS") != CAT_RECE_BUF_NO_STR){
					return HAL_OK;
				}
			}else{
				if(_CAT_MyStrCmp(0,"+MMQTTPUB: 0,0") != CAT_RECE_BUF_NO_STR){
					return HAL_OK;
				}else if(_CAT_MyStrCmp(0,"+MMQTTPUB: 0,1") != CAT_RECE_BUF_NO_STR){
					return HAL_ERROR;
				}
			}
		}else{
			//超时，与CAT通讯失败，这里是消息没有发送到服务端，从而重发机制
			if(TryCount == 3){
				return HAL_TIMEOUT;
			}else{
				goto CAT_PubTopicTag;
			}
		}
	}
}




/***
*将从服务端发送MQTT数据帧从CAT接收缓存中复制到指定区域
*返回值：复制成功返回HAL_OK，否则返回HAL_ERROR
***/
static HAL_StatusTypeDef _CAT_M_CopyReceData(uint8_t* Buf,uint16_t* TotalLen)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	uint16_t ReStrIndex = 0;
	uint8_t i = 0;
	uint8_t TopicLen = 0;
	
	//根据不同的CAT执行不同的指令
	if(CATGlobalStatus.catType == CAT_YK){
		ReStrIndex = _CAT_MyStrCmp(0,"+MSUB: \"");
		if(ReStrIndex != CAT_RECE_BUF_NO_STR){
			ReStrIndex = _CAT_MyStrCmp(ReStrIndex,",");
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				if(_CAT_MyStrCmp(ReStrIndex," bytes") != CAT_RECE_BUF_NO_STR){
					*TotalLen = 0;
					
					//获取总内容长度值
					for(i=0;i<8;i++){
						ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
						
						if(CATReceBuf.buf[ReStrIndex] == ' '){
							break;
						}
						
						*TotalLen = (*TotalLen)*10+(CATReceBuf.buf[ReStrIndex]-'0');
					}
					
					//复制内容到结构体中分析
					return _CAT_MyStrCpy((ReStrIndex+8)&CAT_RECE_BUF_SIZE,Buf,*TotalLen);
				}
			}
		}
	}else{
		ReStrIndex = _CAT_MyStrCmp(0,"+MMQTTURC: 0,\"publish\"");
		if(ReStrIndex != CAT_RECE_BUF_NO_STR){
			ReStrIndex = _CAT_MyStrCmp(ReStrIndex,",");
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
				ReStrIndex = _CAT_MyStrCmp(ReStrIndex,",");
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
					ReStrIndex = _CAT_MyStrCmp(ReStrIndex,",");
					if(ReStrIndex != CAT_RECE_BUF_NO_STR){
						TopicLen = 0;
						ReStrIndex = (ReStrIndex+6)&CAT_RECE_BUF_SIZE;
					
						//获取主题字符串长度值
						for(i=0;i<8;i++){
							ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
							
							if(CATReceBuf.buf[ReStrIndex] == ','){
								break;
							}
							
							TopicLen = (TopicLen)*10+(CATReceBuf.buf[ReStrIndex]-'0');
						}
						
						*TotalLen = 0;
						
						//获取内容字符串长度值
						for(i=0;i<8;i++){
							ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
							
							if(CATReceBuf.buf[ReStrIndex] == '\r'){
								break;
							}
							
							*TotalLen = (*TotalLen)*10+(CATReceBuf.buf[ReStrIndex]-'0');
						}
						
						//复制内容到结构体中分析
						return _CAT_MyStrCpy((ReStrIndex+4+TopicLen)&CAT_RECE_BUF_SIZE,Buf,*TotalLen);
					}
				}
			}
		}
	}
	
	return HAL_ERROR;
}






//===MQTT业务逻辑实现========================================================================
/***
*发送参数非法消息给服务端
***/
static HAL_StatusTypeDef _CAT_B_Send_ParaIllegal(CAT_SERVICE_e CatService)
{
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern MachInfo_s MachInfo;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0000;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 0;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;

	
	//发布消息
	if(CAT_M_PubTopic(TopicString,2,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}




//===处理来自服务端的请求指令内容=======================================
/***
*设备响应服务端请求仪器所有参数信息指令
*返回值：返回HAL_OK表示完成响应，HAL_ERROR表示响应失败
***/
static HAL_StatusTypeDef _CAT_B_Respond_MachineAllParaInfo(uint16_t* TotalLen,MQTT_PayloadBuf_s* MQTTPayloadBuf)
{
	extern MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	osStatus_t xReturn = osOK;
//	uint16_t ReStrIndex = 0;
//	uint8_t i = 0;
	uint8_t TryCount = 0;
	
	
	
_CAT_B_Respond_MachineAllParaInfoTag:
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//发送仪器所有参数信息
	if(CAT_Send_MachineAllPara() != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回下一个操作消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)MQTTPayloadBuf,TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//这里相当于消息已经发送到服务端，但是服务端没有响应，从而重发机制
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto _CAT_B_Respond_MachineAllParaInfoTag;
			}
		}
	}
}




/***
*设备响应服务端请求更改仪器参数设置的指令
*参数：
	InfoType：表示参数类型
	Flag：为1表示更改成功，为0表示更改失败
*返回值：返回HAL_OK表示完成响应，HAL_ERROR表示响应失败
***/
static HAL_StatusTypeDef _CAT_B_Respond_ChangeSetInfo(uint16_t InfoType,uint8_t Flag,uint16_t* TotalLen,MQTT_PayloadBuf_s* MQTTPayloadBuf)
{
	extern MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
//	uint8_t i = 0;
	uint8_t TryCount = 0;
	
CAT_B_RespondChangeSetInfoTag:
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf->cmdH = 0x0004;
	MQTTPayloadBuf->cmdM = 0x00;
	MQTTPayloadBuf->cmdL = 0x01;
	MQTTPayloadBuf->cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf->packetNum = 0;
	MQTTPayloadBuf->payloadLen = sizeof(MQTTPayloadBuf->payload.cmd2_5_2);
	
	MQTTPayloadBuf->payload.cmd2_5_2.infoType = InfoType;
	MQTTPayloadBuf->payload.cmd2_5_2.result = Flag;
	
	MQTTPayloadBuf->crc16 = CRC16((uint8_t*)&MQTTPayloadBuf->payloadLen,MQTTPayloadBuf->payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf->payloadLen+14,(uint8_t*)MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回下一个操作消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)MQTTPayloadBuf,TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto CAT_B_RespondChangeSetInfoTag;
			}
		}
	}
}




/***
*设备响应服务端请求更改仪器特殊功能项设置的指令
*返回值：返回HAL_OK表示完成响应，HAL_ERROR表示响应失败
***/
static HAL_StatusTypeDef _CAT_B_Respond_ChangeSpecialFunInfo(uint8_t ItemFlag,uint16_t* TotalLen,MQTT_PayloadBuf_s* MQTTPayloadBuf)
{
	extern MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
//	uint16_t ReStrIndex = 0;
//	uint8_t i = 0;
	uint8_t TryCount = 0;
	
CAT_B_RespondChangeSpecialFunInfoTag:
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf->cmdH = 0x0004;
	MQTTPayloadBuf->cmdM = 0x01;
	MQTTPayloadBuf->cmdL = 0x01;
	MQTTPayloadBuf->cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf->packetNum = 0;
	MQTTPayloadBuf->payloadLen = sizeof(MQTTPayloadBuf->payload.cmd2_5_4);
	
	MQTTPayloadBuf->payload.cmd2_5_4.itemFlag = ItemFlag;
	MQTTPayloadBuf->payload.cmd2_5_4.resultFlag = 0x01;
	
	MQTTPayloadBuf->crc16 = CRC16((uint8_t*)&MQTTPayloadBuf->payloadLen,MQTTPayloadBuf->payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf->payloadLen+14,(uint8_t*)MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回下一个操作消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)MQTTPayloadBuf,TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto CAT_B_RespondChangeSpecialFunInfoTag;
			}
		}
	}
}









/************************************************************************************
*处理收到的服务端的请求数据内容
*返回值：HAL_OK表示服务端正常发送指令发送完毕指令，通讯控制权将转交给MCU；
		HAL_ERROR表示服务端请求的指令异常，且没有正常发送指令完毕指令，上一级调用者将
		挂起任务。
*************************************************************************************/
static HAL_StatusTypeDef _CAT_B_DealServiceRequestData(uint16_t* TotalLen,MQTT_PayloadBuf_s* MQTTPayloadBuf)
{
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern MachInfo_s MachInfo;
	extern __IO MachRunPara_s MachRunPara;
	extern __IO DataManage_t g_tDataManage;
	extern WM_HWIN CreateDialogPage(void);
	HAL_StatusTypeDef ReturnState = HAL_OK;
	
	
CAT_B_DealServiceRequestDataTag:
	//判断响应数据是否正确
	if(MQTTPayloadBuf->payloadLen+14 != *TotalLen){
		return HAL_ERROR;
	}
	
	//判断CRC是否正确
	if(CRC16((uint8_t*)&MQTTPayloadBuf->payloadLen,MQTTPayloadBuf->payloadLen+2) != MQTTPayloadBuf->crc16){
		return HAL_ERROR;
	}
	
	//处理数据
	switch((uint32_t)MQTTPayloadBuf->cmdH<<16|MQTTPayloadBuf->cmdM<<8|MQTTPayloadBuf->cmdL){
		//服务端请求指令发送完毕
		case 0x00030000:
			return HAL_OK;
		
		//服务端请求仪器所有参数设置信息
		case 0x00030100:
		{
			if(_CAT_B_Respond_MachineAllParaInfo(TotalLen,MQTTPayloadBuf) != HAL_OK){
				return HAL_ERROR;
			}
			goto CAT_B_DealServiceRequestDataTag;
		}
		
		//服务端请求更改仪器参数设定值
		case 0x00040000:
		{
			uint32_t TempU32 = 0;
			char TempStr[70] = {0};
			float TempF = 0;
			CommonDialogPageData_s DialogPageData = {0};
			WM_MESSAGE pMsg;
			extern WM_HWIN DialogWin;
			
			switch(MQTTPayloadBuf->payload.cmd2_5_1.infoType){
				//更改仪器是否锁定的状态
				case 0:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					TempU32 = MachInfo.flag.bit.lock;
					if(MachInfo.flag.bit.lock != (MQTTPayloadBuf->payload.cmd2_5_1.u.lockFlag==0xA5?1:0)){
						MachInfo.flag.bit.lock = (MQTTPayloadBuf->payload.cmd2_5_1.u.lockFlag==0xA5?1:0);
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_FLAG,(uint8_t*)&MachInfo.flag,sizeof(MachInfo.flag));
						if(ReturnState != HAL_OK){
							MachInfo.flag.bit.lock = TempU32;
						}else{
							//判断是否需要锁机或者解锁
							if(MachInfo.flag.bit.lock == 1){
								//弹窗提示
								if(DialogWin == NULL){
									WM_HideWindow(g_hActiveWin);
									DialogWin = CreateDialogPage();
								}
								
								//弹窗提示
								DialogPageData.bmpType = BMP_ALARM;
								DialogPageData.confimButt = NO_CONFIM_BUTTON;
								DialogPageData.fun = NULL;
								strcpy(DialogPageData.str,g_ucaLng_LOCK_MACHINE[MachInfo.systemSet.eLanguage]);		//锁定

								pMsg.MsgId = WM_USER_DATA;
								pMsg.Data.p = &DialogPageData;
								WM_SendMessage(DialogWin,&pMsg);
							}else{
								//弹窗提示
								if(DialogWin == NULL){
									WM_HideWindow(g_hActiveWin);
									DialogWin = CreateDialogPage();
								}
								
								//弹窗提示
								DialogPageData.bmpType = BMP_OK;
								DialogPageData.confimButt = NO_CONFIM_BUTTON;
								DialogPageData.fun = NULL;
								strcpy(DialogPageData.str,g_ucaLng_UNLOCK_MACHINE[MachInfo.systemSet.eLanguage]);		//解锁

								pMsg.MsgId = WM_USER_DATA;
								pMsg.Data.p = &DialogPageData;
								WM_SendMessage(DialogWin,&pMsg);
							}
						}
					}
				}break;
				
				//修改诊所名字符串
				case 1:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 50){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(strcmp((char*)MachInfo.labInfo.ucaClinicName,MQTTPayloadBuf->payload.cmd2_5_1.u.zhensuoName) != 0){
						strcpy(TempStr,(char*)MachInfo.labInfo.ucaClinicName);
						strcpy((char*)MachInfo.labInfo.ucaClinicName,MQTTPayloadBuf->payload.cmd2_5_1.u.zhensuoName);
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_LAB_INFO+((uint8_t*)MachInfo.labInfo.ucaClinicName-(uint8_t*)&MachInfo.labInfo),(uint8_t*)MachInfo.labInfo.ucaClinicName,sizeof(MachInfo.labInfo.ucaClinicName));
						if(ReturnState != HAL_OK){
							strcpy((char*)MachInfo.labInfo.ucaClinicName,TempStr);
						}
					}
				}break;
				
				//修改WBC增益值
				case 2:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.paramSet.ucWBC_Gain != MQTTPayloadBuf->payload.cmd2_5_1.u.wbcPara){
						TempU32 = MachInfo.paramSet.ucWBC_Gain;
						MachInfo.paramSet.ucWBC_Gain = MQTTPayloadBuf->payload.cmd2_5_1.u.wbcPara;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.ucWBC_Gain-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.ucWBC_Gain,sizeof(MachInfo.paramSet.ucWBC_Gain));
						if(ReturnState != HAL_OK){
							MachInfo.paramSet.ucWBC_Gain = TempU32;
						}
						
						//写入WBC增益
						WBC_Gran_Set(WBC_DR_CHANNEL,MachInfo.paramSet.ucWBC_Gain);
					}
				}break;
				
				//修改HGB增益值
				case 3:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.paramSet.ucHGB_Gain != MQTTPayloadBuf->payload.cmd2_5_1.u.hgbPara){
						TempU32 = MachInfo.paramSet.ucHGB_Gain;
						MachInfo.paramSet.ucHGB_Gain = MQTTPayloadBuf->payload.cmd2_5_1.u.hgbPara;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.ucHGB_Gain-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.ucHGB_Gain,sizeof(MachInfo.paramSet.ucHGB_Gain));
						if(ReturnState != HAL_OK){
							MachInfo.paramSet.ucHGB_Gain = TempU32;
						}
						//写入HGB增益
						HGB_Gran_Set(HGB_DR_CHANNEL,MachInfo.paramSet.ucHGB_Gain);
					}
				}break;
				
				//修改泵占空比值
				case 4:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.pumpPara > 100){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.paramSet.ucPump_Scale != MQTTPayloadBuf->payload.cmd2_5_1.u.pumpPara){
						TempU32 = MachInfo.paramSet.ucPump_Scale;
						MachInfo.paramSet.ucPump_Scale = MQTTPayloadBuf->payload.cmd2_5_1.u.hgbPara;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.ucPump_Scale-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.ucPump_Scale,sizeof(MachInfo.paramSet.ucPump_Scale));
						if(ReturnState != HAL_OK){
							MachInfo.paramSet.ucPump_Scale = TempU32;
						}
					}
				}break;
				
				//修改535电流设定值
				case 5:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.current535 > 4095){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.paramSet.current535 != MQTTPayloadBuf->payload.cmd2_5_1.u.current535){
						TempU32 = MachInfo.paramSet.current535;
						MachInfo.paramSet.current535 = MQTTPayloadBuf->payload.cmd2_5_1.u.current535;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.current535-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.current535,sizeof(MachInfo.paramSet.current535));
						if(ReturnState != HAL_OK){
							MachInfo.paramSet.current535 = TempU32;
						}
					}
				}break;
					
				//修改415电流设定值
				case 6:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.current415 > 4095){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.paramSet.current415 != MQTTPayloadBuf->payload.cmd2_5_1.u.current415){
						TempU32 = MachInfo.paramSet.current415;
						MachInfo.paramSet.current415 = MQTTPayloadBuf->payload.cmd2_5_1.u.current415;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.current415-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.current415,sizeof(MachInfo.paramSet.current415));
						if(ReturnState != HAL_OK){
							MachInfo.paramSet.current415 = TempU32;
						}
					}
				}break;
				
				//修改出仓光耦补偿值
				case 7:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.outOC > 20000){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.paramSet.ulOutIn_Out_AddStep != MQTTPayloadBuf->payload.cmd2_5_1.u.outOC){
						TempU32 = MachInfo.paramSet.ulOutIn_Out_AddStep;
						MachInfo.paramSet.ulOutIn_Out_AddStep = MQTTPayloadBuf->payload.cmd2_5_1.u.outOC;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.ulOutIn_Out_AddStep-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.ulOutIn_Out_AddStep,sizeof(MachInfo.paramSet.ulOutIn_Out_AddStep));
						if(ReturnState != HAL_OK){
							MachInfo.paramSet.ulOutIn_Out_AddStep = TempU32;
						}
					}
				}break;
				
				//修改进仓光耦补偿值
				case 8:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.inOC > 20000){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.paramSet.ulOutIn_In_AddStep != MQTTPayloadBuf->payload.cmd2_5_1.u.inOC){
						TempU32 = MachInfo.paramSet.ulOutIn_In_AddStep;
						MachInfo.paramSet.ulOutIn_In_AddStep = MQTTPayloadBuf->payload.cmd2_5_1.u.inOC;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.ulOutIn_In_AddStep-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.ulOutIn_In_AddStep,sizeof(MachInfo.paramSet.ulOutIn_In_AddStep));
						if(ReturnState != HAL_OK){
							MachInfo.paramSet.ulOutIn_In_AddStep = TempU32;
						}
					}
				}break;
				
				//修改固定光耦补偿值
				case 9:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.gdOC > 20000){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.paramSet.ulClamp_Out_AddStep != MQTTPayloadBuf->payload.cmd2_5_1.u.gdOC){
						TempU32 = MachInfo.paramSet.ulClamp_Out_AddStep;
						MachInfo.paramSet.ulClamp_Out_AddStep = MQTTPayloadBuf->payload.cmd2_5_1.u.gdOC;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.ulClamp_Out_AddStep-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.ulClamp_Out_AddStep,sizeof(MachInfo.paramSet.ulClamp_Out_AddStep));
						if(ReturnState != HAL_OK){
							MachInfo.paramSet.ulClamp_Out_AddStep = TempU32;
						}
					}
				}break;
				
				//修改释放光耦补偿值
				case 10:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.sfOC > 20000){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.paramSet.ulClamp_In_AddStep != MQTTPayloadBuf->payload.cmd2_5_1.u.sfOC){
						TempU32 = MachInfo.paramSet.ulClamp_In_AddStep;
						MachInfo.paramSet.ulClamp_In_AddStep = MQTTPayloadBuf->payload.cmd2_5_1.u.sfOC;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.ulClamp_In_AddStep-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.ulClamp_In_AddStep,sizeof(MachInfo.paramSet.ulClamp_In_AddStep));
						if(ReturnState != HAL_OK){
							MachInfo.paramSet.ulClamp_In_AddStep = TempU32;
						}
					}
				}break;
				
				//修改压力补偿值
				case 11:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.presurePara > 100 || MQTTPayloadBuf->payload.cmd2_5_1.u.presurePara < -100){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.paramSet.fPress_Adjust != MQTTPayloadBuf->payload.cmd2_5_1.u.presurePara/10.0){
						TempF = MachInfo.paramSet.fPress_Adjust;
						MachInfo.paramSet.fPress_Adjust = MQTTPayloadBuf->payload.cmd2_5_1.u.presurePara/10.0;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.fPress_Adjust-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.fPress_Adjust,sizeof(MachInfo.paramSet.fPress_Adjust));
						if(ReturnState != HAL_OK){
							MachInfo.paramSet.fPress_Adjust = TempF;
						}
					}
				}break;
				
				//修改WBC校准系数值
				case 12:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if((uint16_t)MachInfo.calibration.fWBC*100 != MQTTPayloadBuf->payload.cmd2_5_1.u.wbcAdjust){
						TempF = MachInfo.calibration.fWBC;
						MachInfo.calibration.fWBC = MQTTPayloadBuf->payload.cmd2_5_1.u.wbcAdjust/100.0;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CALIBRATION+((uint8_t*)&MachInfo.calibration.fWBC-(uint8_t*)&MachInfo.calibration),(uint8_t*)&MachInfo.calibration.fWBC,sizeof(MachInfo.calibration.fWBC));
						if(ReturnState != HAL_OK){
							MachInfo.calibration.fWBC = TempF;
						}
					}
				}break;
				
				//修改HGB校准系数值
				case 13:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if((uint16_t)MachInfo.calibration.fHGB*100 != MQTTPayloadBuf->payload.cmd2_5_1.u.hgbAdjust){
						TempF = MachInfo.calibration.fHGB;
						MachInfo.calibration.fHGB = MQTTPayloadBuf->payload.cmd2_5_1.u.hgbAdjust/100.0;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CALIBRATION+((uint8_t*)&MachInfo.calibration.fHGB-(uint8_t*)&MachInfo.calibration),(uint8_t*)&MachInfo.calibration.fHGB,sizeof(MachInfo.calibration.fHGB));
						if(ReturnState != HAL_OK){
							MachInfo.calibration.fHGB = TempF;
						}
					}
				}break;
					
				//修改当前使用的压力传感器类型
				case 14:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.pressureSensor > 1){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.paramSet.pressSensor != MQTTPayloadBuf->payload.cmd2_5_1.u.pressureSensor){
						TempU32 = MachInfo.paramSet.pressSensor;
						MachInfo.paramSet.pressSensor = MQTTPayloadBuf->payload.cmd2_5_1.u.pressureSensor;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_PARAMSET+((uint8_t*)&MachInfo.paramSet.pressSensor-(uint8_t*)&MachInfo.paramSet),(uint8_t*)&MachInfo.paramSet.pressSensor,sizeof(MachInfo.paramSet.pressSensor));
						if(ReturnState != HAL_OK){
							MachInfo.paramSet.pressSensor = TempU32;
						}
					}
				}break;
					
				//修改当前仪器所属公司
				case 15:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.company >= COMPANY_END){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MachInfo.companyInfo.company != MQTTPayloadBuf->payload.cmd2_5_1.u.company){
						TempU32 = MachInfo.companyInfo.company;
						MachInfo.companyInfo.company = MQTTPayloadBuf->payload.cmd2_5_1.u.company;
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_COMPANY+((uint8_t*)&MachInfo.companyInfo.company-(uint8_t*)&MachInfo.companyInfo),(uint8_t*)&MachInfo.companyInfo.company,sizeof(MachInfo.companyInfo.company));
						if(ReturnState != HAL_OK){
							MachInfo.companyInfo.company = TempU32;
						}else{
							//更新公司特性参数
							Company_ConfigInfo(MachInfo.companyInfo.company);
						}
					}
				}break;
				
				//修改YongFu账户密码
				case 16:
				{
					if(MQTTPayloadBuf->payloadLen-2 != ACCOUNT_PSW_MAX_LEN){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(strlen(MQTTPayloadBuf->payload.cmd2_5_1.u.yongFuPsw) == 0){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(strcmp(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].psw,MQTTPayloadBuf->payload.cmd2_5_1.u.yongFuPsw) != 0){
						memset(TempStr,0,ACCOUNT_PSW_MAX_LEN+5);
						strcpy(TempStr,MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].psw);
						
						memset(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].psw,0,ACCOUNT_PSW_MAX_LEN);
						strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].psw,MQTTPayloadBuf->payload.cmd2_5_1.u.yongFuPsw);
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_ACCOUNT_MM,(uint8_t*)&MachInfo.accountMM,sizeof(MachInfo.accountMM));
						if(ReturnState != HAL_OK){
							memset(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].psw,0,ACCOUNT_PSW_MAX_LEN);
							strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].psw,TempStr);
						}
					}
				}break;
				
				//修改admin账户密码
				case 17:
				{
					if(MQTTPayloadBuf->payloadLen-2 != ACCOUNT_PSW_MAX_LEN){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(strlen(MQTTPayloadBuf->payload.cmd2_5_1.u.adminPsw) == 0){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(strcmp(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,MQTTPayloadBuf->payload.cmd2_5_1.u.adminPsw) != 0){
						memset(TempStr,0,ACCOUNT_PSW_MAX_LEN+5);
						strcpy(TempStr,MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw);
						
						memset(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,0,ACCOUNT_PSW_MAX_LEN);
						strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,MQTTPayloadBuf->payload.cmd2_5_1.u.adminPsw);
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_ACCOUNT_MM,(uint8_t*)&MachInfo.accountMM,sizeof(MachInfo.accountMM));
						if(ReturnState != HAL_OK){
							memset(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,0,ACCOUNT_PSW_MAX_LEN);
							strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,TempStr);
						}
					}
				}break;
				
				//参数异常
				default :
					//发送参数非法消息给服务端
					_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
			
			if(ReturnState != HAL_OK){
				//设备发送参数更改失败消息
				if(_CAT_B_Respond_ChangeSetInfo(MQTTPayloadBuf->payload.cmd2_5_1.infoType,0,TotalLen,MQTTPayloadBuf) != HAL_OK){
					return HAL_ERROR;
				}
			}else{
				//判断是否更改仪器是否锁定状态
				if(MQTTPayloadBuf->payload.cmd2_5_1.infoType == 0){
					if(TempU32 != MachInfo.flag.bit.lock){
						//标记需要重启仪器
						CATGlobalStatus.catDealSysRestartFlag = 0xA55A0FF0;
					}
				}
				
				//设备发送参数更改完成消息
				if(_CAT_B_Respond_ChangeSetInfo(MQTTPayloadBuf->payload.cmd2_5_1.infoType,1,TotalLen,MQTTPayloadBuf) != HAL_OK){
					return HAL_ERROR;
				}
			}
			
			goto CAT_B_DealServiceRequestDataTag;
		}
		
		//服务端请求开启特殊功能
		case 0x00040100:
		{
			switch(MQTTPayloadBuf->payload.u8){ 
				case 0x00:
					CATGlobalStatus.ignoreFunFlag.JSCDect = 1;
				break;
					
				case 0x01:
					CATGlobalStatus.ignoreFunFlag.OCDect = 1;
				break;
					
				case 0x02:
					CATGlobalStatus.ignoreFunFlag.JSCReuse = 1;
				break;
					
				default :break;
			}
		
			//设备发送特殊功能执行标志更改完成消息
			if(_CAT_B_Respond_ChangeSpecialFunInfo(MQTTPayloadBuf->payload.u8,TotalLen,MQTTPayloadBuf) != HAL_OK){
				return HAL_ERROR;
			}
			goto CAT_B_DealServiceRequestDataTag;
		}
		
		
		//===第三方服务端请求相关===
		//第三方服务端请求指令发送完毕
		case 0xA0040200:
			return HAL_OK;
		
		//第三方服务端请求更改仪器参数值
		case 0xA0040100:
		{
			char TempStr[70] = {0};
			
			switch(MQTTPayloadBuf->payload.cmdA_2_6_2.infoType){
				//修改仪器admin账户密码
				case 0:
				{
					if(MQTTPayloadBuf->payloadLen-1 != ACCOUNT_PSW_MAX_LEN){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_OTHER);
						return HAL_ERROR;
					}
					
					if(strlen(MQTTPayloadBuf->payload.cmdA_2_6_2.u.adminPsw) == 0){
						//发送参数非法消息给服务端
						_CAT_B_Send_ParaIllegal(SERVICE_OTHER);
						return HAL_ERROR;
					}
					
					if(strcmp(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,MQTTPayloadBuf->payload.cmdA_2_6_2.u.adminPsw) != 0){
						memset(TempStr,0,ACCOUNT_PSW_MAX_LEN+5);
						strcpy(TempStr,MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw);
						
						memset(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,0,ACCOUNT_PSW_MAX_LEN);
						strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,MQTTPayloadBuf->payload.cmdA_2_6_2.u.adminPsw);
						ReturnState = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_ACCOUNT_MM,(uint8_t*)&MachInfo.accountMM,sizeof(MachInfo.accountMM));
						if(ReturnState != HAL_OK){
							memset(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,0,ACCOUNT_PSW_MAX_LEN);
							strcpy(MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_ADMIN].psw,TempStr);
						}
					}
				}
				break;
				
				
				//参数异常
				default :
					//发送参数非法消息给服务端
					_CAT_B_Send_ParaIllegal(SERVICE_OTHER);
				return HAL_ERROR;
			}
			
			if(ReturnState != HAL_OK){
				//设备发送参数更改失败消息
				if(CAT_B_Other_Respond_ChangeSetInfo(MQTTPayloadBuf->payload.cmdA_2_6_2.infoType,0,TotalLen,MQTTPayloadBuf) != HAL_OK){
					return HAL_ERROR;
				}
			}else{
				//设备发送参数更改完成消息
				if(CAT_B_Other_Respond_ChangeSetInfo(MQTTPayloadBuf->payload.cmdA_2_6_2.infoType,1,TotalLen,MQTTPayloadBuf) != HAL_OK){
					return HAL_ERROR;
				}
			}
			
			goto CAT_B_DealServiceRequestDataTag;
		}
			
		default :return HAL_ERROR;
	}
}





//===处理来自服务端响应消息内容============================================
/***
*处理收到的服务端的响应数据内容
***/
HAL_StatusTypeDef CAT_B_DealServiceAckData(uint16_t SendCmdH,uint8_t SendCmdM,uint8_t SendCmdL,uint16_t TotalLen,MQTT_PayloadBuf_s* MQTTPayloadBuf)
{
	extern MachInfo_s	MachInfo;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern CATGlobalStatus_s CATGlobalStatus;
	HAL_StatusTypeDef ReturnStates = HAL_OK;
	
	//判断响应数据是否是服务端发送的参数非法帧
	if(((uint32_t)MQTTPayloadBuf->cmdH<<16|MQTTPayloadBuf->cmdM<<8|MQTTPayloadBuf->cmdL) == 0x00000000){
		osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_MACHINE_PARA_ILLEGAL);
		return HAL_ERROR;
	}
	
	//判断响应数据是否正确
	if(MQTTPayloadBuf->cmdH != SendCmdH || MQTTPayloadBuf->cmdM != SendCmdM /*|| MQTTPayloadBuf->cmdL != (SendCmdL+1)*/ || MQTTPayloadBuf->payloadLen+14 != TotalLen){
		return HAL_ERROR;
	}
	
	//判断CRC是否正确
	if(CRC16((uint8_t*)&MQTTPayloadBuf->payloadLen,MQTTPayloadBuf->payloadLen+2) != MQTTPayloadBuf->crc16){
		return HAL_ERROR;
	}
	
	//处理数据
	switch((uint32_t)MQTTPayloadBuf->cmdH<<16|MQTTPayloadBuf->cmdM<<8|MQTTPayloadBuf->cmdL){
		//响应仪器出厂信息是否已经录入
		case 0x00010101:
		{
			if(MQTTPayloadBuf->payload.u8 > 1){
				//发送参数非法消息给服务端
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}else if(MQTTPayloadBuf->payload.u8 == 1){
				//发送序列号已存在事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_FACTORY_INFO_EXIST);
				return HAL_BUSY;
			}
			
			//获取TX信号量
			osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
			
			//发送当前仪器中所有的参数信息
			ReturnStates = CAT_Send_MachineAllPara();
			
			//释放信号量
			osSemaphoreRelease(CATSemphrCountTxHandle);
			return ReturnStates;
		}
		
		//服务端响应当前保存的最新发布版本号信息
		case 0x00010201:
		{
			//先判断参数是否非法
			if(MQTTPayloadBuf->payload.cmd2_2_5.bootloadVersion > 999 ||  MQTTPayloadBuf->payload.cmd2_2_5.mainVersion > 999 || MQTTPayloadBuf->payload.cmd2_2_5.uiVersion > 999 ){
				//发送参数非法消息给服务端
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
		
			CATGlobalStatus.serviceVersion.PBootloadVersion = MQTTPayloadBuf->payload.cmd2_2_5.bootloadVersion;
			CATGlobalStatus.serviceVersion.PMainVersion = MQTTPayloadBuf->payload.cmd2_2_5.mainVersion;
			CATGlobalStatus.serviceVersion.PUiVersion = MQTTPayloadBuf->payload.cmd2_2_5.uiVersion;
			return HAL_OK;
		}
		
		//服务端响应请求的程序文件信息
		case 0x00020101:
		{
			//先判断参数是否非法
			if(MQTTPayloadBuf->payload.cmd2_3_4.publishOrTestFlag > 1 || MQTTPayloadBuf->payload.cmd2_3_4.softwareType > 1){
				//发送参数非法消息给服务端
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
			
			CATGlobalStatus.softwareInfo.publishOrTestFlag = MQTTPayloadBuf->payload.cmd2_3_4.publishOrTestFlag;
			CATGlobalStatus.softwareInfo.softwareType = MQTTPayloadBuf->payload.cmd2_3_4.softwareType;
			CATGlobalStatus.softwareInfo.totalLen = MQTTPayloadBuf->payload.cmd2_3_4.totalLen;
			CATGlobalStatus.softwareInfo.crc16 = MQTTPayloadBuf->payload.cmd2_3_4.crc16;
			
			return HAL_OK;
		}
		
		//服务端响应请求的UI资源信息
		case 0x00020201:
		{
			uint8_t i = 0;
			
			//先判断参数是否非法
			if(MQTTPayloadBuf->payload.cmd2_3_6.publishOrTestFlag > 1 || MQTTPayloadBuf->payload.cmd2_3_6.totalNum > 10 || MQTTPayloadBuf->payload.cmd2_3_6.uiVersion != MachInfo.other.uiVersion+1){
				//发送参数非法消息给服务端
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
			
			CATGlobalStatus.uiInfo.publishOrTestFlag = MQTTPayloadBuf->payload.cmd2_3_6.publishOrTestFlag;
			CATGlobalStatus.uiInfo.uiVersion = MQTTPayloadBuf->payload.cmd2_3_6.uiVersion;
			CATGlobalStatus.uiInfo.totalNum = MQTTPayloadBuf->payload.cmd2_3_6.totalNum;
			
			for(i=0;i<CATGlobalStatus.uiInfo.totalNum;i++){
				CATGlobalStatus.uiInfo.file[i].fileSize = MQTTPayloadBuf->payload.cmd2_3_6.file[i].fileSize;
				CATGlobalStatus.uiInfo.file[i].crc16 = MQTTPayloadBuf->payload.cmd2_3_6.file[i].crc16;
				strcpy(CATGlobalStatus.uiInfo.file[i].path,MQTTPayloadBuf->payload.cmd2_3_6.file[i].path);
			}
			
			return HAL_OK;
		}
		
		//服务端响应请求的指定发布版或者测试版的各模块版本号
		case 0x00020001:
		{
			//先判断参数是否非法
			if(MQTTPayloadBuf->payload.cmd2_3_2.bootloadVersion > 999 ||  MQTTPayloadBuf->payload.cmd2_3_2.mainVersion > 999 || MQTTPayloadBuf->payload.cmd2_3_2.uiVersion > 999 || MQTTPayloadBuf->payload.cmd2_3_2.publishOrTestFlag >1 ){
				//发送参数非法消息给服务端
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
			
			if(MQTTPayloadBuf->payload.cmd2_3_2.publishOrTestFlag == 0){
				CATGlobalStatus.serviceVersion.PBootloadVersion = MQTTPayloadBuf->payload.cmd2_3_2.bootloadVersion;
				CATGlobalStatus.serviceVersion.PMainVersion = MQTTPayloadBuf->payload.cmd2_3_2.mainVersion;
				CATGlobalStatus.serviceVersion.PUiVersion = MQTTPayloadBuf->payload.cmd2_3_2.uiVersion;
			}else{
				CATGlobalStatus.serviceVersion.TBootloadVersion = MQTTPayloadBuf->payload.cmd2_3_2.bootloadVersion;
				CATGlobalStatus.serviceVersion.TMainVersion = MQTTPayloadBuf->payload.cmd2_3_2.mainVersion;
				CATGlobalStatus.serviceVersion.TUiVersion = MQTTPayloadBuf->payload.cmd2_3_2.uiVersion;
			}
			
			return HAL_OK;
		}
		
		
		//服务端响应请求的指定患者编号的患者信息
		case 0x00070001:
		{
			//先判断参数是否非法
			if(strcmp(MQTTPayloadBuf->payload.cmd2_8_2.patientID,(char*)MachRunPara.tNextSample.patientID) != 0 || MQTTPayloadBuf->payload.cmd2_8_2.catTestType != TEST_TYPE_WBC_HGB){
				//发送参数非法消息给服务端
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
			
			//保存患者信息
			strcpy((char*)MachRunPara.tNextSample.ucaName,MQTTPayloadBuf->payload.cmd2_8_2.name);
			MachRunPara.tNextSample.ucAge = MQTTPayloadBuf->payload.cmd2_8_2.age;
			MachRunPara.tNextSample.eAgeUnit = MQTTPayloadBuf->payload.cmd2_8_2.ageUnit;
			MachRunPara.tNextSample.eSex = MQTTPayloadBuf->payload.cmd2_8_2.gender;
			strcpy((char*)MachRunPara.tNextSample.orderNum,MQTTPayloadBuf->payload.cmd2_8_2.orderNum);
			
			//自动匹配参考组
			MachRunPara.tNextSample.eReferGroup = AgeToRefGroup(MachRunPara.tNextSample.ucAge,MachRunPara.tNextSample.eAgeUnit,MachRunPara.tNextSample.eSex);
			
			//下拉患者信息成功事件
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_PATIENT_INFO_OK);
			
			return HAL_OK;
		}
		
		//服务端响应请求的指定患者编号不存在患者信息
		case 0x00070002:
		{
			//先判断参数是否非法
			if(strcmp(MQTTPayloadBuf->payload.cmd2_8_3.patientID,(char*)MachRunPara.tNextSample.patientID) != 0 || MQTTPayloadBuf->payload.cmd2_8_3.catTestType != TEST_TYPE_WBC_HGB){
				//发送参数非法消息给服务端
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
			
			//下拉患者信息不存在事件
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_NO_PATIENT_INFO);
			
			return HAL_OK;
		}
		
		
		
		
//===第三方服务端相关================================		
		//第三方服务端响应请求的指定患者编号的患者信息
		case 0xA0050001:
		{
			//先判断参数是否非法
			if(MQTTPayloadBuf->payload.cmdA_2_5_2.catTestType != TEST_TYPE_WBC_HGB || MQTTPayloadBuf->payload.cmdA_2_5_2.pageNum != MachRunPara.patientList.pageNum){
				//发送参数非法消息给服务端
				_CAT_B_Send_ParaIllegal(SERVICE_OTHER);
				return HAL_ERROR;
			}
			
			//判断是否不存在患者信息
			if(MQTTPayloadBuf->payload.cmdA_2_5_2.num == 0){
				//下拉患者信息不存在事件
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_NO_PATIENT_INFO);
				return HAL_OK;
			}
			
			//保存患者信息列表
			MachRunPara.patientList.num = MQTTPayloadBuf->payload.cmdA_2_5_2.num;
			memcpy((uint8_t*)MachRunPara.patientList.info,(uint8_t*)MQTTPayloadBuf->payload.cmdA_2_5_2.info,sizeof(MachRunPara.patientList.info));
			
			//下拉患者信息成功事件
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_PATIENT_INFO_OK);
			
			return HAL_OK;
		}
		
		//服务端确认收到故障日志消息
		case 0xA0020001:
		{
			//先判断参数是否非法
			if(MQTTPayloadBuf->payload.cmdA_2_4_2.catTestType != TEST_TYPE_WBC_HGB || MQTTPayloadBuf->payload.cmdA_2_4_2.logSN != MachRunPara.currSN.logSN){
				//发送参数非法消息给服务端
				_CAT_B_Send_ParaIllegal(SERVICE_OTHER);
				return HAL_ERROR;
			}
			
			return HAL_OK;
		}
		
		//服务端确认收到样本分析结果消息
		case 0xA0010001:
		{
			//先判断参数是否非法
			if(MQTTPayloadBuf->payload.cmdA_2_3_3.catTestType != TEST_TYPE_WBC_HGB || MQTTPayloadBuf->payload.cmdA_2_3_3.sampleSN != MachRunPara.currSN.sampleSN){
				//发送参数非法消息给服务端
				_CAT_B_Send_ParaIllegal(SERVICE_OTHER);
				return HAL_ERROR;
			}
			
			return HAL_OK;
		}
		
		default :return HAL_ERROR;
	}
}







//===发送数据到服务端=======================================================================
/***
*建立MQTT连接，这里必须将仪器序列号录入后才会连接后台，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_CreateMQTTConn(CAT_SERVICE_e CatService)
{
	extern MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osThreadId_t CATTaskHandle;
	char StrTemp[50];
	uint16_t TempLen = 0;
	
	//先判断仪器序列号是否已经录入，否，则直接退出，返回ERROR
	while(strlen((char*)MachInfo.labInfo.ucaMachineSN) < 12){
		CATGlobalStatus.flag.connServerErr = 1;
		osThreadSuspend(CATTaskHandle);
		CATGlobalStatus.flag.connServerErr = 0;
	}
	
	TempLen = sprintf(StrTemp,"C/WM/%s",MachInfo.labInfo.ucaMachineSN);
	StrTemp[TempLen] = '\0';
	
	//配置MQTT参数
	if(CatService == SERVICE_CC){
		//创怀服务端
		if(CAT_M_ConfigClient(CatService,(char*)MachInfo.labInfo.ucaMachineSN,MQTT_SERVICE_USER_NAME,MQTT_SERVICE_PASSWORD,StrTemp,"Offline") != HAL_OK){
			return HAL_ERROR;
		}
	}else{
		//第三方服务端
		if(CAT_M_ConfigClient(CatService,(char*)MachInfo.labInfo.ucaMachineSN,MachInfo.companyInfo.mqtt.name,MachInfo.companyInfo.mqtt.psw,StrTemp,"Offline") != HAL_OK){
			return HAL_ERROR;
		}
	}
	
	//移柯CAT才需要下面指令发送
	if(CATGlobalStatus.catType == CAT_YK){
		//连接服务端
		if(CAT_M_CreateTcpConn(MQTT_SERVICE_IP,MQTT_SERVICE_PORT) != HAL_OK){
			return HAL_ERROR;
		}
		if(CAT_M_CreateMQTTConn() != HAL_OK){
			return HAL_ERROR;
		}
	}
	
	//订阅主题
	TempLen = sprintf(StrTemp,"S/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	StrTemp[TempLen] = '\0';
	if(CAT_M_SubTopic(StrTemp) != HAL_OK){
		return HAL_ERROR;
	}
	
	return HAL_OK;
}




/***
*断开MQTT连接，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_CloseMQTTConn(void)
{
	if(CAT_M_CloseMQTTConn() != HAL_OK){
		osDelay(5);
		return HAL_ERROR;
	}
	
	osDelay(5);
	return HAL_OK;
}




/***
*询问服务端是否有数据发送给设备，切换控制权给服务端。只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_IsServiceWantToSendData(void)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
//	uint16_t ReStrIndex = 0;
	uint16_t TotalLen = 0;
//	uint8_t i = 0;
	uint8_t TryCount = 0;
	
CAT_B_IsServiceWantToSendDataTag:
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0001;
	MQTTPayloadBuf.cmdM = 0x04;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 0;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;

	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}

	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);

		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
							
				//处理服务端发送的请求消息
				if(_CAT_B_DealServiceRequestData(&TotalLen,&MQTTPayloadBuf) != HAL_OK){
					if(TryCount >= 2){
						return HAL_ERROR;
					}else{
						goto CAT_B_IsServiceWantToSendDataTag;
					}
				}
				
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto CAT_B_IsServiceWantToSendDataTag;
			}
		}
	}
}



/***
*设备发送所有参数信息给服务端，注意，由于此指令被多个场景调用，所以这里不获取和释放TX信号量，
而是由上一级调用者来获取即可。只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_Send_MachineAllPara(void)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	char StrTemp[20] = {0};
	uint8_t Len = 0;
	uint8_t i = 0;
	uint16_t AlgoVersion = 0;
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//转换字符串算法版本为整型值
	Len = sprintf(StrTemp,"%s",version());
	for(i=0;i<Len;i++){
		if(StrTemp[i] >= '0' && StrTemp[i] <= '9'){
			AlgoVersion = AlgoVersion*10+(StrTemp[i]-'0');
		}
	}
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0001;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmd2_2_1);
	
	MQTTPayloadBuf.payload.cmd2_2_1.softwareVersion = FULL_VERSION;
	MQTTPayloadBuf.payload.cmd2_2_1.mainVersion = MAIN_VERSION;
	MQTTPayloadBuf.payload.cmd2_2_1.bootloadVersion = *((uint16_t*)BOOTLOAD_VERSION_ADR);
	MQTTPayloadBuf.payload.cmd2_2_1.uiVersion = MachInfo.other.uiVersion;
	MQTTPayloadBuf.payload.cmd2_2_1.algoVersion = AlgoVersion;
	strcpy(MQTTPayloadBuf.payload.cmd2_2_1.mainHardwareVersion,(char*)MachRunPara.hardVer.main);
	strcpy(MQTTPayloadBuf.payload.cmd2_2_1.zjHardwareVersion,(char*)MachRunPara.hardVer.zjb);
	strcpy(MQTTPayloadBuf.payload.cmd2_2_1.imei,CATGlobalStatus.imei);
	strcpy(MQTTPayloadBuf.payload.cmd2_2_1.iccid,CATGlobalStatus.iccid);
	MQTTPayloadBuf.payload.cmd2_2_1.operatorNum = CATGlobalStatus.operatorNum;
	MQTTPayloadBuf.payload.cmd2_2_1.languMode = MachInfo.systemSet.eLanguage;
	MQTTPayloadBuf.payload.cmd2_2_1.lockFlag = MachInfo.flag.bit.lock==1?0xA5:0;
	MQTTPayloadBuf.payload.cmd2_2_1.factoryTimestamp = MachInfo.other.factoryTimestamp;
	MQTTPayloadBuf.payload.cmd2_2_1.userFirstOpenTimestamp = MachInfo.other.userTimestamp;
	MQTTPayloadBuf.payload.cmd2_2_1.company = MachInfo.companyInfo.company;
	strcpy(MQTTPayloadBuf.payload.cmd2_2_1.zhensuoName,(char*)MachInfo.labInfo.ucaClinicName);
	
	MQTTPayloadBuf.payload.cmd2_2_1.wbcPara = MachInfo.paramSet.ucWBC_Gain;
	MQTTPayloadBuf.payload.cmd2_2_1.hgbPara = MachInfo.paramSet.ucHGB_Gain;
	MQTTPayloadBuf.payload.cmd2_2_1.pumpPara = MachInfo.paramSet.ucPump_Scale;
	MQTTPayloadBuf.payload.cmd2_2_1.current535 = MachInfo.paramSet.current535;
	MQTTPayloadBuf.payload.cmd2_2_1.current415 = MachInfo.paramSet.current415;
	MQTTPayloadBuf.payload.cmd2_2_1.outOC = MachInfo.paramSet.ulOutIn_Out_AddStep;
	MQTTPayloadBuf.payload.cmd2_2_1.inOC = MachInfo.paramSet.ulOutIn_In_AddStep;
	MQTTPayloadBuf.payload.cmd2_2_1.gdOC = MachInfo.paramSet.ulClamp_Out_AddStep;
	MQTTPayloadBuf.payload.cmd2_2_1.sfOC = MachInfo.paramSet.ulClamp_In_AddStep;
	MQTTPayloadBuf.payload.cmd2_2_1.presurePara = (int16_t)(MachInfo.paramSet.fPress_Adjust*10);
	MQTTPayloadBuf.payload.cmd2_2_1.wbcAdjust = (uint16_t)(MachInfo.calibration.fWBC*100);
	MQTTPayloadBuf.payload.cmd2_2_1.hgbAdjust = (uint16_t)(MachInfo.calibration.fHGB*100);
	MQTTPayloadBuf.payload.cmd2_2_1.pressureSensor = MachInfo.paramSet.pressSensor;
	MQTTPayloadBuf.payload.cmd2_2_1.catType = CATGlobalStatus.catType;
	MQTTPayloadBuf.payload.cmd2_2_1.downLoadInfoFlag = 0;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		return HAL_ERROR;
	}
	
	return HAL_OK;
}




/***
*发送仪器出厂信息给服务端，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Send_MachineFactoryInfo(void)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
//	uint16_t ReStrIndex = 0;
	uint16_t TotalLen = 0;
//	uint8_t i = 0;
	uint8_t TryCount = 0;
	
	HAL_StatusTypeDef ReturnStatus = HAL_OK;
	
CAT_B_SendMachineFactoryInfoTag:
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0001;
	MQTTPayloadBuf.cmdM = 0x01;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 0;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,2,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
							
				ReturnStatus = CAT_B_DealServiceAckData(0x0001,0x01,0x00,TotalLen,&MQTTPayloadBuf);
				
				if(ReturnStatus == HAL_ERROR){
					if(TryCount >= 2){
						return HAL_ERROR;
					}else{
						goto CAT_B_SendMachineFactoryInfoTag;
					}
				}else if(ReturnStatus == HAL_BUSY){
					return HAL_ERROR;
				}
				
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_TIMEOUT;
			}else{
				goto CAT_B_SendMachineFactoryInfoTag;
			}
		}
	}
}



/***
*发送仪器当前开机信息给服务端，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Send_MachineOpenInfo(void)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
	uint16_t TotalLen = 0;
	uint8_t TryCount = 0;
	extern char MainHardVer[10];
	extern char ConnHardVer[10];
	
CAT_B_SendMachineOpenInfoTag:
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0001;
	MQTTPayloadBuf.cmdM = 0x02;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmd2_2_4);
	
	MQTTPayloadBuf.payload.cmd2_2_4.totalOpenCount = MachInfo.other.totalOpenCount;
	strcpy(MQTTPayloadBuf.payload.cmd2_2_4.position,CATGlobalStatus.position);
	MQTTPayloadBuf.payload.cmd2_2_4.signalStrength = CATGlobalStatus.signalStrength;
	MQTTPayloadBuf.payload.cmd2_2_4.softwareVersion = FULL_VERSION;
	MQTTPayloadBuf.payload.cmd2_2_4.mainVersion = MAIN_VERSION;
	MQTTPayloadBuf.payload.cmd2_2_4.bootloadVersion = *((uint16_t*)BOOTLOAD_VERSION_ADR);
	MQTTPayloadBuf.payload.cmd2_2_4.uiVersion = MachInfo.other.uiVersion;
	strcpy(MQTTPayloadBuf.payload.cmd2_2_4.imei,CATGlobalStatus.imei);
	strcpy(MQTTPayloadBuf.payload.cmd2_2_4.iccid,CATGlobalStatus.iccid);
	MQTTPayloadBuf.payload.cmd2_2_4.catType = CATGlobalStatus.catType;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
						
				if(CAT_B_DealServiceAckData(0x0001,0x02,0x00,TotalLen,&MQTTPayloadBuf) != HAL_OK){
					if(TryCount >= 2){
						return HAL_ERROR;
					}else{
						goto CAT_B_SendMachineOpenInfoTag;
					}
				}
				
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto CAT_B_SendMachineOpenInfoTag;
			}
		}
	}
}









/***
*设备向服务端发送当前信号强度，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Send_SignalStrength(void)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0001;
	MQTTPayloadBuf.cmdM = 0x03;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 1;
	
	MQTTPayloadBuf.payload.u8 = CATGlobalStatus.signalStrength;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}





/***
*设备向服务端发送修改了部分仪器参数指令（指令2.2.9），只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Send_ChangeMachineInfo(uint8_t TypeInfo,uint8_t* Info,uint8_t Len)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0001;
	MQTTPayloadBuf.cmdM = 0x05;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 2+Len;
	
	MQTTPayloadBuf.payload.cmd2_2_8.infoType = TypeInfo;
	
	if(Info != NULL){
		memcpy(MQTTPayloadBuf.payload.cmd2_2_8.info,Info,Len);
	}
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,2,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*设备向服务端发送样本分析数据指令（指令2.6.1），只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Send_SampleInfo(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	uint16_t i = 0;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0005;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmd2_6_1); 
	
	//填充样本分析数据
	MQTTPayloadBuf.payload.cmd2_6_1.sampleSN = WBCHGBTestInfo->ulCurDataSN+1;
	MQTTPayloadBuf.payload.cmd2_6_1.errCode = CountHead->eErrorCode;
	MQTTPayloadBuf.payload.cmd2_6_1.errHideCode = CountHead->eHideErrorCode;
	strcpy(MQTTPayloadBuf.payload.cmd2_6_1.orderNum,WBCHGBTestInfo->orderNum);
	strcpy(MQTTPayloadBuf.payload.cmd2_6_1.patientID,WBCHGBTestInfo->patientID);
	strcpy(MQTTPayloadBuf.payload.cmd2_6_1.name,(char*)WBCHGBTestInfo->ucaName);
	MQTTPayloadBuf.payload.cmd2_6_1.gender = WBCHGBTestInfo->eSex;
	MQTTPayloadBuf.payload.cmd2_6_1.age = WBCHGBTestInfo->ucAge;
	MQTTPayloadBuf.payload.cmd2_6_1.ageUnit = WBCHGBTestInfo->eAgeUnit;
	MQTTPayloadBuf.payload.cmd2_6_1.account = WBCHGBTestInfo->eAccountType;
	MQTTPayloadBuf.payload.cmd2_6_1.timestamp = WBCHGBTestInfo->timestamp;
	MQTTPayloadBuf.payload.cmd2_6_1.refGroups = WBCHGBTestInfo->eReferGroup;
	
	MQTTPayloadBuf.payload.cmd2_6_1.wbc = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC]*100;
	MQTTPayloadBuf.payload.cmd2_6_1.gran = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ]*100;
	MQTTPayloadBuf.payload.cmd2_6_1.mid = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ]*100;
	MQTTPayloadBuf.payload.cmd2_6_1.lym = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ]*100;
	MQTTPayloadBuf.payload.cmd2_6_1.granPrecent = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB]*100;
	MQTTPayloadBuf.payload.cmd2_6_1.midPrecent = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB]*100;
	MQTTPayloadBuf.payload.cmd2_6_1.lymPrecent = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB]*100;
	MQTTPayloadBuf.payload.cmd2_6_1.hgb = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB]*100;
	
	MQTTPayloadBuf.payload.cmd2_6_1.firstX = WBCHGBTestInfo->fLines[0]*100;
	MQTTPayloadBuf.payload.cmd2_6_1.secondX = WBCHGBTestInfo->fLines[1]*100;
	MQTTPayloadBuf.payload.cmd2_6_1.thirdX = WBCHGBTestInfo->fLines[2]*100;
	
	MQTTPayloadBuf.payload.cmd2_6_1.maxFL = WBCHGBTestInfo->fWbcMaxPos*100;
	
	for(i=0;i<256;i++){
		MQTTPayloadBuf.payload.cmd2_6_1.pictValue[i] = WBCHGBTestInfo->fWBC_Histogram[i]*100;
	}
	
	strcpy(MQTTPayloadBuf.payload.cmd2_6_1.wbcErrInfo,(char*)WBCHGBTestInfo->ucaWBC_ErrorPrompt);
	strcpy(MQTTPayloadBuf.payload.cmd2_6_1.hgbErrInfo,(char*)WBCHGBTestInfo->ucaHGB_ErrorPrompt);
	
	MQTTPayloadBuf.payload.cmd2_6_1.wbcAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[0];
	MQTTPayloadBuf.payload.cmd2_6_1.granAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[1];
	MQTTPayloadBuf.payload.cmd2_6_1.midAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[2];
	MQTTPayloadBuf.payload.cmd2_6_1.lymAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[3];
	MQTTPayloadBuf.payload.cmd2_6_1.granPrecentAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[4];
	MQTTPayloadBuf.payload.cmd2_6_1.midPrecentAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[5];
	MQTTPayloadBuf.payload.cmd2_6_1.lymPrecentAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[6];
	MQTTPayloadBuf.payload.cmd2_6_1.hgbAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[7];
	
	MQTTPayloadBuf.payload.cmd2_6_1.alarmFlag = 0;
	for(i=0;i<8;i++){
		if(WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0 && WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0xff){
			MQTTPayloadBuf.payload.cmd2_6_1.alarmFlag = 1;
			break;
		}
	}
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*设备向服务端发送修改样本分析数据指令（指令2.6.2），只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Send_ChangeSampleInfo(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	uint16_t i = 0;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0005;
	MQTTPayloadBuf.cmdM = 0x01;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmd2_6_2); 
	
	//填充样本分析数据
	MQTTPayloadBuf.payload.cmd2_6_2.sampleSN = WBCHGBTestInfo->ulCurDataSN+1;
	strcpy(MQTTPayloadBuf.payload.cmd2_6_2.orderNum,WBCHGBTestInfo->orderNum);
	strcpy(MQTTPayloadBuf.payload.cmd2_6_2.patientID,WBCHGBTestInfo->patientID);
	strcpy(MQTTPayloadBuf.payload.cmd2_6_2.name,(char*)WBCHGBTestInfo->ucaName);
	MQTTPayloadBuf.payload.cmd2_6_2.gender = WBCHGBTestInfo->eSex;
	MQTTPayloadBuf.payload.cmd2_6_2.age = WBCHGBTestInfo->ucAge;
	MQTTPayloadBuf.payload.cmd2_6_2.ageUnit = WBCHGBTestInfo->eAgeUnit;
	MQTTPayloadBuf.payload.cmd2_6_2.account = WBCHGBTestInfo->eAccountType;
	MQTTPayloadBuf.payload.cmd2_6_2.refGroups = WBCHGBTestInfo->eReferGroup;
	
	MQTTPayloadBuf.payload.cmd2_6_2.wbc = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC]*100;
	MQTTPayloadBuf.payload.cmd2_6_2.gran = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ]*100;
	MQTTPayloadBuf.payload.cmd2_6_2.mid = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ]*100;
	MQTTPayloadBuf.payload.cmd2_6_2.lym = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ]*100;
	MQTTPayloadBuf.payload.cmd2_6_2.granPrecent = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB]*100;
	MQTTPayloadBuf.payload.cmd2_6_2.midPrecent = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB]*100;
	MQTTPayloadBuf.payload.cmd2_6_2.lymPrecent = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB]*100;
	MQTTPayloadBuf.payload.cmd2_6_2.hgb = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB]*100;
	
	MQTTPayloadBuf.payload.cmd2_6_2.wbcAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[0];
	MQTTPayloadBuf.payload.cmd2_6_2.granAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[1];
	MQTTPayloadBuf.payload.cmd2_6_2.midAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[2];
	MQTTPayloadBuf.payload.cmd2_6_2.lymAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[3];
	MQTTPayloadBuf.payload.cmd2_6_2.granPrecentAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[4];
	MQTTPayloadBuf.payload.cmd2_6_2.midPrecentAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[5];
	MQTTPayloadBuf.payload.cmd2_6_2.lymPrecentAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[6];
	MQTTPayloadBuf.payload.cmd2_6_2.hgbAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[7];
	
	MQTTPayloadBuf.payload.cmd2_6_2.alarmFlag = 0;
	for(i=0;i<8;i++){
		if(WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0 && WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0xff){
			MQTTPayloadBuf.payload.cmd2_6_2.alarmFlag = 1;
			break;
		}
	}
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}




/***
*设备向服务端发送样本分析数据指令（指令2.7.1），只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Send_SampleLog(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead,uint8_t* pCountLogBuffer, uint16_t usCountLogLen)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	uint16_t i = 0;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0006;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;

	//判断是否是移柯CAT（最大一帧长度4096，所以日志内容小于3950）
	if(CATGlobalStatus.catType == CAT_YK){
		if(usCountLogLen > 3950){
			usCountLogLen = 3950;
		}
	}else{
		if(usCountLogLen > 4096){
			usCountLogLen = 4096;
		}
	}
	
	MQTTPayloadBuf.payloadLen =  usCountLogLen +sizeof(MQTTPayloadBuf.payload.cmd2_7_1);

	
	//填充样本分析数据
	MQTTPayloadBuf.payload.cmd2_7_1.sampleSN = WBCHGBTestInfo->ulCurDataSN+1;
	MQTTPayloadBuf.payload.cmd2_7_1.errCode = CountHead->eErrorCode;
	MQTTPayloadBuf.payload.cmd2_7_1.errHideCode = CountHead->eHideErrorCode;
	MQTTPayloadBuf.payload.cmd2_7_1.errType = CountHead->errType;
	MQTTPayloadBuf.payload.cmd2_7_1.account = WBCHGBTestInfo->eAccountType;
	MQTTPayloadBuf.payload.cmd2_7_1.timestamp = WBCHGBTestInfo->timestamp;
	
	MQTTPayloadBuf.payload.cmd2_7_1.alarmFlag = 0;
	for(i=0;i<8;i++){
		if(WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0 && WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0xff){
			MQTTPayloadBuf.payload.cmd2_7_1.alarmFlag = 1;
			break;
		}
	}
	
	//memcpy(MQTTPayloadBuf.payload.cmd2_7_1.logBuf,CountLog->Str,CountLog->logHead.usCountLogLen);
    memcpy(MQTTPayloadBuf.payload.cmd2_7_1.logBuf, pCountLogBuffer, usCountLogLen);
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*设备向服务端发送样本分析数据指令（指令2.7.2），只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Send_RunLog(RunLog_s* RunLog)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	
	//先判断日志长度是否正常，否则直接返回HAL_OK跳过此条数据即可
	if(RunLog->logHead.logLen > 4100){
		return HAL_OK;
	}
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0006;
	MQTTPayloadBuf.cmdM = 0x01;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = RunLog->logHead.logLen+sizeof(MQTTPayloadBuf.payload.cmd2_7_2);
	
	//填充样本分析数据
	MQTTPayloadBuf.payload.cmd2_7_2.account = RunLog->logHead.eAccountType;
	MQTTPayloadBuf.payload.cmd2_7_2.timestamp = RunLog->logHead.timestamp;
	
	memcpy(MQTTPayloadBuf.payload.cmd2_7_2.logBuf,RunLog->Str,RunLog->logHead.logLen);
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}




/***
*向服务端请求指定的程序文件信息，只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
*参数：
	PublishOrTestFlag：请求的是发布版还是测试版，为0表示请求发布版信息，为1表示请求测试版信息
	SoftwareType：请求的是主程序文件还是引导程序文件，为0表示请求主程序文件，为1表示请求引导程序文件
***/
HAL_StatusTypeDef CAT_B_Send_RespondSoftwareInfo(uint8_t PublishOrTestFlag,uint8_t SoftwareType)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
//	uint16_t ReStrIndex = 0;
	uint16_t TotalLen = 0;
//	uint8_t i = 0;
	uint8_t TryCount = 0;
	
	//判断参数是否合法
	if(PublishOrTestFlag > 1 || SoftwareType > 1){
		return HAL_ERROR;
	}
	
CAT_B_Send_RespondSoftwareInfoTag:
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0002;
	MQTTPayloadBuf.cmdM = 0x01;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmd2_3_3);
	
	MQTTPayloadBuf.payload.cmd2_3_3.publishOrTestFlag = PublishOrTestFlag;
	MQTTPayloadBuf.payload.cmd2_3_3.softwareType = SoftwareType;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
						
				if(CAT_B_DealServiceAckData(0x0002,0x01,0x00,TotalLen,&MQTTPayloadBuf) != HAL_OK){
					if(TryCount >= 2){
						return HAL_ERROR;
					}else{
						goto CAT_B_Send_RespondSoftwareInfoTag;
					}
				}
				
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto CAT_B_Send_RespondSoftwareInfoTag;
			}
		}
	}
}




/***
*向服务端请求指定版本的UI资源，只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
*参数：
	PublishOrTestFlag：请求的是发布版还是测试版，为0表示请求发布版信息，为1表示请求测试版信息
	UIVersion：请求的UI资源版本号
***/
HAL_StatusTypeDef CAT_B_Send_RespondUIInfo(uint8_t PublishOrTestFlag,uint16_t UIVersion)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
//	uint16_t ReStrIndex = 0;
	uint16_t TotalLen = 0;
//	uint8_t i = 0;
	uint8_t TryCount = 0;
	
	//判断参数是否合法
	if(PublishOrTestFlag > 1){
		return HAL_ERROR;
	}
	
CAT_B_Send_RespondUIInfoTag:
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0002;
	MQTTPayloadBuf.cmdM = 0x02;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmd2_3_5);
	
	MQTTPayloadBuf.payload.cmd2_3_5.publishOrTestFlag = PublishOrTestFlag;
	MQTTPayloadBuf.payload.cmd2_3_5.uiVersion = UIVersion;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
						
				if(CAT_B_DealServiceAckData(0x0002,0x02,0x00,TotalLen,&MQTTPayloadBuf) != HAL_OK){
					if(TryCount >= 2){
						return HAL_ERROR;
					}else{
						goto CAT_B_Send_RespondUIInfoTag;
					}
				}
				
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto CAT_B_Send_RespondUIInfoTag;
			}
		}
	}
}



/***
*设备发送仪器自检状态结果给服务端，指令2.2.9，只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_Send_MachineSelfCheck(uint8_t* Buf,uint8_t Len)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	uint8_t TryCount = 0;
	
	//判断参数是否合法
	if(Len == 0){
		return HAL_ERROR;
	}
	
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0001;
	MQTTPayloadBuf.cmdM = 0x06;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = Len;
	
	memcpy(MQTTPayloadBuf.payload.str,Buf,Len);
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}




/***
*向服务端请求最新的发布版或者测试版的各版本号，只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
*参数：
	PublishOrTestFlag：请求的是发布版还是测试版，为0表示请求发布版信息，为1表示请求测试版信息
	UIVersion：请求的UI资源版本号
***/
HAL_StatusTypeDef CAT_B_Send_RespondAllSoftwareVersion(uint8_t PublishOrTestFlag)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
//	uint16_t ReStrIndex = 0;
	uint16_t TotalLen = 0;
	//uint8_t i = 0;
	uint8_t TryCount = 0;
	
	//判断参数是否合法
	if(PublishOrTestFlag > 1){
		return HAL_ERROR;
	}
	
CAT_B_Send_RespondAllSoftwareVersionTag:
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0002;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 1;
	
	MQTTPayloadBuf.payload.u8 = PublishOrTestFlag;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
						
				if(CAT_B_DealServiceAckData(0x0002,0x00,0x00,TotalLen,&MQTTPayloadBuf) != HAL_OK){
					if(TryCount >= 2){
						return HAL_ERROR;
					}else{
						goto CAT_B_Send_RespondAllSoftwareVersionTag;
					}
				}
				
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto CAT_B_Send_RespondAllSoftwareVersionTag;
			}
		}
	}
}



/***
*向服务端请求指定患者编号的患者信息，只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
*参数：
	PatientID：患者编号
***/
HAL_StatusTypeDef CAT_B_Send_RequestPatientInfo(char* PatientID)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern UART_HandleTypeDef huart4;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
	uint16_t TotalLen = 0;
	uint8_t TryCount = 0;
	
CAT_B_Send_RequestPatientInfoTag:
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);

	TryCount++;
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0007;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmd2_8_1); 
	
	strcpy(MQTTPayloadBuf.payload.cmd2_8_1.patientID,PatientID);
	MQTTPayloadBuf.payload.cmd2_8_1.catTestType = TEST_TYPE_WBC_HGB;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				
				if(CAT_B_DealServiceAckData(0x0007,0x00,0x00,TotalLen,&MQTTPayloadBuf) != HAL_OK){
					if(TryCount >= 2){
						return HAL_ERROR;
					}else{
						goto CAT_B_Send_RequestPatientInfoTag;
					}
				}
				
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				//网络异常
				return HAL_ERROR;
			}else{
				goto CAT_B_Send_RequestPatientInfoTag;
			}
		}
	}
}







//===FTP相关===为了防止流量泄漏，只要函数返回ERROR，则挂起任务=================================
/***
*设置FTP服务器端口号
***/
static HAL_StatusTypeDef _CAT_FTPSetPort(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	uint16_t ReStrIndex = 0;
	
	if(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		return HAL_ERROR;
	}
	
CAT_FTPSetPortTag:
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CFTPPORT=%u\r",FTP_SERVICE_PORT);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,5000ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//判断收到的响应是否正确
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//超时，与CAT通讯失败
			if(TryCount == 3){
				//发送全局事件
				return HAL_TIMEOUT;
			}
			
			goto CAT_FTPSetPortTag;
		}
	}
}



/***
*设置FTP服务器IP或者域名
***/
static HAL_StatusTypeDef _CAT_FTPSetIP(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	uint16_t ReStrIndex = 0;
	
	if(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		return HAL_ERROR;
	}
	
	
	
CAT_FTPSetIPTag:
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CFTPSERV=\"%s\"\r",FTP_SERVICE_IP);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,5000ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//判断收到的响应是否正确
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//超时，与CAT通讯失败
			if(TryCount == 3){
				//发送全局事件
				return HAL_TIMEOUT;
			}
			
			goto CAT_FTPSetIPTag;
		}
	}
}



/***
*设置FTP服务器登录名
***/
static HAL_StatusTypeDef _CAT_FTPSetLoginName(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	uint16_t ReStrIndex = 0;
	
	if(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		return HAL_ERROR;
	}
	
CAT_FTPSetLoginNameTag:
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CFTPUN=\"%s\"\r",FTP_SERVICE_USER_NAME);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,5000ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//判断收到的响应是否正确
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//超时，与CAT通讯失败
			if(TryCount == 3){
				//发送全局事件
				return HAL_TIMEOUT;
			}
			
			goto CAT_FTPSetLoginNameTag;
		}
	}
}



/***
*设置FTP服务器登录密码
***/
static HAL_StatusTypeDef _CAT_FTPSetLoginPsw(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	uint16_t ReStrIndex = 0;
	
	if(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		return HAL_ERROR;
	}
	
CAT_FTPSetLoginPswTag:
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CFTPPW=\"%s\"\r",FTP_SERVICE_PASSWORD);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,5000ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//判断收到的响应是否正确
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//超时，与CAT通讯失败
			if(TryCount == 3){
				//发送全局事件
				return HAL_TIMEOUT;
			}
			
			goto CAT_FTPSetLoginPswTag;
		}
	}
}



/***
*移柯CAT配置并启动FTP文件下载
***/
static HAL_StatusTypeDef _YK_CAT_FTPGetFile(uint8_t* Buf,uint32_t* BufLen,char* Path,uint32_t FileOffset)
{
	extern MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern volatile CAT_ReceBuf_s CATReceBuf;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	osStatus_t xReturn = osOK;
	uint16_t ReStrIndex = 0;
	uint16_t TempLen = 0;
	uint8_t i = 0;
	uint8_t TryCount = 0;
	uint32_t TotalLen = 0;
	uint32_t TimeoutCount = 0;
	
	
	if(CATGlobalStatus.flag.ftpConn != 1){
		return HAL_ERROR;
	}
	
YK_CAT_FTPGetFile:
	TryCount++;
	*BufLen = 0;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	CATSendCmdBuf.writeIndex = 0;
	
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CFTPGET=\"%s\",%u\r",Path,FileOffset);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			while(1){
				ReStrIndex = _CAT_MyStrCmp(0,"+CFTPGET:DATA,");
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					TimeoutCount = 0;
					
					if(_CAT_MyStrCmp(ReStrIndex,"\r\n") != CAT_RECE_BUF_NO_STR){
						ReStrIndex = (ReStrIndex+13)&CAT_RECE_BUF_SIZE;
						
						TempLen=0;
						
						//获取总内容长度值
						for(i=0;i<8;i++){
							ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
							
							if(CATReceBuf.buf[ReStrIndex] == '\r'){
								break;
							}
							
							TempLen = TempLen*10+(CATReceBuf.buf[ReStrIndex]-'0');
						}
						
						
						
						//复制内容到缓存中
						while(_CAT_MyStrCpy((ReStrIndex+2)&CAT_RECE_BUF_SIZE,Buf+(*BufLen),TempLen) != HAL_OK){
							TimeoutCount++;
						
							//防止死锁
							if(TimeoutCount > 100000000){
								osSemaphoreRelease(CATSemphrCountTxHandle);
								return HAL_ERROR;
							}
						}
						
						*BufLen += TempLen;
						CATReceBuf.readIndex = (ReStrIndex+2+TempLen)&CAT_RECE_BUF_SIZE;
					}
				}else if(_CAT_MyStrCmp(0,"+CFTPGET:SUCCESS,") != CAT_RECE_BUF_NO_STR){
					break;
				}else if(_CAT_MyStrCmp(0,"+CFTPGET:FAIL") != CAT_RECE_BUF_NO_STR){
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_ERROR;
				}else{
					TimeoutCount++;
					
					if(TimeoutCount > 100000000){
						osSemaphoreRelease(CATSemphrCountTxHandle);
						return HAL_ERROR;
					}
				}
			}
			
			ReStrIndex = _CAT_MyStrCmp(0,"+CFTPGET:SUCCESS,");
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				ReStrIndex = (ReStrIndex+16)&CAT_RECE_BUF_SIZE;
						
				TotalLen = 0;
				
				//获取总内容长度值
				for(i=0;i<20;i++){
					ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
					
					if(CATReceBuf.buf[ReStrIndex] == '\r'){
						break;
					}
					
					TotalLen = TotalLen*10+(CATReceBuf.buf[ReStrIndex]-'0');
				}
				
				//判断总长度是否相等
				if(*BufLen == TotalLen){
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}else{
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_ERROR;
				}
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto YK_CAT_FTPGetFile;
			}
		}
	}
}




/***
*中移物联CAT模块配置FTP服务端参数
***/
static HAL_StatusTypeDef _ZY_CAT_FTPSetServerPara(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	uint16_t ReStrIndex = 0;
	
	if(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		return HAL_ERROR;
	}
	
ZY_CAT_FTPSetServerPara:
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT^FTPOPEN=\"%s:%u\",\"%s\",\"%s\",1,180,1\r",FTP_SERVICE_IP,FTP_SERVICE_PORT,FTP_SERVICE_USER_NAME,FTP_SERVICE_PASSWORD);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,5000ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//判断收到的响应是否正确
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//超时，与CAT通讯失败
			if(TryCount == 3){
				//发送全局事件
				return HAL_TIMEOUT;
			}
			
			goto ZY_CAT_FTPSetServerPara;
		}
	}
}




/***
*中移物联CAT模块配置FTP下载文件参数
***/
static HAL_StatusTypeDef _ZY_CAT_FTPSetFilePara(char* Path,uint32_t FileOffset)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	uint16_t TempLen = 0;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	uint16_t ReStrIndex = 0;
	
	if(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		return HAL_ERROR;
	}
	
ZY_CAT_FTPSetFilePara:
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT^FTPGETSET=\"%s\",%u,0,0\r",Path,FileOffset);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//等待空闲中断处理中释放的二值同步信号量,5000ms超时
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		
		//判断是否收到响应
		if(xReturn == osOK){
			//判断收到的响应是否正确
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//超时，与CAT通讯失败
			if(TryCount == 3){
				//发送全局事件
				return HAL_TIMEOUT;
			}
			
			goto ZY_CAT_FTPSetFilePara;
		}
	}
}



/***
*中移CAT配置并启动FTP文件下载
***/
static HAL_StatusTypeDef _ZY_CAT_FTPGetFile(FTPDownloadData_s* FTPDownLoadData,char* Path,uint32_t FileOffset)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern volatile CAT_ReceBuf_s CATReceBuf;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	osStatus_t xReturn = osOK;
	uint16_t ReStrIndex = 0;
	uint16_t TempLen = 0;
	uint8_t i = 0;
	uint8_t TryCount = 0;
	uint32_t TimeoutCount = 0;
	
	char* TempStr = NULL;
	
	
	if(CATGlobalStatus.flag.ftpConn != 1){
		return HAL_ERROR;
	}
	
	//配置下载文件参数
	if(_ZY_CAT_FTPSetFilePara(Path,FileOffset) != HAL_OK){
		return HAL_ERROR;
	}
	
ZY_CAT_FTPGetFileTag:
	TryCount++;
	FTPDownLoadData->totalLen = 0;
	FTPDownLoadData->tempTotalLen = 0;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT^FTPGET=1\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	TimeoutCount = 0;
	
	while(1){
		//等待服务端返回消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		if(xReturn == osOK){
			//判断下载是否正常启动
			ReStrIndex = _CAT_MyStrCmp(0,"^FTPGET:1,1");
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				CATReceBuf.readIndex = ReStrIndex;
				
				//先将数据全部下载到FTP下载结构体中的临时BUF中保存起来，便于后续提取数据
				while(1){
					if(CATReceBuf.readIndex == CATReceBuf.writeIndex){
						TimeoutCount++;
						if(TimeoutCount > 10000000){
							break;
						}
					}else{
						TimeoutCount = 0;
						FTPDownLoadData->tempData[FTPDownLoadData->tempTotalLen++] = CATReceBuf.buf[CATReceBuf.readIndex];
						CATReceBuf.readIndex = (CATReceBuf.readIndex+1)&CAT_RECE_BUF_SIZE;
						
						//判断下载的数据长度是否已经超限了
						if(FTPDownLoadData->tempTotalLen > FTP_TEMP_DOWNLOAD_BUF_LEN){
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}
					}
				}
				
				//从下载的数据中提取有用数据
				TempStr = (char*)FTPDownLoadData->tempData;
				while(1){
					TempStr = strstr(TempStr,"^FTPGET:2,");
					
					if(TempStr != NULL){
						TempLen = 0;
						TempStr += 10;
						
						//获取总内容长度值
						for(i=0;i<8;i++){
							if(*TempStr == '\r'){
								if(*(TempStr+1) == '\n'){
									break;
								}
							}
							
							TempLen = TempLen*10+(*TempStr-'0');
							TempStr++;
						}
						
						if(i == 8){
							//数据下载异常，退出
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}
						
						//判断数据长度
						if(TempLen == 0){
							//全部数据提取完成，正常结束
							break;
						}else if(TempLen <= 4096){
							//拷贝数据
							TempStr += 2;
							
							while(TempLen--){
								FTPDownLoadData->data[FTPDownLoadData->totalLen++] = *TempStr++;
							}
						}else{
							//数据下载异常，退出
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}
					}else{
						//没有正常收到结束指令^FTPGET:2,0，所以判断为数据异常
						osSemaphoreRelease(CATSemphrCountTxHandle);
						return HAL_ERROR;
					}
				}
				
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"^FTPGET:1,0") != CAT_RECE_BUF_NO_STR){
				//启动FTP下载失败
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_ERROR;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto ZY_CAT_FTPGetFileTag;
			}
		}
	}
}



/***
*中移CAT停止FTP文件下载
***/
static HAL_StatusTypeDef ZY_CAT_FTPStopDownload(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern volatile CAT_ReceBuf_s CATReceBuf;
	CAT_SendCmdBuf_s CATSendCmdBuf;
	uint16_t TempLen = 0;
	
	
	if(CATGlobalStatus.flag.ftpConn != 1){
		return HAL_ERROR;
	}
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT^FTPGET=2,0\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	
	return HAL_OK;
}





/***
*连接FTP服务器，返回不是HAL_OK时，则将调用任务挂起
***/
HAL_StatusTypeDef CAT_ConnFTP(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	
	if(CATGlobalStatus.flag.ftpConn == 1){
		return HAL_OK;
	}
	
	//根据不同的CAT类型执行不同的指令
	if(CATGlobalStatus.catType == CAT_YK){
		if(_CAT_FTPSetPort() != HAL_OK){
			return HAL_ERROR;
		}
		
		if(_CAT_FTPSetIP() != HAL_OK){
			return HAL_ERROR;
		}
		
		if(_CAT_FTPSetLoginName() != HAL_OK){
			return HAL_ERROR;
		}
		
		if(_CAT_FTPSetLoginPsw() != HAL_OK){
			return HAL_ERROR;
		}
	}else{
		if(_ZY_CAT_FTPSetServerPara() != HAL_OK){
			return HAL_ERROR;
		}
	}
	
	CATGlobalStatus.flag.ftpConn = 1;
	
	return HAL_OK;
}



/***
*下载指定路径下的文件
*参数：
	Buf：下载的文件保存缓存
	BufLen：下载的文件总大小
	Path：从FTP中的下载路径
	FileOffset：指定从文件开始下载的偏移量，为0时表示从文件开头处下载
返回不是HAL_OK时，则将调用任务挂起
***/
HAL_StatusTypeDef CAT_FTPGetFile(FTPDownloadData_s* FTPDownLoadData,char* Path,uint32_t FileOffset)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	HAL_StatusTypeDef xReturn = HAL_OK;
	
	//根据不同CAT类型执行不同的指令
	if(CATGlobalStatus.catType == CAT_YK){
		xReturn =  _YK_CAT_FTPGetFile(FTPDownLoadData->data,&FTPDownLoadData->totalLen,Path,FileOffset);
	}else{
		xReturn = _ZY_CAT_FTPGetFile(FTPDownLoadData,Path,FileOffset);
		if(xReturn != HAL_OK){
			//FTP下载失败时，需要发送停止下载指令
			ZY_CAT_FTPStopDownload();
		}
	}
	
	return xReturn;
}












//***与第三方服务端进行数据通讯区*************************************************************
/***
*发送仪器当前开机信息给第三方服务端，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Other_Send_MachineOpenInfo(void)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA000;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmdA_2_2_1);
	
	MQTTPayloadBuf.payload.cmdA_2_2_1.totalOpenCount = MachInfo.other.totalOpenCount;
	strcpy(MQTTPayloadBuf.payload.cmdA_2_2_1.position,CATGlobalStatus.position);
	MQTTPayloadBuf.payload.cmdA_2_2_1.signalStrength = CATGlobalStatus.signalStrength;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*设备向第三方服务端发送当前信号强度，只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Other_Send_SignalStrength(void)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA000;
	MQTTPayloadBuf.cmdM = 0x01;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 1;
	
	MQTTPayloadBuf.payload.u8 = CATGlobalStatus.signalStrength;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}




/***
*设备发送仪器自检状态结果给第三方服务端，只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_Other_Send_MachineSelfCheck(uint8_t* Buf,uint8_t Len)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	uint8_t TryCount = 0;
	
	//判断参数是否合法
	if(Len == 0){
		return HAL_ERROR;
	}
	
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA000;
	MQTTPayloadBuf.cmdM = 0x02;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = Len;
	
	memcpy(MQTTPayloadBuf.payload.str,Buf,Len);
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*设备向第三方服务端发送样本分析故障日志，只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Other_Send_SampleLog(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	uint16_t i = 0;
//	uint8_t TryCount = 0;
//	osStatus_t xReturn = osOK;
//	uint16_t TotalLen = 0;
	
//CAT_B_Other_Send_SampleLogTag:
//	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA002;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmdA_2_4_1);
	
	//填充样本分析数据
	MQTTPayloadBuf.payload.cmdA_2_4_1.sampleSN = WBCHGBTestInfo->ulCurDataSN+1;
	MQTTPayloadBuf.payload.cmdA_2_4_1.errCode = CountHead->eErrorCode;
	MQTTPayloadBuf.payload.cmdA_2_4_1.errHideCode = CountHead->eHideErrorCode;
	MQTTPayloadBuf.payload.cmdA_2_4_1.errType = CountHead->errType;
	MQTTPayloadBuf.payload.cmdA_2_4_1.account = WBCHGBTestInfo->eAccountType;
	MQTTPayloadBuf.payload.cmdA_2_4_1.timestamp = WBCHGBTestInfo->timestamp;
	MQTTPayloadBuf.payload.cmdA_2_4_1.catTestType = TEST_TYPE_WBC_HGB;
	
	MQTTPayloadBuf.payload.cmdA_2_4_1.alarmFlag = 0;
	for(i=0;i<8;i++){
		if(WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0 && WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0xff){
			MQTTPayloadBuf.payload.cmdA_2_4_1.alarmFlag = 1;
			break;
		}
	}
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	MachRunPara.currSN.logSN = WBCHGBTestInfo->ulCurDataSN+1;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
	
//	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
//	osSemaphoreRelease(CATSemphrBinRxHandle);
//	
//	while(1){
//		//等待服务端返回消息
//		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
//		if(xReturn == osOK){
//			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
//				osSemaphoreRelease(CATSemphrCountTxHandle);
//				
//				if(CAT_B_DealServiceAckData(0xA002,0x00,0x00,TotalLen,&MQTTPayloadBuf) != HAL_OK){
//					if(TryCount >= 2){
//						return HAL_ERROR;
//					}else{
//						goto CAT_B_Other_Send_SampleLogTag;
//					}
//				}
//				
//				return HAL_OK;
//			}
//		}else{
//			osSemaphoreRelease(CATSemphrCountTxHandle);
//			
//			if(TryCount >= 2){
//				return HAL_ERROR;
//			}else{
//				goto CAT_B_Other_Send_SampleLogTag;
//			}
//		}
//	}
}




/***
*设备向服务端发送样本分析数据指令，只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Other_Send_SampleInfo(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	uint16_t i = 0;
//	uint8_t TryCount = 0;
//	osStatus_t xReturn = osOK;
//	uint16_t TotalLen = 0;
	
//CAT_B_Other_Send_SampleInfoTag:
//	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA001;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmdA_2_3_1); 
	
	//填充样本分析数据
	strcpy(MQTTPayloadBuf.payload.cmdA_2_3_1.orderNum,(char*)WBCHGBTestInfo->orderNum);
	MQTTPayloadBuf.payload.cmdA_2_3_1.catTestType = TEST_TYPE_WBC_HGB;
	MQTTPayloadBuf.payload.cmdA_2_3_1.sampleSN = WBCHGBTestInfo->ulCurDataSN+1;
	MQTTPayloadBuf.payload.cmdA_2_3_1.errCode = CountHead->eErrorCode;
	MQTTPayloadBuf.payload.cmdA_2_3_1.errHideCode = CountHead->eHideErrorCode;
	strcpy(MQTTPayloadBuf.payload.cmdA_2_3_1.patientID,(char*)WBCHGBTestInfo->patientID);
	strcpy(MQTTPayloadBuf.payload.cmdA_2_3_1.name,(char*)WBCHGBTestInfo->ucaName);
	MQTTPayloadBuf.payload.cmdA_2_3_1.gender = WBCHGBTestInfo->eSex;
	MQTTPayloadBuf.payload.cmdA_2_3_1.age = WBCHGBTestInfo->ucAge;
	MQTTPayloadBuf.payload.cmdA_2_3_1.ageUnit = WBCHGBTestInfo->eAgeUnit;
	MQTTPayloadBuf.payload.cmdA_2_3_1.account = WBCHGBTestInfo->eAccountType;
	MQTTPayloadBuf.payload.cmdA_2_3_1.timestamp = WBCHGBTestInfo->timestamp;
	MQTTPayloadBuf.payload.cmdA_2_3_1.refGroups = WBCHGBTestInfo->eReferGroup;
	
	MQTTPayloadBuf.payload.cmdA_2_3_1.wbc = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC]*100;
	MQTTPayloadBuf.payload.cmdA_2_3_1.gran = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ]*100;
	MQTTPayloadBuf.payload.cmdA_2_3_1.mid = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ]*100;
	MQTTPayloadBuf.payload.cmdA_2_3_1.lym = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ]*100;
	MQTTPayloadBuf.payload.cmdA_2_3_1.granPrecent = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB]*100;
	MQTTPayloadBuf.payload.cmdA_2_3_1.midPrecent = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB]*100;
	MQTTPayloadBuf.payload.cmdA_2_3_1.lymPrecent = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB]*100;
	MQTTPayloadBuf.payload.cmdA_2_3_1.hgb = WBCHGBTestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB]*100;
	
	MQTTPayloadBuf.payload.cmdA_2_3_1.firstX = WBCHGBTestInfo->fLines[0]*100;
	MQTTPayloadBuf.payload.cmdA_2_3_1.secondX = WBCHGBTestInfo->fLines[1]*100;
	MQTTPayloadBuf.payload.cmdA_2_3_1.thirdX = WBCHGBTestInfo->fLines[2]*100;
	
	MQTTPayloadBuf.payload.cmdA_2_3_1.maxFL = WBCHGBTestInfo->fWbcMaxPos*100;
	
	for(i=0;i<256;i++){
		MQTTPayloadBuf.payload.cmdA_2_3_1.pictValue[i] = WBCHGBTestInfo->fWBC_Histogram[i]*100;
	}
	
	strcpy(MQTTPayloadBuf.payload.cmdA_2_3_1.wbcErrInfo,(char*)WBCHGBTestInfo->ucaWBC_ErrorPrompt);
	strcpy(MQTTPayloadBuf.payload.cmdA_2_3_1.hgbErrInfo,(char*)WBCHGBTestInfo->ucaHGB_ErrorPrompt);
	
	MQTTPayloadBuf.payload.cmdA_2_3_1.wbcAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[0];
	MQTTPayloadBuf.payload.cmdA_2_3_1.granAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[1];
	MQTTPayloadBuf.payload.cmdA_2_3_1.midAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[2];
	MQTTPayloadBuf.payload.cmdA_2_3_1.lymAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[3];
	MQTTPayloadBuf.payload.cmdA_2_3_1.granPrecentAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[4];
	MQTTPayloadBuf.payload.cmdA_2_3_1.midPrecentAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[5];
	MQTTPayloadBuf.payload.cmdA_2_3_1.lymPrecentAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[6];
	MQTTPayloadBuf.payload.cmdA_2_3_1.hgbAlarmFlag = WBCHGBTestInfo->ucaWBCHGB_RstAlert[7];
	
	MQTTPayloadBuf.payload.cmdA_2_3_1.alarmFlag = 0;
	for(i=0;i<8;i++){
		if(WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0 && WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0xff){
			MQTTPayloadBuf.payload.cmdA_2_3_1.alarmFlag = 1;
			break;
		}
	}
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	MachRunPara.currSN.sampleSN = WBCHGBTestInfo->ulCurDataSN+1;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
	
//	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
//	osSemaphoreRelease(CATSemphrBinRxHandle);
//	
//	while(1){
//		//等待服务端返回消息
//		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
//		if(xReturn == osOK){
//			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
//				osSemaphoreRelease(CATSemphrCountTxHandle);
//				
//				if(CAT_B_DealServiceAckData(0xA001,0x00,0x00,TotalLen,&MQTTPayloadBuf) != HAL_OK){
//					if(TryCount >= 2){
//						return HAL_ERROR;
//					}else{
//						goto CAT_B_Other_Send_SampleInfoTag;
//					}
//				}
//				
//				return HAL_OK;
//			}
//		}else{
//			osSemaphoreRelease(CATSemphrCountTxHandle);
//			
//			if(TryCount >= 2){
//				return HAL_ERROR;
//			}else{
//				goto CAT_B_Other_Send_SampleInfoTag;
//			}
//		}
//	}
}





/***
*向服务端请求患者信息，只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
*参数：
	PageNum：要请求的患者信息页码
***/
HAL_StatusTypeDef CAT_B_Other_Send_RequestPatientInfo(uint8_t PageNum)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern UART_HandleTypeDef huart4;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
	uint16_t TotalLen = 0;
	uint8_t TryCount = 0;
	
CAT_B_Other_Send_RequestPatientInfoTag:
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);

	TryCount++;
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA005;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmdA_2_5_1); 
	
	MQTTPayloadBuf.payload.cmdA_2_5_1.catTestType = TEST_TYPE_WBC_HGB;
	MQTTPayloadBuf.payload.cmdA_2_5_1.pageNum = PageNum;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				
				if(CAT_B_DealServiceAckData(0xA005,0x00,0x00,TotalLen,&MQTTPayloadBuf) != HAL_OK){
					if(TryCount >= 2){
						return HAL_ERROR;
					}else{
						goto CAT_B_Other_Send_RequestPatientInfoTag;
					}
				}
				
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto CAT_B_Other_Send_RequestPatientInfoTag;
			}
		}
	}
}




/***
*询问第三方服务端是否有数据发送给设备，切换控制权给服务端。只要返回值不为HAL_OK时表示上一级调用者
需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Other_IsServiceWantToSendData(void)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
	uint16_t TotalLen = 0;
	uint8_t TryCount = 0;
	
CAT_B_Other_IsServiceWantToSendDataTag:
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA004;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 0;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;

	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}

	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);

		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
							
				//处理服务端发送的请求消息
				if(_CAT_B_DealServiceRequestData(&TotalLen,&MQTTPayloadBuf) != HAL_OK){
					if(TryCount >= 2){
						return HAL_ERROR;
					}else{
						goto CAT_B_Other_IsServiceWantToSendDataTag;
					}
				}
				
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto CAT_B_Other_IsServiceWantToSendDataTag;
			}
		}
	}
}




/***
*设备响应第三方服务端请求更改仪器参数设置的指令
*参数：
	InfoType：表示参数类型
	Flag：为1表示更改成功，为0表示更改失败
*返回值：返回HAL_OK表示完成响应，HAL_ERROR表示响应失败
***/
HAL_StatusTypeDef CAT_B_Other_Respond_ChangeSetInfo(uint16_t InfoType,uint8_t Flag,uint16_t* TotalLen,MQTT_PayloadBuf_s* MQTTPayloadBuf)
{
	extern MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern UART_HandleTypeDef huart4;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	osStatus_t xReturn = osOK;
//	uint8_t i = 0;
	uint8_t TryCount = 0;
	
CAT_B_Other_Respond_ChangeSetInfoTag:
	TryCount++;
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf->cmdH = 0xA004;
	MQTTPayloadBuf->cmdM = 0x01;
	MQTTPayloadBuf->cmdL = 0x01;
	MQTTPayloadBuf->cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf->packetNum = 0;
	MQTTPayloadBuf->payloadLen = sizeof(MQTTPayloadBuf->payload.cmdA_2_6_3);
	
	MQTTPayloadBuf->payload.cmdA_2_6_3.infoType = InfoType;
	MQTTPayloadBuf->payload.cmdA_2_6_3.result = Flag;
	
	MQTTPayloadBuf->crc16 = CRC16((uint8_t*)&MQTTPayloadBuf->payloadLen,MQTTPayloadBuf->payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf->payloadLen+14,(uint8_t*)MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//这里手动释放一次接收信号量，以便可以提前先检查以便是否已经接收到服务端对应的响应了
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//等待服务端返回下一个操作消息
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)MQTTPayloadBuf,TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto CAT_B_Other_Respond_ChangeSetInfoTag;
			}
		}
	}
}



/***
*设备向第三方服务端发送即将开始测试的患者订单编号，只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Other_Send_ReadyTestOrderNum(char* OrderNum)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA005;
	MQTTPayloadBuf.cmdM = 0x01;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmdA_2_5_3);
	
	//填充样本分析数据
	MQTTPayloadBuf.payload.cmdA_2_5_3.catTestType = TEST_TYPE_WBC_HGB;
	strcpy(MQTTPayloadBuf.payload.cmdA_2_5_3.orderNum,OrderNum);
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*设备向第三方服务端发送网络中断与恢复的情况，只要返回值不为HAL_OK时表示上一级调用者需要将任务挂起
***/
HAL_StatusTypeDef CAT_B_Other_Send_NetStatus(void)
{
	extern MachInfo_s MachInfo;
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	
	//获取计数信号量，防止数据被覆盖，死等
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//复位MQTTPayloadBuf内存，防止出现异常数据
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA000;
	MQTTPayloadBuf.cmdM = 0x03;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmdA_2_2_4);
	
	//填充样本分析数据
	MQTTPayloadBuf.payload.cmdA_2_2_4.interruptSignalStrength = MachRunPara.netStatus.interruptSignalStrength;
	MQTTPayloadBuf.payload.cmdA_2_2_4.interruptTimestamp = MachRunPara.netStatus.interruptTimestamp;
	MQTTPayloadBuf.payload.cmdA_2_2_4.recoverySignalStrength = MachRunPara.netStatus.recoverySignalStrength;
	MQTTPayloadBuf.payload.cmdA_2_2_4.recoveryTimestamp = MachRunPara.netStatus.recoveryTimestamp;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//主题字符串
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//清除接收缓冲的干扰因素
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//发布消息
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}






