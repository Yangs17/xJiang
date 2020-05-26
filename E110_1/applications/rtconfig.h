/* RT-Thread config file */
#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__


/* ϵͳ�ж������ƴ�С*/
#define RT_NAME_MAX 						16

//#define RT_USING_NEWLIB
/* ���뷽ʽ*/
#define RT_ALIGN_SIZE 					4

/* ���֧�ֵ����ȼ���32��256 */
#define RT_THREAD_PRIORITY_MAX 	32


/* ÿ��Ľ�����*/
#define RT_TICK_PER_SECOND 			1000
// <bool name="RT_USING_MODULE" description="Using Application Module" default="true" />
//#define RT_USING_MODULE
/* SECTION: ����ѡ��*/
/* ����*/
//#define RT_DEBUG				        1

/* �߳�ջ��������*/
#define RT_USING_OVERFLOW_CHECK

/* ֧�ֹ��Ӻ���*/
#define RT_USING_HOOK

/* SECTION: �̼߳�ͨ��*/
/* ֧���ź���*/
#define RT_USING_SEMAPHORE

/* RT_NAME_MAX*/
/* ֧�ֻ�����*/
#define RT_USING_MUTEX

/* ֧���¼�*/
#define RT_USING_EVENT

/* ֧������*/
#define RT_USING_MAILBOX

/* ֧����Ϣ����*/
#define RT_USING_MESSAGEQUEUE

/* SECTION: �ڴ����*/
/* ֧���ڴ�ع���*/
//#define RT_USING_MEMPOOL

/* ֧�ֶ�̬���ڴ����*/
#define RT_USING_HEAP

/* ʹ��С���ڴ�ģ��*/
#define RT_USING_SMALL_MEM
/* ֧��SLAB������*/
/* #define RT USING SLAB */
/* SECTION: �豸IOϵͳ*/
/* ֧���豸IO����ϵͳ*/
// <section name="RT_USING_DEVICE" description="Using Device Driver Framework" default="true" >
#define RT_USING_DEVICE
// <bool name=RT_USING_DEVICE_IPC description="Using IPC in Device Driver Framework" default="true" />
#define RT_USING_DEVICE_IPC
// <bool name="RT_USING_COMPONENTS_INIT" description="֧��ϵͳ����Զ���ʼ������" default="true" />
#define RT_USING_COMPONENTS_INIT
// <bool name="RT_USING_SERIAL" description="Using Serial Device Driver Framework" default="true" />
#define RT_USING_SERIAL

#define RT_USBH_MSTORAGE


/* ֧��UART0, 1��2��3  */
#define MAX_USART 6


//#define RT_SERIAL_RB_BUFSZ
/* SECTION: Console options */
#define RT_USING_CONSOLE
#define RT_USING_CONSOLE_UART
#define CONSOLE_DEVICE      				"uart3"  
#define FINSH_DEVICE_NAME   				CONSOLE_DEVICE
#define RT_CONSOLE_DEVICE_NAME  			CONSOLE_DEVICE


//this usart will work for AT device
#define AT_DEVICE_NAME		            	"uart2"
#define SUB_G_DEVICE_NAME					"si443M"

/* SECTION: consoleѡ��*/
/* console���峤��*/
#define RT_CONSOLEBUF_SIZE 					128*2
/* SECTION: FinSH shell ѡ��*/
/* ֧��finsh��Ϊshell */
#define FINSH_THREAD_PRIORITY				20
#define FINSH_THREAD_STACK_SIZE 		    1024*2

#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_USING_HISTORY					1
/* ʹ�÷��ϱ�*/
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
/* SECTION: mini libc��*/
/* ʹ��С��libc��*/
/* #define RT USING MINILIBC */
/* SECTION: C++ ѡ��*/
/* ֧��C++ */
/* #define RT_USING_CPLUSPLUS */

/* Using Software Timer */
#define RT_USING_TIMER_SOFT 
#define RT_TIMER_THREAD_PRIO				4
#define RT_TIMER_THREAD_STACK_SIZE	512
//#define HADDWAE_TEST
//#define RT_USING_COMPONENTS_INIT
#define RT_USING_RTC
#define RT_USING_SPI
#define SPI_USE_DMA

#define RT_USING_WDT   	 		//zxy add 17/11/21
//#define RT_TEMP_USING
//#define RT_FIRE_USING
#define USING_BXCAN1
#define RT_CAN_USING_HDR
//#define RT_METER_USING

#define UDISK_MOUNTPOINT "/"   //usb disk



// #define RT_USING_MTD_NAND
// #define RT_USING_SD_CARD
// #define RT_USING_DFS_UFFS


#if 0
#define RT_USING_DFS_ELMFAT
//#define RT_USING_DFS_ROMFS
#define RT_DFS_ELM_CODE_PAGE				437
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_ERASE
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_DRIVES           2
#define RT_DFS_ELM_USE_LFN          3
#define RT_DFS_ELM_MAX_LFN          255
#define RT_DFS_ELM_MAX_SECTOR_SIZE  512

#endif
/* ֧�������ļ�ϵͳ��Ŀ*/
#define DFS_FILESYSTEMS_MAX 				3

/* ���ͬʱ���ļ���*/
#define DFS_FD_MAX 									10

#define DFS_USING_WORKDIR
/* �������������Ŀ*/
#define DFS_CACHE_MAX_NUM 					20



//#define RT_GSM_USING

/* SECTION: ����TCP/IPЭ��ջѡ��*/

/* ֧�ֵĸ����ͨѶ�ķ�ʽ */
#define RT_USING_LWIP  /* ֧��LwIPЭ��ջ*/
//#define RT_USING_CAN   /* ֧��can*/


#define RT_LWIP_USING_RT_MEM
/* ֧��WebServer */
//#define RT_USING_WEBSERVER

/* ��LwIP������Ϣ*/
#define RT_LWIP_DEBUG 

/* ʹ��ICMPЭ��*/
#define RT_LWIP_ICMP

/* ʹ��UDP Э��*/
#define RT_LWIP_UDP

/* ʹ��TCP protocol*/
#define RT_LWIP_TCP

/* ʹ��SNMP Э��*/
/* #define RT_LWIP_SNMP */

/* ʹ��DHCP*/
#define RT_LWIP_DHCP

/* ʹ��DNS */
#define RT_LWIP_DNS
// <integer name="RT_LWIP_PBUF_NUM" description="Maximal number of buffers in the pbuf pool" default="4" />
#define RT_LWIP_PBUF_NUM							4
/* ͬʱ֧�ֵ�TCP������*/
// <integer name="RT_LWIP_TCP_PCB_NUM" description="Maximal number of simultaneously active TCP connections" default="5" />
#define RT_LWIP_TCP_PCB_NUM						5
/* the number of simulatenously active UDP connections*/
#define RT_LWIP_UDP_PCB_NUM 					5
/* TCP���ͻ���ռ�*/
// <integer name="RT_LWIP_TCP_SND_BUF" description="TCP sender buffer size" default="8192" />
#define RT_LWIP_TCP_SND_BUF						(1460 * 6)
// <integer name="RT_LWIP_TCP_WND" description="TCP receive window" default="8192" />
#define RT_LWIP_TCP_WND								(1460 * 2)
// <integer name="RT_LWIP_TCP_SEG_NUM" description="the number of simultaneously queued TCP" default="4" />
#define RT_LWIP_TCP_SEG_NUM						4


/* the application documents */
#define RT_CDMA_USING
#define RT_CLIENT_USING


/* depended for ftp server */
//#define SO_REUSE  1





/* tcp thread options */
#define RT_LWIP_TCPTHREAD_PRIORITY		12
#define RT_LWIP_TCPTHREAD_MBOX_SIZE		8
#define RT_LWIP_TCPTHREAD_STACKSIZE		2048

/* ethernet if thread options */
#define RT_LWIP_ETHTHREAD_PRIORITY		14
#define RT_LWIP_ETHTHREAD_MBOX_SIZE		8
#define RT_LWIP_ETHTHREAD_STACKSIZE		1024


/* TCP sender buffer space */
//#define RT_LWIP_TCP_SND_BUF	8192
/* TCP receive window. */
//#define RT_LWIP_TCP_WND		8192

#define CHECKSUM_CHECK_TCP              0
#define CHECKSUM_CHECK_IP               0
#define CHECKSUM_CHECK_UDP              0

#define CHECKSUM_GEN_TCP                0
#define CHECKSUM_GEN_IP                 0
#define CHECKSUM_GEN_UDP                0

//Receiving the temperature
#define RT_LM73
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS


#endif


