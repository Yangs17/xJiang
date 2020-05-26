/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"
#include "usart.h"
#include "gpio.h"
#include "ctrl.h"
#include "si4463.h"



//#include "stm32f4xx.h"
/** 
 * @addtogroup STM32
 */

/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, NVIC_VectTab_RAM_OFFSET);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08008000 */
   NVIC_SetVectorTable(NVIC_VectTab_FLASH, NVIC_VectTab_FLASH_OFFSET);
#endif

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
}

/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void  SysTick_Configuration(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    rt_uint32_t         cnts;

    RCC_GetClocksFreq(&rcc_clocks);

    cnts = (rt_uint32_t)rcc_clocks.HCLK_Frequency / RT_TICK_PER_SECOND;
    cnts = cnts / 8;

    SysTick_Config(cnts);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();
	 
    /* leave interrupt */
    rt_interrupt_leave();
}


/**
 * This function  will read the type of AT device.
 *
 * @param void.
 *
 * @return the type of AT device.
 *
 */
char Read_top_tray_status(void)
{
	/* 
		PD 15 :low
		PC 6  :high
	*/
	return PCin(6)<<1|PDin(15);
	  
}




//Initialize the lamp
static void BPS_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//Initialize the lamp
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|
							 RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOF时钟

	//refer to E106's lamp configuration 
	GPIO_InitStructure.GPIO_Pin = LED_MON | LED_CAN | LED_BOX | LED_ROOF;//LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz


	GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PU;//上拉	 低电平有效


  GPIO_Init(LED_GAF, &GPIO_InitStructure);//初始化GPIO
  
		
}



/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
    /* NVIC Configuration */
    NVIC_Configuration();

    /* Configure the SysTick */
    SysTick_Configuration();

	BPS_Init();
	{	
		DefaultSetup(&Sys_Setup);	
	}	



#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif   
#ifdef RT_USING_CONSOLE
    rt_console_set_device(CONSOLE_DEVICE);
#endif
}

/*@}*/

