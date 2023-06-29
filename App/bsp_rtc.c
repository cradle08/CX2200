/****************************************************************************
*HYM8563�ⲿRTCоƬ��I2CͨѶ���ӻ���ַ0x51��400kbps
*****************************************************************************/
#include "bsp_rtc.h"
#include "i2c.h"
#include "msg_def.h"
#include "main.h"
#include "time.h"




/*�궨����******************************************************************/
#define SLAVE_ADR_HYM8563						0x51					//�ӻ���ַ

#define REG_ADR_SEC								0x02
#define REG_ADR_MIN								0x03
#define REG_ADR_HOUR							0x04
#define REG_ADR_DAY								0x05
#define REG_ADR_WEEK							0x06
#define REG_ADR_MONTH							0x07
#define REG_ADR_YEAR							0x08





/*ȫ�ֱ���������************************************************************/
//RTC_HYM8563Info_s RTCHym8563Info = {0};									//���ڱ����RTC�ж�ȡ��ʱ��ֵ









/*�ڲ�������********************************************************/
/***
*�ж�I2C�����Ƿ񱻴ӻ�ռ�ã����߳�������Ӧ�������������������
***/
static void _I2C_ClearErr(void)
{
	extern I2C_HandleTypeDef hi2c3;
	
	if((hi2c3.Instance->SR2 & I2C_FLAG_BUSY) || (hi2c3.Instance->SR1 & I2C_FLAG_AF)){
		GPIO_InitTypeDef GPIO_InitStruct;
		uint16_t i,j;
		
		//�����Ƚ�I2C���������ó�ͨ�ÿ�©ģʽ�������ֶ�����SCL��״̬
		GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
		
		//����һ��STOP���������AF��Ӧ����󣩲����Ĵ���
		GPIOH->BSRR = GPIO_PIN_7<<16;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_8<<16;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_7;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_8;
		for(j=0;j<50000;j++);
		
		//ѭ����SCL������壬�ôӻ��ͷ�I2C���ߵĿ���Ȩ�����BUSY״̬
		for(i=0;i<9;i++){
			GPIOH->BSRR = GPIO_PIN_7<<16;
			for(j=0;j<50000;j++);
			GPIOH->BSRR = GPIO_PIN_7;
			for(j=0;j<50000;j++);
		}
		
		//������I2C������������I2C����ģʽ��I2Cģʽ�����ɽ�������쳣״̬��־
		hi2c3.Instance->CR1 |= I2C_CR1_SWRST;
		hi2c3.State = HAL_I2C_STATE_RESET;
		MX_I2C3_Init();
	}
}












/*�ⲿ������****************************************************************/
/***
*���õ�ǰʱ��
*������
	RTC_Hym8563Info��ʱ����ز����ṹ��
*����ֵ��Ϊ0��ʾ�ɹ�������ֵΪʧ��
***/
HAL_StatusTypeDef RTC_HYM8563SetTime(RTC_HYM8563Info_s *ptRTC_Hym8563Info)
{
	extern I2C_HandleTypeDef hi2c3;
	uint32_t Count = 0;	
	HAL_StatusTypeDef ReState = HAL_OK;
	
    //����
    osKernelLock();
    
	//�������ζ�ȡ֮����̫�̣����´ӻ���δ�ͷ�������ȫ���Ӷ�Ҫ����_I2C_ClearErr�������ͷ�
	for(Count=0;Count<10000;Count++){
		if((hi2c3.Instance->SR2 & I2C_FLAG_BUSY) == RESET){
			break;
		}
	}
	Count = 0;
	
	//�ж�I2C�����Ƿ���BUSY���ǣ�����ͨ���ֶ���ʽ����9��ʱ�������ôӻ��ͷ�����
	_I2C_ClearErr();
	
	//��������д����ᣬ��ʧ�����쳣
	ReState = HAL_I2C_Mem_Write(&hi2c3,SLAVE_ADR_HYM8563<<1,REG_ADR_SEC,1,(uint8_t*)ptRTC_Hym8563Info,sizeof(RTC_HYM8563Info_s),500);
	if(ReState != HAL_OK){
		//������I2C������������I2C����ģʽ��I2Cģʽ�����ɽ�������쳣״̬��־
		hi2c3.Instance->CR1 |= I2C_CR1_SWRST;
		hi2c3.State = HAL_I2C_STATE_RESET;
		MX_I2C3_Init();
		
		ReState = HAL_I2C_Mem_Write(&hi2c3,SLAVE_ADR_HYM8563<<1,REG_ADR_SEC,1,(uint8_t*)ptRTC_Hym8563Info,sizeof(RTC_HYM8563Info_s),500);
	}
	
    //����
    osKernelUnlock();
	return ReState;
}



/***
*��ȡ��ǰʱ��
*������
	RTC_Hym8563Info��ʱ����ز����ṹ��ָ��
*����ֵ��Ϊ0��ʾ�ɹ�������ֵΪʧ��
***/
HAL_StatusTypeDef RTC_HYM8563GetTime(RTC_HYM8563Info_s* ptRTC_Hym8563Info)
{
	extern I2C_HandleTypeDef hi2c3;
	uint32_t Count = 0;	
	HAL_StatusTypeDef ReturnStatus = HAL_OK;
    
    //����
    osKernelLock();
	
	//�������ζ�ȡ֮����̫�̣����´ӻ���δ�ͷ�������ȫ���Ӷ�Ҫ����_I2C_ClearErr�������ͷ�
	for(Count=0;Count<10000;Count++){
		if((hi2c3.Instance->SR2 & I2C_FLAG_BUSY) == RESET){
			break;
		}
	}
	Count = 0;
	
	//�ж�I2C�����Ƿ���BUSY���ǣ�����ͨ���ֶ���ʽ����9��ʱ�������ôӻ��ͷ�����
	_I2C_ClearErr();
	
	ReturnStatus = HAL_I2C_Mem_Read(&hi2c3,SLAVE_ADR_HYM8563<<1,REG_ADR_SEC,1,(uint8_t*)ptRTC_Hym8563Info,sizeof(RTC_HYM8563Info_s),500);
	
	if(ReturnStatus != HAL_OK){
		//������I2C������������I2C����ģʽ��I2Cģʽ�����ɽ�������쳣״̬��־
		hi2c3.Instance->CR1 |= I2C_CR1_SWRST;
		hi2c3.State = HAL_I2C_STATE_RESET;
		MX_I2C3_Init();
		
		ReturnStatus = HAL_I2C_Mem_Read(&hi2c3,SLAVE_ADR_HYM8563<<1,REG_ADR_SEC,1,(uint8_t*)ptRTC_Hym8563Info,sizeof(RTC_HYM8563Info_s),500);
	}
	
    //����
    osKernelUnlock();
	return ReturnStatus;
}




/*
*  ��ȡRTCʱ�䣬���ַ���ֵ��
*/
HAL_StatusTypeDef RTC_Read(uint8_t *pucaDateTime, uint8_t ucLen)
{
	extern MachInfo_s MachInfo;
    RTC_HYM8563Info_s tDateTime = {0};
	HAL_StatusTypeDef ReturnStatus = HAL_OK;
	uint16_t Month = 0;
	uint16_t Len = 0;
    
    if(ucLen < DATE_TIME_LEN)
    {
        LOG_Error("Error Param ucLen(%d)", ucLen);
        return HAL_ERROR;
    }
   
    //��ȡʱ��ṹ��
	ReturnStatus = RTC_HYM8563GetTime(&tDateTime);
    if(HAL_OK == ReturnStatus)
    {
//        if(tDateTime.sec.vl == 0) //��Ч
//        {
            if(tDateTime.month.c == 0) // 20**
            {
				if(MachInfo.systemSet.eLanguage == LANGUAGE_CHINESE){
					//����ʱ��ģʽ
					Len = sprintf((char*)pucaDateTime, "20%01d%01d/%01d%01d/%01d%01d %01d%0d:%01d%01d:%01d%01d",\
						tDateTime.year.bcd_h, tDateTime.year.bcd_l,\
						tDateTime.month.bcd_h, tDateTime.month.bcd_l,\
						tDateTime.day.bcd_h, tDateTime.day.bcd_l,\
						tDateTime.hour.bcd_h, tDateTime.hour.bcd_l,\
						tDateTime.min.bcd_h, tDateTime.min.bcd_l,\
						tDateTime.sec.bcd_h, tDateTime.sec.bcd_l);
					
					pucaDateTime[Len] = '\0';
				}else{
					//Ӣ��ʱ��ģʽ
					Len = 0;
					
					Month = tDateTime.month.bcd_h*10+tDateTime.month.bcd_l;
					
					switch(Month){
						case 1:Len = sprintf((char*)pucaDateTime+Len,"Jan/");break;
						case 2:Len = sprintf((char*)pucaDateTime+Len,"Feb/");break;
						case 3:Len = sprintf((char*)pucaDateTime+Len,"Mar/");break;
						case 4:Len = sprintf((char*)pucaDateTime+Len,"Apr/");break;
						case 5:Len = sprintf((char*)pucaDateTime+Len,"May/");break;
						case 6:Len = sprintf((char*)pucaDateTime+Len,"Jun/");break;
						case 7:Len = sprintf((char*)pucaDateTime+Len,"Jul/");break;
						case 8:Len = sprintf((char*)pucaDateTime+Len,"Aug/");break;
						case 9:Len = sprintf((char*)pucaDateTime+Len,"Sept/");break;
						case 10:Len = sprintf((char*)pucaDateTime+Len,"Oct/");break;
						case 11:Len = sprintf((char*)pucaDateTime+Len,"Nov/");break;
						case 12:Len = sprintf((char*)pucaDateTime+Len,"Dec/");break;
						default :break;
					}
					Len += sprintf((char*)pucaDateTime+Len, "%u%u/20%u%u %u%u:%u%u:%u%u",\
						tDateTime.day.bcd_h, tDateTime.day.bcd_l,\
						tDateTime.year.bcd_h, tDateTime.year.bcd_l,\
						tDateTime.hour.bcd_h, tDateTime.hour.bcd_l,\
						tDateTime.min.bcd_h, tDateTime.min.bcd_l,\
						tDateTime.sec.bcd_h, tDateTime.sec.bcd_l);
					
					pucaDateTime[Len] = '\0';
				}
                 
            }
    }else{
        LOG_Error("RTC Value is Error %u\r\n",ReturnStatus);
        return HAL_ERROR;
    }
    
    return HAL_OK;
}



/*
*  ����RTCʱ��   
*/
HAL_StatusTypeDef RTC_Write(uint8_t *pucaDateTime, uint8_t ucLen)  //pucaDateTime   2021/01/01 00:00:00
{
    uint8_t i = 2;
    RTC_HYM8563Info_s tDateTime = {0};
    

    //��
    tDateTime.year.bcd_h =  pucaDateTime[i++] - '0';
    tDateTime.year.bcd_l =  pucaDateTime[i++] - '0';
    //��
    i++;
    tDateTime.month.bcd_h =  pucaDateTime[i++] - '0';
    tDateTime.month.bcd_l =  pucaDateTime[i++] - '0';
    tDateTime.month.c     = 0;  //20xx��
    //��
    i++;
    tDateTime.day.bcd_h =  pucaDateTime[i++] - '0';
    tDateTime.day.bcd_l =  pucaDateTime[i++] - '0';
    
    //ȥ���ո�
    i++;
    
    //ʱ
    tDateTime.hour.bcd_h =  pucaDateTime[i++] - '0';
    tDateTime.hour.bcd_l =  pucaDateTime[i++] - '0';   
    //��
    i++;
    tDateTime.min.bcd_h =  pucaDateTime[i++] - '0';
    tDateTime.min.bcd_l =  pucaDateTime[i++] - '0';
    //��
    tDateTime.sec.bcd_h =  pucaDateTime[i++] - '0';
    tDateTime.sec.bcd_l =  pucaDateTime[i++] - '0';
    tDateTime.sec.vl    = 0;
    
    return RTC_HYM8563SetTime(&tDateTime);
}


/***
*��ȡ��ǰʱ���ֵ
***/
long GetCurrentTimestamp(RTC_HYM8563Info_s RTCHym8563Info)
{
	struct tm CurrentTime = {0};
	long CurrentTimestamp = 0;
	
	CurrentTime.tm_year = (RTCHym8563Info.year.bcd_h*10+RTCHym8563Info.year.bcd_l)+2000-1900;
	CurrentTime.tm_mon = RTCHym8563Info.month.bcd_h*10+RTCHym8563Info.month.bcd_l-1;
	CurrentTime.tm_mday = RTCHym8563Info.day.bcd_h*10+RTCHym8563Info.day.bcd_l;
	CurrentTime.tm_hour = RTCHym8563Info.hour.bcd_h*10+RTCHym8563Info.hour.bcd_l;
	CurrentTime.tm_min = RTCHym8563Info.min.bcd_h*10+RTCHym8563Info.min.bcd_l;
	CurrentTime.tm_sec = RTCHym8563Info.sec.bcd_h*10+RTCHym8563Info.sec.bcd_l;
	
	CurrentTimestamp = mktime(&CurrentTime);
	
	CurrentTimestamp -= 8*60*60;
	
	return CurrentTimestamp;
}








