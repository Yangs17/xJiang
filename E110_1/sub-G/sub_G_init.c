#include "sub_G.h"
#include "si4463.h"
#include "stm32f4xx_gpio.h"
#include "spi.h"
#include "abox.h"

 
struct rt_spi_device * sub_spi_device;
rt_sem_t    sub_lock;
void checkDataFromSubg(char *checkBuff,rt_uint32_t *checkLength)
{
	if(!checkBuff || !checkLength || !(*checkLength)) return;
	
	rt_uint32_t checkLen = *checkLength;
	stPackCmd *packcmd_t  = (stPackCmd *)checkBuff;
	stPackEstCmd *estpack_t = (stPackEstCmd *)(checkBuff + sizeof(stPackCmd));
	
	rt_uint32_t cmdLen = 0;
//at_printf("1111111\r\n");	
	while(1)
	{
		while(checkLen)
		{
			if((checkBuff[0] != CSOH)  && (checkBuff[0] != SSOH))
			{
				//at_printf("1111111111111 delete data \r\n");	
				data_left((rt_uint8_t *)checkBuff,1,BUFF_LEN);
				checkLen --;
				continue;
			}
			break;
		}

		if(!checkLen) 
		{
			rt_memset(checkBuff,0,BUFF_LEN);
			break;
		}
		
		cmdLen = sizeof(stPackCmd)+packcmd_t->len+2;
		if((cmdLen > BUFF_LEN))
		{
			//at_printf("2222222222222 delete data \r\n");	
			checkBuff[0] = 0;
			continue;
		}

		if(checkLen >= cmdLen)
		{
//at_printf("33333333 checkLen = %d,cmdlen = %d \r\n",checkLen,cmdLen);	

			if(ModbusCrc16((rt_uint8_t *)checkBuff, cmdLen) == 0)
			{
at_printf("check data is ok!len = %d  cmd = %d estcmd = %d\r\n",cmdLen,packcmd_t->cmd,estpack_t->cmd);
				if(Cdma_Info.link != DISCONNENCT)
				{
					sendDataToServer(SEND_DATA, (rt_uint8_t *)checkBuff, cmdLen);
				}
	
				data_left((rt_uint8_t *)checkBuff,cmdLen,BUFF_LEN);
				checkLen -= cmdLen;
			}
			else
			{
				checkBuff[0] = 0;
			}
			continue;
			
		}
		

		break;

	}
	
	*checkLength = checkLen;
	
}






void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD)
{  
	u32 pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//一个个位检查 
		curpin=BITx&pos;//检查引脚是否要设置
		if(curpin==pos)	//需要设置
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->MODER|=MODE<<(pinpos*2);	//设置新的模式 
			if((MODE==0X01)||(MODE==0X02))	//如果是输出模式/复用功能模式
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//清除原来的设置
				GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));//设置新的速度值  
				GPIOx->OTYPER&=~(1<<pinpos) ;		//清除原来的设置
				GPIOx->OTYPER|=OTYPE<<pinpos;		//设置新的输出模式
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	//设置新的上下拉
		}
	}
} 

void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx)
{  
	GPIOx->AFR[BITx>>3]&=~(0X0F<<((BITx&0X07)*4));
	GPIOx->AFR[BITx>>3]|=(u32)AFx<<((BITx&0X07)*4);
}   





//PB15 下降沿中断
void EXTI15_10_IRQHandler(void)
{
	 rt_sem_release(sub_lock);	
	 sub_lock->value = 1;
	 EXTI_ClearITPendingBit(EXTI_Line15);//清除LINE4上的中断标志位 
}




/**
 * This function will read the current signal strength.
 *
 * @param void
 *
 * @return Returns the current signal strength.
 *
 */
unsigned char  getRssi(void)  
{
		spi_read(9,0x22);    //读取rssi值，存放在spi_read_buf[3]中
		return spi_rx_buf[4];//3 当前rssi    4自定义的rssi MODEM_RSSI_CONTROL
}



/**
 * This function will send registration information to Server for sub-g.
 *
 * @param buff
 *
 * @return len : head + data
 *
 */
void sub_PackRegCmd(rt_uint8_t *buff)
{
	if(!buff) return ;
	if(!Sys_Setup.ver) return;

//	rt_uint16_t  tim = 0;
	stPackCmd pp;
	TCMD_21_Info cmd_21;

	pp.soh = CSOH;   
	pp.id  = Sys_Setup.app.id;	
	pp.cmd = TCMD_21;
	pp.len = sizeof(cmd_21);
	rt_memset(buff,0,BUFF_LEN);
	
	rt_memset(&cmd_21, 0, sizeof(cmd_21));
	
	rt_memcpy(buff, &pp, sizeof(stPackCmd));

	/* version costs 2 bytes */
	cmd_21.sw_ver = Sys_Setup.ver;
	cmd_21.dev_id = Cdma_Info.dev_id;


	/* received data costs 8 bytes*/
    rt_memcpy(buff+sizeof(stPackCmd),&cmd_21,sizeof(cmd_21));
	

	rt_uint16_t crc = 0;
	crc = ModbusCrc16((rt_uint8_t *)buff, sizeof(stPackCmd)+sizeof(cmd_21));
	rt_memcpy(buff+sizeof(stPackCmd)+sizeof(cmd_21),&crc,2);	

	sendDataToServer(SEND_DATA, buff, sizeof(stPackCmd)+sizeof(cmd_21)+2);
	
}








/**
 * This function will set to receive status and determine if data is received.
 *
 * @param void
 *
 * @return RT_EOK     It received data in this frequency band.
 *		   RT_ERROR   it doesn't receive data in this frequency band.
 *		
 */
err_t  recvFreqData()
{
	  rt_tick_t sub_tick = rt_tick_get();
	  
      while(1)
      {
	
			if((rt_tick_get() - sub_tick) > SUBG_RECV_TIME_OUT)
				return RT_ERROR;

			if(!nIRQ)
			{  
				clr_interrupt();  
				spi_read_fifo();
				fifo_reset();					
				rx_init();	
				return RT_EOK;

		    }
      }	
	
}


/**
 * This function will set subg's filter.
 *
 * @param filter
 *		  the mark of filter
 *
 * @return void
 *		
 */
void changeSubgFillter(filter_information *filterInfo_t)
{
	if(!filterInfo_t) return;
	
	sdn_reset();		
	SI4463_init(filterInfo_t);	
	rx_init();	
}



/* 核对subg初始化信息 */
void checkSubgInfomation(void)
{
	rt_uint8_t i = 0;
	spi_read(9,0x01);
	for(i=0;i<9;i++)  at_printf("0x%x ",spi_rx_buf[i]);
}




//PB14  sub-G sdn  模块开关控制引脚  必须拉低
//PB15  sub-G IRQ  
void  initSubgHardware(void) 
{
	
	//SDN   PB14
	GPIO_Set(GPIOB,PIN14,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_2M,GPIO_PUPD_PD); 

	//PB15 -- 下降沿触发
	//nIRQ  PB15
	GPIO_Set(GPIOB,PIN15,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);
	
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);//P15 连接到中断线15

	/* 配置EXTI_Line15 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
    EXTI_Init(&EXTI_InitStructure);//配置

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//外部中断10~15
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置
	
	
	
	/* register spi */
	sub_spi_device = (struct rt_spi_device *)rt_device_find("spi10");

    if(sub_spi_device == RT_NULL)
    {
        return ;
    }

    /* config spi */

    {
    	
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode       = RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
        cfg.max_hz     = 3 * 1000 * 1000; //--SPI_BaudRatePrescaler_64  2.625M
	   rt_spi_configure(sub_spi_device , &cfg);
	   SPI1->I2SCFGR&=~(1<<11);//SPI mod is selected. 
	 
    }
	at_printf("spi1 open is OK!\r\n");	


	sub_lock = rt_sem_create("sub_lock",0,RT_IPC_FLAG_FIFO);
	
	

}
