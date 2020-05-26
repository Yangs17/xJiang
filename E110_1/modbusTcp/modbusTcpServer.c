#include "modbusTcp.h"

/*
	1. ����modbus   TCP server    ------ 2019.11.18 zxy	a18667427315@163.com
	2. ��Abox USB ����cbox devID �� Э���е��豸ID���Ӧ�ı���
		ͨ��������������������յ��İ�Ҫ�����ĸ�Cbox������
*/

/*
���ӣ�

ʪ�ȣ� ���ڣ�ʵ��ֵ *100 ������λΪ�ٷֺš�
�¶ȣ����ڣ�ʵ��ֵ *100 + 27315 ����
�¶Ⱦ���: :
���� ���� ��ȡ���¶� ֵ Ϊ6 : 75 C1 (16 ?����) ) �C ת��Ϊ 30145 ( ʮ����) )
ʵ���¶��� T T ?��ʾ :T = (30145 ?�C 27315)/100 = 28.3 ( ( ��) )
�Ĵ������ݶ��壺
ֻ������?
0x0000: ��ǰʪ��ֵ��
0x0001: ��ǰ�¶�ֵ��
��ȡ��ʪ�����ݾ��� �� ��ȡ1 ���Ǳ�����ʪ��ֵ ��03 ��� ��
��ʼ��ַ ��ȡ����
���� 00 00 00 00 00 06 01 03 00 00 00 02
��Ӧ�� 00 00 00 00 00 07 01 03 04 18 3E 77 51

Э�����:
����:
00 00 00 00 �̶���
00 06 ���ݳ���
01 �豸��ַ
03 ������
00 00 ��ʼ�Ĵ�����ַ
00 02 ��ȡ�Ĵ�������
��Ӧ:
00 00 00 00 �̶���
00 07 ���ݳ���
01 �豸��ַ
03 ������
04 ���ؼĴ������ݳ���
18 3E ʪ��(ʵ��ʪ�� = 6206/100 = 62.06%)
77 51 �¶�(ʵ���¶� = (30545 -27315)/100 = 31.30��)

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
		/* ����̬���� ���������� */		
	
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

	
	/* ��ʼ������˵�ַ*/
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(MODBUS_SERVER_PORT); /* ����˹����Ķ˿�*/
	server_addr.sin_addr.s_addr = INADDR_ANY;
	rt_memset(&(server_addr.sin_zero), 8, sizeof(server_addr.sin_zero));



	
	/* ��socket������˵�ַ*/
	if (bind(socketID, (struct sockaddr *) &server_addr, sizeof(struct sockaddr))== -1)
	{
		rt_kprintf("Unable to bind\n");
		rt_free(recv_data);
		goto loop;
	}

	
	/* ��socket�Ͻ��м���*/
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
		//�����趨��·�����ļ���������
		tmp = readFds;
		//����selectʵ�ֶ�·����
		ret = select(maxFd + 1, &tmp, NULL, NULL, NULL);
		if (ret <= 0)
		{
			rt_kprintf("select error");
			break;
		}

		for (i = 0; i <= maxFd; i++)
		{	
			//�ж���һ���ļ��������б仯 		
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
					
					//���µ��������������ŵ���·���ö�����
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









