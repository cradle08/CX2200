#ifndef _SERVICEITEMS_H
#define _SERVICEITEMS_H



#include "stm32f4xx_hal.h"
#include "bsp_rtc.h"




/***
*̽���ȶ��Բ��Խ���ṹ��
***/
typedef __packed struct _TanZhenTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;					//��ʼʱ��
		RTC_HYM8563Info_s endTime;						//����ʱ��
	}rtc;
	
	//С�׵�ѹ
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
*���������ϻ��и���Ŀ�쳣���������һ���쳣ֵ�ṹ��
***/
typedef __packed struct _AgingTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;									//�����ܴ���
	
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
	
	//����Դ
	__packed struct{
		uint16_t count;
		uint16_t maxValue;
	}hly;
	
	//SDRAM
	uint16_t sdramCount;
	
	//NAND
	uint16_t nandFlashCount;
	
	//���ذ��ϵ�EEPROM
	uint16_t eepromCount1;
	
	//ת�Ӱ��ϵ�EEPROM
	uint16_t eepromCount2;
	

	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;					//��ʼʱ��
		RTC_HYM8563Info_s endTime;						//����ʱ��
		int16_t secondOffset;							//����ʱ���ڲ���ʱ������ƫ��ֵ����ֵ��ʾ��ʵ�ʿ죬��ֵ��ʾ��ʵ����
	}rtc;
	
	//��ӡ��ͨѶ�쳣����
	uint16_t printerCount;
	
	//CAT1
	__packed struct{
		uint32_t successCount;
		uint32_t failCount;
	}cat;
	
	//����״̬����صĸ�����
	__packed struct{
		//����
		__packed struct{	
			uint16_t inOCCount;							//���ֹ���
			uint16_t outOCCount;						//���ֹ���
			uint16_t gdOCCount;							//�̶�����
			uint16_t releaseOCCount;					//�ͷŹ���
		}oc;
		
		//С�׵�ѹ
		__packed struct{
			uint16_t count;
			uint16_t maxValue;
		}xk;
		
		//ѹ��ϵͳ
		__packed struct{
			uint16_t buildCount;						//��ѹ�쳣����
			int32_t maxBuildPressureValue;				//��ѹֵƫ�����ֵ
			                                            
			uint16_t airtightCount;						//�ܱ����쳣����
			int32_t maxairtightValue;					//�ܱ���ƫ�����ֵ
			                                            
			uint16_t noPressureCount;					//���������ѹ���쳣����
			int32_t maxnoPressureValue;					//���������ѹ�����ֵ
		}pressure;
		
		//HGB
		__packed struct{
			uint16_t openCount;
			uint16_t maxOpenValue;
			
			uint16_t closeCount;
			uint16_t maxCloseValue;
		}hgb;
		
		//���������޼��
		uint16_t JscDetectionCount;
		
		//���������ͼ��
		uint16_t JscTypeCheckCount;
		
		//�źż��
		TanZhenTestResultInfo_s signal;
	}inside;
	
	//����״̬����صĸ�����
	__packed struct{
		//����
		__packed struct{	
			uint16_t inOCCount;							//���ֹ���
			uint16_t outOCCount;						//���ֹ���
			uint16_t gdOCCount;							//�̶�����
			uint16_t releaseOCCount;					//�ͷŹ���
		}oc;
		
		//С�׵�ѹ
		__packed struct{
			uint16_t count;
			uint16_t maxValue;
		}xk;
		
		//ѹ��ϵͳ
		__packed struct{
			uint16_t buildCount;						//��ѹ�쳣����
			int32_t maxBuildPressureValue;				//��ѹֵƫ�����ֵ
			
			uint16_t airtightCount;						//�ܱ����쳣����
			int32_t maxairtightValue;					//�ܱ���ƫ�����ֵ
			
			uint16_t noPressureCount;					//���������ѹ���쳣����
			int32_t maxnoPressureValue;					//���������ѹ�����ֵ
		}pressure;
		
		//HGB
		__packed struct{
			uint16_t openCount;
			uint16_t maxOpenValue;
			
			uint16_t closeCount;
			uint16_t maxCloseValue;
		}hgb;
		
		//�����ؼ��
		uint16_t JscDetectionCount;		

		//���������ͼ��
		uint16_t JscTypeCheckCount;
		
		//�źż��
		TanZhenTestResultInfo_s signal;
	}outside;
}AgingTestResultInfo_s;


/***
*�����ܱ��Բ��Խ���ṹ��
***/
typedef __packed struct _QizuiTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//��ʼʱ��
		RTC_HYM8563Info_s endTime;					//����ʱ��
	}rtc;
	
	__packed struct{
		uint16_t buildCount;						//��ѹ�쳣����
		int32_t maxBuildPressureValue;				//��ѹֵƫ�����ֵ
		
		uint16_t airtightCount;						//�ܱ����쳣����
		int32_t maxairtightValue;					//�ܱ���ƫ�����ֵ
		
		uint16_t noPressureCount;					//���������ѹ���쳣����
		int32_t maxnoPressureValue;					//���������ѹ�����ֵ
	}pressure;
}QizuiTestResultInfo_s;



/***
*�����Խ���ṹ��
***/
typedef __packed struct _ValveTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//��ʼʱ��
		RTC_HYM8563Info_s endTime;					//����ʱ��
	}rtc;
}ValveTestResultInfo_s;



/***
*��ѹ�ϻ����Խ���ṹ��
***/
typedef __packed struct _BuildPressureTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//��ʼʱ��
		RTC_HYM8563Info_s endTime;					//����ʱ��
	}rtc;
	
	//10kpa
	__packed struct{
		uint16_t buildCount;						//��ѹ�쳣����
		uint32_t time;								//��ѹʱ�䣬ms
	}pressure10;
	
	//20kpa
	__packed struct{
		uint16_t buildCount;						//��ѹ�쳣����
		uint32_t time;								//��ѹʱ�䣬ms
	}pressure20;
	
	//30kpa
	__packed struct{
		uint16_t buildCount;						//��ѹ�쳣����
		uint32_t time;								//��ѹʱ�䣬ms
	}pressure30;
	
	//40kpa
	__packed struct{
		uint16_t buildCount;						//��ѹ�쳣����
		uint32_t time;								//��ѹʱ�䣬ms
	}pressure40;
	
	//50kpa
	__packed struct{
		uint16_t buildCount;						//��ѹ�쳣����
		uint32_t time;								//��ѹʱ�䣬ms
	}pressure50;
}BuildPressureTestResultInfo_s;




/***
*�����缫���Խ���ṹ��
***/
typedef __packed struct _DLElecTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//��ʼʱ��
		RTC_HYM8563Info_s endTime;					//����ʱ��
	}rtc;
	
	uint32_t activeStatusErrCount;					//����̬�������
	uint32_t idleStatusErrCount;					//�Ǵ���̬�������
}DLElecTestResultInfo_s;



/***
*�����ֲ��Խ���ṹ��
***/
typedef __packed struct _OutInTestResultInfo_s{
	uint32_t setCount;
	uint32_t runCount;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//��ʼʱ��
		RTC_HYM8563Info_s endTime;					//����ʱ��
	}rtc;
	
	uint32_t outErrCount;							//�����ֵ�����ִ������
	uint32_t inErrCount;							//�����ֵ�����ִ������
	uint32_t gdErrCount;							//̽�����̶��������
	uint32_t sfErrCount;							//̽�����ͷŴ������
}OutInTestResultInfo_s;



/***
*�����ϻ����Խ���ṹ��
***/
typedef __packed struct _SCAgingTestResultInfo_s{
	//ִ�д���
	__packed struct{
		uint16_t agingTotal;
		uint16_t aging;
	}count;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//��ʼʱ��
		RTC_HYM8563Info_s endTime;					//����ʱ��
	}rtc;
	
	//�����ϻ�
	__packed struct{
		uint32_t stopFlag:					1;							//Ϊ0��ʾͨ����Ϊ1��ʾ��ͨ��
		uint32_t p12vFlag:					1;							//Ϊ0��ʾͨ����Ϊ1��ʾ��ͨ��
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
	
//	//̽���ϻ�
//	__packed struct{
//		uint32_t stopFlag:					1;							//Ϊ0��ʾͨ����Ϊ1��ʾ��ͨ��
//		uint32_t xkFlag:					1;
//		uint32_t cvFlag:					1;
//		uint32_t meanFlag:					1;
//		uint32_t numFlag:					1;
//		uint32_t :							27;
//	}tanzhen;
}SCAgingTestResultInfo_s;



/***
*���ݴ洢���Խ���ṹ��
***/
typedef __packed struct _DataSaveTestResultInfo_s{
	//ִ�д���
	__packed struct{
		uint32_t current;
		uint32_t total;
		uint32_t err;
	}count;
	
	//RTC
	__packed struct{
		RTC_HYM8563Info_s startTime;				//��ʼʱ��
		RTC_HYM8563Info_s endTime;					//����ʱ��
	}rtc;
	
	//����ʱֵ���
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
