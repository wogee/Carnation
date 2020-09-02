#include"Board.h"
#include "GPIO.h"
#include <string.h>
#include "MX25L.h"
#include "SPI.h"

/****************************************************************************\
 Function:  Mx25Init
 Descript:  MX25��ʼ��  (NORFLASH)
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-21
 *****************************************************************************/
void Mx25Init(void)
{
	SpiInit(MX25SPINO);
	Mx25Disable();
	
}


/****************************************************************************\
 Function:  Mx25write
 Descript:  ��mx25д����
 Input:	    ���ݣ£գƣ���ַ�����ݳ���
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-21
 *****************************************************************************/
void Mx25write(uint32_t addr,uint8_t *data,uint16_t len)
{
	uint8_t  temp=0;
	uint16_t i=0;

	if ((data==NULL) || (len==0))
	{
		return ;
	}
	
	Mx25Enable();														// дʹ��
	SpiWriteByte(MX25SPINO,SPI_FLASH_INS_WREN); 
	Mx25Disable();

	while(!(Mx25GetSt()&0x02)); // �Ƿ��д
	
	Mx25Enable();
	SpiWriteByte(MX25SPINO,SPI_FLASH_INS_PP);
	temp=addr>>16;
	SpiWriteByte(MX25SPINO,temp);
	temp=addr>>8;
	SpiWriteByte(MX25SPINO,temp);
	temp=addr&0xff;
	SpiWriteByte(MX25SPINO,temp);
	for(i=0;i<len;i++)
	{
		SpiWriteByte(MX25SPINO,*data++);
	}
	Mx25Disable();

	while(Mx25GetSt()&0x01);	// �Ƿ�д��	

	Mx25Enable();														// дʧ��
	SpiWriteByte(MX25SPINO,SPI_FLASH_INS_WRDI); 
	Mx25Disable();
}



/****************************************************************************\
 Function:  Mx25Read
 Descript:  ��mx25������
 Input:	    ���ݣ£գƣ���ַ�����ݳ���
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-21
 *****************************************************************************/
void Mx25Read(uint32_t addr,uint8_t *data,uint16_t len)
{
	uint16_t i=0;
	uint8_t temp=0;

	if ((data==NULL) || (len==0))
	{
		return;
	}
	
	Mx25Enable();
	
    SpiWriteByte(MX25SPINO,SPI_FLASH_INS_READ);
	temp=addr>>16;
	SpiWriteByte(MX25SPINO,temp);
	temp=addr>>8;
	SpiWriteByte(MX25SPINO,temp);
	temp=addr&0xff;
	SpiWriteByte(MX25SPINO,temp);
	for(i=0;i<len;i++)
	{
		*data++= SpiReadByte(MX25SPINO);;
	}
	
	Mx25Disable();
}


/****************************************************************************\
 Function:  Mx25Erase
 Descript:  ����mx25һ�����������ݡ�(4k)
 Input:	    ��ַ
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-21
 *****************************************************************************/
void Mx25Erase(uint32_t addr)
{
	uint8_t temp=0;

	Mx25Enable();
	SpiWriteByte(MX25SPINO,SPI_FLASH_INS_WREN);			//дʹ��
	Mx25Disable();

	while(!(Mx25GetSt()&0x02));	// �Ƿ��д
	
	Mx25Enable();
	SpiWriteByte(MX25SPINO,0x20);				//��������
	temp=addr>>16;								//������ַ
	SpiWriteByte(MX25SPINO,temp);
	temp=addr>>8;
	SpiWriteByte(MX25SPINO,temp);
	temp=addr&0xff;
	SpiWriteByte(MX25SPINO,temp);
	Mx25Disable();
	
	while(Mx25GetSt()&0x01);  // �Ƿ�д��	

	Mx25Enable();
	SpiWriteByte(MX25SPINO,SPI_FLASH_INS_WRDI);			//дʧ��
	Mx25Disable();
}




/****************************************************************************\
 Function:  Mx25GetSt
 Descript: ����ȡ�ͣأ�����״̬�Ĵ���
 Input:	    ��ַ
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-21
 *****************************************************************************/
static uint8_t Mx25GetSt(void)
{	
	uint8_t st=0;
	
	Mx25Enable();
	
	SpiWriteByte(MX25SPINO,0x05);
	st=SpiReadByte(MX25SPINO);
	
	Mx25Disable();

	return st;
}



/****************************************************************************\
 Function:  Mx25Enable
 Descript: ��ʹ�ܣͣأ���
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-21
 *****************************************************************************/
static void Mx25Enable(void)
{
	GpioSetDir(GPIO_NORCS,OUTPUT);
	GpioSetLev(GPIO_NORCS,LOW);
}



/****************************************************************************\
 Function:  Mx25Disable
 Descript: ��ʧ�ܣͣأ���
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    quanhouwei
 Datetime:  17-08-21
 *****************************************************************************/
static void Mx25Disable(void)
{
	GpioSetDir(GPIO_NORCS,OUTPUT);
	GpioSetLev(GPIO_NORCS,HIGH);
}





