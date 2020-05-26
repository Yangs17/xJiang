#ifndef _SET_POOL_H__
#define _SET_POOL_H__



#include <rtthread.h>
#include <rthw.h>


typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;


typedef struct _pool_setting
{
	uint8_t	*	poolHead; //��������ݳ�
	uint32_t    poolSize; //���Ӵ�С
	uint32_t 	entrySize; //�������Ѿ����ڵ����ݴ�С
	rt_mutex_t  poolMutex; //�����������ٽ���Դ
}pool_setting;


//����
pool_setting *InitDataPool(uint32_t poolSize,const char* name);

//����
int WritePool(pool_setting *poolSetting,void *addBuff,uint16_t writeSize);

//����
rt_err_t ReadPool(pool_setting *poolSetting,uint32_t readPlace,uint32_t readSize,void *saveBuff);

//���
void cleanPool(pool_setting *poolSetting);


#endif //_SET_POOL_H__
