#ifndef _PRINTER_H
#define _PRINTER_H



#include "stm32f4xx_hal.h"
#include "PrinterTask.h"





#define PRINTER_RECE_BUF_LEN					7					//定义打印机接收数据缓冲大小


/***
*定义从打印机中接收数据的循环队列
***/
typedef struct _PrinterReceBuf_s{
	uint8_t buf[PRINTER_RECE_BUF_LEN+1];							//注意这里需要+1
	uint8_t readIndex;
	uint8_t writeIndex;
}PrinterReceBuf_s;



/***
*定义打印机当前的配置状态结构体，保存配置后每行都生效的配置状态
***/
typedef struct _PrinterCurrentState_s{
	uint8_t readyState;								//用于表明打印机当前状态，0x00表示MCU与打印机通讯失败，0x55表示打印机缺纸，0x04表示打印机有纸，并且和MCU通讯正常
	
	uint16_t remainRxBuf;							//用于保存当前打印机中接收缓存的剩余字节数，最多10KB
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
