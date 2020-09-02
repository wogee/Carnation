#include "Board.h"
#include 	<LPC177x_8x.H>
#include <stdio.h>
#include "UART.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "App.h"

static UARTBUF UartBuf[UART_MAX];



/****************************************************************************\
 Function:  UartInit
 Descript:  ���ڳ�ʼ��
 Input:	    ���ںţ�������
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/

void UartInit(uint8_t no,uint32_t bps)
{
	uint32_t data = ( PeripheralClock / 16 ) / (bps) ;
	
	if (no == DEV_UART0)
    {
		LPC_SC->PCONP |= 0x00000008;
    
	    LPC_IOCON->P0_2  &= ~0x07;
	    LPC_IOCON->P0_2  |= 1;                                     
	    LPC_IOCON->P0_3  &= ~0x07;
	    LPC_IOCON->P0_3  |= 1;                                     
	    
	    LPC_UART0->LCR = 0x83;                            
	    LPC_UART0->DLM = data / 256;
	    LPC_UART0->DLL = data % 256;
	    LPC_UART0->LCR = 0x03;                                         
	    LPC_UART0->FCR = 0x87; 

		LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;

	    NVIC_EnableIRQ(UART0_IRQn);

		memset((char *)&(UartBuf[DEV_UART0]),0,sizeof(UARTBUF));
	   
    }
	
    if (no == DEV_UART1)
    {	
   		LPC_SC->PCONP |= 0x00000010;

	    LPC_IOCON->P2_0 &= ~0x07;
	    LPC_IOCON->P2_0 |= 2;                                   
	    LPC_IOCON->P2_1 &= ~0x07;
	    LPC_IOCON->P2_1 |= 2;                                     
		
	    LPC_UART1->LCR = 0x83;
	    LPC_UART1->DLM = data / 256;
	    LPC_UART1->DLL = data % 256;
	    LPC_UART1->LCR = 0x03;                                        
	    LPC_UART1->FCR = 0x87;                                         
	    LPC_UART1->IER = IER_RBR | IER_THRE | IER_RLS;    
		NVIC_EnableIRQ(UART1_IRQn);
		
		memset((char *)&(UartBuf[DEV_UART1]),0,sizeof(UARTBUF));
    }
	
    if (no == DEV_UART2)
    {
		LPC_SC->PCONP |= 0x01000000;
		
		LPC_IOCON->P0_10 &= ~0x07;
		LPC_IOCON->P0_10 |= 1;                                    
		LPC_IOCON->P0_11 &= ~0x07;
		LPC_IOCON->P0_11 |= 1;                                     

		LPC_UART2->LCR = 0x9b;        
		LPC_UART2->DLM = data / 256;
		LPC_UART2->DLL = data % 256;
		LPC_UART2->LCR = 0x1b;   // żУ��   ���     �������������Ҫ������                           
		LPC_UART2->FCR = 0x87;                             
		LPC_UART2->IER = IER_RBR | IER_RLS;
		NVIC_EnableIRQ(UART2_IRQn);

		memset((char *)&(UartBuf[DEV_UART2]),0,sizeof(UARTBUF));
    }
	
    if (no == DEV_UART3)
    {	
   		LPC_SC->PCONP |= 0x02000000;
		
		LPC_IOCON->P0_25  &= ~0x07;
		LPC_IOCON->P0_25  |= 3;                                      
		LPC_IOCON->P0_26  &= ~0x07;
		LPC_IOCON->P0_26  |= 3;                                                 
	  
  		LPC_UART3->LCR  = 0x83;           
    	LPC_UART3->DLM  = data / 256;
    	LPC_UART3->DLL  = data % 256; 
    	LPC_UART3->LCR  = 0x03;                                                                     
  		LPC_UART3->FCR  = 0x87;                                                     
		LPC_UART3->IER = IER_RBR | IER_RLS;
		NVIC_EnableIRQ(UART3_IRQn);

		memset((char *)&(UartBuf[DEV_UART3]),0,sizeof(UARTBUF));
    }
	
	if (no == DEV_UART4)
    {	
   		LPC_SC->PCONP |= 0x00000100;
		
		LPC_IOCON->P5_3  &= ~0x07;
		LPC_IOCON->P5_3  |= 4;                                      
		LPC_IOCON->P5_4  &= ~0x07;
		LPC_IOCON->P5_4  |= 4;                                                 
	  
  		LPC_UART4->LCR  = 0x83;           
    	LPC_UART4->DLM  = data / 256;
    	LPC_UART4->DLL  = data % 256; 
    	LPC_UART4->LCR  = 0x03;                                                                     
  		LPC_UART4->FCR  = 0x87;                                                     
		LPC_UART4->IER = IER_RBR | IER_RLS;
		NVIC_EnableIRQ(UART4_IRQn);

		memset((char *)&(UartBuf[DEV_UART4]),0,sizeof(UARTBUF));
    }
}



/****************************************************************************\
 Function:  UartRead
 Descript:  ����������
 Input:	    ���ںţ����ݣ£գƣ�����
 Output:    �� 
 Return:    ʵ�ʶ��������ݳ���
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/
uint16_t UartRead(uint8_t no,uint8_t *data, uint16_t datalen)
{
	uint16_t r_len=0;

	if((no>=UART_MAX) || (data==NULL) || (datalen==0)) 
	{
        return 0;
	}

	while(UartBuf[no].Rd != UartBuf[no].Wr)
    {
		*data = UartBuf[no].buf[UartBuf[no].Rd];
		UartBuf[no].Rd = (UartBuf[no].Rd+1) % BUFFER_LEN;
		data++;
		if(++r_len >= datalen)
        {
            return r_len;
		}
	}

	return r_len;
}




/****************************************************************************\
 Function:  UartWrite
 Descript:  д��������
 Input:	    ���ںţ����ݣ£գƣ�����
 Output:    �� 
 Return:    ʵ��д�����ݳ���
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/
uint16_t UartWrite( uint8_t no, uint8_t *data, uint16_t datalen)
{
	uint16_t i=0;
	uint8_t err;
	
	if((no>=UART_MAX) || (data==NULL) || (datalen==0)) 
	{
		return 0; 
	}
    if (no == DEV_UART0)
    {
			OSSemPend(DubugSem, 1000, &err);
       for(i=0;i<datalen;i++)
			{
		    LPC_UART0->THR = *(data+i);                                                    
			while ((LPC_UART0->LSR & 0x20) == 0);   
        }
		 OSSemPost(DubugSem); 
    }


    if (no == DEV_UART1)
    {
        for(i=0;i<datalen;i++)
		{
		    LPC_UART1->THR = *(data+i);                                                    
			while ((LPC_UART1->LSR & 0x20) == 0);   
        }
    }

    if (no == DEV_UART2)
    {
        for(i=0;i<datalen;i++)
		{
		    LPC_UART2->THR = *(data+i);                                                    
			while ((LPC_UART2->LSR & 0x20) == 0);   
        }
    }

    if (no == DEV_UART3)
    {
        for(i=0;i<datalen;i++)
		{
		    LPC_UART3->THR = *(data+i);                                                    
			while ((LPC_UART3->LSR & 0x20) == 0);   
        }
    }

	  if (no == DEV_UART4)
    {
        for(i=0;i<datalen;i++)
		{
		    LPC_UART4->THR = *(data+i);                                                    
			while ((LPC_UART4->LSR & 0x20) == 0);   
        }
    }
   
	return datalen;
}

/****************************************************************************\
 Function:  UartFlush
 Descript:  �����������
 Input:	    ���ںţ�
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-23
 *****************************************************************************/
void UartFlush( uint8_t no)
{
	if(no >= UART_MAX) 
	{
		return ; 
	}

	UartBuf[no].Wr=UartBuf[no].Rd=0;
   
}




/****************************************************************************\
 Function:  UART0_IRQHandler
 Descript:  ���ڣ��ж�
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/

void UART0_IRQHandler (void)
{
    uint8_t value=0;

  	value = LPC_UART0->IIR;
    value >>= 1;                                                  
    value &= 0x07;

    if ( value == IIR_RLS ) 
	{                                       
      
        if ( LPC_UART0->LSR & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) 
		{
            return;
        }
        if ( LPC_UART0->LSR & LSR_RDR ) 
		{                                    
          	UartBuf[DEV_UART0].buf[UartBuf[DEV_UART0].Wr] =LPC_UART0->RBR;
			UartBuf[DEV_UART0].Wr = (UartBuf[DEV_UART0].Wr +1)%BUFFER_LEN;
			if(UartBuf[DEV_UART0].Wr == UartBuf[DEV_UART0].Rd ) 
			{
				UartBuf[DEV_UART0].Rd = (UartBuf[DEV_UART0].Rd +1)%BUFFER_LEN;
			}				
        }
    }
	
    else if ( value == IIR_RDA ) 
	{   
		UartBuf[DEV_UART0].buf[UartBuf[DEV_UART0].Wr] =LPC_UART0->RBR;
		UartBuf[DEV_UART0].Wr = (UartBuf[DEV_UART0].Wr +1)%BUFFER_LEN;
		if(UartBuf[DEV_UART0].Wr == UartBuf[DEV_UART0].Rd ) 
		{
			UartBuf[DEV_UART0].Rd = (UartBuf[DEV_UART0].Rd +1)%BUFFER_LEN;
		}	
    }

	
    else if ( value == IIR_CTI ) 
	{                                  
       	while ((LPC_UART0->LSR & 0x01) == 0x01)
		{  
			UartBuf[DEV_UART0].buf[UartBuf[DEV_UART0].Wr] =LPC_UART0->RBR;
			UartBuf[DEV_UART0].Wr = (UartBuf[DEV_UART0].Wr +1)%BUFFER_LEN;
			if(UartBuf[DEV_UART0].Wr == UartBuf[DEV_UART0].Rd ) 
			{
				UartBuf[DEV_UART0].Rd = (UartBuf[DEV_UART0].Rd +1)%BUFFER_LEN;
			}	
		}
			
    }

	
    else if ( value == IIR_THRE ) 
	{                                 
     
    }	

	else
	{
	}
}


/****************************************************************************\
 Function:  UART��_IRQHandler
 Descript:  ���ڣ��ж�
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/

void UART1_IRQHandler (void)
{
    uint8_t value=0;

  	value = LPC_UART1->IIR;
    value >>= 1;                                                  
    value &= 0x07;

    if ( value == IIR_RLS ) 
	{                                       
      
        if ( LPC_UART1->LSR & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) 
		{
            return;
        }
        if ( LPC_UART1->LSR & LSR_RDR ) 
		{                                    
          	UartBuf[DEV_UART1].buf[UartBuf[DEV_UART1].Wr] =LPC_UART1->RBR;
			UartBuf[DEV_UART1].Wr = (UartBuf[DEV_UART1].Wr +1)%BUFFER_LEN;
			if(UartBuf[DEV_UART1].Wr == UartBuf[DEV_UART1].Rd ) 
			{
				UartBuf[DEV_UART1].Rd = (UartBuf[DEV_UART1].Rd +1)%BUFFER_LEN;
			}				
        }
    }
	
    else if ( value == IIR_RDA ) 
	{   
		UartBuf[DEV_UART1].buf[UartBuf[DEV_UART1].Wr] =LPC_UART1->RBR;
		UartBuf[DEV_UART1].Wr = (UartBuf[DEV_UART1].Wr +1)%BUFFER_LEN;
		if(UartBuf[DEV_UART1].Wr == UartBuf[DEV_UART1].Rd ) 
		{
			UartBuf[DEV_UART1].Rd = (UartBuf[DEV_UART1].Rd +1)%BUFFER_LEN;
		}	
    }

	
    else if ( value == IIR_CTI ) 
	{                                  
       	while ((LPC_UART1->LSR & 0x01) == 0x01)
		{  
			UartBuf[DEV_UART1].buf[UartBuf[DEV_UART1].Wr] =LPC_UART1->RBR;
			UartBuf[DEV_UART1].Wr = (UartBuf[DEV_UART1].Wr +1)%BUFFER_LEN;
			if(UartBuf[DEV_UART1].Wr == UartBuf[DEV_UART1].Rd ) 
			{
				UartBuf[DEV_UART1].Rd = (UartBuf[DEV_UART1].Rd +1)%BUFFER_LEN;
			}	
		}
			
    }

	
    else if ( value == IIR_THRE ) 
	{                                 
     
    }	

	else
	{
	}
}



/****************************************************************************\
 Function:  UART��_IRQHandler
 Descript:  ���ڣ��ж�
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/

void UART2_IRQHandler (void)
{
    uint8_t value=0;

  	value = LPC_UART2->IIR;
    value >>= 1;                                                  
    value &= 0x07;

    if ( value == IIR_RLS ) 
	{                                       
      
        if ( LPC_UART2->LSR & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) 
		{
            return;
        }
        if ( LPC_UART2->LSR & LSR_RDR ) 
		{                                    
          	UartBuf[DEV_UART2].buf[UartBuf[DEV_UART2].Wr] =LPC_UART2->RBR;
			UartBuf[DEV_UART2].Wr = (UartBuf[DEV_UART2].Wr +1)%BUFFER_LEN;
			if(UartBuf[DEV_UART2].Wr == UartBuf[DEV_UART2].Rd ) 
			{
				UartBuf[DEV_UART2].Rd = (UartBuf[DEV_UART2].Rd +1)%BUFFER_LEN;
			}				
        }
    }
	
    else if ( value == IIR_RDA ) 
	{   
		UartBuf[DEV_UART2].buf[UartBuf[DEV_UART2].Wr] =LPC_UART2->RBR;
		UartBuf[DEV_UART2].Wr = (UartBuf[DEV_UART2].Wr +1)%BUFFER_LEN;
		if(UartBuf[DEV_UART2].Wr == UartBuf[DEV_UART2].Rd ) 
		{
			UartBuf[DEV_UART2].Rd = (UartBuf[DEV_UART2].Rd +1)%BUFFER_LEN;
		}	
    }

	
    else if ( value == IIR_CTI ) 
	{                                  
       	while ((LPC_UART2->LSR & 0x01) == 0x01)
		{  
			UartBuf[DEV_UART2].buf[UartBuf[DEV_UART2].Wr] =LPC_UART2->RBR;
			UartBuf[DEV_UART2].Wr = (UartBuf[DEV_UART2].Wr +1)%BUFFER_LEN;
			if(UartBuf[DEV_UART2].Wr == UartBuf[DEV_UART2].Rd ) 
			{
				UartBuf[DEV_UART2].Rd = (UartBuf[DEV_UART2].Rd +1)%BUFFER_LEN;
			}	
		}
			
    }

	
    else if ( value == IIR_THRE ) 
	{                                 
     
    }	

	else
	{
	}
}


/****************************************************************************\
 Function:  UART3_IRQHandler
 Descript:  ����3�ж�
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/

void UART3_IRQHandler (void)
{
    uint8_t value=0;

  	value = LPC_UART3->IIR;
    value >>= 1;                                                  
    value &= 0x07;

    if ( value == IIR_RLS ) 
	{                                       
      
        if ( LPC_UART3->LSR & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) 
		{
            return;
        }
        if ( LPC_UART3->LSR & LSR_RDR ) 
		{                                    
          	UartBuf[DEV_UART3].buf[UartBuf[DEV_UART3].Wr] =LPC_UART3->RBR;
			UartBuf[DEV_UART3].Wr = (UartBuf[DEV_UART3].Wr +1)%BUFFER_LEN;
			if(UartBuf[DEV_UART3].Wr == UartBuf[DEV_UART3].Rd ) 
			{
				UartBuf[DEV_UART3].Rd = (UartBuf[DEV_UART3].Rd +1)%BUFFER_LEN;
			}				
        }
    }
	
    else if ( value == IIR_RDA ) 
	{   
		UartBuf[DEV_UART3].buf[UartBuf[DEV_UART3].Wr] =LPC_UART3->RBR;
		UartBuf[DEV_UART3].Wr = (UartBuf[DEV_UART3].Wr +1)%BUFFER_LEN;
		if(UartBuf[DEV_UART3].Wr == UartBuf[DEV_UART3].Rd ) 
		{
			UartBuf[DEV_UART3].Rd = (UartBuf[DEV_UART3].Rd +1)%BUFFER_LEN;
		}	
    }

	
    else if ( value == IIR_CTI ) 
	{                                  
       	while ((LPC_UART3->LSR & 0x01) == 0x01)
		{  
			UartBuf[DEV_UART3].buf[UartBuf[DEV_UART3].Wr] =LPC_UART3->RBR;
			UartBuf[DEV_UART3].Wr = (UartBuf[DEV_UART3].Wr +1)%BUFFER_LEN;
			if(UartBuf[DEV_UART3].Wr == UartBuf[DEV_UART3].Rd ) 
			{
				UartBuf[DEV_UART3].Rd = (UartBuf[DEV_UART3].Rd +1)%BUFFER_LEN;
			}	
		}
			
    }

	
    else if ( value == IIR_THRE ) 
	{                                 
     
    }	

	else
	{
	}
}

/****************************************************************************\
 Function:  UART4_IRQHandler
 Descript:  ����4�ж�
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/

void UART4_IRQHandler (void)
{
    uint8_t value=0;

  	value = LPC_UART4->IIR;
    value >>= 1;                                                  
    value &= 0x07;

    if ( value == IIR_RLS ) 
	{                                       
      
        if ( LPC_UART4->LSR & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) 
		{
            return;
        }
        if ( LPC_UART4->LSR & LSR_RDR ) 
		{                                    
          	UartBuf[DEV_UART4].buf[UartBuf[DEV_UART4].Wr] =LPC_UART4->RBR;
			UartBuf[DEV_UART4].Wr = (UartBuf[DEV_UART4].Wr +1)%BUFFER_LEN;
			if(UartBuf[DEV_UART4].Wr == UartBuf[DEV_UART4].Rd ) 
			{
				UartBuf[DEV_UART4].Rd = (UartBuf[DEV_UART4].Rd +1)%BUFFER_LEN;
			}				
        }
    }
	
    else if ( value == IIR_RDA ) 
	{   
		UartBuf[DEV_UART4].buf[UartBuf[DEV_UART4].Wr] =LPC_UART4->RBR;
		UartBuf[DEV_UART4].Wr = (UartBuf[DEV_UART4].Wr +1)%BUFFER_LEN;
		if(UartBuf[DEV_UART4].Wr == UartBuf[DEV_UART4].Rd ) 
		{
			UartBuf[DEV_UART4].Rd = (UartBuf[DEV_UART4].Rd +1)%BUFFER_LEN;
		}	
    }

	
    else if ( value == IIR_CTI ) 
	{                                  
       	while ((LPC_UART4->LSR & 0x01) == 0x01)
		{  
			UartBuf[DEV_UART4].buf[UartBuf[DEV_UART4].Wr] =LPC_UART4->RBR;
			UartBuf[DEV_UART4].Wr = (UartBuf[DEV_UART4].Wr +1)%BUFFER_LEN;
			if(UartBuf[DEV_UART4].Wr == UartBuf[DEV_UART4].Rd ) 
			{
				UartBuf[DEV_UART4].Rd = (UartBuf[DEV_UART4].Rd +1)%BUFFER_LEN;
			}	
		}
			
    }

	
    else if ( value == IIR_THRE ) 
	{                                 
     
    }	

	else
	{
	}
}





/****************************************************************************\
 Function:  print
 Descript:  ��װ�Ĵ�ӡ����
 Input:	    ���
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-18
 *****************************************************************************/

void print( const char *format,...)
{
	uint8_t   data[10]={0},*str=NULL;
	uint32_t  d=0;
	va_list ap;

	va_start(ap,format);

	while(*format != '\0')
	{
		if (*format  != '%')      //û�аٷֺž�ֱ�Ӵ�ӡ
		{
		    UartWrite(DEV_UART0,(unsigned char *)format,1);
            format++;
		}

		else                           //�аٷֺžͰ�����ӡ
		{
			
            if ((*(format+1) == 'd') || (*(format+1) == 'D'))   //����
			{
				d=va_arg(ap,int);
				memset((char *)data,0,sizeof(data));		
				sprintf((char *)data,"%d",d);
                UartWrite(DEV_UART0,data,strlen((char *)data));
			}

			else if ((*(format+1) == 'x') || (*(format+1) == 'X'))                  //ʮ������  (Ĭ�ϴ�ӡ2λ)
			{
				d=va_arg(ap,int);
				memset((char *)data,0,sizeof(data));
				sprintf((char *)data,"%02X",d);
				UartWrite(DEV_UART0,data,strlen((char *)data));
			}

			else if ((*(format+1) == 's') || (*(format+1) == 'S'))                                              //�ַ�����
			{
				str = (uint8_t *)va_arg(ap, char*);
				UartWrite(DEV_UART0,str,strlen((char *)str));
			}
            else
            {
                format += 2;  
            }
            
			format += 2;   
		}
	}

	va_end(ap); 
    
}

void printx(uint8_t *data,uint16_t datalen)
{
    uint16_t i=0;
    for(i=0;i<datalen;i++)
    {
        print("%X ",data[i]);
    }

    print("\r\n");
}











