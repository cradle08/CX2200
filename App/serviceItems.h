#ifndef _SERVICEITEMS_H
#define _SERVICEITEMS_H



#include "stm32f4xx_hal.h"
#include "bsp_rtc.h"




/***
*探针稳定性测试结果结构体
***/
typedef __packed struct _TanZhenTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;					//开始时间
		RTC_HYM8563Info_s endTime;						//结束时间
	}rtc;
	
	//小孔电压
	__packed struct{
		uint16_t errCount;
		uint16_t maxValue;
	}xk;
	
	__packed struct{
		uint16_t errCount;
		double maxCV;
	}cv;
	
	__packed struct{
		uint16_t errCount;
		double maxMean;
	}mean;
	
	__packed struct{
		uint16_t errCount;
		uint16_t maxNum;
	}num;
}TanZhenTestResultInfo_s;




/***
*定义流程老化中各项目异常次数及最后一次异常值结构体
***/
typedef __packed struct _AgingTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;									//运行总次数
	
	//VCC_P12V
	__packed struct{
		uint16_t count;
		uint16_t maxValue;
	}p12v;
	
	//VCC_N12V
	__packed struct{
		uint16_t count;
		int16_t maxValue;
	}n12v;
	
	//恒流源
	__packed struct{
		uint16_t count;
		uint16_t maxValue;
	}hly;
	
	//SDRAM
	uint16_t sdramCount;
	
	//NAND
	uint16_t nandFlashCount;
	
	//主控板上的EEPROM
	uint16_t eepromCount1;
	
	//转接板上的EEPROM
	uint16_t eepromCount2;
	

	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;					//开始时间
		RTC_HYM8563Info_s endTime;						//结束时间
		int16_t secondOffset;							//结束时与内部定时器的秒偏差值，正值表示比实际快，负值表示比实际慢
	}rtc;
	
	//打印机通讯异常次数
	uint16_t printerCount;
	
	//CAT1
	__packed struct{
		uint32_t successCount;
		uint32_t failCount;
	}cat;
	
	//进仓状态下相关的各参数
	__packed struct{
		//光耦
		__packed struct{	
			uint16_t inOCCount;							//进仓光耦
			uint16_t outOCCount;						//出仓光耦
			uint16_t gdOCCount;							//固定光耦
			uint16_t releaseOCCount;					//释放光耦
		}oc;
		
		//小孔电压
		__packed struct{
			uint16_t count;
			uint16_t maxValue;
		}xk;
		
		//压力系统
		__packed struct{
			uint16_t buildCount;						//建压异常次数
			int32_t maxBuildPressureValue;				//建压值偏差最大值
			                                            
			uint16_t airtightCount;						//密闭性异常次数
			int32_t maxairtightValue;					//密闭性偏差最大值
			                                            
			uint16_t noPressureCount;					//放气后残余压力异常次数
			int32_t maxnoPressureValue;					//放气后残余压力最大值
		}pressure;
		
		//HGB
		__packed struct{
			uint16_t openCount;
			uint16_t maxOpenValue;
			
			uint16_t closeCount;
			uint16_t maxCloseValue;
		}hgb;
		
		//计数池有无检测
		uint16_t JscDetectionCount;
		
		//计数池类型检测
		uint16_t JscTypeCheckCount;
		
		//信号检测
		TanZhenTestResultInfo_s signal;
	}inside;
	
	//出仓状态下相关的各参数
	__packed struct{
		//光耦
		__packed struct{	
			uint16_t inOCCount;							//进仓光耦
			uint16_t outOCCount;						//出仓光耦
			uint16_t gdOCCount;							//固定光耦
			uint16_t releaseOCCount;					//释放光耦
		}oc;
		
		//小孔电压
		__packed struct{
			uint16_t count;
			uint16_t maxValue;
		}xk;
		
		//压力系统
		__packed struct{
			uint16_t buildCount;						//建压异常次数
			int32_t maxBuildPressureValue;				//建压值偏差最大值
			
			uint16_t airtightCount;						//密闭性异常次数
			int32_t maxairtightValue;					//密闭性偏差最大值
			
			uint16_t noPressureCount;					//放气后残余压力异常次数
			int32_t maxnoPressureValue;					//放气后残余压力最大值
		}pressure;
		
		//HGB
		__packed struct{
			uint16_t openCount;
			uint16_t maxOpenValue;
			
			uint16_t closeCount;
			uint16_t maxCloseValue;
		}hgb;
		
		//计数池检测
		uint16_t JscDetectionCount;		

		//计数池类型检测
		uint16_t JscTypeCheckCount;
		
		//信号检测
		TanZhenTestResultInfo_s signal;
	}outside;
}AgingTestResultInfo_s;


/***
*气嘴密闭性测试结果结构体
***/
typedef __packed struct _QizuiTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//开始时间
		RTC_HYM8563Info_s endTime;					//结束时间
	}rtc;
	
	__packed struct{
		uint16_t buildCount;						//建压异常次数
		int32_t maxBuildPressureValue;				//建压值偏差最大值
		
		uint16_t airtightCount;						//密闭性异常次数
		int32_t maxairtightValue;					//密闭性偏差最大值
		
		uint16_t noPressureCount;					//放气后残余压力异常次数
		int32_t maxnoPressureValue;					//放气后残余压力最大值
	}pressure;
}QizuiTestResultInfo_s;



/***
*阀测试结果结构体
***/
typedef __packed struct _ValveTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//开始时间
		RTC_HYM8563Info_s endTime;					//结束时间
	}rtc;
}ValveTestResultInfo_s;



/***
*建压老化测试结果结构体
***/
typedef __packed struct _BuildPressureTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//开始时间
		RTC_HYM8563Info_s endTime;					//结束时间
	}rtc;
	
	//10kpa
	__packed struct{
		uint16_t buildCount;						//建压异常次数
		uint32_t time;								//建压时间，ms
	}pressure10;
	
	//20kpa
	__packed struct{
		uint16_t buildCount;						//建压异常次数
		uint32_t time;								//建压时间，ms
	}pressure20;
	
	//30kpa
	__packed struct{
		uint16_t buildCount;						//建压异常次数
		uint32_t time;								//建压时间，ms
	}pressure30;
	
	//40kpa
	__packed struct{
		uint16_t buildCount;						//建压异常次数
		uint32_t time;								//建压时间，ms
	}pressure40;
	
	//50kpa
	__packed struct{
		uint16_t buildCount;						//建压异常次数
		uint32_t time;								//建压时间，ms
	}pressure50;
}BuildPressureTestResultInfo_s;




/***
*定量电极测试结果结构体
***/
typedef __packed struct _DLElecTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//开始时间
		RTC_HYM8563Info_s endTime;					//结束时间
	}rtc;
	
	uint32_t activeStatusErrCount;					//触发态错误次数
	uint32_t idleStatusErrCount;					//非触发态错误次数
}DLElecTestResultInfo_s;



/***
*进出仓测试结果结构体
***/
typedef __packed struct _OutInTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//开始时间
		RTC_HYM8563Info_s endTime;					//结束时间
	}rtc;
	
	uint32_t outErrCount;							//进出仓电机出仓错误次数
	uint32_t inErrCount;							//进出仓电机进仓错误次数
	uint32_t gdErrCount;							//探针电机固定错误次数
	uint32_t sfErrCount;							//探针电机释放错误次数
}OutInTestResultInfo_s;



/***
*生产老化测试结果结构体
***/
typedef __packed struct _SCAgingTestResultInfo_s{
	//执行次数
	__packed struct{
		uint16_t agingTotal;
		uint16_t aging;
	}count;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//开始时间
		RTC_HYM8563Info_s endTime;					//结束时间
	}rtc;
	
	//流程老化
	__packed struct{
		uint32_t stopFlag:					1;							//为0表示通过，为1表示不通过
		uint32_t p12vFlag:					1;							//为0表示通过，为1表示不通过
		uint32_t n12vFlag:					1;
		uint32_t v55Flag:					1;
		uint32_t sdramFlag:					1;
		uint32_t nandFlag:					1;
		uint32_t mEEPROMFlag:				1;
		uint32_t sEEPROMFlag:				1;
		uint32_t rtcFlag:					1;
		uint32_t catFlag:					1;
		uint32_t ntcFlag:					1;
		uint32_t buildPressFlag:			1;
		uint32_t qimiFlag:					1;
		uint32_t fangqiFlag:				1;
		uint32_t outInFlag:					1;
		uint32_t hgbFlag:					1;
		uint32_t :							16;
	}aging;
	
//	//探针老化
//	__packed struct{
//		uint32_t stopFlag:					1;							//为0表示通过，为1表示不通过
//		uint32_t xkFlag:					1;
//		uint32_t cvFlag:					1;
//		uint32_t meanFlag:					1;
//		uint32_t numFlag:					1;
//		uint32_t :							27;
//	}tanzhen;
}SCAgingTestResultInfo_s;



/***
*数据存储测试结果结构体
***/
typedef __packed struct _DataSaveTestResultInfo_s{
	//执行次数
	__packed struct{
		uint32_t current;
		uint32_t total;
		uint32_t err;
	}count;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//开始时间
		RTC_HYM8563Info_s endTime;					//结束时间
	}rtc;
	
	//出错时值情况
	__packed struct{
		uint32_t write;
		uint32_t read;
	}errValue;
}DataSaveTestResultInfo_s;












HAL_StatusTypeDef AgingTest(uint32_t TestNum);
HAL_StatusTypeDef TanzhenTest(uint32_t TestNum);
HAL_StatusTypeDef QizuiTest(uint32_t TestNum);
HAL_StatusTypeDef CATDataTest(uint32_t StartSNNum);
void RandonTouchTestTask(void *argument);
HAL_StatusTypeDef ShenChanAgingTest(uint32_t SetCount);
HAL_StatusTypeDef DataSaveTest(uint32_t SetCount);


#endif
