#ifndef _NORM_FIT_H
#define _NORM_FIT_H

#include "pdatatype.h"

typedef struct tag_fit_range
{
	int left;
	int right;
}fit_range_t;

typedef struct tag_fit_para
{
	double lower;
	double upper;
	double delta;
}fit_para_t;

BOOL8 makeFitCurve(double *dataDest, const double *dataSrc, int dataLen, const fit_range_t *fitRange, const fit_para_t *paraMu, const fit_para_t *paraSd);


#endif // _NORM_FIT_H

