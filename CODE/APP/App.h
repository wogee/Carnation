#ifndef _APP_H_
#define _APP_H_

#include "includes.h"

extern OS_EVENT *FramMutex;
extern OS_EVENT *SFlashMutex;
extern OS_EVENT *DubugSem;

//����ջ
#define MAINTASKSTKLEN    	3172	//������
#define CHARGETASKSTKLEN		512		//�������
#define PROTOCOLTASKSTKLEN	1536	//ͨ��Э������
#define NETTASKSTKLEN				512		//��������
#define METERTASKSTKLEN			256		//�������
#define CARDTASKSTKLEN			256		//������



//���ȼ�
#define MUTEX_FRAMPRIO			3
#define MUTEX_SFLASHPRIO		4
#define MAINTASKPRIO				7		//������
#define CARDTASKPRIO				6		//������
#define CHARGETASKPRIO			8		//�������
#define PROTOCOLTASKPRIO		10	//ͨ��Э������
#define NETTASKPRIO					9		//��������
#define METERTASKPRIO				5		//�������

#endif
