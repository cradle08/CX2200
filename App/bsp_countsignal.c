#include "main.h"
#include "tim.h"
#include "bsp_countsignal.h"



/*
*  �����źų�ʼ��
*/
void Count_Signal_Init(void)
{
  MX_TIM12_Init();
}  


/*
*  �����źſ���
*/
void Count_Signal_Start(void)
{
  HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
}

/*
*  �����źŹر�
*/
void Count_Signal_Stop(void)
{
  HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_1);
}


/*
* �����ź�����ռ�ձ�(0-100), fRatio, Ƶ��Ϊ1K������1ms�����ձȿ�ȣ�10000, 
*   ĿǰfRatio=0,   PWM_H=0us
*   ĿǰfRatio=0.5, PWM_H=5us
*   ĿǰfRatio=1,   PWM_H=10us
*   ĿǰfRatio=20,  PWM_H=200us
*/
void Count_Signal_SetFreq(float fRatio)
{
  TIM_OC_InitTypeDef sConfigOC = {0};
  uint16_t usFreq = 0;
  
  //��ȡռ�ձȣ���ֵ
  if(fRatio > 100)
  {
    fRatio = 100;
  }
  usFreq = 10000*fRatio/100;

  //����
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = usFreq;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

}
 












