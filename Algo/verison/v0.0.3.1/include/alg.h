/*
* Copyright (c) ���ڴ���ҽ�ƿƼ����޹�˾
* All right reserved.
*
* �ļ����ƣ�alg.h
*
*/

#ifndef _ALG_H_
#define _ALG_H_

#include "npara.h"
#include "dpara.h"
#include "hist.h"
#include "algdata.h"
#include "smpdatadefs.h"

// �㷨��������
typedef struct tag_alg_data_in
{
	// ģʽ�����б�
	nParaList *modeParas;

	// ϵͳ�����б�
	dParaList *systemParas;

	// �����ź�
	pulse_info_i_t *pulseInfo;
	uint32 pulseNum;
}alg_data_in;

// �㷨�������
typedef struct tag_alg_data_out
{
	// ��������б�
	dParaList *reportParas;

	// ���������б��м���Ϣ���м�״̬��
	dParaList *featureParas;

	// ���������б�
	dParaList *flagParas;

	// ֱ��ͼ��Ϣ
	histInforList *histList;//WBC/RBC/PLT
}alg_data_out;

#ifdef __cplusplus
extern "C"
{
#endif

//�㷨���
//
int calculate(alg_data_out *dataOut, const alg_data_in *dataIn);


#ifdef __cplusplus
}
#endif


#endif // _ALG_H_

