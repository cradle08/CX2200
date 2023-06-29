#ifndef __BSP_PRESS_H_
#define __BSP_PRESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"
#include "msg_def.h"

#define USE_ANALOG_PRESS_SENSOR_OR_NOT   1          //1:使用模拟压力传感器，0：使用数据压力传感     

#define BC_TEST_PRESS_VALUE         -30000    		//-测试时使用的压力值
#define MAX_BC_TEST_PRESS_VALUE     -33000    		//最大测试压力值
#define MIN_BC_TEST_PRESS_VALUE     -30000    		//最小测试压力值#define BC_TEST_MIN_PRESS           -25000    //漏气判断最小压力
#define AIR_LEAK_PRESS              -25000    		//漏气判断最小压力
#define POWER_OFF_PRESS_VALUE       -5000           //关机时，如果压力大于该值，放气
		
#define BUILD_PRESS_MAX_TIME        10000     		//10s
#define AIR_TIGHT_MAX_TIME          24000     		//密闭性自检最大时间，
#define AIR_TIGHT_MAX_DIFF_PRESS    -5000    		//密闭性最大压差，

#define AGING_TEST_PRESS           	-40000   		//流程老化压力值
#define NO_PRESS_VALUE           	-2000    		//判断为无压力的值
#define QIZUI_TEST_PRESS           	-40000   		//气嘴密闭性测试压力值






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


