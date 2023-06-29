#include "main.h"
#include "usart.h"
#include "bsp_outin.h"
#include "cmsis_os.h"
#include "bsp_light.h"
#include "monitorV.h"
#include "bsp_resistance.h"


/*宏定义区***************************************************************/
//光路DAC通道，dac1， pa4
#define LIGHT_DAC_CHANNEL   DAC_CHANNEL_1
#define LIGHT_MAX_CURRENT   200   //100ma




/*全局变量定义区*********************************************************/
static uint32_t HGBStartTick = 0;





/*
*   设置光路DAC，来调节光路恒流源输出大小。 值越大，输出电流越大
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
*   光路的开启, 开启的方式为：需先打开DAC（给出一个默认值），然后再打开恒流源开关，
*/
void Light_Start(HGBLed_e HGBLed,uint16_t DACValue)  
{
	//记录启动灯时刻
	HGBStartTick = HAL_GetTick();
	
	if(HGBLed == HGBLED_535){
		HGB_535_ON;
		Light_Set_DAC(DACValue);
		HAL_Delay(2);
		
		//打开光路恒流源
		LIGHT_SW_ON;
	}else{
		HGB_415_ON;
		Light_Set_DAC(DACValue);
		HAL_Delay(2);
		
		//打开光路恒流源
		LIGHT_SW_ON;
	}
}



/*
*   光路的关闭
*/
void Light_Stop(void)  
{
    //关闭光路恒流源
    LIGHT_SW_OFF;    
    
    //清除DAC输出
    Light_Set_DAC(0);
}



/***
*判断距最后一次开灯动作是否已经到指定的时间间隔，是，返回HAL_OK
***/
HAL_StatusTypeDef Light_OpenIsTimeup(uint32_t Time)
{
	extern uint32_t HGBStartTick;
	
	if(HAL_GetTick() - HGBStartTick >= Time){
		return HAL_OK;
	}else{
		return HAL_ERROR;
	}
}




#define HGB_MAX_DIFF_VALL 2
/*
*   根据当前ADC和目标ADC的差值，获取数字电位器的增益的累加值, 
*    转换的关系，由实际测试获取。后续如有变动，需改动
*/
int16_t Get_Grain_Add(int16_t sDiff)
{
    int16_t sValue = 0;
    if(sDiff >= 0 && sDiff <= 15)
    {
        sValue = 1;
    }else if(sDiff >= -15 && sDiff < 0)
    {
        sValue = -1;
    
    }else{
       sValue = sDiff/10;
    }
    
    return sValue;
}



/*
*   根据当前ADC和目标ADC的差值，获取HGB LED 驱动电流累加值, 
*    转换的关系，由实际测试获取。后续如有变动，需改动
*/
int16_t Get_Current_Add(int16_t sDiff)
{
    int16_t sValue = 0;
    if(sDiff >= 0 && sDiff <= 15)
    {
        sValue = 1;
    }else if(sDiff >= -50 && sDiff < 0)
    {
        sValue = -1;
    
    }else{
       sValue = sDiff/2;
        if(sValue >= 250) sValue = 250;
    }
    
    return sValue;
}




