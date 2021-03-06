#include "abox.h"
#include "netif.h"
#include "drv_usbh.h"
#include "net_config.h"
#include "ff.h"



#define LOG_REASON_BUFSIZE 		1024						//原因说明缓冲区
#define LOG_SERVER_BUFSIZE	    1024						//每次发给服务器的数据缓冲区
#define LOG_REC_NUM 			5      						//五条记录

//char log_temp[1024];

typedef enum LOG_TXT_LINE
{
	MOD_ID,
	SOFT_VER,
	CONFIG_LINK_STYLE,
	LOG_NUM,
	MAC_ADDR,
	MODE_STYLE,  
	RECENT_UPDATA_NUM,
	SERVER_NET_INFO,
	LOG_TXT_LINE_NUMBER,
}log_txt_line_name;

typedef enum EACH_TXT_LINE
{
	LOG_NUMBER,
	LOG_TICKS,
	CORRENT_CONNECT_STYLE,
	LINK_STAT,
	ERR_PLACE,

	/* eth */
	DHCP_STAT,
	NETIF_STAT,
	IP_ADDRESS,
	MASK_ADDRESS,
	GATWAY_ADDRESS,

	/* WIFI */
	WIFI_NAME,
	WIFI_PASSWORD,

	ARP_ADDR_TABLE,

	EACH_TXT_LINE_NUMBER,
}each_line_name;


enum LOG_EVENT_SET
{
	/* 处理具体事务 */
	LOG_RECORD_EVENT 	      = (1<<1), 						//记录表格信息
	LOG_DEBUG_EVENT 	      = (1<<2), 						//log日志打印
	LOG_SEND_SERVER_EVENT     = (1<<3),							//把log信息发送给服务器

	/* usb */
	LOG_DEBUG_COMPELET_EVENT  = (1<<4), 						//log日志打印OK

	/* 判断IP情况 */
	LOG_IP_CONFLICT_EVENT     	  = (1<<5),						//ip 冲突
};
	
typedef enum NETWORK_DIAGNOSIS_RESULT
{
	USB_NOT_INSERT,											/* usb未插入 */
	MOD_ID_IS_NULL,											/* 模块ID被清空 */
	MEM_CREAT_ERR,											/* malloc err */											
	USB_FILE_ERR,											/* usb文件打开失败       */
	NETWORK_RUNNING_OK,										/* 网络运行正常 */
}network_result;

/* 动态log   记录 */
struct _log_info 
{            
	unsigned int		mod_id;								//模块编号
	unsigned int		soft_ver;		  					//软件版本
	int 				config_link_stlye;					//允许连接的方式
 															//连接方式 0：无连接 1:eth 2：wifi 3:eth or wifi
	unsigned int 		reclognum;							//记录log的数量,只记录最新的5条
	unsigned char		dev_mac[NETIF_MAX_HWADDR_LEN];		//本机mac
	
	unsigned char		mod_type;							//0：默认连接iot       1:modbus tcp 
															//2: mqtt client  3:ssl/tsl client test
	unsigned char		recent_update_num;	    			// 准备使用第几个数组元素，从0开始 0~4 
															// 例：选用  1,表示之后的数据包里面     
															// 第一包数据是刚刚更新                  
	net_info 			netInfo[SERVER_NUM_MAX];			// 网络连接信息

};                                                                                                                                                              

/* arp 地址信息 */
struct _arp_table_info
{
	unsigned char ip_addr[4];
	unsigned char mac[6];
};

/* 每条数据包含信息 */
struct _each_log
{
	unsigned char		lognum;								//第几条log 从1开始 第几条数据 从1开始 1~5
	rt_tick_t 		    tick;             		   		 	//记录时间
	char		        currentConnectStyle;				//当前连接的方式			NULL = 0,
															//WIFI_CONNECT = 1,ETH_CONNECT = 2,
	int					link_stat;        		  			//连接状态
															//abox： 与服务器连接情况 0: 无连接 1:尝试连接             	   3: eth 连接
	char 				err_palce;
															
	/* eth 连接 */
	char			    DHCP_stat;	
	unsigned short      netif_stat;							//硬件连接 0x00 表示OK 
	/* ip_addr_t ----> unsigned int  */
	ip_addr_t 			ip_addr;
	ip_addr_t			mask_addr;
	ip_addr_t			gatway_addr;		

	/* wifi 连接 */        
	char 				wifiname[WIFI_NAME_MAX];
	char 				wifipassword[WIFI_PASSWORD_MAX];

	/* 依赖arp 表 */   //前4个字节表示ip        后6个字节表示mac
	struct _arp_table_info ip_mac_addr[ARP_TABLE_SIZE];
	
};


/* 表格信息 */
struct _log_sheet 
{
	struct _log_info log_info;
	struct _each_log elog[LOG_REC_NUM];
};

/* 线程中涉及到的变量及函数 */
struct _log_set
{
	rt_event_t log_event;
	FIL  file;
	char *log_reason;										/* log buf */
	char *log_server;										/* log server buf */
	
	struct _log_sheet *log_sheet;							/* 表格信息 */
	
	rt_uint32_t (* log_event_judge)(void);					/* 判断事件 */
	void (* log_record_func)(void);							/* log记录 */
	void (* log_debug_func)(void);							/* log打印输出 USB */
	void (* log_send_server_func)(void);					/* log打印输出到服务器 */
	rt_err_t (* write_file)(char *buf);						/* 往log文件中输入数据 */

}*log_set;



extern struct netif *netif_list;
extern void copy_arp_table(unsigned char *ip,unsigned char *mac,int index);
extern rt_err_t open_log_file(FIL *file_temp);
extern rt_err_t write_log_file(FIL *file_t,char *write_buf,rt_uint32_t wlen);
extern void close_log_file(FIL *file_temp);


/* 整形转化成字符串 */
char *itoa(int num,char *str,int radix) 
{  
	/* 索引表 */ 
	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
	unsigned unum; /* 中间变量 */ 
	int i=0,j,k; 
	/* 确定unum的值 */ 
	if(radix==10&&num<0) /* 十进制负数 */ 
	{ 
		unum=(unsigned)-num; 
		str[i++]='-'; 
	} 
	else unum=(unsigned)num; /* 其它情况 */ 
	/* 逆序 */ 
	do  
	{ 
		str[i++]=index[unum%(unsigned)radix]; 
		unum/=radix; 
	}while(unum); 
	str[i]='\0'; 
	/* 转换 */ 
	if(str[0]=='-') k=1; /* 十进制负数 */ 
	else k=0; 
	/* 将原来的“/2”改为“/2.0”，保证当num在16~255之间，radix等于16时，也能得到正确结果 */ 
	char temp; 
	for(j=k;j<=(i-k-1)/2.0;j++) 
	{ 
		temp=str[j]; 
		str[j]=str[i-j-1]; 
		str[i-j-1]=temp; 
	} 
	return str; 
} 


void send_IP_conflict_result(void)
{
	rt_event_send(log_set->log_event,LOG_IP_CONFLICT_EVENT);
}


/* 进行网络诊断 */
void open_log_diagnosis(void)
{
	rt_event_send(log_set->log_event,LOG_RECORD_EVENT);
}

/* 打印网络诊断内容 */
void print_log_diagnosis(void)
{
	rt_event_send(log_set->log_event,LOG_DEBUG_EVENT);
}


/* 输出网络诊断信息给服务器 */
void send_log_diagnosis(void)
{
	rt_event_send(log_set->log_event,LOG_SEND_SERVER_EVENT);
}
 

/* 等待接收网络诊断完成 */
void wait_print_log(void)
{
	rt_uint32_t recved = 0;
    rt_event_recv(log_set->log_event, 
					 LOG_DEBUG_COMPELET_EVENT ,
					 RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
					 RT_WAITING_FOREVER, 
					 &recved);
}

/* 获取log事件 */
static rt_uint32_t take_log_event(void)
{
	rt_uint32_t recved = 0;
    rt_event_recv(log_set->log_event, 
					 LOG_RECORD_EVENT | LOG_DEBUG_EVENT | LOG_SEND_SERVER_EVENT,  
					 RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
					 RT_WAITING_FOREVER, 
					 &recved);
	return recved;
}

/* 记录log数据 */
static void record_log()
{
	if(!log_set->log_sheet) return;
	int index = 0;

	serverNet_information serNetConfig = returnNetconfig(); 

	/* log info */
	log_set->log_sheet->log_info.mod_id   			= Sys_Setup.app.id;
	getAppVertion((rt_uint32_t *)(&log_set->log_sheet->log_info.soft_ver));
	log_set->log_sheet->log_info.config_link_stlye  = serNetConfig.netConfig.configStyle;
	log_set->log_sheet->log_info.mod_type           = Sys_Setup.app.mode;	

	log_set->log_sheet->log_info.reclognum ++;

	if(log_set->log_sheet->log_info.recent_update_num >= LOG_REC_NUM)
		log_set->log_sheet->log_info.recent_update_num = 0;

	for(index = 0;index < SERVER_NUM_MAX;index ++)
	{
		rt_memcpy(log_set->log_sheet->log_info.netInfo[index].serport,serNetConfig.netInfo[index].serport,8);
		rt_memcpy(log_set->log_sheet->log_info.netInfo[index].seraddr,serNetConfig.netInfo[index].seraddr,16);
	}

	
	struct netif * netif;
    netif = netif_list;
	rt_memcpy(log_set->log_sheet->log_info.dev_mac,netif->hwaddr,netif->hwaddr_len);
	

	/* each log */
	index = log_set->log_sheet->log_info.recent_update_num; 
	struct _each_log *elog_t    = &log_set->log_sheet->elog[index];

	elog_t->lognum 			    = index + 1;
	elog_t->tick 				= rt_tick_get();
	elog_t->currentConnectStyle = Cdma_Info.currentConnectStyle; 	
	elog_t->link_stat			= dicInfo.secondDicInfo.panelLink;
	elog_t->err_palce 		    = Cdma_Info.conErrPlace;

	/* eth log */
	elog_t->DHCP_stat      = serNetConfig.netConfig.ethConfig.status;//DHCP_CLOSE    DHCP_OPEN
	if (!(netif->flags & NETIF_FLAG_UP))
	{
		/* 网口软件层面未启动 ip 未被分配(极大可能是由于硬件连接异常)              */
		elog_t->netif_stat = 1<<8; 
	}
	if (!(netif->flags & NETIF_FLAG_LINK_UP))
	{
		/* 硬件连接断开 */
		elog_t->netif_stat |= 1;
	}
	rt_memcpy(&elog_t->ip_addr,&(netif->ip_addr),sizeof(netif->ip_addr));
	rt_memcpy(&elog_t->mask_addr,&(netif->netmask),sizeof(netif->netmask));
	rt_memcpy(&elog_t->gatway_addr,&(netif->gw),sizeof(netif->gw));

	/* wifi log */
	rt_memcpy(elog_t->wifiname,serNetConfig.netConfig.wifiConfig.wifiName,WIFI_NAME_MAX);
	rt_memcpy(elog_t->wifipassword,serNetConfig.netConfig.wifiConfig.wifiPassword,WIFI_PASSWORD_MAX);	

	/* arp log */
	for(index=0;index<ARP_TABLE_SIZE;index++)
	{
		copy_arp_table(elog_t->ip_mac_addr[index].ip_addr,elog_t->ip_mac_addr[index].mac,index);
	}
	
	log_set->log_sheet->log_info.recent_update_num ++;
}


/*  
	将字符串2添加到字符串1中,返回成功添加的长度
*/
static unsigned int add_string(char *str1,char *str2,rt_uint32_t copy_size)
{
	if(!str1 || !str2) return 0;

	rt_uint32_t str_len;
	if(!copy_size)
		str_len	= strlen(str2);
	else
		str_len = copy_size;

	rt_memcpy(str1,str2,str_len);
	
	return str_len;
}



static rt_uint32_t get_each_txt(int line,rt_uint8_t index)
{
	rt_uint32_t line_len = 0;
	rt_memset(log_set->log_reason,0,LOG_REASON_BUFSIZE);
	char buf[33] = {0}; 

	if(index >= LOG_REC_NUM) return 0;
	
	struct _each_log *elog = &log_set->log_sheet->elog[index];

	switch(line)
	{
		case LOG_NUMBER:
			line_len += add_string(log_set->log_reason,"\n\n/********************/\n",0);
			line_len += add_string(log_set->log_reason+line_len,"log numver: ",0);
		
			itoa(elog->lognum,buf,10);
			line_len += add_string(log_set->log_reason+line_len,buf,0);	
		break;	

		case LOG_TICKS:
			line_len += add_string(log_set->log_reason,"save ticks: ",0);
			
			itoa(elog->tick,buf,10);
			line_len += add_string(log_set->log_reason+line_len,buf,0);	
		break;
			
		case CORRENT_CONNECT_STYLE:
			line_len += add_string(log_set->log_reason,"current connect style: ",0);

			switch(elog->currentConnectStyle)	//当前连接的状态
			{			
				case NULL_DISCONNENCT:
					line_len += add_string(log_set->log_reason+line_len,"disconnect",0);
				break;
				
				case CLIENT_WIFI_CONNECT:
					line_len += add_string(log_set->log_reason+line_len,"wifi connect",0);
				break;

				case CLIENT_ETH_CONNECT:
					line_len += add_string(log_set->log_reason+line_len,"ethernet connect",0);
				break;

				default:
					break;
			}

		break;

		
		case LINK_STAT:
			line_len += add_string(log_set->log_reason,"current link stat: ",0);

			switch(elog->link_stat)	//当前连接的状态
			{			
				case Panel_disconnect:
					line_len += add_string(log_set->log_reason+line_len,"Not connected",0);
				break;
				
				case Panel_tryconnect:
					line_len += add_string(log_set->log_reason+line_len,"Try connected",0);
				break;

				case Panel_con_eth:
					line_len += add_string(log_set->log_reason+line_len,"Connect success",0);
				break;

				default:
					break;
			}

		break;
			
		case ERR_PLACE:
			line_len += add_string(log_set->log_reason,"error place: ",0);

			switch(elog->err_palce)	//当前连接的状态
			{
				case ETH_NETWORK_START_ERROR:
					line_len += add_string(log_set->log_reason+line_len,"ethernet network start",0);
				break;
				
				case ETH_NETWORK_SOCK_ERROR:
					line_len += add_string(log_set->log_reason+line_len,"ethernet network sock error",0);
				break;
				
				case ETH_NETWORK_CONNECT_ERROR:
					line_len += add_string(log_set->log_reason+line_len,"ethernet network connect error",0);
				break;

				case ETH_NETWORK_NO_ERROR:
					line_len += add_string(log_set->log_reason+line_len,"ethernet network is working",0);
				break;
		

				case WIFI_NETWORK_START_ERROR:
					line_len += add_string(log_set->log_reason+line_len,"wifi network start",0);
				break;
					
				case WIFI_NETWORK_AP_CONNECT_ERROR:
					line_len += add_string(log_set->log_reason+line_len,"wifi network ap connect error",0);
				break;
				
				case WIFI_NETWORK_SOCK_ERROR:
					line_len += add_string(log_set->log_reason+line_len,"wifi network sock error",0);
				break;	
	
				case WIFI_NETWORK_CONNECT_ERROR:
					line_len += add_string(log_set->log_reason+line_len,"wifi network connect error",0);
				break;


				default:
					line_len += add_string(log_set->log_reason+line_len,"network connect ok",0);
					break;
			}
			break;
			

		    /* eth */
			case DHCP_STAT:
				if(elog->currentConnectStyle == CLIENT_ETH_CONNECT)
				{
					line_len += add_string(log_set->log_reason,"DHCP stat: ",0);
					if(elog->DHCP_stat	== DHCP_CLOSE)
					{
						line_len += add_string(log_set->log_reason+line_len,"close",0);				
					}
					else if(elog->DHCP_stat	== DHCP_OPEN)
					{
						line_len += add_string(log_set->log_reason+line_len,"open",0);		
					}
				}
			break;
				
			case NETIF_STAT:
				if(elog->currentConnectStyle == CLIENT_ETH_CONNECT)
				{
					line_len += add_string(log_set->log_reason,"netif stat: ",0);
					itoa(elog->netif_stat,buf,10);
					line_len += add_string(log_set->log_reason+line_len,buf,0);	
				}
			break;
				
			case IP_ADDRESS:
				if(elog->currentConnectStyle == CLIENT_ETH_CONNECT)
				{
					line_len += add_string(log_set->log_reason,"ip address: ",0);
					line_len += add_string(log_set->log_reason+line_len,ipaddr_ntoa(&(elog->ip_addr)),16);	
				}
			break;
				
			case MASK_ADDRESS:
				if(elog->currentConnectStyle == CLIENT_ETH_CONNECT)
				{
					line_len += add_string(log_set->log_reason,"mask address: ",0);
					line_len += add_string(log_set->log_reason+line_len,ipaddr_ntoa(&(elog->mask_addr)),16);	
				}
			break;
				
			case GATWAY_ADDRESS:
				if(elog->currentConnectStyle == CLIENT_ETH_CONNECT)
				{
					line_len += add_string(log_set->log_reason,"gatway address: ",0);
					line_len += add_string(log_set->log_reason+line_len,ipaddr_ntoa(&(elog->gatway_addr)),16);	
				}
			break;

			/* WIFI */
			case WIFI_NAME:
				if(elog->currentConnectStyle == CLIENT_WIFI_CONNECT)
				{
					line_len += add_string(log_set->log_reason,"wifi name: ",0);
					line_len += add_string(log_set->log_reason+line_len,elog->wifiname,WIFI_NAME_MAX);
				}
			break;
				
			case WIFI_PASSWORD:
				if(elog->currentConnectStyle == CLIENT_WIFI_CONNECT)
				{
					line_len += add_string(log_set->log_reason,"wifi password: ",0);
					line_len += add_string(log_set->log_reason+line_len,elog->wifiname,WIFI_PASSWORD_MAX);
				}
			break;

			case ARP_ADDR_TABLE:
				if(elog->currentConnectStyle == CLIENT_ETH_CONNECT)
				{
					line_len += add_string(log_set->log_reason,"arp table : \n",0);
					int i = 0;
					int j = 0;
				
					for(;i<ARP_TABLE_SIZE;i++)
					{
						for(j=0;j<6;j++)
						{
							if(elog->ip_mac_addr[i].mac[j] != 0)
								break;
						}

						if((j==6) && (0 == elog->ip_mac_addr[i].mac[5])) //all 0
								continue;
			
						line_len += add_string(log_set->log_reason+line_len,"ip address: ",0);
						
						for(j=0;j<4;j++)
						{
							itoa(elog->ip_mac_addr[i].ip_addr[j],buf,10);
							line_len += add_string(log_set->log_reason+line_len,buf,0);
							if(j != 3)
							log_set->log_reason[line_len] = '.';
							line_len++;
						};

						line_len += add_string(log_set->log_reason+line_len,"     mac: ",0);
						
						for(j=0;j<6;j++)
						{
							itoa(elog->ip_mac_addr[i].mac[j],buf,16);
							if(buf[1] == 0) 
							{
								buf[1] = buf[0];
								buf[0] = '0';
							}
							line_len += add_string(log_set->log_reason+line_len,buf,2);
							line_len += add_string(log_set->log_reason+line_len," ",0);
						}

						log_set->log_reason[line_len] = '\n';
						line_len++;
						
					}
				
					log_set->log_reason[line_len] = '\n';
					line_len++;
				}	
			break;
		
		default:
			break;
	}

	/* new line */
	log_set->log_reason[line_len] = '\n';
	line_len++;

	return line_len;

}


/* 拼凑 txt 每行信息 */
static rt_uint32_t get_log_txt(int line)
{
	rt_uint32_t line_len = 0;
	int i = 0;
	rt_memset(log_set->log_reason,0,LOG_REASON_BUFSIZE);
	char buf[33] = {0}; 
	
	switch(line)
	{
		case MOD_ID:
			line_len += add_string(log_set->log_reason,"mod id: ",0);
		                     			
			itoa(log_set->log_sheet->log_info.mod_id,buf,10);
			line_len += add_string(log_set->log_reason+line_len,buf,0);
			
		break;

		case SOFT_VER:
			line_len += add_string(log_set->log_reason,"soft version: ",0);

			itoa(log_set->log_sheet->log_info.soft_ver,buf,10);
			line_len += add_string(log_set->log_reason+line_len,buf,0);
			 
		break;

		case CONFIG_LINK_STYLE:
			line_len += add_string(log_set->log_reason,"connect link style: ",0);

			switch(log_set->log_sheet->log_info.config_link_stlye)
			{
				case ETH_STYLE:
					line_len += add_string(log_set->log_reason+line_len,"ethernet",0);
				break;

				case WIFI_STYLE:
					line_len += add_string(log_set->log_reason+line_len,"wifi",0);
				break;

				case ETH_WIFI_STYLE:
					line_len += add_string(log_set->log_reason+line_len,"ethernet or wifi",0);
				break;
				
				default:
					break;
			}	
		break;

		case LOG_NUM:
			line_len += add_string(log_set->log_reason,"Number of records: ",0);

			itoa(log_set->log_sheet->log_info.reclognum,buf,10);
			line_len += add_string(log_set->log_reason+line_len,buf,0);
		break; 


		case MAC_ADDR:
			line_len += add_string(log_set->log_reason,"mac: ",0);

			for(i=0;i<NETIF_MAX_HWADDR_LEN;i++)
			{
				itoa(log_set->log_sheet->log_info.dev_mac[i],buf,16);
				if(buf[1] == 0) 
				{
					buf[1] = buf[0];
					buf[0] = '0';
				}
				line_len += add_string(log_set->log_reason+line_len,buf,2);
				line_len += add_string(log_set->log_reason+line_len," ",0);
			}

			
		break;

		case MODE_STYLE:
			line_len += add_string(log_set->log_reason,"mode style: ",0);

			switch(log_set->log_sheet->log_info.mod_type)
			{
				case DEV_DEFAULT:
					line_len += add_string(log_set->log_reason+line_len,"default",0);
				break;

				case DEV_MODBUS_TCP_SERVER:
					line_len += add_string(log_set->log_reason+line_len,"modbus TCP server",0);
				break;
				
				case DEV_MQTT_CLIENT:
					line_len += add_string(log_set->log_reason+line_len,"MQTT client",0);
				break;

				case DEV_SSL_TSL_CLIENT_TEST:
					line_len += add_string(log_set->log_reason+line_len,"ssl/tsl is open",0);
				break;

				default:

					break;
			}
			
		break;


		case RECENT_UPDATA_NUM:
			line_len += add_string(log_set->log_reason,"recent updata number: ",0);

			itoa(log_set->log_sheet->log_info.recent_update_num,buf,10);
			line_len += add_string(log_set->log_reason+line_len,buf,0);
		break;

		case SERVER_NET_INFO:
			line_len += add_string(log_set->log_reason,"server net config: ",0);

			for(i=0;i<SERVER_NUM_MAX;i++)
			{	

				line_len += add_string(log_set->log_reason+line_len,"\nweb[",0);
				itoa(i,buf,10);
				line_len += add_string(log_set->log_reason+line_len,buf,0);
				line_len += add_string(log_set->log_reason+line_len,"]: ",0);
				
				
				line_len += add_string(log_set->log_reason+line_len, 
					log_set->log_sheet->log_info.netInfo[i].seraddr,16);
				
				line_len += add_string(log_set->log_reason+line_len,"  port: ",0);
					
				line_len += add_string(log_set->log_reason+line_len, 
					log_set->log_sheet->log_info.netInfo[i].serport,8);
			}

			
		break;

		
		default:
			break;
	
	}

	/* neew line */
	log_set->log_reason[line_len] = '\n';
	line_len++;
//rt_kprintf("line : %s \r\n",log_set->log_reason);	
	return line_len;
}





/* 往文件中输入数据 */
rt_err_t write_to_log_file(char *w_buf)
{
	if(!w_buf) return RT_ERROR;
	//rt_memset(log_set->log_reason,0,LOG_REASON_BUFSIZE);
	return write_log_file(&log_set->file,w_buf,strlen(w_buf));
}




/* ping ---- arp table 里面的ip */
void check_arp_table(struct _arp_table_info *arp_table)
{
	if(!arp_table) return;

	int suc_rate;
	int i = 0;
	int j = 0;
	rt_uint32_t line_len = 0;
	char buf[4] = {0};

	log_set->write_file("\narp table connection diagnostics: \n");
	for(i=0;i<ARP_TABLE_SIZE;i++)
	{	
		for(j=0;j<6;j++)
		{
			if(arp_table[i].mac[j] != 0)
				break;
		}

		if((j == 6) && (0 == arp_table[i].mac[5])) //all 0
			continue;
 
		rt_memset(buf,0,4);
		rt_memset(log_set->log_reason,0,LOG_REASON_BUFSIZE);
		log_set->write_file("ping ");
		for(j=0;j<4;j++)
		{
			itoa(arp_table[i].ip_addr[j],buf,10);
			line_len += add_string(log_set->log_reason+line_len,buf,0);
			if(j != 3)
			log_set->log_reason[line_len] = '.';
			line_len++;
		}
		log_set->write_file(log_set->log_reason);		
		log_set->write_file("  address result: ");		
 
		rt_memset(buf,0,4);
		ping(log_set->log_reason, 4, 0,&suc_rate);
		itoa(suc_rate,buf,10);
		add_string(log_set->log_reason,buf,0);
		log_set->write_file(buf);
		log_set->write_file("%\n");

	}
	

}

/* ping --- server list */
void check_server()
{
#if 0
	//ping 字符 解析不出来
	serverNet_information serverNet  = return_serverNet_info();
	int suc_rate;
	char buf[4] = {0};

	log_set->write_file("\nserver connection diagnostics: \n");
	int i = 0;
	for(i=0;i<serverNet.serverNetNum;i++)
	{
		log_set->write_file("ping ");
		log_set->write_file(serverNet.netInfo[i].seraddr);
		log_set->write_file(" address result: ");
		ping(serverNet.netInfo[i].seraddr, 4, 0,&suc_rate);
		itoa(suc_rate,buf,10);
		log_set->write_file(buf);
		log_set->write_file("%\n");
	}
#endif//ping
	
	int suc_rate;
	char buf[4] = {0};

	log_set->write_file("\nserver connection diagnostics[iot.esto.cn]: \n");
	log_set->write_file("ping  [119.3.69.167] result: ");
	ping("119.3.69.167", 4, 0,&suc_rate);
	itoa(suc_rate,buf,10);
	log_set->write_file(buf);
	log_set->write_file("%\n");
		
}

/* RT_EOK  IP冲突 */
rt_err_t ip_conflict_detection(ip_addr_t *m_ip)
{
	int suc_rate = 0;
	rt_uint32_t recved = 0;
	char ip_str[16]={0};
	rt_memcpy(ip_str,ipaddr_ntoa(m_ip),16);
	ping(ip_str, 4, 0,&suc_rate);
	
    return rt_event_recv(log_set->log_event, 
						 LOG_IP_CONFLICT_EVENT,  
						 RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
						 1000, 
						 &recved);
}



/* 
	分析原因 
	返回: 错误原因
*/
network_result analysis_reason(void)
{
	int index = 0;
	int j     = 0;
	int max   = 0;
	network_result ret = USB_NOT_INSERT;

	
	/* usb 连接标志 1:usb插入 0:未插入 */
	if(USB_ConnectStatus())
	{
		log_set->log_reason = (char *)rt_calloc(1,LOG_REASON_BUFSIZE); 
		if(!log_set->log_reason) return MEM_CREAT_ERR;
		
	 	if(RT_ERROR == open_log_file(&log_set->file)) return USB_FILE_ERR;

		/* 文件打开 OK:    写数据 */
	    {
	    	/* log txt line number. */
			for(index = 0; index<LOG_TXT_LINE_NUMBER;index++)
			{
				if(RT_ERROR == write_log_file(&log_set->file,log_set->log_reason, get_log_txt(index)))
				{
					rt_kprintf("log txt %d: write log file error! \r\n",index);
				}
			}

			if(max < LOG_REC_NUM)
				max = log_set->log_sheet->log_info.reclognum;
			else
				max = LOG_REC_NUM;

			for(index = 0; index<max;index++)
			{				
				for(j=0;j<EACH_TXT_LINE_NUMBER;j++)
				{
					if(RT_ERROR == write_log_file(&log_set->file,log_set->log_reason, get_each_txt(j,index)))
					{
						rt_kprintf("exch[%d] txt %d: write log file error! \r\n",index,j);
					}
				}
			}
			
		
		}

		if(RT_ERROR == log_set->write_file ("\n/************  ANALYSE  ************/\n"))
		{
			rt_kprintf("analyse log error! \r\n");
		}
		
		
		/* 分析原因 */
		if(!log_set->log_sheet->log_info.mod_id)
		{
			/* mod id 丢失 */
			log_set->write_file("mod id is lost.\n");
		}


		if(!log_set->log_sheet->log_info.recent_update_num)
			index = LOG_REC_NUM -1;
		else
			index = log_set->log_sheet->log_info.recent_update_num -1;		
		
		
		struct _each_log *elog = &log_set->log_sheet->elog[index];

		// log number
		write_log_file(&log_set->file,log_set->log_reason, get_each_txt(LOG_NUMBER,index));

		//if(elog->currentConnectStyle == CLIENT_ETH_CONNECT)
		if(log_set->log_sheet->log_info.config_link_stlye != WIFI_STYLE)
		{
			log_set->write_file("please check ethernet config:\n");
			write_log_file(&log_set->file,log_set->log_reason, get_each_txt(DHCP_STAT,index));
			write_log_file(&log_set->file,log_set->log_reason, get_each_txt(IP_ADDRESS,index));		
			write_log_file(&log_set->file,log_set->log_reason, get_each_txt(MASK_ADDRESS,index));
			write_log_file(&log_set->file,log_set->log_reason, get_each_txt(GATWAY_ADDRESS,index));

			/* 硬件故障 */
			if(elog->netif_stat)
			{
				log_set->write_file("Abox has a hardware connection problem with connect the router.\n");
			}

			/* ip 冲突检测 */
			if(RT_EOK == ip_conflict_detection(&elog->ip_addr))
			{
				log_set->write_file("the error : IP conflict.\n");
			}

			/* 软件故障 */
			write_log_file(&log_set->file,log_set->log_reason, get_each_txt(ARP_ADDR_TABLE,index));
			check_server();
			check_arp_table(elog->ip_mac_addr);
			
							
		}	

		if(log_set->log_sheet->log_info.config_link_stlye != ETH_STYLE)
		{
			log_set->write_file("please check wifi config.\n");
		}
	
			

		close_log_file(&log_set->file);
		ret = NETWORK_RUNNING_OK;
	}
	
		return ret;
}



/* 打印log数据 */
static void debug_log()
{
	/* 刷新log */
	record_log();

	/* 分析原因 输出文本 */
	analysis_reason();

	if(log_set->log_reason)
	{
		rt_free(log_set->log_reason);
		log_set->log_reason = RT_NULL;
	} 
}

/* 发送log数据 */
void send_log_data(char *data,rt_uint16_t len)
{
	if(!data || !len) return;
	char log_buf[LOG_SERVER_BUFSIZE] = {0};
	rt_uint16_t log_len = 0;

	stPackCmd packcmd;
	TCMD_112_Info tcmd_122;
	rt_uint16_t crc = 0;

	/* packcmd */
	packcmd.soh   = CSOH;	 
	packcmd.cmd   = TCMD_112;
	packcmd.id	  = Sys_Setup.app.id; 
	packcmd.len   = sizeof(tcmd_122) + len;

	log_len = sizeof(packcmd);
	rt_memcpy(log_buf,&packcmd,log_len);

	/* tcmd */
	tcmd_122.mark 			= LOG_SHEET_CONFIG;
	tcmd_122.totalPackLen   = 0xffffffff; // 忽略
	tcmd_122.pcaklen        = len;

	rt_memcpy(log_buf+log_len,&tcmd_122,sizeof(tcmd_122));
	log_len += sizeof(tcmd_122);	

	/* data */
	rt_memcpy(log_buf+log_len,data,len);
	log_len += len;

	/* crc */
	crc = ModbusCrc16((rt_uint8_t *)log_buf, log_len);
	rt_memcpy(log_buf+log_len,&crc,2);
	
	/* copy heart data */					
	sendDataToServer(SEND_DATA, (rt_uint8_t *)log_buf,log_len+2);
	
}




/* 数据发送给服务器 */
static void send_log_to_server()
{
	log_set->log_server = rt_malloc(LOG_SERVER_BUFSIZE);
	if(!log_set->log_server) return;

	log_set->log_reason = rt_calloc(1,LOG_REASON_BUFSIZE);
	if(!log_set->log_reason) return;

	/* 刷新log */
	record_log();

	rt_uint16_t log_ser_len = 0;
	rt_uint16_t index = 0;
	int max = 0;
	rt_uint32_t line_len;
	int j  = 0;

	rt_memset(log_set->log_server,0,LOG_SERVER_BUFSIZE);

	/* log txt line number. */
	for(index = 0; index<LOG_TXT_LINE_NUMBER;index++)
	{
		line_len = get_log_txt(index);
		rt_memcpy(log_set->log_server+log_ser_len,log_set->log_reason,line_len);
		log_ser_len += line_len;
	}
	send_log_data(log_set->log_server,log_ser_len);


	if(max < LOG_REC_NUM)
		max = log_set->log_sheet->log_info.reclognum;
	else
		max = LOG_REC_NUM;

	/* send log */
	for(index = 0; index<max;index++)
	{
		rt_memset(log_set->log_server,0,LOG_SERVER_BUFSIZE);
		log_ser_len = 0;
		
		for(j=0;j<EACH_TXT_LINE_NUMBER;j++)
		{
			line_len = get_each_txt(j,index);
			rt_memcpy(log_set->log_server+log_ser_len,log_set->log_reason,line_len);
			log_ser_len += line_len; 
		}
		send_log_data(log_set->log_server,log_ser_len);
		
	}

	rt_free(log_set->log_server);
	rt_free(log_set->log_reason);
}




/* 初始化 log 线程 */
void log_thread_init()
{
  
	log_set = (struct _log_set *)rt_calloc(1,sizeof(struct _log_set));
	if(!log_set) return;

	log_set->log_sheet = (struct _log_sheet *)rt_calloc(1,sizeof(struct _log_sheet));
	if(!log_set->log_sheet) return;

	log_set->log_event = rt_event_create("log_event",RT_IPC_FLAG_FIFO);
	if(!log_set->log_event)
	{
		rt_kprintf("log event is error!\r\n");
	}


	log_set->log_event_judge 		= &take_log_event;
	log_set->log_record_func 		= &record_log;
	log_set->log_debug_func  		= &debug_log;
	log_set->write_file      		= &write_to_log_file;
	log_set->log_send_server_func   = &send_log_to_server;
}


void log_thread(void *parameter)
{
	rt_uint32_t recved = 0;
	log_thread_init();

	while(1)
	{
		recved = log_set->log_event_judge();

		if(recved & LOG_RECORD_EVENT)
		{
			log_set->log_record_func();
			//rt_event_send(log_set->log_event,LOG_RECORD_COMPELET_EVENT);
		}

		if(recved & LOG_DEBUG_EVENT)
		{
			log_set->log_debug_func();
			rt_event_send(log_set->log_event,LOG_DEBUG_COMPELET_EVENT);
		}

		if(recved & LOG_SEND_SERVER_EVENT)
		{
			log_set->log_send_server_func();
		}

	}
}


void Abox_log_init(void)
{

	rt_thread_t tid1 = NULL;
	//TCP check
	tid1 = rt_thread_create("Abox_log",
                           log_thread, NULL,
                          512*4,LOG_EX_PRI, 10);
  
   if (tid1 != RT_NULL) rt_thread_startup(tid1);

}
