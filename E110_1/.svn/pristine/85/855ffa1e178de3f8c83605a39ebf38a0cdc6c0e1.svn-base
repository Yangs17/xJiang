

#include "stm32f4xx.h"
#include "drv_uart.h"
#include "spi_flash_w25qxx.h" 




stComRxBuf	  	_Com1_Rx_Buff;


//init usart 3
//bound  57600
//OE PD10  RX TX PD8 PD9
void 	RS485_init(void)
{  	 
	
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 ,ENABLE);//使能USART1时钟

	
  //串口2引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3); //
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3); //
	
	//USART2    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化
	
	//PA8推挽输出，485模式控制  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化
	

   //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = 57600;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART3, &USART_InitStructure); //初始化串口
	
  USART_Cmd(USART3, ENABLE);  //使能串口 
	
	USART_ClearFlag(USART3, USART_FLAG_TC);

/*
#if EN_USART2_RX	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启接受中断

	//Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif	
*/	
//RS485_TX_EN=0;				//默认为接收模式	
RS485_OE_CLR();

}




int uart1_putc(char c)
{	
		GPIO_SetBits(GPIOA, GPIO_Pin_5);	
		USART_SendData(USART2, c);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
		
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
				
		return 1;
}

int	SendUart(const void  *str, int len)
{
		uint8_t *s = (uint8_t *)str;
	
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		while(len --)
		{
				while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	
				USART_SendData(USART2, *s ++);
		}
		while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);	
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);	
		return 1;
}


