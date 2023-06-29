#include "main.h"
#include "bsp_outin.h"
#include "cmsis_os2.h"
#include "tp.h"
#include "xpt2046.h"
#include "msg_def.h"




/*
*   TP 初始化,
*/
void TP_Init(void)
{
    XPT2046_Init();
}


///*
//*   TP 扫描（电容屏）
//*/
//void TP_Scan(void)
//{


//}







/*
*   获取X坐标
*/
//int TP_Read_X(void)
//{


//}


/*
*   获取Y坐标
*/
//int TP_Read_Y(void)
//{


//}


