#ifndef _PARA_H
#define _PARA_H

//EEPROM�д洢���ݷֲ�
//64���ֽ�Ϊһҳ����64��ҳ
#define EEPROMADDR_PARAINFO 0 //���� ��1ҳ-��16ҳ
#define EEPROMADDR_SAVEINFO 16 // �ϵ���Ҫ�洢������ ��16ҳ
#define EEPROMADDR_SAVERUNHOUR 17// �ϵ���Ҫ�洢������ ��17ҳ
#define EEPROMADDR_TRICKLECTRLINFO 18// �ϵ���Ҫ�洢������ ���������ز��� ��17ҳ



//����ͨ��
#define CHANNEL_ETH		0X01	//��̫��ͨ��
#define CHANNEL_GPRS	0X02	//GPRSͨ��

#define PARACHANNEL_UART  0X01  // ��������ͨ��
#define PARACHANNEL_UDP   0X02  // UDP��������ͨ��

//����
typedef struct{
	uint32_t version; //�汾�� 0x5A5A5A01
	
	uint8_t  ChgGunNumber; // ��ǹ���� 1��2
	
	uint8_t  UpChannel; // ����ͨ��
	uint8_t LocalIp[4]; //����IP
	uint8_t SubMask[4]; //��������
	uint8_t GateWay[4]; //����
	uint8_t PhyMac[6]; //�����ַ
	
	uint8_t HardVersion[16]; //Ӳ���汾�� ASCII
	
	uint8_t CustomerNumber[6]; //�ͻ����
	
	uint8_t APN[16]; //APN
	uint8_t APNUser[32]; //APN�û���
	uint8_t APNPasswd[32]; //APN����
	
	uint8_t ScreenOpenHour; //���������ʱ�� Сʱ
	uint8_t ScreenOpenMin; //���������ʱ�� ����
	uint8_t ScreenStopHour; //������ر�ʱ�� Сʱ
	uint8_t ScreenStopMin; //������ر�ʱ�� ����
	
	struct{
		uint8_t PileNo[32]; //���׮��� ASCII
		uint8_t ACMeterAddr[6]; //��������ַ MeterAddr[0]��A0
		uint8_t DCMeterAddr[6]; //ֱ������ַ MeterAddr[0]��A0
		uint16_t LoginTimeInterval; //ǩ�����ʱ�� ����
		uint16_t NetHeartTime; //������������ ��λS
		uint8_t NetCommOverCnt; //����ͨѶ��ʱ���Դ���
		uint8_t  ServerIp[4]; //������IP
		uint16_t ServerPort; //�������˿�
		uint8_t StartHour[12]; //�ƷѲ��� ��ʼСʱ
		uint8_t StartMin[12]; //�ƷѲ��� ��ʼ����
		uint8_t StopHour[12]; //�ƷѲ��� ����Сʱ
		uint8_t StopMin[12]; //�ƷѲ��� ��������
		uint32_t Money[12]; //�ƷѲ��� ���� ÿ�ȵ�ĵ�� ����5λС��
		uint32_t StateInfoReportPeriod; //���׮״̬��Ϣ�ϱ����� ��λ��
		uint8_t QrData[256]; //��ά������
	}Port[2];
	
	uint8_t ManufactureDate[4]; //��������
	uint8_t ManufactureBase[16]; //��������
	uint8_t TestWorker[8]; //�����
	uint8_t LifeTime[5]; //ʹ����
}PARAINFO;

typedef struct{
	uint32_t Updata;         //�����ɹ���־
	uint16_t Pwm;
	uint8_t ActPassword[4];
	uint32_t LimitHour;
	uint8_t ActCode[32];

}CFGINFO;

typedef struct{
	uint8_t LimitSoc;
	uint16_t LimitCur;
	uint16_t LimitMin;  
	uint32_t InitFlag;  //�����ɹ���־

}TRICKLEINFO;


void ParaInit(void);
void 	ParaSetChgGunNumber(uint8_t number);
uint8_t ParaGetChgGunNumber(void);
void ParaSetPileNo(uint8_t port, uint8_t *data);
void ParaGetPileNo(uint8_t port, uint8_t *data);
void ParaSetLoginTimeInterval(uint8_t port, uint16_t TimeInterval);
uint16_t ParaGetLoginTimeInterval(uint8_t port);
void ParaSetACMeterAddr(uint8_t port, uint8_t *data);
void ParaGetACMeterAddr(uint8_t port, uint8_t *data);
void ParaSetDCMeterAddr(uint8_t port, uint8_t *data);
void ParaGetDCMeterAddr(uint8_t port, uint8_t *data);
void ParaSetNetHeartTime(uint8_t port, uint16_t time);
uint16_t ParaGetNetHeartTime(uint8_t port);
void ParaSetNetCommOverCnt(uint8_t port, uint8_t cnt);
uint8_t ParaGetNetCommOverCnt(uint8_t port);
void ParaSetUpChannel(uint8_t channel);
uint8_t ParaGetUpChannel(void);
void ParaSetLocalIp(uint8_t *data);
void ParaGetLocalIp(uint8_t *data);
void ParaSetSubMask(uint8_t *data);
void ParaGetSubMask(uint8_t *data);
void ParaSetGateWay(uint8_t *data);
void ParaGetGateWay(uint8_t *data);
void ParaSetPhyMac(uint8_t *data);
void ParaGetPhyMac(uint8_t *data);
void ParaSetServerIp(uint8_t port, uint8_t *data);
void ParaGetServerIp(uint8_t port, uint8_t *data);
void ParaSetServerPort(uint8_t port, uint16_t serverport);
uint16_t ParaGetServerPort(uint8_t port);
void ParaSetFeilvTime(uint8_t port, uint8_t period, uint8_t starthour, uint8_t startmin, uint8_t stophour, uint8_t stopmin);
void ParaGetFeilvTime(uint8_t port, uint8_t period, uint8_t *starthour, uint8_t *startmin, uint8_t *stophour, uint8_t *stopmin);
void ParaSetFeilvMoney(uint8_t port, uint8_t period, uint32_t money);
uint32_t ParaGetFeilvMoney(uint8_t port, uint8_t period);
void ParaSetStateInfoReportPeriod(uint8_t port, uint16_t time);
uint16_t ParaGetStateInfoReportPeriod(uint8_t port);
void ParaSetQRCode(uint8_t port, uint8_t *qrcode);
void ParaGetQRCode(uint8_t port, uint8_t *qrcode);
void ParaSetCustomerNumber(uint8_t *data);
void ParaGetCustomerNumber(uint8_t *data);
void ParaSetAPN(uint8_t *data);
void ParaGetAPN(uint8_t *data);
void ParaSetAPNUser(uint8_t *data);
void ParaGetAPNUser(uint8_t *data);
void ParaSetAPNPasswd(uint8_t *data);
void ParaGetAPNPasswd(uint8_t *data);
void ParaSetScreenTime(uint8_t openhour, uint8_t openmin, uint8_t closehour, uint8_t closemin);
void ParaGetScreenTime(uint8_t *openhour, uint8_t *openmin, uint8_t *closehour, uint8_t *closemin);
void ParaSetManufactureDate(uint8_t *Date);
void ParaGetManufactureDate(uint8_t *Date);
void ParaSetManufactureBase(uint8_t *Base);
void ParaGetManufactureBase(uint8_t *Base);
void ParaSetTestWorker(uint8_t *TestWorker);
void ParaGetTestWorker(uint8_t *TestWorker);
void ParaSetLifeTime(uint8_t *LifeTime);
void ParaGetLifeTime(uint8_t *LifeTime);
void AnalyseMsgFromDebugPlatform(void);

void SetUpdataFlag(uint32_t flag);
uint32_t GetUpdata(void);

void ClearParaCmd(void);
uint8_t SetParaCmd(void);
uint8_t GetParaCmd(void);
uint8_t BoardGetModuleType(void);
uint8_t BoardGetMoudleNum(void);
uint16_t BoardGetMaxCurrent(void);
uint8_t BoardGetPowerMode(void);
void SetParaResult(uint8_t Result);

uint32_t ParaGetLimitHour(void);
uint8_t ParaSetActCode(uint8_t * data);
uint32_t ParaGetRunHour(void);
void ParaSavaRunHour(uint32_t  runhour);

void SetTrickleCur(uint16_t Cur);
void SetTrickleSoc(uint8_t Soc);
void SetTrickleMin(uint16_t Min);
void SetTrickleInfo(uint16_t Cur, uint8_t Soc, uint16_t Min);
uint8_t GetTrickleSoc(void);
uint16_t GetTrickleCur(void);
uint16_t GetTrickleMin(void);


uint8_t ConfigProtocolFrameCheck(uint8_t *buf, uint8_t Channel);
uint8_t DebugPlatformReadDataAnalyse(uint8_t *buf, uint8_t Channel);
uint8_t DebugPlatformWriteDataAnalyse(uint8_t *buf, uint8_t Channel);



#endif
