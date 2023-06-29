#ifndef _ALG_PLUS_DEFS_H_
#define _ALG_PLUS_DEFS_H_

// 故障代码
enum emErrorMode
{
	EM_SUCCESS       = 0,	 // 测试成功
    EM_1111          = 1,    // 计数池宝石孔异常（首次开阀2前，前后池导通）
	EM_1112          = 2,    // 计数池宝石孔异常（第一次开阀2后1s内，前后池导通）
	EM_1124          = 3,    // 计数池漏气。三次建压机会完成了测试
	EM_1131          = 4,    // 计数超时异常（前10s）-INFANT/CHILD only
	EM_1132          = 5,    // 计数超时异常（后10s）
	EM_2033          = 6,    // 内部通讯不稳定（数据丢包）
	EM_2091          = 7,    // 计数池电极接触异常（探针对接问题）
};

// 参考组模式
enum emRefGroupMode
{
	RGM_GENERAL    = 0,	   // 通用
	RGM_MALE	   = 1,	   // 成男
	RGM_FEMALE	   = 2,	   // 成女
    RGM_INFANT     = 3,    // 婴幼儿
    RGM_CHILD      = 4,    // 儿童
    RGM_OTHER      = 5,    // 其他（自定义）
};

// 用户模式
enum emUsersMode
{
	UM_EXPERT          = 0,    // 用服工程师用户
	UM_ADMIN           = 1,    // 管理员用户
	UM_NORMAL          = 2,    // 普通用户
};

// 测试模式
enum emTestFlag
{
	TF_NORMAL       = 0,    // 正常测试
	TF_REVIEW       = 1,    // 可疑测试
	TF_ALERT        = 2,    // 异常测试（预测）
	TF_INVALID      = 3,    // 故障测试
};

#endif // _ALG_PLUS_DEFS_H_

