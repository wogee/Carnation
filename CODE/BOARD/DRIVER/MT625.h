#ifndef _MT625_H
#define _MT625_H



#define VIF_REG_SECTOR		0x01 //ע����Ϣ������
#define VIF_DATA_SECTOR		0x02 //��֤����������
#define VIF_REG_BLOCK			0x00 //ע����Ϣ���
#define VIF_DATA_BLOCK    0x00 //0x00~0x03  ��֤�����
#define VIF_BLOCK_NUM_BA   0x01 //0x00~0x03


typedef struct 
{
	uint8_t type;     	// ������
	uint8_t cmd; 			// ���������
	uint8_t buflen; 		// ���ܵ������ݳ���
	uint8_t buf[128];		// ���ܵ�������
}MTRcvMsg;


typedef struct 
{
	uint8_t    sectorx;   //������
    uint8_t    bldata[16];
	
}M1BLOCK;





typedef struct{
	uint8_t KeyA[6];   //��֤B����
	uint8_t KeyB[6];   //��֤A����

	uint32_t cardID;//��ID  
	uint8_t type;//���� 0x01:���߿� 0x02:���߿�
	uint8_t lockst;//��״̬ 0x01:���� 0x02:����
	uint32_t Money;//���
	uint8_t recdInx;//6����¼��
	uint8_t rechargeInx;//��ֵ��¼��
	uint8_t PileNumber[4];//׮��ź�4λ
}M1CARDVIF;//m1����֤���ȡ

typedef struct 
{
    uint8_t    sectorx; //������
    
	uint8_t    KeyA[6]; //��֤B����
	uint8_t    KeyB[6]; //��֤A����
	uint32_t   cardID;
	
  //��Ƭ��¼���
    uint8_t   deviceNoH;  //1׮���      0xFFFF
	uint8_t   deviceNoL;
	uint32_t 	timestpA;	//3~6ʱ���      0xFFFFFFFF
	uint32_t 	timestpB;	//3~6ʱ���      0xFFFFFFFF
	uint32_t 	timestpC;	//3~6ʱ���      0xFFFFFFFF
	uint32_t 	timestpD;	//3~6ʱ���      0xFFFFFFFF
	uint16_t 	usepowerA;	//7~9ʹ�õ���    0x00FFFFFF 
    uint16_t 	usepowerB;	//7~9ʹ�õ���    0x00FFFFFF 
    uint16_t 	usepowerC;	//7~9ʹ�õ���    0x00FFFFFF 	
	uint32_t  conmoneyA;	//10~12ʹ�ý��  0x00FFFFFF    10000.00 Ԫ��������λС��
	uint32_t  conmoneyB;	//10~12ʹ�ý��  0x00FFFFFF    10000.00 Ԫ��������λС��
	uint32_t  conmoneyC;	//10~12ʹ�ý��  0x00FFFFFF    10000.00 Ԫ��������λС��
    uint8_t   rfu[3];     //13~15Ԥ�� 
	uint8_t   crc;        //16           crc8
		
}M1CARDREC;//m1����¼




#define    TYPE_START               0x01	    // ��ʼ
#define    TYPE_END					0x02	    // ����

#define  	MT_CMD_M1_FIND   		0x30        // Ѱ��M1��
#define  	MT_CMD_M1_READID   	    0x31        // ��ȡM1����
#define  	MT_CMD_M1_VIFKEYA       0x32        // ��֤M1����A����
#define  	MT_CMD_M1_VIFKEYB       0x39        // ��֤M1����B����

#define  	MT_CMD_M1_BLOCKRD       0x33        // ��ȡ������
#define  	MT_CMD_M1_BLOCKWR       0x34        // д������

#define 	MT_STX 					0x02        // ͷ
#define 	MT_ETX	 				0x03	    // β

#define     MT_CMD_TYPE1   			0x51   	    // ���1
#define     MT_CMD_TYPE2        	0x34        // ���2

#define  	MT_CMD_CARDFIND   		0x37        // Ѱ��
#define  	MT_CMD_CARDREADINFO		0x36        // ������Ϣ
#define  	MT_CMD_CARDSTART  		0x30	    // ��ʼ�ӵ�
#define  	MT_CMD_CARDEND			0x34        // �����ӵ�
#define  	MT_CMD_CARDSUPPLY 		0x35        // ���佻��
#define  	MT_CMD_CARDREADID		0x56        // ����ID
#define  	MT_CMD_CARDCLOSEANT		0x42        // �ر���Ƶ����


#define     MT_ACK_OK        		0x59        // ��ȷ
#define     MT_ACK_FAIL				0x4e        // ʧ��
#define     MT_ACK_NOCARD          	0x45        // �޿�
#define     MT_ACK_PINERR			0x83	    // ����PINʧ��
#define     MT_ACK_CARDERR1			0x23        // ��Ƭ������
#define     MT_ACK_CARDERR2      	0x47        // psam��λʧ��
#define     MT_ACK_CARDERR3        	0x48	    // �û�����λʧ��





void MTInit(void);
uint8_t MTSearch(uint32_t *UID);
uint8_t MTRead(M1CARDVIF *vifdata);
uint8_t MTWrite(M1CARDVIF *vifdata);


#endif
