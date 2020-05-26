#ifndef _SET_POOL_H__
#define _SET_POOL_H__



#include <rtthread.h>
#include <rthw.h>


typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;


typedef struct _pool_setting
{
	uint8_t	*	poolHead; //申请的数据池
	uint32_t    poolSize; //池子大小
	uint32_t 	entrySize; //池子中已经存在的数据大小
	rt_mutex_t  poolMutex; //互斥锁保护临界资源
}pool_setting;


//申请
pool_setting *InitDataPool(uint32_t poolSize,const char* name);

//发送
int WritePool(pool_setting *poolSetting,void *addBuff,uint16_t writeSize);

//接收
rt_err_t ReadPool(pool_setting *poolSetting,uint32_t readPlace,uint32_t readSize,void *saveBuff);

//清空
void cleanPool(pool_setting *poolSetting);


#endif //_SET_POOL_H__
