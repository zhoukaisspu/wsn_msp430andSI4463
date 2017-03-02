/*****************************************************************
链路层解决的信道使用问题，如何占用一个信道来发送数据
*****************************************************************/
#include "DLLlib.h"

unsigned int g_uDLLLogInt = 300; //定义数据链路注册间隔时间： 以秒为单位

//链路质量定义
unsigned char g_uchDLLQ = DLL_LINK_QUAL;              //数据链路层全局变量，设定默认的网络通讯质量的较优值
unsigned char g_uchDLLCurLQ=0;                        //本级网络链路质量
unsigned char g_uchDLLSupLQ=0;                        //上级网络链路质量
unsigned int g_uDLLStaUp=0;                           //数据链路层全局变量，网络上行稳定量化值
unsigned int g_uDLLStaDown=0;                         //数据链路层全局变量，网络下行稳定量化值
unsigned int g_uDLLQ = DLL_LINK_QUAL;                 //数据链路层全局变量，设定默认的网络通讯质量的较优值

//       UARTA0 buffer  settings
unsigned char  g_uchDLLBufferUart0[DLL_MAX_BUF0];
unsigned char  g_uchDLLBuffer0Head = 0;
unsigned char  g_uchDLLBuffer0Tail = 0;
unsigned char  g_uchDLLBuffer0Flag = 0;

//       UARTA1 buffer  settings
unsigned char  g_uchDLLBufferUart1[DLL_MAX_BUF1];
unsigned char  g_uchDLLBuffer1Head = 0;
unsigned char  g_uchDLLBuffer1Tail = 0;
unsigned char  g_uchDLLBuffer1Flag = 0;

//       UARTA3 buffer  settings
unsigned char  g_uchDLLBufferUart3[DLL_MAX_BUF3];
unsigned char  g_uchDLLBuffer3Head = 0;
unsigned char  g_uchDLLBuffer3Tail = 0;
unsigned char  g_uchDLLBuffer3Flag = 0;


//======const:代表该值在运行过程之中不允许别改变，如果出现改变的状况，将会报错
const unsigned char g_cuchDLLUartSyncData[3] = {0xff,0xaa,0x55};  //Uart同步字序列定义
unsigned char g_uchDLLUartSync = 0;                 //Uart帧同步检验值


unsigned char g_uchWirelessTxBuf[BUFF_MAXPAGE][PAGESIZE] ={0};            //发送缓冲区

//帧缓冲控制字
//Bit位	7	:           1：帧有效         0：帧无效;
//Bit位	6       :           1：需要总回复     0：不需要总回复 
//Bit位	5       : 暂时保留无意义         	
//Bit位	4       :帧类型     1：起始帧         0：被分块的中间帧
//Bit位	3~0     : 帧尝试发送的次数，最大为10，超过10次还没发送成功，则设置帧无效
unsigned char g_uchWirelessTxBufCtl[BUFF_MAXPAGE] = {0};                 //帧缓冲控制字

unsigned char g_uchWirelessRxBuf[256] = {0};                  //接收帧缓冲

int g_iWirelessRxFlag = 1;//无线接收数据标记

unsigned char g_uchDLLFrameNum = 1;                   //当前信息帧编号

//避碰延迟时间定义
unsigned int g_uDLLCATime = DLL_TR_TIME;            //设备通讯发生避碰后所需要的延迟时间，范围（DLL_TR_TIME~10*DLL_TR_TIME）



/***********************************************/
/*      CRC16校验查表码                        */
/***********************************************/

const unsigned int g_cuDLLCRC16_table[256]={
	0x0,	0x1021,	0x2042,	0x3063,	0x4084,	0x50a5,	
		0x60c6,	0x70e7,	0x8108,	0x9129,	0xa14a,	0xb16b,	
		0xc18c,	0xd1ad,	0xe1ce,	0xf1ef,	0x1231,	0x210,	
		0x3273,	0x2252,	0x52b5,	0x4294,	0x72f7,	0x62d6,	
		0x9339,	0x8318,	0xb37b,	0xa35a,	0xd3bd,	0xc39c,	
		0xf3ff,	0xe3de,	0x2462,	0x3443,	0x420,	0x1401,	
		0x64e6,	0x74c7,	0x44a4,	0x5485,	0xa56a,	0xb54b,	
		0x8528,	0x9509,	0xe5ee,	0xf5cf,	0xc5ac,	0xd58d,	
		0x3653,	0x2672,	0x1611,	0x630,	0x76d7,	0x66f6,	
		0x5695,	0x46b4,	0xb75b,	0xa77a,	0x9719,	0x8738,	
		0xf7df,	0xe7fe,	0xd79d,	0xc7bc,	0x48c4,	0x58e5,	
		0x6886,	0x78a7,	0x840,	0x1861,	0x2802,	0x3823,	
		0xc9cc,	0xd9ed,	0xe98e,	0xf9af,	0x8948,	0x9969,	
		0xa90a,	0xb92b,	0x5af5,	0x4ad4,	0x7ab7,	0x6a96,	
		0x1a71,	0xa50,	0x3a33,	0x2a12,	0xdbfd,	0xcbdc,	
		0xfbbf,	0xeb9e,	0x9b79,	0x8b58,	0xbb3b,	0xab1a,	
		0x6ca6,	0x7c87,	0x4ce4,	0x5cc5,	0x2c22,	0x3c03,	
		0xc60,	0x1c41,	0xedae,	0xfd8f,	0xcdec,	0xddcd,	
		0xad2a,	0xbd0b,	0x8d68,	0x9d49,	0x7e97,	0x6eb6,	
		0x5ed5,	0x4ef4,	0x3e13,	0x2e32,	0x1e51,	0xe70,	
		0xff9f,	0xefbe,	0xdfdd,	0xcffc,	0xbf1b,	0xaf3a,	
		0x9f59,	0x8f78,	0x9188,	0x81a9,	0xb1ca,	0xa1eb,	
		0xd10c,	0xc12d,	0xf14e,	0xe16f,	0x1080,	0xa1,	
		0x30c2,	0x20e3,	0x5004,	0x4025,	0x7046,	0x6067,	
		0x83b9,	0x9398,	0xa3fb,	0xb3da,	0xc33d,	0xd31c,	
		0xe37f,	0xf35e,	0x2b1,	0x1290,	0x22f3,	0x32d2,	
		0x4235,	0x5214,	0x6277,	0x7256,	0xb5ea,	0xa5cb,	
		0x95a8,	0x8589,	0xf56e,	0xe54f,	0xd52c,	0xc50d,	
		0x34e2,	0x24c3,	0x14a0,	0x481,	0x7466,	0x6447,	
		0x5424,	0x4405,	0xa7db,	0xb7fa,	0x8799,	0x97b8,	
		0xe75f,	0xf77e,	0xc71d,	0xd73c,	0x26d3,	0x36f2,	
		0x691,	0x16b0,	0x6657,	0x7676,	0x4615,	0x5634,	
		0xd94c,	0xc96d,	0xf90e,	0xe92f,	0x99c8,	0x89e9,	
		0xb98a,	0xa9ab,	0x5844,	0x4865,	0x7806,	0x6827,	
		0x18c0,	0x8e1,	0x3882,	0x28a3,	0xcb7d,	0xdb5c,	
		0xeb3f,	0xfb1e,	0x8bf9,	0x9bd8,	0xabbb,	0xbb9a,	
		0x4a75,	0x5a54,	0x6a37,	0x7a16,	0xaf1,	0x1ad0,	
		0x2ab3,	0x3a92,	0xfd2e,	0xed0f,	0xdd6c,	0xcd4d,	
		0xbdaa,	0xad8b,	0x9de8,	0x8dc9,	0x7c26,	0x6c07,	
		0x5c64,	0x4c45,	0x3ca2,	0x2c83,	0x1ce0,	0xcc1,	
		0xef1f,	0xff3e,	0xcf5d,	0xdf7c,	0xaf9b,	0xbfba,	
		0x8fd9,	0x9ff8,	0x6e17,	0x7e36,	0x4e55,	0x5e74,	
		0x2e93,	0x3eb2,	0xed1,	0x1ef0};



/*********************** hamming code [7,4,3]编码*******************************/
const unsigned char _4bit_ham_table[16]={0x00,0x87,0x99,0x1e,0xaa,0x2d,0x33,
0xb4,0x4b,0xcc,0xd2,0x55,0xe1,0x66,0x78,0xff};

/****************************************************************************/
/*   通过UART发送 数据c                                                   */
/*   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3               */
/*                unsigned char c , 为待发送数据                          */
/*   函数返回值：int 型，0：发送成功；1：所选的串口未打开；2：无此串口    */
/****************************************************************************/
int UartSend(unsigned char com,unsigned char c )
{
  switch(com)
  {
    case UARTA0:
    {
      if(g_uchPHYUART0Open == 1)
      {
        UCA0TXBUF = c;
        while(UCA0STAT&UCBUSY);
        return 0;
      }
      else
        return 1;
    }
    case UARTA1:
    {
      if(g_uchPHYUART1Open == 1)
      {
        UCA1TXBUF = c;
        while(UCA1STAT&UCBUSY);
        return 0;
      }
      else
        return 1;
    }
    case UARTA3:
    {
      if(g_uchPHYUART3Open == 1)
      {
        UCA3TXBUF = c;
        while(UCA3STAT&UCBUSY);
        return 0;
      }
      else
        return 1;
    }
  default:          //====没有找到所选择的串口，返回没有此串口操作
    return 2;
  }
}
/****************************************************************************/
/*   Uart发送系统同步字                                                   */
/*   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3               */
/*   函数返回值：int 型，0：发送成功；1：所选的串口未打卡；2：无此串口    */
/****************************************************************************/
int UartSendSync(unsigned char com)
{
	unsigned char ch = 0;
	unsigned char len = sizeof(g_cuchDLLUartSyncData);
	int ret = 0;
        
	for(;ch<len;ch++) {  
		ret =UartSend(com,g_cuchDLLUartSyncData[ch]);
                if(ret !=0)
                  return ret;
	}
        return 0;
}

/****************************************************************************/
/*     从Uart的接收缓冲区中获取一个数据给 参数C,成功返回0，否则-1           */
/*   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3                 */
/*个人理解：g_uchDLLBuffer0Head代表新写入的一个数组的头
          g_uchDLLBuffer0Tail：代表新写入的一个数组的尾
          当数组的头和数组的尾相同的时候，代表新写入的数组已经读取完毕
         初始状况下：g_uchDLLBuffer0Head=0，g_uchDLLBuffer0Tail=0；*/
/****************************************************************************/
int GetUartData(unsigned char com ,unsigned char* c)        //====获取串口数据
{
	switch(com)
        {
          case UARTA0:
          {
            if(g_uchDLLBuffer0Head == g_uchDLLBuffer0Tail)       //====数组的头和尾是在同一个地方，没有收到数据，或者是收到的数组太大，导致头尾相连
                    return -1;
            
            *c = g_uchDLLBufferUart0[g_uchDLLBuffer0Head];
            g_uchDLLBuffer0Head++;
            
            if(g_uchDLLBuffer0Head == DLL_MAX_BUF0 )
            {
                g_uchDLLBuffer0Head = 0;
            }	
            return 0x00;
          }
          case UARTA1:
          {
            if(g_uchDLLBuffer1Head == g_uchDLLBuffer1Tail)
                    return -1;
            
            *c = g_uchDLLBufferUart1[g_uchDLLBuffer1Head];
            g_uchDLLBuffer1Head++;
            
            if(g_uchDLLBuffer1Head == DLL_MAX_BUF1 )
            {
                g_uchDLLBuffer1Head = 0;
            }	
            return 0x00;
          }
          case UARTA3:
          {
            if(g_uchDLLBuffer3Head == g_uchDLLBuffer3Tail)
                    return -1;
            
            *c = g_uchDLLBufferUart3[g_uchDLLBuffer3Head];
            g_uchDLLBuffer3Head++;
            
            if(g_uchDLLBuffer3Head == DLL_MAX_BUF3 )
            {
                g_uchDLLBuffer3Head = 0;
            }	
            return 0x00;
          }
        default:
          return -1;
        }          
}

/****************************************************************************/
/*     Uart的接收缓冲区清空                                               */
/*   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3                 */
/****************************************************************************/
void UartRxBufferClear(unsigned char com)
{
  switch(com)
  {
    case UARTA0:
    {
      g_uchDLLBuffer0Head = 0;
      g_uchDLLBuffer0Tail = 0;
      g_uchDLLBuffer0Flag = 0;
      break;
    }
    case UARTA1:
    {
      g_uchDLLBuffer1Head = 0;
      g_uchDLLBuffer1Tail = 0;
      g_uchDLLBuffer1Flag = 0;
      break;
    }
    case UARTA3:
    {
      g_uchDLLBuffer3Head = 0;
      g_uchDLLBuffer3Tail = 0;
      g_uchDLLBuffer3Flag = 0;
      break;
    }
  default:
    break;
  }
}

/****************************************************************************/
/*  参数unsigned int overtime设定了接收无线数据包的时间限制                 */
/*  函数返回值 ：0: 接收成功；数据保存在全局变量g_uchWirelessRxBuf
                -1：工程地址不匹配;
                -2: 没有收到任何数据                         
                -3:收到数据但是CRC校验出错                                  
                -4:收到同步字和部分数据,但数据接收超时失败；            
*/
 //返回值       1：在规定的时间内没有收到任何数据               
/*overtime与全局变量g_uPHYTimer0A0Count进行比较，限定在多少时间内接收无线数据包*/
/****************************************************************************/ 
int GetWirelessPacket(unsigned int overtime)
{
        
        int ret;//返回值
        
        unsigned char maxRSSI = 0;
        unsigned char RSSI = 0;        
        
        SetRFRx();                   //====在这里执行了清除FIFO的指令
        
       // CLR_NIRQ_IFG;//清除中断标记
        EINT_NIRQ;

	SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK_1MS);                       //搜寻60ms
        
	while((g_iWirelessRxFlag == 1)&&(g_uPHYTimer0A0Count<overtime))   //限定时间内等待无线数据
	{
          //RSSI = GetRSSI();    //获取最大的RSSI
         // RSSI=GetFastLatchRSSI();
          if(RSSI >maxRSSI)
            maxRSSI = RSSI;        
        }
        StopTimer0A0();
        
	DINT_NIRQ;
        
        g_uchRSSI = maxRSSI;
        
	ret =  g_iWirelessRxFlag;    //先将返回值返回的然后在将标志位改变为默认值的		
	g_iWirelessRxFlag = 1;
	
	//if(ret == 0)//做CRC校验
	//{
	//  if( GenerateCRC(g_uchWirelessRxBuf,g_uchWirelessRxBuf[0])!=0 ) //CRC校验错误
	//    return -3;
	//}
	return ret;
}

/****************************************************************************/
/*     无线通讯接收缓冲区清空                                               */
/****************************************************************************/
void WirelessRxBufferClear(void)
{
  RFResetRxFIFO();
}

/*****************************************************************************/
/* 产生CRC校验码的函数，参数DataBuff是要校验的数据，或者需要生成校验码的数据 */
/* 注意：DataBuff默认最后两字节存放CRC校验码，len是添加校验码后的DataBuff大小*/
/* 函数返回值：unsigned int型CRC码数据                                       */
/*****************************************************************************/
unsigned int GenerateCRC(unsigned char *DataBuff,unsigned int len)
{
	unsigned int oldcrc16;
	unsigned int crc16;
	unsigned int oldcrc;
	unsigned int charcnt;
	unsigned char c,t;
	oldcrc16 = 0x0000; //初值为0
	charcnt=0;
	
	while (len--) {
		t= (oldcrc16 >> 8) & 0xFF; //要移出的字节的值
		oldcrc=g_cuDLLCRC16_table[t]; //根据移出的字节的值查表
		c=DataBuff[charcnt]; //新移进来的字节值
		oldcrc16= ((oldcrc16 << 8)) | c; //将新移进来的字节值添在寄存器末字节中
		oldcrc16=oldcrc16^oldcrc; //将寄存器与查出的值进行xor 运算
		charcnt++;
	}
	crc16=oldcrc16;
	return crc16;
}

/*****************************************************************************/
/* 产生帧的CRC字节                                                           */
/*  TxData为要添加CRC校验的数据帧，其中TxData[0]表示待发送数据帧的长度       */
/*  TxData最后两个地址存放CRC校验后的值                                      */
/*****************************************************************************/
void AddCRC(unsigned char *TxData)
{
	unsigned int CRC;
	TxData[TxData[0]-2] = TxData[TxData[0]-1] = 0x00;
	CRC = GenerateCRC(TxData,TxData[0]); //CRC编码值
	TxData[TxData[0]-2] = ((CRC>>8)&0x0ff);  
	TxData[TxData[0]-1] = (CRC&0x0ff);	
}

/****************************************************************************/
/*获取Uart同步字头, 同步字由全局变量数组 g_uchDLLWireSyncData定义         */
/*        函数返回值: int 类型，  -1----同步失败 ,  0----同步成功           */
/*   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3                 */
/****************************************************************************/
int GetUartSyncWord(unsigned char com)
{
        unsigned char c = 0x00;
	
	if(0 == GetUartData(com,&c))
	{
		switch(g_uchDLLUartSync)
		{
		case 0:
			{
				if(c == g_cuchDLLUartSyncData[0])     //=====获取的是同步字的第一个字节
					g_uchDLLUartSync = 1;
				break;
			}
		case 1:
			{
				if(c == g_cuchDLLUartSyncData[0])           //=====
					break;
				if(c ==g_cuchDLLUartSyncData[1])
					g_uchDLLUartSync = 2;
				else
					g_uchDLLUartSync = 0;        //===收到的字节错误
				break;
			}         
			
		case 2:
			{
				g_uchDLLUartSync = 0;			        
				if(c == g_cuchDLLUartSyncData[2])
					return 0;                        //=====返回值为0的时候，代表同步字提取完毕
				break;
			}
		default:
			break;
		}
		return -1;
	}
	return -1;
}

/*****************************************************************************/
/* 在限定时间内接收Uart信息帧：
   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3                  */
/*                 RxData存放收到的数据,RxData[0]为收到数据长度              */
/*                           TimeLimit存放限定时间，单位毫秒                 */
/* 函数返回值： 0 －－收到数据帧，并存储到RxData                             */
/*             -1－－超时未收到有效数据                                      */
/*****************************************************************************/
int UartRxFrameData(unsigned char com,unsigned char *RxData, unsigned int TimeLimit)
{	
	unsigned int limit = TimeLimit;//限定时间
	unsigned char c,i;
	
        SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK_1MS);	
	while((0 != GetUartSyncWord(com))&&(g_uPHYTimer0A0Count<limit)); //限定时间内等待数据，当收到完整的同步字或者超过定时时间就会跳出while循环
	StopTimer0A0();        

	if(g_uPHYTimer0A0Count >= limit)   //如果是定时器溢出，则在固定时间内没有获取完整的同步字，数据获取错误
	{
	  //g_uchDLLWireFlag = 0;
	  return -1;//超时
	}	
      
        SetTimer0A0_ACLK();                              //====在2毫秒之内获取一个字节的数据，且只取得一个字节的数据，获取一个字节的数据之后跳出while循环
	Timer0A0_Delay(PHY_ACLK_1MS);                                       //====获取串口数据，获取的一个字节的数据，是字节的帧长
	while((0 != GetUartData(com,&c))&&(g_uPHYTimer0A0Count<2));         //2毫秒内取得一个字节数据
	StopTimer0A0();             //====去的一个字节的数据，返回值为0

	if(g_uPHYTimer0A0Count >= 2)       //====确定不是因为定时器溢出而产生的跳出while循环
	{
	  //g_uchDLLWireFlag = 0;
	  return -1;//超时
	}
	
	RxData[0] = c; //得到帧长
	
	i = 1;
	
	while(i<RxData[0])
	{
		
          SetTimer0A0_ACLK();
          Timer0A0_Delay(PHY_ACLK_1MS);	
          while((0 != GetUartData(com,&c))&&(g_uPHYTimer0A0Count<2)); //2毫秒内取得一个字节数据
          StopTimer0A0();        
  
          if(g_uPHYTimer0A0Count >= 2)
            return -1;//超时
	
	  RxData[i++] = c; //保存数据
	}
	
	//g_uchDLLWireFlag = 0;
	return 0;	
}

/*****************************************************************************/
/* Uart发送数据帧
   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3                  */
/*                 TxData存放即将发送的数据，TxData[0]存放发送数据长度     */
/*   函数返回值：int 型，0：发送成功；1：所选的串口未打卡；2：无此串口    */
/*****************************************************************************/
int UartTxFrameData(unsigned char com,unsigned char *TxData)
{
	unsigned char count= TxData[0];
	int ret = UartSendSync(com);
        
        if(ret != 0)
          return ret;
        
	for(unsigned char i=0; i<count; i++)
	{
		ret = UartSend(com,TxData[i]);   //发送有效数据
                if(ret != 0)
                  return ret;
	} 
        return 0;
}

/*****************************************************************************************/
/*获取随机的初始频点序列                                                                 */
/*生成的频点序列放入：A区：uchFreNumA[RF_FRE_AMNUM_A]                                    */
/*                   个数：0~(RF_FRE_AMNUM_A-1)                                          */
/*                    B区：uchFreNumB[RF_FRE_AMNUM_B]                                    */
/*                    个数:RF_FRE_AMNUM_A ~ (RF_FRE_AMNUM_A+RF_FRE_AMNUM_B-1)            */
/*以设备MAC地址+信号强度为随机数种子                                                     */
/*****************************************************************************************/
void GetRandomFreList()
{
	unsigned char i,j,temp;
	unsigned char rssi = 0; //获取随机信号强度
	unsigned char uchFreAmount;
	typedef union {
		unsigned int uRandSeed;
		unsigned char ucRandSeed[2];
	} RandSeed;
	
	RandSeed rd;
	
	rd.ucRandSeed[0]=g_PHYDevInfo.DevMAC[0];
	rd.ucRandSeed[1]=g_PHYDevInfo.DevMAC[1];	

	uchFreAmount = RF_FRE_A_AMOUNT+RF_FRE_B_AMOUNT;
	
	rssi = GetRSSI();
	srand(rd.uRandSeed + rssi);
	
	for(i=0; i<uchFreAmount; i++)
	{
		g_chRFFreList[i] = i;
	}
	
	for(i=0; i<RF_FRE_A_AMOUNT; i++)//不加功放频点 0~RF_FRE_A_AMOUNT-1
	{
		temp = g_chRFFreList[i];
		j = rand()%(RF_FRE_A_AMOUNT-i) + i; //保证从i到RF_FRE_A_AMOUNT-1之间选取
		g_chRFFreList[i] = g_chRFFreList[j];
		g_chRFFreList[j] = temp;
	}
	
	for(i=RF_FRE_A_AMOUNT; i<(RF_FRE_A_AMOUNT+RF_FRE_B_AMOUNT); i++)//加功放频点 RF_FRE_AMNUM_A ~ (RF_FRE_AMNUM_A+RF_FRE_AMNUM_B-1)
	{
		temp = g_chRFFreList[i];
		j = rand()%(RF_FRE_A_AMOUNT+RF_FRE_B_AMOUNT-i) + i; //保证从i到 RF_FRE_AMNUM_A ~ (RF_FRE_AMNUM_A+RF_FRE_AMNUM_B-1)之间选取
		g_chRFFreList[i] = g_chRFFreList[j];
		g_chRFFreList[j] = temp;
	}
}

/*****************************************************************************/
/* 异级信道竞争机制算法 ，函数形参：当前竞争的异级频点，可能是上级频点       */
/*                                                     也可能是下级频点       */
/*      信道竞争成功：异级频点转入发射态占用此频点，并返回 0                 */
/*      信道竞争失败：本级频点转入接收态，并返回 -1                          */
/*****************************************************************************/
int DiffChDetect(unsigned char uchFreNo)
{
	unsigned char Tslice;
	
	unsigned char uchFreNum = uchFreNo;
	
	Tslice = DLL_SLICE_TIME;    //初始竞争最小时间片=2
	    
      FrequencyHop(uchFreNum);  //转入异级频点，或是上级，或是下级 进入接收态
	
	if(GetRSSIPower() < RF_LISTEN_RSSI) //载波监测信号值小于侦听下限，信道空闲
	{                
                
                //射频芯片ready
                SetRFReady();                
                SetTimer0A0_ACLK();
                Timer0A0_Delay(PHY_ACLK_1MS);	
                while(g_uPHYTimer0A0Count<2*Tslice); //延迟时间T=2*Tslice
                StopTimer0A0(); 
          
                //Sleep_Mode3_ACLKms(2*Tslice,RF_READY);

                SetRFRx();//转入接收态
		
		if(GetRSSIPower() < RF_LISTEN_RSSI)  //信道还空闲
		{			
                  //SetRFReady();
			return(0); //返回竞争成功
		}
		else
		{
			//FrequencyHop(g_uchRFSelfFreNum);  //转入本级频点
			
                  //SetRFStandby();
                        
			return(-1);  //返回竞争失败
		}
	}
	else
	{
		//FrequencyHop(g_uchRFSelfFreNum);  //转入本级频点

                //SetRFStandby();
                
		return(-1);  //返回竞争失败
	}
}


/*****************************************************************************/
/*     本级信道竞争机制算法 ，函数形参：                                         */
/*     信道竞争成功：调整进入发射态，占用频点，并返回 0                      */
/*     信道竞争失败：                            返回 -1 （处于接收态）      */

/*****************************************************************************/
int SimiChDetect(unsigned char uchCurFreNum)
{
	unsigned char Tslice;
	unsigned char n; //随机数 0~5 
	unsigned char uchCurFreNo = uchCurFreNum;

	Tslice = DLL_SLICE_TIME;//初始竞争最小时间片=2
	
      FrequencyHop(uchCurFreNo);  //进入本级频点 转入接收态

	if(GetRSSIPower() < RF_LISTEN_RSSI)  //监听到小于侦听下限 ，当前信道空闲
	{                
                
                //射频芯片ready
                SetRFReady();                
                SetTimer0A0_ACLK();
                Timer0A0_Delay(PHY_ACLK_1MS);	
                while(g_uPHYTimer0A0Count<2*Tslice); //延迟时间T=2*Tslice
                StopTimer0A0(); 
                         
                //Sleep_Mode3_ACLKms(2*Tslice,RF_READY);

                SetRFRx();//转入接收态
		
		if(GetRSSIPower() < RF_LISTEN_RSSI)  //信道还空闲
		{    
                        srand(GetRSSI());  //以RSSI值为随机种子
			      n = rand()%6;  
                        
                        //射频芯片ready
                        SetRFReady();                          
                        SetTimer0A0_ACLK();
                        Timer0A0_Delay(PHY_ACLK_1MS);	
                        while(g_uPHYTimer0A0Count<n*Tslice); //延迟时间T=n*Tslice
                        StopTimer0A0();
                        
                        //Sleep_Mode3_ACLKms(n*Tslice,RF_READY);

                        SetRFRx();//转入接收态
			
			if(GetRSSIPower() < RF_LISTEN_RSSI)  //监听到小于侦听下限 ，当前信道还空闲
			{
                          //SetRFReady();
			        return(0); //返回竞争成功
			}
		}		
	}

      //SetRFStandby();
        
	return(-1);//返回失败
}



/********************************************************************************/
/*与ID为DevID的设备解除拓扑关系　　　　　　　　　　　　　　　　　               */
/********************************************************************************/
void ReleaseRelation(unsigned int DevID)
{
	if(DevID == g_uNetParentID)
	{
		g_uDLLStaUp = 0;  //上行网络稳定量化值置0
	}
	else 
	{
		for(char count=0; count<NET_MAX_CAP; count++)
		{
			if(DevID == g_SubDevInfo[count].NetID)
			{ 
				memset((void *) & g_SubDevInfo[count],0x00,sizeof(g_SubDevInfo[count]));  //删除该设备
				break;
			}   
		}
	}
}


/********************************************************************************/
/*异级通讯　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　          */
/*   信道检测，无干扰，发送网络查询帧，接收确认回复                             */
/*   正确确认  返回 0                                                           */
/*       其他  修改相应信息，返回 -1                                            */
/*unsigned char uchFreNum  即将通讯的异级频点                                   */
/*int BuffFrmNo  待发送帧在帧缓冲中的编号                                       */
/********************************************************************************/
int DiffFreqCommunication(unsigned char uchFreNo,int BuffFrmNum)
{
	int returnval;
	unsigned int uUpID;
	unsigned char uchFreNum = uchFreNo;
	int BuffFrmNo = BuffFrmNum;
	
	if(0 == DiffChDetect(uchFreNum))  //调用异级信道竞争算法成功
	{ 
		RFSendPacket(g_uchWirelessTxBuf[BuffFrmNo], g_uchWirelessTxBuf[BuffFrmNo][0]);
		returnval = GetWirelessPacket(60);
		
		FrequencyHop(g_uchRFSelfFreNum);  //进入本级频点接收态

            //射频芯片ready
            SetRFReady();
		
		if((returnval == 0) && (g_uchWirelessRxBuf[11]==0x55) && (g_uchWirelessRxBuf[12]==0xa0))//是拒绝访问确认
		{
		    g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //从缓冲区中清除该帧
		    ReleaseRelation(g_uchWirelessRxBuf[3]+(unsigned int)(g_uchWirelessRxBuf[4]*256));//与发送拒绝帧的设备解除拓扑关系
		    
		    if( uchFreNum == g_uchNetParentFreqNo )	//异级通讯在 父频点
		        g_uDLLStaUp = 0;	
                
		    return(-1); //返回传输信息失败 
		
		}//end if 是拒绝访问确认		
		
		//Tres时间内收到 除了拒绝访问帧外的确认帧 并且目的地址给自己的
		if( (returnval == 0)  && (g_uchWirelessRxBuf[11]==0x55) && ( g_uchWirelessRxBuf[1] == g_uchWirelessTxBuf[BuffFrmNo][3]) && (g_uchWirelessRxBuf[2] == g_uchWirelessTxBuf[BuffFrmNo][4]))
		{	
			g_uchWirelessTxBufCtl[BuffFrmNo] = ( (g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ((g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));//帧传输尝试次数加1
                                                
                        if( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F) > FRAME_MAX_SEND_TIMES) //帧尝试次数大于最大次
                                    g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//从缓冲区中清除该帧 			
		        
			if(g_uchWirelessRxBuf[12] == g_uchWirelessTxBuf[BuffFrmNo][11]) //正确确认 
			{						
                                uUpID = ((unsigned int)g_uchWirelessTxBuf[BuffFrmNo][2]*256) + g_uchWirelessTxBuf[BuffFrmNo][1];  
                                if(uUpID == g_uNetParentID)  //缓存中帧目的地址是上级基站
                                {                                      
                                       g_uDLLStaUp = DLL_MAX_STAUP ; //上行链路系数赋值最大                                  
                                }	
				
				if((g_uchWirelessTxBufCtl[BuffFrmNo]&0x40) != 0x40)//帧控制字bit6不为1，不需要总回复
	                            g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //从缓冲区中清除该帧
							
				g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
							
				switch(g_uchWirelessRxBuf[12])//判断收到的回复帧内容
				{				    
					case 0x40: //数据帧确认
					{
					    if( (g_uchWirelessRxBuf[17] <=254) && (g_uchWirelessRxBuf[17]>0) )
					        g_uAppDataInt = g_uchWirelessRxBuf[17]*60; //以秒为单位,设定注册间隔时间
					    else
					        g_uAppDataInt = 21600;//六小时
					    //重新计算当前设备链路质量
					    LQEvaluation(g_uchWirelessRxBuf[14],g_uchRSSI,g_uchWirelessRxBuf[16],g_uDLLStaUp);
									
					    //修改当前网络级别
					    g_uchNetLev = g_uchWirelessRxBuf[15]+1;
									
					    break;
				        }
					case 0x30://转发的申请注册帧的确认
					{
					     if( (g_uchWirelessRxBuf[16] <=254) && (g_uchWirelessRxBuf[16]>0) )
					        g_uAppDataInt = g_uchWirelessRxBuf[16]*60; //以秒为单位,设定注册间隔时间
					    else
					        g_uAppDataInt = 21600;//六小时
					        
                                            //重新计算当前设备链路质量
                                            //LQEvaluation(g_uchWirelessRxBuf[14],g_uchRSSI,g_uchWirelessRxBuf[16],g_uDLLStaUp);									
                                            //修改当前网络级别
                                            g_uchNetLev = g_uchWirelessRxBuf[15]+1;
					}
					default:
					    break;
				}//end switch
				
				return 0;
							
			}//end if 正确确认
			    
                        else//出错  网络忙 等其他确认
                        {
			        uUpID = ((unsigned int)g_uchWirelessTxBuf[BuffFrmNo][2]*256) + g_uchWirelessTxBuf[BuffFrmNo][1];  
                                if(uUpID == g_uNetParentID)  //缓存中帧目的地址是上级基站
                                {                                          
                                      if(g_uDLLStaUp > DLL_STAUP_RIGHT)
                                          g_uDLLStaUp -= DLL_STAUP_RIGHT ; // 减的权重加大 
                                      else
                                       g_uDLLStaUp = 0;  
                                }
                                
				return 1;//暂时返回1
			}			
		}//end if //Tres时间内收到 除了拒绝访问帧外的确认帧 并且目的地址给自己的
		
		else  //Tres时间内没收到确认帧
		{ 
			uUpID = ((unsigned int)g_uchWirelessTxBuf[BuffFrmNo][2]*256) + g_uchWirelessTxBuf[BuffFrmNo][1];  
			if(uUpID == g_uNetParentID)  //缓存中帧目的地址是上级基站
			{                                     
                                    if(g_uDLLStaUp > DLL_STAUP_RIGHT)
                                      g_uDLLStaUp -= DLL_STAUP_RIGHT ; // 减的权重加大 
                                    else
                                       g_uDLLStaUp = 0;                                     
			}
		}
	} //end if(0 == DiffChDetect(uchFreNum)) 
	
	else  //竞争不成功
	{
		uUpID =  ((unsigned int)g_uchWirelessTxBuf[BuffFrmNo][2]*256) + g_uchWirelessTxBuf[BuffFrmNo][1];  
		if(uUpID == g_uNetParentID) //缓存中帧目的地址是上级基站
		{
                              if(g_uDLLStaUp > DLL_STAUP_RIGHT)
                                   g_uDLLStaUp -= DLL_STAUP_RIGHT ; //减的权重加大 
                              else
                                   g_uDLLStaUp = 0;                                     
		}
		
		FrequencyHop(g_uchRFSelfFreNum);  //进入本级频点接收态
	} 
	
	g_uchWirelessTxBufCtl[BuffFrmNo] = ( (g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ((g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));//帧传输尝试次数加1
	if( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F) > FRAME_MAX_SEND_TIMES) //帧尝试次数大于最大次数
		g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//从缓冲区中清除该帧     

        return(-1);  //返回传输信息失败         
	
}


/********************************************************************************/
/*基站本级通讯                                                                  */
/*   信道检测，无干扰，发送网络查询帧，接收确认回复                             */
/*   正确确认  返回 0                                                           */
/*       其他  修改相应信息，返回 -1                                            */
/* BuffFrmNo：待发送帧在帧缓冲中的编号                                       */
/********************************************************************************/
int BaseSelfFreqCommunication(int BuffFrmNum)
{  
	int returnval;
	int BuffFrmNo = BuffFrmNum;
	
	if(0 == SimiChDetect(g_uchRFSelfFreNum)) //调用本级信道竞争成功
	{
		RFSendPacket(g_uchWirelessTxBuf[BuffFrmNo],g_uchWirelessTxBuf[BuffFrmNo][0]);
		returnval = GetWirelessPacket(60);
                
            //射频芯片ready
            SetRFReady();
		
		if((returnval == 0) && (g_uchWirelessRxBuf[11]==0x55) && (g_uchWirelessRxBuf[12]==0xa0))//是拒绝访问确认
		{
		    g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //0从缓冲区中清除该帧
		    ReleaseRelation(g_uchWirelessRxBuf[3]+(unsigned int)(g_uchWirelessRxBuf[4]*256));//与发送拒绝帧的设备解除拓扑关系
		   
		    g_uDLLStaDown = 0;
                    
		    return(-1); //返回传输信息失败 
		
		}//end if 是拒绝访问确认
		
		//Tres时间内收到 除了拒绝访问帧外的确认帧 并且目的地址给自己的
		if( (returnval == 0)  && (g_uchWirelessRxBuf[11] == 0x55) && (g_uchWirelessRxBuf[1] ==g_uchWirelessTxBuf[BuffFrmNo][3]) && (g_uchWirelessRxBuf[2] ==g_uchWirelessTxBuf[BuffFrmNo][4]))
		{		        										
                            g_uchWirelessTxBufCtl[BuffFrmNo] = ( (g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ((g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));//帧传输尝试次数加1
                            if( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F) > FRAME_MAX_SEND_TIMES) //帧尝试次数大于最大次
                                    g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//从缓冲区中清除该帧  						        
     
			    if(g_uchWirelessRxBuf[12] == g_uchWirelessTxBuf[BuffFrmNo][11]) //正确确认 
			    {				        
				if(g_uDLLStaDown < DLL_MAX_STADOWN)
			            g_uDLLStaDown++;  //下行稳定量化值+1	
						    			        
				if((g_uchWirelessTxBufCtl[BuffFrmNo]&0x40) != 0x40)//帧控制字bit6不为1，不需要总回复
	                            g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //从缓冲区中清除该帧
	                                            
				g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
				
                                return 0;
				
			    }//end if 正确确认
			    else//出错  网络忙 等其他确认
			    {
				if(g_uDLLStaDown>0)
				     g_uDLLStaDown--;  //下行稳定量化值-1
                                
				return 1;//暂时返回1
			    }					
		}//end if Tres时间内收到 除了拒绝访问帧外的确认帧 并且目的地址给自己的
		else
		{    
			if(g_uDLLStaDown>0)
				g_uDLLStaDown--;  //下行稳定量化值-1			
		}
	}//end if 调用本级信道竞争成功	
	else
	{ 
		if(g_uDLLStaDown>0)  
			g_uDLLStaDown--;  //下行稳定量化值-1
	}
	
	if(g_uDLLStaDown <=0)            
	    return -1; 

	g_uchWirelessTxBufCtl[BuffFrmNo] = ( (g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));
	if(g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F > FRAME_MAX_SEND_TIMES) //帧尝试次数大于最大次
		g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//从缓冲区中清除该帧     
        
	return(-1);
}


/********************************************************************************/
/*端机本级通讯                                                                  */
/*   正确确认  返回 0                                                           */
/*       其他  修改相应信息，返回 -1                                            */
/*   BuffFrmNo：待发送帧在帧缓冲中的编号                                     */
/********************************************************************************/
int TermSelfFreqCommunication(int BuffFrmNum)
{
	int returnval;	
	
	int BuffFrmNo = BuffFrmNum;
	
	g_uDLLCATime = DLL_TR_TIME;//设备通讯发生避碰后所需要的延迟时间，范围（DLL_TR_TIME~10*DLL_TR_TIME）
	
	while(1)
	{
		if(0 == SimiChDetect(g_uchRFSelfFreNum)) //调用本级信道竞争成功
		{
			RFSendPacket(g_uchWirelessTxBuf[BuffFrmNo],g_uchWirelessTxBuf[BuffFrmNo][0]);
			returnval = GetWirelessPacket(60);
                        
                  //射频芯片ready
                  SetRFReady();
			
			if((returnval == 0) && (g_uchWirelessRxBuf[11]==0x55) && (g_uchWirelessRxBuf[12]==0xa0))//是拒绝访问确认
		        {
                            g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //0从缓冲区中清除该帧
                            ReleaseRelation(g_uchWirelessRxBuf[3]+(unsigned int)(g_uchWirelessRxBuf[4]*256));//与发送拒绝帧的设备解除拓扑关系
                            
                            g_uDLLStaUp = 0;

                            return(-1); //返回传输信息失败 
		
		        }//end if 是拒绝访问确认
			
			//Tres时间内收到除了拒绝访问帧外的确认帧 并且目的地址给自己的
			if( (returnval == 0)  && (g_uchWirelessRxBuf[11] == 0x55) && (g_uchWirelessRxBuf[1] ==g_uchWirelessTxBuf[BuffFrmNo][3]) && (g_uchWirelessRxBuf[2] ==g_uchWirelessTxBuf[BuffFrmNo][4]))
			{			          			        						
                                    g_uchWirelessTxBufCtl[BuffFrmNo] = ( (g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ((g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));//帧传输尝试次数加1
                                    if( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F) > FRAME_MAX_SEND_TIMES) //帧尝试次数大于最大次
                                        g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//从缓冲区中清除该帧 						        
     
				    if(g_uchWirelessRxBuf[12] == g_uchWirelessTxBuf[BuffFrmNo][11]) //正确确认 
				    {	                                     
                                          g_uDLLStaUp = DLL_MAX_STAUP ; //上行链路系数赋值最大
							    										        
	                                  g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //0从缓冲区中清除该帧
	
					  g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
							
					  switch(g_uchWirelessRxBuf[12])//当前 端机主动通讯的 只有数据信息
					  {
						case 0x40: //数据帧确认
						{
							 if( (g_uchWirelessRxBuf[17] <=254) && (g_uchWirelessRxBuf[17]>0) )
                                                              g_uAppDataInt = g_uchWirelessRxBuf[17]*60; //以秒为单位,设定注册间隔时间
                                                          else
                                                              g_uAppDataInt = 21600;//六小时
							//重新计算当前设备链路质量
							LQEvaluation(g_uchWirelessRxBuf[14],g_uchRSSI,g_uchWirelessRxBuf[16],g_uDLLStaUp);
									
							//修改当前网络级别
							g_uchNetLev = g_uchWirelessRxBuf[15]+1;
									
							break;
						}
						default:
							break;
					  }//end switch

					  return 0;
							
				    }//end if 正确确认
				    else//出错  网络忙 等其他确认
				    {
                                        if( g_uDLLStaUp > DLL_STAUP_RIGHT )
                                            g_uDLLStaUp -= DLL_STAUP_RIGHT ; //减的权重加大 	
                                        else
                                            g_uDLLStaUp = 0;				    
                                        
					return 1;//暂时返回1
				    }                       
			}//end if Tres时间内收到除了拒绝访问帧外的确认帧 并且目的地址给自己的
			else  //Tres时间内没收到确认帧
			{
                                if( g_uDLLStaUp > DLL_STAUP_RIGHT )
                                        g_uDLLStaUp -= DLL_STAUP_RIGHT ; //数据链注册失败 减的权重加大 
                                else
                                        g_uDLLStaUp = 0;
					
				if( g_uDLLStaUp == 0) //上行稳定系数为0 
				{
					g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //缓冲区中清除该帧

                                        return(-1);
				} 				
			}
		}//end if 调用本级信道竞争成功	
		
		else  //调用本级信道竞争不成功
		{
                                if( g_uDLLStaUp > DLL_STAUP_RIGHT )
                                        g_uDLLStaUp -= DLL_STAUP_RIGHT ; // 减的权重加大 
                                else
                                        g_uDLLStaUp = 0;
					
				if( g_uDLLStaUp == 0) //上行稳定系数为0 
				{
					g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //缓冲区中清除该帧
                
                                        return(-1);
				} 

		}//end else 		  
		
		g_uchWirelessTxBufCtl[BuffFrmNo] = ((g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ((g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));  //帧传输尝试次数加1 ？？？？？？
		
		if( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F) > FRAME_MAX_SEND_TIMES) //帧尝试次数大于最大次
                {
                    g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//从缓冲区中清除该帧 	
                    
                    return(-1);
                }
		
		else //随机延时
		{	
                        Sleep_Mode3_ACLKms(g_uDLLCATime,RF_STANDBY);    
                        /*
                        //射频芯片ready
                        SetRFReady(); 
                        SetTimer0A0_ACLK();
                        Timer0A0_Delay(PHY_ACLK_1MS);	
                        while(g_uPHYTimer0A0Count<g_uDLLCATime); //延迟时间T=n*Tslice
                        StopTimer0A0();
                        */
 
			g_uDLLCATime = ( unsigned int ) ( (1.5*g_uDLLCATime) < (10*DLL_TR_TIME) ? (1.5*g_uDLLCATime):(10*DLL_TR_TIME)); //修改避碰延迟时间
			
			continue;
		}		
	}//end while(1)
}

/****************************************************************************/
/* 　　　　　　　　　　    寻上级自适应选频算法   　    　　        	    */
/*                  搜频成功      返回  0                                   */
/*                  搜频不成功    返回  -1                                   */
/* uchLimitGrade  为级别限制 ，若有级别限制则为实际级别限制值                */
/*                             若无级别限制则暂定为 0xff                     */
/****************************************************************************/
int SearchParentFreq(unsigned char LimitGrade)
{
	int returnVal;
	int PageNo;
	
	unsigned char uchLimitGrade = LimitGrade;
	unsigned char upNum = 0;   //存放查询到的上级设备数目
	unsigned char LQList;      //存放遍历过的最大的LQ值的下标
      
      #define SEARCH_NUM 5
	
	typedef struct{
		unsigned char tempLQ;     //链路质量
		unsigned int tempNetID;   //网络ID
		unsigned char tempFre;    //工作频点
		unsigned char tempNetLev; //网络级别
		unsigned char tempPAtype;//设备类型
	}tempupDev;  
	tempupDev upDev[SEARCH_NUM];  //保存查询成功后的上级设备信息
	
	unsigned char * c = (unsigned char *) &upDev; 
	
	for(int i = 0; i<sizeof(upDev) ; i++)
	  c[i] = 0; 
	
	GetRandomFreList(); //获取随机频点序列
	
	//对于A型不加功放的节点 先在自己的不加功放区域搜寻；找不到，再到加功放频点搜寻上级设备
	//对于B型加功放的节点 先在自己的加功放区域搜寻;  找不到,关闭功放，再到不加功放区域搜寻		
	g_uchRxPAtype = g_uchPHYPAType ;//开始和自己同类型的设备所在的频点通讯
	
	unsigned char EndFreNum=0;//结束搜频的频点下标
	unsigned char StartFreNum = 0; //开始搜频的频点下标
	if(g_uchPHYPAType == 0x0a)//若是不加功放节点
	{
	  EndFreNum = RF_FRE_A_AMOUNT;
	  StartFreNum =0;
	}
	else if(g_uchPHYPAType == 0x0b)//若是加功放节点
	{
	  EndFreNum = RF_FRE_B_AMOUNT+RF_FRE_A_AMOUNT;
	  StartFreNum = RF_FRE_A_AMOUNT;
	}
	  
	if(EndFreNum == StartFreNum)
	  return -1;
	
	for(; StartFreNum<EndFreNum; StartFreNum++) //在各自频点领域寻找， 同时功放根据频点所属的范围不同，打开或关闭
	{
	        if(g_chRFFreList[StartFreNum] == g_uchRFSelfFreNum) //上级频点和自己不能同频
	          continue;
	          
	        //设备通讯发生避碰后所需要的延迟时间，范围（DLL_TR_TIME~10*DLL_TR_TIME）
	        g_uDLLCATime = DLL_TR_TIME;
                
                //g_chRFFreList[StartFreNum]=1;
	        
		for(char i=0;i<5;i++)
		{
			PageNo = GenerateQueryFrame(INFO_SIMPLE_QUERY); //形成向上级设备发送的查询帧
			returnVal = DiffFreqCommunication(g_chRFFreList[StartFreNum],PageNo);
			if(returnVal == 0)
			  break;					
                        
                        Sleep_Mode3_ACLKms(g_uDLLCATime,RF_STANDBY);
                        /*
                        //射频芯片ready
                        SetRFReady();                          
                        SetTimer0A0_ACLK();
                        Timer0A0_Delay(PHY_ACLK_1MS);	
                        while(g_uPHYTimer0A0Count<g_uDLLCATime); //延迟时间g_uDLLCATime
                        StopTimer0A0(); 
                        */                        

			g_uDLLCATime = ( unsigned int ) ((1.5*g_uDLLCATime) < (10*DLL_TR_TIME) ? (1.5*g_uDLLCATime):(10*DLL_TR_TIME)); //修改避碰延迟时间
		}
                
		if( (returnVal == 0)  &&(g_uchWirelessRxBuf[12] == INFO_SIMPLE_QUERY))//收到简单查询确认回复，保存该上级设备信息与LQ值
		{ 
			if(g_uchWirelessRxBuf[13] > uchLimitGrade) //若搜到的上级设备网络级别值 大于设备当前的网络级别则不保存该设备，进入下轮搜索
				continue;
				
		        //if(( g_uAppDataType == 2)  &&//图像
		         //   (g_uchWirelessRxBuf[13] < 3) )//要挂在至少第3级网络上
			 //       continue;
                   //     if(g_uchWirelessRxBuf[13] < 2)//要挂在至少第2级网络上
			 //       continue;
		        
		        
		        if( g_uchWirelessRxBuf[19] >= NET_MAX_CAP) //待申请的设备太多 
		                continue;
			
			upDev[upNum].tempNetLev = g_uchWirelessRxBuf[13]; //保存上级设备回复的网络级别 
			upDev[upNum].tempNetID = g_uchWirelessRxBuf[7] + (unsigned int) (g_uchWirelessRxBuf[8]*256); //保存上级设备的网络ID
			upDev[upNum].tempFre = g_chRFFreList[StartFreNum];  //保存当前工作频点
			upDev[upNum].tempPAtype = g_uchWirelessRxBuf[14]; //上级设备功放类型
			
			LQEvaluation(g_uchWirelessRxBuf[18], g_uchRSSI, g_uchWirelessRxBuf[19], 10); //计算出上级设备到本设备的链路质量                    
			upDev[upNum].tempLQ = g_uchDLLCurLQ;    //保存上级设备到本设备的链路质量                  
			
			if(upDev[upNum].tempLQ >= g_uchDLLQ)       //比默认的LQ值好
			{
				returnVal = SubDevLog(g_chRFFreList[StartFreNum],upDev[upNum].tempNetID,upDev[upNum].tempNetLev,upDev[upNum].tempPAtype); //端机发送申请注册帧 
				if(returnVal == 0) //注册成功
				{
					
					g_uDLLStaUp = DLL_MAX_STAUP;// 上行稳定系数赋初值 
					g_uchDLLCurLQ = upDev[upNum].tempLQ;// 本级链路质量计算值
					g_uNetParentID = upDev[upNum].tempNetID; //保存父级网络ID
					g_uchNetParentFreqNo = upDev[upNum].tempFre;//父级频点
					g_uchNetLev = upDev[upNum].tempNetLev + 1;//本级级别 = 上级级别 + 1
					g_uNetParentPAtype = upDev[upNum].tempPAtype; //父亲设备功放类型
					
					g_uchDLLQ = upDev[upNum].tempLQ < DLL_LINK_QUAL ? upDev[upNum].tempLQ : DLL_LINK_QUAL;  //取两个之中较小者
					
					return(0x00);  //返回搜频成功
				}				
			}			
			upNum++;                      //保存的上级设备数量加1； 
                  if(upNum >= (SEARCH_NUM-1))
                  {
                      upNum = SEARCH_NUM-1;
                      break;
                  }
		}
		
	}//end for  在各自频点领域寻找， 同时功放根据频点所属的范围不同，打开或关闭
	
	if ( upNum == 0 )//在自身频点范围内找不到上级 尝试在其他频点找
	{
	    EndFreNum = 0;
	    StartFreNum = 0;
	    
	    if(g_uchPHYPAType == 0x0a)//若是不加功放节点 要在加功放的频点去寻找
            {
              EndFreNum = RF_FRE_B_AMOUNT+RF_FRE_A_AMOUNT;
              StartFreNum = RF_FRE_A_AMOUNT;
              g_uchRxPAtype = 0x0b ;//和自己 不同类型的设备所在的频点通讯
            }
            else if(g_uchPHYPAType == 0x0b)//若是加功放节点 在不加功放的频点去寻找
            {
              EndFreNum = RF_FRE_A_AMOUNT;
              StartFreNum =0;
              g_uchRxPAtype = 0x0a ;//和自己 不同类型的设备所在的频点通讯
            }
              
            if(EndFreNum == StartFreNum)
              return -1;
	  
	    for(; StartFreNum < EndFreNum ; StartFreNum++) //在自身频点范围内找不到上级 尝试在其他频点找
	    {
	        //设备通讯发生避碰后所需要的延迟时间，范围（DLL_TR_TIME~10*DLL_TR_TIME）
	        g_uDLLCATime = DLL_TR_TIME;
	        
		for(char i=0;i<5;i++)
		{
			PageNo = GenerateQueryFrame(INFO_SIMPLE_QUERY); //形成向上级设备发送的查询帧
			returnVal = DiffFreqCommunication(g_chRFFreList[StartFreNum],PageNo);
			if(returnVal == 0)
				break;					

                        Sleep_Mode3_ACLKms(g_uDLLCATime,RF_STANDBY); 
                        /*
                        //射频芯片ready
                        SetRFReady();
                        SetTimer0A0_ACLK();
                        Timer0A0_Delay(PHY_ACLK_1MS);	
                        while(g_uPHYTimer0A0Count<g_uDLLCATime); //延迟时间g_uDLLCATime
                        StopTimer0A0();
                        */
			
		}
                
		if( (returnVal == 0)  &&(g_uchWirelessRxBuf[12] == INFO_SIMPLE_QUERY))//收到简单查询确认回复，保存该上级设备信息与LQ值
		{ 
			if(g_uchWirelessRxBuf[13] > uchLimitGrade) //若搜到的上级设备网络级别值 大于设备当前的网络级别则不保存该设备，进入下轮搜索
				continue;
				
		        if( g_uchWirelessRxBuf[19] >= NET_MAX_CAP) //待申请的设备太多 
		                continue;
			
			upDev[upNum].tempNetLev = g_uchWirelessRxBuf[13]; //保存上级设备回复的网络级别 
			upDev[upNum].tempNetID = g_uchWirelessRxBuf[7] + (unsigned int) (g_uchWirelessRxBuf[8]*256); //保存上级设备的网络ID
			upDev[upNum].tempFre = g_chRFFreList[StartFreNum];  //保存当前工作频点
			upDev[upNum].tempPAtype = g_uchWirelessRxBuf[14]; //上级设备功放类型
			
			LQEvaluation(g_uchWirelessRxBuf[18], g_uchRSSI, g_uchWirelessRxBuf[19], 10); //计算出上级设备到本设备的链路质量                    
			upDev[upNum].tempLQ = g_uchDLLCurLQ;    //保存上级设备到本设备的链路质量                  
			
			if(upDev[upNum].tempLQ >= g_uchDLLQ)       //比默认的LQ值好
			{
				returnVal = SubDevLog(g_chRFFreList[StartFreNum],upDev[upNum].tempNetID,upDev[upNum].tempNetLev,upDev[upNum].tempPAtype); //端机发送申请注册帧 
				if(returnVal == 0) //注册成功
				{
					
					g_uDLLStaUp = DLL_MAX_STAUP;// 上行稳定系数赋初值 
					g_uchDLLCurLQ = upDev[upNum].tempLQ;// 本级链路质量计算值
					g_uNetParentID = upDev[upNum].tempNetID; //保存父级网络ID
					g_uchNetParentFreqNo = upDev[upNum].tempFre;//父级频点
					g_uchNetLev = upDev[upNum].tempNetLev + 1;//本级级别 = 上级级别 + 1
					g_uNetParentPAtype = upDev[upNum].tempPAtype; //父亲设备功放类型
					
					g_uchDLLQ = upDev[upNum].tempLQ < DLL_LINK_QUAL ? upDev[upNum].tempLQ : DLL_LINK_QUAL;  //取两个之中较小者
					
					return(0x00);  //返回搜频成功
				}				
			}			
			upNum++;                      //保存的上级设备数量加1； 
                  if(upNum >= (SEARCH_NUM-1))
                  {
                      upNum = SEARCH_NUM-1;
                      break;
                  }
		}
		
	    }//end for  在自身频点范围内找不到上级 尝试在其他频点找  	    
	
	}//end if  在自身频点范围内找不到上级 尝试在其他频点找
	
	if(upNum != 0) //保存的上级设备的LQ值都没有默认的LQ值好，则需要从保存的较好lq值中选一个注册
	{
		unsigned char tempupNum = upNum;
		
		while(1)//循环得到可以注册的基站
		{
			unsigned char tempMaxLQ = 0;
			LQList = 0;  //LQ数组中的下标
			for(int count=0; count<upNum; count++) //选取LQ值最好的，并保存其数组下标
			{
				if(upDev[count].tempLQ > tempMaxLQ)
				{
					tempMaxLQ = upDev[count].tempLQ;
					LQList = count;
				}
			}
						
			//如要保存已选定的上级设备信息，请在这里保存
			returnVal = SubDevLog(upDev[LQList].tempFre,upDev[LQList].tempNetID,upDev[LQList].tempNetLev,upDev[LQList].tempPAtype);
			
			if(returnVal == 0) //注册成功
			{
				g_uDLLStaUp = DLL_MAX_STAUP;  // 上行稳定系数赋初值      本级链路质量计算值               
				g_uchDLLCurLQ = upDev[LQList].tempLQ;//  
				g_uNetParentID = upDev[LQList].tempNetID; //保存父级网络ID
				g_uchNetParentFreqNo = upDev[LQList].tempFre;//父级频点
				g_uchDLLQ = upDev[LQList].tempLQ;  
				g_uchNetLev = upDev[LQList].tempNetLev + 1;//本级级别 = 上级级别 + 1
				g_uNetParentPAtype = upDev[LQList].tempPAtype; //父亲设备功放类型
				
				return(0);
			}
			else
			{
				upDev[LQList].tempLQ = 0x00;    //删除掉注册未成功的上级设备信息
				tempupNum--;
			}
			if(tempupNum == 0)//上级设备信息为空
				return(-1);
		} // end while 循环得到可以注册的基站   
	}
	else 
		return(-1);//上级基站数目为空，返回失败
	
}   

/****************************************************************************/
/* 　　　　　　　　 确定本级工作频点的自适应选频程序   　            	    */
/*                  搜频成功      返回  0                                   */
/*                  搜频不成功    返回  -1                                  */
/****************************************************************************/
int SearchSelfFreq()
{    
	int trytimes = 8;
	int listentimes = 0;
	int returnVal;
	int PageNo;
	unsigned char overRSSI = 0x00;        //检测到的无线通讯载波RSSI值小于额定值的次数
	
	unsigned char Tslice = DLL_SLICE_TIME;
	
	 // 加功放设备只能在加功放频点搜寻自己的频点
	 //不加功放设备只能在无功放频点搜寻自己的频点
	 // 所以搜寻时，接受设备的类型和自己类型一致
	g_uchRxPAtype = g_uchPHYPAType ; //设定接收设备的类型
	
	GetRandomFreList(); //获取随机频点序列
	
	unsigned char EndFreNum=0;//结束搜频的频点下标
	unsigned char StartFreNum = 0; //开始搜频的频点下标
	if(g_uchPHYPAType == 0x0a)//若是不加功放节点
	{
	  EndFreNum = RF_FRE_A_AMOUNT;
	  StartFreNum =0;
	}
	else if(g_uchPHYPAType == 0x0b)//若是加功放节点
	{
	  EndFreNum = RF_FRE_B_AMOUNT+RF_FRE_A_AMOUNT;
	  StartFreNum = RF_FRE_A_AMOUNT;
	}
	  
	if(EndFreNum == StartFreNum)
	  return -1;
	  
	for(; StartFreNum < EndFreNum; StartFreNum++)//频道遍历
	{     
	        
	        if( g_chRFFreList[StartFreNum] == g_uchNetParentFreqNo)
	          continue;
	          
	        trytimes = 8;
	        
		while(--trytimes >0) //发送8次查询帧
		{
			returnVal = SimiChDetect(g_chRFFreList[StartFreNum]);//调用本级信道竞争机制算法
			
                        if(returnVal != 0) //调用失败 
				break;
			else //调用成功
			{		      
				overRSSI = 0;
				PageNo = GenerateQueryFrame(INFO_SIMPLE_QUERY);
				RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);//发送查询帧
								
				for(listentimes = 0; listentimes < 3; listentimes++)
				{
                                        Sleep_Mode3_ACLKms(6*Tslice,RF_STANDBY);
                                        /*
                                        //射频芯片ready
                                        SetRFReady();                                        
                                        SetTimer0A0_ACLK();
                                        Timer0A0_Delay(PHY_ACLK_1MS);	
                                        while(g_uPHYTimer0A0Count<6*Tslice); //延迟时间T=6*Tslice
                                        StopTimer0A0();
                                        */
                        
                                        SetRFRx();//转入接收态
					 
					int RSSI_dbm = GetRSSIPower();
					if(RSSI_dbm >= RF_LISTEN_RSSI) //若没有回复，则监听到载波信号值不大于限值
					{
						overRSSI = 1;
						break;
					}				
				}
				if(overRSSI > 0)//已经收到干扰
				{
					break;
				}

			}//end else 调用成功
			
			Sleep_Mode3_ACLKms(5,RF_STANDBY);
                        /*
                        //射频芯片ready
                        SetRFReady();  
                        SetTimer0A0_ACLK();
                        Timer0A0_Delay(PHY_ACLK_1MS);	
                        while(g_uPHYTimer0A0Count<5);
                        StopTimer0A0();
                         */
                        
		}//end while 发送8次查询帧
                
                //射频芯片ready
                //SetRFReady();
		
		if(returnVal != 0) //信道竞争调用失败 换到别的频道
			continue;
		
		if(overRSSI == 0) //信道为空
		{
			g_uchRFSelfFreNum = g_chRFFreList[StartFreNum];//设定为当前可用频点
			
			g_uDLLStaDown = 10; //下行稳定系数 
                        
                        //g_uchRFSelfFreNum=2;//测试用 

                        return(0);//返回频点占用成功
		}
		
	}//end for 频道遍历
        
	return(-1);//频点遍历完，返回频点占用失败
}

/*****************************************************************************/
/*  子设备发送注册申请 ：                                                    */
/*       包括如下步骤：                                         	     */
/*             1，发送申请注册帧，等待上级基站回复等待认证帧                 */
/*             2，等待一定时间之后，再次发送申请注册帧，如收到转发过来的     */
/*                中心基站控制器的认证，则说明搜频成功                       */
/*                等待时间待进一步确定？？？？                               */
/*  通过认证0x32：返回0                                                      */
/*  认证失败0x33：返回-2                                                     */
/*  其他        ：返回-1                                                     */
/*****************************************************************************/
int SubDevLog(unsigned char uchFreNum,unsigned int tempNetID,int NetLev, unsigned char destPAtype)
{
	int returnVal;
	int PageNo;
	unsigned int WaitTime;//单位秒
	unsigned int MaxWaitTime = 60;//最大等待时间60秒
	unsigned int WaitTimeSlice = 10;//等待基本时间单元 10秒 
	int trytimes = 3;//发送申请注册的次数
	g_uDLLCATime = DLL_TR_TIME;
	
	unsigned char uchFreNo = uchFreNum; //频点编号
	int netlev = NetLev;//网络等级 
	
	g_uchRxPAtype = destPAtype; //接收设备的功放类型
	
	unsigned char FirstDestID[2];
	
	FirstDestID[0] = tempNetID&0xff;
	FirstDestID[1] = (tempNetID/256)&0xff;
	
	for(char count=0; count<10; count++)  //尝试10次抢占信道
	{
		if(0 == DiffChDetect(uchFreNo))// 频点探测
		{
			PageNo = GenerateApplyLogFrame(FirstDestID);
			RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
			returnVal = GetWirelessPacket(60);
                        
                        //射频芯片ready
                        //SetRFReady();
			
			//收到回复 目的地址是给自己的
			if( (returnVal == 0) && (g_uchWirelessRxBuf[1] ==g_uchWirelessTxBuf[PageNo][3]) && (g_uchWirelessRxBuf[2] ==g_uchWirelessTxBuf[PageNo][4]))
			{	
				if((g_uchWirelessRxBuf[11] == 0x50) && (g_uchWirelessRxBuf[12] == 0x32)) //通过认证 ，中心基站直接回复成功或失败，不做等待认证回复
				{
					 //添加注册间隔时间判断
                                        if ( ( g_uchWirelessRxBuf[23] <=254)  && (g_uchWirelessRxBuf[23] >0) )
                                            g_uAppDataInt=g_uchWirelessRxBuf[23]*60; //配置数据上报间隔时间
                                        else
                                            g_uAppDataInt= 21600;//六小时					

					 //添加注册间隔时间判断
                                        if ( ( g_uchWirelessRxBuf[24] <= 60)  && (g_uchWirelessRxBuf[24] >0) )
                                            g_uDLLLogInt=g_uchWirelessRxBuf[24]*60; //配置链路层注册间隔 
                                        else
                                            g_uDLLLogInt= 3600;//60分钟		
                                            		
					g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
					return(0);
				}
				
				if(g_uchWirelessRxBuf[11] == 0x55) //收到确认回复
				{				
				        if(g_uchWirelessRxBuf[12] == 0x32) //收到通过认证回复
					{
                                                 //添加注册间隔时间判断
                                                if ( ( g_uchWirelessRxBuf[24] <=254)  && (g_uchWirelessRxBuf[24] >0) )
                                                    g_uAppDataInt=g_uchWirelessRxBuf[24]*60; //配置数据上报间隔时间
                                                else
                                                    g_uAppDataInt= 21600;//六小时                                               
        
                                                 //添加注册间隔时间判断
                                                if ( ( g_uchWirelessRxBuf[25] <= 60)  && (g_uchWirelessRxBuf[25] >0) )
                                                    g_uDLLLogInt=g_uchWirelessRxBuf[25]*60; //配置链路层注册间隔 
                                                else
                                                    g_uDLLLogInt= 3600;//60分钟				
						
						g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;										
						return(0);
					}
					
					if((g_uchWirelessRxBuf[12] == 0xa0)||(g_uchWirelessRxBuf[12] == 0xa2)||(g_uchWirelessRxBuf[12] == 0x33))//拒绝访问 或 网络忙 或者注册失败
					{
						return(-1);
					}
			
					if(g_uchWirelessRxBuf[12] == 0x31) //等待认证确认帧
					{
						g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
						
						WaitTime = 10; //第一次等待10秒
						
						//针对加速度传感器特殊设置的 
						if(g_uAppDataType == 0x03)
						{
                                                    WaitTime = 2; //第一次等待3秒
                                                    MaxWaitTime = 5;//最大等待时间5秒
                                                    WaitTimeSlice = 1;//等待基本时间单元 1秒 
	                                        }
						
						while( trytimes-- > 0 )
						{	
                                                        Sleep_Mode3_ACLKs(WaitTime,RF_STANDBY); 
                                                        /*
                                                        //射频芯片ready
                                                        SetRFReady();                                                        
                                                        SetTimer0A0_ACLK();
                                                        Timer0A0_Delay(PHY_ACLK);	
                                                        while(g_uPHYTimer0A0Count<WaitTime); //延迟时间WaitTime
                                                        StopTimer0A0(); 
                                                        */

							WaitTime = (WaitTimeSlice*netlev) < MaxWaitTime ? (WaitTimeSlice*netlev) : MaxWaitTime;//等待认证的时间 单位 秒
							
							g_uDLLCATime = DLL_TR_TIME;
							 
							for(char count=0; count<5; count++)  //尝试5次抢占信道
                                                        {                                                                                                                 
                                                            if(0 == DiffChDetect(uchFreNo)) //频点探测
                                                            {
                                                                    PageNo = GenerateApplyLogFrame(FirstDestID);
                                                                    RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
                                                                    returnVal = GetWirelessPacket(60);
                                                                    
                                                                    //射频芯片ready
                                                                    //SetRFReady();
                                                                    
                                                                    if(returnVal == 0)
                                                                    {								        
                                                                            if(g_uchWirelessRxBuf[12] == 0x32) //收到通过认证回复
                                                                            {       
                                                                                  if(g_uchWirelessRxBuf[11] == 0x50)
                                                                                  {
                                                                                       //添加注册间隔时间判断
                                                                                        if (( g_uchWirelessRxBuf[23] <= 254)  && (g_uchWirelessRxBuf[23] >0) )
                                                                                            g_uAppDataInt=g_uchWirelessRxBuf[23]*60; //配置数据上报间隔时间
                                                                                        else
                                                                                            g_uAppDataInt= 21600;//六小时                                                                                        
                                                
                                                                                         //添加注册间隔时间判断
                                                                                        if (( g_uchWirelessRxBuf[24] < 60)  && (g_uchWirelessRxBuf[24] >0) )
                                                                                            g_uDLLLogInt=g_uchWirelessRxBuf[24]*60; //配置链路层注册间隔 
                                                                                        else
                                                                                            g_uDLLLogInt= 180;//3分钟		
                                                                                  }
                                                                                  if(g_uchWirelessRxBuf[11] == 0x55)
                                                                                  {
                                                                                       //添加注册间隔时间判断
                                                                                        if (( g_uchWirelessRxBuf[24] <=254)  && (g_uchWirelessRxBuf[24] >0) )
                                                                                            g_uAppDataInt=g_uchWirelessRxBuf[24]*60; //配置数据上报间隔时间
                                                                                        else
                                                                                            g_uAppDataInt= 21600;//六小时
                                                                                            
                                                                                         //添加注册间隔时间判断
                                                                                        if (( g_uchWirelessRxBuf[25] < 60)  && (g_uchWirelessRxBuf[25] >0) )
                                                                                            g_uDLLLogInt=g_uchWirelessRxBuf[25]*60; //配置链路层注册间隔 
                                                                                        else
                                                                                            g_uDLLLogInt= 180;//3分钟
                                                                                  }
                                                                                      
                                                                                   g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
                                                                                    
                                                                                    return(0);
                                                                            }
                                                                            
                                                                            if(g_uchWirelessRxBuf[12] == 0x31) //等待认证确认帧
                                                                            {
                                                                                g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
                                                                                break;
                                                                            }
                                                                            
                                                                            if(g_uchWirelessRxBuf[12] == 0x33) //收到认证失败回复
                                                                                    return(-2);
                                                                                    
                                                                             if((g_uchWirelessRxBuf[12] == 0xa0)||(g_uchWirelessRxBuf[12] == 0xa2))//拒绝访问 或 网络忙
                                                                            {
                                                                                    return(-1);
                                                                            }
                                                                    }//end if returnVal == 0                                                                    
                                                            }//end if 频点探测

                                                            Sleep_Mode3_ACLKms(g_uDLLCATime,RF_STANDBY); 
                                                            /*
                                                              //射频芯片ready
                                                              SetRFReady();
                                                              SetTimer0A0_ACLK();
                                                              Timer0A0_Delay(PHY_ACLK_1MS);	
                                                              while(g_uPHYTimer0A0Count<g_uDLLCATime); //延迟时间g_uDLLCATime
                                                              StopTimer0A0(); 
                                                            */
                                                                
                                                            g_uDLLCATime = ( unsigned int )( (1.5*g_uDLLCATime) < (10*DLL_TR_TIME) ? (1.5*g_uDLLCATime):(10*DLL_TR_TIME)); //修改避碰延迟时间
                                                                                                                        
                                                         }//end for 尝试5次抢占信道
							
						}//end while trytimes
                                                
                                                //射频芯片ready
                                                //SetRFReady();
						
						return -1; //没有注册上 
					}//end if 等待认证确认帧
					
				}//end if 收到确认回复
			}//end if 收到回复
		}// end if 频点探测

                Sleep_Mode3_ACLKms(g_uDLLCATime,RF_STANDBY);
                /*
                //射频芯片ready
                SetRFReady();
                SetTimer0A0_ACLK();
                Timer0A0_Delay(PHY_ACLK_1MS);	
                while(g_uPHYTimer0A0Count<g_uDLLCATime); //延迟时间g_uDLLCATime
                StopTimer0A0(); 
                    */            

                g_uDLLCATime = ( unsigned int ) ((1.5*g_uDLLCATime) < (10*DLL_TR_TIME) ? (1.5*g_uDLLCATime):(10*DLL_TR_TIME)); //修改避碰延迟时间

		
	}//end for 循环发送 
        
        //射频芯片ready
        //SetRFReady();
	return(-1);
}

/*****************************************************************************/
/*       设备链路质量评估 ,计算出本级链路质量                               */
/* 函数参数：reflq----参考链路质量值，即上级网络链路质量                    
rssi-----通讯的rssi检测值，非功率值                               
netcap---上级设备当前注册的设备数量 
stab－－－上行网络稳定值                                        */
/*****************************************************************************/
void LQEvaluation(unsigned char reflq, unsigned char RSSI, unsigned char netcap, unsigned char stab)  
{
	unsigned char PQ;
	unsigned char SQ;
	unsigned char CQ;
	
        int RSSI_dbm = (RSSI/2) - 123;
        
	if(RSSI_dbm >= RF_RSSI_GOOD)  // RF_RSSI_GOOD = -90
		PQ = 100;
	else if(RSSI_dbm >= RF_RSSI_NORMAL)  //RF_RSSI_NORMAL = -95
		PQ = 90;
	else 
		PQ = 50;
	
	if(netcap <= NET_MAX_CAP/4)  //设备当前接入的直接下级设备数量  与   最多可接入的下级设备容量相比
		CQ = 100;
	else if((netcap > NET_MAX_CAP/4) && (netcap < NET_MAX_CAP/2))
		CQ = 95;
	else if((netcap >= NET_MAX_CAP/2) && (netcap < (3*NET_MAX_CAP)/4))
		CQ = 90;
	else if((netcap >= (3*NET_MAX_CAP)/4) && (netcap < NET_MAX_CAP)) 
		CQ = 80;
	else
		CQ = 10;
	
	if(stab >= 10)   //以 本级通讯时的网络稳定系数 即 网络下行稳定量化值  为参数
		SQ = 100;
	else if(stab >= 6 && stab <10)
		SQ = 80;
	else
		SQ = 20;
		
	//g_uchDLLCurLQ = (unsigned char)((float)(PQ+SQ+CQ)/300 * (reflq-1));    
      g_uchDLLCurLQ = (unsigned char)((float)(PQ+SQ+CQ)/300 * (reflq-5));
}

