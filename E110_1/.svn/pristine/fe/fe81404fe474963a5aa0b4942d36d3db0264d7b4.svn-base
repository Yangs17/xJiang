#include "var_mq.h"

/*******************************************************************
 * @copyright www.esto.cn
 * @author Yangs
 * @date 22-April-2019
 * @brief Implementation of the corresponding header file function interface.
 * @history 
 *******************************************************************/

var_mq_t creatMsg(const char* name)
{
	return CreatVarMq(name);
}

varErr sendMsg(var_mq_t mq,void* buffer,var_u16 size)
{
	return SendVarMq(mq,buffer,size);
}

varErr recvMsg(var_mq_t mq,void *buffer,var_u16 size,int time)
{
	return RecvVarMq(mq,buffer,size,time);
}





//申请
//msgSize 消息队列中一条消息的最大长度；
//maxMsgs 最大能够容纳的消息数
var_mq_t CreatVarMq(const char* name)
{
	if(!name) return VAR_NULL;

	var_mq_t mqInfo_t = (var_mq_t)VarMalloc(sizeof(var_mq));
	if(!mqInfo_t)
	{
		VarFree(mqInfo_t);
		return VAR_NULL;
	}

	mqInfo_t->entrySize = 0;
	VarMemset(mqInfo_t->msgPool,0, VARSIZE);
	
	mqInfo_t->entryNum  = 0;
	VarMemset(mqInfo_t->mqArray,0, VARARRAYSIZE);


	if(VAR_EOK != VarInitSem(&mqInfo_t->mqSem, name, 0, RT_IPC_FLAG_FIFO))
		return VAR_NULL;
	
	return mqInfo_t;
}


//发送
varErr SendVarMq(var_mq_t mq,void* buffer, var_u16 size)
{
	if(!mq ||!buffer || !size) return VAR_ERROR;
	if(size+mq->entrySize > VARSIZE) return VAR_ERROR;
	if(mq->entryNum >= VARARRAYSIZE) return VAR_ERROR;

	/* lock */
	//var_u32 level = VarInterruptDisablle();
	
	VarMemcpy(mq->msgPool+mq->entrySize,(var_u8 *)buffer,size);
	mq->mqArray[mq->entryNum].msgLength = size;
	mq->mqArray[mq->entryNum].pDataAddr = mq->entrySize;
	mq->entrySize += size;
	mq->entryNum++;
	
	VarRealseSem(&mq->mqSem);

	//VarInterruptEnable(level);
	
	return VAR_EOK;
}


//接收
varErr RecvVarMq(var_mq_t mq,void *buffer,var_u16 size,int time)
{
	if(!mq || !buffer || !size) return VAR_ERROR;
	if(VAR_EOK != VarTakeSem(&mq->mqSem,time)) return VAR_ERROR;
	
	if(0 == mq->entryNum)	return VAR_ERROR;
	if(size < mq->mqArray[0].msgLength) return VAR_ERROR;
	
	/* lock */
	//var_u32 level = VarInterruptDisablle();
	var_u16 i = 0;
	var_u16 j = 0;

	/* data */
	//VarMemcpy((var_u8 *)buffer,mq->msgPool+mq->mqArray[0].pDataAddr,mq->mqArray[0].msgLength);
	rt_memcpy((var_u8 *)buffer,mq->msgPool+mq->mqArray[0].pDataAddr,mq->mqArray[0].msgLength);
	mq->entrySize -= mq->mqArray[0].msgLength;
	
	for(i=mq->mqArray[0].msgLength; i<mq->entrySize; i++)
	{
		mq->msgPool[j] = mq->msgPool[i];
		j++;
	}
	VarMemset(mq->msgPool+mq->entrySize,0,VARSIZE-mq->entrySize);

	/* mark */
	mq->entryNum--;
	for(i=0; i<mq->entryNum; i++)
	{
		mq->mqArray[i].pDataAddr = mq->mqArray[i+1].pDataAddr;
		mq->mqArray[i].msgLength = mq->mqArray[i+1].msgLength;
	}
	mq->mqArray[mq->entryNum].pDataAddr = 0;
	mq->mqArray[mq->entryNum].msgLength = 0;
	
	//VarInterruptEnable(level);
	
	return VAR_EOK;
}



