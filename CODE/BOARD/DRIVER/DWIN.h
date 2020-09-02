#ifndef _DWIN_H
#define _DWIN_H

// ����
#define CMD_WRREGIT  		0x80    //д�Ĵ���
#define CMD_RDREGIT  		0x81	//���Ĵ���	
#define CMD_WRVAR    		0x82	//д����	
#define CMD_RDVAR    		0x83	//������	

// �Ĵ�����ַ
#define REG_VER    			0X00   // ����汾��  �ɶ�  1 BYTE
#define REG_BKLED  			0X01   // ��������    ��д  1 BYTE
#define REG_PAGE   			0X03   // ҳ��        �ɶ���д	2 BYTE
#define REG_RTC    			0X20   // RTCʱ��     �ɶ���д  16 BYTE


#define DWINHEAD1          	0X5A    // ֡ͷ1
#define DWINHEAD2          	0XA5    // ֡ͷ2

typedef struct{
	uint8_t  cmd;
	uint16_t addr;
	uint16_t buflen;
	uint8_t  buf[256];
}DwinRcvMsg;

void DwinInit(void);
void DwinShowPage(uint16_t no);
void DwinWriteData(uint8_t cmd,uint16_t addr,uint16_t datalen,uint8_t* data);
uint8_t DwinReadData(DwinRcvMsg *msg);

#endif
