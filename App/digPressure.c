/**********************************************************************
*数字压力传感器驱动
***********************************************************************/
#include "digPressure.h"
#include "i2c.h"
#include "msg_def.h"





/*宏定义区*************************************************************/
//定义数字压力传感器I2C从机地址
#define PRESURE_I2C_SLAVEADR						0x52











/*函数区**************************************************************/
/***
*判断I2C总线是否被从机占用，总线出错，总线应答错误等情况，若是则解除
***/
static void _I2C_ClearErr(void)
{
	extern I2C_HandleTypeDef hi2c2;
	
	if((hi2c2.Instance->SR2 & I2C_FLAG_BUSY) || (hi2c2.Instance->SR1 & I2C_FLAG_AF)){
		GPIO_InitTypeDef GPIO_InitStruct;
		uint16_t i,j;
		
		//必须先将I2C的引脚配置成通用开漏模式，才能手动控制SCL的状态
		GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
		
		//产生一个STOP条件，清除AF（应答错误）产生的错误
		GPIOH->BSRR = GPIO_PIN_4<<16;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_5<<16;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_4;
		for(j=0;j<50000;j++);
		GPIOH->BSRR = GPIO_PIN_5;
		for(j=0;j<50000;j++);
		
		//循环让SCL输出脉冲，让从机释放I2C总线的控制权，清除BUSY状态
		for(i=0;i<9;i++){
			GPIOH->BSRR = GPIO_PIN_4<<16;
			for(j=0;j<50000;j++);
			GPIOH->BSRR = GPIO_PIN_4;
			for(j=0;j<50000;j++);
		}
		
		//先重置I2C，再重新配置I2C引脚模式和I2C模式，即可解除总线异常状态标志
		hi2c2.Instance->CR1 |= I2C_CR1_SWRST;
		hi2c2.State = HAL_I2C_STATE_RESET;
		MX_I2C2_Init();
	}
}






/***
*获取数字压力传感器状态，0x40表示压力传感器处于空闲状态，为0x60表示压力传感器处于
采集压力状态，为0x00表示不存在数字压力传感器或者处于异常，其他值表示异常
***/
uint8_t DigPressure_GetStatus(void)
{
	extern I2C_HandleTypeDef hi2c2;
	uint32_t Count = 0;	
	HAL_StatusTypeDef ReturnStatus = HAL_OK;
	uint8_t Status = 0;
	
	//避免两次读取之间间隔太短，导致从机尚未释放总线完全，从而要进入_I2C_ClearErr函数来释放
	for(Count=0;Count<10000;Count++){
		if((hi2c2.Instance->SR2 & I2C_FLAG_BUSY) == RESET){
			break;
		}
	}
	Count = 0;
	
	//判断I2C总线是否处于BUSY，是，则先通过手动方式产生9个时钟周期让从机释放总线
	_I2C_ClearErr();
	
	ReturnStatus = HAL_I2C_Master_Receive(&hi2c2,PRESURE_I2C_SLAVEADR,&Status,1,500);
	if(ReturnStatus != HAL_OK){
		return 0;
	}
	return Status;
}




/***
*启动数字压力传感器读取压力值
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
*用于获取数字压力传感器读取到的原始压力数值
***/
HAL_StatusTypeDef DigPressure_GetRawValue(uint32_t* RawPressure)
{
	extern I2C_HandleTypeDef hi2c2;
	uint32_t Count = 0;	
	HAL_StatusTypeDef ReturnStatus = HAL_OK;
	uint8_t Buf[4] = {0};
	
	//避免两次读取之间间隔太短，导致从机尚未释放总线完全，从而要进入_I2C_ClearErr函数来释放
	for(Count=0;Count<10000;Count++){
		if((hi2c2.Instance->SR2 & I2C_FLAG_BUSY) == RESET){
			break;
		}
	}
	Count = 0;
	
	//判断I2C总线是否处于BUSY，是，则先通过手动方式产生9个时钟周期让从机释放总线
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
*转换数字压力传感器的实际压力值
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





