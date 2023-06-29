

#include <stddef.h>
#include <string.h>
#include "WM.h"
#include "GUI.h"
#include "FRAMEWIN.h"
#include "BUTTON.h"
#include "LISTVIEW.h"
#include "EDIT.h"
#include "DROPDOWN.h"
#include "TEXT.h"
#include "main.h"

#include "cx_menu.h"
//#include "Analysis_MenuDLG.h"
#include "Public_menuDLG.h"

#define GUI_SIMULATION
#ifndef GUI_SIMULATION

#endif

 /*********************************************************************
*
*       Defines
*
**********************************************************************
*/
__IO WM_HWIN g_hActiveWin;  //当前活动窗口句柄



// MachInfo.systemSet.eLanguage
const char g_ucaLng_Operator[LANGUAGE_END][10] 	    = {"操作者", "User"};
const char g_ucaLng_Login[LANGUAGE_END][7] 		    = {"登录", "Login"};
const char g_ucaLng_Name[LANGUAGE_END][7] 		    = {"姓名", "Name"};
const char g_ucaLng_Sexual[LANGUAGE_END][7] 	    = {"性别", "Gender"};
const char g_ucaLng_Age[LANGUAGE_END][7] 		    = {"年龄", "Age"};
const char g_ucaLng_CurUser[LANGUAGE_END][13] 	    = {"当前用户", "Current User"};
const char g_ucaLng_NextSampleSN[LANGUAGE_END][16] 	= {"下一编号", "Next Sample ID"}; //NEXT NUM
const char g_ucaLng_SampleNum[LANGUAGE_END][13]     = {"样本编号", "Sample ID"};
const char g_ucaLng_BirthDay[LANGUAGE_END][13]      = {"出生年月", "Birthday"};
const char g_ucaLng_Login_ResetMachPara[LANGUAGE_END][30]      = {"确定重置仪器参数？", "Confirm Reset Machine PARA.?"};
const char g_ucaLng_Num[LANGUAGE_END][13]      = {"编号", "ID"};

const char g_ucaLng_Year[LANGUAGE_END][5]           = {"岁", "Year"};
const char g_ucaLng_Month[LANGUAGE_END][6]          = {"月", "Month"};
const char g_ucaLng_Week[LANGUAGE_END][5]           = {"周", "Week"};
const char g_ucaLng_Day[LANGUAGE_END][4]            = {"天", "Day"};
const char g_ucaLng_ReferGroup[LANGUAGE_END][13]    = {"参考组", "Refer Group"};
const char g_ucaLng_General[LANGUAGE_END][8]        = {"通用", "General"};
const char g_ucaLng_AdultMale[LANGUAGE_END][13]     = {"成男", "Male"};
const char g_ucaLng_AdultFemale[LANGUAGE_END][13]   = {"成女", "Female"};
const char g_ucaLng_Child[LANGUAGE_END][7]          = {"儿童", "Child"};
const char g_ucaLng_Neonatus[LANGUAGE_END][10]      = {"新生儿", "Infant"};//Neonatus
const char g_ucaLng_SexMale[LANGUAGE_END][5]   	    = { "男", "Male" };
const char g_ucaLng_SexFemale[LANGUAGE_END][8] 	    = { "女", "Female" };
const char g_ucaLng_Sure[LANGUAGE_END][8]   	    = { "确定", "Confirm" };
const char g_ucaLng_Cancel[LANGUAGE_END][8] 	    = { "取消", "Cancel" };
const char g_ucaLng_Yes[LANGUAGE_END][4]   	        = { "是", "YES" };
const char g_ucaLng_No[LANGUAGE_END][4] 	        = { "否", "NO" };
const char g_ucaLng_ReCount_Prompt[LANGUAGE_END][19]    = {"是否重新测试", "Analyse Again?"}; //IF NEED RE-TEST
const char g_ucaLng_Ensure_PowerOff[LANGUAGE_END][20]   = {"是否确定关机", "Shut Down?"}; //IF ENSUREE POWEROFF
const char g_ucaLng_TakeOutCell_Before_PowerOff[LANGUAGE_END][50]   = {"请先取出血细胞检测模块,再关机", "Please take out blood cell then shut down"}; //IF ENSUREE POWEROFF
const char g_ucaLng_Ensure_Logout[LANGUAGE_END][19]     = {"是否确定注销", "Logout?"}; //IF ENSUREE LOGOUT
const char g_ucaLng_Delete[LANGUAGE_END][8] 	        = { "删除", "Delete" };
const char g_ucaLng_Refresh[LANGUAGE_END][15] 	        = { "刷新", "Refresh" };

//
const char g_ucaLng_SampleAnalysis[LANGUAGE_END][13] 	= {"样本分析", "Analyse"};
const char g_ucaLng_ListReview[LANGUAGE_END][13] 	    = {"列表回顾", "Review"}; //LIST REVIEW

const char g_ucaLng_QC[LANGUAGE_END][7] 	            = {"质控", "QC"};
const char g_ucaLng_Menu[LANGUAGE_END][7] 	            = {"菜单", "Menu"};
const char g_ucaLng_Service[LANGUAGE_END][8] 	        = {"服务", "Service"};
const char g_ucaLng_Set[LANGUAGE_END][9] 	            = {"设置", "Setting"};
const char g_ucaLng_Logout[LANGUAGE_END][7] 	        = {"注销", "Logout"};
const char g_ucaLng_PowerOff[LANGUAGE_END][10] 	        = {"关机", "Shut Down"};
const char g_ucaLng_Test[LANGUAGE_END][10]               = {"测试", "Analyse"};//Test
const char g_ucaLng_OpenLED[LANGUAGE_END][9]            = {"开灯", "Open Led"};
const char g_ucaLng_CloseLED[LANGUAGE_END][10]          = {"关灯", "Close Led"};
const char g_ucaLng_BlackAutoAdjust[LANGUAGE_END][19]   = {"空白自动校准", "Black Auto Adjust"}; //Black Auto Adjust
const char g_ucaLng_GZAutoAdjust[LANGUAGE_END][19]      = {"工装自动校准", "Tool Auto Adjust"};
const char g_ucaLng_SET_GZ_ADC[LANGUAGE_END][16]        = {"工装ADC", "Tool ADC"};
const char g_ucaLng_LoadDate_Err[LANGUAGE_END][19]      = {"加载数据失败", "Load Data Failed"};

/***********样本分析*********************************************************/
const char g_ucaLng_LastRecord[LANGUAGE_END][13]        = {"上一记录", "Previous"}; //Last Record
const char g_ucaLng_NextRecord[LANGUAGE_END][13]        = {"下一记录", "Next"};
const char g_ucaLng_NextSample[LANGUAGE_END][20]        = {"下一样本分析", "Start Analyse"}; // Analyse
const char g_ucaLng_ReMark[LANGUAGE_END][7] 	        = {"备注", "Remark"};
const char g_ucaLng_NextSampleDownloadInfo[LANGUAGE_END][35]        = {"下一样本分析(编号:", "Start Analyse(Num:"};

const char g_ucaLng_CountingPrompt[LANGUAGE_END][34]    = {"正在分析,请稍等...", "Analysing…"};
const char g_ucaLng_PatientInfo[LANGUAGE_END][13]       = {"病人信息", "Information"}; //Patient Info
const char g_ucaLng_Print[LANGUAGE_END][7] 		        = {"打印", "Print"};
const char g_ucaLng_CheckTime[LANGUAGE_END][13]         = {"检验时间", "Test Time"}; //Check Time
const char g_ucaLng_Param[LANGUAGE_END][13] 	        = {"参数", "PARA."}; //PARAMETER
const char g_ucaLng_PatientInfoDownloadInfo[LANGUAGE_END][30]       = {"病人信息(编号:", "Information(Num:"};

const char g_ucaLng_Result[LANGUAGE_END][7] 	        = {"结果", "Result"};
const char g_ucaLng_Unit[LANGUAGE_END][7] 		        = {"单位", "Unit"};
const char g_ucaLng_WBC[LANGUAGE_END][10] 		        = {"白细胞", "WBC"};
const char g_ucaLng_Gan[LANGUAGE_END][20] 		        = {"中性粒细胞", "Gran#"};
const char g_ucaLng_Mid[LANGUAGE_END][13] 		        = {"中间细胞", "Mid#"};
const char g_ucaLng_Lym[LANGUAGE_END][13] 		        = {"淋巴细胞", "Lym#"};
const char g_ucaLng_HGB[LANGUAGE_END][13] 		        = {"血红蛋白", "HGB"};
//
const char g_ucaLng_ErrorPrompt[LANGUAGE_END][18]       = {"异常提示", "Abnormal Message"}; //Error Prompt
const char g_ucaLng_WBCErrorPrompt[LANGUAGE_END][22]    = {"白细胞异常提示", "WBC Message"};
const char g_ucaLng_HGBErrorPrompt[LANGUAGE_END][25]    = {"血红蛋白异常提示", "HGB Message"};
const char g_ucaLng_LoginErrorPrompt[LANGUAGE_END][28]  = {"账号或密码错误!", "User Or Password Error!"}; //AcCOUNT OR PASSWORD ERROR!
const char g_ucaLng_OutIn_Error[LANGUAGE_END][30]       = {"进出仓异常!", "Counting Bin Movement Abnormal"};
const char g_ucaLng_Analyse_InputUserNum[LANGUAGE_END][20]  = {"患者编号", "Patient Num"};
const char g_ucaLng_Analyse_PatientList[LANGUAGE_END][20]  = {"患者列表", "Patient List"};
const char g_ucaLng_Analyse_InputUserNumNetERR[LANGUAGE_END][45]  = {"网络异常，是否手动录入信息", "Net Error,Confirm Manual Input Info"};
const char g_ucaLng_Analyse_NetERR[LANGUAGE_END][40]  = {"网络超时，请稍后重试", "Net Timeout,Please Try Again Later"};
const char g_ucaLng_Analyse_RequestParientInfo[LANGUAGE_END][30]  = {"请求患者信息中", "Request patient information"};
const char g_ucaLng_Analyse_NoParientInfo[LANGUAGE_END][30]  = {"无患者信息", "No patient information"};
const char g_ucaLng_Analyse_NOJSC[LANGUAGE_END][45]  = {"请放入血细胞检测模块", "Please Put In The blood cell"};

/***********列表回顾*********************************************************/
const char g_ucaLng_ResultInfo[LANGUAGE_END][13] 	    = {"结果详情", "Result INFO."};
const char g_ucaLng_ReFreshNet[LANGUAGE_END][20] 	    = {"刷新网络", "Refresh Net"};
const char g_ucaLng_Edit[LANGUAGE_END][7] 			    = {"编辑", "Edit"};
const char g_ucaLng_Return[LANGUAGE_END][7] 		    = {"返回", "Return"};
const char g_ucaLng_IfDeleteData[LANGUAGE_END][16] 		= {"删除数据?", "Delete Data?"};
const char g_ucaLng_WaitForRefresh[LANGUAGE_END][30] 	= {"刷新网络中...", "Refreshing the Network"};
const char g_ucaLng_RefreshNetOK[LANGUAGE_END][30] 		= {"刷新网络成功", "Refreshing the Network OK"};
const char g_ucaLng_RefreshNetErr[LANGUAGE_END][35] 	= {"刷新网络超时", "Refreshing the Network Timeout"};

 
/***********服务*********************************************************/
const char g_ucaLng_VersionInfo[LANGUAGE_END][22]        = {"版本信息", "Version Information"};
const char g_ucaLng_VersionFull[LANGUAGE_END][19]        = {"软件完整版本", "Full"};
const char g_ucaLng_VersionRelease[LANGUAGE_END][19]     = {"软件发布版本", "Release"};
const char g_ucaLng_VersionMcu[LANGUAGE_END][13]         = {"MCU版本", "MCU"};
const char g_ucaLng_VersionAlgo[LANGUAGE_END][13]        = {"算法版本", "Algorithm"};
const char g_ucaLng_VersionHardWare[LANGUAGE_END][18]    = {"硬件版本", "Hardware"};
const char g_ucaLng_Serve_Serve[LANGUAGE_END][10]        = {"服务", "Service"};
const char g_ucaLng_Serve_Selfcheck[LANGUAGE_END][10]    = {"自检", "Self-Check"};

const char g_ucaLng_Serve_Log[LANGUAGE_END][10]          = {"日志", "LOG"};
const char g_ucaLng_Serve_Info[LANGUAGE_END][15]         = {"信息", "INFO."};
const char g_ucaLng_Serve_BuildCheck[LANGUAGE_END][15]   = {"负压自检", "Pressure"};
const char g_ucaLng_Serve_BuildCheckS[LANGUAGE_END][30]  = {"负压自检成功", "Pressure Check OK"};
const char g_ucaLng_Serve_BuildCheckF[LANGUAGE_END][30]  = {"负压自检失败", "Pressure Check Fail"};
const char g_ucaLng_Serve_Check[LANGUAGE_END][20]        = {"气密性自检", "Leakage"}; //Air-Tight
const char g_ucaLng_Serve_AIRTIGCheckS[LANGUAGE_END][30] = {"气密性自检成功", "Gas Leakage Check OK"};
const char g_ucaLng_Serve_AIRTIGCheckF[LANGUAGE_END][30] = {"气密性自检失败", "Gas Leakage Check Fail"};
const char g_ucaLng_Serve_MotoCheck[LANGUAGE_END][30]    = {"进出仓自检", "Movement"};
const char g_ucaLng_Serve_MotoCheckS[LANGUAGE_END][30]   = {"进出仓自检成功", "Movement Check OK"};
const char g_ucaLng_Serve_MotoCheckF[LANGUAGE_END][30]   = {"进出仓自检失败", "Movement Check Fail"};
const char g_ucaLng_Serve_StatusCheck[LANGUAGE_END][15]  = {"运行状态", "Status"};
const char g_ucaLng_Serve_InteractionCheck[LANGUAGE_END][15] = {"交互自检", "Interaction"};
const char g_ucaLng_Serve_OtherCheck[LANGUAGE_END][15]   = {"其他自检", "Other"};
const char g_ucaLng_Serve_AgingCheck[LANGUAGE_END][15]   = {"组件老化", "Aging"};
const char g_ucaLng_Serve_TestLog[LANGUAGE_END][15]      = {"测试日志", "Analyse"};
const char g_ucaLng_Serve_ErrLog[LANGUAGE_END][15]       = {"故障日志", "Error"};
const char g_ucaLng_Serve_RunLog[LANGUAGE_END][15]       = {"运行日志", "Operate"};
const char g_ucaLng_Serve_Version[LANGUAGE_END][15]      = {"版本信息", "Version"};
const char g_ucaLng_Serve_Disk[LANGUAGE_END][15]         = {"存储信息", "Disk INFO."};
const char g_ucaLng_Serve_DataManage[LANGUAGE_END][15]   = {"数据管理", "Data"};
const char g_ucaLng_Serve_Building[LANGUAGE_END][22]     = {"负压自检中...", "Pressure Checking..."};
const char g_ucaLng_Serve_AirTighting[LANGUAGE_END][25]  = {"气密性自检中...", "Gas Leakage Checking..."};
const char g_ucaLng_Serve_MotoChecking[LANGUAGE_END][30] = {"进出仓自检中...", "Movement Checking..."};

const char g_ucaLng_Serve_Other[LANGUAGE_END][20]       = {"> 其他自检", "> Other"};
const char g_ucaLng_Serve_XK[LANGUAGE_END][30]          = {"小孔电压自检", "Aperture Voltage Check"};
const char g_ucaLng_Serve_Pump[LANGUAGE_END][30]        = {"泵自检", "Pump Check"};
const char g_ucaLng_Serve_Beep[LANGUAGE_END][30]        = {"蜂鸣器自检", "Beep Check"};
const char g_ucaLng_Serve_AirValve[LANGUAGE_END][30]    = {"阀1自检", "Valve1 Check"};
const char g_ucaLng_Serve_AirValveS[LANGUAGE_END][40]   = {"请确认阀是否发出声响", "Did Valve1 Make A Sound?"};
const char g_ucaLng_Serve_LiquidValve[LANGUAGE_END][30] = {"阀2自检", "Valve2 Check"};
const char g_ucaLng_Serve_LiquidValveS[LANGUAGE_END][40] = {"请确认阀是否发出声响", "Did Valve2 Make A Sound?"};
const char g_ucaLng_Serve_HGB[LANGUAGE_END][30]         = {"HGB自检", "HGB Check"};
const char g_ucaLng_Serve_XKSuccess[LANGUAGE_END][30]   = {"小孔电压自检成功", "Aperture Voltage Check OK"};
const char g_ucaLng_Serve_XKFail[LANGUAGE_END][30]      = {"小孔电压自检失败", "Aperture Voltage Check Fail"};
const char g_ucaLng_Serve_AirValveing[LANGUAGE_END][30] = {"阀1自检中...", "Valve1 Checking…"};
const char g_ucaLng_Serve_LiquidValveing[LANGUAGE_END][30] = {"阀2自检中...", "Valve2 Checking…"};
const char g_ucaLng_Serve_PumpChecking[LANGUAGE_END][30]   = {"泵自检中...", "Pump Checking…"};
const char g_ucaLng_Serve_PumpCheckS[LANGUAGE_END][40]     = {"请确认泵是否发出声响", "Did Pump Make A Sound?"};
const char g_ucaLng_Serve_BeepChecking[LANGUAGE_END][30]   = {"蜂鸣器自检中...", "Beep Checking…"};
const char g_ucaLng_Serve_BeepCheckS[LANGUAGE_END][40]     = {"请确认蜂鸣器是否发出声响", "Did Beep Make A Sound?"};
const char g_ucaLng_Serve_HGBChecking[LANGUAGE_END][30]    = {"HGB自检中...", "HGB Checking..."};
const char g_ucaLng_Serve_HGBCheckingOK[LANGUAGE_END][30]    = {"HGB自检成功", "HGB Is OK"};
const char g_ucaLng_Serve_HGBCheckingERR[LANGUAGE_END][30]    = {"HGB自检失败", "HGB Is ERR"};



const char g_ucaLng_Serve_XuHao[LANGUAGE_END][20]          = {"序号", "Index"};
const char g_ucaLng_Serve_Time[LANGUAGE_END][20]           = {"时间", "Time"};
const char g_ucaLng_Serve_OutlineInfo[LANGUAGE_END][20]    = {"概要信息", "Outline"};
const char g_ucaLng_Serve_Status[LANGUAGE_END][20]         = {"状态", "Status"};
const char g_ucaLng_Serve_Operator[LANGUAGE_END][20]       = {"操作者", "User"};
const char g_ucaLng_Serve_HaocaiErr[LANGUAGE_END][20]      = {"耗材故障", "Cassette"};
const char g_ucaLng_Serve_OperatorErr[LANGUAGE_END][20]    = {"操作故障", "Operate"};
const char g_ucaLng_Serve_MachErr[LANGUAGE_END][20]        = {"仪器故障", "Device"};
const char g_ucaLng_Serve_ZHErr[LANGUAGE_END][20]          = {"综合故障", "Complex"};
const char g_ucaLng_Serve_Testlog[LANGUAGE_END][20]        = {"> 测试日志", "> Analyse LOG"};
const char g_ucaLng_Serve_TestUser[LANGUAGE_END][20]       = {"测试用户", "User"};

const char g_ucaLng_Serve_EngineerUser[LANGUAGE_END][20]   = {"工程师用户", "Engineer User"};
const char g_ucaLng_Serve_Errlog[LANGUAGE_END][20]         = {"> 故障日志", "> Error LOG"};
const char g_ucaLng_Serve_Runlog[LANGUAGE_END][20]         = {"> 运行日志", "> Operate LOG"};
const char g_ucaLng_Serve_Conserr[LANGUAGE_END][20]        = {"> 耗材故障", "> Cassette"};
const char g_ucaLng_Serve_Operatorerr[LANGUAGE_END][20]    = {"> 操作故障", "> Operate"};
const char g_ucaLng_Serve_Macherr[LANGUAGE_END][20]        = {"> 仪器故障", "> Device"};
const char g_ucaLng_Serve_ZHerr[LANGUAGE_END][20]          = {"> 综合故障", "> Complex"};
const char g_ucaLng_Serve_VersionInfo[LANGUAGE_END][20]    = {"> 版本信息", "> Version INFO."};
const char g_ucaLng_Serve_FullVer[LANGUAGE_END][20]        = {"软件完整版本", "Full"};
const char g_ucaLng_Serve_AlgoVer[LANGUAGE_END][20]        = {"算法版本", "Algorithm"};
const char g_ucaLng_Serve_MainVer[LANGUAGE_END][20]        = {"主程序版本", "Main"};
const char g_ucaLng_Serve_BootloadVer[LANGUAGE_END][20]    = {"引导程序版本", "Boot"};
const char g_ucaLng_Serve_HardVer[LANGUAGE_END][20]        = {"硬件版本", "Hardware"};
const char g_ucaLng_Serve_UIVer[LANGUAGE_END][20]          = {"UI版本", "UI"};
const char g_ucaLng_Serve_PubVer[LANGUAGE_END][20]          = {"发布版本", "Release"};

const char g_ucaLng_Serve_DiskInfo[LANGUAGE_END][20]       = {"> 存储信息", "> Disk INF0."};
const char g_ucaLng_Serve_UsedPlace[LANGUAGE_END][20]      = {"已用空间", "Used Space"};
const char g_ucaLng_Serve_CanPlace[LANGUAGE_END][20]       = {"可用空间", "Free Space"};
const char g_ucaLng_Serve_TotalPlace[LANGUAGE_END][20]       = {"总容量", "Total Space"};

const char g_ucaLng_Serve_DATAMENT[LANGUAGE_END][30]     = {"> 数据管理", "> Data"};
const char g_ucaLng_Serve_DEL_USERDATA[LANGUAGE_END][30]     = {"删除用户数据", "Del User Data"};
const char g_ucaLng_Serve_CONFIM_DELUSERDATA[LANGUAGE_END][60]    = {"确认删除用户数据，完毕后将自动重启", "Confim Del Data\r\nMachine Will Auto-Restart"};
const char g_ucaLng_Serve_DEL_USERDATA_SUCCESS[LANGUAGE_END][30]     = {"删除用户数据成功", "Del User Data Success"};
const char g_ucaLng_Serve_DEL_USERDATA_FAIL[LANGUAGE_END][30]     = {"删除用户数据失败", "Del User Data Fail"};


/***********设置*********************************************************/
const char g_ucaLng_ManualCalibration[LANGUAGE_END][19] = {"> 校准", "> Calibration"};
const char g_ucaLng_CalibrationFactor[LANGUAGE_END][22] = {"校准系数(%)", "Calibration Factor"};
const char g_ucaLng_CalibrationDate[LANGUAGE_END][22]   = {"校准日期", "Calibration Date"};
const char g_ucaLng_SET_XiTong[LANGUAGE_END][20]        = {"系统设置", "System"};
const char g_ucaLng_SET_YongHu[LANGUAGE_END][20]        = {"用户管理", "User"};
const char g_ucaLng_SET_FuZhu[LANGUAGE_END][20]         = {"辅助设置", "Auxiliary"};
const char g_ucaLng_SET_DaYing[LANGUAGE_END][20]        = {"打印设置", "Printer"};
const char g_ucaLng_SET_ShiYanShi[LANGUAGE_END][20]     = {"实验室信息", "Lab INFO."};
const char g_ucaLng_SET_JiaoZhun[LANGUAGE_END][20]      = {"校准", "Calibration"};
const char g_ucaLng_SET_Canshu[LANGUAGE_END][20]        = {"参数设置", "PARA."};
const char g_ucaLng_SET_TiaoShi[LANGUAGE_END][20]       = {"调试功能", "Debug"};

const char g_ucaLng_SET_Sys[LANGUAGE_END][15]           = {"> 系统设置", "> System"};
const char g_ucaLng_SET_Date[LANGUAGE_END][10]          = {"日期", "Date"};
const char g_ucaLng_SET_Time[LANGUAGE_END][10]          = {"时间", "Time"};
const char g_ucaLng_SET_Language[LANGUAGE_END][10]      = {"语言", "Language"};
const char g_ucaLng_SET_Chinese[LANGUAGE_END][10]       = {"中文", "中文"};
const char g_ucaLng_SET_English[LANGUAGE_END][10]       = {"英文", "English"};
const char g_ucaLng_SET_DataNum[LANGUAGE_END][25]      = {"数据容量", "Data Capacity"};
const char g_ucaLng_SET_ConfirmDataNum[LANGUAGE_END][80] = {"确认更改数据容量？\r\n切换后将删除所有测试数据", "Confirm Change Data Capacity?\r\nWill Del All Data"};

const char g_ucaLng_SET_User[LANGUAGE_END][22]          = {"> 用户管理", "> User Management"};
const char g_ucaLng_SET_NewUser[LANGUAGE_END][15]     	= {"新增用户", "Add User"};
const char g_ucaLng_SET_DelUser[LANGUAGE_END][15]     	= {"删除用户", "Del User"};
const char g_ucaLng_SET_SetDefault[LANGUAGE_END][25]     = {"设置默认用户", "Set Default"};
const char g_ucaLng_SET_ChangePsw[LANGUAGE_END][15]     = {"修改密码", "Change PSW"};
const char g_ucaLng_SET_FactoryReset[LANGUAGE_END][20]  = {"重置密码", "Reset PSW"};
const char g_ucaLng_SET_UserName[LANGUAGE_END][20]      = {"用户名", "User"};
const char g_ucaLng_SET_Limit[LANGUAGE_END][10]         = {"权限", "Limit"};
const char g_ucaLng_SET_Type[LANGUAGE_END][20]        = {"账户类型", "Type"};
const char g_ucaLng_SET_Default[LANGUAGE_END][20]        = {"默认用户", "Default User"};
const char g_ucaLng_SET_DianjiXuanxiang[LANGUAGE_END][20] = {"请点击选项", "Please Select Item"};
const char g_ucaLng_SET_NoLimit[LANGUAGE_END][20]         = {"无权限", "NO Permission"};
const char g_ucaLng_SET_ConfimPsw[LANGUAGE_END][35]       = {"确认恢复出厂密码", "Confirm To Recovery Password"};
const char g_ucaLng_SET_SuperUser[LANGUAGE_END][20]       = {"超级管理员", "Super User"};
const char g_ucaLng_SET_AdminUser[LANGUAGE_END][20]       = {"管理员", "Admin User"};
const char g_ucaLng_SET_NormalUser[LANGUAGE_END][20]      = {"普通用户", "Normal User"};
const char g_ucaLng_SET_NewUserN[LANGUAGE_END][20]        = {"新增用户", "New User"};
const char g_ucaLng_SET_UserFull[LANGUAGE_END][30]        = {"用户数量已满", "The number of users is full"};
const char g_ucaLng_SET_ConfimDelUser[LANGUAGE_END][30]   = {"确认删除此用户？", "Confirm To Delete The User?"};

const char g_ucaLng_SET_ASSIST[LANGUAGE_END][22]        	= {"> 辅助设置", "> Auxiliary"};
const char g_ucaLng_SET_CompanySwitch[LANGUAGE_END][20]     = {"公司切换", "Company Switch"};
const char g_ucaLng_SET_UUpdate[LANGUAGE_END][15]          = {"U盘升级", "U Update"};
const char g_ucaLng_SET_SetSkin[LANGUAGE_END][15]          = {"设置皮肤", "Set Skin"};
const char g_ucaLng_SET_RES3[LANGUAGE_END][15]          = {"预留3", "RES3"};
const char g_ucaLng_SET_UpdateMain[LANGUAGE_END][20]    = {"升级主程序", "Upgrade APP"};
const char g_ucaLng_SET_UpdateBoot[LANGUAGE_END][20]    = {"升级引导程序", "Upgrade BOOT"};
const char g_ucaLng_SET_UpdateUI[LANGUAGE_END][20]      = {"升级UI", "Upgrade UI"};
const char g_ucaLng_SET_Configing[LANGUAGE_END][45]     = {"系统配置中，请稍后...", "System Is Being Upgraged, Please Wait…"};
const char g_ucaLng_SET_ConfirmUUpdate[LANGUAGE_END][80]     = {"确认U盘升级吗？\r\n确认请插上U盘", "Sure The USB Flash Drive is upgraded?\r\nPlease Plug In The USB Flash Drive"};
const char g_ucaLng_SET_UpanTimeout[LANGUAGE_END][40]      = {"U盘插入超时，重启中...", "USB Flash Insert Timeout,Restart..."};
const char g_ucaLng_SET_UpanFoumtFail[LANGUAGE_END][35]      = {"U盘挂载失败，重启中...", "Foumt Failed,Restart..."};
const char g_ucaLng_SET_UUpdateSuccess[LANGUAGE_END][35]      = {"U盘升级成功，重启中...", "Upgrade Success,Restart..."};
const char g_ucaLng_SET_BootUpanFileErr[LANGUAGE_END][35]      = {"BOOT文件校验失败", "Boot File Err"};
const char g_ucaLng_SET_BootUpanFileWriteFail[LANGUAGE_END][35]      = {"BOOT文件写入失败", "Boot File Write Fail"};
const char g_ucaLng_SET_BootUpanOtherErr[LANGUAGE_END][35]      = {"BOOT升级其他异常", "Boot Upgraged Err"};
const char g_ucaLng_SET_MainUpanFileErr[LANGUAGE_END][35]      = {"MAIN文件校验失败", "Main File Err"};
const char g_ucaLng_SET_MainUpanFileWriteFail[LANGUAGE_END][35]      = {"MAIN文件写入失败", "Main File Write Fail"};
const char g_ucaLng_SET_MainUpanOtherErr[LANGUAGE_END][35]      = {"MAIN升级其他异常", "Main Upgraged Err"};
const char g_ucaLng_SET_UIUpanFileErr[LANGUAGE_END][35]      = {"UI文件校验失败", "UI File Err"};
const char g_ucaLng_SET_UIUpanFileWriteFail[LANGUAGE_END][35]      = {"UI文件写入失败", "UI File Write Fail"};
const char g_ucaLng_SET_UIUpanOtherErr[LANGUAGE_END][35]      = {"UI升级其他异常", "UI Upgraged Err"};

const char g_ucaLng_SET_Printer[LANGUAGE_END][20]         = {"> 打印设置", "> Printer"};
const char g_ucaLng_SET_Title[LANGUAGE_END][15]           = {"标题", "Report Title"};
const char g_ucaLng_SET_PrinterMode[LANGUAGE_END][15]     = {"打印方式", "Print Mode"};
const char g_ucaLng_SET_TitleInfo[LANGUAGE_END][30]       = {"血细胞检验报告单", "Blood Analyse Report"};
const char g_ucaLng_SET_AutoPrinter[LANGUAGE_END][30]     = {"分析成功后自动打印", "Auto Printing After Analysis"};
const char g_ucaLng_SET_ManualPrinter[LANGUAGE_END][32]   = {"分析成功后手动打印", "Manual Printing After Analysis"};

const char g_ucaLng_SET_LabInfo[LANGUAGE_END][20]         = {"> 实验室信息", "> Lab INFO."};
const char g_ucaLng_SET_MachName[LANGUAGE_END][20]        = {"仪器名称", "Product Name"};
const char g_ucaLng_SET_MachType[LANGUAGE_END][20]        = {"仪器类型", "Product Type"};
const char g_ucaLng_SET_MachSN[LANGUAGE_END][20]          = {"仪器序列号", "Product SN."};
const char g_ucaLng_SET_ZhensuoName[LANGUAGE_END][20]     = {"诊所名称", "Clinic"};
const char g_ucaLng_SET_MachNameInfo[LANGUAGE_END][35]    = {"半自动血细胞分析仪", "3-Diff Hematology Analyzer"};
const char g_ucaLng_SET_MachNameInfoYL1[LANGUAGE_END][35] = {"半自动血细胞分析仪", "Hematology Analyzer"};
const char g_ucaLng_SET_WaitForConfim[LANGUAGE_END][35]   = {"请等待服务端确认...", "Please Wait For Host Confirmation"};
const char g_ucaLng_SET_ConfimChangeSN[LANGUAGE_END][35]  = {"确认修改仪器序列号?", "Confirm To Change Product Sn?"};
const char g_ucaLng_SET_SNERR[LANGUAGE_END][35]  		  = {"仪器序列号输入错误", "Product Sn Format Error"};

const char g_ucaLng_SET_ParaSet[LANGUAGE_END][20]           = {"> 参数设置","> Parameter"};
const char g_ucaLng_SET_WBC_Gain[LANGUAGE_END][20]          = {"WBC增益", "WBC Gain"};
const char g_ucaLng_SET_HGB_Gain[LANGUAGE_END][20]          = {"HGB增益", "HGB Gain"};
const char g_ucaLng_SET_Pump[LANGUAGE_END][20]              = {"泵占空比", "Pump Pwm"};
const char g_ucaLng_SET_535Current[LANGUAGE_END][20]        = {"535灯电流", "535-Current"};
const char g_ucaLng_SET_415Current[LANGUAGE_END][20]        = {"415灯电流", "415-Current"};
const char g_ucaLng_SET_Pressure[LANGUAGE_END][20]          = {"压力补偿", "Pressure"};
const char g_ucaLng_SET_OCIN[LANGUAGE_END][20]              = {"进仓光耦", "OC_In"};
const char g_ucaLng_SET_OCOUT[LANGUAGE_END][20]             = {"出仓光耦", "OC_Out"};
const char g_ucaLng_SET_OCDAOTONG[LANGUAGE_END][20]         = {"导通光耦", "OC_On"};
const char g_ucaLng_SET_OCDUANKAI[LANGUAGE_END][20]         = {"断开光耦", "OC_Off"};
const char g_ucaLng_SET_PRESSURESensor[LANGUAGE_END][20]    = {"压力传感器", "Press Sensor"};
const char g_ucaLng_SET_AUTOAdjust[LANGUAGE_END][20]        = {"自动调整", "Auto"};
const char g_ucaLng_SET_HGB_Debug[LANGUAGE_END][15]         = {"HGB调校", "HGB Debug"};
const char g_ucaLng_SET_HGB_AjustParaSet[LANGUAGE_END][19]  = {"校准参数复位", "Reset"};
const char g_ucaLng_SET_PumpAutoSuccess[LANGUAGE_END][35]   = {"泵占空比自动调节成功", "Pump Pwm Adjust Success!"};
const char g_ucaLng_SET_PumpAutoFail[LANGUAGE_END][35]      = {"泵占空比自动调节失败", "Pump Pwm Adjust Fail!"};
const char g_ucaLng_SET_BuildPressing[LANGUAGE_END][35]     = {"建压到-30kpa中...", "Setting Pressure To -30kpa…"};
const char g_ucaLng_SET_BuildPressSuccess[LANGUAGE_END][45] = {"建压到-30kpa成功", "Setting Pressure To -30kpa Success!"};
const char g_ucaLng_SET_BuildPressFail[LANGUAGE_END][45]    = {"建压到-30kpa失败", "Setting Pressure To -30kpa Fail!"};


const char g_ucaLng_SET_Debug[LANGUAGE_END][20]             = {"> 调试功能", "> Debug"};
const char g_ucaLng_SET_CountDelay[LANGUAGE_END][20]        = {"计数延迟时间", "Analysis Delay"};
const char g_ucaLng_SET_CountReuse[LANGUAGE_END][30]        = {"计数池重复使用", "Cassette Reuse"};
const char g_ucaLng_SET_DebugWBC[LANGUAGE_END][30]          = {"工装计数池测试WBC", "Debug WBC System By Tool"};
const char g_ucaLng_SET_Count[LANGUAGE_END][20]             = {"执行次数", "Number"};

const char g_ucaLng_SET_LogoSwitch[LANGUAGE_END][20]        = {"公司切换", "Company Switch"};
const char g_ucaLng_SET_CH[LANGUAGE_END][20]   			    = {"创怀医疗", "Createcare"};
const char g_ucaLng_SET_Reserve1[LANGUAGE_END][20]          = {"锦瑞", "JINRUI"};
const char g_ucaLng_SET_Reserve2[LANGUAGE_END][20]          = {"药师帮", "YSB"};
const char g_ucaLng_SET_Reserve3[LANGUAGE_END][20]          = {"和映", "HY"};
const char g_ucaLng_SET_Reserve4[LANGUAGE_END][20]          = {"预留4", "RES4"};
const char g_ucaLng_SET_Reserve5[LANGUAGE_END][20]          = {"预留5", "RES5"};
const char g_ucaLng_SET_Reserve6[LANGUAGE_END][20]           = {"预留6", "RES6"};
const char g_ucaLng_SET_Reserve7[LANGUAGE_END][20]          = {"预留7", "RES7"};

const char g_ucaLng_SET_ChangePW[LANGUAGE_END][20]          = {"修改密码", "Change Password"};
const char g_ucaLng_SET_OldPW[LANGUAGE_END][20]             = {"旧密码", "Old Password"};
const char g_ucaLng_SET_NewPW[LANGUAGE_END][20]             = {"新密码", "New Password"};
const char g_ucaLng_SET_ConfimPW[LANGUAGE_END][20]          = {"确认新密码", "Confim Password"};
const char g_ucaLng_SET_OldPWErr[LANGUAGE_END][50]          = {"【旧密码错误，请重新输入】", "【Old Password Error】"};
const char g_ucaLng_SET_PWNotMatch[LANGUAGE_END][60]        = {"【两次输入的密码不匹配，请重新输入】", "【The Two Passwords Are Different】"};
const char g_ucaLng_SET_EnterNewPW[LANGUAGE_END][40]        = {"【请输入新密码】", "【Please Enter A New Password】"};
const char g_ucaLng_SET_ChangePWLogo1[LANGUAGE_END][40]     = {"修改了001账户密码", "Password Of User 001 Was Changed"};
const char g_ucaLng_SET_ChangePWLogo2[LANGUAGE_END][40]     = {"修改了admin账户密码", "Password Of User Admin Was Changed"};
const char g_ucaLng_SET_ChangePWLogo3[LANGUAGE_END][40]     = {"修改了测试账户密码", "Password Of User Test Was Changed"};
const char g_ucaLng_SET_ChangePWLogo4[LANGUAGE_END][40]     = {"修改了工程师账户密码", "Password Of User Yongfu Was Changed"};
const char g_ucaLng_SET_NewUserName[LANGUAGE_END][20]       = {"用户名", "User"};
const char g_ucaLng_SET_AddUser[LANGUAGE_END][20]       	= {"新增用户", "Add User"};
const char g_ucaLng_SET_NoUserErr[LANGUAGE_END][35]       	= {"【请输入用户名】", "【Please Enter User Name】"};
const char g_ucaLng_SET_UserExists[LANGUAGE_END][35]       	= {"【用户名已存在】", "【The User Already Exists】"};


const char g_ucaLng_SET_RecoverPW[LANGUAGE_END][40]         = {"恢复出厂密码", "Recovery Password To Factory Settings"};
const char g_ucaLng_SET_Recover001PW[LANGUAGE_END][55]      = {"恢复001账户出厂密码", "Recovery Password Of User 001 To Factory Settings."};
const char g_ucaLng_SET_RecoverAdminPW[LANGUAGE_END][55]    = {"恢复admin账户出厂密码", "Recovery Password Of User Admin To Factory Settings."};
const char g_ucaLng_SET_RecoverTestPW[LANGUAGE_END][55]     = {"恢复测试账户出厂密码", "Recovery Password Of User Test To Factory Settings."};
const char g_ucaLng_SET_RecoverEnginePW[LANGUAGE_END][55]   = {"恢复工程师账户出厂密码", "Recovery Password Of User Yongfu To Factory Settings."};
const char g_ucaLng_SET_ChangeSning[LANGUAGE_END][70]       = {"修改仪器序列号中，请勿断电\r\n完毕后自动重启...", "Do Not Turn Off The Power While Modifying Product Sn"};
const char g_ucaLng_SET_DelTestDataing[LANGUAGE_END][25]    = {"删除测试数据中...", "Delete Analysis Data…"};
const char g_ucaLng_SET_DelUserDataing[LANGUAGE_END][25]    = {"删除用户数据中...", "Delete User Data..."};
const char g_ucaLng_SET_RecoverFactory[LANGUAGE_END][70]    = {"恢复出厂设置中，请勿断电\r\n完毕后自动重启...", "Do Not Turn Off The Power While Recover Factor"};


/***********登录/关机*********************************************************/
const char g_ucaLng_Login_SelfCheck[LANGUAGE_END][35] = {"开机自检中", "Self Checking During Startup."};
const char g_ucaLng_PowerOff_Prompt[LANGUAGE_END][50] = {"正在关机中\r\n关机后请关闭电源", "Shutting Down…\r\nPlease Close Power Switch"};
const char g_ucaLng_Selfcheck_Err[LANGUAGE_END][25] = {"登录自检异常", "Self Checking Fail"};

/***********其他*********************************************************/
const char g_ucaLng_ErrorCode[LANGUAGE_END][13]       = {"故障码", "Error Code"};
const char g_ucaLng_ErrorName[LANGUAGE_END][13]       = {"故障名称", "Error Name"};
const char g_ucaLng_ErrorHandle[LANGUAGE_END][13] 	  = {"故障处理", "Error Handle"};
const char g_ucaLng_ClearError[LANGUAGE_END][13] 	  = {"清除故障", "Clear Error"};
const char g_ucaLng_ClearErrorBeforCount[LANGUAGE_END][40] 	  = {"请先清除故障，再分析", "Please Clear All Errors Before Analyse"};
const char g_ucaLng_ClearErrorPrompt[LANGUAGE_END][28] 	  = {"清除故障提示:", "Tips For Error Clearance"};
const char g_ucaLng_Printer_NoPaper_ErrPrompt[LANGUAGE_END][55] = {"请检测打印机是否缺纸!", "Please Check If The Printer Has Run Out Of Paper!"};
const char g_ucaLng_LOCK_MACHINE[LANGUAGE_END][55] 	  = {"仪器锁定中，请联系厂家售后", "Product Is Locked, Please Contact Your Agent For Help"};
const char g_ucaLng_UNLOCK_MACHINE[LANGUAGE_END][40]  = {"仪器解锁成功，重启中...", "Product Has Been Unlocked, Restarting..."};
const char g_ucaLng_START_UPDATE[LANGUAGE_END][50] 	  = {"系统配置中，请勿断电...", "System Is Being Upgraged, Please Wait..."};
const char g_ucaLng_Success[LANGUAGE_END][8] 	      = {"成功", "Success"};
const char g_ucaLng_Failure[LANGUAGE_END][8] 	      = {"失败", "Failure"};
const char g_ucaLng_InFailure[LANGUAGE_END][22] 	  = {"进仓异常", "Movement Failure"};
const char g_ucaLng_Error[LANGUAGE_END][10] 	      = {"异常", "Abnormal"};
const char g_ucaLng_InputError[LANGUAGE_END][13] 	  = {"输入错误", "Input Error"};
const char g_ucaLng_DataTime_InputError[LANGUAGE_END][19] = {"时间格式错误", "Time Format Error"};


/*********** 质控 *********************************************************/
const char g_ucaLng_QC_Set[LANGUAGE_END][13]          = {"质控设定", "QC Setting"};
const char g_ucaLng_QC_Analysis[LANGUAGE_END][13]     = {"质控分析", "QC Analyse"};
//const char g_ucaLng_QC_Chart[LANGUAGE_END][10]      = {"质控图", "QC Chart"};
const char g_ucaLng_QC_List[LANGUAGE_END][13]         = {"质控列表", "QC List"};
const char g_ucaLng_New[LANGUAGE_END][7]              = {"新增", "New"};
const char g_ucaLng_Save[LANGUAGE_END][7]             = {"保存", "Save"};
const char g_ucaLng_QC_Type[LANGUAGE_END][7]          = {"质控", "QC"};
const char g_ucaLng_Data[LANGUAGE_END][7]             = {"数据", "Data"};
const char g_ucaLng_Capacity[LANGUAGE_END][9]         = {"容量", "Capacity"};
const char g_ucaLng_LoseControl[LANGUAGE_END][15]     = {"失控", "Out Of Control"};
const char g_ucaLng_LotNO[LANGUAGE_END][9]            = {"批号", "Lot"};
const char g_ucaLng_UpperLimit[LANGUAGE_END][13]      = {"上限", "Upper Limit"};
const char g_ucaLng_LowerLimit[LANGUAGE_END][13]      = {"下限", "Lower Limit"};
const char g_ucaLng_Level[LANGUAGE_END][7]            = {"水平", "Level"};
const char g_ucaLng_High[LANGUAGE_END][5]             = {"高", "High"};
const char g_ucaLng_Middle[LANGUAGE_END][7]           = {"中", "Middle"};
const char g_ucaLng_Low[LANGUAGE_END][4]              = {"低", "Low"};
const char g_ucaLng_FileNOIsFull[LANGUAGE_END][55]    = {"文件列表已满,请先删除,再新建", "NO Extra Space For New File.Please Delete Before Use"}; //File NO List Is Full,Please Do It After Delete
const char g_ucaLng_InputParamError[LANGUAGE_END][19] = {"输入参数错误", "PARA. Input Error"}; //Input Param Error
const char g_ucaLng_FileNO[LANGUAGE_END][10]          = {"文件号", "File Num"};
const char g_ucaLng_Seter[LANGUAGE_END][15]           = {"设定者", "Operator"};
const char g_ucaLng_ValidDate[LANGUAGE_END][18]       = {"有效期", "Expiration Date"}; //Expiration Date
const char g_ucaLng_QCSampleSN[LANGUAGE_END][19]      = {"质控样本编号", "QC NO."};
const char g_ucaLng_FileNOSelect[LANGUAGE_END][18]    = {"文件号选择", "Select File NO."};
const char g_ucaLng_FileNOInvalid[LANGUAGE_END][18]   = {"文件号无效", "Invalid File NO."};
const char g_ucaLng_ResultInCtrl[LANGUAGE_END][18]    = {"结果在控", "Result In Control"};
const char g_ucaLng_ResultOutCtrl[LANGUAGE_END][20]   = {"结果失控", "Result Out Control"};
const char g_ucaLng_DateTime_Format[LANGUAGE_END][13] = {"时间格式", "Time Format"};

/***********错误码对应的名称*********************************************************/
const char g_ucaLng_ErrCodeName_BuildPress[LANGUAGE_END][25] = {"建立负压异常", "Setting Pressure Error"}; //Build Press Error
const char g_ucaLng_ErrCodeName_AirLight[LANGUAGE_END][22]   = {"系统气密性异常", "Gas Leakage Abnormal"}; //System Air Light Error
const char g_ucaLng_ErrCodeName_BC_Current[LANGUAGE_END][32] = {"恒流源异常", "Constant Current Source Error"}; //Current Error
const char g_ucaLng_ErrCodeName_12VP[LANGUAGE_END][19]       = {"正12伏异常", "Positive 12v Error"}; //12v P Error
const char g_ucaLng_ErrCodeName_12VN[LANGUAGE_END][19]       = {"负12伏异常", "Negtive 12v Error"}; //12v N Error
const char g_ucaLng_ErrCodeName_OutIn_Out[LANGUAGE_END][32]  = {"计数仓出仓异常", "Movement Coming Out Error"}; //Outin Module Out Err
const char g_ucaLng_ErrCodeName_OutIn_In[LANGUAGE_END][32]   = {"计数仓进仓异常", "Movement Coming In Error"}; //Outin Module In Err
const char g_ucaLng_ErrCodeName_OutIn_OC[LANGUAGE_END][25]   = {"进出仓光耦异常", "Movement OC Error"}; //OutIn Module OC Err
const char g_ucaLng_ErrCodeName_TZ_Touch[LANGUAGE_END][28]   = {"探针电机对接异常", "Building Connection Error"}; //Abutting Joint Out Err
const char g_ucaLng_ErrCodeName_TZ_Release[LANGUAGE_END][25] = {"探针电机松开异常", "Disconnection Error"}; //Abutting Joint In Err
const char g_ucaLng_ErrCodeName_Clamp_OC[LANGUAGE_END][25]   = {"对接光耦异常", "Connection OC Error"}; //TZ Module OC Err
const char g_ucaLng_ErrCodeName_NoAir[LANGUAGE_END][23]      = {"气嘴漏气", "Gas Connector Leakage"}; //Air Leak
const char g_ucaLng_ErrCodeName_Elec_Triggle[LANGUAGE_END][33] = {"定量电极触发异常", "Trigger Feedback Electrode Error"}; //Elec Triggle Err
const char g_ucaLng_ErrCodeName_No_Paper[LANGUAGE_END][35]   = {"打印机缺纸", "Printer Is Running Out Of Paper"}; //Printer NO Paper
const char g_ucaLng_ErrCodeName_No_Periter[LANGUAGE_END][25] = {"打印机未就绪", "Printer Connection Error"}; //Printer Conn Err
const char g_ucaLng_ErrCodeName_4G_Cat1[LANGUAGE_END][25]    = {"4g_Cat1模块异常", "4G_CAT1 Module Error"}; //4g_Cat Module Err
const char g_ucaLng_ErrCodeName_Spi_Flash[LANGUAGE_END][30]  = {"Spi Flash连接异常", "Spi Flash Connection Error"}; //Spi Flash Conn Err
const char g_ucaLng_ErrCodeName_Cell_Check[LANGUAGE_END][46] = {"血细胞模块未取出或检测模块异常", "Cassette Error Or Not Being Taken Out"}; //Cell Not Get Out Or Check Module Error
const char g_ucaLng_ErrCodeName_HGB[LANGUAGE_END][20] = {"HGB模块异常", "HGB System Error"}; //HGB Module Err
const char g_ucaLng_ErrCodeName_Disk[LANGUAGE_END][25] = {"磁盘容量不足", "Not Enough Disk Space"}; //Nand Capacity Err
const char g_ucaLng_Prompt_ClearErr[LANGUAGE_END][65] = {"请点击清除故障,若故障无法清除,请联系售后!", "Please Clear Error, If Not Be Cleared, Please Contact Sales!"}; //Nand Capacity Err



/*
const char g_ucaLng_[LANGUAGE_END][8] = {"", ""};
const char g_ucaLng_[LANGUAGE_END][8] = {"", ""};
const char g_ucaLng_[LANGUAGE_END][8] = {"", ""};
*/



 //用作背景的BMP图相关变量
//GUI_HMEM hMemBKBMP;						
//char* BKBmaDataBuf;
//uint32_t BKBMPBufLen;


//正确图标的BMP图相关变量，对话框或者选择对话框启动就只加载一次图片，并且不再释放也不再加载
//GUI_HMEM hMemSuccessBmp = NULL;
//char* SuccessBmpDataBuf = NULL;
//uint32_t SuccessBmpBufLen = 0;

//错误图标的BMP图相关变量，对话框或者选择对话框启动就只加载一次图片，并且不再释放也不再加载
//GUI_HMEM hMemErrBmp = NULL;
//char* ErrBmpDataBuf = NULL;
//uint32_t ErrBmpBufLen = 0;

//自检中图标的BMP图相关变量，对话框或者选择对话框启动就只加载一次图片，并且不再释放也不再加载
//GUI_HMEM hMemSelfCheckBmp = NULL;
//char* SelfCheckBmpDataBuf = NULL;
//uint32_t SelfCheckBmpBufLen = 0;




 
 //ID
#define ID_WINDOW_PUBLIC                    (GUI_PUBLIC_MENU_ID + 0x01)
#define ID_PUBLIC_BUTTON_MENU               (GUI_PUBLIC_MENU_ID + 0x02)
#define ID_PUBLIC_BUTTON_ANALYSIS           (GUI_PUBLIC_MENU_ID + 0x03)
#define ID_PUBLIC_BUTTON_LIST_REVIEW        (GUI_PUBLIC_MENU_ID + 0x04)
#define ID_PUBLIC_BUTTON_ERROR_PROMPT       (GUI_PUBLIC_MENU_ID + 0x05)
#define ID_PUBLIC_TEXT_NEXT_SN              (GUI_PUBLIC_MENU_ID + 0x06)
#define ID_PUBLIC_TEXT_TEST_MODE            (GUI_PUBLIC_MENU_ID + 0x07)
#define ID_PUBLIC_TEXT_USER                 (GUI_PUBLIC_MENU_ID + 0x08)
#define ID_PUBLIC_TEXT_DATE_TIME            (GUI_PUBLIC_MENU_ID + 0x09)
 
//timer id
#define ID_PUBLIC_TIMER                    (GUI_PUBLIC_MENU_ID + 0x0A)
//timer perodic
 #define PUBLIC_TIMER_PERIODIC              10000 //10s
 
// color
#define GUI_BTN_PUBLIC_ERROR_PROMPT_COLOR   0x004FA5FF
 

static char Menu_Btn_Pressed = 0;
static char Analysis_Btn_Pressed = 0;
static char ListReview_Btn_Pressed = 0;



 /*********************************************************************
*
*       _aDialogCreate
*/
 /*
 static const GUI_WIDGET_CREATE_INFO Public_aDialogCreate[] = {
   { WINDOW_CreateIndirect, "",     			ID_WINDOW_PUBLIC,     0, 0, 0, 800, 480, 0, 0x0 },
   { BUTTON_CreateIndirect, "样本分析",      ID_BUTTON_ANALYSIS,     95, 0, 180, 55, 0, 0x0, 0 },
   { BUTTON_CreateIndirect, "列表回顾",      ID_BUTTON_LIST_REVIEW,  300, 0, 180, 55, 0, 0x0, 0 },
   { BUTTON_CreateIndirect, "",       		ID_BUTTON_ERROR_PROMPT, 495, 15, 200, 30, 0, 0x0, 0 },
   { TEXT_CreateIndirect, "logo",          ID_TEXT_LOGO,           710, 14, 80, 30, 0, 0x0, 0 },  
   {BUTTON_CreateIndirect, "",           	ID_BUTTON_MENU,         10, 0, 60, 55, 0, 0x0, 0 },
   // USER START (Optionally insert additional widgets)
   // USER END
 };
*/



/*
* analysis btn callback
*/
 void Public_Menu_Btn_Callback(WM_MESSAGE* pMsg)
 {
     const unsigned char RoundRadius = 20; //显示区域圆角半径
     switch (pMsg->MsgId)
     {
     case WM_PAINT:
     {
         GUI_RECT Rect;
         WM_GetClientRect(&Rect);
         //frame
         /*
         GUI_POINT aPoint[] = { //(0, 0, 70, 55, WM_HBKWIN, WM_CF_SHOW, 
             {Rect.x0, Rect.y0},{Rect.x0, Rect.y1},{Rect.x1- RoundRadius, Rect.y1},\
         {Rect.x1 - RoundRadius, Rect.y1 - RoundRadius}, { Rect.x1 - RoundRadius, Rect.y1 },\
         {Rect.x0, Rect.y1},
         };
         */

         GUI_POINT aPoint[] = { //(0, 0, 70, 55, WM_HBKWIN, WM_CF_SHOW, 
            {0, 0},{0, 70},{50, 55},\
            {50, 35}, {70, 35},\
            {70, 0},};

         //draw frame
         //if (BUTTON_IsPressed(pMsg->hWin)) {
         if(Menu_Btn_Pressed){
             GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
             GUI_AA_FillPolygon(aPoint, GUI_COUNTOF(aPoint), 0, 0);
             GUI_AA_FillCircle(Rect.x1 - RoundRadius, Rect.y1 - RoundRadius, RoundRadius);
             //菜单
             //GUI_PNG_Draw(_acmenu_btn_active, GUI_COUNTOF(_acmenu_btn_active), 20, 20);
			 Show_BK_BMP(BK_SDRAM,ICON_MENU_BTN, 20, 20);
         }
         else {
             GUI_SetColor(MachInfo.companyInfo.skin.bg);
             GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, 0);
             //GUI_AA_FillPolygon(aPoint, sizeof(aPoint), 0, 0);
             //GUI_AA_FillCircle(Rect.x1 - RoundRadius, Rect.y1 - RoundRadius, RoundRadius);
             //菜单
             //GUI_PNG_Draw(_acmenu_btn_idle, GUI_COUNTOF(_acmenu_btn_idle), 20, 20);
			 Show_BK_BMP(BK_SDRAM,ICON_MENU_BTN_UN, 20, 20);
         }
     }
     break;
     default:
         BUTTON_Callback(pMsg);
         break;
     }
    //
    if (pMsg->MsgId == WM_TOUCH) {
        if (BUTTON_IsPressed(pMsg->hWin)) {
            if (!Menu_Btn_Pressed) {
                Menu_Btn_Pressed = 1;
				Analysis_Btn_Pressed = 0;
				ListReview_Btn_Pressed = 0;
            }
        }
        else {
         //   Menu_Btn_Pressed = 0;
        }
    }
 }


/*
* analysis btn callback
*/
void Public_Analysis_Btn_Callback(WM_MESSAGE* pMsg)
{
    const unsigned char RoundRadius = 20; //显示区域圆角半径
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
    {
        GUI_RECT Rect;
        WM_GetClientRect(&Rect);
		
         //if (BUTTON_IsPressed(pMsg->hWin)) {
         if(Analysis_Btn_Pressed){
            GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
            GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, RoundRadius);

			 Show_BK_BMP(BK_SDRAM,ICON_ANALYSIS_BTN, 45, 35);
        
            GUI_SetBkColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
            GUI_SetColor(MachInfo.companyInfo.skin.bg);
             //样本分析
            GUI_DispStringAt(g_ucaLng_SampleAnalysis[MachInfo.systemSet.eLanguage], 80, 40); 
        }
        else {
            GUI_SetColor(MachInfo.companyInfo.skin.bg);
            //GUI_AA_FillPolygon(aPoint, GUI_COUNTOF(aPoint), 0, 0);
            GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, RoundRadius);
            GUI_SetColor(MachInfo.companyInfo.skin.bg);
            GUI_AA_DrawRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, RoundRadius);
            //
			Show_BK_BMP(BK_SDRAM,ICON_ANALYSIS_BTN_UN, 45, 35);
            
            //样本分析
            GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
            GUI_SetBkColor(MachInfo.companyInfo.skin.bg);
            GUI_DispStringAt(g_ucaLng_SampleAnalysis[MachInfo.systemSet.eLanguage], 80, 40);
        }
    }
    break;
    default:
        BUTTON_Callback(pMsg);
        break;
    }
    //
    if (pMsg->MsgId == WM_TOUCH) {
        if (BUTTON_IsPressed(pMsg->hWin)) {
            if (Analysis_Btn_Pressed == 0) {
                Analysis_Btn_Pressed = 1;
				ListReview_Btn_Pressed = 0;
				Menu_Btn_Pressed       = 0;
            }
        }
        else {
        //    Analysis_Btn_Pressed = 0;
        }
    }
}

/*
* list review btn callback
*/

void Public_ListReview_Btn_Callback(WM_MESSAGE* pMsg)
{
    const unsigned char RoundRadius = 20; //显示区域圆角半径
    switch (pMsg->MsgId)
    {
        case WM_PAINT:
        {
            GUI_RECT Rect;
            WM_GetClientRect(&Rect);
            // frame
            //GUI_POINT aPoint[] = {
            //    {0, 0},{0, 40},{15, 40},{15, 54}, {164, 54},{164, 40},{179, 40}, {179, 0}
            //};

            //if (BUTTON_IsPressed(pMsg->hWin)) {
             if (ListReview_Btn_Pressed) {
                //draw frame
                GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
                //GUI_AA_FillPolygon(aPoint, GUI_COUNTOF(aPoint), 0, 0);
                //GUI_AA_FillCircle(14, 40, 15);
                //GUI_FillCircle(164, 40, 15);
                GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, RoundRadius);

                //GUI_PNG_Draw(_aclist_btn_active, sizeof(_aclist_btn_active), 45, 35);
				 Show_BK_BMP(BK_SDRAM,ICON_LIST_BTN, 45, 35);
                GUI_SetBkColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
                GUI_SetColor(MachInfo.companyInfo.skin.bg);
                 //列表回顾
                GUI_DispStringAt(g_ucaLng_ListReview[MachInfo.systemSet.eLanguage], 80, 40);
            }
            else {
                GUI_SetColor(MachInfo.companyInfo.skin.bg);
                GUI_AA_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, RoundRadius);
                GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
                GUI_SetBkColor(MachInfo.companyInfo.skin.bg);
				Show_BK_BMP(BK_SDRAM,ICON_LIST_BTN_UN, 45, 35);
                //列表回顾
                GUI_DispStringAt(g_ucaLng_ListReview[MachInfo.systemSet.eLanguage], 80, 40);
            }
       
        } 
    break;
    default:
        BUTTON_Callback(pMsg);
        break;
    }
    //
    if (pMsg->MsgId == WM_TOUCH) {
        if (BUTTON_IsPressed(pMsg->hWin)) {
            if (ListReview_Btn_Pressed == 0) {
                ListReview_Btn_Pressed = 1;
				Analysis_Btn_Pressed   = 0;
				Menu_Btn_Pressed       = 0;
            }
        }
        else {
        //    ListReview_Btn_Pressed = 0;
        }
    }
}



/*
* error prompt btn callback, 495,15(200, 30)
*/
//static char Public_Errorprompt_Btn_Pressed = 0;
void Public_ErrorPrompt_Btn_Callback(WM_MESSAGE* pMsg)
{
    extern __IO MachRunPara_s MachRunPara;
    
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
    {
        GUI_RECT Rect;
        WM_GetClientRect(&Rect);
        //495,15
        if (BUTTON_IsPressed(pMsg->hWin)) {
        //if (Public_Errorprompt_Btn_Pressed) {
            // draw frame
            GUI_SetColor(MachInfo.companyInfo.skin.bg);
            GUI_FillRectEx(&Rect);
            GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
            GUI_FillRect(Rect.x0 + 15, Rect.y0, Rect.x1 - 15, Rect.y1);

            GUI_AA_FillCircle(15, 15, 15);
            GUI_AA_FillCircle(185, 15, 15);

            //draw nums
            GUI_SetBkColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
            GUI_SetColor(MachInfo.companyInfo.skin.highlightKHNotSelectFont);
            //GUI_SetFont(&GUI_Font16_ASCII);
            
            //显示错误码或信息
            if(MachRunPara.tLogin_SelfCheck.ucErrorNum >= 1)
            {
                char Buffer[10] = {0};
                sprintf(Buffer, "E%d", MachRunPara.tLogin_SelfCheck.eaErrorCodeBuffer[0]);
                GUI_DispStringAt(Buffer, 43, 8);
            }
            // flag
            //Draw_Const_Logos(LOGO_circle_error_prompt ,20, 8);

        }
        else {
            GUI_SetColor(MachInfo.companyInfo.skin.bg);
            GUI_FillRectEx(&Rect);
            GUI_SetColor(GUI_WHITE);
            GUI_FillRect(Rect.x0 + 15, Rect.y0, Rect.x1 - 15, Rect.y1);
            GUI_AA_FillCircle(15, 15, 15);
            GUI_AA_FillCircle(185, 15, 15);

            // draw nums
            GUI_SetColor(GUI_BTN_PUBLIC_ERROR_PROMPT_COLOR);
            GUI_SetBkColor(GUI_WHITE); 
            //GUI_SetFont(&GUI_Font16_ASCII);
            
            //显示错误码或信息
            if(MachRunPara.tLogin_SelfCheck.ucErrorNum >= 1)
            {
                char Buffer[10] = {0};
                sprintf(Buffer, "E%d", MachRunPara.tLogin_SelfCheck.eaErrorCodeBuffer[0]);
                GUI_DispStringAt(Buffer, 43, 8);
            }
            //当错误大于0时，显示感叹号+黄色圆形
            if(MachRunPara.tLogin_SelfCheck.ucErrorNum > 0)
            {
				Show_BK_BMP(BK_SDRAM,ICON_ERR_YUAN_BTN, 20, 8);
            }
        }
    }
    break;
    default:
        BUTTON_Callback(pMsg);
        break;
    }
    
//    //
//    if (pMsg->MsgId == WM_TOUCH) {
//        if (BUTTON_IsPressed(pMsg->hWin)) {
//            if (!Public_Errorprompt_Btn_Pressed) {
//                Public_Errorprompt_Btn_Pressed = 1;
//            }
//        }
//        else {
//        //    Public_Errorprompt_Btn_Pressed = 0;
//        }
//    }
}


/*
* 以下三个按键，需做"长按状态"的显示效果处理
*/
void Public_Reset_Btn_Status(BtnInde_e eIndex)
{
    if(BTN_ANALYSIS ==  eIndex){
        ListReview_Btn_Pressed = 0;
        Analysis_Btn_Pressed   = 1;
        Menu_Btn_Pressed       = 0;
        //WM_InvalidateWindow(WM_GetDialogItem(pMsg->hWin, ID_PUBLIC_BUTTON_ANALYSIS));
    }else if(BTN_LISTREVIEW ==  eIndex){
        ListReview_Btn_Pressed = 1;
        Analysis_Btn_Pressed   = 0;
        Menu_Btn_Pressed       = 0;
        //WM_InvalidateWindow(WM_GetDialogItem(pMsg->hWin, ID_PUBLIC_BUTTON_LIST_REVIEW));
    }else if(BTN_MENU ==  eIndex){
        ListReview_Btn_Pressed = 0;
        Analysis_Btn_Pressed   = 0;
        Menu_Btn_Pressed       = 1;
        //WM_InvalidateWindow(WM_GetDialogItem(pMsg->hWin, ID_PUBLIC_BUTTON_MENU));
    }else{
        ListReview_Btn_Pressed = 0;
        Analysis_Btn_Pressed   = 0;
        Menu_Btn_Pressed       = 0;
    }
    
}






/*
*   公共重绘部分，接口
*/

static BUTTON_Handle hMenu_Btn;
static BUTTON_Handle hAnalysis_Btn;
static BUTTON_Handle hListReview_Btn;
BUTTON_Handle hErrorPrompt_Btn;
static TEXT_Handle   hNextSN_Text;
//static TEXT_Handle   hTestMode_Text;
static TEXT_Handle   hUser_Text;
static TEXT_Handle   hDateTime_Text;

/*
static WM_HWIN hMenu_Btn;
static WM_HWIN hAnalysis_Btn;
static WM_HWIN hListReview_Btn;
static WM_HWIN hErrorPrompt_Btn;
static WM_HWIN   hNextSN_Text;
static WM_HWIN   hTestMode_Text;
static WM_HWIN   hUser_Text;
static WM_HWIN   hDateTime_Text;
*/

/*
    隐藏控件
*/
void Public_Hide_Widget(void)
{
    WM_HideWindow(hMenu_Btn);
    WM_HideWindow(hAnalysis_Btn);
    WM_HideWindow(hListReview_Btn);
    WM_HideWindow(hErrorPrompt_Btn);
    WM_HideWindow(hNextSN_Text);
    WM_HideWindow(hUser_Text);
    WM_HideWindow(hDateTime_Text);
}


/*
    显示控件
*/
void Public_Show_Widget(void)
{
    WM_ShowWindow(hMenu_Btn);
    WM_ShowWindow(hAnalysis_Btn);
    WM_ShowWindow(hListReview_Btn);
    WM_ShowWindow(hErrorPrompt_Btn);
    WM_ShowWindow(hNextSN_Text);
    WM_ShowWindow(hUser_Text);
    WM_ShowWindow(hDateTime_Text);

}



/*
    刷新菜单、样本分析、列表回顾三个控件
*/
void Pulic_Btn_Update(void)
{
    WM_InvalidateWindow(hMenu_Btn);
    WM_InvalidateWindow(hAnalysis_Btn);
    WM_InvalidateWindow(hListReview_Btn);
}



/*
*   更新公共界面中，错误提示按键
*/
void Update_Public_ErrPrompt_Btn(void)
{
    WM_InvalidateWindow(hErrorPrompt_Btn);
}


/*
*   失能公共界面中，错误提示按键
*/
void Disable_Public_ErrPrompt_Btn(void)
{
    WM_DisableWindow(hErrorPrompt_Btn);
}


/*
*   使能公共界面中，错误提示按键
*/
void Enable_Public_ErrPrompt_Btn(void)
{
    WM_EnableWindow(hErrorPrompt_Btn);
}




/*
* 背景初始化，在UI_Init中调用
*/
void Public_Msg_Init(void)
{
    extern __IO MachRunPara_s	MachRunPara;
    GUI_SetBkColor(MachInfo.companyInfo.skin.bg);
    GUI_Clear();

    //menu btn
     hMenu_Btn = BUTTON_CreateEx(0, 0, 70, 50, WM_HBKWIN, WM_CF_SHOW, 0, ID_PUBLIC_BUTTON_MENU);
     WM_SetCallback(hMenu_Btn, Public_Menu_Btn_Callback);

    // analysis btn
    hAnalysis_Btn = BUTTON_CreateEx(95, -20, 180, 70, WM_HBKWIN, WM_CF_SHOW, 0, ID_PUBLIC_BUTTON_ANALYSIS);
    WM_SetCallback(hAnalysis_Btn, Public_Analysis_Btn_Callback);

    // list review btn
    hListReview_Btn = BUTTON_CreateEx(300, -20, 180, 70, WM_HBKWIN, WM_CF_SHOW, 0, ID_PUBLIC_BUTTON_LIST_REVIEW);
    WM_SetCallback(hListReview_Btn, Public_ListReview_Btn_Callback);

    // error prompt btn
    hErrorPrompt_Btn = BUTTON_CreateEx(580, 15, 200, 30, WM_HBKWIN, WM_CF_SHOW, 0, ID_PUBLIC_BUTTON_ERROR_PROMPT);
    WM_SetCallback(hErrorPrompt_Btn, Public_ErrorPrompt_Btn_Callback);
    BUTTON_SetFont(hErrorPrompt_Btn, &SIF_HZ16_SONGTI);

    // 下一编号
    hNextSN_Text = TEXT_CreateEx(20, 463, 200, 16, WM_HBKWIN, WM_CF_SHOW, 0, ID_PUBLIC_TEXT_NEXT_SN, 0);
    TEXT_SetFont(hNextSN_Text, &HZ_SONGTI_12);

    // 当前用户
    hUser_Text = TEXT_CreateEx(355, 463, 200, 16, WM_HBKWIN, WM_CF_SHOW, 0, ID_PUBLIC_TEXT_USER, 0);
    TEXT_SetFont(hUser_Text, &HZ_SONGTI_12);

    // 时间（实时更新）
    hDateTime_Text = TEXT_CreateEx(625, 463, 120, 16, WM_HBKWIN, WM_CF_SHOW, 0, ID_PUBLIC_TEXT_DATE_TIME, 0);
    TEXT_SetFont(hDateTime_Text, &HZ_SONGTI_12);
	TEXT_SetTextAlign(hDateTime_Text,GUI_TA_RIGHT | GUI_TA_VCENTER);
    
    //初始化时，样本分析按钮为选择状态
    Public_Reset_Btn_Status(BTN_ANALYSIS);
}





/*
*   更新公共界面，动态提示部分：下一样本编号，当前用户，时间
*/
static void Update_Public_Dynamic_Prompt(WM_MESSAGE *pMsg)
{
	extern  MachInfo_s	MachInfo;
	extern CATGlobalStatus_s CATGlobalStatus;
	
    char ucaBuffer[DATE_TIME_LEN+32] = {0};
    int BufferLen = DATE_TIME_LEN+32;
    
    //下一样本编号
    if(GUI_WINDOW_ANALYSIS_ID == Get_Cur_MenuID())
    {
        memset(ucaBuffer, 0, BufferLen);
        sprintf(ucaBuffer, "%s: CBC-%05d", g_ucaLng_NextSampleSN[MachInfo.systemSet.eLanguage], MachRunPara.tNextSample.ulNextDataSN+1);
        TEXT_SetText(hNextSN_Text, ucaBuffer);
    }else{
        TEXT_SetText(hNextSN_Text, " ");
    }

    // 当前用户
    memset(ucaBuffer, 0, BufferLen);
    sprintf(ucaBuffer, "%s:%s", g_ucaLng_CurUser[MachInfo.systemSet.eLanguage], MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].user);
    TEXT_SetText(hUser_Text, ucaBuffer);


    // 时间（实时更新）
    memset(ucaBuffer, 0, BufferLen);
    int len = strlen((char*)MachRunPara.ucaLastestDateTime);
    if(len > 3 ) len -= 3; //去掉秒
    strncpy(ucaBuffer, (char*)MachRunPara.ucaLastestDateTime, len);
    TEXT_SetText(hDateTime_Text, ucaBuffer);
	
	
	//英文版不显示信号强度标识
	if(MachInfo.systemSet.eLanguage == LANGUAGE_CHINESE){
		//先判断下是否存在第三方服务端，是，则这里显示和第三方服务端连接情况
		GUI_SetColor(MachInfo.companyInfo.skin.bg);
		GUI_FillRect(765,459,791,475);
		
		switch(MachInfo.companyInfo.company){
			case COMPANY_YSB:
			{
				//药师帮
				//显示第三方服务端连接情况
				if(CATGlobalStatus.flag.connOtherServerErr == 1){
					//连接中断
					GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
					GUI_DrawRect(766,470,770,475);
					GUI_DrawRect(776,465,780,475);
					GUI_DrawRect(786,460,790,475);
					
					GUI_SetColor(GUI_RED);
					GUI_SetPenSize(2);
					GUI_AA_DrawLine(766,460,790,475);
					GUI_AA_DrawLine(766,475,790,460);
				}else{
					if(CATGlobalStatus.signalStrength >= 25){
						GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
						GUI_FillRect(766,470,770,475);
						GUI_FillRect(776,465,780,475);
						GUI_FillRect(786,460,790,475);
					}else if(CATGlobalStatus.signalStrength >= 18){
						GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
						GUI_FillRect(766,470,770,475);
						GUI_FillRect(776,465,780,475);
						GUI_DrawRect(786,460,790,475);
					}else{
						GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
						GUI_FillRect(766,470,770,475);
						GUI_DrawRect(776,465,780,475);
						GUI_DrawRect(786,460,790,475);
					}
				}
			}break;
			
			default :
			{
				//显示创怀服务端连接情况
				if(CATGlobalStatus.flag.connServerErr == 1){
					//连接中断
					GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
					GUI_DrawRect(766,470,770,475);
					GUI_DrawRect(776,465,780,475);
					GUI_DrawRect(786,460,790,475);
					
					GUI_SetColor(GUI_RED);
					GUI_SetPenSize(2);
					GUI_AA_DrawLine(766,460,790,475);
					GUI_AA_DrawLine(766,475,790,460);
				}else{
					if(CATGlobalStatus.signalStrength >= 26){
						GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
						GUI_FillRect(766,470,770,475);
						GUI_FillRect(776,465,780,475);
						GUI_FillRect(786,460,790,475);
					}else if(CATGlobalStatus.signalStrength >= 20){
						GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
						GUI_FillRect(766,470,770,475);
						GUI_FillRect(776,465,780,475);
						GUI_DrawRect(786,460,790,475);
					}else{
						GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
						GUI_FillRect(766,470,770,475);
						GUI_DrawRect(776,465,780,475);
						GUI_DrawRect(786,460,790,475);
					}
				}
			}break;
		}
	}
}



/*
*   公共重绘部分，接口
*/
void Public_Msg_Paint(WM_MESSAGE* pMsg)
{
    GUI_SetBkColor(MachInfo.companyInfo.skin.bg);
    GUI_Clear();

    Update_Public_Dynamic_Prompt(pMsg);

    Public_Show_Widget();
    
}



/*
*   Edit 回调函数（公用）
*/
void Public_Edit_Callback(WM_MESSAGE *pMsg)
{
    char Buffer[64] = {0};
    
    switch (pMsg->MsgId)
    {
        //EDIT_Callback(pMsg);
        case WM_PAINT:
        {
            GUI_RECT Rect;
            WM_GetClientRect(&Rect);
            //fill color
            GUI_SetColor(GUI_WHITE);
            GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);
            
            //draw frame
            GUI_SetColor(EDIT_FRAME_COLOR);
            GUI_AA_DrawRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, EDIT_ROUNDED_RADIUS);
            //set text
            GUI_SetBkColor(GUI_WHITE);
            GUI_SetColor(GUI_BLACK);      
       
            EDIT_GetText(pMsg->hWin, Buffer, sizeof(Buffer));  
            Rect.x0 += 5;
            GUI_DispStringInRect(Buffer, &Rect, GUI_TA_HCENTER |GUI_TA_VCENTER);            
        }
        break;
        default:
            EDIT_Callback(pMsg);
        break;
    }
}



/*
*   DropDown 回调函数（公用）
*/
void Public_Dropdown_Callback(WM_MESSAGE* pMsg)
{
    const unsigned char RoundRadius = 4; //显示区域圆角半径
    uint8_t ucSel = 0;
    char Buffer[32] = {0};
    
    switch (pMsg->MsgId)
    {
        case WM_PAINT:
        {
            GUI_RECT Rect;
            WM_GetClientRect(&Rect);
            GUI_SetColor(GUI_WHITE);
            GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);

            //draw frame
            GUI_SetColor(DROPDOWN_FRAME_COLOR);
            GUI_AA_DrawRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, RoundRadius);
            //draw arrow
			Show_BK_BMP(BK_SDRAM,ICON_XJT_BTN, Rect.x1 - 20, Rect.y1 - 20);
            //draw text
            ucSel = DROPDOWN_GetSel(pMsg->hWin);
            DROPDOWN_GetItemText(pMsg->hWin, ucSel, Buffer, sizeof(Buffer));            
            GUI_SetColor(GUI_BLACK);
            GUI_SetBkColor(GUI_WHITE);    
            Rect.y0 += 3;
            Rect.x0 += 3;            
            GUI_DispStringInRect(Buffer, &Rect, GUI_TA_VCENTER |GUI_TA_LEFT);
        }
        break;
        default:
            DROPDOWN_Callback(pMsg);
            break;
    }
}



/*
*   电机报错提示窗口，处理
*/
void Motor_Error_Prompt(WM_MESSAGE *pMsg, WM_WINDOW_CREATE *pWindowCreate)
{
    CommonDialogPageData_s DialogPageData = {0};
    WM_MESSAGE UserMsg;
    //
    GUI_EndDialog(pMsg->hWin, 0);
    g_hActiveWin = CreateDialogPage();
     //
    DialogPageData.bmpType = BMP_ALARM;
    DialogPageData.confimButt = HAS_CONFIM_BUTTON;
    DialogPageData.fun = pWindowCreate;
    //strcpy(DialogPageData.str,  g_ucaLng_OutIn_Error[MachInfo.systemSet.eLanguage]);
    sprintf(DialogPageData.str, "%sE%04d", g_ucaLng_OutIn_Error[MachInfo.systemSet.eLanguage], pMsg->Data.v);

    UserMsg.MsgId = WM_USER_DATA;
    UserMsg.Data.p = &DialogPageData;
    WM_SendMessage(g_hActiveWin,&UserMsg);
}

/*
    桌面窗口回调函数
*/
void Public_cbBkWin(WM_MESSAGE* pMsg)
{
//    WM_HWIN hItem;
    int     NCode;
    int     Id;

    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
        {
            Public_Msg_Init();
        }
        break;
        case WM_PAINT:
        {         
            Public_Msg_Paint(pMsg);          
        }
        break;
        case WM_PUBLIC_UPDATE: //更新状态栏，样本编号，账号，时间
        {
            Update_Public_Dynamic_Prompt(pMsg);
        }
        break;
//        case WM_TIMER:
//        {
//            Public_Msg_Paint(pMsg);
//            WM_RestartTimer(pMsg->Data.v, PUBLIC_TIMER_PERIODIC);
//        }
//        break;
        case WM_NOTIFY_PARENT:
        {
            Id = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;
            switch (Id)
            {
                case ID_PUBLIC_BUTTON_MENU: // Notifications sent by 'Menu_menu'
                switch (NCode) {
                case WM_NOTIFICATION_CLICKED:
                    // USER START (Optionally insert code for reacting on notification message)
                    Public_Reset_Btn_Status(BTN_MENU);
                    Pulic_Btn_Update();
                    // USER END
                    break;
                case WM_NOTIFICATION_RELEASED:
                    // USER START (Optionally insert code for reacting on notification message)
					Public_Reset_Btn_Status(BTN_MENU);
                    Pulic_Btn_Update();
                    GUI_EndDialog(g_hActiveWin,0);
                    g_hActiveWin = Createmenu_Window();
                    // USER END
                    break;
                case WM_NOTIFICATION_MOVED_OUT:
                    // USER START (Optionally insert code for reacting on notification message)
					GUI_EndDialog(g_hActiveWin,0);
					g_hActiveWin = Createmenu_Window();
                    // USER END
                    break;
                    // USER START (Optionally insert additional code for further notification handling)
                    // USER END
                }
                break;
                case ID_PUBLIC_BUTTON_ANALYSIS: // Notifications sent by 'analysis_btn'
                switch (NCode) {
                case WM_NOTIFICATION_CLICKED:
                    // USER START (Optionally insert code for reacting on notification message)
                    Public_Reset_Btn_Status(BTN_ANALYSIS);
                    Pulic_Btn_Update();
                    // USER END
                    break;
                case WM_NOTIFICATION_RELEASED:
                    // USER START (Optionally insert code for reacting on notification message)
					Public_Reset_Btn_Status(BTN_ANALYSIS);
					Pulic_Btn_Update();
                    GUI_EndDialog(g_hActiveWin, 0);
                    g_hActiveWin = Analysis_Menu();
                    
                    // USER END
                    break;
				
				case WM_NOTIFICATION_MOVED_OUT:
                    // USER START (Optionally insert code for reacting on notification message)
					GUI_EndDialog(g_hActiveWin, 0);
					g_hActiveWin = Analysis_Menu();
                    // USER END
                    break;
                    // USER START (Optionally insert additional code for further notification handling)
                    // USER END
                }
                break;
                case ID_PUBLIC_BUTTON_LIST_REVIEW: // Notifications sent by 'list_btn'
                    switch (NCode) {
                    case WM_NOTIFICATION_CLICKED:
                        // USER START (Optionally insert code for reacting on notification message)
                        Public_Reset_Btn_Status(BTN_LISTREVIEW);
                        Pulic_Btn_Update();
                        // USER END
                        break;
                    case WM_NOTIFICATION_RELEASED:
                        // USER START (Optionally insert code for reacting on notification message)
						Public_Reset_Btn_Status(BTN_LISTREVIEW);
						Pulic_Btn_Update();
                        GUI_EndDialog(g_hActiveWin, 0);
                        g_hActiveWin = CreateListReview_Window();
                        // USER END
                        break;
					
					case WM_NOTIFICATION_MOVED_OUT:
                    // USER START (Optionally insert code for reacting on notification message)
						GUI_EndDialog(g_hActiveWin, 0);
						g_hActiveWin = CreateListReview_Window();
						// USER END
						break;
                        // USER START (Optionally insert additional code for further notification handling)
                        // USER END
                    }
                    break;
                case ID_PUBLIC_BUTTON_ERROR_PROMPT: // Notifications sent by 'eerinfo_btn'
                    switch (NCode) {
                    case WM_NOTIFICATION_CLICKED:
                        // USER START (Optionally insert code for reacting on notification message)
                        Public_Reset_Btn_Status(BTN_END);
                        Pulic_Btn_Update();
                    // USER END
                        break;
                    case WM_NOTIFICATION_RELEASED:
                        // USER START (Optionally insert code for reacting on notification message)
                        Public_Reset_Btn_Status(BTN_END);
						Pulic_Btn_Update();
						GUI_EndDialog(g_hActiveWin, 0);
                        g_hActiveWin = ErrorInfo_CreateWindow();
                        // USER END
                        break;
                        // USER START (Optionally insert additional code for further notification handling)
                        // USER END
                    }
                    break;
            }
        }
        break;
        default:
            WM_DefaultProc(pMsg);
            break;
    }
}



/* 
*   界面背景窗口的绘制填充
*/
void Public_FrameBK(GUI_RECT *ptRect, uint16_t usX, uint16_t usY)
{
      const unsigned char RoundRadius = 10;   //显示区域圆角半径 
      GUI_RECT rect;
      WM_GetClientRect(ptRect); 
      GUI_SetColor(GUI_WHITE);
      GUI_AA_FillRoundedRect(rect.x0+usX, rect.y0, rect.x1-usY, rect.y1+RoundRadius, RoundRadius);
}



/*
*   窗口透明化设置
*/
void Window_Alpha(WM_MESSAGE* pMsg, GUI_RECT *ptRect)
{
    GUI_SetAlpha(PUBLIC_ALPHA_VALUE);
    GUI_SetColor(PUBLIC_ALPHA_COLOR);
    GUI_FillRectEx(ptRect);
    GUI_SetAlpha(0);
}


//透明背景， 回调函数, （该函数共用）
void AlphaBk_cbBkWin(WM_MESSAGE* pMsg)
{
    switch (pMsg->MsgId)
    {
        case WM_PAINT:
        {
            GUI_SetAlpha(PUBLIC_ALPHA_VALUE);
            GUI_SetColor(PUBLIC_ALPHA_COLOR);
            GUI_FillRect(0, 0, 800, 480);
            GUI_SetAlpha(0);
        }
        break;
        default:
        {
            WM_DefaultProc(pMsg);
        }
        break;
    }
}




//不透明背景， 回调函数, （该函数共用）
void NotAlphaBk_cbBkWin(WM_MESSAGE* pMsg)
{
    switch (pMsg->MsgId)
    {
        case WM_PAINT:
        {
//            GUI_SetAlpha(30);
            GUI_SetColor(0x00525a4e);
            GUI_FillRect(0, 0, 800, 480);
//            GUI_SetAlpha(0);
        }
        break;
        default:
        {
            WM_DefaultProc(pMsg);
        }
        break;
    }
}




//空操作（不做有效处理）， 回调函数, （该函数共用）
void None_cbBkWin(WM_MESSAGE* pMsg)
{
    switch (pMsg->MsgId)
    {
//        case WM_INIT_DIALOG:
//        {
//            GUI_SetAlpha(NEXT_SMAPLE_ALPHA);
//            GUI_SetColor(NEXT_SMAPLE_ALPHA_COLOR);
//            GUI_FillRect(0, 0, 800, 480);
//            GUI_SetAlpha(0);
//        }
        case WM_PAINT:
        {
           
        }
        break;
        default:
        {
            WM_DefaultProc(pMsg);
        }
        break;
    }
}


/*
*   记录当前界面编号（在建立界面前调用）
*/
void Set_Cur_MenuID(uint32_t ulID)
{
    extern __IO MachRunPara_s MachRunPara;
    
    MachRunPara.ulMenuID = ulID;
}
    

/*
*   获取当前界面编号
*/
uint32_t Get_Cur_MenuID(void)
{
    extern __IO MachRunPara_s MachRunPara;
    
    return MachRunPara.ulMenuID;
}
      




/*
*   根据错误码，返回对应的错误名称
*/
const char* ErrorCode_Prompt_Info(ErrorCode_e eErrorCode)
{
    const char *pErrorCodeInfo = NULL;
    
    switch(eErrorCode)
    {
        case ERROR_CODE_SUCCESS:
        {
            //正常

        }
        break;     
        case ERROR_CODE_FAILURE:
        {
        
        }
        break;
        case ERROR_CODE_BUILD_PRESS:        //建立负压异常
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_BuildPress[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_AIR_TIGHT:          //系统气密性异常
        {
             pErrorCodeInfo = g_ucaLng_ErrCodeName_AirLight[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_BC_CURRENT:         //恒流源异常(WBC)
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_BC_Current[MachInfo.systemSet.eLanguage];
        }
        break;     
        case ERROR_CODE_POWER_12VP:         //正12V, 供电异常
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_12VP[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_POWER_12VN:        //负12V, 供电异常
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_12VN[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_OUTIN_OUT:          //进出仓电机，出仓异常
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_OutIn_Out[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_OUTIN_IN:           //进出仓电机，进仓异常
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_OutIn_In[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_OUTIN_OC:   //进出仓光耦
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_OutIn_OC[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_CLAMP_OUT:          //探针电机，对接异常
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_TZ_Touch[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_CLAMP_IN:           //探针电机，松开异常  
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_TZ_Release[MachInfo.systemSet.eLanguage];
        }
        break;  
        case ERROR_CODE_CLAMP_OC:           //探针模块光耦
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_Clamp_OC[MachInfo.systemSet.eLanguage];
        }
        break;        
        case ERROR_CODE_NOT_AT_POS:         //未进仓导致计数失败
        {
           
        }
        break;
        case ERROR_CODE_AIR_LEAK:             //气嘴漏气
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_NoAir[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_NONE_CELL:            //计数未放入血细胞检测模块
        {
            
        }
        break;
        case ERROR_CODE_ELEC_TRIGGLE:      //定量电极异常触发
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_Elec_Triggle[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_WBC_ELEC_TOUCH:         //采样电极接触异常
        {
            //pErrorCodeInfo = g_ucaLng_ErrCodeName_BC_Current[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_PRINT_NO_PAPER:     //打印机缺纸
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_No_Paper[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_NO_PRINTER:         //打印机未就绪
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_No_Periter[MachInfo.systemSet.eLanguage];
        }
        break;

        case ERROR_CODE_MONITOR_SIGNAL_ERR: //计数电极接触异常（信号稳定度检测阶段）
        {
            //pErrorCodeInfo = g_ucaLng_ErrCodeName_BC_Current[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_ALGO_MODE:          //算法判断设置模式异常
        {
            //pErrorCodeInfo = g_ucaLng_ErrCodeName_BC_Current[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_ALGO_BREAK:         //算法退出运算
        {
            //pErrorCodeInfo = g_ucaLng_ErrCodeName_BC_Current[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_TEMPERATURE:        //温度异常  
        {
           // pErrorCodeInfo = g_ucaLng_ErrCodeName_BC_Current[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_EEPROM:             //EEPROM异常  
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_BC_Current[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_CAT_ONE:            //CAT1模块异常  
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_4G_Cat1[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_HGB_MODULE:                //HGB模块异常  
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_HGB[MachInfo.systemSet.eLanguage]; 
        }
        break; 
        case ERROR_CODE_SPI_FLASH_CONN_ERR: //spi flash 连接异常
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_Spi_Flash[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_FLASH_DATA_CALIBRATE_ERR:   //SPI Flash数据读写校验失败
        {
            
        }
        break;
        case ERROR_CODE_CELL_CHECK_MODULE:  //对射检测模块异常
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_Cell_Check[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_RESULT_ASTERISK:
        {
            //
        }
        break;
        case ERROR_CODE_DISK_CAPACITY:  //磁盘容量
        {
            pErrorCodeInfo = g_ucaLng_ErrCodeName_Disk[MachInfo.systemSet.eLanguage];
        }
        break;
        case ERROR_CODE_T4_ERR_15S_2_MAXTIME: ////T4时间异常，（大于15s, 但是小于最大计数时间）
        {
        
        }
        break;
        case ERROR_CODE_RE_CONN_2_T4_15S:
        {
        
        }
        break;
        case ERROR_CODE_BSK_2S_15V:             //第一次开阀2后2s内，小孔电压小于1.5V
        {

        
        }
        break;
        case ERROR_CODE_BSK_XKV_TOO_LOW:        //小孔电压过低
        {
        
        }
        break;
        //
        case ERROR_CODE_SAMPLE_NOT_ENOUGH:    //待测样本量不足 
        {
       
        }
        break;
        //
        default: break;
    }
    //
    return pErrorCodeInfo;
}





/***
*将桌面各按键禁止掉
***/
void AbortPublishDestopButton(void)
{
	WM_DisableWindow(WM_GetDialogItem(WM_HBKWIN,ID_PUBLIC_BUTTON_MENU));
	WM_DisableWindow(WM_GetDialogItem(WM_HBKWIN,ID_PUBLIC_BUTTON_ANALYSIS));
	WM_DisableWindow(WM_GetDialogItem(WM_HBKWIN,ID_PUBLIC_BUTTON_LIST_REVIEW));
	WM_DisableWindow(WM_GetDialogItem(WM_HBKWIN,ID_PUBLIC_BUTTON_ERROR_PROMPT));
}


/***
*将桌面各按键重新恢复
***/
void RecoverPublishDestopButton(void)
{
	WM_EnableWindow(WM_GetDialogItem(WM_HBKWIN,ID_PUBLIC_BUTTON_MENU));
	WM_EnableWindow(WM_GetDialogItem(WM_HBKWIN,ID_PUBLIC_BUTTON_ANALYSIS));
	WM_EnableWindow(WM_GetDialogItem(WM_HBKWIN,ID_PUBLIC_BUTTON_LIST_REVIEW));
	WM_EnableWindow(WM_GetDialogItem(WM_HBKWIN,ID_PUBLIC_BUTTON_ERROR_PROMPT));
}




/*
*   获取错误码的错误类型，返回指示灯的显示状态
*/
Panel_LED_Status_e Panel_LED_Status_By_ErrorCode(ErrorCode_e eErrorCode)
{
    Panel_LED_Status_e eStatus =  PANEL_LED_STATUS_NORMAL;   
    switch(eErrorCode)
    {
        case ERROR_CODE_SUCCESS:
        {
            //正常
            eStatus = PANEL_LED_STATUS_NORMAL;
        }
        break;
        
        case ERROR_CODE_NOT_AT_POS:                 //未进仓导致计数失败
//        case ERROR_CODE_ADD_PRESS:                //压力补偿值不一致
//        case ERROR_CODE_ADD_MOTOR_STEP:           //电机补偿值不一致
//        case ERROR_CODE_ADD_BC_GAIN:              //bc增益补偿值不一致
//        case ERROR_CODE_PUMP:                     //泵占空比值不一致
//      case ERROR_CODE_TANZHEN_SIGNAL:             //探针接触异常
        case ERROR_CODE_NO_PRINTER:                 //打印机未就绪
        case ERROR_CODE_MONITOR_SIGNAL_ERR:         //计数电极接触异常（信号稳定度检测阶段）
        case ERROR_CODE_ALGO_MODE:                  //算法判断设置模式异常
        case ERROR_CODE_ALGO_BREAK:                 //算法退出运算
        case ERROR_CODE_TEMPERATURE:                //温度异常  
        case ERROR_CODE_CAT_ONE:                    //CAT1模块异常            
        case ERROR_CODE_INVALID_FILENUM:            //质控，无效的文件号
        case ERROR_CODE_FILENUM_CAPACITY_IS_FULL:   //质控，文件号下存储数据已满
        case ERROR_CODE_PRINT_NO_PAPER:             //打印机缺纸
        case ERROR_CODE_SAMPLE_NOT_ENOUGH:          //待测样本量不足 
        case ERROR_CODE_NONE_CELL:                  //计数未放入血细胞检测模块
        case ERROR_CODE_SAMPLE_NONE:                // 未放入测样本量
//        case ERROR_CODE_TIMEOUT_10_BEFORE:        //超时,前10秒
//        case ERROR_CODE_TIMEOUT_10_AFTER:         //超时,后10秒
        case ERROR_CODE_T4_ERR_15S_2_MAXTIME:       //T4时间异常，（大于15s, 但是小于最大计数时间）
        case ERROR_CODE_RE_CONN_2_T4_15S:           //液路建立阶段重对接次数>=2, 且T4>15s 
        case ERROR_CODE_TIMEOUT:                    //超时
        case ERROR_CODE_AIR_LEAK:                   //气嘴漏气
        case ERROR_CODE_WBC_ELEC_TOUCH:             //采样电极接触异常
        case ERROR_CODE_AIR_LEAK_COUNTING_1:        //计数采样中，漏气。压力低于-25kpa,最多重复建压3次，且3次内定量电极正常触发
        case ERROR_CODE_AIR_LEAK_COUNTING_2:        //计数采样中，漏气。压力低于-25kpa，最多重复建压3次，且3次内定量电极没有正常触发
        case ERROR_CODE_ELEC_TRIGGLE:               //定量电极异常触发
        case ERROR_CODE_ELEC_TRIGGLE_2:             //定量电极异常触发
        case ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE:   //初始化阶段，小孔电压导通 
        case ERROR_CODE_BSK_2S_15V:                 //第一次开阀2后2s内，小孔电压小于1.5V
        case ERROR_CODE_BSK_XKV_TOO_LOW:            //小孔电压过低
        case ERROR_CODE_RESULT_ASTERISK:
        {
            //告警状态
            eStatus =  PANEL_LED_STATUS_WARN;
        }
        break; 
        
        case ERROR_CODE_CELL_CHECK_MODULE:  //对射检测模块异常
        case ERROR_CODE_HGB_MODULE:         //HGB模块异常
        case ERROR_CODE_OUTIN_OUT:          //进出仓电机，出仓异常
        case ERROR_CODE_OUTIN_OC:           //进出模块，光耦异常
        case ERROR_CODE_OUTIN_IN:           //进出仓电机，进仓异常
        case ERROR_CODE_CLAMP_IN:           //夹子电机，进仓异常  
        case ERROR_CODE_CLAMP_OUT:          //夹子电机，出仓异常
        case ERROR_CODE_CLAMP_OC:           //对接模块，光耦异常 
        case ERROR_CODE_BUILD_PRESS:        //建立负压异常
        case ERROR_CODE_AIR_TIGHT:          //系统气密性异常
        case ERROR_CODE_BC_CURRENT:         //恒流源异常
        case ERROR_CODE_POWER_12VP:         //正12V, 供电异常
        case ERROR_CODE_POWER_12VN:         //负12V, 供电异常
        case ERROR_CODE_FAILURE:
        case ERROR_CODE_EEPROM:             //EEPROM异常  
        case ERROR_CODE_SPI_FLASH_CONN_ERR: //spi flash 异常
        case ERROR_CODE_FLASH_DATA_CALIBRATE_ERR: //SPI Flash数据读写校验失败
        case ERROR_CODE_DISK_CAPACITY:      //磁盘容量不足
        {
            //出错状态
            eStatus =  PANEL_LED_STATUS_ERROR;
        
        }
        break;
        default:
           //正常
            eStatus =  PANEL_LED_STATUS_NORMAL;   
        break;
    }

    return eStatus;
}





