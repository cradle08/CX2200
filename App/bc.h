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
#define  MAX_TZ_MOTOR_MOVE_NUM          3           //�����̽��Խӵ�������ִ�д���
#define  MAX_OUTIN_MOTOR_MOVE_NUM       2           //���������������ִ�д���
#define  MAX_PRESS_ERROR_NUM            3           //������ѹ������쳣����
#define  MAX_BUILD_PRESS_NUM            3           //���ѹִ�д���
#define BUILD_PERIOD_PER_TIME           500         //������ѹ�ļ��ʱ��ms

#define  MAX_LIQUID_CREATE_TZ_REPEAT_NUM    3       //Һ·�����׶Σ����̽��ԽӴ���
#define  MAX_SIGNAL_MONITOR_TZ_REPEAT_NUM   5       //�źż��׶Σ����̽��ԽӴ���
#define  MAX_DK_REPEAT_NUM                  3       //����Ŷ´���
//xk_V
#define XK_V_NORMAL_LOW_THRESHOLD        600        //790   //��������£����ֵ
#define XK_V_BLOCK_THRESHOLD             1200       //С�׷����¿׵ģ���ѹ��ֵ
#define XK_V_DISCON_THRESHOLD            2200       //�Ͽ�ʱ��С�׵�ѹ��ֵ
#define XK_V_LIQUID_CREATE_THRESHOLD     1500       //С�׵�ѹ��ֵ


#define BC_SAMPLE_RATE                   1.4e6              //BC ���ݲ���Ƶ�ʣ���λHz, //ADC����Ƶ�ʣ�MCU��Ƶ=180M: 1.5e6; MCU��Ƶ=168M168M=1.4e6; 
//wbc ����������ʱʱ�䣨��ʼ��������
#define BC_CELL_NORMAL_COUNT_TIME        12.21      //�趨ʱ��20210721    //Ѫϸ�����ģ�飬�������Զ�Ӧ�Ĳ�������ʱ��(��)�����ֵ���ݼ����غ������ı仯���仯  !!!!!!!!
//�����ؼ����������������
#define BC_COUNT_VOLUMN                  1071.70    //830.93(���ȼ�����)  //�¿2020.06.01֮�󣩼��������Ϊ1046.26,  826.95 �����ֵ���ݼ����غ������ı仯���仯  !!!!!!!!
//���ݲɼ��ͼ������ʱ��
#define COUNT_TIMEOUT                    30000      //������ʱʱ��ms
//�㷨������/����ʱ��
#define BC_WBC_DATA_SAMPLE_TIMEOUT		 (BC_CELL_NORMAL_COUNT_TIME*1.45*1000) //������ʱ��=��������ʱ���1.45��(��λms)  
//timeout
#define LIQUID_CREATE_TIMEOUT            10000  //Һ·�����׶�ʱ��ms
#define SAMPLE_NOT_ENOUGH_MAX_TIME       7500   //��������������ж�ʱ��
#define SAMPLE_NOT_ENOUGH_MIN_TIME       2000   //������������С�ж�ʱ��

//̽���źţ��ź������ж���ֵ
#define SINGAL_ANALYSIS_NUM_THRESHOLD    150   
#define SINGAL_ANALYSIS_MEAN_THRESHOLD   50    
#define SINGAL_ANALYSIS_CV_THRESHOLD     350

//hgb
#define HGB_BUFFER_LEN 		             20
#define HGB_DILUTION_RATIO               501.0     //HGBϡ�ͱ���
#define HGB_CURVE_FIT_K1                 1061.7    //HGB�������б��1
#define HGB_CURVE_FIT_B1                 0.0       //HGB������߽ؾ�1
#define HGB_CURVE_FIT_K2                 183.37    //HGB�������б��2
#define HGB_CURVE_FIT_B2                 0.0       //HGB������߽ؾ�2
#define HGB_BLACK_ADC_VALUE              3480.0    //HGB����ADCֵ
#define HGB_ADC_BLACK_ADJUST_VALUE       3500      //HGB�հ�У׼ADCֵ���ȱ���ADC��һ��㣩
//


/*
*   ����ģʽ
*/
typedef enum{
    COUNT_MODE_NORMAL                           = 0x01,     //������������
    COUNT_MODE_RE_USE                           = 0x02,     //����ģʽ����ʱ����������ⶨ���缫����,�����������ظ�ʹ��ģʽ��
    COUNT_MODE_100V_FIRE                        = 0x04,     //��������ģʽ�������ֶ¿ף�����100V���� ��***����ʹ��***��
    COUNT_MODE_SIGNAL_SRC                       = 0x05,     //�ź�Դģʽ
    COUNT_MODE_SIMULATION_SIGNAL_INNER          = 0x08,     //�ڲ���װ�źŷ���ģ�����, ���Σ�����������Զ����������н��潻����
    COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO     = 0x09,     //�ڲ���װ�źŷ���ģ����ԣ���Σ�����������Զ��������޽��潻����
    COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE        = 0x0B,     //�ⲿ��װ�źŷ���ģ�����, ���Σ������ֶ����������н��潻����
    COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO   = 0x0C,     //�ⲿ��װ�źŷ���ģ�����, ��Σ����Զ���������һ��Ϊ�ֶ������޽��潻����
    COUNT_MODE_HGB                              = 0x0E,     //HGBģʽ,����Ĭ��ΪWBC_HGB
    COUNT_MODE_QC_NORMAL                        = 0x10,     //�ʿأ�������������
    COUNT_MODE_QC_INNER_AUTO                    = 0x11,     //�ʿأ��ڲ������ź��Զ��������� ��***����ʹ��***��
    COUNT_MODE_QC_OUTSIDE                       = 0x12,     //�ʿأ��ⲿ�����źż������̣����� ��***����ʹ��***��
	
	COUNT_MODE_ZJ_WBC_TEST,									//�ʼ죬WBC����
	COUNT_MODE_ZJ_HGB_KB,									//�ʼ죬HGB�հײ���
	COUNT_MODE_ZJ_HGB_JMD,									//�ʼ죬HGB���ܶȲ���
	COUNT_MODE_ZJ_HGB_JZ_1,									//�ʼ죬HGBУ׼1����
	COUNT_MODE_ZJ_HGB_JZ_2,									//�ʼ죬HGBУ׼2����
	COUNT_MODE_ZJ_HGB_JZ_3,									//�ʼ죬HGBУ׼3����
	COUNT_MODE_ZJ_HGB_XX_1,									//�ʼ죬HGB����1����
	COUNT_MODE_ZJ_HGB_XX_2,									//�ʼ죬HGB����2����
	COUNT_MODE_ZJ_HGB_XX_3,									//�ʼ죬HGB����3����
	COUNT_MODE_ZJ_HGB_XX_4,									//�ʼ죬HGB����4����
	COUNT_MODE_ZJ_HGB_XX_5,									//�ʼ죬HGB����5����
	COUNT_MODE_ZJ_HGB_ZQD_1,								//�ʼ죬׼ȷ��1����
	COUNT_MODE_ZJ_HGB_ZQD_2,								//�ʼ죬׼ȷ��2����
	COUNT_MODE_ZJ_HGB_ZQD_3,								//�ʼ죬׼ȷ��3����
	
	
    COUNT_MODE_INVALIDE                         = 0xFF,     //��Чģʽ
    //
}CountMode_e;


    

//�������ݻ���
#define BC_ADC_HALF_BUF_LEN_1B        1024     //����1024�����ݵ㣬һ�����ݵ�2���ֽ�
#define BC_ADC_HALF_BUF_LEN_2B        512      //512�����ݵ�
#define BC_ADC_FULL_BUF_LEN_2B        1024     //һ֡2*512��Byte
#define BC_MSG_SEND_BUF_LEN_1B        1035     //һ֡11��Byte(ͷ) + 1024(��)

#define BC_MODE_DB                    0

//����Ѫϸ�����ͣ����ݲɼ���WBC�� RBC_PLT
typedef enum {
  BC_WBC     = 0,
  BC_RBC_PLT = 1,
  BC_END     = 2,   //��Ҫ�ı������������
    
} BC_Type_e;
#define IS_BC_TYPE(x)  (((x) == BC_WBC) | ((x) == BC_RBC_PLT))



// ������Ŀ HGB�� WBC��WBC_HGB,  WBC_RBC_PLT
typedef enum {
  TEST_PROJECT_HGB              = 0,
  TEST_PROJECT_WBC_HGB          = 1,
  TEST_PROJECT_RBC_PLT          = 2,
  TEST_PROJECT_WBC_HGB_RBC_PLT  = 3,
  TEST_PROJECT_END              = 4,   
    
} TestProject_e;




/*
* ADC DMA �жϴ�����־
*/
typedef enum {
  BC_ADC_PART_1      = 0,      // ����һ�봥��
  BC_ADC_PART_2      = 1,      // ����������� 
  BC_ADC_PART_NONE   = 2,      // �޴���
  
} BC_Data_Flag_e;

/*
*Ѫϸ���������ƽṹ��
*/
typedef struct {
  BC_Type_e       eType;                          //Ѫϸ���������ͣ�WBC, RBC_PLT
  BC_Data_Flag_e  ucFlag;                         //ADC DMA˫���崥����־λ, ���ǰ봫�䡢������ɱ�־
  uint32_t        ulGenPacketNum;                 //�����İ���
  uint32_t        ulSendPacketNum;                //���͵İ���
  
#if BC_MODE_DB
  uint16_t      ucBuf[2][BC_ADC_HALF_BUF_LEN_2B];  //ADC���ݻ���
#else
  uint16_t        ucBuf[BC_ADC_FULL_BUF_LEN_2B];          //ADC���ݻ���, 
#endif
  
}BC_Data_t;
  



/*
* Э����Ϣͷ
*/
//typedef __packed struct {
//  uint32_t ulCmd;               //������
//  uint16_t usMsgLen;            //��Ϣ����

//}Msg_Head_t;


/*
* BC��Ϣ���ݽṹ�壬11 byte
*/
typedef __packed struct {
  Msg_Head_t stHead;            //Э����Ϣͷ
  BC_Type_e  eType;             //BC���ͣ�WBC��RBC_PLT
  uint32_t   ulSendPacketNum;   //�������ݰ������к�
  //uint8_t    *pucData;          //����ָ��
}Msg_BC_t;





/************************************************************************
*   �������������г���ֵ����
*   
*/



/*
*   ��������ʼ���׶Σ�״̬��Ϣ
*/
typedef struct {
    OutIn_Module_Position_e eOutInModule_Status;    //�����֣�����״̬��
    Bool eTanzhenOC_Status;                         //̽��������״̬��FALSE:δ������TURE���Ѵ���
    Bool eCountCell_Flag;                           //�����ؼ���־��  FALSE:δ��������أ�TURE���ѷ��������
    Bool eElec_Status;                              //�����缫������״̬��FALSE:δδ������TURE���Ѵ���
    uint8_t ucTZMotor_Move_Num;                     //�����̽��Խӵ����ִ�д���
    uint8_t ucOutIn_Motor_Move_Num;                 //������������ִ�д���
    uint8_t ucBuildPress_Num;                       //��ִ�еĽ�ѹ����
    uint8_t HGB535Adc[HGB_BUFFER_LEN];           	//535�����ɼ����ݻ��棬10��ADC����
	uint8_t HGB415Adc[HGB_BUFFER_LEN];           	//415�����ɼ����ݻ��棬10��ADC����
    Bool    eJump_Flag;                             //FALSE: ����Ҫ����Һ·�����׶Σ�TRUE:����Һ·�����׶�
    ErrorCode_e eErrorCode;                         //������  
    
}CountInit_t;



/*
*   ������Һ·�����׶Σ�״̬��Ϣ
*/
#define LIQUID_CREATE_XK_V_NUM              40      //ÿ250ms�ɼ�һ�㣬Һ·�����׶�10000/250=40
typedef struct {
    Bool eElec_Status;                              //�����缫������״̬
    uint8_t ucPress_ErrNum;                         //�����ң��Ѵ������쳣����
 
    uint32_t ulT2_Passed;                           //T2�׶Σ�Һ·�����׶Σ�����ִ��ʱ��
    uint16_t usXK_V_Buffer[LIQUID_CREATE_XK_V_NUM]; //Һ·�����׶ι����У������С�׵�ѹ
    uint16_t usXK_V_Min;                            //Һ·�����׶ι����У������С�׵�ѹ����Сֵ
    uint8_t  ucReConnNum;                            //�����ضԽӴ���
    uint8_t  ucDK_Num;                              //�¿״���
    ErrorCode_e eErrorCode;                         //������ 
    
}LiquidCreate_t;



/*
*   �ź��ȶ��ȼ��׶�, ��Ϣ״̬
*/
typedef struct {
    uint8_t     ucMonitor_Num;               //���ظ�������
    ErrorCode_e eErrorCode;                  //������
} SignalMonitor_t;



/*
*   ���������ݲɼ�������������׶Σ�״̬��Ϣ
*/
typedef struct {
    uint8_t  ucBuildPress_Num;                       //������,��ִ�еĽ�ѹ����
    uint8_t  ucDK_Num;                               //�ѷ����Ŷ´���
    
    Bool     eDataSW_Flag;                           //�Ƿ������ݲɼ���־δ��FALSE��δ������TURE���ѿ���
    uint32_t ulT4_Passed;                            //���ݲɼ���ִ�е�ʱ��
    ErrorCode_e eErrorCode;                          //������ 
     //
} SignalAnalysis_t;



/*
*   �����������׶Σ�״̬��Ϣ
*/
typedef struct {
    uint32_t eErrorCode;                          //�����׶β����Ĵ�����

} CountEnd_t;



/*
*   �������̣�״̬��Ϣ
*/
typedef struct {
    CountInit_t         tCountInit;
    LiquidCreate_t      tLiquidCreate;
    SignalMonitor_t     tSignalMonitor;
    SignalAnalysis_t    tSignalAnalysis;
    CountEnd_t          tCountEnd; 
    uint8_t             ucUseAlgoFlag;    //�����㷨��־λ�������㷨�����ݱ㲻��Ϊ0�� 0��δ���ã�1������
    CountLog_t      	tCountLog;
            
} CountInfo_t;



void CountInfo_Init(__IO CountInfo_t *pCountInfo);
void Printf_Free_StackSize(void);



//bc ���ƽṹ���ʼ��
void BC_Init(BC_Type_e eType);
//ʹ��Ѫϸ��ADC����
void BC_Enable(BC_Type_e eType);
//ʧ��Ѫϸ��ADC����
void BC_Disable(BC_Type_e eType);
//ѭ���ɼ�����������
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

//̽���źŲɼ�������
FeedBack_e Elec_Single_Analyse(uint32_t *pulNum, double *pdMean, double *pdCV);
//��ȡ������־
WBCHGB_ALERT_e WBCHGB_Alert_Flag(uint8_t ucFlag, ReferGroup_e eReferGroup, WBCHGB_Rst_e eParamIndex, float fValue);
//��ȡ�ʿر�����־
WBCHGB_ALERT_e QC_WBCHGB_Alert_Flag(QC_FILENUM_INDEX_e eFileNum, WBCHGB_Rst_e eParamIndex, float fValue);
//�㷨����
ErrorCode_e  Algo_BC_Handler(CountMode_e eCountMode, __IO CountInfo_t *pCountInfo, __IO uint8_t *pDataBuffer, uint16_t usBufferLen);

#ifdef __cplusplus
}
#endif

#endif //__BC_H_

