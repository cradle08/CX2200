#ifndef _PRINTER_H
#define _PRINTER_H



#include "stm32f4xx_hal.h"
#include "PrinterTask.h"





#define PRINTER_RECE_BUF_LEN					7					//�����ӡ���������ݻ����С


/***
*����Ӵ�ӡ���н������ݵ�ѭ������
***/
typedef struct _PrinterReceBuf_s{
	uint8_t buf[PRINTER_RECE_BUF_LEN+1];							//ע��������Ҫ+1
	uint8_t readIndex;
	uint8_t writeIndex;
}PrinterReceBuf_s;



/***
*�����ӡ����ǰ������״̬�ṹ�壬�������ú�ÿ�ж���Ч������״̬
***/
typedef struct _PrinterCurrentState_s{
	uint8_t readyState;								//���ڱ�����ӡ����ǰ״̬��0x00��ʾMCU���ӡ��ͨѶʧ�ܣ�0x55��ʾ��ӡ��ȱֽ��0x04��ʾ��ӡ����ֽ�����Һ�MCUͨѶ����
	
	uint16_t remainRxBuf;							//���ڱ��浱ǰ��ӡ���н��ջ����ʣ���ֽ��������10KB
}PrinterCurrentState_s;





HAL_StatusTypeDef Printer_IsOK(void);
HAL_StatusTypeDef Printer_Init(void);
HAL_StatusTypeDef Printer_ConfigDefault(void);

void PrinterSamplePage(PrinterInfoData_s PrinterInfoData);
void PrinterAgingTestResultPage(PrinterInfoData_s PrinterInfoData);
void PrinterTanZhenTestResultPage(PrinterInfoData_s PrinterInfoData);
void PrinterOutInMotoTestResultPage(PrinterInfoData_s PrinterInfoData);
void PrinterQizuiTestResultPage(PrinterInfoData_s PrinterInfoData);
void PrinterAirValveTestResultPage(PrinterInfoData_s PrinterInfoData);
void PrinterYeFaValveTestResultPage(PrinterInfoData_s PrinterInfoData);
void PrinterBuildPressureTestResultPage(PrinterInfoData_s PrinterInfoData);
void PrinterDLElecTestResultPage(PrinterInfoData_s PrinterInfoData);
void PrinterQCPage(PrinterInfoData_s PrinterInfoData);
void PrinterSCAgingTestResultPage(PrinterInfoData_s PrinterInfoData);


#endif
