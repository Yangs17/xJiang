/*
#include <rtthread.h>
#include <netdb.h> 
#include <sockets.h> 
#include "netif.h"
#include "mydef.h"
#include "dictionary.h"
#include "ctrl.h"
#include "stm32f4xx_gpio.h"
#include "sub_G.h"
#include "at.h"
#include "abox.h"
#include "adc.h"
#include "net_config.h"
*/
#include <netdb.h> 
#include <sockets.h> 
#include "netif.h"
#include "stm32f4xx_gpio.h"
#include "at.h"
#include "abox.h"
#include "adc.h"
#include "modbusTcp.h"



#define BREAK_TIME (RT_TICK_PER_SECOND*15) //保证Cbox 重发0x160

//  结构更改-----------------线程方式更改

rt_uint8_t        Eth_Rev_Buff[BUFF_LEN*2];
rt_uint16_t       Eth_recv_len;


rt_event_t cliEvent;// creat and delete
static serverNet_information serNetConfig;
rt_mq_t climqSend;
e_currentConnectStyle returnClientCurrentLinkStyle(void);


#ifdef RT_CLIENT_USING


void listConnectInfo()
{
	rt_kprintf("\r\n/********* connect information *************/\r\n");
	rt_kprintf("connenct now status: ");

	switch(returnClientCurrentLinkStyle())
	{
		case CLIENT_WIFI_CONNECT:
			rt_kprintf(" wifi connect. ");
		break;

		case CLIENT_ETH_CONNECT:
			rt_kprintf(" eth connect. ");
		break;

		default:
			rt_kprintf(" null connect. ");
		break;
	}

	rt_kprintf("\r\nweb config \r\n");
	rt_kprintf("save web site number: %d \r\n",serNetConfig.serverNetNum);
	int i = 0;
	for(;i<SERVER_NUM_MAX;i++)
	rt_kprintf("web[%d]: %s %s \r\n",i+1,serNetConfig.netInfo[i].seraddr,serNetConfig.netInfo[i].serport);
	
	rt_kprintf("net config style: ");
	switch(serNetConfig.netConfig.configStyle)
	{
		case ETH_STYLE:
			rt_kprintf("eth");
		break;

		case WIFI_STYLE:
			rt_kprintf("wifi");
		break;

		case ETH_WIFI_STYLE:
			rt_kprintf("eth or wifi");
		break;

		default:
			rt_kprintf("null");
		break;
	}
	rt_kprintf("\r\n");
	rt_kprintf("net eth config: \r\n");  
	rt_kprintf("DHCP config: ");
	switch(serNetConfig.netConfig.ethConfig.status)
	{
		case DHCP_CLOSE:
			rt_kprintf(" close. \r\n");
		break;

		case DHCP_OPEN:
			rt_kprintf(" open. \r\n");
		break;

	}
	rt_kprintf("eth ip config: %s \r\n",serNetConfig.netConfig.ethConfig.netAddrConfig.ipaddr);
	rt_kprintf("eth mask config: %s \r\n",serNetConfig.netConfig.ethConfig.netAddrConfig.ipmaskaddr);
	rt_kprintf("eth gatway config: %s \r\n",serNetConfig.netConfig.ethConfig.netAddrConfig.ipgatwayaddr);
	
	rt_kprintf("net wifi config: \r\n");  
	rt_kprintf("wifi name : %s \r\n",serNetConfig.netConfig.wifiConfig.wifiName);
	rt_kprintf("wifi password: %s \r\n",serNetConfig.netConfig.wifiConfig.wifiPassword);

	rt_kprintf("\r\n/********* connect end *************/\r\n");
}


serverNet_information return_serverNet_info(void)
{
	return serNetConfig;
}





void quitTcpConnect(void)
{

	e_currentConnectStyle conStyle = returnClientCurrentLinkStyle();
	Cdma_Info.link = DISCONNENCT;
	Cdma_Info.currentConnectStyle = NULL_DISCONNENCT;
	dicInfo.secondDicInfo.panelLink = Panel_disconnect; 
	switch(conStyle)
	{
		case CLIENT_WIFI_CONNECT:
			quitCC3100Tcp();			
		break;

		case CLIENT_ETH_CONNECT:
			quitEthernetTcp();
		break;
		

		default:
			break;
	}
	cleanCbox();

}






/********************** Public interface **********************/


/*
return: RT_EOK    连接成功  
		RT_ERROR  未连接上
*/
rt_err_t returnServerLinkStatus(void)
{
	if(dicInfo.secondDicInfo.panelLink == Panel_con_eth)
		return RT_EOK;
	else
		return RT_ERROR;
}


serverNet_information returnNetconfig(void)  
{
	return serNetConfig;
}

/* 返回当前的连接方法 wifi or eth */
e_currentConnectStyle returnClientCurrentLinkStyle(void)
{
	return (e_currentConnectStyle)Cdma_Info.currentConnectStyle;
}

/* 返回配置连接方式 */
e_connectMothod returnConfigLinkMothod(void)
{
	return (e_connectMothod)serNetConfig.netConfig.configStyle;
}

/* 数据输入的连接方式  来返回对应的配置 */
e_clientConnectStatus returnConnectConfig(e_currentConnectStyle connectStyle)
{
	e_connectMothod  cMothod = returnConfigLinkMothod();
	e_clientConnectStatus cStatus;
	
	switch(connectStyle)
	{
		case CLIENT_WIFI_CONNECT: //查询wifi 连接	
				cStatus = DHCP_OPEN;		
			break;

		case CLIENT_ETH_CONNECT: //查询eth 连接
				cStatus = (e_clientConnectStatus)serNetConfig.netConfig.ethConfig.status;		
			break;

		default:
			break;

	}

	return cStatus;
}

/********************** end **********************/







/**
 * This function will process data from server.
 *
 * @param void
 *
 * @return void
 *
 */
void processDataFromServer(void)
{
	struct can_pack_head  *head;
	stPackCmd 	 *packcmd= (stPackCmd  *)Eth_Rev_Buff;	
	stPackEstCmd *estpack = (stPackEstCmd *)(Eth_Rev_Buff + sizeof(stPackCmd));
	rt_uint16_t tlen = 0;
	rt_err_t 	ret = RT_EOK;
	rt_uint16_t save_err = 0;
	dicInfo.secondDicInfo.panelRevnum++;
	Cdma_Info.serHeartTime = rt_tick_get();
	
	/* The subcontract processing */
	while(1)
	{
		while(Eth_recv_len>0)
		{
			//at_printf("buf[0] = %d\r\n",Eth_Rev_Buff[0]);
			if(Eth_Rev_Buff[0] != CSOH && Eth_Rev_Buff[0] != SSOH)
			{
				data_left((rt_uint8_t *)Eth_Rev_Buff,1,sizeof(Eth_Rev_Buff));
				Eth_recv_len --;
				continue;
			}
			break;
		}
	
		/* get the length of buff */
		tlen = packcmd->len + sizeof(stPackCmd) + 2;
		if(Eth_recv_len < tlen) break;

		
		/* check data */
		if(ModbusCrc16((rt_uint8_t *)Eth_Rev_Buff, tlen) == 0)
		{
			//check ok
			//rt_memcpy(Eth_Rev_Buff,Eth_Rev_Buff,tlen);
			at_printf("recv eth: packcmd = 0x%x ,estcmd = 0x%x \r\n",packcmd->cmd,estpack->cmd);


			/* judge mod_id */
			if(packcmd->id == Sys_Setup.app.id)
			{
				mq_send(A_STOA_MQ, (char *)Eth_Rev_Buff, tlen,NORMAL_EVENT);
			}
			else
			{
				
				if((Cdma_Info.AT_status != AT_SER_UPDATA) && (Cdma_Info.AT_status != AT_YMODEM_UPDATA))
				{

					while(1)
					{
						rt_enter_critical();
						ret = recvServerData(Eth_Rev_Buff);
						rt_exit_critical();

						if(ret == RT_EOK) break;

						/* total ： 1000 *10 = 10s */
						if(save_err ++ > 1000) 
						{
							cleanPool(aboxInfo.cboxDataPool);	
							break;

						}	

						rt_thread_delay(10);

					}
		

				}
			}



			//clean data
			data_left((rt_uint8_t *)Eth_Rev_Buff, tlen,sizeof(Eth_Rev_Buff));
			Eth_recv_len -= tlen;


			/* Receiving multiple sets of data at a time.  */
			if(Eth_recv_len >= sizeof(stPackCmd)+2)
				continue;
			else
				break;
				
			
		}
		else
		{
			at_printf("eth data1 is err !\n",head->cmd);
			break;
		}	
		

	}




	//if it receives incompleted data to prevent data overflow.
	if(Eth_recv_len > BUFF_LEN)
	{
		rt_memset(Eth_Rev_Buff,0,sizeof(Eth_Rev_Buff));
		Eth_recv_len = 0;
	}


}




/* 数据发送判断       */
void sendNormalData(rt_uint8_t *sendBuf,rt_uint32_t len)
{
	if(!sendBuf || !len) return ;
	if(len > BUFF_LEN)   return ;

	rt_mq_send(climqSend, sendBuf, len);

}

int sendADCInfo(void)
{
	 return packADCInfo();
}


/*
判断发送状态是否准备OK    
返回: RT_EOK 允许发送  RT_ERROR 不允许发送
*/
rt_err_t judgeSendStatus()
{
	e_currentConnectStyle conStyle = returnClientCurrentLinkStyle();
	switch(conStyle)
	{
		case CLIENT_WIFI_CONNECT:
			return judgeWifiSendStatus();
	

		case CLIENT_ETH_CONNECT:
			return	judgeEthSendStatus();
		

		default:
			break;
	}

	return RT_ERROR;
}



/**
 * This function TCP sends data to server.
 *
 * @param void
 *
 * @return RT_ERROR: send error.
 *
 */
void sendDataToServer(char sendType,rt_uint8_t *sendBuf,rt_uint32_t len)
{
	if(RT_ERROR == judgeSendStatus())
		return;


	OutputLed(LEDON, LED_CAN);			
	switch(sendType)
	{
		case SEND_CLOSE:
			quitTcpConnect();
		break;
		
		case SEND_DATA:
			sendNormalData(sendBuf,len);
		break;
			
		case SEND_POWER_DOWN:
			sendADCInfo();
		break;

		default:
			break;
	
	}
	  


	if(!(dicInfo.secondDicInfo.panelSendnum % 2))
	{
		OutputLed(LEDOFF, LED_CAN);
	}

	

}




/**
 * Query character
 *
 * @param sbuf The string being queried
 *
 * @param bsize buff size
 *
 * @param ch Query character
 *
 * @param save Save buff
 *
 * @param slen The len of save buff
 *
 * @return 
 *
 */
void get_ch(char *sbuf,int bsize,char ch,char *save,char slen)
{
	int i = 0;
	int j = 0;
	
	for(;i<bsize;i++)
	{
		if(sbuf[i] == ch)
		{
			if(slen>j)
			{
				save[j] = i;
				j++;
			}
		}
	}

	
}




void set_ipaddr(char *ipaddr,int *waddr)
{
	if(!ipaddr || !waddr) return;

	int n = 0;
	char i = 0;
	char pch[3] = {0};

	get_ch(ipaddr,16,'.',pch,3);
	for(;i<4;i++)
	{
		if(0 == i)
		{
			n = atoi(ipaddr);
			*waddr = n;
		}
		else
		{
			n = atoi(ipaddr+pch[i-1]+1);
			*(waddr + i) = n;
		}
		
	}
}


/**
 * Converts a character IP address to a character IP and saves it.
 *
 * @param System information
 *
 * @return RT_EOK     set ip address is ok
 *         RT_ERROR   set ip address is error
 *
 */
void saveNetAddr(net_addr_config *net)
{
	if(!net) return;
	
	int * waddr = (int *)(&net_addr);
	//ip
	set_ipaddr(net->ipaddr,waddr);

	//MASK  
	set_ipaddr(net->ipmaskaddr,waddr+4);

	//gateway address of target
	set_ipaddr(net->ipgatwayaddr,waddr+8);
	
}


void entryEthStatus(void)
{
	at_printf("Abox eth is init \n");
	Cdma_Info.link = CONNECT_AT;
	dicInfo.secondDicInfo.panelLink = Panel_con_eth;
	dicInfo.secondDicInfo.serConnum++;
	OutputLed(LEDOFF, LED_CAN);
	cleanCbox();
	
}

void exitEthStatus(void)
{
	at_printf("Abox eth is quit \n");
	Cdma_Info.serHeartTime = 0;
	Cdma_Info.link = DISCONNENCT;
	Cdma_Info.currentConnectStyle = NULL_DISCONNENCT;
	dicInfo.secondDicInfo.panelLink = Panel_disconnect;
	cleanCbox();
	/* let Cbox disconnect */
	rt_thread_delay(POLL_TIME_OUT+1000);
	open_log_diagnosis();

}

void ethWifiStyleStart()
{
	ethStyleStart();
	wifiStyleStart();
}


/* eth : 以太网 动态配置 */
void defaultStyleStart()
{
	serNetConfig.netConfig.configStyle = ETH_STYLE;
	serNetConfig.netConfig.ethConfig.status = DHCP_OPEN;
	ethStyleStart();		
}


void tcpSend(void *parameter)
{
	rt_uint8_t sbuf[BUFF_LEN] = {0};
	stPackCmd *packcmd = (stPackCmd *)sbuf;
	stPackEstCmd *estcmd = (stPackEstCmd *)(sbuf + sizeof(stPackCmd));
	rt_uint16_t len = 0; 
	//at_printf("hello tco send!\r\n");
	while(1)
	{
	//rt_thread_delay(100);
		
		rt_mq_recv(climqSend, sbuf, BUFF_LEN, RT_WAITING_FOREVER);
#if 0
		if(RT_EOK != rt_mq_recv(climqSend, sbuf, BUFF_LEN, 10))
		{
			sendCC3100Data("hello,world!\r\n",15);
			continue;
		}
#endif
		len = sizeof(stPackCmd)+2 + packcmd->len;

		if(len >BUFF_LEN) continue;

		
		e_currentConnectStyle conStyle = returnClientCurrentLinkStyle();
		switch(conStyle)
		{
			case CLIENT_WIFI_CONNECT:
				len =	sendCC3100Data(sbuf,len);
			break;
		
			case CLIENT_ETH_CONNECT:
				len = sendEthernetData(sbuf,len);
			break;
		
			default:
				len = 1;
				break;
		}
		
		if(!(len > 0 ))
		{
			rt_kprintf("Eth send ret err!\r\n");
			quitTcpConnect(); 
			OutputLed(LEDON, LED_CAN);	  
		}
		else
		{
			dicInfo.secondDicInfo.panelSendnum++;
		}
			

		at_printf(" send cmd:0x%x is OK!  estcmd: 0x%x !\r\n ",packcmd->cmd,estcmd->cmd);
	}
		
}


/* dhcp 及 ip 配置 */
void dhcpConfig(void)
{
 	if(serNetConfig.netConfig.configStyle == ETH_STYLE && serNetConfig.netConfig.ethConfig.status == DHCP_CLOSE)
		saveNetAddr(&serNetConfig.netConfig.ethConfig.netAddrConfig);	
}


/* init message queue */
static void initializeClient(void)
{	
	
	aboxMqInfo[A_STOA_MQ].mq =	rt_mq_create("mq_stoA", 1024, 3, RT_IPC_FLAG_FIFO);
	climqSend = rt_mq_create("mq_send",1024, 3, RT_IPC_FLAG_FIFO);

	if(!climqSend)
	{
		rt_kprintf("cli-send mq  creat is error!\r\n");
	}


	if(!aboxMqInfo[A_STOA_MQ].mq)
	{
		rt_kprintf("cli:mq is error!\r\n");
	}
	
}



/* send thread */
int tcp_client_send_init(void)
{
	initializeClient();

	rt_thread_t tid1 = NULL;



	tid1 = rt_thread_create("tcpsend",
                           tcpSend, NULL,
                          512*4,TCP_SEND_PRI, 10);
  

    if (tid1 != RT_NULL) rt_thread_startup(tid1);	
	return 0;
	
}


void wait_netif_link(void)
{
	/* wait for link */
	while((!(netif_list->flags & NETIF_FLAG_LINK_UP) || !(netif_list->flags & NETIF_FLAG_UP))
		&& (serNetConfig.netConfig.configStyle != WIFI_STYLE))
	{
		Cdma_Info.conErrPlace = ETH_NETWORK_START_ERROR;
		at_printf("netif is error!\r\n");
		rt_thread_delay(2000);
	}
}




/* Abox mode: default  */
void defaultConfig(void)
{
	exchange_Abox_init();

	tcp_client_send_init();
	switch(serNetConfig.netConfig.configStyle)
	{	
		case ETH_STYLE:
			ethStyleStart();	
		break;

		case WIFI_STYLE:
			wifiStyleStart();
		break;

		case ETH_WIFI_STYLE:
			 ethWifiStyleStart();
		break;


		default:
			defaultStyleStart();
		break;			

	}

}

/* Abox mode: modbus TCP server  */
#if MODBUS_SERVER_CONTROL
void modbusTcpServerConfig(void)
{
	serNetConfig.netConfig.configStyle = ETH_STYLE ;
	serNetConfig.netConfig.ethConfig.status = DHCP_CLOSE ;

	modbus_tcp_server_init();
}
#endif

/* Abox mode: modbus mqtt mode */
#if MQTT_CLIENT_CONTROL
void mqttClientConfig(void)
{
	serNetConfig.netConfig.configStyle = ETH_STYLE ;
	//serNetConfig.netConfig.ethConfig.status = DHCP_OPEN;
	serNetConfig.netConfig.ethConfig.status = DHCP_CLOSE ;

	//extern void mqtt_test();
	mqtt_test();
}
#endif


#if SSL_CLIENT_CONTROL
void sslClientTest(void)
{
	serNetConfig.netConfig.configStyle = ETH_STYLE;
	serNetConfig.netConfig.ethConfig.status = DHCP_OPEN;

#ifdef RT_USING_LWIP
	{
		/* 动静态配置 驱动已配置 */		
	
		//extern void lwip_sys_init(void);
		//extern void rt_hw_stm32_eth_init(void);

		/* init lwip system */
		lwip_sys_init();	   

		/* register ethernetif device */
		rt_hw_stm32_eth_init();
		
		at_printf("TCP/IP initialized!\n");

		rt_thread_delay(1000);

	}
#endif 

	wait_netif_link();
	
	mbedtls_client_start();
}
#endif

/**
 * This function will check the connection of TCP
 *
 * @param void
 *
 * @return void
 *
 */
void tcpcheck(void *parameter)
{
	rt_uint32_t recved = 0;

	//recvive checked result.
	rt_event_recv(cliEvent,  
					TCP_CLI_ABOX,
					RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,
					RT_WAITING_FOREVER ,
					&recved);

	dicInfo.secondDicInfo.panelLink = Panel_disconnect;
	//readServerInfo(&serNetConfig);
	controlServerInfo(READ_NETCONFIG_INFO,&serNetConfig);

#ifdef ABOX_LOG_OPEN 
	Abox_log_init();
#endif


	switch(Sys_Setup.app.mode)
	{	
#if MODBUS_SERVER_CONTROL			
		case DEV_MODBUS_TCP_SERVER:
			modbusTcpServerConfig();
		break;
#endif



#if MQTT_CLIENT_CONTROL
		case DEV_MQTT_CLIENT:
			mqttClientConfig();
		break;
#endif

#if SSL_CLIENT_CONTROL
		case DEV_SSL_TSL_CLIENT_TEST:
			sslClientTest();
		break;
#endif		


		default:
			defaultConfig();
		break;
	}


}





/**
 * This function initialize the TCP client.
 * Initialization consists of three threads: thread 1 checks the connection of TCP, 
 * thread 2 sends TCP data, and thread 3 receives TCP data.
 *
 * @param device GSM's device.
 *
 * @return the length of buff.
 *
 */ 
extern struct netif *netif_list;//include net information
int tcpclient_init(void)
{
	cliEvent = rt_event_create("cliEvent", RT_IPC_FLAG_FIFO);
	if(!cliEvent)
	{
		rt_kprintf("cli:event is error!\r\n");
	}
	

	//rt_thread_delay(500);
	rt_thread_t tid1 = NULL;


	//TCP check
	tid1 = rt_thread_create("tcpcheck",
                           tcpcheck, NULL,
                          512,TCP_SEND_PRI, 10);
  

    if (tid1 != RT_NULL) rt_thread_startup(tid1);

	return 0;
	
}
INIT_APP_EXPORT(tcpclient_init);


#endif 






/*

//test
//默认配置 OK 
//eth      动态OK 

	//serNetConfig.netConfig.configStyle = ETH_STYLE;
	//serNetConfig.netConfig.ethConfig.status = DHCP_OPEN;



//eth     静态配置

	rt_memcpy(serNetConfig.netConfig.ethConfig.netAddrConfig.ipaddr,"192.168.2.48",sizeof("192.168.2.48"));
	rt_memcpy(serNetConfig.netConfig.ethConfig.netAddrConfig.ipmaskaddr,"255.255.255.0",sizeof("255.255.255.0"));
	rt_memcpy(serNetConfig.netConfig.ethConfig.netAddrConfig.ipgatwayaddr,"192.168.2.1",sizeof("192.168.2.1"));

	serNetConfig.netConfig.configStyle = ETH_STYLE;
	serNetConfig.netConfig.ethConfig.status = DHCP_CLOSE;
	saveNetAddr(&serNetConfig.netConfig.ethConfig.netAddrConfig);


//wifi 
//	serNetConfig.netConfig.configStyle = WIFI_STYLE;


*/
