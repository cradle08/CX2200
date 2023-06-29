#include "GUI.h"
#include "GUIConf.h"
#include "Public_menuDLG.h"


#if 1
void _ExecCalibration(void) {

  int xPh[5] = {0}, yPh[5] = {0};
	
  int i = 0;
  int ax_Phys[2] = {0}, ay_Phys[2] = {0};
/* calculate log. Positions */
  //int ax[2] = {XSIZE_PHYS -30, 30};
  int ax[2] = {XSIZE_PHYS - 30, 30};
//  const int ay[2] = { 15, LCD_YSIZE-1-15};
  int ay[2] = {YSIZE_PHYS-30, 30};
 // GUI_TOUCH_SetDefaultCalibration();
/* _Calibrate upper left */
  GUI_SetBkColor(GUI_RED);  
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);  GUI_FillCircle(ax[0], ay[0], 10);
  GUI_SetColor(GUI_RED);    GUI_FillCircle(ax[0], ay[0], 5);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringAt("Press here", ax[0]-55, ay[0]-30);
  // get data five times
  for(i = 0; i < 5; i++)
  {
	  do {
		GUI_PID_STATE State;
		GUI_TOUCH_GetState(&State);
		if (State.Pressed) {
		  xPh[i] = GUI_TOUCH_GetxPhys();
		  yPh[i] = GUI_TOUCH_GetyPhys();
		  break;
		}
	  } while (1);
	  GUI_Delay (100);
  }
  
/* Tell user to release */
  GUI_Clear();
  GUI_DispStringAt("OK", ax[0]-10, ay[0]-10);
  do {
    GUI_PID_STATE State;
    GUI_TOUCH_GetState(&State);
    if (State.Pressed == 0) {
      break;
    }
    GUI_Delay (100);
  } while (1);
  // get x y phys
  for(i = 0; i < 5; i++)
  {
	ax_Phys[0] += xPh[i];
	ay_Phys[0] += yPh[i];
  }
  ax_Phys[0] /= 5;
  ay_Phys[0] /= 5;
  
/* _Calibrate lower right */
  GUI_SetBkColor(GUI_RED);  
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);  GUI_FillCircle(ax[1], ay[1], 10);
  GUI_SetColor(GUI_RED);    GUI_FillCircle(ax[1], ay[1], 5);
  GUI_SetColor(GUI_WHITE);
  GUI_SetTextAlign(GUI_TA_RIGHT);
  GUI_DispStringAt("Press here", ax[1]+55, ay[1]+20);
  // get data five times
  for(i = 0; i < 5; i++)
  {
	  do {
		GUI_PID_STATE State;
		GUI_TOUCH_GetState(&State);
		if (State.Pressed) {
		  xPh[i] = GUI_TOUCH_GetxPhys();
		  yPh[i] = GUI_TOUCH_GetyPhys();
		  break;
		}
	  } while (1);
	  GUI_Delay (100);
  }
  GUI_Clear();
  GUI_DispStringAt("OK", ax[0]-10, ay[0]-10);
  do {
    GUI_PID_STATE State;
    GUI_TOUCH_GetState(&State);
    if (State.Pressed == 0) {
      break;
    }
    GUI_Delay (100);
  } while (1);
  // get x y phys
  for(i = 0; i < 5; i++)
  {
	xPh[1] += xPh[i];
	yPh[1] += yPh[i];
  }
  ax_Phys[1] /= 5;
  ay_Phys[1] /= 5;
  
//  GUI_TOUCH_Calibrate(GUI_COORD_X, ax[0], ax[1], ax_Phys[0], ax_Phys[1]);  // !!!
//  GUI_TOUCH_Calibrate(GUI_COORD_Y, ay[0], ay[1], ay_Phys[0], ay_Phys[1]);  // !!!
  { /* calculate and display values for configuration file */
    int x0, x1;
    int y0, y1;
    GUI_Clear();
    //////_Calibrate(GUI_COORD_X, ax[0], ax[1], ax_Phys[0], ax_Phys[1], &x0, &x1);
    //////_Calibrate(GUI_COORD_Y, ay[0], ay[1], ay_Phys[0], ay_Phys[1], &y0, &y1);
    GUI_DispStringAt("x0: ", 0, 0); GUI_DispDec(x0, 4); GUI_DispNextLine();
    GUI_DispString  ("x1: ");       GUI_DispDec(x1, 4); GUI_DispNextLine();
    GUI_DispString  ("y0: ");       GUI_DispDec(y0, 4); GUI_DispNextLine();
    GUI_DispString  ("y1: ");       GUI_DispDec(y1, 4); GUI_DispNextLine();
    GUI_DispString  ("Please touch display to continue...");
    GUI_Delay(1000);
//    do {
//      GUI_PID_STATE State;
//      GUI_TOUCH_GetState(&State);
//      if (State.Pressed)
//        break;
//      GUI_Delay (10);
//    } while (1);
  }
}

#else

void DrawPoint_Calibration(int sx, int sy, int *px, int*py)
{
  GUI_SetBkColor(GUI_RED);  
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);  GUI_FillCircle(sx, sy, 10);
  GUI_SetColor(GUI_RED);    GUI_FillCircle(sx, sy, 5);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringAt("Press here", sx-55, sy-30);
  do {
    GUI_PID_STATE State;
    GUI_TOUCH_GetState(&State);
    if (State.Pressed) {
      *px = GUI_TOUCH_GetxPhys();
      *py = GUI_TOUCH_GetyPhys();
      break;
    }
    GUI_Delay (100);
  } while (1);
  
  GUI_Clear();
  GUI_DispStringAt("OK", sy, sy-10);
  do {
    GUI_PID_STATE State;
    GUI_TOUCH_GetState(&State);
    if (State.Pressed == 0) {
      break;
    }
    GUI_Delay (100);
  } while (1);
}

void _ExecCalibration(void) {
	int ax_Phys[4],ay_Phys[4];
	int tem1, tem2, d1,d2;
	float fac, fac2;
/* calculate log. Positions */
	int xy[4][2] = {{20,20},{LCD_XSIZE -20, 20},{20,LCD_YSIZE-20},{LCD_XSIZE -20,LCD_YSIZE-20}};
//  const int ay[2] = { 15, LCD_YSIZE-1-15};
//  int ay[2] = {LCD_YSIZE-30, 30};
//  GUI_TOUCH_SetDefaultCalibration();
/* _Calibrate upper left */
/* Tell user to release */

	DrawPoint_Calibration(xy[0][0], xy[0][1], &ax_Phys[0], &ay_Phys[0]);
	DrawPoint_Calibration(xy[1][0], xy[1][1], &ax_Phys[1], &ay_Phys[1]);
	DrawPoint_Calibration(xy[2][0], xy[2][1], &ax_Phys[2], &ay_Phys[2]);
	DrawPoint_Calibration(xy[3][0], xy[3][1], &ax_Phys[3], &ay_Phys[3]);
  
	tem1=abs(xy[0][0]-xy[1][0]);//x1-x2
	tem2=abs(xy[0][1]-xy[1][1]);//y1-y2
	tem1*=tem1;
	tem2*=tem2;
	d1=sqrt(tem1+tem2);//得到1,2的距离

	tem1=abs(xy[2][0]-xy[3][0]);//x3-x4
	tem2=abs(xy[2][1]-xy[3][1]);//y3-y4
	tem1*=tem1;
	tem2*=tem2;
	d2=sqrt(tem1+tem2);//得到3,4的距离
	fac=(float)d1/d2;
	if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
	{
		GUI_DispStringAt("34-Calibration Failure", 50,50);
	}
	tem1=abs(xy[0][0]-xy[2][0]);//x1-x3
	tem2=abs(xy[0][1]-xy[2][1]);//y1-y3
	tem1*=tem1;
	tem2*=tem2;
	d1=sqrt(tem1+tem2);//得到1,3的距离

	tem1=abs(xy[1][0]-xy[3][0]);//x2-x4
	tem2=abs(xy[1][1]-xy[3][1]);//y2-y4
	tem1*=tem1;
	tem2*=tem2;
	d2=sqrt(tem1+tem2);//得到2,4的距离
	fac=(float)d1/d2;
	if(fac<0.95||fac>1.05)//不合格
	{
		GUI_DispStringAt("24-Calibration Failure", 50,50);
	}//正确了
				   
	//对角线相等
	tem1=abs(xy[1][0]-xy[2][0]);//x1-x3
	tem2=abs(xy[1][1]-xy[2][1]);//y1-y3
	tem1*=tem1;
	tem2*=tem2;
	d1=sqrt(tem1+tem2);//得到1,4的距离

	tem1=abs(xy[0][0]-xy[3][0]);//x2-x4
	tem2=abs(xy[0][1]-xy[3][1]);//y2-y4
	tem1*=tem1;
	tem2*=tem2;
	d2=sqrt(tem1+tem2);//得到2,3的距离
	fac=(float)d1/d2;
	if(fac<0.95||fac>1.05)//不合格
	{
		GUI_DispStringAt("13-Calibration Failure", 50,50);
	}//正确了
	//计算结果
	
	gDeviceSet.XFac=(float)(xy[3][0] - xy[0][0])/(ax_Phys[3] - ax_Phys[0]);//得到xfac		 
	gDeviceSet.XOff=xy[0][0] - gDeviceSet.XFac * ax_Phys[0];//得到xoff
		  
	gDeviceSet.YFac=(float)(xy[3][1]-xy[0][1])/(ay_Phys[3] - ay_Phys[0]);//得到yfac
	gDeviceSet.YOff= xy[0][1] - gDeviceSet.YFac* ay_Phys[0];//得到yoff  

	if(abs(gDeviceSet.XFac)>2||abs(gDeviceSet.YFac)>2)//触屏和预设的相反了.
	{
		GUI_DispStringAt("success,but turn", 50,50);
	}
	GUI_DispStringAt("Calibration Success!", 50,50);
}

#endif // AD_ALGO



