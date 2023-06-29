#ifndef _CAT_IOT_H
#define _CAT_IOT_H


#include "stm32f4xx_hal.h"
#include "bsp_rtc.h"
#include "msg_def.h"





/*宏定义区********************************************************/
#define CAT_RECE_BUF_SIZE									4095					//定义从CAT模块接收到的数据循环缓冲队列大小，这里为了进行&运算，所以实际队列长度需要再+1



//#define MQTT_SERVICE_IP									"broker.emqx.io"		//定义MQTT服务端使用的域名或者IP地址
//#define MQTT_SERVICE_PORT									1883					//定义MQTT服务端使用的端口号
//#define MQTT_SERVICE_USER_NAME								"CreateCareMQTT"		//定义MQTT服务端使用的登录用户名
//#define MQTT_SERVICE_PASSWORD								"KaiFa0098"				//定义MQTT服务端使用的登录密码

#define MQTT_SERVICE_IP										"120.78.135.48"			//定义MQTT服务端使用的域名或者IP地址
#define MQTT_SERVICE_PORT									1883					//定义MQTT服务端使用的端口号
#define MQTT_SERVICE_USER_NAME								"CCAdmin"				//定义MQTT服务端使用的登录用户名
#define MQTT_SERVICE_PASSWORD								"HWQ00968163HB"			//定义MQTT服务端使用的登录密码

#define FTP_SERVICE_IP										"120.78.135.48"			//定义FTP服务端使用的域名或者IP地址
#define FTP_SERVICE_PORT									21						//定义FTP服务端使用的端口号
#define FTP_SERVICE_USER_NAME								"CreateCare"			//定义FTP服务端使用的登录用户名
#define FTP_SERVICE_PASSWORD								"huangweiqiang0098Hb"	//定义FTP服务端使用的登录密码

#define GAODE_KEY_POSITION									"271e23f771fe86c306a2b53cdef935cc"	//定义中移CAT定位需要使用的高德KEY

//临时使用的第三方服务端配置
//#define MachInfo.companyInfo.mqtt.ip								"101.33.204.141"		//定义第三方MQTT服务端使用的域名或者IP地址
//#define MQTT_SERVICE_PORT							1883					//定义第三方MQTT服务端使用的端口号
//#define MachInfo.companyInfo.mqtt.name						"yaoShiBang"			//定义第三方MQTT服务端使用的登录用户名
//#define MachInfo.companyInfo.mqtt.psw						"yaoShiBang_#374*_Cc"	//定义第三方MQTT服务端使用的登录密码
//#define MachInfo.companyInfo.mqtt.ip								"ch-mqtt.guangpuyun.cn"	//定义第三方MQTT服务端使用的域名或者IP地址
//#define MQTT_SERVICE_PORT								1883					//定义第三方MQTT服务端使用的端口号
//#define MachInfo.companyInfo.mqtt.name						"yaoShiBang"			//定义第三方MQTT服务端使用的登录用户名
//#define MachInfo.companyInfo.mqtt.psw							"yaoShiBang_#374*_Cc"	//定义第三方MQTT服务端使用的登录密码






/***
*定义从CAT模块接收到的数据循环缓冲队列结构体
***/
typedef struct _CAT_ReceBuf_s
{
	uint16_t readIndex;
	uint16_t writeIndex;
	
	char buf[CAT_RECE_BUF_SIZE+1];
}CAT_ReceBuf_s;


/***
*定义当前使用的CAT模块型号枚举
***/
typedef enum _CAT_Type_e{
	CAT_YK = 0,						//使用移柯CAT
	CAT_ZY,							//使用中移CAT
}CAT_Type_e;


/***
*定义当前要连接的服务端枚举
***/
typedef enum _CAT_SERVICE_e{
	SERVICE_CC = 0,					//创怀服务端
	SERVICE_OTHER,					//第三方服务端
}CAT_SERVICE_e;


/***
*定义当前注网状态
***/
typedef enum _CAT_RegistNetStatus_e{
	CAT_REGISTNET_UNKNOW = 0,							//表示未知状态
	CAT_REGISTNET_SUCCESS,								//表示注网成功
	CAT_REGISTNET_CHECKING,								//表示注网失败，持续搜网中
	CAT_REGISTNET_SIM_OVERDUE,							//表示注网失败，SIM卡过期
}CAT_RegistNetStatus_e;



/***
*定义当前发送的数据所属测试项目代号枚举
***/
typedef enum _CAT_TestType_e{
	TEST_TYPE_WBC_HGB = 0,								//WBC+HGB测试
}CAT_TestType_e;




/***
*定义CAT模块全局状态量结构体
***/
typedef struct _CATGlobalStatus_s{
	struct{
		uint16_t powOn:						1;				//CAT模块是否上电完成
		uint16_t conn:						1;				//CAT模块和MCU连接是否正常
		uint16_t sim:						1;				//是否有卡
		uint16_t connServerErr:				1;				//是否连接服务端异常
		uint16_t connOtherServerErr:		1;				//是否连接第三方服务端异常
		
		uint16_t tcpConn:					1;				//判断是否创建了TCP连接
		uint16_t ftpConn:					1;				//判断是否创建了FTP连接
		uint16_t mqttConn:					1;				//判断是否连接到服务端的MQTT成功
		
		uint16_t noOtherServer:				1;				//判断是否存在第三方服务端，为0表示存在，为1表示不存在
	}flag;
	
	CAT_RegistNetStatus_e registerNet;						//用来表示当前注网状态
	uint32_t catDealSysRestartFlag;							//用来表示CAT处理了一些动作后，需要重启仪器的标志，为0xA55A0FF0时表示需要重启，否则不重启
	CAT_Type_e catType;										//用来表示当前使用的是哪个厂家的CAT模块
	uint8_t operatorNum;									//运营商编号，为0表示中国电信，为1表示中国移动，AT+COPS?指令返回值中，46011表示的是中国电信，46000表示的是中国移动
	uint8_t signalStrength;									//信号强度，数值越小信号越弱，99值表示没有信号
	char imei[16];											//全球移动设备号IMEI，15位字符串
	char iccid[21];											//SIM卡ICCID,20位字符串
	char sn[15];											//模块SN号
	char position[25];										//模块位置信息
	
	//服务端中当前保存的各模块版本号
	struct{
		uint16_t PMainVersion;                            	//服务端中保存的发布版主程序版本号
		uint16_t PBootloadVersion;                       	//服务端中保存的发布版引导程序版本号
		uint16_t PUiVersion;                              	//服务端中保存的发布版UI版本号
		
		uint16_t TMainVersion;                            	//服务端中保存的测试版主程序版本号
		uint16_t TBootloadVersion;                       	//服务端中保存的测试版引导程序版本号
		uint16_t TUiVersion;                              	//服务端中保存的测试版UI版本号
	}serviceVersion;
	
	//本地当前保存的各模块版本号
	struct{
		uint16_t mainVersion;
		uint16_t bootloadVersion;
		uint16_t uiVersion;
	}localVersion;
	
	//服务端请求执行的特殊功能项标志
	struct{
		uint8_t JSCDect;									//服务端请求忽略计数池检测功能标志
		uint8_t OCDect;										//服务端请求忽略光耦检测功能标志
		uint8_t JSCReuse;									//服务端请求计数池重复使用功能标志
	}ignoreFunFlag;
	
	//服务端响应的指定程序文件信息
	struct{
		uint8_t publishOrTestFlag;	
		uint8_t softwareType;	
		uint32_t totalLen;
		uint16_t crc16;
	}softwareInfo;
	
	//服务端响应的指定版本的UI资源信息
	struct{
		uint8_t publishOrTestFlag;
		uint16_t uiVersion;									
		uint8_t totalNum;
		
		struct{
			uint32_t fileSize;
			uint16_t crc16;
			char path[70];
		}file[10];
	}uiInfo;
}CATGlobalStatus_s;




/***
*定义MQTT通用数据结构体
***/
typedef __packed struct _MQTT_PayloadBuf_s{
	uint16_t cmdH;
	uint8_t cmdM;
	uint8_t cmdL;
	
	long cmdTimestamp;											//指令发送时间戳
	
	uint16_t packetNum;											//包序号
	uint16_t crc16;												//CRC16
	uint16_t payloadLen;										//数据内容长度

	//各帧数据内容联合体
	__packed union{
		char str[4200];											//用来存放字符串
		
		uint8_t u8;												//用来存放一个无符号8位的参数
		uint16_t u16;											//用来存放一个无符号16位的参数
		uint32_t u32;											//用来存放一个无符号32位的参数
		int8_t i8;												//用来存放一个有符号8位的参数
		int16_t i16;											//用来存放一个有符号16位的参数
		int32_t i32;											//用来存放一个有符号32位的参数
		
		//仪器所有参数信息帧，指令2.2.1
		__packed struct{
			uint16_t softwareVersion;							//软件完整版本号
			uint16_t mainVersion;                               //主程序版本号
			uint16_t bootloadVersion;                           //引导程序版本号
			uint16_t uiVersion;                                 //UI版本号
			uint16_t algoVersion;                               //算法版本号
			char mainHardwareVersion[5];                    	//主控板硬件版本号，"MA00"
			char zjHardwareVersion[5];                      	//转接板硬件版本号，"DA00"
			char imei[16];                                      //IMEI号
			char iccid[21];                                      //ICCID号
			uint8_t operatorNum;                                //运营商代码，0表示中国移动，1表示中国电信
			uint8_t languMode;                                  //语言模式
			uint8_t lockFlag;									//仪器是否处于锁定状态，为0表示解锁，为0xA5表示锁定状态
			long factoryTimestamp;                         		//出厂开机时间戳，第一次录入仪器序列号的时间
			long userFirstOpenTimestamp;                        //用户第一次开机时间戳
			Company_e company;									//用于表示当前仪器所属的公司
			char zhensuoName[CLINIC_NAME_MAX_LEN];				//诊所名
																
			uint8_t wbcPara;                                    //WBC增益补偿
			uint8_t hgbPara;                                    //HGB增益补偿
			uint8_t pumpPara;                                   //泵占空比值
			uint16_t current535;		    					//535波长灯驱动电流值
			uint16_t current415;		    					//415波长灯驱动电流值
			uint16_t outOC;                                     //出仓光耦补偿值
			uint16_t inOC;                                      //进仓光耦补偿值
			uint16_t gdOC;                                      //固定光耦补偿值
			uint16_t sfOC;                                      //释放光耦补偿值
			int16_t presurePara;                                //压力补偿值
			uint16_t wbcAdjust;                                 //WBC校准系数值
			uint16_t hgbAdjust;                                 //HGB校准系数值
			uint8_t pressureSensor;								//模拟压力传感器类型
			CAT_Type_e catType;									//当前使用的CAT模块类型
			uint8_t downLoadInfoFlag;							//是否开启计数前下拉患者信息的标志，为1表示开启，其他值表示未开启
		}cmd2_2_1;
		
		//仪器当前开机信息，指令2.2.4
		__packed struct{
			uint32_t totalOpenCount;							//当前开机总次数
			char position[22];									//位置信息
			uint8_t signalStrength;								//信号强度
			uint16_t softwareVersion;							//软件完整版本号
			uint16_t mainVersion;                               //主程序版本号
			uint16_t bootloadVersion;                           //引导程序版本号
			uint16_t uiVersion;                                 //UI版本号
			char imei[16];                                      //IMEI号
			char iccid[21];                                      //ICCID号
			CAT_Type_e catType;									//当前使用的CAT模块类型
		}cmd2_2_4;
		
		//服务端中当前保存的各发布版本号信息，指令2.2.5
		__packed struct{
			uint16_t mainVersion;                               //主程序版本号
			uint16_t bootloadVersion;                           //引导程序版本号
			uint16_t uiVersion;                                 //UI版本号
		}cmd2_2_5;
		
		//设备向服务端发送修改仪器信息指令，指令2.2.8
		__packed struct{
			uint16_t infoType;                               	//用于表示当前要修改的信息类型
			uint8_t info[70];                          		 	//修改的内容值
		}cmd2_2_8;
		
		//服务端向设备响应当前服务端中的最新版本号，指令2.3.2
		__packed struct{
			uint8_t publishOrTestFlag;							//请求的是发布版还是测试版，为0表示请求发布版信息，为1表示请求测试版信息
			uint16_t mainVersion;
			uint16_t bootloadVersion;
			uint16_t uiVersion;
		}cmd2_3_2;

		
		//设备向服务端请求程序文件信息，指令2.3.3
		__packed struct{
			uint8_t publishOrTestFlag;							//请求的是发布版还是测试版，为0表示请求发布版信息，为1表示请求测试版信息
			uint8_t softwareType;								//请求的是主程序文件还是引导程序文件，为0表示请求主程序文件，为1表示请求引导程序文件
		}cmd2_3_3;
		
		//服务端响应设备请求的程序文件信息，指令2.3.4
		__packed struct{
			uint8_t publishOrTestFlag;	
			uint8_t softwareType;	
			uint32_t totalLen;
			uint16_t crc16;
		}cmd2_3_4;
		
		//设备向服务端请求指定版本的UI资源，指令2.3.5
		__packed struct{
			uint8_t publishOrTestFlag;							//请求的是发布版还是测试版，为0表示请求发布版信息，为1表示请求测试版信息
			uint16_t uiVersion;									//请求的UI资源版本号
		}cmd2_3_5;
		
		//服务端响应设备请求的指定版本号的UI资源，指令2.3.6
		__packed struct{
			uint8_t publishOrTestFlag;	
			uint16_t uiVersion;									
			uint8_t totalNum;
			
			__packed struct{
				uint32_t fileSize;
				uint16_t crc16;
				char path[70];
			}file[10];
		}cmd2_3_6;
		
		//服务端请求更正仪器参数信息，指令2.5.1
		__packed struct{
			uint16_t infoType;                               	//用于表示当前要修改的信息类型
			
			//修改的内容值
			__packed union{
				char zhensuoName[70];
				
				uint8_t lockFlag;								//锁定状态
				uint8_t wbcPara;                                //WBC增益补偿
				uint8_t hgbPara;                                //HGB增益补偿
				uint8_t pumpPara;                               //泵占空比值
				uint16_t current535;                            //535电流设定值
				uint16_t current415;                            //415电流设定值
				uint16_t outOC;                                 //出仓光耦补偿值
				uint16_t inOC;                                  //进仓光耦补偿值
				uint16_t gdOC;                                  //固定光耦补偿值
				uint16_t sfOC;                                  //释放光耦补偿值
				int16_t presurePara;                            //压力补偿值
				uint16_t wbcAdjust;                             //WBC校准系数值
				uint16_t hgbAdjust;                             //HGB校准系数值
				uint8_t pressureSensor;							//当前使用的压力传感器类型
				uint8_t company;								//当前仪器所属的公司
				char yongFuPsw[ACCOUNT_PSW_MAX_LEN];			//要修改YongFu登录密码
				char adminPsw[ACCOUNT_PSW_MAX_LEN];				//要修改admin登录密码
			}u;
		}cmd2_5_1;
		
		//设备响应服务端请求更正仪器参数信息，指令2.5.2
		__packed struct{
			uint16_t infoType;                               	//用于表示当前要修改的信息类型
			uint8_t result;                          		 	//设备响应的结果，为0表示修改失败，为1表示修改成功
		}cmd2_5_2;
		
		//设备响应服务端请求开启特殊功能项请求，指令2.5.4
		__packed struct{
			uint8_t itemFlag;									//用于表示要更改哪个参数的标志
			uint8_t resultFlag;									//用于表示仪器更改参数是否成功的标志
		}cmd2_5_4;
		
		//设备上传样本分析结果数据，指令2.6.1
		__packed struct{
			uint32_t sampleSN;									//样本编号，数值
			uint16_t errCode;                                   //故障码，数值
			uint16_t errHideCode;                               //隐藏故障码，数值
			uint8_t alarmFlag;									//报警值标志
			char orderNum[ORDER_NUM_MAX_LEN];					//订单编号字符串
			char patientID[PATIENT_ID_MAX_LEN];					//患者编号字符串
			char name[30];										//姓名
			SexType_e gender;                                   //性别
			uint8_t age;                                        //年龄
			AgeUnit_e ageUnit;                                  //年龄单位
			AccountType_e account;                              //账户
			long timestamp;                                     //测试时间戳
			uint8_t refGroups;									//参考组
																
			uint16_t wbc;                                       
			uint16_t gran;                                      
			uint16_t mid;                                       
			uint16_t lym;                                       
			uint16_t granPrecent;                               
			uint16_t midPrecent;                                
			uint16_t lymPrecent;                                
																
			uint16_t hgb;                                       
																
			uint16_t firstX;                                    //第一条分割线
			uint16_t secondX;                                   //第二条分割线
			uint16_t thirdX;                                    //第三条分割线
																
			uint16_t maxFL;                                     //最大飞升值
																
			uint16_t pictValue[256];                            //256通道的直方图数据
			
			char wbcErrInfo[170];								//WBC错误提示信息
			char hgbErrInfo[110];								//HGB错误提示信息
			
			//报警标志
			WBCHGB_ALERT_e wbcAlarmFlag;                                       
			WBCHGB_ALERT_e granAlarmFlag;                                      
			WBCHGB_ALERT_e midAlarmFlag;                                       
			WBCHGB_ALERT_e lymAlarmFlag;                                       
			WBCHGB_ALERT_e granPrecentAlarmFlag;                               
			WBCHGB_ALERT_e midPrecentAlarmFlag;                                
			WBCHGB_ALERT_e lymPrecentAlarmFlag;                                
			WBCHGB_ALERT_e hgbAlarmFlag;
		}cmd2_6_1;
		
		
		//设备上传修改的样本分析结果数据，指令2.6.2
		__packed struct{
			uint32_t sampleSN;									//样本编号，数值
			uint8_t alarmFlag;									//报警值标志
			char orderNum[ORDER_NUM_MAX_LEN];					//订单编号字符串
			char patientID[PATIENT_ID_MAX_LEN];					//患者编号字符串
			char name[30];										//姓名
			SexType_e gender;                                   //性别
			uint8_t age;                                        //年龄
			AgeUnit_e ageUnit;                                    //年龄单位
			AccountType_e account;                              //账户
			uint8_t refGroups;									//参考组
																
			uint16_t wbc;                                       
			uint16_t gran;                                      
			uint16_t mid;                                       
			uint16_t lym;                                       
			uint16_t granPrecent;                               
			uint16_t midPrecent;                                
			uint16_t lymPrecent;                                
																
			uint16_t hgb;                                       
			
			//报警标志
			WBCHGB_ALERT_e wbcAlarmFlag;                                       
			WBCHGB_ALERT_e granAlarmFlag;                                      
			WBCHGB_ALERT_e midAlarmFlag;                                       
			WBCHGB_ALERT_e lymAlarmFlag;                                       
			WBCHGB_ALERT_e granPrecentAlarmFlag;                               
			WBCHGB_ALERT_e midPrecentAlarmFlag;                                
			WBCHGB_ALERT_e lymPrecentAlarmFlag;                                
			WBCHGB_ALERT_e hgbAlarmFlag;
		}cmd2_6_2;
		
		
		//样本分析日志，指令2.7.1
		__packed struct{
			uint32_t sampleSN;									//样本编号，数值
			uint16_t errCode;                                   //故障码，数值
			uint16_t errHideCode;                               //隐藏故障码，数值
			uint8_t alarmFlag;									//报警值标志
			uint8_t errType;                                    //故障分类信息
			AccountType_e account;                              //账户
			long timestamp;                                     //测试时间戳
			
			char logBuf[];										//日志字符串
		}cmd2_7_1;
		
		//运行日志，指令2.7.2
		__packed struct{
			AccountType_e account;                              //账户
			long timestamp;                                     //测试时间戳
			
			char logBuf[];										//日志字符串
		}cmd2_7_2;
		
		//设备向服务端请求指定患者编号的患者信息，指令2.8.1
		__packed struct{
			char patientID[PATIENT_ID_MAX_LEN];					//患者编号字符串
			CAT_TestType_e catTestType;							//测试的项目类型											
		}cmd2_8_1;
		
		//服务端向设备返回请求的指定患者编号的患者信息，指令2.8.2
		__packed struct{
			char orderNum[ORDER_NUM_MAX_LEN];					//订单编号
			char patientID[PATIENT_ID_MAX_LEN];					//患者编号字符串
			CAT_TestType_e catTestType;							//测试的项目类型
			char name[30];										//姓名
			uint8_t age;                                        //年龄
			AgeUnit_e ageUnit;                                  //年龄单位
			SexType_e gender;                                   //性别
		}cmd2_8_2;
		
		//服务端向设备返回请求的指定患者编号不存在，指令2.8.3
		__packed struct{
			char patientID[PATIENT_ID_MAX_LEN];					//患者编号字符串
			CAT_TestType_e catTestType;							//测试的项目类型
		}cmd2_8_3;
		
		
		
		
		
		
//===跟第三方服务端进行通讯的指令集===
		//仪器当前开机信息，指令A.2.2.1
		__packed struct{
			uint32_t totalOpenCount;							//当前开机总次数
			char position[22];									//位置信息
			uint8_t signalStrength;								//信号强度
		}cmdA_2_2_1;
		
		//仪器网络状态信息，指令A.2.2.4
		__packed struct{
			uint32_t interruptTimestamp;						//网络中断时间戳
			uint32_t recoveryTimestamp;							//网络中断时间戳
			uint8_t interruptSignalStrength;					//网络中断时信号强度
			uint8_t recoverySignalStrength;						//网络中断时信号强度
		}cmdA_2_2_4;
		
		//设备上传样本分析结果数据，指令A.2.3.1
		__packed struct{
			char orderNum[ORDER_NUM_MAX_LEN];					//订单编号
			CAT_TestType_e catTestType;							//测试的项目类型
			uint32_t sampleSN;									//样本编号，数值
			uint16_t errCode;                                   //故障码，数值
			uint16_t errHideCode;                               //隐藏故障码，数值
			uint8_t alarmFlag;									//报警值标志
			char patientID[PATIENT_ID_MAX_LEN];					//患者编号字符串
			char name[30];										//姓名
			SexType_e gender;                                   //性别
			uint8_t age;                                        //年龄
			AgeUnit_e ageUnit;                                  //年龄单位
			AccountType_e account;                              //账户
			long timestamp;                                     //测试时间戳
			uint8_t refGroups;									//参考组
																
			uint16_t wbc;                                       
			uint16_t gran;                                      
			uint16_t mid;                                       
			uint16_t lym;                                       
			uint16_t granPrecent;                               
			uint16_t midPrecent;                                
			uint16_t lymPrecent;                                
																
			uint16_t hgb;                                       
																
			uint16_t firstX;                                    //第一条分割线
			uint16_t secondX;                                   //第二条分割线
			uint16_t thirdX;                                    //第三条分割线
																
			uint16_t maxFL;                                     //最大飞升值
																
			uint16_t pictValue[256];                            //256通道的直方图数据
			
			char wbcErrInfo[170];								//WBC错误提示信息
			char hgbErrInfo[110];								//HGB错误提示信息
			
			//报警标志
			WBCHGB_ALERT_e wbcAlarmFlag;                                       
			WBCHGB_ALERT_e granAlarmFlag;                                      
			WBCHGB_ALERT_e midAlarmFlag;                                       
			WBCHGB_ALERT_e lymAlarmFlag;                                       
			WBCHGB_ALERT_e granPrecentAlarmFlag;                               
			WBCHGB_ALERT_e midPrecentAlarmFlag;                                
			WBCHGB_ALERT_e lymPrecentAlarmFlag;                                
			WBCHGB_ALERT_e hgbAlarmFlag;
		}cmdA_2_3_1;
		
		//第三方服务端向设备发送样本结果数据已收到消息
		__packed struct{
			CAT_TestType_e catTestType;							//测试的项目类型
			uint32_t sampleSN;									//样本编号
		}cmdA_2_3_3;
		
		//样本分析故障日志，指令A.2.4.1
		__packed struct{
			uint32_t sampleSN;									//样本编号，数值
			uint16_t errCode;                                   //故障码，数值
			uint16_t errHideCode;                               //隐藏故障码，数值
			uint8_t alarmFlag;									//报警值标志
			uint8_t errType;                                    //故障分类信息
			AccountType_e account;                              //账户
			long timestamp;                                     //测试时间戳
			CAT_TestType_e catTestType;							//测试的项目类型
		}cmdA_2_4_1;
		
		//第三方服务端向设备发送故障日志已收到消息
		__packed struct{
			CAT_TestType_e catTestType;							//测试的项目类型
			uint32_t logSN;										//故障日志的样本编号
		}cmdA_2_4_2;
		
		//设备向第三方服务端请求患者信息
		__packed struct{
			CAT_TestType_e catTestType;							//测试的项目类型
			uint8_t pageNum;									//患者信息页数，从第一页开始计算
		}cmdA_2_5_1;
		
		//第三方服务端向设备返回请求患者信息列表
		__packed struct{
			CAT_TestType_e catTestType;							//测试的项目类型
			uint8_t pageNum;									//页号，从第一页开始计算
			uint8_t num;										//当前页中包含的患者信息个数
			
			__packed struct{
				char 	name[PATIENT_NAME_LEN];					//姓名
				uint8_t 	age;								//年龄
				AgeUnit_e 	eAgeUnit;							//年龄单位
				SexType_e 	eSex;								//性别
				char 	orderNum[ORDER_NUM_MAX_LEN];			//订单编号
			}info[5];
		}cmdA_2_5_2;
		
		//设备向第三方服务端发送即将开始测试的患者订单编号
		__packed struct{
			CAT_TestType_e 	catTestType;						//测试的项目类型
			char 	orderNum[ORDER_NUM_MAX_LEN];				//订单编号
		}cmdA_2_5_3;
		
		//第三方服务端请求更正仪器参数值
		__packed struct{
			uint8_t infoType;                               	//用于表示当前要修改的信息类型
			
			//修改的内容值
			__packed union{
				char adminPsw[ACCOUNT_PSW_MAX_LEN];				//要修改admin登录密码
			}u;
		}cmdA_2_6_2;
		
		//设备响应第三方服务端请求更正仪器参数值
		__packed struct{
			uint8_t infoType;                               	//用于表示当前要修改的信息类型
			uint8_t result;                          		 	//设备响应的结果，为0表示修改失败，为1表示修改成功
		}cmdA_2_6_3;
		
	}payload;
}MQTT_PayloadBuf_s;






uint16_t CRC16(uint8_t* Buf,uint32_t Len);

HAL_StatusTypeDef CAT_RegistNet(void);
HAL_StatusTypeDef CAT_GetSignalStrength(void);
HAL_StatusTypeDef CAT_GetNetTime(RTC_HYM8563Info_s* CurrentNetTime);
HAL_StatusTypeDef CAT_GetPosition(void);
HAL_StatusTypeDef CAT_OpenNet(void);
HAL_StatusTypeDef CAT_IsOK(void);
HAL_StatusTypeDef CAT_ReadType(void);
HAL_StatusTypeDef CAT_InitConfig(void);

HAL_StatusTypeDef CAT_M_ConfigClient(CAT_SERVICE_e CatService,char* ClientID,char* UserName,char* Password,char* WillTopic,char* WillMessage);
HAL_StatusTypeDef CAT_M_CreateTcpConn(char* HostName,uint16_t Port);
HAL_StatusTypeDef CAT_M_CreateMQTTConn(void);
HAL_StatusTypeDef CAT_M_CloseMQTTConn(void);
HAL_StatusTypeDef CAT_M_SubTopic(char* Topic);
HAL_StatusTypeDef CAT_M_PubTopic(char* Topic,uint8_t Qos,uint16_t SendBufLen,uint8_t* SendBuf);

HAL_StatusTypeDef CAT_Send_MachineAllPara(void);
HAL_StatusTypeDef CAT_B_CreateMQTTConn(CAT_SERVICE_e CatService);
HAL_StatusTypeDef CAT_B_CloseMQTTConn(void);
HAL_StatusTypeDef CAT_B_Send_MachineFactoryInfo(void);
HAL_StatusTypeDef CAT_B_Send_MachineOpenInfo(void);
HAL_StatusTypeDef CAT_B_Send_SignalStrength(void);
HAL_StatusTypeDef CAT_B_Send_ChangeMachineInfo(uint8_t TypeInfo,uint8_t* Info,uint8_t Len);
HAL_StatusTypeDef CAT_B_Send_RespondSoftwareInfo(uint8_t PublishOrTestFlag,uint8_t SoftwareType);
HAL_StatusTypeDef CAT_B_Send_RespondUIInfo(uint8_t PublishOrTestFlag,uint16_t UIVersion);
HAL_StatusTypeDef CAT_Send_MachineSelfCheck(uint8_t* Buf,uint8_t Len);
HAL_StatusTypeDef CAT_B_Send_RespondAllSoftwareVersion(uint8_t PublishOrTestFlag);

HAL_StatusTypeDef CAT_B_IsServiceWantToSendData(void);

HAL_StatusTypeDef CAT_B_Send_SampleInfo(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead);
HAL_StatusTypeDef CAT_B_Send_ChangeSampleInfo(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead);
HAL_StatusTypeDef CAT_B_Send_SampleLog(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead,uint8_t* pCountLogBuffer, uint16_t usCountLogLen);
HAL_StatusTypeDef CAT_B_Send_RunLog(RunLog_s* RunLog);
HAL_StatusTypeDef CAT_B_Send_RequestPatientInfo(char* PatientID);

HAL_StatusTypeDef CAT_ConnFTP(void);
HAL_StatusTypeDef CAT_FTPGetFile(FTPDownloadData_s* FTPDownLoadData,char* Path,uint32_t FileOffset);



HAL_StatusTypeDef CAT_B_Other_Send_MachineOpenInfo(void);
HAL_StatusTypeDef CAT_B_Other_Send_SignalStrength(void);
HAL_StatusTypeDef CAT_Other_Send_MachineSelfCheck(uint8_t* Buf,uint8_t Len);
HAL_StatusTypeDef CAT_B_Other_Send_SampleLog(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead);
HAL_StatusTypeDef CAT_B_Other_Send_SampleInfo(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead);
HAL_StatusTypeDef CAT_B_Other_Send_RequestPatientInfo(uint8_t PageNum);
HAL_StatusTypeDef CAT_B_Other_IsServiceWantToSendData(void);
HAL_StatusTypeDef CAT_B_Other_Respond_ChangeSetInfo(uint16_t InfoType,uint8_t Flag,uint16_t* TotalLen,MQTT_PayloadBuf_s* MQTTPayloadBuf);
HAL_StatusTypeDef CAT_B_Other_Send_ReadyTestOrderNum(char* OrderNum);
HAL_StatusTypeDef CAT_B_Other_Send_NetStatus(void);


#endif
