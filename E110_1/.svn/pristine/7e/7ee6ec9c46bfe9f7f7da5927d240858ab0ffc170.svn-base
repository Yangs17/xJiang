/**
  ******************************************************************************
  * @file    STM32F4xx_IAP/src/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    10-October-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/** @addtogroup STM32F4xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include		"stdint.h"
#include 		<stdlib.h>
#include		"flash_if.h"
#include    "stm32f4xx.h"
#include		"drv_uart.h"
#include 		"ff.h" 
#include 		"drv_spi.h" 
#include 		"spi_flash_w25qxx.h"
#include 		"main.h"
#include		"misc.h"
//#include 		"menu.h"
#include	  "stdio.h"

#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)

/* Private define ------------------------------------------------------------*/
#define 	SYSTEMTICK_PERIOD_MS  10
uint32_t 	timingdelay;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */


FATFS 		Fatfs;  
FILINFO 	Finfo;	
char 		Lfname[512];

typedef  void (*pFunction)(void);

pFunction 			 Jump_To_Application;
uint32_t 				 JumpAddress;
uint32_t			app_address;

extern void Main_Menu(void);


void	IAP_BOOT(void)//0x08020000 
{
	
		if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
		{ 
				/* Jump to user application */
				JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
				Jump_To_Application = (pFunction) JumpAddress;
				/* Initialize user application's Stack Pointer */			
				__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);

				/*disable interrupt before jump to application, add by rch, 2017.09.05.*/
				__set_PRIMASK(1);
				Jump_To_Application();
		}	
//SendUart("open err\n", 9);		
}




#if 1
//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;

	RS485_OE_SET();
  	for(t=0;t<len;t++)		//循环发送数据
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //等待发送结束		
    USART_SendData(USART3,buf[t]); //发送数据
	}	 
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //等待发送结束		  
	RS485_OE_CLR();

}



#endif

//PCB ID0 ---PB6
//PCB ID1 ---PB3
//PCB ID2 ---PD7
void Read_hard_status(char *st)
{
	
	  GPIO_InitTypeDef  GPIO_InitStructure;

	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD, ENABLE);

	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
	  GPIO_Init(GPIOB, &GPIO_InitStructure);

	  
	  //PD7
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	st[0] = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6) ;//PE4--ID0
    st[1] = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3);	//PC8--ID1
	st[2] = GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7) ;//PB5--ID2
	  
}



#define BUFSIZE     1024 
static  u8 buf[BUFSIZE]; 

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
 {	

		char PCB_status[3] = {0};
		app_address = APPLICATION_ADDRESS;
		
	
		RCC_DeInit();//为了从IAP程序跳来运行APP的时候正常开始，初始化时要恢复RCC为复位状态，恢复NVIC为复位状态	
	
		RS485_init();//485 设置

		//Jump the address based on the version number you read.
		Read_hard_status(PCB_status);

		switch(PCB_status[0] | (PCB_status[1]<<1) | (PCB_status[2]<<2) )
		{
			case 7:
				RS485_Send_Data("The PCB version；7\r\n",20);
				app_address = APPLICATION_ADDRESS;
				break;
			
			default:
				break;
		}
		

		//s0: bootloader  16k
		//s1: savedata    16K
		//s2~s6:  app	  16+16+64+128+128
		//s9~s11: saveapp 128+128+128

		u32 readAddr  = UPDATA_APP_ADDR;
		u32 writeAddr = APPLICATION_ADDRESS;
		u32 fileLen = 0;
		STMFLASH_Read(readAddr,&fileLen,1);
		
		if(fileLen && (fileLen != 0xffffffff))
		{
			if(checkFlashFile()) 
			{
				/* check err */
				clearSaveDataSector();
				goto loop;
			}

		
			readAddr += 4;
			clearAppDataSector();
			while(1)
			{
			
				if(fileLen > BUFSIZE)
				{
					STMFLASH_Read(readAddr,(u32 *)buf,BUFSIZE/4);
					STMFLASH_Write(writeAddr,(u32 *)buf,BUFSIZE/4);

					fileLen   -= BUFSIZE;
					readAddr  += BUFSIZE;
					writeAddr += BUFSIZE;
				}
				else
				{
					STMFLASH_ReadMoreByte(readAddr,buf,fileLen);
					writeByteToFlash(writeAddr,buf,fileLen);
					RS485_Send_Data("recover data sector is ok! \r\n",writeAddr);

					clearSaveDataSector();
					break;
				}
							
			}

		}

loop:
	
		IAP_BOOT();//main
		return 0;
}



void Delay(uint32_t nCount)
{
  /* Capture the current local time */
  timingdelay = LocalTime + nCount;  

  /* wait until the desired delay finish */  
  while(timingdelay > LocalTime)
  {     
  }
}

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

