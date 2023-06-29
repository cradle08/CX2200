#include "gpio.h"
#include "bsp_outin.h"
#include "main.h"
#include "cmsis_os2.h"

/***********************************************************
* out
*/


/*
*  ִ��Һ�����أ�ulNum����
*/
void Liquid_Valve_Exec(uint32_t ulNum, uint32_t ulDelayTime)
{
    uint32_t i = 0;
    
    for(i = 0; i < ulNum; i++)
    {
        LIQUID_WAVE_OPEN;
        osDelay(ulDelayTime); 
        LIQUID_WAVE_CLOSE;
        osDelay(ulDelayTime);
    }
}



/*
*  ִ���������أ�ulNum����
*/
void Air_Valve_Exec(uint32_t ulNum, uint32_t ulDelayTime)
{
    uint32_t i = 0;
    
    for(i = 0; i < ulNum; i++)
    {
        AIR_WAVE_OPEN;
        osDelay(ulDelayTime);
        AIR_WAVE_CLOSE;
        osDelay(ulDelayTime);
    }
}






/*
*  spi cs en���͵�ƽʹ�ܣ��ߵ�ƽʧ��
*/
void SPI_CS_Enable(SPI_CS_e eIndex)
{
    if(eIndex >= SPI_CS_END)
    {
        LOG_Error("SPI CS Index(%d) Error", eIndex);
    }
    
    switch(eIndex)
    {
        case SPI_CS_FLASH:
        {
            FLASH_SPI_CS_LOW;
            TP_SPI_CS_HIGH;
            DR0_SPI_CS_HIGH;
            DR1_SPI_CS_HIGH;  
        }
        break;
        case SPI_CS_TP:
        {
            FLASH_SPI_CS_HIGH;
            TP_SPI_CS_LOW;
            DR0_SPI_CS_HIGH;
            DR1_SPI_CS_HIGH; 
        }
        break;        
        case SPI_CS_DR0:
        {
            FLASH_SPI_CS_HIGH;
            TP_SPI_CS_HIGH;
            DR0_SPI_CS_LOW;
            DR1_SPI_CS_HIGH; 
        }
        break;
        case SPI_CS_DR1:
        {
            FLASH_SPI_CS_HIGH;
            TP_SPI_CS_HIGH;
            DR0_SPI_CS_HIGH;
            DR1_SPI_CS_LOW; 
        }
        break;
        default:
            break;    
    }

}


/*
*  spi cs disable
*/
void SPI_CS_Disable(SPI_CS_e eIndex)
{
    if(eIndex >= SPI_CS_END)
    {
        LOG_Error("SPI CS Index(%d) Error", eIndex);
    }
    
    //
    switch(eIndex)
    {
        case SPI_CS_FLASH:
        {
            FLASH_SPI_CS_HIGH;
        }
        break;
        case SPI_CS_TP:
        {
            TP_SPI_CS_HIGH;
        }
        break;        
         case SPI_CS_DR0:
        {
            DR0_SPI_CS_HIGH;
        }
        break;
        case SPI_CS_DR1:
        {
            DR1_SPI_CS_HIGH;
        }
        break;
        default:
            break;    
    }   
}



/*
*  spi cs disable all
*/
void SPI_CS_Disable_All(void)
{
    uint8_t i = 0;
    
    for(i = SPI_CS_FLASH; i < SPI_CS_END; i++)
    {
        SPI_CS_Disable(i);
    }

}



/*
*   �������ָʾ�ƣ�״̬
*/
void Panel_Led_Status(RunMode_e eRunMode, Panel_LED_Status_e eStatus)
{
    if(RUN_MODE_COUNT == eRunMode){
        
        switch(eStatus)
        {
            case PANEL_LED_STATUS_NORMAL: //����״̬�����̵���ҫ
            {
                MENU_GREEN_LED_TRIGGLE;
                MENU_RED_LED_OFF;            
            }
            break;        
            case PANEL_LED_STATUS_WARN:   //�澯״̬��������, �̵ƺ�ƶ���(��)
            {
                MENU_GREEN_LED_TRIGGLE;
                MENU_RED_LED_TRIGGLE;
            }
            break;   
            case PANEL_LED_STATUS_ERROR:  //����״̬�������������������
            {
                MENU_GREEN_LED_OFF;
                MENU_RED_LED_TRIGGLE;
            }
            break;  
            case PANEL_LED_STATUS_OFF:   //�ر�״̬����
            {
                MENU_GREEN_LED_OFF;
                MENU_RED_LED_OFF;
            }
            break;   
            default:break;
        }
    }else if(RUN_MODE_NORMAL == eRunMode){
        switch(eStatus)
        {
            case PANEL_LED_STATUS_NORMAL:   //����״̬, ���̵���
            {
                MENU_GREEN_LED_ON;
                MENU_RED_LED_OFF;
            }
            break;      
            case PANEL_LED_STATUS_WARN: //�澯״̬, �̵ƺ�ƶ���
            {
                MENU_GREEN_LED_ON;
                MENU_RED_LED_ON;
            }
            break;
            case PANEL_LED_STATUS_ERROR:    //����״̬���������
            {
                MENU_GREEN_LED_OFF;
                MENU_RED_LED_ON;
            }
            break;       
            case PANEL_LED_STATUS_OFF:  //�ر�״̬����
            {
                MENU_GREEN_LED_OFF;
                MENU_RED_LED_OFF;
            }
            break;          
            default:break;            
        }       
    }
}




/***********************************************************
* in  
*/

/*
*   ��ȡָ������״̬,  �ߣ��ڵ������������ͣ�������δ������
*/
uint8_t OC_Status(OC_Index_e eOC)
{
    uint8_t ucVal = 0, i = 0, ucNum = 11;   
    
    if(eOC >= OC_INDEX_END)
    {
        LOG_Error("eOC(%d) Error", eOC);
        return ucVal;
    }
    
    //��ȡ��Ӧ����״̬
    switch(eOC)
    {
        case OC_INDEX_OUTIN_IN:
        {
            for(i = 0; i < ucNum; i ++)
            {
                ucVal += OUTIN_IN_OC_STATUS;
            }
        }
        break;
        case OC_INDEX_OUTIN_OUT:
        {
            for(i = 0; i < ucNum; i ++)
            {
                ucVal += OUTIN_OUT_OC_STATUS;
            }
        }
        break;
        case OC_INDEX_CLAMP_IN:
        {
            for(i = 0; i < ucNum; i ++)
            {
                ucVal += CLAMP_IN_OC_STATUS;
            }
        }
        break;
        case OC_INDEX_CLAMP_OUT:
        {
            for(i = 0; i < ucNum; i ++)
            {
                ucVal += CLAMP_OUT_OC_STATUS;
            }
        }
        break;
        default:
            break;
    }
    
    //
    return ucVal>=(ucNum>>1)?1:0;
}




/*
* ��ȡ�����缫״̬
*/
uint8_t Get_Elec_Status(void)
{
    uint8_t i = 0, sum = 0, ucNum = 11;
    
    for(i = 0; i < ucNum; i++)
    {
        sum += HAL_GPIO_ReadPin(ELEC_GPIO_Port, ELEC_Pin);
    }
     
    return sum>=(ucNum>>1)?1:0;
}



/*
*   ���������޼��,���ƺ� 1:δ����Ѫϸ�����ģ�飬0������Ѫϸ�����ģ��
*  �ڵƲ�������ת���ܻ���֮��, MCU���״̬Ϊ0(����״̬).-->�����ƺ��Ϊ0=>�ѷ���Ѫϸ�����ģ��
*   ���ƺ�, SET:�ѷ���Ѫϸ�����ģ�飬 RESET:δ����Ѫϸ�����ģ��
*/

FlagStatus ConutCell_Is_Exist(void)
{
    uint8_t i = 0, sum = 0;
    
	osKernelLock();
	
    //����
    COUNT_CELL_SW_ON;
    HAL_Delay(20);
    
    //
    for(i = 0; i < 5; i++)
    {
        if(CHECK_JSC_EXIST_STATUS == JSC_EXIST_STATUS_ACTIVE)
        {
            sum += 1;
        }
        HAL_Delay(10);
    }
	
	osKernelUnlock();
    
    //�ص�
    COUNT_CELL_SW_OFF;
    
    return sum>3?SET:RESET;
}



/***
*ֱ�ӷ���ָ��IO����״̬
***/
uint8_t Input_GetState(InputCH_e InputCH)
{
	switch(InputCH){
		case INPUT_CH_CAT_STATUS:
			return CAT_START_STATUS;
		default :return 0xff;
	}
}






