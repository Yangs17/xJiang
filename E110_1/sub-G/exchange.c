#include <rtthread.h>
#include "board.h"
#include "ctrl.h"
#include "rtdevice.h"
#include "at.h"
#include "stm32f4_rtc.h"
#include <dfs_posix.h>
#include "adc.h"
#include "rthw.h"
#include "rtdef.h"
#include "usart.h"
#include "mydef.h"
#include "dictionary.h"
#include "rtconfig.h"
#include "sub_G.h"

#include "abox.h"

CMD_162_Info cmd_162_info; 
struct rt_event exchange_event; 
//#define URL_SERVER0_NET  "yun.esto.cn"
#define URL_SERVER0_NET  "iot.esto.cn"
#define URL_SERVER0_PORT "19991"

//#define URL_SERVER0_NET  "192.168.0.13"   //直连局域网
//#define URL_SERVER0_NET  "25.25.25.6"
//#define URL_SERVER0_PORT "19991"



#ifdef RT_USING_RTC
#include "stm32f4_rtc.h"
#endif
static int at_print_off = 0;
void at_printf(const char *format, ...)
{
	va_list args;
    
	va_start(args, format);
		
	if(!at_print_off)
		rt_kprintf(format);
		
	va_end(args);
}



void at_print_ctrl(int off)
{
	at_print_off = off;
}

void at_print_close()
{
	at_print_ctrl(1);
}

void at_print_open()
{
	at_print_ctrl(0);
}


/**
 * This function will Send a message to a message queue。
 *
 * @param mq_type : the queue will receive.
 * @param send : the buff will be sent.
 * @param len : the length of the send_buff.
 *
 * @return void
 *
 */
void mq_send(int mq_type,void *send,rt_uint32_t len,int eventType)
{

	/* Judgment information. */ 
	if(mq_type >= ABOX_MQ_NUM) return;

#ifdef VAR_MQ	
	var_mq_t mq = aboxMqInfo[mq_type];
	if(sendMsg(mq,send,len) != RT_EOK)
	{
		at_printf("\r\n mq_send is error!mq= %d\r\n",mq_type);		
		return;
	}

#else
	rt_mq_t mq =  aboxMqInfo[mq_type].mq;	
    if(rt_mq_send(mq,send,len) != RT_EOK)
	{
		at_printf("\r\n mq_send is error!mq= %d\r\n",mq_type);
		OutputLed(LEDON, LED_BOX);
		return;
	}
#endif	

	
}


rt_err_t mq_recv(int mq_type,void *recv,rt_uint16_t len,int time)
{

	if(mq_type >= ABOX_MQ_NUM) return RT_ERROR;

#ifdef VAR_MQ	
	var_mq_t mq = aboxMqInfo[mq_type];
	return recvMsg(mq,recv,len,time);
#else
	rt_mq_t mq =  aboxMqInfo[mq_type].mq;
	return rt_mq_recv(mq,recv,len,time);
#endif
}





/**
 * this function will send package to Abox for Cbox or send pcakage to server for Abox.
 *
 * @param send : send buff
 * @param len  : the length of send buff
 * @event_type : the type of send event
 *
 * @return void
 *
 */
void g3_send_pack(char *send, rt_uint16_t len,int eventType)//len = data length
{
	if(!send) return ;
	Abox_send_pack(send,len,eventType);

}

/**
 * this function will close  link with server.
 *
 *
 * @return void
 *
 */
void close_server()
{
	sendDataToServer(SEND_CLOSE,NULL,0);
}




/**
 * Check server information.
 *
 * @param void
 *
 * @return void
 *
 */
void setSerInfo(serverNet_information *serNetInfo_t)
{
	if(!serNetInfo_t) return;
	setSecret_1(SERVER_NET_INFO_ADDR, (u8 *)serNetInfo_t);
}



/**
 * Check server information.
 *
 * @param void
 *
 * @return void
 *
 */
	
void controlServerInfo(rt_uint8_t styleControl,serverNet_information *RWriteInfo)
{
	serverNet_information serNetInfo;
	rt_memset(&serNetInfo,0,sizeof(serNetInfo)); 
	STMFLASH_Read(SERVER_NET_INFO_ADDR,(u32 *)(&serNetInfo),sizeof(serNetInfo)/4);


	switch(styleControl)
	{
		/* 网络配置相关 */
		case READ_NETCONFIG_INFO: //读取网络配置信息
			rt_memcpy(RWriteInfo,&serNetInfo,sizeof(serverNet_information));
		break;

		case WRITE_NETCONFIG_INFO://写入网络配置信息
			rt_memcpy(&serNetInfo.netConfig,&RWriteInfo->netConfig,sizeof(ser_con_config));
			setSerInfo(&serNetInfo);
		break;

		default:  // 网站配置相关
		{
			if(serNetInfo.serverNetNum < SERVER_DEFAULT_NUM 
			   || serNetInfo.serverNetNum > SERVER_NUM_MAX 
			   || rt_strcmp(URL_SERVER0_NET, serNetInfo.netInfo[NET_SET0].seraddr)
			   || rt_strcmp(URL_SERVER0_PORT, serNetInfo.netInfo[NET_SET0].serport))

			{

				at_printf("check server infor!\r\n");
				/* If the server information does not exist. */
				rt_memset(&serNetInfo,0,sizeof(serNetInfo));

				serNetInfo.serverNetNum = SERVER_DEFAULT_NUM;
				rt_memcpy(serNetInfo.netInfo[NET_SET0].serport,URL_SERVER0_PORT,sizeof(URL_SERVER0_PORT));
				rt_memcpy(serNetInfo.netInfo[NET_SET0].seraddr,URL_SERVER0_NET,sizeof(URL_SERVER0_NET));
				setSerInfo(&serNetInfo);
			}


			switch(styleControl)
			{
				case INIT_SERVER_INFO:
				{
					if(serNetInfo.serverNetNum == SERVER_DEFAULT_NUM)
					{
						Cdma_Info.serNetNum = NET_SET0;
					}
					else if(serNetInfo.serverNetNum > SERVER_DEFAULT_NUM)
					{
						Cdma_Info.serNetNum = NET_SET1;
					}
					//if(!RWriteInfo) return;			
					//rt_memcpy(RWriteInfo,&serNetInfo,sizeof(serverNet_information));

				}
				break;

				case CHANGE_SERVER_INFO:
				{
					if(serNetInfo.serverNetNum == SERVER_DEFAULT_NUM)
					{
						Cdma_Info.serNetNum = NET_SET0;
					}
					else if(serNetInfo.serverNetNum > SERVER_DEFAULT_NUM)
					{
						if(serNetInfo.serverNetNum == (Cdma_Info.serNetNum +1))
							Cdma_Info.serNetNum = NET_SET0;
						else
							Cdma_Info.serNetNum++;
					}
				}
				break;

				case WRITE_SERVER_INFO:
				{
					if(!RWriteInfo) return;
					if(RWriteInfo->serverNetNum > SERVER_WRITE_NUM) return;
					rt_uint8_t i = NET_SET1;
					rt_uint8_t j = 0;			
					rt_memset(&serNetInfo,0,sizeof(serNetInfo));
					setSerInfo(&serNetInfo);
					
					/* write server net information */
					serNetInfo.serverNetNum = SERVER_DEFAULT_NUM;
					rt_memcpy(serNetInfo.netInfo[NET_SET0].serport,URL_SERVER0_PORT,sizeof(URL_SERVER0_PORT));
					rt_memcpy(serNetInfo.netInfo[NET_SET0].seraddr,URL_SERVER0_NET,sizeof(URL_SERVER0_NET));

					serNetInfo.serverNetNum += RWriteInfo->serverNetNum;
					for(;i<serNetInfo.serverNetNum;i++,j++)
					{
						rt_memcpy(serNetInfo.netInfo[i].serport,RWriteInfo->netInfo[j].serport,sizeof(RWriteInfo->netInfo[j].serport));
						rt_memcpy(serNetInfo.netInfo[i].seraddr,RWriteInfo->netInfo[j].seraddr,sizeof(RWriteInfo->netInfo[j].seraddr));
					}

					setSerInfo(&serNetInfo);
					
				}
				return;

				case READ_SERVER_INFO:
				{
					if(!RWriteInfo) return;			
					at_printf("get serNetInfo：%d",serNetInfo.serverNetNum);
					rt_memcpy(RWriteInfo,&serNetInfo,sizeof(serverNet_information));
				}
				return;
					

				default:
					return;
					
			}

			rt_memset(Sys_Setup.serport,0,sizeof(Sys_Setup.serport));
			rt_memset(Sys_Setup.seraddr,0,sizeof(Sys_Setup.seraddr));

			rt_memcpy(Sys_Setup.serport,serNetInfo.netInfo[Cdma_Info.serNetNum].serport,8);
			rt_memcpy(Sys_Setup.seraddr,serNetInfo.netInfo[Cdma_Info.serNetNum].seraddr,16);

			}
			break;


		
	}

}




static void getServerNetAdrr(char *serport,char *seraddr)
{
	serverNet_information readInfo;
	rt_memset(&readInfo,0,sizeof(readInfo));
	readServerInfo(&readInfo);

	rt_memcpy(Sys_Setup.serport,readInfo.netInfo[Cdma_Info.serNetNum].serport,8);		
	rt_memcpy(Sys_Setup.seraddr,readInfo.netInfo[Cdma_Info.serNetNum].seraddr,16);	
}



void initServerInfo(void)
{
	//serverNet_information readInfo;
	//rt_memset(&readInfo,0,sizeof(readInfo));
	controlServerInfo(INIT_SERVER_INFO,NULL);  //   &readInfo);
	//at_printf("1111\r\n");
	
//	rt_memcpy(Sys_Setup.serport,readInfo.netInfo[Cdma_Info.serNetNum].serport,8);		
//	rt_memcpy(Sys_Setup.seraddr,readInfo.netInfo[Cdma_Info.serNetNum].seraddr,16);	
}



void changeServerInfo(void)
{
	controlServerInfo(CHANGE_SERVER_INFO,NULL);
	getServerNetAdrr(Sys_Setup.serport,Sys_Setup.seraddr);
}


void writeServerInfo(serverNet_information *writeInfo)
{
	if(!writeInfo) return;
	if(writeInfo->serverNetNum > SERVER_WRITE_NUM) return;
	controlServerInfo(WRITE_SERVER_INFO,writeInfo);
}

void readServerInfo(serverNet_information *readInfo)
{
	if(!readInfo) return;
	controlServerInfo(READ_SERVER_INFO,readInfo);
}








/**
 * Remote update function.
 *
 * @param mq_recv  : the recvived data mwssage queue.		
 * @param send     : the buff will be send to server.
 * @param recv	   : the buff will recvive from server. 
 *
 * @return void
 *
 */	
void process_DEV_USER_RUPDATA(int mqType,char *send,char *rev)    //DEV_USER_RUPDATA
{
	if(!send || !rev) return;

	int ret = RT_EOK;
	rt_uint32_t startAddr = UPDATA_APP_ADDR;
	rt_uint32_t ver = 0;
	
	stPackCmd packcmd;


	TCMD_12_Info cmd_12;
	TCMD_11_Info cmd_11;

	
	rt_memcpy(&cmd_11, rev + sizeof(stPackCmd), sizeof(cmd_11));
	rt_memcpy(&cmd_12, rev + sizeof(stPackCmd), sizeof(cmd_12));

	
	int filelen = 0;
	int err = 0;

	packcmd.soh = CSOH;	
	packcmd.cmd = 11;
	packcmd.id  = Sys_Setup.app.id;				
	packcmd.len = sizeof(cmd_11);
	Cdma_Info.AT_status = AT_SER_UPDATA;

			
	if((cmd_11.ver < APPF_VER_LOWER)||(cmd_11.ver > APPF_VER_CEILING))
	{
		at_printf("The program version does not match,ver:%d!\r\n");	
		cmd_11.flen = 0xffffffff;//error
		rt_memcpy(send,&packcmd,sizeof(packcmd));
		rt_memcpy(send+sizeof(packcmd),&cmd_11,sizeof(cmd_11));
		g3_send_pack(send, packcmd.len + sizeof(packcmd),UPDATA_EVENT);

		clearSaveDataSector();
		return;
	}


	STMFLASH_Read(startAddr,(u32 *)(&cmd_11.flen),1);
	at_printf("updata start,flen = 0x%x!\r\n",cmd_11.flen);

	if(0xFFFFFFFF == cmd_11.flen)
		cmd_11.flen = 0;
	else
	{
		//check filelen is err!
		if(!(cmd_11.flen < cmd_12.flen))
		{
			/* clear sector */
			clearSaveDataSector();
			cmd_11.flen = 0; // file is NULL.
		}
	}

	startAddr += 4;//save data len
	startAddr += cmd_11.flen;
	
	/* send first pack */
	rt_memcpy(send,&packcmd,sizeof(packcmd));
	rt_memcpy(send+sizeof(packcmd),&cmd_11,sizeof(cmd_11));			
	g3_send_pack(send, packcmd.len + sizeof(packcmd),UPDATA_EVENT);	

	at_printf("cmd11: %d ,cmd12:%d \r\n",cmd_11.flen,cmd_12.flen);


	while(1)
	{ 
	
		   rt_memset(rev,0,BUFF_LEN);
		   if(RT_EOK == mq_recv(mqType, rev,BUFF_LEN,5*RT_TICK_PER_SECOND ))
	       {	
				rt_memcpy(&packcmd, rev, sizeof(stPackCmd));					 
				err = 0;
				if ( packcmd.id == Sys_Setup.app.id && packcmd.cmd == 11)
				{
					
					    rt_memcpy(&filelen,rev + sizeof(stPackCmd),4);
					    if(filelen == cmd_11.flen)
						{			
						    writeByteToFlash(startAddr,(rt_uint8_t *)(rev + sizeof(stPackCmd)+4),packcmd.len-4);

							startAddr += (packcmd.len-4);
							cmd_11.flen += (packcmd.len-4);	
at_printf("write  packcmd.len = %d，cmd_flen = %d，filelen = %d\r\n", packcmd.len,cmd_11.flen,filelen);	
		
							if (cmd_11.flen == cmd_12.flen)	
							{
								writeByteToFlash(UPDATA_APP_ADDR,(rt_uint8_t *)(&cmd_11.flen),4);

								if (RT_EOK == checkFlashFile())
								{
									rt_memcpy(send,&packcmd,sizeof(packcmd));
									rt_memcpy(send+sizeof(packcmd),&cmd_11,sizeof(cmd_11));			
									g3_send_pack( send, packcmd.len + sizeof(packcmd),UPDATA_EVENT);
									ret = 1;
									at_printf("UPDATA IS OK!!!\r\n");
									break;
								}
								else
								{
									ret = -1;
									at_printf("UPDATA IS ERR!!!\r\n");
									break;
						
								}								

							}	
							
							packcmd.soh = CSOH;	
							packcmd.cmd = 11;
							packcmd.id  = Sys_Setup.app.id;				
							packcmd.len = sizeof(cmd_11);
							
							rt_memcpy(send,&packcmd,sizeof(packcmd));
							rt_memcpy(send+sizeof(packcmd),&cmd_11,sizeof(cmd_11));			
							g3_send_pack(send, packcmd.len + sizeof(packcmd),UPDATA_EVENT);

						}
						else
						{
							at_printf("write  error,rev_flen = %d, cmd_11.flen = %d r\n",filelen,cmd_11.flen);
							goto loop;
							
						}
					}
					else
					{															
							ret = -1;
							break;
					}	

			}
			else
			{
					loop:
					at_printf("upload out in rev err %d\r\n", err);
					at_printf("file len = 0x%x,get file len = 0x%x\r\n",cmd_12.flen,cmd_11.flen);
			 
					if(err > 5)
					{
						ret = -1;	
						break;
					}
					else
					{
						g3_send_pack(send, packcmd.len + sizeof(packcmd),UPDATA_EVENT);	
				    	err++;
					}
			}		

	//      
	}

	
	Cdma_Info.AT_status = AT_FREE;
	if (ret == 1) 
	{
		if (Sys_Setup.ver != cmd_12.ver)
		{
			Sys_Setup.ver = ver;
			SaveSetup(&Sys_Setup);
		}

		 extern void	cmd_reboot(void);
		 rt_kprintf("net updata is OK!\r\n");
		 cmd_reboot();
	}	

	/* set error. */
	if(-1 == ret)
	{
		cmd_11.flen = 0xffffffff;//error
		rt_memcpy(send,&packcmd,sizeof(packcmd));
		rt_memcpy(send+sizeof(packcmd),&cmd_11,sizeof(cmd_11));
		g3_send_pack(send, packcmd.len + sizeof(packcmd),UPDATA_EVENT);

		clearSaveDataSector();
	}

	
}








/**
 * Receive server registration information.
 *
 * @param dev_type : current device type.		
 * @param send     : the buff will be send to AT device.
 * @param rev	   : the receive buff
 *
 * @return void
 *
 */	
void process_DEV_USER_GET_SSTATUS(char *rev)    //19
{
	if(!rev) return;

	Abox_DEV_USER_GET_SSTATUS(rev); 


}



void changeAboxFreq(rt_uint8_t *rev)
{
	if(!rev) return;

	rt_uint16_t len = sizeof(stPackCmd);
	freq_information freqInfo;
	
	//rt_memcpy(&freqInfo.preFreq,rev+len,sizeof(freqInfo.preFreq));
	len += 2;
	freqInfo.preFreq = 0; //default

	rt_memcpy(&freqInfo.freqSaveNum,rev+len,sizeof(freqInfo.freqSaveNum));
	len += 2;

	rt_memcpy(freqInfo.freqCollect,rev+len,sizeof(freqInfo.freqCollect[0])*freqInfo.freqSaveNum);

	if(RT_ERROR == checkFreqInfo(&freqInfo))
	{
		saveFreqInfo(&freqInfo);
		aboxInfo.freqInfo = freqInfo;
		rt_event_send(aboxInfo.aboxEvent, SER_CHANGE_FREQ_ET);	
	}
}


/* 发送log数据给服务器 */
void searchLogSheet(void)
{
	send_log_diagnosis();
}




/* 查询配置表信息 */
void searchSheet(rt_uint8_t *rev)
{
	if(!rev) return;

	rt_uint16_t mark;
	rt_memcpy(&mark, rev+sizeof(stPackCmd), sizeof(mark));
	switch(mark)
	{
		case AUX_SHEEET_CONFIG: //aux config 
			//recvAuxSheetRequest(rev);
		break;

		case IO_SHEET_CONFIG: //IO config
			//recvIOSheetRequest(rev);
		break;

		case LOG_SHEET_CONFIG:
			searchLogSheet();
		default:
			break;
	}
	return;
}



/**
 * This function will accept data from the server and parse it.
 *
 * @param precv  : gets the data received from the server.
 * @param mq_recv: the message queues that receive data
 *
 *
 */									   
void cdma_rev_pack(rt_uint8_t *precv,int mqType)
{
	if(!precv) return ;

	/* copy headpack */
	stPackCmd * packcmd = (stPackCmd *)precv;
	

	char send[BUFF_LEN]= {0};

	//at_printf("recv serpack : 0x%x \r\n",packcmd->cmd);
	
	/* choose cmd */
    if (packcmd->soh == SSOH || packcmd->soh == CSOH) // cbox ：  packcmd.id == Sys_Setup.app.id
	{					
			switch (packcmd->cmd)
			{
					case TCMD_1:
						Cdma_Info.serHeartTime = rt_tick_get();
					break;
						
							
					case TCMD_12: //12   remote updata	
					{
						process_DEV_USER_RUPDATA(mqType,send,(char *)precv);  	
					}
				    break;
								
				
					case TCMD_19://19 server registration information
					{	
						process_DEV_USER_GET_SSTATUS((char *)precv); 
					}
					break;
					
				
					case TCMD_110:
					{
						changeAboxFreq(precv);
					}
					break;

					case TCMD_112:
					{
						searchSheet(precv);
					}
					break;

					case TCMD_121:
					{										
						//do dictionary
						recv_dic_cmd((char *)precv,send);
					}
					break;
					
					default:
							break;
			}					
	}
		

}




/**
 * This function will move the data in the buffer to the left.
 *
 * @param buff.
 * @param len
 *
 * @return  void
 *
 */
void data_left(rt_uint8_t *buf,rt_uint32_t len,rt_uint32_t buf_size)
{
	if((!buf) || (!len)) return;

	int i=len;
	int j=0;
	rt_uint8_t *tbuf = buf;


	for(;i<buf_size;i++,j++)
		tbuf[j] = tbuf[i];
	for(;j<buf_size;j++)
		tbuf[j]=0;
	
}





/**
 * This function will reboot moden.
 *
 * @param void
 *
 * @return  void
 *
 */
void HWReboot()
{

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0;
	GPIO_Init(GPIOC, &GPIO_InitStructure);		
			
	GPIO_ResetBits(GPIOC, GPIO_Pin_0);
	while(1)
	{
		at_printf("waiting reboot\r\n");
		rt_thread_delay(RT_TICK_PER_SECOND);
	}	
}







