#include "ftl.h"
#include "bsp_nand.h"
#include "stdio.h"
#include "cmsis_os2.h"
#include "stdlib.h"
#include "main.h"

#if !USE_RL_FLASH_FS

//FTL���ʼ��
//����ֵ:0,����
//    ����,ʧ��
uint8_t FTL_Init(void)
{
    uint8_t temp;
    if(NAND_Init())return 1;									//��ʼ��NAND FLASH
//	if(nand_dev.lut)myfree(SRAMIN,nand_dev.lut);
//	nand_dev.lut=mymalloc(SRAMIN,(nand_dev.block_totalnum)*2); 	//��LUT�������ڴ�
//	memset((void*)nand_dev.lut,0,nand_dev.block_totalnum*2);			//ȫ������
//    if(!nand_dev.lut)return 1;				//�ڴ�����ʧ�� 
    temp=FTL_CreateLUT(1);
    if(temp) 
    {   
        printf("format nand flash...");
        temp=FTL_Format();     //��ʽ��NAND
        if(temp)
        {
            printf("format failed!");
            return 2;
        }
    }else 	//����LUT���ɹ�
	{
		printf("total block num:%d",nand_dev.block_totalnum);
		printf("good block num:%d",nand_dev.good_blocknum);
		printf("valid block num:%d",nand_dev.valid_blocknum);
    }
	return 0;
} 

//���ĳһ����Ϊ����
//blocknum:����,��Χ:0~(block_totalnum-1)
void FTL_BadBlockMark(uint32_t blocknum)
{
    uint32_t temp=0XAAAAAAAA;//������mark,����ֵ��OK,ֻҪ����0XFF.����дǰ4���ֽ�,����FTL_FindUnusedBlock������黵��.(����鱸����,������ٶ�)
    NAND_WriteSpare(blocknum*nand_dev.block_pagenum,0,(uint8_t*)&temp,4);	//�ڵ�һ��page��spare��,��һ���ֽ���������(ǰ4���ֽڶ�д)
    NAND_WriteSpare(blocknum*nand_dev.block_pagenum+1,0,(uint8_t*)&temp,4);	//�ڵڶ���page��spare��,��һ���ֽ���������(������,ǰ4���ֽڶ�д)
} 
//���ĳһ���Ƿ��ǻ���
//blocknum:����,��Χ:0~(block_totalnum-1)
//����ֵ:0,�ÿ�
//	  ����,����
uint8_t FTL_CheckBadBlock(uint32_t blocknum)
{
    uint8_t flag=0; 
    NAND_ReadSpare(blocknum*nand_dev.block_pagenum,0,&flag,1);//��ȡ�����־
    if(flag==0XFF)//�ÿ�?,��ȡ������������
    {
        NAND_ReadSpare(blocknum*nand_dev.block_pagenum+1,0,&flag,1);//��ȡ�����������־
        if(flag==0XFF)return 0;	//�ÿ�
        else return 1;  		//����
    }   
	return 2; 
}
//���ĳһ�����Ѿ�ʹ��
//blocknum:����,��Χ:0~(block_totalnum-1)
//����ֵ:0,�ɹ�
//    ����,ʧ��
uint8_t FTL_UsedBlockMark(uint32_t blocknum)
{
    uint8_t Usedflag=0XCC;
    uint8_t temp=0;
    temp=NAND_WriteSpare(blocknum*nand_dev.block_pagenum,1,(uint8_t*)&Usedflag,1);//д����Ѿ���ʹ�ñ�־
    return temp;
}   
//�Ӹ����Ŀ鿪ʼ�ҵ���ǰ�ҵ�һ��δ��ʹ�õĿ�(ָ������/ż��)
//sblock:��ʼ��,��Χ:0~(block_totalnum-1)
//flag:0,ż����;1,������.
//����ֵ:0XFFFFFFFF,ʧ��
//           ����ֵ,δʹ�ÿ��
uint32_t FTL_FindUnusedBlock(uint32_t sblock,uint8_t flag)
{
    uint32_t temp=0;
    uint32_t blocknum=0; 
	for(blocknum=sblock+1;blocknum>0;blocknum--)
    {
        if(((blocknum-1)%2)==flag)//��ż�ϸ�,�ż��
		{
		    NAND_ReadSpare((blocknum-1)*nand_dev.block_pagenum,0,(uint8_t*)&temp,4);//�����Ƿ�ʹ�ñ��
 			if(temp==0XFFFFFFFF)return(blocknum-1);//�ҵ�һ���տ�,���ؿ���
		}
    }
    return 0XFFFFFFFF;	//δ�ҵ������
    
} 
//�������������ͬһ��plane�ڵ�δʹ�õĿ�
//sblock��������,��Χ:0~(block_totalnum-1)
//����ֵ:0XFFFFFFFF,ʧ��
//           ����ֵ,δʹ�ÿ��
uint32_t FTL_FindSamePlaneUnusedBlock(uint32_t sblock)
{
	static uint32_t curblock=0XFFFFFFFF;
	uint32_t unusedblock=0;  
	if(curblock>(nand_dev.block_totalnum-1))curblock=nand_dev.block_totalnum-1;//������Χ��,ǿ�ƴ����һ���鿪ʼ
  	unusedblock=FTL_FindUnusedBlock(curblock,sblock%2);					//�ӵ�ǰ��,��ʼ,��ǰ���ҿ���� 
 	if(unusedblock==0XFFFFFFFF&&curblock<(nand_dev.block_totalnum-1))	//δ�ҵ�,�Ҳ��Ǵ���ĩβ��ʼ�ҵ�
	{
		curblock=nand_dev.block_totalnum-1;								//ǿ�ƴ����һ���鿪ʼ
		unusedblock=FTL_FindUnusedBlock(curblock,sblock%2);				//����ĩβ��ʼ,������һ��  
	}
	if(unusedblock==0XFFFFFFFF)return 0XFFFFFFFF;						//�Ҳ�������block 
	curblock=unusedblock;												//��ǰ��ŵ���δʹ�ÿ���.�´���Ӵ˴���ʼ����
 	return unusedblock;													//�����ҵ��Ŀ���block
}    
//��һ��������ݿ�������һ��,���ҿ���д������ 
//Source_PageNo:Ҫд�����ݵ�ҳ��ַ,��Χ:0~(block_pagenum*block_totalnum-1)
//ColNum:Ҫд����п�ʼ��ַ(Ҳ����ҳ�ڵ�ַ),��Χ:0~(page_totalsize-1)
//pBuffer:Ҫд������� 
//NumByteToWrite:Ҫд����ֽ�������ֵ���ܳ�������ʣ��������С
//����ֵ:0,�ɹ�
//    ����,ʧ��
uint8_t FTL_CopyAndWriteToBlock(uint32_t Source_PageNum,uint16_t ColNum,uint8_t *pBuffer,uint32_t NumByteToWrite)
{
    uint16_t i=0,temp=0,wrlen;
    uint32_t source_block=0,pageoffset=0;
    uint32_t unusedblock=0; 
    source_block=Source_PageNum/nand_dev.block_pagenum;	//���ҳ���ڵĿ��
    pageoffset=Source_PageNum%nand_dev.block_pagenum;	//���ҳ�����ڿ��ڵ�ƫ�� 
retry:      
    unusedblock=FTL_FindSamePlaneUnusedBlock(source_block);//������Դ����һ��plane��δʹ�ÿ�
    if(unusedblock>nand_dev.block_totalnum)return 1;	//���ҵ��Ŀ����Ŵ��ڿ��������Ļ��϶��ǳ�����
    for(i=0;i<nand_dev.block_pagenum;i++)				//��һ��������ݸ��Ƶ��ҵ���δʹ�ÿ���
    {                                                                                                                                                                                                                                                                                                                                                                                                                                                       
        if(i>=pageoffset&&NumByteToWrite)				//����Ҫд�뵽��ǰҳ
        { 
			if(NumByteToWrite>(nand_dev.page_mainsize-ColNum))//Ҫд�������,�����˵�ǰҳ��ʣ������
			{
				wrlen=nand_dev.page_mainsize-ColNum;	//д�볤�ȵ��ڵ�ǰҳʣ�����ݳ���
			}else wrlen=NumByteToWrite;					//д��ȫ������ 
            temp=NAND_CopyPageWithWrite(source_block*nand_dev.block_pagenum+i,unusedblock*nand_dev.block_pagenum+i,ColNum,pBuffer,wrlen);
			ColNum=0;						//�е�ַ����
			pBuffer+=wrlen;					//д��ַƫ��
			NumByteToWrite-=wrlen;			//д�����ݼ���			
 		}else								//������д��,ֱ�ӿ�������
		{
			temp=NAND_CopyPageWithoutWrite(source_block*nand_dev.block_pagenum+i,unusedblock*nand_dev.block_pagenum+i);
		}
		if(temp)							//����ֵ����,�����鴦��
		{ 
 			FTL_BadBlockMark(unusedblock);	//���Ϊ����
			FTL_CreateLUT(1);				//�ؽ�LUT��
			goto retry;
		}
    } 
    if(i==nand_dev.block_pagenum) 			//�������
    {
        FTL_UsedBlockMark(unusedblock);		//��ǿ��Ѿ�ʹ��	
        NAND_EraseBlock(source_block);		//����Դ��
		//printf("copy block %d to block %d",source_block,unusedblock);//��ӡ������Ϣ
		for(i=0;i<nand_dev.block_totalnum;i++)	//����LUT������unusedblock�滻source_block
		{
			if(nand_dev.lut[i]==source_block)
			{
				nand_dev.lut[i]=unusedblock;
				break;
			}
		}  
    }
    return 0;                               //�ɹ�
}   
//�߼����ת��Ϊ�������
//LBNNum:�߼�����
//����ֵ:��������
uint16_t FTL_LBNToPBN(uint32_t LBNNum)
{
    uint16_t PBNNo=0;
    //���߼���Ŵ�����Ч������ʱ�򷵻�0XFFFF
    if(LBNNum>nand_dev.valid_blocknum)return 0XFFFF;
    PBNNo=nand_dev.lut[LBNNum];
    return PBNNo;
}
//д����(֧�ֶ�����д)��FATFS�ļ�ϵͳʹ��
//pBuffer:Ҫд�������
//SectorNo:��ʼ������
//SectorSize:������С(���ܴ���NAND_ECC_SECTOR_SIZE����Ĵ�С,��������!!)
//SectorCount:Ҫд�����������
//����ֵ:0,�ɹ�
//	  ����,ʧ��
uint8_t FTL_WriteSectors(uint8_t *pBuffer,uint32_t SectorNo,uint16_t SectorSize,uint32_t SectorCount)
{
    uint8_t flag=0;
	uint16_t temp;
    uint32_t i=0;
	uint16_t wsecs;		//дҳ��С
	uint32_t wlen;		//д�볤��
    uint32_t LBNNo;      //�߼����
    uint32_t PBNNo;      //�������
    uint32_t PhyPageNo;  //����ҳ��
    uint32_t PageOffset; //ҳ��ƫ�Ƶ�ַ
    uint32_t BlockOffset;//����ƫ�Ƶ�ַ
	uint32_t markdpbn=0XFFFFFFFF;		//����˵���������  
	for(i=0;i<SectorCount;i++)
    {
        LBNNo=(SectorNo+i)/(nand_dev.block_pagenum*(nand_dev.page_mainsize/SectorSize));//�����߼������ź�������С������߼����
        PBNNo=FTL_LBNToPBN(LBNNo);					//���߼���ת��Ϊ������
        if(PBNNo>=nand_dev.block_totalnum)return 1;	//������Ŵ���NAND FLASH���ܿ���,��ʧ��. 
        BlockOffset=((SectorNo+i)%(nand_dev.block_pagenum*(nand_dev.page_mainsize/SectorSize)))*SectorSize;//�������ƫ��
        PhyPageNo=PBNNo*nand_dev.block_pagenum+BlockOffset/nand_dev.page_mainsize;	//���������ҳ��
        PageOffset=BlockOffset%nand_dev.page_mainsize;								//�����ҳ��ƫ�Ƶ�ַ 
 		temp=nand_dev.page_mainsize-PageOffset;	//page��ʣ���ֽ���
		temp/=SectorSize;						//��������д���sector�� 
		wsecs=SectorCount-i;					//��ʣ���ٸ�sectorҪд
		if(wsecs>=temp)wsecs=temp;				//���ڿ�����д���sector��,��д��temp������  
		wlen=wsecs*SectorSize;					//ÿ��дwsecs��sector  
		//����д���С�������ж��Ƿ�ȫΪ0XFF
		flag=NAND_ReadPageComp(PhyPageNo,PageOffset,0XFFFFFFFF,wlen/4,&temp);		//��һ��wlen/4��С������,����0XFFFFFFFF�Ա�
		if(flag)return 2;						//��д���󣬻��� 
		if(temp==(wlen/4)) flag=NAND_WritePage(PhyPageNo,PageOffset,pBuffer,wlen);	//ȫΪ0XFF,����ֱ��д����
		else flag=1;							//��ȫ��0XFF,����������
		if(flag==0&&(markdpbn!=PBNNo))			//ȫ��0XFF,��д��ɹ�,�ұ���˵��������뵱ǰ�����鲻ͬ
		{
			flag=FTL_UsedBlockMark(PBNNo);		//��Ǵ˿��Ѿ�ʹ��  
			markdpbn=PBNNo;						//������,��ǿ�=��ǰ��,��ֹ�ظ����
		}
		if(flag)//��ȫΪ0XFF/���ʧ�ܣ�������д����һ����   
        {
			temp=((uint32_t)nand_dev.block_pagenum*nand_dev.page_mainsize-BlockOffset)/SectorSize;//��������block��ʣ�¶��ٸ�SECTOR����д��
 			wsecs=SectorCount-i;				//��ʣ���ٸ�sectorҪд
			if(wsecs>=temp)wsecs=temp;			//���ڿ�����д���sector��,��д��temp������ 
			wlen=wsecs*SectorSize;				//ÿ��дwsecs��sector   
            flag=FTL_CopyAndWriteToBlock(PhyPageNo,PageOffset,pBuffer,wlen);//����������һ��block,��д������
            if(flag)return 3;//ʧ�� 
        } 
		i+=wsecs-1;
		pBuffer+=wlen;//���ݻ�����ָ��ƫ��
    }
    return 0;   
} 
//������(֧�ֶ�������)��FATFS�ļ�ϵͳʹ��
//pBuffer:���ݻ�����
//SectorNo:��ʼ������
//SectorSize:������С
//SectorCount:Ҫд�����������
//����ֵ:0,�ɹ�
//	  ����,ʧ��
uint8_t FTL_ReadSectors(uint8_t *pBuffer,uint32_t SectorNo,uint16_t SectorSize,uint32_t SectorCount)
{
    uint8_t flag=0;
	uint16_t rsecs;		//���ζ�ȡҳ�� 
    uint32_t i=0;
    uint32_t LBNNo;      //�߼����
    uint32_t PBNNo;      //�������
    uint32_t PhyPageNo;  //����ҳ��
    uint32_t PageOffset; //ҳ��ƫ�Ƶ�ַ
    uint32_t BlockOffset;//����ƫ�Ƶ�ַ 
    for(i=0;i<SectorCount;i++)
    {
        LBNNo=(SectorNo+i)/(nand_dev.block_pagenum*(nand_dev.page_mainsize/SectorSize));//�����߼������ź�������С������߼����
        PBNNo=FTL_LBNToPBN(LBNNo);					//���߼���ת��Ϊ������
        if(PBNNo>=nand_dev.block_totalnum)return 1;	//������Ŵ���NAND FLASH���ܿ���,��ʧ��.  
        BlockOffset=((SectorNo+i)%(nand_dev.block_pagenum*(nand_dev.page_mainsize/SectorSize)))*SectorSize;//�������ƫ��
        PhyPageNo=PBNNo*nand_dev.block_pagenum+BlockOffset/nand_dev.page_mainsize;	//���������ҳ��
        PageOffset=BlockOffset%nand_dev.page_mainsize;                     			//�����ҳ��ƫ�Ƶ�ַ 
		rsecs=(nand_dev.page_mainsize-PageOffset)/SectorSize;						//����һ�������Զ�ȡ����ҳ
		if(rsecs>(SectorCount-i))rsecs=SectorCount-i;								//��಻�ܳ���SectorCount-i
		flag=NAND_ReadPage(PhyPageNo,PageOffset,pBuffer,rsecs*SectorSize);			//��ȡ����
		if(flag==NSTA_ECC1BITERR)													//����1bit ecc����,����Ϊ����
		{	
			flag=NAND_ReadPage(PhyPageNo,PageOffset,pBuffer,rsecs*SectorSize);		//�ض�����,�ٴ�ȷ��
			if(flag==NSTA_ECC1BITERR)
			{
 				FTL_CopyAndWriteToBlock(PhyPageNo,PageOffset,pBuffer,rsecs*SectorSize);	//�������� 
				flag=FTL_BlockCompare(PhyPageNo/nand_dev.block_pagenum,0XFFFFFFFF);		//ȫ1���,ȷ���Ƿ�Ϊ����
				if(flag==0)
				{
					flag=FTL_BlockCompare(PhyPageNo/nand_dev.block_pagenum,0X00);		//ȫ0���,ȷ���Ƿ�Ϊ����
					NAND_EraseBlock(PhyPageNo/nand_dev.block_pagenum);					//�����ɺ�,���������
				}
				if(flag)																//ȫ0/ȫ1������,�϶��ǻ�����.
				{
					FTL_BadBlockMark(PhyPageNo/nand_dev.block_pagenum);					//���Ϊ����
					FTL_CreateLUT(1);													//�ؽ�LUT�� 
				}
				flag=0;
			}
		}
		if(flag==NSTA_ECC2BITERR)flag=0;	//2bit ecc����,������(�����ǳ���д�����ݵ��µ�)
		if(flag)return 2;					//ʧ��
		pBuffer+=SectorSize*rsecs;			//���ݻ�����ָ��ƫ�� 
		i+=rsecs-1;
    }
    return 0; 
}
//���´���LUT��
//mode:0,������һ��������
//     1,���������Ƕ�Ҫ���(������ҲҪ���)
//����ֵ:0,�ɹ�
//    ����,ʧ��
uint8_t FTL_CreateLUT(uint8_t mode)
{
    uint32_t i;
 	uint8_t buf[4];
    uint32_t LBNnum=0;								//�߼���� 
    for(i=0;i<nand_dev.block_totalnum;i++)		//��λLUT������ʼ��Ϊ��Чֵ��Ҳ����0XFFFF
    {
        nand_dev.lut[i]=0XFFFF;
    } 
	nand_dev.good_blocknum=0;
    for(i=0;i<nand_dev.block_totalnum;i++)
    {
		NAND_ReadSpare(i*nand_dev.block_pagenum,0,buf,4);	//��ȡ4���ֽ�
		if(buf[0]==0XFF&&mode)NAND_ReadSpare(i*nand_dev.block_pagenum+1,0,buf,1);//�ÿ�,����Ҫ���2�λ�����
		if(buf[0]==0XFF)//�Ǻÿ� 				 
        { 
			LBNnum=((uint16_t)buf[3]<<8)+buf[2];		//�õ��߼�����
            if(LBNnum<nand_dev.block_totalnum)	//�߼���ſ϶�С���ܵĿ�����
            {
                nand_dev.lut[LBNnum]=i;			//����LUT����дLBNnum��Ӧ����������
            }
			nand_dev.good_blocknum++;
		}else printf("bad block index:%d", i);
    } 
    
    //��ӡlut
    LOG_Info("----------- lut ----------");
    for(i=0;i<nand_dev.block_totalnum;i++)
    {
     //   LOG_Info("[%04d]=%04d\t", i, nand_dev.lut[i]);
     //   if(i%17 == 0) LOG_Info("");
    }
    
    //LOG_Info("-----------valid_Block----------");
    //LUT����������Ժ�����Ч�����
    for(i=0;i<nand_dev.block_totalnum;i++)
    {
        if(nand_dev.lut[i]>=nand_dev.block_totalnum)
        {
            //LOG_Info("[%04d]=%04d\t", i, nand_dev.lut[i]);
            nand_dev.valid_blocknum=i;
            break;
        }
    }
    if(nand_dev.valid_blocknum<100)return 2;	//��Ч����С��100,������.��Ҫ���¸�ʽ�� 
    return 0;	//LUT���������
} 
//FTL����Block��ĳ�����ݶԱ�
//blockx:block���
//cmpval:Ҫ��֮�Աȵ�ֵ
//����ֵ:0,���ɹ�,ȫ�����
//       1,���ʧ��,�в���ȵ����
uint8_t FTL_BlockCompare(uint32_t blockx,uint32_t cmpval)
{
	uint8_t res;
	uint16_t i,j,k; 
	for(i=0;i<3;i++)//����3�λ���
	{
		for(j=0;j<nand_dev.block_pagenum;j++)
		{
			NAND_ReadPageComp(blockx*nand_dev.block_pagenum,0,cmpval,nand_dev.page_mainsize/4,&k);//���һ��page,����0XFFFFFFFF�Ա�
			if(k!=(nand_dev.page_mainsize/4))break;
		}
		if(j==nand_dev.block_pagenum)return 0;		//���ϸ�,ֱ���˳�
		res=NAND_EraseBlock(blockx);
		if(res)printf("error erase block:%d",i);
		else
		{ 
			if(cmpval!=0XFFFFFFFF)//�����ж�ȫ1,����Ҫ��д����
			{
				for(k=0;k<nand_dev.block_pagenum;k++)
				{
					NAND_WritePageConst(blockx*nand_dev.block_pagenum+k,0,0,nand_dev.page_mainsize/4);//дPAGE 
				}
			}
		}
	}
	printf("bad block checked:%d",blockx);
	return 1;
}
//FTL��ʼ��ʱ����Ѱ���л���,ʹ��:��-д-�� ��ʽ
//512M��NAND ,��ҪԼ3����ʱ��,����ɼ��
//����RGB��,����Ƶ����дNAND,��������Ļ����
//����ֵ���ÿ������
uint32_t FTL_SearchBadBlock(void)
{
    //modify
	uint8_t blktbl[2048] = {0};;//uint8_t *blktbl;
	uint8_t res;
	uint32_t i,j; 
	uint32_t goodblock=0;
	//blktbl=mymalloc(SRAMIN,nand_dev.block_totalnum);//����block������ڴ�,��Ӧ��:0,�ÿ�;1,����;
	NAND_EraseChip(); 						//ȫƬ����
    for(i=0;i<nand_dev.block_totalnum;i++)	//��һ�׶μ��,���ȫ1
    {
 		res=FTL_BlockCompare(i,0XFFFFFFFF);	//ȫ1��� 
		if(res)blktbl[i]=1;					//���� 
		else
		{ 
			blktbl[i]=0;					//�ÿ� 
			for(j=0;j<nand_dev.block_pagenum;j++)//дblockΪȫ0,Ϊ����ļ��׼��
			{
				NAND_WritePageConst(i*nand_dev.block_pagenum+j,0,0,nand_dev.page_mainsize/4);
			} 
		}
    }	
    for(i=0;i<nand_dev.block_totalnum;i++)	//�ڶ��׶μ��,���ȫ0
    { 
 		if(blktbl[i]==0)					//�ڵ�һ�׶�,û�б���ǻ����,�ſ����Ǻÿ�
		{
			res=FTL_BlockCompare(i,0);		//ȫ0��� 
			if(res)blktbl[i]=1;				//��ǻ���
			else goodblock++; 
		}
    }
	NAND_EraseChip();  	//ȫƬ����
    for(i=0;i<nand_dev.block_totalnum;i++)	//�����׶μ��,��ǻ���
    { 
		if(blktbl[i])FTL_BadBlockMark(i);	//�ǻ���
	}
	return goodblock;	//���غÿ������
}

//��ʽ��NAND �ؽ�LUT��
//����ֵ:0,�ɹ�
//    ����,ʧ��
uint8_t FTL_Format(void)
{
    uint16_t laddr = 0;
    
    uint8_t temp;
    uint32_t i,n;
    uint32_t goodblock=0;
	nand_dev.good_blocknum=0;
#if FTL_USE_BAD_BLOCK_SEARCH==1				//ʹ�ò�-д-���ķ�ʽ,��⻵��
	nand_dev.good_blocknum=FTL_SearchBadBlock();//��Ѱ����.��ʱ�ܾ�
#else										//ֱ��ʹ��NAND FLASH�ĳ��������־(������,Ĭ���Ǻÿ�)
    NAND_EraseChip(); //////
    printf("Nand_EraseChip Finished");
    for(i=0;i<nand_dev.block_totalnum;i++)	
    {
		temp=FTL_CheckBadBlock(i);			//���һ�����Ƿ�Ϊ����
        if(temp==0)							//�ÿ�
        { 
			temp=NAND_EraseBlock(i);
			if(temp)						//����ʧ��,��Ϊ����
			{
				printf("Bad block:%d",i);
				FTL_BadBlockMark(i);		//����ǻ���
			}else{
                NAND_ReadSpare(i*nand_dev.block_pagenum, 2,(uint8_t*)&laddr,2);
                LOG_Info("[%04d0]=%04d\t", i, laddr);
                nand_dev.good_blocknum++;	//�ÿ�������һ 
            }
		}
	} 
#endif
    printf("good_blocknum:%d",nand_dev.good_blocknum); 
    if(nand_dev.good_blocknum<100) return 1;	//����ÿ����������100����NAND Flash����   
    goodblock=(nand_dev.good_blocknum*93)/100;	//%93�ĺÿ����ڴ洢����  
    n=0;										
    for(i=0;i<nand_dev.block_totalnum;i++)		//�ںÿ��б�����߼�����Ϣ
    {
        temp=FTL_CheckBadBlock(i);  			//���һ�����Ƿ�Ϊ����
        if(temp==0)                  			//�ÿ�
        { 
            NAND_WriteSpare(i*nand_dev.block_pagenum,2,(uint8_t*)&n,2);//д���߼�����
            n++;								//�߼����ż�1
            if(n==goodblock) break;				//ȫ���������
        }
    }

    //printf lna
    for(i=0;i<nand_dev.block_totalnum;i++)	
    {
        NAND_ReadSpare(i*nand_dev.block_pagenum, 2,(uint8_t*)&laddr,2);
        LOG_Info("[%04d0]=%04d\t", i, laddr);
        if(i%17 == 0) LOG_Info("");
        Delay_US(200);
    }
    
    if(FTL_CreateLUT(1))return 2;      			//�ؽ�LUT��ʧ�� 
    return 0;
}





/*
*   FTL ���Խӿ�
*/
#ifdef CX_DEBUG
#define SECTOR_NUM  115530
void FTL_Debug(uint8_t ucOpt)
{
    uint8_t buf[NAND_ECC_SECTOR_SIZE] = {0}, uckey = 0;
    uint8_t sbuf[8] = {0};
    uint16_t i = 0;
    
    srand(HAL_GetTick());
    
    printf("");
    if(ucOpt == 0)
    {
        // read
        LOG_Info("FTL Read Start.......");
        //__disable_irq();
        uckey = NAND_ReadPage(SECTOR_NUM, 0, buf, NAND_ECC_SECTOR_SIZE);
        //__enable_irq();
        if(uckey == 0)
        {
            LOG_Info("FTL Read Success");
        }else{
            LOG_Info("FTL Read Failure");
        }
        //
        for(i = 0; i < NAND_ECC_SECTOR_SIZE; i++)
        {
            printf("%03d=%d\t", i, buf[i]);
        }
        printf("");
            
        //read spare zoom
        //buf[0] = 0, buf[1] = 0, buf[2] = 0, buf[3] = 0,
        NAND_ReadSpare(SECTOR_NUM, nand_dev.page_mainsize, sbuf, 4);
        LOG_Info("spare zoom buf4: %d, %d, %d, %d", sbuf[0], sbuf[1], sbuf[2], sbuf[3]);
        
    }else if(ucOpt == 1){
        // write
         LOG_Info("FTL Write Start.......");

        for(i = 0; i < NAND_ECC_SECTOR_SIZE; i++)
        {
             buf[i] = rand()%255;
            printf("%03d=%d\t", i, buf[i]);
        }
    
        //__disable_irq();
        uckey = NAND_WritePage(SECTOR_NUM, 0, buf, NAND_ECC_SECTOR_SIZE);
        //__enable_irq();
        if(uckey == 0)
        {
            LOG_Info("FTL Write Success");

        }else{
            LOG_Info("FTL Write Failure");
        }
        
        //write spare zoom
//        buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF;
//        NAND_WriteSpare(SECTOR_NUM, nand_dev.page_mainsize, buf, 4);
    }    
}

#endif //CX_DEBUG


#endif //USE_RL_FLASH_FS
