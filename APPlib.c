#include "APPlib.h"

/*************************************************************************/
/*                         小数据发送测试                               */
/*                     参数 ch设定测试的通讯频点                        */
/*                       发送字节数：12Byte                              */
/*************************************************************************/
void TxTest_11(unsigned char ch)
{
    g_uchRFSelfFreNum = ch;             //自身频点
    
    g_uchConfigFlag = 1;
    
    FrequencyHop(g_uchRFSelfFreNum);
    
    g_uchWirelessTxBuf[0][0] = 12;
    g_uchWirelessTxBuf[0][1] ='z';
    g_uchWirelessTxBuf[0][2] ='h';
    g_uchWirelessTxBuf[0][3] ='o';
    g_uchWirelessTxBuf[0][4] ='u';
    g_uchWirelessTxBuf[0][5] ='k';
    g_uchWirelessTxBuf[0][6] ='a';
    g_uchWirelessTxBuf[0][7] ='i';
    g_uchWirelessTxBuf[0][8] ='7';
    g_uchWirelessTxBuf[0][9] ='_';
    g_uchWirelessTxBuf[0][10] ='$';  
    
    g_uchDLLFrameNum = 0x30;
    
    LedRedOff();
    
    while(1)
    {           
               g_uchWirelessTxBuf[0][11] =g_uchDLLFrameNum;
                    
               g_uchDLLFrameNum = (g_uchDLLFrameNum+1);
          
               if(g_uchDLLFrameNum>0x39)
                   g_uchDLLFrameNum = 0x30;                
                    
               while( RFSendPacket(g_uchWirelessTxBuf[0],g_uchWirelessTxBuf[0][0]) !=0 );   
             
               SetRFReady();
                    
               for( int i = 0 ; i< 500; i++)
                  Timer0A0_Delay_1ms_ACLK();
                
               LedGreenOff(); 
                
               for( int i = 0 ; i< 500; i++)
                  Timer0A0_Delay_1ms_ACLK();
                
               LedGreenOn();
    }
}
/**************************************************
小数据发送测试
发送字节数：21
**************************************************/
void TxTest_20(unsigned char ch)    //发送21字节
{
      g_uchRFSelfFreNum = ch;             //自身频点
    
    g_uchConfigFlag = 1;
    
    FrequencyHop(g_uchRFSelfFreNum);
    
    g_uchWirelessTxBuf[0][0] = 21;
    g_uchWirelessTxBuf[0][1] ='z';
    g_uchWirelessTxBuf[0][2] ='h';
    g_uchWirelessTxBuf[0][3] ='o';
    g_uchWirelessTxBuf[0][4] ='u';
    g_uchWirelessTxBuf[0][5] ='k';
    g_uchWirelessTxBuf[0][6] ='a';
    g_uchWirelessTxBuf[0][7] ='i';
    g_uchWirelessTxBuf[0][8] ='2';
    g_uchWirelessTxBuf[0][9] ='0';
    g_uchWirelessTxBuf[0][10] ='1';  
    g_uchWirelessTxBuf[0][11] ='4';
    g_uchWirelessTxBuf[0][12] ='1';
    g_uchWirelessTxBuf[0][13] ='1';
    g_uchWirelessTxBuf[0][14] ='2';
    g_uchWirelessTxBuf[0][15] ='2';
    g_uchWirelessTxBuf[0][16] ='s';
    g_uchWirelessTxBuf[0][17] ='s';
    g_uchWirelessTxBuf[0][18] ='p';
    g_uchWirelessTxBuf[0][19] ='u';
    g_uchDLLFrameNum = 0x30;
    
    LedRedOff();
    
    while(1)
    {           
               g_uchWirelessTxBuf[0][20] =g_uchDLLFrameNum;
                    
               g_uchDLLFrameNum = (g_uchDLLFrameNum+1);
          
               if(g_uchDLLFrameNum>0x39)
                   g_uchDLLFrameNum = 0x30;                
                    
               while( RFSendPacket(g_uchWirelessTxBuf[0],g_uchWirelessTxBuf[0][0]) !=0 );   
             
               SetRFReady();
                    
               for( int i = 0 ; i< 50; i++)
                  Timer0A0_Delay_1ms_ACLK();
                
               LedGreenOff(); 
                
               for( int i = 0 ; i< 50; i++)
                  Timer0A0_Delay_1ms_ACLK();
                
               LedGreenOn();
    }
}
/*************************************************************************/
/*                    大数据发送测试发送测试                             */
/*                     参数 :ch设定测试的通讯频点                        */
/*                      发送字节数：250Byte                              */
/*************************************************************************/
void TxTest_250(unsigned char ch)
{
    g_uchRFSelfFreNum = ch;             //自身频点
    
    g_uchConfigFlag = 1;
    
    FrequencyHop(g_uchRFSelfFreNum);
    
    g_uchWirelessTxBuf[0][0] = 251;
    //g_uchWirelessTxBuf[0][0] = 61;
    g_uchWirelessTxBuf[0][1] ='z';
    g_uchWirelessTxBuf[0][2] ='h';
    g_uchWirelessTxBuf[0][3] ='o';
    g_uchWirelessTxBuf[0][4] ='u';
    g_uchWirelessTxBuf[0][5] ='k';
    g_uchWirelessTxBuf[0][6] ='a';
    g_uchWirelessTxBuf[0][7] ='i';
    g_uchWirelessTxBuf[0][8] ='2';
    g_uchWirelessTxBuf[0][9] ='0';
    g_uchWirelessTxBuf[0][10] ='1';  
    g_uchWirelessTxBuf[0][11] ='4';
    g_uchWirelessTxBuf[0][12] ='1';
    g_uchWirelessTxBuf[0][13] ='1';
    g_uchWirelessTxBuf[0][14] ='2';
    g_uchWirelessTxBuf[0][15] ='2';
    g_uchWirelessTxBuf[0][16] ='s';
    g_uchWirelessTxBuf[0][17] ='s';
    g_uchWirelessTxBuf[0][18] ='p';
    g_uchWirelessTxBuf[0][19] ='u';
    g_uchDLLFrameNum = 0x30;
    
    for( ch=21; ch<251; ch++)
    {
      g_uchWirelessTxBuf[0][ch] =ch;
    }
    LedRedOff();
    
    while(1)
    {           
               g_uchWirelessTxBuf[0][20] =g_uchDLLFrameNum;
                    
               g_uchDLLFrameNum = (g_uchDLLFrameNum+1);
          
               if(g_uchDLLFrameNum>0x39)
                   g_uchDLLFrameNum = 0x30;                
                    
               while( RFSendPacket(g_uchWirelessTxBuf[0],g_uchWirelessTxBuf[0][0]) !=0 );   
             
               SetRFReady();
                    
               for( int i = 0 ; i< 500; i++)
                  Timer0A0_Delay_1ms_ACLK();
                
               LedGreenOff(); 
                
               for( int i = 0 ; i< 500; i++)
                  Timer0A0_Delay_1ms_ACLK();
                
               LedGreenOn();
    }
}


/**************************************************************************/
/*                   数据接受测试，同时适应与小数据与大数据               */
/*                   参数 :ch设定测试的通讯频点：                         */
/***************************************************************************/
void RxTest(unsigned char ch)
{
    g_uchRFSelfFreNum = ch;             //自身频点
    
    g_uchConfigFlag = 1;
    
   FrequencyHop(g_uchRFSelfFreNum);
    
    OpenUartA0(Baud_115200);  //打开串口，波特率115200
         
    EnableRs232A0();       //使能电平转换芯片
    
    LedRedOn();LedGreenOff(); 

    while(1)
    {
       LedGreenOff(); 
       if( GetWirelessPacket(6000) == 0x00)    //1秒钟对于接收255字节长数据包不够
       {
         LedGreenOn();        
         
         UartSend(UARTA0,0xaa);
         UartSend(UARTA0,0x55);  
         for(unsigned int i=1;i<g_uchWirelessRxBuf[0];i++)
                   UartSend(UARTA0,g_uchWirelessRxBuf[i]); 
         
       }
    }
}