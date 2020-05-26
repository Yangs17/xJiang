/*
 * File      : canapp.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author            Notes
 * 2015-05-14     aubrcool@qq.com   first version
 */

#include <board.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "ctrl.h"
#include "mydef.h"
#include "dictionary.h"

#ifdef RT_USING_CAN

rt_uint8_t        Can_Rev_Buff [BUFF_LEN];
rt_uint8_t        Can_Send_Buff[BUFF_LEN];
rt_uint8_t        Can_Buff[BUFF_LEN];

struct can_app_struct
{
    const char                   *name;
    struct rt_can_filter_config  *filter;
    rt_uint8_t                   eventopt;
    struct rt_event              event;	
};
static struct can_app_struct can_data[2];
static rt_err_t  can1ind(rt_device_t dev,  void *args, rt_int32_t hdr, rt_size_t size)
{
    rt_event_t pevent = (rt_event_t)args;
    rt_event_send(pevent, 1 << (hdr));
    return RT_EOK;
}
static rt_err_t  can2ind(rt_device_t dev, void *args, rt_int32_t hdr, rt_size_t size)
{
    rt_event_t pevent = (rt_event_t)args;	
    rt_event_send(pevent, 1 << (hdr));
    return RT_EOK;
}
struct rt_can_filter_item filter1item[4] =
{
    RT_CAN_FILTER_STD_INIT(0x0451, can1ind, &can_data[0].event),
    RT_CAN_FILTER_STD_INIT(2, can1ind, &can_data[0].event),
    RT_CAN_STD_RMT_FILTER_INIT(3, can1ind, &can_data[0].event),
    RT_CAN_STD_RMT_DATA_FILTER_INIT(4, can1ind, &can_data[0].event),
};
struct rt_can_filter_item filter2item[4] =
{
    RT_CAN_FILTER_STD_INIT(0x0451, can2ind, &can_data[1].event),
    RT_CAN_FILTER_STD_INIT(2, can2ind, &can_data[1].event),
    RT_CAN_STD_RMT_FILTER_INIT(3, can2ind, &can_data[1].event),
    RT_CAN_STD_RMT_DATA_FILTER_INIT(4, can2ind, &can_data[1].event),
};
struct rt_can_filter_config filter1 =
{
    4,
    1,
    filter1item,
};
struct rt_can_filter_config filter2 =
{
    4,
    1,
    filter2item,
};
static struct can_app_struct can_data[2] =
{
    {
        "bxcan1",
        &filter1,
        RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
    },
    {
        "bxcan2",
        &filter2,
        RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
    },
};




void  Pack_Send(rt_device_t dev, rt_uint8_t *buff,  rt_uint16_t len)// len ,buff ---cmd + id + data
{
	  rt_uint16_t           i, j, x, crc = 0;
    struct rt_can_msg     msg;	
    struct can_pack_head  head;	
	
	stPackEstCmd estcmd;
    rt_memcpy(&estcmd,buff,sizeof(estcmd));
//	at_printf("send can_cmd is  %x\r\n",estcmd.cmd);


	  OutputLed(LEDON, LED_CAN);	
	  
	  msg.ide  = RT_CAN_STDID;	  msg.rsv  = 0;	     msg.rtr   = RT_CAN_DTR;			
		msg.id   = 0x409;           msg.len  = 8;	
		
	  head.sot = 0xEB;            head.ver = 0x01;   head.len  = len + (sizeof(head)-4) + 2;

	  

	if(sizeof(Can_Send_Buff) < head.len)
		return;
	
	  rt_memcpy(Can_Send_Buff, &head, sizeof(head));
	
    rt_memcpy(Can_Send_Buff + sizeof(head)-4, buff, len);	
	
//	at_printf("_Can_send_Ok  data : %d \r\n",num);
	  crc = CcittCrc16(Can_Send_Buff, head.len - 2);
		Can_Send_Buff[head.len - 2] = (crc & 0xff);
		Can_Send_Buff[head.len - 1] = (crc >> 8) & 0xff;	

/*
		if(estcmd.cmd == 0x124)
		{
			at_printf("send can 0x124!  len = %d\r\n",len);
		}
*/

		
//at_printf("crc = %x len = %d\n", crc, head.len);	
	  j = head.len % 6;
	  x = head.len / 6 * 6;
	  
	  dic2_data.panel_link = Panel_disconnect;
	  dic2_data.can_op_con = CAN_OP_ERR; 

	  msg.data[0] = 0x51;  msg.data[1] = 0x00;
    for (i = 6; i <= x; i += 6)	
	  {
	      rt_memcpy(msg.data + 2, Can_Send_Buff + i - 6, 6);
			rt_device_write(dev, 0, &msg, sizeof(msg));

		}
		msg.len = j + 2;
//at_printf("l = %d\n", i - 6);		
		if (j > 0)
		{		
	      rt_memcpy(msg.data + 2, Can_Send_Buff + i - 6, j);
        rt_device_write(dev, 0, &msg, sizeof(msg));
	
		}
		dic2_data.panel_link = Panel_con_can;
		dic2_data.can_op_con = CAN_OP_OK<<8 | CAN_OP_ERR;
	  OutputLed(LEDOFF, LED_CAN);
//	  can_tx_tick = rt_tick_get();
}




void rt_can_thread_entry(void *parameter)
{
    struct rt_can_msg msg;
	
    struct can_app_struct  *canpara = (struct can_app_struct *) parameter;
    rt_uint32_t            e, len = 0, offset = 0;
    rt_device_t            candev;

    candev = rt_device_find(canpara->name);
    RT_ASSERT(candev);
    rt_event_init(&canpara->event, canpara->name, RT_IPC_FLAG_FIFO);
    rt_device_open(candev, (RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_INT_TX));
    rt_device_control(candev, RT_CAN_CMD_SET_FILTER, canpara->filter);

	
    while (1)
    {	
	
        if (rt_event_recv(&canpara->event,
						((1 << canpara->filter->items[0].hdr) |
						 (1 << canpara->filter->items[1].hdr) |
						 (1 << canpara->filter->items[2].hdr) |
						 (1 << canpara->filter->items[3].hdr)), 
						 canpara->eventopt, 
						 RT_WAITING_FOREVER, &e) != RT_EOK)
        { 					
            continue;
        }	

		dic2_data.panel_link = Panel_con_can;
						 
        if (e & (1 << canpara->filter->items[0].hdr))
        {
		 
            msg.hdr = canpara->filter->items[0].hdr;
            while (rt_device_read(candev, 0, &msg, sizeof(msg)) == sizeof(msg))
            {	
            	dic2_data.can_revnum++;
            	dic2_data.can_op_con = CAN_OP_OK << 8 | CAN_OP_OK;	
							
				if (msg.data[0] == 0x09 && msg.data[1] == 0x00)
				{	
					if (msg.data[2] == 0xEB && msg.data[3] == 0x01)
					{
						if(((msg.data[5] << 8) | msg.data[4]) < (BUFF_LEN))
						{
							len  = (msg.data[5] << 8) | msg.data[4];
							offset = 0;
							rt_memset(Can_Rev_Buff, 0, BUFF_LEN);
					
						}
						
						else
						{
							len  = (msg.data[5] << 8) | msg.data[4];
							offset = 0;
							rt_memset(Can_Rev_Buff, 0, BUFF_LEN);
							at_printf("context eb01\r\n");
						}
						
					}

					
					if(msg.len > 2 && len > 0)
					{
						rt_memcpy(Can_Rev_Buff+offset, msg.data + 2, msg.len - 2);
						offset += msg.len - 2;
					}
						
														 
					if (offset >= len && len > 0)
                     {                     

						if (CcittCrc16(Can_Rev_Buff, len) == 0)
						{		
						//	at_printf("can recv buff\r\n");
						//	mq_send(&mq_arry[PTOE_MQ],(char *)Can_Rev_Buff, len, NORMAL_EVENT);
							
						}

                        len = 0;	
						offset = 0;										
					 }											 
				}								 
            }

			
        }







		
        if (e & (1 << canpara->filter->items[1].hdr))
        {
at_printf("rev hdr 1\n");					
            msg.hdr = canpara->filter->items[1].hdr;
            while (rt_device_read(candev, 0, &msg, sizeof(msg)) == sizeof(msg))
            {
                rt_device_write(candev, 0, &msg, sizeof(msg));
            }
        }
        if (e & (1 << canpara->filter->items[2].hdr))
        {
at_printf("rev hdr 2\n");						
            msg.hdr = canpara->filter->items[2].hdr;
            while (rt_device_read(candev, 0, &msg, sizeof(msg)) == sizeof(msg))
            {
                rt_device_write(candev, 0, &msg, sizeof(msg));
            }
        }
        if (e & (1 << canpara->filter->items[3].hdr)) //回环模式
        {
at_printf("rev hdr 3\n");						
            msg.hdr = canpara->filter->items[3].hdr;
            while (rt_device_read(candev, 0, &msg, sizeof(msg)) == sizeof(msg))
            {
                rt_device_write(candev, 0, &msg, sizeof(msg));
            }
        }	

		
    }
}




void rt_can_send_thread_entry(void *parameter)
{

     struct can_app_struct *canpara = (struct can_app_struct *) parameter;	
     rt_device_t           candev;	
	 rt_uint16_t     slen;
	 char sign1 ;
	 char sign2 ;
	 	 	 
	
     rt_thread_delay(RT_TICK_PER_SECOND);
     candev = rt_device_find(canpara->name);	 

	
	  while(1)
	  {
	  		rt_memset(Can_Buff,0,sizeof(Can_Buff));
			rt_memset(Can_Send_Buff,0,sizeof(Can_Send_Buff));
			slen = 0;
			
	  		if( RT_EOK != rt_mq_recv(&mq_arry[ETOP_CAN_MQ],Can_Buff, sizeof(Can_Buff),RT_WAITING_FOREVER ))
				continue; 

			//If it's heartbeat information.
			if ( !rt_strcmp((char *)Can_Buff, PANEL_HEART))
       		{	
				   rt_memset(Can_Buff,0,sizeof(Can_Buff));
				   
				   //little-end storage pattern
				   cmd_162_info.estpack.cmd = CMD_162;
				   cmd_162_info.estpack.id = 0;

				   cmd_162_info.status = Cdma_Info.link;
				   cmd_162_info.signal = Cdma_Info.rssi;
				   cmd_162_info.mod_sn = Sys_Setup.app.id;
				   cmd_162_info.mod_pVer = APPF_VERSION * 100 + APPF_SUBVERSION * 10 + APPF_REVISION;

				   sign1 = Cdma_Info.rssi & 0x00FF;
				   if(sign1 < SIGNAL_NOR)
					   sign2 = 1;
				   else
					   sign2 = 0;
				   cmd_162_info.sign_op = ((sign2 << 8) | sign1);

				   {
					 //  dic2_data.panel_link = Panel_disconnect;
					 //  dic2_data.can_op_con = CAN_OP_ERR;					   
					   Pack_Send(candev, (rt_uint8_t *)(&cmd_162_info), sizeof(cmd_162_info));

				   }


			}
			else
			{
				rt_memcpy(&slen,Can_Buff,sizeof(slen));
				data_left(Can_Buff,sizeof(slen),sizeof(Can_Buff),BUFF_LEN);
				
				Pack_Send(candev,Can_Buff, slen);
           	}	

			dic2_data.can_sendnum++;

	  }
}




int rt_can_app_init(void)
{
    rt_thread_t tid;

 //   rt_event_init(&del_event, "del_data", RT_IPC_FLAG_FIFO);
  
	
#ifdef USING_BXCAN1	
    tid = rt_thread_create("can1rev",
                           rt_can_thread_entry, &can_data[0],
                           512, 0x11, 20);



    if (tid != RT_NULL) rt_thread_startup(tid);
	
    tid = rt_thread_create("can1send",
                           rt_can_send_thread_entry, &can_data[0],
                           512, 0x0d, 20);


    if (tid != RT_NULL) rt_thread_startup(tid);
#endif

#ifdef USING_BXCAN2
    tid = rt_thread_create("can2rev",
                           rt_can_thread_entry, &can_data[1],
                           512, 16, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);
	
//		rt_sem_init(&send_can2_sem, "send_can2", 0, 0);			
    tid = rt_thread_create("can2send",
                           rt_can_send_thread_entry, &can_data[1],
                           512, 20, 20);
    if (tid != RT_NULL) rt_thread_startup(tid);	
#endif

    return 0;
}

INIT_APP_EXPORT(rt_can_app_init);

#endif /*RT_USING_CAN*/

