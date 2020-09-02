#ifndef _RECORD_H
#define _RECORD_H
#include "Cfg.h"
//����洢���ݷֲ�
//������Ϣ Ԥ��1024���ֽڿռ�
#define FRAMADDR_LOCKCARDINFO	0
//����¼��Ϣ Ԥ��1024���ֽڿռ�
#define FRAMADDR_CHARGERECORDINFO	0x400
//�������м�¼��Ϣ Ԥ��2*1024�ֽڿռ�
#define FRAMADDR_CHARGINGRECORDINFO	0x800

//����¼������Ϣ
typedef struct{
	uint16_t index;//������
	uint8_t recordstate[RECORD_TOTAL_NUM];//��¼״̬ 0�޼�¼ 1�м�¼
}CHARGERECORDINFO;

//������¼��Ϣ
typedef struct{
	uint32_t StorePoint;//�洢ָ�� 0-199
	uint16_t SerialPoint;//���к�ָ��
	uint16_t SerialNum[100];//���к�
	uint32_t CardID[100];//����
}LOCKCARDINFO;

//�������м�¼��Ϣ
typedef struct{
	uint16_t Head;//�̶�Ϊ0x5a5a
	uint16_t Len;//��������Ч�ֽ��� ������1000�ֽ�
	uint8_t data[1000];//������
	uint16_t Tail;//��ͷ��־������ͷ��־����β��־��������β��־���������ۼӺ�
}CHARGINGINFO;

uint32_t GetChargeRecordIndex(uint8_t port);
uint32_t GetChargeRecordNumber(uint8_t port);

uint8_t InsertChargeRecord(uint8_t *recordbuf, uint16_t recordlen);
void DeleteChargeRecord(uint16_t index);
uint8_t FindChargeRecord(uint16_t index, uint8_t *recordbuf, uint16_t recordlen);

uint32_t InsertLockCardRecord(uint32_t CardID);
void DeleteLockCardRecord(uint32_t CardID);
uint8_t FindLockCardRecord(uint32_t CardID, uint32_t *SerialNum);

uint8_t InsertChargingRecord(uint8_t port, uint8_t *recordbuf, uint16_t recordlen);
void DeleteChargingRecord(uint8_t port);

void RecordInit(void);
void DeleteAllRecord(void);

#endif


