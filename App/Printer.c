/********************************************************************
*��ӡ����������
*********************************************************************/
#include "Printer.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "msg_def.h"
#include "file_operate.h"
#include "Parameter.h"
#include "unicode2gbk.h"
#include "Public_menuDLG.h"
#include "Common.h"



  

/*�궨����***********************************************************/
//�����״ֵ̬��Ĭ��ֵ
#define READY_STATE_DEFAULT							0x00
#define USE_HANZI_MODE_DEFAULT						0x00
#define GRAY_DEFAULT								0x0A
#define LINE_SPACING_DEFAULT						0x03
#define RIGHT_SPACING_DEFAULT						0x03
#define FONT_SIZE_DEFAULT							0x00
#define FONT_SIZE_W_MULTI_DEFAULT					0x01
#define FONT_SIZE_H_MULTI_DEFAULT					0x01
#define BOLD_DEFAULT								0x00
#define UNDERLINE_DEFAULT							0x00
#define OVERLINE_DEFAULT							0x00
#define CHARTERSET_DEFAULT							0x01
#define TABLE_FLAG_DEFAULT							0x00


//���巢�͸���ӡ���Ļ�������С
#define PRINTER_SEND_BUF_SIZE						2048

#define LCD_PIXEL_WIDTH       						800									
#define LCD_PIXEL_HEIGHT      						480									


//�������ӡ����ͼ�������С�����ص�
#define PRINTER_DRAW_SIZE_W							384					
#define PRINTER_DRAW_SIZE_H							220



//�����������ӡ����λͼ�ֽ����ݵ�SDRAM������ʼ��ַ
#define PRINTER_PICTURE_SDRAM_START_ADR				(SDRAM_PRINTER_ADDR+LCD_PIXEL_WIDTH*(PRINTER_DRAW_SIZE_H+1)*2)


//������ɫֵ
#define LCD_COLOR_RGB565_WHITE           			0xFFFF        //11111 111111 11111
#define LCD_COLOR_RGB565_BLACK           			0x0000        //00000 000000 00000
#define LCD_COLOR_RGB565_GREY            			0xF7DE        //11110 111110 11110
#define LCD_COLOR_RGB565_BLUE            			0x001F        //00000 000000 11111
#define LCD_COLOR_RGB565_RED             			0xF800        //11111 000000 00000
#define LCD_COLOR_RGB565_GREEN           			0x07E0        //00000 111111 00000
#define LCD_COLOR_RGB565_YELLOW          			0xFFE0        //11111 111111 00000
		
#define LCD_COLOR_ARGB1555_WHITE          			0xFFFF        //1 11111 11111 11111
#define LCD_COLOR_ARGB1555_BLACK          			0x8000        //1 00000 00000 00000
#define LCD_COLOR_ARGB1555_GREY           			0xFBFE        //1 11110 11111 11110
#define LCD_COLOR_ARGB1555_BLUE           			0x801F        //1 00000 00000 11111
#define LCD_COLOR_ARGB1555_RED            			0xFC00        //1 11111 00000 00000
#define LCD_COLOR_ARGB1555_GREEN          			0x83E0        //1 00000 11111 00000
#define LCD_COLOR_ARGB1555_YELLOW         			0xFFE0        //1 11111 11111 00000
		
		
/*����Ĭ�ϳ��ô�ɫ��Ӧ��ARGB8888*/		
#define LCD_COLOR_ARGB8888_WHITE          			0xFFFFFFFF
#define LCD_COLOR_ARGB8888_BLACK          			0xFF000000
#define LCD_COLOR_ARGB8888_BLUE           			0xFF0000FF
#define LCD_COLOR_ARGB8888_RED            			0xFFFF0000
#define LCD_COLOR_ARGB8888_GREEN          			0xFF00FF00











/***
*���巢�͸���ӡ�����ݵĻ�����
***/
typedef struct _Printer_SendBuf_s{
	uint8_t buf[PRINTER_SEND_BUF_SIZE];
	uint16_t writeIndex;
}Printer_SendBuf_s;






/*ȫ�ֱ���������*****************************************************/
PrinterCurrentState_s PrinterCurrentState = {0};
Printer_SendBuf_s PrinterSendBuf = {0};
PrinterReceBuf_s PrinterReceBuf = {0};



/*�ڲ�������*********************************************************/
/***
*�Զ����ַ�����ȡ���Ⱥ��������ڸ�CATģ���ATָ��ʹ��
*����ֵ�����ص�ǰ���ջ���������յ������ݳ���ֵ
***/
static uint16_t _CAT_MyStrLen(void)
{
	extern PrinterReceBuf_s PrinterReceBuf;
	uint16_t TempLen = 0;
	
	if(PrinterReceBuf.readIndex <= PrinterReceBuf.writeIndex){
		TempLen = PrinterReceBuf.writeIndex - PrinterReceBuf.readIndex;
	}else{
		TempLen = (PRINTER_RECE_BUF_LEN+1)- PrinterReceBuf.readIndex + PrinterReceBuf.writeIndex;
	}
	
	return TempLen;
}




/***
*ʹ��DMA2D��ʽ����ָ����--ʹ�ô�ɫ���ģʽ
������
	CurrentLayer��Ҫ�����Ĳ�
	Color��������������ɫARGB8888ֵ��DMA2D�ڲ����Զ������趨����ɫģʽת�������Ӧ����ɫֵ
***/
static void _LCD_ClearAll(uint32_t Color)
{
	extern DMA2D_HandleTypeDef hdma2d;

	//������Ȼ��������
	HAL_DMA2D_DeInit(&hdma2d);
	
	//����DMA2D
	hdma2d.Init.Mode         = DMA2D_R2M;
	hdma2d.Init.ColorMode    = DMA2D_RGB565;
	hdma2d.Init.OutputOffset = 0;     							//��ƫ����
//	hdma2d.XferCpltCallback  = TransferComplete;				//ʹ��IT��ʽ��������ʱ������Ҫ�����ص�����
//	hdma2d.XferErrorCallback = TransferError;
	hdma2d.Instance          = DMA2D;    

	/* DMA2D Initialization */
	if(HAL_DMA2D_Init(&hdma2d) != HAL_OK) 
	{
		printf("DMA2D is ERR\r\n");
	}

	//��ʼ���䣬�������ɫ��ʽ����ΪARGB8888��������Զ��������Ҫ�ĸ�ʽ
	HAL_DMA2D_Start(&hdma2d,Color,SDRAM_PRINTER_ADDR,800,480);

	//�ȴ�ִ�����
	while(__HAL_DMA2D_GET_FLAG(&hdma2d,DMA2D_FLAG_TC) == RESET);
}


/***
*������Ƕȵ�ʵֱ��
*������
	StartX����ʼ��X����������
	StartY����ʼ��Y����������
	EndX���յ�X����������
	EndY���յ�Y����������
***/
static void _LCD_DrawLine(uint16_t StartX,uint16_t StartY,uint16_t EndX,uint16_t EndY,uint16_t Color)
{
	uint32_t CurrPixelIndex;					//���ڱ�ʾ��ǰָ�����������ֵ
	uint16_t Dy = 0;
	uint16_t Dx = 0;
	uint16_t X = 0;
	uint16_t Y = 0;
	int8_t Xadd1 = 0;
	int8_t Xadd2 = 0;
	int8_t Yadd1 = 0;
	int8_t Yadd2 = 0;
	uint16_t Count = 0;
	uint16_t OverFlagValue = 0;
	uint16_t Sum = 0;
	uint16_t OffsetAdd = 0;
	uint16_t i = 0;
	
	
	//�����Ϸ����ж�
	StartX = StartX>=LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH-1:StartX;
	StartY = StartY>=LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT-1:StartY;
	EndX = EndX>=LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH-1:EndX;
	EndY = EndY>=LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT-1:EndY;
	
	
	Dy = EndY>StartY?(EndY-StartY):(StartY-EndY);
	Dx = EndX>StartX?(EndX-StartX):(StartX-EndX);
	X = StartX;
	Y = StartY;
	
	//�ж�x���y�����������
	if(EndX >= StartX){
		Xadd1 = 1;
		Xadd2 = 1;
	}else{
		Xadd1 = -1;
		Xadd2 = -1;
	}
	
	if(EndY >= StartY){
		Yadd1 = 1;
		Yadd2 = 1;
	}else{
		Yadd1 = -1;
		Yadd2 = -1;
	}
	
	//�ж����ĸ�����Ϊ������
	if(Dx >= Dy){
		Xadd1 = 0;
		Yadd2 = 0;
		
		Count = Dx;
		OverFlagValue = Dx;
		Sum = Dx>>1;
		OffsetAdd = Dy;
	}else{
		Xadd2 = 0;
		Yadd1 = 0;
		
		Count = Dy;
		OverFlagValue = Dy;
		Sum = Dy>>1;
		OffsetAdd = Dx;
	}
	
	for(i=0;i<=Count;i++){
		CurrPixelIndex = Y*LCD_PIXEL_WIDTH + X;
		*(((uint16_t*)SDRAM_PRINTER_ADDR)+CurrPixelIndex) = Color;
		
		Sum += OffsetAdd;
		if(Sum >= OverFlagValue){
			Sum -= OverFlagValue;
			
			X += Xadd1;
			Y += Yadd1;
		}
		
		X += Xadd2;
		Y += Yadd2;
	}
}



/***
*������Ƕȵ���ֱ��
*������
	StartX����ʼ��X����������
	StartY����ʼ��Y����������
	EndX���յ�X����������
	EndY���յ�Y����������
***/
static void _LCD_DrawBrokenLine(uint16_t StartX,uint16_t StartY,uint16_t EndX,uint16_t EndY,uint16_t FColor,uint16_t BColor)
{
	uint32_t CurrPixelIndex;					//���ڱ�ʾ��ǰָ�����������ֵ
	uint16_t Dy = 0;
	uint16_t Dx = 0;
	uint16_t X = 0;
	uint16_t Y = 0;
	int8_t Xadd1 = 0;
	int8_t Xadd2 = 0;
	int8_t Yadd1 = 0;
	int8_t Yadd2 = 0;
	uint16_t Count = 0;
	uint16_t OverFlagValue = 0;
	uint16_t Sum = 0;
	uint16_t OffsetAdd = 0;
	uint16_t i = 0;
	uint8_t k = 0;
	
	
	//�����Ϸ����ж�
	StartX = StartX>=LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH-1:StartX;
	StartY = StartY>=LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT-1:StartY;
	EndX = EndX>=LCD_PIXEL_WIDTH?LCD_PIXEL_WIDTH-1:EndX;
	EndY = EndY>=LCD_PIXEL_HEIGHT?LCD_PIXEL_HEIGHT-1:EndY;
	
	
	Dy = EndY>StartY?(EndY-StartY):(StartY-EndY);
	Dx = EndX>StartX?(EndX-StartX):(StartX-EndX);
	X = StartX;
	Y = StartY;
	
	//�ж�x���y�����������
	if(EndX >= StartX){
		Xadd1 = 1;
		Xadd2 = 1;
	}else{
		Xadd1 = -1;
		Xadd2 = -1;
	}
	
	if(EndY >= StartY){
		Yadd1 = 1;
		Yadd2 = 1;
	}else{
		Yadd1 = -1;
		Yadd2 = -1;
	}
	
	//�ж����ĸ�����Ϊ������
	if(Dx >= Dy){
		Xadd1 = 0;
		Yadd2 = 0;
		
		Count = Dx;
		OverFlagValue = Dx;
		Sum = Dx>>1;
		OffsetAdd = Dy;
	}else{
		Xadd2 = 0;
		Yadd1 = 0;
		
		Count = Dy;
		OverFlagValue = Dy;
		Sum = Dy>>1;
		OffsetAdd = Dx;
	}
	
	for(i=0;i<=Count;i++){
		k = (k+1)&0x07;
		CurrPixelIndex = Y*LCD_PIXEL_WIDTH + X;
		
		if(k<4){
			*(((uint16_t*)SDRAM_PRINTER_ADDR)+CurrPixelIndex) = FColor;
		}else{
			*(((uint16_t*)SDRAM_PRINTER_ADDR)+CurrPixelIndex) = BColor;
		}
		
		Sum += OffsetAdd;
		if(Sum >= OverFlagValue){
			Sum -= OverFlagValue;
			
			X += Xadd1;
			Y += Yadd1;
		}
		
		X += Xadd2;
		Y += Yadd2;
	}
}



/***
*������ֽ��
***/
static void _Printer_ZouZhi(uint16_t RowNum)
{
	extern Printer_SendBuf_s PrinterSendBuf;
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = RowNum;
}





/*�ⲿ������*********************************************************/
/***
*��ѯ��ӡ���Ƿ�ȱֽ����ָ��Ҳ����MCU�ʹ�ӡ������ͬ������
***/
HAL_StatusTypeDef Printer_IsOK(void)
{
	extern UART_HandleTypeDef huart5;
	extern PrinterCurrentState_s PrinterCurrentState;
	extern Printer_SendBuf_s PrinterSendBuf;
	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
	extern osSemaphoreId_t SemphrBinPrinterRxHandle;
	extern osEventFlagsId_t GlobalEventPrintGroupHandle;
	extern PrinterReceBuf_s PrinterReceBuf;
	osStatus_t xReturn = osOK;
	uint8_t TryCount = 0;
	uint16_t Temp = 0;
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
    osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
	
	//���Ͳ�ѯ��ӡ��״̬��ʣ����ջ���ָ��
	PrinterSendBuf.writeIndex = 0;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1c;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x76;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1c;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x46;
	
Printer_IsOKTag:
	TryCount++;	
	
	//������ջ���ĸ�������
	osSemaphoreAcquire(SemphrBinPrinterRxHandle,0);
	PrinterReceBuf.readIndex = PrinterReceBuf.writeIndex;
	
	//����ָ��
	osKernelLock();
	HAL_UART_Transmit(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex,200);
	osKernelUnlock();
	
	while(1){
		//�ȴ����յ���ӡ�����ݵĶ�ֵͬ���ź���
		xReturn = osSemaphoreAcquire(SemphrBinPrinterRxHandle,pdMS_TO_TICKS(1000));
		
		//�ж��Ƿ��յ���Ӧ
		if(xReturn == osOK){
			//�жϽ��յ����ݳ����Ƿ��㹻
			if(_CAT_MyStrLen() >= 3){
				switch(PrinterReceBuf.buf[PrinterReceBuf.readIndex]){
					case 0x55:		//ȱֽ
						//���µ�ǰ��ӡ��ʣ����ջ���
						Temp = PrinterReceBuf.buf[(PrinterReceBuf.readIndex+1)&PRINTER_RECE_BUF_LEN]<<8|PrinterReceBuf.buf[(PrinterReceBuf.readIndex+2)&PRINTER_RECE_BUF_LEN];
						
						//��ӡ���ڲ��������Ϊ8K������ʱ����ֱ�Ӷ�������
						if(Temp > 10240){
							if(TryCount <= 3){
								goto Printer_IsOKTag;
							}else{
								if(PrinterCurrentState.readyState != 0x00){
									PrinterCurrentState.readyState = 0x00;
									osEventFlagsSet(GlobalEventPrintGroupHandle,GLOBAL_EVENT_PRINTER_CONN_ERR);
								}
								
								osSemaphoreRelease(SemphrCountPrinterTxHandle);
								return HAL_ERROR;
							}
						}
					
						PrinterCurrentState.remainRxBuf = Temp;
						
						//����ȫ���¼�
						if(PrinterCurrentState.readyState != 0x55){
							PrinterCurrentState.readyState = 0x55;
							osEventFlagsSet(GlobalEventPrintGroupHandle,GLOBAL_EVENT_PRINTER_NO_PAPER);
						}
						
						osSemaphoreRelease(SemphrCountPrinterTxHandle);
						return HAL_OK;
					case 0x04:		//��ֽ
						//���µ�ǰ��ӡ��ʣ����ջ���
						Temp = PrinterReceBuf.buf[(PrinterReceBuf.readIndex+1)&PRINTER_RECE_BUF_LEN]<<8|PrinterReceBuf.buf[(PrinterReceBuf.readIndex+2)&PRINTER_RECE_BUF_LEN];
						
						//��ӡ���ڲ��������Ϊ10K������ʱ����ֱ�Ӷ�������
						if(Temp > 11264){
							if(TryCount <= 3){
								goto Printer_IsOKTag;
							}else{
								if(PrinterCurrentState.readyState != 0x00){
									PrinterCurrentState.readyState = 0x00;
									osEventFlagsSet(GlobalEventPrintGroupHandle,GLOBAL_EVENT_PRINTER_CONN_ERR);
								}
								
								osSemaphoreRelease(SemphrCountPrinterTxHandle);
								return HAL_ERROR;
							}
						}
					
						PrinterCurrentState.remainRxBuf = Temp;
						
						//����ȫ���¼�
						if(PrinterCurrentState.readyState != 0x04){
							PrinterCurrentState.readyState = 0x04;
							osEventFlagsSet(GlobalEventPrintGroupHandle,GLOBAL_EVENT_PRINTER_RECOVER);
						}
						
						osSemaphoreRelease(SemphrCountPrinterTxHandle);
						return HAL_OK;
					default:		//��������
						if(TryCount <= 3){
							goto Printer_IsOKTag;
						}else{
							if(PrinterCurrentState.readyState != 0x00){
								PrinterCurrentState.readyState = 0x00;
								osEventFlagsSet(GlobalEventPrintGroupHandle,GLOBAL_EVENT_PRINTER_CONN_ERR);
							}
							
							osSemaphoreRelease(SemphrCountPrinterTxHandle);
							return HAL_ERROR;
						}
				}
			}
		}else{
			if(TryCount <= 3){
				goto Printer_IsOKTag;
			}else{
				if(PrinterCurrentState.readyState != 0x00){
					PrinterCurrentState.readyState = 0x00;
					osEventFlagsSet(GlobalEventPrintGroupHandle,GLOBAL_EVENT_PRINTER_CONN_ERR);
				}
				
				osSemaphoreRelease(SemphrCountPrinterTxHandle);
				return HAL_TIMEOUT;
			}
		}
	}
}





/***
*���ô�ӡ��ȫ�ֳ�ʼ��״̬
***/
HAL_StatusTypeDef Printer_Init(void)
{
	extern UART_HandleTypeDef huart5;
	extern Printer_SendBuf_s PrinterSendBuf;
	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
	
	osSemaphoreAcquire(SemphrCountPrinterTxHandle,osWaitForever);
	PrinterSendBuf.writeIndex = 0;
	
	//��λ��ӡ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x40;
	
	//���ûҶ�Ϊ10
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x6D;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = GRAY_DEFAULT;
	
	//�ߵ���ӡ��ʹ��������������ȴ�ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x7B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	//�����Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	//����ָ��
	osKernelLock();
	HAL_UART_Transmit(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex,100);
	osKernelUnlock();
	
	osSemaphoreRelease(SemphrCountPrinterTxHandle);
	return HAL_OK;
}






/***
*���ô�ӡ��ȫ�ֳ�ʼ��״̬
***/
HAL_StatusTypeDef Printer_ConfigDefault(void)
{
	extern UART_HandleTypeDef huart5;
	extern Printer_SendBuf_s PrinterSendBuf;
	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
	
	osSemaphoreAcquire(SemphrCountPrinterTxHandle,osWaitForever);
	PrinterSendBuf.writeIndex = 0;
	
	//���ûҶ�Ϊ10
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x6D;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = GRAY_DEFAULT;
	
	//�ߵ���ӡ��ʹ��������������ȴ�ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x7B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	//�����Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	//����ָ��
	osKernelLock();
	HAL_UART_Transmit(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex,100);
	osKernelUnlock();
	
	osSemaphoreRelease(SemphrCountPrinterTxHandle);
	return HAL_OK;
}





/***********�����������ݴ�ӡ��Ӣ�Ķ��ձ�*********************************************************/
const char g_ucaLng_Printer_Tip[LANGUAGE_END][40]    = {"���ϼ����������ٴ��ο�\r", "Only for clinical reference\r"};
const char g_ucaLng_Printer_Histogram[LANGUAGE_END][40]    = {"��ϸ���ֲ�ֱ��ͼ\r", "Histogram of leukocyte\r"};
const char g_ucaLng_Printer_XHDB[LANGUAGE_END][40]    = {"Ѫ�쵰��\r", "Hemoglobin\r"};
const char g_ucaLng_Printer_LBXBP[LANGUAGE_END][40]    = {"�ܰ�ϸ���ٷֱ�\r", "Lymphocyte%\r"};
const char g_ucaLng_Printer_ZJQXBP[LANGUAGE_END][40]    = {"�м�Ⱥϸ���ٷֱ�\r", "Intermediate group cell%\r"};
const char g_ucaLng_Printer_ZXLXBP[LANGUAGE_END][40]    = {"������ϸ���ٷֱ�\r", "Neutrophil%\r"};
const char g_ucaLng_Printer_LBXB[LANGUAGE_END][40]    = {"�ܰ�ϸ����Ŀ\r", "Lymphocyte#\r"};
const char g_ucaLng_Printer_ZJQXB[LANGUAGE_END][40]    = {"�м�Ⱥϸ����Ŀ\r", "Intermediate group cell#\r"};
const char g_ucaLng_Printer_ZXLXB[LANGUAGE_END][40]    = {"������ϸ����Ŀ\r", "Neutrophil#\r"};
const char g_ucaLng_Printer_WBC[LANGUAGE_END][20]    = {"��ϸ����Ŀ\r", "WBC#\r"};
const char g_ucaLng_Printer_XM[LANGUAGE_END][20]    = {"��Ŀ", "ITEM"};
const char g_ucaLng_Printer_JG[LANGUAGE_END][20]    = {"���", "RST"};
const char g_ucaLng_Printer_DW[LANGUAGE_END][20]    = {"��λ", "UNIT"};
const char g_ucaLng_Printer_CKZ[LANGUAGE_END][20]    = {"�ο���Χ", "REF"};
const char g_ucaLng_Printer_BJ[LANGUAGE_END][20]    = {"����", "ALA"};
const char g_ucaLng_Printer_XINGM[LANGUAGE_END][20]    = {"��    ��", "NAME"};
const char g_ucaLng_Printer_XB[LANGUAGE_END][20]    = {"��    ��", "GENDER"};
const char g_ucaLng_Printer_AGE[LANGUAGE_END][20]    = {"��    ��", "AGE"};
const char g_ucaLng_Printer_SAMPLENUM[LANGUAGE_END][20]    = {"�������", "NUM"};
const char g_ucaLng_Printer_TIME[LANGUAGE_END][20]    = {"����ʱ��", "TIME"};
const char g_ucaLng_Printer_TITLE[LANGUAGE_END][40]    = {"ѪҺ�������鱨�浥", "Blood analysis report"};
const char g_ucaLng_Printer_NAN[LANGUAGE_END][20]    = {"��", "MALE"};
const char g_ucaLng_Printer_NV[LANGUAGE_END][20]    = {"Ů", "FEMALE"};
const char g_ucaLng_Printer_SUI[LANGUAGE_END][20]    = {"��", "YEAR"};
const char g_ucaLng_Printer_YUE[LANGUAGE_END][20]    = {"��", "MONTH"};
const char g_ucaLng_Printer_ZHOU[LANGUAGE_END][20]    = {"��", "WEEK"};
const char g_ucaLng_Printer_TIAN[LANGUAGE_END][20]    = {"��", "DAY"};


/***
*��ӡ��������ҳ�����ݣ����Ŵ�ӡ˳��
***/
void PrinterSamplePage(PrinterInfoData_s PrinterInfoData)
{
	extern UART_HandleTypeDef huart5;
	extern PrinterCurrentState_s PrinterCurrentState;
	extern Printer_SendBuf_s PrinterSendBuf;
	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
	extern MachInfo_s MachInfo;
	uint16_t TempLen = 0;
	uint16_t i,j,k,q,p;
	uint8_t* TempAdr;
	uint8_t* TempAdr2;
	uint16_t* LcdAdr;	
	uint16_t usUnicode[50] = {0};
    uint16_t usGBK[50] = {0};
	char TempStr[10] = {0};
	
	
	//�Ȼ�ȡһ�δ�ӡ��״̬
	Printer_IsOK();
	
	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
		Printer_IsOK();
        osDelay(500);
	}
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
    osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
	PrinterSendBuf.writeIndex = 0;
	
	//ȡ���ߵ���ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x7B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	//���д�ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	//ȡ���Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_Tip[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ��
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_Histogram[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ��
	_Printer_ZouZhi(2);
	
	//�����Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 7;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 15;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 23;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 31;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"100");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"200");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"300");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"FL");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//��ԭ�Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	//���´�ӡ�����ջ����С
	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
	
	//ʹ��DMAģʽ��������
	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
	
	//��ӡֱ��ͼ����
	//����
	_LCD_ClearAll(LCD_COLOR_ARGB8888_BLACK);
	
	//��������
	_LCD_DrawLine(0,0,0,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE);
	_LCD_DrawLine(1,0,1,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE);
	_LCD_DrawLine(0,PRINTER_DRAW_SIZE_H,PRINTER_DRAW_SIZE_W,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE);
	
	//������������ֵ�ֽ��
	for(i=1;i<=8;i++){
		_LCD_DrawLine(48*i,PRINTER_DRAW_SIZE_H,48*i,PRINTER_DRAW_SIZE_H-5,LCD_COLOR_RGB565_WHITE);
	}
	
	//��3���ָ�����
	_LCD_DrawBrokenLine(PrinterInfoData.pageUnion.samplePageInfo.crossLine[0]*1.5f,0,PrinterInfoData.pageUnion.samplePageInfo.crossLine[0]*1.5f,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE,LCD_COLOR_RGB565_BLACK);
	_LCD_DrawBrokenLine(PrinterInfoData.pageUnion.samplePageInfo.crossLine[1]*1.5f,0,PrinterInfoData.pageUnion.samplePageInfo.crossLine[1]*1.5f,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE,LCD_COLOR_RGB565_BLACK);
	_LCD_DrawBrokenLine(PrinterInfoData.pageUnion.samplePageInfo.crossLine[2]*1.5f,0,PrinterInfoData.pageUnion.samplePageInfo.crossLine[2]*1.5f,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE,LCD_COLOR_RGB565_BLACK);
	
	//������
	for(i=0;i<255;i++){
		_LCD_DrawLine(i*1.5f,PRINTER_DRAW_SIZE_H-PrinterInfoData.pageUnion.samplePageInfo.histogramPos[i]/50,(i+1)*1.5f,PRINTER_DRAW_SIZE_H-PrinterInfoData.pageUnion.samplePageInfo.histogramPos[i+1]/50,LCD_COLOR_RGB565_WHITE);
	}

	//����Ļ�У�0,0������PRINTER_DRAW_SIZE_W,PRINTER_DRAW_SIZE_H�������ص�ת�����ֽ�λ��ֵ
	TempAdr = (uint8_t*)PRINTER_PICTURE_SDRAM_START_ADR;
	TempAdr2 = (uint8_t*)PRINTER_PICTURE_SDRAM_START_ADR;
	LcdAdr = (uint16_t*)SDRAM_PRINTER_ADDR;
	k=0;
	for(i=PRINTER_DRAW_SIZE_H;i>0;){
		LcdAdr = (uint16_t*)SDRAM_PRINTER_ADDR + LCD_PIXEL_WIDTH*i;
		for(j=0;j<PRINTER_DRAW_SIZE_W;j++){
			if(*LcdAdr == 0xffff){
				*TempAdr |= 0x80>>k;
			}else{
				*TempAdr &= ~(0x80>>k);
			}
			
			LcdAdr++;
			k++;
			if((k & 0x07) == 0){
				k = 0;
				TempAdr++;
			}
		}
		
		i--;
		
		//ÿ��20�з���һ�δ�ӡ���ݣ���ֹ�������
		if(i%20 == 0){
			//��������Ƚ��в�ѯ�´�ӡ���ڲ��Ƿ����㹻�Ļ��棬����ֱ��ͼ�л����KO����
			Printer_IsOK();
			
			//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
			while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
				Printer_IsOK();
                osDelay(500);
			}
			
			//��ȡ�����ź�������ֹ���ݱ����ǣ�����
            osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
			PrinterSendBuf.writeIndex = 0;	
			
			for(p=0;p<20;p++){
				//��ӡ����ͼ��ָ��
				PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1C;
				PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x4B;
				PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
				PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x30;
				
				for(q=0;q<48;q++){
					PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = *TempAdr2++;
				}
			}
			
			//���´�ӡ�����ջ����С
			PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
			//ʹ��DMAģʽ��������
			HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
		}
	}
    	
	//��������Ƚ��в�ѯ�´�ӡ���ڲ��Ƿ����㹻�Ļ��棬����ֱ��ͼ�л����KO����
	Printer_IsOK();

	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
		Printer_IsOK();
        osDelay(500);
	}
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
    osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
	PrinterSendBuf.writeIndex = 0;
	
	//������ֽ��
	_Printer_ZouZhi(5);
	
	//�����
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"HGB");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"--");
	}else if(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[7] == WBCHGB_ALERT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****");
	}else{
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f",PrinterInfoData.pageUnion.samplePageInfo.hgb);
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s","g/L");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u-%u",MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usHGBL/100,MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usHGBH/100);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
	}else{
		switch(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[7]){
			case WBCHGB_ALERT_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_REVIEW:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?\r");
			break;
			case WBCHGB_ALERT_REVIEW_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			case WBCHGB_ALERT_REVIEW_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			default :
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
			break;
		}
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_XHDB[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Lym%s","%");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"--");
	}else if(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[6]==WBCHGB_ALERT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****");
	}else{
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.samplePageInfo.lymPercent);
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s","%");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usLymPercentL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usLymPercentH/100.0f);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
	}else{
		switch(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[6]){
			case WBCHGB_ALERT_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_REVIEW:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?\r");
			break;
			case WBCHGB_ALERT_REVIEW_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			case WBCHGB_ALERT_REVIEW_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			default :
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
			break;
		}
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_LBXBP[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Mid%s","%");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"--");
	}else if(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[5]==WBCHGB_ALERT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****");
	}else{
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.samplePageInfo.midPercent);
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s","%");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usMidPercentL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usMidPercentH/100.0f);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
	}else{
		switch(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[5]){
			case WBCHGB_ALERT_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_REVIEW:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?\r");
			break;
			case WBCHGB_ALERT_REVIEW_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			case WBCHGB_ALERT_REVIEW_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			default :
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
			break;
		}
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_ZJQXBP[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(MachInfo.companyInfo.company){
		case COMPANY_JIN_RUI:
		{
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Neu%s","%");
		}break;
		
		default :
		{
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Gran%s","%");
		}break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"--");
	}else if(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[4]==WBCHGB_ALERT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****");
	}else{
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.samplePageInfo.granPercent);
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s","%");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usGranPercentL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usGranPercentH/100.0f);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
	}else{
		switch(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[4]){
			case WBCHGB_ALERT_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_REVIEW:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?\r");
			break;
			case WBCHGB_ALERT_REVIEW_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			case WBCHGB_ALERT_REVIEW_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			default :
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
			break;
		}
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_ZXLXBP[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Lym#");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"--");
	}else if(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[3]==WBCHGB_ALERT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****");
	}else{
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.samplePageInfo.lym);
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"10^9/L");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usLymL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usLymH/100.0f);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
	}else{
		switch(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[3]){
			case WBCHGB_ALERT_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_REVIEW:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?\r");
			break;
			case WBCHGB_ALERT_REVIEW_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			case WBCHGB_ALERT_REVIEW_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			default :
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
			break;
		}
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_LBXB[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Mid#");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"--");
	}else if(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[2]==WBCHGB_ALERT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****");
	}else{
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.samplePageInfo.mid);
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"10^9/L");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usMidL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usMidH/100.0f);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
	}else{
		switch(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[2]){
			case WBCHGB_ALERT_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_REVIEW:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?\r");
			break;
			case WBCHGB_ALERT_REVIEW_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			case WBCHGB_ALERT_REVIEW_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			default :
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
			break;
		}
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_ZJQXB[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(MachInfo.companyInfo.company){
		case COMPANY_JIN_RUI:
		{
			//����
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Neu#");
		}break;
		
		default :
		{
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Gran#");
		}break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"--");
	}else if(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[1]==WBCHGB_ALERT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****");
	}else{
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.samplePageInfo.gran);
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"10^9/L");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usGranL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usGranH/100.0f);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
	}else{
		switch(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[1]){
			case WBCHGB_ALERT_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_REVIEW:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?\r");
			break;
			case WBCHGB_ALERT_REVIEW_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			case WBCHGB_ALERT_REVIEW_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			default :
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
			break;
		}
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_ZXLXB[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"WBC");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"--");
	}else if(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[0]==WBCHGB_ALERT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****");
	}else{
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.samplePageInfo.wbc);
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"10^9/L");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usWBCL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.samplePageInfo.refGroups].usWBCH/100.0f);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	if(PrinterInfoData.pageUnion.samplePageInfo.eDataStatus == DATA_STATUS_COUNT_INVALID){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
	}else{
		switch(PrinterInfoData.pageUnion.samplePageInfo.alarmFlag[0]){
			case WBCHGB_ALERT_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��\r");
			break;
			case WBCHGB_ALERT_REVIEW:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?\r");
			break;
			case WBCHGB_ALERT_REVIEW_SMALL:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			case WBCHGB_ALERT_REVIEW_BIG:
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
			break;
			default :
				TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," \r");
			break;
		}
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_WBC[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	//�Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;			//ִ���Ʊ�λ
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_XM[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_JG[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_DW[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_CKZ[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_BJ[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����3mm
	_Printer_ZouZhi(3);
	
	sprintf(TempStr,"%u",PrinterInfoData.pageUnion.samplePageInfo.ucAge);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s��%s\r%s��%s\r%s��%s %s\r%s��%s\r%s��%s\r",
						g_ucaLng_Printer_TIME[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.samplePageInfo.checkTime,
						g_ucaLng_Printer_SAMPLENUM[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.samplePageInfo.sampleNumber,
						g_ucaLng_Printer_AGE[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.samplePageInfo.ucAge==0?" ":TempStr,
						PrinterInfoData.pageUnion.samplePageInfo.ucAge==0?"":(PrinterInfoData.pageUnion.samplePageInfo.eAgeUnit==0?g_ucaLng_Printer_SUI[MachInfo.systemSet.eLanguage]:(PrinterInfoData.pageUnion.samplePageInfo.eAgeUnit==1?g_ucaLng_Printer_YUE[MachInfo.systemSet.eLanguage]:(PrinterInfoData.pageUnion.samplePageInfo.eAgeUnit==2?g_ucaLng_Printer_ZHOU[MachInfo.systemSet.eLanguage]:g_ucaLng_Printer_TIAN[MachInfo.systemSet.eLanguage]))),
						g_ucaLng_Printer_XB[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.samplePageInfo.gender==MALE?g_ucaLng_Printer_NAN[MachInfo.systemSet.eLanguage]:(PrinterInfoData.pageUnion.samplePageInfo.gender==NUL?"":g_ucaLng_Printer_NV[MachInfo.systemSet.eLanguage]),
						g_ucaLng_Printer_XINGM[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.samplePageInfo.name);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����5mm
	_Printer_ZouZhi(5);
	
	//���д�ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	//��ӡ���浥����
	memset(usUnicode,0,50);
	memset(usGBK,0,50);
	if(strlen(MachInfo.printSet.ucaPrintTitle) == 0){
		UTFToUnicode(usUnicode, (uint8_t*)g_ucaLng_Printer_TITLE[MachInfo.systemSet.eLanguage]);
		UnicodeToGBK(usGBK, usUnicode);
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",(char*)usGBK);
	}else{
		UTFToUnicode(usUnicode, (uint8_t*)MachInfo.printSet.ucaPrintTitle);
		UnicodeToGBK(usGBK, usUnicode);
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",(char*)usGBK);
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//��ӡ��������
	if(strlen((char*)MachInfo.labInfo.ucaClinicName) != 0){
		//������ֽ����3mm
		_Printer_ZouZhi(1);
		
		memset(usUnicode,0,50);
		memset(usGBK,0,50);
		UTFToUnicode(usUnicode, (uint8_t*)MachInfo.labInfo.ucaClinicName);
		UnicodeToGBK(usGBK, usUnicode);
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",(char*)usGBK);
		
		PrinterSendBuf.writeIndex += TempLen;
		PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	}
	
	//������ֽ����12mm
	_Printer_ZouZhi(12);
	
	//���´�ӡ�����ջ����С
	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
	
	//ʹ��DMAģʽ��������
	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
}




/***
*��ӡ�����ϻ�ҳ������
***/
void PrinterAgingTestResultPage(PrinterInfoData_s PrinterInfoData)
{
	extern UART_HandleTypeDef huart5;
	extern PrinterCurrentState_s PrinterCurrentState;
	extern Printer_SendBuf_s PrinterSendBuf;
	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
	extern MachInfo_s MachInfo;
	uint16_t TempLen = 0;
	
	
	
	//�Ȼ�ȡһ�δ�ӡ��״̬
	Printer_IsOK();
	
	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
		Printer_IsOK();
		osDelay(500);
	}
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
	PrinterSendBuf.writeIndex = 0;
	
	//�Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	//���д�ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�з������ϻ����Խ�����浥");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����5mm
	_Printer_ZouZhi(2);
	
	//�����
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s��%u\r%s��%s\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r",
						"�����ܴ���",PrinterInfoData.pageUnion.agingTestResultInfo.runCount,
						"SN",MachInfo.labInfo.ucaMachineSN,
						"��ʼʱ��",PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.year.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.year.bcd_l,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.month.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.month.bcd_l,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.day.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.day.bcd_l,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.hour.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.hour.bcd_l,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.min.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.min.bcd_l,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.sec.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.startTime.sec.bcd_l,
						"����ʱ��",PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.year.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.year.bcd_l,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.month.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.month.bcd_l,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.day.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.day.bcd_l,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.hour.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.hour.bcd_l,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.min.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.min.bcd_l,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.sec.bcd_h,PrinterInfoData.pageUnion.agingTestResultInfo.rtc.endTime.sec.bcd_l);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����3mm
	_Printer_ZouZhi(1);
	
	//�����Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 10;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;			//ִ���Ʊ�λ
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣����");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���ֵ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣��Χ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ʾ\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//ȡ���Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"VCC_P12V\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.p12v.count);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.p12v.maxValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.p12v.count<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"VCC_N12V\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.n12v.count);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%d",PrinterInfoData.pageUnion.agingTestResultInfo.n12v.maxValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.n12v.count<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"����Դ��ѹ\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.hly.count);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.hly.maxValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.hly.count<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"SDRAM\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.sdramCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.sdramCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"NAND_FLASH\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.nandFlashCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.nandFlashCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"M_EEPROM\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.eepromCount1);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.eepromCount1<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"S_EEPROM\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.eepromCount2);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.eepromCount2<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"RTC\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%d",PrinterInfoData.pageUnion.agingTestResultInfo.rtc.secondOffset);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��5s");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",(PrinterInfoData.pageUnion.agingTestResultInfo.rtc.secondOffset<5 && PrinterInfoData.pageUnion.agingTestResultInfo.rtc.secondOffset>-5)?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ӡ��\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.printerCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.printerCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"CAT\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.cat.failCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<20%s","%");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.cat.failCount<PrinterInfoData.pageUnion.agingTestResultInfo.runCount*4/5?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"--------����״̬�¸�����--------\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���ֹ���\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.oc.inOCCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.oc.inOCCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���ֹ���\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.oc.outOCCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.oc.outOCCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ͨ����\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.oc.gdOCCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.oc.gdOCCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�Ͽ�����\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.oc.releaseOCCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.oc.releaseOCCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"С�׵�ѹ\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.xk.count);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.xk.maxValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.xk.count<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ѹ�쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.pressure.buildCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%d",PrinterInfoData.pageUnion.agingTestResultInfo.inside.pressure.maxBuildPressureValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<3��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.pressure.buildCount<3?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�ܱ����쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.pressure.airtightCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%d",PrinterInfoData.pageUnion.agingTestResultInfo.inside.pressure.maxairtightValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<3��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.pressure.airtightCount<3?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��������ѹ���쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.pressure.noPressureCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%d",PrinterInfoData.pageUnion.agingTestResultInfo.inside.pressure.maxnoPressureValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<3��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.pressure.noPressureCount<3?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"����HGB\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.hgb.openCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.hgb.maxOpenValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.hgb.openCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�ر�HGB\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.hgb.closeCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.hgb.maxCloseValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.hgb.closeCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�����ؼ����\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.JscTypeCheckCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.JscTypeCheckCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�����ؼ����\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.JscDetectionCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.JscDetectionCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�źż��_CV\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.signal.cv.errCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2lf",PrinterInfoData.pageUnion.agingTestResultInfo.inside.signal.cv.maxCV);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<2��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.signal.cv.errCount<2?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�źż��_MEAN\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.signal.mean.errCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.agingTestResultInfo.inside.signal.mean.maxMean);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<2��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.signal.mean.errCount<2?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�źż��_NUM\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.signal.num.errCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.signal.num.maxNum);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<2��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.signal.num.errCount<2?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"--------����״̬�¸�����--------\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���ֹ���\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.oc.inOCCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.oc.inOCCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���ֹ���\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.oc.outOCCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.oc.outOCCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ͨ����\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.oc.gdOCCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.oc.gdOCCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�Ͽ�����\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.oc.releaseOCCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.oc.releaseOCCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"С�׵�ѹ\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.xk.count);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.xk.maxValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.xk.count<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ѹ�쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.pressure.buildCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%d",PrinterInfoData.pageUnion.agingTestResultInfo.outside.pressure.maxBuildPressureValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<3��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.pressure.buildCount<3?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�ܱ����쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.pressure.airtightCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%d",PrinterInfoData.pageUnion.agingTestResultInfo.outside.pressure.maxairtightValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<3��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.pressure.airtightCount<3?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��������ѹ���쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.pressure.noPressureCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%d",PrinterInfoData.pageUnion.agingTestResultInfo.outside.pressure.maxnoPressureValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<3��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.pressure.noPressureCount<3?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"����HGB\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.hgb.openCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.hgb.maxOpenValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.hgb.openCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�ر�HGB\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.hgb.closeCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.hgb.maxCloseValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.hgb.closeCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�����ؼ����\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.inside.JscTypeCheckCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.inside.JscTypeCheckCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�����ؼ����\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.JscDetectionCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.JscDetectionCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�źż��_CV\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.signal.cv.errCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.agingTestResultInfo.outside.signal.cv.maxCV);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<2��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.signal.cv.errCount<2?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';

	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�źż��_MEAN\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.signal.mean.errCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.agingTestResultInfo.outside.signal.mean.maxMean);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<2��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s\r",PrinterInfoData.pageUnion.agingTestResultInfo.outside.signal.mean.errCount<2?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�źż��_NUM\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.signal.num.errCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.agingTestResultInfo.outside.signal.num.maxNum);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<2��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.agingTestResultInfo.outside.signal.num.errCount<2?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����12mm
	_Printer_ZouZhi(12);
	
	//�ָ�Ĭ���Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	
	
	//���´�ӡ�����ջ����С
	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
	
	//ʹ��DMAģʽ��������
	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
}



/***
*��ӡ̽���ȶ��Բ���ҳ������
***/
void PrinterTanZhenTestResultPage(PrinterInfoData_s PrinterInfoData)
{
	extern UART_HandleTypeDef huart5;
	extern PrinterCurrentState_s PrinterCurrentState;
	extern Printer_SendBuf_s PrinterSendBuf;
	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
	extern MachInfo_s MachInfo;
	uint16_t TempLen = 0;
	
	Printer_IsOK();
	
	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
		Printer_IsOK();
		osDelay(500);
	}
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
	PrinterSendBuf.writeIndex = 0;
	
	//�Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	//���д�ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"̽���ϻ����Խ�����浥");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����5mm
	_Printer_ZouZhi(5);
	
	//�����
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s��%s\r%s��%u/%u\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r",
						"SN",MachInfo.labInfo.ucaMachineSN,					
						"����",PrinterInfoData.pageUnion.tanZhenTestErrInfo.runCount,PrinterInfoData.pageUnion.tanZhenTestErrInfo.setCount,
						"��ʼʱ��",PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.year.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.year.bcd_l,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.month.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.month.bcd_l,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.day.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.day.bcd_l,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.hour.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.hour.bcd_l,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.min.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.min.bcd_l,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.sec.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.startTime.sec.bcd_l,
						"����ʱ��",PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.year.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.year.bcd_l,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.month.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.month.bcd_l,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.day.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.day.bcd_l,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.hour.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.hour.bcd_l,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.min.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.min.bcd_l,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.sec.bcd_h,PrinterInfoData.pageUnion.tanZhenTestErrInfo.rtc.endTime.sec.bcd_l);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	if(PrinterInfoData.pageUnion.tanZhenTestErrInfo.xk.errCount != 0 || PrinterInfoData.pageUnion.tanZhenTestErrInfo.cv.errCount != 0 || PrinterInfoData.pageUnion.tanZhenTestErrInfo.num.errCount != 0 || PrinterInfoData.pageUnion.tanZhenTestErrInfo.mean.errCount != 0 || PrinterInfoData.pageUnion.tanZhenTestErrInfo.setCount != PrinterInfoData.pageUnion.tanZhenTestErrInfo.runCount){
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�ϻ��������ͨ��");
	}else{
		TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�ϻ������ͨ��");
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����3mm
	_Printer_ZouZhi(3);
	
	//�����Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 10;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;			//ִ���Ʊ�λ
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣����");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���ֵ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣��Χ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ʾ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//ȡ���Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"С�׵�ѹ\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.tanZhenTestErrInfo.xk.errCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.tanZhenTestErrInfo.xk.maxValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.tanZhenTestErrInfo.xk.errCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�źż��_CV\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.tanZhenTestErrInfo.cv.errCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.tanZhenTestErrInfo.cv.maxCV);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.tanZhenTestErrInfo.cv.errCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�źż��_MEAN\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.tanZhenTestErrInfo.mean.errCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.tanZhenTestErrInfo.mean.maxMean);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.tanZhenTestErrInfo.mean.errCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�źż��_NUM\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.tanZhenTestErrInfo.num.errCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.tanZhenTestErrInfo.num.maxNum);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.tanZhenTestErrInfo.num.errCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����12mm
	_Printer_ZouZhi(12);
	
	//�ָ�Ĭ���Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	
	
	//���´�ӡ�����ջ����С
	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
	
	//ʹ��DMAģʽ��������
	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
}




/***
*��ӡ�������ϻ�����ҳ������
***/
void PrinterOutInMotoTestResultPage(PrinterInfoData_s PrinterInfoData)
{
//	extern UART_HandleTypeDef huart5;
//	extern PrinterCurrentState_s PrinterCurrentState;
//	extern Printer_SendBuf_s PrinterSendBuf;
//	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
//	extern MachInfo_s MachInfo;
//	uint16_t TempLen = 0;
//	
//	
//	
//	Printer_IsOK();
//	
//	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
//	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
//		Printer_IsOK();
//		osDelay(500);
//	}
//	
//	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
//	osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
//	PrinterSendBuf.writeIndex = 0;
//	
//	//�Ӵ�
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
//	
//	//���д�ӡ
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�������ϻ����Խ�����浥");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//������ֽ����5mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 5;
//	
//	//�����
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s��%u\r%s��%s\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r",
//						"�����ܴ���",PrinterInfoData.pageUnion.outInTestResultInfo.runCount,
//						"SN",MachInfo.labInfo.ucaMachineSN,
//						"��ʼʱ��",PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.year.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.year.bcd_l,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.month.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.month.bcd_l,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.day.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.day.bcd_l,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.hour.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.hour.bcd_l,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.min.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.min.bcd_l,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.sec.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.startTime.sec.bcd_l,
//						"����ʱ��",PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.year.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.year.bcd_l,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.month.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.month.bcd_l,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.day.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.day.bcd_l,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.hour.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.hour.bcd_l,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.min.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.min.bcd_l,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.sec.bcd_h,PrinterInfoData.pageUnion.outInTestResultInfo.rtc.endTime.sec.bcd_l);
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	
//	//������ֽ����3mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 3;
//	
//	//�����Ʊ��
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 10;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;			//ִ���Ʊ�λ
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣����");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���ֵ");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣��Χ");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ʾ");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//ȡ���Ӵ�
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	//������ֽ����2mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 2;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�����쳣\r");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.outInTestResultInfo.outErrCount);
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<5��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.outInTestResultInfo.outErrCount<5?"��":"��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//������ֽ����2mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 2;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�����쳣\r");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.outInTestResultInfo.inErrCount);
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<5��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.outInTestResultInfo.inErrCount<5?"��":"��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	
//	//������ֽ����2mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 2;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ͨ�쳣\r");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.outInTestResultInfo.gdErrCount);
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<5��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.outInTestResultInfo.gdErrCount<5?"��":"��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//������ֽ����2mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 2;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�Ͽ��쳣\r");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.outInTestResultInfo.sfErrCount);
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<5��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.outInTestResultInfo.sfErrCount<5?"��":"��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	
//	//������ֽ����12mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
//	
//	//�ָ�Ĭ���Ʊ��
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	
//	
//	//���´�ӡ�����ջ����С
//	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
//	
//	//ʹ��DMAģʽ��������
//	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
}


/***
*��ӡ�����ϻ�����ҳ������
***/
void PrinterQizuiTestResultPage(PrinterInfoData_s PrinterInfoData)
{
	extern UART_HandleTypeDef huart5;
	extern PrinterCurrentState_s PrinterCurrentState;
	extern Printer_SendBuf_s PrinterSendBuf;
	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
	extern MachInfo_s MachInfo;
	uint16_t TempLen = 0;
	
	
	Printer_IsOK();
	
	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
		Printer_IsOK();
		osDelay(500);
	}
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
	PrinterSendBuf.writeIndex = 0;
	
	//�Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	//���д�ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�����ܱ��Բ��Խ�����浥");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����5mm
	_Printer_ZouZhi(5);
	
	//�����
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s��%u\r%s��%s\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r",
						"�����ܴ���",PrinterInfoData.pageUnion.qizuiTestResultInfo.runCount,
						"SN",MachInfo.labInfo.ucaMachineSN,
						"��ʼʱ��",PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.year.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.year.bcd_l,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.month.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.month.bcd_l,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.day.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.day.bcd_l,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.hour.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.hour.bcd_l,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.min.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.min.bcd_l,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.sec.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.startTime.sec.bcd_l,
						"����ʱ��",PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.year.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.year.bcd_l,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.month.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.month.bcd_l,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.day.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.day.bcd_l,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.hour.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.hour.bcd_l,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.min.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.min.bcd_l,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.sec.bcd_h,PrinterInfoData.pageUnion.qizuiTestResultInfo.rtc.endTime.sec.bcd_l);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����3mm
	_Printer_ZouZhi(3);
	
	//�����Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 10;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;			//ִ���Ʊ�λ
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣����");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���ֵ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣��Χ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ʾ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//ȡ���Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ѹ�쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.qizuiTestResultInfo.pressure.buildCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%d Pa",PrinterInfoData.pageUnion.qizuiTestResultInfo.pressure.maxBuildPressureValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.qizuiTestResultInfo.pressure.buildCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�ܱ����쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.qizuiTestResultInfo.pressure.airtightCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%d Pa",PrinterInfoData.pageUnion.qizuiTestResultInfo.pressure.maxairtightValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.qizuiTestResultInfo.pressure.airtightCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���������쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.qizuiTestResultInfo.pressure.noPressureCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%d Pa",PrinterInfoData.pageUnion.qizuiTestResultInfo.pressure.maxnoPressureValue);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.qizuiTestResultInfo.pressure.noPressureCount<1?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����12mm
	_Printer_ZouZhi(12);
	
	//�ָ�Ĭ���Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	
	
	//���´�ӡ�����ջ����С
	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
	
	//ʹ��DMAģʽ��������
	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
}



/***
*��ӡ�����ϻ�����ҳ������
***/
void PrinterAirValveTestResultPage(PrinterInfoData_s PrinterInfoData)
{
//	extern UART_HandleTypeDef huart5;
//	extern PrinterCurrentState_s PrinterCurrentState;
//	extern Printer_SendBuf_s PrinterSendBuf;
//	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
//	extern MachInfo_s MachInfo;
//	uint16_t TempLen = 0;
//	
//	
//	Printer_IsOK();
//	
//	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
//	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
//		Printer_IsOK();
//		osDelay(500);
//	}
//	
//	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
//	osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
//	PrinterSendBuf.writeIndex = 0;
//	
//	//�Ӵ�
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
//	
//	//���д�ӡ
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�����ϻ����Խ�����浥");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//������ֽ����5mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 5;
//	
//	//�����
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s��%u\r%s��%s\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r",
//						"�����ܴ���",PrinterInfoData.pageUnion.airValveTestResultInfo.runCount,
//						"SN",MachInfo.labInfo.ucaMachineSN,
//						"��ʼʱ��",PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.year.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.year.bcd_l,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.month.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.month.bcd_l,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.day.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.day.bcd_l,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.hour.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.hour.bcd_l,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.min.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.min.bcd_l,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.sec.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.startTime.sec.bcd_l,
//						"����ʱ��",PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.year.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.year.bcd_l,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.month.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.month.bcd_l,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.day.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.day.bcd_l,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.hour.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.hour.bcd_l,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.min.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.min.bcd_l,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.sec.bcd_h,PrinterInfoData.pageUnion.airValveTestResultInfo.rtc.endTime.sec.bcd_l);
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//������ֽ����12mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
//	
//	//�ָ�Ĭ���Ʊ��
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	
//	
//	//���´�ӡ�����ջ����С
//	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
//	
//	//ʹ��DMAģʽ��������
//	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
}



/***
*��ӡҺ���ϻ�����ҳ������
***/
void PrinterYeFaValveTestResultPage(PrinterInfoData_s PrinterInfoData)
{
//	extern UART_HandleTypeDef huart5;
//	extern PrinterCurrentState_s PrinterCurrentState;
//	extern Printer_SendBuf_s PrinterSendBuf;
//	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
//	extern MachInfo_s MachInfo;
//	uint16_t TempLen = 0;
//	
//	
//	
//	Printer_IsOK();
//	
//	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
//	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
//		Printer_IsOK();
//		osDelay(500);
//	}
//	
//	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
//	osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
//	PrinterSendBuf.writeIndex = 0;
//	
//	//�Ӵ�
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
//	
//	//���д�ӡ
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Һ���ϻ����Խ�����浥");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//������ֽ����5mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 5;
//	
//	//�����
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s��%u\r%s��%s\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r",
//						"�����ܴ���",PrinterInfoData.pageUnion.yeFaValveTestResultInfo.runCount,
//						"SN",MachInfo.labInfo.ucaMachineSN,
//						"��ʼʱ��",PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.year.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.year.bcd_l,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.month.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.month.bcd_l,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.day.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.day.bcd_l,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.hour.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.hour.bcd_l,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.min.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.min.bcd_l,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.sec.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.startTime.sec.bcd_l,
//						"����ʱ��",PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.year.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.year.bcd_l,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.month.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.month.bcd_l,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.day.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.day.bcd_l,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.hour.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.hour.bcd_l,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.min.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.min.bcd_l,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.sec.bcd_h,PrinterInfoData.pageUnion.yeFaValveTestResultInfo.rtc.endTime.sec.bcd_l);
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//������ֽ����12mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
//	
//	//�ָ�Ĭ���Ʊ��
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	
//	
//	//���´�ӡ�����ջ����С
//	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
//	
//	//ʹ��DMAģʽ��������
//	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
}



/***
*��ӡ��ѹ�ϻ�����ҳ������
***/
void PrinterBuildPressureTestResultPage(PrinterInfoData_s PrinterInfoData)
{
	extern UART_HandleTypeDef huart5;
	extern PrinterCurrentState_s PrinterCurrentState;
	extern Printer_SendBuf_s PrinterSendBuf;
	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
	extern MachInfo_s MachInfo;
	uint16_t TempLen = 0;
	
	
	Printer_IsOK();
	
	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
		Printer_IsOK();
		osDelay(500);
	}
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
	PrinterSendBuf.writeIndex = 0;
	
	//�Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	//���д�ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ѹ�ϻ����Խ�����浥");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����5mm
	_Printer_ZouZhi(5);
	
	//�����
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s��%u\r%s��%s\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r",
						"�����ܴ���",PrinterInfoData.pageUnion.buildPressureTestResultInfo.runCount,
						"SN",MachInfo.labInfo.ucaMachineSN,
						"��ʼʱ��",PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.year.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.year.bcd_l,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.month.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.month.bcd_l,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.day.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.day.bcd_l,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.hour.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.hour.bcd_l,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.min.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.min.bcd_l,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.sec.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.startTime.sec.bcd_l,
						"����ʱ��",PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.year.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.year.bcd_l,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.month.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.month.bcd_l,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.day.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.day.bcd_l,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.hour.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.hour.bcd_l,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.min.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.min.bcd_l,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.sec.bcd_h,PrinterInfoData.pageUnion.buildPressureTestResultInfo.rtc.endTime.sec.bcd_l);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����3mm
	_Printer_ZouZhi(3);
	
	//�����Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 10;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;			//ִ���Ʊ�λ
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣����");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���ֵ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣��Χ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ʾ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//ȡ���Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ѹ��-10kPa�쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure10.buildCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u ms",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure10.time);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<5��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure10.buildCount<5?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ѹ��-20kPa�쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure20.buildCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u ms",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure20.time);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<5��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure20.buildCount<5?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ѹ��-30kPa�쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure30.buildCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u ms",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure30.time);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<5��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure30.buildCount<5?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ѹ��-40kPa�쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure40.buildCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u ms",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure40.time);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<5��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure40.buildCount<5?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ѹ��-50kPa�쳣\r");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure50.buildCount);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u ms",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure50.time);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<5��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.buildPressureTestResultInfo.pressure50.buildCount<5?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����12mm
	_Printer_ZouZhi(12);
	
	//�ָ�Ĭ���Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	
	
	//���´�ӡ�����ջ����С
	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
	
	//ʹ��DMAģʽ��������
	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
}



/***
*��ӡ�����缫�ϻ�����ҳ������
***/
void PrinterDLElecTestResultPage(PrinterInfoData_s PrinterInfoData)
{
//	extern UART_HandleTypeDef huart5;
//	extern PrinterCurrentState_s PrinterCurrentState;
//	extern Printer_SendBuf_s PrinterSendBuf;
//	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
//	extern MachInfo_s MachInfo;
//	uint16_t TempLen = 0;
//	
//	
//	Printer_IsOK();
//	
//	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
//	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
//		Printer_IsOK();
//		osDelay(500);
//	}
//	
//	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
//	osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
//	PrinterSendBuf.writeIndex = 0;
//	
//	//�Ӵ�
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
//	
//	//���д�ӡ
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�����缫�ϻ����Խ�����浥");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//������ֽ����5mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 5;
//	
//	//�����
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s��%u\r%s��%s\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r",
//						"�����ܴ���",PrinterInfoData.pageUnion.dlElecTestResultInfo.runCount,
//						"SN",MachInfo.labInfo.ucaMachineSN,
//						"��ʼʱ��",PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.year.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.year.bcd_l,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.month.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.month.bcd_l,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.day.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.day.bcd_l,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.hour.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.hour.bcd_l,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.min.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.min.bcd_l,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.sec.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.startTime.sec.bcd_l,
//						"����ʱ��",PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.year.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.year.bcd_l,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.month.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.month.bcd_l,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.day.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.day.bcd_l,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.hour.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.hour.bcd_l,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.min.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.min.bcd_l,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.sec.bcd_h,PrinterInfoData.pageUnion.dlElecTestResultInfo.rtc.endTime.sec.bcd_l);
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//������ֽ����3mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 3;
//	
//	//�����Ʊ��
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 10;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;			//ִ���Ʊ�λ
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣����");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���ֵ");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�쳣��Χ");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ʾ");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//ȡ���Ӵ�
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	//������ֽ����2mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 2;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Ӧ����̬�쳣\r");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.dlElecTestResultInfo.activeStatusErrCount);
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.dlElecTestResultInfo.activeStatusErrCount<1?"��":"��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//������ֽ����2mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 2;
//	
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�Ǵ���̬�쳣\r");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u",PrinterInfoData.pageUnion.dlElecTestResultInfo.idleStatusErrCount);
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"<1��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
//	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.dlElecTestResultInfo.idleStatusErrCount<1?"��":"��");
//	PrinterSendBuf.writeIndex += TempLen;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
//	
//	//������ֽ����12mm
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x64;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
//	
//	//�ָ�Ĭ���Ʊ��
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
//	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
//	
//	
//	
//	//���´�ӡ�����ջ����С
//	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
//	
//	//ʹ��DMAģʽ��������
//	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
}






const char g_ucaLng_Printer_ZhiKongTitle[LANGUAGE_END][35]    = {"�ʿط������鱨�浥", "QC analysis report"};
const char g_ucaLng_Printer_WenJianHao[LANGUAGE_END][15]    = {"�� �� ��", "File Num"};
const char g_ucaLng_Printer_ShuiPing[LANGUAGE_END][15]    = {"ˮ    ƽ", "Level"};
const char g_ucaLng_Printer_PiHao[LANGUAGE_END][15]    = {"��    ��", "Batch No"};
const char g_ucaLng_Printer_ZhiKongBianHao[LANGUAGE_END][15]    = {"�ʿر��", "QC ID"};
const char g_ucaLng_Printer_YouXiaoQi[LANGUAGE_END][15]    = {"�� Ч ��", "Indate"};
const char g_ucaLng_Printer_Time[LANGUAGE_END][15]    = {"����ʱ��", "Time"};
const char g_ucaLng_Printer_Tips[LANGUAGE_END][15]    = {"��ʾ��Ϣ", "Tip"};
const char g_ucaLng_Printer_LOW[LANGUAGE_END][10]    = {"��", "Low"};
const char g_ucaLng_Printer_Mid[LANGUAGE_END][10]    = {"��", "Mid"};
const char g_ucaLng_Printer_High[LANGUAGE_END][10]    = {"��", "High"};


/***
*��ӡ�ʿط���ҳ������
***/
void PrinterQCPage(PrinterInfoData_s PrinterInfoData)
{
	extern UART_HandleTypeDef huart5;
	extern PrinterCurrentState_s PrinterCurrentState;
	extern Printer_SendBuf_s PrinterSendBuf;
	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
	extern MachInfo_s MachInfo;
	uint16_t TempLen = 0;
	uint16_t i,j,k,q,p;
	uint8_t* TempAdr;
	uint8_t* TempAdr2;
	uint16_t* LcdAdr;	
    QC_FILENUM_INDEX_e eFileNumIndex = 0;
    
    //�ļ�������
    eFileNumIndex = PrinterInfoData.pageUnion.qcPageInfo.eFileNumIndex;
		
	//�Ȼ�ȡһ�δ�ӡ��״̬
	Printer_IsOK();
	
	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
		Printer_IsOK();
        osDelay(500);
	}
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
    osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
	PrinterSendBuf.writeIndex = 0;
	
	//�Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	//���д�ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_ZhiKongTitle[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����5mm
	_Printer_ZouZhi(5);
	
	//�����
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s��%u\r%s��%s\r%s��%s\r%s��%s\r%s��%s\r%s��%s\r%s��%s\r",
						g_ucaLng_Printer_WenJianHao[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.qcPageInfo.fileNum,
						g_ucaLng_Printer_ShuiPing[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.qcPageInfo.level==0?g_ucaLng_Printer_LOW[MachInfo.systemSet.eLanguage]:PrinterInfoData.pageUnion.qcPageInfo.level==1?g_ucaLng_Printer_Mid[MachInfo.systemSet.eLanguage]:PrinterInfoData.pageUnion.qcPageInfo.level==2?g_ucaLng_Printer_High[MachInfo.systemSet.eLanguage]:"ERR",
						g_ucaLng_Printer_PiHao[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.qcPageInfo.batchNum,
						g_ucaLng_Printer_ZhiKongBianHao[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.qcPageInfo.qcSN,
						g_ucaLng_Printer_YouXiaoQi[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.qcPageInfo.indate,
						g_ucaLng_Printer_Time[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.qcPageInfo.checkTime,
						g_ucaLng_Printer_Tips[MachInfo.systemSet.eLanguage],PrinterInfoData.pageUnion.qcPageInfo.tip);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����3mm
	_Printer_ZouZhi(3);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;			//ִ���Ʊ�λ
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_XM[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_JG[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_DW[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_CKZ[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_BJ[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//ȡ���Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_WBC[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s","WBC");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[0]==WBCHGB_ALERT_INVALID?(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****")):(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.qcPageInfo.wbc));
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"10^9/L");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	//TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usWBCL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usWBCH/100.0f);
    TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f-%.2f", MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[WBCHGB_RST_WBC], MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[WBCHGB_RST_WBC]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[0]){
		case WBCHGB_ALERT_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_REVIEW:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?");
		break;
		case WBCHGB_ALERT_REVIEW_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		case WBCHGB_ALERT_REVIEW_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		default :
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
		break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_ZXLXB[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(MachInfo.companyInfo.company){
		case COMPANY_JIN_RUI:
		{
			//����
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Neu#");
		}break;
		
		default :
		{
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Gran#");
		}break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[1]==WBCHGB_ALERT_INVALID?(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****")):(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.qcPageInfo.gran));
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"10^9/L");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	//TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usGranL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usGranH/100.0f);
    TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f-%.2f", MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[WBCHGB_RST_GranJ], MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[WBCHGB_RST_GranJ]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[1]){
		case WBCHGB_ALERT_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_REVIEW:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?");
		break;
		case WBCHGB_ALERT_REVIEW_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		case WBCHGB_ALERT_REVIEW_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		default :
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
		break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_ZJQXB[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Mid#");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[2]==WBCHGB_ALERT_INVALID?(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****")):(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.qcPageInfo.mid));
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"10^9/L");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	//TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usMidL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usMidH/100.0f);
    TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f-%.2f", MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[WBCHGB_RST_MidJ], MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[WBCHGB_RST_MidJ]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[2]){
		case WBCHGB_ALERT_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_REVIEW:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?");
		break;
		case WBCHGB_ALERT_REVIEW_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		case WBCHGB_ALERT_REVIEW_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		default :
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
		break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_LBXB[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Lym#");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[3]==WBCHGB_ALERT_INVALID?(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****")):(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.qcPageInfo.lym));
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"10^9/L");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	//TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usLymL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usLymH/100.0f);
    TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f-%.2f", MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[WBCHGB_RST_LymJ], MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[WBCHGB_RST_LymJ]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[3]){
		case WBCHGB_ALERT_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_REVIEW:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?");
		break;
		case WBCHGB_ALERT_REVIEW_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		case WBCHGB_ALERT_REVIEW_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		default :
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
		break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_ZXLXBP[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(MachInfo.companyInfo.company){
		case COMPANY_JIN_RUI:
		{
			//����
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Neu%s","%");
		}break;
		
		default :
		{
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Gran%s","%");
		}break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[4]==WBCHGB_ALERT_INVALID?(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****")):(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.qcPageInfo.granPercent));
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s","%");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	//TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usGranPercentL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usGranPercentH/100.0f);
    TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f-%.2f", MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[WBCHGB_RST_GranB], MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[WBCHGB_RST_GranB]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[4]){
		case WBCHGB_ALERT_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_REVIEW:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?");
		break;
		case WBCHGB_ALERT_REVIEW_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		case WBCHGB_ALERT_REVIEW_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		default :
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
		break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_ZJQXBP[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Mid%s","%");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[5]==WBCHGB_ALERT_INVALID?(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****")):(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.qcPageInfo.midPercent));
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s","%");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	//TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usMidL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usMidH/100.0f);
    TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f-%.2f", MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[WBCHGB_RST_MidB], MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[WBCHGB_RST_MidB]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[5]){
		case WBCHGB_ALERT_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_REVIEW:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?");
		break;
		case WBCHGB_ALERT_REVIEW_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		case WBCHGB_ALERT_REVIEW_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		default :
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
		break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_LBXBP[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"Lym%s","%");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[6]==WBCHGB_ALERT_INVALID?(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****")):(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f",PrinterInfoData.pageUnion.qcPageInfo.lymPercent));
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s","%");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	//TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f-%.1f",MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usLymPercentL/100.0f,MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usLymPercentH/100.0f);
    TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f-%.2f", MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[WBCHGB_RST_LymB], MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[WBCHGB_RST_LymB]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[6]){
		case WBCHGB_ALERT_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_REVIEW:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?");
		break;
		case WBCHGB_ALERT_REVIEW_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		case WBCHGB_ALERT_REVIEW_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		default :
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
		break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
		
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_XHDB[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"HGB");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[7]==WBCHGB_ALERT_INVALID?(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"****")):(TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.1f",PrinterInfoData.pageUnion.qcPageInfo.hgb));
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s","g/L");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	//TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%u-%u",MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usHGBL/100,MachInfo.other.refGroups[PrinterInfoData.pageUnion.qcPageInfo.refGroups].usHGBH/100);
    TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%.2f-%.2f", MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[WBCHGB_RST_HGB], MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[WBCHGB_RST_HGB]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	switch(PrinterInfoData.pageUnion.qcPageInfo.alarmFlag[7]){
		case WBCHGB_ALERT_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��");
		break;
		case WBCHGB_ALERT_REVIEW:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?");
		break;
		case WBCHGB_ALERT_REVIEW_SMALL:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		case WBCHGB_ALERT_REVIEW_BIG:
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"?��\r");
		break;
		default :
			TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex," ");
		break;
	}
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����7mm
	_Printer_ZouZhi(5);
	
	//���´�ӡ�����ջ����С
	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
	
	//ʹ��DMAģʽ��������
	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
    
	//��ӡֱ��ͼ����
	//����
	_LCD_ClearAll(LCD_COLOR_ARGB8888_BLACK);
	
	//��������
	_LCD_DrawLine(0,0,0,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE);
	_LCD_DrawLine(1,0,1,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE);
	_LCD_DrawLine(0,PRINTER_DRAW_SIZE_H,PRINTER_DRAW_SIZE_W,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE);
	
	//������������ֵ�ֽ��
	for(i=1;i<=8;i++){
		_LCD_DrawLine(48*i,PRINTER_DRAW_SIZE_H,48*i,PRINTER_DRAW_SIZE_H-5,LCD_COLOR_RGB565_WHITE);
	}
	
	//��3���ָ�����
	_LCD_DrawBrokenLine(PrinterInfoData.pageUnion.qcPageInfo.crossLine[0]*1.5f,0,PrinterInfoData.pageUnion.qcPageInfo.crossLine[0]*1.5f,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE,LCD_COLOR_RGB565_BLACK);
	_LCD_DrawBrokenLine(PrinterInfoData.pageUnion.qcPageInfo.crossLine[1]*1.5f,0,PrinterInfoData.pageUnion.qcPageInfo.crossLine[1]*1.5f,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE,LCD_COLOR_RGB565_BLACK);
	_LCD_DrawBrokenLine(PrinterInfoData.pageUnion.qcPageInfo.crossLine[2]*1.5f,0,PrinterInfoData.pageUnion.qcPageInfo.crossLine[2]*1.5f,PRINTER_DRAW_SIZE_H,LCD_COLOR_RGB565_WHITE,LCD_COLOR_RGB565_BLACK);
	
	//������
	for(i=0;i<255;i++){
		_LCD_DrawLine(i*1.5f,PRINTER_DRAW_SIZE_H-PrinterInfoData.pageUnion.qcPageInfo.histogramPos[i]/50,(i+1)*1.5f,PRINTER_DRAW_SIZE_H-PrinterInfoData.pageUnion.qcPageInfo.histogramPos[i+1]/50,LCD_COLOR_RGB565_WHITE);
	}

	//����Ļ�У�0,0������PRINTER_DRAW_SIZE_W,PRINTER_DRAW_SIZE_H�������ص�ת�����ֽ�λ��ֵ
	TempAdr = (uint8_t*)PRINTER_PICTURE_SDRAM_START_ADR;
	TempAdr2 = (uint8_t*)PRINTER_PICTURE_SDRAM_START_ADR;
	LcdAdr = (uint16_t*)SDRAM_PRINTER_ADDR;
	k=0;
	for(i=0;i<=PRINTER_DRAW_SIZE_H;){
		LcdAdr = (uint16_t*)SDRAM_PRINTER_ADDR + LCD_PIXEL_WIDTH*i;
		for(j=0;j<PRINTER_DRAW_SIZE_W;j++){
			if(*LcdAdr == 0xffff){
				*TempAdr |= 0x80>>k;
			}else{
				*TempAdr &= ~(0x80>>k);
			}
			
			LcdAdr++;
			k++;
			if((k & 0x07) == 0){
				k = 0;
				TempAdr++;
			}
		}
		
		i++;
		
		//ÿ��20�з���һ�δ�ӡ���ݣ���ֹ�������
		if(i%20 == 0){
			//��������Ƚ��в�ѯ�´�ӡ���ڲ��Ƿ����㹻�Ļ��棬����ֱ��ͼ�л����KO����
			Printer_IsOK();
			
			//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
			while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
				Printer_IsOK();
                osDelay(500);
			}
			
			//��ȡ�����ź�������ֹ���ݱ����ǣ�����
            osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
			PrinterSendBuf.writeIndex = 0;	
			
			for(p=0;p<20;p++){
				//��ӡ����ͼ��ָ��
				PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1C;
				PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x4B;
				PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
				PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x30;
				
				for(q=0;q<48;q++){
					PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = *TempAdr2++;
				}
			}
			
			//���´�ӡ�����ջ����С
			PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
			//ʹ��DMAģʽ��������
			HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
		}
	}
    	
	//��������Ƚ��в�ѯ�´�ӡ���ڲ��Ƿ����㹻�Ļ��棬����ֱ��ͼ�л����KO����
	Printer_IsOK();

	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
		Printer_IsOK();
        osDelay(500);
	}
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
    osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
	PrinterSendBuf.writeIndex = 0;	
	
	//��ӡʣ����������
	j = i%20;
	for(p=0;p<j;p++){
		PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1C;
		PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x4B;
		PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
		PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x30;
		
		for(q=0;q<48;q++){
			PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = *TempAdr2++;
		}
	}
	
	//��ӡʣ����������
	//�����Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 7;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 15;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 23;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 31;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"100");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"200");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"300");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"fl");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//��ԭ�Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	//���д�ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	//������ֽ����13mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_Histogram[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����13mm
	_Printer_ZouZhi(2);
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",g_ucaLng_Printer_Tip[MachInfo.systemSet.eLanguage]);
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
		
	//������ֽ����12mm
	_Printer_ZouZhi(12);
	
	//���´�ӡ�����ջ����С
	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
	
	//ʹ��DMAģʽ��������
	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
}



/***
*��ӡ���������ϻ�����ҳ������
***/
void PrinterSCAgingTestResultPage(PrinterInfoData_s PrinterInfoData)
{
	extern UART_HandleTypeDef huart5;
	extern PrinterCurrentState_s PrinterCurrentState;
	extern Printer_SendBuf_s PrinterSendBuf;
	extern osSemaphoreId_t SemphrCountPrinterTxHandle;
	extern MachInfo_s MachInfo;
	uint16_t TempLen = 0;
	
	
	Printer_IsOK();
	
	//�жϴ�ӡ��״̬��ʣ����ջ����Ƿ��㹻��Ϊ�˲���ʧ���ݣ�������Ҫ�ȴ���ֱ������Ҫ��
	while(PrinterCurrentState.readyState != 0x04 || PrinterCurrentState.remainRxBuf < PRINTER_SEND_BUF_SIZE){
		Printer_IsOK();
		osDelay(500);
	}
	
	//��ȡ�����ź�������ֹ���ݱ����ǣ�����
	osSemaphoreAcquire(SemphrCountPrinterTxHandle, osWaitForever);
	PrinterSendBuf.writeIndex = 0;
	
	//�Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	//���д�ӡ
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x01;
	
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�����ϻ����Խ�����浥");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����5mm
	_Printer_ZouZhi(5);
	
	//�����
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x61;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;

	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s��%s\r%s��%u/%u\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r%s��20%d%d-%d%d-%d%d %d%d:%d%d:%d%d\r%s��%s\r",
						"SN",MachInfo.labInfo.ucaMachineSN,
						"����",PrinterInfoData.pageUnion.scAgingTestResultInfo.count.aging,PrinterInfoData.pageUnion.scAgingTestResultInfo.count.agingTotal,
						"��ʼʱ��",PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.year.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.year.bcd_l,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.month.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.month.bcd_l,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.day.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.day.bcd_l,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.hour.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.hour.bcd_l,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.min.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.min.bcd_l,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.sec.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.startTime.sec.bcd_l,
						"����ʱ��",PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.year.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.year.bcd_l,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.month.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.month.bcd_l,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.day.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.day.bcd_l,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.hour.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.hour.bcd_l,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.min.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.min.bcd_l,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.sec.bcd_h,PrinterInfoData.pageUnion.scAgingTestResultInfo.rtc.endTime.sec.bcd_l,
						"�ϻ����",((*((uint32_t*)&PrinterInfoData.pageUnion.scAgingTestResultInfo.aging)==0)?"ͨ��":"��ͨ��")
						);
						
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����3mm
	_Printer_ZouZhi(3);
	
	//�����Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 25;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;			//ִ���Ʊ�λ
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��Ŀ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"���");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//ȡ���Ӵ�
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x45;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	//������ֽ����2mm
	_Printer_ZouZhi(2);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"VCC_P12V");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.p12vFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"VCC_N12V");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.n12vFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"����Դ��ѹ");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.v55Flag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"SDRAM");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.sdramFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"NAND");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.nandFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"M_EEPROM");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.mEEPROMFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"S_EEPROM");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.sEEPROMFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"RTC");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.rtcFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"CAT1");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.catFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�¶�");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.ntcFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"��ѹ��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.buildPressFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"������");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.qimiFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"й©��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.fangqiFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"�����˶���");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.outInFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����2mm
	_Printer_ZouZhi(1);
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"HGB");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x09;
	TempLen = sprintf((char*)PrinterSendBuf.buf+PrinterSendBuf.writeIndex,"%s",PrinterInfoData.pageUnion.scAgingTestResultInfo.aging.hgbFlag==0?"��":"��");
	PrinterSendBuf.writeIndex += TempLen;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = '\0';
	
	//������ֽ����12mm
	_Printer_ZouZhi(12);
	
	//�ָ�Ĭ���Ʊ��
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x1B;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x44;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 1;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 6;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 12;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 19;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 29;
	PrinterSendBuf.buf[PrinterSendBuf.writeIndex++] = 0x00;
	
	
	//���´�ӡ�����ջ����С
	PrinterCurrentState.remainRxBuf -= PrinterSendBuf.writeIndex;
	
	//ʹ��DMAģʽ��������
	HAL_UART_Transmit_DMA(&huart5,PrinterSendBuf.buf,PrinterSendBuf.writeIndex);
}








