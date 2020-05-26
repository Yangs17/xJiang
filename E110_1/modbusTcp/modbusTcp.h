#ifndef __MODBUS_TCP_H__
#define __MODBUS_TCP_H__


#include <rtthread.h>
#include <lwip/sockets.h> 
#include "rtconfig.h"
#include "abox.h"


/* *************************  modbus TCP  ************************* */
/*
note:
1. 所有接收到的双字节都是 高字节在前 小端存储 exchange()函数可以让双字节的数据大小端转化
2. 所有写出去的双字节也都是 小端存储
3. def_data1 表示事物处理标志  def_data2 表示协议标志  目前按默认都是0x0000 0x0000 来处理
4. modbus TCP client只支持 0x03 0x10 0x06 命令处理
*/


enum FUNC_CODE	//功能码 一个字节
{
	READ_DISPERSED_QUANTITY = 0x02, //读取离散量
	READ_MORE  = 0x03,	//读取多个变量
	WRITE_MORE = 0x10,	//写入多个
	WRITE_ONCE = 0x06,	//写入一个
	ECHO_TEST  = 0x08,	//回波测试	

	/*
		错误码 :
		0x01 不支持该功能码
		0x02 越界
		0x03 寄存器数量超出范围
		0x04 读写错误
	*/
	ERR_CODE1 = 0x01,
	ERR_CODE2 = 0x02,
	ERR_CODE3 = 0x03,
	ERR_CODE4 = 0x04,
};



/* 读寄存器数据的收发结构体 */
typedef struct _rmodbusTcp_send
{
	rt_uint16_t def_data1; //默认0x00 00
	rt_uint16_t def_data2; //默认0x00 00
	rt_uint16_t len; 	   //数据长度
	rt_uint8_t equip_addr; //设备地址
	rt_uint8_t func_mark;  //功能码
	rt_uint16_t sreg_addr; //起始的寄存器地址
	rt_uint16_t reg_num;   //读取寄存器数量
	// rt_uint16_t *data;
}rmtcp_send,*rmtcp_send_t;


typedef struct _rmodbusTcp_reply
{
	rt_uint16_t def_data1; //默认0x00 00
	rt_uint16_t def_data2; //默认0x00 00
	rt_uint16_t len; 	   //数据长度
	rt_uint8_t equip_addr; //设备地址
	rt_uint8_t func_mark;  //功能码 0x03
	rt_uint16_t sreg_addr; //起始的寄存器地址 
	rt_uint16_t reg_num;   //读取寄存器数量          
	// rt_uint16_t *data;
}rmtcp_reply,*rmtcp_reply_t;




/* 写单个寄存器数据的收发结构体 */
typedef struct _w1modbusTcp_send  
{
	rt_uint16_t def_data1; //默认0x00 00
	rt_uint16_t def_data2; //默认0x00 00
	rt_uint16_t len; 	   //数据长度
	rt_uint8_t equip_addr; //设备地址
	rt_uint8_t func_mark;  //功能码
	rt_uint16_t reg_addr; //寄存器地址
	rt_uint16_t reg_data; //寄存器值
	// rt_uint16_t *data;
}w1mtcp_send,*w1mtcp_send_t;


typedef struct _w1modbusTcp_reply
{
	rt_uint16_t def_data1; //默认0x00 00
	rt_uint16_t def_data2; //默认0x00 00
	rt_uint16_t len; 	   //数据长度
	rt_uint8_t equip_addr; //设备地址
	rt_uint8_t func_mark;  //功能码
	rt_uint16_t reg_addr;  //寄存器地址
	rt_uint16_t reg_data;  //寄存器值
	// rt_uint16_t *data;
}w1mtcp_reply,*w1mtcp_reply_t;


/* 写多个寄存器数据的收发结构体 */
typedef struct _w2modbusTcp_send  
{
	rt_uint16_t def_data1; //默认0x00 00
	rt_uint16_t def_data2; //默认0x00 00
	rt_uint16_t len; 	   //数据长度
	rt_uint8_t equip_addr; //设备地址
	rt_uint8_t func_mark;  //功能码
	rt_uint16_t sreg_addr; //写寄存器起始地址
	rt_uint16_t sreg_num;  //寄存器个数
	rt_uint8_t  data_len;  //写入数据的长度
	// rt_uint16_t *data;
}w2mtcp_send,*w2mtcp_send_t;


typedef struct _w2modbusTcp_reply
{
	rt_uint16_t def_data1; //默认0x00 00
	rt_uint16_t def_data2; //默认0x00 00
	rt_uint16_t len; 	   //数据长度
	rt_uint8_t equip_addr; //设备地址
	rt_uint8_t func_mark;  //功能码
	rt_uint16_t sreg_addr; //起始寄存器地址
	rt_uint16_t sreg_num;  //寄存器写入个数
	// rt_uint16_t *data;
}w2mtcp_reply,*w2mtcp_reply_t;



/* 错误情况返回内容 */
typedef struct _err_reply
{
	rt_uint16_t def_data1; //默认0x00 00
	rt_uint16_t def_data2; //默认0x00 00
	rt_uint16_t len; 	   //数据长度
	rt_uint8_t equip_addr; //设备地址
	rt_uint8_t func_mark;  //请求功能码+0x80
	rt_uint8_t err_code;   //错误码
}err_reply,*err_reply_t;





/* *************************  modbus TCP server ************************* */

#define MODBUS_SERVER_PORT 		502
#define MODBUS_SERVER_IP        "192.6.6.120"
#define MODBUS_SERVER_MASK		"255.255.255.0"
#define MODBUS_SERVER_GATEWAY   "192.6.6.1"
#define LISTEN_MEM_MAX          10



/* *************************  modbus TCP client ************************* */










/*
	1. 添加modbus   TCP server    ------ 2019.11.18 zxy		
*/
int modbus_tcp_server_init(void);



/*
	2. 添加modbus TCP client    ------- 2020.02.26 zxy
*/
void modbus_tcp_client_init(void);
void  modbus_client_data_recv(rt_uint8_t *cbuf,rt_uint16_t clen);

#endif // __MODBUS_TCP_H__






/*
2.地址设置说明
(1)	DB_MACHINE 机器参数地址 对应的起始地址 0x00 00 ~ 0x03 FF
(2)	DB_MOLDSET 模具参数地址 对应的起始地址 0x04 00 ~ 0x08 FF
(3)  设备ID、注册信息、在线状态信息地址
0x9000   Abox  ID 低2位字节
0x9001   Abox  ID 高2位字节
0x9002   Abox  允许连接的Cbox 数量（最多允许50台）
0x9003   Cbox1 ID 低2位字节
0x9004   Cbox1 ID 高2位字节
0x9005   Cbox2 ID 低2位字节
0x9006   Cbox2 ID 高2位字节
…….
0x9065   Cbox50 ID 低2位字节
0x9066   Cbox50 ID 高2位字节
允许修改的地址 从0x9003~0x9066。
0x9002   的地址存储的数据会根据0x9003~0x9066中存储的ID数量会动态修改

Abox或者Cbox ID 拼起来之后有四个字节。
高位第一个字：0x00 表示设备离线 ;	0x01 表示设备在线 ;
之后三个字节 表示设备的mod_id 对应设备的编号。

(4) 定时上发数据信息地址 （可按具体需求进行扩展）
起始地址 0xA0 00  终止地址 0xAF FF
0xA0 00   定时上发时间（单位s）
0xA0 01   定时上发时间（单位ms）
0xA0 02   请求定时上发的数据的地址1 
0xA0 03   请求定时上发的数据的地址2 
……
0xAF FF   请求定时上发的数据的地址4094  暂定支持4093个地址定时上发
通过设置0xA0 00  0xA0 01 可以设置定时上发时间 
通过设置0xA0 02 – 0xAF FF来设置需要请求的地址信息


3.写数据。server端通过modbus协议写数据，控制Abox允许连接Cbox的情况。
修改Cbox1 地址
0x00 0x00 0x00 0x00 0x00 0x0E 0x00 0x10 0x90 0x03 0x00 0x02 0x04 0x00 0x65 0x00 0x00
事务元标识符 :  0x00 0x00
协议标识符 :    0x00 0x00
数据长度  ：    0x00 0x0B
单元标识符：	   0x00  //Abox标识符
功能码    ：    0x10
起始地址  ：    0x90 0x03
寄存器数量：	   0x00 0x02
数据字节数：	   0x04
数据	  	 ：   0x00 0x65 0x00 0x00 //Cbox编号

响应回复：
设置成功：
0x00 0x00 0x00 0x00 0x00 0x06 0x00 0x10 0x90 0x03 0x00 0x02
事务元标识符 :  0x00 0x00
协议标识符 :    0x00 0x00
数据长度  ：    0x00 0x06
单元标识符：	   0x00  //Abox标识符
功能码    ：    0x10
起始地址  ：    0x90 0x03
寄存器数量：	   0x00 0x02




4.读数据。PC端下发数据读取对应设备上的地址数据。
举例：
PC下发数据
0x00 0x00 0x00 0x00 0x00 0x06 0x01 0x03 0x01 0x23 0x00 0x01
事务元标识符:0x00 0x00
协议标识符: 0x00 0x00
数据长度：0x00 0x06
单元标识符: 0x1  1表示CboxID_1 
功能码: 0x03
起始地址：0x0123
获取的数据长度： 0x0001

Abox返回数据
0x00 0x00 0x00 0x00 0x00 0x05 0x01 0x03 0x02 0x03 0xE8
事务元标识符:0x00 0x00
协议标识符: 0x00 0x00
数据长度：0x00 0x05
单元标识符: 0x1  1表示CboxID_1
功能码: 0x03
内容长度：0x02 
内容：0x03 0xE8

5.异常回复。在数据收发的过程中，终端设备有可能出现掉电，断离线的情况，此时PC下发数据，会接受到异常回复。
响应功能码 = 请求功能码+0x80
异常码来表明出错的原因。
异常码	MODBUS名称		备注
01		非法的功能码		该地址不响应该功能（读或写不被允许）
02		非法的数据地址	Abox中不包含该Cbox ID
03		非法的数据值	    该Cbox 掉线，无法响应此消息


通过1-5 步骤，服务器可以通过modbusTcp协议来设置具体的信息，完成定时信息请求、单次数据获取、ACbox 的设备注册及在线状态查询

6.流程说明
1)  Abox 第一次连接到服务器需要通过U盘导入服务器信息（服务器IP port）
2)  Abox 第一次连接到服务器，服务器需要通过协议配置每个Abox下允许存在的Cbox设备编号。或者通过U盘导入Cbox设备编号。
3)  PC通过modbus下发数据，解析获取到的数据内容即可。

*/

