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

// ֱ��ͼ���
typedef struct HistInfor
{
	char    name[20];					// ����
	int		dataLen;					// ����
	double	datas[MAX_HIST_DATA_LEN];	// ����
	int		lineLen;					// �ֽ�������
	int		lines[MAX_HIST_LINE_LEN];	// �ֽ���λ��

	mode_info_t modeInfo;
}histInfor;

// ����
typedef struct HistInforList
{
	int			num;				 // dPara����
	histInfor   hists[MAX_HIST_NUM];// ��������
}histInforList;


// ֱ��ͼ��Ϣ
// ��ʼ��
void initHist(histInfor *hist);

// ����
void setHistName(histInfor *hist, const char *name);
const char *histName(const histInfor *hist);

// �������ơ����ݳ��ȡ��ֽ��߳���
void setHistInfo(histInfor *hist, const char *name, int data_len, int line_len);

// ����ֱ��ͼ
void cpyHistInfo(histInfor *histOut, const histInfor *histIn);

// ����ֱ��ͼ����ͳ��[lowTh upperTh]��Χ�ڵ����ݣ�
int makeHistData(histInfor *hist, const pulse_info_i_t *pulseInfo, int pulseNum, int step, int upperTh, int lowerTh, 
				 const int *stampFlags, const int size);

// �Ƕȱ任
void angleTransform(double *angleDatas, const double *orgDatas, int dataLen, double k, double b);

// ����ֱ��ͼģʽ��Ϣ��������������λ�á���硢�ҽ磬rΪ����Ѱ�뾶��
void makeHistModeInfo(histInfor *hist, int r);

// ����ֱ��ͼѹ����
double getHistRate(const histInfor *hist, double rloc);

// ֱ��ͼ�б��ʼ��
void initHistList(histInforList *histList);

// ��������
void getHistNames(char *names[], histInforList *histList);

// ��������ȡ��Ӧֱ��ͼ
histInfor * getHistInfo(histInforList *histList, const char *name);

// ׷��ֱ��ͼ
void appendHistInfo(histInforList *histList, const histInfor *hist);

#endif // _HIST_H_

