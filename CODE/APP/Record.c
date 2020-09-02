#include "App.h"
#include "FRAM.h"
#include "ff.h"
#include "diskio.h"
#include "Record.h"
#include "UART.h"
#include <string.h>
#include <stdio.h>
#include "includes.h"
#include "Cfg.h"
static CHARGERECORDINFO ChargeRecordInfo;

static LOCKCARDINFO LockCardInfo;

static CHARGINGINFO ChargingInfo[2];

static FATFS FecordFS[2];

/************************************************************************************************************
** �� �� �� : GetChargeRecordIndex
** �������� : ��ȡ����¼������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	������
*************************************************************************************************************
*/
uint32_t GetChargeRecordIndex(uint8_t port)
{
	return ChargeRecordInfo.index;
}

/************************************************************************************************************
** �� �� �� : GetChargeRecordNumber
** �������� : ��ȡ����¼����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	����
*************************************************************************************************************
*/
uint32_t GetChargeRecordNumber(uint8_t port)
{
	uint32_t i, num = 0;
	for (i = 0; i < RECORD_TOTAL_NUM; i++){
		if (ChargeRecordInfo.recordstate[i] != 0){
			num++;
		}
	}
	return num;
}

/************************************************************************************************************
** �� �� �� : ChargeRecordInit
** �������� : ����¼��ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ChargeRecordInit(void)
{
	uint8_t os_err;
	uint16_t i;
	
	memset(&ChargeRecordInfo, 0, sizeof(CHARGERECORDINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramRead(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
	OSMutexPost (FramMutex);
	if (ChargeRecordInfo.index >= RECORD_TOTAL_NUM){
		memset(&ChargeRecordInfo, 0, sizeof(CHARGERECORDINFO));
	} else {
		for (i = 0; i < RECORD_TOTAL_NUM; i++){
			if (ChargeRecordInfo.recordstate[i] > 1){
				memset(&ChargeRecordInfo, 0, sizeof(CHARGERECORDINFO));
				break;
			}
		}
	}
}

/************************************************************************************************************
** �� �� �� : InsertChargeRecord
** �������� : �������¼
** ��    �� : recordbuf ��¼������ recordlen ��¼���� ������1000�ֽ�
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
*************************************************************************************************************
*/
uint8_t InsertChargeRecord(uint8_t *recordbuf, uint16_t recordlen)
{
	uint8_t os_err;
	char FileName[16];
	FIL file;
	FRESULT ret;
	uint32_t ByteNumToWrite;
	
	if (recordlen > 1000)
		return 0;
	if ((ChargeRecordInfo.index + 1) >= RECORD_TOTAL_NUM)
		ChargeRecordInfo.index = 0;
	else
		ChargeRecordInfo.index++;
	ChargeRecordInfo.recordstate[ChargeRecordInfo.index] = 1;
	
	OSMutexPend (SFlashMutex, 0, &os_err);
	sprintf(FileName,"1:/rec%d.txt", ChargeRecordInfo.index);
	ret = f_open(&file, FileName, FA_CREATE_ALWAYS | FA_WRITE);
	if(ret != FR_OK){
		OSMutexPost (SFlashMutex);
		return 0;
	}
	ret = f_write(&file, recordbuf, recordlen, &ByteNumToWrite);
	if((ret != FR_OK) || (ByteNumToWrite != recordlen)){
		f_close(&file);
		ret = f_mkfs(SERIALFLASH, 0, 4096);
		if (ret != FR_OK){
			print("error:record filesystem mount fail!\r\n");
		}
		ret = f_open(&file, FileName, FA_CREATE_ALWAYS | FA_WRITE);
		if(ret != FR_OK){
			OSMutexPost (SFlashMutex);
			return 0;
		}
		ret = f_write(&file, recordbuf, recordlen, &ByteNumToWrite);
		if((ret != FR_OK) || (ByteNumToWrite != recordlen)){
			f_close(&file);
			OSMutexPost (SFlashMutex);
			return 0;
		}
	}
	f_close(&file);
	OSMutexPost (SFlashMutex);
	
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
	OSMutexPost (FramMutex);
	
	return 1;
}

/************************************************************************************************************
** �� �� �� : DeleteChargeRecord
** �������� : ɾ������¼
** ��    �� : index ������
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
*************************************************************************************************************
*/
void DeleteChargeRecord(uint16_t index)
{
	uint8_t os_err;
	char FileName[16];
	
	if (index > RECORD_TOTAL_NUM)
		return;
	
	OSMutexPend (SFlashMutex, 0, &os_err);
	sprintf(FileName,"1:/rec%d.txt", index);
	f_unlink(FileName);
	OSMutexPost (SFlashMutex);
	
	ChargeRecordInfo.recordstate[index] = 0;
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
	OSMutexPost (FramMutex);
	
	return;
}

/************************************************************************************************************
** �� �� �� : FindChargeRecord
** �������� : ���ҳ���¼
** ��    �� : index ������ recordbuf ��¼������ recordlen ��¼���� ������1000�ֽ�
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
*************************************************************************************************************
*/
uint8_t FindChargeRecord(uint16_t index, uint8_t *recordbuf, uint16_t recordlen)
{
	uint8_t os_err;
	char FileName[16];
	FIL file;
	FRESULT ret;
	uint32_t ByteNumToRead;
	
	if (index > RECORD_TOTAL_NUM)
		return 0;
	if (ChargeRecordInfo.recordstate[index] == 0){
		return 0;
	}
	
	OSMutexPend (SFlashMutex, 0, &os_err);
	sprintf(FileName,"1:/rec%d.txt", index);
	ret = f_open(&file, FileName, FA_OPEN_EXISTING | FA_READ);
	if(ret != FR_OK){
		OSMutexPost (SFlashMutex);
		return 0;
	}
	ret = f_read(&file, recordbuf, recordlen, &ByteNumToRead);
	if(ret != FR_OK){
		f_close(&file);
		OSMutexPost (SFlashMutex);
		return 0;
	}
	f_close(&file);
	OSMutexPost (SFlashMutex);
	
	return 1;
}




/************************************************************************************************************
** �� �� �� : LockCardRecordInit
** �������� : ������¼��ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void LockCardRecordInit(void)
{
	uint8_t os_err;
	memset(&LockCardInfo, 0, sizeof(LOCKCARDINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramRead(FRAMADDR_LOCKCARDINFO, (uint8_t *)&LockCardInfo, sizeof(LOCKCARDINFO));
	OSMutexPost (FramMutex);
}

/************************************************************************************************************
** �� �� �� : InsertLockCardRecord
** �������� : ����������¼
** ��    �� : CardID ����
** ��    �� : ��
** ��    �� :	������¼��
*************************************************************************************************************
*/
uint32_t InsertLockCardRecord(uint32_t CardID)
{
	uint8_t i;
	uint8_t os_err;
	
	for (i = 0; i < 100; i++){
		if (LockCardInfo.CardID[i] == CardID){
			LockCardInfo.SerialNum[i] = 0;
			LockCardInfo.CardID[i] = 0;
		}
	}
	
	if ((LockCardInfo.StorePoint + 1) >= 100)
		LockCardInfo.StorePoint = 0;
	else
		LockCardInfo.StorePoint++;
	LockCardInfo.SerialPoint = LockCardInfo.SerialPoint + 1;
	LockCardInfo.SerialNum[LockCardInfo.StorePoint] = LockCardInfo.SerialPoint;
	LockCardInfo.CardID[LockCardInfo.StorePoint] = CardID;
	
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_LOCKCARDINFO, (uint8_t *)&LockCardInfo, sizeof(LOCKCARDINFO));
	OSMutexPost (FramMutex);
	
	return LockCardInfo.SerialNum[LockCardInfo.StorePoint];
}

/************************************************************************************************************
** �� �� �� : DeleteLockCardRecord
** �������� : ɾ��������¼
** ��    �� : CardID ����
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DeleteLockCardRecord(uint32_t CardID)
{
	uint8_t i;
	uint8_t os_err;
	
	for (i = 0; i < 100; i++){
		if (LockCardInfo.CardID[i] == CardID){
			LockCardInfo.SerialNum[i] = 0;
			LockCardInfo.CardID[i] = 0;
		}
	}
	
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_LOCKCARDINFO, (uint8_t *)&LockCardInfo, sizeof(LOCKCARDINFO));
	OSMutexPost (FramMutex);
}

/************************************************************************************************************
** �� �� �� : FindLockCardRecord
** �������� : ����������¼
** ��    �� : CardID ����
** ��    �� : SerialNum ���к�
** ��    �� :	0ʧ�� 1�ɹ�
*************************************************************************************************************
*/
uint8_t FindLockCardRecord(uint32_t CardID, uint32_t *SerialNum)
{
	uint8_t i;
	
	for (i = 0; i < 100; i++){
		if (LockCardInfo.CardID[i] == CardID){
			*SerialNum = LockCardInfo.SerialNum[i];
			return 1;
		}
	}
	
	return 0;
}

/************************************************************************************************************
** �� �� �� : ChargingRecordInit
** �������� : �������м�¼��ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ChargingRecordInit(void)
{
	uint8_t os_err;
	uint8_t i = 0,*p1,*p2;
	uint16_t sum,j,len;
	
	for (i = 0; i < 2; i++){
		memset(&ChargingInfo[i], 0, sizeof(CHARGINGINFO));
		OSMutexPend (FramMutex, 0, &os_err);
		FramRead(FRAMADDR_CHARGINGRECORDINFO + i * 1024, (uint8_t *)&ChargingInfo[i], sizeof(CHARGINGINFO));
		OSMutexPost (FramMutex);
		
		p1 = (uint8_t *)&ChargingInfo[i].Head;
		p2 = (uint8_t *)&ChargingInfo[i].Tail;
		len = p2 - p1;
		sum = 0;
		for (j = 0; j < len; j++)
			sum += *p1++;
		if ((ChargingInfo[i].Head == 0x5a5a) && (sum == ChargingInfo[i].Tail) && (ChargingInfo[i].Len <= 1000)){
			InsertChargeRecord(ChargingInfo[i].data, ChargingInfo[i].Len);
		}
		memset(&ChargingInfo[i], 0, sizeof(CHARGINGINFO));
		OSMutexPend (FramMutex, 0, &os_err);
		FramWrite(FRAMADDR_CHARGINGRECORDINFO + i * 1024, (uint8_t *)&ChargingInfo[i], sizeof(CHARGINGINFO));
		OSMutexPost (FramMutex);
	}
}

/************************************************************************************************************
** �� �� �� : InsertChargingRecord
** �������� : ��������̼�¼
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... recordbuf ������ recordlen ����
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
*************************************************************************************************************
*/
uint8_t InsertChargingRecord(uint8_t port, uint8_t *recordbuf, uint16_t recordlen)
{
	uint8_t os_err;
	uint8_t *p1,*p2;
	uint16_t sum,j,len;
	
	if (recordlen > 1000)
		return 0;
	memset(&ChargingInfo[port], 0, sizeof(CHARGINGINFO));
	ChargingInfo[port].Head = 0x5a5a;
	ChargingInfo[port].Len = recordlen;
	memcpy(ChargingInfo[port].data, recordbuf, recordlen);
	p1 = (uint8_t *)&ChargingInfo[port].Head;
	p2 = (uint8_t *)&ChargingInfo[port].Tail;
	len = p2 - p1;
	sum = 0;
	for (j = 0; j < len; j++)
		sum += *p1++;
	ChargingInfo[port].Tail = sum;
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGINGRECORDINFO + port * 1024, (uint8_t *)&ChargingInfo[port], sizeof(CHARGINGINFO));
	OSMutexPost (FramMutex);
	return 1;
}

/************************************************************************************************************
** �� �� �� : DeleteChargingRecord
** �������� : ɾ���������м�¼
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
*************************************************************************************************************
*/
void DeleteChargingRecord(uint8_t port)
{
	uint8_t os_err;
	
	memset(&ChargingInfo[port], 0, sizeof(CHARGINGINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGINGRECORDINFO + port * 1024, (uint8_t *)&ChargingInfo[port], sizeof(CHARGINGINFO));
	OSMutexPost (FramMutex);
}


/************************************************************************************************************
** �� �� �� : RecordInit
** �������� : ��¼��ʼ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void RecordInit(void)
{
	FRESULT ret;
	FIL file;
	
	//����
	ret = f_mount(SERIALFLASH, &FecordFS[SERIALFLASH]);
	if (ret != FR_OK){
		ret = f_mkfs(SERIALFLASH, 0, 4096);
		if (ret != FR_OK){
			print("record filesystem mount fail!\r\n");
			while(1);
		}
	}
	//����ļ�
	ret = f_open(&file,"1:/rec.txt",FA_OPEN_EXISTING | FA_READ);
	if(ret != FR_OK){
		ret = f_open(&file,"1:/rec.txt",FA_OPEN_EXISTING | FA_READ);
		if(ret != FR_OK){
			//��ʽ���ļ�
			print("create filesystem for record\r\n");
			ret = f_mkfs(SERIALFLASH, 0, 4096);
			if(ret != FR_OK){
				print("create filesystem fail!\r\n");
				while(1);
			}
			f_open(&file,"1:/rec.txt",FA_CREATE_ALWAYS | FA_WRITE);
			f_close(&file);
		}
	}
	f_close(&file);
	
	//����¼��ʼ��
	ChargeRecordInit();
	
	//������¼��ʼ��
	LockCardRecordInit();
	
	//�������м�¼��ʼ��
	ChargingRecordInit();
}

/************************************************************************************************************
** �� �� �� : DeleteAllRecord
** �������� : ɾ�����м�¼����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DeleteAllRecord(void)
{
	uint8_t os_err;
	
	//���ز���ʽ���ļ�
	f_mount(SERIALFLASH, &FecordFS[SERIALFLASH]);
	f_mkfs(SERIALFLASH, 0, 4096);
	
	//ɾ������¼��Ϣ
	memset(&ChargeRecordInfo, 0, sizeof(CHARGERECORDINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
	OSMutexPost (FramMutex);
	
	//ɾ��������¼��Ϣ
	memset(&LockCardInfo, 0, sizeof(LOCKCARDINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_LOCKCARDINFO, (uint8_t *)&LockCardInfo, sizeof(LOCKCARDINFO));
	OSMutexPost (FramMutex);
	
	//ɾ���������м�¼��Ϣ
	memset(&ChargingInfo[0], 0, sizeof(CHARGINGINFO));
	memset(&ChargingInfo[1], 0, sizeof(CHARGINGINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGINGRECORDINFO + 0 * 1024, (uint8_t *)&ChargingInfo[0], sizeof(CHARGINGINFO));
	FramWrite(FRAMADDR_CHARGINGRECORDINFO + 0 * 1024, (uint8_t *)&ChargingInfo[1], sizeof(CHARGINGINFO));
	OSMutexPost (FramMutex);
}
