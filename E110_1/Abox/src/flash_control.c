#include "abox.h"




/* 获取app版本 */
void getAppVertion(rt_uint32_t *ver)
{
	if(ver)
	{
		STMFLASH_Read(APP_START_ADDR+4*7,(u32 *)ver,1); 
	}
}


//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}  
//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
rt_uint16_t STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}

/* sector1；save data. 0~1K：save important data. 1K~2K: save server informatuion. */
void setSecret_1(u32 WriteAddr,rt_uint8_t *pBuffer)
{

	if(FLASH_Sector_1 == STMFLASH_GetFlashSector(WriteAddr))
	{		
		stSysSetup 			  setup_temp;
		freq_information      freqInfo;
		serverNet_information serNetInfo;

		rt_uint32_t setupNum  = sizeof(setup_temp);
		rt_uint32_t freqNum   = sizeof(freqInfo);
		rt_uint32_t serverNum = sizeof(serNetInfo);

		rt_memset(&setup_temp,0,setupNum);
		rt_memset(&freqInfo,0,freqNum);
		rt_memset(&serNetInfo,0,serverNum); 

		STMFLASH_ReadMoreByte(DATA_ADDR,(u8 *)(&setup_temp),setupNum);
		STMFLASH_ReadMoreByte(DATA_FREQ_ADDR,(u8 *)(&freqInfo),freqNum);
		STMFLASH_ReadMoreByte(SERVER_NET_INFO_ADDR,(u8 *)(&serNetInfo),serverNum);


		/* get data */
		switch(WriteAddr)
		{
			case DATA_ADDR:
				rt_memcpy(&setup_temp,(stSysSetup *)pBuffer,setupNum);
			break;

			case DATA_FREQ_ADDR:
				rt_memcpy(&freqInfo,(freq_information *)pBuffer,freqNum);
			break;

			case SERVER_NET_INFO_ADDR:
				rt_memcpy(&serNetInfo,(serverNet_information *)pBuffer,serverNum);
			break;	

			default:
				break;
							
		}
		
		FLASH_Unlock();									//解锁 
    	FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存

		FLASH_EraseSector(FLASH_Sector_1,VoltageRange_3);	

		FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
		FLASH_Lock();//上锁
		
		writeByteToFlash(DATA_ADDR,(u8 *)(&setup_temp),setupNum);
		writeByteToFlash(DATA_FREQ_ADDR,(u8 *)(&freqInfo),freqNum);
		writeByteToFlash(SERVER_NET_INFO_ADDR,(u8 *)(&serNetInfo),serverNum);		

	}

}


//从指定地址开始写入指定长度的数据
//特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数
//         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
//         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
//         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
//该函数对OTP区域也有效!可以用来写OTP区!
//OTP区域地址范围:0X1FFF7800~0X1FFF7A0F
//WriteAddr:起始地址(此地址必须为4的倍数!!)
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数.) 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
	
	volatile u32 addrx=0;
	volatile u32 endaddr=0;	
	
	/* 关闭中断*/
	//rt_base_t level;
	//level = rt_hw_interrupt_disable();
//  	FLASH_Status status = FLASH_COMPLETE;
	
	FLASH_Unlock();									//解锁 
    FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
	addrx=WriteAddr;				//写入的起始地址
	endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址


		while(WriteAddr<endaddr)//写数据
		{
			if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)//写入数据
			{ 
				break;	//写入异常
			}
			WriteAddr+=4;
			pBuffer++;
		} 
		
  	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
	/* 恢复中断*/
	//rt_hw_interrupt_enable(level);
} 

u32 STMFLASH_ReadByte(u32 faddr)
{
	return *(vu8*)faddr; 
}  

void STMFLASH_ReadMoreByte(u32 ReadAddr,u8 *pBuffer,u32 NumToRead)   	
{
	u32 i;

	for(i=0;i<NumToRead;i++)
	{		
	   pBuffer[i]=STMFLASH_ReadByte(ReadAddr);//读取1个字节.
	   ReadAddr+=1;//偏移1个字节.
	}

}

rt_err_t writeByteToFlash(rt_uint32_t writeAddr,rt_uint8_t *data,rt_uint32_t len)
{
	if(!writeAddr || !data || !len) return RT_ERROR;
	
	FLASH_Status status;
	
	FLASH_Unlock();									//解锁 
    FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存

	//FLASH->CR |= FLASH_PSIZE_BYTE;

	while(len)
    {	
    	status = FLASH_ProgramByte(writeAddr,*data);
		
		if(status !=FLASH_COMPLETE)//写入数据
		{
			FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
			FLASH_Lock();//上锁
			return RT_ERROR;
		}
		
		
		writeAddr++;
		data++;
		len--;
	}

	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁

	return RT_EOK;

}


//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(4位)数
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;

	for(i=0;i<NumToRead;i++)
	{		
	   pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
	   ReadAddr+=4;//偏移4个字节.
	}

}



rt_err_t clearSaveDataSector(void)
{
	FLASH_Status status;
	int i = 0;
	rt_uint32_t dataAddr = UPDATA_APP_ADDR;

	FLASH_Unlock();									//解锁 
    FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
	
	for(i=0;i<3;i++)
	{
		status=FLASH_EraseSector(STMFLASH_GetFlashSector(dataAddr),VoltageRange_3);
		if(status!=FLASH_COMPLETE)
			break;

		dataAddr += 0x20000;//128K
	}	

	
	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
	
	return RT_EOK;
}



extern rt_err_t calc_flash_crc(unsigned int addr,unsigned int size);
rt_err_t checkFlashFile()
{

	rt_uint32_t flen = 0;
	u32 startAddr = UPDATA_APP_ADDR;
	
	STMFLASH_Read(startAddr,(u32 *)(&flen),1);
	rt_kprintf("flen = %d \r\n",flen);
	
	if(flen && (flen != 0xffffffff))
	{
		return calc_flash_crc(startAddr+4,flen);
	}

	return RT_ERROR;
}


