#include "main.h"
#include "usart.h"
#include "bsp_outin.h"
#include "bsp_motor.h"
#include "bsp_press.h"
#include "bsp_pump.h"
#include "process.h"
#include "cmsis_os.h"
#include "bc.h"
#include "monitorV.h"
#include "backend_msg.h"
#include "file_operate.h"
#include "bsp_eeprom.h"
#include "process.h"
#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "bsp_light.h"
#include "parameter.h"
#include "Printer.h"
#include "CAT_iot.h"
#include "crc16.h"
#include "bsp_spi.h"

//algo
#include "alg.h"
#include "algstring.h"
#include "algpulse.h"
#include "abcompute.h"
#include "algwbc.h"
#include "algflag.h"
#include "algdata.h"
#include "modedefs.h"
//#include "algfile.h"
//#include "absalg.h"
#include "algplusdefs.h"
#include "Public_menuDLG.h"
#include "Common.h"
#include "HGB_Deal.h"


#define XK_V_NONE       2500    //小孔断开
#define XK_V_ON         800     //小孔导通



static __IO CountMode_t g_tCountMode;


/*
*   计数模式信息，初始化
*/
void CountMode_Init(void)
{
    g_tCountMode.ulCountMode        = COUNT_MODE_NORMAL;    //当前计数模式
    g_tCountMode.ulInGZ_CountNum    = 0;                    //内部工自动执行（老化）次数
    g_tCountMode.ulOutGZ_CountNum   = 0;                    //外部工自动执行（老化）次数
}


/*
*   获取计数模式信息，结构体指针
*/
__IO CountMode_t* Get_CountMode_Info(void)
{
    return &g_tCountMode;
}

/*
*   设置计数模式信息，结构体指针
*/
CountMode_e Set_CountMode(CountMode_e eCountMode)
{
    g_tCountMode.ulCountMode = eCountMode;
    return g_tCountMode.ulCountMode;
}

/*
    ******************* Algo ********************************************
*/


/*
*   根据错误码得到对应的错误类型
*/
ErrorType_e ErrorCode_Type(ErrorCode_e eErrorCode)
{
    ErrorType_e eErrorType =  ERROR_CODE_TYPE_NORMAL;   
    switch(eErrorCode)
    {
        case ERROR_CODE_SUCCESS:
        {
            //正常
            eErrorType = ERROR_CODE_TYPE_NORMAL;
        }
        break;

        case ERROR_CODE_FAILURE:
        case ERROR_CODE_BUILD_PRESS:        //建立负压异常
        case ERROR_CODE_AIR_TIGHT:          //系统气密性异常
        case ERROR_CODE_BC_CURRENT:         //恒流源异常
        case ERROR_CODE_POWER_12VP:         //正12V, 供电异常
        case ERROR_CODE_POWER_12VN:         //负12V, 供电异常
        case ERROR_CODE_OUTIN_OUT:          //进出仓电机，出仓异常
        case ERROR_CODE_OUTIN_IN:           //进出仓电机，进仓异常
        case ERROR_CODE_OUTIN_OC:           //进出模块，光耦异常
        case ERROR_CODE_CLAMP_OUT:          //夹子电机，出仓异常
        case ERROR_CODE_CLAMP_IN:           //夹子电机，进仓异常  
        case ERROR_CODE_CLAMP_OC:           //对接模块，光耦异常 
        case ERROR_CODE_NOT_AT_POS:         //未进仓导致计数失败
//        case ERROR_CODE_BSK:                //液路建立阶段，小孔电压低于0.79v
//        case ERROR_CODE_ELEC_TRIGGLE_M1:    //定量电极异常触发，稳定度异常次数1
//        case ERROR_CODE_ELEC_TRIGGLE_M2:    //定量电极异常触发，稳定度异常次数2
//        case ERROR_CODE_ELEC_TRIGGLE_M3:    //定量电极异常触发，稳定度异常次数3
//        case ERROR_CODE_ELEC_TRIGGLE_M4:    //定量电极异常触发，稳定度异常次数4
//        case ERROR_CODE_ELEC_TRIGGLE_M5:    //定量电极异常触发，稳定度异常次数5

//        case ERROR_CODE_ADD_PRESS:          //压力补偿值不一致
//        case ERROR_CODE_ADD_MOTOR_STEP:     //电机补偿值不一致
//        case ERROR_CODE_ADD_BC_GAIN:        //bc增益补偿值不一致
//        case ERROR_CODE_PUMP:               //泵占空比值不一致
//      case ERROR_CODE_TANZHEN_SIGNAL:     //探针接触异常
        case ERROR_CODE_NO_PRINTER:         //打印机未就绪
        case ERROR_CODE_MONITOR_SIGNAL_ERR: //计数电极接触异常（信号稳定度检测阶段）
        case ERROR_CODE_ALGO_MODE:          //算法判断设置模式异常
        case ERROR_CODE_ALGO_BREAK:         //算法退出运算
        case ERROR_CODE_TEMPERATURE:        //温度异常  
        case ERROR_CODE_EEPROM:             //EEPROM异常  
        case ERROR_CODE_CAT_ONE:            //CAT1模块异常  
        case ERROR_CODE_HGB_MODULE:         //HGB模块异常  
        case ERROR_CODE_CELL_CHECK_MODULE:  //对射检测模块异常
        case ERROR_CODE_DISK_CAPACITY:      //磁盘容量不足
        case ERROR_CODE_FLASH_DATA_CALIBRATE_ERR://SPI Flash数据读写校验失败
        {
            //仪器故障
            eErrorType =  ERROR_CODE_TYPE_MACHINE;
        }
        break; 
        
        case ERROR_CODE_AIR_LEAK_COUNTING_1:       //计数采样中，漏气。压力低于-25kpa,最多重复建压3次，且3次内定量电极正常触发
        case ERROR_CODE_AIR_LEAK_COUNTING_2:       //计数采样中，漏气。压力低于-25kpa，最多重复建压3次，且3次内定量电极没有正常触发
        case ERROR_CODE_ELEC_TRIGGLE_2:            //定量电极异常触发
        case ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE:  //初始化阶段，小孔电压导通 
        case ERROR_CODE_BSK_2S_15V:                //第一次开阀2后2s内，小孔电压小于1.5V
        case ERROR_CODE_BSK_XKV_TOO_LOW:           //小孔电压过低
        {
            //耗材故障
            eErrorType =  ERROR_CODE_TYPE_MATERIAL; 
        
        }
        break;

        case ERROR_CODE_ELEC_TRIGGLE:               //定量电极异常触发
        case ERROR_CODE_INVALID_FILENUM:            //质控，无效的文件号
        case ERROR_CODE_FILENUM_CAPACITY_IS_FULL:   //质控，文件号下存储数据已满
        case ERROR_CODE_PRINT_NO_PAPER:             //打印机缺纸
        case ERROR_CODE_SAMPLE_NOT_ENOUGH:          //待测样本量不足 
        case ERROR_CODE_NONE_CELL:                  //计数未放入血细胞检测模块
        case ERROR_CODE_SAMPLE_NONE:                // 未放入测样本量
        {
             //用户操作故障
            eErrorType =  ERROR_CODE_TYPE_USER;            
        }
        break;
        
        case ERROR_CODE_RESULT_ASTERISK:        //计数结果报星号
//        case ERROR_CODE_TIMEOUT_10_BEFORE:    //超时,前10秒
//        case ERROR_CODE_TIMEOUT_10_AFTER:     //超时,后10秒
        case ERROR_CODE_T4_ERR_15S_2_MAXTIME:   //T4时间异常，（大于15s, 但是小于最大计数时间）
        case ERROR_CODE_RE_CONN_2_T4_15S:       //液路建立阶段重对接次数>=2, 且T4>15s 
        case ERROR_CODE_TIMEOUT:                //超时
        case ERROR_CODE_AIR_LEAK:               //气嘴漏气
        case ERROR_CODE_WBC_ELEC_TOUCH:         //采样电极接触异常
        {
            //综合故障
            eErrorType = ERROR_CODE_TYPE_MULTI;
        }
        break;
        
        //
        default:
           //正常
            eErrorType = ERROR_CODE_TYPE_NORMAL;          
        break;
    }

    return eErrorType;
}



/*
* 获取隐藏故障码
*/
ErrorCode_e Get_Hide_ErrorCode(CountInfo_t *ptCountInfo)
{
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
    
    if(ptCountInfo->tCountEnd.eErrorCode != ERROR_CODE_SUCCESS)
    {
        eErrorCode = ptCountInfo->tCountEnd.eErrorCode;
    }else if(ptCountInfo->tSignalAnalysis.eErrorCode != ERROR_CODE_SUCCESS){
        eErrorCode = ptCountInfo->tSignalAnalysis.eErrorCode;
    }else if(ptCountInfo->tSignalMonitor .eErrorCode != ERROR_CODE_SUCCESS){
        eErrorCode = ptCountInfo->tSignalMonitor.eErrorCode;
    }else if(ptCountInfo->tLiquidCreate.eErrorCode != ERROR_CODE_SUCCESS){
        eErrorCode = ptCountInfo->tLiquidCreate.eErrorCode;
    }else if(ptCountInfo->tCountInit.eErrorCode != ERROR_CODE_SUCCESS){
        eErrorCode = ptCountInfo->tCountInit.eErrorCode;
    }
    //
    return eErrorCode;
}


/*
*   计数池重复使用，准备工作
*/
ErrorCode_e ReUse_Init(void)
{
    uint32_t i = 0;
    int32_t lPress = 0;
    uint32_t ulStartTick = 0;
    
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
    
    OutIn_Module_In(MOTOR_MODE_NORMAL);  //进出
    
    /*** 负压过高检测 ***/
    lPress = Press_Value();
    if(lPress < MAX_BC_TEST_PRESS_VALUE)
    {
        //缓冲室负压过高，放气
        LOG_Warm("1w: Prees=%d>%d", lPress, MAX_BC_TEST_PRESS_VALUE);
        Air_Bleed();
    }
    
    /*** 负压过低检测 ***/
    ulStartTick = HAL_GetTick();
    lPress = Press_Value();
    if(lPress > MIN_BC_TEST_PRESS_VALUE)  //-30kpa
    {
        LOG_Warm("Prees=%d<%d", lPress, MIN_BC_TEST_PRESS_VALUE);

        //缓冲室负压过低，建压，连续三次
        for(i = 1; i <= MAX_BUILD_PRESS_NUM; i++)
        {           
            eErrorCode = Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE);  
            osDelay(BUILD_PERIOD_PER_TIME);
        }

        lPress = Press_Value();
        if(lPress > AIR_LEAK_PRESS)
        {
            //压力依旧过低         
//            pCountInfo->tCountInit.ePress_Status = FALSE;
            eErrorCode = ERROR_CODE_BUILD_PRESS;
            LOG_Error("1e: Build Press Failure, ErrCode=%d, Press=%d, Num=%d, T=%d", eErrorCode, lPress,i, HAL_GetTick()-ulStartTick);
            return eErrorCode;
        }    
    }
    
    return eErrorCode;
}


/*
*   执行计数
*/
ErrorCode_e Count_Exec(TestProject_e eTestProject, __IO CountMode_e eCountMode)
{
    //extern __IO uint8_t g_OutIn_Key;
    extern __IO OutIn_Module_Position_e g_eOutIn_Key;
    extern osSemaphoreId xSema_OutIn_Key;
    extern MachInfo_s MachInfo;
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
    extern LTDC_HandleTypeDef hltdc;
    extern __IO uint8_t g_OutIn_Module_Status;
    extern __IO uint8_t g_CountHandle_Status;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern WM_HWIN DialogWin;
    
    uint32_t ulIndex = 0;
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
    FeedBack_e eDataFeedBack = FEED_BACK_SUCCESS;
    uint8_t ucaMsgBuffer[BACKEND_TO_UI_MSG_BUFFER_LEN] =  {0};
	RTC_HYM8563Info_s RTCHym8563Info;
	CATQueueInfoBuf_s CATQueueInfoBuf = {0};
    uint8_t ucaErrTypePrompt[ERROR_CODE_TYPE_END][13] = {"Success", "UsrOptErr", "MachineErr", "MaterialErr", "MultiErr"}; 
	extern osMessageQueueId_t CATQueueHandle;
	extern osMessageQueueId_t CATQueueOtherHandle;
    uint8_t i = 0;
	
	//超级管理员下不需要下拉患者信息
	if(MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].authority < MachInfo.accountMM.accountInfo[ACCOUNT_TYPE_SUPER].authority){
		//判断是否开启了下拉患者信息功能，并且信息已经下拉成功
		switch(MachInfo.companyInfo.company){
			case COMPANY_YSB:
			case COMPANY_HY:
			{
				//药师帮，和映
				if(MachRunPara.tNextSample.validFlag != 1){
					if(MachRunPara.flag.requestingPatient == 1){
						//正在请求患者信息中
						return ERROR_CODE_FAILURE;
					}else{
						MachRunPara.flag.requestingPatient = 1;
						
						//弹出录入患者编号界面并且准备下拉患者信息		
						Msg_Head_t *pMsgHead = (Msg_Head_t*)ucaMsgBuffer;
						pMsgHead->usCmd      = CMD_ANALYSIS_INPUT_USER_NUM;
						BackEnd_Put_Msg((uint8_t*)ucaMsgBuffer);
						return ERROR_CODE_FAILURE;
					}
				}
			}break;
			
			default :break;
		}
	}
	
	//判断是否放入了计数池，未放入时(==RESET)，则直接弹窗提示即可
	if(eCountMode!=COUNT_MODE_SIMULATION_SIGNAL_INNER && eCountMode!=COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO && ConutCell_Is_Exist() == RESET){
		if(DialogWin == NULL){
			extern WM_HWIN CreateDialogPage(void);
			extern WM_HWIN Analysis_Menu(void);
			CommonDialogPageData_s DialogPageData = {0};
			WM_MESSAGE pMsg;
			
			GUI_EndDialog(g_hActiveWin,0);
			g_hActiveWin = CreateDialogPage();
			
			//弹窗提示
			DialogPageData.bmpType = BMP_ALARM;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = Analysis_Menu;
			strcpy(DialogPageData.str,g_ucaLng_Analyse_NOJSC[MachInfo.systemSet.eLanguage]);

			pMsg.MsgId = WM_USER_DATA;
			pMsg.Data.p = &DialogPageData;
			WM_SendMessage(g_hActiveWin,&pMsg);
		}
		
		return ERROR_CODE_FAILURE;
	}
	
    
    //当进出电机，还未执行完成时，等待其完成(防止在执行进出仓的同时，按下计数键，挂起进仓线程)
    while(1 == g_OutIn_Module_Status)
    {
        //未执行完成
        osDelay(100);
        if(0 == g_OutIn_Module_Status || ++i > 70) //进出仓模块执行完成，或超过7s
        {
            break;
        }
    }
            
    //发送消息给UI（样本分析界面），显示样本正在分析中...（自动模式除外，质控除外）
    if(eCountMode != COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO && eCountMode != COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO)
    {
        //当不是自动模式，才发送
        Msg_Head_t *pMsgHead = (Msg_Head_t*)ucaMsgBuffer;
        pMsgHead->usCmd      = CMD_ANALYSIS_PROMPT_COUNTING;
        BackEnd_Put_Msg((uint8_t*)ucaMsgBuffer);
        osDelay(1000); //延时让动画显示
    }
    
    /***************** 计数信息结构体初始化 ***********************/
    CountInfo_t* tCountInfo = (CountInfo_t*)SDRAM_ApplyCommonBuf(sizeof(CountInfo_t));
    CountInfo_Init(tCountInfo);
       
    /*** 获取测试时间 ***/
    __IO WBCHGB_TestInfo_t *pWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)ucaMsgBuffer;
    strncpy((char*)pWBCHGB_TestInfo->ucaDateTime, (char*)MachRunPara.ucaLastestDateTime, DATE_TIME_LEN); 
     Update_WBCHGB_DataManage_LastestDateTime();
	
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	
	//保存时间戳
	pWBCHGB_TestInfo->timestamp = GetCurrentTimestamp(RTCHym8563Info);
	
    //当前操作者
    pWBCHGB_TestInfo->eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
    
    /*** 下一样本信息处理 ***/
    //获取下一个样本测试信息，将其填充到wbc结果信息结构体中, 这步操作在调用用Count_Press之前（在获取算法结果数据后，需要用到参考组处理告警信息），
    LOG_Info("1 Next Sample Info: SN=%d, SampleSN=%s", MachRunPara.tNextSample.ulNextDataSN, MachRunPara.tNextSample.ucaNextSampleSN);
    
	Get_NexSample_Info(pWBCHGB_TestInfo, &MachRunPara.tNextSample); 
    
	LOG_Info("1 pWBCHGB_TestInfo: CurSN=%d, SampleSN=%s", pWBCHGB_TestInfo->ulCurDataSN, pWBCHGB_TestInfo->ucaSampleSN);
	
     //日志信息 FULL_VERSION
    tCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, tCountInfo->tCountLog.ucaLogBuffer, tCountInfo->tCountLog.usLogLen, "SoftV=%d,SN=%s,Temp=%d\r\n", 
            FULL_VERSION, MachInfo.labInfo.ucaMachineSN, NTC_Temperature());      
    tCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, tCountInfo->tCountLog.ucaLogBuffer, tCountInfo->tCountLog.usLogLen, "SampleSN=%s,DateTime=%s,HardV=%s\r\n", 
            pWBCHGB_TestInfo->ucaSampleSN, pWBCHGB_TestInfo->ucaDateTime, MachRunPara.hardVer.main); 

  
    /*** 计数流程选择 ***/
    if(eCountMode == COUNT_MODE_SIGNAL_SRC || eCountMode == COUNT_MODE_SIMULATION_SIGNAL_INNER || eCountMode == COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO)
    {
		//开启恒流源
		BC_CUR_SW_ON;
		HAL_Delay(300);
		
        //计数仓进仓
        OutIn_Module_In(MOTOR_MODE_NORMAL);
        eErrorCode = Algo_BC_Handler(eCountMode, tCountInfo, (__IO uint8_t*)pWBCHGB_TestInfo, BACKEND_TO_UI_MSG_BUFFER_LEN);
    }else if(eCountMode == COUNT_MODE_RE_USE){
		//开启恒流源
		BC_CUR_SW_ON;
		HAL_Delay(500);
		
        eErrorCode = ReUse_Init();
        if(eErrorCode != ERROR_CODE_SUCCESS) return eErrorCode;
        eErrorCode = Algo_BC_Handler(eCountMode, tCountInfo, (__IO uint8_t*)pWBCHGB_TestInfo, BACKEND_TO_UI_MSG_BUFFER_LEN);
    }else if(eCountMode == COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE || eCountMode == COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO){
        //恒流源开
        BC_CUR_SW_ON;
		
        App_Delay(RUN_MODE_COUNT, 300);
        
         //计数仓进仓
        eErrorCode = OutIn_Module_In(MOTOR_MODE_NORMAL);
        
        //纯外部工装信号测试，无流程
        uint32_t ulStartTick = 0, ulEndTick = 0;
        uint8_t ucElec_Status = 0;
        
        ulStartTick = HAL_GetTick();
        ulEndTick = ulStartTick;
        while(ulEndTick - ulStartTick <= 5000)
        {
            //5秒内，定量电极拉低，作为工装计数的计时起点
            ulEndTick = HAL_GetTick();
            ucElec_Status = ELEC_STATUS;
            if(RESET == ucElec_Status)
            { 
                break;
            }
        }
        //
        if(ulEndTick - ulStartTick > 5000)
        {
            //五秒内定量电极未触发，则本次流程结束
            LOG_Error("GZ TimeOut E=%d", ELEC_STATUS);
            eErrorCode = ERROR_CODE_FAILURE;
        }
        //
        if(eErrorCode == ERROR_CODE_SUCCESS)
        {            
            //工装同步信号正常，在进行数据采集
            eErrorCode = Algo_BC_Handler(eCountMode, tCountInfo, (__IO uint8_t*)pWBCHGB_TestInfo, BACKEND_TO_UI_MSG_BUFFER_LEN);
        }
    }else{
        //正常模式
        LIQUID_WAVE_CLOSE;
        AIR_WAVE_CLOSE;
		
        eErrorCode = Count_Process(eTestProject, eCountMode, tCountInfo, (__IO uint8_t*)pWBCHGB_TestInfo, BACKEND_TO_UI_MSG_BUFFER_LEN);
		
        LIQUID_WAVE_CLOSE;
        AIR_WAVE_CLOSE;
    }
	
    //WBC恒流源关
    BC_CUR_SW_OFF;
    
    
    /*** 消息头 ***/
    //更新消息头, 测试模式信息，影响后续界面交互
    if(eCountMode == COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO || eCountMode == COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO) //自动模式
    {
        //计数：外部工装信号仿真模拟测试,纯信号,自动执行模式（不进行自动界面切换）
        pWBCHGB_TestInfo->tMsgHead.usCmd 	= CMD_ANALYSIS_COUNT_MODE_AUTO;
    }else{
        //
        pWBCHGB_TestInfo->tMsgHead.usCmd 	= CMD_ANALYSIS_COUNT;
    }
    
    //更新消息头,错误信息(影响后续界面交互)(需注意数据保存的错误，如果在数据保存失败后，会更新)
	pWBCHGB_TestInfo->tMsgHead.eErrorCode   = eErrorCode;
    if(ERROR_CODE_SUCCESS == eErrorCode)
    {
        //正常状态
        pWBCHGB_TestInfo->eDataStatus       = DATA_STATUS_NORMAL;
        pWBCHGB_TestInfo->tMsgHead.errType  = ERROR_CODE_TYPE_NORMAL;
    }else{
        //异常时，并且数据异常(),把数据状态置为无效
        pWBCHGB_TestInfo->eDataStatus       = DATA_STATUS_COUNT_INVALID; //报错下，获得的数据
        pWBCHGB_TestInfo->tMsgHead.errType  = ErrorCode_Type(eErrorCode);
    }
	pWBCHGB_TestInfo->tMsgHead.usMsgLen 	= WBCHGB_TEST_INFO_LEN - MSG_HEAD_LEN;

    /*** 数据存储/管理更新  ***/  
//    Printf_Free_StackSize();  
    //保存结果信息
    ulIndex = Update_WBCHGB_Tail();
    pWBCHGB_TestInfo->ulCurDataIndex = ulIndex;
    //保存结果
    pWBCHGB_TestInfo->usCrc = CRC16((uint8_t*)&pWBCHGB_TestInfo->tMsgHead, sizeof(WBCHGB_TestInfo_t)-4);
    //
    
    //回写到文件中，
    eDataFeedBack = FatFs_Write_WBCHGB_Info(pWBCHGB_TestInfo->ulCurDataIndex, pWBCHGB_TestInfo);
    if(FEED_BACK_SUCCESS != eDataFeedBack)
    {
        //写数据出错
        tCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, tCountInfo->tCountLog.ucaLogBuffer, tCountInfo->tCountLog.usLogLen, "Write WBCHGB Data Err=%d",eDataFeedBack);
        if(FEED_BACK_FAIL == eDataFeedBack) eErrorCode = ERROR_CODE_FLASH_DATA_CALIBRATE_ERR;  //SPI Flash数据读写校验失败
    }
    
    //保存计数日志,(保存在文件的最后)
    //计数控制信息
    CountHead_t* tLastCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
	CountHead_t* tCurCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
    ulIndex = Get_WBCHGB_BeforeTail();
    if(INVALID_DATA_SN != ulIndex)
    {
        //存在上一条有效数据 (queue不为空) 
        eDataFeedBack = FatFs_Read_ConutHead(ulIndex, tLastCountHead);
        if(FEED_BACK_SUCCESS != eDataFeedBack)
        {
            //写数据出错
            tCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, tCountInfo->tCountLog.ucaLogBuffer, tCountInfo->tCountLog.usLogLen, "Read CountHead Err=%d",eDataFeedBack);
            if(FEED_BACK_FAIL == eDataFeedBack) eErrorCode = ERROR_CODE_FLASH_DATA_CALIBRATE_ERR;  //SPI Flash数据读写校验失败
        }
        
        tCurCountHead->ulLastValidIndex  = tLastCountHead->ulCurIndex;
    }else{
        //不存在上一条数据，（queue为空）
        tCurCountHead->ulLastValidIndex  = INVALID_DATA_SN;          
    }
    //填充tCurCountHead  
    tCurCountHead->eAccountType      = pWBCHGB_TestInfo->eAccountType;
    tCurCountHead->eErrorCode        = pWBCHGB_TestInfo->tMsgHead.eErrorCode;
    tCurCountHead->errType           = pWBCHGB_TestInfo->tMsgHead.errType;
    tCurCountHead->eHideErrorCode    = Get_Hide_ErrorCode(tCountInfo);
    tCurCountHead->eStatus           = pWBCHGB_TestInfo->eDataStatus;
    tCurCountHead->ulCurSN           = pWBCHGB_TestInfo->ulCurDataSN;
    tCurCountHead->ulCurIndex        = pWBCHGB_TestInfo->ulCurDataIndex;
    tCurCountHead->ucUseAlgoFlag     = tCountInfo->ucUseAlgoFlag;
    //
    tCurCountHead->ulNextValidIndex  = INVALID_DATA_SN;
    tCurCountHead->ucaEndFlag[0]     = '\r';
    tCurCountHead->ucaEndFlag[1]     = '\n';     
    strncpy((char*)tCurCountHead->ucaSampleSN, (const char*)pWBCHGB_TestInfo->ucaSampleSN, SAMPLE_SN_LEN);
    strncpy((char*)tCurCountHead->ucaDateTime, (const char*)pWBCHGB_TestInfo->ucaDateTime, DATE_TIME_LEN);       
    
    //保存当前计数头（控制/状态信息）
    tCurCountHead->usCrc = CRC16((uint8_t*)tCurCountHead, sizeof(CountHead_t)-4);
    //回写到文件中，          
    eDataFeedBack = FatFs_Write_CountHead(tCurCountHead->ulCurIndex, tCurCountHead);
    if(FEED_BACK_SUCCESS != eDataFeedBack)
    {
        //写数据出错
        tCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, tCountInfo->tCountLog.ucaLogBuffer, tCountInfo->tCountLog.usLogLen, "Write CountHead Err=%d",eDataFeedBack);
        if(FEED_BACK_FAIL == eDataFeedBack) eErrorCode = ERROR_CODE_FLASH_DATA_CALIBRATE_ERR;  //SPI Flash数据读写校验失败
    }
    
    //保存上一有效计数控制信息
    if(INVALID_DATA_SN != ulIndex)
    {   //存在上一有效数据
        tLastCountHead->ulNextValidIndex = tCurCountHead->ulCurIndex;
        tLastCountHead->usCrc = CRC16((uint8_t*)tLastCountHead, sizeof(CountHead_t)-4);
        eDataFeedBack = FatFs_Write_CountHead(tLastCountHead->ulCurIndex, tLastCountHead);
        if(FEED_BACK_SUCCESS != eDataFeedBack)
        {
            //写数据出错
            tCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, tCountInfo->tCountLog.ucaLogBuffer, tCountInfo->tCountLog.usLogLen, "Write CountHead Err=%d",eDataFeedBack);
            if(FEED_BACK_FAIL == eDataFeedBack) eErrorCode = ERROR_CODE_FLASH_DATA_CALIBRATE_ERR;  //SPI Flash数据读写校验失败
        }
    }
    
    //计数日志以\r\n结束
    tCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, tCountInfo->tCountLog.ucaLogBuffer, tCountInfo->tCountLog.usLogLen, "ErrCode=%d, ErrType=%s",\
            eErrorCode, ucaErrTypePrompt[pWBCHGB_TestInfo->tMsgHead.errType]);
    //保存计数日志信息
    //确保日志最后一个为\0
    tCountInfo->tCountLog.ucaLogBuffer[LOG_BUFFER_MAX_LEN-1] = 0; 
    tCountInfo->tCountLog.usCrc = CRC16(tCountInfo->tCountLog.ucaLogBuffer, tCountInfo->tCountLog.usLogLen);
    FatFs_Write_ConutLog(pWBCHGB_TestInfo->ulCurDataIndex, &tCountInfo->tCountLog, tCountInfo->tCountLog.usLogLen);    
     

    //更新、保存数据管理信息
    if(eDataFeedBack != FEED_BACK_FAIL) 
    {
        //数据保存成功的情况下，才更新数据管理信息，（当三次数据存储校验都是出错的情况下，才是失败）
        Update_WBCHGB_DataManage();
        Save_DataMange_Info(&g_tDataManage);
    }else{
        //数据保存失败，更新消息头,错误信息(影响后续界面交互)(需注意数据保存的错误，如果在数据保存失败后，会更新)，但是此时该错误码未更新到数据和日志！！！
        eErrorCode = ERROR_CODE_FLASH_DATA_CALIBRATE_ERR;
        pWBCHGB_TestInfo->tMsgHead.eErrorCode   = eErrorCode;
    }
     
    //通过CAT发送样本分析结果到服务端
    //标记CAT必发事件标记
	if(MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType == ACCOUNT_DATA_TYPE_OUTSIDE){
		if(MachInfo.catMastSynServEvent.bit.outtorSampleResult != 1){
			MachInfo.catMastSynServEvent.bit.outtorSampleResult = 1;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
		}
		
		if(CATGlobalStatus.flag.noOtherServer == 0){
			MachInfo.catMastSynServEvent.bit.otherOuttorSampleResult = 1;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
		}
	}else{
		if(MachInfo.catMastSynServEvent.bit.innerSampleResult != 1){
			MachInfo.catMastSynServEvent.bit.innerSampleResult = 1;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
		}
		
		if(CATGlobalStatus.flag.noOtherServer == 0){
			MachInfo.catMastSynServEvent.bit.otherInnerSampleResult = 1;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
		}
	}
     
    //这里需要向服务端发送指令
    CATQueueInfoBuf.msgType = SAMPLE_ANALYSE_CAT_MSG_TYPE;
    osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0, 0);
	
	if(CATGlobalStatus.flag.noOtherServer == 0){
		CATQueueInfoBuf.msgType = SAMPLE_ANALYSE_CAT_MSG_TYPE;
		osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0, 0);
	}
    
    /*** 更新下一样本 ***/
    if(ERROR_CODE_SUCCESS == eErrorCode)
    {
        //计数成功,自动清理下一样本的输入信息。失败的话（后续根据界面提示和用户选择决定）
        Update_NexSample_Info(TRUE, &MachRunPara.tNextSample, &g_tDataManage);
        LOG_Info("2 Next Sample Info: SN=%d, SampleSN=%s", MachRunPara.tNextSample.ulNextDataSN, MachRunPara.tNextSample.ucaNextSampleSN);	
    }else{
        //内部和外部工装，自动测试时，出现错误时也需要更新下一样本信息，防止数据查询出错
        if(eCountMode == COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO || eCountMode == COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO)
        {
            Update_NexSample_Info(TRUE, &MachRunPara.tNextSample, &g_tDataManage);
            LOG_Info("2 Next Sample Info: SN=%d, SampleSN=%s", MachRunPara.tNextSample.ulNextDataSN, MachRunPara.tNextSample.ucaNextSampleSN);	
        }
    }
    
    /*** 发送数据, 刷新UI ***/
    BackEnd_Put_Msg((uint8_t*)ucaMsgBuffer); 

   //后续处理
   if(eCountMode == COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO) //自动外部工装模式
   {
       OutIn_Module_Out(MOTOR_MODE_NORMAL);
       //延时，给UI刷新数据
       osDelay(500);
       
   }else if(eCountMode == COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO){ //自动内部工装模式
       //延时，给UI刷新数据
       osDelay(500);
   }else{  
		Msg_Head_t tMsgHead = {0};
	   
		//出仓
		tMsgHead.usCmd = CMD_OTHERS_OUTIN_OUT;
		UI_Put_Msg((uint8_t*)&tMsgHead);
   }
   
   //正常模式、计数结束，指示灯停止闪耀
   g_CountHandle_Status = 0;
   
   return eErrorCode;
}




/*
*   执行质控计数，（只给三种模式：正常模式，内部工装自动模式，外部工装模式（单次））
*/
ErrorCode_e QC_Count_Exec(TestProject_e eTestProject, __IO CountMode_e eCountMode)
{
    //extern __IO uint8_t g_OutIn_Key;
    extern __IO OutIn_Module_Position_e g_eOutIn_Key;
    extern osSemaphoreId xSema_OutIn_Key;
    extern MachInfo_s MachInfo;
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
    extern LTDC_HandleTypeDef hltdc;
    extern __IO uint8_t g_OutIn_Module_Status;
    extern osMessageQueueId_t CATQueueHandle;
    extern __IO uint8_t g_CountHandle_Status;
    
    uint32_t ulIndex = 0;
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
    uint8_t ucaMsgBuffer[BACKEND_TO_UI_MSG_BUFFER_LEN] =  {0};
	RTC_HYM8563Info_s RTCHym8563Info;
	uint8_t i = 0;
    
    //当进出电机，还未执行完成时，等待其完成(防止在执行进出仓的同时，按下计数键，挂起进仓线程)
    while(1 == g_OutIn_Module_Status)
    {
        //未执行完成
        osDelay(100);
        if(0 == g_OutIn_Module_Status || ++i > 70) //进出仓模块执行完成，或超过7s
        {
            break;
        }
    }
    
    /***************** 计数信息结构体初始化 ***********************/
    CountInfo_t *ptCountInfo = (CountInfo_t*)SDRAM_ApplyCommonBuf(sizeof(CountInfo_t));
    CountInfo_Init(ptCountInfo);
    
    __IO QC_WBCHGB_TestInfo_t *pWBCHGB_TestInfo = (QC_WBCHGB_TestInfo_t*)ucaMsgBuffer;
    
    //检测是否存在有效的文件号
    if(QC_FILENUM_INDEX_END == MachRunPara.eQC_Analysis_FileNum_Index)
    {
        //发送给界面，提示  CMD_QC_ANALYSIS_COUNT
        pWBCHGB_TestInfo->tMsgHead.usCmd = CMD_QC_ANALYSIS_COUNT;
        pWBCHGB_TestInfo->tMsgHead.eErrorCode = ERROR_CODE_INVALID_FILENUM;
        
        BackEnd_Put_Msg(ucaMsgBuffer);
        return ERROR_CODE_FAILURE;
    }
    //
    if(Get_QC_TailIndex(pWBCHGB_TestInfo->eFileNumIndex) >= QC_PER_FILE_NUM_MAX_CAPACITY)
    {
        //发送给界面，提示, 该文件下的数据容量已满
        pWBCHGB_TestInfo->tMsgHead.usCmd = CMD_QC_ANALYSIS_COUNT;
        pWBCHGB_TestInfo->tMsgHead.eErrorCode = ERROR_CODE_FILENUM_CAPACITY_IS_FULL;
        
        BackEnd_Put_Msg(ucaMsgBuffer);
        return ERROR_CODE_FAILURE;
    }
    
    /*** 获取测试时间 ***/
    strncpy((char*)pWBCHGB_TestInfo->ucaDateTime, (char*)MachRunPara.ucaLastestDateTime, DATE_TIME_LEN); 
     Update_WBCHGB_DataManage_LastestDateTime();
	//获取当前时间戳
	RTC_HYM8563GetTime(&RTCHym8563Info);
	//保存时间戳
	pWBCHGB_TestInfo->timestamp = GetCurrentTimestamp(RTCHym8563Info);
	
    /*** 下一样本信息处理 ***/  
    //当前测试所属的文件号
    pWBCHGB_TestInfo->eFileNumIndex = MachRunPara.eQC_Analysis_FileNum_Index;
    //当前操作者
    pWBCHGB_TestInfo->eOperator      = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
    pWBCHGB_TestInfo->eSeter         = MachInfo.qcInfo.set[pWBCHGB_TestInfo->eFileNumIndex].eSeter;
    pWBCHGB_TestInfo->eQC_Level      = MachInfo.qcInfo.set[pWBCHGB_TestInfo->eFileNumIndex].eQC_Level;
    pWBCHGB_TestInfo->usFileNum      = MachInfo.qcInfo.set[pWBCHGB_TestInfo->eFileNumIndex].usFileNum;
    pWBCHGB_TestInfo->usTestCapacity = MachInfo.qcInfo.set[pWBCHGB_TestInfo->eFileNumIndex].usTestCapacity;
    strncpy((char*)pWBCHGB_TestInfo->ucaLotNum, (char*)MachInfo.qcInfo.set[pWBCHGB_TestInfo->eFileNumIndex].ucaLotNum, QC_LOT_NUM_LEN);
    strncpy((char*)pWBCHGB_TestInfo->ucaValidDate, (char*)MachInfo.qcInfo.set[pWBCHGB_TestInfo->eFileNumIndex].ucaValidDate, DATE_TIME_LEN);
   
    //更新下一个数据系列号，通过数据管理来赋值 
    sprintf((char*)pWBCHGB_TestInfo->ucaSampleSN, "QC-%d-%03d", pWBCHGB_TestInfo->usFileNum, Get_QC_TailIndex(pWBCHGB_TestInfo->eFileNumIndex)+1);  
    //日志信息
    ptCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, ptCountInfo->tCountLog.ucaLogBuffer, ptCountInfo->tCountLog.usLogLen, "SampleSN=%s,DateTime=%s\r\n", pWBCHGB_TestInfo->ucaSampleSN, pWBCHGB_TestInfo->ucaDateTime);

    /*** 计数流程选择 ***/
    if(eCountMode == COUNT_MODE_QC_INNER_AUTO)
    {
        BC_CUR_SW_ON;
		HAL_Delay(300);
		
        eErrorCode = Algo_BC_Handler(eCountMode, ptCountInfo, (__IO uint8_t*)pWBCHGB_TestInfo, BACKEND_TO_UI_MSG_BUFFER_LEN);
                
    }else if(eCountMode == COUNT_MODE_QC_OUTSIDE){
		BC_CUR_SW_ON;
		HAL_Delay(300);
		
         //计数仓进仓
         OutIn_Module_In(MOTOR_MODE_NORMAL);
        //计数信号稳定性检测
        
        //纯外部工装信号测试，无流程
        uint32_t ulStartTick = 0, ulEndTick = 0;
        uint8_t ucElec_Status = 0;
        
        ulStartTick = HAL_GetTick();
        ulEndTick = ulStartTick;
        while(ulEndTick - ulStartTick <= 5000)
        {
            //5秒内，定量电极拉低，作为工装计数的计时起点
            ulEndTick = HAL_GetTick();
            ucElec_Status = ELEC_STATUS;
            if(RESET == ucElec_Status)
            { 
                break;
            }
        }
        //
        if(ulEndTick - ulStartTick > 5000)
        {
            //五秒内定量电极未触发，则本次流程结束
            LOG_Error("GZ TimeOut E=%d", ELEC_STATUS);
            eErrorCode = ERROR_CODE_FAILURE;
        }
        //
        if(eErrorCode == ERROR_CODE_SUCCESS)
        {            
            //工装同步信号正常，在进行数据采集
            eErrorCode = Algo_BC_Handler(eCountMode, ptCountInfo, (__IO uint8_t*)pWBCHGB_TestInfo, BACKEND_TO_UI_MSG_BUFFER_LEN);
        }
    }else{
        //正常模式
        AIR_WAVE_CLOSE;
        eErrorCode = Count_Process(eTestProject, eCountMode, ptCountInfo, (__IO uint8_t*)pWBCHGB_TestInfo, BACKEND_TO_UI_MSG_BUFFER_LEN);
        LIQUID_WAVE_CLOSE;
        AIR_WAVE_CLOSE;
    }
	
    //WBC恒流源关
    BC_CUR_SW_OFF;
    
    /*** 消息头 ***/
    //更新消息头, 测试模式信息，影响后续界面交互
    if(eCountMode == COUNT_MODE_QC_INNER_AUTO) //自动模式
    {
        //计数：外部工装信号仿真模拟测试,纯信号,自动执行模式（不进行自动界面切换）
        pWBCHGB_TestInfo->tMsgHead.usCmd 	= CMD_QC_ANALYSIS_COUNT_MODE_AUTO;
    }else if(eCountMode == COUNT_MODE_QC_NORMAL || eCountMode == COUNT_MODE_QC_OUTSIDE){
        //质控计数分析
        pWBCHGB_TestInfo->tMsgHead.usCmd 	= CMD_QC_ANALYSIS_COUNT;
    }else{
        pWBCHGB_TestInfo->tMsgHead.usCmd 	= CMD_QC_ANALYSIS_COUNT;
    }
	
    
    //更新消息头,错误信息(影响后续界面交互)
	pWBCHGB_TestInfo->tMsgHead.eErrorCode   = eErrorCode;
    if(ERROR_CODE_SUCCESS == eErrorCode)
    {
        //正常状态
        pWBCHGB_TestInfo->eDataStatus       = DATA_STATUS_NORMAL;
        pWBCHGB_TestInfo->tMsgHead.errType  = ERROR_CODE_TYPE_NORMAL;
    }else{
        //异常时，并且数据异常(),把数据状态置为无效
        pWBCHGB_TestInfo->eDataStatus       = DATA_STATUS_COUNT_INVALID; //报错下，获得的数据
        pWBCHGB_TestInfo->tMsgHead.errType  = ErrorCode_Type(eErrorCode);
    }
	pWBCHGB_TestInfo->tMsgHead.usMsgLen 	= WBCHGB_TEST_INFO_LEN - MSG_HEAD_LEN;
    //失控/在控判断
    Dis_Ctrol_Judge(pWBCHGB_TestInfo);
    
    //计数日志以\r\n结束
    ptCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, ptCountInfo->tCountLog.ucaLogBuffer, ptCountInfo->tCountLog.usLogLen, "ErrCode=%d%s", eErrorCode, "\r\n");
    

    /*** 数据存储/管理更新  ***/  

    //更新Tail
    ulIndex = Get_QC_TailIndex(pWBCHGB_TestInfo->eFileNumIndex);
    if(ulIndex == 0)
    {
        //第一条
        pWBCHGB_TestInfo->ulCurDataIndex   = ulIndex;
        pWBCHGB_TestInfo->ulLastValidIndex = INVALID_DATA_SN;
        pWBCHGB_TestInfo->ulNextValidIndex = INVALID_DATA_SN;           
    }else{
        pWBCHGB_TestInfo->ulCurDataIndex   = ulIndex;
        pWBCHGB_TestInfo->ulLastValidIndex = INVALID_DATA_SN;   //较新
        pWBCHGB_TestInfo->ulNextValidIndex = Get_QC_LastIndex(pWBCHGB_TestInfo->eFileNumIndex, ulIndex); //ulIndex - 1;       //较旧
        //更新上一记录的，索引
        QC_WBCHGB_TestInfo_t* tQC_WBCHGB_TestInfo = (QC_WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(QC_WBCHGB_TestInfo_t));
        FatFs_Read_QC_WBCHGB_Info(pWBCHGB_TestInfo->eFileNumIndex, pWBCHGB_TestInfo->ulNextValidIndex, tQC_WBCHGB_TestInfo);
        tQC_WBCHGB_TestInfo->ulLastValidIndex = pWBCHGB_TestInfo->ulCurDataIndex;
        FatFs_Write_QC_WBCHGB_Info(pWBCHGB_TestInfo->eFileNumIndex, pWBCHGB_TestInfo->ulNextValidIndex, tQC_WBCHGB_TestInfo);
    }

    //保存数据
    FatFs_Write_QC_WBCHGB_Info(pWBCHGB_TestInfo->eFileNumIndex, ulIndex, pWBCHGB_TestInfo);
    //更新、保存数据管理信息
    Add_QC_TailIndex(pWBCHGB_TestInfo->eFileNumIndex);
    Add_QC_ValidNum(pWBCHGB_TestInfo->eFileNumIndex);
    Save_DataMange_Info(&g_tDataManage); 
       
        
    /*** 发送数据, 刷新UI ***/
    BackEnd_Put_Msg((uint8_t*)ucaMsgBuffer); 
        
   if(eCountMode == COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE) //自动外部工装模式
   {
       OutIn_Module_Out(MOTOR_MODE_NORMAL);
   }else if(eCountMode == COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO){ //自动内部工装模式
       //延时，给UI刷新数据
       osDelay(500);
   }else{    
	   Msg_Head_t tMsgHead = {0};
	   
		//出仓
		tMsgHead.usCmd = CMD_OTHERS_OUTIN_OUT;
		UI_Put_Msg((uint8_t*)&tMsgHead);
   }
   
   //计数结束
   g_CountHandle_Status = 0;
   return eErrorCode;
}


/*
*   缓冲室放气
*/
ErrorCode_e Air_Bleed(void)
{
    extern MachInfo_s MachInfo;
    ErrorCode_e eStatus = ERROR_CODE_SUCCESS;
    
    //气嘴松开
    eStatus = Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
    
    //开液阀，放气1.5s
    LIQUID_WAVE_OPEN;
    osDelay(1500);
    LIQUID_WAVE_CLOSE;
    
    //气嘴加紧
    eStatus = Clamp_Motor_Out(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
    
    return eStatus;
}


/*
*   计数, 初始化阶段
*/
ErrorCode_e Count_Init(TestProject_e eTestProject, __IO CountMode_e eCountMode, __IO CountInfo_t *pCountInfo)
{
    extern MachInfo_s MachInfo;
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
    OutIn_Module_Position_e ePosition = OUTIN_MODULE_POSITION_NUKNOWN;
    uint32_t ulStartTick = 0, ulPluseNum = 0;
    int32_t lPress = 0;
    uint16_t usXK_V = 0, i = 0, usTemp = 0, usLen = 0;
    uint8_t Elec_Status = 0;
    double dMean = 0, dCV = 0;

	//提前开启535波长灯
    Light_Start(HGBLED_535,MachInfo.paramSet.current535);
	
    pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%s\r\n", "[CountInit]");  
    
    /*** 计数仓是否已进仓 ***/
    ePosition = OutIn_Module_Positon();
    if(OUTIN_MODULE_POSITION_IN != ePosition)  
    {
        //不在进仓位置，执行进仓
        for(i = 1; i <= MAX_OUTIN_MOTOR_MOVE_NUM; i++) //2
        {
            eErrorCode = OutIn_Module_In(MOTOR_MODE_NORMAL);  //进出
            if(ERROR_CODE_SUCCESS == eErrorCode)
            {
                //进仓成功
                break;
            }
            pCountInfo->tCountInit.ucOutIn_Motor_Move_Num = i;
            if(pCountInfo->tCountInit.ucOutIn_Motor_Move_Num == 0)
            {
                 //进仓失败次数小于最大值，记录信息
                LOG_Warm("1w: ErrCode=%d,ErrNum=%d", eErrorCode, pCountInfo->tCountInit.ucOutIn_Motor_Move_Num);
                pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen,\
                         "1w: ErrCode=%d,ErrNum=%d\r\n", eErrorCode, pCountInfo->tCountInit.ucOutIn_Motor_Move_Num);     
            }else if(pCountInfo->tCountInit.ucOutIn_Motor_Move_Num >= MAX_OUTIN_MOTOR_MOVE_NUM){
                //进仓失败次数大于最大值，退出
                LOG_Error("1e: ErrCode=%d,ErrNum=%d", eErrorCode, pCountInfo->tCountInit.ucOutIn_Motor_Move_Num);
                pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                          "1e: ErrCode=%d,ErrNum=%d\r\n", eErrorCode, pCountInfo->tCountInit.ucOutIn_Motor_Move_Num);
                return eErrorCode;
            }
        }
    }
    //进仓状态
    pCountInfo->tCountInit.eOutInModule_Status = OUTIN_MODULE_POSITION_IN;
    
    
     /*** 计数池是否存在 ***/
    if(SET == ConutCell_Is_Exist())
    {
        pCountInfo->tCountInit.eCountCell_Flag = TRUE;
    }else{
        //计数池不存在
        eErrorCode = ERROR_CODE_NONE_CELL;
        pCountInfo->tCountInit.eCountCell_Flag = FALSE;
        pCountInfo->tCountInit.eErrorCode = eErrorCode;
        pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                    "1e: Cell Not Exist, ErrCode=%d\r\n", eErrorCode);
        return eErrorCode; 
    }
    
    /*** 探针光耦是否触发（探针对接电机是否已夹紧） ***/
    if(OC_Status(OC_INDEX_CLAMP_OUT) != SET)
    {
        //未夹紧, 最多三次对接
        for(i = 1; i <= MAX_TZ_MOTOR_MOVE_NUM; i++)
        {
            eErrorCode = Clamp_Motor_Out_then_In(MOTOR_MODE_NORMAL);
            if(eErrorCode == ERROR_CODE_SUCCESS)
            {
                //对接成功，退出
                break;
            }
            pCountInfo->tCountInit.ucTZMotor_Move_Num = i;
            if(pCountInfo->tCountInit.ucTZMotor_Move_Num == 0)
            {
                //失败次数小于最大值，记录信息
                LOG_Warm("1w: HideErrCode=%d,ErrNum=%d", eErrorCode, pCountInfo->tCountInit.ucTZMotor_Move_Num);
                pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                         "1w: HideErrCode=%d,ErrNum=%d\r\n", eErrorCode, pCountInfo->tCountInit.ucTZMotor_Move_Num);
            }else if(pCountInfo->tCountInit.ucTZMotor_Move_Num >= MAX_TZ_MOTOR_MOVE_NUM){
                //失败次数大于最大值，退出
                LOG_Error("1e: ErrCode=%d,ErrNum=%d", eErrorCode, pCountInfo->tCountInit.ucTZMotor_Move_Num);
                pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                          "1e: ErrCode=%d,ErrNum=%d\r\n", eErrorCode, pCountInfo->tCountInit.ucTZMotor_Move_Num);
                return eErrorCode;
            }
        }
    }
    //探针光耦触发
    pCountInfo->tCountInit.eTanzhenOC_Status = TRUE;
    
    /*** 负压过高检测 ***/
    lPress = Press_Value();
    if(lPress < MAX_BC_TEST_PRESS_VALUE)
    {
        //缓冲室负压过高，放气
        LOG_Warm("1w: Prees=%d>%d", lPress, MAX_BC_TEST_PRESS_VALUE);
        pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                 "1w: Prees=%d>%d\r\n", lPress, MAX_BC_TEST_PRESS_VALUE);
        Air_Bleed();
    }
    
    /*** 负压过低检测 ***/
    ulStartTick = HAL_GetTick();
    lPress = Press_Value();
    if(lPress > MIN_BC_TEST_PRESS_VALUE)  //-30kpa
    {
        pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen,\
                "1w: Prees=%d<%d\r\n", lPress, MIN_BC_TEST_PRESS_VALUE);
        //缓冲室负压过低，建压，连续三次
        for(i = 1; i <= MAX_BUILD_PRESS_NUM; i++)
        {           
            eErrorCode = Build_Press(RUN_MODE_COUNT, BC_TEST_PRESS_VALUE);  
            osDelay(BUILD_PERIOD_PER_TIME);
        }
        pCountInfo->tCountInit.ucBuildPress_Num = i;
        lPress = Press_Value();
        if(lPress > AIR_LEAK_PRESS)
        {
            //压力依旧过低         
            eErrorCode = ERROR_CODE_BUILD_PRESS;
            LOG_Error("1e: Build Press Failure, ErrCode=%d, Press=%d, Num=%d, T=%d", eErrorCode, lPress, pCountInfo->tCountInit.ucBuildPress_Num, HAL_GetTick()-ulStartTick);
            pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                      "1e: Build Press Failure, ErrCode=%d, Press=%d, Num=%d, T=%d\r\n", eErrorCode, lPress, pCountInfo->tCountInit.ucBuildPress_Num, HAL_GetTick()-ulStartTick);
            return eErrorCode;
        }else{
            //成功
            pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                    "1i: Build Press Success, Press=%d, Num=%d, T=%d\r\n", Press_Value(), pCountInfo->tCountInit.ucBuildPress_Num, HAL_GetTick()-ulStartTick);
        }   
    }
    
    
//===采集HGB的ADC数据===
    char HGB535_Log[80] = {0};
	char HGB415_Log[80] = {0};
	
	while(Light_OpenIsTimeup(5000) != HAL_OK){
		//这里确保开灯时间在5s左右
		osDelay(1);
	}
	
	
	
	usLen = 0;
	//先采集535波长数据
	for(i = 0; i < 10; i++){
		usTemp = Get_HGBAvgADC(10);
		
		pCountInfo->tCountInit.HGB535Adc[i*2]   = *(((uint8_t*)&usTemp)+1);
		pCountInfo->tCountInit.HGB535Adc[i*2+1] = *(((uint8_t*)&usTemp));
		usLen += sprintf(HGB535_Log+usLen, "%d,", usTemp);

		if(i == 3 || i == 8){
			Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status); //面板灯 
		}
	}
    pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "1i 535_ADC:%s\r\n", HGB535_Log);    
    
	usLen = 0;
	//采集415波长数据
	Light_Start(HGBLED_415,MachInfo.paramSet.current415);
	osDelay(3000);
	
	//采集415波长数据
	for(i = 0; i < 10; i++){
		usTemp = Get_HGBAvgADC(10);
		
		pCountInfo->tCountInit.HGB415Adc[i*2]   = *(((uint8_t*)&usTemp)+1);
		pCountInfo->tCountInit.HGB415Adc[i*2+1] = *(((uint8_t*)&usTemp));
		usLen += sprintf(HGB415_Log+usLen, "%d,", usTemp);

		if(i == 3 || i == 8){
			Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status); //面板灯 
		}
	}
    pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "1i 415_ADC:%s\r\n", HGB415_Log);
	
	//关闭HGB灯
    Light_Stop(); 
	
	//WBC恒流源开
    BC_CUR_SW_ON;
    App_Delay(RUN_MODE_COUNT, 300);
    
    
    /*** 检测定量电极是否触发 ***/
    Elec_Status = ELEC_STATUS;
    if(SET == Elec_Status)
    {
        LOG_Error("Elec(%d) Had Triggled", ELEC_STATUS);
        pCountInfo->tCountInit.eElec_Status = TRUE;
        //正常计数模式
        if(COUNT_MODE_NORMAL == eCountMode)
        {
            eErrorCode = ERROR_CODE_ELEC_TRIGGLE;
            LOG_Error("1e: ErrCode=%d, Elec=%d, eV=%d", eErrorCode, pCountInfo->tCountInit.eElec_Status, Elec_V());
            pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                            "1e: ErrCode=%d, Elec=%d, eV=%d\r\n", eErrorCode, pCountInfo->tCountInit.eElec_Status, Elec_V());
            return eErrorCode;
        }
    }
        
    /*** 前后池导通异常 检测 ***/
    usXK_V = XK_V();
    if(usXK_V < XK_V_NORMAL_LOW_THRESHOLD)
    {
        //前后池已导通，异常
        eErrorCode = ERROR_CODE_BSK_XKV_TOO_LOW; //
        LOG_Error("1e: ErrCode=%d, XK_V=%d,", eErrorCode, usXK_V);
        pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen,\
                  "1e: ErrCode=%d, XK_V=%d\r\n", eErrorCode, usXK_V);
        return eErrorCode;
    }
    
    
    /*** 前后池导通  检测***/
    usXK_V = XK_V();
    if(usXK_V > XK_V_NORMAL_LOW_THRESHOLD && usXK_V < XK_V_DISCON_THRESHOLD)
    {
        //前后池已导通，
        pCountInfo->tCountInit.eErrorCode = ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE;
        LOG_Warm("1w: XK Had ON, HideErrCode=%d, XK_V=%d,", pCountInfo->tCountInit.eErrorCode, usXK_V);
        pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                "1w: XK Had ON, HideErrCode=%d, XK_V=%d\r\n", pCountInfo->tCountInit.eErrorCode, usXK_V);
        //如果前后池已导通，则跳过液路建立阶段
        pCountInfo->tCountInit.eJump_Flag = TRUE; 
    }    
    
    /*** 检测空白信号(保存到日志中便于后续分析问题) ***/
    if(FEED_BACK_SUCCESS == Elec_Single_Analyse(&ulPluseNum, &dMean, &dCV))
    {       
        pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                "1i: Elec Single Analyse, PluseNum=%d, Mean=%.2f, CV=%.2f\r\n", ulPluseNum, dMean, dCV);
    }else{
        LOG_Warm("1w: Elec Single Analyse, PluseNum=%d, Mean=%.2f, CV=%0.2f", ulPluseNum, dMean, dCV);
        pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                "1w: Elec Single Analyse, PluseNum=%d, Mean=%.2f, CV=%.2f\r\n", ulPluseNum, dMean, dCV);
    }
    
    return eErrorCode;
}



/*
*   计数, 液路建立阶段
*/
ErrorCode_e Count_Liquid_Create(TestProject_e eTestProject, __IO CountMode_e eCountMode, __IO CountInfo_t *pCountInfo)
{
    extern __IO BC_Data_t   g_stBC_Data[BC_END];
    extern MachInfo_s MachInfo;
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
    //OutIn_Module_Position_e ePosition = OUTIN_MODULE_POSITION_NUKNOWN;
    uint32_t T2_Start = 0, T2_End = 0, ulIntervalTicks = 0, ulSumTicks = 0 ,ulT2_Temp = 0, ulT3_Temp = 0;
    int32_t lPress = 0, lStartPress = 0, lEndPress = 0;
    uint16_t i = 0, usXK_V = 0;
    uint8_t ucIndex = 0, uc2S15V_Flag = 0;
    pCountInfo->tLiquidCreate.usXK_V_Min = 0xFFFF;
    
    //信号稳定度分析
    pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%s\r\n", "[LiquidCreate]");
    
    T2_Start = HAL_GetTick();
    T2_End   = T2_Start;  
    
    //开始压力值
    lStartPress = Press_Value();
    //开阀吸液
    LIQUID_WAVE_OPEN;
    
    //液路建立阶段，最大时长10秒
    while(ulSumTicks + ulIntervalTicks <= LIQUID_CREATE_TIMEOUT) // 10S
    { 
        /*** 每隔250ms，记录一次小孔电压，同时更新最小值    ***/
        if(ulSumTicks + ulIntervalTicks - ulT2_Temp >= 250)
        {
            ulT2_Temp = ulSumTicks + ulIntervalTicks;
            if(ucIndex < LIQUID_CREATE_XK_V_NUM)
            {
                pCountInfo->tLiquidCreate.usXK_V_Buffer[ucIndex] = XK_V();
                if(pCountInfo->tLiquidCreate.usXK_V_Buffer[ucIndex] < pCountInfo->tLiquidCreate.usXK_V_Min)
                {
                    pCountInfo->tLiquidCreate.usXK_V_Min = pCountInfo->tLiquidCreate.usXK_V_Buffer[ucIndex];
                }
                ucIndex++;
            }

        }
        
        //每隔500ms.闪灯
        if(ulSumTicks + ulIntervalTicks - ulT3_Temp >= 500)
        {
            ulT3_Temp = ulSumTicks + ulIntervalTicks;
            Panel_Led_Status(RUN_MODE_COUNT, g_ePanel_LED_Status); //面板灯,闪
        }
        
        /*** 检测定量电极是否触发 ***/
        if(SET == ELEC_STATUS)
        {
            //正常计数模式
            if(COUNT_MODE_NORMAL == eCountMode)
            {
                eErrorCode = ERROR_CODE_ELEC_TRIGGLE_2;
                LOG_Error("2e: Elec Triggled,ErrCode=%d, Elec=%d, eV=%d", eErrorCode, ELEC_STATUS, Elec_V());
                pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                          "2e: Elec Triggled,ErrCode=%d, Elec=%d, eV=%d\r\n", eErrorCode, ELEC_STATUS, Elec_V());
                return eErrorCode;
            }
        }
        
        /*** 前后池异常检测 ***/
        usXK_V = XK_V();
        if(usXK_V < XK_V_NORMAL_LOW_THRESHOLD) //??? 改为才比较合理
        {
            //前后池已导通，异常
            eErrorCode = ERROR_CODE_BSK_XKV_TOO_LOW; 
            T2_End = HAL_GetTick();
            LOG_Error("2e: XK Err, ErrCode=%d, XK_V=%d, T=%d", eErrorCode, usXK_V, ulSumTicks+T2_End-T2_Start);
            pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                      "2e: XK Err, ErrCode=%d, XK_V=%d, T=%d\r\n", eErrorCode, usXK_V, ulSumTicks+T2_End-T2_Start);
            return eErrorCode;
        }

        /*** 小孔电压1.5秒内是否低于1.5V ***/
        T2_End = HAL_GetTick();
        //if(ulSumTicks+T2_End - T2_Start < 2000)
        if(ulSumTicks+T2_End - T2_Start < 1500) 
        {
            usXK_V = XK_V();
            if(usXK_V < XK_V_LIQUID_CREATE_THRESHOLD && uc2S15V_Flag == 0)
            {
                uc2S15V_Flag = 1;
                //记录此错误码传给算法
                pCountInfo->tLiquidCreate.eErrorCode = ERROR_CODE_BSK_2S_15V;         
                LOG_Warm("2w: 2S XKV<1.5V, ErrCode=%d, XK_V=%d, T=%d", pCountInfo->tLiquidCreate.eErrorCode, usXK_V, ulSumTicks+T2_End - T2_Start);
                pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen,\
                         "2w: 2S XKV<1.5V, ErrCode=%d, XK_V=%d, T=%d\r\n", pCountInfo->tLiquidCreate.eErrorCode, usXK_V, ulSumTicks+T2_End - T2_Start);
                if(ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE != pCountInfo->tLiquidCreate.eErrorCode)
				{
                    //再液路建立阶段，如果再首次开发前，已发生小孔电压已导通错误码ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE，不再标记ERROR_CODE_BSK_2S_15V错误码
                    pCountInfo->tLiquidCreate.eErrorCode = ERROR_CODE_BSK_2S_15V;  
                }
            }
        }      
        
        /*** 缓冲室压力检测及处理 ***/
        lPress = Press_Value();
        if(lPress > AIR_LEAK_PRESS)
        {          
            /*** 漏气 ***/
            
            //记录缓冲室压力异常发生的时间点
            T2_End = HAL_GetTick();
            ulIntervalTicks = T2_End - T2_Start;
            ulSumTicks += ulIntervalTicks; 
            ulIntervalTicks = 0;            
            T2_Start = T2_End;              
            
            //缓冲室压力异常的次数+1
            pCountInfo->tLiquidCreate.ucPress_ErrNum++;
              
            //缓冲室气压异常次数是否大于最大值（3次）
            if(pCountInfo->tLiquidCreate.ucPress_ErrNum > MAX_PRESS_ERROR_NUM){
                //气嘴漏气
                eErrorCode = ERROR_CODE_AIR_LEAK; 
                LOG_Error("2e: Press Err, ErrCode=%d, Press=%d, XK_V=%d, T=%d", Press_Value(), eErrorCode, XK_V(), ulSumTicks);
                pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen,\
                         "2e: Press Err, ErrCode=%d, Press=%d, XK_V=%d, T=%d\r\n", Press_Value(), eErrorCode, XK_V(), ulSumTicks);
                return eErrorCode;
            }
            
            //缓冲室气压异常次数是否为第一次
            if(pCountInfo->tLiquidCreate.ucPress_ErrNum == 1)
            {
                if(SAMPLE_NOT_ENOUGH_MIN_TIME < ulSumTicks  && ulSumTicks < SAMPLE_NOT_ENOUGH_MAX_TIME) //未加样
                {
                    //第一次漏气再2-8秒之间
                    eErrorCode = ERROR_CODE_SAMPLE_NONE; //未放入待测样本
                    LOG_Error("2e: None Liquid, ErrCode=%d, Press=%d, XK_V=%d, T=%d", eErrorCode, Press_Value(), XK_V(), ulSumTicks);
                    pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                             "2e: None Liquid, ErrCode=%d, Press=%d, XK_V=%d, T=%d\r\n", eErrorCode, Press_Value(),XK_V(), ulSumTicks);
                    return eErrorCode;
                }
            }else{
                LOG_Warm("2w: Press Not Enough, Press=%d, XK_V=%d, T=%d", lPress, XK_V(), ulSumTicks);
                pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                        "2w: Press Not Enough, Press=%d, XK_V=%d, T=%d\r\n", lPress, XK_V(), ulSumTicks);
            }
                       
            //重新气嘴对接
            //eErrorCode = Clamp_Motor_Out_then_In(MOTOR_MODE_NORMAL);
            Clamp_Motor_Out_then_In(MOTOR_MODE_NORMAL);
            
            
            //气嘴对接后，重新建压，连续建压3次数(3次)
            for(i = 0; i < MAX_BUILD_PRESS_NUM; i++)
            {
                eErrorCode = Build_Press(RUN_MODE_COUNT, BC_TEST_PRESS_VALUE);
                osDelay(BUILD_PERIOD_PER_TIME);
            }
            if(ERROR_CODE_BUILD_PRESS == eErrorCode)
            {
                //建压失败
                LOG_Error("2e: Build Press Fail, ErrCode=%d, Press=%d, T=%d", eErrorCode, Press_Value(), pCountInfo->tLiquidCreate.ulT2_Passed);
                pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                          "2e: Build Press Fail, ErrCode=%d, Press=%d, T=%d\r\n", eErrorCode, Press_Value(), pCountInfo->tLiquidCreate.ulT2_Passed);
                return eErrorCode;
            }
            
             //无异常，更新计时起点，统计有效耗时，重新开始计数
             T2_End = HAL_GetTick();
             T2_Start = T2_End;
        }       
        
        /*** 堵孔检测及处理(从5秒开始、小孔电压已下降、堵孔次数小于最大数（3次）) ***/
//        if(ulSumTicks > 5000 && pCountInfo->tLiquidCreate.usXK_V_Min < XK_V_DISCON_THRESHOLD && (pCountInfo->tLiquidCreate.ucDK_Num < MAX_DK_REPEAT_NUM))
//        {
//            //@todo 堵孔流程，注意时间更新
//        }
        
//        /*** 8秒后，开启信号检测 ***/
//        if(ulSumTicks > 5000 && ucSignalCheckFlag == 0) //这个时间待定？？？
//        {
//            //ucSignalCheckFlag = 1;
//            //Elec_Single_Analyse
//             
//            //特征初始化
//            pulseMidFeatureInit(&midFeature);	
//            midFeature.smpRate = smpRate;      
//                
//            //开辟算法内存空间
//            pulseInfo = (pulse_info_i_t*)SDRAM_ALGO_ADDR;
//            memset((void*)pulseInfo, 0, SDRAM_ALGO_LEN);
//            
//            //计时初始化
//            ulStartTick = HAL_GetTick();
//            ulEndTick   = ulStartTick;
//            //关闭调度器
//            osKernelLock();
//            //采集数据
//            //使能血细胞信号采集
//            BC_Enable(BC_WBC);
//            
//            //等待采集到1024字节数据
//            while(ulEndTick - ulStartTick < 5){
//                if(BC_ADC_PART_1 == g_stBC_Data[BC_WBC].ucFlag){
//                    //关闭采集
//                    BC_Disable(BC_WBC);
//                    break;
//                }
//                ulEndTick = HAL_GetTick();
//            }
//            //关闭采集
//            BC_Disable(BC_WBC);
//            //恢复调度器
//            osKernelUnlock();

//            Count++;
//            //复位标志
//            g_stBC_Data[BC_WBC].ucFlag = BC_ADC_PART_NONE;
//            
//            //计算脉冲个数
//            wbcPulseIdentify(pulseInfo, &pulseNum, (const uint8 *)&g_stBC_Data[BC_WBC].ucBuf[0], DATA_UINT_SIZE, &midFeature);
//            
//            //16为数据扩展成32数据
//            for(i=0;i<512;i++){
//                RawChangeBuf[i] = g_stBC_Data[BC_WBC].ucBuf[i];
//            }
//            
//            //计算mean
//            CurrentMean = getMean_i(RawChangeBuf,512);
//            MeanSum += CurrentMean;
//            
//            //计算CV
//            CurrentCV = getCv_i(RawChangeBuf,512,0);
//            CVSum += CurrentCV;
//        }    
        
        /*** 更新计时 ***/
        T2_End = HAL_GetTick();
        ulIntervalTicks = T2_End - T2_Start;  
        //
        osDelay(10);
    } //end 
    
   
    /*** 更新耗时 ***/
    pCountInfo->tLiquidCreate.ulT2_Passed = ulSumTicks + ulIntervalTicks;
    /*** 关闭液阀 ***/
    LIQUID_WAVE_CLOSE;
   
    
    /*** 计算信号 Mean、Num、CV ***/
    //计算平均MEAN
//    MeanAvg = MeanSum/Count;
//    //计算平均CV
//    CVAvg = CVSum/Count*100;
//    LOG_Info("pluseNum=%d, MeanAvg=%f, CVAvg=%f", pulseNum, MeanAvg, CVAvg);
//    pCountInfo->usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->ucaLogBuffer, pCountInfo->usLogLen,\
//            "pluseNum=%d, MeanAvg=%f, CVAvg=%f\r\n", pulseNum, MeanAvg, CVAvg);
                
    
    /*** 保存小孔电压信息到日志 ***/
    pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%s", "2i:XK_V=");
    for(i = 0; i < ucIndex; i++)
    {    
        pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%d,", pCountInfo->tLiquidCreate.usXK_V_Buffer[i]);
    }
    pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "\r\n");
    
    /*** 结束后，检测液路建立过程中小孔变化情况 ***/
    if(pCountInfo->tLiquidCreate.usXK_V_Min > XK_V_DISCON_THRESHOLD)  //2200mv
    {
        //小孔电压的最小值大于 断路阈值，小孔电压,未下降
        /*** 探针重新对接 ***/
        for(i = 1; i <= MAX_LIQUID_CREATE_TZ_REPEAT_NUM; i++)    //3
        {
            Clamp_Motor_Out_then_In(MOTOR_MODE_NORMAL);
            osDelay(1000);
            if(SET == XK_V_Is_LessThan(XK_V_DISCON_THRESHOLD))
            {
                //重对接后，小孔电压正常
                break;
            }else{
                //重对接后，小孔电压未下降，开阀吸液
                LIQUID_WAVE_OPEN;
                App_Delay(RUN_MODE_COUNT, 500);
                LIQUID_WAVE_CLOSE;
                App_Delay(RUN_MODE_COUNT, 500);
            }            
        }
        //
        if(i >=MAX_LIQUID_CREATE_TZ_REPEAT_NUM)
        {
            pCountInfo->tLiquidCreate.ucReConnNum = i - 1;
            //三次对接，检测到的小孔电压未下降
            pCountInfo->tLiquidCreate.eErrorCode = ERROR_CODE_WBC_ELEC_TOUCH;
            LOG_Error("2e: XKV Still>%d, After Elec ReConn, HideErrCode=%d, ReConnNum=%d, XK_V=%d, T=%d", XK_V_DISCON_THRESHOLD, pCountInfo->tLiquidCreate.eErrorCode, i-1, XK_V(), pCountInfo->tLiquidCreate.ulT2_Passed);
            pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                      "2e: XKV Still>%d, After Elec ReConn, HideErrCode=%d, ReConnNum=%d, XK_V=%d, T=%d\r\n", XK_V_DISCON_THRESHOLD, pCountInfo->tLiquidCreate.eErrorCode, i-1, XK_V(), pCountInfo->tLiquidCreate.ulT2_Passed);
            
            //return eErrorCode;
        }else{
            pCountInfo->tLiquidCreate.ucReConnNum = i;
            pCountInfo->tLiquidCreate.eErrorCode = ERROR_CODE_WBC_ELEC_TOUCH;
            LOG_Warm("2w: XKV Still>%d, After Elec ReConn, HideErrCode=%d, ReConnNum=%d, XK_V=%d, T=%d", XK_V_DISCON_THRESHOLD, pCountInfo->tLiquidCreate.eErrorCode, i, XK_V(), pCountInfo->tLiquidCreate.ulT2_Passed);
            pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                     "2w: XKV Still>%d, After Elec ReConn, HideErrCode=%d, ReConnNum=%d, XK_V=%d, T=%d\r\n", XK_V_DISCON_THRESHOLD, pCountInfo->tLiquidCreate.eErrorCode, i, XK_V(), pCountInfo->tLiquidCreate.ulT2_Passed);
        }
    }
    
    //结束压力值
    lEndPress = Press_Value();
    pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen,\
            "XKV=%d, StartPress=%d, EndPress=%d\r\n", XK_V(), lStartPress, lEndPress);
    //
    return eErrorCode;
}





/*
*   计数, 信号稳定度检测阶段
*/
ErrorCode_e Count_Monitor_Signal(TestProject_e eTestProject, __IO CountMode_e eCountMode, __IO CountInfo_t *pCountInfo)
{
    extern MachInfo_s MachInfo;
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
    double dMean = 0, dCV = 0;
    uint32_t ulPluseNum = 0;
    int32_t lStartPress = 0, lEndPress = 0;
    uint8_t Elec_Status = 0;
    
    pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%s", "[MonitorSigal]\r\n");
    
    //开始压力
    lStartPress = Press_Value();
    //重复检测次数不能大于最大值（5次）
    pCountInfo->tSignalMonitor.ucMonitor_Num = 0;
    while(pCountInfo->tSignalMonitor.ucMonitor_Num < MAX_SIGNAL_MONITOR_TZ_REPEAT_NUM)
    {                   
        /*** 先静止一小段时间（1秒）***/
        osDelay(1000);
        
        /*** 信号稳定判断 ***/
        if(FEED_BACK_SUCCESS != Elec_Single_Analyse(&ulPluseNum, &dMean, &dCV))
        {
            //信号不稳定
            pCountInfo->tSignalMonitor.ucMonitor_Num++;
            pCountInfo->tSignalMonitor.eErrorCode = ERROR_CODE_MONITOR_SIGNAL_ERR;
            LOG_Error("3w: Signal Analysis Err, HideErrCode=%d, Num=%d, PluseNum=%d, Mean=%.2f, CV=%.2f",\
                       pCountInfo->tSignalMonitor.eErrorCode, pCountInfo->tSignalMonitor.ucMonitor_Num, ulPluseNum, dMean, dCV);
            pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                      "3w: Signal Analysis Err, HideErrCode=%d, Num=%d, PluseNum=%d, Mean=%.2f, CV=%.2f\r\n",\
                       pCountInfo->tSignalMonitor.eErrorCode, pCountInfo->tSignalMonitor.ucMonitor_Num, ulPluseNum, dMean, dCV);
        }else{
            //信号稳定, 退出本阶段，转入下一阶段
            pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                     "3i: Signal Analysis Ok, Num=%d, PluseNum=%d, Mean=%.2f, CV=%.2f\r\n", pCountInfo->tSignalMonitor.ucMonitor_Num, ulPluseNum, dMean, dCV);
            break;
        }
          
        //
        /*** 信号不稳定的次数是否大于最大值（5次）***/
        if(pCountInfo->tSignalMonitor.ucMonitor_Num >= MAX_SIGNAL_MONITOR_TZ_REPEAT_NUM)
        {
            eErrorCode = ERROR_CODE_MONITOR_SIGNAL_ERR;
            //pCountInfo->tSignalMonitor.eErrorCode = eErrorCode;             
            LOG_Error("3e: Signal Analysis Err, ErrCode=%d, Num=%d", eErrorCode, pCountInfo->tSignalMonitor.ucMonitor_Num);
            pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                                "3e: Signal Analysis Err, ErrCode=%d, Num=%d\r\n", eErrorCode, pCountInfo->tSignalMonitor.ucMonitor_Num);
            return eErrorCode;
        }
    
        /*** 检测定量电极是否触发 ***/
        Elec_Status = ELEC_STATUS;
        if(SET == Elec_Status)
        {           
            //正常计数模式
            if(COUNT_MODE_NORMAL == eCountMode)
            {
                eErrorCode = ERROR_CODE_ELEC_TRIGGLE_2;
                LOG_Error("3e: Elec Had Triggled, ErrCode=%d, Elec=%d, Num=%d, eV=%d", eErrorCode, Elec_Status, pCountInfo->tSignalMonitor.ucMonitor_Num, Elec_V());   
                pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                         "3e: Elec Had Triggled, ErrCode=%d, Elec=%d, Num=%d, eV=%d\r\n",eErrorCode, Elec_Status, pCountInfo->tSignalMonitor.ucMonitor_Num, Elec_V());                
                return eErrorCode;
            }
        }
    
        //探针重新对接
        pCountInfo->tSignalMonitor.eErrorCode = Clamp_Motor_Out_then_In(MOTOR_MODE_NORMAL);
        if(pCountInfo->tSignalMonitor.eErrorCode != ERROR_CODE_SUCCESS)
        {  
            LOG_Error("3e: ReConn Err, ErrCode=%d, Num=%d", pCountInfo->tSignalMonitor.eErrorCode, pCountInfo->tSignalMonitor.ucMonitor_Num);
            pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                      "3e: ReConn Err, ErrCode=%d, Num=%d\r\n", pCountInfo->tSignalMonitor.eErrorCode, pCountInfo->tSignalMonitor.ucMonitor_Num);
        }
    }
    
    //结束压力值
    lEndPress = Press_Value();
    pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen,\
            "XKV=%d, StartPress=%d, EndPress=%d\r\n", XK_V(), lStartPress, lEndPress);
    
    
    return eErrorCode;
}





/*
*   计数, 信号采集分析和结束阶段
*/

ErrorCode_e Count_Before_Count(TestProject_e eTestProject, __IO CountMode_e eCountMode, __IO CountInfo_t *pCountInfo)
{
    extern MachInfo_s MachInfo;
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
//    uint32_t ulStartTick = 0, ulEndTick = 0, ulTemp = 0;
    int32_t lPress = 0;
    uint16_t i = 0;
    uint8_t Elec_Status = 0;
    
    pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, "%s\r\n", "[Count]");
    
    /*** 缓冲室压力检测及处理 ***/
    lPress = Press_Value();
    if(lPress > AIR_LEAK_PRESS) //
    { 
        //重新建压，建压最大次数(3次)
        for(i = 0; i < MAX_BUILD_PRESS_NUM; i++)
        {
            eErrorCode = Build_Press(RUN_MODE_COUNT, BC_TEST_PRESS_VALUE);
            osDelay(BUILD_PERIOD_PER_TIME);
        } 
    }
    if(ERROR_CODE_SUCCESS != eErrorCode)
    {
        eErrorCode = ERROR_CODE_BUILD_PRESS;
        lPress = Press_Value();
        pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                "4e: Before Count Build Press Fail, ErrCode=%d, Num=%d, lPress=%d\r\n", eErrorCode, i, lPress);
        return eErrorCode;
    }
    

    /*** 检测定量电极是否触发 ***/
    Elec_Status = ELEC_STATUS;
    if(SET == Elec_Status)
    {
        //正常计数模式
        if(COUNT_MODE_NORMAL == eCountMode)
        {
            eErrorCode = ERROR_CODE_ELEC_TRIGGLE_2;
            return eErrorCode;
        }
        LOG_Error("4e: Before Count Elec Had Triggled, Elec=%d, ErrCode=%d", Elec_Status, eErrorCode);
        pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                               "4e: Before Count Elec Had Triggled, Elec=%d, ErrCode=%d\r\n", Elec_Status, eErrorCode);
    }
    
    //
    return eErrorCode;
}



/*
*   计数过程，
*   eType：WBC, RBC
*   ulCountMode: 计数模式
*   pDataBuffer: 结果数据buffer
*/
ErrorCode_e Count_Process(TestProject_e eTestProject, __IO CountMode_e eCountMode, __IO CountInfo_t *pCountInfo, __IO uint8_t *pDataBuffer, uint16_t usBufferLen)
{
    extern MachInfo_s MachInfo;
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
    
    /*** 初始化阶段 ***/
    eErrorCode = Count_Init(eTestProject, eCountMode, pCountInfo); 
    
	if(ERROR_CODE_OUTIN_IN == eErrorCode || ERROR_CODE_CLAMP_OUT == eErrorCode || ERROR_CODE_AIR_LEAK_COUNTING_2 == eErrorCode)
    {
        //如果进仓异常，则需要松开夹子
        if(FEED_BACK_SUCCESS != Motor_Run_AddStep(MOTOR_MODE_NORMAL, MOTOR_CLAMP, MOTOR_DIR_IN, 1500))
        {
            //eErrorCode = ERROR_CODE_CLAMP_IN;
        }
    }
	
    //
    if(eErrorCode != ERROR_CODE_SUCCESS)
    {
		//关闭HGB灯
		Light_Stop();
		
        LOG_Info("--- Count Init Error---");
        return eErrorCode;
    }
    
    //WBC恒流源开（确保接下来是打开状态）
    BC_CUR_SW_ON;
    
    //
    if(pCountInfo->tCountInit.eJump_Flag == FALSE)
    {
       /*** 液路建立阶段 ***/
        eErrorCode = Count_Liquid_Create(eTestProject, eCountMode, pCountInfo);
        if(eErrorCode != ERROR_CODE_SUCCESS)
        {
            LOG_Info("--- Count Liquid Create Error ---");
            return eErrorCode;
        } 
    }else{
        //跳过液路建立阶段
        pCountInfo->tCountLog.usLogLen += Count_Log_Output(LOG_TYPE_COUNT_LOG, pCountInfo->tCountLog.ucaLogBuffer, pCountInfo->tCountLog.usLogLen, \
                                                    "2i:%s\r\n", "Jump Over Liquid Create");
    }
    
    /*** 信号检测阶段 ***/
    eErrorCode = Count_Monitor_Signal(eTestProject, eCountMode, pCountInfo);
    if(eErrorCode != ERROR_CODE_SUCCESS)
    {
        LOG_Info("--- Monitor Signal Error ---");
        return eErrorCode;
    }
    
    /*** 计数采样***/
    eErrorCode = Count_Before_Count(eTestProject, eCountMode, pCountInfo);
    if(eErrorCode != ERROR_CODE_SUCCESS)
    {
        LOG_Info("--- Before Signal Analysis Error ---");
        return eErrorCode;
    }
    eErrorCode = Algo_BC_Handler(eCountMode, pCountInfo, pDataBuffer, usBufferLen); //结束阶段，也在该函数中
    if(eErrorCode != ERROR_CODE_SUCCESS)
    {
        return eErrorCode;
    }    
    
    //关阀
    LIQUID_WAVE_CLOSE;
    
    //
    return eErrorCode;
}


/*
*   人工校准参数，消息处理（保存参数）
*/
FeedBack_e Calibration_Msg_Handler(uint8_t *pMsg)
{
    extern MachInfo_s MachInfo;
    CalibrationMsg_t *ptCalibrationMsg = (CalibrationMsg_t*)pMsg;
    
    //赋值参数
    MachInfo.calibration.fWBC = ptCalibrationMsg->calibration.fWBC;
    MachInfo.calibration.fHGB = ptCalibrationMsg->calibration.fHGB;
    strncpy((char*)MachInfo.calibration.ucaWBC_CalibrationDate,(char*)ptCalibrationMsg->calibration.ucaWBC_CalibrationDate, DATE_TIME_LEN);
    strncpy((char*)MachInfo.calibration.ucaHGB_CalibrationDate, (char*)ptCalibrationMsg->calibration.ucaHGB_CalibrationDate, DATE_TIME_LEN);
    
    //保存参数
	EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CALIBRATION,(uint8_t*)&MachInfo.calibration,sizeof(MachInfo.calibration));
    
    //
    return FEED_BACK_SUCCESS;
}



/*
*   设置调试界面，信息处理，用于设置计数模式相关信息
*/
FeedBack_e Debug_Msg_Handler(uint8_t *pMsg)
{
    __IO uint32_t ulCountMode;
    
    ulCountMode = Get_CountMode_Info()->ulCountMode;
    ParaSetDebugInfo_Msg_t *pDebugMsg = (ParaSetDebugInfo_Msg_t*)pMsg;
    
    if(pDebugMsg->sParaSetDebugInfo.gzTestWBCFlag == 0 && pDebugMsg->sParaSetDebugInfo.jscReuseFlag == 0)
    {
        //工装测试=0，计数池重复使用模式=0，都失效，则恢复为正常计数模式
        ulCountMode = COUNT_MODE_NORMAL;
    }else if(pDebugMsg->sParaSetDebugInfo.gzTestWBCFlag == 1 && pDebugMsg->sParaSetDebugInfo.jscReuseFlag == 0){
        //工装测试=1，计数池重复使用模式=0，则为外部工装计数模式
        if(pDebugMsg->sParaSetDebugInfo.gzTestCount == 1 || pDebugMsg->sParaSetDebugInfo.gzTestCount == 0)
        {
            //单次外部工装模式
            ulCountMode = COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE;
        }else{
            //重复（多次）外部工装模式
           ulCountMode = COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO; 
        } 
        Get_CountMode_Info()->ulOutGZ_CountNum = pDebugMsg->sParaSetDebugInfo.gzTestCount;       
    }else if(pDebugMsg->sParaSetDebugInfo.gzTestWBCFlag == 0 && pDebugMsg->sParaSetDebugInfo.jscReuseFlag == 1){
        //工装测试=0，计数池重复使用模式=1，则为计数池重复使用模式
        ulCountMode = COUNT_MODE_RE_USE;
        Get_CountMode_Info()->ulReUseMode_Delay = pDebugMsg->sParaSetDebugInfo.countDelayTime;    
    }else if(pDebugMsg->sParaSetDebugInfo.gzTestWBCFlag == 1 && pDebugMsg->sParaSetDebugInfo.jscReuseFlag == 1){
        //工装测试=1，计数池重复使用模式=1，该设置无效（界面须做二选一处理）， 这里不做处理，保持模式
        ulCountMode = COUNT_MODE_NORMAL;
    }
    
    Get_CountMode_Info()->ulCountMode = ulCountMode;
    
    return FEED_BACK_SUCCESS;
}




/*
    ******************* service ********************************************
*/







/*
    ******************* Login ********************************************
*/
/*
*   开机登录自检流程，
*   
    1、正负12V检测
    2、温度检测
    3、EEPROM检测
    4、CAT1检测
    5、打印机检测
    6、计数仓出仓检测
    7、探针对接断开检测
    8、小孔及56V恒流源检测
    9、液阀、气阀检测
    10、建压检测
    11、计数仓进仓检测
    12、探针对接导通检测
    13、血细胞模块检测
    14、HGB光路检测
  
*   //密闭性
*/
FeedBack_e Login_SelfCheck(uint16_t usCmd)
{
    extern osSemaphoreId xSema_OutIn_Key;
	extern osSemaphoreId xSema_Count_Key;
    extern __IO OutIn_Module_Position_e g_eOutIn_Key;
    extern MachInfo_s MachInfo;
	extern PrinterCurrentState_s PrinterCurrentState;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osMessageQueueId_t CATQueueHandle;
	extern osMessageQueueId_t CATQueueOtherHandle;
    int i = 0;
    uint32_t ulTotalSector = 0, ulFreeSector = 0;
    int32_t lFirstPress = 0, lSecondPress = 0;
	CATQueueInfoBuf_s CATQueueInfoBuf = {0};
    uint8_t ucTemp = 0;
	
    
    ErrorCode_e eErrorCode = ERROR_CODE_SUCCESS;
    uint8_t ucaMsgBuffer[BACKEND_TO_UI_MSG_BUFFER_LEN] =  {0};
    //转为消息结构体
    Login_SelfCheck_t *ptLogin_SelfCheck = (Login_SelfCheck_t*)ucaMsgBuffer;
    //填充消息头
    ptLogin_SelfCheck->ucErrorNum = 0;
    ptLogin_SelfCheck->tMsgHead.usCmd      = usCmd;
    ptLogin_SelfCheck->tMsgHead.eErrorCode = ERROR_CODE_SUCCESS;
    ptLogin_SelfCheck->tMsgHead.usMsgLen   = sizeof(Login_SelfCheck_t) - MSG_HEAD_LEN;
    
    
    //WBC恒流源开
     BC_CUR_SW_ON;
	
	//提前打开HGB灯
	Light_Start(HGBLED_535,MachInfo.paramSet.current535);
    
    if(sizeof(Login_SelfCheck_t) > BACKEND_TO_UI_MSG_BUFFER_LEN)
    {
        LOG_Error("Login_SelfChek_t Size > ucaMsgBuffer");
        return FEED_BACK_FAIL;
    }
    
    /***** 9、液阀、气阀检测 *****/
    // 开液阀 2次，每次500ms
    Liquid_Valve_Exec(2, 500);
    Liquid_Valve_Exec(2, 500);
    //发送消息到界面
    ptLogin_SelfCheck->ucProgress = 20; //完成20%
    BackEnd_Put_Msg(ucaMsgBuffer);
    
        /***** 建压 8s *****/
    for(i = 0; i < 2; i++)
    {
        eErrorCode = Build_Press(RUN_MODE_NORMAL, BC_TEST_PRESS_VALUE);
        osDelay(1000);  
    }
    if(eErrorCode != ERROR_CODE_SUCCESS)
    {
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = eErrorCode;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.builePress = 1;
    }
    lFirstPress = Press_Value();
    //发送消息到界面
    ptLogin_SelfCheck->ucProgress = 40; //完成40%
    BackEnd_Put_Msg(ucaMsgBuffer);
    osDelay(50); 
    
    /***** 负12V：误差0.5V*****/
    eErrorCode = Power_12VN_Is_Normal();
    if(eErrorCode != ERROR_CODE_SUCCESS)
    {
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = eErrorCode;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.n12vFlag = 1;
    }
    
    /***** 正12V：误差0.5V*****/
    eErrorCode = Power_12VP_Is_Normal();
    if(eErrorCode != ERROR_CODE_SUCCESS)
    {
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = eErrorCode;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.p12vFlag = 1;
    }   
    
    /***** 进出仓光耦检测 *****/
    if(OC_Status(OC_INDEX_OUTIN_OUT) == OC_Status(OC_INDEX_OUTIN_IN))
    {
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_OUTIN_OC;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.ocInOut = 1;
    }
    
    /***** 夹子光耦检测 *****/
    if(OC_Status(OC_INDEX_CLAMP_OUT) == OC_Status(OC_INDEX_CLAMP_IN))
    {
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_CLAMP_OC;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.ocTanzhen = 1;
    }
    
    /***** 计数仓出仓检测 *****/
    eErrorCode = OutIn_Motor_Out(MOTOR_MODE_NORMAL, MOTOR_OUTIN_MAX_STEP, MachInfo.paramSet.ulOutIn_Out_AddStep);
    if(eErrorCode != ERROR_CODE_SUCCESS)
    {
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = eErrorCode;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.outInMotoOut = 1;
    }
    
    //发送消息到界面
    ptLogin_SelfCheck->ucProgress = 50; //完成50%
    BackEnd_Put_Msg(ucaMsgBuffer);
    
    
    /***** 探针对接断开检测 *****/
    eErrorCode = Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
    if(eErrorCode != ERROR_CODE_SUCCESS)
    {
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = eErrorCode;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.campleMotoSf = 1;
    }
   
   /***** 8、小孔及56V恒流源检测 *****/
   eErrorCode = BC_Current_Is_Normal();
   if(eErrorCode != ERROR_CODE_SUCCESS)
   {
       ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = eErrorCode;
	   CATQueueInfoBuf.para.CATMachSelfCheckResult.xk = 1;
   }   
    
     /***** 11、计数仓进仓检测 *****/
    eErrorCode = OutIn_Motor_In(MOTOR_MODE_NORMAL,  MOTOR_OUTIN_MAX_STEP, MachInfo.paramSet.ulOutIn_In_AddStep);
    if(eErrorCode != ERROR_CODE_SUCCESS)
    {
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = eErrorCode;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.outInMotoIn = 1;
    }
    
    //发送消息到界面
    ptLogin_SelfCheck->ucProgress = 75; //完成75%
    BackEnd_Put_Msg(ucaMsgBuffer);
    osDelay(25);  
    
    /***** 12、探针对接导通检测 *****/
    eErrorCode = Clamp_Motor_Out(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_Out_AddStep);
    if(eErrorCode != ERROR_CODE_SUCCESS)
    {
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = eErrorCode;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.campleMotoGd = 1;
    }
    
    /***** 13、血细胞模块检测  *****/   
    if(RESET != ConutCell_Is_Exist()) 
    {   
        //有血细胞模块 或 检测模块异常
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_CELL_CHECK_MODULE;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.jscCheck = 1;
    }
    
    /***** 14、HGB光路检测  *****/   
    while(Light_OpenIsTimeup(5000) != HAL_OK){
		//这里确保开灯时间在5s左右
		osDelay(1);
	}
    if(HGB_ADC() < MachInfo.paramSet.KQADC535 - 500 || HGB_ADC() > 3700)
    {
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_HGB_MODULE;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.hgb = 1;
    }else{
		Light_Start(HGBLED_415,MachInfo.paramSet.current415);
		osDelay(2000);
		
		if(HGB_ADC() < MachInfo.paramSet.KQADC415 - 500 || HGB_ADC() > 3700)
		{
			ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_HGB_MODULE;
			CATQueueInfoBuf.para.CATMachSelfCheckResult.hgb = 1;
		}
	}
	
    Light_Stop();
    
    //发送消息到界面
    ptLogin_SelfCheck->ucProgress = 90; //完成90%
    BackEnd_Put_Msg(ucaMsgBuffer);
    osDelay(25);  
    
    /***** 15、磁盘容量检测  *****/     
    Nand_Volume_Info(&ulTotalSector,&ulFreeSector);
    if(ulFreeSector*100/ulTotalSector < 10)
    {
        // 磁盘容量不足10%;
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_DISK_CAPACITY;
    }
    
    /***** 密闭性自检 小于5kpa为正常	*****/
    lSecondPress = Press_Value();
    if(abs(lFirstPress - lSecondPress) > 5000)
    {
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_AIR_TIGHT;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.mbxCheck = 1;
    }

	/***** 温度检测 *****/
    
    /***** EEPROM检测 *****/
    ucTemp = 0xA5;
    EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_AGINGTEST,(uint8_t*)&ucTemp,1);
    ucTemp = 0;
    EEPROMReadData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_AGINGTEST,(uint8_t*)&ucTemp,1);
    if(ucTemp != 0xA5){
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_EEPROM;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.eepromFlag = 1;
    }
    
    /***** SPI Flash检测 *****/
	SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
    if(_SPIFlash_ReadId() != SPIFLASH_JEDEC_ID){
        //更改SPI模式, 为低速模式
        ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_SPI_FLASH_CONN_ERR;
		CATQueueInfoBuf.para.CATMachSelfCheckResult.spiFlag = 1;
    }
    SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
    
    
    /***** CAT1检测 *****/
	//英文模式不检CAT
	if(MachInfo.systemSet.eLanguage == LANGUAGE_CHINESE){
		if(CATGlobalStatus.flag.conn != 1){
			ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_CAT_ONE;
			CATQueueInfoBuf.para.CATMachSelfCheckResult.cat = 1;
		}
	}
    
    /***** 打印机检测 *****/
    if(PrinterCurrentState.readyState == 0x55){
		 ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_PRINT_NO_PAPER;
	}else if(PrinterCurrentState.readyState != 0x55 && PrinterCurrentState.readyState != 0x04){
		 ptLogin_SelfCheck->eaErrorCodeBuffer[ptLogin_SelfCheck->ucErrorNum++] = ERROR_CODE_NO_PRINTER;
	}
    
    //WBC恒流源关
    BC_CUR_SW_OFF;
    
    //发送消息到界面
    ptLogin_SelfCheck->ucProgress = 100; //完成100%
    BackEnd_Put_Msg(ucaMsgBuffer);
    osDelay(25);
	
	//发送给CAT模块将自检结果发送到服务端
	CATQueueInfoBuf.msgType = SELF_CHECK_RESULT_CAT_MSG_TYPE;
	osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0, 0);
	
	//发送自检结果给第三方服务端
	if(CATGlobalStatus.flag.noOtherServer == 0){
		CATQueueInfoBuf.msgType = SELF_CHECK_RESULT_CAT_MSG_TYPE;
		osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0, 0);
	}
	
	//将自检异常时记录到运行日志中
	if(ptLogin_SelfCheck->ucErrorNum != 0){
		RunLog_s RunLog = {0};
		RTC_HYM8563Info_s RTCInfo = {0};
		uint8_t i = 0;
		uint16_t Len = 0;
		
		//保存运行日志
		RunLog.logHead.num = g_tDataManage.ulRunLog_SN;//MachRunPara.tDataManage.lastRunLogNum;
		RunLog.logHead.eAccountType = MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].type;
		RTC_HYM8563GetTime(&RTCInfo);
		sprintf(RunLog.logHead.dataTime,"20%d%d-%d%d-%d%d %d%d:%d%d:%d%d",
						RTCInfo.year.bcd_h,RTCInfo.year.bcd_l,
						RTCInfo.month.bcd_h,RTCInfo.month.bcd_l,
						RTCInfo.day.bcd_h,RTCInfo.day.bcd_l,
						RTCInfo.hour.bcd_h,RTCInfo.hour.bcd_l,
						RTCInfo.min.bcd_h,RTCInfo.min.bcd_l,
						RTCInfo.sec.bcd_h,RTCInfo.sec.bcd_l);
		Len = sprintf(RunLog.logHead.shortStr,"Self Checking Fail");
		RunLog.logHead.shortStr[Len] = '\0';
		
		for(i=0;i<ptLogin_SelfCheck->ucErrorNum;i++){
			RunLog.logHead.logLen += sprintf(RunLog.Str+RunLog.logHead.logLen,"E%u\n",ptLogin_SelfCheck->eaErrorCodeBuffer[i]);
		}
		
		RunLog.Str[RunLog.logHead.logLen] = '\0';
		RunLog.logHead.timestamp = GetCurrentTimestamp(RTCInfo);
        RunLog.usCrc = CRC16((uint8_t*)&RunLog, sizeof(RunLog_s)-2);
		
		Fatfs_Write_RunLog(Update_RunLog_Tail(), &RunLog);
		Add_RunLog_Tail();
		
		g_tDataManage.ulRunLog_SN++;
		Save_DataMange_Info(&g_tDataManage);
	}

    return FEED_BACK_SUCCESS;
}



/*
*   加载用户数据（计数数据等）
*/
void Load_Account_Data(void)
{  
    extern __IO MachRunPara_s MachRunPara;
    extern __IO DataManage_t g_tDataManage;
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS, eFeedBack2 = FEED_BACK_SUCCESS;
    Msg_Head_t tMsgHead = {0};
    
    //上电后，获取最新的一条数据()，赋值给UI_Info（用于显示上次分析结果）。 如果没有记录，则返回，不做处理
    eFeedBack = UI_Get_Lastet_WBCHGB_TestInfo(&MachRunPara.tWBCHGB_TestInfo, &g_tDataManage);
    //上电后， 初始化下一样本信息
    NextSample_Info_Init(&MachRunPara.tNextSample, &g_tDataManage); 
    
    //上电后，初始化,列表回顾显示数据
    eFeedBack2 = UI_ListView_Data_Init(&g_tDataManage);
    
    //上电后,质控数据的加载
    Load_QC_WBCHGB_Data();
    
    //加载数据失败
    if(eFeedBack == FEED_BACK_FAIL || eFeedBack2 == FEED_BACK_FAIL)
    {
        tMsgHead.usCmd = CMD_ANALYSIS_LOAT_DATA_FAIL;
        BackEnd_Put_Msg((uint8_t*)&tMsgHead);
    }else{
        tMsgHead.usCmd = CMD_LOGIN_LOAD_USER_DATA_FINISH;
        BackEnd_Put_Msg((uint8_t*)&tMsgHead);  
    }
}



/*
*    ******************* PowerOff ********************************************
*/
void PowerOff_Exec(uint8_t *pMsg)
{
    extern __IO uint8_t g_OutIn_Module_Status;
    extern __IO OutIn_Module_Position_e g_eOutIn_Key;
	extern osSemaphoreId xSema_OutIn_Key;
    uint16_t i = 0;

	//当进出电机，还未执行完成时，等待其完成(防止在执行进出仓的同时，按下计数键，挂起进仓线程)
	while(1 == g_OutIn_Module_Status)
	{
		//未执行完成
		osDelay(100);
		if(0 == g_OutIn_Module_Status || ++i > 70) //进出仓模块执行完成，或超过7s
		{
			break;
		}
	}
	
	//松开探针电机
	Clamp_Motor_In(MOTOR_MODE_NORMAL, MOTOR_CLAMP_MAX_STEP, MachInfo.paramSet.ulClamp_In_AddStep);
	
	//开液阀
    LIQUID_WAVE_OPEN;
	HAL_Delay(1500);
	LIQUID_WAVE_CLOSE;
    
    //进仓
    OutIn_Module_In(MOTOR_MODE_NORMAL);
    
    //关指示灯
    Panel_Led_Status(RUN_MODE_NORMAL, PANEL_LED_STATUS_OFF);
	g_ePanel_LED_Status = PANEL_LED_STATUS_OFF;  
	
	WM_SendMessageNoPara(g_hActiveWin,WM_USER_DATA);

}







#ifdef CX_DEBUG
//

#endif

