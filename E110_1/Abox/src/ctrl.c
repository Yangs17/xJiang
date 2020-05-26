#include	"rtconfig.h"
#include	"ctrl.h"
#include	"rtdevice.h"
#include	"dfs_posix.h"
#include  "board.h"	
#include "mydef.h"
#include "flash_if.h"
#include "rthw.h"
#include "abox.h"

#define		METER_97	
#define    SER_PORT  "19991"
NET_ADDR   net_addr;	



void  DefaultSetup(stSysSetup *setup)
{
		int i =0;
		
		//usart3 485     usart2 ---- EC20 M26
	    for(i=0;i<MAX_USART;i++)
	    {
			setup->com[i].baud_rate 	= BAUD_RATE_57600;	
			setup->com[i].parity		  = PARITY_NONE;			
			setup->com[i].data_bits   = DATA_BITS_8;
			setup->com[i].stop_bits   = STOP_BITS_1;
			setup->com[i].addr   			= 0;	
	
		}

		//setup->com[0].baud_rate 	= BAUD_RATE_115200;	
		setup->com[1].baud_rate 	= BAUD_RATE_115200;	

//		setup->timezone						= 8;	
//		setup->app.id							= 0;
		setup->app.recmemlen			= 500;
		setup->app.recfilelen			= 10000;
//		setup->app.time 					= NET_TIME;
		setup->app.pagesize				= 10;
		setup->app.rectime				= 1;		

//	 	setup->cdmatype           = 2;
//		setup->heartbeat          = 40;
    	getAppVertion((rt_uint32_t *)(&setup->ver));                //= APPF_VERSION * 100 + APPF_SUBVERSION * 10 + APPF_REVISION;			
		rt_sprintf(setup->setname, "data/setup.dat");	
		rt_sprintf(setup->app.username, "usernmae");
}




/**
 * This function will get the hardware version.
 *
 * @param void
 *
 * @return haedware
*/
rt_uint8_t Read_hard_status(void)
{	
	//PCB ID0 ---PB3
	//PCB ID1 ---PD7
	//PCB ID2 ---PD6
  	GPIO_InitTypeDef  GPIO_InitStructure;

	 char hver[3] = {0};
 	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
 	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
 	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉

 	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  	 GPIO_Init(GPIOB, &GPIO_InitStructure);

  
  	//PD7
 	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
 	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
  	 GPIO_Init(GPIOD, &GPIO_InitStructure);



	hver[0] =  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3) ;
    hver[1] = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7);
	hver[2] = GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6) ;

	return ( hver[2] <<2 | hver[1]<<1 | hver[0]);
	
	  
}


void	ReadSetup(stSysSetup *setup)
{

	STMFLASH_ReadMoreByte(DATA_ADDR,(u8 *)(setup),sizeof(stSysSetup));		
	Cdma_Info.hver = Read_hard_status();	
}



void	SaveSetup(stSysSetup *setup)
{
	if(setup->app.id && setup->app.id != 0xffffffff)
	{				
		setSecret_1(DATA_ADDR, (u8 *)setup);
	}
	else
		at_printf("id is error! id = %d\r\n",setup->app.id);
}






//RT_EOK FALSH's date is ok.
rt_err_t  initFreqInfo(freq_information *freqInfo)
{
	STMFLASH_ReadMoreByte(DATA_FREQ_ADDR ,(u8 *)(freqInfo),sizeof(freq_information));	

	if((freqInfo->preFreq < freqInfo->freqSaveNum) 
		&& (freqInfo->freqSaveNum < RF_FREQ_CONTROL_INTE_8_NUM+1))
	{
		int i = 0;
		for(i=0;i<freqInfo->freqSaveNum;i++)
		{
			if(freqInfo->freqCollect[i] >= RF_FREQ_CONTROL_INTE_8_NUM)
				return RT_ERROR;
		}

		return RT_EOK;
	}

	return RT_ERROR;		
	
}

//RT_EOK：check ok     RT_ERROR：save data
rt_err_t  checkFreqInfo(freq_information * freqInfo)
{
	if((freqInfo->preFreq >= freqInfo->freqSaveNum) || (freqInfo->freqSaveNum > RF_FREQ_CONTROL_INTE_8_NUM))
		return RT_EOK;


	freq_information freqTemp;
	STMFLASH_ReadMoreByte(DATA_FREQ_ADDR ,(u8 *)(&freqTemp),sizeof(freq_information));	

	if((freqInfo->preFreq == freqTemp.preFreq) && (freqInfo->freqSaveNum == freqTemp.freqSaveNum))
	{
		int i = 0;
		for(i=0;i<freqInfo->freqSaveNum;i++)
		{
			if(freqInfo->freqCollect[i] >= RF_FREQ_CONTROL_INTE_8_NUM)
				return RT_EOK;
		}

		for(i=0;i<freqInfo->freqSaveNum;i++)
		{
			if(freqInfo->freqCollect[i] != freqTemp.freqCollect[i])
				return RT_ERROR;
		}

		return RT_EOK;
	}
	
	return RT_ERROR;
}

void saveFreqInfo(freq_information *freqInfo)
{
	setSecret_1(DATA_FREQ_ADDR, (u8 *)freqInfo);
}



void  OutputLed(char onoff, int led)
{
	if(USB_ConnectStatus()) //USB 连接
			return;
	
	if (onoff == 0)
			GPIO_ResetBits(LED_GAF, led);
	else
			GPIO_SetBits(LED_GAF, led);

}



