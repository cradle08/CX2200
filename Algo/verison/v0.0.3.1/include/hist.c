#include "hist.h"
#include "abcompute.h"
#include "pdatatype.h"

#include <math.h>
#include <string.h>

void initHist(histInfor *hist)
{
	strcpy(hist->name, "");
	hist->dataLen = 0;
	hist->lineLen = 0;
	memset(hist->datas, 0, MAX_HIST_DATA_LEN * sizeof(double));
	memset(hist->lines, 0, MAX_HIST_LINE_LEN * sizeof(int));
	memset(&hist->modeInfo, 0, sizeof(mode_info_t));
}

// 名称
void setHistName(histInfor *hist, const char *name)
{
    strcpy(hist->name, name);
}

const char *histName(const histInfor *hist)
{
	return hist->name;
}

// 设置名称、数据长度、分界线长度
void setHistInfo(histInfor *hist, const char *name, int data_len, int line_len)
{
	setHistName(hist, name);
	hist->dataLen = data_len;
	hist->lineLen = line_len;
}

// 生成直方图（仅统计[lowTh upperTh]范围内的数据）
void makeHistData0(histInfor *hist, const unsigned short *buffer, int len, int step, int upperTh, int lowerTh)
{
	int i;
	int index;
	unsigned short val;

	memset(hist->datas, 0, MAX_HIST_DATA_LEN * sizeof(double));
	if ( buffer )
	{
		if ( ( 0 < step ) && ( MAX_HIST_DATA_LEN >= hist->dataLen ) )
		{
			for ( i=0; i<len; i++ )
			{
				val = *(buffer+i);
				if ( ( val < lowerTh ) || ( val > upperTh ) )
				{
					continue;
				}

				index = val / step;
				if ( index < 0 )
				{
					hist->datas[0] += 1;
				}
				else if ( index >= hist->dataLen )
				{
					hist->datas[hist->dataLen-1] += 1;
				}
				else
				{
					hist->datas[index] += 1;
				}
			}
		}
	}
}

int makeHistData(histInfor *hist, const pulse_info_i_t *pulseInfo, int pulseNum, int step, int upperTh, int lowerTh, 
				 const int *stampFlags, const int size)
{
	const pulse_info_i_t *pi;
	int i;
	short val;
	int index;
	int stamp;
	int validStampNum = 0;

    memset(hist->datas, 0, MAX_HIST_DATA_LEN * sizeof(double));
    if ( pulseInfo )
    {
        pi = pulseInfo;

        if ( 0 == stampFlags )
        {
            if ( ( 0 < step ) && ( MAX_HIST_DATA_LEN >= hist->dataLen ) )
            {
                for ( i=0; i<pulseNum; i++ )
                {
                    val = pi->dcPeak;
                    if ( ( val >= lowerTh ) && ( val <= upperTh ) )
                    {
                        index = dmin(dmax(val / step, 0), hist->dataLen-1);
                        hist->datas[index] += 1;
                    }

                    pi++;
                }
            }
        }
        else
        {
            if ( ( 0 < step ) && ( MAX_HIST_DATA_LEN >= hist->dataLen ) )
            {
                for ( i=0; i<pulseNum; i++ )
                {
                    val = pi->dcPeak;
                    stamp = pi->timeStamp;
                    if ( ( val >= lowerTh ) && ( val <= upperTh ) 
                        && ( stamp >= 0 ) && ( stamp < size ) )
                    {
                        if ( 0 == stampFlags[stamp] )
                        {
                            index = dmin(dmax(val / step, 0), hist->dataLen-1);
                            hist->datas[index] += 1;
                        }
                    }

                    pi++;
                }
            }
        }
    }
   
    for ( i=0; i<size; i++ )
    {
        if ( 0 == stampFlags[i] )
        {
            validStampNum++;
        }
    }

    return validStampNum;
}

int makeHistData2(histInfor *hist, const pulse_info_i_t *pulseInfo, int pulseNum, int upperTh, int lowerTh, 
				 const int *stampFlags, const int size)
{
	const pulse_info_i_t *pi;
	double step;
	int i;
	short val;
	int index;
	int stamp;
	int validStampNum = 0;

	memset(hist->datas, 0, MAX_HIST_DATA_LEN * sizeof(double));

	if ( pulseInfo )
	{
		pi = pulseInfo;

		step = (double)(upperTh - lowerTh)/hist->dataLen;

		if ( 0 == stampFlags )
		{
			if ( ( 0 < step ) && ( MAX_HIST_DATA_LEN >= hist->dataLen ) )
			{
				for ( i=0; i<pulseNum; i++ )
				{
					val = pi->dcPeak;
					if ( ( val >= lowerTh ) && ( val <= upperTh ) )
					{
						index = (int)(dmin(dmax(val / step, 0), hist->dataLen-1));
						hist->datas[index] += 1;
					}

					pi++;
				}
			}
		}
		else
		{
			if ( ( 0 < step ) && ( MAX_HIST_DATA_LEN >= hist->dataLen ) )
			{
				for ( i=0; i<pulseNum; i++ )
				{
					val = pi->dcPeak;
					stamp = pi->timeStamp;
					if ( ( val >= lowerTh ) && ( val <= upperTh ) 
						&& ( stamp >= 0 ) && ( stamp < size ) )
					{
						if ( 0 == stampFlags[stamp] )
						{
							index = (int)(dmin(dmax(val / step, 0), hist->dataLen-1));
							hist->datas[index] += 1;
						}
					}

					pi++;
				}
			}
		}
	}

	for ( i=0; i<size; i++ )
	{
		if ( 0 == stampFlags[i] )
		{
			validStampNum++;
		}
	}

	return validStampNum;
}

// 角度变换
void angleTransform(double *angleDatas, const double *orgDatas, int dataLen, double k, double b)
{
	int i;
	int step;
	int left;
	int right;
	double alphaA;
	double alphaB;

	if ( (0 == angleDatas) || (0 == orgDatas) || (0 >= dataLen))
	{
		return;
	}

	memset(angleDatas, 0, dataLen * sizeof(double));

	for ( i=0; i<dataLen; i++)
	{
		step = (int)(k*i + b);
		if ( 0 >= step )
		{
			continue;
		}

		left  = dmax(i-step, 0);
		right = dmin(i+step, dataLen-1);

		alphaA = 0.0;
		if ( left != i)
		{
			alphaA = atan((orgDatas[left] - orgDatas[i])/(left - i));
		}

		alphaB = 0.0;
		if ( right != i)
		{
			alphaB = atan((orgDatas[right] - orgDatas[i])/(right - i));
		}

		angleDatas[i] = ( alphaA - alphaB + PI ) / PI; //0 .. 2
	}
}

// 生成直方图模式信息（包含峰数量、位置、左界、右界，r为峰搜寻半径）
void makeHistModeInfo(histInfor *hist, int r)
{
    int foundNum = 0;
    int histLen = hist->dataLen;
	int i,j,x;
	BOOL8 flagl,flagr;

    memset(&hist->modeInfo, 0, sizeof(mode_info_t));

    for ( i=1; i<histLen-1; i++ )
    {
        // 判断当前点是否大于左侧各点
        flagl = TRUE;
        for ( j=i-r; j<i; j++ )
        {
            if ( ( j < 0 ) || ( j > histLen ) )
            {
                flagl = FALSE;
                break;
            }

            if ( hist->datas[j] >= hist->datas[i] )
            {
                flagl = FALSE;
                break;
                
            }
        }

        if ( flagl )
        {
            // 判断当前点是否不小于右侧各点
            flagr = TRUE;
            for ( j=i+1; j<=i+r; j++ )
            {
                if ( ( j < 0 ) || ( j > histLen ) )
                {
                    flagr = FALSE;
                    break;
                }

                if ( hist->datas[j] > hist->datas[i] )
                {
                    flagr = FALSE;
                    break;
                }
            }
            
            if ( flagr )
            {
                // 保存峰值信息
                if ( hist->modeInfo.modeNum < MAX_HIST_MODE_NUM )
                {
                    hist->modeInfo.modes[foundNum] = i;
                    foundNum++;
                }
            }
        }
    }

    hist->modeInfo.modeNum = foundNum;

    // 确定峰的左右界
    if ( hist->modeInfo.modeNum > 0 )
    {
        hist->modeInfo.lefts[0] = 0;  // 第一个峰的左界

        // 与前峰间的谷点为左界, 与后峰间的谷点为右界
        for ( i=1; i<hist->modeInfo.modeNum; i++ )
        {
            x = hist->modeInfo.modes[i-1];
            for ( j=hist->modeInfo.modes[i-1]; j<hist->modeInfo.modes[i]; j++ )
            {
                if ( hist->datas[j] < hist->datas[x] )
                {
                    x = j;
                }
            }
            hist->modeInfo.lefts[i]    = x;
            hist->modeInfo.rights[i-1] = x;
        }

        hist->modeInfo.rights[hist->modeInfo.modeNum-1] = histLen;  // 最后一个峰的右界
    }

}

// 计算直方图压缩率
double getHistRate(const histInfor *hist, double rloc)
{
    int size = dmax(hist->dataLen-6, 0);
	double rate = 1.0;

    // 求直方图总面积
    double histArea = getSum_d((double *)hist->datas, size);

    // 寻直方图末端位值
    int lastDot = 0;
    double temp = 0;
    for ( lastDot = size; lastDot > 10; lastDot-- )
    {
        temp += *(hist->datas + lastDot);
        if ( temp >= rloc * histArea )
        {
            break;
        }
    }

    // 计算直方图压缩率
    if( 0 < size )
    {
        rate = 1.0 * lastDot / size;
    }

    return rate;
}

// 直方图列表初始化
void initHistList(histInforList *histList)
{
	int i;

	histList->num = 0;
	for (i = 0; i<MAX_HIST_NUM; i++)
	{
		initHist(&histList->hists[i]);
	}
}

// 参数名称
void getHistNames(char *names[], histInforList *histList)
{
	int i;

	for (i = 0; i < histList->num; i++)
	{
		histInfor hist = histList->hists[i];
		strcpy(names[i],hist.name);
	}
}

// 根据名称取对应直方图
histInfor * getHistInfo(histInforList *histList, const char *name)
{
	int i;
	histInfor *hist;

    for ( i = 0; i < histList->num; i++)
    {
        hist = &histList->hists[i];
		if ( 0 == strcmp(hist->name,name) )
		{
			return hist;
		}
    }

    return 0;
}

// 复制直方图
void cpyHistInfo(histInfor *histOut, const histInfor *histIn)
{
	strcpy(histOut->name,histIn->name);
	histOut->dataLen = histIn->dataLen;
	histOut->lineLen = histIn->lineLen;

	memcpy(histOut->datas, histIn->datas, MAX_HIST_DATA_LEN*sizeof(double));
	memcpy(histOut->lines, histIn->lines, MAX_HIST_LINE_LEN*sizeof(int));
	histOut->modeInfo = histIn->modeInfo;
}

// 追加直方图
void appendHistInfo(histInforList *histList, const histInfor *hist)
{
	int num = histList->num;

	if (num + 1 <= MAX_HIST_NUM)
	{
		cpyHistInfo(&histList->hists[num],hist);
		histList->num++;
	}
}

