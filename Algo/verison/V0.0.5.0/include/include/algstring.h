#ifndef _ALGSTRING_H_
#define _ALGSTRING_H_

#define MAX_CHAR_LENGTH 300

// ���ַ�������
int charlen(char *charIn);

// �ַ����ϲ�
void charcat(char *charOut, int nOutLen, char *charIn1, int nInLen1, char *charIn2, int nInLen2);

// �ַ�������
void charcpy(char *charOut, int nOutLen, char *charIn, int nInLen);

#endif // _ALGSTRING_H_

