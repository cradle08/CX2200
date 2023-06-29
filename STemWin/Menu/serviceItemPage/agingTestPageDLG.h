#ifndef _AGINGTESTPAGEDLG_H
#define _AGINGTESTPAGEDLG_H



#include "stm32f4xx_hal.h"
#include "serviceItems.h"
#include "msg_def.h"




/***
*��������ϻ����潫���ݷ��͸���ִ̨�еĽṹ��
***/
typedef __packed struct _AgingTestUIToBackendInfo_s{
	Msg_Head_t msgHead;
	uint32_t count;
}AgingTestUIToBackendInfo_s;






/***
 ��������ϻ������UI��ʾ��ҳ��
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
*�������������ʾ��������ݽṹ��
***/
typedef struct _ServiceAgingTestUIInfo_s{
	uint8_t startFlag;
	uint8_t stopFlag;
	ServiceUIPage_e page;
	
	union{
		AgingTestResultInfo_s agingTestResultInfo;						//�����ϻ����Խ������
		TanZhenTestResultInfo_s tanzhenTestInfo;						//̽���ϻ����Խ������
		QizuiTestResultInfo_s qizuiTestResultInfo;						//�����ܱ����ϻ����Խ������
		ValveTestResultInfo_s airValveTestResultInfo;					//�����ϻ����Խ������
		ValveTestResultInfo_s yeFaValveTestResultInfo;					//Һ���ϻ����Խ������
		BuildPressureTestResultInfo_s buildPressureTestResultInfo;		//��ѹ�ϻ����Խ������
		DLElecTestResultInfo_s dlElecTestResultInfo;					//�����缫�ϻ����Խ������
		OutInTestResultInfo_s outInTestResultInfo;						//�������ϻ����Խ������
		SCAgingTestResultInfo_s scAgingTestResultInfo;					//�����ϻ����Խ������
		DataSaveTestResultInfo_s dataSaveTestResultInfo;				//���ݴ洢���Խ������
	}unions;
}ServiceAgingTestUIInfo_s;


















#endif
