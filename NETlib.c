/*****************************************************************
��������������ݴ������⣬��δ�һ���˻����ݵ���һ���˻�
*****************************************************************/
#include "NETLib.h"

#include "PHYLib.h"


#include "RFlib.h"

#include "DLLLib.h"

#include "APPLib.h"

#include "global.h"

/*****************************************************************************************/
/*            ����������,�õ������ҳ��ͬʱ����֡��������֡����� ������������           */
/* FinalAckFlag�γɵ�֡�Ƿ���Ҫ�ܻظ�(0, ��ҪҪ��1����Ҫ��,FrameLength ��Ҫ�γɵ�֡�ĳ���*/
/* ��������ֵ�� -1:ҳ�ռ�С��2���߿ռ䲻��                                               */
/*              ��������ǰ���õ�ҳ�׵�ַ                                                 */
/*****************************************************************************************/
int  GetFreePageNo(unsigned char FeedbackFlag,int FrameLength)
{
    // ���ֻ��һ֡��Ҫ������֡
    if( FeedbackFlag==1)     
    {
      for(int i=0;i<5;i++)
        // 1101 0000 bit4 bit6 bit7����Ч
	if( (g_uchWirelessTxBufCtl[i]  &
             (FRM_INUSE+FRM_TACK+FRM_BEGIN)) == (FRM_INUSE+FRM_TACK+FRM_BEGIN))  
	    return -1;
    }
    
    int uUseableBuffPage = 0;//��һ�����õķ��ͻ�����ҳ��
    
    int pagenum = 0;//����ռ�õ�ҳ����
    
    
    unsigned char txBufCtl[BUFF_MAXPAGE];
    
    for(char count=0; count<BUFF_MAXPAGE; count++) //Ԥ�ȱ���
    {
        txBufCtl[count] = g_uchWirelessTxBufCtl[count];
        g_uchWirelessTxBufCtl[count] = 0;
    }
    
    for(char count=0; count < BUFF_MAXPAGE; )
    {
        //֡��Ч��־Ϊ1  ��Ϊ��ʼҳ 1001 0000
        if((txBufCtl[count] & (FRM_INUSE+FRM_BEGIN)) == 
                              (FRM_INUSE+FRM_BEGIN)) 
	{
            unsigned char DataLen = g_uchWirelessTxBuf[count][0];
            unsigned char *dst = g_uchWirelessTxBuf[uUseableBuffPage];
            unsigned char *src = g_uchWirelessTxBuf[count];
            
            //����ǰ����֡ҳ��ַ��Ӧ��������ظ���ֵ
            if(uUseableBuffPage != count)
            {            
              for(int i = 0; i< DataLen ;i++)
                dst[i] = src[i];  
            }
            
            pagenum = DataLen/PAGESIZE + 1;//����ռ�õ�ҳ����
            
            if(pagenum > BUFF_MAXPAGE)//����ռ�õ�ҳ�����������ҳ�棬����
              return -1;
            
            for(int pagenum1 = 0; pagenum1 < pagenum; pagenum1++ )
              g_uchWirelessTxBufCtl[uUseableBuffPage+pagenum1] = 
                                          txBufCtl[count+pagenum1]; //���Ʊ�־
            
            uUseableBuffPage +=pagenum;
            count +=pagenum;
            
       }//end if 
       else
	  count++;
    } //end for
	
    pagenum = FrameLength/PAGESIZE + 1;//�������������ռ�õ�ҳ����
    
    if( (uUseableBuffPage + pagenum + 1 ) >BUFF_MAXPAGE )//����ҳ���ַ+����ռ�õ�ҳ��+1 >���ҳ��
	return -1;
	
    return(uUseableBuffPage); //���ص�ǰ���õ�ҳ�׵�ַ ,���Ѿ���ҳ���Ʋ��������޸�
}


/****************************************************************************/
/* ��������������������      �γɼ�(����)��ѯ֡   ��    ����        	    */
/*        QueryType 0x10 �򵥲�ѯ,  QueryType 0x20 ���Ӳ�ѯ                 */
/* ��������ֵ�� -1:ҳ�ռ�С��2���߿ռ䲻��                                  */
/*              ��������ǰ�γɵ�֡��ŵ���ҳ��                               */
/****************************************************************************/
int GenerateQueryFrame(unsigned char QueryType)
{	
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
      
    //���֡ͷ     
    g_uchWirelessTxBuf[FreePageNo][0] = QUERY_FRAME_LENGTH;// ֡����
    g_uchWirelessTxBuf[FreePageNo][1] = g_uchWirelessTxBuf[FreePageNo][2] = 0xff;//��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff; //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;     
    
    //����Ŀ������Դ  
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];    
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                     //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum;     //֡���
    g_uchWirelessTxBuf[FreePageNo][11] = QueryType;            //֡������1   0x10 �򵥲�ѯ,0x20 ���Ӳ�ѯ
    g_uchWirelessTxBuf[FreePageNo][12] = 0;                   //֡������2  ������� 
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);         //���CRC
    
    return FreePageNo;
}

/****************************************************************************/
/* ��������������������      �γɼ򵥲�ѯ�϶��ظ�֡       ����        	    */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���*/
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int GenerateOkAck_SimpleQueryFrame(unsigned char *FirstDestID,unsigned char AckFrameNo)
{
	
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
	
	//���֡ͷ         
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_SIMPLEQUERY_FRAME_LENGTH;  // �򵥻ظ�֡����
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];            //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;   //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    
    //memcpy(g_uchWirelessTxBuf[FreePageNo]+5,g_uchWirelessTxBuf[FreePageNo]+1,4);     //����Ŀ������Դ
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                     //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;          //֡���
    g_uchWirelessTxBuf[FreePageNo][11] = ACK;              //�ظ�֡������1   
    g_uchWirelessTxBuf[FreePageNo][12] = INFO_SIMPLE_QUERY;   //֡������2   �ظ�����0x10 �򵥲�ѯ,0x20 ���Ӳ�ѯ
	
    //�������
    g_uchWirelessTxBuf[FreePageNo][13]= g_uchNetLev; //���缶��
    g_uchWirelessTxBuf[FreePageNo][14]= g_uchPHYPAType;  //��������,����Ϊһ���ֽ�
    g_uchWirelessTxBuf[FreePageNo][15]= g_PHYDevInfo.PowerType;//g_eAppPowerType;    //��Դ���� 
    g_uchWirelessTxBuf[FreePageNo][16]= g_eAppPowerStatus;   //����״̬
    g_uchWirelessTxBuf[FreePageNo][17]= g_uchRFPowerVal[g_uchRFPALev];  //�豸����
    g_uchWirelessTxBuf[FreePageNo][18]= g_uchDLLCurLQ;    //������·����
    g_uchWirelessTxBuf[FreePageNo][19]= (g_uchDLLSubDevNumber/16)+(g_uchDLLSubDevNumber&0x0f);  //�������豸����+�����������豸����
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);                   //���CRC
    
    return FreePageNo;  
}


/****************************************************************************/
/* ��������������������      �γɸ��Ӳ�ѯ�϶��ظ�֡       ����        	    */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���*/
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int GenerateOkAck_ComplexQueryFrame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{
	
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
	
	//���֡ͷ          
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_COMPLEXQUERY_FRAME_LENGTH;   //���Ӳ�ѯȷ��֡����:0x19+3*(g_uchDLLSubDevNumber&0x0f)
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];            //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;   //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    
    //����Ŀ������Դ
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;      //֡���
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;           //�ظ�֡������1 
    g_uchWirelessTxBuf[FreePageNo][12]= INFO_COMPLEX_QUERY;//֡������2   �ظ�����0x10 �򵥲�ѯ,0x20 ���Ӳ�ѯ
    //�������    
    g_uchWirelessTxBuf[FreePageNo][13]= g_uchNetLev; //���缶��
    g_uchWirelessTxBuf[FreePageNo][14]= g_uchPHYPAType;  //��������,����Ϊһ���ֽ�
    g_uchWirelessTxBuf[FreePageNo][15]= g_PHYDevInfo.PowerType;//g_eAppPowerType;    //��Դ���� 
    g_uchWirelessTxBuf[FreePageNo][16]= g_eAppPowerStatus;   //����״̬
    g_uchWirelessTxBuf[FreePageNo][17]= g_uPHYChargePower/16;  //����ѹ
    g_uchWirelessTxBuf[FreePageNo][18]= g_uPHYBattPower/16;    //��ص�ѹ
    g_uchWirelessTxBuf[FreePageNo][19]= g_uPHYBoardPower/16;    //�弶��ѹ
    g_uchWirelessTxBuf[FreePageNo][20]= g_uchRFPowerVal[g_uchRFPALev];  //�豸����
    g_uchWirelessTxBuf[FreePageNo][21]= g_uchDLLCurLQ;  //������·����   
    
    //�����������豸��Ϣ    
    int i=0,j=0;
    
    for(;i<NET_MAX_CAP;i++)
    {
	if( ( g_SubDevInfo[i].Status==1) || (g_SubDevInfo[i].Status==4)) //�������� + ��֤�ɹ�
	{
            g_uchWirelessTxBuf[FreePageNo][23+j*3]=g_SubDevInfo[i].NetID&0xff;
            g_uchWirelessTxBuf[FreePageNo][24+j*3]=g_SubDevInfo[i].NetID/256;
            g_uchWirelessTxBuf[FreePageNo][25+j*3]=g_SubDevInfo[i].PowerStatus;
            j++;
	}
    } 
    
    g_uchDLLSubDevNumber = ( (g_uchDLLSubDevNumber&0xf0) | (j&0x0f) ) ;//���¸��豸������ֵ
    
    g_uchWirelessTxBuf[FreePageNo][22]= g_uchDLLSubDevNumber & 0x0F ;  //�����������豸����
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_COMPLEXQUERY_FRAME_LENGTH; //�������֡�� 
     
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    return FreePageNo;
}

/****************************************************************************/
/* ��������������������      ����ע���豸�γ�����ע��֡   0x30    ��        */
/* ��������ֵ�� -1:ҳ�ռ�С��2���߿ռ䲻��                                  */
/*              ��������ǰ�γɵ�֡��ŵ���ҳ��                               */
/* ֻ����һ�Σ����ɹ�������                                                 */
/****************************************************************************/
int GenerateApplyLogFrame(unsigned char *FirstDestID)
{
	int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
        
	//���֡ͷ   
	g_uchWirelessTxBuf[FreePageNo][0] = APPLY_FRAME_LENGTH; //��ת������ע��֡֡��
	g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //��֡Ŀ��
	g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
	g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
	g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
	g_uchWirelessTxBuf[FreePageNo][5] = 0x00;                //����Ŀ��
        g_uchWirelessTxBuf[FreePageNo][6] = 0x00;
        g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //��ʼԴ
        g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
	g_uchWirelessTxBuf[FreePageNo][9] = 0;      //·�ɸ���
	g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum; //֡���
	g_uchWirelessTxBuf[FreePageNo][11] = INFO_APPLY;  //����ע��֡
	g_uchWirelessTxBuf[FreePageNo][12] = 0;  //�ڶ��������֣�0���
	g_uchWirelessTxBuf[FreePageNo][13] = g_uchPHYPAType;  //��������,����Ϊһ���ֽ�
	g_uchWirelessTxBuf[FreePageNo][14] = ( g_PHYDevInfo.DevType & 0x0F ) + ( (g_uchAppchRoleSet & 0x0F) * 16 ) ;  //�����豸���� ��ǰ�豸���� 
	
	//�豸���ʸ��ݵ�λѡ��
	g_uchWirelessTxBuf[FreePageNo][15] = g_uchRFPowerVal[g_uchRFPALev];  //�豸����
	
        //����ע���豸����ID
	g_uchWirelessTxBuf[FreePageNo][16] = g_uchWirelessTxBuf[FreePageNo][3];
        g_uchWirelessTxBuf[FreePageNo][17] = g_uchWirelessTxBuf[FreePageNo][4];
	 
        // ����ע���豸MAC��ַ 
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
/* ��������������������      �γ�����ע��ȴ���֤�ظ�֡    ��    ����        */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���*/
/*            MAC:��ŷ�������֡��MAC��ַ                                   */
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int GenerateWaitCheckACK_ApplyLogFrame(unsigned char* FirstDestID,unsigned char AckFrameNo,unsigned char *MAC)
{
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
	
	//���֡ͷ        
	g_uchWirelessTxBuf[FreePageNo][0] = WAITCHECKACK_APPLY_FRAME_LENGTH;     //����ע��ȴ���֤�ظ�֡���� 0x1a 
	g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //��֡Ŀ��
	g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
	g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
	g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
	//����Ŀ������Դ
	g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
        g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
        g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
        g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
        
	g_uchWirelessTxBuf[FreePageNo][9] = 0;      //·�ɸ���
	g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //֡���
    
	g_uchWirelessTxBuf[FreePageNo][11]= ACK;            //  ֡������1��0x55
	g_uchWirelessTxBuf[FreePageNo][12]= CHECK_WAIT_ACK; //  ֡������2��0x31
    
	//����ע���豸����ID
	g_uchWirelessTxBuf[FreePageNo][13] = g_uchWirelessTxBuf[FreePageNo][1];
        g_uchWirelessTxBuf[FreePageNo][14] = g_uchWirelessTxBuf[FreePageNo][2];
                                
        // ����ע���豸MAC��ַ   
	g_uchWirelessTxBuf[FreePageNo][15] = MAC[0];
        g_uchWirelessTxBuf[FreePageNo][16] = MAC[1];
        g_uchWirelessTxBuf[FreePageNo][17] = MAC[2];
        g_uchWirelessTxBuf[FreePageNo][18] = MAC[3];
	g_uchWirelessTxBuf[FreePageNo][19] = MAC[4];
        g_uchWirelessTxBuf[FreePageNo][20] = MAC[5];
        g_uchWirelessTxBuf[FreePageNo][21] = MAC[6];
        g_uchWirelessTxBuf[FreePageNo][22] = MAC[7];
        
        //�趨Ŀ���豸ע����  
        g_uchWirelessTxBuf[FreePageNo][23] = 60;//�ȴ���֤��ʱ���� ����Ϊ��λ

	AddCRC(g_uchWirelessTxBuf[FreePageNo]);
	
	return FreePageNo;
}

/****************************************************************************/
/* ��������������������      �γ�ע����֤ͨ���ظ�֡    ��    ����           */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���*/
/*            MAC:��ŷ�������֡��MAC��ַ                                   */
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int GenerateCheckPassACK_ApplyFrame(unsigned char *FirstDestID, unsigned char AckFrameNo, unsigned char *MAC) 
{
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
	
    //���֡
    g_uchWirelessTxBuf[FreePageNo][0] = CHECKPASSACK_APPLY_FRAME_LENGTH;      //0x1C
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //����Ŀ������Դ
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];	
    
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //֡���
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;                   //  ֡������1��0x55
    g_uchWirelessTxBuf[FreePageNo][12]= CHECK_PASS_ACK;    //  ֡������2��0x32
    
    //����ע���豸����ID
    g_uchWirelessTxBuf[FreePageNo][13] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][14] = g_uchWirelessTxBuf[FreePageNo][2];	
    
    // ����ע���豸MAC��ַ 
    g_uchWirelessTxBuf[FreePageNo][15] = MAC[0];
    g_uchWirelessTxBuf[FreePageNo][16] = MAC[1];
    g_uchWirelessTxBuf[FreePageNo][17] = MAC[2];
    g_uchWirelessTxBuf[FreePageNo][18] = MAC[3];
    g_uchWirelessTxBuf[FreePageNo][19] = MAC[4];
    g_uchWirelessTxBuf[FreePageNo][20] = MAC[5];
    g_uchWirelessTxBuf[FreePageNo][21] = MAC[6];
    g_uchWirelessTxBuf[FreePageNo][22] = MAC[7];	
	
	
    g_uchWirelessTxBuf[FreePageNo][23] =  g_uchRFPowerVal[g_uchRFPALev]; //��֡Դ����
	
    //�趨Ŀ���豸Ӧ�ò�ע����  
    g_uchWirelessTxBuf[FreePageNo][24]= (g_uchNetLev+1)*2; //��Ԥ���趨һ��ֵ��Ԥ�������������
    
    //�趨Ŀ���豸��·��ע����  
    g_uchWirelessTxBuf[FreePageNo][25]= 3; //��Ԥ���趨һ��ֵ��Ԥ�������������
	
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
/* ��������������������      �γ�ע����֤ʧ�ܻظ�֡    ��    ����           */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���*/
/*            MAC:��ŷ�������֡��MAC��ַ                                   */
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int GenerateCheckFailACK_ApplyFrame(unsigned char* FirstDestID,unsigned char AckFrameNo,unsigned char *MAC)
{
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ	
	
    //���֡
    g_uchWirelessTxBuf[FreePageNo][0] = CHECKFAILACK_APPLY_FRAME_LENGTH;       //ע����֤ʧ�ܻظ�֡֡��0x19
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //����Ŀ������Դ
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];	
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //֡���
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;                  //  ֡������1��0x55
    g_uchWirelessTxBuf[FreePageNo][12]= CHECK_FAIL_ACK ;    //  ֡������2��0x33
    
    //����ע���豸����ID
    g_uchWirelessTxBuf[FreePageNo][13] = g_uchWirelessTxBuf[FreePageNo][1];
    g_uchWirelessTxBuf[FreePageNo][14] = g_uchWirelessTxBuf[FreePageNo][2];	
    
    // ����ע���豸MAC��ַ 
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
/* ��������������������      ����ע���ܻظ�   0x50    ��                    */
/* ����������FirstDestID:��ű�֡Ŀ�ĵ�ַ��
FrameCmd2�������֤�����ͨ����֤0x32/��֤ʧ��0x33
ApplyDevID���������Ŀ�ĵ�ַ��Ҳ�Ƿ�������ע����Ϣ���豸��ַ
ApplyMAC����ŷ�������ע����Ϣ���豸MAC��ַ
AppLogIntTime���趨�豸��Ӧ�ò�ע����ʱ��
DllLogIntTime:�趨�豸����·��ע����ʱ��
Router��·����Ϣ��Router[0]��·�ɸ�����������·�ɵ�ַ����λ��ǰ*/   
/* ��������ֵ�� -1:ҳ�ռ�С��2���߿ռ䲻��                                  */
/*              ��������ǰ�γɵ�֡��ŵ���ҳ��                               */        
/****************************************************************************/
int GenerateApplyFeedbackFrame(unsigned char* FirstDestID,unsigned char FrameCmd2,unsigned char* ApplyDevID,
							   unsigned char *ApplyMAC,unsigned char AppLogIntTime, unsigned char DllLogIntTime,unsigned char *Router)
{
        int FrameLength = FINALACK_APPLY_FRAME_LENGTH+2*Router[0]; //֡����
        int FeedbackFlag = 0;  //֡�ظ���־
        
	//�жϻ������Ƿ��������   
	int FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
	if(FreePageNo==-1)
          return -1;             //�������
        
        //֡����������
        //  1��01 0000 ��ʼҳ
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1��00 0000 �м�ҳ
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }
	
	//���֡ͷ   
	g_uchWirelessTxBuf[FreePageNo][0] = FINALACK_APPLY_FRAME_LENGTH+2*Router[0]; //����ע��֡�ܻظ�
	g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //��֡Ŀ��
	g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
	g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
	g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
	g_uchWirelessTxBuf[FreePageNo][5] = ApplyDevID[0];        //����Ŀ��
	g_uchWirelessTxBuf[FreePageNo][6] = ApplyDevID[1];
	g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];  //��ʼԴ
	g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
	
	g_uchWirelessTxBuf[FreePageNo][9] = 0;      //·�ɸ���
	g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum; //֡���
	g_uchWirelessTxBuf[FreePageNo][11] = INFO_FEEDBACK;  //�ܻظ�֡
	g_uchWirelessTxBuf[FreePageNo][12] = FrameCmd2;  //  ͨ������ʧ��  ͨ����֤0x32/��֤ʧ��0x33
	g_uchWirelessTxBuf[FreePageNo][13] = ApplyDevID[0];  //�����豸ID
	g_uchWirelessTxBuf[FreePageNo][14] = ApplyDevID[1];  //
	
	///�����豸��MAC��ַ	
	g_uchWirelessTxBuf[FreePageNo][15] = ApplyMAC[0];
        g_uchWirelessTxBuf[FreePageNo][16] = ApplyMAC[1];
        g_uchWirelessTxBuf[FreePageNo][17] = ApplyMAC[2];
        g_uchWirelessTxBuf[FreePageNo][18] = ApplyMAC[3];
        g_uchWirelessTxBuf[FreePageNo][19] = ApplyMAC[4];
        g_uchWirelessTxBuf[FreePageNo][20] = ApplyMAC[5];
        g_uchWirelessTxBuf[FreePageNo][21] = ApplyMAC[6];
        g_uchWirelessTxBuf[FreePageNo][22] = ApplyMAC[7];	
	
	g_uchWirelessTxBuf[FreePageNo][23] = AppLogIntTime;  //Ӧ�ò�ע����
	g_uchWirelessTxBuf[FreePageNo][24] = DllLogIntTime;  //��·��ע����
	
	memcpy(g_uchWirelessTxBuf[FreePageNo]+25,Router+1,2*Router[0]);  //·������
	
	AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
	return FreePageNo;
}

/****************************************************************************/
/* ��������������������     �γ�������ע��֡    ��      0x35 ����          */
/* ��������ֵ�� -1:ҳ�ռ�С��2���߿ռ䲻��                                  */
/*              ��������ǰ�γɵ�֡��ŵ���ҳ��                               */
/****************************************************************************/
int GenerateDllLogFrame(void)
{          
        int FrameLength = DLL_LOG_FRAME_LENGTH; //֡����
        int FeedbackFlag = 0;  //֡�ظ���־
        
	//�жϻ������Ƿ��������   
	int FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
	if(FreePageNo==-1)
        return -1;             //�������
        
        //֡����������
        //  1��01 0000 ��ʼҳ
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1��00 0000 �м�ҳ
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }

    //���֡ͷ     
    g_uchWirelessTxBuf[FreePageNo][0] = DLL_LOG_FRAME_LENGTH;       // ֡����
    
    g_uchWirelessTxBuf[FreePageNo][1] = g_uNetParentID&0xff;         //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = g_uNetParentID/256;
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //����Ŀ��
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //��ʼԴ
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                     //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum;     //֡���
    g_uchWirelessTxBuf[FreePageNo][11] = INOF_DLL_LOG;            //֡������1   0x36������ע��
    g_uchWirelessTxBuf[FreePageNo][12] = 0;                   //֡������2  ������� 
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);         //���CRC
    
    return FreePageNo;
}

/****************************************************************************/
/* ��������������������     �γ�������ע��֡ȷ�ϻظ�          ����          */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���*/
/* ��������ֵ�� -1:ҳ�ռ�С��2���߿ռ䲻��                                  */
/*              ��������ǰ�γɵ�֡��ŵ���ҳ��                               */
/****************************************************************************/
int GenerateOkAck_DllLogFrame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{          
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
	
	//���֡ 
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_DLL_LOG_FRAME_LENGTH ;       //���ݻظ�֡֡��  0x0F
	
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //����Ŀ�ĳ�ʼԴ
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //����Ŀ��
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //��ʼԴ
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //֡���
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;             //  ֡������1��0x55
    g_uchWirelessTxBuf[FreePageNo][12]= INOF_DLL_LOG ;    //  ֡������2��0x36
 
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
	
    return FreePageNo;
}

/****************************************************************************/
/* ��������������������     ����Դ�γ�����״̬��Ϣ֡    ��   0x40 ����      */
/* ����������Data���������Ϣ��Data[0]�����ݵ��ֽڸ���,���������           */
/*  CtlByte ���γ�����״̬��Ϣ�ĵڶ���������                                
                           0xb0	�����Ĵ�����������Ϣ        
                           0xb1	Data��������״̬���������ID�豸��ʧ
                           0xb2	Data��������״̬���������ID����������      */
/* ��������ֵ�� -1:ҳ�ռ�С��2���߿ռ䲻��                                  */
/*              ��������ǰ�γɵ�֡��ŵ���ҳ��                               */
/****************************************************************************/
int GenerateDataStatusFrame(unsigned char *Data,unsigned char CtlByte)
{          
        int FrameLength = DATA_STATUS_FRAME_BASE_LENGTH+Data[0]; //֡����
        
        int FeedbackFlag = 1;  //֡�ظ���־
        
	//�жϻ������Ƿ��������   
	int FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
	if(FreePageNo==-1)
        return -1;    //�������
        
        //֡����������
        //  1��01 0000 ��ʼҳ
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1��00 0000 �м�ҳ
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }
	
	//���֡
    g_uchWirelessTxBuf[FreePageNo][0] = DATA_STATUS_FRAME_BASE_LENGTH+Data[0]; //����֡֡��
    g_uchWirelessTxBuf[FreePageNo][1] = g_uNetParentID&0xff;         //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = g_uNetParentID/256;
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    g_uchWirelessTxBuf[FreePageNo][5] = 0x00;                //����Ŀ��
    g_uchWirelessTxBuf[FreePageNo][6] = 0x00;
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //��ʼԴ
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0; //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum; //֡���    
	
    g_uchWirelessTxBuf[FreePageNo][11]= INFO_DATA_STATUS;      //  ֡������1��0x40
    g_uchWirelessTxBuf[FreePageNo][12]= CtlByte ;              //  ֡������2��
	
	
	g_uchWirelessTxBuf[FreePageNo][13]= g_uchNetLev;    //���缶��
	g_uchWirelessTxBuf[FreePageNo][14]= g_uchPHYPAType;  //��������,����Ϊһ���ֽ�
	g_uchWirelessTxBuf[FreePageNo][15]=  ( g_PHYDevInfo.DevType & 0x0F ) + ( (g_uchAppchRoleSet & 0x0F) * 16 ) ;  //�����豸���� ��ǰ�豸���� 
	g_uchWirelessTxBuf[FreePageNo][16]= g_uchRFSelfFreNum;    //����Ƶ��
	g_uchWirelessTxBuf[FreePageNo][17]= g_uchRFPowerVal[g_uchRFPALev];  //�豸����
	g_uchWirelessTxBuf[FreePageNo][18]= g_PHYDevInfo.PowerType;    //��Դ����               
	g_uchWirelessTxBuf[FreePageNo][19]= g_eAppPowerStatus;   //����״̬
	g_uchWirelessTxBuf[FreePageNo][20]= g_uPHYChargePower/16;                     //����ѹ
	g_uchWirelessTxBuf[FreePageNo][21]= g_uPHYBattPower/16;                     //��ص�ѹ
	g_uchWirelessTxBuf[FreePageNo][22]= g_uPHYBoardPower/16;                     //�弶��ѹ
	g_uchWirelessTxBuf[FreePageNo][23]= g_uchDLLCurLQ;                     //��·����
	g_uchWirelessTxBuf[FreePageNo][24]= (g_uchDLLSubDevNumber&0x0f);   //�����������豸����
	g_uchWirelessTxBuf[FreePageNo][25]= g_uAppDataInt/60;       //ע����, ����
	g_uchWirelessTxBuf[FreePageNo][26]= g_uAppDataType&0xFF;   //���������� ��λ
	g_uchWirelessTxBuf[FreePageNo][27]= g_uAppDataType/256;   //���������� ��λ
	
	memcpy(g_uchWirelessTxBuf[FreePageNo]+28,Data+1,Data[0]);     //������Ϣ
	
	AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
	return FreePageNo;
}

/****************************************************************************/
/* ���������������� �γ�������Ϣ֡�ظ�֡    ��    ����        	            */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���*/
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int GenerateOkAck_DataStatusFrame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
	
	//���֡ 
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_DATA_STATUS_FRAME_LENGTH ;       //���ݻظ�֡֡��  0x14
	
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //����Ŀ�ĳ�ʼԴ
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //����Ŀ��
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //��ʼԴ
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //֡���
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;             //  ֡������1��0x55
    g_uchWirelessTxBuf[FreePageNo][12]= INFO_DATA_STATUS ;    //  ֡������2��0x40
    
	g_uchWirelessTxBuf[FreePageNo][13]=  g_uchRFPowerVal[g_uchRFPALev];  //���豸����
    
    g_uchWirelessTxBuf[FreePageNo][14]= g_uchDLLCurLQ;  //������·����
    
    g_uchWirelessTxBuf[FreePageNo][15]= g_uchNetLev; //�����缶��
	
    g_uchWirelessTxBuf[FreePageNo][16]= g_uchDLLSubDevNumber&0x0f;//���豸��ǰ���ļ���������豸����
    //�趨Ŀ���豸ע����  	
    g_uchWirelessTxBuf[FreePageNo][17]= (g_uchNetLev+1)*2; //��Ԥ���趨һ��ֵ��Ԥ�������������
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
/* ��������������������      �γ�������Ϣ֡�ܵĻظ�֡  ��    ����           */
/* ����������FirstDestID:��ű�֡Ŀ�ĵ�ַ��
FinalDestID���������Ŀ�ĵ�ַ��Ҳ�Ƿ�������ע����Ϣ���豸��ַ
Router��·����Ϣ��Router[0]��·�ɸ�����������·�ɵ�ַ����λ��ǰ
LogTime: ��������Ŀ���豸��ע����ʱ��                        */                
/* ��������ֵ�� -1:ҳ�ռ�С��2���߿ռ䲻��                                  */
/*              ��������ǰ�γɵ�֡��ŵ���ҳ��                               */    
/****************************************************************************/
int GenerateDataFeedbackFrame(unsigned char* FirstDestID,unsigned char* FinalDestID,unsigned char *Router,unsigned char LogTime)
{        
        int FrameLength = DATA_STATUS_FEEDBACK_FRAME_BASE_LENGTH+2*Router[0]; //֡����
        int FeedbackFlag = 0;  //֡�ظ���־
        
	//�жϻ������Ƿ��������   
	int FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
	if(FreePageNo==-1)
          return -1;             //�������
        
        //֡����������
        //  1��01 0000 ��ʼҳ
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1��00 0000 �м�ҳ
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }
	
	//���֡
    g_uchWirelessTxBuf[FreePageNo][0] = DATA_STATUS_FEEDBACK_FRAME_BASE_LENGTH+2*Router[0]; //����֡֡��
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    g_uchWirelessTxBuf[FreePageNo][5] = FinalDestID[0];               //����Ŀ��
    g_uchWirelessTxBuf[FreePageNo][6] = FinalDestID[1]; 
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];  //��ʼԴ
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
	
	
    g_uchWirelessTxBuf[FreePageNo][9] = Router[0]; //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum; //֡���
    
	
    g_uchWirelessTxBuf[FreePageNo][11]= INFO_FEEDBACK;              //  �ܻظ� ֡������1��0x50
    g_uchWirelessTxBuf[FreePageNo][12]= INFO_DATA_STATUS;           //  �ܻظ����� ֡������2������0x40
	
	
    //�趨ע����   
    g_uchWirelessTxBuf[FreePageNo][13]= LogTime;
    
    memcpy(g_uchWirelessTxBuf[FreePageNo]+14,Router+1,2*Router[0]);  //·������
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);         
	
    return FreePageNo;  
}

/****************************************************************************/
/* ��������������������      �γ�������Ϣ֡�ܻظ��Ļظ�֡����        	    */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���*/
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int SendOkAck_FeedbackFrame(unsigned char*FirstDestID,unsigned char AckFrameNo)
{
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
	
    //���֡ 
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_DATA_STATUS_FEEDBACK_FRAME_BASE_LENGTH ;     
	
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //����Ŀ������Դ
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //����Ŀ��
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //��ʼԴ
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //֡���
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;                        //  ֡������1��0x55
    g_uchWirelessTxBuf[FreePageNo][12]= INFO_FEEDBACK ;    //  ֡������2��0x50
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    g_uchRxPAtype = g_uNetParentPAtype; //�����豸����    

    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //������Ϣ֡�ܻظ��Ļظ�֡
	
    return FreePageNo;
}


/****************************************************************************/
/* ��������������������      �γ�����æ�ظ�    ��    ����                   */
/*  ����������DestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���    */
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int SendBusyAckFrame(unsigned char* DestID,unsigned char AckFrameNo)
{
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
	
	//���֡         
    g_uchWirelessTxBuf[FreePageNo][0] = BUSYACK_LENGTH;      //֡����
    g_uchWirelessTxBuf[FreePageNo][1] = DestID[0];         //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = DestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //����Ŀ������Դ
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //����Ŀ��
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //��ʼԴ
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
    
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                     //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;     //֡���
    g_uchWirelessTxBuf[FreePageNo][11] = ACK;            //֡������1   �ظ�֡0x55
    g_uchWirelessTxBuf[FreePageNo][12] = BUSY_ACK;       //֡������2  æ0xa2
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);         //���CRC
    
    //�ж�Ŀ���豸������
    if( (DestID[0] + DestID[1]*256) == g_uNetParentID )//���ϼ��豸
        g_uchRxPAtype = g_uNetParentPAtype; //�����豸����
    else
    {
        int i=0;
	
	 for(;i<NET_MAX_CAP;i++) //����ǲ������豸������
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
    
    
    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //ֱ�ӻظ�����æ  
	
    return FreePageNo;    
}

/****************************************************************************/
/* ��������������������      �γ����������֡    ��  0x60  ����        	    */
/* ����������FirstDestID:��ű�֡Ŀ�ĵ�ַ��
FinalDestID���������Ŀ�ĵ�ַ��Ҳ�Ƿ�������ע����Ϣ���豸��ַ
CmdData :��ž����������
Router��·����Ϣ��Router[0]��·�ɸ�����������·�ɵ�ַ����λ��ǰ  */            
/* ��������ֵ�� -1:ҳ�ռ�С��2���߿ռ䲻��                                  */
/*              ��������ǰ�γɵ�֡��ŵ���ҳ��                               */ 
/****************************************************************************/
int GenerateCmdFrame(unsigned char* FirstDestID,unsigned char* FinalDestID,unsigned char *CmdData,unsigned char *Router)
{
        int FrameLength = CMDFRAME_BASE_LENGTH+2*Router[0]; //֡����
        int FeedbackFlag = 0;  //֡�ظ���־
        
	//�жϻ������Ƿ��������   
	int FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
	if(FreePageNo==-1)
          return -1;             //�������
        
        //֡����������
        //  1��01 0000 ��ʼҳ
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1��00 0000 �м�ҳ
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }
	
	//���֡
    g_uchWirelessTxBuf[FreePageNo][0] = CMDFRAME_BASE_LENGTH+2*Router[0]; //����֡֡��
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    g_uchWirelessTxBuf[FreePageNo][5] = FinalDestID[0];               //����Ŀ��
    g_uchWirelessTxBuf[FreePageNo][6] = FinalDestID[1]; 
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3];    //��ʼԴ
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4];
	
	
    g_uchWirelessTxBuf[FreePageNo][9] = Router[0]; //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = g_uchDLLFrameNum; //֡���    
	
    g_uchWirelessTxBuf[FreePageNo][11]= INFO_CMD;                        //  ֡������1��0x60
	
    memcpy(g_uchWirelessTxBuf[FreePageNo]+12,CmdData,15);     //��������
    memcpy(g_uchWirelessTxBuf[FreePageNo]+27,Router+1,2*Router[0]);  //·������
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    return FreePageNo;
	
}

/****************************************************************************/
/* ��������������������      �γ��������֡ȷ�ϻظ�    ��    ����           */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���*/
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int SendOkAck_CmdFrame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
	
	//���֡ 
    g_uchWirelessTxBuf[FreePageNo][0] = OKACK_CMDFRAME_LENGTH ;       //����֡�ظ�0x11
	
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];         //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
	
    //����Ŀ������Դ
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //����Ŀ��
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //��ʼԴ
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
    
    g_uchWirelessTxBuf[FreePageNo][9] = 0;      //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo; //֡���
	
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;                        //  ֡������1��0x55
    g_uchWirelessTxBuf[FreePageNo][12]= INFO_CMD ;    //  ֡������2��0x60
    
    g_uchWirelessTxBuf[FreePageNo][13]= g_uchRFPowerVal[g_uchRFPALev];  //�豸����
    g_uchWirelessTxBuf[FreePageNo][14]= g_uchDLLCurLQ;  //������·����
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);   
    
    g_uchRxPAtype = g_uNetParentPAtype; //�����豸����
    
    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //ֱ�ӻظ�����æ 
    
    return FreePageNo;
}


/****************************************************************************/
/* ��������������������      �γ�����ת��֡    ��    ����        	    */
/*                  pRelayFrame���յ�����Ҫת����֡                         */
/* ��������ֵ�� -1:ҳ�ռ�С��2���߿ռ䲻��                                  */
/*              ��������ǰ�γɵ�֡��ŵ���ҳ��                               */ 
/****************************************************************************/
int GenerateRelayFrame(unsigned char *pRelayFrame)
{
	int FreePageNo;
	//�ж������л�����������
	if( ( pRelayFrame[9] >0 ) &&( pRelayFrame[pRelayFrame[0] - 4 ] == (g_PHYDevInfo.NetID&0xff)) &&
		( pRelayFrame[pRelayFrame[0] - 3 ] == (g_PHYDevInfo.NetID/256)) )    //��������
	{			
	        int FrameLength = pRelayFrame[0]-2; //֡����
                int FeedbackFlag = 0;  //֡�ظ���־
                
                //�жϻ������Ƿ��������   
                FreePageNo = GetFreePageNo(FeedbackFlag,FrameLength);  
                if(FreePageNo==-1)
                  return -1;             //�������
                
                //֡����������
                //  1��01 0000 ��ʼҳ
                g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
                
                unsigned char pagenum = (FrameLength/PAGESIZE+1);
                if(pagenum>1)
                {
                  for(unsigned char num = 1; num <pagenum ; num++)// 1��00 0000 �м�ҳ
                    g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
                }
        
		memcpy(g_uchWirelessTxBuf[FreePageNo],pRelayFrame,pRelayFrame[0]-4);     //���������ͻ���
		g_uchWirelessTxBuf[FreePageNo][0] -= 2;       //֡����2
		g_uchWirelessTxBuf[FreePageNo][9]--;       //·�ɼ�1
		
		if(g_uchWirelessTxBuf[FreePageNo][9] == 0) //·��Ϊ��,��֡Ŀ�ľ�������Ŀ��
		{
			g_uchWirelessTxBuf[FreePageNo][1] = g_uchWirelessTxBuf[FreePageNo][5];         //��֡Ŀ��
			g_uchWirelessTxBuf[FreePageNo][2] = g_uchWirelessTxBuf[FreePageNo][6];
		}
		else //��֡Ŀ��Ϊ·�ɱ��е���һ��·��ID
		{
			g_uchWirelessTxBuf[FreePageNo][1] = g_uchWirelessTxBuf[FreePageNo][g_uchWirelessTxBuf[FreePageNo][0]-4];         //��֡Ŀ��
			g_uchWirelessTxBuf[FreePageNo][2] = g_uchWirelessTxBuf[FreePageNo][g_uchWirelessTxBuf[FreePageNo][0]-3];
		}
    }
    
    else //��������
    {			
        int FrameLength = pRelayFrame[0]+2; //֡����
        int FeedbackFlag = 0;  //֡�ظ���־
                
        //�жϻ������Ƿ��������   
        FreePageNo=GetFreePageNo(FeedbackFlag,FrameLength);  
        
        if(FreePageNo==-1)
           return -1;             //�������
                
        //֡����������
        //  1��01 0000 ��ʼҳ
        g_uchWirelessTxBufCtl[FreePageNo]= FRM_INUSE+FRM_BEGIN+FRM_TACK*FeedbackFlag ;
        
        unsigned char pagenum = (FrameLength/PAGESIZE+1);
        if(pagenum>1)
        {
          for(unsigned char num = 1; num <pagenum ; num++)// 1��00 0000 �м�ҳ
            g_uchWirelessTxBufCtl[FreePageNo+num]= FRM_INUSE+FRM_TACK*FeedbackFlag ;
        }
        
        memcpy(g_uchWirelessTxBuf[FreePageNo],pRelayFrame,pRelayFrame[0]-2);     //���������ͻ���
        g_uchWirelessTxBuf[FreePageNo][0]+=2;       //֡����2
        g_uchWirelessTxBuf[FreePageNo][9]++;       //·�ɼ�1
        
        g_uchWirelessTxBuf[FreePageNo][1] = g_uNetParentID&0xff;         //��֡Ŀ��Ϊ�ϼ��豸
        g_uchWirelessTxBuf[FreePageNo][2] = g_uNetParentID/256;
        
        g_uchWirelessTxBuf[FreePageNo][g_uchWirelessTxBuf[FreePageNo][0] -4 ] = g_PHYDevInfo.NetID&0xff;  //���·����Ϣ����·�ɱ�
        g_uchWirelessTxBuf[FreePageNo][g_uchWirelessTxBuf[FreePageNo][0] -3 ] = g_PHYDevInfo.NetID/256;   
		
    }
    
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;  //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    return FreePageNo;
}

/****************************************************************************/
/* ��������������������      �γ�����ת��֡ȷ�ϻظ�    ��    ����           */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���
FrameCmd2: �ظ���ת��֡������                                */
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int SendOkAck_RelayFrame(unsigned char* FirstDestID,unsigned char FrameCmd2,unsigned char AckFrameNo)
{   
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
          
    if(FrameCmd2 == INFO_APPLY) //������ע���ת���ظ�
    {            
              //���֡         
          g_uchWirelessTxBuf[FreePageNo][0] = OKACK_RELAYFRAME_LENGTH;   //֡����
          g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];            //��֡Ŀ��
          g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
          g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;   //��֡Դ
          g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
          
          //����Ŀ������Դ
          g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //����Ŀ��
          g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
          g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //��ʼԴ
          g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
              
          g_uchWirelessTxBuf[FreePageNo][9] = 0;      //·�ɸ���
          g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;      //֡���
          g_uchWirelessTxBuf[FreePageNo][11]= ACK;
          g_uchWirelessTxBuf[FreePageNo][12]= FrameCmd2;
              //�������    
          g_uchWirelessTxBuf[FreePageNo][13]=  g_uchRFPowerVal[g_uchRFPALev];  //�豸����
          
          g_uchWirelessTxBuf[FreePageNo][14]= g_uchDLLCurLQ;  //������·����
          
          g_uchWirelessTxBuf[FreePageNo][15]= g_uchNetLev; //���缶��
              
              //�趨Ŀ���豸ע����  
          g_uchWirelessTxBuf[FreePageNo][16]= (g_uchNetLev+1)*2; //��Ԥ���趨һ��ֵ��Ԥ�������������
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
    
    if(FrameCmd2 == INFO_DATA_STATUS)   //�����ݺ�״̬ ֡��ת���ظ�
         FreePageNo = GenerateOkAck_DataStatusFrame(FirstDestID,AckFrameNo);   
         
         
   //�ж�Ŀ���豸������
    if( (FirstDestID[0] + FirstDestID[1]*256) == g_uNetParentID )//���ϼ��豸
        g_uchRxPAtype = g_uNetParentPAtype; //�����豸����
    else
    {
        int i=0;
	
	 for(;i<NET_MAX_CAP;i++) //����ǲ������豸������
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
    
    
    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //ֱ�ӻظ�
    return   FreePageNo;
}

/****************************************************************************/
/* ��������������������      �γɾܾ����ʻظ�֡       ����        	    */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���*/
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int SendRefuseAck_Frame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{
	
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
	
	//���֡ͷ          
    g_uchWirelessTxBuf[FreePageNo][0] = REFUSEACK_FRAME_LENGTH;   //�ܾ�����֡������:0x0F
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];            //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;   //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    
    //����Ŀ������Դ
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //����Ŀ��
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //��ʼԴ
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
    
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;      //֡���
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;           //�ظ�֡������1  0x55
    g_uchWirelessTxBuf[FreePageNo][12]= REFUSE_ACK;//֡������2  �ܾ����� 0xa0
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    
       //�ж�Ŀ���豸������
    if( (FirstDestID[0] + FirstDestID[1]*256) == g_uNetParentID )//���ϼ��豸
        g_uchRxPAtype = g_uNetParentPAtype; //�����豸����
    else
    {
        int i=0;
	
	 for(;i<NET_MAX_CAP;i++) //����ǲ������豸������
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
    
    
    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //ֱ�ӻظ�����æ 
    return FreePageNo;
}


/****************************************************************************/
/* ��������������������      �γɳ���ظ�֡       ����        	           */
/*  ����������FirstDestID:�ǻظ�֡��Ŀ�ĵ�ַ ;AckFrameNo: �ǻظ������֡���*/
/*  ��������ֵ����ǰ�γɵ�֡��ŵ���ҳ��                                   */
/****************************************************************************/
int SendErroAck_Frame(unsigned char* FirstDestID,unsigned char AckFrameNo)
{
	
    //ֱ�ӻظ�֡������Ҫ�ж��ܷ����,����Ҫ����֡������ֱ���γɲ�����
    int FreePageNo = BUFF_MAXPAGE -1;//���һҳ
	
	//���֡ͷ          
    g_uchWirelessTxBuf[FreePageNo][0] = ERROACK_FRAME_LENGTH;   //�ܾ�����֡������:0x0F
    g_uchWirelessTxBuf[FreePageNo][1] = FirstDestID[0];            //��֡Ŀ��
    g_uchWirelessTxBuf[FreePageNo][2] = FirstDestID[1];
    g_uchWirelessTxBuf[FreePageNo][3] = g_PHYDevInfo.NetID&0xff;   //��֡Դ
    g_uchWirelessTxBuf[FreePageNo][4] = g_PHYDevInfo.NetID/256;
    
    //����Ŀ������Դ
    g_uchWirelessTxBuf[FreePageNo][5] = g_uchWirelessTxBuf[FreePageNo][1];               //����Ŀ��
    g_uchWirelessTxBuf[FreePageNo][6] = g_uchWirelessTxBuf[FreePageNo][2];
    g_uchWirelessTxBuf[FreePageNo][7] = g_uchWirelessTxBuf[FreePageNo][3]; //��ʼԴ
    g_uchWirelessTxBuf[FreePageNo][8] = g_uchWirelessTxBuf[FreePageNo][4]; 
    	
    g_uchWirelessTxBuf[FreePageNo][9] = 0;                //·�ɸ���
    g_uchWirelessTxBuf[FreePageNo][10] = AckFrameNo;      //֡���
    g_uchWirelessTxBuf[FreePageNo][11]= ACK;           //�ظ�֡������1  0x55
    g_uchWirelessTxBuf[FreePageNo][12]= ERROR_ACK;//֡������2  ���� 0xa1
	
    AddCRC(g_uchWirelessTxBuf[FreePageNo]);
    
    
     //�ж�Ŀ���豸������
    if( (FirstDestID[0] + FirstDestID[1]*256) == g_uNetParentID )//���ϼ��豸
        g_uchRxPAtype = g_uNetParentPAtype; //�����豸����
    else
    {
        int i=0;
	
	 for(;i<NET_MAX_CAP;i++) //����ǲ������豸������
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
    
    RFSendPacket( g_uchWirelessTxBuf[FreePageNo],g_uchWirelessTxBuf[FreePageNo][0]);    //ֱ�ӻظ�����æ 
    return   FreePageNo;
}


/****************************************************************************/
/* ��������������������     ����򵥲�ѯ        ��    ��       ��           */
/****************************************************************************/
void Process_SimpleQueryFrame()
{
	unsigned char FirstDestID[2]; //��ǰ���������ݵ� ��֡Ŀ��
	int PageNo;  //��ǰ�����͵����ݴ�ŵ�ҳ��
	
	memcpy(FirstDestID,g_uchWirelessRxBuf+3,2);
	PageNo = GenerateOkAck_SimpleQueryFrame(FirstDestID,g_uchWirelessRxBuf[10]);
	
	g_uchRxPAtype = g_uchPHYPAType;  //Ŀ���豸������
	
	RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
}

/****************************************************************************/
/* ��������������������     �����Ӳ�ѯ        ��    ��       ��           */
/****************************************************************************/
void Process_ComplexQueryFrame()
{
	unsigned char FirstDestID[2]; //��ǰ���������ݵ� ��֡Ŀ��
	int PageNo;  //��ǰ�����͵����ݴ�ŵ�ҳ��
	
	memcpy(FirstDestID,g_uchWirelessRxBuf+3,2);
	PageNo = GenerateOkAck_ComplexQueryFrame(FirstDestID,g_uchWirelessRxBuf[10]);
	
	g_uchRxPAtype = g_uchPHYPAType; //Ŀ���豸������
	
	RFSendPacket( g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
}

/****************************************************************************/
/* ��������������������     ��������ע��        ��    ��       ��           */
/* LogIntTime Ϊ����ע����豸����ע��ʱ����                              */
/****************************************************************************/
void Process_ApplyFrame(unsigned char LogIntTime)
{
	unsigned char FirstDestID[2]; //��ǰ���������ݵ� ��֡Ŀ��
	int PageNo;  //��ǰ�����͵����ݴ�ŵ�ҳ��
	int DevApplyedFlag = 0; //���������豸��ID�Ƿ��ѱ����־��
	char status;//��֤״̬
	
	memcpy(FirstDestID,g_uchWirelessRxBuf+3,2); //���δ�������С�ظ��� ��֡Ŀ�� = RxBuf �ı�֡Դ
	
	g_uchRxPAtype = g_uchWirelessRxBuf[13]; //Ŀ���豸������
	
	if((g_uchWirelessRxBuf[5]|(g_uchWirelessRxBuf[6]*256)) == g_PHYDevInfo.NetID)  //����Ŀ���Ǳ��豸 ,���豸�����Ļ�վ
        {
		if(g_uchWirelessRxBuf[9] == 0)     //·�ɸ���Ϊ0��
                {
			for(char count=0; count<NET_MAX_CAP; count++)
                        {
				if((g_uchWirelessRxBuf[7]|(g_uchWirelessRxBuf[8]*256)) == g_SubDevInfo[count].NetID)//���������豸��ID�Ƿ��ѱ���
				{
					DevApplyedFlag = 1;
					status = g_SubDevInfo[count].Status;
					g_SubDevInfo[count].DevLifeTime = 0;  //�����趨���豸���ϴ�ע�ᵽ����Ϊֹ����ȥʱ��
					
					break;
				}
                        }//end for
                        
			if(DevApplyedFlag == 1)//���������豸��ID�ѱ���,����statusֵ�ظ�
                        {
				switch(status)
				{
				case 0://0���豸��ɾ��
					break;
				//case 1://1������������
				//	break;
				case 2://2������֤��					
					PageNo = GenerateWaitCheckACK_ApplyLogFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					break;
				case 3://3����֤ʧ�ܣ�
					PageNo = GenerateCheckFailACK_ApplyFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					break;
				case 1://1������������	
				case 4://4����֤�ɹ�
					PageNo = GenerateCheckPassACK_ApplyFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					break;
				}//end switch
				return;
                        }//end if ���������豸��ID�ѱ���,����statusֵ�ظ�
			else ////���������豸��IDδ����
                        {
				if( ((g_uchDLLSubDevNumber/16)+(g_uchDLLSubDevNumber&0x0f)) >= NET_MAX_CAP) //�������豸������
				{          				        
					SendRefuseAck_Frame(FirstDestID,g_uchWirelessRxBuf[10]);
					return;
				}
				else //�������豸δ��
				{
					//���·��͵ȴ���֤
					PageNo = GenerateWaitCheckACK_ApplyLogFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					
					//���±�������ע������豸��Ϣ
					g_uchDLLSubDevNumber = ((g_uchDLLSubDevNumber/16)+1)*16 + (g_uchDLLSubDevNumber&0x0f) ; //����֤�����豸������1
				
					for(char count=0; count<NET_MAX_CAP; count++)//Ѱ�Ҵ��λ��
					{
					  if(g_SubDevInfo[count].Status == 0) //�ѵ�ǰ����֤�豸����Ϣ �����Ѿ�ɾ�����豸��λ��
                                          {
                                              g_SubDevInfo[count].Status = 2;               //����֤
                                              g_SubDevInfo[count].PAType = g_uchWirelessRxBuf[13];  //��֡Դ��������
                                              g_SubDevInfo[count].DevType = g_uchWirelessRxBuf[14]; //��֡Դ�豸���� (0~3bit:�����豸���ͣ�4~7bit:��ǰ�豸����)
                                              g_SubDevInfo[count].DevPower = g_uchWirelessRxBuf[15];//��֡Դ�豸���� 
                                              g_SubDevInfo[count].NetID = g_uchWirelessRxBuf[16] | (g_uchWirelessRxBuf[17]*256);//��֡Դ�豸NetID
                                              g_SubDevInfo[count].DevLifeTime = 0;  //���豸���ϴ�ע�ᵽ����Ϊֹ����ȥʱ��
                                              g_SubDevInfo[count].AppDataIntTime = 3;  // �趨����ע���豸�������ϱ����2����
                                              g_SubDevInfo[count].DLLLogIntTime = 3;  // �趨����ע���豸��������ע����2����
                                              break;
                                          }
                                       }//end for Ѱ�Ҵ��λ��
					return;
				}//end else �������豸δ��
                          }//end else ���������豸��IDδ����
            
                }
		else //·�ɸ�����Ϊ0
                {
			if(GetFreePageNo(0,g_uchWirelessRxBuf[0]) == -1) //�����γ���������
                        {
				//���·�������æ
				SendBusyAckFrame(FirstDestID,g_uchWirelessRxBuf[10]);
                        }
			else//���γ���������֡
                        {
				//��������Ϊ��ת����С�ظ�
				SendOkAck_RelayFrame(FirstDestID,g_uchWirelessRxBuf[11],g_uchWirelessRxBuf[10]); 
				
				//����Ķ������Ļ�վ���ԣ��Ƚ�����ע����Ϣ���͵����������յ���֤����� ���������·���
				/*
				//����Ϊ��������
				for(char count=0; count<NET_MAX_CAP; count++)
                                   {
					if((g_uchWirelessRxBuf[3]|(g_uchWirelessRxBuf[4]*256)) == g_SubDevInfo[count].NetID)//�ҵ�ƥ��ı�֤Դ�豸NetID
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
                }//end else ·�ɸ�����Ϊ0
        
         }//end if(����Ŀ���Ǳ��豸)
	else //����Ŀ�Ĳ��Ǳ��豸 ���豸����ͨ��վ
        {          	
                //��������ע����豸�Ǳ��豸�ĸ��ڵ� ��˵�����˽ṹ�б仯
                if( (g_uchWirelessRxBuf[3]|(g_uchWirelessRxBuf[4]*256)) == g_uNetParentID)
                {
                    SendRefuseAck_Frame(FirstDestID,g_uchWirelessRxBuf[10]); //ֱ�ӻظ��ܾ�����
                    g_uDLLStaUp =0;
                    return;                
                }//end if ��������ע����豸�Ǳ��豸�ĸ��ڵ�
                else if( (g_uchWirelessRxBuf[7]|(g_uchWirelessRxBuf[8]*256)) == g_uNetParentID)//��֡Դ���Ǹ��ڵ� ���ǳ�ʼԴ�Ǹ��ڵ�
	        {
                    PageNo = GenerateWaitCheckACK_ApplyLogFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
                    RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);	
                    g_uDLLStaUp =0;
                    return;          
	        }//��ʼԴ�Ǹ��ڵ�
	        
		if(g_uchWirelessRxBuf[9] == 0)//·�ɸ���Ϊ0
                {
			for(char count=0; count<NET_MAX_CAP; count++) //��Ѱ�Ƿ��Ѿ�����
                        {
				if( (g_SubDevInfo[count].Status !=0 )&& ( (g_uchWirelessRxBuf[7]|(g_uchWirelessRxBuf[8]*256)) == g_SubDevInfo[count].NetID) )//���������豸��ID�Ƿ��ѱ���
				{
					DevApplyedFlag = 1;
					status = g_SubDevInfo[count].Status;
					g_SubDevInfo[count].DevLifeTime = 0;  //�����趨���豸���ϴ�ע�ᵽ����Ϊֹ����ȥʱ��                                                                                
					break;
				}
                        }//end for ��Ѱ�Ƿ��Ѿ�����
			if(DevApplyedFlag == 1)//���������豸��ID�ѱ���,����statusֵ�ظ�
                        {
		  		switch(status)
				{
				case 0://0���豸��ɾ��
					break;
				//case 1://1������������
				//	break;
				case 2://2������֤��

					PageNo = GenerateWaitCheckACK_ApplyLogFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					
					//�γ����ϵ�ת������
			                PageNo = GenerateRelayFrame(g_uchWirelessRxBuf);
			
			                if(PageNo != -1)			
			                  DiffFreqCommunication(g_uchNetParentFreqNo,PageNo);
					break;
				case 3://3����֤ʧ�ܣ�
					PageNo = GenerateCheckFailACK_ApplyFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					break;
				case 1://1������������
				case 4://4����֤�ɹ�
					PageNo = GenerateCheckPassACK_ApplyFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
					RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
					break;
				}
				return;
                        }//end if ���������豸��ID�ѱ���,����statusֵ�ظ�
			else//��ǰû�б���
                        {
				if(((g_uchDLLSubDevNumber/16)+(g_uchDLLSubDevNumber&0x0f)) == NET_MAX_CAP) //�������豸������
				{
					//���·��;ܾ��ظ�
					SendRefuseAck_Frame(FirstDestID,g_uchWirelessRxBuf[10]);
					return;
				}
                        }//end else ��ǰû�б���
                }//end if ·�ɸ���Ϊ0
		
		/*����Ϊ·�ɸ���Ϊ0���豸��������� �Լ� ·�ɸ�����Ϊ0�����*/
		
		if(GetFreePageNo(0,g_uchWirelessRxBuf[0]) == -1)//�����γ����ϵ�ת��֡
		{
			//���·�������æ�ظ�
			SendBusyAckFrame(FirstDestID,g_uchWirelessRxBuf[10]);
		}
		else //���γ����ϵ�ת��֡
		{
			if(g_uchWirelessRxBuf[9] == 0)//·�ɸ���Ϊ0 
			{
				//���·��͵ȴ���֤
				PageNo = GenerateWaitCheckACK_ApplyLogFrame(FirstDestID,g_uchWirelessRxBuf[10],g_uchWirelessRxBuf+18);
				RFSendPacket(g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
				
				//���±�������ע������豸��Ϣ
				g_uchDLLSubDevNumber = ((g_uchDLLSubDevNumber/16)+1)*16 + (g_uchDLLSubDevNumber&0x0f) ; //����֤�����豸������1
				
				for(char count=0; count<NET_MAX_CAP; count++)//Ѱ�Ҵ��λ��
				{
				    if(g_SubDevInfo[count].Status == 0) //�ѵ�ǰ����֤�豸����Ϣ �����Ѿ�ɾ�����豸��λ��
                                    {
                                        g_SubDevInfo[count].Status = 2;               //����֤
                                        g_SubDevInfo[count].PAType = g_uchWirelessRxBuf[13];  //��֡Դ��������
                                        g_SubDevInfo[count].DevType = g_uchWirelessRxBuf[14]; //��֡Դ�豸���� (0~3bit:�����豸���ͣ�4~7bit:��ǰ�豸����)
                                        g_SubDevInfo[count].DevPower = g_uchWirelessRxBuf[15];//��֡Դ�豸����
                                        g_SubDevInfo[count].NetID = g_uchWirelessRxBuf[16] | (g_uchWirelessRxBuf[17]*256);//��֡Դ�豸NetID
                                        g_SubDevInfo[count].DevLifeTime = 0;  //���豸���ϴ�ע�ᵽ����Ϊֹ����ȥʱ��
                                        g_SubDevInfo[count].AppDataIntTime = 3;  // �趨����ע���豸�������ϱ����3����
                                        g_SubDevInfo[count].DLLLogIntTime = 3;  // �趨����ע���豸��������ע����3����
                                        break;
                                    }
                                }//end for Ѱ�Ҵ��λ��
			}//end if ·�ɸ���Ϊ0
			else
			{	//���¶�ת��������ȷ�ϻظ�
				SendOkAck_RelayFrame(FirstDestID,g_uchWirelessRxBuf[11],g_uchWirelessRxBuf[10]); 
			}
			//�γ����ϵ�ת������
			PageNo = GenerateRelayFrame(g_uchWirelessRxBuf);
			
			if(PageNo != -1)
                  {  
                     for(int times=0;times<3;times++)//����֡
                     {   
                          if( (DiffFreqCommunication(g_uchNetParentFreqNo,PageNo)) ==0 )
                                break;
                     }	
                  }//end if pageNo!=-1
                  
		}//end else ���γ����ϵ�ת��֡		
      } // end else ����Ŀ�Ĳ��Ǳ��豸
}

/****************************************************************************/
/* ��������������������     ����������ע��            ��       ��           */
/****************************************************************************/
void Process_DllLogFrame(void)
{
        unsigned char FirstDestID[2]; //��ǰ���������ݵ� ��֡Ŀ��
	int PageNo;  //��ǰ�����͵����ݴ�ŵ�ҳ��
	
	int flag = 0;	
	
	 for(int i=0;i<NET_MAX_CAP;i++) //����ǲ������豸������ע����Ϣ
	 {	    
	        if( ( (g_SubDevInfo[i].Status == 1) ||(g_SubDevInfo[i].Status == 2) ||(g_SubDevInfo[i].Status == 4) )
	            && (g_SubDevInfo[i].NetID == (g_uchWirelessRxBuf[3] + g_uchWirelessRxBuf[4]*256)) )
		{
		    flag = 1;
		    g_uchRxPAtype = g_SubDevInfo[i].PAType; //�趨�����豸�Ĺ�������    
		    break;
		}
         }
	
	memcpy(FirstDestID,g_uchWirelessRxBuf+3,2);
	
	if(flag) //�����豸������ע����Ϣ
	{		    	    
	    PageNo = GenerateOkAck_DllLogFrame(FirstDestID,g_uchWirelessRxBuf[10]);
	    RFSendPacket( g_uchWirelessTxBuf[PageNo],g_uchWirelessTxBuf[PageNo][0]);
	}
	else// �������豸������ע����Ϣ
	{
	    g_uchRxPAtype = g_uchPHYPAType; //�趨�����豸�Ĺ�������
	    SendRefuseAck_Frame(FirstDestID,g_uchWirelessRxBuf[10]);
	}	
}

/****************************************************************************/
/* ��������������������     ���� ���ݺ�״̬֡         ��       ��           */
/****************************************************************************/
void Process_DataStatusFrame(unsigned char *Router,unsigned char LogIntTime)
{
    int PageNo;//��ǰ�����͵����ݴ�ŵ�ҳ��
    unsigned char FirstDestID[2]; //��ǰ���������ݵ� ��֡Ŀ��
    //unsigned char uchFreNum;
    int flag = -1;//���·�������֡���豸�ڵ�ǰ��������豸��λ��
    
    memcpy(FirstDestID,g_uchWirelessRxBuf+3,2); //���δ�������С�ظ��� ��֡Ŀ�� = RxBuf �ı�֡Դ
    
    for(char count=0; count<NET_MAX_CAP; count++)//�鿴�Ƿ����Լ������豸��������Ϣ
    {
        if( ( g_SubDevInfo[count].Status !=0) &&  ( (g_uchWirelessRxBuf[3]+(g_uchWirelessRxBuf[4]*256)) == g_SubDevInfo[count].NetID) )//�ҵ�ƥ��ı�֤Դ�豸NetID
        {
                flag = count;
                g_uchRxPAtype = g_SubDevInfo[count].PAType; //�趨�����豸�Ĺ�������                  
                break;
        }
    }
        
    if(flag == -1) //�����Լ������豸
    {
        SendRefuseAck_Frame(FirstDestID,g_uchWirelessRxBuf[10]);   
        return;
    } 
    
    if((g_uchWirelessRxBuf[5]|(g_uchWirelessRxBuf[6]*256)) == g_PHYDevInfo.NetID)  //����Ŀ���Ǳ��豸
    {
        if(-1 == GetFreePageNo(0,DATA_STATUS_FEEDBACK_FRAME_BASE_LENGTH+2*g_uchWirelessRxBuf[9])) //�ܷ��γ��ܷ���֡
	{
		//���»ظ�����æ
	    SendBusyAckFrame(FirstDestID,g_uchWirelessRxBuf[10]);
	} //end if �ܷ��γ��ܷ���֡
        else
	{
            //����Ϊ������ת��֡��С�ظ� 
            SendOkAck_RelayFrame(FirstDestID,g_uchWirelessRxBuf[11],g_uchWirelessRxBuf[10]);
            
            //���µ��ܷ�����Ҫ���Ļ�վ���͸����������յ�ȷ�Ϻ󣬲��γɵ�
            /*
            //����Ϊ�γɲ������켶ͨѶ�㷨���ʹ�ظ�
            for(char count=0; count<NET_MAX_CAP; count++)
	    {
		if((g_uchWirelessRxBuf[3]|(g_uchWirelessRxBuf[4]*256)) == g_SubDevInfo[count].NetID)//�ҵ�ƥ��ı�֤Դ�豸NetID
		{
                    uchFreNum = g_SubDevInfo[count].SubFreNum;
                    break;
		}
	    }//end for 
	    PageNo = GenerateDataFeedbackFrame(FirstDestID,g_uchWirelessRxBuf+7,Router,LogIntTime); 
	    DiffFreqCommunication(uchFreNum,PageNo); 
	    */
	}//end else
	
	//��Ҫ������֡�д������豸�����Ϣ����
        g_SubDevInfo[flag].SubFreNum = g_uchWirelessRxBuf[16]; //���豸�Ĺ���Ƶ��
        g_SubDevInfo[flag].Status = 1;               //��������
        g_SubDevInfo[flag].PAType = g_uchWirelessRxBuf[14];  //��������
        g_SubDevInfo[flag].DevType = g_uchWirelessRxBuf[15]; //�豸���� (0~3bit:�����豸���ͣ�4~7bit:��ǰ�豸����)
        g_SubDevInfo[flag].DevPower = g_uchWirelessRxBuf[17];//��֡Դ�豸����
        g_SubDevInfo[flag].DevLifeTime = 0;  //���豸���ϴ�ע�ᵽ����Ϊֹ����ȥʱ��
      
    }//end if ����Ŀ���Ǳ��豸
    else//����Ŀ�Ĳ��Ǳ��豸
    {
        if(-1 != GetFreePageNo(0,g_uchWirelessRxBuf[0]+2))//�ܷ��γ����ϵ�ת��֡
	{
            //����Ϊ��ת��֡��ȷ�ϻظ�
            SendOkAck_RelayFrame(FirstDestID,g_uchWirelessRxBuf[11],g_uchWirelessRxBuf[10]);
            
            //�����豸��Ϣ��¼
            g_SubDevInfo[flag].Status = 1;        //��������
            g_SubDevInfo[flag].DevLifeTime = 0;  //���豸���ϴ�ע�ᵽ����Ϊֹ����ȥʱ��  
            
            if(g_uchWirelessRxBuf[9] == 0) //·�ɸ���Ϊ0  ������Ϣ��û����ת���� ���Լ�ֱ���¼��豸�ϴ���Ϣ
            {//��¼ֱ���¼�����Ϣ
                  g_SubDevInfo[flag].SubFreNum = g_uchWirelessRxBuf[16]; //���豸�Ĺ���Ƶ��
                  g_SubDevInfo[flag].PAType = g_uchWirelessRxBuf[14];  //��������
                  g_SubDevInfo[flag].DevType = g_uchWirelessRxBuf[15]; //�豸���� (0~3bit:�����豸���ͣ�4~7bit:��ǰ�豸����)
                  g_SubDevInfo[flag].DevPower = g_uchWirelessRxBuf[17];//��֡Դ�豸����
            } 
            
            //����Ϊ�γ����ϵ�ת��֡������
            PageNo = GenerateRelayFrame(g_uchWirelessRxBuf);
            
            if(PageNo != -1)
            {  
                     for(int times=0;times<3;times++)//����֡
                     {   
                          if( (DiffFreqCommunication(g_uchNetParentFreqNo,PageNo)) ==0 )
                                break;
                     }	
            }//end if pageNo!=-1

	}//end if �ܷ��γ����ϵ�ת��֡
        else
	{
	     //����Ϊ����æ�ظ�
            SendBusyAckFrame(FirstDestID,g_uchWirelessRxBuf[10]);
	}
    }//end else ����Ŀ�Ĳ��Ǳ��豸
}

/****************************************************************************/
/* ��������������������      �����������֡    ��  0x60  ����        	    */
/*  pFrame �ǽ��յ����������֡                                             */
/****************************************************************************/
void  Process_CmdFrame(unsigned char *pFrame)
{
    int FreePageNo;

//1 ���������豸ʱ        Ŀ���豸ΪFFFF  Ŀ���豸�ϼ�ΪFFFF ����������ֻ��Ҫ�������ת����·�ɸ���ʼ������Ϊ0
//2 �������л�վʱ        Ŀ���豸Ϊ0000  Ŀ���豸�ϼ�ΪFFFF ����������ֻ��Ҫ�������ת����·�ɸ���ʼ������Ϊ0
//3 ����ĳ��վ�������豸  Ŀ���豸ΪFFFF  Ŀ���豸�ϼ�ָ��   �����������·�����·��ͣ�·�ɸ�������
//4 �����ض��豸          Ŀ���豸ָ��    Ŀ���豸�ϼ�ָ��   �����������·�����·��ͣ�·�ɸ�������
//��ǰֻʵ������ĵ�4��
    g_uDLLStaUp = DLL_MAX_STAUP ; //������·ϵ����ֵ���

    if( pFrame[9]>1)   //·�ɸ�������1������֡��Ҫת�� 
    {
        FreePageNo=GenerateRelayFrame(pFrame);  //�γ�ת��֡
		
	if ( FreePageNo<0)  //���������������γ�ת��֡���ظ�����æ
	    SendBusyAckFrame(pFrame+3,pFrame[10]);                //ֱ�ӻظ�����æ
	else  //�ܹ��γ�ת��֡
	{
	    SendOkAck_CmdFrame(pFrame+3,pFrame[10]);       //���ͻظ�֡
			
	    //Ѱ�����豸Ƶ��
	    int DestFreqNo = -1;
	    for(int i=0;i<NET_MAX_CAP;i++)//Ѱ�����豸Ƶ��
	    {
		if ( (g_SubDevInfo[i].Status == 1 ) && ( g_SubDevInfo[i].NetID == (pFrame[pFrame[0]-6]+pFrame[pFrame[0]-5]*256 )))
		{
		    DestFreqNo = g_SubDevInfo[i].SubFreNum;
		    g_uchRxPAtype = g_SubDevInfo[i].PAType;
		    break;
		}
	    }//end for Ѱ�����豸Ƶ��          
          
          if(DestFreqNo != -1)
          {  
                     for(int times=0;times<3;times++)//����֡
                     {   
                          //�����켶�ŵ�ͨѶ�㷨
                          if( (DiffFreqCommunication(DestFreqNo,FreePageNo)) ==0 )
                                break;
                     }	
          }//end if DestFreqNo!=-1
	
        }//end else �ܹ��γ�ת��֡
    }//end if ·�ɸ�������1����������Ҫת�� 
    
    else if( pFrame[9] == 1)  //·�ɸ�������1���Լ�����Ҫ���ƵĽڵ���ϼ�
    {
        //��ʱֻ����������Ϣ �����·���
        SendOkAck_CmdFrame(pFrame+3,pFrame[10]);       //���ͻظ�֡
        
        //���ע����ʱ���ж� �����ϱ���� ���ܳ���3Сʱ
        unsigned char time = pFrame[18];
        
        if ( (time >=180 ) || (time ==0) )
          time = 180; 
        
        
       if( (pFrame[15]==0xff) && ( pFrame[16]==0xff) ) //��վ�����е��豸
	{  
	    for(int i=0;i<NET_MAX_CAP;i++)
		g_SubDevInfo[i].AppDataIntTime = time;//pFrame[18];
	}
	
	else//�ض��豸
	{	
	  int DestFreqNo = -1;
	  for(int i=0;i<NET_MAX_CAP;i++)//Ѱ�����豸Ƶ��
	  {
		if ( (g_SubDevInfo[i].Status == 1 ) && ( g_SubDevInfo[i].NetID == (pFrame[5]+pFrame[6]*256 )))
		{
			g_SubDevInfo[i].AppDataIntTime = time;
			DestFreqNo = g_SubDevInfo[i].SubFreNum;
			g_uchRxPAtype = g_SubDevInfo[i].PAType;
			break;
		}
	  }//end for Ѱ�����豸Ƶ��
	  
	  FreePageNo=GenerateRelayFrame(pFrame);  //�γ�ת��֡
					
	  //���´���
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
         }//end if ���´���
	        
	  }//end else �ض��豸
        
        /*
	if( pFrame[15]==0xff && pFrame[16]==0xff ) //
	{
	    
	    
	    for(int i=0;i<NET_MAX_CAP;i++)
		g_SubDevInfo[i].LogIntTime=pFrame[18];
	}
	
	else
	{
            FreePageNo=GenerateRelayFrame(pFrame);  //�γ�ת��֡
			
            if ( FreePageNo<0)  //���������������γ�ת��֡���ظ�����æ
				SendBusyAckFrame(pFrame+3,pFrame[10]);                //ֱ�ӻظ�����æ
            else                //�ܹ��γ�ת��֡
            {
				SendOkAck_CmdFrame(pFrame+3,pFrame[10]);       //���ͻظ�֡
				
				//Ѱ�����豸Ƶ��
				int DestFreqNo=0xff;
				for(int i=0;i<NET_MAX_CAP;i++)
				{
                    if ( g_SubDevInfo[i].NetID == (pFrame[pFrame[0]-6]+pFrame[pFrame[0]-5]*256 ))
                    {
						DestFreqNo=g_SubDevInfo[i].SubFreNum;
						break;
                    }
				}
				//�����켶�ŵ�ͨѶ�㷨
				DiffFreqCommunication(DestFreqNo,FreePageNo);
            }
			
			
	}
	*/
		
    }
    
    if( pFrame[9]==0)        //��������֡������Ŀ�����Լ�
    {
	//���ͻظ���Ϣ
	SendOkAck_CmdFrame(pFrame+3,pFrame[10]);       //�γɻظ�֡
	if( pFrame[12]==0xa3 )          //��������
	{
	    //���ע����ʱ���ж�
	    if( (pFrame[18] <= 254)  && (pFrame[18] >0) )
	        g_uAppDataInt=pFrame[18]*60;
	    else
	        g_uAppDataInt=21600;//��Сʱ
	        
            g_uchNetLev = g_uchWirelessRxBuf[17] + 1;//�������� = �ϼ����� + 1 
	}
	if( pFrame[12]==0xa5 )          //��ͣ����
	{
	    //���ע����ʱ���ж�
	    if( (pFrame[18] <= 254)  && (pFrame[18] >0) ) 
	        g_uAppDataInt=pFrame[18]*60;
	    else
	        g_uAppDataInt=21600;//��Сʱ
	        
	    g_uchNetStatus |= Parent_Hold;//���� Parent_Hold
	} 
	if(pFrame[12]==0xa6 )          //������˹�ϵ
	{
	    g_uDLLStaUp = 0;	
	}	
    }
}

/****************************************************************************/
/* ��������������������      ������֡    ��  0x50 ����        	    */
/*  pFrame �ǽ��յ��ķ���֡                                                 */
/****************************************************************************/
void  Process_FeedbackFrame(unsigned char *pFrame)
{
    int FreePageNo;
    
    g_uDLLStaUp = DLL_MAX_STAUP ; //������·ϵ����ֵ���
    
    if( (pFrame[12]== CHECK_PASS_ACK) ||  (pFrame[12]== CHECK_FAIL_ACK) )  //������ע�ᷴ��֡
    {
        if( pFrame[9]>1)   //·�ɸ�������1������ע�ᷴ��֡��Ҫת�� 
        {
	    FreePageNo=GenerateRelayFrame(pFrame);  //�γ�ת��֡
			
	    if ( FreePageNo<0)  //���������������γ�ת��֡���ظ�����æ
		SendBusyAckFrame(pFrame+3,pFrame[10]);                //ֱ�ӻظ�����æ
	    else  //�ܹ��γ�ת��֡
	    {
		SendOkAck_FeedbackFrame(pFrame+3,pFrame[10]);  //�ظ�����֡    
            
            //�жϷ���֡ ������������Ŀ�ĺ�֡����Ƿ���Լ�֡�����л�ѹ�� ��Ҫת����֡����ʼԴ��֡���һֱ
            //���һֱ����ɾ�������֡
            //�˶δ�������Ϊ���������������վ���ͻ���֡ʱ���ϼ��豸�ظ�ȷ��֡����վû���յ�����ʱ������֡��֡�����л�ѹ
            //���Ǵ��ϼ��豸�Ѿ��ظ����ܷ����ˡ�
            for(int i = 0;i<BUFF_MAXPAGE;i++)//�����û����Ҫ���͵�֡
            {
                   if (( g_uchWirelessTxBufCtl[i] & 0x90) == 0x90) 
                   {
                      if( (pFrame[5] == g_uchWirelessTxBuf[i][7]) && (pFrame[6] == g_uchWirelessTxBuf[i][8]) &&//����֡����Ŀ�� �ͻ���֡��ʼԴ��ͬ
                          (pFrame[10] == g_uchWirelessTxBuf[i][10]))//����֡֡��� �ͻ���֡ ֡�����ͬ
                      {
                          g_uchWirelessTxBufCtl[i] = 0;//ɾ����֡
                      }
                   }
            }//end for �����û����Ҫ���͵�֡
            
		
		//Ѱ�����豸Ƶ��
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
		//�����켶�ŵ�ͨѶ�㷨
		if(DestFreqNo != -1)
            {
                for(unsigned char ch  = 0; ch<3; ch++)
                {
                    if(DiffFreqCommunication(DestFreqNo,FreePageNo)==0)
                      break;
                }
            }
		  
	    }//end else �ܹ��γ�ת��֡
        }//end if ·�ɸ�������1������ע�ᷴ��֡��Ҫת�� 
        
        else if( pFrame[9]==1)  //·�ɸ�������1��������֤���
        {
	    unsigned char Location = 0xff;//���������豸��Ϣ���е�λ����Ϣ
	    
	    SendOkAck_FeedbackFrame(pFrame+3,pFrame[10]);  //�ظ�����֡  
	    
	    for(int i=0;i<NET_MAX_CAP;i++)//�鿴�Ƿ��м�¼
	    {
	        if( g_SubDevInfo[i].NetID == (pFrame[13]+pFrame[14]*256) )
		{
		    Location = i;
                    break;
		}
            }//end for �鿴�Ƿ��м�¼
            
            if( Location == 0xff ) //��ǰ�޼�¼ ���¼����
            {
                for(int i=0;i<NET_MAX_CAP;i++)//Ѱ�ҿ�λ
                {
                    if(g_SubDevInfo[i].Status == 0)
                    {
                        g_SubDevInfo[i].NetID = (pFrame[13]+pFrame[14]*256);
                        Location = i;
                        break;
                    }
                }//end for Ѱ�ҿ�λ
            }//end if ��ǰ�޼�¼ ���¼����  
            
            if(Location != 0xff ) //��Ϣ����¼����
            {
                  if(pFrame[12] == CHECK_PASS_ACK) //��֤�ɹ�
                  {                
                      g_SubDevInfo[Location].Status = 4;   
                      
                      //�������ע����ʱ���ж�
                      
                      if( (pFrame[23] <= 180 )  && (pFrame[23]>0))                     
                          g_SubDevInfo[Location].AppDataIntTime = pFrame[23];//�趨Ӧ�ò������ϱ����ʱ��  
                      else
                          g_SubDevInfo[Location].AppDataIntTime = 180; //3Сʱ
                      
                      if( (pFrame[24] <= 60) && (pFrame[24]>0))     
                          g_SubDevInfo[Location].DLLLogIntTime = pFrame[24];//�趨��·��ע����ʱ�� 
                      else
                          g_SubDevInfo[Location].DLLLogIntTime = 60;  //60����
                                       
                  }
                  else if(pFrame[12]==CHECK_FAIL_ACK) //��֤ʧ��
                  {
                      g_SubDevInfo[Location].Status = 3; 
                      g_SubDevInfo[Location].AppDataIntTime = 1;//�趨Ӧ�ò�ע����ʱ�� 1���� 
                  }
            }//end if ��Ϣ����¼����
            
            unsigned char high,low;
            high = low = 0;
            
            for(int i=0;i<NET_MAX_CAP;i++)//�������豸������Ϣ
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
            }//end for �������豸������Ϣ
            
            g_uchDLLSubDevNumber = (high&0x0f)*16 + (low&0x0f);  
		    
        }//end else ·�ɸ�������1��������֤���
        
    }//end if ������ע�ᷴ��֡
    
    if( pFrame[12]== INFO_DATA_STATUS )  //������ ��״̬ ��Ϣ����֡
    {		
        if( pFrame[9]>0)   //·�ɸ�������0��������Ϣ����֡��Ҫת�� 
        {
            FreePageNo=GenerateRelayFrame(pFrame);  //�γ�ת��֡
                            
            if ( FreePageNo<0)  //���������������γ�ת��֡���ظ�����æ
                SendBusyAckFrame(pFrame+3,pFrame[10]);                //ֱ�ӻظ�����æ
            else  //�ܹ��γ�ת��֡
            {
                SendOkAck_FeedbackFrame(pFrame+3,pFrame[10]);  //�ظ�����֡     
                
                //�жϷ���֡ ������������Ŀ�ĺ�֡����Ƿ���Լ�֡�����л�ѹ�� ��Ҫת����֡����ʼԴ��֡���һֱ
                //���һֱ����ɾ�������֡
                //�˶δ�������Ϊ���������������վ���ͻ���֡ʱ���ϼ��豸�ظ�ȷ��֡����վû���յ�����ʱ������֡��֡�����л�ѹ
                //���Ǵ��ϼ��豸�Ѿ��ظ����ܷ����ˡ�
                for(int i = 0;i<BUFF_MAXPAGE;i++)//�����û����Ҫ���͵�֡
                {
                       if (( g_uchWirelessTxBufCtl[i] & 0x90) == 0x90) 
                       {
                          if( (pFrame[5] == g_uchWirelessTxBuf[i][7]) && (pFrame[6] == g_uchWirelessTxBuf[i][8]) &&//����֡����Ŀ�� �ͻ���֡��ʼԴ��ͬ
                              (pFrame[10] == g_uchWirelessTxBuf[i][10]))//����֡֡��� �ͻ���֡ ֡�����ͬ
                          {
                              g_uchWirelessTxBufCtl[i] = 0;//ɾ����֡
                          }
                       }
                }//end for �����û����Ҫ���͵�֡ 
                
		//����Ŀ���豸��Ƶ��
		int DestFreqNo = -1;
		
		if(pFrame[9]>1) //����Ŀ�Ĳ��Ǳ�����������豸
		{
                    for(int i=0;i<NET_MAX_CAP;i++)//ȷ����һ��·�����Լ������豸,��ȷ�����豸Ƶ��
                    {
                        if ( (g_SubDevInfo[i].Status == 1 ) && (g_SubDevInfo[i].NetID == (pFrame[pFrame[0]-6] + pFrame[pFrame[0]-5]*256 )))
                        {
                            DestFreqNo = g_SubDevInfo[i].SubFreNum;
                            g_uchRxPAtype = g_SubDevInfo[i].PAType;
                            break;
                        }
                    }//end for Ѱ�����豸Ƶ��
                }//end if ����Ŀ�Ĳ��Ǳ�����������豸
                else//����Ŀ���Ǳ�����������豸
                {
                   for(int i=0;i<NET_MAX_CAP;i++)//Ѱ�����豸Ƶ��
                   {
                        if ( (g_SubDevInfo[i].Status == 1 ) &&  (g_SubDevInfo[i].NetID == (pFrame[5] + pFrame[6]*256 )))
                        {
                            DestFreqNo = g_SubDevInfo[i].SubFreNum;
                            g_uchRxPAtype = g_SubDevInfo[i].PAType;
                            break;
                        }
                    }//end for Ѱ�����豸Ƶ��
                }
		
              if(DestFreqNo != -1)	
              {
                  for(unsigned char ch =0; ch<3;ch++)
                  {
                      if( g_uchRFSelfFreNum== DestFreqNo)//ͬһ��Ƶ��
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
	    }//end else �ܹ��γ�ת��֡
	} //end if ·�ɸ�������0��������Ϣ����֡��Ҫת�� 
		
        else if( pFrame[9]==0)  //·�ɸ�������0,������Ϣ���������յ�
        {
	    SendOkAck_FeedbackFrame(pFrame+3,pFrame[10]);  //�ظ�����֡ 
	    for(int i=0;i<5;i++)
		  if( ( g_uchWirelessTxBufCtl[i] & 0x40 ) == 0x40 )
                      g_uchWirelessTxBufCtl[i]=0;           //���֡��������ַ�����־

            //���ע����ʱ���ж�
	    if ( (pFrame[13] <= 254)  && (pFrame[13] > 0) )
	        g_uAppDataInt=pFrame[13]*60; //���������ϱ����ʱ��
	    else
	        g_uAppDataInt=21600;//��Сʱ
          
        }//end else if ·�ɸ�������0,������Ϣ���������յ�
    }//end if ��������Ϣ����֡
}

