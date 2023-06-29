#include "bsp_adc.h"
#include "main.h"
#include "monitorV.h"








/*
*   ��ȡС��ADCֵ
*/
uint16_t XK_ADC(void)
{
	return MonitorV_ADC(MONITORV_XK);
}


/*
*��ȡС�׵�ѹֵ����λmv
*/
uint16_t XK_V(void)
{
	return XK_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
}


/*
*  С�׵�ѹ�Ƿ�С�� ulV
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
*   ��ȡС�׵�ѹ�Ƿ����ulV
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
*��ȡ����ԴADCֵ
*/
uint16_t BC_Current_ADC(void)
{
  return MonitorV_ADC(MONITORV_BC_CUR);
}


/*
*��ȡ����Դ��ѹ��mv
*/
uint32_t BC_Current_ADC_V(void)
{
  return BC_Current_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
}


/*
*��ȡ����Դ��ѹ��mv (��ǰΪ56V)
*/
uint32_t BC_Current_V(void)
{
  return BC_Current_ADC()*MCU_ADC_REF_V_3300MV*21/MCU_ADC12BIT_RESOLUTION;
}


/*
*   BC����Դ�Ƿ�������56V���� ���ƫ��
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


/* �����缫��ѹ */
uint16_t Elec_ADC(void)
{
	return MonitorV_ADC(MONITORV_ELEC);
}

uint16_t Elec_V(void)
{
	return Elec_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
}



/* 
* 12V�����ѹ������ ��ADCֵ
*/
uint16_t Power_12VN_ADC(void)
{
	return MonitorV_ADC(MONITORV_N12V);
}


/* 
* 12V�����ѹ����������ѹֵ
*/
int16_t Power_12VN_V(void)
{
	return -Power_12VN_ADC()*MCU_ADC_REF_V_3300MV*4/MCU_ADC12BIT_RESOLUTION;
}


/*
*   ��12V�Ƿ�������
*/
ErrorCode_e Power_12VN_Is_Normal(void)
{
    int16_t sTemp = 0;  
    sTemp = Power_12VN_V();
    
    //���0.5V
    if(sTemp < -12000-POWER_12V_MAX_DEVIATION || sTemp > -12000+POWER_12V_MAX_DEVIATION)
    {
        return ERROR_CODE_POWER_12VN;
    }
    
    return ERROR_CODE_SUCCESS;
}




/* 12V�����ѹ������ */
uint16_t Power_12VP_ADC(void)
{
	return MonitorV_ADC(MONITORV_P12V);
}


/* 
* 12V�����ѹ����������ѹֵ
*/
uint16_t Power_12VP_V(void)
{
	return Power_12VP_ADC()*MCU_ADC_REF_V_3300MV*4/MCU_ADC12BIT_RESOLUTION;
}


/*
*   ��12V�Ƿ�������
*/
ErrorCode_e Power_12VP_Is_Normal(void)
{
    int16_t sTemp = 0;
    sTemp = Power_12VP_V();
    
    //���0.5V
    if(sTemp > 12000 + POWER_12V_MAX_DEVIATION || sTemp < 12000 - POWER_12V_MAX_DEVIATION)
    {
        return ERROR_CODE_POWER_12VP;
    }
    
    return ERROR_CODE_SUCCESS;
}


///* �������(������/���ӵ��)�������ο���ѹADC */
//uint16_t Motor_Refer_V_ADC(void)  
//{
//	return MonitorV_ADC(MONITORV_MOTOR_I);
//}


///* �������(������/���ӵ��)�������ο���ѹ�������������Ĺ�ϵΪ��I=0.766V�� */
//uint16_t Motor_Refer_V(void)
//{
//	return Motor_Refer_V_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
//}

////�����������Ϊ I = 0.766V
//uint16_t Motor_Driver_I(void)
//{

//    return 0.766*Motor_Refer_V();
//}


////��·����Դ��ѹ
//uint16_t Light_Current_ADC(void)
//{
//	return MonitorV_ADC(MONITORV_LIGHT_CUR);
//}

//uint16_t Ligth_Current_V(void)
//{
//	return Light_Current_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
//}


//ģ���¶ȴ����������ѹֵ����ת��Ϊ�¶�ֵ����
uint16_t NTC_ADC(void)
{
	return MonitorV_ADC(MONITORV_NTC);
}

uint16_t NTC_V(void)
{
	return NTC_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
}


/***
*NTC���¶ȴ�-10�浽50���Ӧ��NTC���Ե��ѹֵ��mv����60��Ԫ��
***/
const uint16_t NTC_V2T[] = {
	2805,2782,2757,2732,2706,2679,2652,2624,2595,2566,2535,2505,2473,2441,2408,
	2375,2341,2307,2272,2237,2202,2166,2130,2093,2057,2020,1983,1946,1909,1872,
	1834,1797,1760,1723,1686,1650,1613,1577,1541,1505,1470,1435,1401,1367,1334,
	1301,1267,1236,1204,1173,1143,1113,1083,1055,1027,998,971,944,919,894
};



/*
*   ��ȡNTC �¶�
*/
int16_t NTC_Temperature(void)
{
   
    uint32_t ulNTC = 0;
    uint16_t  i = 0;
    
    //ƽ��
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


//ģ����ѹ�������ѹֵ����ת��Ϊ��ѹֵ
uint16_t Press_ADC(void)
{
	return MonitorV_ADC(MONITORV_PRESS);
}

uint16_t Press_V(void)
{
	return Press_ADC();
}



// HGB����ֵ
uint16_t HGB_ADC(void)
{
	return MonitorV_ADC(MONITORV_HGB);
}

//HGB��ѹֵ
uint16_t HGB_V(void)
{
	return HGB_ADC()*MCU_ADC_REF_V_3300MV/MCU_ADC12BIT_RESOLUTION;
}



