#ifndef _PARA_FLAG_H_
#define _PARA_FLAG_H_


// 样本模式
enum emParaFlag
{
    PF_NORMAL       = 0x00,    // 正常值
    PF_REVIEW       = 0x01,    // 可疑值（提示复检）
    PF_INVALID      = 0x02,    // 无效值
    PF_ALERT        = 0x10,    // 异常结果提示标识
};


#endif // _PARA_FLAG_H_

