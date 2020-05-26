
#ifndef __SUB_G_H__
#define __SUB_G_H__

#include "mydef.h"
#include "si4463.h"



/*********
实际测试: 4FSK，500kbps波特率，10000次，64字节，有效字节58字节 32676ms
每次来回一包时间3.2676ms
50个盒子每一个都超时发送（共发三次），需要时间500ms左右
*********/
#define CBOX_MAX_NUM 50  
#define CBOX_MAX_DATA_LIST 10   						  //每个C最多存10条数据
#define RT_TICK_PER_MS (RT_TICK_PER_SECOND/1000) 		  //定义毫秒
#define ONCE_PACK_TIME (6 * RT_TICK_PER_MS)      		  //一包来回4.2ms左右 
#define ONCE_PACK_TIME_OUT (ONCE_PACK_TIME *2) 		      //一包超时时间

#define ONCE_PACK_UPDATA_TIME_OUT (100) 				  //更新程序的时候设置的超时时间 文件写入需要时间

#define POLL_FREQ (20 * CBOX_MAX_NUM) 					  //单次轮询期间所允许的最大发送数据次数，超过此次数
										 				  //就会遍历剩余未被轮询到的设备，并忽略数据传输
														  //每包有效字节为54个字节，1024字节需要19包左右，这里给了每个设备20包的数据

#define POLL_TIME_OUT (POLL_FREQ * ONCE_PACK_TIME) //遍历一次超时时间 
#define SUBG_RECV_TIME_OUT (100 + POLL_TIME_OUT)   //判断环境中的信号时间




/* Timeout determination of the sending function(tx_data). */
#define TX_TIME_OUT (10 * RT_TICK_PER_MS)


/* The command set */
enum SUB_G_CMD{

	TESTCMD1 = 1,  //Abox主动下发测试包
	TESTCMD2,	   //Cbox发送此包，询问Abox能否允许Cbox注册
	TESTCMD3,      //Abox 接收到TCMD2下发此包,发送允许的注册信息
	
	
	CHECK_CMD1,    //Abox主动下发询问，并带发送的数据
	CHECK_CMD2,	   //Cbox接收到CK1  返回的包并带数据
	CHECK_CMD3,    //Cbox接收到CK1  返回的包并带设备删改信息
  

};

/* Cbox  status*/
enum SUBG_FILTER_SETTING{
	SUBG_FILTER_CLOSE = 0x00,
	SUBG_FILTER_OPEN = 0xFF,	
};



/* subg box freq */
#define RF_FREQ_CONTROL_INTE_8_NUM 30
enum SUBG_BOX_FREQ
{
	SUBG_BOX_FREQ_0,
	SUBG_BOX_FREQ_1,
	SUBG_BOX_FREQ_2,
	SUBG_BOX_FREQ_3,
	SUBG_BOX_FREQ_4,
	SUBG_BOX_FREQ_5,
	SUBG_BOX_FREQ_6,
	SUBG_BOX_FREQ_7,
	SUBG_BOX_FREQ_8,
	SUBG_BOX_FREQ_9,

	SUBG_BOX_FREQ_10,
	SUBG_BOX_FREQ_11,
	SUBG_BOX_FREQ_12,
	SUBG_BOX_FREQ_13,
	SUBG_BOX_FREQ_14,
	SUBG_BOX_FREQ_15,
	SUBG_BOX_FREQ_16,
	SUBG_BOX_FREQ_17,
	SUBG_BOX_FREQ_18,
	SUBG_BOX_FREQ_19,

	SUBG_BOX_FREQ_20,
	SUBG_BOX_FREQ_21,
	SUBG_BOX_FREQ_22,
	SUBG_BOX_FREQ_23,
	SUBG_BOX_FREQ_24,
	SUBG_BOX_FREQ_25,
	SUBG_BOX_FREQ_26,
	SUBG_BOX_FREQ_27,
	SUBG_BOX_FREQ_28,
	SUBG_BOX_FREQ_29,
};


enum PACKAGE_SEND_DETERMINE
{
	SEND_END,
	SEND_KEEP,
};

enum PACKAGE_SOH_DETERMINE
{
	CBOX_SEND_ABOX,
	ABOX_SEND_CBOX,
};


/*****************
测试包：协助测试 Abox每次轮询会连续发两包
*****************/
#define	DEVID_TYPE_NUM 6

//A→C：A向C发送注册包 判断是否有需要新注册的Cbox 
typedef struct _test_package1
{	
	rt_uint32_t  cboxDevid;  //0x 00 00 00 00
	rt_uint8_t   soh:   1;  // 1A→C 
	rt_uint8_t   reg:   1;  //SEND_END
	rt_uint8_t   cmd:   6;   //TESTCMD1 
	rt_uint32_t  Abox_dev_id; //Abox dev_id    
}test_package1;

//C→A：C向A发送注册包 
typedef struct _test_package2
{	
	rt_uint32_t  aboxDevid;  //
	rt_uint8_t   soh:   1;  // 0C->A 
	rt_uint8_t   reg:   1;  //SEND_END
	rt_uint8_t   cmd:   6;   //TESTCMD2 
  	rt_uint32_t  p_dev_id[DEVID_TYPE_NUM]; //从Cbox,面板,辅机1~4 按顺序放入，为0 表示该设备不存在  
}test_package2;

//A→C：A向C发送注册包 判断是否有需要新注册的Cbox 
typedef struct _test_package3
{	
	rt_uint32_t  cboxDevid;  //0x 00 00 00 00
	rt_uint8_t   soh:   1;  // 1A→C 
	rt_uint8_t   reg:   1;  //SEND_END
	rt_uint8_t   cmd:   6;   //TESTCMD2 
	rt_uint8_t   cboxNum;   //已注册的Cbox数量
	rt_uint32_t  Cbox_dev_id;    //已经成功注册上的Cbox  dev_id
}test_package3;






/************
查询包：模块空闲时，会轮询注册表，此时Abox会主动下发查询包，
询问Cbox是否有数据需要发送。或者Abox主动下发数据，
当Abox 与Cbox同时有数据需要发送时，优先处理A->C的数据。
************/

//A→C：询问Cbox数据或Abox主动下发数据
typedef struct CHECK_PACKAGE1
{
	u32_t cboxDevid;  //Cbox dev_id
	u8_t  soh:   1 ;  // 1 A→C 
	u8_t  reg:   1 ;  //1 表示这包之后还有数据需要被发送                   													
					  //0 表示这包之后没有数据需要发送
	u8_t  cmd :  6;   //CHECK_CMD1
	u8_t  aboxPackNum; //Abox发给Cbox的包编号,累加
	
	u8_t  lenth;  // 数据长度 ---- 长度为0 则后面数据无效

	
	//u8_t  data[lenth];// 命令内容
}check_package1;
	       
//C→A：C向A发送查询包的回执 ack
typedef struct CHECK_PACKAGE2
{
	u32_t aboxDevid;  //Abox dev_id
	u8_t  soh:   1 ;  // 0 C→A 
	u8_t  reg:   1 ; //1 表示这包之后还有数据需要被发送                   													
					 //0 表示这包之后没有数据需要发送
	u8_t  cmd :  6;  //CHECK_CMD1  
	u8_t  cboxPackNum;//Cbox发给Abox的包编号,累加
	
	u8_t  lenth;  // 数据长度 ---- 长度为0 则后面数据无效
	//u8_t  data[lenth]; //命令内容

}check_package2;

//C→A：C向A发送查询包的回执 ack
typedef struct CHECK_PACKAGE3
{
	u32_t aboxDevid;  //Abox dev_id
	u8_t  soh:   1 ;  // 0 C→A 
	u8_t  reg:   1 ;  // 1 表示Cbox有数据需要被发送，0表示无数据发送
	u8_t  cmd :  6;   //CHECK_CMD2

	//若 reg 为1 则以下数据有效    表示当前Cbox 下设备有变动
	u32_t  p_dev_id[DEVID_TYPE_NUM]; //从Cbox,面板,辅机1~4 按顺序放入，为0 表示该设备不存在

}check_package3;



/*
紧急包：reg为1        表示Cbox会发送紧急信息给Abox,比如掉电信息，
				 这类包通常由Cbox主动发起
*/
//C→A：C向A发送查询包的回执 ack
typedef struct CHECK_PACKAGE4
{
	u32_t  dev_id;	//Abox dev_id
	u8_t  soh:	 1 ;  // 0 C->A 
	u8_t  reg:	 1 ;  //默认1    发送紧急信息
	u8_t  cmd :  6;   //CHECK_CMD3
	u8_t  lenth;  // 数据长度      
	//u8_t  data[lenth];// 命令内容

}check_package4;




/************
心跳包：    1.在单次轮询中，收发的包到达设定的极限，则会退出当前包的传输
		Abox会遍历之后的已注册的设备，防止设备掉线 ---
		
		2.当Abox设备未被服务器或与服务器未建立连接时，会主动下发此包
				
************/
typedef struct HREAT_PACKAGE1
{
	u32_t  dev_id;	//Cbox dev_id      Abox dev_id
	u8_t  soh:	 1 ;  // 1 A→C 			0 C->A
	u8_t  reg:	 1 ;  //默认0   
	u8_t  cmd :  6;   //7

	u8_t  slink; //Abox与服务器连接状态
				// Cmda_info.link 	DISCONNENCT,		 //0：未连接
  				//					CONNECT_AT,			 //1: 未注册
				//					CONNECT_SERVER,		 //2：已与服务器建立连接

}heart_package1;

//当接收到的slink 为0~1 ，则处于情况2		  
typedef struct HREAT_PACKAGE2
{
	u32_t  dev_id;	//Cbox dev_id      Abox dev_id
	u8_t  soh:	 1 ;  // 1 A→C 			0 C->A
	u8_t  reg:	 1 ;  //默认0   
	u8_t  cmd :  6;   //8

	u32_t  p_dev_id[DEVID_TYPE_NUM]; //从Cbox,面板,辅机1~4 按顺序放入，为0 表示该设备不存在

}heart_package2;



#define FILETER_LENGTH 4
typedef struct _filter_information
{
	unsigned char filterMark;
	unsigned char filterBuf[FILETER_LENGTH]; 
}filter_information;



//mq infomation
#define CBOX_MQ_NUM 4

/* MQ type */
//Cbox mq_type
enum CBOX_MQ_TYPE{
	C_PTOC_MQ = 0,// The panel or auxiliary equipment is sent to the Cbox.
	C_CTOP_MQ = 1,

	C_CTOA_MQ = 2,//The Cbox is sent to the buffer of Abox or Server
	C_ATOC_MQ = 3,//The Abox or server is sent to the buffer of Cbox
};


//mq infomation
#define ABOX_MQ_NUM 1

/* MQ type */
//Abox mq_type
enum ABOX_MQ_TYPE{
	A_STOA_MQ, // server - > Abox 
};





extern rt_sem_t    sub_lock;

unsigned char  getRssi(void);  
void checkSubgInfomation(void);

void subg_Abox_init(void);
void subg_Cbox_init(void);
void initSubgHardware(void);
err_t recvFreqData(void);
void EXTI15_10_IRQHandler(void);
//err_t subg_cdma_rev_pack(rt_uint8_t *precv);
void sub_PackRegCmd(rt_uint8_t *buff);
void SI4463_init(filter_information *filterInfo_t);
void checkDataFromSubg(char *checkBuff,rt_uint32_t *checkLength);
void changeSubgFillter(filter_information *filterInfo_t);



#endif

