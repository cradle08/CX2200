#ifndef _OUTPUT_H
#define _OUTPUT_H


#include "stm32f4xx.h"




/*宏定义区**********************************************************************/
//定义引脚输出高低电平
#define LED_R_LOW							GPIOH->BSRRH |= GPIO_Pin_10				//复位
#define LED_R_HIGH							GPIOH->BSRRL |= GPIO_Pin_10				//置位
#define LED_R_TOGGLE						GPIOH->ODR ^= GPIO_Pin_10				//翻转切换

#define LED_G_LOW							GPIOH->BSRRH |= GPIO_Pin_11				//复位
#define LED_G_HIGH							GPIOH->BSRRL |= GPIO_Pin_11				//置位
#define LED_G_TOGGLE						GPIOH->ODR ^= GPIO_Pin_11				//翻转切换

#define LED_B_LOW							GPIOH->BSRRH |= GPIO_Pin_12				//复位
#define LED_B_HIGH							GPIOH->BSRRL |= GPIO_Pin_12				//置位
#define LED_B_TOGGLE						GPIOH->ODR ^= GPIO_Pin_12				//翻转切换





void Output_Config(void);





#endif
