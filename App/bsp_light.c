#include "main.h"
#include "usart.h"
#include "bsp_outin.h"
#include "cmsis_os.h"
#include "bsp_light.h"
#include "monitorV.h"
#include "bsp_resistance.h"


/*�궨����***************************************************************/
//��·DACͨ����dac1�� pa4
#define LIGHT_DAC_CHANNEL   DAC_CHANNEL_1
#define LIGHT_MAX_CURRENT   200   //100ma




/*ȫ�ֱ���������*********************************************************/
static uint32_t HGBStartTick = 0;





/*
*   ���ù�·DAC�������ڹ�·����Դ�����С�� ֵԽ���������Խ��
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
*   ��·�Ŀ���, �����ķ�ʽΪ�����ȴ�DAC������һ��Ĭ��ֵ����Ȼ���ٴ򿪺���Դ���أ�
*/
void Light_Start(HGBLed_e HGBLed,uint16_t DACValue)  
{
	//��¼������ʱ��
	HGBStartTick = HAL_GetTick();
	
	if(HGBLed == HGBLED_535){
		HGB_535_ON;
		Light_Set_DAC(DACValue);
		HAL_Delay(2);
		
		//�򿪹�·����Դ
		LIGHT_SW_ON;
	}else{
		HGB_415_ON;
		Light_Set_DAC(DACValue);
		HAL_Delay(2);
		
		//�򿪹�·����Դ
		LIGHT_SW_ON;
	}
}



/*
*   ��·�Ĺر�
*/
void Light_Stop(void)  
{
    //�رչ�·����Դ
    LIGHT_SW_OFF;    
    
    //���DAC���
    Light_Set_DAC(0);
}



/***
*�жϾ����һ�ο��ƶ����Ƿ��Ѿ���ָ����ʱ�������ǣ�����HAL_OK
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
*   ���ݵ�ǰADC��Ŀ��ADC�Ĳ�ֵ����ȡ���ֵ�λ����������ۼ�ֵ, 
*    ת���Ĺ�ϵ����ʵ�ʲ��Ի�ȡ���������б䶯����Ķ�
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
*   ���ݵ�ǰADC��Ŀ��ADC�Ĳ�ֵ����ȡHGB LED ���������ۼ�ֵ, 
*    ת���Ĺ�ϵ����ʵ�ʲ��Ի�ȡ���������б䶯����Ķ�
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




