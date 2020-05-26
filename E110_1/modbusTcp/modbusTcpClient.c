#include "modbusTcp.h"


/*
	1. ���modbus   TCP client    ------ 2020.02.26 zxy	a18667427315@163.com
	2. ��Abox USB ����cbox devID �� Э���е��豸ID���Ӧ�ı�
		ͨ��������������������յ��İ�Ҫ�����ĸ�Cbox������
*/

/* ����Ļ����������������̣߳���ֹ�̶߳�ջ��� */
#define MDBT_POOL_SIZE (2*1024)   //������2K
#define TEMP_BUFF_SIZE (1024)     //���ݴ�����1K
typedef struct _modbus_client
{
	rt_uint16_t  len;
	rt_uint8_t   tbuf[TEMP_BUFF_SIZE];
	rt_uint8_t 	 dbuf[MDBT_POOL_SIZE];
}mdbt_client;
mdbt_client mcli;



//ע��: �жϵ�ʱ�����ݵĴ�С�˿�����Ҫת�� 
/* modbus: ���ֽ� ���ֽ���ǰ ���ֽ��ں�  */
rt_uint16_t exChange(rt_uint16_t data)
{
	return (data<<8)|(data>>8);
}


/* ��ʼ�� modbus tcp client  */
void modbus_tcp_client_init(void)
{
	mcli.len = 0;
	rt_memset(mcli.dbuf,0,MDBT_POOL_SIZE);
	rt_memset(mcli.tbuf,0,TEMP_BUFF_SIZE);
}



void check_read_reg()
{

}


/* 	
	�˲�buf �����һ�������Ƿ��Ƿ���modbus TCP Ҫ�������
	��Σ�  	�˲��buf
	����ֵ��
	0xffff 	�˶�ʧ��
	>0      �˶Ե����ݵĳ���
	0       ���ݲ���
*/
rt_uint16_t check_modbus_tcp_data(rt_uint8_t *cbuf,rt_uint16_t clen)
{
	if(!cbuf) return 0;

	rmtcp_send_t tbuf = (rmtcp_send_t)cbuf;
	rt_uint16_t ret = 0xffff;

	if(clen > (MDBT_POOL_SIZE - 500)) return ret; //��ֹ��������̫�ർ��pool����
	
	if(tbuf->def_data1)  || (tbuf->def_data2)) return ret;  //Ĭ�� 0x0000

	if((READ_MORE != tbuf->func_mark)      //ֻ֧�������ֹ�����
		&& (WRITE_MORE != tbuf->func_mark)
		&& (WRITE_ONCE != tbuf->func_mark))
		return ret;

	rt_uint16_t buf_len = tbuf->len + sizeof(tbuf->def_data1) + sizeof(tbuf->def_data2)+sizeof(tbuf->len);
	if(buf_len > TEMP_BUFF_SIZE) return ret;
	
	if(buf_len > clen) return 0; // ���ݲ���
	else
		return buf_len; 

}

/*
����ֵ��
0xffff	�˶�ʧ��
>0		�˶Ե����ݵĳ���
0		���ݲ���
*/
void package_server_data(rt_uint8_t *)
{
	rmtcp_send_t tbuf = (rmtcp_send_t)cbuf;
	rt_uint16_t ret = 0xffff;
	stPackEstCmd estcmd;

	switch(tbuf->func_mark)
	{
		case READ_DISPERSED_QUANTITY:	//��ȡ��ɢ��

		break;
		
		case READ_MORE: //��ȡ�������  ----���󵥴��·�
		//��С��ת��
		rt_uint16_t start_addr = exChange(tbuf->sreg_addr);
		rt_uint16_t reg_num = exChange(tbuf->reg_num);

		//estcmd package
		//estcmd.id  = ((start_addr>>16) & 0xfff0);
		estcmd.id = 0;
		estcmd.dev_id
		estcmd.cmd = CMD_103;

		
		//0x103 package
		uint16_t sub_ind = (((start_addr>>16)&0x000f)<<12) | (length&0x0fff);
		uint16_t offset = start_addr &0x0000ffff;
					
		break;
		
		case WRITE_MORE:	//д����
			w2mtcp_send_t

		break;
		
		case WRITE_ONCE:	//д��һ��
			w1mtcp_send_t

		break;
		
		case ECHO_TEST: 	//�ز�����

		break;

		default:
			break;
			
	}

	//pack 0x124 head
	if((ret > 0) && (ret < 0xffff))
	{
		stPackCmd headpack;
		headpack.soh = SSOH;
		headpack.cmd = TCMD_102;
		headpack.id
	}

	return ret;

}


/* 
	Э��ת���������յ��ķ���������ת����modbus TCP Э��.
	ÿ�ε��þͻ�ѻ���������ĵ�һ����������תΪ modbus TCP Э��İ����ݸ��������
	
	���1�� ���յ��ķ��������ݵ�ַ
	���2�� ���յ������ݳ��ȵ�ַ
*/
void  modbus_client_data_recv(rt_uint8_t *cbuf,rt_uint16_t *clen)
{
	if(!cbuf || !clen) return;

	if((*clen + mcli.len) > MDBT_POOL_SIZE) break;

	/* copy data */
	rt_memcpy(mcli.dbuf+mcli.len,cbuf,*clen);
	mcli.len += (*clen);

	rt_memset(cbuf,0,BUFF_LEN);
	*clen = 0;

	rt_uint16_t ret;
	ret == check_modbus_tcp_data(mcli.dbuf,mcli.len);
	switch(ret)
	{
		case 0xffff:
			/* ���ݳ��� ��ջ���� */
			rt_memset(mcli.dbuf,0,MDBT_POOL_SIZE);
			mcli.len = 0;
		break;

		case 0:
			/* ���ݲ���,�����ȴ����� */
		break;

		default:
			/* ������һ������ */
			package_server_data()
		break;
	}
	

}






