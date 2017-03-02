#include <msp430x54xA.h>

#include "RFlib.h"

//系统子系统时钟频率
#define PHY_SMCLK_4MHZ   1
#define PHY_SMCLK_4200KHZ   2
#define PHY_SMCLK_8MHZ   3

//SMCLK驱动
#define PHY_SMCLK_DCO   1
#define PHY_SMCLK_XT2   2

//#define PHY_FLLN      122       //DCO倍频 4MHz
#define PHY_FLLN      127         //DCO倍频 4.2MHz

#define PHY_ACLK      32768     //辅助时钟

//#define PHY_MCLK      4030464   // MCLK=((PHY_FLLN+1)*32768)=4030464
//#define PHY_MCLK_1MS  3936      // ((PHY_FLLN+1)*32768)/3936 = 1024
#define PHY_MCLK      4194304     // MCLK=((PHY_FLLN+1)*32768)=4194304
#define PHY_MCLK_1MS  4096        // ((PHY_FLLN+1)*32768)/4096 = 1024

#define PHY_ACLK_1MS  32        // 32768/32 = 1024

#define EN_XT2CLK 1 //开启XT2时钟 
#define DIS_XT2CLK 0//关闭XT2时钟

//XT2时钟开启标志
extern unsigned char g_uchPHYXt2Flag;

//SMCLK驱动时钟选择
extern unsigned char g_uchPHYSMclkSel;
//SMCLK时钟频率设定
extern unsigned char g_uchPHYSMclkSet;


//定时器变量
extern unsigned int g_uPHYTimer0A0Count;                     //用于三个定时器定时次数的设定
extern unsigned int g_uPHYTimer1A0Count;
extern unsigned int g_uPHYTimerBCount;


//定时器中断标志
extern unsigned char g_chPHYTimerBFlag ; 
extern unsigned int g_uPHYTimerBAppCount;    //应用层计数 中断次数
extern unsigned int g_uPHYTimerBDLLCount ;    //链路层计数 中断次数
extern unsigned int g_uPHYTimerB_TimeElapse; //注册设备自上次注册到现在的逝去时间

//IO中断标志
extern unsigned char g_uchPHYIntP14;//端口P1.4中断标志:0 ：无中断；1：有中断

//比较器TA0参数
extern unsigned int TA0StartTime;
extern unsigned int TA0EndTime;
extern unsigned char TA0Overflow; 
extern unsigned char TA0Flag;
extern unsigned long  TA0TimeCount;

extern unsigned char g_uchPHYWDFlag; //防止程序跑飞而设定的看门狗打开标记，0 ：关闭；1：打开
extern unsigned int g_uPHYWDTimeHead; //防止程序跑飞而设定的看门狗时间头
extern unsigned int g_uPHYWDTimeTail; //防止程序跑飞而设定的看门狗时间尾

#define PHY_SLEEP_QUIT   0     //无休眠
#define PHY_SLEEP_MODE3  1     //低功耗模式3 休眠
#define PHY_SLEEP_FULL   2     //全休眠
extern unsigned char g_uchPHYSleepMode;   //休眠模式 0表示未休眠

extern unsigned int g_uPHYSleppTicks  ;  //修眠的时钟次数


//定义采用哪个UART
#define UARTA0  1
#define UARTA1  2
//#define UARTA2  3
#define UARTA3  4
extern unsigned char g_uchPHYUART0Open ;//UART0打开标记0:开，1：关
extern unsigned char g_uchPHYUART1Open ;//UART1打开标记0:开，1：关
extern unsigned char g_uchPHYUART2Open ;//UART2打开标记0:开，1：关
extern unsigned char g_uchPHYUART3Open ;//UART3打开标记0:开，1：关


extern unsigned char g_uchPHYPAType;  //物理层全局变量，设备功放类型（A型或B型）A型：0x0a; B型：0x0b

extern unsigned int g_uPHYBattPower;  //电池电量
extern unsigned int g_uPHYChargePower; //充电电压
extern unsigned int g_uPHYBoardPower; //板极电压


/***********************************************/
/*          定义FLASH存储器B段地址             */
/***********************************************/
//#define FLASH_ADDRESS 0x01000
#define FLASH_ADDRESS 0x01800

/***********************************************/
/*                   Baud rate settings        */
/*          串口通讯波特率设置定义             */
/***********************************************/
#define Baud_2400  0x00
#define Baud_4800  0x01
#define Baud_9600  0x02
#define Baud_19200  0x03
//#define Baud_38400  0x04
#define Baud_57600  0x05
#define Baud_115200  0x07


/********************************************
相应管脚的寄存器名称设置，方便编程
470通用板两个串口都需要开关设置
*********************************************/
/*============================================
470通用板串口  USCI A0 开关控制管脚为 P5.4
************电源控制管脚错误****************
摄像头的串口转接除
基站的串口
=============================================*/
/*
//232A0口 p4.7
#define PHY_232A0_PxSEL        P4SEL
#define PHY_232A0_PxOUT        P4OUT
#define PHY_232A0_PxDIR        P4DIR
#define PHY_232A0_PxIN         BIT7
*/

/*ZK*/

//对应的控制管脚
//232A0口P5.4
#define PHY_232A0_PxSEL        P5SEL
#define PHY_232A0_PxOUT        P5OUT
#define PHY_232A0_PxDIR        P5DIR
#define PHY_232A0_PxIN         BIT4

/*======================================
470通用板的一路串口USCI A3
=========================================*/
//232A3口 p11.1    
#define PHY_232A3_PxSEL        P11SEL
#define PHY_232A3_PxOUT        P11OUT
#define PHY_232A3_PxDIR        P11DIR
#define PHY_232A3_PxIN         BIT1

//USCIA3工作方式（232/TTL）选择口 P4.6
//高电平232模式，低电平TTL模式
#define PHY_232A3SEL_PxSEL        P4SEL
#define PHY_232A3SEL_PxDIR        P4DIR
#define PHY_232A3SEL_PIN          P4IN
#define PHY_232A3SEL_PxIN         BIT6
/*======================================
红灯与绿灯只需要设置输出方向和高低电平
========================================*/
//LED 红P2.2  绿P2.1
#define PHY_LEDG_PxOUT        P2OUT      // ==输出电平的定义
#define PHY_LEDG_PxDIR        P2DIR      //==输出或输入方向的定义
#define PHY_LEDG_PxIN         BIT1       //==红灯管脚定义

#define PHY_LEDR_PxOUT        P2OUT
#define PHY_LEDR_PxDIR        P2DIR
#define PHY_LEDR_PxIN         BIT2


/*======================================
//闪光灯控制  控制充电板上的AAT4280 P4.3
双图像里面要在适当的时候控制AAT4280管脚
的开关来控制闪光灯的供电，也只需要设置输出
高低电平
========================================*/
#define PHY_FLASH_LED_PxDIR   P4DIR 
#define PHY_FLASH_LED_PxOUT   P4OUT
#define PHY_FLASH_LED_PxIN    BIT3


/*======================================
蜂鸣器的控制P2.3
========================================*/
//蜂鸣器控制口 P2.3
#define PHY_BEEP_PxOUT        P2OUT
#define PHY_BEEP_PxDIR        P2DIR
#define PHY_BEEP_PxIN         BIT3



/*======================================
通过电池供电，电压转换模块，电源开关控制
AAT3221 IC2
USCIA1的TTL通信
P10.1
=========================================*/
//USCIA1外接设备的电源控制口2 P10.1
#define PHY_DEV_POWER2_PxSEL        P10SEL
#define PHY_DEV_POWER2_PxOUT        P10OUT
#define PHY_DEV_POWER2_PxDIR        P10DIR
#define PHY_DEV_POWER2_PxIN         BIT1

/*======================================
通过电池供电，电压转换模块，电源开关控制
AAT3221 IC3
P10.2
=========================================*/
//USCIB0/AD外接设备的电源控制口3 P10.2
#define PHY_DEV_POWER3_PxSEL        P10SEL
#define PHY_DEV_POWER3_PxOUT        P10OUT
#define PHY_DEV_POWER3_PxDIR        P10DIR
#define PHY_DEV_POWER3_PxIN         BIT2


/*======================================
470通用板AAT4280的电源供应
控制摄像头的电源供应 P10.3
=========================================*/
//用户的电源控制口4 P10.3
#define PHY_DEV_POWERUSR_PxSEL        P10SEL
#define PHY_DEV_POWERUSR_PxOUT        P10OUT
#define PHY_DEV_POWERUSR_PxDIR        P10DIR
#define PHY_DEV_POWERUSR_PxIN         BIT3

unsigned int  AdRandom(unsigned char time);
/**********************************************************************/                                         
/*       根据宏定义选择不同的时钟系统                                */
/* PHY_SMCLK_DCO:DCO= MCLK = SMCLK ,XT1CLK=ACLK ,    关闭 XT2        */
/* PHY_SMCLK_XT2:DCO =MCLK=4MHz,XT2 =SMCLK,xt2=ACLK                  */
/* 默认定义PHY_SMCLK_DCO                                             */
/**********************************************************************/
extern void InitClk(void);

/**********************************************************************/										 
/*	输出时钟：P11.0 ACLK；P11.1 MCLK；P11.2 SMCLK		     */
/**********************************************************************/
extern void OutPutClk(void);

/**********************************************************************/                                         
/*           利用系统主时钟系统软件延时1/1024秒                       */
/**********************************************************************/
extern void PHYMCLKSoftDelay1ms(void);

/**********************************************************************/										 
/*		利用系统主时钟系统软件延时1/2048秒,约为0.5ms	      */
/**********************************************************************/
extern void PHYMCLKSoftDelayHalfms(void);

/**********************************************************************/										 
/*		利用系统主时钟系统软件延时1/4096秒,约为244us	      */
/**********************************************************************/
extern void PHYMCLKSoftDelay250us(void);

/****************************************************************************/
/*  判断设备是A型（无功放）还是B型（有功放）。若是B型，返回0；否则返回－1   */
/****************************************************************************/
extern int DevHasPAOrNot(void);

/****************************************************************************/
/*  Set RS232A0 control    设置对外RS232A0通讯芯片控制                          */
/*  用到的MCU管脚:      P4.7       OK                                        */
/****************************************************************************/
//打开接收RS232A0
extern void EnableRs232A0(void);
//关闭RS232A0接口
extern void DisableRs232A0(void);

/****************************************************************************/
/*  Set RS232A3 control    设置对外RS232A3通讯芯片控制                          */
/*  用到的MCU管脚:      P11.1        OK                                       */
/****************************************************************************/
//打开接收RS232A3
extern void EnableRs232A3(void);
//关闭RS232A3接口
extern void DisableRs232A3(void);

/****************************************************************************/
/*  Set Green light   control  控制 Green LED                               */
/*  用到的MCU管脚 :   P2.1      OK                                           */
/*  控制作用      :   控制绿灯，Off关闭，On开启                             */
/****************************************************************************/
//开启绿灯
extern void LedGreenOn(void);
//关闭绿灯
extern void LedGreenOff(void);

/****************************************************************************/
/*  Set Red light  control  控制Red LED                                     */
/*  用到的MCU管脚 :   P2.2          OK                                       */
/*  控制作用      :   控制红灯，Off关闭，On开启                             */
/****************************************************************************/
//开启红灯
extern void LedRedOn(void);
//关闭红灯
extern void LedRedOff(void);

/****************************************************************************/
/*	Set flash light  control	控制flash light 			*/
/*	用到的MCU管脚 :   P4.3	                 OK			*/
/*	控制作用	  :   控制充电板上的AAT4280，开关控制闪光灯 		*/
/****************************************************************************/
//开启闪光灯
extern  void FlashLightOn(void);
//关闭闪光灯
extern  void FlashLightOff(void);

/****************************************************************************/
/*  蜂鸣器驱动 ,用到P2.3    ==OK 蜂鸣器的开启与关闭都具有                                                   */
/****************************************************************************/
extern void Sound();

/****************************************************************************/
/*  图像出错的蜂鸣器的驱动，两者响不同的时间 ,用到P2.3    ======ZK                                                 */
/****************************************************************************/
extern void PhotoerrorSound();

/****************************************************************************/
/*  Set power 1 control  USCIA1外接设备电源开关控制      3221               */
/*  用到的MCU管脚 :   P10.1                                                 */
/****************************************************************************/
extern void DevPower2On(void);
extern void DevPower2Off(void);

/****************************************************************************/
/*  Set power 3 control     USCIB0/AD外接设备的电源控制    3221             */
/*  用到的MCU管脚 :   P10.2                                                 */
/****************************************************************************/
extern void DevPower3On(void);
extern void DevPower3Off(void);

/****************************************************************************/
/*  Set power USR control     用户外接设备的电源控制    4280                */
/*  用到的MCU管脚 :   P10.3                                                 */
/****************************************************************************/
extern void DevPowerUsrOn(void);
extern void DevPowerUsrOff(void);

/****************************************************************************/
/*               电源管理函数库；主电源默认打开。                           */
/*    单片机电源管理接口初始化；默认关闭其它用电器电源；                    */
/****************************************************************************/
extern void InitPower(void);

/****************************************************************************/
 /*              低功耗模式下，没有用到的管脚的初始化                      */
/*             对整个系统中没有用到的管脚全部设置为高阻状态                */
/****************************************************************************/
extern void LowPowerPinInit(void);

/****************************************************************************/
/*  Set UARTa0 control          串口A0波特率设置                            */
/*  用到的MCU管脚      :  P3.4,P3.5 UART模块功能                            */
/*  函数参数           : char bps                                           */
/*  可选的波特率设置   : Baud_2400，Baud_4800，Baud_9600，Baud_19200，
Baud_57600，Baud_115200                */
/****************************************************************************/
extern void OpenUartA0(char bps);

/****************************************************************************/
/*  关闭UART0口                                                             */
/****************************************************************************/
extern void CloseUartA0(void);

/****************************************************************************/
/*  Set UARTa1 control          串口A1波特率设置                            */
/*  用到的MCU管脚      :  P5.6,P5.7 UART模块功能                            */
/*  函数参数           : char bps                                           */
/*  可选的波特率设置   : Baud_2400，Baud_4800，Baud_9600，Baud_19200，
Baud_57600，Baud_115200                */
/****************************************************************************/
extern void OpenUartA1(char bps);

/****************************************************************************/
/*  关闭UART0口                                                             */
/****************************************************************************/
extern void CloseUartA1(void);

/****************************************************************************/
/*  Set SPI主A1 control          A1用于SPI主机通讯                          */
/*  3线，8比特，时钟极性为高电平无效，先发送MSB,通讯时钟为MCLK/2            */
/*  用到的MCU管脚      :  P5.6 P5.7的SPI模块功能                             */
/****************************************************************************/
extern void OpenMasterSpiA1(void);

/****************************************************************************/
/*  关闭SPI A1口                                                             */
/****************************************************************************/
extern void CloseSpiA1(void);

/****************************************************************************/
/*  Set UARTA2 control          串口A2波特率设置                            */
/*  用到的MCU管脚      :  P9.4,P9.5 UART模块功能                            */
/*  函数参数           : char bps                                           */
/*  可选的波特率设置   : Baud_2400，Baud_4800，Baud_9600，Baud_19200，
Baud_57600，Baud_115200                */
/****************************************************************************/
extern void OpenUartA2(char bps);

/****************************************************************************/
/*  关闭UART2口                                                             */
/****************************************************************************/
extern void CloseUartA2(void);

/****************************************************************************/
/*  Set SPI主A2 control          A2用于SPI主机通讯                          */
/*  3线，8比特，时钟极性为高电平无效，先发送MSB,通讯时钟为MCLK/2            */
/*  用到的MCU管脚      :  P9.4,P9.5 SPI模块功能                             */
/****************************************************************************/
extern void OpenMasterSpiA2(void);
/****************************************************************************/
/*  关闭SPI A2口                                                             */
/****************************************************************************/
extern void CloseSpiA2(void);

/****************************************************************************/
/*  Set UARTA3 control          串口A3波特率设置                            */
/*  用到的MCU管脚      :  P10.4,P10.5 UART模块功能                            */
/*  函数参数           : char bps                                           */
/*  可选的波特率设置   : Baud_2400，Baud_4800，Baud_9600，Baud_19200，
Baud_57600，Baud_115200                */
/****************************************************************************/
extern void OpenUartA3(char bps);

/****************************************************************************/
/*  关闭UART3口                                                             */
/****************************************************************************/
extern void CloseUartA3(void);

/****************************************************************************/
/*  初始化定时器A0 用系统辅助时钟 ACLK = 32768Hz   CCR0 = 32768            */
/****************************************************************************/
extern void SetTimer0A0_ACLK(void);

/****************************************************************************/
/*                    Stop Timer_A0    停止计数                             */
/****************************************************************************/
extern void StopTimer0A0(void);

/****************************************************************************/
/*  Set Timer_A0 delay time    设置定时时间                                 */
/*  参数          : long timeset                                            */
/*  定时          : (timeset / 时钟频率) ×1000 ms                          */
/****************************************************************************/
extern void  Timer0A0_Delay(long timeSet);

/****************************************************************************/
/*             用系统辅助时钟 定时1/1024秒钟                                */
/****************************************************************************/
extern void Timer0A0_Delay_1ms_ACLK(void);

/****************************************************************************/
/*             用系统辅助时钟 定时1秒钟                                     */
/****************************************************************************/
extern void Timer0A0_Delay_1s_ACLK(void);

/****************************************************************************/
/*  初始化定时器A1 用系统辅助时钟 ACLK = 32768Hz   CCR0 = 32768            */
/****************************************************************************/
extern void SetTimer1A0_ACLK(void);

/****************************************************************************/
/*                    Stop Timer_A1    停止计数                             */
/****************************************************************************/
extern void StopTimer1A0(void);

/****************************************************************************/
/*  Set Timer_A1 delay time    设置定时时间                                 */
/*  参数          : long timeset                                            */
/*  定时          : (timeset / 时钟频率) ×1000 ms                          */
/****************************************************************************/
extern void  Timer1A0_Delay(long timeSet);

/****************************************************************************/
/*             用系统辅助时钟 定时1/1024秒钟                                */
/****************************************************************************/
extern void Timer1A0_Delay_1ms_ACLK(void);

/****************************************************************************/
/*             用系统辅助时钟 定时1秒钟                                     */
/****************************************************************************/
extern void Timer1A0_Delay_1s_ACLK(void);

/****************************************************************************/
/*  初始化定时器TimerB 用系统辅助时钟 ACLK = 32768Hz   CCR0 = 32768 ;       */
/****************************************************************************/
extern void SetTimerB_ACLK(void);

/****************************************************************************/
/*                    Stop Timer_B    停止计数                              */
/****************************************************************************/
extern void StopTimerB(void);

/****************************************************************************/
/*  Set Timer_B delay time    设置定时时间                                  */
/*  参数          : long timeset                                            */
/*  定时          : (timeset / 时钟频率) ×1000 ms                            */
/****************************************************************************/
extern void  TimerB_Delay(long timeSet);

extern void TimerB_Delay_1ms_ACLK();
extern void TimerB_Delay_1s_ACLK();

/****************************************************************************/
/*是函数ADCGetSingleChSingleData的扩展，添加多次采集失败后RESET             */
/*函数参数:                                                                 */
/*        unsigned char AdSet: 0~7,12~15对应需要采样的AD口A0~A7,A12~A15	    */
/*        unsigned char Ref:选择参考源。                                    */
/*                1---内部2.5V；2---外部电源；其他---内部1.5V               */
/*函数返回值:                                                               */
/*        unsigned int 返回采集到的数据                                     */
/****************************************************************************/
extern unsigned int ADCGetSingleChSingleDataEx(unsigned char AdSet,unsigned char Ref);

/****************************************************************************/
/*                A0口   检测板级电源电压，采样值以 unsigned int返回               */
/****************************************************************************/
extern unsigned int BoardEnergyDetect(void);

/****************************************************************************/
/*              A1口     检测电池电源电压，采样值以 unsigned int返回               */
/****************************************************************************/
extern unsigned int BattEnergyDetect(void);

/****************************************************************************/
/*              A2口  检测充电电源电压，采样值以 unsigned int返回               */
/****************************************************************************/
extern unsigned int ChargerEnergyDetect(void);

/****************************************************************************/
/*	  系统进入低功耗模式3													*/
/*unsigned char RFmode 设置RF休眠模式*/
/****************************************************************************/
extern void EnterLowPowerMode3(unsigned char RFmode);

/****************************************************************************/
/* 低功耗模式3 豪秒级休眠													*/
/* 由时钟ACLK= 32768Hz 控制的 毫秒级休眠 ，参数 unsigned int ms 休眠的毫秒数*/
/*unsigned char RFmode 设置RF休眠模式*/
/****************************************************************************/
extern void Sleep_Mode3_ACLKms(unsigned int ms, unsigned char RFmode);


/****************************************************************************/
/* 低功耗模式3 秒级休眠 												   */
/*由时钟ACLK= 32768Hz 控制的 秒级休眠 ，参数 unsigned int s 休眠的秒数	   */
/*unsigned char RFmode 设置RF休眠模式*/
/****************************************************************************/
extern void Sleep_Mode3_ACLKs(unsigned int s,unsigned char RFmode);

/****************************************************************************/
/* 向Data_ptr指向的FLASH里面写入一个字 word                                 */
/****************************************************************************/
extern void Flash_ww(unsigned int *Data_ptr,unsigned int word);

/****************************************************************************/
/* 向Data_ptr指向的FLASH里面写入一个字节byte                                 */
/****************************************************************************/
extern void Flash_wb(unsigned char *Data_ptr,unsigned char byte);

/****************************************************************************/
/* 清除Data_ptr指向的FLASH里面的字                                          */
/****************************************************************************/
extern void Flash_clr(unsigned int *Data_ptr);