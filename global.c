#include "global.h"

_PowerStatus g_eAppPowerStatus;  
_DevType g_eAppDevType;    
_DevInfo g_PHYDevInfo;


//��վ��ǰע������豸���� ��4��bit��bit3~bit0����
//����ʽ����������豸�� ǰ�ĸ�bit����δ��ʽ����������豸
unsigned char g_uchDLLSubDevNumber = 0; 

_SubInfo g_SubDevInfo[NET_MAX_CAP]; //�������豸����Ϣ  

unsigned char g_uchNetStatus;                 //����״̬

unsigned char g_uchNetLev = 0xFF;                    //�����ȫ�ֱ������������缶��

unsigned int g_uNetParentID = 0;              //���ڻ�վ�Ͷ˻����ԣ������ϼ��豸������ID
unsigned char g_uchNetParentFreqNo = 0xff;       //�����ϼ��豸��Ƶ����
unsigned char g_uNetParentPAtype = 0;    //��¼�ϼ��豸�Ĺ�������

unsigned int g_uAppDataInt = 600;             //������Ӧ�ò������ϱ����ʱ�� ����Ϊ��λ 


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

unsigned char g_uchAppchRoleSet = 0x00; //��ʼ��Ϊδ֪ 

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
unsigned int g_uAppDataType = 0; 


//���ռ��ֵ��ʵ��ֵ 1��3��������
const unsigned int g_uAppFaraCapFull =0x0A3D ;  //�D�D�D�D�������ݱ���̬����ֵ��
const unsigned int g_uAppFaraCapNormal =0x92C ;// �D�D�D�D�������ݳ�̬����ֵ��
const unsigned int g_uAppFaraCapShort = 0x69D; //�D�D�D�D��������ȱʧ̬��ѹ����ֵ��

//���ռ��ֵ��ʵ��ֵ 1.2��3.2��������
const unsigned int g_uApp3NiHiFull = 0x87F;   //�D�D�D3�������ر���̬����ֵ
const unsigned int g_uApp3NiHiNormal = 0x823; //�D�D�D3�������س�̬����ֵ
const unsigned int g_uApp3NiHiShort = 0x7EB;  //�D�D�D3��������ȱʧ̬����ֵ

const unsigned int g_uApp4NiHiFull = 0x0BF9;   //�D�D�D4�������ر���̬����ֵ
const unsigned int g_uApp4NiHiNormal = 0x0BDB; //�D�D�D4�������س�̬����ֵ
const unsigned int g_uApp4NiHiShort = 0x0ACC;  //�D�D�D4��������ȱʧ̬����ֵ

const unsigned int g_uchApp1NormFull = 0x31F;   //�D�D�D1��һ���Ե�ر���̬����ֵ
const unsigned int g_uchApp1NormNormal = 0x2B0; //�D�D�D1��һ���Ե�س�̬����ֵ
const unsigned int g_uchApp1NormShort = 0x291;  //�D�D�D1��һ���Ե��ȱʧ̬����ֵ

const unsigned int g_uchApp2NormFull = 0x63F;   //�D�D�D2��һ���Ե�ر���̬����ֵ
const unsigned int g_uchApp2NormNormal = 0x560; //�D�D�D2��һ���Ե�س�̬����ֵ
const unsigned int g_uchApp2NormShort = 0x522;  //�D�D�D2��һ���Ե��ȱʧ̬����ֵ

const unsigned int g_uApp1NiHiFull = 0x31F;   //�D�D�D1�������ر���̬����ֵ
const unsigned int g_uApp1NiHiNormal = 0x2B0; //�D�D�D1�������س�̬����ֵ
const unsigned int g_uApp1NiHiShort = 0x291;  //�D�D�D1��������ȱʧ̬����ֵ

const unsigned int g_uApp2NiHiFull = 0x63F;   //�D�D�D2�������ر���̬����ֵ
const unsigned int g_uApp2NiHiNormal = 0x560; //�D�D�D2�������س�̬����ֵ
const unsigned int g_uApp2NiHiShort = 0x522;  //�D�D�D2��������ȱʧ̬����ֵ

const unsigned int g_uAppLABat4Full = 0xA45;   //�D�D�D4VǦ���ر���̬����ֵ
const unsigned int g_uAppLABat4Normal = 0xA08; //�D�D�D4VǦ���س�̬����ֵ
const unsigned int g_uAppLABat4Short = 0x9CA;  //�D�D�D4VǦ����ȱʧ̬����ֵ

unsigned char g_uchConfigFlag = 0;//1 Ϊ����̬

unsigned char g_uPicControlFlag = 0; //��ͼ�񴫸������Ϳ�������