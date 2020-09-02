#include "IAP.h"
#include "lpc177x_8x.h"
#include "includes.h"
__align(4) uint8_t GucIapTmp[1024];

void (*IAP_Entry) (uint32_t param_tab[], uint32_t result_tab[]);

uint32_t  paramin[8];
uint32_t  paramout[8];

uint32_t *GpulFlagPoint = (uint32_t *)USERFLAGADDR;

uint8_t SysUpdateFlag;
uint32_t update_len;
uint32_t update_last_remain_len;

/*********************************************************************************************************
** Function name:       sectorPrepare
** Descriptions:        IAP��������ѡ���������50
** input parameters:    sec1:           ��ʼ����
**                      sec2:           ��ֹ����
** output parameters:   paramout[0]:    IAP����״̬��,IAP����ֵ     
** Returned value:      paramout[0]:    IAP����״̬��,IAP����ֵ                     
*********************************************************************************************************/
uint32_t  sectorPrepare (uint8_t sec1, uint8_t sec2)
{  
    paramin[0] = IAP_Prepare;                                           /* ����������                   */
    paramin[1] = sec1;                                                  /* ���ò���                     */
    paramin[2] = sec2;                            
    (*IAP_Entry)(paramin, paramout);                                    /* ����IAP�������              */
   
    return (paramout[0]);                                               /* ����״̬��                   */
}

/*********************************************************************************************************
** Function name:       ramCopy
** Descriptions:        ����RAM�����ݵ�FLASH���������51
** input parameters:    dst:            Ŀ���ַ����FLASH��ʼ��ַ����512�ֽ�Ϊ�ֽ�
**                      src:            Դ��ַ����RAM��ַ����ַ�����ֶ���
**                      no:             �����ֽڸ�����Ϊ512/1024/4096/8192
** output parameters:   paramout[0]:    IAP����״̬��,IAP����ֵ     
** Returned value:      paramout[0]:    IAP����״̬��,IAP����ֵ                     
*********************************************************************************************************/
uint32_t  ramToFlash (uint32_t dst, uint32_t src, uint32_t no)
{  
    paramin[0] = IAP_RAMTOFLASH;                                        /* ����������                   */
    paramin[1] = dst;                                                   /* ���ò���                     */
    paramin[2] = src;
    paramin[3] = no;
    paramin[4] = IAP_FCCLK;
    (*IAP_Entry)(paramin, paramout);                                    /* ����IAP�������              */
    
    return (paramout[0]);                                               /* ����״̬��                   */
}

/*********************************************************************************************************
** Function name:       sectorErase
** Descriptions:        �����������������52
** input parameters:    sec1            ��ʼ����
**                      sec2            ��ֹ����92
** output parameters:   paramout[0]:    IAP����״̬��,IAP����ֵ
** Returned value:      paramout[0]:    IAP����״̬��,IAP����ֵ                     
*********************************************************************************************************/
uint32_t  sectorErase (uint8_t sec1, uint8_t sec2)
{  
    paramin[0] = IAP_ERASESECTOR;                                       /* ����������                   */
    paramin[1] = sec1;                                                  /* ���ò���                     */
    paramin[2] = sec2;
    paramin[3] = IAP_FCCLK;
    (*IAP_Entry)(paramin, paramout);                                    /* ����IAP�������              */
   
    return (paramout[0]);                                               /* ����״̬��                   */
}

/*********************************************************************************************************
** Function name:       blankChk
** Descriptions:        ������գ��������53
** input parameters:    sec1:           ��ʼ����
**                      sec2:           ��ֹ����92
** output parameters:   paramout[0]:    IAP����״̬��,IAP����ֵ
** Returned value:      paramout[0]:    IAP����״̬��,IAP����ֵ                     
*********************************************************************************************************/
uint32_t  blankChk (uint8_t sec1, uint8_t sec2)
{  
    paramin[0] = IAP_BLANKCHK;                                          /* ����������                   */
    paramin[1] = sec1;                                                  /* ���ò���                     */
    paramin[2] = sec2;
    (*IAP_Entry)(paramin, paramout);                                    /* ����IAP�������              */

    return (paramout[0]);                                               /* ����״̬��                   */
}

/*********************************************************************************************************
** Function name:       parIdRead
** Descriptions:        ������գ��������54
** input parameters:    ��
** output parameters:   paramout[0]:    IAP����״̬��,IAP����ֵ
** Returned value:      paramout[0]:    IAP����״̬��,IAP����ֵ                     
*********************************************************************************************************/
uint32_t  parIdRead (void)
{  
    paramin[0] = IAP_READPARTID;                                        /* ����������                   */
    (*IAP_Entry)(paramin, paramout);                                    /* ����IAP�������              */

    return (paramout[0]);                                               /* ����״̬��                   */
}

/*********************************************************************************************************
** Function name:       codeIdBoot
** Descriptions:        ������գ��������55
** input parameters:    ��
** output parameters:   paramout[0]:    IAP����״̬��,IAP����ֵ
** Returned value:      paramout[0]:    IAP����״̬��,IAP����ֵ                     
*********************************************************************************************************/
uint32_t  codeIdBoot (void)
{  
    paramin[0] = IAP_BOOTCODEID;                                        /* ����������                   */
    (*IAP_Entry)(paramin, paramout);                                    /* ����IAP�������              */

    return (paramout[0]);                                               /* ����״̬��                   */
}

/*********************************************************************************************************
** Function name:       dataCompare
** Descriptions:        У�����ݣ��������56
** input parameters:    dst:            Ŀ���ַ����RAM/FLASH��ʼ��ַ����ַ�����ֶ���
**                      src:            Դ��ַ����FLASH/RAM��ַ����ַ�����ֶ���
**                      no:             �����ֽڸ����������ܱ�4����
** output parameters:   paramout[0]:    IAP����״̬��,IAP����ֵ
** Returned value:      paramout[0]:    IAP����״̬��,IAP����ֵ                     
*********************************************************************************************************/
uint32_t  dataCompare (uint32_t dst, uint32_t src, uint32_t no)
{  
    paramin[0] = IAP_COMPARE;                                           /* ����������                   */
    paramin[1] = dst;                                                   /* ���ò���                     */
    paramin[2] = src;
    paramin[3] = no;
    (*IAP_Entry)(paramin, paramout);                                    /* ����IAP�������              */

    return (paramout[0]);                                               /* ����״̬��                   */
}

/*********************************************************************************************************
** Function name:			begin_update
** Descriptions:			��ʼ����ʱ����
** Input parameters:	��
** Output parameters:	��
** Returned value:		0ʧ�� 1�ɹ�
*********************************************************************************************************/
void begin_update(void)
{
	SysUpdateFlag = 1;
	update_len = 0;
	update_last_remain_len = 0;
	IAP_Entry = (void(*)())IAP_ENTER_ADR;
	OS_ENTER_CRITICAL();
	sectorPrepare(SECTORDOWNLOAD1, SECTORDOWNLOAD2);
	sectorErase(SECTORDOWNLOAD1, SECTORDOWNLOAD2);
	OS_EXIT_CRITICAL();
}

/*********************************************************************************************************
** Function name:			process_update
** Descriptions:			�������̺���
** Input parameters:	pdat�û����� flen��д����û������ֽ���
** Output parameters:	��
** Returned value:		�ɹ�����0 ���򷵻�1
*********************************************************************************************************/
uint8_t process_update(uint32_t total_len,uint8_t *pdat,uint32_t flen)
{
	uint8_t sectoroffset;
	uint32_t sectoraddress;
	
	if(SysUpdateFlag != 1)
		return UPERR;

	while(flen){
		if((update_last_remain_len + flen) >= 512){
			memcpy(GucIapTmp+update_last_remain_len, pdat, 512 - update_last_remain_len);
			flen = flen - (512 - update_last_remain_len);
			pdat = pdat + (512 - update_last_remain_len);
			update_last_remain_len = 512;
		}else{
			memcpy(GucIapTmp + update_last_remain_len, pdat, flen);
			update_last_remain_len += flen;
			flen = 0;
		}
	
		if(update_last_remain_len == 512){
			sectoroffset = update_len / SECTORSIZE + SECTORDOWNLOAD1;  // ��������ƫ��
			sectoraddress = DOWNLOADADDR + update_len;
			OS_ENTER_CRITICAL();
			sectorPrepare(sectoroffset, sectoroffset);
			ramToFlash(sectoraddress, (uint32_t)GucIapTmp, 512);
			if(0 != dataCompare((uint32_t)GucIapTmp, sectoraddress, 512)){
				OS_EXIT_CRITICAL();
				return UPERR;
			}
			OS_EXIT_CRITICAL();
			update_len += 512;
			update_last_remain_len = 0;
		}
	}
	if((update_last_remain_len != 0) && ((update_last_remain_len + update_len) >= total_len)){//�����ļ��Ѵ�����ɣ�
		memset(GucIapTmp+update_last_remain_len, 0xff, 512 - update_last_remain_len);
		sectoroffset = update_len / SECTORSIZE+SECTORDOWNLOAD1;
		sectoraddress = DOWNLOADADDR + update_len;
		OS_ENTER_CRITICAL();
		sectorPrepare(sectoroffset, sectoroffset);
		ramToFlash(sectoraddress,(uint32_t)GucIapTmp, 512);
		if(0 != dataCompare((uint32_t)GucIapTmp, sectoraddress, 512)){
			OS_EXIT_CRITICAL();
			return UPERR;
		}
		OS_EXIT_CRITICAL();
		update_len += update_last_remain_len;
		update_last_remain_len = 0;
	}
	
	return UPOK;
}

/*********************************************************************************************************
** Function name:	    	end_update
** Descriptions:	    	��������ʱ����
** input parameters:    ��
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void end_update(void)
{
	memset(GucIapTmp, 0xff, sizeof(GucIapTmp));
	memset(GucIapTmp, 0x5a, 4);
	OS_ENTER_CRITICAL();
	sectorPrepare(SECTORFLAG, SECTORFLAG);				/* ����׼��	 */
	sectorErase(SECTORFLAG, SECTORFLAG);					/* ��������	 */
	sectorPrepare(SECTORFLAG, SECTORFLAG);				/* ѡ������	 */
	ramToFlash(USERFLAGADDR,(uint32_t)GucIapTmp, 256);	/* ���FLASH */
	OS_EXIT_CRITICAL();
}

