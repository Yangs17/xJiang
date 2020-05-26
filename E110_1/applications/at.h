#ifndef __AT_H__
#define __AT_H__

#ifdef RT_CDMA_USING
#ifdef __cplusplus
extern "C" {
#endif
	
#include "rtdevice.h"	
#include "ctrl.h"	
struct rt_gprs_fun
{
	void (*moden_boot)  (void);

	//return: RT_EOK     at start is ok
	//		  RT_ERROR   at start is err.
    int (*get_sta)     (rt_device_t device, char *in_buff, stCdmaInfo *info);


	int (*get_csq)     (rt_device_t device, char *in_buff, stCdmaInfo *info);	
    void (*get_info)    (rt_device_t device, char *in_buff, stCdmaInfo *info);
    void (*get_time)    (rt_device_t device, char *in_buff, stCdmaInfo *info, int type); 

    int  (*open_tcp)    (rt_device_t device, char *in_buff);
    void  (*colse_tcp)   (rt_device_t device, char *in_buff);
    int  (*send_tcp)    (rt_device_t device, char *in_buff, char *send,stCdmaInfo *info);
    int	 (*rev_tcp)     (rt_device_t device, char *in_buff, char *rev,rt_uint32_t *recvlen);
    
};	
	

//the function of M26
void M26_moden_boot(void);
int M26_get_sta_mx(rt_device_t device, char *in_buff, stCdmaInfo *info);
int M26_get_csq_mx(rt_device_t device, char *in_buff, stCdmaInfo *info);
void M26_get_info_mx(rt_device_t device, char *in_buff, stCdmaInfo *info);

void M26_get_time_mx(rt_device_t device, char *in_buff, stCdmaInfo *info, int type);
int  M26_open_tcp_mx(rt_device_t device, char *in_buff);
void  M26_colse_tcp_mx(rt_device_t device, char *in_buff);

int M26_send_tcp_mx	(rt_device_t device, char *in_buff, char *send,stCdmaInfo *info);
int M26_rev_tcp_mx(rt_device_t device, char *in_buff, char *rev,rt_uint32_t *recvlen);



//the function of EC20
void EC20_moden_boot(void);
int EC20_get_sta_mx(rt_device_t device, char *in_buff, stCdmaInfo *info);
int EC20_get_csq_mx(rt_device_t device, char *in_buff, stCdmaInfo *info);
void EC20_get_info_mx(rt_device_t device, char *in_buff, stCdmaInfo *info);

void EC20_get_time_mx(rt_device_t device, char *in_buff, stCdmaInfo *info, int type);
int  EC20_open_tcp_mx(rt_device_t device, char *in_buff);
void  EC20_colse_tcp_mx(rt_device_t device, char *in_buff);

int EC20_send_tcp_mx	(rt_device_t device, char *in_buff, char *send,stCdmaInfo *info);
int  EC20_rev_tcp_mx(rt_device_t device, char *in_buff, char *rev,rt_uint32_t *recvlen);




int ex_csq(stCdmaInfo *info,char *in_buff);	
int  str_scan(char *k, char *in_buff, char key, char key1, char key2, char off);
void  AT_send_close_cmd(rt_device_t device, char *cmd, char *ack, char *buff);

int  AT_send_cmd(rt_device_t device, char *cmd,char *ack, char *buff);
rt_err_t crc_ipd_len(char *in_buff,rt_uint32_t *bufflen);
void AT_get_sta_2(rt_device_t device, char *in_buff, char *cmd, char *ack, char *a, char *b);
int  AT_send_mdn(char *mdn);
void AT_recv_free(rt_device_t device);
int AT_recv_result(rt_device_t device,rt_size_t writelen);
rt_size_t AT_send_bufflen(rt_device_t device);
void AT_get_str(rt_device_t device, char *cmd, char *ack, char *buff,unsigned char *a,char type);




rt_uint16_t  ModbusCrc16(rt_uint8_t * frame, rt_uint16_t len);




int search_str(char *sbuf,int sbufsize,char *mbuf,int mbufsize);

void AT_tcp_init(void);


#ifdef __cplusplus
}
#endif
#endif
#endif
