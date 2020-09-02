/****************************************Copyright (c)**************************************************
**                         Guangzhou ZHIYUAN electronics Co.,LTD.                               
**                                     
**                               http://www.embedtools.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name: includes.h
** Last modified Date:  2004-05-20
** Last Version: 1.0
** Descriptions: uC/OS-II Include File
**
**------------------------------------------------------------------------------------------------------
** Created By: Chenmingji
** Created date:   2004-05-20
** Version: 1.0
** Descriptions: First version
**
**------------------------------------------------------------------------------------------------------
** Modified by: Chenmingji
** Modified date: 2004-09-17
** Version: 1.02
** Descriptions: Change for IRQ.S; Support C++
**
**------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
********************************************************************************************************/
/********************************/
/*      uC/OS-II���������       */
/*      uC/OS-II specital code  */
/********************************/
#ifndef __INCLUDES_H 
#define __INCLUDES_H

	#ifdef __cplusplus
	extern "C" {
	#endif

#include    "os_cpu.h"
#include    "os_cfg.h"
#include    "ucos_ii.h"

	#ifdef __cplusplus
	}
	#endif

#include    <string.h>

   /* ���ڲ�ͬ����ж�OSIntExit()�Զ�ջ��ʹ�������ͬ�������������궨��    */
   /* for any types of interrupt , OSIntExit() using the stacks is different , so ,must add this define*/
//#define OSIntCtxSw()    return


#endif
/**********************end****************************************/
