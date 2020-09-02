#include"Board.h"
#include 	<LPC177x_8x.H>
#include <stdio.h>


/****************************************************************************\
 Function:  SpiInit
 Descript:  spi ��ʼ��
 Input:	    �ӣУɺ�
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/
void SpiInit(uint8_t no)
{
	if (no == 0)
	{
  		LPC_SC->PCONP |= (0x1 << 21);                                      
	/*	
		if (LPC_SC->PCLKSEL < 4) 
		{
			LPC_SC->PCLKSEL = 4;
		}
	*/
	
		LPC_IOCON->P0_15 &= ~0x07;
		LPC_IOCON->P0_15 |=  0x02;                                     
		LPC_IOCON->P0_17 &= ~0x07;
		LPC_IOCON->P0_17 |=  0x02;                                     
		LPC_IOCON->P0_18 &= ~0x07;    
		LPC_IOCON->P0_18 |=  0x02;                                      
		LPC_IOCON->P0_16 &= ~0x07;                                  

		
    	LPC_SSP0->CR0  = 0x0807;											
    	LPC_SSP0->CPSR = 0x08;			    		                                                                                         
    	LPC_SSP0->CR1  = 0x02;	
		
	}

	if (no == 1)
	{
  		LPC_SC->PCONP |= (0x1 << 10);                                      
	/*	
		if (LPC_SC->PCLKSEL < 4) 
		{
			LPC_SC->PCLKSEL = 4;
		}
	*/
	
		LPC_IOCON->P1_18 &= ~0x07;
		LPC_IOCON->P1_18 |=  0x05;                                     
		LPC_IOCON->P1_19 &= ~0x07;
		LPC_IOCON->P1_19 |=  0x05;                                     
		LPC_IOCON->P1_22 &= ~0x07;    
		LPC_IOCON->P1_22 |=  0x05;                                      
		LPC_IOCON->P1_26 &= ~0x07;                                  
		
    	LPC_SSP1->CR0  = 0x0807;											
    	LPC_SSP1->CPSR = 0x08;			    		                                                                                         
    	LPC_SSP1->CR1  = 0x02;		   			                              		   			                             
	}

	if (no == 2)
	{
  		LPC_SC->PCONP |= (0x1 << 20);                                      
	/*	
		if (LPC_SC->PCLKSEL < 4) 
		{
			LPC_SC->PCLKSEL = 4;
		}
	*/
	
		LPC_IOCON->P1_0 &= ~0x07;
		LPC_IOCON->P1_0 |=  0x04;                                     
		LPC_IOCON->P1_1 &= ~0x07;
		LPC_IOCON->P1_1 |=  0x04;                                      
		LPC_IOCON->P1_4 &= ~0x07;																		
		LPC_IOCON->P1_4 |=	0x04;
		LPC_IOCON->P1_8 &= ~0x07;                                  
		
		
    	LPC_SSP2->CR0  = 0x0807;											
    	LPC_SSP2->CPSR = 0x02;			    		                                                                                         
    	LPC_SSP2->CR1  = 0x02;		   			                              		   			                             
	}
}


/****************************************************************************\
 Function:  SpiWriteByte
 Descript:  ��ӣУ�дһ���ֽ�����
 Input:	    �ӣУɺţ��ֽ�����
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/

void SpiWriteByte(uint8_t no,uint8_t data)
{	
	if (no == 0)
	{
		while (!(LPC_SSP0->SR & 0x01));	    				                /* �ȴ�����FIFO�����ռ�   	*/
		LPC_SSP0->DR = data;
		while ((LPC_SSP0->SR & 0x10) == 0x10);	 				            /* �ȴ�����֡�������		*/
	}

	if (no == 1)
	{
		while (!(LPC_SSP1->SR & 0x01));	    				                /* �ȴ�����FIFO�����ռ�   	*/
		LPC_SSP1->DR = data;
		while ((LPC_SSP1->SR & 0x10) == 0x10);	 				            /* �ȴ�����֡�������		*/
	}

	if (no == 2)
	{
		while (!(LPC_SSP2->SR & 0x01));	    				                /* �ȴ�����FIFO�����ռ�   	*/
		LPC_SSP2->DR = data;
		while ((LPC_SSP2->SR & 0x10) == 0x10);	 				            /* �ȴ�����֡�������		*/
	}
}


/****************************************************************************\
 Function:  SpiReadByte
 Descript:  �ӣӣУɶ�һ���ֽ�����
 Input:	    �ӣУɺ�
 Output:    �� 
 Return:    ������һ������
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/

uint8_t SpiReadByte(uint8_t no)
{
	uint8_t data=0;

	if (no == 0)
	{
		while ((LPC_SSP0->SR & 0x04) == 0x04)
			data = LPC_SSP0->DR ;																			/* ��ս���FIFO�����ռ�   	*/   				                    							  
    	while ((LPC_SSP0->SR & 0x01) == 0);	    				            /* ��շ���FIFO�����ռ�   	*/
       	LPC_SSP0->DR = 0xff ;
    	while ((LPC_SSP0->SR & 0x10) == 0x10);	 				            /* �ȴ�����֡�������		*/
    	while ((LPC_SSP0->SR & 0x04) == 0x00);	 				            /* �ȴ�����֡�������		*/
        data = LPC_SSP0->DR ;
			
		return data;
		
	}

	if (no == 1)
	{
		while ((LPC_SSP1->SR & 0x04) == 0x04)
			data = LPC_SSP1->DR ;																			/* ��ս���FIFO�����ռ�   	*/   				                    							  
    	while ((LPC_SSP1->SR & 0x01) == 0);	    				            /* ��շ���FIFO�����ռ�   	*/
       	LPC_SSP1->DR = 0xff ;
    	while ((LPC_SSP1->SR & 0x10) == 0x10);	 				            /* �ȴ�����֡�������		*/
    	while ((LPC_SSP1->SR & 0x04) == 0x00);	 				            /* �ȴ�����֡�������		*/
        data = LPC_SSP1->DR ;
			
		return data;
	}

	if (no == 2)
	{
		while ((LPC_SSP2->SR & 0x04) == 0x04)
			data = LPC_SSP2->DR ;																			/* ��ս���FIFO�����ռ�   	*/   				                    							  
    	while ((LPC_SSP2->SR & 0x01) == 0);	    				            /* ��շ���FIFO�����ռ�   	*/
       	LPC_SSP2->DR = 0xff ;
    	while ((LPC_SSP2->SR & 0x10) == 0x10);	 				            /* �ȴ�����֡�������		*/
    	while ((LPC_SSP2->SR & 0x04) == 0x00);	 				            /* �ȴ�����֡�������		*/
        data = LPC_SSP2->DR ;
			
		return data;
	}
	
	return 0;

}

