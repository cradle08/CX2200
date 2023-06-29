#ifndef _SMP_DATA_DEFS_H_
#define _SMP_DATA_DEFS_H_


#include "pdatatype.h"

// 光学通道脉冲信息
typedef struct tag_pulse_info_o
{
	short lsPeak;       // LS峰值（原始）
	short msPeak;       // MS峰值（原始）
	short hsPeak;       // HS峰值（原始）
	short lsPeakL;      // LS峰值（对数）
	short msPeakL;      // MS峰值（对数）
	short hsPeakL;      // HS峰值（对数）
	short fullWidth;    // 全峰宽
	short timeStamp;    // 时间戳
	short cellType;     // 细胞类型
	long  peakPos;      // 峰值位置
}pulse_info_o_t;

// 阻抗通道粒子信息
typedef struct tag_pulse_info_i
{
	short dcPeak;       // DC峰值
	short rfPeak;       // RF峰值
	short priWidth;     // 前峰宽
	short subWidth;     // 后峰宽
	short fullWidth;    // 全峰宽
	short baseLine;     // 脉冲基线
	short timeStamp;    // 时间戳
	short pulseType;    // 脉冲类型（0--正常；..）
	long  peakPos;      // 峰值位置
}pulse_info_i_t;

// 阻抗通道脉冲识别参数
typedef struct tag_pulse_iden_para_i
{
	uint16          pulseHeightTh;              // 脉冲高度识别阈值
	uint16          minPeakHeight;              // 最小脉冲峰值
	uint16          maxPeakHeight;              // 最大脉冲峰值
	uint16          minPulseWidth;              // 最小脉冲宽度
	uint16          maxPulseWidth;              // 最大脉冲宽度
	uint16          minRiseRate1;               // 最小首次上升速率
	uint16          minRiseRate2;               // 最小第二次上升速率
	uint16          minMeanRiseRate;            // 最小平均上升速率
	uint16          minMeanFallRate;            // 最小平均下降速率
	uint16          topHeightTh;                // 高值点高度阈值
	uint16          maxTopWidth;                // 最大连续高值点区域宽度
	uint16          startHPerTh;                // 起始点高度百分比阈值
	uint16          normalEndHPerTh;            // 一般方式结束处高度百分比阈值
	uint16          valeEndHPerTh;              // 谷点方式结束处高度百分比阈值
	uint16          valeEndHPeakTh;             // 谷点方式结束时峰高阈值
}pulse_iden_para_i_t;

#endif // _SMP_DATA_DEFS_H_

