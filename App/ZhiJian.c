/***********************************************************************
*质检相关
************************************************************************/
#include "ZhiJian.h"
#include "main.h"
#include "bsp_outin.h"
#include "WM.h"
#include "Public_menuDLG.h"
#include "Common.h"
#include "alg.h"
#include "modedefs.h"
#include "algpulse.h"
#include "algplusdefs.h"
#include "modedefs.h"
#include "abcompute.h"
#include "DIALOG.h"
#include "HGB_Deal.h"
#include "bsp_light.h"
#include "process.h"



/*宏定义区*****************************************************************/
//质检页面控件ID
#define ID_TEXT_5    (SERVICE_XNCHECK_PAGE_ID + 0x12)
#define ID_TEXT_17    (SERVICE_XNCHECK_PAGE_ID + 0x2E)

#define ID_MULTIEDIT_0    (SERVICE_XNCHECK_PAGE_ID + 0x2C)

#define ID_TEXT_2    (SERVICE_XNCHECK_PAGE_ID + 0x0E)
#define ID_TEXT_3    (SERVICE_XNCHECK_PAGE_ID + 0x0F)
#define ID_TEXT_4    (SERVICE_XNCHECK_PAGE_ID + 0x10)

#define ID_TEXT_6    (SERVICE_XNCHECK_PAGE_ID + 0x1C)
#define ID_TEXT_7    (SERVICE_XNCHECK_PAGE_ID + 0x1D)
#define ID_TEXT_8    (SERVICE_XNCHECK_PAGE_ID + 0x1E)
#define ID_TEXT_9    (SERVICE_XNCHECK_PAGE_ID + 0x1F)
#define ID_TEXT_10    (SERVICE_XNCHECK_PAGE_ID + 0x20)
#define ID_TEXT_11    (SERVICE_XNCHECK_PAGE_ID + 0x21)

#define ID_TEXT_12    (SERVICE_XNCHECK_PAGE_ID + 0x27)
#define ID_TEXT_13    (SERVICE_XNCHECK_PAGE_ID + 0x28)
#define ID_TEXT_14    (SERVICE_XNCHECK_PAGE_ID + 0x29)
#define ID_TEXT_15    (SERVICE_XNCHECK_PAGE_ID + 0x2A)
#define ID_TEXT_16    (SERVICE_XNCHECK_PAGE_ID + 0x2B)

#define ID_BUTTON_1    (SERVICE_XNCHECK_PAGE_ID + 0x04)
#define ID_BUTTON_2    (SERVICE_XNCHECK_PAGE_ID + 0x05)
#define ID_BUTTON_3    (SERVICE_XNCHECK_PAGE_ID + 0x06)
#define ID_BUTTON_4    (SERVICE_XNCHECK_PAGE_ID + 0x07)
#define ID_BUTTON_5    (SERVICE_XNCHECK_PAGE_ID + 0x0B)
#define ID_BUTTON_6    (SERVICE_XNCHECK_PAGE_ID + 0x0C)
#define ID_BUTTON_7    (SERVICE_XNCHECK_PAGE_ID + 0x0D)
#define ID_BUTTON_8    (SERVICE_XNCHECK_PAGE_ID + 0x11)
#define ID_BUTTON_9    (SERVICE_XNCHECK_PAGE_ID + 0x19)
#define ID_BUTTON_10    (SERVICE_XNCHECK_PAGE_ID + 0x1A)
#define ID_BUTTON_11    (SERVICE_XNCHECK_PAGE_ID + 0x1B)
#define ID_BUTTON_12    (SERVICE_XNCHECK_PAGE_ID + 0x22)
#define ID_BUTTON_13    (SERVICE_XNCHECK_PAGE_ID + 0x23)
#define ID_BUTTON_14    (SERVICE_XNCHECK_PAGE_ID + 0x24)
#define ID_BUTTON_15    (SERVICE_XNCHECK_PAGE_ID + 0x25)
#define ID_BUTTON_16    (SERVICE_XNCHECK_PAGE_ID + 0x26)
#define ID_BUTTON_17    (SERVICE_XNCHECK_PAGE_ID + 0x2D)







/*内部函数区**************************************************************/
/***
*数据采集与算法调用，这里只采集信号数据并计算，不执行其他操作，如开阀，建压等操作
并且当需要计算HGB时，HGB数据要提前采集完毕
***/
static void _Algo_DataCollCalculate(enum emAnalysisMode AlgoMode,CountInfo_t* tCountInfo,WBCHGB_TestInfo_t* pWBCHGB_TestInfo)
{
	extern __IO BC_Data_t 	g_stBC_Data[BC_END];
	enum emErrorMode eToAlgoErrorCode = EM_SUCCESS;
	enum emFunctionMode eFunctionMode = FM_SAMPLE;
	__IO uint32_t  ulTemp = 0, ulTemp2 = 0, ulQuery = 0, ulCountMaxTime = 0, usersMode = 0;
	__IO uint32_t ulT4_Start =0, ulT4_End = 0, ulTicks = 0, ulStartTick = 0, ulEndTick = 0;
	uint16_t i = 0;
	char ucaTimeBuffer[20] = {0};
	uint8_t ucFlag = 0; 
	uint8_t Elec_Status = RESET;

	//算法参数定义
	// 参数列表
	nParaList* modeParas = (nParaList*)SDRAM_ApplyCommonBuf(sizeof(nParaList));	// 模式参数
	dParaList* systemParas = (dParaList*)SDRAM_ApplyCommonBuf(sizeof(dParaList));	// 系统参数
	dParaList* configParas = (dParaList*)SDRAM_ApplyCommonBuf(sizeof(dParaList));	// 配置参数 *
	dParaList* reportParas = (dParaList*)SDRAM_ApplyCommonBuf(sizeof(dParaList));	// 报告参数
	dParaList* featureParas = (dParaList*)SDRAM_ApplyCommonBuf(sizeof(dParaList));	// 特征参数（中间信息、中间状态）
	dParaList* flagParas = (dParaList*)SDRAM_ApplyCommonBuf(sizeof(dParaList));	// 报警参数
	histInforList* histList = (histInforList*)SDRAM_ApplyCommonBuf(sizeof(histInforList));	// 直方图信息//WBC/RBC/PLT

	//采集数据大小，Byte
	int dataSize = 0, C_Num = 0;

	// 中间状态信息初始化
	pulse_info_i_t *wbcPulseInfo;  	//脉冲信息buffer
	uint32 wbcPulseNum = 0;        	//脉冲个数
	double64 smpRate = BC_SAMPLE_RATE;
	pulse_mid_feature* midFeature = (pulse_mid_feature*)SDRAM_ApplyCommonBuf(sizeof(pulse_mid_feature));		
	BOOL8 status;       			//算法计算状态
	int nModePara = 0;     	 		//测量模式
	double countTime = 0;   		//计数时间
	double volumnTime = 0;  		//抽液时间

	int* timeStamps = (int*)SDRAM_ApplyCommonBuf(4*MAXDURATION);
	int* timeStampFlags = (int*)SDRAM_ApplyCommonBuf(4*MAXDURATION);

	/*****************算法输入输出****************************/
	alg_data_in* dataIn = (alg_data_in*)SDRAM_ApplyCommonBuf(sizeof(alg_data_in));
	alg_data_out* dataOut = (alg_data_out*)SDRAM_ApplyCommonBuf(sizeof(alg_data_out));

	// 结果输出
	histInfor *wbcDispHist = NULL;

	/******************算法初始化***********************/
	memset(dataIn, 0, sizeof(alg_data_in)); 
	memset(dataOut, 0, sizeof(alg_data_out)); 
	initParaListNP(modeParas);
	initParaListDP(systemParas);
	initParaListDP(reportParas);
	initParaListDP(configParas);	
	initParaListDP(featureParas);
	initParaListDP(flagParas);
	initHistList(histList);

	setModePara(modeParas, "AnalysisMode",		AlgoMode); //WBC HGB 模式
	setModePara(modeParas, "SamplingVialMode",	SVM_OPEN);
	setModePara(modeParas, "SpecimenMode",		SM_PVB);
	setModePara(modeParas, "SpeciesMode",		SPM_HUMAN);
	setModePara(modeParas, "AlgMode",			AM_RELEASE); //AM_DEBUG
	setModePara(modeParas, "RefGroupMode",		RGM_GENERAL);	 // 参考组模式 , 根据用户的选中设置

	//根据当前账号类型，设置算法的账号模式
	//cc内部账号
	usersMode = UM_EXPERT;
	setModePara(modeParas, "UsersMode",usersMode);

	/*** 根据计算模式，设置最大计数时间, 计数模式 ***/
	ulCountMaxTime = 20000;
	eFunctionMode = FM_TOOL;
	setModePara(modeParas, "FunctionMode", eFunctionMode);     //FM_SAMPLE

	//获取分析模式
	nModePara = modePara(modeParas, "AnalysisMode");
	
	//WBC模式数据采集与算法设置
	if(am_hasWbcMode(nModePara))
	{
		/*******************计数池等参数*************************/
		setSystemPara(systemParas, "WbcCalFactor", MachInfo.calibration.fWBC, 2);  //校准系数
		setSystemPara(systemParas, "WbcDilutionRatio", 501.0, 1);     //样本稀释倍数
		setSystemPara(systemParas, "WbcVolumn", BC_COUNT_VOLUMN, 2);  //新款（2020.06.01之后）计数池体积为1046.26,  826.95 ，这个值根据计数池后池体积的变化而变化  !!!!!!!!
		setSystemPara(systemParas, "WbcSmpRate", smpRate, 2);  //采样率
		setSystemPara(systemParas, "WbcExpectedCountTime", BC_CELL_NORMAL_COUNT_TIME, 2);//单位s，正常测试对应的采样计数时间 11.6 , 这个值根据计数池后池体积的变化而变化  !!!!!!!!

		//脉冲信息保存缓冲
		wbcPulseInfo = (pulse_info_i_t*)SDRAM_ALGO_ADDR;
		memset((void*)wbcPulseInfo, 0, SDRAM_ALGO_LEN); //8M
		pulseInit(wbcPulseInfo);

		//特征初始化
		pulseMidFeatureInit(midFeature);	
		midFeature->smpRate = smpRate;		

		//初始化控制结构体
		BC_Init(BC_WBC);  

		/*********************WBC数据采集、初步数据处理（获取脉冲信息）*********************/
		//时间赋值
		ulStartTick = HAL_GetTick();//osKernelGetTickCount();
		ulEndTick   = ulStartTick;
		ulTemp      = ulStartTick;
		ulTemp2     = ulStartTick;

		/*** 计时开始 ***/
		ulT4_Start =  HAL_GetTick();
		ulT4_End = ulT4_Start;
		tCountInfo->tSignalAnalysis.ulT4_Passed = 0;

		/*** 关闭系统调度 ***/
		osKernelLock();
		
		/*** 使能血细胞信号采集 ***/
		BC_Enable(BC_WBC);
		
		//已开启数据采集
		tCountInfo->tSignalAnalysis.eDataSW_Flag = TRUE; 

		/*** 执行数据采集过程（边吸液、边数据采集） ***/
		while(tCountInfo->tSignalAnalysis.ulT4_Passed + ulTicks <= ulCountMaxTime)
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
				status = wbcPulseIdentify(wbcPulseInfo, &wbcPulseNum, (const uint8 *)&g_stBC_Data[BC_WBC].ucBuf[0], DATA_UINT_SIZE, midFeature);
				if (!status)
				{
					BC_Disable(BC_WBC);
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
				status = wbcPulseIdentify(wbcPulseInfo, &wbcPulseNum, (const uint8 *)&g_stBC_Data[BC_WBC].ucBuf[BC_ADC_HALF_BUF_LEN_2B], DATA_UINT_SIZE, midFeature);
				if (!status)
				{
					BC_Disable(BC_WBC);
					break;
				} 
			}

			/*** 面板指示灯，计数状态 ***/
			if(ulT4_End - ulTemp2 >= 500) 
			{
				ulTemp2 = ulT4_End;
				Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status);
			}

			/********** 定量电极触发，退出计数 *********/
			Elec_Status = ELEC_STATUS;
			if(SET == Elec_Status)
			{
				BC_Disable(BC_WBC);
				break;
			}     

			/*** 更新已执行的时间 ***/
			ulT4_End = HAL_GetTick();
			ulTicks = ulT4_End - ulT4_Start;
		}
		
		//停止血细胞信号采集
		BC_Disable(BC_WBC);
		
		//更新耗时
		tCountInfo->tSignalAnalysis.ulT4_Passed += ulTicks;
		
		//恢复调度
		osKernelUnlock();

		//WBC数据赋值
		dataIn->wbcPulseInfo = wbcPulseInfo;
		dataIn->wbcPulseNum = wbcPulseNum; 	

		/*** 计数时间 ***/
		sprintf(ucaTimeBuffer, "%d.%d%d", tCountInfo->tSignalAnalysis.ulT4_Passed/1000, \
		(tCountInfo->tSignalAnalysis.ulT4_Passed%1000)/100, ((tCountInfo->tSignalAnalysis.ulT4_Passed%100)/10));

		countTime = (float)atof(ucaTimeBuffer); //采样时间（单位秒）, 时间值和实际不一致的话，会导致wbc结果偏差

		/*** 抽液时间 ***/
		ulTicks = tCountInfo->tLiquidCreate.ulT2_Passed + tCountInfo->tSignalAnalysis.ulT4_Passed;    
		sprintf(ucaTimeBuffer, "%d.%d%d", ulTicks/1000, (ulTicks%1000)/100, ((ulTicks%100)/10));
		volumnTime = (float)atof(ucaTimeBuffer);

		setSystemPara(systemParas, "WbcCountTime", countTime, 2);
		setSystemPara(systemParas, "WbcVolumnTime", volumnTime, 2);
		
		/*** 传递故障码给算法  ***/
		setModePara(modeParas, "ErrorMode", eToAlgoErrorCode); // 故障模式(样本分析后，计算前)

	}
	
	
	//HGB数据模式数据采集与算法设置
	if(am_hasHgbMode(nModePara))
	{
		//已采集HGB信号
		dataIn->hgbDataBuffer = (char*)tCountInfo->tCountInit.HGB535Adc;			//这里利用内存连续性，把415和535的ADC数据都传入此buf中了

		/*** HGB算法初始化参数设置 ***/
		setModePara(modeParas, "HgbMethodMode",HMM_DOUBLE_WAVE);
		setSystemPara(systemParas, "HgbCalFactor",MachInfo.calibration.fHGB, 2);				//校准系数，由人工校准界面传入
		
		setSystemPara(systemParas, "HgbDilutionRatio", HGB_DILUTION_RATIO, 1);					//试剂与血样的稀释比，固定501.0
		setSystemPara(systemParas, "HgbBlankADC", HGB_BLACK_ADC_VALUE, 1);						//比色皿加入溶血剂的ADC值，固定3480.0

		// HGB单波长方法
		setSystemPara(systemParas, "HgbCurveFit_K1", HGB_CURVE_FIT_K1, 2);						//拟合曲线斜率
		setSystemPara(systemParas, "HgbCurveFit_B1", HGB_CURVE_FIT_B1, 2);						//拟合曲线截距
		// HGB双波长方法
		setSystemPara(systemParas, "HgbCurveFit_K2", HGB_CURVE_FIT_K2, 2);						//拟合曲线斜率
		setSystemPara(systemParas, "HgbCurveFit_B2", HGB_CURVE_FIT_B2, 2);						//拟合曲线截距
	}

	dataSize = dataSize*DATA_UINT_SIZE;
	
	//===算法主程序===
	dataIn->modeParas = modeParas;
	dataIn->systemParas = systemParas;
	dataIn->configParas = configParas;
	dataOut->reportParas = reportParas;
	dataOut->featureParas = featureParas;
	dataOut->timeStamps	= timeStamps;
	dataOut->timeStampFlags	= timeStampFlags;
	dataOut->flagParas = flagParas;
	dataOut->histList = histList;

	/*** 算法计算  ***/
	Panel_Led_Status(RUN_MODE_NORMAL, g_ePanel_LED_Status);  //算法计算时，常亮，不闪耀 
	status = calculate(dataOut, dataIn);

	/******** 样本分析，计数结果数据 *********************************************************3***/
	// 直方图
	memset((void*)pWBCHGB_TestInfo->fWBC_Histogram, 0, sizeof(pWBCHGB_TestInfo->fWBC_Histogram));
	wbcDispHist = getHistInfo(histList, "WbcDispHistInfo");
	if (wbcDispHist)
	{
		for (i = 0; i < wbcDispHist->dataLen; i++)
		{
			pWBCHGB_TestInfo->fWBC_Histogram[i] = (float)wbcDispHist->datas[i];
			if ( i%5 == 0 || i == wbcDispHist->dataLen - 1 )
			{
			}
		}
	}

	/*获取WBC结果数据*/
	//最大值的fl值
	pWBCHGB_TestInfo->fWbcMaxPos = valueListDP(reportParas, "PeakX");
	pWBCHGB_TestInfo->fWbcMaxPos = (pWBCHGB_TestInfo->fWbcMaxPos + 1)*400/256;
	//wbc
	pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC]    =  valueListDP(reportParas, "WBC");
	ucFlag = flagListDP(reportParas, "WBC");
	pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_WBC] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_WBC, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC]);

	//GRAN#
	pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ] =  valueListDP(reportParas, "GRAN#");
	ucFlag = flagListDP(reportParas, "GRAN#");
	pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_GranJ] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_GranJ, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ]);

	//MID#
	pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ] =  valueListDP(reportParas, "MID#");
	ucFlag = flagListDP(reportParas, "MID#");    
	pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_MidJ] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_MidJ, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ]);

	//LYM#
	pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ] =  valueListDP(reportParas, "LYM#");
	ucFlag = flagListDP(reportParas, "LYM#");
	pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_LymJ] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_LymJ, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ]);

	//GRAN%
	pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB] =  valueListDP(reportParas, "GRAN%");
	ucFlag = flagListDP(reportParas, "GRAN%");
	pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_GranB] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_GranB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB]);

	//MID%
	pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB] =  valueListDP(reportParas, "MID%");
	ucFlag = flagListDP(reportParas, "MID%");
	pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_MidB] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_MidB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB]); 

	//LYM%
	pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB] =  valueListDP(reportParas, "LYM%");
	ucFlag = flagListDP(reportParas, "LYM%");
	pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_LymB] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_LymB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB]);    

	//HGB
	pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB]    =  valueListDP(reportParas, "HGB");
	ucFlag = flagListDP(reportParas, "HGB");
	pWBCHGB_TestInfo->ucaWBCHGB_RstAlert[WBCHGB_RST_HGB] =  WBCHGB_Alert_Flag(ucFlag, pWBCHGB_TestInfo->eReferGroup, WBCHGB_RST_HGB, pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB]);       

	//Line1
	pWBCHGB_TestInfo->fLines[0] =  valueListDP(reportParas, "Line1");  // 三条分割线当前是按照400的通道给出，不是256通道，需要转一下。;		
	//Line2
	pWBCHGB_TestInfo->fLines[1] =  valueListDP(reportParas, "Line2");				
	//Line3
	pWBCHGB_TestInfo->fLines[2] =  valueListDP(reportParas, "Line3");				

	//设置结束标志
	pWBCHGB_TestInfo->ucaEndFlag[0] = '\r';
	pWBCHGB_TestInfo->ucaEndFlag[1] = '\n';
}






/*函数区*****************************************************************/
/***
*执行WBC数字工装质检模式
***/
void ZJ_WBCGZTest(void)
{
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern __IO uint8_t g_CountHandle_Status;
	extern __IO WM_HWIN g_hActiveWin;
	WM_HWIN hItem;
	uint16_t i = 0;
	uint16_t j = 0;
	uint8_t ReturnFlag = 0;
	double TempD[10];
	double Mean1,Mean2,Mean3;
	double CV;
	char Str[100] = {0};
	uint16 Len = 0;
	
	//打开恒流源
	BC_CUR_SW_ON;
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//屏蔽进出仓按键
	Disable_Motor_Task();
	
	//仅进出仓电机进仓
	OutIn_Motor_In(MOTOR_MODE_NORMAL, MOTOR_OUTIN_MAX_STEP, MachInfo.paramSet.ulOutIn_In_AddStep);
	
	
	//重复执行，空白3次--精密度10次--校准15次--线性度15次
	for(i=0;i<43;i++){
		//探针电机对接
		Clamp_Motor_Out(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_Out_AddStep);
		
		/***************** 计数信息结构体初始化 ***********************/
		CountInfo_t* tCountInfo = (CountInfo_t*)SDRAM_ApplyCommonBuf(sizeof(CountInfo_t));
		CountInfo_Init(tCountInfo);
		   
		WBCHGB_TestInfo_t* pWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
		
		//当前操作者
		pWBCHGB_TestInfo->eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
		
		/*** 下一样本信息处理 ***/
		Get_NexSample_Info(pWBCHGB_TestInfo, &MachRunPara.tNextSample); 
		
		//纯外部工装信号测试，无流程
		uint32_t ulStartTick = 0, ulEndTick = 0;
		uint8_t ucElec_Status = 0;
		
		//定量电极状态应由：触发-->持续最多8s-->非触发，来表示开始发送信号
		if(ELEC_STATUS != SET){
			extern WM_HWIN DialogWin;
			CommonDialogPageData_s DialogPageData = {0};
			WM_MESSAGE pMsg;
			
			DialogPageData.bmpType = BMP_ALARM;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = NULL;
			
			strcpy(DialogPageData.str,"WBC工装定量电极异常1，中止测试");

			pMsg.MsgId = WM_USER_DATA;
			pMsg.Data.p = &DialogPageData;
			WM_SendMessage(DialogWin,&pMsg);
			
			//恒流源关
			BC_CUR_SW_OFF;
			
			//出仓
			OutIn_Module_Out(MOTOR_MODE_NORMAL);

			Enable_Motor_Task(TRUE);
			
			//关闭面板指示灯
			g_CountHandle_Status = 0;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
			
			return;
		}
		
		//5秒内，定量电极拉低，作为工装计数的计时起点
		ulStartTick = HAL_GetTick();
		ulEndTick = ulStartTick;
		while(ulEndTick - ulStartTick <= 9000)
		{
			ulEndTick = HAL_GetTick();
			
			ucElec_Status = ELEC_STATUS;
			
			if(RESET == ucElec_Status)
			{ 
				break;
			}
		}
		
		if(ulEndTick - ulStartTick >= 9000)
		{
			//五秒内定量电极未触发，则本次流程结束
			extern WM_HWIN DialogWin;
			CommonDialogPageData_s DialogPageData = {0};
			WM_MESSAGE pMsg;
			
			DialogPageData.bmpType = BMP_ALARM;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = NULL;
			
			strcpy(DialogPageData.str,"WBC工装定量电极异常2，中止测试");

			pMsg.MsgId = WM_USER_DATA;
			pMsg.Data.p = &DialogPageData;
			WM_SendMessage(DialogWin,&pMsg);
			
			//恒流源关
			BC_CUR_SW_OFF;
			
			//出仓
			OutIn_Module_Out(MOTOR_MODE_NORMAL);

			Enable_Motor_Task(TRUE);
			
			//关闭面板指示灯
			g_CountHandle_Status = 0;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
			
			return;
		}
		
		//调用算法
		_Algo_DataCollCalculate(AM_WBC,tCountInfo,pWBCHGB_TestInfo);
		
		//填充并判断数据是否合法
		if(i<3){
			//WBC空白--0/200
			MachRunPara.zj.wbcGZ.kb_0_200[j].wbc = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
			MachRunPara.zj.wbcGZ.kb_0_200[j].fl = pWBCHGB_TestInfo->fWbcMaxPos;
			
			if(MachRunPara.zj.wbcGZ.kb_0_200[j].wbc > 0.2f){
				ReturnFlag = 1;
				break;
			}
			
			j++;
			if(j==3){
				j = 0;
			}
		}else if(i<13){
			//WBC精密度--8/300
			MachRunPara.zj.wbcGZ.jmd_8_300[j].wbc = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
			MachRunPara.zj.wbcGZ.jmd_8_300[j].fl = pWBCHGB_TestInfo->fWbcMaxPos;
			
			if(MachRunPara.zj.wbcGZ.jmd_8_300[j].wbc > 9.0f || MachRunPara.zj.wbcGZ.jmd_8_300[j].wbc < 7.0f){
				ReturnFlag = 2;
				break;
			}
			
			j++;
			if(j==10){
				j = 0;
				
				TempD[0] = MachRunPara.zj.wbcGZ.jmd_8_300[0].wbc;
				TempD[1] = MachRunPara.zj.wbcGZ.jmd_8_300[1].wbc;
				TempD[2] = MachRunPara.zj.wbcGZ.jmd_8_300[2].wbc;
				TempD[3] = MachRunPara.zj.wbcGZ.jmd_8_300[3].wbc;
				TempD[4] = MachRunPara.zj.wbcGZ.jmd_8_300[4].wbc;
				TempD[5] = MachRunPara.zj.wbcGZ.jmd_8_300[5].wbc;
				TempD[6] = MachRunPara.zj.wbcGZ.jmd_8_300[6].wbc;
				TempD[7] = MachRunPara.zj.wbcGZ.jmd_8_300[7].wbc;
				TempD[8] = MachRunPara.zj.wbcGZ.jmd_8_300[8].wbc;
				TempD[9] = MachRunPara.zj.wbcGZ.jmd_8_300[9].wbc;
				
				CV = getCv_d(TempD,10,0);
				if(CV > 0.04){
					ReturnFlag = 3;
					break;
				}
				
				TempD[0] = MachRunPara.zj.wbcGZ.jmd_8_300[0].fl;
				TempD[1] = MachRunPara.zj.wbcGZ.jmd_8_300[1].fl;
				TempD[2] = MachRunPara.zj.wbcGZ.jmd_8_300[2].fl;
				TempD[3] = MachRunPara.zj.wbcGZ.jmd_8_300[3].fl;
				TempD[4] = MachRunPara.zj.wbcGZ.jmd_8_300[4].fl;
				TempD[5] = MachRunPara.zj.wbcGZ.jmd_8_300[5].fl;
				TempD[6] = MachRunPara.zj.wbcGZ.jmd_8_300[6].fl;
				TempD[7] = MachRunPara.zj.wbcGZ.jmd_8_300[7].fl;
				TempD[8] = MachRunPara.zj.wbcGZ.jmd_8_300[8].fl;
				TempD[9] = MachRunPara.zj.wbcGZ.jmd_8_300[9].fl;
				
				CV = getCv_d(TempD,10,0);
				if(CV > 0.04){
					ReturnFlag = 3;
					break;
				}
			}
		}else if(i<18){
			//WBC校准--5/300
			MachRunPara.zj.wbcGZ.jz_5_300[j].wbc = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
			MachRunPara.zj.wbcGZ.jz_5_300[j].fl = pWBCHGB_TestInfo->fWbcMaxPos;
			
			if(MachRunPara.zj.wbcGZ.jz_5_300[j].wbc > 6.0f || MachRunPara.zj.wbcGZ.jz_5_300[j].wbc < 4.0f){
				ReturnFlag = 4;
				break;
			}
			
			j++;
			if(j==5){
				j = 0;
			}
		}else if(i<23){
			//WBC校准--8/300
			MachRunPara.zj.wbcGZ.jz_8_300[j].wbc = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
			MachRunPara.zj.wbcGZ.jz_8_300[j].fl = pWBCHGB_TestInfo->fWbcMaxPos;
			
			if(MachRunPara.zj.wbcGZ.jz_8_300[j].wbc > 9.0f || MachRunPara.zj.wbcGZ.jz_8_300[j].wbc < 7.0f){
				ReturnFlag = 5;
				break;
			}
			
			j++;
			if(j==5){
				j = 0;
			}
		}else if(i<28){
			//WBC校准--10/300
			MachRunPara.zj.wbcGZ.jz_10_300[j].wbc = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
			MachRunPara.zj.wbcGZ.jz_10_300[j].fl = pWBCHGB_TestInfo->fWbcMaxPos;
			
			if(MachRunPara.zj.wbcGZ.jz_10_300[j].wbc > 11.0f || MachRunPara.zj.wbcGZ.jz_10_300[j].wbc < 9.0f){
				ReturnFlag = 6;
				break;
			}
			
			j++;
			if(j==5){
				j = 0;
				
				TempD[0] = MachRunPara.zj.wbcGZ.jz_5_300[0].wbc;
				TempD[1] = MachRunPara.zj.wbcGZ.jz_5_300[1].wbc;
				TempD[2] = MachRunPara.zj.wbcGZ.jz_5_300[2].wbc;
				TempD[3] = MachRunPara.zj.wbcGZ.jz_5_300[3].wbc;
				TempD[4] = MachRunPara.zj.wbcGZ.jz_5_300[4].wbc;
				Mean1 = getMean_d(TempD,5);
				
				TempD[0] = MachRunPara.zj.wbcGZ.jz_8_300[0].wbc;
				TempD[1] = MachRunPara.zj.wbcGZ.jz_8_300[1].wbc;
				TempD[2] = MachRunPara.zj.wbcGZ.jz_8_300[2].wbc;
				TempD[3] = MachRunPara.zj.wbcGZ.jz_8_300[3].wbc;
				TempD[4] = MachRunPara.zj.wbcGZ.jz_8_300[4].wbc;
				Mean2 = getMean_d(TempD,5);
				
				TempD[0] = MachRunPara.zj.wbcGZ.jz_10_300[0].wbc;
				TempD[1] = MachRunPara.zj.wbcGZ.jz_10_300[1].wbc;
				TempD[2] = MachRunPara.zj.wbcGZ.jz_10_300[2].wbc;
				TempD[3] = MachRunPara.zj.wbcGZ.jz_10_300[3].wbc;
				TempD[4] = MachRunPara.zj.wbcGZ.jz_10_300[4].wbc;
				Mean3 = getMean_d(TempD,5);
				
				MachInfo.calibration.fWBC = (MachInfo.calibration.fWBC*5.0f/Mean1+MachInfo.calibration.fWBC*8.0f/Mean2+MachInfo.calibration.fWBC*10.0f/Mean3)/3.0f;
				if(MachInfo.calibration.fWBC<0.75f || MachInfo.calibration.fWBC>1.25f){
					ReturnFlag = 7;
					break;
				}
			}
		}else if(i<31){
			//WBC线性度--0.1/200
			MachRunPara.zj.wbcGZ.xxd_0_1_200[j].wbc = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
			MachRunPara.zj.wbcGZ.xxd_0_1_200[j].fl = pWBCHGB_TestInfo->fWbcMaxPos;
			
			if(MachRunPara.zj.wbcGZ.xxd_0_1_200[j].wbc > 0.5f){
				ReturnFlag = 8;
				break;
			}
			
			j++;
			if(j==3){
				j = 0;
			}
		}else if(i<34){
			//WBC线性度--25/200
			MachRunPara.zj.wbcGZ.xxd_25_200[j].wbc = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
			MachRunPara.zj.wbcGZ.xxd_25_200[j].fl = pWBCHGB_TestInfo->fWbcMaxPos;
			
			if(MachRunPara.zj.wbcGZ.xxd_25_200[j].wbc > 27.0f || MachRunPara.zj.wbcGZ.xxd_25_200[j].wbc < 23.0f){
				ReturnFlag = 9;
				break;
			}
			
			j++;
			if(j==3){
				j = 0;
			}
		}else if(i<37){
			//WBC线性度--50/200
			MachRunPara.zj.wbcGZ.xxd_50_200[j].wbc = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
			MachRunPara.zj.wbcGZ.xxd_50_200[j].fl = pWBCHGB_TestInfo->fWbcMaxPos;
			
			if(MachRunPara.zj.wbcGZ.xxd_50_200[j].wbc > 52.0f || MachRunPara.zj.wbcGZ.xxd_50_200[j].wbc < 48.0f){
				ReturnFlag = 10;
				break;
			}
			
			j++;
			if(j==3){
				j = 0;
			}
		}else if(i<40){
			//WBC线性度--75/200
			MachRunPara.zj.wbcGZ.xxd_75_200[j].wbc = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
			MachRunPara.zj.wbcGZ.xxd_75_200[j].fl = pWBCHGB_TestInfo->fWbcMaxPos;
			
			if(MachRunPara.zj.wbcGZ.xxd_75_200[j].wbc > 77.0f || MachRunPara.zj.wbcGZ.xxd_75_200[j].wbc < 73.0f){
				ReturnFlag = 11;
				break;
			}
			
			j++;
			if(j==3){
				j = 0;
			}
		}else{
			//WBC线性度--100/200
			MachRunPara.zj.wbcGZ.xxd_100_200[j].wbc = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
			MachRunPara.zj.wbcGZ.xxd_100_200[j].fl = pWBCHGB_TestInfo->fWbcMaxPos;
			
			if(MachRunPara.zj.wbcGZ.xxd_100_200[j].wbc > 103.0f || MachRunPara.zj.wbcGZ.xxd_100_200[j].wbc < 97.0f){
				ReturnFlag = 12;
				break;
			}
			
			j++;
			if(j==3){
				j = 0;
			}
		}
		
		//探针电机松开
		Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
	}
	
	//判断是否测试中出错
	if(ReturnFlag != 0){
		extern WM_HWIN DialogWin;
		CommonDialogPageData_s DialogPageData = {0};
		WM_MESSAGE pMsg;
		
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = HAS_CONFIM_BUTTON;
		DialogPageData.fun = NULL;
		
		switch(ReturnFlag){
			case 1:strcpy(DialogPageData.str,"WBC工装空白超范围，中止测试");break;
			case 2:strcpy(DialogPageData.str,"WBC工装精密度超范围，中止测试");break;
			case 3:strcpy(DialogPageData.str,"WBC工装精密度CV超范围，中止测试");break;
			case 4:strcpy(DialogPageData.str,"WBC工装校准5.0超范围，中止测试");break;
			case 5:strcpy(DialogPageData.str,"WBC工装校准8.0超范围，中止测试");break;
			case 6:strcpy(DialogPageData.str,"WBC工装校准10.0超范围，中止测试");break;
			case 7:strcpy(DialogPageData.str,"WBC工装校准系数超范围，中止测试");break;
			case 8:strcpy(DialogPageData.str,"WBC工装线性0.1超范围，中止测试");break;
			case 9:strcpy(DialogPageData.str,"WBC工装线性25超范围，中止测试");break;
			case 10:strcpy(DialogPageData.str,"WBC工装线性50超范围，中止测试");break;
			case 11:strcpy(DialogPageData.str,"WBC工装线性75超范围，中止测试");break;
			case 12:strcpy(DialogPageData.str,"WBC工装线性100超范围，中止测试");break;
			default:break;
		}

		pMsg.MsgId = WM_USER_DATA;
		pMsg.Data.p = &DialogPageData;
		WM_SendMessage(DialogWin,&pMsg);
		
		//恒流源关
		BC_CUR_SW_OFF;
		
		//出仓
		OutIn_Module_Out(MOTOR_MODE_NORMAL);

		Enable_Motor_Task(TRUE);
		
		//关闭面板指示灯
		g_CountHandle_Status = 0;
		g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
		
		return;
	}
	
	//恒流源关
	BC_CUR_SW_OFF;
	
	//出仓
	OutIn_Module_Out(MOTOR_MODE_NORMAL);

	Enable_Motor_Task(TRUE);
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//刷新界面显示内容
	//更新WBC校准系数显示
	hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_17);
	Len = sprintf(Str,"%.2f",MachInfo.calibration.fWBC);
	Str[Len] = '\0';
	TEXT_SetText(hItem,Str);
	
	hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
	MULTIEDIT_AddText(hItem,"WBC测试：\r\n");
	MULTIEDIT_AddText(hItem,"WBC空白测试完毕\r\n");
	MULTIEDIT_AddText(hItem,"WBC精密度测试完毕\r\n");
	MULTIEDIT_AddText(hItem,"WBC校准测试完毕\r\n");
	MULTIEDIT_AddText(hItem,"WBC线性度测试完毕\r\n");
	MULTIEDIT_AddText(hItem,"请确认WBC校准系数是否合适？\r\n");
	MULTIEDIT_AddText(hItem,"\r\nHGB测试：\r\n");
	
	//解锁下一个按钮
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_17));
	
	//向界面发送后端执行完毕消息
	WM_SendMessageNoPara(g_hActiveWin,WM_ZJ_TEST_OVER);
}




/***
*执行HGB质检模式--空白
***/
void ZJ_HGB_KB(void)
{
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern __IO uint8_t g_CountHandle_Status;
	extern __IO WM_HWIN g_hActiveWin;
	WM_HWIN hItem;
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t Temp = 0;
	
	//确保关闭恒流源
	BC_CUR_SW_OFF;
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//屏蔽进出仓按键
	Disable_Motor_Task();
	
	//进仓
	OutIn_Module_In(MOTOR_MODE_NORMAL);
	
	//重复执行空白3次
	for(i=0;i<3;i++){
		/***************** 计数信息结构体初始化 ***********************/
		CountInfo_t* tCountInfo = (CountInfo_t*)SDRAM_ApplyCommonBuf(sizeof(CountInfo_t));
		CountInfo_Init(tCountInfo);
		   
		WBCHGB_TestInfo_t* pWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
		
		//当前操作者
		pWBCHGB_TestInfo->eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
		
		/*** 下一样本信息处理 ***/
		Get_NexSample_Info(pWBCHGB_TestInfo, &MachRunPara.tNextSample); 
		
		
		//采集535波长数据
		Light_Start(HGBLED_535,MachInfo.paramSet.current535);
		osDelay(3000);
		
		//先采集535波长数据
		for(j = 0; j < 10; j++){
			Temp = Get_HGBAvgADC(10);
			
			tCountInfo->tCountInit.HGB535Adc[j*2]   = *(((uint8_t*)&Temp)+1);
			tCountInfo->tCountInit.HGB535Adc[j*2+1] = *(((uint8_t*)&Temp));
		}
		
		//采集415波长数据
		Light_Start(HGBLED_415,MachInfo.paramSet.current415);
		osDelay(3000);
		
		//采集415波长数据
		for(j = 0; j < 10; j++){
			Temp = Get_HGBAvgADC(10);
			
			tCountInfo->tCountInit.HGB415Adc[j*2]   = *(((uint8_t*)&Temp)+1);
			tCountInfo->tCountInit.HGB415Adc[j*2+1] = *(((uint8_t*)&Temp));
		}
		
		//关闭HGB灯
		Light_Stop();
		
		//调用算法
		_Algo_DataCollCalculate(AM_HGB,tCountInfo,pWBCHGB_TestInfo);
		
		//填充数据
		MachRunPara.zj.hgb.kb[i] = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB];
		
		//判断数据是否合法
		if(MachRunPara.zj.hgb.kb[i] > 1.0f){
			extern WM_HWIN DialogWin;
			CommonDialogPageData_s DialogPageData = {0};
			WM_MESSAGE pMsg;
			
			DialogPageData.bmpType = BMP_ALARM;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = NULL;
			
			strcpy(DialogPageData.str,"HGB空白测试异常，中止测试");

			pMsg.MsgId = WM_USER_DATA;
			pMsg.Data.p = &DialogPageData;
			WM_SendMessage(DialogWin,&pMsg);
			
			//出仓
			OutIn_Module_Out(MOTOR_MODE_NORMAL);

			Enable_Motor_Task(TRUE);
			
			//关闭面板指示灯
			g_CountHandle_Status = 0;
			g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
			
			return;
		}
	}
	
	//出仓
	OutIn_Module_Out(MOTOR_MODE_NORMAL);

	Enable_Motor_Task(TRUE);
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//刷新界面显示内容
	hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
	MULTIEDIT_AddText(hItem,"HGB空白测试完毕\r\n");
	
	//解锁下一个按钮
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_1));
	
	//向界面发送后端执行完毕消息
	WM_SendMessageNoPara(g_hActiveWin,WM_ZJ_TEST_OVER);
}




/***
*执行HGB质检模式--精密度
***/
void ZJ_HGB_JMD(void)
{
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern __IO uint8_t g_CountHandle_Status;
	extern __IO WM_HWIN g_hActiveWin;
	WM_HWIN hItem;
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t Temp = 0;
	double TempD[10];
	double CV;
	
	//确保关闭恒流源
	BC_CUR_SW_OFF;
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//屏蔽进出仓按键
	Disable_Motor_Task();
	
	//进仓
	OutIn_Module_In(MOTOR_MODE_NORMAL);
	
	//重复执行精密度10次
	for(i=0;i<10;i++){
		/***************** 计数信息结构体初始化 ***********************/
		CountInfo_t* tCountInfo = (CountInfo_t*)SDRAM_ApplyCommonBuf(sizeof(CountInfo_t));
		CountInfo_Init(tCountInfo);
		   
		WBCHGB_TestInfo_t* pWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
		
		//当前操作者
		pWBCHGB_TestInfo->eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
		
		/*** 下一样本信息处理 ***/
		Get_NexSample_Info(pWBCHGB_TestInfo, &MachRunPara.tNextSample); 
		
		
		//采集535波长数据
		Light_Start(HGBLED_535,MachInfo.paramSet.current535);
		osDelay(3000);
		
		//先采集535波长数据
		for(j = 0; j < 10; j++){
			Temp = Get_HGBAvgADC(10);
			
			tCountInfo->tCountInit.HGB535Adc[j*2]   = *(((uint8_t*)&Temp)+1);
			tCountInfo->tCountInit.HGB535Adc[j*2+1] = *(((uint8_t*)&Temp));
		}
		
		//采集415波长数据
		Light_Start(HGBLED_415,MachInfo.paramSet.current415);
		osDelay(3000);
		
		//采集415波长数据
		for(j = 0; j < 10; j++){
			Temp = Get_HGBAvgADC(10);
			
			tCountInfo->tCountInit.HGB415Adc[j*2]   = *(((uint8_t*)&Temp)+1);
			tCountInfo->tCountInit.HGB415Adc[j*2+1] = *(((uint8_t*)&Temp));
		}
		
		//关闭HGB灯
		Light_Stop();
		
		//调用算法
		_Algo_DataCollCalculate(AM_HGB,tCountInfo,pWBCHGB_TestInfo);
		
		//填充数据
		MachRunPara.zj.hgb.jmd[i] = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB];
	}
	
	//判断数据是否合法
	TempD[0] = MachRunPara.zj.hgb.jmd[0];
	TempD[1] = MachRunPara.zj.hgb.jmd[1];
	TempD[2] = MachRunPara.zj.hgb.jmd[2];
	TempD[3] = MachRunPara.zj.hgb.jmd[3];
	TempD[4] = MachRunPara.zj.hgb.jmd[4];
	TempD[5] = MachRunPara.zj.hgb.jmd[5];
	TempD[6] = MachRunPara.zj.hgb.jmd[6];
	TempD[7] = MachRunPara.zj.hgb.jmd[7];
	TempD[8] = MachRunPara.zj.hgb.jmd[8];
	TempD[9] = MachRunPara.zj.hgb.jmd[9];
	
	CV = getCv_d(TempD,10,0);
	if(CV > 0.02){
		extern WM_HWIN DialogWin;
		CommonDialogPageData_s DialogPageData = {0};
		WM_MESSAGE pMsg;
		
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = HAS_CONFIM_BUTTON;
		DialogPageData.fun = NULL;
		
		strcpy(DialogPageData.str,"HGB精密度测试异常，中止测试");

		pMsg.MsgId = WM_USER_DATA;
		pMsg.Data.p = &DialogPageData;
		WM_SendMessage(DialogWin,&pMsg);
		
		//出仓
		OutIn_Module_Out(MOTOR_MODE_NORMAL);

		Enable_Motor_Task(TRUE);
		
		//关闭面板指示灯
		g_CountHandle_Status = 0;
		g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
		
		return;
	}
	
	//出仓
	OutIn_Module_Out(MOTOR_MODE_NORMAL);

	Enable_Motor_Task(TRUE);
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//刷新界面显示内容
	hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
	MULTIEDIT_AddText(hItem,"HGB精密度测试完毕\r\n");
	
	//解锁下一个按钮
	WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_5));
	
	//向界面发送后端执行完毕消息
	WM_SendMessageNoPara(g_hActiveWin,WM_ZJ_TEST_OVER);
}




/***
*执行HGB质检模式--校准
***/
void ZJ_HGB_JZ(ZJ_HGBJZ_e ZJHGBjzMode)
{
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern __IO uint8_t g_CountHandle_Status;
	extern __IO WM_HWIN g_hActiveWin;
	WM_HWIN hItem;
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t Temp = 0;
	char Str[50] = {0};
	uint8_t Len = 0;
	float HGBAvg;
	double TempD[10];
	double Mean1,Mean2,Mean3;
	
	//确保关闭恒流源
	BC_CUR_SW_OFF;
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//屏蔽进出仓按键
	Disable_Motor_Task();
	
	//进仓
	OutIn_Module_In(MOTOR_MODE_NORMAL);
	
	//重复执行校准5次
	for(i=0;i<5;i++){
		/***************** 计数信息结构体初始化 ***********************/
		CountInfo_t* tCountInfo = (CountInfo_t*)SDRAM_ApplyCommonBuf(sizeof(CountInfo_t));
		CountInfo_Init(tCountInfo);
		   
		WBCHGB_TestInfo_t* pWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
		
		//当前操作者
		pWBCHGB_TestInfo->eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
		
		/*** 下一样本信息处理 ***/
		Get_NexSample_Info(pWBCHGB_TestInfo, &MachRunPara.tNextSample); 
		
		
		//采集535波长数据
		Light_Start(HGBLED_535,MachInfo.paramSet.current535);
		osDelay(3000);
		
		//先采集535波长数据
		for(j = 0; j < 10; j++){
			Temp = Get_HGBAvgADC(10);
			
			tCountInfo->tCountInit.HGB535Adc[j*2]   = *(((uint8_t*)&Temp)+1);
			tCountInfo->tCountInit.HGB535Adc[j*2+1] = *(((uint8_t*)&Temp));
		}
		
		//采集415波长数据
		Light_Start(HGBLED_415,MachInfo.paramSet.current415);
		osDelay(3000);
		
		//采集415波长数据
		for(j = 0; j < 10; j++){
			Temp = Get_HGBAvgADC(10);
			
			tCountInfo->tCountInit.HGB415Adc[j*2]   = *(((uint8_t*)&Temp)+1);
			tCountInfo->tCountInit.HGB415Adc[j*2+1] = *(((uint8_t*)&Temp));
		}
		
		//关闭HGB灯
		Light_Stop();
		
		//调用算法
		_Algo_DataCollCalculate(AM_HGB,tCountInfo,pWBCHGB_TestInfo);
		
		//填充数据
		MachRunPara.zj.hgb.jz[ZJHGBjzMode].value[i] = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB];
	}
	
	//出仓
	OutIn_Module_Out(MOTOR_MODE_NORMAL);

	Enable_Motor_Task(TRUE);
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//刷新界面显示内容
	//更新HGB校准结果数据显示（平均值）
	HGBAvg = 0.0f;
	for(i=0;i<5;i++){
		HGBAvg += MachRunPara.zj.hgb.jz[ZJHGBjzMode].value[i];
	}
	HGBAvg = HGBAvg/5.0f;
	
	switch(ZJHGBjzMode){
		case ZJ_HGBJZ_1:
		{
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_2);
			Len = sprintf(Str,"%.2f",HGBAvg);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			MULTIEDIT_AddText(hItem,"HGB校准1测试完毕\r\n");
			
			//解锁下一个按钮
			WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_6));
		}break;
		case ZJ_HGBJZ_2:
		{
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_3);
			Len = sprintf(Str,"%.2f",HGBAvg);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			MULTIEDIT_AddText(hItem,"HGB校准2测试完毕\r\n");
			
			//解锁下一个按钮
			WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_7));
		}break;
		case ZJ_HGBJZ_3:
		{
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_4);
			Len = sprintf(Str,"%.2f",HGBAvg);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			MULTIEDIT_AddText(hItem,"HGB校准3测试完毕\r\n");
			MULTIEDIT_AddText(hItem,"请确认HGB校准系数是否合适？\r\n");
			
			//计算HGB校准系数值
			TempD[0] = MachRunPara.zj.hgb.jz[0].value[0];
			TempD[1] = MachRunPara.zj.hgb.jz[0].value[1];
			TempD[2] = MachRunPara.zj.hgb.jz[0].value[2];
			TempD[3] = MachRunPara.zj.hgb.jz[0].value[3];
			TempD[4] = MachRunPara.zj.hgb.jz[0].value[4];
			Mean1 = getMean_d(TempD,5);
			
			TempD[0] = MachRunPara.zj.hgb.jz[1].value[0];
			TempD[1] = MachRunPara.zj.hgb.jz[1].value[1];
			TempD[2] = MachRunPara.zj.hgb.jz[1].value[2];
			TempD[3] = MachRunPara.zj.hgb.jz[1].value[3];
			TempD[4] = MachRunPara.zj.hgb.jz[1].value[4];
			Mean2 = getMean_d(TempD,5);
			
			TempD[0] = MachRunPara.zj.hgb.jz[2].value[0];
			TempD[1] = MachRunPara.zj.hgb.jz[2].value[1];
			TempD[2] = MachRunPara.zj.hgb.jz[2].value[2];
			TempD[3] = MachRunPara.zj.hgb.jz[2].value[3];
			TempD[4] = MachRunPara.zj.hgb.jz[2].value[4];
			Mean3 = getMean_d(TempD,5);
			
			MachInfo.calibration.fHGB = (MachInfo.calibration.fHGB*MachRunPara.zj.hgb.jz[0].dest/Mean1+MachInfo.calibration.fHGB*MachRunPara.zj.hgb.jz[1].dest/Mean2+MachInfo.calibration.fHGB*MachRunPara.zj.hgb.jz[2].dest/Mean3)/3.0f;
			//更新HGB校准系数显示
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_5);
			Len = sprintf(Str,"%.2f",MachInfo.calibration.fHGB);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			//解锁下一个按钮
			WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_8));
		}break;
		default :break;
	}
	
	
	//向界面发送后端执行完毕消息
	WM_SendMessageNoPara(g_hActiveWin,WM_ZJ_TEST_OVER);
}



/***
*执行HGB质检模式--线性
***/
void ZJ_HGB_XX(ZJ_HGBXX_e ZJHGBxxMode)
{
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern __IO uint8_t g_CountHandle_Status;
	extern __IO WM_HWIN g_hActiveWin;
	WM_HWIN hItem;
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t Temp = 0;
	char Str[50] = {0};
	uint8_t Len = 0;
	float HGBAvg;
	
	//确保关闭恒流源
	BC_CUR_SW_OFF;
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//屏蔽进出仓按键
	Disable_Motor_Task();
	
	//进仓
	OutIn_Module_In(MOTOR_MODE_NORMAL);
	
	//重复执行校准3次
	for(i=0;i<3;i++){
		/***************** 计数信息结构体初始化 ***********************/
		CountInfo_t* tCountInfo = (CountInfo_t*)SDRAM_ApplyCommonBuf(sizeof(CountInfo_t));
		CountInfo_Init(tCountInfo);
		   
		WBCHGB_TestInfo_t* pWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
		
		//当前操作者
		pWBCHGB_TestInfo->eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
		
		/*** 下一样本信息处理 ***/
		Get_NexSample_Info(pWBCHGB_TestInfo, &MachRunPara.tNextSample); 
		
		
		//采集535波长数据
		Light_Start(HGBLED_535,MachInfo.paramSet.current535);
		osDelay(3000);
		
		//先采集535波长数据
		for(j = 0; j < 10; j++){
			Temp = Get_HGBAvgADC(10);
			
			tCountInfo->tCountInit.HGB535Adc[j*2]   = *(((uint8_t*)&Temp)+1);
			tCountInfo->tCountInit.HGB535Adc[j*2+1] = *(((uint8_t*)&Temp));
		}
		
		//采集415波长数据
		Light_Start(HGBLED_415,MachInfo.paramSet.current415);
		osDelay(3000);
		
		//采集415波长数据
		for(j = 0; j < 10; j++){
			Temp = Get_HGBAvgADC(10);
			
			tCountInfo->tCountInit.HGB415Adc[j*2]   = *(((uint8_t*)&Temp)+1);
			tCountInfo->tCountInit.HGB415Adc[j*2+1] = *(((uint8_t*)&Temp));
		}
		
		//关闭HGB灯
		Light_Stop();
		
		//调用算法
		_Algo_DataCollCalculate(AM_HGB,tCountInfo,pWBCHGB_TestInfo);
		
		//填充数据
		MachRunPara.zj.hgb.xx[ZJHGBxxMode][i] = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB];
	}
	
	//出仓
	OutIn_Module_Out(MOTOR_MODE_NORMAL);

	Enable_Motor_Task(TRUE);
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//刷新界面显示内容
	//更新HGB校准结果数据显示（平均值）
	HGBAvg = 0.0f;
	for(i=0;i<3;i++){
		HGBAvg += MachRunPara.zj.hgb.xx[ZJHGBxxMode][i];
	}
	HGBAvg = HGBAvg/3.0f;
	
	switch(ZJHGBxxMode){
		case ZJ_HGBXX_1:
		{
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_12);
			Len = sprintf(Str,"%.2f",HGBAvg);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			MULTIEDIT_AddText(hItem,"HGB线性1测试完毕\r\n");
			
			//解锁下一个按钮
			WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_13));
		}break;
		case ZJ_HGBXX_2:
		{
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_13);
			Len = sprintf(Str,"%.2f",HGBAvg);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			MULTIEDIT_AddText(hItem,"HGB线性2测试完毕\r\n");
			
			//解锁下一个按钮
			WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_14));
		}break;
		case ZJ_HGBXX_3:
		{
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_14);
			Len = sprintf(Str,"%.2f",HGBAvg);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			MULTIEDIT_AddText(hItem,"HGB线性3测试完毕\r\n");
			
			//解锁下一个按钮
			WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_15));
		}break;
		case ZJ_HGBXX_4:
		{
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_15);
			Len = sprintf(Str,"%.2f",HGBAvg);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			MULTIEDIT_AddText(hItem,"HGB线性4测试完毕\r\n");
			
			//解锁下一个按钮
			WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_16));
		}break;
		case ZJ_HGBXX_5:
		{
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_16);
			Len = sprintf(Str,"%.2f",HGBAvg);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			MULTIEDIT_AddText(hItem,"HGB线性5测试完毕\r\n");
			
			//解锁下一个按钮
			WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_9));
		}break;
		default :break;
	}
	
	
	//向界面发送后端执行完毕消息
	WM_SendMessageNoPara(g_hActiveWin,WM_ZJ_TEST_OVER);
}




/***
*执行质检模式--准确度
***/
void ZJ_HGB_ZQD(ZJ_HGBZQD_e ZJHGBzqdMode)
{
	extern Panel_LED_Status_e g_ePanel_LED_Status;
	extern __IO uint8_t g_CountHandle_Status;
	extern __IO WM_HWIN g_hActiveWin;
	WM_HWIN hItem;
	char Str[50] = {0};
	uint8_t Len = 0;
	ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
	
	//确保关闭恒流源
	BC_CUR_SW_OFF;
	
	//面板指示灯闪烁
	g_CountHandle_Status = 1;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//屏蔽进出仓按键
	Disable_Motor_Task();
	
	//进仓
	OutIn_Module_In(MOTOR_MODE_NORMAL);
	
	/***************** 计数信息结构体初始化 ***********************/
	CountInfo_t* tCountInfo = (CountInfo_t*)SDRAM_ApplyCommonBuf(sizeof(CountInfo_t));
	CountInfo_Init(tCountInfo);
	   
	WBCHGB_TestInfo_t* pWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
	
	//当前操作者
	pWBCHGB_TestInfo->eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
	
	/*** 下一样本信息处理 ***/
	Get_NexSample_Info(pWBCHGB_TestInfo, &MachRunPara.tNextSample); 
	
	//正常模式
	LIQUID_WAVE_CLOSE;
	AIR_WAVE_CLOSE;
	
	eErrorCode = Count_Process(TEST_PROJECT_WBC_HGB, COUNT_MODE_NORMAL, tCountInfo, (__IO uint8_t*)pWBCHGB_TestInfo, BACKEND_TO_UI_MSG_BUFFER_LEN);
	
	LIQUID_WAVE_CLOSE;
	AIR_WAVE_CLOSE;
	BC_CUR_SW_OFF;
	
	if(eErrorCode == ERROR_CODE_SUCCESS){
		//填充数据
		MachRunPara.zj.zqd[ZJHGBzqdMode].hgbValue = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB];
		MachRunPara.zj.zqd[ZJHGBzqdMode].wbcValue.wbc = pWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
		MachRunPara.zj.zqd[ZJHGBzqdMode].wbcValue.fl = pWBCHGB_TestInfo->fWbcMaxPos;
	}else{
		extern WM_HWIN DialogWin;
		CommonDialogPageData_s DialogPageData = {0};
		WM_MESSAGE pMsg;
		
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = HAS_CONFIM_BUTTON;
		DialogPageData.fun = NULL;
		
		Len = sprintf(DialogPageData.str,"测试失败（E%04u），请重新测试",eErrorCode);
		DialogPageData.str[Len] = '\0';

		pMsg.MsgId = WM_USER_DATA;
		pMsg.Data.p = &DialogPageData;
		WM_SendMessage(DialogWin,&pMsg);
		
		//出仓
		OutIn_Module_Out(MOTOR_MODE_NORMAL);

		Enable_Motor_Task(TRUE);
		
		//关闭面板指示灯
		g_CountHandle_Status = 0;
		g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
		
		return;
	}
	
	//出仓
	OutIn_Module_Out(MOTOR_MODE_NORMAL);

	Enable_Motor_Task(TRUE);
	
	//关闭面板指示灯
	g_CountHandle_Status = 0;
	g_ePanel_LED_Status = PANEL_LED_STATUS_NORMAL;
	
	//刷新界面显示内容
	switch(ZJHGBzqdMode){
		case ZJ_HGBZQD_1:
		{
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_6);
			Len = sprintf(Str,"%.2f",MachRunPara.zj.zqd[ZJHGBzqdMode].wbcValue.wbc);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_9);
			Len = sprintf(Str,"%.2f",MachRunPara.zj.zqd[ZJHGBzqdMode].hgbValue);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			MULTIEDIT_AddText(hItem,"准确度1测试完毕\r\n");
			
			//解锁下一个按钮
			WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_10));
		}break;
		case ZJ_HGBZQD_2:
		{
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_7);
			Len = sprintf(Str,"%.2f",MachRunPara.zj.zqd[ZJHGBzqdMode].wbcValue.wbc);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_10);
			Len = sprintf(Str,"%.2f",MachRunPara.zj.zqd[ZJHGBzqdMode].hgbValue);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			MULTIEDIT_AddText(hItem,"准确度2测试完毕\r\n");
			
			//解锁下一个按钮
			WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_11));
		}break;
		case ZJ_HGBZQD_3:
		{
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_8);
			Len = sprintf(Str,"%.2f",MachRunPara.zj.zqd[ZJHGBzqdMode].wbcValue.wbc);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_TEXT_11);
			Len = sprintf(Str,"%.2f",MachRunPara.zj.zqd[ZJHGBzqdMode].hgbValue);
			Str[Len] = '\0';
			TEXT_SetText(hItem,Str);
			
			hItem = WM_GetDialogItem(g_hActiveWin, ID_MULTIEDIT_0);
			MULTIEDIT_AddText(hItem,"准确度3测试完毕\r\n");
			MULTIEDIT_AddText(hItem,"\r\n请点击数据上传按钮\r\n");
			
			//解锁下一个按钮
			WM_EnableWindow(WM_GetDialogItem(g_hActiveWin,ID_BUTTON_4));
		}break;
		default :break;
	}
	
	
	//向界面发送后端执行完毕消息
	WM_SendMessageNoPara(g_hActiveWin,WM_ZJ_TEST_OVER);
}













