#ifndef _CARDTASK_H
#define _CARDTASK_H

#include "MT625.h"

// ����״̬
#define CARD_ST_INIT 		0		// ��ʼ��
#define CARD_ST_READCARD    1		// ����
#define CARD_ST_WRITECARD   2		// д��

//��״̬
#define UNLOCK              1
#define LOCK                2


//������
#define  TYPE_CARD_ONLINE  	1		// ���߿�
#define  TYPE_CARD_OFFLINE	2		// ���߿�

typedef struct
{
	uint8_t 	state;		    // ״̬
	uint8_t 	swmark;		    // �Ƿ��п�ˢ  0 û��ˢ��  1 ��ˢ��
	uint8_t 	type;		    // ����  ���߿������߿�
	uint8_t 	id[10];		    // ��ID 
	uint32_t    remoney;	    // ���
	uint8_t     lockst;		    // ��״̬   0 δ��  1 ����
	uint8_t 	lockctl;        // ���� 1 ����  2 ����
	uint32_t  conmoney;         // ���ѽ��		
	M1CARDVIF cardinfo;         // ����Ϣ
}CARDTASKINFO;


uint8_t CardSetMoneyAndUnlock(uint32_t id, uint32_t money);
uint8_t CardSetPileNumberAndLock(uint32_t id, uint8_t *data);
uint8_t CardGetState(void);
uint8_t CardGetType(void);
uint32_t CardGetId(void);
uint8_t CardSetLockState(uint32_t id, uint8_t state);
uint8_t CardGetLockState(void);
uint8_t CardSetMoney(uint32_t id, uint32_t money);
uint32_t CardGetMoney(void);
uint8_t CardSetPileNumber(uint32_t id, uint8_t *data);
void CardGetPileNumber(uint8_t *data);
void CardTask(void *pdata);

#endif
