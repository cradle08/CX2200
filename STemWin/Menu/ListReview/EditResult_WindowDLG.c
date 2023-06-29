/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.44                          *
*        Compiled Nov 10 2017, 08:53:57                              *
*        (c) 2017 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
/*
*
*   �б��ع˽����У��༭�������
*
*/
// USER END

#include "DIALOG.h"

#include "Public_menuDLG.h"
#include "bc.h"
#include "Crc16.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_EDIT_RESULT   (GUI_EDIT_RESULT_ID + 0x00)
#define ID_TEXT_0               (GUI_ID_USER + 0x01)
#define ID_TEXT_1               (GUI_ID_USER + 0x02)
#define ID_TEXT_2               (GUI_ID_USER + 0x03)
#define ID_TEXT_3               (GUI_ID_USER + 0x04)
#define ID_EDIT_WBC             (GUI_ID_USER + 0x05)
#define ID_EDIT_GRAN            (GUI_ID_USER + 0x06)
#define ID_EDIT_MID             (GUI_ID_USER + 0x07)
#define ID_EDIT_LYM             (GUI_ID_USER + 0x08)    
#define ID_TEXT_4               (GUI_ID_USER + 0x09)
#define ID_TEXT_5               (GUI_ID_USER + 0x0A)
#define ID_TEXT_6               (GUI_ID_USER + 0x0B)
#define ID_TEXT_7               (GUI_ID_USER + 0x0C)
#define ID_BUTTON_CANCEL        (GUI_ID_USER + 0x0D)
#define ID_BUTTON_SURE          (GUI_ID_USER + 0x0E)
#define ID_TEXT_INPUT_ERROR     (GUI_ID_USER + 0x0F)
#define ID_EDIT_HGB             (GUI_ID_USER + 0x10)
#define ID_TEXT_8               (GUI_ID_USER + 0x11)
#define ID_TEXT_9               (GUI_ID_USER + 0x12)

// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
#define ER_X_MOVE_LEN   0
#define ER_Y_MOVE_LEN   60
static uint8_t g_sER_MoveFlag = 0;

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO EditResult_aDialogCreate[] = {
  { WINDOW_CreateIndirect, "", ID_WINDOW_EDIT_RESULT,       140, 60, 500, 390, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "WBC",      ID_TEXT_0,             100, 50, 80, 40, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Gran%",    ID_TEXT_1,             100, 100, 80, 40, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Mid%",     ID_TEXT_2,             100, 150, 80, 40, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Lym%",     ID_TEXT_3,             100, 200, 80, 40, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "HGB",      ID_TEXT_8,             100, 250, 80, 40, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "",         ID_EDIT_WBC,           200, 50, 100, 40, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "",         ID_EDIT_GRAN,          200, 100, 100, 40, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "",         ID_EDIT_MID,           200, 150, 100, 40, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "",         ID_EDIT_LYM,           200, 200, 100, 40, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "",         ID_EDIT_HGB,           200, 250, 100, 40, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "10^9/L",   ID_TEXT_4,             310, 50, 80, 40, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "%",        ID_TEXT_5,             310, 100, 80, 40, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "%",        ID_TEXT_6,             310, 150, 80, 40, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "%",        ID_TEXT_7,             310, 200, 80, 40, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "g/L",      ID_TEXT_9,             310, 250, 80, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "",       ID_BUTTON_CANCEL,      0, 350, 250, 40, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "",       ID_BUTTON_SURE,        250, 350, 250, 40, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "",         ID_TEXT_INPUT_ERROR,   200, 300, 120, 40, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END





/*
*  ErrorPrompt  btn callback
*/
static void EditResult_Btn_Callback(WM_MESSAGE* pMsg)
{
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
    {
        GUI_RECT Rect;
        WM_GetClientRect(&Rect);

        if (BUTTON_IsPressed(pMsg->hWin)) {
            //draw frame
            GUI_SetColor(MachInfo.companyInfo.skin.highlightKJSelect);
            GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);

            // display char
            GUI_SetColor(MachInfo.companyInfo.skin.highlightKJSelectFont);
            GUI_SetBkColor(MachInfo.companyInfo.skin.highlightKJSelect);
			
            if(ID_BUTTON_SURE == WM_GetId(pMsg->hWin))
            {
                //ȷ��
                GUI_DispStringInRect(g_ucaLng_Sure[MachInfo.systemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }else if(ID_BUTTON_CANCEL == WM_GetId(pMsg->hWin)){
            
                //ȡ��
                GUI_DispStringInRect(g_ucaLng_Cancel[MachInfo.systemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }
        }
        else {
            if(ID_BUTTON_SURE == WM_GetId(pMsg->hWin))
            {
				// draw frame
				GUI_SetColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
				GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);

				// display char
				GUI_SetColor(MachInfo.companyInfo.skin.highlightKHNotSelectFont);
				GUI_SetBkColor(MachInfo.companyInfo.skin.highlightKJNotSelect);
				
                //ȷ��
                GUI_DispStringInRect(g_ucaLng_Sure[MachInfo.systemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }else if(ID_BUTTON_CANCEL == WM_GetId(pMsg->hWin)){
				// draw frame
				GUI_SetColor(GUI_LIGHTGRAY);
				GUI_FillRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1);

				// display char
				GUI_SetColor(GUI_BTN_RELEASE_CHAR_COLOR);
				GUI_SetBkColor(GUI_LIGHTGRAY);
				
                //ȡ��
                GUI_DispStringInRect(g_ucaLng_Cancel[MachInfo.systemSet.eLanguage], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }
        }
    }
    break;
    default:
        BUTTON_Callback(pMsg);
        break;
    }
}


/*
*   ��ʼ��
*/
static void EditResult_Msg_Init(WM_MESSAGE* pMsg)
{
    extern uint8_t g_ucaListReview_DataTable[LIST_REVIEW_ROW_NUM];
    extern __IO MachRunPara_s MachRunPara;
    
    WM_HWIN hItem;
    char buffer[10] = {0};
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    //
    // Initialization of 'Gran%'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
	switch(MachInfo.companyInfo.company){
		case COMPANY_JIN_RUI:
		{
			//����
			TEXT_SetText(hItem,"Neu%");
		}break;
		
		default :
		{
			TEXT_SetText(hItem,"Gran%");
		}break;
	}
    //
    // Initialization of 'Mid%'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    //
    // Initialization of 'Lym%'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    //
    // Initialization of 'wbc_edit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_WBC);
    EDIT_SetMaxLen(hItem, 6);
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%.2f", MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_WBC]);
    EDIT_SetText(hItem, buffer);
    EDIT_EnableBlink(hItem, 500, 1);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    WM_SetCallback(hItem, Public_Edit_Callback);
    //
    // Initialization of 'Gran_edit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAN);
    EDIT_SetMaxLen(hItem, 5);
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%.2f", MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_GranB]); 
    EDIT_SetText(hItem, buffer);
    EDIT_EnableBlink(hItem, 500, 1);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    WM_SetCallback(hItem, Public_Edit_Callback);
    //
    // Initialization of 'Mid_edit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MID);
    EDIT_SetMaxLen(hItem, 5);
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%.2f", MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_MidB]); 
    EDIT_SetText(hItem, buffer);
    EDIT_EnableBlink(hItem, 500, 1);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    WM_SetCallback(hItem, Public_Edit_Callback);
    //
    // Initialization of 'Lym_edit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LYM);
    EDIT_SetMaxLen(hItem, 5);
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%.2f", MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_LymB]); 
    EDIT_SetText(hItem, buffer);
    EDIT_EnableBlink(hItem, 500, 1);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    WM_SetCallback(hItem, Public_Edit_Callback);
    
    //
    // Initialization of 'HGB_edit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB);
    EDIT_SetMaxLen(hItem, 6);
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%.2f", MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_HGB]); 
    EDIT_SetText(hItem, buffer);
    EDIT_EnableBlink(hItem, 500, 1);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    WM_SetCallback(hItem, Public_Edit_Callback);
    
    //sure btn
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SURE);
    WM_SetCallback(hItem, EditResult_Btn_Callback);
    //WM_InvalidateWindow(hItem);
    
    //cancel btn
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CANCEL);
    WM_SetCallback(hItem, EditResult_Btn_Callback);
    //WM_InvalidateWindow(hItem);
}
    

/*
*   �����޸ĺ�����ݣ���������Ϣ����ˣ������ļ�
*/
#define EDIT_RESULT_PERCENT_MAX_LEN     6
#define EDIT_RESULT_RESULT_MAX_LEN      7
FeedBack_e CheckAndUpdate_Result(WM_MESSAGE *pMsg)
{
    extern __IO MachRunPara_s MachRunPara;
    extern uint8_t g_ucaListReview_DataTable[LIST_REVIEW_ROW_NUM];
    extern uint8_t g_ucListView_DataIndex;
        
    WM_HWIN hItem;
    float  fWBC = 0, fGranB = 0, fMidB = 0, fLymB = 0, fHGB = 0;
    char buffer[10] = {0};
    WBCHGB_Rst_e eDataType = WBCHGB_RST_WBC;
    
    //wbc
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_WBC);
    memset(buffer, 0, sizeof(buffer));
    EDIT_GetText(hItem, buffer, EDIT_RESULT_RESULT_MAX_LEN); //7����Init��������������볤��
    fWBC = (float)atof(buffer);
    
    //Gran%
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAN);
    memset(buffer, 0, sizeof(buffer));
    EDIT_GetText(hItem, buffer, EDIT_RESULT_PERCENT_MAX_LEN); //6����Init��������������볤��
    fGranB = (float)atof(buffer);
    
    //fMid%
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_MID);
    memset(buffer, 0, sizeof(buffer));
    EDIT_GetText(hItem, buffer, EDIT_RESULT_PERCENT_MAX_LEN); //6����Init��������������볤��
    fMidB = (float)atof(buffer);
    
    //fLym%
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_LYM);
    memset(buffer, 0, sizeof(buffer));
    EDIT_GetText(hItem, buffer, EDIT_RESULT_PERCENT_MAX_LEN); //6����Init��������������볤��
    fLymB = (float)atof(buffer);
    
    //fHGB
    hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB);
    memset(buffer, 0, sizeof(buffer));
    EDIT_GetText(hItem, buffer, EDIT_RESULT_RESULT_MAX_LEN); //7����Init��������������볤��
    fHGB = (float)atof(buffer);
    
    //check input param
     if((fGranB+fMidB+fLymB) < 99.99999f || (fGranB+fMidB+fLymB) > 100.00001f)
     {
        return FEED_BACK_ERROR;
     }
    
    //��ֵ�滻��ǰ�������
    MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_WBC]   = fWBC;
    MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_GranB] = fGranB;
    MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_MidB]  = fMidB;
    MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_LymB]  = fLymB;
    MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_GranJ] = (float)(fGranB*fWBC/100.0f);
    MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_MidJ]  = (float)(fMidB*fWBC/100.0f);
    MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_LymJ]  = (float)(fLymB*fWBC/100.0f);
    MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[WBCHGB_RST_HGB]   = fHGB;

    //�޸Ľ�����ݺ�Ҳ��Ҫ���¶�Ӧ�ĸ澯��־
    for(eDataType = WBCHGB_RST_WBC; eDataType < WBCHGB_RST_END; eDataType++)
    {
        //PF_NORMAL 0
        MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].ucaWBCHGB_RstAlert[eDataType] = \
            WBCHGB_Alert_Flag(0, MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].eReferGroup, \
                eDataType, MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].fWBCHGB_RstData[eDataType]);
    }
    //����CRC
    MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].usCrc = \
        CRC16((uint8_t*)&MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].tMsgHead, sizeof(WBCHGB_TestInfo_t)-4);
        
    //���������Ϊ���µģ�����Ҫ���¶�Ӧ��������������
    if(MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]].ulCurDataSN == Get_WBCHGB_Tail())
    {
        memmove((char*)&MachRunPara.tWBCHGB_TestInfo, (char*)&MachRunPara.taListView_Data[g_ucaListReview_DataTable[g_ucListView_DataIndex]], sizeof(WBCHGB_TestInfo_t));
    }
    
    //������Ϣ����ˣ��������ݵ��ļ�
    ListReview_t tListView = {0};
    tListView.tMsgHead.usCmd        = CMD_LIST_REVIEW_EDIT;
    tListView.tMsgHead.eErrorCode   = ERROR_CODE_SUCCESS; 
    tListView.tMsgHead.usMsgLen     = MSG_HEAD_LEN; 
    tListView.ucIndex               = g_ucListView_DataIndex;
    //������Ϣ
    UI_Put_Msg((uint8_t*)&tListView);

    
    return FEED_BACK_SUCCESS;
}



/*********************************************************************
*
*       _cbDialog
*/
static void EditResult_cbDialog(WM_MESSAGE * pMsg) {
  
  extern PrinterErrorType_e g_ePrinterErrorType;
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  int     i;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'WBC'
    //
    EditResult_Msg_Init(pMsg);

    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    case WM_KEYBOARD_UPDATE: 
    {
        char buffer[EDIT_RESULT_RESULT_MAX_LEN+1] = {0};
        uint8_t ucNum = 0, ucDNum = 0;
        //��������ʱ�����������ʾ
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_INPUT_ERROR);
        TEXT_SetText(hItem, "");
        
        //��ǰ�����edit
        hItem = WM_GetFocusedWindow();
        Id = WM_GetId(hItem);
        
        //
        i = strlen(pMsg->Data.p);
        if(i == 1) //ASCII��
        {
            char c = *((char*)pMsg->Data.p);
            if(('0' <= c && c <= '9') || c == '.')
            {
//                if(WM_HasFocus(WM_GetDialogItem(pMsg->hWin, ID_EDIT_WBC)))
//                {
//                    EDIT_GetText(hItem, buffer, EDIT_RESULT_RESULT_MAX_LEN);  
//                }else if(WM_HasFocus(WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAN))){ 
//                    EDIT_GetText(hItem, buffer, EDIT_RESULT_PERCENT_MAX_LEN);   
//                }else if(WM_HasFocus(WM_GetDialogItem(pMsg->hWin, ID_EDIT_MID))){
//                    EDIT_GetText(hItem, buffer, EDIT_RESULT_PERCENT_MAX_LEN);
//                }else if(WM_HasFocus(WM_GetDialogItem(pMsg->hWin, ID_EDIT_LYM))){
//                    EDIT_GetText(hItem, buffer, EDIT_RESULT_PERCENT_MAX_LEN);  
//                }else if(WM_HasFocus(WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB))){
//                    EDIT_GetText(hItem, buffer, EDIT_RESULT_RESULT_MAX_LEN);  
//                }
                //
                EDIT_GetText(hItem, buffer, 10);  
                for(i = 0; i < sizeof(buffer); i++)
                {
                    //ͳ��.����
                    if(buffer[i] ==  '.')
                    {
                        ucNum++;
                    }
                    //ucDNumС��������ֵĸ���
                    if(ucNum >= 1 && ('0' <= buffer[i] && buffer[i] <= '9')) ucDNum++;
                }
                //�����ظ������'.'�Ѿ�С�����Ķ�������ݣ�������λС�������λ��
                if((!(ucNum >= 1 && c == '.')) && ucDNum <= 2)
                {
                    GUI_StoreKeyMsg(c, 1);
                }
            }
        }
    }
    break;
    case WM_KEYBOARD_END: 
    {
        //��Ҫ�������浽ԭ����λ��
        if(1 == g_sER_MoveFlag)
        {
            WM_MoveTo(pMsg->hWin, 140, 60);
            g_sER_MoveFlag = 0;
        }
//        //��Ҫ�������浽ԭ����λ��
//       if(WM_IsEnabled(WM_GetDialogItem(pMsg->hWin, ID_EDIT_WBC)))
//       {
////           if(g_sER_MoveFlag == 1)
////           {
////                WM_MoveWindow(pMsg->hWin, ER_X_MOVE_LEN, ER_Y_MOVE_LEN);
////                g_sER_MoveFlag = 0;
////           }
//       }else if(WM_IsEnabled(WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAN))){    
////           if(g_sER_MoveFlag == 1)
////           {
////                WM_MoveWindow(pMsg->hWin, ER_X_MOVE_LEN, ER_Y_MOVE_LEN*2);
////                g_sER_MoveFlag = 0;
////           }
//       }else if(WM_IsEnabled(WM_GetDialogItem(pMsg->hWin, ID_EDIT_MID))){    
//           if(g_sER_MoveFlag == 1)
//           {
//                WM_MoveWindow(pMsg->hWin, ER_X_MOVE_LEN, ER_Y_MOVE_LEN);
//                g_sER_MoveFlag = 0;
//           }
//       }else if(WM_IsEnabled(WM_GetDialogItem(pMsg->hWin, ID_EDIT_LYM))){    
//           if(g_sER_MoveFlag == 1)
//           {
//                WM_MoveWindow(pMsg->hWin, ER_X_MOVE_LEN, ER_Y_MOVE_LEN*2);
//                g_sER_MoveFlag = 0;
//           }
//       }else if(WM_IsEnabled(WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB))){    
//           if(g_sER_MoveFlag == 1)
//           {
//                WM_MoveWindow(pMsg->hWin, ER_X_MOVE_LEN, ER_Y_MOVE_LEN*3);
//                g_sER_MoveFlag = 0;
//           }
//       }
       
       //
//        for(i = ID_EDIT_WBC; i <= ID_EDIT_HGB; i++)
//        {
//            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, i));
//        }
    }
    case WM_DELETE:
    {
        g_sER_MoveFlag = 0;
    }
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_EDIT_WBC: // Notifications sent by 'wbc_edit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
//        for(i = ID_EDIT_WBC; i <= ID_EDIT_HGB; i++)
//        {
//            WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, i));
//        }
//        WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_WBC));
        WM_SetFocus(pMsg->hWinSrc);
//        if(g_sER_MoveFlag == 0) //�ƶ�����
//        {
//             WM_MoveWindow(pMsg->hWin, ER_X_MOVE_LEN, -ER_Y_MOVE_LEN);
//            g_sER_MoveFlag = 1;
//        }
		MachRunPara.keyboardInitJM = KEYBOARD_NUM;
        CreateKeyboard_Window();
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_GRAN: // Notifications sent by 'Gran_btn'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
//        for(i = ID_EDIT_WBC; i <= ID_EDIT_HGB; i++)
//        {
//            WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, i));
//        }
//        WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_GRAN));
        WM_SetFocus(pMsg->hWinSrc);
        
//        if(g_sER_MoveFlag == 0) //�ƶ�����
//        {
//            WM_MoveWindow(pMsg->hWin, ER_X_MOVE_LEN, -ER_Y_MOVE_LEN*2);
//            g_sER_MoveFlag = 1;
//        }
		MachRunPara.keyboardInitJM = KEYBOARD_NUM;
        CreateKeyboard_Window();
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_MID: // Notifications sent by 'Mid_edit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
//        for(i = ID_EDIT_WBC; i <= ID_EDIT_HGB; i++)
//        {
//            WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, i));
//        }
//        WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_MID));
        WM_SetFocus(pMsg->hWinSrc); 
        if(g_sER_MoveFlag == 0) //�ƶ�����
        {
            WM_MoveWindow(pMsg->hWin, ER_X_MOVE_LEN, -ER_Y_MOVE_LEN);
            g_sER_MoveFlag = 1;
        }
		MachRunPara.keyboardInitJM = KEYBOARD_NUM;
        CreateKeyboard_Window();
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_EDIT_LYM: // Notifications sent by 'Lym_edit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
//        for(i = ID_EDIT_WBC; i <= ID_EDIT_HGB; i++)
//        {
//            WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, i));
//        }
//        WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_LYM));
        WM_SetFocus(pMsg->hWinSrc);   
        if(g_sER_MoveFlag == 0) //�ƶ�����
        {
            WM_MoveWindow(pMsg->hWin, ER_X_MOVE_LEN, -ER_Y_MOVE_LEN*2);
            g_sER_MoveFlag = 1;
        }
		MachRunPara.keyboardInitJM = KEYBOARD_NUM;
        CreateKeyboard_Window();
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
     case ID_EDIT_HGB: // Notifications sent by 'HGB_edit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
//        for(i = ID_EDIT_WBC; i <= ID_EDIT_HGB; i++)
//        {
//            WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, i));
//        }
//        WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_EDIT_HGB));
        WM_SetFocus(pMsg->hWinSrc);   
        if(g_sER_MoveFlag == 0) //�ƶ�����
        {
            WM_MoveWindow(pMsg->hWin, ER_X_MOVE_LEN, -ER_Y_MOVE_LEN*3);
            g_sER_MoveFlag = 1;
        }
		MachRunPara.keyboardInitJM = KEYBOARD_NUM;
        CreateKeyboard_Window();
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_CANCEL: // Notifications sent by 'cancel'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        GUI_EndDialog(pMsg->hWin, 0);
        if(MachRunPara.ulMenuID == GUI_WINDOW_LIST_REVIEW_ID)
        {
            g_hActiveWin = CreateListReview_Window();
        }else if(MachRunPara.ulMenuID == GUI_WINDOW_RESULT_INFO_ID){
            g_hActiveWin = ResultInfo_Menu();
        }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_SURE: // Notifications sent by 'sure'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(FEED_BACK_SUCCESS == CheckAndUpdate_Result(pMsg))
        {
            GUI_EndDialog(pMsg->hWin, 0);
            if(MachRunPara.ulMenuID == GUI_WINDOW_LIST_REVIEW_ID)
            {
                g_hActiveWin = CreateListReview_Window();
            }else if(MachRunPara.ulMenuID == GUI_WINDOW_RESULT_INFO_ID){
                g_hActiveWin = ResultInfo_Menu();
            }
            
        }else{
            TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_INPUT_ERROR), GUI_RED);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_INPUT_ERROR), g_ucaLng_InputError[MachInfo.systemSet.eLanguage]);
        }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateWindow
*/
WM_HWIN EditResult_CreateWindow(void);
WM_HWIN EditResult_CreateWindow(void) {
  WM_HWIN hWin;

  Public_Hide_Widget(); //���ع������ֿؼ�,
  WM_SetCallback(WM_HBKWIN, AlphaBk_cbBkWin);
	
	Disable_Algo_Task();
	
  hWin = GUI_CreateDialogBox(EditResult_aDialogCreate, GUI_COUNTOF(EditResult_aDialogCreate), EditResult_cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/