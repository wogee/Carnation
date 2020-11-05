#ifndef __CHGINTERFACE
#define __CHGINTERFACE
#pragma pack(1)
#define CAUSE_USER_NORMAL_STOP 										0x00//用户正常停止
#define CAUSE_WAIT_INSERTGUN_TIMEOUT							0x01//等待插枪超时
#define CAUSE_LOCK_GUN_FAILED											0x02//锁枪失败
#define CAUSE_K1K2_ADHESION_FAULT									0x03//继电器粘连故障
#define CAUSE_K1K2_CLOSE_FAULT										0x04//继电器拒动故障
#define CAUSE_K1K2_OUTSIDE_VOL_GREATER_THAN_10V		0x05//绝缘检测前K1K2外侧电压大于10V
#define CAUSE_CHARGEMODULE_OUTPUT_FAULT						0x06//充电模块输出故障
#define CAUSE_INSULATION_DETECT_FAULT							0x07//绝缘检测异常
#define CAUSE_BLEED_UNIT_FAULT										0x08//泄放电路异常
#define CAUSE_BMS_PARA_CANNOT_FIT									0x09//车辆参数不适合
#define CAUSE_START_VOLTAGE_FAULT									0x0A//启动时电压异常
#define CAUSE_BATTERY_READY_TO_NOREADY						0x0B//电池准备就绪变为未就绪
#define CAUSE_SUSPEND_TIMEOUT											0x0C//充电桩充电中暂停超时
#define CAUSE_CC1_FAULT														0x0D//CC1异常
#define CAUSE_BMS_COMMUNICATE_TIMEOUT							0x0E//BMS通信超时
#define CAUSE_SCRAMSTOP_FAULT											0x0F//急停动作故障
#define CAUSE_DOOR_OPEN_FAULT											0x10//门禁故障
#define CAUSE_GUNPORT_OVERTEMP										0x11//充电接口过温
#define CAUSE_OUPUTVOLTAGE_OVER_FAULT							0x12//直流输出电压过压故障
#define CAUSE_DEMANDVOL_FAULT											0x13//需求电压异常
#define CAUSE_BSM_SINGLEBATTERY_OVERVOL_FAULT			0x14//BSM报文中单体动力蓄电池电压过高
#define CAUSE_BSM_SINGLEBATTERY_LESSVOL_FAULT			0x15//BSM报文中单体动力蓄电池电压过低
#define CAUSE_BSM_SOC_OVER_FAULT									0x16//BSM报文中SOC过高
#define CAUSE_BSM_SOC_LESS_FAULT									0x17//BSM报文中SOC过低
#define CAUSE_BSM_SINGLEBATTERY_OVERCUR_FAULT			0x18//BSM报文中单体动力蓄电池电流过流
#define CAUSE_BSM_TEMP_OVER_FAULT									0x19//BSM报文中动力蓄电池温度过高
#define CAUSE_BSM_INSULATION_FAULT								0x1A//BSM报文中动力蓄电池绝缘状态异常
#define CAUSE_BSM_OUTPUTCONNECTER_FAULT						0x1B//BSM报文中动力蓄电池组输出连接器连接状态异常
#define CAUSE_BST_NORMAL_REACHSOC									0x1C//BST报文中达到所需求的SOC目标值
#define CAUSE_BST_NORMAL_REACHTOTALVOL						0x1D//BST报文中达到总电压的设定值
#define CAUSE_BST_NORMAL_REACHSINGLEVOL						0x1E//BST报文中达到单体电压的设定值
#define CAUSE_BST_FAULT_INSULATION								0x1F//BST报文中绝缘故障
#define CAUSE_BST_FAULT_OUTPUTCONNECTER_OVERTEMP	0x20//BST报文中输出连接器过温故障
#define CAUSE_BST_FAULT_ELEMENT_OVERTEMP					0x21//BST报文中BMS元件、输出连接器过温
#define CAUSE_BST_FAULT_OUTPUTCONNECTER						0x22//BST报文中充电连接器故障
#define CAUSE_BST_FAULT_BATTERYOVERTEMP						0x23//BST报文中电池组温度过高故障
#define CAUSE_BST_FAULT_RELAY											0x24//BST报文中高压继电器故障
#define CAUSE_BST_FAULT_CHECKPOINT2								0x25//BST报文中检测点2电压检测故障
#define CAUSE_BST_FAULT_OTHER											0x26//BST报文中其他故障
#define CAUSE_BST_ERROR_CUR												0x27//BST报文中电流过大
#define CAUSE_BST_ERROR_VOL												0x28//BST报文中电压过大
#define CAUSE_CP_FAULT														0x29//CP异常
#define CAUSE_CC_FAULT														0x2A//CC异常
#define CAUSE_GET_SOURCE_FAILED                   0x2B//充电资源申请失败
#define CAUSE_DC_OVERCUR_FAULT                    0x2C//直流输出过流
#define CAUSE_AC_OVERVOL_FAULT                    0x2D//交流输出过压
#define CAUSE_AC_LESSVOL_FAULT                    0x2E//交流输出欠压
#define CAUSE_AC_OVERCUR_FAULT                    0x2F//交流输出过流
#define CAUSE_AC_LESSCUR_FAULT                    0x30//交流输出欠流
#define CAUSE_MONEY_FAULT                         0x31//账户余额不足
#define CAUSE_MODULE_OVERVOL_FAULT                0x32//充电模块输入过压
#define CAUSE_MODULE_LESSVOL_FAULT                0x33//充电模块输如欠压
#define CAUSE_ELM_FAULT                           0x34//电表故障
#define CAUSE_PAUSE_TIMES_FAULT                   0x35//充电中充电暂停次数过多
#define CAUSE_DC_LESSCUR_FAULT                    0x36//直流输出欠流
#define CAUSE_CHG_COMPLETED                       0x37//充电已完成


typedef struct{
	//实时数据
	uint8_t ScramState; //急停状态 0正常 1故障
	uint8_t DoorState; //门状态 0正常 1故障
	uint8_t GunSeatState;//充电桩充电枪座状态 0未连接 1已连接
	uint8_t CCBAddrConflictState;//充电控制板地址冲突状态 0未冲突 1有冲突
	
	//实时数据
	uint8_t ChargeState; //充电状态 0待机 1充电
	uint16_t OutputVol;//充电输出电压 数据分辨率0.1V/位 0V偏移量 数据范围0.0V-1000V
	uint16_t OutputCur;//充电输出电流 数据分辨率0.1A/位 -400A偏移量 数据范围-400A-0.0A
	uint8_t  SOC;//SOC 数据分辨率1%/位 0%偏移量 数据范围0%-100%
	uint8_t  BetteryLowestTemperature;//电池组最低温度 数据分辨率1度/位 -50度偏移量 数据范围-50度-+200度
	uint8_t  BetteryHighestTemperature;//电池组最高温度 //电池组最低温度 数据分辨率1度/位 -50度偏移量 数据范围-50度-+200度
	uint16_t BetteryHighestVol;//单体电池最高电压 数据分辨率0.01V/位 0V偏移量 数据范围0.00V-24.00V
	uint16_t GuideVol;//充电导引电压 数据分辨率0.01V/位 0V偏移量 数据范围0.00V-30.00V
	uint16_t BmsDemandVol;//BMS需求电压 数据分辨率0.1V/位 0V偏移量 数据范围0.0V-1000V
	uint16_t BmsDemandCur;//BMS需求电流 数据分辨率0.1A/位 -400A偏移量 数据范围-400A-0.0A
	uint8_t  Mode;//充电模式 1恒压充电 2恒流充电
	uint16_t BmsMeasureVol;//BMS充电电压测量值 数据分辨率0.01V/位 0V偏移量 数据范围0.00V-30.00V
	uint16_t BmsMeasureCur;//BMS充电电流测量值 数据分辨率0.01V/位 0V偏移量 数据范围0.00V-30.00V
	uint16_t SurplusMinute;//估算剩余充电时间 数据分辨率1min/位 0min偏移量 数据范围0-600min
	
	//启动完成时数据
	uint32_t ChargerProtocolVersion;//充电桩通信协议版本号 HEX码 0x000101代表V1.1 0x000100代表V1.0
	uint32_t BmsProtocolVersion;//BMS通信协议版本号 HEX码 0x000101代表V1.1 0x000100代表V1.0
	uint8_t BatteryType;//电池类型 01H铅酸电池 02H镍氢电池 03H磷酸铁锂电池 04H锰酸锂电池 05H钴酸锂电池 06H三元材料电池 07H聚合物锂离子电池 08H钛酸锂电池 FFH其他电池
	uint8_t HighestTemperatureLimit;//最高允许温度 数据分辨率1度/位 -50度偏移量 数据范围-50度~+200度
	uint16_t HighestVoltageLimit;//最高允许充电总电压 数据分辨率0.1V/位 0V偏移量
	uint16_t SingleBatteryHighestVoltageLimit;//单体动力蓄电池最高允许充电电压 数据分辨率0.01V/位 0V偏移量 数据范围0~24V
	uint16_t HighestCurrentLimit;//最高允许充电电流 数据分辨率0.1A/位 -400A偏移量 数据范围-400A-0.0A
	uint16_t RatedVol;//整车动力蓄电池系统额定总电压 数据分辨率0.1/V，0.1V/位，0V偏移量
	uint16_t BatteryVol;//整车动力蓄电池当前电池电压 数据分辨率0.1V/位 0V偏移量
	uint16_t RatedCapacity;//整车动力蓄电池系统额定容量/，0.1Ah/位，0Ah偏移量
	uint16_t RateDenergy;//动力蓄电池标称总能量 数据分辨率0.1kWh/位 0kWh偏移量 数据范围0~1000kWh
	uint16_t BatterySOC;//整车动力蓄电池荷电状态 数据分辨率0.1%/位 0%偏移量 数据范围0~100%
	uint8_t VIN[17];//车辆识别码（VIN）	
	uint8_t CommunicateState;//通信状态 0正常 1通信异常
	
}CHARGEINFO;



uint8_t GetStopCause(uint8_t port);
uint8_t StartCharge(uint8_t port, uint8_t APVol);
uint8_t StopCharge(uint8_t port);
CHARGEINFO GetChgInfo(uint8_t port);
void ChargeTask(void * pdata);

#endif

