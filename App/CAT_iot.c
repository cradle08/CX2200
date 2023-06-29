/************************************************************
*CATģ��ײ��������
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


/*�궨����*********************************************************/
#define CAT_CMD_SEND_BUF_SIZE								256						//���巢�͸�CATģ���ָ�����ݻ�������С
#define CAT_RECE_BUF_NO_STR									0xFFFF					//����CAT���ջ���Buf��û��ָ�����ַ����ı�־












/***
*���巢�͸�CATģ�����������ṹ��
***/
typedef struct _CAT_SendCmdBuf_s{
	char buf[CAT_CMD_SEND_BUF_SIZE];
	uint16_t writeIndex;
}CAT_SendCmdBuf_s;








/*ȫ�ֱ���������**************************************************/
CATGlobalStatus_s CATGlobalStatus = {0};								//CATģ��ȫ��״̬����������
MQTT_PayloadBuf_s MQTTPayloadBuf = {0};									//MQTT�������߽��յ���Ϣ����buf
volatile CAT_ReceBuf_s CATReceBuf = {0};								//�����CAT�н������ݵ�ѭ������










/***
*�Զ����ַ����ȽϺ��������ڸ�CATģ���ATָ��ʹ��
*������
	StartIndex��ָ�����ĸ�������ʼ�Ƚ��ַ�����Ϊ0��ʾreadIndex����ʼ
	Str��Ҫ�Ƚϵ��ַ���
*����ֵ��������ָ���ַ���ʱ���򷵻س��ָ��ַ�������������������������ʱ�򷵻�0xFFFF
***/
static uint16_t _CAT_MyStrCmp(uint16_t StartIndex,char* Str)
{
	extern volatile CAT_ReceBuf_s CATReceBuf;
	uint16_t TempIndex = 0;
	char* TempStr = Str;
	uint16_t ReturnIndex = 0;
	
	//�жϲ����Ϸ���
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
		//���ҵ���һ���ַ���ͬ��λ�ô�
		if(CATReceBuf.buf[TempIndex] == *TempStr){
			ReturnIndex = TempIndex;
			
			//��֤�����ַ��Ƿ�һһ��Ӧ
			while(*TempStr != '\0' && CATReceBuf.buf[TempIndex] == *TempStr && TempIndex != CATReceBuf.writeIndex){
				TempIndex = (TempIndex+1)&CAT_RECE_BUF_SIZE;
				TempStr++;
			}
			
			if(*TempStr == '\0'){
				//�ҵ�ָ���ַ���
				return ReturnIndex;
			}else{
				//�˴�û���ҵ�ָ���ַ�����������һ��ѭ��
				TempStr = Str;
			}
		}else{
			TempIndex = (TempIndex+1)&CAT_RECE_BUF_SIZE;
		}
	}
	
	return CAT_RECE_BUF_NO_STR;
}



/***
*�Զ����ַ�����ȡ���Ⱥ��������ڸ�CATģ���ATָ��ʹ��
*����ֵ�����ص�ǰ���ջ���������յ������ݳ���ֵ
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
*�Զ����ȡ��ָ��������ʼ����ʣ�����ݳ��Ⱥ��������ڸ�CATģ���ATָ��ʹ��
*����ֵ�����ص�ǰ���ջ���������յ���ʣ�����ݳ���ֵ
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
*�Զ����ַ������ƺ��������ڸ�CATģ���ATָ��ʹ��
*������
	Index����ʾ�Ӹ���������ʼ�������ݵ�ָ��λ�ô�
	Buf��ָ������Ļ�����
	Len��Ҫ���Ƶĳ���
*����ֵ�������ɹ����򷵻�HAL_OK������HAL_ERROR���ʾ����ʧ��
***/
static HAL_StatusTypeDef _CAT_MyStrCpy(uint16_t Index,uint8_t* Buf,uint16_t Len)
{
	extern volatile CAT_ReceBuf_s CATReceBuf;
	
	//�жϲ����Ƿ�Ϸ�
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
*��ѯ��Ӫ��
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
	
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+COPS?\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
_CAT_CheckOperatorTag:
	TryCount++;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�ж��յ�����Ӧ�Ƿ���ȷ
			if(_CAT_MyStrCmp(0,"46011") != CAT_RECE_BUF_NO_STR){				//���ſ�
				CATGlobalStatus.operatorNum = 0;
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"46000") != CAT_RECE_BUF_NO_STR){			//�ƶ���
				CATGlobalStatus.operatorNum = 1;
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"+COPS: 0\r\n") != CAT_RECE_BUF_NO_STR){
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SIM_UNACTIVE_ERR);
				
				CATGlobalStatus.operatorNum = 0xFF;
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_ERROR;
			}
		}else{
			//��ʱ����CATͨѶʧ��
			if(TryCount >= 10){
				CATGlobalStatus.flag.conn = 0;
			
				//����ȫ���¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto _CAT_CheckOperatorTag;
			}
		}
	}
}



/***
*��ѯIMEI��
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	//����CAT���ͺŲ�ִͬ�в�ͬ��ָ��
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CGSN\r");
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CGSN=1\r");
	}
	
	CATSendCmdBuf.writeIndex += TempLen;
	
_CAT_CheckIMEITag:
	TryCount++;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//���ݲ�ͬ��CATģ������ִ�в�ͬ�Ķ���
			if(CATGlobalStatus.catType == CAT_YK){
				ReStrIndex = _CAT_MyStrCmp(0,"\r\n\"");
			
				//�ж��յ�����Ӧ�Ƿ���ȷ
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					if(_CAT_MyStrCmp(ReStrIndex,"\"\r\n") != CAT_RECE_BUF_NO_STR){
						if(_CAT_MyStrCpy((ReStrIndex+3)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.imei,15) == HAL_OK){
							CATGlobalStatus.imei[15] = '\0';
							
							//��ָ��ִ����ϣ��ͷż����ź���
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}
				}
			}else{	//����CAT
				ReStrIndex = _CAT_MyStrCmp(0,"+CGSN: ");
			
				//�ж��յ�����Ӧ�Ƿ���ȷ
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
						if(_CAT_MyStrCpy((ReStrIndex+7)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.imei,15) == HAL_OK){
							CATGlobalStatus.imei[15] = '\0';
							
							//��ָ��ִ����ϣ��ͷż����ź���
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}
				}
			}
		}else{
			if(TryCount >= 5){
				//��ʱ����CATͨѶʧ��
				CATGlobalStatus.flag.conn = 0;
			
				//����ȫ���¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				
				return HAL_TIMEOUT;
			}else{
				goto _CAT_CheckIMEITag;
			}
		}
	}
}



/***
*��ѯICCID��
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
	
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+ICCID\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
_CAT_CheckICCIDTag:
	TryCount++;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			ReStrIndex = _CAT_MyStrCmp(0,"+ICCID:");
			
			//�ж��յ�����Ӧ�Ƿ���ȷ
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
					if(CATGlobalStatus.catType == CAT_YK){
						if(_CAT_MyStrCpy((ReStrIndex+7)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.iccid,20) == HAL_OK){
							CATGlobalStatus.iccid[20] = '\0';
							
							CATGlobalStatus.flag.sim = 1;
							
							//��ָ��ִ����ϣ��ͷż����ź���
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}else{
						if(_CAT_MyStrCpy((ReStrIndex+8)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.iccid,20) == HAL_OK){
							CATGlobalStatus.iccid[20] = '\0';
							
							CATGlobalStatus.flag.sim = 1;
							
							//��ָ��ִ����ϣ��ͷż����ź���
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}
				}
			}else if(_CAT_MyStrCmp(0,"+CME ERROR") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.sim = 0;
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_NO_SIM_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_ERROR;
			}else if(_CAT_MyStrCmp(0,"ERROR:") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.sim = 0;
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_NO_SIM_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_ERROR;
			}
		}else{
			//��ʱ����CATͨѶʧ��
			if(TryCount >= 10){
				CATGlobalStatus.flag.conn = 0;
			
				//����ȫ���¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto _CAT_CheckICCIDTag;
			}
		}
	}
}



/***
*��ѯSN��
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

	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	//����CATģ���ͺŷ��Ͳ�ָͬ��
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MRD_SN?\r");
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CGSN=0\r");
	}
	
	CATSendCmdBuf.writeIndex += TempLen;

_CAT_CheckSNTag:
	TryCount++;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//����CATģ���ͺŷ��Ͳ�ָͬ��
			if(CATGlobalStatus.catType == CAT_YK){
				ReStrIndex = _CAT_MyStrCmp(0,"*MRD_SN:");
			
				//�ж��յ�����Ӧ�Ƿ���ȷ
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
						if(_CAT_MyStrCpy((ReStrIndex+8)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.sn,14) == HAL_OK){	
							CATGlobalStatus.sn[14] = '\0';
							
							//��ָ��ִ����ϣ��ͷż����ź���
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}
				}
			}else{		//����CAT
				ReStrIndex = _CAT_MyStrCmp(0,"\r\n");
			
				//�ж��յ�����Ӧ�Ƿ���ȷ
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
						if(_CAT_MyStrCpy((ReStrIndex+8)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.sn,14) == HAL_OK){
							CATGlobalStatus.sn[14] = '\0';
							
							//��ָ��ִ����ϣ��ͷż����ź���
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_OK;
						}
					}
				}
			}
		}else{
			//��ʱ����CATͨѶʧ��
			if(TryCount >= 5){
				//��ʱ����CATͨѶʧ��
				CATGlobalStatus.flag.conn = 0;
			
				//����ȫ���¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto _CAT_CheckSNTag;
			}
		}
	}
}









/*�ⲿ������****************************************************/
/***
*��ѯCATģ���Ƿ�ע��������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CREG?\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_RegistNetTag:
	TryCount++;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�ж��յ�����Ӧ�Ƿ���ȷ
			ReStrIndex = _CAT_MyStrCmp(0,"+CREG: ");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
					if(CATReceBuf.buf[(ReStrIndex+9)&CAT_RECE_BUF_SIZE] == '1' || CATReceBuf.buf[(ReStrIndex+9)&CAT_RECE_BUF_SIZE] == '5'){
						if(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
							CATGlobalStatus.registerNet = CAT_REGISTNET_SUCCESS;
							
							osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS);
						}
						
						//��ָ��ִ����ϣ��ͷż����ź���
						osSemaphoreRelease(CATSemphrCountTxHandle);
						return HAL_OK;
					}else if(CATReceBuf.buf[(ReStrIndex+9)&CAT_RECE_BUF_SIZE] == '2'){
						if(TryCount >= 10){
							CATGlobalStatus.registerNet = CAT_REGISTNET_CHECKING;
							
							osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_ERR);
							
							//��ָ��ִ����ϣ��ͷż����ź���
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}else{
							goto CAT_RegistNetTag;
						}
					}else if(CATReceBuf.buf[(ReStrIndex+9)&CAT_RECE_BUF_SIZE] == '3'){
						if(TryCount >= 10){
							CATGlobalStatus.registerNet = CAT_REGISTNET_SIM_OVERDUE;
							
							osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_ERR);
							
							//��ָ��ִ����ϣ��ͷż����ź���
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}else{
							goto CAT_RegistNetTag;
						}
					}else{
						if(TryCount >= 10){
							CATGlobalStatus.registerNet = CAT_REGISTNET_UNKNOW;
							
							osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_ERR);
							
							//��ָ��ִ����ϣ��ͷż����ź���
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}else{
							goto CAT_RegistNetTag;
						}
					}
				}
			}
		}else{
			//��ʱ����CATͨѶʧ��
			if(TryCount >= 50){
				CATGlobalStatus.flag.conn = 0;
			
				//����ȫ���¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_RegistNetTag;
			}
		}
	}
}




/***
*��ѯCATģ���ź�ǿ��
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

	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CSQ\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_GetSignalStrengthTag:
	TryCount++;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			ReStrIndex = _CAT_MyStrCmp(0,"+CSQ: ");
			
			//�ж��յ�����Ӧ�Ƿ���ȷ
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
					CATGlobalStatus.signalStrength = CATReceBuf.buf[(ReStrIndex+6)&CAT_RECE_BUF_SIZE]-'0';
				
					if(CATReceBuf.buf[(ReStrIndex+7)&CAT_RECE_BUF_SIZE] != ','){
						CATGlobalStatus.signalStrength = CATGlobalStatus.signalStrength*10+(CATReceBuf.buf[(ReStrIndex+7)&CAT_RECE_BUF_SIZE]-'0');
					}
					
					//��ָ��ִ����ϣ��ͷż����ź���
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}
			}
		}else{
			//��ʱ����CATͨѶʧ��
			if(TryCount >= 3){
				CATGlobalStatus.flag.conn = 0;
			
				//����ȫ���¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_GetSignalStrengthTag;
			}
		}
	}
}




/***
*��ѯ����ʱ�䣬������RTCʱ��
����ֵ������HAL_OK��ʾ��ȡ������ʱ����ţ�������ʾ����ʱ���쳣
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
	uint8_t TimeZoneConstant = 8;					//ʱ����ֵ
	uint8_t Hour = 0;
	uint8_t Day = 0;
	uint8_t Month = 0;
	uint16_t Year = 0;
	uint8_t DayAdd = 0;
	uint8_t MonthAdd = 0;
	uint16_t Temp = 0;
	


	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CCLK?\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_GetNetTimeTag:
	TryCount++;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,1000ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			ReStrIndex = _CAT_MyStrCmp(0,"+CCLK: \"");
			
			//�ж��յ�����Ӧ�Ƿ���ȷ
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
					//ƴ��ʱ�䣬ע�⣬�����϶�ȡ��ʱ����Ҫ+8Сʱ�����й�ʱ��
					CurrentNetTime->year.bcd_h = CATReceBuf.buf[(ReStrIndex+8)&CAT_RECE_BUF_SIZE] - '0';
					CurrentNetTime->year.bcd_l = CATReceBuf.buf[(ReStrIndex+9)&CAT_RECE_BUF_SIZE] - '0';
					CurrentNetTime->month.c = 0;							//20xx��
					
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
					
					//���ݲ�ͬ��CAT�ͺ�ִ�в�ͬ�Ķ������ƿµ�CAT����Ҫ��8Сʱ�����ƵĲ���Ҫ
					if(CATGlobalStatus.catType == CAT_YK){
						Hour = CurrentNetTime->hour.bcd_h*10+CurrentNetTime->hour.bcd_l;
						Day = CurrentNetTime->day.bcd_h*10+CurrentNetTime->day.bcd_l;
						Month = CurrentNetTime->month.bcd_h*10+CurrentNetTime->month.bcd_l;
						Year = 2000+CurrentNetTime->year.bcd_h*10+CurrentNetTime->year.bcd_l;
						
						//����+8Сʱ��������ı仯
						Temp = (Hour+TimeZoneConstant)%24;
						DayAdd = (Hour+TimeZoneConstant)/24;
						Hour = Temp;
						
						//�жϽ����Ƿ������꣺�ܱ�4�����������ܱ�100���������Ա�400�������궼������
						if(((Year%4)!=0) || (((Year%4)==0) && ((Year%100)==0) && ((Year%400)!=0))){
							//������
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
							//����
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
						
						//��ԭ��BCD�뵽�ṹ����
						CurrentNetTime->year.bcd_h = (Year-2000)/10;
						CurrentNetTime->year.bcd_l = (Year-2000)%10;
						
						CurrentNetTime->month.bcd_h = Month/10;
						CurrentNetTime->month.bcd_l = Month%10;
						
						CurrentNetTime->day.bcd_h = Day/10;
						CurrentNetTime->day.bcd_l = Day%10;
						
						CurrentNetTime->hour.bcd_h = Hour/10;
						CurrentNetTime->hour.bcd_l = Hour%10;
					}
					
					//��ָ��ִ����ϣ��ͷż����ź���
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}
			}
		}else{
			//��ʱ����CATͨѶʧ��
			if(TryCount >= 5){
				CATGlobalStatus.flag.conn = 0;
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_GetNetTimeTag;
			}
		}
	}
}





/***
*��ѯ����������CAT��λ��Ҫʹ�õĸߵ�KEY
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

	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+LBSKEY?\r");
	
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			sprintf(TempStr,"+LBSKEY:%s",GAODE_KEY_POSITION);
			
			//�ж��յ�����Ӧ�Ƿ���ȷ
			if(_CAT_MyStrCmp(0,TempStr) != CAT_RECE_BUF_NO_STR){	//�Ѵ���KEY����ֱ���˳�����
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else{			//������KEY������Ҫ����KEY
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
			//��ʱ����CATͨѶʧ��
			CATGlobalStatus.flag.conn = 0;
		
			//����ȫ���¼�
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			
			//��ָ��ִ����ϣ��ͷż����ź���
			osSemaphoreRelease(CATSemphrCountTxHandle);
			return HAL_TIMEOUT;
		}
	}
}





/***
*��ѯ��γ��λ����Ϣ
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

	
	//���ж��Ƿ�������CAT���ǣ�����Ҫ�����øߵ�KEY��Ȼ�����������λ��KEY����һ�ξͻ��Զ�����
	if(CATGlobalStatus.catType == CAT_ZY){
		_CAT_ZY_SetGDKey();
	}
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
CAT_GetPositionTag:
	TryCount++;
	
	CATSendCmdBuf.writeIndex = 0;
	
	//������λ
	//���ݲ�ͬ��CAT�ͺ�ִ�в�ͬ��ָ��
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+GTPOS=1\r");
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+QCELLLOC\r");
	}
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//��ͬCATִ�в�ͬ��ָ��
			if(CATGlobalStatus.catType == CAT_YK){
				if(_CAT_MyStrCmp(0,"CONNECT OK") != CAT_RECE_BUF_NO_STR || _CAT_MyStrCmp(0,"ALREARY CONNECTED") != CAT_RECE_BUF_NO_STR){				
					//===��ȡλ������===
					CATSendCmdBuf.writeIndex = 0;
					
					//��ȡ��λ��Ϣ
					TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+GTPOS=2\r");
					CATSendCmdBuf.writeIndex += TempLen;
					
					osKernelLock();
					HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
					osKernelUnlock();
					
					while(1){
						//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,20000ms��ʱ
						xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
						
						//�ж��Ƿ��յ���Ӧ
						if(xReturn == osOK){
							ReStrIndex = _CAT_MyStrCmp(0,"GTPOS:");
				
							//�ж��յ�����Ӧ�Ƿ���ȷ
							if(ReStrIndex != CAT_RECE_BUF_NO_STR){
								if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
									//����λ����Ϣ
									if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") > ReStrIndex){
										_CAT_MyStrCpy((ReStrIndex+6)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.position,(_CAT_MyStrCmp(ReStrIndex,"\r\nOK")-ReStrIndex-8));
									}else{
										_CAT_MyStrCpy((ReStrIndex+6)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.position,(_CAT_MyStrCmp(ReStrIndex,"\r\nOK")+CAT_RECE_BUF_SIZE+1-ReStrIndex-8));
									}
									
									//===�رն�λ===
									CATSendCmdBuf.writeIndex = 0;
									
									//�رն�λ
									TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+GTPOS=0\r");
									CATSendCmdBuf.writeIndex += TempLen;
									
									osKernelLock();
									HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
									osKernelUnlock();
									
									while(1){
										//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
										xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
										
										//�ж��Ƿ��յ���Ӧ
										if(xReturn == osOK){
											if(_CAT_MyStrCmp(0,"OK") != CAT_RECE_BUF_NO_STR){
												osSemaphoreRelease(CATSemphrCountTxHandle);
												return HAL_OK;
											}
										}else{
											//��ʱ����CATͨѶʧ��
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
							//��ʱ����CATͨѶʧ��
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
				//����CAT
				ReStrIndex = _CAT_MyStrCmp(0,"+QCELLLOC: ");
				
				//�ж��յ�����Ӧ�Ƿ���ȷ
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") != CAT_RECE_BUF_NO_STR){
						//����λ����Ϣ
						if(_CAT_MyStrCmp(ReStrIndex,"\r\nOK") > ReStrIndex){
							_CAT_MyStrCpy((ReStrIndex+11)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.position,(_CAT_MyStrCmp(ReStrIndex,"\r\nOK")-ReStrIndex-13));
						}else{
							_CAT_MyStrCpy((ReStrIndex+11)&CAT_RECE_BUF_SIZE,(uint8_t*)CATGlobalStatus.position,(_CAT_MyStrCmp(ReStrIndex,"\r\nOK")+CAT_RECE_BUF_SIZE+1-ReStrIndex-13));
						}
						
						//��ָ��ִ����ϣ��ͷż����ź���
						osSemaphoreRelease(CATSemphrCountTxHandle);
						return HAL_OK;
					}
				}
			}
		}else{
			//��ʱ����CATͨѶʧ��
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
*��CATģ������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	//���ݲ�ͬCAT�ͺ�ִ�в�ָͬ��
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+NETOPEN\r");
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CGATT=1\rAT+CGACT=1\r");
	}
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_OpenNetTag:
	TryCount++;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//���ݲ�ͬ��CAT�ͺ�ִ�в�ͬ��ָ��
			if(CATGlobalStatus.catType == CAT_YK){
				//�ж��յ�����Ӧ�Ƿ���ȷ
				if(_CAT_MyStrCmp(0,"+NETOPEN:SUCCESS") != CAT_RECE_BUF_NO_STR){
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}else if(_CAT_MyStrCmp(0,"ERROR: 902") != CAT_RECE_BUF_NO_STR){
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}else{
					if(TryCount >= 5){
						osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_ERR);
						
						//��ָ��ִ����ϣ��ͷż����ź���
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
			//��ʱ����CATͨѶʧ��
			if(TryCount >= 5){
				CATGlobalStatus.flag.conn = 0;
			
				//����ȫ���¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_OpenNetTag;
			}
		}
	}
}




/***
*��ѯCATģ����MCUͨѶ�Ƿ������
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

	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_IsOKTag:
	TryCount++;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,200);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�ж��յ�����Ӧ�Ƿ���ȷ
			if(_CAT_MyStrCmp(0,"\r\nOK\r\n") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.conn = 1;
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			if(TryCount >= 20){
				//��ʱ����CATͨѶʧ��
				CATGlobalStatus.flag.conn = 0;
				
				//����ȫ���¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_IsOKTag;
			}
		}
	}
}




/***
*��ѯCATģ���ͺţ��Ա�����ƿº����Ƶ���������
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

	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CGMM\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
CAT_ReadTypeTag:
	TryConut++;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,500);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			if(_CAT_MyStrCmp(0,"+CGMM: LYNQ_L501") != CAT_RECE_BUF_NO_STR){
				//�ж��Ƿ����ƿµ�ģ��
				CATGlobalStatus.catType = CAT_YK;
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"ML302") != CAT_RECE_BUF_NO_STR){
				//�ж��Ƿ������Ƶ�ģ��
				CATGlobalStatus.catType = CAT_ZY;
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			if(TryConut >= 10){
				//��ʱ����CATͨѶʧ��
				CATGlobalStatus.flag.conn = 0;
				
				//����ȫ���¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_TIMEOUT;
			}else{
				goto CAT_ReadTypeTag;
			}
		}
	}
}





/***
*��ʼ������CATģ��
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
	static uint8_t CATRestartCount = 0;					//���ڼ�¼CAT��������
	
	
	//��ģ�����ƿµģ���ȴ�ģ��������ϣ���������Call Ready�ַ����󣬷�ֹģ����δ������ϾͿ�ʼ����ָ��ʱ�����쳣�������Ƶ�ģ����ֱ�������˲���
	while(CATGlobalStatus.catType == CAT_YK){
		//15s��ʱ�����˳�ѭ��
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,15000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			if(_CAT_MyStrCmp(0,"Call Ready") != CAT_RECE_BUF_NO_STR){
				break;
			}
		}else{
			break;
		}
	}
	
	//���ù���
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	//ȥ��ָ����Թ���
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"ATE0\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,500);
	osKernelUnlock();
	
	//�ӳ�1s
	osDelay(1000);
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	
	//�ȴ��������
	if(CAT_IsOK() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//����̨������Ϣ������CAT��ʼ������
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//ģ�鴮��ͨѶ�쳣
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//��ѯIMEI��
	if(_CAT_CheckIMEI() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//����̨������Ϣ������CAT��ʼ������
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//ģ�鴮��ͨѶ�쳣
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//��ѯSN��
	if(_CAT_CheckSN() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//����̨������Ϣ������CAT��ʼ������
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//ģ�鴮��ͨѶ�쳣
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//��ѯICCID��
	if(_CAT_CheckICCID() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//����̨������Ϣ������CAT��ʼ������
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//ģ�������쳣
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//��ѯ�ź�ǿ�ȣ�ֱ���ź�ǿ�ȳ���13ʱ�ż���ִ�к����������ֹ����
	TryCount = 0;
	while(1){
		CAT_GetSignalStrength();
		
		if(CATGlobalStatus.signalStrength > 10){
			break;
		}else{
			osDelay(1000);
			
			TryCount++;
			
			//60����źŻ�δ����������̨������Ϣ������CAT��ʼ������
			if(TryCount > 60){
				CATRestartCount++;
				
				if(CATRestartCount <= 3){
					Msg_Head_t BackendMsg = {0};
				
					//����̨������Ϣ������CAT��ʼ������
					BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
					UI_Put_Msg((uint8_t*)&BackendMsg);
					
					osDelay(10000);
				}else{
					//����CATģ��3�κ������绹δ�ã����������
					osThreadSuspend(CATUpdateStatusTaskHandle);
				}
			}
		}
	}
	
	//�ȴ�ע��������
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
				
					//����̨������Ϣ������CAT��ʼ������
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
			
			//30����źŻ�δ���������������
			if(TryCount > 20){
				CATRestartCount++;
				
				if(CATRestartCount <= 3){
					Msg_Head_t BackendMsg = {0};
				
					//����̨������Ϣ������CAT��ʼ������
					BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
					UI_Put_Msg((uint8_t*)&BackendMsg);
					
					osDelay(10000);
				}else{
					osThreadSuspend(CATUpdateStatusTaskHandle);
				}
			}
		}
	}
	
	//��ѯ��Ӫ��
	if(_CAT_CheckOperator() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//����̨������Ϣ������CAT��ʼ������
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//ģ�������쳣
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//������
	if(CAT_OpenNet() != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//����̨������Ϣ������CAT��ʼ������
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//ģ�������쳣
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//��ȡ����ʱ��
	if(CAT_GetNetTime(&CurrentNetTime) != HAL_OK){
		CATRestartCount++;
			
		if(CATRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//����̨������Ϣ������CAT��ʼ������
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//ģ�������쳣
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//ͬ��RTC��HYM8563
	RTC_HYM8563SetTime(&CurrentNetTime);
	
	//��ȡ��λ��Ϣ����������ܻ�վӰ�죬���¶�λ��Ϣ��ȡ������ֻ���´ο�������
	CAT_GetPosition();
	
	return HAL_OK;
}





//===MQTT��صײ�ָ��=Ϊ�˷�ֹ����й©��ֻҪ��������ERROR�����������============================================================
/***
*CAT������MQTT�ͻ��������Ϣ����������������Ϣ���ƣ�retain=0��qos=1��ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
*������
	ClientID���ͻ���ID�ַ�������󳤶�Ϊ256�ֽ�
	UserName����¼�û����ַ���������ΪNULL
	Password����¼�����ַ���������ΪNULL
	WillTopic��������Ϣ�����ַ���
	WillMessage��������Ϣ�ַ���
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
	
	//���жϲ����Ƿ�Ϸ�
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
	//�ж������Ƿ����ӣ�����һֱ�ȴ����ӳɹ�����Ҫ������¼���־
	osEventFlagsClear(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS);
	while(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS,osFlagsWaitAny,osWaitForever);
	}

	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	if(UserName == NULL){
		if(WillTopic == NULL){
			//���ݲ�ͬCATģ������ִ�в�ָͬ��
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
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,500);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//���ݲ�ͬ��CAT����ִ�в�ͬ��ָ��
			if(CATGlobalStatus.catType == CAT_YK){
				//�ж��յ�����Ӧ�Ƿ���ȷ
				if(_CAT_MyStrCmp(0,"\r\nOK\r\n") != CAT_RECE_BUF_NO_STR){
					
					//��ָ��ִ����ϣ��ͷż����ź���
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}
			}else{
				if(_CAT_MyStrCmp(0,"+MMQTTCON: 0,0") != CAT_RECE_BUF_NO_STR){
					CATGlobalStatus.flag.tcpConn = 1;
					CATGlobalStatus.flag.mqttConn = 1;
					
					//��ָ��ִ����ϣ��ͷż����ź���
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}else if(_CAT_MyStrCmp(0,"+MMQTTCON: 0,1") != CAT_RECE_BUF_NO_STR){
					//��ָ��ִ����ϣ��ͷż����ź���
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_ERROR;
				}
			}
		}else{
			//��ʱ����CATͨѶʧ��
			//��ָ��ִ����ϣ��ͷż����ź���
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
*CAT�����ӷ���˺Ͷ˿ڣ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
*������
	HostName�������IP��ַ��������
	Port��MQTT�˿�
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
	
	//���жϲ����Ƿ�Ϸ�
	if(HostName == NULL){
		return HAL_ERROR;
	}else if(strlen(HostName) > 256){
		return HAL_ERROR;
	}
	
CAT_CreateTcpConnTag:
	//�ж������Ƿ����ӣ�����һֱ�ȴ����ӳɹ�����Ҫ������¼���־
	osEventFlagsClear(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS);
	while(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS,osFlagsWaitAny,osWaitForever);
	}

	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MIPSTART=\"%s\",%u\r\n",HostName,Port);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�ж��յ�����Ӧ�Ƿ���ȷ
			if(_CAT_MyStrCmp(0,"+MIPSTART: SUCCESS") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.tcpConn = 1;
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"+MIPSTART: FAILURE") != CAT_RECE_BUF_NO_STR){
				//������������ظ����Ӷ������Ĵ�������������Ӵ�������������Ҫ����һ���Ͽ����ӵĲ�����Ȼ����������
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
			//��ʱ����CATͨѶʧ��		
			//��ָ��ִ����ϣ��ͷż����ź���
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			if(TryCount == 3){
				//����ȫ���¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				return HAL_TIMEOUT;
			}else{
				goto CAT_CreateTcpConnTag;
			}
		}
	}
}




/***
*CAT�����ӷ���˵�MQTT��ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
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
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MCONNECT=1,50\r\n");
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,200);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�ж��յ�����Ӧ�Ƿ���ȷ
			if(_CAT_MyStrCmp(0,"+MCONNECT: SUCCESS") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.mqttConn = 1;
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"+MCONNECT: FAILURE") != CAT_RECE_BUF_NO_STR){
				CATGlobalStatus.flag.mqttConn = 1;
				
				//��ָ��ִ����ϣ��ͷż����ź���
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			//��ָ��ִ����ϣ��ͷż����ź���
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//��ʱ����CATͨѶʧ��
			if(TryCount == 3){
				//����ȫ���¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
				return HAL_TIMEOUT;
			}else{
				goto CAT_CreateMQTTConnTag;
			}
		}
	}
}




/***
*CAT���ر�MQTT���ӣ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
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
	//�ж������Ƿ����ӣ�����һֱ�ȴ����ӳɹ�����Ҫ������¼���־
	osEventFlagsClear(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS);
	while(CATGlobalStatus.registerNet != CAT_REGISTNET_SUCCESS){
		osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REGIST_NET_SUCCESS,osFlagsWaitAny,osWaitForever);
	}
	
	TryCount++;
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	//���ݲ�ͬCAT����ִ�в�ָͬ��
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MDISCONNECT\r\nAT+MIPCLOSE\r\n");
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTDISCON=0\r\n");
	}
	
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,200);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,1000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//���ݲ�ͬCAT����ִ�в�ָͬ��
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
			
			//��ʱ����CATͨѶʧ��
			if(TryCount == 3){
				return HAL_TIMEOUT;
			}else{
				goto CAT_CloseMQTTConnTag;
			}
		}
	}
}




/***
*CAT���������⣬ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
*������
	Topic�����ĵ������ַ���
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
	
	//���жϲ����Ƿ�Ϸ�
	if(Topic == NULL){
		return HAL_ERROR;
	}
	
	//�ж��Ƿ�����MQTT����˳ɹ�
	if(CATGlobalStatus.flag.mqttConn == 0 || CATGlobalStatus.flag.tcpConn == 0){
		return HAL_ERROR;
	}
	
CAT_SubTopicTag:
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	//���ݲ�ͬCAT����ִ�в�ͬ��ָ��
	if(CATGlobalStatus.catType == CAT_YK){
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MSUB=\"%s\",2\r\n",Topic);
	}else{
		TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+MMQTTSUB=0,\"%s\",0,2\r\n",Topic);
	}
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,500ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//���ݲ�ͬCAT����ִ�в�ͬ��ָ��
			if(CATGlobalStatus.catType == CAT_YK){
				//�ж��յ�����Ӧ�Ƿ���ȷ
				if(_CAT_MyStrCmp(0,"+MSUB: SUCCESS") != CAT_RECE_BUF_NO_STR){
					
					//��ָ��ִ����ϣ��ͷż����ź���
					osSemaphoreRelease(CATSemphrCountTxHandle);
					return HAL_OK;
				}else if(_CAT_MyStrCmp(0,"+MSUB: FAILURE") != CAT_RECE_BUF_NO_STR){
					
					//��ָ��ִ����ϣ��ͷż����ź���
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
			//��ָ��ִ����ϣ��ͷż����ź���
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//��ʱ����CATͨѶʧ��
			if(TryCount == 3){
				return HAL_TIMEOUT;
			}else{
				goto CAT_SubTopicTag;
			}
		}
	}
}



/***
*CAT����������Ϣ�����ҵ���Ҫ�������Ӧ��Ϣʱ���򽫽��ջ������úã�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
*������
	Topic�����ĵ������ַ���
	Qos����Ϣ��������
	SendBufLen��Payload�ܳ���
	SendBuf����������Ϣ����
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
	
	//���жϲ����Ƿ�Ϸ�
	if(Topic == NULL || Qos > 2 || SendBuf == NULL || SendBufLen > 4200){
		return HAL_ERROR;
	}
	
	//�ж��Ƿ�����MQTT����˳ɹ�
	if(CATGlobalStatus.flag.mqttConn == 0 || CATGlobalStatus.flag.tcpConn == 0){
		return HAL_ERROR;
	}
	
CAT_PubTopicTag:
	TryCount++;
	
	//���﷢���ź����ͽ����ź������ɵ����߸����ȡ����Ϊ������Ϣ����ܲ���Ҫ�ȴ�����˷��ص���Ϣ��Ҳ������Ҫ�ȴ�����˷��ص���Ϣ
	CATSendCmdBuf.writeIndex = 0;
	
	//�ȷ���ָ��ӳ�25ms���Ϻ��ڷ�������
	//���ݲ�ͬ��CAT���ͷ��Ͳ�ͬ��ָ��
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
	
	//ͨ��DMA��ʽ������������
	HAL_UART_Transmit_DMA(&huart4,SendBuf,SendBufLen);
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,30000ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//���ݲ�ͬ��CAT���ͷ��Ͳ�ͬ��ָ��
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
			//��ʱ����CATͨѶʧ�ܣ���������Ϣû�з��͵�����ˣ��Ӷ��ط�����
			if(TryCount == 3){
				return HAL_TIMEOUT;
			}else{
				goto CAT_PubTopicTag;
			}
		}
	}
}




/***
*���ӷ���˷���MQTT����֡��CAT���ջ����и��Ƶ�ָ������
*����ֵ�����Ƴɹ�����HAL_OK�����򷵻�HAL_ERROR
***/
static HAL_StatusTypeDef _CAT_M_CopyReceData(uint8_t* Buf,uint16_t* TotalLen)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	uint16_t ReStrIndex = 0;
	uint8_t i = 0;
	uint8_t TopicLen = 0;
	
	//���ݲ�ͬ��CATִ�в�ͬ��ָ��
	if(CATGlobalStatus.catType == CAT_YK){
		ReStrIndex = _CAT_MyStrCmp(0,"+MSUB: \"");
		if(ReStrIndex != CAT_RECE_BUF_NO_STR){
			ReStrIndex = _CAT_MyStrCmp(ReStrIndex,",");
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				if(_CAT_MyStrCmp(ReStrIndex," bytes") != CAT_RECE_BUF_NO_STR){
					*TotalLen = 0;
					
					//��ȡ�����ݳ���ֵ
					for(i=0;i<8;i++){
						ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
						
						if(CATReceBuf.buf[ReStrIndex] == ' '){
							break;
						}
						
						*TotalLen = (*TotalLen)*10+(CATReceBuf.buf[ReStrIndex]-'0');
					}
					
					//�������ݵ��ṹ���з���
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
					
						//��ȡ�����ַ�������ֵ
						for(i=0;i<8;i++){
							ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
							
							if(CATReceBuf.buf[ReStrIndex] == ','){
								break;
							}
							
							TopicLen = (TopicLen)*10+(CATReceBuf.buf[ReStrIndex]-'0');
						}
						
						*TotalLen = 0;
						
						//��ȡ�����ַ�������ֵ
						for(i=0;i<8;i++){
							ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
							
							if(CATReceBuf.buf[ReStrIndex] == '\r'){
								break;
							}
							
							*TotalLen = (*TotalLen)*10+(CATReceBuf.buf[ReStrIndex]-'0');
						}
						
						//�������ݵ��ṹ���з���
						return _CAT_MyStrCpy((ReStrIndex+4+TopicLen)&CAT_RECE_BUF_SIZE,Buf,*TotalLen);
					}
				}
			}
		}
	}
	
	return HAL_ERROR;
}






//===MQTTҵ���߼�ʵ��========================================================================
/***
*���Ͳ����Ƿ���Ϣ�������
***/
static HAL_StatusTypeDef _CAT_B_Send_ParaIllegal(CAT_SERVICE_e CatService)
{
	extern MQTT_PayloadBuf_s MQTTPayloadBuf;
	extern MachInfo_s MachInfo;
	extern osSemaphoreId_t CATSemphrBinRxHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	RTC_HYM8563Info_s RTCHym8563Info;
	char TopicString[25] = {0};
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0000;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 0;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;

	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,2,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}




//===�������Է���˵�����ָ������=======================================
/***
*�豸��Ӧ����������������в�����Ϣָ��
*����ֵ������HAL_OK��ʾ�����Ӧ��HAL_ERROR��ʾ��Ӧʧ��
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//�����������в�����Ϣ
	if(CAT_Send_MachineAllPara() != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����һ��������Ϣ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)MQTTPayloadBuf,TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//�����൱����Ϣ�Ѿ����͵�����ˣ����Ƿ����û����Ӧ���Ӷ��ط�����
			if(TryCount >= 2){
				return HAL_ERROR;
			}else{
				goto _CAT_B_Respond_MachineAllParaInfoTag;
			}
		}
	}
}




/***
*�豸��Ӧ�����������������������õ�ָ��
*������
	InfoType����ʾ��������
	Flag��Ϊ1��ʾ���ĳɹ���Ϊ0��ʾ����ʧ��
*����ֵ������HAL_OK��ʾ�����Ӧ��HAL_ERROR��ʾ��Ӧʧ��
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf->payloadLen+14,(uint8_t*)MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����һ��������Ϣ
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
*�豸��Ӧ�������������������⹦�������õ�ָ��
*����ֵ������HAL_OK��ʾ�����Ӧ��HAL_ERROR��ʾ��Ӧʧ��
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf->payloadLen+14,(uint8_t*)MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����һ��������Ϣ
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
*�����յ��ķ���˵�������������
*����ֵ��HAL_OK��ʾ�������������ָ������ָ�ͨѶ����Ȩ��ת����MCU��
		HAL_ERROR��ʾ����������ָ���쳣����û����������ָ�����ָ���һ�������߽�
		��������
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
	//�ж���Ӧ�����Ƿ���ȷ
	if(MQTTPayloadBuf->payloadLen+14 != *TotalLen){
		return HAL_ERROR;
	}
	
	//�ж�CRC�Ƿ���ȷ
	if(CRC16((uint8_t*)&MQTTPayloadBuf->payloadLen,MQTTPayloadBuf->payloadLen+2) != MQTTPayloadBuf->crc16){
		return HAL_ERROR;
	}
	
	//��������
	switch((uint32_t)MQTTPayloadBuf->cmdH<<16|MQTTPayloadBuf->cmdM<<8|MQTTPayloadBuf->cmdL){
		//���������ָ������
		case 0x00030000:
			return HAL_OK;
		
		//����������������в���������Ϣ
		case 0x00030100:
		{
			if(_CAT_B_Respond_MachineAllParaInfo(TotalLen,MQTTPayloadBuf) != HAL_OK){
				return HAL_ERROR;
			}
			goto CAT_B_DealServiceRequestDataTag;
		}
		
		//���������������������趨ֵ
		case 0x00040000:
		{
			uint32_t TempU32 = 0;
			char TempStr[70] = {0};
			float TempF = 0;
			CommonDialogPageData_s DialogPageData = {0};
			WM_MESSAGE pMsg;
			extern WM_HWIN DialogWin;
			
			switch(MQTTPayloadBuf->payload.cmd2_5_1.infoType){
				//���������Ƿ�������״̬
				case 0:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//���Ͳ����Ƿ���Ϣ�������
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
							//�ж��Ƿ���Ҫ�������߽���
							if(MachInfo.flag.bit.lock == 1){
								//������ʾ
								if(DialogWin == NULL){
									WM_HideWindow(g_hActiveWin);
									DialogWin = CreateDialogPage();
								}
								
								//������ʾ
								DialogPageData.bmpType = BMP_ALARM;
								DialogPageData.confimButt = NO_CONFIM_BUTTON;
								DialogPageData.fun = NULL;
								strcpy(DialogPageData.str,g_ucaLng_LOCK_MACHINE[MachInfo.systemSet.eLanguage]);		//����

								pMsg.MsgId = WM_USER_DATA;
								pMsg.Data.p = &DialogPageData;
								WM_SendMessage(DialogWin,&pMsg);
							}else{
								//������ʾ
								if(DialogWin == NULL){
									WM_HideWindow(g_hActiveWin);
									DialogWin = CreateDialogPage();
								}
								
								//������ʾ
								DialogPageData.bmpType = BMP_OK;
								DialogPageData.confimButt = NO_CONFIM_BUTTON;
								DialogPageData.fun = NULL;
								strcpy(DialogPageData.str,g_ucaLng_UNLOCK_MACHINE[MachInfo.systemSet.eLanguage]);		//����

								pMsg.MsgId = WM_USER_DATA;
								pMsg.Data.p = &DialogPageData;
								WM_SendMessage(DialogWin,&pMsg);
							}
						}
					}
				}break;
				
				//�޸��������ַ���
				case 1:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 50){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				//�޸�WBC����ֵ
				case 2:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//���Ͳ����Ƿ���Ϣ�������
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
						
						//д��WBC����
						WBC_Gran_Set(WBC_DR_CHANNEL,MachInfo.paramSet.ucWBC_Gain);
					}
				}break;
				
				//�޸�HGB����ֵ
				case 3:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//���Ͳ����Ƿ���Ϣ�������
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
						//д��HGB����
						HGB_Gran_Set(HGB_DR_CHANNEL,MachInfo.paramSet.ucHGB_Gain);
					}
				}break;
				
				//�޸ı�ռ�ձ�ֵ
				case 4:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.pumpPara > 100){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				//�޸�535�����趨ֵ
				case 5:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.current535 > 4095){
						//���Ͳ����Ƿ���Ϣ�������
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
					
				//�޸�415�����趨ֵ
				case 6:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.current415 > 4095){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				//�޸ĳ��ֹ����ֵ
				case 7:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.outOC > 20000){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				//�޸Ľ��ֹ����ֵ
				case 8:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.inOC > 20000){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				//�޸Ĺ̶������ֵ
				case 9:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.gdOC > 20000){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				//�޸��ͷŹ����ֵ
				case 10:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.sfOC > 20000){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				//�޸�ѹ������ֵ
				case 11:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
				
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.presurePara > 100 || MQTTPayloadBuf->payload.cmd2_5_1.u.presurePara < -100){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				//�޸�WBCУ׼ϵ��ֵ
				case 12:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				//�޸�HGBУ׼ϵ��ֵ
				case 13:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 2){
						//���Ͳ����Ƿ���Ϣ�������
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
					
				//�޸ĵ�ǰʹ�õ�ѹ������������
				case 14:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.pressureSensor > 1){
						//���Ͳ����Ƿ���Ϣ�������
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
					
				//�޸ĵ�ǰ����������˾
				case 15:
				{
					if(MQTTPayloadBuf->payloadLen-2 != 1){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(MQTTPayloadBuf->payload.cmd2_5_1.u.company >= COMPANY_END){
						//���Ͳ����Ƿ���Ϣ�������
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
							//���¹�˾���Բ���
							Company_ConfigInfo(MachInfo.companyInfo.company);
						}
					}
				}break;
				
				//�޸�YongFu�˻�����
				case 16:
				{
					if(MQTTPayloadBuf->payloadLen-2 != ACCOUNT_PSW_MAX_LEN){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(strlen(MQTTPayloadBuf->payload.cmd2_5_1.u.yongFuPsw) == 0){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				//�޸�admin�˻�����
				case 17:
				{
					if(MQTTPayloadBuf->payloadLen-2 != ACCOUNT_PSW_MAX_LEN){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_CC);
						return HAL_ERROR;
					}
					
					if(strlen(MQTTPayloadBuf->payload.cmd2_5_1.u.adminPsw) == 0){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				//�����쳣
				default :
					//���Ͳ����Ƿ���Ϣ�������
					_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
			
			if(ReturnState != HAL_OK){
				//�豸���Ͳ�������ʧ����Ϣ
				if(_CAT_B_Respond_ChangeSetInfo(MQTTPayloadBuf->payload.cmd2_5_1.infoType,0,TotalLen,MQTTPayloadBuf) != HAL_OK){
					return HAL_ERROR;
				}
			}else{
				//�ж��Ƿ���������Ƿ�����״̬
				if(MQTTPayloadBuf->payload.cmd2_5_1.infoType == 0){
					if(TempU32 != MachInfo.flag.bit.lock){
						//�����Ҫ��������
						CATGlobalStatus.catDealSysRestartFlag = 0xA55A0FF0;
					}
				}
				
				//�豸���Ͳ������������Ϣ
				if(_CAT_B_Respond_ChangeSetInfo(MQTTPayloadBuf->payload.cmd2_5_1.infoType,1,TotalLen,MQTTPayloadBuf) != HAL_OK){
					return HAL_ERROR;
				}
			}
			
			goto CAT_B_DealServiceRequestDataTag;
		}
		
		//��������������⹦��
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
		
			//�豸�������⹦��ִ�б�־���������Ϣ
			if(_CAT_B_Respond_ChangeSpecialFunInfo(MQTTPayloadBuf->payload.u8,TotalLen,MQTTPayloadBuf) != HAL_OK){
				return HAL_ERROR;
			}
			goto CAT_B_DealServiceRequestDataTag;
		}
		
		
		//===������������������===
		//���������������ָ������
		case 0xA0040200:
			return HAL_OK;
		
		//��������������������������ֵ
		case 0xA0040100:
		{
			char TempStr[70] = {0};
			
			switch(MQTTPayloadBuf->payload.cmdA_2_6_2.infoType){
				//�޸�����admin�˻�����
				case 0:
				{
					if(MQTTPayloadBuf->payloadLen-1 != ACCOUNT_PSW_MAX_LEN){
						//���Ͳ����Ƿ���Ϣ�������
						_CAT_B_Send_ParaIllegal(SERVICE_OTHER);
						return HAL_ERROR;
					}
					
					if(strlen(MQTTPayloadBuf->payload.cmdA_2_6_2.u.adminPsw) == 0){
						//���Ͳ����Ƿ���Ϣ�������
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
				
				
				//�����쳣
				default :
					//���Ͳ����Ƿ���Ϣ�������
					_CAT_B_Send_ParaIllegal(SERVICE_OTHER);
				return HAL_ERROR;
			}
			
			if(ReturnState != HAL_OK){
				//�豸���Ͳ�������ʧ����Ϣ
				if(CAT_B_Other_Respond_ChangeSetInfo(MQTTPayloadBuf->payload.cmdA_2_6_2.infoType,0,TotalLen,MQTTPayloadBuf) != HAL_OK){
					return HAL_ERROR;
				}
			}else{
				//�豸���Ͳ������������Ϣ
				if(CAT_B_Other_Respond_ChangeSetInfo(MQTTPayloadBuf->payload.cmdA_2_6_2.infoType,1,TotalLen,MQTTPayloadBuf) != HAL_OK){
					return HAL_ERROR;
				}
			}
			
			goto CAT_B_DealServiceRequestDataTag;
		}
			
		default :return HAL_ERROR;
	}
}





//===�������Է������Ӧ��Ϣ����============================================
/***
*�����յ��ķ���˵���Ӧ��������
***/
HAL_StatusTypeDef CAT_B_DealServiceAckData(uint16_t SendCmdH,uint8_t SendCmdM,uint8_t SendCmdL,uint16_t TotalLen,MQTT_PayloadBuf_s* MQTTPayloadBuf)
{
	extern MachInfo_s	MachInfo;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern osSemaphoreId_t CATSemphrCountTxHandle;
	extern CATGlobalStatus_s CATGlobalStatus;
	HAL_StatusTypeDef ReturnStates = HAL_OK;
	
	//�ж���Ӧ�����Ƿ��Ƿ���˷��͵Ĳ����Ƿ�֡
	if(((uint32_t)MQTTPayloadBuf->cmdH<<16|MQTTPayloadBuf->cmdM<<8|MQTTPayloadBuf->cmdL) == 0x00000000){
		osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_MACHINE_PARA_ILLEGAL);
		return HAL_ERROR;
	}
	
	//�ж���Ӧ�����Ƿ���ȷ
	if(MQTTPayloadBuf->cmdH != SendCmdH || MQTTPayloadBuf->cmdM != SendCmdM /*|| MQTTPayloadBuf->cmdL != (SendCmdL+1)*/ || MQTTPayloadBuf->payloadLen+14 != TotalLen){
		return HAL_ERROR;
	}
	
	//�ж�CRC�Ƿ���ȷ
	if(CRC16((uint8_t*)&MQTTPayloadBuf->payloadLen,MQTTPayloadBuf->payloadLen+2) != MQTTPayloadBuf->crc16){
		return HAL_ERROR;
	}
	
	//��������
	switch((uint32_t)MQTTPayloadBuf->cmdH<<16|MQTTPayloadBuf->cmdM<<8|MQTTPayloadBuf->cmdL){
		//��Ӧ����������Ϣ�Ƿ��Ѿ�¼��
		case 0x00010101:
		{
			if(MQTTPayloadBuf->payload.u8 > 1){
				//���Ͳ����Ƿ���Ϣ�������
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}else if(MQTTPayloadBuf->payload.u8 == 1){
				//�������к��Ѵ����¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_FACTORY_INFO_EXIST);
				return HAL_BUSY;
			}
			
			//��ȡTX�ź���
			osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
			
			//���͵�ǰ���������еĲ�����Ϣ
			ReturnStates = CAT_Send_MachineAllPara();
			
			//�ͷ��ź���
			osSemaphoreRelease(CATSemphrCountTxHandle);
			return ReturnStates;
		}
		
		//�������Ӧ��ǰ��������·����汾����Ϣ
		case 0x00010201:
		{
			//���жϲ����Ƿ�Ƿ�
			if(MQTTPayloadBuf->payload.cmd2_2_5.bootloadVersion > 999 ||  MQTTPayloadBuf->payload.cmd2_2_5.mainVersion > 999 || MQTTPayloadBuf->payload.cmd2_2_5.uiVersion > 999 ){
				//���Ͳ����Ƿ���Ϣ�������
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
		
			CATGlobalStatus.serviceVersion.PBootloadVersion = MQTTPayloadBuf->payload.cmd2_2_5.bootloadVersion;
			CATGlobalStatus.serviceVersion.PMainVersion = MQTTPayloadBuf->payload.cmd2_2_5.mainVersion;
			CATGlobalStatus.serviceVersion.PUiVersion = MQTTPayloadBuf->payload.cmd2_2_5.uiVersion;
			return HAL_OK;
		}
		
		//�������Ӧ����ĳ����ļ���Ϣ
		case 0x00020101:
		{
			//���жϲ����Ƿ�Ƿ�
			if(MQTTPayloadBuf->payload.cmd2_3_4.publishOrTestFlag > 1 || MQTTPayloadBuf->payload.cmd2_3_4.softwareType > 1){
				//���Ͳ����Ƿ���Ϣ�������
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
			
			CATGlobalStatus.softwareInfo.publishOrTestFlag = MQTTPayloadBuf->payload.cmd2_3_4.publishOrTestFlag;
			CATGlobalStatus.softwareInfo.softwareType = MQTTPayloadBuf->payload.cmd2_3_4.softwareType;
			CATGlobalStatus.softwareInfo.totalLen = MQTTPayloadBuf->payload.cmd2_3_4.totalLen;
			CATGlobalStatus.softwareInfo.crc16 = MQTTPayloadBuf->payload.cmd2_3_4.crc16;
			
			return HAL_OK;
		}
		
		//�������Ӧ�����UI��Դ��Ϣ
		case 0x00020201:
		{
			uint8_t i = 0;
			
			//���жϲ����Ƿ�Ƿ�
			if(MQTTPayloadBuf->payload.cmd2_3_6.publishOrTestFlag > 1 || MQTTPayloadBuf->payload.cmd2_3_6.totalNum > 10 || MQTTPayloadBuf->payload.cmd2_3_6.uiVersion != MachInfo.other.uiVersion+1){
				//���Ͳ����Ƿ���Ϣ�������
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
		
		//�������Ӧ�����ָ����������߲��԰�ĸ�ģ��汾��
		case 0x00020001:
		{
			//���жϲ����Ƿ�Ƿ�
			if(MQTTPayloadBuf->payload.cmd2_3_2.bootloadVersion > 999 ||  MQTTPayloadBuf->payload.cmd2_3_2.mainVersion > 999 || MQTTPayloadBuf->payload.cmd2_3_2.uiVersion > 999 || MQTTPayloadBuf->payload.cmd2_3_2.publishOrTestFlag >1 ){
				//���Ͳ����Ƿ���Ϣ�������
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
		
		
		//�������Ӧ�����ָ�����߱�ŵĻ�����Ϣ
		case 0x00070001:
		{
			//���жϲ����Ƿ�Ƿ�
			if(strcmp(MQTTPayloadBuf->payload.cmd2_8_2.patientID,(char*)MachRunPara.tNextSample.patientID) != 0 || MQTTPayloadBuf->payload.cmd2_8_2.catTestType != TEST_TYPE_WBC_HGB){
				//���Ͳ����Ƿ���Ϣ�������
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
			
			//���滼����Ϣ
			strcpy((char*)MachRunPara.tNextSample.ucaName,MQTTPayloadBuf->payload.cmd2_8_2.name);
			MachRunPara.tNextSample.ucAge = MQTTPayloadBuf->payload.cmd2_8_2.age;
			MachRunPara.tNextSample.eAgeUnit = MQTTPayloadBuf->payload.cmd2_8_2.ageUnit;
			MachRunPara.tNextSample.eSex = MQTTPayloadBuf->payload.cmd2_8_2.gender;
			strcpy((char*)MachRunPara.tNextSample.orderNum,MQTTPayloadBuf->payload.cmd2_8_2.orderNum);
			
			//�Զ�ƥ��ο���
			MachRunPara.tNextSample.eReferGroup = AgeToRefGroup(MachRunPara.tNextSample.ucAge,MachRunPara.tNextSample.eAgeUnit,MachRunPara.tNextSample.eSex);
			
			//����������Ϣ�ɹ��¼�
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_PATIENT_INFO_OK);
			
			return HAL_OK;
		}
		
		//�������Ӧ�����ָ�����߱�Ų����ڻ�����Ϣ
		case 0x00070002:
		{
			//���жϲ����Ƿ�Ƿ�
			if(strcmp(MQTTPayloadBuf->payload.cmd2_8_3.patientID,(char*)MachRunPara.tNextSample.patientID) != 0 || MQTTPayloadBuf->payload.cmd2_8_3.catTestType != TEST_TYPE_WBC_HGB){
				//���Ͳ����Ƿ���Ϣ�������
				_CAT_B_Send_ParaIllegal(SERVICE_CC);
				return HAL_ERROR;
			}
			
			//����������Ϣ�������¼�
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_NO_PATIENT_INFO);
			
			return HAL_OK;
		}
		
		
		
		
//===��������������================================		
		//�������������Ӧ�����ָ�����߱�ŵĻ�����Ϣ
		case 0xA0050001:
		{
			//���жϲ����Ƿ�Ƿ�
			if(MQTTPayloadBuf->payload.cmdA_2_5_2.catTestType != TEST_TYPE_WBC_HGB || MQTTPayloadBuf->payload.cmdA_2_5_2.pageNum != MachRunPara.patientList.pageNum){
				//���Ͳ����Ƿ���Ϣ�������
				_CAT_B_Send_ParaIllegal(SERVICE_OTHER);
				return HAL_ERROR;
			}
			
			//�ж��Ƿ񲻴��ڻ�����Ϣ
			if(MQTTPayloadBuf->payload.cmdA_2_5_2.num == 0){
				//����������Ϣ�������¼�
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_NO_PATIENT_INFO);
				return HAL_OK;
			}
			
			//���滼����Ϣ�б�
			MachRunPara.patientList.num = MQTTPayloadBuf->payload.cmdA_2_5_2.num;
			memcpy((uint8_t*)MachRunPara.patientList.info,(uint8_t*)MQTTPayloadBuf->payload.cmdA_2_5_2.info,sizeof(MachRunPara.patientList.info));
			
			//����������Ϣ�ɹ��¼�
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_PATIENT_INFO_OK);
			
			return HAL_OK;
		}
		
		//�����ȷ���յ�������־��Ϣ
		case 0xA0020001:
		{
			//���жϲ����Ƿ�Ƿ�
			if(MQTTPayloadBuf->payload.cmdA_2_4_2.catTestType != TEST_TYPE_WBC_HGB || MQTTPayloadBuf->payload.cmdA_2_4_2.logSN != MachRunPara.currSN.logSN){
				//���Ͳ����Ƿ���Ϣ�������
				_CAT_B_Send_ParaIllegal(SERVICE_OTHER);
				return HAL_ERROR;
			}
			
			return HAL_OK;
		}
		
		//�����ȷ���յ��������������Ϣ
		case 0xA0010001:
		{
			//���жϲ����Ƿ�Ƿ�
			if(MQTTPayloadBuf->payload.cmdA_2_3_3.catTestType != TEST_TYPE_WBC_HGB || MQTTPayloadBuf->payload.cmdA_2_3_3.sampleSN != MachRunPara.currSN.sampleSN){
				//���Ͳ����Ƿ���Ϣ�������
				_CAT_B_Send_ParaIllegal(SERVICE_OTHER);
				return HAL_ERROR;
			}
			
			return HAL_OK;
		}
		
		default :return HAL_ERROR;
	}
}







//===�������ݵ������=======================================================================
/***
*����MQTT���ӣ�������뽫�������к�¼���Ż����Ӻ�̨��ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
***/
HAL_StatusTypeDef CAT_B_CreateMQTTConn(CAT_SERVICE_e CatService)
{
	extern MachInfo_s MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osThreadId_t CATTaskHandle;
	char StrTemp[50];
	uint16_t TempLen = 0;
	
	//���ж��������к��Ƿ��Ѿ�¼�룬����ֱ���˳�������ERROR
	while(strlen((char*)MachInfo.labInfo.ucaMachineSN) < 12){
		CATGlobalStatus.flag.connServerErr = 1;
		osThreadSuspend(CATTaskHandle);
		CATGlobalStatus.flag.connServerErr = 0;
	}
	
	TempLen = sprintf(StrTemp,"C/WM/%s",MachInfo.labInfo.ucaMachineSN);
	StrTemp[TempLen] = '\0';
	
	//����MQTT����
	if(CatService == SERVICE_CC){
		//���������
		if(CAT_M_ConfigClient(CatService,(char*)MachInfo.labInfo.ucaMachineSN,MQTT_SERVICE_USER_NAME,MQTT_SERVICE_PASSWORD,StrTemp,"Offline") != HAL_OK){
			return HAL_ERROR;
		}
	}else{
		//�����������
		if(CAT_M_ConfigClient(CatService,(char*)MachInfo.labInfo.ucaMachineSN,MachInfo.companyInfo.mqtt.name,MachInfo.companyInfo.mqtt.psw,StrTemp,"Offline") != HAL_OK){
			return HAL_ERROR;
		}
	}
	
	//�ƿ�CAT����Ҫ����ָ���
	if(CATGlobalStatus.catType == CAT_YK){
		//���ӷ����
		if(CAT_M_CreateTcpConn(MQTT_SERVICE_IP,MQTT_SERVICE_PORT) != HAL_OK){
			return HAL_ERROR;
		}
		if(CAT_M_CreateMQTTConn() != HAL_OK){
			return HAL_ERROR;
		}
	}
	
	//��������
	TempLen = sprintf(StrTemp,"S/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	StrTemp[TempLen] = '\0';
	if(CAT_M_SubTopic(StrTemp) != HAL_OK){
		return HAL_ERROR;
	}
	
	return HAL_OK;
}




/***
*�Ͽ�MQTT���ӣ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
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
*ѯ�ʷ�����Ƿ������ݷ��͸��豸���л�����Ȩ������ˡ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0001;
	MQTTPayloadBuf.cmdM = 0x04;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 0;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;

	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}

	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����Ϣ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);

		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
							
				//�������˷��͵�������Ϣ
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
*�豸�������в�����Ϣ������ˣ�ע�⣬���ڴ�ָ�����������ã��������ﲻ��ȡ���ͷ�TX�ź�����
��������һ������������ȡ���ɡ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//ת���ַ����㷨�汾Ϊ����ֵ
	Len = sprintf(StrTemp,"%s",version());
	for(i=0;i<Len;i++){
		if(StrTemp[i] >= '0' && StrTemp[i] <= '9'){
			AlgoVersion = AlgoVersion*10+(StrTemp[i]-'0');
		}
	}
	
	//��ȡ��ǰʱ���
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		return HAL_ERROR;
	}
	
	return HAL_OK;
}




/***
*��������������Ϣ������ˣ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0001;
	MQTTPayloadBuf.cmdM = 0x01;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 0;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,2,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
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
*����������ǰ������Ϣ������ˣ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����Ϣ
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
*�豸�����˷��͵�ǰ�ź�ǿ�ȣ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0001;
	MQTTPayloadBuf.cmdM = 0x03;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 1;
	
	MQTTPayloadBuf.payload.u8 = CATGlobalStatus.signalStrength;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}





/***
*�豸�����˷����޸��˲�����������ָ�ָ��2.2.9����ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,2,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*�豸�����˷���������������ָ�ָ��2.6.1����ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0005;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmd2_6_1); 
	
	//���������������
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*�豸�����˷����޸�������������ָ�ָ��2.6.2����ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0005;
	MQTTPayloadBuf.cmdM = 0x01;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmd2_6_2); 
	
	//���������������
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}




/***
*�豸�����˷���������������ָ�ָ��2.7.1����ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0006;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;

	//�ж��Ƿ����ƿ�CAT�����һ֡����4096��������־����С��3950��
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

	
	//���������������
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*�豸�����˷���������������ָ�ָ��2.7.2����ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//���ж���־�����Ƿ�����������ֱ�ӷ���HAL_OK�����������ݼ���
	if(RunLog->logHead.logLen > 4100){
		return HAL_OK;
	}
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0006;
	MQTTPayloadBuf.cmdM = 0x01;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = RunLog->logHead.logLen+sizeof(MQTTPayloadBuf.payload.cmd2_7_2);
	
	//���������������
	MQTTPayloadBuf.payload.cmd2_7_2.account = RunLog->logHead.eAccountType;
	MQTTPayloadBuf.payload.cmd2_7_2.timestamp = RunLog->logHead.timestamp;
	
	memcpy(MQTTPayloadBuf.payload.cmd2_7_2.logBuf,RunLog->Str,RunLog->logHead.logLen);
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}




/***
*����������ָ���ĳ����ļ���Ϣ��ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
*������
	PublishOrTestFlag��������Ƿ����滹�ǲ��԰棬Ϊ0��ʾ���󷢲�����Ϣ��Ϊ1��ʾ������԰���Ϣ
	SoftwareType����������������ļ��������������ļ���Ϊ0��ʾ�����������ļ���Ϊ1��ʾ�������������ļ�
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
	
	//�жϲ����Ƿ�Ϸ�
	if(PublishOrTestFlag > 1 || SoftwareType > 1){
		return HAL_ERROR;
	}
	
CAT_B_Send_RespondSoftwareInfoTag:
	TryCount++;
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����Ϣ
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
*����������ָ���汾��UI��Դ��ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
*������
	PublishOrTestFlag��������Ƿ����滹�ǲ��԰棬Ϊ0��ʾ���󷢲�����Ϣ��Ϊ1��ʾ������԰���Ϣ
	UIVersion�������UI��Դ�汾��
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
	
	//�жϲ����Ƿ�Ϸ�
	if(PublishOrTestFlag > 1){
		return HAL_ERROR;
	}
	
CAT_B_Send_RespondUIInfoTag:
	TryCount++;
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����Ϣ
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
*�豸���������Լ�״̬���������ˣ�ָ��2.2.9��ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//�жϲ����Ƿ�Ϸ�
	if(Len == 0){
		return HAL_ERROR;
	}
	
	TryCount++;
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0001;
	MQTTPayloadBuf.cmdM = 0x06;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = Len;
	
	memcpy(MQTTPayloadBuf.payload.str,Buf,Len);
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}




/***
*�������������µķ�������߲��԰�ĸ��汾�ţ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
*������
	PublishOrTestFlag��������Ƿ����滹�ǲ��԰棬Ϊ0��ʾ���󷢲�����Ϣ��Ϊ1��ʾ������԰���Ϣ
	UIVersion�������UI��Դ�汾��
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
	
	//�жϲ����Ƿ�Ϸ�
	if(PublishOrTestFlag > 1){
		return HAL_ERROR;
	}
	
CAT_B_Send_RespondAllSoftwareVersionTag:
	TryCount++;
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0x0002;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 1;
	
	MQTTPayloadBuf.payload.u8 = PublishOrTestFlag;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����Ϣ
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
*����������ָ�����߱�ŵĻ�����Ϣ��ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
*������
	PatientID�����߱��
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
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);

	TryCount++;
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����Ϣ
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
				//�����쳣
				return HAL_ERROR;
			}else{
				goto CAT_B_Send_RequestPatientInfoTag;
			}
		}
	}
}







//===FTP���===Ϊ�˷�ֹ����й©��ֻҪ��������ERROR�����������=================================
/***
*����FTP�������˿ں�
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
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CFTPPORT=%u\r",FTP_SERVICE_PORT);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,5000ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�ж��յ�����Ӧ�Ƿ���ȷ
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//��ʱ����CATͨѶʧ��
			if(TryCount == 3){
				//����ȫ���¼�
				return HAL_TIMEOUT;
			}
			
			goto CAT_FTPSetPortTag;
		}
	}
}



/***
*����FTP������IP��������
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
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CFTPSERV=\"%s\"\r",FTP_SERVICE_IP);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,5000ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�ж��յ�����Ӧ�Ƿ���ȷ
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//��ʱ����CATͨѶʧ��
			if(TryCount == 3){
				//����ȫ���¼�
				return HAL_TIMEOUT;
			}
			
			goto CAT_FTPSetIPTag;
		}
	}
}



/***
*����FTP��������¼��
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
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CFTPUN=\"%s\"\r",FTP_SERVICE_USER_NAME);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,5000ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�ж��յ�����Ӧ�Ƿ���ȷ
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//��ʱ����CATͨѶʧ��
			if(TryCount == 3){
				//����ȫ���¼�
				return HAL_TIMEOUT;
			}
			
			goto CAT_FTPSetLoginNameTag;
		}
	}
}



/***
*����FTP��������¼����
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
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CFTPPW=\"%s\"\r",FTP_SERVICE_PASSWORD);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,5000ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,5000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�ж��յ�����Ӧ�Ƿ���ȷ
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//��ʱ����CATͨѶʧ��
			if(TryCount == 3){
				//����ȫ���¼�
				return HAL_TIMEOUT;
			}
			
			goto CAT_FTPSetLoginPswTag;
		}
	}
}



/***
*�ƿ�CAT���ò�����FTP�ļ�����
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	CATSendCmdBuf.writeIndex = 0;
	
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT+CFTPGET=\"%s\",%u\r",Path,FileOffset);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����Ϣ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);
		if(xReturn == osOK){
			while(1){
				ReStrIndex = _CAT_MyStrCmp(0,"+CFTPGET:DATA,");
				if(ReStrIndex != CAT_RECE_BUF_NO_STR){
					TimeoutCount = 0;
					
					if(_CAT_MyStrCmp(ReStrIndex,"\r\n") != CAT_RECE_BUF_NO_STR){
						ReStrIndex = (ReStrIndex+13)&CAT_RECE_BUF_SIZE;
						
						TempLen=0;
						
						//��ȡ�����ݳ���ֵ
						for(i=0;i<8;i++){
							ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
							
							if(CATReceBuf.buf[ReStrIndex] == '\r'){
								break;
							}
							
							TempLen = TempLen*10+(CATReceBuf.buf[ReStrIndex]-'0');
						}
						
						
						
						//�������ݵ�������
						while(_CAT_MyStrCpy((ReStrIndex+2)&CAT_RECE_BUF_SIZE,Buf+(*BufLen),TempLen) != HAL_OK){
							TimeoutCount++;
						
							//��ֹ����
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
				
				//��ȡ�����ݳ���ֵ
				for(i=0;i<20;i++){
					ReStrIndex = (ReStrIndex+1)&CAT_RECE_BUF_SIZE;
					
					if(CATReceBuf.buf[ReStrIndex] == '\r'){
						break;
					}
					
					TotalLen = TotalLen*10+(CATReceBuf.buf[ReStrIndex]-'0');
				}
				
				//�ж��ܳ����Ƿ����
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
*��������CATģ������FTP����˲���
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
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT^FTPOPEN=\"%s:%u\",\"%s\",\"%s\",1,180,1\r",FTP_SERVICE_IP,FTP_SERVICE_PORT,FTP_SERVICE_USER_NAME,FTP_SERVICE_PASSWORD);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,5000ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�ж��յ�����Ӧ�Ƿ���ȷ
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//��ʱ����CATͨѶʧ��
			if(TryCount == 3){
				//����ȫ���¼�
				return HAL_TIMEOUT;
			}
			
			goto ZY_CAT_FTPSetServerPara;
		}
	}
}




/***
*��������CATģ������FTP�����ļ�����
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
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TryCount++;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT^FTPGETSET=\"%s\",%u,0,0\r",Path,FileOffset);
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	while(1){
		//�ȴ������жϴ������ͷŵĶ�ֵͬ���ź���,5000ms��ʱ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�ж��յ�����Ӧ�Ƿ���ȷ
			ReStrIndex = _CAT_MyStrCmp(0,"\r\nOK\r\n");
			
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}
		}else{
			osSemaphoreRelease(CATSemphrCountTxHandle);
			
			//��ʱ����CATͨѶʧ��
			if(TryCount == 3){
				//����ȫ���¼�
				return HAL_TIMEOUT;
			}
			
			goto ZY_CAT_FTPSetFilePara;
		}
	}
}



/***
*����CAT���ò�����FTP�ļ�����
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
	
	//���������ļ�����
	if(_ZY_CAT_FTPSetFilePara(Path,FileOffset) != HAL_OK){
		return HAL_ERROR;
	}
	
ZY_CAT_FTPGetFileTag:
	TryCount++;
	FTPDownLoadData->totalLen = 0;
	FTPDownLoadData->tempTotalLen = 0;
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT^FTPGET=1\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	TimeoutCount = 0;
	
	while(1){
		//�ȴ�����˷�����Ϣ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,10000);
		if(xReturn == osOK){
			//�ж������Ƿ���������
			ReStrIndex = _CAT_MyStrCmp(0,"^FTPGET:1,1");
			if(ReStrIndex != CAT_RECE_BUF_NO_STR){
				CATReceBuf.readIndex = ReStrIndex;
				
				//�Ƚ�����ȫ�����ص�FTP���ؽṹ���е���ʱBUF�б������������ں�����ȡ����
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
						
						//�ж����ص����ݳ����Ƿ��Ѿ�������
						if(FTPDownLoadData->tempTotalLen > FTP_TEMP_DOWNLOAD_BUF_LEN){
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}
					}
				}
				
				//�����ص���������ȡ��������
				TempStr = (char*)FTPDownLoadData->tempData;
				while(1){
					TempStr = strstr(TempStr,"^FTPGET:2,");
					
					if(TempStr != NULL){
						TempLen = 0;
						TempStr += 10;
						
						//��ȡ�����ݳ���ֵ
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
							//���������쳣���˳�
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}
						
						//�ж����ݳ���
						if(TempLen == 0){
							//ȫ��������ȡ��ɣ���������
							break;
						}else if(TempLen <= 4096){
							//��������
							TempStr += 2;
							
							while(TempLen--){
								FTPDownLoadData->data[FTPDownLoadData->totalLen++] = *TempStr++;
							}
						}else{
							//���������쳣���˳�
							osSemaphoreRelease(CATSemphrCountTxHandle);
							return HAL_ERROR;
						}
					}else{
						//û�������յ�����ָ��^FTPGET:2,0�������ж�Ϊ�����쳣
						osSemaphoreRelease(CATSemphrCountTxHandle);
						return HAL_ERROR;
					}
				}
				
				osSemaphoreRelease(CATSemphrCountTxHandle);
				return HAL_OK;
			}else if(_CAT_MyStrCmp(0,"^FTPGET:1,0") != CAT_RECE_BUF_NO_STR){
				//����FTP����ʧ��
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
*����CATֹͣFTP�ļ�����
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	CATSendCmdBuf.writeIndex = 0;
	
	TempLen = sprintf(CATSendCmdBuf.buf+CATSendCmdBuf.writeIndex,"AT^FTPGET=2,0\r");
	CATSendCmdBuf.writeIndex += TempLen;
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	osKernelLock();
	HAL_UART_Transmit(&huart4,(uint8_t*)CATSendCmdBuf.buf,CATSendCmdBuf.writeIndex,100);
	osKernelUnlock();
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	
	return HAL_OK;
}





/***
*����FTP�����������ز���HAL_OKʱ���򽫵����������
***/
HAL_StatusTypeDef CAT_ConnFTP(void)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	
	if(CATGlobalStatus.flag.ftpConn == 1){
		return HAL_OK;
	}
	
	//���ݲ�ͬ��CAT����ִ�в�ͬ��ָ��
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
*����ָ��·���µ��ļ�
*������
	Buf�����ص��ļ����滺��
	BufLen�����ص��ļ��ܴ�С
	Path����FTP�е�����·��
	FileOffset��ָ�����ļ���ʼ���ص�ƫ������Ϊ0ʱ��ʾ���ļ���ͷ������
���ز���HAL_OKʱ���򽫵����������
***/
HAL_StatusTypeDef CAT_FTPGetFile(FTPDownloadData_s* FTPDownLoadData,char* Path,uint32_t FileOffset)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	HAL_StatusTypeDef xReturn = HAL_OK;
	
	//���ݲ�ͬCAT����ִ�в�ͬ��ָ��
	if(CATGlobalStatus.catType == CAT_YK){
		xReturn =  _YK_CAT_FTPGetFile(FTPDownLoadData->data,&FTPDownLoadData->totalLen,Path,FileOffset);
	}else{
		xReturn = _ZY_CAT_FTPGetFile(FTPDownLoadData,Path,FileOffset);
		if(xReturn != HAL_OK){
			//FTP����ʧ��ʱ����Ҫ����ֹͣ����ָ��
			ZY_CAT_FTPStopDownload();
		}
	}
	
	return xReturn;
}












//***�����������˽�������ͨѶ��*************************************************************
/***
*����������ǰ������Ϣ������������ˣ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*�豸�����������˷��͵�ǰ�ź�ǿ�ȣ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA000;
	MQTTPayloadBuf.cmdM = 0x01;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 1;
	
	MQTTPayloadBuf.payload.u8 = CATGlobalStatus.signalStrength;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}




/***
*�豸���������Լ�״̬���������������ˣ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//�жϲ����Ƿ�Ϸ�
	if(Len == 0){
		return HAL_ERROR;
	}
	
	TryCount++;
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA000;
	MQTTPayloadBuf.cmdM = 0x02;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = Len;
	
	memcpy(MQTTPayloadBuf.payload.str,Buf,Len);
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*�豸�����������˷�����������������־��ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA002;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmdA_2_4_1);
	
	//���������������
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	MachRunPara.currSN.logSN = WBCHGBTestInfo->ulCurDataSN+1;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
	
//	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
//	osSemaphoreRelease(CATSemphrBinRxHandle);
//	
//	while(1){
//		//�ȴ�����˷�����Ϣ
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
*�豸�����˷���������������ָ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA001;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmdA_2_3_1); 
	
	//���������������
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	MachRunPara.currSN.sampleSN = WBCHGBTestInfo->ulCurDataSN+1;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
	
//	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
//	osSemaphoreRelease(CATSemphrBinRxHandle);
//	
//	while(1){
//		//�ȴ�����˷�����Ϣ
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
*��������������Ϣ��ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
*������
	PageNum��Ҫ����Ļ�����Ϣҳ��
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
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);

	TryCount++;
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����Ϣ
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
*ѯ�ʵ�����������Ƿ������ݷ��͸��豸���л�����Ȩ������ˡ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ��������
��Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA004;
	MQTTPayloadBuf.cmdM = 0x00;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = 0;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;

	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}

	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����Ϣ
		xReturn = osSemaphoreAcquire(CATSemphrBinRxHandle,30000);

		if(xReturn == osOK){
			if(_CAT_M_CopyReceData((uint8_t*)&MQTTPayloadBuf,&TotalLen) == HAL_OK){
				osSemaphoreRelease(CATSemphrCountTxHandle);
							
				//�������˷��͵�������Ϣ
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
*�豸��Ӧ�����������������������������õ�ָ��
*������
	InfoType����ʾ��������
	Flag��Ϊ1��ʾ���ĳɹ���Ϊ0��ʾ����ʧ��
*����ֵ������HAL_OK��ʾ�����Ӧ��HAL_ERROR��ʾ��Ӧʧ��
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
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
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf->payloadLen+14,(uint8_t*)MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	//�����ֶ��ͷ�һ�ν����ź������Ա������ǰ�ȼ���Ա��Ƿ��Ѿ����յ�����˶�Ӧ����Ӧ��
	osSemaphoreRelease(CATSemphrBinRxHandle);
	
	while(1){
		//�ȴ�����˷�����һ��������Ϣ
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
*�豸�����������˷��ͼ�����ʼ���ԵĻ��߶�����ţ�ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA005;
	MQTTPayloadBuf.cmdM = 0x01;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmdA_2_5_3);
	
	//���������������
	MQTTPayloadBuf.payload.cmdA_2_5_3.catTestType = TEST_TYPE_WBC_HGB;
	strcpy(MQTTPayloadBuf.payload.cmdA_2_5_3.orderNum,OrderNum);
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}



/***
*�豸�����������˷��������ж���ָ��������ֻҪ����ֵ��ΪHAL_OKʱ��ʾ��һ����������Ҫ���������
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
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
	
	//��λMQTTPayloadBuf�ڴ棬��ֹ�����쳣����
	memset((uint8_t*)&MQTTPayloadBuf,0,sizeof(MQTT_PayloadBuf_s));
	
	//��ȡ��ǰʱ���
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	MQTTPayloadBuf.cmdH = 0xA000;
	MQTTPayloadBuf.cmdM = 0x03;
	MQTTPayloadBuf.cmdL = 0x00;
	MQTTPayloadBuf.cmdTimestamp = GetCurrentTimestamp(RTCHym8563Info);
	MQTTPayloadBuf.packetNum = 0;
	MQTTPayloadBuf.payloadLen = sizeof(MQTTPayloadBuf.payload.cmdA_2_2_4);
	
	//���������������
	MQTTPayloadBuf.payload.cmdA_2_2_4.interruptSignalStrength = MachRunPara.netStatus.interruptSignalStrength;
	MQTTPayloadBuf.payload.cmdA_2_2_4.interruptTimestamp = MachRunPara.netStatus.interruptTimestamp;
	MQTTPayloadBuf.payload.cmdA_2_2_4.recoverySignalStrength = MachRunPara.netStatus.recoverySignalStrength;
	MQTTPayloadBuf.payload.cmdA_2_2_4.recoveryTimestamp = MachRunPara.netStatus.recoveryTimestamp;
	
	MQTTPayloadBuf.crc16 = CRC16((uint8_t*)&MQTTPayloadBuf.payloadLen,MQTTPayloadBuf.payloadLen+2);
	
	//�����ַ���
	sprintf(TopicString,"C/Norm/%s",MachInfo.labInfo.ucaMachineSN);
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(CATSemphrBinRxHandle,0);
	CATReceBuf.readIndex = CATReceBuf.writeIndex;
	
	//������Ϣ
	if(CAT_M_PubTopic(TopicString,1,MQTTPayloadBuf.payloadLen+14,(uint8_t*)&MQTTPayloadBuf) != HAL_OK){
		osSemaphoreRelease(CATSemphrCountTxHandle);
		return HAL_ERROR;
	}
	
	osSemaphoreRelease(CATSemphrCountTxHandle);
	return HAL_OK;
}






