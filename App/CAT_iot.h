#ifndef _CAT_IOT_H
#define _CAT_IOT_H


#include "stm32f4xx_hal.h"
#include "bsp_rtc.h"
#include "msg_def.h"





/*�궨����********************************************************/
#define CAT_RECE_BUF_SIZE									4095					//�����CATģ����յ�������ѭ��������д�С������Ϊ�˽���&���㣬����ʵ�ʶ��г�����Ҫ��+1



//#define MQTT_SERVICE_IP									"broker.emqx.io"		//����MQTT�����ʹ�õ���������IP��ַ
//#define MQTT_SERVICE_PORT									1883					//����MQTT�����ʹ�õĶ˿ں�
//#define MQTT_SERVICE_USER_NAME								"CreateCareMQTT"		//����MQTT�����ʹ�õĵ�¼�û���
//#define MQTT_SERVICE_PASSWORD								"KaiFa0098"				//����MQTT�����ʹ�õĵ�¼����

#define MQTT_SERVICE_IP										"120.78.135.48"			//����MQTT�����ʹ�õ���������IP��ַ
#define MQTT_SERVICE_PORT									1883					//����MQTT�����ʹ�õĶ˿ں�
#define MQTT_SERVICE_USER_NAME								"CCAdmin"				//����MQTT�����ʹ�õĵ�¼�û���
#define MQTT_SERVICE_PASSWORD								"HWQ00968163HB"			//����MQTT�����ʹ�õĵ�¼����

#define FTP_SERVICE_IP										"120.78.135.48"			//����FTP�����ʹ�õ���������IP��ַ
#define FTP_SERVICE_PORT									21						//����FTP�����ʹ�õĶ˿ں�
#define FTP_SERVICE_USER_NAME								"CreateCare"			//����FTP�����ʹ�õĵ�¼�û���
#define FTP_SERVICE_PASSWORD								"huangweiqiang0098Hb"	//����FTP�����ʹ�õĵ�¼����

#define GAODE_KEY_POSITION									"271e23f771fe86c306a2b53cdef935cc"	//��������CAT��λ��Ҫʹ�õĸߵ�KEY

//��ʱʹ�õĵ��������������
//#define MachInfo.companyInfo.mqtt.ip								"101.33.204.141"		//���������MQTT�����ʹ�õ���������IP��ַ
//#define MQTT_SERVICE_PORT							1883					//���������MQTT�����ʹ�õĶ˿ں�
//#define MachInfo.companyInfo.mqtt.name						"yaoShiBang"			//���������MQTT�����ʹ�õĵ�¼�û���
//#define MachInfo.companyInfo.mqtt.psw						"yaoShiBang_#374*_Cc"	//���������MQTT�����ʹ�õĵ�¼����
//#define MachInfo.companyInfo.mqtt.ip								"ch-mqtt.guangpuyun.cn"	//���������MQTT�����ʹ�õ���������IP��ַ
//#define MQTT_SERVICE_PORT								1883					//���������MQTT�����ʹ�õĶ˿ں�
//#define MachInfo.companyInfo.mqtt.name						"yaoShiBang"			//���������MQTT�����ʹ�õĵ�¼�û���
//#define MachInfo.companyInfo.mqtt.psw							"yaoShiBang_#374*_Cc"	//���������MQTT�����ʹ�õĵ�¼����






/***
*�����CATģ����յ�������ѭ��������нṹ��
***/
typedef struct _CAT_ReceBuf_s
{
	uint16_t readIndex;
	uint16_t writeIndex;
	
	char buf[CAT_RECE_BUF_SIZE+1];
}CAT_ReceBuf_s;


/***
*���嵱ǰʹ�õ�CATģ���ͺ�ö��
***/
typedef enum _CAT_Type_e{
	CAT_YK = 0,						//ʹ���ƿ�CAT
	CAT_ZY,							//ʹ������CAT
}CAT_Type_e;


/***
*���嵱ǰҪ���ӵķ����ö��
***/
typedef enum _CAT_SERVICE_e{
	SERVICE_CC = 0,					//���������
	SERVICE_OTHER,					//�����������
}CAT_SERVICE_e;


/***
*���嵱ǰע��״̬
***/
typedef enum _CAT_RegistNetStatus_e{
	CAT_REGISTNET_UNKNOW = 0,							//��ʾδ֪״̬
	CAT_REGISTNET_SUCCESS,								//��ʾע���ɹ�
	CAT_REGISTNET_CHECKING,								//��ʾע��ʧ�ܣ�����������
	CAT_REGISTNET_SIM_OVERDUE,							//��ʾע��ʧ�ܣ�SIM������
}CAT_RegistNetStatus_e;



/***
*���嵱ǰ���͵���������������Ŀ����ö��
***/
typedef enum _CAT_TestType_e{
	TEST_TYPE_WBC_HGB = 0,								//WBC+HGB����
}CAT_TestType_e;




/***
*����CATģ��ȫ��״̬���ṹ��
***/
typedef struct _CATGlobalStatus_s{
	struct{
		uint16_t powOn:						1;				//CATģ���Ƿ��ϵ����
		uint16_t conn:						1;				//CATģ���MCU�����Ƿ�����
		uint16_t sim:						1;				//�Ƿ��п�
		uint16_t connServerErr:				1;				//�Ƿ����ӷ�����쳣
		uint16_t connOtherServerErr:		1;				//�Ƿ����ӵ�����������쳣
		
		uint16_t tcpConn:					1;				//�ж��Ƿ񴴽���TCP����
		uint16_t ftpConn:					1;				//�ж��Ƿ񴴽���FTP����
		uint16_t mqttConn:					1;				//�ж��Ƿ����ӵ�����˵�MQTT�ɹ�
		
		uint16_t noOtherServer:				1;				//�ж��Ƿ���ڵ���������ˣ�Ϊ0��ʾ���ڣ�Ϊ1��ʾ������
	}flag;
	
	CAT_RegistNetStatus_e registerNet;						//������ʾ��ǰע��״̬
	uint32_t catDealSysRestartFlag;							//������ʾCAT������һЩ��������Ҫ���������ı�־��Ϊ0xA55A0FF0ʱ��ʾ��Ҫ��������������
	CAT_Type_e catType;										//������ʾ��ǰʹ�õ����ĸ����ҵ�CATģ��
	uint8_t operatorNum;									//��Ӫ�̱�ţ�Ϊ0��ʾ�й����ţ�Ϊ1��ʾ�й��ƶ���AT+COPS?ָ���ֵ�У�46011��ʾ�����й����ţ�46000��ʾ�����й��ƶ�
	uint8_t signalStrength;									//�ź�ǿ�ȣ���ֵԽС�ź�Խ����99ֵ��ʾû���ź�
	char imei[16];											//ȫ���ƶ��豸��IMEI��15λ�ַ���
	char iccid[21];											//SIM��ICCID,20λ�ַ���
	char sn[15];											//ģ��SN��
	char position[25];										//ģ��λ����Ϣ
	
	//������е�ǰ����ĸ�ģ��汾��
	struct{
		uint16_t PMainVersion;                            	//������б���ķ�����������汾��
		uint16_t PBootloadVersion;                       	//������б���ķ�������������汾��
		uint16_t PUiVersion;                              	//������б���ķ�����UI�汾��
		
		uint16_t TMainVersion;                            	//������б���Ĳ��԰�������汾��
		uint16_t TBootloadVersion;                       	//������б���Ĳ��԰���������汾��
		uint16_t TUiVersion;                              	//������б���Ĳ��԰�UI�汾��
	}serviceVersion;
	
	//���ص�ǰ����ĸ�ģ��汾��
	struct{
		uint16_t mainVersion;
		uint16_t bootloadVersion;
		uint16_t uiVersion;
	}localVersion;
	
	//���������ִ�е����⹦�����־
	struct{
		uint8_t JSCDect;									//�����������Լ����ؼ�⹦�ܱ�־
		uint8_t OCDect;										//�����������Թ����⹦�ܱ�־
		uint8_t JSCReuse;									//���������������ظ�ʹ�ù��ܱ�־
	}ignoreFunFlag;
	
	//�������Ӧ��ָ�������ļ���Ϣ
	struct{
		uint8_t publishOrTestFlag;	
		uint8_t softwareType;	
		uint32_t totalLen;
		uint16_t crc16;
	}softwareInfo;
	
	//�������Ӧ��ָ���汾��UI��Դ��Ϣ
	struct{
		uint8_t publishOrTestFlag;
		uint16_t uiVersion;									
		uint8_t totalNum;
		
		struct{
			uint32_t fileSize;
			uint16_t crc16;
			char path[70];
		}file[10];
	}uiInfo;
}CATGlobalStatus_s;




/***
*����MQTTͨ�����ݽṹ��
***/
typedef __packed struct _MQTT_PayloadBuf_s{
	uint16_t cmdH;
	uint8_t cmdM;
	uint8_t cmdL;
	
	long cmdTimestamp;											//ָ���ʱ���
	
	uint16_t packetNum;											//�����
	uint16_t crc16;												//CRC16
	uint16_t payloadLen;										//�������ݳ���

	//��֡��������������
	__packed union{
		char str[4200];											//��������ַ���
		
		uint8_t u8;												//�������һ���޷���8λ�Ĳ���
		uint16_t u16;											//�������һ���޷���16λ�Ĳ���
		uint32_t u32;											//�������һ���޷���32λ�Ĳ���
		int8_t i8;												//�������һ���з���8λ�Ĳ���
		int16_t i16;											//�������һ���з���16λ�Ĳ���
		int32_t i32;											//�������һ���з���32λ�Ĳ���
		
		//�������в�����Ϣ֡��ָ��2.2.1
		__packed struct{
			uint16_t softwareVersion;							//��������汾��
			uint16_t mainVersion;                               //������汾��
			uint16_t bootloadVersion;                           //��������汾��
			uint16_t uiVersion;                                 //UI�汾��
			uint16_t algoVersion;                               //�㷨�汾��
			char mainHardwareVersion[5];                    	//���ذ�Ӳ���汾�ţ�"MA00"
			char zjHardwareVersion[5];                      	//ת�Ӱ�Ӳ���汾�ţ�"DA00"
			char imei[16];                                      //IMEI��
			char iccid[21];                                      //ICCID��
			uint8_t operatorNum;                                //��Ӫ�̴��룬0��ʾ�й��ƶ���1��ʾ�й�����
			uint8_t languMode;                                  //����ģʽ
			uint8_t lockFlag;									//�����Ƿ�������״̬��Ϊ0��ʾ������Ϊ0xA5��ʾ����״̬
			long factoryTimestamp;                         		//��������ʱ�������һ��¼���������кŵ�ʱ��
			long userFirstOpenTimestamp;                        //�û���һ�ο���ʱ���
			Company_e company;									//���ڱ�ʾ��ǰ���������Ĺ�˾
			char zhensuoName[CLINIC_NAME_MAX_LEN];				//������
																
			uint8_t wbcPara;                                    //WBC���油��
			uint8_t hgbPara;                                    //HGB���油��
			uint8_t pumpPara;                                   //��ռ�ձ�ֵ
			uint16_t current535;		    					//535��������������ֵ
			uint16_t current415;		    					//415��������������ֵ
			uint16_t outOC;                                     //���ֹ����ֵ
			uint16_t inOC;                                      //���ֹ����ֵ
			uint16_t gdOC;                                      //�̶������ֵ
			uint16_t sfOC;                                      //�ͷŹ����ֵ
			int16_t presurePara;                                //ѹ������ֵ
			uint16_t wbcAdjust;                                 //WBCУ׼ϵ��ֵ
			uint16_t hgbAdjust;                                 //HGBУ׼ϵ��ֵ
			uint8_t pressureSensor;								//ģ��ѹ������������
			CAT_Type_e catType;									//��ǰʹ�õ�CATģ������
			uint8_t downLoadInfoFlag;							//�Ƿ�������ǰ����������Ϣ�ı�־��Ϊ1��ʾ����������ֵ��ʾδ����
		}cmd2_2_1;
		
		//������ǰ������Ϣ��ָ��2.2.4
		__packed struct{
			uint32_t totalOpenCount;							//��ǰ�����ܴ���
			char position[22];									//λ����Ϣ
			uint8_t signalStrength;								//�ź�ǿ��
			uint16_t softwareVersion;							//��������汾��
			uint16_t mainVersion;                               //������汾��
			uint16_t bootloadVersion;                           //��������汾��
			uint16_t uiVersion;                                 //UI�汾��
			char imei[16];                                      //IMEI��
			char iccid[21];                                      //ICCID��
			CAT_Type_e catType;									//��ǰʹ�õ�CATģ������
		}cmd2_2_4;
		
		//������е�ǰ����ĸ������汾����Ϣ��ָ��2.2.5
		__packed struct{
			uint16_t mainVersion;                               //������汾��
			uint16_t bootloadVersion;                           //��������汾��
			uint16_t uiVersion;                                 //UI�汾��
		}cmd2_2_5;
		
		//�豸�����˷����޸�������Ϣָ�ָ��2.2.8
		__packed struct{
			uint16_t infoType;                               	//���ڱ�ʾ��ǰҪ�޸ĵ���Ϣ����
			uint8_t info[70];                          		 	//�޸ĵ�����ֵ
		}cmd2_2_8;
		
		//��������豸��Ӧ��ǰ������е����°汾�ţ�ָ��2.3.2
		__packed struct{
			uint8_t publishOrTestFlag;							//������Ƿ����滹�ǲ��԰棬Ϊ0��ʾ���󷢲�����Ϣ��Ϊ1��ʾ������԰���Ϣ
			uint16_t mainVersion;
			uint16_t bootloadVersion;
			uint16_t uiVersion;
		}cmd2_3_2;

		
		//�豸��������������ļ���Ϣ��ָ��2.3.3
		__packed struct{
			uint8_t publishOrTestFlag;							//������Ƿ����滹�ǲ��԰棬Ϊ0��ʾ���󷢲�����Ϣ��Ϊ1��ʾ������԰���Ϣ
			uint8_t softwareType;								//��������������ļ��������������ļ���Ϊ0��ʾ�����������ļ���Ϊ1��ʾ�������������ļ�
		}cmd2_3_3;
		
		//�������Ӧ�豸����ĳ����ļ���Ϣ��ָ��2.3.4
		__packed struct{
			uint8_t publishOrTestFlag;	
			uint8_t softwareType;	
			uint32_t totalLen;
			uint16_t crc16;
		}cmd2_3_4;
		
		//�豸����������ָ���汾��UI��Դ��ָ��2.3.5
		__packed struct{
			uint8_t publishOrTestFlag;							//������Ƿ����滹�ǲ��԰棬Ϊ0��ʾ���󷢲�����Ϣ��Ϊ1��ʾ������԰���Ϣ
			uint16_t uiVersion;									//�����UI��Դ�汾��
		}cmd2_3_5;
		
		//�������Ӧ�豸�����ָ���汾�ŵ�UI��Դ��ָ��2.3.6
		__packed struct{
			uint8_t publishOrTestFlag;	
			uint16_t uiVersion;									
			uint8_t totalNum;
			
			__packed struct{
				uint32_t fileSize;
				uint16_t crc16;
				char path[70];
			}file[10];
		}cmd2_3_6;
		
		//����������������������Ϣ��ָ��2.5.1
		__packed struct{
			uint16_t infoType;                               	//���ڱ�ʾ��ǰҪ�޸ĵ���Ϣ����
			
			//�޸ĵ�����ֵ
			__packed union{
				char zhensuoName[70];
				
				uint8_t lockFlag;								//����״̬
				uint8_t wbcPara;                                //WBC���油��
				uint8_t hgbPara;                                //HGB���油��
				uint8_t pumpPara;                               //��ռ�ձ�ֵ
				uint16_t current535;                            //535�����趨ֵ
				uint16_t current415;                            //415�����趨ֵ
				uint16_t outOC;                                 //���ֹ����ֵ
				uint16_t inOC;                                  //���ֹ����ֵ
				uint16_t gdOC;                                  //�̶������ֵ
				uint16_t sfOC;                                  //�ͷŹ����ֵ
				int16_t presurePara;                            //ѹ������ֵ
				uint16_t wbcAdjust;                             //WBCУ׼ϵ��ֵ
				uint16_t hgbAdjust;                             //HGBУ׼ϵ��ֵ
				uint8_t pressureSensor;							//��ǰʹ�õ�ѹ������������
				uint8_t company;								//��ǰ���������Ĺ�˾
				char yongFuPsw[ACCOUNT_PSW_MAX_LEN];			//Ҫ�޸�YongFu��¼����
				char adminPsw[ACCOUNT_PSW_MAX_LEN];				//Ҫ�޸�admin��¼����
			}u;
		}cmd2_5_1;
		
		//�豸��Ӧ����������������������Ϣ��ָ��2.5.2
		__packed struct{
			uint16_t infoType;                               	//���ڱ�ʾ��ǰҪ�޸ĵ���Ϣ����
			uint8_t result;                          		 	//�豸��Ӧ�Ľ����Ϊ0��ʾ�޸�ʧ�ܣ�Ϊ1��ʾ�޸ĳɹ�
		}cmd2_5_2;
		
		//�豸��Ӧ��������������⹦��������ָ��2.5.4
		__packed struct{
			uint8_t itemFlag;									//���ڱ�ʾҪ�����ĸ������ı�־
			uint8_t resultFlag;									//���ڱ�ʾ�������Ĳ����Ƿ�ɹ��ı�־
		}cmd2_5_4;
		
		//�豸�ϴ���������������ݣ�ָ��2.6.1
		__packed struct{
			uint32_t sampleSN;									//������ţ���ֵ
			uint16_t errCode;                                   //�����룬��ֵ
			uint16_t errHideCode;                               //���ع����룬��ֵ
			uint8_t alarmFlag;									//����ֵ��־
			char orderNum[ORDER_NUM_MAX_LEN];					//��������ַ���
			char patientID[PATIENT_ID_MAX_LEN];					//���߱���ַ���
			char name[30];										//����
			SexType_e gender;                                   //�Ա�
			uint8_t age;                                        //����
			AgeUnit_e ageUnit;                                  //���䵥λ
			AccountType_e account;                              //�˻�
			long timestamp;                                     //����ʱ���
			uint8_t refGroups;									//�ο���
																
			uint16_t wbc;                                       
			uint16_t gran;                                      
			uint16_t mid;                                       
			uint16_t lym;                                       
			uint16_t granPrecent;                               
			uint16_t midPrecent;                                
			uint16_t lymPrecent;                                
																
			uint16_t hgb;                                       
																
			uint16_t firstX;                                    //��һ���ָ���
			uint16_t secondX;                                   //�ڶ����ָ���
			uint16_t thirdX;                                    //�������ָ���
																
			uint16_t maxFL;                                     //������ֵ
																
			uint16_t pictValue[256];                            //256ͨ����ֱ��ͼ����
			
			char wbcErrInfo[170];								//WBC������ʾ��Ϣ
			char hgbErrInfo[110];								//HGB������ʾ��Ϣ
			
			//������־
			WBCHGB_ALERT_e wbcAlarmFlag;                                       
			WBCHGB_ALERT_e granAlarmFlag;                                      
			WBCHGB_ALERT_e midAlarmFlag;                                       
			WBCHGB_ALERT_e lymAlarmFlag;                                       
			WBCHGB_ALERT_e granPrecentAlarmFlag;                               
			WBCHGB_ALERT_e midPrecentAlarmFlag;                                
			WBCHGB_ALERT_e lymPrecentAlarmFlag;                                
			WBCHGB_ALERT_e hgbAlarmFlag;
		}cmd2_6_1;
		
		
		//�豸�ϴ��޸ĵ���������������ݣ�ָ��2.6.2
		__packed struct{
			uint32_t sampleSN;									//������ţ���ֵ
			uint8_t alarmFlag;									//����ֵ��־
			char orderNum[ORDER_NUM_MAX_LEN];					//��������ַ���
			char patientID[PATIENT_ID_MAX_LEN];					//���߱���ַ���
			char name[30];										//����
			SexType_e gender;                                   //�Ա�
			uint8_t age;                                        //����
			AgeUnit_e ageUnit;                                    //���䵥λ
			AccountType_e account;                              //�˻�
			uint8_t refGroups;									//�ο���
																
			uint16_t wbc;                                       
			uint16_t gran;                                      
			uint16_t mid;                                       
			uint16_t lym;                                       
			uint16_t granPrecent;                               
			uint16_t midPrecent;                                
			uint16_t lymPrecent;                                
																
			uint16_t hgb;                                       
			
			//������־
			WBCHGB_ALERT_e wbcAlarmFlag;                                       
			WBCHGB_ALERT_e granAlarmFlag;                                      
			WBCHGB_ALERT_e midAlarmFlag;                                       
			WBCHGB_ALERT_e lymAlarmFlag;                                       
			WBCHGB_ALERT_e granPrecentAlarmFlag;                               
			WBCHGB_ALERT_e midPrecentAlarmFlag;                                
			WBCHGB_ALERT_e lymPrecentAlarmFlag;                                
			WBCHGB_ALERT_e hgbAlarmFlag;
		}cmd2_6_2;
		
		
		//����������־��ָ��2.7.1
		__packed struct{
			uint32_t sampleSN;									//������ţ���ֵ
			uint16_t errCode;                                   //�����룬��ֵ
			uint16_t errHideCode;                               //���ع����룬��ֵ
			uint8_t alarmFlag;									//����ֵ��־
			uint8_t errType;                                    //���Ϸ�����Ϣ
			AccountType_e account;                              //�˻�
			long timestamp;                                     //����ʱ���
			
			char logBuf[];										//��־�ַ���
		}cmd2_7_1;
		
		//������־��ָ��2.7.2
		__packed struct{
			AccountType_e account;                              //�˻�
			long timestamp;                                     //����ʱ���
			
			char logBuf[];										//��־�ַ���
		}cmd2_7_2;
		
		//�豸����������ָ�����߱�ŵĻ�����Ϣ��ָ��2.8.1
		__packed struct{
			char patientID[PATIENT_ID_MAX_LEN];					//���߱���ַ���
			CAT_TestType_e catTestType;							//���Ե���Ŀ����											
		}cmd2_8_1;
		
		//��������豸���������ָ�����߱�ŵĻ�����Ϣ��ָ��2.8.2
		__packed struct{
			char orderNum[ORDER_NUM_MAX_LEN];					//�������
			char patientID[PATIENT_ID_MAX_LEN];					//���߱���ַ���
			CAT_TestType_e catTestType;							//���Ե���Ŀ����
			char name[30];										//����
			uint8_t age;                                        //����
			AgeUnit_e ageUnit;                                  //���䵥λ
			SexType_e gender;                                   //�Ա�
		}cmd2_8_2;
		
		//��������豸���������ָ�����߱�Ų����ڣ�ָ��2.8.3
		__packed struct{
			char patientID[PATIENT_ID_MAX_LEN];					//���߱���ַ���
			CAT_TestType_e catTestType;							//���Ե���Ŀ����
		}cmd2_8_3;
		
		
		
		
		
		
//===������������˽���ͨѶ��ָ�===
		//������ǰ������Ϣ��ָ��A.2.2.1
		__packed struct{
			uint32_t totalOpenCount;							//��ǰ�����ܴ���
			char position[22];									//λ����Ϣ
			uint8_t signalStrength;								//�ź�ǿ��
		}cmdA_2_2_1;
		
		//��������״̬��Ϣ��ָ��A.2.2.4
		__packed struct{
			uint32_t interruptTimestamp;						//�����ж�ʱ���
			uint32_t recoveryTimestamp;							//�����ж�ʱ���
			uint8_t interruptSignalStrength;					//�����ж�ʱ�ź�ǿ��
			uint8_t recoverySignalStrength;						//�����ж�ʱ�ź�ǿ��
		}cmdA_2_2_4;
		
		//�豸�ϴ���������������ݣ�ָ��A.2.3.1
		__packed struct{
			char orderNum[ORDER_NUM_MAX_LEN];					//�������
			CAT_TestType_e catTestType;							//���Ե���Ŀ����
			uint32_t sampleSN;									//������ţ���ֵ
			uint16_t errCode;                                   //�����룬��ֵ
			uint16_t errHideCode;                               //���ع����룬��ֵ
			uint8_t alarmFlag;									//����ֵ��־
			char patientID[PATIENT_ID_MAX_LEN];					//���߱���ַ���
			char name[30];										//����
			SexType_e gender;                                   //�Ա�
			uint8_t age;                                        //����
			AgeUnit_e ageUnit;                                  //���䵥λ
			AccountType_e account;                              //�˻�
			long timestamp;                                     //����ʱ���
			uint8_t refGroups;									//�ο���
																
			uint16_t wbc;                                       
			uint16_t gran;                                      
			uint16_t mid;                                       
			uint16_t lym;                                       
			uint16_t granPrecent;                               
			uint16_t midPrecent;                                
			uint16_t lymPrecent;                                
																
			uint16_t hgb;                                       
																
			uint16_t firstX;                                    //��һ���ָ���
			uint16_t secondX;                                   //�ڶ����ָ���
			uint16_t thirdX;                                    //�������ָ���
																
			uint16_t maxFL;                                     //������ֵ
																
			uint16_t pictValue[256];                            //256ͨ����ֱ��ͼ����
			
			char wbcErrInfo[170];								//WBC������ʾ��Ϣ
			char hgbErrInfo[110];								//HGB������ʾ��Ϣ
			
			//������־
			WBCHGB_ALERT_e wbcAlarmFlag;                                       
			WBCHGB_ALERT_e granAlarmFlag;                                      
			WBCHGB_ALERT_e midAlarmFlag;                                       
			WBCHGB_ALERT_e lymAlarmFlag;                                       
			WBCHGB_ALERT_e granPrecentAlarmFlag;                               
			WBCHGB_ALERT_e midPrecentAlarmFlag;                                
			WBCHGB_ALERT_e lymPrecentAlarmFlag;                                
			WBCHGB_ALERT_e hgbAlarmFlag;
		}cmdA_2_3_1;
		
		//��������������豸������������������յ���Ϣ
		__packed struct{
			CAT_TestType_e catTestType;							//���Ե���Ŀ����
			uint32_t sampleSN;									//�������
		}cmdA_2_3_3;
		
		//��������������־��ָ��A.2.4.1
		__packed struct{
			uint32_t sampleSN;									//������ţ���ֵ
			uint16_t errCode;                                   //�����룬��ֵ
			uint16_t errHideCode;                               //���ع����룬��ֵ
			uint8_t alarmFlag;									//����ֵ��־
			uint8_t errType;                                    //���Ϸ�����Ϣ
			AccountType_e account;                              //�˻�
			long timestamp;                                     //����ʱ���
			CAT_TestType_e catTestType;							//���Ե���Ŀ����
		}cmdA_2_4_1;
		
		//��������������豸���͹�����־���յ���Ϣ
		__packed struct{
			CAT_TestType_e catTestType;							//���Ե���Ŀ����
			uint32_t logSN;										//������־���������
		}cmdA_2_4_2;
		
		//�豸��������������������Ϣ
		__packed struct{
			CAT_TestType_e catTestType;							//���Ե���Ŀ����
			uint8_t pageNum;									//������Ϣҳ�����ӵ�һҳ��ʼ����
		}cmdA_2_5_1;
		
		//��������������豸������������Ϣ�б�
		__packed struct{
			CAT_TestType_e catTestType;							//���Ե���Ŀ����
			uint8_t pageNum;									//ҳ�ţ��ӵ�һҳ��ʼ����
			uint8_t num;										//��ǰҳ�а����Ļ�����Ϣ����
			
			__packed struct{
				char 	name[PATIENT_NAME_LEN];					//����
				uint8_t 	age;								//����
				AgeUnit_e 	eAgeUnit;							//���䵥λ
				SexType_e 	eSex;								//�Ա�
				char 	orderNum[ORDER_NUM_MAX_LEN];			//�������
			}info[5];
		}cmdA_2_5_2;
		
		//�豸�����������˷��ͼ�����ʼ���ԵĻ��߶������
		__packed struct{
			CAT_TestType_e 	catTestType;						//���Ե���Ŀ����
			char 	orderNum[ORDER_NUM_MAX_LEN];				//�������
		}cmdA_2_5_3;
		
		//��������������������������ֵ
		__packed struct{
			uint8_t infoType;                               	//���ڱ�ʾ��ǰҪ�޸ĵ���Ϣ����
			
			//�޸ĵ�����ֵ
			__packed union{
				char adminPsw[ACCOUNT_PSW_MAX_LEN];				//Ҫ�޸�admin��¼����
			}u;
		}cmdA_2_6_2;
		
		//�豸��Ӧ��������������������������ֵ
		__packed struct{
			uint8_t infoType;                               	//���ڱ�ʾ��ǰҪ�޸ĵ���Ϣ����
			uint8_t result;                          		 	//�豸��Ӧ�Ľ����Ϊ0��ʾ�޸�ʧ�ܣ�Ϊ1��ʾ�޸ĳɹ�
		}cmdA_2_6_3;
		
	}payload;
}MQTT_PayloadBuf_s;






uint16_t CRC16(uint8_t* Buf,uint32_t Len);

HAL_StatusTypeDef CAT_RegistNet(void);
HAL_StatusTypeDef CAT_GetSignalStrength(void);
HAL_StatusTypeDef CAT_GetNetTime(RTC_HYM8563Info_s* CurrentNetTime);
HAL_StatusTypeDef CAT_GetPosition(void);
HAL_StatusTypeDef CAT_OpenNet(void);
HAL_StatusTypeDef CAT_IsOK(void);
HAL_StatusTypeDef CAT_ReadType(void);
HAL_StatusTypeDef CAT_InitConfig(void);

HAL_StatusTypeDef CAT_M_ConfigClient(CAT_SERVICE_e CatService,char* ClientID,char* UserName,char* Password,char* WillTopic,char* WillMessage);
HAL_StatusTypeDef CAT_M_CreateTcpConn(char* HostName,uint16_t Port);
HAL_StatusTypeDef CAT_M_CreateMQTTConn(void);
HAL_StatusTypeDef CAT_M_CloseMQTTConn(void);
HAL_StatusTypeDef CAT_M_SubTopic(char* Topic);
HAL_StatusTypeDef CAT_M_PubTopic(char* Topic,uint8_t Qos,uint16_t SendBufLen,uint8_t* SendBuf);

HAL_StatusTypeDef CAT_Send_MachineAllPara(void);
HAL_StatusTypeDef CAT_B_CreateMQTTConn(CAT_SERVICE_e CatService);
HAL_StatusTypeDef CAT_B_CloseMQTTConn(void);
HAL_StatusTypeDef CAT_B_Send_MachineFactoryInfo(void);
HAL_StatusTypeDef CAT_B_Send_MachineOpenInfo(void);
HAL_StatusTypeDef CAT_B_Send_SignalStrength(void);
HAL_StatusTypeDef CAT_B_Send_ChangeMachineInfo(uint8_t TypeInfo,uint8_t* Info,uint8_t Len);
HAL_StatusTypeDef CAT_B_Send_RespondSoftwareInfo(uint8_t PublishOrTestFlag,uint8_t SoftwareType);
HAL_StatusTypeDef CAT_B_Send_RespondUIInfo(uint8_t PublishOrTestFlag,uint16_t UIVersion);
HAL_StatusTypeDef CAT_Send_MachineSelfCheck(uint8_t* Buf,uint8_t Len);
HAL_StatusTypeDef CAT_B_Send_RespondAllSoftwareVersion(uint8_t PublishOrTestFlag);

HAL_StatusTypeDef CAT_B_IsServiceWantToSendData(void);

HAL_StatusTypeDef CAT_B_Send_SampleInfo(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead);
HAL_StatusTypeDef CAT_B_Send_ChangeSampleInfo(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead);
HAL_StatusTypeDef CAT_B_Send_SampleLog(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead,uint8_t* pCountLogBuffer, uint16_t usCountLogLen);
HAL_StatusTypeDef CAT_B_Send_RunLog(RunLog_s* RunLog);
HAL_StatusTypeDef CAT_B_Send_RequestPatientInfo(char* PatientID);

HAL_StatusTypeDef CAT_ConnFTP(void);
HAL_StatusTypeDef CAT_FTPGetFile(FTPDownloadData_s* FTPDownLoadData,char* Path,uint32_t FileOffset);



HAL_StatusTypeDef CAT_B_Other_Send_MachineOpenInfo(void);
HAL_StatusTypeDef CAT_B_Other_Send_SignalStrength(void);
HAL_StatusTypeDef CAT_Other_Send_MachineSelfCheck(uint8_t* Buf,uint8_t Len);
HAL_StatusTypeDef CAT_B_Other_Send_SampleLog(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead);
HAL_StatusTypeDef CAT_B_Other_Send_SampleInfo(WBCHGB_TestInfo_t* WBCHGBTestInfo,CountHead_t* CountHead);
HAL_StatusTypeDef CAT_B_Other_Send_RequestPatientInfo(uint8_t PageNum);
HAL_StatusTypeDef CAT_B_Other_IsServiceWantToSendData(void);
HAL_StatusTypeDef CAT_B_Other_Respond_ChangeSetInfo(uint16_t InfoType,uint8_t Flag,uint16_t* TotalLen,MQTT_PayloadBuf_s* MQTTPayloadBuf);
HAL_StatusTypeDef CAT_B_Other_Send_ReadyTestOrderNum(char* OrderNum);
HAL_StatusTypeDef CAT_B_Other_Send_NetStatus(void);


#endif
