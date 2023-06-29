#ifndef __BSP_DREGISTER_H_
#define __BSP_DREGISTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

//HGB数字电位器默认值
#define HGB_DR_DEFAULT_VALUE    150

/*
*   数字电位器通道定义,(当前用到数字电位器，只有两个通道)
*/
typedef enum {
    DR_CHANNEL_0 = 0,
    DR_CHANNEL_1 = 1,
    DR_CHANNEL_2 = 2,
    DR_CHANNEL_3 = 3,
    
}DR_Channel_e;

//HGB增益，对应的数字电位器通道
#define HGB_DR_CHANNEL  DR_CHANNEL_0

//WBC增益，对应的数字电位器通道
#define WBC_DR_CHANNEL  DR_CHANNEL_0


/*
*   数字电位器编号
*/
typedef enum{
    DR_0 = 0,   //用于设置HGB增益，有两个通道，只用到通道0
    DR_1,       //用于设置WBC增益，有两个通道，只用到通道0
    
}DR_e;

//
void DRegister_Init(void);
void DRegister_Write(DR_e eDR, uint8_t *pucData, uint8_t ucLen);
// 设置数字电位器的值
void DRegister_SetValue(DR_e eDR, uint8_t ucChannel, uint8_t ucVal);

//设置HGB增益    
void HGB_Gran_Set(uint8_t ucChannel, uint8_t ucVal);
//设置WBC增益
void WBC_Gran_Set(uint8_t ucChannel, uint8_t ucVal);

  
  
  
  
  
  
  
  
  
  
#ifdef __cplusplus
}
#endif

#endif //__BSP_DREGISTER_H_

