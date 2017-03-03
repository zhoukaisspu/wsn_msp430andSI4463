
#include "PHYlib.h" 
#include "RFlib.h"
#include "global.h"
#include "NETlib.h"
#include "DLLlib.h"
#include "NETlib.h"
#include "APPlib.h"


/****************************************************************************/
/*                              ϵͳ��ʼ��                                  */
/****************************************************************************/
void InitSystem(void)
{    
     LowPowerPinInit();  //�����ʼ�����������еĹܽŽ��뵽����״̬
    //SMCLKʱ��ѡ��
     g_uchPHYSMclkSel = PHY_SMCLK_DCO;   
    //SMCLKʱ��Ƶ���趨
     g_uchPHYSMclkSet = PHY_SMCLK_4200KHZ;  
     InitClk();  
     RFCtlLineInit();                   //��ʼ�����ƶ˿�  ��Ƶ�ܽŵĳ�ʼ�� 
     InitPower();                      //��Դ��ʼ�����ر�����ͷ��Դ  ==�ر�����Ƶ�Դ
    
     LedGreenOff();                    //�̵ƹر�
     LedRedOff();                      //��ƹر�
     DisableRs232A0();                 //�ر�RS232 A0�Ĵ���оƬ     ǰ�˵�ͼ��ɼ����ڣ���վ��������ͻ��˽�������
     DisableRs232A3();                 //�ر�RS232 A3�Ĵ���оƬ     �ô�����ͼ��ɼ�ϵͳ�в�δ��             
     LedRedOn();LedGreenOn();         //���̵���
            
     HWResetRF();	            //��ƵӲ����λ
     InitRFChipWorkBand();           //��ʼ����Ƶ����Ƶ��
     Sound();
     _EINT();                          //�����ж�
}


void main()
{
InitSystem();

  while(1)
  {  
      //TxTest_11(0x00);    //12���ֽ����ݵķ��Ͳ��Ժ���
        TxTest_250(0x00);       //255���ֽڷ��Ͳ��Ժ���
       // RxTest(0x00);        //���ղ��Ժ���
 
  }  
}

/****************************************************************************/
/*                         ʱ��ʧЧ    �жϴ���                             */
/****************************************************************************/
#pragma vector=UNMI_VECTOR
__interrupt void UNMI_ISR(void)
{
    InitClk();
}
/****************************************************************************/
/*             ��ʱ��A0     �жϴ���     ==����ͼ��ɼ��Ķ�ʱ����                    */
/****************************************************************************/
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR(void)
{
	g_uPHYTimer0A0Count++; 	                       //���뵽��ʱ���ж�һ�Σ���ֵ++���ж���ʱ�˶೤ʱ��
        
        /*
        #define PHY_SLEEP_QUIT   0     //������
        �ж����ڴ�������ģʽ��������ʱ�䵽����� 
        */
        if((g_uchPHYSleepMode != PHY_SLEEP_QUIT) && ( g_uPHYTimer0A0Count >=g_uPHYSleppTicks)) //����
        {                         
            g_uPHYTimer0A0Count = 0;
            
            LPM3_EXIT;                              //��ʱ60ms���˳�LPM3ģʽ
            g_uchPHYSleepMode = PHY_SLEEP_QUIT;     
        }  
}

/**************************************************
            ��ʱ��TIMER0_A1�жϴ���
            ���ں����¶Ȳ���
**************************************************/
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR(void)
{
   /* switch(TA0IV)
    {
        case 4:  
         if(TemGuanCengWork==0x00)  //û����ʽ����
         {
            TA0CCTL2=CM_3+CAP+CCIE;
            TemGuanCengWork=0x01;    //��ʽ����        
            TA0R=0;
         }            //end if TemGuanCengWork==0x00
         else
         {
            if(TemGuanCengCount==0x00)
            {
               Temtpw=TA0CCR2;        //��Ч����ʱ��
               TemGuanCengCount++; 
            }
            else
            {
               TemTpw=TA0CCR2;
               TemGuanCengCount=0x00;
               TemGuanCengSuccess=0x00;          //���ݶ�ȡ�ɹ�    
               TA0CCTL2&=~CM_3;                  //ֹͣ����   
            }
         }     //end else TemGuanCengWork==0x01              
            break;
       case 6:                                    //T2���жϳ���T2�¶ȴ�������һֱ������
         if(TemEnvironmentWork==0x00)  //û����ʽ����
         {
            TA0CCTL3=CM_3+CAP+CCIE;
            TemEnvironmentWork=0x01;    //��ʽ����        
            TA0R=0;
         }            //end if TemGuanCengWork==0x00
         else
         {
            if(TemEnvironmentCount==0x00)
            {
               Temtpw=TA0CCR3;        //��Ч����ʱ��
               TemEnvironmentCount++; 
            }
            else
            {
               TemTpw=TA0CCR3;
               TemEnvironmentCount=0x00;
               TemEnvironmentSuccess=0x00;          //���ݶ�ȡ�ɹ�    
               TA0CCTL3&=~CM_3;                  //ֹͣ����   
            }
         }     //end else TemEnvironmentWork==0x01    
           break;          
       default: 
          break;
     }
  */
}
/****************************************************************************/
/*                       ��ʱ��A1     �жϴ��� 
                             ���ڶ�ʱ����
*/
/****************************************************************************/
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR(void)
{
	g_uPHYTimer1A0Count++;
}

/****************************************************************************/
/*                           ��ʱ��A1  �жϴ���
                             ���ں���Ĳ���ģʽ
*/
/****************************************************************************/
#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer1_A1_ISR(void)
{
               
}

/****************************************************************************/
/*                               ��ʱ��B0    �жϴ���   
                                  ���ڶ�ʱ����
*/
/****************************************************************************/
#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B_ISR (void)
{
        g_uPHYTimerBCount++;
        
}

/****************************************************************************/
/*        ���� USCIA0�������жϴ���������0���յ����ݴ���BufferUart0       */
/*       ====== ��վ�Լ�ǰ�˽ڵ㶼�õ��˸ô����ж�                         */
/****************************************************************************/
#pragma vector=USCI_A0_VECTOR

__interrupt void USCI_A0_ISR(void)
{
   /********************�����Ҫ�������������������*****************************/

   /*********************�����Ҫ�������������������*****************************/  
          g_uchDLLBuffer0Flag =1;                                  //��־���ڽ��յ�������        
            
          g_uchDLLBufferUart0[g_uchDLLBuffer0Tail] = UCA0RXBUF;
          g_uchDLLBuffer0Tail++;
          
          if(g_uchDLLBuffer0Tail == DLL_MAX_BUF0)    //���buffer�Ѿ����������ͷ��ʼ�ڴ�����
                  g_uchDLLBuffer0Tail = 0;
          if(g_uchDLLBuffer0Tail ==g_uchDLLBuffer0Head)     //====�����������������������̫�󣬵���ͷβ��ͬ
                  g_uchDLLBuffer0Tail = g_uchDLLBuffer0Head = 0;
}

/****************************************************************************/
/*        ���� USCIA1�������жϴ���������1���յ����ݴ���BufferUart1       */
/****************************************************************************/
#pragma vector=USCI_A1_VECTOR

__interrupt void USCI_A1_ISR(void)
{
   /********************�����Ҫ�������������������*****************************/

   /*********************�����Ҫ�������������������*****************************/  
           g_uchDLLBuffer1Flag =1;
           
          g_uchDLLBufferUart1[g_uchDLLBuffer1Tail] = UCA1RXBUF;
          g_uchDLLBuffer1Tail++;
          
          if(g_uchDLLBuffer1Tail == DLL_MAX_BUF1)
                  g_uchDLLBuffer1Tail = 0;
          if(g_uchDLLBuffer1Tail ==g_uchDLLBuffer1Head)
                  g_uchDLLBuffer1Tail = g_uchDLLBuffer1Head = 0;
}


/*******************************************************************************/
/*        ���� USCIA2�������жϴ���������2���յ����ݴ���BufferUart2           */
/********************************************************************************/
#pragma vector=USCI_A2_VECTOR
__interrupt void USCI_A2_ISR(void)
{
   /********************�����Ҫ�������������������*****************************/

   /*********************�����Ҫ�������������������*****************************/  
         /*  g_uchDLLBuffer2Flag =1;
           
          g_uchDLLBufferUart2[g_uchDLLBuffer2Tail] = UCA2RXBUF;
          g_uchDLLBuffer2Tail++;
          
          if(g_uchDLLBuffer2Tail == DLL_MAX_BUF2)
                  g_uchDLLBuffer2Tail = 0;
          if(g_uchDLLBuffer2Tail ==g_uchDLLBuffer2Head)
                  g_uchDLLBuffer2Tail = g_uchDLLBuffer2Head = 0;*/
}

/****************************************************************************/
/*        ���� USCIA3�������жϴ���������3���յ����ݴ���BufferUart3       */
/****************************************************************************/
#pragma vector=USCI_A3_VECTOR

__interrupt void USCI_A3_ISR(void)
{
   /********************�����Ҫ�������������������*****************************/

   /*********************�����Ҫ�������������������*****************************/  
           g_uchDLLBuffer3Flag =1;
           
          g_uchDLLBufferUart3[g_uchDLLBuffer3Tail] = UCA3RXBUF;
          g_uchDLLBuffer3Tail++;
          
          if(g_uchDLLBuffer3Tail == DLL_MAX_BUF3)
                  g_uchDLLBuffer3Tail = 0;
          if(g_uchDLLBuffer3Tail ==g_uchDLLBuffer3Head)
                  g_uchDLLBuffer3Tail = g_uchDLLBuffer3Head = 0;
}

/****************************************************************************/
/*                               P1��        �жϴ���                       */
/****************************************************************************/
#pragma vector=PORT1_VECTOR
__interrupt void port1_ISR (void)
{
    if(P1IFG & RF_SI_NIRQ_PIN) //P1.6 RF��������NIRQ�ж�
    {
      DINT_NIRQ;               //��ֹP1.6 NIRQ�ж�
      if( GET_NIRQ_BIT == 0 )//���ж�
       {            
	    g_iWirelessRxFlag = RFReceivePacket(g_uchWirelessRxBuf);
            
            /*if(g_iWirelessRxFlag == 0)    //������һ�����յ�һ�����������ݰ��������ز���ռ������ŵ�
            {
              g_iWirelessRxFlag = 0;
            } */
       }
       EINT_NIRQ;//�����ж�
       CLR_NIRQ_IFG;//����жϱ��  ������ܽŵ�IFG,û�ж�ȡ�жϱ�־λ
    }
}

