#ifndef _DATA_MAPS_H_
#define _DATA_MAPS_H_

#include "pdatatype.h"

// ����������������-������ʾ��Ϣ������:isChLangΪTRUE��Ӣ��:isChLangΪFALSE��
BOOL8 flagParaItemMessageMapWbc(char * messageDest, const char * messageSrc, int usersMode, BOOL8 isChLang);

BOOL8 flagParaItemMessageMapHgb(char * messageDest, const char * messageSrc, int usersMode, BOOL8 isChLang);

#endif // _DATA_MAPS_H_

