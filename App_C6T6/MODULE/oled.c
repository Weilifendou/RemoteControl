/************************************************************************************

* Function List:
*    1. void I2C_Configuration(void) -- ����CPU��Ӳ��I2C
* 2. void I2C_WriteByte(uint8_t addr,uint8_t data) -- ��Ĵ�����ַдһ��byte������
* 3. void WriteCmd(u8 I2C_Command) -- д����
* 4. void WriteDat(u8 I2C_Data) -- д����
* 5. void OLED_Init(void) -- OLED����ʼ��
* 6. void OLED_SetPos(u8 x, u8 y) -- ������ʼ������
* 7. void OLED_DrawFillRectangle(u8 fill_Data) -- ȫ�����
* 8. void OLED_CLS(void) -- ����
* 9. void OLED_ON(void) -- ����
* 10. void OLED_OFF(void) -- ˯��
* 11. void OLED_ShowStr(u8 x, u8 y, u8 ch[], u8 TextSize) -- ��ʾ�ַ���(�����С��6*8��8*16����)
* 12. void OLED_ShowCN(u8 x, u8 y, u8 N) -- ��ʾ����(������Ҫ��ȡģ��Ȼ��ŵ�codetab.h��)
* 13. void OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[]) -- BMPͼƬ
*
* History: none;
*
*************************************************************************************/

#include "oled.h"
#include "delay.h"
#include "codetab.h"

static u8 oled_buff[128][8];
static u8 last_oled_buff[128][8];

//��ʼ��IIC
void OLED_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //RCC->APB2ENR|=1<<4;//��ʹ������IO PORTCʱ�� 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    
       
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
 
    OLED_IIC_SCL=1;
    OLED_IIC_SDA=1;
}

void OLED_SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void OLED_SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


//����IIC��ʼ�ź�
void OLED_IIC_Start(void)
{
    OLED_SDA_OUT();     //sda�����
    OLED_IIC_SDA=1;            
    OLED_IIC_SCL=1;
    OLED_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
    OLED_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}      
//����IICֹͣ�ź�
void OLED_IIC_Stop(void)
{
    OLED_SDA_OUT();//sda�����
    OLED_IIC_SCL=0;
    OLED_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    OLED_IIC_SCL=1; 
    OLED_IIC_SDA=1;//����I2C���߽����ź�                               
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 OLED_IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    OLED_SDA_IN();      //SDA����Ϊ����  
    OLED_IIC_SDA=1;   
    OLED_IIC_SCL=1;     
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            OLED_IIC_Stop();
            return 1;
        }
    }
    OLED_IIC_SCL=0;//ʱ�����0        
    return 0;  
} 
//����ACKӦ��
void OLED_IIC_Ack(void)
{
    OLED_IIC_SCL=0;
    OLED_SDA_OUT();
    OLED_IIC_SDA=0;
    OLED_IIC_SCL=1;
    OLED_IIC_SCL=0;
}
//������ACKӦ��            
void OLED_IIC_NAck(void)
{
    OLED_IIC_SCL=0;
    OLED_SDA_OUT();
    OLED_IIC_SDA=1;
    OLED_IIC_SCL=1;
    OLED_IIC_SCL=0;
}                                          
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��              
void OLED_IIC_Send_Byte(u8 txd)
{                        
    u8 t;
    OLED_SDA_OUT();
    OLED_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        OLED_IIC_SDA=(txd&0x80)>>7;
        txd<<=1;       
        OLED_IIC_SCL=1;
        OLED_IIC_SCL=0;    
    }
    
}         
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 OLED_IIC_Read_Byte(u8 ack)
{
    u8 i,receive=0;
    OLED_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
    {
        OLED_IIC_SCL=0; 
        OLED_IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
    }                     
    if (!ack)
        OLED_IIC_NAck();//����nACK
    else
        OLED_IIC_Ack(); //����ACK   
    return receive;
}


void OLED_Write(u8 a, u8 b)
{
    __disable_irq();   // �ر����ж�
    OLED_IIC_Start();
    OLED_IIC_Send_Byte(OLED_ADDRESS); //����������ַ+дָ��
    OLED_IIC_Wait_Ack();
    OLED_IIC_Send_Byte(a);        //���Ϳ����ֽ�
    OLED_IIC_Wait_Ack();
    OLED_IIC_Send_Byte(b);
    OLED_IIC_Wait_Ack();
    OLED_IIC_Stop();
    __enable_irq();   // �����ж�
}


void WriteCmd(u8 I2C_Command)//д����
{
    OLED_Write(0x00, I2C_Command);
}

void WriteDat(u8 I2C_Data)//д����
{
    OLED_Write(0x40, I2C_Data);
}

void OLED_Init(void)
{
    OLED_IIC_Init();
    delay_ms(100); //�������ʱ����Ҫ
    WriteCmd(0xAE); //display off
    WriteCmd(0x20);    //Set Memory Addressing Mode    
    WriteCmd(0x10);    //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    WriteCmd(0xb0);    //Set Page Start Address for Page Addressing Mode,0-7
    WriteCmd(0xc8);    //Set COM Output Scan Direction
    WriteCmd(0x00); //---set low column address
    WriteCmd(0x10); //---set high column address
    WriteCmd(0x40); //--set start line address
    WriteCmd(0x81); //--set contrast control register
    WriteCmd(0xff); //���ȵ��� 0x00~0xff
    WriteCmd(0xa1); //--set segment re-map 0 to 127
    WriteCmd(0xa6); //--set normal display
    WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
    WriteCmd(0x3F); //
    WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    WriteCmd(0xd3); //-set display offset
    WriteCmd(0x00); //-not offset
    WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    WriteCmd(0xf0); //--set divide ratio
    WriteCmd(0xd9); //--set pre-charge period
    WriteCmd(0x22); //
    WriteCmd(0xda); //--set com pins hardware configuration
    WriteCmd(0x12);
    WriteCmd(0xdb); //--set vcomh
    WriteCmd(0x20); //0x20,0.77xVcc
    WriteCmd(0x8d); //--set DC-DC enable
    WriteCmd(0x14); //
    WriteCmd(0xaf); //--turn on oled panel
    
}

void OLED_SetPos(u8 x, u8 y) //������ʼ������
{ 
    WriteCmd(0xb0+y);
    WriteCmd(((x&0xf0)>>4)|0x10);
    WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(u8 fill_Data)//ȫ�����
{
    u8 m,n;
    for(m=0;m<8;m++)
    {
        WriteCmd(0xb0+m);        //page0-page1
        WriteCmd(0x00);        //low column start address
        WriteCmd(0x10);        //high column start address
        for(n=0;n<128;n++)
        {
            WriteDat(fill_Data);
        }
    }
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED�������л���
//--------------------------------------------------------------
void OLED_ON(void)
{
    WriteCmd(0X8D);  //���õ�ɱ�
    WriteCmd(0X14);  //������ɱ�
    WriteCmd(0XAF);  //OLED����
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
    WriteCmd(0X8D);  //���õ�ɱ�
    WriteCmd(0X10);  //�رյ�ɱ�
    WriteCmd(0XAE);  //OLED����
}

void OLED_WriteBuff(u8 x,u8 y, u8 ch)
{
    if (x >= 128) x -= 128;
    if (y >= 8) y -= 8;
    oled_buff[x][y] = ch;
}

void OLED_ClearBuff(void)
{
    u8 m,n;
    for(m=0;m<8;m++)
        for(n=0;n<128;n++)
            oled_buff[n][m] = 0x00;
}

void OLED_Validate(void) {
    u8 m,n;
    for(m=0;m<8;m++)
    {
        WriteCmd(0xb0+m);        //page0-page1
        WriteCmd(0x00);        //low column start address
        WriteCmd(0x10);        //high column start address
        for(n=0;n<128;n++)
        {
            WriteDat(oled_buff[n][m]);
        }
    }
    OLED_ClearBuff();
}

void OLED_DrawPoint(u8 x, u8 y) {
    u8 bit = y % 8;
    y /= 8;
    oled_buff[x][y] |= (0x01 << bit);
}


/*******************************************************************
 * @name       :void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u16 color)
 * @date       :2018-08-09 
 * @function   :Draw a line between two points
 * @parameters :x1:the bebinning x coordinate of the line
                y1:the bebinning y coordinate of the line
                                x2:the ending x coordinate of the line
                                y2:the ending y coordinate of the line
                                color:the filled color value
 * @retvalue   :None
********************************************************************/
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2)
{
    u16 t; 
    int xerr=0,yerr=0,delta_x,delta_y,distance; 
    int incx,incy,uRow,uCol; 

    delta_x=x2-x1; //������������ 
    delta_y=y2-y1; 
    uRow=x1; 
    uCol=y1; 
    if(delta_x>0)incx=1; //���õ������� 
    else if(delta_x==0)incx=0;//��ֱ�� 
    else {incx=-1;delta_x=-delta_x;} 
    if(delta_y>0)incy=1; 
    else if(delta_y==0)incy=0;//ˮƽ�� 
    else{incy=-1;delta_y=-delta_y;} 
    if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
    else distance=delta_y; 
    for(t=0;t<=distance+1;t++ )//������� 
    {  
        OLED_DrawPoint(uRow,uCol);//����
        xerr+=delta_x ; 
        yerr+=delta_y ; 
        if(xerr>distance) 
        { 
            xerr-=distance; 
            uRow+=incx; 
        } 
        if(yerr>distance) 
        { 
            yerr-=distance; 
            uCol+=incy; 
        } 
    }  
} 

/*****************************************************************************
 * @name       :void OLED_DrawRectangle(u8 x1, u8 y1, u8 x2, u8 y2, u16 color)
 * @date       :2018-08-09 
 * @function   :Draw a rectangle
 * @parameters :x1:the bebinning x coordinate of the rectangle
                y1:the bebinning y coordinate of the rectangle
                                x2:the ending x coordinate of the rectangle
                                y2:the ending y coordinate of the rectangle
                                color:the filled color value
 * @retvalue   :None
******************************************************************************/
void OLED_DrawRectangle(u8 x1, u8 y1, u8 x2, u8 y2)
{
    OLED_DrawLine(x1,y1,x2,y1);
    OLED_DrawLine(x1,y1,x1,y2);
    OLED_DrawLine(x1,y2,x2,y2);
    OLED_DrawLine(x2,y1,x2,y2);
}  
/*****************************************************************************
 * @name       :void OLED_DrawFillRectangle(u8 x1, u8 y1, u8 x2, u8 y2, u16 color)
 * @date       :2018-08-09 
 * @function   :Filled a rectangle
 * @parameters :x1:the bebinning x coordinate of the filled rectangle
                y1:the bebinning y coordinate of the filled rectangle
                                x2:the ending x coordinate of the filled rectangle
                                y2:the ending y coordinate of the filled rectangle
                                color:the filled color value
 * @retvalue   :None
******************************************************************************/  
void OLED_DrawFillRectangle(u8 x1, u8 y1, u8 x2, u8 y2)
{
    u8 x = 0;
    u8 y = 0;
    for (x = x1; x < x2; x++)
        for (y = y1; y < y2; y++)
        OLED_DrawPoint(x, y);
}
 
/*****************************************************************************
 * @name       :void _draw_circle_8(int xc, int yc, int x, int y, u16 c)
 * @date       :2018-08-09 
 * @function   :8 symmetry circle drawing algorithm (internal call)
 * @parameters :xc:the x coordinate of the Circular center 
                yc:the y coordinate of the Circular center 
                                x:the x coordinate relative to the Circular center 
                                y:the y coordinate relative to the Circular center 
                                c:the color value of the circle
 * @retvalue   :None
******************************************************************************/  
static void _draw_circle_8(u8 xc, u8 yc, u8 x, u8 y)
{
    OLED_DrawPoint(xc + x, yc + y);

    OLED_DrawPoint(xc - x, yc + y);

    OLED_DrawPoint(xc + x, yc - y);

    OLED_DrawPoint(xc - x, yc - y);

    OLED_DrawPoint(xc + y, yc + x);

    OLED_DrawPoint(xc - y, yc + x);

    OLED_DrawPoint(xc + y, yc - x);

    OLED_DrawPoint(xc - y, yc - x);
}

/*****************************************************************************
 * @name       :void OLED_DrawCircle(int xc, int yc,int r, int fill, u16 color)
 * @date       :2022-07-06 
 * @function   :Draw a circle of specified size at a specified location
 * @parameters :xc:the x coordinate of the Circular center 
                yc:the y coordinate of the Circular center 
                                r:Circular radius
                                fill:1-filling,0-no filling
 * @retvalue   :None
******************************************************************************/  
void OLED_DrawCircle(u8 xc, u8 yc, u8 r, u8 fill)
{
    int x = 0, y = r, yi, d;
    d = 3 - 2 * r;
    if (fill)
    {
        // �����䣨��ʵ��Բ��
        while (x <= y) {
            for (yi = x; yi <= y; yi++)
                _draw_circle_8(xc, yc, x, yi);
            if (d < 0) {
                d = d + 4 * x + 6;
            } else {
                d = d + 4 * (x - y) + 10;
                y--;
            }
            x++;
        }
    } else {
        // �������䣨������Բ��
        while (x <= y) {
            _draw_circle_8(xc, yc, x, y);
            if (d < 0) {
                d = d + 4 * x + 6;
            } else {
                d = d + 4 * (x - y) + 10;
                y--;
            }
            x++;
        }
    }
}

/*****************************************************************************
 * @name       :void Draw_Triangle(u8 x0,u8 y0,u8 x1,u8 y1,u8 x2,u8 y2)
 * @date       :2018-08-09 
 * @function   :Draw a triangle at a specified position
 * @parameters :x0:the bebinning x coordinate of the triangular edge 
                y0:the bebinning y coordinate of the triangular edge 
                                x1:the vertex x coordinate of the triangular
                                y1:the vertex y coordinate of the triangular
                                x2:the ending x coordinate of the triangular edge 
                                y2:the ending y coordinate of the triangular edge 
 * @retvalue   :None
******************************************************************************/ 
void OLED_DrawTriangle(u8 x0,u8 y0,u8 x1,u8 y1,u8 x2,u8 y2)
{
    OLED_DrawLine(x0,y0,x1,y1);
    OLED_DrawLine(x1,y1,x2,y2);
    OLED_DrawLine(x2,y2,x0,y0);
}

static void _swap(u8 *a, u8 *b)
{
    u8 tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

/*****************************************************************************
 * @name       :void OLED_DrawFillRectangleTriangle(u8 x0,u8 y0,u8 x1,u8 y1,u8 x2,u8 y2)
 * @date       :2018-08-09 
 * @function   :filling a triangle at a specified position
 * @parameters :x0:the bebinning x coordinate of the triangular edge 
                y0:the bebinning y coordinate of the triangular edge 
                                x1:the vertex x coordinate of the triangular
                                y1:the vertex y coordinate of the triangular
                                x2:the ending x coordinate of the triangular edge 
                                y2:the ending y coordinate of the triangular edge 
 * @retvalue   :None
******************************************************************************/ 
void OLED_DrawFillTriangle(u8 x0,u8 y0,u8 x1,u8 y1,u8 x2,u8 y2)
{
    u8 a, b, y, last;
    int dx01, dy01, dx02, dy02, dx12, dy12;
    long sa = 0;
    long sb = 0;
     if (y0 > y1) 
    {
    _swap(&y0,&y1); 
        _swap(&x0,&x1);
     }
     if (y1 > y2) 
    {
    _swap(&y2,&y1); 
        _swap(&x2,&x1);
     }
  if (y0 > y1) 
    {
    _swap(&y0,&y1); 
        _swap(&x0,&x1);
  }
    if(y0 == y2) 
    { 
        a = b = x0;
        if(x1 < a)
    {
            a = x1;
    }
    else if(x1 > b)
    {
            b = x1;
    }
    if(x2 < a)
    {
            a = x2;
    }
        else if(x2 > b)
    {
            b = x2;
    }
        OLED_DrawFillRectangle(a,y0,b,y0);
    return;
    }
    dx01 = x1 - x0;
    dy01 = y1 - y0;
    dx02 = x2 - x0;
    dy02 = y2 - y0;
    dx12 = x2 - x1;
    dy12 = y2 - y1;
    
    if(y1 == y2)
    {
        last = y1; 
    }
  else
    {
        last = y1-1; 
    }
    for(y=y0; y<=last; y++) 
    {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
    sb += dx02;
    if(a > b)
    {
        _swap(&a,&b);
    }
    OLED_DrawFillRectangle(a,y,b,y);
    }
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) 
    {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if(a > b)
        {
            _swap(&a,&b);
        }
        OLED_DrawFillRectangle(a,y,b,y);
    }
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(u8 x, u8 y, u8 ch[], u8 TextSize)
// Calls          : 
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); ch[] -- Ҫ��ʾ���ַ���; TextSize -- �ַ���С(1:6*8 ; 2:8*16)
// Description    : ��ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
//--------------------------------------------------------------
void OLED_ShowStr(u8 x, u8 y, char* ch, u8 TextSize)
{
    u8 c = 0,i = 0,j = 0;
    switch(TextSize)
    {
        case 1:
        {
            while(*(ch+j) != '\0')
            {
                c = *(ch+j) - 32;
                if(x > 122)
                {
                    x = 0;
                    y++;
                }
//                OLED_SetPos(x,y);
                for(i=0;i<6;i++)
//                    WriteDat(F6x8[c][i]);
                    OLED_WriteBuff(x++, y, F6x8[c][i]);
                j++;
            }
        }break;
        case 2:
        {
            while(*(ch+j) != '\0')
            {
                c = *(ch+j) - 32;
                if(x > 120)
                {
                    x = 0;
                    y++;
                }
//                OLED_SetPos(x,y);
                for(i=0;i<8;i++)
//                    WriteDat(F8X16[c*16+i]);
                    OLED_WriteBuff(x+i, y, F8X16[c*16+i]);
//                OLED_SetPos(x,y+1);
                for(i=0;i<8;i++)
//                    WriteDat(F8X16[c*16+i+8]);
                    OLED_WriteBuff(x+i, y+1, F8X16[c*16+i+8]);
                x += 8;
                j++;
            }
        }break;
    }
}
void OLED_LeftShowStr(u8 y, char* ch, u8 TextSize)
{
    OLED_ShowStr(0, y, ch, TextSize);
}
void OLED_RightShowStr(u8 y, char* ch, u8 TextSize)
{
    u8 x = 0;
    u8 temp = 0;
    switch(TextSize)
    {
        case 1:
        {
            while(ch[temp++] != 0);
            if (temp >= 21)temp = 21;
            x = 128 - temp * 6; //one char take up 6 pixel, and get its half length
        }break;
        case 2:
        {
            while(ch[temp++] != 0);
            if (temp >= 16) temp = 16;
            x = 128 - temp * 8; //one char take up 8 pixel, and get its half length
        }break;
    }
    OLED_ShowStr(x, y, ch, TextSize);
}


void OLED_CenterShowStr(u8 y, char* ch, u8 TextSize)
{
    u8 x = 0;
    u8 temp = 0;
    switch(TextSize)
    {
        case 1:
        {
            while(ch[temp++] != 0);
            if (temp >= 21) temp = 21;
            x = 64 - temp * 6 / 2; //one char take up 6 pixel, and get its half length
        }break;
        case 2:
        {
            while(ch[temp++] != 0);
            if (temp >= 16) temp = 16;
            x = 64 - temp * 8 / 2; //one char take up 8 pixel, and get its half length
        }break;
    }
    OLED_ShowStr(x, y, ch, TextSize);
}


