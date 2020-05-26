#include <rtthread.h>
#include <lwip/sockets.h> 
#include "rtconfig.h"

#define PORT 5000

/* �����õ�������*/
ALIGN(4)   //���ֽڶ���


static const char send_data[] = "This is TCP Server from RT-Thread.";


void tcpserv(void* parameter)
{
	char *recv_data; 
	rt_uint32_t sin_size;
	
	int sock, connected, bytes_received;
	struct sockaddr_in server_addr, client_addr;
	rt_bool_t stop = RT_FALSE; /* ֹͣ��־*/


	recv_data = rt_malloc(1024); 
	if (recv_data == RT_NULL)
	{
		rt_kprintf("No memory\n");
		return;
	}

	

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		rt_kprintf("Socket error\n");
		rt_free(recv_data);
		return;
	}

	
	/* ��ʼ������˵�ַ*/
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT); /* ����˹����Ķ˿�*/
	server_addr.sin_addr.s_addr = INADDR_ANY;
	rt_memset(&(server_addr.sin_zero), 8, sizeof(server_addr.sin_zero));



	
	/* ��socket������˵�ַ*/
	if (bind(sock, (struct sockaddr *) &server_addr, sizeof(struct sockaddr))== -1)
	{
		rt_kprintf("Unable to bind\n");
		rt_free(recv_data);
		return;
	}

	
	/* ��socket�Ͻ��м���*/
	if (listen(sock, 5) == -1)
	{
		rt_kprintf("Listen error\n");
		/* release recv buffer */
		rt_free(recv_data);
		return;
	}
	
	rt_kprintf("\nTCPServer Waiting for client on port 5000...\n");
	
	while (stop != RT_TRUE)
	{
		sin_size = sizeof(struct sockaddr_in);

		//block
		connected = accept(sock, (struct sockaddr *) &client_addr, &sin_size);

		rt_kprintf("I got a connection from (%s , %d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		
	
		while (1)
		{
			/* �������ݵ�connected socket */
			send(connected, send_data, strlen(send_data), 0);

			bytes_received = recv(connected, recv_data, 1024, 0);
			if (bytes_received < 0)
			{
				/* ����ʧ�ܣ��ر����connected socket */
				lwip_close(connected);
				break;

			}
			/* �н��յ����ݣ���ĩ������*/
			recv_data[bytes_received] = '\0';
			{
				/* �ڿ����ն���ʾ�յ�������*/
				rt_kprintf("RECIEVED DATA = %s \n", recv_data);
			}
		}
	}
	/* �˳�����*/
	lwip_close(sock);
	/* �ͷŽ��ջ���*/
	rt_free(recv_data);
	return;
}
#ifdef RT_USING_FINSH
#include <finsh.h>
/* ���tcpserv������finsh shell��*/
FINSH_FUNCTION_EXPORT(tcpserv, startup tcp server);
#endif
