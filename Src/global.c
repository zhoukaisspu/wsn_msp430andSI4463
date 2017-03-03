#include "global.h"

_PowerStatus g_eAppPowerStatus;  
_DevType g_eAppDevType;    
_DevInfo g_PHYDevInfo;


//基站当前注册的子设备数量 后4个bit（bit3~bit0）：
//已正式加入网络的设备； 前四个bit：暂未正式加入网络的设备
unsigned char g_uchDLLSubDevNumber = 0; 

_SubInfo g_SubDevInfo[NET_MAX_CAP]; //保存子设备的信息  

unsigned char g_uchNetStatus;                 //网络状态

unsigned char g_uchNetLev = 0xFF;                    //网络层全局变量，定义网络级别

unsigned int g_uNetParentID = 0;              //对于基站和端机而言，定义上级设备的网络ID
unsigned char g_uchNetParentFreqNo = 0xff;       //定义上级设备的频点编号
unsigned char g_uNetParentPAtype = 0;    //记录上级设备的功放类型

unsigned int g_uAppDataInt = 600;             //本定义应用层数据上报间隔时间 以秒为单位 


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

unsigned char g_uchAppchRoleSet = 0x00; //初始化为未知 

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
unsigned int g_uAppDataType = 0; 


//按照检测值与实际值 1：3比例测试
const unsigned int g_uAppFaraCapFull =0x0A3D ;  //――――法拉电容饱和态下限值；
const unsigned int g_uAppFaraCapNormal =0x92C ;// ――――法拉电容常态下限值；
const unsigned int g_uAppFaraCapShort = 0x69D; //――――法拉电容缺失态电压下限值；

//按照检测值与实际值 1.2：3.2比例测试
const unsigned int g_uApp3NiHiFull = 0x87F;   //―――3节镍氢电池饱和态下限值
const unsigned int g_uApp3NiHiNormal = 0x823; //―――3节镍氢电池常态下限值
const unsigned int g_uApp3NiHiShort = 0x7EB;  //―――3节镍氢电池缺失态下限值

const unsigned int g_uApp4NiHiFull = 0x0BF9;   //―――4节镍氢电池饱和态下限值
const unsigned int g_uApp4NiHiNormal = 0x0BDB; //―――4节镍氢电池常态下限值
const unsigned int g_uApp4NiHiShort = 0x0ACC;  //―――4节镍氢电池缺失态下限值

const unsigned int g_uchApp1NormFull = 0x31F;   //―――1节一次性电池饱和态下限值
const unsigned int g_uchApp1NormNormal = 0x2B0; //―――1节一次性电池常态下限值
const unsigned int g_uchApp1NormShort = 0x291;  //―――1节一次性电池缺失态下限值

const unsigned int g_uchApp2NormFull = 0x63F;   //―――2节一次性电池饱和态下限值
const unsigned int g_uchApp2NormNormal = 0x560; //―――2节一次性电池常态下限值
const unsigned int g_uchApp2NormShort = 0x522;  //―――2节一次性电池缺失态下限值

const unsigned int g_uApp1NiHiFull = 0x31F;   //―――1节镍氢电池饱和态下限值
const unsigned int g_uApp1NiHiNormal = 0x2B0; //―――1节镍氢电池常态下限值
const unsigned int g_uApp1NiHiShort = 0x291;  //―――1节镍氢电池缺失态下限值

const unsigned int g_uApp2NiHiFull = 0x63F;   //―――2节镍氢电池饱和态下限值
const unsigned int g_uApp2NiHiNormal = 0x560; //―――2节镍氢电池常态下限值
const unsigned int g_uApp2NiHiShort = 0x522;  //―――2节镍氢电池缺失态下限值

const unsigned int g_uAppLABat4Full = 0xA45;   //―――4V铅酸电池饱和态下限值
const unsigned int g_uAppLABat4Normal = 0xA08; //―――4V铅酸电池常态下限值
const unsigned int g_uAppLABat4Short = 0x9CA;  //―――4V铅酸电池缺失态下限值

unsigned char g_uchConfigFlag = 0;//1 为配置态

unsigned char g_uPicControlFlag = 0; //向图像传感器发送控制命令