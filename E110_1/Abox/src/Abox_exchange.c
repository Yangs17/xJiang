#include "abox.h"

#define ABOXTIMEOUT (5*RT_TICK_PER_SECOND)  //5 seconds at a time
#define SERTIMEOUT  (READ_CLI_TIMEOUT_S)*1000 //超时设置



	/* get data */
#ifdef VAR_MQ	
	//var_mq_t aboxMqInfo[ABOX_MQ_NUM];
#else
	Message_Queue_Info aboxMqInfo[ABOX_MQ_NUM];
#endif	







/**
 * This function will show the existing Cbox information in Abox.
 *
 * @param void
 * @return void
 *
 */	
void listReport()
{
	at_print_open();

	rt_uint8_t index = 0;
	cbox_assessment_report cboxReportTemp;
	at_printf("\r\n/******** listReport ***********/\r\n");
	for(index = 0;index<aboxInfo.cboxTotalInfo.cboxOnlineNum;index++)
	{
		cboxReportTemp = aboxInfo.cboxTotalInfo.cboxInfo[index].cboxAssessReport;
	    at_printf("boxID : %d\r\n",aboxInfo.cboxTotalInfo.cboxInfo[index].devidCollection[0]&0xffffff);
		at_printf("send packNum: %d\r\n",cboxReportTemp.packTotal);
		at_printf("err  packNum: %d\r\n",cboxReportTemp.packErr);
		at_printf("\r\n");
	}
	at_printf("\r\n/******** end ***********/\r\n");
	
	at_print_close();


}


/**
 * This function will Process five yards of data for Abox.
 *
 * @param send        : the buff will be send.
 * @param len         : the length of valid data in the send buffer.
 * @param event_type  : the urgency of the message 
 *
 * @return void
 *
 */	
int Abox_send_pack(char *send, rt_uint16_t len,int eventType)//len = packcmd + data length
{
	if(RT_NULL == send) 	return RT_ERROR;

	rt_uint16_t 			crc;
							
	crc = ModbusCrc16((rt_uint8_t *)send, len);
	send[len + 0] =  crc & 0xff;
	send[len + 1] = (crc & 0xff00) >> 8;				
	crc = len + 2;    
	sendDataToServer(SEND_DATA,(rt_uint8_t *)send,crc);
	
	return RT_EOK;			
}





/* @return acceptable length */
rt_uint32_t requireServerPoolAcceptLength(void)
{
	return (aboxInfo.cboxDataPool->poolSize - aboxInfo.cboxDataPool->entrySize);
}




/**
 * This function will process data from server.
 *
 * @param addDtaInfo_t recving information.
 *
 * @return RT_EOK
 *
 */
rt_err_t recvServerData(rt_uint8_t *recv)
{
	if(!recv) return RT_EOK;	
	if(!aboxInfo.cboxTotalInfo.cboxOnlineNum) return RT_EOK;
	
	stPackCmd *packcmd= (stPackCmd	*)recv; 
	rt_uint32_t i = 0;
	rt_uint8_t  entryNum  = 0;
	uint16_t recvLen = sizeof(stPackCmd)+packcmd->len+2;

	at_printf("recvLen = %d \r\n",recvLen);

	for(i=0; i<aboxInfo.cboxTotalInfo.cboxOnlineNum; i++)
	{
		/* check modid */
		if(packcmd->id == (aboxInfo.cboxTotalInfo.cboxInfo[i].devidCollection[CBOX_DEVID] & 0x00ffffff) )	
		{
			entryNum = aboxInfo.cboxTotalInfo.cboxInfo[i].cboxDataInfo.entryNum;
		
			/* check data number allow */
			if(entryNum < CBOX_MAX_DATA_LIST)
			{
			
				/* check save ok */
				aboxInfo.cboxTotalInfo.cboxInfo[i].cboxDataInfo.cboxDataList[entryNum].dataAddr 
					= WritePool(aboxInfo.cboxDataPool,recv,recvLen);

				if(aboxInfo.cboxTotalInfo.cboxInfo[i].cboxDataInfo.cboxDataList[entryNum].dataAddr >= 0)
				{					
					aboxInfo.cboxTotalInfo.cboxInfo[i].cboxDataInfo.cboxDataList[entryNum].dataLength = recvLen;
					aboxInfo.cboxTotalInfo.cboxInfo[i].cboxDataInfo.entryNum++;
				}
				else
				{
					at_printf("get data is err!\r\n");
					//aboxInfo.cboxTotalInfo.cboxInfo[i].cboxDataInfo.cboxDataList[entryNum-1].dataAddr = 0;	
					return RT_ERROR;
				}

			}
			break;
		}
		

	}

	return RT_EOK;

}



/**
 * Receive server registration information for Abox.
 *
 * @param dev_type : current device type.		
 * @param rev : the received buff 
 *
 * @return void
 *
 */	
void Abox_DEV_USER_GET_SSTATUS(char *rev)    //cmd 19
{
	if(!rev) return;
	
	TCMD_19_Info cmd_19_info;
	rt_memcpy(&cmd_19_info, rev + sizeof(stPackCmd), sizeof(cmd_19_info));


	/* panel registration information */

	switch(cmd_19_info.status)
	{
		case 0:  //no
			Cdma_Info.link = CONNECT_AT;
			break;
		case 1: //yes
			Cdma_Info.link = CONNECT_SERVER;

		    /* 每次发送成功之后，都会将记录上传 */
			send_log_diagnosis();
		
			break;


		default:
			break;
	}
		
    at_printf("19  cmd  is get！cmd_162_info.sreg  = %d  !\r\n",cmd_19_info.status );	



}






/**
 * this function will package a heartbeat for server.
 *
 * @param sendbuf: send buff
 *
 * @return void
 *
 */ 
void pack_ser_heart(rt_uint8_t *sendbuf)
{
	rt_uint16_t crc;
	
	if(RT_NULL != sendbuf)
	{
		rt_memset(sendbuf,0,BUFF_LEN);
		
		stPackCmd packcmd;
		rt_uint8_t i = 0;

		/* packcmd */
		packcmd.soh = CSOH;	 
		packcmd.cmd   = TCMD_1;
		packcmd.id		= Sys_Setup.app.id; 
		packcmd.len = aboxInfo.cboxTotalInfo.cboxOnlineNum* 4;
		rt_memcpy(sendbuf,&packcmd,sizeof(packcmd));

		cbox_information  *cboxInfo_t = &aboxInfo.cboxTotalInfo.cboxInfo[0];

		/* data copy */
		for(i=0;i<aboxInfo.cboxTotalInfo.cboxOnlineNum;i++)
		{
			//at_printf("cbox[%d]= %d\r\n",i,cboxInfo_t->devidCollection[CBOX_DEVID]&0xffffff);
			rt_memcpy(sendbuf+sizeof(packcmd)+4*i,&cboxInfo_t->devidCollection[CBOX_DEVID],4);	
			cboxInfo_t++;
		}
	
		crc = ModbusCrc16(sendbuf, sizeof(stPackCmd)+packcmd.len);
		rt_memcpy(sendbuf+sizeof(stPackCmd)+packcmd.len,&crc,2);


		/* copy heart data */					
		sendDataToServer(SEND_DATA, sendbuf,sizeof(stPackCmd)+packcmd.len+2);
	}

}



/**
 * Send registration information.
 *
 * @param void
 *
 * @return void
 *
 */


/**
 * Send registration information.
 *
 * @param void
 *
 * @return void
 *
 */
rt_uint8_t serHeart[BUFF_LEN];
static void ser_reg()
{
	//if(Cdma_Info.link == CONNECT_AT)
	{
		at_printf("send reg!\r\n");
		sub_PackRegCmd(serHeart);	
	}
}

void ser_heart()
{
	//if(Cdma_Info.link == CONNECT_SERVER)
	{
			at_printf("send heart!\r\n");
			pack_ser_heart(serHeart);
	}

}


/**
 * it is used to process data from Abox exchange.
 * Receive data from the server (Ethernet), process data from Cbox (sub-g).
 *
 * @param void
 *
 * @return void
 *
 */
void Abox_thread_entry(void* parameter)
{
	static rt_uint8_t  ex_buf[BUFF_LEN];
	static rt_uint32_t err_times = 0;
	
	while(1)
	{
		rt_memset(ex_buf,0,sizeof(ex_buf));
			
		if(RT_EOK == mq_recv(A_STOA_MQ,ex_buf, sizeof(ex_buf),ABOXTIMEOUT))
		{
			//at_printf("recv serpack \r\n");
			cdma_rev_pack(ex_buf,A_STOA_MQ);
		}
		else
		{
			if(Cdma_Info.serHeartTime)
			{
				if(Cdma_Info.serHeartTime > rt_tick_get())
				{
					Cdma_Info.serHeartTime = rt_tick_get();
					continue;
				}

				if((rt_tick_get() - Cdma_Info.serHeartTime) > SERTIMEOUT)
				{
					quitTcpConnect();
					rt_thread_delay(1000);
					cmd_reboot();
				}
			}
#if 1		//2020.01.14  --- 金华设备连接不上	
			else  
			{
				err_times ++; //一次 5s ABOXTIMEOUT
				if(err_times > 12*20) //分钟 *分钟数 --- 20分钟无数据自动重启
				{
					quitTcpConnect();
					rt_thread_delay(1000);
					cmd_reboot();
				}
			}
#endif		
		
		}
		


#if 0   //防止wifi设备因为 出现网络故障而阻塞在tcp recv 中
		if(timeout_err ++ > 800) //800 * 5 s 无数据 则重启
				{
					quitTcpConnect();
					rt_thread_delay(1000);
					cmd_reboot();
				}

		if(	(Cdma_Info.serHeartTime > rt_tick_get()) 
			|| (AT_SER_UPDATA == Cdma_Info.AT_status)
		    || (AT_YMODEM_UPDATA == Cdma_Info.AT_status)
			|| (Panel_con_eth != dicInfo.secondDicInfo.panelLink)
			|| (CONNECT_AT == Cdma_Info.link)
			|| (CLIENT_WIFI_CONNECT != returnClientCurrentLinkStyle()))
		{
			Cdma_Info.serHeartTime = rt_tick_get();
		}
		else
		{
			/* 当出现需要重启lwip的时候           需要重启设备再次开启lwip */
			if(  ((Panel_con_eth == dicInfo.secondDicInfo.panelLink)
					|| (Cdma_Info.link == CONNECT_SERVER))
				&& ((rt_tick_get()-Cdma_Info.serHeartTime) > SERTIMEOUT))
			{
				Cdma_Info.serHeartTime = rt_tick_get();
				rt_kprintf("server heart timeout!err = %d \r\n");

				quitTcpConnect();
				{
					rt_thread_delay(1000);
					cmd_reboot();
				}
			}

			continue;
		}
#endif		

	}	

}



void Led_box_thread_entry(void* parameter)
{
	int i = 0;
	while(1)
	{
		rt_thread_delay(1000);
		if(!aboxInfo.cboxTotalInfo.cboxOnlineNum)
		{
				OutputLed(LEDON, LED_BOX);	
		}
		else
		{
			for(i=0;i<aboxInfo.cboxTotalInfo.cboxOnlineNum;i++)
			{
				OutputLed(LEDON, LED_BOX);
				rt_thread_delay(200);
				OutputLed(LEDOFF,LED_BOX);
				rt_thread_delay(200);
			}
		}
	}
	
}

void Led_roof_thread_entry(void* parameter)
{
	while(1)
	{
		if(!(aboxInfo.packageInfo.packageSendNum % 200))
		{
			OutputLed(LEDON, LED_ROOF);
			rt_thread_delay(100);
		}
		else
		{
			OutputLed(LEDOFF, LED_ROOF);
			rt_thread_delay(5);
		}
			
	}
	
}

static void heart_timeout(void* parameter)
{
	switch(Cdma_Info.link)
	{
		case  CONNECT_AT:
			ser_reg();
		break;

		case CONNECT_SERVER:
			ser_heart();
		break;

		default:
			break;
	}
	
	
}




/**
 * this function will initialize the main function, which is used primarily 
 * for the data interaction between the panel and the server,and it works for Abox's program.
 *
 * @param void
 *
 * @return void
 *
 */
void exchange_Abox_init(void)
{
	//	  rt_err_t  	init_thread;
	  rt_thread_t exchange_thread; //

	  exchange_thread = rt_thread_create("Abox_ex",
							  Abox_thread_entry, RT_NULL,
							  1024*3,AOX_EX_PRI, 20);


	  if (exchange_thread != RT_NULL)
      rt_thread_startup(exchange_thread);


	  rt_thread_t ledbox_thread; //

	  ledbox_thread = rt_thread_create("ledbox_ex1",
							  Led_box_thread_entry, RT_NULL,
							  256,LED_SET_PRI, 20);


	  if (ledbox_thread != RT_NULL)
      rt_thread_startup(ledbox_thread);

	  rt_thread_t ledRoof_thread; //
	  ledRoof_thread = rt_thread_create("led_ex2",
							  Led_roof_thread_entry, RT_NULL,
							  256,LED_SET_PRI, 20);


	  if (ledRoof_thread != RT_NULL)
      rt_thread_startup(ledRoof_thread );

	 
	 rt_timer_t	heart_timer = rt_timer_create("heart_timer", /* 定时器名字 */
								heart_timeout, /* 超时时回调的处理函数*/
								RT_NULL, /* 超时函数的入口参数*/
								10*1000,//100, /* 定时长度为10*1000个OS Tick */
								RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_HARD_TIMER
								); /* 周期定时     软件定时器*/
	 /* 启动定时器*/
	 if(heart_timer != RT_NULL) rt_timer_start(heart_timer);

//      return 0;			
		
}



