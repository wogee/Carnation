#ifndef __XRD_H
#define __XRD_H
#include "stdio.h"
#include <stdint.h>

#define LCDDATAMAXLEN   512
typedef struct{
	uint8_t ReBuf[LCDDATAMAXLEN];
	uint16_t RecvWr;
	uint16_t RecvRd;
}LCDINFO;


#define PACKHEAD1 	0x5a
#define PACKHEAD2	  0xa5
#define WRITE_CMD       	0x80
#define READ_CMD          0x81
#define WRITE_VAR_CMD     0x82
#define READ_VAR_CMD      0x83 
#define PIC_ADDR          0x03

static uint8_t PacketHead[3]={PACKHEAD1,PACKHEAD2,'\0'};


#define KEY_RETURN   0x800             //按键返回值得地址
#define KEY_INPUT    0x790             //按键返回值得地址


#define PAGE0       0x00    //开机启动
#define PAGE1       0x01    //A待机，B待机
#define PAGE2       0x02    //A待机，B停止
#define PAGE3       0x03    //A待机，B充电
#define PAGE4       0x04    //A停止，B待机
#define PAGE5       0x05    //A停止，B充电
#define PAGE6       0x06    //A停止，B停止
#define PAGE7       0x07    //A充电，B停止
#define PAGE8       0x08    //A充电，B待机
#define PAGE9       0x09    //A充电，B充电
#define PAGE10      0x0A    //连接提示
#define PAGE11      0x0B    //输入密码
#define PAGE12      0x0C    //更多信息
#define PAGE13      0x0D    //充电方式选择
#define PAGE14      0x0E    //结算
#define PAGE15      0x0F    //锁卡
#define PAGE16      0x10    //解锁失败
#define PAGE17      0x11    //无充电位，充电枪可用
#define PAGE18      0x12    //余额
#define PAGE19      0x13    //充电故障
#define PAGE20      0x14    //单枪待机
#define PAGE21      0x15    //单枪充电中
#define PAGE22      0x16    //充电停止
#define PAGE23      0x17      //验证中
#define PAGE24      0x18  		//验证成功
#define PAGE25      0x19		  //验证失败
#define PAGE26      0x1A      //启动充电中，绝缘检测中
#define PAGE27      0x1B      //操作中
#define PAGE28      0x1C      //启动成功
#define PAGE29      0x1D      //启动充电失败    
#define PAGE30      0x1E      //输入密码
#define PAGE31      0x1F      //充电记录
#define PAGE32      0x20      //充电记录详情
#define PAGE33      0x21      //电桩升中

#define PAGE_A_ERRID         0x720
#define PAGE_B_ERRID         0x730

#define PAGE_A_ERRID_LEN        16
#define PAGE_B_ERRID_LEN        16

#define PAGE_All_TIME        0x690             //时间
#define PAGE_ON_LINEFLAG     0x6A8             //在线显示

#define PAGE0_VER_ADDR      0x700
#define PAGE0_DAY_ADDR      0x720							//激活码

#define PAGE1_A_QR          0x00              //A枪二维码
#define PAGE1_A_DEVNUM      0x4A0             //A枪设备编号
#define PAGE1_A_INSERTFLAG  0x468             //A枪插枪标志

#define PAGE1_B_QR          0x200             //B枪二维码       
#define PAGE1_B_DEVNUM      0x5A0             //B枪设备编号
#define PAGE1_B_INSERTFLAG  0x568             //B枪插枪标志

#define PAGE2_A_QR           0x00             //A枪二维码
#define PAGE2_A_DEVNUM       0x4A0            //A枪设备编号
#define PAGE2_A_INSERTFLAG  0x468             //A枪插枪标志
#define PAGE2_B_STOP_REASON  0x528            //B枪停止原因
#define PAGE2_B_CHARG_POWER  0x570            //B充电电量
#define PAGE2_B_CHARG_MONEY  0x578            //B花费金额
#define PAGE2_B_CHARG_PROMPT 0x580            //B停止提示

#define PAGE3_A_QR          0x00              //A枪二维码
#define PAGE3_A_DEVNUM      0x4A0             //A枪设备编号
#define PAGE3_A_INSERTFLAG  0x468             //A枪插枪标志

#define PAGE3_B_POWER         0x500             //B当前电量
#define PAGE3_B_PERCENTAGE    0x508             //B充电完成百分比
#define PAGE3_B_COST          0x510             //B当前花费
#define PAGE3_B_I             0x518             //B电流
#define PAGE3_B_VOLTAGE       0x520             //B电压
#define PAGE3_B_CH_TIME       0x548             //B充电时间 

#define PAGE4_A_STOP_REASON   0x428             //A停止原因
#define PAGE4_A_CHARG_POWER   0x470             //A充电电量
#define PAGE4_A_CHARG_MONEY   0x478             //A花费金额
#define PAGE4_A_CHARG_PROMPT  0x480             //A停止提示 
#define PAGE4_B_QR            0x200             //B枪二维码 
#define PAGE4_B_DEVNUM        0x5A0             //B枪设备编号
#define PAGE4_B_INSERTFLAG    0x568             //B枪插枪标志

#define PAGE5_A_STOP_REASON   0x428             //A停止原因
#define PAGE5_A_CHARG_POWER   0x470             //A充电电量
#define PAGE5_A_CHARG_MONEY   0x478             //A 花费金额
#define PAGE5_A_CHARG_PROMPT  0x480             //A停止提示 

#define PAGE5_B_POWER         0x500             //B当前电量
#define PAGE5_B_PERCENTAGE    0x508             //B充电完成百分比
#define PAGE5_B_COST          0x510             //B当前花费
#define PAGE5_B_I             0x518             //B电流
#define PAGE5_B_VOLTAGE       0x520             //B电压	
#define PAGE5_B_CH_TIME       0x548             //B充电时间

#define PAGE6_A_STOP_REASON   0x428             //A停止原因
#define PAGE6_A_CHARG_POWER   0x470             //A充电电量
#define PAGE6_A_CHARG_MONEY   0x478             //A 花费金额
#define PAGE6_A_CHARG_PROMPT  0x480             //A停止提示 
#define PAGE6_B_STOP_REASON   0x528             //B停止原因
#define PAGE6_B_CHARG_POWER   0x570             //B充电电量
#define PAGE6_B_CHARG_MONEY   0x578             //B 花费金额
#define PAGE6_B_CHARG_PROMPT  0x580             //B停止提示 

#define PAGE7_A_POWER         0x400             //A当前电量
#define PAGE7_A_PERCENTAGE    0x408             //A充电完成百分比
#define PAGE7_A_COST          0x410             //A当前花费
#define PAGE7_A_I             0x418             //A电流
#define PAGE7_A_VOLTAGE       0x420             //A电压	
#define PAGE7_A_CH_TIME       0x448             //A充电时间
#define PAGE7_B_STOP_REASON   0x528             //B停止原因
#define PAGE7_B_CHARG_POWER   0x570             //B充电电量
#define PAGE7_B_CHARG_MONEY   0x578             //B 花费金额
#define PAGE7_B_CHARG_PROMPT  0x580             //B停止提示 	

#define PAGE8_A_POWER         0x400             //A当前电量
#define PAGE8_A_PERCENTAGE    0x408             //A充电完成百分比
#define PAGE8_A_COST          0x410             //A当前花费
#define PAGE8_A_I             0x418             //A电流
#define PAGE8_A_VOLTAGE       0x420             //A电压	
#define PAGE8_A_CH_TIME       0x448             //A充电时间
#define PAGE8_B_QR            0x200
#define PAGE8_B_DEVNUM        0x5A0
#define PAGE8_B_INSERTFLAG    0x568

#define PAGE9_A_POWER         0x400             //A当前电量
#define PAGE9_A_PERCENTAGE    0x408             //A充电完成百分比
#define PAGE9_A_COST          0x410             //A当前花费
#define PAGE9_A_I             0x418             //A电流
#define PAGE9_A_VOLTAGE       0x420             //A电压	
#define PAGE9_A_CH_TIME       0x448             //A充电时间
#define PAGE9_B_POWER         0x500             //B当前电量
#define PAGE9_B_PERCENTAGE    0x508             //B充电完成百分比
#define PAGE9_B_COST          0x510             //B当前花费
#define PAGE9_B_I             0x518             //B电流
#define PAGE9_B_VOLTAGE       0x520             //B电压	
#define PAGE9_B_CH_TIME       0x548             //B充电时间

// PAGE 11 页
#define PAGE12_CH_MODE        				0x700          //充电模式
#define PAGE12_REQ_VOLTAGE 						0x710          //需求点压
#define PAGE12_RATED_VOLTAGE 	 	  		0x718          //额定电压
#define PAGE12_MAX_ALLOW_VOLTAGE  		0x720          //最高允许单体电压
#define PAGE12_COMMUNICATION_VERSION 	0x730          //通讯版本
#define PAGE12_SURPLUSMINUTE          0x7B0          // 估算剩余时间


#define PAGE12_BAT_TYPE               0x750          //电池类型
#define PAGE12_REQ_I                  0x770          //需求电流
#define PAGE12_RATED_VOLUME           0x778          //额定容量
#define PAGE12_HIGH_TEMPERATURE       0x780          //最高温度
#define PAGE12_HIGH_VOLTAGE           0x788          //最高电压
#define PAGE12_REMAIN_TIME            0x7A0
// PAGE 13 页
#define PAGE13_BAT_MODE               0x6C0          //辅源类型
#define PAGE13_CHANEG_MODE            0x6D0          //充电类型
#define PAGE13_REMAIN_TIME            0x7A8

#define PAGE14_AB_POWER      0x600             //AB当前电量
#define PAGE14_AB_REMAIN     0x608             //余额
#define PAGE14_AB_CH_TIME    0x610             //总耗时
#define PAGE14_AB_DATE       0x620             //日期
#define PAGE14_AB_COST       0x6A0             //花费金额

#define PAGE15_AB_LOCK       0x670             //电桩锁定
#define PAGE16_AB_FAILURE_ID 0x650             //失败ID

#define PAGE18_AB_CURRENT_MONEY 0x6B0         //当前金额
#define PAGE19_AB_ERR        0x640             //电桩故障



#define PAGE20_A_QR          0x00
#define PAGE20_A_DEVNUM      0x4A0
#define PAGE20_A_INSERTFLAG  0x468

#define PAGE21_A_POWER         0x400             //A当前电量
#define PAGE21_A_PERCENTAGE    0x408             //A充电完成百分比
#define PAGE21_A_COST          0x410             //A当前花费
#define PAGE21_A_I             0x418             //A电流
#define PAGE21_A_VOLTAGE       0x420             //A电压	
#define PAGE21_A_CH_TIME       0x448             //A充电时间

#define PAGE22_A_STOP_REASON   0x428             //A停止原因
#define PAGE22_A_CHARG_POWER   0x470             //A充电电量
#define PAGE22_A_CHARG_MONEY   0x478             //A 花费金额
#define PAGE22_A_CHARG_PROMPT  0x480             //A停止提示 
#define PAGE24_MONEY           0x800             //验证成功余额
#define PAGE25_FAILURE         0x808             //验证失败
#define PAGE26_TIME            0x810             //启动中时间	

#define PAGE30_TIP             0x748
#define PAGE30_TIME            0x740
#define PAGE31_BASE_RECORD_ID            0x900
#define PAGE31_BASE_RECORD_STARTTIME     0x910
#define PAGE31_BASE_RECORD_CH_TIME       0x930
#define PAGE31_BASE_RECORD_CH_POWER      0x950
#define PAGE31_BASE_RECORD_CH_MONEY      0x958
#define PAGE31_BASE_RECORD_ERR_CODE      0x960
#define PAGE31_STEP_LENGTH               0x80

#define PAGE32_CARDID_ADDR       0x0C00
#define PAGE32_STARTTYPE_ADDR    0x0C10
#define PAGE32_SUMMONEY_ADDR     0x0C20
#define PAGE32_STARTSOC_ADDR     0x0C28
#define PAGE32_STARTTIME_ADDR    0x0C30
#define PAGE32_SUMTIME_ADDR      0x0C50
#define PAGE32_CHGPORT_ADDR      0x0C60
#define PAGE32_SUMENERGY_ADDR    0x0C68
#define PAGE32_STOPCAUSE_ADDR    0x0C70  
#define PAGE32_STOPSOC_ADDR      0x0C78
#define PAGE32_STOPTIME_ADDR     0x0C80
#define PAGE32_STARTMONEY_ADDR   0x0CA0


#define PAGE33_UPDATA_PERSENT_ADDR         0x0700
#define PAGE33_UPDATA_TIP_ADDR             0x0708
#define PAGE33_UPDATA_PERSENT_ADDR_LEN      8

#define PAGE33_UPDATA_TIP_ADDR_LEN         8






/***************  长度 *******************/
#define PAGE0_VER_LEN            32
#define PAGE0_DAY_ADDR_LEN       32
#define PAGE_All_TIME_LEN         16             //时间长度
#define PAGE_ON_LINEFLAG_LEN       8            //在线显示长度

#define PAGE1_A_QR_LEN     			128
#define PAGE1_A_DEV_NUM_LEN  		32
#define PAGE1_A_CH_FLAG_LEN     8             //插枪标识长度
#define PAGE1_B_QR_LEN          128             //B二维码长度
#define PAGE1_B_DEV_NUM_LEN     32              //B设备号长度
#define PAGE1_B_CH_FLAG_LEN     8               //B插枪标识长度

#define PAGE2_A_QR_LEN     			128
#define PAGE2_A_DEV_NUM_LEN  		32
#define PAGE2_A_CH_FLAG_LEN     8             //插枪标识长度
#define PAGE2_B_STOP_REASON_LEN   64             //B停止原因长度
#define PAGE2_B_CHARG_POWER_LEN   8            //B 充电电量长度
#define PAGE2_B_CHARG_MONEY_LEN   8            //B 花费金额长度
#define PAGE2_B_CHARG_PROMPT_LEN  32           //B 停止提示长度

#define PAGE3_A_QR_LEN     		  	128
#define PAGE3_A_DEV_NUM_LEN  	   	32
#define PAGE3_A_CH_FLAG_LEN       8             //插枪标识长度
#define PAGE3_B_POWER_LEN         8             //B当前电量长度
#define PAGE3_B_PERCENTAGE_LEN    8             //B充电完成百分比
#define PAGE3_B_COST_LEN          8             //B当前花费长度
#define PAGE3_B_I_LEN             8             //B电流长度
#define PAGE3_B_VOLTAGE_LEN       8             //B电压长度
#define PAGE3_B_CH_TIME_LEN       32            //B充电时间长度


#define PAGE4_A_STOP_REASON_LEN   64           //停止原因长度
#define PAGE4_A_CHARG_POWER_LEN   8            //A 充电电量
#define PAGE4_A_CHARG_MONEY_LEN   8            //A 花费金额
#define PAGE4_A_CHARG_PROMPT_LEN  32           //A停止提示
#define PAGE4_B_QR_LEN           128             //B二维码长度
#define PAGE4_B_DEV_NUM_LEN      32              //B设备号长度
#define PAGE4_B_CH_FLAG_LEN      8               //B插枪标识长度


#define PAGE5_A_STOP_REASON_LEN   64           //停止原因长度
#define PAGE5_A_CHARG_POWER_LEN   8            //A 充电电量
#define PAGE5_A_CHARG_MONEY_LEN   8            //A 花费金额
#define PAGE5_A_CHARG_PROMPT_LEN  32           //A停止提示
#define PAGE5_B_POWER_LEN         8             //B当前电量长度
#define PAGE5_B_PERCENTAGE_LEN    8             //B充电完成百分比
#define PAGE5_B_COST_LEN          8             //B当前花费长度
#define PAGE5_B_I_LEN             8             //B电流长度
#define PAGE5_B_VOLTAGE_LEN       8             //B电压长度
#define PAGE5_B_CH_TIME_LEN       32            //B充电时间长度

#define PAGE6_A_STOP_REASON_LEN   64           //停止原因长度
#define PAGE6_A_CHARG_POWER_LEN   8            //A 充电电量
#define PAGE6_A_CHARG_MONEY_LEN   8            //A 花费金额
#define PAGE6_A_CHARG_PROMPT_LEN  32           //A停止提示
#define PAGE6_B_STOP_REASON_LEN   64           //B停止原因长度
#define PAGE6_B_CHARG_POWER_LEN   8            //B 充电电量长度
#define PAGE6_B_CHARG_MONEY_LEN   8            //B 花费金额长度
#define PAGE6_B_CHARG_PROMPT_LEN  32           //B 停止提示长度

#define PAGE7_A_POWER_LEN         8             //A当前电量长度
#define PAGE7_A_PERCENTAGE_LEN   8             //A充电完成百分比
#define PAGE7_A_COST_LEN          8             //A当前花费长度
#define PAGE7_A_I_LEN             8             //A电流长度
#define PAGE7_A_VOLTAGE_LEN       8             //A电压长度
#define PAGE7_A_CH_TIME_LEN       32            //A充电时间长度
#define PAGE7_B_STOP_REASON_LEN   64             //B停止原因长度
#define PAGE7_B_CHARG_POWER_LEN   8            //B 充电电量长度
#define PAGE7_B_CHARG_MONEY_LEN   8            //B 花费金额长度
#define PAGE7_B_CHARG_PROMPT_LEN  32           //B 停止提示长度


#define PAGE8_A_POWER_LEN         8             //A当前电量长度
#define PAGE8_A_PERCENTAGE_LEN    8             //A充电完成百分比
#define PAGE8_A_COST_LEN          8             //A当前花费长度
#define PAGE8_A_I_LEN             8             //A电流长度
#define PAGE8_A_VOLTAGE_LEN       8             //A电压长度
#define PAGE8_A_CH_TIME_LEN       32            //A充电时间长度
#define PAGE8_B_QR_LEN          128             //B二维码长度
#define PAGE8_B_DEV_NUM_LEN     32              //B设备号长度
#define PAGE8_B_CH_FLAG_LEN     8               //B插枪标识长度


#define PAGE9_A_POWER_LEN         8             //A当前电量长度
#define PAGE9_A_PERCENTAGE_LEN    8             //A充电完成百分比
#define PAGE9_A_COST_LEN          8             //A当前花费长度
#define PAGE9_A_I_LEN             8             //A电流长度
#define PAGE9_A_VOLTAGE_LEN       8             //A电压长度
#define PAGE9_A_CH_TIME_LEN       32            //A充电时间长度
#define PAGE9_B_POWER_LEN         8             //B当前电量长度
#define PAGE9_B_PERCENTAGE_LEN    8             //B充电完成百分比
#define PAGE9_B_COST_LEN          8             //B当前花费长度
#define PAGE9_B_I_LEN             8             //B电流长度
#define PAGE9_B_VOLTAGE_LEN       8             //B电压长度
#define PAGE9_B_CH_TIME_LEN       32            //B充电时间长度

// PAGE 11 页
#define PAGE12_CH_MODE_LEN         				16        //充电模式长度
#define PAGE12_REQ_VOLTAGE_LEN  		    	8         //需求点压长度
#define PAGE12_RATED_VOLTAGE_LEN  	 	  	8          //额定电压长度
#define PAGE12_MAX_ALLOW_VOLTAGE_LEN   		8         //最高允许单体电压长度
#define PAGE12_COMMUNICATION_VERSION_LEN  32          //通讯版本长度
#define PAGE12_SURPLUSMINUTE_LEN          8         //估算剩余时间
#define PAGE12_BAT_TYPE_LEN               32          //电池类型长度
#define PAGE12_REQ_I_LEN                  8          //需求电流长度
#define PAGE12_RATED_VOLUME_LEN           8          //额定容量长度
#define PAGE12_HIGH_TEMPERATURE_LEN       8         //最高温度长度
#define PAGE12_HIGH_VOLTAGE_LEN           8          //最高电压长度
#define PAGE12_REMAIN_TIME_LEN            2

#define PAGE13_BAT_MODE_LEN               16         //辅源类型长度
#define PAGE13_CHANEG_MODE_LEN            16          //充电类型长度
#define PAGE13_REMAIN_TIME_LEN            2

#define PAGE14_AB_POWER_LEN         8             //AB当前电量长度
#define PAGE14_AB_REMAIN_LEN        8             //余额长度
#define PAGE14_AB_CH_TIME_LEN       16             //总耗时长度
#define PAGE14_AB_DATE_LEN          32             //日期长度
#define PAGE14_AB_COST_LEN          8             //花费金额长度
#define PAGE15_AB_LOCK_LEN          32             //电桩锁定长度
#define PAGE16_AB_FAILURE_ID_LEN    16             //失败ID长度
#define PAGE18_AB_CURRENT_MONEY_LEN  8           //当前金额长度  
#define PAGE19_AB_ERR_LEN           16             //电桩故障长度


#define PAGE20_A_QR_LEN     			128
#define PAGE20_A_DEVNUM_LEN  		32
#define PAGE20_A_CH_FLAG_LEN      8             //插枪标识长度

#define PAGE21_A_POWER_LEN         8             //A当前电量长度
#define PAGE21_A_PERCENTAGE_LEN    8             //A充电完成百分比
#define PAGE21_A_COST_LEN          8             //A当前花费长度
#define PAGE21_A_I_LEN             8             //A电流长度
#define PAGE21_A_VOLTAGE_LEN       8             //A电压长度
#define PAGE21_A_CH_TIME_LEN       32            //A充电时间长度


#define PAGE22_A_STOP_REASON_LEN   64           //停止原因长度
#define PAGE22_A_CHARG_POWER_LEN   8            //A 充电电量
#define PAGE22_A_CHARG_MONEY_LEN   8            //A 花费金额
#define PAGE22_A_CHARG_PROMPT_LEN  32           //A停止提示   

#define PAGE24_MONEY_LEN     8
#define PAGE25_FAILURE_LEN   32
#define PAGE26_TIME_LEN      8 






#define PAGE30_TIP_LEN                       16
#define PAGE30_TIME_LEN                      8
#define PAGE31_BASE_RECORD_ID_LEN            16
#define PAGE31_BASE_RECORD_STARTTIME_LEN     32
#define PAGE31_BASE_RECORD_CH_TIME_LEN       32
#define PAGE31_BASE_RECORD_CH_POWER_LEN      8
#define PAGE31_BASE_RECORD_CH_MONEY_LEN      8
#define PAGE31_BASE_RECORD_ERR_CODE_LEN      8




#define PAGE32_CARDID_ADDR_LEN       16
#define PAGE32_STARTTYPE_ADDR_LEN    16
#define PAGE32_SUMMONEY_ADDR_LEN      8
#define PAGE32_STARTSOC_ADDR_LEN      8
#define PAGE32_STARTTIME_ADDR_LEN     32
#define PAGE32_SUMTIME_ADDR_LEN       16
#define PAGE32_CHGPORT_ADDR_LEN       8
#define PAGE32_SUMENERGY_ADDR_LEN     8
#define PAGE32_STOPCAUSE_ADDR_LEN     8  
#define PAGE32_STOPSOC_ADDR_LEN       8
#define PAGE32_STOPTIME_ADDR_LEN      32
#define PAGE32_STARTMONEY_ADDR_LEN    8

#define SHOW_LOG             0x0401
#define RETURN_BUTTON        0x0700   
#define OK_BUTTON            0x0790
#define NEXT_PAGE_BUTTON     0x0710
#define PRE_PAGR_BUTTON      0x0711

#define LINE1_BUTTON         0x0712
#define LINE2_BUTTON         0x0713
#define LINE3_BUTTON         0x0714
#define LINE4_BUTTON         0x0715
#define LINE5_BUTTON         0x0716




#define PAGE3_B_MORE         0x0301
#define PAGE5_B_MORE         0x0501
#define PAGE7_A_MORE         0x0701
#define PAGE8_A_MORE         0x0801
#define PAGE9_A_MORE         0x0901
#define PAGE9_B_MORE         0x0902

#define PAGE11_OK            0x0790                  //数据录入地址

#define PAGE12_BACK          0x0C01
#define PAGE13_START_WORK    0x0D01
#define PAGE13_AUTO_SW       0x0D02
#define PAGE13_12V           0x0D03
#define PAGE13_24V           0x0D04
#define PAGE13_AUTO_FULL     0x0D05
#define PAGE13_MONEY         0x0D06
#define PAGE13_POWER         0x0D07
#define PAGE13_TIME          0x0D08

#define PAGE21_MORE          0x1501






#define PAGE3_B_MORE         0x0301
#define PAGE5_B_MORE         0x0501
#define PAGE7_A_MORE         0x0701
#define PAGE8_A_MORE         0x0801
#define PAGE9_A_MORE         0x0901
#define PAGE9_B_MORE         0x0902

#define PAGE11_OK            0x0790                  //数据录入地址

#define PAGE12_BACK          0x0C01
#define PAGE13_START_WORK    0x0D01
#define PAGE13_AUTO_SW       0x0D02
#define PAGE13_12V           0x0D03
#define PAGE13_24V           0x0D04
#define PAGE13_AUTO_FULL     0x0D05
#define PAGE13_MONEY         0x0D06
#define PAGE13_POWER         0x0D07
#define PAGE13_TIME          0x0D08

#define PAGE21_MORE          0x1501


void XRD_Load_Page(uint16_t page);// 切换图片
void XRD_Write_Addr(uint16_t addr, char* str, uint8_t slen);
void XRD_Draw_Ioc(uint16_t addr, uint16_t num);
void XRDClearInput(void);
uint8_t GetTouchInfo(uint8_t* strbuf, uint8_t strlen, uint16_t* keyfunc);

#endif


