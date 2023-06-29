/**********************************************************************************
打印机相关任务进程，并且创建一个二值信号量，用来监听是否接受到的数据了；创建一个
计数信号量用于防止发送数据被覆盖，创建一个间隔2s查询一次打印机状态任务，创建一个打印机数据
打印任务；创建一个消息队列，用来传递打印的数据信息
***********************************************************************************/
#include "PrinterTask.h"
#include "Printer.h"
#include "cmsis_os2.h"
#include "file_operate.h"
#include "unicode2gbk.h"
#include "Common.h"



/*全局变量定义区*******************************************************************/
osSemaphoreId_t  SemphrCountPrinterTxHandle;
osSemaphoreId_t  SemphrBinPrinterRxHandle;
osMessageQueueId_t PrinterInfoQueueHandle;






/*外部函数区***********************************************************************/
/***
*查询打印机状态任务
***/
void PrinterIsOKTask(void *argument)
{
	while(1){
		osDelay(2000);
        Printer_IsOK();
	}
}






/***
*打印机打印任务进程
***/
void PrinterTask(void *argument)
{
	PrinterInfoData_s PrinterInfoData;
	
    extern osEventFlagsId_t GlobalEventPrintGroupHandle;  
    extern osMessageQueueId_t PrinterInfoQueueHandle;
	extern PrinterCurrentState_s PrinterCurrentState;
    
	
	//等待打印机通讯成功事件，并清除标志位
    osEventFlagsWait(GlobalEventPrintGroupHandle,GLOBAL_EVENT_PRINTER_NO_PAPER|GLOBAL_EVENT_PRINTER_RECOVER, osFlagsWaitAny, osWaitForever);
	
	//将打印机初始化各参数为默认值
	Printer_Init();
	
	while(1){
		//等待打印数据
		osMessageQueueGet(PrinterInfoQueueHandle,&PrinterInfoData,0 ,osWaitForever);
		
		//配置打印机各参数为默认值，防止偶发配置不成功时，导致打印异常
		Printer_ConfigDefault();
		
		//判断打印那个页面
		switch(PrinterInfoData.page){
			//打印样本分析数据
			case SAMPLE_RESULT_PAGE:							
				PrinterSamplePage(PrinterInfoData);
			break;
			
			//打印流程老化结果数据
			case AGING_TEST_RESULT_PAGE:						
				PrinterAgingTestResultPage(PrinterInfoData);
			break;
			
			//打印探针稳定性结果数据
			case TAN_ZHEN_TEST_RESULT_PAGE:						
				PrinterTanZhenTestResultPage(PrinterInfoData);
			break;
			
			//打印进出仓老化测试结果数据
			case MOTOR_OUT_IN_RESULT_PAGE:
				PrinterOutInMotoTestResultPage(PrinterInfoData);
			break;
			
			//打印气嘴老化测试结果数据
			case QIZUI_TEST_RESULT_PAGE:
				PrinterQizuiTestResultPage(PrinterInfoData);
			break;
			
			//打印气阀老化测试结果数据
			case AIR_TEST_RESULT_PAGE:
				PrinterAirValveTestResultPage(PrinterInfoData);
			break;
			
			//打印液阀老化测试结果数据
			case YEFA_TEST_RESULT_PAGE:
				PrinterYeFaValveTestResultPage(PrinterInfoData);
			break;
			
			//打印建压老化测试结果数据
			case BUILD_PRESSURE_TEST_RESULT_PAGE:
				PrinterBuildPressureTestResultPage(PrinterInfoData);
			break;
			
			//打印定量电极老化测试结果数据
			case DL_ELEC_TEST_RESULT_PAGE:
				PrinterDLElecTestResultPage(PrinterInfoData);
			break;
			
			//打印质控分析结果数据
			case QC_RESULT_PAGE:
				PrinterQCPage(PrinterInfoData);
			break;
			
			//打印生产流程老化结果数据
			case SC_AGING_TEST_RESULT_PAGE:
				PrinterSCAgingTestResultPage(PrinterInfoData);
			break;
			
			default :break;
		}
		
		//等待打印完毕
		while(PrinterCurrentState.remainRxBuf < 0x2400){
			osDelay(1000);
		}
		
		//将在打印期间发送的队列消息全部清除掉，避免多次点击打印按钮导致重复打印
		osMessageQueueReset(PrinterInfoQueueHandle);
	}
}




const char g_ucaLng_QC_ResultInCtrl[LANGUAGE_END][18]    = {"结果在控", "Result In Control"};
const char g_ucaLng_QC_ResultOutCtrl[LANGUAGE_END][20]   = {"结果失控", "Result Out Control"};

/*
*   提供外部分，打印结果信息接口
*/
void CreateAndSend_QC_WBCHGB_PrintData(__IO QC_WBCHGB_TestInfo_t *ptQC_WBCHGB_TestInfo)
{     
    /******************************************************************/    
	extern MachInfo_s MachInfo;
	extern osMessageQueueId_t PrinterInfoQueueHandle;
	PrinterInfoData_s* PrinterInfoData = (PrinterInfoData_s*)SDRAM_ApplyCommonBuf(sizeof(PrinterInfoData_s));
	uint8_t TempLen = 0;
    uint16_t i = 0;
	
    //WBC HGB结果打印
	PrinterInfoData->page = QC_RESULT_PAGE;
   
    //文件号 
    PrinterInfoData->pageUnion.qcPageInfo.fileNum = ptQC_WBCHGB_TestInfo->usFileNum;
    //文件号索引值
    PrinterInfoData->pageUnion.qcPageInfo.eFileNumIndex = ptQC_WBCHGB_TestInfo->eFileNumIndex;
    //水平
    PrinterInfoData->pageUnion.qcPageInfo.level = ptQC_WBCHGB_TestInfo->eQC_Level;
    //质控状态
    PrinterInfoData->pageUnion.qcPageInfo.eQC_Ctrl_Status = ptQC_WBCHGB_TestInfo->eQC_Ctrl_Status;
    if(QC_CTRL_STATUS_IN == PrinterInfoData->pageUnion.qcPageInfo.eQC_Ctrl_Status)
    {
        TempLen = sprintf(PrinterInfoData->pageUnion.qcPageInfo.tip, "%s", g_ucaLng_QC_ResultInCtrl[MachInfo.systemSet.eLanguage]);
    }else if(QC_CTRL_STATUS_OUT == PrinterInfoData->pageUnion.qcPageInfo.eQC_Ctrl_Status){
        TempLen = sprintf(PrinterInfoData->pageUnion.qcPageInfo.tip, "%s", g_ucaLng_QC_ResultOutCtrl[MachInfo.systemSet.eLanguage]);
    }
    PrinterInfoData->pageUnion.qcPageInfo.batchNum[TempLen] = '\0';
    
    //批号
    TempLen = sprintf(PrinterInfoData->pageUnion.qcPageInfo.batchNum, "%s", ptQC_WBCHGB_TestInfo->ucaLotNum);
	PrinterInfoData->pageUnion.qcPageInfo.batchNum[TempLen] = '\0';
    //有效期
    TempLen = sprintf(PrinterInfoData->pageUnion.qcPageInfo.indate, "%s", ptQC_WBCHGB_TestInfo->ucaValidDate);
	PrinterInfoData->pageUnion.qcPageInfo.indate[TempLen] = '\0';
    //样本编号
    TempLen = sprintf(PrinterInfoData->pageUnion.qcPageInfo.qcSN, "%s", ptQC_WBCHGB_TestInfo->ucaSampleSN);
	PrinterInfoData->pageUnion.qcPageInfo.qcSN[TempLen] = '\0';
    //检测时间
    TempLen = sprintf(PrinterInfoData->pageUnion.qcPageInfo.checkTime, "%s", ptQC_WBCHGB_TestInfo->ucaDateTime);
	PrinterInfoData->pageUnion.qcPageInfo.checkTime[TempLen] = '\0';

     //报警标志
	for(i=0;i<8;i++){
		PrinterInfoData->pageUnion.qcPageInfo.alarmFlag[i] = ptQC_WBCHGB_TestInfo->ucaWBCHGB_RstAlert[i];
	}

    //结果值
	PrinterInfoData->pageUnion.qcPageInfo.wbc  = ptQC_WBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
    PrinterInfoData->pageUnion.qcPageInfo.gran = ptQC_WBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ];
    PrinterInfoData->pageUnion.qcPageInfo.mid  = ptQC_WBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ];
    PrinterInfoData->pageUnion.qcPageInfo.lym  = ptQC_WBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ];
    PrinterInfoData->pageUnion.qcPageInfo.granPercent = ptQC_WBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB];
    PrinterInfoData->pageUnion.qcPageInfo.midPercent  = ptQC_WBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB];
    PrinterInfoData->pageUnion.qcPageInfo.lymPercent  = ptQC_WBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB];
    
    //HGB数据
    PrinterInfoData->pageUnion.qcPageInfo.hgb = ptQC_WBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB];
    
    //直方图
    for(i = 0; i < GRAPH_DATA_LEN; i++)
    {
        PrinterInfoData->pageUnion.qcPageInfo.histogramPos[i] = ptQC_WBCHGB_TestInfo->fWBC_Histogram[i]*100;
    }

	//三条分割线
	PrinterInfoData->pageUnion.qcPageInfo.crossLine[0] = ptQC_WBCHGB_TestInfo->fLines[0];
	PrinterInfoData->pageUnion.qcPageInfo.crossLine[1] = ptQC_WBCHGB_TestInfo->fLines[1];
	PrinterInfoData->pageUnion.qcPageInfo.crossLine[2] = ptQC_WBCHGB_TestInfo->fLines[2];
	
	//发送要打印的数据到消息队列中
    osMessageQueuePut(PrinterInfoQueueHandle,PrinterInfoData, 0, 0);
}





/*
*   提供外部分，打印结果信息接口
*/
void CreateAndSend_WBCHGB_PrintData(__IO WBCHGB_TestInfo_t *ptWBCHGB_TestInfo)
{
	extern osMessageQueueId_t PrinterInfoQueueHandle;
	PrinterInfoData_s* PrinterInfoData = (PrinterInfoData_s*)SDRAM_ApplyCommonBuf(sizeof(PrinterInfoData_s));
	uint8_t TempLen = 0;
    uint16_t i = 0;
	uint16_t usUnicode[30] = {0};
    uint16_t usGBK[30] = {0};

    //WBC HGB结果打印
	PrinterInfoData->page = SAMPLE_RESULT_PAGE;

    //错误码
    PrinterInfoData->pageUnion.samplePageInfo.eDataStatus = ptWBCHGB_TestInfo->eDataStatus;
    
    //姓名
    //UTF8-->Unicode-->GBK
    UTFToUnicode(usUnicode, (uint8_t*)ptWBCHGB_TestInfo->ucaName);
    UnicodeToGBK(usGBK, usUnicode);
    strncpy(PrinterInfoData->pageUnion.samplePageInfo.name, (char*)usGBK, strlen((char*)usGBK));
	PrinterInfoData->pageUnion.samplePageInfo.name[strlen((char*)usGBK)] = '\0';
	
    //年龄
	PrinterInfoData->pageUnion.samplePageInfo.ucAge = ptWBCHGB_TestInfo->ucAge;
	
	//年龄单位
	PrinterInfoData->pageUnion.samplePageInfo.eAgeUnit = ptWBCHGB_TestInfo->eAgeUnit;
	
    //性别
    PrinterInfoData->pageUnion.samplePageInfo.gender = (Gender_e)ptWBCHGB_TestInfo->eSex;
	
	//样本编号
	TempLen = sprintf(PrinterInfoData->pageUnion.samplePageInfo.sampleNumber, "%s", ptWBCHGB_TestInfo->ucaSampleSN);
	PrinterInfoData->pageUnion.samplePageInfo.sampleNumber[TempLen] = '\0';
	//检测时间
	TempLen = sprintf(PrinterInfoData->pageUnion.samplePageInfo.checkTime, "%s", ptWBCHGB_TestInfo->ucaDateTime);
	PrinterInfoData->pageUnion.samplePageInfo.checkTime[TempLen] = '\0';
	
    //参考组
	PrinterInfoData->pageUnion.samplePageInfo.refGroups = ptWBCHGB_TestInfo->eReferGroup;
	
    //报警标志
	for(i=0;i<8;i++){
		PrinterInfoData->pageUnion.samplePageInfo.alarmFlag[i] = ptWBCHGB_TestInfo->ucaWBCHGB_RstAlert[i];
	}
	
	//数据是否有效
	PrinterInfoData->pageUnion.samplePageInfo.eDataStatus = ptWBCHGB_TestInfo->eDataStatus;

    //结果值
	PrinterInfoData->pageUnion.samplePageInfo.wbc  = ptWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_WBC];
    PrinterInfoData->pageUnion.samplePageInfo.gran = ptWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranJ];
    PrinterInfoData->pageUnion.samplePageInfo.mid  = ptWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidJ];
    PrinterInfoData->pageUnion.samplePageInfo.lym  = ptWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymJ];
    PrinterInfoData->pageUnion.samplePageInfo.granPercent = ptWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_GranB];
    PrinterInfoData->pageUnion.samplePageInfo.midPercent  = ptWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_MidB];
    PrinterInfoData->pageUnion.samplePageInfo.lymPercent  = ptWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_LymB];
    
    //HGB数据
    PrinterInfoData->pageUnion.samplePageInfo.hgb = ptWBCHGB_TestInfo->fWBCHGB_RstData[WBCHGB_RST_HGB];
    
    //直方图
    for(i = 0; i < GRAPH_DATA_LEN; i++)
    {
        PrinterInfoData->pageUnion.samplePageInfo.histogramPos[i] = ptWBCHGB_TestInfo->fWBC_Histogram[i]*100;
    }

	//三条分割线
	PrinterInfoData->pageUnion.samplePageInfo.crossLine[0] = ptWBCHGB_TestInfo->fLines[0];
	PrinterInfoData->pageUnion.samplePageInfo.crossLine[1] = ptWBCHGB_TestInfo->fLines[1];
	PrinterInfoData->pageUnion.samplePageInfo.crossLine[2] = ptWBCHGB_TestInfo->fLines[2];
		
	
	//发送要打印的数据到消息队列中
    osMessageQueuePut(PrinterInfoQueueHandle,PrinterInfoData, 0, 0);
}







/*外部函数区*********************************************************/
/***
*临时测试函数任务
***/
void TestPrinter(void)
{
	extern osMessageQueueId_t PrinterInfoQueueHandle;
	PrinterInfoData_s* PrinterInfoData = (PrinterInfoData_s*)SDRAM_ApplyCommonBuf(sizeof(PrinterInfoData_s));
	uint8_t TempLen = 0;
	

//===测试打印机===
	PrinterInfoData->page = SAMPLE_RESULT_PAGE;

	TempLen = sprintf(PrinterInfoData->pageUnion.samplePageInfo.name,"%s","2020创怀医疗2021");
	PrinterInfoData->pageUnion.samplePageInfo.name[TempLen] = '\0';
	
	PrinterInfoData->pageUnion.samplePageInfo.ucAge = 20;
	PrinterInfoData->pageUnion.samplePageInfo.eAgeUnit = 1;
	PrinterInfoData->pageUnion.samplePageInfo.name[TempLen] = '\0';
	
	PrinterInfoData->pageUnion.samplePageInfo.gender = MALE;
	
	TempLen = sprintf(PrinterInfoData->pageUnion.samplePageInfo.sampleNumber,"%s","CBC-001");
	PrinterInfoData->pageUnion.samplePageInfo.sampleNumber[TempLen] = '\0';
	
	TempLen = sprintf(PrinterInfoData->pageUnion.samplePageInfo.checkTime,"%s","2020-12-07 10:00");
	PrinterInfoData->pageUnion.samplePageInfo.checkTime[TempLen] = '\0';
	
	//报警标志
	PrinterInfoData->pageUnion.samplePageInfo.alarmFlag[0] = WBCHGB_ALERT_NORMAL;
	PrinterInfoData->pageUnion.samplePageInfo.alarmFlag[1] = WBCHGB_ALERT_SMALL;
	PrinterInfoData->pageUnion.samplePageInfo.alarmFlag[2] = WBCHGB_ALERT_BIG;
	PrinterInfoData->pageUnion.samplePageInfo.alarmFlag[3] = WBCHGB_ALERT_REVIEW;
	PrinterInfoData->pageUnion.samplePageInfo.alarmFlag[4] = WBCHGB_ALERT_INVALID;
	PrinterInfoData->pageUnion.samplePageInfo.alarmFlag[5] = WBCHGB_ALERT_NORMAL;
	PrinterInfoData->pageUnion.samplePageInfo.alarmFlag[6] = WBCHGB_ALERT_NORMAL;
	PrinterInfoData->pageUnion.samplePageInfo.alarmFlag[7] = WBCHGB_ALERT_NORMAL;
	
	PrinterInfoData->pageUnion.samplePageInfo.wbc = 0xffff;
	PrinterInfoData->pageUnion.samplePageInfo.lymPercent = 50;
	
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[0]=38			;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[1]=92	        ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[2]=186	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[3]=351	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[4]=614	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[5]=999	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[6]=1518	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[7]=2144	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[8]=2793	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[9]=3399	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[10]=3882	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[11]=4178	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[12]=4259	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[13]=4140	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[14]=3876	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[15]=3539	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[16]=2908	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[17]=1708	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[18]=1276	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[19]=1113	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[20]=1097	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[21]=1177	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[22]=1434	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[23]=2102	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[24]=3159	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[25]=3722	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[26]=4370	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[27]=4989	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[28]=5632	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[29]=6307	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[30]=6948	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[31]=7575	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[32]=8168	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[33]=8704	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[34]=9165	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[35]=9536	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[36]=9805	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[37]=9962	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[38]=10000	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[39]=9915	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[40]=9710	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[41]=9390	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[42]=8967	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[43]=8459	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[44]=7886	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[45]=7272	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[46]=6643	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[47]=6024	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[48]=5422	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[49]=4834	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[50]=4316	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[51]=3858	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[52]=3453	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[53]=3091	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[54]=2761	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[55]=2429	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[56]=2143	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[57]=1887	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[58]=1667	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[59]=1491	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[60]=1367	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[61]=1278	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[62]=1224	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[63]=1200	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[64]=1201	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[65]=1223	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[66]=1264	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[67]=1320	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[68]=1388	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[69]=1462	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[70]=1509	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[71]=1569	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[72]=1616	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[73]=1647	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[74]=1661	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[75]=1618	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[76]=1608	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[77]=1590	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[78]=1567	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[79]=1538	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[80]=1471	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[81]=1435	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[82]=1395	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[83]=1354	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[84]=1317	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[85]=1289	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[86]=1276	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[87]=1274	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[88]=1281	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[89]=1295	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[90]=1314	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[91]=1328	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[92]=1341	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[93]=1352	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[94]=1366	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[95]=1380	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[96]=1412	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[97]=1461	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[98]=1531	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[99]=1624	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[100]=1729	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[101]=1857	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[102]=1988	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[103]=2112	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[104]=2220	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[105]=2295	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[106]=2362	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[107]=2419	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[108]=2477	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[109]=2548	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[110]=2638	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[111]=2768	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[112]=2937	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[113]=3148	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[114]=3402	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[115]=3685	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[116]=3935	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[117]=4196	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[118]=4458	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[119]=4707	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[120]=4931	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[121]=5119	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[122]=5267	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[123]=5374	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[124]=5447	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[125]=5496	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[126]=5534	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[127]=5574	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[128]=5626	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[129]=5697	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[130]=5786	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[131]=5889	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[132]=5994	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[133]=6089	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[134]=6160	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[135]=6196	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[136]=6192	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[137]=6146	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[138]=6067	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[139]=5966	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[140]=5858	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[141]=5758	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[142]=5674	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[143]=5612	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[144]=5567	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[145]=5532	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[146]=5498	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[147]=5457	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[148]=5404	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[149]=5341	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[150]=5270	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[151]=5198	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[152]=5129	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[153]=5067	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[154]=5011	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[155]=4958	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[156]=4901	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[157]=4836	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[158]=4759	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[159]=4666	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[160]=4560	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[161]=4443	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[162]=4319	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[163]=4193	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[164]=4067	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[165]=3944	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[166]=3823	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[167]=3704	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[168]=3585	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[169]=3467	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[170]=3348	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[171]=3228	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[172]=3093	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[173]=2957	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[174]=2831	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[175]=2711	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[176]=2597	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[177]=2485	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[178]=2373	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[179]=2256	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[180]=2131	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[181]=2007	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[182]=1880	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[183]=1752	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[184]=1624	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[185]=1500	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[186]=1382	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[187]=1271	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[188]=1169	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[189]=1077	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[190]=995	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[191]=923	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[192]=862	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[193]=809	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[194]=761	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[195]=716	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[196]=671	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[197]=625	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[198]=580	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[199]=535	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[200]=494	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[201]=456	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[202]=423	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[203]=393	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[204]=367	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[205]=342	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[206]=318	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[207]=294	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[208]=271	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[209]=248	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[210]=226	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[211]=206	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[212]=189	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[213]=174	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[214]=162	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[215]=151	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[216]=141	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[217]=131	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[218]=122	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[219]=113	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[220]=103	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[221]=95	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[222]=88	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[223]=81	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[224]=76	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[225]=71	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[226]=67	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[227]=63	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[228]=60	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[229]=58	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[230]=56	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[231]=55	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[232]=54	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[233]=54	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[234]=54	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[235]=54	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[236]=54	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[237]=53	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[238]=51	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[239]=48	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[240]=44	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[241]=39	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[242]=35	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[243]=32	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[244]=31	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[245]=31	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[246]=32	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[247]=33	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[248]=34	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[249]=33	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[250]=30	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[251]=27	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[252]=22	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[253]=18	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[254]=13	    ;
	PrinterInfoData->pageUnion.samplePageInfo.histogramPos[255]=10       ;
	
	PrinterInfoData->pageUnion.samplePageInfo.crossLine[0] = 26;
	PrinterInfoData->pageUnion.samplePageInfo.crossLine[1] = 64;
	PrinterInfoData->pageUnion.samplePageInfo.crossLine[2] = 83;
	
	
	//发送要打印的数据到消息队列中
    osMessageQueuePut(PrinterInfoQueueHandle,PrinterInfoData, 0, 0);
}











