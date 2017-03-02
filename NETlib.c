/*****************************************************************
网络层解决的是数据传输问题，如何从一个端机传递到另一个端机
*****************************************************************/
#include "NETLib.h"

#include "PHYLib.h"


#include "RFlib.h"

#include "DLLLib.h"

#include "APPLib.h"

#include "global.h"

/*****************************************************************************************/
/*            缓冲区处理,得到缓冲空页。同时设置帧缓冲控制字　　　 　　　　　　           */
/* FinalAckFlag形成的帧是否需要总回复(0, 不要要，1，需要）,FrameLength 需要形成的帧的长度*/
/* 函数返回值： -1:页空间小于2或者空间不够                                               */
/*              其他：当前可用的页首地址                                                 */
/*****************************************************************************************/
int  GetFreePageNo(unsigned char FeedbackFlag,int FrameLength)
{
    // 最多只有一帧需要反馈的帧
    if( FeedbackFlag==1)     
    {
      for(int i=0;i<5;i++)
        // 1101 0000 bit4 bit6 bit7都有效
	if( (g_uchWirelessTxBufCtl[i]  &
             (FRM_INUSE+FRM_TACK+FRM_BEGIN)) == (FRM_INUSE+FRM_TACK+FRM_BEGIN))  
	    return -1;
    }
    
    int uUseableBuffPage = 0;//第一个可用的发送缓冲区页面
    
    int pagenum = 0;//数据占用的页面数
    
    
    unsigned char txBufCtl[BUFF_MAXPAGE];
    
    for(char count=0; count<BUFF_MAXPAGE; count++) //预先备份
    {
        txBufCtl[count] = g_uchWirelessTxBufCtl[count];
        g_uchWirelessTxBufCtl[count] = 0;
    }
    
    for(char count=0; count < BUFF_MAXPAGE; )
    {
        //帧有效标志为1  且为起始页 1001 0000
        if((txBufCtl[count] & (FRM_INUSE+FRM_BEGIN)) == 
                              (FRM_INUSE+FRM_BEGIN)) 
	{
            unsigned char DataLen = g_uchWirelessTxBuf[count][0];
            unsigned char *dst = g_uchWirelessTxBuf[uUseableBuffPage];
            unsigned char *src = g_uchWirelessTxBuf[count];
            
            //若当前可用帧页地址对应，则避免重复赋值
            if(uUseableBuffPage != count)
            {            
              for(int i = 0; i< DataLen ;i++)
                dst[i] = src[i];  
            }
            
            pagenum = DataLen/PAGESIZE + 1;//数据占用的页面数
            
            if(pagenum > BUFF_MAXPAGE)//数据占用的页面数大于最大页面，出错
              return -1;
            
            for(int pagenum1 = 0; pagenum1 < pagenum; pagenum1++ )
              g_uchWirelessTxBufCtl[uUseableBuffPage+pagenum1] = 
                                          txBufCtl[count+pagenum1]; //复制标志
            
            uUseableBuffPage +=pagenum;
            count +=pagenum;
            
       }//end if 
       else
	  count++;
    } //end for
	
    pagenum = FrameLength/PAGESIZE + 1;//即将填入的数据占用的页面数
    
    if( (uUseableBuffPage + pagenum + 1 ) >BUFF_MAXPAGE )//可用页面地址+即将占用的页面+1 >最大页面
	return -1;
	
    return(uUseableBuffPage); //返回当前可用的页首地址 ,并已经对页控制参数做了修改
}


/****************************************************************************/
/* 　　　　　　　　　　      形成简单(复杂)查询帧   　    　　        	    */
/*        QueryType 0x10 简单查询,  QueryType 0x20 复杂查询                 */
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */
/****************************************************************************/
int GenerateQueryFrame(unsigned char QueryType)
{	
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
      
    //填充帧头     
    g_uchWirelessTxBuf[FreePageNo][0] = QUERY_FRAME_LENGTH;// 帧长度
    g_uchWirelessTxBuf[FreePageNo][1] = g_uchWirelessTxBuf[FreePageNo][2] = 0xff;//本帧目的
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff; //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;     
    
    //最终目的最终源  
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];    
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                     //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum;     //帧编号
    g_uchWirelessTxBuf[FreePageNo][11] = QueryType;            //帧控制字1   0x10 简单查询,0x20 复杂查询
    g_uchWirelessTxBuf[FreePageNo][12] = 0;                   //帧控制字2  任意填充 
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);         //添加CRC
    
    return FreePageNo;
}

/****************************************************************************/
/* 　　　　　　　　　　      形成简单查询肯定回复帧       　　        	    */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int GenerateOkAck_SimpleQueryFrame(unsigned char *FirstDestID,unsigned char AckFrameNo)
{
	
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
	
	//填充帧头         
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_SIMPLEQUERY_FRAME_LENGTH;  // 简单回复帧长度
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];            //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;   //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    
    //memcpy(g_uchWirelessTxBuf[FreePageNo]+5,g_uchWirelessTxBuf[FreePageNo]+1,4);     //最终目的最终源
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                     //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;          //帧编号
    g_uchWirelessTxBuf[FreePageNo][11] = ACK;              //回复帧控制字1   
    g_uchWirelessTxBuf[FreePageNo][12] = INFO_SIMPLE_QUERY;   //帧控制字2   回复对象：0x10 简单查询,0x20 复杂查询
	
    //填充数据
    g_uchWirelessTxBuf[FreePageNo][13]= g_uchNetLev; //网络级别
    g_uchWirelessTxBuf[FreePageNo][14]= g_uchPHYPAType;  //功放类型,定义为一个字节
    g_uchWirelessTxBuf[FreePageNo][15]= g_PHYDevInfo.PowerType;//g_eAppPowerType;    //电源类型 
    g_uchWirelessTxBuf[FreePageNo][16]= g_eAppPowerStatus;   //能量状态
    g_uchWirelessTxBuf[FreePageNo][17]= g_uchRFPowerVal[g_uchRFPALev];  //设备功率
    g_uchWirelessTxBuf[FreePageNo][18]= g_uchDLLCurLQ;    //本级链路质量
    g_uchWirelessTxBuf[FreePageNo][19]= (g_uchDLLSubDevNumber/16)+(g_uchDLLSubDevNumber&0x0f);  //申请子设备个数+正常工作子设备个数
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);                   //添加CRC
    
    return FreePageNo;  
}


/****************************************************************************/
/* 　　　　　　　　　　      形成复杂查询肯定回复帧       　　        	    */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int GenerateOkAck_ComplexQueryFrame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{
	
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
	
	//填充帧头          
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_COMPLEXQUERY_FRAME_LENGTH;   //复杂查询确认帧长度:0x19+3*(g_uchDLLSubDevNumber&0x0f)
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];            //本帧目得
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;   //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    
    //最终目的最终源
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;      //帧编号
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;           //回复帧控制字1 
    g_uchWirelessTxBuf[FreePageNo][12]= INFO_COMPLEX_QUERY;//帧控制字2   回复对象：0x10 简单查询,0x20 复杂查询
    //填充数据    
    g_uchWirelessTxBuf[FreePageNo][13]= g_uchNetLev; //网络级别
    g_uchWirelessTxBuf[FreePageNo][14]= g_uchPHYPAType;  //功放类型,定义为一个字节
    g_uchWirelessTxBuf[FreePageNo][15]= g_PHYDevInfo.PowerType;//g_eAppPowerType;    //电源类型 
    g_uchWirelessTxBuf[FreePageNo][16]= g_eAppPowerStatus;   //能量状态
    g_uchWirelessTxBuf[FreePageNo][17]= g_uPHYChargePower/16;  //充电电压
    g_uchWirelessTxBuf[FreePageNo][18]= g_uPHYBattPower/16;    //电池电压
    g_uchWirelessTxBuf[FreePageNo][19]= g_uPHYBoardPower/16;    //板级电压
    g_uchWirelessTxBuf[FreePageNo][20]= g_uchRFPowerVal[g_uchRFPALev];  //设备功率
    g_uchWirelessTxBuf[FreePageNo][21]= g_uchDLLCurLQ;  //本级链路质量   
    
    //正常工作子设备信息    
    int i=0,j=0;
    
    for(;i<NET_MAX_CAP;i++)
    {
	if( ( g_SubDevInfo[i].Status==1) || (g_SubDevInfo[i].Status==4)) //正常工作 + 认证成功
	{
            g_uchWirelessTxBuf[FreePageNo][23+j*3]=g_SubDevInfo[i].NetID&0xff;
            g_uchWirelessTxBuf[FreePageNo][24+j*3]=g_SubDevInfo[i].NetID/256;
            g_uchWirelessTxBuf[FreePageNo][25+j*3]=g_SubDevInfo[i].PowerStatus;
            j++;
	}
    } 
    
    g_uchDLLSubDevNumber = ( (g_uchDLLSubDevNumber&0xf0) | (j&0x0f) ) ;//重新给设备个数赋值
    
    g_uchWirelessTxBuf[FreePageNo][22]= g_uchDLLSubDevNumber & 0x0F ;  //正常工作子设备个数
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_COMPLEXQUERY_FRAME_LENGTH; //重新填充帧长 
     
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    return FreePageNo;
}

/****************************************************************************/
/* 　　　　　　　　　　      申请注册设备形成申请注册帧   0x30    　        */
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */
/* 只发送一次，不成功即丢弃                                                 */
/****************************************************************************/
int GenerateApplyLogFrame(unsigned char *FirstDestID)
{
	int FreePageNo = BUFF_MAXPAGE -1;//最后一页
        
	//填充帧头   
	g_uchWirelessTxBuf[FreePageNo][0] = APPLY_FRAME_LENGTH; //非转发申请注册帧帧长
	g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //本帧目的
	g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
	g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
	g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
	g_uchWirelessTxBuf[FreePageNo][5] = 0x00;                //最终目得
        g_uchWirelessTxBuf[FreePageNo][6] = 0x00;
        g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //初始源
        g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
	g_uchWirelessTxBuf[FreePageNo][9] = 0;      //路由个数
	g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum; //帧编号
	g_uchWirelessTxBuf[FreePageNo][11] = INFO_APPLY;  //申请注册帧
	g_uchWirelessTxBuf[FreePageNo][12] = 0;  //第二个控制字，0填充
	g_uchWirelessTxBuf[FreePageNo][13] = g_uchPHYPAType;  //功放类型,定义为一个字节
	g_uchWirelessTxBuf[FreePageNo][14] = ( g_PHYDevInfo.DevType & 0x0F ) + ( (g_uchAppchRoleSet & 0x0F) * 16 ) ;  //配置设备类型 当前设备类型 
	
	//设备功率根据档位选择
	g_uchWirelessTxBuf[FreePageNo][15] = g_uchRFPowerVal[g_uchRFPALev];  //设备功率
	
        //申请注册设备网络ID
	g_uchWirelessTxBuf[FreePageNo][16] = g_uchWirelessTxBuf[FreePageNo][3];
        g_uchWirelessTxBuf[FreePageNo][17] = g_uchWirelessTxBuf[FreePageNo][4];
	 
        // 申请注册设备MAC地址 
	g_uchWirelessTxBuf[FreePageNo][18] = g_PHYDevInfo.DevMAC[0];
        g_uchWirelessTxBuf[FreePageNo][19] = g_PHYDevInfo.DevMAC[1];
        g_uchWirelessTxBuf[FreePageNo][20] = g_PHYDevInfo.DevMAC[2];
        g_uchWirelessTxBuf[FreePageNo][21] = g_PHYDevInfo.DevMAC[3];
	g_uchWirelessTxBuf[FreePageNo][22] = g_PHYDevInfo.DevMAC[4];
        g_uchWirelessTxBuf[FreePageNo][23] = g_PHYDevInfo.DevMAC[5];
        g_uchWirelessTxBuf[FreePageNo][24] = g_PHYDevInfo.DevMAC[6];
        g_uchWirelessTxBuf[FreePageNo][25] = g_PHYDevInfo.DevMAC[7];
        	
	AddCRC(g_uchWirelessTxBuf[FreePageNo]);    
	
	return FreePageNo;
}

/****************************************************************************/
/* 　　　　　　　　　　      形成申请注册等待认证回复帧    　    　　        */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*            MAC:存放发送申请帧的MAC地址                                   */
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int GenerateWaitCheckACK_ApplyLogFrame(unsigned char* FirstDestID,unsigned char AckFrameNo,unsigned char *MAC)
{
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
	
	//填充帧头        
	g_uchWirelessTxBuf[FreePageNo][0] = WAITCHECKACK_APPLY_FRAME_LENGTH;     //申请注册等待认证回复帧长度 0x1a 
	g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //本帧目的
	g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
	g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
	g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
	//最终目的最终源
	g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
        g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
        g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
        g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
        
	g_uchWirelessTxBuf[FreePageNo][9] = 0;      //路由个数
	g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //帧编号
    
	g_uchWirelessTxBuf[FreePageNo][11]= ACK;            //  帧控制字1，0x55
	g_uchWirelessTxBuf[FreePageNo][12]= CHECK_WAIT_ACK; //  帧控制字2，0x31
    
	//申请注册设备网络ID
	g_uchWirelessTxBuf[FreePageNo][13] = g_uchWirelessTxBuf[FreePageNo][1];
        g_uchWirelessTxBuf[FreePageNo][14] = g_uchWirelessTxBuf[FreePageNo][2];
                                
        // 申请注册设备MAC地址   
	g_uchWirelessTxBuf[FreePageNo][15] = MAC[0];
        g_uchWirelessTxBuf[FreePageNo][16] = MAC[1];
        g_uchWirelessTxBuf[FreePageNo][17] = MAC[2];
        g_uchWirelessTxBuf[FreePageNo][18] = MAC[3];
	g_uchWirelessTxBuf[FreePageNo][19] = MAC[4];
        g_uchWirelessTxBuf[FreePageNo][20] = MAC[5];
        g_uchWirelessTxBuf[FreePageNo][21] = MAC[6];
        g_uchWirelessTxBuf[FreePageNo][22] = MAC[7];
        
        //设定目的设备注册间隔  
        g_uchWirelessTxBuf[FreePageNo][23] = 60;//等待认证的时间间隔 以秒为单位

	AddCRC(g_uchWirelessTxBuf[FreePageNo]);
	
	return FreePageNo;
}

/****************************************************************************/
/* 　　　　　　　　　　      形成注册认证通过回复帧    　    　　           */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*            MAC:存放发送申请帧的MAC地址                                   */
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int GenerateCheckPassACK_ApplyFrame(unsigned char *FirstDestID, unsigned char AckFrameNo, unsigned char *MAC) 
{
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
	
    //填充帧
    g_uchWirelessTxBuf[FreePageNo][0] = CHECKPASSACK_APPLY_FRAME_LENGTH;      //0x1C
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //最终目的最终源
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];	
    
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //帧编号
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;                   //  帧控制字1，0x55
    g_uchWirelessTxBuf[FreePageNo][12]= CHECK_PASS_ACK;    //  帧控制字2，0x32
    
    //申请注册设备网络ID
    g_uchWirelessTxBuf[FreePageNo][13] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][14] = g_uchWirelessTxBuf[FreePageNo][2];	
    
    // 申请注册设备MAC地址 
    g_uchWirelessTxBuf[FreePageNo][15] = MAC[0];
    g_uchWirelessTxBuf[FreePageNo][16] = MAC[1];
    g_uchWirelessTxBuf[FreePageNo][17] = MAC[2];
    g_uchWirelessTxBuf[FreePageNo][18] = MAC[3];
    g_uchWirelessTxBuf[FreePageNo][19] = MAC[4];
    g_uchWirelessTxBuf[FreePageNo][20] = MAC[5];
    g_uchWirelessTxBuf[FreePageNo][21] = MAC[6];
    g_uchWirelessTxBuf[FreePageNo][22] = MAC[7];	
	
	
    g_uchWirelessTxBuf[FreePageNo][23] =  g_uchRFPowerVal[g_uchRFPALev]; //本帧源功率
	
    //设定目的设备应用层注册间隔  
    g_uchWirelessTxBuf[FreePageNo][24]= (g_uchNetLev+1)*2; //先预先设定一个值，预防下面检索不到
    
    //设定目的设备链路层注册间隔  
    g_uchWirelessTxBuf[FreePageNo][25]= 3; //先预先设定一个值，预防下面检索不到
	
    for(int i=0;i<NET_MAX_CAP;i++)
    {
		if( ( g_uchWirelessTxBuf[FreePageNo][1] == ( g_SubDevInfo[i].NetID&0xff ) ) && 
            (g_uchWirelessTxBuf[FreePageNo][2] == (g_SubDevInfo[i].NetID/256)) )
		{
            g_uchWirelessTxBuf[FreePageNo][24]= g_SubDevInfo[i].AppDataIntTime;
            g_uchWirelessTxBuf[FreePageNo][25]= g_SubDevInfo[i].DLLLogIntTime;
            break;
		}
    }
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
	
    return FreePageNo;
}


/****************************************************************************/
/* 　　　　　　　　　　      形成注册认证失败回复帧    　    　　           */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*            MAC:存放发送申请帧的MAC地址                                   */
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int GenerateCheckFailACK_ApplyFrame(unsigned char* FirstDestID,unsigned char AckFrameNo,unsigned char *MAC)
{
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页	
	
    //填充帧
    g_uchWirelessTxBuf[FreePageNo][0] = CHECKFAILACK_APPLY_FRAME_LENGTH;       //注册认证失败回复帧帧长0x19
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //最终目的最终源
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];	
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //帧编号
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;                  //  帧控制字1，0x55
    g_uchWirelessTxBuf[FreePageNo][12]= CHECK_FAIL_ACK ;    //  帧控制字2，0x33
    
    //申请注册设备网络ID
    g_uchWirelessTxBuf[FreePageNo][13] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][14] = g_uchWirelessTxBuf[FreePageNo][2];	
    
    // 申请注册设备MAC地址 
    g_uchWirelessTxBuf[FreePageNo][15] = MAC[0];
    g_uchWirelessTxBuf[FreePageNo][16] = MAC[1];
    g_uchWirelessTxBuf[FreePageNo][17] = MAC[2];
    g_uchWirelessTxBuf[FreePageNo][18] = MAC[3];
    g_uchWirelessTxBuf[FreePageNo][19] = MAC[4];
    g_uchWirelessTxBuf[FreePageNo][20] = MAC[5];
    g_uchWirelessTxBuf[FreePageNo][21] = MAC[6];
    g_uchWirelessTxBuf[FreePageNo][22] = MAC[7];	
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
	
    return FreePageNo;
}


/****************************************************************************/
/* 　　　　　　　　　　      申请注册总回复   0x50    　                    */
/* 函数参数：FirstDestID:存放本帧目的地址；
FrameCmd2：存放认证结果：通过认证0x32/认证失败0x33
ApplyDevID：存放最终目的地址，也是发送申请注册信息的设备地址
ApplyMAC：存放发送申请注册信息的设备MAC地址
AppLogIntTime：设定设备的应用层注册间隔时间
DllLogIntTime:设定设备的链路层注册间隔时间
Router：路由信息，Router[0]是路由个数，依次是路由地址，低位在前*/   
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */        
/****************************************************************************/
int GenerateApplyFeedbackFrame(unsigned char* FirstDestID,unsigned char FrameCmd2,unsigned char* ApplyDevID,
							   unsigned char *ApplyMAC,unsigned char AppLogIntTime, unsigned char DllLogIntTime,unsigned char *Router)
{
        int FrameLength = FINALACK_APPLY_FRAME_LENGTH+2*Router[0]; //帧长度
        int FeedbackFlag = 0;  //帧回复标志
        
	//判断缓冲区是否能填充下   
	int FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
	if(FreePageNo==-1)
          return -1;             //不能填充
        
        //帧控制字设置
        //  1×01 0000 起始页
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1×00 0000 中间页
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }
	
	//填充帧头   
	g_uchWirelessTxBuf[FreePageNo][0] = FINALACK_APPLY_FRAME_LENGTH+2*Router[0]; //申请注册帧总回复
	g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //本帧目的
	g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
	g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
	g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
	g_uchWirelessTxBuf[FreePageNo][5] = ApplyDevID[0];        //最终目的
	g_uchWirelessTxBuf[FreePageNo][6] = ApplyDevID[1];
	g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];  //初始源
	g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
	
	g_uchWirelessTxBuf[FreePageNo][9] = 0;      //路由个数
	g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum; //帧编号
	g_uchWirelessTxBuf[FreePageNo][11] = INFO_FEEDBACK;  //总回复帧
	g_uchWirelessTxBuf[FreePageNo][12] = FrameCmd2;  //  通过还是失败  通过认证0x32/认证失败0x33
	g_uchWirelessTxBuf[FreePageNo][13] = ApplyDevID[0];  //申请设备ID
	g_uchWirelessTxBuf[FreePageNo][14] = ApplyDevID[1];  //
	
	///申请设备的MAC地址	
	g_uchWirelessTxBuf[FreePageNo][15] = ApplyMAC[0];
        g_uchWirelessTxBuf[FreePageNo][16] = ApplyMAC[1];
        g_uchWirelessTxBuf[FreePageNo][17] = ApplyMAC[2];
        g_uchWirelessTxBuf[FreePageNo][18] = ApplyMAC[3];
        g_uchWirelessTxBuf[FreePageNo][19] = ApplyMAC[4];
        g_uchWirelessTxBuf[FreePageNo][20] = ApplyMAC[5];
        g_uchWirelessTxBuf[FreePageNo][21] = ApplyMAC[6];
        g_uchWirelessTxBuf[FreePageNo][22] = ApplyMAC[7];	
	
	g_uchWirelessTxBuf[FreePageNo][23] = AppLogIntTime;  //应用层注册间隔
	g_uchWirelessTxBuf[FreePageNo][24] = DllLogIntTime;  //链路层注册间隔
	
	memcpy(g_uchWirelessTxBuf[FreePageNo]+25,Router+1,2*Router[0]);  //路由数据
	
	AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
	return FreePageNo;
}

/****************************************************************************/
/* 　　　　　　　　　　     形成数据链注册帧    　      0x35 　　          */
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */
/****************************************************************************/
int GenerateDllLogFrame(void)
{          
        int FrameLength = DLL_LOG_FRAME_LENGTH; //帧长度
        int FeedbackFlag = 0;  //帧回复标志
        
	//判断缓冲区是否能填充下   
	int FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
	if(FreePageNo==-1)
        return -1;             //不能填充
        
        //帧控制字设置
        //  1×01 0000 起始页
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1×00 0000 中间页
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }

    //填充帧头     
    g_uchWirelessTxBuf[FreePageNo][0] = DLL_LOG_FRAME_LENGTH;       // 帧长度
    
    g_uchWirelessTxBuf[FreePageNo][1] = g_uNetParentID&0xff;         //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = g_uNetParentID/256;
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //最终目得
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //初始源
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                     //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum;     //帧编号
    g_uchWirelessTxBuf[FreePageNo][11] = INOF_DLL_LOG;            //帧控制字1   0x36数据链注册
    g_uchWirelessTxBuf[FreePageNo][12] = 0;                   //帧控制字2  任意填充 
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);         //添加CRC
    
    return FreePageNo;
}

/****************************************************************************/
/* 　　　　　　　　　　     形成数据链注册帧确认回复          　　          */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */
/****************************************************************************/
int GenerateOkAck_DllLogFrame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{          
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
	
	//填充帧 
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_DLL_LOG_FRAME_LENGTH ;       //数据回复帧帧长  0x0F
	
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //最终目的初始源
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //最终目得
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //初始源
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //帧编号
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;             //  帧控制字1，0x55
    g_uchWirelessTxBuf[FreePageNo][12]= INOF_DLL_LOG ;    //  帧控制字2，0x36
 
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
	
    return FreePageNo;
}

/****************************************************************************/
/* 　　　　　　　　　　     数据源形成数据状态信息帧    　   0x40 　　      */
/* 函数参数：Data存放数据信息，Data[0]是数据的字节个数,后面跟数据           */
/*  CtlByte 是形成数据状态信息的第二个控制字                                
                           0xb0	正常的传感器数据信息        
                           0xb1	Data中描述的状态对象的网络ID设备丢失
                           0xb2	Data中描述的状态对象的网络ID传感器出错      */
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */
/****************************************************************************/
int GenerateDataStatusFrame(unsigned char *Data,unsigned char CtlByte)
{          
        int FrameLength = DATA_STATUS_FRAME_BASE_LENGTH+Data[0]; //帧长度
        
        int FeedbackFlag = 1;  //帧回复标志
        
	//判断缓冲区是否能填充下   
	int FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
	if(FreePageNo==-1)
        return -1;    //不能填充
        
        //帧控制字设置
        //  1×01 0000 起始页
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1×00 0000 中间页
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }
	
	//填充帧
    g_uchWirelessTxBuf[FreePageNo][0] = DATA_STATUS_FRAME_BASE_LENGTH+Data[0]; //数据帧帧长
    g_uchWirelessTxBuf[FreePageNo][1] = g_uNetParentID&0xff;         //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = g_uNetParentID/256;
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    g_uchWirelessTxBuf[FreePageNo][5] = 0x00;                //最终目得
    g_uchWirelessTxBuf[FreePageNo][6] = 0x00;
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //初始源
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0; //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum; //帧编号    
	
    g_uchWirelessTxBuf[FreePageNo][11]= INFO_DATA_STATUS;      //  帧控制字1，0x40
    g_uchWirelessTxBuf[FreePageNo][12]= CtlByte ;              //  帧控制字2，
	
	
	g_uchWirelessTxBuf[FreePageNo][13]= g_uchNetLev;    //网络级别
	g_uchWirelessTxBuf[FreePageNo][14]= g_uchPHYPAType;  //功放类型,定义为一个字节
	g_uchWirelessTxBuf[FreePageNo][15]=  ( g_PHYDevInfo.DevType & 0x0F ) + ( (g_uchAppchRoleSet & 0x0F) * 16 ) ;  //配置设备类型 当前设备类型 
	g_uchWirelessTxBuf[FreePageNo][16]= g_uchRFSelfFreNum;    //工作频点
	g_uchWirelessTxBuf[FreePageNo][17]= g_uchRFPowerVal[g_uchRFPALev];  //设备功率
	g_uchWirelessTxBuf[FreePageNo][18]= g_PHYDevInfo.PowerType;    //电源类型               
	g_uchWirelessTxBuf[FreePageNo][19]= g_eAppPowerStatus;   //能量状态
	g_uchWirelessTxBuf[FreePageNo][20]= g_uPHYChargePower/16;                     //充电电压
	g_uchWirelessTxBuf[FreePageNo][21]= g_uPHYBattPower/16;                     //电池电压
	g_uchWirelessTxBuf[FreePageNo][22]= g_uPHYBoardPower/16;                     //板级电压
	g_uchWirelessTxBuf[FreePageNo][23]= g_uchDLLCurLQ;                     //链路质量
	g_uchWirelessTxBuf[FreePageNo][24]= (g_uchDLLSubDevNumber&0x0f);   //正常工作子设备个数
	g_uchWirelessTxBuf[FreePageNo][25]= g_uAppDataInt/60;       //注册间隔, 分钟
	g_uchWirelessTxBuf[FreePageNo][26]= g_uAppDataType&0xFF;   //传感器类型 低位
	g_uchWirelessTxBuf[FreePageNo][27]= g_uAppDataType/256;   //传感器类型 高位
	
	memcpy(g_uchWirelessTxBuf[FreePageNo]+28,Data+1,Data[0]);     //数据信息
	
	AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
	return FreePageNo;
}

/****************************************************************************/
/* 　　　　　　　　 形成数据信息帧回复帧    　    　　        	            */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int GenerateOkAck_DataStatusFrame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
	
	//填充帧 
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_DATA_STATUS_FRAME_LENGTH ;       //数据回复帧帧长  0x14
	
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //最终目的初始源
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //最终目得
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //初始源
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //帧编号
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;             //  帧控制字1，0x55
    g_uchWirelessTxBuf[FreePageNo][12]= INFO_DATA_STATUS ;    //  帧控制字2，0x40
    
	g_uchWirelessTxBuf[FreePageNo][13]=  g_uchRFPowerVal[g_uchRFPALev];  //本设备功率
    
    g_uchWirelessTxBuf[FreePageNo][14]= g_uchDLLCurLQ;  //本级链路质量
    
    g_uchWirelessTxBuf[FreePageNo][15]= g_uchNetLev; //本网络级别
	
    g_uchWirelessTxBuf[FreePageNo][16]= g_uchDLLSubDevNumber&0x0f;//本设备当前带的加入网络的设备数量
    //设定目得设备注册间隔  	
    g_uchWirelessTxBuf[FreePageNo][17]= (g_uchNetLev+1)*2; //先预先设定一个值，预防下面检索不到
    for(int i=0;i<NET_MAX_CAP;i++)
    {
		if( ( g_uchWirelessTxBuf[FreePageNo][1] == ( g_SubDevInfo[i].NetID&0xff ) ) && 
            (g_uchWirelessTxBuf[FreePageNo][2] == (g_SubDevInfo[i].NetID/256)))
		{
            g_uchWirelessTxBuf[FreePageNo][17]= g_SubDevInfo[i].AppDataIntTime;
            break;
		}
    }  
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
	
    return FreePageNo;
}


/****************************************************************************/
/* 　　　　　　　　　　      形成数据信息帧总的回复帧  　    　　           */
/* 函数参数：FirstDestID:存放本帧目的地址；
FinalDestID：存放最终目的地址，也是发送申请注册信息的设备地址
Router：路由信息，Router[0]是路由个数，依次是路由地址，低位在前
LogTime: 设置最终目的设备的注册间隔时间                        */                
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */    
/****************************************************************************/
int GenerateDataFeedbackFrame(unsigned char* FirstDestID,unsigned char* FinalDestID,unsigned char *Router,unsigned char LogTime)
{        
        int FrameLength = DATA_STATUS_FEEDBACK_FRAME_BASE_LENGTH+2*Router[0]; //帧长度
        int FeedbackFlag = 0;  //帧回复标志
        
	//判断缓冲区是否能填充下   
	int FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
	if(FreePageNo==-1)
          return -1;             //不能填充
        
        //帧控制字设置
        //  1×01 0000 起始页
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1×00 0000 中间页
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }
	
	//填充帧
    g_uchWirelessTxBuf[FreePageNo][0] = DATA_STATUS_FEEDBACK_FRAME_BASE_LENGTH+2*Router[0]; //数据帧帧长
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    g_uchWirelessTxBuf[FreePageNo][5] = FinalDestID[0];               //最终目得
    g_uchWirelessTxBuf[FreePageNo][6] = FinalDestID[1]; 
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];  //初始源
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
	
	
    g_uchWirelessTxBuf[FreePageNo][9] = Router[0]; //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum; //帧编号
    
	
    g_uchWirelessTxBuf[FreePageNo][11]= INFO_FEEDBACK;              //  总回复 帧控制字1，0x50
    g_uchWirelessTxBuf[FreePageNo][12]= INFO_DATA_STATUS;           //  总回复对象 帧控制字2，数据0x40
	
	
    //设定注册间隔   
    g_uchWirelessTxBuf[FreePageNo][13]= LogTime;
    
    memcpy(g_uchWirelessTxBuf[FreePageNo]+14,Router+1,2*Router[0]);  //路由数据
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);         
	
    return FreePageNo;  
}

/****************************************************************************/
/* 　　　　　　　　　　      形成数据信息帧总回复的回复帧　　        	    */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int SendOkAck_FeedbackFrame(unsigned char*FirstDestID,unsigned char AckFrameNo)
{
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
	
    //填充帧 
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_DATA_STATUS_FEEDBACK_FRAME_BASE_LENGTH ;     
	
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //最终目的最终源
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //最终目得
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //初始源
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //帧编号
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;                        //  帧控制字1，0x55
    g_uchWirelessTxBuf[FreePageNo][12]= INFO_FEEDBACK ;    //  帧控制字2，0x50
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    g_uchRxPAtype = g_uNetParentPAtype; //接收设备类型    

    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //数据信息帧总回复的回复帧
	
    return FreePageNo;
}


/****************************************************************************/
/* 　　　　　　　　　　      形成网络忙回复    　    　　                   */
/*  函数参数：DestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号    */
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int SendBusyAckFrame(unsigned char* DestID,unsigned char AckFrameNo)
{
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
	
	//填充帧         
    g_uchWirelessTxBuf[FreePageNo][0] = BUSYACK_LENGTH;      //帧长度
    g_uchWirelessTxBuf[FreePageNo][1] = DestID[0];         //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = DestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //最终目的最终源
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //最终目得
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //初始源
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
    
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                     //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;     //帧编号
    g_uchWirelessTxBuf[FreePageNo][11] = ACK;            //帧控制字1   回复帧0x55
    g_uchWirelessTxBuf[FreePageNo][12] = BUSY_ACK;       //帧控制字2  忙0xa2
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);         //添加CRC
    
    //判断目的设备的类型
    if( (DestID[0] + DestID[1]*256) == g_uNetParentID )//是上级设备
        g_uchRxPAtype = g_uNetParentPAtype; //接收设备类型
    else
    {
        int i=0;
	
	 for(;i<NET_MAX_CAP;i++) //检查是不是子设备发来的
	 {	    
	        if( ( (g_SubDevInfo[i].Status == 1) ||(g_SubDevInfo[i].Status == 2) ||(g_SubDevInfo[i].Status == 4) )
	            && (g_SubDevInfo[i].NetID == (DestID[0] + DestID[1]*256)) )
		{
		    g_uchRxPAtype = g_SubDevInfo[i].PAType;
		    break;
		}
         }
         
         if( i>= NET_MAX_CAP)
          g_uchRxPAtype = g_uchPHYPAType;  
    }
    
    
    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //直接回复网络忙  
	
    return FreePageNo;    
}

/****************************************************************************/
/* 　　　　　　　　　　      形成命令控制器帧    　  0x60  　　        	    */
/* 函数参数：FirstDestID:存放本帧目的地址；
FinalDestID：存放最终目的地址，也是发送申请注册信息的设备地址
CmdData :存放具体控制内容
Router：路由信息，Router[0]是路由个数，依次是路由地址，低位在前  */            
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */ 
/****************************************************************************/
int GenerateCmdFrame(unsigned char* FirstDestID,unsigned char* FinalDestID,unsigned char *CmdData,unsigned char *Router)
{
        int FrameLength = CMDFRAME_BASE_LENGTH+2*Router[0]; //帧长度
        int FeedbackFlag = 0;  //帧回复标志
        
	//判断缓冲区是否能填充下   
	int FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
	if(FreePageNo==-1)
          return -1;             //不能填充
        
        //帧控制字设置
        //  1×01 0000 起始页
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1×00 0000 中间页
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }
	
	//填充帧
    g_uchWirelessTxBuf[FreePageNo][0] = CMDFRAME_BASE_LENGTH+2*Router[0]; //数据帧帧长
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    g_uchWirelessTxBuf[FreePageNo][5] = FinalDestID[0];               //最终目得
    g_uchWirelessTxBuf[FreePageNo][6] = FinalDestID[1]; 
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];    //初始源
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
	
	
    g_uchWirelessTxBuf[FreePageNo][9] = Router[0]; //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum; //帧编号    
	
    g_uchWirelessTxBuf[FreePageNo][11]= INFO_CMD;                        //  帧控制字1，0x60
	
    memcpy(g_uchWirelessTxBuf[FreePageNo]+12,CmdData,15);     //控制内容
    memcpy(g_uchWirelessTxBuf[FreePageNo]+27,Router+1,2*Router[0]);  //路由数据
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    return FreePageNo;
	
}

/****************************************************************************/
/* 　　　　　　　　　　      形成命令控制帧确认回复    　    　　           */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int SendOkAck_CmdFrame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
	
	//填充帧 
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_CMDFRAME_LENGTH ;       //命令帧回复0x11
	
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //本帧目的
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //最终目的最终源
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //最终目得
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //初始源
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
    
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //帧编号
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;                        //  帧控制字1，0x55
    g_uchWirelessTxBuf[FreePageNo][12]= INFO_CMD ;    //  帧控制字2，0x60
    
    g_uchWirelessTxBuf[FreePageNo][13]= g_uchRFPowerVal[g_uchRFPALev];  //设备功率
    g_uchWirelessTxBuf[FreePageNo][14]= g_uchDLLCurLQ;  //本级链路质量
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);   
    
    g_uchRxPAtype = g_uNetParentPAtype; //接收设备类型
    
    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //直接回复网络忙 
    
    return FreePageNo;
}


/****************************************************************************/
/* 　　　　　　　　　　      形成数据转发帧    　    　　        	    */
/*                  pRelayFrame是收到的需要转发的帧                         */
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */ 
/****************************************************************************/
int GenerateRelayFrame(unsigned char *pRelayFrame)
{
	int FreePageNo;
	//判断是上行还是下行数据
	if( ( pRelayFrame[9] >0 ) &&( pRelayFrame[pRelayFrame[0] - 4 ] == (g_PHYDevInfo.NetID&0xff)) &&
		( pRelayFrame[pRelayFrame[0] - 3 ] == (g_PHYDevInfo.NetID/256)) )    //下行数据
	{			
	        int FrameLength = pRelayFrame[0]-2; //帧长度
                int FeedbackFlag = 0;  //帧回复标志
                
                //判断缓冲区是否能填充下   
                FreePageNo = GetFreePageNo(FeedbackFlag,FrameLength);  
                if(FreePageNo==-1)
                  return -1;             //不能填充
                
                //帧控制字设置
                //  1×01 0000 起始页
                g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
                
                unsigned char pagenum = (FrameLength/PAGESIZE+1);
                if(pagenum>1)
                {
                  for(unsigned char num = 1; num <pagenum ; num++)// 1×00 0000 中间页
                    g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
                }
        
		memcpy(g_uchWirelessTxBuf[FreePageNo],pRelayFrame,pRelayFrame[0]-4);     //拷贝到发送缓冲
		g_uchWirelessTxBuf[FreePageNo][0] -= 2;       //帧长减2
		g_uchWirelessTxBuf[FreePageNo][9]--;       //路由减1
		
		if(g_uchWirelessTxBuf[FreePageNo][9] == 0) //路由为空,本帧目的就是最终目的
		{
			g_uchWirelessTxBuf[FreePageNo][1] = g_uchWirelessTxBuf[FreePageNo][5];         //本帧目的
			g_uchWirelessTxBuf[FreePageNo][2] = g_uchWirelessTxBuf[FreePageNo][6];
		}
		else //本帧目的为路由表中的下一个路由ID
		{
			g_uchWirelessTxBuf[FreePageNo][1] = g_uchWirelessTxBuf[FreePageNo][g_uchWirelessTxBuf[FreePageNo][0]-4];         //本帧目的
			g_uchWirelessTxBuf[FreePageNo][2] = g_uchWirelessTxBuf[FreePageNo][g_uchWirelessTxBuf[FreePageNo][0]-3];
		}
    }
    
    else //上行数据
    {			
        int FrameLength = pRelayFrame[0]+2; //帧长度
        int FeedbackFlag = 0;  //帧回复标志
                
        //判断缓冲区是否能填充下   
        FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
        
        if(FreePageNo==-1)
           return -1;             //不能填充
                
        //帧控制字设置
        //  1×01 0000 起始页
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1×00 0000 中间页
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }
        
        memcpy(g_uchWirelessTxBuf[FreePageNo],pRelayFrame,pRelayFrame[0]-2);     //拷贝到发送缓冲
        g_uchWirelessTxBuf[FreePageNo][0]+=2;       //帧长加2
        g_uchWirelessTxBuf[FreePageNo][9]++;       //路由加1
        
        g_uchWirelessTxBuf[FreePageNo][1] = g_uNetParentID&0xff;         //本帧目的为上级设备
        g_uchWirelessTxBuf[FreePageNo][2] = g_uNetParentID/256;
        
        g_uchWirelessTxBuf[FreePageNo][g_uchWirelessTxBuf[FreePageNo][0] -4 ] = g_PHYDevInfo.NetID&0xff;  //添加路由信息进入路由表
        g_uchWirelessTxBuf[FreePageNo][g_uchWirelessTxBuf[FreePageNo][0] -3 ] = g_PHYDevInfo.NetID/256;   
		
    }
    
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    return FreePageNo;
}

/****************************************************************************/
/* 　　　　　　　　　　      形成数据转发帧确认回复    　    　　           */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号
FrameCmd2: 回复的转发帧的类型                                */
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int SendOkAck_RelayFrame(unsigned char* FirstDestID,unsigned char FrameCmd2,unsigned char AckFrameNo)
{   
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
          
    if(FrameCmd2 == INFO_APPLY) //是申请注册的转发回复
    {            
              //填充帧         
          g_uchWirelessTxBuf[FreePageNo][0] = OKACK_RELAYFRAME_LENGTH;   //帧长度
          g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];            //本帧目得
          g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
          g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;   //本帧源
          g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
          
          //最终目的最终源
          g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //最终目得
          g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
          g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //初始源
          g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
              
          g_uchWirelessTxBuf[FreePageNo][9] = 0;      //路由个数
          g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;      //帧编号
          g_uchWirelessTxBuf[FreePageNo][11]= ACK;
          g_uchWirelessTxBuf[FreePageNo][12]= FrameCmd2;
              //填充数据    
          g_uchWirelessTxBuf[FreePageNo][13]=  g_uchRFPowerVal[g_uchRFPALev];  //设备功率
          
          g_uchWirelessTxBuf[FreePageNo][14]= g_uchDLLCurLQ;  //本级链路质量
          
          g_uchWirelessTxBuf[FreePageNo][15]= g_uchNetLev; //网络级别
              
              //设定目得设备注册间隔  
          g_uchWirelessTxBuf[FreePageNo][16]= (g_uchNetLev+1)*2; //先预先设定一个值，预防下面检索不到
          for(int i=0;i<NET_MAX_CAP;i++)
          {
                      if( ( g_uchWirelessTxBuf[FreePageNo][1] == ( g_SubDevInfo[i].NetID&0xff ))  && 
                  (g_uchWirelessTxBuf[FreePageNo][2] == (g_SubDevInfo[i].NetID/256) ))
                      {
                  g_uchWirelessTxBuf[FreePageNo][16]= g_SubDevInfo[i].AppDataIntTime;
                  break;
                      }
          } 
          AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    }
    
    if(FrameCmd2 == INFO_DATA_STATUS)   //是数据和状态 帧的转发回复
         FreePageNo = GenerateOkAck_DataStatusFrame(FirstDestID,AckFrameNo);   
         
         
   //判断目的设备的类型
    if( (FirstDestID[0] + FirstDestID[1]*256) == g_uNetParentID )//是上级设备
        g_uchRxPAtype = g_uNetParentPAtype; //接收设备类型
    else
    {
        int i=0;
	
	 for(;i<NET_MAX_CAP;i++) //检查是不是子设备发来的
	 {	    
	        if( ( (g_SubDevInfo[i].Status == 1) ||(g_SubDevInfo[i].Status == 2) ||(g_SubDevInfo[i].Status == 4) )
	            && (g_SubDevInfo[i].NetID == (FirstDestID[0] + FirstDestID[1]*256)) )
		{
		    g_uchRxPAtype = g_SubDevInfo[i].PAType;
		    break;
		}
         }
         
         if( i>= NET_MAX_CAP)
          g_uchRxPAtype = g_uchPHYPAType;  
    } 
    
    
    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //直接回复
    return   FreePageNo;
}

/****************************************************************************/
/* 　　　　　　　　　　      形成拒绝访问回复帧       　　        	    */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int SendRefuseAck_Frame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{
	
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
	
	//填充帧头          
    g_uchWirelessTxBuf[FreePageNo][0] = REFUSEACK_FRAME_LENGTH;   //拒绝访问帧，长度:0x0F
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];            //本帧目得
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;   //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    
    //最终目的最终源
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //最终目得
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //初始源
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
    
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;      //帧编号
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;           //回复帧控制字1  0x55
    g_uchWirelessTxBuf[FreePageNo][12]= REFUSE_ACK;//帧控制字2  拒绝访问 0xa0
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    
       //判断目的设备的类型
    if( (FirstDestID[0] + FirstDestID[1]*256) == g_uNetParentID )//是上级设备
        g_uchRxPAtype = g_uNetParentPAtype; //接收设备类型
    else
    {
        int i=0;
	
	 for(;i<NET_MAX_CAP;i++) //检查是不是子设备发来的
	 {	    
	        if( ( (g_SubDevInfo[i].Status == 1) ||(g_SubDevInfo[i].Status == 2) ||(g_SubDevInfo[i].Status == 4) )
	            && (g_SubDevInfo[i].NetID == (FirstDestID[0] + FirstDestID[1]*256)) )
		{
		    g_uchRxPAtype = g_SubDevInfo[i].PAType;
		    break;
		}
         }
         
         if( i>= NET_MAX_CAP)
            g_uchRxPAtype = g_uchPHYPAType;  
    }  
    
    
    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //直接回复网络忙 
    return FreePageNo;
}


/****************************************************************************/
/* 　　　　　　　　　　      形成出错回复帧       　　        	           */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int SendErroAck_Frame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{
	
    //直接回复帧，不需要判断能否填充,不需要设置帧控制字直接形成并发送
    int FreePageNo = BUFF_MAXPAGE -1;//最后一页
	
	//填充帧头          
    g_uchWirelessTxBuf[FreePageNo][0] = ERROACK_FRAME_LENGTH;   //拒绝访问帧，长度:0x0F
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];            //本帧目得
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;   //本帧源
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    
    //最终目的最终源
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //最终目得
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //初始源
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
    	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                //路由个数
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;      //帧编号
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;           //回复帧控制字1  0x55
    g_uchWirelessTxBuf[FreePageNo][12]= ERROR_ACK;//帧控制字2  出错 0xa1
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    
     //判断目的设备的类型
    if( (FirstDestID[0] + FirstDestID[1]*256) == g_uNetParentID )//是上级设备
        g_uchRxPAtype = g_uNetParentPAtype; //接收设备类型
    else
    {
        int i=0;
	
	 for(;i<NET_MAX_CAP;i++) //检查是不是子设备发来的
	 {	    
	        if( ( (g_SubDevInfo[i].Status == 1) ||(g_SubDevInfo[i].Status == 2) ||(g_SubDevInfo[i].Status == 4) )
	            && (g_SubDevInfo[i].NetID == (FirstDestID[0] + FirstDestID[1]*256)) )
		{
		    g_uchRxPAtype = g_SubDevInfo[i].PAType;
		    break;
		}
         }
         
         if( i>= NET_MAX_CAP)
          g_uchRxPAtype = g_uchPHYPAType;  
    }     
    
    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //直接回复网络忙 
    return   FreePageNo;
}


/****************************************************************************/
/* 　　　　　　　　　　     处理简单查询        　    　       　           */
/****************************************************************************/
void Process_SimpleQueryFrame()
{
	unsigned char FirstDestID[2]; //当前待发送数据的 本帧目的
	int PageNo;  //当前待发送的数据存放的页码
	
	memcpy(FirstDestID,g_uchWirelessRxBuf+3,2);
	PageNo = GenerateOkAck_SimpleQueryFrame(FirstDestID,g_uchWirelessRxBuf[10]);
	
	g_uchRxPAtype = g_uchPHYPAType;  //目的设备的类型
	
	RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
}

/****************************************************************************/
/* 　　　　　　　　　　     处理复杂查询        　    　       　           */
/****************************************************************************/
void Process_ComplexQueryFrame()
{
	unsigned char FirstDestID[2]; //当前待发送数据的 本帧目的
	int PageNo;  //当前待发送的数据存放的页码
	
	memcpy(FirstDestID,g_uchWirelessRxBuf+3,2);
	PageNo = GenerateOkAck_ComplexQueryFrame(FirstDestID,g_uchWirelessRxBuf[10]);
	
	g_uchRxPAtype = g_uchPHYPAType; //目的设备的类型
	
	RFSendPacket( g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
}

/****************************************************************************/
/* 　　　　　　　　　　     处理申请注册        　    　       　           */
/* LogIntTime 为申请注册的设备分配注册时间间隔                              */
/****************************************************************************/
void Process_ApplyFrame(unsigned char LogIntTime)
{
	unsigned char FirstDestID[2]; //当前待发送数据的 本帧目的
	int PageNo;  //当前待发送的数据存放的页码
	int DevApplyedFlag = 0; //发送申请设备的ID是否已保存标志：
	char status;//认证状态
	
	memcpy(FirstDestID,g_uchWirelessRxBuf+3,2); //本次处理所有小回复的 本帧目的 = RxBuf 的本帧源
	
	g_uchRxPAtype = g_uchWirelessRxBuf[13]; //目的设备的类型
	
	if((g_uchWirelessRxBuf[5]|(g_uchWirelessRxBuf[6]*256)) == g_PHYDevInfo.NetID)  //最终目的是本设备 ,本设备是中心基站
        {
		if(g_uchWirelessRxBuf[9] == 0)     //路由个数为0？
                {
			for(char count=0; count<NET_MAX_CAP; count++)
                        {
				if((g_uchWirelessRxBuf[7]|(g_uchWirelessRxBuf[8]*256)) == g_SubDevInfo[count].NetID)//发送申请设备的ID是否已保存
				{
					DevApplyedFlag = 1;
					status = g_SubDevInfo[count].Status;
					g_SubDevInfo[count].DevLifeTime = 0;  //重新设定子设备自上次注册到现在为止的逝去时间
					
					break;
				}
                        }//end for
                        
			if(DevApplyedFlag == 1)//发送申请设备的ID已保存,根据status值回复
                        {
				switch(status)
				{
				case 0://0：设备被删除
					break;
				//case 1://1：正常工作；
				//	break;
				case 2://2：待认证；					
					PageNo = GenerateWaitCheckACK_ApplyLogFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					break;
				case 3://3：认证失败；
					PageNo = GenerateCheckFailACK_ApplyFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					break;
				case 1://1：正常工作；	
				case 4://4：认证成功
					PageNo = GenerateCheckPassACK_ApplyFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					break;
				}//end switch
				return;
                        }//end if 发送申请设备的ID已保存,根据status值回复
			else ////发送申请设备的ID未保存
                        {
				if( ((g_uchDLLSubDevNumber/16)+(g_uchDLLSubDevNumber&0x0f)) >= NET_MAX_CAP) //所有子设备数已满
				{          				        
					SendRefuseAck_Frame(FirstDestID,g_uchWirelessRxBuf[10]);
					return;
				}
				else //所有子设备未满
				{
					//以下发送等待认证
					PageNo = GenerateWaitCheckACK_ApplyLogFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					
					//以下保存申请注册的子设备信息
					g_uchDLLSubDevNumber = ((g_uchDLLSubDevNumber/16)+1)*16 + (g_uchDLLSubDevNumber&0x0f) ; //待认证的子设备数量加1
				
					for(char count=0; count<NET_MAX_CAP; count++)//寻找存放位置
					{
					  if(g_SubDevInfo[count].Status == 0) //把当前待认证设备的信息 放入已经删除的设备的位置
                                          {
                                              g_SubDevInfo[count].Status = 2;               //待认证
                                              g_SubDevInfo[count].PAType = g_uchWirelessRxBuf[13];  //本帧源攻放类型
                                              g_SubDevInfo[count].DevType = g_uchWirelessRxBuf[14]; //本帧源设备类型 (0~3bit:配置设备类型；4~7bit:当前设备类型)
                                              g_SubDevInfo[count].DevPower = g_uchWirelessRxBuf[15];//本帧源设备功率 
                                              g_SubDevInfo[count].NetID = g_uchWirelessRxBuf[16] | (g_uchWirelessRxBuf[17]*256);//本帧源设备NetID
                                              g_SubDevInfo[count].DevLifeTime = 0;  //子设备自上次注册到现在为止的逝去时间
                                              g_SubDevInfo[count].AppDataIntTime = 3;  // 设定申请注册设备的数据上报间隔2分钟
                                              g_SubDevInfo[count].DLLLogIntTime = 3;  // 设定申请注册设备的数据链注册间隔2分钟
                                              break;
                                          }
                                       }//end for 寻找存放位置
					return;
				}//end else 所有子设备未满
                          }//end else 发送申请设备的ID未保存
            
                }
		else //路由个数不为0
                {
			if(GetFreePageNo(0,g_uchWirelessRxBuf[0]) == -1) //不能形成主动反馈
                        {
				//以下发送网络忙
				SendBusyAckFrame(FirstDestID,g_uchWirelessRxBuf[10]);
                        }
			else//能形成主动反馈帧
                        {
				//以下两行为对转发的小回复
				SendOkAck_RelayFrame(FirstDestID,g_uchWirelessRxBuf[11],g_uchWirelessRxBuf[10]); 
				
				//下面的对于中心基站来言，先将申请注册信息发送到服务器，收到认证结果后 再主动往下发送
				/*
				//以下为主动反馈
				for(char count=0; count<NET_MAX_CAP; count++)
                                   {
					if((g_uchWirelessRxBuf[3]|(g_uchWirelessRxBuf[4]*256)) == g_SubDevInfo[count].NetID)//找到匹配的本证源设备NetID
					{
						uchFreNum = g_SubDevInfo[count].SubFreNum;
						break;
					}
                                   }
				PageNo = GenerateApplyFeedbackFrame(FirstDestID,g_uchWirelessRxBuf[12],g_uchWirelessRxBuf+16,
					g_uchWirelessRxBuf+18,LogIntTime,g_uchWirelessRxBuf+26); 
				
				DiffFreqCommunication(uchFreNum,PageNo); 
				*/          
                             }
                }//end else 路由个数不为0
        
         }//end if(最终目的是本设备)
	else //最终目的不是本设备 本设备是普通基站
        {          	
                //发送申请注册的设备是本设备的父节点 则说明拓扑结构有变化
                if( (g_uchWirelessRxBuf[3]|(g_uchWirelessRxBuf[4]*256)) == g_uNetParentID)
                {
                    SendRefuseAck_Frame(FirstDestID,g_uchWirelessRxBuf[10]); //直接回复拒绝访问
                    g_uDLLStaUp =0;
                    return;                
                }//end if 发送申请注册的设备是本设备的父节点
                else if( (g_uchWirelessRxBuf[7]|(g_uchWirelessRxBuf[8]*256)) == g_uNetParentID)//本帧源不是父节点 但是初始源是父节点
	        {
                    PageNo = GenerateWaitCheckACK_ApplyLogFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
                    RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);	
                    g_uDLLStaUp =0;
                    return;          
	        }//初始源是父节点
	        
		if(g_uchWirelessRxBuf[9] == 0)//路由个数为0
                {
			for(char count=0; count<NET_MAX_CAP; count++) //搜寻是否已经保存
                        {
				if( (g_SubDevInfo[count].Status !=0 )&& ( (g_uchWirelessRxBuf[7]|(g_uchWirelessRxBuf[8]*256)) == g_SubDevInfo[count].NetID) )//发送申请设备的ID是否已保存
				{
					DevApplyedFlag = 1;
					status = g_SubDevInfo[count].Status;
					g_SubDevInfo[count].DevLifeTime = 0;  //重新设定子设备自上次注册到现在为止的逝去时间                                                                                
					break;
				}
                        }//end for 搜寻是否已经保存
			if(DevApplyedFlag == 1)//发送申请设备的ID已保存,根据status值回复
                        {
		  		switch(status)
				{
				case 0://0：设备被删除
					break;
				//case 1://1：正常工作；
				//	break;
				case 2://2：待认证；

					PageNo = GenerateWaitCheckACK_ApplyLogFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					
					//形成向上的转发申请
			                PageNo = GenerateRelayFrame(g_uchWirelessRxBuf);
			
			                if(PageNo != -1)			
			                  DiffFreqCommunication(g_uchNetParentFreqNo,PageNo);
					break;
				case 3://3：认证失败；
					PageNo = GenerateCheckFailACK_ApplyFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					break;
				case 1://1：正常工作；
				case 4://4：认证成功
					PageNo = GenerateCheckPassACK_ApplyFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					break;
				}
				return;
                        }//end if 发送申请设备的ID已保存,根据status值回复
			else//以前没有保存
                        {
				if(((g_uchDLLSubDevNumber/16)+(g_uchDLLSubDevNumber&0x0f)) == NET_MAX_CAP) //所有子设备数已满
				{
					//以下发送拒绝回复
					SendRefuseAck_Frame(FirstDestID,g_uchWirelessRxBuf[10]);
					return;
				}
                        }//end else 以前没有保存
                }//end if 路由个数为0
		
		/*以下为路由个数为0但设备不满的情况 以及 路由个数不为0的情况*/
		
		if(GetFreePageNo(0,g_uchWirelessRxBuf[0]) == -1)//不能形成向上的转发帧
		{
			//以下发送网络忙回复
			SendBusyAckFrame(FirstDestID,g_uchWirelessRxBuf[10]);
		}
		else //能形成向上的转发帧
		{
			if(g_uchWirelessRxBuf[9] == 0)//路由个数为0 
			{
				//以下发送等待认证
				PageNo = GenerateWaitCheckACK_ApplyLogFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
				RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
				
				//以下保存申请注册的子设备信息
				g_uchDLLSubDevNumber = ((g_uchDLLSubDevNumber/16)+1)*16 + (g_uchDLLSubDevNumber&0x0f) ; //待认证的子设备数量加1
				
				for(char count=0; count<NET_MAX_CAP; count++)//寻找存放位置
				{
				    if(g_SubDevInfo[count].Status == 0) //把当前待认证设备的信息 放入已经删除的设备的位置
                                    {
                                        g_SubDevInfo[count].Status = 2;               //待认证
                                        g_SubDevInfo[count].PAType = g_uchWirelessRxBuf[13];  //本帧源攻放类型
                                        g_SubDevInfo[count].DevType = g_uchWirelessRxBuf[14]; //本帧源设备类型 (0~3bit:配置设备类型；4~7bit:当前设备类型)
                                        g_SubDevInfo[count].DevPower = g_uchWirelessRxBuf[15];//本帧源设备功率
                                        g_SubDevInfo[count].NetID = g_uchWirelessRxBuf[16] | (g_uchWirelessRxBuf[17]*256);//本帧源设备NetID
                                        g_SubDevInfo[count].DevLifeTime = 0;  //子设备自上次注册到现在为止的逝去时间
                                        g_SubDevInfo[count].AppDataIntTime = 3;  // 设定申请注册设备的数据上报间隔3分钟
                                        g_SubDevInfo[count].DLLLogIntTime = 3;  // 设定申请注册设备的数据链注册间隔3分钟
                                        break;
                                    }
                                }//end for 寻找存放位置
			}//end if 路由个数为0
			else
			{	//以下对转发申请作确认回复
				SendOkAck_RelayFrame(FirstDestID,g_uchWirelessRxBuf[11],g_uchWirelessRxBuf[10]); 
			}
			//形成向上的转发申请
			PageNo = GenerateRelayFrame(g_uchWirelessRxBuf);
			
			if(PageNo != -1)
                  {  
                     for(int times=0;times<3;times++)//反馈帧
                     {   
                          if( (DiffFreqCommunication(g_uchNetParentFreqNo,PageNo)) ==0 )
                                break;
                     }	
                  }//end if pageNo!=-1
                  
		}//end else 能形成向上的转发帧		
      } // end else 最终目的不是本设备
}

/****************************************************************************/
/* 　　　　　　　　　　     处理数据链注册            　       　           */
/****************************************************************************/
void Process_DllLogFrame(void)
{
        unsigned char FirstDestID[2]; //当前待发送数据的 本帧目的
	int PageNo;  //当前待发送的数据存放的页码
	
	int flag = 0;	
	
	 for(int i=0;i<NET_MAX_CAP;i++) //检查是不是子设备发来的注册信息
	 {	    
	        if( ( (g_SubDevInfo[i].Status == 1) ||(g_SubDevInfo[i].Status == 2) ||(g_SubDevInfo[i].Status == 4) )
	            && (g_SubDevInfo[i].NetID == (g_uchWirelessRxBuf[3] + g_uchWirelessRxBuf[4]*256)) )
		{
		    flag = 1;
		    g_uchRxPAtype = g_SubDevInfo[i].PAType; //设定接收设备的功放类型    
		    break;
		}
         }
	
	memcpy(FirstDestID,g_uchWirelessRxBuf+3,2);
	
	if(flag) //是子设备发来的注册信息
	{		    	    
	    PageNo = GenerateOkAck_DllLogFrame(FirstDestID,g_uchWirelessRxBuf[10]);
	    RFSendPacket( g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
	}
	else// 不是子设备发来的注册信息
	{
	    g_uchRxPAtype = g_uchPHYPAType; //设定接收设备的功放类型
	    SendRefuseAck_Frame(FirstDestID,g_uchWirelessRxBuf[10]);
	}	
}

/****************************************************************************/
/* 　　　　　　　　　　     处理 数据和状态帧         　       　           */
/****************************************************************************/
void Process_DataStatusFrame(unsigned char *Router,unsigned char LogIntTime)
{
    int PageNo;//当前待发送的数据存放的页码
    unsigned char FirstDestID[2]; //当前待发送数据的 本帧目的
    //unsigned char uchFreNum;
    int flag = -1;//记下发送数据帧的设备在当前保存的子设备的位置
    
    memcpy(FirstDestID,g_uchWirelessRxBuf+3,2); //本次处理所有小回复的 本帧目的 = RxBuf 的本帧源
    
    for(char count=0; count<NET_MAX_CAP; count++)//查看是否是自己的子设备发来的信息
    {
        if( ( g_SubDevInfo[count].Status !=0) &&  ( (g_uchWirelessRxBuf[3]+(g_uchWirelessRxBuf[4]*256)) == g_SubDevInfo[count].NetID) )//找到匹配的本证源设备NetID
        {
                flag = count;
                g_uchRxPAtype = g_SubDevInfo[count].PAType; //设定接收设备的功放类型                  
                break;
        }
    }
        
    if(flag == -1) //不是自己的子设备
    {
        SendRefuseAck_Frame(FirstDestID,g_uchWirelessRxBuf[10]);   
        return;
    } 
    
    if((g_uchWirelessRxBuf[5]|(g_uchWirelessRxBuf[6]*256)) == g_PHYDevInfo.NetID)  //最终目的是本设备
    {
        if(-1 == GetFreePageNo(0,DATA_STATUS_FEEDBACK_FRAME_BASE_LENGTH+2*g_uchWirelessRxBuf[9])) //能否形成总反馈帧
	{
		//以下回复网络忙
	    SendBusyAckFrame(FirstDestID,g_uchWirelessRxBuf[10]);
	} //end if 能否形成总反馈帧
        else
	{
            //以下为对数据转发帧的小回复 
            SendOkAck_RelayFrame(FirstDestID,g_uchWirelessRxBuf[11],g_uchWirelessRxBuf[10]);
            
            //以下的总反馈，要中心基站发送给服务器后，收到确认后，才形成的
            /*
            //以下为形成并调用异级通讯算法发送大回复
            for(char count=0; count<NET_MAX_CAP; count++)
	    {
		if((g_uchWirelessRxBuf[3]|(g_uchWirelessRxBuf[4]*256)) == g_SubDevInfo[count].NetID)//找到匹配的本证源设备NetID
		{
                    uchFreNum = g_SubDevInfo[count].SubFreNum;
                    break;
		}
	    }//end for 
	    PageNo = GenerateDataFeedbackFrame(FirstDestID,g_uchWirelessRxBuf+7,Router,LogIntTime); 
	    DiffFreqCommunication(uchFreNum,PageNo); 
	    */
	}//end else
	
	//需要对数据帧中带来的设备相关信息保存
        g_SubDevInfo[flag].SubFreNum = g_uchWirelessRxBuf[16]; //子设备的工作频点
        g_SubDevInfo[flag].Status = 1;               //正常工作
        g_SubDevInfo[flag].PAType = g_uchWirelessRxBuf[14];  //攻放类型
        g_SubDevInfo[flag].DevType = g_uchWirelessRxBuf[15]; //设备类型 (0~3bit:配置设备类型；4~7bit:当前设备类型)
        g_SubDevInfo[flag].DevPower = g_uchWirelessRxBuf[17];//本帧源设备功率
        g_SubDevInfo[flag].DevLifeTime = 0;  //子设备自上次注册到现在为止的逝去时间
      
    }//end if 最终目的是本设备
    else//最终目的不是本设备
    {
        if(-1 != GetFreePageNo(0,g_uchWirelessRxBuf[0]+2))//能否形成向上的转发帧
	{
            //以下为对转发帧的确认回复
            SendOkAck_RelayFrame(FirstDestID,g_uchWirelessRxBuf[11],g_uchWirelessRxBuf[10]);
            
            //对子设备信息记录
            g_SubDevInfo[flag].Status = 1;        //正常工作
            g_SubDevInfo[flag].DevLifeTime = 0;  //子设备自上次注册到现在为止的逝去时间  
            
            if(g_uchWirelessRxBuf[9] == 0) //路由个数为0  表明信息还没经过转发， 是自己直接下级设备上传信息
            {//记录直接下级的信息
                  g_SubDevInfo[flag].SubFreNum = g_uchWirelessRxBuf[16]; //子设备的工作频点
                  g_SubDevInfo[flag].PAType = g_uchWirelessRxBuf[14];  //攻放类型
                  g_SubDevInfo[flag].DevType = g_uchWirelessRxBuf[15]; //设备类型 (0~3bit:配置设备类型；4~7bit:当前设备类型)
                  g_SubDevInfo[flag].DevPower = g_uchWirelessRxBuf[17];//本帧源设备功率
            } 
            
            //以下为形成向上的转发帧并发送
            PageNo = GenerateRelayFrame(g_uchWirelessRxBuf);
            
            if(PageNo != -1)
            {  
                     for(int times=0;times<3;times++)//反馈帧
                     {   
                          if( (DiffFreqCommunication(g_uchNetParentFreqNo,PageNo)) ==0 )
                                break;
                     }	
            }//end if pageNo!=-1

	}//end if 能否形成向上的转发帧
        else
	{
	     //以下为网络忙回复
            SendBusyAckFrame(FirstDestID,g_uchWirelessRxBuf[10]);
	}
    }//end else 最终目的不是本设备
}

/****************************************************************************/
/* 　　　　　　　　　　      处理命令控制帧    　  0x60  　　        	    */
/*  pFrame 是接收到的命令控制帧                                             */
/****************************************************************************/
void  Process_CmdFrame(unsigned char *pFrame)
{
    int FreePageNo;

//1 控制所有设备时        目标设备为FFFF  目标设备上级为FFFF ，控制命令只需要层层往下转发，路由个数始终设置为0
//2 控制所有基站时        目标设备为0000  目标设备上级为FFFF ，控制命令只需要层层往下转发，路由个数始终设置为0
//3 控制某基站下所有设备  目标设备为FFFF  目标设备上级指定   ，控制命令按照路由向下发送，路由个数不定
//4 控制特定设备          目标设备指定    目标设备上级指定   ，控制命令按照路由向下发送，路由个数不定
//当前只实现上面的第4条
    g_uDLLStaUp = DLL_MAX_STAUP ; //上行链路系数赋值最大

    if( pFrame[9]>1)   //路由个数大于1，命令帧需要转发 
    {
        FreePageNo=GenerateRelayFrame(pFrame);  //形成转发帧
		
	if ( FreePageNo<0)  //缓冲区满，不能形成转发帧，回复网络忙
	    SendBusyAckFrame(pFrame+3,pFrame[10]);                //直接回复网络忙
	else  //能够形成转发帧
	{
	    SendOkAck_CmdFrame(pFrame+3,pFrame[10]);       //发送回复帧
			
	    //寻找子设备频点
	    int DestFreqNo = -1;
	    for(int i=0;i<NET_MAX_CAP;i++)//寻找子设备频点
	    {
		if ( (g_SubDevInfo[i].Status == 1 ) && ( g_SubDevInfo[i].NetID == (pFrame[pFrame[0]-6]+pFrame[pFrame[0]-5]*256 )))
		{
		    DestFreqNo = g_SubDevInfo[i].SubFreNum;
		    g_uchRxPAtype = g_SubDevInfo[i].PAType;
		    break;
		}
	    }//end for 寻找子设备频点          
          
          if(DestFreqNo != -1)
          {  
                     for(int times=0;times<3;times++)//反馈帧
                     {   
                          //调用异级信道通讯算法
                          if( (DiffFreqCommunication(DestFreqNo,FreePageNo)) ==0 )
                                break;
                     }	
          }//end if DestFreqNo!=-1
	
        }//end else 能够形成转发帧
    }//end if 路由个数大于1，命令镇需要转发 
    
    else if( pFrame[9] == 1)  //路由个数等于1，自己是需要控制的节点的上级
    {
        //暂时只保存命令信息 不往下发送
        SendOkAck_CmdFrame(pFrame+3,pFrame[10]);       //发送回复帧
        
        //添加注册间隔时间判断 数据上报间隔 不能超过3小时
        unsigned char time = pFrame[18];
        
        if ( (time >=180 ) || (time ==0) )
          time = 180; 
        
        
       if( (pFrame[15]==0xff) && ( pFrame[16]==0xff) ) //基站下所有的设备
	{  
	    for(int i=0;i<NET_MAX_CAP;i++)
		g_SubDevInfo[i].AppDataIntTime = time;//pFrame[18];
	}
	
	else//特定设备
	{	
	  int DestFreqNo = -1;
	  for(int i=0;i<NET_MAX_CAP;i++)//寻找子设备频点
	  {
		if ( (g_SubDevInfo[i].Status == 1 ) && ( g_SubDevInfo[i].NetID == (pFrame[5]+pFrame[6]*256 )))
		{
			g_SubDevInfo[i].AppDataIntTime = time;
			DestFreqNo = g_SubDevInfo[i].SubFreNum;
			g_uchRxPAtype = g_SubDevInfo[i].PAType;
			break;
		}
	  }//end for 寻找子设备频点
	  
	  FreePageNo=GenerateRelayFrame(pFrame);  //形成转发帧
					
	  //向下传输
	  if( (DestFreqNo != -1) &&(FreePageNo!= -1))
	  {          
            for(int times=0;times<3;times++)
            {
                if(DestFreqNo != g_uchRFSelfFreNum)
                {
                  if( (DiffFreqCommunication(DestFreqNo,FreePageNo)) ==0)
                    break;
                }
                else
                {
                  if(BaseSelfFreqCommunication(FreePageNo) == 0 )
                    break;
                }
            }//end for 
         }//end if 向下传输
	        
	  }//end else 特定设备
        
        /*
	if( pFrame[15]==0xff && pFrame[16]==0xff ) //
	{
	    
	    
	    for(int i=0;i<NET_MAX_CAP;i++)
		g_SubDevInfo[i].LogIntTime=pFrame[18];
	}
	
	else
	{
            FreePageNo=GenerateRelayFrame(pFrame);  //形成转发帧
			
            if ( FreePageNo<0)  //缓冲区满，不能形成转发帧，回复网络忙
				SendBusyAckFrame(pFrame+3,pFrame[10]);                //直接回复网络忙
            else                //能够形成转发帧
            {
				SendOkAck_CmdFrame(pFrame+3,pFrame[10]);       //发送回复帧
				
				//寻找子设备频点
				int DestFreqNo=0xff;
				for(int i=0;i<NET_MAX_CAP;i++)
				{
                    if ( g_SubDevInfo[i].NetID == (pFrame[pFrame[0]-6]+pFrame[pFrame[0]-5]*256 ))
                    {
						DestFreqNo=g_SubDevInfo[i].SubFreNum;
						break;
                    }
				}
				//调用异级信道通讯算法
				DiffFreqCommunication(DestFreqNo,FreePageNo);
            }
			
			
	}
	*/
		
    }
    
    if( pFrame[9]==0)        //控制命令帧的最终目的是自己
    {
	//发送回复信息
	SendOkAck_CmdFrame(pFrame+3,pFrame[10]);       //形成回复帧
	if( pFrame[12]==0xa3 )          //参数调整
	{
	    //添加注册间隔时间判断
	    if( (pFrame[18] <= 254)  && (pFrame[18] >0) )
	        g_uAppDataInt=pFrame[18]*60;
	    else
	        g_uAppDataInt=21600;//六小时
	        
            g_uchNetLev = g_uchWirelessRxBuf[17] + 1;//本级级别 = 上级级别 + 1 
	}
	if( pFrame[12]==0xa5 )          //暂停访问
	{
	    //添加注册间隔时间判断
	    if( (pFrame[18] <= 254)  && (pFrame[18] >0) ) 
	        g_uAppDataInt=pFrame[18]*60;
	    else
	        g_uAppDataInt=21600;//六小时
	        
	    g_uchNetStatus |= Parent_Hold;//进入 Parent_Hold
	} 
	if(pFrame[12]==0xa6 )          //解除拓扑关系
	{
	    g_uDLLStaUp = 0;	
	}	
    }
}

/****************************************************************************/
/* 　　　　　　　　　　      处理反馈帧    　  0x50 　　        	    */
/*  pFrame 是接收到的反馈帧                                                 */
/****************************************************************************/
void  Process_FeedbackFrame(unsigned char *pFrame)
{
    int FreePageNo;
    
    g_uDLLStaUp = DLL_MAX_STAUP ; //上行链路系数赋值最大
    
    if( (pFrame[12]== CHECK_PASS_ACK) ||  (pFrame[12]== CHECK_FAIL_ACK) )  //是申请注册反馈帧
    {
        if( pFrame[9]>1)   //路由个数大于1，申请注册反馈帧需要转发 
        {
	    FreePageNo=GenerateRelayFrame(pFrame);  //形成转发帧
			
	    if ( FreePageNo<0)  //缓冲区满，不能形成转发帧，回复网络忙
		SendBusyAckFrame(pFrame+3,pFrame[10]);                //直接回复网络忙
	    else  //能够形成转发帧
	    {
		SendOkAck_FeedbackFrame(pFrame+3,pFrame[10]);  //回复反馈帧    
            
            //判断反馈帧 所反馈的最终目的和帧编号是否和自己帧缓冲中积压的 需要转发的帧的起始源和帧编号一直
            //如果一直，则删除缓冲的帧
            //此段处理是因为出现如下情况：基站发送缓冲帧时，上级设备回复确认帧，基站没有收到，此时该数据帧在帧缓冲中积压
            //但是从上级设备已经回复了总反馈了。
            for(int i = 0;i<BUFF_MAXPAGE;i++)//检查有没有需要发送的帧
            {
                   if (( g_uchWirelessTxBufCtl[i] & 0x90) == 0x90) 
                   {
                      if( (pFrame[5] == g_uchWirelessTxBuf[i][7]) && (pFrame[6] == g_uchWirelessTxBuf[i][8]) &&//反馈帧最终目的 和缓冲帧初始源相同
                          (pFrame[10] == g_uchWirelessTxBuf[i][10]))//反馈帧帧编号 和缓冲帧 帧编号相同
                      {
                          g_uchWirelessTxBufCtl[i] = 0;//删除该帧
                      }
                   }
            }//end for 检查有没有需要发送的帧
            
		
		//寻找子设备频点
		int DestFreqNo = -1;
		
		for(int i=0;i<NET_MAX_CAP;i++)
		{
		    if ((g_SubDevInfo[i].Status == 1 ) && ( g_SubDevInfo[i].NetID == (pFrame[pFrame[0]-6]+pFrame[pFrame[0]-5]*256 )))
		    {
			DestFreqNo = g_SubDevInfo[i].SubFreNum;
			g_uchRxPAtype = g_SubDevInfo[i].PAType;			
			break;
		    }
	        }
		//调用异级信道通讯算法
		if(DestFreqNo != -1)
            {
                for(unsigned char ch  = 0; ch<3; ch++)
                {
                    if(DiffFreqCommunication(DestFreqNo,FreePageNo)==0)
                      break;
                }
            }
		  
	    }//end else 能够形成转发帧
        }//end if 路由个数大于1，申请注册反馈帧需要转发 
        
        else if( pFrame[9]==1)  //路由个数等于1，保存认证结果
        {
	    unsigned char Location = 0xff;//保存在子设备信息表中的位置信息
	    
	    SendOkAck_FeedbackFrame(pFrame+3,pFrame[10]);  //回复反馈帧  
	    
	    for(int i=0;i<NET_MAX_CAP;i++)//查看是否有记录
	    {
	        if( g_SubDevInfo[i].NetID == (pFrame[13]+pFrame[14]*256) )
		{
		    Location = i;
                    break;
		}
            }//end for 查看是否有记录
            
            if( Location == 0xff ) //当前无记录 则记录下来
            {
                for(int i=0;i<NET_MAX_CAP;i++)//寻找空位
                {
                    if(g_SubDevInfo[i].Status == 0)
                    {
                        g_SubDevInfo[i].NetID = (pFrame[13]+pFrame[14]*256);
                        Location = i;
                        break;
                    }
                }//end for 寻找空位
            }//end if 当前无记录 则记录下来  
            
            if(Location != 0xff ) //信息被记录下来
            {
                  if(pFrame[12] == CHECK_PASS_ACK) //认证成功
                  {                
                      g_SubDevInfo[Location].Status = 4;   
                      
                      //以下添加注册间隔时间判断
                      
                      if( (pFrame[23] <= 180 )  && (pFrame[23]>0))                     
                          g_SubDevInfo[Location].AppDataIntTime = pFrame[23];//设定应用层数据上报间隔时间  
                      else
                          g_SubDevInfo[Location].AppDataIntTime = 180; //3小时
                      
                      if( (pFrame[24] <= 60) && (pFrame[24]>0))     
                          g_SubDevInfo[Location].DLLLogIntTime = pFrame[24];//设定链路层注册间隔时间 
                      else
                          g_SubDevInfo[Location].DLLLogIntTime = 60;  //60分钟
                                       
                  }
                  else if(pFrame[12]==CHECK_FAIL_ACK) //认证失败
                  {
                      g_SubDevInfo[Location].Status = 3; 
                      g_SubDevInfo[Location].AppDataIntTime = 1;//设定应用层注册间隔时间 1分钟 
                  }
            }//end if 信息被记录下来
            
            unsigned char high,low;
            high = low = 0;
            
            for(int i=0;i<NET_MAX_CAP;i++)//保存子设备个数信息
            {
                switch(g_SubDevInfo[i].Status)
                {
                  case 0:
                    break;
                  case 1:
                  case 4:
                    low++;
                    break;
                  case 2:
                  case 3:
                    high++;
                    break;
                 }
            }//end for 保存子设备个数信息
            
            g_uchDLLSubDevNumber = (high&0x0f)*16 + (low&0x0f);  
		    
        }//end else 路由个数等于1，保存认证结果
        
    }//end if 是申请注册反馈帧
    
    if( pFrame[12]== INFO_DATA_STATUS )  //是数据 和状态 信息反馈帧
    {		
        if( pFrame[9]>0)   //路由个数大于0，数据信息反馈帧需要转发 
        {
            FreePageNo=GenerateRelayFrame(pFrame);  //形成转发帧
                            
            if ( FreePageNo<0)  //缓冲区满，不能形成转发帧，回复网络忙
                SendBusyAckFrame(pFrame+3,pFrame[10]);                //直接回复网络忙
            else  //能够形成转发帧
            {
                SendOkAck_FeedbackFrame(pFrame+3,pFrame[10]);  //回复反馈帧     
                
                //判断反馈帧 所反馈的最终目的和帧编号是否和自己帧缓冲中积压的 需要转发的帧的起始源和帧编号一直
                //如果一直，则删除缓冲的帧
                //此段处理是因为出现如下情况：基站发送缓冲帧时，上级设备回复确认帧，基站没有收到，此时该数据帧在帧缓冲中积压
                //但是从上级设备已经回复了总反馈了。
                for(int i = 0;i<BUFF_MAXPAGE;i++)//检查有没有需要发送的帧
                {
                       if (( g_uchWirelessTxBufCtl[i] & 0x90) == 0x90) 
                       {
                          if( (pFrame[5] == g_uchWirelessTxBuf[i][7]) && (pFrame[6] == g_uchWirelessTxBuf[i][8]) &&//反馈帧最终目的 和缓冲帧初始源相同
                              (pFrame[10] == g_uchWirelessTxBuf[i][10]))//反馈帧帧编号 和缓冲帧 帧编号相同
                          {
                              g_uchWirelessTxBufCtl[i] = 0;//删除该帧
                          }
                       }
                }//end for 检查有没有需要发送的帧 
                
		//保存目的设备的频点
		int DestFreqNo = -1;
		
		if(pFrame[9]>1) //最终目的不是本机下面的子设备
		{
                    for(int i=0;i<NET_MAX_CAP;i++)//确定下一个路由是自己的子设备,并确定子设备频点
                    {
                        if ( (g_SubDevInfo[i].Status == 1 ) && (g_SubDevInfo[i].NetID == (pFrame[pFrame[0]-6] + pFrame[pFrame[0]-5]*256 )))
                        {
                            DestFreqNo = g_SubDevInfo[i].SubFreNum;
                            g_uchRxPAtype = g_SubDevInfo[i].PAType;
                            break;
                        }
                    }//end for 寻找子设备频点
                }//end if 最终目的不是本机下面的子设备
                else//最终目的是本机下面的子设备
                {
                   for(int i=0;i<NET_MAX_CAP;i++)//寻找子设备频点
                   {
                        if ( (g_SubDevInfo[i].Status == 1 ) &&  (g_SubDevInfo[i].NetID == (pFrame[5] + pFrame[6]*256 )))
                        {
                            DestFreqNo = g_SubDevInfo[i].SubFreNum;
                            g_uchRxPAtype = g_SubDevInfo[i].PAType;
                            break;
                        }
                    }//end for 寻找子设备频点
                }
		
              if(DestFreqNo != -1)	
              {
                  for(unsigned char ch =0; ch<3;ch++)
                  {
                      if( g_uchRFSelfFreNum== DestFreqNo)//同一个频点
                      {
                          if(BaseSelfFreqCommunication(FreePageNo)==0)
                            break;
                      }
                      else
                      {
                          if(DiffFreqCommunication(DestFreqNo,FreePageNo)==0)
                            break;
                      }
                   }
              }
	    }//end else 能够形成转发帧
	} //end if 路由个数大于0，数据信息反馈帧需要转发 
		
        else if( pFrame[9]==0)  //路由个数等于0,数据信息反馈到达终点
        {
	    SendOkAck_FeedbackFrame(pFrame+3,pFrame[10]);  //回复反馈帧 
	    for(int i=0;i<5;i++)
		  if( ( g_uchWirelessTxBufCtl[i] & 0x40 ) == 0x40 )
                      g_uchWirelessTxBufCtl[i]=0;           //清除帧缓冲控制字反馈标志

            //添加注册间隔时间判断
	    if ( (pFrame[13] <= 254)  && (pFrame[13] > 0) )
	        g_uAppDataInt=pFrame[13]*60; //配置数据上报间隔时间
	    else
	        g_uAppDataInt=21600;//六小时
          
        }//end else if 路由个数等于0,数据信息反馈到达终点
    }//end if 是数据信息反馈帧
}

