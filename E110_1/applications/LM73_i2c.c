#include "LM73_i2c.h"

#if 0 

#define I2C1_GPIO_Type_SDA  GPIOC
#define I2C1_GPIO_Pin_SDA   GPIO_Pin_9

#define I2C1_GPIO_Type_SCL  GPIOA
#define I2C1_GPIO_Pin_SCL   GPIO_Pin_8


// PA8 clk  PC9 data
// LM_73_EN PC8  低电平有效
#define IIC_SCL()    GPIO_SetBits(I2C1_GPIO_Type_SCL,I2C1_GPIO_Pin_SCL)//SCL
#define READ_SCL()    GPIO_ResetBits(I2C1_GPIO_Type_SCL,I2C1_GPIO_Pin_SCL)//SCL 

#define IIC_SDA()   GPIO_SetBits(I2C1_GPIO_Type_SDA,I2C1_GPIO_Pin_SDA)//SDA
#define READ_SDA()  GPIO_ResetBits(I2C1_GPIO_Type_SDA,I2C1_GPIO_Pin_SDA)//SDA


static void RCC_Configuration(void)
{

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOB时钟

}

static void GPIO_Configuration(void)
{
	   GPIO_InitTypeDef  GPIO_InitStructure;
	  //GPIOB8,B9初始化设置
	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化 
	

	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化 
	
#ifdef RT_LM73
	
	  //Enable
	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
	  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化 
	  
#endif 


}



//
void set_sda(void *data, rt_int32_t state)
{
    if(state == 1)
      IIC_SDA();//SDA
    else if(state == 0)
      READ_SDA();//SDA
}

void set_scl(void *data, rt_int32_t state)
{
    if(state == 1)
      IIC_SCL();
    else if(state == 0)
       READ_SCL();
}

rt_int32_t get_sda(void *data)
{
    return (rt_int32_t)GPIO_ReadInputDataBit(I2C1_GPIO_Type_SDA,I2C1_GPIO_Pin_SDA);//return(GPIOB->IDR  & I2C1_GPIO_SDA)
}

rt_int32_t get_scl(void *data)
{
    return (rt_int32_t)GPIO_ReadInputDataBit(I2C1_GPIO_Type_SCL,I2C1_GPIO_Pin_SCL);//return(GPIOB->IDR  & I2C1_GPIO_SCL)
}

void udelay(rt_uint32_t us)
{
	rt_thread_delay(us);
}



/*
*********************************************************************************************************
*	函 数 名: rt_hw_i2c_init
*	功能说明: 初始化I2C
*	形    参: void
*	返 回 值: 无	             
*********************************************************************************************************
*/
static const struct  rt_i2c_bit_ops stm32_i2c_bit_ops =
{
    set_sda,
    set_scl,
    get_sda,
    get_scl,
	udelay,

};


/*
*********************************************************************************************************
*	函 数 名: rt_hw_i2c_init
*	功能说明: 初始化I2C
*	形    参: void
*	返 回 值: 无	             
*********************************************************************************************************
*/
int rt_hw_i2c_init(void)
{
	//"static" add by me. It must be add "static", or it will be hard fault
    static struct rt_i2c_bus_device stm32_i2c;
	
    RCC_Configuration();
    GPIO_Configuration();
    

   
    rt_memset((void *)&stm32_i2c, 0, sizeof(struct rt_i2c_bus_device));
    stm32_i2c.priv = (void *)&stm32_i2c_bit_ops;

   
    rt_i2c_bit_add_bus(&stm32_i2c, "i2c1");  

	return RT_EOK;

}
INIT_BOARD_EXPORT(rt_hw_i2c_init);




#ifdef RT_LM73

static rt_device_t i2c_device;

/*
*********************************************************************************************************
*	函 数 名: I2C_LM73_ByteWrite
*	功能说明: 向LM73写入1Byte数据
*	形    参: Buffer：字节  WriteAddr:地址
*	返 回 值: 无	             
*********************************************************************************************************
*/
void I2C_LM73_ByteWrite(BYTE Buffer, BYTE WriteAddr)
{
    BYTE sdata = 0x92; //0x92为写LM73的设备地址，该地址由ADDR的连接方式来决定
    if(i2c_device == RT_NULL)
		return;
	
	rt_device_write(i2c_device, 0, &sdata, 1);
	rt_device_write(i2c_device, 0, &WriteAddr, 1);
	rt_device_write(i2c_device, 0, &Buffer, 1);
}




/*
*********************************************************************************************************
*	函 数 名: I2C_LM73_BufferRead
*	功能说明: 从LM73中读取两字节数据
*	形    参: pBuffer：读取到的字节 ReadAddr：读取的地址  ack:有无应答
*	返 回 值: 无	             
*********************************************************************************************************
*/
void I2C_LM73_BufferRead(BYTE* pBuffer, u8 ReadAddr,u8 ack)
{


	BYTE sdata = 0x92; //0x92为写LM73的设备地址，该地址由ADDR的连接方式来决定
	BYTE sdata1 = (0x92|0x01);
	BYTE* pbuf = pBuffer;
	
    if(i2c_device == RT_NULL)
		return;

	rt_device_write(i2c_device, 0, &sdata, 1);
    rt_device_write(i2c_device, 0, &ReadAddr, 1);
	rt_device_write(i2c_device, 0, &sdata1, 1);

    rt_device_read(i2c_device,0,pbuf,ack);
    pbuf++;
    rt_device_read(i2c_device,0,pbuf,0);

}






/*
*********************************************************************************************************
*	函 数 名: I2C_LM73_ByteWrite
*	功能说明: 向LM73写入1Byte数据
*	形    参: Buffer：字节  WriteAddr:地址
*	返 回 值: 无	             
*********************************************************************************************************
*/
void LM73_Init(void)
{
	u8 temp[2] = {0, 0};
	u16 temp_ID = 0;

	IIC_SDA();
	IIC_SCL();
	rt_thread_delay(5);

	I2C_LM73_ByteWrite(PRECISION_14,RP_CS); //将LM73设置为14位精度
	I2C_LM73_BufferRead(&temp[0],RP_ID,1);
	temp_ID=temp[0];
	temp_ID<<=8;
	temp_ID|=temp[1];
	while(temp_ID!=LM730_ID);
	
}


WORD ReadTemp()
{
	u8 temp[2] = {0, 0};
	WORD temperatrue = 0;
	WORD Val_temperatrue = 0 ;
	I2C_LM73_BufferRead(&temp[0],RP_TEM,1);
	temperatrue=temp[0];
	temperatrue<<=8;
	temperatrue|=temp[1];
//	LM73_new = 1;
	if(temperatrue & 0x8000)
	{
		Val_temperatrue = (WORD)((float) (( ~temperatrue + 1 ) >> 2) * 0.03125 *10 + 0.5 ) ;
		Val_temperatrue |= 0x8000;
	}
	else
	{
		Val_temperatrue =  (WORD)((float) (( temperatrue & 0x7FFF ) >> 2) * 0.03125 *10 + 0.5) ;
	}	
	return Val_temperatrue;
}




/*
*********************************************************************************************************
*	函 数 名: rt_hw_LM73_i2c_init
*	功能说明: 初始化LM73
*	形    参: void
*	返 回 值: 无	             
*********************************************************************************************************
*/
int rt_hw_LM73_i2c_init(void)
{
	//struct rt_i2c_bus_device * i2c_bus = (struct rt_i2c_bus_device *)rt_device_find("i2c1");
	i2c_device =  rt_device_find("i2c1");
	
	if(i2c_device == RT_NULL)
	{    
	   rt_kprintf("\ni2c is not found!\n");
	   return -RT_ENOSYS;
	}

    if(RT_EOK !=  rt_device_open(i2c_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) )	
    {
         rt_kprintf("\ni2c is opened failed!\n");
         return -RT_EEMPTY;
    }

	rt_kprintf("i2c is OK!\n");

    
	//LM73_Init();

   
    //WORD temperatrue =  ReadTemp();
	//rt_kprintf("temperatrue = %d\r\n",temperatrue);
	return RT_EOK;

	
}
//INIT_APP_EXPORT(rt_hw_LM73_i2c_init);
#endif   
#endif

