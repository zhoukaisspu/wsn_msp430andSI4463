#include "PHYlib.h"
#include "DriveLCD.h"

#define CPU_F ((double)8000000)
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

void Send_command(unsigned char command_data)//向LCD12232传送指令的函数
{       
    unsigned char i;
    unsigned char i_data;
    i_data=0xf8;
    C_S_1
    S_CLK_0
    for(i=0;i<8;i++)
      {
          LCDOUT &=~SID;
          LCDOUT|=(i_data&0x80);//0x80
          S_CLK_0
          S_CLK_1
          i_data=i_data<<1;
      }
    i_data=command_data;
    i_data&=0xf0;
    for(i=0;i<8;i++)
      {
          LCDOUT &=~0x80;
          LCDOUT|=(i_data&0x80);
          S_CLK_0
          S_CLK_1
          i_data=i_data<<1;
      }
    i_data=command_data;
    i_data&=0x0f;
    i_data<<=4;
    for(i=0;i<8;i++)
      {
          LCDOUT &=~0x80;
          LCDOUT|=(i_data&0x80);
          S_CLK_0
          S_CLK_1
          i_data=i_data<<1;
      }
    C_S_0
    delay_ms(10);
}

void Send_data(unsigned char command_data)
{       
    unsigned char i;
    unsigned char i_data;
    i_data=0xfa;
    C_S_1
    S_CLK_0
    for(i=0;i<8;i++)
      {
          LCDOUT &=~0x80;
          LCDOUT|=(i_data&0x80);
          S_CLK_0
          S_CLK_1
          i_data=i_data<<1;
      }
    i_data=command_data;
    i_data&=0xf0;
    for(i=0;i<8;i++)
      {
          LCDOUT &=~0x80;
          LCDOUT|=(i_data&0x80);
          S_CLK_0
          S_CLK_1
          i_data=i_data<<1;
      }
    i_data=command_data;
    i_data&=0x0f;
    i_data<<=4;
    for(i=0;i<8;i++)
      {
          LCDOUT &=~0x80;
          LCDOUT|=(i_data&0x80);
          S_CLK_0
          S_CLK_1
          i_data=i_data<<1;
      }
    C_S_0
    delay_ms(10);
}

void Lcd12232_init()
{
    LCDSEL=0x00;
    LCDDIR=0xff;       //初始化LCD12232用到的I/O使其为输出状态
    delay_ms(100);
    Send_command(0x30);//设置8位数据接口，基本指令模式
    Send_command(0x02);//清DDRAM
    Send_command(0x06);//游标及显示右移一位
    Send_command(0x0c);//整体显示开，游标关
    Send_command(0x01);//清空屏幕
    Send_command(0x80);//设定首次显示位置
    
    Send_command(0x80);
}

void Locate(unsigned char hang,unsigned char lie)
{       
    if(hang==1)
        Send_command(0x80+lie);
    if(hang==2)
        Send_command(0x90+lie);
}

void Write_string(unsigned char *p)
{
     for(;*p!='\0';p++)
        Send_data(*p);
}
