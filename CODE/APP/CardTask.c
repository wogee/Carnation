#include "includes.h"
#include "stdint.h"
#include "encrypt.h"
#include "MT625.h"
#include "CardTask.h"
#include "Para.h"
#include "UART.h"
static CARDTASKINFO CardTaskInfo={0};
static uint8_t SetCardFlag;
static uint8_t SetCardType;  //
static uint8_t SetCardPara[16];
static uint8_t SetCardRet;   //0ʧ�� 1�ɹ�
 
/************************************************************************************************************
** �� �� �� : CardGetState
** �������� : ��ȡ��״̬
** ��    �� : ��
** ��    �� : ��
** ��    �� :	1 ��ˢ��  0 δˢ��
*************************************************************************************************************
*/
uint8_t CardGetState(void)
{
	if (CardTaskInfo.swmark == 1){
		return 1;
	} else {
		return 0;
	}
}

/************************************************************************************************************
** �� �� �� : CardGetType
** �������� : ��ȡ������ (���߿� �����߿�)
** ��    �� : ��
** ��    �� : 1 ���߿�  2 ���߿�
** ��    �� :	��
*************************************************************************************************************
*/
uint8_t CardGetType(void)
{
	return CardTaskInfo.cardinfo.type;
}

/************************************************************************************************************
** �� �� �� : CardGetId
** �������� : ��ȡ��ID
** ��    �� : ��
** ��    �� : ��
** ��    �� :	����
*************************************************************************************************************
*/
uint32_t CardGetId(void)
{
	return CardTaskInfo.cardinfo.cardID;
}

/************************************************************************************************************
** �� �� �� : CardSetLockState
** �������� : ���ÿ���״̬
** ��    �� : state 1������� 2����
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
*************************************************************************************************************
*/
uint8_t CardSetLockState(uint32_t id, uint8_t state)
{
	SetCardRet = 0;
	SetCardType = 1;
	SetCardPara[0] = state;
	SetCardFlag = 1;
	while(SetCardFlag == 1)
		OSTimeDlyHMSM (0, 0, 0, 20);	
	return SetCardRet;
}
 


/************************************************************************************************************
** �� �� �� : CardGetLockState
** �������� : ��ȡ����״̬
** ��    �� : ��
** ��    �� : ��
** ��    �� :	1 ����   2 ����
*************************************************************************************************************
*/
uint8_t CardGetLockState(void)
{
	return CardTaskInfo.cardinfo.lockst;
}

/************************************************************************************************************
** �� �� �� : CardSetMoney
** �������� : ���ÿ����
** ��    �� : money ����0.01Ԫ
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
*************************************************************************************************************
*/
uint8_t CardSetMoney(uint32_t id, uint32_t money)
{
	SetCardRet = 0;
	SetCardType = 2;
	memcpy(SetCardPara, &money, 4);
	SetCardFlag = 1;
	while(SetCardFlag == 1)
		OSTimeDlyHMSM (0, 0, 0, 20);	
	return SetCardRet;
}


/************************************************************************************************************
** �� �� �� : CardSetMoneyAndUnlock
** �������� : ���ÿ�������
** ��    �� : money ����0.01Ԫ
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
*************************************************************************************************************
*/
uint8_t CardSetMoneyAndUnlock(uint32_t id, uint32_t money)
{
	if(id != CardGetId())
		return 0;
	SetCardRet = 0;
	SetCardType = 4;
	memcpy(SetCardPara, &money, 4);
	SetCardFlag = 1;
	while(SetCardFlag == 1)
		OSTimeDlyHMSM (0, 0, 0, 20);	
	return SetCardRet;
}




/************************************************************************************************************
** �� �� �� : CardGetMoney
** �������� : ��ȡ�����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��� ����0.01Ԫ
*************************************************************************************************************
*/
uint32_t CardGetMoney(void)
{
	return CardTaskInfo.cardinfo.Money;
}

/************************************************************************************************************
** �� �� �� : CardSetPileNumber
** �������� : ���ÿ�׮���
** ��    �� : data ��� 4�ֽ�
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
*************************************************************************************************************
*/
uint8_t CardSetPileNumber(uint32_t id, uint8_t *data)
{
	SetCardRet = 0;
	SetCardType = 3;
	memcpy(SetCardPara, data, 4);
	SetCardFlag = 1;
	while(SetCardFlag == 1)
		OSTimeDlyHMSM (0, 0, 0, 20);
	return SetCardRet;
}




/************************************************************************************************************
** �� �� �� : CardSetPileNumberAndLock
** �������� : ���ÿ�׮��Ų�����
** ��    �� : data ��� 4�ֽ�
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
*************************************************************************************************************
*/
uint8_t CardSetPileNumberAndLock(uint32_t id, uint8_t *data)
{
	if(id != CardGetId())
		return 0;
	SetCardRet = 0;
	SetCardType = 5;
	memcpy(SetCardPara, data, 4);
	SetCardFlag = 1;
	while(SetCardFlag == 1)
		OSTimeDlyHMSM (0, 0, 0, 20);
	return SetCardRet;
}



/************************************************************************************************************
** �� �� �� : CardGetPileNumber
** �������� : ��ȡ��׮���
** ��    �� : ��
** ��    �� : data ��� 4�ֽ�
** ��    �� :	��
*************************************************************************************************************
*/
void CardGetPileNumber(uint8_t *data)
{
	if (data != 0){
		memcpy(data, CardTaskInfo.cardinfo.PileNumber, 4);
	}
}



/************************************************************************************************************
** �� �� �� : CardProc
** �������� : ��������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void CardProc(void)
{
	uint32_t UID;

	static uint8_t state;
	uint64_t data64;
	uint8_t CustomNum[4]; 
	
	if (MTSearch(&UID) == 1){
		if (state != 1){
			ParaGetCustomerNumber(CustomNum);                          //��ȡ�ͻ����
			data64 = CalcKey(*(uint32_t*)CustomNum);                   //�ͻ����תΪ��֤��
			memcpy(CardTaskInfo.cardinfo.KeyA, &data64, 6);
			if (MTRead(&CardTaskInfo.cardinfo) == 1){
				state = 1;
				CardTaskInfo.swmark = 1;                              //��ˢ��
			}
		}
	} else {
		state = 0;
		CardTaskInfo.cardinfo.cardID = 0;
		CardTaskInfo.swmark = 0;                                      //ûˢ��
	}
	
	if (SetCardFlag == 1){
		ParaGetCustomerNumber(CustomNum);
		data64 = CalcKey(*(uint32_t*)CustomNum);
		memcpy(CardTaskInfo.cardinfo.KeyA, &data64, 6);
		if (SetCardType == 1){                                        //����
			CardTaskInfo.cardinfo.lockst = SetCardPara[0];
			if (MTWrite(&CardTaskInfo.cardinfo) == 1){
				SetCardRet = 1;
			}
			SetCardFlag = 0;
		} else if (SetCardType == 2){                                 //д���
			memcpy(&CardTaskInfo.cardinfo.Money , SetCardPara, 4);
			if (MTWrite(&CardTaskInfo.cardinfo) == 1){
				SetCardRet = 1;
			}
			SetCardFlag = 0;
		} else if (SetCardType == 3){                                 //д׮���
			memcpy(&CardTaskInfo.cardinfo.PileNumber , SetCardPara, 4);
			if (MTWrite(&CardTaskInfo.cardinfo) == 1){
				SetCardRet = 1;
			}
			SetCardFlag = 0;
		} else if (SetCardType == 4){                                 //д������
			memcpy(&CardTaskInfo.cardinfo.Money , SetCardPara, 4);
			CardTaskInfo.cardinfo.lockst = UNLOCK;//����    
			if (MTWrite(&CardTaskInfo.cardinfo) == 1){
				SetCardRet = 1;
			}
			SetCardFlag = 0;
		}
		else if (SetCardType == 5){                                   //д׮�Ų�����
			memcpy(&CardTaskInfo.cardinfo.PileNumber , SetCardPara, 4);
			CardTaskInfo.cardinfo.lockst = LOCK;//����
			if (MTWrite(&CardTaskInfo.cardinfo) == 1){
				SetCardRet = 1;
			}
			SetCardFlag = 0;
		}
	}
}

/************************************************************************************************************
** �� �� �� : CardTask
** �������� : ������
** ��    �� : pdata ��ʹ��
** ��    �� : ��

** ��    �� :	��
*************************************************************************************************************
*/
void CardTask(void *pdata)
{
	print("card task start...\r\n");
	MTInit();
	while(1){
		CardProc();
		OSTimeDlyHMSM (0, 0, 0, 20);
	}
}


