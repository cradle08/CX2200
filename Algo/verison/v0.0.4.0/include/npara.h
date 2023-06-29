#ifndef _NPARA_H_
#define _NPARA_H_

#include "pdatatype.h"

#define MAX_NPARA_NUM 20

// 定义
typedef struct NPara
{
	char	name[20];			// 参数名称
	int		value;				// 参数值
	int     flag;				// 参数标记
	char	unit[10];			// 参数单位
}nPara;

// 定义
typedef struct NParaList
{
	int		num;				 // nPara数量
	nPara   paras[MAX_NPARA_NUM];// 参数数组
}nParaList;

// 初始化
void initNP(nPara *para);

// 输出
void outputNP(nPara *para);

// 参数名称
const char *nameNP(nPara *para);
void setNameNP(nPara *para, const char* name);

// 参数值
int valueNP(nPara *para);
void setValueNP(nPara *para, int value);

// 参数标记
int flagNP(nPara *para);
void setFlagNP(nPara *para, int flag);

// 单位
const char *unitNP(nPara *para);
void setUnitNP(nPara *para, const char *unit);

// 设置参数
void setParaNP(nPara *para, const char *name, int value, int flag, const char *unit);

// 参数复制
void cpyParaNP(nPara *paraOut, nPara *paraIn);

// 参数比较
BOOL8 cmpParaNP(nPara *paraOut, nPara *paraIn);

// nPara数组实现
// 初始化
void initParaListNP(nParaList *paraList);

// 输出
void outputListNP(nParaList *paraList);

// paraList所有参数名称
void setParaNamesNP(nParaList *paraList, char *names[]);

void getParaNamesNP(char *names[], nParaList *paraList);

// 取参数
nPara getParaNP(nParaList *paraList, const char *name);

nPara * getParaPtrNP(nParaList *paraList, const char *name);

// 追加参数
BOOL8 appendDetailParaNP(nParaList *paraList, const char *name, int value, int flag, const char *unit);

// 追加参数
BOOL8 appendParaNP(nParaList *paraList, nPara *para);


// 判断是否包含参数
BOOL8 containsNP(nParaList *paraList, const char *name);

// 返回para结果
int valueListNP(nParaList *paraList, const char *name);

#endif // _NPARA_H_  
