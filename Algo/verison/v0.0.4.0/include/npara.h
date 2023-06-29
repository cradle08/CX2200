#ifndef _NPARA_H_
#define _NPARA_H_

#include "pdatatype.h"

#define MAX_NPARA_NUM 20

// ����
typedef struct NPara
{
	char	name[20];			// ��������
	int		value;				// ����ֵ
	int     flag;				// �������
	char	unit[10];			// ������λ
}nPara;

// ����
typedef struct NParaList
{
	int		num;				 // nPara����
	nPara   paras[MAX_NPARA_NUM];// ��������
}nParaList;

// ��ʼ��
void initNP(nPara *para);

// ���
void outputNP(nPara *para);

// ��������
const char *nameNP(nPara *para);
void setNameNP(nPara *para, const char* name);

// ����ֵ
int valueNP(nPara *para);
void setValueNP(nPara *para, int value);

// �������
int flagNP(nPara *para);
void setFlagNP(nPara *para, int flag);

// ��λ
const char *unitNP(nPara *para);
void setUnitNP(nPara *para, const char *unit);

// ���ò���
void setParaNP(nPara *para, const char *name, int value, int flag, const char *unit);

// ��������
void cpyParaNP(nPara *paraOut, nPara *paraIn);

// �����Ƚ�
BOOL8 cmpParaNP(nPara *paraOut, nPara *paraIn);

// nPara����ʵ��
// ��ʼ��
void initParaListNP(nParaList *paraList);

// ���
void outputListNP(nParaList *paraList);

// paraList���в�������
void setParaNamesNP(nParaList *paraList, char *names[]);

void getParaNamesNP(char *names[], nParaList *paraList);

// ȡ����
nPara getParaNP(nParaList *paraList, const char *name);

nPara * getParaPtrNP(nParaList *paraList, const char *name);

// ׷�Ӳ���
BOOL8 appendDetailParaNP(nParaList *paraList, const char *name, int value, int flag, const char *unit);

// ׷�Ӳ���
BOOL8 appendParaNP(nParaList *paraList, nPara *para);


// �ж��Ƿ��������
BOOL8 containsNP(nParaList *paraList, const char *name);

// ����para���
int valueListNP(nParaList *paraList, const char *name);

#endif // _NPARA_H_  
