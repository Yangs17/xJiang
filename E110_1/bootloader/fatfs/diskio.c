/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

#include "spi_flash_w25qxx.h"
//#include "malloc.h"		


#define W25QXX 		0			//外部W25QXX,卷标为0


//此处自定义
#define FLASH_SECTOR_SIZE 	4096			  
#define FLASH_BLOCK_SIZE   	4096     	//每个BLOCK有8个扇区
#define	FLASH_SECTOR_COUNT	2048			//w25q64
//初始化磁盘
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{	    
	switch(pdrv)
	{
		case 	W25QXX://外部W25QXX
			SPI_Flash_Init();
			return RES_OK;
	}		 
	return  STA_NOINIT;
}  

//获得磁盘状态
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{ 
	return 0;
} 

//读扇区
//drv:磁盘编号0~9
//*buff:数据接收缓冲首地址
//sector:扇区地址
//count:需要读取的扇区数
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
		switch(pdrv)
		{

			case W25QXX://外部W25QXX
				for(; count > 0; count--)
				{
					SPI_Flash_Read(buff, sector * FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
					sector ++;
					buff += FLASH_SECTOR_SIZE;
				}
				return RES_OK;
		}
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
		return RES_ERROR;	   
}

//写扇区
//drv:磁盘编号0~9
//*buff:发送数据首地址
//sector:扇区地址
//count:需要写入的扇区数
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{	

    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
		switch(pdrv)
		{
			case W25QXX://外部W25QXX
				for( ; count > 0; count--)
				{										    
					SPI_Flash_Write((u8*)buff, sector * FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
					sector ++;
					buff += FLASH_SECTOR_SIZE;
				}
				return RES_OK;
		}
		return RES_ERROR;	
}
#endif


//其他表参数的获得
 //drv:磁盘编号0~9
 //ctrl:控制代码
 //*buff:发送/接收缓冲区指针
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
			DRESULT res;						  			     
	    switch(cmd)
	    {
		    case CTRL_SYNC:
						res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = FLASH_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }    
			return res;
}
#endif
//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}			 

