/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2014-04-27     Bernard      make code cleanup. 
 */

#include <board.h>
#include <rtthread.h>
#include "sub_G.h"


#include "abox.h"


#ifdef RT_USING_COMPONENTS_INIT
#include <components.h>
#endif
#ifdef	RT_USING_AT25SFXX
#include "spi_flash_at25sfxx.h"
#endif

#ifdef RT_USING_LWIP
#include "netif/ethernetif.h"
#endif

//id 31278884
//qq 710338291
#include "ctrl.h"
#include "mydef.h"
stSysSetup				Sys_Setup;
stCdmaInfo	Cdma_Info;



//message pool 
char *msg_pool_arry;// message pool

void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
  rt_components_init();

	
#elif defined(RT_USING_FINSH)
    finsh_system_init(); //shell
#endif

	
#ifdef	RT_USING_AT25SFXX
	{
   at25sfxx_init("flash0", "spi30");
	//if(RT_EOK == w25qxx_init("flash0", "spi20"))
		//GPIO_SetBits(GPIOD,GPIO_Pin_1); 
	}
#endif


	ReadSetup(&Sys_Setup);// 读取 信息初始化串口	
	//Sys_Setup.app.mode = DEV_MODBUS_TCP_SERVER;
	Sys_Setup.app.mode = DEV_DEFAULT;
	//Sys_Setup.app.mode = DEV_SSL_TSL_CLIENT_TEST;


    /* Initialize part of the information */
    Cdma_Info.ver = 0x01;
    Cdma_Info.link = DISCONNENCT;
	
	Sys_Setup.cdmatype = AT_SUB_G;
	at_printf("id = %d\r\n",Sys_Setup.app.id);



	subg_Abox_init();
	at_print_close();
	//at_print_open();
 
}


int rt_application_init()
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
							rt_init_thread_entry, RT_NULL,
							4096, RT_THREAD_PRIORITY_MAX/3, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}

