#include "main.h"
#include "tim.h"
#include "cmsis_os.h"
#include "bsp_outin.h"
#include "bsp_beep.h"



/*
* �ó�ʼ��
*/
void Beep_Init(void)
{
    Beep_SetFreq(9, 2500); //4k
}  


/*
* ����������
*/
void Beep_Start(void)
{
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
}

/*
* �������ر�
*/
void Beep_Stop(void)
{
  HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_2);
}


/*
*  ������Ƶ�����ã� usPsc��AP1(90M)��Ƶϵ���� usArr����װ��ֵ
*/
void Beep_SetFreq(uint16_t usPsc, uint16_t usArr) 
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim5.Instance = TIM5;
    htim5.Init.Prescaler = usPsc - 1;
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = usArr;
    htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
    {
    Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
    {
    Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = usArr/2;          //ռ�ձ�50%
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
    Error_Handler();
    }
    HAL_TIM_MspPostInit(&htim5);

}
  
  
//������,���ڵ�ͨ��
void Beep_Period(uint8_t ucNum, uint32_t ulPeriod)
{
    uint8_t i = 0;
    
    for(i = 0; i < ucNum; i++)
    {
        Beep_Start();
        osDelay(ulPeriod);
        Beep_Stop();
        osDelay(ulPeriod);
    }
}



//������,���ڵ�ͨ��
void Beep_Exec(Bool eFlag)
{
    if(TRUE == eFlag)
    {
        Beep_Start();
    }else{
        Beep_Stop();
    }
}
