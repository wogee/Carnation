#include <stdint.h>
#include "relay.h"
#include "GPIO.h"
#include "Board.h"

/************************************************************************************************************
** �� �� �� : RelayInit
** �������� : �̵�����ʼ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void RelayInit(void)
{
	GpioSetDir(GPIO_OUTPUT1, OUTPUT);
	GpioSetDir(GPIO_OUTPUT2, OUTPUT);
	GpioSetDir(GPIO_OUTPUT3, OUTPUT);
	GpioSetDir(GPIO_OUTPUT4, OUTPUT);
	
	GpioSetLev(GPIO_OUTPUT1,LOW);
	GpioSetLev(GPIO_OUTPUT2,LOW);
	GpioSetLev(GPIO_OUTPUT3,LOW);
	GpioSetLev(GPIO_OUTPUT4,LOW);
	
// ����
//	for (i = 0; i< 4; i++){
//		SetRelayState(i, 1);
//		SetRelayState(i, 0);
//		SetRelayState(i, 1);
//		SetRelayState(i, 0);
//	}
}

/************************************************************************************************************
** �� �� �� : SetRelayState
** �������� : ���ü̵���״̬����
** ��    �� : num�̵������� 0-3 state�̵���״̬ 0�Ͽ� 1����
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SetRelayState(uint8_t num, uint8_t state)
{
	if (state == 1){
		if (num == 0){
			GpioSetLev(GPIO_OUTPUT1, HIGH);
		} else if (num == 1){
			GpioSetLev(GPIO_OUTPUT2, HIGH);
		} else if (num == 2){
			GpioSetLev(GPIO_OUTPUT3, HIGH);
		} else if (num == 3){
			GpioSetLev(GPIO_OUTPUT4, HIGH);
		} 
	} else {
		if (num == 0){
			GpioSetLev(GPIO_OUTPUT1, LOW);
		} else if (num == 1){
			GpioSetLev(GPIO_OUTPUT2, LOW);
		} else if (num == 2){
			GpioSetLev(GPIO_OUTPUT3, LOW);
		} else if (num == 3){
			GpioSetLev(GPIO_OUTPUT4, LOW);
		} 
	}
}
