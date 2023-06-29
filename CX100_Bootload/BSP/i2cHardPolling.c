/***************************************************************
Ӳ��I2C��ѯģʽ�����з�ֹ���߱��ӻ��������ܣ���ֹͨѶ���������쳣���ź���
����ͨѶ�����Ĺ��ܣ��߱��Իָ����ܣ�һ��I2Cʹ��Ӳ����ѯģʽ���ɣ�����Ҫʹ��DMAģʽ
****************************************************************/
#include "i2cHardPolling.h"




/*�궨����***********************************************/
#define I2C_GPIO_PORT           	GPIOH                    	//����Ӳ��I2Cʹ�õĶ˿�
#define I2C_PORT_CLOCK          	RCC_AHB1Periph_GPIOH    	//����Ӳ��I2Cʹ�õĶ˿�ʱ��
#define I2C_SCL_PIN             	GPIO_Pin_7                	//����Ӳ��I2Cʹ�õ�SCL����
#define I2C_SDA_PIN             	GPIO_Pin_8                	//����Ӳ��I2Cʹ�õ�SDA����
#define I2C_SCL_SOURCE				GPIO_PinSource7				//����Ӳ��I2Cʹ�õ�SCL���Ÿ���Դ
#define I2C_SDA_SOURCE				GPIO_PinSource8				//����Ӳ��I2Cʹ�õ�SDA���Ÿ���Դ
	
#define I2C_ADDRESS             	0x10                    	//���屾��I2C�ĵ�ַ��ע�⣬����ĵ�ַ��7λ������ʵ�����ַ����һλ���ֵ
#define I2C_SPEED               	100000                    	//����I2C������ٶ�ֵ����Ҫ����400k
	
#define MAX_TIMEOUT					500000						//����ѭ���ȴ��¼���־ʱ�����ĳ�ʱ����


#define I2C_SR2_BUSY_FLAG			0x0002						//����BUSY��Ӧ���жϱ�־λ(SR2)
#define I2C_SR1_BERR_AF_FLAG		0x0500						//����BERR��AF��Ӧ���жϱ�־λ(SR1)
#define I2C_SR1_TXE_FLAG			0x0080						//����TXE��Ӧ���жϱ�־λ(SR1)
#define I2C_SR2_TRA_BUSY_MSL_FLAG	0x0007						//����TRA,BUSY,MSL��Ӧ���жϱ�־λ(SR2)
#define I2C_CR1_ACK_FLAG			0x0400						//����ACKλ
#define I2C_CR1_POS_FLAG			0x0800						//����POSλ
#define I2C_CR1_STOP_FLAG			0x0200						//����STOPλ
#define I2C_SR2_BUSY_MSL_FLAG		0x0003						//����BUSY,MSL��Ӧ���жϱ�־λ(SR2)
#define I2C_SR1_RXNE_BTF_FLAG		0x0044						//����RXNE,BTFλ��Ӧ���жϱ�־λ(SR1)
#define I2C_SR1_RXNE_FLAG			0x0040						//����RXNEλ��Ӧ���жϱ�־λ(SR1)


/*�ڲ�������*********************************************/
/***
*�ڲ�����������I2C����ģʽ
***/
static void _I2C_Pin_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(I2C_PORT_CLOCK, ENABLE);
	
	GPIO_PinAFConfig(I2C_GPIO_PORT, I2C_SCL_SOURCE, GPIO_AF_I2C3);		//����PB8ΪAFģʽ����I2C3����
	GPIO_PinAFConfig(I2C_GPIO_PORT, I2C_SDA_SOURCE, GPIO_AF_I2C3);   	//����PB9ΪAFģʽ����I2C3����
    
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);
}


/***
*�ڲ�����������I2C
***/
static void _I2C_Config(void)
{
    I2C_InitTypeDef  I2C_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
    
    I2C_DeInit(I2C3);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                    	//����ʹ��I2Cģʽ����ʹ��SMBus������ģʽ��ע�⣺���ﲻ����I2C����Ϊ�������Ǵӻ����Զ��л����ӣ�
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                            	//����SCL�ĸߵ͵�ƽ��ռ�ձȣ���2:1������16:9��ֻ���ڿ���ģʽ�²��иߵ͵�ƽռ�ձȵ�˵������ͨѶ�ٶȴ���100KHZ��
    I2C_InitStructure.I2C_OwnAddress1 = I2C_ADDRESS;                            	//����I2C�ı�����ַ(7λ�ӻ���ַ����һλ���ֵ)
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                  	//�����Ƿ�ʹ���Զ�Ӧ���ܣ�һ�㶼Ҫʹ��
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;    	//����ʹ��7λ�ӻ���ַ����10λ�ӻ���ַ
    I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;                                	//I2C�Ĵ����ٶȣ�һ�����ֵ��Ҫ����400kHZ
    I2C_Init(I2C3, &I2C_InitStructure);
    
    I2C_Cmd(I2C3, ENABLE);
}


/***
*�ж�I2C�����Ƿ񱻴ӻ�ռ�ã����߳�������Ӧ�������������������
***/
static void _I2C_ClearErr(void)
{
	if((I2C3->SR2 & I2C_SR2_BUSY_FLAG) || (I2C3->SR1 & I2C_SR1_BERR_AF_FLAG)){
		GPIO_InitTypeDef  GPIO_InitStructure;
		uint8_t i;
		uint16_t j;
		
		//�����Ƚ�I2C���������ó�ͨ�ÿ�©ģʽ�������ֶ�����SCL��״̬
		GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);
		
		//����һ��STOP���������AF��Ӧ����󣩲����Ĵ���
		I2C_GPIO_PORT->BSRRH = I2C_SCL_PIN;
		for(j=0;j<50000;j++);
		I2C_GPIO_PORT->BSRRH = I2C_SDA_PIN;
		for(j=0;j<50000;j++);
		I2C_GPIO_PORT->BSRRL = I2C_SCL_PIN;
		for(j=0;j<50000;j++);
		I2C_GPIO_PORT->BSRRL = I2C_SDA_PIN;
		for(j=0;j<50000;j++);
		
		//ѭ����SCL������壬�ôӻ��ͷ�I2C���ߵĿ���Ȩ�����BUSY״̬
		for(i=0;i<9;i++){
			I2C_GPIO_PORT->BSRRH = I2C_SCL_PIN;
			for(j=0;j<50000;j++);
			I2C_GPIO_PORT->BSRRL = I2C_SCL_PIN;
			for(j=0;j<50000;j++);
		}
		
		//������I2C������������I2C����ģʽ��I2Cģʽ�����ɽ�������쳣״̬��־
		I2C3->CR1 |= I2C_CR1_SWRST;
		_I2C_Pin_Config();
		_I2C_Config();
	}
}


/*�ⲿ������********************************************/
/***
*����I2C�Ͷ�Ӧ������
***/
void I2C_Config(void)
{
    _I2C_Pin_Config();
    _I2C_Config();
	
	//I2C��ʼ�������ж��������Ƿ�ռ�ã����߳�������Ӧ��������������������ӻ���������
	_I2C_ClearErr();
}


/***
*����I2C
***/
void I2C_Reset(void)
{
	//��λI2C
	I2C3->CR1 |= I2C_CR1_SWRST;
	_I2C_Pin_Config();
	_I2C_Config();
}




/***
*���ӻ�ָ���Ĵ�����д������
*����:
    SlaveAdr���ӻ������ַ
    Reg��ָ���Ĵ����ĵ�ַ
	RegLen���Ĵ�����ַ����
    Buf��Ҫд������ݵ���ʼ��ַ
	Len��Ҫд�����ݵĳ���
*����ֵ��0Ϊд�����ݳɹ�����0Ϊд������ʧ��
***/
ErrorStatus I2CWriteData(uint8_t SlaveAdr,uint16_t Reg,uint8_t RegLen,uint8_t* Buf,uint16_t Len)
{
	uint32_t Count = 0;					//��ֹ��ʱ��������ļ���ֵ
	
	//��������д��֮����̫�̣����´ӻ���δ��ȫ�ͷ����ߣ��Ӷ�Ҫ����_I2C_ClearErr�������ͷ�
	for(Count=0;Count<10000;Count++){
		if(!(I2C3->SR2 & I2C_SR2_BUSY_FLAG)){
			break;
		}
	}
	Count = 0;
	
	//�ж������Ƿ��쳣�����ǣ����������쳣
	_I2C_ClearErr();
	
	//������ʼ������ע�⵱���ߴ���BUSY��ʱ�����������ܻ�ÿ���Ȩ���Ӷ����³�����
    I2C_GenerateSTART(I2C3,ENABLE);
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_MODE_SELECT)){
		Count++;
		if(Count>=MAX_TIMEOUT){
			return ERROR;
		}
	}
    
	//ע�⣺I2C_Send7bitAddress�����ж���SlaveAdr�Ĵ���ʽ����Ҫ�Ƚ�SlaveAdr����1λ�ٴ���
    I2C_Send7bitAddress(I2C3,SlaveAdr<<1,I2C_Direction_Transmitter);
	Count = 0;
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){	//ע�⣬��debugģʽ�£���������I2C�ļ��Ӵ��ڣ�����Զ��Ķ�ȡSR1��SR2��������ʾ���Ӷ�����ADDR���Զ�����ˣ������ֳ�ʱ����
		Count++;
		if(Count>=MAX_TIMEOUT){
			return ERROR;
		}
	}
    
	if(RegLen == 1){
		I2C_SendData(I2C3,(uint8_t)(Reg&0x00ff));
		Count = 0;
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//ֻҪTXEΪ1ʱ�������DR�Ĵ���Ϊ���ˣ���λ�Ĵ������ܲ�Ϊ�գ�����ʱ���Լ���д�����ݵ�DR����
			Count++;
			if(Count>=MAX_TIMEOUT){
				return ERROR;
			}
		}
	}else{
		I2C_SendData(I2C3,(uint8_t)((Reg&0xff00)>>8));
		Count = 0;
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//ֻҪTXEΪ1ʱ�������DR�Ĵ���Ϊ���ˣ���λ�Ĵ������ܲ�Ϊ�գ�����ʱ���Լ���д�����ݵ�DR����
			Count++;
			if(Count>=MAX_TIMEOUT){
				return ERROR;
			}
		}
		
		I2C_SendData(I2C3,(uint8_t)(Reg&0x00ff));
		Count = 0;
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//ֻҪTXEΪ1ʱ�������DR�Ĵ���Ϊ���ˣ���λ�Ĵ������ܲ�Ϊ�գ�����ʱ���Լ���д�����ݵ�DR����
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
    
	//�������ݷ��������ΪBTF��־λ����λ�Ӷ�����SCL������STOPλ����λ���Է������λ����
    I2C_GenerateSTOP(I2C3,ENABLE);
	return SUCCESS;
}




/***
*���ӻ��е�ָ���Ĵ�����ʼ��������ֽ����ݣ�ע�⣬����Ҫ���������ݵĳ���1,2������ֽ��в�ͬ�Ĵ���ʽ
*����:
    SlaveAdr���ӻ������ַ
    Reg��ָ���Ĵ����ĵ�ַ
	RegLen���Ĵ�����ַ����
    Buf:���������ݴ�Ż����ַ
	Len��Ҫ�������ٸ��ֽ����ݳ���
*����ֵ��0Ϊд�����ݳɹ���1Ϊд������ʧ��
***/
ErrorStatus I2CReadData(uint8_t SlaveAdr,uint16_t Reg,uint8_t RegLen,uint8_t* Buf,uint16_t Len)
{
	uint32_t Count = 0;					//��ֹ��ʱ��������ļ���ֵ
	
	//�������ζ�ȡ֮����̫�̣����´ӻ���δ�ͷ�������ȫ���Ӷ�Ҫ����_I2C_ClearErr�������ͷ�
	for(Count=0;Count<10000;Count++){
		if(!(I2C3->SR2 & I2C_SR2_BUSY_FLAG)){
			break;
		}
	}
	Count = 0;
	
	//�ж������Ƿ��쳣�����ǣ����������쳣
	_I2C_ClearErr();
	
	//���Զ�Ӧ��λ�������ã��Ա�����ֽڵĶ�ȡ���Զ�����ACK
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
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//ֻҪTXEΪ1ʱ�������DR�Ĵ���Ϊ���ˣ���λ�Ĵ������ܲ�Ϊ�գ�����ʱ���Լ���д�����ݵ�DR����
			Count++;
			if(Count>=MAX_TIMEOUT){
				return ERROR;
			}
		}
	}else{
		I2C_SendData(I2C3,(uint8_t)((Reg&0xff00)>>8));
		Count = 0;
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//ֻҪTXEΪ1ʱ�������DR�Ĵ���Ϊ���ˣ���λ�Ĵ������ܲ�Ϊ�գ�����ʱ���Լ���д�����ݵ�DR����
			Count++;
			if(Count>=MAX_TIMEOUT){
				return ERROR;
			}
		}
		
		I2C_SendData(I2C3,(uint8_t)(Reg&0x00ff));
		Count = 0;
		while(((I2C3->SR1 & I2C_SR1_TXE_FLAG) != I2C_SR1_TXE_FLAG) || ((I2C3->SR2 & I2C_SR2_TRA_BUSY_MSL_FLAG) != I2C_SR2_TRA_BUSY_MSL_FLAG)){	//ֻҪTXEΪ1ʱ�������DR�Ĵ���Ϊ���ˣ���λ�Ĵ������ܲ�Ϊ�գ�����ʱ���Լ���д�����ݵ�DR����
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
	//���ݶ�ȡ�����ݳ��Ȳ�ͬ��������ͬ
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
	
	//���ݶ�ȡ�����ݳ��Ȳ�ͬ��������ͬ
	if(Len == 1){								//��ȡһ���ֽ�
		I2C3->CR1 |= I2C_CR1_STOP_FLAG;			//STOP=1;��ֻ��һ���ֽڵ�ʱ�������޷�����BTF��ǯλSCL�ĵ͵�ƽ�����Ա��������ADDR֮��������STOPλ��λ��������ȷ�Ľ�����ǰ�Ĵ���,�����п��ܳ���STOP�źŷ��Ͳ���ʱ�����
		
		Count = 0;
		while(!I2C_CheckEvent(I2C3, I2C_EVENT_MASTER_BYTE_RECEIVED)){
			Count++;
			if(Count>=MAX_TIMEOUT){
				//�����п�������ΪNACK����������stop�ź�δ��ʱ����������ALRO��BUSYλ����λ�����Գ�ʱ��ʱ���˳�ѭ����Ȼ���ٶ�ȡDR�е����ݼ���
				break;
			}
			if(I2C3->SR1==0x00 && I2C3->SR2==0x00){
				//�����п�������ΪNACK��STOP�źŶ���������������û�м�ʱ��ȡI2C_EVENT_MASTER_BYTE_RECEIVED����¼���stop�źŷ����󣬻��Զ���SR1��SR2��λ��������������������ֱ���˳�ѭ����Ȼ���ȡDR����
				break;
			}
		}
		
		*Buf = I2C_ReceiveData(I2C3);
	}else if(Len == 2){							//��ȡ2���ֽ�
		Count = 0;
		while(((I2C3->SR2 & I2C_SR2_BUSY_MSL_FLAG) != I2C_SR2_BUSY_MSL_FLAG) || ((I2C3->SR1 & I2C_SR1_RXNE_BTF_FLAG) != I2C_SR1_RXNE_BTF_FLAG)){		//BTF=1
			Count++;
			if(Count>=MAX_TIMEOUT){
				//�����п�������ΪNACK����������stop�ź�δ��ʱ����������ALRO��BUSYλ����λ�����Գ�ʱ��ʱ���˳�ѭ����Ȼ���ٶ�ȡDR�е����ݼ���
				break;
			}
		}
		
		//STOP=1;
		I2C3->CR1 |= I2C_CR1_STOP_FLAG;
		
		//��ȡDR�Ĵ�������λ�Ĵ����е�����
		*Buf++ = I2C_ReceiveData(I2C3);
		*Buf++ = I2C_ReceiveData(I2C3);
	}else{										//��ȡ����ֽ�
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
				break;									//������Ҫ����while����ֹ���汻Len--
			}else{
				Count = 0;
				while((I2C3->SR1 & I2C_SR1_RXNE_FLAG) != I2C_SR1_RXNE_FLAG){	//����ֻҪRXNE=1ʱ�Ϳ�ʼ��ȡ���ݣ����������γ�������������
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
*ר�Ÿ�EEPROM�õģ������Ƿ��Ѿ�������д����ϵĺ���
*������
	SlaveAdr��EEPROM�����ַ
*����ֵ��SUCCESS��ʾEEPROM����Ӧ��ERROR��ʾ����Ӧ
***/
ErrorStatus RequestEEPROMAck(uint8_t SlaveAdr)
{
	uint32_t Count = 0;					//��ֹ��ʱ��������ļ���ֵ
	
	//��������д��֮����̫�̣����´ӻ���δ��ȫ�ͷ����ߣ��Ӷ�Ҫ����_I2C_ClearErr�������ͷ�
	for(Count=0;Count<10000;Count++){
		if(!(I2C3->SR2 & I2C_SR2_BUSY_FLAG)){
			break;
		}
	}
	Count = 0;
	
	//�ж������Ƿ��쳣�����ǣ����������쳣
	_I2C_ClearErr();
	
	//������ʼ����������ʼ�ź�û����Ӧ��STARTλû�б�Ӳ�����ʱ����Ҫ��λ��I2C
    I2C_GenerateSTART(I2C3,ENABLE);
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_MODE_SELECT)){
		Count++;
		if(Count>=10000){
			//��λI2C
			I2C_Reset();
			return ERROR;
		}
	}
    
	//���ʹӻ���ַ���ȴ���Ӧ������ʱû����Ӧ�������EEPROM��û��д����ϣ���Ҫ����ֹͣ�źţ������AF��־λ
    I2C_Send7bitAddress(I2C3,SlaveAdr<<1,I2C_Direction_Transmitter);
	Count = 0;
    while(!I2C_CheckEvent(I2C3,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){	//ע�⣬��debugģʽ�£���������I2C�ļ��Ӵ��ڣ�����Զ��Ķ�ȡSR1��SR2��������ʾ���Ӷ�����ADDR���Զ�����ˣ������ֳ�ʱ����
		Count++;
		if(Count>=1000){
			I2C3->CR1 |= I2C_CR1_STOP;
			I2C_ClearFlag(I2C3,I2C_FLAG_AF);						//������Ҫ�����AF��־
			return ERROR;
		}
	}
	
	I2C3->CR1 |= I2C_CR1_STOP;
	
	//������Ҫ���ADDR��־λ
	I2C3->SR1;
	I2C3->SR2;
	
	return SUCCESS;
}



