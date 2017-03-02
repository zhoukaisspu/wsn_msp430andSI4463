/******************************************************************************/
/*            应用目标板：SI4463 B系列射频板                                  */
/*            内容描述：SI4463射频模块物理层驱动源文件                        */
/*            作者：周凯 ,上海第二工业大学                                    */
/*            版本号：V1.0.0                                                  */
/*            日期：2014.08.20                                                */
/*            修改者：                                                        */
/*            修改日期：                                                      */
/******************************************************************************/
#define uchar unsigned char  
#define uint  unsigned int 
/*
RF管脚定义：
   SDN:P8.2     关断控制
   NSEL:P8.1    SPI通讯允许选择...NSEL管脚拉低初始化通讯接口
   SCLK:P8.0    SPI时钟信号管脚，运行最大10MHz
   SDI:P7.3     数据输入管脚
   SDO:P7.2     数据输出管脚
   NIRQ:P1.6    中断管脚
   GPIO2:P8.4   功能待定???????
*/
//<注意>：在一些管脚中，当PXSEL的管脚选择第一功能的时候，就具有外部中断功能
//GPIO2管脚P8.4没有外部中断功能，所以也不存在P8IE
#define RF_SI_GPIO2_PxOUT        P8OUT       //输出电平控制
#define RF_SI_GPIO2_PxIN         P8IN        //输入电平判断
#define RF_SI_GPIO2_PxDIR        P8DIR        //方向控制
#define RF_SI_GPIO2_PxREN        P8REN        //上/下拉电阻使能控制
#define RF_SI_GPIO2_PxIES        P8IES        //中断产生条件控制
#define RF_SI_GPIO2_PxIFG        P8IFG        //状态标志位
#define RF_SI_GPIO2_PIN          BIT4          //管脚编号

#define RF_SI_NIRQ_PxOUT        P1OUT
#define RF_SI_NIRQ_PxIN         P1IN
#define RF_SI_NIRQ_PxDIR        P1DIR
#define RF_SI_NIRQ_PxREN        P1REN
#define RF_SI_NIRQ_PxIE         P1IE
#define RF_SI_NIRQ_PxIES        P1IES
#define RF_SI_NIRQ_PxIFG        P1IFG
#define RF_SI_NIRQ_PIN          BIT6

#define RF_SI_SDO_PxOUT        P7OUT
#define RF_SI_SDO_PxIN         P7IN
#define RF_SI_SDO_PxDIR        P7DIR
#define RF_SI_SDO_PxREN        P7REN
#define RF_SI_SDO_PIN          BIT2

#define RF_SI_SDI_PxOUT        P7OUT
#define RF_SI_SDI_PxIN         P7IN
#define RF_SI_SDI_PxDIR        P7DIR
#define RF_SI_SDI_PxREN        P7REN
#define RF_SI_SDI_PIN          BIT3

#define RF_SI_SCLK_PxOUT        P8OUT
#define RF_SI_SCLK_PxIN         P8IN
#define RF_SI_SCLK_PxDIR        P8DIR
#define RF_SI_SCLK_PxREN        P8REN
#define RF_SI_SCLK_PIN          BIT0

#define RF_SI_NSEL_PxOUT        P8OUT
#define RF_SI_NSEL_PxIN         P8IN
#define RF_SI_NSEL_PxDIR        P8DIR
#define RF_SI_NSEL_PxREN        P8REN
#define RF_SI_NSEL_PIN          BIT1

#define RF_SI_SDN_PxOUT        P8OUT
#define RF_SI_SDN_PxIN         P8IN
#define RF_SI_SDN_PxDIR        P8DIR
#define RF_SI_SDN_PxREN        P8REN
#define RF_SI_SDN_PIN          BIT2

//GPIO2相关配置    
#define SET_GPIO2_OUT (RF_SI_GPIO2_PxDIR |= RF_SI_GPIO2_PIN)//GPIO2输出
#define GPIO2_OUT_H   (RF_SI_GPIO2_PxOUT |= RF_SI_GPIO2_PIN)//GPIO2输出高电平
#define GPIO2_OUT_L   (RF_SI_GPIO2_PxOUT &= ~RF_SI_GPIO2_PIN)//GPIO2输出低电平
#define EN_GPIO2_RES  (RF_SI_GPIO2_PxREN |= RF_SI_GPIO2_PIN)//GPIO2带上拉电阻
#define SET_GPIO2_IN  (RF_SI_GPIO2_PxDIR &= ~RF_SI_GPIO2_PIN)//GPIO2为输入
#define GET_GPIO2_BIT  (RF_SI_GPIO2_PxIN & RF_SI_GPIO2_PIN)//获取当前GPIO2的位 
 
//NIRQ中断输入型 
#define SET_NIRQ_OUT  (RF_SI_NIRQ_PxDIR |= RF_SI_NIRQ_PIN)//NIRQ输出 
#define NIRQ_OUT_H    (RF_SI_NIRQ_PxOUT |= RF_SI_NIRQ_PIN)//NIRQ输出高电平 
#define EN_NIRQ_RES   (RF_SI_NIRQ_PxREN |= RF_SI_NIRQ_PIN)//NIRQ带上拉电阻
#define DIS_NIRQ_RES  (RF_SI_NIRQ_PxREN &= ~RF_SI_NIRQ_PIN)//NIRQ不带上拉/下拉电阻
#define SET_NIRQ_IN   (RF_SI_NIRQ_PxDIR &= ~RF_SI_NIRQ_PIN)//NIRQ为输入
#define SET_NIRQ_IES  (RF_SI_NIRQ_PxIES |= RF_SI_NIRQ_PIN)//NIRQ高到底中断
#define CLR_NIRQ_IFG  (RF_SI_NIRQ_PxIFG &= ~RF_SI_NIRQ_PIN)//清除中断标记
#define EINT_NIRQ     (RF_SI_NIRQ_PxIE |= RF_SI_NIRQ_PIN)//允许NIRQ中断
#define DINT_NIRQ     (RF_SI_NIRQ_PxIE &= ~RF_SI_NIRQ_PIN)//禁止NIRQ中断
#define GET_NIRQ_BIT  (RF_SI_NIRQ_PxIN & RF_SI_NIRQ_PIN)//获取当前NIRQ的位

//SDO输入  ==射频的输出对应MCU的输入
#define DIS_SDO_RES   (RF_SI_SDO_PxREN &= ~RF_SI_SDO_PIN)//SDO不要上拉/下拉电阻
#define EN_SDO_RES    (RF_SI_SDO_PxREN |= RF_SI_SDO_PIN)//SDO要下拉电阻
#define SDO_OUT_H     (RF_SI_SDO_PxOUT |= RF_SI_SDO_PIN)//SDO输出高电平
#define SDO_OUT_L     (RF_SI_SDO_PxOUT &= ~RF_SI_SDO_PIN)//SDO输出低电平
#define SET_SDO_IN    (RF_SI_SDO_PxDIR &= ~RF_SI_SDO_PIN)//定义SDO为输入
#define GET_SDO_BIT   (RF_SI_SDO_PxIN & RF_SI_SDO_PIN)//获取当前SDO的位

//SDI输出
#define DIS_SDI_RES   (RF_SI_SDI_PxREN &= ~RF_SI_SDI_PIN)//SDI不要上拉/下拉电阻
#define SET_SDI_OUT   (RF_SI_SDI_PxDIR |= RF_SI_SDI_PIN)//定义SDI为输出
#define SDI_OUT_H     (RF_SI_SDI_PxOUT |= RF_SI_SDI_PIN)//SDI输出高电平
#define SDI_OUT_L     (RF_SI_SDI_PxOUT &= ~RF_SI_SDI_PIN)//SDI输出低电平

//SCLK输出
#define DIS_SCLK_RES  (RF_SI_SCLK_PxREN &= ~RF_SI_SCLK_PIN)//SCLK不要上拉/下拉电阻
#define SET_SCLK_OUT  (RF_SI_SCLK_PxDIR |= RF_SI_SCLK_PIN)//定义SCLK为输出 
#define SCLK_OUT_H    (RF_SI_SCLK_PxOUT |= RF_SI_SCLK_PIN)//SCLK输出高电平
#define SCLK_OUT_L    (RF_SI_SCLK_PxOUT &= ~RF_SI_SCLK_PIN)//SCLK输出低电平

//NSEL输出
#define DIS_NSEL_RES  (RF_SI_NSEL_PxREN &= ~RF_SI_NSEL_PIN)//NSEL不要上拉/下拉电阻
#define SET_NSEL_OUT  (RF_SI_NSEL_PxDIR |= RF_SI_NSEL_PIN)//定义NSEL为输出 
#define NSEL_OUT_H    (RF_SI_NSEL_PxOUT |= RF_SI_NSEL_PIN)//NSEL输出高电平
#define NSEL_OUT_L    (RF_SI_NSEL_PxOUT &= ~RF_SI_NSEL_PIN)//NSEL输出低电平


//SDN输出
#define DIS_SDN_RES   (RF_SI_SDN_PxREN &= ~RF_SI_SDN_PIN)//SDN不要上拉/下拉电阻
#define SET_SDN_OUT   (RF_SI_SDN_PxDIR |= RF_SI_SDN_PIN)//定义SDN为输出
#define SDN_OUT_H     (RF_SI_SDN_PxOUT |= RF_SI_SDN_PIN)//SDN输出高电平
#define SDN_OUT_L     (RF_SI_SDN_PxOUT &= ~RF_SI_SDN_PIN)//SDN输出低电平

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Command Summary
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
/********************************
Boot Commands
*********************************/
#define CMD_POWER_UP  0x02
/********************************
Common Commands
*********************************/
#define CMD_NOP             0x00
#define CMD_PART_INFO       0x01
#define CMD_FUNC_INFO       0x10
#define CMD_SET_PROPERTY    0x11
#define CMD_GET_PROPERTY    0x12
#define CMD_GPIO_PIN_CFG    0x13
#define CMD_GET_ADC_READING 0x14
#define CMD_FIFO_INFO       0x15
#define CMD_PROTOCOL_CFG    0x18
#define CMD_GET_INT_STATUS  0x20
#define CMD_GET_PH_STATUS   0x21
#define CMD_GET_CHIP_STATUS 0x23
#define CMD_REQUEST_DEVICE_STATE 0x33
#define CMD_CHANGE_STATE    0x34
#define CMD_READ_CMD_BUFF   0x44 
#define CMD_FRR_A_READ      0x50
#define CMD_FRR_B_READ      0x51
#define CMD_FRR_C_READ      0x53
#define CMD_FRR_D_READ      0x57
/****************************************
IR_CAL_COMMANDS
*****************************************/
#define CMD_IRCAL           0x17
/****************************************
TX_COMMANDS
*****************************************/
#define CMD_START_TX        0x31
#define CMD_WRITE_TX_FIFO   0x66
/***************************************
RX_COMMANDS
****************************************/
#define CMD_PACKET_INFO     0x16
#define CMD_GET_MODEM_STATUS 0x22
#define CMD_START_RX        0x32
#define CMD_RX_HOP          0x36
#define CMD_READ_RX_FIFO    0x77
/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
Property Summary
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/**********************************
GLOBAL group:0x00
定义格式：
组名称宏定义
组内地址宏定义
***********************************/
#define Group_GLOBAL                  0x00          //组名称定义
#define ad_GLOBAL_XO_TUNE             0x00
#define ad_GLOBAL_CLK_CFG             0x01
#define ad_GLOBAL_LOW_BATT_THRESH     0x02
#define ad_GLOBAL_CONFIG              0x03
#define ad_GLOBAL_WUT_CONFIG          0x04
#define ad_GLOBAL_WUT_M_15_8          0x05
#define ad_GLOBAL_WUT_M_7_0           0x06
#define ad_GLOBAL_WUT_R               0x07
#define ad_GLOBAL_WUT_LDC             0x08
#define ad_GLOBAL_WUT_CAL             0x09
//算法
#define GLOBAL_XO_TUNE( x )                 API_SET_PROPERTY_1(Group_GLOBAL,ad_GLOBAL_XO_TUNE,x)
#define GLOBAL_CLK_CFG( x )                 API_SET_PROPERTY_1(Group_GLOBAL,ad_GLOBAL_CLK_CFG,x)
#define GLOBAL_LOW_BATT_THRESH( x )         API_SET_PROPERTY_1(Group_GLOBAL,ad_GLOBAL_LOW_BATT_THRESH,x)
#define GLOBAL_CONFIG( x )                  API_SET_PROPERTY_1(Group_GLOBAL,ad_GLOBAL_CONFIG,x)
#define GLOBAL_WUT_CONFIG( x )              API_SET_PROPERTY_1(Group_GLOBAL,ad_GLOBAL_WUT_CONFIG,x)
#define GLOBAL_WUT_M_15_8( x )              API_SET_PROPERTY_1(Group_GLOBAL,ad_GLOBAL_WUT_M_15_8,x)
#define GLOBAL_WUT_M_7_0( x )               API_SET_PROPERTY_1(Group_GLOBAL,ad_GLOBAL_WUT_M_7_0,x)
#define GLOBAL_WUT_R( x )                   API_SET_PROPERTY_1(Group_GLOBAL,ad_GLOBAL_WUT_R,x)
#define GLOBAL_WUT_LDC( x )                 API_SET_PROPERTY_1(Group_GLOBAL,ad_GLOBAL_WUT_LDC,x)
#define GLOBAL_WUT_CAL( x )                 API_SET_PROPERTY_1(Group_GLOBAL,ad_GLOBAL_WUT_CAL,x)
/******************************************
INT  group:0x01  中断寄存器
定义格式：
组名称宏定义
组内地址宏定义
*******************************************/
#define Grout_INT                     0x01
#define ad_INT_CTL_ENABLE             0x00
#define ad_INT_CTL_PH_ENABLE          0x01
#define ad_INT_CTL_MODEM_ENABLE       0x02
#define ad_INT_CTL_CHIP_ENABLE        0x03
//对单个寄存器进行设置
#define INT_CTL_ENABLE( x )                 API_SET_PROPERTY_1(Grout_INT,ad_INT_CTL_ENABLE,x)
#define INT_CTL_PH_ENABLE( x )              API_SET_PROPERTY_1(Grout_INT,ad_INT_CTL_PH_ENABLE,x)
#define INT_CTL_MODEM_ENABLE( x )           API_SET_PROPERTY_1(Grout_INT,ad_INT_CTL_MODEM_ENABLE,x)
#define INT_CTL_CHIP_ENABLE( x )            API_SET_PROPERTY_1(Grout_INT,ad_INT_CTL_CHIP_ENABLE,x)
//读取寄存器值
#define GET_INT_CTL_ENABLE(void)                 API_GET_PROPERTY_1(Grout_INT,ad_INT_CTL_ENABLE)
#define GET_INT_CTL_PH_ENABLE(void)              API_GET_PROPERTY_1(Grout_INT,ad_INT_CTL_PH_ENABLE)
#define GET_INT_CTL_MODEM_ENABLE(void)           API_GET_PROPERTY_1(Grout_INT,ad_INT_CTL_MODEM_ENABLE)
#define GET_INT_CTL_CHIP_ENABLE(void)            API_GET_PROPERTY_1(Grout_INT,ad_INT_CTL_CHIP_ENABLE)
/*****************************************
FRR  group:0x02   快速反应寄存器 
定义格式：
组名称宏定义
组内地址宏定义
******************************************/
#define Grout_FRR                     0x02
#define ad_FRR_CTL_A_MODE             0x00
#define ad_FRR_CTL_B_MODE             0x01
#define ad_FRR_CTL_C_MODE             0x02
#define ad_FRR_CTL_D_MODE             0x03
//寄存器配置
#define FRR_CTL_A_MODE( x )                 API_SET_PROPERTY_1(Grout_FRR,ad_FRR_CTL_A_MODE,x)
#define FRR_CTL_B_MODE( x )                 API_SET_PROPERTY_1(Grout_FRR,ad_FRR_CTL_B_MODE,x)
#define FRR_CTL_C_MODE( x )                 API_SET_PROPERTY_1(Grout_FRR,ad_FRR_CTL_C_MODE,x)
#define FRR_CTL_D_MODE( x )                 API_SET_PROPERTY_1(Grout_FRR,ad_FRR_CTL_D_MODE,x)
/******************************************
PREAMBLE  group:0x10   前导码寄存器 
定义格式：
组名称宏定义
组内地址宏定义
*******************************************/
#define Group_PREAMBLE                0x10
#define ad_PREAMBLE_TX_LENGTH         0x00
#define ad_PREAMBLE_CONFIG_STD_1      0x01
#define ad_PREAMBLE_CONFIG_NSTD       0x02
#define ad_PREAMBLE_CONFIG_STD_2      0x03
#define ad_PREAMBLE_CONFIG            0x04
#define ad_PREAMBLE_PATTERN_31_24     0x05
#define ad_PREAMBLE_PATTERN_23_16     0x06
#define ad_PREAMBLE_PATTERN_15_8      0x07
#define ad_PREAMBLE_PATTERN_7_0       0x08
//寄存器配置
#define PREAMBLE_TX_LENGTH(x)             API_SET_PROPERTY_1(Group_PREAMBLE,ad_PREAMBLE_TX_LENGTH,x)      //0X0A
#define PREAMBLE_CONFIG(x)                API_SET_PROPERTY_1(Group_PREAMBLE,ad_PREAMBLE_CONFIG,x)         //0X31
#define PREAMBLE_CONFIG_STD1( x )           API_SET_PROPERTY_1(Group_PREAMBLE,ad_PREAMBLE_CONFIG_STD_1,x)   //
#define PREAMBLE_CONFIG_STD2( x )           API_SET_PROPERTY_1(Group_PREAMBLE,ad_PREAMBLE_CONFIG_STD_2,x)   //
/*******************************************
SYNC  group:0x11    同步字配置寄存器
定义格式：
组名称宏定义
组内地址宏定义
********************************************/
#define Group_SYNC                    0x11
#define ad_SYNC_CONFIG                0x00
#define ad_SYNC_BITS_31_24            0x01
#define ad_SYNC_BITS_23_16            0x02
#define ad_SYNC_BITS_15_8             0x03
#define ad_SYNC_BITS_7_0              0x04
//寄存器配置
#define SYNC_CONFIG( x )                   API_SET_PROPERTY_1(Group_SYNC,ad_SYNC_CONFIG,x) //0X01
#define SYNC_BITS_31_24( x )               API_SET_PROPERTY_1(Group_SYNC,ad_SYNC_BITS_31_24,x) //0XB4
#define SYNC_BITS_23_16( x )               API_SET_PROPERTY_1(Group_SYNC,ad_SYNC_BITS_23_16,x) //0X2B
#define SYNC_BITS_15_8( x )                API_SET_PROPERTY_1(Group_SYNC,ad_SYNC_BITS_15_8,x) //
#define SYNC_BITS_7_0( x )                 API_SET_PROPERTY_1(Group_SYNC,ad_SYNC_BITS_7_0,x) //
/********************************************
PKT grout:0x12    数据包配置
定义格式：
组名称宏定义
组内地址宏定义
********************************************/
#define Group_PKT                     0x12
#define ad_PKT_CRC_CONFIG             0x00
#define ad_PKT_WHT_POLY_15_8          0x01
#define ad_PKT_WHT_POLY_7_0           0x02
#define ad_PKT_WHT_SEED_15_8          0x03
#define ad_PKT_WHT_SEED_7_0           0x04
#define ad_PKT_WHT_BIT_NUM            0x05
#define ad_PKT_CONFIG1                0x06
#define ad_PKT_LEN                    0x08
#define ad_PKT_LEN_FIELD_SOURCE       0x09
#define ad_PKT_LEN_ADJUST             0x0a
#define ad_PKT_TX_THRESHOLD           0x0b
#define ad_PKT_RX_THRESHOLD           0x0c
#define ad_PKT_FIELD_1_LENGTH_15_8    0x0d
#define ad_PKT_FIELD_1_LENGTH_7_0     0x0e
#define ad_PKT_FIELD_1_CONFIG         0x0f
#define ad_PKT_FIELD_1_CRC_CONFIG     0x10
#define ad_PKT_FIELD_2_LENGTH_15_8    0x11
#define ad_PKT_FIELD_2_LENGTH_7_0     0x12
#define ad_PKT_FIELD_2_CONFIG         0x13
#define ad_PKT_FIELD_2_CRC_CONFIG     0x14
#define ad_PKT_FIELD_3_LENGTH_15_8    0x15
#define ad_PKT_FIELD_3_LENGTH_7_0     0x16
#define ad_PKT_FIELD_3_CONFIG         0x17
#define ad_PKT_FIELD_3_CRC_CONFIG     0x18
#define ad_PKT_FIELD_4_LENGTH_15_8    0x19
#define ad_PKT_FIELD_4_LENGTH_7_0     0x1a
#define ad_PKT_FIELD_4_CONFIG         0x1b
#define ad_PKT_FIELD_4_CRC_CONFIG     0x1c 
#define ad_PKT_FIELD_5_LENGTH_15_8    0x1d
#define ad_PKT_FIELD_5_LENGTH_7_0     0x1e
#define ad_PKT_FIELD_5_CONFIG         0x1f
#define ad_PKT_FIELD_5_CRC_CONFIG     0x20
#define ad_PKT_RX_FIELD_1_LENGTH_15_8 0x21
#define ad_PKT_RX_FIELD_1_LENGTH_7_0  0x22
#define ad_PKT_RX_FIELD_1_CONFIG      0x23
#define ad_PKT_RX_FIELD_1_CRC_CONFIG  0x24
#define ad_PKT_RX_FIELD_2_LENGTH_15_8 0x25
#define ad_PKT_RX_FIELD_2_LENGTH_7_0  0x26
#define ad_PKT_RX_FIELD_2_CONFIG      0x27
#define ad_PKT_RX_FIELD_2_CRC_CONFIG  0x28
#define ad_PKT_RX_FIELD_3_LENGTH_15_8 0x29
#define ad_PKT_RX_FIELD_3_LENGTH_7_0  0x2a
#define ad_PKT_RX_FIELD_3_CONFIG      0x2b
#define ad_PKT_RX_FIELD_3_CRC_CONFIG  0x2c
#define ad_PKT_RX_FIELD_4_LENGTH_15_8 0x2d
#define ad_PKT_RX_FIELD_4_LENGTH_7_0  0x2e
#define ad_PKT_RX_FIELD_4_CONFIG      0x2f
#define ad_PKT_RX_FIELD_4_CRC_CONFIG  0x30
#define ad_PKT_RX_FIELD_5_LENGTH_15_8 0x31
#define ad_PKT_RX_FIELD_5_LENGTH_7_0  0x32
#define ad_PKT_RX_FIELD_5_CONFIG      0x33
#define ad_PKT_RX_FIELD_5_CRC_CONFIG  0x34

//寄存器配置
#define PKT_CONFIG1( x )                   API_SET_PROPERTY_1(Group_PKT,ad_PKT_CONFIG1,x) //0X02
#define PKT_LEN(x)                         API_SET_PROPERTY_1(Group_PKT,ad_PKT_LEN,x) //0X02                   
#define PKT_LEN_FIELD_SOURCE(x)            API_SET_PROPERTY_1(Group_PKT,ad_PKT_LEN_FIELD_SOURCE,x)       
#define PKT_LEN_ADJUST(x)                  API_SET_PROPERTY_1(Group_PKT,ad_PKT_LEN_ADJUST,x)              



#define PKT_FIELD_1_CONFIG( x )            API_SET_PROPERTY_1(Group_PKT,ad_PKT_FIELD_1_CONFIG,x) //0X04
#define PKT_FIELD_1_LENGTH_7_0( x )        API_SET_PROPERTY_1(Group_PKT,ad_PKT_FIELD_1_LENGTH_7_0,x) //0X07
#define PKT_FIELD_1_LENGTH_12_8( x )       API_SET_PROPERTY_1(Group_PKT,ad_PKT_FIELD_1_LENGTH_15_8,x) //0X00
#define PKT_FIELD_1_CRC_CONFIG( x )        API_SET_PROPERTY_1(Group_PKT,ad_PKT_FIELD_1_CRC_CONFIG,x) //0X07

#define PKT_FIELD_2_LENGTH_7_0(x)          API_SET_PROPERTY_1(Group_PKT,ad_PKT_FIELD_2_LENGTH_7_0,x) //0X07
#define PKT_FIELD_2_LENGTH_15_8(x)         API_SET_PROPERTY_1(Group_PKT,ad_PKT_FIELD_2_LENGTH_15_8,x) //0X07
#define PKT_FIELD_2_CONFIG(x)             API_SET_PROPERTY_1(Group_PKT,ad_PKT_FIELD_2_CONFIG,x) //0X07
#define PKT_FIELD_2_CRC_CONFIG(x)          API_SET_PROPERTY_1(Group_PKT,ad_PKT_FIELD_2_CRC_CONFIG,x) //0X07

#define PKT_CRC_CONFIG( x )                API_SET_PROPERTY_1(Group_PKT,ad_PKT_CRC_CONFIG,x) //0X00
#define PKT_TX_THRESHOLD(x)                API_SET_PROPERTY_1(Group_PKT,ad_PKT_TX_THRESHOLD,x)
#define PKT_RX_THRESHOLD(x)                API_SET_PROPERTY_1(Group_PKT,ad_PKT_RX_THRESHOLD,x)
/***********************************************
MODEM  group:0x20  调制模式配置寄存器
定义格式：
组名称宏定义
组内地址宏定义
************************************************/
#define Group_MODEM                   0x20
#define ad_MODEM_MOD_TYPE             0x00
#define ad_MODEM_MAP_CONTROL          0x01
#define ad_MODEM_DSM_CTRL             0x02
#define ad_MODEM_DATA_RATE_2          0x03
#define ad_MODEM_DATA_RATE_1          0x04
#define ad_MODEM_DATA_RATE_0          0x05
#define ad_MODEM_TX_NCO_MODEM_3       0x06
#define ad_MODEM_TX_NCO_MODEM_2       0x07
#define ad_MODEM_TX_NCO_MODEM_1       0x08
#define ad_MODEM_TX_NCO_MODEM_0       0x09
#define ad_MODEM_FREQ_DEV_2           0x0a
#define ad_MODEM_FREQ_DEV_1           0x0b
#define ad_MODEM_FREQ_DEV_0           0x0c
#define ad_MODEM_FREQ_OFFSET_1        0x0d
#define ad_MODEM_FREQ_OFFSET_0        0x0e
#define ad_MODEM_TX_FILTER_COEFF_8    0x0f
#define ad_MODEM_TX_FILTER_COEFF_7    0x10
#define ad_MODEM_TX_FILTER_COEFF_6    0x11
#define ad_MODEM_TX_FILTER_COEFF_5    0x12
#define ad_MODEM_TX_FILTER_COEFF_4    0x13
#define ad_MODEM_TX_FILTER_COEFF_3    0x14
#define ad_MODEM_TX_FILTER_COEFF_2    0x15
#define ad_MODEM_TX_FILTER_COEFF_1    0x16
#define ad_MODEM_TX_FILTER_COEFF_0    0x17
#define ad_MODEM_TX_RAMP_DELAY        0x18
#define ad_MODEM_MDM_CTRL             0x19
#define ad_MODEM_IF_CONTROL           0x1a
#define ad_MODEM_IF_FREQ_2            0x1b
#define ad_MODEM_IF_FREQ_1            0x1c
#define ad_MODEM_IF_FREQ_0            0x1d
#define ad_MODEM_DECIMATION_CFG1      0x1e
#define ad_MODEM_DECIMATION_CFG0      0x1f
#define ad_MODEM_BCR_OSR_1            0x22
#define ad_MODEM_BCR_OSR_0            0x23
#define ad_MODEM_BCR_NCO_OFFSET_2     0x24
#define ad_MODEM_BCR_NCO_OFFSET_1     0x25
#define ad_MODEM_BCR_NCO_OFFSET_0     0x26
#define ad_MODEM_BCR_GAIN_1           0x27
#define ad_MODEM_BCR_GAIN_0           0x28
#define ad_MODEM_BCR_GEAR             0x29
#define ad_MODEM_BCR_MISC1            0x2a
#define ad_MODEM_BCR_MISC0            0x2b
#define ad_MODEM_AFC_GEAR             0x2c
#define ad_MODEM_AFC_WAIT             0x2d
#define ad_MODEM_AFC_GAIN_1           0x2e
#define ad_MODEM_AFC_GAIN_0           0x2f
#define ad_MODEM_AFC_LIMITER_1        0x30
#define ad_MODEM_AFC_LIMITER_0        0x31
#define ad_MODEM_AFC_MISC             0x32
#define ad_MODEM_AFC_ZIFOFF           0x33
#define ad_MODEM_ADC_CTRL             0x34
#define ad_MODEM_AGC_CONTROL          0x35
#define ad_MODEM_AGC_WINDOW_SIZE      0x38
#define ad_MODEM_AGC_RFPD_DECAY       0x39
#define ad_MODEM_AGC_IFPD_DECAY       0x3a
#define ad_MODEM_FSK4_GAIN1           0x3b
#define ad_MODEM_FSK4_GAIN0           0x3c
#define ad_MODEM_FSK4_TH_1            0x3d
#define ad_MODEM_FSK4_TH_0            0x3e
#define ad_MODEM_FSK4_MAP             0x3f
#define ad_MODEM_OOK_PDTC             0x40
#define ad_MODEM_OOK_BLOPK            0x41
#define ad_MODEM_OOK_CNT1             0x42
#define ad_MODEM_OOK_MISC             0x43
#define ad_MODEM_RAW_SEARCH           0x44
#define ad_MODEM_RAW_CONTROL          0x45
#define ad_MODEM_RAW_EYE_1            0x46
#define ad_MODEM_RAW_EYE_0            0x47
#define ad_MODEM_ANT_DIV_MODE         0x48
#define ad_MODEM_ANT_DIV_CONTROL      0x49
#define ad_MODEM_RSSI_THRESH          0x4a
#define ad_MODEM_RSSI_JUMP_THRESH     0x4b
#define ad_MODEM_RSSI_CONTROL         0x4c
#define ad_MODEM_RSSI_CONTROL2        0x4d
#define ad_MODEM_RSSI_COMP            0x4e
#define ad_MODEM_CLKGEN_BAND          0x51
//寄存器配置
#define MODEM_MOD_TYPE( x )                   API_SET_PROPERTY_1(0X20,0X00,x) 
#define MODEM_MAP_CONTROL( x )                API_SET_PROPERTY_1(0X20,0X01,x) 
#define MODEM_DSM_CONTROL( x )                API_SET_PROPERTY_1(0X20,0X02,x) 
#define MODEM_DATA_RATE_2( x )                API_SET_PROPERTY_1(0X20,0X03,x) 
#define MODEM_DATA_RATE_1( x )                API_SET_PROPERTY_1(0X20,0X04,x) 
#define MODEM_DATA_RATE_0( x )                API_SET_PROPERTY_1(0X20,0X05,x) 
#define MODEM_TX_NCO_MODE_3(x)                API_SET_PROPERTY_1(0X20,0X06,x)
#define MODEM_TX_NCO_MODE_2(x)                API_SET_PROPERTY_1(0X20,0X07,x)
#define MODEM_TX_NCO_MODE_1(x)                API_SET_PROPERTY_1(0X20,0X08,x)
#define MODEM_TX_NCO_MODE_0(x)                API_SET_PROPERTY_1(0X20,0X09,x)
#define MODEM_FREQ_DEV_2(x)                   API_SET_PROPERTY_1(0X20,0X0A,x)
#define MODEM_FREQ_DEV_1(x)                   API_SET_PROPERTY_1(0X20,0X0B,x)
#define MODEM_FREQ_DEV_0(x)                   API_SET_PROPERTY_1(0X20,0X0C,x)
#define MODEM_TX_RAMP_DELAY(x)                API_SET_PROPERTY_1(0X20,0X18,x)
#define MODEM_IF_CONTROL(x)                   API_SET_PROPERTY_1(Group_MODEM,ad_MODEM_IF_CONTROL,x)
#define MODEM_IF_FREQ_2(x)                    API_SET_PROPERTY_1(Group_MODEM,ad_MODEM_IF_FREQ_2,x)
#define MODEM_IF_FREQ_1(x)                    API_SET_PROPERTY_1(Group_MODEM,ad_MODEM_IF_FREQ_1,x)
#define MODEM_IF_FREQ_0(x)                    API_SET_PROPERTY_1(Group_MODEM,ad_MODEM_IF_FREQ_0,x)
#define MODEM_DECIMATION_CFG1(x)              API_SET_PROPERTY_1(Group_MODEM,ad_MODEM_DECIMATION_CFG1,x)
#define MODEM_DECIMATION_CFG0(x)              API_SET_PROPERTY_1(Group_MODEM,ad_MODEM_DECIMATION_CFG0,x)
#define MODEM_CLKGEN_BAND(x)                  API_SET_PROPERTY_1(Group_MODEM,0X51,x)
#define MODEM_RSSI_THRESH(x)                  API_SET_PROPERTY_1(Group_MODEM,ad_MODEM_RSSI_THRESH,x)
#define MODEM_RSSI_JUMP_THRESH(x)             API_SET_PROPERTY_1(Group_MODEM,ad_MODEM_RSSI_JUMP_THRESH,x)
#define MODEM_RSSI_CONTROL(x)                 API_SET_PROPERTY_1(Group_MODEM,ad_MODEM_RSSI_CONTROL,x)
#define MODEM_RSSI_CONTROL2(x)                API_SET_PROPERTY_1(Group_MODEM,ad_MODEM_RSSI_CONTROL2,x)
#define MODEM_RSSI_COMP(x)                    API_SET_PROPERTY_1(Group_MODEM,ad_MODEM_RSSI_COMP,x)
/**********************************************
MODEM_CHFLT group:0x21
定义格式：
组名称宏定义
组内地址宏定义
**********************************************/
#define Group_MODEM_CHFLT                     0x21
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_13       0x00
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_12       0x01
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_11       0x02
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_10       0x03
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_9        0x04
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_8        0x05
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_7        0x06
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_6        0x07
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_5        0x08
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_4        0x09
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_3        0x0a
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_2        0x0b
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_1        0x0c
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_0        0x0d
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_10_13    0x0e
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_6_9      0x0f
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_2_5      0x10
#define ad_MODEM_CHFLT_RX1_CHFLT_COE_0_1      0x11
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_13       0x12
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_12       0x13
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_11       0x14
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_10       0x15
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_9        0x16
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_8        0x17
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_7        0x18
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_6        0x19
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_5        0x1a
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_4        0x1b
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_3        0x1c
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_2        0x1d
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_1        0x1e 
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_0        0x1f
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_10_13    0x20
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_6_9      0x21
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_2_5      0x22
#define ad_MODEM_CHFLT_RX2_CHFLT_COE_0_1      0x23
//寄存器配置

/******************************************************
PA group:0x22 功率放大器配置寄存器
定义格式：
组名称宏定义
组内地址宏定义
*******************************************************/
#define Group_PA                               0x22
#define ad_PA_MODE                             0x00
#define ad_PA_PWR_LVL                          0x01
#define ad_PA_BIAS_CLKDUTY                     0x02
#define ad_PA_TC                               0x03
#define ad_PA_RAMP_EX                          0x04
#define ad_PA_RAMP_DOWN_DELAY                  0x05
//寄存器配置
#define PA_MODE( x )                       API_SET_PROPERTY_1(0X22,0X00,x) //0X08
#define PA_PWR_LVL( x )                    API_SET_PROPERTY_1(0X22,0X01,x) //0X7F
#define PA_BIAS_CLKDUTY( x )               API_SET_PROPERTY_1(0X22,0X02,x) //0X00
#define PA_TC( x )                         API_SET_PROPERTY_1(0X22,0X03,x) //0X3D
/******************************************************
SYNTH group:0x23
定义格式：
组名称宏定义
组内地址宏定义
*******************************************************/
#define Group_SYNTH                           0x23
#define ad_SYNTH_PFDCP_CPFF                   0x00
#define ad_SYNTH_PFDCP_CPINT                  0x01
#define ad_SYNTH_VCO_KV                       0x02
#define ad_SYNTH_LPFILT3                      0x03
#define ad_SYNTH_LPFILT2                      0x04
#define ad_SYNTH_LPFILT1                      0x05
#define ad_SYNTH_LPFILT0                      0x06
#define ad_SYNTH_VCO_KVCAL                    0x07
//寄存器配置
#define SYNTH_PFDCP_CPFF(x)             API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_PFDCP_CPFF,x) 
#define SYNTH_PFDCP_CPINT(x)            API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_PFDCP_CPINT,x)
#define VCO_KV(x)                       API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_VCO_KV,x)
#define SYNTH_LPFILT3(x)                API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_LPFILT3,x) 
#define SYNTH_LPFILT2(x)                API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_LPFILT2,x)
#define SYNTH_LPFILT1(x)                API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_LPFILT1,x)
#define SYNTH_LPFILT0(x)                API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_LPFILT0,x)
/*******************************************************
MATCH group:0x30
定义格式：
组名称宏定义
组内地址宏定义
********************************************************/
#define Group_MATCH                           0x30
#define ad_MATCH_VALUE_1                      0x00
#define ad_MATCH_MASK_1                       0x01
#define ad_MATCH_CTRL_1                       0x02
#define ad_MATCH_VALUE_2                      0x03
#define ad_MATCH_MASK_2                       0x04
#define ad_MATCH_CTRL_2                       0x05
#define ad_MATCH_VALUE_3                      0x06
#define ad_MATCH_ MASK_3                       0x07
#define ad_MATCH_CTRL_3                       0x08
#define ad_MATCH_VALUE_4                      0x09
#define ad_MATCH_MASK_4                       0x0a
#define ad_MATCH_CTRL_4                       0x0b
/******************************************************
FREQ_CONTROL group:0x40
定义格式：
组名称宏定义
组内地址宏定义
*******************************************************/
#define Group_FREQ_CONTROL                    0x40
#define ad_FREQ_CONTROL_INTE                  0x00
#define ad_FREQ_CONTROL_FRAC_2                0x01
#define ad_FREQ_CONTROL_FRAC_1                0x02
#define ad_FREQ_CONTROL_FRAC_0                0x03
#define ad_FREQ_CONTROL_CHANNEL_STEP_SIZE_1   0x04
#define ad_FREQ_CONTROL_CHANNEL_STEP_SIZE_0   0x05
#define ad_FREQ_CONTROL_W_SIZE                0x06
#define ad_FREQ_CONTROL_VCOCNT_RX_ADJ         0x07
//寄存器配置
#define FREQ_CONTROL_INTE(x)                             API_SET_PROPERTY_1(0X40,0X00,x)
#define FREQ_CONTROL_FRAC_2(x)                           API_SET_PROPERTY_1(0X40,0X01,x)
#define FREQ_CONTROL_FRAC_1(x)                           API_SET_PROPERTY_1(0X40,0X02,x)  
#define FREQ_CONTROL_FRAC_0(x)                           API_SET_PROPERTY_1(0X40,0X03,x)
#define FREQ_CONTROL_CHANNEL_STEP_SIZE_1(x)              API_SET_PROPERTY_1(0X40,0X04,x)
#define FREQ_CONTROL_CHANNEL_STEP_SIZE_0(x)              API_SET_PROPERTY_1(0X40,0X05,x)
#define FREQ_CONTROL_W_SIZE(x)                           API_SET_PROPERTY_1(0X40,0X06,x)
#define FREQ_CONTROL_VCOCNT_RX_ADJ(x)                    API_SET_PROPERTY_1(0X40,0X07,x)
/*****************************************************
RX_HOP  group:0x50
定义格式：
组名称宏定义
组内地址宏定义
******************************************************/
#define Group_RX_HOP                          0x50
#define ad_RX_HOP_CONTROL                     0x00
#define ad_RX_HOP_TABLE_SIZE                  0x01
#define ad_RX_HOP_TABLE_ENTRY_0               0x02
/*后面包括ad_RX_HOP_TABLE_ENTRY[0-63]宏定义，不一一列举，
使用的时候自行添加*/
/*<><><><><><><><><><><><><><><><><><><><><><><><><><><><>*/
/*******************************
CTS校验最大值配置
********************************/
#define MAX_CTS_RETRY 2500     //定义CTS读取最多2500次，否则视为错误

//RF状态定义
#define RF_Shutdown 0
#define RF_Standby  1
#define RF_Sleep    2
#define RF_SPIactive 3
#define RF_Ready    4
#define RF_TxTune   5
#define RF_RxTune   6
#define RF_Tx       7
#define RF_Rx       8


#define RF_SHUTDOWN  RF_Shutdown
#define RF_STANDBY   RF_Standby
#define RF_SLEEP     RF_Sleep
#define RF_READY     RF_Ready
#define RF_RX        RF_Rx
#define RF_TX        RF_Tx
extern unsigned char g_uchrRFMode;  //RF状态标志


//以下为射频物理层频道参数
//下面的宏定义给出了频点分布范围
#define RF_FRE_A_AMOUNT   5   //射频物理层定义A区频点数量
#define RF_FRE_B_AMOUNT   0   //射频物理层定义B区频点数量
#define RF_FRE_AMOUNT    (RF_FRE_A_AMOUNT+RF_FRE_B_AMOUNT)  //系统总共可用频点数量


//设定RSSI的下限对应值
#define RF_RSSI_BEST    -80  //0xF6     //设定监测到的"优"信号强度下限值  ，取-80dbm   
#define RF_RSSI_BETTER  -85  //0xEC     //设定"好"信号强度下限值   取-85dbm
#define RF_RSSI_GOOD    -90  //0xE2     //设定监测到的"较好"信号强度下限值  取-90dbm
#define RF_RSSI_NORMAL  -95  //0xD8     //设定一般信号强度下限值    取－95dBm

#define RF_TRANSMIT_RSSI  -85//-90//0xE2 //无线通信传输范围载波信号下限功率值-90dBm
#define RF_LISTEN_RSSI    -75//-95//-100//0xCE //无线通信侦听范围载波信号下限功率值-100dBm
#define RF_RSSI_LOGON    RF_TRANSMIT_RSSI //设定注册信号强度下限值，和设备传输范围信号功率值相同－90dBm


extern unsigned char g_uchRFSelfFreNum; //物理层全局变量，定义自身的工作频道号
extern unsigned char g_uchRFCurFreNum;  //射频物理层全局变量，定义当前射频所在的频点编号
extern unsigned char g_chRFFreList[RF_FRE_AMOUNT];   //频点序列组


#define MAX_RF_PA_SET   0x80   //4463射频内置功放档值
extern const unsigned char g_uchRFPowerVal[MAX_RF_PA_SET];
extern unsigned char g_uchRFPALev; // 定义射频全局变量功率档值g_uchRFPowerVal的级别，即g_uchRFPowerVal的当前下标值
extern unsigned char g_uchRxPAtype;  // 表示接收设备的功放类型 0x0A --A型设备 无功放；0x0B --B型设备 有功放； 0x00 --未知


extern unsigned char g_uchRSSI;     //接收到的无线信号的RSSI

/***************************************************************
RF的状态初始化
***************************************************************/
extern void RFCtlLineInit(void);

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                          子函数定义
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/******************************************************************************/
/**************************************************************
基本操作子函数
***************************************************************/
/*
发送一个字节的数据
*/
extern void  API_SendDataByte(uchar Data) ;
/*
发送多个字节的数据
*/
extern void API_SendDataNoResponse(uchar DataLength,uchar *Data) ;
/*
发送命令后取回复数据
形参：回复数据的长度、存储回复数据的数组头指针
*/
extern void API_SendDataGetResponse(uchar DataLength,uchar *Data);
/*
发送命令
形参：命令长度，存储命令的数组头指针
*/
extern uchar API_SendCommand(uchar CmdLength,uchar *CmdData);
/*
获取回复的数据
形参：回复数据的长度，存储回复数据的数组头指针
*/
extern uchar API_GetResponse(uchar DataLength,uchar *Data);

/*************************************************************
等待命令完成
返回值：0:命令处理成功  1：命令处理失败
*************************************************************/
extern uchar API_WaitforCTS(void);

/*************************************************************************/


/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                SI4463开发应用函数
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
/*************************************************************
读取设备信息
*************************************************************/
extern uchar API_PART_INFO(uchar PARTInfoLength,uchar *PARTInfo);

/************************************************************
读取设备现在所处的状态
形参：存储返回数据的数组指针
返回值：0x00:获取返回值成功 
        0x01:获取返回值失败
************************************************************/
extern uchar API_REQUEST_DEVICE_STATE(uchar *RFStatus);

/*************************************************************
函数1：读取状态标志位，并清除状态标志位
函数2：读取Packet Handler的中断标志位，但是不一定清除中断
函数3：读取Modem的中断标志位，但是不一定清除中断位
函数4：读取Chip的中断标志位，但是不一定清除中断位。
*************************************************************/
//函数1：读取状态标志位，并清除状态标志位
extern uchar  API_Get_All_IntStatus(uchar IntStatusLength,uchar *IntStatus);

//函数2：读取Packet Handler的中断标志位，但是不一定清除中断
extern uchar  API_Get_PH_IntStatus(uchar PHIntStatusLength,uchar *PHIntStatus);

//清除数据包处理的中断标志位
extern void API_ClearPHInt(void);

//函数3：读取Modem的中断标志位，但是不一定清除中断位
extern uchar  API_Get_Modem_IntStatus(uchar ModemIntStatusLength,uchar *ModemIntStatus);

//函数4：读取Chip的中断标志位，但是不一定清除中断位。
extern uchar  API_Get_Chip_IntStatus(uchar ChipIntStatusLength,uchar *ChipIntStatus);
/*****************************************************************
函数1：清空TxFIFO
函数2：清空RxFIFO
******************************************************************/
/*
清空TxFIFO
返回值：0xff清空错误
        0x40:清空成功
*/
extern uchar API_ClearTxFIFO(void);
extern uchar RFResetRxFIFO(void);    //对于链路层函数

/*
清空RxFIFO
返回值：0xff:清空失败
        0x40:清空成功
*/
extern uchar API_ClearRxFIFO(void);
extern uchar RFResetTxFIFO(void);    //对于链路层函数
/*
写入TXDataBuffer
形参：写入数据长度，数组头指针
*/
extern uchar API_WriteTxDataBuffer(uchar TxFifoLenght,uchar *TxFifoData);
/*
读出RxDataBuffer
形参:读出的数据长度，接收数据数组的头指针
*/
extern uchar API_ReadRxDataBuffer(uchar RxFifoLength,uchar *RxFifoData);
/*
读出FIFO的空间使用情况
形参：存储返回数据的数组头指针
*/
extern void API_FIFO_INFO(uchar *FIFO_INFO);

/*********************************************************
读取快速反应寄存器里的存储值
形参：bStartReg：寄存器的开始地址
      bNumberOfReg：读取寄存器的个数
      RegValues： 存储寄存器的二维数组头指针
返回值：0x01:写入地址错误
        0x00:读取数据成功
**********************************************************/
uchar API_GetFastResponseRegister(uchar bStartReg,uchar bNumberOfReg,uchar *RegValues);

/*********************************************************
RF的GPIO配置
形参：GPIO0：GPIO0配置方式
      GPIO1：GPIO1配置方式
      GPIO2：GPIO2配置方式
*********************************************************/
extern void API_GPIO_CFG(uchar GPIO0,uchar GPIO1,uchar GPIO2,uchar GPIO3);

/**********************************************************
改变射频模块的状态
**********************************************************/
extern uchar API_CHANGE_STATE(uchar RFStatus);

/**********************************************************
上电芯片初始化
**********************************************************/
extern void API_POWER_UP(void);


/***********************************************************
寄存器设置函数
函数一：设置一个寄存器
函数二：连续设置一个组内的多个寄存器
************************************************************/
/*寄存器的设置，对同一个组的多个寄存器连续进行设置
形参：group：寄存器所属的组
      num_pro：需要连续设置寄存器的个数
     start_pro：设置寄存器组的初始地址
     *Data：寄存器设置的数据
返回值:0：一次性配置寄存器成功
       1：一次性配置的寄存器过多，配置失败
        
*/
extern uchar API_SET_PROPERTY_X(unsigned char group,unsigned char num_pro,unsigned char start_pro,unsigned char *Data); 

/*
对一个寄存器进行设置
形参：group:该寄存器所属的组
      address：该寄存器的地址
      Data：对该寄存器配置的数据
返回值：0：对寄存器的设置成功
        1：对寄存器的设置失败
*/
 extern uchar API_SET_PROPERTY_1(unsigned char group,unsigned char address,unsigned char Data);
 
 /*
 读取一个寄存器的值
 形参：该寄存器所属的组，寄存器的地址
 返回值：读取的寄存器的值
 */
 extern uchar API_GET_PROPERTY_1(unsigned char group,unsigned char start_pro);
 
/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
使用的函数定义
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/**************************************************************/
/*设定RF进入到Power Down模式   耗电量1                        */
/**************************************************************/
extern void SetRFOff(void);

/***************************************************************/
/*设定RF开机                                                   */
/***************************************************************/
extern void SetRFOn(void);

/***************************************************************/
/*设定RF进入Standby模式     耗电量2                             */
/****************************************************************/
extern void SetRFStandby(void);

/***************************************************************/
/*设定RF进入Sleep模式    耗电量3                               */
/****************************************************************/
extern void SetRFSleep(void);

/***************************************************************/
/*设定RF进入Ready模式    耗电量4                              */
/****************************************************************/
extern void SetRFReady(void);

/***************************************************************/
/*设定RF进入TX模式下的TUNE模式                                         */
/****************************************************************/
extern void SetRFTuneForTx(void);

/***************************************************************/
/*设定RF进入Rx模式下的TUNE模式                                         */
/****************************************************************/
extern void SetRFTuneForRx(void);

/***************************************************************/
/*设定RF进入TX模式 (注：数据发送完毕之后自动退出)                     */
//注意这两个函数的区别
/****************************************************************/
extern void SetRFTx(void);        //使用START_TX命令,可以改变工作频道号与发送数据长度  
extern void SetRFStartTx(void);  //使用CHANGE_STATE命令  
 
/***************************************************************/
/*设定RF进入RX模式                                         */
//注意这两个函数的区别
/****************************************************************/
extern void SetRFRx(void);      //使用START_RX命令

extern void SetRFStartRx(void);  //使用CHANGE_STATE命令

/***************************************************************
硬件重设RF Reset RF
****************************************************************/
extern void HWResetRF(void);

/****************************************************************
跳频函数，写入相应的参数，跳频至相应的；频道号
注意：这里跳频只是改变了工作频道号的全局变量，并没有真正的实现
跳频，真正的实现跳频要在SetRFStartTx(),SetRFStartRx()函数中实现，
即：在执行发送命令和接受命令的时候在实现跳频
**********************************************************************/                                         
/*    跳频至选择的频率,参数unsigned char RFFre选择工作中心频率        */
// A区                                                           
// 0x00 -------------------------------------470.100MHz           
// 0x01 -------------------------------------470.300MHz             
// 0x02 -------------------------------------470.500MHz          
// 0x03 -------------------------------------470.700MHz           
// 0x04 -------------------------------------470.900MHz     
// B区 无 
/****************************************************************/
extern void FrequencyHop(unsigned char RFFreNum);

/*****************************************************************
配置RF在工作频段
*****************************************************************/
extern void InitRFChipWorkBand(void);


/****************************************************************/
/*           获取RSSI值并函数返回                               */
/****************************************************************/
extern unsigned char GetRSSI(void);
extern unsigned char GetFastLatchRSSI(void);
extern int GetRSSIPower(void);
/****************************************************************
设定RF进入到TX发射载波
*****************************************************************/
extern void SetTxCarrier(void);

/****************************************************************************/
/*  发送数据包，                                                            */
/*  函数参数:                                                               */
/*   char *txBuffer: 待发送的数据；                                         */
/*   int size：存放txBuffer中总共需要发送的数据长度                         */
/*   待发送的数据长度不超过 255-5 ＝250字节                                */
/*  函数返回值 ：0: 发送成功；-1：发送失败;-2:数据长度太长                 */
/*  发送完毕后自动进入接收态                                               */
/****************************************************************************/
extern int RFSendPacket(unsigned char *txBuffer, int size);

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
extern int RFReceivePacket(unsigned char *rxBuffer);