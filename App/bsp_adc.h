#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"


//每个通道采样平滑5次
#define MONITORV_SMOOTH_NUM         5


//// 各个监控电压，再ADC3缓存的位置，这部分排序和ADC3初始化相关，
//typedef enum {
//  MONITORV_PRESS       = 0,  //模拟气压             //ADC3_IN0_PA0
//  MONITORV_MOTOR_I     = 1,  //电机驱动电流         //ADC3_IN1_PA1
//  MONITORV_HGB         = 2,  //HGB采样电压          //ADC3_IN2_PA2
//  MONITORV_ELEC        = 3,  //定量电极触发电压     //ADC3_IN4_PF6
//  MONITORV_LIGHT_CUR   = 4,  //光路恒流源电压       //ADC3_IN5_PF7
//  MONITORV_NTC         = 5,  //NTC温度传感器电压    //ADC3_IN6_PF8
//  MONITORV_BC_CUR      = 6,  //血液检测恒流源电压   //ADC3_IN7_PF9
//  MONITORV_XK          = 7,  // 小孔电压            //ADC3_IN11_PC1
//  MONITORV_P12V        = 8,  //+12V                 //ADC3_IN12_PC2
//  MONITORV_N12V        = 9,  //-12V                 //ADC3_IN13_PC3
//  MONITORV_END         = 10,  //end
//  
//} MonitorV_e;



// 各个监控电压，再ADC3缓存的位置，这部分排序和ADC3初始化相关，
typedef enum {
  MONITORV_PRESS       = 0,  //模拟气压             //ADC3_IN0_PA0
  //MONITORV_MOTOR_I,        //电机驱动电流         //ADC3_IN1_PA1
  MONITORV_HGB,              //HGB采样电压          //ADC3_IN2_PA2
  MONITORV_ELEC,             //定量电极触发电压     //ADC3_IN4_PF6
//  MONITORV_LIGHT_CUR,      //光路恒流源电压       //ADC3_IN5_PF7
  MONITORV_NTC,              //NTC温度传感器电压    //ADC3_IN6_PF8
  MONITORV_BC_CUR,           //血液检测恒流源电压   //ADC3_IN7_PF9
  MONITORV_XK,               // 小孔电压            //ADC3_IN11_PC1
  MONITORV_P12V,             //+12V                 //ADC3_IN12_PC2
  MONITORV_N12V,             //-12V                 //ADC3_IN13_PC3
  MONITORV_END,              //end
  
} MonitorV_e;



void ADC3_Start(void);
uint16_t MonitorV_ADC(MonitorV_e eV);





  
  
  
  
  
  
  
  
  
#ifdef __cplusplus
}
#endif

#endif //__BSP_PRESS_H_


