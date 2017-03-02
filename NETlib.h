//---------------------------------------------------------------------------------------------
//-------------------------- 网络层参数定义---------------------------------------------------
//---------------------------------------------------------------------------------------------

/****************************信息帧控制字定义*****************************/
#define INFO_SIMPLE_QUERY 		0x10		//简单网络查询
#define INFO_COMPLEX_QUERY 		0x20		//复杂网络查询
#define INFO_APPLY 		        0x30		//申请注册

#define INOF_DLL_LOG                    0x35            //数据链注册

#define INFO_DATA_STATUS 		0x40		//数据和状态信息
       // 数据和状态信息的二次控制字宏定义
       #define INFO_DATA                0xB0           //数据信息
       #define INFO_DEV_LOST            0xB1           //设备注销
       #define INFO_SENSOR_ERRO         0xB2           //传感器故障

#define INFO_FEEDBACK   		0x50		//主动反馈帧
#define INFO_CMD                        0x60            //命令控制

#define CHECK_WAIT_ACK	                0x31	        //等待验证申请
#define CHECK_PASS_ACK                  0x32            //验证成功
#define CHECK_FAIL_ACK                  0x33            //验证失败
 
#define ACK                             0x55            //回复帧
#define REFUSE_ACK                      0xA0            //拒绝访问
#define ERROR_ACK                       0xA1            //出错重发
#define BUSY_ACK                        0xA2            //等待下次发送  


/****************************帧长度定义*****************************/
#define QUERY_FRAME_LENGTH                          0x0F    //查询帧基本长度            
#define OKACK_SIMPLEQUERY_FRAME_LENGTH              0x16    //简单查询回复帧长
#define OKACK_COMPLEXQUERY_FRAME_LENGTH             0x19+3*(g_uchDLLSubDevNumber&0x0f)  //复杂查询回复帧长
#define APPLY_FRAME_LENGTH                          0x1C    //申请注册帧长
#define WAITCHECKACK_APPLY_FRAME_LENGTH             0x1A    //申请注册等待认证回复帧长
#define CHECKPASSACK_APPLY_FRAME_LENGTH             0x1C    //申请注册通过认证回复帧长
#define CHECKFAILACK_APPLY_FRAME_LENGTH             0x19    //申请注册认证失败回复帧长
#define FINALACK_APPLY_FRAME_LENGTH                 0x1B    //申请注册认证总反馈帧基本长度
#define OKACK_FINALACK_APPLY_FRAME_LENGTH           0x0F    //申请注册认证总反馈的回复帧长

#define DLL_LOG_FRAME_LENGTH                          0x0F   //数据链路注册帧长
#define OKACK_DLL_LOG_FRAME_LENGTH                    0x0F   //数据链路注册帧正确确认长度

#define DATA_STATUS_FRAME_BASE_LENGTH                 0x1E  //数据和状态帧基本长度
#define OKACK_DATA_STATUS_FRAME_LENGTH                0x14  //数据和状态确认帧长
#define DATA_STATUS_FEEDBACK_FRAME_BASE_LENGTH        0x10  //数据和状态反馈帧长
#define OKACK_DATA_STATUS_FEEDBACK_FRAME_BASE_LENGTH  0x0F  //数据和状态反馈帧的确认帧基本长度

#define BUSYACK_LENGTH                                0x0F  //链路忙回复帧长
#define OKACK_RELAYFRAME_LENGTH                       0x13  //中继回复帧长
#define CMDFRAME_BASE_LENGTH                          0x1d  //命令基本帧长
#define OKACK_CMDFRAME_LENGTH                         0x11  //命令回复帧长

#define REFUSEACK_FRAME_LENGTH                        0x0F  //拒绝帧长
#define ERROACK_FRAME_LENGTH                          0x0F  //出错帧长



/****************************命令帧宏定义*****************************/
//0x60  +  
#define EO_CMD_NORMAL_WORK		        0xA3		//正常工作
#define EO_CMD_TRANS_INFO 		        0xA4		//立即向上级设备发送信息帧
#define EO_CMD_SEARCH_FRE   		        0xA5		//本设备进入搜频程序，暂停其他设备对网络访问
#define EO_CMD_REMOVE_TOPO                    0xA6            //解除拓扑关系
#define EO_CMD_INQMODE2_OPEN                  0xA7            //开启基站复杂查询服务
#define EO_CMD_INQMODE2_CLOSE                 0xA8            //关闭基站复杂查询服务




/*****************************************************************************************/
/*            缓冲区处理,得到缓冲空页。同时设置帧缓冲控制字　　　 　　　　　　           */
/* FinalAckFlag形成的帧是否需要总回复(0, 不要要，1，需要）,FrameLength 需要形成的帧的长度*/
/* 函数返回值： -1:页空间小于2或者空间不够                                               */
/*              其他：当前可用的页首地址                                                 */
/*****************************************************************************************/
int  GetFreePageNo(unsigned char,int);

/****************************************************************************/
/* 　　　　　　　　　　      形成简单(复杂)查询帧   　    　　        	    */
/*        QueryType 0x10 简单查询,  QueryType 0x20 复杂查询                 */
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */
/****************************************************************************/
int GenerateQueryFrame(unsigned char);

/****************************************************************************/
/* 　　　　　　　　　　      申请注册设备形成申请注册帧   0x30    　       */
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */
/****************************************************************************/
int GenerateApplyLogFrame(unsigned char *FirstDestID);

/****************************************************************************/
/* 　　　　　　　　　　     形成数据链注册帧    　      0x35 　　          */
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */
/****************************************************************************/
int GenerateDllLogFrame(void);

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
int GenerateDataStatusFrame(unsigned char *Data,unsigned char CtlByte);

/****************************************************************************/
/* 　　　　　　　　　　      形成命令控制器帧    　  0x60  　　        	    */
/* 函数参数：FirstDestID:存放本帧目的地址；
FinalDestID：存放最终目的地址，也是发送申请注册信息的设备地址
CmdData :存放具体控制内容
Router：路由信息，Router[0]是路由个数，依次是路由地址，低位在前  */            
/* 函数返回值： -1:页空间小于2或者空间不够                                  */
/*              其他：当前形成的帧存放的首页号                               */ 
/****************************************************************************/
int GenerateCmdFrame(unsigned char* FirstDestID,unsigned char* FinalDestID,unsigned char *CmdData,unsigned char *Router);

/****************************************************************************/
/* 　　　　　　　　　　      形成命令控制帧确认回复    　    　　           */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int SendOkAck_CmdFrame(unsigned char*,unsigned char);



/****************************************************************************/
/* 　　　　　　　　　　     处理申请注册        　    　       　           */
/* LogIntTime 为申请注册的设备分配注册时间间隔                              */
/****************************************************************************/
void Process_ApplyFrame(unsigned char LogIntTime);


/****************************************************************************/
/* 　　　　　　　　　　     处理数据链注册            　       　           */
/****************************************************************************/
void Process_DllLogFrame(void);

/****************************************************************************/
/* 　　　　　　　　　　      处理命令控制帧    　  0x60  　　        	    */
/*  pFrame 是接收到的命令控制帧                                                         */
/****************************************************************************/
void  Process_CmdFrame(unsigned char*);

/****************************************************************************/
/* 　　　　　　　　　　      形成拒绝访问回复帧       　　        	    */
/*  函数参数：FirstDestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号*/
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int SendRefuseAck_Frame(unsigned char*,unsigned char);

/****************************************************************************/
/* 　　　　　　　　　　      形成网络忙回复    　    　　                   */
/*  函数参数：DestID:是回复帧的目的地址 ;AckFrameNo: 是回复对象的帧编号    */
/*  函数返回值：当前形成的帧存放的首页号                                   */
/****************************************************************************/
int SendBusyAckFrame(unsigned char* DestID,unsigned char AckFrameNo);

/****************************************************************************/
/* 　　　　　　　　　　     处理简单查询        　    　       　           */
/****************************************************************************/
void Process_SimpleQueryFrame();

/****************************************************************************/
/* 　　　　　　　　　　     处理复杂查询        　    　       　           */
/****************************************************************************/
void Process_ComplexQueryFrame();

/****************************************************************************/
/* 　　　　　　　　　　     处理 数据和状态帧          　       　           */
/****************************************************************************/
void Process_DataStatusFrame(unsigned char *Router,unsigned char LogIntTime);

/****************************************************************************/
/* 　　　　　　　　　　      处理反馈帧    　  0x50 　　        	    */
/*  pFrame 是接收到的反馈帧                                                 */
/****************************************************************************/
void  Process_FeedbackFrame(unsigned char *pFrame);



