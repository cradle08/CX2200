/**********************************************************************
*����ѹ������������
***********************************************************************/
#include "digPressure.h"
#include "i2c.h"
#include "msg_def.h"





/*�궨����*************************************************************/
//��������ѹ��������I2C�ӻ���ַ
#define PRESURE_I2C_SLAVEADR						0x52











/*������**************************************************************/
/***
*�ж�I2C�����Ƿ񱻴ӻ�ռ�ã����߳�������Ӧ�������������������
***/
static void _I2C_ClearErr(void)
{
	extern I2C_HandleTypeDef hi2c2;
	
	if((hi2c2.Instance->SR2 & I2C_FLAG_BUSY) || (hi2c2.Instance->SR1 & I2C_FLAG_AF)){
		GPIO_InitTypeDef GPIO_InitStruct;
		uint16_t i,j;
		
		//�����Ƚ�I2C���������ó�ͨ�ÿ�©ģʽ�������ֶ�����SCL��״̬
		GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
		
		//����һ��STOP���������AF��Ӧ����󣩲����Ĵ���
		GPIOH->BSRR = GPIO_PIN_4<<16;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_5<<16;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_4;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_5;
		for(j=0;j<50000;j++);
		
		//ѭ����SCL������壬�ôӻ��ͷ�I2C���ߵĿ���Ȩ�����BUSY״̬
		for(i=0;i<9;i++){
			GPIOH->BSRR = GPIO_PIN_4<<16;
			for(j=0;j<50000;j++);
			GPIOH->BSRR = GPIO_PIN_4;
			for(j=0;j<50000;j++);
		}
		
		//������I2C������������I2C����ģʽ��I2Cģʽ�����ɽ�������쳣״̬��־
		hi2c2.Instance->CR1 |= I2C_CR1_SWRST;
		hi2c2.State = HAL_I2C_STATE_RESET;
		MX_I2C2_Init();
	}
}






/***
*��ȡ����ѹ��������״̬��0x40��ʾѹ�����������ڿ���״̬��Ϊ0x60��ʾѹ������������
�ɼ�ѹ��״̬��Ϊ0x00��ʾ����������ѹ�����������ߴ����쳣������ֵ��ʾ�쳣
***/
uint8_t DigPressure_GetStatus(void)
{
	extern I2C_HandleTypeDef hi2c2;
	uint32_t Count = 0;	
	HAL_StatusTypeDef ReturnStatus = HAL_OK;
	uint8_t Status = 0;
	
	//�������ζ�ȡ֮����̫�̣����´ӻ���δ�ͷ�������ȫ���Ӷ�Ҫ����_I2C_ClearErr�������ͷ�
	for(Count=0;Count<10000;Count++){
		if((hi2c2.Instance->SR2 & I2C_FLAG_BUSY) == RESET){
			break;
		}
	}
	Count = 0;
	
	//�ж�I2C�����Ƿ���BUSY���ǣ�����ͨ���ֶ���ʽ����9��ʱ�������ôӻ��ͷ�����
	_I2C_ClearErr();
	
	ReturnStatus = HAL_I2C_Master_Receive(&hi2c2,PRESURE_I2C_SLAVEADR,&Status,1,500);
	if(ReturnStatus != HAL_OK){
		return 0;
	}
	return Status;
}




/***
*��������ѹ����������ȡѹ��ֵ
***/
HAL_StatusTypeDef DigPressureStart(DigPresureSampleMode_e SampleMode)
{
	extern I2C_HandleTypeDef hi2c2;
	uint8_t Cmd = 0;
	
	switch(SampleMode){
		case SINGLE:
        {
			Cmd = 0xaa;
			return HAL_I2C_Master_Transmit(&hi2c2,PRESURE_I2C_SLAVEADR,&Cmd,1,500);
        }
		case AVG2:	
        {
			Cmd = 0xac;
			return HAL_I2C_Master_Transmit(&hi2c2,PRESURE_I2C_SLAVEADR,&Cmd,1,500);
        }
		case AVG4:
        {
			Cmd = 0xad;
			return HAL_I2C_Master_Transmit(&hi2c2,PRESURE_I2C_SLAVEADR,&Cmd,1,500);
        }
		case AVG8:	
        {
			Cmd = 0xae;
			return HAL_I2C_Master_Transmit(&hi2c2,PRESURE_I2C_SLAVEADR,&Cmd,1,500);
        }
		case AVG16:
        {
			Cmd = 0xaf;
			return HAL_I2C_Master_Transmit(&hi2c2,PRESURE_I2C_SLAVEADR,&Cmd,1,500);
        }
		default :return HAL_ERROR;
	}
}



/***
*���ڻ�ȡ����ѹ����������ȡ����ԭʼѹ����ֵ
***/
HAL_StatusTypeDef DigPressure_GetRawValue(uint32_t* RawPressure)
{
	extern I2C_HandleTypeDef hi2c2;
	uint32_t Count = 0;	
	HAL_StatusTypeDef ReturnStatus = HAL_OK;
	uint8_t Buf[4] = {0};
	
	//�������ζ�ȡ֮����̫�̣����´ӻ���δ�ͷ�������ȫ���Ӷ�Ҫ����_I2C_ClearErr�������ͷ�
	for(Count=0;Count<10000;Count++){
		if((hi2c2.Instance->SR2 & I2C_FLAG_BUSY) == RESET){
			break;
		}
	}
	Count = 0;
	
	//�ж�I2C�����Ƿ���BUSY���ǣ�����ͨ���ֶ���ʽ����9��ʱ�������ôӻ��ͷ�����
	_I2C_ClearErr();
	
	ReturnStatus = HAL_I2C_Master_Receive(&hi2c2,PRESURE_I2C_SLAVEADR,Buf,4,500);
	if(ReturnStatus != HAL_OK){
		return ReturnStatus;
	}
	
	if(Buf[0] != 0x40){
		return HAL_BUSY;
	}
	
	*RawPressure = (((uint32_t)Buf[1])<<16)|(Buf[2]<<8)|Buf[3];
	return HAL_OK;
}



/***
*ת������ѹ����������ʵ��ѹ��ֵ
***/
int32_t DigPressure_ConvertValue(uint32_t RawPressure)
{
	extern  MachInfo_s MachInfo;
	
	int32_t Temp = 0;
	
	Temp = (RawPressure/16777216.0)*258550.0-129275.0;
	
	if(Temp > -10000){
		return Temp-MachInfo.paramSet.digSensor[0];
	}else if(Temp > -20000){
		return Temp-MachInfo.paramSet.digSensor[1];
	}else if(Temp > -30000){
		return Temp-MachInfo.paramSet.digSensor[2];
	}else if(Temp > -40000){
		return Temp-MachInfo.paramSet.digSensor[3];
	}else{
		return Temp-MachInfo.paramSet.digSensor[4];
	}
}





