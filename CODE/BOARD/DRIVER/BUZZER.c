#include"Board.h"
#include "GPIO.h"


/****************************************************************************\
 Function:  BuzzerInit
 Descript:  ��������ʼ��
 Input:	    ��	
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/
void BuzzerInit(void)
{
	GpioSetDir(GPIO_BUZZER,OUTPUT);

	GpioSetLev(GPIO_BUZZER,LOW);
}


/****************************************************************************\
 Function:  BuzzerOn
 Descript:  ʹ�ܷ�����
 Input:	    ��	
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/
void BuzzerOn(void)
{
	GpioSetLev(GPIO_BUZZER,HIGH);
}


/****************************************************************************\
 Function:  BuzzerOff
 Descript:  ʧ�ܷ�����
 Input:	    ��	
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/
void BuzzerOff(void)
{
	GpioSetLev(GPIO_BUZZER,LOW);
}





