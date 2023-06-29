#include "main.h"
#include "bsp_outin.h"
#include "bsp_resistance.h"
#include "spi.h"
#include "bsp_spi.h"

/*
*   ���ֵ�λ����ʼ��
*/
void DRegister_Init(void)
{
    //MX_SPI4_Init_LowSpeed();
    //MX_SPI4_Init();
    SPI4_ChangeModeTo(SPI_MODE_DR);//
}


/*
*   д���ֵ�λ��ֵ, eDR�����ֵ�λ����ţ� pucData��д�����ݣ�ucLen������
*/
void DRegister_Write(DR_e eDR, uint8_t *pucData, uint8_t ucLen)
{    
	SPI4_ChangeModeTo(SPI_MODE_DR);//SPI4_ChangeModeToDR();
	
//    __disable_irq();
    //Ƭѡ
    if(DR_0 == eDR)
    {
        SPI_CS_Enable(SPI_CS_DR0);
    }else{
        SPI_CS_Enable(SPI_CS_DR1);
    }
    
    //д����
    if(HAL_OK != HAL_SPI_Transmit(&hspi4, pucData, ucLen, 10))
    {
        if(HAL_OK != HAL_SPI_Transmit(&hspi4, pucData, ucLen, 10))
        {
            LOG_Error("Write DR0 Fail");
        }
    }
    
    //�ͷ�Ƭѡ
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
* ���ù�·��HGB�����ֵ�λ����ֵ�� ucChannelͨ������ͨ���Ĵ�С(ucVal)��HGB��ucChannel=0��ͨ��0���� ucVal(0-255 --> 0-10k)
*  ��ǰ�����ֵ�λ��������ͨ����ֻ�õ�һ��ͨ��
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
*   HGB��������
*   ucChannelͨ������ͨ���Ĵ�С(ucVal)��HGB��ucChannel=0��ͨ��0���� ucVal(0-255 --> 0-10k)
*   ��ǰ�����ֵ�λ��DR0 ������ͨ����ֻ�õ�һ��ͨ��
*/
void HGB_Gran_Set(uint8_t ucChannel, uint8_t ucVal)
{
    DRegister_SetValue(DR_0, ucChannel, ucVal);
}    


/*
*   WBC��������
*   ucChannelͨ������ͨ���Ĵ�С(ucVal), WBC=0��ͨ��0���� ucVal(0-255 --> 0-10k)
*   ��ǰ�����ֵ�λ��DR1 ������ͨ����ֻ�õ�һ��ͨ��
*/
void WBC_Gran_Set(uint8_t ucChannel, uint8_t ucVal)
{
    DRegister_SetValue(DR_1, ucChannel, ucVal);
    
}

  







