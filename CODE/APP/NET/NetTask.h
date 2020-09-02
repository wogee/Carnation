#ifndef _NETTASK_H
#define _NETTASK_H

#define NETDATAMAXLEN     1024   // ����������󳤶�
#define SNNUM             2

#define GPRS             2
#define ETH              1
typedef struct{
	uint8_t state;

	uint8_t step;			// ����
	uint8_t loginmark;   // ��¼��־ 1 ��¼�ɹ�  0 δ��¼
	uint8_t heartmark;	// ������־ 1 ������Ӧ��  0 ������Ӧ��
	
	uint8_t socketportusedflag[SNNUM];
	uint8_t serverip[SNNUM][4];
	uint16_t serverport[SNNUM];
	
	uint8_t LinkFlag; //��̫��link״̬ 1����link up
	
	uint8_t ConnCmd[SNNUM]; //���ӷ��������� 0���� 1����
	uint8_t ConnState[SNNUM]; //���ӷ�����״̬ 0δ���� 1������
	
	uint8_t SendCmd[SNNUM]; //������������ 0���� 1����
	uint8_t SendState[SNNUM]; //��������״̬ 0δ�ɹ� 1�ɹ�
	
	uint8_t SendBuf[SNNUM][NETDATAMAXLEN];
	uint16_t SendLen[SNNUM];
	
	uint8_t RecvBuf[SNNUM][NETDATAMAXLEN];
	uint16_t RecvWr[SNNUM];
	uint16_t RecvRr[SNNUM];
}NETINFO;


uint8_t NetSend(uint8_t sn, uint8_t *buffer, uint16_t len);
uint16_t NetRecv(uint8_t sn, uint8_t *buffer, uint16_t len);
uint8_t NetConn(uint8_t sn, uint8_t serverip[4], uint16_t serverport);
void NetTask(void *pdata);


#endif




