#ifndef _PRINTERTASK_H
#define _PRINTERTASK_H



#include "stm32f4xx_hal.h"
#include "msg_def.h"
#include "serviceItems.h"







/***
*�����ӡ����ö��
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
*�����Ա�ö��
***/
typedef enum _Gender_e{
	NUL = 0,
	MALE,
	FEMALE
}Gender_e;



/***
*����ֱ��ͼͨ��x,y�ṹ��
***/
typedef struct _HistogramPos_s{
	uint16_t x;
	uint16_t y;
}HistogramPos_s;









/***
*�������д�ӡҳ�����ݵ�������
***/
typedef struct _PrinterInfoData_s{
	PrinterPage_e page;
	
	union{
		//����������ӡҳ���������
		struct{
			char name[PATIENT_NAME_LEN];								//������һ��������Ҫռ��2���ֽ�
			Gender_e gender;											//�Ա�
			uint8_t ucAge;												//����
			AgeUnit_e eAgeUnit;											//���䵥λ
			char sampleNumber[SAMPLE_SN_LEN];							//�������
			char checkTime[DATE_TIME_LEN];								//����ʱ��,��ʽ��2020-12-03 15:22
			
			DataStatus_e eDataStatus;									//���������Ƿ���Ч��־����Чʱ����ӡ--
			ReferGroup_e refGroups;                                     //�ο�������
			WBCHGB_ALERT_e alarmFlag[8];								//����״̬
							
			float wbc;													//WBCֵ�������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
			float gran;													//������ϸ��ֵ�������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
			float mid;													//�м�Ⱥϸ��ֵ�������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
			float lym;													//�ܰ�ϸ��ֵ�������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
								
			float granPercent;											//�������ٷֱȣ������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
			float midPercent;											//�м�Ⱥ�ٷֱȣ������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
			float lymPercent;											//�ܰͰٷֱȣ������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
				
			float hgb;													//hgbֵ�������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
							
			uint16_t histogramPos[GRAPH_DATA_LEN];						//ֱ��ͼ����ֵ����Ҫ��Yֵ����λС��ת������
			float crossLine[3];											//3���ָ���X����ֵ
		}samplePageInfo;
		
		
		//�ʿش�ӡҳ���������
		struct{
			uint16_t fileNum;											//�ļ���
            QC_FILENUM_INDEX_e eFileNumIndex;                           //�ļ�������
			QC_LEVEL_e level;										    //ˮƽ��Ϊ0��ʾ�ͣ�Ϊ1��ʾ�У�Ϊ2��ʾ��
            QC_Ctrl_Status_e eQC_Ctrl_Status;							//�ʿ�״̬��Ϊ0��ʾ�ڿأ�Ϊ1��ʾʧ��
			char batchNum[QC_LOT_NUM_LEN];								//����
			char qcSN[SAMPLE_SN_LEN];									//�ʿ��������
			char indate[DATE_TIME_LEN];									//��Ч��
			char checkTime[DATE_TIME_LEN];								//����ʱ��
			char tip[100];												//��ʾ��Ϣ
			
			ReferGroup_e refGroups;                                     //�ο�������
			WBCHGB_ALERT_e alarmFlag[8];								//����״̬
							
			float wbc;													//WBCֵ�������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
			float gran;													//������ϸ��ֵ�������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
			float mid;													//�м�Ⱥϸ��ֵ�������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
			float lym;													//�ܰ�ϸ��ֵ�������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
								
			float granPercent;											//�������ٷֱȣ������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
			float midPercent;											//�м�Ⱥ�ٷֱȣ������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
			float lymPercent;											//�ܰͰٷֱȣ������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
				
			float hgb;													//hgbֵ�������ֵΪʵ��ֵ*100������ֵΪ0xffffʱ�����ӡ****
							
			uint16_t histogramPos[GRAPH_DATA_LEN];						//ֱ��ͼ����ֵ����Ҫ��Yֵ����λС��ת������
			float crossLine[3];											//3���ָ���X����ֵ
		}qcPageInfo;
		
			
		//����--����ϻ������ӡ��������
		AgingTestResultInfo_s agingTestResultInfo;						//�����ϻ������ӡ�������
		TanZhenTestResultInfo_s tanZhenTestErrInfo;						//̽���ϻ������������
		OutInTestResultInfo_s outInTestResultInfo;						//�������ϻ����Խ������
		QizuiTestResultInfo_s qizuiTestResultInfo;						//�����ܱ����ϻ����Խ������
		ValveTestResultInfo_s airValveTestResultInfo;					//�����ϻ����Խ������
		ValveTestResultInfo_s yeFaValveTestResultInfo;					//Һ���ϻ����Խ������
		BuildPressureTestResultInfo_s buildPressureTestResultInfo;		//��ѹ�ϻ����Խ������
		DLElecTestResultInfo_s dlElecTestResultInfo;					//�����缫�ϻ����Խ������
		SCAgingTestResultInfo_s scAgingTestResultInfo;					//�����ϻ����Խ������
	}pageUnion;
}PrinterInfoData_s;















void PrinterIsOKTask(void *argument);
void PrinterTask(void *argument);
void CreateAndSend_WBCHGB_PrintData(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);
void CreateAndSend_QC_WBCHGB_PrintData(__IO QC_WBCHGB_TestInfo_t *ptWBCHGB_TestInfo);

void TestPrinter(void);












#endif
