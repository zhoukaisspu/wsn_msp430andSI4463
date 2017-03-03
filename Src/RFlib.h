/******************************************************************************/
/*            Ӧ��Ŀ��壺SI4463 Bϵ����Ƶ��                                  */
/*            ����������SI4463��Ƶģ�����������Դ�ļ�                        */
/*            ���ߣ��ܿ� ,�Ϻ��ڶ���ҵ��ѧ                                    */
/*            �汾�ţ�V1.0.0                                                  */
/*            ���ڣ�2014.08.20                                                */
/*            �޸��ߣ�                                                        */
/*            �޸����ڣ�                                                      */
/******************************************************************************/
#define uchar unsigned char  
#define uint  unsigned int 
/*
RF�ܽŶ��壺
   SDN:P8.2     �ضϿ���
   NSEL:P8.1    SPIͨѶ����ѡ��...NSEL�ܽ����ͳ�ʼ��ͨѶ�ӿ�
   SCLK:P8.0    SPIʱ���źŹܽţ��������10MHz
   SDI:P7.3     ��������ܽ�
   SDO:P7.2     ��������ܽ�
   NIRQ:P1.6    �жϹܽ�
   GPIO2:P8.4   ���ܴ���???????
*/
//<ע��>����һЩ�ܽ��У���PXSEL�Ĺܽ�ѡ���һ���ܵ�ʱ�򣬾;����ⲿ�жϹ���
//GPIO2�ܽ�P8.4û���ⲿ�жϹ��ܣ�����Ҳ������P8IE
#define RF_SI_GPIO2_PxOUT        P8OUT       //�����ƽ����
#define RF_SI_GPIO2_PxIN         P8IN        //�����ƽ�ж�
#define RF_SI_GPIO2_PxDIR        P8DIR        //�������
#define RF_SI_GPIO2_PxREN        P8REN        //��/��������ʹ�ܿ���
#define RF_SI_GPIO2_PxIES        P8IES        //�жϲ�����������
#define RF_SI_GPIO2_PxIFG        P8IFG        //״̬��־λ
#define RF_SI_GPIO2_PIN          BIT4          //�ܽű��

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

//GPIO2�������    
#define SET_GPIO2_OUT (RF_SI_GPIO2_PxDIR |= RF_SI_GPIO2_PIN)//GPIO2���
#define GPIO2_OUT_H   (RF_SI_GPIO2_PxOUT |= RF_SI_GPIO2_PIN)//GPIO2����ߵ�ƽ
#define GPIO2_OUT_L   (RF_SI_GPIO2_PxOUT &= ~RF_SI_GPIO2_PIN)//GPIO2����͵�ƽ
#define EN_GPIO2_RES  (RF_SI_GPIO2_PxREN |= RF_SI_GPIO2_PIN)//GPIO2����������
#define SET_GPIO2_IN  (RF_SI_GPIO2_PxDIR &= ~RF_SI_GPIO2_PIN)//GPIO2Ϊ����
#define GET_GPIO2_BIT  (RF_SI_GPIO2_PxIN & RF_SI_GPIO2_PIN)//��ȡ��ǰGPIO2��λ 
 
//NIRQ�ж������� 
#define SET_NIRQ_OUT  (RF_SI_NIRQ_PxDIR |= RF_SI_NIRQ_PIN)//NIRQ��� 
#define NIRQ_OUT_H    (RF_SI_NIRQ_PxOUT |= RF_SI_NIRQ_PIN)//NIRQ����ߵ�ƽ 
#define EN_NIRQ_RES   (RF_SI_NIRQ_PxREN |= RF_SI_NIRQ_PIN)//NIRQ����������
#define DIS_NIRQ_RES  (RF_SI_NIRQ_PxREN &= ~RF_SI_NIRQ_PIN)//NIRQ��������/��������
#define SET_NIRQ_IN   (RF_SI_NIRQ_PxDIR &= ~RF_SI_NIRQ_PIN)//NIRQΪ����
#define SET_NIRQ_IES  (RF_SI_NIRQ_PxIES |= RF_SI_NIRQ_PIN)//NIRQ�ߵ����ж�
#define CLR_NIRQ_IFG  (RF_SI_NIRQ_PxIFG &= ~RF_SI_NIRQ_PIN)//����жϱ��
#define EINT_NIRQ     (RF_SI_NIRQ_PxIE |= RF_SI_NIRQ_PIN)//����NIRQ�ж�
#define DINT_NIRQ     (RF_SI_NIRQ_PxIE &= ~RF_SI_NIRQ_PIN)//��ֹNIRQ�ж�
#define GET_NIRQ_BIT  (RF_SI_NIRQ_PxIN & RF_SI_NIRQ_PIN)//��ȡ��ǰNIRQ��λ

//SDO����  ==��Ƶ�������ӦMCU������
#define DIS_SDO_RES   (RF_SI_SDO_PxREN &= ~RF_SI_SDO_PIN)//SDO��Ҫ����/��������
#define EN_SDO_RES    (RF_SI_SDO_PxREN |= RF_SI_SDO_PIN)//SDOҪ��������
#define SDO_OUT_H     (RF_SI_SDO_PxOUT |= RF_SI_SDO_PIN)//SDO����ߵ�ƽ
#define SDO_OUT_L     (RF_SI_SDO_PxOUT &= ~RF_SI_SDO_PIN)//SDO����͵�ƽ
#define SET_SDO_IN    (RF_SI_SDO_PxDIR &= ~RF_SI_SDO_PIN)//����SDOΪ����
#define GET_SDO_BIT   (RF_SI_SDO_PxIN & RF_SI_SDO_PIN)//��ȡ��ǰSDO��λ

//SDI���
#define DIS_SDI_RES   (RF_SI_SDI_PxREN &= ~RF_SI_SDI_PIN)//SDI��Ҫ����/��������
#define SET_SDI_OUT   (RF_SI_SDI_PxDIR |= RF_SI_SDI_PIN)//����SDIΪ���
#define SDI_OUT_H     (RF_SI_SDI_PxOUT |= RF_SI_SDI_PIN)//SDI����ߵ�ƽ
#define SDI_OUT_L     (RF_SI_SDI_PxOUT &= ~RF_SI_SDI_PIN)//SDI����͵�ƽ

//SCLK���
#define DIS_SCLK_RES  (RF_SI_SCLK_PxREN &= ~RF_SI_SCLK_PIN)//SCLK��Ҫ����/��������
#define SET_SCLK_OUT  (RF_SI_SCLK_PxDIR |= RF_SI_SCLK_PIN)//����SCLKΪ��� 
#define SCLK_OUT_H    (RF_SI_SCLK_PxOUT |= RF_SI_SCLK_PIN)//SCLK����ߵ�ƽ
#define SCLK_OUT_L    (RF_SI_SCLK_PxOUT &= ~RF_SI_SCLK_PIN)//SCLK����͵�ƽ

//NSEL���
#define DIS_NSEL_RES  (RF_SI_NSEL_PxREN &= ~RF_SI_NSEL_PIN)//NSEL��Ҫ����/��������
#define SET_NSEL_OUT  (RF_SI_NSEL_PxDIR |= RF_SI_NSEL_PIN)//����NSELΪ��� 
#define NSEL_OUT_H    (RF_SI_NSEL_PxOUT |= RF_SI_NSEL_PIN)//NSEL����ߵ�ƽ
#define NSEL_OUT_L    (RF_SI_NSEL_PxOUT &= ~RF_SI_NSEL_PIN)//NSEL����͵�ƽ


//SDN���
#define DIS_SDN_RES   (RF_SI_SDN_PxREN &= ~RF_SI_SDN_PIN)//SDN��Ҫ����/��������
#define SET_SDN_OUT   (RF_SI_SDN_PxDIR |= RF_SI_SDN_PIN)//����SDNΪ���
#define SDN_OUT_H     (RF_SI_SDN_PxOUT |= RF_SI_SDN_PIN)//SDN����ߵ�ƽ
#define SDN_OUT_L     (RF_SI_SDN_PxOUT &= ~RF_SI_SDN_PIN)//SDN����͵�ƽ

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
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
***********************************/
#define Group_GLOBAL                  0x00          //�����ƶ���
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
//�㷨
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
INT  group:0x01  �жϼĴ���
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
*******************************************/
#define Grout_INT                     0x01
#define ad_INT_CTL_ENABLE             0x00
#define ad_INT_CTL_PH_ENABLE          0x01
#define ad_INT_CTL_MODEM_ENABLE       0x02
#define ad_INT_CTL_CHIP_ENABLE        0x03
//�Ե����Ĵ�����������
#define INT_CTL_ENABLE( x )                 API_SET_PROPERTY_1(Grout_INT,ad_INT_CTL_ENABLE,x)
#define INT_CTL_PH_ENABLE( x )              API_SET_PROPERTY_1(Grout_INT,ad_INT_CTL_PH_ENABLE,x)
#define INT_CTL_MODEM_ENABLE( x )           API_SET_PROPERTY_1(Grout_INT,ad_INT_CTL_MODEM_ENABLE,x)
#define INT_CTL_CHIP_ENABLE( x )            API_SET_PROPERTY_1(Grout_INT,ad_INT_CTL_CHIP_ENABLE,x)
//��ȡ�Ĵ���ֵ
#define GET_INT_CTL_ENABLE(void)                 API_GET_PROPERTY_1(Grout_INT,ad_INT_CTL_ENABLE)
#define GET_INT_CTL_PH_ENABLE(void)              API_GET_PROPERTY_1(Grout_INT,ad_INT_CTL_PH_ENABLE)
#define GET_INT_CTL_MODEM_ENABLE(void)           API_GET_PROPERTY_1(Grout_INT,ad_INT_CTL_MODEM_ENABLE)
#define GET_INT_CTL_CHIP_ENABLE(void)            API_GET_PROPERTY_1(Grout_INT,ad_INT_CTL_CHIP_ENABLE)
/*****************************************
FRR  group:0x02   ���ٷ�Ӧ�Ĵ��� 
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
******************************************/
#define Grout_FRR                     0x02
#define ad_FRR_CTL_A_MODE             0x00
#define ad_FRR_CTL_B_MODE             0x01
#define ad_FRR_CTL_C_MODE             0x02
#define ad_FRR_CTL_D_MODE             0x03
//�Ĵ�������
#define FRR_CTL_A_MODE( x )                 API_SET_PROPERTY_1(Grout_FRR,ad_FRR_CTL_A_MODE,x)
#define FRR_CTL_B_MODE( x )                 API_SET_PROPERTY_1(Grout_FRR,ad_FRR_CTL_B_MODE,x)
#define FRR_CTL_C_MODE( x )                 API_SET_PROPERTY_1(Grout_FRR,ad_FRR_CTL_C_MODE,x)
#define FRR_CTL_D_MODE( x )                 API_SET_PROPERTY_1(Grout_FRR,ad_FRR_CTL_D_MODE,x)
/******************************************
PREAMBLE  group:0x10   ǰ����Ĵ��� 
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
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
//�Ĵ�������
#define PREAMBLE_TX_LENGTH(x)             API_SET_PROPERTY_1(Group_PREAMBLE,ad_PREAMBLE_TX_LENGTH,x)      //0X0A
#define PREAMBLE_CONFIG(x)                API_SET_PROPERTY_1(Group_PREAMBLE,ad_PREAMBLE_CONFIG,x)         //0X31
#define PREAMBLE_CONFIG_STD1( x )           API_SET_PROPERTY_1(Group_PREAMBLE,ad_PREAMBLE_CONFIG_STD_1,x)   //
#define PREAMBLE_CONFIG_STD2( x )           API_SET_PROPERTY_1(Group_PREAMBLE,ad_PREAMBLE_CONFIG_STD_2,x)   //
/*******************************************
SYNC  group:0x11    ͬ�������üĴ���
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
********************************************/
#define Group_SYNC                    0x11
#define ad_SYNC_CONFIG                0x00
#define ad_SYNC_BITS_31_24            0x01
#define ad_SYNC_BITS_23_16            0x02
#define ad_SYNC_BITS_15_8             0x03
#define ad_SYNC_BITS_7_0              0x04
//�Ĵ�������
#define SYNC_CONFIG( x )                   API_SET_PROPERTY_1(Group_SYNC,ad_SYNC_CONFIG,x) //0X01
#define SYNC_BITS_31_24( x )               API_SET_PROPERTY_1(Group_SYNC,ad_SYNC_BITS_31_24,x) //0XB4
#define SYNC_BITS_23_16( x )               API_SET_PROPERTY_1(Group_SYNC,ad_SYNC_BITS_23_16,x) //0X2B
#define SYNC_BITS_15_8( x )                API_SET_PROPERTY_1(Group_SYNC,ad_SYNC_BITS_15_8,x) //
#define SYNC_BITS_7_0( x )                 API_SET_PROPERTY_1(Group_SYNC,ad_SYNC_BITS_7_0,x) //
/********************************************
PKT grout:0x12    ���ݰ�����
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
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

//�Ĵ�������
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
MODEM  group:0x20  ����ģʽ���üĴ���
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
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
//�Ĵ�������
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
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
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
//�Ĵ�������

/******************************************************
PA group:0x22 ���ʷŴ������üĴ���
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
*******************************************************/
#define Group_PA                               0x22
#define ad_PA_MODE                             0x00
#define ad_PA_PWR_LVL                          0x01
#define ad_PA_BIAS_CLKDUTY                     0x02
#define ad_PA_TC                               0x03
#define ad_PA_RAMP_EX                          0x04
#define ad_PA_RAMP_DOWN_DELAY                  0x05
//�Ĵ�������
#define PA_MODE( x )                       API_SET_PROPERTY_1(0X22,0X00,x) //0X08
#define PA_PWR_LVL( x )                    API_SET_PROPERTY_1(0X22,0X01,x) //0X7F
#define PA_BIAS_CLKDUTY( x )               API_SET_PROPERTY_1(0X22,0X02,x) //0X00
#define PA_TC( x )                         API_SET_PROPERTY_1(0X22,0X03,x) //0X3D
/******************************************************
SYNTH group:0x23
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
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
//�Ĵ�������
#define SYNTH_PFDCP_CPFF(x)             API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_PFDCP_CPFF,x) 
#define SYNTH_PFDCP_CPINT(x)            API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_PFDCP_CPINT,x)
#define VCO_KV(x)                       API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_VCO_KV,x)
#define SYNTH_LPFILT3(x)                API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_LPFILT3,x) 
#define SYNTH_LPFILT2(x)                API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_LPFILT2,x)
#define SYNTH_LPFILT1(x)                API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_LPFILT1,x)
#define SYNTH_LPFILT0(x)                API_SET_PROPERTY_1(Group_SYNTH,ad_SYNTH_LPFILT0,x)
/*******************************************************
MATCH group:0x30
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
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
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
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
//�Ĵ�������
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
�����ʽ��
�����ƺ궨��
���ڵ�ַ�궨��
******************************************************/
#define Group_RX_HOP                          0x50
#define ad_RX_HOP_CONTROL                     0x00
#define ad_RX_HOP_TABLE_SIZE                  0x01
#define ad_RX_HOP_TABLE_ENTRY_0               0x02
/*�������ad_RX_HOP_TABLE_ENTRY[0-63]�궨�壬��һһ�о٣�
ʹ�õ�ʱ���������*/
/*<><><><><><><><><><><><><><><><><><><><><><><><><><><><>*/
/*******************************
CTSУ�����ֵ����
********************************/
#define MAX_CTS_RETRY 2500     //����CTS��ȡ���2500�Σ�������Ϊ����

//RF״̬����
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
extern unsigned char g_uchrRFMode;  //RF״̬��־


//����Ϊ��Ƶ�����Ƶ������
//����ĺ궨�������Ƶ��ֲ���Χ
#define RF_FRE_A_AMOUNT   5   //��Ƶ����㶨��A��Ƶ������
#define RF_FRE_B_AMOUNT   0   //��Ƶ����㶨��B��Ƶ������
#define RF_FRE_AMOUNT    (RF_FRE_A_AMOUNT+RF_FRE_B_AMOUNT)  //ϵͳ�ܹ�����Ƶ������


//�趨RSSI�����޶�Ӧֵ
#define RF_RSSI_BEST    -80  //0xF6     //�趨��⵽��"��"�ź�ǿ������ֵ  ��ȡ-80dbm   
#define RF_RSSI_BETTER  -85  //0xEC     //�趨"��"�ź�ǿ������ֵ   ȡ-85dbm
#define RF_RSSI_GOOD    -90  //0xE2     //�趨��⵽��"�Ϻ�"�ź�ǿ������ֵ  ȡ-90dbm
#define RF_RSSI_NORMAL  -95  //0xD8     //�趨һ���ź�ǿ������ֵ    ȡ��95dBm

#define RF_TRANSMIT_RSSI  -85//-90//0xE2 //����ͨ�Ŵ��䷶Χ�ز��ź����޹���ֵ-90dBm
#define RF_LISTEN_RSSI    -75//-95//-100//0xCE //����ͨ��������Χ�ز��ź����޹���ֵ-100dBm
#define RF_RSSI_LOGON    RF_TRANSMIT_RSSI //�趨ע���ź�ǿ������ֵ�����豸���䷶Χ�źŹ���ֵ��ͬ��90dBm


extern unsigned char g_uchRFSelfFreNum; //�����ȫ�ֱ�������������Ĺ���Ƶ����
extern unsigned char g_uchRFCurFreNum;  //��Ƶ�����ȫ�ֱ��������嵱ǰ��Ƶ���ڵ�Ƶ����
extern unsigned char g_chRFFreList[RF_FRE_AMOUNT];   //Ƶ��������


#define MAX_RF_PA_SET   0x80   //4463��Ƶ���ù��ŵ�ֵ
extern const unsigned char g_uchRFPowerVal[MAX_RF_PA_SET];
extern unsigned char g_uchRFPALev; // ������Ƶȫ�ֱ������ʵ�ֵg_uchRFPowerVal�ļ��𣬼�g_uchRFPowerVal�ĵ�ǰ�±�ֵ
extern unsigned char g_uchRxPAtype;  // ��ʾ�����豸�Ĺ������� 0x0A --A���豸 �޹��ţ�0x0B --B���豸 �й��ţ� 0x00 --δ֪


extern unsigned char g_uchRSSI;     //���յ��������źŵ�RSSI

/***************************************************************
RF��״̬��ʼ��
***************************************************************/
extern void RFCtlLineInit(void);

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                          �Ӻ�������
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/******************************************************************************/
/**************************************************************
���������Ӻ���
***************************************************************/
/*
����һ���ֽڵ�����
*/
extern void  API_SendDataByte(uchar Data) ;
/*
���Ͷ���ֽڵ�����
*/
extern void API_SendDataNoResponse(uchar DataLength,uchar *Data) ;
/*
���������ȡ�ظ�����
�βΣ��ظ����ݵĳ��ȡ��洢�ظ����ݵ�����ͷָ��
*/
extern void API_SendDataGetResponse(uchar DataLength,uchar *Data);
/*
��������
�βΣ�����ȣ��洢���������ͷָ��
*/
extern uchar API_SendCommand(uchar CmdLength,uchar *CmdData);
/*
��ȡ�ظ�������
�βΣ��ظ����ݵĳ��ȣ��洢�ظ����ݵ�����ͷָ��
*/
extern uchar API_GetResponse(uchar DataLength,uchar *Data);

/*************************************************************
�ȴ��������
����ֵ��0:�����ɹ�  1�������ʧ��
*************************************************************/
extern uchar API_WaitforCTS(void);

/*************************************************************************/


/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
                SI4463����Ӧ�ú���
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
/*************************************************************
��ȡ�豸��Ϣ
*************************************************************/
extern uchar API_PART_INFO(uchar PARTInfoLength,uchar *PARTInfo);

/************************************************************
��ȡ�豸����������״̬
�βΣ��洢�������ݵ�����ָ��
����ֵ��0x00:��ȡ����ֵ�ɹ� 
        0x01:��ȡ����ֵʧ��
************************************************************/
extern uchar API_REQUEST_DEVICE_STATE(uchar *RFStatus);

/*************************************************************
����1����ȡ״̬��־λ�������״̬��־λ
����2����ȡPacket Handler���жϱ�־λ�����ǲ�һ������ж�
����3����ȡModem���жϱ�־λ�����ǲ�һ������ж�λ
����4����ȡChip���жϱ�־λ�����ǲ�һ������ж�λ��
*************************************************************/
//����1����ȡ״̬��־λ�������״̬��־λ
extern uchar  API_Get_All_IntStatus(uchar IntStatusLength,uchar *IntStatus);

//����2����ȡPacket Handler���жϱ�־λ�����ǲ�һ������ж�
extern uchar  API_Get_PH_IntStatus(uchar PHIntStatusLength,uchar *PHIntStatus);

//������ݰ�������жϱ�־λ
extern void API_ClearPHInt(void);

//����3����ȡModem���жϱ�־λ�����ǲ�һ������ж�λ
extern uchar  API_Get_Modem_IntStatus(uchar ModemIntStatusLength,uchar *ModemIntStatus);

//����4����ȡChip���жϱ�־λ�����ǲ�һ������ж�λ��
extern uchar  API_Get_Chip_IntStatus(uchar ChipIntStatusLength,uchar *ChipIntStatus);
/*****************************************************************
����1�����TxFIFO
����2�����RxFIFO
******************************************************************/
/*
���TxFIFO
����ֵ��0xff��մ���
        0x40:��ճɹ�
*/
extern uchar API_ClearTxFIFO(void);
extern uchar RFResetRxFIFO(void);    //������·�㺯��

/*
���RxFIFO
����ֵ��0xff:���ʧ��
        0x40:��ճɹ�
*/
extern uchar API_ClearRxFIFO(void);
extern uchar RFResetTxFIFO(void);    //������·�㺯��
/*
д��TXDataBuffer
�βΣ�д�����ݳ��ȣ�����ͷָ��
*/
extern uchar API_WriteTxDataBuffer(uchar TxFifoLenght,uchar *TxFifoData);
/*
����RxDataBuffer
�β�:���������ݳ��ȣ��������������ͷָ��
*/
extern uchar API_ReadRxDataBuffer(uchar RxFifoLength,uchar *RxFifoData);
/*
����FIFO�Ŀռ�ʹ�����
�βΣ��洢�������ݵ�����ͷָ��
*/
extern void API_FIFO_INFO(uchar *FIFO_INFO);

/*********************************************************
��ȡ���ٷ�Ӧ�Ĵ�����Ĵ洢ֵ
�βΣ�bStartReg���Ĵ����Ŀ�ʼ��ַ
      bNumberOfReg����ȡ�Ĵ����ĸ���
      RegValues�� �洢�Ĵ����Ķ�ά����ͷָ��
����ֵ��0x01:д���ַ����
        0x00:��ȡ���ݳɹ�
**********************************************************/
uchar API_GetFastResponseRegister(uchar bStartReg,uchar bNumberOfReg,uchar *RegValues);

/*********************************************************
RF��GPIO����
�βΣ�GPIO0��GPIO0���÷�ʽ
      GPIO1��GPIO1���÷�ʽ
      GPIO2��GPIO2���÷�ʽ
*********************************************************/
extern void API_GPIO_CFG(uchar GPIO0,uchar GPIO1,uchar GPIO2,uchar GPIO3);

/**********************************************************
�ı���Ƶģ���״̬
**********************************************************/
extern uchar API_CHANGE_STATE(uchar RFStatus);

/**********************************************************
�ϵ�оƬ��ʼ��
**********************************************************/
extern void API_POWER_UP(void);


/***********************************************************
�Ĵ������ú���
����һ������һ���Ĵ���
����������������һ�����ڵĶ���Ĵ���
************************************************************/
/*�Ĵ��������ã���ͬһ����Ķ���Ĵ���������������
�βΣ�group���Ĵ�����������
      num_pro����Ҫ�������üĴ����ĸ���
     start_pro�����üĴ�����ĳ�ʼ��ַ
     *Data���Ĵ������õ�����
����ֵ:0��һ�������üĴ����ɹ�
       1��һ�������õļĴ������࣬����ʧ��
        
*/
extern uchar API_SET_PROPERTY_X(unsigned char group,unsigned char num_pro,unsigned char start_pro,unsigned char *Data); 

/*
��һ���Ĵ�����������
�βΣ�group:�üĴ�����������
      address���üĴ����ĵ�ַ
      Data���ԸüĴ������õ�����
����ֵ��0���ԼĴ��������óɹ�
        1���ԼĴ���������ʧ��
*/
 extern uchar API_SET_PROPERTY_1(unsigned char group,unsigned char address,unsigned char Data);
 
 /*
 ��ȡһ���Ĵ�����ֵ
 �βΣ��üĴ����������飬�Ĵ����ĵ�ַ
 ����ֵ����ȡ�ļĴ�����ֵ
 */
 extern uchar API_GET_PROPERTY_1(unsigned char group,unsigned char start_pro);
 
/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
ʹ�õĺ�������
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/**************************************************************/
/*�趨RF���뵽Power Downģʽ   �ĵ���1                        */
/**************************************************************/
extern void SetRFOff(void);

/***************************************************************/
/*�趨RF����                                                   */
/***************************************************************/
extern void SetRFOn(void);

/***************************************************************/
/*�趨RF����Standbyģʽ     �ĵ���2                             */
/****************************************************************/
extern void SetRFStandby(void);

/***************************************************************/
/*�趨RF����Sleepģʽ    �ĵ���3                               */
/****************************************************************/
extern void SetRFSleep(void);

/***************************************************************/
/*�趨RF����Readyģʽ    �ĵ���4                              */
/****************************************************************/
extern void SetRFReady(void);

/***************************************************************/
/*�趨RF����TXģʽ�µ�TUNEģʽ                                         */
/****************************************************************/
extern void SetRFTuneForTx(void);

/***************************************************************/
/*�趨RF����Rxģʽ�µ�TUNEģʽ                                         */
/****************************************************************/
extern void SetRFTuneForRx(void);

/***************************************************************/
/*�趨RF����TXģʽ (ע�����ݷ������֮���Զ��˳�)                     */
//ע������������������
/****************************************************************/
extern void SetRFTx(void);        //ʹ��START_TX����,���Ըı乤��Ƶ�����뷢�����ݳ���  
extern void SetRFStartTx(void);  //ʹ��CHANGE_STATE����  
 
/***************************************************************/
/*�趨RF����RXģʽ                                         */
//ע������������������
/****************************************************************/
extern void SetRFRx(void);      //ʹ��START_RX����

extern void SetRFStartRx(void);  //ʹ��CHANGE_STATE����

/***************************************************************
Ӳ������RF Reset RF
****************************************************************/
extern void HWResetRF(void);

/****************************************************************
��Ƶ������д����Ӧ�Ĳ�������Ƶ����Ӧ�ģ�Ƶ����
ע�⣺������Ƶֻ�Ǹı��˹���Ƶ���ŵ�ȫ�ֱ�������û��������ʵ��
��Ƶ��������ʵ����ƵҪ��SetRFStartTx(),SetRFStartRx()������ʵ�֣�
������ִ�з�������ͽ��������ʱ����ʵ����Ƶ
**********************************************************************/                                         
/*    ��Ƶ��ѡ���Ƶ��,����unsigned char RFFreѡ��������Ƶ��        */
// A��                                                           
// 0x00 -------------------------------------470.100MHz           
// 0x01 -------------------------------------470.300MHz             
// 0x02 -------------------------------------470.500MHz          
// 0x03 -------------------------------------470.700MHz           
// 0x04 -------------------------------------470.900MHz     
// B�� �� 
/****************************************************************/
extern void FrequencyHop(unsigned char RFFreNum);

/*****************************************************************
����RF�ڹ���Ƶ��
*****************************************************************/
extern void InitRFChipWorkBand(void);


/****************************************************************/
/*           ��ȡRSSIֵ����������                               */
/****************************************************************/
extern unsigned char GetRSSI(void);
extern unsigned char GetFastLatchRSSI(void);
extern int GetRSSIPower(void);
/****************************************************************
�趨RF���뵽TX�����ز�
*****************************************************************/
extern void SetTxCarrier(void);

/****************************************************************************/
/*  �������ݰ���                                                            */
/*  ��������:                                                               */
/*   char *txBuffer: �����͵����ݣ�                                         */
/*   int size�����txBuffer���ܹ���Ҫ���͵����ݳ���                         */
/*   �����͵����ݳ��Ȳ����� 255-5 ��250�ֽ�                                */
/*  ��������ֵ ��0: ���ͳɹ���-1������ʧ��;-2:���ݳ���̫��                 */
/*  ������Ϻ��Զ��������̬                                               */
/****************************************************************************/
extern int RFSendPacket(unsigned char *txBuffer, int size);

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
extern int RFReceivePacket(unsigned char *rxBuffer);