#ifndef __UI_FILES_H__
#define __UI_FILES_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f4xx_hal.h"
#include "file_operate.h"
#include "GUI_Type.h"



/*
*   背景画面的类型
*/
typedef enum {

    //analysis
    BK_PICTURE_ANALYSIS_WARN_BLUE = 0,
    BK_PICTURE_ANALYSIS_WANR_WHITE ,
    BK_PICTURE_ANALYSIS_COUNT_FAIL ,     		//计数失败，静态界面部分的背景
    
    
    //others
    BK_PICTURE_OTHERS_IconBkSmall ,     		//其他背景图片，用于一级菜单按键背景，小号
    BK_PICTURE_OTHERS_IconBkBig ,     			//其他背景图片，用于一级菜单按键背景，大号
	BK_PICTURE_OTHERS_POWER_ON_OFF ,     		//其他背景图片，开机、关机背景
	BK_PICTURE_OTHERS_LOGIN ,     				//其他背景图片，登录背景
    BK_PICTURE_OTHERS_SELF_CHECK ,     			//其他背景图片，自检背景
    
    //analysis //当前这几个没有加载到SDRAM中
    BK_PICTURE_COUNTING ,     					//样本正在分析，背景图片
    BK_PICTURE_IF_RE_COUNT ,    	 			//是否重新测试，背景图片
    BK_PICTURE_N_SEC_OUTIN_IN ,     			//N秒后即将进仓，背景图片
    BK_PICTURE_PRINTER_NO_PAPER ,     			//打印机缺纸，背景图片
    BK_PICTURE_PRINTER_CONN_ERROR ,     		//打印机连接异常，背景图片
    BK_PICTURE_PRINTER_TAKEOUT_CELL ,     		//请取出血细胞检测模块，背景图片
    BK_PICTURE_PRINTER_CONFIRM_TAKEOUT ,     	//请确认检测模块是否取出，背景图片    

    //listview

    //qc
    BK_PICTURE_QC_SET            ,     			//质控设定按钮，背景图片
    BK_PICTURE_QC_ANALYSIS       ,     			//质控分析按钮，背景图片
    BK_PICTURE_QC_LIST           ,     			//质控 质控图按钮，背景图片
			
    //set			
    BK_PICTURE_SET_ChangeLogo    ,     			//设置界面，更换logo
    BK_PICTURE_SET_SwitchClose   ,     			//设置界面，
    BK_PICTURE_SET_SwitchOpen    ,     			//设置界面，
    BK_PICTURE_SET_UpdateLogo    ,     			//设置界面，
    //			
    BK_PICTURE_SET_FuZhu         ,     			//设置界面，  
    BK_PICTURE_SET_FuZhuUnactive ,     			//设置界面，      
    BK_PICTURE_SET_JiaoZhun      ,     			//设置界面，
    BK_PICTURE_SET_JiaoZhunGray  ,     			//设置界面，
    BK_PICTURE_SET_LabInfo       ,     			//设置界面，
    BK_PICTURE_SET_Para          ,     			//设置界面，
    BK_PICTURE_SET_ParaGray      ,     			//设置界面，
    BK_PICTURE_SET_Printer       ,     			//设置界面，    
    BK_PICTURE_SET_Sys           ,     			//设置界面，
    BK_PICTURE_SET_Test          ,     			//设置界面，
    BK_PICTURE_SET_TestGray      ,     			//设置界面
    BK_PICTURE_SET_User          ,     			//设置界面，
    
    //service
    BK_PICTURE_SERVICE_AgingTestActive     ,     //服务界面，老化按键激活背景图片   
    BK_PICTURE_SERVICE_AgingTestUnactive   ,     //服务界面，
    BK_PICTURE_SERVICE_AirLight            ,     //服务界面，
    BK_PICTURE_SERVICE_AllLog              ,     //服务界面，
    BK_PICTURE_SERVICE_Disk                ,     //服务界面，
    BK_PICTURE_SERVICE_ErrLog              ,     //服务界面，
    BK_PICTURE_SERVICE_InterActionActive   ,     //服务界面，
    BK_PICTURE_SERVICE_InterActionUnactive ,     //服务界面，
    BK_PICTURE_SERVICE_Moto                ,     //服务界面，
    BK_PICTURE_SERVICE_Others              ,     //服务界面，
    BK_PICTURE_SERVICE_ParaLog             ,     //服务界面，
    BK_PICTURE_SERVICE_Pressure            ,     //服务界面，  
    BK_PICTURE_SERVICE_StatusActive        ,     //服务界面，
    BK_PICTURE_SERVICE_StatusUnactive      ,     //服务界面，
    BK_PICTURE_SERVICE_Version             ,     //服务界面，
    BK_PICTURE_SERVICE_DataActive          ,     //服务界面，
    BK_PICTURE_SERVICE_DataUnActive        ,     //服务界面，
	BK_PICTURE_SERVICE_AllLogUnactive      ,
	BK_PICTURE_SERVICE_ErrLogUnactive      ,
	BK_PICTURE_SERVICE_ParaLogUnactive     ,
	
    //
    BK_PICTURE_SERVICE_Err                 ,     //服务界面，   
    BK_PICTURE_SERVICE_Ok                  ,     //服务界面，
    BK_PICTURE_SERVICE_SelfCheck           ,     //服务界面，
	
	//ICON图标
	ICON_START_INDEX,							//这个为ICON图标加载的起始索引号
	
	ICON_ANALYSIS_BTN,							//样本分析点击图标
	ICON_ANALYSIS_BTN_UN,						//样本分析未点击图标
	ICON_LIST_BTN,								//列表回顾点击图标
	ICON_LIST_BTN_UN,							//列表回顾未点击图标
	ICON_ERR_YUAN_BTN,							//错误提示圆圈图标
	ICON_ERR_SJ_BTN,							//错误提示三角图标
	ICON_MENU_BTN,								//菜单点击图标
	ICON_MENU_BTN_UN,							//菜单未点击图标
	ICON_MENU_ANALYSIS_BTN,						//菜单界面内样本分析图标
	ICON_MENU_LIST_BTN,							//菜单界面内列表回顾图标
	ICON_MENU_LOGOUT_BTN,						//菜单界面内注销图标
	ICON_MENU_OFF_BTN,							//菜单界面内关机图标
	ICON_MENU_QC_BTN,							//菜单界面内质控图标
	ICON_MENU_SERVICE_BTN,						//菜单界面内服务图标
	ICON_MENU_SET_BTN,							//菜单界面内设置图标
	ICON_MENU_USER_BTN,							//菜单界面内用户头像图标
	ICON_XJT_BTN,								//下拉箭头图标
	
	ICON_END_INDEX,								//这个为ICON图标加载的结束索引号

}BK_PICTURE_e;




/*
*   GIF 类型
*/
typedef enum {
    GIF_INPUT_GUIDE         = 0,
    GIF_RE_INPUT_GUIDE,
    GIF_OK_OUTPUT_GUIDE,
    GIF_FAIL_OUTPUT_GUIDE,    //GIF包括两部分，拿出血细胞模块gif和计数失败静态部分
    GIF_FAIL_OUTPUT_GUIDE2,   //只GIF包括，拿出血细胞模块gif（计数失败静态部分，单独显示）
    GIF_OPERATE_GUIDE,
    
} GIF_Index_e;


/*
*   BK文件存储的位置
*/
typedef enum{
    BK_SDRAM    = 0, 
    BK_FILE     = 1,
    BK_END      = 2
    
} BK_e;





void SIF_Font_Init(void);
FeedBack_e LoadFile_To_Buffer(const char* pacFileName, uint8_t **pBuffer, uint32_t ulMaxLen);
void LoadBMP_To_SDRAM(BK_PICTURE_e eBkPicture);
void LoadPartBMP_To_SDRAM(void);
void LoadAllICONToSDRAM(void);
//
void Show_SDRAM_BMP(BK_PICTURE_e eBkPicture, uint16_t usX, uint16_t usY);
void Show_File_BMP(BK_PICTURE_e eBkPicture, uint16_t usX, uint16_t usY);
void Show_BK_BMP(BK_e eBK, BK_PICTURE_e eBkPicture, uint16_t usX, uint16_t usY);

//
int8_t Show_GIF(GIF_Index_e eGIF, uint16_t usX, uint16_t usY);












  
#ifdef __cplusplus
}
#endif

#endif //__UI_FILES_H__




