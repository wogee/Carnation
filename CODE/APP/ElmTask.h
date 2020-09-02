#ifndef _ELMTASK_H
#define _ELMTASK_H

// ״̬
#define ELM_ST_INIT       	0     	// ��ʼ��
#define ELM_ST_TEST       	1		// ���
#define ELM_ST_READTIME   	2    	// ��ʱ��
#define ELM_ST_READDATA     3	   	// ������	


// ����
#define TYPE_ELECY		0X01 // ����
#define TYPE_VOL			0X02 // ��ѹ
#define TYPE_CURRENT	0X03 // ����	
#define TYPE_DATE			0X04 // ����
#define TYPE_TIME			0X05 // ʱ��



#define ELMBAUD         		2400   		// ������� 

#define ELMMAXCNT     			2   		// ��������



#define ELM_CMD_READDATA   		0X11  		// ������
#define ELM_ACK_READDATA   		0X91	 	// ������Ӧ��
#define ELM_ERR_READDATA   		0XD1  		// �����ݴ���


#define ELM_DI_READTOTALELC  	0X00010000  // ���ܵ���
#define ELM_DI_READVOL	 		0x0201ff00  // ����ѹ
#define ELM_DI_READCURRENT		0x0202ff00	// ������
#define ELM_DI_READDAY			0x04000101  // ������
#define ELM_DI_READTIME			0x04000102  // ��ʱ��




#define ELMHEAD  0x68    // ֡ͷ
#define ELMTAIL  0x16	 // ֡β	


typedef struct 
{
    uint8_t  addr[6];		// ��ַ
	uint8_t  ctlcode; 	// ������
	uint8_t  buflen; 		// ���ݳ���
	uint8_t  buf[128];	// ����
}ElmRcvMsg;


typedef struct
{
	uint8_t state;				// ״̬

	uint32_t totalelc;   			// ��ǰ�ܵ���  1/100KWH

	uint32_t vol;			  		// ��ѹ	1/10 v

	uint32_t current;		  		// ����   1/1000 A

	uint32_t timeoutcnt;	  		// ��ʱ����
	
	uint8_t commstate; //ͨ��״̬ 0���� 1ͨ���쳣
}ELMINFO;









void 	ElmTask(void *pdata);
uint8_t 	ElmIsOK(uint8_t id);
uint32_t 	ElmGetTotalElc(uint8_t id);
uint32_t 	ElmGetVolA(uint8_t id);
uint32_t 	ElmGetVolB(uint8_t id);
uint32_t 	ElmGetVolC(uint8_t id);
uint32_t 	ElmGetCurA(uint8_t id);
uint32_t 	ElmGetCurB(uint8_t id);
uint32_t 	ElmGetCurC(uint8_t id);
uint8_t ElmGetCommState(uint8_t id);


static void 	ElmProc(void);
void 	ElmInit(uint8_t id);
static uint8_t  	ElmTest(uint8_t id);
static uint8_t 	ElmCheckTime(uint8_t id);
static void 	ElmErrCheck(void);




#endif




