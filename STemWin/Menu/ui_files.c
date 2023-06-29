#include "msg_def.h"
#include "ui_msg.h"
#include "main.h"
#include "cmsis_os2.h"
#include "WM.h"
#include "cx_menu.h"
#include "Public_menuDLG.h"
#include "file_operate.h"
#include "ui_files.h"
#include "GUI.h"
#include "GUI_Type.h"
#include "bc.h"
#include "bsp_motor.h"
#include "bsp_outin.h"
#include "bsp_beep.h"
#include "FreeRTOS.h"
#include "task.h"
#include "xpt2046.h"
#include "Common.h"

#if USE_RL_FLASH_FS
#include "rl_fs.h"
#include "rl_fs_lib.h"   
#else
    #include "ff.h"
    #include "fatfs.h"
#endif

GUI_FONT SIF_HZ12_SONGTI;   //12号宋体
GUI_FONT SIF_HZ16_SONGTI;   //16号宋体
GUI_FONT SIF_HZ24_SONGTI;   //24号宋体

const char gc_ucaPicture_PowerOnOff[COMPANY_END][20] = {"CC_PowerOnOff.bmp", "YL1_PowerOnOff.bmp", "YL2_PowerOnOff.bmp" , "YL3_PowerOnOff.bmp","YL4_PowerOnOff.bmp", "YL5_PowerOnOff.bmp", "YL6_PowerOnOff.bmp", "YL7_PowerOnOff.bmp"};
const char gc_ucaPicture_Login[COMPANY_END][20] = {"CC_Login.bmp", "YL1_Login.bmp", "YL2_Login.bmp" , "YL3_Login.bmp","YL4_Login.bmp", "YL5_Login.bmp", "YL6_Login.bmp", "YL7_Login.bmp"};
const char gc_ucaPicture_SelfCheck[COMPANY_END][20] = {"CC_SelfCheck.bmp", "YL1_SelfCheck.bmp", "YL2_SelfCheck.bmp" , "YL3_SelfCheck.bmp","YL4_SelfCheck.bmp", "YL5_SelfCheck.bmp", "YL6_SelfCheck.bmp", "YL7_SelfCheck.bmp"};




/*
* 获取背景bmp图片的路径和名称
*/
static void BK_BMP_FileNamePath(BK_PICTURE_e eBK_BMP, char *pucFileName)
{
    //格式, 由于部分图片有文字，所以这部分图片，需要区分中英文，加载对应的图片
    char caFileNameFormat[30] = {0};
	
    if(LANGUAGE_ENGLISH == MachInfo.systemSet.eLanguage)
    {
        strcpy(caFileNameFormat, "%s%sen_%s");
    }else{
        strcpy(caFileNameFormat, "%s%s%s");
    }
    
    //
    switch(eBK_BMP)
        {
            /******* Analysis ************/
            case BK_PICTURE_ANALYSIS_WARN_BLUE:
            {
                //按键背景（空），用于一级菜单按键背景
                sprintf(pucFileName, "%s%s%s", DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, PICTURE_ANALYSIS_WARN_BLUE);
            }
            break;
            case BK_PICTURE_ANALYSIS_WANR_WHITE:
            {
                //按键背景（空），用于一级菜单按键背景
                sprintf(pucFileName, "%s%s%s", DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, PICTURE_ANALYSIS_WARN_WHILE);
            }
            break;
            case BK_PICTURE_ANALYSIS_COUNT_FAIL:
            {
                //计数失败，静态界面部分的背景
                sprintf(pucFileName, caFileNameFormat, DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, ANALYSIS_PICTURE_BMP_COUNT_FAIL);
            }
            break;        
            
            /******* others ************/
            case BK_PICTURE_OTHERS_IconBkSmall:
            {
                //按键背景（空），用于一级菜单按键背景
                sprintf(pucFileName, "%s%s%s", DRV_PATH, OTHERS_PICTURE_DIR_PATH, PICTURE_OTHERS_IconBkSmall);
            }
            break;
            case BK_PICTURE_OTHERS_IconBkBig:
            {
                //按键背景（空），用于一级菜单按键背景
                sprintf(pucFileName, "%s%s%s", DRV_PATH, OTHERS_PICTURE_DIR_PATH, PICTURE_OTHERS_IconBkBig);
            }
            break;
            case BK_PICTURE_OTHERS_LOGIN:
            {
                //登录
				switch(MachInfo.companyInfo.company){
					case COMPANY_CREATE_CARE:
					{
						//创怀区分中英文登录界面
						sprintf(pucFileName, caFileNameFormat, DRV_PATH, OTHERS_PICTURE_DIR_PATH, gc_ucaPicture_Login[MachInfo.companyInfo.company]);
					}break;
					
					default :
					{
						sprintf(pucFileName, "%s%s%s", DRV_PATH, OTHERS_PICTURE_DIR_PATH, gc_ucaPicture_Login[MachInfo.companyInfo.company]);
					}break;
				}
            }
            break;
            case BK_PICTURE_OTHERS_SELF_CHECK:
            {
                //开机自检
                sprintf(pucFileName, "%s%s%s", DRV_PATH, OTHERS_PICTURE_DIR_PATH, gc_ucaPicture_SelfCheck[MachInfo.companyInfo.company]);
            }
            break;   
            case BK_PICTURE_OTHERS_POWER_ON_OFF:
            {
				//关机
				switch(MachInfo.companyInfo.company){
					case COMPANY_CREATE_CARE:
					{
						//创怀区分中英文关机界面
						sprintf(pucFileName, caFileNameFormat, DRV_PATH, OTHERS_PICTURE_DIR_PATH, gc_ucaPicture_PowerOnOff[MachInfo.companyInfo.company]);
					}break;
					
					default :
					{
						sprintf(pucFileName, "%s%s%s", DRV_PATH, OTHERS_PICTURE_DIR_PATH, gc_ucaPicture_PowerOnOff[MachInfo.companyInfo.company]);
					}break;
				}
            }
            break;
            /******* analysis ************/  
            case BK_PICTURE_COUNTING:
            {
                 //样本正在分析，背景图片
                sprintf(pucFileName, caFileNameFormat, DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, ANALYSIS_PICTURE_BMP_COUNTING);
            }
            break;
            case BK_PICTURE_IF_RE_COUNT:
            {
                //是否重新测试，背景图片
                sprintf(pucFileName, caFileNameFormat, DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, ANALYSIS_PICTURE_BMP_RE_COUNT);
            }
            break;   
            case BK_PICTURE_N_SEC_OUTIN_IN:
            {
                //N秒后即将进仓，背景图片
                sprintf(pucFileName, caFileNameFormat, DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, ANALYSIS_PICTURE_BMP_N_SEC_OUTIN_IN);
            }
            break;  
            case BK_PICTURE_PRINTER_NO_PAPER:
            {
                //打印机缺纸
                sprintf(pucFileName, caFileNameFormat, DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, PICTURE_BMP_PRITER_NO_PAPER);
            }
            break;  
            case BK_PICTURE_PRINTER_CONN_ERROR:
            {
                //打印机连接错误
                sprintf(pucFileName, caFileNameFormat, DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, PICTURE_BMP_PRITER_CONN_ERROR);
            }
            break;  
            case BK_PICTURE_PRINTER_TAKEOUT_CELL:
            {
                //请取出血细胞检测模块
                //sprintf(acFileName, "%s%s%s", USERPath, ANALYSIS_PICTURE_DIR_PATH, ANALYSIS_PICTURE_BMP_TAKEOUT_CELL);
            }
            break; 
            case BK_PICTURE_PRINTER_CONFIRM_TAKEOUT:
            {
                //请确认检测模块是否取出
                sprintf(pucFileName, caFileNameFormat, DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, ANALYSIS_PICTURE_BMP_CONFIRM_TAKEOUT);
            }
            break;    
            
            /******* qc ************/
            case BK_PICTURE_QC_SET:
            {
                //质控设置
                sprintf(pucFileName, "%s%s%s", DRV_PATH, QC_PICTURE_DIR_PATH, PICTURE_QC_SET);
            }
            break;  
            case BK_PICTURE_QC_ANALYSIS:
            {
                //质控分析
                sprintf(pucFileName, "%s%s%s", DRV_PATH, QC_PICTURE_DIR_PATH, PICTURE_QC_ANALYSIS);
            }
            break;  
            case BK_PICTURE_QC_LIST:
            {
                //质控列表
                sprintf(pucFileName, "%s%s%s", DRV_PATH, QC_PICTURE_DIR_PATH, PICTURE_QC_LIST);
            }
            break;  
            
            /******* set ************/
            case BK_PICTURE_SET_ChangeLogo:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_ChangeLogo);
            }
            break;
            case BK_PICTURE_SET_FuZhu:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_FuZhu);
            }
            break;
            case BK_PICTURE_SET_FuZhuUnactive:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_FuZhuUnactive);
            }
            break;   
            case BK_PICTURE_SET_JiaoZhun:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_JiaoZhen);
            }
            break;
            case BK_PICTURE_SET_JiaoZhunGray:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_JiaoZhunGray);
            }
            break;
            case BK_PICTURE_SET_LabInfo:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_LabInfo);
            }
            break;
            case BK_PICTURE_SET_Para:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_Para);
            }
            break;
            case BK_PICTURE_SET_ParaGray:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_ParaGray);
            }
            break;
            case BK_PICTURE_SET_Printer:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_Printer);
            }
            break;
            case BK_PICTURE_SET_SwitchClose:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_SwitchClose);
            }
            break;
            case BK_PICTURE_SET_SwitchOpen:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_SwitchOpen);
            }
            break;
            case BK_PICTURE_SET_Sys:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_Sys);
            }
            break;
            case BK_PICTURE_SET_Test:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_Test);
            }
            break;
            case BK_PICTURE_SET_TestGray:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_TestGray);
            }
            break;
            case BK_PICTURE_SET_User:
            {
                //set 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SET_PICTURE_DIR_PATH, PICTURE_SET_User);
            }
            break; 
            
            /******* service ************/
            case BK_PICTURE_SERVICE_AgingTestActive:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_AgingTestActive);
            }
            break;
            case BK_PICTURE_SERVICE_AgingTestUnactive:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_AgingTestUnactive);
            }
            break;            
            case BK_PICTURE_SERVICE_AirLight:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_AirLight);
            }
            break;  
            case BK_PICTURE_SERVICE_AllLog:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_AllLog);
            }
            break;  
			case BK_PICTURE_SERVICE_AllLogUnactive:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_AllLogUnactive);
            }
            break;
//            case BK_PICTURE_SERVICE_Data:
//            {
//                //service 
//                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_Data);
//            }
//            break;  
            case BK_PICTURE_SERVICE_Disk:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_Disk);
            }
            break;  
            case BK_PICTURE_SERVICE_Err:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_Err);
            }
            break;  
            case BK_PICTURE_SERVICE_ErrLog:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_ErrLog);
            }
            break;
			case BK_PICTURE_SERVICE_ErrLogUnactive:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_ErrLogUnactive);
            }
            break;
            case BK_PICTURE_SERVICE_InterActionActive:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_InterActionActive);
            }
            break;  
            case BK_PICTURE_SERVICE_InterActionUnactive:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_InterActionUnactive);
            }
            break;  
            case BK_PICTURE_SERVICE_Moto:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_Moto);
            }
            break;  
            case BK_PICTURE_SERVICE_Ok:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_Ok);
            }
            break;  
            case BK_PICTURE_SERVICE_Others:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_Others);
            }
            break;  
            case BK_PICTURE_SERVICE_ParaLog:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_ParaLog);
            }
            break;  
			case BK_PICTURE_SERVICE_ParaLogUnactive:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_ParaLogUnactive);
            }
            break; 
            case BK_PICTURE_SERVICE_Pressure:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_Pressure);
            }
            break;  
            case BK_PICTURE_SERVICE_SelfCheck:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_SelfCheck);
            }
            break;  
            case BK_PICTURE_SERVICE_StatusActive:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_StatusActive);
            }
            break;  
            case BK_PICTURE_SERVICE_StatusUnactive:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_StatusUnactive);
            }
            break;  
            case BK_PICTURE_SERVICE_Version:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_Version);
            }
            break;   
            case BK_PICTURE_SERVICE_DataActive:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_DataActive);
            }
            break;   
            case BK_PICTURE_SERVICE_DataUnActive:
            {
                //service 
                sprintf(pucFileName, "%s%s%s", DRV_PATH, SERVICE_PICTURE_DIR_PATH, PICTURE_SERVICE_DataUnActive);
            }
            break; 
			
			//===ICON===
			//样本分析点击图标
			case ICON_ANALYSIS_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\analysis-btn.bmp");
			}break;
			
			//样本分析未点击图标
			case ICON_ANALYSIS_BTN_UN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\analysis-btn-un.bmp");
			}break;
			
			//列表回顾点击图标
			case ICON_LIST_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\list-btn.bmp");
			}break;		
			
			//列表回顾未点击图标
			case ICON_LIST_BTN_UN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\list-btn-un.bmp");
			}break;			
			
			//错误提示圆圈图标
			case ICON_ERR_YUAN_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\err_tip.bmp");
			}break;
			
			//错误提示三角图标
			case ICON_ERR_SJ_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\errPage_tip.bmp");
			}break;
			
			//菜单点击图标
			case ICON_MENU_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\menu-btn.bmp");
			}break;
				
			//菜单未点击图标
			case ICON_MENU_BTN_UN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\menu-btn-un.bmp");
			}break;	
			
			//菜单界面内样本分析图标
			case ICON_MENU_ANALYSIS_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\menu-analysis.bmp");
			}break;

			//菜单界面内列表回顾图标
			case ICON_MENU_LIST_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\menu-list.bmp");
			}break;

			//菜单界面内注销图标
			case ICON_MENU_LOGOUT_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\menu-logout.bmp");
			}break;

			//菜单界面内关机图标
			case ICON_MENU_OFF_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\menu-powerOff.bmp");
			}break;

			//菜单界面内质控图标
			case ICON_MENU_QC_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\menu-qc.bmp");
			}break;

			//菜单界面内服务图标
			case ICON_MENU_SERVICE_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\menu-service.bmp");
			}break;

			//菜单界面内设置图标
			case ICON_MENU_SET_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\menu-set.bmp");
			}break;

			//菜单界面内用户头像图标
			case ICON_MENU_USER_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\menu-user.bmp");
			}break;			
			
			//下拉箭头图标
			case ICON_XJT_BTN:
			{
				sprintf(pucFileName, "N0:\\Picture\\ICON\\XJT.bmp");
			}break;
			
			
            default:
            {
                return;
            }
        }
}



/*
*   显示背景图片,  显示BMP图片（申请GUI显存，从文件中加载，显示，释放显存）
*/
void Show_File_BMP(BK_PICTURE_e eBkPicture, uint16_t usX, uint16_t usY)
{
    FILE *ptFile;

    GUI_HMEM hMem;
    uint32_t ulFileSize = 0;
    char *pacBuffer;
    char acFileName[128] = {0};
    
    //挂载文件系统，若挂载失败，则直接退出即可
    if(FS_Mount(FS_MOUNT_NOT_LOCK) != FEED_BACK_SUCCESS){
		return;
	}
        
    //获取文件名及路径
    BK_BMP_FileNamePath(eBkPicture, acFileName);
    
    //打开文件
    ptFile = fopen(acFileName, "r");
    if(NULL == ptFile)
    {
        LOG_Error("Open %s Failure", acFileName);
		
		//卸载文件系统
		FS_Unmount();
		
        return;
    }

    //文件大小
    fseek(ptFile, 0, SEEK_END);
    ulFileSize = ftell(ptFile);
    fseek(ptFile, 0, SEEK_SET);
    
    //ui分配动态内存
    hMem = GUI_ALLOC_AllocZero(ulFileSize);
    pacBuffer =  GUI_ALLOC_h2p(hMem);
    
    //读取文件到动态内存    
	fread(pacBuffer, sizeof(uint8_t), ulFileSize, ptFile);
	if (ferror(ptFile))
	{
        fclose(ptFile);
        LOG_Error(" Read %s Failure", acFileName);
	}
    
    //背景图片
    GUI_BMP_Draw(pacBuffer, usX, usY);
    
    //释放内存
    GUI_ALLOC_Free(hMem);
    
    fclose(ptFile);
	
    //卸载文件系统
	FS_Unmount();
}





/*
*   显示背景BMP, eBkPicture：图片index
*/
void Show_BK_BMP(BK_e eBK, BK_PICTURE_e eBkPicture, uint16_t usX, uint16_t usY)
{
    if(BK_SDRAM == eBK) //从SDRAM中显示
    {
        Show_SDRAM_BMP(eBkPicture, usX, usY);
    }else if(BK_FILE == eBK){   //读取文件并显示
        Show_File_BMP(eBkPicture, usX, usY);
    }else{
        LOG_Error("Unkonwn BK");
    }
}





/*
*   显示GIF，
返回-1表示文件异常，返回0表示计数池检测逻辑退出，返回1表示进仓按键触发退出，返回2表示计数按键触发退出
*/
int8_t Show_GIF(GIF_Index_e eGIF, uint16_t usX, uint16_t usY)
{
    extern __IO OutIn_Module_Position_e g_eOutIn_Key;
    extern __IO uint8_t g_ucOutInKey;
    extern __IO uint8_t g_Count_Key;
    extern MachInfo_s MachInfo;
    int8_t ucRet = 0;
    FILE *ptFile;
    
    //按键（计数、进出仓）状态，通过判断按键状态的改变，从而退出GIF，（还有触摸），每帧GIF刷新耗时越快，效果越好
    uint8_t ucOutInKey = g_ucOutInKey;
    uint8_t ucCountKey = g_Count_Key;
    
    Bool eFlag = FALSE;
    uint16_t i = 0, usBeepNum = 0;
    uint32_t ulT1 = 0, ulT2 = 0, ulFileSize = 0, ulStartTicks = 0, ulEndTicks = 0, ulIndex = 0;;
    GUI_HMEM hMem;
    char *pacBuffer;
    GUI_GIF_INFO tGIFInfo;
    GUI_GIF_IMAGE_INFO tGIFImageInfo;
    
    extern __IO osSemaphoreId_t xSema_OutIn_Key;
    extern __IO osSemaphoreId_t xSema_Count_Key;
    uint32_t ulDelay = 0;
    
    char caFileName[30] = {0};
    char caFileNameFormat[30] = {0};
	
	
	//挂载文件系统，若挂载失败，则直接退出程序
    if(FS_Mount(FS_MOUNT_NOT_LOCK) != FEED_BACK_SUCCESS){
		return -1;
	}
    
    //获取文件名  格式
    memset(caFileNameFormat, 0, sizeof(caFileNameFormat));
    if(LANGUAGE_ENGLISH == MachInfo.systemSet.eLanguage)
    {
        strcpy(caFileNameFormat, "%s%sen_%s");
    }else{
        strcpy(caFileNameFormat, "%s%s%s");
    }
    
    //获取GIF动画文件名
    switch(eGIF)
    {
        case GIF_INPUT_GUIDE:
        case GIF_RE_INPUT_GUIDE:
        {
            sprintf(caFileName, caFileNameFormat, DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, PICTURE_GIF_INPUT);
        }
        break;
        case GIF_OK_OUTPUT_GUIDE:
        {
            //蜂鸣器开关次数， 6（开3次，关3次）
            usBeepNum = 6;
            ulStartTicks = HAL_GetTick();
            sprintf(caFileName, caFileNameFormat, DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, PICTURE_GIF_OK_OUTPUT);
        }
        break;
        case GIF_FAIL_OUTPUT_GUIDE:
        {
        }
        break;    
        case GIF_FAIL_OUTPUT_GUIDE2:
        {
            //蜂鸣器开关次数， 10（开5次，关5次）
            usBeepNum = 10;
            ulStartTicks = HAL_GetTick();
            //GIF包括两部分，拿出血细胞模块gif和计数失败静态部分
            sprintf(caFileName, caFileNameFormat, DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, PICTURE_GIF_FAIL_OUTPUT2);  
        }
        break;          
        case GIF_OPERATE_GUIDE:
        {
            sprintf(caFileName, caFileNameFormat, DRV_PATH, ANALYSIS_PICTURE_DIR_PATH, PICTURE_GIF_OPERATE); 
        }
        break;  
        default:
			FS_Unmount();
            return -1;
    }

    
    //osMutexAcquire(xMutex_Com, 0);
    ptFile = fopen(caFileName, "r");
    if(ptFile == NULL)
    {
        LOG_Error("open %s failure", caFileName);
		FS_Unmount();
        return -1;
    }
    fseek(ptFile, 0, SEEK_END);
    ulFileSize = ftell(ptFile);
    fseek(ptFile, 0, SEEK_SET);
	
	//这里提前开启计数池检测灯
	COUNT_CELL_SW_ON;

    //申请内存并清零
    hMem = GUI_ALLOC_AllocZero(ulFileSize);
    //将内存句柄装换成指针类型
    pacBuffer = GUI_ALLOC_h2p(hMem);
    
    //读取文件到动态内存    
    ulT1 = HAL_GetTick();
	fread((void*)pacBuffer, sizeof(uint8_t), ulFileSize, ptFile);
    ulT2 = HAL_GetTick();
    
	if (ferror(ptFile))
	{
        LOG_Error(" Read %s Failure", caFileName);
        GUI_ALLOC_Free(hMem);
        fclose(ptFile);
        FS_Unmount();
		
		//关闭计数池检测灯
		COUNT_CELL_SW_OFF;
		return -1;
	}
    
    //获取GIF图片信息
    GUI_GIF_GetInfo(pacBuffer, ulFileSize, &tGIFInfo);
    while(1)
    {
        if(i < tGIFInfo.NumImages)
        {
            GUI_GIF_GetImageInfo(pacBuffer, ulFileSize, &tGIFImageInfo, i);
            ulDelay = tGIFImageInfo.Delay *10; //获取gif图片的延时单位为：10ms
            if(ulDelay == 0)
            {
            }else{
               //使用GIF自带延时，（最好使用GIF自带延时）
               ulT1 = HAL_GetTick();
               //一共75张，刷新第一张耗时1077ms，前面20张每张大概200ms左右，之后的大部分位几ms到几十ms
               GUI_GIF_DrawSub(pacBuffer, ulFileSize, usX, usY, i++); 
               ulT2 = HAL_GetTick();
               
               //gif默认延时
               if(ulT2 - ulT1 < ulDelay) 
               {
                   if(PenIRQ_GetState() == PENIRQ_ACTIVE)
                   {
                        break;
                   }
                   
                    GUI_Delay(ulT2 - ulT1);
               }
               GUI_Delay(3);
               
               /* 退出GIF： 有触摸触发 */
               if(PenIRQ_GetState() == PENIRQ_ACTIVE)
               {
                    break;
               }
               
               /* 退出GIF： 进出仓 按键有触发 */
               if(ucOutInKey != g_ucOutInKey)
               {
                   ucRet = 1;
                   break;
               }
               
               /* 退出GIF：  计数 按键有触发    */
               if(ucCountKey != g_Count_Key)
               {
					ucRet = 2; 
                   break;
               }
               
               /* 退出GIF： 血细胞检测模块已取出 */
               if(eGIF == GIF_OK_OUTPUT_GUIDE || eGIF == GIF_FAIL_OUTPUT_GUIDE || eGIF == GIF_FAIL_OUTPUT_GUIDE2) 
               {
                   // 计数成功或失败后，需拿出计数池，GIF退出 
                   if(CHECK_JSC_EXIST_STATUS != JSC_EXIST_STATUS_ACTIVE) 
                   { 
                       //退出，GIF
                       break;
                   }
               }
               
                /* 退出GIF：血细胞检测模块已放入 */  
               if(eGIF == GIF_RE_INPUT_GUIDE || eGIF == GIF_INPUT_GUIDE) // 1：计数成功失败后，重新放入新的计数池，当拿出旧的计数池时，GIF退出。 2：提示放入计数池动画，检测到之后退出
               {
                    //
                   if(CHECK_JSC_EXIST_STATUS == JSC_EXIST_STATUS_ACTIVE) 
                   { 
                       //退出，GIF
                       break;
                   }
               }
            }//end Delay
            
            //
            if(GIF_OK_OUTPUT_GUIDE == eGIF || GIF_FAIL_OUTPUT_GUIDE == eGIF || GIF_FAIL_OUTPUT_GUIDE2 == eGIF)
            {
                //蜂鸣器开关，时间间隔100ms
                if(ulIndex < usBeepNum)
                {
                    if(Delay_Until(100))
                    {
                        eFlag = !eFlag;
                        Beep_Exec(eFlag);
                        ulIndex++;
                    }
                } 
                //超时判断（超时后蜂鸣器不停的响 ）
                ulEndTicks = HAL_GetTick();
                if(ulEndTicks - ulStartTicks > MachInfo.paramSet.usNsec_BeepWaring*1000) //s->ms
                {
                    //超时，则蜂鸣器不停的响
                    if(Delay_Until(100))
                    {
                        eFlag = !eFlag;
                        Beep_Exec(eFlag);
                    }
                }
            }
            //
            osDelay(1);
        }else{
            //从头开始，刷新GIF
            i = 0;
        }
    }
	
	//关闭计数池检测灯
	COUNT_CELL_SW_OFF;
    
    //确保蜂鸣器，关
    Beep_Stop();
    
    //结束,释放资源
    GUI_ALLOC_Free(hMem);
    fclose(ptFile);
    FS_Unmount();
    
    return ucRet;
}




/*
*   加载文件到缓存（SDRAM）,
*/
FeedBack_e LoadFile_To_Buffer(const char* pacFileName, uint8_t **pBuffer, uint32_t ulMaxLen)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    FILE *ptFile;
    uint32_t ulFileSize = 0;
    
	//挂载文件系统，若挂载失败，则直接锁定程序
	FS_Mount(FS_MOUNT_LOCK);
	
    __disable_irq();
	
    ptFile = fopen(pacFileName, "r");
    if(ptFile == NULL)
    {
        LOG_Error(" Open %s Failure", pacFileName);
		eFeedBack = FEED_BACK_FAIL;
        goto LoadFile_To_BufferTag;
    }
    
    fseek(ptFile, 0, SEEK_END);
    ulFileSize = ftell(ptFile);
    fseek(ptFile, 0, SEEK_SET);
    
    //检测大小，注意大小不要超过 ulMaxLen
    if(ulFileSize > ulMaxLen)
    {
        LOG_Error(" %s More Than %X", pacFileName, ulMaxLen);
		eFeedBack = FEED_BACK_FAIL;
        goto LoadFile_To_BufferTag;
    }
    
    //指定字体存储的SDRAM位置, 
    memset((void*)*pBuffer, 0, ulMaxLen);

    //读取文件到动态内存    
	fread(*pBuffer, sizeof(uint8_t), ulFileSize, ptFile);
    
	if (ferror(ptFile))
	{
        LOG_Error(" Read %s Failure", pacFileName);
		eFeedBack = FEED_BACK_FAIL;
        goto LoadFile_To_BufferTag;
	}
    
LoadFile_To_BufferTag:
    fclose(ptFile);
	
	//卸载文件系统
	FS_Unmount();
	
	__enable_irq();
    return eFeedBack;
}


/*
* 显示已加载到SDRAM中的，BMP背景图片。（该函数必须在LoadBMP_To_SDRAM之后使用）
*/
void Show_SDRAM_BMP(BK_PICTURE_e eBkPicture, uint16_t usX, uint16_t usY)
{
    switch(eBkPicture)
    {
//            case SDRAM_BMP_Frame:
//            {
//                memset(caFileName, 0, sizeof(caFileName));
//                sprintf(caFileName, "%s%s%s", DRV_PATH, OTHERS_PICTURE_DIR_PATH, PICTURE_OTHERS_FRAME_BK);
//                pBuffer = (uint8_t*)SDRAM_BK_PICTURE_FRAMEBK_ADDR;
//                ulBMP_MaxSize = SDRAM_BK_PICTURE_FRAMEBK_LEN;
//                memset((void*)SDRAM_BK_PICTURE_FRAMEBK_ADDR, 0, SDRAM_BK_PICTURE_FRAMEBK_LEN);
//            }
//            break;
        
        /***  样本分析 ***/ 
        case BK_PICTURE_ANALYSIS_WARN_BLUE:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_WARN_BLUE_ADDR, usX, usY);                               
        }
        break;
        case BK_PICTURE_ANALYSIS_WANR_WHITE:
        {                
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_WARN_WHITE_ADDR, usX, usY);               
        }
        break;
        case BK_PICTURE_ANALYSIS_COUNT_FAIL:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_COUNT_FAIL_ADDR, usX, usY);
        }
        break;
//        case BK_PICTURE_ANALYSIS_WAITING:
//        {
//            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_WAITING_ADDR, usX, usY);          
//        }
//        break;
        
        /***  其他 ***/ 
        case BK_PICTURE_OTHERS_POWER_ON_OFF:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_PWOER_ON_OFF_ADDR, usX, usY);                               
        }
        break;
        case BK_PICTURE_OTHERS_IconBkBig:
        {                
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_ICONBKBIG_ADDR, usX, usY);               
        }
        break;
        case BK_PICTURE_OTHERS_IconBkSmall:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_ICONBKSMALL_ADDR, usX, usY);          
        }
        break;
        case BK_PICTURE_OTHERS_LOGIN:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_LOGIN_ADDR, usX, usY);                           
        }
        break;
        case BK_PICTURE_OTHERS_SELF_CHECK:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_SELF_CHECK_ADDR, usX, usY);                     
        }
        break;
     
        
        /***  质控 ***/
        case BK_PICTURE_QC_SET:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_QC_SET_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_QC_ANALYSIS:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_QC_ANALYSIS_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_QC_LIST:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_QC_LIST_ADDR, usX, usY);
        }
        break;
        
        /***  设置 ***/
        case BK_PICTURE_SET_SwitchClose:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_SwitchClose_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SET_SwitchOpen:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_SwitchOpen_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SET_FuZhu:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_FuZhu_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SET_FuZhuUnactive:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_FuZhuUnactive_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SET_JiaoZhun:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_JiaoZhun_ADDR, usX, usY); 
        }
        break;
        case BK_PICTURE_SET_JiaoZhunGray:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_JiaoZhunGray_ADDR, usX, usY); 
        }
        break;
        case BK_PICTURE_SET_LabInfo:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_LabInfo_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SET_Para:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_Para_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SET_ParaGray:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_ParaGray_ADDR, usX, usY); 
        }
        break;
        case  BK_PICTURE_SET_Printer:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_Printer_ADDR, usX, usY); 
        }
        break;
        case BK_PICTURE_SET_Sys:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_Sys_ADDR, usX, usY);  
        }
        break;
        case BK_PICTURE_SET_Test:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_Test_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SET_TestGray:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_TestGray_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SET_User:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_User_ADDR, usX, usY);
        }
        break;
       
        
        /*** 服务 ***/
        case BK_PICTURE_SERVICE_AgingTestActive:
        {   
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_AgingTestActive_ADDR, usX, usY);   
        }
        break;
        case BK_PICTURE_SERVICE_AgingTestUnactive:
        { 
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_AgingTestUnactive_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_AirLight:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_AirLight_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_AllLog:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_AllLog_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_Disk:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_Disk_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_ErrLog:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_ErrLog_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_InterActionActive:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_InterActionActive_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_InterActionUnactive:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_InterActionUnactive_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_Moto:
        { 
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_Moto_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_Others:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_Others_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_ParaLog:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_ParaLog_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_Pressure:        
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_Pressure_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_StatusActive:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_StatusActive_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_StatusUnactive:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_StatusUnactive_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_Version:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_Version_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_DataActive:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_DataActive_ADDR, usX, usY);
        }
        break;
        case BK_PICTURE_SERVICE_DataUnActive:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_DataUnActive_ADDR, usX, usY);
        }
        break;      
        case BK_PICTURE_SERVICE_Err:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_Err_ADDR, usX, usY); 
        }
        break;
        case BK_PICTURE_SERVICE_Ok:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_Ok_ADDR, usX, usY); 
        }
        break;
        case BK_PICTURE_SERVICE_SelfCheck:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_SelfCheck_ADDR, usX, usY);
        }
        break;
		
		case BK_PICTURE_SERVICE_AllLogUnactive:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_AllLogUnactive_ADDR, usX, usY);
        }
        break;
		
		case BK_PICTURE_SERVICE_ErrLogUnactive:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_ErrLogUnactive_ADDR, usX, usY);
        }
        break;
		
		case BK_PICTURE_SERVICE_ParaLogUnactive:
        {
            GUI_BMP_Draw((const void*)SDRAM_BK_PICTURE_ParaLogUnactive_ADDR, usX, usY);
        }
        break;
		
		//===ICON===
		//样本分析点击图标
		case ICON_ANALYSIS_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_ANALYSIS_BTN_ADDR, usX, usY);
		}break;
		
		//样本分析未点击图标
		case ICON_ANALYSIS_BTN_UN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_ANALYSIS_BTN_UN_ADDR, usX, usY);
		}break;
		
		//列表回顾点击图标
		case ICON_LIST_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_LIST_BTN_ADDR, usX, usY);
		}break;		
		
		//列表回顾未点击图标
		case ICON_LIST_BTN_UN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_LIST_BTN_UN_ADDR, usX, usY);
		}break;			
		
		//错误提示圆圈图标
		case ICON_ERR_YUAN_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_ERR_YUAN_BTN_ADDR, usX, usY);
		}break;
		
		//错误提示三角图标
		case ICON_ERR_SJ_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_ERR_SJ_BTN_ADDR, usX, usY);
		}break;
		
		//菜单点击图标
		case ICON_MENU_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_MENU_BTN_ADDR, usX, usY);
		}break;
			
		//菜单未点击图标
		case ICON_MENU_BTN_UN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_MENU_BTN_UN_ADDR, usX, usY);
		}break;	
		
		//菜单界面内样本分析图标
		case ICON_MENU_ANALYSIS_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_MENU_ANALYSIS_BTN_ADDR, usX, usY);
		}break;

		//菜单界面内列表回顾图标
		case ICON_MENU_LIST_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_MENU_LIST_BTN_ADDR, usX, usY);
		}break;

		//菜单界面内注销图标
		case ICON_MENU_LOGOUT_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_MENU_LOGOUT_BTN_ADDR, usX, usY);
		}break;

		//菜单界面内关机图标
		case ICON_MENU_OFF_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_MENU_OFF_BTN_ADDR, usX, usY);
		}break;

		//菜单界面内质控图标
		case ICON_MENU_QC_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_MENU_QC_BTN_ADDR, usX, usY);
		}break;

		//菜单界面内服务图标
		case ICON_MENU_SERVICE_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_MENU_SERVICE_BTN_ADDR, usX, usY);
		}break;

		//菜单界面内设置图标
		case ICON_MENU_SET_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_MENU_SET_BTN_ADDR, usX, usY);
		}break;

		//菜单界面内用户头像图标
		case ICON_MENU_USER_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_MENU_USER_BTN_ADDR, usX, usY);
		}break;			
		

		//下拉箭头图标
		case ICON_XJT_BTN:
		{
			GUI_BMP_Draw((const void*)SDRAM_BK_ICON_XJT_BTN_ADDR, usX, usY);
		}break;
		
		
		
		
        default:
        {
            LOG_Error("Index Error");
            return;
        }
    }
}




/*
* 加载指定部分 BMP背景图片到SDRAM中
*/
void LoadPartBMP_To_SDRAM(void)
{
    BK_PICTURE_e eBk;
    
    //analysis
    for(eBk = BK_PICTURE_ANALYSIS_WARN_BLUE; eBk <= BK_PICTURE_ANALYSIS_COUNT_FAIL; eBk++)
    {
        LoadBMP_To_SDRAM(eBk);
    }  
    
    //others
    for(eBk = BK_PICTURE_OTHERS_IconBkSmall; eBk <= BK_PICTURE_OTHERS_SELF_CHECK; eBk++)
    {
        LoadBMP_To_SDRAM(eBk);
    }    
    
    //qc
    for(eBk = BK_PICTURE_QC_SET; eBk <= BK_PICTURE_QC_LIST; eBk++)
    {
        LoadBMP_To_SDRAM(eBk);
    }
    
    //set
    for(eBk = BK_PICTURE_SET_FuZhu; eBk <= BK_PICTURE_SET_User; eBk++)
    {
        LoadBMP_To_SDRAM(eBk);
    }
    
    //service
    for(eBk = BK_PICTURE_SERVICE_AgingTestActive; eBk <= BK_PICTURE_SERVICE_SelfCheck; eBk++)
    {
        LoadBMP_To_SDRAM(eBk);
    }
}



/***
*加载所有用到的ICON到SDRAM
***/
void LoadAllICONToSDRAM(void)
{
	uint16_t i = 0;
	
	for(i=ICON_START_INDEX+1;i<ICON_END_INDEX;i++){
		LoadBMP_To_SDRAM(i);
	}
}






/*
* 加载BMP背景图片到SDRAM中
*/
void LoadBMP_To_SDRAM(BK_PICTURE_e eBkPicture)
{
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
    uint32_t ulBMP_MaxSize = 0;
    char caFileName[FILE_NAME_MAX_LEN] = {0};
    uint8_t *pBuffer = NULL;
    
    //获取文件名及路径
     memset(caFileName, 0, sizeof(caFileName));
    BK_BMP_FileNamePath(eBkPicture, caFileName);
       
    //
    switch(eBkPicture)
    {
        
        /***  样本分析 ***/
        case BK_PICTURE_ANALYSIS_WARN_BLUE:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_WARN_BLUE_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_WARN_BLUE_LEN; 
        }
        break;
        case BK_PICTURE_ANALYSIS_WANR_WHITE:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_WARN_WHITE_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_WARN_WHITE_LEN; 
        }
        break;
        case BK_PICTURE_ANALYSIS_COUNT_FAIL:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_COUNT_FAIL_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_COUNT_FAIL_LEN;   
        }
        break;
        
        /***  其他 ***/
        case BK_PICTURE_OTHERS_IconBkBig:
        {                
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_ICONBKBIG_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_ICONBKBIG_LEN;                   
        }
        break;
        case BK_PICTURE_OTHERS_IconBkSmall:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_ICONBKSMALL_ADDR;    
            ulBMP_MaxSize = SDRAM_BK_PICTURE_ICONBKSAMLL_LEN;                
        }
        break;
        case BK_PICTURE_OTHERS_POWER_ON_OFF:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_PWOER_ON_OFF_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_PWOER_ON_OFF_LEN;                       
        }
        break;
        case BK_PICTURE_OTHERS_LOGIN:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_LOGIN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_LOGIN_LEN;                            
        }
        break;
        case BK_PICTURE_OTHERS_SELF_CHECK:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_SELF_CHECK_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_SELF_CHECK_LEN;                       
        }
        break;           
        
        
        /***  质控 ***/
        case BK_PICTURE_QC_SET:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_QC_SET_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_QC_SET_LEN; 
        }
        break;
        case BK_PICTURE_QC_ANALYSIS:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_QC_LIST_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_QC_LIST_LEN; 
        }
        break;
        case BK_PICTURE_QC_LIST:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_QC_ANALYSIS_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_QC_ANALYSIS_LEN; 
        }
        break;
        
        /***  设置 ***/
        case BK_PICTURE_SET_SwitchClose:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_SwitchClose_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_SwitchClose_LEN;  
        }
        break;
        case BK_PICTURE_SET_SwitchOpen:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_SwitchOpen_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_SwitchOpen_LEN;  
        }
        break;
        case BK_PICTURE_SET_FuZhu:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_FuZhu_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_FuZhu_LEN;  
        }
        break;
        case BK_PICTURE_SET_FuZhuUnactive:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_FuZhuUnactive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_FuZhuUnactive_LEN;  
        }
        break;
        case BK_PICTURE_SET_JiaoZhun:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_JiaoZhun_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_JiaoZhun_LEN;  
        }
        break;
        case BK_PICTURE_SET_JiaoZhunGray:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_JiaoZhunGray_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_JiaoZhunGray_LEN;  
        }
        break;
        case BK_PICTURE_SET_LabInfo:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_LabInfo_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_LabInfo_LEN;  
        }
        break;
        case BK_PICTURE_SET_Para:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_Para_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_Para_LEN;  
        }
        break;
        case BK_PICTURE_SET_ParaGray:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_ParaGray_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_ParaGray_LEN;  
        }
        break;
        case  BK_PICTURE_SET_Printer:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_Printer_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_Printer_LEN;  
        }
        break;
        case BK_PICTURE_SET_Sys:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_Sys_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_Sys_LEN;  
        }
        break;
        case BK_PICTURE_SET_Test:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_Test_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_Test_LEN;  
        }
        break;
        case BK_PICTURE_SET_TestGray:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_TestGray_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_TestGray_LEN;  
        }
        break;
        case BK_PICTURE_SET_User:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_User_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_User_LEN;  
        }
        break;
       
        
        /*** 服务 ***/
        case BK_PICTURE_SERVICE_AgingTestActive:
        {   
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_AgingTestActive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_AgingTestActive_LEN;   
        }
        break;
        case BK_PICTURE_SERVICE_AgingTestUnactive:
        { 
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_AgingTestUnactive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_AgingTestUnactive_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_AirLight:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_AirLight_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_AirLight_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_AllLog:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_AllLog_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_AllLog_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_Disk:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_Disk_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_Disk_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_ErrLog:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_ErrLog_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_ErrLog_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_InterActionActive:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_InterActionActive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_InterActionActive_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_InterActionUnactive:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_InterActionUnactive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_InterActionUnactive_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_Moto:
        { 
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_Moto_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_Moto_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_Others:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_Others_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_Others_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_ParaLog:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_ParaLog_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_ParaLog_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_Pressure:        
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_Pressure_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_Pressure_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_StatusActive:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_StatusActive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_StatusActive_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_StatusUnactive:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_StatusUnactive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_StatusUnactive_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_Version:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_Version_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_Version_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_DataActive:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_DataActive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_DataActive_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_DataUnActive:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_DataUnActive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_DataUnActive_LEN; 
        }
        break;      
        case BK_PICTURE_SERVICE_Err:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_Err_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_Err_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_Ok:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_Ok_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_Ok_LEN; 
        }
        break;
        case BK_PICTURE_SERVICE_SelfCheck:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_SelfCheck_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_SelfCheck_LEN; 
        }
        break;
		case BK_PICTURE_SERVICE_AllLogUnactive:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_AllLogUnactive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_AllLogUnactive_LEN; 
        }
        break;
		case BK_PICTURE_SERVICE_ErrLogUnactive:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_ErrLogUnactive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_ErrLogUnactive_LEN; 
        }
        break;
		case BK_PICTURE_SERVICE_ParaLogUnactive:
        {
            pBuffer = (uint8_t*)SDRAM_BK_PICTURE_ParaLogUnactive_ADDR;
            ulBMP_MaxSize = SDRAM_BK_PICTURE_ParaLogUnactive_LEN; 
        }
        break;
		
		//===ICON===
		//样本分析点击图标
		case ICON_ANALYSIS_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_ANALYSIS_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_ANALYSIS_BTN_LEN;
		}break;
		
		//样本分析未点击图标
		case ICON_ANALYSIS_BTN_UN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_ANALYSIS_BTN_UN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_ANALYSIS_BTN_UN_LEN;
		}break;
		
		//列表回顾点击图标
		case ICON_LIST_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_LIST_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_LIST_BTN_LEN;
		}break;		
		
		//列表回顾未点击图标
		case ICON_LIST_BTN_UN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_LIST_BTN_UN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_LIST_BTN_UN_LEN;
		}break;			
		
		//错误提示圆圈图标
		case ICON_ERR_YUAN_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_ERR_YUAN_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_ERR_YUAN_BTN_LEN;
		}break;
		
		//错误提示三角图标
		case ICON_ERR_SJ_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_ERR_SJ_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_ERR_SJ_BTN_LEN;
		}break;
		
		//菜单点击图标
		case ICON_MENU_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_MENU_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_MENU_BTN_LEN;
		}break;
			
		//菜单未点击图标
		case ICON_MENU_BTN_UN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_MENU_BTN_UN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_MENU_BTN_UN_LEN;
		}break;	
		
		
		//菜单界面内样本分析图标
		case ICON_MENU_ANALYSIS_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_MENU_ANALYSIS_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_MENU_ANALYSIS_BTN_LEN;
		}break;

		//菜单界面内列表回顾图标
		case ICON_MENU_LIST_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_MENU_LIST_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_MENU_LIST_BTN_LEN;
		}break;

		//菜单界面内注销图标
		case ICON_MENU_LOGOUT_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_MENU_LOGOUT_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_MENU_LOGOUT_BTN_LEN;
		}break;

		//菜单界面内关机图标
		case ICON_MENU_OFF_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_MENU_OFF_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_MENU_OFF_BTN_LEN;
		}break;

		//菜单界面内质控图标
		case ICON_MENU_QC_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_MENU_QC_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_MENU_QC_BTN_LEN;
		}break;

		//菜单界面内服务图标
		case ICON_MENU_SERVICE_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_MENU_SERVICE_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_MENU_SERVICE_BTN_LEN;
		}break;

		//菜单界面内设置图标
		case ICON_MENU_SET_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_MENU_SET_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_MENU_SET_BTN_LEN;
		}break;

		//菜单界面内用户头像图标
		case ICON_MENU_USER_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_MENU_USER_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_MENU_USER_BTN_LEN;
		}break;			
		
		//下拉箭头图标
		case ICON_XJT_BTN:
		{
			pBuffer = (uint8_t*)SDRAM_BK_ICON_XJT_BTN_ADDR;
            ulBMP_MaxSize = SDRAM_BK_ICON_XJT_BTN_LEN;
		}break;
		
		
        default:
        {
            LOG_Error("Index Error");
            return;
        }
    }
    memset((void*)pBuffer, 0, ulBMP_MaxSize); 
    eFeedBack = LoadFile_To_Buffer(caFileName, &pBuffer, ulBMP_MaxSize); 
    if(FEED_BACK_SUCCESS != eFeedBack)
    {
       LOG_Error("Load %s File Failure", caFileName);
       return;
    }  
}



/*
*   初始化SIF字体
*/
void SIF_Font_Init(void)
{
    extern GUI_FONT SIF_HZ12_SONGTI;   //12号宋体
    extern GUI_FONT SIF_HZ16_SONGTI; 
    extern GUI_FONT SIF_HZ24_SONGTI; 
    
    FeedBack_e eFeedBack = FEED_BACK_SUCCESS;
//    uint32_t ulFontMaxLen = 0;
    char caFileName[FILE_NAME_MAX_LEN] = {0};
    uint8_t *pBuffer = NULL;
	
	
	/**********************UNI2GBK*********************/
    sprintf(caFileName, "%s%s%s", DRV_PATH, FONT_DIR_PATH, FONT_UNI2GBK);
    
    pBuffer = (uint8_t*)SDRAM_UNI2GBK_ADDR;
    eFeedBack = LoadFile_To_Buffer(caFileName, &pBuffer, SDRAM_UNI2GBK_LEN);
    if(FEED_BACK_SUCCESS != eFeedBack)
    {
        LOG_Error("Load UNI2GBK Failure");
    }
    
    /**********************宋体12*********************/
    sprintf(caFileName, "%s%s%s", DRV_PATH, FONT_DIR_PATH, FONT_SIF_SONGTI_12);
    //加载sif字体文件
    
    pBuffer = (uint8_t*)SDRAM_FONT12_ADDR;
    eFeedBack = LoadFile_To_Buffer(caFileName, &pBuffer, SDRAM_FONT12_LEN);
    if(FEED_BACK_SUCCESS != eFeedBack)
    {
        LOG_Error("Load SIF File Failure");
        return;
    }
    //创建字体
    GUI_SIF_CreateFont((uint8_t*)pBuffer, &SIF_HZ12_SONGTI, GUI_SIF_TYPE_PROP);
    
    
    
    /**********************宋体16*****************/
    sprintf(caFileName, "%s%s%s", DRV_PATH, FONT_DIR_PATH, FONT_SIF_SONGTI_16);
    //加载sif字体文件
    
    pBuffer = (uint8_t*)SDRAM_FONT16_ADDR;
    eFeedBack = LoadFile_To_Buffer(caFileName, &pBuffer, SDRAM_FONT16_LEN);
    if(FEED_BACK_SUCCESS != eFeedBack)
    {
        LOG_Error("Load SIF File Failure");
        return;
    }
    //创建字体
    GUI_SIF_CreateFont((uint8_t*)pBuffer, &SIF_HZ16_SONGTI, GUI_SIF_TYPE_PROP);
    
    
    
//    /*******************宋体24*******************/
//    memset(caFileName, 0, sizeof(caFileName));
//    sprintf(caFileName, "%s%s%s", DRV_PATH, FONT_DIR_PATH, FONT_SIF_SONGTI_24);
//    //加载sif字体文件
//    pBuffer = (uint8_t*)SDRAM_FONT24_ADDR;
//    eFeedBack = LoadFile_To_Buffer(caFileName, &pBuffer, SDRAM_FONT24_LEN);
//    if(FEED_BACK_SUCCESS!= eFeedBack)
//    {
//        LOG_Error("Load SIF File Failure");
//        return;
//    }
//    //创建字体
//    GUI_SIF_CreateFont((uint8_t*)pBuffer, &SIF_HZ24_SONGTI, GUI_SIF_TYPE_PROP);
    
}







