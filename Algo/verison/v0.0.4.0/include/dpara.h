#ifndef _DPARA_H_
#define _DPARA_H_


#include <stdio.h>
#include "pdatatype.h"

#define MAX_DPARA_NUM 40

// 定义
typedef struct DPara
{
	char	name[30];			// 参数名称
	double	value;				// 参数值
	int     flag;				// 参数标记
	char	unit[10];			// 参数单位
	int		precision;			// 精度
}dPara;

// 定义
typedef struct DParaList
{
	int		num;				 // dPara数量
	dPara   paras[MAX_DPARA_NUM];// 参数数组
}dParaList;

// 初始化
void initDP(dPara *para);

// 输出
void outputDP(dPara *para, BOOL8 usePrecision);

// 参数名称
const char *nameDP(dPara *para);
void setNameDP(dPara *para, const char* name);

// 参数值
double valueDP(dPara *para);
void setValueDP(dPara *para, double value);

// 参数标记
int flagDP(dPara *para);
void setFlagDP(dPara *para, int flag);

// 单位
const char *unitDP(dPara *para);
void setUnitDP(dPara *para, const char *unit);

// 参数精度
void setPrecisionDP(dPara *para, int precision);
int precisionDP(dPara *para);

// 设置参数
void setParaDP(dPara *para, const char *name, double value, int flag, const char *unit, int precision);

// 参数复制
void cpyParaDP(dPara *paraOut, dPara *paraIn);

// 参数比较
BOOL8 cmpParaDP(dPara *paraOut, dPara *paraIn);

// dPara数组实现
// 初始化
void initParaListDP(dParaList *paraList);

// 输出
void outputListDP(dParaList *paraList, BOOL8 usePrecision);

// paraList所有参数名称
void setParaNamesDP(dParaList *paraList, char *names[]);

void getParaNamesDP(char *names[], dParaList *paraList);

// 取参数
dPara getParaDP(dParaList *paraList, const char *name);
dPara * getParaPtrDP(dParaList *paraList, const char *name);

// 追加参数
BOOL8 appendDetailParaDP(dParaList *paraList, const char *name, double value, int flag, const char *unit, int precision);

// 追加参数
BOOL8 appendParaDP(dParaList *paraList, dPara *para);

// 判断是否包含参数
BOOL8 containsDP(dParaList *paraList, const char *name);

// 返回para结果
double valueListDP(dParaList *paraList, const char *name);

// 返回para的Flag
int flagListDP(dParaList *paraList, const char *name);

#endif // _DPARA_H_  
