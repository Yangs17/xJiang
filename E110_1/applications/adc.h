#ifndef __ADC_H__
#define __ADC_H__

#include <stm32f4xx_adc.h>
#include "rtdef.h"
#include "ctrl.h"
#define VDDthr  1000   //电压阈值1V * 1000
/*
typedef struct Power_Lost_info{
	rt_uint16_t  ConBo_op;		// 15-8位表示2Gmod与面板是否连接（线有没有接上）   
						//7-0位表示2Gmod与面板是否连接（数据能否传输）
	rt_uint16_t	 mod_signAVE;	//与服务器连接信号平均值
	rt_uint16_t  PL_op;	 		
	
}PL_Info;
*/

typedef struct Power_Lost_info{
	rt_uint16_t	 mod_signAVE;	//与服务器连接信号平均值
	rt_uint16_t  device;	
	rt_uint16_t  dev_judge;	 		
	
}PL_Info;

typedef struct send_ADCinfo
{
	
	stPackCmd packcmd;

	rt_uint8_t  dev_type; //  设备类型

//	struct can_pack_head can_pack; //head
	PL_Info  plInfo;			   //data
	rt_uint16_t crc;			   //crc
}send_ADCinfo;


void rt_system_PD_init(void);
int packADCInfo(void);
rt_uint32_t ADC_Check(void);





#endif

