#include "board.h"
#include "ctrl.h"
#include <rtthread.h>
#include "wwdg.h"


rt_tick_t monitorTick;
void monitor_thread_entry(void *parameter)
{
	  char   cont = 0;
    while(1)
    {

				if (cont == 1)
						OutputLed(LEDON, LED_MON);					
				else
						OutputLed(LEDOFF, LED_MON);			
	
				cont ^= 1;

		monitorTick = rt_tick_get();
        rt_thread_delay(RT_TICK_PER_SECOND / 5);
		//IWDG_Feed();
    } /* while(1) */
}


static struct rt_thread rt_monitor_thread;//  __attribute__((section ("OTHER_RAM"), zero_init));
static char rt_monitor_thread_stack[256];//   __attribute__((section ("OTHER_RAM"), zero_init));
int  monitor_task(void)
{
    rt_err_t  	init_thread;

		init_thread	= rt_thread_init(&rt_monitor_thread,
									"monitor",
									monitor_thread_entry, RT_NULL,
									&rt_monitor_thread_stack[0], sizeof(rt_monitor_thread_stack),
									RT_THREAD_PRIORITY_MAX - 2, 2);
		
    if (init_thread == RT_EOK)
        rt_thread_startup(&rt_monitor_thread);	
		return 0;
	
}
INIT_APP_EXPORT(monitor_task);

