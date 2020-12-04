/***********************************************************************************************************
* brief: AC monitor system
* create date: 2018-09-27
* create author: Huang Kong
************************************************************************************************************
* modify date: xxxx-xx-xx
* modify author: xxxxxx
* modify note: xx
************************************************************************************************************
* modify date: xxxx-xx-xx
* modify author: xxxxxx
* modify note: xx
************************************************************************************************************/

#include "OS.h"
#include "App.h"
#include "MainProc.h"
#include "Board.h"
#include "Para.h"
#include "Record.h"
#include "ChgInterface.h"
#include "ChgInterface.h"
#include "Protocol.h"
#include "NetTask.h"
#include "ElmTask.h"
#include "CardTask.h"
static OS_STK MainTaskStk[MAINTASKSTKLEN];//������
static OS_STK ChargeTaskStk[CHARGETASKSTKLEN];//�������
static OS_STK ProtocolTaskStk[PROTOCOLTASKSTKLEN];//ͨ��Э������
static OS_STK NetTaskStk[NETTASKSTKLEN];//��������
static OS_STK MeterTaskStk[METERTASKSTKLEN];//�������
static OS_STK CardTaskStk[CARDTASKSTKLEN];//������

OS_EVENT *FramMutex;
OS_EVENT *SFlashMutex;
OS_EVENT *DubugSem;

/************************************************************************************************************
** �� �� �� : main
** �������� : ������ں���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	0
*************************************************************************************************************
*/

int main(void)
{
	INT8U os_err;

	BoardInit();
	
	OSInit();
	
	FramMutex = OSMutexCreate(MUTEX_FRAMPRIO, &os_err);
	SFlashMutex = OSMutexCreate(MUTEX_SFLASHPRIO, &os_err);
	DubugSem = OSSemCreate(1);
	
	ParaInit();
	RecordInit();

	OSTaskCreateExt(MainTask, (void *)0, &MainTaskStk[MAINTASKSTKLEN - 1],  MAINTASKPRIO,	0, &MainTaskStk[0], MAINTASKSTKLEN,  (void *)0,  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  // ������
	OSTaskCreateExt(ChargeTask, (void *)0, &ChargeTaskStk[CHARGETASKSTKLEN - 1],  CHARGETASKPRIO,	0, &ChargeTaskStk[0], CHARGETASKSTKLEN,  (void *)0,  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  // �������
	OSTaskCreateExt(TaskProtocolHandle, (void *)0, &ProtocolTaskStk[PROTOCOLTASKSTKLEN - 1],  PROTOCOLTASKPRIO,	0, &ProtocolTaskStk[0], PROTOCOLTASKSTKLEN,  (void *)0,  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); // ͨ��Э������
	OSTaskCreateExt(NetTask, (void *)0, &NetTaskStk[NETTASKSTKLEN - 1],  NETTASKPRIO,	0, &NetTaskStk[0], NETTASKSTKLEN,  (void *)0,  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  // ������������
	OSTaskCreateExt(ElmTask, (void *)0, &MeterTaskStk[METERTASKSTKLEN - 1],  METERTASKPRIO,	0, &MeterTaskStk[0], METERTASKSTKLEN,  (void *)0,  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  // �������
	OSTaskCreateExt(CardTask, (void *)0, &CardTaskStk[CARDTASKSTKLEN - 1],  CARDTASKPRIO,	0, &CardTaskStk[0], CARDTASKSTKLEN,  (void *)0,  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  // ������
	
	OSStart();

	return 0;
}
