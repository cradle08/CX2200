#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H
 

#include "stm32f4xx_hal.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO?úμ??·ó3é?
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO?ú2ù×÷,????μ￥ò?μ?IO?ú!
//è·±￡nμ??μD?óú16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //ê?3? 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //ê?è? 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //ê?3? 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //ê?è? 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //ê?3? 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //ê?è? 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //ê?3? 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //ê?è? 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //ê?3? 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //ê?è?

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //ê?3? 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //ê?è?

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //ê?3? 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //ê?è?

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //ê?3? 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //ê?è?

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //ê?3? 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //ê?è?



//IO方向设置
#define CT_SDA_IN()  {GPIOI->MODER&=~(3<<(3*2));GPIOI->MODER|=0<<3*2;}	//PI3输入模式
#define CT_SDA_OUT() {GPIOI->MODER&=~(3<<(3*2));GPIOI->MODER|=1<<3*2;} 	//PI3输出模式
//IO操作函数	 
#define CT_IIC_SCL  PHout(6)  //SCL
#define CT_IIC_SDA  PIout(3)  //SDA	 
#define CT_READ_SDA PIin(3)   //输入SDA 
 

//IIC所有操作函数
void CT_IIC_Init(void);                	//初始化IIC的IO口				 
void CT_IIC_Start(void);				//发送IIC开始信号
void CT_IIC_Stop(void);	  				//发送IIC停止信号
void CT_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t CT_IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
uint8_t CT_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void CT_IIC_Ack(void);					//IIC发送ACK信号
void CT_IIC_NAck(void);					//IIC不发送ACK信号

#endif







