/*
* Copyright (c) 深圳创怀医疗科技有限公司
* All right reserved.
*
* 文件名称：algpulse.h
*
*/

#ifndef _ALG_PULSE_H_
#define _ALG_PULSE_H_

#include "pdatatype.h"
#include "smpdatadefs.h"
#include "channaldefs.h"
#include "modedefs.h"

#define PRE_PULSE_NUM  250000 //最大脉冲数
#define DATA_UINT_SIZE 1024  //每包数据大小为1024字节

// 状态变量取值
enum 
{ 
	CS_IDLE = 0,
	CS_START,
	CS_END
};

// 脉冲识别特征信息
typedef struct tag_pulse_feature_info
{
	uint16          peaky;                      // 脉冲峰值
	uint32          peakx;                      // 脉冲峰值位置
	uint16          starty;                     // 起始点高度
	uint32          startx;                     // 起始点位置
	uint16          endy;                       // 结束点高度
	uint32          endx;                       // 结束点位置
	uint16          fpeaky;                     // 前峰高度
	uint32          fpeakx;                     // 前峰位置
	uint16          bpeaky;                     // 后峰高度
	uint32          bpeakx;                     // 后峰位置
	uint16          valey;                      // 谷高度
	uint32          valex;                      // 谷位置
	uint32          width;                      // 脉冲宽度
	uint32          topWidth;                   // 边续高值点区域宽度
	uint32          peakNum;                    // 峰数目
	uint8           valeFlag;                   // 谷标志
}pulse_feature_info_t;

// 脉冲识别中间状态信息
typedef struct tag_pulse_mid_feature
{
	int16					nCS;				// 信号状态, IDLE --> START --> END --> IDLE
	pulse_feature_info_t	featureInfo;		// 脉冲识别特征信息
	uint16					firData;			// pulseData的第一个数据
	uint16					secData;			// pulseData的第二个数据
	uint16					thiData;			// pulseData的第三个数据
	double64				smpRate;			// 数据的采样频率
	uint32					iterNum;			//主函数执行次数
}pulse_mid_feature;

// 初始化脉冲信息
void pulseInit(pulse_info_i_t *pulseInfo);

// WBC通道脉冲识别（阻抗）
BOOL8 wbcPulseIdentify(pulse_info_i_t *wbcPulseInfo, uint32 *pulseNum, const uint8 *pulseData, long dataLen, pulse_mid_feature *midFeature);

// RBC通道脉冲识别（阻抗）
BOOL8 rbcPulseIdentify(pulse_info_i_t *rbcPulseInfo, uint32 *pulseNum, const uint8 *pulseData, long dataLen, pulse_mid_feature *midFeature);

// PLT通道脉冲识别（阻抗）
BOOL8 pltPulseIdentify(pulse_info_i_t *pltPulseInfo, uint32 *pulseNum, const uint8 *pulseData, long dataLen, pulse_mid_feature *midFeature);

#endif // _ALGPULSE_H_

