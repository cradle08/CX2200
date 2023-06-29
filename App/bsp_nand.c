#include "bsp_nand.h"
#include "main.h"
#include "file_operate.h"
#include "bsp_outin.h"






/* ����ȴ���������������쳣, �˴�Ӧ���жϳ�ʱ*/
#define WAIT_BUSY()	{	\
	uint16_t k; 	\
	for (k = 0; k < 200; k++)	\
	{	\
		if ((NAND_READY_BUSY_Port->IDR & NAND_READY_BUSY_Pin) == 0) break;	\
	}	\
	for (k = 0; k < 2000; k++)	\
	{	\
		if ((NAND_READY_BUSY_Port->IDR & NAND_READY_BUSY_Pin) != 0) break;	\
	}	\
}

//1=busy, 0=not busy, or error
int32_t Driver_NAND0_GetDeviceBusy (uint32_t dev_num)
{
	WAIT_BUSY();
	
	return 0;
}



/**************************************************************/
#if USE_RL_FLASH_FS

//NAND��ʱ
//һ��i++������Ҫ4ns
void NAND_Delay(__IO uint32_t i)
{
	while(i>0)i--;
}


//��NAND״̬
//����ֵ:NAND״ֵ̬
//bit0:0,�ɹ�;1,����(���/����/READ)
//bit6:0,Busy;1,Ready
uint8_t NAND_ReadStatus(void)
{
    __IO uint8_t data=0; 
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_READSTA;//���Ͷ�״̬����
    NAND_Delay(NAND_TWHR_DELAY);	//�ȴ�tWHR,�ٶ�ȡ״̬�Ĵ���
 	data=*(__IO uint8_t*)NAND_ADDRESS;			//��ȡ״ֵ̬
    return data;
}




//�ȴ�NAND׼����
//����ֵ:NSTA_TIMEOUT �ȴ���ʱ��
//      NSTA_READY    �Ѿ�׼����
uint8_t NAND_WaitForReady(void)
{
    uint8_t status=0;
    __IO uint32_t time=0; 
	while(1)						//�ȴ�ready
	{
		status=NAND_ReadStatus();	//��ȡ״ֵ̬
		if(status&NSTA_READY)break;
		time++;
		if(time>=0X1FFFFFFF)return NSTA_TIMEOUT;//��ʱ
	}  
    return NSTA_READY;//׼����
}  




//��λNAND
//����ֵ:0,�ɹ�;
//    ����,ʧ��
uint8_t NAND_Reset(void)
{ 
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_RESET;	//��λNAND
    if(NAND_WaitForReady()==NSTA_READY)return 0;//��λ�ɹ�
    else return 1;								//��λʧ��
} 

//����һ����
//BlockNum:Ҫ������BLOCK���,��Χ:0-(block_totalnum-1)
//����ֵ:0,�����ɹ�
//    ����,����ʧ��
uint8_t NAND_EraseBlock(uint32_t BlockNum)
{
    BlockNum<<=7;
	//if(nand_dev.id==MT29F16G08ABABA)BlockNum<<=7;  	//�����ַת��Ϊҳ��ַ
    //else if(nand_dev.id==MT29F4G08ABADA)BlockNum<<=6;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_ERASE0;
    //���Ϳ��ַ
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)BlockNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(BlockNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(BlockNum>>16);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_ERASE1;
	HAL_Delay(NAND_TBERS_DELAY);		//�ȴ������ɹ�
	if(NAND_WaitForReady()!=NSTA_READY)return NSTA_ERROR;//ʧ��
    return 0;	//�ɹ�   
} 

//ȫƬ����NAND FLASH
void NAND_EraseChip(void)
{
    uint8_t status;
    uint16_t i=0;
    for(i=0;i<NAND_BLOCK_NUM;i++) //ѭ���������еĿ�
    {
        status=NAND_EraseBlock(i);
        if(status)LOG_Info("Erase %d block fail!!��������Ϊ%d",i,status);//����ʧ��
    }
}


#else // USE_RL_FLASH_FS



NAND_HandleTypeDef NAND_Handler;    //NAND FLASH���
nand_attriute nand_dev;             //nand��Ҫ�����ṹ��


void FMC_NAND_Coustom_Define(void)
{
    NAND_Reset();       		        //��λNAND
    HAL_Delay(100);
    //nand_dev.id=NAND_ReadID();	        //��ȡID
	NAND_ModeSet(4);

}



//��ʼ��NAND FLASH
uint8_t NAND_Init(void)
{
//    FMC_NAND_PCC_TimingTypeDef ComSpaceTiming,AttSpaceTiming;
//                                              
//    NAND_Handler.Instance=FMC_NAND_DEVICE;
//    NAND_Handler.Init.NandBank=FMC_NAND_BANK3;                          //NAND����BANK3��
//    NAND_Handler.Init.Waitfeature=FMC_NAND_PCC_WAIT_FEATURE_DISABLE;    //�رյȴ�����
//    NAND_Handler.Init.MemoryDataWidth=FMC_NAND_PCC_MEM_BUS_WIDTH_8;     //8λ���ݿ��
//    NAND_Handler.Init.EccComputation=FMC_NAND_ECC_DISABLE;              //��ʹ��ECC
//    NAND_Handler.Init.ECCPageSize=FMC_NAND_ECC_PAGE_SIZE_2048BYTE;      //ECCҳ��СΪ2k
//    NAND_Handler.Init.TCLRSetupTime=0;                                  //����TCLR(tCLR=CLE��RE����ʱ)=(TCLR+TSET+2)*THCLK,THCLK=1/180M=5.5ns
//    NAND_Handler.Init.TARSetupTime=1;                                   //����TAR(tAR=ALE��RE����ʱ)=(TAR+TSET+2)*THCLK,THCLK=1/180M=5.5n��   
//   
//    ComSpaceTiming.SetupTime=2;         //����ʱ��
//    ComSpaceTiming.WaitSetupTime=3;     //�ȴ�ʱ��
//    ComSpaceTiming.HoldSetupTime=2;     //����ʱ��
//    ComSpaceTiming.HiZSetupTime=1;      //����̬ʱ��
//    
//    AttSpaceTiming.SetupTime=2;         //����ʱ��
//    AttSpaceTiming.WaitSetupTime=3;     //�ȴ�ʱ��
//    AttSpaceTiming.HoldSetupTime=2;     //����ʱ��
//    AttSpaceTiming.HiZSetupTime=1;      //����̬ʱ��
//    
//    HAL_NAND_Init(&NAND_Handler,&ComSpaceTiming,&AttSpaceTiming); 
    NAND_Reset();       		        //��λNAND
    HAL_Delay(100);
    nand_dev.id=NAND_ReadID();	        //��ȡID
    LOG_Info("Nand ID: %X", nand_dev.id);
	NAND_ModeSet(4);			        //����ΪMODE4,����ģʽ 
    if(nand_dev.id==MT29F16G08ABABA)    //NANDΪMT29F16G08ABABA
    {
        nand_dev.page_totalsize=4320;  	//nandһ��page���ܴ�С������spare����     
        nand_dev.page_mainsize=4096;   	//nandһ��page����Ч��������С    
        nand_dev.page_sparesize=224;	//nandһ��page��spare����С
        nand_dev.block_pagenum=128;		//nandһ��block��������page��Ŀ
        nand_dev.plane_blocknum=2048;	//nandһ��plane��������block��Ŀ
        nand_dev.block_totalnum=4096;  	//nand����block��Ŀ  
    }
    else if(nand_dev.id==MT29F4G08ABADA)//NANDΪMT29F4G08ABADA
    {
        nand_dev.page_totalsize=2112;	//nandһ��page���ܴ�С������spare����
        nand_dev.page_mainsize=2048; 	//nandһ��page����Ч��������С
        nand_dev.page_sparesize=64;		//nandһ��page��spare����С
        nand_dev.block_pagenum=64;		//nandһ��block��������page��Ŀ
        nand_dev.plane_blocknum=2048;	//nandһ��plane��������block��Ŀ
        nand_dev.block_totalnum=4096; 	//nand����block��Ŀ
    }else {
        LOG_Info("Error nand_dev info init");
        while(1);
        return 1;
    }	//���󣬷���
    return 0;
}

////NAND FALSH�ײ�����,�������ã�ʱ��ʹ��
////�˺����ᱻHAL_NAND_Init()����
//void HAL_NAND_MspInit(NAND_HandleTypeDef *hnand)
//{
//    GPIO_InitTypeDef GPIO_Initure;
//    
//    __HAL_RCC_FMC_CLK_ENABLE();             //ʹ��FMCʱ��
//    __HAL_RCC_GPIOD_CLK_ENABLE();           //ʹ��GPIODʱ��
//    __HAL_RCC_GPIOE_CLK_ENABLE();           //ʹ��GPIOEʱ��
//    __HAL_RCC_GPIOG_CLK_ENABLE();           //ʹ��GPIOGʱ��
//    
//	//��ʼ��PD6 R/B����
//	GPIO_Initure.Pin=GPIO_PIN_6;
//    GPIO_Initure.Mode=GPIO_MODE_INPUT;          //����
//    GPIO_Initure.Pull=GPIO_PULLUP;    			//����          
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //����
//    HAL_GPIO_Init(GPIOD,&GPIO_Initure);
//	   
//	//��ʼ��PG9 NCE3����
//    GPIO_Initure.Pin=GPIO_PIN_9;
//    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //����
//    GPIO_Initure.Pull=GPIO_NOPULL;    			//����          
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //����
//	GPIO_Initure.Alternate=GPIO_AF12_FMC;       //����ΪFMC
//    HAL_GPIO_Init(GPIOG,&GPIO_Initure);  
//	
//    //��ʼ��PD0,1,4,5,11,12,14,15
//    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|\
//                     GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_15;
//    GPIO_Initure.Pull=GPIO_NOPULL;              
//    HAL_GPIO_Init(GPIOD,&GPIO_Initure);

//    //��ʼ��PE7,8,9,10
//    GPIO_Initure.Pin=GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
//    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
//}

//��ȡNAND FLASH��ID
//����ֵ:0,�ɹ�;
//    ����,ʧ��
uint8_t NAND_ModeSet(uint8_t mode)
{   
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_FEATURE;//����������������
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=0X01;		//��ַΪ0X01,����mode
 	*(__IO uint8_t*)NAND_ADDRESS=mode;					//P1����,����mode
	*(__IO uint8_t*)NAND_ADDRESS=0;
	*(__IO uint8_t*)NAND_ADDRESS=0;
	*(__IO uint8_t*)NAND_ADDRESS=0; 
    if(NAND_WaitForReady()==NSTA_READY)return 0;//�ɹ�
    else return 1;								//ʧ��
}

//��ȡNAND FLASH��ID
//��ͬ��NAND���в�ͬ��������Լ���ʹ�õ�NAND FALSH�����ֲ�����д����
//����ֵ:NAND FLASH��IDֵ
uint32_t NAND_ReadID(void)
{
    uint8_t deviceid[5]; 
    uint32_t id;  
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_READID; //���Ͷ�ȡID����
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=0X00;
	//IDһ����5���ֽ�
    deviceid[0]=*(__IO uint8_t*)NAND_ADDRESS;      
    deviceid[1]=*(__IO uint8_t*)NAND_ADDRESS;  
    deviceid[2]=*(__IO uint8_t*)NAND_ADDRESS; 
    deviceid[3]=*(__IO uint8_t*)NAND_ADDRESS; 
    deviceid[4]=*(__IO uint8_t*)NAND_ADDRESS;  
    //þ���NAND FLASH��IDһ��5���ֽڣ�����Ϊ�˷�������ֻȡ4���ֽ����һ��32λ��IDֵ
    //����NAND FLASH�������ֲᣬֻҪ��þ���NAND FLASH����ôһ���ֽ�ID�ĵ�һ���ֽڶ���0X2C
    //�������ǾͿ����������0X2C��ֻȡ�������ֽڵ�IDֵ��
    id=((uint32_t)deviceid[1])<<24|((uint32_t)deviceid[2])<<16|((uint32_t)deviceid[3])<<8|deviceid[4];
    return id;
}  
//��NAND״̬
//����ֵ:NAND״ֵ̬
//bit0:0,�ɹ�;1,����(���/����/READ)
//bit6:0,Busy;1,Ready
uint8_t NAND_ReadStatus(void)
{
    __IO uint8_t data=0; 
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_READSTA;//���Ͷ�״̬����
    NAND_Delay(NAND_TWHR_DELAY);	//�ȴ�tWHR,�ٶ�ȡ״̬�Ĵ���
 	data=*(__IO uint8_t*)NAND_ADDRESS;			//��ȡ״ֵ̬
    return data;
}
//�ȴ�NAND׼����
//����ֵ:NSTA_TIMEOUT �ȴ���ʱ��
//      NSTA_READY    �Ѿ�׼����
uint8_t NAND_WaitForReady(void)
{
    uint8_t status=0;
    __IO uint32_t time=0; 
	while(1)						//�ȴ�ready
	{
		status=NAND_ReadStatus();	//��ȡ״ֵ̬
		if(status&NSTA_READY)break;
		time++;
		if(time>=0X1FFFFFFF)return NSTA_TIMEOUT;//��ʱ
	}  
    return NSTA_READY;//׼����
}  
//��λNAND
//����ֵ:0,�ɹ�;
//    ����,ʧ��
uint8_t NAND_Reset(void)
{ 
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_RESET;	//��λNAND
    if(NAND_WaitForReady()==NSTA_READY)return 0;//��λ�ɹ�
    else return 1;								//��λʧ��
} 
//�ȴ�RB�ź�Ϊĳ����ƽ
//rb:0,�ȴ�RB==0
//   1,�ȴ�RB==1
//����ֵ:0,�ɹ�
//       1,��ʱ
uint8_t NAND_WaitRB(__IO uint8_t rb)
{
    __IO uint32_t time=0;  
	while(time<0X1FFFFFF)
	{
		time++;
		if(NAND_RB==rb)return 0;
	}
	return 1;
}
//NAND��ʱ
//һ��i++������Ҫ4ns
void NAND_Delay(__IO uint32_t i)
{
	while(i>0)i--;
}
//��ȡNAND Flash��ָ��ҳָ���е�����(main����spare��������ʹ�ô˺���)
//PageNum:Ҫ��ȡ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫ��ȡ���п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
//*pBuffer:ָ�����ݴ洢��
//NumByteToRead:��ȡ�ֽ���(���ܿ�ҳ��)
//����ֵ:0,�ɹ� 
//    ����,�������
uint8_t NAND_ReadPage(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToRead)
{
    __IO uint16_t i=0;
	uint8_t res=0;
	uint8_t eccnum=0;		//��Ҫ�����ECC������ÿNAND_ECC_SECTOR_SIZE�ֽڼ���һ��ecc
	uint8_t eccstart=0;		//��һ��ECCֵ�����ĵ�ַ��Χ
	uint8_t errsta=0;
	uint8_t *p;
     *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_AREA_A;
    //���͵�ַ
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)ColNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(ColNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)PageNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(PageNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(PageNum>>16);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_AREA_TRUE1;
    //�������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
    //��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
    //�����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
    //��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
    //���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������
	res=NAND_WaitRB(0);			//�ȴ�RB=0 
    if(res)return NSTA_TIMEOUT;	//��ʱ�˳�
    //����2�д����������ж�NAND�Ƿ�׼���õ�
	res=NAND_WaitRB(1);			//�ȴ�RB=1 
    if(res)return NSTA_TIMEOUT;	//��ʱ�˳�
	if(NumByteToRead%NAND_ECC_SECTOR_SIZE)//����NAND_ECC_SECTOR_SIZE����������������ECCУ��
	{ 
		//��ȡNAND FLASH�е�ֵ
		for(i=0;i<NumByteToRead;i++)
		{
			*(__IO uint8_t*)pBuffer++ = *(__IO uint8_t*)NAND_ADDRESS;
		}
	}else
	{
		eccnum=NumByteToRead/NAND_ECC_SECTOR_SIZE;			//�õ�ecc�������
		eccstart=ColNum/NAND_ECC_SECTOR_SIZE;
		p=pBuffer;
		for(res=0;res<eccnum;res++)
		{
			FMC_Bank2_3->PCR3|=1<<6;						//ʹ��ECCУ�� 
			for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)				//��ȡNAND_ECC_SECTOR_SIZE������
			{
				*(__IO uint8_t*)pBuffer++ = *(__IO uint8_t*)NAND_ADDRESS;
			}		
			while(!(FMC_Bank2_3->SR3&(1<<6)));				//�ȴ�FIFO��	
			nand_dev.ecc_hdbuf[res+eccstart]=FMC_Bank2_3->ECCR3;//��ȡӲ��������ECCֵ
			FMC_Bank2_3->PCR3&=~(1<<6);						//��ֹECCУ��
		} 
		i=nand_dev.page_mainsize+0X10+eccstart*4;			//��spare����0X10λ�ÿ�ʼ��ȡ֮ǰ�洢��eccֵ
		NAND_Delay(NAND_TRHW_DELAY);//�ȴ�tRHW 
		*(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=0X05;				//�����ָ��
		//���͵�ַ
		*(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)i;
		*(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(i>>8);
		*(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=0XE0;				//��ʼ������
		NAND_Delay(NAND_TWHR_DELAY);//�ȴ�tWHR 
		pBuffer=(uint8_t*)&nand_dev.ecc_rdbuf[eccstart];
		for(i=0;i<4*eccnum;i++)								//��ȡ�����ECCֵ
		{
			*(__IO uint8_t*)pBuffer++= *(__IO uint8_t*)NAND_ADDRESS;
		}			
		for(i=0;i<eccnum;i++)								//����ECC
		{
			if(nand_dev.ecc_rdbuf[i+eccstart]!=nand_dev.ecc_hdbuf[i+eccstart])//�����,��ҪУ��
			{
				LOG_Info("err hd,rd:0x%x,0x%x",nand_dev.ecc_hdbuf[i+eccstart],nand_dev.ecc_rdbuf[i+eccstart]); 
 				LOG_Info("eccnum,eccstart:%d,%d",eccnum,eccstart);	
				LOG_Info("PageNum,ColNum:%d,%d",PageNum,ColNum);	
				res=NAND_ECC_Correction(p+NAND_ECC_SECTOR_SIZE*i,nand_dev.ecc_rdbuf[i+eccstart],nand_dev.ecc_hdbuf[i+eccstart]);//ECCУ��
				if(res)errsta=NSTA_ECC2BITERR;				//���2BIT������ECC����
				else errsta=NSTA_ECC1BITERR;				//���1BIT ECC����
			} 
		} 		
	}
    if(NAND_WaitForReady()!=NSTA_READY)errsta=NSTA_ERROR;	//ʧ��
    return errsta;	//�ɹ�   
} 
//��ȡNAND Flash��ָ��ҳָ���е�����(main����spare��������ʹ�ô˺���),���Ա�(FTL����ʱ��Ҫ)
//PageNum:Ҫ��ȡ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫ��ȡ���п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
//CmpVal:Ҫ�Աȵ�ֵ,��uint32_tΪ��λ
//NumByteToRead:��ȡ����(��4�ֽ�Ϊ��λ,���ܿ�ҳ��)
//NumByteEqual:�ӳ�ʼλ�ó�����CmpValֵ��ͬ�����ݸ���
//����ֵ:0,�ɹ�
//    ����,�������
uint8_t NAND_ReadPageComp(uint32_t PageNum,uint16_t ColNum,uint32_t CmpVal,uint16_t NumByteToRead,uint16_t *NumByteEqual)
{
    uint16_t i=0;
	uint8_t res=0;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_AREA_A;
    //���͵�ַ
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)ColNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(ColNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)PageNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(PageNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(PageNum>>16);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_AREA_TRUE1;
    //�������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
    //��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
    //�����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
    //��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
    //���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������
	res=NAND_WaitRB(0);			//�ȴ�RB=0 
	if(res)return NSTA_TIMEOUT;	//��ʱ�˳�
    //����2�д����������ж�NAND�Ƿ�׼���õ�
	res=NAND_WaitRB(1);			//�ȴ�RB=1 
    if(res)return NSTA_TIMEOUT;	//��ʱ�˳�  
    for(i=0;i<NumByteToRead;i++)//��ȡ����,ÿ�ζ�4�ֽ�
    {
		if(*(__IO uint32_t*)NAND_ADDRESS!=CmpVal)break;	//������κ�һ��ֵ,��CmpVal�����,���˳�.
    }
	*NumByteEqual=i;					//��CmpValֵ��ͬ�ĸ���
    if(NAND_WaitForReady()!=NSTA_READY)return NSTA_ERROR;//ʧ��
    return 0;	//�ɹ�   
} 
//��NANDһҳ��д��ָ�����ֽڵ�����(main����spare��������ʹ�ô˺���)
//PageNum:Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫд����п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
//pBbuffer:ָ�����ݴ洢��
//NumByteToWrite:Ҫд����ֽ�������ֵ���ܳ�����ҳʣ���ֽ���������
//����ֵ:0,�ɹ� 
//    ����,�������
uint8_t NAND_WritePage(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToWrite)
{
    __IO uint16_t i=0;  
	uint8_t res=0;
	uint8_t eccnum=0;		//��Ҫ�����ECC������ÿNAND_ECC_SECTOR_SIZE�ֽڼ���һ��ecc
	uint8_t eccstart=0;		//��һ��ECCֵ�����ĵ�ַ��Χ
	
	*(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_WRITE0;
    //���͵�ַ
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)ColNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(ColNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)PageNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(PageNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(PageNum>>16);
	NAND_Delay(NAND_TADL_DELAY);//�ȴ�tADL 
	if(NumByteToWrite%NAND_ECC_SECTOR_SIZE)//����NAND_ECC_SECTOR_SIZE����������������ECCУ��
	{  
		for(i=0;i<NumByteToWrite;i++)		//д������
		{
			*(__IO uint8_t*)NAND_ADDRESS=*(__IO uint8_t*)pBuffer++;
		}
	}else
	{
		eccnum=NumByteToWrite/NAND_ECC_SECTOR_SIZE;			//�õ�ecc�������
		eccstart=ColNum/NAND_ECC_SECTOR_SIZE; 
 		for(res=0;res<eccnum;res++)
		{
			FMC_Bank2_3->PCR3|=1<<6;						//ʹ��ECCУ�� 
			for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)				//д��NAND_ECC_SECTOR_SIZE������
			{
				*(__IO uint8_t*)NAND_ADDRESS=*(__IO uint8_t*)pBuffer++;
			}		
			while(!(FMC_Bank2_3->SR3&(1<<6)));				//�ȴ�FIFO��	
			nand_dev.ecc_hdbuf[res+eccstart]=FMC_Bank2_3->ECCR3;	//��ȡӲ��������ECCֵ
  			FMC_Bank2_3->PCR3&=~(1<<6);						//��ֹECCУ��
		}  
		i=nand_dev.page_mainsize+0X10+eccstart*4;			//����д��ECC��spare����ַ
		NAND_Delay(NAND_TADL_DELAY);//�ȴ�tADL 
		*(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=0X85;				//���дָ��
		//���͵�ַ
		*(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)i;
		*(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(i>>8);
		NAND_Delay(NAND_TADL_DELAY);//�ȴ�tADL 
		pBuffer=(uint8_t*)&nand_dev.ecc_hdbuf[eccstart];
		for(i=0;i<eccnum;i++)					//д��ECC
		{ 
			for(res=0;res<4;res++)				 
			{
				*(__IO uint8_t*)NAND_ADDRESS=*(__IO uint8_t*)pBuffer++;
			}
		} 		
	}
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_WRITE_TURE1; 
 	NAND_Delay(NAND_TPROG_DELAY); 
    LOG_Info("hdbuf:%d, %d, %d, %d", nand_dev.ecc_hdbuf[0], nand_dev.ecc_hdbuf[1], nand_dev.ecc_hdbuf[2], nand_dev.ecc_hdbuf[3]);
	if(NAND_WaitForReady()!=NSTA_READY)return NSTA_ERROR;//ʧ��
    return 0;//�ɹ�   
}
//��NANDһҳ�е�ָ����ַ��ʼ,д��ָ�����ȵĺ㶨����
//PageNum:Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫд����п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
//cval:Ҫд���ָ������
//NumByteToWrite:Ҫд�������(��4�ֽ�Ϊ��λ)
//����ֵ:0,�ɹ� 
//    ����,�������
uint8_t NAND_WritePageConst(uint32_t PageNum,uint16_t ColNum,uint32_t cval,uint16_t NumByteToWrite)
{
    uint16_t i=0;  
	*(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_WRITE0;
    //���͵�ַ
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)ColNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(ColNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)PageNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(PageNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(PageNum>>16);
		NAND_Delay(NAND_TADL_DELAY);//�ȴ�tADL 
	for(i=0;i<NumByteToWrite;i++)		//д������,ÿ��д4�ֽ�
	{
		*(__IO uint32_t*)NAND_ADDRESS=cval;
	} 
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_WRITE_TURE1; 
 	Delay_US(NAND_TPROG_DELAY);	//�ȴ�tPROG
    if(NAND_WaitForReady()!=NSTA_READY)return NSTA_ERROR;//ʧ��
    return 0;//�ɹ�   
}
//��һҳ���ݿ�������һҳ,��д��������
//ע��:Դҳ��Ŀ��ҳҪ��ͬһ��Plane�ڣ�
//Source_PageNo:Դҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//Dest_PageNo:Ŀ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)  
//����ֵ:0,�ɹ�
//    ����,�������
uint8_t NAND_CopyPageWithoutWrite(uint32_t Source_PageNum,uint32_t Dest_PageNum)
{
	uint8_t res=0;
    uint16_t source_block=0,dest_block=0;  
    //�ж�Դҳ��Ŀ��ҳ�Ƿ���ͬһ��plane��
    source_block=Source_PageNum/nand_dev.block_pagenum;
    dest_block=Dest_PageNum/nand_dev.block_pagenum;
    if((source_block%2)!=(dest_block%2))return NSTA_ERROR;	//����ͬһ��plane�� 
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_MOVEDATA_CMD0;	//��������0X00
    //����Դҳ��ַ
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)0;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)0;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)Source_PageNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(Source_PageNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(Source_PageNum>>16);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_MOVEDATA_CMD1;//��������0X35 
    //�������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
    //��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
    //�����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
    //��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
    //���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������
	res=NAND_WaitRB(0);			//�ȴ�RB=0 
	if(res)return NSTA_TIMEOUT;	//��ʱ�˳�
    //����2�д����������ж�NAND�Ƿ�׼���õ�
	res=NAND_WaitRB(1);			//�ȴ�RB=1 
    if(res)return NSTA_TIMEOUT;	//��ʱ�˳� 
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_MOVEDATA_CMD2;  //��������0X85
    //����Ŀ��ҳ��ַ
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)0;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)0;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)Dest_PageNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(Dest_PageNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(Dest_PageNum>>16);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_MOVEDATA_CMD3;	//��������0X10 
	Delay_US(NAND_TPROG_DELAY);	//�ȴ�tPROG
    if(NAND_WaitForReady()!=NSTA_READY)return NSTA_ERROR;	//NANDδ׼���� 
    return 0;//�ɹ�   
}

//��һҳ���ݿ�������һҳ,���ҿ���д������
//ע��:Դҳ��Ŀ��ҳҪ��ͬһ��Plane�ڣ�
//Source_PageNo:Դҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//Dest_PageNo:Ŀ��ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)  
//ColNo:ҳ���е�ַ,��Χ:0~(page_totalsize-1)
//pBuffer:Ҫд�������
//NumByteToWrite:Ҫд������ݸ���
//����ֵ:0,�ɹ� 
//    ����,�������
uint8_t NAND_CopyPageWithWrite(uint32_t Source_PageNum,uint32_t Dest_PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToWrite)
{
	uint8_t res=0;
    __IO uint16_t i=0;
	uint16_t source_block=0,dest_block=0;  
	uint8_t eccnum=0;		//��Ҫ�����ECC������ÿNAND_ECC_SECTOR_SIZE�ֽڼ���һ��ecc
	uint8_t eccstart=0;		//��һ��ECCֵ�����ĵ�ַ��Χ
    //�ж�Դҳ��Ŀ��ҳ�Ƿ���ͬһ��plane��
    source_block=Source_PageNum/nand_dev.block_pagenum;
    dest_block=Dest_PageNum/nand_dev.block_pagenum;
    if((source_block%2)!=(dest_block%2))return NSTA_ERROR;//����ͬһ��plane��
	*(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_MOVEDATA_CMD0;  //��������0X00
    //����Դҳ��ַ
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)0;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)0;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)Source_PageNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(Source_PageNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(Source_PageNum>>16);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_MOVEDATA_CMD1;  //��������0X35
    
    //�������д����ǵȴ�R/B���ű�Ϊ�͵�ƽ����ʵ��Ҫ����ʱ���õģ��ȴ�NAND����R/B���š���Ϊ������ͨ��
    //��STM32��NWAIT����(NAND��R/B����)����Ϊ��ͨIO��������ͨ����ȡNWAIT���ŵĵ�ƽ���ж�NAND�Ƿ�׼��
    //�����ġ����Ҳ����ģ��ķ������������ٶȺܿ��ʱ���п���NAND��û���ü�����R/B��������ʾNAND��æ
    //��״̬��������ǾͶ�ȡ��R/B����,���ʱ��϶������ģ���ʵ��ȷʵ�ǻ����!���Ҳ���Խ���������
    //���뻻����ʱ����,ֻ������������Ϊ��Ч������û������ʱ������
	res=NAND_WaitRB(0);			//�ȴ�RB=0 
	if(res)return NSTA_TIMEOUT;	//��ʱ�˳�
    //����2�д����������ж�NAND�Ƿ�׼���õ�
	res=NAND_WaitRB(1);			//�ȴ�RB=1 
    if(res)return NSTA_TIMEOUT;	//��ʱ�˳� 
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_MOVEDATA_CMD2;  //��������0X85
    //����Ŀ��ҳ��ַ
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)ColNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(ColNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)Dest_PageNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(Dest_PageNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(Dest_PageNum>>16); 
    //����ҳ���е�ַ
	NAND_Delay(NAND_TADL_DELAY);//�ȴ�tADL 
	if(NumByteToWrite%NAND_ECC_SECTOR_SIZE)//����NAND_ECC_SECTOR_SIZE����������������ECCУ��
	{  
		for(i=0;i<NumByteToWrite;i++)		//д������
		{
			*(__IO uint8_t*)NAND_ADDRESS=*(__IO uint8_t*)pBuffer++;
		}
	}else
	{
		eccnum=NumByteToWrite/NAND_ECC_SECTOR_SIZE;			//�õ�ecc�������
		eccstart=ColNum/NAND_ECC_SECTOR_SIZE; 
 		for(res=0;res<eccnum;res++)
		{
			FMC_Bank2_3->PCR3|=1<<6;						//ʹ��ECCУ�� 
			for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)				//д��NAND_ECC_SECTOR_SIZE������
			{
				*(__IO uint8_t*)NAND_ADDRESS=*(__IO uint8_t*)pBuffer++;
			}		
			while(!(FMC_Bank2_3->SR3&(1<<6)));				//�ȴ�FIFO��	
			nand_dev.ecc_hdbuf[res+eccstart]=FMC_Bank2_3->ECCR3;	//��ȡӲ��������ECCֵ
 			FMC_Bank2_3->PCR3&=~(1<<6);						//��ֹECCУ��
		}  
		i=nand_dev.page_mainsize+0X10+eccstart*4;			//����д��ECC��spare����ַ
		NAND_Delay(NAND_TADL_DELAY);//�ȴ�tADL 
		*(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=0X85;				//���дָ��
		//���͵�ַ
		*(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)i;
		*(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(i>>8);
		NAND_Delay(NAND_TADL_DELAY);//�ȴ�tADL 
		pBuffer=(uint8_t*)&nand_dev.ecc_hdbuf[eccstart];
		for(i=0;i<eccnum;i++)					//д��ECC
		{ 
			for(res=0;res<4;res++)				 
			{
				*(__IO uint8_t*)NAND_ADDRESS=*(__IO uint8_t*)pBuffer++;
			}
		} 		
	}
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_MOVEDATA_CMD3;	//��������0X10 
 	Delay_US(NAND_TPROG_DELAY);							//�ȴ�tPROG
    if(NAND_WaitForReady()!=NSTA_READY)return NSTA_ERROR;	//ʧ��
    return 0;	//�ɹ�   
} 
//��ȡspare���е�����
//PageNum:Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫд���spare����ַ(spare�����ĸ���ַ),��Χ:0~(page_sparesize-1) 
//pBuffer:�������ݻ����� 
//NumByteToRead:Ҫ��ȡ���ֽ���(������page_sparesize)
//����ֵ:0,�ɹ�
//    ����,�������
uint8_t NAND_ReadSpare(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToRead)
{
    uint8_t temp=0;
    uint8_t remainbyte=0;
    remainbyte=nand_dev.page_sparesize-ColNum;
    if(NumByteToRead>remainbyte) NumByteToRead=remainbyte;  //ȷ��Ҫд����ֽ���������spareʣ��Ĵ�С
    temp=NAND_ReadPage(PageNum,ColNum+nand_dev.page_mainsize,pBuffer,NumByteToRead);//��ȡ����
    return temp;
} 
//��spare����д����
//PageNum:Ҫд���ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫд���spare����ַ(spare�����ĸ���ַ),��Χ:0~(page_sparesize-1)  
//pBuffer:Ҫд��������׵�ַ 
//NumByteToWrite:Ҫд����ֽ���(������page_sparesize)
//����ֵ:0,�ɹ�
//    ����,ʧ��
uint8_t NAND_WriteSpare(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToWrite)
{
    uint8_t temp=0;
    uint8_t remainbyte=0;
    remainbyte=nand_dev.page_sparesize-ColNum;
    if(NumByteToWrite>remainbyte) NumByteToWrite=remainbyte;  //ȷ��Ҫ��ȡ���ֽ���������spareʣ��Ĵ�С
    temp=NAND_WritePage(PageNum,ColNum+nand_dev.page_mainsize,pBuffer,NumByteToWrite);//��ȡ
    return temp;
} 
//����һ����
//BlockNum:Ҫ������BLOCK���,��Χ:0-(block_totalnum-1)
//����ֵ:0,�����ɹ�
//    ����,����ʧ��
uint8_t NAND_EraseBlock(uint32_t BlockNum)
{
	if(nand_dev.id==MT29F16G08ABABA)BlockNum<<=7;  	//�����ַת��Ϊҳ��ַ
    else if(nand_dev.id==MT29F4G08ABADA)BlockNum<<=6;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_ERASE0;
    //���Ϳ��ַ
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)BlockNum;
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(BlockNum>>8);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_ADDR)=(uint8_t)(BlockNum>>16);
    *(__IO uint8_t*)(NAND_ADDRESS|NAND_CMD)=NAND_ERASE1;
	HAL_Delay(NAND_TBERS_DELAY);		//�ȴ������ɹ�
	if(NAND_WaitForReady()!=NSTA_READY)return NSTA_ERROR;//ʧ��
    return 0;	//�ɹ�   
} 
//ȫƬ����NAND FLASH
void NAND_EraseChip(void)
{
    uint8_t status;
    uint16_t i=0;
    for(i=0;i<nand_dev.block_totalnum;i++) //ѭ���������еĿ�
    {
        status=NAND_EraseBlock(i);
        if(status)LOG_Info("Erase %d block fail!!��������Ϊ%d",i,status);//����ʧ��
    }
}

void NAND_EraseChip_All(void)
{
    uint8_t buf[20] = {0xFF, 0xFF, 0x00, 0x00};
    uint8_t status, temp;
    uint16_t i=0;
    
    for(i=0;i<nand_dev.block_totalnum;i++) //ѭ���������еĿ�
    {
        status=NAND_EraseBlock(i);
        if(status)LOG_Info("Erase %d block fail!!��������Ϊ%d",i,status);//����ʧ��     
    }
    //
    for(i=0; i<nand_dev.block_totalnum; i++)
    {
        buf[2] = i;
        buf[3] = i >> 8;
        temp=NAND_WritePage(i*nand_dev.block_pagenum, nand_dev.page_mainsize, buf, 4);
        if(temp)LOG_Info("write spare 1 or 2 in block fail!!��������Ϊ%d",i);//����ʧ��
        temp=NAND_WritePage(i*nand_dev.block_pagenum+1, nand_dev.page_mainsize, buf, 4);
        if(temp)LOG_Info("write spare 1 or 2 in block fail!!��������Ϊ%d",i);//����ʧ��
    }
 }

//��ȡECC������λ/ż��λ
//oe:0,ż��λ
//   1,����λ
//eccval:�����eccֵ
//����ֵ:������eccֵ(���16λ)
uint16_t NAND_ECC_Get_OE(uint8_t oe,uint32_t eccval)
{
	uint8_t i;
	uint16_t ecctemp=0;
	for(i=0;i<24;i++)
	{
		if((i%2)==oe)
		{
			if((eccval>>i)&0X01)ecctemp+=1<<(i>>1); 
		}
	}
	return ecctemp;
} 
//ECCУ������
//eccrd:��ȡ����,ԭ�������ECCֵ
//ecccl:��ȡ����ʱ,Ӳ�������ECCֻ
//����ֵ:0,����������
//    ����,ECC����(�д���2��bit�Ĵ���,�޷��ָ�)
uint8_t NAND_ECC_Correction(uint8_t* data_buf,uint32_t eccrd,uint32_t ecccl)
{
	uint16_t eccrdo,eccrde,eccclo,ecccle;
	uint16_t eccchk=0;
	uint16_t errorpos=0; 
	uint32_t bytepos=0;  
	eccrdo=NAND_ECC_Get_OE(1,eccrd);	//��ȡeccrd������λ
	eccrde=NAND_ECC_Get_OE(0,eccrd);	//��ȡeccrd��ż��λ
	eccclo=NAND_ECC_Get_OE(1,ecccl);	//��ȡecccl������λ
	ecccle=NAND_ECC_Get_OE(0,ecccl); 	//��ȡecccl��ż��λ
	eccchk=eccrdo^eccrde^eccclo^ecccle;
	if(eccchk==0XFFF)	//ȫ1,˵��ֻ��1bit ECC����
	{
		errorpos=eccrdo^eccclo; 
		LOG_Info("errorpos:%d",errorpos); 
		bytepos=errorpos/8; 
		data_buf[bytepos]^=1<<(errorpos%8);
	}else				//����ȫ1,˵��������2bit ECC����,�޷��޸�
	{
		LOG_Info("2bit ecc error or more");
		return 1;
	} 
	return 0;
}
 

#endif // USE_RL_FLASH_FS




