#ifndef __MENU_H__
#define __MENU_H__

#include "msg_def.h"

#define USE_EXTERN_FONT     1

//UI 数据结构体
extern __IO MachRunPara_s   MachRunPara;


void UI_Info_Init_Debug(void); 

void UI_Show(void);
void UI_Hide(void);

#endif








