#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <rtthread.h>
#include <finsh.h>
#include <ctrl.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME    "mqtt.sample"
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR
#include <rtdebug.h>

#include "mqtt_client.h"

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
#define MQTT_URI                "tcp://192.6.6.6:1883" // "tcp://iot.eclipse.org:1883"   // ���ò��Է�������ַ
#define MQTT_USERNAME           "TEST"//"admin"   
#define MQTT_PASSWORD           "TEST"//"admin"
#define MQTT_SUBTOPIC           "/mqtt/test"                   // ���ö�������
#define MQTT_PUBTOPIC           "/mqtt/test"                   // ������������
#define MQTT_WILLMSG            "Goodbye!"                     // ����������Ϣ



typedef mqtt_client MQTTClient;
typedef message_data MessageData;

/* ���� MQTT �ͻ��˻����ṹ�� */
static MQTTClient client;
static int is_started = 0;

/* MQTT �����¼��Զ���ص����� */
static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    rt_kprintf("mqtt sub callback: %.*s %.*s",
               msg_data->topic_name->lenstring.len,
               msg_data->topic_name->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

/* MQTT �����¼�Ĭ�ϻص����� */
static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    rt_kprintf("mqtt sub default callback: %.*s %.*s",
               msg_data->topic_name->lenstring.len,
               msg_data->topic_name->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

/* MQTT �����¼��ص����� */
static void mqtt_connect_callback(MQTTClient *c)
{
    rt_kprintf("inter mqtt_connect_callback!");
}

/* MQTT �����¼��ص����� */
static void mqtt_online_callback(MQTTClient *c)
{
    rt_kprintf("inter mqtt_online_callback!");
}

/* MQTT �����¼��ص����� */
static void mqtt_offline_callback(MQTTClient *c)
{
    rt_kprintf("inter mqtt_offline_callback!");
}

static int cmd_mqtt_start()
{
    /* ʹ�� MQTTPacket_connectData_initializer ��ʼ�� condata ���� */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = { 0 };

   // if (argc != 1)
    {
        rt_kprintf("mqtt_start    --start a mqtt worker thread.\n");
     //   return -1;
    }

    if (is_started)
    {
        rt_kprintf("mqtt client is already connected.");
        return -1;
    }
     /* ���� MQTT �ṹ�����ݲ��� */
    {

	
        client.isconnected = 0;
        client.uri = MQTT_URI;

        /* ��������Ŀͻ��� ID */
        rt_snprintf(cid, sizeof(cid), "rtthread%d", rt_tick_get());
        /* �������Ӳ��� */
        memcpy(&client.condata, &condata, sizeof(condata));
        client.condata.clientID.cstring = cid;
        client.condata.keepAliveInterval = 30;
        client.condata.cleansession = 1;
        client.condata.username.cstring = MQTT_USERNAME;
        client.condata.password.cstring = MQTT_PASSWORD;

        /* ���� MQTT ���Բ��� */
        client.condata.willFlag = 1;
        client.condata.will.qos = 1;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = MQTT_PUBTOPIC;
        client.condata.will.message.cstring = MQTT_WILLMSG;

         /* ���仺���� */
        client.buf_size = client.readbuf_size = 1024;
        client.buf = rt_calloc(1, client.buf_size);
        client.readbuf = rt_calloc(1, client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            rt_kprintf("no memory for MQTT client buffer!");
            return -1;
        }

        /* �����¼��ص����� */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;

        /* ���ö��ı���¼��ص�����*/
        client.message_handlers[0].topicFilter = rt_strdup(MQTT_SUBTOPIC);
        client.message_handlers[0].callback = mqtt_sub_callback;
        client.message_handlers[0].qos = QOS1;

        /* ����Ĭ�ϵĶ�������*/
        client.default_message_handlers = mqtt_sub_default_callback;
    }

    /* ���� MQTT �ͻ��� */
	  paho_mqtt_start(&client,1024,0x10);
    is_started = 1;

    return 0;
}

/* �ú�������ֹͣ MQTT �ͻ��˲��ͷ��ڴ�ռ� */
static int cmd_mqtt_stop(int argc, char **argv)
{
    if (argc != 1)
    {
        rt_kprintf("mqtt_stop    --stop mqtt worker thread and free mqtt client object.\n");
    }

    is_started = 0;

    return paho_mqtt_stop(&client);
}

/* �ú������ڷ������ݵ�ָ�� topic */
static int cmd_mqtt_publish(int argc, char **argv)
{
    if (is_started == 0)
    {
        rt_kprintf("mqtt client is not connected.");
        return -1;
    }

    if (argc == 2)
    {
		paho_mqtt_publish(&client, QOS1, MQTT_PUBTOPIC, argv[1],10);
    }
    else if (argc == 3)
    {    
        paho_mqtt_publish(&client, QOS1, argv[1], argv[2],10);
    }
    else
    {
        rt_kprintf("mqtt_publish <topic> [message]  --mqtt publish message to specified topic.\n");
        return -1;
    }

    return 0;
}

/* MQTT �µĶ����¼��Զ���ص����� */
static void mqtt_new_sub_callback(MQTTClient *client, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    rt_kprintf("mqtt new subscribe callback: %.*s %.*s",
               msg_data->topic_name->lenstring.len,
               msg_data->topic_name->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

/* �ú������ڶ����µ� Topic */
static int cmd_mqtt_subscribe(int argc, char **argv)
{
    if (argc != 2)
    {
        rt_kprintf("mqtt_subscribe [topic]  --send an mqtt subscribe packet and wait for suback before returning.\n");
        return -1;
    }
	
	if (is_started == 0)
    {
        rt_kprintf("mqtt client is not connected.");
        return -1;
    }

    return paho_mqtt_subscribe(&client, QOS1, argv[1], mqtt_new_sub_callback);
}

/* �ú�������ȡ������ָ���� Topic */
static int cmd_mqtt_unsubscribe(int argc, char **argv)
{
    if (argc != 2)
    {
        rt_kprintf("mqtt_unsubscribe [topic]  --send an mqtt unsubscribe packet and wait for suback before returning.\n");
        return -1;
    }
	
	if (is_started == 0)
    {
        rt_kprintf("mqtt client is not connected.");
        return -1;
    }

    return paho_mqtt_unsubscribe(&client, argv[1]);
}

#ifdef FINSH_USING_MSH
FINSH_FUNCTION_EXPORT_ALIAS(cmd_mqtt_start, __cmd_mqtt_start, startup mqtt client.);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_mqtt_stop, __cmd_mqtt_stop, stop mqtt client.);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_mqtt_publish, __cmd_mqtt_publish, mqtt publish message to specified topic.);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_mqtt_subscribe,__cmd_mqtt_subscribe,  mqtt subscribe topic);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_mqtt_unsubscribe,__cmd_mqtt_unsubscribe,  mqtt unsubscribe topic);
#endif /* FINSH_USING_MSH */


#if 0
## ����ʾ��

�� msh ��������������ʾ�����룬����ʵ���������������������ض�����������Ϣ�Ĺ��ܣ�����ʾ����������Ч�����£�

- ���� MQTT �ͻ��ˣ����Ӵ�����������������⣺

```c
msh />mqtt_start                /* ���� MQTT �ͻ������� Eclipse ������ */
inter mqtt_connect_callback!  /* ���������ӳɹ����������ӻص�������ӡ��������Ϣ */
ipv4 address port: 1883
[MQTT] HOST =  'iot.eclipse.org'
msh />[MQTT] Subscribe #0 /mqtt/test  OK!  /* �������� /mqtt/test �ɹ� */
inter mqtt_online_callback!   /* MQTT ���߳ɹ����������߻ص����� */
msh />
```
- ��Ϊ��������ָ�����ⷢ����Ϣ��

```c
msh />mqtt_pub  /mqtt/test hello-rtthread   /* ��ָ�����ⷢ��  hello-rtthread ��Ϣ */
msh />mqtt sub callback: /mqtt/test hello-rtthread /* �յ���Ϣ��ִ�лص����� */
msh />
```

-  �����µ� topic

```c
msh />mqtt_subscribe /test/topic          /* ����ָ���� topic */
[I/MQTT:] Subscribe #1 /test/topic OK!
msh />
```

- ȡ������ topic

```c
msh />mqtt_unsubscribe /test/topic       /* ȡ������ָ���� topic */
[561315] I/MQTT: Unsubscribe #1 /test/topic OK!
msh />
```

- �ر� MQTT  �ͻ���

```c
msh />mqtt_stop
[585107] D/MQTT: pub_sock recv 11 byte: DISCONNECT
[585110] I/MQTT: MQTT server is disconnected.
msh />
```

#endif
