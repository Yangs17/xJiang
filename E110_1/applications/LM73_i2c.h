
#ifndef __RT_LM73_H__
#define __RT_LM73_H__

#include "rtthread.h"
#include <rtdef.h>
#include "stm32f4xx_gpio.h"
#include "i2c-bit-ops.h"
#include "i2c.h"
#include "integer.h"

#define RP_TEM 0x00 //�¶ȼĴ�����ַ
#define RP_CS 0x04 //����/״̬�Ĵ�����ַ
#define RP_ID 0x07 //ID��ַ


#define LM730_ID 0x0190
#define PRECISION_11 0x00 //11λ����ѡ��
#define PRECISION_12 0x20 //12λ����ѡ��
#define PRECISION_13 0x40 //13λ����ѡ��
#define PRECISION_14 0x60 //14λ����ѡ��





int rt_hw_i2c_init(void);


#ifdef RT_LM73

int rt_hw_LM73_i2c_init(void);
void LM73_Init(void);


void I2C_LM73_BufferRead(BYTE* pBuffer, u8 ReadAddr,u8 ack);
void I2C_LM73_ByteWrite(BYTE Buffer, BYTE WriteAddr);

#endif

#endif 
