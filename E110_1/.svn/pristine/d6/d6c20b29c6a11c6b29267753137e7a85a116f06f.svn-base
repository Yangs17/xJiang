#include "drv_spi.h"
#include "flash_if.h"

static const unsigned int		ulCrcTable[256] =
{
   0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 0x706af48fL,
   0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
   0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L, 0x1db71064L, 0x6ab020f2L,
   0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
   0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
   0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
   0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL, 0x35b5a8faL, 0x42b2986cL,
   0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
   0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L,
   0xcfba9599L, 0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
   0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L, 0x01db7106L,
   0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
   0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL,
   0x91646c97L, 0xe6635c01L, 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
   0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
   0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
   0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L,
   0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
   0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL,
   0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
   0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L,
   0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
   0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L, 0xe3630b12L, 0x94643b84L,
   0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
   0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
   0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
   0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L, 0xd6d6a3e8L, 0xa1d1937eL,
   0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
   0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L,
   0x316e8eefL, 0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
   0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL, 0xb2bd0b28L,
   0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
   0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL,
   0x72076785L, 0x05005713L, 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
   0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
   0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
   0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L,
   0x616bffd3L, 0x166ccf45L, 0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
   0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL,
   0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
   0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L, 0xcdd70693L,
   0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
   0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

 
unsigned int crc32(unsigned int crc,unsigned char *buffer, unsigned int size)    
{    
    unsigned int i;    
    for (i = 0; i < size; i++) 
		{    
        crc = ulCrcTable[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);    
    }    
    return crc ;    
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
u16 STMFLASH_GetFlashSector(u32 addr)
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

void writeByteToFlash(u32 writeAddr,u8 *data,u32 len)
{
	if(!writeAddr || !data || !len) return ;
	
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
			return ;
		}
		
		
		writeAddr++;
		data++;
		len--;
	}

	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁

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



void clearSaveDataSector()
{
	FLASH_Status status;
	int i = 0;
	u32 dataAddr = UPDATA_APP_ADDR;

	FLASH_Unlock();									//解锁 
    FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存

	//s9 ~ s11
	for(i=0;i<3;i++)
	{
		status=FLASH_EraseSector(STMFLASH_GetFlashSector(dataAddr),VoltageRange_3);
		if(status!=FLASH_COMPLETE)
			break;

		dataAddr += 0x20000;//128K
	}	

	
	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁

}

//s2~s6
void clearAppDataSector()
{
	FLASH_Status status;
	int i = 0;
	u32 dataAddr = 0;

	FLASH_Unlock();									//解锁 
    FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存

	//s2~s6
	for(i=0;i<5;i++)
	{
		switch(i)  //0 : APPLICATION_ADDRESS ---- ADDR_FLASH_SECTOR_2
		{
			case 0:
				dataAddr = APPLICATION_ADDRESS;//ADDR_FLASH_SECTOR_2
				break;
			
			case 1:
				dataAddr = ADDR_FLASH_SECTOR_3;//
				break;
			
			case 2:
				dataAddr = ADDR_FLASH_SECTOR_4;
				break;

			case 3:
				dataAddr = ADDR_FLASH_SECTOR_5;
				break;

			case 4:
				dataAddr = ADDR_FLASH_SECTOR_6;
				break;

			default:
				break;

			
		}
	
		status=FLASH_EraseSector(STMFLASH_GetFlashSector(dataAddr),VoltageRange_3);
		if(status!=FLASH_COMPLETE)
			break;


	}	

	
	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
}

 
#define BUFSIZE     1024 
static  u8 buf[BUFSIZE]; 
u8 calc_flash_crc(unsigned int addr,unsigned int size)
{
	unsigned int crc = 0xffffffff;
	unsigned int readAddr = addr;
	unsigned int len = 0;
	//at_printf("addr = %d\r\n",addr);
	while(1)
	{
		if(size > BUFSIZE)	
	    	len = BUFSIZE;	
		else
			len = size;
		
		size -= len;	

		STMFLASH_ReadMoreByte(readAddr,buf,len); 
		readAddr += len;
		
		crc = crc32(crc, buf, len);
      	//rt_kprintf("len = %d,crc32 =  %d.size = %d\n", len,crc,size); 
		
		if(!size) break;

	}

	if(!crc) return 0;
	else	return 1;
	
}

/* 0 : OK */
u8 checkFlashFile()
{

	u32 flen = 0;
	u32 startAddr = UPDATA_APP_ADDR;
	
	STMFLASH_Read(startAddr,&flen,1);
	//rt_kprintf("flen = %d \r\n",flen);
	
	if(flen && (flen != 0xffffffff))
	{
		return calc_flash_crc(startAddr+4,flen);
	}

	return 1;
}


