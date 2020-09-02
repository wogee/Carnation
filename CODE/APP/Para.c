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
** 函 数 名 : GetUpdata
** 功能描述 : 获取升级标志
** 输    入 : 无
** 输    出 : 无
** 返    回 :	0x5a 升级   0  不升级
*************************************************************************************************************
*/
uint32_t GetUpdata(void)
{
	EepromRead(0, EEPROMADDR_SAVEINFO, &CfgInfo, MODE_8_BIT, sizeof(CFGINFO));
	return CfgInfo.Updata;
}



/************************************************************************************************************
** 函 数 名 : CfgSave
** 功能描述 : 保存参数
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
static void CfgSave(void)
{
	//应增加互斥型信号量
	EepromWrite(0, EEPROMADDR_SAVEINFO, &CfgInfo, MODE_8_BIT, sizeof(CFGINFO));
}

/************************************************************************************************************
** 函 数 名 : InitTrickleInfo
** 功能描述 : 涓流充电参数初始化
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : SetTrickleCur
** 功能描述 : 设置涓流充电电流限制值    单位  0.1A
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void SetTrickleCur(uint16_t Cur)
{
	TrickleInfo.LimitCur = Cur;
	EepromWrite(0, EEPROMADDR_TRICKLECTRLINFO, &TrickleInfo, MODE_8_BIT, sizeof(TRICKLEINFO));
}

/************************************************************************************************************
** 函 数 名 : SetTrickleInfo
** 功能描述 : 设置涓流充电限制信息    单位  0.1A
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : GetTrickleCur
** 功能描述 : 获取涓流充电电流限制值    单位  0.1A
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
uint16_t GetTrickleCur(void)
{
	return TrickleInfo.LimitCur;
}	


/************************************************************************************************************
** 函 数 名 : GetTrickleMin
** 功能描述 : 获取涓流充电电流限制值    单位  0.1A
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
uint16_t GetTrickleMin(void)
{
	return TrickleInfo.LimitMin;
}
/************************************************************************************************************
** 函 数 名 : SetTrickleMin
** 功能描述 : 设置涓流充电 时间限制 单位分钟  
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void SetTrickleMin(uint16_t Min)
{
	TrickleInfo.LimitMin = Min;
	EepromWrite(0, EEPROMADDR_TRICKLECTRLINFO, &TrickleInfo, MODE_8_BIT, sizeof(TRICKLEINFO));
}

/************************************************************************************************************
** 函 数 名 : SetTrickleSoc
** 功能描述 : 设置涓流充电 SOC 限制值   
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void SetTrickleSoc(uint8_t Soc)
{
	TrickleInfo.LimitSoc = Soc;
	EepromWrite(0, EEPROMADDR_TRICKLECTRLINFO, &TrickleInfo, MODE_8_BIT, sizeof(TRICKLEINFO));
}
/************************************************************************************************************
** 函 数 名 : GetTrickleSoc
** 功能描述 : 获取涓流充电 SOC 限制值   
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
uint8_t GetTrickleSoc(void)
{
	return TrickleInfo.LimitSoc;
}
/************************************************************************************************************
** 函 数 名 : SetUpdataFlag
** 功能描述 : 设置更新标识
** 输    入 : flag
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void SetUpdataFlag(uint32_t flag)
{
	CfgInfo.Updata = flag;
	CfgSave();
}


/************************************************************************************************************
** 函 数 名 : ParaDefault
** 功能描述 : 设置默认参数
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
static void ParaDefault(void)
{
	uint8_t port,i;
	
	memset(&ParaInfo,0,sizeof(PARAINFO));
	
	ParaInfo.version = 0x5a5a5a02;
	
	//充枪数量
	ParaInfo.ChgGunNumber = 1;
	
	//上行通道 默认1以太网
	ParaInfo.UpChannel = 1;
	
	//本机IP
	ParaInfo.LocalIp[0] = 192;
	ParaInfo.LocalIp[1] = 168;
	ParaInfo.LocalIp[2] = 1;
	ParaInfo.LocalIp[3] = 10;
	
	//网关
	ParaInfo.GateWay[0] = 192;
	ParaInfo.GateWay[1] = 168;
	ParaInfo.GateWay[2] = 1;
	ParaInfo.GateWay[3] = 1;
	
	//子网掩码
	ParaInfo.SubMask[0] = 255;
	ParaInfo.SubMask[1] = 255;
	ParaInfo.SubMask[2] = 255;
	ParaInfo.SubMask[3] = 0;
	
	//物理地址
	ParaInfo.PhyMac[0] = 0x00;
	ParaInfo.PhyMac[1] = 0x00;
	ParaInfo.PhyMac[2] = 0x10;
	ParaInfo.PhyMac[3] = 0x00;
	ParaInfo.PhyMac[4] = 0x00;
	ParaInfo.PhyMac[5] = 0x00;
	
	//硬件版本号
	memset(ParaInfo.HardVersion, 0, 16);
	
	//客户编号
	memset(ParaInfo.CustomerNumber, 0, 6);
	
	memset(ParaInfo.APN, 0, 16); //APN
	memset(ParaInfo.APNUser, 0, 32); //APN用户名
	memset(ParaInfo.APNPasswd, 0, 32); //APN密码
	
	ParaInfo.ScreenOpenHour = 18; //广告屏开启时间 小时
	ParaInfo.ScreenOpenMin = 0; //广告屏开启时间 分钟
	ParaInfo.ScreenStopHour = 6; //广告屏关闭时间 小时
	ParaInfo.ScreenStopMin = 0; //广告屏关闭时间 分钟
	
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
			ParaInfo.Port[port].StartMin[i] = 0; //计费策略 开始分钟
			ParaInfo.Port[port].StopHour[i] = 0; //计费策略 结束小时
			ParaInfo.Port[port].StopMin[i] = 0; //计费策略 结束分钟
			ParaInfo.Port[port].Money[i] = 0; //计费策略 费率 每度电的电费 保留5位小数
		}
		
		ParaInfo.Port[port].StateInfoReportPeriod = 15;
		//memcpy(ParaInfo.Port[port].QrData, ParaInfo.Port[port].PileNo, 32);
	}
	
	memset(ParaInfo.ManufactureDate, 0, 4); //生产日期
	memset(ParaInfo.ManufactureBase, 0, 16); //生产基地
	memset(ParaInfo.TestWorker, 0, 8); //检测人
	memset(ParaInfo.LifeTime, 0, 5); //使用期
}

/************************************************************************************************************
** 函 数 名 : SaveInfoDefault
** 功能描述 : 参数初始化
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : SaveInfoV2Def
** 功能描述 : 新增参数变量初始化
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void SaveInfoV2Def(void){
	memcpy(CfgInfo.ActPassword, "0000", 4);
	CfgInfo.LimitHour = 0;
	RunHour = 0;	
}


/************************************************************************************************************
** 函 数 名 : ParaInit
** 功能描述 : 参数初始化
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
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
			ParaDefault(); //默认参数	
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
** 函 数 名 : ParaSave
** 功能描述 : 保存参数
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSave(void)
{
	//应增加互斥型信号量
	EepromWrite(0, EEPROMADDR_PARAINFO, &ParaInfo, MODE_8_BIT, sizeof(PARAINFO));
}

/************************************************************************************************************
** 函 数 名 : ParaSetChgGunNumber
** 功能描述 : 设置充枪个数
** 输    入 : number 个数 1或2
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetChgGunNumber
** 功能描述 : 获取充枪个数
** 输    入 : 无
** 输    出 : 无
** 返    回 :	充枪个数 1或2
*************************************************************************************************************
*/
uint8_t ParaGetChgGunNumber(void)
{
	return ParaInfo.ChgGunNumber;
}

/************************************************************************************************************
** 函 数 名 : ParaSetPileNo
** 功能描述 : 设置充电桩编号 32个字节
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......  data 充电桩编号 32个字节
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetPileNo
** 功能描述 : 获取充电桩编号 32个字节
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......  
** 输    出 : 充电桩编号
** 返    回 :	无
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
** 函 数 名 : ParaSetLoginTimeInterval
** 功能描述 : 设置签到间隔时间
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪...... TimeInterval 分钟
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetLoginTimeInterval(uint8_t port, uint16_t TimeInterval)
{
	ParaInfo.Port[port].LoginTimeInterval = TimeInterval;
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetLoginTimeInterval
** 功能描述 : 获取签到间隔时间
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	时间 分钟
*************************************************************************************************************
*/
uint16_t ParaGetLoginTimeInterval(uint8_t port)
{
	return ParaInfo.Port[port].LoginTimeInterval;
}

/************************************************************************************************************
** 函 数 名 : ParaSetACMeterAddr
** 功能描述 : 设置交流电表地址
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   data 电表地址 data[0]为A0
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetACMeterAddr(uint8_t port, uint8_t *data)
{
	memcpy(ParaInfo.Port[port].ACMeterAddr, data, 6);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetACMeterAddr
** 功能描述 : 获取交流电表地址
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   data 电表地址 data[0]为A0
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetACMeterAddr(uint8_t port, uint8_t *data)
{
	memcpy(data, ParaInfo.Port[port].ACMeterAddr, 6);
}

/************************************************************************************************************
** 函 数 名 : ParaSetDCMeterAddr
** 功能描述 : 设置直流电表地址
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   data 电表地址 data[0]为A0
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetDCMeterAddr(uint8_t port, uint8_t *data)
{
	memcpy(ParaInfo.Port[port].DCMeterAddr, data, 6);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetDCMeterAddr
** 功能描述 : 获取直流电表地址
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   data 电表地址 data[0]为A0
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetDCMeterAddr(uint8_t port, uint8_t *data)
{
	memcpy(data, ParaInfo.Port[port].DCMeterAddr, 6);
}

/************************************************************************************************************
** 函 数 名 : ParaSetNetHeartTime
** 功能描述 : 设置网络心跳时间
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   time 单位 S
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetNetHeartTime
** 功能描述 : 获取网络心跳时间
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   cnt 重发次数
** 输    出 : 无
** 返    回 :	心跳时间  单位 S
*************************************************************************************************************
*/
uint16_t ParaGetNetHeartTime(uint8_t port)
{
	return ParaInfo.Port[port].NetHeartTime;
}

/************************************************************************************************************
** 函 数 名 : ParaSetNetCommOverCnt
** 功能描述 : 设置网络通讯超时重发次数 
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   cnt 重发次数
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetNetCommOverCnt
** 功能描述 : 获取网络通讯超时重发次数
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   cnt 重发次数
** 输    出 : 无
** 返    回 :	重发次数
*************************************************************************************************************
*/
uint8_t ParaGetNetCommOverCnt(uint8_t port)
{
	return ParaInfo.Port[port].NetCommOverCnt;
}

/************************************************************************************************************
** 函 数 名 : ParaSetUpChannel
** 功能描述 : 设置上行通道
** 输    入 : channel 0以太网 1GPRS
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetUpChannel(uint8_t channel)
{
	ParaInfo.UpChannel = channel;
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetUpChannel
** 功能描述 : 获取上行通道
** 输    入 : 无
** 输    出 : 无
** 返    回 :	0以太网 1GPRS
*************************************************************************************************************
*/
uint8_t ParaGetUpChannel(void)
{
	return ParaInfo.UpChannel;
}

/************************************************************************************************************
** 函 数 名 : ParaSetLocalIp
** 功能描述 : 设置本机IP 4个字节
** 输    入 : IP
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetLocalIp
** 功能描述 : 获取本机IP 4个字节
** 输    入 : IP
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaSetSubMask
** 功能描述 : 设置子网掩码  4个字节
** 输    入 : 子网掩码
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetSubMask
** 功能描述 : 获取子网掩码 4个字节
** 输    入 : 缓存指针
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaSetGateWay
** 功能描述 : 设置网关  4个字节
** 输    入 : 缓存指针
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetGateWay
** 功能描述 : 获取网关 4个字节
** 输    入 : 缓存指针
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaSetPhyMac
** 功能描述 : 设置物理MAC  6个字节
** 输    入 : 物理MAC
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetPhyMac
** 功能描述 : 获取物理MAC 6个字节
** 输    入 : 缓存指针
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaSetServerIp
** 功能描述 : 设置服务器IP
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   data 服务器IP
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetServerIp
** 功能描述 : 获取服务器IP
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   data 服务器IP
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaSetServerPort
** 功能描述 : 设置服务器端口
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   port 服务器端口
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetServerPort
** 功能描述 : 获取服务器端口
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   
** 输    出 : 无
** 返    回 :	服务器端口
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
** 函 数 名 : ParaSetFeilvTime
** 功能描述 : 设置计费策略的时间
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   
							period 12个时段 0-11
							starthour 开始小时 0-24
							startmin 开始分钟 0或30
							stophour 结束小时 0-24
							stopmin 结束分钟 0或30
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetFeilvTime
** 功能描述 : 获取计费策略的时间
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......    period 12个时段 0-11
** 输    出 : starthour 开始小时 0-24
							startmin 开始分钟 0或30
							stophour 结束小时 0-24
							stopmin 结束分钟 0或30
** 返    回 :	服务器端口
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
** 函 数 名 : ParaSetFeilvMoney
** 功能描述 : 设置计费策略的单价
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   
							period 12个时段 0-11
							money 单价 保留5位小数
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetFeilvMoney
** 功能描述 : 获取计费策略的单价
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......    period 12个时段 0-11
** 输    出 : 无
** 返    回 :	money 单价 保留5位小数
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
** 函 数 名 : ParaSetStateInfoReportPeriod
** 功能描述 : 设置状态信息上报周期
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   time 单位 S
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetStateInfoReportPeriod
** 功能描述 : 获取状态信息上报周期
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	心跳时间  单位 S
*************************************************************************************************************
*/
uint16_t ParaGetStateInfoReportPeriod(uint8_t port)
{
	return ParaInfo.Port[port].StateInfoReportPeriod;
}

/************************************************************************************************************
** 函 数 名 : ParaSetQRCode
** 功能描述 : 设置二维码
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......   qrcode 二维码，最长256个字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetQRCode(uint8_t port, uint8_t *qrcode)
{
	memcpy(ParaInfo.Port[port].QrData, qrcode, 256);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetQRCode
** 功能描述 : 获取二维码
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪...... qrcode 二维码，最长256个字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetQRCode(uint8_t port, uint8_t *qrcode)
{
	memcpy(qrcode, ParaInfo.Port[port].QrData, 256);
}

/************************************************************************************************************
** 函 数 名 : ParaSetHardVersion
** 功能描述 : 设置硬件版本号
** 输    入 : data 版本号 ASCII 16字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetHardVersion(uint8_t *data)
{
	memcpy(ParaInfo.HardVersion, data, 16);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetHardVersion
** 功能描述 : 获取硬件版本号
** 输    入 : data 版本号 ASCII 16字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetHardVersion(uint8_t *data)
{
	memcpy(data, ParaInfo.HardVersion, 16);
}

/************************************************************************************************************
** 函 数 名 : ParaSetHardVersion
** 功能描述 : 设置客户编号
** 输    入 : data 客户编号 6字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetCustomerNumber(uint8_t *data)
{
	memcpy(ParaInfo.CustomerNumber, data, 4);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetCustomerNumber
** 功能描述 : 获取客户编号
** 输    入 : data 客户编号 6字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetCustomerNumber(uint8_t *data)
{
	memcpy(data, ParaInfo.CustomerNumber, 4);
}

/************************************************************************************************************
** 函 数 名 : ParaSetAPN
** 功能描述 : 设置APN
** 输    入 : data 客户编号 16字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetAPN(uint8_t *data)
{
	memcpy(ParaInfo.APN, data, 16);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetAPN
** 功能描述 : 获取APN
** 输    入 : data 6字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetAPN(uint8_t *data)
{
	memcpy(data, ParaInfo.APN, 16);
}

/************************************************************************************************************
** 函 数 名 : ParaSetAPNUser
** 功能描述 : 设置APN用户名
** 输    入 : data 32字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetAPNUser(uint8_t *data)
{
	memcpy(ParaInfo.APNUser, data, 32);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetAPNUser
** 功能描述 : 获取APN用户名
** 输    入 : data 6字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetAPNUser(uint8_t *data)
{
	memcpy(data, ParaInfo.APNUser, 32);
}

/************************************************************************************************************
** 函 数 名 : ParaSetAPNPasswd
** 功能描述 : 设置APN密码
** 输    入 : data 32字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetAPNPasswd(uint8_t *data)
{
	memcpy(ParaInfo.APNPasswd, data, 32);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetAPNPasswd
** 功能描述 : 获取APN密码
** 输    入 : data 6字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetAPNPasswd(uint8_t *data)
{
	memcpy(data, ParaInfo.APNPasswd, 32);
}

/************************************************************************************************************
** 函 数 名 : ParaSetFeilvTime
** 功能描述 : 设置计费策略的时间
** 输    入 : openhour 开启小时 0-24
							openmin 开启分钟 0或30
							closehour 关闭小时 0-24
							closemin 关闭分钟 0或30
** 输    出 : 无
** 返    回 :	无
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
** 函 数 名 : ParaGetScreenTime
** 功能描述 : 获取广告屏开启关闭时间
** 输    入 : 无
** 输    出 : openhour 开启小时 0-24
							openmin 开启分钟 0或30
							closehour 关闭小时 0-24
							closemin 关闭分钟 0或30
** 返    回 :	无
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
** 函 数 名 : ParaSetManufactureDate
** 功能描述 : 设置出厂日期
** 输    入 : Date 日期 4字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetManufactureDate(uint8_t *Date)
{
	memcpy(ParaInfo.ManufactureDate, Date, 4);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetManufactureDate
** 功能描述 : 获取出厂日期
** 输    入 : 无
** 输    出 : Date 日期 4字节
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetManufactureDate(uint8_t *Date)
{
	memcpy(Date, ParaInfo.ManufactureDate, 4);
}

/************************************************************************************************************
** 函 数 名 : ParaSetManufactureBase
** 功能描述 : 设置生产基地
** 输    入 : Base 日期 16字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetManufactureBase(uint8_t *Base)
{
	memcpy(ParaInfo.ManufactureBase, Base, 16);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetManufactureBase
** 功能描述 : 获取生产基地
** 输    入 : 无
** 输    出 : Base 日期 16字节
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetManufactureBase(uint8_t *Base)
{
	memcpy(Base, ParaInfo.ManufactureBase, 16);
}

/************************************************************************************************************
** 函 数 名 : ParaSetTestWorker
** 功能描述 : 设置检验人
** 输    入 : TestWorker 检验人 8字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetTestWorker(uint8_t *TestWorker)
{
	memcpy(ParaInfo.TestWorker, TestWorker, 8);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetManufactureBase
** 功能描述 : 获取检验人
** 输    入 : 无
** 输    出 : TestWorker 检验人 8字节
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetTestWorker(uint8_t *TestWorker)
{
	memcpy(TestWorker, ParaInfo.TestWorker, 8);
}

/************************************************************************************************************
** 函 数 名 : ParaSetLifeTime
** 功能描述 : 设置使用期限
** 输    入 : LifeTime 使用期限 5字节
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSetLifeTime(uint8_t *LifeTime)
{
	memcpy(ParaInfo.LifeTime, LifeTime, 5);
	ParaSave();
}

/************************************************************************************************************
** 函 数 名 : ParaGetLifeTime
** 功能描述 : 获取使用期限
** 输    入 : 无
** 输    出 : LifeTime 使用期限 5字节
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetLifeTime(uint8_t *LifeTime)
{
	memcpy(LifeTime, ParaInfo.LifeTime, 5);
}


/************************************************************************************************************
** 函 数 名 : ParaGetLimitHour
** 功能描述 : 获取设置限制使用的时长，单位
** 输    入 : 无
** 输    出 : 无
** 返    回 :	设置的时长，单位：小时
*************************************************************************************************************
*/
uint32_t ParaGetLimitHour(void)
{
	return CfgInfo.LimitHour;
}



/************************************************************************************************************
** 函 数 名 : ParaSetActCode
** 功能描述 : 设置激活码，若密码正确，成功写入返回0  否则返回1
** 输    入 : buf
** 输    出 : 无
** 返    回 :	1 写入错误   0 写入正确
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
	Decryption(Getbuf, strlen((const char *)Getbuf), str);		//解密
	pstr = str;
	memcpy(newpass, pstr, 4);
	memcpy(oldpass, pstr+4, 4);
	memcpy(daystr, pstr+8, 4);
	if(memcmp(oldpass, CfgInfo.ActPassword, 4))
		return 1;
	memcpy(CfgInfo.ActPassword, newpass, 4);// 将new  写入  old
	memcpy(CfgInfo.ActCode, data, 32); //写入激活码
	CfgInfo.LimitHour =  atoi((const char *)daystr)* 24;	
	RunHour = 0;
	EepromWrite(0, EEPROMADDR_SAVERUNHOUR, &RunHour, MODE_8_BIT, sizeof(RunHour));
	EepromWrite(0, EEPROMADDR_SAVEINFO, &CfgInfo, MODE_8_BIT, sizeof(CFGINFO));
	return 0;
}


/************************************************************************************************************
** 函 数 名 : ParaGetActCode
** 功能描述 : 获取激活码
** 输    入 : 无
** 输    出 : 32字节激活码
** 返    回 :	无
*************************************************************************************************************
*/
void ParaGetActCode(uint8_t *ActCode)
{
	memcpy(ActCode, CfgInfo.ActCode, 32);
}

/************************************************************************************************************
** 函 数 名 : ParaGetRunHour
** 功能描述 : 获取设备从设置使用期限起到现在的运行时间  单位：小时
** 输    入 : 无
** 输    出 : 无
** 返    回 :	正确返回1 错误返回0
*************************************************************************************************************
*/
uint32_t ParaGetRunHour(void)
{
	return RunHour;
}


/************************************************************************************************************
** 函 数 名 : ParaSavaRunHour
** 功能描述 : 存储运行时间参数
** 输    入 : 运行时间
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ParaSavaRunHour(uint32_t  runhour)
{
	RunHour = runhour;
	EepromWrite(0, EEPROMADDR_SAVERUNHOUR, &RunHour, MODE_8_BIT, sizeof(RunHour));
}







/************************************************************************************************************
** 函 数 名 : ConfigProtocolFrameCheck
** 功能描述 : 配置协议帧检查函数
** 输    入 : buf数据缓冲区首地址  Channel 协议通道
** 输    出 : 无
** 返    回 :	正确返回1 错误返回0
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
		if( memcmp( &buf[1], PileNumber, 32) ){  //判断桩编号
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
** 函 数 名 : DebugPlatformReadDataAnalyse
** 功能描述 : 测试平台读数据分析
** 输    入 : buf数据缓冲区首地址  Channel：串口 OR UDP
** 输    出 : 无
** 返    回 :	正确返回1 错误返回0
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
		case 0x0001://充电桩编号
			databuf[0] = *(pbuf + 5);
			if (*(pbuf + 5) == 1){
				ParaGetPileNo(0, databuf + 1);
			} else if (*(pbuf + 5) == 2){
				ParaGetPileNo(1, databuf + 1);
			}
			datalen = 33;
			ret = 0;
			break;
			
		case 0x0002://软件版本号
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
		
		case 0x0003://硬件版本
			ParaGetHardVersion(databuf);
			datalen = 16;
			ret = 0;
			break;
		
		case 0x0004://充电桩类型
			databuf[0] = ParaGetChgGunNumber();
			datalen = 1;
			ret = 0;
			break;
		
		case 0x0007://充电桩当前时间
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
		
		case 0x0008://心跳周期
			databuf[0] = ParaGetNetHeartTime(0);
			datalen = 1;
			ret = 0;
			break;
		
		case 0x0009://广告屏开启时间
			ParaGetScreenTime(databuf + 1, databuf, &closehour, &closemin);
			datalen = 2;
			ret = 0;
			break;
		
		case 0x000A://广告屏关闭时间
			ParaGetScreenTime(&openhour, &openmin, databuf + 1, databuf);
			datalen = 2;
			ret = 0;
			break;
		
		case 0x000B://交流电表地址
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				databuf[0] = *(pbuf + 5);
				ParaGetACMeterAddr(*(pbuf + 5) - 1, databuf + 1);
				datalen = 7;
			}
			ret = 0;
			break;
			
		case 0x000C://直流电表地址
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				databuf[0] = *(pbuf + 5);
				ParaGetDCMeterAddr(*(pbuf + 5) - 1, databuf + 1);
				datalen = 7;
			}
			ret = 0;
			break;
			
		case 0x000D://客户编号
			ParaGetCustomerNumber(databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x000F://本机机器码
			memcpy(databuf, "0000000000000001", 16);
			datalen = 16;
			ret = 0;
			break;
		
		case 0x0011://二维码
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				databuf[0] = *(pbuf + 5);
				ParaGetQRCode(*(pbuf + 5) - 1, databuf + 1);
				datalen = 129;
				ret = 0;
			}
			break;
		
		case 0x0101://上行通道
			databuf[0] = ParaGetUpChannel();
			datalen = 1;
			ret = 0;
			break;
		
		case 0x0102://服务器IP
			ParaGetServerIp(0, databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x0103://服务器端口
			data16 = ParaGetServerPort(0);
			memcpy(databuf, &data16, 2);
			datalen = 2;
			ret = 0;
			break;	
		
		case 0x0104://本机IP
			ParaGetLocalIp(databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x0105://子网掩码
			ParaGetSubMask(databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x0106://网关
			ParaGetGateWay(databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x0107://物理地址
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
		
		case 0x0201://尖峰平谷费率
			for (i = 0; i < 12; i++){
				ParaGetFeilvTime(0, i, databuf + i * 8 + 1, databuf + i * 8 + 0, databuf + i * 8 + 3, databuf + i * 8 + 2);
				data32 = ParaGetFeilvMoney(0, i);
				memcpy(databuf + i * 8 + 4, &data32, 4);
			}
			datalen = 96;
			ret = 0;
			break;
			
		case 0x0012://生产日期
			ParaGetManufactureDate(databuf);
			datalen = 4;
			ret = 0;
			break;
		
		case 0x0013://生产基地
			ParaGetManufactureBase(databuf);
			datalen = 16;
			ret = 0;
			break;
		
		case 0x0014://检测人
			ParaGetTestWorker(databuf);
			datalen = 8;
			ret = 0;
			break;
		
		case 0x0015://使用期
			ParaGetLifeTime(databuf);
			datalen = 5;
			ret = 0;
			break;
		case 0x0016://激活码
			ParaGetActCode(databuf);
			datalen = 32;
			ret = 0;
			break;
		case 0x0017:// 涓流电流控制	
			databuf[0] = (GetTrickleCur() & 0xFF);
			databuf[1] = (GetTrickleCur() >> 8);
			datalen = 2;
			ret = 0;
			break;
		
		case 0x0018:// 涓流SOC控制		
			databuf[0] = GetTrickleSoc();
			datalen = 1;
			ret = 0;
			break;	
		
		case 0x00D1:  //模块类型
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
		case 0x00D2:  //模块数量
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
		case 0x00D4:  //电流值
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
		case 0x00D6:  //功率分配模式
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
			ParaGetPileNo(0, p);  //桩编号
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
			ParaGetPileNo(0, p);  //桩编号
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
** 函 数 名 : SetParaResult
** 功能描述 : 设置成功标志
** 输    入 : Result 设置结果  1成功  0  失败
** 输    出 : 无
** 返    回 :	
*************************************************************************************************************
*/
void SetParaResult(uint8_t Result)
{
	ParaResult = Result; 
}


/************************************************************************************************************
** 函 数 名 : SetParaCmd
** 功能描述 : 获取串口设置相关命令
** 输    入 : port  充电枪编号
** 输    出 : 无
** 返    回 :	
*************************************************************************************************************
*/
uint8_t SetParaCmd(void)
{
	return SetParacmd;
}


/************************************************************************************************************
** 函 数 名 : GetParaCmd
** 功能描述 : 获取串口设置相关命令
** 输    入 : port  充电枪编号
** 输    出 : 无
** 返    回 :	
*************************************************************************************************************
*/
uint8_t GetParaCmd(void)
{
	return GetParacmd;
}



/************************************************************************************************************
** 函 数 名 : ClearParaCmd
** 功能描述 : 清除串口设置相关命令
** 输    入 : 
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ClearParaCmd(void)
{
	 SetParacmd = 0x00;
	 GetParacmd = 0x00;
}

/************************************************************************************************************
** 函 数 名 : BoardGetModuleType
** 功能描述 : 获取串口下发模块类型
** 输    入 :	
** 输    出 :	无
** 返    回 :	 ModuleType 模块类型 
*************************************************************************************************************
*/
uint8_t BoardGetModuleType(void)
{
	return ModuleType;
}


/************************************************************************************************************
** 函 数 名 : BoardGetMoudleNum
** 功能描述 : 获取串口下发模块数量
** 输    入 : 
** 输    出 : 无
** 返    回 :	正确返回1 错误返回0
*************************************************************************************************************
*/
uint8_t BoardGetMoudleNum(void)
{
	return MoudleNum;
}


/************************************************************************************************************
** 函 数 名 : BoardGetMaxCurrent
** 功能描述 : 获取串口下发最大电流
** 输    入 : 
** 输    出 : 无
** 返    回 :	
*************************************************************************************************************
*/
uint16_t BoardGetMaxCurrent(void)
{
	return MaxCurrent;
}
/************************************************************************************************************
** 函 数 名 : BoardGetPowerMode
** 功能描述 : 获取串口下发的功率分配模式
** 输    入 : 
** 输    出 : 无
** 返    回 :	
*************************************************************************************************************
*/
uint8_t BoardGetPowerMode(void)
{
	return PowerMode;
}


/************************************************************************************************************
** 函 数 名 : DebugPlatformWriteDataAnalyse
** 功能描述 : 测试平台写数据分析
** 输    入 : buf数据缓冲区首地址  Channel:串口 OR UDP
** 输    出 : 无
** 返    回 :	正确返回1 错误返回0
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
		case 0x0001://充电桩编号
			if (*(pbuf + 5) == 1){
				ParaSetPileNo(0, pbuf + 5 + 1);
				ParaSetPileNo(1, pbuf + 5 + 1);
				ret = 0;
			} else if (*(pbuf + 5) == 2){
				ParaSetPileNo(1, pbuf + 5 + 1);
				ret = 0;
			}
			break;
			
		case 0x0003://硬件版本
			ParaSetHardVersion(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0004://充电桩类型
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				ParaSetChgGunNumber(*(pbuf + 5));
				ret = 0;
			}
			break;
		
		case 0x0007://充电桩当前时间
			datetime.year = *(pbuf + 10) | ( *(pbuf + 11)<< 8);
			datetime.month = *(pbuf + 9);
			datetime.day = *(pbuf + 8);
			datetime.hour = *(pbuf + 7);
			datetime.min = *(pbuf + 6);
			datetime.sec = *(pbuf + 5);
			RtcWrite(&datetime);
			ret = 0;
			break;
		
		case 0x0008://心跳周期
			ParaSetNetHeartTime(0, *(pbuf + 5));
			ParaSetNetHeartTime(1, *(pbuf + 5));
			ret = 0;
			break;
		
		case 0x0009://广告屏开启时间
			ParaGetScreenTime(&openhour, &openmin, &closehour, &closemin);
			ParaSetScreenTime(*(pbuf + 6), *(pbuf + 5), closehour, closemin);
			ret = 0;
			break;
		
		case 0x000A://广告屏关闭时间
			ParaGetScreenTime(&openhour, &openmin, &closehour, &closemin);
			ParaSetScreenTime(openhour, openmin, *(pbuf + 6), *(pbuf + 5));
			ret = 0;
			break;
		
		case 0x000B://交流电表地址
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				ParaSetACMeterAddr(*(pbuf + 5) - 1, pbuf + 5 + 1);
				ret = 0;
			}
			break;
			
		case 0x000C://直流电表地址
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				ParaSetDCMeterAddr(*(pbuf + 5) - 1, pbuf + 5 + 1);
				ret = 0;
			}
			break;
			
		case 0x000D://客户编号
			ParaSetCustomerNumber(pbuf + 5);
			ret = 0;
			break;
			
		case 0x000E://终端复位
			if ((*(pbuf + 5) == 0) || (*(pbuf + 5) == 1)){
				//数据区复位
				DeleteAllRecord();
			}
			ret = 0;
			break;
		
		case 0x0010://注册码
			ret = 0;
			break;
		
		case 0x0011://二维码
			if ((*(pbuf + 5) == 1) || (*(pbuf + 5) == 2)){
				ParaSetQRCode(*(pbuf + 5) - 1, pbuf + 5 + 1);
				ret = 0;
			}
			break;
		
		case 0x0101://上行通道
			ParaSetUpChannel(*(pbuf + 5));
			ret = 0;
			break;
		
		case 0x0102://服务器IP
			ParaSetServerIp(0, pbuf + 5);
			ret = 0;
			break;
		
		case 0x0103://服务器端口
			ParaSetServerPort(0, *(pbuf + 5) | (*(pbuf + 6) << 8));
			ParaSetServerPort(1, *(pbuf + 5) | (*(pbuf + 6) << 8));
			ret = 0;
			break;
		
		case 0x0104://本机IP
			ParaSetLocalIp(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0105://子网掩码
			ParaSetSubMask(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0106://网关
			ParaSetGateWay(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0107://物理地址
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
		
		case 0x0201://尖峰平谷费率
			for (i = 0; i < 12; i++){
				ParaSetFeilvTime(0, i, *(pbuf + 5 + i * 8 + 1), *(pbuf + 5 + i * 8 + 0), *(pbuf + 5 + i * 8 + 3),  *(pbuf + 5 + i * 8 + 2));
				memcpy(&data32, pbuf + 5 + i * 8 + 4, 4);
				ParaSetFeilvMoney(0, i, data32);
				ParaSetFeilvTime(1, i, *(pbuf + 5 + i * 8 + 1), *(pbuf + 5 + i * 8 + 0), *(pbuf + 5 + i * 8 + 3),  *(pbuf + 5 + i * 8 + 2));
				ParaSetFeilvMoney(1, i, data32);
			}
			ret = 0;
			break;
			
		case 0x0012://生产日期
			ParaSetManufactureDate(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0013://生产基地
			ParaSetManufactureBase(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0014://检测人
			ParaSetTestWorker(pbuf + 5);
			ret = 0;
			break;
		
		case 0x0015://使用期
			ParaSetLifeTime(pbuf + 5);
			ret = 0;
			break;
		case 0x0016://激活码
			ret =ParaSetActCode(pbuf + 5);
			break;
		case 0x0017:// 涓流电流控制
			SetTrickleCur((*(pbuf + 5) | (*(pbuf + 6) << 8)));
			ret = 0;
			break;
		case 0x0018:// 涓流SOC控制
			SetTrickleSoc(*(pbuf + 5) );
			ret = 0;
			break;
		
		case 0x00D1: //电源模块类型
			ModuleType = *(pbuf + 5);
			MoudleNum = 0;
			MaxCurrent = 0;
			SetParacmd = 1;   //第8位代表是哪把枪
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
		case 0x00D2://电源模块数量
			
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
		
		case 0x00D4://最大总电流限定值
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
		case 0x00D6:// 功率分配模式
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
			memcpy(p, PileNo, 32);//桩编号
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
			memcpy(p, PileNo, 32);//桩编号
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
	
	if (DI == 0x000E){ //终端复位
		OSTimeDlyHMSM (0, 0, 1, 0);
		NVIC_SystemReset();
	}
	
	return 1;
}

/****************************************************************************\
 Function:  AnalyseMsgFromDebugPlatform
 Descript:  分析从调试平台接收到的数据
 Input:	    无
 Output:    无
 Return:    无
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
