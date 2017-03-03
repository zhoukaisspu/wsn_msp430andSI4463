#ifndef _DriveLCD_H 
#define _DriveLCD_H
#include "PHYlib.h"
#define LCDOUT P4OUT
#define LCDDIR P4DIR
#define LCDSEL P4SEL
//LED4.7 --> SID
//LED4.6 --> CS
//LED4.5 --> SCLK

#define SID BIT7
#define CS BIT6
#define SCLK BIT5

#define C_S_1 LCDOUT |=CS;
#define C_S_0 LCDOUT &=~CS;
#define S_CLK_0 LCDOUT&=~SCLK;
#define S_CLK_1 LCDOUT|=SCLK;

void Send_command(unsigned char command_data);
void Send_data(unsigned char command_data);
void Lcd12232_init(void);
void Locate(unsigned char hang,unsigned char lie);//�У���1��ʼ���д�0��ʼ
void Write_string(unsigned char *p);//��Ҫ������a[]���׵�ַ����Ӧ����������a
#endif