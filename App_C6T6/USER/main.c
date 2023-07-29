#include "led.h"
#include "delay.h"
#include "sys.h"
#include "uart.h"
#include "str.h"
#include "oled.h"
#include "cncode.h"
#include "dht11.h"
#include "pwm.h"
#include "timer.h"
#include "iwdg.h"
#include "exti.h"
#include "infrared.h"
#include "stdlib.h"
#include "string.h"



/*********************��ֲʱ���ע���޸�************************/
//���ע���޸� FLASH_APP_ADDR 0x08002800 //��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)
//���ע���޸� STM32_FLASH_SIZE 32 //��ѡSTM32��FLASH������С(��λΪK)
/***************************************************************/

extern u8 FunCode;

int main(void)
{
    u16 i = 0;
    char text[30] = {0};
    char ymd[15] = {0};
    char hms[15] = {0};
    u8 delayer = 0;
    u8 TimeDelayer = 0;
//    NVIC_SetVectorTable(0x08000000, 0x2800);
    delay_init(); //����Ҫ���ã�������ʱ��׼ȷ
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    UART_Init(115200); 
    LED_Init(); //LED��ʼ��
    Infrared_Init();
    DHT11_Init(); //��ʪ�ȴ�������ʼ��
    EXTIX_Init();
    OLED_Init(); //��Ļ��ʼ��
    TIM2_Int_Init(4999, 71); //��ʱ��2��ʼ����ʱ1s
//    Iwdg_Init();
    
    while(1)
    {
        if (TIM_GetCounter(TIM2) >= MAX_DURATION) {
            Flag = 0;
            Index = 0;
            TIM_Cmd(TIM2, DISABLE);
            TIM_SetCounter(TIM2, 0);
        }
        if (FunCode == 0x55) {
            FunCode = 0x00;
            for (i = 0; TimeOrder[i]; i++) {
                printf("%d\r\n", TimeOrder[i]);
//                TimeOrder[i] = 0;
            }
            printf("\r\n");
            __disable_irq();
            for (i = 0; TimeOrder[i]; i++) {
                if (i % 2 == 0) {
                    INFRARED = 0;
                } else {
                    INFRARED = 1;
                }
                delay_us(TimeOrder[i]);
            }
            INFRARED = 1;
            __enable_irq();
//            while(i < BUFFER_LENGTH) TimeOrder[i++] = 0;
            
        }
        if (TimeDelayer >= 5) {
            TimeDelayer = 0;
            //��ʾ��ǰ�¶�
            
            sprintf(text, "%.1f^C", Temperature / 10.0);
            OLED_LeftShowStr(0, text, 2);
            
            //��ʾ��ǰʪ��
            sprintf(text, "%.1f%%", Humidty / 10.0);
            OLED_RightShowStr(0, text, 2);
            
            StampToStandard(text, TimeStamp);
            strncpy(ymd, text, 10);
            OLED_CenterShowStr(3, ymd, 2);
            
            strncpy(hms, text+11, 10);
            OLED_CenterShowStr(5, hms, 2);
            
            
            OLED_Validate();
        }
        if (delayer >= 30) {
            delayer = 0;
            DHT11_ReadData();
        }
        if (FunCode == 1 || FunCode == 3) {
            FunCode = 0;
            NVIC_SystemReset();
        }
//        IWDG_ReloadCounter();
        delay_ms(100);
        delayer++;
        TimeDelayer++;
    }
} 

