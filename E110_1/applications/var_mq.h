#ifndef _VAR_MQ_H_
#define _VAR_MQ_H_

#include <rtthread.h>
#include <rthw.h>


/*******************************************************************
 * @copyright www.esto.cn
 * @author Yangs
 * @date 22-April-2019
 * @brief The functions in this file are used for some dynamically requested message queue information,
 		   and are suitable for RTT systems where most of the type information needs to be changed if other
 		   systems are required.
 		   important: This message queue keeps the principle of first-in and first-out.
 		   
 * @history 
 *******************************************************************/

/* RTT set */
#define VAR_NULL 		(0)   //NULL
#define VAR_EOK 		 0   /**< There is no error */
#define	VAR_ERROR		 1	 /**< A generic error happens */


#define VarInterruptDisablle()  		rt_hw_interrupt_disable()
#define	VarInterruptEnable(level) 		rt_hw_interrupt_enable(level)
#define	VarMalloc(size) 				rt_malloc(size)
#define	VarFree(rmem)					rt_free(rmem)
#define VarMemcpy(dest,src,n) 			rt_memcpy(dest,src,n)
#define VarMemset(src,c,n)				rt_memset(src,c,n)

//sem
#define VarInitSem(sem,name,value,flag)	rt_sem_init(sem,name,value,flag)
#define VarDetchSem(sem)				rt_sem_detach(sem)
#define VarTakeSem(sem,time)			rt_sem_take(sem,time)
#define	VarRealseSem(sem)				rt_sem_release(sem)


typedef rt_uint8_t		var_u8;
typedef rt_uint16_t		var_u16;
typedef rt_uint32_t 	var_u32;
typedef	rt_err_t		varErr;	
typedef struct rt_semaphore		varSem;
/* RTOS set end */



#define VARSIZE 		3*1024  //buff size
#define VARARRAYSIZE    20

struct _list_info
{ 
	var_u32	pDataAddr;
	var_u16 msgLength;	
};
typedef struct _list_info list_info;



struct _var_messagequeue
{
	var_u16     entrySize; 					/* 已存储的数据大小*/
	var_u8      msgPool[VARSIZE]; 			/* 存放消息的消息池开始地址*/

	var_u16 	entryNum;					/* 已存储的数据条数 */
	list_info   mqArray[VARARRAYSIZE];		/* 存储的数据信息 */

	varSem    mqSem;					    /* 信号量控制阻塞 */
	
};
typedef struct _var_messagequeue  var_mq;
typedef struct _var_messagequeue* var_mq_t;




/*******自封装函数********/
var_mq_t creatMsg(const char* name);
varErr sendMsg(var_mq_t mq,void* buffer,var_u16 size);
varErr recvMsg(var_mq_t mq,void *buffer,var_u16 size,int time);

/***************/





//申请
var_mq_t CreatVarMq(const char* name);

//发送
varErr SendVarMq(var_mq_t mq,void* buffer, var_u16 size);

//接收
varErr RecvVarMq(var_mq_t mq,void *buffer,var_u16 size,int time);

#endif //_VAR_MQ_H_
