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



/*********************移植时务必注意修改************************/
//务必注意修改 FLASH_APP_ADDR 0x08002800 //第一个应用程序起始地址(存放在FLASH)
//务必注意修改 STM32_FLASH_SIZE 32 //所选STM32的FLASH容量大小(单位为K)
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
    delay_init(); //必须要调用，否则延时不准确
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    UART_Init(115200); 
    LED_Init(); //LED初始化
    Infrared_Init();
    DHT11_Init(); //温湿度传感器初始化
    EXTIX_Init();
    OLED_Init(); //屏幕初始化
    TIM2_Int_Init(4999, 71); //定时器2初始化定时1s
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
            //显示当前温度
            
            sprintf(text, "%.1f^C", Temperature / 10.0);
            OLED_LeftShowStr(0, text, 2);
            
            //显示当前湿度
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

