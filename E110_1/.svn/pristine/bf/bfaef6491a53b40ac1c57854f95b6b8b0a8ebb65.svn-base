#include "setpool.h"

#define POOL_MUTEX_TIMEOUT 2*1000
rt_err_t ret = 0;



//申请
pool_setting *InitDataPool(uint32_t poolSize,const char* name)
{
	if(!poolSize) return RT_NULL;
	pool_setting *poolSetting = (pool_setting *)rt_malloc(sizeof(pool_setting));
	if(!poolSetting)
	{
		rt_free(poolSetting);
		return RT_NULL;
	}

	poolSetting->poolHead = rt_malloc(poolSize);
	if(!poolSetting->poolHead)
	{
		rt_free(poolSetting->poolHead);
		rt_free(poolSetting);
		return RT_NULL;
	}

	poolSetting->poolSize  = poolSize;
	poolSetting->entrySize = 0; 


	poolSetting->poolMutex = rt_mutex_create (name, RT_IPC_FLAG_FIFO);
	if(RT_NULL == poolSetting->poolMutex)
	{
		rt_free(poolSetting->poolHead);
		rt_free(poolSetting);
		rt_kprintf("thread set pool mutex is err!\r\n");
		return RT_NULL;	
	}

	rt_memset(poolSetting->poolHead,0,poolSize);
	
	return poolSetting;
	
}

//发送   返回 保存的起始位置 
int  WritePool(pool_setting *poolSetting,void *addBuff,uint16_t writeSize)
{
	if(!poolSetting || !addBuff || !writeSize) return -RT_ERROR;

	ret = rt_mutex_take(poolSetting->poolMutex,POOL_MUTEX_TIMEOUT);
	if(ret != RT_EOK)
	{
		at_printf("write pool 2 = %d,ret = %d,owner = %s\r\n",
			poolSetting->poolMutex->value,ret,poolSetting->poolMutex->owner->name);
		//rt_mutex_release(poolSetting->poolMutex);
		return -RT_ERROR;
	}
	

	
	if((poolSetting->entrySize + writeSize) > poolSetting->poolSize)
	{
		at_printf("poolSetting->entrySize  = %d,writeSize = %d",poolSetting->entrySize,writeSize);
		rt_mutex_release(poolSetting->poolMutex);
		return -RT_ERROR;
	}

	uint32_t savePlace = poolSetting->entrySize;

	rt_memcpy(poolSetting->poolHead+poolSetting->entrySize,(uint8_t *)addBuff,writeSize);
	poolSetting->entrySize += writeSize;
	
	rt_mutex_release(poolSetting->poolMutex);
	//rt_kprintf("mutex realase! \r\n");
	return savePlace;
}

//接收
rt_err_t ReadPool(pool_setting *poolSetting,uint32_t readPlace,uint32_t readSize,void *saveBuff)
{
	if(!poolSetting || !readSize || !saveBuff) return -RT_ERROR;	

	ret = rt_mutex_take(poolSetting->poolMutex,POOL_MUTEX_TIMEOUT);
	if(ret != RT_EOK)
	{
		at_printf("read pool 2 = %d,ret = %d,owner = %s\r\n",
			poolSetting->poolMutex->value,ret,poolSetting->poolMutex->owner->name);
		//rt_mutex_release(poolSetting->poolMutex);
		return -RT_ERROR;
	}
	
	
	if((readPlace + readSize) >  poolSetting->entrySize) 
	{
		rt_mutex_release(poolSetting->poolMutex);
		return -RT_ERROR;
	}
	

	rt_memcpy((uint8_t *)saveBuff,poolSetting->poolHead+readPlace,readSize);
	data_left(poolSetting->poolHead+readPlace, readSize, poolSetting->poolSize-readPlace);
	poolSetting->entrySize -= readSize;	

	rt_mutex_release(poolSetting->poolMutex);
	//rt_kprintf("mutex realase! \r\n");
	return RT_EOK;
}


//清空
void cleanPool(pool_setting *poolSetting)
{
	if(!poolSetting) return;
	rt_mutex_take (poolSetting->poolMutex,RT_WAITING_FOREVER);
	rt_memset(poolSetting->poolHead,0,poolSetting->poolSize);
	poolSetting->entrySize = 0;
	rt_mutex_release(poolSetting->poolMutex);
}



//销毁
void Destroy(pool_setting *poolSetting)
{
	if(!poolSetting) return;

	poolSetting->entrySize = 0;
	poolSetting->poolSize  = 0;
	
	rt_free(poolSetting->poolHead);
	poolSetting->poolHead = RT_NULL;

	rt_free(poolSetting);
	poolSetting = RT_NULL;
	
}

