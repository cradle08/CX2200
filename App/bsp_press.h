#ifndef __BSP_PRESS_H_
#define __BSP_PRESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"
#include "msg_def.h"

#define USE_ANALOG_PRESS_SENSOR_OR_NOT   1          //1:ʹ��ģ��ѹ����������0��ʹ������ѹ������     

#define BC_TEST_PRESS_VALUE         -30000    		//-����ʱʹ�õ�ѹ��ֵ
#define MAX_BC_TEST_PRESS_VALUE     -33000    		//������ѹ��ֵ
#define MIN_BC_TEST_PRESS_VALUE     -30000    		//��С����ѹ��ֵ#define BC_TEST_MIN_PRESS           -25000    //©���ж���Сѹ��
#define AIR_LEAK_PRESS              -25000    		//©���ж���Сѹ��
#define POWER_OFF_PRESS_VALUE       -5000           //�ػ�ʱ�����ѹ�����ڸ�ֵ������
		
#define BUILD_PRESS_MAX_TIME        10000     		//10s
#define AIR_TIGHT_MAX_TIME          24000     		//�ܱ����Լ����ʱ�䣬
#define AIR_TIGHT_MAX_DIFF_PRESS    -5000    		//�ܱ������ѹ�

#define AGING_TEST_PRESS           	-40000   		//�����ϻ�ѹ��ֵ
#define NO_PRESS_VALUE           	-2000    		//�ж�Ϊ��ѹ����ֵ
#define QIZUI_TEST_PRESS           	-40000   		//�����ܱ��Բ���ѹ��ֵ






//ErrorCode_e Build_Press(int32_t nPress);
ErrorCode_e Build_Press(RunMode_e eRunMode, int32_t nPress);
uint16_t Press_ADC(void);
int32_t Press_Value(void);

FlagStatus Press_Is_Normal(void);
FlagStatus Press_Is_MoreThan(int32_t lPress);
FlagStatus Press_Is_LessThan(int32_t lPress);

ErrorCode_e Air_Tight_SelfCheck(int32_t nPress);

HAL_StatusTypeDef BuildPressTime(int32_t nPress,uint32_t* Time);
 
  
  
  
  
  
  
  
  
  
  
#ifdef __cplusplus
}
#endif

#endif //__BSP_PRESS_H_


