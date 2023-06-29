#include "GUI.h"
//#include "bsp_touch.h"
#include "usart.h"
#include "ltdc.h"
#include "xpt2046.h"
#include "bsp_spi.h"
#include "tp.h"

/*
* ���µ��ݴ������ӿ�
*/
void TP_Scan(void)
{
	extern TouchPos_s TouchPixelPos;
	GUI_PID_STATE State;
    
    GUI_TOUCH_GetState(&State);
    
    //�����ж�
	if(State.x >= 0 && State.x <= LCD_WIDTH_PIXEL)
    {
        if(State.y >= 0 && State.y <= LCD_HIGHT_PIXEL)
        {
            //�д���
            State.Pressed = 1;
            State.Layer = 0;
            GUI_TOUCH_StoreStateEx(&State);
            return;
        }
    }
    
    //�޴���
    State.x = -1;
	State.y = -1;
    State.Pressed = 0;
    State.Layer = 0;
    GUI_TOUCH_StoreStateEx(&State);
}



///*
//* ����Ϊ������������ֲ�ӿ�
//*/
//void GUI_TOUCH_X_ActivateX(void) 
//{
// // XPT2046_WriteCMD(0x90);
//}


//void GUI_TOUCH_X_ActivateY(void)
//{
//  //XPT2046_WriteCMD(0xd0);
//}


//int  GUI_TOUCH_X_MeasureX(void) 
//{
//    //XPT2046_WriteCMD(0x90);
//   return 0;
//}

//int  GUI_TOUCH_X_MeasureY(void) 
//{	

//  //XPT2046_WriteCMD(0xd0);
//  return 0;
//}

