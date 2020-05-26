/*
 * File      : spi_flash_w25qxx.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2011, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-12     sea_start    the first version
 */

#ifndef SPI_FLASH_W25QXX_H_INCLUDED
#define SPI_FLASH_W25QXX_H_INCLUDED

#include <rtthread.h>
#include <spi.h>

#define AT25SFXX	1
#define IS25LPXX	2

#define SPI_FLASH_TYPE 	IS25LPXX
//#define SPI_FLASH_TYPE 	AT25SFXX

#if SPI_FLASH_TYPE == AT25SFXX
/*normal read maxize frequence*/
#define SPI_FLASH_NRD_MAXMZ		66   /*Mhz*/
#else
#define SPI_FLASH_NRD_MAXMZ		50   /*Mhz*/
#endif

struct spi_flash_device
{
    struct rt_device                flash_device;
    struct rt_device_blk_geometry   geometry;
    struct rt_spi_device *          rt_spi_device;
    struct rt_mutex                 lock;
};

extern rt_err_t at25sfxx_init(const char * flash_device_name,
                            const char * spi_device_name);


#endif // SPI_FLASH_W25QXX_H_INCLUDED
