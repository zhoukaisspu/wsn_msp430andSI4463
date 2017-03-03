
#include "PHYlib.h" 
#include "RFlib.h"
#include "global.h"
#include "NETlib.h"
#include "DLLlib.h"
#include "NETlib.h"
#include "APPlib.h"


/****************************************************************************/
/*                              系统初始化                                  */
/****************************************************************************/
void InitSystem(void)
{    
     LowPowerPinInit();  //程序初始化，设置所有的管脚进入到高阻状态
    //SMCLK时钟选择
     g_uchPHYSMclkSel = PHY_SMCLK_DCO;   
    //SMCLK时钟频率设定
     g_uchPHYSMclkSet = PHY_SMCLK_4200KHZ;  
     InitClk();  
     RFCtlLineInit();                   //初始化控制端口  射频管脚的初始化 
     InitPower();                      //电源初始化，关闭摄像头电源  ==关闭闪光灯电源
    
     LedGreenOff();                    //绿灯关闭
     LedRedOff();                      //红灯关闭
     DisableRs232A0();                 //关闭RS232 A0的串口芯片     前端的图像采集串口，基站的数据与客户端交互串口
     DisableRs232A3();                 //关闭RS232 A3的串口芯片     该串口在图像采集系统中并未用             
     LedRedOn();LedGreenOn();         //红绿灯亮
            
     HWResetRF();	            //射频硬件复位
     InitRFChipWorkBand();           //初始化射频工作频段
     Sound();
     _EINT();                          //开启中断
}


void main()
{
InitSystem();

  while(1)
  {  
      //TxTest_11(0x00);    //12个字节数据的发送测试函数
        TxTest_250(0x00);       //255个字节发送测试函数
       // RxTest(0x00);        //接收测试函数
 
  }  
}

/****************************************************************************/
/*                         时钟失效    中断处理                             */
/****************************************************************************/
#pragma vector=UNMI_VECTOR
__interrupt void UNMI_ISR(void)
{
    InitClk();
}
/****************************************************************************/
/*             定时器A0     中断处理     ==用于图像采集的定时管理                    */
/****************************************************************************/
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR(void)
{
	g_uPHYTimer0A0Count++; 	                       //进入到定时器中断一次，该值++，判断延时了多长时间
        
        /*
        #define PHY_SLEEP_QUIT   0     //无休眠
        判断现在处于休眠模式，且休眠时间到达，唤醒 
        */
        if((g_uchPHYSleepMode != PHY_SLEEP_QUIT) && ( g_uPHYTimer0A0Count >=g_uPHYSleppTicks)) //唤醒
        {                         
            g_uPHYTimer0A0Count = 0;
            
            LPM3_EXIT;                              //延时60ms后退出LPM3模式
            g_uchPHYSleepMode = PHY_SLEEP_QUIT;     
        }  
}

/**************************************************
            定时器TIMER0_A1中断处理
            用于红外温度捕获
**************************************************/
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR(void)
{
   /* switch(TA0IV)
    {
        case 4:  
         if(TemGuanCengWork==0x00)  //没有正式工作
         {
            TA0CCTL2=CM_3+CAP+CCIE;
            TemGuanCengWork=0x01;    //正式工作        
            TA0R=0;
         }            //end if TemGuanCengWork==0x00
         else
         {
            if(TemGuanCengCount==0x00)
            {
               Temtpw=TA0CCR2;        //有效脉冲时间
               TemGuanCengCount++; 
            }
            else
            {
               TemTpw=TA0CCR2;
               TemGuanCengCount=0x00;
               TemGuanCengSuccess=0x00;          //数据读取成功    
               TA0CCTL2&=~CM_3;                  //停止捕获   
            }
         }     //end else TemGuanCengWork==0x01              
            break;
       case 6:                                    //T2的中断程序，T2温度传感器是一直工作的
         if(TemEnvironmentWork==0x00)  //没有正式工作
         {
            TA0CCTL3=CM_3+CAP+CCIE;
            TemEnvironmentWork=0x01;    //正式工作        
            TA0R=0;
         }            //end if TemGuanCengWork==0x00
         else
         {
            if(TemEnvironmentCount==0x00)
            {
               Temtpw=TA0CCR3;        //有效脉冲时间
               TemEnvironmentCount++; 
            }
            else
            {
               TemTpw=TA0CCR3;
               TemEnvironmentCount=0x00;
               TemEnvironmentSuccess=0x00;          //数据读取成功    
               TA0CCTL3&=~CM_3;                  //停止捕获   
            }
         }     //end else TemEnvironmentWork==0x01    
           break;          
       default: 
          break;
     }
  */
}
/****************************************************************************/
/*                       定时器A1     中断处理 
                             用于定时控制
*/
/****************************************************************************/
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR(void)
{
	g_uPHYTimer1A0Count++;
}

/****************************************************************************/
/*                           定时器A1  中断处理
                             用于红外的捕获模式
*/
/****************************************************************************/
#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer1_A1_ISR(void)
{
               
}

/****************************************************************************/
/*                               定时器B0    中断处理   
                                  用于定时控制
*/
/****************************************************************************/
#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B_ISR (void)
{
        g_uPHYTimerBCount++;
        
}

/****************************************************************************/
/*        有线 USCIA0，接收中断处理，将串口0接收的数据存入BufferUart0       */
/*       ====== 基站以及前端节点都用到了该串口中断                         */
/****************************************************************************/
#pragma vector=USCI_A0_VECTOR

__interrupt void USCI_A0_ISR(void)
{
   /********************如过需要，在下面添加其他程序*****************************/

   /*********************如过需要，在上面添加其他程序*****************************/  
          g_uchDLLBuffer0Flag =1;                                  //标志串口接收到了数据        
            
          g_uchDLLBufferUart0[g_uchDLLBuffer0Tail] = UCA0RXBUF;
          g_uchDLLBuffer0Tail++;
          
          if(g_uchDLLBuffer0Tail == DLL_MAX_BUF0)    //如果buffer已经存满，则从头开始在存数据
                  g_uchDLLBuffer0Tail = 0;
          if(g_uchDLLBuffer0Tail ==g_uchDLLBuffer0Head)     //====出现这种情况，肯能是数组太大，导致头尾相同
                  g_uchDLLBuffer0Tail = g_uchDLLBuffer0Head = 0;
}

/****************************************************************************/
/*        有线 USCIA1，接收中断处理，将串口1接收的数据存入BufferUart1       */
/****************************************************************************/
#pragma vector=USCI_A1_VECTOR

__interrupt void USCI_A1_ISR(void)
{
   /********************如过需要，在下面添加其他程序*****************************/

   /*********************如过需要，在上面添加其他程序*****************************/  
           g_uchDLLBuffer1Flag =1;
           
          g_uchDLLBufferUart1[g_uchDLLBuffer1Tail] = UCA1RXBUF;
          g_uchDLLBuffer1Tail++;
          
          if(g_uchDLLBuffer1Tail == DLL_MAX_BUF1)
                  g_uchDLLBuffer1Tail = 0;
          if(g_uchDLLBuffer1Tail ==g_uchDLLBuffer1Head)
                  g_uchDLLBuffer1Tail = g_uchDLLBuffer1Head = 0;
}


/*******************************************************************************/
/*        有线 USCIA2，接收中断处理，将串口2接收的数据存入BufferUart2           */
/********************************************************************************/
#pragma vector=USCI_A2_VECTOR
__interrupt void USCI_A2_ISR(void)
{
   /********************如过需要，在下面添加其他程序*****************************/

   /*********************如过需要，在上面添加其他程序*****************************/  
         /*  g_uchDLLBuffer2Flag =1;
           
          g_uchDLLBufferUart2[g_uchDLLBuffer2Tail] = UCA2RXBUF;
          g_uchDLLBuffer2Tail++;
          
          if(g_uchDLLBuffer2Tail == DLL_MAX_BUF2)
                  g_uchDLLBuffer2Tail = 0;
          if(g_uchDLLBuffer2Tail ==g_uchDLLBuffer2Head)
                  g_uchDLLBuffer2Tail = g_uchDLLBuffer2Head = 0;*/
}

/****************************************************************************/
/*        有线 USCIA3，接收中断处理，将串口3接收的数据存入BufferUart3       */
/****************************************************************************/
#pragma vector=USCI_A3_VECTOR

__interrupt void USCI_A3_ISR(void)
{
   /********************如过需要，在下面添加其他程序*****************************/

   /*********************如过需要，在上面添加其他程序*****************************/  
           g_uchDLLBuffer3Flag =1;
           
          g_uchDLLBufferUart3[g_uchDLLBuffer3Tail] = UCA3RXBUF;
          g_uchDLLBuffer3Tail++;
          
          if(g_uchDLLBuffer3Tail == DLL_MAX_BUF3)
                  g_uchDLLBuffer3Tail = 0;
          if(g_uchDLLBuffer3Tail ==g_uchDLLBuffer3Head)
                  g_uchDLLBuffer3Tail = g_uchDLLBuffer3Head = 0;
}

/****************************************************************************/
/*                               P1口        中断处理                       */
/****************************************************************************/
#pragma vector=PORT1_VECTOR
__interrupt void port1_ISR (void)
{
    if(P1IFG & RF_SI_NIRQ_PIN) //P1.6 RF接收数据NIRQ中断
    {
      DINT_NIRQ;               //禁止P1.6 NIRQ中断
      if( GET_NIRQ_BIT == 0 )//有中断
       {            
	    g_iWirelessRxFlag = RFReceivePacket(g_uchWirelessRxBuf);
            
            /*if(g_iWirelessRxFlag == 0)    //在这里一旦接收到一个完整的数据包，则发送载波，占有这个信道
            {
              g_iWirelessRxFlag = 0;
            } */
       }
       EINT_NIRQ;//允许中断
       CLR_NIRQ_IFG;//清除中断标记  ，清除管脚的IFG,没有读取中断标志位
    }
}

