#ifndef _ABCOMPUTE_H_
#define _ABCOMPUTE_H_


#include "pdatatype.h"
#include "smpdatadefs.h"

// ���(int/double)
double getSum_i(const int *datas, int dataLen);
double getSum_d(const double *datas, int dataLen);

// ��� [from, to)
double getSum_ft(const double *datas, int from, int to);

// ���Ȩ�� [from, to)
double getWtSum(const double *datas, int from, int to);

// ��ƽ��ֵ
double getMean_i(const int *datas, int dataLen);
double getMean_d(const double *datas, int dataLen);

// ��ƽ��ֵ [from, to)
double getMean_ft(const double *data, int from, int to);

// ���Ȩƽ��ֵ [from, to)
double getWtMean(const double *datas, int from, int to);

// ������Сֵ
double getMin(const double *datas, int dataLen, int *index);

double getMin_ft(const double *datas, int from, int to, int *index);

// �������ֵ
double getMax(const double *datas, int dataLen, int *index);

double getMax_ft(const double *datas, int from, int to, int *index);

// �󷽲�
double getSd_i(const int *datas, int dataLen, BOOL8 flag);
double getSd_d(const double *datas, int dataLen, BOOL8 flag);

// �����ϵ��
double getCv_i(const int *datas, int dataLen, BOOL8 flag);
double getCv_d(const double *datas, int dataLen, BOOL8 flag);

// ��������
double getMse(const double *odatas, const double *pdatas, int from, int to);

// ���ۼ�����
void cumsum(double* sumdatas, double* datas, int dataLen);

// ����
void sort(int *indexes, const double *datas, int dataLen);

//��˹ƽ��
void gsFilter(double *dout, const double *din, int dataLen, double ds);

// ��������������
BOOL8 evaData(const pulse_info_i_t *pulseInfo, uint32 pulseNum, int *lossSum);

// ���ݲ�ֵ�����ԣ�,outLens > inLens
void dataInterpolation(const double *dIn, const int inLens, double *dOut, const int outLens);

#endif // _ABCOMPUTE_H_

