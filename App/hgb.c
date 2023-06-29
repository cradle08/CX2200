#include "main.h"
#include "usart.h"
#include "bsp_outin.h"
#include "cmsis_os.h"
#include "hgb.h"
#include "monitorV.h"


//��·DACͨ����dac1�� pa4
#define LIGHT_DAC_CHANNEL   DAC_CHANNEL_1
#define LIGHT_MAX_CURRENT   100   //100ma


/*
*   ���ù�·DAC�����ֵ
*/
void Light_Set_DAC(uint16_t usDAC)
{
    uint16_t usVal = 0;
    extern DAC_HandleTypeDef hdac;
    
    //��ȡDACֵ
    if(usDAC > 4095)
    {
        usDAC = 4095;
    }
    usVal = usDAC;
    
    //����DAC���
    HAL_DAC_SetValue(&hdac, LIGHT_DAC_CHANNEL, DAC_ALIGN_12B_R, usVal);
    HAL_DAC_Start(&hdac, LIGHT_DAC_CHANNEL);
}




/*
*   ���ù�·���ֵ,������С�� ��������DAC�������Ȼ���ٴ򿪹�·����Դ���أ������������
*   DACԤADC�Ĺ�ϵ��Current = V/z = (ADC*3300/4095)/100 = ADC*33/4095
*/
void Light_Set_Current(uint16_t usCurrent)  
{
    uint16_t usDAC = 0;
    
    //�������ֵ
    if(usCurrent > LIGHT_MAX_CURRENT)  //
    {
        usCurrent = LIGHT_MAX_CURRENT;
    }
    
    //Current to ADC
    usDAC = usCurrent*33/4095;
    
    //����DAC
    Light_Set_DAC(usDAC);
    
    //�򿪹�·����Դ
    LIGHT_SW_ON;
    Delay_US(100);
}


