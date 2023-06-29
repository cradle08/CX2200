#ifndef __BC_H_
#define __BC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"
#include "msg_def.h"
#include "bsp_motor.h"


//num
#define  MAX_TZ_MOTOR_MOVE_NUM          3           //电机（探针对接电机）最大执行次数
#define  MAX_OUTIN_MOTOR_MOVE_NUM       2           //电机（主电机）最大执行次数
#define  MAX_PRESS_ERROR_NUM            3           //缓冲室压力最大异常次数
#define  MAX_BUILD_PRESS_NUM            3           //最大建压执行次数
#define BUILD_PERIOD_PER_TIME           500         //连续建压的间隔时间ms

#define  MAX_LIQUID_CREATE_TZ_REPEAT_NUM    3       //液路建立阶段，最大探针对接次数
#define  MAX_SIGNAL_MONITOR_TZ_REPEAT_NUM   5       //信号检测阶段，最大探针对接次数
#define  MAX_DK_REPEAT_NUM                  3       //最大排堵次数
//xk_V
#define XK_V_NORMAL_LOW_THRESHOLD        600        //790   //正常情况下，最低值
#define XK_V_BLOCK_THRESHOLD             1200       //小孔发生堵孔的，电压阈值
#define XK_V_DISCON_THRESHOLD            2200       //断开时，小孔电压阈值
#define XK_V_LIQUID_CREATE_THRESHOLD     1500       //小孔电压阈值


#define BC_SAMPLE_RATE                   1.4e6              //BC 数据采样频率，单位Hz, //ADC采样频率：MCU主频=180M: 1.5e6; MCU主频=168M168M=1.4e6; 
//wbc 正常采样超时时间（开始计数到）
#define BC_CELL_NORMAL_COUNT_TIME        12.21      //设定时间20210721    //血细胞检测模块，正常测试对应的采样计数时间(秒)，这个值根据计数池后池体积的变化而变化  !!!!!!!!
//计数池计数体积（后池体积）
#define BC_COUNT_VOLUMN                  1071.70    //830.93(过度计数池)  //新款（2020.06.01之后）计数池体积为1046.26,  826.95 ，这个值根据计数池后池体积的变化而变化  !!!!!!!!
//数据采集和计数最大时间
#define COUNT_TIMEOUT                    30000      //计数超时时间ms
//算法最大采样/计数时间
#define BC_WBC_DATA_SAMPLE_TIMEOUT		 (BC_CELL_NORMAL_COUNT_TIME*1.45*1000) //最大计数时间=正常计数时间的1.45倍(单位ms)  
//timeout
#define LIQUID_CREATE_TIMEOUT            10000  //液路建立阶段时间ms
#define SAMPLE_NOT_ENOUGH_MAX_TIME       7500   //样本量不足最大判断时间
#define SAMPLE_NOT_ENOUGH_MIN_TIME       2000   //样本量不足最小判断时间

//探针信号，信号质量判断阈值
#define SINGAL_ANALYSIS_NUM_THRESHOLD    150   
#define SINGAL_ANALYSIS_MEAN_THRESHOLD   50    
#define SINGAL_ANALYSIS_CV_THRESHOLD     350

//hgb
#define HGB_BUFFER_LEN 		             20
#define HGB_DILUTION_RATIO               501.0     //HGB稀释倍数
#define HGB_CURVE_FIT_K1                 1061.7    //HGB拟合曲线斜率1
#define HGB_CURVE_FIT_B1                 0.0       //HGB拟合曲线截距1
#define HGB_CURVE_FIT_K2                 183.37    //HGB拟合曲线斜率2
#define HGB_CURVE_FIT_B2                 0.0       //HGB拟合曲线截距2
#define HGB_BLACK_ADC_VALUE              3480.0    //HGB本底ADC值
#define HGB_ADC_BLACK_ADJUST_VALUE       3500      //HGB空白校准ADC值（比本地ADC大一点点）
//


/*
*   计数模式
*/
typedef enum{
    COUNT_MODE_NORMAL                           = 0x01,     //正常计数流程
    COUNT_MODE_RE_USE                           = 0x02,     //测试模式（定时采样，不检测定量电极触发,即：计数池重复使用模式）
    COUNT_MODE_100V_FIRE                        = 0x04,     //开启灼烧模式，当出现堵孔，开启100V灼烧 （***不再使用***）
    COUNT_MODE_SIGNAL_SRC                       = 0x05,     //信号源模式
    COUNT_MODE_SIMULATION_SIGNAL_INNER          = 0x08,     //内部工装信号仿真模拟测试, 单次，（发送命令，自动计数，带有界面交互）
    COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO     = 0x09,     //内部工装信号仿真模拟测试，多次，（发送命令，自动计数，无界面交互）
    COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE        = 0x0B,     //外部工装信号仿真模拟测试, 单次，（需手动计数，带有界面交互）
    COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO   = 0x0C,     //外部工装信号仿真模拟测试, 多次，（自动计数（第一次为手动），无界面交互）
    COUNT_MODE_HGB                              = 0x0E,     //HGB模式,其他默认为WBC_HGB
    COUNT_MODE_QC_NORMAL                        = 0x10,     //质控，正常计数流程
    COUNT_MODE_QC_INNER_AUTO                    = 0x11,     //质控，内部仿真信号自动计数流程 （***不再使用***）
    COUNT_MODE_QC_OUTSIDE                       = 0x12,     //质控，外部仿真信号计数流程，单次 （***不再使用***）
	
	COUNT_MODE_ZJ_WBC_TEST,									//质检，WBC测试
	COUNT_MODE_ZJ_HGB_KB,									//质检，HGB空白测试
	COUNT_MODE_ZJ_HGB_JMD,									//质检，HGB精密度测试
	COUNT_MODE_ZJ_HGB_JZ_1,									//质检，HGB校准1测试
	COUNT_MODE_ZJ_HGB_JZ_2,									//质检，HGB校准2测试
	COUNT_MODE_ZJ_HGB_JZ_3,									//质检，HGB校准3测试
	COUNT_MODE_ZJ_HGB_XX_1,									//质检，HGB线性1测试
	COUNT_MODE_ZJ_HGB_XX_2,									//质检，HGB线性2测试
	COUNT_MODE_ZJ_HGB_XX_3,									//质检，HGB线性3测试
	COUNT_MODE_ZJ_HGB_XX_4,									//质检，HGB线性4测试
	COUNT_MODE_ZJ_HGB_XX_5,									//质检，HGB线性5测试
	COUNT_MODE_ZJ_HGB_ZQD_1,								//质检，准确度1测试
	COUNT_MODE_ZJ_HGB_ZQD_2,								//质检，准确度2测试
	COUNT_MODE_ZJ_HGB_ZQD_3,								//质检，准确度3测试
	
	
    COUNT_MODE_INVALIDE                         = 0xFF,     //无效模式
    //
}CountMode_e;


    

//采样数据缓存
#define BC_ADC_HALF_BUF_LEN_1B        1024     //数据1024个数据点，一个数据点2个字节
#define BC_ADC_HALF_BUF_LEN_2B        512      //512个数据点
#define BC_ADC_FULL_BUF_LEN_2B        1024     //一帧2*512个Byte
#define BC_MSG_SEND_BUF_LEN_1B        1035     //一帧11个Byte(头) + 1024(体)

#define BC_MODE_DB                    0

//测试血细胞类型，数据采集，WBC， RBC_PLT
typedef enum {
  BC_WBC     = 0,
  BC_RBC_PLT = 1,
  BC_END     = 2,   //不要改变这三个的序号
    
} BC_Type_e;
#define IS_BC_TYPE(x)  (((x) == BC_WBC) | ((x) == BC_RBC_PLT))



// 测试项目 HGB， WBC，WBC_HGB,  WBC_RBC_PLT
typedef enum {
  TEST_PROJECT_HGB              = 0,
  TEST_PROJECT_WBC_HGB          = 1,
  TEST_PROJECT_RBC_PLT          = 2,
  TEST_PROJECT_WBC_HGB_RBC_PLT  = 3,
  TEST_PROJECT_END              = 4,   
    
} TestProject_e;




/*
* ADC DMA 中断触发标志
*/
typedef enum {
  BC_ADC_PART_1      = 0,      // 采样一半触发
  BC_ADC_PART_2      = 1,      // 采样完成填满 
  BC_ADC_PART_NONE   = 2,      // 无触发
  
} BC_Data_Flag_e;

/*
*血细胞采样控制结构体
*/
typedef struct {
  BC_Type_e       eType;                          //血细胞测试类型：WBC, RBC_PLT
  BC_Data_Flag_e  ucFlag;                         //ADC DMA双缓冲触发标志位, 或是半传输、传输完成标志
  uint32_t        ulGenPacketNum;                 //产生的包数
  uint32_t        ulSendPacketNum;                //发送的包数
  
#if BC_MODE_DB
  uint16_t      ucBuf[2][BC_ADC_HALF_BUF_LEN_2B];  //ADC数据缓存
#else
  uint16_t        ucBuf[BC_ADC_FULL_BUF_LEN_2B];          //ADC数据缓存, 
#endif
  
}BC_Data_t;
  



/*
* 协议消息头
*/
//typedef __packed struct {
//  uint32_t ulCmd;               //命令字
//  uint16_t usMsgLen;            //消息长度

//}Msg_Head_t;


/*
* BC消息数据结构体，11 byte
*/
typedef __packed struct {
  Msg_Head_t stHead;            //协议消息头
  BC_Type_e  eType;             //BC类型：WBC、RBC_PLT
  uint32_t   ulSendPacketNum;   //发送数据包的序列号
  //uint8_t    *pucData;          //数据指针
}Msg_BC_t;





/************************************************************************
*   样本分析过程中常量值定义
*   
*/



/*
*   计数：初始化阶段，状态信息
*/
typedef struct {
    OutIn_Module_Position_e eOutInModule_Status;    //计数仓，进出状态。
    Bool eTanzhenOC_Status;                         //探针光耦，触发状态。FALSE:未触发，TURE：已触发
    Bool eCountCell_Flag;                           //计数池检测标志。  FALSE:未放入计数池，TURE：已放入计数池
    Bool eElec_Status;                              //定量电极，触发状态。FALSE:未未触发，TURE：已触发
    uint8_t ucTZMotor_Move_Num;                     //电机（探针对接电机）执行次数
    uint8_t ucOutIn_Motor_Move_Num;                 //电机（主电机）执行次数
    uint8_t ucBuildPress_Num;                       //已执行的建压次数
    uint8_t HGB535Adc[HGB_BUFFER_LEN];           	//535波长采集数据缓存，10个ADC数据
	uint8_t HGB415Adc[HGB_BUFFER_LEN];           	//415波长采集数据缓存，10个ADC数据
    Bool    eJump_Flag;                             //FALSE: 不需要跳过液路建立阶段，TRUE:跳过液路建立阶段
    ErrorCode_e eErrorCode;                         //错误码  
    
}CountInit_t;



/*
*   计数：液路建立阶段，状态信息
*/
#define LIQUID_CREATE_XK_V_NUM              40      //每250ms采集一点，液路建立阶段10000/250=40
typedef struct {
    Bool eElec_Status;                              //定量电极，触发状态
    uint8_t ucPress_ErrNum;                         //缓冲室，已触发的异常次数
 
    uint32_t ulT2_Passed;                           //T2阶段（液路建立阶段）的已执行时间
    uint16_t usXK_V_Buffer[LIQUID_CREATE_XK_V_NUM]; //液路建立阶段过程中，保存的小孔电压
    uint16_t usXK_V_Min;                            //液路建立阶段过程中，保存的小孔电压的最小值
    uint8_t  ucReConnNum;                            //发生重对接次数
    uint8_t  ucDK_Num;                              //堵孔次数
    ErrorCode_e eErrorCode;                         //错误码 
    
}LiquidCreate_t;



/*
*   信号稳定度检测阶段, 信息状态
*/
typedef struct {
    uint8_t     ucMonitor_Num;               //已重复检测次数
    ErrorCode_e eErrorCode;                  //错误码
} SignalMonitor_t;



/*
*   计数：数据采集、分析、计算阶段，状态信息
*/
typedef struct {
    uint8_t  ucBuildPress_Num;                       //缓冲室,已执行的建压次数
    uint8_t  ucDK_Num;                               //已发生排堵次数
    
    Bool     eDataSW_Flag;                           //是否开启数据采集标志未，FALSE：未开启，TURE：已开启
    uint32_t ulT4_Passed;                            //数据采集已执行的时间
    ErrorCode_e eErrorCode;                          //错误码 
     //
} SignalAnalysis_t;



/*
*   计数：结束阶段，状态信息
*/
typedef struct {
    uint32_t eErrorCode;                          //结束阶段产生的错误码

} CountEnd_t;



/*
*   计数过程，状态信息
*/
typedef struct {
    CountInit_t         tCountInit;
    LiquidCreate_t      tLiquidCreate;
    SignalMonitor_t     tSignalMonitor;
    SignalAnalysis_t    tSignalAnalysis;
    CountEnd_t          tCountEnd; 
    uint8_t             ucUseAlgoFlag;    //调用算法标志位，调用算法，数据便不会为0， 0：未调用，1：调用
    CountLog_t      	tCountLog;
            
} CountInfo_t;



void CountInfo_Init(__IO CountInfo_t *pCountInfo);
void Printf_Free_StackSize(void);



//bc 控制结构体初始化
void BC_Init(BC_Type_e eType);
//使能血细胞ADC采样
void BC_Enable(BC_Type_e eType);
//失能血细胞ADC采样
void BC_Disable(BC_Type_e eType);
//循环采集、发送数据
void BC_Send_Data(BC_Type_e eType);

// double buffer callback function
void BC_WBC_ADC1_DMA_CpltCallback(DMA_HandleTypeDef *__hdma);
void BC_WBC_ADC1_DMA_M1CpltCallback(DMA_HandleTypeDef *__hdma);
void BC_WBC_ADC1_DMA_ErrorCallback(DMA_HandleTypeDef *__hdma);


//wbc adc callback function
void HAL_ADC_WBC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_WBC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
// rbc plt adc callback function
void HAL_ADC_RBC_PLT_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_RBC_PLT_ConvCpltCallback(ADC_HandleTypeDef* hadc);

//探针信号采集，分析
FeedBack_e Elec_Single_Analyse(uint32_t *pulNum, double *pdMean, double *pdCV);
//获取报警标志
WBCHGB_ALERT_e WBCHGB_Alert_Flag(uint8_t ucFlag, ReferGroup_e eReferGroup, WBCHGB_Rst_e eParamIndex, float fValue);
//获取质控报警标志
WBCHGB_ALERT_e QC_WBCHGB_Alert_Flag(QC_FILENUM_INDEX_e eFileNum, WBCHGB_Rst_e eParamIndex, float fValue);
//算法处理
ErrorCode_e  Algo_BC_Handler(CountMode_e eCountMode, __IO CountInfo_t *pCountInfo, __IO uint8_t *pDataBuffer, uint16_t usBufferLen);

#ifdef __cplusplus
}
#endif

#endif //__BC_H_

