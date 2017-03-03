/******************************************************************************/
/*            Ӧ��Ŀ��壺SI4463 Bϵ����Ƶ��                                  */
/*            ����������SI4463��Ƶģ�����������Դ�ļ�                        */
/*            ���ߣ��ܿ� ,�Ϻ��ڶ���ҵ��ѧ                                    */
/*            �汾�ţ�V1.0.0                                                  */
/*            ���ڣ�2014.08.20                                                */
/*            �޸��ߣ�                                                        */
/*            �޸����ڣ�                                                      */
/******************************************************************************/
#include "PHYlib.h"
#include "RFlib.h"
#include "global.h"

unsigned char g_uchrRFMode=RF_Standby;    //����Ĭ��ģʽ:standby

//#define LengthFieldInTwoByte   //���������ֽڱ�ʾ����֡����
//Ƶ�����е���ض���
unsigned char g_chRFFreList[RF_FRE_AMOUNT] = {0};           //Ƶ����������
//unsigned char g_uchRFSelfFreNum =0xff ;                     //��Ƶ�����ȫ�ֱ��������嵱ǰ�Լ�����Ƶ����
unsigned char g_uchRFSelfFreNum=0x00;          //����Ĭ��Ƶ����:0x00
unsigned char g_uchRFCurFreNum = 0x00;   

const unsigned char g_uchRFPowerVal[MAX_RF_PA_SET]={0x00,0x01,0x02,0x03,
0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,
0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,
0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,
0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f};
unsigned char g_uchRFPALev=MAX_RF_PA_SET-1;

unsigned char g_uchRxPAtype = 0;    //��ʾ���ͺͽ����豸�Ĺ������� 0x0A --A���豸 �޹��ţ�0x0B --B���豸 �й��ţ� 0x00 --δ֪

unsigned char g_uchRSSI = 0;//���������ź�RSSIֵ


/******************************
RF���ƹܽ�״̬��ʼ��
******************************/
void RFCtlLineInit()
{
  //GPIO2���뷽��
  SET_GPIO2_IN;//GPIO2���뷽��
  EN_GPIO2_RES;//GPIO2��/��������ʹ��
  GPIO2_OUT_L; //GPIO2��������
  
  //SDN
  DIS_SDN_RES;//SDN��Ҫ����/��������
  SET_SDN_OUT;//����SDNΪ���  
  SDN_OUT_H;//SDN����ߵ�ƽ RF����power down
  
  //SCLK
  DIS_SCLK_RES;//SCLK��Ҫ����/��������
  SET_SCLK_OUT;//����SCLKΪ��� 
  SCLK_OUT_L;
  
  //SDI
  DIS_SDI_RES;//SDI��Ҫ����/��������
  SET_SDI_OUT;//����SDIΪ���
  SDI_OUT_L;
  //SDO
  SDO_OUT_L;//SDO����͵�ƽ
  EN_SDO_RES;//SDOҪ��������  
  SET_SDO_IN;//����SDOΪ����
  
  //NSEL
  DIS_NSEL_RES;//NSEL��Ҫ����/��������
  SET_NSEL_OUT;//����NSELΪ��� 
  NSEL_OUT_H;
  
  //NIRQ  �����������룬�ر��ж�
  NIRQ_OUT_H;//NIRQ����ߵ�ƽ 
  EN_NIRQ_RES;//NIRQ����������  
 //DIS_NIRQ_RES;//NIRQ������/��������  
  SET_NIRQ_IN;//�ٶ���NIRQΪ����
  SET_NIRQ_IES;//�����ɸߵ����ж�
  CLR_NIRQ_IFG;//����жϱ��
 //EINT_NIRQ;//�����ж�
  DINT_NIRQ;  //�жϹر�
}
/**************************************
����һ���ֽڵ�����
�βΣ�Data��Ҫ���͵�����
����ֵ����
****************************************/
/*
void  API_SendDataByte(uchar Data)   //�Ӻ���
{
     unsigned char i; 

    for(i=0;i<8;i++)
    { 
        if(Data&0x80)
        {
          SDI_OUT_H;
        }
        else
        {
          SDI_OUT_L;
        }
        _NOP();_NOP();_NOP();_NOP();
        SCLK_OUT_H;   
        _NOP();_NOP();_NOP();_NOP(); 
        SCLK_OUT_L;   
        _NOP();_NOP();_NOP();_NOP();  
        Data<<=1;
   }
}
*/

void  API_SendDataByte(uchar Data)   //�Ӻ���
{
     unsigned char i; 

    for(i=0;i<8;i++)
    { 
        if(Data&0x80)
        {
          SDI_OUT_H;
        }
        else
        {
          SDI_OUT_L;
        }
        //_NOP();_NOP();_NOP();_NOP();
        SCLK_OUT_H;   
        //_NOP();_NOP();_NOP();_NOP(); 
        SCLK_OUT_L;   
       // _NOP();_NOP();_NOP();_NOP();  
        Data<<=1;
   }
}

/***************************************
���Ͷ���ֽڵ����ݣ�����ȡ����ֵ
�βΣ�DataLength�������ݳ���
      *Data������ͷָ��
����ֵ����
***************************************/
/*
void API_SendDataNoResponse(uchar DataLength,uchar *Data)   //�Ӻ���
{
   unsigned char i;
   unsigned char data_in;
   while(DataLength>0)
   {      
         data_in=*Data;
         for(i=0;i<8;i++)
         {
             if(data_in&0x80)
            {
               SDI_OUT_H; 
            }
             else
            {
               SDI_OUT_L;
            }
            _NOP();_NOP();_NOP();_NOP();_NOP();
            SCLK_OUT_H;
            _NOP();_NOP();_NOP();_NOP();_NOP();
            SCLK_OUT_L; 
            _NOP();_NOP();_NOP();_NOP();_NOP(); 
            data_in<<=1;   
        }
        Data++;
        DataLength--;  
   }
}
*/

void API_SendDataNoResponse(uchar DataLength,uchar *Data)   //�Ӻ���
{
   unsigned char i;
   unsigned char data_in;
   while(DataLength>0)
   {      
         data_in=*Data;
         for(i=0;i<8;i++)
         {
             if(data_in&0x80)
            {
               SDI_OUT_H; 
            }
             else
            {
               SDI_OUT_L;
            }
          //  _NOP();_NOP();_NOP();_NOP();_NOP();
            SCLK_OUT_H;
            //_NOP();_NOP();_NOP();_NOP();_NOP();
            SCLK_OUT_L; 
            //_NOP();_NOP();_NOP();_NOP();_NOP(); 
            data_in<<=1;   
        }
        Data++;
        DataLength--;  
   }
}

/************************************************
�������ݺ�õ�����ֵ�������ǻ�ȡ����ֽڵĺ���
�βΣ�DataLength����ֵ�ĳ���
      *Data�洢����ֵ�����ͷָ��
����ֵ����
************************************************/
/*
void API_SendDataGetResponse(uchar DataLength,uchar *Data)   //�Ӻ���
{
  unsigned char i;
  unsigned char data_out=0x00;
  while(DataLength>0)
  { 
    for(i=0;i<8;i++)
    {
      data_out<<=1;
      _NOP();_NOP();_NOP();_NOP();
      if(GET_SDO_BIT)     //������ص��Ǹߵ�ƽ
      {
        data_out=data_out|0x01;
      }
      SCLK_OUT_H;
      _NOP();_NOP();_NOP();_NOP();_NOP(); 
      SCLK_OUT_L;  
      _NOP();_NOP();_NOP();_NOP();_NOP(); 
    }
      *Data=data_out;
      data_out=0x00;
      Data++;
      DataLength--;
  }  
}
*/
void API_SendDataGetResponse(uchar DataLength,uchar *Data)   //�Ӻ���
{
  unsigned char i;
  unsigned char data_out=0x00;
  while(DataLength>0)
  { 
    for(i=0;i<8;i++)
    {
      data_out<<=1;
      //_NOP();_NOP();_NOP();_NOP();
      if(GET_SDO_BIT)     //������ص��Ǹߵ�ƽ
      {
        data_out=data_out|0x01;
      }
      SCLK_OUT_H;
      //_NOP();_NOP();_NOP();_NOP();_NOP(); 
      SCLK_OUT_L;  
     // _NOP();_NOP();_NOP();_NOP();_NOP(); 
    }
      *Data=data_out;
      data_out=0x00;
      Data++;
      DataLength--;
  }  
}
/*************************************************
���������
�βΣ�CmdLength����ĳ���
      *CmdData���������ͷָ��
����ֵ��0
**************************************************/
uchar API_SendCommand(uchar CmdLength,uchar *CmdData)
{  
    NSEL_OUT_L;        //NSEL�͵�ƽ��ʼ��ͨѶ�ӿ�
    API_SendDataNoResponse(CmdLength,CmdData);
    NSEL_OUT_H;
    return 0x00;
}

/***************************************************
����ɷ�������󣬵õ����������õ��ķ���ֵ
�βΣ�DataLength�������ݵĳ���
     *Data�洢�������ݵ�����ͷָ��
����ֵ��0����ȡ�������ݳɹ�
        1����ȡ��������ʧ��
 ****************************************************/
uchar API_GetResponse(uchar DataLength,uchar *Data)
{
     unsigned char CTS_value=0;     //���ص�CTS
     unsigned int  CTS_err=0;       //��ȡCTS����Ĵ���
     while(CTS_value!=0xff)    //�ж��Ƿ��������ж�CTS
    {
      NSEL_OUT_L;
      API_SendDataByte(CMD_READ_CMD_BUFF);   //����READ_CMD_BUFFER����
      API_SendDataGetResponse(1,&CTS_value);
      if(CTS_value!=0xff)
        {
          CTS_value=0;
          NSEL_OUT_H;
        }
      if(++CTS_err>=MAX_CTS_RETRY)
        {
         return 1;
        }
    }
     API_SendDataGetResponse(DataLength,Data); //CTS��ɺ���Զ�ȡBUFFER��ֵ
     NSEL_OUT_H;
     return 0;
}
/***********************************************************
�ȴ���������ɣ��õ���ȷ��CTS
����ֵ��0:��������ɹ�
        1:��������ʧ��
*************************************************************/
uchar API_WaitforCTS(void)
{
     unsigned char CTS_value=0;
     unsigned int  CTS_err=0;
     while(CTS_value!=0xff)
     {
        NSEL_OUT_L;
        API_SendDataByte(CMD_READ_CMD_BUFF);
        API_SendDataGetResponse(1,&CTS_value);
        NSEL_OUT_H;
        if(++CTS_err>=MAX_CTS_RETRY)
           { 
             CTS_value=0x00;
             return 1;
           }
     }
         return 0; 
}   
/***************************************************************
��ȡ�жϱ�־��������жϱ�־��ֵ
��ȡ����PH.MODEM.CHIP�����жϵı�־
�βΣ�IntStatusLength������Ļظ����ĳ���
      *IntStatus���洢�ظ����ݵ�����ָ��
����ֵ��0x00:��ȡ����ֵ���ݳɹ�
        0x01:��ȡ����ֵ����ʧ��
****************************************************************/
uchar API_Get_All_IntStatus(uchar IntStatusLength,uchar *IntStatus)
{
  unsigned char API_WRITER[4];
  API_WRITER[0]=CMD_GET_INT_STATUS;
  API_WRITER[1]=0x00;
  API_WRITER[2]=0x00;
  API_WRITER[3]=0x00;
  API_SendCommand(0x04,API_WRITER);
  if(API_GetResponse(IntStatusLength,IntStatus)==0)
      return 0x00;
  return 0x01; 
}
/***************************************************************
������ݰ������״̬��־λ
�βΣ���
����ֵ����
***************************************************************/
 void API_ClearPHInt()
 {
   unsigned char API_WRITER[2];
  API_WRITER[0]=CMD_GET_INT_STATUS;
  API_WRITER[1]=0x00;
  API_SendCommand(0x02,API_WRITER);
  //��ʱ3���룬�ȴ�NIRQ��־λ�����
  /**ע��***/
  //����ʱ��������ɾ����ɾ�����ܻᵼ���������ݶ�ȡ�Ĵ���
  PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();
 }
/****************************************************************
��ȡ���ݰ�������ж�״̬,���ܻ����״̬��־λ
�βΣ�PHIntStatusLength������״̬���ݵĳ���
      *PHIntStatus���洢�������������ͷָ��
����ֵ��0x00:��ȡ����ֵ���ݳɹ�
        0x01:��ȡ����ֵ����ʧ��
****************************************************************/
uchar API_Get_PH_IntStatus(uchar PHIntStatusLength,uchar *PHIntStatus)
{
  unsigned char API_Writer=CMD_GET_PH_STATUS;
  API_SendCommand(0x01,&API_Writer);
  if(API_GetResponse(PHIntStatusLength,PHIntStatus)==0)
    return 0x00;
  return 0x01;
}

/****************************************************************
��ȡ����״̬���ж�״̬�����ܻ����״̬��־λ
�βΣ�ModemIntStatusLength������״̬���ݵĳ���
      *ModemIntStatus���洢�������������ͷָ��
����ֵ��0x00:��ȡ����ֵ���ݳɹ�
        0x01:��ȡ����ֵ����ʧ��
****************************************************************/
uchar API_Get_Modem_IntStatus(uchar ModemIntStatusLength,uchar *ModemIntStatus)
{
  unsigned char API_Writer=CMD_GET_MODEM_STATUS;
  API_SendCommand(0x01,&API_Writer);
  if(API_GetResponse(ModemIntStatusLength,ModemIntStatus)==0)
    return 0x00;
  return 0x01;
}

/****************************************************************
��ȡоƬ״̬���ж�״̬�����ܻ����״̬��־λ
�β�:ChipIntStatusLength������״̬���ݵĳ���
      *ChipIntStatus���洢�������������ͷָ��
����ֵ��0x00:��ȡ����ֵ���ݳɹ�
        0x01:��ȡ����ֵ����ʧ��
****************************************************************/
uchar  API_Get_Chip_IntStatus(uchar ChipIntStatusLength,uchar *ChipIntStatus)
{
  unsigned char API_Writer=CMD_GET_CHIP_STATUS;
  API_SendCommand(0x01,&API_Writer);
  if(API_GetResponse(ChipIntStatusLength,ChipIntStatus)==0)
    return 0x00;
  return 0x01;
}
/*************************************************************
��ȡоƬ�Ļ�����Ϣ
�βΣ�PARTInfoLength:����оƬ�������ݵĳ���
      *PARTInfo���洢�������������ͷָ��
����ֵ��0x00:��ȡ�豸��Ϣ����
        0x01:��ȡ�豸��Ϣʧ��
*************************************************************/
uchar API_PART_INFO(uchar PARTInfoLength,uchar *PARTInfo)
{
  unsigned char API_WRITER=CMD_PART_INFO; 
  API_SendCommand(0x01,&API_WRITER);
  if(API_GetResponse(PARTInfoLength,PARTInfo)==0)
    return 0x00;
  return 0x01;
}
/*************************************************************
��TXFIFO��д�����ݣ�һ����������д��64�ֽ�
�βΣ�TxFifoLenght��д�����ݵĳ���
      *TxFifoData���洢FIFO���������ͷָ��
����ֵ��0
**************************************************************/
uchar API_WriteTxDataBuffer(uchar TxFifoLenght,uchar *TxFifoData)
{
  NSEL_OUT_L;
  API_SendDataByte(0x66);
  API_SendDataNoResponse(TxFifoLenght,TxFifoData);
  NSEL_OUT_H;
  return 0;
}
/***************************************************************
��RXFIFO�ж�ȡ����
�βΣ�RxFifoLength����ȡ���ݵĳ���
      *RxFifoData���洢RXFIFO�����������ͷָ��
����ֵ��0
***************************************************************/
uchar API_ReadRxDataBuffer(uchar RxFifoLength,uchar *RxFifoData)
{
  NSEL_OUT_L;
  API_SendDataByte(0x77);
  API_SendDataGetResponse(RxFifoLength,RxFifoData);
  NSEL_OUT_H;
  return 0x00;
}
/**************************************************************
���TXFIFO
�βΣ���
����ֵ��TXFIFOʣ��Ŀռ�
        0xff:���TXFIFOʧ��
***************************************************************/
uchar API_ClearTxFIFO(void)
{
  unsigned char API_WRITER[2];
  unsigned char res[2];
  API_WRITER[0]=CMD_FIFO_INFO;
  API_WRITER[1]=0x01;
  API_SendCommand(2,API_WRITER);
  if(API_GetResponse(0x02,res)==0)
     return res[1];
  else
  {
    return 0xff;
  }
}
uchar RFResetTxFIFO()   //������·�㺯��
{
  unsigned char Ret=API_ClearTxFIFO();
  return Ret;
}
/*************************************************************
���RXFIFO
�βΣ���
����ֵ��RXFIFO��ʹ�ÿռ�
        0xff:RXFIFO���ʧ��
**************************************************************/
uchar API_ClearRxFIFO(void)
{
  unsigned char API_WRITER[2];
  unsigned char ReturnValue[2];
  API_WRITER[0]=CMD_FIFO_INFO;
  API_WRITER[1]=0x02;
  API_SendCommand(2,API_WRITER);
  if(API_GetResponse(0x02,ReturnValue)==0)
  return ReturnValue[0];
  else
  {
    return 0xff;
  }
}
uchar RFResetRxFIFO()   //������·�㺯��
{
  unsigned char Ret=API_ClearRxFIFO();
  return Ret;
}
/*********************************************************
��ȡ���ٷ�Ӧ�Ĵ�����Ĵ洢ֵ
�βΣ�bStartReg���Ĵ����Ŀ�ʼ��ַ
      bNumberOfReg����ȡ�Ĵ����ĸ���
      RegValues�� �洢�Ĵ����Ķ�ά����ͷָ��
����ֵ��0x01:���ݶ�ȡ���󣨶�ȡ�Ĵ����������ߵ�ַ����
        0x00:��ȡ���ݳɹ�
**********************************************************/
uchar API_GetFastResponseRegister(uchar bStartReg,uchar bNumberOfReg,uchar *RegValues)
{
   if (bNumberOfReg<=0||bNumberOfReg>4)   //�ж��Ƿ��ȡ�ĸ�������
  {
    return 0x01;
  }
  if (bStartReg==0x50||bStartReg==0x51||bStartReg==0x53||bStartReg==0x57) //�ж�����������Ƿ��Ƕ�ȡ�����Ĵ���������
  {
        NSEL_OUT_L;
        API_SendDataByte(bStartReg);
        API_SendDataGetResponse(bNumberOfReg,RegValues);
        NSEL_OUT_H;
        return 0;
  }
  return 0x01;
}
/*****************************************************************
����FIFO��ʣ��ռ䣬������FIFO
�βΣ��洢ʣ��ռ�Ķ�ά����ͷָ��
����ֵ����
*****************************************************************/
 void API_FIFO_INFO(uchar *FIFO_INFO)
 {
   unsigned char API_WRITER[2];
   API_WRITER[0]=CMD_FIFO_INFO;
   API_WRITER[1]=0x00;
   API_SendCommand(2,API_WRITER);
   API_GetResponse(2,FIFO_INFO);
 }
 
 /***************************************************************
 RFͨ��IO�ܽ�����
�βΣ�GPIO0��GPIO0���÷�ʽ
      GPIO1��GPIO1���÷�ʽ
      GPIO2��GPIO2���÷�ʽ
      GPIO3: GPIO3���÷�ʽ
 ����ֵ����
�ο���GPIO0��0X20��GPIO1��0X21
 ***************************************************************/
 void API_GPIO_CFG(uchar GPIO0,uchar GPIO1,uchar GPIO2,uchar GPIO3)
 {
   unsigned char API_WRITER[8];
   API_WRITER[0]=CMD_GPIO_PIN_CFG; API_WRITER[1]=GPIO0;
   API_WRITER[2]=GPIO1;            API_WRITER[3]=GPIO2;
   API_WRITER[4]=GPIO3;             API_WRITER[5]=0x00;
   API_WRITER[6]=0x00;             API_WRITER[7]=0x00; 
   API_SendCommand(0x08,API_WRITER);
   //API_WaitforCTS();
 }
 
/**********************************************************
�ϵ�оƬ��ʼ��,��RFоƬ�󣬳�ʼ��RF
�βΣ���
����ֵ����
**********************************************************/
void API_POWER_UP()
{
  unsigned char API_WRITER[7];
  API_WRITER[0]=CMD_POWER_UP; API_WRITER[1]=0x01;
  API_WRITER[2]=0x00;         API_WRITER[3]=0x01;
  API_WRITER[4]=0xc9;         API_WRITER[5]=0xc3;
  API_WRITER[6]=0x80;         
  API_SendCommand(0x07,API_WRITER);
  API_WaitforCTS();
  PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();//��ʱ6����
  PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();
}

/**********************************************************************
�Ĵ��������ã���ͬһ����Ķ���Ĵ���������������
�βΣ�group���Ĵ�����������
      num_pro����Ҫ�������üĴ����ĸ���
     start_pro�����üĴ�����ĳ�ʼ��ַ
     *Data���Ĵ������õ�����
����ֵ��1��һ�������õļĴ������࣬����ʧ��
        0��һ�������üĴ����ɹ�
************************************************************************/
uchar API_SET_PROPERTY_X(unsigned char group,unsigned char num_pro,unsigned char start_pro,unsigned char *Data)  //unsigned char *Data
{
  uchar API_WRITER[20];       
  uchar m=4;         //��ʼ�Ĵ������ݵ������±�
  uchar num=num_pro; //��Ҫ���õļĴ����ĸ���
  if(num_pro>12)
      {
        return 0x01;
      }
  else
     {
        API_WRITER[0]=CMD_SET_PROPERTY; 
        API_WRITER[1]=group;
        API_WRITER[2]=num_pro;
        API_WRITER[3]=start_pro;
        
        while(num>0)
         {
            API_WRITER[m]=*Data;
            Data++;
            m++;
            num--; 
         }
         API_SendCommand(num_pro+4,API_WRITER);
         if(API_WaitforCTS())     //CTS����ֵ����
           return 0x01;
         return 0x00;
     }
}
/**********************************************************************
��һ���Ĵ�����������
�βΣ�group:�üĴ�����������
      address���üĴ����ĵ�ַ
      Data���ԸüĴ������õ�����
����ֵ��0���ԼĴ��������óɹ�
        1���ԼĴ���������ʧ��
**********************************************************************/
 uchar API_SET_PROPERTY_1(unsigned char group,unsigned char address,unsigned char Data)
 {
    uchar API_WRITER[5];
    API_WRITER[0]=CMD_SET_PROPERTY; 
    API_WRITER[1]=group;
    API_WRITER[2]=0X01;
    API_WRITER[3]=address;
    API_WRITER[4]=Data;
    API_SendCommand(0x05,API_WRITER);
    if(API_WaitforCTS())
      return 0x01;
    return 0x00;
 }
/*****************************************************************
��ȡ�Ĵ��������õ�ֵ
******************************************************************/
uchar API_GET_PROPERTY_1(unsigned char group,unsigned char start_pro)
{
    uchar API_PROPERTY;
    uchar API_WRITER[4];
    API_WRITER[0]=CMD_GET_PROPERTY; 
    API_WRITER[1]=group;
    API_WRITER[2]=0x01;
    API_WRITER[3]=start_pro;
    API_SendCommand(0x04,API_WRITER);
    API_GetResponse(0x01,&API_PROPERTY);
    return API_PROPERTY;
}
/******************************************************************
�ı�оƬ������״̬
�βΣ�RFStatus��RF��Ҫ�����״̬
����ֵ��0x01:״̬�ı䲻�ɹ�
        0x00:״̬�ı�ɹ�
******************************************************************/
uchar API_CHANGE_STATE(uchar RFStatus)       //ʹ�øô������ʱ��Ƶģ����뵽��Ӧ��״̬
{
   uchar API_WRITER[2];
   API_WRITER[0]=CMD_CHANGE_STATE;
   API_WRITER[1]=RFStatus;
   API_SendCommand(0x02,API_WRITER);
   if(API_WaitforCTS())
     return 0x01;
   return 0x00;
} 
/*========================================
�����õ�ǰоƬ������״̬
�βΣ��洢RF״̬������ͷָ��
����ֵ��0x00:��ȡ����ֵ�ɹ� 
        0x01:��ȡ����ֵʧ��
0:no change /// 1:sleep state /// 2:spi active state
3:ready state /// 4:another enumeration for ready state
5:tune state for tx /// 6:tune state for rx
7:tx state /// 8:rx state
========================================*/
uchar API_REQUEST_DEVICE_STATE(uchar *RFStatus)
{
  uchar cmd=CMD_REQUEST_DEVICE_STATE;
  API_SendCommand(0x01,&cmd);
  if(API_GetResponse(0x02,RFStatus)==0)    //��ȡ����ֵ�ɹ�
    return 0x00;
  return 0x01;
}
/**************************************************************/
/*�趨RF���뵽Power Downģʽ                                  */
/***************************************************************/
void SetRFOff(void)
{
  SDN_OUT_H;  //SDN�ܽŸߵ�ƽ��RF���뵽ShutDownģʽ
  
  g_uchrRFMode=RF_Shutdown;
}

/***************************************************************/
/*�趨RF����                                                   */
/***************************************************************/
 void SetRFOn(void)
 {
   SDN_OUT_L;    //SDN�ܽŵ͵�ƽ��RF����
   
   //��shutdownģʽ�µ�TX/RXģʽ������15ms�����ٵȴ�15ms
   int i=40;   //��ʱԼ40ms
   while(i--)
   PHYMCLKSoftDelay1ms();
   
   g_uchrRFMode=RF_Standby;
 }
/***************************************************************
Ӳ������RF Reset RF
****************************************************************/
 void HWResetRF()
{
  uchar IntStatus[8];
  uchar DeviceInfo[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  RFCtlLineInit(); //RF�����߳�ʼ��
  
  SetRFOff();  //�ر�RF  �ر�֮�����ٵȴ�10ns
  PHYMCLKSoftDelay1ms();   //��ʱ1ms
  SetRFOn(); //RF����
  API_PART_INFO(0x08,DeviceInfo);  //��ȡ�豸��Ϣ
  if(DeviceInfo[1]!=0x44||DeviceInfo[2]!=0x63)  //�ж��豸�Ķ�ȡ��Ϣ�Ƿ���ȷ
    {
      int i=200;
      while(1)
      {
        LedRedOn();
        i=200;
        while(--i)
          PHYMCLKSoftDelay1ms();   //��ʱ1ms
        LedRedOff();
        i=200;
        while(--i)
          PHYMCLKSoftDelay1ms();   //��ʱ1ms        
      }
    }
  API_Get_All_IntStatus(0x08,IntStatus);//��ȡ�жϱ�־������жϱ�־λ
}

 /****************************************************************
��Ƶ������д����Ӧ�Ĳ�������Ƶ����Ӧ�ģ�Ƶ���ţ���Ƶ��ɺ���뵽
����״̬
����Ƶ�㣺��������������
****************************************************************/
 void FrequencyHop(unsigned char RFFreNum)
 {
   if(RFFreNum>(RF_FRE_AMOUNT-1))   //����Ƶ���Ŵ������Ƶ���ţ����������Ƶ����
        RFFreNum=RF_FRE_AMOUNT-1;
   if(RFFreNum!=g_uchRFCurFreNum)
   {
     g_uchRFCurFreNum = RFFreNum;       //��¼���������Ĺ���Ƶ��
     SetRFRx();                         //�����Ƶ�������뵽����ģʽ
   }
 }

/***************************************************************/
/*�趨RF����Standbyģʽ                                        */
//Standby��Sleepģʽ�޷�ʹ��REQUEST_DEVICE_STATE����õ�
/***************************************************************/
void SetRFStandby(void)
{
  API_CHANGE_STATE(0x01);    //sleep state
  g_uchrRFMode = RF_Sleep;
}

/***************************************************************/
/*�趨RF����Sleepģʽ    �ĵ���3                               */
/****************************************************************/
void SetRFSleep()
{
  API_CHANGE_STATE(0x01);  //sleep state
  g_uchrRFMode = RF_Sleep;
}


/***************************************************************/
/*�趨RF����Readyģʽ                                         */
/****************************************************************/
void SetRFReady(void)
{
   uchar RFStatus[2]; //�洢�����豸״̬�ķ�������
   API_CHANGE_STATE(0x03);
   if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //��ȡ�������ݳɹ�
   {
     if(RFStatus[0]==0x03)
     {
       g_uchrRFMode=RF_Ready;     //��¼�����豸����Readyģʽ
     }
   } 
}

/***************************************************************/
/*�趨RF����Txģʽ�µ�TUNEģʽ                                         */
/****************************************************************/
void SetRFTuneForTx(void)
{
  uchar RFStatus[2]; //�洢�����豸״̬�ķ�������
   API_CHANGE_STATE(0x05); 
   if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //��ȡ�������ݳɹ�
   {
     if(RFStatus[0]==0x03)
     {
       g_uchrRFMode=RF_TxTune;     //��¼�����豸����TX_TUNEģʽ
     }
   } 
}
/***************************************************************/
/*�趨RF����Rxģʽ�µ�TUNEģʽ                                         */
/****************************************************************/
void SetRFTuneForRx(void)
{
   uchar RFStatus[2]; //�洢�����豸״̬�ķ�������
   API_CHANGE_STATE(0x06); 
   if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //��ȡ�������ݳɹ�
   {
     if(RFStatus[0]==0x03)
     {
       g_uchrRFMode=RF_RxTune;     //��¼�����豸����RX_TUNEģʽ
     }
   } 
}

/***************************************************************/
/*�趨RF����TXģʽ (ע�����ݷ������֮���Զ��˳�)              */
//ע�����ߵ�����CHANGE_STATE����ķ��ͺ����޷�д��Ƶ���ţ����ͳ���
//START_TX��������д��Ƶ�����Լ����ݷ��ͳ���
/****************************************************************/
//ʹ��CHANGE_STATE����ķ��ͺ���
void SetRFStartTx(void)
{
   /*����ı�TX״̬��ʹ��CHANGE_STATE����
   ��Ϊ�޷�д��Ƶ���ŵ�����  */
    uchar RFStatus[2]; //�洢�����豸״̬�ķ�������
    API_CHANGE_STATE(0x07);
    if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //��ȡ�������ݳɹ�
   {
     if(RFStatus[0]==0x07)
     {
       g_uchrRFMode=RF_Tx;     //��¼�����豸����TXģʽ
     }
   } 
  
}
//ʹ��START_TX����ķ��ͺ���     ..Ƶ���ż��3��
void SetRFTx()    //sizeʹ��uchar���ͣ���෢�͸��ֽ���ֻ����255
{
  /****************************************/
  /*�ɴ�������Ӵ���*/
  //��ӵ����ز�ģʽ�Ĵ���
   // MODEM_MOD_TYPE(0x00);   //�ز�ģʽ
  /*�ɴ�������Ӵ���*/
  /****************************************/
  
  unsigned char RFStatus[2]; //�洢�����豸״̬�ķ�������
  unsigned char Command_Tx[5];
  Command_Tx[0]=CMD_START_TX;
  Command_Tx[1]=g_uchRFSelfFreNum*3;   //ʹ����Ƶ����ı乤��Ƶ����
  Command_Tx[2]=0x30;     //������ɺ��˳�����ģʽ
  Command_Tx[3]=0x00;
  Command_Tx[4]=0x00;
  while(1)
  {
    unsigned char trytimes=0;
    while(++trytimes<3)
    {
        API_SendCommand(0x05,Command_Tx);     //API_WRITER 
        PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();
        if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //��ȡ�������ݳɹ�
          {
              if(RFStatus[0]==0x07)
                {
                  g_uchrRFMode=RF_Tx; //��¼�����豸����TXģʽ
                  break;     
                }
          }       
    }
     if(g_uchrRFMode==RF_Tx)
       break;   
    /**********************************/
    /*�ɴ˴�������Ӵ���*/
    HWResetRF();    
    InitRFChipWorkBand();
    FrequencyHop(g_uchRFSelfFreNum);    //������ڣ�2014-11-22
    /*�ɴ˴�������Ӵ���*/
    /**********************************/
  }
}
/***************************************************************/
/*    �趨RF����RXģʽ                                         */
/*    ����ı�RX״̬ʹ��CHANGE_STATE����                     */
/*      ��Ϊ�޷�д��Ƶ���ŵ�����                               */
/***************************************************************/
//ʹ��CHANGE_STATE������뵽RX״̬
void SetRFStartRx(void)
{
     uchar RFStatus[2]; //�洢�����豸״̬�ķ�������
     API_CHANGE_STATE(0x08); 
     if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //��ȡ�������ݳɹ�
   {
     if(RFStatus[0]==0x08)
     {
       g_uchrRFMode=RF_Rx;     //��¼�����豸����RXģʽ
     }
   }  
}

//ʹ��START_RX������뵽RX״̬,�βδ�����Ҫ�޸��β�
void SetRFRx()  
{
  unsigned char RFStatus[2]; //�洢�����豸״̬�ķ�������
  //unsigned char RF_Mode=g_uchrRFMode; ��¼�ϴε�״̬���������ϴ�״̬ת����Ϊ����״̬����Ҫ��ʱ��
  unsigned char Command_Rx[7];
  Command_Rx[0]=CMD_START_RX;
  Command_Rx[1]=g_uchRFCurFreNum*3;          //���ڵ�Ƶ��
  Command_Rx[2]=0x00;
  Command_Rx[3]=0x00;
  Command_Rx[4]=0x00;
  Command_Rx[5]=0x00;
  Command_Rx[6]=0x00;
  while(1)
  {
    unsigned char trytimes=0;
    while(++trytimes<3)
    {
      /*********************************************/
      /*�ɴ�������Ӵ���*/
       INT_CTL_ENABLE(0x01);     //�������ݰ��ж�
       INT_CTL_PH_ENABLE(0x19);   //RX_Fifo�����ͽ�������ж�
      // INT_CTL_PH_ENABLE(0x18);   //RX_Fifo�����ͽ�������ж�
      /*�ɴ�������Ӵ���*/
      /********************************************/
        API_ClearRxFIFO();                  //���RxFIFO
        API_SendCommand(0x07,Command_Rx);   //API_WRITER 
        API_WaitforCTS();
        if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //��ȡ�������ݳɹ�
          {
              if(RFStatus[0]==0x08)
                {
                  g_uchrRFMode=RF_Rx; //��¼�����豸����TXģʽ
                  break;     
                }
          }       
    }
         if(g_uchrRFMode==RF_Rx)
          break;  
   HWResetRF();
   InitRFChipWorkBand();
   FrequencyHop(g_uchRFSelfFreNum);    //������ڣ�2014-11-22
   //ע����SI4432������
   //���ﲻ�����Ƶ���룬��Ƶ�����ڷ��������Լ����������������
  }   
}

/****************************************************************/
/*           ��ȡRSSIֵ����������                               */
/****************************************************************/
 unsigned char GetRSSI()
 { 
   unsigned int i=0;
   unsigned char RSSI[4];
   unsigned char count;
   unsigned char max=0;
   unsigned char min=0xff;
   //SetRFRx();     //���뵽����ģʽ
   for(count=0;count<6;count++)
   {
    //API_GetFastResponseRegister(0x51,0x01,&RSSI);    //���ٷ�Ӧ�Ĵ�����ȡ latch rssi
     API_Get_Modem_IntStatus(0x04,RSSI);
     i+=RSSI[2];
     if(RSSI[2]>max)
       max=RSSI[2];
     if(RSSI[2]<min)
       min=RSSI[2];
   }
  //SetRFReady();  //���뵽readyģʽ
   i=i-max-min;
   RSSI[2]=i/4;
   return RSSI[2];
 }
 /****************************************************
 ͨ����ȡFast response��ȡlatch rssi
 *****************************************************/
unsigned char GetFastLatchRSSI()
{
  unsigned char RSSI=0x00;
  API_GetFastResponseRegister(0x51,0x01,&RSSI);    //���ٷ�Ӧ�Ĵ�����ȡ latch rssi
  return RSSI;
}
 /************************************************************
               ����ȡ��RSSI��ֵת��ΪdBm����ʽ
 *************************************************************/
int GetRSSIPower()
{
  int Ret=GetRSSI();
  return Ret;
}
/**************************************************************
 �趨RF���뵽TX�����ز�
***************************************************************/
 void SetTxCarrier()
 {
   MODEM_MOD_TYPE(0x00);
   SetRFStartTx();
 }
//���Ƶ��470M
//����4kbps,Ƶ��ƫ��20KHz
/***********************************************************************/
/*            ������ʼ��RF��standby*/
/***********************************************************************/
void InitRFChipB470_Baud4000(void)    
{
  unsigned char API_WRITER[16];        //�����
  unsigned char IntSta[12];
  //power up
  API_POWER_UP();                          //��ͬ
  API_Get_All_IntStatus(0x08,IntSta);      //��ͬ
  
  //GPIO config
 // API_GPIO_CFG(0x20,0x21,0x20,0x21);
    API_GPIO_CFG(0x21,0x20,0x20,0x21);   //��Ҫ��������Ƶ���صķ��������趨
    
 //GLOBAL_XO_TUNE
  GLOBAL_XO_TUNE(0x50);                      //��ͬ
  
  //GLOBAL_CONFIG
  GLOBAL_CONFIG(0x60);                       //��ͬ
  
     //MODEM_CLK_BAND
  MODEM_CLKGEN_BAND(0x0A);        //��ͬ
  
    //MODEM
  //modem type
  MODEM_MOD_TYPE(0x03); //2GFSK,FIFO handler    //��ͬ
  MODEM_MAP_CONTROL(0x00);
  MODEM_DSM_CONTROL(0x07);
  
 //modem data rate         ������     //����ͬ�����Խ�������
  /*
  MODEM_DATA_RATE_2(0x0c);        //40kbps
  MODEM_DATA_RATE_1(0x35);                
  MODEM_DATA_RATE_0(0x00);
  MODEM_TX_NCO_MODE_3(0x09);                
  MODEM_TX_NCO_MODE_2(0xc9);                
  MODEM_TX_NCO_MODE_1(0xc3);                
  MODEM_TX_NCO_MODE_0(0x80);
  */
  MODEM_DATA_RATE_2(0x02);        //4kbps
  MODEM_DATA_RATE_1(0x71);                
  MODEM_DATA_RATE_0(0x00);
  MODEM_TX_NCO_MODE_3(0x05);                
  MODEM_TX_NCO_MODE_2(0xc9);                
  MODEM_TX_NCO_MODE_1(0xc3);                
  MODEM_TX_NCO_MODE_0(0x80);
  
  //modem deviation   Ƶƫ      20kHz
  MODEM_FREQ_DEV_2(0x00);    //��ͬ
  MODEM_FREQ_DEV_1(0x05);
  MODEM_FREQ_DEV_0(0x76);
  
  
    
  //RF_FREQ_CONTROL_INTE_8   //�������    Ƶ������
  FREQ_CONTROL_INTE(0x3d);     //����Ƶ��470.1MHz  ������200khz
  FREQ_CONTROL_FRAC_2(0x0d);
  FREQ_CONTROL_FRAC_1(0x70);
  FREQ_CONTROL_FRAC_0(0xa3);
  FREQ_CONTROL_CHANNEL_STEP_SIZE_1(0x36);
  FREQ_CONTROL_CHANNEL_STEP_SIZE_0(0x9d);
  FREQ_CONTROL_W_SIZE(0x20);
  FREQ_CONTROL_VCOCNT_RX_ADJ(0xfe);
  
 
  
  //INT_CTL_ENALBE
   INT_CTL_ENABLE(0x00);       //�ر����е��ж�
  
   
  //FRR_CTL_MODE
 
  FRR_CTL_A_MODE(0x04);    //���ݰ���������ж�
  FRR_CTL_B_MODE(0x0a);    //latch RSSI value as defined in MODEM_RSSI_CONTROL
  /*���ٷ�Ӧ�Ĵ���C/D�ɸ���ʵ�������������*/
  //FRR_CTL_C_MODE(0X00);
  //FRR_CTL_D_MODE(0X00);
  
  //PREAMBLE  ǰ����
  PREAMBLE_TX_LENGTH(0x0a);        //��ͬ
  PREAMBLE_CONFIG(0x31);
  /*0x14��0x0fΪ�Ĵ���Ĭ��ֵ���������ﲻ��Ҫ���ã������Ҫ��
   �Ĵ������в��ָı���޸Ĵ˴�*/
  //PREAMBLE_CONFIG_STD1(0X14);  //RXǰ�����������
  //PREAMBLE_CONFIG_STD2(0X0F);  //RX����ǰ��������
  
  //SYNC   ͬ����                 //��ͬ
  SYNC_CONFIG(0x01);   //�����ֽڵ�ͬ���֣���������˹�ر��룬����4FSK����
  SYNC_BITS_31_24(0xB4);
  SYNC_BITS_23_16(0x2B);
  
  //PKT_CRC_CONFIG  CRC����ѡ��

   PKT_CRC_CONFIG(0x85);
   PKT_CONFIG1(0x02);  //new
   PKT_TX_THRESHOLD(0x30);      //���Ϳ������ֵ  32�ֽ�
   PKT_RX_THRESHOLD(0x30);      //���ܿ�������ֵ  32��
   
#ifdef LengthFieldInTwoByte
   PKT_LEN(0x3a);               //�����ֽ����ó�Ϊ�����ֽ�
#else
   PKT_LEN(0x2a);               //�����ֽ����ó�Ϊһ���ֽ�
#endif   
   PKT_LEN_FIELD_SOURCE(0x01);            
   PKT_LEN_ADJUST(0x00);
   PKT_FIELD_1_LENGTH_7_0(0x01);
   PKT_FIELD_1_CONFIG(0x04);
   PKT_FIELD_2_LENGTH_7_0(0xff);
   PKT_FIELD_2_CONFIG(0x04);
  
   

   
   
   MODEM_RSSI_CONTROL(0x13);
  /*
  �βΣ�group���Ĵ�����������
      num_pro����Ҫ�������üĴ����ĸ���
     start_pro�����üĴ�����ĳ�ʼ��ַ
     *Data���Ĵ������õ�����
  */
  //RF_MODEM_TX_RAMP_DELAY_8         //����ͬ
   
  API_WRITER[0]=0X01;        API_WRITER[1]=0x80; //API_WRITER[1]=0x00;
  API_WRITER[2]=0x08;         API_WRITER[3]=0x03;
  API_WRITER[4]=0x80;         API_WRITER[5]=0x00;
  API_WRITER[6]=0x20;       API_WRITER[7]=0x10;//  API_WRITER[7]=0x20;
  API_SET_PROPERTY_X(Group_MODEM,0X08,ad_MODEM_TX_RAMP_DELAY,API_WRITER);
  
 //RF_MODEM_BCR_OSR_1_9      //����ͬ
 /* API_WRITER[0]=0X00;          API_WRITER[1]=0x5E;
  API_WRITER[2]=0x05;          API_WRITER[3]=0x76;
  API_WRITER[4]=0x1A;          API_WRITER[5]=0x05;
  API_WRITER[6]=0x72;          API_WRITER[7]=0x02;
  API_WRITER[8]=0X00;*/   
  API_WRITER[0]=0X02;          API_WRITER[1]=0x71;
  API_WRITER[2]=0x00;          API_WRITER[3]=0xd1;
  API_WRITER[4]=0xb7;          API_WRITER[5]=0x00;
  API_WRITER[6]=0x69;          API_WRITER[7]=0x02;
  API_WRITER[8]=0Xc2;
  API_SET_PROPERTY_X(Group_MODEM,0X09,ad_MODEM_BCR_OSR_1,API_WRITER);
  
  
  //RF_MODEM_AFC_GEAR_7       //����ͬ
  /*
  API_WRITER[0]=0X00;          API_WRITER[1]=0x12;
  API_WRITER[2]=0x81;          API_WRITER[3]=0x5E;
  API_WRITER[4]=0x01;          API_WRITER[5]=0xCD;
  API_WRITER[6]=0xa0;  
  */
  API_WRITER[0]=0X04;          API_WRITER[1]=0x36;
  API_WRITER[2]=0xc0;          API_WRITER[3]=0x0c;
  API_WRITER[4]=0x24;          API_WRITER[5]=0x31;
  API_WRITER[6]=0xc0;  
  API_SET_PROPERTY_X(Group_MODEM,0X07,ad_MODEM_AFC_GEAR,API_WRITER);
  
  //RF_MODEM_AGC_CONTROL_1
  API_WRITER[0]=0xE2;           //��ͬ
  API_SET_PROPERTY_X(Group_MODEM,0x01,ad_MODEM_AGC_CONTROL,API_WRITER);
  
  //RF_MODEM_AGC_WINDOW_SIZE_9
  /*
   API_WRITER[0]=0X11;          API_WRITER[1]=0x15;   //����ͬ
  API_WRITER[2]=0x15;          API_WRITER[3]=0x00;
  API_WRITER[4]=0x1A;          API_WRITER[5]=0x40;
  API_WRITER[6]=0x00;          API_WRITER[7]=0x00;
  API_WRITER[8]=0X28;       
*/
  API_WRITER[0]=0X11;          API_WRITER[1]=0x89;   //����ͬ
  API_WRITER[2]=0x89;          API_WRITER[3]=0x00;
  API_WRITER[4]=0x1A;          API_WRITER[5]=0xff;
  API_WRITER[6]=0xff;          API_WRITER[7]=0x00;
  API_WRITER[8]=0X2b;
  API_SET_PROPERTY_X(Group_MODEM,0X09,ad_MODEM_AGC_WINDOW_SIZE,API_WRITER);
  
  //RF_MODEM_OOK_CNT1_11
 /* API_WRITER[0]=0xA4;          API_WRITER[1]=0x03;   //����ͬ
  API_WRITER[2]=0xD6;          API_WRITER[3]=0x03;
  API_WRITER[4]=0x00;          API_WRITER[5]=0xCC;
  API_WRITER[6]=0x01;          API_WRITER[7]=0x80;
  */
  API_WRITER[0]=0xA4;          API_WRITER[1]=0x02;   //����ͬ
  API_WRITER[2]=0xD6;          API_WRITER[3]=0x83;
  API_WRITER[4]=0x01;          API_WRITER[5]=0x33;
  API_WRITER[6]=0x01;          API_WRITER[7]=0x80;
 // API_WRITER[8]=0XFF;           API_WRITER[9]=0x0C;
  //API_WRITER[10]=0x00; 
  API_SET_PROPERTY_X(Group_MODEM,0X08,ad_MODEM_OOK_CNT1,API_WRITER);
  
  
 //RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12
  /*
  API_WRITER[0]=0xA2;          API_WRITER[1]=0x81;  //��ͬ
  API_WRITER[2]=0x26;          API_WRITER[3]=0xAF;
  API_WRITER[4]=0x3F;          API_WRITER[5]=0xEE;
  API_WRITER[6]=0xC8;          API_WRITER[7]=0xC7;
  API_WRITER[8]=0xDB;          API_WRITER[9]=0xF2;
  API_WRITER[10]=0x02;         API_WRITER[11]=0x08;
  */
  API_WRITER[0]=0xff;          API_WRITER[1]=0xba;  //��ͬ
  API_WRITER[2]=0x0f;          API_WRITER[3]=0x51;
  API_WRITER[4]=0xcf;          API_WRITER[5]=0xa9;
  API_WRITER[6]=0xC9;          API_WRITER[7]=0xfc;
  API_WRITER[8]=0x1b;          API_WRITER[9]=0x1e;
  API_WRITER[10]=0x0f;         API_WRITER[11]=0x01;
  API_SET_PROPERTY_X(Group_MODEM_CHFLT,0X0C,ad_MODEM_CHFLT_RX1_CHFLT_COE_13,API_WRITER); 
  
  //RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12   //��ͬ
  /*
  API_WRITER[0]=0X07;          API_WRITER[1]=0x03;
  API_WRITER[2]=0x15;          API_WRITER[3]=0xFC;
  API_WRITER[4]=0x0F;          API_WRITER[5]=0x00;
  API_WRITER[6]=0xA2;          API_WRITER[7]=0x81;
  API_WRITER[8]=0x26;          API_WRITER[9]=0xAF;
  API_WRITER[10]=0x3F;         API_WRITER[11]=0xEE;
  */
  API_WRITER[0]=0Xfc;          API_WRITER[1]=0xfd;
  API_WRITER[2]=0x15;          API_WRITER[3]=0xff;
  API_WRITER[4]=0x00;          API_WRITER[5]=0x0f;
  API_WRITER[6]=0x5b;          API_WRITER[7]=0x47;
  API_WRITER[8]=0x0f;          API_WRITER[9]=0xc0;
  API_WRITER[10]=0x6d;         API_WRITER[11]=0x25;
  API_SET_PROPERTY_X(Group_MODEM_CHFLT,0X0C,ad_MODEM_CHFLT_RX1_CHFLT_COE_1,API_WRITER); 
  
  //RF_MODEM_RX2_CHFLT_RX2_CHFLT_COE7_7_0_12
 /* API_WRITER[0]=0XC8;          API_WRITER[1]=0xC7;   //��ͬ
  API_WRITER[2]=0xDB;          API_WRITER[3]=0xF2;
  API_WRITER[4]=0x02;          API_WRITER[5]=0x08;
  API_WRITER[6]=0x07;          API_WRITER[7]=0x03;
  API_WRITER[8]=0x15;          API_WRITER[9]=0xFC;
  API_WRITER[10]=0x0F;         API_WRITER[11]=0x00;*/
  API_WRITER[0]=0Xf4;          API_WRITER[1]=0xdb;   //��ͬ
  API_WRITER[2]=0xD6;          API_WRITER[3]=0xdf;
  API_WRITER[4]=0xec;          API_WRITER[5]=0xf7;
  API_WRITER[6]=0xfe;          API_WRITER[7]=0x01;
  API_WRITER[8]=0x15;          API_WRITER[9]=0xF0;
  API_WRITER[10]=0xff;         API_WRITER[11]=0x03;
  API_SET_PROPERTY_X(Group_MODEM_CHFLT,0X0C,ad_MODEM_CHFLT_RX2_CHFLT_COE_7,API_WRITER); 
  
  
  //RF_PA_MODE_4        ���ʷŴ���  //��ͬ
  PA_MODE(0x08);
  PA_PWR_LVL(0x7f);
  PA_BIAS_CLKDUTY(0x00);
  PA_TC(0x3d);
   
  
  //RF_SYNTH_PFDCP_CPFF_7     //��ͬ
  SYNTH_PFDCP_CPFF(0x2C);
  SYNTH_PFDCP_CPINT(0x0E);
  VCO_KV(0x0B);
  SYNTH_LPFILT3(0x04);
  SYNTH_LPFILT2(0x0C);
  SYNTH_LPFILT1(0x73);
  SYNTH_LPFILT0(0x03);
  
  
  //RF_MATCH_VALUE_1_12
  /****************************/
  

    
}
//���Ƶ��470.1M
//����40kbps,Ƶ��ƫ��20KHz
/***********************************************************************/
/*            ������ʼ��RF��standby*/
/***********************************************************************/
void InitRFChipB470_Baud40000(void)    
{
    unsigned char API_WRITER[16];  //�����
    unsigned char IntSta[12];

  //power up
  API_WRITER[0]=CMD_POWER_UP; API_WRITER[1]=0x01;
  API_WRITER[2]=0x00;         API_WRITER[3]=0x01;
  API_WRITER[4]=0xc9;         API_WRITER[5]=0xc3;
  API_WRITER[6]=0x80;         
  API_SendCommand(0x07,API_WRITER);
  API_WaitforCTS();

  
  //GPIO confi
  API_GPIO_CFG(0x21,0x20,0x20,0x21);
  
  
  //GLOBAL_XO_TUNE
  GLOBAL_XO_TUNE(0x52);
  
  //GLOBAL_CONFIG
  GLOBAL_CONFIG(0X60);
  
  //INT_CTL_ENALBE
  INT_CTL_ENABLE(0x00);  //Ĭ�Ϲر����е��ж�
//INT_CTL_PH_ENABLE(0x18);    //0x20
  
  
  //FRR_CTL_MODE            ����ʵ���������
  FRR_CTL_A_MODE(0x04);    //���ݰ���������ж�
  FRR_CTL_B_MODE(0x0a);    //latch RSSI value as defined in MODEM_RSSI_CONTROL
  /*���ٷ�Ӧ�Ĵ���C/D�ɸ���ʵ��ʹ����������趨*/
  //FRR_CTL_C_MODE(0X00);
  //FRR_CTL_D_MODE(0X00);
  
  //PREAMBLE
  PREAMBLE_TX_LENGTH(0X08);  //0x0a
  PREAMBLE_CONFIG(0X31);
  /*0x14��0x0fΪ�Ĵ���Ĭ��ֵ���������ﲻ��Ҫ���ã������Ҫ��
   �Ĵ������в��ָı���޸Ĵ˴�*/
 // PREAMBLE_CONFIG_STD1(0X14);  //RXǰ�����������
  //PREAMBLE_CONFIG_STD2(0X0F);  //RX����ǰ��������
  
  //SYNC
  SYNC_CONFIG(0X01);
  SYNC_BITS_31_24(0XB4);
  SYNC_BITS_23_16(0X2B);
  
  
  //PKT_CRC_CONFIG  CRC����ѡ��
  PKT_CONFIG1(0x02);
  PKT_CRC_CONFIG(0x85);
  PKT_TX_THRESHOLD(0x30);      //���Ϳ������ֵ  48�ֽ�
  PKT_RX_THRESHOLD(0x30);      //���ܿ�������ֵ  48�ֽ�
#ifdef LengthFieldInTwoByte
   PKT_LEN(0x3a);               //�����ֽ����ó�Ϊ�����ֽ�
#else
   PKT_LEN(0x2a);               //�����ֽ����ó�Ϊһ���ֽ�
#endif  
   PKT_LEN_FIELD_SOURCE(0x01);            
   PKT_LEN_ADJUST(0x00);
   PKT_FIELD_1_LENGTH_7_0(0x01);
   PKT_FIELD_1_CONFIG(0x04);
   PKT_FIELD_2_LENGTH_7_0(0xff);
   PKT_FIELD_2_CONFIG(0x04);
  
  //MODEM
  //modem type
  MODEM_MOD_TYPE(0x03); //2GFSK,FIFO handler
  MODEM_MAP_CONTROL(0x00);
  MODEM_DSM_CONTROL(0x07);
  
  //MODEM_CLK_BAND
  MODEM_CLKGEN_BAND(0X0A);
  
  //modem data rate         ������
  MODEM_DATA_RATE_2(0X0c);        //40kbps
  MODEM_DATA_RATE_1(0X35);                
  MODEM_DATA_RATE_0(0X00);
  MODEM_TX_NCO_MODE_3(0X09);                
  MODEM_TX_NCO_MODE_2(0Xc9);                
  MODEM_TX_NCO_MODE_1(0Xc3);                
  MODEM_TX_NCO_MODE_0(0X80);
  
  //modem deviation   Ƶƫ
  MODEM_FREQ_DEV_2(0x00);
  MODEM_FREQ_DEV_1(0x05);
  MODEM_FREQ_DEV_0(0x76);
  
  
  /*
  �βΣ�group���Ĵ�����������
      num_pro����Ҫ�������üĴ����ĸ���
     start_pro�����üĴ�����ĳ�ʼ��ַ
     *Data���Ĵ������õ�����
  */
  //RF_MODEM_TX_RAMP_DELAY_8
  API_WRITER[0]=0X01;         API_WRITER[1]=0x00;
  API_WRITER[2]=0x08;         API_WRITER[3]=0x03;
  API_WRITER[4]=0x80;         API_WRITER[5]=0x00;
  API_WRITER[6]=0x20;         API_WRITER[7]=0x20;
  API_SET_PROPERTY_X(Group_MODEM,0X08,ad_MODEM_TX_RAMP_DELAY,API_WRITER);
  
 //RF_MODEM_BCR_OSR_1_9 
  API_WRITER[0]=0X00;          API_WRITER[1]=0x5E;
  API_WRITER[2]=0x05;          API_WRITER[3]=0x76;
  API_WRITER[4]=0x1A;          API_WRITER[5]=0x05;
  API_WRITER[6]=0x72;          API_WRITER[7]=0x02;
  API_WRITER[8]=0X00;          
  API_SET_PROPERTY_X(Group_MODEM,0X09,ad_MODEM_BCR_OSR_1,API_WRITER);
  
  //RF_MODEM_AFC_GEAR_7
  API_WRITER[0]=0X00;          API_WRITER[1]=0x12;
  API_WRITER[2]=0xC1;          API_WRITER[3]=0x5E;
  API_WRITER[4]=0x01;          API_WRITER[5]=0xCD;
  API_WRITER[6]=0xE0;          
  API_SET_PROPERTY_X(Group_MODEM,0X07,ad_MODEM_AFC_GEAR,API_WRITER);
  
  //RF_MODEM_AGC_CONTROL_1
  API_WRITER[0]=0XE2;
  API_SET_PROPERTY_X(Group_MODEM,0X01,ad_MODEM_AGC_CONTROL,API_WRITER);
  
  //RF_MODEM_AGC_WINDOW_SIZE_9
   API_WRITER[0]=0X11;          API_WRITER[1]=0x15;
  API_WRITER[2]=0x15;          API_WRITER[3]=0x00;
  API_WRITER[4]=0x1A;          API_WRITER[5]=0x40;
  API_WRITER[6]=0x00;          API_WRITER[7]=0x00;
  API_WRITER[8]=0X28;          
  API_SET_PROPERTY_X(Group_MODEM,0X09,ad_MODEM_AGC_WINDOW_SIZE,API_WRITER);
  
  //RF_MODEM_OOK_CNT1_11
  API_WRITER[0]=0XA4;          API_WRITER[1]=0x03;
  API_WRITER[2]=0xD6;          API_WRITER[3]=0x03;
  API_WRITER[4]=0x00;          API_WRITER[5]=0xCC;
  API_WRITER[6]=0x01;          API_WRITER[7]=0x80;
  API_WRITER[8]=0XFF;           API_WRITER[9]=0x0C;
  API_WRITER[10]=0x00; 
  API_SET_PROPERTY_X(Group_MODEM,0X0B,ad_MODEM_OOK_CNT1,API_WRITER);
  
 //RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12
  API_WRITER[0]=0XA2;          API_WRITER[1]=0x81;
  API_WRITER[2]=0x26;          API_WRITER[3]=0xAF;
  API_WRITER[4]=0x3F;          API_WRITER[5]=0xEE;
  API_WRITER[6]=0xC8;          API_WRITER[7]=0xC7;
  API_WRITER[8]=0XDB;          API_WRITER[9]=0xF2;
  API_WRITER[10]=0x02;         API_WRITER[11]=0x08;
  API_SET_PROPERTY_X(Group_MODEM_CHFLT,0X0C,ad_MODEM_CHFLT_RX1_CHFLT_COE_13,API_WRITER); 
  
  //RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12
  API_WRITER[0]=0X07;          API_WRITER[1]=0x03;
  API_WRITER[2]=0x15;          API_WRITER[3]=0xFC;
  API_WRITER[4]=0x0F;          API_WRITER[5]=0x00;
  API_WRITER[6]=0xA2;          API_WRITER[7]=0x81;
  API_WRITER[8]=0X26;          API_WRITER[9]=0xAF;
  API_WRITER[10]=0x3F;         API_WRITER[11]=0xEE;
  API_SET_PROPERTY_X(Group_MODEM_CHFLT,0X0C,ad_MODEM_CHFLT_RX1_CHFLT_COE_1,API_WRITER); 
  
  //RF_MODEM_RX2_CHFLT_RX2_CHFLT_COE7_7_0_12
  API_WRITER[0]=0XC8;          API_WRITER[1]=0xC7;
  API_WRITER[2]=0xDB;          API_WRITER[3]=0xF2;
  API_WRITER[4]=0x02;          API_WRITER[5]=0x08;
  API_WRITER[6]=0x07;          API_WRITER[7]=0x03;
  API_WRITER[8]=0X15;          API_WRITER[9]=0xFC;
  API_WRITER[10]=0x0F;         API_WRITER[11]=0x00;
  API_SET_PROPERTY_X(Group_MODEM_CHFLT,0X0C,ad_MODEM_CHFLT_RX2_CHFLT_COE_7,API_WRITER); 
  
  //RF_PA_MODE_4  ���ʷŴ���
  PA_MODE(0x08);
  PA_PWR_LVL(0x7f);
  PA_BIAS_CLKDUTY(0x00);
  PA_TC(0x3d);
    
  //RF_SYNTH_PFDCP_CPFF_7
  SYNTH_PFDCP_CPFF(0X2C);
  SYNTH_PFDCP_CPINT(0X0E);
  VCO_KV(0X0B);
  SYNTH_LPFILT3(0X04);
  SYNTH_LPFILT2(0X0C);
  SYNTH_LPFILT1(0X73);
  SYNTH_LPFILT0(0X03);
  
  //RF_MATCH_VALUE_1_12
  /****************************/
  
  //RF_FREQ_CONTROL_INTE_8
  FREQ_CONTROL_INTE(0x3d);     //����Ƶ��470.1MHz  ������200khz
  FREQ_CONTROL_FRAC_2(0x0d);
  FREQ_CONTROL_FRAC_1(0x70);
  FREQ_CONTROL_FRAC_0(0xa3);
  FREQ_CONTROL_CHANNEL_STEP_SIZE_1(0x36);
  FREQ_CONTROL_CHANNEL_STEP_SIZE_0(0x9d);
  FREQ_CONTROL_W_SIZE(0x20);
  FREQ_CONTROL_VCOCNT_RX_ADJ(0xfe);
  API_Get_All_IntStatus(0x08,IntSta);
  
}

/*************************************************
����RF��������Ӧ��Ƶ��
*************************************************/
void InitRFChipWorkBand()
{
          // InitRFChipB470_Baud4000();
             InitRFChipB470_Baud40000();
}

/*********************************************************************/
/*  �������ݰ���                                                     */
/*  ��������:                                                        */
/*   char *txBuffer: �����͵����ݣ�                                  */
/*   int size�����txBuffer���ܹ���Ҫ���͵����ݳ���                  */
/*   �����͵����ݳ��Ȳ����� 255-5 ��250�ֽ�                          */
/*  ��������ֵ ��0: ���ͳɹ�;-1������ʧ��;-2:���ݳ���̫��;           */
/*  ������Ϻ��Զ��������̬                                         */
/*********************************************************************/
 int RFSendPacket(unsigned char *txBuffer, int size)
 {
#ifdef LengthFieldInTwoByte
   if(size>8187)
     return -2;
   unsigned int MaxSize=8187;   //������ܹ����͵������
   unsigned char TxBuffer[8191]; //�������ݻ���  
#else
   if(size>251)
     return -2;
   unsigned int MaxSize=251;    //������ܹ����͵������
   unsigned char TXBuffer[255]; //�������ݻ���
#endif
   unsigned char IntSta[8];     //�洢�жϱ�־
   unsigned int  DataSent=0;
   unsigned char length=size+4;
   unsigned char ret[2];

   //���������͵�����֡��ʽ��һ����Ҫ�󣬵�һ���ֽ�ҪΪ����֡���ܳ���
   //�����ݰ�����PKT_LEN(??)�ֶθı�ʱ����ʽҪ���Ÿı䣬����ο������ֲ�
#ifdef LengthFieldInTwoByte        //�����ֽڱ�ʾ����֡���ȣ��ܹ����ʹ���255�ֽڵ�����
     for(int i=2;i<size;i++)
   {
     TXBuffer[i+4]=txBuffer[i];
   }
   
    TXBuffer[0]=length/256;
    TXBuffer[1]=length%256;
   if(g_uchConfigFlag==0)
   {
      TXBuffer[2] = g_PHYDevInfo.IntegratorID.ucIntegratorID[0];   //������ID
      TXBuffer[3] = g_PHYDevInfo.IntegratorID.ucIntegratorID[1];
      TXBuffer[4] = g_PHYDevInfo.ProjectID.ucProjectID[0];          //�豸����ID
      TXBuffer[5] = g_PHYDevInfo.ProjectID.ucProjectID[1];
     
   }
   else
   {
     TXBuffer[5]=TXBuffer[2]=TXBuffer[3]=TXBuffer[4]=0x88;
   }
   PKT_FIELD_2_LENGTH_7_0(TXBuffer[1]-1);         //�ֶ�2�ĳ�������
   PKT_FIELD_2_LENGTH_12_8(TXBuffer[0]);          //�ֶ�2�ĳ�������
  
#else                           //һ���ֽڱ�ʾ����֡�ĳ���
   for(int i=1;i<size;i++)
   {
     TXBuffer[i+4]=txBuffer[i];
   }
   
    TXBuffer[0]=length;
   if(g_uchConfigFlag==0)
   {
      TXBuffer[1] = g_PHYDevInfo.IntegratorID.ucIntegratorID[0];   //������ID
      TXBuffer[2] = g_PHYDevInfo.IntegratorID.ucIntegratorID[1];
      TXBuffer[3] = g_PHYDevInfo.ProjectID.ucProjectID[0];          //�豸����ID
      TXBuffer[4] = g_PHYDevInfo.ProjectID.ucProjectID[1];
     
   }
   else
   {
     TXBuffer[1]=TXBuffer[2]=TXBuffer[3]=TXBuffer[4]=0x88;
   }
   PKT_FIELD_2_LENGTH_7_0(length-1);         //�ֶ�2�ĳ�������
#endif


 //���ﲻҪһ��������FIFO����������ᵼ��FIFO�����ʹ�����ݷ��Ͳ���ȷ
   if(size<=50)      //����FIFO�л�ʣ��10���ֽڵĿ�λ
   {
       API_ClearTxFIFO();
       INT_CTL_ENABLE(0x01);                  //�������ݰ��ж�
       INT_CTL_PH_ENABLE(0x20);               //�������ݷ�������ж�
       API_WriteTxDataBuffer(length,TXBuffer);      //ע������д�����length
       API_FIFO_INFO(ret);
       SetRFStartTx();               //��ʼ����
     while(GET_NIRQ_BIT);
       
     API_FIFO_INFO(ret);  
     API_Get_All_IntStatus(0x08,IntSta);    //����жϱ�־
        if(IntSta[3]&0x20)
        {
          return 0;   //���ͳɹ�
        } 
        else
        {
          return -1;     //����ʧ��
        }
       
   }
   else if(size<=MaxSize)
   {
     size-=50;
     DataSent=54;            //һ�����д��54�ֽڵ�FIFO��
     INT_CTL_ENABLE(0x01);       //�������ݰ��ж�
     INT_CTL_PH_ENABLE(0x02);    //����FIFO�������
     API_WriteTxDataBuffer(54,TXBuffer);
     API_FIFO_INFO(ret);
     API_Get_All_IntStatus(0x08,IntSta);    //����жϱ�־
     SetRFStartTx();    //��ʼ����
     while(size>48)
     {
         while(GET_NIRQ_BIT);  //�ȴ��жϵĲ��� 
        API_WriteTxDataBuffer(48,TXBuffer+DataSent);
        DataSent=DataSent+48;
        size=size-48;
        API_ClearPHInt();
     }
        while(GET_NIRQ_BIT);        //�ȴ��жϵĲ��� 
        API_WriteTxDataBuffer(size,TXBuffer+DataSent);
        INT_CTL_PH_ENABLE(0x20);    //���ݷ�������ж�
        while(GET_NIRQ_BIT);
        API_Get_All_IntStatus(0x08,IntSta);    //����жϱ�־
        if(IntSta[3]&0x20)
        {
          return 0;   //���ͳɹ�
        } 
        else
        {
          return -1;     //����ʧ��
        }
        
   } 
   return -1;
 }
/****************************************************************************/
/*  �������ݰ������Ȳ���                                                    */
/*  ��������: unsigned char *rxBuffer ��Ž��յ�����                        */ 
/*                                          ,rxBuffer[0] �����Ч���ݰ���С */
/*  ��������ֵ ��0: ���ճɹ���
                -1�����̵�ַ��ƥ��;
                -2: û���յ��κ�����                         
                -3:�յ����ݵ���CRCУ�����                                  
                -4:�յ�ͬ���ֺͲ�������,�����ݽ��ճ�ʱʧ�ܣ�                */
/****************************************************************************/  
 int RFReceivePacket(unsigned char *rxBuffer)
{
   unsigned char IntSta[8];        //�洢�жϱ�־
   unsigned int  offset=0;         //����ƫ��
   unsigned int  HaveReadLen=0;    //�ѽ��ܵ����ݰ�����
   unsigned int  DataLength=0;     //���ݰ��ĳ���
   unsigned char flag=0;           //��һ�����ݵı�־
   unsigned char ReadLen=0;        //ʵ��һ�ζ�ȡ����
   unsigned char OneTimeLen = 48;  //�趨һ�ζ�ȡ���ݳ��� 
   unsigned char IDcheck[4];       //У��ID
       
    //1��ʱ������(1/PHY_MCLK)��,
    unsigned long time = PHY_MCLK_1MS;    //���������ݴ������ʽ��͵�ʱ����Ҫ�޸Ĳ���ʱ��
    time=time*20;
    unsigned long  Timeout=time;
#ifdef  LengthFieldInTwoByte
    PKT_FIELD_2_LENGTH_7_0(0xff);    //���������ݵ����� 
    PKT_FIELD_2_LENGTH_12_8(0x1f);    //���������ݵ����� 
#else
    PKT_FIELD_2_LENGTH_7_0(255);    //���������ݵ����� 
#endif
     
    
   while(--Timeout)
   {
     if(GET_NIRQ_BIT==0)
     {
       API_GetFastResponseRegister(0x50,0x01,IntSta);   //��ȡ�жϱ�־λ
       API_ClearPHInt();                                //������ݰ��ж�
   
       
       if(IntSta[0]&0x80)   //CRCУ�����
       {
         API_ClearRxFIFO();
         return -3;
       }
       
       
       if(IntSta[0]&0x10)   //���ݰ��������
       {
         if(flag==0)  //��һ������
         {
            //�ȶ�ȡ����֡�����ֽڣ��ڶ�ȡ����ID
#ifdef    LengthFieldInTwoByte
             API_ReadRxDataBuffer(0x02,rxBuffer); //��ȡ�����ֽڵ�����֡����
             API_ReadRxDataBuffer(0x04,IDcheck);  //����ID,������ID
             DataLength=rxBuffer[0]*256+rxBuffer[1]-4; //���ݰ��ĳ���
             rxBuffer[0]=DataLength/256;               //����֡���ȵĸ��ֽ�
             rxBuffer[1]=DataLength%256;               //����֡���ȵĵ��ֽ�
#else
             API_ReadRxDataBuffer(0x01,rxBuffer); //��ȡ����֡����
             API_ReadRxDataBuffer(0x04,IDcheck);  //����ID,������ID
             DataLength=rxBuffer[0]-4;            //ȥ������ID������֡���ܳ���
             rxBuffer[0]=DataLength;              //����֡����
#endif
            
             
             if(DataLength<1)
             {
               API_ClearRxFIFO();  //���RxFIFO
               API_ClearPHInt();   //����жϱ�־
               return -4;
             }
             ReadLen=DataLength-1;
             if(ReadLen>0)
                API_ReadRxDataBuffer(ReadLen,rxBuffer+1);
             offset = ReadLen+1;//�ڶ�ȡ�������е�ƫ��
             HaveReadLen = ReadLen+1;                 
         }
         else if(DataLength>offset)
         {
           ReadLen = (DataLength-offset) ;
           API_ReadRxDataBuffer(ReadLen,rxBuffer+offset);
           offset+=ReadLen;
           HaveReadLen+=ReadLen;
         }
            API_ClearRxFIFO();
            return 0;  
       }
       
       
       if(IntSta[0]&0x01)     //RX_fifo����
       {
           if(flag==0)   //�ǵ�һ������
           {
             flag=1;
#ifdef  LengthFieldInTwoByte
             API_ReadRxDataBuffer(0x02,rxBuffer); //��ȡ�����ֽڵ�����֡����
             API_ReadRxDataBuffer(0x04,IDcheck);  //����ID,������ID
             DataLength=rxBuffer[0]*256+rxBuffer[1]-4;
             rxBuffer[0]=DataLength/256;
             rxBuffer[1]=DataLength%256;
#else
             API_ReadRxDataBuffer(0x01,rxBuffer); //��ȡһ���ֽڵ�����֡����
             API_ReadRxDataBuffer(0x04,IDcheck);  //����ID,������ID
             DataLength=rxBuffer[0]-4;
             rxBuffer[0]=DataLength;
#endif       
             if(DataLength<1)     //����֡���ȴ���
             {
                API_ClearRxFIFO();  //���RX_FIFO
                
                 return -4;
             }
               ReadLen=(DataLength-1)<OneTimeLen-5? (DataLength-1):OneTimeLen-5;
             if(ReadLen>0)
                API_ReadRxDataBuffer(ReadLen,rxBuffer+1);    //��ȡʣ��һ���Զ�ȡ���ֽ�
             offset=ReadLen+1;
             HaveReadLen=ReadLen+5; 
             
              //У��ID
             if(g_uchConfigFlag == 0)//��������̬ 
                {
                   if( (IDcheck[0] != g_PHYDevInfo.IntegratorID.ucIntegratorID[0]) ||
                       (IDcheck[1] != g_PHYDevInfo.IntegratorID.ucIntegratorID[1]) ||
                       (IDcheck[2] != g_PHYDevInfo.ProjectID.ucProjectID[0])       ||
                       (IDcheck[3] != g_PHYDevInfo.ProjectID.ucProjectID[1]) )
                                            
                        {
                            API_ClearRxFIFO();  //���RX_FIFO
                            
                            return -1; //��ַ��ƥ��
                            
                        } //end if  ������ID���߹���ID��ƥ��
                   
                  }//end if ��������̬ 
             else//����̬ 
                  {
                     if( (IDcheck[0] != 0x88) ||
                         (IDcheck[1] != 0x88) ||
                         (IDcheck[2] != 0x88) ||
                         (IDcheck[3] != 0x88) )                                     
                        { 
                            API_ClearRxFIFO();  //���RX_FIFO
                                return -1; //��ַ��ƥ��                        
                        }        //end if  ����̬ID��ƥ��    
                     
                   }//end else ����̬      
             
             if(DataLength==offset)
             {
               API_ClearRxFIFO();  //���RxFIFO
               API_ClearPHInt();   //����жϱ�־
               return 0;
             }
           }   //end if RX_FIFO��������һ������
           else   //���ǵ�һ������
           {
              ReadLen=(DataLength-offset)<OneTimeLen ? (DataLength-offset):OneTimeLen;
              API_ReadRxDataBuffer(ReadLen,rxBuffer+offset); 
              offset=offset+ReadLen;           //λ��
              HaveReadLen=HaveReadLen+ReadLen;
              if(DataLength==offset)
              {
                  API_ClearRxFIFO();  //���RxFIFO
                  API_ClearPHInt();   //����жϱ�־
                  return 0;
              }
           }    //end else RX_FIFO���������ǵ�һ������
           Timeout = time;//ʱ������
           
       }   //end if RX_FIFO��Ҫ��
       
     }  //end if ���жϲ���
     
   }    //end if while
   return -4;
 }