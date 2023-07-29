#ifndef __EXTI_H
#define __EXTI_H
#include "sys.h" 

#define BUFFER_LENGTH 350
#define MAX_DURATION 40000


extern u16 TimeOrder[BUFFER_LENGTH];
extern u16 Index;
extern u8 Flag;

void EXTIX_Init(void);//外部中断初始化	

#endif

