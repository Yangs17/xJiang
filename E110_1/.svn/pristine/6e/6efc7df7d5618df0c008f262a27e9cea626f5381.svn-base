#include "drv_spi.h"

static rt_err_t configure(struct rt_spi_device* device, struct rt_spi_configuration* configuration);
static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message);

static struct rt_spi_ops stm32_spi_ops =
{
    configure,
    xfer
};

#ifdef USING_SPI1
static struct stm32_spi_bus stm32_spi_bus_1;
#endif /* #ifdef USING_SPI1 */

#ifdef USING_SPI2
static struct stm32_spi_bus stm32_spi_bus_2;
#endif /* #ifdef USING_SPI2 */

#ifdef USING_SPI3
static struct stm32_spi_bus stm32_spi_bus_3;
#endif /* #ifdef USING_SPI3 */

//------------------ DMA ------------------
#ifdef SPI_USE_DMA
static uint8_t dummy = 0xFF;
#endif

#ifdef SPI_USE_DMA
static void DMA_Configuration(struct stm32_spi_bus * stm32_spi_bus, const void * send_addr, void * recv_addr, rt_size_t size)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_ClearFlag(stm32_spi_bus->DMA_Stream_RX,
					stm32_spi_bus->DMA_Stream_RX_FLAG_TC
                  | stm32_spi_bus->DMA_Stream_RX_FLAG_TE);

				  

    /* RX channel configuration */
    DMA_Cmd(stm32_spi_bus->DMA_Stream_RX, DISABLE);
	DMA_DeInit(stm32_spi_bus->DMA_Stream_RX); 
	DMA_InitStructure.DMA_Channel = stm32_spi_bus->DMA_Stream_RX_Chanel;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(stm32_spi_bus->SPI->DR));
	    if(recv_addr != RT_NULL)
    {
        DMA_InitStructure.DMA_Memory0BaseAddr = (u32) recv_addr;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    }
    else
    {
        DMA_InitStructure.DMA_Memory0BaseAddr = (u32) (&dummy);
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    }
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;

	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    

  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输

    DMA_Init(stm32_spi_bus->DMA_Stream_RX, &DMA_InitStructure);
    DMA_Cmd(stm32_spi_bus->DMA_Stream_RX, ENABLE);




    /* TX channel configuration */
	    DMA_ClearFlag(stm32_spi_bus->DMA_Stream_TX,
					stm32_spi_bus->DMA_Stream_TX_FLAG_TC
                  | stm32_spi_bus->DMA_Stream_TX_FLAG_TE);

				  
    DMA_Cmd(stm32_spi_bus->DMA_Stream_TX, DISABLE);
	DMA_DeInit(stm32_spi_bus->DMA_Stream_TX); 

	DMA_InitStructure.DMA_Channel = stm32_spi_bus->DMA_Stream_TX_Chanel;
	DMA_InitStructure.DMA_PeripheralInc	   = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(stm32_spi_bus->SPI->DR));//
	if(send_addr != RT_NULL)
    {
        DMA_InitStructure.DMA_Memory0BaseAddr = (u32)send_addr;
        DMA_InitStructure.DMA_MemoryInc      = DMA_MemoryInc_Enable;
    }
    else
    {
        DMA_InitStructure.DMA_Memory0BaseAddr = (u32)(&dummy);;
        DMA_InitStructure.DMA_MemoryInc      = DMA_MemoryInc_Disable;
    }
	 
	DMA_InitStructure.DMA_DIR 			   = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_InitStructure.DMA_PeripheralInc       = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize	   = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode			   = DMA_Mode_Normal;  
	
	DMA_InitStructure.DMA_Priority		   = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;		   
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;

	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输


    DMA_Init(stm32_spi_bus->DMA_Stream_TX, &DMA_InitStructure);

    DMA_Cmd(stm32_spi_bus->DMA_Stream_TX, ENABLE);
}
#endif

rt_inline uint16_t get_spi_BaudRatePrescaler(rt_uint32_t max_hz)
{
    uint16_t SPI_BaudRatePrescaler;

    /* STM32F10x SPI MAX 18Mhz   f407 168M */
    if(max_hz >= SystemCoreClock / 2 && SystemCoreClock / 2 <= 18000000)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    }
    else if(max_hz >= SystemCoreClock / 4)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    }
    else if(max_hz >= SystemCoreClock / 8)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    }
    else if(max_hz >= SystemCoreClock / 16)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    }
    else if(max_hz >= SystemCoreClock / 32)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    }
    else if(max_hz >= SystemCoreClock / 64)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    }
    else if(max_hz >= SystemCoreClock / 128)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    }
    else
    {
        /* min prescaler 256 */
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    }

    return SPI_BaudRatePrescaler;
}

static rt_err_t configure(struct rt_spi_device* device, struct rt_spi_configuration* configuration)
{
    struct stm32_spi_bus * stm32_spi_bus = (struct stm32_spi_bus *)device->bus;
    SPI_InitTypeDef SPI_InitStructure;

    SPI_StructInit(&SPI_InitStructure);


    /* data_width */
    if(configuration->data_width <= 8)
    {
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    }
    else if(configuration->data_width <= 16)
    {
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    }
    else
    {
        return RT_EIO;
    }
    /* baudrate */
    SPI_InitStructure.SPI_BaudRatePrescaler = get_spi_BaudRatePrescaler(configuration->max_hz);
    /* CPOL */
    if(configuration->mode & RT_SPI_CPOL)
    {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    }
    else
    {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    }
    /* CPHA */
    if(configuration->mode & RT_SPI_CPHA)
    {
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    }
    else
    {
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    }
    /* MSB or LSB */
    if(configuration->mode & RT_SPI_MSB)
    {
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    }
    else
    {
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    }



	
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS  = SPI_NSS_Soft;

    /* init SPI */
    SPI_I2S_DeInit(stm32_spi_bus->SPI);
    SPI_Init(stm32_spi_bus->SPI, &SPI_InitStructure);
    /* Enable SPI_MASTER */
    SPI_Cmd(stm32_spi_bus->SPI, ENABLE);
    SPI_CalculateCRC(stm32_spi_bus->SPI, DISABLE);

    return RT_EOK;
};

static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message)
{
    struct stm32_spi_bus * stm32_spi_bus = (struct stm32_spi_bus *)device->bus;
    struct rt_spi_configuration * config = &device->config;
    SPI_TypeDef * SPI = stm32_spi_bus->SPI;
    struct stm32_spi_cs * stm32_spi_cs = device->parent.user_data;
    rt_uint32_t size = message->length;



    /* take CS */
    if(message->cs_take)
    {
        GPIO_ResetBits(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin);
    }

#ifdef SPI_USE_DMA
    if(message->length > 32)
    {
        if(config->data_width <= 8)
        {

		
            DMA_Configuration(stm32_spi_bus, message->send_buf, message->recv_buf, message->length);
            SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
            while (DMA_GetFlagStatus(stm32_spi_bus->DMA_Stream_RX,stm32_spi_bus->DMA_Stream_RX_FLAG_TC) == RESET
                    || DMA_GetFlagStatus(stm32_spi_bus->DMA_Stream_TX,stm32_spi_bus->DMA_Stream_TX_FLAG_TC) == RESET);
            SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, DISABLE);



		}
//        rt_memcpy(buffer,_spi_flash_buffer,DMA_BUFFER_SIZE);
//        buffer += DMA_BUFFER_SIZE;
    }
    else
#endif
    {
        if(config->data_width <= 8)
        {
            const rt_uint8_t * send_ptr = message->send_buf;
            rt_uint8_t * recv_ptr = message->recv_buf;

            while(size--)
            {
                rt_uint8_t data = 0xFF;

                if(send_ptr != RT_NULL)
                {
                    data = *send_ptr++;
                }


                //Wait until the transmit buffer is empty
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_TXE) == RESET);
                // Send the byte
                SPI_I2S_SendData(SPI, data);

                //Wait until a data is received
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_RXNE) == RESET);
                // Get the received data
                data = SPI_I2S_ReceiveData(SPI);


				
                if(recv_ptr != RT_NULL)
                {
                    *recv_ptr++ = data;
                }
            }
        }
        else if(config->data_width <= 16)
        {
            const rt_uint16_t * send_ptr = message->send_buf;
            rt_uint16_t * recv_ptr = message->recv_buf;

            while(size--)
            {
                rt_uint16_t data = 0xFF;

                if(send_ptr != RT_NULL)
                {
                    data = *send_ptr++;
                }

                //Wait until the transmit buffer is empty
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_TXE) == RESET);
                // Send the byte
                SPI_I2S_SendData(SPI, data);

                //Wait until a data is received
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_RXNE) == RESET);
                // Get the received data
                data = SPI_I2S_ReceiveData(SPI);

                if(recv_ptr != RT_NULL)
                {
                    *recv_ptr++ = data;
                }
            }
        }
    }

    /* release CS */
    if(message->cs_release)
    {
        GPIO_SetBits(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin);
    }

    return message->length;
};

/** \brief init and register stm32 spi bus.
 *
 * \param SPI: STM32 SPI, e.g: SPI1,SPI2,SPI3.
 * \param stm32_spi: stm32 spi bus struct.
 * \param spi_bus_name: spi bus name, e.g: "spi1"
 * \return
 *
 */
rt_err_t stm32_spi_register(SPI_TypeDef * SPI,
                            struct stm32_spi_bus * stm32_spi,
                            const char * spi_bus_name)
{
   // RCC_APB2PeriphClockCmd(RCC_APBPeriph_AFIO, ENABLE);

    if(SPI == SPI1)
    {
    	stm32_spi->SPI = SPI1;
#ifdef SPI_USE_DMA
        /* Enable the DMA1 Clock */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA1时钟使能 

		//DMA1_FLAG_TC1: DMA1 Channel1传输完整标志
		//DMA1_FLAG_TE1: DMA1 Channel1传输错误标志


        stm32_spi->DMA_Stream_RX = DMA2_Stream2;//chanel 3
		stm32_spi->DMA_Stream_RX_Chanel = DMA_Channel_3;
		
        stm32_spi->DMA_Stream_TX = DMA2_Stream3;//chanel 3
		stm32_spi->DMA_Stream_TX_Chanel = DMA_Channel_3;

		
        stm32_spi->DMA_Stream_RX_FLAG_TC = DMA_FLAG_TCIF2;
        stm32_spi->DMA_Stream_RX_FLAG_TE = DMA_FLAG_TEIF2;
        stm32_spi->DMA_Stream_TX_FLAG_TC = DMA_FLAG_TCIF3;
        stm32_spi->DMA_Stream_TX_FLAG_TE = DMA_FLAG_TEIF3;
#endif
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    }
    else if(SPI == SPI2)
    {
        stm32_spi->SPI = SPI2;
#ifdef SPI_USE_DMA
        /* Enable the DMA1 Clock */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能

        stm32_spi->DMA_Stream_RX = DMA1_Stream3;//chanel 0
        stm32_spi->DMA_Stream_RX_Chanel = DMA_Channel_0;
		
        stm32_spi->DMA_Stream_TX = DMA1_Stream4;//chanel 0
        stm32_spi->DMA_Stream_TX_Chanel = DMA_Channel_0;

        stm32_spi->DMA_Stream_RX_FLAG_TC = DMA_FLAG_TCIF3;
        stm32_spi->DMA_Stream_RX_FLAG_TE = DMA_FLAG_TEIF3;
        stm32_spi->DMA_Stream_TX_FLAG_TC = DMA_FLAG_TCIF4;
        stm32_spi->DMA_Stream_TX_FLAG_TE = DMA_FLAG_TEIF4;

	
#endif
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    }
    else if(SPI == SPI3)
    {
    	stm32_spi->SPI = SPI3;
#ifdef SPI_USE_DMA
        /* Enable the DMA2 Clock */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);

        stm32_spi->DMA_Stream_RX = DMA1_Stream2;//chanel 0
             stm32_spi->DMA_Stream_RX_Chanel = DMA_Channel_0;
        stm32_spi->DMA_Stream_TX = DMA1_Stream5;//chanel 0
           stm32_spi->DMA_Stream_TX_Chanel = DMA_Channel_0;

        stm32_spi->DMA_Stream_RX_FLAG_TC = DMA_FLAG_TCIF2;
        stm32_spi->DMA_Stream_RX_FLAG_TE = DMA_FLAG_TEIF2;
        stm32_spi->DMA_Stream_TX_FLAG_TC = DMA_FLAG_TCIF5;
        stm32_spi->DMA_Stream_TX_FLAG_TE = DMA_FLAG_TEIF5;
#endif
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    }
    else
    {
        return RT_ENOSYS;
    }

    return rt_spi_bus_register(&stm32_spi->parent, spi_bus_name, &stm32_spi_ops);
}
#ifdef RT_USING_SPI
#include "drv_spi.h"
#include "spi_flash_w25qxx.h"



/*
SPI1_MOSI: PB5
SPI1_MISO: PB4
SPI1_SCK : PA5
SPI1_NSS --CS : PA4 
*/
int rt_hw_spi1_init(void) // sub G
{

	    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        GPIO_InitTypeDef GPIO_InitStructure;
#if 1
     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1  , ENABLE);//使能SPI1时钟
	 
	 //GPIOFB3,4,5初始化设置
	   
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4;//PB3~5复用功能输出  
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	 
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//100MHz

	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//上拉
	 GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	   
	 GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); //PB3复用为 SPI1
	 GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1); //PB4复用为 SPI1
	 GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1); //PB5复用为 SPI1
#endif
 
   //这里只针对SPI口初始化
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//使能SPI1时钟
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//停止复位SPI1




        stm32_spi_register(SPI1, &stm32_spi, "spi1");
   
		}
    /* attach cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟

 		//cs    
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
 		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
		
        spi_cs.GPIOx    = GPIOA;
        spi_cs.GPIO_Pin = GPIO_Pin_4;

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);


		rt_spi_bus_attach_device(&spi_device, "spi10", "spi1", (void*)&spi_cs);
	      
    }
		return 0;

	 

}



/*
SPI3_MOSI: PC12
SPI3_MISO: PC11
SPI3_SCK : PC10
SPI3_CS  : PA15
//SPI3_WP  : PD2
*/
int rt_hw_spi3_init(void) // file system
{

    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        GPIO_InitTypeDef GPIO_InitStructure;

     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3 , ENABLE);//使能SPI3时钟
	 
	 //GPIOFB3,4,5初始化设置
	   
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;//PB10-12复用功能输出  
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	 GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化

	   
	 GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3); //PB10复用为 SPI3
	 GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SPI3); //PB11复用为 SPI3
	 GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3); //PB12复用为 SPI3

 
   //这里只针对SPI口初始化
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//使能SPI1时钟
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//停止复位SPI1



        stm32_spi_register(SPI3, &stm32_spi, "spi3");
    
		}
    /* attach cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;

        GPIO_InitTypeDef GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟

 		//cs    
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
 		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
		
        spi_cs.GPIOx    = GPIOA;
        spi_cs.GPIO_Pin = GPIO_Pin_15;

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);


		//wp
		/*
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOA时钟
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOD, &GPIO_InitStructure);		
				GPIO_SetBits(GPIOD, GPIO_Pin_0);
		*/	
		rt_spi_bus_attach_device(&spi_device, "spi30", "spi3", (void*)&spi_cs);
	      
    }
		return 0;

}


int rt_hw_spi_init(void)
{
	rt_hw_spi1_init();//sub G
	rt_hw_spi3_init();//wifi
	return 0;
}

INIT_BOARD_EXPORT(rt_hw_spi_init);
#endif /* RT_USING_SPI */
