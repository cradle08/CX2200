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




//血细胞数据结构体
__IO BC_Data_t   g_stBC_Data[BC_END] = {0};


/*
*ADC半接受完成中断触发
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
*ADC接受完成中断触发
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
* bc 控制结构体初始化
*/
void BC_Init(BC_Type_e eType)
{
    //参数判断
    if(IS_BC_TYPE(eType) == RESET) 
    {
    //@todo error
    }
    
    //初始化结构体
    memset((void*)&g_stBC_Data, 0, sizeof(g_stBC_Data));
    g_stBC_Data[eType].eType = eType;
    g_stBC_Data[eType].ucFlag = BC_ADC_PART_NONE;           
}


/*
*使能血细胞ADC采样
*/
void BC_Enable(BC_Type_e eType)
{
  extern ADC_HandleTypeDef hadc1;
  extern DMA_HandleTypeDef hdma_adc1;
  extern ADC_HandleTypeDef hadc2;
  extern DMA_HandleTypeDef hdma_adc2;
  
  //参数判断
  if(IS_BC_TYPE(eType) == RESET) 
  {
    //@todo error
  }
  
  if(BC_WBC == eType) //白细胞
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
*失能血细胞ADC采样
*/
void BC_Disable(BC_Type_e eType)
{
  //参数判断
  if(IS_BC_TYPE(eType) == RESET) 
  {
    //@todo 
  }
  
  //失能ADC采样
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
*   计数，信息初始化
*/
void CountInfo_Init(__IO CountInfo_t *pCountInfo)
{
    //初始化阶段，信息初始化
    pCountInfo->tCountInit.eOutInModule_Status       = OUTIN_MODULE_POSITION_NUKNOWN;   
    pCountInfo->tCountInit.eTanzhenOC_Status         = FALSE;                //探针光耦，触发状态。FALSE:未触发，TURE：已触发
    pCountInfo->tCountInit.eCountCell_Flag           = FALSE;                //计数池检测标志。  FALSE:未放入计数池，TURE：已放入计数池
    pCountInfo->tCountInit.eElec_Status              = FALSE;                //定量电极，未触发状态。FALSE:未未触发，TURE：已触发  
    pCountInfo->tCountInit.ucTZMotor_Move_Num        = 0;                    //电机（探针对接电机）执行次数
    pCountInfo->tCountInit.ucOutIn_Motor_Move_Num    = 0;                    //电机（主电机）执行次数 
    pCountInfo->tCountInit.ucBuildPress_Num          = 0;                    //已执行的建压次数 
//    memset((void*)pCountInfo->tCountInit.ucHGB_Buffer, 0,  HGB_BUFFER_LEN);  //HGB采集数据缓存，10个ADC数据
    pCountInfo->tCountInit.eJump_Flag                = FALSE;                //FALSE: 不需要跳过液路建立阶段，TRUE:跳过液路建立阶段
    pCountInfo->tCountInit.eErrorCode                = ERROR_CODE_SUCCESS;   //错误码
    
    //液路建立阶段
    pCountInfo->tLiquidCreate.eElec_Status              = FALSE;    //定量电极，未触发状态
    pCountInfo->tLiquidCreate.ucPress_ErrNum            = 0;        //缓冲室，已触发的异常次数   
    pCountInfo->tLiquidCreate.ulT2_Passed               = 0;        //T2阶段（液路建立阶段）的已执行时间
    memset((void*)pCountInfo->tLiquidCreate.usXK_V_Buffer, 0, LIQUID_CREATE_XK_V_NUM);
    pCountInfo->tLiquidCreate.usXK_V_Min                = 0xFFFF;   //小孔电压最小值
    pCountInfo->tLiquidCreate.ucDK_Num                  = 0;        //发生堵孔的次数
    pCountInfo->tLiquidCreate.eErrorCode                = ERROR_CODE_SUCCESS;
       
    //信号稳定度检测阶段
    pCountInfo->tSignalMonitor.ucMonitor_Num            = 0;                     //已重复检测次数//
    pCountInfo->tSignalMonitor.eErrorCode               = ERROR_CODE_SUCCESS;    //错误码

    //信号采集及分析阶段
    pCountInfo->tSignalAnalysis.eDataSW_Flag            = FALSE;   //是否开启数据采集标志未，FALSE：未开启，TURE：已开启
    pCountInfo->tSignalAnalysis.ucBuildPress_Num        = 0;       //缓冲室,已执行的建压次数
    pCountInfo->tSignalAnalysis.ucDK_Num                = 0;       //已发生排堵次数
    pCountInfo->tSignalAnalysis.ulT4_Passed             = 0;       //信号分析阶段，耗时
    pCountInfo->tSignalAnalysis.eErrorCode              = ERROR_CODE_SUCCESS;    //错误码
       
    //结束阶段
    pCountInfo->tCountEnd.eErrorCode                    = ERROR_CODE_SUCCESS;    //错误码
    
    pCountInfo->tCountLog.usLogLen                      = 0;
    pCountInfo->tCountLog.usCrc                         = 0;
}






/*
*  探针信号采集，分析
*/
FeedBack_e Elec_Single_Analyse(uint32_t *pulNum, double *pdMean, double *pdCV)
{  	
    pulse_mid_feature midFeature = {0};	
	double64 smpRate = BC_SAMPLE_RATE;   //数据采样频率，单位Hz
    pulse_info_i_t *pulseInfo;  //脉冲信息buffer
    uint32_t pulseNum = 0;      //本次测试脉冲个数
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

  
      //算法版本，编译日期
//    LOG_Info("alg_version = %s\t",version());
//    LOG_Info("alg_release_Date = %s\n",versionDate());
 
	//特征初始化
    pulseMidFeatureInit(&midFeature);	
	midFeature.smpRate = smpRate;      
        
    //开辟算法内存空间
	pulseInfo = (pulse_info_i_t*)SDRAM_ALGO_ADDR;
    memset((void*)pulseInfo, 0, SDRAM_ALGO_LEN);
    
    //计时初始化
    ulStartTick = HAL_GetTick();
    ulEndTick   = ulStartTick;
    //关闭调度器
    osKernelLock();
    //采集数据
    while(ulEndTick - ulStartTick < 1500) //1500ms
    {
        //使能血细胞信号采集
        BC_Enable(BC_WBC);
        
        //等待采集到1024字节数据
        while(1){
            if(BC_ADC_PART_1 == g_stBC_Data[BC_WBC].ucFlag){
                //关闭采集
                BC_Disable(BC_WBC);
                break;
            }
        }	
        Count++;
        //复位标志
        g_stBC_Data[BC_WBC].ucFlag = BC_ADC_PART_NONE;
        
        //计算脉冲个数
        wbcPulseIdentify(pulseInfo, &pulseNum, (const uint8 *)&g_stBC_Data[BC_WBC].ucBuf[0], DATA_UINT_SIZE, &midFeature);
        
        //16为数据扩展成32数据
        for(i=0;i<512;i++){
            RawChangeBuf[i] = g_stBC_Data[BC_WBC].ucBuf[i];
        }
        
        //计算mean
        CurrentMean = getMean_i(RawChangeBuf,512);
        MeanSum += CurrentMean;
        
        //计算CV
        CurrentCV = getCv_i(RawChangeBuf,512,0);
        CVSum += CurrentCV;
        
        //
        ulEndTick = HAL_GetTick();
	}
		
    //恢复调度器
    osKernelUnlock();
    
    //计算平均MEAN
    MeanAvg = MeanSum/Count;
    //计算平均CV
    CVAvg = CVSum/Count*100;
    
    //判断是否出现异常
    *pulNum = pulseNum; *pdMean = MeanAvg; *pdCV = CVAvg;
    //if(pulseNum > SINGAL_ANALYSIS_NUM_THRESHOLD || MeanAvg > SINGAL_ANALYSIS_MEAN_THRESHOLD || CVAvg > SINGAL_ANALYSIS_CV_THRESHOLD){
    if(pulseNum > SINGAL_ANALYSIS_NUM_THRESHOLD){
        eFeedBack = FEED_BACK_FAIL; //探针接触异常
    }
    
    return eFeedBack;
}



/*
* 获取报警标志，信息， 正常/可疑值才会进行偏大/偏小判断，无效值报*， ucFlag：为算法报警标志
*/
WBCHGB_ALERT_e WBCHGB_Alert_Flag(uint8_t ucFlag, ReferGroup_e eReferGroup, WBCHGB_Rst_e eParamIndex, float fValue)
{
    extern MachInfo_s MachInfo;
    uint32_t ulValue = 0;
    uint8_t ucFlagL = 0, ucFlagH = 0;
  
    
    //报* ？判断
    if(ucFlag == PF_INVALID) //无效值，报*
    {
        return WBCHGB_ALERT_INVALID;
    }else if(ucFlag == PF_NORMAL){  //正常值
    
        ucFlagH = 0x00; 
    }else if(ucFlag == PF_REVIEW){  //可疑值，报？
        ucFlagH = 0x10;
    }
    
    //↑↓判断，正常和可疑值，才会进行偏高↑、偏低↓判断
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
*   获取质控报警标志，对于质控，只需显示偏大↑，偏小↓
*/
WBCHGB_ALERT_e QC_WBCHGB_Alert_Flag(QC_FILENUM_INDEX_e eFileNumIndex, WBCHGB_Rst_e eParamIndex, float fValue)
{
    extern MachInfo_s MachInfo;  
    float ulValue = 0; //uint32_t ulValue = 0;
    uint8_t ucFlagL = 0, ucFlagH = 0;
    
    //↑↓判断，正常和可疑值，才会进行偏高↑、偏低↓判断   
    ulValue = fValue;//ulValue = (uint32_t)(fValue*100);
    switch(eParamIndex)
    {
        case WBCHGB_RST_WBC:
        {
            if(ulValue > MachInfo.qcInfo.set[eFileNumIndex].faUpperLimit[eParamIndex])
            {
                ucFlagL = 0x02; //偏大↑
            }else if(ulValue < MachInfo.qcInfo.set[eFileNumIndex].faLowerLimit[eParamIndex]){
                ucFlagL = 0x01; //偏小↓
            }else{
                ucFlagL = 0x00; //正常
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
*  WBC算法计算处理
*/
#define PROMPT_INFO_LEN		30
#define COUNT_MAX_XK_V_NUM  ((uint32_t)(COUNT_TIMEOUT*2/1000))   //一秒2个小孔电压数据，在COUNT_TIMEOUT超时内最大的个数

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
	uint16_t usaXK_V_Buffer[COUNT_MAX_XK_V_NUM]  = {0}; //计数过程保存小孔电压信息
	uint8_t ucXK_V_Index                         = 0;   //当前已缓存小孔电压的个数
	uint8_t *pWBCPrompt = NULL, *pHGBPrompt = NULL, ucFlag = 0, ucLanguage = 1; //0:英文，1：中文
	uint8_t Elec_Status = RESET;
	int32_t lPress = 0, lStartPress = 0, lEndPress = 0;

	// 数据完整性评价
	int lossSum = 0, isPulseValid = 0;

	//算法参数定义
	// 参数列表
	nParaList modeParas;	// 模式参数
	dParaList systemParas;	// 系统参数
	dParaList configParas;	// 配置参数 *
	dParaList reportParas;	// 报告参数
	dParaList featureParas;	// 特征参数（中间信息、中间状态）
	dParaList flagParas;	// 报警参数
	histInforList histList;	// 直方图信息//WBC/RBC/PLT

	//采集数据大小，Byte
	int dataSize = 0, C_Num = 0;

	// 中间状态信息初始化
	pulse_info_i_t *wbcPulseInfo;  //脉冲信息buffer
	uint32 wbcPulseNum = 0;        //脉冲个数
	double64 smpRate = BC_SAMPLE_RATE;      //MCU主频180M=1.5e6; 168M=1.5e6;   //数据采样频率，单位Hz
	pulse_mid_feature midFeature = {0};		
	BOOL8 status;       //算法计算状态
	int nModePara = 0;      //测量模式
	double countTime = 0;   //计数时间
	double volumnTime = 0;  //抽液时间
	double hgbDataCV = 0, ratioHgbAve = 0;

	int stampNum = 0;
	int timeStamps[MAXDURATION] = {0};
	int timeStampFlags[MAXDURATION] = {0};

	/*****************算法输入输出****************************/
	alg_data_in dataIn = {0};
	alg_data_out dataOut = {0};

	// 结果输出
	histInfor *wbcDispHist = NULL;

	/******************算法初始化***********************/
	// 初始化
	memset(&dataIn, 0, sizeof(alg_data_in)); //***
	memset(&dataOut, 0, sizeof(alg_data_out)); //***
	initParaListNP(&modeParas);
	initParaListDP(&systemParas);
	initParaListDP(&reportParas);
	initParaListDP(&configParas);	//***
	initParaListDP(&featureParas);
	initParaListDP(&flagParas);
	initHistList(&histList);

	setModePara(&modeParas, "AnalysisMode",		AM_WBC_HGB); //WBC HGB 模式
	setModePara(&modeParas, "SamplingVialMode",	SVM_OPEN);
	setModePara(&modeParas, "SpecimenMode",		SM_PVB);
	setModePara(&modeParas, "SpeciesMode",		SPM_HUMAN);
	setModePara(&modeParas, "AlgMode",			AM_RELEASE); //AM_DEBUG
	setModePara(&modeParas, "RefGroupMode",		RGM_GENERAL);	 // 参考组模式 , 根据用户的选中设置 ！！！！！！

	//根据当前账号类型，设置算法的账号模式
	if(ACCOUNT_TYPE_NORMAL == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type || ACCOUNT_TYPE_NEW == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type)
	{
		// 001
		usersMode = UM_NORMAL;
	}else if(ACCOUNT_TYPE_ADMIN == MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type){
		//admin
		usersMode = UM_ADMIN;
	}else{
		//cc/test 内部账号
		usersMode = UM_EXPERT;
	}
	setModePara(&modeParas, "UsersMode",		usersMode);		 // 用户模式

	/*** 根据计算模式，设置最大计数时间, 计数模式 ***/
	if(COUNT_MODE_NORMAL == eCountMode)  
	{
		//样本分析，正常测试模式
		ulCountMaxTime = (uint32_t)COUNT_TIMEOUT; //
		eFunctionMode = FM_SAMPLE;        
	}else if(COUNT_MODE_QC_NORMAL == eCountMode || COUNT_MODE_QC_OUTSIDE == eCountMode){
		//质控分析（正常模式，外部工装单次模式）
		ulCountMaxTime = (uint32_t)COUNT_TIMEOUT; //
		eFunctionMode = FM_QC; 
	}else if(COUNT_MODE_QC_INNER_AUTO == eCountMode){
		//质控分析（内部工装自动模式）
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

	//获取分析模式
	nModePara = modePara(&modeParas, "AnalysisMode");
	
	//WBC模式判断
	i = am_hasWbcMode(nModePara);
	if ((am_hasWbcMode(nModePara)))
	{
		/*******************计数池等参数*************************/
		setSystemPara(&systemParas, "WbcCalFactor", MachInfo.calibration.fWBC, 2);  //校准系数
		setSystemPara(&systemParas, "WbcDilutionRatio", 501.0, 1);     //样本稀释倍数
		setSystemPara(&systemParas, "WbcVolumn", BC_COUNT_VOLUMN, 2);  //新款（2020.06.01之后）计数池体积为1046.26,  826.95 ，这个值根据计数池后池体积的变化而变化  !!!!!!!!
		setSystemPara(&systemParas, "WbcSmpRate", smpRate, 2);  //采样率
		setSystemPara(&systemParas, "WbcExpectedCountTime", BC_CELL_NORMAL_COUNT_TIME, 2);//单位s，正常测试对应的采样计数时间 11.6 , 这个值根据计数池后池体积的变化而变化  !!!!!!!!

		// MCU: clear buffer to zero
		wbcPulseInfo = (pulse_info_i_t*)SDRAM_ALGO_ADDR;
		memset((void*)wbcPulseInfo, 0, SDRAM_ALGO_LEN); //8M
		pulseInit(wbcPulseInfo);

		//特征初始化
		pulseMidFeatureInit(&midFeature);	
		midFeature.smpRate = smpRate;		

		//初始化控制结构体
		BC_Init(BC_WBC);  

		//工装测试，检测定量电极接触状态，高表示接下来将要发送信号，时间赋值
		ulStartTick = HAL_GetTick();//osKernelGetTickCount();
		ulEndTick   = ulStartTick;

		/*********************WBC数据采集、初步数据处理（获取脉冲信息）*********************/
		//时间赋值
		ulStartTick = HAL_GetTick();//osKernelGetTickCount();
		ulEndTick   = ulStartTick;
		ulTemp      = ulStartTick;
		ulTemp2     = ulStartTick;

		//初始压力
		lStartPress = Press_Value();
		//开液阀
		if(COUNT_MODE_NORMAL == eCountMode || COUNT_MODE_QC_NORMAL == eCountMode || COUNT_MODE_RE_USE == eCountMode || COUNT_MODE_100V_FIRE == eCountMode)      
		{
			LIQUID_WAVE_OPEN;
		}
		/*** 计时开始 ***/
		ulT4_Start =  HAL_GetTick();
		ulT4_End = ulT4_Start;
		pCountInfo->tSignalAnalysis.ulT4_Passed = 0;

		/*** 关闭系统调度 ***/
		osKernelLock();
		/*** 使能血细胞信号采集 ***/
		BC_Enable(BC_WBC);
		//已开启数据采集
		pCountInfo->tSignalAnalysis.eDataSW_Flag = TRUE; 

		/*** 执行数据采集过程（边吸液、边数据采集） ***/
		while(pCountInfo->tSignalAnalysis.ulT4_Passed + ulTicks <= ulCountMaxTime)
		{
			ulQuery++;
			ulT4_End = HAL_GetTick();//osKernelGetTickCount();             

			/*** 算法处理ADC采样数据，buffer1 ***/
			if(BC_ADC_PART_1 == g_stBC_Data[BC_WBC].ucFlag)
			{
				g_stBC_Data[BC_WBC].ucFlag = BC_ADC_PART_NONE;
				dataSize++;
				C_Num++;
				
				//指向后部分数据段 
				status = wbcPulseIdentify(wbcPulseInfo, &wbcPulseNum, (const uint8 *)&g_stBC_Data[BC_WBC].ucBuf[0], DATA_UINT_SIZE, &midFeature);
				if (!status)
				{
					BC_Disable(BC_WBC);
					eErrorCode = ERROR_CODE_ALGO_BREAK;
					break;
				} 
			} 
				
			/*** 算法处理ADC采样数据，buffer2 ***/
			if(BC_ADC_PART_2 == g_stBC_Data[BC_WBC].ucFlag)
			{
				g_stBC_Data[BC_WBC].ucFlag = BC_ADC_PART_NONE;
				dataSize++;
				C_Num++;
				//指向后部分数据段
				status = wbcPulseIdentify(wbcPulseInfo, &wbcPulseNum, (const uint8 *)&g_stBC_Data[BC_WBC].ucBuf[BC_ADC_HALF_BUF_LEN_2B], DATA_UINT_SIZE, &midFeature);
				if (!status)
				{
					BC_Disable(BC_WBC);
					eErrorCode = ERROR_CODE_ALGO_BREAK;
					break;
				} 
			}

			/*** 面板指示灯，计数状态 ***/
			if(ulT4_End - ulTemp2 >= MENU_LED_LIGHT_SHPERIOD) //500mv
			{
				ulTemp2 = ulT4_End;
				Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status);
			}

			/********** 定量电极触发，退出计数 *********/
			if(COUNT_MODE_NORMAL == eCountMode || COUNT_MODE_QC_NORMAL == eCountMode || COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO == eCountMode || COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE == eCountMode){ //正常/外部工装模式，检测定量电极
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

			/*** 打印小孔电压 ***/
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

			/*** 缓冲室压力检测及处理 ***/
			lPress = Press_Value();
			//正常/外部工装全流程，模式，检测压力
			if(lPress > AIR_LEAK_PRESS && (COUNT_MODE_NORMAL == eCountMode)) 
			{                         
				//关闭ADC采样，关阀
				BC_Disable(BC_WBC);
				LIQUID_WAVE_CLOSE;
				//未开启数据采集
				pCountInfo->tSignalAnalysis.eDataSW_Flag = FALSE; 

				//更新已执行的时间
				ulT4_End = HAL_GetTick();
				ulTicks = ulT4_End - ulT4_Start;
				pCountInfo->tSignalAnalysis.ulT4_Passed += ulTicks;
				ulTicks = 0;
				ulT4_Start = ulT4_End;

				//检测异常次数是否大于最大值3
				pCountInfo->tSignalAnalysis.ucBuildPress_Num++;
				if(pCountInfo->tSignalAnalysis.ucBuildPress_Num > MAX_PRESS_ERROR_NUM)
				{
					//异常次数过多
					eErrorCode = ERROR_CODE_AIR_LEAK_COUNTING_2; //返回错误，将在界面显示
					pCountInfo->tSignalAnalysis.eErrorCode = ERROR_CODE_AIR_LEAK_COUNTING_2; //计数过程中漏气（大于MAX_PRESS_ERROR_NUM）
					LOG_Error("4e: Press Err, Press=%d, ErrCode=%d, Num=%d, T4=%d", lPress, pCountInfo->tSignalAnalysis.eErrorCode, pCountInfo->tSignalAnalysis.ucBuildPress_Num, pCountInfo->tSignalAnalysis.ulT4_Passed);
					pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
					"4e: Press Err, Press=%d, ErrCode=%d, Num=%d, T4=%d;\r\n", lPress, pCountInfo->tSignalAnalysis.eErrorCode, pCountInfo->tSignalAnalysis.ucBuildPress_Num, pCountInfo->tSignalAnalysis.ulT4_Passed);

					break;
				}else{
					//计数过程中漏气（小于MAX_PRESS_ERROR_NUM）
					pCountInfo->tSignalAnalysis.eErrorCode = ERROR_CODE_AIR_LEAK_COUNTING_1;
					LOG_Error("4w: Press Err, Press=%d, HideErrCode=%d, Num=%d, T4=%d", lPress, pCountInfo->tSignalAnalysis.eErrorCode, pCountInfo->tSignalAnalysis.ucBuildPress_Num, pCountInfo->tSignalAnalysis.ulT4_Passed);
					pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
					"4w: Press Err, Press=%d, HideErrCode=%d, Num=%d, T4=%d;\r\n", lPress, pCountInfo->tSignalAnalysis.eErrorCode, pCountInfo->tSignalAnalysis.ucBuildPress_Num, pCountInfo->tSignalAnalysis.ulT4_Passed);                  
				}

				//重新建压一次
				if(ERROR_CODE_SUCCESS != Build_Press(RUN_MODE_COUNT, BC_TEST_PRESS_VALUE)){
					LOG_Error("4w: Build Press Fail, Press=%d, T4=%d", Press_Value(), pCountInfo->tSignalAnalysis.ulT4_Passed);
					pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
					"4w: Build Press Fail, Press=%d, T4=%d", Press_Value(), pCountInfo->tSignalAnalysis.ulT4_Passed);
				}

				//跳到最新的时间节点，开始计时
				ulT4_Start = HAL_GetTick();
				ulT4_End   = ulT4_Start;
			}


			//如果采样过程中有关闭数据采集，再开启
			if(pCountInfo->tSignalAnalysis.eDataSW_Flag == FALSE)
			{
				//开阀，采样
				LIQUID_WAVE_OPEN;
				BC_Enable(BC_WBC);
				pCountInfo->tSignalAnalysis.eDataSW_Flag = TRUE; 
			}   

			/*** 更新已执行的时间 ***/
			ulT4_End = HAL_GetTick();
			ulTicks = ulT4_End - ulT4_Start;
		}
		//停止血细胞信号采集
		BC_Disable(BC_WBC);
		//更新耗时
		pCountInfo->tSignalAnalysis.ulT4_Passed += ulTicks;      
		//恢复调度
		osKernelUnlock();
		//关液阀
		LIQUID_WAVE_CLOSE; 
		//面板指示灯，运行状态
		////        Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status);

		//结束压力值
		lEndPress = Press_Value();     

		//WBC数据赋值
		dataIn.wbcPulseInfo = wbcPulseInfo;
		dataIn.wbcPulseNum = wbcPulseNum; 	

		//小孔电压日志保存
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%s", "XK_V: ");
		for(i = 0; i < ucXK_V_Index; i++)
		{      
			//保存小孔电压信息到日志中
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%d,", usaXK_V_Buffer[i]);
		}
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "\r\n");

		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
		"4i: T2=%d, T4=%d, ErrCode=%d, StartPress=%d, EndPress=%d, XKV=%d, Elec=%d, eV=%d;\r\n", pCountInfo->tLiquidCreate.ulT2_Passed, pCountInfo->tSignalAnalysis.ulT4_Passed, pCountInfo->tSignalAnalysis.eErrorCode ,\
		lStartPress, lEndPress, XK_V(), Elec_Status, Elec_V());


		//在以下模式下有 开阀吸液操作，需关阀、异常处理
		if((COUNT_MODE_NORMAL == eCountMode && eErrorCode == ERROR_CODE_SUCCESS) || (COUNT_MODE_QC_NORMAL == eCountMode && eErrorCode == ERROR_CODE_SUCCESS))
		{  
			/*** 在信号采集阶段，开阀后，定量电极瞬间触发(防止有些使用过的血细胞模块，再次测试计入信号采集阶段) ***/
			if(pCountInfo->tSignalAnalysis.ulT4_Passed < 200)// )
			{
				eErrorCode = ERROR_CODE_ELEC_TRIGGLE_2;
				pCountInfo->tCountEnd.eErrorCode = eErrorCode;
				LOG_Error("4e: Elec Triggle, Elec=%d, ErrCode=%d", Elec_Status, pCountInfo->tCountEnd.eErrorCode);
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"4e: Elec Triggle, Elec=%d, ErrCode=%d;\r\n", Elec_Status, pCountInfo->tCountEnd.eErrorCode);
			} 

			/*** 计数时间小于8秒，待测样本量不足 ***/
			if(pCountInfo->tSignalAnalysis.ulT4_Passed < 8000)//  该参数后续需要修改
			{
				eErrorCode = ERROR_CODE_SAMPLE_NOT_ENOUGH;
				pCountInfo->tCountEnd.eErrorCode = eErrorCode;
				LOG_Error("4e: Sample Not Enough, ErrCode=%d", pCountInfo->tCountEnd.eErrorCode);
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"4e: Sample Not Enough, ErrCode=%d;\r\n", pCountInfo->tCountEnd.eErrorCode);
			}            

			/*** 计数时间大于15s，小于（最大计数时间 30s）***/
			if(pCountInfo->tSignalAnalysis.ulT4_Passed > 15000 && pCountInfo->tSignalAnalysis.ulT4_Passed < ulCountMaxTime)//  该参数后续需要修改
			{
				//
				pCountInfo->tCountEnd.eErrorCode = ERROR_CODE_T4_ERR_15S_2_MAXTIME; //T4时间过长（隐藏故障码1132）
				//如果在初始化阶段，小孔电压已导通，则此时"T4时间过长（隐藏故障码1132）" 改为 "初始化阶段，小孔电压导通 E1111", 即：有E1111带来的E1132
				if(pCountInfo->tCountInit.eErrorCode == ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE) 
				pCountInfo->tCountEnd.eErrorCode = ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE;

				LOG_Error("4e: T4 Err, T4=%d, HideErrCode=%d", pCountInfo->tSignalAnalysis.ulT4_Passed, pCountInfo->tCountEnd.eErrorCode);
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"4e: T4 Err, T4=%d, HideErrCode=%d;\r\n", pCountInfo->tSignalAnalysis.ulT4_Passed, pCountInfo->tCountEnd.eErrorCode);
			}     

			/*** 超时堵孔判断 ***/
			if(pCountInfo->tSignalAnalysis.ulT4_Passed >= ulCountMaxTime)
			{
				//超时
				eErrorCode = ERROR_CODE_TIMEOUT;
				pCountInfo->tSignalAnalysis.eErrorCode = eErrorCode;
				LOG_Error("4e: TimeOut, ErrCode=%d, T4=%d, Elec=%d, eV=%d", eErrorCode, pCountInfo->tSignalAnalysis.ulT4_Passed, Elec_Status, Elec_V());
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"4e: TimeOut, ErrCode=%d, T4=%d, Elec=%d, eV=%d;\r\n", eErrorCode, pCountInfo->tSignalAnalysis.ulT4_Passed, Elec_Status, Elec_V());
				return eErrorCode;
			}

			/***  异常处理(传给算法故障码) ***/    
			if(pCountInfo->tCountInit.eErrorCode == ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE){
				eToAlgoErrorCode = EM_1111; //EM111优先级比1112要高
			}else if(pCountInfo->tLiquidCreate.eErrorCode == ERROR_CODE_BSK_2S_15V){           
				eToAlgoErrorCode = EM_1112;
			}else if(pCountInfo->tSignalAnalysis.eErrorCode == ERROR_CODE_BSK_2S_15V){           
			}else if(pCountInfo->tSignalAnalysis.eErrorCode == ERROR_CODE_BSK_2S_15V){           
			}

			/**************************************** END *****************************************/ 
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, 
			"%s", "[EndCount]\r\n");  

			/*** 防漏液处理 ***/
			usXK_V = XK_V(); 
			if(usXK_V < XK_V_NORMAL_LOW_THRESHOLD || pCountInfo->tSignalAnalysis.ulT4_Passed > COUNT_TIMEOUT) //小孔电压过低、超时、计数池重复使用，防漏液处理
			{   //mv
				//防漏液处理（开液阀吸气，同时对接电极松开，如果气嘴处存在液体，会被吸入到缓存室）
				LIQUID_WAVE_OPEN;
				Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
				LIQUID_WAVE_CLOSE;
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"5w: Leakproof Fluid Treatment, XKV=%d, Press;\r\n", usXK_V, Press_Value());

			}else{
				//多吸液处理（短暂的多吸液，使定量电极完全浸没，防止在重复使用时，初始阶段定量电极未能计时触发）
				LIQUID_WAVE_OPEN;
				ulEndTick = HAL_GetTick();
				ulTemp   = ulEndTick;
				ulTemp2   = ulEndTick;
				while(ulEndTick - ulTemp < 2000) //2000ms
				{
					ulEndTick = HAL_GetTick();
					//再抽2s后关阀，确保定量电机完成浸没，面板指示灯继续运行
					if(ulEndTick - ulTemp2 >= MENU_LED_LIGHT_SHPERIOD) //500mv
					{
						ulTemp2 = ulEndTick;
						Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status);
					}
				}
				LIQUID_WAVE_CLOSE; //关液阀
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
				"5w: Multi Suction Liquid,  XKV=%d, Press=%d, T=%d;\r\n", XK_V(), Press_Value(), ulEndTick - ulTemp);
			}
		}else if((COUNT_MODE_NORMAL == eCountMode || COUNT_MODE_RE_USE == eCountMode || COUNT_MODE_100V_FIRE == eCountMode) && eErrorCode != ERROR_CODE_SUCCESS){
			//当前有 E1125， ERROR_CODE_ALGO_BREAK
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
			"5w: ErrCode=%d, XKV=%d, Press=%d, T=%d;\r\n", eErrorCode, XK_V(), Press_Value(), ulEndTick - ulTemp);

			if(COUNT_MODE_RE_USE == eCountMode)
			{
				//重复使用模式，多吸液处理，防漏液处理（开液阀吸气，同时对接电极松开，如果气嘴处存在液体，会被吸入到缓存室）
				LIQUID_WAVE_OPEN;
				Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
				LIQUID_WAVE_CLOSE;
			}
			return eErrorCode;
		}

		//
		if(COUNT_MODE_RE_USE == eCountMode)
		{
			//重复使用模式，多吸液处理，防漏液处理（开液阀吸气，同时对接电极松开，如果气嘴处存在液体，会被吸入到缓存室）
			LIQUID_WAVE_OPEN;
			Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
			LIQUID_WAVE_CLOSE;
		}

		/****************打印部分原始数据*********************/
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

		/*** 计数时间 ***/
		sprintf(ucaTimeBuffer, "%d.%d%d", pCountInfo->tSignalAnalysis.ulT4_Passed/1000, \
		(pCountInfo->tSignalAnalysis.ulT4_Passed%1000)/100, ((pCountInfo->tSignalAnalysis.ulT4_Passed%100)/10));

		countTime = (float)atof(ucaTimeBuffer); //采样时间（单位秒）, 时间值和实际不一致的话，会导致wbc结果偏差

		/*** 抽液时间 ***/
		ulTicks = pCountInfo->tLiquidCreate.ulT2_Passed + pCountInfo->tSignalAnalysis.ulT4_Passed;    
		sprintf(ucaTimeBuffer, "%d.%d%d", ulTicks/1000, (ulTicks%1000)/100, ((ulTicks%100)/10));
		volumnTime = (float)atof(ucaTimeBuffer);

		setSystemPara(&systemParas, "WbcCountTime", countTime, 2);
		setSystemPara(&systemParas, "WbcVolumnTime", volumnTime, 2);
		/*** 传递故障码给算法  ***/
		setModePara(&modeParas, "ErrorMode", eToAlgoErrorCode);//eEM_SUCCESS); // 故障模式(样本分析后，计算前)        

		/*** 传递时间参数给算法 ***/
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, 
		"5i: ToAlgoErrCode=%d, CountTime: %d, %0.2f, volumnTime: %d, %0.2f;\r\n", eToAlgoErrorCode, pCountInfo->tSignalAnalysis.ulT4_Passed, countTime, ulTicks, volumnTime);
	}//*** End WBC


	/*************  HGB   *************/
	if(am_hasHgbMode(nModePara))
	{
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "HGB ADC: ");
		//除了正常计数模式模式，在其他模式下零时采集HGB信号，并计算
		if(COUNT_MODE_NORMAL  == eCountMode || COUNT_MODE_QC_NORMAL  == eCountMode)
		{
			//WBC_HGB模式，在初始化阶段已采集HGB信号
			dataIn.hgbDataBuffer = (char*)pCountInfo->tCountInit.HGB535Adc;			//这里利用内存连续性，把415和535的ADC数据都传入此buf中了
		}else{
			//===采集HGB的ADC数据===
			uint16_t Temp = 0;
			
			BC_CUR_SW_OFF;
			
			//采集535波长数据
			Light_Start(HGBLED_535,MachInfo.paramSet.current535);
			osDelay(5000);
			
			//先采集535波长数据
			for(i = 0; i < 10; i++){
				Temp = Get_HGBAvgADC(10);
				
				pCountInfo->tCountInit.HGB535Adc[i*2]   = *(((uint8_t*)&Temp)+1);
				pCountInfo->tCountInit.HGB535Adc[i*2+1] = *(((uint8_t*)&Temp));
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%d", Temp);

				if(i == 3 || i == 8){
					Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status); //面板灯 
				}
			}
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "\r\n");
			
			//采集415波长数据
			Light_Start(HGBLED_415,MachInfo.paramSet.current415);
			osDelay(3000);
			
			//采集415波长数据
			for(i = 0; i < 10; i++){
				Temp = Get_HGBAvgADC(10);
				
				pCountInfo->tCountInit.HGB415Adc[i*2]   = *(((uint8_t*)&Temp)+1);
				pCountInfo->tCountInit.HGB415Adc[i*2+1] = *(((uint8_t*)&Temp));
				pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%d", Temp);

				if(i == 3 || i == 8){
					Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status); //面板灯 
				}
			}
			pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "\r\n");
			
			//关闭HGB灯
			Light_Stop();
			dataIn.hgbDataBuffer = (char*)pCountInfo->tCountInit.HGB535Adc;			//这里利用内存连续性，把415和535的ADC数据都传入此buf中了
		}

		/*** HGB算法初始化参数设置 ***/
		setModePara(&modeParas, "HgbMethodMode",HMM_DOUBLE_WAVE);
		setSystemPara(&systemParas, "HgbCalFactor",MachInfo.calibration.fHGB, 2);				//校准系数，由人工校准界面传入
		
		setSystemPara(&systemParas, "HgbDilutionRatio", HGB_DILUTION_RATIO, 1);					//试剂与血样的稀释比，固定501.0
		setSystemPara(&systemParas, "HgbBlankADC", HGB_BLACK_ADC_VALUE, 1);						//比色皿加入溶血剂的ADC值，固定3480.0

		// HGB单波长方法
		setSystemPara(&systemParas, "HgbCurveFit_K1", HGB_CURVE_FIT_K1, 2);						//拟合曲线斜率
		setSystemPara(&systemParas, "HgbCurveFit_B1", HGB_CURVE_FIT_B1, 2);						//拟合曲线截距
		// HGB双波长方法
		setSystemPara(&systemParas, "HgbCurveFit_K2", HGB_CURVE_FIT_K2, 2);						//拟合曲线斜率
		setSystemPara(&systemParas, "HgbCurveFit_B2", HGB_CURVE_FIT_B2, 2);						//拟合曲线截距
	} //*** END HGB	

	/****************************************************************************************************************************************************
	*
	*   算计计算
	*
	****************************************************************************************************************************************************/   


	dataSize = dataSize*DATA_UINT_SIZE;
	// 算法主程序	

	dataIn.modeParas = &modeParas;
	dataIn.systemParas = &systemParas;
	dataIn.configParas = &configParas;	//***
	dataOut.reportParas = &reportParas;
	dataOut.featureParas = &featureParas;
	dataOut.timeStamps	= timeStamps;
	dataOut.timeStampFlags	= timeStampFlags;
	dataOut.flagParas = &flagParas;
	dataOut.histList = &histList;

	/*** 算法计算  ***/
	Panel_Led_Status(RUN_MODE_NORMAL, g_ePanel_LED_Status);  //算法计算时，常亮，不闪耀 
	ulStartTick = HAL_GetTick();
	status = calculate(&dataOut, &dataIn); //////  error at this place
	ulEndTick = HAL_GetTick();

	/*** 算法信息参数（保存到日志）  ***/
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%s", "[Algo]\r\n");

	//算法版本，编译日期,wbc计算参数
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
	"alg_ver=%s, release_Date=%s, usersMode=%d, FunctionMode=%d,MaxCountTime=%0.2f;\r\n",\
	version(), versionDate(), usersMode, eFunctionMode, BC_WBC_DATA_SAMPLE_TIMEOUT);
			  
	/*** WBC 设置参数 ***/
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, 
	"M_WbcCalFactor=%0.2f, WbcCalFactor=%0.2f, WbcVolumn=%0.2f, WbcExpectedCountTime=%0.2f, ToAlgoErrCode=%d;\r\n", MachInfo.calibration.fWBC, systemPara(&systemParas, "WbcCalFactor", 0),BC_COUNT_VOLUMN, BC_CELL_NORMAL_COUNT_TIME, eToAlgoErrorCode);

	/*** HGB 计算参数 ***/
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
	"M_HgbCalFactor=%0.2f,HgbCalFactor=%0.2f, HgbBlankADC=%0.2f, HgbCurveFit_K1=%0.2f, HgbCurveFit_B1=%0.2f,HgbCurveFit_K2=%0.2f, HgbCurveFit_B2=%0.2f;",\
	MachInfo.calibration.fHGB, systemPara(&systemParas, "HgbCalFactor", 0),HGB_BLACK_ADC_VALUE, HGB_CURVE_FIT_K1, HGB_CURVE_FIT_B1,HGB_CURVE_FIT_K2, HGB_CURVE_FIT_B2);

	//HGB 特征数据
	hgbDataCV	= valueListDP(&featureParas, "HgbDataCV");			// HGB数据CV
	ratioHgbAve = valueListDP(&featureParas, "HgbTransmittance");	// HGB透光率
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
	"HGB Feature: hgbDataCV=%0.2f, ratioHgbAve=%.2f;\r\n", hgbDataCV, ratioHgbAve);

	/*******  质控 获取计数结果数据 **************************************/
	if(eCountMode == COUNT_MODE_QC_NORMAL || eCountMode == COUNT_MODE_QC_INNER_AUTO  || eCountMode == COUNT_MODE_QC_OUTSIDE)
	{ 
		/*********  质控计数数据 ************************************************************1********/

		QC_WBCHGB_TestInfo_t *pWBCHGB_TestInfo = (QC_WBCHGB_TestInfo_t*)pDataBuffer;   
		// 直方图
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

		/*获取WBC结果数据*/
		//最大值的fl值
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
		pWBCHGB_TestInfo->fLines[0] =  valueListDP(&reportParas, "Line1");  // 三条分割线当前是按照400的通道给出，不是256通道，需要转一下。;		
		//Line2
		pWBCHGB_TestInfo->fLines[1] =  valueListDP(&reportParas, "Line2");				
		//Line3
		pWBCHGB_TestInfo->fLines[2] =  valueListDP(&reportParas, "Line3");				
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
		"\r\nWBC=%0.2f, HGB=%0.2f(HGB1=%0.2f,HGB2=%0.2f, Gran#=%0.2f, MID#=%0.2f, LYM#=%0.2f, GranB=%0.2f, MidB=%0.2f, LymB=%0.2f, wbcMaxPos=%0.2f;\r\n", pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB],
		valueListDP(&featureParas,"HGB1"),valueListDP(&featureParas,"HGB2"),pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ],\
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB], pWBCHGB_TestInfo->fWbcMaxPos);


		// 报警信息输出
		pWBCPrompt = pWBCHGB_TestInfo->ucaWBC_ErrorPrompt;
		pHGBPrompt = pWBCHGB_TestInfo->ucaHGB_ErrorPrompt;
		for (i = 0; i<flagParas.num; i++)
		{
			if( flagParas.paras[i].flag )
			{
				if(LANGUAGE_ENGLISH == MachInfo.systemSet.eLanguage)
				{
					ucLanguage = 0; //1:中文，0：英文
				}else{
					ucLanguage = 1;
				}

				// WBC通道报警
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
				// HGB通道报警
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

		//设置结束标志
		pWBCHGB_TestInfo->ucaEndFlag[0] = '\r';
		pWBCHGB_TestInfo->ucaEndFlag[1] = '\n';    
	}else{
		/******** 样本分析，计数结果数据 *********************************************************3***/
		WBCHGB_TestInfo_t *pWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)pDataBuffer;

		// 直方图
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

		/*获取WBC结果数据*/
		//最大值的fl值
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
		pWBCHGB_TestInfo->fLines[0] =  valueListDP(&reportParas, "Line1");  // 三条分割线当前是按照400的通道给出，不是256通道，需要转一下。;		
		//Line2
		pWBCHGB_TestInfo->fLines[1] =  valueListDP(&reportParas, "Line2");				
		//Line3
		pWBCHGB_TestInfo->fLines[2] =  valueListDP(&reportParas, "Line3");				
		pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
		"WBC=%0.2f, HGB=%0.2f(HGB1=%0.2f,HGB2=%0.2f), Gran#=%0.2f, MID#=%0.2f, LYM#=%0.2f, GranB=%0.2f, MidB=%0.2f, LymB=%0.2f, wbcMaxPos=%0.2f;\r\n", pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB],
		valueListDP(&featureParas,"HGB1"),valueListDP(&featureParas,"HGB2"),pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ],\
		pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB], pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB], pWBCHGB_TestInfo->fWbcMaxPos);


		// 报警信息输出
		pWBCPrompt = pWBCHGB_TestInfo->ucaWBC_ErrorPrompt;
		pHGBPrompt = pWBCHGB_TestInfo->ucaHGB_ErrorPrompt;
		for (i = 0; i<flagParas.num; i++)
		{
			if( flagParas.paras[i].flag )
			{
				//
				if(LANGUAGE_ENGLISH == MachInfo.systemSet.eLanguage)
				{
					ucLanguage = 0; //1:中文，0：英文
				}else{
					ucLanguage = 1;
				}

				// WBC通道报警
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
				// HGB通道报警
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

		//设置结束标志
		pWBCHGB_TestInfo->ucaEndFlag[0] = '\r';
		pWBCHGB_TestInfo->ucaEndFlag[1] = '\n';
	} /********************************************************************************4*********/
	//调用算法计算，并已获取数据
	pCountInfo->ucUseAlgoFlag = 1;

	/*****************数据完整性评价******************/
	evaData( wbcPulseInfo, wbcPulseNum, &lossSum);
	
	/*** 脉冲个数 ***/
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
	"wbcPulseNum=%d, DataSize=%d;\r\n", wbcPulseNum, dataSize);

	/***************算法结果输出*****************/
	// 特征参数数组（加入日志）
	// 脉冲数据有效性
	isPulseValid = (int)( valueListDP(&featureParas, "IsPulseValid") + 0.5 );      
	pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
	  "6_i:isPulseValid=%d\r\n%s\r\n", isPulseValid, "[DataFeature]");                                            
	
	if(TRUE)
	{
		stampNum = *dataOut.stampNum;
		for (i= 0; i<stampNum; i++)
		{
			//timeStamps：单位时间内信号数量，timeStampFlags：表示是否参与计算，0：参与，1：不参与
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
		eErrorCode = ERROR_CODE_RE_CONN_2_T4_15S;      //液路建立阶段重对接次数>=2, 且T4>15s 
	}
	
	return eErrorCode;
}



