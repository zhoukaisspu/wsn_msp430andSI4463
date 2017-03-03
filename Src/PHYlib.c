#include "PHYlib.h"
#include "global.h"

//XT2ʱ�ӿ�����־
unsigned char g_uchPHYXt2Flag = DIS_XT2CLK;    //#define DIS_XT2CLK 0 ����XT2ʱ���Ƿ��Ѿ���   

//SMCLKʱ��ѡ��
unsigned char g_uchPHYSMclkSel = PHY_SMCLK_DCO;     //SMCLK����ѡ��XT2��DCO

//SMCLKʱ��Ƶ���趨
unsigned char g_uchPHYSMclkSet = PHY_SMCLK_4MHZ;

//��ʱ������
unsigned int g_uPHYTimer0A0Count = 0;
unsigned int g_uPHYTimer1A0Count = 0;
unsigned int g_uPHYTimerBCount = 0;

unsigned int g_uPHYTimerB_TimeElapse = 0; //ע���豸���ϴ�ע�ᵽ���ڵ���ȥʱ��

//��ʱ���жϱ�־
unsigned char g_chPHYTimerBFlag =0 ; 
unsigned int g_uPHYTimerBAppCount=0;    //Ӧ�ò���� �жϴ���
unsigned int g_uPHYTimerBDLLCount=0 ;   //��·����� �жϴ���

unsigned char g_uchPHYSleepMode = PHY_SLEEP_QUIT;
unsigned int g_uPHYSleppTicks  = 0xffff;  //���ߵ�ʱ�Ӵ���

//IO�жϱ�־
unsigned char g_uchPHYIntP14 = 0;//�˿�P1.4�жϱ�־:0 �����жϣ�1�����ж�

//�Ƚ���TA0����
unsigned int TA0StartTime = 0;
unsigned int TA0EndTime = 0;
unsigned char TA0Overflow = 0; 
unsigned char TA0Flag = 0;
unsigned long  TA0TimeCount = 0;//�ܼ���ֵ

unsigned char g_uchPHYWDFlag = 0; //��ֹ�����ܷɶ��趨�Ŀ��Ź��򿪱�ǣ�0 ���رգ�1����
unsigned int g_uPHYWDTimeHead = 0; //��ֹ�����ܷɶ��趨�Ŀ��Ź�ʱ��ͷ
unsigned int g_uPHYWDTimeTail = 0; //��ֹ�����ܷɶ��趨�Ŀ��Ź�ʱ��β

unsigned char g_uchPHYUART0Open = 0 ;//UART0�򿪱��1:����0����
unsigned char g_uchPHYUART1Open = 0 ;//UART1�򿪱��1:����0����
unsigned char g_uchPHYUART2Open = 0 ;//UART2�򿪱��1:����0����
unsigned char g_uchPHYUART3Open = 0 ;//UART3�򿪱��1:����0����


unsigned char g_uchPHYPAType = 0;	 //�豸���ͣ�Ĭ��ΪB��

unsigned int g_uPHYBattPower = 0;  //��ص���
unsigned int g_uPHYChargePower = 0; //����ѹ
unsigned int g_uPHYBoardPower = 0; //�弫��ѹ

/**********************************************************************/										 
/*��ʼ��ϵͳʱ��DCO= MCLK = SMCLK ,XT1CLK=ACLK ,	�ر� XT2		  */
/**********************************************************************/
void SetDCOClk(void)
{  
	P5SEL &= ~0x0C; 			 // Port Dis-select XT2
	P7SEL |= 0x03;				// Port select XT1
	
	UCSCTL6 &= ~XT1OFF; 	         // XT1 ON  
	UCSCTL6 |= XT2OFF;		 // Set XT2 Off
	
	UCSCTL6 |= (XT1DRIVE_3);//XT1 drive strength
	UCSCTL6 &= ~(XT2DRIVE_3);//XT2 drive strength  
	
	UCSCTL3 = SELREF_0; 	  // Set DCO FLL reference =xt1
	
	UCSCTL0 = 0x00; 			 // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_4;		 // Select DCO range
	UCSCTL2 = PHY_FLLN; 		 // Set DCO Multiplier for 4MHz
	// (N + 1) * FLLRef = Fdco
	// (122 + 1) * 32768 = 4030464 Hz
	
	//Set SMCLK =DCOCLK
	//Set MCLK = DCOCLK
	//Set ACLK = xt1
	UCSCTL4 = SELS_3 + SELM_3 + SELA_0; 
	
	do
	{
		UCSCTL7 &= ~(XT1LFOFFG +XT2OFFG +DCOFFG);
		// Clear XT2,XT1 default flags
		SFRIFG1 &= ~OFIFG;						// Clear fault flags
		for(unsigned int i=0;i<0xFFFF;i++); 	// Delay for Osc to stabilize
	}while (SFRIFG1&OFIFG); 				  // Test oscillator fault flag
	
	UCSCTL6 &= ~(XT1DRIVE_3);	   // Xtal is now stable, reduce drive strength
	
	g_uchPHYXt2Flag = DIS_XT2CLK;
}

/**********************************************************************/										 
/*��ʼ��ϵͳʱ��DCO= MCLK = SMCLK ,REFO=ACLK ,	  �ر�XT1��XT2	*/
/**********************************************************************/
/*
void InitClk(void)
{
   WDTCTL = WDTPW+WDTHOLD; 				  // Stop WDT  

  P5SEL &= ~0x0C;			   // Port Dis-select XT2
  P7SEL &= ~0x03;			   // Port Dis-select XT1
  
	UCSCTL6 |= XT1OFF;		 // XT1 Off  
	UCSCTL6 |= XT2OFF;		 // Set XT2 Off
	UCSCTL6 &= ~(XT1DRIVE_3 + XT2DRIVE_3);//Lowest current consumption
	
	  UCSCTL3 = SELREF_2;		// Set DCO FLL reference = REFO
	  
		//ACLK = REFO = 32kHz, MCLK = SMCLK =DCO= 4MHz	
		UCSCTL0 = 0x00; 			 // Set lowest possible DCOx, MODx
		UCSCTL1 = DCORSEL_4;		 // Select DCO range
		UCSCTL2 = PHY_FLLN; 		 // Set DCO Multiplier for 4MHz
		// (N + 1) * FLLRef = Fdco
		// (122 + 1) * 32768 = 4030464 Hz
		
		  //Set SMCLK =DCOCLK
		  //Set MCLK = DCOCLK
		  //Set ACLK = REFO
		  UCSCTL4 = SELS_3 + SELM_3 + SELA_2; 
		  
			do
			{
			UCSCTL7 &= ~(XT1LFOFFG +XT2OFFG +DCOFFG);
			// Clear XT2,XT1 default flags
			SFRIFG1 &= ~OFIFG;						// Clear fault flags
			for(unsigned int i=0;i<0xFFFF;i++); 	// Delay for Osc to stabilize
			}while (SFRIFG1&OFIFG); 				  // Test oscillator fault flag
			
			  g_uchPHYXt2Flag = DIS_XT2CLK;
			  }
*/

/**********************************************************************/										 
/*		  ����ϵͳʱ��DCO =MCLK=4MHz,XT2 =SMCLK,xt2=ACLK			 */
/**********************************************************************/
void SetXT2ClkOn(void)
{  
	//���XT2�Ѿ����������˳�
	if(g_uchPHYXt2Flag == EN_XT2CLK )
		return;
	
	P5SEL |= 0x0C;				// Port select XT2
	P7SEL |= 0x03;				// Port select XT1
	
	UCSCTL6 &= ~XT1OFF; 	  // XT1 ON  
	UCSCTL6 &= ~XT2OFF; 	  // XT2 On  XT2 enabled in active mode (AM) through LPM4.
	
	UCSCTL6 |= (XT1DRIVE_3);//XT1 drive strength
	UCSCTL6 &= ~(XT2DRIVE_3);//XT2 drive strength  
	
	UCSCTL3 = SELREF_0; 	  // Set DCO FLL reference =xt1
	
	UCSCTL0 = 0x00; 			 // Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_4;		 // Select DCO range
	UCSCTL2 = PHY_FLLN; 		 // Set DCO Multiplier for 4MHz
	// (N + 1) * FLLRef = Fdco
	// (122 + 1) * 32768 = 4030464 Hz
        //(127+1)*32768=4194304HZ   //ZK ������N���ó�Ϊ��127
	
	//Set SMCLK =XT2CLK when available, otherwise DCOCLKDIV
	//Set MCLK = DCOCLK
	//Set ACLK = XT1
	UCSCTL4 = SELS_5 + SELM_3 + SELA_0;
	
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG+ DCOFFG );
		// Clear XT2,XT1 default flags
		SFRIFG1 &= ~OFIFG;						// Clear fault flags
		for(unsigned int i=0;i<0xFFFF;i++); 	// Delay for Osc to stabilize
	}while (SFRIFG1&OFIFG); 				  // Test oscillator fault flag
	
	g_uchPHYXt2Flag = EN_XT2CLK;
}

/**********************************************************************/										 
/*		 �ر�XT2	  DCO= MCLK, DCOCLKDIV = SMCLK,XT1 = ACLK		  */
/**********************************************************************/
void SetXT2ClkOff(void)
{
	SFRIE1 &= ~OFIE;    // Disable osc fault interrupt
  
        //Set SMCLK =DCOCLK
	//Set MCLK = DCOCLK
	//Set ACLK = xt1
	UCSCTL4 = SELS_3 + SELM_3 + SELA_0;
	
	P5SEL &= ~0x0C; 			 // Port Dis-select XT2
	
	UCSCTL6 |= XT2OFF;			 // XT2 Off
	
	g_uchPHYXt2Flag = DIS_XT2CLK;
}

/**********************************************************************/										 
/*		 ���ݺ궨��ѡ��ͬ��ʱ��ϵͳ								 */
/* PHY_SMCLK_DCO:DCO= MCLK = SMCLK ,XT1CLK=ACLK ,	 �ر� XT2		 */
/* PHY_SMCLK_XT2:DCO =MCLK=4MHz,XT2 =SMCLK,xt2=ACLK 				 */
/* Ĭ�϶���PHY_SMCLK_DCO											 */
/**********************************************************************/
//��ϵͳ����ʱ�ӵ��趨��SMCLKͨ��ѡ���ж�SMCLKʱ��Դ
void InitClk(void)
{ 
	WDTCTL = WDTPW+WDTHOLD; 				  // Stop WDT 
	
	if(g_uchPHYSMclkSel == PHY_SMCLK_DCO)
		SetDCOClk();
	else if(g_uchPHYSMclkSel==PHY_SMCLK_XT2)
		SetXT2ClkOn();
	else
		SetDCOClk();
        
        SFRIE1 = OFIE;    // Enable osc fault interrupt
 
}

/**********************************************************************/										 
/*	���ʱ�ӣ�P11.0 ACLK��P11.1 MCLK��P11.2 SMCLK		     */
/**********************************************************************/
void OutPutClk(void)
{
  P11DIR = BIT2 + BIT1 + BIT0; // P11.2,1,0 to output directi on
  P11SEL = BIT2 + BIT1 + BIT0; // P11.2 to output SMCLK
                               // P11.1 to output MCLK 
                               // P11.0 to output ACLK
}

/**********************************************************************/										 
/*		����ϵͳ��ʱ��ϵͳ�����ʱ1/1024��,ԼΪ1ms	      */
/**********************************************************************/
void PHYMCLKSoftDelay1ms(void)
{
	//1��ʱ������(1/PHY_MCLK)��
	unsigned int i = (PHY_MCLK_1MS>>2);
        
	while(--i);//4*i ��ʱ������ = 1/1024��
}


/**********************************************************************/										 
/*		����ϵͳ��ʱ��ϵͳ�����ʱ1/2048��,ԼΪ0.5ms	      */
/**********************************************************************/
void PHYMCLKSoftDelayHalfms(void)
{
	//1��ʱ������(1/PHY_MCLK)��
	unsigned int i = (PHY_MCLK_1MS>>3);
	
	while(--i);//4*i ��ʱ������ = 1/2048��
}

/**********************************************************************/										 
/*		����ϵͳ��ʱ��ϵͳ�����ʱ1/4096��,ԼΪ244us	      */
/**********************************************************************/
void PHYMCLKSoftDelay250us(void)
{
	//1��ʱ������(1/PHY_MCLK)��
	unsigned int i = (PHY_MCLK_1MS>>4);
	
	while(--i);//4*i ��ʱ������ = 1/4096��
}

/****************************************************************************/
/*	�ж��豸��A�ͣ��޹��ţ�����B�ͣ��й��ţ�������B�ͣ�����0�����򷵻أ�1	*/
/****************************************************************************/
int DevHasPAOrNot(void)
{
	return -1;
}

/****************************************************************************/
/*	USCIA3������ʽ��232/TTL��ѡ�� P4.6	�ߵ�ƽ232ģʽ���͵�ƽTTLģʽ		*/
/*��������ֵ��0��232ģʽ��1�� TTLģʽ										*/
/****************************************************************************/
int USCIA3Rs232Sel(void)
{
	PHY_232A3SEL_PxSEL &= ~PHY_232A3SEL_PxIN;               //����һ���IO�ܽŹ���
	PHY_232A3SEL_PxDIR &= ~PHY_232A3SEL_PxIN;               //����Ϊ���뷽��
	
	if( PHY_232A3SEL_PIN & PHY_232A3SEL_PxIN)
		return 0;
	else
		return 1;
}


/****************************************************************************/
/*	Set RS232A0 control    ���ö���RS232A0ͨѶоƬ����							*/
/*	�õ���MCU�ܽ�:		P4.7	
	//=======Դ������ΪRS232A0�ܽŵĿ�������ΪP4.7.����ͨ�ð��Ͽ�������
       �õ��Ĺܽ���P5.4
       �ڶ˻��иô�����������ͷͼ��ɼ�
       �ڻ�վ�иô�����������Կͻ��˽���ͨ��
*/
/****************************************************************************/
//�򿪽���RS232A0   =====����FORCEOFF����ߵ�ƽ���򿪴���ͨ��оƬ
void EnableRs232A0(void)
{
	PHY_232A0_PxSEL &= ~PHY_232A0_PxIN; 		//ʹ��I/O����
	PHY_232A0_PxDIR |= PHY_232A0_PxIN;		   //���		
	PHY_232A0_PxOUT |= PHY_232A0_PxIN;		   //����оƬ
}
//�ر�RS232A0�ӿ�   ,====����FORCEOFF����͵�ƽ���رմ���ͨ��оƬ
void DisableRs232A0(void)
{
	PHY_232A0_PxSEL &=~PHY_232A0_PxIN;			//ʹ��I/O����
	PHY_232A0_PxDIR |= PHY_232A0_PxIN;			//���		
	PHY_232A0_PxOUT &= ~PHY_232A0_PxIN; 		//����оƬ
}

/****************************************************************************/
/*	Set RS232A3 control    ���ö���RS232A3ͨѶоƬ����							*/
/*	�õ���MCU�ܽ�:		P11.1	
       === PHY_232A3_PxIN �����ܽű��
       ===�ô��ڿ���ѡ�񴮿ڵĹ���ģʽTTL/RS232
       ===USCI A3
      *******************OKOKOKOKOKOKOKOKOK**********************
*/
/****************************************************************************/
//�򿪽���RS232A3
void EnableRs232A3(void)
{
	if(USCIA3Rs232Sel() == 1 )//ѡ��TTLģʽ
		DisableRs232A3();
	else
	{  
		PHY_232A3_PxSEL &=~PHY_232A3_PxIN;  //==�ܽ�ѡ��һ���IO����		   //ʹ��I/O����
		PHY_232A3_PxDIR |= PHY_232A3_PxIN;		   //���		
		PHY_232A3_PxOUT |= PHY_232A3_PxIN;		   //����оƬ
	}
}
//�ر�RS232A3�ӿ�
void DisableRs232A3(void)
{
	PHY_232A3_PxSEL &=~PHY_232A3_PxIN;			//ʹ��I/O����
	PHY_232A3_PxDIR |= PHY_232A3_PxIN;			//���		
	PHY_232A3_PxOUT &= ~PHY_232A3_PxIN; 		//����оƬ
}

/****************************************************************************/
/*	Set Green light   control  ���� Green LED								*/
/*	�õ���MCU�ܽ� :   P2.1												   */
/*	��������	  :   �����̵ƣ�Off�رգ�On���� 							*/
/****************************************************************************/
//�����̵�
void LedGreenOn(void)
{
	PHY_LEDG_PxDIR |= PHY_LEDG_PxIN;   
	PHY_LEDG_PxOUT |= PHY_LEDG_PxIN;	   
}
//�ر��̵�
void LedGreenOff(void)
{
	PHY_LEDG_PxDIR |= PHY_LEDG_PxIN;   
	PHY_LEDG_PxOUT &= ~PHY_LEDG_PxIN;	 
	
}
/****************************************************************************/
/*	Set Red light  control	����Red LED 									*/
/*	�õ���MCU�ܽ� :   P2.2													*/
/*	��������	  :   ���ƺ�ƣ�Off�رգ�On���� 							*/
/****************************************************************************/
//�������
void LedRedOn(void)
{
	PHY_LEDR_PxDIR |= PHY_LEDR_PxIN;   
	PHY_LEDR_PxOUT |= PHY_LEDR_PxIN;	  
}
//�رպ��
void LedRedOff(void)
{
	PHY_LEDR_PxDIR |= PHY_LEDR_PxIN;   
	PHY_LEDR_PxOUT &= ~PHY_LEDR_PxIN;	
	
}

/****************************************************************************/
/*	Set flash light  control	����flash light 			*/
/*	�õ���MCU�ܽ� :   P4.3						*/
/*	��������	  :   ���Ƴ����ϵ�AAT4280�����ؿ�������� 		*/
/****************************************************************************/
//���������
void FlashLightOn(void)
{
	PHY_FLASH_LED_PxDIR   |= PHY_FLASH_LED_PxIN;   
	PHY_FLASH_LED_PxOUT   |= PHY_FLASH_LED_PxIN;	  
}
//�ر������
void FlashLightOff(void)
{
	PHY_FLASH_LED_PxDIR   |= PHY_FLASH_LED_PxIN;   
	PHY_FLASH_LED_PxOUT   &= ~PHY_FLASH_LED_PxIN;	
}

/****************************************************************************/
/*	���������� ,�õ�P2.3		��30ms										   */
/****************************************************************************/
void Sound()
{
	PHY_BEEP_PxDIR |= PHY_BEEP_PxIN;   
	PHY_BEEP_PxOUT |= PHY_BEEP_PxIN;	 /*����*/	  
	for(int j=0;j<30;j++)		
		PHYMCLKSoftDelay1ms();
	PHY_BEEP_PxOUT &= ~PHY_BEEP_PxIN;	 /*�ر�*/
	PHY_BEEP_PxDIR &= ~PHY_BEEP_PxIN; 
}
/****************************************************************************/
/*  ͼ�����ķ������������������첻ͬ��ʱ�� ,�õ�P2.3    ======ZK  ��1s                                               */
/****************************************************************************/
void PhotoerrorSound()
{
  	PHY_BEEP_PxDIR |= PHY_BEEP_PxIN;   
	PHY_BEEP_PxOUT |= PHY_BEEP_PxIN;	 /*����*/	  
	for(int j=0;j<1000;j++)		
		PHYMCLKSoftDelay1ms();
	PHY_BEEP_PxOUT &= ~PHY_BEEP_PxIN;	 /*�ر�*/
	PHY_BEEP_PxDIR &= ~PHY_BEEP_PxIN; 
}
/****************************************************************************/
/*	Set power 2 control  ����豸USCIA1��Դ���ؿ��� 	 3221				*/
/*	�õ���MCU�ܽ� :   P10.1 												*/
/****************************************************************************/
void DevPower2On(void)
{
	PHY_DEV_POWER2_PxSEL &= ~PHY_DEV_POWER2_PxIN;
	PHY_DEV_POWER2_PxDIR |= PHY_DEV_POWER2_PxIN;   
	PHY_DEV_POWER2_PxOUT |= PHY_DEV_POWER2_PxIN;	 /*����*/	
}
void DevPower2Off(void)
{
	PHY_DEV_POWER2_PxSEL &= ~PHY_DEV_POWER2_PxIN;
	PHY_DEV_POWER2_PxDIR |= PHY_DEV_POWER2_PxIN;   
	PHY_DEV_POWER2_PxOUT &= ~PHY_DEV_POWER2_PxIN;	 /*�ر�*/  
}

/****************************************************************************/
/*	Set power 3 control 	USCIB0/AD����豸�ĵ�Դ����    3221 			*/
/*	�õ���MCU�ܽ� :   P10.2 												*/
/****************************************************************************/
void DevPower3On(void)
{
	PHY_DEV_POWER3_PxSEL &= ~PHY_DEV_POWER3_PxIN;
	PHY_DEV_POWER3_PxDIR |= PHY_DEV_POWER3_PxIN;   
	PHY_DEV_POWER3_PxOUT |= PHY_DEV_POWER3_PxIN;	 /*����*/	
}
void DevPower3Off(void)
{
	PHY_DEV_POWER3_PxSEL &= ~PHY_DEV_POWER3_PxIN;
	PHY_DEV_POWER3_PxDIR |= PHY_DEV_POWER3_PxIN;   
	PHY_DEV_POWER3_PxOUT &= ~PHY_DEV_POWER3_PxIN;	 /*�ر�*/  
}

/****************************************************************************/
/*	Set power USR control	  �û�����豸�ĵ�Դ����	4280				*/
/*	�õ���MCU�ܽ� :   P10.3
        ����ͷ���Ƶ�Դ
*/
/****************************************************************************/
void DevPowerUsrOn(void)
{
	PHY_DEV_POWERUSR_PxSEL &= ~PHY_DEV_POWERUSR_PxIN;
	PHY_DEV_POWERUSR_PxDIR |= PHY_DEV_POWERUSR_PxIN;   
	PHY_DEV_POWERUSR_PxOUT |= PHY_DEV_POWERUSR_PxIN;	 /*����*/	
}
void DevPowerUsrOff(void)
{
	PHY_DEV_POWERUSR_PxSEL &= ~PHY_DEV_POWERUSR_PxIN;       //==����Ϊһ��IO����
	PHY_DEV_POWERUSR_PxDIR |= PHY_DEV_POWERUSR_PxIN;        //����Ϊ�������
	PHY_DEV_POWERUSR_PxOUT &= ~PHY_DEV_POWERUSR_PxIN;	 /*�ر�*/   //����͵�ƽ������ͷ��Դ�ر� 
}

/****************************************************************************/
 /*              �͹���ģʽ�£�û���õ��Ĺܽŵĳ�ʼ��                      */
/*             ������ϵͳ��û���õ��Ĺܽ�ȫ������Ϊ����״̬                */
/*                ע���ڳ����ڿ�ʼִ������£�Ҫ�����еĹܽŽ��г�ʼ��״̬*/
/*             ����ϵͳ�Ĺ���                                               */
/****************************************************************************/
  void LowPowerPinInit(void)
 {
     P1REN=0xFF; P2REN=0xFF;P3REN=0xFF; P4REN=0xFF;
     P5REN=0xFF;P6REN=0xFF; P7REN=0xFF; P8REN=0xFF;
     P9REN=0xFF;P10REN=0xFF;P11REN=0xFF;PJREN=0xFF;
     
     P1DIR = 0x00;P2DIR = 0x00;P3DIR = 0x00; P4DIR = 0x00;
     P5DIR = 0x00;P6DIR = 0x00;P7DIR = 0x00;P8DIR = 0x00;
     P9DIR = 0x00;P10DIR = 0x00;P11DIR = 0x00;PJDIR = 0x00;
  
     P1OUT = 0xFF;P2OUT = 0xFF;P3OUT = 0xFF;P4OUT = 0xFF;
     P5OUT = 0xFF;P6OUT = 0xFF;P7OUT = 0xFF;P8OUT = 0xFF;
     P9OUT = 0xFF;P10OUT = 0xFF;P11OUT = 0xFF;PJOUT = 0xFF;
 }

/****************************************************************************/
/*				 ��Դ�������⣻����ԴĬ�ϴ򿪡�							*/
/*	  ��Ƭ����Դ����ӿڳ�ʼ����Ĭ�Ϲر������õ�����Դ��					*/
/****************************************************************************/
void InitPower(void)
{  
/*  
     if(g_uAppDataInt >0) //��ע������
    {  
        if(  (g_uAppDataType != 2)  &&  //��ͼ�񴫸������ض�
             (g_uAppDataType != 0x0d)&& //����
             (g_uAppDataType != 0x0e)&& //����
             (g_uAppDataType != 0x0f)&& //�۳�
             (g_uAppDataType != 0x10)&&//����
             (g_uAppDataType != 0x11)&& // ��ʪ�Ⱥ͹��ն����
             (g_uAppDataType != 0x12)&& //��ʪ�Ⱥͺ������
             (g_uAppDataType != 0x14)&& //��ʪ�ȺͶ�����̼���
             (g_uAppDataType != 0x05))//��ʪ������
          
          {
                  DevPower2Off();
                  DevPowerUsrOff();
          }
    }
    else
    {
      if(g_uAppDataType != 2)  //��ͼ�񴫸������ض�
      {
          DevPower2Off();
          DevPowerUsrOff();
      }
    }
  */
    DevPowerUsrOff();
    FlashLightOff();                    //====ZK
    DevPower3Off();                     //��Ϊ��·���Ϻ�����IC3��AAT3221������ִ�йر�
                                        //=====ZK  ����Ƶ�Դ�رգ�IC3��AAT3221���ر�
    
    //if(g_uchAppchRoleSet != CenterBase)
    //  DevPower3Off();
}

/****************************************************************************/
/*	Set UARTa0 control			����A0����������							*/
/*	�õ���MCU�ܽ�	   :  P3.4,P3.5 UARTģ�鹦��							*/
/*	��������		   : char bps											*/
/*	��ѡ�Ĳ���������   : Baud_2400��Baud_4800��Baud_9600��Baud_19200��
Baud_38400,Baud_57600��Baud_115200											*/
/*	Ĭ��ѡ��Baud_9600�������ʸ���Baud_9600ʱ��Ҫ��XT2CLK					*/
/****************************************************************************/
void OpenUartA0(char bps)
{
	UCA0CTL1 |=UCSWRST; //RESET
	
	P3SEL |= BIT4 + BIT5;  // P3.4= USARTA0 TXD P3.5 RXD  ����P3.4,P3.5��UART����
	
	UCA0CTL0=0; 	// 8N1 
	
	//Ĭ���趨Baud_9600
	UCA0CTL1 = UCSSEL0;// CLK = ACLK = 32768Hz
	UCA0BR0 = 0x03;
	UCA0BR1 = 0;
	UCA0MCTL = UCBRS_3+UCBRF_0;
	
	switch(bps) 	  
	{
	case Baud_2400: 
		UCA0BR0 = 0x0d;
		UCA0BR1 = 0;
		UCA0MCTL = UCBRS_6+UCBRF_0;
		break;		
	case Baud_4800: 	
		UCA0BR0 = 0x06;
		UCA0BR1 = 0;
		UCA0MCTL = UCBRS_7+UCBRF_0;
		break;		
	case Baud_9600:
		UCA0BR0 = 0x03;
		UCA0BR1 = 0;
		UCA0MCTL = UCBRS_3+UCBRF_0;
		break;		
	case Baud_19200:
		{	  
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;
			
			UCA0CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA0BR0 = 0xA0;
				UCA0BR1 = 0x01;
				UCA0MCTL = UCBRS_6+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA0BR0 = 0xD0;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_3+UCBRF_0;
			}	
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA0BR0 = 0xDA;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_4+UCBRF_0;
			}	
                  
			break;	
		}
            /*
	case Baud_38400:
		{
			if(g_uchPHYXt2Flag == DIS_XT2CLK)
				break;
			
			UCA0CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA0BR0 = 0xD0;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_3+UCBRF_0;
			} 
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA0BR0 = 0x68;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_1+UCBRF_0;
			}
			break;	
		}	
            */
	case Baud_57600:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;	
			
			UCA0CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK 		   
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA0BR0 = 0x8A;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_7+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA0BR0 = 0x45;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_4+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA0BR0 = 0x48;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_7+UCBRF_0;
			}	
			break;	
		}				
	case Baud_115200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;
			
			UCA0CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA0BR0 = 0x45;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_4+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{				 //clk = Smclk = 4MHz
				UCA0BR0 = 0x22;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_6+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA0BR0 = 0x24;
				UCA0BR1 = 0x00;
				UCA0MCTL = UCBRS_3+UCBRF_0;
			}	
                  
			break;	
		}		
	default:
		break;
	}
	
	UCA0CTL1 &=~UCSWRST;	//Initialize USCI state machine
	UCA0IE |= UCRXIE;	 //������0�����ж�
	
	g_uchPHYUART0Open = 1;
} 

/****************************************************************************/
/*	�ر�UART0�� 															*/
/****************************************************************************/
void CloseUartA0(void)
{
	UCA0IE &= ~UCRXIE;	  //��ֹUART0�����ж�
	UCA0CTL1 |=UCSWRST; //RESET
	
	g_uchPHYUART0Open = 0;
}


/****************************************************************************/
/*	Set UARTa1 control			����A1����������							*/
/*	�õ���MCU�ܽ�	   :       P5.6,P5.7 UARTģ�鹦��							*/
/*	��������		   : char bps											*/
/*	��ѡ�Ĳ���������   : Baud_2400��Baud_4800��Baud_9600��Baud_19200��
Baud_38400,Baud_57600��Baud_115200				  */
/****************************************************************************/
void OpenUartA1(char bps)
{
	UCA1CTL1 |=UCSWRST; //RESET
	
	P5SEL |= BIT6 + BIT7;  // P5.6= USARTA1 TXD P5.7 RXD  ����UART����
	
	UCA1CTL0=0; 	// 8N1 
	
	//Ĭ��9600
	UCA1CTL1 = UCSSEL0;// CLK = ACLK = 32768Hz
	UCA1BR0 = 0x03;
	UCA1BR1 = 0;
	UCA1MCTL = UCBRS_3+UCBRF_0;
	
	switch(bps) 	  
	{
	case Baud_2400: 
		UCA1BR0 = 0x0d;
		UCA1BR1 = 0;
		UCA1MCTL = UCBRS_6+UCBRF_0;
		break;
		
	case Baud_4800: 	
		UCA1BR0 = 0x06;
		UCA1BR1 = 0;
		UCA1MCTL = UCBRS_7+UCBRF_0;
		break;
		
	case Baud_9600:
		UCA1BR0 = 0x03;
		UCA1BR1 = 0;
		UCA1MCTL = UCBRS_3+UCBRF_0;
		break;
		
	case Baud_19200://clk = Smclk = 8MHz
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;	
		  
			UCA1CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA1BR0 = 0xA0;
				UCA1BR1 = 0x01;
				UCA1MCTL = UCBRS_6+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA1BR0 = 0xd0;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_3+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA1BR0 = 0xDA;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_4+UCBRF_0;
			}	
                  
			break;	
		}
            /*
	case Baud_38400:
		{
			if(g_uchPHYXt2Flag == DIS_XT2CLK)
				break;			  
			
			UCA1CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA1BR0 = 0xD0;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_3+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA1BR0 = 0x68;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_1+UCBRF_0;
			}
			break;	
		}	
            */
	case Baud_57600:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;			  
			
			UCA1CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA1BR0 = 0x8A;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_7+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA1BR0 = 0x45;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_4+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA1BR0 = 0x48;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_7+UCBRF_0;
			}	
                  
			break;			
		}
	case Baud_115200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;			  
			
			UCA1CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA1BR0 = 0x45;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_4+UCBRF_0; 
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA1BR0 = 0x22;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_6+UCBRF_0; 
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA1BR0 = 0x24;
				UCA1BR1 = 0x00;
				UCA1MCTL = UCBRS_3+UCBRF_0;
			}	
			break;
		}		
	default:
		break; 
	}
	
	UCA1CTL1 &=~UCSWRST;	//Initialize USCI state machine
	UCA1IE |= UCRXIE;	 //������1�����ж�
	
	g_uchPHYUART1Open = 1;
} 

/****************************************************************************/
/*	�ر�UART1�� 															*/
/****************************************************************************/
void CloseUartA1(void)
{
	UCA1IE &= ~UCRXIE;	  //��ֹUART1�����ж�
	UCA1CTL1 |=UCSWRST;       //RESET	
	g_uchPHYUART1Open = 0;
}

/****************************************************************************/
/*	Set SPI��A1 control 		 A1����SPI����ͨѶ							*/
/*	3�ߣ�8���أ�ʱ�Ӽ���Ϊ�ߵ�ƽ��Ч���ȷ���MSB,ͨѶʱ��ΪMCLK/2			*/
/*	�õ���MCU�ܽ�	   :  P5.6 P5.7��SPIģ�鹦��							 */
/****************************************************************************/
void OpenMasterSpiA1(void)
{  
	UCA1CTL1 |=UCSWRST; //RESET
	
	P5SEL |= BIT6 + BIT7;  // ����P5.6,P5.7��SPI����
	
	UCA1CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;	  // 3-pin, 8-bit SPI master
	// Clock polarity high, MSB
	UCA1CTL1 |= UCSSEL_2;					  // MCLK
	UCA1BR0 = 0x02; 						  // /2
	UCA1BR1 = 0;							  //
	UCA1MCTL = 0;							  // No modulation
	UCA1CTL1 &= ~UCSWRST;					  // **Initialize USCI state machine**
	UCA1IE |= UCRXIE;						  // Enable USCI_A2 RX interrupt
}

/****************************************************************************/
/*	�ر�SPI A1��															 */
/****************************************************************************/
void CloseSpiA1(void)
{
	UCA1IE &= ~UCRXIE;	  //��ֹSPI�����ж�
	UCA1CTL1 |=UCSWRST; //RESET
}


/****************************************************************************/
/*	Set UARTA2 control			����A2����������							*/
/*	�õ���MCU�ܽ�	   :  P9.4,P9.5 UARTģ�鹦��							*/
/*	��������		   : char bps											*/
/*	��ѡ�Ĳ���������   : Baud_2400��Baud_4800��Baud_9600��Baud_19200��
Baud_57600��Baud_115200 			   */

//===��470ͨ�ð��в�û���õ��ùܽŵĹ��ܣ�P9.4,P9.5��û��������
/****************************************************************************/
void OpenUartA2(char bps)
{
	UCA2CTL1 |=UCSWRST; //RESET
	
	P9SEL |= BIT4 + BIT5;  // P9.4= USARTA2 TXD P9.5 RXD  ����P9.4,P9.5��UART����
	
	UCA2CTL0=0; 	// 8N1 
	
	//Ĭ��9600
	UCA2CTL1 = UCSSEL0;// CLK = ACLK = 32768Hz
	UCA2BR0 = 0x03;
	UCA2BR1 = 0x0;
	UCA2MCTL = UCBRS_3+UCBRF_0;
	
	switch(bps) 	  
	{
	case Baud_2400: 
		UCA2BR0 = 0x0d;
		UCA2BR1 = 0;
		UCA2MCTL = UCBRS_6+UCBRF_0;
		break;
		
	case Baud_4800: 	
		UCA2BR0 = 0x06;
		UCA2BR1 = 0;
		UCA2MCTL = UCBRS_7+UCBRF_0;
		break;
		
	case Baud_9600:
		UCA2BR0 = 0x03;
		UCA2BR1 = 0x0;
		UCA2MCTL = UCBRS_3+UCBRF_0;
		break;
		
	case Baud_19200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;
              
			UCA2CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA2BR0 = 0xA0;
				UCA2BR1 = 0x01;
				UCA2MCTL = UCBRS_6+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA2BR0 = 0xd0;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_3+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA2BR0 = 0xDA;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_4+UCBRF_0;
			}	
			break;
		}
            /*
	case Baud_38400:
		{
			if(g_uchPHYXt2Flag == DIS_XT2CLK)
				break;
			UCA2CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA2BR0 = 0xD0;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_3+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA2BR0 = 0x68;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_1+UCBRF_0;
			}				
			break;
		}
            */
	case Baud_57600:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;
			UCA2CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA2BR0 = 0x8a;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_7+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA2BR0 = 0x45;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_4+UCBRF_0;
			}	

                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA2BR0 = 0x48;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_7+UCBRF_0;
			}				
			break;
		}		
	case Baud_115200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;
			UCA2CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA2BR0 = 0x45;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_4+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA2BR0 = 0x22;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_6+UCBRF_0;
			}	
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA2BR0 = 0x24;
				UCA2BR1 = 0x00;
				UCA2MCTL = UCBRS_3+UCBRF_0;
			}	
                  
			break;
		}	
		
	default:
		break; 
	}
	
	UCA2CTL1 &=~UCSWRST;	//Initialize USCI state machine
	UCA2IE |= UCRXIE;	 //������2�����ж�
	
	g_uchPHYUART2Open = 1;
} 

/****************************************************************************/
/*	�ر�UART2�� 															*/
/****************************************************************************/
void CloseUartA2(void)
{
	UCA2IE &= ~UCRXIE;	  //��ֹUART2�����ж�
	UCA2CTL1 |=UCSWRST; //RESET
	
	g_uchPHYUART2Open = 0;
}

/****************************************************************************/
/*	Set SPI��A2 control 		 A2����SPI����ͨѶ							*/
/*	3�ߣ�8���أ�ʱ�Ӽ���Ϊ�ߵ�ƽ��Ч���ȷ���MSB,ͨѶʱ��ΪMCLK/2			*/
/*	�õ���MCU�ܽ�	   :  P9.4,P9.5 SPIģ�鹦�� 							*/
/****************************************************************************/
void OpenMasterSpiA2(void)
{  
	UCA2CTL1 |=UCSWRST; //RESET
	
	P9SEL |= BIT4 + BIT5;  // ����P9.4,P9.5��SPI����
	
	UCA2CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;	  // 3-pin, 8-bit SPI master
	// Clock polarity high, MSB
	UCA2CTL1 |= UCSSEL_2;					  // MCLK
	UCA2BR0 = 0x02; 						  // /2
	UCA2BR1 = 0;							  //
	UCA2MCTL = 0;							  // No modulation
	UCA2CTL1 &= ~UCSWRST;					  // **Initialize USCI state machine**
	UCA2IE |= UCRXIE;						  // Enable USCI_A2 RX interrupt
}

/****************************************************************************/
/*	�ر�SPI A2��															 */
/****************************************************************************/
void CloseSpiA2(void)
{
	UCA2IE &= ~UCRXIE;	  //��ֹSPI�����ж�
	UCA2CTL1 |=UCSWRST; //RESET
}

/****************************************************************************/
/*	Set UARTA3 control			����A3����������							*/
/*	�õ���MCU�ܽ�	   :  P10.4,P10.5 UARTģ�鹦��							  */
/*	��������		   : char bps											*/
/*	��ѡ�Ĳ���������   : Baud_2400��Baud_4800��Baud_9600��Baud_19200��
Baud_57600��Baud_115200 			   */
/****************************************************************************/
void OpenUartA3(char bps)
{  
	UCA3CTL1 |=UCSWRST; //RESET
	
	P10SEL |= BIT4 + BIT5;	// P10.4= USARTA2 TXD P10.5 RXD  ����P10.4,P10.5��UART����
	
	UCA3CTL0=0; 	// 8N1
	
	//Ĭ��9600
	UCA3CTL1 = UCSSEL0;// CLK = ACLK = 32768Hz
	UCA3BR0 = 0x03;
	UCA3BR1 = 0;
	UCA3MCTL = UCBRS_3+UCBRF_0;
	
	switch(bps) 	  
	{
	case Baud_2400: 
		UCA3BR0 = 0x0d;
		UCA3BR1 = 0;
		UCA3MCTL = UCBRS_6+UCBRF_0;
		break;
		
	case Baud_4800: 	
		UCA3BR0 = 0x06;
		UCA3BR1 = 0;
		UCA3MCTL = UCBRS_7+UCBRF_0;
		break;
		
	case Baud_9600:
		UCA3BR0 = 0x03;
		UCA3BR1 = 0;
		UCA3MCTL = UCBRS_3+UCBRF_0;
		break;
		
	case Baud_19200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;	
			
			UCA3CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA3BR0 = 0xA0;
				UCA3BR1 = 0x01;
				UCA3MCTL = UCBRS_6+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA3BR0 = 0xd0;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_3+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA3BR0 = 0xDA;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_4+UCBRF_0;
			}	
			break;	
		}
            /*
	case Baud_38400:
		{
			if(g_uchPHYXt2Flag == DIS_XT2CLK)
				break;	
			
			UCA3CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA3BR0 = 0xd0;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_3+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA3BR0 = 0x68;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_1+UCBRF_0;
			}
			break;	
		}
            */
	case Baud_57600:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;	
			
			UCA3CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA3BR0 = 0x8a;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_7+UCBRF_0;
			}
			
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA3BR0 = 0x45;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_4+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA3BR0 = 0x48;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_7+UCBRF_0;
			}	
                  
			break;	
		}		
	case Baud_115200:
		{
			//if(g_uchPHYXt2Flag == DIS_XT2CLK)
			//	break;	
			
			UCA3CTL1 =UCSSEL0+UCSSEL1;// CLK = SMCLK
			if(g_uchPHYSMclkSet==PHY_SMCLK_8MHZ)
			{
				//clk = Smclk = 8MHz
				UCA3BR0 = 0x45;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_4+UCBRF_0;
			}
			if(g_uchPHYSMclkSet==PHY_SMCLK_4MHZ)
			{
				//clk = Smclk = 4MHz
				UCA3BR0 = 0x22;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_6+UCBRF_0;
			}
                  
                  if(g_uchPHYSMclkSet==PHY_SMCLK_4200KHZ)
			{
				//clk = Smclk = 4.2MHz
				UCA3BR0 = 0x24;
				UCA3BR1 = 0x00;
				UCA3MCTL = UCBRS_3+UCBRF_0;
			}	
			break;	
		}
		
	default:
		break; 
	}
	
	UCA3CTL1 &=~UCSWRST;	//Initialize USCI state machine
	UCA3IE |= UCRXIE;	 //������3�����ж�
	
	g_uchPHYUART3Open = 1;
} 

/****************************************************************************/
/*	�ر�UART3�� 															*/
/****************************************************************************/
void CloseUartA3(void)
{
	UCA3IE &= ~UCRXIE;	  //��ֹUART3�����ж�
	UCA3CTL1 |=UCSWRST; //RESET
	
	g_uchPHYUART3Open = 0;
}

/*===========================================================================
��ʱ��������õ��ܽ�
��MSP430��ͷ�ļ��У�MC0��MC1���ֶ�������������ĳһ��λΪ1
MC_0��MC_1��MC_2��MC_3����������ÿһ���ε�ȫ�����ܣ���MC����λ����2λ��
����4�ֿ���ѡ��ʽ���ֱ��������ʾ�����ֹ���
#define MC1                 (0x0020u)   Timer A mode control 1       //������ʽѡ��
#define MC0                 (0x0010u)   Timer A mode control 0
#define MC_0                (0*0x10u)   Timer A mode control: 0 - Stop 
#define MC_1                (1*0x10u)   Timer A mode control: 1 - Up to CCR0 
#define MC_2                (2*0x10u)   Timer A mode control: 2 - Continuous up 
#define MC_3                (3*0x10u)   Timer A mode control: 3 - Up/Down 

#define TASSEL1             (0x0200u)   Timer A clock source select 1   //ʱ��ѡ��
#define TASSEL0             (0x0100u)   Timer A clock source select 0 
#define TASSEL_0            (0*0x100u)  Timer A clock source select: 0 - TACLK 
#define TASSEL_1            (1*0x100u)  Timer A clock source select: 1 - ACLK  
#define TASSEL_2            (2*0x100u)  Timer A clock source select: 2 - SMCLK 
#define TASSEL_3            (3*0x100u)  Timer A clock source select: 3 - INCLK 
***********ע��***********
//ע�����ߵı�﷽ʽ������:
��1�ı�﷽ʽ�У��ǰ�TASSEL_1+TACLR��ֵ0x0104��ֵ��ֵ��TA0CTL
��2�ı�﷽ʽ�У�����Ԥ�Ȳ�֪��TA0CTL��ֵ��ֻ�ǿ��Ա�֤TASSEL_1λ��TACLRλ��Ϊ1��
��ʼTA0CTL��ֵ�ǲ����κθı䣬ֻ����ԭ����������һ����������
1��TA0CTL = TASSEL_1 + TACLR;   
2��TA0CTL|= TASSEL_1 + TACLR;
=============================================================================*/
/****************************************************************************/
/*	��ʼ����ʱ��0A0 ��ϵͳ����ʱ�� ACLK = 32768Hz	CCR0 = 32768			*/
/****************************************************************************/
void SetTimer0A0_ACLK(void)
{
	TA0CTL &= ~(MC0+MC1);				        /*��ͣ����*/
	TA0CTL = TASSEL_1 + TACLR;				/*ʹ�ø������� ACLK, ���λCLR*/
	TA0CCTL0 = CCIE;				        /*CCR0 �ж�����	*/
}

/****************************************************************************/
/*	��ʼ����ʱ��0A0 ����ϵͳʱ�� SMCLK										*/
/****************************************************************************/
void SetTimer0A0_SMCLK(void)
{
	TA0CTL &= ~(MC0+MC1);				        /*��ͣ����					 */
	TA0CTL = TASSEL_2 + TACLR;				/*ʹ����ϵͳ���� SMCLK, ���λCLR*/
	TA0CCTL0 = CCIE;				        /*CCR0 �ж�����				*/
}

/****************************************************************************/
/*					  Stop Timer0A0    ֹͣ���� 							*/
/****************************************************************************/
void StopTimer0A0(void)
{
	TA0CTL &= ~(MC0+MC1);				       /*��ͣ����					*/
}

/****************************************************************************/
/*	Set Timer0A0 delay time    ���ö�ʱʱ�� 								*/
/*	����		  : long timeset											*/
/*	��ʱ		  : (timeset / ʱ��Ƶ��) ��1000 ms							*/
/****************************************************************************/
void  Timer0A0_Delay(long timeSet)
{
	TA0CTL &= ~(MC0+MC1);  
	TA0CCR0 = timeSet;                          /*ȷ����ʱ��ʱ��*/
	TA0CTL |= MC0;				 /*��������CCRO������0��ʼ*/
	g_uPHYTimer0A0Count = 0;	          /*ȫ�ֱ��� ��ʱ������λ*/
}

/****************************************************************************/
/*			 ����ϵͳ��ʱ�� ��ʱ1����										*/
/****************************************************************************/
void Timer0A0_Delay_1s_SMCLK(void)
{
	SetTimer0A0_SMCLK();                  //ѡ��SMCLK
	Timer0A0_Delay(PHY_MCLK_1MS);         //һ���ж�(1/1024)��
	while(g_uPHYTimer0A0Count <1024);     //��ѭ��1024��ʱ����ʱ�ﵽ1s
	StopTimer0A0();
}

/****************************************************************************/
/*			   ����ϵͳ��ʱ�� ��ʱ1/1024��									*/
/****************************************************************************/
void Timer0A0_Delay_1ms_SMCLK(void)
{
	SetTimer0A0_SMCLK();
	Timer0A0_Delay(PHY_MCLK_1MS);
	while(g_uPHYTimer0A0Count <1);
	StopTimer0A0();
}

/****************************************************************************/
/*			   ��ϵͳ����ʱ�� ��ʱ1/1024��									*/
/****************************************************************************/
void Timer0A0_Delay_1ms_ACLK(void)
{
	SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK_1MS);
	while(g_uPHYTimer0A0Count < 1);
	StopTimer0A0();
}

/****************************************************************************/
/*			   ��ϵͳ����ʱ�� ��ʱ1���� 									*/
/****************************************************************************/
void Timer0A0_Delay_1s_ACLK(void)
{
	SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK);
	while(g_uPHYTimer0A0Count < 1);
	StopTimer0A0();
}

/****************************************************************************/
/*	��ʼ����ʱ��1A0 ��ϵͳ����ʱ�� ACLK = 32768Hz	CCR0 = 32768			*/
/****************************************************************************/
void SetTimer1A0_ACLK(void)
{
	TA1CTL &= ~(MC0+MC1);				        /*��ͣ����*/
	TA1CTL = TASSEL_1 + TACLR;				/*ʹ�ø������� ACLK, ���λCLR*/
	TA1CCTL0 = CCIE;				        /*CCR0 �ж�����*/
}

/****************************************************************************/
/*	��ʼ����ʱ��1A0 ����ϵͳʱ�� SMCLK = 8388608Hz							*/
/****************************************************************************/
void SetTimer1A0_SMCLK(void)
{
	TA1CTL &= ~(MC0+MC1);				/*��ͣ����					  */
	TA1CTL = TASSEL_2 + TACLR;			/*ʹ����ϵͳ���� SMCLK, ���λCLR*/
	TA1CCTL0 = CCIE;				/*CCR0 �ж����� 			  */
}

/****************************************************************************/
/*					  Stop Timer1A0    ֹͣ���� 							*/
/****************************************************************************/
void StopTimer1A0(void)
{
	TA1CTL &= ~(MC0+MC1);				  /*��ͣ����					*/
}

/****************************************************************************/
/*	Set Timer1A0 delay time    ���ö�ʱʱ�� 								*/
/*	����		  : long timeset											*/
/*	��ʱ		  : (timeset / ʱ��Ƶ��) ��1000     ms							*/
/****************************************************************************/
void  Timer1A0_Delay(long timeSet)
{
	TA1CTL &= ~(MC0+MC1);  
	TA1CCR0 = timeSet; 
	TA1CTL |= MC0;				 /*��������CCRO������0��ʼ*/
	g_uPHYTimer1A0Count = 0;	  /*ȫ�ֱ��� ��ʱ������λ*/
}

/****************************************************************************/
/*			 ����ϵͳ��ʱ�� ��ʱ1����										*/
/****************************************************************************/
void Timer1A0_Delay_1s_SMCLK(void)
{
	SetTimer1A0_SMCLK();
	Timer1A0_Delay(PHY_MCLK_1MS);
	while(g_uPHYTimer1A0Count <1024);//һ���ж�(1/1024)��
	StopTimer1A0();
}

/****************************************************************************/
/*			   ����ϵͳ��ʱ�� ��ʱ1���� 									*/
/****************************************************************************/
void Timer1A0_Delay_1ms_SMCLK(void)
{
	SetTimer1A0_SMCLK();
	Timer1A0_Delay(PHY_MCLK_1MS);
	while(g_uPHYTimer1A0Count <1);
	StopTimer1A0();
}

/****************************************************************************/
/*			   ��ϵͳ����ʱ�� ��ʱ1/1024����								*/
/****************************************************************************/
void Timer1A0_Delay_1ms_ACLK(void)
{
	SetTimer1A0_ACLK();
	Timer1A0_Delay(PHY_ACLK_1MS);
	while(g_uPHYTimer1A0Count < 1);
	StopTimer1A0();
}

/****************************************************************************/
/*			   ��ϵͳ����ʱ�� ��ʱ1���� 									*/
/****************************************************************************/
void Timer1A0_Delay_1s_ACLK(void)
{
	SetTimer1A0_ACLK();
	Timer1A0_Delay(PHY_ACLK);
	while(g_uPHYTimer1A0Count < 1);
	StopTimer1A0();
}

/****************************************************************************/
/*	��ʼ����ʱ��TimerB ��ϵͳ����ʱ�� ACLK = 32768Hz   CCR0 = 32768 ;		*/
/****************************************************************************/
void SetTimerB_ACLK(void)
{
	TBCTL &= ~(MC0+MC1);				  /*��ͣ����					*/
	TBCTL = TBSSEL0 + TBCLR;			  /*ʹ�ø������� ACLK, ���λCLR*/
	TBCCTL0 = CCIE; 					  /*CCR0 �ж�����				*/	  
}
/****************************************************************************/
/*	��ʼ����ʱ��TimerB ����ϵͳ��ʱ��										*/
/****************************************************************************/
void SetTimerB_SMCLK(void)
{
	TBCTL &= ~(MC0+MC1);				  /*��ͣ����					*/
	TBCTL = TBSSEL1 + TBCLR;			  /*ʹ��SMCLK, ���λCLR*/
	TBCCTL0 = CCIE; 			  /*CCR0 �ж�����				*/
	
}

/****************************************************************************/
/*					  Stop Timer_B	  ֹͣ����	     */
/****************************************************************************/
void StopTimerB(void)
{
	TBCTL &= ~(MC0+MC1);				 /*��ͣ���� 				   */
}

/****************************************************************************/
/*	Set Timer_B delay time	  ���ö�ʱʱ��									*/
/*	����		  : long timeset											*/
/*	��ʱ		  : (timeset / ʱ��Ƶ��) ��1000 ms							  */
/****************************************************************************/
void  TimerB_Delay(long timeSet)
{
	TBCTL &= ~(MC0+MC1);  
	TBCCR0 = timeSet; 
	TBCTL |= MC0;				/*��������CCRO������0��ʼ*/
	
	g_uPHYTimerBCount = 0;			   /*ȫ�ֱ��� ��ʱ������λ*/
}

/****************************************************************************/
/*			   ��ϵͳ����ʱ�� ��ʱ1/1024����	            */
/****************************************************************************/
void TimerB_Delay_1ms_ACLK()
{
	SetTimerB_ACLK();
	TimerB_Delay(PHY_ACLK_1MS);
	while(g_uPHYTimerBCount <1);
	StopTimerB();  
}

/****************************************************************************/
/*			   ��ϵͳ����ʱ�� ��ʱ1���� 									*/
/****************************************************************************/
void TimerB_Delay_1s_ACLK()
{
	SetTimerB_ACLK();
	TimerB_Delay(PHY_ACLK);
	while(g_uPHYTimerBCount <1);
	StopTimerB();  
}
/****************************************************************************/
/*     ͨ��ADת�����ܻ�ȡ�˱�ָ���������         P6.7                      */
/****************************************************************************/
unsigned int  AdRandom(unsigned char time)
{
  unsigned int result;
 //��ͨ������ת�������ж� 
  P6SEL |= 0x07;     //ѡ��ADת������
  P6DIR&=~BIT7;
  REFCTL0&=~REFMSTR; //��ȡ�ο���Դ�Ŀ���Ȩ
  ADC12CTL0|=ADC12ON;
  ADC12CTL0|=ADC12SHT03+ADC12REFON+ADC12REF2_5V;
  ADC12CTL1|=ADC12SHP+ADC12SSEL_1+ADC12CSTARTADD_7;//ACLK
  ADC12CTL2|=ADC12RES_2+ADC12SR;//12λ�ֱ��ʣ�����Ƶ��50ksps�����͹���
  ADC12MCTL7|=ADC12INCH_7+ADC12SREF_1;    
  ADC12CTL0 |= ADC12ENC;
  ADC12CTL0|=ADC12SC;
  PHYMCLKSoftDelay1ms(); //��ʱ1ms
  result=ADC12MEM7;
  ADC12IFG&=~ADC12IFG7;
  ADC12CTL0&=~ADC12ENC;
  ADC12CTL0&=~(ADC12SC+ADC12REFON);
  result=result%time;
  return result;
}
/****************************************************************************/
/*    ��ͨ�����в���ģʽ ����һ��ͨ���ظ�����10�Σ�ȡƽ��ֵ                 */
/*	  ADC12���ݲ�������  �õ�P6��, P6.0~P6.7��ӦA0~A7;						*/
/*							 P7�ڣ�p7.4~p7.7��ӦA12~A15������:				*/
/*	  A0---- �弶��Դ�������,��ѹ��ȣ�200K:1M 							*/		 
/*	  A1-----�����ѹDC-DCǰ�ĵ�ص�����⣬��ѹ��ȣ�200K:1M				*/
/*	  A2-----̫���ܻ�AC-DCǰ�ĵ������,��ѹ��ȣ�200K:1M					*/
/*	  A3~A7,A12~A15������ģ�⴫������ѹ���룬RCʱ�䳣����Ҫ�趨 			*/
/*	  �������� char AdSet 0~7,12~15��Ӧ��Ҫ������AD��A0~A7,A12~A15			*/
/*	  ��������ֵ unsigned int ��12λAD���ݣ����λ��LSB 					*/
/*      ��������ֵ����Чλ��12bit,���λ�����1������AD��Ч��                       */
/****************************************************************************/

//��ͨ���ظ� ��չ����ģʽ
unsigned int ADSequence(unsigned char AdSet)
{ 
	unsigned int ad = 0;
	char chP6[] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17};
	char chP7[] = {0x1C,0x1D,0x1E,0x1F};
	
	if(AdSet<=7)//A0~A7     //�ж���Ҫ��һ���ܽŽ���ADת��
      {
        P6SEL |= (0x01 << AdSet);		  //P6��ѡ�� ģ�����빦��
        //P6REN &= ~(0x01 << AdSet);         //��Ҫ�ϡ���������
      }
	else if((AdSet>=12) && (AdSet<=15)) //A12~A15
      {
        P7SEL |= (0x01 <<(AdSet-8));	  //P7��ѡ�� ģ�����빦��
        //P7REN &= ~(0x01 << (AdSet-8));         //��Ҫ�ϡ���������
      }
	else return 0;
	
        REFCTL0&=~ REFMSTR;//msp430f5438a�ĵ�ѹ�ο�ģ��
	ADC12CTL0 &= ~ADC12ENC; 				 // �ı�����ǰֹͣA/Dת��
	
	//�ڲ��ο���ѹ 2.5V��  ADC�ں����Ĵ� ���ж�
	ADC12CTL0 = ADC12REF2_5V + ADC12REFON + ADC12ON; 
      
      // ADC�ں����Ĵ� ���ж�
	//ADC12CTL0 = ADC12ON; 
	
	// ��ͨ���ظ���������չ����ģʽ�������ź�SAMPCON�ɲ��������źſ���
	//���������ź�ԴΪADC12SC����λ
	//ת���ں�ʱ��ԴΪϵͳ����ʱ��
	//�������������ADC12MEM0
	ADC12CTL1 = ADC12CONSEQ_2+ ADC12SSEL_1; 
	
	//��������12bit����������~50KSPS�Խ��͹���
	ADC12CTL2 = ADC12SR + ADC12RES1;
	
	//ת���洢�Ĵ���ADC12MEM0�Ŀ��ƼĴ���
	if(AdSet<=7)//A0~A7
		ADC12MCTL0 = chP6[AdSet]; //ѡ�����ͨ��  �ο��ڲ���ѹԴ Vref+	AVss	
	if((AdSet>=12) && (AdSet<=15)) //A12~A15
		ADC12MCTL0 = chP7[AdSet-12]; //ѡ�����ͨ��  �ο��ڲ���ѹԴ Vref+  AVss 
	
	//�ȴ��ڲ��ο���ѹ�������� Լ20΢��        
        //1��ʱ������(1/PHY_MCLK)��
	unsigned int i = (PHY_MCLK_1MS>>3);	
	while(--i);//4*i ��ʱ������ = 4/(1024*8)�� = 488΢��
	
	ADC12CTL0 |= ADC12ENC; // ����ת��
      
      unsigned char c = 0;
      unsigned long lad = 0;
      while(1)//����10�� ȡƽ��ֵ
      {
        
          ADC12CTL0 |= ADC12SC;  // ��ʼ����	
          
          //��չģʽ�µĲ�������ʱ��		 
          //���1.2M 2M��ѹ����  ��һ�β���RC��·�ȶ�ʱ�� 339uS
          //���1M 0.2M��ѹ����  ��һ�β���RC��·�ȶ�ʱ�� 76uS
          switch(AdSet)
          {
          case 0://A0 200K:1M 
          case 1://A1 200K:1M
          case 2://A2 200K:1M
                //1��ʱ������(1/PHY_MCLK)��
                    i = (PHY_MCLK_1MS>>5);	
                  while(--i);//4*i ��ʱ������ = 4/(1024*32)�� = 122΢��
                    //i = (PHY_MCLK_1MS>>3);	
                  //while(--i);//4*i ��ʱ������ = 4/(1024*8)�� = 488΢��
                break;
          default:
                 i = (PHY_MCLK_1MS>>3);	
                  while(--i);//4*i ��ʱ������ = 4/(1024*8)�� = 488΢��
                break; 
          }
          
          ADC12IFG &= ~0x01 ;//����ձ��
          ADC12CTL0 &= ~ADC12SC;	// ��ʼ����ת��
          
          //while(ADC12CTL1&ADC12BUSY); //�ȴ���ת�����
          
          unsigned int time = PHY_MCLK_1MS;
          time = time*10;
          
          while( (!(ADC12IFG&0x01)) && (--time) ); //40ms �ȴ���ת�����
          if(time == 0)
          {
            ad = 0x8000;
            
            ADC12CTL0 &= ~ADC12ENC; 					// �ı�����ǰֹͣA/Dת��
            ADC12CTL0 &= ~(ADC12REFON + ADC12ON);		//�ر��ڲ��ο���ѹ���ر�ADC12��
            
            if(AdSet<=7)//A0~A7
                  P6SEL &= ~(0x01 << AdSet);		   //P6��ѡ�� ȡ��ģ�����빦��
            else if((AdSet>=12) && (AdSet<=15)) //A12~A15
                  P7SEL &= ~(0x01 <<(AdSet-8));	   //P7��ѡ�� ȡ��ģ�����빦��
            
            return ad;
          }           
          else
            ad = ADC12MEM0;
            
          lad += ad;
          
          if( ++c == 10)
            break;
      }
      
      lad = lad/c;
      
      ad = lad;
	
	ADC12CTL0 &= ~ADC12ENC; 					// �ı�����ǰֹͣA/Dת��
	ADC12CTL0 &= ~(ADC12REFON + ADC12ON);		//�ر��ڲ��ο���ѹ���ر�ADC12��
	
	if(AdSet<=7)//A0~A7
		P6SEL &= ~(0x01 << AdSet);		   //P6��ѡ�� ȡ��ģ�����빦��
	else if((AdSet>=12) && (AdSet<=15)) //A12~A15
		P7SEL &= ~(0x01 <<(AdSet-8));	   //P7��ѡ�� ȡ��ģ�����빦��
	
	return ad;
}

unsigned int ADC12SequenceSample(unsigned char AdSet)
{
  unsigned int ad;
  unsigned int k = 0;
  while(1)
  {
      ad = ADSequence(AdSet);
      if(ad & 0x8000)
          k++;
      else
          return ad;
      if(k > 10)
      {
          WDTCTL = WDT_ARST_1_9; //��������  
          while(1);
      }
  }
}

/****************************************************************************/
/*                        ����ADC12���е�ͨ��������չ����                   */
/*����˵���� ADC12���ݲ�������  �õ�P6��, P6.0~P6.7��ӦA0~A7                */
/*				    P7�ڣ�p7.4~p7.7��ӦA12~A15������:       */
/*	  A0---- �弶��Դ�������,��ѹ��ȣ�200K:1M 			    */		 
/*	  A1-----�����ѹDC-DCǰ�ĵ�ص�����⣬��ѹ��ȣ�200K:1M	    */
/*	  A2-----̫���ܻ�AC-DCǰ�ĵ������,��ѹ��ȣ�200K:1M		    */
/*	  A3~A7,A12~A15������ģ�⴫������ѹ���룬RCʱ�䳣����Ҫ�趨 	    */
/*��������:                                                                 */
/*        unsigned char AdSet: 0~7,12~15��Ӧ��Ҫ������AD��A0~A7,A12~A15	    */
/*        unsigned char Ref:ѡ��ο�Դ��                                    */
/*                1---�ڲ�2.5V��2---�ⲿ��Դ������---�ڲ�1.5V               */
/*        unsigned int *Data:��Ųɼ���������                               */
/*��������ֵ:                                                               */
/*        unsigned int ���زɼ�����ŵ�*Data�е����ݸ�������Ϊ0����ɼ�ʧ��.*/
/****************************************************************************/
unsigned int ADCGetSingleChSingleData(unsigned char AdSet,unsigned char Ref,unsigned int *Data)
{ 
      //Ĭ��ѡ���ڲ��ο�Դ
      unsigned char chP6[] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17}; 
      unsigned char chP7[] = {0x1C,0x1D,0x1E,0x1F};
        
      if(Ref == 2)  //ѡ���ⲿ��Դ AVss AVcc��Ϊ�ο�Դ
      {
        chP6[0] = 0x00;
        chP6[1] = 0x01;
        chP6[2] = 0x02;
        chP6[3] = 0x03;
        chP6[4] = 0x04;
        chP6[5] = 0x05;
        chP6[6] = 0x06;
        chP6[7] = 0x07;

        chP7[0] = 0x0C;
        chP7[1] = 0x0D;
        chP7[2] = 0x0E;
        chP7[3] = 0x0F;
      }
	
      if(AdSet<=7)                        //A0~A7
        P6SEL |= (0x01 << AdSet);         //P6��ѡ�� ģ�����빦��
      else if((AdSet>=12) && (AdSet<=15)) //A12~A15
        P7SEL |= (0x01 <<(AdSet-8));	  //P7��ѡ�� ģ�����빦��
      else return 0;
	REFCTL0&=~ REFMSTR;   //msp430f5438a�ĵ�ѹ�ο�ģ��
      ADC12CTL0 &= ~ADC12ENC; 	//�ı�����ǰֹͣA/Dת��
	
      if(Ref == 1)
	//�ڲ��ο���ѹ 2.5V��  ADC�ں����Ĵ� ���ж�
	ADC12CTL0 = ADC12REF2_5V + ADC12REFON + ADC12ON;
      else if(Ref == 2)
        //ADC�ں����Ĵ� ���ж�
	ADC12CTL0 = ADC12ON;        
      else 
         //�ڲ��ο���ѹ 1.5V��  ADC�ں����Ĵ� ���ж�
        ADC12CTL0 =  ADC12REFON + ADC12ON;
	
      //��ͨ�����β�������չ����ģʽ�������ź�SAMPCON�ɲ��������źſ���
      //���������ź�ԴΪADC12SC����λ
      //ת���ں�ʱ��ԴΪϵͳ��ʱ��
      //�������������ADC12MEM0
      ADC12CTL1 = ADC12CONSEQ_0+ ADC12SSEL_2; 
              
      //��������12bit����������~50KSPS�Խ��͹���,�ο���ѹ״̬ͨ��P5.0���
      //ADC12CTL2 = ADC12SR + ADC12RES1 + ADC12REFOUT;
              
      //��������12bit����������~50KSPS�Խ��͹���
      ADC12CTL2 = ADC12SR + ADC12RES1 ;        
        
      //ת���洢�Ĵ���ADC12MEM0�Ŀ��ƼĴ���
      if(AdSet<=7)//A0~A7
          ADC12MCTL0 = chP6[AdSet]; //ѡ�����ͨ��  �ο���ѹ
      if((AdSet>=12) && (AdSet<=15)) //A12~A15
          ADC12MCTL0 = chP7[AdSet-12]; //ѡ�����ͨ��  �ο���ѹ
	
      //�ȴ��ο���ѹ�������� Լ20΢��        
      //1��ʱ������(1/PHY_MCLK)��
      unsigned int i = (PHY_MCLK_1MS>>3);	
      while(--i);//4*i ��ʱ������ = 4/(1024*8)�� = 488΢��
	
      ADC12CTL0 |= ADC12ENC; // ����ת��
      ADC12CTL0 |= ADC12SC;  // ��ʼ����	
      
      //��չģʽ�µĲ�������ʱ��		 
      //���1.2M 2M��ѹ����  ��һ�β���RC��·�ȶ�ʱ�� 339uS
      //���1M 0.2M��ѹ����  ��һ�β���RC��·�ȶ�ʱ�� 76uS
      switch(AdSet)
      {
          case 0://A0 200K:1M 
          case 1://A1 200K:1M
          case 2://A2 200K:1M
                //1��ʱ������(1/PHY_MCLK)��
                  i = (PHY_MCLK_1MS>>5);	
                  while(--i);//4*i ��ʱ������ = 4/(1024*32)�� = 122΢��
                  
                  i = (PHY_MCLK_1MS>>4)-20;	
                  while(--i);//4*i ��ʱ������ = 4/(1024*16)�� = 244΢��
                break;
          case 3://A3
                  i = (PHY_MCLK_1MS>>4)-20;	
                  while(--i);//4*i ��ʱ������ = 4/(1024*16)�� =244΢��
                  break;
          default:
                 i = (PHY_MCLK_1MS>>4);	
                  while(--i);//4*i ��ʱ������ = 4/(1024*16)�� =244΢��
                break; 
      }
	
      ADC12IFG &= ~0x01 ;       //����ձ��
      ADC12CTL0 &= ~ADC12SC;	// ��ʼ����ת��
	       
      unsigned int time = PHY_MCLK_1MS;          
      while( (!(ADC12IFG&0x01)) && (--time) );          //�ȴ���ת�����
      if(time == 0)
          *Data = 0x8000;
      else
          *Data = ADC12MEM0;
	
      ADC12CTL0 &= ~ADC12ENC; 			// �ı�����ǰֹͣA/Dת��
      ADC12CTL0 &= ~(ADC12REFON + ADC12ON);		//�ر��ڲ��ο���ѹ���ر�ADC12��
	
      if(time == 0)
        return 0;
      else
        return 1;
}

/****************************************************************************/
/*�Ǻ���ADCGetSingleChSingleData����չ����Ӷ�βɼ�ʧ�ܺ�RESET             */
/*��������:                                                                 */
/*        unsigned char AdSet: 0~7,12~15��Ӧ��Ҫ������AD��A0~A7,A12~A15	    */
/*        unsigned char Ref:ѡ��ο�Դ��                                    */
/*                1---�ڲ�2.5V��2---�ⲿ��Դ������---�ڲ�1.5V               */
/*��������ֵ:                                                               */
/*        unsigned int ���زɼ���������                                     */
/****************************************************************************/
unsigned int ADCGetSingleChSingleDataEx(unsigned char AdSet,unsigned char Ref)
{
  unsigned int ad;
  unsigned int k = 0;
  while(1)
  {
      if( 0 == ADCGetSingleChSingleData(AdSet,Ref,&ad))
          k++;
      else
          return ad;
      if(k > 10)
      {
          WDTCTL = WDT_ARST_1_9; //��������  
          while(1);
      }
  }
}


/****************************************************************************/
/*A0��	 ���弶��Դ��ѹ������ֵ�� unsigned int����			    */
/****************************************************************************/
unsigned int BoardEnergyDetect(void)
{
	unsigned int ad = 0;
	ad =ADCGetSingleChSingleDataEx(0,1);
	return ad;
}

/****************************************************************************/
/*A1��	 ����ص�Դ��ѹ������ֵ�� unsigned int����			    */
/****************************************************************************/
unsigned int BattEnergyDetect(void)
{
	unsigned int ad = 0;
	ad =ADCGetSingleChSingleDataEx(1,1);
	return ad;
}

/****************************************************************************/
/*A2��  ������Դ��ѹ������ֵ�� unsigned int����			    */
/****************************************************************************/
unsigned int ChargerEnergyDetect(void)
{
	unsigned int ad = 0;
	ad =ADCGetSingleChSingleDataEx(2,1);
	return ad;
}

/****************************************************************************/
/*	  ϵͳ����͹���ģʽ3													*/
/*unsigned char RFmode ����RF����ģʽ*/
/****************************************************************************/

void EnterLowPowerMode3(unsigned char RFmode)
{
	InitPower();   //�رմ�������Դ�����ŵ�Դ���ر�����ͷ��Դ��
	
	LedGreenOff(); 
	LedRedOff();
	
        switch(RFmode)
        {
        case RF_SHUTDOWN:   //�ر�ģʽ
          SetRFOff();
          break;
        case RF_STANDBY:    //����ģʽ
          SetRFStandby();
          break;
        case RF_SLEEP:      //˯��ģʽ
          SetRFSleep();
          break;
        case RF_READY:      //
          SetRFReady();
          break;
        case RF_TxTune:     //TX����ģʽ
          SetRFTuneForTx();
          break;
        case RF_RxTune:     //RX����ģʽ
          SetRFTuneForRx();
          break;
        default:
          SetRFStandby();
          break;          
        }	
	g_uchPHYSleepMode = PHY_SLEEP_MODE3;   //���߱�־��1��
	
	//if(g_uchPHYXt2Flag == EN_XT2CLK)
	//SetXT2ClkOff();
	
	LPM3;	 
        
        //SMCLKʱ��ѡ��:����XT2
        //if(g_uchPHYSMclkSel == PHY_SMCLK_XT2)
        //  SetXT2ClkOn();
          
}

/****************************************************************************/
/* �͹���ģʽ3  ���뼶����													*/
/* ��ʱ��ACLK= 32768Hz ���Ƶ� ���뼶���� ������ unsigned int ms ���ߵĺ�����*/
/*unsigned char RFmode ����RF����ģʽ*/
/****************************************************************************/
void Sleep_Mode3_ACLKms(unsigned int ms, unsigned char RFmode)
{
	g_uPHYSleppTicks = ms;
	//��ʱ��ѡȡACLK: �͹���ģʽ�� MCLK�Ѿ��ر� ȫ���ر�
	SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK_1MS);
        
        /* ���뵽�͹���ģʽ��ͬʱRF���뵽ָ����ģʽ
           ��Ƭ�����뵽LPM3ģʽ��SMCLK,SMCLK�رգ��������ڴ˴�ִֹͣ�У�ִֻ���жϳ���
        */
	
        EnterLowPowerMode3(RFmode);  //MCU��RF������,XT2ON	  3.6V ��0.126mA = 0.4536 mW (���뵽�͹���ģʽ��ͬʱRF���뵽ָ����ģʽ)  
        
	StopTimer0A0();  
}

/****************************************************************************/
/* �͹���ģʽ3 �뼶���� 												   */
/*��ʱ��ACLK= 32768Hz ���Ƶ� �뼶���� ������ unsigned int s ���ߵ�����	   */
/*unsigned char RFmode ����RF����ģʽ*/
/****************************************************************************/
void Sleep_Mode3_ACLKs(unsigned int s,unsigned char RFmode)
{
	g_uPHYSleppTicks = s;
	//��ʱ��ѡȡACLK: �͹���ģʽ�� MCLK�Ѿ��ر� ȫ���ر�
	SetTimer0A0_ACLK();
	Timer0A0_Delay(PHY_ACLK);     //====�뼶������״̬								
	
        EnterLowPowerMode3(RFmode);//MCU��RF������,XT2ON	 3.6V ��0.126mA = 0.4536 mW        
	StopTimer0A0();  
}

/****************************************************************************/
/* ��Data_ptrָ���FLASH����д��һ���� word 								*/
/****************************************************************************/
void Flash_ww(unsigned int *Data_ptr,unsigned int word)
{
	FCTL3 = 0x0A500;
	FCTL1 = 0x0A540;
	*Data_ptr = word;
}

/****************************************************************************/
/* ��Data_ptrָ���FLASH����д��һ���ֽ�byte								 */
/****************************************************************************/
void Flash_wb(unsigned char *Data_ptr,unsigned char byte)
{
	FCTL3 = 0x0A500;
	FCTL1 = 0x0A540;
	*Data_ptr = byte;
}


/****************************************************************************/
/* ���Data_ptrָ���FLASH�������											*/
/****************************************************************************/
void Flash_clr(unsigned int *Data_ptr)
{
	FCTL1 = 0x0A502;
	FCTL3 = 0x0A500;
	*Data_ptr = 0;
}