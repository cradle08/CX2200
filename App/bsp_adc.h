#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"


//ÿ��ͨ������ƽ��5��
#define MONITORV_SMOOTH_NUM         5


//// ������ص�ѹ����ADC3�����λ�ã��ⲿ�������ADC3��ʼ����أ�
//typedef enum {
//  MONITORV_PRESS       = 0,  //ģ����ѹ             //ADC3_IN0_PA0
//  MONITORV_MOTOR_I     = 1,  //�����������         //ADC3_IN1_PA1
//  MONITORV_HGB         = 2,  //HGB������ѹ          //ADC3_IN2_PA2
//  MONITORV_ELEC        = 3,  //�����缫������ѹ     //ADC3_IN4_PF6
//  MONITORV_LIGHT_CUR   = 4,  //��·����Դ��ѹ       //ADC3_IN5_PF7
//  MONITORV_NTC         = 5,  //NTC�¶ȴ�������ѹ    //ADC3_IN6_PF8
//  MONITORV_BC_CUR      = 6,  //ѪҺ������Դ��ѹ   //ADC3_IN7_PF9
//  MONITORV_XK          = 7,  // С�׵�ѹ            //ADC3_IN11_PC1
//  MONITORV_P12V        = 8,  //+12V                 //ADC3_IN12_PC2
//  MONITORV_N12V        = 9,  //-12V                 //ADC3_IN13_PC3
//  MONITORV_END         = 10,  //end
//  
//} MonitorV_e;



// ������ص�ѹ����ADC3�����λ�ã��ⲿ�������ADC3��ʼ����أ�
typedef enum {
  MONITORV_PRESS       = 0,  //ģ����ѹ             //ADC3_IN0_PA0
  //MONITORV_MOTOR_I,        //�����������         //ADC3_IN1_PA1
  MONITORV_HGB,              //HGB������ѹ          //ADC3_IN2_PA2
  MONITORV_ELEC,             //�����缫������ѹ     //ADC3_IN4_PF6
//  MONITORV_LIGHT_CUR,      //��·����Դ��ѹ       //ADC3_IN5_PF7
  MONITORV_NTC,              //NTC�¶ȴ�������ѹ    //ADC3_IN6_PF8
  MONITORV_BC_CUR,           //ѪҺ������Դ��ѹ   //ADC3_IN7_PF9
  MONITORV_XK,               // С�׵�ѹ            //ADC3_IN11_PC1
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


