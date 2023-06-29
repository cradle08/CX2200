/**************************************************************************
*���败����أ�ʹ��XPT2046оƬ��SPI�ӿڣ�����ģʽ0����������XPT2046ʱ�Ӳ�2M
����SPI�ӿ�ʱ�Ӳ���̫�죬������Ҫ�ֳ�256��Ƶ��ʹ��
***************************************************************************/
#include "xpt2046.h"
#include "main.h"
#include "spi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "GUI.h"
#include "cmsis_os2.h"
#include "msg_def.h"
#include "file_operate.h"
#include "bsp_outin.h"
#include "bsp_spi.h"



/*�궨����****************************************************************/
#define SPI_NSS_LOW						SPI_CS_Enable(SPI_CS_TP)    //	TP_SPI_CS_GPIO_Port->BSRR |= TP_SPI_CS_Pin<<16		//�������NSS1Ϊ�͵�ƽ
#define SPI_NSS_HIGH					SPI_CS_Disable(SPI_CS_TP)   //	TP_SPI_CS_GPIO_Port->BSRR |= TP_SPI_CS_Pin			//�������NSS1Ϊ�ߵ�ƽ
		
#define INVALID_VALUE						0x00												//�������ڶ�ȡ����ʹ�õ���Ч����
#define SPI_MAX_TIMEOUT						500000												//����SPI��дһ���ֽ����ĳ�ʱʱ��

//===����оƬSPIͨѶ������======
#define CMD_TOUCH_X_POS						0xD0												//���ڻ�ȡ���败���е�Xλ�õ�������
#define CMD_TOUCH_Y_POS						0x90												//���ڻ�ȡ���败���е�Yλ�õ�������


//===У׼����������ֵ===
#define TOUCH_ADJUST_PIXEL_X_0				100//200
#define TOUCH_ADJUST_PIXEL_Y_0				100

#define TOUCH_ADJUST_PIXEL_X_1				700//600
#define TOUCH_ADJUST_PIXEL_Y_1				100

#define TOUCH_ADJUST_PIXEL_X_2				400//600
#define TOUCH_ADJUST_PIXEL_Y_2				380//400

#define TOUCH_ADJUST_PIXEL_X_3				400
#define TOUCH_ADJUST_PIXEL_Y_3				200



/***
*����PB��Ӧ������ͨ����Ӧ��IO��ʵ��״̬������
***/
typedef union _PGStates_u{
	uint16_t whole;
	
	struct{
		//PG
		uint16_t :							10;
		uint16_t penirq:					1;
		uint16_t :							5;
	}bit;
}PGStates_u;










/*ȫ�ֱ���������***************************************************/
PGStates_u PGStates = {0};									

uint16_t PGFirstInputState = 0;
uint16_t PGSecondInputState = 0;	

//TouchAdjustPara_s MachInfo.touch = {0};
TouchPos_s TouchPixelPos = {0};



/*
*   xpt2046, spi�ڳ�ʼ��
*/
void XPT2046_Init(void)
{
    //MX_SPI4_Init_LowSpeed();
    //MX_SPI4_Init();    
    SPI4_ChangeModeTo(SPI_MODE_TP);//
}


/*�ڲ�������***************************************************************/
/***
*�Ա��״�ɨ��IO���������ɨ��IO�Ľ�����Ӷ��õ��˴�IOɨ��׶ε�����״̬���
***/
static void _InputStateCompare()
{
	extern PGStates_u PGStates;
	
	extern uint16_t PGFirstInputState;
	extern uint16_t PGSecondInputState;
	
	uint16_t i;
	uint16_t IOStateTemp;
	
	//PB�˿ڱȽ�
	for(i=0;i<16;i++){
		IOStateTemp = (PGFirstInputState>>i)&0x01;
		if(IOStateTemp == ((PGSecondInputState>>i)&0x01)){				//�ж����βɼ�������ͨ��IO״̬�Ƿ�һ�£�һ�²ű����״̬����һ����ֱ�Ӻ�����βɼ�
			if(IOStateTemp){
				PGStates.whole |= 1<<i;
			}else{
				PGStates.whole &= ~(1<<i);
			}
		}
	}
}




/***
*��дһ���ֽڣ�����д��һ���ֽڣ����߶���һ���ֽ�
*������
	Data��Ҫд������ݣ�����Ϊ�˶���һ���ֽڶ�ʹ�õ���Ч�ֽ���������SPI����ʱ��
*����ֵ����Ҫ���ص�һ���ֽڣ���������д��һ���ֽڴӶ��ӻ����ص���Ч����
***/
static uint8_t _SPI_RW(uint8_t Data)
{
	extern SPI_HandleTypeDef hspi4;
	uint32_t Count = 0;
	
	//�ж�TXE�Ƿ�Ϊ1��������ָ������ݵ�����
	while(__HAL_SPI_GET_FLAG(&hspi4,SPI_FLAG_TXE)==RESET){
		Count++;
		if(Count>=SPI_MAX_TIMEOUT){
			return 0;
		}
	}
	hspi4.Instance->DR = Data;
	
	//�ж�RXNE�Ƿ�Ϊ1���Ӷ���֪�����Ƿ��Ѿ��������,��RXNEΪ1ʱ������һ�������շ�����ˣ����԰���Ҫ��NSS������
	Count = 0;
	while(__HAL_SPI_GET_FLAG(&hspi4,SPI_FLAG_RXNE)==RESET){
		Count++;
		if(Count>=SPI_MAX_TIMEOUT){
			return 0;
		}
	}
	return hspi4.Instance->DR;
}








/*�ⲿ������**************************************************************/
/***
*ɨ��PENIRQ����IO��״̬���˺�����Ҫ��ѭ����ʹ�ã���Ϊ�������Ĺ���
***/
void PenIRQInputStateScanning(void)
{
	extern uint16_t PGFirstInputState;
	extern uint16_t PGSecondInputState;
	static uint8_t OnceFlag = 0;
	
	if(OnceFlag == 0){
		OnceFlag = 1;
		PGFirstInputState = (uint16_t)GPIOG->IDR;
	}else{
		OnceFlag = 0;
		PGSecondInputState = (uint16_t)GPIOG->IDR;
		
		//�ȽϽ��
		_InputStateCompare();
	}
}



/***
*����PENIRQ������״̬
***/
uint8_t PenIRQ_GetState(void)
{
	extern PGStates_u PGStates;
	
	return PGStates.bit.penirq;
}



/***
*��ȡX���Y��Ĵ���ԭʼ����ֵ������������ȡ6�Σ�ȥ����ߺ����ֵ����ƽ��
*������
	TouchRawPos������ɼ����Ĵ���ԭʼ����ֵ��ƽ�����ֵ��
*����ֵ:��ǰ�ɼ�����X���Y��Ĵ���ԭʼ����ֵ�ɹ�ʱ����SUCCESS�����򷵻�ʧ��
***/
ErrorStatus XPT2046_GetRawPosValue(TouchPos_s* TouchRawPos)
{
	extern uint8_t SPI4ResourceNum;
	TouchPos_s TouchRawPosTemp[6];
	uint16_t Temp1 = 0;
	uint16_t Temp2 = 0;
	uint8_t i = 0;
	uint8_t j = 0;
	uint32_t Sum = 0;
	PGStates_u PGStatesTemp;
	
    SPI4_ChangeModeTo(SPI_MODE_TP);
	//���ʹӻ�NSS��֪ͨ�ӻ���ʼͨѶ
	SPI_NSS_LOW;
	
	for(i=0;i<6;i++){
		//�������24��ʱ��ʱ���ģʽ�����⵼��PENIRQ�����ڲɼ������б����Σ��Ӷ��ɼ������������
		_SPI_RW(CMD_TOUCH_X_POS);
		Temp1 = _SPI_RW(INVALID_VALUE);
		Temp2 = _SPI_RW(INVALID_VALUE);
		
		TouchRawPosTemp[i].x = (((Temp1<<8)|Temp2)>>3)&0xfff;
		
		_SPI_RW(CMD_TOUCH_Y_POS);
		Temp1 = _SPI_RW(INVALID_VALUE);
		Temp2 = _SPI_RW(INVALID_VALUE);
		
		TouchRawPosTemp[i].y = (((Temp1<<8)|Temp2)>>3)&0xfff;
		
		//�ж��Ƿ��ڲɼ��������ͷ��˴���
		PGStatesTemp.whole = (uint16_t)GPIOG->IDR;
		if(PGStatesTemp.bit.penirq == PENIRQ_IDLE){
			SPI_NSS_HIGH;
			SPI4_ExitModeToDefault(); 
			//�ͷ�SPI4��Դ
			SPI4ResourceNum = 1;

			return ERROR;
		}
	}
	
	//���ߴӻ�NSS��֪ͨ�ӻ�����ͨѶ
	SPI_NSS_HIGH;
    SPI4_ExitModeToDefault(); 
	
	//�ͷ�SPI4��Դ
	SPI4ResourceNum = 1;
	
	//������ƽ��ֵ
	for(i=0;i<6;i++){
		for(j=0;j<5-i;j++){
			if(TouchRawPosTemp[j].x > TouchRawPosTemp[j+1].x){
				Temp1 = TouchRawPosTemp[j].x;
				TouchRawPosTemp[j].x = TouchRawPosTemp[j+1].x;
				TouchRawPosTemp[j+1].x = Temp1;
			}
		}
	}
	
	for(i=0;i<6;i++){
		for(j=0;j<5-i;j++){
			if(TouchRawPosTemp[j].y > TouchRawPosTemp[j+1].y){
				Temp1 = TouchRawPosTemp[j].y;
				TouchRawPosTemp[j].y = TouchRawPosTemp[j+1].y;
				TouchRawPosTemp[j+1].y = Temp1;
			}
		}
	}
	
	Sum = TouchRawPosTemp[1].x + TouchRawPosTemp[2].x + TouchRawPosTemp[3].x + TouchRawPosTemp[4].x;
	TouchRawPos->x = Sum>>2;
	
	Sum = TouchRawPosTemp[1].y + TouchRawPosTemp[2].y + TouchRawPosTemp[3].y + TouchRawPosTemp[4].y;
	TouchRawPos->y = (Sum>>2);
	
	
	
	return SUCCESS;
}



///***
//*���ش�����X���Y������ص�����ֵ
//***/
//ErrorStatus Touch_GetPixelPosValue(TouchPos_s* TouchPos)
//{
////	extern TouchAdjustPara_s MachInfo.touch;
//    extern MachInfo_s MachInfo;
//	TouchPos_s TouchRawPos = {0};
//	ErrorStatus ReturnState = SUCCESS;
//	
//	//��ȡ����ԭʼֵ
//	ReturnState = XPT2046_GetRawPosValue(&TouchRawPos);
//	
//	if(ReturnState == SUCCESS){
//		//�����Ӧ����������ֵ
//		TouchPos->x = MachInfo.tTouchAdjustPara.a1*TouchRawPos.x+MachInfo.tTouchAdjustPara.b1*TouchRawPos.y+MachInfo.tTouchAdjustPara.c1;
//		TouchPos->y = MachInfo.tTouchAdjustPara.a2*TouchRawPos.x+MachInfo.tTouchAdjustPara.b2*TouchRawPos.y+MachInfo.tTouchAdjustPara.c2;
//	}else{
//		return ERROR;
//	}
//	
//    //LOG_Info("X(%d)=%d, Y(%d)=%d", TouchPos->x , TouchRawPos.x, TouchPos->y, TouchRawPos.y);
//	return SUCCESS;
//}





