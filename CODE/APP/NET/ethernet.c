#include "includes.h"
#include <stdint.h>
#include <string.h>
#include "w5500.h"
#include "socket.h"
#include "NetTask.h"
#include "SPI.h"
#include "GPIO.h"
#include "Board.h"
#include "UART.h"
#include "includes.h"
#include "Para.h"
#include "timer.h"
static NETINFO EthInfo={0};
static uint16_t UdpPort = 0;      //���յ�UDP�˿ں�

#define UDPSOCKET  7

/************************************************************************************************************
** �� �� �� : W5500_CS_Select
** �������� : w5500ѡ�к���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void W5500_CS_Select(void)
{
	GpioSetLev(GPIO_SNETCS, LOW);
}

/************************************************************************************************************
** �� �� �� : W5500_CS_Deselect
** �������� : w5500��ѡ�к���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void W5500_CS_Deselect(void)
{
	GpioSetLev(GPIO_SNETCS, HIGH);
}

/************************************************************************************************************
** �� �� �� : SPI_SendByte
** �������� : SPI���ͺ���
** ��    �� : data ����
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void SPI_SendByte(uint8_t data)
{
	SpiWriteByte(ETHSPINO, data);
}

/************************************************************************************************************
** �� �� �� : SPI_RecvByte
** �������� : SPI���պ���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��������
*************************************************************************************************************
*/
static uint8_t SPI_RecvByte(void)
{
	uint8_t data;
	data = SpiReadByte(ETHSPINO);
	return data;
}


/************************************************************************************************************
** �� �� �� : EthInit
** �������� : ��̫����ʼ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void EthInit(void)
{
	wiz_NetTimeout nettime,readbacknettime;
	wiz_NetInfo netinfo,readbacknetinfo;
	uint8_t arg;
	uint8_t txrxsize[_WIZCHIP_SOCK_NUM_*2] = {2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2};
	uint8_t data[6];
	
	SpiInit(ETHSPINO);   	// SPI ��ʼ��
	GpioSetDir(GPIO_SNETRST,OUTPUT);  // ��λ��
	GpioSetLev(GPIO_SNETRST,LOW);
	OSTimeDlyHMSM (0, 0, 2, 0);
	GpioSetLev(GPIO_SNETRST,HIGH);
	OSTimeDlyHMSM (0, 0, 2, 0);
	GpioSetDir(GPIO_SNETCS,OUTPUT);  // ʧ��
	GpioSetLev(GPIO_SNETCS,HIGH);
	
	//ע��ӿ�
	reg_wizchip_cs_cbfunc(W5500_CS_Select, W5500_CS_Deselect);
	reg_wizchip_spi_cbfunc(SPI_RecvByte, SPI_SendByte);
	reg_wizchip_cris_cbfunc(OS_ENTER_CRITICAL, OS_EXIT_CRITICAL);	
		
	//�����λW5500
	ctlwizchip(CW_RESET_WIZCHIP,&arg);
	//�����������
	ParaGetPhyMac(data);
	memcpy(netinfo.mac, data, 6);
	ParaGetLocalIp(data);
	memcpy(netinfo.ip, data, 4);
	ParaGetSubMask(data);
	memcpy(netinfo.sn, data, 4);
	ParaGetGateWay(data);
	memcpy(netinfo.gw, data, 4);
	memset(netinfo.dns,0,4);
	netinfo.dhcp=NETINFO_STATIC;
	ctlnetwork(CN_SET_NETINFO,&netinfo);
	ctlnetwork(CN_GET_NETINFO,&readbacknetinfo);
	//���ó�ʱ����
	nettime.retry_cnt=3;
	nettime.time_100us=2000;
	ctlnetwork(CN_SET_TIMEOUT,&nettime);
	ctlnetwork(CN_GET_TIMEOUT,&readbacknettime);
	//���÷��ͽ��ջ�������С
	ctlwizchip(CW_INIT_WIZCHIP,txrxsize);
}

/************************************************************************************************************
** �� �� �� : EthConn
** �������� : ��̫�����Ӻ���
** ��    �� : sn socketֵ0-1 serverip������IP serverport�������˿�
** ��    �� : ��
** ��    �� :	1�ɹ� ����ʧ��
*************************************************************************************************************
*/
uint8_t EthConn(uint8_t sn, uint8_t serverip[4], uint16_t serverport)
{
	if(sn >= SNNUM){
		return 0;
	}
	if(EthInfo.LinkFlag != 1){
		return 0;
	}
	print("Connect to %d.%d.%d.%d,%d P%d:\r\n", serverip[0], serverip[1], serverip[2], serverip[3], serverport, sn);
	memcpy(EthInfo.serverip[sn], serverip, 4);
	EthInfo.serverport[sn] = serverport;
	EthInfo.socketportusedflag[sn] = 1;
	EthInfo.ConnCmd[sn] = 1;
	while(EthInfo.ConnCmd[sn] == 1){
		OSTimeDlyHMSM (0, 0, 0, 100);
	}
	if(EthInfo.ConnState[sn] == 1){
		print("Connect success P%d:\r\n", sn);
		return 1;
	} else {
		print("Connect fail P%d:\r\n", sn);
		return 0;
	}
}

/************************************************************************************************************
** �� �� �� : EthSend
** �������� : ��̫�����ͺ���
** ��    �� : sn socketֵ0-1 buffer���ͻ����� len���������ֽڳ���
** ��    �� : ��
** ��    �� :	1�ɹ� ����ʧ��
*************************************************************************************************************
*/
uint8_t EthSend(uint8_t sn, uint8_t *buffer, uint16_t len)
{
	if(sn >= SNNUM){
		return 0;
	}
	if(EthInfo.LinkFlag != 1){
		return 0;
	}
	if(EthInfo.socketportusedflag[sn] == 0){
		return 0;
	}
	if(EthInfo.ConnState[sn] != 1){
		return 0;
	}
	if(len > NETDATAMAXLEN){
		len = NETDATAMAXLEN;
	}
	memcpy(EthInfo.SendBuf[sn], buffer, len);
	EthInfo.SendLen[sn] = len;
	EthInfo.SendCmd[sn] = 1;
	while(EthInfo.SendCmd[sn] == 1){
		OSTimeDlyHMSM (0, 0, 1, 0);
	}
	if(EthInfo.SendState[sn] == 1){
		return 1;
	}
	return 0;
}

/************************************************************************************************************
** �� �� �� : EthRecv
** �������� : ��̫�����պ���
** ��    �� : sn socketֵ0-1 buffer���ջ����� len���������ֽڳ���
** ��    �� : ��
** ��    �� :	ʵ�ʽ��յ������ݳ���
*************************************************************************************************************
*/
uint16_t EthRecv(uint8_t sn, uint8_t *buffer, uint16_t len)
{
	uint16_t rlen = 0;
	
	if(sn >= SNNUM){
		return 0;
	}
	if(EthInfo.LinkFlag != 1){
		return 0;
	}
	if(EthInfo.socketportusedflag[sn] == 0){
		return 0;
	}
	if(EthInfo.ConnState[sn] != 1){
		return 0;
	}
	while (EthInfo.RecvRr[sn] != EthInfo.RecvWr[sn]){
		*buffer++ = EthInfo.RecvBuf[sn][EthInfo.RecvRr[sn]];
		EthInfo.RecvRr[sn] = (EthInfo.RecvRr[sn] + 1) % NETDATAMAXLEN;
		if (++rlen > len){
			return len;
		}
	}
	return rlen;
}



/************************************************************************************************************
** �� �� �� : EthHandle
** �������� : ��̫��������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void EthHandle(void)
{
	static uint32_t timer = 0;
	int32_t ret;
	uint8_t arg,sn;
	uint16_t port = 9000,i,sentsize,size;
	uint8_t buffer[NETDATAMAXLEN];
	
	//��ѯlink״̬
	ret = ctlwizchip(CW_GET_PHYLINK, &arg);
	if((ret != 0) || (arg != PHY_LINK_ON)){
		if(EthInfo.LinkFlag == 1){
			EthInfo.LinkFlag = 0;
			for(sn = 0; sn < 8; sn++){
				close(sn);
			}
		}
		if (TimerRead() - timer >= T1M){
			timer = TimerRead();
			EthInit();
			print("net fail\r\n");
		}
		return;
	} else {
		EthInfo.LinkFlag = 1;
		timer = TimerRead();
	}
	
	//���ݴ���
	for(sn = 0; sn < SNNUM; sn++){
		if(EthInfo.socketportusedflag[sn] == 1){
			//��������Ҫ��
			if(EthInfo.ConnCmd[sn] == 1){
				close(sn);
				EthInfo.ConnState[sn] = 0;
				if( (ret = socket(sn, Sn_MR_TCP, port + sn, 0x00)) == sn){
					if( (ret = connect(sn, EthInfo.serverip[sn], EthInfo.serverport[sn])) == SOCK_OK){
						EthInfo.ConnState[sn] = 1;
					}
				}
				memset(EthInfo.RecvBuf[sn], 0, NETDATAMAXLEN);
				EthInfo.RecvWr[sn] = 0;
				EthInfo.RecvRr[sn] = 0;
				EthInfo.ConnCmd[sn] = 0;
			}
			//����������Ҫ��
			if(EthInfo.SendCmd[sn] == 1){
				EthInfo.SendState[sn] = 0;
				ret = getSn_SR(sn);
				if (ret == SOCK_ESTABLISHED){
					sentsize = 0;
					while(sentsize != EthInfo.SendLen[sn]){
						ret = send(sn, EthInfo.SendBuf[sn] + sentsize, EthInfo.SendLen[sn] - sentsize);
						if(ret < 0) {
							break;
						}
						sentsize += ret;
					}
					if(sentsize == EthInfo.SendLen[sn]){
						EthInfo.SendState[sn] = 1;
					}
				}
				EthInfo.SendCmd[sn] = 0;
			}
			//�����������
			ret = getSn_SR(sn);
			if (ret == SOCK_ESTABLISHED){
				if(getSn_IR(sn) & Sn_IR_CON){
					setSn_IR(sn, Sn_IR_CON);
				}
				if((size = getSn_RX_RSR(sn)) > 0){
					if(size > NETDATAMAXLEN){
						size = NETDATAMAXLEN;
					}
					ret = recv(sn, buffer, size);
					if(ret > 0){
						for(i = 0; i < ret; i++){
							EthInfo.RecvBuf[sn][EthInfo.RecvWr[sn]] = buffer[i];
							EthInfo.RecvWr[sn] = (EthInfo.RecvWr[sn] + 1) % NETDATAMAXLEN;
						}
					}
				}
			} else if (ret == SOCK_CLOSED){
				close(sn);
				EthInfo.ConnState[sn] = 0;
			}
		}
	}
}




/************************************************************************************************************
** �� �� �� : UDPSend
** �������� : UDP���ͺ���
** ��    �� : buf ���͵�buff  , len   ���͵ĳ���
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void UDPSend(uint8_t* buf, uint16_t len)
{
	uint8_t ip[4]={255,255,255,255};
	sendto(UDPSOCKET, buf,len, ip, UdpPort);                       // ���յ����ݺ��ٻظ�Զ����λ����������ݻػ�
}





/************************************************************************************************************
** �� �� �� : AnalyseMsgFromUdpPlatform
** �������� : ����UDPͨ�����ú���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void AnalyseMsgFromUdpPlatform(void)
{
	uint16_t len, i;
	static uint8_t buffer[512];
	int32_t ret;
	uint8_t arg;
	static uint16_t readlen;
	uint8_t UdpIp[4]={255,255,255,255};
	ret = ctlwizchip(CW_GET_PHYLINK, &arg);
	if((ret != 0) || (arg != PHY_LINK_ON))
		return;

	switch(getSn_SR(UDPSOCKET))                                                         // ��ȡsocket0��״̬
	{
		case SOCK_UDP:          // Socket���ڳ�ʼ�����(��)״̬
			if(getSn_IR(UDPSOCKET) & Sn_IR_RECV){
				setSn_IR(UDPSOCKET, Sn_IR_RECV);   // Sn_IR��RECVλ��1
			}
			if((len = getSn_RX_RSR(UDPSOCKET))>0){
				if(readlen + len > sizeof(buffer)){
					readlen = 0; // ����
				}				
				if(len > sizeof(buffer)){
					len = sizeof(buffer); // ����
				}
				recvfrom(UDPSOCKET, buffer + readlen, len, UdpIp, &UdpPort); // W5500��������Զ����λ�������ݣ���ͨ��SPI���͸�MCU		
				readlen +=len;
				if (readlen){
					for (i = 0; i < readlen; i++){
						if (ConfigProtocolFrameCheck(buffer + i, PARACHANNEL_UDP) == 1){
							if ((buffer[i + 1 + 33] & 0x1f) ==0x01){
								DebugPlatformReadDataAnalyse(&buffer[i], PARACHANNEL_UDP);
							} else if ((buffer[i + 1 + 33] & 0x1f) ==0x11){
								DebugPlatformWriteDataAnalyse(&buffer[i], PARACHANNEL_UDP);
							}
							memset(buffer, 0, sizeof(buffer));
							readlen = 0;
							break;
						}
					}		
				}
			}          
			break;
		case SOCK_CLOSED:                                                       // Socket���ڹر�״̬
			socket(UDPSOCKET,Sn_MR_UDP,2408, 0);                                   // ��Socket0��������ΪUDPģʽ����һ�����ض˿�
			break;
		default:
				break;
	}
}








/************************************************************************************************************
** �� �� �� : EthTask
** �������� : ��̫����ͨ����
** ��    �� : pdata δʹ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/

/*
void EthTask(void *pdata)
{
	print("net task start...\r\n");
  EthInit();
	
	while(1){
		EthHandle();
	
		OSTimeDlyHMSM (0, 0, 0, 15);
	}
}


*/

