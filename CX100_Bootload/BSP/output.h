#ifndef _OUTPUT_H
#define _OUTPUT_H


#include "stm32f4xx.h"




/*�궨����**********************************************************************/
//������������ߵ͵�ƽ
#define LED_R_LOW							GPIOH->BSRRH |= GPIO_Pin_10				//��λ
#define LED_R_HIGH							GPIOH->BSRRL |= GPIO_Pin_10				//��λ
#define LED_R_TOGGLE						GPIOH->ODR ^= GPIO_Pin_10				//��ת�л�

#define LED_G_LOW							GPIOH->BSRRH |= GPIO_Pin_11				//��λ
#define LED_G_HIGH							GPIOH->BSRRL |= GPIO_Pin_11				//��λ
#define LED_G_TOGGLE						GPIOH->ODR ^= GPIO_Pin_11				//��ת�л�

#define LED_B_LOW							GPIOH->BSRRH |= GPIO_Pin_12				//��λ
#define LED_B_HIGH							GPIOH->BSRRL |= GPIO_Pin_12				//��λ
#define LED_B_TOGGLE						GPIOH->ODR ^= GPIO_Pin_12				//��ת�л�





void Output_Config(void);





#endif
