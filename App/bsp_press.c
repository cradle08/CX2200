#include "main.h"
#include "bsp_adc.h"
#include "bsp_press.h"
#include "bsp_outin.h"
#include "bsp_pump.h"
#include "bsp_motor.h"
#include "bsp_motor.h"
#include "monitorV.h"
#include "cmsis_os2.h"
#include "digPressure.h"


/* //cx2000ģ����ѹ�Ƶ������ӿ�

const uint32_t gs_ulK = 173280;
const uint32_t gs_ulB = 97380000;

//��ȡѹ��ADC
uint16_t Press_ADC(void)
{
  extern uint16_t g_usADC3_Buffer[ADC3_CH_END][ADC_CH_SMOOTH_NUM];
  uint8_t i = 0;
  uint32_t usSum = 0;
  
  for(i = 0; i < ADC_CH_SMOOTH_NUM; i++)
  {
    usSum += g_usADC3_Buffer[ADC3_CH_PRESS][i];
  }
  
  return (usSum/ADC_CH_SMOOTH_NUM);
}


//��ȡ��ѹֵ
int32_t Press_Value(void)
{
  uint16_t usADC  = 0;
  int32_t nValue = 0;
  double fValue = 0;

  usADC = Press_ADC();
  fValue = 0.66*usADC * ((double)gs_ulK);
  if (fValue <= (double)gs_ulB)
  {
      return 0;
  }
  fValue -= (double)gs_ulB;
  nValue = (int32_t)fValue + 10000000;

  return nValue;
}
*/



/*
* ��ȡ��ѹֵ��ģ�⴫�������0.5-3.0V����Ӧѹ����Χ��0kpa��-50kpa, --> 0.02kpa/mv = 20pa/mv
*/
#define PRESS_MV_ZERO_VALUE  500     //mv
int32_t Press_Value(void)
{
	extern MachInfo_s MachInfo;
    uint8_t i = 0;
	uint32_t usV  = 0;
	int32_t lPress = 0;

	//��ѹֵת����ѹ��ֵ
	if(MachInfo.paramSet.pressSensor != 0){
	  //ʹ��HONEYWELLѹ�����������������...
		for(i = 0; i < 5; i++)
		{
			usV  += Press_V();
		}
		usV /= 5;
		
		lPress = 7810.66 - 15.816*usV;
	}else{
		//ģ����ѹ�Ƶĵ�ѹֵ
		for(i = 0; i < 5; i++)
		{
			usV  += Press_V();
		}
		usV /= 5;
		//ʹ��IT110Aѹ��������
		lPress = -(20*usV-6200);
	}

	return lPress - MachInfo.paramSet.fPress_Adjust*1000;

}






/*
*	�жϵ�ǰ������ѹ���Ƿ��ں���Χ��
*/
FlagStatus Press_Is_Normal(void)
{
    int32_t nTempPress = 0;
    
    nTempPress = Press_Value();
    if(nTempPress < MAX_BC_TEST_PRESS_VALUE || nTempPress > MIN_BC_TEST_PRESS_VALUE)//��ֵ�Ƚ�
    {
        //ѹ�����ں���Χ�ڣ�
        return RESET;
    }else{
        //ѹ���ں���Χ��
        return SET;
    }  
}


/*
*	�жϵ�ǰ������ѹ���Ƿ����lPress������ǰѹ����Ϊ��ֵ��
*/
FlagStatus Press_Is_LessThan(int32_t lPress)
{
    int32_t nTempPress = 0;
    
    nTempPress = Press_Value();
    if(nTempPress < lPress)//��ֵ�Ƚ�
    {
        //С��
        return SET;
    }else{       
        return RESET;
    } 
}


/*
*	�жϵ�ǰ������ѹ���Ƿ����lPress������ǰѹ����Ϊ��ֵ��
*/
FlagStatus Press_Is_MoreThan(int32_t lPress)
{
    int32_t nTempPress = 0;
    
    nTempPress = Press_Value();
    if(nTempPress > lPress)//��ֵ�Ƚ�
    {
        //����
        return SET;
    }else{
        return RESET;
    } 
}



/*
*	��ѹ��ָ��ѹ��ֵ, eRunMode:�ýӿ�ʹ�õ�ģʽ������������Ϊ������ģʽ������ģʽ������������ʱ��
*/
ErrorCode_e Build_Press(RunMode_e eRunMode, int32_t nPress)
{
    extern  MachInfo_s MachInfo;
    uint32_t ulStartTick = 0, ulEndTick = 0, ulTemp = 0;
    int32_t nTempPress = 0;
	
    //��ȡѹ��ֵ
    nTempPress = Press_Value();

    //��ʱ��ʼ
    ulStartTick = HAL_GetTick();
    ulEndTick   = ulStartTick;
    ulTemp      = ulStartTick;

    //����������
    AIR_WAVE_OPEN;
    App_Delay(eRunMode, 400);
    Pump_Start();

    //ѹ��ʵʱ���
    while(ulEndTick - ulStartTick <= BUILD_PRESS_MAX_TIME)
    {
        nTempPress =  Press_Value();
        if(nTempPress <=  nPress)
        {
            break;
        }
        //
        if(ulEndTick - ulTemp >= 10) //100ms
        {
            ulTemp = ulEndTick;
        }
        //
        App_Delay(eRunMode, 10);
        ulEndTick = HAL_GetTick();
    }
    //�ط����ر�
    AIR_WAVE_CLOSE;
    App_Delay(eRunMode, 400);
    Pump_Stop();

    //��ѹ��ʱ
    if(ulEndTick - ulStartTick > BUILD_PRESS_MAX_TIME)
    {
        LOG_Info("Build Press Timeout(%d)", ulEndTick - ulStartTick);
        return ERROR_CODE_BUILD_PRESS;
    }

    return ERROR_CODE_SUCCESS;
}



/***
*��ѹ���������ѹʱ��
*������
	nPress��Ҫ����ѹ��ֵ
	Time����ѹʱ�䣬��λms
*����ֵ����ѹ�ɹ��򷵻�HAL_OK������ʱ����HAL_TIMEOUT
***/
HAL_StatusTypeDef BuildPressTime(int32_t nPress,uint32_t* Time)
{
	uint32_t StartTimestamp = 0;
	uint32_t EndTimestamp = 0;
	uint32_t TimeoutCount = 0;
	
	StartTimestamp = HAL_GetTick();
	Pump_Start();
	AIR_WAVE_OPEN;
	
	while(1){
		if(Press_Value() < nPress){
			break;
		}
		
		if(TimeoutCount > BUILD_PRESS_MAX_TIME/100){
			EndTimestamp = HAL_GetTick();
			*Time = EndTimestamp-StartTimestamp;
			
			AIR_WAVE_CLOSE;
			Pump_Stop();
			return HAL_ERROR;
		}
		
		TimeoutCount++;
		osDelay(100);
	}
	
	EndTimestamp = HAL_GetTick();
	*Time = EndTimestamp-StartTimestamp;
	
	AIR_WAVE_CLOSE;
	Pump_Stop();
	
	return HAL_OK;
}






/*
*	�ܱ����Լ�
*/
ErrorCode_e Air_Tight_SelfCheck(int32_t nPress)
{
    extern  MachInfo_s MachInfo;
    uint32_t ulStartTick = 0, ulEndTick = 0;
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
    int32_t nTempPress = 0;
    
    //��ѹ
    eErrorCode = Build_Press(RUN_MODE_NORMAL, nPress);
    osDelay(1000);
    eErrorCode = Build_Press(RUN_MODE_NORMAL, nPress);
    if(ERROR_CODE_SUCCESS != eErrorCode)
    {
        LOG_Error("Build Press Fail");
        return ERROR_CODE_BUILD_PRESS;
    }
    
    //��ʱ��ʼ
    ulStartTick = osKernelGetTickCount();
    ulEndTick   = ulStartTick;
    //�ܱ����Լ죬����AIR_TIGHT_MAX_TIME��
    while(ulEndTick - ulStartTick <= AIR_TIGHT_MAX_TIME)
    {
        nTempPress =  Press_Value();
        if(nPress - nTempPress < AIR_TIGHT_MAX_DIFF_PRESS)
        {
            LOG_Error("CurPress=%d, nPress=%d, diff=%d", nTempPress, nPress, nPress-nTempPress);
            return ERROR_CODE_AIR_TIGHT;;
        }
        //
        osDelay(10);
        ulEndTick = osKernelGetTickCount();
    }   

    return ERROR_CODE_SUCCESS;
}




