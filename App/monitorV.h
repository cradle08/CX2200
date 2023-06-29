#ifndef __MONITORV_H__
#define __MONITORV_H__


#include "stm32f4xx.h"
#include "main.h"
#include "msg_def.h"


#define MCU_ADC12BIT_RESOLUTION     4095
#define MCU_ADC_REF_V_3300MV        3300
#define POWER_12V_MAX_DEVIATION     500     //12电源最大的偏差，500mv
#define BC_CURRENT_MAX_DEVIATION    3000    //BC恒流源最大的偏差，3000mv


// 获取小孔ADC值
uint16_t XK_ADC(void);

/*
*获取小孔电压值,单位mv
*/
uint16_t XK_V(void);
FlagStatus  XK_V_Is_LessThan(uint16_t ulV);
FlagStatus  XK_V_Is_MoreThan(uint16_t ulV);


/*
*获取恒流源电压,mv值
*/
uint32_t BC_Current_V(void);
uint32_t BC_Current_ADC_V(void);
uint32_t BC_Current_V(void);


//定量电机电压
uint16_t Elec_ADC(void);
uint16_t Elec_V(void);

//12V供电电压，负极
uint16_t Power_12VN_ADC(void);
int16_t Power_12VN_V(void);
ErrorCode_e Power_12VN_Is_Normal(void);

//12V供电电压，正极
uint16_t Power_12VP_ADC(void);
uint16_t Power_12VP_V(void);
ErrorCode_e Power_12VP_Is_Normal(void);

//步进电机的驱动电流，对应的电压值（I=0.766V）
//uint16_t Motor_Refer_V_ADC(void);
//uint16_t Motor_Refer_V(void);
//uint16_t Motor_Driver_I(void);
    

//光路恒流源电压
//uint16_t Light_Current_ADC(void);
//uint16_t Ligth_Current_V(void);
ErrorCode_e BC_Current_Is_Normal(void);

//模拟温度传感器(NTC)输出电压值，可转化为温度值（）
uint16_t NTC_ADC(void);
uint16_t NTC_V(void);
int16_t NTC_Temperature(void);

//模拟气压计输出电压值，可转化为气压值（）
uint16_t Press_ADC(void);
uint16_t Press_V(void);

// HGB采样值
uint16_t HGB_ADC(void);
uint16_t HGB_V(void);

#endif
