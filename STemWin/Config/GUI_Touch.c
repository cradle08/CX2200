/**********************************************************************
STemwin触摸驱动
***********************************************************************/
#include "GUI_Touch.h"
#include "xpt2046.h"
#include "GUI.h"
#include "cmsis_os.h"
#include "msg_def.h"
#include "bsp_eeprom.h"
#include "bsp_outin.h"
#include "bsp_spi.h"

/*宏定义区*************************************************************/
 








/*全局变量定义区*******************************************************/
TouchPos_s TouchRawPos = {0};







/*外部函数区***********************************************************/
/***
*激活X轴电压，测量Y轴位置，这里每次都直接读取X和Y轴的数据出来，并加入均值算法
***/
void GUI_TOUCH_X_ActivateX(void)
{
	extern TouchPos_s TouchRawPos;
	ErrorStatus ReState;

    ReState = XPT2046_GetRawPosValue(&TouchRawPos);
    if(ReState != SUCCESS){
        TouchRawPos.x = -1;
        TouchRawPos.y = -1;
    }
    
//    if(SPI4_ChangeModeTo(SPI_MODE_TP) == SPI_MODE_TP) //if(SPI4_ChangeModeToXPT2046() == HAL_OK)
//    {
//        ReState = XPT2046_GetRawPosValue(&TouchRawPos);
//        if(ReState != SUCCESS){
//            TouchRawPos.x = -1;
//            TouchRawPos.y = -1;
//        }
//    }else{
//        TouchRawPos.x = -1;
//        TouchRawPos.y = -1;   
//    }
}



/***
*激活Y轴电压，测量X轴位置，这里每次都直接读取X和Y轴的数据出来，并加入均值算法
***/
void GUI_TOUCH_X_ActivateY(void)
{
	extern TouchPos_s TouchRawPos;
	ErrorStatus ReState;

     ReState = XPT2046_GetRawPosValue(&TouchRawPos);
     if(ReState != SUCCESS){
        TouchRawPos.x = -1;
        TouchRawPos.y = -1;
     }
     
    
     
//    if(SPI4_ChangeModeToXPT2046() == HAL_OK)
//    {
//        ReState = XPT2046_GetRawPosValue(&TouchRawPos);
//        if(ReState != SUCCESS){
//            TouchRawPos.x = -1;
//            TouchRawPos.y = -1;
//        }
//    }else{
//            TouchRawPos.x = -1;
//            TouchRawPos.y = -1;    
//    }
}



/***
*返回X轴测量到的AD值
***/
int GUI_TOUCH_X_MeasureX(void)
{
	extern TouchPos_s TouchRawPos;
	
	return TouchRawPos.x;
}



/***
*返回Y轴测量到的AD值
***/
int GUI_TOUCH_X_MeasureY(void)
{
	extern TouchPos_s TouchRawPos;
	
	return TouchRawPos.y;
}


/*
*  触摸扫描
*/
void Touch_Scan(void)
{
    PenIRQInputStateScanning();
    GUI_TOUCH_Exec();
}


/***
*3点及以上电阻屏触摸校准函数
*参数：
	SaveFlag：为0表示不需要将校准参数保存在EEPROM中，为1表示需要将校准参数保存在EEPROM中
	CalibrateFlag：为0表示不需要校准，直接进入画板模式，为1表示要先进行校准，然后才能进入到画板模式
***/
void GUI_TouchCalibrate(uint8_t SaveFlag,uint8_t CalibrateFlag)
{
	GUI_PID_STATE TouchState;
	int i;
	int xSize, ySize;
	int CheckPixelX;
	int CheckPixelY;
	HAL_StatusTypeDef Restate;
	int xPhys,yPhys;
	GUI_RECT RedoButRect;
	GUI_RECT CelButRect;
	
	int PixelX[NUM_CALIB_POINTS];
	int PixelY[NUM_CALIB_POINTS];
	int SamX[NUM_CALIB_POINTS];	
	int SamY[NUM_CALIB_POINTS];
	
	GUI_CURSOR_Select(&GUI_CursorCrossL);
	
	//获取屏幕大小
	xSize = LCD_GetXSize();
	ySize = LCD_GetYSize();
	
	//获取用于检验校准系数是否合适的坐标点
	CheckPixelX = xSize/2-50;
	CheckPixelY = ySize/2;
	
	//计算用于校准的各像素点坐标值
	MachInfo.touch.pixelX[0] = (xSize * 5) / 100;
	MachInfo.touch.pixelY[0] = (ySize * 5) / 100;
	MachInfo.touch.pixelX[1] = xSize - (xSize * 5) / 100;
	MachInfo.touch.pixelY[1] = MachInfo.touch.pixelY[0];
	MachInfo.touch.pixelX[2] = MachInfo.touch.pixelX[1];
	MachInfo.touch.pixelY[2] = ySize - (ySize * 5) / 100;
	MachInfo.touch.pixelX[3] = MachInfo.touch.pixelX[0];
	MachInfo.touch.pixelY[3] = MachInfo.touch.pixelY[2];
	MachInfo.touch.pixelX[4] = xSize / 2;
	MachInfo.touch.pixelY[4] = ySize / 2;
	
	//设置画笔大小，像素值
	GUI_SetPenSize(3);		
	
Tag_GUI_Touch_Calibrate:
	//先隐藏手标
	GUI_CURSOR_Hide();	

	//设置字体大小
	GUI_SetFont(GUI_FONT_24_1);
	
	//判断是否不需要校准
	if(CalibrateFlag == 0){
		goto GUI_Touch_CalibrateTag;
	}
	
	//获取5组触摸AD数据
	for (i = 0; i < NUM_CALIB_POINTS; i++) {
		GUI_Clear();
		GUI_DispStringHCenterAt("Please touch the point", LCD_GetXSize() / 2, LCD_GetYSize() / 2 - 60);
		GUI_DrawCircle(MachInfo.touch.pixelX[i], MachInfo.touch.pixelY[i], 5);
		
		while(PenIRQ_GetState() == PENIRQ_ACTIVE){
            Touch_Scan(); 
            osDelay(3);
        }
		osDelay(1000);
        
		while(PenIRQ_GetState() != PENIRQ_ACTIVE){
            Touch_Scan(); 
            osDelay(3);
        }
		osDelay(1000);
		
		//保存采样的AD值
	   MachInfo.touch.samX[i] = GUI_TOUCH_GetxPhys();
	   MachInfo.touch.samY[i] = GUI_TOUCH_GetyPhys();
	}
	
	
	
	for(i=0;i<NUM_CALIB_POINTS;i++){
		PixelX[i] = MachInfo.touch.pixelX[i];
	}
	for(i=0;i<NUM_CALIB_POINTS;i++){
		PixelY[i] = MachInfo.touch.pixelY[i];
	}
	for(i=0;i<NUM_CALIB_POINTS;i++){
		SamX[i] = MachInfo.touch.samX[i];
	}
	for(i=0;i<NUM_CALIB_POINTS;i++){
		SamY[i] = MachInfo.touch.samY[i];
	}
	
	//校准参数
	GUI_TOUCH_CalcCoefficients(NUM_CALIB_POINTS, PixelX, PixelY,SamX, SamY, xSize, ySize);
	
	//判断校准系数是否合适
	GUI_Clear();
	GUI_DispStringHCenterAt("Please checkout the para", LCD_GetXSize() / 2, LCD_GetYSize() / 2-60);
	GUI_DrawCircle(CheckPixelX, CheckPixelY, 5);
	
	//等待触摸
	while(PenIRQ_GetState() == PENIRQ_ACTIVE){ 
        Touch_Scan(); 
        osDelay(3); 
    }
	osDelay(1000);
	while(PenIRQ_GetState() != PENIRQ_ACTIVE){ 
        Touch_Scan(); 
       osDelay(3); 
    }
	osDelay(1000);
		
	//获取像素X坐标值
	GUI_TOUCH_GetState(&TouchState);
	
	//校验像素坐标X是否正确
	if(CheckPixelX+5<TouchState.x || CheckPixelX-5>TouchState.x){
		GUI_Clear();
		GUI_SetColor(GUI_RED);
		GUI_DispStringHCenterAt("GUI_TouchCalibrate failed! ReCalibrate", LCD_GetXSize() / 2, LCD_GetYSize() / 2 - 60);
		osDelay(2000);
		
		goto Tag_GUI_Touch_Calibrate;
	}
	
	//校验像素坐标Y是否正确
	if(CheckPixelY+5<TouchState.y || CheckPixelY-5>TouchState.y){
		GUI_Clear();
		GUI_SetColor(GUI_RED);
		GUI_DispStringHCenterAt("GUI_TouchCalibrate failed! ReCalibrate", LCD_GetXSize() / 2, LCD_GetYSize() / 2 - 60);
		osDelay(2000);
		
		goto Tag_GUI_Touch_Calibrate;
	}
	
	GUI_Clear();
	GUI_SetColor(GUI_RED);
	GUI_DispStringHCenterAt("GUI_TouchCalibrate success!", LCD_GetXSize() / 2, LCD_GetYSize() / 2 - 60);
	osDelay(2000);
	
	MachInfo.touch.flag = TOUCH_INIT_FLAG;
	
	if(SaveFlag == 1){
		//保留校准参数值，写入EEPROM中
		Restate = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_MACHINFO_INIT_TOUCH,(uint8_t*)&MachInfo.touch,sizeof(MachInfo.touch));
		if(Restate != HAL_OK){
			printf("参数写入EEPROM中失败，即将重写一次\r\n");
			Restate = EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_MACHINFO_INIT_TOUCH,(uint8_t*)&MachInfo.touch,sizeof(MachInfo.touch));
			if(Restate != HAL_OK){
				printf("参数写入EEPROM中失败两次\r\n");
			}
		}
	}
	
GUI_Touch_CalibrateTag:
	//显示手标
	GUI_CURSOR_Show();
	
	GUI_Clear();
	
	//绘制退出按键
	CelButRect.x0 = 660;
	CelButRect.y0 = 420;
	CelButRect.x1 = 780;
	CelButRect.y1 = 460;
	GUI_SetColor(GUI_BLUE);
	GUI_AA_FillRoundedRectEx(&CelButRect,20);
	GUI_SetBkColor(GUI_BLUE);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringInRect("EXIT",&CelButRect,GUI_TA_HCENTER|GUI_TA_VCENTER);
	
	//绘制重校准按键
	RedoButRect.x0 = 520;
	RedoButRect.y0 = 420;
	RedoButRect.x1 = 640;
	RedoButRect.y1 = 460;
	GUI_SetColor(GUI_BLUE);
	GUI_AA_FillRoundedRectEx(&RedoButRect,20);
	GUI_SetBkColor(GUI_BLUE);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringInRect("AGAIN",&RedoButRect,GUI_TA_HCENTER|GUI_TA_VCENTER);
	
	GUI_SetFont(GUI_FONT_6X8);
	GUI_SetBkColor(GUI_WHITE);
	
	//校准完毕后显示触摸界面以及画板
	while(1){
        Touch_Scan();
		//获取像素坐标
		GUI_TOUCH_GetState(&TouchState);  
		
		//获取原始AD值
		xPhys = GUI_TOUCH_GetxPhys();     
		yPhys = GUI_TOUCH_GetyPhys();     
		
		//显示原始AD值
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt("Analog input:\n", 0, 20);
		GUI_GotoY(GUI_GetDispPosY() + 2);
		GUI_DispString("x:");
		GUI_DispDec(xPhys, 4);
		GUI_DispString(", y:");
		GUI_DispDec(yPhys, 4);
		
		//显示像素坐标值
		GUI_SetColor(GUI_RED);
		GUI_GotoY(GUI_GetDispPosY() + 4);
		GUI_DispString("\nPosition:\n");
		GUI_GotoY(GUI_GetDispPosY() + 2);
		GUI_DispString("x:");
		GUI_DispDec(TouchState.x,4);
		GUI_DispString(", y:");
		GUI_DispDec(TouchState.y,4);
		GUI_DispString("\nPressure:");
		GUI_DispDec(TouchState.Pressed,4);
		
		//判断是否在退出按钮上区域
		if((TouchState.x>CelButRect.x0 && TouchState.x<CelButRect.x1) && (TouchState.y>CelButRect.y0 && TouchState.y<CelButRect.y1)){
			//退出
			GUI_Clear();
			break;
		}
		
		//判断是否在重校准按钮上区域
		if((TouchState.x>RedoButRect.x0 && TouchState.x<RedoButRect.x1) && (TouchState.y>RedoButRect.y0 && TouchState.y<RedoButRect.y1)){
			GUI_Clear();
			CalibrateFlag = 1;
			goto Tag_GUI_Touch_Calibrate;
		}
		
		//画轨迹点
		GUI_SetColor(GUI_BLACK);
		GUI_DrawPoint(TouchState.x,TouchState.y);
		
		GUI_Delay(pdMS_TO_TICKS(20));
	}
}






/*
*   触摸校准
*/
void Touch_Adjust(void)
{
    /* 设置背景颜色 */
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();

    //开启背光
    LCD_BL_ON;

    /* 开启光标 */
    GUI_CURSOR_Show();
    GUI_CURSOR_Select(&GUI_CursorCrossL);

    GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(GUI_BLACK);
    GUI_Clear();


    //3点及以上校准
    GUI_TouchCalibrate(1,1);

    //显示手标
    GUI_CURSOR_Select(&GUI_CursorArrowSI);
    GUI_CURSOR_SetPosition(200, 200);
    GUI_CURSOR_Show();

    GUI_SetColor(GUI_BLACK);
    GUI_Clear();
}







///***
//*触摸校准，3点法+1点检验
//PixelX = A1*TX + B1*TY + C1
//PixelY = A2*TX + B2*TY + C2
//***/
//void Touch_Adjust(void)
//{
//    extern MachInfo_s MachInfo;
//	//extern TouchAdjustPara_s MachInfo.touch;  
//	TouchPos_s TouchRawPos[3];
//	TouchPos_s TouchPixelPos = {0};
//	ErrorStatus ReturnState = SUCCESS;
//	uint8_t OnceFlag = 0;
////	uint32_t Temp = 0;
//	long K = 0;
//    char buffer[30] = {0};
//	
//Touch_Adjust_Tag:
//	//清除屏幕
//    GUI_SetColor(GUI_WHITE);
//	GUI_Clear();
//    
//	GUI_SetColor(GUI_RED);
//	GUI_DispStringAt("Ready to adjust screen!", 100, 100);
//	
//	//更新下触摸中输入IO状态，防止误判
//	GUI_Delay(2000);
//	
//	//清屏
//    GUI_SetColor(GUI_BLACK);
//	GUI_Clear();
//	GUI_Delay (5);
//    
//	//画第一个十字校准点
//    GUI_SetColor(GUI_RED);
//	GUI_DrawLine(TOUCH_ADJUST_PIXEL_X_0-20,TOUCH_ADJUST_PIXEL_Y_0,TOUCH_ADJUST_PIXEL_X_0+20,TOUCH_ADJUST_PIXEL_Y_0);
//	GUI_DrawLine(TOUCH_ADJUST_PIXEL_X_0,TOUCH_ADJUST_PIXEL_Y_0-20,TOUCH_ADJUST_PIXEL_X_0,TOUCH_ADJUST_PIXEL_Y_0+20);
//	GUI_Exec();
//    
//	//等待触摸
//	while(1){
//		PenIRQInputStateScanning();
//		
//		//触摸按下
//		if(PenIRQ_GetState() == PENIRQ_ACTIVE){
//			if(OnceFlag == 0){
//				ReturnState = XPT2046_GetRawPosValue(&TouchRawPos[0]);
//				if(ReturnState == SUCCESS){
//					OnceFlag = 1;
//				}
//			}
//		}
//		
//		//触摸松开
//		if(PenIRQ_GetState() == PENIRQ_IDLE){
//			if(OnceFlag == 1){
//				OnceFlag = 0;
//				
//				break;
//			}
//		}
//        //GUI_Delay (2);
//	}
//    //显示坐标信息
//    LOG_Info("Dot_0, ADC X=%d, Y=%d", TouchRawPos[0].x, TouchRawPos[0].y);
//    memset(buffer, 0, sizeof(buffer));
//    sprintf(buffer, "%d, %d", TouchRawPos[0].x, TouchRawPos[0].y);
//    GUI_DispStringAt(buffer, TOUCH_ADJUST_PIXEL_X_0, TOUCH_ADJUST_PIXEL_Y_0+30);
//    GUI_Delay(1000);   
//    //清屏
//    //GUI_SetColor(GUI_BLACK);
//	//GUI_Clear();
//    
//	
//	//画第二个十字校准点
//    GUI_SetColor(GUI_RED);
//	GUI_DrawLine(TOUCH_ADJUST_PIXEL_X_1-20,TOUCH_ADJUST_PIXEL_Y_1,TOUCH_ADJUST_PIXEL_X_1+20,TOUCH_ADJUST_PIXEL_Y_1);
//	GUI_DrawLine(TOUCH_ADJUST_PIXEL_X_1,TOUCH_ADJUST_PIXEL_Y_1-20,TOUCH_ADJUST_PIXEL_X_1,TOUCH_ADJUST_PIXEL_Y_1+20);
//	GUI_Exec();
//    
//	//等待触摸
//	while(1){
//		PenIRQInputStateScanning();
//		
//		//触摸按下
//		if(PenIRQ_GetState() == PENIRQ_ACTIVE){
//			if(OnceFlag == 0){
//				ReturnState = XPT2046_GetRawPosValue(&TouchRawPos[1]);
//				if(ReturnState == SUCCESS){
//					OnceFlag = 1;
//				}
//			}
//		}
//		
//		//触摸松开
//		if(PenIRQ_GetState() == PENIRQ_IDLE){
//			if(OnceFlag == 1){
//				OnceFlag = 0;
//				
//				break;
//			}
//		}
//        //GUI_Delay (2);
//	}
//	

//   //显示坐标信息
//    LOG_Info("Dot_1, ADC X=%d, Y=%d", TouchRawPos[1].x, TouchRawPos[1].y);
//    memset(buffer, 0, sizeof(buffer));
//    sprintf(buffer, "%d, %d", TouchRawPos[1].x, TouchRawPos[1].y);
//    GUI_DispStringAt(buffer, TOUCH_ADJUST_PIXEL_X_1-50, TOUCH_ADJUST_PIXEL_Y_1+30);
//	GUI_Delay(1000);
//	//清屏
////  GUI_SetColor(GUI_BLACK);
////	GUI_Clear();    
//    
//    
//	//画第三个十字校准点
//    GUI_SetColor(GUI_RED);
//	GUI_DrawLine(TOUCH_ADJUST_PIXEL_X_2-20,TOUCH_ADJUST_PIXEL_Y_2,TOUCH_ADJUST_PIXEL_X_2+20,TOUCH_ADJUST_PIXEL_Y_2);
//	GUI_DrawLine(TOUCH_ADJUST_PIXEL_X_2,TOUCH_ADJUST_PIXEL_Y_2-20,TOUCH_ADJUST_PIXEL_X_2,TOUCH_ADJUST_PIXEL_Y_2+20);
//	GUI_Exec();
//    
//	//等待触摸
//	while(1){
//		PenIRQInputStateScanning();
//		
//		//触摸按下
//		if(PenIRQ_GetState() == PENIRQ_ACTIVE){
//			if(OnceFlag == 0){
//				ReturnState = XPT2046_GetRawPosValue(&TouchRawPos[2]);
//				if(ReturnState == SUCCESS){
//					OnceFlag = 1;
//				}
//			}
//		}
//		
//		//触摸松开
//		if(PenIRQ_GetState() == PENIRQ_IDLE){
//			if(OnceFlag == 1){
//				OnceFlag = 0;
//				
//				break;
//			}
//		}
//        //GUI_Delay (5);
//	}
//   //显示坐标信息
//    LOG_Info("Dot_2, ADC X=%d, Y=%d", TouchRawPos[2].x, TouchRawPos[2].y);
//    memset(buffer, 0, sizeof(buffer));
//    sprintf(buffer, "%d, %d", TouchRawPos[2].x, TouchRawPos[2].y);
//    GUI_DispStringAt(buffer, TOUCH_ADJUST_PIXEL_X_2, TOUCH_ADJUST_PIXEL_Y_2+30);
//	GUI_Delay(1500);
//    
//	//计算参数
//	K = (TouchRawPos[0].x - TouchRawPos[2].x)*(TouchRawPos[1].y - TouchRawPos[2].y) - (TouchRawPos[1].x - TouchRawPos[2].x)*(TouchRawPos[0].y - TouchRawPos[2].y);
//	MachInfo.tTouchAdjustPara.a1 = ((TOUCH_ADJUST_PIXEL_X_0 - TOUCH_ADJUST_PIXEL_X_2)*(TouchRawPos[1].y - TouchRawPos[2].y) - (TOUCH_ADJUST_PIXEL_X_1 - TOUCH_ADJUST_PIXEL_X_2)*(TouchRawPos[0].y - TouchRawPos[2].y))*1.0/K;
//	MachInfo.tTouchAdjustPara.b1 = ((TouchRawPos[0].x - TouchRawPos[2].x)*(TOUCH_ADJUST_PIXEL_X_1 - TOUCH_ADJUST_PIXEL_X_2) - (TOUCH_ADJUST_PIXEL_X_0 - TOUCH_ADJUST_PIXEL_X_2)*(TouchRawPos[1].x - TouchRawPos[2].x))*1.0/K;
//	MachInfo.tTouchAdjustPara.c1 = (TouchRawPos[0].y*(TouchRawPos[2].x*TOUCH_ADJUST_PIXEL_X_1-TouchRawPos[1].x*TOUCH_ADJUST_PIXEL_X_2) + TouchRawPos[1].y*(TouchRawPos[0].x*TOUCH_ADJUST_PIXEL_X_2-TouchRawPos[2].x*TOUCH_ADJUST_PIXEL_X_0) +
//							TouchRawPos[2].y*(TouchRawPos[1].x*TOUCH_ADJUST_PIXEL_X_0-TouchRawPos[0].x*TOUCH_ADJUST_PIXEL_X_1))*1.0/K;
//	
//	MachInfo.tTouchAdjustPara.a2 = ((TOUCH_ADJUST_PIXEL_Y_0 - TOUCH_ADJUST_PIXEL_Y_2)*(TouchRawPos[1].y - TouchRawPos[2].y) - (TOUCH_ADJUST_PIXEL_Y_1 - TOUCH_ADJUST_PIXEL_Y_2)*(TouchRawPos[0].y - TouchRawPos[2].y))*1.0/K;
//	MachInfo.tTouchAdjustPara.b2 = ((TOUCH_ADJUST_PIXEL_Y_1 - TOUCH_ADJUST_PIXEL_Y_2)*(TouchRawPos[0].x - TouchRawPos[2].x) - (TOUCH_ADJUST_PIXEL_Y_0 - TOUCH_ADJUST_PIXEL_Y_2)*(TouchRawPos[1].x - TouchRawPos[2].x))*1.0/K;
//	MachInfo.tTouchAdjustPara.c2 = (TouchRawPos[0].y*(TouchRawPos[2].x*TOUCH_ADJUST_PIXEL_Y_1-TouchRawPos[1].x*TOUCH_ADJUST_PIXEL_Y_2) + TouchRawPos[1].y*(TouchRawPos[0].x*TOUCH_ADJUST_PIXEL_Y_2-TouchRawPos[2].x*TOUCH_ADJUST_PIXEL_Y_0) +
//							TouchRawPos[2].y*(TouchRawPos[1].x*TOUCH_ADJUST_PIXEL_Y_0-TouchRawPos[0].x*TOUCH_ADJUST_PIXEL_Y_1))*1.0/K;
//	
//	//
//    Print_TouchAdjustParam(&MachInfo.tTouchAdjustPara);
//	//生成第四点，校验参数是否合适
//	//清屏
//    //GUI_SetColor(GUI_BLACK);
//	//GUI_Clear();
//    
//	
//	//画第四个十字校准点
//    GUI_SetColor(GUI_RED);
//	GUI_DrawLine(TOUCH_ADJUST_PIXEL_X_3-20,TOUCH_ADJUST_PIXEL_Y_3,TOUCH_ADJUST_PIXEL_X_3+20,TOUCH_ADJUST_PIXEL_Y_3);
//	GUI_DrawLine(TOUCH_ADJUST_PIXEL_X_3,TOUCH_ADJUST_PIXEL_Y_3-20,TOUCH_ADJUST_PIXEL_X_3,TOUCH_ADJUST_PIXEL_Y_3+20);
//	GUI_Delay(1500);
//    
//	//等待触摸
//	while(1){
//		PenIRQInputStateScanning();
//		
//		//触摸按下
//		if(PenIRQ_GetState() == PENIRQ_ACTIVE){
//			if(OnceFlag == 0){
//				ReturnState = Touch_GetPixelPosValue(&TouchPixelPos);
//				if(ReturnState == SUCCESS){
//					OnceFlag = 1;
//				}
//			}
//		}
//		
//		//触摸松开
//		if(PenIRQ_GetState() == PENIRQ_IDLE){
//			if(OnceFlag == 1){
//				OnceFlag = 0;
//				
//				break;
//			}
//		}
//        //GUI_Delay (2);
//	}
//    //显示坐标信息
//    LOG_Info("Dot_3, Axis X=%d, Y=%d", TouchPixelPos.x, TouchPixelPos.y);
//    memset(buffer, 0, sizeof(buffer));
//    sprintf(buffer, "Axis:%d,%d", TouchPixelPos.x, TouchPixelPos.y);
//    GUI_DispStringAt(buffer, TOUCH_ADJUST_PIXEL_X_3, TOUCH_ADJUST_PIXEL_Y_3+30);
//    memset(buffer, 0, sizeof(buffer));
//    sprintf(buffer, "Real Axis:%d,%d", TOUCH_ADJUST_PIXEL_X_3, TOUCH_ADJUST_PIXEL_Y_3);
//    GUI_DispStringAt(buffer, TOUCH_ADJUST_PIXEL_X_3, TOUCH_ADJUST_PIXEL_Y_3+60);
//	GUI_Delay(5000);
//    
//    
//	//检验参数是否合适
//	if(((TouchPixelPos.x <= TOUCH_ADJUST_PIXEL_X_3 + 20) && (TouchPixelPos.x >= TOUCH_ADJUST_PIXEL_X_3 - 20)) && ((TouchPixelPos.y <= TOUCH_ADJUST_PIXEL_Y_3 + 20) && (TouchPixelPos.y >= TOUCH_ADJUST_PIXEL_Y_3 - 20))){
//		//参数校验成功
////		GUI_SetColor(GUI_BLACK);
////        GUI_Clear();

//        GUI_SetColor(GUI_RED);
//		GUI_DispStringAt("Adjust screen successfully!", 100, 120);
//		GUI_Delay(2000);
//    
//        //
//        GUI_SetColor(GUI_BLACK);
//		GUI_Clear();
//	}else{
//		//参数校验失败，重新校验
//		GUI_SetColor(GUI_BLACK);
//        GUI_Clear();
//	
//        GUI_SetColor(GUI_RED);
//		GUI_DispStringAt("Adjust screen Error!", 100, 140);
//		GUI_Delay(2000);
//        
//        GUI_SetColor(GUI_BLACK);
//		GUI_Clear();
//		
//		goto Touch_Adjust_Tag;
//	}
//}







