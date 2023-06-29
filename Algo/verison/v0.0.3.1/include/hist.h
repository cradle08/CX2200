#ifndef _HIST_H_
#define _HIST_H_


#include "smpdatadefs.h"

#define MAX_HIST_LINE_LEN   8
#define MAX_HIST_DATA_LEN   256
#define MAX_HIST_MODE_NUM   100
#define MAX_HIST_NUM		3

typedef struct tag_mode_info
{
	int       modeNum;
	int       modes[MAX_HIST_MODE_NUM];
	int       lefts[MAX_HIST_MODE_NUM];
	int       rights[MAX_HIST_MODE_NUM];
}mode_info_t;

// 直方图结果
typedef struct HistInfor
{
	char    name[20];					// 名称
	int		dataLen;					// 长度
	double	datas[MAX_HIST_DATA_LEN];	// 数据
	int		lineLen;					// 分界线数量
	int		lines[MAX_HIST_LINE_LEN];	// 分界线位置

	mode_info_t modeInfo;
}histInfor;

// 定义
typedef struct HistInforList
{
	int			num;				 // dPara数量
	histInfor   hists[MAX_HIST_NUM];// 参数数组
}histInforList;


// 直方图信息
// 初始化
void initHist(histInfor *hist);

// 名称
void setHistName(histInfor *hist, const char *name);
const char *histName(const histInfor *hist);

// 设置名称、数据长度、分界线长度
void setHistInfo(histInfor *hist, const char *name, int data_len, int line_len);

// 复制直方图
void cpyHistInfo(histInfor *histOut, const histInfor *histIn);

// 生成直方图（仅统计[lowTh upperTh]范围内的数据）
int makeHistData(histInfor *hist, const pulse_info_i_t *pulseInfo, int pulseNum, int step, int upperTh, int lowerTh, 
				 const int *stampFlags, const int size);

// 角度变换
void angleTransform(double *angleDatas, const double *orgDatas, int dataLen, double k, double b);

// 生成直方图模式信息（包含峰数量、位置、左界、右界，r为峰搜寻半径）
void makeHistModeInfo(histInfor *hist, int r);

// 计算直方图压缩率
double getHistRate(const histInfor *hist, double rloc);

// 直方图列表初始化
void initHistList(histInforList *histList);

// 参数名称
void getHistNames(char *names[], histInforList *histList);

// 根据名称取对应直方图
histInfor * getHistInfo(histInforList *histList, const char *name);

// 追加直方图
void appendHistInfo(histInforList *histList, const histInfor *hist);

#endif // _HIST_H_

