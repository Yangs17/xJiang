#include "adc.h"
#include <rthw.h>
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "mydef.h"
#include "dictionary.h"
#include "netif.h"
#include "sub_G.h"
#include "abox.h"
#include "sockets.h"


extern rt_uint16_t  ModbusCrc16(rt_uint8_t * frame, rt_uint16_t len);


static rt_thread_t powerThread; //
static rt_sem_t powerSem;



/**
 * 
 * Send out the message.
 *
 * @param device gsm module's device
 *
 * @return void
 *
 */
int packADCInfo(void)
{
	send_ADCinfo adc_info;

	rt_uint8_t sendInfo[100];
	rt_memset(sendInfo,0,100);
	rt_memset(&adc_info,0,sizeof(send_ADCinfo));


	rt_uint16_t  crc = 0;
	adc_info.packcmd.soh     = CSOH;   //??????
	adc_info.packcmd.cmd	 = TCMD_120;
	adc_info.packcmd.id      = Sys_Setup.app.id;	//mod 				
	adc_info.packcmd.len     = sizeof(PL_Info)+sizeof(adc_info.dev_type);//data_len + dev_type

	adc_info.dev_type = ABOX_DEV_TYPE;

	{ 
		adc_info.plInfo.mod_signAVE = 0xffff;
		adc_info.plInfo.device = 0xffff;
		adc_info.plInfo.dev_judge = 0xffff;
	}
	
	crc = ModbusCrc16((rt_uint8_t *)(&adc_info), sizeof(adc_info)-2);
	adc_info.crc = crc;
	rt_memcpy(sendInfo, &adc_info, sizeof(send_ADCinfo)); 
	at_printf("send power supply alarm!\r\n");	


	//return	send(sock, sendInfo,sizeof(send_ADCinfo), 0);
	sendNormalData(sendInfo,sizeof(send_ADCinfo));
	return 0;
}




//PB8 �½����ж�
void EXTI9_5_IRQHandler(void)
{
	 rt_sem_release(powerSem);	
	 EXTI_ClearITPendingBit(EXTI_Line8);//���LINE6�ϵ��жϱ�־λ 
}

static void power_thread_entry(void* parameter)
{
	while(1)
	{
		if(RT_EOK == rt_sem_take(powerSem,RT_WAITING_FOREVER))
		{
			rt_kprintf("send power down info!\r\n");
			sendDataToServer(SEND_POWER_DOWN,NULL,0);
			rt_thread_delay(2000);
			cmd_reboot();
		}
	}
}





/**
 * 
 * this function will initialize the ADC device.
 *
 * @param void
 *
 * @return void
 *
 */

void rt_system_PD_init(void)
{
#if 1


	//PB8 -- �½��ش���
	//nIRQ  PB8
	GPIO_Set(GPIOB,PIN8,GPIO_MODE_IN,0,0,GPIO_PUPD_PD);
	
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource8);//P15 ���ӵ��ж���15

	/* ����EXTI_Line15 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
    EXTI_Init(&EXTI_InitStructure);//����

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;;//�ⲿ�ж�9~5
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);//����


	powerSem = rt_sem_create ("powerSem",0, RT_IPC_FLAG_FIFO);
	powerThread = rt_thread_create("power_ex",
							power_thread_entry, RT_NULL,
							1024,11, 20);
	if(!powerSem || !powerThread)
	{
		at_printf("power set is err!\r\n");
	}
	else
	{

		powerSem->value = 0;
		rt_thread_startup(powerThread);
	}

#endif

	return;
}

