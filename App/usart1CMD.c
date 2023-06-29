/************************************************************
*串口1输入命令相关处理
*************************************************************/
#include "usart1CMD.h"
#include "cmsis_os2.h"
#include "usart.h"
#include "bsp_nand.h"
#include "bsp_eeprom.h"
#include "parameter.h"
#include "bsp_countsignal.h"
#include "cmsis_os.h"
#include "process.h"
#include "bsp_light.h"
#include "monitorV.h"
#include "Common.h"





/*全局变量定义区*********************************************/
Usart1ReceBuf_s Usart1ReceBuf = {0};

//串口1接收数据二值信号量
osSemaphoreId_t Usart1RxSempBinHandle = NULL;




/*内部函数区*******************************************************/
/***
*获取数值
返回数值为:0xFFFFFFFF时表示获取数值失败
***/
static uint32_t _CMD_GetValue(uint16_t Index)
{
	extern Usart1ReceBuf_s Usart1ReceBuf;
	uint32_t Value = 0;
	
	while(Index != Usart1ReceBuf.writeIndex){
		if(Usart1ReceBuf.buf[Index] == '_'){
			break;
		}
		
		if(Usart1ReceBuf.buf[Index] < '0' || Usart1ReceBuf.buf[Index] > '9'){
			return 0xFFFFFFFF;
		}
		
		if(Value > 100000){
			return 0xFFFFFFFF;
		}
		
		Value = Value*10+(Usart1ReceBuf.buf[Index]-'0');
		Index = (Index+1)&USART1_RECE_BUF_LEN;
	}
	
	if(Value == 0){
		return 0xFFFFFFFF;
	}
	
	Usart1ReceBuf.readIndex = Index;
	
	return Value;
}



/***
*计数相关指令操作
***/
static void _CMD_CountSignal(uint16_t Mode,uint16_t Pluse)
{
	extern __IO osSemaphoreId xSema_Count_Key;
	float fRadio = 0.0;
	
	switch(Pluse){
		case 1:fRadio = 0.5;break;
		case 2:fRadio = 1.0;break;
		case 3:fRadio = 20.0;break;
		default :fRadio = 0.0;break;
	}
	
	//为0关闭工装信号，
    if(Mode==6 || Mode == 7 || Mode == 11) //内部工装模式
    {
        if(fRadio == 0)
        {
            //接通板载工装信号
            COUNT_SIGNAL_SW_OFF;
            //关闭信号
            Count_Signal_Stop();
            //
            osSemaphoreAcquire(xSema_Count_Key, 0); 
            Set_CountMode(COUNT_MODE_NORMAL);
        }else{
            //接通板载工装信号
            COUNT_SIGNAL_SW_ON;
            //设置板载工装信号的*空比，启动信号
            Count_Signal_SetFreq(fRadio);
            Count_Signal_Start();
        }
    }
    
    //计数模式
    if(Mode == 1){
        //正常计数流程
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_NORMAL;
        osSemaphoreRelease(xSema_Count_Key);  
    }else if(Mode == 2){
        //即：计数池重复使用模式）
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_RE_USE;
        osSemaphoreRelease(xSema_Count_Key);  
    }else if(Mode == 3){
        //开启灼烧模式，当出现堵孔，开启100V灼烧
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_100V_FIRE;
        osSemaphoreRelease(xSema_Count_Key);  
    }else if(Mode == 4){
        //外部工装信号仿真模拟测试，全流程模式
     //   Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIMULATION_PROCESS_OUTSIDE;
      //  osSemaphoreRelease(xSema_Count_Key);  
    }else if(Mode == 5){
        //外部工装信号仿真模拟测试,纯信号模式
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE;
        osSemaphoreRelease(xSema_Count_Key);  
    }else if(Mode == 6){   
        //内部工装信号仿真模拟测试，自动模式      
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO;
        osSemaphoreRelease(xSema_Count_Key);  
     }else if(Mode == 7){
        //内部工装信号仿真模拟测试, 单次纯信号模式
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIMULATION_SIGNAL_INNER;
        osSemaphoreRelease(xSema_Count_Key);
    }else if(Mode == 8){
         //信号源模式
         Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIGNAL_SRC;
        osSemaphoreRelease(xSema_Count_Key);
    }else if(Mode == 9){
         //计数：外部工装信号仿真模拟测试,纯信号,自动执行模式
         Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO;
         osSemaphoreRelease(xSema_Count_Key); 
    }else if(Mode == 10){
        //HGB
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_HGB;
        osSemaphoreRelease(xSema_Count_Key);
    }else if(Mode == 11){
        //质控内部工装信号仿真模拟测试，自动模式      
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_QC_INNER_AUTO;
        osSemaphoreRelease(xSema_Count_Key); 
    }else{
		printf("CMD input error!!!\r\n");
	}
}



/***
*输出指定样本数据或者日志
***/
static void _CMD_OutputDataLog(uint16_t Mode,uint32_t SampleSN)
{
	uint32_t ulQueueMaxNum = 0, i = 0;

	ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
	SampleSN = (SampleSN%ulQueueMaxNum);

	if(Mode == 1)
	{
		WBCHGB_TestInfo_t* tWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
		
		//读取WBCHGB数据
		FatFs_Read_WBCHGB_Info(SampleSN, tWBCHGB_TestInfo);
		Print_WBCHGB_TestInfo(tWBCHGB_TestInfo);
	}else if(Mode == 2){
		//读取WBCHGB日志
		CountLog_t* ptCountLog = (CountLog_t*)SDRAM_ApplyCommonBuf(sizeof(CountLog_t));
		memset((void*)ptCountLog, 0, LOG_INFO_MAX_LEN);
		LOG_Info("Len=%d, Crc=%d", ptCountLog->usLogLen, ptCountLog->usCrc);
		FatFs_Read_ConutLog(SampleSN, ptCountLog, LOG_INFO_MAX_LEN);
		for(i = 0; i < ptCountLog->usLogLen; i++)
		{
			printf("%c", ptCountLog->ucaLogBuffer[i]);
		}
	}else if(Mode == 3){
		//公共头信息
		CountHead_t* tCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
		FatFs_Read_ConutHead(SampleSN, tCountHead);
		Print_Count_Head(tCountHead);
	}else{
		printf("CMD input error!!!\r\n");
	}
}



/***
*双波长灯HGB测试
***/
static void _CMD_2LEDHGBTest(uint32_t Current535,uint32_t Current415)
{
	uint32_t Value535 = 0;
	uint32_t Value415 = 0;
	
	//测试535波长
	Light_Start(HGBLED_535,Current535);
	osDelay(5000);
	Value535 = HGB_ADC();
	Light_Stop();
	
	osDelay(500);
	
	//测试415波长
	Light_Start(HGBLED_415,Current415);
	osDelay(5000);
	Value415 = HGB_ADC();
	Light_Stop();
	
	printf("535ADC:%u,,,415ADC:%u\r\n",Value535,Value415);
}









/*外部函数区*******************************************************/
/***
*串口1接收数据处理任务
***/
void Usart1CMDTask(void *argument)
{
	extern osSemaphoreId_t Usart1RxSempBinHandle;
	extern Usart1ReceBuf_s Usart1ReceBuf;
	extern  MachInfo_s	MachInfo;
	
	uint32_t Value = 0;
	uint32_t Value2 = 0;
	
	while(1){
		osSemaphoreAcquire(Usart1RxSempBinHandle,osWaitForever);
		
		while(Usart1ReceBuf.readIndex != Usart1ReceBuf.writeIndex){
			//判断帧头AT:
			if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != 'A'){
				Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
				continue;
			}else{
				Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
			}
			
			if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != 'T'){
				Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
				continue;
			}else{
				Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
			}
			
			if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != ':'){
				Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
				continue;
			}else{
				Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
			}
			
			//执行内容
			switch(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex]){
				//打印所有的命令
				case 'G':
				{
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\r'){
						break;
					}
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\n'){
						break;
					}
					
					//打印出所有的命令
					printf("***ALL CMD************************************\r\n");
					printf("AT:G\t获取所有命令\r\n");
					printf("AT:C_X_\t清除参数：X为1：清除仪器参数；X为2：清除触摸参数；X为3：清除数据管理参数\r\n");
					printf("AT:S_X_\t打印状态参数，X为1：打印任务栈信息\r\n");
					printf("AT:J_X_Y_\t开启计数\r\n \
					X为1：正常计数；X为2：计数池Reuse；X为3：开启灼烧； \r\n \
					X为4：外工装全流程；X为5：外工装纯信号；X为6：内工装自动; \r\n \
					X为7：内工装单次纯信号；X为8：信号源模式；X为9：外工装纯信号自动执行； \r\n \
					X为10：HGB；X为11：质控内工装自动\r\n \
					Y为内部工装信号占空比，其他模式时Y为0即可，Y为1：0.5；为2：1；为3：20\r\n");
					printf("AT:P_X_Y_\t输出测试数据或日志，X为1：输出测试数据；X为2：输出日志；X为3：输出公共头；Y表示样本编号\r\n");
					printf("AT:T_X_Y_\t双波长HGB测试，X：535波长灯电流值；Y：415波长灯电流值\r\n");
					printf("***CMD END************************************\r\n");
				}break;
				
				//清除参数
				case 'C':
				{
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '_'){
						break;
					}
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					Value = _CMD_GetValue(Usart1ReceBuf.readIndex);
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\r'){
						break;
					}
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\n'){
						break;
					}
					
					switch(Value){
						//清除仪器参数
						case 1:
						{
							MachInfo.initFlag = 0;
							EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_INIT_FLAG,(uint8_t*)&MachInfo.initFlag,sizeof(MachInfo.initFlag));
							
							osDelay(1000);
					
							//重新登录
							__set_PRIMASK(1);
							NVIC_SystemReset();
						}break;
						
						//清除触摸参数
						case 2:
						{
							MachInfo.touch.flag = 0;
							EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_MACHINFO_INIT_TOUCH,(uint8_t*)&MachInfo.touch,sizeof(MachInfo.touch));
							
							osDelay(1000);
					
							//重新登录
							__set_PRIMASK(1);
							NVIC_SystemReset();
						}break;
						
						//清除数据管理参数
						case 3:
						{
							Set_Default_DataManage(&g_tDataManage);
							Save_DataMange_Info(&g_tDataManage);
							
							osDelay(1000);
					
							//重新登录
							__set_PRIMASK(1);
							NVIC_SystemReset();
						}break;
						
						case 0xFFFF:printf("CMD input error!!!\r\n");break;
						default :printf("CMD input error!!!\r\n");break;
					}
				}break;
				
				//打印状态信息
				case 'S':
				{
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '_'){
						break;
					}
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					Value = _CMD_GetValue(Usart1ReceBuf.readIndex);
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\r'){
						break;
					}
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\n'){
						break;
					}
					
					switch(Value){
						//打印任务栈信息
						case 1:
						{
							Printf_Free_StackSize();
						}break;
						
						case 0xFFFF:printf("CMD input error!!!\r\n");break;
						default :printf("CMD input error!!!\r\n");break;
					}
				}break;
				
				//计数相关
				case 'J':
				{
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '_'){
						break;
					}
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					Value = _CMD_GetValue(Usart1ReceBuf.readIndex);
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					Value2 = _CMD_GetValue(Usart1ReceBuf.readIndex);
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\r'){
						break;
					}
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\n'){
						break;
					}
					
					if(Value == 0xFFFFFFFF || Value2 == 0xFFFFFFFF){
						printf("CMD input error!!!\r\n");
						break;
					}
					
					_CMD_CountSignal(Value,Value2);
				}break;
				
				//输出数据或者日志
				case 'P':
				{
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '_'){
						break;
					}
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					Value = _CMD_GetValue(Usart1ReceBuf.readIndex);
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					Value2 = _CMD_GetValue(Usart1ReceBuf.readIndex);
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\r'){
						break;
					}
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\n'){
						break;
					}
					
					if(Value == 0xFFFFFFFF || Value2 == 0xFFFFFFFF){
						printf("CMD input error!!!\r\n");
						break;
					}
					
					_CMD_OutputDataLog(Value,Value2);
				}break;
				
				//双波长HGB测试
				case 'T':
				{
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '_'){
						break;
					}
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					Value = _CMD_GetValue(Usart1ReceBuf.readIndex);
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					Value2 = _CMD_GetValue(Usart1ReceBuf.readIndex);
					
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\r'){
						break;
					}
					Usart1ReceBuf.readIndex = (Usart1ReceBuf.readIndex+1)&USART1_RECE_BUF_LEN;
					if(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex] != '\n'){
						break;
					}
					
					if(Value == 0xFFFFFFFF || Value2 == 0xFFFFFFFF){
						printf("CMD input error!!!\r\n");
						break;
					}
					
					_CMD_2LEDHGBTest(Value,Value2);
				}break;
				
				default :
					printf("CMD input error!!!\r\n");
				break;
			}
		}
	}
}

















