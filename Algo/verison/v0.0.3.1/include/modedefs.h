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


// 分析模式
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

// 进样模式
enum emSamplingVialMode
{
    SVM_OPEN        = 0,    // 开放进样
    SVM_AUTO        = 1,    // 自动进样
    SVM_CLOSED      = 2,    // 封闭进样
};

// 样本模式
enum emSpecimenMode
{
    SM_VB           = 0,    // 静脉血（Venous Blood），静脉全血（Venous Whole Blood）
    SM_PB           = 1,    // 末梢血（Peripheral Blood），末梢全血（Peripheral Whole Blood）
    SM_PxB          = 2,    // 预稀释血（Prediluted VB or PB）
    SM_PVB          = 3,    // 预稀释静脉血（Prediluted VB）
    SM_PPB          = 4,    // 预稀释末梢血（Prediluted PB）
};

// 物种模式
enum emSpeciesMode
{
    SPM_HUMAN       = 0,    // 人
    SPM_CAT         = 1,    // 猫
    SPM_DOG         = 2,    // 狗
    SPM_MOUSE       = 3,    // 鼠
    SPM_RABBIT      = 4,    // 兔
    SPM_OTHER       = 99,   // 其他（自定义）
};

// 功能模式
enum emFunctionMode
{
    FM_SAMPLE        = 0,    // 样本分析
    FM_QC            = 1,    // 质控计数
    FM_CALIBRATION   = 2,    // 校准计数
    FM_BACKGROUND    = 3,    // 本底计数
};

// CRP定标模式
enum emCrpScaleMode
{
    CSM_TPLINE       = 0,     // 两点定标
    CSM_SPLINE       = 1,     // 多点定标
    CSM_LOG5P        = 2,     // 对数定标
};

// 算法运行模式
enum emAlgMode
{
	AM_DEBUG		= 0,     // 调试模式（含信息打印）
	AM_RELEASE      = 1,     // 发布模式
};


#endif // _MODE_DEFS_H_

