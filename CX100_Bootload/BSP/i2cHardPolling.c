/***************************************************************
硬件I2C轮询模式，具有防止总线被从机锁死功能，防止通讯过程中受异常干扰后导致
总线通讯锁死的功能，具备自恢复功能，一般I2C使用硬件轮询模式即可，不需要使用DMA模式
****************************************************************/
#include "i2cHardPolling.h"




/*宏定义区***********************************************/
#define I2C_GPIO_PORT           	GPIOH                    	//定义硬件I2C使用的端口
#define I2C_PORT_CLOCK          	RCC_AHB1Periph_GPIOH    	//定义硬件I2C使用的端口时钟
#define I2C_SCL_PIN             	GPIO_Pin_7                	//定义硬件I2C使用的SCL引脚
#define I2C_SDA_PIN             	GPIO_Pin_8                	//定义硬件I2C使用的SDA引脚
#define I2C_SCL_SOURCE				GPIO_PinSource7				//定义硬件I2C使用的SCL引脚复用源
#define I2C_SDA_SOURCE				GPIO_PinSource8				//定义硬件I2C使用的SDA引脚复用源
	
#define I2C_ADDRESS             	0x10                    	//定义本机I2C的地址，注意，这里的地址是7位主机真实物理地址左移一位后的值
#define I2C_SPEED               	100000                    	//定义I2C传输的速度值，不要超过400k
	
#define MAX_TIMEOUT					500000						//定义循环等待事件标志时，最大的超时次数


#define I2C_SR2_BUSY_FLAG			0x0002						//定义BUSY对应的判断标志位(SR2)
#define I2C_SR1_BERR_AF_FLAG		0x0500						//定义BERR和AF对应的判断标志位(SR1)
#define I2C_SR1_TXE_FLAG			0x0080						//定义TXE对应的判断标志位(SR1)
#define I2C_SR2_TRA_BUSY_MSL_FLAG	0x0007						//定义TRA,BUSY,MSL对应的判断标志位(SR2)
#define I2C_CR1_ACK_FLAG			0x0400						//定义ACK位
#define I2C_CR1_POS_FLAG			0x0800						//定义POS位
#define I2C_CR1_STOP_FLAG			0x0200						//定义STOP位
#define I2C_SR2_BUSY_MSL_FLAG		0x0003						//定义BUSY,MSL对应的判断标志位(SR2)
#define I2C_SR1_RXNE_BTF_FLAG		0x0044						//定义RXNE,BTF位对应的判断标志位(SR1)
#define I2C_SR1_RXNE_FLAG			0x0040						//定义RXNE位对应的判断标志位(SR1)


/*内部函数区*********************************************/
/***
*内部函数，配置I2C引脚模式
***/
static void _I2C_Pin_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(I2C_PORT_CLOCK, ENABLE);
	
	GPIO_PinAFConfig(I2C_GPIO_PORT, I2C_SCL_SOURCE, GPIO_AF_I2C3);		//设置PB8为AF模式，给I2C3复用
	GPIO_PinAFConfig(I2C_GPIO_PORT, I2C_SDA_SOURCE, GPIO_AF_I2C3);   	//设置PB9为AF模式，给I2C3复用
    
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);
}


/***
*内部函数，配置I2C
***/
static void _I2C_Config(void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
    
    I2C_DeInit(I2C3);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                    	//设置使用I2C模式还是使用SMBus的主从模式，注意：这里不区分I2C是作为主机还是从机（自动切换主从）
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                            	//设置SCL的高低电平的占空比，有2:1，或者16:9，只有在快速模式下才有高低电平占空比的说法（即通讯速度大于100KHZ）
    I2C_InitStructure.I2C_OwnAddress1 = I2C_ADDRESS;                            	//设置I2C的本机地址(7位从机地址左移一位后的值)
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                  	//设置是否使能自动应答功能，一般都要使能
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;    	//设置使用7位从机地址还是10位从机地址
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;                                	//I2C的传输速度，一般这个值不要超过400kHZ
    I2C_Init(I2C3, &I2C_InitStructure);
    
    I2C_Cmd(I2C3, ENABLE);
}


/***
*判断I2C总线是否被从机占用，总线出错，总线应答错误等情况，若是则解除
***/
static void _I2C_ClearErr(void)
{
	if((I2C3->SR2 & I2C_SR2_BUSY_FLAG) || (I2C3->SR1 & I2C_SR1_BERR_AF_FLAG)){
		GPIO_InitTypeDef  GPIO_InitStructure;
		uint8_t i;
		uint16_t j;
		
		//必须先将I2C的引脚配置成通用开漏模式，才能手动控制SCL的状态
		GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);
		
		//产生一个STOP条件，清除AF（应答错误）产生的错误
		I2C_GPIO_PORT->BSRRH = I2C_SCL_PIN;
		for(j=0;j<50000;j++);
		I2C_GPIO_PORT->BSRRH = I2C_SDA_PIN;
		for(j=0;j<50000;j++);
		I2C_GPIO_PORT->BSRRL = I2C_SCL_PIN;
		for(j=0;j<50000;j++);
		I2C_GPIO_PORT->BSRRL = I2C_SDA_PIN;
		for(j=0;j<50000;j++);
		
		//循环让SCL输出脉冲，让从机释放I2C总线的控制权，清除BUSY状态
		for(i=0;i<9;i++){
			I2C_GPIO_PORT->BSRRH = I2C_SCL_PIN;
			for(j=0;j<50000;j++);
			I2C_GPIO_PORT->BSRRL = I2C_SCL_PIN;
			for(j=0;j<50000;j++);
		}
		
		//先重置I2C，再重新配置I2C引脚模式和I2C模式，即可解除总线异常状态标志
		I2C3->CR1 |= I2C_CR1_SWRST;
		_I2C_Pin_Config();
		_I2C_Config();
	}
}


/*外部函数区********************************************/
/***
*配置I2C和对应的引脚
***/
void I2C_Config(void)
{
    _I2C_Pin_Config();
    _I2C_Config();
	
	//I2C初始化后先判断下总线是否被占用，总线出错，总线应答错误等情况，若是则解除从机控制总线
	_I2C_ClearErr();
}


/***
*重置I2C
***/
void I2C_Reset(void)
{
	//复位I2C
	I2C3->CR1 |= I2C_CR1_SWRST;
	_I2C_Pin_Config();
	_I2C_Config();
}




/***
*往从机指定寄存器中写入数据
*参数:
    SlaveAdr：从机物理地址
    Reg：指定寄存器的地址
	RegLen：寄存器地址长度
    Buf：要写入的数据的起始地址
	Len：要写入数据的长度
*返回值：0为写入数据成功，非0为写入数据失败
***/
ErrorStatus I2CWriteData(uint8_t SlaveAdr,uint16_t Reg,uint8_t RegLen,uint8_t* Buf,uint16_t Len)
{
	uint32_t Count = 0;					//防止超时卡死现象的计数值
	
	//避免两次写入之间间隔太短，导致从机尚未完全释放总线，从而要进入_I2C_ClearErr函数来释放
	for(Count=0;Count<10000;Count++){
		if(!(I2C3->SR2 & I2C_SR2_BUSY_FLAG)){
			break;
		}
	}
	Count = 0;
	
	//判断总线是否异常，若是，则解除总线异常
	_I2C_ClearErr();
	
	//产生起始条件，注意当总线处于BUSY的时候，则主机不能获得控制权，从而导致程序卡死
    I2C_GenerateSTART(I2C3,ENABLE);
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_MODE_SELECT)){
		Count++;
		if(Count>=MAX_TIMEOUT){
			return ERROR;
		}
	}
    
	//注意：I2C_Send7bitAddress函数中对于SlaveAdr的处理方式，需要先将SlaveAdr左移1位再传入
    I2C_Send7bitAddress(I2C3,SlaveAdr<<1,I2C_Direction_Transmitter);
	Count = 0;
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){	//注意，在debug模式下，若开启了I2C的监视窗口，则会自动的读取SR1和SR2来更新显示，从而导致ADDR被自动清除了，而出现超时现象
		Count++;
		if(Count>=MAX_TIMEOUT){
			return ERROR;
		}
	}
    
	if(RegLen == 1){
		I2C_SendData(I2C3,(uint8_t)(Reg&0x00ff));
		Count = 0;
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//只要TXE为1时，则表明DR寄存器为空了（移位寄存器可能不为空），此时可以继续写入数据到DR中了
			Count++;
			if(Count>=MAX_TIMEOUT){
				return ERROR;
			}
		}
	}else{
		I2C_SendData(I2C3,(uint8_t)((Reg&0xff00)>>8));
		Count = 0;
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//只要TXE为1时，则表明DR寄存器为空了（移位寄存器可能不为空），此时可以继续写入数据到DR中了
			Count++;
			if(Count>=MAX_TIMEOUT){
				return ERROR;
			}
		}
		
		I2C_SendData(I2C3,(uint8_t)(Reg&0x00ff));
		Count = 0;
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//只要TXE为1时，则表明DR寄存器为空了（移位寄存器可能不为空），此时可以继续写入数据到DR中了
			Count++;
			if(Count>=MAX_TIMEOUT){
				return ERROR;
			}
		}
	}
    
    
    while(Len--){
        I2C_SendData(I2C3,*Buf);
        Buf++;
		Count = 0;
        while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){
			Count++;
			if(Count>=MAX_TIMEOUT){
				return ERROR;
			}
		}
    }
    
	//由于数据发送完会因为BTF标志位的置位从而拉低SCL，所以STOP位的置位可以放在这个位置上
    I2C_GenerateSTOP(I2C3,ENABLE);
	return SUCCESS;
}




/***
*往从机中的指定寄存器开始读出多个字节数据，注意，根据要读出的数据的长度1,2，多个字节有不同的处理方式
*参数:
    SlaveAdr：从机物理地址
    Reg：指定寄存器的地址
	RegLen：寄存器地址长度
    Buf:读出的数据存放缓存地址
	Len：要读出多少个字节数据长度
*返回值：0为写入数据成功，1为写入数据失败
***/
ErrorStatus I2CReadData(uint8_t SlaveAdr,uint16_t Reg,uint8_t RegLen,uint8_t* Buf,uint16_t Len)
{
	uint32_t Count = 0;					//防止超时卡死现象的计数值
	
	//避免两次读取之间间隔太短，导致从机尚未释放总线完全，从而要进入_I2C_ClearErr函数来释放
	for(Count=0;Count<10000;Count++){
		if(!(I2C3->SR2 & I2C_SR2_BUSY_FLAG)){
			break;
		}
	}
	Count = 0;
	
	//判断总线是否异常，若是，则解除总线异常
	_I2C_ClearErr();
	
	//将自动应答位重新设置，以便后续字节的读取后自动发送ACK
	I2C3->CR1 |= I2C_CR1_ACK_FLAG;			//ACK=1
	I2C3->CR1 &= ~I2C_CR1_POS_FLAG;			//POS=0
	
    I2C_GenerateSTART(I2C3,ENABLE);
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_MODE_SELECT)){
		Count++;
		if(Count>=MAX_TIMEOUT){
			return ERROR;
		}
	}
    
    I2C_Send7bitAddress(I2C3,SlaveAdr<<1,I2C_Direction_Transmitter);
	Count = 0;
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
		Count++;
		if(Count>=MAX_TIMEOUT){
			return ERROR;
		}
	}
    
    if(RegLen == 1){
		I2C_SendData(I2C3,(uint8_t)(Reg&0x00ff));
		Count = 0;
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//只要TXE为1时，则表明DR寄存器为空了（移位寄存器可能不为空），此时可以继续写入数据到DR中了
			Count++;
			if(Count>=MAX_TIMEOUT){
				return ERROR;
			}
		}
	}else{
		I2C_SendData(I2C3,(uint8_t)((Reg&0xff00)>>8));
		Count = 0;
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//只要TXE为1时，则表明DR寄存器为空了（移位寄存器可能不为空），此时可以继续写入数据到DR中了
			Count++;
			if(Count>=MAX_TIMEOUT){
				return ERROR;
			}
		}
		
		I2C_SendData(I2C3,(uint8_t)(Reg&0x00ff));
		Count = 0;
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//只要TXE为1时，则表明DR寄存器为空了（移位寄存器可能不为空），此时可以继续写入数据到DR中了
			Count++;
			if(Count>=MAX_TIMEOUT){
				return ERROR;
			}
		}
	}
    
    
    I2C_GenerateSTART(I2C3,ENABLE);
	Count = 0;
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_MODE_SELECT)){
		Count++;
		if(Count>=MAX_TIMEOUT){
			return ERROR;
		}
	}
    
    I2C_Send7bitAddress(I2C3,SlaveAdr<<1,I2C_Direction_Receiver);
	//根据读取的数据长度不同处理动作不同
	if(Len == 1){
		I2C3->CR1 &= ~I2C_CR1_ACK_FLAG;			//ACK=0
	}else if(Len == 2){
		I2C3->CR1 &= ~I2C_CR1_ACK_FLAG;			//ACK=0
		I2C3->CR1 |= I2C_CR1_POS_FLAG;			//POS=1
	}
	Count = 0;
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){
		Count++;
		if(Count>=MAX_TIMEOUT){
			return ERROR;
		}
	}
	
	//根据读取的数据长度不同处理动作不同
	if(Len == 1){								//读取一个字节
		I2C3->CR1 |= I2C_CR1_STOP_FLAG;			//STOP=1;当只读一个字节的时候，由于无法利用BTF来钳位SCL的低电平，所以必须在清除ADDR之后立即将STOP位置位，才能正确的结束当前的传输,这里有可能出现STOP信号发送不及时的情况
		
		Count = 0;
		while(!I2C_CheckEvent(I2C3, I2C_EVENT_MASTER_BYTE_RECEIVED)){
			Count++;
			if(Count>=MAX_TIMEOUT){
				//这里有可能是因为NACK正常发出后，stop信号未及时发出，导致ALRO和BUSY位被置位，所以超时的时候退出循环，然后再读取DR中的数据即可
				break;
			}
			if(I2C3->SR1==0x00 && I2C3->SR2==0x00){
				//这里有可能是因为NACK和STOP信号都正常发出，但是没有及时读取I2C_EVENT_MASTER_BYTE_RECEIVED这个事件（stop信号发出后，会自动将SR1和SR2的位都清除），所以这里可以直接退出循环，然后读取DR即可
				break;
			}
		}
		
		*Buf = I2C_ReceiveData(I2C3);
	}else if(Len == 2){							//读取2个字节
		Count = 0;
		while(((I2C3->SR2 & I2C_SR2_BUSY_MSL_FLAG) != I2C_SR2_BUSY_MSL_FLAG) || ((I2C3->SR1 & I2C_SR1_RXNE_BTF_FLAG) != I2C_SR1_RXNE_BTF_FLAG)){		//BTF=1
			Count++;
			if(Count>=MAX_TIMEOUT){
				//这里有可能是因为NACK正常发出后，stop信号未及时发出，导致ALRO和BUSY位被置位，所以超时的时候退出循环，然后再读取DR中的数据即可
				break;
			}
		}
		
		//STOP=1;
		I2C3->CR1 |= I2C_CR1_STOP_FLAG;
		
		//读取DR寄存器和移位寄存器中的数据
		*Buf++ = I2C_ReceiveData(I2C3);
		*Buf++ = I2C_ReceiveData(I2C3);
	}else{										//读取多个字节
		while(Len){
			if(Len == 3){
				Count = 0;
				while(((I2C3->SR2 & I2C_SR2_BUSY_MSL_FLAG) != I2C_SR2_BUSY_MSL_FLAG) || ((I2C3->SR1 & I2C_SR1_RXNE_BTF_FLAG) != I2C_SR1_RXNE_BTF_FLAG)){		//BTF=1
					Count++;
					if(Count>=MAX_TIMEOUT){
						return ERROR;
					}
				}
				
				I2C3->CR1 &= ~I2C_CR1_ACK_FLAG;			//ACK=0
				*Buf++ = I2C_ReceiveData(I2C3);
			}else if(Len < 3){
				Count = 0;
				while(((I2C3->SR2 & I2C_SR2_BUSY_MSL_FLAG) != I2C_SR2_BUSY_MSL_FLAG) || ((I2C3->SR1 & I2C_SR1_RXNE_BTF_FLAG) != I2C_SR1_RXNE_BTF_FLAG)){		//BTF=1
					Count++;
					if(Count>=MAX_TIMEOUT){
						return ERROR;
					}
				}
				
				I2C3->CR1 |= I2C_CR1_STOP_FLAG;			//STOP=1;
				
				*Buf++ = I2C_ReceiveData(I2C3);
				*Buf++ = I2C_ReceiveData(I2C3);
				Len = 0;
				break;									//这里需要跳出while，防止后面被Len--
			}else{
				Count = 0;
				while((I2C3->SR1 & I2C_SR1_RXNE_FLAG) != I2C_SR1_RXNE_FLAG){	//这里只要RXNE=1时就开始读取数据，这样可以形成连续的数据流
					Count++;
					if(Count>=MAX_TIMEOUT){
						return ERROR;
					}
				}
				
				*Buf++ = I2C_ReceiveData(I2C3);
			}
			
			Len--;
		}
	}
	
	return SUCCESS;
}



/***
*专门给EEPROM用的，测试是否已经将数据写入完毕的函数
*参数：
	SlaveAdr：EEPROM物理地址
*返回值：SUCCESS表示EEPROM有响应，ERROR表示无响应
***/
ErrorStatus RequestEEPROMAck(uint8_t SlaveAdr)
{
	uint32_t Count = 0;					//防止超时卡死现象的计数值
	
	//避免两次写入之间间隔太短，导致从机尚未完全释放总线，从而要进入_I2C_ClearErr函数来释放
	for(Count=0;Count<10000;Count++){
		if(!(I2C3->SR2 & I2C_SR2_BUSY_FLAG)){
			break;
		}
	}
	Count = 0;
	
	//判断总线是否异常，若是，则解除总线异常
	_I2C_ClearErr();
	
	//产生起始条件，当起始信号没有响应，START位没有被硬件清除时，需要复位下I2C
    I2C_GenerateSTART(I2C3,ENABLE);
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_MODE_SELECT)){
		Count++;
		if(Count>=10000){
			//复位I2C
			I2C_Reset();
			return ERROR;
		}
	}
    
	//发送从机地址，等待响应，若超时没有响应，则表明EEPROM还没有写入完毕，需要生成停止信号，并清除AF标志位
    I2C_Send7bitAddress(I2C3,SlaveAdr<<1,I2C_Direction_Transmitter);
	Count = 0;
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){	//注意，在debug模式下，若开启了I2C的监视窗口，则会自动的读取SR1和SR2来更新显示，从而导致ADDR被自动清除了，而出现超时现象
		Count++;
		if(Count>=1000){
			I2C3->CR1 |= I2C_CR1_STOP;
			I2C_ClearFlag(I2C3,I2C_FLAG_AF);						//这里需要清除掉AF标志
			return ERROR;
		}
	}
	
	I2C3->CR1 |= I2C_CR1_STOP;
	
	//这里需要清除ADDR标志位
	I2C3->SR1;
	I2C3->SR2;
	
	return SUCCESS;
}



