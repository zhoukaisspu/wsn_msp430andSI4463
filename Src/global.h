#ifndef _GLOBAL
#define _GLOBAL 

//���屾�豸��ǰ����̬��
typedef enum { Status_TBD,                //0 ����
Power_Full,        	//1 ����̬
Power_Normal,           //2 ��̬
Power_Short,            //3 ȱʧ̬
Power_Protect}_PowerStatus ;      //4 ����̬


//�����豸����
typedef enum  {	 DevTypeTBD,  //0 δ����
Server,                       //1 ������
CBC,                          //2 ���Ļ�վ������
CenterBase,		      //3 ���Ļ�վ  
Base,			      //4 ��վ   
Term1,	        	      //5�����߶˻������ȿ�����վ�ֿ����˻����豸��ʼ��Ϊ�˻�ʱΪ�����߶˻���
Term2,	        	      //6�������߶˻���
Monitor,		      //7 ���ڵ�
Base_or_Term}_DevType;        //8 ��վ���߶˻�

//���屾�豸��ǰ������������
typedef enum  {  Type_TBD,	        //0x00 ����
Bat1_Norm,              		//0x01  1��һ���Ե�� 
Bat2_Norm, 	        		//0x02 	2��һ���Ե�� 
NiHi3_220_N,            		//0x03 	220V��3������
NiHi3_220_Y, 				//0x04 	220V��3������+ ���ε��
NiHi4_220_N,            		//0x05  220V��4������
NiHi4_220_Y, 				//0x06  220V��4������+ ���ε��
LABat4_220_N,            		//0x07 	220V��Ǧ����4
LABat4_220_Y, 				//0x08 	220V��Ǧ����4+ ���ε��
NiHi3_Solar_N,         	 	        //0x09	̫���ܣ�3������
NiHi3_Solar_Y, 				//0x0a  ̫���ܣ�3������+ ���ε��
NiHi4_Solar_N,  			//0x0b  ̫���ܣ�4������
NiHi4_Solar_Y, 				//0x0c  ̫���ܣ�4������+ ���ε��
LABat4_Solar_N,  			//0x0d  ̫���ܣ�Ǧ����4
LABat4_Solar_Y, 			//0x0e  ̫���ܣ�Ǧ����4+ ���ε��
FaraCap_Solar_N,			//0x0f  ̫���ܣ���������
FaraCap_Solar_Y,        		//0x10  ̫���ܣ���������+ ���ε��
NiHi1_220_N,            		//0x11  220V��1������
NiHi1_220_Y, 				//0x12	220V��1������+ ���ε��
NiHi2_220_N,            		//0x13  220V��2������
NiHi2_220_Y, 				//0x14  220V��2������+ ���ε��
NiHi1_Solar_N,          		//0x15  ̫���ܣ�1������
NiHi1_Solar_Y, 	        		//0x16	̫���ܣ�1������+���ε��
NiHi2_Solar_N,          		//0x17  ̫���ܣ�2������
NiHi2_Solar_Y 	        		//0x18  ̫���ܣ�2������+���ε��
}_PowerType;


/***********************************************/
/*          ��Ҫ��д��FLASH��Ϣ�洢��������    */
/***********************************************/
typedef struct {
	union {
		unsigned int uIntegratorID;   
		unsigned char ucIntegratorID [2];
	} IntegratorID;           //�豸������ID
	union {
		unsigned int uProjectID;
		unsigned char ucProjectID [2];
	} ProjectID;              //�豸����ID
	
	//���屾�豸��ǰ������������
	_PowerType PowerType;
	
	//�����豸����
	_DevType DevType;
	
	unsigned int NetID;        //����ID
	unsigned char DevMAC[8];    //�豸�����ַ
} _DevInfo;

extern _DevInfo g_PHYDevInfo;
extern _PowerStatus g_eAppPowerStatus;

//��վ��ǰע������豸���� ��4��bit��bit3~bit0����
//����ʽ����������豸�� ǰ�ĸ�bit����δ��ʽ����������豸
extern unsigned char g_uchDLLSubDevNumber; 

#define NET_MAX_CAP  12             //�����������

//��վҪ������¼��豸��Ϣ���� 
typedef struct{
	unsigned char Status; //ע��״̬��0: �豸��ɾ����1������������2������֤��3����֤ʧ�ܣ�4����֤�ɹ�
	unsigned char DevType;     //�豸���� (0~3bit:�����豸���ͣ�4~7bit:��ǰ�豸����)
	unsigned int NetID;        //�豸����ID
	unsigned char PAType;      //��������
	unsigned char PowerType;   //��Դ����
	unsigned char DevPower;    //�豸����
	unsigned char AppDataIntTime;  //Ӧ�ò����ݼ��ʱ��
	unsigned char DLLLogIntTime;  //������·��ע����ʱ��
	unsigned char PowerStatus; //����״̬
	unsigned char SubFreNum;     //����Ƶ��   //g_uchNetSubBFreNum[NET_MAX_CAP/2]
	unsigned char SubDevNum;   //���豸���� ��4��bit��bit3~bit0��������ʽ����������豸�� ǰ�ĸ�bit����δ��ʽ����������豸
	unsigned char DevLifeTime;   //��վ��������豸���ϴ�ע�ᵽ����Ϊֹ����ȥʱ��
	unsigned char LatestFrameNo; //���豸���͵����һ֡���ݵ�֡���
} _SubInfo;

extern _SubInfo  g_SubDevInfo[NET_MAX_CAP];


//����״̬�궨��
#define Parent_Lost         0x01  //���ϼ�����Ҫ��Ѱ���޼�������
#define Parent_Lost_Grade   0x02  //���ϼ�����Ҫ��Ѱ���м�������
#define Parent_Hold         0x04  //�ϼ�æ������Ѱ��
#define Child_Lost          0x08  //�¼�ͨѶ��Ҫȷ������Ƶ��

extern unsigned char g_uchNetStatus;          //����״̬        

extern unsigned char g_uchNetLev;            //�����ȫ�ֱ������������缶��

extern unsigned int g_uNetParentID ;       //���ڻ�վ�Ͷ˻����ԣ������ϼ��豸������ID
extern unsigned char g_uchNetParentFreqNo ; // �����ϼ��豸��Ƶ����
extern unsigned char g_uNetParentPAtype;  //��¼�ϼ��豸�Ĺ�������




extern unsigned int g_uAppDataInt ;  //Ӧ�ò������ϱ����ʱ�� ����Ϊ��λ 



// �D�D�D�DӦ�ò�ȫ�ֱ������趨�豸��ǰ�Ľ�ɫ��
//1�ֽڱ�ʾ	��־���豸���ܽ�ɫ
//0 δ����
//1 ������
//2 ���Ļ�վ������
//3 ���Ļ�վ  
//4 ��վ   
//5�����߶˻������ȿ�����վ�ֿ����˻����豸��ʼ��Ϊ�˻�ʱΪ�����߶˻���
//6�������߶˻���
//7 ���ڵ�
//8 ��վ���߶˻�

extern unsigned char g_uchAppchRoleSet; //��ʼ��Ϊδ֪ 

//��־�ڵ�Ĵ���������
//2�ֽڱ�ʾ	��־���豸����������
//0x00 00 	δ֪
//0x00 01	��������ҵ��
//0x00 02	������ͼ��ҵ��
//0x00 03	���ٶ�
//0x00 04	���ն�
//0x00 05	��ʪ��
//0x00 06	����
//0x00 07	�����豸�������ܶȴ�����	
//0x00 08       430Ƭ���¶ȴ�����	
//0x00 09     ������̼
//0x00 0A     ���ٷ���	
//0x00 0B     ����

extern unsigned int g_uAppDataType; 


extern const unsigned int g_uAppFaraCapFull;  //�D�D�D�D�������ݱ���̬����ֵ��
extern const unsigned int g_uAppFaraCapNormal;// �D�D�D�D�������ݳ�̬����ֵ��
extern const unsigned int g_uAppFaraCapShort; //�D�D�D�D��������ȱʧ̬��ѹ����ֵ��

extern const unsigned int g_uApp3NiHiFull;   //�D�D�D3�������ر���̬����ֵ
extern const unsigned int g_uApp3NiHiNormal; //�D�D�D3�������س�̬����ֵ
extern const unsigned int g_uApp3NiHiShort;  //�D�D�D3��������ȱʧ̬����ֵ

extern const unsigned int g_uApp4NiHiFull;   //�D�D�D4�������ر���̬����ֵ
extern const unsigned int g_uApp4NiHiNormal; //�D�D�D4�������س�̬����ֵ
extern const unsigned int g_uApp4NiHiShort;  //�D�D�D4��������ȱʧ̬����ֵ

extern const unsigned int g_uchApp1NormFull;   //�D�D�D1��һ���Ե�ر���̬����ֵ
extern const unsigned int g_uchApp1NormNormal ; //�D�D�D1��һ���Ե�س�̬����ֵ
extern const unsigned int g_uchApp1NormShort ;  //�D�D�D1��һ���Ե��ȱʧ̬����ֵ

extern const unsigned int g_uchApp2NormFull ;   //�D�D�D2��һ���Ե�ر���̬����ֵ
extern const unsigned int g_uchApp2NormNormal ; //�D�D�D2��һ���Ե�س�̬����ֵ
extern const unsigned int g_uchApp2NormShort ;  //�D�D�D2��һ���Ե��ȱʧ̬����ֵ

extern const unsigned int g_uApp1NiHiFull ;   //�D�D�D1�������ر���̬����ֵ
extern const unsigned int g_uApp1NiHiNormal ; //�D�D�D1�������س�̬����ֵ
extern const unsigned int g_uApp1NiHiShort ;  //�D�D�D1��������ȱʧ̬����ֵ

extern const unsigned int g_uApp2NiHiFull ;   //�D�D�D2�������ر���̬����ֵ
extern const unsigned int g_uApp2NiHiNormal ; //�D�D�D2�������س�̬����ֵ
extern const unsigned int g_uApp2NiHiShort ;  //�D�D�D2��������ȱʧ̬����ֵ

extern const unsigned int g_uAppLABat4Full;   //�D�D�D4VǦ���ر���̬����ֵ
extern const unsigned int g_uAppLABat4Normal ; //�D�D�D4VǦ���س�̬����ֵ
extern const unsigned int g_uAppLABat4Short ;  //�D�D�D4VǦ����ȱʧ̬����ֵ

extern unsigned char g_uchConfigFlag;         //����״̬��־

extern unsigned char g_uPicControlFlag;        //��ͼ�񴫸������Ϳ�������


#endif 