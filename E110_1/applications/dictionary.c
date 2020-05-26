#include "dictionary.h"
#include <string.h>
#include <rtthread.h>
#include <rthw.h>
#include "mydef.h"
#include "abox.h"

/********************
内容包括: 1.数据字典的数据保存
		  2.对于接收cdma命令        0x103 跟 0x124 的处理（跟数据字典有关）
********************/

#define CError 11
#define NUMNULL 0 
#define Period_ms_min 50 //50ms 

dictionary_info dicInfo;
	
/**********  第一类数据字典内容 ***********/
//字典第一类数据 接收数据
void setDicnary1()
{
	first_dictionary_info  firstDicInfo;
	
	//Sys_Setup.ver = APPF_VERSION * 100 + APPF_SUBVERSION * 10 + APPF_REVISION;
	getAppVertion((rt_uint32_t *)(&firstDicInfo.softVer));
	if(firstDicInfo.softVer != Sys_Setup.ver)
	{
		Sys_Setup.ver = firstDicInfo.softVer;
		SaveSetup(&Sys_Setup);	
	}
	
	firstDicInfo.modID = Sys_Setup.app.id;
   
	firstDicInfo.modType = E110;

	firstDicInfo.hwVer = Cdma_Info.hver; //预留
	firstDicInfo.proModel = 1;

	dicInfo.firstDicInfo = firstDicInfo;

}


void setDicnary3()
{
	rt_uint8_t password1[8] = "esto1111"; //Immediately reset the password.
	rt_uint8_t password2[8] = "esto2222"; //Delay reset password.
	dicInfo.thirdDicInfo.resetStatus = 0;
	dicInfo.thirdDicInfo.resetTime   = 0;
	
	rt_memcpy(dicInfo.thirdDicInfo.imePassword,password1,sizeof(password1));   //立即复位
	rt_memcpy(dicInfo.thirdDicInfo.delPassword,password2,sizeof(password2));   //延时复位	
}



void initDictionary()
{
	setDicnary1();
	setDicnary3();
}



/**
 * This function will get signal value.
 *
 * @param   void
 *
 * @return  void
 *
 */
void getModsignal(void)
{
	static U32 times;
	static 	U32 mod_signal_sum;//信号总量
	static U32 mod_signal_num;//信号刷新次数  用来求平均值
	static U8 onceInit1 = 1;
	
	U32 now = 0;
	U16 mod_signal = (U16)Cdma_Info.rssi;
	dicInfo.fourthDicInfo.csq = (U8)mod_signal;

	now = rt_tick_get();
   
	//第一次获取信号强度
	if(onceInit1)
	{
		times = rt_tick_get();
		dicInfo.fourthDicInfo.modSignalMAX =  mod_signal;
		dicInfo.fourthDicInfo.modSignalMIN =  mod_signal;
		dicInfo.fourthDicInfo.modSignalAVE =  mod_signal;
		mod_signal_sum = mod_signal;
		mod_signal_num = 1;
		onceInit1--;
		return;
	}

	//时间大于1天  数据重置
	if(!((now - times)< (RT_TICK_PER_SECOND *60 *60 *24)) )
	{
		onceInit1 =1;
		return;
	}

	//5s刷新一次
	if(!((now - times)< (RT_TICK_PER_SECOND *5)) )
	{
		mod_signal_num++;
		if(mod_signal > dicInfo.fourthDicInfo.modSignalMAX)
		{
			dicInfo.fourthDicInfo.modSignalMAX = mod_signal;
		}

		if(mod_signal < dicInfo.fourthDicInfo.modSignalMIN)
		{
			dicInfo.fourthDicInfo.modSignalMIN = mod_signal;
		}
		mod_signal_sum += (U32)mod_signal;
		dicInfo.fourthDicInfo.modSignalAVE = (U16)(mod_signal_sum/mod_signal_num);
		times = now;
	}

}












void getDicInfo(cycle_info *cycleInfo_t,char *send,rt_uint16_t *sendLen_t,rt_uint16_t *totalCount_t)
{
	if(!cycleInfo_t || !send || !sendLen_t) return;

	rt_uint16_t len = *sendLen_t;
	rt_uint16_t count = *totalCount_t;
	
	if(len > BUFF_LEN) return;

	rt_uint16_t getAdrrNum = 0;
	rt_uint32_t startAddr =  cycleInfo_t->startAddr;
	rt_uint32_t endAddr   =  0;
	rt_uint16_t dataLen   =  0;
	void (*getDataFunc)(rt_uint32_t ,char *,rt_uint16_t *);

	switch(cycleInfo_t->startAddr >> 16)
	{
		case DIC_FIRST_ADDR:
			endAddr = FIRSTDIC_END_ADDR;
			getDataFunc = getDicFirstInfo;
		break;

		case DIC_SECOND_ADDR:
			endAddr = SECONDDIC_END_ADDR;
			getDataFunc = getDicSecondInfo;
		break;

		case DIC_THIRD_ADDR:
			endAddr = THIRDDIC_END_ADDR;
			getDataFunc = getDicThirdInfo;
		break;

		case DIC_FOURTH_ADDR:
			endAddr = FOURTHDIC_END_ADDR;
			getDataFunc = getDicFourthInfo;
		break;

		case DIC_FIFTH_ADDR:
			endAddr = FIFTHDIC_END_ADDR;
			getDataFunc = getDicFifthInfo;
		break;

		default:
			break;

	}
	
	/* get data */
	for(getAdrrNum = 0; getAdrrNum < cycleInfo_t->len; getAdrrNum++)
	{
		rt_memcpy(send+len,&startAddr,sizeof(startAddr));
		len += sizeof(startAddr);

		/* Data overflow */
		if(startAddr > endAddr)
		{
			dataLen = 0;
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);	
		}
		else
		{
			(*getDataFunc)(startAddr,send,&len);
		}
		count ++;
		startAddr ++;
		
	}

	*totalCount_t = count;
	*sendLen_t = len;

}





/**
 * Process commands from the server asking for the data dictionary.
 *
 * @param device At device
 * @param rev the recvive buff
 * @param send the send buff
 * @param buff the void buff
 *
 * @return  void
 *
 */
void del_smd1_func(char *rev, char *send)
{
	if( !rev || !send) return ;

	TCMD_121_1_Info cmdInfo;
	
	rt_uint16_t i = 0;
	rt_uint16_t dataLen = sizeof(stPackCmd)+2;
	rt_uint16_t scmd = TCMD_121_SCMD_LOOK_INFO;
	rt_uint16_t sendLen = 0;
	rt_uint16_t totalCount = 0;
	rt_memset(send,0,BUFF_LEN);
	
	
	/* Structure body bytes not aligned. */
	rt_memcpy(&cmdInfo.count,rev+dataLen,sizeof(cmdInfo.count));
	dataLen += sizeof(cmdInfo.count);

	/* set pack head */
	stPackCmd packcmd;
	packcmd.soh = CSOH;
	packcmd.cmd = TCMD_122;
	packcmd.id = Sys_Setup.app.id;
	packcmd.len = 0;

	//rt_memcpy(send,&packcmd,sizeof(packcmd));
	sendLen = sizeof(packcmd);

	/* scmd */
	//rt_memcpy(send+sendLen,&scmd,sizeof(scmd));
	sendLen += sizeof(scmd);

	/* count */
	//rt_memcpy(send+sendLen,&cmdInfo.count,sizeof(cmdInfo.count));
	sendLen += sizeof(cmdInfo.count);

	/* set pack data */
	for(i = 0; i < cmdInfo.count; i++)
	{
		rt_memcpy(&cmdInfo.cycleInfo.startAddr,rev+dataLen,sizeof(cmdInfo.cycleInfo.startAddr));
		dataLen += sizeof(cmdInfo.cycleInfo.startAddr);
		
		rt_memcpy(&cmdInfo.cycleInfo.len,rev+dataLen,sizeof(cmdInfo.cycleInfo.len));
		dataLen += sizeof(cmdInfo.cycleInfo.len);

		getDicInfo(&cmdInfo.cycleInfo,send,&sendLen,&totalCount);	
	}

	packcmd.len = sendLen - sizeof(packcmd);
	
	rt_memcpy(send,&packcmd,sizeof(packcmd));
	rt_memcpy(send+sizeof(packcmd),&scmd,sizeof(scmd));
	rt_memcpy(send+sizeof(packcmd)+sizeof(scmd),&totalCount,sizeof(totalCount));

	g3_send_pack(send,sendLen,NORMAL_EVENT);


	 
}


/**
 * Process commands from the server asking for the data dictionary.
 *
 * @param rev the recvive buff
 *
 * @return  void
 *
 */
void del_smd2_func(char *rev)
{
	if(!rev) return;
	char password[8];
	rt_memcpy(password,dicInfo.thirdDicInfo.imePassword,8);
	if(rt_strcmp(rev, password))
	{
		extern void HWReboot(void);
		HWReboot();
	}
	

}



void sendCmd122ForScmd4Scmd5(char *send,rt_uint16_t scmd)
{
	if(!send) return;

	/* set pack head */
	stPackCmd packcmd;
	rt_uint16_t len  = 0;
	rt_uint8_t i = 0;
	serverNet_information serNetInfo;
	rt_memset(&packcmd,0,sizeof(packcmd));
	rt_memset(&serNetInfo,0,sizeof(serNetInfo));
	
   	readServerInfo(&serNetInfo);
	
	packcmd.soh = CSOH;
	packcmd.cmd = TCMD_122;
	packcmd.id = Sys_Setup.app.id;
	packcmd.len = sizeof(scmd) + sizeof(serNetInfo.serverNetNum) + serNetInfo.serverNetNum*sizeof(net_info);
	rt_memset(send,0,BUFF_LEN);
	len = sizeof(packcmd);

	/* scmd */
	rt_memcpy(send+len,&scmd,2);
	len += 2;
	
	/* data */
	rt_memcpy(send+len,&serNetInfo.serverNetNum,sizeof(serNetInfo.serverNetNum));
	len += sizeof(serNetInfo.serverNetNum);
	for(i=0;i<serNetInfo.serverNetNum;i++)
	{
		rt_memcpy(send+len,serNetInfo.netInfo[i].serport,sizeof(serNetInfo.netInfo[i].serport));
		len += sizeof(serNetInfo.netInfo[i].serport);

		rt_memcpy(send+len,serNetInfo.netInfo[i].seraddr,sizeof(serNetInfo.netInfo[i].seraddr));
		len += sizeof(serNetInfo.netInfo[i].seraddr);
	}

	rt_memcpy(send,&packcmd,sizeof(packcmd));
	
	g3_send_pack(send,len,NORMAL_EVENT);


}



/**
 * Process commands from the server asking for the data dictionary.
 *
 * @param rev the recvive buff
 *
 * @return  void
 *
 */
void del_smd4_func(char *rev,char *send)
{
	if(!rev) return;
	serverNet_information serNetInfo;
	rt_memcpy(&serNetInfo,rev+sizeof(stPackCmd)+2,sizeof(serNetInfo));
	rt_uint16_t scmd = TCMD_121_SCMD_SERVER_INFO;

	if(serNetInfo.serverNetNum && (serNetInfo.serverNetNum < SERVER_NUM_MAX))
	{
		rt_kprintf("write net info: %d\r\n",serNetInfo.serverNetNum);

		writeServerInfo(&serNetInfo);
		
		sendCmd122ForScmd4Scmd5(send,scmd);

		rt_thread_delay(1000);

		close_server();

		extern void cmd_reboot(void);
		cmd_reboot();
	}


}

void del_smd5_func(char *send)
{
	rt_uint16_t scmd = TCMD_121_SCMD_SERVER_POLL;
	sendCmd122ForScmd4Scmd5(send,scmd);
}





//recvbuf : cmd id data  从cmd开始 不包括crc   recvlen 不包括crc 长度
void recv_dic_cmd(char *rev, char *send)
{
	if(!rev || !send) return ;

	//stPackEstCmd *recv_pack ;
	rt_uint16_t scmd = 0;
	rt_memcpy(&scmd,rev+sizeof(stPackCmd),2);
	rt_memset(send,0,BUFF_LEN);

	/* The secondary command */
	switch(scmd)
	{
		case TCMD_121_SCMD_LOOK_INFO:
		{
			del_smd1_func(rev,send);
		}
		break;

		case TCMD_121_SCMD_REBOOT:
		{
			del_smd2_func(rev);
		}
		break;

		case TCMD_121_SCMD_TELNET:
		{	
#ifdef ABOX	
			extern int telnet_server(void);
			telnet_server();			
#endif
		}
		break;

		case TCMD_121_SCMD_SERVER_INFO:
		{
			del_smd4_func(rev,send);
		}
		break;

		case TCMD_121_SCMD_SERVER_POLL:
		{
			del_smd5_func(send);
		}

		default:
			break;
		
	}

}








void getDicFirstInfo(rt_uint32_t addr,char *send,rt_uint16_t *sendLen_t)
{
	if(!send || !sendLen_t) return;

	rt_uint16_t len = *sendLen_t;
	rt_uint16_t dataLen = 0;
	first_dictionary_info *firstDicInfo_t = &dicInfo.firstDicInfo;

	switch(addr)
	{
		case 0x10000:
			/* len */
			dataLen = sizeof(firstDicInfo_t->softVer);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);

			/* data */
			rt_memcpy(send+len,&firstDicInfo_t->softVer,dataLen);
			len += dataLen;
		break;

		case 0x10001:
			dataLen = sizeof(firstDicInfo_t->hwVer);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
		
			rt_memcpy(send+len,&firstDicInfo_t->hwVer,dataLen);
			len += dataLen;
		break;

		case 0x10002:
			dataLen = sizeof(firstDicInfo_t->modID);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
		
			rt_memcpy(send+len,&firstDicInfo_t->modID,dataLen);
			len += dataLen;
		break;

		case 0x10003:
			dataLen = sizeof(firstDicInfo_t->modType);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
			
			rt_memcpy(send+len,&firstDicInfo_t->modType,dataLen);
			len += dataLen;
		break;

		case 0x10004:
			dataLen = sizeof(firstDicInfo_t->proModel);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);

			rt_memcpy(send+len,&firstDicInfo_t->proModel,dataLen);
			len += dataLen;
		break;

		default:
			at_printf("get dictionnary data err!\r\n");
			
			break;
	}

	*sendLen_t = len;

}


void getDicSecondInfo(rt_uint32_t addr,char *send,rt_uint16_t *sendLen_t)
{
	if(!send || !sendLen_t) return;

	rt_uint16_t len = *sendLen_t;
	rt_uint16_t dataLen = 0;
	second_dictionary_info *secondDicInfo_t = &dicInfo.secondDicInfo;

	switch(addr)
	{
		case 0x20000:
			/* len */
			dataLen = sizeof(secondDicInfo_t->panelLink);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);

			/* data */
			rt_memcpy(send+len,&secondDicInfo_t->panelLink,dataLen);
			len += dataLen;
		break;

		case 0x20001:
			dataLen = sizeof(secondDicInfo_t->panelRevnum);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
		
			rt_memcpy(send+len,&secondDicInfo_t->panelRevnum,dataLen);
			len += dataLen;
		break;

		case 0x20002:
			dataLen = sizeof(secondDicInfo_t->panelSendnum);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
		
			rt_memcpy(send+len,&secondDicInfo_t->panelSendnum,dataLen);
			len += dataLen;
		break;

		case 0x20003:
			dataLen = sizeof(secondDicInfo_t->serConnum);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
			
			rt_memcpy(send+len,&secondDicInfo_t->serConnum,dataLen);
			len += dataLen;
		break;

		case 0x20004:
			dataLen = sizeof(secondDicInfo_t->worktime);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);

			rt_memcpy(send+len,&secondDicInfo_t->worktime,dataLen);
			len += dataLen;
		break;

		default:
			at_printf("get dictionnary data err!\r\n");
			
			break;
	}

	*sendLen_t = len;

}


void getDicThirdInfo(rt_uint32_t addr,char *send,rt_uint16_t *sendLen_t)
{
	if(!send || !sendLen_t) return;

	rt_uint16_t len = *sendLen_t;
	rt_uint16_t dataLen = 0;
	third_dictionary_info *thirdDicInfo_t = &dicInfo.thirdDicInfo;

	switch(addr)
	{
		case 0x30000:
			/* len */
			dataLen = sizeof(thirdDicInfo_t->resetStatus);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);

			/* data */
			rt_memcpy(send+len,&thirdDicInfo_t->resetStatus,dataLen);
			len += dataLen;
		break;

		case 0x30001:
			dataLen = sizeof(thirdDicInfo_t->resetTime);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
		
			rt_memcpy(send+len,&thirdDicInfo_t->resetTime,dataLen);
			len += dataLen;
		break;

		case 0x30002:
			dataLen = 8;
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
		
			rt_memcpy(send+len,thirdDicInfo_t->imePassword,dataLen);
			len += dataLen;
		break;

		case 0x30003:
			dataLen = 8;
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
			
			rt_memcpy(send+len,thirdDicInfo_t->delPassword,dataLen);
			len += dataLen;
		break;

		default:
			at_printf("get dictionnary data err!\r\n");
			
			break;
	}

	*sendLen_t = len;

}



void getDicFourthInfo(rt_uint32_t addr,char *send,rt_uint16_t *sendLen_t)
{
	if(!send || !sendLen_t) return;

	rt_uint16_t len = *sendLen_t;
	rt_uint16_t dataLen = 0;
	fourth_dictionary_info *fourthDicInfo_t = &dicInfo.fourthDicInfo;

	switch(addr)
	{
		case 0x40000:
			/* len */
			dataLen = sizeof(fourthDicInfo_t->csq);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);

			/* data */
			rt_memcpy(send+len,&fourthDicInfo_t->csq,dataLen);
			len += dataLen;
		break;

		case 0x40001:
			dataLen = sizeof(fourthDicInfo_t->modSignalMAX);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
		
			rt_memcpy(send+len,&fourthDicInfo_t->modSignalMAX,dataLen);
			len += dataLen;
		break;

		case 0x40002:
			dataLen = sizeof(fourthDicInfo_t->modSignalMIN);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
		
			rt_memcpy(send+len,&fourthDicInfo_t->modSignalMIN,dataLen);
			len += dataLen;
		break;

		case 0x40003:
			dataLen = sizeof(fourthDicInfo_t->modSignalAVE);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
		
			rt_memcpy(send+len,&fourthDicInfo_t->modSignalAVE,dataLen);
			len += dataLen;
		break;

		case 0x40004:
			dataLen = 16;
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
			
			rt_memcpy(send+len,fourthDicInfo_t->mdn,dataLen);
			len += dataLen;
		break;

		case 0x40005:
			dataLen = 32;
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
			
			rt_memcpy(send+len,fourthDicInfo_t->ccid,dataLen);
			len += dataLen;
		break;

		default:
			at_printf("get dictionnary data err!\r\n");
			
			break;
	}

	*sendLen_t = len;

}




void getDicFifthInfo(rt_uint32_t addr,char *send,rt_uint16_t *sendLen_t)
{
	if(!send || !sendLen_t) return;

	rt_uint16_t len = *sendLen_t;
	rt_uint16_t dataLen = 0;
	fifth_dictionary_info *fifthDicInfo_t = &dicInfo.fifthDicInfo;

	switch(addr)
	{
		case 0x50000:
			/* len */
			dataLen = sizeof(fifthDicInfo_t->freq_num);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);

			/* data */
			rt_memcpy(send+len,&fifthDicInfo_t->freq_num,dataLen);
			len += dataLen;
		break;

		case 0x50001:
			dataLen = sizeof(fifthDicInfo_t->cboxRssi);
			rt_memcpy(send+len,&dataLen,sizeof(dataLen));
			len += sizeof(dataLen);
		
			rt_memcpy(send+len,&fifthDicInfo_t->cboxRssi,dataLen);
			len += dataLen;
		break;


		default:
			at_printf("get dictionnary data err!\r\n");
			
			break;
	}

	*sendLen_t = len;

}


