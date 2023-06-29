#ifndef _ABCOMPUTE_H_
#define _ABCOMPUTE_H_


#include "pdatatype.h"
#include "smpdatadefs.h"

// 求和(int/double)
double getSum_i(const int *datas, int dataLen);
double getSum_d(const double *datas, int dataLen);

// 求和 [from, to)
double getSum_ft(const double *datas, int from, int to);

// 求加权和 [from, to)
double getWtSum(const double *datas, int from, int to);

// 求平均值
double getMean_i(const int *datas, int dataLen);
double getMean_d(const double *datas, int dataLen);

// 求平均值 [from, to)
double getMean_ft(const double *data, int from, int to);

// 求加权平均值 [from, to)
double getWtMean(const double *datas, int from, int to);

// 查找最小值
double getMin(const double *datas, int dataLen, int *index);

double getMin_ft(const double *datas, int from, int to, int *index);

// 查找最大值
double getMax(const double *datas, int dataLen, int *index);

double getMax_ft(const double *datas, int from, int to, int *index);

// 求方差
double getSd_i(const int *datas, int dataLen, BOOL8 flag);
double getSd_d(const double *datas, int dataLen, BOOL8 flag);

// 求变异系数
double getCv_i(const int *datas, int dataLen, BOOL8 flag);
double getCv_d(const double *datas, int dataLen, BOOL8 flag);

// 求均方误差
double getMse(const double *odatas, const double *pdatas, int from, int to);

// 求累加数组
void cumsum(double* sumdatas, double* datas, int dataLen);

// 排序
void sort(int *indexes, const double *datas, int dataLen);

//高斯平滑
void gsFilter(double *dout, const double *din, int dataLen, double ds);

// 数据完整性评价
BOOL8 evaData(const pulse_info_i_t *pulseInfo, uint32 pulseNum, int *lossSum);

// 数据插值（线性）,outLens > inLens
void dataInterpolation(const double *dIn, const int inLens, double *dOut, const int outLens);

#endif // _ABCOMPUTE_H_

