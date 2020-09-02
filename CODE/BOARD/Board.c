#include "Board.h"
#include <LPC177x_8x.H>
#include "UART.h"
#include "TIMER.h"
#include "CAN.h"
#include "LED.h"
#include "FRAM.h"
#include "MX25L.h"
#include "RTC.h"
#include "BUZZER.h"
#include "DWIN.h"
#include "EEPROM.h"
#include "RELAY.h"
#include "GPIO.h"
#include "SPI.h"
#include "includes.h"


/****************************************************************************\
 Function:  BoardInit
 Descript:  �弶��ʼ��
 Input:	    ��	
 Output:    �� 
 Return:    0
 Author:    quanhouwei
 Datetime:  17-08-21
 *****************************************************************************/
void BoardInit(void)
{	
	SystemInit();	      // ʱ��   (CPU 12M p 60M)
	 
	UartInit(DEBUGUARTNO,115200);   // ����

	TimerInit(0,SYSTIME); // ��ʱ��
	
	CANInit(DEV_CAN1, 125000); //������ư�ͨ��

	LedInit();			  // LED��	
	
	RelayInit(); //�̵�����ʼ��
	
	FramInit();			  // ����

	RtcInit();			  // RTC

	BuzzerInit();		  // ������

	DwinInit(); //��������ʼ��
	
	EepromInit(); //EEPROM��ʼ��
	
	GpioSetDir(GPIO_NORCS,OUTPUT);
	
	SpiInit(MX25SPINO); //SPI��ʼ����NORFLASH
	
}


void delay(uint16_t dely)
{
	uint16_t i=0, j=0;
	
	for(i=0; i <= dely/10; i++)
	{
		for(j = 0; j <= dely; j++)
		{
		}
	}
}


/****************************************************************************\
 Function:  DelayMs 
 Descript:  ��ʱ���룬���ò���ϵͳ�ĵ���
 Input:	    ��ʱ�ĺ�����	
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-17
 *****************************************************************************/
void DelayMs(uint16_t data)
{
	  OSTimeDly(data);
}


























































