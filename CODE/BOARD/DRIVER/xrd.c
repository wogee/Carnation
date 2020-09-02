#include <stdint.h>
#include "board.h"
#include "xrd.h"
#include "UART.h"
#include <string.h>
static LCDINFO LcdInfo;

void XRD_Load_Page(uint16_t page)
{
	uint8_t len=0;
	uint8_t Packbuf[12]={0};
	Packbuf[0]=PACKHEAD1;
	Packbuf[1]=PACKHEAD2;
	len=2+2;
	Packbuf[2]=len;
	Packbuf[3]=WRITE_CMD;
	Packbuf[4]=PIC_ADDR;
	Packbuf[5]=page>>8;
	Packbuf[6]=(uint8_t)page;
	len=len+3;
	UartWrite(DISPUARTNO,Packbuf,len);
}





/************************************************************************************************************
** �� �� �� : DispDeviceFault
** �������� : ��ʾ��׮���Ͻ���
** ��    �� : tempbuf    ����ID BUf
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void XRD_Write_Addr(uint16_t addr, char* str, uint8_t strl)
{
	uint8_t len,i;
	uint8_t lenstr;
	uint8_t Packbuf[256 + 6] = {0};
	lenstr = strlen(str);
	if(lenstr > strl)
	{
		lenstr = strl;
	}
	Packbuf[0] = PACKHEAD1;
	Packbuf[1] = PACKHEAD2;
	len = 3 + strl;
	Packbuf[2] = len;
	Packbuf[3] = WRITE_VAR_CMD;
	Packbuf[4] = addr >> 8;
	Packbuf[5] = (uint8_t)addr;
	for(i=0; i <= lenstr; i++)
	{
		Packbuf[6+i] = *(str+i);
	}
	for(i=0; i < strl - lenstr; i++)
	{
		Packbuf[6+strlen(str)] = '\0';     //��ո�
	}
	len=len+3;
	UartWrite( DISPUARTNO, Packbuf, len);
}





void XRD_Draw_Ioc(uint16_t addr, uint16_t num)
{
	uint8_t len;	
	char Packbuf[256 + 6]={0};
	char* str1=Packbuf;
	*(str1+0)=PACKHEAD1;
	*(str1+1)=PACKHEAD2;
	len=3+2;
	*(str1+2)=len;
	*(str1+3)=WRITE_VAR_CMD;
	*(str1+4)=addr>>8;
	*(str1+5)=(uint8_t)addr;
	*(str1+6)=num>>8;
	*(str1+7)=(uint8_t)num;
	len=len+3;
	UartWrite(DISPUARTNO,(uint8_t *)Packbuf,len);
}



/************************************************************************************************************
** �� �� �� : LcdReadbuf
** �������� : ��ȡLcd�����յ�������
** ��    �� : data:��Ž��յ����ݵ�ָ�룬 ��Ҫ��ȡ�����ݳ���
** ��    �� : ��
** ��    �� : 0 δ�������� ���򷵻ض��������ݳ���
*************************************************************************************************************
*/
static uint16_t LcdReadbuf(uint8_t *data, uint16_t datalen)
{
	uint16_t r_len = 0;
	
	if( (data == NULL) || (datalen == 0)){
		return 0;
	}
	while(LcdInfo.RecvRd != LcdInfo.RecvWr){
		*data = LcdInfo.ReBuf[LcdInfo.RecvRd];
		LcdInfo.RecvRd = (LcdInfo.RecvRd + 1) % LCDDATAMAXLEN;
		data++;
		if(++r_len >= datalen){
			return r_len;
		}
	}
	return r_len;
}
/************************************************************************************************************
** �� �� �� : Recoverbuf
** �������� : �ָ�GPRS���ڴ���������
** ��    �� : ��Ҫ�ָ������ݳ���
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
static void RecoverLcdbuf(uint16_t len)
{
	if(len == 0)
			return ;
	if( LcdInfo.RecvRd >= len) {
		LcdInfo.RecvRd = LcdInfo.RecvRd - len;   
	}else{
		LcdInfo.RecvRd = LCDDATAMAXLEN - (len - LcdInfo.RecvRd );
	}
}



/************************************************************************************************************
** �� �� �� : GetTouchInfo
** �������� : �õ���������ֵ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
uint8_t GetTouchInfo(uint8_t* strbuf, uint8_t strlen, uint16_t* keyfunc)
{
	uint16_t len;
	uint16_t i;
	uint8_t readbuf[512];
	if(keyfunc ==NULL)
		return 0;
	len = UartRead(DISPUARTNO, (uint8_t *)readbuf, 512);						
	if(len != 0){
			for(i = 0; i < len; i++){
				LcdInfo.ReBuf[LcdInfo.RecvWr] = readbuf[i];
				LcdInfo.RecvWr = (LcdInfo.RecvWr + 1) % LCDDATAMAXLEN;
				if(LcdInfo.RecvWr == LcdInfo.RecvRd){
					LcdInfo.RecvRd = (LcdInfo.RecvRd + 1) % LCDDATAMAXLEN;
				}	
			}		
	}	
	len = LcdReadbuf((uint8_t *)readbuf, 1024);		
	for(i=0; i < len; i++){
		if((readbuf[i] == 0x5a) && (readbuf[i + 1] == 0xa5)){
			if(readbuf[i + 2] + 3 <= len){// ���յ���ͷ�����ݳ��Ȳ�����ֱ�ӷ���
				if(readbuf[i + 3] == 0x83){    //�����ݱ���������
					if((readbuf[i + 4] == 0x08)&& (readbuf[i + 5] == 0x00)){
						*keyfunc = (uint16_t)readbuf[i + 7] << 8|readbuf[i + 8];				
						RecoverLcdbuf(len -(readbuf[i + 2] + 3));	
						return 1;						
					}else if((readbuf[i + 4] == 0x07)&& (readbuf[i + 5] == 0x90)){
						*keyfunc = (uint16_t)readbuf[i + 4] << 8 | readbuf[i + 5];
						memcpy(strbuf, &readbuf[i + 7], strlen);
						RecoverLcdbuf(len -(readbuf[i + 2] + 3));	
						return 1;
					}else{
						RecoverLcdbuf(len -(readbuf[i + 2] + 3));	
						return 0;		
					}		
				}else{
					RecoverLcdbuf(len -(readbuf[i + 2] + 3));	
					return 0;			
				}
			}
			RecoverLcdbuf(len - i);	
			return 0;			
		}
	}
	return 0;
}



/************************************************************************************************************
** �� �� �� : XRDClearInput
** �������� : �����������¼��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void XRDClearInput(void)
{
	uint8_t Packbuf[32] = {0};
	Packbuf[0] = 0x5A;
	Packbuf[1] = 0xA5;
	Packbuf[2] = 0x03;
	Packbuf[3] = 0x80;
	Packbuf[4] = 0x4F;
	Packbuf[5] = 0x01;
	UartWrite(DISPUARTNO,(uint8_t *)Packbuf,6);
}






















