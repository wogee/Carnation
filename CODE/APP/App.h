#ifndef _APP_H_
#define _APP_H_

#include "includes.h"

extern OS_EVENT *FramMutex;
extern OS_EVENT *SFlashMutex;
extern OS_EVENT *DubugSem;

//任务栈
#define MAINTASKSTKLEN    	3172	//主任务
#define CHARGETASKSTKLEN		512		//充电任务
#define PROTOCOLTASKSTKLEN	1536	//通信协议任务
#define NETTASKSTKLEN				512		//网络任务
#define METERTASKSTKLEN			256		//电表任务
#define CARDTASKSTKLEN			256		//卡任务



//优先级
#define MUTEX_FRAMPRIO			3
#define MUTEX_SFLASHPRIO		4
#define MAINTASKPRIO				7		//主任务
#define CARDTASKPRIO				6		//卡任务
#define CHARGETASKPRIO			8		//充电任务
#define PROTOCOLTASKPRIO		10	//通信协议任务
#define NETTASKPRIO					9		//网络任务
#define METERTASKPRIO				5		//电表任务

#endif
