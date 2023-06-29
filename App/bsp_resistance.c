#include "main.h"
#include "bsp_outin.h"
#include "bsp_resistance.h"
#include "spi.h"
#include "bsp_spi.h"

/*
*   数字电位器初始化
*/
void DRegister_Init(void)
{
    //MX_SPI4_Init_LowSpeed();
    //MX_SPI4_Init();
    SPI4_ChangeModeTo(SPI_MODE_DR);//
}


/*
*   写数字电位器值, eDR：数字电位器编号， pucData：写入数据，ucLen：长度
*/
void DRegister_Write(DR_e eDR, uint8_t *pucData, uint8_t ucLen)
{    
	SPI4_ChangeModeTo(SPI_MODE_DR);//SPI4_ChangeModeToDR();
	
//    __disable_irq();
    //片选
    if(DR_0 == eDR)
    {
        SPI_CS_Enable(SPI_CS_DR0);
    }else{
        SPI_CS_Enable(SPI_CS_DR1);
    }
    
    //写数据
    if(HAL_OK != HAL_SPI_Transmit(&hspi4, pucData, ucLen, 10))
    {
        if(HAL_OK != HAL_SPI_Transmit(&hspi4, pucData, ucLen, 10))
        {
            LOG_Error("Write DR0 Fail");
        }
    }
    
    //释放片选
    if(DR_0 == eDR)
    {
        SPI_CS_Disable(SPI_CS_DR0);
    }else{
        SPI_CS_Disable(SPI_CS_DR1);
    }
//    __enable_irq();

	SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
}



//#define CLK_L   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET)
//#define CLK_H   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET)
//#define DATA_L   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET)
//#define DATA_H   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET)

/*
* 设置光路（HGB）数字电位器的值， ucChannel通道，及通道的大小(ucVal)，HGB：ucChannel=0（通道0）， ucVal(0-255 --> 0-10k)
*  当前该数字电位器有两个通道，只用到一个通道
*/
void DRegister_SetValue(DR_e eDR, uint8_t ucChannel, uint8_t ucVal)
{
    uint8_t CmdBuffer[2], i = 0;
    CmdBuffer[0] = (ucChannel & 0x01);
    CmdBuffer[1] =  ucVal;
    
    for(i = 0; i < 3; i++)
    { 
        DRegister_Write(eDR, CmdBuffer, 2);
    }

    
//    int8_t i = 0;
//    
//    __disable_irq();
//     CLK_L;
//    DATA_L;
//    SPI_CS_Enable(SPI_CS_DR0);
//    Delay_US(20);
//    CLK_H;
//    Delay_US(5);
//    CLK_L;
//    Delay_US(5);
//    for(i = 7; i >= 0; i--)
//    {
//        if(ucVal & (0x01<<i))
//        {
//            DATA_H;
//            Delay_US(5);
//            CLK_H;
//            Delay_US(5);
//        }else{
//            DATA_L;
//            Delay_US(5);
//            CLK_H;
//            Delay_US(5);
//        
//        }
//        CLK_L;
//        Delay_US(5);
//    }
//    SPI_CS_Disable(SPI_CS_DR0);
//    Delay_US(5);
//    CLK_L;
//    DATA_L;
//    __enable_irq();


}


/*
*   HGB增益设置
*   ucChannel通道，及通道的大小(ucVal)，HGB：ucChannel=0（通道0）， ucVal(0-255 --> 0-10k)
*   当前该数字电位器DR0 有两个通道，只用到一个通道
*/
void HGB_Gran_Set(uint8_t ucChannel, uint8_t ucVal)
{
    DRegister_SetValue(DR_0, ucChannel, ucVal);
}    


/*
*   WBC增益设置
*   ucChannel通道，及通道的大小(ucVal), WBC=0（通道0）， ucVal(0-255 --> 0-10k)
*   当前该数字电位器DR1 有两个通道，只用到一个通道
*/
void WBC_Gran_Set(uint8_t ucChannel, uint8_t ucVal)
{
    DRegister_SetValue(DR_1, ucChannel, ucVal);
    
}

  







