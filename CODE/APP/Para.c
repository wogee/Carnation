#include "App.h"
#include "EEPROM.h"
#include "UART.h"
#include <LPC177x_8x.H>
#include "RTC.h"
#include "Para.h"
#include <string.h>
#include "cfg.h"
#include "Board.h"
#include <stdio.h>
#include "includes.h"
#include "record.h"
#include "timer.h"
#include "EXT_ChgTask.h"
#include "MyAlgLib.h"
#include "stdlib.h"
#include "ethernet.h"

static PARAINFO ParaInfo;
static CFGINFO CfgInfo;
static TRICKLEINFO TrickleInfo;

static uint32_t RunHour;

static uint8_t ModuleType;
static uint8_t MoudleNum;
static uint16_t MaxCurrent;
static uint8_t PowerMode;
static uint8_t SetParacmd;
static uint8_t ParaResult;
static uint8_t  GetParacmd;

/************************************************************************************************************
** �� �� �� : GetUpdata
** �������� : ��ȡ������־
** ��    �� : ��
** ��    �� : ��
** ��    �� :	0x5a ����   0  ������
*************************************************************************************************************
*/
uint32_t GetUpdata(void)
{
	EepromRead(0, EEPROMADDR_SAVEINFO, &CfgInfo, MODE_8_BIT, sizeof(CFGINFO));
	return CfgInfo.Updata;
}



/************************************************************************************************************
** �� �� �� : CfgSave
** �������� : �������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void CfgSave(void)
{
	//Ӧ���ӻ������ź���
	EepromWrite(0, EEPROMADDR_SAVEINFO, &CfgInfo, MODE_8_BIT, sizeof(CFGINFO));
}

/************************************************************************************************************
** �� �� �� : InitTrickleInfo
** �������� : �����������ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void InitTrickleInfo(void)
{
	EepromRead(0, EEPROMADDR_TRICKLECTRLINFO, &TrickleInfo, MODE_8_BIT, sizeof(TRICKLEINFO));
	if(TrickleInfo.InitFlag !=0x5a5a5a52){
		TrickleInfo.InitFlag = 0x5a5a5a52;
		TrickleInfo.LimitSoc = 100;
		TrickleInfo.LimitCur = 0;
		TrickleInfo.LimitMin = 0;
		EepromWrite(0, EEPROMADDR_TRICKLECTRLINFO, &TrickleInfo, MODE_8_BIT, sizeof(TRICKLEINFO));
	}
}


/************************************************************************************************************
** �� �� �� : SetTrickleCur
** �������� : �����������������ֵ    ��λ  0.1A
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SetTrickleCur(uint16_t Cur)
{
	TrickleInfo.LimitCur = Cur;
	EepromWrite(0, EEPROMADDR_TRICKLECTRLINFO, &TrickleInfo, MODE_8_BIT, sizeof(TRICKLEINFO));
}

/************************************************************************************************************
** �� �� �� : SetTrickleInfo
** �������� : ����������������Ϣ    ��λ  0.1A
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SetTrickleInfo(uint16_t Cur, uint8_t Soc, uint16_t Min)
{
	TrickleInfo.LimitCur = Cur;
	TrickleInfo.LimitSoc = Soc;
	TrickleInfo.LimitMin = Min;
	EepromWrite(0, EEPROMADDR_TRICKLECTRLINFO, &TrickleInfo, MODE_8_BIT, sizeof(TRICKLEINFO));
}
/************************************************************************************************************
** �� �� �� : GetTrickleCur
** �������� : ��ȡ�������������ֵ    ��λ  0.1A
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
uint16_t GetTrickleCur(void)
{
	return TrickleInfo.LimitCur;
}	


/************************************************************************************************************
** �� �� �� : GetTrickleMin
** �������� : ��ȡ�������������ֵ    ��λ  0.1A
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
uint16_t GetTrickleMin(void)
{
	return TrickleInfo.LimitMin;
}
/************************************************************************************************************
** �� �� �� : SetTrickleMin
** �������� : ���������� ʱ������ ��λ����  
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SetTrickleMin(uint16_t Min)
{
	TrickleInfo.LimitMin = Min;
	EepromWrite(0, EEPROMADDR_TRICKLECTRLINFO, &TrickleInfo, MODE_8_BIT, sizeof(TRICKLEINFO));
}

/************************************************************************************************************
** �� �� �� : SetTrickleSoc
** �������� : ���������� SOC ����ֵ   
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SetTrickleSoc(uint8_t Soc)
{
	TrickleInfo.LimitSoc = Soc;
	EepromWrite(0, EEPROMADDR_TRICKLECTRLINFO, &TrickleInfo, MODE_8_BIT, sizeof(TRICKLEINFO));
}
/************************************************************************************************************
** �� �� �� : GetTrickleSoc
** �������� : ��ȡ������ SOC ����ֵ   
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
uint8_t GetTrickleSoc(void)
{
	return TrickleInfo.LimitSoc;
}
/************************************************************************************************************
** �� �� �� : SetUpdataFlag
** �������� : ���ø��±�ʶ
** ��    �� : flag
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SetUpdataFlag(uint32_t flag)
{
	CfgInfo.Updata = flag;
	CfgSave();
}


/************************************************************************************************************
** �� �� �� : ParaDefault
** �������� : ����Ĭ�ϲ���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void ParaDefault(void)
{
	uint8_t port,i;
	
	memset(&ParaInfo,0,sizeof(PARAINFO));
	
	ParaInfo.version = 0x5a5a5a02;
	
	//��ǹ����
	ParaInfo.ChgGunNumber = 1;
	
	//����ͨ�� Ĭ��1��̫��
	ParaInfo.UpChannel = 1;
	
	//����IP
	ParaInfo.LocalIp[0] = 192;
	ParaInfo.LocalIp[1] = 168;
	ParaInfo.LocalIp[2] = 1;
	ParaInfo.LocalIp[3] = 10;
	
	//����
	ParaInfo.GateWay[0] = 192;
	ParaInfo.GateWay[1] = 168;
	ParaInfo.GateWay[2] = 1;
	ParaInfo.GateWay[3] = 1;
	
	//��������
	ParaInfo.SubMask[0] = 255;
	ParaInfo.SubMask[1] = 255;
	ParaInfo.SubMask[2] = 255;
	ParaInfo.SubMask[3] = 0;
	
	//�����ַ
	ParaInfo.PhyMac[0] = 0x00;
	ParaInfo.PhyMac[1] = 0x00;
	ParaInfo.PhyMac[2] = 0x10;
	ParaInfo.PhyMac[3] = 0x00;
	ParaInfo.PhyMac[4] = 0x00;
	ParaInfo.PhyMac[5] = 0x00;
	
	//Ӳ���汾��
	memset(ParaInfo.HardVersion, 0, 16);
	
	//�ͻ����
	memset(ParaInfo.CustomerNumber, 0, 6);
	
	memset(ParaInfo.APN, 0, 16); //APN
	memset(ParaInfo.APNUser, 0, 32); //APN�û���
	memset(ParaInfo.APNPasswd, 0, 32); //APN����
	
	ParaInfo.ScreenOpenHour = 18; //���������ʱ�� Сʱ
	ParaInfo.ScreenOpenMin = 0; //���������ʱ�� ����
	ParaInfo.ScreenStopHour = 6; //������ر�ʱ�� Сʱ
	ParaInfo.ScreenStopMin = 0; //������ر�ʱ�� ����
	
	for(port = 0; port < 2; port++){
		if(port == 0){
			memcpy(ParaInfo.Port[port].PileNo, "0000000000000001", 16);
		}else{
			memcpy(ParaInfo.Port[port].PileNo, "0000000000000002", 16);
		}
		memset(ParaInfo.Port[port].ACMeterAddr, 0, 6);
		memset(ParaInfo.Port[port].DCMeterAddr, 0, 6);
		ParaInfo.Port[port].LoginTimeInterval = 30;
		ParaInfo.Port[port].NetHeartTime = 20;
		ParaInfo.Port[port].NetCommOverCnt = 3;
		ParaInfo.Port[port].ServerIp[0] = 39;
		ParaInfo.Port[port].ServerIp[1] = 108;
		ParaInfo.Port[port].ServerIp[2] = 62;
		ParaInfo.Port[port].ServerIp[3] = 139;
		ParaInfo.Port[port].ServerPort = 18090;
		for (i = 0; i < 12; i++){
			ParaInfo.Port[port].StartHour[i] = 0;
			ParaInfo.Port[port].StartMin[i] = 0; //�ƷѲ��� ��ʼ����
			ParaInfo.Port[port].StopHour[i] = 0; //�ƷѲ��� ����Сʱ
			ParaInfo.Port[port].StopMin[i] = 0; //�ƷѲ��� ��������
			ParaInfo.Port[port].Money[i] = 0; //�ƷѲ��� ���� ÿ�ȵ�ĵ�� ����5λС��
		}
		
		ParaInfo.Port[port].StateInfoReportPeriod = 15;
		//memcpy(ParaInfo.Port[port].QrData, ParaInfo.Port[port].PileNo, 32);
	}
	
	memset(ParaInfo.ManufactureDate, 0, 4); //��������
	memset(ParaInfo.ManufactureBase, 0, 16); //��������
	memset(ParaInfo.TestWorker, 0, 8); //�����
	memset(ParaInfo.LifeTime, 0, 5); //ʹ����
}

/************************************************************************************************************
** �� �� �� : SaveInfoDefault
** �������� : ������ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SaveInfoDefault(void)
{
	memset(&CfgInfo,0,sizeof(CFGINFO));
	CfgInfo.Updata = 0;
	memcpy(CfgInfo.ActPassword, "0000", 4);
	CfgInfo.LimitHour = 0;
	RunHour = 0;
}


/************************************************************************************************************
** �� �� �� : SaveInfoV2Def
** �������� : ��������������ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SaveInfoV2Def(void){
	memcpy(CfgInfo.ActPassword, "0000", 4);
	CfgInfo.LimitHour = 0;
	RunHour = 0;	
}


/************************************************************************************************************
** �� �� �� : ParaInit
** �������� : ������ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaInit(void)
{
	uint32_t i;
	
	if (sizeof(PARAINFO) > 1024){
		print("para length fail\r\n");
		while(1);
	}
	EepromRead(0, EEPROMADDR_PARAINFO, &ParaInfo, MODE_8_BIT, 1024);
	EepromRead(0, EEPROMADDR_SAVEINFO, &CfgInfo, MODE_8_BIT, sizeof(CFGINFO));
	EepromRead(0, EEPROMADDR_SAVERUNHOUR, &RunHour, MODE_8_BIT, sizeof(RunHour));
	if (ParaInfo.version != 0x5a5a5a02){
		if (ParaInfo.version == 0x5a5a5a01){
			ParaInfo.version = 0x5a5a5a02;
			SaveInfoV2Def();
			EepromWrite(0, EEPROMADDR_PARAINFO, &ParaInfo, MODE_8_BIT, sizeof(PARAINFO));
			EepromErase(EEPROMADDR_SAVEINFO);
			EepromWrite(0, EEPROMADDR_SAVEINFO, &CfgInfo, MODE_8_BIT, sizeof(CFGINFO));
			EepromErase(EEPROMADDR_SAVERUNHOUR);
			EepromWrite(0, EEPROMADDR_SAVERUNHOUR, &RunHour, MODE_8_BIT, sizeof(RunHour));			
		}else{
			ParaDefault(); //Ĭ�ϲ���	
			for (i = 0; i< 16; i++){
				EepromErase(EEPROMADDR_PARAINFO + i);
			}		
			EepromWrite(0, EEPROMADDR_PARAINFO, &ParaInfo, MODE_8_BIT, sizeof(PARAINFO));		
			SaveInfoDefault();
			EepromErase(EEPROMADDR_SAVEINFO);
			EepromWrite(0, EEPROMADDR_SAVEINFO, &CfgInfo, MODE_8_BIT, sizeof(CFGINFO));
			EepromErase(EEPROMADDR_SAVERUNHOUR);
			EepromWrite(0, EEPROMADDR_SAVERUNHOUR, &RunHour, MODE_8_BIT, sizeof(RunHour));			
		}	
	}
	ParaInfo.Port[0].StateInfoReportPeriod = 15;
	InitTrickleInfo();
}

/************************************************************************************************************
** �� �� �� : ParaSave
** �������� : �������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSave(void)
{
	//Ӧ���ӻ������ź���
	EepromWrite(0, EEPROMADDR_PARAINFO, &ParaInfo, MODE_8_BIT, sizeof(PARAINFO));
}

/************************************************************************************************************
** �� �� �� : ParaSetChgGunNumber
** �������� : ���ó�ǹ����
** ��    �� : number ���� 1��2
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetChgGunNumber(uint8_t number)
{
	if ((number != 1) && (number != 2)){
		return;
	}
	
	ParaInfo.ChgGunNumber = number;
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetChgGunNumber
** �������� : ��ȡ��ǹ����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��ǹ���� 1��2
*************************************************************************************************************
*/
uint8_t ParaGetChgGunNumber(void)
{
	return ParaInfo.ChgGunNumber;
}

/************************************************************************************************************
** �� �� �� : ParaSetPileNo
** �������� : ���ó��׮��� 32���ֽ�
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......  data ���׮��� 32���ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetPileNo(uint8_t port, uint8_t *data)
{
	if (data == NULL){
		return;
	}
	memcpy(ParaInfo.Port[port].PileNo, data, 32);
	
	sprintf((char *)ParaInfo.Port[0].QrData, "http://chargepile.tomorn.cn/%sA", data);
	sprintf((char *)ParaInfo.Port[1].QrData, "http://chargepile.tomorn.cn/%sB", data);
	
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetPileNo
** �������� : ��ȡ���׮��� 32���ֽ�
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......  
** ��    �� : ���׮���
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetPileNo(uint8_t port, uint8_t *data)
{
	if (data == NULL){
		return;
	}
	memcpy(data, ParaInfo.Port[port].PileNo, 32);
}

/************************************************************************************************************
** �� �� �� : ParaSetLoginTimeInterval
** �������� : ����ǩ�����ʱ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... TimeInterval ����
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetLoginTimeInterval(uint8_t port, uint16_t TimeInterval)
{
	ParaInfo.Port[port].LoginTimeInterval = TimeInterval;
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetLoginTimeInterval
** �������� : ��ȡǩ�����ʱ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	ʱ�� ����
*************************************************************************************************************
*/
uint16_t ParaGetLoginTimeInterval(uint8_t port)
{
	return ParaInfo.Port[port].LoginTimeInterval;
}

/************************************************************************************************************
** �� �� �� : ParaSetACMeterAddr
** �������� : ���ý�������ַ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   data ����ַ data[0]ΪA0
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetACMeterAddr(uint8_t port, uint8_t *data)
{
	memcpy(ParaInfo.Port[port].ACMeterAddr, data, 6);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetACMeterAddr
** �������� : ��ȡ��������ַ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   data ����ַ data[0]ΪA0
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetACMeterAddr(uint8_t port, uint8_t *data)
{
	memcpy(data, ParaInfo.Port[port].ACMeterAddr, 6);
}

/************************************************************************************************************
** �� �� �� : ParaSetDCMeterAddr
** �������� : ����ֱ������ַ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   data ����ַ data[0]ΪA0
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetDCMeterAddr(uint8_t port, uint8_t *data)
{
	memcpy(ParaInfo.Port[port].DCMeterAddr, data, 6);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetDCMeterAddr
** �������� : ��ȡֱ������ַ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   data ����ַ data[0]ΪA0
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetDCMeterAddr(uint8_t port, uint8_t *data)
{
	memcpy(data, ParaInfo.Port[port].DCMeterAddr, 6);
}

/************************************************************************************************************
** �� �� �� : ParaSetNetHeartTime
** �������� : ������������ʱ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   time ��λ S
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetNetHeartTime(uint8_t port, uint16_t time)
{
	if (time == 0){
		return;
	}
	ParaInfo.Port[port].NetHeartTime=time;
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetNetHeartTime
** �������� : ��ȡ��������ʱ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   cnt �ط�����
** ��    �� : ��
** ��    �� :	����ʱ��  ��λ S
*************************************************************************************************************
*/
uint16_t ParaGetNetHeartTime(uint8_t port)
{
	return ParaInfo.Port[port].NetHeartTime;
}

/************************************************************************************************************
** �� �� �� : ParaSetNetCommOverCnt
** �������� : ��������ͨѶ��ʱ�ط����� 
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   cnt �ط�����
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetNetCommOverCnt(uint8_t port, uint8_t cnt)
{
	if (cnt == 0) {
		return;
	}
	ParaInfo.Port[port].NetCommOverCnt=cnt;
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetNetCommOverCnt
** �������� : ��ȡ����ͨѶ��ʱ�ط�����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   cnt �ط�����
** ��    �� : ��
** ��    �� :	�ط�����
*************************************************************************************************************
*/
uint8_t ParaGetNetCommOverCnt(uint8_t port)
{
	return ParaInfo.Port[port].NetCommOverCnt;
}

/************************************************************************************************************
** �� �� �� : ParaSetUpChannel
** �������� : ��������ͨ��
** ��    �� : channel 0��̫�� 1GPRS
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetUpChannel(uint8_t channel)
{
	ParaInfo.UpChannel = channel;
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetUpChannel
** �������� : ��ȡ����ͨ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	0��̫�� 1GPRS
*************************************************************************************************************
*/
uint8_t ParaGetUpChannel(void)
{
	return ParaInfo.UpChannel;
}

/************************************************************************************************************
** �� �� �� : ParaSetLocalIp
** �������� : ���ñ���IP 4���ֽ�
** ��    �� : IP
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetLocalIp(uint8_t *data)
{
	if (data == NULL){
		return;
	}

	memcpy(ParaInfo.LocalIp,data,4);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetLocalIp
** �������� : ��ȡ����IP 4���ֽ�
** ��    �� : IP
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetLocalIp(uint8_t *data)
{
	if (data == NULL){
		return;
	}
	
	memcpy(data, ParaInfo.LocalIp, 4);
}

/************************************************************************************************************
** �� �� �� : ParaSetSubMask
** �������� : ������������  4���ֽ�
** ��    �� : ��������
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetSubMask(uint8_t *data)
{
	if (data == NULL){
		return;
	}
	
	memcpy(ParaInfo.SubMask,data,4);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetSubMask
** �������� : ��ȡ�������� 4���ֽ�
** ��    �� : ����ָ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetSubMask(uint8_t *data)
{
	if (data == NULL){
		return;
	}
	
	memcpy(data, ParaInfo.SubMask, 4);
}

/************************************************************************************************************
** �� �� �� : ParaSetGateWay
** �������� : ��������  4���ֽ�
** ��    �� : ����ָ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetGateWay(uint8_t *data)
{
	if (data == NULL){
		return;
	}

	memcpy(ParaInfo.GateWay,data,4);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetGateWay
** �������� : ��ȡ���� 4���ֽ�
** ��    �� : ����ָ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetGateWay(uint8_t *data)
{
	if (data == NULL){
		return;
	}
	
	memcpy(data,ParaInfo.GateWay,4);
}

/************************************************************************************************************
** �� �� �� : ParaSetPhyMac
** �������� : ��������MAC  6���ֽ�
** ��    �� : ����MAC
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetPhyMac(uint8_t *data)
{
	if (data == NULL){
		return;
	}

	memcpy(ParaInfo.PhyMac,data,6);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetPhyMac
** �������� : ��ȡ����MAC 6���ֽ�
** ��    �� : ����ָ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetPhyMac(uint8_t *data)
{
	if (data == NULL){
		return;
	}
	
	memcpy(data, ParaInfo.PhyMac, 6);
}

/************************************************************************************************************
** �� �� �� : ParaSetServerIp
** �������� : ���÷�����IP
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   data ������IP
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetServerIp(uint8_t port, uint8_t *data)
{
	if (data == NULL){
		return;
	}
	memcpy(ParaInfo.Port[0].ServerIp, data, 4);
	memcpy(ParaInfo.Port[1].ServerIp, data, 4);
	ParaSave();
	DeleteAllRecord();
}

/************************************************************************************************************
** �� �� �� : ParaGetServerIp
** �������� : ��ȡ������IP
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   data ������IP
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetServerIp(uint8_t port, uint8_t *data)
{
	if (data == NULL){
		return;
	}
	memcpy(data, ParaInfo.Port[port].ServerIp, 4);
}

/************************************************************************************************************
** �� �� �� : ParaSetServerPort
** �������� : ���÷������˿�
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   port �������˿�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetServerPort(uint8_t port, uint16_t serverport)
{
	if (port >= 2){
		return;
	}
	ParaInfo.Port[0].ServerPort = serverport;
	ParaInfo.Port[1].ServerPort = serverport;
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetServerPort
** �������� : ��ȡ�������˿�
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   
** ��    �� : ��
** ��    �� :	�������˿�
*************************************************************************************************************
*/
uint16_t ParaGetServerPort(uint8_t port)
{
	if (port >= 2){
		return 0;
	}
	return ParaInfo.Port[port].ServerPort;
}

/************************************************************************************************************
** �� �� �� : ParaSetFeilvTime
** �������� : ���üƷѲ��Ե�ʱ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   
							period 12��ʱ�� 0-11
							starthour ��ʼСʱ 0-24
							startmin ��ʼ���� 0��30
							stophour ����Сʱ 0-24
							stopmin �������� 0��30
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetFeilvTime(uint8_t port, uint8_t period, uint8_t starthour, uint8_t startmin, uint8_t stophour, uint8_t stopmin)
{
	if ((port >= 2) || (period >= 12)){
		return;
	}
	ParaInfo.Port[port].StartHour[period] = starthour;
	ParaInfo.Port[port].StartMin[period] = startmin;
	ParaInfo.Port[port].StopHour[period] = stophour;
	ParaInfo.Port[port].StopMin[period] = stopmin;
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetFeilvTime
** �������� : ��ȡ�ƷѲ��Ե�ʱ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......    period 12��ʱ�� 0-11
** ��    �� : starthour ��ʼСʱ 0-24
							startmin ��ʼ���� 0��30
							stophour ����Сʱ 0-24
							stopmin �������� 0��30
** ��    �� :	�������˿�
*************************************************************************************************************
*/
void ParaGetFeilvTime(uint8_t port, uint8_t period, uint8_t *starthour, uint8_t *startmin, uint8_t *stophour, uint8_t *stopmin)
{
	if ((port >= 2) || (period >= 12)){
		return;
	}
	*starthour = ParaInfo.Port[port].StartHour[period];
	*startmin = ParaInfo.Port[port].StartMin[period];
	*stophour = ParaInfo.Port[port].StopHour[period];
	*stopmin = ParaInfo.Port[port].StopMin[period];
}

/************************************************************************************************************
** �� �� �� : ParaSetFeilvMoney
** �������� : ���üƷѲ��Եĵ���
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   
							period 12��ʱ�� 0-11
							money ���� ����5λС��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetFeilvMoney(uint8_t port, uint8_t period, uint32_t money)
{
	if ((port >= 2) || (period >= 12)){
		return;
	}
	ParaInfo.Port[port].Money[period] = money;
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetFeilvMoney
** �������� : ��ȡ�ƷѲ��Եĵ���
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......    period 12��ʱ�� 0-11
** ��    �� : ��
** ��    �� :	money ���� ����5λС��
*************************************************************************************************************
*/
uint32_t ParaGetFeilvMoney(uint8_t port, uint8_t period)
{
	if ((port >= 2) || (period >= 12)){
		return 0;
	}
	return ParaInfo.Port[port].Money[period];
}

/************************************************************************************************************
** �� �� �� : ParaSetStateInfoReportPeriod
** �������� : ����״̬��Ϣ�ϱ�����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   time ��λ S
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetStateInfoReportPeriod(uint8_t port, uint16_t time)
{
	if (time == 0){
		return;
	}
	ParaInfo.Port[port].StateInfoReportPeriod = time;
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetStateInfoReportPeriod
** �������� : ��ȡ״̬��Ϣ�ϱ�����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	����ʱ��  ��λ S
*************************************************************************************************************
*/
uint16_t ParaGetStateInfoReportPeriod(uint8_t port)
{
	return ParaInfo.Port[port].StateInfoReportPeriod;
}

/************************************************************************************************************
** �� �� �� : ParaSetQRCode
** �������� : ���ö�ά��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   qrcode ��ά�룬�256���ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetQRCode(uint8_t port, uint8_t *qrcode)
{
	memcpy(ParaInfo.Port[port].QrData, qrcode, 256);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetQRCode
** �������� : ��ȡ��ά��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... qrcode ��ά�룬�256���ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetQRCode(uint8_t port, uint8_t *qrcode)
{
	memcpy(qrcode, ParaInfo.Port[port].QrData, 256);
}

/************************************************************************************************************
** �� �� �� : ParaSetHardVersion
** �������� : ����Ӳ���汾��
** ��    �� : data �汾�� ASCII 16�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetHardVersion(uint8_t *data)
{
	memcpy(ParaInfo.HardVersion, data, 16);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetHardVersion
** �������� : ��ȡӲ���汾��
** ��    �� : data �汾�� ASCII 16�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetHardVersion(uint8_t *data)
{
	memcpy(data, ParaInfo.HardVersion, 16);
}

/************************************************************************************************************
** �� �� �� : ParaSetHardVersion
** �������� : ���ÿͻ����
** ��    �� : data �ͻ���� 6�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetCustomerNumber(uint8_t *data)
{
	memcpy(ParaInfo.CustomerNumber, data, 4);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetCustomerNumber
** �������� : ��ȡ�ͻ����
** ��    �� : data �ͻ���� 6�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetCustomerNumber(uint8_t *data)
{
	memcpy(data, ParaInfo.CustomerNumber, 4);
}

/************************************************************************************************************
** �� �� �� : ParaSetAPN
** �������� : ����APN
** ��    �� : data �ͻ���� 16�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetAPN(uint8_t *data)
{
	memcpy(ParaInfo.APN, data, 16);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetAPN
** �������� : ��ȡAPN
** ��    �� : data 6�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetAPN(uint8_t *data)
{
	memcpy(data, ParaInfo.APN, 16);
}

/************************************************************************************************************
** �� �� �� : ParaSetAPNUser
** �������� : ����APN�û���
** ��    �� : data 32�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetAPNUser(uint8_t *data)
{
	memcpy(ParaInfo.APNUser, data, 32);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetAPNUser
** �������� : ��ȡAPN�û���
** ��    �� : data 6�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetAPNUser(uint8_t *data)
{
	memcpy(data, ParaInfo.APNUser, 32);
}

/************************************************************************************************************
** �� �� �� : ParaSetAPNPasswd
** �������� : ����APN����
** ��    �� : data 32�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetAPNPasswd(uint8_t *data)
{
	memcpy(ParaInfo.APNPasswd, data, 32);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetAPNPasswd
** �������� : ��ȡAPN����
** ��    �� : data 6�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetAPNPasswd(uint8_t *data)
{
	memcpy(data, ParaInfo.APNPasswd, 32);
}

/************************************************************************************************************
** �� �� �� : ParaSetFeilvTime
** �������� : ���üƷѲ��Ե�ʱ��
** ��    �� : openhour ����Сʱ 0-24
							openmin �������� 0��30
							closehour �ر�Сʱ 0-24
							closemin �رշ��� 0��30
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetScreenTime(uint8_t openhour, uint8_t openmin, uint8_t closehour, uint8_t closemin)
{
	ParaInfo.ScreenOpenHour = openhour;
	ParaInfo.ScreenOpenMin = openmin;
	ParaInfo.ScreenStopHour = closehour;
	ParaInfo.ScreenStopMin = closemin;
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetScreenTime
** �������� : ��ȡ����������ر�ʱ��
** ��    �� : ��
** ��    �� : openhour ����Сʱ 0-24
							openmin �������� 0��30
							closehour �ر�Сʱ 0-24
							closemin �رշ��� 0��30
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetScreenTime(uint8_t *openhour, uint8_t *openmin, uint8_t *closehour, uint8_t *closemin)
{
	*openhour = ParaInfo.ScreenOpenHour;
	*openmin = ParaInfo.ScreenOpenMin;
	*closehour = ParaInfo.ScreenStopHour;
	*closemin = ParaInfo.ScreenStopMin;
}

/************************************************************************************************************
** �� �� �� : ParaSetManufactureDate
** �������� : ���ó�������
** ��    �� : Date ���� 4�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetManufactureDate(uint8_t *Date)
{
	memcpy(ParaInfo.ManufactureDate, Date, 4);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetManufactureDate
** �������� : ��ȡ��������
** ��    �� : ��
** ��    �� : Date ���� 4�ֽ�
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetManufactureDate(uint8_t *Date)
{
	memcpy(Date, ParaInfo.ManufactureDate, 4);
}

/************************************************************************************************************
** �� �� �� : ParaSetManufactureBase
** �������� : ������������
** ��    �� : Base ���� 16�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetManufactureBase(uint8_t *Base)
{
	memcpy(ParaInfo.ManufactureBase, Base, 16);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetManufactureBase
** �������� : ��ȡ��������
** ��    �� : ��
** ��    �� : Base ���� 16�ֽ�
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetManufactureBase(uint8_t *Base)
{
	memcpy(Base, ParaInfo.ManufactureBase, 16);
}

/************************************************************************************************************
** �� �� �� : ParaSetTestWorker
** �������� : ���ü�����
** ��    �� : TestWorker ������ 8�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetTestWorker(uint8_t *TestWorker)
{
	memcpy(ParaInfo.TestWorker, TestWorker, 8);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetManufactureBase
** �������� : ��ȡ������
** ��    �� : ��
** ��    �� : TestWorker ������ 8�ֽ�
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetTestWorker(uint8_t *TestWorker)
{
	memcpy(TestWorker, ParaInfo.TestWorker, 8);
}

/************************************************************************************************************
** �� �� �� : ParaSetLifeTime
** �������� : ����ʹ������
** ��    �� : LifeTime ʹ������ 5�ֽ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSetLifeTime(uint8_t *LifeTime)
{
	memcpy(ParaInfo.LifeTime, LifeTime, 5);
	ParaSave();
}

/************************************************************************************************************
** �� �� �� : ParaGetLifeTime
** �������� : ��ȡʹ������
** ��    �� : ��
** ��    �� : LifeTime ʹ������ 5�ֽ�
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetLifeTime(uint8_t *LifeTime)
{
	memcpy(LifeTime, ParaInfo.LifeTime, 5);
}


/************************************************************************************************************
** �� �� �� : ParaGetLimitHour
** �������� : ��ȡ��������ʹ�õ�ʱ������λ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	���õ�ʱ������λ��Сʱ
*************************************************************************************************************
*/
uint32_t ParaGetLimitHour(void)
{
	return CfgInfo.LimitHour;
}



/************************************************************************************************************
** �� �� �� : ParaSetActCode
** �������� : ���ü����룬��������ȷ���ɹ�д�뷵��0  ���򷵻�1
** ��    �� : buf
** ��    �� : ��
** ��    �� :	1 д�����   0 д����ȷ
*************************************************************************************************************
*/
uint8_t ParaSetActCode(uint8_t * data)
{
	uint8_t Getbuf[32],str[32], oldpass[4], newpass[4], daystr[5];
	uint8_t *pstr = NULL;
	
	memcpy(Getbuf, data, 32);	
	if(strlen((const char *)Getbuf) != 26)
		return 1;
	memset(str, 0, 32);	
	memset(daystr, '\0', sizeof(daystr));	
	Decryption(Getbuf, strlen((const char *)Getbuf), str);		//����
	pstr = str;
	memcpy(newpass, pstr, 4);
	memcpy(oldpass, pstr+4, 4);
	memcpy(daystr, pstr+8, 4);
	if(memcmp(oldpass, CfgInfo.ActPassword, 4))
		return 1;
	memcpy(CfgInfo.ActPassword, newpass, 4);// ��new  д��  old
	memcpy(CfgInfo.ActCode, data, 32); //д�뼤����
	CfgInfo.LimitHour =  atoi((const char *)daystr)* 24;	
	RunHour = 0;
	EepromWrite(0, EEPROMADDR_SAVERUNHOUR, &RunHour, MODE_8_BIT, sizeof(RunHour));
	EepromWrite(0, EEPROMADDR_SAVEINFO, &CfgInfo, MODE_8_BIT, sizeof(CFGINFO));
	return 0;
}


/************************************************************************************************************
** �� �� �� : ParaGetActCode
** �������� : ��ȡ������
** ��    �� : ��
** ��    �� : 32�ֽڼ�����
** ��    �� :	��
*************************************************************************************************************
*/
void ParaGetActCode(uint8_t *ActCode)
{
	memcpy(ActCode, CfgInfo.ActCode, 32);
}

/************************************************************************************************************
** �� �� �� : ParaGetRunHour
** �������� : ��ȡ�豸������ʹ�����������ڵ�����ʱ��  ��λ��Сʱ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��ȷ����1 ���󷵻�0
*************************************************************************************************************
*/
uint32_t ParaGetRunHour(void)
{
	return RunHour;
}


/************************************************************************************************************
** �� �� �� : ParaSavaRunHour
** �������� : �洢����ʱ�����
** ��    �� : ����ʱ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ParaSavaRunHour(uint32_t  runhour)
{
	RunHour = runhour;
	EepromWrite(0, EEPROMADDR_SAVERUNHOUR, &RunHour, MODE_8_BIT, sizeof(RunHour));
}







/************************************************************************************************************
** �� �� �� : ConfigProtocolFrameCheck
** �������� : ����Э��֡��麯��
** ��    �� : buf���ݻ������׵�ַ  Channel Э��ͨ��
** ��    �� : ��
** ��    �� :	��ȷ����1 ���󷵻�0
*************************************************************************************************************
*/
uint8_t ConfigProtocolFrameCheck(uint8_t *buf, uint8_t Channel)
{
	uint8_t cs,i;
	uint8_t PileNumber[32]={0};
	if(Channel == PARACHANNEL_UART){
		
		if (buf[0] != 0x68){
			return 0;
		}
		if (buf[1] & 0x80){
			return 0;
		}
		cs = 0;
		for (i =0; i < buf[2] + 3; i++){
			cs += buf[i];
		}
		if (cs != buf[buf[2] + 3]){
			return 0;
		}
		if(buf[buf[2] + 4] != 0x16){
			return 0;
		}
		return 1;
		
	}else if(Channel == PARACHANNEL_UDP){
		if (buf[0] != 0x68){
			return 0;
		}
		
		memset(PileNumber, '0', 32);
		if( memcmp( &buf[1], PileNumber, 32) ){  //�ж�׮���
				ParaGetPileNo(0, PileNumber);
				if( memcmp( &buf[1], PileNumber, 32)){
					return 0;
				}
		}
		if (buf[33] != 0x68){
			return 0;
		}
		if (buf[1 + 33] & 0x80){
			return 0;
		}
		cs = 0;
		for (i =0; i < buf[2 + 33] + 3; i++){
			cs += buf[i+33];
		}
		if (cs != buf[33 + buf[2 + 33] + 3]){
			return 0;
		}
		if(buf[33 + buf[2 + 33] + 4] != 0x16){
			return 0;
		}
		return 1;
		
	}else{
		return 0;
	}

}




/************************************************************************************************************
** �� �� �� : DebugPlatformReadDataAnalyse
** �������� : ����ƽ̨�����ݷ���
** ��    �� : buf���ݻ������׵�ַ  Channel������ OR UDP
** ��    �� : ��
** ��    �� :	��ȷ����1 ���󷵻�0
*************************************************************************************************************
*/
uint8_t DebugPlatformReadDataAnalyse(uint8_t *pbuf, uint8_t Channel)
{
	uint16_t DI;
	uint8_t ret = 0x02,databuf[257],datalen,ackbuffer[256],cs,i,*p,openhour,openmin,closehour,closemin;
	uint8_t data8;
	uint16_t data16;
	uint32_t data32;
	Rtc datetime;
	uint32_t TimerSend;
	
	memset(databuf, 0, sizeof(databuf));
	datalen = 0;
	if(Channel == PARACHANNEL_UDP)
		pbuf +=33;
	
	DI = *(pbuf + 3) | (*(pbuf + 4) << 8);
	switch (DI){
		case 0x0001://���׮���
			databuf[0] = *(pbuf + 5);
			if (*(pbuf + 5) == 1){
				ParaGetPileNo(0, databuf + 1);
			} else if (*(pbuf + 5) == 2){
				ParaGetPileNo(1, databuf + 1);
			}
			datalen = 33;
			ret = 0;
			break;
			
		case 0x0002://����汾��
			p = databuf;
			data8 = _VERSION_MAJOR;
			if (data8 >= 100){
				*p++ = (data8 / 100) | 0x30;
				data8 %= 100;
				*p++ = (data8 / 10) | 0x30;
				*p++ = (data8 % 10) | 0x30;
			} else if (data8 >= 10){
				*p++ = (data8 / 10) | 0x30;
				*p++ = (data8 % 10) | 0x30;
			} else {
				*p++ = data8 | 0x30;
			}
			*p++ = '.';
			data8 = _VERSION_MINOR;
			if (data8 >= 100){
				*p++ = (data8 / 100) | 0x30;
				data8 %= 100;
				*p++ = (data8 / 10) | 0x30;
				*p++ = (data8 % 10) | 0x30;
			} else if (data8 >= 10){
				*p++ = (data8 / 10) | 0x30;
				*p++ = (data8 % 10) | 0x30;
			} else {
				*p++ = data8 | 0x30;
			}
			*p++ = '.';
			data8 = _VERSION_PATCH;
			if (data8 >= 100){
				*p++ = (data8 / 100) | 0x30;
				data8 %= 100;
				*p++ = (data8 / 10) | 0x30;
				*p++ = (data8 % 10) | 0x30;
			} else if (data8 >= 10){
				*p++ = (data8 / 10) | 0x30;
				*p++ = (data8 % 10) | 0x30;
			} else {
				*p++ = data8 | 0x30;
			}
			*p++ = '\0';
			datalen = 16;
			ret = 0;
			break;
		
		case 0x0003://Ӳ���汾
			ParaGetHardVersion(databuf);
			datalen = 16;
			ret = 0;
			break;
		
		case 0x0004://���׮����
			databuf[0] = ParaGetChgGunNumber();
			datalen = 1;
			ret = 0;
			break;
		
		case 0x0007://���׮��ǰʱ��
			RtcRead(&datetime);
			databuf[6] = datetime.year >> 8;
			databuf[5] = datetime.year;
			databuf[4] = datetime.month;
			databuf[3] = datetime.day;
			databuf[2] = datetime.hour;
			databuf[1] = datetime.min;
			databuf[0] = datetime.sec;
			datalen = 7;
			ret = 0;
			break;
		
		case 0x0008://��������
			databuf[0] = ParaGetNetHeartTime(0);
			datalen = 1;
			ret = 0;
			break;
		
		case 0x0009://���������ʱ��
			ParaGetScreenTime(databuf + 1, databuf, &closehour, &closemin);
			datalen = 2;
			ret = 0;
			break;
		
		case 0x000A://������ر�ʱ��
			ParaGetScreenTime(&openhour, &openmin, databuf + 1, databuf);
			datalen = 2;
			ret = 0;
			break;
		
		case 0x000B://��������ַ
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				databuf[0] = *(pbuf + 5);
				ParaGetACMeterAddr(*(pbuf + 5) - 1, databuf + 1);
				datalen = 7;
			}
			ret = 0;
			break;
			
		case 0x000C://ֱ������ַ
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				databuf[0] = *(pbuf + 5);
				ParaGetDCMeterAddr(*(pbuf + 5) - 1, databuf + 1);
				datalen = 7;
			}
			ret = 0;
			break;
			
		case 0x000D://�ͻ����
			ParaGetCustomerNumber(databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x000F://����������
			memcpy(databuf, "0000000000000001", 16);
			datalen = 16;
			ret = 0;
			break;
		
		case 0x0011://��ά��
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				databuf[0] = *(pbuf + 5);
				ParaGetQRCode(*(pbuf + 5) - 1, databuf + 1);
				datalen = 129;
				ret = 0;
			}
			break;
		
		case 0x0101://����ͨ��
			databuf[0] = ParaGetUpChannel();
			datalen = 1;
			ret = 0;
			break;
		
		case 0x0102://������IP
			ParaGetServerIp(0, databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x0103://�������˿�
			data16 = ParaGetServerPort(0);
			memcpy(databuf, &data16, 2);
			datalen = 2;
			ret = 0;
			break;	
		
		case 0x0104://����IP
			ParaGetLocalIp(databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x0105://��������
			ParaGetSubMask(databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x0106://����
			ParaGetGateWay(databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x0107://�����ַ
			ParaGetPhyMac(databuf);
			datalen = 6;
			ret = 0;
			break;
		
		case 0x0109://APN
			ParaGetAPN(databuf);
			datalen = 16;
			ret = 0;
			break;
		
		case 0x010A://APN User
			ParaGetAPNUser(databuf);
			datalen = 32;
			ret = 0;
			break;
		
		case 0x010B://APN Passwd
			ParaGetAPNPasswd(databuf);
			datalen = 32;
			ret = 0;
			break;
		
		case 0x0201://���ƽ�ȷ���
			for (i = 0; i < 12; i++){
				ParaGetFeilvTime(0, i, databuf + i * 8 + 1, databuf + i * 8 + 0, databuf + i * 8 + 3, databuf + i * 8 + 2);
				data32 = ParaGetFeilvMoney(0, i);
				memcpy(databuf + i * 8 + 4, &data32, 4);
			}
			datalen = 96;
			ret = 0;
			break;
			
		case 0x0012://��������
			ParaGetManufactureDate(databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x0013://��������
			ParaGetManufactureBase(databuf);
			datalen = 16;
			ret = 0;
			break;
		
		case 0x0014://�����
			ParaGetTestWorker(databuf);
			datalen = 8;
			ret = 0;
			break;
		
		case 0x0015://ʹ����
			ParaGetLifeTime(databuf);
			datalen = 5;
			ret = 0;
			break;
		case 0x0016://������
			ParaGetActCode(databuf);
			datalen = 32;
			ret = 0;
			break;
		case 0x0017:// �����������	
			databuf[0] = (GetTrickleCur() & 0xFF);
			databuf[1] = (GetTrickleCur() >> 8);
			datalen = 2;
			ret = 0;
			break;
		
		case 0x0018:// ���SOC����		
			databuf[0] = GetTrickleSoc();
			datalen = 1;
			ret = 0;
			break;	
		
		case 0x00D1:  //ģ������
			GetParacmd = 1;
			ParaResult = 0;
			TimerSend =TimerRead();
			while(((TimerRead()-TimerSend) < T1S*4 )&& (ParaResult ==0 )){
				OSTimeDlyHMSM (0, 0, 0, 10);
			}
			if(ParaResult){
				ret = 0;
				databuf[0] =  ReadModuletype();
				datalen = 1;
			}else{
				ret = 1;
			}			
			break;
		case 0x00D2:  //ģ������
			GetParacmd = 2 + ((*(pbuf + 5) - 1)<< 7);
			ParaResult = 0;
			TimerSend =TimerRead();
			while(((TimerRead()-TimerSend) < T1S*4 )&& (ParaResult ==0 )){
				OSTimeDlyHMSM (0, 0, 0, 10);
			}
			if(ParaResult){
				ret = 0;
				databuf[0] = *(pbuf + 5);
				databuf[1] = ReadMoudlenum();
				datalen = 2;
			}else{
				ret = 1;
			}			
			break;
		case 0x00D4:  //����ֵ
			GetParacmd = 3 + ((*(pbuf + 5) - 1)<< 7);
			ParaResult = 0;
			TimerSend =TimerRead();
			while(((TimerRead()-TimerSend) < T1S*4 )&& (ParaResult ==0 )){
				OSTimeDlyHMSM (0, 0, 0, 10);
			}
			if(ParaResult){
				ret = 0;
				databuf[0] = *(pbuf + 5);
				data16 = ReadMaxcurrent();
				memcpy((databuf+1), &data16, 2);
				datalen = 3;
			}else{
				ret = 1;
			}			
			break;
		case 0x00D6:  //���ʷ���ģʽ
			GetParacmd = 4;
			ParaResult = 0;
			TimerSend =TimerRead();
			while(((TimerRead()-TimerSend) < T1S*4 )&& (ParaResult ==0 )){
				OSTimeDlyHMSM (0, 0, 0, 10);
			}
			if(ParaResult){
				ret = 0;
				databuf[0] = ReadPowerMod();
				datalen = 1;
			}else{
				ret = 1;
			}			
			break;
		default:
			break;
	}
	
	if (ret == 0){

		if(Channel == PARACHANNEL_UART){		
			p = ackbuffer;
			*p++ = 0x68;
			*p++ = 0x81;
			*p++ = datalen + 2;
			*p++ = *(pbuf + 3);
			*p++ = *(pbuf + 4);
			memcpy(p, databuf, datalen);
			p += datalen;
			cs = 0;
			for (i = 0; i < datalen + 5; i++){
				cs += ackbuffer[i];
			}
			*p++ = cs;
			*p++ = 0x16;
			UartWrite(DEBUGUARTNO, ackbuffer, p - ackbuffer);
		}else if(Channel == PARACHANNEL_UDP){
			p = ackbuffer;
			*p++ = 0x68;
			ParaGetPileNo(0, p);  //׮���
			p += 32;
			
			*p++ = 0x68;
			*p++ = 0x81;
			*p++ = datalen + 2;
			*p++ = *(pbuf + 3);
			*p++ = *(pbuf + 4);
			memcpy(p, databuf, datalen);
			p += datalen;
			cs = 0;
			for (i = 0; i < datalen + 5; i++){
				cs += ackbuffer[i+33];
			}
			*p++ = cs;
			*p++ = 0x16;
			UDPSend( ackbuffer, p - ackbuffer);
		}
	} else {
		if(Channel == PARACHANNEL_UART){
			p = ackbuffer;
			*p++ = 0x68;
			*p++ = 0xC1;
			*p++ = 0x01;
			*p++ = ret;
			cs = 0;
			for (i = 0; i < 4; i++){
				cs += ackbuffer[i];
			}
			*p++ = cs;
			*p++ = 0x16;
			UartWrite(DEBUGUARTNO, ackbuffer, p - ackbuffer);
		}else if(Channel == PARACHANNEL_UDP){
			p = ackbuffer;
			*p++ = 0x68;
			ParaGetPileNo(0, p);  //׮���
			p += 32;
			
			*p++ = 0x68;
			*p++ = 0xC1;
			*p++ = 0x01;
			*p++ = ret;
			cs = 0;
			for (i = 0; i < 4; i++){
				cs += ackbuffer[i+33];
			}
			*p++ = cs;
			*p++ = 0x16;
			UDPSend( ackbuffer, p - ackbuffer);
		}
	}
	
	return 1;
}





/************************************************************************************************************
** �� �� �� : SetParaResult
** �������� : ���óɹ���־
** ��    �� : Result ���ý��  1�ɹ�  0  ʧ��
** ��    �� : ��
** ��    �� :	
*************************************************************************************************************
*/
void SetParaResult(uint8_t Result)
{
	ParaResult = Result; 
}


/************************************************************************************************************
** �� �� �� : SetParaCmd
** �������� : ��ȡ���������������
** ��    �� : port  ���ǹ���
** ��    �� : ��
** ��    �� :	
*************************************************************************************************************
*/
uint8_t SetParaCmd(void)
{
	return SetParacmd;
}


/************************************************************************************************************
** �� �� �� : GetParaCmd
** �������� : ��ȡ���������������
** ��    �� : port  ���ǹ���
** ��    �� : ��
** ��    �� :	
*************************************************************************************************************
*/
uint8_t GetParaCmd(void)
{
	return GetParacmd;
}



/************************************************************************************************************
** �� �� �� : ClearParaCmd
** �������� : ������������������
** ��    �� : 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ClearParaCmd(void)
{
	 SetParacmd = 0x00;
	 GetParacmd = 0x00;
}

/************************************************************************************************************
** �� �� �� : BoardGetModuleType
** �������� : ��ȡ�����·�ģ������
** ��    �� :	
** ��    �� :	��
** ��    �� :	 ModuleType ģ������ 
*************************************************************************************************************
*/
uint8_t BoardGetModuleType(void)
{
	return ModuleType;
}


/************************************************************************************************************
** �� �� �� : BoardGetMoudleNum
** �������� : ��ȡ�����·�ģ������
** ��    �� : 
** ��    �� : ��
** ��    �� :	��ȷ����1 ���󷵻�0
*************************************************************************************************************
*/
uint8_t BoardGetMoudleNum(void)
{
	return MoudleNum;
}


/************************************************************************************************************
** �� �� �� : BoardGetMaxCurrent
** �������� : ��ȡ�����·�������
** ��    �� : 
** ��    �� : ��
** ��    �� :	
*************************************************************************************************************
*/
uint16_t BoardGetMaxCurrent(void)
{
	return MaxCurrent;
}
/************************************************************************************************************
** �� �� �� : BoardGetPowerMode
** �������� : ��ȡ�����·��Ĺ��ʷ���ģʽ
** ��    �� : 
** ��    �� : ��
** ��    �� :	
*************************************************************************************************************
*/
uint8_t BoardGetPowerMode(void)
{
	return PowerMode;
}


/************************************************************************************************************
** �� �� �� : DebugPlatformWriteDataAnalyse
** �������� : ����ƽ̨д���ݷ���
** ��    �� : buf���ݻ������׵�ַ  Channel:���� OR UDP
** ��    �� : ��
** ��    �� :	��ȷ����1 ���󷵻�0
*************************************************************************************************************
*/
uint8_t DebugPlatformWriteDataAnalyse(uint8_t *pbuf, uint8_t Channel)
{
	uint16_t DI;
	uint8_t ret = 0x02,databuf[128],ackbuffer[256],cs,i,*p,openhour,openmin,closehour,closemin;
	uint32_t data32;
	Rtc datetime;
	uint32_t TimerSend;
	uint8_t PileNo[32];
	ParaGetPileNo(0, PileNo);
	memset(databuf, 0, sizeof(databuf));
	if(Channel == PARACHANNEL_UDP)
		pbuf +=33;
	
	DI = *(pbuf + 3) | (*(pbuf + 4) << 8);
	switch (DI){
		case 0x0001://���׮���
			if (*(pbuf + 5) == 1){
				ParaSetPileNo(0, pbuf + 5 + 1);
				ParaSetPileNo(1, pbuf + 5 + 1);
				ret = 0;
			} else if (*(pbuf + 5) == 2){
				ParaSetPileNo(1, pbuf + 5 + 1);
				ret = 0;
			}
			break;
			
		case 0x0003://Ӳ���汾
			ParaSetHardVersion(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0004://���׮����
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				ParaSetChgGunNumber(*(pbuf + 5));
				ret = 0;
			}
			break;
		
		case 0x0007://���׮��ǰʱ��
			datetime.year = *(pbuf + 10) | ( *(pbuf + 11)<< 8);
			datetime.month = *(pbuf + 9);
			datetime.day = *(pbuf + 8);
			datetime.hour = *(pbuf + 7);
			datetime.min = *(pbuf + 6);
			datetime.sec = *(pbuf + 5);
			RtcWrite(&datetime);
			ret = 0;
			break;
		
		case 0x0008://��������
			ParaSetNetHeartTime(0, *(pbuf + 5));
			ParaSetNetHeartTime(1, *(pbuf + 5));
			ret = 0;
			break;
		
		case 0x0009://���������ʱ��
			ParaGetScreenTime(&openhour, &openmin, &closehour, &closemin);
			ParaSetScreenTime(*(pbuf + 6), *(pbuf + 5), closehour, closemin);
			ret = 0;
			break;
		
		case 0x000A://������ر�ʱ��
			ParaGetScreenTime(&openhour, &openmin, &closehour, &closemin);
			ParaSetScreenTime(openhour, openmin, *(pbuf + 6), *(pbuf + 5));
			ret = 0;
			break;
		
		case 0x000B://��������ַ
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				ParaSetACMeterAddr(*(pbuf + 5) - 1, pbuf + 5 + 1);
				ret = 0;
			}
			break;
			
		case 0x000C://ֱ������ַ
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				ParaSetDCMeterAddr(*(pbuf + 5) - 1, pbuf + 5 + 1);
				ret = 0;
			}
			break;
			
		case 0x000D://�ͻ����
			ParaSetCustomerNumber(pbuf + 5);
			ret = 0;
			break;
			
		case 0x000E://�ն˸�λ
			if ((*(pbuf + 5) == 0) || (*(pbuf + 5) == 1)){
				//��������λ
				DeleteAllRecord();
			}
			ret = 0;
			break;
		
		case 0x0010://ע����
			ret = 0;
			break;
		
		case 0x0011://��ά��
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				ParaSetQRCode(*(pbuf + 5) - 1, pbuf + 5 + 1);
				ret = 0;
			}
			break;
		
		case 0x0101://����ͨ��
			ParaSetUpChannel(*(pbuf + 5));
			ret = 0;
			break;
		
		case 0x0102://������IP
			ParaSetServerIp(0, pbuf + 5);
			ret = 0;
			break;
		
		case 0x0103://�������˿�
			ParaSetServerPort(0, *(pbuf + 5) | (*(pbuf + 6) << 8));
			ParaSetServerPort(1, *(pbuf + 5) | (*(pbuf + 6) << 8));
			ret = 0;
			break;
		
		case 0x0104://����IP
			ParaSetLocalIp(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0105://��������
			ParaSetSubMask(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0106://����
			ParaSetGateWay(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0107://�����ַ
			ParaSetPhyMac(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0109://APN
			ParaSetAPN(pbuf + 5);
			ret = 0;
			break;
		
		case 0x010A://APN User
			ParaSetAPNUser(pbuf + 5);
			ret = 0;
			break;
		
		case 0x010B://APN Passwd
			ParaSetAPNPasswd(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0201://���ƽ�ȷ���
			for (i = 0; i < 12; i++){
				ParaSetFeilvTime(0, i, *(pbuf + 5 + i * 8 + 1), *(pbuf + 5 + i * 8 + 0), *(pbuf + 5 + i * 8 + 3),  *(pbuf + 5 + i * 8 + 2));
				memcpy(&data32, pbuf + 5 + i * 8 + 4, 4);
				ParaSetFeilvMoney(0, i, data32);
				ParaSetFeilvTime(1, i, *(pbuf + 5 + i * 8 + 1), *(pbuf + 5 + i * 8 + 0), *(pbuf + 5 + i * 8 + 3),  *(pbuf + 5 + i * 8 + 2));
				ParaSetFeilvMoney(1, i, data32);
			}
			ret = 0;
			break;
			
		case 0x0012://��������
			ParaSetManufactureDate(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0013://��������
			ParaSetManufactureBase(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0014://�����
			ParaSetTestWorker(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0015://ʹ����
			ParaSetLifeTime(pbuf + 5);
			ret = 0;
			break;
		case 0x0016://������
			ret =ParaSetActCode(pbuf + 5);
			break;
		case 0x0017:// �����������
			SetTrickleCur((*(pbuf + 5) | (*(pbuf + 6) << 8)));
			ret = 0;
			break;
		case 0x0018:// ���SOC����
			SetTrickleSoc(*(pbuf + 5) );
			ret = 0;
			break;
		
		case 0x00D1: //��Դģ������
			ModuleType = *(pbuf + 5);
			MoudleNum = 0;
			MaxCurrent = 0;
			SetParacmd = 1;   //��8λ�������İ�ǹ
			ParaResult = 0;
			TimerSend = TimerRead();
			while((ParaResult == 0) && (TimerRead()- TimerSend) < T1S*4){
				OSTimeDlyHMSM (0, 0, 0, 10);
			}
			if(ParaResult )
				ret = 0;
			else
				ret = 1;
			break;
		case 0x00D2://��Դģ������
			
			ModuleType = 0;
			MoudleNum = *(pbuf + 6);
			MaxCurrent = 0;
			SetParacmd = 2 + ((*(pbuf + 5)-1)<< 7);
			ParaResult = 0;
			TimerSend = TimerRead();
			while((ParaResult == 0) && (TimerRead()- TimerSend) < T1S*4){
				OSTimeDlyHMSM (0, 0, 0, 10);
			}
			if(ParaResult)
				ret = 0;
			else
				ret = 1;
			break;
		
		case 0x00D4://����ܵ����޶�ֵ
			ModuleType = 0;
			MoudleNum = 0;
			MaxCurrent = (*(pbuf + 6) | (*(pbuf + 7) << 8));
			SetParacmd = 3 + ((*(pbuf + 5) - 1)<< 7);
			TimerSend = TimerRead();
			while((ParaResult == 0) && (TimerRead()- TimerSend) < T1S*4){
				OSTimeDlyHMSM (0, 0, 0, 10);
			}
			if(ParaResult )
				ret = 0;
			else
				ret = 1;
			break;
		case 0x00D6:// ���ʷ���ģʽ
			PowerMode = *(pbuf + 5);
			SetParacmd = 4;
			TimerSend = TimerRead();
			while((ParaResult == 0) && (TimerRead()- TimerSend) < T1S*4){
				OSTimeDlyHMSM (0, 0, 0, 10);
			}
			if(ParaResult )
				ret = 0;
			else
				ret = 1;
			break;
		default:
			break;
	}
	
	if (ret == 0){
		if(Channel == PARACHANNEL_UART){
			p = ackbuffer;
			*p++ = 0x68;
			*p++ = 0x91;
			*p++ = 0x00;
			*p++ = 0xF9;
			*p++ = 0x16;
			UartWrite(DEBUGUARTNO, ackbuffer, p - ackbuffer);
		}else if(Channel == PARACHANNEL_UDP){
			p = ackbuffer;
			*p++ = 0x68;
			memcpy(p, PileNo, 32);//׮���
			p += 32;
			*p++ = 0x68;
			*p++ = 0x91;
			*p++ = 0x00;
			*p++ = 0xF9;
			*p++ = 0x16;
			UDPSend( ackbuffer, p - ackbuffer);
		}
	} else {

		if(Channel == PARACHANNEL_UART){
			p = ackbuffer;
			*p++ = 0x68;
			*p++ = 0xD1;
			*p++ = 0x01;
			*p++ = ret;
			cs = 0;
			for (i = 0; i < 4; i++){
				cs += ackbuffer[i];
			}
			*p++ = cs;
			*p++ = 0x16;
			UartWrite(DEBUGUARTNO, ackbuffer, p - ackbuffer);
		}else if(Channel == PARACHANNEL_UDP){
			p = ackbuffer;
			*p++ = 0x68;
			memcpy(p, PileNo, 32);//׮���
			p += 32;
			*p++ = 0x68;
			*p++ = 0xD1;
			*p++ = 0x01;
			*p++ = ret;
			cs = 0;
			for (i = 0; i < 4; i++){
				cs += ackbuffer[i+33];
			}
			*p++ = cs;
			*p++ = 0x16;
			UDPSend( ackbuffer, p - ackbuffer);
		}
	}
	
	if (DI == 0x000E){ //�ն˸�λ
		OSTimeDlyHMSM (0, 0, 1, 0);
		NVIC_SystemReset();
	}
	
	return 1;
}

/****************************************************************************\
 Function:  AnalyseMsgFromDebugPlatform
 Descript:  �����ӵ���ƽ̨���յ�������
 Input:	    ��
 Output:    ��
 Return:    ��
 *****************************************************************************/
void AnalyseMsgFromDebugPlatform(void)
{
	static uint8_t buffer[512];
	static uint16_t len;
	uint16_t ret,i;
	
	ret = UartRead(DEBUGUARTNO, buffer + len, sizeof(buffer) - len);
	len += ret;
	if (len >= sizeof(buffer)){
		memset(buffer, 0, sizeof(buffer));
		len = 0;
	}
	if (len){
		for (i = 0; i < len; i++){
			if (ConfigProtocolFrameCheck(buffer + i, PARACHANNEL_UART) == 1){
				if ((buffer[i + 1] & 0x1f) ==0x01){
					DebugPlatformReadDataAnalyse(&buffer[i], PARACHANNEL_UART);
				} else if ((buffer[i + 1] & 0x1f) ==0x11){
					DebugPlatformWriteDataAnalyse(&buffer[i], PARACHANNEL_UART);
				}
				memset(buffer, 0, sizeof(buffer));
				len = 0;
				break;
			}
		}
	}
}
