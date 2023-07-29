#include "exti.h"
#include "led.h"
#include "delay.h"
#include "uart.h"

u16 TimeOrder[BUFFER_LENGTH];
u16 Index;
u8 Flag;

void EXTIX_Init(void)
{
 
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);  //使能复用功能时钟

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

    EXTI_InitStructure.EXTI_Line= EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级2， 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03; //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure); 
}



//外部中断0服务程序 
void EXTI1_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line1);
    TimeOrder[Index++] = TIM_GetCounter(TIM2);
    if (!Flag) {
        Index--;
        TIM_Cmd(TIM2, ENABLE);  //使能TIMx
    }
    TIM_SetCounter(TIM2, 0);
    while(!PBin(1));
    TimeOrder[Index++] = TIM_GetCounter(TIM2);
    TIM_SetCounter(TIM2, 0);
    if (Index >= BUFFER_LENGTH) Index = 0;
    Flag = 1;
}



