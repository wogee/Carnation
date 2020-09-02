#include "xrd.h"
#include "MyAlgLib.h"
#include "MainTask.h"
#include "Record.h"
#include "DisPlay.h"
#include "Para.h"
/************************************************************************************************************
** �� �� �� : DispMenuInfo
** �������� : ��ʾ�˵������Ϣ
** ��    �� : Time  ʱ��ָ��  Online  ���߱�ʶ
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispMenuInfo(uint8_t* Time, uint8_t* Online)
{

	XRD_Write_Addr(PAGE_All_TIME, (char *)Time, PAGE_All_TIME_LEN);//ʱ��
	XRD_Write_Addr(PAGE_ON_LINEFLAG, (char *)Online, PAGE_ON_LINEFLAG_LEN);//���߱�־
}


/************************************************************************************************************
** �� �� �� : DispStartDeviceInfo
** �������� : ��ʾ�豸�����н���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
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
			sprintf((char *)tempbuf, "�뼤��");
		}else{
			day = (ParaGetLimitHour() -ParaGetRunHour()) /24;
			sprintf((char *)tempbuf, "ʹ��ʣ��%d��", day);
		}
		XRD_Write_Addr(PAGE0_DAY_ADDR, (char *)tempbuf, PAGE0_DAY_ADDR_LEN );
	}
	XRD_Load_Page(PAGE0);
}



/************************************************************************************************************
** �� �� �� : DispErrIdGunInfo
** �������� : ��ʾǹ����ID
** ��    �� : port  AǹBǹ   IDBuf ����IDָ��
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispAccount
** �������� : ��ʾ�������
** ��    �� : SumEnergy    ������           0.01kwh
**						SumMoney     ���ѽ��           0.01Ԫ
**            RemainMoney   ���              0.01Ԫ  
**            SumTime      ���ʱ��           S  ������
**            Date          ����ָ��        
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAccount(uint32_t SumEnergy, uint32_t SumMoney, uint32_t RemainMoney, uint32_t SumTime, uint8_t* Date)
{
	uint8_t tempbuf[128]={0};
	
	sprintf((char *)tempbuf, "%d.%02d", SumEnergy / 100, SumEnergy % 100);
	XRD_Write_Addr(PAGE14_AB_POWER, (char *)tempbuf, PAGE14_AB_POWER_LEN);//AB��ǰ����	
	sprintf((char *)tempbuf, "%d.%02d", SumMoney / 100, SumMoney % 100);
	XRD_Write_Addr(PAGE14_AB_COST, (char *)tempbuf, PAGE14_AB_COST_LEN);//�������ѽ��
	sprintf((char *)tempbuf, "%d.%02d", RemainMoney / 100, RemainMoney % 100);
	XRD_Write_Addr(PAGE14_AB_REMAIN, (char *)tempbuf, PAGE14_AB_REMAIN_LEN);//���
	sprintf((char *)tempbuf, "%d Min", SumTime / 60);
	XRD_Write_Addr(PAGE14_AB_CH_TIME, (char *)tempbuf, PAGE14_AB_CH_TIME_LEN);//�ܺ�ʱ
	sprintf((char *)tempbuf, "20%02d-%02d-%02d %02d:%02d:%02d", Date[0], Date[1], Date[2], Date[3], Date[4],Date[5]);
	XRD_Write_Addr(PAGE14_AB_DATE, (char *)tempbuf, PAGE14_AB_DATE_LEN);//����
	XRD_Load_Page(PAGE14);
}



/************************************************************************************************************
** �� �� �� : DispInsertGunInfo
** �������� : ��ʾ���ǹ����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispInsertGunInfo(void)
{
		XRD_Load_Page(PAGE10);
}



/************************************************************************************************************
** �� �� �� : DispDeviceFault
** �������� : ��ʾ��׮���Ͻ���
** ��    �� : tempbuf    ����ID buf
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispDeviceFault(uint8_t* tempbuf)
{

	XRD_Write_Addr(PAGE19_AB_ERR,(char *)tempbuf,PAGE19_AB_ERR_LEN);
	XRD_Load_Page(PAGE19);     
}




/************************************************************************************************************
** �� �� �� : DispCardLockInfo
** �������� : ��ʾ��������
** ��    �� : Pile    ׮����ַ���
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispCardLockInfo(uint8_t* Pile )
{
	XRD_Write_Addr(PAGE15_AB_LOCK, (char *)Pile, PAGE15_AB_LOCK_LEN);	
	XRD_Load_Page(PAGE15); 			
}



/************************************************************************************************************
** �� �� �� : DispUnLockFailureInfo
** �������� : ��ʾ����ʧ�ܽ���
** ��    �� : FailureId    ʧ��ID
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispNoPileUsed
** �������� : ��ʾ�޿��г��λ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispNoPileUsed(void)
{
	XRD_Load_Page(PAGE17); 				
}



/************************************************************************************************************
** �� �� �� : DispRemainMoneyInfo
** �������� : ��ʾ������
** ��    �� : Money     0.01Ԫ
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispInVerify
** �������� : ��ʾ��֤�н���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispInVerify(void)
{
	XRD_Load_Page(PAGE23);
}



/************************************************************************************************************
** �� �� �� : DispVerifySuccesInfo
** �������� : ��ʾ��֤�ɹ�����
** ��    �� : Money   ��ǰ���   0.01Ԫ
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispVerifyFailure
** �������� : ��ʾ��֤ʧ�ܽ���
** ��    �� : tempbuf   ʧ��ԭ��ָ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispVerifyFailure(uint8_t* tempbuf)
{
	
	XRD_Write_Addr(PAGE25_FAILURE, (char *)tempbuf, PAGE25_FAILURE_LEN);
	XRD_Load_Page(PAGE25);
}


/************************************************************************************************************
** �� �� �� : DispStartChgInfo
** �������� : ��ʾ�����н���
** ��    �� : Time   ����ʱ��    ��λS ��
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispOperationInfo
** �������� : ��ʾ�����н���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispOperationInfo(void)
{
	XRD_Load_Page(PAGE27); 	
}

/************************************************************************************************************
** �� �� �� : DispStartSucessInfo
** �������� : ��ʾ�����ɹ�����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispStartSucessInfo(void)
{
	XRD_Load_Page(PAGE28); 
}


/************************************************************************************************************
** �� �� �� : DispStartFailureInfo
** �������� : ��ʾ����ʧ�ܽ���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispStartFailureInfo(void)
{
	XRD_Load_Page(PAGE29); 	
}






/************************************************************************************************************
******************************                ��ǹ������             **************************************
*************************************************************************************************************
*/

/************************************************************************************************************
** �� �� �� : DispAIdleInfo
** �������� : ��ʾA��ǹ��������
** ��    �� : ACode   Aǹ��ά��ָ��    APileNum  Aǹ����ַ�ָ��  GunStatus  A��ǹ״̬
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispAChargeInfo
** �������� : ��ʾA��ǹ������
** ��    �� : SumEnergy   A��ǰ����  0.01kwh
**						SumMoney    A��ǰ����  0.01Ԫ
**            CurrentA    A����      0.001 ��3λС��
**            VoltageA    A��ѹ      0.1  V
**            SumTime     A���ʱ��  S  ������
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAChargeInfo(uint32_t SumEnergy, uint32_t SumMoney, uint32_t CurrentA, uint32_t VoltageA, uint32_t SumTime,uint8_t SoC)
{
	char tempbuf[56]={0};
	sprintf((char *)tempbuf, "%02d%%", SoC);
	XRD_Write_Addr(PAGE21_A_PERCENTAGE, (char *)tempbuf, PAGE21_A_PERCENTAGE_LEN);//A��ǰSOC
	
	sprintf((char *)tempbuf, "%d.%02d", SumEnergy / 100, SumEnergy % 100);
	XRD_Write_Addr(PAGE21_A_POWER, (char *)tempbuf, PAGE21_A_POWER_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", SumMoney / 100, SumMoney % 100);
	XRD_Write_Addr(PAGE21_A_COST, (char *)tempbuf, PAGE21_A_COST_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", CurrentA / 1000, (CurrentA % 1000)/10);
	XRD_Write_Addr(PAGE21_A_I, (char *)tempbuf, PAGE21_A_I_LEN);//A����
	sprintf((char *)tempbuf, "%d.%d", VoltageA / 10, VoltageA % 10);
	XRD_Write_Addr(PAGE21_A_VOLTAGE, (char *)tempbuf, PAGE21_A_VOLTAGE_LEN);//A��ѹ
	sprintf((char *)tempbuf, "%02d:%02d:%02d", SumTime / 3600, (SumTime % 3600)/60, ((SumTime % 3600)%60) );
	XRD_Write_Addr(PAGE21_A_CH_TIME, (char *)tempbuf, PAGE21_A_CH_TIME_LEN);//A���ʱ��
	XRD_Load_Page(PAGE21);
}




/************************************************************************************************************
** �� �� �� : DispAStopInfo
** �������� : ��ʾA��ǹֹͣ����
** ��    �� : Prompt     	A��ʾ�ַ���ָ��
**						StpReason   Aֹͣԭ���ַ�ָ��
**            SumEnergy   A������          0.01kwh
**            SumMoney    A�����          0.01Ԫ
** ��    �� : ��
** ��    �� :	��
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
******************************                ˫ǹ������             **************************************
*************************************************************************************************************
*/

/************************************************************************************************************
** �� �� �� : DispAIdleBIdleInfo
** �������� : ��ʾA����B��������
** ��    �� :  ACode   Aǹ��ά��ָ��    APileNum  Aǹ����ַ�ָ��  AGunStatus  A��ǹ״̬  
**          :  BCode   Bǹ��ά��ָ��    BPileNum  Bǹ����ַ�ָ��  BGunStatus  B��ǹ״̬   
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispAChgBIdleInfo
** �������� : ��ʾA���B��������
** ��    �� : ASumEnergy   A��ǰ����   0.01kwh
**						ASumMoney    A��ǰ����   0.01Ԫ
**            ACurrent     A����       0.001 ��3λС��
**            AVoltage     A��ѹ       0.1  V
**            ASumTime     A���ʱ��    S  ������
**            ASoc         Aǹ SoC
**            BCode   Bǹ��ά��ָ��    BPileNum  Bǹ����ַ�ָ��  BGunStatus  B��ǹ״̬   
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAChgBIdleInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrent, uint32_t AVoltage,  uint32_t ASumTime,uint8_t ASoc, \
											  uint8_t* BCode, uint8_t* BPileNum, uint8_t BGunStatus)
{
	char tempbuf[56]={0};
	
	sprintf((char *)tempbuf, "%02d%%", ASoc);
	XRD_Write_Addr(PAGE8_A_PERCENTAGE, (char *)tempbuf, PAGE8_A_PERCENTAGE_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE8_A_POWER, (char *)tempbuf, PAGE8_A_POWER_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);
	XRD_Write_Addr(PAGE8_A_COST, (char *)tempbuf, PAGE8_A_COST_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ACurrent / 1000, (ACurrent % 1000)/10);
	XRD_Write_Addr(PAGE8_A_I, (char *)tempbuf, PAGE8_A_I_LEN);//A����
	sprintf((char *)tempbuf, "%d.%d", AVoltage / 10, AVoltage % 10);
	XRD_Write_Addr(PAGE8_A_VOLTAGE, (char *)tempbuf, PAGE8_A_VOLTAGE_LEN);//A��ѹ
	sprintf((char *)tempbuf, "%02d:%02d:%02d", ASumTime / 3600, (ASumTime % 3600)/60, ((ASumTime % 3600)%60) );
	XRD_Write_Addr(PAGE8_A_CH_TIME, (char *)tempbuf, PAGE8_A_CH_TIME_LEN);//A���ʱ��
		
	XRD_Write_Addr(PAGE8_B_DEVNUM, (char *)BPileNum, PAGE8_B_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE8_B_QR, (char *)BCode, PAGE8_B_QR_LEN);	
	XRD_Draw_Ioc(PAGE8_B_INSERTFLAG, BGunStatus);
	XRD_Load_Page(PAGE8);	
}

/************************************************************************************************************
** �� �� �� : DispAStopBIdle
** �������� : ��ʾAǹֹͣB��������
** ��    �� : APrompt      A��ʾ�ַ���ָ��
**						AStpReason   Aֹͣԭ���ַ�ָ��
**            ASumEnergy   A������          0.01kwh
**            ASumMoney    A�����          0.01Ԫ
**            BCode   Bǹ��ά��ָ��    BPileNum  Bǹ����ַ�ָ��  BGunStatus  B��ǹ״̬  
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispAIdleBChgInfo
** �������� : ��ʾA����B����н���
** ��    �� :  ACode   Aǹ��ά��ָ��    APileNum  Aǹ����ַ�ָ��  AGunStatus  A��ǹ״̬  
**             BSumEnergy   B��ǰ����   0.01kwh
**						 BSumMoney    B��ǰ����   0.01Ԫ
**             BCurrent     B����       0.001 ��3λС��
**             BVoltage     B��ѹ       0.1  V
**             BSumTime     B���ʱ��    S  ������  BSoc
**             BSoc         BSoc
** ��    �� : ��
** ��    �� :	��
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
	XRD_Write_Addr(PAGE3_B_PERCENTAGE, (char *)tempbuf, PAGE3_B_PERCENTAGE_LEN);//B��ǰ����	
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE3_B_POWER, (char *)tempbuf, PAGE3_B_POWER_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);
	XRD_Write_Addr(PAGE3_B_COST, (char *)tempbuf, PAGE3_B_COST_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BCurrent / 1000, (BCurrent % 1000)/10);
	XRD_Write_Addr(PAGE3_B_I, (char *)tempbuf, PAGE3_B_I_LEN);//B����
	sprintf((char *)tempbuf, "%d.%d", BVoltage / 10, BVoltage % 10);
	XRD_Write_Addr(PAGE3_B_VOLTAGE, (char *)tempbuf, PAGE3_B_VOLTAGE_LEN);//B��ѹ
	sprintf((char *)tempbuf, "%02d:%02d:%02d", BSumTime / 3600, (BSumTime % 3600)/60, ((BSumTime % 3600)%60) );
	XRD_Write_Addr(PAGE3_B_CH_TIME, (char *)tempbuf, PAGE3_B_CH_TIME_LEN);//B���ʱ��
	XRD_Load_Page(PAGE3);	
}


/************************************************************************************************************
** �� �� �� : DispAIdleBStop
** �������� : ��ʾA����B���ֹͣ����
** ��    �� :  ACode   Aǹ��ά��ָ��    APileNum  Aǹ����ַ�ָ��  AGunStatus  A��ǹ״̬  
**             BPrompt      B��ʾ�ַ���ָ��
**						 BStpReason   Bֹͣԭ���ַ�ָ��
**             BSumEnergy   B������          0.01kwh
**             BSumMoney    B�����          0.01Ԫ
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispAStopBChgInfo
** �������� : ��ʾA���ֹͣB����н���
** ��    �� :  APrompt      A��ʾ�ַ���ָ��
**						 AStpReason   Aֹͣԭ���ַ�ָ��
**             ASumEnergy   A������          0.01kwh
**             ASumMoney    A�����          0.01Ԫ
**             BSumEnergy   B��ǰ����          0.01kwh
**						 BSumMoney    B��ǰ����          0.01Ԫ
**             BCurrent     B����              0.001 ��3λС��
**             BVoltage     B��ѹ              0.1  V
**             BSumTime     B���ʱ��          S  ������  
** ��    �� : ��
** ��    �� :	��
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
	XRD_Write_Addr(PAGE5_B_POWER, (char *)tempbuf, PAGE5_B_POWER_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);
	XRD_Write_Addr(PAGE5_B_COST, (char *)tempbuf, PAGE5_B_COST_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BCurrent / 1000, (BCurrent % 1000)/10);
	XRD_Write_Addr(PAGE5_B_I, (char *)tempbuf, PAGE5_B_I_LEN);//B����
	sprintf((char *)tempbuf, "%d.%d", BVoltage / 10, BVoltage % 10);
	XRD_Write_Addr(PAGE5_B_VOLTAGE, (char *)tempbuf, PAGE5_B_VOLTAGE_LEN);//B��ѹ
	sprintf((char *)tempbuf, "%02d:%02d:%02d", BSumTime / 3600, (BSumTime % 3600)/60, ((BSumTime % 3600)%60) );
	XRD_Write_Addr(PAGE5_B_CH_TIME, (char *)tempbuf, PAGE5_B_CH_TIME_LEN);//B���ʱ��
	XRD_Load_Page(PAGE5);	
}

/************************************************************************************************************
** �� �� �� : DispAChgBStopInfo
** �������� : ��ʾA�����B���ֹͣ����
** ��    �� :  ASumEnergy   A��ǰ����           0.01kwh
**						 ASumMoney    A��ǰ����           0.01Ԫ
**             ACurrent     A����               0.001 ��3λС��
**             AVoltage     A��ѹ               0.1  V
**             ASumTime     A���ʱ��           S  ������
**             BPrompt      B��ʾ�ַ���ָ��
**						 BStpReason   Bֹͣԭ���ַ�ָ��
**             BSumEnergy   B������           0.01kwh
**             BSumMoney    B�����           0.01Ԫ
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAChgBStopInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrent, uint32_t AVoltage,  uint32_t ASumTime,uint8_t ASoc, \
												uint8_t* BPrompt, uint8_t* BStpReason, uint32_t BSumEnergy, uint32_t BSumMoney )
{
	char tempbuf[56]={0};
	
	sprintf((char *)tempbuf, "%02d%%", ASoc);
	XRD_Write_Addr(PAGE7_A_PERCENTAGE, (char *)tempbuf, PAGE7_A_PERCENTAGE_LEN);
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE7_A_POWER, (char *)tempbuf, PAGE7_A_POWER_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);
	XRD_Write_Addr(PAGE7_A_COST, (char *)tempbuf, PAGE7_A_COST_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ACurrent / 1000, (ACurrent % 1000)/10);
	XRD_Write_Addr(PAGE7_A_I, (char *)tempbuf, PAGE7_A_I_LEN);//A����
	sprintf((char *)tempbuf, "%d.%d", AVoltage / 10, AVoltage % 10);
	XRD_Write_Addr(PAGE7_A_VOLTAGE, (char *)tempbuf, PAGE7_A_VOLTAGE_LEN);//A��ѹ
	sprintf((char *)tempbuf, "%02d:%02d:%02d", ASumTime / 3600, (ASumTime % 3600)/60, ((ASumTime % 3600)%60) );
	XRD_Write_Addr(PAGE7_A_CH_TIME, (char *)tempbuf, PAGE7_A_CH_TIME_LEN);//A���ʱ��
	
	XRD_Write_Addr(PAGE7_B_STOP_REASON, (char *)BStpReason, PAGE7_B_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE7_B_CHARG_PROMPT, (char *)BPrompt, PAGE7_B_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE7_B_CHARG_POWER,(char *)tempbuf, PAGE7_B_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);;											
	XRD_Write_Addr(PAGE7_B_CHARG_MONEY,(char *)tempbuf, PAGE7_B_CHARG_MONEY_LEN);	
	XRD_Load_Page(PAGE7);	
	
}


/************************************************************************************************************
** �� �� �� : DispAChgBChgInfo
** �������� : ��ʾA�����B����н���
** ��    �� :  ASumEnergy   A��ǰ����           0.01kwh
**						 ASumMoney    A��ǰ����           0.01Ԫ
**             ACurrent     A����               0.001 ��3λС��
**             AVoltage     A��ѹ               0.1  V
**             ASumTime     A���ʱ��           S  ������
**             BSumEnergy   B��ǰ����           0.01kwh
**						 BSumMoney    B��ǰ����           0.01Ԫ
**             BCurrent     B����               0.001 ��3λС��
**             BVoltage     B��ѹ               0.1  V
**             BSumTime     B���ʱ��           S  ������  
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAChgBChgInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrent, uint32_t AVoltage,  uint32_t ASumTime, uint8_t ASoc,\
											 uint32_t BSumEnergy, uint32_t BSumMoney, uint32_t BCurrent, uint32_t BVoltage,  uint32_t BSumTime, uint8_t BSoc)
{
	char tempbuf[56]={0};
	
	sprintf((char *)tempbuf, "%02d%%", ASoc);
	XRD_Write_Addr(PAGE9_A_PERCENTAGE, (char *)tempbuf, PAGE9_A_PERCENTAGE_LEN);
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE9_A_POWER, (char *)tempbuf, PAGE9_A_POWER_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);
	XRD_Write_Addr(PAGE9_A_COST, (char *)tempbuf, PAGE9_A_COST_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ACurrent / 1000, (ACurrent % 1000)/10);
	XRD_Write_Addr(PAGE9_A_I, (char *)tempbuf, PAGE9_A_I_LEN);//A����
	sprintf((char *)tempbuf, "%d.%d", AVoltage / 10, AVoltage % 10);
	XRD_Write_Addr(PAGE9_A_VOLTAGE, (char *)tempbuf, PAGE9_A_VOLTAGE_LEN);//A��ѹ
	sprintf((char *)tempbuf, "%02d:%02d:%02d", ASumTime / 3600, (ASumTime % 3600)/60, ((ASumTime % 3600)%60) );
	XRD_Write_Addr(PAGE9_A_CH_TIME, (char *)tempbuf, PAGE9_A_CH_TIME_LEN);//A���ʱ��
	
	sprintf((char *)tempbuf, "%02d%%", BSoc);
	XRD_Write_Addr(PAGE9_B_PERCENTAGE, (char *)tempbuf, PAGE9_B_PERCENTAGE_LEN);
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE9_B_POWER, (char *)tempbuf, PAGE9_B_POWER_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);
	XRD_Write_Addr(PAGE9_B_COST, (char *)tempbuf, PAGE9_B_COST_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BCurrent / 1000, (BCurrent % 1000)/10);
	XRD_Write_Addr(PAGE9_B_I, (char *)tempbuf, PAGE9_B_I_LEN);//B����
	sprintf((char *)tempbuf, "%d.%d", BVoltage / 10, BVoltage % 10);
	XRD_Write_Addr(PAGE9_B_VOLTAGE, (char *)tempbuf, PAGE9_B_VOLTAGE_LEN);//B��ѹ
	sprintf((char *)tempbuf, "%02d:%02d:%02d", BSumTime / 3600, (BSumTime % 3600)/60, ((BSumTime % 3600)%60) );
	XRD_Write_Addr(PAGE9_B_CH_TIME, (char *)tempbuf, PAGE9_B_CH_TIME_LEN);//B���ʱ��
	
	XRD_Load_Page(PAGE9);
}




/************************************************************************************************************
** �� �� �� : DispAStopBStopInfo
** �������� : ��ʾAֹͣBֹͣ����
** ��    �� :  APrompt      A��ʾ�ַ���ָ��
**						 AStpReason   Aֹͣԭ���ַ�ָ��
**             ASumEnergy   A������            0.01kwh
**             ASumMoney    A�����            0.01Ԫ
**             BPrompt      B��ʾ�ַ���ָ��
**						 BStpReason   Bֹͣԭ���ַ�ָ��
**             BSumEnergy   B������            0.01kwh
**             BSumMoney    B�����            0.01Ԫ
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispRecordInfo
** �������� : ��ʾ����¼�ĵڼ���
** ��    �� :  StartType    		������ʽ
**						 StartCardID   		����/ID
**             StartDateTime 	  ��ʼʱ��
**             SumTime    			���ʱ��
**             SumEnergy        ������
**						 SumMoney         ���ѽ��
**             StopCause        ֹͣԭ��
**             Line             �ڼ���
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispRecordNullInfo
** �������� : ���û�г���¼
** ��    �� : i   	�ڼ���
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : DispRecordMoreInfo
** �������� : ��ʾ����¼����
** ��    �� :  ChgPort           �ĸ�ǹ
**             StartType    		������ʽ
**						 StartCardID   		����/ID
**             StartDateTime 	  ��ʼʱ��
**             StopDateTime 	  ֹͣʱ��
**             SumTime    			���ʱ��
**             SumEnergy        ������StartMoney
**             StartMoney       ��ʼ���
**						 SumMoney         ���ѽ��
**             StopCause        ֹͣԭ��
**             StartSOC         ��ʼSOC
**             StopSOC          ֹͣSOC
** ��    �� : ��
** ��    �� :	��
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
		sprintf((char *)tempbuf, "���߿�");
	}else if(StartType == STARTCHARGETYPE_ONLINECARD){
		sprintf((char *)tempbuf, "���߿�");
	}else if(StartType == STARTCHARGETYPE_PLATFORM){
		sprintf((char *)tempbuf, "ƽ̨");
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
		sprintf((char *)tempbuf, "A ǹ");
	}else{
		sprintf((char *)tempbuf, "B ǹ");
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
** �� �� �� : DispUpdataInfo
** �������� : ��ʾ�����������
** ��    �� :  Present    		�����ٷֱ�
** ��    �� : ��
** ��    �� :	��
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
































