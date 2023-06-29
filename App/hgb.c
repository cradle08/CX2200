#include "main.h"
#include "usart.h"
#include "bsp_outin.h"
#include "cmsis_os.h"
#include "hgb.h"
#include "monitorV.h"


//光路DAC通道，dac1， pa4
#define LIGHT_DAC_CHANNEL   DAC_CHANNEL_1
#define LIGHT_MAX_CURRENT   100   //100ma


/*
*   设置光路DAC，输出值
*/
void Light_Set_DAC(uint16_t usDAC)
{
    uint16_t usVal = 0;
    extern DAC_HandleTypeDef hdac;
    
    //获取DAC值
    if(usDAC > 4095)
    {
        usDAC = 4095;
    }
    usVal = usDAC;
    
    //设置DAC输出
    HAL_DAC_SetValue(&hdac, LIGHT_DAC_CHANNEL, DAC_ALIGN_12B_R, usVal);
    HAL_DAC_Start(&hdac, LIGHT_DAC_CHANNEL);
}




/*
*   设置光路输出值,电流大小， 需先设置DAC的输出，然后再打开光路恒流源开关，否则无输出，
*   DAC预ADC的关系，Current = V/z = (ADC*3300/4095)/100 = ADC*33/4095
*/
void Light_Set_Current(uint16_t usCurrent)  
{
    uint16_t usDAC = 0;
    
    //检测输入值
    if(usCurrent > LIGHT_MAX_CURRENT)  //
    {
        usCurrent = LIGHT_MAX_CURRENT;
    }
    
    //Current to ADC
    usDAC = usCurrent*33/4095;
    
    //设置DAC
    Light_Set_DAC(usDAC);
    
    //打开光路恒流源
    LIGHT_SW_ON;
    Delay_US(100);
}


