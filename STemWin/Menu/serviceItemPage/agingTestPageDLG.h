#ifndef _AGINGTESTPAGEDLG_H
#define _AGINGTESTPAGEDLG_H



#include "stm32f4xx_hal.h"
#include "serviceItems.h"
#include "msg_def.h"




/***
*定义组件老化界面将数据发送给后台执行的结构体
***/
typedef __packed struct _AgingTestUIToBackendInfo_s{
	Msg_Head_t msgHead;
	uint32_t count;
}AgingTestUIToBackendInfo_s;






/***
 定义组件老化界面给UI显示的页面
***/ 
typedef enum _ServiceAgingTestUIPage_e{
	TANZHEN_TEST_RESULT = 0,
	AGING_TEST_RESULT,
	MOTOR_OUT_IN_RESULT,
	QIZUI_TEST_RESULT,
	AIR_TEST_RESULT,
	YEFA_TEST_RESULT,
	BUILD_PRESSURE_TEST_RESULT,
	DL_ELEC_TEST_RESULT,
	SC_AGING_TEST_RESULT,
	DATA_SAVE_TEST_RESULT,
}ServiceUIPage_e;



/***
*定义给界面上显示的相关数据结构体
***/
typedef struct _ServiceAgingTestUIInfo_s{
	uint8_t startFlag;
	uint8_t stopFlag;
	ServiceUIPage_e page;
	
	union{
		AgingTestResultInfo_s agingTestResultInfo;						//流程老化测试结果数据
		TanZhenTestResultInfo_s tanzhenTestInfo;						//探针老化测试结果数据
		QizuiTestResultInfo_s qizuiTestResultInfo;						//气嘴密闭性老化测试结果数据
		ValveTestResultInfo_s airValveTestResultInfo;					//气阀老化测试结果数据
		ValveTestResultInfo_s yeFaValveTestResultInfo;					//液阀老化测试结果数据
		BuildPressureTestResultInfo_s buildPressureTestResultInfo;		//建压老化测试结果数据
		DLElecTestResultInfo_s dlElecTestResultInfo;					//定量电极老化测试结果数据
		OutInTestResultInfo_s outInTestResultInfo;						//进出仓老化测试结果数据
		SCAgingTestResultInfo_s scAgingTestResultInfo;					//生产老化测试结果数据
		DataSaveTestResultInfo_s dataSaveTestResultInfo;				//数据存储测试结果数据
	}unions;
}ServiceAgingTestUIInfo_s;


















#endif
