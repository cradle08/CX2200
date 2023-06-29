#ifndef __MSG_DEF_H__
#define __MSG_DEF_H__


#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "main.h"
#include "serviceItems.h"
#include "CAT_iotTask.h"
#include "bsp_outin.h"


/*
***************  analysis cmd  **************************
*/
#define CMD_ANALYSIS                   	 0x1
#define CMD_ANALYSIS_LAST_RECORD         0x1000
#define CMD_ANALYSIS_NEXT_RECORD         0x1001
#define CMD_ANALYSIS_NEXT_SAMPLE       	 0x1002
#define CMD_ANALYSIS_LAST_PAGE           0x1003
#define CMD_ANALYSIS_NEXT_PACE           0x1004
#define CMD_ANALYSIS_PRINT          	 0x1005
#define CMD_ANALYSIS_UPDATE_PATIENT_INFO 0x1006 //更新、保存修改后的病人信息
//
#define CMD_ANALYSIS_COUNT_MODE_AUTO     0x1019		//计数：外部工装信号仿真模拟测试,纯信号,自动执行模式
#define CMD_ANALYSIS_COUNT				 0x1020		//计数
#define CMD_ANALYSIS_MOTOR_OUTIN	     0x1030		//进出仓电机
#define CMD_ANALYSIS_MOTOR_CALAMP  		 0x1031		//夹子电机
//
#define CMD_ANALYSIS_PROMPT_COUNTING     0x1040		//UI提示，样本分析正在进行中，请稍等...

#define CMD_ANALYSIS_LOAT_DATA_FAIL      0x1050     //样本分析加载数据失败

#define CMD_ANALYSIS_INPUT_USER_NUM      0x1060     //弹出输入患者编号界面
#define CMD_ANALYSIS_REQUEST_USER_NUM      0x1061   //向服务端请求指定患者编号的患者信息中

/*
***************  list review  **************************
*/
#define CMD_LIST                        0x3
#define CMD_LIST_REVIEW_INIT            0x3000  //初始化，列表回顾显示数据
#define CMD_LIST_REVIEW_MENU_OUT        0x3001  //当退出列表回顾时，需更新当前数据
#define CMD_LIST_REVIEW_SYNC            0x3002  //当计数后，生成的新的数据同步到列表回顾数据

#define CMD_LIST_REVIEW_EDIT            0x3010  //编辑
#define CMD_LIST_REVIEW_DELETE          0x3011  //删除
#define CMD_LIST_REVIEW_PRINT           0x3012  //打印
#define CMD_LIST_REVIEW_LAST_RECORD     0x3013  //上一条
#define CMD_LIST_REVIEW_NEXT_RECORD     0x3014  //下一条
#define CMD_LIST_REVIEW_LAST_PAGE       0x3015  //上一页
#define CMD_LIST_REVIEW_NEXT_PAGE       0x3016  //下一页

//
#define CMD_LIST_REVIEW_LOAT_DATA_FAIL  0x3020     //列表回顾加载数据失败

/*
***************  set  cmd **************************
*/
#define CMD_SET                         0x5
#define CMD_SET_SYSTEM                  0x5000
#define CMD_SET_USER_MANAGE             0x5100
#define CMD_SET_ASSIST                  0x5200
#define CMD_SET_PRINT                   0x5300
#define CMD_SET_LAB_INFO                0x5400
#define CMD_SET_PARAMETER               0x5500
#define CMD_SET_CALIBRATION             0x5600  //WBC HGB校准
#define CMD_SET_DEBUG                   0x5700  //设置->调试,通信命令
#define CMD_SET_SN_WAIT_CAT				0x5800	//设置了SN后等待服务端响应
#define CMD_CHANGE_SN_WAIT_CAT			0x5900	//修改SN后等待服务端响应
#define CMD_SET_SWITCH_LOGO             0x5920  //切换LOGO，切换不同的开关机和登录图片
#define CMD_SET_BUILD_PRESSURE          0x5930  //建压到-30Kpa
#define CMD_SET_UPDATE_LOGIN_PICTURE    0x5940  //系统设置中，当切换语言后，更新加载登录界面图片到SDRAM中
#define CMD_SET_U_UPDATE			    0x5950  //U盘升级
#define CMD_LIST_REVIEW_REFRESH_NET     0x5960  //手动刷新网络

/*
***************  service cmd **************************
*/
#define CMD_SERVER                                  0x7
#define CMD_SERVER_PRESS                            0x7000
#define CMD_SERVER_AIR_LIGHT                        0x7100
#define CMD_SERVER_MOTOR                            0x7200
#define CMD_SERVER_STATUS                           0x7300

#define CMD_SERVER_INTERACTION                      0x7400
#define CMD_SERVER_INTERACTION_PRINTER	            0x7401
#define CMD_SERVER_INTERACTION_SCREEN_BOARD         0x7402

#define CMD_SERVER_OTHERS                           0x7500
#define CMD_SERVER_COMPONENT_AGING                  0x7600
#define CMD_SERVER_LOG                              0x7700
#define CMD_SERVER_VERSION                          0x7800
#define CMD_SERVER_DISK                             0x7900

#define CMD_SERVER_DATA_MANAGE                      0x7A00
#define CMD_SERVER_DEL_TEST_DATA        			0x7A01				//删除测试数据
#define CMD_SERVER_DEL_USER_DATA        			0x7A02				//删除用户数据
#define CMD_SERVER_RECOVER_FACTORY        			0x7A03				//恢复出厂设置
#define CMD_SERVER_CAT_RESTART	        			0x7A04				//重启CAT

#define CMD_SERVER_AGING		                    0x7B00
#define CMD_SERVER_TANZHEN_TEST         			0x7B01				//探针老化测试
#define CMD_SERVER_AGING_TEST         				0x7B02				//流程老化测试
#define CMD_SERVER_OUTIN_MODULE_OUT_IN  			0x7B03  			//进出仓模块，往复运动
#define CMD_SERVER_QIZUI_TEST			 			0x7B04  			//气嘴老化测试
#define CMD_SERVER_AIR_VALVE_TEST			 		0x7B05  			//气阀老化测试
#define CMD_SERVER_YEFA_VALVE_TEST			 		0x7B06  			//液阀老化测试
#define CMD_SERVER_BUILD_PRESSURE_TEST			 	0x7B07  			//建压老化测试
#define CMD_SERVER_DL_ELEC_TEST			 			0x7B08  			//定量电极老化测试
#define CMD_SERVER_CAT_DATA_TEST			 		0x7B09  			//CAT数据测试
#define CMD_SERVER_RAND_TOUCH_TEST			 		0x7B0a  			//随机点击测试
#define CMD_SERVER_SHENGCHAN_AGING			 		0x7B0b  			//生产老化测试
#define CMD_SERVER_DATA_SAVE_TEST			 		0x7B0c  			//数据存储测试

#define CMD_SERVER_OTHER_CHECK                      0x7C00				//其他自检项
#define CMD_SERVER_OTHER_CHECK_PUMP        			0x7C01				//泵自检项
#define CMD_SERVER_OTHER_CHECK_BEEP        			0x7C02				//蜂鸣器自检项
#define CMD_SERVER_OTHER_CHECK_AIR_VALVE   			0x7C03				//气阀自检项
#define CMD_SERVER_OTHER_CHECK_YE_VALVE    			0x7C04				//液阀自检项
#define CMD_SERVER_OTHER_CHECK_HGB        			0x7C05				//HGB自检项

#define CMD_SERVER_PCBA_CHECK                      	0x7D00				//PCBA检验项
#define CMD_SERVER_PCBA_CHECK_MAIN                  0x7D01				//主控板检验项
#define CMD_SERVER_PCBA_CHECK_ZJ                    0x7D02				//转接板检验项
#define CMD_SERVER_PCBA_CHECK_CAT                   0x7D03				//CAT检验项
#define CMD_SERVER_PCBA_CHECK_TOUCH                 0x7D04				//触摸屏检验项
#define CMD_SERVER_PCBA_CHECK_MOTO                  0x7D05				//电机检验项
#define CMD_SERVER_PCBA_CHECK_PRESURE               0x7D06				//压力传感器检验项
#define CMD_SERVER_PCBA_CHECK_VALVE		            0x7D07				//阀检验项
#define CMD_SERVER_PCBA_CHECK_PUMP		            0x7D08				//泵检验项
#define CMD_SERVER_PCBA_CHECK_HONE		            0x7D09				//HONEYWELL检验项
#define CMD_SERVER_PCBA_CHECK_IT110		            0x7D0A				//IT110检验项
#define CMD_SERVER_PCBA_PRESSURE_ADJUST	            0x7D0B				//PCBA压力校准


//
#define CMD_SET_HGB_BLACK_AUTO_ADJUST              	0x7E01				//HGB空白增益自动校准
#define CMD_SET_HGB_GZ_AUTO_ADJUST              	0x7E02				//HGB工装增益自动校准
#define CMD_SET_HGB_TEST                      	    0x7E03				//HGB测试
#define CMD_SET_HGB_ADJUST_TEST                     0x7E04				//HGB调校界面HGB测试
#define CMD_SET_HGB_ADJUST_OPEN415                  0x7E05				//HGB调校界面开415
#define CMD_SET_HGB_ADJUST_OPEN535                  0x7E06				//HGB调校界面开535
#define CMD_SET_HGB_ADJUST_CLOSE                  	0x7E07				//HGB调校界面关灯
#define CMD_SET_HGB_ADJUST_KQ_AUTO                  0x7E08				//HGB调校界面空气自动校准
#define CMD_SET_HGB_ADJUST_KB_AUTO                  0x7E09				//HGB调校界面空白自动校准

/*
***************  login cmd **************************
*/
#define CMD_LOGIN                                   0x9   
#define CMD_LOGIN_SELFCHECK                         0x9000   //登录自检
#define CMD_LOGIN_ERROR_CLEAR                       0x9001   //登录自检后，故障清除
#define CMD_LOGIN_OK_LOAD_USER_DATA                 0x9005   //账号登录校验完成后，发送消息给后端加载用户数据
#define CMD_LOGIN_LOAD_USER_DATA_FINISH             0x9006   //账号登录校验完成后，加载用户数据

/*
***************  poweroff cmd **************************
*/
#define CMD_POWEROFF                                0xA   
#define CMD_POWEROFF_EXEC                           0xA001   //关机


/*
***************  other cmd **************************
*/
//打印机
#define CMD_OTHERS_PRITER_NO_PAPER                  0xC020   //打印机缺纸
#define CMD_OTHERS_PRITER_CONN_ERR                  0xC021   //打印连接错误

#define CMD_OTHERS                                  0xC
#define CMD_OTHERS_OUTIN_MODULE_OUT                 0xC010   //进出仓模块，出仓
#define CMD_OTHERS_OUTIN_MODULE_IN                  0xC011   //进出仓模块，进仓
//进出仓
#define CMD_OTHERS_OUTIN_IN			                0xC030   //进仓
#define CMD_OTHERS_OUTIN_OUT			            0xC031   //出仓
#define CMD_OTHERS_START_COUNT			            0xC032   //启动计数






/*
***************  QC cmd **************************
*/
#define CMD_QC                                      0xD
#define CMD_QC_ANALYSIS_COUNT			            0xD021  //质控计数
#define CMD_QC_ANALYSIS_COUNT_MODE_AUTO             0xD022  //质控计数：纯信号,自动执行模式
#define CMD_QC_UPDATE_ANALYSIS_DATA                 0xD023  //质控分析，更新当前显示的列表数据
//
#define CMD_QC_LIST_LAST_RECORD                     0xD002  //质控列表、上一记录
#define CMD_QC_LIST_NEXT_RECORD                     0xD003  //质控列表、下一记录
#define CMD_QC_LIST_DELETE                          0xD004  //质控列表、删除
#define CMD_QC_UPDATE_LIST_DATA                     0xD005  //质控，更新当前显示的列表数据
//
#define CMD_QC_PRINT          	                    0xD008  //质控分析、打印
#define CMD_QC_SET_RE_READ_QC_INFO         	        0xD009  //质控设定，重新读取，质控设定参数
#define CMD_QC_SET_SAVE         	                0xD010  //质控设定，保存质控设定参数
#define CMD_QC_SET_DELETE         	                0xD011  //质控设定，删除指定的质控设定参数
//
#define CMD_QC_LOAD_ANALYSIS_LIST_DATA              0xD015  //质控，加载质控样本分析、列表数据
//#define CMD_QC_VALID_FILE_NUM           	        0xD016  //质控，不存在有效的文件号
//#define CMD_QC_FILENUM_CAPACITY_IS_FULL    	        0xD017  //质控，文件号对于的数据容量已满，

// 大小长度定义
#define UI_TO_BACKEND_MSG_BUFFER_LEN  	256		//UI发送给后端消息大小
#define BACKEND_TO_UI_MSG_BUFFER_LEN  	WBCHGB_TEST_INFO_LEN	//UI发送给后端消息大小
#define GRAPH_DATA_LEN  				256		//直方图数据大小
#define DATE_TIME_LEN	  				25		//日期时间长度
#define SAMPLE_SN_LEN	  				10		//测试样本编号
#define PATIENT_NAME_LEN	  		    22		//病人姓名长度
#define PATIENT_BIRTHE_DAY_MAX_LEN	  	20		//病人出生年月
#define REMARK_LEN	  					91		//备注信息长度
//#define DIAGNOSIS_INFO_LEN	  		50		//诊断信息长度
#define WBC_PROMPT_LEN	  				161		//WBC提示信息长度
#define HGB_PROMPT_LEN	  				97		//HGB提示信息长度
#define SERIAL_NUM_LEN	  				10		//系列号转为数组长度
#define QC_LOT_NUM_LEN                  15      //质控，批号长度


/*
*   故障码
*/
typedef enum {
    ERROR_CODE_SUCCESS                  = 0000,    //成功
    ERROR_CODE_FAILURE                  = 0001,    //失败
    
    ERROR_CODE_BUILD_PRESS              = 1011,    //建立负压异常
    ERROR_CODE_AIR_TIGHT                = 1021,    //系统气密性异常
//  ERROR_CODE_PRESS_TOO_LOW            = 1025,    //缓冲室压力过低(<-22kpa)
//  ERROR_CODE_PRESS_TOO_HIGH           = 1026,    //缓冲室压力过高（>-33kpa） 
    ERROR_CODE_BC_CURRENT               = 1031,    //恒流源异常
    ERROR_CODE_POWER_12VP               = 1032,    //正12V, 供电异常
    ERROR_CODE_POWER_12VN               = 1033,    //负12V, 供电异常
    ERROR_CODE_OUTIN_OUT                = 1041,    //进出仓电机，出仓异常
    ERROR_CODE_OUTIN_IN                 = 1051,    //进出仓电机，进仓异常
    ERROR_CODE_OUTIN_OC                 = 1055,    //进出模块，光耦异常
    //
    ERROR_CODE_CLAMP_OUT                = 1061,    //对接电机，对接异常
    ERROR_CODE_CLAMP_IN                 = 1071,    //对接电机，松开异常  
    ERROR_CODE_CLAMP_OC                 = 1075,    //对接模块，光耦异常 
    //
    ERROR_CODE_NOT_AT_POS               = 1081,    //未进仓导致计数失败
    ERROR_CODE_ELEC_TRIGGLE             = 1091,    //初始化阶段，定量电极异常触发
    ERROR_CODE_ELEC_TRIGGLE_2           = 1092,    //液路建立、信号稳定检测、信号采集、结束等后续阶段，定量电极异常触发
//  ERROR_CODE_ELEC_TRIGGLE_M1          = 1093,    //定量电极异常触发，稳定度异常次数1
//  ERROR_CODE_ELEC_TRIGGLE_M2          = 1094,    //定量电极异常触发，稳定度异常次数2
//  ERROR_CODE_ELEC_TRIGGLE_M3          = 1095,    //定量电极异常触发，稳定度异常次数3
//  ERROR_CODE_ELEC_TRIGGLE_M4          = 1096,    //定量电极异常触发，稳定度异常次数4
//  ERROR_CODE_ELEC_TRIGGLE_M5          = 1097,    //定量电极异常触发，稳定度异常次数5

    /*** usr operator err type ***/
    ERROR_CODE_WBC_ELEC_TOUCH           = 1101,    //采样电极接触异常   
    //
    ERROR_CODE_XKV_ON_BEFORE_OPEN_WALVE = 1111,    //初始化阶段，小孔电压导通
    ERROR_CODE_BSK_2S_15V               = 1112,    //液路建立阶段，小孔电压小于1.5V
    ERROR_CODE_BSK_XKV_TOO_LOW          = 1113,    //小孔电压小于0.60V
    //
//  ERROR_CODE_BSK                      = 1114,    //数据采集结束关闭阀2前，小孔电压小于0.65V
//  ERROR_CODE_BSK                      = 1115,    //液路建立阶段，小孔电压低于0.79v                                           
    ERROR_CODE_AIR_LEAK                 = 1121,    //气嘴漏气
    //
    ERROR_CODE_SAMPLE_NONE              = 1123,    //未放入测样本量 
    ERROR_CODE_AIR_LEAK_COUNTING_1      = 1124,    //计数采样中，漏气。压力低于-25kpa,最多重复建压3次，且3次内定量电极正常触发
    ERROR_CODE_AIR_LEAK_COUNTING_2      = 1125,    //计数采样中，漏气。压力低于-25kpa，最多重复建压3次，且3次内定量电极没有正常触发  
    
    /*** material err type ***/
//    ERROR_CODE_TIMEOUT_10_BEFORE        = 1131,    //超时,前10秒（前十秒发生，堵孔）
//    ERROR_CODE_TIMEOUT_10_AFTER         = 1132,    //超时,后10秒（后十秒发生，堵孔）
    ERROR_CODE_TIMEOUT                  = 1131,      //超时（超过最大时间，未发生堵孔）
    ERROR_CODE_T4_ERR_15S_2_MAXTIME     = 1132,      //T4时间异常，（大于15s, 但是小于最大计数时间）
    ERROR_CODE_RE_CONN_2_T4_15S         = 1133,      //液路建立阶段重对接次数>=2, 且T4>15s 
        
    /*** usr operator err type ***/
    ERROR_CODE_SAMPLE_NOT_ENOUGH        = 1141,    //待测样本量不足 , 
    ERROR_CODE_NONE_CELL                = 1151,    //未放入血细胞检测模块
    /*** Machine error type ***/
    ERROR_CODE_TEMPERATURE              = 1161,    //温度异常  
    ERROR_CODE_EEPROM                   = 1171,    //EEPROM异常  
    ERROR_CODE_CAT_ONE                  = 1181,    //CAT1模块异常  
    ERROR_CODE_HGB_MODULE               = 1191,    //HGB模块异常
    ERROR_CODE_DISK_CAPACITY            = 1201,    //磁盘容量不足
    ERROR_CODE_SPI_FLASH_CONN_ERR       = 1202,    //SPI Flash通信异常
    ERROR_CODE_FLASH_DATA_CALIBRATE_ERR = 1203,    //SPI Flash数据读写校验失败
    ERROR_CODE_CELL_CHECK_MODULE        = 1205,    //对射检测模块异常  
    ERROR_CODE_RESULT_ASTERISK          = 1231,    //结果报星号     
    
    /*** Machine error type ***/
//    ERROR_CODE_ADD_PRESS                = 2041,    //压力补偿值不一致
//    ERROR_CODE_ADD_MOTOR_STEP           = 2051,    //电机补偿值不一致
//    ERROR_CODE_ADD_BC_GAIN              = 2061,    //bc增益补偿值不一致
//    ERROR_CODE_PUMP                     = 2101,    //泵占空比值不一致
    ERROR_CODE_PRINT_NO_PAPER           = 2071,    //打印机缺纸
//  ERROR_CODE_TANZHEN_SIGNAL           = 2121,    //探针接触异常
    ERROR_CODE_NO_PRINTER               = 2081,    //打印机未就绪
    ERROR_CODE_MONITOR_SIGNAL_ERR       = 2091,    //计数电极接触异常（信号稳定度检测阶段）	
    //Algo    
    ERROR_CODE_ALGO_MODE                = 3000,    //算法判断设置模式异常
    ERROR_CODE_ALGO_BREAK               = 3001,    //算法退出运算
    //QC Errorcode
    ERROR_CODE_INVALID_FILENUM          = 4000,     //质控，无效的文件号
    ERROR_CODE_FILENUM_CAPACITY_IS_FULL = 4001,     //质控，文件号下存储数据已满
    
    
    //这些错误码只用于，上/下一记录状态的返回
//    ERROR_CODE_NO_LAST_RECORD      					= 9001,   //无上一条数据，用于样本分析查看上一条数据且已是第一条（不存在上一条数据）
//    ERROR_CODE_NO_NEXT_RECORD      					= 9002,   //无下一条数据，用于样本分析查看下一条数据且已是第一条（不存在下一条数据）
} ErrorCode_e;



/*
*   错误码类型（故障类型）
*/
typedef enum {
    ERROR_CODE_TYPE_NORMAL              = 0,    //无错误，正常
    ERROR_CODE_TYPE_USER 	            = 1,    //用户操作故障
	ERROR_CODE_TYPE_MACHINE             = 2,	//仪器故障
	ERROR_CODE_TYPE_MATERIAL            = 3,	//耗材故障
    ERROR_CODE_TYPE_MULTI               = 4,    //综合故障
    
//	ERROR_CODE_TYPE_USER_MACHINE		= 4,    //用户操作故障 或 仪器故障
//    ERROR_CODE_TYPE_USER_MATERIAL       = 5,    //用户操作故障 或 耗材故障
//    ERROR_CODE_TYPE_MACHIHNE_MATERIAL   = 6,    //仪器故障 或 耗材故障
    ERROR_CODE_TYPE_END                 = 5,
} ErrorType_e;







/*
* 	消息发送方向
*/
typedef enum {
	INNERMSMG_DIR_UI_TO_BACKEND = 0,
	INNERMSMG_DIR_BACKEND_TO_UI = 0,
	
} InnerMsg_Dir_e;




/********************* 样本分析 ***************************************************/

/*
* 	测试模式
*/
typedef enum {
	TEST_MODE_WBC_HGB = 0,
	TEST_MODE_WBC_HGB_RBC_PLT = 1,
	
} TestMode_e;


/***
*定义当前使用仪器的公司代号枚举，ODM公司有专有的UI和皮肤色对应，否则统一称为预留
***/
typedef enum _Company_e{
	COMPANY_CREATE_CARE = 0,			//创怀
	COMPANY_JIN_RUI,					//锦瑞
	COMPANY_YSB,						//药师帮
	COMPANY_HY,                    		//和映
	COMPANY_YU_LIU4,                    //预留4
	COMPANY_YU_LIU5,                    //预留5
	COMPANY_YU_LIU6,                    //预留6
	COMPANY_YU_LIU7,                    //预留7
	
	COMPANY_END
}Company_e;


/*
* 	性别
*/
typedef enum {
    SEX_TYPE_NONE       = 0,        //空
	SEX_TYPE_MALE		= 1,		//男
	SEX_TYPE_FEMALE		= 2,		//女
	SEX_TYPE_END		= 3,
    
} SexType_e;

typedef enum {
	REFER_GROUP_CMOMMON		= 0,	//通用
	REFER_GROUP_MALE		= 1,	//成年男士
	REFER_GROUP_FEMALE		= 2,	//成年女士
	REFER_GROUP_CHILDREN	= 3,	//儿童
	REFER_GROUP_BOBY		= 4,	//新生儿
    REF_GROUPS_END          = 5,
} ReferGroup_e;








/*
*	WBC HGB 结果项
*/
typedef enum {
	WBCHGB_RST_WBC		= 0,
	WBCHGB_RST_GranJ	= 1,
	WBCHGB_RST_MidJ		= 2,
	WBCHGB_RST_LymJ		= 3,
	WBCHGB_RST_GranB	= 4,
	WBCHGB_RST_MidB		= 5,
	WBCHGB_RST_LymB		= 6,
	WBCHGB_RST_HGB		= 7,
	WBCHGB_RST_END		= 8,
	
} WBCHGB_Rst_e;


/*
*	 RBC 结果项
*/
typedef enum {
	RBC_RST_RBC		= 0,
	RBC_RST_HCT		= 1,
	RBC_RST_MCV		= 2,
	RBC_RST_MCH		= 3,
	RBC_RST_MCHC	= 4,
	RBC_RST_RDW_CV	= 5,
	RBC_RST_RDW_SD	= 6,
	RBC_RST_END		= 7,
    
} RBC_Rst_e;



/*
*	 PLT 结果项
*/
typedef enum {
	PLT_RST_PLT		= 0,
	PLT_RST_MPV		= 1,
	PLT_RST_PDW		= 2,
	PLT_RST_PCT		= 3,
	PLT_RST_P_LCR	= 4,
	PLT_RST_P_LCC	= 5,
	PLT_RST_END		= 6,
    
} PLT_Rst_e;


/*
	年龄 单位
*/
typedef enum {
	AGE_UNIT_YEAR	= 0,	//岁
	AGE_UNIT_MONTH	= 1,	//月
	AGE_UNIT_WEEK	= 2,	//周
	AGE_UNIT_DAY	= 3,	//天
	AGE_UNIT_END	= 4,
    
} AgeUnit_e;



/*
*   质控，文件号编号索引，0-5（当前最多数据输入6个文件编号）
*/
typedef enum {
    QC_FILENUM_INDEX_0      = 0,
    QC_FILENUM_INDEX_1      = 1,
    QC_FILENUM_INDEX_2      = 2,
    QC_FILENUM_INDEX_3      = 3,
    QC_FILENUM_INDEX_4      = 4,
    QC_FILENUM_INDEX_5      = 5,
    QC_FILENUM_INDEX_END    = 6,   
    
} QC_FILENUM_INDEX_e;


/*
*   质控品，水平/浓度（高、中、低）
*/
typedef enum {
    QC_LEVEL_LOW    = 0,
    QC_LEVEL_MIDDLE = 1,
    QC_LEVEL_HIGH   = 2,
    QC_LEVEL_END    = 3,
    
} QC_LEVEL_e;






/*
*	内部消息头
*/
typedef __packed struct {
  //InnerMsg_Dir_e   ucDir;         //消息方向，0：UI发到后端，1：后端到UI
	uint16_t  		usCmd;          //命令，
	ErrorCode_e  	eErrorCode;	    //消息异常状态
	ErrorType_e		errType;		//异常类型
	uint16_t  		usMsgLen;       //消息长度，usMsgLen字段之后的所有消息字段长度之和
    
}Msg_Head_t;
#define MSG_HEAD_LEN	sizeof(Msg_Head_t)



/*
*********************  analysis msg define  ********************************
*/

/*
*	上一记录
*/
typedef  __packed struct{
	Msg_Head_t 	tMsgHead;						        //消息头
	uint32_t			ulCurDataIndex;			        //当前测试结果系列号，
    QC_FILENUM_INDEX_e  eFileNumIndex;                  //用于质控列表，当前的文件号
//	TestMode_e 			eTestMode;						//当前测试模式 WBC+HGB
		
} LastRecord_t, NextRecord_t, QC_Record_t;


///*
//*	下一记录
//*/
//typedef  __packed struct {
//	Msg_Head_t 	tMsgHead;						        //消息头
//	uint32_t			ulCurDataIndex;			        //当前测试结果系列号
//    QC_FILENUM_INDEX_e  eFileNumIndex;                  //用于质控列表，当前的文件号
////	TestMode_e 			eTestMode;						//当前测试模式 WBC+HGB

//} NextRecord_t;




/*
*	打印
*/
typedef __packed struct{
	Msg_Head_t		tMsgHead;						//消息头
	uint32_t		ulCurDataIndex;					//当前测试结果系列号，自动递增,用于数据管理
    QC_FILENUM_INDEX_e  eFileNumIndex;               //用于质控列表，当前的文件号
} Print_t;



#define PATIENT_ID_MAX_LEN			20				//患者编号字符串最大长度，这里包括'\0'
#define ORDER_NUM_MAX_LEN			20				//订单编号字符串最大长度，这里包括'\0'


/*
*	下一样本测试, 信息结构体，定位结果信息位置
*/
typedef  __packed struct{
	uint8_t 			validFlag;									//下一个样本信息是否有效，为0表示无效，为1表示有效
	uint32_t			ulNextDataSN;								//下一样本系列号，在当前系列号上曾一
	uint8_t 			ucaNextSampleSN[SAMPLE_SN_LEN];				//下一样本编号,在当前编号上增一,"CBC-0000N"
	char 				patientID[PATIENT_ID_MAX_LEN];				//患者编号字符串
	uint8_t 			ucaName[PATIENT_NAME_LEN];					//姓名
	SexType_e 			eSex;										//性别
	uint8_t 			ucAge;										//年龄
	AgeUnit_e			eAgeUnit;									//年龄单位
	uint8_t				ucaBirthDay[PATIENT_BIRTHE_DAY_MAX_LEN];	//出生年月
	ReferGroup_e  		eReferGroup;								//参考组		
	TestMode_e 			eTestMode;									//当前测试模式 WBC+HGB
	uint8_t		  		ucaRemark[REMARK_LEN];						//备注
	char 				orderNum[ORDER_NUM_MAX_LEN];				//订单编号
} NextSample_t;








// WBC HGB 测试结果信息,状态
typedef enum {
	DATA_STATUS_INIT		        = 0,    //初始化后，填充WBCHGB_TestInfo, taWBCHGB_Data的默认数据状态，不是有效的数据（不显示到UI）
    DATA_STATUS_NORMAL		        = 1,    //正常状态（显示到UI）
	DATA_STATUS_DELETE		        = 2,	//已删除，数据已删除（不显示到UI）
    DATA_STATUS_COUNT_INVALID       = 3,    //计数无效数据，1、计数报错下，获得的数据（显示到UI）
	DATA_STATUS_CHANGE				= 5,	//表示数据修改过
	DATA_STATUS_END			        = 6,
	
} DataStatus_e;


//WBC HGB参数报警标志， 正常/可疑值才会进行偏大/偏小判断，无效值报*
typedef enum{
    WBCHGB_ALERT_NORMAL         = 0x00,  //正常
    WBCHGB_ALERT_SMALL          = 0x01,  //偏小↓
    WBCHGB_ALERT_BIG            = 0x02,  //偏大↑
    WBCHGB_ALERT_REVIEW         = 0x10,  //可疑值 ?
    WBCHGB_ALERT_REVIEW_SMALL   = 0x11,  //可疑值 ?，且偏小↓
    WBCHGB_ALERT_REVIEW_BIG     = 0x12,  //可疑值 ?，且偏大↑

    WBCHGB_ALERT_INVALID        = 0xFF,  //无效值，报* 
    WBCHGB_ALERT_END            = 0,

}WBCHGB_ALERT_e;






/*
***************  list review msg define  **************************
*/

#define LISTVIEW_MAX_ITEM_NUM			7

/*
*   列表回顾界面，交互消息结构体
*/
typedef struct {
    Msg_Head_t  tMsgHead;
    uint8_t     ucIndex;     //ListReview_DataTable对应的下标
    uint32_t    ulSN;        //列表回顾中，操作的数据对于的SN号

}ListReview_t;





/*********************** 设置界面 **************************************/



/*
*	HGB 自动校准信息结构体
*/
typedef __packed struct {
     Msg_Head_t tMsgHead;			//消息头
     uint8_t  ucHGB_DR;             //自动校准后。HGB增益值
     uint16_t current535;     //自动校准后，HGB驱动电流
     uint16_t usHGB_ADC_Avg;        //HGB当前校准时，获取的HGB ADC的平均值
    
} HGB_AutoAdjust_t;

 




 


 

 

 

/*
*   语言
*/
typedef enum {
	LANGUAGE_CHINESE = 0,			//中文
	LANGUAGE_ENGLISH = 1,			//英语
	LANGUAGE_END     = 2,
    
} Language_e;
 
 
 
 
#define ACCOUNT_MAX_NUM									10				//最大账户数量
#define ACCOUNT_USER_NAME_MAX_LEN						10				//用户名字符最大长度
#define ACCOUNT_PSW_MAX_LEN								15				//密码字符最大长度


/***
*不同账号产生的数据类型，（便于后台、设备端对数据管理）
****/
typedef enum _AccountDataType_e{
	ACCOUNT_DATA_TYPE_INNER = 0,		//内部账号（测试账号，工程师账号）产生的数据
    ACCOUNT_DATA_TYPE_OUTSIDE,  		//外部账号（普通账号，管理员账号）产生的数据
	ACCOUNT_DATA_TYPE_END
}AccountDataType_e;


/***
*账户类型枚举
***/
typedef enum _AccountType_e{
	ACCOUNT_TYPE_SUPER = 0,											//超级管理员账号
	ACCOUNT_TYPE_ADMIN,    											//管理员账号
	ACCOUNT_TYPE_NORMAL,											//普通账号
	ACCOUNT_TYPE_NEW												//新增账号
}AccountType_e;


/***
*账户信息结构体
***/
typedef __packed struct _AccountInfo_s{
	char user[ACCOUNT_USER_NAME_MAX_LEN];							//账户名
	char psw[ACCOUNT_PSW_MAX_LEN];									//密码
	uint8_t authority;												//权限值
	AccountType_e type;												//账户类型
	AccountDataType_e dataType;										//账户对应的数据类型
}AccountInfo_s;






/*
*	 病人信息结构体
*/
typedef __packed struct{
	Msg_Head_t 			tMsgHead;								//消息头
	uint32_t			ulCurDataIndex;							//当前测试结果系列号，自动递增,用于数据管理
	uint8_t 			ucaSampleSN[SAMPLE_SN_LEN];				//当前样本编号,自动递增,"CBC-0001"
	char 				patientID[PATIENT_ID_MAX_LEN];			//患者编号字符串
	uint8_t 			ucaName[PATIENT_NAME_LEN];				//姓名
	SexType_e			eSex;									//性别
	uint8_t 			ucAge;									//年龄
    AgeUnit_e			eAgeUnit;								//年龄单位
	ReferGroup_e  		eReferGroup;							//参考组		
	TestMode_e 			eTestMode;								//当前测试模式 WBC+HGB
	AccountType_e	    eAccountType;							//操作者
    uint8_t				ucaDateTime[DATE_TIME_LEN];				//时间
    uint8_t			    ucaBirthDay[PATIENT_BIRTHE_DAY_MAX_LEN];//出生年月
	uint8_t		 		ucaRemark[REMARK_LEN];					//备注
	char 				orderNum[ORDER_NUM_MAX_LEN];			//订单编号，部分公司（如药师帮）才需要此字段
} Patient_Info_t;



/*
***************  login msg define  **************************
*/

/*
*	登录自检返回，信息
*/
#define MAX_ERROR_CODE_NUM        14                //一次最多错误码数（和最多检测项目数保持一致），
typedef __packed struct{
	Msg_Head_t		tMsgHead;						//消息头
	//
    uint8_t         ucProgress;                     //开机自检进度（0-100）
    uint8_t         ucErrorNum;                     //错误的个数
    ErrorCode_e     eaErrorCodeBuffer[MAX_ERROR_CODE_NUM];//错误码（错误也许有多个，根据ucErrorNum个数，查找）
	
} Login_SelfCheck_t;


 
/*
*   打印方式
*/
typedef enum {
	PRINTF_WAY_AUTO				= 0, 	//自动打印
	PRINTF_WAY_MANUAL			= 1, 	//手动打印
	PRINTF_WAY_END				= 2,
	 
 } PrintWay_e;

 
 
 
 

 

  
 
/*********************** 服务界面 **************************************/
 
  /*
 *	版本信息 结构体，规则：数值101对应的显示版本为：V1.0.1；硬件版本规则：
 字符串A01对应的显示版本为：字符串A.01
 */
 #define  CLINIC_NAME_MAX_LEN					50

#define RL_FLASH_FS_NAND_DRV_PATH               "N0:"
#define RL_FLASH_FS_NAND_FORMAT                 "/LL /L Nand /FAT16"
#define ACCOUNT_DATA_TYPE_OUTSIDE_PATH          "\\0"                 //外部账号产生数据保存的路径
#define ACCOUNT_DATA_TYPE_INNER_PATH            "\\1"                 //内部账号产生数据保存的路径
#define DATA_DIR_PATH                           "\\Data\\"     //数据存储路径
#define LOG_DIR_PATH                            "\\Log\\"       //日志存储路径
#define HEAD_DIR_PATH                           "\\Head\\"       //日志存储路径
/* 其他 */
#define OTHERS_DIR_PATH                         "\\Others\\"      //其他（运行日志。质控）存储路径

/* 图片 */
#define PICTURE_DIR_PATH                         "\\Picture\\"          //图片存储路径
/* 样本分析 相关图片*/
#define ANALYSIS_PICTURE_DIR_PATH                "\\Picture\\Analysis\\"  //样本分析图片存储路径
#define OTHERS_PICTURE_DIR_PATH                 "\\Picture\\Others\\"  //其他图片存储路径
/*软件*/
#define SOFTWARE_DIR_PATH                       "\\Software\\"   //软件存储路径
#define FONT_DIR_PATH                           "\\Font\\"       //字体存储路径
#define QC_PICTURE_DIR_PATH                     "\\Picture\\QC\\"
#define SET_PICTURE_DIR_PATH                    "\\Picture\\Set\\"
#define SERVICE_PICTURE_DIR_PATH                "\\Picture\\Service\\"


//单个文件存储最大数据数
#define MAX_DATA_NUM_ONE_FILE                   250
//文件名长度
#define FILE_NAME_MAX_LEN                       64

/*
*	 样本分析数据、日志信息。（样本分析数据、日志都通过DataManage_t管理，数据和日志的系列号一一对应）
*/
#define	DATA_FILE_RECORD_NUM			        MAX_DATA_NUM_ONE_FILE			//每个数据文件存储数据的条数
#define	DATA_INFO_LEN					        WBCHGB_TEST_INFO_LEN		//sizeof(WBCHGB_TestInfo_T)每条数据记录的结果大小，
#define DATA_FILE_PREFIX				        "Data_"     //数据文件名前缀
#define DATA_FILE_SUFFIX				        ".txt"      //数据文件名后缀


#define	LOG_FILE_RECORD_NUM				        MAX_DATA_NUM_ONE_FILE			//每个日志文件存储的条数
#define	LOG_INFO_MAX_LEN					    4096		                    //每条日志最大4k，
#define LOG_BUFFER_MAX_LEN                      (LOG_INFO_MAX_LEN-4)            //日志buffer最大长度，日志信息中包括，长度，crc，buffer，具体参考 CountLog_t 结构体
#define LOG_FILE_PREFIX					        "Log_"      //日志文件名前缀
#define LOG_FILE_SUFFIX					        ".txt"      //日志文件名后缀


#define	HEAD_FILE_RECORD_NUM				    MAX_DATA_NUM_ONE_FILE	        //每个计数控制文件存储的条数
#define	HEAD_INFO_LEN					        sizeof(CountHead_t)		    //每条控制信息的大小，
#define HEAD_FILE_PREFIX					    "Head_"      //日志文件名前缀
#define HEAD_FILE_SUFFIX					    ".txt"      //日志文件名后缀


#define LOG_RUN_HEAD_FILE_PREFIX		        "RunLog"  //运行日志头文件名前缀
#define LOG_RUN_STR_FILE_PREFIX			        ".txt"  //运行日志字符串文件名前缀

#define SAMPLE_SN_PREFIX                         "CBC-"        //样本编号前缀

/*
*   有些交互图片，图片中带有文字的，一般都有中英文版本，英文版本的文件名，在中文版上加有 "en_"的前缀
*/
//样本分析
#define PICTURE_GIF_OPERATE                      "Operate.gif"       //操作流程引导
#define PICTURE_GIF_INPUT                        "Input.gif"         //放入计数池
#define PICTURE_GIF_OK_OUTPUT                    "OkOutput.gif"     //计数成功，请拿出计数池
#define PICTURE_GIF_FAIL_OUTPUT2                 "FailOutput2.gif"  //计数失败，请拿出计数池（只有动画部分）
//
#define ANALYSIS_PICTURE_BMP_COUNT_FAIL           "CountFail.bmp"            //样本分析无效
#define ANALYSIS_PICTURE_BMP_COUNTING             "Counting.bmp"             //样本正在分析中...
#define ANALYSIS_PICTURE_BMP_RE_COUNT             "ReCount.bmp"             //是否重新测试
#define ANALYSIS_PICTURE_BMP_N_SEC_OUTIN_IN       "NSecOutinIn.bmp"       //N秒后自动进仓
#define ANALYSIS_PICTURE_BMP_CONFIRM_TAKEOUT      "ConfirmTakeout.bmp"     //请确认检测模块是否取出
//printer
#define PICTURE_BMP_PRITER_NO_PAPER               "PrinterNoPaper.bmp"
#define PICTURE_BMP_PRITER_CONN_ERROR             "PrinterConnError.bmp"
//样本分析，切图
#define PICTURE_ANALYSIS_WARN_BLUE               "WarnBlue.bmp"
#define PICTURE_ANALYSIS_WARN_WHILE              "WarnWhite.bmp"
//#define PICTURE_ANALYSIS_WAITING                 "Waiting.bmp"

/* 其他 切图 */
#define PICTURE_OTHERS_SELFCHECK                "SelfCheck.bmp"

extern const char gc_ucaPicture_PowerOnOff[COMPANY_END][20];
extern const char gc_ucaPicture_Login[COMPANY_END][20];
extern const char gc_ucaPicture_SelfCheck[COMPANY_END][20];


#define PICTURE_OTHERS_FRAME_BK                 "FrameBk.bmp"
#define PICTURE_OTHERS_IconBkBig                "IconBkBig.bmp"
#define PICTURE_OTHERS_IconBkSmall              "IconBkSmall.bmp"

/* 质控按钮背景 切图 */
#define PICTURE_QC_SET                          "QcSet.bmp"
#define PICTURE_QC_ANALYSIS                     "QcAnalysis.bmp"
#define PICTURE_QC_CHART                        "QcChart.bmp"
#define PICTURE_QC_LIST                         "QcList.bmp"
#define PICTURE_QC_ARROW                        "QcArrow.bmp"

/* 设置界面，相关背景 切图 */
#define PICTURE_SET_ChangeLogo                  "ChangeLog.bmp"
#define PICTURE_SET_FuZhu                       "FuZhu.bmp"
#define PICTURE_SET_FuZhuUnactive               "FuZhuUnactive.bmp"
#define PICTURE_SET_JiaoZhen                    "JiaoZhun.bmp"
#define PICTURE_SET_JiaoZhunGray                "JiaoZhunGray.bmp"
#define PICTURE_SET_LabInfo                     "LabInfo.bmp"
#define PICTURE_SET_Para                        "Para.bmp"
#define PICTURE_SET_ParaGray                    "ParaGray.bmp"
#define PICTURE_SET_Printer                     "Printer.bmp"
#define PICTURE_SET_SwitchClose                 "SwitchClose.bmp"
#define PICTURE_SET_SwitchOpen                  "SwitchOpen.bmp"
#define PICTURE_SET_Sys                         "Sys.bmp"
#define PICTURE_SET_Test                        "Test.bmp"
#define PICTURE_SET_TestGray                    "TestGray.bmp"
#define PICTURE_SET_User                        "User.bmp"

/* 服务界面，相关背景 切图 */
#define PICTURE_SERVICE_AgingTestActive         "AgingTestActive.bmp"
#define PICTURE_SERVICE_AgingTestUnactive       "AgingTestUnactive.bmp"
#define PICTURE_SERVICE_AirLight                "AirLight.bmp"
#define PICTURE_SERVICE_AllLog                  "AllLog.bmp"
#define PICTURE_SERVICE_AllLogUnactive          "AllLogUnactive.bmp"
//#define PICTURE_SERVICE_Data                    "Data.bmp"
#define PICTURE_SERVICE_Disk                    "Disk.bmp"
#define PICTURE_SERVICE_ErrLog                  "ErrLog.bmp"
#define PICTURE_SERVICE_ErrLogUnactive          "ErrLogUnactive.bmp"
#define PICTURE_SERVICE_InterActionActive       "InterActionActive.bmp"
#define PICTURE_SERVICE_InterActionUnactive     "InterActionUnactive.bmp"
#define PICTURE_SERVICE_Moto                    "Moto.bmp"
#define PICTURE_SERVICE_Others                  "Others.bmp"
#define PICTURE_SERVICE_ParaLog                 "ParaLog.bmp"
#define PICTURE_SERVICE_ParaLogUnactive         "ParaLogUnactive.bmp"
#define PICTURE_SERVICE_Pressure                "Pressure.bmp"
#define PICTURE_SERVICE_StatusActive            "StatusActive.bmp"
#define PICTURE_SERVICE_StatusUnactive          "StatusUnactive.bmp"
#define PICTURE_SERVICE_Version                 "Version.bmp"
#define PICTURE_SERVICE_DataActive              "DataActive.bmp"
#define PICTURE_SERVICE_DataUnActive            "DataUnactive.bmp"
#define PICTURE_SERVICE_Err                     "Err.bmp"
#define PICTURE_SERVICE_Ok                      "Ok.bmp"
#define PICTURE_SERVICE_SelfCheck               "SelfCheck.bmp"

/* 字库 */
#define FONT_SIF_SONGTI_12                      "songti12.sif"
#define FONT_SIF_SONGTI_16                      "songti16.sif"
#define FONT_UNI2GBK							"UNI2GBK.bin"








/*
*	HGB 测试结果信息,  
*/
typedef __packed struct{
    Msg_Head_t tMsgHead;
//    HGB_Test_e eHGB_Test;                             //HGB调试界面的，测试项目
    float       fHGB;                                   //HGB测试值
    float       fSrc_HGB;                               //HGB结果, 原始值
    uint16_t    usHGB_ADC_SrcBuffer[10];                //当前计数HGB结果，对应的原始ADC值
    float       fDst_HGB;                               //HGB结果，校准值
    uint16_t    usHGB_ADC_DstBuffer[10];                //当前计数HGB结果，对应的校准ADC值
    uint8_t     ucHGB_RstAlert;                         //HGB结果报警信息，0：
    uint8_t     ucaHGB_ErrorPrompt[HGB_PROMPT_LEN];		//HGB错误提示
    
} HGB_Data_t;



/*
*	WBC HGB 测试结果信息,  该结果数据需要保存到文件中
*/
#define INVALID_DATA_SN     0xFFFFFFFF


typedef __packed struct{	
	Msg_Head_t tMsgHead;								    //消息头
	
	//测试结果系列号，自动递增，该参数程序内部使用，用于区分不同结果
	DataStatus_e	    eDataStatus;							//数据状态,
	uint32_t		    ulCurDataSN;							//当前测试结果系列号，自动递增,
    uint32_t		    ulCurDataIndex;							//当前测试结果在数据管理中的索引号index
    uint8_t 		    ucaSampleSN[SAMPLE_SN_LEN];				//当前样本编号,自动递增,"CBC-00001"
	char				patientID[PATIENT_ID_MAX_LEN];			//患者编号字符串
	uint8_t 		    ucaName[PATIENT_NAME_LEN];				//姓名
    uint8_t			    ucaBirthDay[PATIENT_BIRTHE_DAY_MAX_LEN];//出生年月
	SexType_e		    eSex;									//性别
	uint8_t 		    ucAge;									//年龄
    AgeUnit_e		    eAgeUnit;								//年龄单位
	ReferGroup_e	    eReferGroup;							//参考组		
	TestMode_e 		    eTestMode;								//当前测试模式 WBC+HGB
	AccountType_e	    eAccountType;							//测试账号
	uint8_t			    ucaDateTime[DATE_TIME_LEN];				//时间
	long 			    timestamp;								//时间戳
    uint8_t		  	    ucaRemark[REMARK_LEN];					//备注
	//
	float 			    fWBC_Histogram[GRAPH_DATA_LEN];			//直方图数据
	float 			    fWBCHGB_RstData[WBCHGB_RST_END];		//WBC HGB值
	uint8_t             ucaWBCHGB_RstAlert[WBCHGB_RST_END];     //WBC HGB结果报警信息，0：
    float 			    fLines[3];								//三条分割线
	float 			    fWbcMaxPos;								//最大飞升值
	uint8_t 		    ucaWBC_ErrorPrompt[WBC_PROMPT_LEN];		//WBC错误提示
	uint8_t 		    ucaHGB_ErrorPrompt[HGB_PROMPT_LEN];		//HGB错误提示
	char 				orderNum[ORDER_NUM_MAX_LEN];			//订单编号
    uint16_t            usCrc;                                  //crc16数据校验值（）
    uint8_t             ucaEndFlag[2];                          //结束标志位，固定为"\r\n"，用于分隔每个测试结果
	
} WBCHGB_TestInfo_t;
#define WBCHGB_TEST_INFO_LEN			sizeof(WBCHGB_TestInfo_t)


/*
*   质控，结果，在控状态
*/
typedef enum {
    QC_CTRL_STATUS_IN     = 0,   //质控结果在控
    QC_CTRL_STATUS_OUT    = 1,   //质控结果不在控
    QC_CTRL_STATUS_END    = 2,
    
}QC_Ctrl_Status_e;

/*
*	质控，WBC HGB 测试结果信息,
*/
typedef __packed struct{	
	Msg_Head_t tMsgHead;								    //消息头
	
	//测试结果系列号，自动递增，该参数程序内部使用，用于区分不同结果
//    ErrorCode_e       eHideErrorCode;                       //隐藏故障码
	DataStatus_e	    eDataStatus;							//数据状态,
	uint32_t		    ulCurDataSN;							//当前测试结果系列号，自动递增,
    uint32_t		    ulCurDataIndex;							//当前测试结果在数据管理中的索引号index
    uint8_t 		    ucaSampleSN[SAMPLE_SN_LEN];				//当前样本编号,自动递增,"CBC-00001"
	uint8_t 		    ucaName[PATIENT_NAME_LEN];				//姓名
    //
    uint32_t            ulLastValidIndex;                       //上一个有效的数据的索引号(较新的)
    uint32_t            ulNextValidIndex;                       //下一个有效的数据的索引号（较旧的）
    QC_FILENUM_INDEX_e  eFileNumIndex;                          //用于质控，该数据所属文件号
    uint16_t            usFileNum;                              //文件号（0-65535）
    uint16_t            usTestCapacity;                         //一个文件号下，最大可测试次数（当前为100）
    uint8_t             ucaLotNum[QC_LOT_NUM_LEN];              //批号
    QC_LEVEL_e          eQC_Level;                              //质控品，水平/溶度
    AccountType_e       eSeter;                                 //当前文件号对于的质控设定信息的，操作者
    AccountType_e       eOperator;                              //测试账号(操作者)  
    uint8_t             ucaValidDate[DATE_TIME_LEN];            //质控品，有效期
    QC_Ctrl_Status_e    eQC_Ctrl_Status;                        //质控，在控状态
	//
	uint8_t			    ucaDateTime[DATE_TIME_LEN];				//时间
	long 			    timestamp;								//时间戳
	//
	float 			    fWBC_Histogram[GRAPH_DATA_LEN];			//直方图数据
	float 			    fWBCHGB_RstData[WBCHGB_RST_END];		//WBC HGB值
	uint8_t             ucaWBCHGB_RstAlert[WBCHGB_RST_END];     //WBC HGB结果报警信息，0：
    float 			    fLines[3];								//三条分割线
	float 			    fWbcMaxPos;								//最大飞升值
	uint8_t 		    ucaWBC_ErrorPrompt[WBC_PROMPT_LEN];		//WBC错误提示
	uint8_t 		    ucaHGB_ErrorPrompt[HGB_PROMPT_LEN];		//HGB错误提示
    uint16_t            usCrc;                                  //crc16数据校验值（）
    uint8_t             ucaEndFlag[2];                          //结束标志位，固定为"\r\n"，用于分隔每个测试结果
	
} QC_WBCHGB_TestInfo_t;
#define QC_WBCHGB_TEST_INFO_LEN			sizeof(QC_WBCHGB_TestInfo_t)


/************************************************************************************/
/*
*   计数数据和计数日志，状态控制信息结构体
*/
typedef __packed struct _CountHead_t{   
	ErrorCode_e  	eErrorCode;	                        //计数错误码
    ErrorCode_e  	eHideErrorCode;	                    //计数，隐藏错误码
	ErrorType_e		errType;		                    //计数异常类型
    uint8_t         ucUseAlgoFlag;                      //调用算法标志位，调用算法，数据便不会为0， 0：未调用，1：调用
    DataStatus_e    eStatus;                            //计数数据和日志状态,（同时改变）   
	uint8_t 		ucaSampleSN[SAMPLE_SN_LEN];			//当前样本编号,自动递增,"CBC-00001"
	AccountType_e	eAccountType;						//测试账号
	uint8_t			ucaDateTime[DATE_TIME_LEN];			//测试时间
    //数据SN号
    uint32_t		ulCurSN;							//当前计数的SN号， 该系列号和WBCHGB_TestInfo中的数据系列号（ulCurDataSN）保持一一对应
    uint32_t		ulCurIndex;							//当前计数的索引号
    uint32_t        ulLastValidIndex;                   //上一个有效的数据的索引号(较旧的)，不存在时该值为：INVALID_DATA_SN
    uint32_t        ulNextValidIndex;                   //下一个有效的数据的索引号（较新的），不存在时该值为：INVALID_DATA_SN
	
	uint8_t 		payingStatus;						//样本付款状态信息，为0表示尚未付款，为1表示已经付款
    
    uint16_t        usCrc;                                //crc16数据校验值
    uint8_t         ucaEndFlag[2];                        //"\r\n"
   
} CountHead_t;
#define COUNT_HEAD_SIZE     sizeof(CountHead_t)



/***
*计数日志结构体,  一共4k
***/
typedef __packed struct{
    uint16_t usCrc;                                 //校验        
	uint16_t usLogLen;                              //日志长度
    uint8_t ucaLogBuffer[LOG_BUFFER_MAX_LEN];       //日志buffer
    
}CountLog_t;


/***
*计数日志结构体
***/
typedef __packed struct _CountLog_s{
	CountHead_t logHead;							//计数日志头
	char Str[4096];							    //计数日志字符串
}CountLog_s;


/*
*运行日志头
*/
typedef __packed struct {
	uint32_t		num;								//当前运行日志索引序列号
	
	char 			shortStr[30];						//概要信息
	AccountType_e	eAccountType;						//账号
	char			dataTime[25];						//时间
	long 			timestamp;							//时间戳
    uint16_t        logLen;                          	//日志缓存长度
    uint32_t        logBufIndex;        
    //对应日志字符串文件中的偏移量

}RunLogHead_t;



/***
*运行日志
***/
typedef __packed struct _RunLog_s{
	RunLogHead_t logHead;
	char Str[512];
    uint16_t usCrc;                                 //校验
    
}RunLog_s;

#define RUN_LOG_MAX_LEN     sizeof(RunLog_s);









/***
*设置--调试功能界面参数，注意此界面参数不保存到EEPROM，即重启后设置失效
***/
typedef __packed struct ParaSetDebugInfo_s{
	uint32_t countDelayTime;					    //计数延迟时间，单位秒
	uint8_t jscReuseFlag;						//计数池重复使用标志
	uint8_t gzTestWBCFlag;						//工装测试WBC标识
	uint32_t gzTestCount;						//工装测试WBC执行次数
    
}ParaSetDebugInfo_s;



/***
*设置--调试功能参数,通信结构体
***/
typedef __packed struct{
    Msg_Head_t tMsgHead;						//消息头
	ParaSetDebugInfo_s sParaSetDebugInfo;		//调试功能参数
    
}ParaSetDebugInfo_Msg_t;




/***
*质控信息结构体, (上下值输入时均为float，这里统一*100，按uint32_t存储)
***/
typedef __packed struct{  
	QC_FILENUM_INDEX_e 	eFileNumIndex;       		//文件号编号索引，0-5（当前最多数据输入6个文件编号）
	uint16_t 	usFileNum;                      	//文件号（0-65535）
	uint16_t 	usTestCapacity;                 	//一个文件号下，最大可测试次数（当前为100）
	uint8_t 	ucaLotNum[QC_LOT_NUM_LEN];      	//批号
	QC_LEVEL_e 	eQC_Level;                      	//质控品，水平/溶度
	AccountType_e 	eSeter; 
	uint8_t 	ucaValidDate[DATE_TIME_LEN];    	//质控品，有效期
	float 	faUpperLimit[WBCHGB_RST_END];   		//WBC HGB 参数 上限
	float 	faLowerLimit[WBCHGB_RST_END];   		//WBC HGB 参数 下限
}QC_SetInfo_s;


/*
*	QC质控设定，交互信息结构体 
*/
typedef struct {
    Msg_Head_t      tMsgHead;
    uint8_t         ucCurIndex;      //文件列表选中项对应的下标
    uint8_t         ucMaxIndex;      //文件列表对应的最大下标
    QC_SetInfo_s    tQC_SetInfo;  //操作的，QC_SetInfo
    
}QC_SetInfo_Msg_t;


/*
*	QC质控，交互信息结构体 
*/
typedef struct {
    Msg_Head_t      tMsgHead;
    uint32_t        ulIndex;    //质控数据对于存储下标 或是 文件号的下标
}QC_Msg_t;



//===仪器参数相关定义=====================================================
#define MACHINFO_INIT_FLAG								0xA55A0FF0
#define STRUCT_START_INIT_FLAG							0xA55A0FF0
					
#define SKIN_INIT_FLAG									0xA55A0FF0
#define DOWN_LOAD_INFO_FLAG								0xA55A0FF0

#define TOUCH_INIT_FLAG									0x5AA50FF0		//表示触摸参数已经初始化过了
#define TOUCH_FOCUS_FLAG								0xF00FA55A		//表示要强制进入校准模式
					
#define MQTT_IP_MAX_LEN									30
#define MQTT_NAME_MAX_LEN								30
#define MQTT_PSW_MAX_LEN								30


//账户权限值	
#define ACCOUNT_SUPER_AUTHORITY							255				//超级管理员权限值
#define ACCOUNT_ADMIN_AUTHORITY							100				//普通管理员权限值
#define ACCOUNT_NORMAL_AUTHORITY						50				//普通用户权限值
#define ACCOUNT_NEW_AUTHORITY							1				//新增用户权限值

//内置账户默认密码
#define ACCOUNT_SUPER_DEFAULT_PSW						"YongFu123"		//超级管理员默认密码值
#define ACCOUNT_ADMIN_DEFAULT_PSW						"admin"			//管理员默认密码值
#define ACCOUNT_NORMAL_DEFAULT_PSW						"001"			//普通用户默认密码值


/***
*标志结构体
***/
typedef __packed struct{
	uint32_t	startFlag;										//当此标志不为STRUCT_START_INIT_FLAG时，且不为0xFFFFFFFF时，则标明该结构体数据内容有被覆盖，直接锁定程序；当为0xFFFFFFFF时，则标明是新增的结构体，则直接初始化即可
	uint16_t 	size;											//用来记录此结构体字节长度，便于增加变量时能被感知到，以便对新变量进行初始化
	
	__packed struct{
		uint64_t	connNet:						1;			//为0表示尚未将仪器信息发送到服务端，为1表示已经将仪器信息发送到服务端
		uint64_t	userTimestampInit:				1;			//为1表示已经初始化了用户首次开机时间戳
		uint64_t	lock:							1;			//表示是否需要锁定当前仪器，为1表示锁定，为0表示不锁定
	}bit;
}MachInfoFlag_s;


/***
*公司特性设置
***/
typedef __packed struct{
	uint32_t	startFlag;
	uint16_t 	size;

	Company_e	company;										//当前公司代号

	//第三方MQTT服务端设置
	__packed struct{
		char	ip[MQTT_IP_MAX_LEN];							//IP或域名
		char	name[MQTT_NAME_MAX_LEN];						//用户名
		char	psw[MQTT_PSW_MAX_LEN];							//密码
	}mqtt;

	//皮肤色设置
	__packed struct{
		uint32_t	bg;											//背景色
		uint32_t	title;										//标题栏色
		uint32_t	titleFont;									//标题栏中字符色
		uint32_t	buttNotPress;								//按钮未点击色
		uint32_t	buttNotPressFont;							//按钮未点击字符色
		uint32_t	buttPress;									//按钮点击色
		uint32_t	buttPressFont;								//按钮点击字符色
		uint32_t	buttBoard;									//按钮未点击时边框色
		uint32_t	highlightKJNotSelect;						//高亮控件未选中色
		uint32_t	highlightKHNotSelectFont;					//高亮控件未选中字符色
		uint32_t	highlightKJSelect;							//高亮控件选中色
		uint32_t	highlightKJSelectFont;						//高亮控件选中字符色
	}skin;

	__packed struct{
		uint64_t	hasOtherServer:						1;		//为1表示存在第三方服务端
		uint64_t	diffPowerOnOffCnEnPic:				1;		//为1表示区分中英文模式下登录和关机界面
		uint64_t	downLoadPatientInfo:				1;		//为1表示开启下拉患者信息功能
	}flag;
}CompanyInfo_s;


/***
*CAT关键性同步服务端事件标志
***/
typedef __packed struct{
	uint32_t	startFlag;
	uint16_t 	size;
	
	__packed struct{
		uint64_t	paraSet:							1;			//设置部分参数
		uint64_t	innerSampleResult:					1;			//内部账号发送样本分析结果数据
		uint64_t	outtorSampleResult:					1;			//外部账号发送样本分析结果数据
		uint64_t	otherInnerSampleResult:				1;			//内部账号发送第三方服务端样本分析结果数据
		uint64_t	otherOuttorSampleResult:			1;			//外部账号发送第三方服务端样本分析结果数据
		uint64_t	innerChangeSampleResult:			1;			//修改内部账号发送样本分析结果数据
		uint64_t	outtorChangeSampleResult:			1;			//修改外部账号发送样本分析结果数据
		uint64_t	otherInnerChangeSampleResult:		1;			//修改内部账号发送给第三方服务端样本分析结果数据
		uint64_t	otherOuttorChangeSampleResult:		1;			//修改外部账号发送给第三方服务端样本分析结果数据
		uint64_t	sendRunLog:							1;			//发送运行日志，每次开机时才检查一次是否有运行日志需要发送给服务端，并且一次性发送完毕所有待发送的运行日志
		uint64_t	delTestData:						1;			//清除了测试数据
		uint64_t	delUserData:						1;			//清除了用户数据
		uint64_t	resetUserOpenTimestamp:				1;			//重置用户开机时间
	}bit;
}CatMastSynServEvent_s;


/***
*仪器参数设置结构体
***/
typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	uint8_t 	ucWBC_Gain;			        //WBC增益
	uint8_t		ucHGB_Gain;				    //HGB增益
	
	uint16_t 	current535;		    		//535波长灯驱动电流值
	uint16_t 	current415;		    		//415波长灯驱动电流值
	uint16_t   	KQADC415;            		//415波长HGB空气校准ADC值
	uint16_t   	KQADC535;            		//535波长HGB空气校准ADC值
	uint16_t   	KBADC415;            		//415波长HGB空白校准ADC值
	uint16_t   	KBADC535;            		//535波长HGB空白校准ADC值
	uint16_t   	point1Curr415;            	//415波长线性点1电流值
	uint16_t   	point1Curr535;            	//535波长线性点1电流值
	uint16_t   	point2Curr415;            	//415波长线性点2电流值
	uint16_t   	point2Curr535;            	//535波长线性点2电流值

	uint8_t		ucPump_Scale;			    //泵占空比 （0-100）%
	uint16_t   	usNsec_BeepWaring;          //样本分析后, N秒后进出仓未进仓，则蜂鸣器报警
	uint32_t 	ulOutIn_In_AddStep;		    //进出电机，往里走，光耦补充
	uint32_t 	ulOutIn_Out_AddStep;	    //进出电机，往外走，光耦补充 
	uint32_t 	ulClamp_In_AddStep;		    //夹子电机，往里走，光耦补充
	uint32_t 	ulClamp_Out_AddStep;        //夹子电机，往外走，光耦补充
	float		fPress_Adjust;			    //气压校准值   
	uint8_t 	pressSensor;				//压力传感器选择，为0表示使用IT110A，为1表示使用HONEYWELL传感器

	int 		digSensor[5];				//数字压力传感器补偿值，5点补偿，0Kpa，-10Kpa，-20Kpa，-30Kpa，-40Kpa
}ParamSet_s;


/***
*人工校准结构体
***/
typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	float 	fWBC;										//WBC校准系数
	uint8_t 	ucaWBC_CalibrationDate[DATE_TIME_LEN];  //校准日期
	
	float 	fHGB;										//HGB校准系数
	uint8_t 	ucaHGB_CalibrationDate[DATE_TIME_LEN];  //校准日期
}Calibration_s;


/***
*系统设置 结构体
***/
typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	uint8_t 	ucaDate[DATE_TIME_LEN];					//日期
	uint8_t 	ucaTime[DATE_TIME_LEN];					//时间
	Language_e 	eLanguage;								//语言，中文，英语
}SystemSet_s;



/***
*账户信息管理结构体
***/
typedef __packed struct _AccountManagement_s{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	AccountInfo_s 	accountInfo[ACCOUNT_MAX_NUM];					//所有账户信息
	uint8_t 	endIndex;											//尾部索引
	uint8_t 	defaultIndex;										//默认账户索引
	uint8_t 	curIndex;											//当前登录的账户索引
}AccountManagement_s;


/***
*打印设置 结构体
***/
 #define PRINTSET_TITILE_MAX_LEN		31

typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	char 	ucaPrintTitle[PRINTSET_TITILE_MAX_LEN];	 //打印标题
	PrintWay_e		ePrintWay;
}PrintSet_s;


/***
*实验室信息 结构体
***/
#define  CLINIC_NAME_MAX_LEN					50
#define MACHINE_SN_LEN							12

typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	uint8_t 	ucaMachineName[30];						//机器名称
	uint8_t 	ucaMachineType[10];						//机器类型
	uint8_t		ucaMachineSN[MACHINE_SN_LEN+1];			//机器系列号
	uint8_t		ucaClinicName[CLINIC_NAME_MAX_LEN];		//诊所名称	
}LabInfo_s;


/***
*参考组数据内容结构体
***/
typedef __packed struct _RefGroups_s{
	uint16_t usWBCH;						//上限，这里的值=实际值*100
	uint16_t usWBCL;						//下限，这里的值=实际值*100
	uint16_t usGranH;
	uint16_t usGranL;
	uint16_t usMidH;
	uint16_t usMidL;
	uint16_t usLymH;
	uint16_t usLymL;
	uint16_t usGranPercentH;
	uint16_t usGranPercentL;
	uint16_t usMidPercentH;
	uint16_t usMidPercentL;
	uint16_t usLymPercentH;
	uint16_t usLymPercentL;
	uint16_t usHGBH;
	uint16_t usHGBL;
}RefGroups_s;


/***
*电阻触摸屏，校准参数
***/
#define NUM_CALIB_POINTS  			5 
typedef __packed struct{
	uint32_t 	startFlag;
	uint16_t 	size;
	
	uint32_t flag;							//为TOUCH_INIT_FLAG时表示已经校准过触摸参数，为TOUCH_FOCUS_FLAG时表示要强制进入校准模式
	int pixelX[NUM_CALIB_POINTS];			//定义用于保存绘制校准点的像素坐标
	int pixelY[NUM_CALIB_POINTS];
	int samX[NUM_CALIB_POINTS];				//定义用于保存采集到校准点的AD值  
	int samY[NUM_CALIB_POINTS];
}TouchPara_s;






/***
*仪器参数结构体，注意这里必须为__packed模式
***/
typedef __packed struct _MachInfo_s{	
	uint32_t 	initFlag;									//当此标志不为MACHINFO_INIT_FLAG时，则需要将全部参数进行初始化操作
	
	TouchPara_s 	touch;									//触摸参数
	MachInfoFlag_s 	flag;									//标志类
	CompanyInfo_s 	companyInfo;							//公司特性参数
	CatMastSynServEvent_s 	catMastSynServEvent;			//CAT关键性同步服务端事件标志
	ParamSet_s 	paramSet;									//参数信息
	Calibration_s 	calibration;							//人工校准信息
	SystemSet_s		systemSet;								//系统设置信息
	AccountManagement_s		accountMM;						//账户管理信息
	PrintSet_s 	printSet;									//打印设置信息
	LabInfo_s 	labInfo;									//实验室信息
	
	__packed struct{
		uint32_t 	startFlag;
		uint16_t 	size;
		
		QC_SetInfo_s 	set[QC_FILENUM_INDEX_END]; 			//质控设定参数
	}qcInfo;
	
	//===其他类变量===
	__packed struct{
		uint32_t 	startFlag;
		uint16_t 	size;
		
		RefGroups_s 	refGroups[REF_GROUPS_END];   		//参考组对应的取值范围
		long 	factoryTimestamp;                 			//出厂开机时间戳，第一次录入仪器序列号的时间
		long 	userTimestamp;								//用户首次开机时间戳，用户第一次开机使用的时间
		uint32_t 	totalOpenCount;							//当前开机总次数		
		uint16_t 	uiVersion;								//UI资源版本号，默认100即V1.0.0
	}other;
}MachInfo_s;


extern MachInfo_s MachInfo;


/***
*定义键盘弹出初始界面枚举
***/
typedef enum _KeyBoardInitJM_e{
	KEYBOARD_HZ = 0,				//初始显示输入汉字界面
	KEYBOARD_CHAR,					//初始显示字母界面
	KEYBOARD_NUM					//初始显示数字界面
}KeyBoardInitJM_e;



/*
*   wbc hgb 人工校准消息
*/
typedef __packed struct {
    Msg_Head_t      tMsgHead;
    Calibration_s   calibration;   
    
} CalibrationMsg_t;


typedef __packed struct _WBCGZData_s{
	float wbc;
	float fl;
}WBCGZData_s;


/*
*	仪器实时运行状态信息结构体
*/
#define LIST_REVIEW_ROW_NUM     			7
#define HARD_VERSION_MAX_LEN				10										//硬件版本号字符串最大长度
	

typedef __packed struct _MachRunPara_s{
    NextSample_t            tNextSample;            					//下一样本信息 
    uint8_t                 ucaLastestDateTime[DATE_TIME_LEN];          //界面显示的实时时间      
    Login_SelfCheck_t       tLogin_SelfCheck;        					//登录自检信息（保存登录自检等，返回的错误码）
    WBCHGB_TestInfo_t       taListView_Data[LIST_REVIEW_ROW_NUM];  		//列表回顾显示数据，为下面的类型(不会加载已删除的数据): 正常数据、初始化数据
    uint32_t                ulMenuID;               					//当前界面ID，（菜单、登录，样本分析，列表回顾，设置、服务等），根据ID号对公共部分代码（如：样本分析和列表回顾公共界面，键盘默认中英方式等）做差异化出来
    KeyBoardInitJM_e		keyboardInitJM;								//键盘初始界面显示枚举
	//
    WBCHGB_TestInfo_t 		tWBCHGB_TestInfo;		                    //WBC HGB测试结果信息
	ParaSetDebugInfo_s		psDebugInfo;			                    //设置--调试功能信息
	
    //质控数据
    QC_FILENUM_INDEX_e      eQC_Analysis_FileNum_Index;
    QC_FILENUM_INDEX_e      eQC_List_FileNum_Index;
    QC_SetInfo_s            taQC_SetInfo[QC_FILENUM_INDEX_END];         //质控设定信息
    QC_WBCHGB_TestInfo_t    tQC_WBCHGB_TestInfo;                        //质控分析界面显示数据
    QC_WBCHGB_TestInfo_t    tQC_ListView_Data;                          //质控列表回顾显示数据   

	//硬件版本号
	__packed struct{
		char main[HARD_VERSION_MAX_LEN];								//主控板硬件版本号
		char zjb[HARD_VERSION_MAX_LEN];									//转接板硬件版本号
	}hardVer;
	
	//升级相关
	__packed struct{
		uint32_t nextOpenToStartUpdateFlag;								//用来表示是否下一次开机时将进入在线升级流程的标志
		uint32_t needBootToUpdateMainFlag;								//用来表示主程序已经下载完毕，需要重启让BOOT将主程序写入到指定位置处
	}update;
   
	//实时标志相关
	__packed struct{
		uint8_t requestingPatient;										//用于表示正在请求患者信息中，为1表示正在请求患者信息中
	}flag;
	
	//药师帮下拉患者列表信息
	__packed struct{
		uint8_t pageNum;												//页号，从第一页开始计算
		uint8_t num;													//当前页中包含的患者信息个数
		
		__packed struct{
			char 	name[PATIENT_NAME_LEN];								//姓名
			uint8_t 	age;											//年龄
			AgeUnit_e 	eAgeUnit;										//年龄单位
			SexType_e 	eSex;											//性别
			char 	orderNum[ORDER_NUM_MAX_LEN];						//订单编号
		}info[5];
	}patientList;
	
	//网络中断与恢复时状态
	__packed struct{
		uint8_t flag;													//为1表示网络已经中断，等待恢复
		uint32_t interruptTimestamp;									//网络中断时间戳
		uint8_t interruptSignalStrength;								//网络中断时信号强度
		uint32_t recoveryTimestamp;										//网络中断时间戳
		uint8_t recoverySignalStrength;									//网络中断时信号强度
	}netStatus;
	
	//当前等待服务端响应的结果数据或者故障日志的样本编号
	__packed struct{
		uint32_t sampleSN;
		uint32_t logSN;
	}currSN;
	
	//质检相关数据
	__packed struct{
		__packed struct{
			WBCGZData_s kb_0_200[3];									//WBC空白
			WBCGZData_s jmd_8_300[10];									//WBC精密度
			WBCGZData_s jz_5_300[5];									//WBC校准--5/300
			WBCGZData_s jz_8_300[5];									//WBC校准--8/300
			WBCGZData_s jz_10_300[5];									//WBC校准--10/300
			WBCGZData_s xxd_0_1_200[3];									//WBC线性度--0.1/200
			WBCGZData_s xxd_25_200[3];									//WBC线性度--25/200
			WBCGZData_s xxd_50_200[3];									//WBC线性度--50/200
			WBCGZData_s xxd_75_200[3];									//WBC线性度--75/200
			WBCGZData_s xxd_100_200[3];									//WBC线性度--100/200
		}wbcGZ;
		
		__packed struct{
			float kb[3];												//HGB空白
			float jmd[10];												//HBG精密度
			
			//HGB校准
			__packed struct{
				float dest;												//靶值
				float value[5];											//测量值
			}jz[3];
			
			float xx[5][3];												//HGB线性
		}hgb;
		
		//准确度
		__packed struct{
			//靶值
			WBCGZData_s wbcDest;
			float hgbDest;
			
			//测量值
			WBCGZData_s wbcValue;
			float hgbValue;
		}zqd[3];
	}zj;
}MachRunPara_s;


#define OUTSIDE_DATA_NUM_INIT_FLAG						0x5AA50FF0		//外部账号样本分析数据容量初始化标记
#define OUTSIDE_DATA_NUM_MODE_NUM						2				//外部账号样本分析数据容量模式种类数
#define OUTSIDE_DATA_MAX_NUM							4500			//外部账号样本分析数据最大容量

/***
*外部账号样本分析数据容量结构体
***/
typedef struct _OutSideDataNum_s{
	uint32_t initFlag;													//初始化标记
	
	uint8_t index;
	uint32_t dataNum[OUTSIDE_DATA_NUM_MODE_NUM];
}OutSideDataNum_s;




#define RUN_LOG_MAX_NUM                                     500     //运行日志最大条数(当前存储再spiflash中，不要超过500)
#define QC_DATA_MAX_NUM                                     50      //质控数据最大条数
#define INNER_ACCOUNT_COUNT_MAX_NUM                         501      //内部账号最大测试数据条数
#define QC_PER_FILE_NUM_MAX_CAPACITY                        100 	//每个文件号最大测试的次数
/*
*   WBC HGB数据管理,有限双循环链表
*/
typedef __packed struct{
    uint32_t        ulLastestSN;                            //下一样本系列号，标记每一个测试数据，从0开始递增，（样本编号=样本系列号+1）
	uint32_t  		ulQueue_HeadIndex;					    //队列头，第一条有效数据
    uint32_t  		ulQueue_TailIndex;				        //队列尾，下一个即将存储位置（它的上一条为最新的有效数据）
    uint32_t        ulValidNum;                             //队列 中有效数据个数
    uint8_t         ucaLastestDateTime[DATE_TIME_LEN];		//最后一次测试时间
    
	uint32_t        ccSendIndex;                            //指向当前尚未发送样本分析结果给创怀服务端的样本编号索引值
	uint32_t        otherSendIndex;                         //指向当前尚未发送样本分析结果给第三方服务端的样本编号索引值
} DataManage_WBCHGB_t;


/*
*    QC数据管理,有限双循环链表
*/
typedef __packed struct {
    uint32_t        ulQC_SN;                                //质控数据的系列号,从零开始递增，标记每个日志
    uint32_t        ulQC_Queue_HeadIndex;                   //质控数据的队列的，第一条日志的index
    uint32_t        ulQC_Queue_TailIndex;                   //质控数据的队列的，下一条即将写入index
    uint32_t        ulValidNum;                             //质控数据的队列的，有效个数

} DataManage_QC_t;



/*
*  数据管理结构体
*/
typedef __packed struct{
       
    //外部、内部账号，样本分析数据（WBC、HGB）管理
    DataManage_WBCHGB_t tDataManage_WBCHGB[ACCOUNT_DATA_TYPE_END];   
    //质控数据管理
    DataManage_QC_t tDataManage_QC[QC_FILENUM_INDEX_END];
    //运行日志管理
    uint32_t        ulRunLog_SN;                            //运行日志的系列号,从零开始递增，标记每个日志
    uint32_t        ulRunLog_Queue_HeadIndex;               //运行日志队列的，第一条日志的index
    uint32_t        ulRunLog_Queue_TailIndex;               //运行日志队列的，下一条即将写入index
    uint32_t        ulRunLog_SendIndex;                     //运行日志队列，下一条需要发送日志的索引号（index）
    //
    uint16_t        usCrc;                                  //crc16数据校验值
} DataManage_t;








#endif



