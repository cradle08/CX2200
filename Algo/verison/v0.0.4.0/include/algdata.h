/*
* Copyright (c) ���ڴ���ҽ�ƿƼ����޹�˾
* All right reserved.
*
* �ļ����ƣ�algdata.h
*
*/

#ifndef _ALGDATA_H_
#define _ALGDATA_H_


#include "pdatatype.h"
#include "npara.h"
#include "dpara.h"
#include "epara.h"

// ģʽ������ģʽ��Ѫ��ģʽ������ģʽ������ģʽ�ȣ�
void setModePara(nParaList *paraList, const char *name, uint8 mode);
int modePara(nParaList *paraList, const char *name);

// ϵͳ������У׼ϵ����ϡ�ͱȡ��������������ʱ��ȣ�
void setSystemPara(dParaList *paraList, const char *name, double value, int precision);
double systemPara(dParaList *paraList, const char *name, double value);

// ����������ʶ������
void setSmpData(eParaList *paraList, const char *name, const uint8 *dataBuffer, uint32 dataLen);
void appendSmpData(eParaList *paraList, const ePara *para);

// ���ò���
void setConfigurePara(dParaList *paraList, const char *name, double value);
double configurePara(dParaList *paraList, const char *name, double value);

// �汾��
const char *version(void);

const char *versionDate(void);

#endif // _ALGDATA_H_
