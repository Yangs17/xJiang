#ifndef __CTRL_H__
#define __CTRL_H__

#ifdef __cplusplus
extern "C" {
#endif
	
#include	"stdint.h"	
#include    "rtdef.h"	
#include 	"stm32f4xx_gpio.h"
#define ABOX

//#define SOFT_VER									"Ver"	

/*
3.02 Ƶ��5��      �ȶ�  
3.03 Ƶ��30��     �ȶ�
3.04 ����wifi+usb ����   ����modbus TCP
3.05 ����DEV_MQTT_CLIENT + DEV_SSL_TSL_CLIENT_TEST ����OK  ͨ��Sys_Setup.app.mode����ѡ��
	 ����ģ��log ��Ϣ��ӡ��USB��
	 bug: 
	 1.erx_thread �޸�
	 2.tcp_in.c   �޸�
	 3.setpool.c  �޸�
	 4.����log ��Ϣ�Զ��ϴ�

     20/02/26  ����modbusTcpClient.c  
	 
*/
//#define APPF_VERSION             	3             /**< major version number */
//#define APPF_SUBVERSION          	0              /**< minor version number */
//#define APPF_REVISION            	4L            /**< revise version number */	
//#define APP_VER	 (APPF_VERSION *100 + APPF_SUBVERSION *10 + APPF_REVISION)
	
//#define APPH_VERSION             	0L              /**< major version number */
//#define APPH_SUBVERSION          	151015L         /**< minor version number */
//#define APPH_REVISION            	1L             /**< revise version number */	

#define APP_START_ADDR			   0x8008000   //app �ļ���ʼ��ַ
#define APPF_VER_CEILING           400 // ����汾����   ����汾300-400
#define APPF_VER_LOWER 		       300 //����汾����

#define ABOX_LOG_OPEN 								1 //����box log ��ӡ 3.05�汾

#define LEDON										0
#define LEDOFF									 	1

//#define Dev_type 					0x10   //  ���ӵ��豸����

#define LED_GAF									GPIOD
#define LED_ROOF								GPIO_Pin_2  //���� RUN  -----LD104
#define LED_MON									GPIO_Pin_3	//monitor  MON -----LD103
#define LED_CAN								    GPIO_Pin_4	//CAN ETH  TALK	-----LD101
#define LED_BOX									GPIO_Pin_5	 //LINK  ----------LD102

	
#define CDMA_TIME									1
#define GPS_TIME									2
#define NET_TIME									3
#define CPU_TIME									4
#define GSM_TIME									5	//add

#define RT_USING_UART1
#define RT_USING_UART3
//#define RT_FILE_SYS_ERROR  /* �����ļ�ϵͳ��������� */

#define SIMPWR                    GPIO_Pin_0
//#define SIMRST										GPIO_Pin_0	 
#define IN_K											GPIO_Pin_4



#define RT_DELAY_MS               1000 / RT_TICK_PER_SECOND
//#define VPDN
#define SOCKET_CLOSE              (-1)//0xFF   // �������ر�

#define	BUFF_LEN				          1024
#pragma pack(1) 	

struct can_pack_head
{
		uint8_t		sot; // 0xEB
		uint8_t		ver; // 0x01
		
		uint16_t		len; // ??
		uint16_t		cmd; // ??
		uint16_t		id;
};

/*
struct can_pack_head1
{
		uint8_t		sot; // 0xEB
		uint8_t		ver; // 0x02
		uint16_t	len; // ??

		uint32_t 	dev_id;
		uint16_t	cmd; // ??
		uint16_t	id;
};
*/


typedef struct
{
	int 				baud_rate;        	//������     		1200 - 115200
	char 				data_bits;        	//����λ     		8 
	char 				stop_bits;        	//ֹͣλ     		1  
	char 				parity;           	//У��λ     		0: NONE 1: ODD 2: EVEN
	char				addr;             	//��ַ    	 		0 - 255
}uart_conf;


typedef struct setapp
{
	uart_conf 	com1;								//����	
	uart_conf 	com2;								//����	
	uart_conf 	com3;								// ����	
		
	uint32_t		    id;					//�豸��       1-65535
	unsigned char	    mode;				//�豸����      ��ͨ�豸

	int					rectime;            //�洢ʱ���� 30��
	int					recfilelen;         //�ļ���¼����
	int					recmemlen;          //����������	
	int					pagesize;           //��ʾҳ������	
	char				username[32];       //�豸�û���   XX��˾
	char				time;               //ʱ��ͬ��Դ   1: CDMA 2: GPS 3: NET 4: NONE	
}SETAPP;

#define MDN_NUM 16


//���ֽڶ���
typedef struct 
{
// system setup
	char				mdn[MDN_NUM];	

	/* �ݲ���Ҫ */
	char				ipaddr[16];	        //IP��ַ     		192.168.1.3
	char				ipmaskaddr[16];     //MASK       		255.255.255.255
	char				ipgatwayaddr[16];   //GATWAY     		192.168.1.1
	
	char				ipdnsaddr[16];      //DNS        		27.12.23.1
	char				seraddr[16];				//��������ַ 		223.12.11.11
	char				serport[8];					//�������˿� 		8888
	char				seruse[32];					//�û���     		m2m
	char				setpass[16];				//����       		vnet.mobi
 //  char        heartbeat;          // �������	
	uart_conf 	com[6];							//����
	
	//char				timezone;	          //ʱ��
	char				setname[20];        //��¼�ļ���    power.dat	
	
//app setup	
    rt_uint32_t       cdmatype;           //E107  ģ���ͺ�      0: sub-g  1:2G  2:4G
	uint32_t         ver;	//�����汾
	SETAPP			app;
	//char    sreq;

	
	

}stSysSetup;



typedef struct packcmd
{
		char			soh; //��ʼ��  		$
		char			cmd; //101--E106   102--E107							
		int32_t		id;       					//�豸��  		1-65535
		int16_t		len;      					//���峤��
}stPackCmd;

/*  */
typedef struct packestcmd
{
	    unsigned int dev_id;  // ��Ҫָ�������豸��dev_id             102Э��
		int16_t		cmd;       					//------����
	    int16_t   id;
//		int16_t		subind;      				//���峤��
//		int16_t		offset;      				//���峤��
}stPackEstCmd;


//#ifdef RT_CDMA_USING
typedef struct stGprsimsi
{
    unsigned char ccid[32]; //��ǰSIM����CCID�� ��־��
    
    /*//�����д���й�
    unsigned char imsi[16];   
    unsigned char akey[18];
    unsigned char esn[10];
    unsigned char an[32];
    unsigned char aaa[18];
	*/
    unsigned char sn[32];//��ǰ�豸�����к�
}stGprsimsi;

typedef struct stGprsInfo
{
    stGprsimsi    imsi;
    unsigned char name[16];             //��Ʒ��Ϣ
    unsigned char revision[48];    //�����汾 �̼��汾
    unsigned char imei[16]; 
    unsigned char chip[16];
    unsigned char cops[16];
    unsigned char meid[16]; //��ǰSIM����IMSI��	
    unsigned char mdn[16];    //�ֻ�����
    char          rssi;				//�ź�ǿ��
    char          ser;                 
    char          creg;
    char          cregsta;				//����ע�ἰ״̬��ѯ
    char          sim;
    char          cgatt;
    int           basepos;
    int           lac;
    int           cid;
    int           e;
    int           n;
    int           down;
	unsigned int  dev_id;						//ģ���dev_id    ʶ�����ĵ�Ψһ��־
	unsigned char  hver;   		//Ӳ���汾
    short int     link;
	rt_device_t   AT_device;
	rt_uint8_t    ver;     //����Э��汾---- ����ox161���� ����---��0x20����
	char 		  AT_status;
	char 		  currentConnectStyle;//��ǰ���ӵķ�ʽ       	NULL = 0,WIFI_CONNECT = 1,ETH_CONNECT = 2,
	char 		  freq_num; //��ǰƵ��
	char		  serNetNum;//��ǰ��������Ŀ��
	rt_tick_t     serHeartTime;//�������������ж�����
	
	unsigned char conErrPlace; //�������Ӵ���ĵط�
	
}stCdmaInfo;

//#endif
extern stCdmaInfo        Cdma_Info;
extern	stSysSetup		 Sys_Setup;

void getAppVertion(rt_uint32_t *ver);
void  DefaultSetup(stSysSetup *setup);
void	ReadSetup(stSysSetup *setup);
void	SaveSetup(stSysSetup *setup);
void  OutputLed(char onoff, int led);
rt_uint32_t USB_ConnectStatus(void);


void at_printf(const char *format, ...);
void at_print_close(void);
void at_print_open(void);



#ifdef __cplusplus
}
#endif
#endif