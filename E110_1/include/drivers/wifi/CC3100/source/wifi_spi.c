/*
 * spi.c - CC3100-STM32F4 SPI interface implementation
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/


//*****************************************************************************
//
//! \addtogroup spi_api
//! @{
//
//*****************************************************************************

#include "stm32f4xx.h"
//#include "stm32f407xx.h"
//#include "stm32f4xx_hal.h"
//#include "stm32f4_discovery.h"
//#include "stm32f4xx_hal_gpio.h"

#include "net_config.h"
#include "wifi_spi.h"
#include "spi.h"
//SPI_CS PA15  CSN
//SPI_SCK PC10	
//SPI_MISO PC11   
//SPI_MOSI PC12   

#define SPI_MOSI_PIN    PIN12
#define SPI_MOSI_PORT   GPIOC
#define SPI_MISO_PIN    PIN11
#define SPI_MISO_PORT   GPIOC
#define SPI_CLK_PIN     PIN10
#define SPI_CLK_PORT    GPIOC
#define SPI_CS_PIN      PIN15
#define SPI_CS_PORT     GPIOA

#define SPI_TIMEOUT_MAX 0x1000

/* Enable chip select */
//#define ASSERT_CS()     (HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_RESET))
/* Disable chip select */
//#define DEASSERT_CS()   (HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET))

/* SPI handler declaration */
static struct rt_spi_device * wifi_spi_device;


/* Static function declarations */
static int wifi_spi_Init(void);
//static void Error_Handler(void);

/*!
    \brief open spi communication port to be used for communicating with a SimpleLink device

    Given an interface name and option flags, this function opens the spi communication port
    and creates a file descriptor. This file descriptor can be used afterwards to read and
    write data from and to this specific spi channel.
    The SPI speed, clock polarity, clock phase, chip select and all other attributes are all
    set to hardcoded values in this function.

    \param          ifName      -   points to the interface name/path. The interface name is an
                                    optional attributes that the simple link driver receives
                                    on opening the device. in systems that the spi channel is
                                    not implemented as part of the os device drivers, this
                                    parameter could be NULL.
    \param          flags       -   option flags

    \return         upon successful completion, the function shall open the spi channel and return
                    a non-negative integer representing the file descriptor.
                    Otherwise, -1 shall be returned

    \sa             spi_Close , spi_Read , spi_Write
    \note
    \warning
*/
Fd_t spi_Open(char *ifName, unsigned long flags)
{
    wifi_spi_Init();

    /* Initialize and disable the nHIB line */
    CC3100_nHIB_init();
    CC3100_disable();
    
    Delay(50);
    return 0;
}

/*!
    \brief closes an opened spi communication port

    \param          fd          -   file descriptor of an opened SPI channel

    \return         upon successful completion, the function shall return 0.
                    Otherwise, -1 shall be returned

    \sa             spi_Open
    \note
    \warning
*/
int spi_Close(Fd_t fd)
{
    /* Disable Interrupt in GPIOA module... */
    CC3100_InterruptDisable();

    /* Deinitialize SPI */
    //HAL_SPI_DeInit(&SpiHandle);
    return 0;
}

/*!
    \brief attempts to write up to len bytes to the SPI channel

    \param          fd          -   file descriptor of an opened SPI channel

    \param          pBuff       -   points to first location to start getting the data from

    \param          len         -   number of bytes to write to the SPI channel

    \return         upon successful completion, the function shall return 0.
                    Otherwise, -1 shall be returned

    \sa             spi_Open , spi_Read
    \note           This function could be implemented as zero copy and return only upon successful completion
                    of writing the whole buffer, but in cases that memory allocation is not too tight, the
                    function could copy the data to internal buffer, return back and complete the write in
                    parallel to other activities as long as the other SPI activities would be blocked untill
                    the entire buffer write would be completed
    \warning
*/
int spi_Write (Fd_t fd, unsigned char *pBuff, int len)
{
	if(wifi_spi_device)
	{
		rt_spi_send(wifi_spi_device, pBuff,len);
		return len;
	}
    return -1;

}

/*!
    \brief attempts to read up to len bytes from SPI channel into a buffer starting at pBuff.

    \param          fd          -   file descriptor of an opened SPI channel

    \param          pBuff       -   points to first location to start writing the data

    \param          len         -   number of bytes to read from the SPI channel

    \return         upon successful completion, the function shall return 0.
                    Otherwise, -1 shall be returned

    \sa             spi_Open , spi_Write
    \note
    \warning
*/
int spi_Read(Fd_t fd, unsigned char *pBuff, int len)
{
	if(wifi_spi_device)
	{	
		rt_spi_recv(wifi_spi_device, pBuff,len);
		return len;
	}
    return -1;
}

/**
  * @brief SPI MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - NVIC configuration for SPI interrupt request enable
  * @param hspi: SPI handle pointer
  * @retval None
  */
static int wifi_spi_Init()
{
	//rt_hw_spi3_init(): °å¼¶³õÊ¼»¯OK 
	/* register spi */
	wifi_spi_device = (struct rt_spi_device *)rt_device_find("spi30");
	
	if(wifi_spi_device == RT_NULL)
	{
		rt_kprintf("wifi spi dev set err!\r\n");
		return 0;
	}
	
	/* config spi */
	
	{
		
		struct rt_spi_configuration cfg;
		cfg.data_width = 8;
		cfg.mode	   = RT_SPI_MASTER|RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
		cfg.max_hz	   = 3 * 1000 * 1000; //--SPI_BaudRatePrescaler_64	2.625M
	   rt_spi_configure(wifi_spi_device , &cfg);
	   SPI1->I2SCFGR&=~(1<<11);//SPI mod is selected. 
	 
	}


    /* Enable interrupt on the GPIOA pin of CC3100 IRQ */
    CC3100_InterruptEnable();

    return 0;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
#if 0
static void Error_Handler(void)
{
    while(1)
        ;
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
