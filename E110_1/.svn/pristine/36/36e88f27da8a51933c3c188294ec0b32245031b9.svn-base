#include <rtthread.h>
#include <rthw.h>
#include "dictionary.h"
#define CPU_USAGE_CALC_TICK	 10
#define CPU_USAGE_LOOP		   100

static rt_uint8_t  cpu_usage = 0;
static rt_uint32_t total_count = 0;
static void cpu_usage_idle_hook(void)
{
	rt_tick_t tick;
	rt_uint32_t count;
	rt_uint32_t s_time;
	volatile rt_uint32_t loop;

	if (total_count == 0)
	{
		/* get total count */
		rt_enter_critical();
		tick = rt_tick_get();
		while(rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
		{
			total_count ++;
			loop = 0;

			while (loop < CPU_USAGE_LOOP) loop ++;
		}
		rt_exit_critical();
	}

	count = 0;
	
	/* get CPU usage */
	tick = rt_tick_get();
	while (rt_tick_get() - tick < CPU_USAGE_CALC_TICK)
	{
		count ++;
		loop  = 0;
		while (loop < CPU_USAGE_LOOP) loop ++;
	}


	
	/* calculate major and minor */
	if (count < total_count)
	{
		count = total_count - count;
		cpu_usage = (count * 100) / total_count;
//		cpu_usage_minor = ((count * 100) % total_count) * 100 / total_count;
	}
	else
	{
		total_count = count;
		cpu_usage = 0;
		/* no CPU usage */
//		cpu_usage_major = 0;
//		cpu_usage_minor = 0;
	}


	
	//判断 worktime 是否变小 变小则累加
	s_time = (rt_tick_get() / RT_TICK_PER_SECOND);
	if(dicInfo.secondDicInfo.worktime > s_time)    //tick 重置   
		dicInfo.secondDicInfo.worktime += s_time;
	else
		dicInfo.secondDicInfo.worktime = s_time;;


	
}
rt_uint8_t get_cpu_usage(void)
{
	return cpu_usage;
}
void cpu_usage_init(void)
{
	/* set idle thread hook */
	rt_thread_idle_sethook(cpu_usage_idle_hook);
}

