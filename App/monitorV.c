#include "bsp_adc.h"
#include "main.h"
#include "monitorV.h"








/*
*   获取小孔ADC值
*/
uint16_t XK_ADC(void)
{
	return MonitorV_ADC(MONITORV_XK);
}


/*
*获取小孔电压值，单位mv
*/
uint16_t XK_V(void)
{
	return XK_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
}


/*
*  小孔电压是否小于 ulV
*/
FlagStatus  XK_V_Is_LessThan(uint16_t ulV)
{
    uint16_t ulTempV = 0;
    
    ulTempV = XK_V();
    //LOG_Info("XK_V=%d", ulTempV);
    
    if(ulTempV < ulV)
    {
        return SET;
    }else{
        return RESET;
    }
}


/*
*   获取小孔电压是否大于ulV
*/
FlagStatus  XK_V_Is_MoreThan(uint16_t ulV)
{
    uint16_t ulTempV = 0;
    
    ulTempV = XK_V();
    //LOG_Info("XK_V=%d", ulTempV);
    
    if(ulTempV >= ulV)
    {
        return SET;
    }else{
        return RESET;
    }
}




/*
*获取恒流源ADC值
*/
uint16_t BC_Current_ADC(void)
{
  return MonitorV_ADC(MONITORV_BC_CUR);
}


/*
*获取恒流源电压，mv
*/
uint32_t BC_Current_ADC_V(void)
{
  return BC_Current_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
}


/*
*获取恒流源电压，mv (当前为56V)
*/
uint32_t BC_Current_V(void)
{
  return BC_Current_ADC()*MCU_ADC_REF_V_3300MV*21/MCU_ADC12BIT_RESOLUTION;
}


/*
*   BC恒流源是否正常（56V）， 最大偏差
*/
ErrorCode_e BC_Current_Is_Normal(void)
{
    uint32_t ulV = 0;
    
    ulV = BC_Current_V();
    if(ulV > 56000+BC_CURRENT_MAX_DEVIATION || ulV < 56000 - BC_CURRENT_MAX_DEVIATION)
    {
        return ERROR_CODE_BC_CURRENT;
    }
    return ERROR_CODE_SUCCESS;
}


/* 定量电极电压 */
uint16_t Elec_ADC(void)
{
	return MonitorV_ADC(MONITORV_ELEC);
}

uint16_t Elec_V(void)
{
	return Elec_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
}



/* 
* 12V供电电压，负极 ，ADC值
*/
uint16_t Power_12VN_ADC(void)
{
	return MonitorV_ADC(MONITORV_N12V);
}


/* 
* 12V供电电压，负极，电压值
*/
int16_t Power_12VN_V(void)
{
	return -Power_12VN_ADC()*MCU_ADC_REF_V_3300MV*4/MCU_ADC12BIT_RESOLUTION;
}


/*
*   正12V是否正常，
*/
ErrorCode_e Power_12VN_Is_Normal(void)
{
    int16_t sTemp = 0;  
    sTemp = Power_12VN_V();
    
    //误差0.5V
    if(sTemp < -12000-POWER_12V_MAX_DEVIATION || sTemp > -12000+POWER_12V_MAX_DEVIATION)
    {
        return ERROR_CODE_POWER_12VN;
    }
    
    return ERROR_CODE_SUCCESS;
}




/* 12V供电电压，正极 */
uint16_t Power_12VP_ADC(void)
{
	return MonitorV_ADC(MONITORV_P12V);
}


/* 
* 12V供电电压，正极，电压值
*/
uint16_t Power_12VP_V(void)
{
	return Power_12VP_ADC()*MCU_ADC_REF_V_3300MV*4/MCU_ADC12BIT_RESOLUTION;
}


/*
*   正12V是否正常，
*/
ErrorCode_e Power_12VP_Is_Normal(void)
{
    int16_t sTemp = 0;
    sTemp = Power_12VP_V();
    
    //误差0.5V
    if(sTemp > 12000 + POWER_12V_MAX_DEVIATION || sTemp < 12000 - POWER_12V_MAX_DEVIATION)
    {
        return ERROR_CODE_POWER_12VP;
    }
    
    return ERROR_CODE_SUCCESS;
}


///* 步进电机(进出仓/夹子电机)的驱动参考电压ADC */
//uint16_t Motor_Refer_V_ADC(void)  
//{
//	return MonitorV_ADC(MONITORV_MOTOR_I);
//}


///* 步进电机(进出仓/夹子电机)的驱动参考电压，和驱动电流的关系为（I=0.766V） */
//uint16_t Motor_Refer_V(void)
//{
//	return Motor_Refer_V_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
//}

////电机驱动电流为 I = 0.766V
//uint16_t Motor_Driver_I(void)
//{

//    return 0.766*Motor_Refer_V();
//}


////光路恒流源电压
//uint16_t Light_Current_ADC(void)
//{
//	return MonitorV_ADC(MONITORV_LIGHT_CUR);
//}

//uint16_t Ligth_Current_V(void)
//{
//	return Light_Current_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
//}


//模拟温度传感器输出电压值，可转化为温度值（）
uint16_t NTC_ADC(void)
{
	return MonitorV_ADC(MONITORV_NTC);
}

uint16_t NTC_V(void)
{
	return NTC_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
}


/***
*NTC，温度从-10℃到50℃对应的NTC测试点电压值，mv，共60个元素
***/
const uint16_t NTC_V2T[] = {
	2805,2782,2757,2732,2706,2679,2652,2624,2595,2566,2535,2505,2473,2441,2408,
	2375,2341,2307,2272,2237,2202,2166,2130,2093,2057,2020,1983,1946,1909,1872,
	1834,1797,1760,1723,1686,1650,1613,1577,1541,1505,1470,1435,1401,1367,1334,
	1301,1267,1236,1204,1173,1143,1113,1083,1055,1027,998,971,944,919,894
};



/*
*   获取NTC 温度
*/
int16_t NTC_Temperature(void)
{
   
    uint32_t ulNTC = 0;
    uint16_t  i = 0;
    
    //平滑
    for(i = 0; i < 10; i++)
    {
        ulNTC += NTC_V();
    }
    ulNTC /= 10;
	
	if(ulNTC > NTC_V2T[0]){
		return -10;
	}
	
	if(ulNTC < NTC_V2T[59]){
		return 50;
	}
    
	for(i=0;i<60;i++){
		if(ulNTC > NTC_V2T[i]){
			break;
		}
	}
	
	return i-10;
}    


//模拟气压计输出电压值，可转化为气压值
uint16_t Press_ADC(void)
{
	return MonitorV_ADC(MONITORV_PRESS);
}

uint16_t Press_V(void)
{
	return Press_ADC();
}



// HGB采样值
uint16_t HGB_ADC(void)
{
	return MonitorV_ADC(MONITORV_HGB);
}

//HGB电压值
uint16_t HGB_V(void)
{
	return HGB_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
}



