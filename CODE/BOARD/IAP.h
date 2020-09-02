#ifndef __IAP_H
#define	__IAP_H

#include <stdint.h>
#include <string.h>

#define UPOK			0
#define UPERR			1

#define IAP_FCCLK				(96000)			//����CCLKֵ��С����λΪKHz 
#define IAP_ENTER_ADR		0x1FFF1FF1	//IAP��ڵ�ַ����

#define SECTORSIZE			(32*1024)		//������С
 
//����IAP������                                  
#define	IAP_Prepare				50		/* ѡ������                     */
#define	IAP_RAMTOFLASH		51		/* �������� FLASHĿ���ַ       */
#define	IAP_ERASESECTOR		52		/* ��������    ����ʼ������     */
#define	IAP_BLANKCHK			53		/* �������    ����ʼ�����š�   */
#define	IAP_READPARTID		54		/* ������ID    ���ޡ�           */
#define	IAP_BOOTCODEID		55		/* ��Boot�汾�š��ޡ�           */
#define	IAP_COMPARE				56		/* �Ƚ�����    ��Flash��ʼ��ַ  */

#define USERFLAGADDR		0x0000F000	//�û������־��
#define DOWNLOADADDR		0x00048000	//�������׵�ַ

#define SECTORFLAG			15	//�û������־������
#define SECTORDOWNLOAD1	23	//��������ʼ����
#define SECTORDOWNLOAD2	29	//��������������

extern void begin_update(void);
extern uint8_t process_update(uint32_t total_len,uint8_t *pdat,uint32_t flen);
extern void end_update(void);

#endif
