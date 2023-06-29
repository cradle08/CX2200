/*
* Copyright (c) ���ڴ���ҽ�ƿƼ����޹�˾
* All right reserved.
*
* �ļ����ƣ�algpulse.h
*
*/

#ifndef _ALG_PULSE_H_
#define _ALG_PULSE_H_

#include "pdatatype.h"
#include "smpdatadefs.h"
#include "channaldefs.h"
#include "modedefs.h"

#define PRE_PULSE_NUM  250000 //���������
#define DATA_UINT_SIZE 1024  //ÿ�����ݴ�СΪ1024�ֽ�

// ״̬����ȡֵ
enum 
{ 
	CS_IDLE = 0,
	CS_START,
	CS_END
};

// ����ʶ��������Ϣ
typedef struct tag_pulse_feature_info
{
	uint16          peaky;                      // �����ֵ
	uint32          peakx;                      // �����ֵλ��
	uint16          starty;                     // ��ʼ��߶�
	uint32          startx;                     // ��ʼ��λ��
	uint16          endy;                       // ������߶�
	uint32          endx;                       // ������λ��
	uint16          fpeaky;                     // ǰ��߶�
	uint32          fpeakx;                     // ǰ��λ��
	uint16          bpeaky;                     // ���߶�
	uint32          bpeakx;                     // ���λ��
	uint16          valey;                      // �ȸ߶�
	uint32          valex;                      // ��λ��
	uint32          width;                      // ������
	uint32          topWidth;                   // ������ֵ��������
	uint32          peakNum;                    // ����Ŀ
	uint8           valeFlag;                   // �ȱ�־
}pulse_feature_info_t;

// ����ʶ���м�״̬��Ϣ
typedef struct tag_pulse_mid_feature
{
	int16					nCS;				// �ź�״̬, IDLE --> START --> END --> IDLE
	pulse_feature_info_t	featureInfo;		// ����ʶ��������Ϣ
	uint16					firData;			// pulseData�ĵ�һ������
	uint16					secData;			// pulseData�ĵڶ�������
	uint16					thiData;			// pulseData�ĵ���������
	double64				smpRate;			// ���ݵĲ���Ƶ��
	uint32					iterNum;			//������ִ�д���
}pulse_mid_feature;

// ��ʼ��������Ϣ
void pulseInit(pulse_info_i_t *pulseInfo);

// WBCͨ������ʶ���迹��
BOOL8 wbcPulseIdentify(pulse_info_i_t *wbcPulseInfo, uint32 *pulseNum, const uint8 *pulseData, long dataLen, pulse_mid_feature *midFeature);

// RBCͨ������ʶ���迹��
BOOL8 rbcPulseIdentify(pulse_info_i_t *rbcPulseInfo, uint32 *pulseNum, const uint8 *pulseData, long dataLen, pulse_mid_feature *midFeature);

// PLTͨ������ʶ���迹��
BOOL8 pltPulseIdentify(pulse_info_i_t *pltPulseInfo, uint32 *pulseNum, const uint8 *pulseData, long dataLen, pulse_mid_feature *midFeature);

#endif // _ALGPULSE_H_

