#include "main.h"
#include "adc.h"
#include "cmsis_os.h"
#include "bsp_adc.h"



//ADC3采样缓存  
uint16_t g_usADC3_Buffer[MONITORV_SMOOTH_NUM][MONITORV_END] = {0};

//
void ADC3_Start(void)
{
  extern ADC_HandleTypeDef hadc3;
  extern DMA_HandleTypeDef hdma_adc3;
    
  HAL_DMA_Start(&hdma_adc3, (uint32_t)&hadc3.Instance->DR, (uint32_t)&g_usADC3_Buffer[0][0], MONITORV_END*MONITORV_SMOOTH_NUM);
  
  __HAL_DMA_DISABLE_IT(&hdma_adc3, DMA_IT_TC);
  __HAL_DMA_DISABLE_IT(&hdma_adc3, DMA_IT_HT);

   hadc3.Instance->CR2 |= (uint32_t)ADC_CR2_DDS;
   hadc3.Instance->CR2 |= ADC_CR2_DMA; 
   hadc3.Instance->CR2 |= ADC_CR2_ADON;
   hadc3.Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
}



/*
*   获取ADC3指定通道的ADC值
*/
uint16_t MonitorV_ADC(MonitorV_e eV)
{
    uint8_t i = 0;
    uint32_t ulSum = 0; 
    
    if(eV >= MONITORV_END)
    {
        //LOG_Error("Param eCh(%d) Error", (int)eV);
    }
    
    //取平均值
    for(i = 0; i < MONITORV_SMOOTH_NUM; i++)
    {
        ulSum += g_usADC3_Buffer[i][eV];
    } 
    return ulSum/MONITORV_SMOOTH_NUM;
}







