#include "dht11.h"
#include "delay.h"

u16 Temperature, Humidty;

void DHT11_Init(void) {

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
}

void DatLineOut(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_8);
    
}


void DatLineIn(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
}


void DHT11_Start(void) {
    u16 overtime = 0;
    DatLineOut();
    DHT_DOWN;
    delay_ms(20);
    DHT_UP;
    DatLineIn();
    overtime = 0xf000;
    while (DHT && overtime++);
//    PrintText("Sucessful Response!");
    overtime = 0xf000;
    while (!DHT && overtime++);
//    PrintText("Prepare Transport!");
    overtime = 0xf000;
    while (DHT && overtime++);
//    PrintText("Start Transport!");
}

static u8 GetOneByte(void) {
    u16 overtime = 0;
    u8 i = 0;
    u8 dat = 0;
    for(i = 0; i < 8;i++) {
        overtime = 0xf000;
        while (!DHT && overtime++);
        delay_us(30);  //非常关键的延时
        dat <<= 1;
        if (DHT) {
            dat |= 1;
        }
        overtime = 0xf000;
        while (DHT && overtime++);
    }
    return dat;
}

void DHT11_ReadData(void) {
    u8 i = 0;
    u8 check = 0;
    u8 dat[5] = {0};
    float temp = 0;
    //EA = 0;   //必须关闭其他所有中断，可能会出错
    DHT11_Start();
    __disable_irq();   // 关闭总中断，按理说，应该放在start前面，但是放前面总卡死
    for (i = 0; i < 5; i++) {
        dat[i] = GetOneByte();
    }
    DatLineOut();
    DHT_UP; //拉高数据线，便于下次读取
    //EA = 1;  //打开关闭的中断
    /* 数据格式依次是 */
    /* dat[0]->湿度整数部分 */
    /* dat[1]->湿度小数部分 */
    /* dat[2]->温度整数部分 */
    /* dat[3]->温度小数部分 */
    /* dat[4]->校验和（前四个数据之和） */
    
    for (i = 0; i < 4; i++) {
        check += dat[i];
    }
    if (dat[4] != check) {
        DHT11_ReadData();
    }
    __enable_irq();   // 打开总中断
    temp = dat[0] + dat[1] * 1.0 / 10.0;
    Humidty = temp * 10;
    
    temp = dat[2] + dat[3] * 1.0 / 10.0;
    Temperature = temp * 10;
}




