/**************************************************************************
*电阻触摸相关，使用XPT2046芯片，SPI接口，极性模式0，这里由于XPT2046时钟才2M
所以SPI接口时钟不能太快，所以需要分成256分频来使用
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



/*宏定义区****************************************************************/
#define SPI_NSS_LOW						SPI_CS_Enable(SPI_CS_TP)    //	TP_SPI_CS_GPIO_Port->BSRR |= TP_SPI_CS_Pin<<16		//定义输出NSS1为低电平
#define SPI_NSS_HIGH					SPI_CS_Disable(SPI_CS_TP)   //	TP_SPI_CS_GPIO_Port->BSRR |= TP_SPI_CS_Pin			//定义输出NSS1为高电平
		
#define INVALID_VALUE						0x00												//定义用于读取数据使用的无效数据
#define SPI_MAX_TIMEOUT						500000												//定义SPI读写一个字节最大的超时时间

//===触摸芯片SPI通讯命令字======
#define CMD_TOUCH_X_POS						0xD0												//用于获取电阻触摸中的X位置的命令字
#define CMD_TOUCH_Y_POS						0x90												//用于获取电阻触摸中的Y位置的命令字


//===校准点像素坐标值===
#define TOUCH_ADJUST_PIXEL_X_0				100//200
#define TOUCH_ADJUST_PIXEL_Y_0				100

#define TOUCH_ADJUST_PIXEL_X_1				700//600
#define TOUCH_ADJUST_PIXEL_Y_1				100

#define TOUCH_ADJUST_PIXEL_X_2				400//600
#define TOUCH_ADJUST_PIXEL_Y_2				380//400

#define TOUCH_ADJUST_PIXEL_X_3				400
#define TOUCH_ADJUST_PIXEL_Y_3				200



/***
*定义PB对应的输入通道对应的IO的实际状态联合体
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










/*全局变量定义区***************************************************/
PGStates_u PGStates = {0};									

uint16_t PGFirstInputState = 0;
uint16_t PGSecondInputState = 0;	

//TouchAdjustPara_s MachInfo.touch = {0};
TouchPos_s TouchPixelPos = {0};



/*
*   xpt2046, spi口初始化
*/
void XPT2046_Init(void)
{
    //MX_SPI4_Init_LowSpeed();
    //MX_SPI4_Init();    
    SPI4_ChangeModeTo(SPI_MODE_TP);//
}


/*内部函数区***************************************************************/
/***
*对比首次扫描IO和消抖后的扫描IO的结果，从而得到此次IO扫描阶段的最终状态结果
***/
static void _InputStateCompare()
{
	extern PGStates_u PGStates;
	
	extern uint16_t PGFirstInputState;
	extern uint16_t PGSecondInputState;
	
	uint16_t i;
	uint16_t IOStateTemp;
	
	//PB端口比较
	for(i=0;i<16;i++){
		IOStateTemp = (PGFirstInputState>>i)&0x01;
		if(IOStateTemp == ((PGSecondInputState>>i)&0x01)){				//判断两次采集的输入通道IO状态是否一致，一致才保存的状态，不一致则直接忽略这次采集
			if(IOStateTemp){
				PGStates.whole |= 1<<i;
			}else{
				PGStates.whole &= ~(1<<i);
			}
		}
	}
}




/***
*读写一个字节，用于写入一个字节，或者读出一个字节
*参数：
	Data：要写入的数据，或者为了读出一个字节而使用的无效字节驱动主机SPI发出时钟
*返回值：需要读回的一个字节，或者由于写入一个字节从而从机返回的无效数据
***/
static uint8_t _SPI_RW(uint8_t Data)
{
	extern SPI_HandleTypeDef hspi4;
	uint32_t Count = 0;
	
	//判断TXE是否为1，避免出现覆盖数据的现象
	while(__HAL_SPI_GET_FLAG(&hspi4,SPI_FLAG_TXE)==RESET){
		Count++;
		if(Count>=SPI_MAX_TIMEOUT){
			return 0;
		}
	}
	hspi4.Instance->DR = Data;
	
	//判断RXNE是否为1，从而得知数据是否已经接收完成,当RXNE为1时，表明一轮数据收发完成了，可以按需要将NSS拉高了
	Count = 0;
	while(__HAL_SPI_GET_FLAG(&hspi4,SPI_FLAG_RXNE)==RESET){
		Count++;
		if(Count>=SPI_MAX_TIMEOUT){
			return 0;
		}
	}
	return hspi4.Instance->DR;
}








/*外部函数区**************************************************************/
/***
*扫描PENIRQ输入IO的状态，此函数需要在循环中使用，因为有消抖的功能
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
		
		//比较结果
		_InputStateCompare();
	}
}



/***
*返回PENIRQ的输入状态
***/
uint8_t PenIRQ_GetState(void)
{
	extern PGStates_u PGStates;
	
	return PGStates.bit.penirq;
}



/***
*读取X轴和Y轴的触摸原始坐标值，这里连续读取6次，去掉最高和最低值后，求平均
*参数：
	TouchRawPos：保存采集到的触摸原始坐标值（平均后的值）
*返回值:当前采集到的X轴和Y轴的触摸原始坐标值成功时返回SUCCESS，否则返回失败
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
	//拉低从机NSS，通知从机开始通讯
	SPI_NSS_LOW;
	
	for(i=0;i<6;i++){
		//这里采用24个时钟时序的模式，避免导致PENIRQ引脚在采集过程中被屏蔽，从而采集到错误的数据
		_SPI_RW(CMD_TOUCH_X_POS);
		Temp1 = _SPI_RW(INVALID_VALUE);
		Temp2 = _SPI_RW(INVALID_VALUE);
		
		TouchRawPosTemp[i].x = (((Temp1<<8)|Temp2)>>3)&0xfff;
		
		_SPI_RW(CMD_TOUCH_Y_POS);
		Temp1 = _SPI_RW(INVALID_VALUE);
		Temp2 = _SPI_RW(INVALID_VALUE);
		
		TouchRawPosTemp[i].y = (((Temp1<<8)|Temp2)>>3)&0xfff;
		
		//判断是否在采集过程中释放了触摸
		PGStatesTemp.whole = (uint16_t)GPIOG->IDR;
		if(PGStatesTemp.bit.penirq == PENIRQ_IDLE){
			SPI_NSS_HIGH;
			SPI4_ExitModeToDefault(); 
			//释放SPI4资源
			SPI4ResourceNum = 1;

			return ERROR;
		}
	}
	
	//拉高从机NSS，通知从机结束通讯
	SPI_NSS_HIGH;
    SPI4_ExitModeToDefault(); 
	
	//释放SPI4资源
	SPI4ResourceNum = 1;
	
	//排序求平均值
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
//*返回触摸点X轴和Y轴的像素点坐标值
//***/
//ErrorStatus Touch_GetPixelPosValue(TouchPos_s* TouchPos)
//{
////	extern TouchAdjustPara_s MachInfo.touch;
//    extern MachInfo_s MachInfo;
//	TouchPos_s TouchRawPos = {0};
//	ErrorStatus ReturnState = SUCCESS;
//	
//	//获取触摸原始值
//	ReturnState = XPT2046_GetRawPosValue(&TouchRawPos);
//	
//	if(ReturnState == SUCCESS){
//		//计算对应的像素坐标值
//		TouchPos->x = MachInfo.tTouchAdjustPara.a1*TouchRawPos.x+MachInfo.tTouchAdjustPara.b1*TouchRawPos.y+MachInfo.tTouchAdjustPara.c1;
//		TouchPos->y = MachInfo.tTouchAdjustPara.a2*TouchRawPos.x+MachInfo.tTouchAdjustPara.b2*TouchRawPos.y+MachInfo.tTouchAdjustPara.c2;
//	}else{
//		return ERROR;
//	}
//	
//    //LOG_Info("X(%d)=%d, Y(%d)=%d", TouchPos->x , TouchRawPos.x, TouchPos->y, TouchRawPos.y);
//	return SUCCESS;
//}





