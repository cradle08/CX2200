/**********************************************************************
系统滴答定时器,用于一定时基的延迟或者计时。


注意事项：
1.systick一般在CM4中的时钟是168M，24位向下计数器


主函数初始化代码：
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



/*宏定义区和静态全局定义区**********************************************************/
#define MAXVALUE				0xffffffff			//StampCount变量的最大值，为了增加效率，替代%65536取模运算，使用&（65536-1）的算法,注意：MAXVALUE必须是2^x-1的值
#define TIMEBASE				10					//决定systick的计时基准时间，这里是10ms产生一次更新中断
static u32 StampCount = 0;							//用于记录systick触发中断的次数，相当于一个时间戳的效果





/*外部函数区************************************************************************/
/***
*初始化systick定时器
***/
void Systick_InitConfig(void)
{
	//设置重载值，取值范围：0~2^24
	SysTick->LOAD  = (uint32_t)(SystemCoreClock/1000*TIMEBASE - 1UL);

	//清空计数器中的值
	SysTick->VAL = 0UL;

	//设置systick的时钟源和开启systick的中断
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk|SysTick_CTRL_TICKINT_Msk;
		
	//打开SYSTICK
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}



/***
*延迟函数,由于需要比较精确的计时，所以尽量少用除法，取模运算
*参数：
	Time:要延迟的时间，单位是ms，注意由于计时基准是10ms，所以Time最好是10的倍数，否则会出现四舍五入的现象
***/
void Delay_MS(uint32_t Time)
{
	uint32_t Count;
	uint32_t FirstStamp;
	uint32_t LastStamp;
	FlagStatus Flag;									//用来判断结束时刻是否大于StampCount最大值，大于和小于的判断方式是不一样的，所以需要分情况讨论
	
	if(Time<=0)
		return;

	FirstStamp = StampCount;
	Count = (Time+(TIMEBASE>>1))/TIMEBASE;				//四舍五入的巧妙算法（a/b+0.5，再利用强制转成整形的方式得到）
	
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
*定时函数
*参数：
	FirstStamp：第一次触发该函数的时间戳
	Time：要定时的时间间隔，单位是ms
*返回值：当定时时间到，返回SET，否则返回RESET
***/
FlagStatus Time_MS(uint32_t FirstStamp,uint32_t Time)
{
	uint32_t Count;
	uint32_t LastStamp;			//到达要定时的时刻
	FlagStatus Flag;			//用来判断结束时刻是否大于StampCount最大值，大于和小于的判断方式是不一样的，所以需要分情况讨论
	
	if(Time<=0)
		return SET;

	Count = (Time+(TIMEBASE>>1))/TIMEBASE;				//四舍五入
	
	//判断FirstStamp+Count是否超过了范围，并做相应的处理
	if(FirstStamp+Count<MAXVALUE){
		LastStamp = FirstStamp + Count;
		Flag = SET;
	}else{
		LastStamp = FirstStamp + Count - MAXVALUE;
		Flag = RESET;
	}
	
	//开始判断是否定时到
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
*获得时间戳函数
*返回值：当前的时间戳的值
***/
uint32_t Get_Stamp(void)
{
	return StampCount;
}



//===========================systick中断处理函数===========================
void SysTick_Handler(void)
{
	StampCount++;
}

















