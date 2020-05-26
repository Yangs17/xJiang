#ifndef STM32_SPI_H_INCLUDED
#define STM32_SPI_H_INCLUDED

#include <rtdevice.h>
#include "board.h"
#include "stm32f4xx.h"
//#define SPI_USE_DMA

struct stm32_spi_bus
{
    struct rt_spi_bus parent;
    SPI_TypeDef * SPI;
#ifdef SPI_USE_DMA
    DMA_Stream_TypeDef * DMA_Stream_TX;
	uint32_t DMA_Stream_TX_Chanel;
	
    DMA_Stream_TypeDef * DMA_Stream_RX;
	uint32_t DMA_Stream_RX_Chanel;
	
    uint32_t DMA_Stream_TX_FLAG_TC;
    uint32_t DMA_Stream_TX_FLAG_TE;
    uint32_t DMA_Stream_RX_FLAG_TC;
    uint32_t DMA_Stream_RX_FLAG_TE;
#endif /* SPI_USE_DMA */
};

struct stm32_spi_cs
{
    GPIO_TypeDef * GPIOx;
    uint16_t GPIO_Pin;
};

/* public function list */
rt_err_t stm32_spi_register(SPI_TypeDef * SPI,
                            struct stm32_spi_bus * stm32_spi,
                            const char * spi_bus_name);

int rt_hw_spi3_init(void); 


#endif // STM32_SPI_H_INCLUDED

