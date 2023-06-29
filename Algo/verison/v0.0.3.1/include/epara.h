#ifndef _EPARA_H_
#define _EPARA_H_


#include "pdatatype.h"

#define MAX_EPARA_NUM 10

// ����
typedef struct EPara
{
	char	name[20];		// ��������
	uint8	dataType;		// ������������
	uint32	dataNum;		// �������ݸ���
	char	*dataArray;		// ��������
	uint8	flag;			// �������
	char	unit[10];		// ������λ
}ePara;

// ����
typedef struct EParaList
{
	int		num;				 // ePara����
	ePara   paras[MAX_EPARA_NUM];// ��������
}eParaList;

// ��ʼ��
void initEP(ePara *para);

// ����
void clearEP(ePara *para);

// ���
void outputEP(ePara *para, BOOL8 usePrecision);

// ��������
const char *nameEP(ePara *para);
void setNameEP(ePara *para, const char* name);

// ��������
void setDataEP(ePara *para, uint8 dataType, uint32 dataNum, const char *dataArray);
uint8 dataTypeEP(ePara *para);
uint32 dataNumEP(ePara *para);
char * dataArrayEP(ePara *para);

// �������
void setFlagEP(ePara *para, uint8 flag);
uint8 flagEP(ePara *para);

// ������λ
void setUnitEP(ePara *para, const char *unit);
const char *unitEP(ePara *para);

// ���ò���
void setParaEP(ePara *para, const char *name, uint8 dataType, uint32 dataNum, const char *dataArray, uint8 flag, const char *unit);

// ��������
void cpyParaEP(ePara *paraOut, const ePara *paraIn);

//EParaList
// ��ʼ��
void initParaListEP(eParaList *paraList);

// ����EParaList
void clearListEP(eParaList *paraList);

// ���
void outputListEP(eParaList *paraList, BOOL8 usePrecision);

// ��������
void setParaNamesEP(eParaList *paraList, char *names[]);
void getParaNamesEP(char *names[], eParaList *paraList);

// ȡ����
ePara * getParaPtrEP(eParaList *paraList, char *name);

// ׷�Ӳ���
void appendDetailParaEP(eParaList *paraList, const char *name, uint8 dataType, uint32 dataNum, 
						const char *dataArray, uint8 flag, const char *unit);
void appendParaEP(eParaList *paraList, const ePara *para);

// �ж��Ƿ��������
BOOL8 containsEP(eParaList *paraList, const char *name);

#endif // _EPARA_H_

