/**********************************************************************
ϵͳ�δ�ʱ��,����һ��ʱ�����ӳٻ��߼�ʱ��


ע�����
1.systickһ����CM4�е�ʱ����168M��24λ���¼�����


��������ʼ�����룺
	RCC_ClocksTypeDef RCC_Clocks = {0};
	
	Systick_InitConfig();
    USART_Printf_Config();
	printf("ready\r\n");
	
	RCC_GetClocksFreq(&RCC_Clocks);
	
	printf("HCLK:%d\r\n",RCC_Clocks.HCLK_Frequency);
	printf("PCLK1:%d\r\n",RCC_Clocks.PCLK1_Frequency);
	printf("PCLK2:%d\r\n",RCC_Clocks.PCLK2_Frequency);
	printf("SYSCLK:%d\r\n",RCC_Clocks.SYSCLK_Frequency);
	
	while(1){
		printf("a\r\n");
		Delay_MS(1000);
	}
***********************************************************************/
#include "systick.h"



/*�궨�����;�̬ȫ�ֶ�����**********************************************************/
#define MAXVALUE				0xffffffff			//StampCount���������ֵ��Ϊ������Ч�ʣ����%65536ȡģ���㣬ʹ��&��65536-1�����㷨,ע�⣺MAXVALUE������2^x-1��ֵ
#define TIMEBASE				10					//����systick�ļ�ʱ��׼ʱ�䣬������10ms����һ�θ����ж�
static u32 StampCount = 0;							//���ڼ�¼systick�����жϵĴ������൱��һ��ʱ�����Ч��





/*�ⲿ������************************************************************************/
/***
*��ʼ��systick��ʱ��
***/
void Systick_InitConfig(void)
{
	//��������ֵ��ȡֵ��Χ��0~2^24
	SysTick->LOAD  = (uint32_t)(SystemCoreClock/1000*TIMEBASE - 1UL);

	//��ռ������е�ֵ
	SysTick->VAL = 0UL;

	//����systick��ʱ��Դ�Ϳ���systick���ж�
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk|SysTick_CTRL_TICKINT_Msk;
		
	//��SYSTICK
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}



/***
*�ӳٺ���,������Ҫ�ȽϾ�ȷ�ļ�ʱ�����Ծ������ó�����ȡģ����
*������
	Time:Ҫ�ӳٵ�ʱ�䣬��λ��ms��ע�����ڼ�ʱ��׼��10ms������Time�����10�ı��������������������������
***/
void Delay_MS(uint32_t Time)
{
	uint32_t Count;
	uint32_t FirstStamp;
	uint32_t LastStamp;
	FlagStatus Flag;									//�����жϽ���ʱ���Ƿ����StampCount���ֵ�����ں�С�ڵ��жϷ�ʽ�ǲ�һ���ģ�������Ҫ���������
	
	if(Time<=0)
		return;

	FirstStamp = StampCount;
	Count = (Time+(TIMEBASE>>1))/TIMEBASE;				//��������������㷨��a/b+0.5��������ǿ��ת�����εķ�ʽ�õ���
	
	if(FirstStamp+Count<MAXVALUE){
		LastStamp = FirstStamp + Count;
		Flag = SET;
	}else{
		LastStamp = FirstStamp + Count - MAXVALUE;
		Flag = RESET;
	}
	
	while(1){
		if(Flag){
			if(StampCount>=LastStamp || StampCount<FirstStamp)
				break;
		}else{
			if(StampCount>=LastStamp && StampCount<FirstStamp)
				break;
		}
	}
}



/***
*��ʱ����
*������
	FirstStamp����һ�δ����ú�����ʱ���
	Time��Ҫ��ʱ��ʱ��������λ��ms
*����ֵ������ʱʱ�䵽������SET�����򷵻�RESET
***/
FlagStatus Time_MS(uint32_t FirstStamp,uint32_t Time)
{
	uint32_t Count;
	uint32_t LastStamp;			//����Ҫ��ʱ��ʱ��
	FlagStatus Flag;			//�����жϽ���ʱ���Ƿ����StampCount���ֵ�����ں�С�ڵ��жϷ�ʽ�ǲ�һ���ģ�������Ҫ���������
	
	if(Time<=0)
		return SET;

	Count = (Time+(TIMEBASE>>1))/TIMEBASE;				//��������
	
	//�ж�FirstStamp+Count�Ƿ񳬹��˷�Χ��������Ӧ�Ĵ���
	if(FirstStamp+Count<MAXVALUE){
		LastStamp = FirstStamp + Count;
		Flag = SET;
	}else{
		LastStamp = FirstStamp + Count - MAXVALUE;
		Flag = RESET;
	}
	
	//��ʼ�ж��Ƿ�ʱ��
	if(Flag){
		if(StampCount>=LastStamp || StampCount<FirstStamp)
			return SET;
	}else{
		if(StampCount>=LastStamp && StampCount<FirstStamp)
			return SET;
	}
	
	return RESET;
}



/***
*���ʱ�������
*����ֵ����ǰ��ʱ�����ֵ
***/
uint32_t Get_Stamp(void)
{
	return StampCount;
}



//===========================systick�жϴ�����===========================
void SysTick_Handler(void)
{
	StampCount++;
}

















