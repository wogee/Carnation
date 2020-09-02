#include 	<LPC177x_8x.H>
#include "Board.h"
#include <stdio.h>

static uint32_t TimerTick=0;


/****************************************************************************\
 Function:  TimerInit
 Descript:  ��ʱ����ʼ��  
 Input:	    ��ʱ���ţ������ж�ֵ
 Output:    �� 
 Return:    0
 Author:    quanhouwei
 Datetime:  17-08-17
 *****************************************************************************/
void TimerInit(uint8_t no,uint16_t time)
{
	if (no == 0)
    {
    	LPC_SC->PCONP  |= (1 << 1);    //�򿪹��ʿտ͹�ֵ                                     

    	LPC_TIM0->TCR  = 0x02;         // ��λ��ʱ�� 
    	LPC_TIM0->IR   = 1;
    	LPC_TIM0->CTCR = 0;
    	LPC_TIM0->TC   = 0;
    	LPC_TIM0->PR   = time-1;

    	LPC_TIM0->MR0  = PeripheralClock/1000;

    	LPC_TIM0->MCR  = 0x03;       // MRO��TC֮ƥ��ʱ�������ж�                                              

    	LPC_TIM0->TCR  = 0x01;       // ����      

		NVIC_EnableIRQ(TIMER0_IRQn);                                        
    	NVIC_SetPriority(TIMER0_IRQn, 3);
	}

	if (no == 1)
	{
	}
 
}


void TIMER0_IRQHandler(void)
{
	LPC_TIM0->IR = 0x01;        // ���жϱ�־
	TimerTick++;
}

uint32_t TimerRead(void)
{
	return TimerTick;
}











