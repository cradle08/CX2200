#ifndef _EPARA_H_
#define _EPARA_H_


#include "pdatatype.h"

#define MAX_EPARA_NUM 10

// 定义
typedef struct EPara
{
	char	name[20];		// 参数名称
	uint8	dataType;		// 参数数据类型
	uint32	dataNum;		// 参数数据个数
	char	*dataArray;		// 参数数据
	uint8	flag;			// 参数标记
	char	unit[10];		// 参数单位
}ePara;

// 定义
typedef struct EParaList
{
	int		num;				 // ePara数量
	ePara   paras[MAX_EPARA_NUM];// 参数数组
}eParaList;

// 初始化
void initEP(ePara *para);

// 清理
void clearEP(ePara *para);

// 输出
void outputEP(ePara *para, BOOL8 usePrecision);

// 参数名称
const char *nameEP(ePara *para);
void setNameEP(ePara *para, const char* name);

// 参数数据
void setDataEP(ePara *para, uint8 dataType, uint32 dataNum, const char *dataArray);
uint8 dataTypeEP(ePara *para);
uint32 dataNumEP(ePara *para);
char * dataArrayEP(ePara *para);

// 参数标记
void setFlagEP(ePara *para, uint8 flag);
uint8 flagEP(ePara *para);

// 参数单位
void setUnitEP(ePara *para, const char *unit);
const char *unitEP(ePara *para);

// 设置参数
void setParaEP(ePara *para, const char *name, uint8 dataType, uint32 dataNum, const char *dataArray, uint8 flag, const char *unit);

// 参数复制
void cpyParaEP(ePara *paraOut, const ePara *paraIn);

//EParaList
// 初始化
void initParaListEP(eParaList *paraList);

// 清理EParaList
void clearListEP(eParaList *paraList);

// 输出
void outputListEP(eParaList *paraList, BOOL8 usePrecision);

// 参数名称
void setParaNamesEP(eParaList *paraList, char *names[]);
void getParaNamesEP(char *names[], eParaList *paraList);

// 取参数
ePara * getParaPtrEP(eParaList *paraList, char *name);

// 追加参数
void appendDetailParaEP(eParaList *paraList, const char *name, uint8 dataType, uint32 dataNum, 
						const char *dataArray, uint8 flag, const char *unit);
void appendParaEP(eParaList *paraList, const ePara *para);

// 判断是否包含参数
BOOL8 containsEP(eParaList *paraList, const char *name);

#endif // _EPARA_H_

