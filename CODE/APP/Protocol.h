#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#include "Cfg.h"
#pragma pack(1)

#define STARTCODE1	0xAA //��ʼ��1
#define STARTCODE2	0xF5 //��ʼ��2
#define MSGCODE			0x10 //��Ϣ��

#define COMMBUFFERLEN 1024 //ͨ�Ż����С

#define CTL_NONE					0 //�޲���
#define CTL_STARTCHARGE		1 //�������
#define CTL_STOPCHARGE		2 //ֹͣ���

typedef struct{
	uint16_t index;//����
//	uint8_t ReportState;//�ϱ�״̬ 0δ�ϱ� 1�ϱ��Ⱥ�̨Ӧ�� 2�ϱ����ѵõ���̨��Ӧ
//	uint8_t CardID[32];//��翨��
//	uint8_t StartDateTime[8];//�������ʱ��
//	uint8_t StopDateTime[8];//������ʱ��
}REPORTCTL;

typedef struct{
	uint32_t ReportOnlineCardID;//�ϱ����߿�����
	uint8_t ReportFlag;//�ϱ�204���ı�־ 0�����ϱ� 1���ϱ� 2�ϱ��ѵõ��ظ�
	uint8_t ReportRet;//�ϱ�204�����յ��Ľ�� 0��Ч�ʻ� 1�Ƿ��ʻ� 2���� 3�Ѻڿ��ָ��������� 4����������ڿ� 5�Ƿ����� 6��ʧ 7���� 8������� 9�ÿ��Ѿ��ڱ��׮ʹ��
	uint32_t OnlineCardIDBalance;//���߿���� ��λ0.01Ԫ
	
}REportCMD204;

//ͨ������
typedef struct{
	uint8_t buffer[COMMBUFFERLEN];
	uint16_t len;
	uint8_t SendCmd204;             //�����û���ѯ��־  0ֹͣ  1����	
	uint8_t loginackflag;           //ǩ��Ӧ���־
	uint8_t heartbeatackflag;       //����Ӧ���־	
	uint8_t startflag;              //��������־ 0��Ч 1����
	uint8_t stopflag;               //ֹͣ����־ 0��Ч 1ֹͣ	
	uint8_t AckCMD201Flag;          //ƽ̨Ӧ��201�����־����Ӧ����׮�ϱ��ĳ����Ϣ����	
	uint8_t chargestrategy;         //������ 0����Ϊֹ 1ʱ�� 2��� 3����
	uint32_t chargepara;            //������ ʱ�䵥λΪ1�� ��λΪ0.01Ԫ ������λΪ0.01
	uint8_t TradeSerialNumber[32];  //������ˮ��	
	uint8_t ChargeCardNumber[32];   //��翨��	
	uint32_t ChargeRecordIndexBak;
	uint32_t ChargeRecordNumberBak;
	uint8_t NeedReportNum;
	uint8_t UserId[2];//�û�ID
	uint8_t SendCmd104;             //����״̬��Ϣ��־  0ֹͣ  1����
	REPORTCTL ReportCtl[REPORT_NUM];
}COMMINFO;

typedef struct{
	uint8_t UpdataFlag;
	uint16_t UserId;//�û�ID
	uint32_t LastMoney;
	uint32_t NewMoney;
}MODIFINFO;


uint8_t GetOnlineState(uint8_t port);   
void GetUserId(uint8_t port, uint8_t* buf);
void TaskProtocolHandle(void *pdata);
void SendCMD204Flay(uint8_t port,uint8_t mark);
void SendCMD104Flay(uint8_t port,uint8_t mark);
void SetUserId(uint8_t port,uint16_t UserId);
uint8_t VerifyOnlineCardInfo(uint8_t port, uint32_t CardID, uint32_t *Balance);
uint32_t UpdateMoneyJudge(uint8_t port, uint16_t UserId, uint32_t RawMoney);
void TaskProtocolHandle(void *pdata);
uint16_t GetUpdataPre(void);
#endif
