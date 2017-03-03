/******************************************************************************/
/*            应用目标板：SI4463 B系列射频板                                  */
/*            内容描述：SI4463射频模块物理层驱动源文件                        */
/*            作者：周凯 ,上海第二工业大学                                    */
/*            版本号：V1.0.0                                                  */
/*            日期：2014.08.20                                                */
/*            修改者：                                                        */
/*            修改日期：                                                      */
/******************************************************************************/
#include "PHYlib.h"
#include "RFlib.h"
#include "global.h"

unsigned char g_uchrRFMode=RF_Standby;    //开机默认模式:standby

//#define LengthFieldInTwoByte   //定义两个字节表示数据帧长度
//频点序列的相关定义
unsigned char g_chRFFreList[RF_FRE_AMOUNT] = {0};           //频点序列数组
//unsigned char g_uchRFSelfFreNum =0xff ;                     //射频物理层全局变量，定义当前自己工作频点编号
unsigned char g_uchRFSelfFreNum=0x00;          //开机默认频道号:0x00
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

unsigned char g_uchRxPAtype = 0;    //表示发送和接收设备的功放类型 0x0A --A型设备 无功放；0x0B --B型设备 有功放； 0x00 --未知

unsigned char g_uchRSSI = 0;//定义无线信号RSSI值


/******************************
RF控制管脚状态初始化
******************************/
void RFCtlLineInit()
{
  //GPIO2输入方向
  SET_GPIO2_IN;//GPIO2输入方向
  EN_GPIO2_RES;//GPIO2上/下拉电阻使能
  GPIO2_OUT_L; //GPIO2下拉电阻
  
  //SDN
  DIS_SDN_RES;//SDN不要上拉/下拉电阻
  SET_SDN_OUT;//定义SDN为输出  
  SDN_OUT_H;//SDN输出高电平 RF进入power down
  
  //SCLK
  DIS_SCLK_RES;//SCLK不要上拉/下拉电阻
  SET_SCLK_OUT;//定义SCLK为输出 
  SCLK_OUT_L;
  
  //SDI
  DIS_SDI_RES;//SDI不要上拉/下拉电阻
  SET_SDI_OUT;//定义SDI为输出
  SDI_OUT_L;
  //SDO
  SDO_OUT_L;//SDO输出低电平
  EN_SDO_RES;//SDO要下拉电阻  
  SET_SDO_IN;//定义SDO为输入
  
  //NSEL
  DIS_NSEL_RES;//NSEL不要上拉/下拉电阻
  SET_NSEL_OUT;//定义NSEL为输出 
  NSEL_OUT_H;
  
  //NIRQ  上拉电阻输入，关闭中断
  NIRQ_OUT_H;//NIRQ输出高电平 
  EN_NIRQ_RES;//NIRQ带上拉电阻  
 //DIS_NIRQ_RES;//NIRQ不带上/下拉电阻  
  SET_NIRQ_IN;//再定义NIRQ为输入
  SET_NIRQ_IES;//设置由高到低中断
  CLR_NIRQ_IFG;//清除中断标记
 //EINT_NIRQ;//允许中断
  DINT_NIRQ;  //中断关闭
}
/**************************************
发送一个字节的数据
形参：Data需要发送的数据
返回值：无
****************************************/
/*
void  API_SendDataByte(uchar Data)   //子函数
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

void  API_SendDataByte(uchar Data)   //子函数
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
发送多个字节的数据，不获取返回值
形参：DataLength发送数据长度
      *Data：数组头指针
返回值：无
***************************************/
/*
void API_SendDataNoResponse(uchar DataLength,uchar *Data)   //子函数
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

void API_SendDataNoResponse(uchar DataLength,uchar *Data)   //子函数
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
发送数据后得到返回值，仅仅是获取输出字节的函数
形参：DataLength返回值的长度
      *Data存储返回值数组的头指针
返回值：无
************************************************/
/*
void API_SendDataGetResponse(uchar DataLength,uchar *Data)   //子函数
{
  unsigned char i;
  unsigned char data_out=0x00;
  while(DataLength>0)
  { 
    for(i=0;i<8;i++)
    {
      data_out<<=1;
      _NOP();_NOP();_NOP();_NOP();
      if(GET_SDO_BIT)     //如果返回的是高电平
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
void API_SendDataGetResponse(uchar DataLength,uchar *Data)   //子函数
{
  unsigned char i;
  unsigned char data_out=0x00;
  while(DataLength>0)
  { 
    for(i=0;i<8;i++)
    {
      data_out<<=1;
      //_NOP();_NOP();_NOP();_NOP();
      if(GET_SDO_BIT)     //如果返回的是高电平
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
发送命令函数
形参：CmdLength命令的长度
      *CmdData命令数组的头指针
返回值：0
**************************************************/
uchar API_SendCommand(uchar CmdLength,uchar *CmdData)
{  
    NSEL_OUT_L;        //NSEL低电平初始化通讯接口
    API_SendDataNoResponse(CmdLength,CmdData);
    NSEL_OUT_H;
    return 0x00;
}

/***************************************************
当完成发送命令后，得到发送命令后得到的返回值
形参：DataLength返回数据的长度
     *Data存储返回数据的数组头指针
返回值：0：获取返回数据成功
        1：获取返回数据失败
 ****************************************************/
uchar API_GetResponse(uchar DataLength,uchar *Data)
{
     unsigned char CTS_value=0;     //返回的CTS
     unsigned int  CTS_err=0;       //读取CTS错误的次数
     while(CTS_value!=0xff)    //判断是否完成命令，判断CTS
    {
      NSEL_OUT_L;
      API_SendDataByte(CMD_READ_CMD_BUFF);   //发送READ_CMD_BUFFER命令
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
     API_SendDataGetResponse(DataLength,Data); //CTS完成后可以读取BUFFER的值
     NSEL_OUT_H;
     return 0;
}
/***********************************************************
等待命令处理的完成，得到正确的CTS
返回值：0:处理命令成功
        1:处理命令失败
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
读取中断标志，并清除中断标志的值
读取的是PH.MODEM.CHIP三个中断的标志
形参：IntStatusLength：命令的回复流的长度
      *IntStatus：存储回复数据的数组指针
返回值：0x00:读取返回值数据成功
        0x01:读取返回值数据失败
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
清除数据包处理的状态标志位
形参：无
返回值：无
***************************************************************/
 void API_ClearPHInt()
 {
   unsigned char API_WRITER[2];
  API_WRITER[0]=CMD_GET_INT_STATUS;
  API_WRITER[1]=0x00;
  API_SendCommand(0x02,API_WRITER);
  //延时3毫秒，等待NIRQ标志位的清除
  /**注意***/
  //该延时函数不可删除，删除可能会导致无线数据读取的错误
  PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();
 }
/****************************************************************
获取数据包处理的中断状态,可能会清除状态标志位
形参：PHIntStatusLength：返回状态数据的长度
      *PHIntStatus：存储返回数据数组的头指针
返回值：0x00:读取返回值数据成功
        0x01:读取返回值数据失败
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
获取调制状态的中断状态，可能会清除状态标志位
形参：ModemIntStatusLength：返回状态数据的长度
      *ModemIntStatus：存储返回数据数组的头指针
返回值：0x00:读取返回值数据成功
        0x01:读取返回值数据失败
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
获取芯片状态的中断状态，可能会清除状态标志位
形参:ChipIntStatusLength：返回状态数据的长度
      *ChipIntStatus：存储返回数据数组的头指针
返回值：0x00:读取返回值数据成功
        0x01:读取返回值数据失败
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
获取芯片的基本信息
形参：PARTInfoLength:返回芯片基本数据的长度
      *PARTInfo：存储返回数据数组的头指针
返回值：0x00:读取设备信息正常
        0x01:读取设备信息失败
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
向TXFIFO中写入数据，一次性最多可以写入64字节
形参：TxFifoLenght：写入数据的长度
      *TxFifoData：存储FIFO数据数组的头指针
返回值：0
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
从RXFIFO中读取数据
形参：RxFifoLength：读取数据的长度
      *RxFifoData：存储RXFIFO中数据数组的头指针
返回值：0
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
清空TXFIFO
形参：无
返回值：TXFIFO剩余的空间
        0xff:清空TXFIFO失败
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
uchar RFResetTxFIFO()   //对于链路层函数
{
  unsigned char Ret=API_ClearTxFIFO();
  return Ret;
}
/*************************************************************
清空RXFIFO
形参：无
返回值：RXFIFO的使用空间
        0xff:RXFIFO清空失败
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
uchar RFResetRxFIFO()   //对于链路层函数
{
  unsigned char Ret=API_ClearRxFIFO();
  return Ret;
}
/*********************************************************
读取快速反应寄存器里的存储值
形参：bStartReg：寄存器的开始地址
      bNumberOfReg：读取寄存器的个数
      RegValues： 存储寄存器的二维数组头指针
返回值：0x01:数据读取错误（读取寄存器个数或者地址错误）
        0x00:读取数据成功
**********************************************************/
uchar API_GetFastResponseRegister(uchar bStartReg,uchar bNumberOfReg,uchar *RegValues)
{
   if (bNumberOfReg<=0||bNumberOfReg>4)   //判读是否读取的个数错误
  {
    return 0x01;
  }
  if (bStartReg==0x50||bStartReg==0x51||bStartReg==0x53||bStartReg==0x57) //判断输入的命令是否是读取几个寄存器的命令
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
读出FIFO的剩余空间，不重置FIFO
形参：存储剩余空间的二维数组头指针
返回值：无
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
 RF通用IO管脚配置
形参：GPIO0：GPIO0配置方式
      GPIO1：GPIO1配置方式
      GPIO2：GPIO2配置方式
      GPIO3: GPIO3配置方式
 返回值：无
参考：GPIO0：0X20，GPIO1：0X21
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
上电芯片初始化,打开RF芯片后，初始化RF
形参：无
返回值：无
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
  PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();//延时6毫秒
  PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();
}

/**********************************************************************
寄存器的设置，对同一个组的多个寄存器连续进行设置
形参：group：寄存器所属的组
      num_pro：需要连续设置寄存器的个数
     start_pro：设置寄存器组的初始地址
     *Data：寄存器设置的数据
返回值：1：一次性配置的寄存器过多，配置失败
        0：一次性配置寄存器成功
************************************************************************/
uchar API_SET_PROPERTY_X(unsigned char group,unsigned char num_pro,unsigned char start_pro,unsigned char *Data)  //unsigned char *Data
{
  uchar API_WRITER[20];       
  uchar m=4;         //开始寄存器数据的数组下标
  uchar num=num_pro; //需要配置的寄存器的个数
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
         if(API_WaitforCTS())     //CTS返回值错误
           return 0x01;
         return 0x00;
     }
}
/**********************************************************************
对一个寄存器进行设置
形参：group:该寄存器所属的组
      address：该寄存器的地址
      Data：对该寄存器配置的数据
返回值：0：对寄存器的设置成功
        1：对寄存器的设置失败
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
读取寄存器的设置的值
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
改变芯片所处的状态
形参：RFStatus：RF将要进入的状态
返回值：0x01:状态改变不成功
        0x00:状态改变成功
******************************************************************/
uchar API_CHANGE_STATE(uchar RFStatus)       //使用该代码完成时射频模块进入到相应的状态
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
请求获得当前芯片所处的状态
形参：存储RF状态的数组头指针
返回值：0x00:获取返回值成功 
        0x01:获取返回值失败
0:no change /// 1:sleep state /// 2:spi active state
3:ready state /// 4:another enumeration for ready state
5:tune state for tx /// 6:tune state for rx
7:tx state /// 8:rx state
========================================*/
uchar API_REQUEST_DEVICE_STATE(uchar *RFStatus)
{
  uchar cmd=CMD_REQUEST_DEVICE_STATE;
  API_SendCommand(0x01,&cmd);
  if(API_GetResponse(0x02,RFStatus)==0)    //获取返回值成功
    return 0x00;
  return 0x01;
}
/**************************************************************/
/*设定RF进入到Power Down模式                                  */
/***************************************************************/
void SetRFOff(void)
{
  SDN_OUT_H;  //SDN管脚高电平，RF进入到ShutDown模式
  
  g_uchrRFMode=RF_Shutdown;
}

/***************************************************************/
/*设定RF开机                                                   */
/***************************************************************/
 void SetRFOn(void)
 {
   SDN_OUT_L;    //SDN管脚低电平，RF开机
   
   //从shutdown模式下到TX/RX模式下至少15ms，至少等待15ms
   int i=40;   //延时约40ms
   while(i--)
   PHYMCLKSoftDelay1ms();
   
   g_uchrRFMode=RF_Standby;
 }
/***************************************************************
硬件重设RF Reset RF
****************************************************************/
 void HWResetRF()
{
  uchar IntStatus[8];
  uchar DeviceInfo[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  RFCtlLineInit(); //RF控制线初始化
  
  SetRFOff();  //关闭RF  关闭之后至少等待10ns
  PHYMCLKSoftDelay1ms();   //延时1ms
  SetRFOn(); //RF开机
  API_PART_INFO(0x08,DeviceInfo);  //读取设备信息
  if(DeviceInfo[1]!=0x44||DeviceInfo[2]!=0x63)  //判断设备的读取信息是否正确
    {
      int i=200;
      while(1)
      {
        LedRedOn();
        i=200;
        while(--i)
          PHYMCLKSoftDelay1ms();   //延时1ms
        LedRedOff();
        i=200;
        while(--i)
          PHYMCLKSoftDelay1ms();   //延时1ms        
      }
    }
  API_Get_All_IntStatus(0x08,IntStatus);//读取中断标志，清除中断标志位
}

 /****************************************************************
跳频函数，写入相应的参数，跳频至相应的；频道号，跳频完成后进入到
接收状态
工作频点：？？？？？待定
****************************************************************/
 void FrequencyHop(unsigned char RFFreNum)
 {
   if(RFFreNum>(RF_FRE_AMOUNT-1))   //工作频道号大于最大频道号，则工作于最大频道号
        RFFreNum=RF_FRE_AMOUNT-1;
   if(RFFreNum!=g_uchRFCurFreNum)
   {
     g_uchRFCurFreNum = RFFreNum;       //记录现在所处的工作频点
     SetRFRx();                         //完成跳频，并进入到接收模式
   }
 }

/***************************************************************/
/*设定RF进入Standby模式                                        */
//Standby和Sleep模式无法使用REQUEST_DEVICE_STATE命令得到
/***************************************************************/
void SetRFStandby(void)
{
  API_CHANGE_STATE(0x01);    //sleep state
  g_uchrRFMode = RF_Sleep;
}

/***************************************************************/
/*设定RF进入Sleep模式    耗电量3                               */
/****************************************************************/
void SetRFSleep()
{
  API_CHANGE_STATE(0x01);  //sleep state
  g_uchrRFMode = RF_Sleep;
}


/***************************************************************/
/*设定RF进入Ready模式                                         */
/****************************************************************/
void SetRFReady(void)
{
   uchar RFStatus[2]; //存储请求设备状态的返回数据
   API_CHANGE_STATE(0x03);
   if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //获取返回数据成功
   {
     if(RFStatus[0]==0x03)
     {
       g_uchrRFMode=RF_Ready;     //记录现在设备处于Ready模式
     }
   } 
}

/***************************************************************/
/*设定RF进入Tx模式下的TUNE模式                                         */
/****************************************************************/
void SetRFTuneForTx(void)
{
  uchar RFStatus[2]; //存储请求设备状态的返回数据
   API_CHANGE_STATE(0x05); 
   if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //获取返回数据成功
   {
     if(RFStatus[0]==0x03)
     {
       g_uchrRFMode=RF_TxTune;     //记录现在设备处于TX_TUNE模式
     }
   } 
}
/***************************************************************/
/*设定RF进入Rx模式下的TUNE模式                                         */
/****************************************************************/
void SetRFTuneForRx(void)
{
   uchar RFStatus[2]; //存储请求设备状态的返回数据
   API_CHANGE_STATE(0x06); 
   if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //获取返回数据成功
   {
     if(RFStatus[0]==0x03)
     {
       g_uchrRFMode=RF_RxTune;     //记录现在设备处于RX_TUNE模式
     }
   } 
}

/***************************************************************/
/*设定RF进入TX模式 (注：数据发送完毕之后自动退出)              */
//注意两者的区别，CHANGE_STATE命令的发送函数无法写入频道号，发送长度
//START_TX函数可以写入频道号以及数据发送长度
/****************************************************************/
//使用CHANGE_STATE命令的发送函数
void SetRFStartTx(void)
{
   /*这里改变TX状态不使用CHANGE_STATE命令
   因为无法写入频道号等命令  */
    uchar RFStatus[2]; //存储请求设备状态的返回数据
    API_CHANGE_STATE(0x07);
    if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //获取返回数据成功
   {
     if(RFStatus[0]==0x07)
     {
       g_uchrRFMode=RF_Tx;     //记录现在设备处于TX模式
     }
   } 
  
}
//使用START_TX命令的发送函数     ..频道号间隔3倍
void SetRFTx()    //size使用uchar类型，最多发送个字节数只能是255
{
  /****************************************/
  /*由此往下添加代码*/
  //添加调制载波模式的代码
   // MODEM_MOD_TYPE(0x00);   //载波模式
  /*由此往上添加代码*/
  /****************************************/
  
  unsigned char RFStatus[2]; //存储请求设备状态的返回数据
  unsigned char Command_Tx[5];
  Command_Tx[0]=CMD_START_TX;
  Command_Tx[1]=g_uchRFSelfFreNum*3;   //使用跳频代码改变工作频道号
  Command_Tx[2]=0x30;     //发送完成后不退出发送模式
  Command_Tx[3]=0x00;
  Command_Tx[4]=0x00;
  while(1)
  {
    unsigned char trytimes=0;
    while(++trytimes<3)
    {
        API_SendCommand(0x05,Command_Tx);     //API_WRITER 
        PHYMCLKSoftDelay1ms();PHYMCLKSoftDelay1ms();
        if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //获取返回数据成功
          {
              if(RFStatus[0]==0x07)
                {
                  g_uchrRFMode=RF_Tx; //记录现在设备处于TX模式
                  break;     
                }
          }       
    }
     if(g_uchrRFMode==RF_Tx)
       break;   
    /**********************************/
    /*由此处往下添加代码*/
    HWResetRF();    
    InitRFChipWorkBand();
    FrequencyHop(g_uchRFSelfFreNum);    //添加日期：2014-11-22
    /*由此处往上添加代码*/
    /**********************************/
  }
}
/***************************************************************/
/*    设定RF进入RX模式                                         */
/*    这里改变RX状态使用CHANGE_STATE命令                     */
/*      因为无法写入频道号等命令                               */
/***************************************************************/
//使用CHANGE_STATE命令进入到RX状态
void SetRFStartRx(void)
{
     uchar RFStatus[2]; //存储请求设备状态的返回数据
     API_CHANGE_STATE(0x08); 
     if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //获取返回数据成功
   {
     if(RFStatus[0]==0x08)
     {
       g_uchrRFMode=RF_Rx;     //记录现在设备处于RX模式
     }
   }  
}

//使用START_RX命令进入到RX状态,形参错误，需要修改形参
void SetRFRx()  
{
  unsigned char RFStatus[2]; //存储请求设备状态的返回数据
  //unsigned char RF_Mode=g_uchrRFMode; 记录上次的状态，计算由上次状态转换成为本次状态所需要的时间
  unsigned char Command_Rx[7];
  Command_Rx[0]=CMD_START_RX;
  Command_Rx[1]=g_uchRFCurFreNum*3;          //现在的频点
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
      /*由此往下添加代码*/
       INT_CTL_ENABLE(0x01);     //开启数据包中断
       INT_CTL_PH_ENABLE(0x19);   //RX_Fifo快满和接受完毕中断
      // INT_CTL_PH_ENABLE(0x18);   //RX_Fifo快满和接受完毕中断
      /*由此往上添加代码*/
      /********************************************/
        API_ClearRxFIFO();                  //清空RxFIFO
        API_SendCommand(0x07,Command_Rx);   //API_WRITER 
        API_WaitforCTS();
        if(API_REQUEST_DEVICE_STATE(RFStatus)==0)  //获取返回数据成功
          {
              if(RFStatus[0]==0x08)
                {
                  g_uchrRFMode=RF_Rx; //记录现在设备处于TX模式
                  break;     
                }
          }       
    }
         if(g_uchrRFMode==RF_Rx)
          break;  
   HWResetRF();
   InitRFChipWorkBand();
   FrequencyHop(g_uchRFSelfFreNum);    //添加日期：2014-11-22
   //注意与SI4432的区别
   //这里不添加跳频代码，调频代码在发送命令以及接受命令里面完成
  }   
}

/****************************************************************/
/*           获取RSSI值并函数返回                               */
/****************************************************************/
 unsigned char GetRSSI()
 { 
   unsigned int i=0;
   unsigned char RSSI[4];
   unsigned char count;
   unsigned char max=0;
   unsigned char min=0xff;
   //SetRFRx();     //进入到接收模式
   for(count=0;count<6;count++)
   {
    //API_GetFastResponseRegister(0x51,0x01,&RSSI);    //快速反应寄存器读取 latch rssi
     API_Get_Modem_IntStatus(0x04,RSSI);
     i+=RSSI[2];
     if(RSSI[2]>max)
       max=RSSI[2];
     if(RSSI[2]<min)
       min=RSSI[2];
   }
  //SetRFReady();  //进入到ready模式
   i=i-max-min;
   RSSI[2]=i/4;
   return RSSI[2];
 }
 /****************************************************
 通过读取Fast response读取latch rssi
 *****************************************************/
unsigned char GetFastLatchRSSI()
{
  unsigned char RSSI=0x00;
  API_GetFastResponseRegister(0x51,0x01,&RSSI);    //快速反应寄存器读取 latch rssi
  return RSSI;
}
 /************************************************************
               将获取的RSSI的值转换为dBm的形式
 *************************************************************/
int GetRSSIPower()
{
  int Ret=GetRSSI();
  return Ret;
}
/**************************************************************
 设定RF进入到TX发射载波
***************************************************************/
 void SetTxCarrier()
 {
   MODEM_MOD_TYPE(0x00);
   SetRFStartTx();
 }
//最低频率470M
//速率4kbps,频率偏移20KHz
/***********************************************************************/
/*            开机初始化RF至standby*/
/***********************************************************************/
void InitRFChipB470_Baud4000(void)    
{
  unsigned char API_WRITER[16];        //命令缓存
  unsigned char IntSta[12];
  //power up
  API_POWER_UP();                          //相同
  API_Get_All_IntStatus(0x08,IntSta);      //相同
  
  //GPIO config
 // API_GPIO_CFG(0x20,0x21,0x20,0x21);
    API_GPIO_CFG(0x21,0x20,0x20,0x21);   //重要，根据射频开关的方法具体设定
    
 //GLOBAL_XO_TUNE
  GLOBAL_XO_TUNE(0x50);                      //相同
  
  //GLOBAL_CONFIG
  GLOBAL_CONFIG(0x60);                       //相同
  
     //MODEM_CLK_BAND
  MODEM_CLKGEN_BAND(0x0A);        //相同
  
    //MODEM
  //modem type
  MODEM_MOD_TYPE(0x03); //2GFSK,FIFO handler    //相同
  MODEM_MAP_CONTROL(0x00);
  MODEM_DSM_CONTROL(0x07);
  
 //modem data rate         数据率     //不相同，可以进行设置
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
  
  //modem deviation   频偏      20kHz
  MODEM_FREQ_DEV_2(0x00);    //相同
  MODEM_FREQ_DEV_1(0x05);
  MODEM_FREQ_DEV_0(0x76);
  
  
    
  //RF_FREQ_CONTROL_INTE_8   //测试完成    频点设置
  FREQ_CONTROL_INTE(0x3d);     //中心频点470.1MHz  步长：200khz
  FREQ_CONTROL_FRAC_2(0x0d);
  FREQ_CONTROL_FRAC_1(0x70);
  FREQ_CONTROL_FRAC_0(0xa3);
  FREQ_CONTROL_CHANNEL_STEP_SIZE_1(0x36);
  FREQ_CONTROL_CHANNEL_STEP_SIZE_0(0x9d);
  FREQ_CONTROL_W_SIZE(0x20);
  FREQ_CONTROL_VCOCNT_RX_ADJ(0xfe);
  
 
  
  //INT_CTL_ENALBE
   INT_CTL_ENABLE(0x00);       //关闭所有的中断
  
   
  //FRR_CTL_MODE
 
  FRR_CTL_A_MODE(0x04);    //数据包处理挂起中断
  FRR_CTL_B_MODE(0x0a);    //latch RSSI value as defined in MODEM_RSSI_CONTROL
  /*快速反应寄存器C/D可根据实际情况进行设置*/
  //FRR_CTL_C_MODE(0X00);
  //FRR_CTL_D_MODE(0X00);
  
  //PREAMBLE  前导码
  PREAMBLE_TX_LENGTH(0x0a);        //相同
  PREAMBLE_CONFIG(0x31);
  /*0x14与0x0f为寄存器默认值，所以这里不需要设置，如果需要对
   寄存器进行部分改变可修改此处*/
  //PREAMBLE_CONFIG_STD1(0X14);  //RX前导码接受门限
  //PREAMBLE_CONFIG_STD2(0X0F);  //RX接受前导码配置
  
  //SYNC   同步字                 //相同
  SYNC_CONFIG(0x01);   //两个字节的同步字，不是曼彻斯特编码，不是4FSK调制
  SYNC_BITS_31_24(0xB4);
  SYNC_BITS_23_16(0x2B);
  
  //PKT_CRC_CONFIG  CRC种子选择

   PKT_CRC_CONFIG(0x85);
   PKT_CONFIG1(0x02);  //new
   PKT_TX_THRESHOLD(0x30);      //发送快空门限值  32字节
   PKT_RX_THRESHOLD(0x30);      //接受快满门限值  32字
   
#ifdef LengthFieldInTwoByte
   PKT_LEN(0x3a);               //长度字节配置成为两个字节
#else
   PKT_LEN(0x2a);               //长度字节配置成为一个字节
#endif   
   PKT_LEN_FIELD_SOURCE(0x01);            
   PKT_LEN_ADJUST(0x00);
   PKT_FIELD_1_LENGTH_7_0(0x01);
   PKT_FIELD_1_CONFIG(0x04);
   PKT_FIELD_2_LENGTH_7_0(0xff);
   PKT_FIELD_2_CONFIG(0x04);
  
   

   
   
   MODEM_RSSI_CONTROL(0x13);
  /*
  形参：group：寄存器所属的组
      num_pro：需要连续设置寄存器的个数
     start_pro：设置寄存器组的初始地址
     *Data：寄存器设置的数据
  */
  //RF_MODEM_TX_RAMP_DELAY_8         //不相同
   
  API_WRITER[0]=0X01;        API_WRITER[1]=0x80; //API_WRITER[1]=0x00;
  API_WRITER[2]=0x08;         API_WRITER[3]=0x03;
  API_WRITER[4]=0x80;         API_WRITER[5]=0x00;
  API_WRITER[6]=0x20;       API_WRITER[7]=0x10;//  API_WRITER[7]=0x20;
  API_SET_PROPERTY_X(Group_MODEM,0X08,ad_MODEM_TX_RAMP_DELAY,API_WRITER);
  
 //RF_MODEM_BCR_OSR_1_9      //不相同
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
  
  
  //RF_MODEM_AFC_GEAR_7       //不相同
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
  API_WRITER[0]=0xE2;           //相同
  API_SET_PROPERTY_X(Group_MODEM,0x01,ad_MODEM_AGC_CONTROL,API_WRITER);
  
  //RF_MODEM_AGC_WINDOW_SIZE_9
  /*
   API_WRITER[0]=0X11;          API_WRITER[1]=0x15;   //不相同
  API_WRITER[2]=0x15;          API_WRITER[3]=0x00;
  API_WRITER[4]=0x1A;          API_WRITER[5]=0x40;
  API_WRITER[6]=0x00;          API_WRITER[7]=0x00;
  API_WRITER[8]=0X28;       
*/
  API_WRITER[0]=0X11;          API_WRITER[1]=0x89;   //不相同
  API_WRITER[2]=0x89;          API_WRITER[3]=0x00;
  API_WRITER[4]=0x1A;          API_WRITER[5]=0xff;
  API_WRITER[6]=0xff;          API_WRITER[7]=0x00;
  API_WRITER[8]=0X2b;
  API_SET_PROPERTY_X(Group_MODEM,0X09,ad_MODEM_AGC_WINDOW_SIZE,API_WRITER);
  
  //RF_MODEM_OOK_CNT1_11
 /* API_WRITER[0]=0xA4;          API_WRITER[1]=0x03;   //不相同
  API_WRITER[2]=0xD6;          API_WRITER[3]=0x03;
  API_WRITER[4]=0x00;          API_WRITER[5]=0xCC;
  API_WRITER[6]=0x01;          API_WRITER[7]=0x80;
  */
  API_WRITER[0]=0xA4;          API_WRITER[1]=0x02;   //不相同
  API_WRITER[2]=0xD6;          API_WRITER[3]=0x83;
  API_WRITER[4]=0x01;          API_WRITER[5]=0x33;
  API_WRITER[6]=0x01;          API_WRITER[7]=0x80;
 // API_WRITER[8]=0XFF;           API_WRITER[9]=0x0C;
  //API_WRITER[10]=0x00; 
  API_SET_PROPERTY_X(Group_MODEM,0X08,ad_MODEM_OOK_CNT1,API_WRITER);
  
  
 //RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12
  /*
  API_WRITER[0]=0xA2;          API_WRITER[1]=0x81;  //相同
  API_WRITER[2]=0x26;          API_WRITER[3]=0xAF;
  API_WRITER[4]=0x3F;          API_WRITER[5]=0xEE;
  API_WRITER[6]=0xC8;          API_WRITER[7]=0xC7;
  API_WRITER[8]=0xDB;          API_WRITER[9]=0xF2;
  API_WRITER[10]=0x02;         API_WRITER[11]=0x08;
  */
  API_WRITER[0]=0xff;          API_WRITER[1]=0xba;  //相同
  API_WRITER[2]=0x0f;          API_WRITER[3]=0x51;
  API_WRITER[4]=0xcf;          API_WRITER[5]=0xa9;
  API_WRITER[6]=0xC9;          API_WRITER[7]=0xfc;
  API_WRITER[8]=0x1b;          API_WRITER[9]=0x1e;
  API_WRITER[10]=0x0f;         API_WRITER[11]=0x01;
  API_SET_PROPERTY_X(Group_MODEM_CHFLT,0X0C,ad_MODEM_CHFLT_RX1_CHFLT_COE_13,API_WRITER); 
  
  //RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12   //相同
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
 /* API_WRITER[0]=0XC8;          API_WRITER[1]=0xC7;   //相同
  API_WRITER[2]=0xDB;          API_WRITER[3]=0xF2;
  API_WRITER[4]=0x02;          API_WRITER[5]=0x08;
  API_WRITER[6]=0x07;          API_WRITER[7]=0x03;
  API_WRITER[8]=0x15;          API_WRITER[9]=0xFC;
  API_WRITER[10]=0x0F;         API_WRITER[11]=0x00;*/
  API_WRITER[0]=0Xf4;          API_WRITER[1]=0xdb;   //相同
  API_WRITER[2]=0xD6;          API_WRITER[3]=0xdf;
  API_WRITER[4]=0xec;          API_WRITER[5]=0xf7;
  API_WRITER[6]=0xfe;          API_WRITER[7]=0x01;
  API_WRITER[8]=0x15;          API_WRITER[9]=0xF0;
  API_WRITER[10]=0xff;         API_WRITER[11]=0x03;
  API_SET_PROPERTY_X(Group_MODEM_CHFLT,0X0C,ad_MODEM_CHFLT_RX2_CHFLT_COE_7,API_WRITER); 
  
  
  //RF_PA_MODE_4        功率放大器  //相同
  PA_MODE(0x08);
  PA_PWR_LVL(0x7f);
  PA_BIAS_CLKDUTY(0x00);
  PA_TC(0x3d);
   
  
  //RF_SYNTH_PFDCP_CPFF_7     //相同
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
//最低频率470.1M
//速率40kbps,频率偏移20KHz
/***********************************************************************/
/*            开机初始化RF至standby*/
/***********************************************************************/
void InitRFChipB470_Baud40000(void)    
{
    unsigned char API_WRITER[16];  //命令缓存
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
  INT_CTL_ENABLE(0x00);  //默认关闭所有的中断
//INT_CTL_PH_ENABLE(0x18);    //0x20
  
  
  //FRR_CTL_MODE            根据实际情况设置
  FRR_CTL_A_MODE(0x04);    //数据包处理挂起中断
  FRR_CTL_B_MODE(0x0a);    //latch RSSI value as defined in MODEM_RSSI_CONTROL
  /*快速反应寄存器C/D可根据实际使用情况进行设定*/
  //FRR_CTL_C_MODE(0X00);
  //FRR_CTL_D_MODE(0X00);
  
  //PREAMBLE
  PREAMBLE_TX_LENGTH(0X08);  //0x0a
  PREAMBLE_CONFIG(0X31);
  /*0x14与0x0f为寄存器默认值，所以这里不需要设置，如果需要对
   寄存器进行部分改变可修改此处*/
 // PREAMBLE_CONFIG_STD1(0X14);  //RX前导码接受门限
  //PREAMBLE_CONFIG_STD2(0X0F);  //RX接受前导码配置
  
  //SYNC
  SYNC_CONFIG(0X01);
  SYNC_BITS_31_24(0XB4);
  SYNC_BITS_23_16(0X2B);
  
  
  //PKT_CRC_CONFIG  CRC种子选择
  PKT_CONFIG1(0x02);
  PKT_CRC_CONFIG(0x85);
  PKT_TX_THRESHOLD(0x30);      //发送快空门限值  48字节
  PKT_RX_THRESHOLD(0x30);      //接受快满门限值  48字节
#ifdef LengthFieldInTwoByte
   PKT_LEN(0x3a);               //长度字节配置成为两个字节
#else
   PKT_LEN(0x2a);               //长度字节配置成为一个字节
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
  
  //modem data rate         数据率
  MODEM_DATA_RATE_2(0X0c);        //40kbps
  MODEM_DATA_RATE_1(0X35);                
  MODEM_DATA_RATE_0(0X00);
  MODEM_TX_NCO_MODE_3(0X09);                
  MODEM_TX_NCO_MODE_2(0Xc9);                
  MODEM_TX_NCO_MODE_1(0Xc3);                
  MODEM_TX_NCO_MODE_0(0X80);
  
  //modem deviation   频偏
  MODEM_FREQ_DEV_2(0x00);
  MODEM_FREQ_DEV_1(0x05);
  MODEM_FREQ_DEV_0(0x76);
  
  
  /*
  形参：group：寄存器所属的组
      num_pro：需要连续设置寄存器的个数
     start_pro：设置寄存器组的初始地址
     *Data：寄存器设置的数据
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
  
  //RF_PA_MODE_4  功率放大器
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
  FREQ_CONTROL_INTE(0x3d);     //中心频点470.1MHz  步长：200khz
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
配置RF工作在相应的频段
*************************************************/
void InitRFChipWorkBand()
{
          // InitRFChipB470_Baud4000();
             InitRFChipB470_Baud40000();
}

/*********************************************************************/
/*  发送数据包，                                                     */
/*  函数参数:                                                        */
/*   char *txBuffer: 待发送的数据；                                  */
/*   int size：存放txBuffer中总共需要发送的数据长度                  */
/*   待发送的数据长度不超过 255-5 ＝250字节                          */
/*  函数返回值 ：0: 发送成功;-1：发送失败;-2:数据长度太长;           */
/*  发送完毕后自动进入接收态                                         */
/*********************************************************************/
 int RFSendPacket(unsigned char *txBuffer, int size)
 {
#ifdef LengthFieldInTwoByte
   if(size>8187)
     return -2;
   unsigned int MaxSize=8187;   //标记所能够发送的最长数据
   unsigned char TxBuffer[8191]; //发送数据缓存  
#else
   if(size>251)
     return -2;
   unsigned int MaxSize=251;    //标记所能够发送的最长数据
   unsigned char TXBuffer[255]; //发送数据缓存
#endif
   unsigned char IntSta[8];     //存储中断标志
   unsigned int  DataSent=0;
   unsigned char length=size+4;
   unsigned char ret[2];

   //这里所发送的数据帧格式有一定的要求，第一个字节要为数据帧的总长度
   //当数据包配置PKT_LEN(??)字段改变时，格式要跟着改变，详情参考开发手册
#ifdef LengthFieldInTwoByte        //两个字节表示数据帧长度，能够发送大于255字节的数据
     for(int i=2;i<size;i++)
   {
     TXBuffer[i+4]=txBuffer[i];
   }
   
    TXBuffer[0]=length/256;
    TXBuffer[1]=length%256;
   if(g_uchConfigFlag==0)
   {
      TXBuffer[2] = g_PHYDevInfo.IntegratorID.ucIntegratorID[0];   //集成商ID
      TXBuffer[3] = g_PHYDevInfo.IntegratorID.ucIntegratorID[1];
      TXBuffer[4] = g_PHYDevInfo.ProjectID.ucProjectID[0];          //设备工程ID
      TXBuffer[5] = g_PHYDevInfo.ProjectID.ucProjectID[1];
     
   }
   else
   {
     TXBuffer[5]=TXBuffer[2]=TXBuffer[3]=TXBuffer[4]=0x88;
   }
   PKT_FIELD_2_LENGTH_7_0(TXBuffer[1]-1);         //字段2的长度设置
   PKT_FIELD_2_LENGTH_12_8(TXBuffer[0]);          //字段2的长度设置
  
#else                           //一个字节表示数据帧的长度
   for(int i=1;i<size;i++)
   {
     TXBuffer[i+4]=txBuffer[i];
   }
   
    TXBuffer[0]=length;
   if(g_uchConfigFlag==0)
   {
      TXBuffer[1] = g_PHYDevInfo.IntegratorID.ucIntegratorID[0];   //集成商ID
      TXBuffer[2] = g_PHYDevInfo.IntegratorID.ucIntegratorID[1];
      TXBuffer[3] = g_PHYDevInfo.ProjectID.ucProjectID[0];          //设备工程ID
      TXBuffer[4] = g_PHYDevInfo.ProjectID.ucProjectID[1];
     
   }
   else
   {
     TXBuffer[1]=TXBuffer[2]=TXBuffer[3]=TXBuffer[4]=0x88;
   }
   PKT_FIELD_2_LENGTH_7_0(length-1);         //字段2的长度设置
#endif


 //这里不要一次性填满FIFO，如果填满会导致FIFO溢出，使得数据发送不正确
   if(size<=50)      //最少FIFO中会剩余10个字节的空位
   {
       API_ClearTxFIFO();
       INT_CTL_ENABLE(0x01);                  //开启数据包中断
       INT_CTL_PH_ENABLE(0x20);               //开启数据发送完毕中断
       API_WriteTxDataBuffer(length,TXBuffer);      //注意这里写入的是length
       API_FIFO_INFO(ret);
       SetRFStartTx();               //开始发送
     while(GET_NIRQ_BIT);
       
     API_FIFO_INFO(ret);  
     API_Get_All_IntStatus(0x08,IntSta);    //清除中断标志
        if(IntSta[3]&0x20)
        {
          return 0;   //发送成功
        } 
        else
        {
          return -1;     //发送失败
        }
       
   }
   else if(size<=MaxSize)
   {
     size-=50;
     DataSent=54;            //一次最多写入54字节到FIFO中
     INT_CTL_ENABLE(0x01);       //开启数据包中断
     INT_CTL_PH_ENABLE(0x02);    //开启FIFO快空门限
     API_WriteTxDataBuffer(54,TXBuffer);
     API_FIFO_INFO(ret);
     API_Get_All_IntStatus(0x08,IntSta);    //清除中断标志
     SetRFStartTx();    //开始发送
     while(size>48)
     {
         while(GET_NIRQ_BIT);  //等待中断的产生 
        API_WriteTxDataBuffer(48,TXBuffer+DataSent);
        DataSent=DataSent+48;
        size=size-48;
        API_ClearPHInt();
     }
        while(GET_NIRQ_BIT);        //等待中断的产生 
        API_WriteTxDataBuffer(size,TXBuffer+DataSent);
        INT_CTL_PH_ENABLE(0x20);    //数据发送完毕中断
        while(GET_NIRQ_BIT);
        API_Get_All_IntStatus(0x08,IntSta);    //清除中断标志
        if(IntSta[3]&0x20)
        {
          return 0;   //发送成功
        } 
        else
        {
          return -1;     //发送失败
        }
        
   } 
   return -1;
 }
/****************************************************************************/
/*  接收数据包，长度不限                                                    */
/*  函数参数: unsigned char *rxBuffer 存放接收的数据                        */ 
/*                                          ,rxBuffer[0] 存放有效数据包大小 */
/*  函数返回值 ：0: 接收成功；
                -1：工程地址不匹配;
                -2: 没有收到任何数据                         
                -3:收到数据但是CRC校验出错                                  
                -4:收到同步字和部分数据,但数据接收超时失败；                */
/****************************************************************************/  
 int RFReceivePacket(unsigned char *rxBuffer)
{
   unsigned char IntSta[8];        //存储中断标志
   unsigned int  offset=0;         //数据偏移
   unsigned int  HaveReadLen=0;    //已接受的数据包长度
   unsigned int  DataLength=0;     //数据包的长度
   unsigned char flag=0;           //第一包数据的标志
   unsigned char ReadLen=0;        //实际一次读取长度
   unsigned char OneTimeLen = 48;  //设定一次读取数据长度 
   unsigned char IDcheck[4];       //校验ID
       
    //1个时钟周期(1/PHY_MCLK)秒,
    unsigned long time = PHY_MCLK_1MS;    //当无线数据传输速率降低的时候，需要修改采样时间
    time=time*20;
    unsigned long  Timeout=time;
#ifdef  LengthFieldInTwoByte
    PKT_FIELD_2_LENGTH_7_0(0xff);    //最大接收数据的设置 
    PKT_FIELD_2_LENGTH_12_8(0x1f);    //最大接收数据的设置 
#else
    PKT_FIELD_2_LENGTH_7_0(255);    //最大接收数据的设置 
#endif
     
    
   while(--Timeout)
   {
     if(GET_NIRQ_BIT==0)
     {
       API_GetFastResponseRegister(0x50,0x01,IntSta);   //读取中断标志位
       API_ClearPHInt();                                //清除数据包中断
   
       
       if(IntSta[0]&0x80)   //CRC校验出错
       {
         API_ClearRxFIFO();
         return -3;
       }
       
       
       if(IntSta[0]&0x10)   //数据包接受完毕
       {
         if(flag==0)  //第一包数据
         {
            //先读取数据帧长度字节，在读取工程ID
#ifdef    LengthFieldInTwoByte
             API_ReadRxDataBuffer(0x02,rxBuffer); //读取两个字节的数据帧长度
             API_ReadRxDataBuffer(0x04,IDcheck);  //工程ID,集成商ID
             DataLength=rxBuffer[0]*256+rxBuffer[1]-4; //数据包的长度
             rxBuffer[0]=DataLength/256;               //数据帧长度的高字节
             rxBuffer[1]=DataLength%256;               //数据帧长度的低字节
#else
             API_ReadRxDataBuffer(0x01,rxBuffer); //读取数据帧长度
             API_ReadRxDataBuffer(0x04,IDcheck);  //工程ID,集成商ID
             DataLength=rxBuffer[0]-4;            //去除工程ID的数据帧的总长度
             rxBuffer[0]=DataLength;              //数据帧长度
#endif
            
             
             if(DataLength<1)
             {
               API_ClearRxFIFO();  //清除RxFIFO
               API_ClearPHInt();   //清除中断标志
               return -4;
             }
             ReadLen=DataLength-1;
             if(ReadLen>0)
                API_ReadRxDataBuffer(ReadLen,rxBuffer+1);
             offset = ReadLen+1;//在读取缓冲区中的偏移
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
       
       
       if(IntSta[0]&0x01)     //RX_fifo快满
       {
           if(flag==0)   //是第一包数据
           {
             flag=1;
#ifdef  LengthFieldInTwoByte
             API_ReadRxDataBuffer(0x02,rxBuffer); //读取两个字节的数据帧长度
             API_ReadRxDataBuffer(0x04,IDcheck);  //工程ID,集成商ID
             DataLength=rxBuffer[0]*256+rxBuffer[1]-4;
             rxBuffer[0]=DataLength/256;
             rxBuffer[1]=DataLength%256;
#else
             API_ReadRxDataBuffer(0x01,rxBuffer); //读取一个字节的数据帧长度
             API_ReadRxDataBuffer(0x04,IDcheck);  //工程ID,集成商ID
             DataLength=rxBuffer[0]-4;
             rxBuffer[0]=DataLength;
#endif       
             if(DataLength<1)     //数据帧长度错误
             {
                API_ClearRxFIFO();  //清除RX_FIFO
                
                 return -4;
             }
               ReadLen=(DataLength-1)<OneTimeLen-5? (DataLength-1):OneTimeLen-5;
             if(ReadLen>0)
                API_ReadRxDataBuffer(ReadLen,rxBuffer+1);    //读取剩余一次性读取的字节
             offset=ReadLen+1;
             HaveReadLen=ReadLen+5; 
             
              //校验ID
             if(g_uchConfigFlag == 0)//不是配置态 
                {
                   if( (IDcheck[0] != g_PHYDevInfo.IntegratorID.ucIntegratorID[0]) ||
                       (IDcheck[1] != g_PHYDevInfo.IntegratorID.ucIntegratorID[1]) ||
                       (IDcheck[2] != g_PHYDevInfo.ProjectID.ucProjectID[0])       ||
                       (IDcheck[3] != g_PHYDevInfo.ProjectID.ucProjectID[1]) )
                                            
                        {
                            API_ClearRxFIFO();  //清除RX_FIFO
                            
                            return -1; //地址不匹配
                            
                        } //end if  集成商ID或者工程ID不匹配
                   
                  }//end if 不是配置态 
             else//配置态 
                  {
                     if( (IDcheck[0] != 0x88) ||
                         (IDcheck[1] != 0x88) ||
                         (IDcheck[2] != 0x88) ||
                         (IDcheck[3] != 0x88) )                                     
                        { 
                            API_ClearRxFIFO();  //清除RX_FIFO
                                return -1; //地址不匹配                        
                        }        //end if  配置态ID不匹配    
                     
                   }//end else 配置态      
             
             if(DataLength==offset)
             {
               API_ClearRxFIFO();  //清除RxFIFO
               API_ClearPHInt();   //清除中断标志
               return 0;
             }
           }   //end if RX_FIFO快满，第一包数据
           else   //不是第一包数据
           {
              ReadLen=(DataLength-offset)<OneTimeLen ? (DataLength-offset):OneTimeLen;
              API_ReadRxDataBuffer(ReadLen,rxBuffer+offset); 
              offset=offset+ReadLen;           //位移
              HaveReadLen=HaveReadLen+ReadLen;
              if(DataLength==offset)
              {
                  API_ClearRxFIFO();  //清除RxFIFO
                  API_ClearPHInt();   //清除中断标志
                  return 0;
              }
           }    //end else RX_FIFO快满，不是第一包数据
           Timeout = time;//时间重置
           
       }   //end if RX_FIFO快要满
       
     }  //end if 有中断产生
     
   }    //end if while
   return -4;
 }