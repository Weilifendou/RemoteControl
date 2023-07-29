#include "sys.h"
#include "uart.h"
#include "str.h"
#include "delay.h"

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB      
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
    int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
    x = x; 
}

//__use_no_semihosting was requested, but _ttywrch was 
void _ttywrch(int ch)
{
    ch = ch;
}
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
    while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
    return ch;
}
#endif 


void UART_Init(u32 bound){
  //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
     
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);    //使能USART1，GPIOA时钟

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

    //USART1_RX      GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);    //根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART1, ENABLE);                    //使能串口1 

}


u16 CRCCheckout(u8 *dat, u16 length) {
    u8 temp = 0;
    u16 i = 0;
    u16 j = 0;
    u16 crc = 0xffff;
    for (i = 0; i < length; i++) {
        temp = *(dat + i) & 0x00ff;
        crc ^= temp;
        for (j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xa001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}


void SendData(u8 funCode, u8* dat, u16 length) {
    u16 i = 0;
    u16 crc = 0;
    u8 headerLength = 5;
    u8 checkLength = 2;
    const u8 buffLength = 8;
    u8 sendBuff[buffLength] = {0};
    sendBuff[0] = 0x55;
    sendBuff[1] = 0xaa;
    sendBuff[2] = funCode; //功能码
    sendBuff[3] = length >> 8; //数据长度
    sendBuff[4] = length; //数据长度
     for (i = 0; i < length; i++)
    {
        sendBuff[i + headerLength] = *(dat + i);
    }
    crc = CRCCheckout(sendBuff, buffLength - checkLength);
    sendBuff[buffLength - 2] = crc >> 8;
    sendBuff[buffLength - 1] = crc;
    for (i = 0; i < buffLength; i++) {
        SendByte(sendBuff[i]);
    }
}

//__attribute__((at(0x20001000)));


u8 RecBuff[BUFF_SIZE];
u8 FunCode;


//跳转到应用程序段
//addr:用户代码起始地址.
void JumpToAddr(u32 addr)
{
    JumpFun jumpAddr;
    if(((*(vu32*)addr)&0x2FFE0000)==0x20000000) //检查栈顶地址是否合法.
    { 
        jumpAddr=(JumpFun)*(vu32*)(addr+4); //用户代码区第二个字为程序开始地址(复位地址)
        MSR_MSP(*(vu32*)addr); //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
        jumpAddr(); //跳转到APP.
    }
}


void USART1_IRQHandler(void)                    //串口1中断服务程序
{
    u16 s = 0, i = 0;
//    u16 recCRC = 0, calCRC = 0;
    u16 datLength = 0;
    u8 checkout = 0;
    static u16 buffCounter = 0;
    static u8 hash[16] = {0};
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
    {
        RecBuff[buffCounter++] = USART_ReceiveData(USART1);
        FunCode = USART_ReceiveData(USART1);
        if (buffCounter >= BUFF_SIZE) {
            buffCounter = 0;
            for (s = 0; s < BUFF_SIZE; s++) {
                if (RecBuff[s] == 0x55 && RecBuff[s+1] == 0xaa
               && RecBuff[s+2] == 0x55 && RecBuff[s+3] == 0xaa) {
                    datLength = RecBuff[s+4];
                    datLength <<= 8;
                    datLength |= RecBuff[s+5];
                    break;
                }
            }
            if (s+datLength < BUFF_SIZE) {
//                calCRC = CRCCheckout(RecBuff+s, datLength);
//                recCRC = RecBuff[s+datLength];
//                recCRC <<= 8;
//                recCRC |= RecBuff[s+datLength+1];
//                if (calCRC == recCRC) {
//                MD5Final(hash, RecBuff+s, datLength);
                for (i = 0; i < 16; i++) {
                    if (hash[i] == RecBuff[datLength+i]) {
                        checkout++;
                    }
                }
                if (checkout >= 16) {
                    s += 6;
                    FunCode = RecBuff[s++];
                }
            }
        }
    }
}

void SendByte(u8 byte)
{
    USART_SendData(USART1,byte);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC )==RESET);
}


