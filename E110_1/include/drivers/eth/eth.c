#include <netdb.h> 
#include <sockets.h> 
#include "netif.h"
#include "stm32f4xx_gpio.h"
#include "at.h"
#include "abox.h"
#include "adc.h"
#include "sockets.h"

static int sock = -1;

/* 判断以太网连接时的发送情况 */
rt_err_t judgeEthSendStatus(void)
{
	if(Panel_con_eth != dicInfo.secondDicInfo.panelLink
	|| !(netif_list->flags & NETIF_FLAG_UP)
	|| !(netif_list->flags & NETIF_FLAG_LINK_UP)) 
	{	
		at_printf(" tcp is closed! %d  \r\n",dicInfo.secondDicInfo.panelLink);
		if(Panel_con_eth == dicInfo.secondDicInfo.panelLink)
		{
			quitTcpConnect();
			//close(sock);
		}
		return RT_ERROR;
	}
	return RT_EOK;
}





/* chat client */
void ethChat(void)
{
	fd_set rfd;
	fd_set tmp;
	struct timeval timeout;  //超时时间 

	timeout.tv_sec = READ_CLI_TIMEOUT_S;
	timeout.tv_usec = 0;

	FD_ZERO(&rfd);
	FD_ZERO(&tmp);
	FD_SET(sock, &rfd);

	int len = 0;

	while(1)
	{	
		tmp = rfd;

		
		len = select(sock+1,&tmp,NULL,NULL,&timeout);
		if(len > 0)
		{
			if(FD_ISSET(sock,&tmp))
			{
				len = recv(sock,Eth_Rev_Buff+Eth_recv_len, sizeof(Eth_Rev_Buff)-Eth_recv_len, MSG_DONTWAIT);
				if(len>0)
				{
					Eth_recv_len += len;				
					processDataFromServer();
				}
				else
				{
					at_printf("eth recv err len = %d!\r\n",len);
					break;
				}
			}else
			break;
		}
		else
		{
			at_printf("fail to select, ret = %d !\r\n",len);
			break;
		}


		
			
	}
	
	FD_CLR(sock, &rfd);
	
	//phy is ok, server is closed.  panel is dead.
	at_printf("tcp recv is break!\r\n");
	OutputLed(LEDON, LED_CAN);	 
	close(sock);			


}






void tcp_eth_recv(void *parameter)
{

	//char *recv_data;
	struct hostent *host = RT_NULL;
	struct sockaddr_in server_addr;
	rt_uint32_t recved = 0;
	char cnt ;
	char cnt1;
	u16_t port;	

	while(1)
	{
		
		OutputLed(LEDON, LED_CAN);
		cnt = 0;
		cnt1 = 0;
		
		//establish a TCP connection.
		at_printf("TCP_CREAT  is open\n");
		closesocket(sock);                                                                                                            	

		while(1)
		{
			Cdma_Info.conErrPlace = ETH_NETWORK_START_ERROR;
		
			/* check link ok!*/ 
			while(!(netif_list->flags & NETIF_FLAG_LINK_UP)  
				  || !(netif_list->flags & NETIF_FLAG_UP)
				  || (dicInfo.secondDicInfo.panelLink == Panel_con_eth)
				  )
				  
			{	
				rt_thread_delay(RT_TICK_PER_SECOND );
				if(dicInfo.secondDicInfo.panelLink == Panel_con_eth)
				{
					//at_printf("choose WIFI connect!\r\n");
					continue;
				}
				else
				{
					at_printf("netif link err!\r\n");
					if(cnt1++ > 10)
					      goto loop;
				}
			
	

			}

			dicInfo.secondDicInfo.panelLink = Panel_tryconnect;

			port = atoi(Sys_Setup.serport);
			host = gethostbyname(Sys_Setup.seraddr);

			at_printf("sever port:%s ,addr:%s \r\n",Sys_Setup.serport,Sys_Setup.seraddr);

			/* get sock */
			while(1)
			{									
				if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
				{
					at_printf("Socket error\n");
					Cdma_Info.conErrPlace = ETH_NETWORK_SOCK_ERROR;
					continue;
				}
				at_printf("socket =%d\n",sock);
				break;		
			}

			/* connect */
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(port);
			server_addr.sin_addr = *((struct in_addr *) host->h_addr);
			rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));


			if(connect(sock, (struct sockaddr *) &server_addr,sizeof(struct sockaddr)) == -1)
			{
				at_printf("Connect error!\n");
				cnt ++;
				closesocket(sock);

				Cdma_Info.conErrPlace = ETH_NETWORK_CONNECT_ERROR;
				
				//if conenct fail，it will reconnect.
				rt_thread_delay(RT_TICK_PER_SECOND);

				if(cnt > RECONNECT_TIMES) /* connect failed */
				{	
					changeServerInfo();
					cnt = 0;
				}

				loop:
				if(returnConfigLinkMothod()==ETH_WIFI_STYLE)
				{
					rt_event_send(cliEvent, TCP_CLI_WIFI);					

					//recvive checked result.
					rt_event_recv(cliEvent,  
									TCP_CLI_ETH,
									RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,
									10*1000 ,
									&recved);

				}

							
			}
			else  //eth ok
			{
				at_printf("Connect	OK!\n");
				Cdma_Info.conErrPlace = ETH_NETWORK_NO_ERROR;
				cnt = 0;
				rt_thread_delay(RT_TICK_PER_SECOND);
				Cdma_Info.currentConnectStyle = CLIENT_ETH_CONNECT;
				break;
			}

			 
		}


		/* Abox creat */
		{
			entryEthStatus();

			ethChat();//

			exitEthStatus();
					
		}
	


	}
}


int sendEthernetData(rt_uint8_t *sendBuf,rt_uint32_t len)
{
	if(!sendBuf || !len) return 0;
	if(len > BUFF_LEN)   return 0;
#if 0
	int send_result = 0;
	int send_time   = 0;
	int send_len    = 0;
	
	while(1)
	{
		send_result = send(sock, sendBuf, len-send_len,  MSG_DONTWAIT);
		send_time ++;

		/* 发送出错 */
		if(!(send_result > 0))
		{
			return send_result;
		}

		/* 发送成功 */
		send_len += send_result;
		if(send_len == len)
			break;
		else
		{
			/* 发了一部分 */
			if(send_time > 10)
				break;

			rt_thread_delay(10);
		}
	
	}
	return send_len;
#endif
	 return send(sock, sendBuf, len, 0);
}


void quitEthernetTcp(void)
{
	 close(sock);
	 at_printf("close ethernet sock.\r\n");
}




/* 以太网模式启动 */
void ethStyleStart(void)
{
	dhcpConfig();
	
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

	}
#endif 
//tcp_keepalive(struct tcp_pcb * pcb)

	rt_thread_t	tid1 = rt_thread_create("eth_recv",
                           tcp_eth_recv, NULL,
                          512*4,TCP_RECV_PRI, 10);//缓冲区2K 当退出的时候 栈溢出
  

    if (tid1 != RT_NULL) rt_thread_startup(tid1);

		
}


