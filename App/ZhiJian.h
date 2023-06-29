#ifndef _ZHIJIAN_H
#define _ZHIJIAN_H


#include "stm32f4xx_hal.h"





/***
*质检--HGB校准序号枚举
***/
typedef enum _ZJ_HGBJZ_e{
	ZJ_HGBJZ_1 = 0,
	ZJ_HGBJZ_2,
	ZJ_HGBJZ_3
}ZJ_HGBJZ_e;



/***
*质检--HGB线性序号枚举
***/
typedef enum _ZJ_HGBXX_e{
	ZJ_HGBXX_1 = 0,
	ZJ_HGBXX_2,
	ZJ_HGBXX_3,
	ZJ_HGBXX_4,
	ZJ_HGBXX_5
}ZJ_HGBXX_e;



/***
*质检--HGB准确度序号枚举
***/
typedef enum _ZJ_HGBZQD_e{
	ZJ_HGBZQD_1 = 0,
	ZJ_HGBZQD_2,
	ZJ_HGBZQD_3
}ZJ_HGBZQD_e;








void ZJ_WBCGZTest(void);
void ZJ_HGB_KB(void);
void ZJ_HGB_JMD(void);
void ZJ_HGB_JZ(ZJ_HGBJZ_e ZJHGBjzMode);
void ZJ_HGB_XX(ZJ_HGBXX_e ZJHGBxxMode);
void ZJ_HGB_ZQD(ZJ_HGBZQD_e ZJHGBzqdMode);


#endif
