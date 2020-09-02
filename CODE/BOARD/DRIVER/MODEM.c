#include "Board.h"
#include "MODEM.h"
#include "GPIO.h"
#include <string.h>
#include "UART.h"
#include "includes.h"
#include "MyAlgLib.h"
#include <stdio.h>
#include "Timer.h"

static MODEMRCVMSG  ModemRcvMsg;      // ����

/************************************************************************************************************
** �� �� �� : ModemInit
** �������� : ����ģ���ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ModemInit(void)
{
	UartInit(GPRSUARTNO, 115200); //����

//	GpioSetDir(GPIO_GPRSRST, OUTPUT); //��λ��
//	GpioSetLev(GPIO_GPRSRST, LOW);

//	GpioSetDir(GPIO_GPRSPWR, OUTPUT); //��Դ
//	GpioSetLev(GPIO_GPRSPWR, HIGH);
//	OSTimeDlyHMSM (0, 0, 10, 0); //��ʱ����ģ��ŵ�Ÿɾ�
//	GpioSetLev(GPIO_GPRSPWR, LOW);
//	OSTimeDlyHMSM (0, 0, 10, 0); //��ʱ���ȴ�ģ������
	


	GpioSetDir(GPIO_GPRSPWR, OUTPUT); //��Դ
	GpioSetLev(GPIO_GPRSPWR, HIGH);
	OSTimeDlyHMSM (0, 0, 10, 0); //��ʱ����ģ��ŵ�Ÿɾ�
	GpioSetLev(GPIO_GPRSPWR, LOW);
	OSTimeDlyHMSM (0, 0, 3, 0); //��ʱ���ȴ�ģ������
	
	GpioSetDir(GPIO_GPRSRST, OUTPUT); //��λ��
	GpioSetLev(GPIO_GPRSRST, HIGH);
	OSTimeDlyHMSM (0, 0, 1, 0);
	GpioSetLev(GPIO_GPRSRST, LOW);
	OSTimeDlyHMSM (0, 0, 1, 0);
	
}

/************************************************************************************************************
** �� �� �� : ModemConfig
** �������� : ����ģ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
uint8_t ModemConfig(void)
{
//	if (ModemWrWaitRd("ATE","OK",3,NULL) != ATOK)     		// AT ����
//	{
//		return 0;
//	}
	if (ModemWrWaitRd("ATI","OK",1,NULL) != ATOK)     		// AT ����
	{
		return 0;
	}

	if (ModemWrWaitRd("AT+CGMR","OK",3,NULL) != ATOK)     	// ��汾��
	{
		return 0;
	}

	if (ModemWrWaitRd("AT+CPIN?","OK",5,NULL) != ATOK)  	// �鿨
	{
		return 0;
	}

	if (ModemWrWaitRd("ATE0","OK",3,NULL) != ATOK)    		// �رջ���
	{
		return 0;
	}

	if (ModemWrWaitRd("AT+CFUN=1","OK",3,NULL) != ATOK) 	// ����ȫ����
	{
		return 0;
	}

	if (ModemWrWaitRd("AT+ZSNT=0,0,0","OK",3,NULL) != ATOK)  // �Զ�ѡ������
	{
		return 0;
	}

	return 1;
}


/****************************************************************************\
 Function:  ModemConnect
 Descript:  ����ģ�����ӷ�����
 Input:	    SOCKID  ����(TCP/UDP)   APN,IP PORT	
 Output:    �� 
 Return:    1 OK  0 fail
 Author:    quanhouwei
 Datetime:  17-09-07
 *****************************************************************************/
uint8_t ModemConnect(uint8_t sockid,uint8_t type,uint8_t *apn,uint8_t *dip,uint16_t dport,uint16_t port)
{
	uint8_t sendbuf[128]={0},rcvbuf[128]={0};
	uint8_t *p=NULL,csq=0,creg=0,cgreg=0;
	
	if ((sockid>=GPRSMAXSOCKET) || (apn==NULL) || (dip==NULL) || (dport==0) || (port==0) )
	{
		return 0;
	}

	if (ModemWrWaitRd("AT","OK",3,NULL) != ATOK)		// AT ����
	{
		return 0;
	}

	if (ModemWrWaitRd("AT+CPIN?","OK",5,NULL) != ATOK)  // �鿨
	{
		return 0;
	}

	ModemDisCon(sockid);          						// �Ͽ�����

	
	if (ModemWrWaitRd("AT+CSQ","OK",3,rcvbuf) != ATOK) // ���ź�
	{
		return 0;
	}
	p=(uint8_t *)strstr((char *)rcvbuf,"+CSQ: ");
    if(p == NULL)
	{
		return 0;
	}
	p+=strlen((char *)"+CSQ: ");
    csq=MyAtoi(p);
    if (csq <=5)
    {
       return 0;
    }


	if (ModemWrWaitRd("AT+CREG?","OK",3,rcvbuf) != ATOK) // ���Ƿ�ע������ 1 ��¼��������  5 ��¼��������
  	{
  		return 0;
	}
    p=(uint8_t *)strstr((char *)rcvbuf,"+CREG: ");
	if (p == NULL)
	{
		return 0;
	}
    p+=strlen((char *)"+CREG: ");
    p+=2;
    creg=*p-'0';
    if ((creg!=1) && (creg!=5))  
    {
        return 0;
    }


	if (ModemWrWaitRd("AT+CGREG?","OK",180,rcvbuf) != ATOK) // �Ƿ�������
	{
		return 0;
	}
	p=(uint8_t *)strstr((char *)rcvbuf,"+CGREG: ");
	if (p == NULL)
	{
		return 0;
	}
	p+=strlen((char *)"+CGREG: ");
	p+=2;
	cgreg=*p-'0';
	if (cgreg != 1)
	{
		return 0;
	}


	sprintf((char *)sendbuf,"AT+ZIPCFG=%s",apn);     // APN
	if (ModemWrWaitRd(sendbuf,"OK",3,NULL) != ATOK)
	{
		return 0;
	}

	if (ModemWrWaitRd("AT+ZIPCALL=1","OK",150,NULL) != ATOK)  // ������
	{
		return 0;
	}


	sprintf((char *)sendbuf,"AT+ZIPOPEN=%d,%d,%s,%d,%d",sockid,type,dip,dport,port);	 // ����
	if (ModemWrWaitRd(sendbuf,"OK",150,NULL) != ATOK)
	{
		return 0;
	}

	if (ModemGetConSt(sockid) == 1)    // ��ȡ����״̬
	{
		return 1;
	}
	else
	{
		return 0;
	}

}



/****************************************************************************\
 Function:  ModemDisCon
 Descript:  �Ͽ�����
 Input:	    SOCKID 	
 Output:    �� 
 Return:    1 OK  0 fail
 Author:    quanhouwei
 Datetime:  17-09-07
 *****************************************************************************/
uint8_t ModemDisCon(uint8_t sockid)
{
	uint8_t sendbuf[128]={0};

	if (sockid >= GPRSMAXSOCKET)
	{
		return 0;
	}
	
	sprintf((char *)sendbuf,"AT+ZIPCLOSE=%d",sockid);
	if (ModemWrWaitRd(sendbuf,"OK",3,NULL) != ATOK)
	{
		return 0;
	}

	if (ModemWrWaitRd("AT+ZIPCALL=0","OK",3,NULL) != ATOK)
	{
		return 0;
	}

	return 1;
}


/****************************************************************************\
 Function:  ModemGetLinkSt
 Descript:  ��ȡ����״̬
 Input:	    SOCKID 	
 Output:    �� 
 Return:    1 ����  0 δ����
 Author:    quanhouwei
 Datetime:  17-09-07
 *****************************************************************************/
uint8_t ModemGetConSt(uint8_t sockid)
{
	uint8_t *p=NULL,sendbuf[128]={0},dat8=0;

	if (sockid >= GPRSMAXSOCKET)
	{
		return 0;
	}
	
	sprintf((char *)sendbuf,"AT+ZIPSTAT=%d",sockid);
	if (ModemWrWaitRd(sendbuf,"OK",3,NULL) != ATOK)
	{
		return 0;
	}
	p=(uint8_t *)strstr((char *)p,"+ZIPSTAT: ");
    if(p == NULL)
	{
		return 0;
	}
	p+=strlen((char *)"+ZIPSTAT: ");
    dat8=*p-'0';
	if (dat8 != sockid)
	{
		return 0;
	}
	p+=2;
	dat8=*p-'0';
	if (dat8 == 1)
	{
		return 1;
	}

	return 0;
}



/****************************************************************************\
 Function:  ModemSendData
 Descript:  ��������
 Input:	    SOCKID 	����  ���ݳ���
 Output:    �� 
 Return:    1 OK  0 fail
 Author:    quanhouwei
 Datetime:  17-09-07
 *****************************************************************************/
uint8_t ModemSendData(uint8_t sockid,uint8_t *data,uint16_t datalen)
{
	uint8_t sendbuf[128]={0};

	if (ModemWrWaitRd("AT","OK",3,NULL) != ATOK)
	{
		return 0;
	}

	if (ModemWrWaitRd("AT+CPIN?","OK",3,NULL) != ATOK)
	{
		return 0;
	}

	if (ModemGetConSt(sockid) == 0)     // ��ȡ����״̬
	{
		return 0;
	}
	
	sprintf((char *)sendbuf,"AT+ZIPSENDRAW=%d,%d,0",sockid,datalen);
	if (ModemWrWaitRd(sendbuf,">",3,NULL) == ATOK)
	{
		UartWrite(GPRSUARTNO,data,datalen);

		return 1;
	}
	else
	{
		return 0;
	}
}


/****************************************************************************\
 Function:  ModemRcvData
 Descript:  ��������
 Input:	    SOCKID 	����  
 Output:    �� 
 Return:    �������ݵĳ���
 Author:    quanhouwei
 Datetime:  17-09-07
 *****************************************************************************/
uint16_t ModemRcvData(uint8_t sockid,uint8_t *outbuf)
{
	static  uint8_t	step=0;
	static  uint16_t len=0;
//	static  uint8_t	type=0;
	static  uint32_t time=0;
	static  uint8_t	rcvbuf[MODEMBUFLEN]={0};
	static  uint16_t rcvlen=0;
	uint8_t	data=0,dat8=0;
	

	while(UartRead(GPRSUARTNO,&dat8,1))        
  	{
       	ModemBufWrByte(dat8);        // �ŵ����ڻ�����
  	}
	
	if (TimerRead()-time >= T1S*3)	 // 3S������δ������˳�������	
	{
		step=0;
	}
	
	while(ModemBufRdByte(&data))		 // �Ӵ��ڻ�����ȡ����
	{
		time = TimerRead();	 //��¼ʱ��
		switch(step)
	 	{
			case 0:
				if (data == '+')				 // ͷ
				{
					step++;
				}
				else
				{
					step=0;
				}
				break;
				 
			case 1:
				if (data == 'Z')
				{
					step++;
				}
				else
				{
					step=0;
				}
				break;
				 
			case 2:
				if (data == 'I')
				{
					step++;
				}
				else
				{
					step=0;
				}
				break;
				 
			case 3:
				if (data == 'P')
				{
					step++;
				}
				else
				{
					step=0;
				}
				break;
				 
			case 4:
				if (data == 'R')
				{
					step++;
				}
				else
				{
					step=0;
				}
				break;
				 
			case 5:
				if (data == 'E')
				{
					step++;
				}
				else
				{
					step=0;
				}
				break;
				 
			case 6:
				if (data == 'C')
				{
					step++;
				}
				else
				{
					step=0;
				}
				break;
				 
			case 7:
				if (data == 'V')
				{
					step++;
				}
				else
				{
					step=0;
				}
				break;
				 
			case 8:
				if (data == ':')
				{
//					type=0;
					step++;
				}
				else
				{
					step=0;
				}
				break;
	
			case 9:
//				type=data-'0';
				break;
	
			case 10:
				if (data == ',')
				{
					step++;
				}
				else
				{
					step=0;
				}
				break;

			case 11:
				if (data == ',')
			 	{
			 		len=0;
					step++;
			 	}
				break;
				 
			  case 12:						 // ���� ()
				 if ((data<='9') && (data>='0'))
				 {	
					 len=len*10+(data-'0');
				 }
				 else
				 {
					 if ((len==0) || (len>= MODEMBUFLEN))
					 {
						 step=0;
					 }
					 else
					 { 
						 step++;
					 }
				 }
				 break;
	
			   case 13:  
				 if (data == ',')	
				 {
					 rcvlen=0;
					 step++;
				 }
				 else
				 {
					 step=0;
				 }
				 break;
				 
	
	
			  case 14:			  //����
				 rcvbuf[rcvlen++]=data;
				 if (rcvlen == len)
				 {
					 step=0;
					 memcpy(outbuf,rcvbuf,rcvlen);
					 return rcvlen;
					
				 }
				 break;
	
			  default:
				 step=0;
				 break;
				 
		  }
	 }
	
	 return 1;
	
//	return NODATA;
}



	























/************************************************************************************************************
** �� �� �� : ModemWrWaitRd
** �������� : ����AT����ȴ�����ֵ
** ��    �� : data �������� ack ����ֵ time ��ʱʱ�� s
** ��    �� : outbuf ���ص�����
** ��    �� :	��
*************************************************************************************************************
*/
static uint8_t ModemWrWaitRd(uint8_t *data,uint8_t* ack,uint16_t time,uint8_t *outbuf)
{
	uint16_t len=0,i=0;
	uint8_t  rcvbuf[MODEMBUFLEN]={0};
	uint8_t  dat8=0;

	//�������
	while (UartRead(GPRSUARTNO, &dat8, 1));

	UartWrite(GPRSUARTNO, data, strlen((char *)data));
	UartWrite(GPRSUARTNO, "\x0D", 1);
	UartWrite(GPRSUARTNO, "\x0A", 1);

	print("send:%s\r\n",data);

	for (i = 0; i < (time * 1000); i++){
		OSTimeDlyHMSM (0, 0, 0, 1);
		while (UartRead(GPRSUARTNO, &dat8, 1)){
			ModemBufWrByte(dat8);        // �ŵ����ڻ�����

			if (len <= MODEMBUFLEN){
				rcvbuf[len++] = dat8;
			}
			if(strstr((char *)rcvbuf,(char *)ack)){// �յ���ȷӦ��
				print("rcv:%s\r\n",rcvbuf);
				return ATOK;
			}
			if(strstr((char *)rcvbuf,"ERROR")){// �յ���ȷӦ��
				print("rcv:%s\r\n",rcvbuf);
				return ATERR;
			}
		}
	}

	return ATTIMEOUT;
}


/****************************************************************************\
 Function:  ModemBufWrByte
 Descript:  дһ���ֽ����ݵ�������
 Input:	    ��д����ֽ�����
 Output:    ���ص�����
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-24
 *****************************************************************************/
static void ModemBufWrByte(uint8_t data)
{
	if ((ModemRcvMsg.in>=MODEMBUFLEN)  || (ModemRcvMsg.out>=MODEMBUFLEN))    // ����
    {
        ModemRcvMsg.in = 0;
        ModemRcvMsg.out = 0;
    }

    ModemRcvMsg.buf[ModemRcvMsg.in] = data;

    if (++ModemRcvMsg.in >= MODEMBUFLEN)
    {
        ModemRcvMsg.in = 0;
    }

    if (ModemRcvMsg.in == ModemRcvMsg.out)
    {
        ModemRcvMsg.out++;
        if (ModemRcvMsg.out >= MODEMBUFLEN)
        {
            ModemRcvMsg.out = 0;
        } 
    } 
}



/****************************************************************************\
 Function:  ModemBufRdByte
 Descript: �ӻ�������һ���ֽ�����
 Input:	    ��
 Output:   ����������
 Return:   ����δ�������ݡ���������������
 Author:    quanhouwei
 Datetime:  17-08-24
 *****************************************************************************/
static uint8_t ModemBufRdByte(uint8_t *data)
{
	if ((ModemRcvMsg.in>=MODEMBUFLEN)  || (ModemRcvMsg.out>=MODEMBUFLEN))   // ����
    {
        ModemRcvMsg.in = 0;
        ModemRcvMsg.out = 0;
        return 0;
    }  

    if (ModemRcvMsg.in == ModemRcvMsg.out)      // û������
    {
        return 0;
    }


    *data = ModemRcvMsg.buf[ModemRcvMsg.out];

    ModemRcvMsg.out++;
    if (ModemRcvMsg.out >= MODEMBUFLEN)
    {
       ModemRcvMsg.out = 0;
    }

    return 1;
	
}

