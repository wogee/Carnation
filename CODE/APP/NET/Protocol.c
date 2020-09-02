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
#include <LPC177x_8x.H>
#include "UART.h"
#include "MainTask.h"
#include "ChgTask.h"
#include "MyAlgLib.h"
#include <stdio.h>
#include <math.h>
#include "ElmTask.h"

static COMMINFO CommInfo[2];

static struct{
	uint32_t ReportOnlineCardID;//�ϱ����߿�����
	uint8_t ReportFlag;//�ϱ�204���ı�־ 0�����ϱ� 1���ϱ� 2�ϱ��ѵõ��ظ�
	uint8_t ReportRet;//�ϱ�204�����յ��Ľ�� 0��Ч�ʻ� 1�Ƿ��ʻ� 2���� 3�Ѻڿ��ָ��������� 4����������ڿ� 5�Ƿ����� 6��ʧ 7���� 8������� 9�ÿ��Ѿ��ڱ��׮���
	uint32_t OnlineCardIDBalance;//���߿���� ��λ0.01Ԫ
}ReportCMD204[2];

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
	uint8_t buffer[COMMBUFFERLEN], *pbuf,i;
	
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
	ParaGetPileNo(0, pbuf);
	for (i = 0; i < 32; i++){
		if (pbuf[i] == 0x00){
			pbuf[i] = port + 'A';
			break;
		}
	}
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
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf, type,ret;
	uint32_t paraaddr,num,i,paranum,data32;
	
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
			num =0;
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
		ParaGetPileNo(0, pbuf);
		for (i = 0; i < 32; i++){
			if (pbuf[i] == 0x00){
				pbuf[i] = port + 'A';
				break;
			}
		}
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
		ParaGetPileNo(0, pbuf);
		for (i = 0; i < 32; i++){
			if (pbuf[i] == 0x00){
				pbuf[i] = port + 'A';
				break;
			}
		}
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
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf, type,meteraddr[6],qrcode[256],oldpileno[32],oldpilenoflag = 0;
	uint32_t paraaddr,num,i;
	Rtc datetime;
	
	paraaddr = buffer[13] | (buffer[14] << 8) | (buffer[15] << 16) | (buffer[16] << 24);
	type = buffer[12];
	if(type == 0x00){ //��ѯ
		memset(ackbuffer, 0, sizeof(ackbuffer));
		switch(paraaddr){
			case 1: //���׮����
//				ParaGetPileNo(port, &ackbuffer[50]);
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
		ParaGetPileNo(0, pbuf);
		for (i = 0; i < 32; i++){
			if (pbuf[i] == 0x00){
				pbuf[i] = port + 'A';
				break;
			}
		}
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
				ParaGetPileNo(port, oldpileno);
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
				//ParaSetDCMeterAddr(port, meteraddr);
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
			memcpy(pbuf, oldpileno, 32);
		} else {
			ParaGetPileNo(port, pbuf);
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
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf,ret = 1,rebootflag = 0,i;
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
	ParaGetPileNo(0, pbuf);
	for (i = 0; i < 32; i++){
		if (pbuf[i] == 0x00){
			pbuf[i] = port + 'A';
			break;
		}
	}
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
		print("׼�������豸..................\r\n\n\n");
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
	memcpy(&CommInfo[port].UserId, &buffer[8], 2);//�û�ID
	CommInfo[port].chargestrategy = buffer[21]; //������
	CommInfo[port].chargepara = buffer[25] | (buffer[26] << 8) | (buffer[27] << 16) | (buffer[28] << 24); //�����Բ���
	memcpy(CommInfo[port].ChargeCardNumber, &buffer[38], 32);//��翨��/�û�ʶ���
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
	uint8_t buffer[COMMBUFFERLEN], *pbuf,i;
	
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
	ParaGetPileNo(0, pbuf);
	for (i = 0; i < 32; i++){
		if (pbuf[i] == 0x00){
			pbuf[i] = port + 'A';
			break;
		}
	}
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
	uint8_t buffer[COMMBUFFERLEN], *pbuf,datetime[8],i;
	uint32_t data32,data1,data2,data3,data4,data5,data6;
	uint8_t MainState;
	CHARGEINFO info;
	uint32_t alarmcode;
	
	MainState = GetMainCharge(port);
	info = GetCCBInfo(port);
	pbuf = buffer;
	*pbuf++ = STARTCODE1; //��ʼ��
	*pbuf++ = STARTCODE2;
	*pbuf++ = 0xB2; //������
	*pbuf++ = 0x00;
	*pbuf++ = MSGCODE; //��Ϣ��
	*pbuf++ = 0x41; //���к���
	*pbuf++ = 104; //�������
	*pbuf++ = 0;
	
	memcpy(pbuf, &CommInfo[port].UserId, 2);
	pbuf+= 2;
//	*pbuf++ = 0x00; //������
//	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	ParaGetPileNo(0, pbuf); //���׮����
	for (i = 0; i < 32; i++){
		if (pbuf[i] == 0x00){
			pbuf[i] = port + 'A';
			break;
		}
	}
	pbuf += 32;
	*pbuf++ = ParaGetChgGunNumber(); //���ǹ���� 1/2
	if (ParaGetChgGunNumber() > 1){
		*pbuf++ = port + 1; //���ں�
	} else {
		*pbuf++ = 0;
	}
	*pbuf++ = 0x01; //���ǹ���� 01ֱ�� 02����
	if (MainState == 0){
		*pbuf++ = 0; //����״̬ ����
	} else if (MainState == 1){
		*pbuf++ = 2; //����״̬ ��������
	} else if (MainState == 2){
		*pbuf++ = 3; //����״̬ ������
	} else {
		*pbuf++ = 0;
	}
	*pbuf++ = info.SOC; //��ǰSOC
	if (info.ScramState == 1)//��ͣ����
		alarmcode = 3;
	else if (info.DoorState == 1)//�Ž�����
		alarmcode = 4;
	else if(ElmGetCommState(port) == 1)//���ͨѶ�쳣
		alarmcode = 1002;
	else
		alarmcode = 0;
	memcpy(pbuf, &alarmcode, 4);//��ǰ��߸澯����
	pbuf += 4;
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
	GetVoltage(port, &data32, &data2, &data3);//ֱ������ѹ
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	GetCurrent(port, &data32, &data2, &data3);//ֱ��������
	data32 /= 100;
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = info.BmsDemandVol; //BMS�����ѹ
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = (4000 - info.BmsDemandCur) * 10; //BMS�������
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	*pbuf++ = info.Mode; //BMS���ģʽ
	data32 = 0;//����A�����ѹ ���----------------
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = 0;//����B�����ѹ ���----------------
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = 0;//����C�����ѹ ���----------------
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = 0;//����A������� ���----------------
	data32 /= 100;
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = 0;//����B������� ���----------------
	data32 /= 100;
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = 0;//����C������� ���----------------
	data32 /= 100;
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = info.SurplusMinute; //ʣ����ʱ��
	memcpy(pbuf, &data32, 2);
	pbuf += 2;
	data32 = GetSumTime(port);//���ʱ��
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	data32 = GetSumEnergy(port);//���γ���ۼƳ�����
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	data32 = GetStartMeterEnergy(port);//���ǰ������
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	data32 = GetStopMeterEnergy(port);//��ǰ������
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	if (GetStartType(port) == STARTCHARGETYPE_PLATFORM)
		*pbuf++ = 0x01; //���������ʽ ��̨����
	else
		*pbuf++ = 0x00; //���������ʽ ����ˢ������
	*pbuf++ = GetStartMode(port); //������
	data32 = GetStartPara(port);//�����Բ���
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	*pbuf++ = 0x00; //ԤԼ��־
	memset(pbuf, 0x00, 32); //��翨��
	GetCardOrDiscernNumber(port, pbuf);
	pbuf += 32;
	*pbuf++ = 0x00; //ԤԼ��ʱʱ��
	GetStartDateTime(port, datetime);
	*pbuf++ = 0x20;//ԤԼ/��ʼ��翪ʼʱ��
	*pbuf++ = Hex32ToBcd32(datetime[0]);
	*pbuf++ = Hex32ToBcd32(datetime[1]);
	*pbuf++ = Hex32ToBcd32(datetime[2]);
	*pbuf++ = Hex32ToBcd32(datetime[3]);
	*pbuf++ = Hex32ToBcd32(datetime[4]);
	*pbuf++ = Hex32ToBcd32(datetime[5]);
	*pbuf++ = 0xff;
	data32 = GetStartCardMoney(port); //���ǰ�����
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	memset(pbuf, 0x00, 4); //Ԥ��
	pbuf += 4;
	GetVoltage(port, &data1, &data2, &data3);
	GetCurrent(port, &data4, &data5, &data6);
	data32 = (data1 * data4 + data2 * data5 + data3 * data6) / 1000;
	memcpy(pbuf, &data32, 4);//��繦��
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
static void SendCMD202(uint8_t port, CHARGINGDATA *ChgDat)
{
	uint8_t buffer[COMMBUFFERLEN], *pbuf,i;
	uint32_t data32;
	
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
//	*pbuf++ = 0x00; //������
//	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	*pbuf++ = 0x00;
	
	ParaGetPileNo(0, pbuf); //���׮����
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
	memcpy(pbuf, ChgDat->CardOrDiscernNumber, 32);//GetCardOrDiscernNumber(port, pbuf);
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
	data32 = 0;
//	if (ChgDat->StopCause == CAUSE_OTHER)//����ԭ��
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_USER_NORMAL_STOP)//�Ʒѿ��ư巢��ֹͣ�������
//		data32 = 200;//�û���ֹ
//	else if (ChgDat->StopCause == CAUSE_WAIT_INSERTGUN_TIMEOUT)//�ȴ���ǹ��ʱ
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_LOCK_GUN_FAILED)//��ǹʧ��
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_K1K2_ADHESION_FAULT)//�̵���ճ������
//		data32 = 2007;//�Ӵ����쳣
//	else if (ChgDat->StopCause == CAUSE_K1K2_CLOSE_FAULT)//�̵����ܶ�����
//		data32 = 2007;//�Ӵ����쳣
//	else if (ChgDat->StopCause == CAUSE_K1K2_OUTSIDE_VOL_GREATER_THAN_10V)//��Ե���ǰK1K2����ѹ����10V
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_CHARGEMODULE_OUTPUT_FAULT)//���ģ���������
//		data32 = 2008;//ģ�����
//	else if (ChgDat->StopCause == CAUSE_INSULATION_DETECT_FAULT)//��Ե����쳣
//		data32 = 304;//��Ե����쳣
//	else if (ChgDat->StopCause == CAUSE_BLEED_UNIT_FAULT)//й�ŵ�·�쳣
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BMS_PARA_CANNOT_FIT)//�����������ʺ�
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_START_VOLTAGE_FAULT)//����ʱ��ѹ�쳣
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BATTERY_READY_TO_NOREADY)//���׼��������Ϊδ����
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_SUSPEND_TIMEOUT)//���׮�������ͣ��ʱ
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_CC1_FAULT)//CC1�쳣
//		data32 = 300;//CC1���ӶϿ�
//	else if (ChgDat->StopCause == CAUSE_BMS_COMMUNICATE_TIMEOUT)//BMSͨ�ų�ʱ
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_SCRAMSTOP_FAULT)//��ͣ��������
//		data32 = 302;//����ͣ��
//	else if (ChgDat->StopCause == CAUSE_DOOR_OPEN_FAULT)//�Ž�����
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_GUNPORT_OVERTEMP)//���ӿڹ���
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_OUPUTVOLTAGE_OVER_FAULT)//ֱ�������ѹ��ѹ����
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_DEMANDVOL_FAULT)//�����ѹ�쳣
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BSM_SINGLEBATTERY_OVERVOL_FAULT)//BSM�����е��嶯�����ص�ѹ����
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BSM_SINGLEBATTERY_LESSVOL_FAULT)//BSM�����е��嶯�����ص�ѹ����
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BSM_SOC_OVER_FAULT)//BSM������SOC����
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BSM_SOC_LESS_FAULT)//BSM������SOC����
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BSM_SINGLEBATTERY_OVERCUR_FAULT)//BSM�����е��嶯�����ص�������
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BSM_TEMP_OVER_FAULT)//BSM�����ж��������¶ȹ���
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BSM_INSULATION_FAULT)//BSM�����ж������ؾ�Ե״̬�쳣
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BSM_OUTPUTCONNECTER_FAULT)//BSM�����ж����������������������״̬�쳣
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_NORMAL_REACHSOC)//BST�����дﵽ�������SOCĿ��ֵ
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_NORMAL_REACHTOTALVOL)//BST�����дﵽ�ܵ�ѹ���趨ֵ
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_NORMAL_REACHSINGLEVOL)//BST�����дﵽ�����ѹ���趨ֵ
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_FAULT_INSULATION)//BST�����о�Ե����
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_FAULT_OUTPUTCONNECTER_OVERTEMP)//BST������������������¹���
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_FAULT_ELEMENT_OVERTEMP)//BST������BMSԪ�����������������
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_FAULT_OUTPUTCONNECTER)//BST�����г������������
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_FAULT_BATTERYOVERTEMP)//BST�����е�����¶ȹ��߹���
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_FAULT_RELAY)//BST�����и�ѹ�̵�������
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_FAULT_CHECKPOINT2)//BST�����м���2��ѹ������
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_FAULT_OTHER)//BST��������������
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_ERROR_CUR)//BST�����е�������
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_BST_ERROR_VOL)//BST�����е�ѹ����
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_CP_FAULT)//CP�쳣
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_CC_FAULT)//CC�쳣
//		data32 = 2000;//ϵͳ��������
//	else if (ChgDat->StopCause == CAUSE_CHARGE_RESOURCE_APPLY_FAIL)//�����Դ����ʧ��
//		data32 = 2000;//ϵͳ��������
//	else
//		data32 = 2000;//ϵͳ��������
	memcpy(pbuf, &data32, 4);//������ԭ��
	pbuf += 4;
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
	memcpy(pbuf, &CommInfo[port].ChargeRecordIndexBak, 4);//��ǰ����¼����
	pbuf += 4;
	memcpy(pbuf, &CommInfo[port].ChargeRecordNumberBak, 4);//�ܳ���¼��Ŀ
	pbuf += 4;
	*pbuf++ = 0; //Ԥ��
	*pbuf++ = ChgDat->Mode; //������
	memcpy(pbuf, &ChgDat->Para, 4); //�����Բ���
	pbuf += 4;
	memcpy(pbuf, ChgDat->VIN, 17); //����VIN
	pbuf += 17;
	memset(pbuf, 0, 8); //���ƺ�
	pbuf += 8;
	memcpy(pbuf, ChgDat->PeriodEnergy, 48 * 2); //ʱ�ε���
	pbuf += 48 * 2;
	if (ChgDat->StartType == STARTCHARGETYPE_PLATFORM)
		*pbuf++ = 0x01; //���������ʽ ��̨����
	else
		*pbuf++ = 0x00; //���������ʽ ����ˢ������	
	memcpy(pbuf, ChgDat->TradeSerialNumber, 32); //������ˮ��
	pbuf += 32;
	
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
	uint8_t buffer[COMMBUFFERLEN], *pbuf,i,strlen;
	uint32_t data32;
	
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
	ParaGetPileNo(0, pbuf); //���׮����
	for (i = 0; i < 32; i++){
		if (pbuf[i] == 0x00){
			pbuf[i] = port + 'A';
			break;
		}
	}
	pbuf += 32;
	memset(pbuf, 0x00, 32); //��翨��
	strlen = sprintf((char *)pbuf, "%u", ReportCMD204[port].ReportOnlineCardID);
	if (strlen < 10){
		memset(pbuf, '0', 10 - strlen);
		sprintf((char *)pbuf + (10 - strlen), "%d", ReportCMD204[port].ReportOnlineCardID);
	}
	pbuf += 32;
	data32 = 0; //��翨���
	memcpy(pbuf, &data32, 4);
	pbuf += 4;
	*pbuf++ = 0x00; //��翨��������־
	memset(pbuf, 0x00, 32); //�û���翨����
	pbuf += 32;
	memset(pbuf, 0x00, 48); //���������������
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
	uint8_t buffer[COMMBUFFERLEN], *pbuf, i;
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
	ParaGetPileNo(0, pbuf); //���׮����
	for (i = 0; i < 32; i++){
		if (pbuf[i] == 0x00){
			pbuf[i] = port + 'A';
			break;
		}
	}
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
	ReportCMD204[port].ReportRet = buffer[12];
	ReportCMD204[port].OnlineCardIDBalance = buffer[16] | (buffer[17] << 8) | (buffer[18] << 16) | (buffer[19] << 24);
	ReportCMD204[port].ReportFlag = 2;
}

/************************************************************************************************************
** �� �� �� : RecvCMD1011
** �������� : ���շ������·�����ָ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void RecvCMD1011(uint8_t port, uint8_t *buffer)
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
	
	print("׼�������豸..................\r\n\n\n");
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
	uint8_t ackbuffer[COMMBUFFERLEN], *pbuf,ret = 1,*p;
	uint32_t i,data32;
	 
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
** ��    �� :	��
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
			print("CMD201 ������Ӧ����׮�ϱ������Ϣ����P%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			CommInfo[port].AckCMD201Flag = 1;
			break;
		case 203: //������Ӧ���ʻ���ѯ��Ϣ
			print("CMD203 ������Ӧ���ʻ���ѯ��ϢP%d:", port);
			printx(buffer, buffer[2] + (buffer[3] << 8));
			RecvCMD203(port, buffer);
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
** ��    �� :	��
*************************************************************************************************************
*/
static void AccountQueryReport(uint8_t port)
{
	static uint8_t test1;
	
	if(test1 == 1){
		test1 = 0;
		SendCMD204(port);
	}
}

///************************************************************************************************************
//** �� �� �� : ReportChargeRecord
//** �������� : �ϱ����׼�¼
//** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
//** ��    �� : ��
//** ��    �� :	��
//*************************************************************************************************************
//*/
//static void ReportChargeRecord(uint8_t port)
//{
//	CHARGINGDATA ChgDat;
//	uint16_t index;
//	
//	if (CommInfo[port].ChargeRecordNumberBak != GetChargeRecordNumber(port)){//�½��׼�¼
//		CommInfo[port].ChargeRecordNumberBak = GetChargeRecordNumber(port);
//		index = GetChargeRecordIndex(port);
//		CommInfo[port].ChargeRecordIndexBak = index;
//		if (FindChargeRecord(index,  (uint8_t *)&ChgDat, sizeof(CHARGINGDATA)) == 1){//��ѯ�ɹ�
//			SendCMD202(port, &ChgDat);
//		}
//	} else {
//		
//	}
//}

/************************************************************************************************************
** �� �� �� : ReportChargeRecord
** �������� : �ϱ����׼�¼
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void ReportChargeRecord(uint8_t port)
{
	static uint32_t Timer[2];
	CHARGINGDATA ChgDat;
	uint16_t index,i;
	
	if (CommInfo[port].AckCMD201Flag == 1){//��̨Ӧ�����¼
		CommInfo[port].AckCMD201Flag = 0;
		for (i = 0; i < 50 - 1; i++)
			CommInfo[port].ReportCtl[i] = CommInfo[port].ReportCtl[i + 1];
		CommInfo[port].NeedReportNum--;
		Timer[port] = 0;
	}
	if ((TimerRead() - Timer[port] > T1S * 10) || (CommInfo[port].ChargeRecordNumberBak != GetChargeRecordNumber(port))){
		if (CommInfo[port].ChargeRecordNumberBak != GetChargeRecordNumber(port)){//�½��׼�¼
			CommInfo[port].ChargeRecordNumberBak = GetChargeRecordNumber(port);
			index = GetChargeRecordIndex(port);
			CommInfo[port].ChargeRecordIndexBak = index;
			if (FindChargeRecord(index,  (uint8_t *)&ChgDat, sizeof(CHARGINGDATA)) == 1){//��ѯ�ɹ�
				if (ChgDat.ChgPort == port){
					for (i = 0; i < 50 - 1; i++)
						CommInfo[port].ReportCtl[50 - 1 - i] = CommInfo[port].ReportCtl[50 - 2 - i];
					CommInfo[port].ReportCtl[0].index = index;
					CommInfo[port].NeedReportNum++;
				}
			}
		}
		if (CommInfo[port].NeedReportNum > 0){
			FindChargeRecord(CommInfo[port].ReportCtl[0].index,  (uint8_t *)&ChgDat, sizeof(CHARGINGDATA));
			if (ChgDat.ChgPort == port){
				SendCMD202(port, &ChgDat);
			}
		}
//		if (ChgDat.ChgPort == port)
//			Timer[port] = TimerRead();
	}
}

/************************************************************************************************************
** �� �� �� : ProtocolHandle
** �������� : ͨ��Э�鴦����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void ProtocolHandle(uint8_t port)
{
	static uint8_t state[2];
	static uint8_t logincount[2],heartbeatcount[2];
	static uint32_t timer[2],reportstatetimer[2],report204timer[2];
	static uint32_t reportstartchargeresulttimer[2];
	static uint8_t chgstate[2],mainstatebak[2];
	uint8_t sip[4];
	uint16_t sport,peroid;
	
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
			if ((peroid = ParaGetNetHeartTime(port)) < 3)
				peroid = 3;
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
			if ((peroid = ParaGetStateInfoReportPeriod(port)) < 3)
				peroid = 3;
			if(TimerRead() - reportstatetimer[port] > T1S * peroid){
				reportstatetimer[port] = TimerRead();
				SendCMD104(port);
			}
			//��Ӧ�仯�ϱ�
			if (GetMainCharge(port) != mainstatebak[port]){
				mainstatebak[port] = GetMainCharge(port);
				SendCMD104(port);
			}
			
			//��ʱ�ϱ�δӦ��Ľ��׼�¼
			ReportChargeRecord(port);
			
			//�ϱ����߿���ѯ��Ϣ
			if (ReportCMD204[port].ReportFlag == 1){
				if (TimerRead() - report204timer[port] > T1S * 3){
					report204timer[port] = TimerRead();
					SendCMD204(port);
				}
			}
		
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
			if(CommInfo[port].startflag == 1){
				CommInfo[port].startflag = 0;
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
				print("test1----\r\n");
				ReportChargeRecord(port);
				chgstate[port] = 0;
				CommInfo[port].startflag = 0;
			}
			if(CommInfo[port].stopflag == 1){//��̨�·�ֹͣ���
				CommInfo[port].stopflag = 0;
				SetMainCharge(port, SET_STOP_CHARGE, 0 , 0, 0, 0);
//				ReportChargeRecord(port);
//				chgstate[port] = 0;
				CommInfo[port].startflag = 0;
			}
			break;
	}
}

/************************************************************************************************************
** �� �� �� : GetOnlineState
** �������� : ��ȡ����״̬
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
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
** ��    �� :	0��Ч�ʻ� 1�Ƿ��ʻ� 2���� 3�Ѻڿ��ָ��������� 4����������ڿ� 5�Ƿ����� 6��ʧ 7���� 8������� 9�ÿ��Ѿ��ڱ��׮��� 
*************************************************************************************************************
*/
int8_t VerifyOnlineCardInfo(uint8_t port, uint32_t CardID, uint32_t *Balance)
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
			ReportCMD204[port].ReportFlag = 0;
			*Balance = ReportCMD204[port].OnlineCardIDBalance;
			return ReportCMD204[port].ReportRet;
		}
	}
	ReportCMD204[port].ReportOnlineCardID = 0;
	ReportCMD204[port].OnlineCardIDBalance = 0;
	ReportCMD204[port].ReportRet = 0;
	ReportCMD204[port].ReportFlag = 0;
	return -1;
}

/************************************************************************************************************
** �� �� �� : ProtocolInit
** �������� : ��ʼ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
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
		num = GetChargeRecordNumber(port) % 50;
		CommInfo[port].ChargeRecordNumberBak = num;
		for (i = 0; i < num; i++){
			if (FindChargeRecord(index,  (uint8_t *)&ChgDat, sizeof(CHARGINGDATA)) == 1)//��ѯ�ɹ�
				CommInfo[port].ReportCtl[CommInfo[port].NeedReportNum++].index = index;
			if (index == 0)
				index = 1000;
			else
				index --;
			OSTimeDlyHMSM (0, 0, 0, 1);
		}
	}
}

/************************************************************************************************************
** �� �� �� : TaskProtocolHandle
** �������� : ͨ��Э�鴦������
** ��    �� : pdata δʹ��
** ��    �� : ��
** ��    �� :	��
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
