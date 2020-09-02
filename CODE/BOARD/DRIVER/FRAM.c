#include "Board.h"
#include "FRAM.h"
#include "GPIO.h"
#include "SPI.h"
#include "includes.h"

/************************************************************************************************************
** �� �� �� : FramEnable
** �������� : ʹ������洢��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void FramEnable(void)
{
	GpioSetDir(GPIO_FRAMCS,OUTPUT);
	GpioSetLev(GPIO_FRAMCS,LOW);
}

/************************************************************************************************************
** �� �� �� : FramDisable
** �������� : ʧ������洢��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void FramDisable(void)
{
	GpioSetDir(GPIO_FRAMCS,OUTPUT);
	GpioSetLev(GPIO_FRAMCS,HIGH);
}

/************************************************************************************************************
** �� �� �� : FramInit
** �������� : ����洢����ʼ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void FramInit(void)
{
	SpiInit(FRAMSPINO);
	FramDisable();
}

/************************************************************************************************************
** �� �� �� : tdelay
** �������� : ��ʱ����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void tdelay(uint16_t dely)
{
	uint16_t i=0, j=0;

	for(i = 0; i <= dely/10; i++){
		for(j = 0; j <= dely; j++){
		}
	}
}

/************************************************************************************************************
** �� �� �� : FramInit
** �������� : ����洢��д����
** ��    �� : addr ��ַ data ���ݻ����� length ���ݳ���
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void FramWrite(uint16_t addr, uint8_t *data, uint16_t length)
{
	uint16_t  i=0;
	uint8_t   st=0;

	FramEnable();
	tdelay(5);
	SpiWriteByte(FRAMSPINO,COMD_RD_STATUS);
	st=SpiReadByte(FRAMSPINO);
	FramDisable();

	if ((st & 0x71) == 0){
		FramEnable();
		tdelay(5);
		SpiWriteByte(FRAMSPINO,COMD_WR_ENABLE);
		FramDisable();
		
		do {
			FramEnable();
			tdelay(5);
			SpiWriteByte(FRAMSPINO,COMD_RD_STATUS);
			st=SpiReadByte(FRAMSPINO);
			FramDisable();
		} while(st & 0x80);

		FramEnable();
		tdelay(5);
		SpiWriteByte(FRAMSPINO,COMD_WR_MEMORY);
		SpiWriteByte(FRAMSPINO,(addr >> 8) & 0xff);
		SpiWriteByte(FRAMSPINO,addr & 0xff);
		for (i = 0; i < length; i++){	
			SpiWriteByte(FRAMSPINO,*data);
			data	+= 1;
		}
		tdelay(5);
		FramDisable();
	}
}

/************************************************************************************************************
** �� �� �� : FramRead
** �������� : ����洢��������
** ��    �� : addr ��ַ data ���ݻ����� length ���ݳ���
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void FramRead(uint16_t addr, uint8_t *data, uint16_t length)
{
	uint16_t i=0;

	FramEnable();
	tdelay(5);
	SpiWriteByte(FRAMSPINO,COMD_RD_MEMORY);
	SpiWriteByte(FRAMSPINO,(addr >> 8) & 0xff);
	SpiWriteByte(FRAMSPINO,addr & 0xff);
	for (i = 0; i < length; i++){
		*data	= SpiReadByte(FRAMSPINO);
		data++;
	}
	FramDisable();
	tdelay(5);
}
