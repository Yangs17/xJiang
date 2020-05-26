#include "sys.h"
//#include "radio_config_si4463_125k_4FSK.h"
#include "radio_config_si4463_256k_4FSK.h"
//#include "radio_config_si4463_500k_4FSK.h"
//#include "radio_config_si4463_256k.h"
//#include "radio_config_si4463_500k.h"
//#include "led.h"
//#include "delay.h"
#include "usart.h"
#include "si4463.h"
#include "spi.h"
#include "ctrl.h"
#include "at.h"
#include "sub_G.h"
#include "abox.h"



extern struct rt_spi_device * sub_spi_device;



unsigned char RF_FREQ_CONTROL_INTE_8_data[RF_FREQ_CONTROL_INTE_8_NUM][12] = 		
{
	{RF_FREQ_CONTROL_INTE_8_0},
	{RF_FREQ_CONTROL_INTE_8_1},	
	{RF_FREQ_CONTROL_INTE_8_2},
	{RF_FREQ_CONTROL_INTE_8_3},
	{RF_FREQ_CONTROL_INTE_8_4},
	{RF_FREQ_CONTROL_INTE_8_5},
	{RF_FREQ_CONTROL_INTE_8_6},
	{RF_FREQ_CONTROL_INTE_8_7},
	{RF_FREQ_CONTROL_INTE_8_8},
	{RF_FREQ_CONTROL_INTE_8_9},
	
	{RF_FREQ_CONTROL_INTE_8_10},
	{RF_FREQ_CONTROL_INTE_8_11}, 
	{RF_FREQ_CONTROL_INTE_8_12},
	{RF_FREQ_CONTROL_INTE_8_13},
	{RF_FREQ_CONTROL_INTE_8_14},
	{RF_FREQ_CONTROL_INTE_8_15},
	{RF_FREQ_CONTROL_INTE_8_16},
	{RF_FREQ_CONTROL_INTE_8_17},
	{RF_FREQ_CONTROL_INTE_8_18},
	{RF_FREQ_CONTROL_INTE_8_19},

	{RF_FREQ_CONTROL_INTE_8_20},
	{RF_FREQ_CONTROL_INTE_8_21},	
	{RF_FREQ_CONTROL_INTE_8_22},
	{RF_FREQ_CONTROL_INTE_8_23},
	{RF_FREQ_CONTROL_INTE_8_24},
	{RF_FREQ_CONTROL_INTE_8_25},
	{RF_FREQ_CONTROL_INTE_8_26},
	{RF_FREQ_CONTROL_INTE_8_27},
	{RF_FREQ_CONTROL_INTE_8_28},
	{RF_FREQ_CONTROL_INTE_8_29},

};


unsigned char RF_POWER_UP_data[] = 			   				{ RF_POWER_UP};
unsigned char RF_FRR_CTL_A_MODE_4_data[] = 		   		{ RF_FRR_CTL_A_MODE_4};
 unsigned char RF_MODEM_FREQ_DEV_0_1_data[] = 		   		{ RF_MODEM_FREQ_DEV_0_1};
 unsigned char RF_MODEM_AGC_CONTROL_1_data[] = 			{ RF_MODEM_AGC_CONTROL_1};
 unsigned char RF_MODEM_MOD_TYPE_12_data[]=				{RF_MODEM_MOD_TYPE_12};
 unsigned char RF_MODEM_TX_RAMP_DELAY_12_data[]=			{RF_MODEM_TX_RAMP_DELAY_12};
 unsigned char RF_MODEM_BCR_NCO_OFFSET_2_12_data[]=		{RF_MODEM_BCR_NCO_OFFSET_2_12};
 unsigned char RF_MODEM_AFC_LIMITER_1_3_data[]=			{RF_MODEM_AFC_LIMITER_1_3};
 unsigned char RF_MODEM_AGC_WINDOW_SIZE_12_data[]=		{RF_MODEM_AGC_WINDOW_SIZE_12};
 unsigned char RF_MODEM_RAW_CONTROL_10_data[]=			{RF_MODEM_RAW_CONTROL_10};
 unsigned char RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12_data[]={RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12};
 unsigned char RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12_data[]=	 {RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12};
 unsigned char RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12_data[]=	 {RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12};
 unsigned char RF_SYNTH_PFDCP_CPFF_7_data[]=				{RF_SYNTH_PFDCP_CPFF_7};
 unsigned char RF_MODEM_RAW_SEARCH2_2_data[]=			{RF_MODEM_RAW_SEARCH2_2};
 unsigned char RF_MODEM_SPIKE_DET_2_data[]=			{RF_MODEM_SPIKE_DET_2};
 unsigned char RF_MODEM_DSA_CTRL1_5_data[]=			{RF_MODEM_DSA_CTRL1_5};

//前四个字节为识别头码，模块自动识别 第五个字节为从机号

//FlagType	Flag;

//U32	count_1hz, rf_timeout;


U8 spi_tx_buf[payload_length];
U8 spi_rx_buf[payload_length];


void enable_tx_interrupt(void)		
{	
	U8 p[6];

	p[0] = 0x11;
	p[1] = 0x01;
	p[2] = 0x02;
	p[3] = 0x00;
	p[4] = 0x01;
	p[5] = 0x20;	
	spi_write(0x06, p);	
}	

void rf_standby(void)				
{
	U8 p[2];
	
	p[0] = CHANGE_STATE ;	// CHANGE_STATE
	p[1] = 0x01 ; // sleep mode
	spi_write(2, p);  
} 

void tx_start(void)				
{
	U8 p[5];
	
	p[0] = START_TX ;
	p[1] = freq_channel ; 		//	channel 0
	p[2] = 0x30; 
	p[3] = 0;
	p[4] = 0;
	spi_write(5, p);  
} 


	
/**
 * This function will send buff through spi device to sub-g.
 *
 * @param send_buf:		The buff to be sent
 *		  data_length:  The length of the buff
 * @return void
 *
 */
void tx_data(void *send_buf,U8 data_length)					
{		
	if( !send_buf || !data_length)  return;

	rt_memcpy(spi_tx_buf,send_buf,data_length);
	sub_lock ->value = 0;


	fifo_reset();

//at_printf("tx2 %d\n",nIRQ);	
	spi_write_fifo(data_length); 
	
//at_printf("tx3 %d\n",nIRQ);

	enable_tx_interrupt();	
//at_printf("tx4 %d\n",nIRQ);


	clr_interrupt();
//at_printf("tx5 %d\n",nIRQ);	

	tx_start();
//at_printf("tx6 %d\n",nIRQ);

	/* 等待下降沿触发 返回OK */
	if(RT_EOK != rt_sem_take(sub_lock, TX_TIME_OUT))					
	{
		filter_information *filterInfo_t = RT_NULL;
	
#ifdef ABOX
		#include "abox.h"	
	    filterInfo_t = &aboxInfo.filterInfo;
#endif

	
		//10ms等待，有发送就跳出，一般正常发送3ms左右
		{
			at_printf("tx is error\n");  //发送失败，复位
			sdn_reset();       
 			SI4463_init(filterInfo_t); 
			return;
		}			
	}
//at_printf("tx7 %d\n",nIRQ);
	rx_init();

	
}




/**
 * This function will recv buff through spi device to sub-g.
 *
 * @param recv_buf   :	The buff to be recevied
 *		  data_length:  The length of the received buff
 *		  time_out   :  Set timeout period.
 *
 * @return RT_EOK	: recvived is ok
 *		   RT_ERROR : received is error
 */
rt_err_t rx_data(void *recv_buf,U8 data_length,rt_tick_t time_out)					
{		
	//rt_tick_t rt_tick_get(void)
	rt_err_t ret;

	if( !recv_buf || !data_length)  return RT_ERROR;
	ret = rt_sem_take(sub_lock, time_out);

	if(RT_EOK == ret)
	{
		clr_interrupt();   // clear interrupt	

		/* get info */
		spi_read_fifo();

		rt_memcpy(recv_buf,spi_rx_buf,data_length);
		fifo_reset();		
	}

#if 0 // test : rssi = 0
	if(CONNECT_SERVER == Cdma_Info.link)
	{
		spi_read(9,0x22);    //读取rssi值，存放在spi_read_buf[3]中
		at_printf("rssi: %d \r\n",spi_rx_buf[4]);
	}
#endif
	return ret;


}




	
/**
 * This function will send buff through spi device to sub-g.
 *
 * @param tx_length send buff's length.
 * @param p the send buff.
 *
 * @return void
 *
 */

void spi_write_fifo(U8 data_length)
{
	
	U8 i;	
	i = 0;
	while(i!=0xff)
	{
		i = check_cts();		
	}

	 i = WRITE_TX_FIFO;
	 rt_spi_send_then_send(sub_spi_device,&i, 1,spi_tx_buf,data_length);

	 
}	





 /**
  * This function will send buff through spi device to sub-g.
  *
  * @param tx_length send buff's length.
  *
  * @return void
  *
  */
 void spi_read_fifo(void)
 {
	 U8 i;
	 
	 i = 0;
	 
	 while(i!=0xff)
	 {
		 i = check_cts();		 
	 }		 
		 

	 i = READ_RX_FIFO;
	 rt_spi_send_then_recv(sub_spi_device, &i, 1, spi_rx_buf, payload_length);

	 
 }	 



 /**
  * This function will send buff to spi device.
  *
  * @param tx_length send buff's length.
  * @param p the send buff.
  *
  * @return void
  *
  */
void spi_write(unsigned char tx_length, unsigned char *p)
{
	unsigned char i = 0;


	while(i!=0xff)
	{
		i = check_cts();
	}
	rt_spi_send(sub_spi_device,p,tx_length);

}



/**
 * This function will reset RF.
 *
 * @param 
 *
 * @return void
 *
 */
void sdn_reset(void)
{		
	nSEL = 1;   
 	SDN = 1;
 	//delay_1ms(2);	
 	rt_thread_delay(2);
 	SDN = 0;
 	//delay_1ms(10);	
	rt_thread_delay(10);
	SCK = 0;


	rt_spi_send(sub_spi_device,RF_POWER_UP_data,7);
	
 	rt_thread_delay(20);
}



/**
 * This function will configuration register.
 *
 * @param filter the first four bytes filter the data
 *
 * @return void
 *
 */
/* Configuration information for field 1 */
typedef struct FIELD_CONFIG
{	
	unsigned char len_mode;// 1：	variable mode  0:fixed mode
	unsigned short max_len;
}field_1_config;

void SI4463_init(filter_information *filterInfo_t)
{	
	if(RT_NULL == filterInfo_t) return;

	field_1_config field_1;
	field_1.len_mode = 1;
	field_1.max_len = payload_length;
	
	U8 app_command_buf[20];
				
	//spi_write(0x07, RF_GPIO_PIN_CFG_data);   
	app_command_buf[0] = 0x13;			// SET GPIO PORT
	app_command_buf[1]  = 19; 		// gpio 0 ,Rx data
	app_command_buf[2]  = 0x02;    		// gpio1, output 0
	app_command_buf[3]  = 0x21;  		// gpio2, hign while in receive mode
	app_command_buf[4]  = 0x20; 		// gpio3, hign while in transmit mode 
	app_command_buf[5]  = 0x27;   		// nIRQ
	app_command_buf[6]  = 0x0b;  		// sdo
	spi_write(7, app_command_buf); 
		
	// spi_write(0x05, RF_GLOBAL_XO_TUNE_1_data); 
    app_command_buf[0] = 0x11;  
	app_command_buf[1]  = 0x00;    
	app_command_buf[2]  = 0x01;    
	app_command_buf[3]  = 0x00;  
	app_command_buf[4]  = 98;  			// freq  adjustment
	spi_write(5, app_command_buf);

	// spi_write(0x05, RF_GLOBAL_CONFIG_1_data);
  	app_command_buf[0] = 0x11;  
	app_command_buf[1]  = 0x00;
	app_command_buf[2]  = 0x01; 
	app_command_buf[3]  = 0x03; 
	app_command_buf[4]  = 0x40;  		// tx = rx = 64 byte,PH,high performance mode
	spi_write(5, app_command_buf); 
    
    spi_write(0x08, RF_FRR_CTL_A_MODE_4_data);    // disable all fast response register
     
    // spi_write(0x0D, RF_PREAMBLE_TX_LENGTH_9_data); // set Preamble
 	app_command_buf[0] = 0x11;  
	app_command_buf[1]  = 0x10;    
	app_command_buf[2]  = 0x09;    
	app_command_buf[3]  = 0x00;   
	app_command_buf[4]  = 0x08;							//  8 bytes Preamble
	app_command_buf[5]  = 0x14;							//  detect 20 bits
	app_command_buf[6]  = 0x00;						
	app_command_buf[7]  = 0x0f;
	app_command_buf[8]  = 0x31;  						//  no manchest.1010.??
	app_command_buf[9]  = 0x00;
	app_command_buf[10]  = 0x00;
	app_command_buf[11]  = 0x00;
	app_command_buf[12]  = 0x00;
	spi_write(13, app_command_buf);	
	
	//  RF_SYNC_CONFIG_5_data,							// set sync
    app_command_buf[0] = 0x11;  
	app_command_buf[1]  = 0x11;
	app_command_buf[2]  = 0x05;
	app_command_buf[3]  = 0x00;
	app_command_buf[4]  = 0x01;   						// no manchest , 2 bytes
	app_command_buf[5]  = 0xb4;   						// sync byte3
	app_command_buf[6]  = 0x2b;							// sync byte2
	app_command_buf[7]  = 0x00;							// sync byte1
	app_command_buf[8]  = 0x00;							// sync byte0
    spi_write(9, app_command_buf);
        
	//  packet crc         
    app_command_buf[0] = 0x11;  
	app_command_buf[1]  = 0x12; 
	app_command_buf[2]  = 0x01; 
	app_command_buf[3]  = 0x00;
	app_command_buf[4]  = 0x81;							// CRC = itu-c, enable crc
    spi_write(5, app_command_buf);  
        
	// packet   gernale configuration        
    app_command_buf[0] = 0x11;  
	app_command_buf[1]  = 0x12;
	app_command_buf[2]  = 0x01;
	app_command_buf[3]  = 0x06;
	app_command_buf[4]  = 0x22;							// CRC MSB, data MSB
    spi_write(5, app_command_buf);
        
  	// spi_write(0x07, RF_PKT_LEN_3_data);   
    app_command_buf[0] = 0x11;  
	app_command_buf[1]  = 0x12;
	app_command_buf[2]  = 0x03;
	app_command_buf[3]  = 0x08;
	app_command_buf[4]  = 0x00;//1208
	app_command_buf[5]  = field_1.len_mode;//1209
	app_command_buf[6]  = 0x00;						 
    spi_write(7, app_command_buf);         
	
	app_command_buf[0] = 0x11;  
	app_command_buf[1]  = 0x12;
	app_command_buf[2]  = 0x0c;
	app_command_buf[3]  = 0x0d;
	
	app_command_buf[4]  = field_1.max_len >> 8;
	app_command_buf[5]  = field_1.max_len;

	app_command_buf[6]  = 0x14;
	app_command_buf[7]  = 0xaa;
	app_command_buf[8]  = 0x00;
	app_command_buf[9]  = 0x00;
	app_command_buf[10]  = 0x00;
	app_command_buf[11]  = 0x00;
	app_command_buf[12]  = 0x00; 
	app_command_buf[13]  = 0x00;
	app_command_buf[14]  = 0x00;
	app_command_buf[15]  = 0x00;
	spi_write(16, app_command_buf);					// set length of Field 1 -- 4
  
    // spi_write(0x0C, RF_PKT_FIELD_4_LENGTH_12_8_8_data);
    app_command_buf[0] = 0x11;  
	app_command_buf[1]  = 0x12; 
	app_command_buf[2]  = 0x08;
	app_command_buf[3]  = 0x19;
	app_command_buf[4]  = 0x00;
	app_command_buf[5]  = 0x00;
	app_command_buf[6]  = 0x00;
	app_command_buf[7]  = 0x00;
	app_command_buf[8]  = 0x00;
	app_command_buf[9]  = 0x00;
	app_command_buf[10]  = 0x00;
	app_command_buf[11]  = 0x00;
    spi_write(12, app_command_buf);
    
    spi_write(0x10, RF_MODEM_MOD_TYPE_12_data);        
	spi_write(0x05, RF_MODEM_FREQ_DEV_0_1_data);
	
    spi_write(0x10, RF_MODEM_TX_RAMP_DELAY_12_data);    	
    spi_write(0x10, RF_MODEM_BCR_NCO_OFFSET_2_12_data);
	spi_write(0x10, RF_MODEM_TX_RAMP_DELAY_12_data);			
    spi_write(0x07, RF_MODEM_AFC_LIMITER_1_3_data);	
	//spi_write(0x10, BCR_NCO_OFFSET_2_12_data);

    spi_write(0x05, RF_MODEM_AGC_CONTROL_1_data);		
    spi_write(0x10, RF_MODEM_AGC_WINDOW_SIZE_12_data);	
    spi_write(14, RF_MODEM_RAW_CONTROL_10_data);
    
	spi_write(0x06, RF_MODEM_SPIKE_DET_2_data);
	// spi_write(0x05, RF_MODEM_RSSI_COMP_1_data);
	app_command_buf[0] = 0x11;  
	app_command_buf[1] = 0x20;                                                     
	app_command_buf[2] = 0x01;                                                   
	app_command_buf[3] = 0x4e;             
	app_command_buf[4]  = 0x40;
    spi_write(5, app_command_buf);            	     
	spi_write(0x09, RF_MODEM_DSA_CTRL1_5_data);
   
    spi_write(0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12_data);
    spi_write(0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12_data);
    spi_write(0x10, RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12_data);       
        
	// RF_PA
	app_command_buf[0] = 0x11;  
	app_command_buf[1]  = 0x22;                                                    
	app_command_buf[2]  = 0x04;                                               
	app_command_buf[3]  = 0x00;                                                     
	app_command_buf[4]  = 0x08;
	app_command_buf[5]  = 127;							// set max power
	app_command_buf[6]  =0x00;
	app_command_buf[7]  = 0x3d;
    spi_write(8, app_command_buf);        
    
	spi_write(0x0B, RF_SYNTH_PFDCP_CPFF_7_data);
        
    	// header match
 	app_command_buf[0] = 0x11;  
	app_command_buf[1]  = 0x30;                                                     
	app_command_buf[2]  = 0x0c;                                                   
	app_command_buf[3]  = 0x00;                                                       
app_command_buf[4]  =   filterInfo_t->filterBuf[0]; // master_header1; //slaver_header1;
	app_command_buf[5]  = filterInfo_t->filterMark;//0
	app_command_buf[6]  = 0x40;
app_command_buf[7]  =  	filterInfo_t->filterBuf[1]; // master_header2;  //slaver_header2;                                      
	app_command_buf[8]  = filterInfo_t->filterMark;//0                                       
	app_command_buf[9]  = 0x01; 
app_command_buf[10] = 	filterInfo_t->filterBuf[2];  //master_header3; //slaver_header3;                                   
	app_command_buf[11] = filterInfo_t->filterMark;//0                                       
	app_command_buf[12]  =0x02;
app_command_buf[13]  =  filterInfo_t->filterBuf[3]; //master_header4; //slaver_header4;                                  
	app_command_buf[14] = filterInfo_t->filterMark;//0
	app_command_buf[15]  =0x03;
    spi_write(16, app_command_buf);
    
	spi_write(6, RF_MODEM_RAW_SEARCH2_2_data);


 	//spi_write(12, RF_FREQ_CONTROL_INTE_8_data[10]); //test 
 
    spi_write(12, RF_FREQ_CONTROL_INTE_8_data[Cdma_Info.freq_num]); 	    // set frequency 

}


void fifo_reset(void)			// reset FIFO
{
	U8 p[2];
	
	p[0] = FIFO_INFO;
	p[1] = 0x03;   // reset tx ,rx fifo
	spi_write(2,p);
}


void enable_rx_interrupt(void)
{
	U8 p[7];

	p[0] = 0x11;
	p[1] = 0x01;  // 0x0100
	p[2] = 0x03;// 3 parameters
	p[3] = 0x00;   // 0100
	p[4] = 0x03;   // ph, modem int
	p[5] = 0x18; // 0x10;   // Pack received int
	p[6] = 0x00;   //preamble int, sync int setting	
	spi_write(0x07, p);  // enable  packet receive interrupt	
}




void clr_interrupt(void)		// clar interrupt
{
	U8 p[4];
	
	p[0] = GET_INT_STATUS;
	p[1] = 0;   // clr  PH pending
	p[2] = 0;   // clr modem_pending
	p[3] = 0;   // clr chip pending
	spi_write(4,p);
	//spi_read(9,GET_INT_STATUS); 
}

void rx_start(void)				
{
	U8 p[8];
	
	p[0] = START_RX ; 
	p[1] = freq_channel ; 			//	channel 0
	p[2] = 0x00; 
	p[3] = 0;
	p[4] = 0;
	p[5] = 0;
	p[6] = 0x08;  
	p[7] = 0x08;   	
	spi_write(8, p);  
}


U8 check_cts(void)	
{
	U8 i = 0x44;
	U8 j ;
		
    rt_spi_send_then_recv(sub_spi_device, &i, 1, &j, 1);
	
	return j;
}	

void rx_init(void)				
{
	sub_lock ->value = 0;
	fifo_reset();  					
	enable_rx_interrupt();	
	clr_interrupt();   				
	rx_start(); 
	
}	



void spi_read(U8 data_length, U8 api_command )
{
	U8 i;
	
	U8 p[1];
	p[0] = api_command; 
	i = 0;
	while(i!=0xff)
	{
		i = check_cts();		
	}
		
	spi_write(1, p);  
	
	i = 0;
	while(i!=0xff)
	{
		i = check_cts();		
	}


	p[0] = 0x44;
	rt_spi_send_then_recv(sub_spi_device, p, 1, spi_rx_buf, data_length);


}


