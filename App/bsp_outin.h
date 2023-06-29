#ifndef __BSP_OUTIN_H_
#define __BSP_OUTIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"

/*******************************************************************************************************
* out
*/

//nand  空闲、忙碌状态检测
#define NAND_READY_BUSY_Port     GPIOD
#define NAND_READY_BUSY_Pin      GPIO_PIN_6

//面板，绿灯控制,高有效
#define MENU_GREEN_LED_ON        HAL_GPIO_WritePin(MENU_GREEN_GPIO_Port, MENU_GREEN_Pin, GPIO_PIN_SET)
#define MENU_GREEN_LED_OFF       HAL_GPIO_WritePin(MENU_GREEN_GPIO_Port, MENU_GREEN_Pin, GPIO_PIN_RESET)
#define MENU_GREEN_LED_TRIGGLE   HAL_GPIO_TogglePin(MENU_GREEN_GPIO_Port, MENU_GREEN_Pin)

//面板，红灯控制, 高有效
#define MENU_RED_LED_ON         HAL_GPIO_WritePin(MENU_RED_GPIO_Port, MENU_RED_Pin, GPIO_PIN_SET)
#define MENU_RED_LED_OFF        HAL_GPIO_WritePin(MENU_RED_GPIO_Port, MENU_RED_Pin, GPIO_PIN_RESET)
#define MENU_RED_LED_TRIGGLE    HAL_GPIO_TogglePin(MENU_RED_GPIO_Port, MENU_RED_Pin)

 //主板LED指示灯， 高有效
#define LED0_ON                 HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET)
#define LED0_OFF                HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET)
#define LED0_TRIGGLE            HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin)


//BC 恒流源（56V）开关， 高有效，默认处于关闭状态，只在需要设计56V电压，小孔电压相关的时候才开启，并且用完及时关闭
#define BC_CUR_SW_ON           HAL_GPIO_WritePin(BC_CURRENT_SW_GPIO_Port, BC_CURRENT_SW_Pin, GPIO_PIN_SET)
#define BC_CUR_SW_OFF          HAL_GPIO_WritePin(BC_CURRENT_SW_GPIO_Port, BC_CURRENT_SW_Pin, GPIO_PIN_RESET)
//Nand flash写保护，默认配置OFF
#define NAND_WP_OFF            HAL_GPIO_WritePin(NAND_WP_GPIO_Port, NAND_WP_Pin, GPIO_PIN_SET)
#define NAND_WP_ON             HAL_GPIO_WritePin(NAND_WP_GPIO_Port, NAND_WP_Pin, GPIO_PIN_RESET)


//光路恒流源，开关控制，高有效
#define LIGHT_SW_ON             HAL_GPIO_WritePin(LIGHT_SW_GPIO_Port, LIGHT_SW_Pin, GPIO_PIN_SET)
#define LIGHT_SW_OFF            HAL_GPIO_WritePin(LIGHT_SW_GPIO_Port, LIGHT_SW_Pin, GPIO_PIN_RESET)
 

//液阀， 高有效
#define LIQUID_WAVE_OPEN        HAL_GPIO_WritePin(LIQUID_VALVE_GPIO_Port, LIQUID_VALVE_Pin, GPIO_PIN_SET)
#define LIQUID_WAVE_CLOSE       HAL_GPIO_WritePin(LIQUID_VALVE_GPIO_Port, LIQUID_VALVE_Pin, GPIO_PIN_RESET)
#define LIQUID_WAVE_TRIGGLE     HAL_GPIO_TogglePin(LIQUID_VALVE_GPIO_Port, LIQUID_VALVE_Pin)

//气阀，高有效
#define AIR_WAVE_OPEN           HAL_GPIO_WritePin(AIR_VALVE_GPIO_Port, AIR_VALVE_Pin, GPIO_PIN_SET)
#define AIR_WAVE_CLOSE          HAL_GPIO_WritePin(AIR_VALVE_GPIO_Port, AIR_VALVE_Pin, GPIO_PIN_RESET)
#define AIR_WAVE_TRIGGLE     	HAL_GPIO_TogglePin(AIR_VALVE_GPIO_Port, AIR_VALVE_Pin)
 

//计数池有无检测，发射灯开关控制，高有效
#define COUNT_CELL_SW_ON        HAL_GPIO_WritePin(COUNT_CELL_SW_GPIO_Port, COUNT_CELL_SW_Pin, GPIO_PIN_SET)
#define COUNT_CELL_SW_OFF       HAL_GPIO_WritePin(COUNT_CELL_SW_GPIO_Port, COUNT_CELL_SW_Pin, GPIO_PIN_RESET)
#define COUNT_CELL_SW_TRIGGLE   HAL_GPIO_TogglePin(COUNT_CELL_SW_GPIO_Port, COUNT_CELL_SW_Pin)
 
//HGB535和415波长灯切换开关，高为415有效，低为535有效
#define HGB_415_ON    			HAL_GPIO_WritePin(COUNT_CELL_TYPE_SW_GPIO_Port, COUNT_CELL_TYPE_SW_Pin, GPIO_PIN_SET)
#define HGB_535_ON			    HAL_GPIO_WritePin(COUNT_CELL_TYPE_SW_GPIO_Port, COUNT_CELL_TYPE_SW_Pin, GPIO_PIN_RESET)
 
 
//板载工装信号，选中开关 ， ON：接通板载工装信号，OFF：断开板载工装信号
#define COUNT_SIGNAL_SW_ON    HAL_GPIO_WritePin(COUNT_SIGNAL_SW_GPIO_Port, COUNT_SIGNAL_SW_Pin, GPIO_PIN_SET)
#define COUNT_SIGNAL_SW_OFF   HAL_GPIO_WritePin(COUNT_SIGNAL_SW_GPIO_Port, COUNT_SIGNAL_SW_Pin, GPIO_PIN_RESET)


//LCD背光控制，高有效
#define LCD_BL_ON                HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET)
#define LCD_BL_OFF               HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET)
  
//LCD复位，低有效，拉低10ms便可复位，当前初始化为高，后续不再复位
#define LCD_RST_ON              HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)
#define LCD_RST_OFF             HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)
  
//LCD供电控制，默认高（先数字电源，再模拟电源），低（反之）
#define LCD_BL_ON               HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET)
#define LCD_BL_OFF              HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET)
  

//SPI4 片选输出
//Flash CS
#define FLASH_SPI_CS_HIGH      HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin, GPIO_PIN_SET)
#define FLASH_SPI_CS_LOW       HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin, GPIO_PIN_RESET)

//TP CS
#define TP_SPI_CS_HIGH      HAL_GPIO_WritePin(TP_SPI_CS_GPIO_Port, TP_SPI_CS_Pin, GPIO_PIN_SET)
#define TP_SPI_CS_LOW       HAL_GPIO_WritePin(TP_SPI_CS_GPIO_Port, TP_SPI_CS_Pin, GPIO_PIN_RESET)

//TP CS
#define TP_SPI_CS_HIGH      HAL_GPIO_WritePin(TP_SPI_CS_GPIO_Port, TP_SPI_CS_Pin, GPIO_PIN_SET)
#define TP_SPI_CS_LOW       HAL_GPIO_WritePin(TP_SPI_CS_GPIO_Port, TP_SPI_CS_Pin, GPIO_PIN_RESET)

//DR0 CS (AD5162)
#define DR0_SPI_CS_HIGH      HAL_GPIO_WritePin(DR0_SPI_CS_GPIO_Port, DR0_SPI_CS_Pin, GPIO_PIN_SET)
#define DR0_SPI_CS_LOW       HAL_GPIO_WritePin(DR0_SPI_CS_GPIO_Port, DR0_SPI_CS_Pin, GPIO_PIN_RESET)

//DR1 CS (AD8402)
#define DR1_SPI_CS_HIGH      HAL_GPIO_WritePin(DR1_SPI_CS_GPIO_Port, DR1_SPI_CS_Pin, GPIO_PIN_SET)
#define DR1_SPI_CS_LOW       HAL_GPIO_WritePin(DR1_SPI_CS_GPIO_Port, DR1_SPI_CS_Pin, GPIO_PIN_RESET)


//定义CAT_PWR开关，低电平有效，开机需要拉低3~5s，这里使用了三极管，所以极性相反
#define CAT_PWR_ON							CAT_PWRKEY_GPIO_Port->BSRR |= CAT_PWRKEY_Pin
#define CAT_PWR_OFF							CAT_PWRKEY_GPIO_Port->BSRR |= CAT_PWRKEY_Pin<<16
#define CAT_PWR_TRIGGLE     				HAL_GPIO_TogglePin(CAT_PWRKEY_GPIO_Port, CAT_PWRKEY_Pin)


//定义CAT_RESET开关，低电平有效，需要拉低1s，这里使用了三极管，所以极性相反
#define CAT_RESET_ON						CAT_RESET_GPIO_Port->BSRR |= CAT_RESET_Pin
#define CAT_RESET_OFF						CAT_RESET_GPIO_Port->BSRR |= CAT_RESET_Pin<<16
#define CAT_RESET_TRIGGLE     				HAL_GPIO_TogglePin(CAT_RESET_GPIO_Port, CAT_RESET_Pin)


/*************************************************************************************/

//进出仓按键，低有效，触发外部中断，切换到电机任务， 
#define OUTIN_KEY_STATUS               HAL_GPIO_ReadPin(OUTIN_KEY_GPIO_Port, OUTIN_KEY_Pin)

//计数按键状态，低有效, 触发外部中断来切换到算法任务，低有效
#define COUNT_KEY_STATUS               HAL_GPIO_ReadPin(BC_COUNT_KEY_GPIO_Port, BC_COUNT_KEY_Pin)



//进出仓电机光耦状态，高：遮挡（灯灭），低：敞开（灯亮）
#define OUTIN_IN_OC_STATUS             HAL_GPIO_ReadPin(OUTIN_IN_OC_GPIO_Port, OUTIN_IN_OC_Pin)
#define OUTIN_OUT_OC_STATUS            HAL_GPIO_ReadPin(OUTIN_OUT_OC_GPIO_Port, OUTIN_OUT_OC_Pin)

//夹子电机光耦状态, 高：遮挡（灯灭），低：敞开（灯亮）
#define CLAMP_IN_OC_STATUS             HAL_GPIO_ReadPin(CLAMP_IN_OC_GPIO_Port, CLAMP_IN_OC_Pin)
#define CLAMP_OUT_OC_STATUS            HAL_GPIO_ReadPin(CLAMP_OUT_OC_GPIO_Port, CLAMP_OUT_OC_Pin)

//定量电极状态， 高：触发， 低：未触发
#define ELEC_STATUS                    Get_Elec_Status()    //HAL_GPIO_ReadPin(ELEC_GPIO_Port, ELEC_Pin)




//CAT模块启动是否成功状态
#define CAT_START_STATUS             	HAL_GPIO_ReadPin(CAT_STATUS_GPIO_Port, CAT_STATUS_Pin)
#define INPUT_CAT_STATUS_ACTIVE			0			//模块STATUS引脚在开机成功后输出高电平，这里使用了三极管后，则极性相反
#define INPUT_CAT_STATUS_IDLE			1


//计数池有无检测，无：1， 有：0
#define CHECK_JSC_EXIST_STATUS        	HAL_GPIO_ReadPin(COUNT_CELL_CHECK_GPIO_Port, COUNT_CELL_CHECK_Pin)
#define JSC_EXIST_STATUS_ACTIVE			0			
#define JSC_EXIST_STATUS_IDLE			1



/***
*IO输入通道枚举
***/
typedef enum _InputCH_e{
	INPUT_CH_CAT_STATUS = 0,
}InputCH_e;






/*
*   面板灯状态（表示仪器状态）
*/
typedef enum{
    PANEL_LED_STATUS_NORMAL         = 0,    //正常运行状态，仅绿灯亮
    PANEL_LED_STATUS_WARN           = 1,    //告警状态，绿灯红灯都亮
    PANEL_LED_STATUS_ERROR          = 2,    //出错状态，仅红灯亮
    PANEL_LED_STATUS_OFF            = 3,    //关闭状态
    PANEL_LED_STATUS_END            = 4,
    
} Panel_LED_Status_e;



/*
*   光耦编号
*/
typedef enum {
    OC_INDEX_OUTIN_IN        = 0,   //进出仓，进仓光耦
    OC_INDEX_OUTIN_OUT       = 1,   //进出仓，出仓光耦
    OC_INDEX_CLAMP_IN        = 2,   //夹子，   往里走光耦
    OC_INDEX_CLAMP_OUT       = 3,   //夹子，   往外走光耦
    OC_INDEX_END             = 4,
    
} OC_Index_e;
  
//获取指定光耦状态
uint8_t OC_Status(OC_Index_e eOC);  
//获取定量电极状态
uint8_t Get_Elec_Status(void);  
  
/*
*   SPI4 各个外设使能序号
*/
typedef enum {
    SPI_CS_FLASH        = 0,   //flash sp cs
    SPI_CS_TP           = 1,   //电阻触摸屏
    SPI_CS_DR0          = 2,   //数字电位器0（HGB光路）
    SPI_CS_DR1          = 3,   //数字电位器1（WBC放大电路）
    SPI_CS_END          = 4,
    
} SPI_CS_e;  


/*
*   外设或流程 运行模式
*/
typedef enum{
    RUN_MODE_NORMAL              = 0,   //正常模式 
    RUN_MODE_COUNT               = 1,   //计数模式
    RUN_MODE_END                 = 2,
    
} RunMode_e;





//
void SPI_CS_Enable(SPI_CS_e eIndex);
void SPI_CS_Disable(SPI_CS_e eIndex);
void SPI_CS_Disable_All(void);

//阀控制
void Liquid_Valve_Exec(uint32_t ulNum, uint32_t ulDelayTime);
void Air_Valve_Exec(uint32_t ulNum, uint32_t ulDelayTime);


//面板指示灯控制
void Panel_Led_Status(RunMode_e eRunMode, Panel_LED_Status_e eStatus);


//计数池是否存在
FlagStatus ConutCell_Is_Exist(void);
//    
uint8_t Input_GetState(InputCH_e InputCH);




#ifdef __cplusplus
}
#endif

#endif //__BSP_OUTIN_H_



