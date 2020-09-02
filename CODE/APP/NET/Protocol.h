#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

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

//ͨ������
typedef struct{
	uint8_t buffer[COMMBUFFERLEN];
	uint16_t len;
	
	uint8_t loginackflag; //ǩ��Ӧ���־
	uint8_t heartbeatackflag; //����Ӧ���־
	
	uint8_t startflag; //��������־ 0��Ч 1����
	uint8_t stopflag; //ֹͣ����־ 0��Ч 1ֹͣ
	
	uint8_t AckCMD201Flag;//ƽ̨Ӧ��201�����־����Ӧ����׮�ϱ��ĳ����Ϣ����
	
	uint8_t chargestrategy;//������ 0����Ϊֹ 1ʱ�� 2��� 3����
	uint32_t chargepara;//������ ʱ�䵥λΪ1�� ��λΪ0.01Ԫ ������λΪ0.01

	uint8_t TradeSerialNumber[32]; //������ˮ��
	
	uint8_t ChargeCardNumber[32]; //��翨��/�û�ʶ���
	
	uint8_t UserId[2];//�û�ID
	
	uint32_t ChargeRecordIndexBak;
	uint32_t ChargeRecordNumberBak;
	uint8_t NeedReportNum;
	REPORTCTL ReportCtl[50];
}COMMINFO;

uint8_t GetOnlineState(uint8_t port);
int8_t VerifyOnlineCardInfo(uint8_t port, uint32_t CardID, uint32_t *Balance);
void TaskProtocolHandle(void *pdata);


#endif
