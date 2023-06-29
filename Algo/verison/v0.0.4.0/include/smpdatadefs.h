#ifndef _SMP_DATA_DEFS_H_
#define _SMP_DATA_DEFS_H_


#include "pdatatype.h"

// ��ѧͨ��������Ϣ
typedef struct tag_pulse_info_o
{
	short lsPeak;       // LS��ֵ��ԭʼ��
	short msPeak;       // MS��ֵ��ԭʼ��
	short hsPeak;       // HS��ֵ��ԭʼ��
	short lsPeakL;      // LS��ֵ��������
	short msPeakL;      // MS��ֵ��������
	short hsPeakL;      // HS��ֵ��������
	short fullWidth;    // ȫ���
	short timeStamp;    // ʱ���
	short cellType;     // ϸ������
	long  peakPos;      // ��ֵλ��
}pulse_info_o_t;

// �迹ͨ��������Ϣ
typedef struct tag_pulse_info_i
{
	short dcPeak;       // DC��ֵ
	short rfPeak;       // RF��ֵ
	short priWidth;     // ǰ���
	short subWidth;     // ����
	short fullWidth;    // ȫ���
	short baseLine;     // �������
	short timeStamp;    // ʱ���
	short pulseType;    // �������ͣ�0--������..��
	long  peakPos;      // ��ֵλ��
}pulse_info_i_t;

// �迹ͨ������ʶ�����
typedef struct tag_pulse_iden_para_i
{
	uint16          pulseHeightTh;              // ����߶�ʶ����ֵ
	uint16          minPeakHeight;              // ��С�����ֵ
	uint16          maxPeakHeight;              // ��������ֵ
	uint16          minPulseWidth;              // ��С������
	uint16          maxPulseWidth;              // ���������
	uint16          minRiseRate1;               // ��С�״���������
	uint16          minRiseRate2;               // ��С�ڶ�����������
	uint16          minMeanRiseRate;            // ��Сƽ����������
	uint16          minMeanFallRate;            // ��Сƽ���½�����
	uint16          topHeightTh;                // ��ֵ��߶���ֵ
	uint16          maxTopWidth;                // ���������ֵ��������
	uint16          startHPerTh;                // ��ʼ��߶Ȱٷֱ���ֵ
	uint16          normalEndHPerTh;            // һ�㷽ʽ�������߶Ȱٷֱ���ֵ
	uint16          valeEndHPerTh;              // �ȵ㷽ʽ�������߶Ȱٷֱ���ֵ
	uint16          valeEndHPeakTh;             // �ȵ㷽ʽ����ʱ�����ֵ
}pulse_iden_para_i_t;

#endif // _SMP_DATA_DEFS_H_

