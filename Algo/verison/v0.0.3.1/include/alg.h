/*
* Copyright (c) 深圳创怀医疗科技有限公司
* All right reserved.
*
* 文件名称：alg.h
*
*/

#ifndef _ALG_H_
#define _ALG_H_

#include "npara.h"
#include "dpara.h"
#include "hist.h"
#include "algdata.h"
#include "smpdatadefs.h"

// 算法输入数据
typedef struct tag_alg_data_in
{
	// 模式参数列表
	nParaList *modeParas;

	// 系统参数列表
	dParaList *systemParas;

	// 脉冲信号
	pulse_info_i_t *pulseInfo;
	uint32 pulseNum;
}alg_data_in;

// 算法输出数据
typedef struct tag_alg_data_out
{
	// 报告参数列表
	dParaList *reportParas;

	// 特征参数列表（中间信息、中间状态）
	dParaList *featureParas;

	// 报警参数列表
	dParaList *flagParas;

	// 直方图信息
	histInforList *histList;//WBC/RBC/PLT
}alg_data_out;

#ifdef __cplusplus
extern "C"
{
#endif

//算法入口
//
int calculate(alg_data_out *dataOut, const alg_data_in *dataIn);


#ifdef __cplusplus
}
#endif


#endif // _ALG_H_

