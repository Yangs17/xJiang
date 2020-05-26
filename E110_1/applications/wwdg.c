#include <wwdg.h>
#include "stm32f4xx_wwdg.h"



enum WWDG_STATUS
{
	WWDG_OPEN,
	WWDG_CLOSE,
};
static rt_uint8_t wwdgStatus = WWDG_CLOSE;


void wwdg_auto_close()
{
	wwdgStatus = WWDG_CLOSE;
}

void wwdg_auto_open()
{
	wwdgStatus = WWDG_OPEN;
}



//初始化独立看门狗
//prer:分频数:0~7(只有低3位有效!)
//rlr:自动重装载值,0~0XFFF.
//分频因子=4*2^prer.但最大值只能是256!
//rlr:重装载寄存器值:低11位有效.
//时间计算(大概):Tout=((4*2^prer)*rlr)/32 (ms).

void IWDG_Init(u8 prer,u16 rlr)
{
	wwdg_auto_close();

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //使能对IWDG->PR IWDG->RLR的写
	
	IWDG_SetPrescaler(prer); //设置IWDG分频系数

	IWDG_SetReload(rlr);   //设置IWDG装载值

	IWDG_ReloadCounter(); //reload
	
	IWDG_Enable();       //使能看门狗
}

//喂独立看门狗
void IWDG_Feed(void)
{
	
	if(monitorTick &&  (wwdgStatus == WWDG_CLOSE))    
	{
		if(rt_tick_get() > monitorTick)
		{
			if((rt_tick_get() - monitorTick) > 10*1000) //超过10s 触发wwdg
				return;
		}
		else 
			monitorTick = rt_tick_get();
	}
	
	if(wwdgStatus == WWDG_OPEN)
		monitorTick = rt_tick_get();

	IWDG_ReloadCounter();//reload
}



/* 定时器的控制块*/
static rt_timer_t IWDG_timer;
void rt_thread_IWDG_init(void)
{
	//rt_err_t result;
	IWDG_Init(4,2000);//1000ms
#if 1
	IWDG_timer = rt_timer_create("IWDG_timer", /* 定时器名字 */
								IWDG_Feed, /* 超时时回调的处理函数*/
								RT_NULL, /* 超时函数的入口参数*/
								100, /* 定时长度为100个OS Tick */
								RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_HARD_TIMER
								); /* 周期定时     软件定时器*/
	/* 启动定时器*/
	if (IWDG_timer != RT_NULL) rt_timer_start(IWDG_timer);
#endif

}








#if 0

void Feeddog(void)
{
	 WWDG_SetCounter(0x7f);
	 WWDG_ClearFlag();
}




void WWDG_IRQHandler(void)
{
	Feeddog();
}


//初始化 watchdog  	
void watchdog_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE);//	WWDG时钟使能  
	WWDG_DeInit();//将外设 WWDG 寄存器重设为缺省值 
	WWDG_SetPrescaler(WWDG_Prescaler_8);//设置WWDG预分频值 
	WWDG_SetWindowValue(0x7F);//设置窗口值    	上限
	WWDG_Enable(0x70);//使能WWDG 并装载初始值   

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=WWDG_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;					
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  
	NVIC_Init(&NVIC_InitStructure);

	WWDG_ClearFlag();//清除早期唤醒中断标志位 
	WWDG_EnableIT();//使能 WWDG 早期唤醒中断（EWI）
	

}







static void wwdg_timeout(void* parameter)
{
	//rt_kprintf("wwdg_timeout\r\n");
	Feeddog();
}

/* 定时器的控制块*/
static rt_timer_t wwdg_timer;

void rt_thread_wwdg_init(void)
{
	//rt_err_t result;
	watchdog_init();

	wwdg_timer = rt_timer_create("wwdg_timer", /* 定时器名字 */
								wwdg_timeout, /* 超时时回调的处理函数*/
								RT_NULL, /* 超时函数的入口参数*/
								5, /* 定时长度为100个OS Tick */
								RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_HARD_TIMER
								); /* 周期定时     软件定时器*/
	/* 启动定时器*/
	if (wwdg_timer != RT_NULL) rt_timer_start(wwdg_timer);

}
#endif
