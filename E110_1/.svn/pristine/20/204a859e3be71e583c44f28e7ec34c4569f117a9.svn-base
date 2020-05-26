#ifndef __WWDG_H__
#define __WWDG_H__

#include <rtthread.h>
#include <stm32f4xx.h>
#include <rthw.h>

#define RT_WWDG_THREAD_PRIO 17
#define WWDG_SECOND 10
static struct rt_thread wwdg_thread;
static rt_uint8_t wwdg_thread_stack[RT_TIMER_THREAD_STACK_SIZE];
extern rt_tick_t monitorTick; //监控时间 更新 防止程序陷入死循环

//初始化 watchdog  	
void watchdog_init(void);
void wwdg_auto_close(void);
void wwdg_auto_open(void);

void rt_thread_wwdg_entry(void *parameter);
void rt_thread_wwdg_init(void);
void IWDG_Feed(void);
void rt_thread_IWDG_init(void);
#endif

