/*
 * File      : stm32_usbh.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2015, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author            Notes
 * 2017-12-12     ZYH            the first version
 */
#ifndef __STM32_USBH_H__
#define __STM32_USBH_H__
#include <rtthread.h>

#define USB_BUF_LEN 1024
#define USB_DATA_MARK1   ':'    //�ָ���
#define USB_DATA_MARK2   '\r'


enum USB_SETTING_MARK
{
	DEFAULT_MARK = 0, //usb �ղ����ʼ״̬

	NET_SET_OK  = 1,
	NET_SET_ERR = 2,

	APP_SET_OK  = 1,
	APP_SET_ERR = 2,
};

#define LINE_NUM_MAX      20  //���20��
#define LINE_DATA_LEN_MAX 24  //���������24�ֽ�

enum USB_NETLINE_MARK
{
	CONFIG_STYLE_LINE = 0,//��һ�� ��ʾ���ӷ�ʽ
	DHCP_LINE ,//DHCP �Ƿ���
	IP_ADDR_LINE ,//ip addr
	MASK_ADDR_LINE,
	GATWAY_ADDR_LINE,
	WIFI_LOGIN_ID_LINE,
	WIFI_PASSWORD_LINE,
};



typedef struct _usbh_info
{
	rt_uint8_t   appMark;
	rt_uint8_t   netMark; 
	
	unsigned int blen;
	char   		 buff[USB_BUF_LEN];	
	
}usbh_info;


int stm_usbh_register(void);
#endif