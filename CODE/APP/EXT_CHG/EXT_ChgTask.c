#include "EXT_ChgTask.h"
static struct{
	CHARGEINFO info;
	uint8_t CCBState; //�����ư�״̬
	uint8_t State;    //״̬
	uint8_t StartResult;//������� 0�ɹ� 1ʧ��
	uint8_t StopCause;//ֹͣԭ��
	uint8_t RecvCCB_PF30_Flag;
	uint8_t RecvCCB_PF31_Flag;
	uint8_t RecvCCB_PF32_Flag;
	uint8_t RecvCCB_PF33_Flag;
	uint8_t RecvCCB_PF34_Flag;
	uint8_t RecvCCB_PF35_Flag;
}ChgCtl[2];

static uint8_t StartFlag[2],StartRet[2];
static uint8_t StopFlag[2],StopRet[2];
static uint8_t AssistPowerType[2];

static uint8_t ReadModuleType;
static uint8_t ReadMoudleNum;
static uint8_t ReadPowerMode;
static uint16_t ReadMaxCurrent;

/************************************************************************************************************
** �� �� �� : ReadModuletype
** �������� : ��ȡ���ʰ巢�͵�ģ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	���ʰ巢�͵�ģ������
*************************************************************************************************************
*/
uint8_t ReadModuletype(void)
{
	return ReadModuleType;
}




/************************************************************************************************************
** �� �� �� : ReadMoudlenum
** �������� : ��ȡ���ʰ巢�͵�ģ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	���ʰ巢�͵�ģ������
*************************************************************************************************************
*/
uint8_t ReadMoudlenum(void)
{
	return ReadMoudleNum;
}
/************************************************************************************************************
** �� �� �� : ReadMoudlenum
** �������� : ��ȡ���ʰ巢�͵�ģ��������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	���ʰ巢�͵�ģ��������
*************************************************************************************************************
*/
uint16_t ReadMaxcurrent(void)
{
	return ReadMaxCurrent;
}

/************************************************************************************************************
** �� �� �� : ReadPowerMod
** �������� : ��ȡ���ʰ巢�͵Ĺ��ʷ���ģʽ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	���ʰ巢�͵Ĺ��ʷ���ģʽ
*************************************************************************************************************
*/
uint8_t ReadPowerMod(void)
{
	return ReadPowerMode;
}




/************************************************************************************************************
** �� �� �� : SendDataToCCB
** �������� : �������ݵ������ư庯��
** ��    �� : canid��CAN��ID��
							pdata��Ҫ���͵����ݻ�������ַ
							len��Ҫ���͵��ֽ���
** ��    �� : ��
** ��    �� :	У���
*************************************************************************************************************
*/
static void SendDataToCCB(uint32_t canid,uint8_t *pdata,uint8_t len)
{
	if (CANWrite(DEV_CAN1, canid, pdata, len) == 0){
		OSTimeDlyHMSM (0, 0, 0, 1);
		CANWrite(DEV_CAN1, canid, pdata, len);
	}
	OSTimeDlyHMSM (0, 0, 0, 1);
}

/*********************************************************************************************************
** Function name:			MultiFramePackAndSend
** Descriptions:			��֡����ͷ��ͺ���
** Input parameters:	canid:CAN ID
											pdata:��������ݻ�����
											len:��������ݳ���
** Output parameters:	��
** Returned value:		��
*********************************************************************************************************/
static void MultiFramePackAndSend(uint32_t id, uint8_t *pdata, uint16_t len)
{
	uint8_t data[8]={0,0,0,0,0,0,0,0},*p;
	uint8_t packnum,num=2;
	uint16_t remainlen;
	
	if (len <= 8){
		SendDataToCCB(id, pdata, 8);
	} else {
		packnum = len / 7;
		if(len % 7){
			packnum++;
		}
		remainlen = len;
		p = pdata;
		for (num = 0; num < packnum; num++){
			memset(data, 0, 8);
			data[0] = num + 1;
			if(remainlen > 7){
				memcpy(&data[1], p, 7);
				remainlen -= 7;
				p += 7;
			} else {
				memcpy(&data[1], p, remainlen);
			}
			SendDataToCCB(id, data, 8);
		}
	}
}

/************************************************************************************************************
** �� �� �� : SendPF30
** �������� : ���ͼƷѿ��ư��������֡
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SendPF30(uint8_t port)
{
	uint32_t id=0x103000a0 | (port << 8);
	uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	
	if (AssistPowerType[port] == 24)
		data[0] = 1;
	else
		data[0] = 0;
	
	SendDataToCCB(id, data, 8);
}

/************************************************************************************************************
** �� �� �� : SendPF31
** �������� : ���ͼƷѿ��ư�Ӧ������ư��ϱ�������������֡
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SendPF31(uint8_t port)
{
	uint32_t id=0x103100a0 | (port << 8);
	uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	
	data[0] = 0;
	
	SendDataToCCB(id, data, 8);
}

/************************************************************************************************************
** �� �� �� : SendPF42
** �������� : ���ͼƷѿ��ư�ֹͣ���֡
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SendPF32(uint8_t port)
{
	uint32_t id=0x103200a0 | (port << 8);
	uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	
	SendDataToCCB(id, data, 8);
}

/************************************************************************************************************
** �� �� �� : SendPF43
** �������� : ���ͼƷѿ��ư�Ӧ������ư��ϱ���ֹͣ������֡
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SendPF33(uint8_t port)
{
	uint32_t id=0x103300a0 | (port << 8);
	uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	
	data[0] = 0;
	
	SendDataToCCB(id, data, 8);
}

/************************************************************************************************************
** �� �� �� : SendPara
** �������� : �������ö�ȡ���ʰ���ز���
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......   Mode  = 0   ����    = 1  ��ȡ
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void SendPara( uint8_t Mode)
{
	uint32_t id =0;
	uint8_t data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint16_t data16;
	if(Mode == 1){
		id = (SetParaCmd() & 0x80) == 0 ? 0x183600a0 : 0x183601a0;
		data[0] = (SetParaCmd() &0x7F);
		switch(data[0]){
			case 1:    //  ModuleType
				data[1] = BoardGetModuleType();
				break;
			case 2:   //  ModuleNum
				data[1] = BoardGetMoudleNum();
				break;
			case 3:   //MaxCurrent
				data16 = BoardGetMaxCurrent();
				data[1] = data16 & 0xff; 
				data[2] = (data16 >> 8) & 0xff;
				break;
			case 4:
				data[1] = BoardGetPowerMode();
				break;
			default:
				break;
		}
	}else{
		id = (GetParaCmd() & 0x80) == 0 ? 0x183700a0 : 0x183701a0;
		data[0] = (GetParaCmd() &0x7F);
	}
	ClearParaCmd();
	MultiFramePackAndSend(id, data, 8);
}


/************************************************************************************************************
** �� �� �� : SendPF35
** �������� : ���ͼƷѿ��ư�ң��֡
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SendPF35(uint8_t port)
{
	uint32_t id=0x183500a0 | (port << 8);
	uint8_t data[16], *pdata;
	uint32_t data32;
	
	memset(data, 0 , sizeof(data));
	pdata = data;
	data32 = ElmGetVolA(port) * 100;
	memcpy(pdata, &data32, 4);
	pdata+=4;
	data32 = ElmGetCurA(port);
	memcpy(pdata, &data32, 4);
	pdata+=4;

	data32 = 0;
	memcpy(pdata, &data32, 4);
	pdata+=4;
	data32 = 0;
	memcpy(pdata, &data32, 4);
	pdata+=4;
	MultiFramePackAndSend(id, data, 16);
}

/************************************************************************************************************
** �� �� �� : AnalyseMsgFromCCB
** �������� : ���������ڳ����ư������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void AnalyseMsgFromCCB(void)
{
	uint32_t id;
	uint8_t len,data[8],port = 0;
	uint8_t gunnum = ParaGetChgGunNumber();
	static uint8_t YaoCeBuffer[2][100],StartComplete[2][100];
	uint8_t *pdata;
	
	do{
		len = CANRead(DEV_CAN1, &id, data);
		if ((len && (((id >> 8) & 0x000000ff) == 0xa0) && ((id & 0x000000ff) < 2)) || ((id &0xffff) ==0xa080)|| ((id &0xffff) ==0xa081)){
			port = id & 0x000000ff;//port = id & 0x000000ff - 1;
			switch(id >> 16){
				case 0x1030://�������Ӧ��֡
					ChgCtl[port].RecvCCB_PF30_Flag = 1;
					break;
				case 0x1031://�������֡
					memcpy(&StartComplete[port][7 * (data[0] - 1)], &data[1], 7);
					pdata = StartComplete[port];
					ChgCtl[port].StartResult = *pdata++;
					ChgCtl[port].StopCause = *pdata++;
					memcpy(&ChgCtl[port].info.ChargerProtocolVersion, pdata, 3);
					pdata += 3;
					memcpy(&ChgCtl[port].info.BmsProtocolVersion, pdata, 3);
					pdata += 3;
					ChgCtl[port].info.BatteryType = *pdata++;
					ChgCtl[port].info.HighestTemperatureLimit = *pdata++;
					memcpy(&ChgCtl[port].info.HighestVoltageLimit, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.SingleBatteryHighestVoltageLimit, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.HighestCurrentLimit, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.RatedVol, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.BatteryVol, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.RatedCapacity, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.RateDenergy, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.BatterySOC, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.VIN, pdata, 17);
					pdata += 17;
					SendPF31(port);
					ChgCtl[port].RecvCCB_PF31_Flag = 1;
					break;
				case 0x1032://ֹͣ���Ӧ��֡
					ChgCtl[port].RecvCCB_PF32_Flag = 1;
					break;
				case 0x1033://ֹͣ���֡
					ChgCtl[port].StopCause = data[1];
					SendPF33(port);
					ChgCtl[port].RecvCCB_PF33_Flag = 1;
					break;
				case 0x1834://ң��
					if (data[0] & (1 << 0))
						ChgCtl[port].info.ScramState = 1;
					else
						ChgCtl[port].info.ScramState = 0;
					if (data[0] & (1 << 1))
						ChgCtl[port].info.DoorState = 1;
					else
						ChgCtl[port].info.DoorState = 0;
					if (data[0] & (1 << 2))
						ChgCtl[port].info.GunSeatState = 1;
					else
						ChgCtl[port].info.GunSeatState = 0;
					if (data[0] & (1 << 3))
						ChgCtl[port].info.CCBAddrConflictState = 1;
					else
						ChgCtl[port].info.CCBAddrConflictState = 0;
					ChgCtl[port].RecvCCB_PF34_Flag = 1;
					break;
				case 0x1835://ң��
					memcpy(&YaoCeBuffer[port][7 * (data[0] - 1)], &data[1], 7);
					pdata = YaoCeBuffer[port];
					ChgCtl[port].CCBState = *pdata++;
					memcpy(&ChgCtl[port].info.OutputVol, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.OutputCur, pdata, 2);
					pdata += 2;
					ChgCtl[port].info.SOC = *pdata++;
					ChgCtl[port].info.BetteryLowestTemperature = *pdata++;
					ChgCtl[port].info.BetteryHighestTemperature = *pdata++;
					memcpy(&ChgCtl[port].info.BetteryHighestVol, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.GuideVol, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.BmsDemandVol, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.BmsDemandCur, pdata, 2);
					pdata += 2;
					ChgCtl[port].info.Mode = *pdata++;
					memcpy(&ChgCtl[port].info.BmsMeasureVol, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.BmsMeasureCur, pdata, 2);
					pdata += 2;
					memcpy(&ChgCtl[port].info.SurplusMinute, pdata, 2);
					pdata += 2;
					ChgCtl[port].RecvCCB_PF35_Flag = 1;
					break;		
				case 0x1836://���ù��ʷ����
					if(data[1])
						SetParaResult(0);			
					else
						SetParaResult(1);		
					break;	
				case 0x1837://�����ʷ������ز���
					if(data[0] == 1){
						ReadModuleType = data[1];
					}else if(data[0] == 2){
						ReadMoudleNum = data[1]; 
					}else if(data[0] == 3){
						ReadMaxCurrent = ((data[2]<<8)|data[1]); 
					}else if(data[0] == 4){
						ReadPowerMode = data[1];
					}
					SetParaResult(1);				
					break;	
				default:
					break;
			}
		}
	}while(len);
}

/************************************************************************************************************
** �� �� �� : ChgHandle
** �������� : ��紦����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ChgHandle(uint8_t port)
{
	static uint32_t Timer[2],StartTimer[2],StopTimer[2],YaoCeTimer[2],CommTimer[2];
	
	if(SetParaCmd()){
		SendPara(1);
	}
	if(GetParaCmd()){
		SendPara(0);
	}
	
	switch (ChgCtl[port].State){
		case 0: //����
			ChgCtl[port].info.ChargeState = 0;
			if ((ChgCtl[port].CCBState != 0) && (TimerRead() - StopTimer[port] > T100MS * 5)){
				StopTimer[port] = TimerRead();
				SendPF32(port);
			}
			if (StartFlag[port] == 1){
				StopFlag[port] = 0;
				ChgCtl[port].State++;
				ChgCtl[port].RecvCCB_PF30_Flag = 0;
				ChgCtl[port].RecvCCB_PF31_Flag = 0;
				ChgCtl[port].RecvCCB_PF32_Flag = 0;
				ChgCtl[port].RecvCCB_PF33_Flag = 0;
				StartTimer[port] = TimerRead();
				Timer[port] = TimerRead();
				SendPF30(port);
			}
			break;
			
		case 1: //����
			ChgCtl[port].info.ChargeState = 2;
			StartFlag[port] = 0;
			if ((ChgCtl[port].RecvCCB_PF30_Flag == 0) && (TimerRead() - Timer[port] > T100MS * 5)){
				Timer[port] = TimerRead();
				SendPF30(port);
			}
			if (TimerRead() - StartTimer[port] > T1S * 120){
				SendPF32(port);
				StartRet[port] = 2;
				ChgCtl[port].State = 0;
			}
			if (ChgCtl[port].RecvCCB_PF31_Flag == 1){
				if (ChgCtl[port].StartResult == 0){//�����ɹ�
					ChgCtl[port].State++;
					Timer[port] = TimerRead();
				} else {//����ʧ��
					SendPF32(port);
					StartRet[port] = 2;
					ChgCtl[port].State = 0;
				}
			}
			if (StopFlag[port] == 1){
				ChgCtl[port].State = 3;
				StartRet[port] = 2;
				StopRet[port] = 1;
			}
			break;
			
		case 2: //���
			ChgCtl[port].info.ChargeState = 1;
			StartRet[port] = 1;
			if (StopFlag[port] == 1){
				ChgCtl[port].State++;
				StopRet[port] = 1;
			}
			if((ChgCtl[port].CCBState != 1) && (TimerRead() - Timer[port] > T1S * 5)){
				SendPF32(port);
				Timer[port] = TimerRead();
				ChgCtl[port].State++;
			}
			break;
			
		case 3: //ֹͣ
			ChgCtl[port].info.ChargeState = 1;
			StopFlag[port] = 0;
			if ((ChgCtl[port].RecvCCB_PF32_Flag == 0) && (TimerRead() - StopTimer[port] > T100MS * 5)){
				StopTimer[port] = TimerRead();
				SendPF32(port);
			}
			if ((ChgCtl[port].RecvCCB_PF33_Flag == 1) || ((TimerRead() - Timer[port] > T1S * 10))){
				ChgCtl[port].State = 0;
			}
			break;
			
		default:
			break;
	}
	
	if (TimerRead() - YaoCeTimer[port] > T100MS * 10){
		YaoCeTimer[port] = TimerRead();
		SendPF35(port);
	}
	
	if (ChgCtl[port].RecvCCB_PF34_Flag == 0){
		if (TimerRead() - CommTimer[port] > T1S * 10){
			ChgCtl[port].info.CommunicateState = 1;
		}
	} else {
		ChgCtl[port].RecvCCB_PF34_Flag = 0;
		CommTimer[port] = TimerRead();
		ChgCtl[port].info.CommunicateState = 0;
	}
	AnalyseMsgFromCCB();
}


/************************************************************************************************************
** �� �� �� : GetStopCause
** �������� : ��ȡ���ֹͣԭ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
uint8_t GetEXT_StopCause(uint8_t port)
{
	return ChgCtl[port].StopCause;
}


/************************************************************************************************************
** �� �� �� : GetCCBInfo
** �������� : ��ȡCCB��Ϣ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
CHARGEINFO GetEXT_CCBInfo(uint8_t port)
{
	return ChgCtl[port].info;
}

/************************************************************************************************************
** �� �� �� : StartCharge
** �������� : ������纯��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��� 0�ɹ� 1ʧ��
*************************************************************************************************************
*/
uint8_t EXT_StartCharge(uint8_t port, uint8_t APVol)
{
	AssistPowerType[port] = APVol;
	StartRet[port] = 0;
	StartFlag[port] = 1;
	while (StartFlag[port] == 1)
		OSTimeDlyHMSM (0, 0, 0, 100);
	
	while (1){
		if (StartRet[port] == 1)
			return 0;
		if (StartRet[port] == 2)
			return 1;
		OSTimeDlyHMSM (0, 0, 0, 100);
	}
}

/************************************************************************************************************
** �� �� �� : StopCharge
** �������� : ֹͣ��纯��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
uint8_t EXT_StopCharge(uint8_t port)
{
	uint32_t timer;
	
	StopRet[port] = 0;
	StopFlag[port] = 1;
	while (StopFlag[port] == 1)
		OSTimeDlyHMSM (0, 0, 0, 100);
	timer = TimerRead();
	while(TimerRead() - timer < T1S * 10){
		if (StopRet[port] == 1)
			break;
		OSTimeDlyHMSM (0, 0, 0, 100);
	}
	return 1;
}

/************************************************************************************************************
** �� �� �� : ChgTask
** �������� : �������
** ��    �� : pdata δʹ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void ChgTask(void *pdata)
{
	uint8_t port, PortNum = ParaGetChgGunNumber();
	
	while(1) {
		for (port = 0; port < PortNum; port++){
			ChgHandle(port);
			OSTimeDlyHMSM (0, 0, 0, 10);
		}
	}
}
