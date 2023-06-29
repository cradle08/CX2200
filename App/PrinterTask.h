#ifndef _PRINTERTASK_H
#define _PRINTERTASK_H



#include "stm32f4xx_hal.h"
#include "msg_def.h"
#include "serviceItems.h"







/***
*定义打印界面枚举
***/
typedef enum _PrinterPage_e{
	SAMPLE_RESULT_PAGE = 0,
	AGING_TEST_RESULT_PAGE,
	TAN_ZHEN_TEST_RESULT_PAGE,
	MOTOR_OUT_IN_RESULT_PAGE,
	QIZUI_TEST_RESULT_PAGE,
	AIR_TEST_RESULT_PAGE,
	YEFA_TEST_RESULT_PAGE,
	BUILD_PRESSURE_TEST_RESULT_PAGE,
	DL_ELEC_TEST_RESULT_PAGE,
	QC_RESULT_PAGE,
	SC_AGING_TEST_RESULT_PAGE
}PrinterPage_e;



/***
*定义性别枚举
***/
typedef enum _Gender_e{
	NUL = 0,
	MALE,
	FEMALE
}Gender_e;



/***
*定义直方图通道x,y结构体
***/
typedef struct _HistogramPos_s{
	uint16_t x;
	uint16_t y;
}HistogramPos_s;









/***
*定义所有打印页面数据的联合体
***/
typedef struct _PrinterInfoData_s{
	PrinterPage_e page;
	
	union{
		//样本分析打印页面相关数据
		struct{
			char name[PATIENT_NAME_LEN];								//姓名，一个中文需要占用2个字节
			Gender_e gender;											//性别
			uint8_t ucAge;												//年龄
			AgeUnit_e eAgeUnit;											//年龄单位
			char sampleNumber[SAMPLE_SN_LEN];							//样本编号
			char checkTime[DATE_TIME_LEN];								//检验时间,格式：2020-12-03 15:22
			
			DataStatus_e eDataStatus;									//样本数据是否有效标志，无效时，打印--
			ReferGroup_e refGroups;                                     //参考组索引
			WBCHGB_ALERT_e alarmFlag[8];								//报警状态
							
			float wbc;													//WBC值，这里的值为实际值*100，当此值为0xffff时，则打印****
			float gran;													//中性粒细胞值，这里的值为实际值*100，当此值为0xffff时，则打印****
			float mid;													//中间群细胞值，这里的值为实际值*100，当此值为0xffff时，则打印****
			float lym;													//淋巴细胞值，这里的值为实际值*100，当此值为0xffff时，则打印****
								
			float granPercent;											//中性粒百分比，这里的值为实际值*100，当此值为0xffff时，则打印****
			float midPercent;											//中间群百分比，这里的值为实际值*100，当此值为0xffff时，则打印****
			float lymPercent;											//淋巴百分比，这里的值为实际值*100，当此值为0xffff时，则打印****
				
			float hgb;													//hgb值，这里的值为实际值*100，当此值为0xffff时，则打印****
							
			uint16_t histogramPos[GRAPH_DATA_LEN];						//直方图坐标值，需要将Y值的两位小数转成整数
			float crossLine[3];											//3条分割线X坐标值
		}samplePageInfo;
		
		
		//质控打印页面相关数据
		struct{
			uint16_t fileNum;											//文件号
            QC_FILENUM_INDEX_e eFileNumIndex;                           //文件号索引
			QC_LEVEL_e level;										    //水平，为0表示低，为1表示中，为2表示高
            QC_Ctrl_Status_e eQC_Ctrl_Status;							//质控状态，为0表示在控，为1表示失控
			char batchNum[QC_LOT_NUM_LEN];								//批号
			char qcSN[SAMPLE_SN_LEN];									//质控样本编号
			char indate[DATE_TIME_LEN];									//有效期
			char checkTime[DATE_TIME_LEN];								//检验时间
			char tip[100];												//提示信息
			
			ReferGroup_e refGroups;                                     //参考组索引
			WBCHGB_ALERT_e alarmFlag[8];								//报警状态
							
			float wbc;													//WBC值，这里的值为实际值*100，当此值为0xffff时，则打印****
			float gran;													//中性粒细胞值，这里的值为实际值*100，当此值为0xffff时，则打印****
			float mid;													//中间群细胞值，这里的值为实际值*100，当此值为0xffff时，则打印****
			float lym;													//淋巴细胞值，这里的值为实际值*100，当此值为0xffff时，则打印****
								
			float granPercent;											//中性粒百分比，这里的值为实际值*100，当此值为0xffff时，则打印****
			float midPercent;											//中间群百分比，这里的值为实际值*100，当此值为0xffff时，则打印****
			float lymPercent;											//淋巴百分比，这里的值为实际值*100，当此值为0xffff时，则打印****
				
			float hgb;													//hgb值，这里的值为实际值*100，当此值为0xffff时，则打印****
							
			uint16_t histogramPos[GRAPH_DATA_LEN];						//直方图坐标值，需要将Y值的两位小数转成整数
			float crossLine[3];											//3条分割线X坐标值
		}qcPageInfo;
		
			
		//服务--组件老化界面打印数据内容
		AgingTestResultInfo_s agingTestResultInfo;						//流程老化结果打印相关数据
		TanZhenTestResultInfo_s tanZhenTestErrInfo;						//探针老化测试相关数据
		OutInTestResultInfo_s outInTestResultInfo;						//进出仓老化测试结果数据
		QizuiTestResultInfo_s qizuiTestResultInfo;						//气嘴密闭性老化测试结果数据
		ValveTestResultInfo_s airValveTestResultInfo;					//气阀老化测试结果数据
		ValveTestResultInfo_s yeFaValveTestResultInfo;					//液阀老化测试结果数据
		BuildPressureTestResultInfo_s buildPressureTestResultInfo;		//建压老化测试结果数据
		DLElecTestResultInfo_s dlElecTestResultInfo;					//定量电极老化测试结果数据
		SCAgingTestResultInfo_s scAgingTestResultInfo;					//生产老化测试结果数据
	}pageUnion;
}PrinterInfoData_s;















void PrinterIsOKTask(void *argument);
void PrinterTask(void *argument);
void CreateAndSend_WBCHGB_PrintData(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);
void CreateAndSend_QC_WBCHGB_PrintData(__IO QC_WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);

void TestPrinter(void);












#endif
