/*
 * File      : usart.h
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
 */

#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#define		MAX_COM_BUFF									1024
#define		CMD_MODE											0x00
#define		NOT_CMD_MODE									0x01
#define 	UART_ENABLE_IRQ(n)            NVIC_EnableIRQ((n))
#define 	UART_DISABLE_IRQ(n)           NVIC_DisableIRQ((n))



#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814  
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define RS485_TX_EN		PGout(8)	//485模式控制.0,接收;1,发送.


typedef struct	stComRxBuf
{
	int			tail;
	int			head;
	int			bytes;
	char		buf[MAX_COM_BUFF];
} stComRxBuf;

extern	stComRxBuf	  	_Com1_Rx_Buff;
extern	char						_Cmd_Flag;
void 	RS485_init(void);
int 	uart1_putc(char c);
int 	uart1_getc(void);
void 	PutStr(const void *str);
int	  SendUart(const void  *str, int len);
#endif
