#include "gpio.h"
#include "bsp_outin.h"
#include "main.h"
#include "cmsis_os2.h"

/***********************************************************
* out
*/


/*
*  执行液阀开关，ulNum次数
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
*  执行气阀开关，ulNum次数
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
*  spi cs en，低电平使能，高电平失能
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
*   设置面板指示灯，状态
*/
void Panel_Led_Status(RunMode_e eRunMode, Panel_LED_Status_e eStatus)
{
    if(RUN_MODE_COUNT == eRunMode){
        
        switch(eStatus)
        {
            case PANEL_LED_STATUS_NORMAL: //计数状态，仅绿灯闪耀
            {
                MENU_GREEN_LED_TRIGGLE;
                MENU_RED_LED_OFF;            
            }
            break;        
            case PANEL_LED_STATUS_WARN:   //告警状态（计数）, 绿灯红灯都亮(闪)
            {
                MENU_GREEN_LED_TRIGGLE;
                MENU_RED_LED_TRIGGLE;
            }
            break;   
            case PANEL_LED_STATUS_ERROR:  //出错状态（计数），仅红灯亮闪
            {
                MENU_GREEN_LED_OFF;
                MENU_RED_LED_TRIGGLE;
            }
            break;  
            case PANEL_LED_STATUS_OFF:   //关闭状态，黑
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
            case PANEL_LED_STATUS_NORMAL:   //运行状态, 仅绿灯亮
            {
                MENU_GREEN_LED_ON;
                MENU_RED_LED_OFF;
            }
            break;      
            case PANEL_LED_STATUS_WARN: //告警状态, 绿灯红灯都亮
            {
                MENU_GREEN_LED_ON;
                MENU_RED_LED_ON;
            }
            break;
            case PANEL_LED_STATUS_ERROR:    //出错状态，仅红灯亮
            {
                MENU_GREEN_LED_OFF;
                MENU_RED_LED_ON;
            }
            break;       
            case PANEL_LED_STATUS_OFF:  //关闭状态，黑
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
*   获取指定光耦状态,  高：遮挡（触发），低：敞开（未触发）
*/
uint8_t OC_Status(OC_Index_e eOC)
{
    uint8_t ucVal = 0, i = 0, ucNum = 11;   
    
    if(eOC >= OC_INDEX_END)
    {
        LOG_Error("eOC(%d) Error", eOC);
        return ucVal;
    }
    
    //获取对应光耦状态
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
* 获取定量电极状态
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
*   计数池有无检测,开灯后， 1:未放入血细胞检测模块，0：放入血细胞检测模块
*  在灯不亮或光电转换管坏了之后, MCU检测状态为0(触发状态).-->当开灯后均为0=>已放入血细胞检测模块
*   开灯后, SET:已放入血细胞检测模块， RESET:未放入血细胞检测模块
*/

FlagStatus ConutCell_Is_Exist(void)
{
    uint8_t i = 0, sum = 0;
    
	osKernelLock();
	
    //开灯
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
    
    //关灯
    COUNT_CELL_SW_OFF;
    
    return sum>3?SET:RESET;
}



/***
*直接返回指定IO输入状态
***/
uint8_t Input_GetState(InputCH_e InputCH)
{
	switch(InputCH){
		case INPUT_CH_CAT_STATUS:
			return CAT_START_STATUS;
		default :return 0xff;
	}
}






