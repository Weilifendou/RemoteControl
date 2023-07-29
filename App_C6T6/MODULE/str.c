#include "str.h"


/*****************************************************

以下代码是数值转换为字符串的程序。
u8* IntToString(long value); 此函数将一个整型数据转换为字符串，返回值为一个字符串指针。
u8* FloatToString(double value, u8 precision); 此函数将一个浮点型数据转换为字符串，返回值为一个字符串指针。

*******************************************************/

void AddEndChar(char* str, char c) {
    u8 i = 0;
    for (i = 0; str[i] != 0; i++);
    str[i] = c;
    str[i + 1] = 0;
    
}
void CutEndChar(char* str) {
    u8 i = 0;
    for (i = 0; str[i] != 0; i++);
    str[i - 1] = 0;
    
}

void ClearStr(char* str) {
    u8 i = 0;
    for (i = 0; str[i] != 0; i++)
        str[i] = 0;
}

u8 GetStrLen(char* str) {
    u8 i = 0;
    for (i = 0; str[i] != 0; i++);
    return i;
    
}
void AddStr(char* preStr, char* subStr) {
    u8 i = 0;
    u8 temp = GetStrLen(preStr);
    for (i = 0; subStr[i] != 0; i++) {
        preStr[temp + i] = subStr[i];
    }
    preStr[temp + i] = 0;
}


u8 CompareStr(char* s1, char* s2) {
    u8 i = 0;
    u8 j = 0;
    i = GetStrLen(s1);
    j = GetStrLen(s2);
    if (j != i) return 0;
    else {
        while (i--) {
            if (s1[i] != s2[i]) {
                return 0;
            }
        }
        return 1;
    }
}

char* IntToHex(u32 num) {
    u8 bits = 0;
    u8 temp = 0;
    u32 compare = 1;
    static char str[20] = {0};
    do{
        compare *= 0x10;
    }while(compare <= num);
    do{
        compare /= 0x10;
        temp = num / compare % 0x10;
        if(temp < 10) {
            str[bits] = temp + '0';
        } else {
            str[bits] = temp - 10 + 'A';
        }
        bits++;
    }while(compare > 1);
    str[bits] = 0;
    return str;
    
}

char* IntToString(s32 num)        //整型数值转换为字符串函数
{
    static char str[20] = {0};
    u8 bits = 0;
    long compare = 1;
    if(num < 0){
        num = -num;
        str[bits] = '-';
        bits++;
    }
    do{
        compare *= 10;
    }while(compare <= num);
    do{                                         
        compare /= 10;
        str[bits] = num / compare % 10 + '0';
        bits++;
    }while(compare > 1);
    str[bits] = 0;
    return str;
}

char* FloatToString(double num, u8 precision)       //浮点型数值转换为字符串函数
{
    static char str[20] = {0};
    u8 bits = 0;
    long integer = 0;
    long compare = 1;
    long decimalPower = 1;  
    double decimal = 0;
    if(num < 0){
        num = -num;
        str[bits] = '-';
        bits++;
    }
    integer = (long) num;
    decimal = num - integer;
    do{
        compare *= 10;
    }while(compare <= integer);
    do{
        compare /= 10;
        str[bits] = integer / compare % 10 + '0';
        bits++;
    }while(compare > 1);
    if(precision > 0){
        str[bits] = '.';
        do{
            bits++;
            precision--;
            decimalPower *= 10;
            str[bits] = (long) (decimal * decimalPower) % 10 + '0';
        }while(precision);
        str[++bits] = 0;
    }
    else str[bits] = 0;
    return str;
}


long StrToInt(char* str) {
    u8 i = 0;
    long temp = 1;
    long res = 0;
    if (str[0] == 0) {
        return 0;
    }
    if (str[0] != '-') {
        if (str[0] < '0' || str[0] > '9') {
            return 0xaaaaaaaa;
        }
    }
    i = (str[0] == '-') ? 1: 0;
    while (str[i] != 0) {
        temp *= 10;
        i++;
    }
    i = (str[0] == '-') ? 1: 0;
    while (str[i] != 0) {
        temp /= 10;
        res += (str[i] - '0') * temp;
        i++;
    }
    res = (str[0] == '-') ? -res: res;
    return res;
}


double StrToFloat(char* str) {
    u8 i = 0;
    long temp1 = 1;
    double temp2 = 1;
    double res = 0;
    u8 pointNum = 0;
    if (str[0] == 0) {
        return 0;
    }
    if (str[0] != '-') {
        if (str[0] < '0' || str[0] > '9') {
            return 0;
        }
    }
    for (i = 1; str[i] != 0; i++) {
        if (str[i] < '0' || str [i] > '9') {
            if (str[i] == '.') pointNum++;
            else return 0;
            if (pointNum > 1) return 0;
        }
    }
    i = (str[0] == '-') ? 1: 0;
    while (str[i] != '.') {
        temp1 *= 10;
        i++;
    }
    i = (str[0] == '-') ? 1: 0;
    while (str[i] != '.') {
        temp1 /= 10;
        res += (str[i] - '0') * temp1;
        i++;
    }
    i++;
    while (str[i] != 0) {
        temp2 /= 10;
        res += (str[i] - '0') * temp2;
        i++;
    }
    res = (str[0] == '-') ? -res: res;
    return res;
}


