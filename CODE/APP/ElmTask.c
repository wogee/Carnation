#include <stdint.h>
#include "App.h"
#include "ElmTask.h"
#include "RTC.h"
#include "RS485.h"
#include "UART.h"
#include "Timer.h"
#include "Board.h"
#include "Para.h"
#include "Myalglib.h"

static ELMINFO  ElmInfo[ELMMAXCNT];

/************************************************************************************************************
** �� �� �� : ElmCheckMsg
** �������� : У������
** ��    �� : ���ݣ����� �ṹ������
** ��    �� : ��
** ��    �� :	0 FAIL  1 OK
*************************************************************************************************************
*/
static uint8_t ElmCheckMsg(uint8_t *data,uint16_t datalen,ElmRcvMsg *msg)
{
	uint8_t  checksum=0,len=0;
	uint16_t i=0;
	
	if ((data==NULL) || (datalen<12) || (datalen>200) || (msg==NULL)){
		return 0;
	}
	
	for (i = 0; i < datalen; i++){
		if (data[i] != 0xfe)
			break;
	}

	data += i;

	if (data[len++] != ELMHEAD)					// ͷ(1)
	{
		return 0;
	}

	memcpy(msg->addr,data+len,6);	 			// ��ַ (6)	
	len+=6;

	if (*(data+len) != ELMHEAD)					// ͷ (1)
	{
		return 0;
	}
	len++;

	msg->ctlcode=*(data+len);					// ������ (1)
	len++;

	msg->buflen=*(data+len);					// �����򳤶� (1)
	if (msg->buflen >200)
	{
		return 0;
	}
	len++;

	if (msg->buflen != 0)						// ������
	{
		for (i=0;i<msg->buflen;i++)
		{
			msg->buf[i]=*(data+len+i)-0x33;
		}
		len+=msg->buflen;	
	}
		
	checksum=MyCum8Sum(data,len);			// У�� (1)
	
	if (checksum != *(data+len))
	{
		return 0;
	}
	len++;

	if (*(data+len) != ELMTAIL)					// β(1)
	{
		return 0;
	}

	return 1;
}

/************************************************************************************************************
** �� �� �� : ElmSenddRv
** �������� : �������ݲ��ȴ�����
** ��    �� : �跢�͵����ݺͳ���
** ��    �� : ��
** ��    �� :	0 FAIL  1 OK
*************************************************************************************************************
*/
static uint8_t ElmSenddRv(uint8_t id,uint8_t *data,uint8_t datalen,ElmRcvMsg *msg)
{
	uint8_t  tmpbuf[100]={0},rcvbuf[128]={0};
	uint16_t tmplen=0,rcvlen=0;
	uint32_t timer;
	
	if ((data==NULL) || (datalen==0))
	{
		return 0;
	}

	RS485Flush();          			// ��մ��ڻ���
	RS485Write(data,datalen);		// ��������

	timer = TimerRead();
	while (TimerRead() - timer < T1S * 3){
		DelayMs(1);
		
		tmplen=RS485Read(tmpbuf,100);
		if (tmplen != 0)
		{
			if (rcvlen+tmplen >= sizeof(rcvbuf))
			{
				return 0;
			}
			memcpy(rcvbuf+rcvlen,tmpbuf,tmplen);
			rcvlen += tmplen;
		
			if (ElmCheckMsg(rcvbuf,rcvlen,msg) == 1)
			{
				//break;
				return 1;
			}
		}
	}
	return 0;
}

/************************************************************************************************************
** �� �� �� : ElmMakeMsg
** �������� : �������
** ��    �� : ��ַ�������룬�������ͣ����ݺͳ���
** ��    �� : ��
** ��    �� :	��������ݳ���
*************************************************************************************************************
*/
static uint16_t ElmMakeMsg(uint8_t *addr,uint8_t ctlcode,uint32_t type,uint8_t *data,uint16_t datalen,uint8_t *outbuf)
{
	uint16_t outlen=0,i=0;

	if ((addr==NULL) || (outbuf==NULL) || (datalen>46))
	{
		return 0;
	}
	
	outbuf[outlen++] = 0xfe;
	outbuf[outlen++] = 0xfe;
	outbuf[outlen++] = 0xfe;
	outbuf[outlen++] = 0xfe;

	outbuf[outlen++]=ELMHEAD;  						// ͷ (1)

	memcpy(outbuf+outlen,addr,6);							// ��ַ (6)
	outlen+=6;

	outbuf[outlen++]=ELMHEAD;  						// ͷ (1)

	outbuf[outlen++]=ctlcode;  						// ������ (1)

	outbuf[outlen++]=4+datalen;        				// ���� (1)

    outbuf[outlen++]=(type%256)+0x33;  				//���ݱ�ʶ
	outbuf[outlen++]= (type>>8)%256+0x33;
	outbuf[outlen++]= (type>>16)%256+0x33;
	outbuf[outlen++]= (type>>24)%256+0x33;

	if ((data!=NULL) && (datalen!=0))
	{
		for (i=0;i<datalen;i++)
		{
			outbuf[outlen++]=*(data+i)+0x33;
		}
	}

	outbuf[outlen++]=MyCum8Sum(outbuf+4,outlen - 4);   // У�� (1)  

	outbuf[outlen++]=ELMTAIL;						// β (1)

    return outlen;

}

/************************************************************************************************************
** �� �� �� : ElmRead
** �������� : ��������� ���ID,���� ����ָ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
uint8_t ElmRead(uint8_t id,uint8_t readtype,uint32_t *data, uint8_t MeterAddr[6])
{
	uint8_t  senddata[128]={0};
	uint32_t type=0;
	uint16_t sendlen=0;
	ElmRcvMsg rcvmsg;
//	uint8_t addr[10]={0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};

	if (readtype == TYPE_ELECY)
	{
		sendlen=ElmMakeMsg(MeterAddr,ELM_CMD_READDATA,ELM_DI_READTOTALELC,NULL,0,senddata);
		if (ElmSenddRv(id,senddata,sendlen,&rcvmsg) != 1)
		{
			return 0;
		}
		if (rcvmsg.ctlcode == ELM_ACK_READDATA)
		{
			//printx(rcvmsg.buf,rcvmsg.buflen);
			memcpy(&type,rcvmsg.buf,4);
			if (type == ELM_DI_READTOTALELC)
			{
				*data=Mylbcdstr2hex(rcvmsg.buf+4,4);
				return 1;
			}
		}
	}

	

	if (readtype == TYPE_VOL)
	{
		sendlen=ElmMakeMsg(MeterAddr,ELM_CMD_READDATA,ELM_DI_READVOL,NULL,0,senddata);
		if (ElmSenddRv(id,senddata,sendlen,&rcvmsg) != 1)
		{
			return 0;
		}
		if (rcvmsg.ctlcode == ELM_ACK_READDATA)
		{
			memcpy(&type,rcvmsg.buf,4);
			if (type == ELM_DI_READVOL)
			{
				*data=Mylbcdstr2hex(rcvmsg.buf+4,2);
				return 1;
			}	
		}
	}


	if (readtype == TYPE_CURRENT)
	{
		sendlen=ElmMakeMsg(MeterAddr,ELM_CMD_READDATA,ELM_DI_READCURRENT,NULL,0,senddata);
		if (ElmSenddRv(id,senddata,sendlen,&rcvmsg) != 1)
		{
			return 0;
		}
		
		if (rcvmsg.ctlcode == ELM_ACK_READDATA)
		{
			memcpy(&type,rcvmsg.buf,4);
			if (type == ELM_DI_READCURRENT)
			{
				*data=Mylbcdstr2hex(rcvmsg.buf+4,3);
				return 1;
			}
		}
	}


	if (readtype == TYPE_DATE)
	{
		sendlen=ElmMakeMsg(MeterAddr,ELM_CMD_READDATA,ELM_DI_READDAY,NULL,0,senddata);
		if (ElmSenddRv(id,senddata,sendlen,&rcvmsg) != 1)
		{
			return 0;
		}
		if (rcvmsg.ctlcode == ELM_ACK_READDATA)
		{
			memcpy(&type,rcvmsg.buf,4);
			if (type == ELM_DI_READDAY)
			{
				data[0]=Mybcd2hex(rcvmsg.buf[7]);
				data[1]=Mybcd2hex(rcvmsg.buf[6]);
				data[2]=Mybcd2hex(rcvmsg.buf[5]);
				return 1;
				
			}
		}
	}


	if (readtype == TYPE_TIME)
	{
		if (ElmSenddRv(id,senddata,sendlen,&rcvmsg) != 1)
		{
			return 0;
		}
		if (rcvmsg.ctlcode == ELM_ACK_READDATA)
		{
			memcpy(&type,rcvmsg.buf,4);
			if (type == ELM_DI_READTIME)
			{
				data[0]=Mybcd2hex(rcvmsg.buf[6]);
				data[1]=Mybcd2hex(rcvmsg.buf[5]);
				data[2]=Mybcd2hex(rcvmsg.buf[4]);
				return 1;
				
			}
		}
	
	}

	return 0;
	
}

/************************************************************************************************************
** �� �� �� : ElmReadData
** �������� : ��������� (�ܵ�������ѹ������)
** ��    �� : MeterAddr ����ַ
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void ElmReadData(uint8_t id, uint8_t MeterAddr[6])
{
	uint8_t ret=0;

	ret = ElmRead(id,TYPE_ELECY,&(ElmInfo[id].totalelc), MeterAddr);
	if (ret == 1){
		ElmInfo[id].timeoutcnt=0;
	} else {
		print("warnning:read meter ELE timeout!");
		ElmInfo[id].timeoutcnt++;
	}
		
	OSTimeDlyHMSM (0, 0, 0, 200);

	ret = ElmRead(id,TYPE_VOL,&(ElmInfo[id].vol), MeterAddr);
	if (ret == 1){
		ElmInfo[id].timeoutcnt=0;
	} else {
		print("warnning:read meter VOL timeout!");
		ElmInfo[id].timeoutcnt++;
	}
	
	OSTimeDlyHMSM (0, 0, 0, 200);

	ret = ElmRead(id,TYPE_CURRENT,&(ElmInfo[id].current), MeterAddr);	
	if (ret == 1){
		ElmInfo[id].timeoutcnt=0;
	} else {
		print("warnning:read meter CUR timeout!");
		ElmInfo[id].timeoutcnt++;
	}
	
	OSTimeDlyHMSM (0, 0, 0, 200);
}

/************************************************************************************************************
** �� �� �� : ElmGetTotalElc
** �������� : ��ȡ����ܵ���
** ��    �� : id 0/1
** ��    �� : ��
** ��    �� : ���� ��2λС��
*************************************************************************************************************
*/
uint32_t ElmGetTotalElc(uint8_t id)
{
	if (id >= ELMMAXCNT)
	{
		return 0;
	}
	
	return ElmInfo[id].totalelc;
}

/************************************************************************************************************
** �� �� �� : ElmGetVolA
** �������� : ��ȡ����ѹ A��
** ��    �� : id 0/1
** ��    �� : ��
** ��    �� : ��ѹ ��1λС��
*************************************************************************************************************
*/
uint32_t ElmGetVolA(uint8_t id)
{
	if (id >= ELMMAXCNT)
	{
		return 0;
	}
	
	return ElmInfo[id].vol;
}

/************************************************************************************************************
** �� �� �� : ElmGetVolB
** �������� : ��ȡ����ѹ B��
** ��    �� : id 0/1
** ��    �� : ��
** ��    �� : ��ѹ ��1λС��
*************************************************************************************************************
*/
uint32_t ElmGetVolB(uint8_t id)
{
	if (id >= ELMMAXCNT)
	{
		return 0;
	}
	
	return 0;
	//return ElmInfo[id].vol;
}

/************************************************************************************************************
** �� �� �� : ElmGetVolB
** �������� : ��ȡ����ѹ C��
** ��    �� : id 0/1
** ��    �� : ��
** ��    �� : ��ѹ ��1λС��
*************************************************************************************************************
*/
uint32_t ElmGetVolC(uint8_t id)
{
	if (id >= ELMMAXCNT)
	{
		return 0;
	}
	
	return 0;
	//return ElmInfo[id].vol;
}

/************************************************************************************************************
** �� �� �� : ElmGetCurA
** �������� : ��ȡ������ A��
** ��    �� : id 0/1
** ��    �� : ��
** ��    �� : ���� ��3λС��
*************************************************************************************************************
*/
uint32_t ElmGetCurA(uint8_t id)
{
	if (id >= ELMMAXCNT)
	{
		return 0;
	}
	
	return ElmInfo[id].current;
}

/************************************************************************************************************
** �� �� �� : ElmGetCurB
** �������� : ��ȡ������ B��
** ��    �� : id 0/1
** ��    �� : ��
** ��    �� : ���� ��3λС��
*************************************************************************************************************
*/
uint32_t ElmGetCurB(uint8_t id)
{
	if (id >= ELMMAXCNT)
	{
		return 0;
	}
	
	return 0;
	//return ElmInfo[id].current;
}

/************************************************************************************************************
** �� �� �� : ElmGetCurC
** �������� : ��ȡ������ C��
** ��    �� : id 0/1
** ��    �� : ��
** ��    �� : ���� ��3λС��
*************************************************************************************************************
*/
uint32_t ElmGetCurC(uint8_t id)
{
	if (id >= ELMMAXCNT)
	{
		return 0;
	}
	
	return 0;
	//return ElmInfo[id].current;
}

/************************************************************************************************************
** �� �� �� : ElmGetCurC
** �������� : ��ȡ���ͨ��״̬
** ��    �� : id 0/1
** ��    �� : ��
** ��    �� : 0ͨ������ 1ͨ���쳣
*************************************************************************************************************
*/
uint8_t ElmGetCommState(uint8_t id)
{
	if (id >= ELMMAXCNT)
	{
		return 0;
	}
	
	return ElmInfo[id].commstate;
}

/************************************************************************************************************
** �� �� �� : ElmProc
** �������� : �������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void ElmProc(void)
{
	static uint32_t timer,cnt;
	uint8_t MeterAddr[6];
	uint8_t PortNum = ParaGetChgGunNumber();
	
	if (TimerRead() - timer > T1S * 1){
		timer = TimerRead();
		if (PortNum == 1){//��ǹ
			memset(MeterAddr, 0xAA, 6);
			ElmReadData(0, MeterAddr);
		} else {//˫ǹ
			if (cnt == 0){
				cnt = 1;
				ParaGetDCMeterAddr(0, MeterAddr);
				ElmReadData(0, MeterAddr);
			} else {
				cnt = 0;
				ParaGetDCMeterAddr(1, MeterAddr);
				ElmReadData(1, MeterAddr);
			}
		}
	}
	if (ElmInfo[0].timeoutcnt >= 5){
		ElmInfo[0].commstate = 1;
	} else {
		ElmInfo[0].commstate = 0;
	}
	if (ElmInfo[1].timeoutcnt >= 5){
		ElmInfo[1].commstate = 1;
	} else {
		ElmInfo[1].commstate = 0;
	}
}

/************************************************************************************************************
** �� �� �� : ElmTask
** �������� : ���������
** ��    �� : pdata ��ʹ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ElmTask(void *pdata)
{
	RS485Init(ELMBAUD);
	print("Elm task start...\r\n");
	
	while(1){
		ElmProc();
		OSTimeDlyHMSM (0, 0, 0, 10);
	}
}

