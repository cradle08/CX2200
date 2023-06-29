#ifndef _MODE_DEFS_H_
#define _MODE_DEFS_H_


#define  am_hasWbcMode(m)  \
    ( m == AM_WBC ) | ( m == AM_WBC_HGB ) | ( m == AM_WBC_HGB_RBC_PLT ) | ( m == AM_WBC_HGB_CRP ) | ( m == AM_WBC_HGB_RBC_PLT_CRP )

#define  am_hasHgbMode(m)  \
	( m == AM_HGB ) | ( m == AM_WBC_HGB ) | ( m == AM_WBC_HGB_RBC_PLT ) | ( m == AM_WBC_HGB_CRP ) | ( m == AM_WBC_HGB_RBC_PLT_CRP )

#define  am_hasRbcMode(m) \
	( m == AM_RBC ) | ( m == AM_RBC_PLT ) | ( m == AM_WBC_HGB_RBC_PLT ) | ( m == AM_WBC_HGB_RBC_PLT_CRP )

#define  am_hasPltMode(m) \
    ( m == AM_PLT ) | ( m == AM_RBC_PLT ) | ( m == AM_WBC_HGB_RBC_PLT ) | ( m == AM_WBC_HGB_RBC_PLT_CRP )

#define  am_hasCrpMode(m)  \
	( m == AM_CRP ) | ( m == AM_WBC_HGB_CRP ) | ( m == AM_WBC_HGB_RBC_PLT_CRP )


// ����ģʽ
enum emAnalysisMode
{
/*
    AM_CBC          = 0,    // CBC
    AM_WBC          = 1,    // WBC
    AM_CBC_DIFF     = 2,    // CBC + DIFF
    AM_CRP          = 3,    // CRP
    AM_CBC_CRP      = 4,    // CBC + CRP
    AM_CBC_DIFF_CRP = 5,    // CBC + DIFF + CRP
    AM_WHB          = 6,    // WBC + HGB*/

	//new
	AM_WBC					= 0,    // WBC
	AM_HGB					= 1,    // HGB
	AM_RBC					= 2,    // RBC
	AM_PLT					= 3,    // PLT
	AM_CRP					= 4,    // CRP
	AM_WBC_HGB				= 5,    // WBC + HGB
	AM_RBC_PLT				= 6,    // RBC + PLT
	AM_WBC_HGB_RBC_PLT		= 7,    // WBC + HGB + RBC + PLT
	AM_WBC_HGB_CRP		    = 8,    // WBC + HGB + CRP
	AM_WBC_HGB_RBC_PLT_CRP  = 9,    // WBC + HGB + RBC + PLT + CRP
};

// ����ģʽ
enum emSamplingVialMode
{
    SVM_OPEN        = 0,    // ���Ž���
    SVM_AUTO        = 1,    // �Զ�����
    SVM_CLOSED      = 2,    // ��ս���
};

// ����ģʽ
enum emSpecimenMode
{
    SM_VB           = 0,    // ����Ѫ��Venous Blood��������ȫѪ��Venous Whole Blood��
    SM_PB           = 1,    // ĩ��Ѫ��Peripheral Blood����ĩ��ȫѪ��Peripheral Whole Blood��
    SM_PxB          = 2,    // Ԥϡ��Ѫ��Prediluted VB or PB��
    SM_PVB          = 3,    // Ԥϡ�;���Ѫ��Prediluted VB��
    SM_PPB          = 4,    // Ԥϡ��ĩ��Ѫ��Prediluted PB��
};

// ����ģʽ
enum emSpeciesMode
{
    SPM_HUMAN       = 0,    // ��
    SPM_CAT         = 1,    // è
    SPM_DOG         = 2,    // ��
    SPM_MOUSE       = 3,    // ��
    SPM_RABBIT      = 4,    // ��
    SPM_OTHER       = 99,   // �������Զ��壩
};

// ����ģʽ
enum emFunctionMode
{
    FM_SAMPLE        = 0,    // ��������
    FM_QC            = 1,    // �ʿؼ���
    FM_CALIBRATION   = 2,    // У׼����
    FM_BACKGROUND    = 3,    // ���׼���
};

// CRP����ģʽ
enum emCrpScaleMode
{
    CSM_TPLINE       = 0,     // ���㶨��
    CSM_SPLINE       = 1,     // ��㶨��
    CSM_LOG5P        = 2,     // ��������
};

// �㷨����ģʽ
enum emAlgMode
{
	AM_DEBUG		= 0,     // ����ģʽ������Ϣ��ӡ��
	AM_RELEASE      = 1,     // ����ģʽ
};


#endif // _MODE_DEFS_H_

