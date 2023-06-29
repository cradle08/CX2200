#ifndef _DATA_MAPS_H_
#define _DATA_MAPS_H_

#include "pdatatype.h"

// 报警参数：报警项-报警提示信息（中文:isChLang为TRUE，英文:isChLang为FALSE）
BOOL8 flagParaItemMessageMapWbc(char * messageDest, const char * messageSrc, int usersMode, BOOL8 isChLang);

BOOL8 flagParaItemMessageMapHgb(char * messageDest, const char * messageSrc, int usersMode, BOOL8 isChLang);

#endif // _DATA_MAPS_H_

