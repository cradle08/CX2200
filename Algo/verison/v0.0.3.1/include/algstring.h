#ifndef _ALGSTRING_H_
#define _ALGSTRING_H_

#define MAX_CHAR_LENGTH 300

// Çó×Ö·û´®³¤¶È
int charlen(char *charIn);

// ×Ö·û´®ºÏ²¢
void charcat(char *charOut, int nOutLen, char *charIn1, int nInLen1, char *charIn2, int nInLen2);

// ×Ö·û´®¸´ÖÆ
void charcpy(char *charOut, int nOutLen, char *charIn, int nInLen);

#endif // _ALGSTRING_H_

