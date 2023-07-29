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
        delay_us(30);  //�ǳ��ؼ�����ʱ
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
    //EA = 0;   //����ر����������жϣ����ܻ����
    DHT11_Start();
    __disable_irq();   // �ر����жϣ�����˵��Ӧ�÷���startǰ�棬���Ƿ�ǰ���ܿ���
    for (i = 0; i < 5; i++) {
        dat[i] = GetOneByte();
    }
    DatLineOut();
    DHT_UP; //���������ߣ������´ζ�ȡ
    //EA = 1;  //�򿪹رյ��ж�
    /* ���ݸ�ʽ������ */
    /* dat[0]->ʪ���������� */
    /* dat[1]->ʪ��С������ */
    /* dat[2]->�¶��������� */
    /* dat[3]->�¶�С������ */
    /* dat[4]->У��ͣ�ǰ�ĸ�����֮�ͣ� */
    
    for (i = 0; i < 4; i++) {
        check += dat[i];
    }
    if (dat[4] != check) {
        DHT11_ReadData();
    }
    __enable_irq();   // �����ж�
    temp = dat[0] + dat[1] * 1.0 / 10.0;
    Humidty = temp * 10;
    
    temp = dat[2] + dat[3] * 1.0 / 10.0;
    Temperature = temp * 10;
}




