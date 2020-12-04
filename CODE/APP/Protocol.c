#include <stdio.h>
#include <LPC177x_8x.H>
#include <stdint.h>
#include "includes.h"
#include "Protocol.h"
#include "Para.h"
#include "RTC.h"
#include "Cfg.h"
#include "Record.h"
#include "Timer.h"
#include "MyAlgLib.h"
#include "Board.h"
#include "NetTask.h"
#include "MainTask.h"
#include "UART.h"
#include "MyAlgLib.h"
#include <string.h>
#include "ChgInterface.h"
#include "ElmTask.h"
#include "IAP.h"
static COMMINFO CommInfo[2];
static REportCMD204  ReportCMD204[2];

static MODIFINFO  ModifInfo[2];

static uint32_t UpdataFileSize;
static uint32_t UpdataTimer;
static uint8_t FileName[128];
static uint8_t FileIndexNum;
static uint8_t UpdataFlag;

/************************************************************************************************************
** �� �� �� : GetUserId
** �������� : ����û�Id
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	�û� Id
*************************************************************************************************************
*/
void GetUserId(uint8_t port, uint8_t* buf)
{
	memcpy((char *)buf, CommInfo[port].UserId, 2);

}
/************************************************************************************************************
** �� �� �� : SendCMD204flay
** �������� : ���ͳ��׮�ϴ���������Ϣ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ   mark  =1  ����   =0 ������
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SendCMD204Flay(uint8_t port, uint8_t mark)
{

	CommInfo[port].SendCmd204 = mark;
	
}


/************************************************************************************************************
** �� �� �� : ChangeStatus
** �������� : �жϳ��׮״̬�Ƿ����仯
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ    
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static uint8_t ChangeStatus(uint8_t port)
{
	static uint8_t MainCharge1,GunStatus1, MainCharge2, GunStatus2, Count1, Count2;
	CHARGEINFO info;
	
	info = GetChgInfo(port);
	if(port==0){
		if((MainCharge1 != info.ChargeState )|| (GunStatus1 != info.GunSeatState) )
		{
			Count1++;
			if(Count1 >= 20){
				MainCharge1 = info.ChargeState;
				GunStatus1 = info.GunSeatState;
				Count1 = 0;
				return 1;
			}		
		}
	}else if(port==1){
		if((MainCharge2 != info.ChargeState ) || (GunStatus2 != info.GunSeatState))
		{
			Count2++;
			if(Count2 >= 20){
			MainCharge2 = info.ChargeState ;
			GunStatus2 = info.GunSeatState;
			Count2 = 0;
			return 1;
			}
		}
	}
	return 0;
}




/************************************************************************************************************
** �� �� �� : GetOnlineState
** �������� : ��ȡ����״̬
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	����ǹ�����߱�ʶ
*************************************************************************************************************
*/
uint8_t GetOnlineState(uint8_t port)
{
	if (CommInfo[port].loginackflag == 1){
		return 1;
	}
	return 0;
}



/************************************************************************************************************
** �� �� �� : VerifyOnlineCardInfo
** �������� : ��ʵ���߿���Ϣ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......  CardID ���� 
** ��    �� : Balance���
** ��    �� :	0��Ч�ʻ� 1�Ƿ��ʻ� 2���� 3�Ѻڿ��ָ��������� 4����������ڿ� 5�Ƿ����� 6��ʧ 7���� 8������� 9�ÿ��Ѿ��ڱ��׮ʹ��
*************************************************************************************************************
*/
uint8_t VerifyOnlineCardInfo(uint8_t port, uint32_t CardID, uint32_t *Balance)
{
	uint32_t Timer;
	
	*Balance = 0;
	ReportCMD204[port].ReportOnlineCardID = CardID;
	ReportCMD204[port].OnlineCardIDBalance = 0;
	ReportCMD204[port].ReportRet = 0;
	ReportCMD204[port].ReportFlag = 1;
	Timer = TimerRead();
	while(TimerRead() - Timer < T1S * 10){
		OSTimeDlyHMSM (0, 0, 0, 100);
		if (ReportCMD204[port].ReportFlag == 2){
			*Balance = ReportCMD204[port].OnlineCardIDBalance;
			return ReportCMD204[port].ReportRet;
		}
	}
	return 0xFF;
}

/************************************************************************************************************
** �� �� �� : addChecksum
** �������� : У��ͼ��㺯��
** ��    �� : buffer ������ length ����������
** ��    �� : ��
** ��    �� :	У���
*************************************************************************************************************
*/
static uint8_t addChecksum(const uint8_t* buffer, uint32_t length)
{
	uint32_t sum=0;
	uint32_t i=0;
	
	if (!buffer){
		return 0;
	}
	for (i=0; i<length; i++){
		sum = sum + *buffer;
		buffer++;
	}
	return sum&0xff;
}





/************************************************************************************************************
** �� �� �� : SendCMD102
** �������� : ���ͳ��׮�ϴ���������Ϣ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void SendCMD102(uint8_t port)
{
	static uint16_t snum[2];
	uint8_t buffer[COMMBUFFERLEN], *pbuf,*ptr,PileNumber[32];
	
	pbuf = buffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x2F; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x41; //���к���
	*pbuf++ = 102; //�������
	*pbuf++ = 0;
	*pbuf++ = 0x00; //������
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	if(port == 0){
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'A';
  }else  if(port == 1){
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'B';	   
	}	
	memcpy(pbuf, PileNumber, 32);
	pbuf += 32;
	*pbuf++ = snum[port];
	*pbuf++ = snum[port] >> 8;
	snum[port]++;
	*pbuf++ = addChecksum(buffer + 6, pbuf - buffer - 6);
	
	NetSend(port, buffer, pbuf - buffer);
	print("CMD102 ���ͳ��׮�ϴ���������ϢP%d:", port);
	printx(buffer, pbuf - buffer);
}

/************************************************************************************************************
** �� �� �� : RecvCMD1
** �������� : ���պ�̨�������·����׮���͹�������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD1(uint8_t port, uint8_t *buffer)
{
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf, type, ret, PileNumber[32], *ptr;
	uint32_t paraaddr, num, i, paranum, data32;
	
	paraaddr = buffer[13] | (buffer[14] << 8) | (buffer[15] << 16) | (buffer[16] << 24);
	type = buffer[12];
	if(type == 0x00){ //��ѯ
		ret = 0;
		memset(ackbuffer, 0, sizeof(ackbuffer));
		paranum = buffer[17];
		for(i = 0; i < paranum; i++){
			switch(paraaddr++){
				case 1: //ǩ��ʱ����
					data32 = ParaGetLoginTimeInterval(port);
					memcpy(&ackbuffer[51 + i * 4], &data32, 4);
					break;
				case 21: //�����ϱ�����
					data32 = ParaGetNetHeartTime(port);
					memcpy(&ackbuffer[51 + i * 4], &data32, 4);
					break;
				case 22: //��������ⳬʱ����
					data32 = ParaGetNetCommOverCnt(port);
					memcpy(&ackbuffer[51 + i * 4], &data32, 4);
					break;
				case 23: //���׮״̬��Ϣ���ϱ�����
					data32 = ParaGetStateInfoReportPeriod(port);
					memcpy(&ackbuffer[51 + i * 4], &data32, 4);
					break;
				case 25: //���ķ�������ַ
					ParaGetServerIp(port, &ackbuffer[51 + i * 4]);
					break;
				case 26: //���ķ������˿�
					data32 = ParaGetServerPort(port);
					memcpy(&ackbuffer[51 + i * 4], &data32, 4);
					break;
				default:
					ret = 1;
					break;
			}
		}
		if (ret != 0){
			num = 0;
		} else {
			num = i;
		}
		pbuf = ackbuffer;
		*pbuf++ = STARTCODE1; //��ʼ��
		*pbuf++ = STARTCODE2;
		*pbuf++ = 9 + 43 + num * 4; //������
		*pbuf++ = 0x00;
		*pbuf++ = MSGCODE; //��Ϣ��
		*pbuf++ = 0x00; //���к���
		*pbuf++ = 2; //�������
		*pbuf++ = 0;
		*pbuf++ = 0x00; //������
		*pbuf++ = 0x00;
		*pbuf++ = 0x00;
		*pbuf++ = 0x00;
		if(port == 0){
			ParaGetPileNo(0, PileNumber);
			ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
			*ptr  = 'A';
		}else if(port == 1){
			ParaGetPileNo(0, PileNumber);
			ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
			*ptr  = 'B';	   
		}	
		memcpy(pbuf, PileNumber, 32);
		pbuf += 32;
		*pbuf++ = buffer[12];
		memcpy(pbuf, &buffer[13], 4);
		pbuf += 4;
		*pbuf++ = buffer[17];
		*pbuf++ = ret;
		pbuf += num * 4;
		*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
		
		NetSend(port, ackbuffer, pbuf - ackbuffer);
		print("CMD2 ���׮�������β�ѯӦ��P%d:", port);
		printx(ackbuffer, pbuf - ackbuffer);
	}
	if(type == 0x01){ //����
		ret = 0;
		paranum = buffer[17];
		num = buffer[18] | (buffer[19] << 8);
		for(i = 0; i < paranum; i++){
			switch(paraaddr++){
				case 1: //ǩ��ʱ����
					data32 = buffer[20 + i * 4] | (buffer[21 + i * 4] << 8);
					ParaSetLoginTimeInterval(port, data32);
					break;
				case 21: //�����ϱ�����
					data32 = buffer[20 + i * 4];
					ParaSetNetHeartTime(port, data32);
					break;
				case 22: //��������ⳬʱ����
					data32 = buffer[20 + i * 4];
					ParaSetNetCommOverCnt(port, data32);
					break;
				case 23: //���׮״̬��Ϣ���ϱ�����
					data32 = buffer[20 + i * 4] | (buffer[21 + i * 4] << 8);
					ParaSetStateInfoReportPeriod(port, data32);
					break;
				case 25: //���ķ�������ַ
					ParaSetServerIp(port, &buffer[20 + i * 4]);
					break;
				case 26: //���ķ������˿�
					data32 = buffer[20 + i * 4] | (buffer[21 + i * 4] << 8);
					ParaSetServerPort(port, data32);
					break;
				default:
					ret = 1;
					break;
			}
		}
		pbuf = ackbuffer;
		*pbuf++ = STARTCODE1; //��ʼ��
		*pbuf++ = STARTCODE2;
		*pbuf++ = 0x34; //������
		*pbuf++ = 0x00;
		*pbuf++ = MSGCODE; //��Ϣ��
		*pbuf++ = 0x00; //���к���
		*pbuf++ = 2; //�������
		*pbuf++ = 0;
		*pbuf++ = 0x00; //������
		*pbuf++ = 0x00;
		*pbuf++ = 0x00;
		*pbuf++ = 0x00;
		if(port == 0){ //���׮���
			ParaGetPileNo(0, PileNumber);
			ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
			*ptr  = 'A';
		}else if(port == 1){
			ParaGetPileNo(0, PileNumber);
			ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
			*ptr  = 'B';	   
		}	
		memcpy(pbuf, PileNumber, 32);
		pbuf += 32;
		*pbuf++ = buffer[12];
		memcpy(pbuf, &buffer[13], 4);
		pbuf += 4;
		*pbuf++ = buffer[17];
		*pbuf++ = ret;
		*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
		
		NetSend(port, ackbuffer, pbuf - ackbuffer);
		print("CMD2 ���׮������������Ӧ��P%d:", port);
		printx(ackbuffer, pbuf - ackbuffer);
	}
}

/************************************************************************************************************
** �� �� �� : RecvCMD3
** �������� : ���պ�̨�������·����׮�ַ��͹�������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD3(uint8_t port, uint8_t *buffer)
{
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf, type,meteraddr[6], qrcode[256], oldpileno[32], oldpilenoflag = 0, PileNumber[32], *ptr;
	uint32_t paraaddr, num, i;
	Rtc datetime;
	
	paraaddr = buffer[13] | (buffer[14] << 8) | (buffer[15] << 16) | (buffer[16] << 24);
	type = buffer[12];
	if(type == 0x00){ //��ѯ
		memset(ackbuffer, 0, sizeof(ackbuffer));
		switch(paraaddr){
			case 1: //���׮����
				if(port == 0){ //���׮���
					ParaGetPileNo(0, PileNumber);
					ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
					*ptr  = 'A';
				}else if(port == 1){
					ParaGetPileNo(0, PileNumber);
					ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
					*ptr  = 'B';	   
				}	
				memcpy(ackbuffer, PileNumber, 32);				
				num = 32;
				break;
			case 2: //��׼ʱ��ʱ��
				RtcRead(&datetime);
				ackbuffer[50 + 0] = Hex32ToBcd32(datetime.year / 100);
				ackbuffer[50 + 1] = Hex32ToBcd32(datetime.year % 100);
				ackbuffer[50 + 2] = Hex32ToBcd32(datetime.month);
				ackbuffer[50 + 3] = Hex32ToBcd32(datetime.day);
				ackbuffer[50 + 4] = Hex32ToBcd32(datetime.hour);
				ackbuffer[50 + 5] = Hex32ToBcd32(datetime.min);
				ackbuffer[50 + 6] = Hex32ToBcd32(datetime.sec);
				ackbuffer[50 + 7] = 0xFF;
				num = 8;
				break;
			case 3: //����Ա����
				memset(&ackbuffer[50], 0, 8);
				ackbuffer[50 + 0] = '\0';
				num = 8;
				break;
			case 4: //����Ա����
				memset(&ackbuffer[50], 0, 8);
				ackbuffer[50 + 0] = '\0';
				num = 8;
				break;
			case 5: //MAC��ַ Ԥ��
				memset(&ackbuffer[50], 0, 6);
				ackbuffer[50 + 0] = '\0';
				num = 6;
				break;
			case 6: //Ԥ��
				memset(&ackbuffer[50], 0, 16);
				ackbuffer[50 + 0] = '\0';
				num = 16;
				break;
			case 7: //��ά��
				ParaGetQRCode(port, qrcode);
				num = 0;
				for(i = 0; i < 256; i++){
					num ++;
					if(qrcode[i] == '\0')
						break;
					ackbuffer[50 + i] = qrcode[i];
				}
				ackbuffer[50 + i] = '\0';
				break;
			case 8: //�ͻ���������1 Ԥ��
				ackbuffer[50 + 0] = '\0';
				num = 1;
				break;
			case 9: //�ͻ���������2 Ԥ��
				ackbuffer[50 + 0] = '\0';
				num = 1;
				break;
			case 10: //�û�֧����ά�� Ԥ��
				ackbuffer[50 + 0] = '\0';
				num = 1;
				break;
			case 11: //׮�������ά��ǰ׺
				ackbuffer[50 + 0] = '\0';
				num = 1;
				break;
			case 12: //DLT645-2007
				ParaGetACMeterAddr(port, meteraddr);
				//ParaGetDCMeterAddr(port, meteraddr);
				ackbuffer[50] = (meteraddr[5] >> 4) | 0x30;
				ackbuffer[51] = (meteraddr[5] & 0x0f) | 0x30;
				ackbuffer[52] = (meteraddr[4] >> 4) | 0x30;
				ackbuffer[53] = (meteraddr[4] & 0x0f) | 0x30;
				ackbuffer[54] = (meteraddr[3] >> 4) | 0x30;
				ackbuffer[55] = (meteraddr[3] & 0x0f) | 0x30;
				ackbuffer[56] = (meteraddr[2] >> 4) | 0x30;
				ackbuffer[57] = (meteraddr[2] & 0x0f) | 0x30;
				ackbuffer[58] = (meteraddr[1] >> 4) | 0x30;
				ackbuffer[59] = (meteraddr[1] & 0x0f) | 0x30;
				ackbuffer[60] = (meteraddr[0] >> 4) | 0x30;
				ackbuffer[61] = (meteraddr[0] & 0x0f) | 0x30;
				num = 12;
				break;
			default:
				break;
		}
		pbuf = ackbuffer;
		*pbuf++ = STARTCODE1; //��ʼ��
		*pbuf++ = STARTCODE2;
		*pbuf++ = 9 + 42 + num; //������
		*pbuf++ = 0x00;
		*pbuf++ = MSGCODE; //��Ϣ��
		*pbuf++ = 0x00; //���к���
		*pbuf++ = 4; //�������
		*pbuf++ = 0;
		*pbuf++ = 0x00; //������
		*pbuf++ = 0x00;
		*pbuf++ = 0x00;
		*pbuf++ = 0x00;
		if(port == 0){ //���׮���
			ParaGetPileNo(0, PileNumber);
			ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
			*ptr  = 'A';
		}else if(port == 1){
			ParaGetPileNo(0, PileNumber);
			ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
			*ptr  = 'B';	   
		}	
		memcpy(pbuf, PileNumber, 32);
		pbuf += 32;
		*pbuf++ = buffer[12];
		memcpy(pbuf, &buffer[13], 4);
		pbuf += 4;
		*pbuf++ = 0;
		pbuf += num;
		*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
		
		NetSend(port, ackbuffer, pbuf - ackbuffer);
		print("CMD4 ���׮�����ַ��β�ѯӦ��P%d:", port);
		printx(ackbuffer, pbuf - ackbuffer);
	}
	if(type == 0x01){ //����
		switch(paraaddr){
			case 1: //���׮����
				if(port == 0){   //���׮���
					ParaGetPileNo(0, PileNumber);
					ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
					*ptr  = 'A';
				}else if(port == 1){
					ParaGetPileNo(0, PileNumber);
					ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
					*ptr  = 'B';	   
				}	
				memcpy(oldpileno, PileNumber, 32);						
				oldpilenoflag = 1;
				ParaSetPileNo(port, &buffer[19]);
				break;
			case 2: //��׼ʱ��ʱ��
				datetime.year = Bcd32ToHex32(buffer[19]) * 100 + Bcd32ToHex32(buffer[19 + 1]);
				datetime.month = Bcd32ToHex32(buffer[19 + 2]);
				datetime.day = Bcd32ToHex32(buffer[19 + 3]);
				datetime.hour = Bcd32ToHex32(buffer[19 + 4]);
				datetime.min = Bcd32ToHex32(buffer[19 + 5]);
				datetime.sec = Bcd32ToHex32(buffer[19 + 6]);
				RtcWrite(&datetime);
				break;
			case 3: //����Ա����
				break;
			case 4: //����Ա����
				break;
			case 5: //MAC��ַ Ԥ��
				break;
			case 6: //Ԥ��
				break;
			case 7: //��ά�� Ԥ��
				memset(qrcode, 0, 256);
				num = 0;
				for(i = 0; i < 256; i++){
					num ++;
					if(buffer[19 + i] == '\0')
						break;
				}
				memcpy(qrcode, &buffer[19], num);
				ParaSetQRCode(port, qrcode);
				break;
			case 8: //�ͻ���������1 Ԥ��
				break;
			case 9: //�ͻ���������2 Ԥ��
				break;
			case 10: //�û�֧����ά�� Ԥ��
				break;
			case 11: //׮�������ά��ǰ׺
				for(i = 0; i < 128; i++){
					num ++;
					if(buffer[19 + i] == '\0'){
						break;
					}
				}
				break;
			case 12: //DLT645-2007����ַ
				meteraddr[5] = ((buffer[19] & 0x0f) << 4) | (buffer[20] & 0x0f);
				meteraddr[4] = ((buffer[21] & 0x0f) << 4) | (buffer[22] & 0x0f);
				meteraddr[3] = ((buffer[23] & 0x0f) << 4) | (buffer[24] & 0x0f);
				meteraddr[2] = ((buffer[25] & 0x0f) << 4) | (buffer[26] & 0x0f);
				meteraddr[1] = ((buffer[27] & 0x0f) << 4) | (buffer[28] & 0x0f);
				meteraddr[0] = ((buffer[29] & 0x0f) << 4) | (buffer[30] & 0x0f);
				ParaSetACMeterAddr(port, meteraddr);
				break;
			default:
				break;
		}
		pbuf = ackbuffer;
		*pbuf++ = STARTCODE1; //��ʼ��
		*pbuf++ = STARTCODE2;
		*pbuf++ = 0x33; //������
		*pbuf++ = 0x00;
		*pbuf++ = MSGCODE; //��Ϣ��
		*pbuf++ = 0x00; //���к���
		*pbuf++ = 4; //�������
		*pbuf++ = 0;
		*pbuf++ = 0x00; //������
		*pbuf++ = 0x00;
		*pbuf++ = 0x00;
		*pbuf++ = 0x00;
		if (oldpilenoflag == 1){
			memcpy(PileNumber, oldpileno, 32);
			if(port == 0){//���׮���
				ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
				*ptr  = 'A';
			}else if(port == 1)
			{
				ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
				*ptr  = 'B';	   
			}	
			memcpy(pbuf, PileNumber, 32);			
		} else {
			if(port == 0){ //���׮���
				ParaGetPileNo(0, PileNumber);
				ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
				*ptr  = 'A';
			}else  if(port == 1){
				ParaGetPileNo(0, PileNumber);
				ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
				*ptr  = 'B';	   
			}	
			memcpy(pbuf, PileNumber, 32);
		}
		pbuf += 32;
		*pbuf++ = buffer[12];
		memcpy(pbuf, &buffer[13], 4);
		pbuf += 4;
		*pbuf++ = 0;
		*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
		
		NetSend(port, ackbuffer, pbuf - ackbuffer);
		print("CMD4 ���׮�����ַ�������Ӧ��P%d:", port);
		printx(ackbuffer, pbuf - ackbuffer);
	}
}

/************************************************************************************************************
** �� �� �� : RecvCMD5
** �������� : ���պ�̨�������·����׮��������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD5(uint8_t port, uint8_t *buffer)
{
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf,ret = 1, rebootflag = 0, PileNumber[32], *ptr;
	uint32_t cmdaddr;//,cmdnum,cmdlen;
	
	cmdaddr = buffer[13] | (buffer[14] << 8) | (buffer[15] << 16) | (buffer[16] << 24);
//	cmdnum = buffer[17];
//	cmdlen = buffer[18] | (buffer[19] << 8);
	switch(cmdaddr){
		case 2: //ֹͣ���
			if(buffer[20] == 0x55){
				//Ӧ����ֹͣ��纯��
				CommInfo[port].stopflag = 1;
				ret = 0;
			}
			break;
		case 10: //ȡ��ԤԼ
			ret = 1;
			break;
		case 11: //�豸����
			rebootflag = 1;
			break;
		default:
			ret = 0;
			break;
	}
	
	pbuf = ackbuffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x34; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x00; //���к���
	*pbuf++ = 0x06; //�������
	*pbuf++ = 0x00;
	memset(pbuf, 0, 4);//������
	pbuf += 4;
	if(port == 0){//���׮���
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'A';
	}else  if(port == 1){
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'B';	   
	}	
	memcpy(pbuf, PileNumber, 32);	
	pbuf += 32;
	*pbuf++ = buffer[12];
	memcpy(pbuf, &buffer[13], 4);
	pbuf += 4;
	*pbuf++ = buffer[17];
	*pbuf++ = ret;
	*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
	
	NetSend(port, ackbuffer, pbuf - ackbuffer);
	print("CMD6 ���׮�Ժ�̨��������Ӧ��P%d:", port);
	printx(ackbuffer, pbuf - ackbuffer);
	
	if (rebootflag == 1){
		print("׼�������豸..................\r\n");
		OSTimeDlyHMSM (0, 0, 3, 0);
		//�Ƿ���Ҫ��������������
		NVIC_SystemReset();
	}
}

/************************************************************************************************************
** �� �� �� : RecvCMD7
** �������� : ���պ�̨�������·����׮��������������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD7(uint8_t port, uint8_t *buffer)
{
	if(buffer[14] != 0x00){ //�����Ч���Ͳ�֧��
		return;
	}
	memcpy(&CommInfo[port].UserId,&buffer[8],2);
	CommInfo[port].chargestrategy = buffer[21]; //������
	CommInfo[port].chargepara = buffer[25] | (buffer[26] << 8) | (buffer[27] << 16) | (buffer[28] << 24); //�����Բ���
	memcpy(CommInfo[port].ChargeCardNumber, &buffer[38], 32);  
	CommInfo[port].startflag = 1;
}

/************************************************************************************************************
** �� �� �� : SendCMD8
** �������� : ���ͳ��׮�Ժ�̨�·��ĳ��׮����������Ӧ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... failcode 0����ɹ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void SendCMD8(uint8_t port, uint32_t failcode)
{
	uint8_t buffer[COMMBUFFERLEN], *pbuf,PileNumber[32],*ptr;
	
	pbuf = buffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x32; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x00; //���к���
	*pbuf++ = 8; //�������
	*pbuf++ = 0;
	*pbuf++ = 0x00; //������
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	if(port == 0){   //���׮���
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'A';
	}else  if(port == 1){
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'B';	   
	}	
	memcpy(pbuf, PileNumber, 32);	
	pbuf += 32;
	if(ParaGetChgGunNumber() > 1){
		*pbuf++ = port + 1;
	} else {
		*pbuf++ = 0;
	}
	memcpy(pbuf, &failcode, 4);
	pbuf += 4;
	*pbuf++ = addChecksum(buffer + 6, pbuf - buffer - 6);
	
	NetSend(port, buffer, pbuf - buffer);
	print("CMD8 ���׮�Ժ�̨�·��ĳ��׮����������Ӧ��P%d:", port);
	printx(buffer, pbuf - buffer);
}

/************************************************************************************************************
** �� �� �� : SendCMD104
** �������� : ���ͳ��׮״̬��Ϣ���ϱ�
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void SendCMD104(uint8_t port)
{
	uint8_t buffer[COMMBUFFERLEN], *pbuf,datetime[8], *ptr, PileNumber[32];
	uint32_t data32, data1, data2, data3, data4, data5, data6;
	uint8_t MainState,buf1[32],buf[32];
	CHARGEINFO info;
	
	MainState = GetMainCharge(port);
	info = GetChgInfo(port);
	pbuf = buffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0xB2; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x41; //���к���
	*pbuf++ = 104; //�������
	*pbuf++ = 0;
	memcpy(pbuf,&CommInfo[port].UserId,2);
	pbuf += 2;	
	*pbuf++ = 0x00; //������
	*pbuf++ = 0x00;
//	*pbuf++ = 0x00; //������
//	*pbuf++ = 0x00;

	if(port == 0){ //���׮���
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'A';
	}else  if(port == 1){
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'B';	   
	}	
	memcpy(pbuf, PileNumber, 32);
	pbuf += 32;
	*pbuf++ = ParaGetChgGunNumber(); //���ǹ���� 1/2
	if (ParaGetChgGunNumber() > 1){
		*pbuf++ = port + 1; //���ں�
	} else {
		*pbuf++ = 0;
	}
	*pbuf++ = 0x01; //���ǹ���� 01ֱ�� 02����
	

	if (MainState == 0){
		if ((info.ScramState == 1) || (info.DoorState == 1) || GetLifeTime() || (info.CommunicateState == 1) || (ElmGetCommState(port) == 1) || (info.CCBAddrConflictState == 1) ){
			*pbuf++ = 6; //����״̬ ����
		}else if(GetStartingChg(port)){
			*pbuf++ = 1; //����״̬ ������
//			print("CMD104������P%d\r\n", port);
		}else{
			*pbuf++ = 0; //����״̬ ����
		}	
	} else if (MainState == 1){
		*pbuf++ = 2; //����״̬ ��������
	} else if (MainState == 2){
		*pbuf++ = 3; //����״̬ ������
	} else {
		*pbuf++ = 0;
	}

	*pbuf++ = info.SOC; //��ǰSOC
	data32 = 0;	
	if(GetLifeTime()){
		data32 = 2;  //���޵���
	} else if(info.ScramState == 1){
		data32 = 3;  //��ͣ
	}	else if(info.DoorState == 1){
		data32 = 4;  //�Ž�
	} else if(info.CommunicateState == 1){
		data32 = 1001;  //ͨѶ����
	} else if(ElmGetCommState(port) == 1){
		data32 = 1002;  //������
	}
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	
//	*pbuf++ = 0x00; //��ǰ��߸澯���� ���----------------
//	*pbuf++ = 0x00;
//	*pbuf++ = 0x00;
//	*pbuf++ = 0x00;
	if (info.GunSeatState == 0){
		*pbuf++ = 0; //������״̬ �Ͽ�
	} else {
		*pbuf++ = 2; //������״̬ ����
	}
	data32 = GetSumMoney(port); //���γ���ۼƳ����� 
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	memset(pbuf, 0x00, 8); //�ڲ�����2,3
	pbuf += 8;
	data32 = info.OutputVol; //ֱ������ѹ
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
//	data32 = (4000 - info.OutputCur) * 10; //ֱ��������
	data32 = (4000 - info.OutputCur); //ֱ��������
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = info.BmsDemandVol; //BMS�����ѹ
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
//	data32 = (4000 - info.BmsDemandCur) * 10; //BMS�������
	data32 = (4000 - info.BmsDemandCur); //BMS�������
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	*pbuf++ = info.Mode; //BMS���ģʽ
	GetVoltage(port, &data32, &data2, &data3);//pmaininfo->GunInfo[port].VoltageA; //0; //����A�����ѹ ���----------------
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	GetVoltage(port, &data1, &data32, &data3);//0; //����B�����ѹ ���----------------
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	GetVoltage(port, &data1, &data2, &data32);//0; //����C�����ѹ ���----------------
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	GetCurrent(port, &data32, &data2, &data3);//0; //����A������� ���----------------
	data32 /= 100;
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	GetCurrent(port, &data1, &data32, &data3);//0; //����B������� ���----------------
	data32 /= 100;
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	GetCurrent(port, &data1, &data2, &data32);//0; //����C������� ���----------------
	data32 /= 100;
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = info.SurplusMinute; //ʣ����ʱ��
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = GetSumTime(port);//0; //���ʱ�� ���----------------
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	data32 = GetSumEnergy(port);//0; //���γ���ۼƳ�����
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	data32 = GetStartMeterEnergy(port);//0; //���ǰ������ ���----------------
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	data32 = GetStopMeterEnergy(port);//0; //��ǰ������ ���----------------
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	if (GetStartType(port) == 1)
		*pbuf++ = 0x01; //���������ʽ ���----------------
	else if(GetStartType(port) == 2)
		*pbuf++ = 0x02; //���������ʽ ���---------------
	else
		*pbuf++ = 0x00; //���������ʽ ���---------------
	*pbuf++ = GetStartMode(port); //������ ���----------------
	data32 = GetStartPara(port);//0; //�����Բ��� ���----------------
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	*pbuf++ = 0x00; //ԤԼ��־ ���----------------
	memset(pbuf, 0x00, 32); //��翨�� ----------------
//	memcpy(pbuf,CommInfo[port].ChargeCardNumber,32);  
	if(GetStartType(port) == 1){
		data32 = 0;
		memcpy(&data32, CommInfo[port].UserId, 2);//data32 =  CommInfo[port].UserId;
		sprintf((char *)pbuf, "%d", data32);
	}else{
		data32 = GetStartCardID(port);
		u32tobcdnomstr1(data32, buf, 12);
		memset(buf1,0,32);
		sprintf((char *)buf1, "%d%d%d%d%d%d%d%d%d%d", buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);
		memcpy(pbuf, buf1, 32);	
	}
	pbuf += 32;		
	*pbuf++ = 0x00; //ԤԼ��ʱʱ�� ���----------------
	GetStartDateTime(port, datetime);
	*pbuf++ = 0x20;//ԤԼ/��ʼ��翪ʼʱ��
	*pbuf++ = Hex32ToBcd32(datetime[0]);
	*pbuf++ = Hex32ToBcd32(datetime[1]);
	*pbuf++ = Hex32ToBcd32(datetime[2]);
	*pbuf++ = Hex32ToBcd32(datetime[3]);
	*pbuf++ = Hex32ToBcd32(datetime[4]);
	*pbuf++ = Hex32ToBcd32(datetime[5]);
	*pbuf++ = 0xff;
	data32 = 0; //���ǰ����� ���----------------
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	memset(pbuf, 0x00, 4); //Ԥ��
	pbuf += 4;
	GetVoltage(port, &data1, &data2, &data3);
	GetCurrent(port, &data4, &data5, &data6);
	data32 = (data1 * data4 + data2 * data5 + data3 * data6) / 1000;
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	memset(pbuf, 0x00, 12); //ϵͳ����3,4,5
	pbuf += 12;
	*pbuf++ = addChecksum(buffer + 6, pbuf - buffer - 6);
	
	NetSend(port, buffer, pbuf - buffer);
	print("CMD104 ���׮״̬��Ϣ���ϱ�P%d:", port);
	printx(buffer, pbuf - buffer);
}




/************************************************************************************************************
** �� �� �� : SendCMD202
** �������� : ���ͳ��׮�ϱ�����¼��Ϣ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... data �ӳ��׮����-�����ˮ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void SendCMD202(uint8_t port, CHARGINGDATA *ChgDat, uint32_t current_index)
{
	uint8_t buffer[COMMBUFFERLEN], *pbuf;
	uint32_t data32;
	uint8_t i;
	uint8_t buf[100],buf1[100];
	uint16_t UseId;
	
	pbuf = buffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x29;//0x09; //������
	*pbuf++ = 0x01;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x41; //���к���
	*pbuf++ = 202; //�������
	*pbuf++ = 0;

	memcpy(pbuf, ChgDat->UserId, 2);
	pbuf += 2;		
	*pbuf++ = 0x00; //������
	*pbuf++ = 0x00;

	ParaGetPileNo(0, pbuf); 
	for (i = 0; i < 32; i++){
		if (pbuf[i] == 0x00){
			pbuf[i] = port + 'A';
			break;
		}
	}
	pbuf += 32;
	*pbuf++ = 1; //���ǹλ������
	if(ParaGetChgGunNumber() > 1){
		*pbuf++ = port + 1; //���ǹ��
	} else {
		*pbuf++ = 0;
	}
	memset(pbuf, 0, 32);
	if(ChgDat->StartType == 1){
			memset(buf1, 0, 32);
			UseId = ChgDat->UserId[0]|ChgDat->UserId[1] << 8;
			sprintf((char *)buf1, "%d", UseId);
			memcpy(pbuf, buf1, 32);	
	}else {
		u32tobcdnomstr1(ChgDat->StartCardID, buf, 12);
		memset(buf1, 0, 32);
		sprintf((char *)buf1, "%d%d%d%d%d%d%d%d%d%d", buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);
		memcpy(pbuf, buf1, 32);	
	}
	pbuf += 32;	
	*pbuf++ = 0x20;//��翪ʼʱ�� 
	*pbuf++ = Hex32ToBcd32(ChgDat->StartDateTime[0]);
	*pbuf++ = Hex32ToBcd32(ChgDat->StartDateTime[1]);
	*pbuf++ = Hex32ToBcd32(ChgDat->StartDateTime[2]);
	*pbuf++ = Hex32ToBcd32(ChgDat->StartDateTime[3]);
	*pbuf++ = Hex32ToBcd32(ChgDat->StartDateTime[4]);
	*pbuf++ = Hex32ToBcd32(ChgDat->StartDateTime[5]);
	*pbuf++ = 0xff;
	*pbuf++ = 0x20;//������ʱ��
	*pbuf++ = Hex32ToBcd32(ChgDat->StopDateTime[0]);
	*pbuf++ = Hex32ToBcd32(ChgDat->StopDateTime[1]);
	*pbuf++ = Hex32ToBcd32(ChgDat->StopDateTime[2]);
	*pbuf++ = Hex32ToBcd32(ChgDat->StopDateTime[3]);
	*pbuf++ = Hex32ToBcd32(ChgDat->StopDateTime[4]);
	*pbuf++ = Hex32ToBcd32(ChgDat->StopDateTime[5]);
	*pbuf++ = 0xff;
	data32 = ChgDat->SumTime;
	memcpy(pbuf, &data32, 4);//���ʱ�䳤��
	pbuf += 4;
	*pbuf++ = ChgDat->StartSOC;//��ʼSOC
	*pbuf++ = ChgDat->StopSOC;//����SOC
	*pbuf++ = ChgDat->StopCause; //������ԭ�� ���---------------
	*pbuf++ = 0;
	*pbuf++ = 0;
	*pbuf++ = 0;
	memcpy(pbuf, &ChgDat->SumEnergy, 4); //���γ�����
	pbuf += 4;
	memcpy(pbuf, &ChgDat->StartMeterEnergy, 4); //���ǰ������
	pbuf += 4;
	memcpy(pbuf, &ChgDat->StopMeterEnergy, 4); //���������
	pbuf += 4;
	memcpy(pbuf, &ChgDat->SumMoney, 4); //���γ����
	pbuf += 4;
	memset(pbuf, 0, 4); //Ԥ��
	pbuf += 4;
	memcpy(pbuf, &ChgDat->StartCardMoney, 4); //���ǰ�����
	pbuf += 4;
	memcpy(pbuf, &current_index, 4);//��ǰ����¼����//memcpy(pbuf, &CommInfo[port].ChargeRecordIndexBak, 4);//��ǰ����¼����
	pbuf += 4;
	memcpy(pbuf, &CommInfo[port].ChargeRecordNumberBak, 4);//�ܳ���¼��Ŀ
	pbuf += 4;
	*pbuf++ = 0; //Ԥ��
	*pbuf++ = ChgDat->Mode; //������
	memcpy(pbuf, &ChgDat->Para, 4); //�����Բ���
	pbuf += 4;
	memcpy(pbuf, &ChgDat->VIN, 17); //����VIN
	pbuf += 17;
	memset(pbuf, 0, 8); //���ƺ�
	pbuf += 8;
	memcpy(pbuf, ChgDat->PeriodEnergy, 48 * 2); //ʱ�ε���
	pbuf += 48 * 2;
	if(ChgDat->StartType == 3) //�ϱ����߿�������ʽ��̨�޷�ʶ��
		ChgDat->StartType = 0;
	*pbuf++ = ChgDat->StartType; //������ʽ 0����ˢ�� 1��̨���� 2���ع���Ա����
	memcpy(pbuf, ChgDat->TradeSerialNumber, 32); //������ˮ��
	pbuf += 32;

	
//	memcpy(pbuf, data, 284);
//	pbuf += 284;
	*pbuf++ = addChecksum(buffer + 6, pbuf - buffer - 6);
	
	NetSend(port, buffer, pbuf - buffer);
	print("CMD202 ���׮�ϱ�����¼��ϢP%d:", port);
	printx(buffer, pbuf - buffer);
}

/************************************************************************************************************
** �� �� �� : SendCMD204
** �������� : ���ͳ��׮����ϴ��û��ʻ���ѯ����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void SendCMD204(uint8_t port)
{
	uint8_t buffer[COMMBUFFERLEN], *pbuf, buf[32], buf1[32], PileNumber[32], *ptr;
	uint32_t data32;
	uint32_t ID;
	
	pbuf = buffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0xA2; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x41; //���к���
	*pbuf++ = 204; //�������
	*pbuf++ = 0;
	*pbuf++ = 0x00; //������
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	if(port == 0){
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'A';
  }else  if(port == 1){
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'B';	   
	}		
	pbuf += 32;
	memset(pbuf, 0x00, 32); //��翨�� ���----------------
	ID = ReportCMD204[port].ReportOnlineCardID;
	u32tobcdnomstr1(ID, buf, 12);
	memset(buf1, 0, 32);
	sprintf((char *)buf1, "%d%d%d%d%d%d%d%d%d%d", buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);
//	buf1[10]=0x30;
	memcpy(pbuf, buf1, 32);
	pbuf += 32;
	data32 = 0; //��翨��� ���----------------
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	*pbuf++ = 0x00; //��翨��������־
	memset(pbuf, 0x00, 32); //�û���翨���� ���----------------
	memcpy(pbuf, "123456", 6);
	pbuf += 32;
	memset(pbuf, 0x00, 48); //��������������� ���----------------
	*pbuf = 0x74;
	*(pbuf+1) = 0x65;
	*(pbuf+2) = 0x73;
	*(pbuf+3) = 0x74;
	pbuf += 48;
	*pbuf++ = addChecksum(buffer + 6, pbuf - buffer - 6);
	
	NetSend(port, buffer, pbuf - buffer);
	print("CMD204 ���׮����ϴ��û��ʻ���ѯ����P%d:", port);
	printx(buffer, pbuf - buffer);
}

/************************************************************************************************************
** �� �� �� : SendCMD106
** �������� : ���ͳ��׮ǩ����Ϣ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void SendCMD106(uint8_t port)
{
	uint8_t buffer[COMMBUFFERLEN], *pbuf, PileNumber[32], *ptr;
	Rtc datetime;
	uint32_t data32;
	
	pbuf = buffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x69; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x41; //���к���
	*pbuf++ = 106; //�������
	*pbuf++ = 0;
	*pbuf++ = 0x00; //������
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	if(port == 0){
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'A';
  }else  if(port == 1){
		ParaGetPileNo(0, PileNumber);
		ptr = (uint8_t *)strchr((char *)PileNumber, '\0');
		*ptr  = 'B';	   
	}		
  memcpy(pbuf, PileNumber, 32);
	pbuf += 32;
	*pbuf++ = 0x00; //��־
	*pbuf++ = _VERSION_MAJOR; //���׮����汾
	*pbuf++ = _VERSION_MINOR;
	*pbuf++ = _VERSION_PATCH;
	*pbuf++ = 0x00;
	memset(pbuf ,0x00, 6); //���׮��Ŀ���� ��������
	pbuf += 6;
	*pbuf++ = 0x02; //�����ϴ�ģʽ
	*pbuf++ = ParaGetLoginTimeInterval(port) & 0xff; //ǩ�����ʱ��
	*pbuf++ = (uint16_t)ParaGetLoginTimeInterval(port) >> 8;
	*pbuf++ = 0x00; //�����ڲ�����
	*pbuf++ = ParaGetChgGunNumber(); //���ǹ����
	*pbuf++ = ParaGetNetHeartTime(port); //�����ϱ�����
	*pbuf++ = ParaGetNetCommOverCnt(port); //��������ⳬʱ����
	data32 = GetChargeRecordNumber(port); //����¼����
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	RtcRead(&datetime);
	*pbuf++ = Hex32ToBcd32(datetime.year / 100) & 0xff; //��ǰ���׮ϵͳʱ��
	*pbuf++ = Hex32ToBcd32(datetime.year % 100) & 0xff;
	*pbuf++ = Hex32ToBcd32(datetime.month) & 0xff;
	*pbuf++ = Hex32ToBcd32(datetime.day) & 0xff;
	*pbuf++ = Hex32ToBcd32(datetime.hour) & 0xff;
	*pbuf++ = Hex32ToBcd32(datetime.min) & 0xff;
	*pbuf++ = Hex32ToBcd32(datetime.sec) & 0xff;
	*pbuf++ = 0xFF;
	memset(pbuf ,0x00, 28);
	pbuf += 28;
	*pbuf++ = 26; //׮��̨ͨ��Э��汾�� ��Э����V2.6
	*pbuf++ = 0;
	*pbuf++ = addChecksum(buffer + 6, pbuf - buffer - 6);
	
	NetSend(port, buffer, pbuf - buffer);
	print("CMD106 ���ͳ��׮ǩ����ϢP%d:", port);
	printx(buffer, pbuf - buffer);
}

/************************************************************************************************************
** �� �� �� : RecvCMD203
** �������� : ���շ�����Ӧ���ʻ���ѯ��Ϣ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD203(uint8_t port, uint8_t *buffer)
{

	memcpy(&ReportCMD204[port].ReportRet, &buffer[12], 1);
	ReportCMD204[port].OnlineCardIDBalance  = buffer[16] | (buffer[17] << 8) | (buffer[18] << 16) | (buffer[19] << 24);
	ReportCMD204[port].ReportFlag = 2;
//	balance = buffer[16] | (buffer[17] << 8) | (buffer[18] << 16) | (buffer[19] << 24);
//	passwdflag = buffer[24];
//	VINflag = buffer[25];
}

/************************************************************************************************************
** �� �� �� : RecvCMD1011
** �������� : ���շ������·�����ָ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void RecvCMD1011(uint8_t port, uint8_t *buffer)
{
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf;

	pbuf = ackbuffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x0D; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x00; //���к���
	*pbuf++ = 0xF4; //�������
	*pbuf++ = 0x03;
	memset(pbuf, 0, 4);//������
	pbuf += 4;
	*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
	
	NetSend(port, ackbuffer, pbuf - ackbuffer);
	print("CMD1012 ���׮Ӧ��������·���������P%d:", port);
	printx(ackbuffer, pbuf - ackbuffer);
	
	print("׼�������豸..................\r\n");

	if(UpdataFlag){
		SetUpdataFlag(0xAA);
	}
	OSTimeDlyHMSM (0, 0, 3, 0);
	//�Ƿ���Ҫ��������������
	NVIC_SystemReset();
}

/************************************************************************************************************
** �� �� �� : RecvCMD1101
** �������� : ���պ�̨��������ѯ24ʱ��ѼƼ۲�����Ϣ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD1101(uint8_t port, uint8_t *buffer)
{
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf, *p;
	uint32_t i,data32;
	 
	p = &ackbuffer[8];
	for(i = 0; i < 12; i++){
		ParaGetFeilvTime(port, i, &p[0], &p[1], &p[2], &p[3]);
		data32 = ParaGetFeilvMoney(port, i) / 1000;
		memcpy(&p[4], &data32, 4);
		p += 8;
	}

	pbuf = ackbuffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x69; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x00; //���к���
	*pbuf++ = 0x4E; //�������
	*pbuf++ = 0x04;
	pbuf += 96;//������
	*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
	
	NetSend(port, ackbuffer, pbuf - ackbuffer);
	print("CMD1102 ���׮Ӧ���̨��������ѯ24С�Ƽ۲�����ϢP%d:", port);
	printx(ackbuffer, pbuf - ackbuffer);
}


/************************************************************************************************************
** �� �� �� : RecvCMD1103
** �������� : ���պ�̨����������24ʱ��ѼƼ۲�����Ϣ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD1103(uint8_t port, uint8_t *buffer)
{
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf, ret = 1, *p;
	uint32_t i, data32;
	 
	p = &buffer[8];
	for(i = 0; i < 12; i++){
		if((p[0] > 24) || (p[2] > 24)){
			break;
		}
		if(((p[1] != 0) && (p[1] != 30)) || ((p[3] != 0) && (p[3] != 30))){
			break;
		}
		if(((p[0] == p[2]) && (p[0] != 0)) && ((p[1] == p[3]) && (p[1] != 0))){
			break;
		}
		p += 8;
	}
	if(i == 12){
		ret = 0;
		p = &buffer[8];
		for(i = 0; i < 12; i++){
			ParaSetFeilvTime(port, i, p[0], p[1], p[2], p[3]);
			data32 = p[4] | (p[5] << 8) | (p[6] << 16) | (p[7] << 24);
			ParaSetFeilvMoney(port, i, data32 * 1000);
			p += 8;
		}
	}

	pbuf = ackbuffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x0A; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x00; //���к���
	*pbuf++ = 0x50; //�������
	*pbuf++ = 0x04;
	*pbuf++ = ret; //������
	*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
	
	NetSend(port, ackbuffer, pbuf - ackbuffer);
	print("CMD1104 ���׮Ӧ���̨����������24С�Ƽ۲�����ϢP%d:", port);
	printx(ackbuffer, pbuf - ackbuffer);
}


/************************************************************************************************************
** �� �� �� : RecvCMD1001
** �������� : ���պ�̨�������·�����ָ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD1001(uint8_t port, uint8_t *buffer)
{
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf, ret;
	if (port != 0)
		return;
	if((buffer[8]== 0x55) && (buffer[9] == 0xaa)){
		if(ParaGetChgGunNumber() == 2){
			 if((GetMainCharge(0) ==STATE_IDLE) &&( GetMainCharge(1) ==STATE_IDLE)){
			 	begin_update(); //�����洢������
				ret = 100;
				print("�����洢������������ȷP%d:\r\n", port);
				UpdataTimer = TimerRead();
				UpdataFlag = 0x01;
			 }else{
				ret = 0;
				print("����н�ֹ����\r\n");
			 }
		}else{
			if(GetMainCharge(0) ==STATE_IDLE ){
			 	begin_update(); //�����洢������
				ret = 100;
				print("�����洢������������ȷP%d:\r\n", port);
				UpdataFlag = 0x01;
				UpdataTimer = TimerRead();
			 }else{
				ret = 0;
				print("����н�ֹ����\r\n");
			 }
		}
	}else{
		ret = 0;
		print("�����洢���������������P%d\r\n", port);
		printx((buffer+8), 4);
	}
	pbuf = ackbuffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x0A; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x00; //���к���
	*pbuf++ = 0xEA; //�������
	*pbuf++ = 0x03;
	*pbuf++ = ret; //������   
	*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
	
	NetSend(port, ackbuffer, pbuf - ackbuffer);
	print("CMD1002 ���׮Ӧ���̨��������������P%d:", port);
	printx(ackbuffer, pbuf - ackbuffer);
}




/************************************************************************************************************
** �� �� �� : RecvCMD1003
** �������� : ���պ�̨���������ļ���ָ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD1003(uint8_t port, uint8_t *buffer)
{
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf;
	uint32_t data32;
	uint16_t datalen;
	if (port != 0)
		return;
	datalen = buffer[2]|(buffer[3]<<8);
	//�ļ����洢
	memcpy(FileName, &buffer[8], datalen - 9); 
	print("CMD1004 ���׮Ӧ���̨�������·������ļ���P%d  : %s\r\n", port, FileName);
	UpdataTimer = TimerRead();
	pbuf = ackbuffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x0D; // ������  9+4
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x00; //���к���
	*pbuf++ = 0xEC; //�������
	*pbuf++ = 0x03;
	data32 = 512;
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
	
	NetSend(port, ackbuffer, pbuf - ackbuffer);
	print("CMD1004 ���׮Ӧ���̨�������·������ļ���ָ��P%d:", port);
	printx(ackbuffer, pbuf - ackbuffer);
}


/************************************************************************************************************
** �� �� �� : RecvCMD1005
** �������� : ���պ�̨�������������ļ���Сָ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��     
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD1005(uint8_t port, uint8_t *buffer)
{
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf;
	uint32_t  data32;
	if (port != 0)
		return;
  UpdataFileSize = buffer[8] | (buffer[9]<<8) | (buffer[10]<<16) |(buffer[11]<< 24);
	if( UpdataFileSize > (224*1024)){  //�����ļ����ڳ���洢��
		data32 = 1;
		UpdataFileSize = 0;
		UpdataFlag = 0;
	}else{
		data32 = 0;
		
	}
	print("�����ļ���СΪP%d  ��  %d\r\n", port, UpdataFileSize);
	UpdataTimer = TimerRead();
	pbuf = ackbuffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x0D; //������  9+4
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x00; //���к���
	*pbuf++ = 0xEE; //�������
	*pbuf++ = 0x03;
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
	NetSend(port, ackbuffer, pbuf - ackbuffer);
	print("CMD1006 ���׮Ӧ���̨�������·������ļ���Сָ��P%d:", port);
	printx(ackbuffer, pbuf - ackbuffer);
	FileIndexNum = 0;
}













/************************************************************************************************************
** �� �� �� : RecvCMD1007
** �������� : ���պ�̨�������������ļ���������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��   
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD1007(uint8_t port, uint8_t *buffer)
{
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf, ret, SN;
	uint16_t datalen;
	uint8_t IndexNum;
	if (port != 0)
		return;
	IndexNum = buffer[5];
	datalen = buffer[2]|(buffer[3]<<8);
//	print("CMD1007 �������У�%d  FileIndexNum = %d ���ݳ���= %d\r\n", IndexNum, FileIndexNum, datalen -9 );
	UpdataTimer = TimerRead();
	buffer = buffer + 8;
	if(IndexNum == FileIndexNum){
		SN = 0x00;
		ret = process_update(UpdataFileSize, buffer,datalen -9);//д���ݵ� ����洢��
		if(ret == 0)
			FileIndexNum++;
	}else{
		ret = 0x01;
		SN = 0x01;
	}

	pbuf = ackbuffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x11; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x00; //���к���
	*pbuf++ = 0xF0; //�������
	*pbuf++ = 0x03;
	*pbuf++ = SN; //��ȷ���յ�SN
	*pbuf++ = ret; //д��ɹ�
	*pbuf++ = 0x00; //Ԥ��
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
	if(	UpdataFlag ){   
		NetSend(port, ackbuffer, pbuf - ackbuffer);
		print("CMD1008 ���׮Ӧ���̨�������·���������ָ��P%d:", port);
		printx(ackbuffer, pbuf - ackbuffer);
	}

}


/************************************************************************************************************
** �� �� �� : GetUpdataPre
** �������� : ��ȡ�����ٷֱ�
** ��    �� : ��
** ��    �� : ��    
** ��    �� :	�� 8 Bit  Ϊ1 ��ʾ��������    ǰ8λ��ʾ �����ٷֱ� 
*************************************************************************************************************
*/
uint16_t GetUpdataPre(void)
{
	uint16_t Pre = 0;
	uint8_t SumIndex;
	if( UpdataFlag ){
	 Pre = (1 << 8);
	}
	if( TimerRead() - UpdataTimer > T1M){
		UpdataFlag = 0;
		Pre = 0;
	}
	if((UpdataFileSize % 512) == 0){
		SumIndex = (UpdataFileSize/512) +1;
	}else{
		SumIndex = UpdataFileSize/512;
	}
	Pre |= (FileIndexNum *100) / SumIndex;
	return Pre;
}








/************************************************************************************************************
** �� �� �� : RecvCMD1009
** �������� : ���պ�̨�������·������ļ����ݽ���ָ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��    
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD1009(uint8_t port, uint8_t *buffer)
{
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf;
	if (port != 0)
		return;
	end_update();
	UpdataTimer = TimerRead();
	pbuf = ackbuffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0x0D; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x00; //���к���
	*pbuf++ = 0xF2; //�������
	*pbuf++ = 0x03;
	*pbuf++ = 0x00; //Ԥ�� 
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = addChecksum(ackbuffer + 6, pbuf - ackbuffer - 6);
	if(UpdataFlag ){
		NetSend(port, ackbuffer, pbuf - ackbuffer);
		print("CMD1010 ���׮Ӧ��������·������ļ����ݽ���ָ��P%d:", port);
		printx(ackbuffer, pbuf - ackbuffer);
	}
}

/************************************************************************************************************
** �� �� �� : SendCMD602
** �������� : ���׮Ӧ��������·������������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   res ִ�н�� 0��ʾ�ɹ���������ʾʧ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void SendCMD602(uint8_t port, uint8_t res)
{
	uint8_t buffer[COMMBUFFERLEN], *pbuf;
	pbuf = buffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ =  19;		 //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x41; //���к���
	*pbuf++ = 0x5A; //�������  602
	*pbuf++ = 0x02;
	
	*pbuf++ = 0x00;//������
	*pbuf++ = 0x00;//������
	*pbuf++ = 0x00;//������
	*pbuf++ = 0x00;//������
	*pbuf++ = GetTrickleSoc();
	*pbuf++ = (GetTrickleCur() &0xFF);
	*pbuf++ = ((GetTrickleCur()>>8) &0xFF);
	*pbuf++ = (GetTrickleMin() &0xFF);
	*pbuf++ = ((GetTrickleMin()>>8) &0xFF);
	*pbuf++ = res;
	*pbuf++ = addChecksum(buffer + 6, pbuf - buffer - 6);
	NetSend(port, buffer, pbuf - buffer);
	print("CMD602  ���׮Ӧ��������·������������%d:", port);
	printx(buffer, pbuf - buffer);
	
}


/************************************************************************************************************
** �� �� �� : RecvCMD601
** �������� : ���պ�̨�������·������������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��    
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD601(uint8_t port, uint8_t *buffer)
{
	uint8_t LimitSoc;
	uint16_t LimitMin;
	uint32_t LimitCur;
	LimitSoc = buffer[12];
	LimitCur = (buffer[13]|(buffer[14]<<8));
	LimitMin = (buffer[15]|(buffer[16]<<8));
	print("LimitSoc: %d��LimitCur:%d LimitMin: %d\r\n", LimitSoc,LimitCur, LimitMin);
	SetTrickleInfo(LimitCur, LimitSoc, LimitMin);
	SendCMD602(port, 0);
}


/************************************************************************************************************
** �� �� �� : SendCMD604
** �������� : Ӧ���̨�������·�������Ʋ�ѯ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��    
** ��    �� :	��
*************************************************************************************************************
*/
static void SendCMD604(uint8_t port)
{
	uint8_t buffer[COMMBUFFERLEN], *pbuf;
	pbuf = buffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 18;		 //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x41; //���к���
	*pbuf++ = 0x5C; //������� 604
	*pbuf++ = 0x02;
	
	*pbuf++ = 0x00;//������
	*pbuf++ = 0x00;//������
	*pbuf++ = 0x00;//������
	*pbuf++ = 0x00;//������
	*pbuf++ = GetTrickleSoc();
	*pbuf++ = (GetTrickleCur() &0xFF);
	*pbuf++ = ((GetTrickleCur()>>8) &0xFF);
	*pbuf++ = (GetTrickleMin() &0xFF);
	*pbuf++ = ((GetTrickleMin()>>8) &0xFF);
	*pbuf++ = addChecksum(buffer + 6, pbuf - buffer - 6);
	NetSend(port, buffer, pbuf - buffer);
	print("CMD604  ���׮Ӧ��������Ʋ�ѯ%d:", port );
	printx(buffer, pbuf - buffer);
}
/************************************************************************************************************
** �� �� �� : RecvCMD603
** �������� : ���պ�̨�������·�������Ʋ�ѯ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��    
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD603(uint8_t port, uint8_t *buffer)
{
	SendCMD604(port);
}

/************************************************************************************************************
** �� �� �� : SendCMD702
** �������� : ���ͳ�����޸Ľ��Ӧ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......  Result�� 0�ɹ�  ����ʧ��
** ��    �� : ��    
** ��    �� :	��
*************************************************************************************************************
*/
static void SendCMD702(uint8_t port, uint8_t Result)
{
	uint8_t buffer[COMMBUFFERLEN], *pbuf;
	pbuf = buffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 24;		  //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x41; //���к���
	*pbuf++ = 0xBE; //������� 702
	*pbuf++ = 0x02;
	
	*pbuf++ = 0x00;//������
	*pbuf++ = 0x00;//������
	*pbuf++ = 0x00;//������
	*pbuf++ = 0x00;//������
	
	
	*pbuf++ = (ModifInfo[port].UserId & 0xFF); //USerID
	*pbuf++ = ((ModifInfo[port].UserId >> 8) & 0xFF);
	
	*pbuf++ = (ModifInfo[port].LastMoney & 0xFF);  //����֮ǰ�����
	*pbuf++ = ((ModifInfo[port].LastMoney >> 8) & 0xFF);
	*pbuf++ = ((ModifInfo[port].LastMoney >> 16) & 0xFF);
	*pbuf++ = ((ModifInfo[port].LastMoney >> 24) & 0xFF);
	
	*pbuf++ = (ModifInfo[port].NewMoney & 0xFF); //��ǰ���
	*pbuf++ = ((ModifInfo[port].NewMoney >> 8) & 0xFF);
	*pbuf++ = ((ModifInfo[port].NewMoney >> 16) & 0xFF);
	*pbuf++ = ((ModifInfo[port].NewMoney >> 24) & 0xFF);
	
	*pbuf++ = Result;  //ִ�н��
	
	*pbuf++ = addChecksum(buffer + 6, pbuf - buffer - 6);
	NetSend(port, buffer, pbuf - buffer);
	print("CMD702 ���׮Ӧ���޸ĳ�������%d:", port );
	printx(buffer, pbuf - buffer);
}

/************************************************************************************************************
** �� �� �� : UpdateMoneyJudge
** �������� : ����и��½���ж�
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ....     UserId:�û�ID  RawMoney:�����ʼ���
** ��    �� : ��    
** ��    �� :	���º�Ľ��  0��������
*************************************************************************************************************
*/
uint32_t UpdateMoneyJudge(uint8_t port, uint16_t UserId, uint32_t RawMoney)
{
	if(ModifInfo[port].UpdataFlag == 0x5A){	
		if(UserId == ModifInfo[port].UserId){
			ModifInfo[port].LastMoney = RawMoney;
			ModifInfo[port].UpdataFlag = 0xA5;
			return ModifInfo[port].NewMoney;
		}else{
			ModifInfo[port].LastMoney = 0;
			ModifInfo[port].NewMoney = 0; 
			ModifInfo[port].UpdataFlag = 1;
			return 0;
		}
	}
	return 0;
}



/************************************************************************************************************
** �� �� �� : JudgeSendCMD702
** �������� : �ظ��޸ĳ�����޸Ľ�� 
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ.... 
** ��    �� : ��    
** ��    �� :	��
*************************************************************************************************************
*/
static void JudgeSendCMD702(uint8_t port)
{
	if((ModifInfo[port].UpdataFlag == 0xA5) ||( ModifInfo[port].UpdataFlag == 0x5A  )|| ( ModifInfo[port].UpdataFlag == 0x01 )){
			if(ModifInfo[port].UpdataFlag == 0xA5){
				ModifInfo[port].UpdataFlag = 0;
				SendCMD702(port, 0);  //�����޸Ľ��ɹ�			
			}else if( ModifInfo[port].UpdataFlag == 0x01  ){
				ModifInfo[port].UpdataFlag = 0;
				SendCMD702(port, 1);  //�����޸Ľ��ʧ��				
			}else if((ModifInfo[port].UpdataFlag == 0x5A) && (GetMainCharge(port) != STATE_CHARGE)) {  //����ֹͣ��緵�� 
				ModifInfo[port].UpdataFlag = 0;
				SendCMD702(port, 1);  //�����޸Ľ��ʧ��			
			}	
	}else{
		ModifInfo[port].UpdataFlag = 0;	
	}
}

/************************************************************************************************************
** �� �� �� : RecvCMD701
** �������� : ���պ�̨�������·�������޸Ľ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��    
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD701(uint8_t port, uint8_t *buffer)
{
	ModifInfo[port].UserId = ( buffer[12] | ( buffer[13] << 8 ) );
	ModifInfo[port].NewMoney = ( buffer[14] | ( buffer[15] << 8 ) | ( buffer[16] << 16 )|( buffer[17] << 24 ));
	ModifInfo[port].UpdataFlag = 0x5A;
	print("UserId: %d  NewMoney: %d\r\n",ModifInfo[port].UserId,  ModifInfo[port].NewMoney);
}

/************************************************************************************************************
** �� �� �� : FrameFormatCheck
** �������� : ֡��ʽ��⺯��
** ��    �� : buffer ������ length ����������
** ��    �� : ��
** ��    �� :	���ɹ�����1 ���򷵻�0
*************************************************************************************************************
*/
static uint8_t FrameFormatCheck(const uint8_t* buffer)
{
	uint16_t len;
	uint8_t checksum;
	
	if(buffer[0] != STARTCODE1){
		return 0;
	}
	if(buffer[1] != STARTCODE2){
		return 0;
	}
	len = buffer[2] | (buffer[3] << 8);
	if(len < 9){
		return 0;
	}
	checksum = addChecksum(buffer + 6, len - 6 - 1);
	if(checksum != buffer[len - 1]){
		return 0;
	}
	return 1;
}


/************************************************************************************************************
** �� �� �� : UnpackMsg
** �������� : �������ݰ�
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... buffer ���ݰ�
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
static void UnpackMsg(uint8_t port,uint8_t *buffer)
{
	uint16_t cmdcode;
	
	cmdcode = buffer[6] + (buffer[7] << 8);
	switch(cmdcode){
		case 1: //��̨�������·����׮���͹�������
			print("CMD1 ��̨�������·����׮���͹�������P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD1(port, buffer);
			break;
		case 3: //��̨�������·����׮�ַ��͹�������
			print("CMD3 ��̨�������·����׮�ַ��͹�������P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD3(port, buffer);
			break;
		case 5: //��̨�������·����׮��������
			print("CMD5 ��̨�������·����׮��������P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD5(port, buffer);
			break;
		case 7: //��̨�������·����׮��������������
			print("CMD7 ��̨�������·����׮��������������P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD7(port, buffer);
			break;
		case 101: //������Ӧ����������Ϣ
			print("CMD101 ������Ӧ����������ϢP%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			CommInfo[port].heartbeatackflag = 1;
			break;
		case 103: //������Ӧ����׮״̬��Ϣ��
			print("CMD103 ������Ӧ����׮״̬��Ϣ��P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			break;
		case 105: //������Ӧ����׮ǩ������
			print("CMD105 ������Ӧ����׮ǩ������P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			CommInfo[port].loginackflag = 1;
			break;
		case 201: //������Ӧ����׮�ϱ������Ϣ����
			print("CMD201 ������Ӧ����׮�ϱ�����¼��Ϣ����P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			CommInfo[port].AckCMD201Flag = 1;
			break;
		case 203: //������Ӧ���ʻ���ѯ��Ϣ
			print("CMD203 ������Ӧ���ʻ���ѯ��ϢP%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD203(port, buffer);
			break;
		case 601://��̨�������·������������
			print("CMD601 ��̨�������·������������P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD601(port, buffer);
			break;	
		case 603://��̨�������·�������Ʋ�ѯ
			print("CMD603 ��̨�������·�������Ʋ�ѯP%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD603(port, buffer);
			break;
		case 701://��̨�������·��޸Ľ��
			print("CMD701 ��̨�������·��޸Ľ��P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD701(port, buffer);
			break;
		case 1011: //�������·�����ָ��
			print("CMD1011 �������·�����ָ��P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD1011(port, buffer);
			break;
		case 1101: //��̨��������ѯ24ʱ��ѼƼ۲�����Ϣ
			print("CMD1101 ��̨��������ѯ24ʱ��ѼƼ۲�����ϢP%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD1101(port, buffer);
			break;
		case 1103: //��̨����������24ʱ��ѼƼ۲�����Ϣ
			print("CMD1103 ��̨����������24ʱ��ѼƼ۲�����ϢP%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD1103(port, buffer);
			break;
		case 1001: //��̨�������������·�����ָ��
			print("CMD1001 ��̨�������·�����ָ��P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD1001(port, buffer);
			break;
		case 1003: //��̨�������������·�����ָ��
			print("CMD1003 ��̨�������·������ļ���ָ��P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD1003(port, buffer);
			break;
		case 1005: //��̨�������������·�����ָ��
			print("CMD1005 ��̨�������·������ļ���Сָ��P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD1005(port, buffer);
			break;
		case 1007: //��̨�����������������ļ�����
			print("CMD1007 ��̨�������·������ļ�����P%d \r\n", port);
			RecvCMD1007(port, buffer);
			break;
		case 1009: //��̨�����������������ļ�����
			print("CMD1009 ��̨�������·������ļ����ݽ���ָ��P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD1009(port, buffer);
			break;
		default:
			break;
	}
}

/************************************************************************************************************
** �� �� �� : AnalyseMsgFromMSA
** �������� : ����������վ����Ϣ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void AnalyseMsgFromMSA(uint8_t port)
{
	uint32_t i,j,len;
	uint16_t data16;
	
	do{
		len = NetRecv(port, &CommInfo[port].buffer[CommInfo[port].len], COMMBUFFERLEN - CommInfo[port].len);
		CommInfo[port].len += len;
		if(CommInfo[port].len >= COMMBUFFERLEN){
			CommInfo[port].len = 0;
			break;
		}
	}while(len);
	
	i = 0;
	do{
		if(FrameFormatCheck(&CommInfo[port].buffer[i]) == 1){
			UnpackMsg(port, &CommInfo[port].buffer[i]); //��������
			data16 = CommInfo[port].buffer[i + 2] + (CommInfo[port].buffer[i + 3] << 8);
			for(j = 0; j < CommInfo[port].len; j++){
				CommInfo[port].buffer[j] = CommInfo[port].buffer[i + j + data16];
			}
			CommInfo[port].len -= data16;
			i = 0;
		} else {
			i++;
		}
	}while(i < CommInfo[port].len);
}

/************************************************************************************************************
** �� �� �� : AccountQueryReport
** �������� : ���׮����ϴ��û��ʻ���ѯ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
static void AccountQueryReport(uint8_t port)
{

	static uint32_t report204timer[2];
	
	if(port > 1)
		return ;
//	if(CommInfo[port].SendCmd204 == 1){
//		CommInfo[port].SendCmd204 = 0;
//		SendCMD204(port);
//	}
	  
	if(ReportCMD204[port].ReportFlag == 1){
		if(TimerRead() - report204timer[port] > T1S * 3){
			report204timer[port] = TimerRead();
			SendCMD204(port);
		}
	}
}



/************************************************************************************************************
** �� �� �� : ReportChargeRecord
** �������� : �ϱ����׼�¼
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
static void ReportChargeRecord(uint8_t port)
{
	static uint32_t Timer[2];
	static uint32_t TimerCount[2];
	static uint32_t IsStopReport[2];
	CHARGINGDATA ChgDat;
	uint16_t index,i;
	ChgDat.ChgPort = 0x5a;
	if (CommInfo[port].AckCMD201Flag == 1){//��̨Ӧ�����¼
		CommInfo[port].AckCMD201Flag = 0;
		for (i = 0; i < REPORT_NUM - 1; i++)
			CommInfo[port].ReportCtl[i] = CommInfo[port].ReportCtl[i + 1];
			CommInfo[port].NeedReportNum--;
			Timer[port] = 0;
	}
	if (TimerRead() - Timer[port] > T1S * 10){
		if ((CommInfo[port].ChargeRecordNumberBak != GetChargeRecordNumber(port)) ||	CommInfo[port].ChargeRecordIndexBak != GetChargeRecordIndex(port)){//�½��׼�¼
			CommInfo[port].ChargeRecordNumberBak = GetChargeRecordNumber(port);
			index = GetChargeRecordIndex(port);
			CommInfo[port].ChargeRecordIndexBak = index;
			IsStopReport[port] = 0;  
			if(GetChargeRecord(port, &ChgDat)){			
				for (i = 0; i < REPORT_NUM - 1; i++)
					CommInfo[port].ReportCtl[REPORT_NUM - 1 - i] = CommInfo[port].ReportCtl[REPORT_NUM - 2 - i];
					CommInfo[port].ReportCtl[0].index = index;
					CommInfo[port].NeedReportNum++;	
			}else if (FindChargeRecord(index,  (uint8_t *)&ChgDat, sizeof(CHARGINGDATA)) == 1){//��ѯ�ɹ�
				for (i = 0; i < REPORT_NUM - 1; i++)
					CommInfo[port].ReportCtl[REPORT_NUM - 1 - i] = CommInfo[port].ReportCtl[REPORT_NUM - 2 - i];
					CommInfo[port].ReportCtl[0].index = index;
					CommInfo[port].NeedReportNum++;
			}
		}
		//����·����ָֹͣ��ȴ��µĽ��׼�¼����ϱ�
		if(CommInfo[port].stopflag == 1){
			if(IsStopReport[port] == 0){
				TimerCount[port] = TimerRead();
				IsStopReport[port] = 1;
			}
		}
		if(TimerRead() - TimerCount[port] > T1S * 30){
			IsStopReport[port] = 0;
		}
		if(IsStopReport[port] != 1){
			if(CommInfo[port].NeedReportNum > 0){
				if((CommInfo[port].ReportCtl[0].index == index)||(CommInfo[port].ReportCtl[0].index == index-1) || ((index == 0)&& (CommInfo[port].ReportCtl[0].index == RECORD_TOTAL_NUM-1))) {
					if(GetChargeRecord(port, &ChgDat)){
					}else{ 
						FindChargeRecord(CommInfo[port].ReportCtl[0].index,  (uint8_t *)&ChgDat, sizeof(CHARGINGDATA));
					}
				}else{
					FindChargeRecord(CommInfo[port].ReportCtl[0].index,  (uint8_t *)&ChgDat, sizeof(CHARGINGDATA));
				}
				if(ChgDat.ChgPort == port){
					SendCMD202(port, &ChgDat,CommInfo[port].ReportCtl[0].index);
				}else{
					for (i = 0; i < REPORT_NUM - 1; i++)
						CommInfo[port].ReportCtl[i] = CommInfo[port].ReportCtl[i + 1];
						CommInfo[port].NeedReportNum--;
						Timer[port] = 0;
				}
			}
		}
		if(ChgDat.ChgPort == port)
			Timer[port] = TimerRead();
	}
}

/************************************************************************************************************
** �� �� �� : ProtocolHandle
** �������� : ͨ��Э�鴦����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
static void ProtocolHandle(uint8_t port)
{
	static uint8_t state[2];
	static uint8_t logincount[2], heartbeatcount[2];
	static uint32_t timer[2], reportstatetimer[2];
	static uint32_t reportstartchargeresulttimer[2];
	static uint8_t chgstate[2];
	uint8_t sip[4];
	uint16_t sport, peroid;
	
	switch(state[port]){
		case 0: //���ӷ�����
			CommInfo[port].loginackflag = 0;
			ParaGetServerIp(port, sip);
			sport = ParaGetServerPort(port);
			if (NetConn(port, sip, sport) != 1)
				break;
			memset(CommInfo[port].buffer, 0, COMMBUFFERLEN);
			CommInfo[port].len = 0;
			logincount[port] = 0;
			state[port]++;
			print("Connect Success P%d \r\n", port);
			
		case 1: //�������ǩ��
			logincount[port]++;
			if (logincount[port] > 3){
				state[port]--;
				break;
			}
			SendCMD106(port);
			timer[port] = TimerRead();
			state[port]++;
		
		case 2: //�ȴ�������Ӧ��ǩ��
			AnalyseMsgFromMSA(port);
			if(TimerRead() - timer[port] > T1S * 5){
				state[port]--;
				break;
			}
			if(CommInfo[port].loginackflag != 1)
				break;
			CommInfo[port].heartbeatackflag = 0;
			timer[port] = TimerRead();
			heartbeatcount[port] = 0;
			state[port]++;
		
		case 3: //��ʱ����
			AnalyseMsgFromMSA(port);
			if ((peroid = ParaGetNetHeartTime(port)) < 12)
				peroid = 12;
			if (TimerRead() - timer[port] > T1S * peroid){
				timer[port] = TimerRead();
				if (CommInfo[port].heartbeatackflag == 1){
					heartbeatcount[port] = 0;
				}
				if (heartbeatcount[port]++ > ParaGetNetCommOverCnt(port)){
					state[port] = 0;
					break;
				} else {
					CommInfo[port].heartbeatackflag = 0;
					SendCMD102(port);
				}
			}
				
			//��ʱ�ϱ�״̬��Ϣ
			AnalyseMsgFromMSA(port);
			if ((peroid = ParaGetStateInfoReportPeriod(port)) < 20)
				peroid = 20;
			if( (TimerRead() - reportstatetimer[port] > T1S * peroid )){
				reportstatetimer[port] = TimerRead();
				SendCMD104(port);
			}
			if( ChangeStatus(port) )
			{
				SendCMD104(port);
			}
			JudgeSendCMD702(port);
			//��ʱ�ϱ�δӦ��Ľ��׼�¼
			ReportChargeRecord(port);

		
			//�������ݽ���
			AnalyseMsgFromMSA(port);
			AccountQueryReport(port);
			
			break;
		
		default:
			state[port] = 0;
			break;
	}
	
	//������
	switch(chgstate[port]){
		case 0://����
			if(CommInfo[port].stopflag == 1){
				CommInfo[port].stopflag = 0;
			}
			if(CommInfo[port].startflag == 1){
				CommInfo[port].startflag = 0;
				ModifInfo[port].UpdataFlag = 0;
				if(CommInfo[port].chargestrategy == 1)//��ʱ��
					SetMainCharge(port, SET_START_CHARGE, 2, CommInfo[port].chargepara, CommInfo[port].ChargeCardNumber, CommInfo[port].UserId);
				else if(CommInfo[port].chargestrategy == 2)//�����
					SetMainCharge(port, SET_START_CHARGE, 3, CommInfo[port].chargepara, CommInfo[port].ChargeCardNumber, CommInfo[port].UserId);
				else if(CommInfo[port].chargestrategy == 3)//������
					SetMainCharge(port, SET_START_CHARGE, 1, CommInfo[port].chargepara, CommInfo[port].ChargeCardNumber, CommInfo[port].UserId);
				else
					SetMainCharge(port, SET_START_CHARGE, 0, CommInfo[port].chargepara, CommInfo[port].ChargeCardNumber, CommInfo[port].UserId);
				reportstartchargeresulttimer[port] = TimerRead();
				chgstate[port]++;
				CommInfo[port].stopflag = 0;
			}
			break;	
			
		case 1://�ȴ��������
			if (GetMainCharge(port) == STATE_CHARGE){//�����
				SendCMD8(port, 0);
				chgstate[port]++;
			}
			if (GetMainCharge(port) == STATE_COMPLETE){//������
				SetMainCharge(port, SET_STOP_CHARGE, 0 , 0, 0, 0);
				SendCMD8(port, 100001);
				chgstate[port] = 0;
				CommInfo[port].startflag = 0;
			}
			if (TimerRead() - reportstartchargeresulttimer[port] > T1S * 120){//��ʱ
				SetMainCharge(port, SET_STOP_CHARGE, 0 , 0, 0, 0);
				SendCMD8(port, 100002);
				chgstate[port] = 0;
				CommInfo[port].startflag = 0;
			}
			break;
			
		case 2://�����
			if (GetMainCharge(port) != STATE_CHARGE){//�ǳ����
//				ReportChargeRecord(port);
				chgstate[port] = 0;
				CommInfo[port].startflag = 0;
			}
			if(CommInfo[port].stopflag == 1){//��̨�·�ֹͣ���
				CommInfo[port].stopflag = 0;
				SetMainCharge(port, SET_STOP_CHARGE, 0 , 0, 0, 0);
//				ReportChargeRecord(port);
				chgstate[port] = 0;
				CommInfo[port].startflag = 0;
			}
			break;
	}
}


/************************************************************************************************************
** �� �� �� : ProtocolInit
** �������� : ��ʼ������
** ��    �� : ��
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
static void ProtocolInit(void)
{
	uint8_t port, PortNum = ParaGetChgGunNumber();
	uint16_t index,num,i;
	CHARGINGDATA ChgDat;
	
	memset(&CommInfo, 0, sizeof(COMMINFO));
	for(port = 0; port < PortNum; port++){
		index = GetChargeRecordIndex(port);
		CommInfo[port].ChargeRecordIndexBak = index;
		num = GetChargeRecordNumber(port) % REPORT_NUM;
		if((GetChargeRecordNumber(port) / REPORT_NUM) != 0)
			num = REPORT_NUM;
		CommInfo[port].ChargeRecordNumberBak = num;
		for (i = 0; i < num; i++){
			if (FindChargeRecord(index,  (uint8_t *)&ChgDat, sizeof(CHARGINGDATA)) == 1)//��ѯ�ɹ�
				CommInfo[port].ReportCtl[CommInfo[port].NeedReportNum++].index = index;
			if (index == 0)
				index = RECORD_TOTAL_NUM;
			else
				index --;
		}
	}
}

/************************************************************************************************************
** �� �� �� : TaskProtocolHandle
** �������� : ͨ��Э�鴦������
** ��    �� : pdata δʹ��
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
void TaskProtocolHandle(void *pdata)
{
	uint8_t port;
	uint8_t PortNum = ParaGetChgGunNumber();
	
	ProtocolInit();
	while(1){
		for(port = 0; port < PortNum; port++){
			ProtocolHandle(port);
			OSTimeDlyHMSM (0, 0, 0, 10);
		}
	}
}



