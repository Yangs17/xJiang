#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include "ctrl.h"
#include <string.h>
#include "rtdef.h"

#define U32 unsigned int
#define U16 unsigned short
#define	U8	unsigned char

/* connection status with panel */
enum PANEL_LINK_STATUS{
	Panel_disconnect,
	Panel_tryconnect,	
	//Panel_con_can,
	Panel_con_eth,
	//Panel_con_aux
};


enum MOD_TYPE{	
	//	D701+U702=E106(2g)
	//	D701+U703=E107(4g)
	//	D701+U701=E109（sub-g）

	E106,   //M26
	E107,   //EC20
	E108,   //蓝牙
	E109,   //subgCbox
	E110,   //subgAbox
} ;	


enum DICTIONARY_COLLECTION
{
	DIC_FIRST_ADDR  = 0x1,
	DIC_SECOND_ADDR = 0x2,
	DIC_THIRD_ADDR  = 0x3,
	DIC_FOURTH_ADDR = 0x4,
	DIC_FIFTH_ADDR  = 0x5,
};



/* The collection of subcommands below the command. */
enum TCMD_121_SCMD{
	TCMD_121_SCMD_LOOK_INFO   = 0X01,
		
	TCMD_121_SCMD_REBOOT      = 0X02,
	TCMD_121_SCMD_TELNET      = 0X03,
	TCMD_121_SCMD_SERVER_INFO = 0X04,
	TCMD_121_SCMD_SERVER_POLL = 0X05,
};



//第一类数据字典         默认的静态数据
#define FIRSTDIC_END_ADDR 0x10004
typedef struct _first_dictionary_info
{
	  U32   softVer; 				//0x10000 软件程序版本
	  U32	hwVer;					//0x10001 硬件版本号--预留
	  U32	modID;					//0x10002 模块编号	  	  
	  U16	modType; 				//0x10003 模块设备类型	 
	  								//enum MOD_TYPE
	  								
	  U16	proModel; 				//0x10004 准讯协议信息
	  								// 0 :101 
	  								// 1 :102
}first_dictionary_info;

//第二类数据字典         动态数据
#define SECONDDIC_END_ADDR 0x20004
typedef struct _second_dictionary_info
{
	U32 	panelLink;  		//0x20000 
								//cbox：  与面板的连接方式	0: 无连接	1:尝试连接 (eth) 		       2: can 连接 3: eth 连接 4:485连接	
								//abox： 与服务器连接情况 0: 无连接 1:尝试连接 (eth)            	   3: eth 连接
								
	U32 	panelRevnum;		//0x20001 已经接收到的eth、485、can 数据包数量
	U32 	panelSendnum;		//0x20002 已经发送到的eth、485、can 数据包数量	

	U32	 	serConnum;			//0x20003 重连上一级的次数
	U32		worktime; 			//0x20004 已开机时间按秒计时 49710 天复位一次 136年复位一次
  
}second_dictionary_info;



//第三类数据字典 重启设置或者重要的数据信息
#define THIRDDIC_END_ADDR 0x30003
typedef struct  _third_dictionary_info{

   U32	resetStatus;		//0x30000 复位设置 2表示延时复位 1 表示立即复位，0表示不复位
   U32  resetTime;			//0x30001 定时复位时间设置 单位s

   U8  imePassword[8];		//0x30002 立即复位密码
   U8  delPassword[8];		//0x30003 延时复位密码
	
}third_dictionary_info;


//第四类数据字典 AT模块信息
#define FOURTHDIC_END_ADDR 0x40005
typedef struct  _fourth_dictionary_info{

   U8	   csq;			       //0x40000 实时的信号强度
   U16	   modSignalMAX; 	   //0x40001 最近一小时，跟服务器连接信号最高值
   U16	   modSignalMIN; 	   //0x40002 最近一小时，跟服务器连接信号最低值
   U16	   modSignalAVE; 	   //0x40003 最近一小时，跟服务器连接信号平均值
   
   U8 	   mdn[16];			   //0x40004 
   U8      ccid[32];           //0x40005
}fourth_dictionary_info;

//第五类数据字典  subg 信息
#define FIFTHDIC_END_ADDR 0x50001
typedef struct  _fifth_dictionary_info{

   U8	   freq_num;			//0x50000 当前频段 
   U8	   cboxRssi;			//0x50001 对应的Cbox的rssi 
   
}fifth_dictionary_info;


typedef struct _cycle_info
{
	U32 startAddr; //前两字节表示 字典类型 后两字节表示字典内容
	U16 len;
}cycle_info;


//下发的121 命令中的scmd 1 命令：查询数据字典信息
typedef struct TCMD_121_1_INFO{
	U16 count; 				// 循环个数
	cycle_info cycleInfo;
}TCMD_121_1_Info;



//上传的122 命令中的scmd 1 命令：查询数据字典信息
typedef struct TCMD_122_1_INFO{
	U16 count; 
	cycle_info cycleInfo;
	U8 *data;
}TCMD_122_1_Info;



typedef struct  _ictionary_info{
	first_dictionary_info  firstDicInfo;
	second_dictionary_info secondDicInfo;
   	third_dictionary_info  thirdDicInfo;
	fourth_dictionary_info fourthDicInfo;
	fifth_dictionary_info  fifthDicInfo;
}dictionary_info;

extern dictionary_info dicInfo;

void initDictionary(void);
void getModsignal(void);
void recv_dic_cmd(char *rev, char *send);


void getDicFirstInfo(rt_uint32_t addr,char *send,rt_uint16_t *sendLen_t);
void getDicSecondInfo(rt_uint32_t addr,char *send,rt_uint16_t *sendLen_t);
void getDicThirdInfo(rt_uint32_t addr, char * send, rt_uint16_t * sendLen_t);
void getDicFourthInfo(rt_uint32_t addr,char *send,rt_uint16_t *sendLen_t);
void getDicFifthInfo(rt_uint32_t addr,char *send,rt_uint16_t *sendLen_t);



#endif






