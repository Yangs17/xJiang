#include "modbusTcp.h"


/*
	1. 添加modbus   TCP client    ------ 2020.02.26 zxy	a18667427315@163.com
	2. 从Abox USB 导入cbox devID 与 协议中的设备ID相对应的表，
		通过解析这个表来分析接收到的包要请求哪个Cbox的数据
*/

/* 申请的缓冲区尽量不依靠线程，防止线程堆栈溢出 */
#define MDBT_POOL_SIZE (2*1024)   //缓冲区2K
#define TEMP_BUFF_SIZE (1024)     //数据处理区1K
typedef struct _modbus_client
{
	rt_uint16_t  len;
	rt_uint8_t   tbuf[TEMP_BUFF_SIZE];
	rt_uint8_t 	 dbuf[MDBT_POOL_SIZE];
}mdbt_client;
mdbt_client mcli;



//注意: 判断的时候数据的大小端可能需要转换 
/* modbus: 两字节 高字节在前 低字节在后  */
rt_uint16_t exChange(rt_uint16_t data)
{
	return (data<<8)|(data>>8);
}


/* 初始化 modbus tcp client  */
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
	核查buf 里面第一组数据是否是符合modbus TCP 要求的数据
	入参：  	核查的buf
	返回值：
	0xffff 	核对失败
	>0      核对的数据的长度
	0       数据不够
*/
rt_uint16_t check_modbus_tcp_data(rt_uint8_t *cbuf,rt_uint16_t clen)
{
	if(!cbuf) return 0;

	rmtcp_send_t tbuf = (rmtcp_send_t)cbuf;
	rt_uint16_t ret = 0xffff;

	if(clen > (MDBT_POOL_SIZE - 500)) return ret; //防止错误数据太多导致pool满了
	
	if(tbuf->def_data1)  || (tbuf->def_data2)) return ret;  //默认 0x0000

	if((READ_MORE != tbuf->func_mark)      //只支持这三种功能码
		&& (WRITE_MORE != tbuf->func_mark)
		&& (WRITE_ONCE != tbuf->func_mark))
		return ret;

	rt_uint16_t buf_len = tbuf->len + sizeof(tbuf->def_data1) + sizeof(tbuf->def_data2)+sizeof(tbuf->len);
	if(buf_len > TEMP_BUFF_SIZE) return ret;
	
	if(buf_len > clen) return 0; // 数据不足
	else
		return buf_len; 

}

/*
返回值：
0xffff	核对失败
>0		核对的数据的长度
0		数据不够
*/
void package_server_data(rt_uint8_t *)
{
	rmtcp_send_t tbuf = (rmtcp_send_t)cbuf;
	rt_uint16_t ret = 0xffff;
	stPackEstCmd estcmd;

	switch(tbuf->func_mark)
	{
		case READ_DISPERSED_QUANTITY:	//读取离散量

		break;
		
		case READ_MORE: //读取多个变量  ----请求单次下发
		//大小端转换
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
		
		case WRITE_MORE:	//写入多个
			w2mtcp_send_t

		break;
		
		case WRITE_ONCE:	//写入一个
			w1mtcp_send_t

		break;
		
		case ECHO_TEST: 	//回波测试

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
	协议转换：将接收到的服务器数据转换成modbus TCP 协议.
	每次调用就会把缓冲区里面的第一个完整数据转为 modbus TCP 协议的包传递给两个入参
	
	入参1： 接收到的服务器数据地址
	入参2： 接收到的数据长度地址
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
			/* 数据出错 清空缓冲池 */
			rt_memset(mcli.dbuf,0,MDBT_POOL_SIZE);
			mcli.len = 0;
		break;

		case 0:
			/* 数据不足,继续等待接收 */
		break;

		default:
			/* 有完整一包数据 */
			package_server_data()
		break;
	}
	

}






