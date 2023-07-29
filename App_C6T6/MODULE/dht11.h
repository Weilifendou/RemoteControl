#ifndef __DHT11_H_
#define __DHT11_H_

#include "sys.h"

#define DHT PAin(8)
#define DHT_UP GPIO_SetBits(GPIOA, GPIO_Pin_8)
#define DHT_DOWN GPIO_ResetBits(GPIOA, GPIO_Pin_8)

extern u16 Temperature, Humidty;

void DatLineOut(void);
void DatLineIn(void);


void DHT11_Init(void);
void DHT11_Start(void);

void DHT11_ReadData(void);


#endif



