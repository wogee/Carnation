#include"Board.h"
#include "UART.h"
#include "GPIO.h"
#include "includes.h"
#include "TIMER.h"

/****************************************************************************\
 Function:  RS485Init
 Descript:  485��ʼ��
 Input:	    ������
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-30
 *****************************************************************************/
void RS485Init(uint32_t bps)
{
	UartInit(RS485UARTNO,bps);

	GpioSetDir(GPIO_485CON2,OUTPUT);
	GpioSetLev(GPIO_485CON2,LOW);
	
}


/****************************************************************************\
 Function:  RS485Write
 Descript:  ��485д����
 Input:	    ��д������ݺ����ݳ���
 Output:    �� 
 Return:    ʵ��д��ĳ���
 Author:    quanhouwei
 Datetime:  17-08-30
 *****************************************************************************/
uint16_t RS485Write(uint8_t *data, uint16_t datalen)
{
	uint16_t len=0;
//	uint32_t tim;
//	static uint32_t timeroutcnt = 250000;
	
	GpioSetLev(GPIO_485CON2,LOW);

	DelayMs(10);
	
	len=UartWrite(RS485UARTNO,data,datalen);
	
//	for (tim = 0; tim < timeroutcnt; tim ++)
//		__nop();

	DelayMs(5);

	GpioSetLev(GPIO_485CON2,HIGH);
	
	return len;

	
}


/****************************************************************************\
 Function:  RS485Read
 Descript:  ��485������
 Input:	    �����ݵ�BUF�������ĳ���
 Output:    �� 
 Return:    ���ݶ�ȡ�ĳ���
 Author:    quanhouwei
 Datetime:  17-08-30
 *****************************************************************************/
uint16_t RS485Read(uint8_t *data, uint16_t datalen)
{
	return UartRead(RS485UARTNO,data,datalen);
}


/****************************************************************************\
 Function:  RS485Flush
 Descript:  ���485���ݻ���
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-30
 *****************************************************************************/
void RS485Flush(void)
{
	UartFlush(RS485UARTNO);
}








