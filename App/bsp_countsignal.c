#include "main.h"
#include "tim.h"
#include "bsp_countsignal.h"



/*
*  计数信号初始化
*/
void Count_Signal_Init(void)
{
  MX_TIM12_Init();
}  


/*
*  计数信号开启
*/
void Count_Signal_Start(void)
{
  HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
}

/*
*  计数信号关闭
*/
void Count_Signal_Stop(void)
{
  HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_1);
}


/*
* 计数信号设置占空比(0-100), fRatio, 频率为1K，周期1ms，正空比宽度：10000, 
*   目前fRatio=0,   PWM_H=0us
*   目前fRatio=0.5, PWM_H=5us
*   目前fRatio=1,   PWM_H=10us
*   目前fRatio=20,  PWM_H=200us
*/
void Count_Signal_SetFreq(float fRatio)
{
  TIM_OC_InitTypeDef sConfigOC = {0};
  uint16_t usFreq = 0;
  
  //获取占空比，数值
  if(fRatio > 100)
  {
    fRatio = 100;
  }
  usFreq = 10000*fRatio/100;

  //设置
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = usFreq;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

}
 












