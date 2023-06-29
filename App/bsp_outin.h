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

//nand  ���С�æµ״̬���
#define NAND_READY_BUSY_Port     GPIOD
#define NAND_READY_BUSY_Pin      GPIO_PIN_6

//��壬�̵ƿ���,����Ч
#define MENU_GREEN_LED_ON        HAL_GPIO_WritePin(MENU_GREEN_GPIO_Port, MENU_GREEN_Pin, GPIO_PIN_SET)
#define MENU_GREEN_LED_OFF       HAL_GPIO_WritePin(MENU_GREEN_GPIO_Port, MENU_GREEN_Pin, GPIO_PIN_RESET)
#define MENU_GREEN_LED_TRIGGLE   HAL_GPIO_TogglePin(MENU_GREEN_GPIO_Port, MENU_GREEN_Pin)

//��壬��ƿ���, ����Ч
#define MENU_RED_LED_ON         HAL_GPIO_WritePin(MENU_RED_GPIO_Port, MENU_RED_Pin, GPIO_PIN_SET)
#define MENU_RED_LED_OFF        HAL_GPIO_WritePin(MENU_RED_GPIO_Port, MENU_RED_Pin, GPIO_PIN_RESET)
#define MENU_RED_LED_TRIGGLE    HAL_GPIO_TogglePin(MENU_RED_GPIO_Port, MENU_RED_Pin)

 //����LEDָʾ�ƣ� ����Ч
#define LED0_ON                 HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET)
#define LED0_OFF                HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET)
#define LED0_TRIGGLE            HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin)


//BC ����Դ��56V�����أ� ����Ч��Ĭ�ϴ��ڹر�״̬��ֻ����Ҫ���56V��ѹ��С�׵�ѹ��ص�ʱ��ſ������������꼰ʱ�ر�
#define BC_CUR_SW_ON           HAL_GPIO_WritePin(BC_CURRENT_SW_GPIO_Port, BC_CURRENT_SW_Pin, GPIO_PIN_SET)
#define BC_CUR_SW_OFF          HAL_GPIO_WritePin(BC_CURRENT_SW_GPIO_Port, BC_CURRENT_SW_Pin, GPIO_PIN_RESET)
//Nand flashд������Ĭ������OFF
#define NAND_WP_OFF            HAL_GPIO_WritePin(NAND_WP_GPIO_Port, NAND_WP_Pin, GPIO_PIN_SET)
#define NAND_WP_ON             HAL_GPIO_WritePin(NAND_WP_GPIO_Port, NAND_WP_Pin, GPIO_PIN_RESET)


//��·����Դ�����ؿ��ƣ�����Ч
#define LIGHT_SW_ON             HAL_GPIO_WritePin(LIGHT_SW_GPIO_Port, LIGHT_SW_Pin, GPIO_PIN_SET)
#define LIGHT_SW_OFF            HAL_GPIO_WritePin(LIGHT_SW_GPIO_Port, LIGHT_SW_Pin, GPIO_PIN_RESET)
 

//Һ���� ����Ч
#define LIQUID_WAVE_OPEN        HAL_GPIO_WritePin(LIQUID_VALVE_GPIO_Port, LIQUID_VALVE_Pin, GPIO_PIN_SET)
#define LIQUID_WAVE_CLOSE       HAL_GPIO_WritePin(LIQUID_VALVE_GPIO_Port, LIQUID_VALVE_Pin, GPIO_PIN_RESET)
#define LIQUID_WAVE_TRIGGLE     HAL_GPIO_TogglePin(LIQUID_VALVE_GPIO_Port, LIQUID_VALVE_Pin)

//����������Ч
#define AIR_WAVE_OPEN           HAL_GPIO_WritePin(AIR_VALVE_GPIO_Port, AIR_VALVE_Pin, GPIO_PIN_SET)
#define AIR_WAVE_CLOSE          HAL_GPIO_WritePin(AIR_VALVE_GPIO_Port, AIR_VALVE_Pin, GPIO_PIN_RESET)
#define AIR_WAVE_TRIGGLE     	HAL_GPIO_TogglePin(AIR_VALVE_GPIO_Port, AIR_VALVE_Pin)
 

//���������޼�⣬����ƿ��ؿ��ƣ�����Ч
#define COUNT_CELL_SW_ON        HAL_GPIO_WritePin(COUNT_CELL_SW_GPIO_Port, COUNT_CELL_SW_Pin, GPIO_PIN_SET)
#define COUNT_CELL_SW_OFF       HAL_GPIO_WritePin(COUNT_CELL_SW_GPIO_Port, COUNT_CELL_SW_Pin, GPIO_PIN_RESET)
#define COUNT_CELL_SW_TRIGGLE   HAL_GPIO_TogglePin(COUNT_CELL_SW_GPIO_Port, COUNT_CELL_SW_Pin)
 
//HGB535��415�������л����أ���Ϊ415��Ч����Ϊ535��Ч
#define HGB_415_ON    			HAL_GPIO_WritePin(COUNT_CELL_TYPE_SW_GPIO_Port, COUNT_CELL_TYPE_SW_Pin, GPIO_PIN_SET)
#define HGB_535_ON			    HAL_GPIO_WritePin(COUNT_CELL_TYPE_SW_GPIO_Port, COUNT_CELL_TYPE_SW_Pin, GPIO_PIN_RESET)
 
 
//���ع�װ�źţ�ѡ�п��� �� ON����ͨ���ع�װ�źţ�OFF���Ͽ����ع�װ�ź�
#define COUNT_SIGNAL_SW_ON    HAL_GPIO_WritePin(COUNT_SIGNAL_SW_GPIO_Port, COUNT_SIGNAL_SW_Pin, GPIO_PIN_SET)
#define COUNT_SIGNAL_SW_OFF   HAL_GPIO_WritePin(COUNT_SIGNAL_SW_GPIO_Port, COUNT_SIGNAL_SW_Pin, GPIO_PIN_RESET)


//LCD������ƣ�����Ч
#define LCD_BL_ON                HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET)
#define LCD_BL_OFF               HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET)
  
//LCD��λ������Ч������10ms��ɸ�λ����ǰ��ʼ��Ϊ�ߣ��������ٸ�λ
#define LCD_RST_ON              HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)
#define LCD_RST_OFF             HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)
  
//LCD������ƣ�Ĭ�ϸߣ������ֵ�Դ����ģ���Դ�����ͣ���֮��
#define LCD_BL_ON               HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET)
#define LCD_BL_OFF              HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET)
  

//SPI4 Ƭѡ���
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


//����CAT_PWR���أ��͵�ƽ��Ч��������Ҫ����3~5s������ʹ���������ܣ����Լ����෴
#define CAT_PWR_ON							CAT_PWRKEY_GPIO_Port->BSRR |= CAT_PWRKEY_Pin
#define CAT_PWR_OFF							CAT_PWRKEY_GPIO_Port->BSRR |= CAT_PWRKEY_Pin<<16
#define CAT_PWR_TRIGGLE     				HAL_GPIO_TogglePin(CAT_PWRKEY_GPIO_Port, CAT_PWRKEY_Pin)


//����CAT_RESET���أ��͵�ƽ��Ч����Ҫ����1s������ʹ���������ܣ����Լ����෴
#define CAT_RESET_ON						CAT_RESET_GPIO_Port->BSRR |= CAT_RESET_Pin
#define CAT_RESET_OFF						CAT_RESET_GPIO_Port->BSRR |= CAT_RESET_Pin<<16
#define CAT_RESET_TRIGGLE     				HAL_GPIO_TogglePin(CAT_RESET_GPIO_Port, CAT_RESET_Pin)


/*************************************************************************************/

//�����ְ���������Ч�������ⲿ�жϣ��л���������� 
#define OUTIN_KEY_STATUS               HAL_GPIO_ReadPin(OUTIN_KEY_GPIO_Port, OUTIN_KEY_Pin)

//��������״̬������Ч, �����ⲿ�ж����л����㷨���񣬵���Ч
#define COUNT_KEY_STATUS               HAL_GPIO_ReadPin(BC_COUNT_KEY_GPIO_Port, BC_COUNT_KEY_Pin)



//�����ֵ������״̬���ߣ��ڵ������𣩣��ͣ�������������
#define OUTIN_IN_OC_STATUS             HAL_GPIO_ReadPin(OUTIN_IN_OC_GPIO_Port, OUTIN_IN_OC_Pin)
#define OUTIN_OUT_OC_STATUS            HAL_GPIO_ReadPin(OUTIN_OUT_OC_GPIO_Port, OUTIN_OUT_OC_Pin)

//���ӵ������״̬, �ߣ��ڵ������𣩣��ͣ�������������
#define CLAMP_IN_OC_STATUS             HAL_GPIO_ReadPin(CLAMP_IN_OC_GPIO_Port, CLAMP_IN_OC_Pin)
#define CLAMP_OUT_OC_STATUS            HAL_GPIO_ReadPin(CLAMP_OUT_OC_GPIO_Port, CLAMP_OUT_OC_Pin)

//�����缫״̬�� �ߣ������� �ͣ�δ����
#define ELEC_STATUS                    Get_Elec_Status()    //HAL_GPIO_ReadPin(ELEC_GPIO_Port, ELEC_Pin)




//CATģ�������Ƿ�ɹ�״̬
#define CAT_START_STATUS             	HAL_GPIO_ReadPin(CAT_STATUS_GPIO_Port, CAT_STATUS_Pin)
#define INPUT_CAT_STATUS_ACTIVE			0			//ģ��STATUS�����ڿ����ɹ�������ߵ�ƽ������ʹ���������ܺ������෴
#define INPUT_CAT_STATUS_IDLE			1


//���������޼�⣬�ޣ�1�� �У�0
#define CHECK_JSC_EXIST_STATUS        	HAL_GPIO_ReadPin(COUNT_CELL_CHECK_GPIO_Port, COUNT_CELL_CHECK_Pin)
#define JSC_EXIST_STATUS_ACTIVE			0			
#define JSC_EXIST_STATUS_IDLE			1



/***
*IO����ͨ��ö��
***/
typedef enum _InputCH_e{
	INPUT_CH_CAT_STATUS = 0,
}InputCH_e;






/*
*   ����״̬����ʾ����״̬��
*/
typedef enum{
    PANEL_LED_STATUS_NORMAL         = 0,    //��������״̬�����̵���
    PANEL_LED_STATUS_WARN           = 1,    //�澯״̬���̵ƺ�ƶ���
    PANEL_LED_STATUS_ERROR          = 2,    //����״̬���������
    PANEL_LED_STATUS_OFF            = 3,    //�ر�״̬
    PANEL_LED_STATUS_END            = 4,
    
} Panel_LED_Status_e;



/*
*   ������
*/
typedef enum {
    OC_INDEX_OUTIN_IN        = 0,   //�����֣����ֹ���
    OC_INDEX_OUTIN_OUT       = 1,   //�����֣����ֹ���
    OC_INDEX_CLAMP_IN        = 2,   //���ӣ�   �����߹���
    OC_INDEX_CLAMP_OUT       = 3,   //���ӣ�   �����߹���
    OC_INDEX_END             = 4,
    
} OC_Index_e;
  
//��ȡָ������״̬
uint8_t OC_Status(OC_Index_e eOC);  
//��ȡ�����缫״̬
uint8_t Get_Elec_Status(void);  
  
/*
*   SPI4 ��������ʹ�����
*/
typedef enum {
    SPI_CS_FLASH        = 0,   //flash sp cs
    SPI_CS_TP           = 1,   //���败����
    SPI_CS_DR0          = 2,   //���ֵ�λ��0��HGB��·��
    SPI_CS_DR1          = 3,   //���ֵ�λ��1��WBC�Ŵ��·��
    SPI_CS_END          = 4,
    
} SPI_CS_e;  


/*
*   ��������� ����ģʽ
*/
typedef enum{
    RUN_MODE_NORMAL              = 0,   //����ģʽ 
    RUN_MODE_COUNT               = 1,   //����ģʽ
    RUN_MODE_END                 = 2,
    
} RunMode_e;





//
void SPI_CS_Enable(SPI_CS_e eIndex);
void SPI_CS_Disable(SPI_CS_e eIndex);
void SPI_CS_Disable_All(void);

//������
void Liquid_Valve_Exec(uint32_t ulNum, uint32_t ulDelayTime);
void Air_Valve_Exec(uint32_t ulNum, uint32_t ulDelayTime);


//���ָʾ�ƿ���
void Panel_Led_Status(RunMode_e eRunMode, Panel_LED_Status_e eStatus);


//�������Ƿ����
FlagStatus ConutCell_Is_Exist(void);
//    
uint8_t Input_GetState(InputCH_e InputCH);




#ifdef __cplusplus
}
#endif

#endif //__BSP_OUTIN_H_



