#include "bc.h"
#include "main.h"
#include "adc.h"
#include "cmsis_os2.h" 
#include <string.h>
#include "usart.h"
#include "bsp_outin.h"
#include "cx_menu.h"
#include "bsp_motor.h"
 
//algo
#include "alg.h"
#include "algstring.h"
#include "algpulse.h"
#include "abcompute.h"
#include "algwbc.h"
#include "algflag.h"
#include "algdata.h"
#include "modedefs.h"
#include "algplusdefs.h"

//
#include "msg_def.h"
#include "backend_msg.h"
#include "monitorV.h"
#include "process.h"
#include "bsp_outin.h"
#include "bsp_press.h"
#include "bsp_light.h"
#include "paraflagdefs.h"
#include "file_operate.h"
#include "Common.h"
#include "HGB_Deal.h"




//Ѫϸ�����ݽṹ��
__IO BC_Data_t   g_stBC_Data[BC_END] = {0};


/*
*ADC���������жϴ���
*/
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(ADC1 == hadc->Instance)
    {
        // ADC1 WBC 
        g_stBC_Data[BC_WBC].ulGenPacketNum++;
        g_stBC_Data[BC_WBC].ucFlag = BC_ADC_PART_1;
    }else if(ADC2 == hadc->Instance){
        
        //ADC2 RBC PLT
        g_stBC_Data[BC_RBC_PLT].ulGenPacketNum++;
        g_stBC_Data[BC_RBC_PLT].ucFlag = BC_ADC_PART_1;        
    }
}

/*
*ADC��������жϴ���
*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(ADC1 == hadc->Instance)
    {
        // ADC1 WBC 
        g_stBC_Data[BC_WBC].ulGenPacketNum++;
        g_stBC_Data[BC_WBC].ucFlag = BC_ADC_PART_2;
    }else if(ADC2 == hadc->Instance){
        
        //ADC2 RBC PLT
        g_stBC_Data[BC_RBC_PLT].ulGenPacketNum++;
        g_stBC_Data[BC_RBC_PLT].ucFlag = BC_ADC_PART_2;        
    }
}





/*
* bc ���ƽṹ���ʼ��
*/
void BC_Init(BC_Type_e eType)
{
    //�����ж�
    if(IS_BC_TYPE(eType) == RESET) 
    {
    //@todo error
    }
    
    //��ʼ���ṹ��
    memset((void*)&g_stBC_Data, 0, sizeof(g_stBC_Data));
    g_stBC_Data[eType].eType = eType;
    g_stBC_Data[eType].ucFlag = BC_ADC_PART_NONE;           
}


/*
*ʹ��Ѫϸ��ADC����
*/
void BC_Enable(BC_Type_e eType)
{
  extern ADC_HandleTypeDef hadc1;
  extern DMA_HandleTypeDef hdma_adc1;
  extern ADC_HandleTypeDef hadc2;
  extern DMA_HandleTypeDef hdma_adc2;
  
  //�����ж�
  if(IS_BC_TYPE(eType) == RESET) 
  {
    //@todo error
  }
  
  if(BC_WBC == eType) //��ϸ��
  {
    
#if BC_MODE_DB 
     hadc1.Instance->CR2 |= ADC_CR2_DMA;
     hadc1.Instance->CR2 |= (uint32_t)ADC_CR2_ADON;
     hadc1.Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
    //HAL_ADC_Start(&hadc1);
#else   
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&g_stBC_Data[BC_WBC].ucBuf[0], BC_ADC_FULL_BUF_LEN_2B);
    hadc1.Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
#endif
    
  }else if(BC_RBC_PLT == eType){ 
    // RBC PLT
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&g_stBC_Data[BC_RBC_PLT].ucBuf[0], BC_ADC_FULL_BUF_LEN_2B);
    hadc2.Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
  }
}



/*
*ʧ��Ѫϸ��ADC����
*/
void BC_Disable(BC_Type_e eType)
{
  //�����ж�
  if(IS_BC_TYPE(eType) == RESET) 
  {
    //@todo 
  }
  
  //ʧ��ADC����
  if(BC_WBC == eType) 
  {
      //WBC
#if BC_MODE_DB
    //__HAL_DMA_DISABLE();
    //HAL_ADC_Stop(&hadc1);
    hadc1.Instance->CR2 |= (uint32_t)~ADC_CR2_ADON;
#else
    HAL_ADC_Stop_DMA(&hadc1);
#endif
  }else if(BC_RBC_PLT == eType){
    // RBC PLT
    HAL_ADC_Stop_DMA(&hadc2);
  }
}



/*
*   ��������Ϣ��ʼ��
*/
void CountInfo_Init(__IO CountInfo_t *pCountInfo)
{
    //��ʼ���׶Σ���Ϣ��ʼ��
    pCountInfo->tCountInit.eOutInModule_Status       = OUTIN_MODULE_POSITION_NUKNOWN;   
    pCountInfo->tCountInit.eTanzhenOC_Status         = FALSE;                //̽��������״̬��FALSE:δ������TURE���Ѵ���
    pCountInfo->tCountInit.eCountCell_Flag           = FALSE;                //�����ؼ���־��  FALSE:δ��������أ�TURE���ѷ��������
    pCountInfo->tCountInit.eElec_Status              = FALSE;                //�����缫��δ����״̬��FALSE:δδ������TURE���Ѵ���  
    pCountInfo->tCountInit.ucTZMotor_Move_Num        = 0;                    //�����̽��Խӵ����ִ�д���
    pCountInfo->tCountInit.ucOutIn_Motor_Move_Num    = 0;                    //������������ִ�д��� 
    pCountInfo->tCountInit.ucBuildPress_Num          = 0;                    //��ִ�еĽ�ѹ���� 
//    memset((void*)pCountInfo->tCountInit.ucHGB_Buffer, 0,  HGB_BUFFER_LEN);  //HGB�ɼ����ݻ��棬10��ADC����
    pCountInfo->tCountInit.eJump_Flag                = FALSE;                //FALSE: ����Ҫ����Һ·�����׶Σ�TRUE:����Һ·�����׶�
    pCountInfo->tCountInit.eErrorCode                = ERROR_CODE_SUCCESS;   //������
    
    //Һ·�����׶�
    pCountInfo->tLiquidCreate.eElec_Status              = FALSE;    //�����缫��δ����״̬
    pCountInfo->tLiquidCreate.ucPress_ErrNum            = 0;        //�����ң��Ѵ������쳣����   
    pCountInfo->tLiquidCreate.ulT2_Passed               = 0;        //T2�׶Σ�Һ·�����׶Σ�����ִ��ʱ��
    memset((void*)pCountInfo->tLiquidCreate.usXK_V_Buffer, 0, LIQUID_CREATE_XK_V_NUM);
    pCountInfo->tLiquidCreate.usXK_V_Min                = 0xFFFF;   //С�׵�ѹ��Сֵ
    pCountInfo->tLiquidCreate.ucDK_Num                  = 0;        //�����¿׵Ĵ���
    pCountInfo->tLiquidCreate.eErrorCode                = ERROR_CODE_SUCCESS;
       
    //�ź��ȶ��ȼ��׶�
    pCountInfo->tSignalMonitor.ucMonitor_Num            = 0;                     //���ظ�������//
    pCountInfo->tSignalMonitor.eErrorCode               = ERROR_CODE_SUCCESS;    //������

    //�źŲɼ��������׶�
    pCountInfo->tSignalAnalysis.eDataSW_Flag            = FALSE;   //�Ƿ������ݲɼ���־δ��FALSE��δ������TURE���ѿ���
    pCountInfo->tSignalAnalysis.ucBuildPress_Num        = 0;       //������,��ִ�еĽ�ѹ����
    pCountInfo->tSignalAnalysis.ucDK_Num                = 0;       //�ѷ����Ŷ´���
    pCountInfo->tSignalAnalysis.ulT4_Passed             = 0;       //�źŷ����׶Σ���ʱ
    pCountInfo->tSignalAnalysis.eErrorCode              = ERROR_CODE_SUCCESS;    //������
       
    //�����׶�
    pCountInfo->tCountEnd.eErrorCode                    = ERROR_CODE_SUCCESS;    //������
    
    pCountInfo->tCountLog.usLogLen                      = 0;
    pCountInfo->tCountLog.usCrc                         = 0;
}






/*
*  ̽���źŲɼ�������
*/
FeedBack_e Elec_Single_Analyse(uint32_t *pulNum, double *pdMean, double *pdCV)
{  	
    pulse_mid_feature midFeature = {0};	
	double64 smpRate = BC_SAMPLE_RATE;   //���ݲ���Ƶ�ʣ���λHz
    pulse_info_i_t *pulseInfo;  //������Ϣbuffer
    uint32_t pulseNum = 0;      //���β����������
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    
	int RawChangeBuf[512] = {0};
	uint16 i;
	uint32_t Count = 0;
		
	double CurrentMean = 0;
	double MeanSum = 0;
	double MeanAvg = 0;
		
	double CurrentCV = 0;
	double CVSum = 0;
	double CVAvg = 0;

    uint32_t ulStartTick = 0, ulEndTick = 0;

  
      //�㷨�汾����������
//    LOG_Info("alg_version = %s\t",version());
//    LOG_Info("alg_release_Date = %s\n",versionDate());
 
	//������ʼ��
    pulseMidFeatureInit(&midFeature);	
	midFeature.smpRate = smpRate;      
        
    //�����㷨�ڴ�ռ�
	pulseInfo = (pulse_info_i_t*)SDRAM_ALGO_ADDR;
    memset((void*)pulseInfo, 0, SDRAM_ALGO_LEN);
    
    //��ʱ��ʼ��
    ulStartTick = HAL_GetTick();
    ulEndTick   = ulStartTick;
    //�رյ�����
    osKernelLock();
    //�ɼ�����
    while(ulEndTick - ulStartTick < 1500) //1500ms
    {
        //ʹ��Ѫϸ���źŲɼ�
        BC_Enable(BC_WBC);
        
        //�ȴ��ɼ���1024�ֽ�����
        while(1){
            if(BC_ADC_PART_1 == g_stBC_Data[BC_WBC].ucFlag){
                //�رղɼ�
                BC_Disable(BC_WBC);
                break;
            }
        }	
        Count++;
        //��λ��־
        g_stBC_Data[BC_WBC].ucFlag = BC_ADC_PART_NONE;
        
        //�����������
        wbcPulseIdentify(pulseInfo, &pulseNum, (const uint8 *)&g_stBC_Data[BC_WBC].ucBuf[0], DATA_UINT_SIZE, &midFeature);
        
        //16Ϊ������չ��32����
        for(i=0;i<512;i++){
            RawChangeBuf[i] = g_stBC_Data[BC_WBC].ucBuf[i];
        }
        
        //����mean
        CurrentMean = getMean_i(RawChangeBuf,512);
        MeanSum += CurrentMean;
        
        //����CV
        CurrentCV = getCv_i(RawChangeBuf,512,0);
        CVSum += CurrentCV;
        
        //
        ulEndTick = HAL_GetTick();
	}
		
    //�ָ�������
    osKernelUnlock();
    
    //����ƽ��MEAN
    MeanAvg = MeanSum/Count;
    //����ƽ��CV
    CVAvg = CVSum/Count*100;
    
    //�ж��Ƿ�����쳣
    *pulNum = pulseNum; *pdMean = MeanAvg; *pdCV = CVAvg;
    //if(pulseNum > SINGAL_ANALYSIS_NUM_THRESHOLD || MeanAvg > SINGAL_ANALYSIS_MEAN_THRESHOLD || CVAvg > SINGAL_ANALYSIS_CV_THRESHOLD){
    if(pulseNum > SINGAL_ANALYSIS_NUM_THRESHOLD){
        eFeedBack = FEED_BACK_FAIL; //̽��Ӵ��쳣
    }
    
    return eFeedBack;
}



/*
* ��ȡ������־����Ϣ�� ����/����ֵ�Ż����ƫ��/ƫС�жϣ���Чֵ��*�� ucFlag��Ϊ�㷨������־
*/
WBCHGB_ALERT_e WBCHGB_Alert_Flag(uint8_t ucFlag, ReferGroup_e eReferGroup, WBCHGB_Rst_e eParamIndex, float fValue)
{
    extern MachInfo_s MachInfo;
    uint32_t ulValue = 0;
    uint8_t ucFlagL = 0, ucFlagH = 0;
  
    
    //��* ���ж�
    if(ucFlag == PF_INVALID) //��Чֵ����*
    {
        return WBCHGB_ALERT_INVALID;
    }else if(ucFlag == PF_NORMAL){  //����ֵ
    
        ucFlagH = 0x00; 
    }else if(ucFlag == PF_REVIEW){  //����ֵ������
        ucFlagH = 0x10;
    }
    
    //�����жϣ������Ϳ���ֵ���Ż����ƫ�ߡ���ƫ�͡��ж�
    ulValue = (uint32_t)(fValue*100);
    switch(eParamIndex)
    {
        case WBCHGB_RST_WBC:
        {
            if(ulValue > MachInfo.other.refGroups[eReferGroup].usWBCH)
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.other.refGroups[eReferGroup].usWBCL){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
         case WBCHGB_RST_GranJ:
        {
            if(ulValue > MachInfo.other.refGroups[eReferGroup].usGranH)
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.other.refGroups[eReferGroup].usGranL){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_MidJ:
        {
            if(ulValue > MachInfo.other.refGroups[eReferGroup].usMidH)
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.other.refGroups[eReferGroup].usMidL){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_LymJ:
        {
            if(ulValue > MachInfo.other.refGroups[eReferGroup].usLymH)
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.other.refGroups[eReferGroup].usLymL){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_GranB:
        {
            if(ulValue > MachInfo.other.refGroups[eReferGroup].usGranPercentH)
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.other.refGroups[eReferGroup].usGranPercentL){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_MidB:
        {
            if(ulValue > MachInfo.other.refGroups[eReferGroup].usMidPercentH)
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.other.refGroups[eReferGroup].usMidPercentL){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_LymB:
        {
            if(ulValue > MachInfo.other.refGroups[eReferGroup].usLymPercentH)
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.other.refGroups[eReferGroup].usLymPercentL){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_HGB:
        {
            if(ulValue > MachInfo.other.refGroups[eReferGroup].usHGBH)
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.other.refGroups[eReferGroup].usHGBL){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;      
        case WBCHGB_RST_END:
        {
        
        }
        break;           
    }
    //
    return (ucFlagH | ucFlagL);
}


/*
*   ��ȡ�ʿر�����־�������ʿأ�ֻ����ʾƫ�����ƫС��
*/
WBCHGB_ALERT_e QC_WBCHGB_Alert_Flag(QC_FILENUM_INDEX_e eFileNumIndex, WBCHGB_Rst_e eParamIndex, float fValue)
{
    extern MachInfo_s MachInfo;  
    float ulValue = 0; //uint32_t ulValue = 0;
    uint8_t ucFlagL = 0, ucFlagH = 0;
    
    //�����жϣ������Ϳ���ֵ���Ż����ƫ�ߡ���ƫ�͡��ж�   
    ulValue = fValue;//ulValue = (uint32_t)(fValue*100);
    switch(eParamIndex)
    {
        case WBCHGB_RST_WBC:
        {
            if(ulValue > MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[eParamIndex])
            {
                ucFlagL = 0x02; //ƫ���
            }else if(ulValue < MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[eParamIndex]){
                ucFlagL = 0x01; //ƫС��
            }else{
                ucFlagL = 0x00; //����
            }
        }
        break;
         case WBCHGB_RST_GranJ:
        {
            if(ulValue > MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[eParamIndex])
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[eParamIndex]){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_MidJ:
        {
            if(ulValue > MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[eParamIndex])
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[eParamIndex]){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_LymJ:
        {
            if(ulValue > MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[eParamIndex])
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[eParamIndex]){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_GranB:
        {
            if(ulValue > MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[eParamIndex])
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[eParamIndex]){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_MidB:
        {
            if(ulValue > MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[eParamIndex])
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[eParamIndex]){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_LymB:
        {
            if(ulValue > MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[eParamIndex])
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[eParamIndex]){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;
        case WBCHGB_RST_HGB:
        {
            if(ulValue > MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[eParamIndex])
            {
                ucFlagL = 0x02;
            }else if(ulValue < MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[eParamIndex]){
                ucFlagL = 0x01;
            }else{
                ucFlagL = 0x00;
            }
        }
        break;      
        case WBCHGB_RST_END:
        {
        
        }
        break;           
    }
    
    return (ucFlagH | ucFlagL);
}


    



/*
*  WBC�㷨���㴦��
*/
#define PROMPT_INFO_LEN		30
#define COUNT_MAX_XK_V_NUM  ((uint32_t)(COUNT_TIMEOUT*2/1000))   //һ��2��С�׵�ѹ���ݣ���COUNT_TIMEOUT��ʱ�����ĸ���

ErrorCode_e  Algo_BC_Handler(CountMode_e eCountMode, __IO CountInfo_t *pCountInfo, __IO uint8_t *pDataBuffer, uint16_t usBufferLen)
{
	extern  MachInfo_s MachInfo;
	extern __IO DataManage_t g_tDataManage;
	extern DMA_HandleTypeDef hdma_adc1;
	ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
	enum emErrorMode eToAlgoErrorCode = EM_SUCCESS;
	enum emFunctionMode eFunctionMode = FM_SAMPLE;
	__IO uint32_t  ulTemp = 0, ulTemp2 = 0, ulQuery = 0, ulCountMaxTime = 0, usersMode = 0;

	__IO uint32_t ulT4_Start =0, ulT4_End = 0, ulTicks = 0, ulStartTick = 0, ulEndTick = 0;
	uint16_t i = 0, ulIndex = 0, usXK_V = 0;
	char ucaPromptInfo[30] = {0};
	char ucaTimeBuffer[20] = {0};
	uint16_t usaXK_V_Buffer[COUNT_MAX_XK_V_NUM]  = {0}; //�������̱���С�׵�ѹ��Ϣ
	uint8_t ucXK_V_Index                         = 0;   //��ǰ�ѻ���С�׵�ѹ�ĸ���
	uint8_t *pWBCPrompt = NULL, *pHGBPrompt = NULL, ucFlag = 0, ucLanguage = 1; //0:Ӣ�ģ�1������
	uint8_t Elec_Status = RESET;
	int32_t lPress = 0, lStartPress = 0, lEndPress = 0;

	// ��������������
	int lossSum = 0, isPulseValid = 0;

	//�㷨��������
	// �����б�
	nParaList modeParas;	// ģʽ����
	dParaList systemParas;	// ϵͳ����
	dParaList configParas;	// ���ò��� *
	dParaList reportParas;	// �������
	dParaList featureParas;	// �����������м���Ϣ���м�״̬��
	dParaList flagParas;	// ��������
	histInforList histList;	// ֱ��ͼ��Ϣ//WBC/RBC/PLT

	//�ɼ����ݴ�С��Byte
	int dataSize = 0, C_Num = 0;

	// �м�״̬��Ϣ��ʼ��
	pulse_info_i_t *wbcPulseInfo;  //������Ϣbuffer
	uint32 wbcPulseNum = 0;        //�������
	double64 smpRate = BC_SAMPLE_RATE;      //MCU��Ƶ180M=1.5e6; 168M=1.5e6;   //���ݲ���Ƶ�ʣ���λHz
	pulse_mid_feature midFeature = {0};		
	BOOL8 status;       //�㷨����״̬
	int nModePara = 0;      //����ģʽ
	double countTime = 0;   //����ʱ��
	double volumnTime = 0;  //��Һʱ��
	double hgbDataCV = 0, ratioHgbAve = 0;

	int stampNum = 0;
	int timeStamps[MAXDURATION] = {0};
	int timeStampFlags[MAXDURATION] = {0};

	/*****************�㷨�������****************************/
	alg_data_in dataIn = {0};
	alg_data_out dataOut = {0};

	// ������
	histInfor *wbcDispHist = NULL;

	/******************�㷨��ʼ��***********************/
	// ��ʼ��
	memset(&dataIn, 0, sizeof(alg_data_in)); //***
	memset(&dataOut, 0, sizeof(alg_data_out)); //***
	initParaListNP(&modeParas);
	initParaListDP(&systemParas);
	initParaListDP(&reportParas);
	initParaListDP(&configParas);	//***
	initParaListDP(&featureParas);
	initParaListDP(&flagParas);
	initHistList(&histList);

	setModePara(&modeParas, "AnalysisMode",		AM_WBC_HGB); //WBC HGB ģʽ
	setModePara(&modeParas, "SamplingVialMode",	SVM_OPEN);
	setModePara(&modeParas, "SpecimenMode",		SM_PVB);
	setModePara(&modeParas, "SpeciesMode",		SPM_HUMAN);
	setModePara(&modeParas, "AlgMode",			AM_RELEASE); //AM_DEBUG
	setModePara(&modeParas, "RefGroupMode",		RGM_GENERAL);	 // �ο���ģʽ , �����û���ѡ������ ������������

	//���ݵ�ǰ�˺����ͣ������㷨���˺�ģʽ
	if(ACCOUNT_TYPE_NORMAL == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type || ACCOUNT_TYPE_NEW == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type)
	{
		// 001
		usersMode = UM_NORMAL;
	}else if(ACCOUNT_TYPE_ADMIN == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type){
		//admin
		usersMode = UM_ADMIN;
	}else{
		//cc/test �ڲ��˺�
		usersMode = UM_EXPERT;
	}
	setModePara(&modeParas, "UsersMode",		usersMode);		 // �û�ģʽ

	/*** ���ݼ���ģʽ������������ʱ��, ����ģʽ ***/
	if(COUNT_MODE_NORMAL == eCountMode)  
	{
		//������������������ģʽ
		ulCountMaxTime = (uint32_t)COUNT_TIMEOUT; //
		eFunctionMode = FM_SAMPLE;        
	}else if(COUNT_MODE_QC_NORMAL == eCountMode || COUNT_MODE_QC_OUTSIDE == eCountMode){
		//�ʿط���������ģʽ���ⲿ��װ����ģʽ��
		ulCountMaxTime = (uint32_t)COUNT_TIMEOUT; //
		eFunctionMode = FM_QC; 
	}else if(COUNT_MODE_QC_INNER_AUTO == eCountMode){
		//�ʿط������ڲ���װ�Զ�ģʽ��
		ulCountMaxTime = 3000; //1.5s
		eFunctionMode = FM_QC; 

	}else if(COUNT_MODE_SIGNAL_SRC == eCountMode || COUNT_MODE_SIMULATION_SIGNAL_INNER == eCountMode || COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO == eCountMode){ 
		//
		ulCountMaxTime = 3000; // 1.5s
		eFunctionMode = FM_TOOL;
	}else if(COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO == eCountMode || COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE == eCountMode){         
		ulCountMaxTime = 20000;
		eFunctionMode = FM_TOOL;
	}else if(COUNT_MODE_RE_USE == eCountMode || COUNT_MODE_100V_FIRE == eCountMode){
		ulCountMaxTime = 10000;
		eFunctionMode = FM_SAMPLE;
	}else{
		//
		ulCountMaxTime = COUNT_TIMEOUT; //S
		eFunctionMode = FM_TOOL;
	}
	setModePara(&modeParas, "FunctionMode", eFunctionMode);     //FM_SAMPLE

	//��ȡ����ģʽ
	nModePara = modePara(&modeParas, "AnalysisMode");
	
	//WBCģʽ�ж�
	i = am_hasWbcMode(nModePara);
	if ((am_hasWbcMode(nModePara)))
	{
		/*******************�����صȲ���*************************/
		setSystemPara(&systemParas, "WbcCalFactor", MachInfo.calibration.fWBC, 2);  //У׼ϵ��
		setSystemPara(&systemParas, "WbcDilutionRatio", 501.0, 1);     //����ϡ�ͱ���
		setSystemPara(&systemParas, "WbcVolumn", BC_COUNT_VOLUMN, 2);  //�¿2020.06.01֮�󣩼��������Ϊ1046.26,  826.95 �����ֵ���ݼ����غ������ı仯���仯  !!!!!!!!
		setSystemPara(&systemParas, "WbcSmpRate", smpRate, 2);  //������
		setSystemPara(&systemParas, "WbcExpectedCountTime", BC_CELL_NORMAL_COUNT_TIME, 2);//��λs���������Զ�Ӧ�Ĳ�������ʱ�� 11.6 , ���ֵ���ݼ����غ������ı仯���仯  !!!!!!!!

		// MCU: clear buffer to zero
		wbcPulseInfo = (pulse_info_i_t*)SDRAM_ALGO_ADDR;
		memset((void*)wbcPulseInfo, 0, SDRAM_ALGO_LEN); //8M
		pulseInit(wbcPulseInfo);

		//������ʼ��
		pulseMidFeatureInit(&midFeature);	
		midFeature.smpRate = smpRate;		

		//��ʼ�����ƽṹ��
		BC_Init(BC_WBC);  

		//��װ���ԣ���ⶨ���缫�Ӵ�״̬���߱�ʾ��������Ҫ�����źţ�ʱ�丳ֵ
		ulStartTick = HAL_GetTick();//osKernelGetTickCount();
		ulEndTick   = ulStartTick;

		/*********************WBC���ݲɼ����������ݴ�����ȡ������Ϣ��*********************/
		//ʱ�丳ֵ
		ulStartTick = HAL_GetTick();//osKernelGetTickCount();
		ulEndTick   = ulStartTick;
		ulTemp      = ulStartTick;
		ulTemp2     = ulStartTick;

		//��ʼѹ��
		lStartPress = Press_Value();
		//��Һ��
		if(COUNT_MODE_NORMAL == eCountMode || COUNT_MODE_QC_NORMAL == eCountMode || COUNT_MODE_RE_USE == eCountMode || COUNT_MODE_100V_FIRE == eCountMode)      
		{
			LIQUID_WAVE_OPEN;
		}
		/*** ��ʱ��ʼ ***/
		ulT4_Start =  HAL_GetTick();
		ulT4_End = ulT4_Start;
		pCountInfo->tSignalAnalysis.ulT4_Passed = 0;

		/*** �ر�ϵͳ���� ***/
		osKernelLock();
		/*** ʹ��Ѫϸ���źŲɼ� ***/
		BC_Enable(BC_WBC);
		//�ѿ������ݲɼ�
		pCountInfo->tSignalAnalysis.eDataSW_Flag = TRUE; 

		/*** ִ�����ݲɼ����̣�����Һ�������ݲɼ��� ***/
		while(pCountInfo->tSignalAnalysis.ulT4_Passed + ulTicks <= ulCountMaxTime)
		{
			ulQuery++;
			ulT4_End = HAL_GetTick();//osKernelGetTickCount();             

			/*** �㷨����ADC�������ݣ�buffer1 ***/
			if(BC_ADC_PART_1 == g_stBC_Data[BC_WBC].ucFlag)
			{
				g_stBC_Data[BC_WBC].ucFlag = BC_ADC_PART_NONE;
				dataSize++;
				C_Num++;
				
				//ָ��󲿷����ݶ� 
				status = wbcPulseIdentify(wbcPulseInfo, &wbcPulseNum, (const uint8 *)&g_stBC_Data[BC_WBC].ucBuf[0], DATA_UINT_SIZE, &midFeature);
				if (!status)
				{
					BC_Disable(BC_WBC);
					eErrorCode = ERROR_CODE_ALGO_BREAK;
					break;
				} 
			} 
				
			/*** �㷨����ADC�������ݣ�buffer2 ***/
			if(BC_ADC_PART_2 == g_stBC_Data[BC_WBC].ucFlag)
			{
				g_stBC_Data[BC_WBC].ucFlag = BC_ADC_PART_NONE;
				dataSize++;
				C_Num++;
				//ָ��󲿷����ݶ�
				status = wbcPulseIdentify(wbcPulseInfo, &wbcPulseNum, (const uint8 *)&g_stBC_Data[BC_WBC].ucBuf[BC_ADC_HALF_BUF_LEN_2B], DATA_UINT_SIZE, &midFeature);
				if (!status)
				{
					BC_Disable(BC_WBC);
					eErrorCode = ERROR_CODE_ALGO_BREAK;
					break;
				} 
			}

			/*** ���ָʾ�ƣ�����״̬ ***/
			if(ulT4_End - ulTemp2 >= MENU_LED_LIGHT_SHPERIOD) //500mv
			{
				ulTemp2 = ulT4_End;
				Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status);
			}

			/********** �����缫�������˳����� *********/
			if(COUNT_MODE_NORMAL == eCountMode || COUNT_MODE_QC_NORMAL == eCountMode || COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO == eCountMode || COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE == eCountMode){ //����/�ⲿ��װģʽ����ⶨ���缫
				Elec_Status = ELEC_STATUS;
				if(SET == Elec_Status)
				{
					BC_Disable(BC_WBC);
					usXK_V = Elec_V();
					LIQUID_WAVE_CLOSE;
					pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
					"4i: Elec Had Triggled=%d, eV=%d, Press=%d;\r\n", Elec_Status, usXK_V, Press_Value());
					break;
				}    
			}    

			/*** ��ӡС�׵�ѹ ***/
			if(ulT4_End - ulTemp >= 500)
			{
				if(ucXK_V_Index < COUNT_MAX_XK_V_NUM)
				{
					usaXK_V_Buffer[ucXK_V_Index] = XK_V();
					//printf("%d,", usaXK_V_Buffer[ucXK_V_Index]);
					ucXK_V_Index++;
				}
				ulTemp = ulT4_End;            
			}

			/*** ������ѹ����⼰���� ***/
			lPress = Press_Value();
			//����/�ⲿ��װȫ���̣�ģʽ�����ѹ��
			if(lPress > AIR_LEAK_PRESS && (COUNT_MODE_NORMAL == eCountMode)) 
			{                         
				//�ر�ADC�������ط�
				BC_Disable(BC_WBC);
				LIQUID_WAVE_CLOSE;
				//δ�������ݲɼ�
				pCountInfo->tSignalAnalysis.eDataSW_Flag = FALSE; 

				//������ִ�е�ʱ��
				ulT4_End = HAL_GetTick();
				ulTicks = ulT4_End - ulT4_Start;
				pCountInfo->tSignalAnalysis.ulT4_Passed += ulTicks;
				ulTicks = 0;
				ulT4_Start = ulT4_End;

				//����쳣�����Ƿ�������ֵ3
				pCountInfo->tSignalAnalysis.ucBuildPress_Num++;
				if(pCountInfo->tSignalAnalysis.ucBuildPress_Num > MAX_PRESS_ERROR_NUM)
				{
					//�쳣��������
					eErrorCode = ERROR_CODE_AIR_LEAK_COUNTING_2; //���ش��󣬽��ڽ�����ʾ
					pCountInfo->tSignalAnalysis.eErrorCode = ERROR_CODE_AIR_LEAK_COUNTING_2; //����������©��������MAX_PRESS_ERROR_NUM��
					LOG_Error("4e: Press Err, Press=%d, ErrCode=%d, Num=%d, T4=%d", lPress, pCountInfo->tSignalAnalysis.eErrorCode, pCountInfo->tSignalAnalysis.ucBuildPress_Num, pCountInfo->tSignalAnalysis.ulT4_Passed);
					pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
					"4e: Press Err, Press=%d, ErrCode=%d, Num=%d, T4=%d;\r\n", lPress, pCountInfo->tSignalAnalysis.eErrorCode, pCountInfo->tSignalAnalysis.ucBuildPress_Num, pCountInfo->tSignalAnalysis.ulT4_Passed);

					break;
				}else{
					//����������©����С��MAX_PRESS_ERROR_NUM��
					pCountInfo->tSignalAnalysis.eErrorCode = ERROR_CODE_AIR_LEAK_COUNTING_1;
					LOG_Error("4w: Press Err, Press=%d, HideErrCode=%d, Num=%d, T4=%d", lPress, pCountInfo->tSignalAnalysis.eErrorCode, pCountInfo->tSignalAnalysis.ucBuildPress_Num, pCountInfo->tSignalAnalysis.ulT4_Passed);
					pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
					"4w: Press Err, Press=%d, HideErrCode=%d, Num=%d, T4=%d;\r\n", lPress, pCountInfo->tSignalAnalysis.eErrorCode, pCountInfo->tSignalAnalysis.ucBuildPress_Num, pCountInfo->tSignalAnalysis.ulT4_Passed);                  
				}

				//���½�ѹһ��
				if(ERROR_CODE_SUCCESS != Build_Press(RUN_MODE_COUNT, BC_TEST_PRESS_VALUE)){
					LOG_Error("4w: Build Press Fail, Press=%d, T4=%d", Press_Value(), pCountInfo->tSignalAnalysis.ulT4_Passed);
					pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
					"4w: Build Press Fail, Press=%d, T4=%d", Press_Value(), pCountInfo->tSignalAnalysis.ulT4_Passed);
				}

				//�������µ�ʱ��ڵ㣬��ʼ��ʱ
				ulT4_Start = HAL_GetTick();
				ulT4_End   = ulT4_Start;
			}


			//��������������йر����ݲɼ����ٿ���
			if(pCountInfo->tSignalAnalysis.eDataSW_Flag == FALSE)
			{
				//����������
				LIQUID_WAVE_OPEN;
				BC_Enable(BC_WBC);
				pCountInfo->tSignalAnalysis.eDataSW_Flag = TRUE; 
			}   

			/*** ������ִ�е�ʱ�� ***/
			ulT4_End = HAL_GetTick();
			ulTicks = ulT4_End - ulT4_Start;
		}
		//ֹͣѪϸ���źŲɼ�
		BC_Disable(BC_WBC);
		//���º�ʱ
		pCountInfo->tSignalAnalysis.ulT4_Passed += ulTicks;      
		//�ָ�����
		osKernelUnlock();
		//��Һ��
		LIQUID_WAVE_CLOSE; 
		//���ָʾ�ƣ�����״̬
		////        Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status);

		//����ѹ��ֵ
		lEndPress = Press_Value();     

		//WBC���ݸ�ֵ
		dataIn.wbcPulseInfo = wbcPulseInfo;
		dataIn.wbcPulseNum = wbcPulseNum; 	

		//С�׵�ѹ��־����
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%s", "XK_V: ");
		for(i = 0; i < ucXK_V_Index; i++)
		{      
			//����С�׵�ѹ��Ϣ����־��
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%d,", usaXK_V_Buffer[i]);
		}
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "\r\n");

		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
		"4i: T2=%d, T4=%d, ErrCode=%d, StartPress=%d, EndPress=%d, XKV=%d, Elec=%d, eV=%d;\r\n", pCountInfo->tLiquidCreate.ulT2_Passed, pCountInfo->tSignalAnalysis.ulT4_Passed, pCountInfo->tSignalAnalysis.eErrorCode ,\
		lStartPress, lEndPress, XK_V(), Elec_Status, Elec_V());


		//������ģʽ���� ������Һ��������ط����쳣����
		if((COUNT_MODE_NORMAL == eCountMode && eErrorCode == ERROR_CODE_SUCCESS) || (COUNT_MODE_QC_NORMAL == eCountMode && eErrorCode == ERROR_CODE_SUCCESS))
		{  
			/*** ���źŲɼ��׶Σ������󣬶����缫˲�䴥��(��ֹ��Щʹ�ù���Ѫϸ��ģ�飬�ٴβ��Լ����źŲɼ��׶�) ***/
			if(pCountInfo->tSignalAnalysis.ulT4_Passed < 200)// )
			{
				eErrorCode = ERROR_CODE_ELEC_TRIGGLE_2;
				pCountInfo->tCountEnd.eErrorCode = eErrorCode;
				LOG_Error("4e: Elec Triggle, Elec=%d, ErrCode=%d", Elec_Status, pCountInfo->tCountEnd.eErrorCode);
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"4e: Elec Triggle, Elec=%d, ErrCode=%d;\r\n", Elec_Status, pCountInfo->tCountEnd.eErrorCode);
			} 

			/*** ����ʱ��С��8�룬�������������� ***/
			if(pCountInfo->tSignalAnalysis.ulT4_Passed < 8000)//  �ò���������Ҫ�޸�
			{
				eErrorCode = ERROR_CODE_SAMPLE_NOT_ENOUGH;
				pCountInfo->tCountEnd.eErrorCode = eErrorCode;
				LOG_Error("4e: Sample Not Enough, ErrCode=%d", pCountInfo->tCountEnd.eErrorCode);
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"4e: Sample Not Enough, ErrCode=%d;\r\n", pCountInfo->tCountEnd.eErrorCode);
			}            

			/*** ����ʱ�����15s��С�ڣ�������ʱ�� 30s��***/
			if(pCountInfo->tSignalAnalysis.ulT4_Passed > 15000 && pCountInfo->tSignalAnalysis.ulT4_Passed < ulCountMaxTime)//  �ò���������Ҫ�޸�
			{
				//
				pCountInfo->tCountEnd.eErrorCode = ERROR_CODE_T4_ERR_15S_2_MAXTIME; //T4ʱ����������ع�����1132��
				//����ڳ�ʼ���׶Σ�С�׵�ѹ�ѵ�ͨ�����ʱ"T4ʱ����������ع�����1132��" ��Ϊ "��ʼ���׶Σ�С�׵�ѹ��ͨ E1111", ������E1111������E1132
				if(pCountInfo->tCountInit.eErrorCode == ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE) 
				pCountInfo->tCountEnd.eErrorCode = ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE;

				LOG_Error("4e: T4 Err, T4=%d, HideErrCode=%d", pCountInfo->tSignalAnalysis.ulT4_Passed, pCountInfo->tCountEnd.eErrorCode);
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"4e: T4 Err, T4=%d, HideErrCode=%d;\r\n", pCountInfo->tSignalAnalysis.ulT4_Passed, pCountInfo->tCountEnd.eErrorCode);
			}     

			/*** ��ʱ�¿��ж� ***/
			if(pCountInfo->tSignalAnalysis.ulT4_Passed >= ulCountMaxTime)
			{
				//��ʱ
				eErrorCode = ERROR_CODE_TIMEOUT;
				pCountInfo->tSignalAnalysis.eErrorCode = eErrorCode;
				LOG_Error("4e: TimeOut, ErrCode=%d, T4=%d, Elec=%d, eV=%d", eErrorCode, pCountInfo->tSignalAnalysis.ulT4_Passed, Elec_Status, Elec_V());
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"4e: TimeOut, ErrCode=%d, T4=%d, Elec=%d, eV=%d;\r\n", eErrorCode, pCountInfo->tSignalAnalysis.ulT4_Passed, Elec_Status, Elec_V());
				return eErrorCode;
			}

			/***  �쳣����(�����㷨������) ***/    
			if(pCountInfo->tCountInit.eErrorCode == ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE){
				eToAlgoErrorCode = EM_1111; //EM111���ȼ���1112Ҫ��
			}else if(pCountInfo->tLiquidCreate.eErrorCode == ERROR_CODE_BSK_2S_15V){           
				eToAlgoErrorCode = EM_1112;
			}else if(pCountInfo->tSignalAnalysis.eErrorCode == ERROR_CODE_BSK_2S_15V){           
			}else if(pCountInfo->tSignalAnalysis.eErrorCode == ERROR_CODE_BSK_2S_15V){           
			}

			/**************************************** END *****************************************/ 
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, 
			"%s", "[EndCount]\r\n");  

			/*** ��©Һ���� ***/
			usXK_V = XK_V(); 
			if(usXK_V < XK_V_NORMAL_LOW_THRESHOLD || pCountInfo->tSignalAnalysis.ulT4_Passed > COUNT_TIMEOUT) //С�׵�ѹ���͡���ʱ���������ظ�ʹ�ã���©Һ����
			{   //mv
				//��©Һ������Һ��������ͬʱ�Խӵ缫�ɿ���������촦����Һ�壬�ᱻ���뵽�����ң�
				LIQUID_WAVE_OPEN;
				Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
				LIQUID_WAVE_CLOSE;
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"5w: Leakproof Fluid Treatment, XKV=%d, Press;\r\n", usXK_V, Press_Value());

			}else{
				//����Һ�������ݵĶ���Һ��ʹ�����缫��ȫ��û����ֹ���ظ�ʹ��ʱ����ʼ�׶ζ����缫δ�ܼ�ʱ������
				LIQUID_WAVE_OPEN;
				ulEndTick = HAL_GetTick();
				ulTemp   = ulEndTick;
				ulTemp2   = ulEndTick;
				while(ulEndTick - ulTemp < 2000) //2000ms
				{
					ulEndTick = HAL_GetTick();
					//�ٳ�2s��ط���ȷ�����������ɽ�û�����ָʾ�Ƽ�������
					if(ulEndTick - ulTemp2 >= MENU_LED_LIGHT_SHPERIOD) //500mv
					{
						ulTemp2 = ulEndTick;
						Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status);
					}
				}
				LIQUID_WAVE_CLOSE; //��Һ��
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"5w: Multi Suction Liquid,  XKV=%d, Press=%d, T=%d;\r\n", XK_V(), Press_Value(), ulEndTick - ulTemp);
			}
		}else if((COUNT_MODE_NORMAL == eCountMode || COUNT_MODE_RE_USE == eCountMode || COUNT_MODE_100V_FIRE == eCountMode) && eErrorCode != ERROR_CODE_SUCCESS){
			//��ǰ�� E1125�� ERROR_CODE_ALGO_BREAK
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
			"5w: ErrCode=%d, XKV=%d, Press=%d, T=%d;\r\n", eErrorCode, XK_V(), Press_Value(), ulEndTick - ulTemp);

			if(COUNT_MODE_RE_USE == eCountMode)
			{
				//�ظ�ʹ��ģʽ������Һ������©Һ������Һ��������ͬʱ�Խӵ缫�ɿ���������촦����Һ�壬�ᱻ���뵽�����ң�
				LIQUID_WAVE_OPEN;
				Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
				LIQUID_WAVE_CLOSE;
			}
			return eErrorCode;
		}

		//
		if(COUNT_MODE_RE_USE == eCountMode)
		{
			//�ظ�ʹ��ģʽ������Һ������©Һ������Һ��������ͬʱ�Խӵ缫�ɿ���������촦����Һ�壬�ᱻ���뵽�����ң�
			LIQUID_WAVE_OPEN;
			Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
			LIQUID_WAVE_CLOSE;
		}

		/****************��ӡ����ԭʼ����*********************/
		printf("--------------------bc raw data ------------");
		for(i = 0; i < 200; i++)
		{
			printf("%d,", (int)g_stBC_Data[BC_WBC].ucBuf[i]); 
		}
		printf("");  
		for(i = 512; i < 200; i++)
		{
			printf("%d,", (int)g_stBC_Data[BC_WBC].ucBuf[i]); 
		}
		printf("\r\n");  

		/*** ����ʱ�� ***/
		sprintf(ucaTimeBuffer, "%d.%d%d", pCountInfo->tSignalAnalysis.ulT4_Passed/1000, \
		(pCountInfo->tSignalAnalysis.ulT4_Passed%1000)/100, ((pCountInfo->tSignalAnalysis.ulT4_Passed%100)/10));

		countTime = (float)atof(ucaTimeBuffer); //����ʱ�䣨��λ�룩, ʱ��ֵ��ʵ�ʲ�һ�µĻ����ᵼ��wbc���ƫ��

		/*** ��Һʱ�� ***/
		ulTicks = pCountInfo->tLiquidCreate.ulT2_Passed + pCountInfo->tSignalAnalysis.ulT4_Passed;    
		sprintf(ucaTimeBuffer, "%d.%d%d", ulTicks/1000, (ulTicks%1000)/100, ((ulTicks%100)/10));
		volumnTime = (float)atof(ucaTimeBuffer);

		setSystemPara(&systemParas, "WbcCountTime", countTime, 2);
		setSystemPara(&systemParas, "WbcVolumnTime", volumnTime, 2);
		/*** ���ݹ�������㷨  ***/
		setModePara(&modeParas, "ErrorMode", eToAlgoErrorCode);//eEM_SUCCESS); // ����ģʽ(���������󣬼���ǰ)        

		/*** ����ʱ��������㷨 ***/
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, 
		"5i: ToAlgoErrCode=%d, CountTime: %d, %0.2f, volumnTime: %d, %0.2f;\r\n", eToAlgoErrorCode, pCountInfo->tSignalAnalysis.ulT4_Passed, countTime, ulTicks, volumnTime);
	}//*** End WBC


	/*************  HGB   *************/
	if(am_hasHgbMode(nModePara))
	{
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "HGB ADC: ");
		//������������ģʽģʽ��������ģʽ����ʱ�ɼ�HGB�źţ�������
		if(COUNT_MODE_NORMAL  == eCountMode || COUNT_MODE_QC_NORMAL  == eCountMode)
		{
			//WBC_HGBģʽ���ڳ�ʼ���׶��Ѳɼ�HGB�ź�
			dataIn.hgbDataBuffer = (char*)pCountInfo->tCountInit.HGB535Adc;			//���������ڴ������ԣ���415��535��ADC���ݶ������buf����
		}else{
			//===�ɼ�HGB��ADC����===
			uint16_t Temp = 0;
			
			BC_CUR_SW_OFF;
			
			//�ɼ�535��������
			Light_Start(HGBLED_535,MachInfo.paramSet.current535);
			osDelay(5000);
			
			//�Ȳɼ�535��������
			for(i = 0; i < 10; i++){
				Temp = Get_HGBAvgADC(10);
				
				pCountInfo->tCountInit.HGB535Adc[i*2]   = *(((uint8_t*)&Temp)+1);
				pCountInfo->tCountInit.HGB535Adc[i*2+1] = *(((uint8_t*)&Temp));
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%d", Temp);

				if(i == 3 || i == 8){
					Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status); //���� 
				}
			}
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "\r\n");
			
			//�ɼ�415��������
			Light_Start(HGBLED_415,MachInfo.paramSet.current415);
			osDelay(3000);
			
			//�ɼ�415��������
			for(i = 0; i < 10; i++){
				Temp = Get_HGBAvgADC(10);
				
				pCountInfo->tCountInit.HGB415Adc[i*2]   = *(((uint8_t*)&Temp)+1);
				pCountInfo->tCountInit.HGB415Adc[i*2+1] = *(((uint8_t*)&Temp));
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%d", Temp);

				if(i == 3 || i == 8){
					Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status); //���� 
				}
			}
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "\r\n");
			
			//�ر�HGB��
			Light_Stop();
			dataIn.hgbDataBuffer = (char*)pCountInfo->tCountInit.HGB535Adc;			//���������ڴ������ԣ���415��535��ADC���ݶ������buf����
		}

		/*** HGB�㷨��ʼ���������� ***/
		setModePara(&modeParas, "HgbMethodMode",HMM_DOUBLE_WAVE);
		setSystemPara(&systemParas, "HgbCalFactor",MachInfo.calibration.fHGB, 2);				//У׼ϵ�������˹�У׼���洫��
		
		setSystemPara(&systemParas, "HgbDilutionRatio", HGB_DILUTION_RATIO, 1);					//�Լ���Ѫ����ϡ�ͱȣ��̶�501.0
		setSystemPara(&systemParas, "HgbBlankADC", HGB_BLACK_ADC_VALUE, 1);						//��ɫ�������Ѫ����ADCֵ���̶�3480.0

		// HGB����������
		setSystemPara(&systemParas, "HgbCurveFit_K1", HGB_CURVE_FIT_K1, 2);						//�������б��
		setSystemPara(&systemParas, "HgbCurveFit_B1", HGB_CURVE_FIT_B1, 2);						//������߽ؾ�
		// HGB˫��������
		setSystemPara(&systemParas, "HgbCurveFit_K2", HGB_CURVE_FIT_K2, 2);						//�������б��
		setSystemPara(&systemParas, "HgbCurveFit_B2", HGB_CURVE_FIT_B2, 2);						//������߽ؾ�
	} //*** END HGB	

	/****************************************************************************************************************************************************
	*
	*   ��Ƽ���
	*
	****************************************************************************************************************************************************/   


	dataSize = dataSize*DATA_UINT_SIZE;
	// �㷨������	

	dataIn.modeParas = &modeParas;
	dataIn.systemParas = &systemParas;
	dataIn.configParas = &configParas;	//***
	dataOut.reportParas = &reportParas;
	dataOut.featureParas = &featureParas;
	dataOut.timeStamps	= timeStamps;
	dataOut.timeStampFlags	= timeStampFlags;
	dataOut.flagParas = &flagParas;
	dataOut.histList = &histList;

	/*** �㷨����  ***/
	Panel_Led_Status(RUN_MODE_NORMAL, g_ePanel_LED_Status);  //�㷨����ʱ������������ҫ 
	ulStartTick = HAL_GetTick();
	status = calculate(&dataOut, &dataIn); //////  error at this place
	ulEndTick = HAL_GetTick();

	/*** �㷨��Ϣ���������浽��־��  ***/
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%s", "[Algo]\r\n");

	//�㷨�汾����������,wbc�������
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
	"alg_ver=%s, release_Date=%s, usersMode=%d, FunctionMode=%d,MaxCountTime=%0.2f;\r\n",\
	version(), versionDate(), usersMode, eFunctionMode, BC_WBC_DATA_SAMPLE_TIMEOUT);
			  
	/*** WBC ���ò��� ***/
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, 
	"M_WbcCalFactor=%0.2f, WbcCalFactor=%0.2f, WbcVolumn=%0.2f, WbcExpectedCountTime=%0.2f, ToAlgoErrCode=%d;\r\n", MachInfo.calibration.fWBC, systemPara(&systemParas, "WbcCalFactor", 0),BC_COUNT_VOLUMN, BC_CELL_NORMAL_COUNT_TIME, eToAlgoErrorCode);

	/*** HGB ������� ***/
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
	"M_HgbCalFactor=%0.2f,HgbCalFactor=%0.2f, HgbBlankADC=%0.2f, HgbCurveFit_K1=%0.2f, HgbCurveFit_B1=%0.2f,HgbCurveFit_K2=%0.2f, HgbCurveFit_B2=%0.2f;",\
	MachInfo.calibration.fHGB, systemPara(&systemParas, "HgbCalFactor", 0),HGB_BLACK_ADC_VALUE, HGB_CURVE_FIT_K1, HGB_CURVE_FIT_B1,HGB_CURVE_FIT_K2, HGB_CURVE_FIT_B2);

	//HGB ��������
	hgbDataCV	= valueListDP(&featureParas, "HgbDataCV");			// HGB����CV
	ratioHgbAve = valueListDP(&featureParas, "HgbTransmittance");	// HGB͸����
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
	"HGB Feature: hgbDataCV=%0.2f, ratioHgbAve=%.2f;\r\n", hgbDataCV, ratioHgbAve);

	/*******  �ʿ� ��ȡ����������� **************************************/
	if(eCountMode == COUNT_MODE_QC_NORMAL || eCountMode == COUNT_MODE_QC_INNER_AUTO  || eCountMode == COUNT_MODE_QC_OUTSIDE)
	{ 
		/*********  �ʿؼ������� ************************************************************1********/

		QC_WBCHGB_TestInfo_t *pWBCHGB_TestInfo = (QC_WBCHGB_TestInfo_t*)pDataBuffer;   
		// ֱ��ͼ
		memset((void*)pWBCHGB_TestInfo->fWBC_Histogram, 0, sizeof(pWBCHGB_TestInfo->fWBC_Histogram));
		wbcDispHist = getHistInfo(&histList, "WbcDispHistInfo");
		if (wbcDispHist)
		{
			for (i = 0; i < wbcDispHist->dataLen; i++)
			{
				pWBCHGB_TestInfo->fWBC_Histogram[i] = (float)wbcDispHist->datas[i];
				//                printf("[%03d] = %0.2f\t", i, pWBCHGB_TestInfo->fWBC_Histogram[i]);
				if ( i%5 == 0 || i == wbcDispHist->dataLen - 1 )
				{
					//printf("\n");
				}
			}
		}     

		/*��ȡWBC�������*/
		//���ֵ��flֵ
		pWBCHGB_TestInfo->fWbcMaxPos = valueListDP(&reportParas, "PeakX");
		pWBCHGB_TestInfo->fWbcMaxPos = (pWBCHGB_TestInfo->fWbcMaxPos + 1)*400/256;
		//wbc
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC]    =  valueListDP(&reportParas, "WBC");
		ucFlag = flagListDP(&reportParas, "WBC");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_WBC] =  QC_WBCHGB_Alert_Flag(pWBCHGB_TestInfo->eFileNumIndex, WBCHGB_RST_WBC, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC]);

		//GRAN#
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ] =  valueListDP(&reportParas, "GRAN#");
		ucFlag = flagListDP(&reportParas, "GRAN#");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_GranJ] =  QC_WBCHGB_Alert_Flag(pWBCHGB_TestInfo->eFileNumIndex, WBCHGB_RST_GranJ, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ]);       

		//MID#
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ] =  valueListDP(&reportParas, "MID#");
		ucFlag = flagListDP(&reportParas, "MID#");    
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_MidJ] =  QC_WBCHGB_Alert_Flag(pWBCHGB_TestInfo->eFileNumIndex, WBCHGB_RST_MidJ, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ]);       

		//LYM#
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ] =  valueListDP(&reportParas, "LYM#");
		ucFlag = flagListDP(&reportParas, "LYM#");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_LymJ] =  QC_WBCHGB_Alert_Flag(pWBCHGB_TestInfo->eFileNumIndex, WBCHGB_RST_LymJ, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ]);

		//GRAN%
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB] =  valueListDP(&reportParas, "GRAN%");
		ucFlag = flagListDP(&reportParas, "GRAN%");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_GranB] =  QC_WBCHGB_Alert_Flag(pWBCHGB_TestInfo->eFileNumIndex, WBCHGB_RST_GranB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB]);

		//MID%
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB] =  valueListDP(&reportParas, "MID%");
		ucFlag = flagListDP(&reportParas, "MID%");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_MidB] =  QC_WBCHGB_Alert_Flag(pWBCHGB_TestInfo->eFileNumIndex, WBCHGB_RST_MidB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB]);         

		//LYM%
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB] =  valueListDP(&reportParas, "LYM%");
		ucFlag = flagListDP(&reportParas, "LYM%");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_LymB] =  QC_WBCHGB_Alert_Flag(pWBCHGB_TestInfo->eFileNumIndex, WBCHGB_RST_LymB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB]);         

		//HGB
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB]    =  valueListDP(&reportParas, "HGB");
		ucFlag = flagListDP(&reportParas, "HGB");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_HGB] =  QC_WBCHGB_Alert_Flag(pWBCHGB_TestInfo->eFileNumIndex, WBCHGB_RST_HGB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB]);


		//Line1
		pWBCHGB_TestInfo->fLines[0] =  valueListDP(&reportParas, "Line1");  // �����ָ��ߵ�ǰ�ǰ���400��ͨ������������256ͨ������Ҫתһ�¡�;		
		//Line2
		pWBCHGB_TestInfo->fLines[1] =  valueListDP(&reportParas, "Line2");				
		//Line3
		pWBCHGB_TestInfo->fLines[2] =  valueListDP(&reportParas, "Line3");				
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
		"\r\nWBC=%0.2f, HGB=%0.2f(HGB1=%0.2f,HGB2=%0.2f, Gran#=%0.2f, MID#=%0.2f, LYM#=%0.2f, GranB=%0.2f, MidB=%0.2f, LymB=%0.2f, wbcMaxPos=%0.2f;\r\n", pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB],
		valueListDP(&featureParas,"HGB1"),valueListDP(&featureParas,"HGB2"),pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ],\
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB], pWBCHGB_TestInfo->fWbcMaxPos);


		// ������Ϣ���
		pWBCPrompt = pWBCHGB_TestInfo->ucaWBC_ErrorPrompt;
		pHGBPrompt = pWBCHGB_TestInfo->ucaHGB_ErrorPrompt;
		for (i = 0; i<flagParas.num; i++)
		{
			if( flagParas.paras[i].flag )
			{
				if(LANGUAGE_ENGLISH == MachInfo.systemSet.eLanguage)
				{
					ucLanguage = 0; //1:���ģ�0��Ӣ��
				}else{
					ucLanguage = 1;
				}

				// WBCͨ������
				if (flagParaItemMessageMapWbc(ucaPromptInfo, flagParas.paras[i].name, usersMode, ucLanguage))
				{
					ulIndex = strlen(ucaPromptInfo);
					if(((pWBCPrompt - pWBCHGB_TestInfo->ucaWBC_ErrorPrompt) + ulIndex) < WBC_PROMPT_LEN)
					{
						strncpy((char*)pWBCPrompt, ucaPromptInfo, ulIndex);
						pWBCPrompt += ulIndex;
						*(pWBCPrompt++) = '\r';
						*(pWBCPrompt++) = '\n';
					}

				}
				// HGBͨ������
				if (flagParaItemMessageMapHgb(ucaPromptInfo, flagParas.paras[i].name, usersMode, ucLanguage))
				{
					ulIndex = strlen(ucaPromptInfo);
					if(((pHGBPrompt - pWBCHGB_TestInfo->ucaHGB_ErrorPrompt) + ulIndex) < HGB_PROMPT_LEN)
					{
						strncpy((char*)pHGBPrompt, ucaPromptInfo, ulIndex);
						pHGBPrompt += ulIndex;
						*(pHGBPrompt++) = '\r';
						*(pHGBPrompt++) = '\n';
					}
				}			
			}
		}

		//���ý�����־
		pWBCHGB_TestInfo->ucaEndFlag[0] = '\r';
		pWBCHGB_TestInfo->ucaEndFlag[1] = '\n';    
	}else{
		/******** ��������������������� *********************************************************3***/
		WBCHGB_TestInfo_t *pWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)pDataBuffer;

		// ֱ��ͼ
		memset((void*)pWBCHGB_TestInfo->fWBC_Histogram, 0, sizeof(pWBCHGB_TestInfo->fWBC_Histogram));
		wbcDispHist = getHistInfo(&histList, "WbcDispHistInfo");
		if (wbcDispHist)
		{
			for (i = 0; i < wbcDispHist->dataLen; i++)
			{
				pWBCHGB_TestInfo->fWBC_Histogram[i] = (float)wbcDispHist->datas[i];
				//                printf("[%03d] = %0.2f\t", i, pWBCHGB_TestInfo->fWBC_Histogram[i]);
				if ( i%5 == 0 || i == wbcDispHist->dataLen - 1 )
				{
					//printf("\n");
				}
			}
		}     

		/*��ȡWBC�������*/
		//���ֵ��flֵ
		pWBCHGB_TestInfo->fWbcMaxPos = valueListDP(&reportParas, "PeakX");
		pWBCHGB_TestInfo->fWbcMaxPos = (pWBCHGB_TestInfo->fWbcMaxPos + 1)*400/256;
		//wbc
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC]    =  valueListDP(&reportParas, "WBC");
		ucFlag = flagListDP(&reportParas, "WBC");
		if(PF_INVALID == ucFlag)
		{
			eErrorCode = ERROR_CODE_RESULT_ASTERISK;
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
			"Count Result ***, ErrCode=%d,", eErrorCode);
		}
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_WBC] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_WBC, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC]);

		//GRAN#
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ] =  valueListDP(&reportParas, "GRAN#");
		ucFlag = flagListDP(&reportParas, "GRAN#");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_GranJ] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_GranJ, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ]);

		//MID#
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ] =  valueListDP(&reportParas, "MID#");
		ucFlag = flagListDP(&reportParas, "MID#");    
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_MidJ] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_MidJ, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ]);

		//LYM#
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ] =  valueListDP(&reportParas, "LYM#");
		ucFlag = flagListDP(&reportParas, "LYM#");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_LymJ] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_LymJ, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ]);

		//GRAN%
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB] =  valueListDP(&reportParas, "GRAN%");
		ucFlag = flagListDP(&reportParas, "GRAN%");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_GranB] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_GranB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB]);

		//MID%
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB] =  valueListDP(&reportParas, "MID%");
		ucFlag = flagListDP(&reportParas, "MID%");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_MidB] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_MidB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB]); 

		//LYM%
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB] =  valueListDP(&reportParas, "LYM%");
		ucFlag = flagListDP(&reportParas, "LYM%");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_LymB] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_LymB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB]);    

		//HGB
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB]    =  valueListDP(&reportParas, "HGB");
		ucFlag = flagListDP(&reportParas, "HGB");
		pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_HGB] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_HGB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB]);       

		//Line1
		pWBCHGB_TestInfo->fLines[0] =  valueListDP(&reportParas, "Line1");  // �����ָ��ߵ�ǰ�ǰ���400��ͨ������������256ͨ������Ҫתһ�¡�;		
		//Line2
		pWBCHGB_TestInfo->fLines[1] =  valueListDP(&reportParas, "Line2");				
		//Line3
		pWBCHGB_TestInfo->fLines[2] =  valueListDP(&reportParas, "Line3");				
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
		"WBC=%0.2f, HGB=%0.2f(HGB1=%0.2f,HGB2=%0.2f), Gran#=%0.2f, MID#=%0.2f, LYM#=%0.2f, GranB=%0.2f, MidB=%0.2f, LymB=%0.2f, wbcMaxPos=%0.2f;\r\n", pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB],
		valueListDP(&featureParas,"HGB1"),valueListDP(&featureParas,"HGB2"),pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ],\
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB], pWBCHGB_TestInfo->fWbcMaxPos);


		// ������Ϣ���
		pWBCPrompt = pWBCHGB_TestInfo->ucaWBC_ErrorPrompt;
		pHGBPrompt = pWBCHGB_TestInfo->ucaHGB_ErrorPrompt;
		for (i = 0; i<flagParas.num; i++)
		{
			if( flagParas.paras[i].flag )
			{
				//
				if(LANGUAGE_ENGLISH == MachInfo.systemSet.eLanguage)
				{
					ucLanguage = 0; //1:���ģ�0��Ӣ��
				}else{
					ucLanguage = 1;
				}

				// WBCͨ������
				if (flagParaItemMessageMapWbc(ucaPromptInfo, flagParas.paras[i].name, usersMode, ucLanguage))
				{
					ulIndex = strlen(ucaPromptInfo);
					if(((pWBCPrompt - pWBCHGB_TestInfo->ucaWBC_ErrorPrompt) + ulIndex) < WBC_PROMPT_LEN)
					{
						strncpy((char*)pWBCPrompt, ucaPromptInfo, ulIndex);
						pWBCPrompt += ulIndex;
						*(pWBCPrompt++) = '\r';
						*(pWBCPrompt++) = '\n';
					}
				}
				// HGBͨ������
				if (flagParaItemMessageMapHgb(ucaPromptInfo, flagParas.paras[i].name, usersMode, ucLanguage))
				{
					ulIndex = strlen(ucaPromptInfo);
					if(((pHGBPrompt - pWBCHGB_TestInfo->ucaHGB_ErrorPrompt) + ulIndex) < HGB_PROMPT_LEN)
					{
						strncpy((char*)pHGBPrompt, ucaPromptInfo, ulIndex);
						pHGBPrompt += ulIndex;
						*(pHGBPrompt++) = '\r';
						*(pHGBPrompt++) = '\n';
					}
				}			
			}
		}

		//���ý�����־
		pWBCHGB_TestInfo->ucaEndFlag[0] = '\r';
		pWBCHGB_TestInfo->ucaEndFlag[1] = '\n';
	} /********************************************************************************4*********/
	//�����㷨���㣬���ѻ�ȡ����
	pCountInfo->ucUseAlgoFlag = 1;

	/*****************��������������******************/
	evaData( wbcPulseInfo, wbcPulseNum, &lossSum);
	
	/*** ������� ***/
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
	"wbcPulseNum=%d, DataSize=%d;\r\n", wbcPulseNum, dataSize);

	/***************�㷨������*****************/
	// �����������飨������־��
	// ����������Ч��
	isPulseValid = (int)( valueListDP(&featureParas, "IsPulseValid") + 0.5 );      
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
	  "6_i:isPulseValid=%d\r\n%s\r\n", isPulseValid, "[DataFeature]");                                            
	
	if(TRUE)
	{
		stampNum = *dataOut.stampNum;
		for (i= 0; i<stampNum; i++)
		{
			//timeStamps����λʱ�����ź�������timeStampFlags����ʾ�Ƿ������㣬0�����룬1��������
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%03d=%d,%d;\t", i,timeStamps[i],timeStampFlags[i]);
			if(i%9 == 0 && i != 0)
			{
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "\r\n");
			}
		}
	}
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "\r\n");

	//
	if(pCountInfo->tLiquidCreate.ucReConnNum >= 2 && pCountInfo->tSignalAnalysis.ulT4_Passed > 15000)
	{
		eErrorCode = ERROR_CODE_RE_CONN_2_T4_15S;      //Һ·�����׶��ضԽӴ���>=2, ��T4>15s 
	}
	
	return eErrorCode;
}



