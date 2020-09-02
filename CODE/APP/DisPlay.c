#include "xrd.h"
#include "MyAlgLib.h"
#include "MainTask.h"
#include "Record.h"
#include "DisPlay.h"
#include "Para.h"
/************************************************************************************************************
** 函 数 名 : DispMenuInfo
** 功能描述 : 显示菜单相关信息
** 输    入 : Time  时间指针  Online  在线标识
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispMenuInfo(uint8_t* Time, uint8_t* Online)
{

	XRD_Write_Addr(PAGE_All_TIME, (char *)Time, PAGE_All_TIME_LEN);//时间
	XRD_Write_Addr(PAGE_ON_LINEFLAG, (char *)Online, PAGE_ON_LINEFLAG_LEN);//在线标志
}


/************************************************************************************************************
** 函 数 名 : DispStartDeviceInfo
** 功能描述 : 显示设备启动中界面
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispStartDeviceInfo(uint8_t MajorVer, uint8_t MinorVer, uint8_t PatchVer)
{
	uint8_t tempbuf[64]={0};
	uint32_t day;
	sprintf((char *)tempbuf, "VER:%d.%d.%d",MajorVer, MinorVer, PatchVer);
	XRD_Write_Addr(PAGE0_VER_ADDR, (char *)tempbuf, PAGE0_VER_LEN);
	XRD_Write_Addr(PAGE0_DAY_ADDR, (char *)"    ", PAGE0_DAY_ADDR_LEN );
	if(ParaGetLimitHour()){
		if(ParaGetRunHour() >= ParaGetLimitHour()){
			sprintf((char *)tempbuf, "请激活");
		}else{
			day = (ParaGetLimitHour() -ParaGetRunHour()) /24;
			sprintf((char *)tempbuf, "使用剩余%d天", day);
		}
		XRD_Write_Addr(PAGE0_DAY_ADDR, (char *)tempbuf, PAGE0_DAY_ADDR_LEN );
	}
	XRD_Load_Page(PAGE0);
}



/************************************************************************************************************
** 函 数 名 : DispErrIdGunInfo
** 功能描述 : 显示枪故障ID
** 输    入 : port  A枪B枪   IDBuf 故障ID指针
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispErrIdGunInfo(uint8_t port, uint8_t* IdBuf)
{
	if(port == 0){
		XRD_Write_Addr(PAGE_A_ERRID, (char *)IdBuf, PAGE_A_ERRID_LEN);
	}else {
		XRD_Write_Addr(PAGE_B_ERRID, (char *)IdBuf, PAGE_B_ERRID_LEN);
	}
}
	






/************************************************************************************************************
** 函 数 名 : DispAccount
** 功能描述 : 显示结算界面
** 输    入 : SumEnergy    充电电量           0.01kwh
**						SumMoney     花费金额           0.01元
**            RemainMoney   余额              0.01元  
**            SumTime      充电时长           S  多少秒
**            Date          日期指针        
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAccount(uint32_t SumEnergy, uint32_t SumMoney, uint32_t RemainMoney, uint32_t SumTime, uint8_t* Date)
{
	uint8_t tempbuf[128]={0};
	
	sprintf((char *)tempbuf, "%d.%02d", SumEnergy / 100, SumEnergy % 100);
	XRD_Write_Addr(PAGE14_AB_POWER, (char *)tempbuf, PAGE14_AB_POWER_LEN);//AB当前电量	
	sprintf((char *)tempbuf, "%d.%02d", SumMoney / 100, SumMoney % 100);
	XRD_Write_Addr(PAGE14_AB_COST, (char *)tempbuf, PAGE14_AB_COST_LEN);//本次消费金额
	sprintf((char *)tempbuf, "%d.%02d", RemainMoney / 100, RemainMoney % 100);
	XRD_Write_Addr(PAGE14_AB_REMAIN, (char *)tempbuf, PAGE14_AB_REMAIN_LEN);//余额
	sprintf((char *)tempbuf, "%d Min", SumTime / 60);
	XRD_Write_Addr(PAGE14_AB_CH_TIME, (char *)tempbuf, PAGE14_AB_CH_TIME_LEN);//总耗时
	sprintf((char *)tempbuf, "20%02d-%02d-%02d %02d:%02d:%02d", Date[0], Date[1], Date[2], Date[3], Date[4],Date[5]);
	XRD_Write_Addr(PAGE14_AB_DATE, (char *)tempbuf, PAGE14_AB_DATE_LEN);//日期
	XRD_Load_Page(PAGE14);
}



/************************************************************************************************************
** 函 数 名 : DispInsertGunInfo
** 功能描述 : 显示请插枪界面
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispInsertGunInfo(void)
{
		XRD_Load_Page(PAGE10);
}



/************************************************************************************************************
** 函 数 名 : DispDeviceFault
** 功能描述 : 显示电桩故障界面
** 输    入 : tempbuf    故障ID buf
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispDeviceFault(uint8_t* tempbuf)
{

	XRD_Write_Addr(PAGE19_AB_ERR,(char *)tempbuf,PAGE19_AB_ERR_LEN);
	XRD_Load_Page(PAGE19);     
}




/************************************************************************************************************
** 函 数 名 : DispCardLockInfo
** 功能描述 : 显示锁卡界面
** 输    入 : Pile    桩编号字符串
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispCardLockInfo(uint8_t* Pile )
{
	XRD_Write_Addr(PAGE15_AB_LOCK, (char *)Pile, PAGE15_AB_LOCK_LEN);	
	XRD_Load_Page(PAGE15); 			
}



/************************************************************************************************************
** 函 数 名 : DispUnLockFailureInfo
** 功能描述 : 显示解锁失败界面
** 输    入 : FailureId    失败ID
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispUnLockFailureInfo(uint32_t FailureId )
{
	uint8_t tempbuf[56]={0};
	
	sprintf((char *)tempbuf, "%02d ", FailureId);
	XRD_Write_Addr(PAGE16_AB_FAILURE_ID,(char *)tempbuf, PAGE16_AB_FAILURE_ID_LEN);	
	XRD_Load_Page(PAGE16); 				
}


/************************************************************************************************************
** 函 数 名 : DispNoPileUsed
** 功能描述 : 显示无空闲充电位
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispNoPileUsed(void)
{
	XRD_Load_Page(PAGE17); 				
}



/************************************************************************************************************
** 函 数 名 : DispRemainMoneyInfo
** 功能描述 : 显示余额界面
** 输    入 : Money     0.01元
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispRemainMoneyInfo(uint32_t Money)
{
	uint8_t tempbuf[56]={0};
	
	sprintf((char *)tempbuf,"%d.%02d", Money/100, Money%100);
	XRD_Write_Addr(PAGE18_AB_CURRENT_MONEY, (char *)tempbuf, PAGE18_AB_CURRENT_MONEY_LEN);
	XRD_Load_Page(PAGE18); 
}
	


/************************************************************************************************************
** 函 数 名 : DispInVerify
** 功能描述 : 显示验证中界面
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispInVerify(void)
{
	XRD_Load_Page(PAGE23);
}



/************************************************************************************************************
** 函 数 名 : DispVerifySuccesInfo
** 功能描述 : 显示验证成功界面
** 输    入 : Money   当前余额   0.01元
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispVerifySuccesInfo(uint32_t Money)
{
	uint8_t tempbuf[56]={0};
	
	sprintf((char *)tempbuf,"%d.%02d", Money/100, Money%100);
	XRD_Write_Addr(PAGE24_MONEY, (char *)tempbuf, PAGE24_MONEY_LEN);
	XRD_Load_Page(PAGE24); 
}



/************************************************************************************************************
** 函 数 名 : DispVerifyFailure
** 功能描述 : 显示验证失败界面
** 输    入 : tempbuf   失败原因指针
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispVerifyFailure(uint8_t* tempbuf)
{
	
	XRD_Write_Addr(PAGE25_FAILURE, (char *)tempbuf, PAGE25_FAILURE_LEN);
	XRD_Load_Page(PAGE25);
}


/************************************************************************************************************
** 函 数 名 : DispStartChgInfo
** 功能描述 : 显示启动中界面
** 输    入 : Time   启动时间    单位S 秒
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispStartChgInfo(uint32_t Time)
{
	uint8_t tempbuf[56]={0};
	
	sprintf((char *)tempbuf,"%02d", Time);
//	XRD_Write_Addr(PAGE23_TIME, (char *)tempbuf, PAGE23_TIME_LEN);
	XRD_Load_Page(PAGE26); 
}


/************************************************************************************************************
** 函 数 名 : DispOperationInfo
** 功能描述 : 显示操作中界面
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispOperationInfo(void)
{
	XRD_Load_Page(PAGE27); 	
}

/************************************************************************************************************
** 函 数 名 : DispStartSucessInfo
** 功能描述 : 显示启动成功界面
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispStartSucessInfo(void)
{
	XRD_Load_Page(PAGE28); 
}


/************************************************************************************************************
** 函 数 名 : DispStartFailureInfo
** 功能描述 : 显示启动失败界面
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispStartFailureInfo(void)
{
	XRD_Load_Page(PAGE29); 	
}






/************************************************************************************************************
******************************                单枪处理部分             **************************************
*************************************************************************************************************
*/

/************************************************************************************************************
** 函 数 名 : DispAIdleInfo
** 功能描述 : 显示A单枪待机界面
** 输    入 : ACode   A枪二维码指针    APileNum  A枪编号字符指针  GunStatus  A插枪状态
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAIdleInfo(uint8_t* ACode, uint8_t* APileNum, uint8_t GunStatus)
{
	
	XRD_Write_Addr(PAGE20_A_DEVNUM, (char *)APileNum, PAGE20_A_DEVNUM_LEN);			
	XRD_Write_Addr(PAGE20_A_QR, (char *)ACode, PAGE20_A_QR_LEN);	
	XRD_Draw_Ioc(PAGE20_A_INSERTFLAG, GunStatus);
	XRD_Load_Page(PAGE20);
}



/************************************************************************************************************
** 函 数 名 : DispAChargeInfo
** 功能描述 : 显示A单枪充电界面
** 输    入 : SumEnergy   A当前电量  0.01kwh
**						SumMoney    A当前花费  0.01元
**            CurrentA    A电流      0.001 含3位小数
**            VoltageA    A电压      0.1  V
**            SumTime     A充电时间  S  多少秒
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAChargeInfo(uint32_t SumEnergy, uint32_t SumMoney, uint32_t CurrentA, uint32_t VoltageA, uint32_t SumTime,uint8_t SoC)
{
	char tempbuf[56]={0};
	sprintf((char *)tempbuf, "%02d%%", SoC);
	XRD_Write_Addr(PAGE21_A_PERCENTAGE, (char *)tempbuf, PAGE21_A_PERCENTAGE_LEN);//A当前SOC
	
	sprintf((char *)tempbuf, "%d.%02d", SumEnergy / 100, SumEnergy % 100);
	XRD_Write_Addr(PAGE21_A_POWER, (char *)tempbuf, PAGE21_A_POWER_LEN);//A当前电量
	sprintf((char *)tempbuf, "%d.%02d", SumMoney / 100, SumMoney % 100);
	XRD_Write_Addr(PAGE21_A_COST, (char *)tempbuf, PAGE21_A_COST_LEN);//A当前花费
	sprintf((char *)tempbuf, "%d.%02d", CurrentA / 1000, (CurrentA % 1000)/10);
	XRD_Write_Addr(PAGE21_A_I, (char *)tempbuf, PAGE21_A_I_LEN);//A电流
	sprintf((char *)tempbuf, "%d.%d", VoltageA / 10, VoltageA % 10);
	XRD_Write_Addr(PAGE21_A_VOLTAGE, (char *)tempbuf, PAGE21_A_VOLTAGE_LEN);//A电压
	sprintf((char *)tempbuf, "%02d:%02d:%02d", SumTime / 3600, (SumTime % 3600)/60, ((SumTime % 3600)%60) );
	XRD_Write_Addr(PAGE21_A_CH_TIME, (char *)tempbuf, PAGE21_A_CH_TIME_LEN);//A充电时间
	XRD_Load_Page(PAGE21);
}




/************************************************************************************************************
** 函 数 名 : DispAStopInfo
** 功能描述 : 显示A单枪停止界面
** 输    入 : Prompt     	A提示字符串指针
**						StpReason   A停止原因字符指针
**            SumEnergy   A充电电量          0.01kwh
**            SumMoney    A充电金额          0.01元
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAStopInfo(uint8_t* Prompt, uint8_t* StpReason, uint32_t SumEnergy, uint32_t SumMoney )
{
	char tempbuf[56]={0};
	XRD_Write_Addr(PAGE22_A_STOP_REASON, (char *)StpReason, PAGE22_A_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE22_A_CHARG_PROMPT, (char *)Prompt, PAGE22_A_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", SumEnergy / 100, SumEnergy % 100);
	XRD_Write_Addr(PAGE22_A_CHARG_POWER,(char *)tempbuf,PAGE22_A_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", SumMoney / 100, SumMoney % 100);;											
	XRD_Write_Addr(PAGE22_A_CHARG_MONEY,(char *)tempbuf,PAGE22_A_CHARG_MONEY_LEN);	
	XRD_Load_Page(PAGE22); 
}



/************************************************************************************************************
******************************                双枪处理部分             **************************************
*************************************************************************************************************
*/

/************************************************************************************************************
** 函 数 名 : DispAIdleBIdleInfo
** 功能描述 : 显示A待机B待机界面
** 输    入 :  ACode   A枪二维码指针    APileNum  A枪编号字符指针  AGunStatus  A插枪状态  
**          :  BCode   B枪二维码指针    BPileNum  B枪编号字符指针  BGunStatus  B插枪状态   
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAIdleBIdleInfo( uint8_t* ACode, uint8_t* APileNum, uint8_t AGunStatus, \
							           uint8_t* BCode, uint8_t* BPileNum, uint8_t BGunStatus)
{
	XRD_Write_Addr(PAGE1_A_DEVNUM, (char *)APileNum, PAGE1_A_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE1_A_QR, (char *)ACode, PAGE1_A_QR_LEN);	
	XRD_Draw_Ioc(PAGE1_A_INSERTFLAG, AGunStatus);
	
	XRD_Write_Addr(PAGE1_B_DEVNUM, (char *)BPileNum, PAGE1_B_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE1_B_QR, (char *)BCode, PAGE1_B_QR_LEN);	
	XRD_Draw_Ioc(PAGE1_B_INSERTFLAG, BGunStatus);
	XRD_Load_Page(PAGE1);
}



/************************************************************************************************************
** 函 数 名 : DispAChgBIdleInfo
** 功能描述 : 显示A充电B待机界面
** 输    入 : ASumEnergy   A当前电量   0.01kwh
**						ASumMoney    A当前花费   0.01元
**            ACurrent     A电流       0.001 含3位小数
**            AVoltage     A电压       0.1  V
**            ASumTime     A充电时间    S  多少秒
**            ASoc         A枪 SoC
**            BCode   B枪二维码指针    BPileNum  B枪编号字符指针  BGunStatus  B插枪状态   
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAChgBIdleInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrent, uint32_t AVoltage,  uint32_t ASumTime,uint8_t ASoc, \
											  uint8_t* BCode, uint8_t* BPileNum, uint8_t BGunStatus)
{
	char tempbuf[56]={0};
	
	sprintf((char *)tempbuf, "%02d%%", ASoc);
	XRD_Write_Addr(PAGE8_A_PERCENTAGE, (char *)tempbuf, PAGE8_A_PERCENTAGE_LEN);//A当前电量
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE8_A_POWER, (char *)tempbuf, PAGE8_A_POWER_LEN);//A当前电量
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);
	XRD_Write_Addr(PAGE8_A_COST, (char *)tempbuf, PAGE8_A_COST_LEN);//A当前花费
	sprintf((char *)tempbuf, "%d.%02d", ACurrent / 1000, (ACurrent % 1000)/10);
	XRD_Write_Addr(PAGE8_A_I, (char *)tempbuf, PAGE8_A_I_LEN);//A电流
	sprintf((char *)tempbuf, "%d.%d", AVoltage / 10, AVoltage % 10);
	XRD_Write_Addr(PAGE8_A_VOLTAGE, (char *)tempbuf, PAGE8_A_VOLTAGE_LEN);//A电压
	sprintf((char *)tempbuf, "%02d:%02d:%02d", ASumTime / 3600, (ASumTime % 3600)/60, ((ASumTime % 3600)%60) );
	XRD_Write_Addr(PAGE8_A_CH_TIME, (char *)tempbuf, PAGE8_A_CH_TIME_LEN);//A充电时间
		
	XRD_Write_Addr(PAGE8_B_DEVNUM, (char *)BPileNum, PAGE8_B_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE8_B_QR, (char *)BCode, PAGE8_B_QR_LEN);	
	XRD_Draw_Ioc(PAGE8_B_INSERTFLAG, BGunStatus);
	XRD_Load_Page(PAGE8);	
}

/************************************************************************************************************
** 函 数 名 : DispAStopBIdle
** 功能描述 : 显示A枪停止B待机界面
** 输    入 : APrompt      A提示字符串指针
**						AStpReason   A停止原因字符指针
**            ASumEnergy   A充电电量          0.01kwh
**            ASumMoney    A充电金额          0.01元
**            BCode   B枪二维码指针    BPileNum  B枪编号字符指针  BGunStatus  B插枪状态  
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAStopBIdleInfo( uint8_t* APrompt, uint8_t* AStpReason, uint32_t ASumEnergy, uint32_t ASumMoney, \
										     uint8_t* BCode, uint8_t* BPileNum, uint8_t BGunStatus)
{
	char tempbuf[56]={0};
	
	XRD_Write_Addr(PAGE4_A_STOP_REASON, (char *)AStpReason, PAGE4_A_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE4_A_CHARG_PROMPT, (char *)APrompt, PAGE4_A_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE4_A_CHARG_POWER,(char *)tempbuf, PAGE4_A_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);;											
	XRD_Write_Addr(PAGE4_A_CHARG_MONEY,(char *)tempbuf, PAGE4_A_CHARG_MONEY_LEN);	
	
	XRD_Write_Addr(PAGE4_B_DEVNUM, (char *)BPileNum, PAGE4_B_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE4_B_QR, (char *)BCode, PAGE4_B_QR_LEN);	
	XRD_Draw_Ioc(PAGE4_B_INSERTFLAG, BGunStatus);
	XRD_Load_Page(PAGE4);	
}

/************************************************************************************************************
** 函 数 名 : DispAIdleBChgInfo
** 功能描述 : 显示A待机B充电中界面
** 输    入 :  ACode   A枪二维码指针    APileNum  A枪编号字符指针  AGunStatus  A插枪状态  
**             BSumEnergy   B当前电量   0.01kwh
**						 BSumMoney    B当前花费   0.01元
**             BCurrent     B电流       0.001 含3位小数
**             BVoltage     B电压       0.1  V
**             BSumTime     B充电时间    S  多少秒  BSoc
**             BSoc         BSoc
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAIdleBChgInfo( uint8_t* ACode, uint8_t* APileNum, uint8_t AGunStatus,  \
												uint32_t BSumEnergy, uint32_t BSumMoney, uint32_t BCurrent, uint32_t BVoltage, uint32_t BSumTime, uint8_t BSoc)
{
	char tempbuf[56]={0};
	
	XRD_Write_Addr(PAGE3_A_DEVNUM, (char *)APileNum, PAGE3_A_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE3_A_QR, (char *)ACode, PAGE3_A_QR_LEN);	
	XRD_Draw_Ioc(PAGE3_A_INSERTFLAG, AGunStatus);

	sprintf((char *)tempbuf, "%02d%%", BSoc);
	XRD_Write_Addr(PAGE3_B_PERCENTAGE, (char *)tempbuf, PAGE3_B_PERCENTAGE_LEN);//B当前电量	
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE3_B_POWER, (char *)tempbuf, PAGE3_B_POWER_LEN);//B当前电量
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);
	XRD_Write_Addr(PAGE3_B_COST, (char *)tempbuf, PAGE3_B_COST_LEN);//B当前花费
	sprintf((char *)tempbuf, "%d.%02d", BCurrent / 1000, (BCurrent % 1000)/10);
	XRD_Write_Addr(PAGE3_B_I, (char *)tempbuf, PAGE3_B_I_LEN);//B电流
	sprintf((char *)tempbuf, "%d.%d", BVoltage / 10, BVoltage % 10);
	XRD_Write_Addr(PAGE3_B_VOLTAGE, (char *)tempbuf, PAGE3_B_VOLTAGE_LEN);//B电压
	sprintf((char *)tempbuf, "%02d:%02d:%02d", BSumTime / 3600, (BSumTime % 3600)/60, ((BSumTime % 3600)%60) );
	XRD_Write_Addr(PAGE3_B_CH_TIME, (char *)tempbuf, PAGE3_B_CH_TIME_LEN);//B充电时间
	XRD_Load_Page(PAGE3);	
}


/************************************************************************************************************
** 函 数 名 : DispAIdleBStop
** 功能描述 : 显示A待机B充电停止界面
** 输    入 :  ACode   A枪二维码指针    APileNum  A枪编号字符指针  AGunStatus  A插枪状态  
**             BPrompt      B提示字符串指针
**						 BStpReason   B停止原因字符指针
**             BSumEnergy   B充电电量          0.01kwh
**             BSumMoney    B充电金额          0.01元
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAIdleBStopInfo( uint8_t* ACode, uint8_t* APileNum, uint8_t AGunStatus, \
												 uint8_t* BPrompt, uint8_t* BStpReason, uint32_t BSumEnergy, uint32_t BSumMoney )
{
	char tempbuf[56]={0};
	
	XRD_Write_Addr(PAGE2_A_DEVNUM, (char *)APileNum, PAGE2_A_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE2_A_QR, (char *)ACode, PAGE2_A_QR_LEN);	
	XRD_Draw_Ioc(PAGE2_A_INSERTFLAG, AGunStatus);
	
	XRD_Write_Addr(PAGE2_B_STOP_REASON, (char *)BStpReason, PAGE2_B_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE2_B_CHARG_PROMPT, (char *)BPrompt, PAGE2_B_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE2_B_CHARG_POWER,(char *)tempbuf, PAGE2_B_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);;											
	XRD_Write_Addr(PAGE2_B_CHARG_MONEY,(char *)tempbuf, PAGE2_B_CHARG_MONEY_LEN);	
	XRD_Load_Page(PAGE2);	
	
}










/************************************************************************************************************
** 函 数 名 : DispAStopBChgInfo
** 功能描述 : 显示A充电停止B充电中界面
** 输    入 :  APrompt      A提示字符串指针
**						 AStpReason   A停止原因字符指针
**             ASumEnergy   A充电电量          0.01kwh
**             ASumMoney    A充电金额          0.01元
**             BSumEnergy   B当前电量          0.01kwh
**						 BSumMoney    B当前花费          0.01元
**             BCurrent     B电流              0.001 含3位小数
**             BVoltage     B电压              0.1  V
**             BSumTime     B充电时间          S  多少秒  
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAStopBChgInfo( uint8_t* APrompt, uint8_t* AStpReason, uint32_t ASumEnergy, uint32_t ASumMoney, \
												uint32_t BSumEnergy, uint32_t BSumMoney, uint32_t BCurrent, uint32_t BVoltage,  uint32_t BSumTime,uint8_t BSoc )
{
	char tempbuf[56]={0};
	
	XRD_Write_Addr(PAGE5_A_STOP_REASON, (char *)AStpReason, PAGE5_A_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE5_A_CHARG_PROMPT, (char *)APrompt, PAGE5_A_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE5_A_CHARG_POWER,(char *)tempbuf, PAGE5_A_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);;											
	XRD_Write_Addr(PAGE5_A_CHARG_MONEY,(char *)tempbuf, PAGE5_A_CHARG_MONEY_LEN);	

	sprintf((char *)tempbuf, "%02d%%", BSoc);
	XRD_Write_Addr(PAGE5_B_PERCENTAGE, (char *)tempbuf, PAGE5_B_PERCENTAGE_LEN);
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE5_B_POWER, (char *)tempbuf, PAGE5_B_POWER_LEN);//B当前电量
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);
	XRD_Write_Addr(PAGE5_B_COST, (char *)tempbuf, PAGE5_B_COST_LEN);//B当前花费
	sprintf((char *)tempbuf, "%d.%02d", BCurrent / 1000, (BCurrent % 1000)/10);
	XRD_Write_Addr(PAGE5_B_I, (char *)tempbuf, PAGE5_B_I_LEN);//B电流
	sprintf((char *)tempbuf, "%d.%d", BVoltage / 10, BVoltage % 10);
	XRD_Write_Addr(PAGE5_B_VOLTAGE, (char *)tempbuf, PAGE5_B_VOLTAGE_LEN);//B电压
	sprintf((char *)tempbuf, "%02d:%02d:%02d", BSumTime / 3600, (BSumTime % 3600)/60, ((BSumTime % 3600)%60) );
	XRD_Write_Addr(PAGE5_B_CH_TIME, (char *)tempbuf, PAGE5_B_CH_TIME_LEN);//B充电时间
	XRD_Load_Page(PAGE5);	
}

/************************************************************************************************************
** 函 数 名 : DispAChgBStopInfo
** 功能描述 : 显示A充电中B充电停止界面
** 输    入 :  ASumEnergy   A当前电量           0.01kwh
**						 ASumMoney    A当前花费           0.01元
**             ACurrent     A电流               0.001 含3位小数
**             AVoltage     A电压               0.1  V
**             ASumTime     A充电时间           S  多少秒
**             BPrompt      B提示字符串指针
**						 BStpReason   B停止原因字符指针
**             BSumEnergy   B充电电量           0.01kwh
**             BSumMoney    B充电金额           0.01元
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAChgBStopInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrent, uint32_t AVoltage,  uint32_t ASumTime,uint8_t ASoc, \
												uint8_t* BPrompt, uint8_t* BStpReason, uint32_t BSumEnergy, uint32_t BSumMoney )
{
	char tempbuf[56]={0};
	
	sprintf((char *)tempbuf, "%02d%%", ASoc);
	XRD_Write_Addr(PAGE7_A_PERCENTAGE, (char *)tempbuf, PAGE7_A_PERCENTAGE_LEN);
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE7_A_POWER, (char *)tempbuf, PAGE7_A_POWER_LEN);//A当前电量
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);
	XRD_Write_Addr(PAGE7_A_COST, (char *)tempbuf, PAGE7_A_COST_LEN);//A当前花费
	sprintf((char *)tempbuf, "%d.%02d", ACurrent / 1000, (ACurrent % 1000)/10);
	XRD_Write_Addr(PAGE7_A_I, (char *)tempbuf, PAGE7_A_I_LEN);//A电流
	sprintf((char *)tempbuf, "%d.%d", AVoltage / 10, AVoltage % 10);
	XRD_Write_Addr(PAGE7_A_VOLTAGE, (char *)tempbuf, PAGE7_A_VOLTAGE_LEN);//A电压
	sprintf((char *)tempbuf, "%02d:%02d:%02d", ASumTime / 3600, (ASumTime % 3600)/60, ((ASumTime % 3600)%60) );
	XRD_Write_Addr(PAGE7_A_CH_TIME, (char *)tempbuf, PAGE7_A_CH_TIME_LEN);//A充电时间
	
	XRD_Write_Addr(PAGE7_B_STOP_REASON, (char *)BStpReason, PAGE7_B_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE7_B_CHARG_PROMPT, (char *)BPrompt, PAGE7_B_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE7_B_CHARG_POWER,(char *)tempbuf, PAGE7_B_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);;											
	XRD_Write_Addr(PAGE7_B_CHARG_MONEY,(char *)tempbuf, PAGE7_B_CHARG_MONEY_LEN);	
	XRD_Load_Page(PAGE7);	
	
}


/************************************************************************************************************
** 函 数 名 : DispAChgBChgInfo
** 功能描述 : 显示A充电中B充电中界面
** 输    入 :  ASumEnergy   A当前电量           0.01kwh
**						 ASumMoney    A当前花费           0.01元
**             ACurrent     A电流               0.001 含3位小数
**             AVoltage     A电压               0.1  V
**             ASumTime     A充电时间           S  多少秒
**             BSumEnergy   B当前电量           0.01kwh
**						 BSumMoney    B当前花费           0.01元
**             BCurrent     B电流               0.001 含3位小数
**             BVoltage     B电压               0.1  V
**             BSumTime     B充电时间           S  多少秒  
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAChgBChgInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrent, uint32_t AVoltage,  uint32_t ASumTime, uint8_t ASoc,\
											 uint32_t BSumEnergy, uint32_t BSumMoney, uint32_t BCurrent, uint32_t BVoltage,  uint32_t BSumTime, uint8_t BSoc)
{
	char tempbuf[56]={0};
	
	sprintf((char *)tempbuf, "%02d%%", ASoc);
	XRD_Write_Addr(PAGE9_A_PERCENTAGE, (char *)tempbuf, PAGE9_A_PERCENTAGE_LEN);
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE9_A_POWER, (char *)tempbuf, PAGE9_A_POWER_LEN);//A当前电量
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);
	XRD_Write_Addr(PAGE9_A_COST, (char *)tempbuf, PAGE9_A_COST_LEN);//A当前花费
	sprintf((char *)tempbuf, "%d.%02d", ACurrent / 1000, (ACurrent % 1000)/10);
	XRD_Write_Addr(PAGE9_A_I, (char *)tempbuf, PAGE9_A_I_LEN);//A电流
	sprintf((char *)tempbuf, "%d.%d", AVoltage / 10, AVoltage % 10);
	XRD_Write_Addr(PAGE9_A_VOLTAGE, (char *)tempbuf, PAGE9_A_VOLTAGE_LEN);//A电压
	sprintf((char *)tempbuf, "%02d:%02d:%02d", ASumTime / 3600, (ASumTime % 3600)/60, ((ASumTime % 3600)%60) );
	XRD_Write_Addr(PAGE9_A_CH_TIME, (char *)tempbuf, PAGE9_A_CH_TIME_LEN);//A充电时间
	
	sprintf((char *)tempbuf, "%02d%%", BSoc);
	XRD_Write_Addr(PAGE9_B_PERCENTAGE, (char *)tempbuf, PAGE9_B_PERCENTAGE_LEN);
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE9_B_POWER, (char *)tempbuf, PAGE9_B_POWER_LEN);//B当前电量
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);
	XRD_Write_Addr(PAGE9_B_COST, (char *)tempbuf, PAGE9_B_COST_LEN);//B当前花费
	sprintf((char *)tempbuf, "%d.%02d", BCurrent / 1000, (BCurrent % 1000)/10);
	XRD_Write_Addr(PAGE9_B_I, (char *)tempbuf, PAGE9_B_I_LEN);//B电流
	sprintf((char *)tempbuf, "%d.%d", BVoltage / 10, BVoltage % 10);
	XRD_Write_Addr(PAGE9_B_VOLTAGE, (char *)tempbuf, PAGE9_B_VOLTAGE_LEN);//B电压
	sprintf((char *)tempbuf, "%02d:%02d:%02d", BSumTime / 3600, (BSumTime % 3600)/60, ((BSumTime % 3600)%60) );
	XRD_Write_Addr(PAGE9_B_CH_TIME, (char *)tempbuf, PAGE9_B_CH_TIME_LEN);//B充电时间
	
	XRD_Load_Page(PAGE9);
}




/************************************************************************************************************
** 函 数 名 : DispAStopBStopInfo
** 功能描述 : 显示A停止B停止界面
** 输    入 :  APrompt      A提示字符串指针
**						 AStpReason   A停止原因字符指针
**             ASumEnergy   A充电电量            0.01kwh
**             ASumMoney    A充电金额            0.01元
**             BPrompt      B提示字符串指针
**						 BStpReason   B停止原因字符指针
**             BSumEnergy   B充电电量            0.01kwh
**             BSumMoney    B充电金额            0.01元
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAStopBStopInfo( uint8_t* APrompt, uint8_t* AStpReason, uint32_t ASumEnergy, uint32_t ASumMoney, \
												 uint8_t* BPrompt, uint8_t* BStpReason, uint32_t BSumEnergy, uint32_t BSumMoney )
{	
	char tempbuf[56]={0};
	
	XRD_Write_Addr(PAGE6_A_STOP_REASON, (char *)AStpReason, PAGE6_A_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE6_A_CHARG_PROMPT, (char *)APrompt, PAGE6_A_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE6_A_CHARG_POWER,(char *)tempbuf, PAGE6_A_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);;											
	XRD_Write_Addr(PAGE6_A_CHARG_MONEY,(char *)tempbuf, PAGE6_A_CHARG_MONEY_LEN);	
		
	XRD_Write_Addr(PAGE6_B_STOP_REASON, (char *)BStpReason, PAGE6_B_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE6_B_CHARG_PROMPT, (char *)BPrompt, PAGE6_B_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE6_B_CHARG_POWER,(char *)tempbuf, PAGE6_B_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);;											
	XRD_Write_Addr(PAGE6_B_CHARG_MONEY,(char *)tempbuf, PAGE6_B_CHARG_MONEY_LEN);	
	XRD_Load_Page(PAGE6);	
}




/************************************************************************************************************
** 函 数 名 : DispRecordInfo
** 功能描述 : 显示充电记录的第几行
** 输    入 :  StartType    		启动方式
**						 StartCardID   		卡号/ID
**             StartDateTime 	  开始时间
**             SumTime    			充电时长
**             SumEnergy        充电电量
**						 SumMoney         花费金额
**             StopCause        停止原因
**             Line             第几行
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispRecordInfo(uint8_t StartType,uint32_t StartCardID,uint8_t* StartDateTime,uint32_t SumTime, uint32_t SumEnergy,uint32_t SumMoney, uint8_t StopCause, uint8_t Line )
{
	uint8_t tempbuf[56]={0};
	if(StartType == 1){
		sprintf((char *)tempbuf, "%05d", StartCardID);
	}else {
		u32tobcdnomstr1(StartCardID,tempbuf, 12);
		sprintf((char *)tempbuf, "%d%d%d%d%d%d%d%d%d%d", tempbuf[2], tempbuf[3], tempbuf[4], tempbuf[5], tempbuf[6], tempbuf[7], tempbuf[8], tempbuf[9], tempbuf[10], tempbuf[11]);
	}
	XRD_Write_Addr(PAGE31_BASE_RECORD_ID+(PAGE31_STEP_LENGTH * Line),(char *)tempbuf, PAGE31_BASE_RECORD_ID_LEN);			
	sprintf((char *)tempbuf, "%02d-%02d-%02d %02d:%02d:%02d",StartDateTime[0],StartDateTime[1], StartDateTime[2], StartDateTime[3], StartDateTime[4], StartDateTime[5]);			
	XRD_Write_Addr(PAGE31_BASE_RECORD_STARTTIME+(PAGE31_STEP_LENGTH * Line),(char *)tempbuf, PAGE31_BASE_RECORD_STARTTIME_LEN);	
	sprintf((char *)tempbuf, "%02d:%02d:%02d",  SumTime / 3600, ( SumTime  % 3600)/60, (( SumTime  % 3600)%60) );						
	XRD_Write_Addr(PAGE31_BASE_RECORD_CH_TIME+(PAGE31_STEP_LENGTH * Line),(char *)tempbuf, PAGE31_BASE_RECORD_CH_TIME_LEN);		
	sprintf((char *)tempbuf, "%d.%02d",  SumEnergy / 100,  SumEnergy  % 100);
	XRD_Write_Addr(PAGE31_BASE_RECORD_CH_POWER+(PAGE31_STEP_LENGTH * Line),(char *)tempbuf, PAGE31_BASE_RECORD_CH_POWER_LEN);		
	sprintf((char *)tempbuf, "%d.%02d",  SumMoney / 100,  SumMoney  % 100);
	XRD_Write_Addr(PAGE31_BASE_RECORD_CH_MONEY+(PAGE31_STEP_LENGTH * Line),(char *)tempbuf, PAGE31_BASE_RECORD_CH_MONEY_LEN);			
	sprintf((char *)tempbuf, "%02X",  StopCause);
	XRD_Write_Addr(PAGE31_BASE_RECORD_ERR_CODE+(PAGE31_STEP_LENGTH * Line),(char *)tempbuf, PAGE31_BASE_RECORD_ERR_CODE_LEN);	
}

/************************************************************************************************************
** 函 数 名 : DispRecordNullInfo
** 功能描述 : 填充没有充电记录
** 输    入 : i   	第几行
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispRecordNullInfo(uint8_t i )
{
	XRD_Write_Addr(PAGE31_BASE_RECORD_ID+(PAGE31_STEP_LENGTH * i),(char *)"------------", PAGE31_BASE_RECORD_ID_LEN);	
	XRD_Write_Addr(PAGE31_BASE_RECORD_STARTTIME+(PAGE31_STEP_LENGTH * i),(char *)"--/--/-- --/--/--", PAGE31_BASE_RECORD_STARTTIME_LEN);
	XRD_Write_Addr(PAGE31_BASE_RECORD_CH_TIME+(PAGE31_STEP_LENGTH * i),(char *)"--:--:--", PAGE31_BASE_RECORD_CH_TIME_LEN);
	XRD_Write_Addr(PAGE31_BASE_RECORD_CH_POWER+(PAGE31_STEP_LENGTH * i),(char *)"-.--", PAGE31_BASE_RECORD_CH_POWER_LEN);
	XRD_Write_Addr(PAGE31_BASE_RECORD_CH_MONEY+(PAGE31_STEP_LENGTH * i),(char *)"-.--", PAGE31_BASE_RECORD_CH_MONEY_LEN);	
	XRD_Write_Addr(PAGE31_BASE_RECORD_ERR_CODE+(PAGE31_STEP_LENGTH * i),(char *)"--", PAGE31_BASE_RECORD_ERR_CODE_LEN);

}



/************************************************************************************************************
** 函 数 名 : DispRecordMoreInfo
** 功能描述 : 显示充电记录详情
** 输    入 :  ChgPort           哪个枪
**             StartType    		启动方式
**						 StartCardID   		卡号/ID
**             StartDateTime 	  开始时间
**             StopDateTime 	  停止时间
**             SumTime    			充电时长
**             SumEnergy        充电电量StartMoney
**             StartMoney       起始金额
**						 SumMoney         花费金额
**             StopCause        停止原因
**             StartSOC         开始SOC
**             StopSOC          停止SOC
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispRecordMoreInfo(uint8_t ChgPort,uint8_t StartType,uint32_t StartCardID,uint8_t* StartDateTime, uint8_t* StopDateTime,uint32_t SumTime, uint32_t SumEnergy,uint32_t StartMoney, uint32_t SumMoney,uint8_t StopCause,uint8_t StartSoc, uint8_t StopSoc )
{
	uint8_t tempbuf[56]={0};
	if(StartType == 1){
		sprintf((char *)tempbuf, "%05d", StartCardID);
	}else {
		u32tobcdnomstr1(StartCardID, tempbuf, 12);
		sprintf((char *)tempbuf, "%d%d%d%d%d%d%d%d%d%d", tempbuf[2], tempbuf[3], tempbuf[4], tempbuf[5], tempbuf[6], tempbuf[7], tempbuf[8], tempbuf[9], tempbuf[10], tempbuf[11]);
	}
	XRD_Write_Addr(PAGE32_CARDID_ADDR,(char *)tempbuf, PAGE32_CARDID_ADDR_LEN);
	if(StartType == STARTCHARGETYPE_OFFLINECARD){
		sprintf((char *)tempbuf, "离线卡");
	}else if(StartType == STARTCHARGETYPE_ONLINECARD){
		sprintf((char *)tempbuf, "在线卡");
	}else if(StartType == STARTCHARGETYPE_PLATFORM){
		sprintf((char *)tempbuf, "平台");
	}
	XRD_Write_Addr(PAGE32_STARTTYPE_ADDR,(char *)tempbuf, PAGE32_STARTTYPE_ADDR_LEN);
	sprintf((char *)tempbuf, "%d.%02d", SumMoney / 100, SumMoney  % 100);
	XRD_Write_Addr(PAGE32_SUMMONEY_ADDR,(char *)tempbuf, PAGE32_SUMMONEY_ADDR_LEN);	
	sprintf((char *)tempbuf, "%02d%%",StartSoc );
	XRD_Write_Addr(PAGE32_STARTSOC_ADDR,(char *)tempbuf, PAGE32_STARTSOC_ADDR_LEN);	//	STARTSOC
	sprintf((char *)tempbuf, "%02d%%",StopSoc );
	XRD_Write_Addr(PAGE32_STOPSOC_ADDR,(char *)tempbuf, PAGE32_STOPSOC_ADDR_LEN);	//	STOPSOC	
	sprintf((char *)tempbuf, "%02d/%02d/%02d %02d:%02d:%02d", StartDateTime[0],StartDateTime[1],StartDateTime[2],StartDateTime[3],StartDateTime[4],StartDateTime[5]);			
	XRD_Write_Addr(PAGE32_STARTTIME_ADDR,(char *)tempbuf, PAGE32_STARTTIME_ADDR_LEN);
	sprintf((char *)tempbuf, "%02d:%02d:%02d", SumTime / 3600, ( SumTime  % 3600)/60, (( SumTime  % 3600)%60) );
	XRD_Write_Addr(PAGE32_SUMTIME_ADDR,(char *)tempbuf, PAGE32_SUMTIME_ADDR_LEN);
	if(ChgPort == 0){
		sprintf((char *)tempbuf, "A 枪");
	}else{
		sprintf((char *)tempbuf, "B 枪");
	}	
	XRD_Write_Addr(PAGE32_CHGPORT_ADDR,(char *)tempbuf, PAGE32_CHGPORT_ADDR_LEN);		
	sprintf((char *)tempbuf, "%d.%02d", SumEnergy / 100, SumEnergy  % 100);
	XRD_Write_Addr(PAGE32_SUMENERGY_ADDR,(char *)tempbuf, PAGE32_SUMENERGY_ADDR_LEN);	
	sprintf((char *)tempbuf, "%02X", StopCause);
	XRD_Write_Addr(PAGE32_STOPCAUSE_ADDR,(char *)tempbuf, PAGE32_STOPCAUSE_ADDR_LEN);	
												
	sprintf((char *)tempbuf, "%02d/%02d/%02d %02d:%02d:%02d", StopDateTime[0],StopDateTime[1],StopDateTime[2],StopDateTime[3],StopDateTime[4],StopDateTime[5]);			
	XRD_Write_Addr(PAGE32_STOPTIME_ADDR,(char *)tempbuf, PAGE32_STOPTIME_ADDR_LEN);	
	sprintf((char *)tempbuf, "%d.%02d", StartMoney / 100, StartMoney  % 100);
	XRD_Write_Addr(PAGE32_STARTMONEY_ADDR,(char *)tempbuf, PAGE32_STARTMONEY_ADDR_LEN);	
	XRD_Load_Page(PAGE32);							
	
	
}

/************************************************************************************************************
** 函 数 名 : DispUpdataInfo
** 功能描述 : 显示充电升级进度
** 输    入 :  Present    		升级百分比
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispUpdataInfo( uint8_t Present,uint8_t* Tip)
{
	uint8_t tempbuf[56]={0};
	sprintf((char *)tempbuf, "%02d%%",Present );
	XRD_Write_Addr(PAGE33_UPDATA_PERSENT_ADDR,(char *)tempbuf, PAGE33_UPDATA_PERSENT_ADDR_LEN);	
	XRD_Write_Addr(PAGE33_UPDATA_TIP_ADDR,(char *)Tip, PAGE33_UPDATA_TIP_ADDR_LEN);
	XRD_Load_Page(PAGE33);					
}
































