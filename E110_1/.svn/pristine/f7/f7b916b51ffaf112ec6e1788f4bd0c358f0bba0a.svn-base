/*
 * File      : usart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2010-03-29     Bernard      remove interrupt Tx and DMA Rx mode
 * 2012-02-08     aozima       update for F4.
 * 2012-07-28     aozima       update for ART board.
 */

#include "stm32f4xx.h"
#include "usart.h"
#include "board.h"
#include "ctrl.h"
#include <rtdevice.h>


#ifdef RT_USING_UART1

#define UART1_GPIO_TX       GPIO_Pin_9
#define UART1_TX_PIN_SOURCE GPIO_PinSource9
#define UART1_GPIO_RX       GPIO_Pin_10
#define UART1_RX_PIN_SOURCE GPIO_PinSource10
#define UART1_GPIO          GPIOA
#define UART1_GPIO_OE 		GPIO_Pin_8

#endif


#ifdef RT_USING_UART2
//Cbox OE:PE12   Abox OE:PA8

#define UART2_GPIO_TX       GPIO_Pin_5
#define UART2_TX_PIN_SOURCE GPIO_PinSource5
#define UART2_GPIO_RX       GPIO_Pin_6
#define UART2_RX_PIN_SOURCE GPIO_PinSource6
#define UART2_GPIO          GPIOD

#endif


#ifdef RT_USING_UART3

#define UART3_GPIO_TX       GPIO_Pin_8
#define UART3_TX_PIN_SOURCE GPIO_PinSource8
#define UART3_GPIO_RX       GPIO_Pin_9
#define UART3_RX_PIN_SOURCE GPIO_PinSource9
#define UART3_GPIO          GPIOD
#define UART3_GPIO_OE 		GPIO_Pin_10


#endif



#if 0

#define UART6_GPIO_TX       GPIO_Pin_6
#define UART6_TX_PIN_SOURCE GPIO_PinSource6
#define UART6_GPIO_RX       GPIO_Pin_7
#define UART6_RX_PIN_SOURCE GPIO_PinSource7
#define UART6_GPIO          GPIOC
#define UART6_GPIO_RCC      RCC_AHB1Periph_GPIOC
#define RCC_APBPeriph_UART6 RCC_APB2Periph_USART6 

#endif




/* STM32 uart driver */
struct stm32_uart
{
    USART_TypeDef *uart_device;
    IRQn_Type irq;
};

static rt_err_t stm32_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct stm32_uart *uart;
    USART_InitTypeDef USART_InitStructure;

    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    uart = (struct stm32_uart *)serial->parent.user_data;

    USART_InitStructure.USART_BaudRate = cfg->baud_rate;

    if (cfg->data_bits == DATA_BITS_8)
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    if (cfg->stop_bits == STOP_BITS_1)
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
    else if (cfg->stop_bits == STOP_BITS_2)
        USART_InitStructure.USART_StopBits = USART_StopBits_2;
	
	if(uart->uart_device == USART1)
	{
		USART_InitStructure.USART_BaudRate = 57600;

	}	
	if(uart->uart_device == USART2)
	{
		USART_InitStructure.USART_BaudRate = 115200;
	}
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
		
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(uart->uart_device, &USART_InitStructure);

    /* Enable USART */
 
    USART_Cmd(uart->uart_device, ENABLE);
    /* enable interrupt */

   USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
    return RT_EOK;
}

static rt_err_t stm32_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct stm32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        UART_DISABLE_IRQ(uart->irq);
        /* disable interrupt */
        USART_ITConfig(uart->uart_device, USART_IT_RXNE, DISABLE);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        UART_ENABLE_IRQ(uart->irq);
        /* enable interrupt */
        USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
        break;
    }

    return RT_EOK;
}

static int stm32_putc(struct rt_serial_device *serial, char c)
{
    struct stm32_uart* uart;

    uart = (struct stm32_uart *)serial->parent.user_data;
	if (uart->irq == USART3_IRQn)
	{
		GPIO_SetBits(UART3_GPIO ,UART3_GPIO_OE);			
	}
	
    RT_ASSERT(serial != RT_NULL);
    
    while (!(uart->uart_device->SR & USART_FLAG_TXE)){}
    uart->uart_device->DR = c;
    while (!(uart->uart_device->SR & USART_FLAG_TC)){}
			
	if (uart->irq == USART3_IRQn)	
	{
		GPIO_ResetBits(UART3_GPIO ,UART3_GPIO_OE);
	} 
    return 1;


}




static int stm32_getc(struct rt_serial_device *serial)
{
    int ch;
    struct stm32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    ch = -1;
    if (uart->uart_device->SR & USART_FLAG_RXNE)
    {
        ch = uart->uart_device->DR & 0xff;
//		USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
    }

    return ch;
}


/**
 * Uart common interrupt process. This need add to uart ISR.
 *
 * @param serial serial device
 */
static void uart_isr(struct rt_serial_device *serial) 
{
    struct stm32_uart *uart = (struct stm32_uart *) serial->parent.user_data;

    RT_ASSERT(uart != RT_NULL);
	// Judge whether the receiving interrupt occurred.
    if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
    {
    	// Judge parity bit.
        if(USART_GetFlagStatus(uart->uart_device, USART_FLAG_PE) == RESET)
        {
        	//Process receiving signals.
            rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
        }
				/* clear interrupt */
     	else
		{
		   USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
		}
    }


	// Send completion mark
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET)
    {
        /* clear interrupt */
        if(serial->parent.open_flag & RT_DEVICE_FLAG_INT_TX)
        {
            rt_hw_serial_isr(serial, RT_SERIAL_EVENT_TX_DONE);
        }
        USART_ITConfig(uart->uart_device, USART_IT_TC, DISABLE);
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }
	// Overflow error flag bit.
    if (USART_GetFlagStatus(uart->uart_device, USART_FLAG_ORE) == SET)
    {
        stm32_getc(serial);
    }
}

static const struct rt_uart_ops stm32_uart_ops =
{
    stm32_configure,
    stm32_control,
    stm32_putc,
    stm32_getc,
};

#if defined(RT_USING_UART1)
/* UART1 device driver structure */
struct stm32_uart uart1 =
{
    USART1,
    USART1_IRQn,
};
struct rt_serial_device serial1;

void USART1_IRQHandler(void)
{
//    struct stm32_uart *uart;

 //   uart = &uart1;

    /* enter interrupt */
    rt_interrupt_enter();
	uart_isr(&serial1);
#if 0
    if (USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
    {
        rt_hw_serial_isr(&serial1, RT_SERIAL_EVENT_RX_IND);
    }
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET)
    {
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }
#endif
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART1 */


#if defined(RT_USING_UART2)
/* UART2 device driver structure */
struct stm32_uart uart2 =
{
    USART2,
    USART2_IRQn,
};
struct rt_serial_device serial2;

void USART2_IRQHandler(void)
{
    struct stm32_uart *uart;

    uart = &uart2;

    /* enter interrupt */
    rt_interrupt_enter();
    if (USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
    {
        rt_hw_serial_isr(&serial2, RT_SERIAL_EVENT_RX_IND);
    }
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET)
    {
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }

    /* leave interrupt */
    rt_interrupt_leave();
}

#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
/* UART3 device driver structure */
struct stm32_uart uart3 =
{
    USART3,
    USART3_IRQn,
};
struct rt_serial_device serial3;



void USART3_IRQHandler(void)
{
    struct stm32_uart *uart;

    uart = &uart3;

    /* enter interrupt */
    rt_interrupt_enter();
    if (USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
    {
        rt_hw_serial_isr(&serial3, RT_SERIAL_EVENT_RX_IND);
    }
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET)
    {
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART3 */



#if defined(RT_USING_UART6)
/* UART1 device driver structure */
struct stm32_uart uart6 =
{
    USART6,
    USART6_IRQn,
};
struct rt_serial_device serial6;

void USART6_IRQHandler(void)
{
    struct stm32_uart *uart;

    uart = &uart6;

    /* enter interrupt */
    rt_interrupt_enter();
    if (USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
    {
        rt_hw_serial_isr(&serial1, RT_SERIAL_EVENT_RX_IND);
    }
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET)
    {
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART6 */


static void RCC_Configuration(void)
{
#ifdef RT_USING_UART1
    /* Enable UART1 GPIO clocks */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    /* Enable UART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
#endif /* RT_USING_UART1 */

#ifdef RT_USING_UART2
    /* Enable UART2 GPIO clocks */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
     /* Enable UART2 clock */
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
#endif /* RT_USING_UART1 */

#ifdef RT_USING_UART3
    /* Enable UART3 GPIO clocks */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    /* Enable UART3 clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
#endif /* RT_USING_UART3 */

#ifdef RT_USING_UART6
    /* Enable UART3 GPIO clocks */
    RCC_AHB1PeriphClockCmd(UART6_GPIO_RCC, ENABLE);
    /* Enable UART3 clock */
    RCC_APB1PeriphClockCmd(RCC_APBPeriph_UART6, ENABLE);
#endif /* RT_USING_UART6 */


}

static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitTypeDef GPIO_InitStructureOE;
//	USART_InitTypeDef USART_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 


#ifdef RT_USING_UART1
    /* Configure USART1 Rx/tx PIN */
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_RX | UART1_GPIO_TX;

    /* Connect alternate function */
    GPIO_PinAFConfig(UART1_GPIO, UART1_TX_PIN_SOURCE, GPIO_AF_USART1);
    GPIO_PinAFConfig(UART1_GPIO, UART1_RX_PIN_SOURCE, GPIO_AF_USART1);
	GPIO_Init(UART1_GPIO, &GPIO_InitStructure);

	
#endif /* RT_USING_UART1 */

#ifdef RT_USING_UART2

    /* Configure USART2 Rx/tx PIN */

   GPIO_InitStructure.GPIO_Pin = UART2_GPIO_RX | UART2_GPIO_TX;

    /* Connect alternate function */
    GPIO_PinAFConfig(UART2_GPIO, UART2_TX_PIN_SOURCE, GPIO_AF_USART2);
    GPIO_PinAFConfig(UART2_GPIO, UART2_RX_PIN_SOURCE, GPIO_AF_USART2);
	
	GPIO_Init(UART2_GPIO, &GPIO_InitStructure);


#endif


#ifdef RT_USING_UART3
    /* Configure USART3 Rx/tx PIN */
    GPIO_InitStructure.GPIO_Pin = UART3_GPIO_TX | UART3_GPIO_RX;

    /* Connect alternate function */
    GPIO_PinAFConfig(UART3_GPIO, UART3_TX_PIN_SOURCE, GPIO_AF_USART3);
    GPIO_PinAFConfig(UART3_GPIO, UART3_RX_PIN_SOURCE, GPIO_AF_USART3);
	GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
#endif /* RT_USING_UART3 */



#ifdef RT_USING_UART6
    /* Configure USART3 Rx/tx PIN */
    GPIO_InitStructure.GPIO_Pin = UART6_GPIO_TX | UART6_GPIO_RX;

    /* Connect alternate function */
    GPIO_PinAFConfig(UART6_GPIO, UART6_TX_PIN_SOURCE, GPIO_AF_USART6);
    GPIO_PinAFConfig(UART6_GPIO, UART6_RX_PIN_SOURCE, GPIO_AF_USART6);
	    GPIO_Init(UART6_GPIO, &GPIO_InitStructure);
#endif /* RT_USING_UART6 */

	//usart1，485模式控制  
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PE 12 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOE,&GPIO_InitStructure); //初始化

	//usart3，485模式控制  
	GPIO_InitStructure.GPIO_Pin = UART3_GPIO_OE; //PD 10
	GPIO_Init(UART3_GPIO,&GPIO_InitStructure); //初始化

}


static void NVIC_Configuration(struct stm32_uart *uart)
{
    NVIC_InitTypeDef NVIC_InitStructure;

   
    NVIC_InitStructure.NVIC_IRQChannel = uart->irq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

int stm32_hw_usart_init(void)
{
  		struct stm32_uart* uart;
		struct serial_configure config;


		RCC_Configuration();
		GPIO_Configuration();
	//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

#ifdef RT_USING_UART1
		uart = &uart1;
		config.baud_rate = Sys_Setup.com[0].baud_rate;
		config.bit_order = BIT_ORDER_LSB;
		config.data_bits = Sys_Setup.com[0].data_bits;
		config.parity    = Sys_Setup.com[0].parity;
		config.stop_bits = Sys_Setup.com[0].stop_bits;
		config.invert    = NRZ_NORMAL;
		config.bufsz	 	 = RT_SERIAL_RB_BUFSZ;
		config.reserved	 = 0;		
		serial1.ops      = &stm32_uart_ops;
		serial1.config   = config;

		NVIC_Configuration(&uart1);

		/* register UART1 device */
		rt_hw_serial_register(&serial1, "uart1",
													RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX ,
													uart);
#endif

#ifdef RT_USING_UART2
		uart = &uart2;
  

		config.baud_rate = Sys_Setup.com[1].baud_rate;
		config.bit_order = BIT_ORDER_LSB;
		config.data_bits = Sys_Setup.com[1].data_bits;
		config.parity    = Sys_Setup.com[1].parity;
		config.stop_bits = Sys_Setup.com[1].stop_bits;
		config.invert    = NRZ_NORMAL;
		config.bufsz	   = RT_SERIAL_RB_BUFSZ;
		config.reserved	 = 2;	
		serial2.ops      = &stm32_uart_ops;
	//	serial2.int_rx = &uart2_int_rx;
		serial2.config   = config;

		NVIC_Configuration(&uart2);


		/* register UART2 device */
		rt_hw_serial_register(&serial2,
		 "uart2",											
		 RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX ,
		 uart);
#endif

#ifdef RT_USING_UART3
		uart = &uart3;

		config.baud_rate = Sys_Setup.com[2].baud_rate;
		config.bit_order = BIT_ORDER_LSB;
		config.data_bits = Sys_Setup.com[2].data_bits;
		config.parity    = Sys_Setup.com[2].parity;
		config.stop_bits = Sys_Setup.com[2].stop_bits;
		config.invert    = NRZ_NORMAL;
		config.bufsz	   = RT_SERIAL_RB_BUFSZ;
		config.reserved	 = 3;		
		serial3.ops    	 = &stm32_uart_ops;
	//	serial3.int_rx = &uart3_int_rx;
		serial3.config   = config;

		NVIC_Configuration(&uart3);

		/* register UART3 device */
		rt_hw_serial_register(&serial3, "uart3",
													RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX ,
													uart);
#endif

#ifdef RT_USING_UART4
		uart = &uart4;

		config.baud_rate = Sys_Setup.com[3].baud_rate;
		config.bit_order = BIT_ORDER_LSB;
		config.data_bits = Sys_Setup.com[3].data_bits;
		config.parity    = Sys_Setup.com[3].parity;
		config.stop_bits = Sys_Setup.com[3].stop_bits;
		config.invert    = NRZ_NORMAL;
		config.bufsz	   = RT_SERIAL_RB_BUFSZ;
		config.reserved	 = 4;
		serial4.ops    	 = &stm32_uart_ops;
	//	serial4.int_rx = &UART4_int_rx;
		serial4.config   = config;

		NVIC_Configuration(&uart4);

		/* register UART4 device */
		rt_hw_serial_register(&serial4, "uart4",
													RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM,
													uart);
#endif

#ifdef RT_USING_UART5
		uart = &uart5;

		config.baud_rate = Sys_Setup.com[4].baud_rate;
		config.bit_order = BIT_ORDER_LSB;
		config.data_bits = Sys_Setup.com[4].data_bits;
		config.parity    = Sys_Setup.com[4].parity;
		config.stop_bits = Sys_Setup.com[4].stop_bits;
		config.invert    = NRZ_NORMAL;
		config.bufsz	   = RT_SERIAL_RB_BUFSZ;
		config.reserved	 = 5;
		serial5.ops    	 = &stm32_uart_ops;
	//	serial5.int_rx = &UART5_int_rx;
		serial5.config   = config;

		NVIC_Configuration(&uart5);

		/* register UART5 device */
		rt_hw_serial_register(&serial5, "uart5",
													RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM,
													uart);
#endif

#ifdef RT_USING_UART6
		  uart = &uart6;
  
		  config.baud_rate = Sys_Setup.com[5].baud_rate;
		  config.bit_order = BIT_ORDER_LSB;
		  config.data_bits = Sys_Setup.com[5].data_bits;
		  config.parity    = Sys_Setup.com[5].parity;
		  config.stop_bits = Sys_Setup.com[5].stop_bits;
		  config.invert    = NRZ_NORMAL;
		  config.bufsz		 = RT_SERIAL_RB_BUFSZ;
		  config.reserved  = 6;
		  serial6.ops	   = &stm32_uart_ops;
	  //  serial6.int_rx = &UART5_int_rx;
		  serial6.config   = config;
  
		  NVIC_Configuration(&uart6);
  
		  /* register UART5 device */
		  rt_hw_serial_register(&serial6, "uart6",
													  RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM,
													  uart);
#endif

  return 0;
}
INIT_BOARD_EXPORT(stm32_hw_usart_init);

