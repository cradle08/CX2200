/**************************************************************************
CAT模块通讯相关任务，串口4开启空闲中断，DMA收发，并且创建一个二值信号量，
用来中空闲中断中监听是否接受到的数据了；创建一个计数信号量用于防止发送数据被覆盖，
创建一个消息队列，用来传递要发送的数据信息
***************************************************************************/
#include "CAT_iotTask.h"
#include "CAT_iot.h"
#include "bsp_eeprom.h"
#include "bsp_outin.h"
#include "cmsis_os2.h"
#include "file_operate.h"
#include "Public_menuDLG.h"
#include "bsp_spi.h"
#include "flash.h"
#include "parameter.h"
#include "Common.h"


/*宏定义区*****************************************************************/
#define CAT_UPDATE_STATUS_TIME_INTERVAL					20000					//用于表示CAT定时更新状态的任务的时间间隔，单位ms
#define CAT_TIME_INTERVAL_ALARM							3600000					//用于表示定时唤醒CAT连接服务端的任务时间间隔，单位ms
#define SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET				30*1024*1024			//定义用于在SPI_FLASH中存放升级包数据的起始偏移量







/*全局变量定义区************************************************************/
osSemaphoreId_t CATSemphrCountTxHandle  = NULL;						//CAT计数信号量
osSemaphoreId_t CATSemphrBinRxHandle    = NULL;						//CAT二值信号量

osMessageQueueId_t CATQueueHandle = NULL;							//用于通知CAT1发布消息的消息队列

uint32_t AlarmCATTimeupCount = 0;									//用于保存定时唤醒CAT连接服务端的时间间隔计数值

WM_HWIN DialogWin = NULL;											//用于保存升级弹窗的句柄

//第三方服务端相关
osSemaphoreId_t CATSemMutexHandle  = NULL;							//CAT计数信号量，用于存在和第三方服务端进行通讯时，起到和创怀服务端之间进行互斥的作用，避免数据串扰
osMessageQueueId_t CATQueueOtherHandle = NULL;						//用于通知CAT1给第三方服务端发布消息的消息队列


/*外部函数区****************************************************************/
/***
*版本转换，字符串转成整数
***/
uint16_t VersionStr2Int(char* StrBuf)
{
	uint16_t VersionInt = 0;
	
	VersionInt += (*(StrBuf+1)-'0')*100;
	VersionInt += (*(StrBuf+3)-'0')*10;
	VersionInt += *(StrBuf+5)-'0';
	
	return VersionInt;
}



/***
*版本转换，整数转换字符串
***/
void VersionInt2Str(uint16_t Version,char* VersionStr)
{
	uint8_t StrLen = 0;
	
	if(Version>999){
		Version = 999;
	}
	
	StrLen = sprintf(VersionStr,"V%u.%u.%u",Version/100,(Version%100)/10,(Version%100)%10);
	*(VersionStr+StrLen) = '\0'; 
}



/***
*路径转换，将路径中的/转换成\\
*参数：
	DesPath：目标路径
	SrcPath：源路径
***/
static void PathChange(char* DesPath,char* SrcPath)
{
	uint8_t Len = 0;
	char* StrP1 = NULL;
	
	if(SrcPath == NULL){
		return;
	}
	
	StrP1 = strtok(SrcPath,"/");
	
	while(1){
		if(StrP1 == NULL){
			break;
		}
		
		Len += sprintf(DesPath+Len,"%s\\",StrP1);
		
		StrP1 = strtok(NULL,"/");
	}
	
	*(DesPath + Len - 1) = '\0';
	
	return;
}






/***
*主动点击升级主程序，引导程序，UI资源包到发布版或者测试版，强制升级，不判断版本号，注意测试版不升级UI资源
参数：
	VersionType：为0表示主动请求发布版，为1表示主动请求测试版
	UpdateType：为0表示升级主程序，为1表示升级引导程序，为2表示升级UI
***/
static HAL_StatusTypeDef _CAT_UpdateSoftware(uint8_t VersionType,uint8_t UpdateType)
{
	extern CATGlobalStatus_s CATGlobalStatus;
	extern MachInfo_s MachInfo;
	
	FTPDownloadData_s* FTPDownLoadData = (FTPDownloadData_s*)SDRAM_ApplyCommonBuf(sizeof(FTPDownloadData_s));
	CommonDialogPageData_s DialogPageData = {0};
	
	WM_MESSAGE pMsg;
	char TempPath[50] = {0};
	char TempPath2[50] = {0};
	extern osThreadId_t CATTaskHandle;
	
		
	//请求服务端指定发布版或者测试版的各模块最新版本号
	if(CAT_B_Send_RespondAllSoftwareVersion(VersionType) != HAL_OK){
		//弹窗提示
		DialogPageData.bmpType = BMP_ALARM;
		DialogPageData.confimButt = HAS_CONFIM_BUTTON;
		DialogPageData.fun = NULL;
		strcpy(DialogPageData.str,"联网失败，重启后再试");
		
		pMsg.MsgId = WM_USER_DATA;
		pMsg.Data.p = &DialogPageData;
		WM_SendMessage(DialogWin,&pMsg);
		
		return HAL_ERROR;
	}
	
	//判断执行升级主程序还是引导程序或者UI资源
	switch(UpdateType){
		//升级主程序
		case 0:
		{
			//升级主程序
			if(CAT_B_Send_RespondSoftwareInfo(VersionType,0) != HAL_OK){
				//弹窗提示
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = NULL;
				strcpy(DialogPageData.str,"联网失败，稍后重试");
				
				pMsg.MsgId = WM_USER_DATA;
				pMsg.Data.p = &DialogPageData;
				WM_SendMessage(DialogWin,&pMsg);
				
				return HAL_ERROR;
			}else{
				//判断服务器响应的参数是否正确
				if(CATGlobalStatus.softwareInfo.publishOrTestFlag == VersionType && CATGlobalStatus.softwareInfo.softwareType == 0){
					//开始建立FTP连接，临时关闭MQTT连接
					if(CATGlobalStatus.flag.mqttConn == 1){
						CAT_B_CloseMQTTConn();
					}
					
					//建立FTP连接
					if(CAT_ConnFTP() != HAL_OK){
						//FTP连接建立失败，弹出窗即可，不能挂起任务
						//弹窗提示
						DialogPageData.bmpType = BMP_ALARM;
						DialogPageData.confimButt = HAS_CONFIM_BUTTON;
						DialogPageData.fun = NULL;
						strcpy(DialogPageData.str,"FTP连接建立失败");
						
						pMsg.MsgId = WM_USER_DATA;
						pMsg.Data.p = &DialogPageData;
						WM_SendMessage(DialogWin,&pMsg);
						
						//这里直接退出强制升级流程，并且不能让上级调用挂起
						return HAL_OK;
					}else{
						//下载文件
						memset(TempPath,0,50);
						if(VersionType == 0){
							sprintf(TempPath,"/ftp/code/main_v%03u.bin",CATGlobalStatus.serviceVersion.PMainVersion);
						}else{
							sprintf(TempPath,"/ftp/test_code/main_v%03u.bin",CATGlobalStatus.serviceVersion.TMainVersion);
						}
						
						if(CAT_FTPGetFile(FTPDownLoadData,TempPath,0) != HAL_OK){
							//文件下载失败，弹窗即可，不能挂起任务
							//弹窗提示
							DialogPageData.bmpType = BMP_ALARM;
							DialogPageData.confimButt = HAS_CONFIM_BUTTON;
							DialogPageData.fun = NULL;
							strcpy(DialogPageData.str,"主程序文件下载失败");
							
							pMsg.MsgId = WM_USER_DATA;
							pMsg.Data.p = &DialogPageData;
							WM_SendMessage(DialogWin,&pMsg);
							
							//这里直接退出强制升级流程，并且不能让上级调用挂起
							return HAL_OK;
						}else{
							//下载文件成功，校验文件
							if(CATGlobalStatus.softwareInfo.totalLen != FTPDownLoadData->totalLen || FTPDownLoadData->totalLen > 1024*1024 || CRC16(FTPDownLoadData->data,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
								//文件校验异常，弹窗...
								//弹窗提示
								DialogPageData.bmpType = BMP_ALARM;
								DialogPageData.confimButt = HAS_CONFIM_BUTTON;
								DialogPageData.fun = NULL;
								strcpy(DialogPageData.str,"主程序文件校验失败");
								
								pMsg.MsgId = WM_USER_DATA;
								pMsg.Data.p = &DialogPageData;
								WM_SendMessage(DialogWin,&pMsg);
								
								//这里直接退出强制升级流程，并且不能让上级调用挂起
								return HAL_OK;
							}else{
								//校验成功，写入文件到存储设备
								FTPDownLoadData->flag = 0xA55A0FF0;
								FTPDownLoadData->crc16 = CATGlobalStatus.softwareInfo.crc16;
								
								SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
								if(SPIFlash_WriteData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)FTPDownLoadData,12+FTPDownLoadData->totalLen) == SUCCESS){ 
									//写入成功，重新读取出来，判断CRC是否一致
									if(SPIFlash_DMA_ReadData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)FTPDownLoadData,12+FTPDownLoadData->totalLen) == SUCCESS){
                                        SPI4_ExitModeToDefault();//
										if(CRC16((uint8_t*)FTPDownLoadData->data,FTPDownLoadData->totalLen) == CATGlobalStatus.softwareInfo.crc16){
											//往EEPROM做好BOOT升级MAIN程序的标志
											MachRunPara.update.needBootToUpdateMainFlag = UPDATE_MAIN_SOFTWARE_FLAG;
											EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_UPDATE_MAIN_SOFTWARE_FLAG,(uint8_t*)&MachRunPara.update.needBootToUpdateMainFlag,sizeof(MachRunPara.update.needBootToUpdateMainFlag));
										}else{
											//弹窗提示
											DialogPageData.bmpType = BMP_ALARM;
											DialogPageData.confimButt = HAS_CONFIM_BUTTON;
											DialogPageData.fun = NULL;
											strcpy(DialogPageData.str,"主程序文件写入后校验失败");
											
											pMsg.MsgId = WM_USER_DATA;
											pMsg.Data.p = &DialogPageData;
											WM_SendMessage(DialogWin,&pMsg);
											
											//这里直接退出强制升级流程，并且不能让上级调用挂起
											return HAL_OK;
										}
									}
                                    SPI4_ExitModeToDefault();//
								}else{
                                    SPI4_ExitModeToDefault();//
									//弹窗提示
									DialogPageData.bmpType = BMP_ALARM;
									DialogPageData.confimButt = HAS_CONFIM_BUTTON;
									DialogPageData.fun = NULL;
									strcpy(DialogPageData.str,"主程序文件写入失败");
									
									pMsg.MsgId = WM_USER_DATA;
									pMsg.Data.p = &DialogPageData;
									WM_SendMessage(DialogWin,&pMsg);
									
									//这里直接退出强制升级流程，并且不能让上级调用挂起
									return HAL_OK;
								}
								SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
							}
						}
					}
				}else{
					//弹窗提示
					DialogPageData.bmpType = BMP_ALARM;
					DialogPageData.confimButt = HAS_CONFIM_BUTTON;
					DialogPageData.fun = NULL;
					strcpy(DialogPageData.str,"主程序文件参数响应异常");
					
					pMsg.MsgId = WM_USER_DATA;
					pMsg.Data.p = &DialogPageData;
					WM_SendMessage(DialogWin,&pMsg);
					
					//这里直接退出强制升级流程，并且不能让上级调用挂起
					return HAL_OK;
				}
			}
		}break;
		
		//升级引导程序
		case 1:
		{
			//升级引导程序
			if(CAT_B_Send_RespondSoftwareInfo(VersionType,1) != HAL_OK){
				//弹窗提示
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = NULL;
				strcpy(DialogPageData.str,"联网失败，稍后重试");
				
				pMsg.MsgId = WM_USER_DATA;
				pMsg.Data.p = &DialogPageData;
				WM_SendMessage(DialogWin,&pMsg);
				
				return HAL_ERROR;
			}else{
				//判断服务器响应的参数是否正确
				if(CATGlobalStatus.softwareInfo.publishOrTestFlag == VersionType && CATGlobalStatus.softwareInfo.softwareType == 1){
					//开始建立FTP连接，临时关闭MQTT连接
					if(CATGlobalStatus.flag.mqttConn == 1){
						CAT_B_CloseMQTTConn();
					}
					
					//建立FTP连接
					if(CAT_ConnFTP() != HAL_OK){
						//FTP连接建立失败，弹出窗即可，不能挂起任务
						//弹窗提示
						DialogPageData.bmpType = BMP_ALARM;
						DialogPageData.confimButt = HAS_CONFIM_BUTTON;
						DialogPageData.fun = NULL;
						strcpy(DialogPageData.str,"FTP连接建立失败");
						
						pMsg.MsgId = WM_USER_DATA;
						pMsg.Data.p = &DialogPageData;
						WM_SendMessage(DialogWin,&pMsg);
						
						//这里直接退出强制升级流程，并且不能让上级调用挂起
						return HAL_OK;
					}else{
						//下载文件
						memset(TempPath,0,50);
						if(VersionType == 0){
							sprintf(TempPath,"/ftp/code/bootload_v%03u.bin",CATGlobalStatus.serviceVersion.PBootloadVersion);
						}else{
							sprintf(TempPath,"/ftp/test_code/bootload_v%03u.bin",CATGlobalStatus.serviceVersion.TBootloadVersion);
						}
						
						if(CAT_FTPGetFile(FTPDownLoadData,TempPath,0) != HAL_OK){
							//文件下载失败，弹窗即可，不能挂起任务
							//弹窗提示
							DialogPageData.bmpType = BMP_ALARM;
							DialogPageData.confimButt = HAS_CONFIM_BUTTON;
							DialogPageData.fun = NULL;
							strcpy(DialogPageData.str,"引导程序文件下载失败");
							
							pMsg.MsgId = WM_USER_DATA;
							pMsg.Data.p = &DialogPageData;
							WM_SendMessage(DialogWin,&pMsg);
							
							//这里直接退出强制升级流程，并且不能让上级调用挂起
							return HAL_OK;
						}else{
							//下载文件成功，校验文件
							if(CATGlobalStatus.softwareInfo.totalLen != FTPDownLoadData->totalLen || FTPDownLoadData->totalLen > 32*1024 || CRC16(FTPDownLoadData->data,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
								//文件校验异常，弹窗...
								//弹窗提示
								DialogPageData.bmpType = BMP_ALARM;
								DialogPageData.confimButt = HAS_CONFIM_BUTTON;
								DialogPageData.fun = NULL;
								strcpy(DialogPageData.str,"引导程序文件校验失败");
								
								pMsg.MsgId = WM_USER_DATA;
								pMsg.Data.p = &DialogPageData;
								WM_SendMessage(DialogWin,&pMsg);
								
								//这里直接退出强制升级流程，并且不能让上级调用挂起
								return HAL_OK;
							}else{
								//弹窗提示
								DialogPageData.bmpType = BMP_ALARM;
								DialogPageData.confimButt = NO_CONFIM_BUTTON;
								DialogPageData.fun = NULL;
								strcpy(DialogPageData.str,"引导程序写入中，请勿断电");
								
								pMsg.MsgId = WM_USER_DATA;
								pMsg.Data.p = &DialogPageData;
								WM_SendMessage(DialogWin,&pMsg);
								
								//校验成功，写入文件到bootload程序段FLASH中
								if(FLASH_WriteData_8(FLASH_BASE,FTPDownLoadData->data,FTPDownLoadData->totalLen) != HAL_OK){
									//再写入一次
									if(FLASH_WriteData_8(FLASH_BASE,FTPDownLoadData->data,FTPDownLoadData->totalLen) != HAL_OK){
										//弹窗提示
										DialogPageData.bmpType = BMP_ALARM;
										DialogPageData.confimButt = HAS_CONFIM_BUTTON;
										DialogPageData.fun = NULL;
										strcpy(DialogPageData.str,"升级BOOT失败，请联系厂家售后");
										
										pMsg.MsgId = WM_USER_DATA;
										pMsg.Data.p = &DialogPageData;
										WM_SendMessage(DialogWin,&pMsg);
										
										//这里直接退出强制升级流程，并且不能让上级调用挂起
										return HAL_OK;
									}else{
										//校验写入到FLASH中的数据
										if(CRC16((uint8_t*)FLASH_BASE,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
											//校验失败，再重新写入一次
											if(FLASH_WriteData_8(FLASH_BASE,FTPDownLoadData->data,FTPDownLoadData->totalLen) != HAL_OK){
												//弹窗提示
												DialogPageData.bmpType = BMP_ALARM;
												DialogPageData.confimButt = HAS_CONFIM_BUTTON;
												DialogPageData.fun = NULL;
												strcpy(DialogPageData.str,"升级BOOT失败，请联系厂家售后");
												
												pMsg.MsgId = WM_USER_DATA;
												pMsg.Data.p = &DialogPageData;
												WM_SendMessage(DialogWin,&pMsg);
												
												//这里直接退出强制升级流程，并且不能让上级调用挂起
												return HAL_OK;
											}else{
												if(CRC16((uint8_t*)FLASH_BASE,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
													//弹窗提示
													DialogPageData.bmpType = BMP_ALARM;
													DialogPageData.confimButt = HAS_CONFIM_BUTTON;
													DialogPageData.fun = NULL;
													strcpy(DialogPageData.str,"升级BOOT失败，请联系厂家售后");
													
													pMsg.MsgId = WM_USER_DATA;
													pMsg.Data.p = &DialogPageData;
													WM_SendMessage(DialogWin,&pMsg);
													
													//这里直接退出强制升级流程，并且不能让上级调用挂起
													return HAL_OK;
												}
											}
										}
									}
								}else{
									//校验写入到FLASH中的数据
									if(CRC16((uint8_t*)FLASH_BASE,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
										//校验失败，再重新写入一次
										if(FLASH_WriteData_8(FLASH_BASE,FTPDownLoadData->data,FTPDownLoadData->totalLen) != HAL_OK){
											//弹窗提示
											DialogPageData.bmpType = BMP_ALARM;
											DialogPageData.confimButt = HAS_CONFIM_BUTTON;
											DialogPageData.fun = NULL;
											strcpy(DialogPageData.str,"升级BOOT失败，请联系厂家售后");
											
											pMsg.MsgId = WM_USER_DATA;
											pMsg.Data.p = &DialogPageData;
											WM_SendMessage(DialogWin,&pMsg);
											
											//这里直接退出强制升级流程，并且不能让上级调用挂起
											return HAL_OK;
										}else{
											if(CRC16((uint8_t*)FLASH_BASE,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
												//弹窗提示
												DialogPageData.bmpType = BMP_ALARM;
												DialogPageData.confimButt = HAS_CONFIM_BUTTON;
												DialogPageData.fun = NULL;
												strcpy(DialogPageData.str,"升级BOOT失败，请联系厂家售后");
												
												pMsg.MsgId = WM_USER_DATA;
												pMsg.Data.p = &DialogPageData;
												WM_SendMessage(DialogWin,&pMsg);
												
												//这里直接退出强制升级流程，并且不能让上级调用挂起
												return HAL_OK;
											}
										}
									}
								}
							}
						}
					}
				}else{
					//弹窗提示
					DialogPageData.bmpType = BMP_ALARM;
					DialogPageData.confimButt = HAS_CONFIM_BUTTON;
					DialogPageData.fun = NULL;
					strcpy(DialogPageData.str,"引导程序文件参数响应异常");
					
					pMsg.MsgId = WM_USER_DATA;
					pMsg.Data.p = &DialogPageData;
					WM_SendMessage(DialogWin,&pMsg);
					
					//这里直接退出强制升级流程，并且不能让上级调用挂起
					return HAL_OK;
				}
			}
		}break;
		
		//升级UI
		case 2:
		{
			//UI资源只升级发布版，测试版直接跳过
			if(VersionType == 0){
				//判断UI资源是否需要升级
				while(CATGlobalStatus.serviceVersion.PUiVersion > MachInfo.other.uiVersion){
					uint8_t i = 0;
					
					if(CAT_B_Send_RespondUIInfo(0,MachInfo.other.uiVersion+1) != HAL_OK){
						//弹窗提示
						DialogPageData.bmpType = BMP_ALARM;
						DialogPageData.confimButt = HAS_CONFIM_BUTTON;
						DialogPageData.fun = NULL;
						strcpy(DialogPageData.str,"联网失败，稍后重试");
						
						pMsg.MsgId = WM_USER_DATA;
						pMsg.Data.p = &DialogPageData;
						WM_SendMessage(DialogWin,&pMsg);
						
						return HAL_ERROR;
					}else{
						//判断服务器响应的参数是否正确
						if(CATGlobalStatus.uiInfo.publishOrTestFlag == 0 && CATGlobalStatus.uiInfo.uiVersion == MachInfo.other.uiVersion+1){
							//开始建立FTP连接，临时关闭MQTT连接
							if(CATGlobalStatus.flag.mqttConn == 1){
								CAT_B_CloseMQTTConn();
							}
							
							//建立FTP连接
							if(CAT_ConnFTP() != HAL_OK){
								//FTP连接建立失败，弹出窗即可，不能挂起任务
								//弹窗提示
								DialogPageData.bmpType = BMP_ALARM;
								DialogPageData.confimButt = HAS_CONFIM_BUTTON;
								DialogPageData.fun = NULL;
								strcpy(DialogPageData.str,"FTP连接建立失败");
								
								pMsg.MsgId = WM_USER_DATA;
								pMsg.Data.p = &DialogPageData;
								WM_SendMessage(DialogWin,&pMsg);
								
								//这里直接退出强制升级流程，并且不能让上级调用挂起
								return HAL_OK;
							}else{
								//为了节省流量，当已经存在同名的UI资源时，则不会再从FTP中下载文件了
								for(i=0;i<CATGlobalStatus.uiInfo.totalNum;i++){
									//先判断下是否已经存在该文件
									memset(TempPath,0,50);
									memset(TempPath2,0,50);
									sprintf(TempPath,"N0:\\%s", CATGlobalStatus.uiInfo.file[i].path);
									
									//更换路径
									PathChange(TempPath2,TempPath);
									
									if(FatFs_IsExistFile(TempPath2) == FEED_BACK_FAIL){
										memset(TempPath,0,50);
										sprintf(TempPath,"/ftp/ui_v%03u/%s",CATGlobalStatus.uiInfo.uiVersion,CATGlobalStatus.uiInfo.file[i].path);
										
										//下载文件
										if(CAT_FTPGetFile(FTPDownLoadData,TempPath,0) != HAL_OK){
											//文件下载失败，弹窗即可，不能挂起任务
											//弹窗提示
											DialogPageData.bmpType = BMP_ALARM;
											DialogPageData.confimButt = HAS_CONFIM_BUTTON;
											DialogPageData.fun = NULL;
											strcpy(DialogPageData.str,"UI文件下载失败");
											
											pMsg.MsgId = WM_USER_DATA;
											pMsg.Data.p = &DialogPageData;
											WM_SendMessage(DialogWin,&pMsg);
											
											//这里直接退出强制升级流程，并且不能让上级调用挂起
											return HAL_OK;
										}else{
											//下载文件成功，校验文件
											if(CATGlobalStatus.uiInfo.file[i].fileSize != FTPDownLoadData->totalLen || CRC16(FTPDownLoadData->data,FTPDownLoadData->totalLen) != CATGlobalStatus.uiInfo.file[i].crc16){
												//文件校验异常，弹窗...
												//弹窗提示
												DialogPageData.bmpType = BMP_ALARM;
												DialogPageData.confimButt = HAS_CONFIM_BUTTON;
												DialogPageData.fun = NULL;
												strcpy(DialogPageData.str,"UI文件校验失败");
												
												pMsg.MsgId = WM_USER_DATA;
												pMsg.Data.p = &DialogPageData;
												WM_SendMessage(DialogWin,&pMsg);
												
												//这里直接退出强制升级流程，并且不能让上级调用挂起
												return HAL_OK;
											}else{
												//校验成功，写入文件到存储设备
												memset(TempPath,0,50);
												memset(TempPath2,0,50);
												sprintf(TempPath,"N0:\\%s",CATGlobalStatus.uiInfo.file[i].path);
												
												//更换路径
												PathChange(TempPath2,TempPath);
												
												if(FatFs_WriteNewFile((uint8_t*)TempPath2,FTPDownLoadData->data,FTPDownLoadData->totalLen) == FEED_BACK_SUCCESS){
													//写入成功，重新读取出来，判断CRC是否一致
													if(FatFs_ReadAllFile((uint8_t*)TempPath2,FTPDownLoadData->data+FTPDownLoadData->totalLen+100,&FTPDownLoadData->totalLen) == FEED_BACK_SUCCESS){
														if(CRC16(FTPDownLoadData->data+FTPDownLoadData->totalLen+100,FTPDownLoadData->totalLen) != CATGlobalStatus.uiInfo.file[i].crc16){
															//弹窗提示
															DialogPageData.bmpType = BMP_ALARM;
															DialogPageData.confimButt = HAS_CONFIM_BUTTON;
															DialogPageData.fun = NULL;
															strcpy(DialogPageData.str,"UI文件写入后校验失败");
															
															pMsg.MsgId = WM_USER_DATA;
															pMsg.Data.p = &DialogPageData;
															WM_SendMessage(DialogWin,&pMsg);
															
															//这里直接退出强制升级流程，并且不能让上级调用挂起
															return HAL_OK;
														}
													}
												}else{
													//弹窗提示
													DialogPageData.bmpType = BMP_ALARM;
													DialogPageData.confimButt = HAS_CONFIM_BUTTON;
													DialogPageData.fun = NULL;
													strcpy(DialogPageData.str,"UI文件写入失败");
													
													pMsg.MsgId = WM_USER_DATA;
													pMsg.Data.p = &DialogPageData;
													WM_SendMessage(DialogWin,&pMsg);
													
													//这里直接退出强制升级流程，并且不能让上级调用挂起
													return HAL_OK;
												}
											}
										}
									}
								}
								
								CATGlobalStatus.localVersion.uiVersion += 1;
								
								//更新EEPOM中的UI版本号
								MachInfo.other.uiVersion += 1;
								EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_OTHER+((uint8_t*)&MachInfo.other.uiVersion-(uint8_t*)&MachInfo.other),(uint8_t*)&MachInfo.other.uiVersion,sizeof(MachInfo.other.uiVersion));
							}
						}else{
							//弹窗提示
							DialogPageData.bmpType = BMP_ALARM;
							DialogPageData.confimButt = HAS_CONFIM_BUTTON;
							DialogPageData.fun = NULL;
							strcpy(DialogPageData.str,"UI文件参数异常");
							
							pMsg.MsgId = WM_USER_DATA;
							pMsg.Data.p = &DialogPageData;
							WM_SendMessage(DialogWin,&pMsg);
							
							//这里直接退出强制升级流程，并且不能让上级调用挂起
							return HAL_OK;
						}
					}
				}
			}
		}break;
		
		default :return HAL_OK;
	}
	
	//升级完成，弹窗提示
	DialogPageData.bmpType = BMP_OK;
	DialogPageData.confimButt = HAS_CONFIM_BUTTON;
	DialogPageData.fun = NULL;
	strcpy(DialogPageData.str,"系统配置完成");
	
	pMsg.MsgId = WM_USER_DATA;
	pMsg.Data.p = &DialogPageData;
	WM_SendMessage(DialogWin,&pMsg);
	
	//标记需要重启仪器
	CATGlobalStatus.catDealSysRestartFlag = 0xA55A0FF0;
	
	return HAL_OK;
}





/***
*查询CAT模块是否正常，以及CAT模块状态的更新任务
***/
void CATUpdateStatusTask(void *argument)
{
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osMessageQueueId_t CATQueueHandle;
	extern uint32_t AlarmCATTimeupCount;
	extern osThreadId_t CATUpdateStatusTaskHandle;
	uint8_t RetryCount = 0;
	CATQueueInfoBuf_s CATQueueInfoBuf;
	static uint8_t CatRestartCount = 0;
	
	//英文模式不开启CAT任务
	if(MachInfo.systemSet.eLanguage == LANGUAGE_ENGLISH){
		osThreadSuspend(CATUpdateStatusTaskHandle);
	}
	
	//拉低PWRKEY引脚3~5s后，拉高，使得模块开机
	CAT_PWR_ON;
	osDelay(3000);
	CAT_PWR_OFF;
	
	//硬复位模块
	CAT_RESET_ON;
	osDelay(1000);
	CAT_RESET_OFF;
	
	//等待STATUS引脚电平变为高电平，不过这里需要注意：经过三极管后，这里极性相反，即低电平表示模块开机成功
	while(Input_GetState(INPUT_CH_CAT_STATUS) != INPUT_CAT_STATUS_ACTIVE){
		RetryCount++;
		
		if(RetryCount > 20){
			CatRestartCount++;
			
			if(CatRestartCount <= 3){
				Msg_Head_t BackendMsg = {0};
				
				//往后台发送消息，重启CAT初始化任务
				BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
				UI_Put_Msg((uint8_t*)&BackendMsg);
				
				osDelay(10000);
			}else{
				CATGlobalStatus.flag.powOn = 0;
			
				//模块启动异常
				osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_START_ERR);
				osThreadSuspend(CATUpdateStatusTaskHandle);
			}
		}
		
		osDelay(500);
	}
	
	//模块上电开机成功
	CATGlobalStatus.flag.powOn = 1;
	
	//将运营商识别初始化
	CATGlobalStatus.operatorNum = 0xFF;
	
	//将注网状态初始化
	CATGlobalStatus.registerNet = CAT_REGISTNET_UNKNOW;
	
	osDelay(500);
	
	//先等待CAT和串口通讯成功
	if(CAT_IsOK() != HAL_OK){
		CatRestartCount++;
			
		if(CatRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//往后台发送消息，重启CAT初始化任务
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//模块串口通讯异常
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//识别当前模块型号
	if(CAT_ReadType() != HAL_OK){
		CatRestartCount++;
			
		if(CatRestartCount <= 3){
			Msg_Head_t BackendMsg = {0};
			
			//往后台发送消息，重启CAT初始化任务
			BackendMsg.usCmd = CMD_SERVER_CAT_RESTART;
			UI_Put_Msg((uint8_t*)&BackendMsg);
			
			osDelay(10000);
		}else{
			//模块串口通讯异常
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_SERIAL_ERR);
			osThreadSuspend(CATUpdateStatusTaskHandle);
		}
	}
	
	//初始化CAT模块
	if(CAT_InitConfig() == HAL_OK){
		//发送CAT初始化成功事件
		osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_INIT_SUCCESS);
	}
	
	while(1){
		AlarmCATTimeupCount++;
		
		//周期性查询信号强度，网络注册状态，网络连接状态
		CAT_RegistNet();
		CAT_GetSignalStrength();
		
		//判断前后连接服务端间隔是否已经到60分钟
		if(AlarmCATTimeupCount > CAT_TIME_INTERVAL_ALARM/CAT_UPDATE_STATUS_TIME_INTERVAL){
			AlarmCATTimeupCount = 0;
			
			CATQueueInfoBuf.msgType = TIME_UP_CAT_MSG_TYPE;
			osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
			
			if(CATGlobalStatus.flag.noOtherServer == 0){
				CATQueueInfoBuf.msgType = TIME_UP_CAT_MSG_TYPE;
				osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0,0);
			}
		}
		
		osDelay(CAT_UPDATE_STATUS_TIME_INTERVAL);
	}
}




/***
*CAT模块数据收发相关任务
***/
void CATTask(void *argument)
{
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern CATGlobalStatus_s CATGlobalStatus;
	extern osMessageQueueId_t CATQueueHandle;
	extern uint32_t AlarmCATTimeupCount;
	extern osThreadId_t CATTaskHandle;
	extern char MainHardVer[10];
	extern char ConnHardVer[10];
	extern osThreadId_t AlgoHandle;
	extern WM_HWIN CreateDialogPage(void);
	extern __IO WM_HWIN g_hActiveWin;
	extern __IO DataManage_t g_tDataManage;
	extern MachInfo_s MachInfo;
	extern osSemaphoreId_t CATSemMutexHandle;
	CATQueueInfoBuf_s CATQueueInfoBuf;
	CommonDialogPageData_s DialogPageData = {0};
	uint8_t UpdateFlag = 0;
	WM_MESSAGE pMsg;
	char TempPath[50] = {0};
	char TempPath2[50] = {0};
	uint16_t RetryCount = 0;
	
	osPriority_t CurrentTaskPri = osPriorityNone;				//用于升级时临时提高此任务优先级时，临时保存当前优先级，以便后续还原
	
	
	//初始化网络状态标志
	CATGlobalStatus.flag.connServerErr = 1;
	
	//等待CAT初始化成功事件，并清除标志位
	osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_INIT_SUCCESS,osFlagsWaitAny,osWaitForever);
	
	osDelay(2000);
	
	//获取CAT模块与服务端通讯权
	osSemaphoreAcquire(CATSemMutexHandle,osWaitForever);
	
	//升级中不启动第三方服务端
	if(MachRunPara.update.nextOpenToStartUpdateFlag != NEXT_OPEN_TO_START_UPDATE_FLAG){
		//发送与第三方服务端通讯的任务启动事件
		osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_START);
	}
		
	//建立MQTT连接
	while(CAT_B_CreateMQTTConn(SERVICE_CC) != HAL_OK){
		CATGlobalStatus.flag.connServerErr = 1;
		
		//断开MQTT连接
		CAT_B_CloseMQTTConn();
		
		osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_NET_ERR);
		
		//释放CAT模块与服务端通讯权
		osSemaphoreRelease(CATSemMutexHandle);
		
		osDelay(10000);
		
		//获取CAT模块与服务端通讯权
		osSemaphoreAcquire(CATSemMutexHandle,osWaitForever);
		
		CATGlobalStatus.flag.connServerErr = 0;
	}
	
	CATGlobalStatus.flag.connServerErr = 0;
	
	//间隔100ms，给服务端订阅设备主题提供缓冲时间
	osDelay(100);
	
	//判断是否已经发送过仪器出厂确认信息给服务端
	if(MachInfo.flag.bit.connNet != 1){
		while(CAT_B_Send_MachineFactoryInfo() != HAL_OK){
			CATGlobalStatus.flag.connServerErr = 1;
			
			CAT_B_CloseMQTTConn();
			
			osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_NET_ERR);
			
			//释放CAT模块与服务端通讯权
			osSemaphoreRelease(CATSemMutexHandle);
			
			osThreadSuspend(CATTaskHandle);
			
			//获取CAT模块与服务端通讯权
			osSemaphoreAcquire(CATSemMutexHandle,osWaitForever);
			
			CATGlobalStatus.flag.connServerErr = 0;
			CAT_B_CreateMQTTConn(SERVICE_CC);
		}
		//发送序列号已录入成功事件
		osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_FACTORY_INFO_OK);
		
		MachInfo.flag.bit.connNet = 1;
			
		//写入EEPROM，只更新该字段内容
		EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_FLAG,(uint8_t*)&MachInfo.flag,sizeof(MachInfo.flag));
	}
	
	//发送设备当前开机信息
	while(CAT_B_Send_MachineOpenInfo() != HAL_OK){
		CATGlobalStatus.flag.connServerErr = 1;
		
		CAT_B_CloseMQTTConn();
		
		osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_NET_ERR);
		
		//释放CAT模块与服务端通讯权
		osSemaphoreRelease(CATSemMutexHandle);
		
		osDelay(10000);
		
		//获取CAT模块与服务端通讯权
		osSemaphoreAcquire(CATSemMutexHandle,osWaitForever);
		
		CATGlobalStatus.flag.connServerErr = 0;
		CAT_B_CreateMQTTConn(SERVICE_CC);
	}
	
	
	//判断是否需要升级
	//判断主程序是否需要升级
	if(CATGlobalStatus.serviceVersion.PMainVersion > CATGlobalStatus.localVersion.mainVersion){
		UpdateFlag = 1;
		
		if(MachRunPara.update.nextOpenToStartUpdateFlag == NEXT_OPEN_TO_START_UPDATE_FLAG){
			//临时提高此任务优先级
			CurrentTaskPri = osThreadGetPriority(CATTaskHandle);
			osThreadSetPriority(CATTaskHandle,osPriorityHigh);
			
			if(CAT_B_Send_RespondSoftwareInfo(0,0) != HAL_OK){
				//弹窗提示
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = NULL;
				strcpy(DialogPageData.str,"联网失败，稍后重试");
				
				pMsg.MsgId = WM_USER_DATA;
				pMsg.Data.p = &DialogPageData;
				WM_SendMessage(DialogWin,&pMsg);
				
				
				CAT_B_CloseMQTTConn();
				CATGlobalStatus.flag.connServerErr = 1;
				
				//释放CAT模块与服务端通讯权
				osSemaphoreRelease(CATSemMutexHandle);
				osThreadSuspend(CATTaskHandle);
			}else{
				//判断服务器响应的参数是否正确
				if(CATGlobalStatus.softwareInfo.publishOrTestFlag == 0 && CATGlobalStatus.softwareInfo.softwareType == 0){
					FTPDownloadData_s* FTPDownLoadData = (FTPDownloadData_s*)SDRAM_ApplyCommonBuf(sizeof(FTPDownloadData_s));
					
					//开始建立FTP连接，临时关闭MQTT连接
					if(CATGlobalStatus.flag.mqttConn == 1){
						CAT_B_CloseMQTTConn();
					}
					
					//建立FTP连接
					if(CAT_ConnFTP() != HAL_OK){
						//FTP连接建立失败，弹出窗即可，不能挂起任务
						//弹窗提示
						DialogPageData.bmpType = BMP_ALARM;
						DialogPageData.confimButt = HAS_CONFIM_BUTTON;
						DialogPageData.fun = NULL;
						strcpy(DialogPageData.str,"FTP连接建立失败");
						
						pMsg.MsgId = WM_USER_DATA;
						pMsg.Data.p = &DialogPageData;
						WM_SendMessage(DialogWin,&pMsg);
						
						goto UpdateStopTag;
					}else{
						//下载文件
						memset(TempPath,0,50);
						sprintf(TempPath,"/ftp/code/main_v%03u.bin",CATGlobalStatus.serviceVersion.PMainVersion);
						if(CAT_FTPGetFile(FTPDownLoadData,TempPath,0) != HAL_OK){
							//文件下载失败，弹窗即可，不能挂起任务
							//弹窗提示
							DialogPageData.bmpType = BMP_ALARM;
							DialogPageData.confimButt = HAS_CONFIM_BUTTON;
							DialogPageData.fun = NULL;
							strcpy(DialogPageData.str,"主程序文件下载失败");
							
							pMsg.MsgId = WM_USER_DATA;
							pMsg.Data.p = &DialogPageData;
							WM_SendMessage(DialogWin,&pMsg);
							
							goto UpdateStopTag;
						}else{
							//下载文件成功，校验文件
							if(CATGlobalStatus.softwareInfo.totalLen != FTPDownLoadData->totalLen || FTPDownLoadData->totalLen > 1024*1024 || CRC16(FTPDownLoadData->data,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
								//文件校验异常，弹窗...
								//弹窗提示
								DialogPageData.bmpType = BMP_ALARM;
								DialogPageData.confimButt = HAS_CONFIM_BUTTON;
								DialogPageData.fun = NULL;
								strcpy(DialogPageData.str,"主程序文件校验失败");
								
								pMsg.MsgId = WM_USER_DATA;
								pMsg.Data.p = &DialogPageData;
								WM_SendMessage(DialogWin,&pMsg);
								
								goto UpdateStopTag;
							}else{
								//校验成功，写入文件到存储设备
								FTPDownLoadData->flag = 0xA55A0FF0;
								FTPDownLoadData->crc16 = CATGlobalStatus.softwareInfo.crc16;
								
								SPI4_ChangeModeTo(SPI_MODE_FLASH);//SPI4_ChangeModeToFlash();
								if(SPIFlash_WriteData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)FTPDownLoadData,12+FTPDownLoadData->totalLen) == SUCCESS){
									//写入成功，重新读取出来，判断CRC是否一致
									if(SPIFlash_DMA_ReadData(SPI_FLASH_SAVE_MAIN_SOFTWARE_OFFSET,(uint8_t*)FTPDownLoadData,12+FTPDownLoadData->totalLen) == SUCCESS){
										SPI4_ExitModeToDefault();//
                                        if(CRC16((uint8_t*)FTPDownLoadData->data,FTPDownLoadData->totalLen) == CATGlobalStatus.softwareInfo.crc16){
											
											//升级成功
											//往EEPROM做好BOOT写入main程序标志
											MachRunPara.update.needBootToUpdateMainFlag = UPDATE_MAIN_SOFTWARE_FLAG;
											EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_UPDATE_MAIN_SOFTWARE_FLAG,(uint8_t*)&MachRunPara.update.needBootToUpdateMainFlag,sizeof(MachRunPara.update.needBootToUpdateMainFlag));
										}else{
											//弹窗提示
											DialogPageData.bmpType = BMP_ALARM;
											DialogPageData.confimButt = HAS_CONFIM_BUTTON;
											DialogPageData.fun = NULL;
											strcpy(DialogPageData.str,"主程序文件写入后校验失败");
											
											pMsg.MsgId = WM_USER_DATA;
											pMsg.Data.p = &DialogPageData;
											WM_SendMessage(DialogWin,&pMsg);
											
											goto UpdateStopTag;
										}
									}
                                    SPI4_ExitModeToDefault();//
								}else{
                                    SPI4_ExitModeToDefault();//
									//弹窗提示
									DialogPageData.bmpType = BMP_ALARM;
									DialogPageData.confimButt = HAS_CONFIM_BUTTON;
									DialogPageData.fun = NULL;
									strcpy(DialogPageData.str,"主程序文件写入失败");
									
									pMsg.MsgId = WM_USER_DATA;
									pMsg.Data.p = &DialogPageData;
									WM_SendMessage(DialogWin,&pMsg);
									
									goto UpdateStopTag;
								}
								SPI4_ExitModeToDefault();//SPI4_ChangeModeToIDLE();
							}
						}
					}
					
					//重新开启MQTT连接
					if(CATGlobalStatus.flag.mqttConn == 0){
						CAT_B_CreateMQTTConn(SERVICE_CC);
					}
				}else{
					//弹窗提示
					DialogPageData.bmpType = BMP_ALARM;
					DialogPageData.confimButt = HAS_CONFIM_BUTTON;
					DialogPageData.fun = NULL;
					strcpy(DialogPageData.str,"主程序文件参数响应异常");
					
					pMsg.MsgId = WM_USER_DATA;
					pMsg.Data.p = &DialogPageData;
					WM_SendMessage(DialogWin,&pMsg);
					
					goto UpdateStopTag;
				}
			}
		}
	}
	
	
	//判断引导程序是否需要升级
	if(CATGlobalStatus.serviceVersion.PBootloadVersion > CATGlobalStatus.localVersion.bootloadVersion){
		UpdateFlag = 1;
		
		if(MachRunPara.update.nextOpenToStartUpdateFlag == NEXT_OPEN_TO_START_UPDATE_FLAG){
			//临时提高此任务优先级
			if(osThreadGetPriority(CATTaskHandle) != osPriorityHigh){
				CurrentTaskPri = osThreadGetPriority(CATTaskHandle);
				osThreadSetPriority(CATTaskHandle,osPriorityHigh);
			}
			
			if(CAT_B_Send_RespondSoftwareInfo(0,1) != HAL_OK){
				//弹窗提示
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = NULL;
				strcpy(DialogPageData.str,"联网失败，稍后重试");
				
				pMsg.MsgId = WM_USER_DATA;
				pMsg.Data.p = &DialogPageData;
				WM_SendMessage(DialogWin,&pMsg);
				
				CAT_B_CloseMQTTConn();
				CATGlobalStatus.flag.connServerErr = 1;
				
				//释放CAT模块与服务端通讯权
				osSemaphoreRelease(CATSemMutexHandle);
				osThreadSuspend(CATTaskHandle);
			}else{
				//判断服务器响应的参数是否正确
				if(CATGlobalStatus.softwareInfo.publishOrTestFlag == 0 && CATGlobalStatus.softwareInfo.softwareType == 1){
					FTPDownloadData_s* FTPDownLoadData = (FTPDownloadData_s*)SDRAM_ApplyCommonBuf(sizeof(FTPDownloadData_s));
					
					//开始建立FTP连接，临时关闭MQTT连接
					if(CATGlobalStatus.flag.mqttConn == 1){
						CAT_B_CloseMQTTConn();
					}
					
					//建立FTP连接
					if(CAT_ConnFTP() != HAL_OK){
						//FTP连接建立失败，弹出窗即可，不能挂起任务
						//弹窗提示
						DialogPageData.bmpType = BMP_ALARM;
						DialogPageData.confimButt = HAS_CONFIM_BUTTON;
						DialogPageData.fun = NULL;
						strcpy(DialogPageData.str,"FTP连接建立失败");
						
						pMsg.MsgId = WM_USER_DATA;
						pMsg.Data.p = &DialogPageData;
						WM_SendMessage(DialogWin,&pMsg);
						
						goto UpdateStopTag;
					}else{
						//下载文件
						memset(TempPath,0,50);
						sprintf(TempPath,"/ftp/code/bootload_v%03u.bin",CATGlobalStatus.serviceVersion.PBootloadVersion);
						if(CAT_FTPGetFile(FTPDownLoadData,TempPath,0) != HAL_OK){
							//文件下载失败，弹窗即可，不能挂起任务
							//弹窗提示
							DialogPageData.bmpType = BMP_ALARM;
							DialogPageData.confimButt = HAS_CONFIM_BUTTON;
							DialogPageData.fun = NULL;
							strcpy(DialogPageData.str,"引导程序文件下载失败");
							
							pMsg.MsgId = WM_USER_DATA;
							pMsg.Data.p = &DialogPageData;
							WM_SendMessage(DialogWin,&pMsg);
							
							goto UpdateStopTag;
						}else{
							//下载文件成功，校验文件
							if(CATGlobalStatus.softwareInfo.totalLen != FTPDownLoadData->totalLen || FTPDownLoadData->totalLen > 32*1024 || CRC16(FTPDownLoadData->data,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
								//文件校验异常，弹窗...
								//弹窗提示
								DialogPageData.bmpType = BMP_ALARM;
								DialogPageData.confimButt = HAS_CONFIM_BUTTON;
								DialogPageData.fun = NULL;
								strcpy(DialogPageData.str,"引导程序文件校验失败");
								
								pMsg.MsgId = WM_USER_DATA;
								pMsg.Data.p = &DialogPageData;
								WM_SendMessage(DialogWin,&pMsg);
								
								goto UpdateStopTag;
							}else{
								//弹窗提示
								DialogPageData.bmpType = BMP_ALARM;
								DialogPageData.confimButt = NO_CONFIM_BUTTON;
								DialogPageData.fun = NULL;
								strcpy(DialogPageData.str,"引导程序写入中，请勿断电");
								
								pMsg.MsgId = WM_USER_DATA;
								pMsg.Data.p = &DialogPageData;
								WM_SendMessage(DialogWin,&pMsg);
								
								//校验成功，写入文件到bootload程序段FLASH中
								if(FLASH_WriteData_8(FLASH_BASE,FTPDownLoadData->data,FTPDownLoadData->totalLen) != HAL_OK){
									//再写入一次
									if(FLASH_WriteData_8(FLASH_BASE,FTPDownLoadData->data,FTPDownLoadData->totalLen) != HAL_OK){
										//弹窗提示
										DialogPageData.bmpType = BMP_ALARM;
										DialogPageData.confimButt = HAS_CONFIM_BUTTON;
										DialogPageData.fun = NULL;
										strcpy(DialogPageData.str,"升级BOOT失败，请联系厂家售后");
										
										pMsg.MsgId = WM_USER_DATA;
										pMsg.Data.p = &DialogPageData;
										WM_SendMessage(DialogWin,&pMsg);
										
										goto UpdateStopTag;
									}else{
										//校验写入到FLASH中的数据
										if(CRC16((uint8_t*)FLASH_BASE,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
											//校验失败，再重新写入一次
											if(FLASH_WriteData_8(FLASH_BASE,FTPDownLoadData->data,FTPDownLoadData->totalLen) != HAL_OK){
												//弹窗提示
												DialogPageData.bmpType = BMP_ALARM;
												DialogPageData.confimButt = HAS_CONFIM_BUTTON;
												DialogPageData.fun = NULL;
												strcpy(DialogPageData.str,"升级BOOT失败，请联系厂家售后");
												
												pMsg.MsgId = WM_USER_DATA;
												pMsg.Data.p = &DialogPageData;
												WM_SendMessage(DialogWin,&pMsg);
												
												goto UpdateStopTag;
											}else{
												if(CRC16((uint8_t*)FLASH_BASE,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
													//弹窗提示
													DialogPageData.bmpType = BMP_ALARM;
													DialogPageData.confimButt = HAS_CONFIM_BUTTON;
													DialogPageData.fun = NULL;
													strcpy(DialogPageData.str,"升级BOOT失败，请联系厂家售后");
													
													pMsg.MsgId = WM_USER_DATA;
													pMsg.Data.p = &DialogPageData;
													WM_SendMessage(DialogWin,&pMsg);
													
													goto UpdateStopTag;
												}
											}
										}
									}
								}else{
									//校验写入到FLASH中的数据
									if(CRC16((uint8_t*)FLASH_BASE,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
										//校验失败，再重新写入一次
										if(FLASH_WriteData_8(FLASH_BASE,FTPDownLoadData->data,FTPDownLoadData->totalLen) != HAL_OK){
											//弹窗提示
											DialogPageData.bmpType = BMP_ALARM;
											DialogPageData.confimButt = HAS_CONFIM_BUTTON;
											DialogPageData.fun = NULL;
											strcpy(DialogPageData.str,"升级BOOT失败，请联系厂家售后");
											
											pMsg.MsgId = WM_USER_DATA;
											pMsg.Data.p = &DialogPageData;
											WM_SendMessage(DialogWin,&pMsg);
											
											goto UpdateStopTag;
										}else{
											if(CRC16((uint8_t*)FLASH_BASE,FTPDownLoadData->totalLen) != CATGlobalStatus.softwareInfo.crc16){
												//弹窗提示
												DialogPageData.bmpType = BMP_ALARM;
												DialogPageData.confimButt = HAS_CONFIM_BUTTON;
												DialogPageData.fun = NULL;
												strcpy(DialogPageData.str,"升级BOOT失败，请联系厂家售后");
												
												pMsg.MsgId = WM_USER_DATA;
												pMsg.Data.p = &DialogPageData;
												WM_SendMessage(DialogWin,&pMsg);
												
												goto UpdateStopTag;
											}
										}
									}
								}
							}
						}
					}
					
					//重新开启MQTT连接
					if(CATGlobalStatus.flag.mqttConn == 0){
						CAT_B_CreateMQTTConn(SERVICE_CC);
					}
				}else{
					//弹窗提示
					DialogPageData.bmpType = BMP_ALARM;
					DialogPageData.confimButt = HAS_CONFIM_BUTTON;
					DialogPageData.fun = NULL;
					strcpy(DialogPageData.str,"引导程序文件参数响应异常");
					
					pMsg.MsgId = WM_USER_DATA;
					pMsg.Data.p = &DialogPageData;
					WM_SendMessage(DialogWin,&pMsg);
					
					goto UpdateStopTag;
				}
			}
		}
	}
	
	
	//判断UI资源是否需要升级
	while(CATGlobalStatus.serviceVersion.PUiVersion > CATGlobalStatus.localVersion.uiVersion){
		UpdateFlag = 1;
		
		uint8_t i = 0;
		
		if(MachRunPara.update.nextOpenToStartUpdateFlag == NEXT_OPEN_TO_START_UPDATE_FLAG){
			//临时提高此任务优先级
			if(osThreadGetPriority(CATTaskHandle) != osPriorityHigh){
				CurrentTaskPri = osThreadGetPriority(CATTaskHandle);
				osThreadSetPriority(CATTaskHandle,osPriorityHigh);
			}
			
			if(CAT_B_Send_RespondUIInfo(0,MachInfo.other.uiVersion+1) != HAL_OK){
				//弹窗提示
				DialogPageData.bmpType = BMP_ALARM;
				DialogPageData.confimButt = HAS_CONFIM_BUTTON;
				DialogPageData.fun = NULL;
				strcpy(DialogPageData.str,"联网失败，稍后重试");
				
				pMsg.MsgId = WM_USER_DATA;
				pMsg.Data.p = &DialogPageData;
				WM_SendMessage(DialogWin,&pMsg);
				
				CAT_B_CloseMQTTConn();
				CATGlobalStatus.flag.connServerErr = 1;
				
				//释放CAT模块与服务端通讯权
				osSemaphoreRelease(CATSemMutexHandle);
				osThreadSuspend(CATTaskHandle);
			}else{
				//判断服务器响应的参数是否正确
				if(CATGlobalStatus.uiInfo.publishOrTestFlag == 0 && CATGlobalStatus.uiInfo.uiVersion == MachInfo.other.uiVersion+1){
					FTPDownloadData_s* FTPDownLoadData = (FTPDownloadData_s*)SDRAM_ApplyCommonBuf(sizeof(FTPDownloadData_s));
					
					//开始建立FTP连接，临时关闭MQTT连接
					if(CATGlobalStatus.flag.mqttConn == 1){
						CAT_B_CloseMQTTConn();
					}
					
					//建立FTP连接
					if(CAT_ConnFTP() != HAL_OK){
						//FTP连接建立失败，弹出窗即可，不能挂起任务
						//弹窗提示
						DialogPageData.bmpType = BMP_ALARM;
						DialogPageData.confimButt = HAS_CONFIM_BUTTON;
						DialogPageData.fun = NULL;
						strcpy(DialogPageData.str,"FTP连接建立失败");
						
						pMsg.MsgId = WM_USER_DATA;
						pMsg.Data.p = &DialogPageData;
						WM_SendMessage(DialogWin,&pMsg);
						
						goto UpdateStopTag;
					}else{
						//为了节省流量，当已经存在同名的UI资源时，则不会再从FTP中下载文件了
						for(i=0;i<CATGlobalStatus.uiInfo.totalNum;i++){
							//先判断下是否已经存在该文件
							memset(TempPath,0,50);
							memset(TempPath2,0,50);
							sprintf(TempPath,"N0:\\%s", CATGlobalStatus.uiInfo.file[i].path);
							
							//更换路径
							PathChange(TempPath2,TempPath);
							
							if(FatFs_IsExistFile(TempPath2) != FEED_BACK_SUCCESS){
								memset(TempPath,0,50);
								sprintf(TempPath,"/ftp/ui_v%03u/%s",CATGlobalStatus.uiInfo.uiVersion,CATGlobalStatus.uiInfo.file[i].path);
								
								//下载文件
								if(CAT_FTPGetFile(FTPDownLoadData,TempPath,0) != HAL_OK){
									//文件下载失败，弹窗即可，不能挂起任务
									//弹窗提示
									DialogPageData.bmpType = BMP_ALARM;
									DialogPageData.confimButt = HAS_CONFIM_BUTTON;
									DialogPageData.fun = NULL;
									strcpy(DialogPageData.str,"UI文件下载失败");
									
									pMsg.MsgId = WM_USER_DATA;
									pMsg.Data.p = &DialogPageData;
									WM_SendMessage(DialogWin,&pMsg);
									
									goto UpdateStopTag;
								}else{
									//下载文件成功，校验文件
									if(CATGlobalStatus.uiInfo.file[i].fileSize != FTPDownLoadData->totalLen || CRC16(FTPDownLoadData->data,FTPDownLoadData->totalLen) != CATGlobalStatus.uiInfo.file[i].crc16){
										//文件校验异常，弹窗...
										//弹窗提示
										DialogPageData.bmpType = BMP_ALARM;
										DialogPageData.confimButt = HAS_CONFIM_BUTTON;
										DialogPageData.fun = NULL;
										strcpy(DialogPageData.str,"UI文件校验失败");
										
										pMsg.MsgId = WM_USER_DATA;
										pMsg.Data.p = &DialogPageData;
										WM_SendMessage(DialogWin,&pMsg);
										
										goto UpdateStopTag;
									}else{
										//校验成功，写入文件到存储设备
										memset(TempPath,0,50);
										memset(TempPath2,0,50);
										sprintf(TempPath,"N0:\\%s",CATGlobalStatus.uiInfo.file[i].path);
										
										//更换路径
										PathChange(TempPath2,TempPath);
										
										if(FatFs_WriteNewFile((uint8_t*)TempPath2,FTPDownLoadData->data,FTPDownLoadData->totalLen) == FEED_BACK_SUCCESS){
											//写入成功，重新读取出来，判断CRC是否一致
											if(FatFs_ReadAllFile((uint8_t*)TempPath2,FTPDownLoadData->data+FTPDownLoadData->totalLen+100,&FTPDownLoadData->totalLen) == FEED_BACK_SUCCESS){
												if(CRC16(FTPDownLoadData->data+FTPDownLoadData->totalLen+100,FTPDownLoadData->totalLen) != CATGlobalStatus.uiInfo.file[i].crc16){
													//弹窗提示
													DialogPageData.bmpType = BMP_ALARM;
													DialogPageData.confimButt = HAS_CONFIM_BUTTON;
													DialogPageData.fun = NULL;
													strcpy(DialogPageData.str,"UI文件写入后校验失败");
													
													pMsg.MsgId = WM_USER_DATA;
													pMsg.Data.p = &DialogPageData;
													WM_SendMessage(DialogWin,&pMsg);
													
													goto UpdateStopTag;
												}
											}
										}else{
											//弹窗提示
											DialogPageData.bmpType = BMP_ALARM;
											DialogPageData.confimButt = HAS_CONFIM_BUTTON;
											DialogPageData.fun = NULL;
											strcpy(DialogPageData.str,"UI文件写入失败");
											
											pMsg.MsgId = WM_USER_DATA;
											pMsg.Data.p = &DialogPageData;
											WM_SendMessage(DialogWin,&pMsg);
											
											goto UpdateStopTag;
										}
									}
								}
							}
						}
						
						CATGlobalStatus.localVersion.uiVersion += 1;
						
						//更新EEPOM中的UI版本号
						MachInfo.other.uiVersion += 1;
						EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_OTHER+((uint8_t*)&MachInfo.other.uiVersion-(uint8_t*)&MachInfo.other),(uint8_t*)&MachInfo.other.uiVersion,sizeof(MachInfo.other.uiVersion));
					}
					
					//重新开启MQTT连接
					if(CATGlobalStatus.flag.mqttConn == 0){
						CAT_B_CreateMQTTConn(SERVICE_CC);
					}
				}else{
					//弹窗提示
					DialogPageData.bmpType = BMP_ALARM;
					DialogPageData.confimButt = HAS_CONFIM_BUTTON;
					DialogPageData.fun = NULL;
					strcpy(DialogPageData.str,"UI文件参数异常");
					
					pMsg.MsgId = WM_USER_DATA;
					pMsg.Data.p = &DialogPageData;
					WM_SendMessage(DialogWin,&pMsg);
					
					goto UpdateStopTag;
				}
			}
		}else{
			break;
		}
	}
		
	//升级下载各模块文件成功，更新升级标志
	if(UpdateFlag == 1){
		if(MachRunPara.update.nextOpenToStartUpdateFlag != NEXT_OPEN_TO_START_UPDATE_FLAG){
			MachRunPara.update.nextOpenToStartUpdateFlag = NEXT_OPEN_TO_START_UPDATE_FLAG;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_NEXT_UPDATE_FLAG,(uint8_t*)&MachRunPara.update.nextOpenToStartUpdateFlag,sizeof(MachRunPara.update.nextOpenToStartUpdateFlag));
		}else{
			MachRunPara.update.nextOpenToStartUpdateFlag = 0;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_NEXT_UPDATE_FLAG,(uint8_t*)&MachRunPara.update.nextOpenToStartUpdateFlag,sizeof(MachRunPara.update.nextOpenToStartUpdateFlag));
			
			//弹窗提示
			DialogPageData.bmpType = BMP_OK;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = NULL;
			strcpy(DialogPageData.str,"系统配置完成");
			
			pMsg.MsgId = WM_USER_DATA;
			pMsg.Data.p = &DialogPageData;
			WM_SendMessage(DialogWin,&pMsg);
		
			osDelay(1000);
				
			//重新登录
			__set_PRIMASK(1);
			NVIC_SystemReset();
		}
	}else{
		if(MachRunPara.update.nextOpenToStartUpdateFlag == NEXT_OPEN_TO_START_UPDATE_FLAG){
			//升级触发异常，重置
			MachRunPara.update.nextOpenToStartUpdateFlag = 0;
			EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADR_NEXT_UPDATE_FLAG,(uint8_t*)&MachRunPara.update.nextOpenToStartUpdateFlag,sizeof(MachRunPara.update.nextOpenToStartUpdateFlag));
			
			//弹窗提示
			DialogPageData.bmpType = BMP_OK;
			DialogPageData.confimButt = HAS_CONFIM_BUTTON;
			DialogPageData.fun = NULL;
			strcpy(DialogPageData.str,"系统配置异常，即将重启");
			
			pMsg.MsgId = WM_USER_DATA;
			pMsg.Data.p = &DialogPageData;
			WM_SendMessage(DialogWin,&pMsg);
		
			osDelay(5000);
				
			//重新登录
			__set_PRIMASK(1);
			NVIC_SystemReset();
		}
	}
	
//升级异常，停止升级
UpdateStopTag:
	//还原此任务优先级
	if(osThreadGetPriority(CATTaskHandle) == osPriorityHigh){
		osThreadSetPriority(CATTaskHandle,CurrentTaskPri);
	}
	
	if(CATGlobalStatus.flag.mqttConn == 0){
		CAT_B_CreateMQTTConn(SERVICE_CC);
	}

	
	//询问后台是否有数据发送给仪器
	while(CAT_B_IsServiceWantToSendData() != HAL_OK){
		CATGlobalStatus.flag.connServerErr = 1;
		CAT_B_CloseMQTTConn();
		
		osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_NET_ERR);
		
		//释放CAT模块与服务端通讯权
		osSemaphoreRelease(CATSemMutexHandle);
		
		osDelay(10000);
		
		//获取CAT模块与服务端通讯权
		osSemaphoreAcquire(CATSemMutexHandle,osWaitForever);
		
		CATGlobalStatus.flag.connServerErr = 0;
		CAT_B_CreateMQTTConn(SERVICE_CC);
	}
	
	//断开MQTT连接
	CAT_B_CloseMQTTConn();
	
	//判断是否因CAT动作处理导致需要重启仪器
	if(CATGlobalStatus.catDealSysRestartFlag == 0xA55A0FF0){
		osDelay(1000);
				
		//重新登录
		__set_PRIMASK(1);
		NVIC_SystemReset();
	}
	
	//释放CAT模块与服务端通讯权
	osSemaphoreRelease(CATSemMutexHandle);
	RetryCount = 0;
	
	while(1){
		//等待消息
		osMessageQueueGet(CATQueueHandle,&CATQueueInfoBuf,0,osWaitForever);
		
		//获取CAT模块与服务端通讯权
		osSemaphoreAcquire(CATSemMutexHandle,osWaitForever);
		
		//建立MQTT连接
		if(CAT_B_CreateMQTTConn(SERVICE_CC) != HAL_OK){
			CATGlobalStatus.flag.connServerErr = 1;
			
			//不同事件执行不同的重发机制，需要等待服务端响应的则重发2次后就判定为超时，不需要服务端响应的则可以一直重发
			switch(CATQueueInfoBuf.msgType){
				//界面上存在弹窗，需要超时结束，重发2次
				case REFRESH_NET_CAT_MSG_TYPE:						//手动刷新网络事件
				case CHANGE_MACHINE_INFO_CAT_MSG_TYPGE:				//修改仪器关键参数唤醒任务
				case REQUEST_PATIENT_ID_CAT_MSG_TYPE:				//请求指定患者编号对应的患者信息
				{
					RetryCount++;
					if(RetryCount <= 2){
						osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
					}else{
						RetryCount = 0;
						osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_NET_ERR);
					}
				}break;
				
				//可以一直重发的事件
				default :
				{
					osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
				}break;
			}
			
			//断开MQTT连接
			CAT_B_CloseMQTTConn();
			
			//释放CAT模块与服务端通讯权
			osSemaphoreRelease(CATSemMutexHandle);
			
			//延迟10s后重新尝试连接
			osDelay(10000);
			
			continue;
		}
		
		CATGlobalStatus.flag.connServerErr = 0;
		
		//间隔200ms，给服务端订阅设备主题提供缓冲时间
		osDelay(200);
		
		//将队列中的消息全部取出来，待定...
		do{
			switch(CATQueueInfoBuf.msgType){
				//老化测试任务
				case AGING_TEST_CAT_MSG_TYPE:
				{
					//这里不需要执行其他操作，只是查看连接服务端和断开操作是否正常即可
				}break;
				
				//定时唤醒任务
				case TIME_UP_CAT_MSG_TYPE:
				{
					//发送当前信号强度
					if(CAT_B_Send_SignalStrength() != HAL_OK){
						CATGlobalStatus.flag.connServerErr = 1;
						
						osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
						
						goto CAT_iotTaskTag;
					}
				}
				break;
				
				//样本分析完毕后唤醒任务
				case SAMPLE_ANALYSE_CAT_MSG_TYPE:
				{
					WBCHGB_TestInfo_t* WBCHGBTestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
                    CountLog_t* ptCountLog = (CountLog_t*)SDRAM_ApplyCommonBuf(sizeof(CountLog_t));
                    CountHead_t* tCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
					uint32_t ulTailIndex = 0, ccSendIndex = 0, ulLogLen = 0;
					uint16_t i = 0;
					uint8_t AlarmFlag = 0;
                    
                    //读取SN
                    ccSendIndex  = Get_WBCHGB_SendIndex(SERVICE_CC);
                    ulTailIndex = Get_WBCHGB_Tail();                 
                    while(ccSendIndex != ulTailIndex){
                        //读取公共头信息
                        if(FatFs_Read_ConutHead(ccSendIndex, tCountHead) == FEED_BACK_FAIL){
							//重新读取一次
							if(FatFs_Read_ConutHead(ccSendIndex, tCountHead) == FEED_BACK_FAIL){
								//读取的数据校验异常，直接跳过
								ccSendIndex = Add_WBCHGB_SendIndex(SERVICE_CC);
								continue;
							}
						}
						
						if(FatFs_Read_WBCHGB_Info(tCountHead->ulCurIndex, WBCHGBTestInfo) == FEED_BACK_FAIL){
							//重新读取一次
							if(FatFs_Read_WBCHGB_Info(tCountHead->ulCurIndex, WBCHGBTestInfo) == FEED_BACK_FAIL){
								//读取的数据校验异常，直接跳过
								ccSendIndex = Add_WBCHGB_SendIndex(SERVICE_CC);
								continue;
							}
						}
						
						//只要调用算法产生数据则将结果数据上传到服务端
						if(tCountHead->ucUseAlgoFlag == 1){
							//发送样本分析数据
							if(CAT_B_Send_SampleInfo(WBCHGBTestInfo,tCountHead) != HAL_OK){
								Save_DataMange_Info(&g_tDataManage);
								
								CATGlobalStatus.flag.connServerErr = 1;
								
								osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
								
								goto CAT_iotTaskTag;
							}
						}
						
						//判断是否存在报警情况
						AlarmFlag = 0;
						for(i=0;i<8;i++){
							if(WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0 && WBCHGBTestInfo->ucaWBCHGB_RstAlert[i] != 0xff){
								AlarmFlag = 1;
								break;
							}
						}
						
						//判断测试是否出现异常，只上传异常日志到服务端
                        if(tCountHead->eErrorCode != ERROR_CODE_SUCCESS || tCountHead->eHideErrorCode != ERROR_CODE_SUCCESS || AlarmFlag != 0){
                             //读取日志信息
                             if(FatFs_Read_ConutLog(tCountHead->ulCurIndex, ptCountLog, COUNT_LOG_DEFAULT_MAX_LEN) == FEED_BACK_FAIL){
								 //重新读取一次
								 if(FatFs_Read_ConutLog(tCountHead->ulCurIndex, ptCountLog, COUNT_LOG_DEFAULT_MAX_LEN) == FEED_BACK_FAIL){
									ccSendIndex = Add_WBCHGB_SendIndex(SERVICE_CC);
									continue;
								 }
							}
							 
							ulLogLen = strlen((char*)(ptCountLog->ucaLogBuffer));                           
								
							//发送日志
							if(CAT_B_Send_SampleLog(WBCHGBTestInfo,tCountHead,ptCountLog->ucaLogBuffer, ulLogLen) != HAL_OK){
								Save_DataMange_Info(&g_tDataManage);
								
								CATGlobalStatus.flag.connServerErr = 1;
								
								osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
								
								goto CAT_iotTaskTag;
							}
						}
						
                        ccSendIndex = Add_WBCHGB_SendIndex(SERVICE_CC);
					}
					
					Save_DataMange_Info(&g_tDataManage);
					
					//去除必发标志
					if(MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType == ACCOUNT_DATA_TYPE_OUTSIDE){
						MachInfo.catMastSynServEvent.bit.outtorSampleResult = 0;
					}else{
						MachInfo.catMastSynServEvent.bit.innerSampleResult = 0;
					}
					EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
				}
				break;
				
				
				//修改了样本分析数据后唤醒任务
				case CHANGE_SAMPLE_ANALYSE_CAT_MSG_TYPE:
				{
					WBCHGB_TestInfo_t* WBCHGBTestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
                    CountHead_t* tCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
					uint32_t ulTailIndex = 0, ccSendIndex = 0;
					uint32_t TempSendIndex = Get_WBCHGB_SendIndex(SERVICE_CC);
                    
					//从头开始寻找
					Set_WBCHGB_SendIndex(SERVICE_CC,Get_WBCHGB_Head());
					
                    //读取SN
					ccSendIndex = Get_WBCHGB_SendIndex(SERVICE_CC);
                    ulTailIndex = Get_WBCHGB_Tail();                 
                    while(ccSendIndex != ulTailIndex){
                        //读取公共头信息
                        if(FatFs_Read_ConutHead(ccSendIndex, tCountHead) == FEED_BACK_FAIL){
							//重新读取一次
							if(FatFs_Read_ConutHead(ccSendIndex, tCountHead) == FEED_BACK_FAIL){
								//读取的数据校验异常，直接跳过
								ccSendIndex = Add_WBCHGB_SendIndex(SERVICE_CC);
								continue;
							}
						}
						
						//判断是否是修改过的数据
						if(tCountHead->eStatus != DATA_STATUS_CHANGE){
							ccSendIndex = Add_WBCHGB_SendIndex(SERVICE_CC);
							continue;
						}
						
						if(FatFs_Read_WBCHGB_Info(tCountHead->ulCurIndex, WBCHGBTestInfo) == FEED_BACK_FAIL){
							//重新读取一次
							if(FatFs_Read_WBCHGB_Info(tCountHead->ulCurIndex, WBCHGBTestInfo) == FEED_BACK_FAIL){
								//读取的数据校验异常，直接跳过
								ccSendIndex = Add_WBCHGB_SendIndex(SERVICE_CC);
								continue;
							}
						}
						
						//只要调用算法产生数据则将结果数据上传到服务端
						if(tCountHead->ucUseAlgoFlag == 1){
							//发送修改后的样本分析数据
							if(CAT_B_Send_ChangeSampleInfo(WBCHGBTestInfo,tCountHead) != HAL_OK){
								CATGlobalStatus.flag.connServerErr = 1;
								
								osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
								
								goto CAT_iotTaskTag;
							}
						}
						
						//去除修改标志
						//先判断是否两个服务端都已经发送完毕了，是，则擦除数据修改标志
						if(MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType == ACCOUNT_DATA_TYPE_OUTSIDE){
							if(MachInfo.catMastSynServEvent.bit.otherOuttorChangeSampleResult == 0){
								tCountHead->eStatus = DATA_STATUS_NORMAL;
								tCountHead->usCrc = CRC16((uint8_t*)tCountHead, sizeof(CountHead_t)-4);
								FatFs_Write_CountHead(ccSendIndex, tCountHead);
							}
						}else{
							if(MachInfo.catMastSynServEvent.bit.otherInnerChangeSampleResult == 0){
								tCountHead->eStatus = DATA_STATUS_NORMAL;
								tCountHead->usCrc = CRC16((uint8_t*)tCountHead, sizeof(CountHead_t)-4);
								FatFs_Write_CountHead(ccSendIndex, tCountHead);
							}
						}
						
                        ccSendIndex = Add_WBCHGB_SendIndex(SERVICE_CC);
					}
					
					//恢复SendIndex原始值
					Set_WBCHGB_SendIndex(SERVICE_CC,TempSendIndex);
					
					//去除必发标志
					if(MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType == ACCOUNT_DATA_TYPE_OUTSIDE){
						MachInfo.catMastSynServEvent.bit.outtorChangeSampleResult = 0;
					}else{
						MachInfo.catMastSynServEvent.bit.innerChangeSampleResult = 0;
					}
					EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
				}
				break;
				
				
				//参数部分修改后唤醒任务
				case SET_PARA_OVER_CAT_MSG_TYPE:
				{
					//获取TX信号量
					osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
					
					//发送当前仪器中所有的参数信息
					if(CAT_Send_MachineAllPara() != HAL_OK){
						//释放信号量
						osSemaphoreRelease(CATSemphrCountTxHandle);
						
						CATGlobalStatus.flag.connServerErr = 1;
						
						osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
						
						goto CAT_iotTaskTag;
					}
					
					//释放信号量
					osSemaphoreRelease(CATSemphrCountTxHandle);
					
					//清除事件触发标记
					MachInfo.catMastSynServEvent.bit.paraSet = 0;
					EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
				}
				break;
					
					
				//修改仪器部分参数唤醒任务
				case CHANGE_MACHINE_INFO_CAT_MSG_TYPGE:
				{
					switch(CATQueueInfoBuf.para.changeMachineInfo.changeType){
						//通知服务端，修改了仪器序列号
						case 0:
							if(CAT_B_Send_ChangeMachineInfo(0,NULL,0) != HAL_OK){
								//发送修改序列号失败事件
								osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_ERR);
								
								//这里挂起此任务，等待重启
								osThreadSuspend(CATTaskHandle);
							}else{
								//发送修改序列号成功事件
								osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CHANGE_MACHINE_INFO_OK);
								
								//这里挂起此任务，等待重启
								osThreadSuspend(CATTaskHandle);
							}
						break;
							
						//通知服务端，清除了测试数据
						case 1:
							if(CAT_B_Send_ChangeMachineInfo(1,NULL,0) != HAL_OK){
								CATGlobalStatus.flag.connServerErr = 1;
								
								osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
								
								goto CAT_iotTaskTag;
							}
							
							//清除事件触发标记
							MachInfo.catMastSynServEvent.bit.delTestData = 0;
							EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
						break;
							
						//通知服务端，清除了用户数据
						case 2:
							if(CAT_B_Send_ChangeMachineInfo(2,NULL,0) != HAL_OK){
								CATGlobalStatus.flag.connServerErr = 1;
								
								osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
								
								goto CAT_iotTaskTag;
							}
							
							//清除事件触发标记
							MachInfo.catMastSynServEvent.bit.delUserData = 0;
							EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
						break;
							
						//通知服务端，恢复出厂设置
						case 3:
							if(CAT_B_Send_ChangeMachineInfo(3,NULL,0) != HAL_OK){
								//发送恢复出厂失败事件
								osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_RECOVER_FACTORY_ERR);
								
								//这里挂起此任务，等待重启
								osThreadSuspend(CATTaskHandle);
							}
							
							osSemaphoreAcquire(CATSemphrCountTxHandle,osWaitForever);
							//发送当前仪器中所有的参数信息
							CAT_Send_MachineAllPara();
							osSemaphoreRelease(CATSemphrCountTxHandle);
							
							//发送修改序列号成功事件
							osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_RECOVER_FACTORY_OK);
							
							//这里挂起此任务，等待重启
							osThreadSuspend(CATTaskHandle);
						break;
							
						//重置用户开机时间
						case 4:
							if(CAT_B_Send_ChangeMachineInfo(4,NULL,0) != HAL_OK){
								CATGlobalStatus.flag.connServerErr = 1;
								
								osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
								
								goto CAT_iotTaskTag;
							}
							
							//清除事件触发标记
							MachInfo.catMastSynServEvent.bit.resetUserOpenTimestamp = 0;
							EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
						break;
						
						default : break;
					}
				}
				break;
					
				//发送运行日志
				case SEND_RUN_LOG_CAT_MSG_TYPGE:
				{
					extern  __IO DataManage_t g_tDataManage;
					RunLog_s* RunLog = (RunLog_s*)SDRAM_ApplyCommonBuf(sizeof(RunLog_s));
					
                    while(g_tDataManage.ulRunLog_SendIndex != g_tDataManage.ulRunLog_Queue_TailIndex)
                    {
						//从存储器中获取结果数据
                        if(FatFs_Read_RunLog(g_tDataManage.ulRunLog_SendIndex, RunLog) != FEED_BACK_SUCCESS){
							//重新读取一次
							if(FatFs_Read_RunLog(g_tDataManage.ulRunLog_SendIndex, RunLog) != FEED_BACK_SUCCESS){
								Add_RunLog_SendIndex();
								continue;
							}
						}
						
						//发送运行日志
						if(CAT_B_Send_RunLog(RunLog) != HAL_OK){
							Save_DataMange_Info(&g_tDataManage);
							
							CATGlobalStatus.flag.connServerErr = 1;
							
							osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
							
							goto CAT_iotTaskTag;
						}

						Add_RunLog_SendIndex();
					}					
					Save_DataMange_Info(&g_tDataManage);
					
					//去除必发标志
					MachInfo.catMastSynServEvent.bit.sendRunLog = 0;
					EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
				}
				break;
				
				//发送仪器自检结果状态唤醒任务
				case SELF_CHECK_RESULT_CAT_MSG_TYPE:
				{
					//发送当前仪器中所有的参数信息
					if(CAT_Send_MachineSelfCheck((uint8_t*)&CATQueueInfoBuf.para.CATMachSelfCheckResult,sizeof(CAT_MachSelfCheckResult_s)) != HAL_OK){
						CATGlobalStatus.flag.connServerErr = 1;
						
						osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
						
						goto CAT_iotTaskTag;
					}
				}
				break;
				
				//主动升级程序到发布版或者测试版，这里均为强制升级，即不判断各模块的版本号直接强制升级
				case ACTIVE_UPDATE_CAT_MSG_TYPE:
				{
					//临时提高此任务优先级
					if(osThreadGetPriority(CATTaskHandle) != osPriorityHigh){
						CurrentTaskPri = osThreadGetPriority(CATTaskHandle);
						osThreadSetPriority(CATTaskHandle,osPriorityHigh);
					}
					
					if(_CAT_UpdateSoftware(CATQueueInfoBuf.para.forceUpdate.versionType,CATQueueInfoBuf.para.forceUpdate.updateType) != HAL_OK){
						//还原此任务优先级
						if(osThreadGetPriority(CATTaskHandle) == osPriorityHigh){
							osThreadSetPriority(CATTaskHandle,CurrentTaskPri);
						}
						
						CATGlobalStatus.flag.connServerErr = 1;
						
						osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
						
						goto CAT_iotTaskTag;
					}
					
					//重新开启MQTT连接
					if(CATGlobalStatus.flag.mqttConn == 0){
						CAT_B_CreateMQTTConn(SERVICE_CC);
					}
					
					//还原此任务优先级
					if(osThreadGetPriority(CATTaskHandle) == osPriorityHigh){
						osThreadSetPriority(CATTaskHandle,CurrentTaskPri);
					}
				}
				break;
				
				//向服务端请求指定患者编号的患者信息
				case REQUEST_PATIENT_ID_CAT_MSG_TYPE:
				{
					if(CAT_B_Send_RequestPatientInfo(CATQueueInfoBuf.para.Str) != HAL_OK){
						CATGlobalStatus.flag.connServerErr = 1;
						
						osMessageQueuePut(CATQueueHandle,&CATQueueInfoBuf,0,0);
						
						goto CAT_iotTaskTag;
					}
				}
				break;
				
				//手动刷新网络事件，这里不需要处理啥
				case REFRESH_NET_CAT_MSG_TYPE:
				{
					extern osEventFlagsId_t GlobalEventCatGroupHandle;
					
					switch(MachInfo.companyInfo.company){
						case COMPANY_YSB:
						{
							//药师帮
							//当存在第三方服务端时，则这里不能发送此标志，这里不做任何处理
						}break;
						
						default :
						{
							osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REFRESH_NET);
						}break;
					}
				}break;
				
				default :break;
			}
		}while(osMessageQueueGet(CATQueueHandle,&CATQueueInfoBuf,0,0) == osOK);	
		
		//消息队列中已经全部取完，询问服务端是否有数据发送
		//询问后台是否有数据发送给仪器
		if(CAT_B_IsServiceWantToSendData() != HAL_OK){
			CATGlobalStatus.flag.connServerErr = 1;
			goto CAT_iotTaskTag;
		}
	
		//更新计时起点
		AlarmCATTimeupCount = 0;
		
		//判断是否因CAT动作处理导致需要重启仪器
		if(CATGlobalStatus.catDealSysRestartFlag == 0xA55A0FF0){
			osDelay(1000);
					
			//重新登录
			__set_PRIMASK(1);
			NVIC_SystemReset();
		}
		
CAT_iotTaskTag:
		//断开MQTT连接
		CAT_B_CloseMQTTConn();
		
		//释放CAT模块与服务端通讯权
		osSemaphoreRelease(CATSemMutexHandle);
	}
}









/***
*第三方CAT服务端相关收发任务，原则上：
1.第三方服务端的收发任务优先级要高于创怀
2.当不存在第三方服务端时，则将此任务和对应的消息队列删除
3.创建一个计数信号量用于CAT与第三方服务端和创怀服务端通讯时进行互斥，避免数据串扰
***/
void CAT_OtherTask(void *argument)
{
	extern osThreadId_t CATOtherTaskHandle;
	extern osMessageQueueId_t CATQueueOtherHandle;
	extern osEventFlagsId_t GlobalEventCatGroupHandle;
	extern CATGlobalStatus_s CATGlobalStatus;
	CATQueueInfoBuf_s CATQueueInfoBuf;
	uint16_t RetryCount = 0;
	
	//初始化网络状态标志
	CATGlobalStatus.flag.noOtherServer = 1;
	CATGlobalStatus.flag.connOtherServerErr = 1;
	MachRunPara.netStatus.flag = 0;
	
	//清除第三方服务端通讯的任务启动事件标志
	osEventFlagsClear(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_START);
	
	//首先判断IP，用户名，密码是否存在，不存在，则表示不需要和第三方服务端进行通讯，将输出任务和对应的消息队列
	if(strlen(MachInfo.companyInfo.mqtt.ip) == 0 || strlen(MachInfo.companyInfo.mqtt.name) == 0 || strlen(MachInfo.companyInfo.mqtt.psw) == 0){
		//不存在第三方服务端
		CATGlobalStatus.flag.noOtherServer = 1;
		CATGlobalStatus.flag.connOtherServerErr = 1;
		
		osMessageQueueDelete(CATQueueOtherHandle);
		osThreadTerminate(CATOtherTaskHandle);
		return;
	}
	
	CATGlobalStatus.flag.noOtherServer = 0;
	
	
	//等待与第三方服务端通讯的任务启动事件
	osEventFlagsWait(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_OTHER_SER_START,osFlagsWaitAny,osWaitForever);
	
	//获取CAT模块与服务端通讯权
	osSemaphoreAcquire(CATSemMutexHandle,osWaitForever);
	
	//建立MQTT连接
	while(CAT_B_CreateMQTTConn(SERVICE_OTHER) != HAL_OK){
		CATGlobalStatus.flag.connOtherServerErr = 1;
		
		//断开MQTT连接
		CAT_B_CloseMQTTConn();
		
		osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR);
		
		//记录网络首次中断时刻状态
		if(MachRunPara.netStatus.flag == 0){
			RTC_HYM8563Info_s RTCHym8563Info;
			
			MachRunPara.netStatus.flag = 1;
			
			//获取当前时间戳
			RTC_HYM8563GetTime(&RTCHym8563Info);										
			MachRunPara.netStatus.interruptTimestamp = GetCurrentTimestamp(RTCHym8563Info);
			
			//获取当前信号强度
			CAT_GetSignalStrength();
			MachRunPara.netStatus.interruptSignalStrength = CATGlobalStatus.signalStrength;
		}
		
		//释放CAT模块与服务端通讯权
		osSemaphoreRelease(CATSemMutexHandle);
		
		osDelay(10000);
		
		//获取CAT模块与服务端通讯权
		osSemaphoreAcquire(CATSemMutexHandle,osWaitForever);
		
		CATGlobalStatus.flag.connOtherServerErr = 0;
	}
	
	
	CATGlobalStatus.flag.connOtherServerErr = 0;
	
	//发送设备当前开机信息
	while(CAT_B_Other_Send_MachineOpenInfo() != HAL_OK){
		CATGlobalStatus.flag.connOtherServerErr = 1;
		
		CAT_B_CloseMQTTConn();
		
		osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR);
		
		//释放CAT模块与服务端通讯权
		osSemaphoreRelease(CATSemMutexHandle);
		
		osDelay(10000);
		
		//获取CAT模块与服务端通讯权
		osSemaphoreAcquire(CATSemMutexHandle,osWaitForever);
		
		CATGlobalStatus.flag.connOtherServerErr = 0;
		CAT_B_CreateMQTTConn(SERVICE_OTHER);
	}
	
	//询问第三方服务端是否有数据发送给仪器
	while(CAT_B_Other_IsServiceWantToSendData() != HAL_OK){
		CAT_B_CloseMQTTConn();
		CATGlobalStatus.flag.connOtherServerErr = 1;
		
		osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR);
		
		//释放CAT模块与服务端通讯权
		osSemaphoreRelease(CATSemMutexHandle);
		
		osDelay(10000);
		
		//获取CAT模块与服务端通讯权
		osSemaphoreAcquire(CATSemMutexHandle,osWaitForever);
		
		CATGlobalStatus.flag.connOtherServerErr = 0;
		CAT_B_CreateMQTTConn(SERVICE_OTHER);
	}
	
	//断开MQTT连接
	CAT_B_CloseMQTTConn();
	
	//释放CAT模块与服务端通讯权
	osSemaphoreRelease(CATSemMutexHandle);
	
	while(1){
		//等待消息
		osMessageQueueGet(CATQueueOtherHandle,&CATQueueInfoBuf,0,osWaitForever);
		
		//获取CAT模块与服务端通讯权
		osSemaphoreAcquire(CATSemMutexHandle,osWaitForever);
		
		//建立MQTT连接
		if(CAT_B_CreateMQTTConn(SERVICE_OTHER) != HAL_OK){
			CATGlobalStatus.flag.connOtherServerErr = 1;
			
			//不同事件执行不同的重发机制，需要等待服务端响应的则重发2次后就判定为超时，不需要服务端响应的则可以一直重发
			switch(CATQueueInfoBuf.msgType){
				//界面上存在弹窗，需要超时结束，重发2次
				case REQUEST_PATIENT_ID_CAT_MSG_TYPE:				//向服务端请求指定患者编号的患者信息
				case REFRESH_NET_CAT_MSG_TYPE:						//手动刷新网络事件
				{
					RetryCount++;
					if(RetryCount <= 2){
						osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0,0);
					}else{
						RetryCount = 0;
						osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_CONN_OTHER_NET_ERR);
					}
				}break;
				
				//可以一直重发的事件
				default :
				{
					osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0,0);
				}break;
			}
			
			//断开MQTT连接
			CAT_B_CloseMQTTConn();
			
			//记录网络首次中断时刻状态
			if(MachRunPara.netStatus.flag == 0){
				RTC_HYM8563Info_s RTCHym8563Info;
				
				MachRunPara.netStatus.flag = 1;
				
				//获取当前时间戳
				RTC_HYM8563GetTime(&RTCHym8563Info);										
				MachRunPara.netStatus.interruptTimestamp = GetCurrentTimestamp(RTCHym8563Info);
				
				//获取当前信号强度
				CAT_GetSignalStrength();
				MachRunPara.netStatus.interruptSignalStrength = CATGlobalStatus.signalStrength;
			}
			
			//释放CAT模块与服务端通讯权
			osSemaphoreRelease(CATSemMutexHandle);
			
			//信号差，延迟10s后重新尝试连接
			osDelay(10000);
			
			continue;
		}
		
		//记录网络从中断到重新恢复的时刻状态
		if(MachRunPara.netStatus.flag != 0){
			RTC_HYM8563Info_s RTCHym8563Info;
			CATQueueInfoBuf_s CATQueueInfoBufTemp = {0};
			
			//获取当前时间戳
			RTC_HYM8563GetTime(&RTCHym8563Info);										
			MachRunPara.netStatus.recoveryTimestamp = GetCurrentTimestamp(RTCHym8563Info);
			
			//获取当前信号强度
			CAT_GetSignalStrength();
			MachRunPara.netStatus.recoverySignalStrength = CATGlobalStatus.signalStrength;
			
			CATQueueInfoBufTemp.msgType = NET_RECOVER_CAT_MSG_TYPE;
			osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBufTemp,0,0);
		}
		
		CATGlobalStatus.flag.connOtherServerErr = 0;
		
		//间隔100ms，给服务端订阅设备主题提供缓冲时间
		osDelay(100);
		
		//将队列中的消息全部取出来，待定...
		do{
			switch(CATQueueInfoBuf.msgType){
				//定时唤醒任务
				case TIME_UP_CAT_MSG_TYPE:
				{
					//发送当前信号强度
					if(CAT_B_Other_Send_SignalStrength() != HAL_OK){
						CATGlobalStatus.flag.connOtherServerErr = 1;
						
						osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0,0);
						
						goto CAT_OtherTaskTag;
					}
				}
				break;
				
				//发送仪器自检结果状态唤醒任务
				case SELF_CHECK_RESULT_CAT_MSG_TYPE:
				{
					if(CAT_Other_Send_MachineSelfCheck((uint8_t*)&CATQueueInfoBuf.para.CATMachSelfCheckResult,sizeof(CAT_MachSelfCheckResult_s)) != HAL_OK){
						CATGlobalStatus.flag.connOtherServerErr = 1;
						
						osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0, 0);
						
						goto CAT_OtherTaskTag;
					}
				}
				break;
				
				//样本分析完毕后唤醒任务
				case SAMPLE_ANALYSE_CAT_MSG_TYPE:
				{
					WBCHGB_TestInfo_t* WBCHGBTestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
                    CountLog_t* ptCountLog = (CountLog_t*)SDRAM_ApplyCommonBuf(sizeof(CountLog_t));
                    CountHead_t* tCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
					uint32_t ulTailIndex = 0, ccSendIndex = 0;
                    
                    //读取SN
                    ccSendIndex  = Get_WBCHGB_SendIndex(SERVICE_OTHER);
                    ulTailIndex = Get_WBCHGB_Tail();                 
                    while(ccSendIndex != ulTailIndex){
                        //读取公共头信息
                        if(FatFs_Read_ConutHead(ccSendIndex, tCountHead) == FEED_BACK_FAIL){
							//重新读取一次
							if(FatFs_Read_ConutHead(ccSendIndex, tCountHead) == FEED_BACK_FAIL){
								//读取的数据校验异常，直接跳过
								ccSendIndex = Add_WBCHGB_SendIndex(SERVICE_OTHER);
								continue;
							}
						}
						
						if(FatFs_Read_WBCHGB_Info(tCountHead->ulCurIndex, WBCHGBTestInfo) == FEED_BACK_FAIL){
							//重新读取一次
							if(FatFs_Read_WBCHGB_Info(tCountHead->ulCurIndex, WBCHGBTestInfo) == FEED_BACK_FAIL){
								//读取的数据校验异常，直接跳过
								ccSendIndex = Add_WBCHGB_SendIndex(SERVICE_OTHER);
								continue;
							}
						}
						
						//判断测试是否出现异常，只上传异常日志到服务端
                        if(tCountHead->eErrorCode != ERROR_CODE_SUCCESS){
                             //发送日志
							if(CAT_B_Other_Send_SampleLog(WBCHGBTestInfo,tCountHead) != HAL_OK){
								Save_DataMange_Info(&g_tDataManage);
								
								CATGlobalStatus.flag.connOtherServerErr = 1;
								
								osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0, 0);
								
								goto CAT_OtherTaskTag;
							}
						}else{
							//发送样本分析数据
							if(CAT_B_Other_Send_SampleInfo(WBCHGBTestInfo,tCountHead) != HAL_OK){
								Save_DataMange_Info(&g_tDataManage);
								
								CATGlobalStatus.flag.connOtherServerErr = 1;
								
								osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0, 0);
								
								goto CAT_OtherTaskTag;
							}
						}
						
                        ccSendIndex = Add_WBCHGB_SendIndex(SERVICE_OTHER);
					}
					
					Save_DataMange_Info(&g_tDataManage);
					
					//去除必发标志
					if(MachInfo.accountMM.accountInfo[MachInfo.accountMM.curIndex].dataType == ACCOUNT_DATA_TYPE_OUTSIDE){
						MachInfo.catMastSynServEvent.bit.otherOuttorSampleResult = 0;
					}else{
						MachInfo.catMastSynServEvent.bit.otherInnerSampleResult = 0;
					}
					EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_CAT_SYNC_SER_EVENT,(uint8_t*)&MachInfo.catMastSynServEvent,sizeof(MachInfo.catMastSynServEvent));
				}
				break;
				
				//向服务端请求指定患者编号的患者信息
				case REQUEST_PATIENT_ID_CAT_MSG_TYPE:
				{
					if(CAT_B_Other_Send_RequestPatientInfo(CATQueueInfoBuf.para.TempU8) != HAL_OK){
						CATGlobalStatus.flag.connOtherServerErr = 1;
						
						osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0, 0);
						
						goto CAT_OtherTaskTag;
					}
				}
				break;
				
				//手动刷新网络事件，这里不需要处理啥
				case REFRESH_NET_CAT_MSG_TYPE:
				{
					extern osEventFlagsId_t GlobalEventCatGroupHandle;
					
					osEventFlagsSet(GlobalEventCatGroupHandle,GLOBAL_EVENT_CAT_REFRESH_NET);
				}break;
				
				//向服务端发送即将进行测试的患者订单号
				case SEND_PATIENT_ORDER_NUM_CAT_MSG_TYPE:
				{
					if(CAT_B_Other_Send_ReadyTestOrderNum(CATQueueInfoBuf.para.Str) != HAL_OK){
						CATGlobalStatus.flag.connOtherServerErr = 1;
						
						osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0, 0);
						
						goto CAT_OtherTaskTag;
					}
				}break;
				
				//向服务端发送网络中断与恢复情况
				case NET_RECOVER_CAT_MSG_TYPE:
				{
					if(CAT_B_Other_Send_NetStatus() != HAL_OK){
						CATGlobalStatus.flag.connOtherServerErr = 1;
						
						osMessageQueuePut(CATQueueOtherHandle,&CATQueueInfoBuf,0, 0);
						
						goto CAT_OtherTaskTag;
					}
					
					MachRunPara.netStatus.flag = 0;
				}break;
				
				
				default :break;
			}
		}while(osMessageQueueGet(CATQueueOtherHandle,&CATQueueInfoBuf,0,0) == osOK);
		
CAT_OtherTaskTag:
		//断开MQTT连接
		CAT_B_CloseMQTTConn();
		
		//释放CAT模块与服务端通讯权
		osSemaphoreRelease(CATSemMutexHandle);
		
		osDelay(5);
	}
}

















