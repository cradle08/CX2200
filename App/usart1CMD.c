/************************************************************
*����1����������ش���
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





/*ȫ�ֱ���������*********************************************/
Usart1ReceBuf_s Usart1ReceBuf = {0};

//����1�������ݶ�ֵ�ź���
osSemaphoreId_t Usart1RxSempBinHandle = NULL;




/*�ڲ�������*******************************************************/
/***
*��ȡ��ֵ
������ֵΪ:0xFFFFFFFFʱ��ʾ��ȡ��ֵʧ��
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
*�������ָ�����
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
	
	//Ϊ0�رչ�װ�źţ�
    if(Mode==6 || Mode == 7 || Mode == 11) //�ڲ���װģʽ
    {
        if(fRadio == 0)
        {
            //��ͨ���ع�װ�ź�
            COUNT_SIGNAL_SW_OFF;
            //�ر��ź�
            Count_Signal_Stop();
            //
            osSemaphoreAcquire(xSema_Count_Key, 0); 
            Set_CountMode(COUNT_MODE_NORMAL);
        }else{
            //��ͨ���ع�װ�ź�
            COUNT_SIGNAL_SW_ON;
            //���ð��ع�װ�źŵ�*�ձȣ������ź�
            Count_Signal_SetFreq(fRadio);
            Count_Signal_Start();
        }
    }
    
    //����ģʽ
    if(Mode == 1){
        //������������
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_NORMAL;
        osSemaphoreRelease(xSema_Count_Key);  
    }else if(Mode == 2){
        //�����������ظ�ʹ��ģʽ��
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_RE_USE;
        osSemaphoreRelease(xSema_Count_Key);  
    }else if(Mode == 3){
        //��������ģʽ�������ֶ¿ף�����100V����
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_100V_FIRE;
        osSemaphoreRelease(xSema_Count_Key);  
    }else if(Mode == 4){
        //�ⲿ��װ�źŷ���ģ����ԣ�ȫ����ģʽ
     //   Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIMULATION_PROCESS_OUTSIDE;
      //  osSemaphoreRelease(xSema_Count_Key);  
    }else if(Mode == 5){
        //�ⲿ��װ�źŷ���ģ�����,���ź�ģʽ
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE;
        osSemaphoreRelease(xSema_Count_Key);  
    }else if(Mode == 6){   
        //�ڲ���װ�źŷ���ģ����ԣ��Զ�ģʽ      
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIMULATION_SIGNAL_INNER_AUTO;
        osSemaphoreRelease(xSema_Count_Key);  
     }else if(Mode == 7){
        //�ڲ���װ�źŷ���ģ�����, ���δ��ź�ģʽ
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIMULATION_SIGNAL_INNER;
        osSemaphoreRelease(xSema_Count_Key);
    }else if(Mode == 8){
         //�ź�Դģʽ
         Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIGNAL_SRC;
        osSemaphoreRelease(xSema_Count_Key);
    }else if(Mode == 9){
         //�������ⲿ��װ�źŷ���ģ�����,���ź�,�Զ�ִ��ģʽ
         Get_CountMode_Info()->ulCountMode = COUNT_MODE_SIMULATION_SIGNAL_OUTSIDE_AUTO;
         osSemaphoreRelease(xSema_Count_Key); 
    }else if(Mode == 10){
        //HGB
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_HGB;
        osSemaphoreRelease(xSema_Count_Key);
    }else if(Mode == 11){
        //�ʿ��ڲ���װ�źŷ���ģ����ԣ��Զ�ģʽ      
        Get_CountMode_Info()->ulCountMode = COUNT_MODE_QC_INNER_AUTO;
        osSemaphoreRelease(xSema_Count_Key); 
    }else{
		printf("CMD input error!!!\r\n");
	}
}



/***
*���ָ���������ݻ�����־
***/
static void _CMD_OutputDataLog(uint16_t Mode,uint32_t SampleSN)
{
	uint32_t ulQueueMaxNum = 0, i = 0;

	ulQueueMaxNum = Get_WBCHGB_Queue_MaxNum();
	SampleSN = (SampleSN%ulQueueMaxNum);

	if(Mode == 1)
	{
		WBCHGB_TestInfo_t* tWBCHGB_TestInfo = (WBCHGB_TestInfo_t*)SDRAM_ApplyCommonBuf(sizeof(WBCHGB_TestInfo_t));
		
		//��ȡWBCHGB����
		FatFs_Read_WBCHGB_Info(SampleSN, tWBCHGB_TestInfo);
		Print_WBCHGB_TestInfo(tWBCHGB_TestInfo);
	}else if(Mode == 2){
		//��ȡWBCHGB��־
		CountLog_t* ptCountLog = (CountLog_t*)SDRAM_ApplyCommonBuf(sizeof(CountLog_t));
		memset((void*)ptCountLog, 0, LOG_INFO_MAX_LEN);
		LOG_Info("Len=%d, Crc=%d", ptCountLog->usLogLen, ptCountLog->usCrc);
		FatFs_Read_ConutLog(SampleSN, ptCountLog, LOG_INFO_MAX_LEN);
		for(i = 0; i < ptCountLog->usLogLen; i++)
		{
			printf("%c", ptCountLog->ucaLogBuffer[i]);
		}
	}else if(Mode == 3){
		//����ͷ��Ϣ
		CountHead_t* tCountHead = (CountHead_t*)SDRAM_ApplyCommonBuf(sizeof(CountHead_t));
		FatFs_Read_ConutHead(SampleSN, tCountHead);
		Print_Count_Head(tCountHead);
	}else{
		printf("CMD input error!!!\r\n");
	}
}



/***
*˫������HGB����
***/
static void _CMD_2LEDHGBTest(uint32_t Current535,uint32_t Current415)
{
	uint32_t Value535 = 0;
	uint32_t Value415 = 0;
	
	//����535����
	Light_Start(HGBLED_535,Current535);
	osDelay(5000);
	Value535 = HGB_ADC();
	Light_Stop();
	
	osDelay(500);
	
	//����415����
	Light_Start(HGBLED_415,Current415);
	osDelay(5000);
	Value415 = HGB_ADC();
	Light_Stop();
	
	printf("535ADC:%u,,,415ADC:%u\r\n",Value535,Value415);
}









/*�ⲿ������*******************************************************/
/***
*����1�������ݴ�������
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
			//�ж�֡ͷAT:
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
			
			//ִ������
			switch(Usart1ReceBuf.buf[Usart1ReceBuf.readIndex]){
				//��ӡ���е�����
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
					
					//��ӡ�����е�����
					printf("***ALL CMD************************************\r\n");
					printf("AT:G\t��ȡ��������\r\n");
					printf("AT:C_X_\t���������XΪ1���������������XΪ2���������������XΪ3��������ݹ������\r\n");
					printf("AT:S_X_\t��ӡ״̬������XΪ1����ӡ����ջ��Ϣ\r\n");
					printf("AT:J_X_Y_\t��������\r\n \
					XΪ1������������XΪ2��������Reuse��XΪ3���������գ� \r\n \
					XΪ4���⹤װȫ���̣�XΪ5���⹤װ���źţ�XΪ6���ڹ�װ�Զ�; \r\n \
					XΪ7���ڹ�װ���δ��źţ�XΪ8���ź�Դģʽ��XΪ9���⹤װ���ź��Զ�ִ�У� \r\n \
					XΪ10��HGB��XΪ11���ʿ��ڹ�װ�Զ�\r\n \
					YΪ�ڲ���װ�ź�ռ�ձȣ�����ģʽʱYΪ0���ɣ�YΪ1��0.5��Ϊ2��1��Ϊ3��20\r\n");
					printf("AT:P_X_Y_\t����������ݻ���־��XΪ1������������ݣ�XΪ2�������־��XΪ3���������ͷ��Y��ʾ�������\r\n");
					printf("AT:T_X_Y_\t˫����HGB���ԣ�X��535�����Ƶ���ֵ��Y��415�����Ƶ���ֵ\r\n");
					printf("***CMD END************************************\r\n");
				}break;
				
				//�������
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
						//�����������
						case 1:
						{
							MachInfo.initFlag = 0;
							EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EPPROM_ADDR_MACHINFO_INIT_FLAG,(uint8_t*)&MachInfo.initFlag,sizeof(MachInfo.initFlag));
							
							osDelay(1000);
					
							//���µ�¼
							__set_PRIMASK(1);
							NVIC_SystemReset();
						}break;
						
						//�����������
						case 2:
						{
							MachInfo.touch.flag = 0;
							EEPROMWriteData(EEPROM_24C32,MAIN_BORD_EEPROM_24C32,EEPROM_ADDR_MACHINFO_INIT_TOUCH,(uint8_t*)&MachInfo.touch,sizeof(MachInfo.touch));
							
							osDelay(1000);
					
							//���µ�¼
							__set_PRIMASK(1);
							NVIC_SystemReset();
						}break;
						
						//������ݹ������
						case 3:
						{
							Set_Default_DataManage(&g_tDataManage);
							Save_DataMange_Info(&g_tDataManage);
							
							osDelay(1000);
					
							//���µ�¼
							__set_PRIMASK(1);
							NVIC_SystemReset();
						}break;
						
						case 0xFFFF:printf("CMD input error!!!\r\n");break;
						default :printf("CMD input error!!!\r\n");break;
					}
				}break;
				
				//��ӡ״̬��Ϣ
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
						//��ӡ����ջ��Ϣ
						case 1:
						{
							Printf_Free_StackSize();
						}break;
						
						case 0xFFFF:printf("CMD input error!!!\r\n");break;
						default :printf("CMD input error!!!\r\n");break;
					}
				}break;
				
				//�������
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
				
				//������ݻ�����־
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
				
				//˫����HGB����
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

















