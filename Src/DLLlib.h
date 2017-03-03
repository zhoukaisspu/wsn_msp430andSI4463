#include "PHYlib.h"                   //物理层头文件
#include "RFlib.h"                    //射频驱动头文件
#include "NETlib.h"


#include "global.h"

#include <stdlib.h>
#include <string.h>


extern unsigned int g_uDLLLogInt; //定义数据链路注册间隔时间： 以秒为单位

//链路质量定义
#define DLL_LINK_QUAL     98          //设定默认的网络通讯质量的较优值
extern unsigned char g_uchDLLQ;        //数据链路层全局变量，设定默认的网络通讯质量的较优值
extern unsigned char g_uchDLLCurLQ;    //本级网络链路质量
extern unsigned char g_uchDLLSupLQ;    //上级网络链路质量

#define DLL_MAX_STAUP      100        //数据链路层全局变量，网络上行稳定量化最大值
extern unsigned int g_uDLLStaUp;           //数据链路层全局变量，网络上行稳定量化值

#define DLL_MAX_STADOWN      100        //数据链路层全局变量，网络下行稳定量化最大值
extern unsigned int g_uDLLStaDown;         //数据链路层全局变量，网络下行稳定量化值

#define DLL_STAUP_RIGHT     5          //网络上行通讯权重
#define DLL_STADOWN_RIGHT     5         //网络下行通讯权重


extern unsigned char g_uchDLLCentreLQ;     //中心基站网络链路质量


#define DLL_MAX_BUF0 128     //===UART0的buffer字节最多存储多少数据
extern unsigned char  g_uchDLLBufferUart0[DLL_MAX_BUF0];
extern unsigned char  g_uchDLLBuffer0Head;
extern unsigned char  g_uchDLLBuffer0Tail;
extern unsigned char  g_uchDLLBuffer0Flag;

#define DLL_MAX_BUF1 128
extern unsigned char  g_uchDLLBufferUart1[DLL_MAX_BUF1];
extern unsigned char  g_uchDLLBuffer1Head;
extern unsigned char  g_uchDLLBuffer1Tail;
extern unsigned char  g_uchDLLBuffer1Flag;

#define DLL_MAX_BUF3 128
extern unsigned char  g_uchDLLBufferUart3[DLL_MAX_BUF3];
extern unsigned char  g_uchDLLBuffer3Head;
extern unsigned char  g_uchDLLBuffer3Tail;
extern unsigned char  g_uchDLLBuffer3Flag;


extern unsigned char g_uchDLLUartSync;                 //Uart同步检验值
extern const unsigned char g_cuchDLLUartSyncData[3];   //Uart同步字序列

/****************************帧缓冲定义*****************************/
#define BUFF_MAXPAGE  6                             //定义缓冲区页面数量
#define PAGESIZE  64                                //发送缓冲区页大小
#define FRAME_MAX_SEND_TIMES 10                     //帧最大尝试发送的次数

#define FRM_INUSE 0x80
#define FRM_TACK  0x40
#define FRM_RES   0x20
#define FRM_BEGIN 0x10
extern unsigned char g_uchWirelessTxBuf[BUFF_MAXPAGE][PAGESIZE] ;      //发送缓冲区定义

//帧缓冲控制字
//Bit位	7	:           1：帧有效         0：帧无效;
//Bit位	6       :           1：需要总回复     0：不需要总回复 
//Bit位	5       : 暂时保留无意义         	
//Bit位	4       :帧类型     1：起始帧         0：被分块的中间帧
//Bit位	3~0     : 帧尝试发送的次数，最大为FRAME_MAX_SEND_TIMES，超过FRAME_MAX_SEND_TIMES还没发送成功，则设置帧无效
extern unsigned char g_uchWirelessTxBufCtl[BUFF_MAXPAGE];     //帧缓冲控制字

extern unsigned char g_uchWirelessRxBuf[256];                  //接收帧缓冲

extern int g_iWirelessRxFlag;//无线接收数据标记

//前信息帧编号参数
extern unsigned char g_uchDLLFrameNum ;//帧编号


//通讯时间片定义
#define DLL_SLICE_TIME  2   //设备通讯竞争最小时间片，一般长度的信息处理时间+信息准备时间+接收转发送时间，单位ms
#define DLL_ACK_TIME    30  //设备发送数据帧后，等待确认帧的时间，单位ms
#define DLL_TR_TIME     300 //设备一次通讯，发送和接收需要的时间，单位ms

//避碰延迟时间定义 
extern unsigned int g_uDLLCATime;     //设备通讯发生避碰后所需要的延迟时间，范围（DLL_TR_TIME~10*DLL_TR_TIME）



/****************************************************************************/
/*   通过UART发送 数据c                                                   */
/*   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3               */
/*                unsigned char c , 为待发送数据                          */
/*   函数返回值：int 型，0：发送成功；1：所选的串口未打卡；2：无此串口    */
/****************************************************************************/
extern int UartSend(unsigned char com,unsigned char c );

/****************************************************************************/
/*   Uart发送系统同步字                                                   */
/*   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3               */
/*   函数返回值：int 型，0：发送成功；1：所选的串口未打卡；2：无此串口    */
/****************************************************************************/
extern int UartSendSync(unsigned char com);


/****************************************************************************/
/*     从Uart的接收缓冲区中获取一个数据给 参数C,成功返回0，否则-1           */
/*   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3                 */
/****************************************************************************/
extern int GetUartData(unsigned char com ,unsigned char* c);

/****************************************************************************/
/*     Uart的接收缓冲区清空                                               */
/*   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3                 */
/****************************************************************************/
extern void UartRxBufferClear(unsigned char com);

/****************************************************************************/
/*  参数unsigned int overtime设定了接收无线数据包的时间限制                 */
/*  函数返回值 ：0: 接收成功；数据保存在全局变量g_uchWirelessRxBuf
                -1：工程地址不匹配;
                -2: 没有收到任何数据                         
                -3:收到数据但是CRC校验出错                                  
                -4:收到同步字和部分数据,但数据接收超时失败；                */
/****************************************************************************/ 
extern int GetWirelessPacket(unsigned int overtime);

/****************************************************************************/
/*     无线通讯接收缓冲区清空                                               */
/****************************************************************************/
extern void WirelessRxBufferClear(void);

/*****************************************************************************/
/* 产生CRC校验码的函数，参数DataBuff是要校验的数据，或者需要生成校验码的数据 */
/* 注意：DataBuff默认最后两字节存放CRC校验码，len是添加校验码后的DataBuff大小*/
/* 函数返回值：unsigned int型CRC码数据                                       */
/*****************************************************************************/
extern unsigned int GenerateCRC(unsigned char *DataBuff,unsigned int len);

/*****************************************************************************/
/* 产生帧的CRC字节                                                           */
/*  TxData为要添加CRC校验的数据帧，其中TxData[0]表示待发送数据帧的长度       */
/*  TxData最后两个地址存放CRC校验后的值                                      */
/*****************************************************************************/
extern void AddCRC(unsigned char *TxData);

/****************************************************************************/
/*获取Uart同步字头, 同步字由全局变量数组 g_uchDLLWireSyncData定义         */
/*        函数返回值: int 类型，  -1----同步失败 ,  0----同步成功           */
/*   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3                 */
/****************************************************************************/
extern int GetUartSyncWord(unsigned char com);

/*****************************************************************************/
/* 在限定时间内接收Uart信息帧：
   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3                  */
/*                 RxData存放收到的数据,RxData[0]为收到数据长度              */
/*                           TimeLimit存放限定时间，单位毫秒                 */
/* 函数返回值： 0 －－收到数据帧，并存储到RxData                             */
/*             -1－－超时未收到有效数据                                      */
/*****************************************************************************/
extern int UartRxFrameData(unsigned char com,unsigned char *RxData, unsigned int TimeLimit);

/*****************************************************************************/
/* Uart发送数据帧
   参数   :     unsigned char com 选择发送数据的串口，可选值有：
                                     UARTA0，UARTA1，UARTA3                  */
/*                 TxData存放即将发送的数据，TxData[0]存放发送数据长度     */
/*   函数返回值：int 型，0：发送成功；1：所选的串口未打卡；2：无此串口    */
/*****************************************************************************/
extern int UartTxFrameData(unsigned char com,unsigned char *TxData);

/*****************************************************************************************/
/*获取随机的初始频点序列                                                                 */
/*生成的频点序列放入：A区：uchFreNumA[RF_FRE_AMNUM_A]                                    */
/*                   个数：0~(RF_FRE_AMNUM_A-1)                                          */
/*                    B区：uchFreNumB[RF_FRE_AMNUM_B]                                    */
/*                    个数:RF_FRE_AMNUM_A ~ (RF_FRE_AMNUM_A+RF_FRE_AMNUM_B-1)            */
/*以设备MAC地址+系统时钟为随机数种子                                                     */
/*****************************************************************************************/
extern void GetRandomFreList();

/*****************************************************************************/
/* 异级信道竞争机制算法 ，函数形参：当前竞争的异级频点，可能是上级频点       */
/*                                                    也可能是下级频点       */
/*      信道竞争成功：异级频点转入发射态占用此频点，并返回 0                 */
/*      信道竞争失败：本级频点转入接收态，并返回 -1                          */
/*****************************************************************************/
extern int DiffChDetect(unsigned char uchFreNo);

/*****************************************************************************/
/* 本级信道竞争机制算法 ，函数形参：                                         */
/*     信道竞争成功：调整进入发射态，占用频点，并返回 0                      */
/*     信道竞争失败：                            返回 -1 （处于接收态）      */

/*****************************************************************************/
extern int SimiChDetect(unsigned char uchCurFreNum);

/********************************************************************************/
/*与ID为DevID的设备解除拓扑关系　　　　　　　　　　　　　　　　　               */
/********************************************************************************/
extern void ReleaseRelation(unsigned int DevID);

/********************************************************************************/
/*异级通讯　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　          */
/*   信道检测，无干扰，发送网络查询帧，接收确认回复                             */
/*   正确确认  返回 0                                                           */
/*       其他  修改相应信息，返回 -1                                            */
/*unsigned char uchFreNum  即将通讯的异级频点                                   */
/*int BuffFrmNo  待发送帧在帧缓冲中的编号                                       */
/********************************************************************************/
extern int DiffFreqCommunication(unsigned char uchFreNo,int BuffFrmNum);

/********************************************************************************/
/*基站本级通讯                                                                  */
/*   信道检测，无干扰，发送网络查询帧，接收确认回复                             */
/*   正确确认  返回 0                                                           */
/*       其他  修改相应信息，返回 -1                                            */
/* BuffFrmNo：待发送帧在帧缓冲中的编号                                       */
/********************************************************************************/
extern int BaseSelfFreqCommunication(int BuffFrmNum);

/********************************************************************************/
/*端机本级通讯                                                                  */
/*   正确确认  返回 0                                                           */
/*       其他  修改相应信息，返回 -1                                            */
/*   BuffFrmNo：待发送帧在帧缓冲中的编号                                     */
/********************************************************************************/
extern int TermSelfFreqCommunication(int BuffFrmNum);

/****************************************************************************/
/* 　　　　　　　　　　    寻上级自适应选频算法   　    　　        	    */
/*                  搜频成功      返回  0                                   */
/*                  搜频不成功    返回  -1                                   */
/* uchLimitGrade  为级别限制 ，若有级别限制则为实际级别限制值                */
/*                             若无级别限制则暂定为 0xff                     */
/****************************************************************************/
extern int SearchParentFreq(unsigned char LimitGrade);

/****************************************************************************/
/* 　　　　　　　　 确定本级工作频点的自适应选频程序   　            	    */
/*                  搜频成功      返回  0                                   */
/*                  搜频不成功    返回  -1                                  */
/****************************************************************************/
extern int SearchSelfFreq();

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
extern int SubDevLog(unsigned char uchFreNum,unsigned int tempNetID,int NetLev, unsigned char destPAtype);

/*****************************************************************************/
/*       设备链路质量评估 ,计算出本级链路质量                               */
/* 函数参数：reflq----参考链路质量值，即上级网络链路质量                    
rssi-----通讯的rssi检测值，非功率值                               
netcap---上级设备当前注册的设备数量 
stab－－－上行网络稳定值                                        */
/*****************************************************************************/
extern void LQEvaluation(unsigned char reflq,unsigned char rssi, unsigned char netcap, unsigned char stab);