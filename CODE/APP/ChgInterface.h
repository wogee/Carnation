#ifndef __CHGINTERFACE
#define __CHGINTERFACE
#pragma pack(1)
#define CAUSE_USER_NORMAL_STOP 										0x00//�û�����ֹͣ
#define CAUSE_WAIT_INSERTGUN_TIMEOUT							0x01//�ȴ���ǹ��ʱ
#define CAUSE_LOCK_GUN_FAILED											0x02//��ǹʧ��
#define CAUSE_K1K2_ADHESION_FAULT									0x03//�̵���ճ������
#define CAUSE_K1K2_CLOSE_FAULT										0x04//�̵����ܶ�����
#define CAUSE_K1K2_OUTSIDE_VOL_GREATER_THAN_10V		0x05//��Ե���ǰK1K2����ѹ����10V
#define CAUSE_CHARGEMODULE_OUTPUT_FAULT						0x06//���ģ���������
#define CAUSE_INSULATION_DETECT_FAULT							0x07//��Ե����쳣
#define CAUSE_BLEED_UNIT_FAULT										0x08//й�ŵ�·�쳣
#define CAUSE_BMS_PARA_CANNOT_FIT									0x09//�����������ʺ�
#define CAUSE_START_VOLTAGE_FAULT									0x0A//����ʱ��ѹ�쳣
#define CAUSE_BATTERY_READY_TO_NOREADY						0x0B//���׼��������Ϊδ����
#define CAUSE_SUSPEND_TIMEOUT											0x0C//���׮�������ͣ��ʱ
#define CAUSE_CC1_FAULT														0x0D//CC1�쳣
#define CAUSE_BMS_COMMUNICATE_TIMEOUT							0x0E//BMSͨ�ų�ʱ
#define CAUSE_SCRAMSTOP_FAULT											0x0F//��ͣ��������
#define CAUSE_DOOR_OPEN_FAULT											0x10//�Ž�����
#define CAUSE_GUNPORT_OVERTEMP										0x11//���ӿڹ���
#define CAUSE_OUPUTVOLTAGE_OVER_FAULT							0x12//ֱ�������ѹ��ѹ����
#define CAUSE_DEMANDVOL_FAULT											0x13//�����ѹ�쳣
#define CAUSE_BSM_SINGLEBATTERY_OVERVOL_FAULT			0x14//BSM�����е��嶯�����ص�ѹ����
#define CAUSE_BSM_SINGLEBATTERY_LESSVOL_FAULT			0x15//BSM�����е��嶯�����ص�ѹ����
#define CAUSE_BSM_SOC_OVER_FAULT									0x16//BSM������SOC����
#define CAUSE_BSM_SOC_LESS_FAULT									0x17//BSM������SOC����
#define CAUSE_BSM_SINGLEBATTERY_OVERCUR_FAULT			0x18//BSM�����е��嶯�����ص�������
#define CAUSE_BSM_TEMP_OVER_FAULT									0x19//BSM�����ж��������¶ȹ���
#define CAUSE_BSM_INSULATION_FAULT								0x1A//BSM�����ж������ؾ�Ե״̬�쳣
#define CAUSE_BSM_OUTPUTCONNECTER_FAULT						0x1B//BSM�����ж����������������������״̬�쳣
#define CAUSE_BST_NORMAL_REACHSOC									0x1C//BST�����дﵽ�������SOCĿ��ֵ
#define CAUSE_BST_NORMAL_REACHTOTALVOL						0x1D//BST�����дﵽ�ܵ�ѹ���趨ֵ
#define CAUSE_BST_NORMAL_REACHSINGLEVOL						0x1E//BST�����дﵽ�����ѹ���趨ֵ
#define CAUSE_BST_FAULT_INSULATION								0x1F//BST�����о�Ե����
#define CAUSE_BST_FAULT_OUTPUTCONNECTER_OVERTEMP	0x20//BST������������������¹���
#define CAUSE_BST_FAULT_ELEMENT_OVERTEMP					0x21//BST������BMSԪ�����������������
#define CAUSE_BST_FAULT_OUTPUTCONNECTER						0x22//BST�����г������������
#define CAUSE_BST_FAULT_BATTERYOVERTEMP						0x23//BST�����е�����¶ȹ��߹���
#define CAUSE_BST_FAULT_RELAY											0x24//BST�����и�ѹ�̵�������
#define CAUSE_BST_FAULT_CHECKPOINT2								0x25//BST�����м���2��ѹ������
#define CAUSE_BST_FAULT_OTHER											0x26//BST��������������
#define CAUSE_BST_ERROR_CUR												0x27//BST�����е�������
#define CAUSE_BST_ERROR_VOL												0x28//BST�����е�ѹ����
#define CAUSE_CP_FAULT														0x29//CP�쳣
#define CAUSE_CC_FAULT														0x2A//CC�쳣
#define CAUSE_GET_SOURCE_FAILED                   0x2B//�����Դ����ʧ��
#define CAUSE_DC_OVERCUR_FAULT                    0x2C//ֱ���������
#define CAUSE_AC_OVERVOL_FAULT                    0x2D//���������ѹ
#define CAUSE_AC_LESSVOL_FAULT                    0x2E//�������Ƿѹ
#define CAUSE_AC_OVERCUR_FAULT                    0x2F//�����������
#define CAUSE_AC_LESSCUR_FAULT                    0x30//�������Ƿ��
#define CAUSE_MONEY_FAULT                         0x31//�˻�����
#define CAUSE_MODULE_OVERVOL_FAULT                0x32//���ģ�������ѹ
#define CAUSE_MODULE_LESSVOL_FAULT                0x33//���ģ������Ƿѹ
#define CAUSE_ELM_FAULT                           0x34//������
#define CAUSE_PAUSE_TIMES_FAULT                   0x35//����г����ͣ��������
#define CAUSE_DC_LESSCUR_FAULT                    0x36//ֱ�����Ƿ��
#define CAUSE_CHG_COMPLETED                       0x37//��������


typedef struct{
	//ʵʱ����
	uint8_t ScramState; //��ͣ״̬ 0���� 1����
	uint8_t DoorState; //��״̬ 0���� 1����
	uint8_t GunSeatState;//���׮���ǹ��״̬ 0δ���� 1������
	uint8_t CCBAddrConflictState;//�����ư��ַ��ͻ״̬ 0δ��ͻ 1�г�ͻ
	
	//ʵʱ����
	uint8_t ChargeState; //���״̬ 0���� 1���
	uint16_t OutputVol;//��������ѹ ���ݷֱ���0.1V/λ 0Vƫ���� ���ݷ�Χ0.0V-1000V
	uint16_t OutputCur;//���������� ���ݷֱ���0.1A/λ -400Aƫ���� ���ݷ�Χ-400A-0.0A
	uint8_t  SOC;//SOC ���ݷֱ���1%/λ 0%ƫ���� ���ݷ�Χ0%-100%
	uint8_t  BetteryLowestTemperature;//���������¶� ���ݷֱ���1��/λ -50��ƫ���� ���ݷ�Χ-50��-+200��
	uint8_t  BetteryHighestTemperature;//���������¶� //���������¶� ���ݷֱ���1��/λ -50��ƫ���� ���ݷ�Χ-50��-+200��
	uint16_t BetteryHighestVol;//��������ߵ�ѹ ���ݷֱ���0.01V/λ 0Vƫ���� ���ݷ�Χ0.00V-24.00V
	uint16_t GuideVol;//��絼����ѹ ���ݷֱ���0.01V/λ 0Vƫ���� ���ݷ�Χ0.00V-30.00V
	uint16_t BmsDemandVol;//BMS�����ѹ ���ݷֱ���0.1V/λ 0Vƫ���� ���ݷ�Χ0.0V-1000V
	uint16_t BmsDemandCur;//BMS������� ���ݷֱ���0.1A/λ -400Aƫ���� ���ݷ�Χ-400A-0.0A
	uint8_t  Mode;//���ģʽ 1��ѹ��� 2�������
	uint16_t BmsMeasureVol;//BMS����ѹ����ֵ ���ݷֱ���0.01V/λ 0Vƫ���� ���ݷ�Χ0.00V-30.00V
	uint16_t BmsMeasureCur;//BMS����������ֵ ���ݷֱ���0.01V/λ 0Vƫ���� ���ݷ�Χ0.00V-30.00V
	uint16_t SurplusMinute;//����ʣ����ʱ�� ���ݷֱ���1min/λ 0minƫ���� ���ݷ�Χ0-600min
	
	//�������ʱ����
	uint32_t ChargerProtocolVersion;//���׮ͨ��Э��汾�� HEX�� 0x000101����V1.1 0x000100����V1.0
	uint32_t BmsProtocolVersion;//BMSͨ��Э��汾�� HEX�� 0x000101����V1.1 0x000100����V1.0
	uint8_t BatteryType;//������� 01HǦ���� 02H������ 03H������﮵�� 04H����﮵�� 05H����﮵�� 06H��Ԫ���ϵ�� 07H�ۺ�������ӵ�� 08H����﮵�� FFH�������
	uint8_t HighestTemperatureLimit;//��������¶� ���ݷֱ���1��/λ -50��ƫ���� ���ݷ�Χ-50��~+200��
	uint16_t HighestVoltageLimit;//����������ܵ�ѹ ���ݷֱ���0.1V/λ 0Vƫ����
	uint16_t SingleBatteryHighestVoltageLimit;//���嶯����������������ѹ ���ݷֱ���0.01V/λ 0Vƫ���� ���ݷ�Χ0~24V
	uint16_t HighestCurrentLimit;//������������ ���ݷֱ���0.1A/λ -400Aƫ���� ���ݷ�Χ-400A-0.0A
	uint16_t RatedVol;//������������ϵͳ��ܵ�ѹ ���ݷֱ���0.1/V��0.1V/λ��0Vƫ����
	uint16_t BatteryVol;//�����������ص�ǰ��ص�ѹ ���ݷֱ���0.1V/λ 0Vƫ����
	uint16_t RatedCapacity;//������������ϵͳ�����/��0.1Ah/λ��0Ahƫ����
	uint16_t RateDenergy;//�������ر�������� ���ݷֱ���0.1kWh/λ 0kWhƫ���� ���ݷ�Χ0~1000kWh
	uint16_t BatterySOC;//�����������غɵ�״̬ ���ݷֱ���0.1%/λ 0%ƫ���� ���ݷ�Χ0~100%
	uint8_t VIN[17];//����ʶ���루VIN��	
	uint8_t CommunicateState;//ͨ��״̬ 0���� 1ͨ���쳣
	
}CHARGEINFO;



uint8_t GetStopCause(uint8_t port);
uint8_t StartCharge(uint8_t port, uint8_t APVol);
uint8_t StopCharge(uint8_t port);
CHARGEINFO GetChgInfo(uint8_t port);
void ChargeTask(void * pdata);

#endif

