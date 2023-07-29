#ifndef __STR_H_
#define __STR_H_
#include "sys.h"

void AddEndChar(char* str, char c);
void CutEndChar(char* str);
void ClearStr(char* str);
u8 GetStrLen(char* str);
void AddStr(char* preStr, char* subStr);
u8 CompareStr(char* s1, char* s2);
char* IntToString(s32 num);
char* FloatToString(double num, u8 precision);
long StrToInt(char* str);
double StrToFloat(char* str);
char* IntToHex(u32 num);
#endif




