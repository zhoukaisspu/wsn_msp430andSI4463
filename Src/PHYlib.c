#include "PHYlib.h"
#include "global.h"

//XT2时钟开启标志
unsigned char g_uchPHYXt2Flag = DIS_XT2CLK;    //#define DIS_XT2CLK 0 定义XT2时钟是否已经打开   

//SMCLK时钟选择
unsigned char g_uchPHYSMclkSel = PHY_SMCLK_DCO;     //SMCLK可以选择XT2，DCO

//SMCLK时钟频率设定
unsigned char g_uchPHYSMclkSet = PHY_SMCLK_4MHZ;

//定时器变量
unsigned int g_uPHYTimer0A0Count = 0;
unsigned int g_uPHYTimer1A0Count = 0;
unsigned int g_uPHYTimerBCount = 0;

unsigned int g_uPHYTimerB_TimeElapse = 0; //注册设备自上次注册到现在的逝去时间

//定时器中断标志
unsigned char g_chPHYTimerBFlag =0 ; 
unsigned int g_uPHYTimerBAppCount=0;    //应用层计数 中断次数
unsigned int g_uPHYTimerBDLLCount=0 ;   //链路层计数 中断次数

unsigned char g_uchPHYSleepMode = PHY_SLEEP_QUIT;
unsigned int g_uPHYSleppTicks  = 0xffff;  //修眠的时钟次数

//IO中断标志
unsigned char g_uchPHYIntP14 = 0;//端口P1.4中断标志:0 ：无中断；1：有中断

//比较器TA0参数
unsigned int TA0StartTime = 0;
unsigned int TA0EndTime = 0;
unsigned char TA0Overflow = 0; 
unsigned char TA0Flag = 0;
unsigned long  TA0TimeCount = 0;//总计数值

unsigned char g_uchPHYWDFlag = 0; //防止程序跑飞而设定的看门狗打开标记，0 ：关闭；1：打开
unsigned int g_uPHYWDTimeHead = 0; //防止程序跑飞而设定的看门狗时间头
unsigned int g_uPHYWDTimeTail = 0; //防止程序跑飞而设定的看门狗时间尾

unsigned char g_uchPHYUART0Open = 0 ;//UART0打开标记1:开，0：关
unsigned char g_uchPHYUART1Open = 0 ;//UART1打开标记1:开，0：关
unsigned char g_uchPHYUART2Open = 0 ;//UART2打开标记1:开，0：关
unsigned char g_uchPHYUART3Open = 0 ;//UART3打开标记1:开，0：关


unsigned char g_uchPHYPAType = 0;	 //设备类型，默认为B型

unsigned int g_uPHYBattPower = 0;  //电池电量
unsigned int g_uPHYChargePower = 0; //充电电压
unsigned int g_uPHYBoardPower = 0; //板极电压

/**********************************************************************/										 
/*初始化系统时钟DCO= MCLK = SMCLK ,XT1CLK=ACLK ,	关闭 XT2		  */
/**********************************************************************/
void SetDCOClk(void)
{  
	P5SEL &= ~0x0C; 			 // Port Dis-select XT2
	P7SEL |= 0x03;				// Port select XT1
	
	UCSCTL6 &= ~XT1OFF; 	         // XT1 ON  
	UCSCTL6 |= XT2OFF;		 // Set XT2 Off
	
	UCSCTL6 |= (XT1DRIVE_3);//XT1 drive strength
	UCSCTL6 &= ~(XT2DRIVE_3);//XT2 drive strength  
	
	UCSCTL3 = SELREF_0; 	  // Set DCO FLL reference =xt1
	
	UCSCTL0 = 0x00; 			 // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_4;		 // Select DCO range
	UCSCTL2 = PHY_FLLN; 		 // Set DCO Multiplier for 4MHz
	// (N + 1) * FLLRef = Fdco
	// (122 + 1) * 32768 = 4030464 Hz
	
	//Set SMCLK =DCOCLK
	//Set MCLK = DCOCLK
	//Set ACLK = xt1
	UCSCTL4 = SELS_3 + SELM_3 + SELA_0; 
	
	do
	{
		UCSCTL7 &= ~(XT1LFOFFG +XT2OFFG +DCOFFG);
		// Clear XT2,XT1 default flags
		SFRIFG1 &= ~OFIFG;						// Clear fault flags
		for(unsigned int i=0;i<0xFFFF;i++); 	// Delay for Osc to stabilize
	}while (SFRIFG1&OFIFG); 				  // Test oscillator fault flag
	
	UCSCTL6 &= ~(XT1DRIVE_3);	   // Xtal is now stable, reduce drive strength
	
	g_uchPHYXt2Flag = DIS_XT2CLK;
}

/**********************************************************************/										 
/*初始化系统时钟DCO= MCLK = SMCLK ,REFO=ACLK ,	  关闭XT1和XT2	*/
/**********************************************************************/
/*
void InitClk(void)
{
   WDTCTL = WDTPW+WDTHOLD; 				  // Stop WDT  

  P5SEL &= ~0x0C;			   // Port Dis-select XT2
  P7SEL &= ~0x03;			   // Port Dis-select XT1
  
	UCSCTL6 |= XT1OFF;		 // XT1 Off  
	UCSCTL6 |= XT2OFF;		 // Set XT2 Off
	UCSCTL6 &= ~(XT1DRIVE_3 + XT2DRIVE_3);//Lowest current consumption
	
	  UCSCTL3 = SELREF_2;		// Set DCO FLL reference = REFO
	  
		//ACLK = REFO = 32kHz, MCLK = SMCLK =DCO= 4MHz	
		UCSCTL0 = 0x00; 			 // Set lowest possible DCOx, MODx
		UCSCTL1 = DCORSEL_4;		 // Select DCO range
		UCSCTL2 = PHY_FLLN; 		 // Set DCO Multiplier for 4MHz
		// (N + 1) * FLLRef = Fdco
		// (122 + 1) * 32768 = 4030464 Hz
		
		  //Set SMCLK =DCOCLK
		  //Set MCLK = DCOCLK
		  //Set ACLK = REFO
		  UCSCTL4 = SELS_3 + SELM_3 + SELA_2; 
		  
			do
			{
			UCSCTL7 &= ~(XT1LFOFFG +XT2OFFG +DCOFFG);
			// Clear XT2,XT1 default flags
			SFRIFG1 &= ~OFIFG;						// Clear fault flags
			for(unsigned int i=0;i<0xFFFF;i++); 	// Delay for Osc to stabilize
			}while (SFRIFG1&OFIFG); 				  // Test oscillator fault flag
			
			  g_uchPHYXt2Flag = DIS_XT2CLK;
			  }
*/

/**********************************************************************/										 
/*		  设置系统时钟DCO =MCLK=4MHz,XT2 =SMCLK,xt2=ACLK			 */
/**********************************************************************/
void SetXT2ClkOn(void)
{  
	//如果XT2已经开启，则退出
	if(g_uchPHYXt2Flag == EN_XT2CLK )
		return;
	
	P5SEL |= 0x0C;				// Port select XT2
	P7SEL |= 0x03;				// Port select XT1
	
	UCSCTL6 &= ~XT1OFF; 	  // XT1 ON  
	UCSCTL6 &= ~XT2OFF; 	  // XT2 On  XT2 enabled in active mode (AM) through LPM4.
	
	UCSCTL6 |= (XT1DRIVE_3);//XT1 drive strength
	UCSCTL6 &= ~(XT2DRIVE_3);//XT2 drive strength  
	
	UCSCTL3 = SELREF_0; 	  // Set DCO FLL reference =xt1
	
	UCSCTL0 = 0x00; 			 // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_4;		 // Select DCO range
	UCSCTL2 = PHY_FLLN; 		 // Set DCO Multiplier for 4MHz
	// (N + 1) * FLLRef = Fdco
	// (122 + 1) * 32768 = 4030464 Hz
        //(127+1)*32768=4194304HZ   //ZK 在这里N设置成为了127
	
	//Set SMCLK =XT2CLK when available, otherwise DCOCLKDIV
	//Set MCLK = DCOCLK
	//Set ACLK = XT1
	UCSCTL4 = SELS_5 + SELM_3 + SELA_0;
	
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG+ DCOFFG );
		// Clear XT2,XT1 default flags
		SFRIFG1 &= ~OFIFG;						// Clear fault flags
		for(unsigned int i=0;i<0xFFFF;i++); 	// Delay for Osc to stabilize
	}while (SFRIFG1&OFIFG); 				  // Test oscillator fault flag
	
	g_uchPHYXt2Flag = EN_XT2CLK;
}

/**********************************************************************/										 
/*		 关闭XT2	  DCO= MCLK, DCOCLKDIV = SMCLK,XT1 = ACLK		  */
/**********************************************************************/
void SetXT2ClkOff(void)
{
	SFRIE1 &= ~OFIE;    // Disable osc fault interrupt
  
        //Set SMCLK =DCOCLK
	//Set MCLK = DCOCLK
	//Set ACLK = xt1
	UCSCTL4 = SELS_3 + SELM_3 + SELA_0;
	
	P5SEL &= ~0x0C; 			 // Port Dis-select XT2
	
	UCSCTL6 |= XT2OFF;			 // XT2 Off
	
	g_uchPHYXt2Flag = DIS_XT2CLK;
}

/**********************************************************************/										 
/*		 根据宏定义选择不同的时钟系统								 */
/* PHY_SMCLK_DCO:DCO= MCLK = SMCLK ,XT1CLK=ACLK ,	 关闭 XT2		 */
/* PHY_SMCLK_XT2:DCO =MCLK=4MHz,XT2 =SMCLK,xt2=ACLK 				 */
/* 默认定义PHY_SMCLK_DCO											 */
/**********************************************************************/
//对系统整个时钟的设定，SMCLK通过选择，判断SMCLK时钟源
void InitClk(void)
{ 
	WDTCTL = WDTPW+WDTHOLD; 				  // Stop WDT 
	
	if(g_uchPHYSMclkSel == PHY_SMCLK_DCO)
		SetDCOClk();
	else if(g_uchPHYSMclkSel==PHY_SMCLK_XT2)
		SetXT2ClkOn();
	else
		SetDCOClk();
        
        SFRIE1 = OFIE;    // Enable osc fault interrupt
 
}

/**********************************************************************/										 
/*	输出时钟：P11.0 ACLK；P11.1 MCLK；P11.2 SMCLK		     */
/**********************************************************************/
void OutPutClk(void)
{
  P11DIR = BIT2 + BIT1 + BIT0; // P11.2,1,0 to output directi on
  P11SEL = BIT2 + BIT1 + BIT0; // P11.2 to output SMCLK
                               // P11.1 to output MCLK 
                               // P11.0 to output ACLK
}

/**********************************************************************/										 
/*		利用系统主时钟系统软件延时1/1024秒,约为1ms	      */
/**********************************************************************/
void PHYMCLKSoftDelay1ms(void)
{
	//1个时钟周期(1/PHY_MCLK)秒
	unsigned int i = (PHY_MCLK_1MS>>2);
        
	while(--i);//4*i 个时钟周期 = 1/1024秒
}


/**********************************************************************/										 
/*		利用系统主时钟系统软件延时1/2048秒,约为0.5ms	      */
/**********************************************************************/
void PHYMCLKSoftDelayHalfms(void)
{
	//1个时钟周期(1/PHY_MCLK)秒
	unsigned int i = (PHY_MCLK_1MS>>3);
	
	while(--i);//4*i 个时钟周期 = 1/2048秒
}

/**********************************************************************/										 
/*		利用系统主时钟系统软件延时1/4096秒,约为244us	      */
/**********************************************************************/
void PHYMCLKSoftDelay250us(void)
{
	//1个时钟周期(1/PHY_MCLK)秒
	unsigned int i = (PHY_MCLK_1MS>>4);
	
	while(--i);//4*i 个时钟周期 = 1/4096秒
}

/****************************************************************************/
/*	判断设备是A型（无功放）还是B型（有功放）。若是B型，返回0；否则返回－1	*/
/****************************************************************************/
int DevHasPAOrNot(void)
{
	return -1;
}

/****************************************************************************/
/*	USCIA3工作方式（232/TTL）选择 P4.6	高电平232模式，低电平TTL模式		*/
/*函数返回值：0：232模式，1： TTL模式										*/
/****************************************************************************/
int USCIA3Rs232Sel(void)
{
	PHY_232A3SEL_PxSEL &= ~PHY_232A3SEL_PxIN;               //用于一般的IO管脚功能
	PHY_232A3SEL_PxDIR &= ~PHY_232A3SEL_PxIN;               //设置为输入方向
	
	if( PHY_232A3SEL_PIN & PHY_232A3SEL_PxIN)
		return 0;
	else
		return 1;
}


/****************************************************************************/
/*	Set RS232A0 control    设置对外RS232A0通讯芯片控制							*/
/*	用到的MCU管脚:		P4.7	
	//=======源程序认为RS232A0管脚的开关设置为P4.7.但是通用板上开关设置
       用到的管脚是P5.4
       在端机中该串口用于摄像头图像采集
       在基站中该串口用于与电脑客户端进行通信
*/
/****************************************************************************/
//打开接收RS232A0   =====控制FORCEOFF进入高电平，打开串口通信芯片
void EnableRs232A0(void)
{
	PHY_232A0_PxSEL &= ~PHY_232A0_PxIN; 		//使用I/O功能
	PHY_232A0_PxDIR |= PHY_232A0_PxIN;		   //输出		
	PHY_232A0_PxOUT |= PHY_232A0_PxIN;		   //开启芯片
}
//关闭RS232A0接口   ,====控制FORCEOFF输出低电平，关闭串口通信芯片
void DisableRs232A0(void)
{
	PHY_232A0_PxSEL &=~PHY_232A0_PxIN;			//使用I/O功能
	PHY_232A0_PxDIR |= PHY_232A0_PxIN;			//输出		
	PHY_232A0_PxOUT &= ~PHY_232A0_PxIN; 		//开启芯片
}

/****************************************************************************/
/*	Set RS232A3 control    设置对外RS232A3通讯芯片控制							*/
/*	用到的MCU管脚:		P11.1	
       === PHY_232A3_PxIN 操作管脚编号
       ===该串口可以选择串口的工作模式TTL/RS232
       ===USCI A3
      *******************OKOKOKOKOKOKOKOKOK**********************
*/
/****************************************************************************/
//打开接收RS232A3
void EnableRs232A3(void)
{
	if(USCIA3Rs232Sel() == 1 )//选择TTL模式
		DisableRs232A3();
	else
	{  
		PHY_232A3_PxSEL &=~PHY_232A3_PxIN;  //==管脚选择一般的IO功能		   //使用I/O功能
		PHY_232A3_PxDIR |= PHY_232A3_PxIN;		   //输出		
		PHY_232A3_PxOUT |= PHY_232A3_PxIN;		   //开启芯片
	}
}
//关闭RS232A3接口
void DisableRs232A3(void)
{
	PHY_232A3_PxSEL &=~PHY_232A3_PxIN;			//使用I/O功能
	PHY_232A3_PxDIR |= PHY_232A3_PxIN;			//输出		
	PHY_232A3_PxOUT &= ~PHY_232A3_PxIN; 		//开启芯片
}

/****************************************************************************/
/*	Set Green light   control  控制 Green LED								*/
/*	用到的MCU管脚 :   P2.1												   */
/*	控制作用	  :   控制绿灯，Off关闭，On开启 							*/
/****************************************************************************/
//开启绿灯
void LedGreenOn(void)
{
	PHY_LEDG_PxDIR |= PHY_LEDG_PxIN;   
	PHY_LEDG_PxOUT |= PHY_LEDG_PxIN;	   
}
//关闭绿灯
void LedGreenOff(void)
{
	PHY_LEDG_PxDIR |= PHY_LEDG_PxIN;   
	PHY_LEDG_PxOUT &= ~PHY_LEDG_PxIN;	 
	
}
/****************************************************************************/
/*	Set Red light  control	控制Red LED 									*/
/*	用到的MCU管脚 :   P2.2													*/
/*	控制作用	  :   控制红灯，Off关闭，On开启 							*/
/****************************************************************************/
//开启红灯
void LedRedOn(void)
{
	PHY_LEDR_PxDIR |= PHY_LEDR_PxIN;   
	PHY_LEDR_PxOUT |= PHY_LEDR_PxIN;	  
}
//关闭红灯
void LedRedOff(void)
{
	PHY_LEDR_PxDIR |= PHY_LEDR_PxIN;   
	PHY_LEDR_PxOUT &= ~PHY_LEDR_PxIN;	
	
}

/****************************************************************************/
/*	Set flash light  control	控制flash light 			*/
/*	用到的MCU管脚 :   P4.3						*/
/*	控制作用	  :   控制充电板上的AAT4280，开关控制闪光灯 		*/
/****************************************************************************/
//开启闪光灯
void FlashLightOn(void)
{
	PHY_FLASH_LED_PxDIR   |= PHY_FLASH_LED_PxIN;   
	PHY_FLASH_LED_PxOUT   |= PHY_FLASH_LED_PxIN;	  
}
//关闭闪光灯
void FlashLightOff(void)
{
	PHY_FLASH_LED_PxDIR   |= PHY_FLASH_LED_PxIN;   
	PHY_FLASH_LED_PxOUT   &= ~PHY_FLASH_LED_PxIN;	
}

/****************************************************************************/
/*	蜂鸣器驱动 ,用到P2.3		响30ms										   */
/****************************************************************************/
void Sound()
{
	PHY_BEEP_PxDIR |= PHY_BEEP_PxIN;   
	PHY_BEEP_PxOUT |= PHY_BEEP_PxIN;	 /*开启*/	  
	for(int j=0;j<30;j++)		
		PHYMCLKSoftDelay1ms();
	PHY_BEEP_PxOUT &= ~PHY_BEEP_PxIN;	 /*关闭*/
	PHY_BEEP_PxDIR &= ~PHY_BEEP_PxIN; 
}
/****************************************************************************/
/*  图像出错的蜂鸣器的驱动，两者响不同的时间 ,用到P2.3    ======ZK  响1s                                               */
/****************************************************************************/
void PhotoerrorSound()
{
  	PHY_BEEP_PxDIR |= PHY_BEEP_PxIN;   
	PHY_BEEP_PxOUT |= PHY_BEEP_PxIN;	 /*开启*/	  
	for(int j=0;j<1000;j++)		
		PHYMCLKSoftDelay1ms();
	PHY_BEEP_PxOUT &= ~PHY_BEEP_PxIN;	 /*关闭*/
	PHY_BEEP_PxDIR &= ~PHY_BEEP_PxIN; 
}
/****************************************************************************/
/*	Set power 2 control  外接设备USCIA1电源开关控制 	 3221				*/
/*	用到的MCU管脚 :   P10.1 												*/
/****************************************************************************/
void DevPower2On(void)
{
	PHY_DEV_POWER2_PxSEL &= ~PHY_DEV_POWER2_PxIN;
	PHY_DEV_POWER2_PxDIR |= PHY_DEV_POWER2_PxIN;   
	PHY_DEV_POWER2_PxOUT |= PHY_DEV_POWER2_PxIN;	 /*开启*/	
}
void DevPower2Off(void)
{
	PHY_DEV_POWER2_PxSEL &= ~PHY_DEV_POWER2_PxIN;
	PHY_DEV_POWER2_PxDIR |= PHY_DEV_POWER2_PxIN;   
	PHY_DEV_POWER2_PxOUT &= ~PHY_DEV_POWER2_PxIN;	 /*关闭*/  
}

/****************************************************************************/
/*	Set power 3 control 	USCIB0/AD外接设备的电源控制    3221 			*/
/*	用到的MCU管脚 :   P10.2 												*/
/****************************************************************************/
void DevPower3On(void)
{
	PHY_DEV_POWER3_PxSEL &= ~PHY_DEV_POWER3_PxIN;
	PHY_DEV_POWER3_PxDIR |= PHY_DEV_POWER3_PxIN;   
	PHY_DEV_POWER3_PxOUT |= PHY_DEV_POWER3_PxIN;	 /*开启*/	
}
void DevPower3Off(void)
{
	PHY_DEV_POWER3_PxSEL &= ~PHY_DEV_POWER3_PxIN;
	PHY_DEV_POWER3_PxDIR |= PHY_DEV_POWER3_PxIN;   
	PHY_DEV_POWER3_PxOUT &= ~PHY_DEV_POWER3_PxIN;	 /*关闭*/  
}

/****************************************************************************/
/*	Set power USR control	  用户外接设备的电源控制	4280				*/
/*	用到的MCU管脚 :   P10.3
        摄像头控制电源
*/
/****************************************************************************/
void DevPowerUsrOn(void)
{
	PHY_DEV_POWERUSR_PxSEL &= ~PHY_DEV_POWERUSR_PxIN;
	PHY_DEV_POWERUSR_PxDIR |= PHY_DEV_POWERUSR_PxIN;   
	PHY_DEV_POWERUSR_PxOUT |= PHY_DEV_POWERUSR_PxIN;	 /*开启*/	
}
void DevPowerUsrOff(void)
{
	PHY_DEV_POWERUSR_PxSEL &= ~PHY_DEV_POWERUSR_PxIN;       //==设置为一般IO功能
	PHY_DEV_POWERUSR_PxDIR |= PHY_DEV_POWERUSR_PxIN;        //设置为输出方向
	PHY_DEV_POWERUSR_PxOUT &= ~PHY_DEV_POWERUSR_PxIN;	 /*关闭*/   //输出低电平，摄像头电源关闭 
}

/****************************************************************************/
 /*              低功耗模式下，没有用到的管脚的初始化                      */
/*             对整个系统中没有用到的管脚全部设置为高阻状态                */
/*                注意在程序在开始执行情况下，要对所有的管脚进行初始化状态*/
/*             降低系统的功耗                                               */
/****************************************************************************/
  void LowPowerPinInit(void)
 {
     P1REN=0xFF; P2REN=0xFF;P3REN=0xFF; P4REN=0xFF;
     P5REN=0xFF;P6REN=0xFF; P7REN=0xFF; P8REN=0xFF;
     P9REN=0xFF;P10REN=0xFF;P11REN=0xFF;PJREN=0xFF;
     
     P1DIR = 0x00;P2DIR = 0x00;P3DIR = 0x00; P4DIR = 0x00;
     P5DIR = 0x00;P6DIR = 0x00;P7DIR = 0x00;P8DIR = 0x00;
     P9DIR = 0x00;P10DIR = 0x00;P11DIR = 0x00;PJDIR = 0x00;
  
     P1OUT = 0xFF;P2OUT = 0xFF;P3OUT = 0xFF;P4OUT = 0xFF;
     P5OUT = 0xFF;P6OUT = 0xFF;P7OUT = 0xFF;P8OUT = 0xFF;
     P9OUT = 0xFF;P10OUT = 0xFF;P11OUT = 0xFF;PJOUT = 0xFF;
 }

/****************************************************************************/
/*				 电源管理函数库；主电源默认打开。							*/
/*	  单片机电源管理接口初始化；默认关闭其它用电器电源；					*/
/****************************************************************************/
void InitPower(void)
{  
/*  
     if(g_uAppDataInt >0) //有注册任务
    {  
        if(  (g_uAppDataType != 2)  &&  //对图像传感器不关断
             (g_uAppDataType != 0x0d)&& //红外
             (g_uAppDataType != 0x0e)&& //烟雾
             (g_uAppDataType != 0x0f)&& //粉尘
             (g_uAppDataType != 0x10)&&//甲烷
             (g_uAppDataType != 0x11)&& // 温湿度和光照度组合
             (g_uAppDataType != 0x12)&& //温湿度和红外组合
             (g_uAppDataType != 0x14)&& //温湿度和二氧化碳组合
             (g_uAppDataType != 0x05))//温湿度数显
          
          {
                  DevPower2Off();
                  DevPowerUsrOff();
          }
    }
    else
    {
      if(g_uAppDataType != 2)  //对图像传感器不关断
      {
          DevPower2Off();
          DevPowerUsrOff();
      }
    }
  */
    DevPowerUsrOff();
    FlashLightOff();                    //====ZK
    DevPower3Off();                     //因为电路板上焊接了IC3的AAT3221，所以执行关闭
                                        //=====ZK  闪光灯电源关闭，IC3（AAT3221）关闭
    
    //if(g_uchAppchRoleSet != CenterBase)
    //  DevPower3Off();
}

/****************************************************************************/
/*	Set UARTa0 control			串口A0波特率设置							*/
/*	用到的MCU管脚	   :  P3.4,P3.5 UART模块功能							*/
/*	函数参数		   : char bps											*/
/*	可选的波特率设置   : Baud_2400，Baud_4800，Baud_9600，Baud_19200，
Baud_38400,Baud_57600，Baud_115200											*/
/*	默认选用Baud_9600，波特率高于Baud_9600时需要打开XT2CLK					*/
/****************************************************************************/
void OpenUartA0(char bps)
{
	UCA0CTL1 |=UCSWRST; //RESET
	
	P3SEL |= BIT4 + BIT5;  // P3.4= USARTA0 TXD P3.5 RXD  设置P3.4,P3.5的UART功能
	
	UCA0CTL0=0; 	// 8N1 
	
	//默认设定Baud_9600
	UCA0CTL1 = UCSSEL0;// CLK = ACLK = 32768Hz
	UCA0BR0 = 0x03;
	UCA0BR1 = 0;
	UCA0MCTL = UCBRS_3+UCBRF_0;
	
	switch(bps) 	  
	{
	case Baud_2400: 
		UCA0BR0 = 0x0d;
		UCA0BR1 = 0;
		UCA0MCTL = UCBRS_6+UCBRF_0;
		break;		
	case Baud_4800: 	
		UCA0BR0 = 0x06;
		UCA0BR1 = 0;
		UCA0MCTL = UCBRS_7+UCBRF_0;
		break;		
	case Baud_9600:
		UCA0BR0 = 0x03;
		UCA0BR1 = 0;
		UCA0MCTL = UCBRS_3+UCBRF_0;
		break;		
	case Baud_19200:
		{	  
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;
			
			UCA0CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA0BR0 = 0xA0;
				UCA0BR1 = 0x01;
				UCA0MCTL = UCBRS_6+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA0BR0 = 0xD0;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_3+UCBRF_0;
			}	
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA0BR0 = 0xDA;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_4+UCBRF_0;
			}	
                  
			break;	
		}
            /*
	case Baud_38400:
		{
			if(g_uchPHYXt2Flag == DIS_XT2CLK)
				break;
			
			UCA0CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA0BR0 = 0xD0;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_3+UCBRF_0;
			} 
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA0BR0 = 0x68;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_1+UCBRF_0;
			}
			break;	
		}	
            */
	case Baud_57600:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;	
			
			UCA0CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK 		   
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA0BR0 = 0x8A;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_7+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA0BR0 = 0x45;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_4+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA0BR0 = 0x48;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_7+UCBRF_0;
			}	
			break;	
		}				
	case Baud_115200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;
			
			UCA0CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA0BR0 = 0x45;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_4+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{				 //clk = Smclk = 4MHz
				UCA0BR0 = 0x22;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_6+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA0BR0 = 0x24;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_3+UCBRF_0;
			}	
                  
			break;	
		}		
	default:
		break;
	}
	
	UCA0CTL1 &=~UCSWRST;	//Initialize USCI state machine
	UCA0IE |= UCRXIE;	 //允许串口0接收中断
	
	g_uchPHYUART0Open = 1;
} 

/****************************************************************************/
/*	关闭UART0口 															*/
/****************************************************************************/
void CloseUartA0(void)
{
	UCA0IE &= ~UCRXIE;	  //禁止UART0接收中断
	UCA0CTL1 |=UCSWRST; //RESET
	
	g_uchPHYUART0Open = 0;
}


/****************************************************************************/
/*	Set UARTa1 control			串口A1波特率设置							*/
/*	用到的MCU管脚	   :       P5.6,P5.7 UART模块功能							*/
/*	函数参数		   : char bps											*/
/*	可选的波特率设置   : Baud_2400，Baud_4800，Baud_9600，Baud_19200，
Baud_38400,Baud_57600，Baud_115200				  */
/****************************************************************************/
void OpenUartA1(char bps)
{
	UCA1CTL1 |=UCSWRST; //RESET
	
	P5SEL |= BIT6 + BIT7;  // P5.6= USARTA1 TXD P5.7 RXD  设置UART功能
	
	UCA1CTL0=0; 	// 8N1 
	
	//默认9600
	UCA1CTL1 = UCSSEL0;// CLK = ACLK = 32768Hz
	UCA1BR0 = 0x03;
	UCA1BR1 = 0;
	UCA1MCTL = UCBRS_3+UCBRF_0;
	
	switch(bps) 	  
	{
	case Baud_2400: 
		UCA1BR0 = 0x0d;
		UCA1BR1 = 0;
		UCA1MCTL = UCBRS_6+UCBRF_0;
		break;
		
	case Baud_4800: 	
		UCA1BR0 = 0x06;
		UCA1BR1 = 0;
		UCA1MCTL = UCBRS_7+UCBRF_0;
		break;
		
	case Baud_9600:
		UCA1BR0 = 0x03;
		UCA1BR1 = 0;
		UCA1MCTL = UCBRS_3+UCBRF_0;
		break;
		
	case Baud_19200://clk = Smclk = 8MHz
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;	
		  
			UCA1CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA1BR0 = 0xA0;
				UCA1BR1 = 0x01;
				UCA1MCTL = UCBRS_6+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA1BR0 = 0xd0;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_3+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA1BR0 = 0xDA;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_4+UCBRF_0;
			}	
                  
			break;	
		}
            /*
	case Baud_38400:
		{
			if(g_uchPHYXt2Flag == DIS_XT2CLK)
				break;			  
			
			UCA1CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA1BR0 = 0xD0;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_3+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA1BR0 = 0x68;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_1+UCBRF_0;
			}
			break;	
		}	
            */
	case Baud_57600:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;			  
			
			UCA1CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA1BR0 = 0x8A;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_7+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA1BR0 = 0x45;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_4+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA1BR0 = 0x48;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_7+UCBRF_0;
			}	
                  
			break;			
		}
	case Baud_115200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;			  
			
			UCA1CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA1BR0 = 0x45;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_4+UCBRF_0; 
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA1BR0 = 0x22;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_6+UCBRF_0; 
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA1BR0 = 0x24;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_3+UCBRF_0;
			}	
			break;
		}		
	default:
		break; 
	}
	
	UCA1CTL1 &=~UCSWRST;	//Initialize USCI state machine
	UCA1IE |= UCRXIE;	 //允许串口1接收中断
	
	g_uchPHYUART1Open = 1;
} 

/****************************************************************************/
/*	关闭UART1口 															*/
/****************************************************************************/
void CloseUartA1(void)
{
	UCA1IE &= ~UCRXIE;	  //禁止UART1接收中断
	UCA1CTL1 |=UCSWRST;       //RESET	
	g_uchPHYUART1Open = 0;
}

/****************************************************************************/
/*	Set SPI主A1 control 		 A1用于SPI主机通讯							*/
/*	3线，8比特，时钟极性为高电平无效，先发送MSB,通讯时钟为MCLK/2			*/
/*	用到的MCU管脚	   :  P5.6 P5.7的SPI模块功能							 */
/****************************************************************************/
void OpenMasterSpiA1(void)
{  
	UCA1CTL1 |=UCSWRST; //RESET
	
	P5SEL |= BIT6 + BIT7;  // 设置P5.6,P5.7的SPI功能
	
	UCA1CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;	  // 3-pin, 8-bit SPI master
	// Clock polarity high, MSB
	UCA1CTL1 |= UCSSEL_2;					  // MCLK
	UCA1BR0 = 0x02; 						  // /2
	UCA1BR1 = 0;							  //
	UCA1MCTL = 0;							  // No modulation
	UCA1CTL1 &= ~UCSWRST;					  // **Initialize USCI state machine**
	UCA1IE |= UCRXIE;						  // Enable USCI_A2 RX interrupt
}

/****************************************************************************/
/*	关闭SPI A1口															 */
/****************************************************************************/
void CloseSpiA1(void)
{
	UCA1IE &= ~UCRXIE;	  //禁止SPI接收中断
	UCA1CTL1 |=UCSWRST; //RESET
}


/****************************************************************************/
/*	Set UARTA2 control			串口A2波特率设置							*/
/*	用到的MCU管脚	   :  P9.4,P9.5 UART模块功能							*/
/*	函数参数		   : char bps											*/
/*	可选的波特率设置   : Baud_2400，Baud_4800，Baud_9600，Baud_19200，
Baud_57600，Baud_115200 			   */

//===在470通用板中并没有用到该管脚的功能，P9.4,P9.5并没有引出来
/****************************************************************************/
void OpenUartA2(char bps)
{
	UCA2CTL1 |=UCSWRST; //RESET
	
	P9SEL |= BIT4 + BIT5;  // P9.4= USARTA2 TXD P9.5 RXD  设置P9.4,P9.5的UART功能
	
	UCA2CTL0=0; 	// 8N1 
	
	//默认9600
	UCA2CTL1 = UCSSEL0;// CLK = ACLK = 32768Hz
	UCA2BR0 = 0x03;
	UCA2BR1 = 0x0;
	UCA2MCTL = UCBRS_3+UCBRF_0;
	
	switch(bps) 	  
	{
	case Baud_2400: 
		UCA2BR0 = 0x0d;
		UCA2BR1 = 0;
		UCA2MCTL = UCBRS_6+UCBRF_0;
		break;
		
	case Baud_4800: 	
		UCA2BR0 = 0x06;
		UCA2BR1 = 0;
		UCA2MCTL = UCBRS_7+UCBRF_0;
		break;
		
	case Baud_9600:
		UCA2BR0 = 0x03;
		UCA2BR1 = 0x0;
		UCA2MCTL = UCBRS_3+UCBRF_0;
		break;
		
	case Baud_19200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;
              
			UCA2CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA2BR0 = 0xA0;
				UCA2BR1 = 0x01;
				UCA2MCTL = UCBRS_6+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA2BR0 = 0xd0;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_3+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA2BR0 = 0xDA;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_4+UCBRF_0;
			}	
			break;
		}
            /*
	case Baud_38400:
		{
			if(g_uchPHYXt2Flag == DIS_XT2CLK)
				break;
			UCA2CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA2BR0 = 0xD0;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_3+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA2BR0 = 0x68;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_1+UCBRF_0;
			}				
			break;
		}
            */
	case Baud_57600:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;
			UCA2CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA2BR0 = 0x8a;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_7+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA2BR0 = 0x45;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_4+UCBRF_0;
			}	

                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA2BR0 = 0x48;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_7+UCBRF_0;
			}				
			break;
		}		
	case Baud_115200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;
			UCA2CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA2BR0 = 0x45;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_4+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA2BR0 = 0x22;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_6+UCBRF_0;
			}	
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA2BR0 = 0x24;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_3+UCBRF_0;
			}	
                  
			break;
		}	
		
	default:
		break; 
	}
	
	UCA2CTL1 &=~UCSWRST;	//Initialize USCI state machine
	UCA2IE |= UCRXIE;	 //允许串口2接收中断
	
	g_uchPHYUART2Open = 1;
} 

/****************************************************************************/
/*	关闭UART2口 															*/
/****************************************************************************/
void CloseUartA2(void)
{
	UCA2IE &= ~UCRXIE;	  //禁止UART2接收中断
	UCA2CTL1 |=UCSWRST; //RESET
	
	g_uchPHYUART2Open = 0;
}

/****************************************************************************/
/*	Set SPI主A2 control 		 A2用于SPI主机通讯							*/
/*	3线，8比特，时钟极性为高电平无效，先发送MSB,通讯时钟为MCLK/2			*/
/*	用到的MCU管脚	   :  P9.4,P9.5 SPI模块功能 							*/
/****************************************************************************/
void OpenMasterSpiA2(void)
{  
	UCA2CTL1 |=UCSWRST; //RESET
	
	P9SEL |= BIT4 + BIT5;  // 设置P9.4,P9.5的SPI功能
	
	UCA2CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;	  // 3-pin, 8-bit SPI master
	// Clock polarity high, MSB
	UCA2CTL1 |= UCSSEL_2;					  // MCLK
	UCA2BR0 = 0x02; 						  // /2
	UCA2BR1 = 0;							  //
	UCA2MCTL = 0;							  // No modulation
	UCA2CTL1 &= ~UCSWRST;					  // **Initialize USCI state machine**
	UCA2IE |= UCRXIE;						  // Enable USCI_A2 RX interrupt
}

/****************************************************************************/
/*	关闭SPI A2口															 */
/****************************************************************************/
void CloseSpiA2(void)
{
	UCA2IE &= ~UCRXIE;	  //禁止SPI接收中断
	UCA2CTL1 |=UCSWRST; //RESET
}

/****************************************************************************/
/*	Set UARTA3 control			串口A3波特率设置							*/
/*	用到的MCU管脚	   :  P10.4,P10.5 UART模块功能							  */
/*	函数参数		   : char bps											*/
/*	可选的波特率设置   : Baud_2400，Baud_4800，Baud_9600，Baud_19200，
Baud_57600，Baud_115200 			   */
/****************************************************************************/
void OpenUartA3(char bps)
{  
	UCA3CTL1 |=UCSWRST; //RESET
	
	P10SEL |= BIT4 + BIT5;	// P10.4= USARTA2 TXD P10.5 RXD  设置P10.4,P10.5的UART功能
	
	UCA3CTL0=0; 	// 8N1
	
	//默认9600
	UCA3CTL1 = UCSSEL0;// CLK = ACLK = 32768Hz
	UCA3BR0 = 0x03;
	UCA3BR1 = 0;
	UCA3MCTL = UCBRS_3+UCBRF_0;
	
	switch(bps) 	  
	{
	case Baud_2400: 
		UCA3BR0 = 0x0d;
		UCA3BR1 = 0;
		UCA3MCTL = UCBRS_6+UCBRF_0;
		break;
		
	case Baud_4800: 	
		UCA3BR0 = 0x06;
		UCA3BR1 = 0;
		UCA3MCTL = UCBRS_7+UCBRF_0;
		break;
		
	case Baud_9600:
		UCA3BR0 = 0x03;
		UCA3BR1 = 0;
		UCA3MCTL = UCBRS_3+UCBRF_0;
		break;
		
	case Baud_19200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;	
			
			UCA3CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA3BR0 = 0xA0;
				UCA3BR1 = 0x01;
				UCA3MCTL = UCBRS_6+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA3BR0 = 0xd0;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_3+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA3BR0 = 0xDA;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_4+UCBRF_0;
			}	
			break;	
		}
            /*
	case Baud_38400:
		{
			if(g_uchPHYXt2Flag == DIS_XT2CLK)
				break;	
			
			UCA3CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA3BR0 = 0xd0;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_3+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA3BR0 = 0x68;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_1+UCBRF_0;
			}
			break;	
		}
            */
	case Baud_57600:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;	
			
			UCA3CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA3BR0 = 0x8a;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_7+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA3BR0 = 0x45;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_4+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA3BR0 = 0x48;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_7+UCBRF_0;
			}	
                  
			break;	
		}		
	case Baud_115200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;	
			
			UCA3CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA3BR0 = 0x45;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_4+UCBRF_0;
			}
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA3BR0 = 0x22;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_6+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA3BR0 = 0x24;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_3+UCBRF_0;
			}	
			break;	
		}
		
	default:
		break; 
	}
	
	UCA3CTL1 &=~UCSWRST;	//Initialize USCI state machine
	UCA3IE |= UCRXIE;	 //允许串口3接收中断
	
	g_uchPHYUART3Open = 1;
} 

/****************************************************************************/
/*	关闭UART3口 															*/
/****************************************************************************/
void CloseUartA3(void)
{
	UCA3IE &= ~UCRXIE;	  //禁止UART3接收中断
	UCA3CTL1 |=UCSWRST; //RESET
	
	g_uchPHYUART3Open = 0;
}

/*===========================================================================
定时器相关设置的总结
在MSP430的头文件中，MC0与MC1这种定义适用于设置某一个位为1
MC_0，MC_1，MC_2，MC_3适用于设置每一个段的全部功能，如MC控制位共有2位，
则共有4种控制选择方式，分别代表所表示的四种功能
#define MC1                 (0x0020u)   Timer A mode control 1       //工作方式选择
#define MC0                 (0x0010u)   Timer A mode control 0
#define MC_0                (0*0x10u)   Timer A mode control: 0 - Stop 
#define MC_1                (1*0x10u)   Timer A mode control: 1 - Up to CCR0 
#define MC_2                (2*0x10u)   Timer A mode control: 2 - Continuous up 
#define MC_3                (3*0x10u)   Timer A mode control: 3 - Up/Down 

#define TASSEL1             (0x0200u)   Timer A clock source select 1   //时钟选择
#define TASSEL0             (0x0100u)   Timer A clock source select 0 
#define TASSEL_0            (0*0x100u)  Timer A clock source select: 0 - TACLK 
#define TASSEL_1            (1*0x100u)  Timer A clock source select: 1 - ACLK  
#define TASSEL_2            (2*0x100u)  Timer A clock source select: 2 - SMCLK 
#define TASSEL_3            (3*0x100u)  Timer A clock source select: 3 - INCLK 
***********注意***********
//注意两者的表达方式的区别:
在1的表达方式中，是把TASSEL_1+TACLR的值0x0104的值赋值给TA0CTL
在2的表达方式中，这里预先不知道TA0CTL的值，只是可以保证TASSEL_1位与TACLR位都为1，
初始TA0CTL的值是不做任何改变，只是在原基础上做了一个“或”运算
1：TA0CTL = TASSEL_1 + TACLR;   
2：TA0CTL|= TASSEL_1 + TACLR;
=============================================================================*/
/****************************************************************************/
/*	初始化定时器0A0 用系统辅助时钟 ACLK = 32768Hz	CCR0 = 32768			*/
/****************************************************************************/
void SetTimer0A0_ACLK(void)
{
	TA0CTL &= ~(MC0+MC1);				        /*暂停计数*/
	TA0CTL = TASSEL_1 + TACLR;				/*使用辅助晶振 ACLK, 清除位CLR*/
	TA0CCTL0 = CCIE;				        /*CCR0 中断允许	*/
}

/****************************************************************************/
/*	初始化定时器0A0 用子系统时钟 SMCLK										*/
/****************************************************************************/
void SetTimer0A0_SMCLK(void)
{
	TA0CTL &= ~(MC0+MC1);				        /*暂停计数					 */
	TA0CTL = TASSEL_2 + TACLR;				/*使用子系统晶振 SMCLK, 清除位CLR*/
	TA0CCTL0 = CCIE;				        /*CCR0 中断允许				*/
}

/****************************************************************************/
/*					  Stop Timer0A0    停止计数 							*/
/****************************************************************************/
void StopTimer0A0(void)
{
	TA0CTL &= ~(MC0+MC1);				       /*暂停计数					*/
}

/****************************************************************************/
/*	Set Timer0A0 delay time    设置定时时间 								*/
/*	参数		  : long timeset											*/
/*	定时		  : (timeset / 时钟频率) ×1000 ms							*/
/****************************************************************************/
void  Timer0A0_Delay(long timeSet)
{
	TA0CTL &= ~(MC0+MC1);  
	TA0CCR0 = timeSet;                          /*确定延时的时间*/
	TA0CTL |= MC0;				 /*增计数到CCRO，并从0开始*/
	g_uPHYTimer0A0Count = 0;	          /*全局变量 定时次数复位*/
}

/****************************************************************************/
/*			 用子系统主时钟 定时1秒钟										*/
/****************************************************************************/
void Timer0A0_Delay_1s_SMCLK(void)
{
	SetTimer0A0_SMCLK();                  //选用SMCLK
	Timer0A0_Delay(PHY_MCLK_1MS);         //一次中断(1/1024)秒
	while(g_uPHYTimer0A0Count <1024);     //当循环1024次时，延时达到1s
	StopTimer0A0();
}

/****************************************************************************/
/*			   用子系统主时钟 定时1/1024秒									*/
/****************************************************************************/
void Timer0A0_Delay_1ms_SMCLK(void)
{
	SetTimer0A0_SMCLK();
	Timer0A0_Delay(PHY_MCLK_1MS);
	while(g_uPHYTimer0A0Count <1);
	StopTimer0A0();
}

/****************************************************************************/
/*			   用系统辅助时钟 定时1/1024秒									*/
/****************************************************************************/
void Timer0A0_Delay_1ms_ACLK(void)
{
	SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK_1MS);
	while(g_uPHYTimer0A0Count < 1);
	StopTimer0A0();
}

/****************************************************************************/
/*			   用系统辅助时钟 定时1秒钟 									*/
/****************************************************************************/
void Timer0A0_Delay_1s_ACLK(void)
{
	SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK);
	while(g_uPHYTimer0A0Count < 1);
	StopTimer0A0();
}

/****************************************************************************/
/*	初始化定时器1A0 用系统辅助时钟 ACLK = 32768Hz	CCR0 = 32768			*/
/****************************************************************************/
void SetTimer1A0_ACLK(void)
{
	TA1CTL &= ~(MC0+MC1);				        /*暂停计数*/
	TA1CTL = TASSEL_1 + TACLR;				/*使用辅助晶振 ACLK, 清除位CLR*/
	TA1CCTL0 = CCIE;				        /*CCR0 中断允许*/
}

/****************************************************************************/
/*	初始化定时器1A0 用子系统时钟 SMCLK = 8388608Hz							*/
/****************************************************************************/
void SetTimer1A0_SMCLK(void)
{
	TA1CTL &= ~(MC0+MC1);				/*暂停计数					  */
	TA1CTL = TASSEL_2 + TACLR;			/*使用子系统晶振 SMCLK, 清除位CLR*/
	TA1CCTL0 = CCIE;				/*CCR0 中断允许 			  */
}

/****************************************************************************/
/*					  Stop Timer1A0    停止计数 							*/
/****************************************************************************/
void StopTimer1A0(void)
{
	TA1CTL &= ~(MC0+MC1);				  /*暂停计数					*/
}

/****************************************************************************/
/*	Set Timer1A0 delay time    设置定时时间 								*/
/*	参数		  : long timeset											*/
/*	定时		  : (timeset / 时钟频率) ×1000     ms							*/
/****************************************************************************/
void  Timer1A0_Delay(long timeSet)
{
	TA1CTL &= ~(MC0+MC1);  
	TA1CCR0 = timeSet; 
	TA1CTL |= MC0;				 /*增计数到CCRO，并从0开始*/
	g_uPHYTimer1A0Count = 0;	  /*全局变量 定时次数复位*/
}

/****************************************************************************/
/*			 用子系统主时钟 定时1秒钟										*/
/****************************************************************************/
void Timer1A0_Delay_1s_SMCLK(void)
{
	SetTimer1A0_SMCLK();
	Timer1A0_Delay(PHY_MCLK_1MS);
	while(g_uPHYTimer1A0Count <1024);//一次中断(1/1024)秒
	StopTimer1A0();
}

/****************************************************************************/
/*			   用子系统主时钟 定时1毫钟 									*/
/****************************************************************************/
void Timer1A0_Delay_1ms_SMCLK(void)
{
	SetTimer1A0_SMCLK();
	Timer1A0_Delay(PHY_MCLK_1MS);
	while(g_uPHYTimer1A0Count <1);
	StopTimer1A0();
}

/****************************************************************************/
/*			   用系统辅助时钟 定时1/1024秒钟								*/
/****************************************************************************/
void Timer1A0_Delay_1ms_ACLK(void)
{
	SetTimer1A0_ACLK();
	Timer1A0_Delay(PHY_ACLK_1MS);
	while(g_uPHYTimer1A0Count < 1);
	StopTimer1A0();
}

/****************************************************************************/
/*			   用系统辅助时钟 定时1秒钟 									*/
/****************************************************************************/
void Timer1A0_Delay_1s_ACLK(void)
{
	SetTimer1A0_ACLK();
	Timer1A0_Delay(PHY_ACLK);
	while(g_uPHYTimer1A0Count < 1);
	StopTimer1A0();
}

/****************************************************************************/
/*	初始化定时器TimerB 用系统辅助时钟 ACLK = 32768Hz   CCR0 = 32768 ;		*/
/****************************************************************************/
void SetTimerB_ACLK(void)
{
	TBCTL &= ~(MC0+MC1);				  /*暂停计数					*/
	TBCTL = TBSSEL0 + TBCLR;			  /*使用辅助晶振 ACLK, 清除位CLR*/
	TBCCTL0 = CCIE; 					  /*CCR0 中断允许				*/	  
}
/****************************************************************************/
/*	初始化定时器TimerB 用子系统主时钟										*/
/****************************************************************************/
void SetTimerB_SMCLK(void)
{
	TBCTL &= ~(MC0+MC1);				  /*暂停计数					*/
	TBCTL = TBSSEL1 + TBCLR;			  /*使用SMCLK, 清除位CLR*/
	TBCCTL0 = CCIE; 			  /*CCR0 中断允许				*/
	
}

/****************************************************************************/
/*					  Stop Timer_B	  停止计数	     */
/****************************************************************************/
void StopTimerB(void)
{
	TBCTL &= ~(MC0+MC1);				 /*暂停计数 				   */
}

/****************************************************************************/
/*	Set Timer_B delay time	  设置定时时间									*/
/*	参数		  : long timeset											*/
/*	定时		  : (timeset / 时钟频率) ×1000 ms							  */
/****************************************************************************/
void  TimerB_Delay(long timeSet)
{
	TBCTL &= ~(MC0+MC1);  
	TBCCR0 = timeSet; 
	TBCTL |= MC0;				/*增计数到CCRO，并从0开始*/
	
	g_uPHYTimerBCount = 0;			   /*全局变量 定时次数复位*/
}

/****************************************************************************/
/*			   用系统辅助时钟 定时1/1024秒钟	            */
/****************************************************************************/
void TimerB_Delay_1ms_ACLK()
{
	SetTimerB_ACLK();
	TimerB_Delay(PHY_ACLK_1MS);
	while(g_uPHYTimerBCount <1);
	StopTimerB();  
}

/****************************************************************************/
/*			   用系统辅助时钟 定时1秒钟 									*/
/****************************************************************************/
void TimerB_Delay_1s_ACLK()
{
	SetTimerB_ACLK();
	TimerB_Delay(PHY_ACLK);
	while(g_uPHYTimerBCount <1);
	StopTimerB();  
}
/****************************************************************************/
/*     通过AD转换功能获取退避指数的随机码         P6.7                      */
/****************************************************************************/
unsigned int  AdRandom(unsigned char time)
{
  unsigned int result;
 //单通道单次转换，无中断 
  P6SEL |= 0x07;     //选择AD转换功能
  P6DIR&=~BIT7;
  REFCTL0&=~REFMSTR; //获取参考电源的控制权
  ADC12CTL0|=ADC12ON;
  ADC12CTL0|=ADC12SHT03+ADC12REFON+ADC12REF2_5V;
  ADC12CTL1|=ADC12SHP+ADC12SSEL_1+ADC12CSTARTADD_7;//ACLK
  ADC12CTL2|=ADC12RES_2+ADC12SR;//12位分辨率，采样频率50ksps，降低功耗
  ADC12MCTL7|=ADC12INCH_7+ADC12SREF_1;    
  ADC12CTL0 |= ADC12ENC;
  ADC12CTL0|=ADC12SC;
  PHYMCLKSoftDelay1ms(); //延时1ms
  result=ADC12MEM7;
  ADC12IFG&=~ADC12IFG7;
  ADC12CTL0&=~ADC12ENC;
  ADC12CTL0&=~(ADC12SC+ADC12REFON);
  result=result%time;
  return result;
}
/****************************************************************************/
/*    单通道序列采样模式 ，对一个通道重复采样10次，取平均值                 */
/*	  ADC12数据采样控制  用到P6口, P6.0~P6.7对应A0~A7;						*/
/*							 P7口，p7.4~p7.7对应A12~A15。其中:				*/
/*	  A0---- 板级电源电量检测,电压配比：200K:1M 							*/		 
/*	  A1-----输入电压DC-DC前的电池电量检测，电压配比：200K:1M				*/
/*	  A2-----太阳能或AC-DC前的电量检测,电压配比：200K:1M					*/
/*	  A3~A7,A12~A15接其他模拟传感器电压输入，RC时间常数需要设定 			*/
/*	  函数参数 char AdSet 0~7,12~15对应需要采样的AD口A0~A7,A12~A15			*/
/*	  函数返回值 unsigned int 是12位AD数据，最低位是LSB 					*/
/*      函数返回值：有效位是12bit,最高位如果是1，表明AD无效。                       */
/****************************************************************************/

//单通道重复 扩展采样模式
unsigned int ADSequence(unsigned char AdSet)
{ 
	unsigned int ad = 0;
	char chP6[] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17};
	char chP7[] = {0x1C,0x1D,0x1E,0x1F};
	
	if(AdSet<=7)//A0~A7     //判断需要哪一个管脚进行AD转换
      {
        P6SEL |= (0x01 << AdSet);		  //P6口选择 模拟输入功能
        //P6REN &= ~(0x01 << AdSet);         //不要上、下拉电阻
      }
	else if((AdSet>=12) && (AdSet<=15)) //A12~A15
      {
        P7SEL |= (0x01 <<(AdSet-8));	  //P7口选择 模拟输入功能
        //P7REN &= ~(0x01 << (AdSet-8));         //不要上、下拉电阻
      }
	else return 0;
	
        REFCTL0&=~ REFMSTR;//msp430f5438a的电压参考模块
	ADC12CTL0 &= ~ADC12ENC; 				 // 改变设置前停止A/D转换
	
	//内部参考电压 2.5V打开  ADC内核消耗打开 无中断
	ADC12CTL0 = ADC12REF2_5V + ADC12REFON + ADC12ON; 
      
      // ADC内核消耗打开 无中断
	//ADC12CTL0 = ADC12ON; 
	
	// 单通道重复采样，扩展采样模式，采样信号SAMPCON由采样输入信号控制
	//采样输入信号源为ADC12SC控制位
	//转换内核时钟源为系统辅助时钟
	//采样结果保存在ADC12MEM0
	ADC12CTL1 = ADC12CONSEQ_2+ ADC12SSEL_1; 
	
	//采样精度12bit，采样速率~50KSPS以降低功耗
	ADC12CTL2 = ADC12SR + ADC12RES1;
	
	//转换存储寄存器ADC12MEM0的控制寄存器
	if(AdSet<=7)//A0~A7
		ADC12MCTL0 = chP6[AdSet]; //选择采样通道  参考内部电压源 Vref+	AVss	
	if((AdSet>=12) && (AdSet<=15)) //A12~A15
		ADC12MCTL0 = chP7[AdSet-12]; //选择采样通道  参考内部电压源 Vref+  AVss 
	
	//等待内部参考电压建立起来 约20微秒        
        //1个时钟周期(1/PHY_MCLK)秒
	unsigned int i = (PHY_MCLK_1MS>>3);	
	while(--i);//4*i 个时钟周期 = 4/(1024*8)秒 = 488微秒
	
	ADC12CTL0 |= ADC12ENC; // 允许转换
      
      unsigned char c = 0;
      unsigned long lad = 0;
      while(1)//采样10次 取平均值
      {
        
          ADC12CTL0 |= ADC12SC;  // 开始采样	
          
          //扩展模式下的采样保持时间		 
          //外接1.2M 2M分压电阻  第一次测量RC电路稳定时间 339uS
          //外接1M 0.2M分压电阻  第一次测量RC电路稳定时间 76uS
          switch(AdSet)
          {
          case 0://A0 200K:1M 
          case 1://A1 200K:1M
          case 2://A2 200K:1M
                //1个时钟周期(1/PHY_MCLK)秒
                    i = (PHY_MCLK_1MS>>5);	
                  while(--i);//4*i 个时钟周期 = 4/(1024*32)秒 = 122微秒
                    //i = (PHY_MCLK_1MS>>3);	
                  //while(--i);//4*i 个时钟周期 = 4/(1024*8)秒 = 488微秒
                break;
          default:
                 i = (PHY_MCLK_1MS>>3);	
                  while(--i);//4*i 个时钟周期 = 4/(1024*8)秒 = 488微秒
                break; 
          }
          
          ADC12IFG &= ~0x01 ;//先清空标记
          ADC12CTL0 &= ~ADC12SC;	// 开始保持转换
          
          //while(ADC12CTL1&ADC12BUSY); //等待到转换完成
          
          unsigned int time = PHY_MCLK_1MS;
          time = time*10;
          
          while( (!(ADC12IFG&0x01)) && (--time) ); //40ms 等待到转换完成
          if(time == 0)
          {
            ad = 0x8000;
            
            ADC12CTL0 &= ~ADC12ENC; 					// 改变设置前停止A/D转换
            ADC12CTL0 &= ~(ADC12REFON + ADC12ON);		//关闭内部参考电压，关闭ADC12核
            
            if(AdSet<=7)//A0~A7
                  P6SEL &= ~(0x01 << AdSet);		   //P6口选择 取消模拟输入功能
            else if((AdSet>=12) && (AdSet<=15)) //A12~A15
                  P7SEL &= ~(0x01 <<(AdSet-8));	   //P7口选择 取消模拟输入功能
            
            return ad;
          }           
          else
            ad = ADC12MEM0;
            
          lad += ad;
          
          if( ++c == 10)
            break;
      }
      
      lad = lad/c;
      
      ad = lad;
	
	ADC12CTL0 &= ~ADC12ENC; 					// 改变设置前停止A/D转换
	ADC12CTL0 &= ~(ADC12REFON + ADC12ON);		//关闭内部参考电压，关闭ADC12核
	
	if(AdSet<=7)//A0~A7
		P6SEL &= ~(0x01 << AdSet);		   //P6口选择 取消模拟输入功能
	else if((AdSet>=12) && (AdSet<=15)) //A12~A15
		P7SEL &= ~(0x01 <<(AdSet-8));	   //P7口选择 取消模拟输入功能
	
	return ad;
}

unsigned int ADC12SequenceSample(unsigned char AdSet)
{
  unsigned int ad;
  unsigned int k = 0;
  while(1)
  {
      ad = ADSequence(AdSet);
      if(ad & 0x8000)
          k++;
      else
          return ad;
      if(k > 10)
      {
          WDTCTL = WDT_ARST_1_9; //重新启动  
          while(1);
      }
  }
}

/****************************************************************************/
/*                        利用ADC12进行单通道单次扩展采样                   */
/*函数说明： ADC12数据采样控制  用到P6口, P6.0~P6.7对应A0~A7                */
/*				    P7口，p7.4~p7.7对应A12~A15。其中:       */
/*	  A0---- 板级电源电量检测,电压配比：200K:1M 			    */		 
/*	  A1-----输入电压DC-DC前的电池电量检测，电压配比：200K:1M	    */
/*	  A2-----太阳能或AC-DC前的电量检测,电压配比：200K:1M		    */
/*	  A3~A7,A12~A15接其他模拟传感器电压输入，RC时间常数需要设定 	    */
/*函数参数:                                                                 */
/*        unsigned char AdSet: 0~7,12~15对应需要采样的AD口A0~A7,A12~A15	    */
/*        unsigned char Ref:选择参考源。                                    */
/*                1---内部2.5V；2---外部电源；其他---内部1.5V               */
/*        unsigned int *Data:存放采集到的数据                               */
/*函数返回值:                                                               */
/*        unsigned int 返回采集并存放到*Data中的数据个数。若为0，则采集失败.*/
/****************************************************************************/
unsigned int ADCGetSingleChSingleData(unsigned char AdSet,unsigned char Ref,unsigned int *Data)
{ 
      //默认选择内部参考源
      unsigned char chP6[] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17}; 
      unsigned char chP7[] = {0x1C,0x1D,0x1E,0x1F};
        
      if(Ref == 2)  //选择外部电源 AVss AVcc做为参考源
      {
        chP6[0] = 0x00;
        chP6[1] = 0x01;
        chP6[2] = 0x02;
        chP6[3] = 0x03;
        chP6[4] = 0x04;
        chP6[5] = 0x05;
        chP6[6] = 0x06;
        chP6[7] = 0x07;

        chP7[0] = 0x0C;
        chP7[1] = 0x0D;
        chP7[2] = 0x0E;
        chP7[3] = 0x0F;
      }
	
      if(AdSet<=7)                        //A0~A7
        P6SEL |= (0x01 << AdSet);         //P6口选择 模拟输入功能
      else if((AdSet>=12) && (AdSet<=15)) //A12~A15
        P7SEL |= (0x01 <<(AdSet-8));	  //P7口选择 模拟输入功能
      else return 0;
	REFCTL0&=~ REFMSTR;   //msp430f5438a的电压参考模块
      ADC12CTL0 &= ~ADC12ENC; 	//改变设置前停止A/D转换
	
      if(Ref == 1)
	//内部参考电压 2.5V打开  ADC内核消耗打开 无中断
	ADC12CTL0 = ADC12REF2_5V + ADC12REFON + ADC12ON;
      else if(Ref == 2)
        //ADC内核消耗打开 无中断
	ADC12CTL0 = ADC12ON;        
      else 
         //内部参考电压 1.5V打开  ADC内核消耗打开 无中断
        ADC12CTL0 =  ADC12REFON + ADC12ON;
	
      //单通道单次采样，扩展采样模式，采样信号SAMPCON由采样输入信号控制
      //采样输入信号源为ADC12SC控制位
      //转换内核时钟源为系统主时钟
      //采样结果保存在ADC12MEM0
      ADC12CTL1 = ADC12CONSEQ_0+ ADC12SSEL_2; 
              
      //采样精度12bit，采样速率~50KSPS以降低功耗,参考电压状态通过P5.0输出
      //ADC12CTL2 = ADC12SR + ADC12RES1 + ADC12REFOUT;
              
      //采样精度12bit，采样速率~50KSPS以降低功耗
      ADC12CTL2 = ADC12SR + ADC12RES1 ;        
        
      //转换存储寄存器ADC12MEM0的控制寄存器
      if(AdSet<=7)//A0~A7
          ADC12MCTL0 = chP6[AdSet]; //选择采样通道  参考电压
      if((AdSet>=12) && (AdSet<=15)) //A12~A15
          ADC12MCTL0 = chP7[AdSet-12]; //选择采样通道  参考电压
	
      //等待参考电压建立起来 约20微秒        
      //1个时钟周期(1/PHY_MCLK)秒
      unsigned int i = (PHY_MCLK_1MS>>3);	
      while(--i);//4*i 个时钟周期 = 4/(1024*8)秒 = 488微秒
	
      ADC12CTL0 |= ADC12ENC; // 允许转换
      ADC12CTL0 |= ADC12SC;  // 开始采样	
      
      //扩展模式下的采样保持时间		 
      //外接1.2M 2M分压电阻  第一次测量RC电路稳定时间 339uS
      //外接1M 0.2M分压电阻  第一次测量RC电路稳定时间 76uS
      switch(AdSet)
      {
          case 0://A0 200K:1M 
          case 1://A1 200K:1M
          case 2://A2 200K:1M
                //1个时钟周期(1/PHY_MCLK)秒
                  i = (PHY_MCLK_1MS>>5);	
                  while(--i);//4*i 个时钟周期 = 4/(1024*32)秒 = 122微秒
                  
                  i = (PHY_MCLK_1MS>>4)-20;	
                  while(--i);//4*i 个时钟周期 = 4/(1024*16)秒 = 244微秒
                break;
          case 3://A3
                  i = (PHY_MCLK_1MS>>4)-20;	
                  while(--i);//4*i 个时钟周期 = 4/(1024*16)秒 =244微秒
                  break;
          default:
                 i = (PHY_MCLK_1MS>>4);	
                  while(--i);//4*i 个时钟周期 = 4/(1024*16)秒 =244微秒
                break; 
      }
	
      ADC12IFG &= ~0x01 ;       //先清空标记
      ADC12CTL0 &= ~ADC12SC;	// 开始保持转换
	       
      unsigned int time = PHY_MCLK_1MS;          
      while( (!(ADC12IFG&0x01)) && (--time) );          //等待到转换完成
      if(time == 0)
          *Data = 0x8000;
      else
          *Data = ADC12MEM0;
	
      ADC12CTL0 &= ~ADC12ENC; 			// 改变设置前停止A/D转换
      ADC12CTL0 &= ~(ADC12REFON + ADC12ON);		//关闭内部参考电压，关闭ADC12核
	
      if(time == 0)
        return 0;
      else
        return 1;
}

/****************************************************************************/
/*是函数ADCGetSingleChSingleData的扩展，添加多次采集失败后RESET             */
/*函数参数:                                                                 */
/*        unsigned char AdSet: 0~7,12~15对应需要采样的AD口A0~A7,A12~A15	    */
/*        unsigned char Ref:选择参考源。                                    */
/*                1---内部2.5V；2---外部电源；其他---内部1.5V               */
/*函数返回值:                                                               */
/*        unsigned int 返回采集到的数据                                     */
/****************************************************************************/
unsigned int ADCGetSingleChSingleDataEx(unsigned char AdSet,unsigned char Ref)
{
  unsigned int ad;
  unsigned int k = 0;
  while(1)
  {
      if( 0 == ADCGetSingleChSingleData(AdSet,Ref,&ad))
          k++;
      else
          return ad;
      if(k > 10)
      {
          WDTCTL = WDT_ARST_1_9; //重新启动  
          while(1);
      }
  }
}


/****************************************************************************/
/*A0口	 检测板级电源电压，采样值以 unsigned int返回			    */
/****************************************************************************/
unsigned int BoardEnergyDetect(void)
{
	unsigned int ad = 0;
	ad =ADCGetSingleChSingleDataEx(0,1);
	return ad;
}

/****************************************************************************/
/*A1口	 检测电池电源电压，采样值以 unsigned int返回			    */
/****************************************************************************/
unsigned int BattEnergyDetect(void)
{
	unsigned int ad = 0;
	ad =ADCGetSingleChSingleDataEx(1,1);
	return ad;
}

/****************************************************************************/
/*A2口  检测充电电源电压，采样值以 unsigned int返回			    */
/****************************************************************************/
unsigned int ChargerEnergyDetect(void)
{
	unsigned int ad = 0;
	ad =ADCGetSingleChSingleDataEx(2,1);
	return ad;
}

/****************************************************************************/
/*	  系统进入低功耗模式3													*/
/*unsigned char RFmode 设置RF休眠模式*/
/****************************************************************************/

void EnterLowPowerMode3(unsigned char RFmode)
{
	InitPower();   //关闭传感器电源，功放电源（关闭摄像头电源）
	
	LedGreenOff(); 
	LedRedOff();
	
        switch(RFmode)
        {
        case RF_SHUTDOWN:   //关闭模式
          SetRFOff();
          break;
        case RF_STANDBY:    //空闲模式
          SetRFStandby();
          break;
        case RF_SLEEP:      //睡眠模式
          SetRFSleep();
          break;
        case RF_READY:      //
          SetRFReady();
          break;
        case RF_TxTune:     //TX调制模式
          SetRFTuneForTx();
          break;
        case RF_RxTune:     //RX调制模式
          SetRFTuneForRx();
          break;
        default:
          SetRFStandby();
          break;          
        }	
	g_uchPHYSleepMode = PHY_SLEEP_MODE3;   //休眠标志置1；
	
	//if(g_uchPHYXt2Flag == EN_XT2CLK)
	//SetXT2ClkOff();
	
	LPM3;	 
        
        //SMCLK时钟选择:采用XT2
        //if(g_uchPHYSMclkSel == PHY_SMCLK_XT2)
        //  SetXT2ClkOn();
          
}

/****************************************************************************/
/* 低功耗模式3  豪秒级休眠													*/
/* 由时钟ACLK= 32768Hz 控制的 毫秒级休眠 ，参数 unsigned int ms 休眠的毫秒数*/
/*unsigned char RFmode 设置RF休眠模式*/
/****************************************************************************/
void Sleep_Mode3_ACLKms(unsigned int ms, unsigned char RFmode)
{
	g_uPHYSleppTicks = ms;
	//定时器选取ACLK: 低功耗模式下 MCLK已经关闭 全部关闭
	SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK_1MS);
        
        /* 进入到低功耗模式，同时RF进入到指定的模式
           单片机进入到LPM3模式，SMCLK,SMCLK关闭，主程序在此处停止执行，只执行中断程序
        */
	
        EnterLowPowerMode3(RFmode);  //MCU和RF都休眠,XT2ON	  3.6V ×0.126mA = 0.4536 mW (进入到低功耗模式，同时RF进入到指定的模式)  
        
	StopTimer0A0();  
}

/****************************************************************************/
/* 低功耗模式3 秒级休眠 												   */
/*由时钟ACLK= 32768Hz 控制的 秒级休眠 ，参数 unsigned int s 休眠的秒数	   */
/*unsigned char RFmode 设置RF休眠模式*/
/****************************************************************************/
void Sleep_Mode3_ACLKs(unsigned int s,unsigned char RFmode)
{
	g_uPHYSleppTicks = s;
	//定时器选取ACLK: 低功耗模式下 MCLK已经关闭 全部关闭
	SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK);     //====秒级的休眠状态								
	
        EnterLowPowerMode3(RFmode);//MCU和RF都休眠,XT2ON	 3.6V ×0.126mA = 0.4536 mW        
	StopTimer0A0();  
}

/****************************************************************************/
/* 向Data_ptr指向的FLASH里面写入一个字 word 								*/
/****************************************************************************/
void Flash_ww(unsigned int *Data_ptr,unsigned int word)
{
	FCTL3 = 0x0A500;
	FCTL1 = 0x0A540;
	*Data_ptr = word;
}

/****************************************************************************/
/* 向Data_ptr指向的FLASH里面写入一个字节byte								 */
/****************************************************************************/
void Flash_wb(unsigned char *Data_ptr,unsigned char byte)
{
	FCTL3 = 0x0A500;
	FCTL1 = 0x0A540;
	*Data_ptr = byte;
}


/****************************************************************************/
/* 清除Data_ptr指向的FLASH里面的字											*/
/****************************************************************************/
void Flash_clr(unsigned int *Data_ptr)
{
	FCTL1 = 0x0A502;
	FCTL3 = 0x0A500;
	*Data_ptr = 0;
}