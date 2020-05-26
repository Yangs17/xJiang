#include "drv_spi.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "spi_flash_w25qxx.h" 


u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
 		    
}


void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI3->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI3->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI3,ENABLE); //使能SPI1
} 




//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25X16/24L01/JF24C							  
//SPI口初始化
//这里针是对SPI3的初始化
void SPI2_Init(void)
{


		GPIO_InitTypeDef  GPIO_InitStructure;
		
		 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
		 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		 	
			 //F_CS  片选管脚 PA15  
		 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
		 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
		 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
		 GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化

		 
				
		//F_wp  片选管脚 
		 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
		 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
		 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
		 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//
		 GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
		 GPIO_SetBits(GPIOD, GPIO_Pin_2);

		   
		   SPI_CS_SET()	;	   //SPI FLASH不选中



	SPI_InitTypeDef  SPI_InitStructure;

	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOB时钟
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//使能SPI1时钟
	
	 //MISO MOSI CLK 初始化设置
	   
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;//PB3~5复用功能输出  
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	 GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
	   
	   GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SPI3); //PB3复用为 SPI1
	   GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SPI3); //PB4复用为 SPI1
	   GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3); //PB5复用为 SPI1
	
	   //这里只针对SPI口初始化
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//使能SPI1时钟
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);//停止复位SPI1
	
	   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	   //设置SPI工作模式:设置为主SPI
	   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	   //设置SPI的数据大小:SPI发送接收8位帧结构
	   SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	   //串行同步时钟的空闲状态为高电平
	   SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;    //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	   //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; 	   //定义波特率预分频的值:波特率预分频值为256
	   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	   SPI_InitStructure.SPI_CRCPolynomial = 7;    //CRC值计算的多项式
	   SPI_Init(SPI3, &SPI_InitStructure);	//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	   SPI_Cmd(SPI3, ENABLE); //使能SPI外设
	
	   SPI2_ReadWriteByte(0xff);//启动传输 
	   
		SPI1_SetSpeed(SPI_BaudRatePrescaler_2); 	 //设置为72M时钟,高速模式 
	   // W25QXX_TYPE=W25QXX_ReadID();	  //读取FLASH ID.

																								
}   







//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   (SPI 36M@sys 72M)
//SPI_BaudRatePrescaler_8   8分频   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_16  16分频  (SPI 4.5M@sys 72M)
//SPI_BaudRatePrescaler_256 256分频 (SPI 281.25K@sys 72M)

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节

u8 SPI2_ReadWriteByte(u8 TxData)
{		
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	  
	  SPI_I2S_SendData(SPI3, TxData); //通过外设SPIx发送一个byte  数据
		  
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
	
	  return SPI_I2S_ReceiveData(SPI3); //返回通过SPIx最近接收的数据   


		    
}
/*

u8 SPI2_ReadWriteByte(u8 TxData)
{		 			 

	 while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET){}//等待发送区空  
	SPI_I2S_SendData(SPI3, TxData); //通过外设SPIx发送一个byte  数据
	 
  while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET){}//等待发送区空  
	return SPI_I2S_ReceiveData(SPI3); //返回通过SPIx最近接收的数据	
 		    
}
*/
