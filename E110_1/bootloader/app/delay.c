#include 		"stm32f4xx.h"



 static __IO uint32_t TimingDelay;

	 /****************************************
	 *SystemCoreClock/100       10ms中断一次  *
	 *SystemCoreClock/1000      1ms中断一次  *
    *SystemCoreClock/10000     100us中断一次 *
	 *SystemCoreClock/100000    10us中断一次 *
	 *SystemCoreClock/1000000   1us中断一次  *
	 
	 
	 *SysTick_Config(SystemCoreClock/1000)  函数为core_cm4.h中定义的设
	 *置节拍时钟的函数，当设置成功时，返回0，失败返回1
	 
	 
	 *****************************************/



/***************************************************************************
*函数名：SysTick_Init()
*参数：void
*返回值:void
*功能：系统节拍时钟初始化函数
*****************************************************************************/
void SysTick_Init(void)
{
	RCC_ClocksTypeDef    RCC_Clocks; 
	/****************************************
	 *SystemFrequency/1000      1ms中断一次  *
	 *SystemFrequency/100000    10us中断一次 *
	 *SystemFrequency/1000000   1us中断一次  *
	 *****************************************/
	//RCC->APB2ENR |= (1<<14);  //使能syscfg时钟
  // while(SysTick_Config(SystemCoreClock/1000)!=0);    //设置系统节拍时钟为1ms中断一次

  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
	while(SysTick_Config(RCC_Clocks.HCLK_Frequency/1000)!=0); 	   //设置系统节拍时钟为1ms中断一次
	

	
	/*附加笔记：
	SysTick_Config(uint32_t ticks)函数为core_cm4.h中定义的设置节拍时钟的函数，当设置成功时，返回0，失败返回1
	*/
}

/***************************************************************************
*函数名：TimeTick_Decrement()
*参数：void
*返回值:void
*功能：系统节拍时钟中断计数，给系统节拍中断函数调用
*****************************************************************************/ 
void TimeTick_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


 /***************************************************************************
*函数名：delay_1ms()
*参数：void
*返回值:void
*功能：使用系统节拍时钟精准延时1个毫秒函数
*****************************************************************************/
void delay_1ms(void)
{
	TimingDelay = 1;
	while(TimingDelay != 0);
}

/***************************************************************************
*函数名：delay_ms()
*参数：u32 n  时钟延时的毫秒数
*返回值:void
*功能：系统节拍时钟初始化函数
*****************************************************************************/
void delay_ms(u32 nms)
{
  while(nms--)
  {
	  delay_1ms();
  }
}






