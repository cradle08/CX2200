#ifndef __MONITORV_H__
#define __MONITORV_H__


#include "stm32f4xx.h"
#include "main.h"
#include "msg_def.h"


#define MCU_ADC12BIT_RESOLUTION     4095
#define MCU_ADC_REF_V_3300MV        3300
#define POWER_12V_MAX_DEVIATION     500     //12��Դ����ƫ�500mv
#define BC_CURRENT_MAX_DEVIATION    3000    //BC����Դ����ƫ�3000mv


// ��ȡС��ADCֵ
uint16_t XK_ADC(void);

/*
*��ȡС�׵�ѹֵ,��λmv
*/
uint16_t XK_V(void);
FlagStatus  XK_V_Is_LessThan(uint16_t ulV);
FlagStatus  XK_V_Is_MoreThan(uint16_t ulV);


/*
*��ȡ����Դ��ѹ,mvֵ
*/
uint32_t BC_Current_V(void);
uint32_t BC_Current_ADC_V(void);
uint32_t BC_Current_V(void);


//���������ѹ
uint16_t Elec_ADC(void);
uint16_t Elec_V(void);

//12V�����ѹ������
uint16_t Power_12VN_ADC(void);
int16_t Power_12VN_V(void);
ErrorCode_e Power_12VN_Is_Normal(void);

//12V�����ѹ������
uint16_t Power_12VP_ADC(void);
uint16_t Power_12VP_V(void);
ErrorCode_e Power_12VP_Is_Normal(void);

//���������������������Ӧ�ĵ�ѹֵ��I=0.766V��
//uint16_t Motor_Refer_V_ADC(void);
//uint16_t Motor_Refer_V(void);
//uint16_t Motor_Driver_I(void);
    

//��·����Դ��ѹ
//uint16_t Light_Current_ADC(void);
//uint16_t Ligth_Current_V(void);
ErrorCode_e BC_Current_Is_Normal(void);

//ģ���¶ȴ�����(NTC)�����ѹֵ����ת��Ϊ�¶�ֵ����
uint16_t NTC_ADC(void);
uint16_t NTC_V(void);
int16_t NTC_Temperature(void);

//ģ����ѹ�������ѹֵ����ת��Ϊ��ѹֵ����
uint16_t Press_ADC(void);
uint16_t Press_V(void);

// HGB����ֵ
uint16_t HGB_ADC(void);
uint16_t HGB_V(void);

#endif
