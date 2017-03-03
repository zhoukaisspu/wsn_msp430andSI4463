/*****************************************************************
��·�������ŵ�ʹ�����⣬���ռ��һ���ŵ�����������
*****************************************************************/
#include "DLLlib.h"

unsigned int g_uDLLLogInt = 300; //����������·ע����ʱ�䣺 ����Ϊ��λ

//��·��������
unsigned char g_uchDLLQ = DLL_LINK_QUAL;              //������·��ȫ�ֱ������趨Ĭ�ϵ�����ͨѶ�����Ľ���ֵ
unsigned char g_uchDLLCurLQ=0;                        //����������·����
unsigned char g_uchDLLSupLQ=0;                        //�ϼ�������·����
unsigned int g_uDLLStaUp=0;                           //������·��ȫ�ֱ��������������ȶ�����ֵ
unsigned int g_uDLLStaDown=0;                         //������·��ȫ�ֱ��������������ȶ�����ֵ
unsigned int g_uDLLQ = DLL_LINK_QUAL;                 //������·��ȫ�ֱ������趨Ĭ�ϵ�����ͨѶ�����Ľ���ֵ

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


//======const:�����ֵ�����й���֮�в������ı䣬������ָı��״�������ᱨ��
const unsigned char g_cuchDLLUartSyncData[3] = {0xff,0xaa,0x55};  //Uartͬ�������ж���
unsigned char g_uchDLLUartSync = 0;                 //Uart֡ͬ������ֵ


unsigned char g_uchWirelessTxBuf[BUFF_MAXPAGE][PAGESIZE] ={0};            //���ͻ�����

//֡���������
//Bitλ	7	:           1��֡��Ч         0��֡��Ч;
//Bitλ	6       :           1����Ҫ�ܻظ�     0������Ҫ�ܻظ� 
//Bitλ	5       : ��ʱ����������         	
//Bitλ	4       :֡����     1����ʼ֡         0�����ֿ���м�֡
//Bitλ	3~0     : ֡���Է��͵Ĵ��������Ϊ10������10�λ�û���ͳɹ���������֡��Ч
unsigned char g_uchWirelessTxBufCtl[BUFF_MAXPAGE] = {0};                 //֡���������

unsigned char g_uchWirelessRxBuf[256] = {0};                  //����֡����

int g_iWirelessRxFlag = 1;//���߽������ݱ��

unsigned char g_uchDLLFrameNum = 1;                   //��ǰ��Ϣ֡���

//�����ӳ�ʱ�䶨��
unsigned int g_uDLLCATime = DLL_TR_TIME;            //�豸ͨѶ��������������Ҫ���ӳ�ʱ�䣬��Χ��DLL_TR_TIME~10*DLL_TR_TIME��



/***********************************************/
/*      CRC16У������                        */
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



/*********************** hamming code [7,4,3]����*******************************/
const unsigned char _4bit_ham_table[16]={0x00,0x87,0x99,0x1e,0xaa,0x2d,0x33,
0xb4,0x4b,0xcc,0xd2,0x55,0xe1,0x66,0x78,0xff};

/****************************************************************************/
/*   ͨ��UART���� ����c                                                   */
/*   ����   :     unsigned char com ѡ�������ݵĴ��ڣ���ѡֵ�У�
                                     UARTA0��UARTA1��UARTA3               */
/*                unsigned char c , Ϊ����������                          */
/*   ��������ֵ��int �ͣ�0�����ͳɹ���1����ѡ�Ĵ���δ�򿪣�2���޴˴���    */
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
  default:          //====û���ҵ���ѡ��Ĵ��ڣ�����û�д˴��ڲ���
    return 2;
  }
}
/****************************************************************************/
/*   Uart����ϵͳͬ����                                                   */
/*   ����   :     unsigned char com ѡ�������ݵĴ��ڣ���ѡֵ�У�
                                     UARTA0��UARTA1��UARTA3               */
/*   ��������ֵ��int �ͣ�0�����ͳɹ���1����ѡ�Ĵ���δ�򿨣�2���޴˴���    */
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
/*     ��Uart�Ľ��ջ������л�ȡһ�����ݸ� ����C,�ɹ�����0������-1           */
/*   ����   :     unsigned char com ѡ�������ݵĴ��ڣ���ѡֵ�У�
                                     UARTA0��UARTA1��UARTA3                 */
/*������⣺g_uchDLLBuffer0Head������д���һ�������ͷ
          g_uchDLLBuffer0Tail��������д���һ�������β
          �������ͷ�������β��ͬ��ʱ�򣬴�����д��������Ѿ���ȡ���
         ��ʼ״���£�g_uchDLLBuffer0Head=0��g_uchDLLBuffer0Tail=0��*/
/****************************************************************************/
int GetUartData(unsigned char com ,unsigned char* c)        //====��ȡ��������
{
	switch(com)
        {
          case UARTA0:
          {
            if(g_uchDLLBuffer0Head == g_uchDLLBuffer0Tail)       //====�����ͷ��β����ͬһ���ط���û���յ����ݣ��������յ�������̫�󣬵���ͷβ����
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
/*     Uart�Ľ��ջ��������                                               */
/*   ����   :     unsigned char com ѡ�������ݵĴ��ڣ���ѡֵ�У�
                                     UARTA0��UARTA1��UARTA3                 */
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
/*  ����unsigned int overtime�趨�˽����������ݰ���ʱ������                 */
/*  ��������ֵ ��0: ���ճɹ������ݱ�����ȫ�ֱ���g_uchWirelessRxBuf
                -1�����̵�ַ��ƥ��;
                -2: û���յ��κ�����                         
                -3:�յ����ݵ���CRCУ�����                                  
                -4:�յ�ͬ���ֺͲ�������,�����ݽ��ճ�ʱʧ�ܣ�            
*/
 //����ֵ       1���ڹ涨��ʱ����û���յ��κ�����               
/*overtime��ȫ�ֱ���g_uPHYTimer0A0Count���бȽϣ��޶��ڶ���ʱ���ڽ����������ݰ�*/
/****************************************************************************/ 
int GetWirelessPacket(unsigned int overtime)
{
        
        int ret;//����ֵ
        
        unsigned char maxRSSI = 0;
        unsigned char RSSI = 0;        
        
        SetRFRx();                   //====������ִ�������FIFO��ָ��
        
       // CLR_NIRQ_IFG;//����жϱ��
        EINT_NIRQ;

	SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK_1MS);                       //��Ѱ60ms
        
	while((g_iWirelessRxFlag == 1)&&(g_uPHYTimer0A0Count<overtime))   //�޶�ʱ���ڵȴ���������
	{
          //RSSI = GetRSSI();    //��ȡ����RSSI
         // RSSI=GetFastLatchRSSI();
          if(RSSI >maxRSSI)
            maxRSSI = RSSI;        
        }
        StopTimer0A0();
        
	DINT_NIRQ;
        
        g_uchRSSI = maxRSSI;
        
	ret =  g_iWirelessRxFlag;    //�Ƚ�����ֵ���ص�Ȼ���ڽ���־λ�ı�ΪĬ��ֵ��		
	g_iWirelessRxFlag = 1;
	
	//if(ret == 0)//��CRCУ��
	//{
	//  if( GenerateCRC(g_uchWirelessRxBuf,g_uchWirelessRxBuf[0])!=0 ) //CRCУ�����
	//    return -3;
	//}
	return ret;
}

/****************************************************************************/
/*     ����ͨѶ���ջ��������                                               */
/****************************************************************************/
void WirelessRxBufferClear(void)
{
  RFResetRxFIFO();
}

/*****************************************************************************/
/* ����CRCУ����ĺ���������DataBuff��ҪУ������ݣ�������Ҫ����У��������� */
/* ע�⣺DataBuffĬ��������ֽڴ��CRCУ���룬len�����У������DataBuff��С*/
/* ��������ֵ��unsigned int��CRC������                                       */
/*****************************************************************************/
unsigned int GenerateCRC(unsigned char *DataBuff,unsigned int len)
{
	unsigned int oldcrc16;
	unsigned int crc16;
	unsigned int oldcrc;
	unsigned int charcnt;
	unsigned char c,t;
	oldcrc16 = 0x0000; //��ֵΪ0
	charcnt=0;
	
	while (len--) {
		t= (oldcrc16 >> 8) & 0xFF; //Ҫ�Ƴ����ֽڵ�ֵ
		oldcrc=g_cuDLLCRC16_table[t]; //�����Ƴ����ֽڵ�ֵ���
		c=DataBuff[charcnt]; //���ƽ������ֽ�ֵ
		oldcrc16= ((oldcrc16 << 8)) | c; //�����ƽ������ֽ�ֵ���ڼĴ���ĩ�ֽ���
		oldcrc16=oldcrc16^oldcrc; //���Ĵ���������ֵ����xor ����
		charcnt++;
	}
	crc16=oldcrc16;
	return crc16;
}

/*****************************************************************************/
/* ����֡��CRC�ֽ�                                                           */
/*  TxDataΪҪ���CRCУ�������֡������TxData[0]��ʾ����������֡�ĳ���       */
/*  TxData���������ַ���CRCУ����ֵ                                      */
/*****************************************************************************/
void AddCRC(unsigned char *TxData)
{
	unsigned int CRC;
	TxData[TxData[0]-2] = TxData[TxData[0]-1] = 0x00;
	CRC = GenerateCRC(TxData,TxData[0]); //CRC����ֵ
	TxData[TxData[0]-2] = ((CRC>>8)&0x0ff);  
	TxData[TxData[0]-1] = (CRC&0x0ff);	
}

/****************************************************************************/
/*��ȡUartͬ����ͷ, ͬ������ȫ�ֱ������� g_uchDLLWireSyncData����         */
/*        ��������ֵ: int ���ͣ�  -1----ͬ��ʧ�� ,  0----ͬ���ɹ�           */
/*   ����   :     unsigned char com ѡ�������ݵĴ��ڣ���ѡֵ�У�
                                     UARTA0��UARTA1��UARTA3                 */
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
				if(c == g_cuchDLLUartSyncData[0])     //=====��ȡ����ͬ���ֵĵ�һ���ֽ�
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
					g_uchDLLUartSync = 0;        //===�յ����ֽڴ���
				break;
			}         
			
		case 2:
			{
				g_uchDLLUartSync = 0;			        
				if(c == g_cuchDLLUartSyncData[2])
					return 0;                        //=====����ֵΪ0��ʱ�򣬴���ͬ������ȡ���
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
/* ���޶�ʱ���ڽ���Uart��Ϣ֡��
   ����   :     unsigned char com ѡ�������ݵĴ��ڣ���ѡֵ�У�
                                     UARTA0��UARTA1��UARTA3                  */
/*                 RxData����յ�������,RxData[0]Ϊ�յ����ݳ���              */
/*                           TimeLimit����޶�ʱ�䣬��λ����                 */
/* ��������ֵ�� 0 �����յ�����֡�����洢��RxData                             */
/*             -1������ʱδ�յ���Ч����                                      */
/*****************************************************************************/
int UartRxFrameData(unsigned char com,unsigned char *RxData, unsigned int TimeLimit)
{	
	unsigned int limit = TimeLimit;//�޶�ʱ��
	unsigned char c,i;
	
        SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK_1MS);	
	while((0 != GetUartSyncWord(com))&&(g_uPHYTimer0A0Count<limit)); //�޶�ʱ���ڵȴ����ݣ����յ�������ͬ���ֻ��߳�����ʱʱ��ͻ�����whileѭ��
	StopTimer0A0();        

	if(g_uPHYTimer0A0Count >= limit)   //����Ƕ�ʱ����������ڹ̶�ʱ����û�л�ȡ������ͬ���֣����ݻ�ȡ����
	{
	  //g_uchDLLWireFlag = 0;
	  return -1;//��ʱ
	}	
      
        SetTimer0A0_ACLK();                              //====��2����֮�ڻ�ȡһ���ֽڵ����ݣ���ֻȡ��һ���ֽڵ����ݣ���ȡһ���ֽڵ�����֮������whileѭ��
	Timer0A0_Delay(PHY_ACLK_1MS);                                       //====��ȡ�������ݣ���ȡ��һ���ֽڵ����ݣ����ֽڵ�֡��
	while((0 != GetUartData(com,&c))&&(g_uPHYTimer0A0Count<2));         //2������ȡ��һ���ֽ�����
	StopTimer0A0();             //====ȥ��һ���ֽڵ����ݣ�����ֵΪ0

	if(g_uPHYTimer0A0Count >= 2)       //====ȷ��������Ϊ��ʱ�����������������whileѭ��
	{
	  //g_uchDLLWireFlag = 0;
	  return -1;//��ʱ
	}
	
	RxData[0] = c; //�õ�֡��
	
	i = 1;
	
	while(i<RxData[0])
	{
		
          SetTimer0A0_ACLK();
          Timer0A0_Delay(PHY_ACLK_1MS);	
          while((0 != GetUartData(com,&c))&&(g_uPHYTimer0A0Count<2)); //2������ȡ��һ���ֽ�����
          StopTimer0A0();        
  
          if(g_uPHYTimer0A0Count >= 2)
            return -1;//��ʱ
	
	  RxData[i++] = c; //��������
	}
	
	//g_uchDLLWireFlag = 0;
	return 0;	
}

/*****************************************************************************/
/* Uart��������֡
   ����   :     unsigned char com ѡ�������ݵĴ��ڣ���ѡֵ�У�
                                     UARTA0��UARTA1��UARTA3                  */
/*                 TxData��ż������͵����ݣ�TxData[0]��ŷ������ݳ���     */
/*   ��������ֵ��int �ͣ�0�����ͳɹ���1����ѡ�Ĵ���δ�򿨣�2���޴˴���    */
/*****************************************************************************/
int UartTxFrameData(unsigned char com,unsigned char *TxData)
{
	unsigned char count= TxData[0];
	int ret = UartSendSync(com);
        
        if(ret != 0)
          return ret;
        
	for(unsigned char i=0; i<count; i++)
	{
		ret = UartSend(com,TxData[i]);   //������Ч����
                if(ret != 0)
                  return ret;
	} 
        return 0;
}

/*****************************************************************************************/
/*��ȡ����ĳ�ʼƵ������                                                                 */
/*���ɵ�Ƶ�����з��룺A����uchFreNumA[RF_FRE_AMNUM_A]                                    */
/*                   ������0~(RF_FRE_AMNUM_A-1)                                          */
/*                    B����uchFreNumB[RF_FRE_AMNUM_B]                                    */
/*                    ����:RF_FRE_AMNUM_A ~ (RF_FRE_AMNUM_A+RF_FRE_AMNUM_B-1)            */
/*���豸MAC��ַ+�ź�ǿ��Ϊ���������                                                     */
/*****************************************************************************************/
void GetRandomFreList()
{
	unsigned char i,j,temp;
	unsigned char rssi = 0; //��ȡ����ź�ǿ��
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
	
	for(i=0; i<RF_FRE_A_AMOUNT; i++)//���ӹ���Ƶ�� 0~RF_FRE_A_AMOUNT-1
	{
		temp = g_chRFFreList[i];
		j = rand()%(RF_FRE_A_AMOUNT-i) + i; //��֤��i��RF_FRE_A_AMOUNT-1֮��ѡȡ
		g_chRFFreList[i] = g_chRFFreList[j];
		g_chRFFreList[j] = temp;
	}
	
	for(i=RF_FRE_A_AMOUNT; i<(RF_FRE_A_AMOUNT+RF_FRE_B_AMOUNT); i++)//�ӹ���Ƶ�� RF_FRE_AMNUM_A ~ (RF_FRE_AMNUM_A+RF_FRE_AMNUM_B-1)
	{
		temp = g_chRFFreList[i];
		j = rand()%(RF_FRE_A_AMOUNT+RF_FRE_B_AMOUNT-i) + i; //��֤��i�� RF_FRE_AMNUM_A ~ (RF_FRE_AMNUM_A+RF_FRE_AMNUM_B-1)֮��ѡȡ
		g_chRFFreList[i] = g_chRFFreList[j];
		g_chRFFreList[j] = temp;
	}
}

/*****************************************************************************/
/* �켶�ŵ����������㷨 �������βΣ���ǰ�������켶Ƶ�㣬�������ϼ�Ƶ��       */
/*                                                     Ҳ�������¼�Ƶ��       */
/*      �ŵ������ɹ����켶Ƶ��ת�뷢��̬ռ�ô�Ƶ�㣬������ 0                 */
/*      �ŵ�����ʧ�ܣ�����Ƶ��ת�����̬�������� -1                          */
/*****************************************************************************/
int DiffChDetect(unsigned char uchFreNo)
{
	unsigned char Tslice;
	
	unsigned char uchFreNum = uchFreNo;
	
	Tslice = DLL_SLICE_TIME;    //��ʼ������Сʱ��Ƭ=2
	    
      FrequencyHop(uchFreNum);  //ת���켶Ƶ�㣬�����ϼ��������¼� �������̬
	
	if(GetRSSIPower() < RF_LISTEN_RSSI) //�ز�����ź�ֵС���������ޣ��ŵ�����
	{                
                
                //��ƵоƬready
                SetRFReady();                
                SetTimer0A0_ACLK();
                Timer0A0_Delay(PHY_ACLK_1MS);	
                while(g_uPHYTimer0A0Count<2*Tslice); //�ӳ�ʱ��T=2*Tslice
                StopTimer0A0(); 
          
                //Sleep_Mode3_ACLKms(2*Tslice,RF_READY);

                SetRFRx();//ת�����̬
		
		if(GetRSSIPower() < RF_LISTEN_RSSI)  //�ŵ�������
		{			
                  //SetRFReady();
			return(0); //���ؾ����ɹ�
		}
		else
		{
			//FrequencyHop(g_uchRFSelfFreNum);  //ת�뱾��Ƶ��
			
                  //SetRFStandby();
                        
			return(-1);  //���ؾ���ʧ��
		}
	}
	else
	{
		//FrequencyHop(g_uchRFSelfFreNum);  //ת�뱾��Ƶ��

                //SetRFStandby();
                
		return(-1);  //���ؾ���ʧ��
	}
}


/*****************************************************************************/
/*     �����ŵ����������㷨 �������βΣ�                                         */
/*     �ŵ������ɹ����������뷢��̬��ռ��Ƶ�㣬������ 0                      */
/*     �ŵ�����ʧ�ܣ�                            ���� -1 �����ڽ���̬��      */

/*****************************************************************************/
int SimiChDetect(unsigned char uchCurFreNum)
{
	unsigned char Tslice;
	unsigned char n; //����� 0~5 
	unsigned char uchCurFreNo = uchCurFreNum;

	Tslice = DLL_SLICE_TIME;//��ʼ������Сʱ��Ƭ=2
	
      FrequencyHop(uchCurFreNo);  //���뱾��Ƶ�� ת�����̬

	if(GetRSSIPower() < RF_LISTEN_RSSI)  //������С���������� ����ǰ�ŵ�����
	{                
                
                //��ƵоƬready
                SetRFReady();                
                SetTimer0A0_ACLK();
                Timer0A0_Delay(PHY_ACLK_1MS);	
                while(g_uPHYTimer0A0Count<2*Tslice); //�ӳ�ʱ��T=2*Tslice
                StopTimer0A0(); 
                         
                //Sleep_Mode3_ACLKms(2*Tslice,RF_READY);

                SetRFRx();//ת�����̬
		
		if(GetRSSIPower() < RF_LISTEN_RSSI)  //�ŵ�������
		{    
                        srand(GetRSSI());  //��RSSIֵΪ�������
			      n = rand()%6;  
                        
                        //��ƵоƬready
                        SetRFReady();                          
                        SetTimer0A0_ACLK();
                        Timer0A0_Delay(PHY_ACLK_1MS);	
                        while(g_uPHYTimer0A0Count<n*Tslice); //�ӳ�ʱ��T=n*Tslice
                        StopTimer0A0();
                        
                        //Sleep_Mode3_ACLKms(n*Tslice,RF_READY);

                        SetRFRx();//ת�����̬
			
			if(GetRSSIPower() < RF_LISTEN_RSSI)  //������С���������� ����ǰ�ŵ�������
			{
                          //SetRFReady();
			        return(0); //���ؾ����ɹ�
			}
		}		
	}

      //SetRFStandby();
        
	return(-1);//����ʧ��
}



/********************************************************************************/
/*��IDΪDevID���豸������˹�ϵ����������������������������������               */
/********************************************************************************/
void ReleaseRelation(unsigned int DevID)
{
	if(DevID == g_uNetParentID)
	{
		g_uDLLStaUp = 0;  //���������ȶ�����ֵ��0
	}
	else 
	{
		for(char count=0; count<NET_MAX_CAP; count++)
		{
			if(DevID == g_SubDevInfo[count].NetID)
			{ 
				memset((void *) & g_SubDevInfo[count],0x00,sizeof(g_SubDevInfo[count]));  //ɾ�����豸
				break;
			}   
		}
	}
}


/********************************************************************************/
/*�켶ͨѶ������������������������������������������������������������          */
/*   �ŵ���⣬�޸��ţ����������ѯ֡������ȷ�ϻظ�                             */
/*   ��ȷȷ��  ���� 0                                                           */
/*       ����  �޸���Ӧ��Ϣ������ -1                                            */
/*unsigned char uchFreNum  ����ͨѶ���켶Ƶ��                                   */
/*int BuffFrmNo  ������֡��֡�����еı��                                       */
/********************************************************************************/
int DiffFreqCommunication(unsigned char uchFreNo,int BuffFrmNum)
{
	int returnval;
	unsigned int uUpID;
	unsigned char uchFreNum = uchFreNo;
	int BuffFrmNo = BuffFrmNum;
	
	if(0 == DiffChDetect(uchFreNum))  //�����켶�ŵ������㷨�ɹ�
	{ 
		RFSendPacket(g_uchWirelessTxBuf[BuffFrmNo], g_uchWirelessTxBuf[BuffFrmNo][0]);
		returnval = GetWirelessPacket(60);
		
		FrequencyHop(g_uchRFSelfFreNum);  //���뱾��Ƶ�����̬

            //��ƵоƬready
            SetRFReady();
		
		if((returnval == 0) && (g_uchWirelessRxBuf[11]==0x55) && (g_uchWirelessRxBuf[12]==0xa0))//�Ǿܾ�����ȷ��
		{
		    g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //�ӻ������������֡
		    ReleaseRelation(g_uchWirelessRxBuf[3]+(unsigned int)(g_uchWirelessRxBuf[4]*256));//�뷢�;ܾ�֡���豸������˹�ϵ
		    
		    if( uchFreNum == g_uchNetParentFreqNo )	//�켶ͨѶ�� ��Ƶ��
		        g_uDLLStaUp = 0;	
                
		    return(-1); //���ش�����Ϣʧ�� 
		
		}//end if �Ǿܾ�����ȷ��		
		
		//Tresʱ�����յ� ���˾ܾ�����֡���ȷ��֡ ����Ŀ�ĵ�ַ���Լ���
		if( (returnval == 0)  && (g_uchWirelessRxBuf[11]==0x55) && ( g_uchWirelessRxBuf[1] == g_uchWirelessTxBuf[BuffFrmNo][3]) && (g_uchWirelessRxBuf[2] == g_uchWirelessTxBuf[BuffFrmNo][4]))
		{	
			g_uchWirelessTxBufCtl[BuffFrmNo] = ( (g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ((g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));//֡���䳢�Դ�����1
                                                
                        if( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F) > FRAME_MAX_SEND_TIMES) //֡���Դ�����������
                                    g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//�ӻ������������֡ 			
		        
			if(g_uchWirelessRxBuf[12] == g_uchWirelessTxBuf[BuffFrmNo][11]) //��ȷȷ�� 
			{						
                                uUpID = ((unsigned int)g_uchWirelessTxBuf[BuffFrmNo][2]*256) + g_uchWirelessTxBuf[BuffFrmNo][1];  
                                if(uUpID == g_uNetParentID)  //������֡Ŀ�ĵ�ַ���ϼ���վ
                                {                                      
                                       g_uDLLStaUp = DLL_MAX_STAUP ; //������·ϵ����ֵ���                                  
                                }	
				
				if((g_uchWirelessTxBufCtl[BuffFrmNo]&0x40) != 0x40)//֡������bit6��Ϊ1������Ҫ�ܻظ�
	                            g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //�ӻ������������֡
							
				g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
							
				switch(g_uchWirelessRxBuf[12])//�ж��յ��Ļظ�֡����
				{				    
					case 0x40: //����֡ȷ��
					{
					    if( (g_uchWirelessRxBuf[17] <=254) && (g_uchWirelessRxBuf[17]>0) )
					        g_uAppDataInt = g_uchWirelessRxBuf[17]*60; //����Ϊ��λ,�趨ע����ʱ��
					    else
					        g_uAppDataInt = 21600;//��Сʱ
					    //���¼��㵱ǰ�豸��·����
					    LQEvaluation(g_uchWirelessRxBuf[14],g_uchRSSI,g_uchWirelessRxBuf[16],g_uDLLStaUp);
									
					    //�޸ĵ�ǰ���缶��
					    g_uchNetLev = g_uchWirelessRxBuf[15]+1;
									
					    break;
				        }
					case 0x30://ת��������ע��֡��ȷ��
					{
					     if( (g_uchWirelessRxBuf[16] <=254) && (g_uchWirelessRxBuf[16]>0) )
					        g_uAppDataInt = g_uchWirelessRxBuf[16]*60; //����Ϊ��λ,�趨ע����ʱ��
					    else
					        g_uAppDataInt = 21600;//��Сʱ
					        
                                            //���¼��㵱ǰ�豸��·����
                                            //LQEvaluation(g_uchWirelessRxBuf[14],g_uchRSSI,g_uchWirelessRxBuf[16],g_uDLLStaUp);									
                                            //�޸ĵ�ǰ���缶��
                                            g_uchNetLev = g_uchWirelessRxBuf[15]+1;
					}
					default:
					    break;
				}//end switch
				
				return 0;
							
			}//end if ��ȷȷ��
			    
                        else//����  ����æ ������ȷ��
                        {
			        uUpID = ((unsigned int)g_uchWirelessTxBuf[BuffFrmNo][2]*256) + g_uchWirelessTxBuf[BuffFrmNo][1];  
                                if(uUpID == g_uNetParentID)  //������֡Ŀ�ĵ�ַ���ϼ���վ
                                {                                          
                                      if(g_uDLLStaUp > DLL_STAUP_RIGHT)
                                          g_uDLLStaUp -= DLL_STAUP_RIGHT ; // ����Ȩ�ؼӴ� 
                                      else
                                       g_uDLLStaUp = 0;  
                                }
                                
				return 1;//��ʱ����1
			}			
		}//end if //Tresʱ�����յ� ���˾ܾ�����֡���ȷ��֡ ����Ŀ�ĵ�ַ���Լ���
		
		else  //Tresʱ����û�յ�ȷ��֡
		{ 
			uUpID = ((unsigned int)g_uchWirelessTxBuf[BuffFrmNo][2]*256) + g_uchWirelessTxBuf[BuffFrmNo][1];  
			if(uUpID == g_uNetParentID)  //������֡Ŀ�ĵ�ַ���ϼ���վ
			{                                     
                                    if(g_uDLLStaUp > DLL_STAUP_RIGHT)
                                      g_uDLLStaUp -= DLL_STAUP_RIGHT ; // ����Ȩ�ؼӴ� 
                                    else
                                       g_uDLLStaUp = 0;                                     
			}
		}
	} //end if(0 == DiffChDetect(uchFreNum)) 
	
	else  //�������ɹ�
	{
		uUpID =  ((unsigned int)g_uchWirelessTxBuf[BuffFrmNo][2]*256) + g_uchWirelessTxBuf[BuffFrmNo][1];  
		if(uUpID == g_uNetParentID) //������֡Ŀ�ĵ�ַ���ϼ���վ
		{
                              if(g_uDLLStaUp > DLL_STAUP_RIGHT)
                                   g_uDLLStaUp -= DLL_STAUP_RIGHT ; //����Ȩ�ؼӴ� 
                              else
                                   g_uDLLStaUp = 0;                                     
		}
		
		FrequencyHop(g_uchRFSelfFreNum);  //���뱾��Ƶ�����̬
	} 
	
	g_uchWirelessTxBufCtl[BuffFrmNo] = ( (g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ((g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));//֡���䳢�Դ�����1
	if( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F) > FRAME_MAX_SEND_TIMES) //֡���Դ�������������
		g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//�ӻ������������֡     

        return(-1);  //���ش�����Ϣʧ��         
	
}


/********************************************************************************/
/*��վ����ͨѶ                                                                  */
/*   �ŵ���⣬�޸��ţ����������ѯ֡������ȷ�ϻظ�                             */
/*   ��ȷȷ��  ���� 0                                                           */
/*       ����  �޸���Ӧ��Ϣ������ -1                                            */
/* BuffFrmNo��������֡��֡�����еı��                                       */
/********************************************************************************/
int BaseSelfFreqCommunication(int BuffFrmNum)
{  
	int returnval;
	int BuffFrmNo = BuffFrmNum;
	
	if(0 == SimiChDetect(g_uchRFSelfFreNum)) //���ñ����ŵ������ɹ�
	{
		RFSendPacket(g_uchWirelessTxBuf[BuffFrmNo],g_uchWirelessTxBuf[BuffFrmNo][0]);
		returnval = GetWirelessPacket(60);
                
            //��ƵоƬready
            SetRFReady();
		
		if((returnval == 0) && (g_uchWirelessRxBuf[11]==0x55) && (g_uchWirelessRxBuf[12]==0xa0))//�Ǿܾ�����ȷ��
		{
		    g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //0�ӻ������������֡
		    ReleaseRelation(g_uchWirelessRxBuf[3]+(unsigned int)(g_uchWirelessRxBuf[4]*256));//�뷢�;ܾ�֡���豸������˹�ϵ
		   
		    g_uDLLStaDown = 0;
                    
		    return(-1); //���ش�����Ϣʧ�� 
		
		}//end if �Ǿܾ�����ȷ��
		
		//Tresʱ�����յ� ���˾ܾ�����֡���ȷ��֡ ����Ŀ�ĵ�ַ���Լ���
		if( (returnval == 0)  && (g_uchWirelessRxBuf[11] == 0x55) && (g_uchWirelessRxBuf[1] ==g_uchWirelessTxBuf[BuffFrmNo][3]) && (g_uchWirelessRxBuf[2] ==g_uchWirelessTxBuf[BuffFrmNo][4]))
		{		        										
                            g_uchWirelessTxBufCtl[BuffFrmNo] = ( (g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ((g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));//֡���䳢�Դ�����1
                            if( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F) > FRAME_MAX_SEND_TIMES) //֡���Դ�����������
                                    g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//�ӻ������������֡  						        
     
			    if(g_uchWirelessRxBuf[12] == g_uchWirelessTxBuf[BuffFrmNo][11]) //��ȷȷ�� 
			    {				        
				if(g_uDLLStaDown < DLL_MAX_STADOWN)
			            g_uDLLStaDown++;  //�����ȶ�����ֵ+1	
						    			        
				if((g_uchWirelessTxBufCtl[BuffFrmNo]&0x40) != 0x40)//֡������bit6��Ϊ1������Ҫ�ܻظ�
	                            g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //�ӻ������������֡
	                                            
				g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
				
                                return 0;
				
			    }//end if ��ȷȷ��
			    else//����  ����æ ������ȷ��
			    {
				if(g_uDLLStaDown>0)
				     g_uDLLStaDown--;  //�����ȶ�����ֵ-1
                                
				return 1;//��ʱ����1
			    }					
		}//end if Tresʱ�����յ� ���˾ܾ�����֡���ȷ��֡ ����Ŀ�ĵ�ַ���Լ���
		else
		{    
			if(g_uDLLStaDown>0)
				g_uDLLStaDown--;  //�����ȶ�����ֵ-1			
		}
	}//end if ���ñ����ŵ������ɹ�	
	else
	{ 
		if(g_uDLLStaDown>0)  
			g_uDLLStaDown--;  //�����ȶ�����ֵ-1
	}
	
	if(g_uDLLStaDown <=0)            
	    return -1; 

	g_uchWirelessTxBufCtl[BuffFrmNo] = ( (g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));
	if(g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F > FRAME_MAX_SEND_TIMES) //֡���Դ�����������
		g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//�ӻ������������֡     
        
	return(-1);
}


/********************************************************************************/
/*�˻�����ͨѶ                                                                  */
/*   ��ȷȷ��  ���� 0                                                           */
/*       ����  �޸���Ӧ��Ϣ������ -1                                            */
/*   BuffFrmNo��������֡��֡�����еı��                                     */
/********************************************************************************/
int TermSelfFreqCommunication(int BuffFrmNum)
{
	int returnval;	
	
	int BuffFrmNo = BuffFrmNum;
	
	g_uDLLCATime = DLL_TR_TIME;//�豸ͨѶ��������������Ҫ���ӳ�ʱ�䣬��Χ��DLL_TR_TIME~10*DLL_TR_TIME��
	
	while(1)
	{
		if(0 == SimiChDetect(g_uchRFSelfFreNum)) //���ñ����ŵ������ɹ�
		{
			RFSendPacket(g_uchWirelessTxBuf[BuffFrmNo],g_uchWirelessTxBuf[BuffFrmNo][0]);
			returnval = GetWirelessPacket(60);
                        
                  //��ƵоƬready
                  SetRFReady();
			
			if((returnval == 0) && (g_uchWirelessRxBuf[11]==0x55) && (g_uchWirelessRxBuf[12]==0xa0))//�Ǿܾ�����ȷ��
		        {
                            g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //0�ӻ������������֡
                            ReleaseRelation(g_uchWirelessRxBuf[3]+(unsigned int)(g_uchWirelessRxBuf[4]*256));//�뷢�;ܾ�֡���豸������˹�ϵ
                            
                            g_uDLLStaUp = 0;

                            return(-1); //���ش�����Ϣʧ�� 
		
		        }//end if �Ǿܾ�����ȷ��
			
			//Tresʱ�����յ����˾ܾ�����֡���ȷ��֡ ����Ŀ�ĵ�ַ���Լ���
			if( (returnval == 0)  && (g_uchWirelessRxBuf[11] == 0x55) && (g_uchWirelessRxBuf[1] ==g_uchWirelessTxBuf[BuffFrmNo][3]) && (g_uchWirelessRxBuf[2] ==g_uchWirelessTxBuf[BuffFrmNo][4]))
			{			          			        						
                                    g_uchWirelessTxBufCtl[BuffFrmNo] = ( (g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ((g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));//֡���䳢�Դ�����1
                                    if( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F) > FRAME_MAX_SEND_TIMES) //֡���Դ�����������
                                        g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//�ӻ������������֡ 						        
     
				    if(g_uchWirelessRxBuf[12] == g_uchWirelessTxBuf[BuffFrmNo][11]) //��ȷȷ�� 
				    {	                                     
                                          g_uDLLStaUp = DLL_MAX_STAUP ; //������·ϵ����ֵ���
							    										        
	                                  g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //0�ӻ������������֡
	
					  g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
							
					  switch(g_uchWirelessRxBuf[12])//��ǰ �˻�����ͨѶ�� ֻ��������Ϣ
					  {
						case 0x40: //����֡ȷ��
						{
							 if( (g_uchWirelessRxBuf[17] <=254) && (g_uchWirelessRxBuf[17]>0) )
                                                              g_uAppDataInt = g_uchWirelessRxBuf[17]*60; //����Ϊ��λ,�趨ע����ʱ��
                                                          else
                                                              g_uAppDataInt = 21600;//��Сʱ
							//���¼��㵱ǰ�豸��·����
							LQEvaluation(g_uchWirelessRxBuf[14],g_uchRSSI,g_uchWirelessRxBuf[16],g_uDLLStaUp);
									
							//�޸ĵ�ǰ���缶��
							g_uchNetLev = g_uchWirelessRxBuf[15]+1;
									
							break;
						}
						default:
							break;
					  }//end switch

					  return 0;
							
				    }//end if ��ȷȷ��
				    else//����  ����æ ������ȷ��
				    {
                                        if( g_uDLLStaUp > DLL_STAUP_RIGHT )
                                            g_uDLLStaUp -= DLL_STAUP_RIGHT ; //����Ȩ�ؼӴ� 	
                                        else
                                            g_uDLLStaUp = 0;				    
                                        
					return 1;//��ʱ����1
				    }                       
			}//end if Tresʱ�����յ����˾ܾ�����֡���ȷ��֡ ����Ŀ�ĵ�ַ���Լ���
			else  //Tresʱ����û�յ�ȷ��֡
			{
                                if( g_uDLLStaUp > DLL_STAUP_RIGHT )
                                        g_uDLLStaUp -= DLL_STAUP_RIGHT ; //������ע��ʧ�� ����Ȩ�ؼӴ� 
                                else
                                        g_uDLLStaUp = 0;
					
				if( g_uDLLStaUp == 0) //�����ȶ�ϵ��Ϊ0 
				{
					g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //�������������֡

                                        return(-1);
				} 				
			}
		}//end if ���ñ����ŵ������ɹ�	
		
		else  //���ñ����ŵ��������ɹ�
		{
                                if( g_uDLLStaUp > DLL_STAUP_RIGHT )
                                        g_uDLLStaUp -= DLL_STAUP_RIGHT ; // ����Ȩ�ؼӴ� 
                                else
                                        g_uDLLStaUp = 0;
					
				if( g_uDLLStaUp == 0) //�����ȶ�ϵ��Ϊ0 
				{
					g_uchWirelessTxBufCtl[BuffFrmNo] = 0; //�������������֡
                
                                        return(-1);
				} 

		}//end else 		  
		
		g_uchWirelessTxBufCtl[BuffFrmNo] = ((g_uchWirelessTxBufCtl[BuffFrmNo] & 0xf0) | ((g_uchWirelessTxBufCtl[BuffFrmNo]&0x0f) + 0x01));  //֡���䳢�Դ�����1 ������������
		
		if( (g_uchWirelessTxBufCtl[BuffFrmNo]&0x0F) > FRAME_MAX_SEND_TIMES) //֡���Դ�����������
                {
                    g_uchWirelessTxBufCtl[BuffFrmNo] = 0;//�ӻ������������֡ 	
                    
                    return(-1);
                }
		
		else //�����ʱ
		{	
                        Sleep_Mode3_ACLKms(g_uDLLCATime,RF_STANDBY);    
                        /*
                        //��ƵоƬready
                        SetRFReady(); 
                        SetTimer0A0_ACLK();
                        Timer0A0_Delay(PHY_ACLK_1MS);	
                        while(g_uPHYTimer0A0Count<g_uDLLCATime); //�ӳ�ʱ��T=n*Tslice
                        StopTimer0A0();
                        */
 
			g_uDLLCATime = ( unsigned int ) ( (1.5*g_uDLLCATime) < (10*DLL_TR_TIME) ? (1.5*g_uDLLCATime):(10*DLL_TR_TIME)); //�޸ı����ӳ�ʱ��
			
			continue;
		}		
	}//end while(1)
}

/****************************************************************************/
/* ��������������������    Ѱ�ϼ�����ӦѡƵ�㷨   ��    ����        	    */
/*                  ��Ƶ�ɹ�      ����  0                                   */
/*                  ��Ƶ���ɹ�    ����  -1                                   */
/* uchLimitGrade  Ϊ�������� �����м���������Ϊʵ�ʼ�������ֵ                */
/*                             ���޼����������ݶ�Ϊ 0xff                     */
/****************************************************************************/
int SearchParentFreq(unsigned char LimitGrade)
{
	int returnVal;
	int PageNo;
	
	unsigned char uchLimitGrade = LimitGrade;
	unsigned char upNum = 0;   //��Ų�ѯ�����ϼ��豸��Ŀ
	unsigned char LQList;      //��ű�����������LQֵ���±�
      
      #define SEARCH_NUM 5
	
	typedef struct{
		unsigned char tempLQ;     //��·����
		unsigned int tempNetID;   //����ID
		unsigned char tempFre;    //����Ƶ��
		unsigned char tempNetLev; //���缶��
		unsigned char tempPAtype;//�豸����
	}tempupDev;  
	tempupDev upDev[SEARCH_NUM];  //�����ѯ�ɹ�����ϼ��豸��Ϣ
	
	unsigned char * c = (unsigned char *) &upDev; 
	
	for(int i = 0; i<sizeof(upDev) ; i++)
	  c[i] = 0; 
	
	GetRandomFreList(); //��ȡ���Ƶ������
	
	//����A�Ͳ��ӹ��ŵĽڵ� �����Լ��Ĳ��ӹ���������Ѱ���Ҳ������ٵ��ӹ���Ƶ����Ѱ�ϼ��豸
	//����B�ͼӹ��ŵĽڵ� �����Լ��ļӹ���������Ѱ;  �Ҳ���,�رչ��ţ��ٵ����ӹ���������Ѱ		
	g_uchRxPAtype = g_uchPHYPAType ;//��ʼ���Լ�ͬ���͵��豸���ڵ�Ƶ��ͨѶ
	
	unsigned char EndFreNum=0;//������Ƶ��Ƶ���±�
	unsigned char StartFreNum = 0; //��ʼ��Ƶ��Ƶ���±�
	if(g_uchPHYPAType == 0x0a)//���ǲ��ӹ��Žڵ�
	{
	  EndFreNum = RF_FRE_A_AMOUNT;
	  StartFreNum =0;
	}
	else if(g_uchPHYPAType == 0x0b)//���Ǽӹ��Žڵ�
	{
	  EndFreNum = RF_FRE_B_AMOUNT+RF_FRE_A_AMOUNT;
	  StartFreNum = RF_FRE_A_AMOUNT;
	}
	  
	if(EndFreNum == StartFreNum)
	  return -1;
	
	for(; StartFreNum<EndFreNum; StartFreNum++) //�ڸ���Ƶ������Ѱ�ң� ͬʱ���Ÿ���Ƶ�������ķ�Χ��ͬ���򿪻�ر�
	{
	        if(g_chRFFreList[StartFreNum] == g_uchRFSelfFreNum) //�ϼ�Ƶ����Լ�����ͬƵ
	          continue;
	          
	        //�豸ͨѶ��������������Ҫ���ӳ�ʱ�䣬��Χ��DLL_TR_TIME~10*DLL_TR_TIME��
	        g_uDLLCATime = DLL_TR_TIME;
                
                //g_chRFFreList[StartFreNum]=1;
	        
		for(char i=0;i<5;i++)
		{
			PageNo = GenerateQueryFrame(INFO_SIMPLE_QUERY); //�γ����ϼ��豸���͵Ĳ�ѯ֡
			returnVal = DiffFreqCommunication(g_chRFFreList[StartFreNum],PageNo);
			if(returnVal == 0)
			  break;					
                        
                        Sleep_Mode3_ACLKms(g_uDLLCATime,RF_STANDBY);
                        /*
                        //��ƵоƬready
                        SetRFReady();                          
                        SetTimer0A0_ACLK();
                        Timer0A0_Delay(PHY_ACLK_1MS);	
                        while(g_uPHYTimer0A0Count<g_uDLLCATime); //�ӳ�ʱ��g_uDLLCATime
                        StopTimer0A0(); 
                        */                        

			g_uDLLCATime = ( unsigned int ) ((1.5*g_uDLLCATime) < (10*DLL_TR_TIME) ? (1.5*g_uDLLCATime):(10*DLL_TR_TIME)); //�޸ı����ӳ�ʱ��
		}
                
		if( (returnVal == 0)  &&(g_uchWirelessRxBuf[12] == INFO_SIMPLE_QUERY))//�յ��򵥲�ѯȷ�ϻظ���������ϼ��豸��Ϣ��LQֵ
		{ 
			if(g_uchWirelessRxBuf[13] > uchLimitGrade) //���ѵ����ϼ��豸���缶��ֵ �����豸��ǰ�����缶���򲻱�����豸��������������
				continue;
				
		        //if(( g_uAppDataType == 2)  &&//ͼ��
		         //   (g_uchWirelessRxBuf[13] < 3) )//Ҫ�������ٵ�3��������
			 //       continue;
                   //     if(g_uchWirelessRxBuf[13] < 2)//Ҫ�������ٵ�2��������
			 //       continue;
		        
		        
		        if( g_uchWirelessRxBuf[19] >= NET_MAX_CAP) //��������豸̫�� 
		                continue;
			
			upDev[upNum].tempNetLev = g_uchWirelessRxBuf[13]; //�����ϼ��豸�ظ������缶�� 
			upDev[upNum].tempNetID = g_uchWirelessRxBuf[7] + (unsigned int) (g_uchWirelessRxBuf[8]*256); //�����ϼ��豸������ID
			upDev[upNum].tempFre = g_chRFFreList[StartFreNum];  //���浱ǰ����Ƶ��
			upDev[upNum].tempPAtype = g_uchWirelessRxBuf[14]; //�ϼ��豸��������
			
			LQEvaluation(g_uchWirelessRxBuf[18], g_uchRSSI, g_uchWirelessRxBuf[19], 10); //������ϼ��豸�����豸����·����                    
			upDev[upNum].tempLQ = g_uchDLLCurLQ;    //�����ϼ��豸�����豸����·����                  
			
			if(upDev[upNum].tempLQ >= g_uchDLLQ)       //��Ĭ�ϵ�LQֵ��
			{
				returnVal = SubDevLog(g_chRFFreList[StartFreNum],upDev[upNum].tempNetID,upDev[upNum].tempNetLev,upDev[upNum].tempPAtype); //�˻���������ע��֡ 
				if(returnVal == 0) //ע��ɹ�
				{
					
					g_uDLLStaUp = DLL_MAX_STAUP;// �����ȶ�ϵ������ֵ 
					g_uchDLLCurLQ = upDev[upNum].tempLQ;// ������·��������ֵ
					g_uNetParentID = upDev[upNum].tempNetID; //���游������ID
					g_uchNetParentFreqNo = upDev[upNum].tempFre;//����Ƶ��
					g_uchNetLev = upDev[upNum].tempNetLev + 1;//�������� = �ϼ����� + 1
					g_uNetParentPAtype = upDev[upNum].tempPAtype; //�����豸��������
					
					g_uchDLLQ = upDev[upNum].tempLQ < DLL_LINK_QUAL ? upDev[upNum].tempLQ : DLL_LINK_QUAL;  //ȡ����֮�н�С��
					
					return(0x00);  //������Ƶ�ɹ�
				}				
			}			
			upNum++;                      //������ϼ��豸������1�� 
                  if(upNum >= (SEARCH_NUM-1))
                  {
                      upNum = SEARCH_NUM-1;
                      break;
                  }
		}
		
	}//end for  �ڸ���Ƶ������Ѱ�ң� ͬʱ���Ÿ���Ƶ�������ķ�Χ��ͬ���򿪻�ر�
	
	if ( upNum == 0 )//������Ƶ�㷶Χ���Ҳ����ϼ� ����������Ƶ����
	{
	    EndFreNum = 0;
	    StartFreNum = 0;
	    
	    if(g_uchPHYPAType == 0x0a)//���ǲ��ӹ��Žڵ� Ҫ�ڼӹ��ŵ�Ƶ��ȥѰ��
            {
              EndFreNum = RF_FRE_B_AMOUNT+RF_FRE_A_AMOUNT;
              StartFreNum = RF_FRE_A_AMOUNT;
              g_uchRxPAtype = 0x0b ;//���Լ� ��ͬ���͵��豸���ڵ�Ƶ��ͨѶ
            }
            else if(g_uchPHYPAType == 0x0b)//���Ǽӹ��Žڵ� �ڲ��ӹ��ŵ�Ƶ��ȥѰ��
            {
              EndFreNum = RF_FRE_A_AMOUNT;
              StartFreNum =0;
              g_uchRxPAtype = 0x0a ;//���Լ� ��ͬ���͵��豸���ڵ�Ƶ��ͨѶ
            }
              
            if(EndFreNum == StartFreNum)
              return -1;
	  
	    for(; StartFreNum < EndFreNum ; StartFreNum++) //������Ƶ�㷶Χ���Ҳ����ϼ� ����������Ƶ����
	    {
	        //�豸ͨѶ��������������Ҫ���ӳ�ʱ�䣬��Χ��DLL_TR_TIME~10*DLL_TR_TIME��
	        g_uDLLCATime = DLL_TR_TIME;
	        
		for(char i=0;i<5;i++)
		{
			PageNo = GenerateQueryFrame(INFO_SIMPLE_QUERY); //�γ����ϼ��豸���͵Ĳ�ѯ֡
			returnVal = DiffFreqCommunication(g_chRFFreList[StartFreNum],PageNo);
			if(returnVal == 0)
				break;					

                        Sleep_Mode3_ACLKms(g_uDLLCATime,RF_STANDBY); 
                        /*
                        //��ƵоƬready
                        SetRFReady();
                        SetTimer0A0_ACLK();
                        Timer0A0_Delay(PHY_ACLK_1MS);	
                        while(g_uPHYTimer0A0Count<g_uDLLCATime); //�ӳ�ʱ��g_uDLLCATime
                        StopTimer0A0();
                        */
			
		}
                
		if( (returnVal == 0)  &&(g_uchWirelessRxBuf[12] == INFO_SIMPLE_QUERY))//�յ��򵥲�ѯȷ�ϻظ���������ϼ��豸��Ϣ��LQֵ
		{ 
			if(g_uchWirelessRxBuf[13] > uchLimitGrade) //���ѵ����ϼ��豸���缶��ֵ �����豸��ǰ�����缶���򲻱�����豸��������������
				continue;
				
		        if( g_uchWirelessRxBuf[19] >= NET_MAX_CAP) //��������豸̫�� 
		                continue;
			
			upDev[upNum].tempNetLev = g_uchWirelessRxBuf[13]; //�����ϼ��豸�ظ������缶�� 
			upDev[upNum].tempNetID = g_uchWirelessRxBuf[7] + (unsigned int) (g_uchWirelessRxBuf[8]*256); //�����ϼ��豸������ID
			upDev[upNum].tempFre = g_chRFFreList[StartFreNum];  //���浱ǰ����Ƶ��
			upDev[upNum].tempPAtype = g_uchWirelessRxBuf[14]; //�ϼ��豸��������
			
			LQEvaluation(g_uchWirelessRxBuf[18], g_uchRSSI, g_uchWirelessRxBuf[19], 10); //������ϼ��豸�����豸����·����                    
			upDev[upNum].tempLQ = g_uchDLLCurLQ;    //�����ϼ��豸�����豸����·����                  
			
			if(upDev[upNum].tempLQ >= g_uchDLLQ)       //��Ĭ�ϵ�LQֵ��
			{
				returnVal = SubDevLog(g_chRFFreList[StartFreNum],upDev[upNum].tempNetID,upDev[upNum].tempNetLev,upDev[upNum].tempPAtype); //�˻���������ע��֡ 
				if(returnVal == 0) //ע��ɹ�
				{
					
					g_uDLLStaUp = DLL_MAX_STAUP;// �����ȶ�ϵ������ֵ 
					g_uchDLLCurLQ = upDev[upNum].tempLQ;// ������·��������ֵ
					g_uNetParentID = upDev[upNum].tempNetID; //���游������ID
					g_uchNetParentFreqNo = upDev[upNum].tempFre;//����Ƶ��
					g_uchNetLev = upDev[upNum].tempNetLev + 1;//�������� = �ϼ����� + 1
					g_uNetParentPAtype = upDev[upNum].tempPAtype; //�����豸��������
					
					g_uchDLLQ = upDev[upNum].tempLQ < DLL_LINK_QUAL ? upDev[upNum].tempLQ : DLL_LINK_QUAL;  //ȡ����֮�н�С��
					
					return(0x00);  //������Ƶ�ɹ�
				}				
			}			
			upNum++;                      //������ϼ��豸������1�� 
                  if(upNum >= (SEARCH_NUM-1))
                  {
                      upNum = SEARCH_NUM-1;
                      break;
                  }
		}
		
	    }//end for  ������Ƶ�㷶Χ���Ҳ����ϼ� ����������Ƶ����  	    
	
	}//end if  ������Ƶ�㷶Χ���Ҳ����ϼ� ����������Ƶ����
	
	if(upNum != 0) //������ϼ��豸��LQֵ��û��Ĭ�ϵ�LQֵ�ã�����Ҫ�ӱ���ĽϺ�lqֵ��ѡһ��ע��
	{
		unsigned char tempupNum = upNum;
		
		while(1)//ѭ���õ�����ע��Ļ�վ
		{
			unsigned char tempMaxLQ = 0;
			LQList = 0;  //LQ�����е��±�
			for(int count=0; count<upNum; count++) //ѡȡLQֵ��õģ��������������±�
			{
				if(upDev[count].tempLQ > tempMaxLQ)
				{
					tempMaxLQ = upDev[count].tempLQ;
					LQList = count;
				}
			}
						
			//��Ҫ������ѡ�����ϼ��豸��Ϣ���������ﱣ��
			returnVal = SubDevLog(upDev[LQList].tempFre,upDev[LQList].tempNetID,upDev[LQList].tempNetLev,upDev[LQList].tempPAtype);
			
			if(returnVal == 0) //ע��ɹ�
			{
				g_uDLLStaUp = DLL_MAX_STAUP;  // �����ȶ�ϵ������ֵ      ������·��������ֵ               
				g_uchDLLCurLQ = upDev[LQList].tempLQ;//  
				g_uNetParentID = upDev[LQList].tempNetID; //���游������ID
				g_uchNetParentFreqNo = upDev[LQList].tempFre;//����Ƶ��
				g_uchDLLQ = upDev[LQList].tempLQ;  
				g_uchNetLev = upDev[LQList].tempNetLev + 1;//�������� = �ϼ����� + 1
				g_uNetParentPAtype = upDev[LQList].tempPAtype; //�����豸��������
				
				return(0);
			}
			else
			{
				upDev[LQList].tempLQ = 0x00;    //ɾ����ע��δ�ɹ����ϼ��豸��Ϣ
				tempupNum--;
			}
			if(tempupNum == 0)//�ϼ��豸��ϢΪ��
				return(-1);
		} // end while ѭ���õ�����ע��Ļ�վ   
	}
	else 
		return(-1);//�ϼ���վ��ĿΪ�գ�����ʧ��
	
}   

/****************************************************************************/
/* ���������������� ȷ����������Ƶ�������ӦѡƵ����   ��            	    */
/*                  ��Ƶ�ɹ�      ����  0                                   */
/*                  ��Ƶ���ɹ�    ����  -1                                  */
/****************************************************************************/
int SearchSelfFreq()
{    
	int trytimes = 8;
	int listentimes = 0;
	int returnVal;
	int PageNo;
	unsigned char overRSSI = 0x00;        //��⵽������ͨѶ�ز�RSSIֵС�ڶֵ�Ĵ���
	
	unsigned char Tslice = DLL_SLICE_TIME;
	
	 // �ӹ����豸ֻ���ڼӹ���Ƶ����Ѱ�Լ���Ƶ��
	 //���ӹ����豸ֻ�����޹���Ƶ����Ѱ�Լ���Ƶ��
	 // ������Ѱʱ�������豸�����ͺ��Լ�����һ��
	g_uchRxPAtype = g_uchPHYPAType ; //�趨�����豸������
	
	GetRandomFreList(); //��ȡ���Ƶ������
	
	unsigned char EndFreNum=0;//������Ƶ��Ƶ���±�
	unsigned char StartFreNum = 0; //��ʼ��Ƶ��Ƶ���±�
	if(g_uchPHYPAType == 0x0a)//���ǲ��ӹ��Žڵ�
	{
	  EndFreNum = RF_FRE_A_AMOUNT;
	  StartFreNum =0;
	}
	else if(g_uchPHYPAType == 0x0b)//���Ǽӹ��Žڵ�
	{
	  EndFreNum = RF_FRE_B_AMOUNT+RF_FRE_A_AMOUNT;
	  StartFreNum = RF_FRE_A_AMOUNT;
	}
	  
	if(EndFreNum == StartFreNum)
	  return -1;
	  
	for(; StartFreNum < EndFreNum; StartFreNum++)//Ƶ������
	{     
	        
	        if( g_chRFFreList[StartFreNum] == g_uchNetParentFreqNo)
	          continue;
	          
	        trytimes = 8;
	        
		while(--trytimes >0) //����8�β�ѯ֡
		{
			returnVal = SimiChDetect(g_chRFFreList[StartFreNum]);//���ñ����ŵ����������㷨
			
                        if(returnVal != 0) //����ʧ�� 
				break;
			else //���óɹ�
			{		      
				overRSSI = 0;
				PageNo = GenerateQueryFrame(INFO_SIMPLE_QUERY);
				RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);//���Ͳ�ѯ֡
								
				for(listentimes = 0; listentimes < 3; listentimes++)
				{
                                        Sleep_Mode3_ACLKms(6*Tslice,RF_STANDBY);
                                        /*
                                        //��ƵоƬready
                                        SetRFReady();                                        
                                        SetTimer0A0_ACLK();
                                        Timer0A0_Delay(PHY_ACLK_1MS);	
                                        while(g_uPHYTimer0A0Count<6*Tslice); //�ӳ�ʱ��T=6*Tslice
                                        StopTimer0A0();
                                        */
                        
                                        SetRFRx();//ת�����̬
					 
					int RSSI_dbm = GetRSSIPower();
					if(RSSI_dbm >= RF_LISTEN_RSSI) //��û�лظ�����������ز��ź�ֵ��������ֵ
					{
						overRSSI = 1;
						break;
					}				
				}
				if(overRSSI > 0)//�Ѿ��յ�����
				{
					break;
				}

			}//end else ���óɹ�
			
			Sleep_Mode3_ACLKms(5,RF_STANDBY);
                        /*
                        //��ƵоƬready
                        SetRFReady();  
                        SetTimer0A0_ACLK();
                        Timer0A0_Delay(PHY_ACLK_1MS);	
                        while(g_uPHYTimer0A0Count<5);
                        StopTimer0A0();
                         */
                        
		}//end while ����8�β�ѯ֡
                
                //��ƵоƬready
                //SetRFReady();
		
		if(returnVal != 0) //�ŵ���������ʧ�� �������Ƶ��
			continue;
		
		if(overRSSI == 0) //�ŵ�Ϊ��
		{
			g_uchRFSelfFreNum = g_chRFFreList[StartFreNum];//�趨Ϊ��ǰ����Ƶ��
			
			g_uDLLStaDown = 10; //�����ȶ�ϵ�� 
                        
                        //g_uchRFSelfFreNum=2;//������ 

                        return(0);//����Ƶ��ռ�óɹ�
		}
		
	}//end for Ƶ������
        
	return(-1);//Ƶ������꣬����Ƶ��ռ��ʧ��
}

/*****************************************************************************/
/*  ���豸����ע������ ��                                                    */
/*       �������²��裺                                         	     */
/*             1����������ע��֡���ȴ��ϼ���վ�ظ��ȴ���֤֡                 */
/*             2���ȴ�һ��ʱ��֮���ٴη�������ע��֡�����յ�ת��������     */
/*                ���Ļ�վ����������֤����˵����Ƶ�ɹ�                       */
/*                �ȴ�ʱ�����һ��ȷ����������                               */
/*  ͨ����֤0x32������0                                                      */
/*  ��֤ʧ��0x33������-2                                                     */
/*  ����        ������-1                                                     */
/*****************************************************************************/
int SubDevLog(unsigned char uchFreNum,unsigned int tempNetID,int NetLev, unsigned char destPAtype)
{
	int returnVal;
	int PageNo;
	unsigned int WaitTime;//��λ��
	unsigned int MaxWaitTime = 60;//���ȴ�ʱ��60��
	unsigned int WaitTimeSlice = 10;//�ȴ�����ʱ�䵥Ԫ 10�� 
	int trytimes = 3;//��������ע��Ĵ���
	g_uDLLCATime = DLL_TR_TIME;
	
	unsigned char uchFreNo = uchFreNum; //Ƶ����
	int netlev = NetLev;//����ȼ� 
	
	g_uchRxPAtype = destPAtype; //�����豸�Ĺ�������
	
	unsigned char FirstDestID[2];
	
	FirstDestID[0] = tempNetID&0xff;
	FirstDestID[1] = (tempNetID/256)&0xff;
	
	for(char count=0; count<10; count++)  //����10����ռ�ŵ�
	{
		if(0 == DiffChDetect(uchFreNo))// Ƶ��̽��
		{
			PageNo = GenerateApplyLogFrame(FirstDestID);
			RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
			returnVal = GetWirelessPacket(60);
                        
                        //��ƵоƬready
                        //SetRFReady();
			
			//�յ��ظ� Ŀ�ĵ�ַ�Ǹ��Լ���
			if( (returnVal == 0) && (g_uchWirelessRxBuf[1] ==g_uchWirelessTxBuf[PageNo][3]) && (g_uchWirelessRxBuf[2] ==g_uchWirelessTxBuf[PageNo][4]))
			{	
				if((g_uchWirelessRxBuf[11] == 0x50) && (g_uchWirelessRxBuf[12] == 0x32)) //ͨ����֤ �����Ļ�վֱ�ӻظ��ɹ���ʧ�ܣ������ȴ���֤�ظ�
				{
					 //���ע����ʱ���ж�
                                        if ( ( g_uchWirelessRxBuf[23] <=254)  && (g_uchWirelessRxBuf[23] >0) )
                                            g_uAppDataInt=g_uchWirelessRxBuf[23]*60; //���������ϱ����ʱ��
                                        else
                                            g_uAppDataInt= 21600;//��Сʱ					

					 //���ע����ʱ���ж�
                                        if ( ( g_uchWirelessRxBuf[24] <= 60)  && (g_uchWirelessRxBuf[24] >0) )
                                            g_uDLLLogInt=g_uchWirelessRxBuf[24]*60; //������·��ע���� 
                                        else
                                            g_uDLLLogInt= 3600;//60����		
                                            		
					g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
					return(0);
				}
				
				if(g_uchWirelessRxBuf[11] == 0x55) //�յ�ȷ�ϻظ�
				{				
				        if(g_uchWirelessRxBuf[12] == 0x32) //�յ�ͨ����֤�ظ�
					{
                                                 //���ע����ʱ���ж�
                                                if ( ( g_uchWirelessRxBuf[24] <=254)  && (g_uchWirelessRxBuf[24] >0) )
                                                    g_uAppDataInt=g_uchWirelessRxBuf[24]*60; //���������ϱ����ʱ��
                                                else
                                                    g_uAppDataInt= 21600;//��Сʱ                                               
        
                                                 //���ע����ʱ���ж�
                                                if ( ( g_uchWirelessRxBuf[25] <= 60)  && (g_uchWirelessRxBuf[25] >0) )
                                                    g_uDLLLogInt=g_uchWirelessRxBuf[25]*60; //������·��ע���� 
                                                else
                                                    g_uDLLLogInt= 3600;//60����				
						
						g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;										
						return(0);
					}
					
					if((g_uchWirelessRxBuf[12] == 0xa0)||(g_uchWirelessRxBuf[12] == 0xa2)||(g_uchWirelessRxBuf[12] == 0x33))//�ܾ����� �� ����æ ����ע��ʧ��
					{
						return(-1);
					}
			
					if(g_uchWirelessRxBuf[12] == 0x31) //�ȴ���֤ȷ��֡
					{
						g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
						
						WaitTime = 10; //��һ�εȴ�10��
						
						//��Լ��ٶȴ������������õ� 
						if(g_uAppDataType == 0x03)
						{
                                                    WaitTime = 2; //��һ�εȴ�3��
                                                    MaxWaitTime = 5;//���ȴ�ʱ��5��
                                                    WaitTimeSlice = 1;//�ȴ�����ʱ�䵥Ԫ 1�� 
	                                        }
						
						while( trytimes-- > 0 )
						{	
                                                        Sleep_Mode3_ACLKs(WaitTime,RF_STANDBY); 
                                                        /*
                                                        //��ƵоƬready
                                                        SetRFReady();                                                        
                                                        SetTimer0A0_ACLK();
                                                        Timer0A0_Delay(PHY_ACLK);	
                                                        while(g_uPHYTimer0A0Count<WaitTime); //�ӳ�ʱ��WaitTime
                                                        StopTimer0A0(); 
                                                        */

							WaitTime = (WaitTimeSlice*netlev) < MaxWaitTime ? (WaitTimeSlice*netlev) : MaxWaitTime;//�ȴ���֤��ʱ�� ��λ ��
							
							g_uDLLCATime = DLL_TR_TIME;
							 
							for(char count=0; count<5; count++)  //����5����ռ�ŵ�
                                                        {                                                                                                                 
                                                            if(0 == DiffChDetect(uchFreNo)) //Ƶ��̽��
                                                            {
                                                                    PageNo = GenerateApplyLogFrame(FirstDestID);
                                                                    RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
                                                                    returnVal = GetWirelessPacket(60);
                                                                    
                                                                    //��ƵоƬready
                                                                    //SetRFReady();
                                                                    
                                                                    if(returnVal == 0)
                                                                    {								        
                                                                            if(g_uchWirelessRxBuf[12] == 0x32) //�յ�ͨ����֤�ظ�
                                                                            {       
                                                                                  if(g_uchWirelessRxBuf[11] == 0x50)
                                                                                  {
                                                                                       //���ע����ʱ���ж�
                                                                                        if (( g_uchWirelessRxBuf[23] <= 254)  && (g_uchWirelessRxBuf[23] >0) )
                                                                                            g_uAppDataInt=g_uchWirelessRxBuf[23]*60; //���������ϱ����ʱ��
                                                                                        else
                                                                                            g_uAppDataInt= 21600;//��Сʱ                                                                                        
                                                
                                                                                         //���ע����ʱ���ж�
                                                                                        if (( g_uchWirelessRxBuf[24] < 60)  && (g_uchWirelessRxBuf[24] >0) )
                                                                                            g_uDLLLogInt=g_uchWirelessRxBuf[24]*60; //������·��ע���� 
                                                                                        else
                                                                                            g_uDLLLogInt= 180;//3����		
                                                                                  }
                                                                                  if(g_uchWirelessRxBuf[11] == 0x55)
                                                                                  {
                                                                                       //���ע����ʱ���ж�
                                                                                        if (( g_uchWirelessRxBuf[24] <=254)  && (g_uchWirelessRxBuf[24] >0) )
                                                                                            g_uAppDataInt=g_uchWirelessRxBuf[24]*60; //���������ϱ����ʱ��
                                                                                        else
                                                                                            g_uAppDataInt= 21600;//��Сʱ
                                                                                            
                                                                                         //���ע����ʱ���ж�
                                                                                        if (( g_uchWirelessRxBuf[25] < 60)  && (g_uchWirelessRxBuf[25] >0) )
                                                                                            g_uDLLLogInt=g_uchWirelessRxBuf[25]*60; //������·��ע���� 
                                                                                        else
                                                                                            g_uDLLLogInt= 180;//3����
                                                                                  }
                                                                                      
                                                                                   g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
                                                                                    
                                                                                    return(0);
                                                                            }
                                                                            
                                                                            if(g_uchWirelessRxBuf[12] == 0x31) //�ȴ���֤ȷ��֡
                                                                            {
                                                                                g_uchDLLFrameNum = (g_uchDLLFrameNum)%255+1;
                                                                                break;
                                                                            }
                                                                            
                                                                            if(g_uchWirelessRxBuf[12] == 0x33) //�յ���֤ʧ�ܻظ�
                                                                                    return(-2);
                                                                                    
                                                                             if((g_uchWirelessRxBuf[12] == 0xa0)||(g_uchWirelessRxBuf[12] == 0xa2))//�ܾ����� �� ����æ
                                                                            {
                                                                                    return(-1);
                                                                            }
                                                                    }//end if returnVal == 0                                                                    
                                                            }//end if Ƶ��̽��

                                                            Sleep_Mode3_ACLKms(g_uDLLCATime,RF_STANDBY); 
                                                            /*
                                                              //��ƵоƬready
                                                              SetRFReady();
                                                              SetTimer0A0_ACLK();
                                                              Timer0A0_Delay(PHY_ACLK_1MS);	
                                                              while(g_uPHYTimer0A0Count<g_uDLLCATime); //�ӳ�ʱ��g_uDLLCATime
                                                              StopTimer0A0(); 
                                                            */
                                                                
                                                            g_uDLLCATime = ( unsigned int )( (1.5*g_uDLLCATime) < (10*DLL_TR_TIME) ? (1.5*g_uDLLCATime):(10*DLL_TR_TIME)); //�޸ı����ӳ�ʱ��
                                                                                                                        
                                                         }//end for ����5����ռ�ŵ�
							
						}//end while trytimes
                                                
                                                //��ƵоƬready
                                                //SetRFReady();
						
						return -1; //û��ע���� 
					}//end if �ȴ���֤ȷ��֡
					
				}//end if �յ�ȷ�ϻظ�
			}//end if �յ��ظ�
		}// end if Ƶ��̽��

                Sleep_Mode3_ACLKms(g_uDLLCATime,RF_STANDBY);
                /*
                //��ƵоƬready
                SetRFReady();
                SetTimer0A0_ACLK();
                Timer0A0_Delay(PHY_ACLK_1MS);	
                while(g_uPHYTimer0A0Count<g_uDLLCATime); //�ӳ�ʱ��g_uDLLCATime
                StopTimer0A0(); 
                    */            

                g_uDLLCATime = ( unsigned int ) ((1.5*g_uDLLCATime) < (10*DLL_TR_TIME) ? (1.5*g_uDLLCATime):(10*DLL_TR_TIME)); //�޸ı����ӳ�ʱ��

		
	}//end for ѭ������ 
        
        //��ƵоƬready
        //SetRFReady();
	return(-1);
}

/*****************************************************************************/
/*       �豸��·�������� ,�����������·����                               */
/* ����������reflq----�ο���·����ֵ�����ϼ�������·����                    
rssi-----ͨѶ��rssi���ֵ���ǹ���ֵ                               
netcap---�ϼ��豸��ǰע����豸���� 
stab���������������ȶ�ֵ                                        */
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
	
	if(netcap <= NET_MAX_CAP/4)  //�豸��ǰ�����ֱ���¼��豸����  ��   ���ɽ�����¼��豸�������
		CQ = 100;
	else if((netcap > NET_MAX_CAP/4) && (netcap < NET_MAX_CAP/2))
		CQ = 95;
	else if((netcap >= NET_MAX_CAP/2) && (netcap < (3*NET_MAX_CAP)/4))
		CQ = 90;
	else if((netcap >= (3*NET_MAX_CAP)/4) && (netcap < NET_MAX_CAP)) 
		CQ = 80;
	else
		CQ = 10;
	
	if(stab >= 10)   //�� ����ͨѶʱ�������ȶ�ϵ�� �� ���������ȶ�����ֵ  Ϊ����
		SQ = 100;
	else if(stab >= 6 && stab <10)
		SQ = 80;
	else
		SQ = 20;
		
	//g_uchDLLCurLQ = (unsigned char)((float)(PQ+SQ+CQ)/300 * (reflq-1));    
      g_uchDLLCurLQ = (unsigned char)((float)(PQ+SQ+CQ)/300 * (reflq-5));
}

