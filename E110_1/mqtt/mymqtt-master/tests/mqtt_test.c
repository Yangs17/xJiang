/*
 * File      : mqtt_test.c
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-03-12     armink       the first version
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <rtthread.h>
#include "mqtt_client.h"

#define PKG_USING_MYMQTT_TEST

#ifdef PKG_USING_MYMQTT_TEST

/**
 * MQTT URI farmat:
 * domain mode
 * tcp://iot.eclipse.org:1883
 *
 * ipv4 mode
 * tcp://192.168.10.1:1883
 * ssl://192.168.10.1:1884
 *
 * ipv6 mode
 * tcp://[fe80::20c:29ff:fe9a:a07e]:1883
 * ssl://[fe80::20c:29ff:fe9a:a07e]:1884
 */
#define MQTT_TEST_SERVER_URI    "tcp://192.6.6.6:1883" //"tcp://192.168.2.242:1883"  //"tcp://iot.eclipse.org:1883"
#define MQTT_CLIENTID           "rtthread-mqtt"
#define MQTT_USERNAME           "TEST"  // "admin"
#define MQTT_PASSWORD           "TEST"  //"admin"
#define MQTT_SUBTOPIC           "/mqtt/test"                   // 设置订阅主题
#define MQTT_PUBTOPIC           "/mqtt/test"                   // 设置推送主题
#define MQTT_WILLMSG            "Goodbye!"                     // 设置遗言消息


#define MQTT_TEST_QOS           QOS1
#define MQTT_PUB_SUB_BUF_SIZE   1024

#define CMD_INFO                "'mqtt_test <start|stop>'"
#define TEST_DATA_SIZE          256
#define PUB_CYCLE_TM            1000

typedef mqtt_client MQTTClient;
typedef message_data MessageData;

static rt_thread_t pub_thread_tid = RT_NULL;

static char *pub_data = RT_NULL;

/* define MQTT client context */
static MQTTClient client;

static rt_uint32_t pub_count = 0, sub_count = 0;
static int recon_count = -1;
//static int test_start_tm = 0;
static int test_is_started = 0;



#define MQTT_CLIENT_IP        "192.6.6.120"
#define MQTT_CLIENT_MASK	  "255.255.255.0"
#define MQTT_CLIENT_GATEWAY   "192.6.6.1"


void mqtt_client_set(void)
{
	/* server ip config */
	net_addr_config net;
	rt_memset(&net,0,sizeof(net));
	
	rt_memcpy(&net.ipaddr,MQTT_CLIENT_IP,strlen(MQTT_CLIENT_IP));
	rt_memcpy(&net.ipmaskaddr,MQTT_CLIENT_MASK,strlen(MQTT_CLIENT_MASK));
	rt_memcpy(&net.ipgatwayaddr,MQTT_CLIENT_GATEWAY,strlen(MQTT_CLIENT_GATEWAY));
	
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




static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    sub_count ++;
    return;
}

static void mqtt_connect_callback(MQTTClient *c)
{
    return;
}

static void mqtt_online_callback(MQTTClient *c)
{
    recon_count ++;
    return;
}

static void mqtt_offline_callback(MQTTClient *c)
{
    return;
}

/**
 * This function create and config a mqtt client.
 *
 * @param void
 *
 * @return none
 */
static void mq_start(void)
{
    /* init condata param by using MQTTPacket_connectData_initializer */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;

    rt_memset(&client, 0, sizeof(MQTTClient));

    /* config MQTT context param */
    {
        client.uri = MQTT_TEST_SERVER_URI;

        /* config connect param */
        memcpy(&client.condata, &condata, sizeof(condata));
        client.condata.clientID.cstring = MQTT_CLIENTID;
        client.condata.keepAliveInterval = 60;
        client.condata.cleansession = 1;
        client.condata.username.cstring = MQTT_USERNAME;
        client.condata.password.cstring = MQTT_PASSWORD;

        /* config MQTT will param. */
        client.condata.willFlag = 1;
        client.condata.will.qos = MQTT_TEST_QOS;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = MQTT_PUBTOPIC;
        client.condata.will.message.cstring = MQTT_WILLMSG;

        /* malloc buffer. */
        client.buf_size = client.readbuf_size = MQTT_PUB_SUB_BUF_SIZE;
        client.buf = rt_malloc(client.buf_size);
        client.readbuf = rt_malloc(client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            rt_kprintf("no memory for MQTT client buffer!\n");
            goto _exit;
        }

        /* set event callback function */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;

        /* set subscribe table and event callback */
        client.message_handlers[0].topicFilter = rt_strdup(MQTT_SUBTOPIC);
        client.message_handlers[0].callback = mqtt_sub_callback;
        client.message_handlers[0].qos = MQTT_TEST_QOS;
		

        /* set default subscribe event callback */
        client.default_message_handlers = mqtt_sub_callback;
    }

    /* run mqtt client */
    paho_mqtt_start(&client,1024,0x10);

    return;

_exit:
    if (client.buf)
    {
        rt_free(client.buf);
        client.buf = RT_NULL;
    }
    if (client.readbuf)
    {
        rt_free(client.readbuf);
        client.readbuf = RT_NULL;
    }
    return;
}

static void show_test_info(void)
{
    char temp[50] = {0};
    rt_kprintf("\r==== MQTT Stability test ====\n");
    rt_kprintf("Server: "MQTT_TEST_SERVER_URI"\n");
    rt_kprintf("QoS   : %d\n", MQTT_TEST_QOS);

//    rt_kprintf("Test duration(sec)            : %d\n", time((time_t *)RT_NULL) - test_start_tm);
    rt_kprintf("Number of published  packages : %d\n", pub_count);
    rt_kprintf("Number of subscribed packages : %d\n", sub_count);
    snprintf(temp, sizeof(temp), "Packet loss rate              : %.2f%%   \n", (float)((float)(pub_count - sub_count) * 100.0f / pub_count));
    rt_kprintf(temp);
    rt_kprintf("Number of reconnections       : %d\n", recon_count);
    /* up the cursor 8 line */
    rt_kprintf("\033[8A");
}

static void thread_pub(void *parameter)
{
    pub_data = rt_malloc(TEST_DATA_SIZE * sizeof(char));
    if (!pub_data)
    {
        rt_kprintf("no memory for pub_data\n");
        return;
    }
    rt_memset(pub_data, '*', TEST_DATA_SIZE * sizeof(char));

//    test_start_tm = time((time_t *)RT_NULL);
//    rt_kprintf("test start at '%d'\r\n", test_start_tm);

    while (1)
    {
        if (!paho_mqtt_publish(&client, QOS1, MQTT_PUBTOPIC, pub_data,TEST_DATA_SIZE))
        {
            ++ pub_count;
        }

        rt_thread_delay(PUB_CYCLE_TM);

        show_test_info();
    }
}

static void mqtt_test_start(void)
{
    if (test_is_started)
    {
        return;
    }

    mq_start();

    while (!client.isconnected)
    {
        rt_kprintf("Waiting for mqtt connection...\n");
        rt_thread_delay(1000);
    }

    pub_thread_tid = rt_thread_create("pub_thread", thread_pub, RT_NULL, 1024, 8, 100);
    if (pub_thread_tid != RT_NULL)
    {
        rt_thread_startup(pub_thread_tid);
    }

    test_is_started = 1;

    return;
}

static void mqtt_test_stop(void)
{
    MQTTClient *local_client = &client;

    if (pub_thread_tid)
    {
        rt_thread_delete(pub_thread_tid);
    }

    if (pub_data)
    {
        rt_free(pub_data);
        pub_data = RT_NULL;
    }

    if (local_client)
    {
        paho_mqtt_stop(local_client);
    }

    /* up the cursor 1 line */
    rt_kprintf("\033[1A");

    show_test_info();

    /* down the cursor 10 line */
    rt_kprintf("\033[10B");

    pub_count = sub_count = recon_count = 0;
    test_is_started = 0;

    rt_kprintf("==== MQTT Stability test stop ====\n");
}

void mqtt_test()
{
	mqtt_client_set();
	
   	mqtt_test_start();
		
	  //mqtt_test_stop();
}

//MSH_CMD_EXPORT(mqtt_test, MQTT test CMD_INFO);

#endif /* PKG_USING_PAHOMQTT_TEST */
