#include "modbusTcp.h"

/*
	1. 添加modbus   TCP server    ------ 2019.11.18 zxy	a18667427315@163.com
	2. 从Abox USB 导入cbox devID 与 协议中的设备ID相对应的表，
		通过解析这个表来分析接收到的包要请求哪个Cbox的数据
*/

/*
例子：

湿度： 等于（实际值 *100 ），单位为百分号。
温度：等于（实际值 *100 + 27315 ）。
温度举例: :
假如 软件 读取到温度 值 为6 : 75 C1 (16 ?进制) ) – 转换为 30145 ( 十进制) )
实际温度用 T T ?表示 :T = (30145 ?– 27315)/100 = 28.3 ( ( ℃) )
寄存器内容定义：
只读对象：?
0x0000: 当前湿度值，
0x0001: 当前温度值，
读取温湿度数据举例 （ 读取1 号仪表的温湿度值 ，03 命令） ：
起始地址 读取个数
请求： 00 00 00 00 00 06 01 03 00 00 00 02
响应： 00 00 00 00 00 07 01 03 04 18 3E 77 51

协议解析:
请求:
00 00 00 00 固定码
00 06 数据长度
01 设备地址
03 功能码
00 00 起始寄存器地址
00 02 读取寄存器个数
响应:
00 00 00 00 固定码
00 07 数据长度
01 设备地址
03 功能码
04 返回寄存器数据长度
18 3E 湿度(实际湿度 = 6206/100 = 62.06%)
77 51 温度(实际温度 = (30545 -27315)/100 = 31.30℃)

*/



void modbus_server_ip_set(void)
{
	/* server ip config */
	net_addr_config net;
	rt_memset(&net,0,sizeof(net));
	
	rt_memcpy(&net.ipaddr,MODBUS_SERVER_IP,strlen(MODBUS_SERVER_IP));
	rt_memcpy(&net.ipmaskaddr,MODBUS_SERVER_MASK,strlen(MODBUS_SERVER_MASK));
	rt_memcpy(&net.ipgatwayaddr,MODBUS_SERVER_GATEWAY,strlen(MODBUS_SERVER_GATEWAY));
	
	saveNetAddr(&net);

#ifdef RT_USING_LWIP
	{
		/* 动静态配置 驱动已配置 */		
	
		extern void lwip_sys_init(void);
		extern void rt_hw_stm32_eth_init(void);

		/* init lwip system */
		lwip_sys_init();	   

		/* register ethernetif device */
		rt_hw_stm32_eth_init();
		
		at_printf("TCP/IP initialized!\n");

	}
#endif 


	wait_netif_link();


	
}



void modbus_tcp_trhead(void *parameter)
{

	char *recv_data; 
	int socketID = 0;
	struct sockaddr_in server_addr;
	int newID = 0;
	int ret = 0;
	int maxFd = 0;
	int i = 0;
	fd_set readFds;
	fd_set tmp;

	recv_data = rt_malloc(BUFF_LEN); 
	if (recv_data == RT_NULL)
	{
		rt_kprintf("No memory\n");
		return;
	}


	modbus_server_ip_set();	



	if ((socketID = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		rt_kprintf("Socket error\n");
		rt_free(recv_data);
		goto loop;
	}

	
	/* 初始化服务端地址*/
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(MODBUS_SERVER_PORT); /* 服务端工作的端口*/
	server_addr.sin_addr.s_addr = INADDR_ANY;
	rt_memset(&(server_addr.sin_zero), 8, sizeof(server_addr.sin_zero));



	
	/* 绑定socket到服务端地址*/
	if (bind(socketID, (struct sockaddr *) &server_addr, sizeof(struct sockaddr))== -1)
	{
		rt_kprintf("Unable to bind\n");
		rt_free(recv_data);
		goto loop;
	}

	
	/* 在socket上进行监听*/
	if (listen(socketID, LISTEN_MEM_MAX) == -1)
	{
		rt_kprintf("Listen error\n");
		/* release recv buffer */
		rt_free(recv_data);
		goto loop;
	}
	
	rt_kprintf("\nTCPServer Waiting for client on port: 502...\n");


	FD_ZERO(&readFds);	
	FD_SET(socketID, &readFds);
	maxFd = socketID;
	
	while(1)
	{
		//重新设定多路复用文件描述符集
		tmp = readFds;
		//调用select实现多路复用
		ret = select(maxFd + 1, &tmp, NULL, NULL, NULL);
		if (ret <= 0)
		{
			rt_kprintf("select error");
			break;
		}

		for (i = 0; i <= maxFd; i++)
		{	
			//判断哪一个文件描述符有变化 		
			if (FD_ISSET(i, &tmp))
			{
				if (i == socketID)
				{
					struct sockaddr_in addr;
					int addrLength = sizeof(addr);
					newID = accept(socketID, (struct sockaddr *)&addr, &addrLength);
					if (newID < 0)
					{
						rt_kprintf("accept error");
						continue;	
					}
					rt_kprintf("client port : %d \r\n", ntohs(addr.sin_port));
					
					//把新的已连接描述符放到多路复用队列里
					FD_SET(newID, &readFds);
					maxFd = maxFd > newID ? maxFd : newID; 
					//maxFd = ((maxFd - 1) > newID) ? maxFd : (newID + 1) ;
				}
				else 
				{
					ret = recv(i, recv_data, BUFF_LEN, 0);	
					if ((ret == 0) || (ret  < 0))
					{
						rt_kprintf("read data error");
						close(i);
						FD_CLR(i, &readFds);
						continue;
					}			

					rt_kprintf("server get len: %d \r\n data:  ",ret);
					int j = 0;
					for(j=0;j<ret;j++)
					rt_kprintf(" 0x%x  \r\n",recv_data[j]);
					
					//send(i, "get it ", 7, 0);
				}
			}//if
		
		}//for 
	}//while

loop:
	close(socketID);
	rt_free(recv_data);
	return ;

}


int modbus_tcp_server_init(void)
{	
	rt_thread_t tid = NULL;

	//TCP check
	tid = rt_thread_create("tcpcheck",
                           modbus_tcp_trhead, NULL,
                           512*3,TCP_RECV_PRI, 10);
  
    if (tid != RT_NULL) rt_thread_startup(tid);
	return 0;
	
}










