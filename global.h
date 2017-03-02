#ifndef _GLOBAL
#define _GLOBAL 

//定义本设备当前能量态。
typedef enum { Status_TBD,                //0 待定
Power_Full,        	//1 充足态
Power_Normal,           //2 常态
Power_Short,            //3 缺失态
Power_Protect}_PowerStatus ;      //4 保护态


//定义设备类型
typedef enum  {	 DevTypeTBD,  //0 未定义
Server,                       //1 服务器
CBC,                          //2 中心基站控制器
CenterBase,		      //3 中心基站  
Base,			      //4 基站   
Term1,	        	      //5可休眠端机：（既可作基站又可作端机的设备初始化为端机时为可休眠端机）
Term2,	        	      //6不可休眠端机：
Monitor,		      //7 监测节点
Base_or_Term}_DevType;        //8 基站或者端机

//定义本设备当前能量供给类型
typedef enum  {  Type_TBD,	        //0x00 待定
Bat1_Norm,              		//0x01  1节一次性电池 
Bat2_Norm, 	        		//0x02 	2节一次性电池 
NiHi3_220_N,            		//0x03 	220V＋3节镍氢
NiHi3_220_Y, 				//0x04 	220V＋3节镍氢+ 二次电池
NiHi4_220_N,            		//0x05  220V＋4节镍氢
NiHi4_220_Y, 				//0x06  220V＋4节镍氢+ 二次电池
LABat4_220_N,            		//0x07 	220V＋铅酸电池4
LABat4_220_Y, 				//0x08 	220V＋铅酸电池4+ 二次电池
NiHi3_Solar_N,         	 	        //0x09	太阳能＋3节镍氢
NiHi3_Solar_Y, 				//0x0a  太阳能＋3节镍氢+ 二次电池
NiHi4_Solar_N,  			//0x0b  太阳能＋4节镍氢
NiHi4_Solar_Y, 				//0x0c  太阳能＋4节镍氢+ 二次电池
LABat4_Solar_N,  			//0x0d  太阳能＋铅酸电池4
LABat4_Solar_Y, 			//0x0e  太阳能＋铅酸电池4+ 二次电池
FaraCap_Solar_N,			//0x0f  太阳能＋法拉电容
FaraCap_Solar_Y,        		//0x10  太阳能＋法拉电容+ 二次电池
NiHi1_220_N,            		//0x11  220V＋1节镍氢
NiHi1_220_Y, 				//0x12	220V＋1节镍氢+ 二次电池
NiHi2_220_N,            		//0x13  220V＋2节镍氢
NiHi2_220_Y, 				//0x14  220V＋2节镍氢+ 二次电池
NiHi1_Solar_N,          		//0x15  太阳能＋1节镍氢
NiHi1_Solar_Y, 	        		//0x16	太阳能＋1节镍氢+二次电池
NiHi2_Solar_N,          		//0x17  太阳能＋2节镍氢
NiHi2_Solar_Y 	        		//0x18  太阳能＋2节镍氢+二次电池
}_PowerType;


/***********************************************/
/*          需要烧写到FLASH信息存储区的内容    */
/***********************************************/
typedef struct {
	union {
		unsigned int uIntegratorID;   
		unsigned char ucIntegratorID [2];
	} IntegratorID;           //设备集成商ID
	union {
		unsigned int uProjectID;
		unsigned char ucProjectID [2];
	} ProjectID;              //设备工程ID
	
	//定义本设备当前能量供给类型
	_PowerType PowerType;
	
	//定义设备类型
	_DevType DevType;
	
	unsigned int NetID;        //网络ID
	unsigned char DevMAC[8];    //设备物理地址
} _DevInfo;

extern _DevInfo g_PHYDevInfo;
extern _PowerStatus g_eAppPowerStatus;

//基站当前注册的子设备数量 后4个bit（bit3~bit0）：
//已正式加入网络的设备； 前四个bit：暂未正式加入网络的设备
extern unsigned char g_uchDLLSubDevNumber; 

#define NET_MAX_CAP  12             //网络最大容量

//基站要保存的下级设备信息定义 
typedef struct{
	unsigned char Status; //注册状态：0: 设备被删除；1：正常工作；2：待认证；3：认证失败；4：认证成功
	unsigned char DevType;     //设备类型 (0~3bit:配置设备类型；4~7bit:当前设备类型)
	unsigned int NetID;        //设备网络ID
	unsigned char PAType;      //功放类型
	unsigned char PowerType;   //电源类型
	unsigned char DevPower;    //设备功率
	unsigned char AppDataIntTime;  //应用层数据间隔时间
	unsigned char DLLLogIntTime;  //数据链路层注册间隔时间
	unsigned char PowerStatus; //能量状态
	unsigned char SubFreNum;     //工作频点   //g_uchNetSubBFreNum[NET_MAX_CAP/2]
	unsigned char SubDevNum;   //子设备数量 后4个bit（bit3~bit0）：已正式加入网络的设备； 前四个bit：暂未正式加入网络的设备
	unsigned char DevLifeTime;   //基站保存的子设备自上次注册到现在为止的逝去时间
	unsigned char LatestFrameNo; //子设备发送的最后一帧数据的帧编号
} _SubInfo;

extern _SubInfo  g_SubDevInfo[NET_MAX_CAP];


//网络状态宏定义
#define Parent_Lost         0x01  //无上级，需要搜寻，无级别限制
#define Parent_Lost_Grade   0x02  //无上级，需要搜寻，有级别限制
#define Parent_Hold         0x04  //上级忙正在搜寻，
#define Child_Lost          0x08  //下级通讯需要确定工作频点

extern unsigned char g_uchNetStatus;          //网络状态        

extern unsigned char g_uchNetLev;            //网络层全局变量，定义网络级别

extern unsigned int g_uNetParentID ;       //对于基站和端机而言，定义上级设备的网络ID
extern unsigned char g_uchNetParentFreqNo ; // 定义上级设备的频点编号
extern unsigned char g_uNetParentPAtype;  //记录上级设备的功放类型




extern unsigned int g_uAppDataInt ;  //应用层数据上报间隔时间 以秒为单位 



// ――――应用层全局变量，设定设备当前的角色；
//1字节表示	标志的设备功能角色
//0 未定义
//1 服务器
//2 中心基站控制器
//3 中心基站  
//4 基站   
//5可休眠端机：（既可作基站又可作端机的设备初始化为端机时为可休眠端机）
//6不可休眠端机：
//7 监测节点
//8 基站或者端机

extern unsigned char g_uchAppchRoleSet; //初始化为未知 

//标志节点的传感器功能
//2字节表示	标志的设备传感器功能
//0x00 00 	未知
//0x00 01	短数据链业务
//0x00 02	长数据图像业务
//0x00 03	加速度
//0x00 04	光照度
//0x00 05	温湿度
//0x00 06	酸碱度
//0x00 07	特种设备，客流密度传感器	
//0x00 08       430片内温度传感器	
//0x00 09     二氧化碳
//0x00 0A     风速风向	
//0x00 0B     噪声

extern unsigned int g_uAppDataType; 


extern const unsigned int g_uAppFaraCapFull;  //――――法拉电容饱和态下限值；
extern const unsigned int g_uAppFaraCapNormal;// ――――法拉电容常态下限值；
extern const unsigned int g_uAppFaraCapShort; //――――法拉电容缺失态电压下限值；

extern const unsigned int g_uApp3NiHiFull;   //―――3节镍氢电池饱和态下限值
extern const unsigned int g_uApp3NiHiNormal; //―――3节镍氢电池常态下限值
extern const unsigned int g_uApp3NiHiShort;  //―――3节镍氢电池缺失态下限值

extern const unsigned int g_uApp4NiHiFull;   //―――4节镍氢电池饱和态下限值
extern const unsigned int g_uApp4NiHiNormal; //―――4节镍氢电池常态下限值
extern const unsigned int g_uApp4NiHiShort;  //―――4节镍氢电池缺失态下限值

extern const unsigned int g_uchApp1NormFull;   //―――1节一次性电池饱和态下限值
extern const unsigned int g_uchApp1NormNormal ; //―――1节一次性电池常态下限值
extern const unsigned int g_uchApp1NormShort ;  //―――1节一次性电池缺失态下限值

extern const unsigned int g_uchApp2NormFull ;   //―――2节一次性电池饱和态下限值
extern const unsigned int g_uchApp2NormNormal ; //―――2节一次性电池常态下限值
extern const unsigned int g_uchApp2NormShort ;  //―――2节一次性电池缺失态下限值

extern const unsigned int g_uApp1NiHiFull ;   //―――1节镍氢电池饱和态下限值
extern const unsigned int g_uApp1NiHiNormal ; //―――1节镍氢电池常态下限值
extern const unsigned int g_uApp1NiHiShort ;  //―――1节镍氢电池缺失态下限值

extern const unsigned int g_uApp2NiHiFull ;   //―――2节镍氢电池饱和态下限值
extern const unsigned int g_uApp2NiHiNormal ; //―――2节镍氢电池常态下限值
extern const unsigned int g_uApp2NiHiShort ;  //―――2节镍氢电池缺失态下限值

extern const unsigned int g_uAppLABat4Full;   //―――4V铅酸电池饱和态下限值
extern const unsigned int g_uAppLABat4Normal ; //―――4V铅酸电池常态下限值
extern const unsigned int g_uAppLABat4Short ;  //―――4V铅酸电池缺失态下限值

extern unsigned char g_uchConfigFlag;         //配置状态标志

extern unsigned char g_uPicControlFlag;        //向图像传感器发送控制命令


#endif 