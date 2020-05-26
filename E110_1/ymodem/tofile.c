/*
 * File      : tofile.c
 * ZHOUSAN ETENG CO.,LTD
 * COPYRIGHT (C) 2016 - 2026
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-15     sea_start       the first version
*/
#include <rtthread.h>
#include <ymodem.h>
#include <dfs_posix.h>
#include <stdlib.h>
#include "abox.h"
#include <board.h>

typedef enum 
{
	FILE_UPDATA,
	FILE_NET_CONFIG,

	FILE_CHECK_ERROR,
	FILE_CHECK_OK,
}file_type;
file_type fileType;

struct custom_ctx
{
    struct rym_ctx parent;
    u32    flen;
   	u32    fileAddr;
};
extern void at_print_ctrl(int off);



int search_ch(rt_uint8_t *sbuf,int bsize,char ch)
{
	int i =0;
	for(i=0;i<bsize;i++)
	{
		if(sbuf[i] == ch)
			return i;
	}

	return -RT_ERROR;
	
}

char temp[1024];
static enum rym_code _rym_bg(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    struct custom_ctx *cctx = (struct custom_ctx*)ctx;
	rt_memset(temp,0,1024);
	rt_memcpy(temp,buf,len);

	 
	
	//clearSaveDataSector();
	rt_uint8_t zeroPlace = 0;
	zeroPlace = search_ch(buf,128,0);//"est.bin"0"179380"00000
    cctx->flen = atoi((char *)buf+zeroPlace+1);   
	cctx->fileAddr = UPDATA_APP_ADDR+4;
	fileType = FILE_UPDATA;

	if(!rt_memcmp(buf,NET_FILE_NAME,strlen(buf)))
	{
		fileType = FILE_NET_CONFIG;
	}
    return RYM_CODE_ACK;
}

static enum rym_code _rym_tof(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    struct custom_ctx *cctx = (struct custom_ctx*)ctx;
	rt_memset(temp,0,1024);
	rt_memcpy(temp,buf,len);

	if(fileType == FILE_UPDATA)
	{
		writeByteToFlash(cctx->fileAddr,buf,len);
		cctx->fileAddr += len;
	}

    return RYM_CODE_ACK;
}

static enum rym_code _rym_end(
        struct rym_ctx *ctx,
        rt_uint8_t *buf,
        rt_size_t len)
{
    struct custom_ctx *cctx = (struct custom_ctx*)ctx;
	rt_memset(temp,0,1024);
	rt_memcpy(temp,buf,1024);

	switch(fileType)
	{
		case FILE_UPDATA:
			writeByteToFlash(cctx->fileAddr,buf,len);
			cctx->fileAddr += len;

			writeByteToFlash(UPDATA_APP_ADDR,(u8 *)&cctx->flen,4);
			at_printf("ry fileLen = %d \r\n",cctx->flen);

			if (RT_EOK == checkFlashFile())
			{
				rt_kprintf("check file is OK!\r\n");
				rt_thread_delay(500);
				fileType = FILE_CHECK_OK;
			}
			else
				clearSaveDataSector();

			
		break;

		case FILE_NET_CONFIG:
			{
				serverNet_information serNetConfig;
				
				if(RT_EOK == handleSerNetConfigFile(&serNetConfig,temp,cctx->flen))
				{
					at_printf(" connect info : %d \r\n ",serNetConfig.netConfig.configStyle);
				
					controlServerInfo(WRITE_NETCONFIG_INFO,&serNetConfig);
					fileType = FILE_CHECK_OK;

					//cmd_reboot();
				}
					
				
			}
		break;

		default:
				break;
	}


    return RYM_CODE_ACK;
}

rt_err_t rym_write_to_file(rt_device_t idev)
{
    rt_err_t res;
    struct custom_ctx *ctx = rt_malloc(sizeof(*ctx));

    RT_ASSERT(idev);
    res = rym_recv_on_device(&ctx->parent, idev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                             _rym_bg, _rym_tof, _rym_end, 1000);


    rt_free(ctx);

    return res;
}

#include <finsh.h>
#include "mydef.h"
rt_err_t cmd_ry(int argc, char** argv)
{
   rt_err_t res;
	//rt_kprintf("here is rz!\n");
	char AT_status = Cdma_Info.AT_status;
	Cdma_Info.AT_status = AT_YMODEM_UPDATA;	
    rt_device_t dev = rt_console_get_device();
    if (!dev)
    {
		//rt_kprintf("could not find device:%s\n", RT_CONSOLE_DEVICE_NAME);
		rt_kprintf("could not get console device\n");
		Cdma_Info.AT_status = AT_status;
        return -RT_ERROR;
    }
	wwdg_auto_open();
	at_print_ctrl(1);
    res = rym_write_to_file(dev);
	at_print_ctrl(0);

	if(fileType == FILE_CHECK_OK)
	{
		cmd_connectInfo();		
		cmd_reboot();
	}

	wwdg_auto_close();
	Cdma_Info.AT_status = AT_status;
	return res;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_ry, __cmd_ry, receive files by ymodem protocol.);




