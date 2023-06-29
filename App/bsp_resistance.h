#ifndef __BSP_DREGISTER_H_
#define __BSP_DREGISTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

//HGB���ֵ�λ��Ĭ��ֵ
#define HGB_DR_DEFAULT_VALUE    150

/*
*   ���ֵ�λ��ͨ������,(��ǰ�õ����ֵ�λ����ֻ������ͨ��)
*/
typedef enum {
    DR_CHANNEL_0 = 0,
    DR_CHANNEL_1 = 1,
    DR_CHANNEL_2 = 2,
    DR_CHANNEL_3 = 3,
    
}DR_Channel_e;

//HGB���棬��Ӧ�����ֵ�λ��ͨ��
#define HGB_DR_CHANNEL  DR_CHANNEL_0

//WBC���棬��Ӧ�����ֵ�λ��ͨ��
#define WBC_DR_CHANNEL  DR_CHANNEL_0


/*
*   ���ֵ�λ�����
*/
typedef enum{
    DR_0 = 0,   //��������HGB���棬������ͨ����ֻ�õ�ͨ��0
    DR_1,       //��������WBC���棬������ͨ����ֻ�õ�ͨ��0
    
}DR_e;

//
void DRegister_Init(void);
void DRegister_Write(DR_e eDR, uint8_t *pucData, uint8_t ucLen);
// �������ֵ�λ����ֵ
void DRegister_SetValue(DR_e eDR, uint8_t ucChannel, uint8_t ucVal);

//����HGB����    
void HGB_Gran_Set(uint8_t ucChannel, uint8_t ucVal);
//����WBC����
void WBC_Gran_Set(uint8_t ucChannel, uint8_t ucVal);

  
  
  
  
  
  
  
  
  
  
#ifdef __cplusplus
}
#endif

#endif //__BSP_DREGISTER_H_

