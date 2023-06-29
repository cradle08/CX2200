/*
* Copyright (c) 深圳创怀医疗科技有限公司
* All right reserved.
*
* 文件名称：algdata.h
*
*/

#ifndef _ALGDATA_H_
#define _ALGDATA_H_


#include "pdatatype.h"
#include "npara.h"
#include "dpara.h"
#include "epara.h"

// 模式（分析模式、血样模式、进样模式、物种模式等）
void setModePara(nParaList *paraList, const char *name, uint8 mode);
int modePara(nParaList *paraList, const char *name);

// 系统参数（校准系数、稀释比、计量体积、采样时间等）
void setSystemPara(dParaList *paraList, const char *name, double value, int precision);
double systemPara(dParaList *paraList, const char *name, double value);

// 样本采样及识别数据
void setSmpData(eParaList *paraList, const char *name, const uint8 *dataBuffer, uint32 dataLen);
void appendSmpData(eParaList *paraList, const ePara *para);

// 配置参数
void setConfigurePara(dParaList *paraList, const char *name, double value);
double configurePara(dParaList *paraList, const char *name, double value);

// 版本号
const char *version(void);

const char *versionDate(void);

#endif // _ALGDATA_H_
