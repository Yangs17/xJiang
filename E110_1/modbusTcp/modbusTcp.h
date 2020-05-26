#ifndef __MODBUS_TCP_H__
#define __MODBUS_TCP_H__


#include <rtthread.h>
#include <lwip/sockets.h> 
#include "rtconfig.h"
#include "abox.h"


/* *************************  modbus TCP  ************************* */
/*
note:
1. ���н��յ���˫�ֽڶ��� ���ֽ���ǰ С�˴洢 exchange()����������˫�ֽڵ����ݴ�С��ת��
2. ����д��ȥ��˫�ֽ�Ҳ���� С�˴洢
3. def_data1 ��ʾ���ﴦ���־  def_data2 ��ʾЭ���־  Ŀǰ��Ĭ�϶���0x0000 0x0000 ������
4. modbus TCP clientֻ֧�� 0x03 0x10 0x06 �����
*/


enum FUNC_CODE	//������ һ���ֽ�
{
	READ_DISPERSED_QUANTITY = 0x02, //��ȡ��ɢ��
	READ_MORE  = 0x03,	//��ȡ�������
	WRITE_MORE = 0x10,	//д����
	WRITE_ONCE = 0x06,	//д��һ��
	ECHO_TEST  = 0x08,	//�ز�����	

	/*
		������ :
		0x01 ��֧�ָù�����
		0x02 Խ��
		0x03 �Ĵ�������������Χ
		0x04 ��д����
	*/
	ERR_CODE1 = 0x01,
	ERR_CODE2 = 0x02,
	ERR_CODE3 = 0x03,
	ERR_CODE4 = 0x04,
};



/* ���Ĵ������ݵ��շ��ṹ�� */
typedef struct _rmodbusTcp_send
{
	rt_uint16_t def_data1; //Ĭ��0x00 00
	rt_uint16_t def_data2; //Ĭ��0x00 00
	rt_uint16_t len; 	   //���ݳ���
	rt_uint8_t equip_addr; //�豸��ַ
	rt_uint8_t func_mark;  //������
	rt_uint16_t sreg_addr; //��ʼ�ļĴ�����ַ
	rt_uint16_t reg_num;   //��ȡ�Ĵ�������
	// rt_uint16_t *data;
}rmtcp_send,*rmtcp_send_t;


typedef struct _rmodbusTcp_reply
{
	rt_uint16_t def_data1; //Ĭ��0x00 00
	rt_uint16_t def_data2; //Ĭ��0x00 00
	rt_uint16_t len; 	   //���ݳ���
	rt_uint8_t equip_addr; //�豸��ַ
	rt_uint8_t func_mark;  //������ 0x03
	rt_uint16_t sreg_addr; //��ʼ�ļĴ�����ַ 
	rt_uint16_t reg_num;   //��ȡ�Ĵ�������          
	// rt_uint16_t *data;
}rmtcp_reply,*rmtcp_reply_t;




/* д�����Ĵ������ݵ��շ��ṹ�� */
typedef struct _w1modbusTcp_send  
{
	rt_uint16_t def_data1; //Ĭ��0x00 00
	rt_uint16_t def_data2; //Ĭ��0x00 00
	rt_uint16_t len; 	   //���ݳ���
	rt_uint8_t equip_addr; //�豸��ַ
	rt_uint8_t func_mark;  //������
	rt_uint16_t reg_addr; //�Ĵ�����ַ
	rt_uint16_t reg_data; //�Ĵ���ֵ
	// rt_uint16_t *data;
}w1mtcp_send,*w1mtcp_send_t;


typedef struct _w1modbusTcp_reply
{
	rt_uint16_t def_data1; //Ĭ��0x00 00
	rt_uint16_t def_data2; //Ĭ��0x00 00
	rt_uint16_t len; 	   //���ݳ���
	rt_uint8_t equip_addr; //�豸��ַ
	rt_uint8_t func_mark;  //������
	rt_uint16_t reg_addr;  //�Ĵ�����ַ
	rt_uint16_t reg_data;  //�Ĵ���ֵ
	// rt_uint16_t *data;
}w1mtcp_reply,*w1mtcp_reply_t;


/* д����Ĵ������ݵ��շ��ṹ�� */
typedef struct _w2modbusTcp_send  
{
	rt_uint16_t def_data1; //Ĭ��0x00 00
	rt_uint16_t def_data2; //Ĭ��0x00 00
	rt_uint16_t len; 	   //���ݳ���
	rt_uint8_t equip_addr; //�豸��ַ
	rt_uint8_t func_mark;  //������
	rt_uint16_t sreg_addr; //д�Ĵ�����ʼ��ַ
	rt_uint16_t sreg_num;  //�Ĵ�������
	rt_uint8_t  data_len;  //д�����ݵĳ���
	// rt_uint16_t *data;
}w2mtcp_send,*w2mtcp_send_t;


typedef struct _w2modbusTcp_reply
{
	rt_uint16_t def_data1; //Ĭ��0x00 00
	rt_uint16_t def_data2; //Ĭ��0x00 00
	rt_uint16_t len; 	   //���ݳ���
	rt_uint8_t equip_addr; //�豸��ַ
	rt_uint8_t func_mark;  //������
	rt_uint16_t sreg_addr; //��ʼ�Ĵ�����ַ
	rt_uint16_t sreg_num;  //�Ĵ���д�����
	// rt_uint16_t *data;
}w2mtcp_reply,*w2mtcp_reply_t;



/* ��������������� */
typedef struct _err_reply
{
	rt_uint16_t def_data1; //Ĭ��0x00 00
	rt_uint16_t def_data2; //Ĭ��0x00 00
	rt_uint16_t len; 	   //���ݳ���
	rt_uint8_t equip_addr; //�豸��ַ
	rt_uint8_t func_mark;  //��������+0x80
	rt_uint8_t err_code;   //������
}err_reply,*err_reply_t;





/* *************************  modbus TCP server ************************* */

#define MODBUS_SERVER_PORT 		502
#define MODBUS_SERVER_IP        "192.6.6.120"
#define MODBUS_SERVER_MASK		"255.255.255.0"
#define MODBUS_SERVER_GATEWAY   "192.6.6.1"
#define LISTEN_MEM_MAX          10



/* *************************  modbus TCP client ************************* */










/*
	1. ���modbus   TCP server    ------ 2019.11.18 zxy		
*/
int modbus_tcp_server_init(void);



/*
	2. ���modbus TCP client    ------- 2020.02.26 zxy
*/
void modbus_tcp_client_init(void);
void  modbus_client_data_recv(rt_uint8_t *cbuf,rt_uint16_t clen);

#endif // __MODBUS_TCP_H__






/*
2.��ַ����˵��
(1)	DB_MACHINE ����������ַ ��Ӧ����ʼ��ַ 0x00 00 ~ 0x03 FF
(2)	DB_MOLDSET ģ�߲�����ַ ��Ӧ����ʼ��ַ 0x04 00 ~ 0x08 FF
(3)  �豸ID��ע����Ϣ������״̬��Ϣ��ַ
0x9000   Abox  ID ��2λ�ֽ�
0x9001   Abox  ID ��2λ�ֽ�
0x9002   Abox  �������ӵ�Cbox �������������50̨��
0x9003   Cbox1 ID ��2λ�ֽ�
0x9004   Cbox1 ID ��2λ�ֽ�
0x9005   Cbox2 ID ��2λ�ֽ�
0x9006   Cbox2 ID ��2λ�ֽ�
����.
0x9065   Cbox50 ID ��2λ�ֽ�
0x9066   Cbox50 ID ��2λ�ֽ�
�����޸ĵĵ�ַ ��0x9003~0x9066��
0x9002   �ĵ�ַ�洢�����ݻ����0x9003~0x9066�д洢��ID�����ᶯ̬�޸�

Abox����Cbox ID ƴ����֮�����ĸ��ֽڡ�
��λ��һ���֣�0x00 ��ʾ�豸���� ;	0x01 ��ʾ�豸���� ;
֮�������ֽ� ��ʾ�豸��mod_id ��Ӧ�豸�ı�š�

(4) ��ʱ�Ϸ�������Ϣ��ַ ���ɰ��������������չ��
��ʼ��ַ 0xA0 00  ��ֹ��ַ 0xAF FF
0xA0 00   ��ʱ�Ϸ�ʱ�䣨��λs��
0xA0 01   ��ʱ�Ϸ�ʱ�䣨��λms��
0xA0 02   ����ʱ�Ϸ������ݵĵ�ַ1 
0xA0 03   ����ʱ�Ϸ������ݵĵ�ַ2 
����
0xAF FF   ����ʱ�Ϸ������ݵĵ�ַ4094  �ݶ�֧��4093����ַ��ʱ�Ϸ�
ͨ������0xA0 00  0xA0 01 �������ö�ʱ�Ϸ�ʱ�� 
ͨ������0xA0 02 �C 0xAF FF��������Ҫ����ĵ�ַ��Ϣ


3.д���ݡ�server��ͨ��modbusЭ��д���ݣ�����Abox��������Cbox�������
�޸�Cbox1 ��ַ
0x00 0x00 0x00 0x00 0x00 0x0E 0x00 0x10 0x90 0x03 0x00 0x02 0x04 0x00 0x65 0x00 0x00
����Ԫ��ʶ�� :  0x00 0x00
Э���ʶ�� :    0x00 0x00
���ݳ���  ��    0x00 0x0B
��Ԫ��ʶ����	   0x00  //Abox��ʶ��
������    ��    0x10
��ʼ��ַ  ��    0x90 0x03
�Ĵ���������	   0x00 0x02
�����ֽ�����	   0x04
����	  	 ��   0x00 0x65 0x00 0x00 //Cbox���

��Ӧ�ظ���
���óɹ���
0x00 0x00 0x00 0x00 0x00 0x06 0x00 0x10 0x90 0x03 0x00 0x02
����Ԫ��ʶ�� :  0x00 0x00
Э���ʶ�� :    0x00 0x00
���ݳ���  ��    0x00 0x06
��Ԫ��ʶ����	   0x00  //Abox��ʶ��
������    ��    0x10
��ʼ��ַ  ��    0x90 0x03
�Ĵ���������	   0x00 0x02




4.�����ݡ�PC���·����ݶ�ȡ��Ӧ�豸�ϵĵ�ַ���ݡ�
������
PC�·�����
0x00 0x00 0x00 0x00 0x00 0x06 0x01 0x03 0x01 0x23 0x00 0x01
����Ԫ��ʶ��:0x00 0x00
Э���ʶ��: 0x00 0x00
���ݳ��ȣ�0x00 0x06
��Ԫ��ʶ��: 0x1  1��ʾCboxID_1 
������: 0x03
��ʼ��ַ��0x0123
��ȡ�����ݳ��ȣ� 0x0001

Abox��������
0x00 0x00 0x00 0x00 0x00 0x05 0x01 0x03 0x02 0x03 0xE8
����Ԫ��ʶ��:0x00 0x00
Э���ʶ��: 0x00 0x00
���ݳ��ȣ�0x00 0x05
��Ԫ��ʶ��: 0x1  1��ʾCboxID_1
������: 0x03
���ݳ��ȣ�0x02 
���ݣ�0x03 0xE8

5.�쳣�ظ����������շ��Ĺ����У��ն��豸�п��ܳ��ֵ��磬�����ߵ��������ʱPC�·����ݣ�����ܵ��쳣�ظ���
��Ӧ������ = ��������+0x80
�쳣�������������ԭ��
�쳣��	MODBUS����		��ע
01		�Ƿ��Ĺ�����		�õ�ַ����Ӧ�ù��ܣ�����д��������
02		�Ƿ������ݵ�ַ	Abox�в�������Cbox ID
03		�Ƿ�������ֵ	    ��Cbox ���ߣ��޷���Ӧ����Ϣ


ͨ��1-5 ���裬����������ͨ��modbusTcpЭ�������þ������Ϣ����ɶ�ʱ��Ϣ���󡢵������ݻ�ȡ��ACbox ���豸ע�ἰ����״̬��ѯ

6.����˵��
1)  Abox ��һ�����ӵ���������Ҫͨ��U�̵����������Ϣ��������IP port��
2)  Abox ��һ�����ӵ�����������������Ҫͨ��Э������ÿ��Abox��������ڵ�Cbox�豸��š�����ͨ��U�̵���Cbox�豸��š�
3)  PCͨ��modbus�·����ݣ�������ȡ�����������ݼ��ɡ�

*/

