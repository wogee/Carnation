#ifndef __DISPLAY_H
#define __DISPLAY_H
#include 	<LPC177x_8x.H>

void DispMenuInfo(uint8_t* Time, uint8_t* Online);
void DispStartDeviceInfo(uint8_t MajorVer, uint8_t MinorVer, uint8_t PatchVer);
void DispNoPileUsed(void);
void DispInsertGunInfo(void);
void DispDeviceFault(uint8_t* tempbuf);
void DispErrIdGunInfo(uint8_t port, uint8_t* IdBuf);

//刷卡相关
void DispCardLockInfo(uint8_t* Pile );
void DispUnLockFailureInfo(uint32_t FailureId );

//显示当前余额
void DispRemainMoneyInfo(uint32_t Money);
//结算界面
void DispAccount(uint32_t SumEnergy, uint32_t SumMoney, uint32_t RemainMoney, uint32_t SumTime, uint8_t* Date);

//在线卡验证相关界面
void DispInVerify(void);
void DispVerifySuccesInfo(uint32_t Money);
void DispVerifyFailure(uint8_t* tempbuf);

//启动相关界面
void DispStartChgInfo(uint32_t Time);
void DispOperationInfo(void);
void DispStartSucessInfo(void);
void DispStartFailureInfo(void);

//升级相关
void DispUpdataInfo( uint8_t Present,uint8_t* Tip);

//单枪处理相关界面
void DispAIdleInfo(uint8_t* ACode, uint8_t* APileNum, uint8_t GunStatus);
void DispAChargeInfo(uint32_t SumEnergy, uint32_t SumMoney, uint32_t CurrentA, uint32_t VoltageA, uint32_t SumTime,uint8_t SoC);
void DispAStopInfo(uint8_t* Prompt, uint8_t* StpReason, uint32_t SumEnergy, uint32_t SumMoney );




//双枪处理相关界面
void DispAIdleBIdleInfo( uint8_t* ACode, uint8_t* APileNum, uint8_t AGunStatus, \
							           uint8_t* BCode, uint8_t* BPileNum, uint8_t BGunStatus);

void DispAIdleBChgInfo( uint8_t* ACode, uint8_t* APileNum, uint8_t AGunStatus,  \
												uint32_t BSumEnergy, uint32_t BSumMoney, uint32_t BCurrent, uint32_t BVoltage, uint32_t BSumTime, uint8_t BSoc);

void DispAIdleBStopInfo( uint8_t* ACode, uint8_t* APileNum, uint8_t AGunStatus, \
												 uint8_t* BPrompt, uint8_t* BStpReason, uint32_t BSumEnergy, uint32_t BSumMoney );

void DispAChgBIdleInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrent, uint32_t AVoltage,  uint32_t ASumTime,uint8_t ASoc, \
											  uint8_t* BCode, uint8_t* BPileNum, uint8_t BGunStatus);

void DispAChgBChgInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrent, uint32_t AVoltage,  uint32_t ASumTime, uint8_t ASoc,\
											 uint32_t BSumEnergy, uint32_t BSumMoney, uint32_t BCurrent, uint32_t BVoltage,  uint32_t BSumTime, uint8_t BSoc);

void DispAChgBStopInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrent, uint32_t AVoltage,  uint32_t ASumTime,uint8_t ASoc, \
												uint8_t* BPrompt, uint8_t* BStpReason, uint32_t BSumEnergy, uint32_t BSumMoney );

void DispAStopBChgInfo( uint8_t* APrompt, uint8_t* AStpReason, uint32_t ASumEnergy, uint32_t ASumMoney, \
												uint32_t BSumEnergy, uint32_t BSumMoney, uint32_t BCurrent, uint32_t BVoltage,  uint32_t BSumTime,uint8_t BSoc );

void DispAStopBStopInfo( uint8_t* APrompt, uint8_t* AStpReason, uint32_t ASumEnergy, uint32_t ASumMoney, \
												 uint8_t* BPrompt, uint8_t* BStpReason, uint32_t BSumEnergy, uint32_t BSumMoney );

void DispAStopBIdleInfo( uint8_t* APrompt, uint8_t* AStpReason, uint32_t ASumEnergy, uint32_t ASumMoney, \
										     uint8_t* BCode, uint8_t* BPileNum, uint8_t BGunStatus);

void DispRecordInfo(uint8_t StartType,uint32_t StartCardID,uint8_t* StartDateTime,uint32_t SumTime, uint32_t SumEnergy,uint32_t SumMoney,uint8_t StopCause, uint8_t Line );

void DispRecordNullInfo(uint8_t i );

void DispRecordMoreInfo(uint8_t ChgPort,uint8_t StartType,uint32_t StartCardID,uint8_t* StartDateTime, uint8_t* StopDateTime,uint32_t SumTime, uint32_t SumEnergy,uint32_t StartMoney, uint32_t SumMoney,uint8_t StopCause,uint8_t StartSoc, uint8_t StopSoc );

void DispMoreMsgInfo(uint8_t ChgMode, uint8_t BatType,uint32_t NeedVol,uint32_t NeedCur, uint32_t AbiVol, uint32_t AbiBat,uint32_t MaxAllowVol,
										uint32_t MaxTemp, uint8_t* Version, uint32_t MaxVol,uint32_t LostTime,uint8_t BackTime);

void DispSwitchChgWayInfo(uint8_t* PowerType, uint8_t Mode, uint32_t* Para);

#endif




