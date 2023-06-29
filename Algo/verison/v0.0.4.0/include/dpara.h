#ifndef _DPARA_H_
#define _DPARA_H_


#include <stdio.h>
#include "pdatatype.h"

#define MAX_DPARA_NUM 40

// ����
typedef struct DPara
{
	char	name[30];			// ��������
	double	value;				// ����ֵ
	int     flag;				// �������
	char	unit[10];			// ������λ
	int		precision;			// ����
}dPara;

// ����
typedef struct DParaList
{
	int		num;				 // dPara����
	dPara   paras[MAX_DPARA_NUM];// ��������
}dParaList;

// ��ʼ��
void initDP(dPara *para);

// ���
void outputDP(dPara *para, BOOL8 usePrecision);

// ��������
const char *nameDP(dPara *para);
void setNameDP(dPara *para, const char* name);

// ����ֵ
double valueDP(dPara *para);
void setValueDP(dPara *para, double value);

// �������
int flagDP(dPara *para);
void setFlagDP(dPara *para, int flag);

// ��λ
const char *unitDP(dPara *para);
void setUnitDP(dPara *para, const char *unit);

// ��������
void setPrecisionDP(dPara *para, int precision);
int precisionDP(dPara *para);

// ���ò���
void setParaDP(dPara *para, const char *name, double value, int flag, const char *unit, int precision);

// ��������
void cpyParaDP(dPara *paraOut, dPara *paraIn);

// �����Ƚ�
BOOL8 cmpParaDP(dPara *paraOut, dPara *paraIn);

// dPara����ʵ��
// ��ʼ��
void initParaListDP(dParaList *paraList);

// ���
void outputListDP(dParaList *paraList, BOOL8 usePrecision);

// paraList���в�������
void setParaNamesDP(dParaList *paraList, char *names[]);

void getParaNamesDP(char *names[], dParaList *paraList);

// ȡ����
dPara getParaDP(dParaList *paraList, const char *name);
dPara * getParaPtrDP(dParaList *paraList, const char *name);

// ׷�Ӳ���
BOOL8 appendDetailParaDP(dParaList *paraList, const char *name, double value, int flag, const char *unit, int precision);

// ׷�Ӳ���
BOOL8 appendParaDP(dParaList *paraList, dPara *para);

// �ж��Ƿ��������
BOOL8 containsDP(dParaList *paraList, const char *name);

// ����para���
double valueListDP(dParaList *paraList, const char *name);

// ����para��Flag
int flagListDP(dParaList *paraList, const char *name);

#endif // _DPARA_H_  
