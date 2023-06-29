#ifndef __BSP_MOTOR_H__
#define __BSP_MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "msg_def.h"

#define MOTOR_TEST      0


/*进出仓电机*/
//en
#define OUTIN_MOTOR_ENABLE       HAL_GPIO_WritePin(OUTIN_MOTOR_EN_GPIO_Port, OUTIN_MOTOR_EN_Pin, GPIO_PIN_RESET)
#define OUTIN_MOTOR_DISABLE      HAL_GPIO_WritePin(OUTIN_MOTOR_EN_GPIO_Port, OUTIN_MOTOR_EN_Pin, GPIO_PIN_SET)
//dir
#define OUTIN_MOTOR_DIR_OUT      HAL_GPIO_WritePin(OUTIN_MOTOR_DIR_GPIO_Port, OUTIN_MOTOR_DIR_Pin, GPIO_PIN_RESET)
#define OUTIN_MOTOR_DIR_IN       HAL_GPIO_WritePin(OUTIN_MOTOR_DIR_GPIO_Port, OUTIN_MOTOR_DIR_Pin, GPIO_PIN_SET)
//pwm
#define OUTIN_MOTOR_PWM_HIGH     HAL_GPIO_WritePin(OUTIN_MOTOR_PWM_GPIO_Port, OUTIN_MOTOR_PWM_Pin, GPIO_PIN_SET)
#define OUTIN_MOTOR_PWM_LOW      HAL_GPIO_WritePin(OUTIN_MOTOR_PWM_GPIO_Port, OUTIN_MOTOR_PWM_Pin, GPIO_PIN_RESET)

/*夹子电机仓电机*/
//en
#define CLAMP_MOTOR_ENABLE       HAL_GPIO_WritePin(CLAMP_MOTOR_EN_GPIO_Port, CLAMP_MOTOR_EN_Pin, GPIO_PIN_RESET)
#define CLAMP_MOTOR_DISABLE      HAL_GPIO_WritePin(CLAMP_MOTOR_EN_GPIO_Port, CLAMP_MOTOR_EN_Pin, GPIO_PIN_SET)
//dir
#define CLAMP_MOTOR_DIR_OUT      HAL_GPIO_WritePin(CLAMP_MOTOR_DIR_GPIO_Port, CLAMP_MOTOR_DIR_Pin, GPIO_PIN_SET)
#define CLAMP_MOTOR_DIR_IN       HAL_GPIO_WritePin(CLAMP_MOTOR_DIR_GPIO_Port, CLAMP_MOTOR_DIR_Pin, GPIO_PIN_RESET)
//pwm
#define CLAMP_MOTOR_PWM_HIGH     HAL_GPIO_WritePin(CLAMP_MOTOR_PWM_GPIO_Port, CLAMP_MOTOR_PWM_Pin, GPIO_PIN_SET)
#define CLAMP_MOTOR_PWM_LOW      HAL_GPIO_WritePin(CLAMP_MOTOR_PWM_GPIO_Port, CLAMP_MOTOR_PWM_Pin, GPIO_PIN_RESET)


// DAC for motor current set
#define MOTOR_DAC_CHANNEL             DAC_CHANNEL_2
#define MOTOR_I_MAX_VALUE             2000       //2000ma

//motor current , //1.2v -->500ma, 0.5v-->150ma
#define MOTOR_CURRENT_VALUE_START     1240  //1v
#define MOTOR_CURRENT_VALUE_RUN       800   //0.644v
#define MOTOR_CURRENT_VALUE_STOP      1240  //1v

//电机运行超时时间
#define MOTOR_MAX_TIMEOUT             10000     //10S
#define MOTOR_CLAMP_MAX_STEP          7000     //夹子电机默认执行的最大步数
#define MOTOR_OUTIN_MAX_STEP          30000     //进出仓电机默认执行的最大步数


//进出仓模块的位置
typedef enum {
    
  OUTIN_MODULE_POSITION_OUT       = 0, //出仓位置 
  OUTIN_MODULE_POSITION_IN        = 1, //进仓位置
  OUTIN_MODULE_POSITION_NUKNOWN   = 2, //未知位置
    
} OutIn_Module_Position_e;




//电机方向枚举
typedef enum {
  MOTOR_DIR_OUT = 0,
  MOTOR_DIR_IN  = 1,
  MOTOR_DIR_END = 2,
  
} MotorDir_e;

#define IS_MOTOR_DIR(x)  ((MOTOR_DIR_OUT == x) || (MOTOR_DIR_IN == x))



/****************************************************************/
//电机
typedef enum {
    MOTOR_OUTIN         = 0,    //进出仓电机
    MOTOR_CLAMP         = 1,    //探针电机
    MOTOR_END           = 2,
} Motor_e;




/*
*   电机 运行模式
*/
typedef enum{
    //正常模式（出仓：主电机出仓，探针对接电机松开。进仓：主电机进仓，探针对接电机加紧）
    MOTOR_MODE_NORMAL              = 0x01,    
    //计数模式（出仓：主电机出仓，探针对接电机松开。进仓：主电机进仓，探针对接电机松开）
    MOTOR_MODE_COUNT               = 0x02,    
//    MOTOR_MODE_WITH_BUILD_PRESS    = 0x04,    //边执行，边建压
//    MOTOR_MODE_AGING               = 0x08,    //老化模式
    MOTOR_MODE_END                 = 6,
    
} MotorMode_e;




/*
*   电机运行状态
*/
typedef enum 		  
{
    Motor_Phase_IDLE    = 0,          // idle
	Motor_Phase_LOAD    = 1,	      // load value the frist time
    Motor_Phase_ACC     = 2,          // speed up
	Motor_Phase_EQU     = 3, 	      // speed keep
    Motor_Phase_DEC     = 4,          // speed reduction
    Motor_Phase_FIN     = 5,          // finish
} MotorPhase_e;



/*
*   电机运行状态
*/
typedef struct {
	__IO MotorDir_e     eDir;    		// dir 
	__IO Bool           eFinish; 		// finish flag
	__IO uint32_t       nSteps;  		// total step
	
	__IO uint8_t        bAble;  		// ture: monitor oc change status or not
	__IO uint8_t	    nOCIndex;		// oc index(number)
	__IO uint8_t	    nOCStatus; 		// oc status

    __IO uint32_t   	nCurFreq;     	// current frequence of the motor     
    __IO uint32_t   	nTimeLoad;    	// the next value reloaded of the timer     
    
    __IO uint32_t   	nFreqMin;	   // the frequence at the begining or ending  
    __IO uint32_t   	nFreqMax;	   // the max frequence of the motor     
    __IO uint32_t   	nFreqInc;	   // the increment of frequence changing 
    __IO uint32_t   	nFreqSam;      // the width of a step when frequence changing

    __IO uint32_t   	nStepsAcc;	   // steps, phase speed up 
    __IO uint32_t   	nStepsEqu;     // steps, phase speed keep
	__IO uint32_t   	nStepsDec;	   // steps, phase speed reduction
	__IO uint32_t	 	nStepsExecuted; // steps had gone
    //IO_ uint32_t   	nStepsTotal;    // steps, total at the movement

    __IO MotorPhase_e   ePhase;         // phase

} MotorStatus_t;



/*
*   电机运行参数
*/
typedef struct {
    __IO uint32_t   nFreqMin;	   // the frequence at the begining or ending  
    __IO uint32_t   nFreqMax;	   // the max frequence of the motor     
    __IO uint32_t   nFreqInc;	   // the increment of frequence changing 
    __IO uint32_t   nFreqSam;     // the width of a step when frequence changing

} MotorPara_t;


extern __IO  MotorStatus_t  g_tMotorStatus[MOTOR_END]; // status
extern __IO  MotorPara_t    g_tMotorPara[MOTOR_END];   // paramenters


void Motor_Delay(MotorMode_e eMode, uint32_t ulTime);
//
void Motor_Set_Current(uint16_t usCurent);
void Motor_Enable(Motor_e eMotor, Bool eBool);
void Motor_Set_Dir(Motor_e eMotor, MotorDir_e eDir);
uint8_t Motor_OC_Status(Motor_e eMotor, MotorDir_e eDir);
void Motor_PWM_High(Motor_e eMotor);
void Motor_PWM_Low(Motor_e eMotor);
OutIn_Module_Position_e OutIn_Module_Positon(void);


//void OutIn_Motor_Dir(MotorDir_e eDir);
FeedBack_e Motor_Run_AddStep(MotorMode_e eMode, Motor_e eMotor, MotorDir_e eDir, uint32_t ulAddStep);
//
ErrorCode_e Motor_In(Motor_e eMotor, uint32_t ulAddStep);

ErrorCode_e OutIn_Motor_Out(MotorMode_e eMode, uint32_t ulStep, uint32_t ulAddStep);
ErrorCode_e OutIn_Motor_In(MotorMode_e eMode,  uint32_t ulStep, uint32_t ulAddStep);
ErrorCode_e OutIn_Motor_Out_Or_In(MotorMode_e eMode);

ErrorCode_e Clamp_Motor_Out(MotorMode_e eMode, uint32_t ulStep, uint32_t ulAddStep);
ErrorCode_e Clamp_Motor_In(MotorMode_e eMode,  uint32_t ulStep, uint32_t ulAddStep);
ErrorCode_e Clamp_Motor_Out_Or_In(MotorMode_e eMode);
ErrorCode_e Clamp_Motor_Out_then_In(MotorMode_e eMode);

ErrorCode_e OutIn_Module_Out(MotorMode_e eMode);
ErrorCode_e OutIn_Module_In(MotorMode_e eMode);
ErrorCode_e OutIn_Module_Out_Or_In(MotorMode_e eMode);
//
void MTx_PWM_ISR(Motor_e eMotor);
uint8_t  MTx_DriverEnable(Motor_e eMotor, Bool bAble);
//
void MTx_Timer_Init(void);
void MV_InitMotorTimer(Motor_e eMotor);
void MTx_IoMinitor(Motor_e eMotor);
void MTx_IoMinitor_Enable(Motor_e eMotor);
void MTx_IoMinitor_Disnable(Motor_e  eMotor);

// initialize the moving parameters of the motor driver
uint8_t  MV_InitPara(Motor_e eMotor, uint32_t nFreqMin, uint32_t nFreqMax, uint32_t nFreqInc, uint32_t nFreqSam);

// start a movement, according the moving parameters set before
uint8_t  MV_Move(Motor_e eMotor, uint32_t nSteps, MotorDir_e eDir);
// stop the movemnet immediately
uint8_t  MV_Stop(Motor_e eMotor);
// waiting the movement until it has finished
uint8_t  MV_Wait(Motor_e eMotor, uint32_t nTimeout, MotorDir_e bFlag);
// get the steps has executed at any time 
uint32_t MV_GetStepsExecuted(Motor_e eMotor);
// get the moving status
Bool  MV_IsFinished(Motor_e eMotor);
// handle the status of the motors
uint8_t  MV_IsrMoveHandling(void);







#ifdef CX_DEBUG
//
#endif




#ifdef __cplusplus
}
#endif

#endif //__BSP_MOTOR_H_

