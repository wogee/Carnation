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


#define KEY_RETURN   0x800             //��������ֵ�õ�ַ
#define KEY_INPUT    0x790             //��������ֵ�õ�ַ


#define PAGE0       0x00    //��������
#define PAGE1       0x01    //A������B����
#define PAGE2       0x02    //A������Bֹͣ
#define PAGE3       0x03    //A������B���
#define PAGE4       0x04    //Aֹͣ��B����
#define PAGE5       0x05    //Aֹͣ��B���
#define PAGE6       0x06    //Aֹͣ��Bֹͣ
#define PAGE7       0x07    //A��磬Bֹͣ
#define PAGE8       0x08    //A��磬B����
#define PAGE9       0x09    //A��磬B���
#define PAGE10      0x0A    //������ʾ
#define PAGE11      0x0B    //��������
#define PAGE12      0x0C    //������Ϣ
#define PAGE13      0x0D    //��緽ʽѡ��
#define PAGE14      0x0E    //����
#define PAGE15      0x0F    //����
#define PAGE16      0x10    //����ʧ��
#define PAGE17      0x11    //�޳��λ�����ǹ����
#define PAGE18      0x12    //���
#define PAGE19      0x13    //������
#define PAGE20      0x14    //��ǹ����
#define PAGE21      0x15    //��ǹ�����
#define PAGE22      0x16    //���ֹͣ
#define PAGE23      0x17      //��֤��
#define PAGE24      0x18  		//��֤�ɹ�
#define PAGE25      0x19		  //��֤ʧ��
#define PAGE26      0x1A      //��������У���Ե�����
#define PAGE27      0x1B      //������
#define PAGE28      0x1C      //�����ɹ�
#define PAGE29      0x1D      //�������ʧ��    
#define PAGE30      0x1E      //��������
#define PAGE31      0x1F      //����¼
#define PAGE32      0x20      //����¼����
#define PAGE33      0x21      //��׮����

#define PAGE_A_ERRID         0x720
#define PAGE_B_ERRID         0x730

#define PAGE_A_ERRID_LEN        16
#define PAGE_B_ERRID_LEN        16

#define PAGE_All_TIME        0x690             //ʱ��
#define PAGE_ON_LINEFLAG     0x6A8             //������ʾ

#define PAGE0_VER_ADDR      0x700
#define PAGE0_DAY_ADDR      0x720							//������

#define PAGE1_A_QR          0x00              //Aǹ��ά��
#define PAGE1_A_DEVNUM      0x4A0             //Aǹ�豸���
#define PAGE1_A_INSERTFLAG  0x468             //Aǹ��ǹ��־

#define PAGE1_B_QR          0x200             //Bǹ��ά��       
#define PAGE1_B_DEVNUM      0x5A0             //Bǹ�豸���
#define PAGE1_B_INSERTFLAG  0x568             //Bǹ��ǹ��־

#define PAGE2_A_QR           0x00             //Aǹ��ά��
#define PAGE2_A_DEVNUM       0x4A0            //Aǹ�豸���
#define PAGE2_A_INSERTFLAG  0x468             //Aǹ��ǹ��־
#define PAGE2_B_STOP_REASON  0x528            //Bǹֹͣԭ��
#define PAGE2_B_CHARG_POWER  0x570            //B������
#define PAGE2_B_CHARG_MONEY  0x578            //B���ѽ��
#define PAGE2_B_CHARG_PROMPT 0x580            //Bֹͣ��ʾ

#define PAGE3_A_QR          0x00              //Aǹ��ά��
#define PAGE3_A_DEVNUM      0x4A0             //Aǹ�豸���
#define PAGE3_A_INSERTFLAG  0x468             //Aǹ��ǹ��־

#define PAGE3_B_POWER         0x500             //B��ǰ����
#define PAGE3_B_PERCENTAGE    0x508             //B�����ɰٷֱ�
#define PAGE3_B_COST          0x510             //B��ǰ����
#define PAGE3_B_I             0x518             //B����
#define PAGE3_B_VOLTAGE       0x520             //B��ѹ
#define PAGE3_B_CH_TIME       0x548             //B���ʱ�� 

#define PAGE4_A_STOP_REASON   0x428             //Aֹͣԭ��
#define PAGE4_A_CHARG_POWER   0x470             //A������
#define PAGE4_A_CHARG_MONEY   0x478             //A���ѽ��
#define PAGE4_A_CHARG_PROMPT  0x480             //Aֹͣ��ʾ 
#define PAGE4_B_QR            0x200             //Bǹ��ά�� 
#define PAGE4_B_DEVNUM        0x5A0             //Bǹ�豸���
#define PAGE4_B_INSERTFLAG    0x568             //Bǹ��ǹ��־

#define PAGE5_A_STOP_REASON   0x428             //Aֹͣԭ��
#define PAGE5_A_CHARG_POWER   0x470             //A������
#define PAGE5_A_CHARG_MONEY   0x478             //A ���ѽ��
#define PAGE5_A_CHARG_PROMPT  0x480             //Aֹͣ��ʾ 

#define PAGE5_B_POWER         0x500             //B��ǰ����
#define PAGE5_B_PERCENTAGE    0x508             //B�����ɰٷֱ�
#define PAGE5_B_COST          0x510             //B��ǰ����
#define PAGE5_B_I             0x518             //B����
#define PAGE5_B_VOLTAGE       0x520             //B��ѹ	
#define PAGE5_B_CH_TIME       0x548             //B���ʱ��

#define PAGE6_A_STOP_REASON   0x428             //Aֹͣԭ��
#define PAGE6_A_CHARG_POWER   0x470             //A������
#define PAGE6_A_CHARG_MONEY   0x478             //A ���ѽ��
#define PAGE6_A_CHARG_PROMPT  0x480             //Aֹͣ��ʾ 
#define PAGE6_B_STOP_REASON   0x528             //Bֹͣԭ��
#define PAGE6_B_CHARG_POWER   0x570             //B������
#define PAGE6_B_CHARG_MONEY   0x578             //B ���ѽ��
#define PAGE6_B_CHARG_PROMPT  0x580             //Bֹͣ��ʾ 

#define PAGE7_A_POWER         0x400             //A��ǰ����
#define PAGE7_A_PERCENTAGE    0x408             //A�����ɰٷֱ�
#define PAGE7_A_COST          0x410             //A��ǰ����
#define PAGE7_A_I             0x418             //A����
#define PAGE7_A_VOLTAGE       0x420             //A��ѹ	
#define PAGE7_A_CH_TIME       0x448             //A���ʱ��
#define PAGE7_B_STOP_REASON   0x528             //Bֹͣԭ��
#define PAGE7_B_CHARG_POWER   0x570             //B������
#define PAGE7_B_CHARG_MONEY   0x578             //B ���ѽ��
#define PAGE7_B_CHARG_PROMPT  0x580             //Bֹͣ��ʾ 	

#define PAGE8_A_POWER         0x400             //A��ǰ����
#define PAGE8_A_PERCENTAGE    0x408             //A�����ɰٷֱ�
#define PAGE8_A_COST          0x410             //A��ǰ����
#define PAGE8_A_I             0x418             //A����
#define PAGE8_A_VOLTAGE       0x420             //A��ѹ	
#define PAGE8_A_CH_TIME       0x448             //A���ʱ��
#define PAGE8_B_QR            0x200
#define PAGE8_B_DEVNUM        0x5A0
#define PAGE8_B_INSERTFLAG    0x568

#define PAGE9_A_POWER         0x400             //A��ǰ����
#define PAGE9_A_PERCENTAGE    0x408             //A�����ɰٷֱ�
#define PAGE9_A_COST          0x410             //A��ǰ����
#define PAGE9_A_I             0x418             //A����
#define PAGE9_A_VOLTAGE       0x420             //A��ѹ	
#define PAGE9_A_CH_TIME       0x448             //A���ʱ��
#define PAGE9_B_POWER         0x500             //B��ǰ����
#define PAGE9_B_PERCENTAGE    0x508             //B�����ɰٷֱ�
#define PAGE9_B_COST          0x510             //B��ǰ����
#define PAGE9_B_I             0x518             //B����
#define PAGE9_B_VOLTAGE       0x520             //B��ѹ	
#define PAGE9_B_CH_TIME       0x548             //B���ʱ��

// PAGE 11 ҳ
#define PAGE12_CH_MODE        				0x700          //���ģʽ
#define PAGE12_REQ_VOLTAGE 						0x710          //�����ѹ
#define PAGE12_RATED_VOLTAGE 	 	  		0x718          //���ѹ
#define PAGE12_MAX_ALLOW_VOLTAGE  		0x720          //����������ѹ
#define PAGE12_COMMUNICATION_VERSION 	0x730          //ͨѶ�汾
#define PAGE12_SURPLUSMINUTE          0x7B0          // ����ʣ��ʱ��


#define PAGE12_BAT_TYPE               0x750          //�������
#define PAGE12_REQ_I                  0x770          //�������
#define PAGE12_RATED_VOLUME           0x778          //�����
#define PAGE12_HIGH_TEMPERATURE       0x780          //����¶�
#define PAGE12_HIGH_VOLTAGE           0x788          //��ߵ�ѹ
#define PAGE12_REMAIN_TIME            0x7A0
// PAGE 13 ҳ
#define PAGE13_BAT_MODE               0x6C0          //��Դ����
#define PAGE13_CHANEG_MODE            0x6D0          //�������
#define PAGE13_REMAIN_TIME            0x7A8

#define PAGE14_AB_POWER      0x600             //AB��ǰ����
#define PAGE14_AB_REMAIN     0x608             //���
#define PAGE14_AB_CH_TIME    0x610             //�ܺ�ʱ
#define PAGE14_AB_DATE       0x620             //����
#define PAGE14_AB_COST       0x6A0             //���ѽ��

#define PAGE15_AB_LOCK       0x670             //��׮����
#define PAGE16_AB_FAILURE_ID 0x650             //ʧ��ID

#define PAGE18_AB_CURRENT_MONEY 0x6B0         //��ǰ���
#define PAGE19_AB_ERR        0x640             //��׮����



#define PAGE20_A_QR          0x00
#define PAGE20_A_DEVNUM      0x4A0
#define PAGE20_A_INSERTFLAG  0x468

#define PAGE21_A_POWER         0x400             //A��ǰ����
#define PAGE21_A_PERCENTAGE    0x408             //A�����ɰٷֱ�
#define PAGE21_A_COST          0x410             //A��ǰ����
#define PAGE21_A_I             0x418             //A����
#define PAGE21_A_VOLTAGE       0x420             //A��ѹ	
#define PAGE21_A_CH_TIME       0x448             //A���ʱ��

#define PAGE22_A_STOP_REASON   0x428             //Aֹͣԭ��
#define PAGE22_A_CHARG_POWER   0x470             //A������
#define PAGE22_A_CHARG_MONEY   0x478             //A ���ѽ��
#define PAGE22_A_CHARG_PROMPT  0x480             //Aֹͣ��ʾ 
#define PAGE24_MONEY           0x800             //��֤�ɹ����
#define PAGE25_FAILURE         0x808             //��֤ʧ��
#define PAGE26_TIME            0x810             //������ʱ��	

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






/***************  ���� *******************/
#define PAGE0_VER_LEN            32
#define PAGE0_DAY_ADDR_LEN       32
#define PAGE_All_TIME_LEN         16             //ʱ�䳤��
#define PAGE_ON_LINEFLAG_LEN       8            //������ʾ����

#define PAGE1_A_QR_LEN     			128
#define PAGE1_A_DEV_NUM_LEN  		32
#define PAGE1_A_CH_FLAG_LEN     8             //��ǹ��ʶ����
#define PAGE1_B_QR_LEN          128             //B��ά�볤��
#define PAGE1_B_DEV_NUM_LEN     32              //B�豸�ų���
#define PAGE1_B_CH_FLAG_LEN     8               //B��ǹ��ʶ����

#define PAGE2_A_QR_LEN     			128
#define PAGE2_A_DEV_NUM_LEN  		32
#define PAGE2_A_CH_FLAG_LEN     8             //��ǹ��ʶ����
#define PAGE2_B_STOP_REASON_LEN   64             //Bֹͣԭ�򳤶�
#define PAGE2_B_CHARG_POWER_LEN   8            //B ����������
#define PAGE2_B_CHARG_MONEY_LEN   8            //B ���ѽ���
#define PAGE2_B_CHARG_PROMPT_LEN  32           //B ֹͣ��ʾ����

#define PAGE3_A_QR_LEN     		  	128
#define PAGE3_A_DEV_NUM_LEN  	   	32
#define PAGE3_A_CH_FLAG_LEN       8             //��ǹ��ʶ����
#define PAGE3_B_POWER_LEN         8             //B��ǰ��������
#define PAGE3_B_PERCENTAGE_LEN    8             //B�����ɰٷֱ�
#define PAGE3_B_COST_LEN          8             //B��ǰ���ѳ���
#define PAGE3_B_I_LEN             8             //B��������
#define PAGE3_B_VOLTAGE_LEN       8             //B��ѹ����
#define PAGE3_B_CH_TIME_LEN       32            //B���ʱ�䳤��


#define PAGE4_A_STOP_REASON_LEN   64           //ֹͣԭ�򳤶�
#define PAGE4_A_CHARG_POWER_LEN   8            //A ������
#define PAGE4_A_CHARG_MONEY_LEN   8            //A ���ѽ��
#define PAGE4_A_CHARG_PROMPT_LEN  32           //Aֹͣ��ʾ
#define PAGE4_B_QR_LEN           128             //B��ά�볤��
#define PAGE4_B_DEV_NUM_LEN      32              //B�豸�ų���
#define PAGE4_B_CH_FLAG_LEN      8               //B��ǹ��ʶ����


#define PAGE5_A_STOP_REASON_LEN   64           //ֹͣԭ�򳤶�
#define PAGE5_A_CHARG_POWER_LEN   8            //A ������
#define PAGE5_A_CHARG_MONEY_LEN   8            //A ���ѽ��
#define PAGE5_A_CHARG_PROMPT_LEN  32           //Aֹͣ��ʾ
#define PAGE5_B_POWER_LEN         8             //B��ǰ��������
#define PAGE5_B_PERCENTAGE_LEN    8             //B�����ɰٷֱ�
#define PAGE5_B_COST_LEN          8             //B��ǰ���ѳ���
#define PAGE5_B_I_LEN             8             //B��������
#define PAGE5_B_VOLTAGE_LEN       8             //B��ѹ����
#define PAGE5_B_CH_TIME_LEN       32            //B���ʱ�䳤��

#define PAGE6_A_STOP_REASON_LEN   64           //ֹͣԭ�򳤶�
#define PAGE6_A_CHARG_POWER_LEN   8            //A ������
#define PAGE6_A_CHARG_MONEY_LEN   8            //A ���ѽ��
#define PAGE6_A_CHARG_PROMPT_LEN  32           //Aֹͣ��ʾ
#define PAGE6_B_STOP_REASON_LEN   64           //Bֹͣԭ�򳤶�
#define PAGE6_B_CHARG_POWER_LEN   8            //B ����������
#define PAGE6_B_CHARG_MONEY_LEN   8            //B ���ѽ���
#define PAGE6_B_CHARG_PROMPT_LEN  32           //B ֹͣ��ʾ����

#define PAGE7_A_POWER_LEN         8             //A��ǰ��������
#define PAGE7_A_PERCENTAGE_LEN   8             //A�����ɰٷֱ�
#define PAGE7_A_COST_LEN          8             //A��ǰ���ѳ���
#define PAGE7_A_I_LEN             8             //A��������
#define PAGE7_A_VOLTAGE_LEN       8             //A��ѹ����
#define PAGE7_A_CH_TIME_LEN       32            //A���ʱ�䳤��
#define PAGE7_B_STOP_REASON_LEN   64             //Bֹͣԭ�򳤶�
#define PAGE7_B_CHARG_POWER_LEN   8            //B ����������
#define PAGE7_B_CHARG_MONEY_LEN   8            //B ���ѽ���
#define PAGE7_B_CHARG_PROMPT_LEN  32           //B ֹͣ��ʾ����


#define PAGE8_A_POWER_LEN         8             //A��ǰ��������
#define PAGE8_A_PERCENTAGE_LEN    8             //A�����ɰٷֱ�
#define PAGE8_A_COST_LEN          8             //A��ǰ���ѳ���
#define PAGE8_A_I_LEN             8             //A��������
#define PAGE8_A_VOLTAGE_LEN       8             //A��ѹ����
#define PAGE8_A_CH_TIME_LEN       32            //A���ʱ�䳤��
#define PAGE8_B_QR_LEN          128             //B��ά�볤��
#define PAGE8_B_DEV_NUM_LEN     32              //B�豸�ų���
#define PAGE8_B_CH_FLAG_LEN     8               //B��ǹ��ʶ����


#define PAGE9_A_POWER_LEN         8             //A��ǰ��������
#define PAGE9_A_PERCENTAGE_LEN    8             //A�����ɰٷֱ�
#define PAGE9_A_COST_LEN          8             //A��ǰ���ѳ���
#define PAGE9_A_I_LEN             8             //A��������
#define PAGE9_A_VOLTAGE_LEN       8             //A��ѹ����
#define PAGE9_A_CH_TIME_LEN       32            //A���ʱ�䳤��
#define PAGE9_B_POWER_LEN         8             //B��ǰ��������
#define PAGE9_B_PERCENTAGE_LEN    8             //B�����ɰٷֱ�
#define PAGE9_B_COST_LEN          8             //B��ǰ���ѳ���
#define PAGE9_B_I_LEN             8             //B��������
#define PAGE9_B_VOLTAGE_LEN       8             //B��ѹ����
#define PAGE9_B_CH_TIME_LEN       32            //B���ʱ�䳤��

// PAGE 11 ҳ
#define PAGE12_CH_MODE_LEN         				16        //���ģʽ����
#define PAGE12_REQ_VOLTAGE_LEN  		    	8         //�����ѹ����
#define PAGE12_RATED_VOLTAGE_LEN  	 	  	8          //���ѹ����
#define PAGE12_MAX_ALLOW_VOLTAGE_LEN   		8         //����������ѹ����
#define PAGE12_COMMUNICATION_VERSION_LEN  32          //ͨѶ�汾����
#define PAGE12_SURPLUSMINUTE_LEN          8         //����ʣ��ʱ��
#define PAGE12_BAT_TYPE_LEN               32          //������ͳ���
#define PAGE12_REQ_I_LEN                  8          //�����������
#define PAGE12_RATED_VOLUME_LEN           8          //���������
#define PAGE12_HIGH_TEMPERATURE_LEN       8         //����¶ȳ���
#define PAGE12_HIGH_VOLTAGE_LEN           8          //��ߵ�ѹ����
#define PAGE12_REMAIN_TIME_LEN            2

#define PAGE13_BAT_MODE_LEN               16         //��Դ���ͳ���
#define PAGE13_CHANEG_MODE_LEN            16          //������ͳ���
#define PAGE13_REMAIN_TIME_LEN            2

#define PAGE14_AB_POWER_LEN         8             //AB��ǰ��������
#define PAGE14_AB_REMAIN_LEN        8             //����
#define PAGE14_AB_CH_TIME_LEN       16             //�ܺ�ʱ����
#define PAGE14_AB_DATE_LEN          32             //���ڳ���
#define PAGE14_AB_COST_LEN          8             //���ѽ���
#define PAGE15_AB_LOCK_LEN          32             //��׮��������
#define PAGE16_AB_FAILURE_ID_LEN    16             //ʧ��ID����
#define PAGE18_AB_CURRENT_MONEY_LEN  8           //��ǰ����  
#define PAGE19_AB_ERR_LEN           16             //��׮���ϳ���


#define PAGE20_A_QR_LEN     			128
#define PAGE20_A_DEVNUM_LEN  		32
#define PAGE20_A_CH_FLAG_LEN      8             //��ǹ��ʶ����

#define PAGE21_A_POWER_LEN         8             //A��ǰ��������
#define PAGE21_A_PERCENTAGE_LEN    8             //A�����ɰٷֱ�
#define PAGE21_A_COST_LEN          8             //A��ǰ���ѳ���
#define PAGE21_A_I_LEN             8             //A��������
#define PAGE21_A_VOLTAGE_LEN       8             //A��ѹ����
#define PAGE21_A_CH_TIME_LEN       32            //A���ʱ�䳤��


#define PAGE22_A_STOP_REASON_LEN   64           //ֹͣԭ�򳤶�
#define PAGE22_A_CHARG_POWER_LEN   8            //A ������
#define PAGE22_A_CHARG_MONEY_LEN   8            //A ���ѽ��
#define PAGE22_A_CHARG_PROMPT_LEN  32           //Aֹͣ��ʾ   

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

#define PAGE11_OK            0x0790                  //����¼���ַ

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

#define PAGE11_OK            0x0790                  //����¼���ַ

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


void XRD_Load_Page(uint16_t page);// �л�ͼƬ
void XRD_Write_Addr(uint16_t addr, char* str, uint8_t slen);
void XRD_Draw_Ioc(uint16_t addr, uint16_t num);
void XRDClearInput(void);
uint8_t GetTouchInfo(uint8_t* strbuf, uint8_t strlen, uint16_t* keyfunc);

#endif


