#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern u32 TimeStamp;

void TIM2_Int_Init(u16 arr,u16 psc);
void StampToStandard(char* s, u32 stampTime);
#endif
