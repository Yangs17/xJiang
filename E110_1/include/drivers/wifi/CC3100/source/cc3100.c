/*
 * main.c - TCP socket sample application
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * Application Name     -   TCP socket
 * Application Overview -   This is a sample application demonstrating how to open
 *                          and use a standard TCP socket with CC3100.
 * Application Details  -   http://processors.wiki.ti.com/index.php/CC31xx_TCP_Socket_Application
 *                          doc\examples\tcp_socket.pdf
 */


#include "simplelink.h"
#include "sl_common.h"
#include "net_config.h"
//#include "abox.h"
#include "dictionary.h"
#define APPLICATION_VERSION "1.3.0"
#define SL_STOP_TIMEOUT        0xFF




/* IP addressed of server side socket. Should be in long format,
 * E.g: 0xc0a8026c == 192.168.2.108
 0xc0a8022d   [192.168.2.45]
 0x700E356A		yun
 0x770345A7   [119.3.69.167] iot
 [112.14.53.106]
 */
#define IP_ADDR         0x770345A7      //iot
#define PORT_NUM        19991           /* Port number to be used */


#define BUF_SIZE        1024//20
#define NO_OF_PACKETS   10

#define CONFIG_IP       SL_IPV4_VAL(192,168,124,101)    //(192,168,2,243)       /* Static IP to be configured */
#define AP_MASK         SL_IPV4_VAL(255,255,255,0)      /* Subnet Mask for the station */
#define AP_GATEWAY      SL_IPV4_VAL(192,168,124,1)     //(192,168,2,1)        /* Default Gateway address */
#define AP_DNS          SL_IPV4_VAL(192,168,124,1)     //(192,168,2,1)            /* DNS Server Address */

#define RSSI_THRESHOLD_VALUE    -110
#define SCAN_TABLE_SIZE         20
#define RSSI_BUF_SIZE           5
#define SCAN_INTERVAL           60
Sl_WlanNetworkEntry_t g_NetEntries[SCAN_TABLE_SIZE] = {0};



/* Application specific status/error codes */
typedef enum{
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,        /* Choosing this number to avoid overlap w/ host-driver's error codes */
    TCP_SEND_ERROR = DEVICE_NOT_IN_STATION_MODE - 1,
    TCP_RECV_ERROR = TCP_SEND_ERROR -1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

/*
 * GLOBAL VARIABLES -- Start
 */
_u8 g_Status = 0;
static _i16		  SockID = 0;

#define WIFI_AP_CLOSE	  (1<<1)  //Router disconnected
#define WIFI_SOCK_CLOSE   (1<<2)  //wifi sock is close
static rt_event_t wifiEvent;

/*
 * GLOBAL VARIABLES -- End
 */

/*
 * STATIC FUNCTION DEFINITIONS -- Start
 */
static _i32 configureSimpleLinkToDefaultState(void);
static _i32 establishConnectionWithAP(void);
static _i32 initializeAppVariables(void);
static _i32 BsdTcpClient(_u16 Port);
//static _i32 BsdTcpServer(_u16 Port);
static void displayBanner(void);



/* 判断wifi连接时的发送情况 */
rt_err_t judgeWifiSendStatus()
{
	if(Panel_con_eth != dicInfo.secondDicInfo.panelLink)
	{	
		at_printf(" tcp is closed! %d \r\n",dicInfo.secondDicInfo.panelLink);
		sl_Close(SockID);
		return RT_ERROR;
	}
	return RT_EOK;

}


void quitCC3100Tcp(void)
{
	rt_event_send(wifiEvent, WIFI_SOCK_CLOSE);
	sl_Close(SockID);
	rt_kprintf("close cc3100 sock.\r\n");
}


rt_err_t recvCC3100TcpClose(void)
{
	rt_uint32_t recved;
	
	return rt_event_recv(wifiEvent,  
			  WIFI_SOCK_CLOSE,
			  RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,
			  RT_WAITING_NO,
			  &recved);
				
}



/* 发送数据 */
int sendCC3100Data(rt_uint8_t *sendBuf,rt_uint32_t len)
{
	if(!sendBuf || !len) return 0;
	if(len > BUFF_LEN)   return 0;
 
 	return sl_Send(SockID, sendBuf, len, 0);
}



/*
 * ASYNCHRONOUS EVENT HANDLERS -- Start
 */
/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(pWlanEvent == NULL)
    {
        at_printf(" [WLAN EVENT] NULL Pointer Error \n\r");
        return;
    }
    
    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            /*
             * Information about the connected AP (like name, MAC etc) will be
             * available in 'slWlanConnectAsyncResponse_t' - Applications
             * can use it if required
             *
             * slWlanConnectAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
             *
             */
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_ACQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            /* If the user has initiated 'Disconnect' request, 'reason_code' is
             * SL_USER_INITIATED_DISCONNECTION */
            if(SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                at_printf(" Device disconnected from the AP on application's request \n\r");
            }
            else
            {
                at_printf(" Device disconnected from the AP on an ERROR..!! \n\r");
				rt_event_send(wifiEvent, WIFI_AP_CLOSE);
            }
        }
        break;

        default:
        {
            at_printf(" [WLAN EVENT] Unexpected event \n\r");
        }
        break;
    }
}

/*!
    \brief This function handles events for IP address acquisition via DHCP
           indication

    \param[in]      pNetAppEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if(pNetAppEvent == NULL)
    {
        at_printf(" [NETAPP EVENT] NULL Pointer Error \n\r");
        return;
    }
 
    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SET_STATUS_BIT(g_Status, STATUS_BIT_IP_ACQUIRED);

            /*
             * Information about the connection (like IP, gateway address etc)
             * will be available in 'SlIpV4AcquiredAsync_t'
             * Applications can use it if required
             *
             * SlIpV4AcquiredAsync_t *pEventData = NULL;
             * pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
             *
             */
        }
        break;

        default:
        {
            at_printf(" [NETAPP EVENT] Unexpected event \n\r");
        }
        break;
    }
}

/*!
    \brief This function handles callback for the HTTP server events

    \param[in]      pHttpEvent - Contains the relevant event information
    \param[in]      pHttpResponse - Should be filled by the user with the
                    relevant response information

    \return         None

    \note

    \warning
*/
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    /* Unused in this application */
    at_printf(" [HTTP EVENT] Unexpected event \n\r");
}

/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */
    at_printf(" [GENERAL EVENT] \n\r");
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
*/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(pSock == NULL)
    {
        at_printf(" [SOCK EVENT] NULL Pointer Error \n\r");
        return;
    }
    
    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            /*
             * TX Failed
             *
             * Information about the socket descriptor and status will be
             * available in 'SlSockEventData_t' - Applications can use it if
             * required
             *
            * SlSockEventData_u *pEventData = NULL;
            * pEventData = & pSock->socketAsyncEvent;
             */
            switch( pSock->socketAsyncEvent.SockTxFailData.status )
            {
                case SL_ECLOSE:
                    at_printf(" [SOCK EVENT] Close socket operation, failed to transmit all queued packets\n\r");
                    break;
                default:
                    at_printf(" [SOCK EVENT] Unexpected event \n\r");
                    break;
            }
            break;

        default:
            at_printf(" [SOCK EVENT] Unexpected event \n\r");
            break;
    }
}
/*
 * ASYNCHRONOUS EVENT HANDLERS -- End
 */



/*!
    \brief This function configure the SimpleLink device in its default state. It:
           - Sets the mode to STATION
           - Configures connection policy to Auto and AutoSmartConfig
           - Deletes all the stored profiles
           - Enables DHCP
           - Disables Scan policy
           - Sets Tx power to maximum
           - Sets power policy to normal
           - Unregisters mDNS services
           - Remove all filters

    \param[in]      none

    \return         On success, zero is returned. On error, negative is returned
*/
static _i32 configureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    _u8           val = 1;
    _u8           configOpt = 0;
    _u8           configLen = 0;
    _u8           power = 0;

    _i32          retVal = -1;
    _i32          mode = -1;

    mode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(mode);

    /* If the device is not in station-mode, try configuring it in staion-mode */
    if (ROLE_STA != mode)
    {
        if (ROLE_AP == mode)
        {
            /* If the device is in AP mode, we need to wait for this event before doing anything */
            while(!IS_IP_ACQUIRED(g_Status)) { _SlNonOsMainLoopTask(); }
        }

        /* Switch to STA role and restart */
        retVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(retVal);

        retVal = sl_Stop(SL_STOP_TIMEOUT);
        ASSERT_ON_ERROR(retVal);

        retVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(retVal);

        /* Check if the device is in station again */
        if (ROLE_STA != retVal)
        {
            /* We don't want to proceed if the device is not coming up in station-mode */
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
        }
    }

    /* Get the device's version-information */
    configOpt = SL_DEVICE_GENERAL_VERSION;
    configLen = sizeof(ver);
    retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (_u8 *)(&ver));
    ASSERT_ON_ERROR(retVal);

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
    retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Remove all profiles */
    retVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(retVal);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
    retVal = sl_WlanDisconnect();
    if(0 == retVal)
    {
        /* Wait */
        while(IS_CONNECTED(g_Status)) { _SlNonOsMainLoopTask(); }
    }

    /* Enable DHCP client*/
    retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&val);
    ASSERT_ON_ERROR(retVal);

    /* Disable scan */
    configOpt = SL_SCAN_POLICY(0);
    retVal = sl_WlanPolicySet(SL_POLICY_SCAN , configOpt, NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
    power = 0;
    retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (_u8 *)&power);
    ASSERT_ON_ERROR(retVal);

    /* Set PM policy to normal */
   // retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
	retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_ALWAYS_ON_POLICY, NULL, 0);

	ASSERT_ON_ERROR(retVal);

    /* Unregister mDNS services */
    retVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(retVal);

    /* Remove  all 64 filters (8*8) */
    pal_Memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    retVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(retVal);

    retVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(retVal);

    retVal = initializeAppVariables();
    ASSERT_ON_ERROR(retVal);

    return retVal; /* Success */
}

/*!
    \brief Connecting to a WLAN Access point

    This function connects to the required AP (SSID_NAME).
    The function will return once we are connected and have acquired IP address

    \param[in]  None

    \return     0 on success, negative error-code on error

    \note

    \warning    If the WLAN connection fails or we don't acquire an IP address,
                We will be stuck in this function forever.
*/
static _i32 establishConnectionWithAP()
{
    SlSecParams_t secParams = {0};
    _i32 retVal = 0;


	serverNet_information serNetConfig;	
	controlServerInfo(READ_NETCONFIG_INFO,&serNetConfig);
#if 0
    secParams.Key = PASSKEY;
    secParams.KeyLen = pal_Strlen(PASSKEY);
    secParams.Type = SEC_TYPE;
    retVal = sl_WlanConnect(SSID_NAME, pal_Strlen(SSID_NAME), 0, &secParams, 0);
#endif

	secParams.Key = serNetConfig.netConfig.wifiConfig.wifiPassword;
    secParams.KeyLen = pal_Strlen(serNetConfig.netConfig.wifiConfig.wifiPassword);
    secParams.Type = SEC_TYPE;
    retVal = sl_WlanConnect(serNetConfig.netConfig.wifiConfig.wifiName, 
							pal_Strlen(serNetConfig.netConfig.wifiConfig.wifiName),
							0, &secParams, 0);

    ASSERT_ON_ERROR(retVal);

	at_printf("wait for Ap satrting!\r\n");
    /* Wait */
    while((!IS_CONNECTED(g_Status)) || (!IS_IP_ACQUIRED(g_Status))) 
	{
		Cdma_Info.conErrPlace = WIFI_NETWORK_AP_CONNECT_ERROR;
		_SlNonOsMainLoopTask(); 
		Delay(1);
	}
	
	at_printf("AP station is opening!\r\n");
    return SL_SUCCESS;
}




/* chat client */
void wifiChat(void)
{
	int len = 0;
#if 1
	while(1)
	{
		len = sl_Recv(SockID,Eth_Rev_Buff+Eth_recv_len,sizeof(Eth_Rev_Buff)-Eth_recv_len, 0);
		if(len>0)
		{
			OutputLed(LEDOFF, LED_CAN);	
			Eth_recv_len += len;				
			processDataFromServer();
		}
		else
		{
			at_printf("len = %d acc = %d\r\n",len);
			break;
		}
	}

#else
	/* data can't recevied. */
	SlFdSet_t rfd;
	SlFdSet_t tmp;
	SlTimeval_t timeout;  //超时时间 

	timeout.tv_sec = READ_CLI_TIMEOUT_S;
	timeout.tv_usec = 0;

	SL_FD_ZERO(&rfd);
	SL_FD_ZERO(&tmp);
	SL_FD_SET(SockID, &rfd);

	

	while(1)
	{	
		tmp = rfd;
		len = sl_Select(SockID+1,&tmp,NULL,NULL,&timeout);
		if(len > 0)
		{
			if(SL_FD_ISSET(SockID,&tmp))
			{
				len = sl_Recv(SockID,Eth_Rev_Buff+Eth_recv_len, sizeof(Eth_Rev_Buff)-Eth_recv_len, 0);
				if(len>0)
				{
					Eth_recv_len += len;				
					processDataFromServer();
				}
				else
					break;

			}else
			break;
		}
		else
		{
			at_printf("fail to select!\r\n");
			break;
		}


		
			
	}
	
	SL_FD_CLR(SockID, &rfd);
#endif	
	//phy is ok, server is closed.  panel is dead.
	at_printf("tcp recv is break!\r\n");
	OutputLed(LEDON, LED_CAN);	 
	sl_Close(SockID);			


}




/*!
    \brief Opening a client side socket and sending data

    This function opens a TCP socket and tries to connect to a Server IP_ADDR
    waiting on port PORT_NUM. If the socket connection is successful then the
    function will send 1000 TCP packets to the server.

    \param[in]      port number on which the server will be listening on

    \return         0 on success, -1 on Error.

    \note

    \warning        A server must be waiting with an open TCP socket on the
                    right port number before calling this function.
                    Otherwise the socket creation will fail.
*/
static _i32 BsdTcpClient(_u16 Port)
{
    SlSockAddrIn_t  Addr;

//    _u16          idx = 0;
    _u16          AddrSize = 0;
    _i16          Status = 0;
//    _u16          LoopCount = 0;
	_u16		  cnt = 0;
	rt_uint32_t   recved;

	
	if(returnConfigLinkMothod()==ETH_WIFI_STYLE)
	{									
		//recvive checked result.
		rt_event_recv(cliEvent,  
						TCP_CLI_WIFI,
						RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,
						RT_WAITING_FOREVER ,
						&recved);
		
	}

	while(1)
	{
		OutputLed(LEDON, LED_CAN);
		cnt = 0;
		
		//establish a TCP connection.
		at_printf("TCP_CREAT  is open\n");
		sl_Close(SockID);   
	
		while(1)
		{
		
			
			/* check link ok!*/ 
			while (!Sys_Setup.app.id	
				   || (dicInfo.secondDicInfo.panelLink == Panel_con_eth)
				  )
				  
			{	
				if(dicInfo.secondDicInfo.panelLink == Panel_con_eth)
				{
					//at_printf("choose ETHERNET connect!\r\n");
				}
				else
				{
					at_printf("id is error = %d \r",Sys_Setup.app.id);
				}
			
				//at_printf("netif link err!\r\n");
				rt_thread_delay(RT_TICK_PER_SECOND );

				continue;
			}	
			dicInfo.secondDicInfo.panelLink = Panel_tryconnect;
			at_printf(" [TCP Client] client is start! \n\r");

		    Addr.sin_family = SL_AF_INET;
		    Addr.sin_port = sl_Htons((_u16)Port);
		    Addr.sin_addr.s_addr = sl_Htonl((_u32)IP_ADDR);
		    AddrSize = sizeof(SlSockAddrIn_t);
			
	
		    SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
		    if( SockID < 0 )
		    {
		    	Cdma_Info.conErrPlace = WIFI_NETWORK_SOCK_ERROR;	
		        at_printf(" [TCP Client] Create socket Error \n\r");
				continue;
		    }

		    Status = sl_Connect(SockID, ( SlSockAddr_t *)&Addr, AddrSize);
		    if( Status < 0 )
		    {
		        sl_Close(SockID);
		        at_printf(" [TCP Client]  TCP connection Error \n\r");
				Cdma_Info.conErrPlace = WIFI_NETWORK_CONNECT_ERROR;
				
				if(RT_EOK ==  rt_event_recv(wifiEvent,  
							  WIFI_AP_CLOSE,
							  RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,
							  RT_WAITING_NO,
							  &recved))
				{
					/* Router disconnected */
					if(recved & WIFI_AP_CLOSE)
					{
						at_printf("wifi ap station is closed.\r\n");
						return SL_FAIL;
					}
				}
				

				cnt++;
				if(cnt > RECONNECT_TIMES) /* connect failed */
				{	
					changeServerInfo();
					cnt = 0;
				}
				
				if(returnConfigLinkMothod()==ETH_WIFI_STYLE)
				{	
					rt_event_send(cliEvent, TCP_CLI_ETH); 
					//recvive checked result.
					rt_event_recv(cliEvent,  
									TCP_CLI_WIFI,
									RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,
									RT_WAITING_FOREVER,
									&recved);
					
				}
						

		    }
			else
			{
				at_printf(" [TCP Client] wifi TCP connection OK! \n\r");
				cnt = 0;
				Cdma_Info.conErrPlace = 
				rt_thread_delay(RT_TICK_PER_SECOND);
				Cdma_Info.currentConnectStyle = CLIENT_WIFI_CONNECT;
				break;

			}


		}

		/* Abox creat */
		{
			entryEthStatus();

			wifiChat();//

			exitEthStatus();
					
		}
	
	
	}
    Status = sl_Close(SockID);
    ASSERT_ON_ERROR(Status);

    return SL_SUCCESS;
}

/*!
    \brief Opening a server side socket and receiving data

    This function opens a TCP socket in Listen mode and waits for an incoming
    TCP connection. If a socket connection is established then the function
    will try to read 1000 TCP packets from the connected client.

    \param[in]      port number on which the server will be listening on

    \return         0 on success, -1 on Error.

    \note           This function will wait for an incoming connection till one
                    is established

    \warning
*/
#if 0
static _i32 BsdTcpServer(_u16 Port)
{
    SlSockAddrIn_t  Addr;
    SlSockAddrIn_t  LocalAddr;

    _u16          idx = 0;
    _u16          AddrSize = 0;
    _i16          SockID = 0;
    _i32          Status = 0;
    _i16          newSockID = 0;
    _u16          LoopCount = 0;
    _i16          recvSize = 0;

    for (idx=0 ; idx<BUF_SIZE ; idx++)
    {
        uBuf.BsdBuf[idx] = (_u8)(idx % 10);
    }

    LocalAddr.sin_family = SL_AF_INET;
    LocalAddr.sin_port = sl_Htons((_u16)Port);
    LocalAddr.sin_addr.s_addr = 0;

    SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    if( SockID < 0 )
    {
        at_printf(" [TCP Server] Create socket Error \n\r");
        ASSERT_ON_ERROR(SockID);
    }
		else
			at_printf(" [TCP Server] Create socket OK\n\r");

    AddrSize = sizeof(SlSockAddrIn_t);
    Status = sl_Bind(SockID, (SlSockAddr_t *)&LocalAddr, AddrSize);
    if( Status < 0 )
    {
        sl_Close(SockID);
        at_printf(" [TCP Server] Socket address assignment Error \n\r");
        ASSERT_ON_ERROR(Status);
    }
		else
        at_printf(" [TCP Server] Socket address assignment OK\n\r");

    Status = sl_Listen(SockID, 0);
    if( Status < 0 )
    {
        sl_Close(SockID);
        at_printf(" [TCP Server] Listen Error \n\r");
        ASSERT_ON_ERROR(Status);
    }
		else
        at_printf(" [TCP Server] Listen OK\n\r");

    newSockID = sl_Accept(SockID, ( struct SlSockAddr_t *)&Addr,
                              (SlSocklen_t*)&AddrSize);
    if( newSockID < 0 )
    {
        sl_Close(SockID);
        at_printf(" [TCP Server] Accept connection Error \n\r");
        ASSERT_ON_ERROR(newSockID);
    }
		else
        at_printf(" [TCP Server] Accept connection OK \n\r");

    while (LoopCount < NO_OF_PACKETS)
    {
        recvSize = BUF_SIZE;
        do
        {
            Status = sl_Recv(newSockID, &(uBuf.BsdBuf[BUF_SIZE - recvSize]), recvSize, 0);
            if( Status <= 0 )
            {
                sl_Close(newSockID);
                sl_Close(SockID);
                at_printf(" [TCP Server] Data recv Error \n\r");
                ASSERT_ON_ERROR(TCP_RECV_ERROR);
            }
						else
                at_printf(" [TCP Server] Data recv OK\n\r");

            recvSize -= Status;
        }
        while(recvSize > 0);

        LoopCount++;
    }

    Status = sl_Close(newSockID);
    ASSERT_ON_ERROR(Status);

    Status = sl_Close(SockID);
    ASSERT_ON_ERROR(Status);

    return SL_SUCCESS;
}
#endif

/*!
    \brief This function initializes the application variables

    \param[in]  None

    \return     0 on success, negative error-code on error
*/
static _i32 initializeAppVariables()
{
    g_Status = 0;

	//wifiSemInit();

	displayBanner();
	 
    return SL_SUCCESS;
}

/*!
    \brief This function displays the application's banner

    \param      None

    \return     None
*/
static void displayBanner()
{
    at_printf("\n\r\n\r");
    at_printf(" TCP socket application - Version ");
    at_printf(APPLICATION_VERSION);
    at_printf("\n\r*******************************************************************************\n\r");
}

static _i8 getSignalStrength()
{
    _u32  IntervalVal = SCAN_INTERVAL;
    _u8   policyOpt = 0;

    _i16 retVal = -1;
    _i16 idx = -1;

    policyOpt = SL_CONNECTION_POLICY(0, 0, 0, 0, 0);
    retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION , policyOpt, NULL, 0);
    if(retVal < 0)
    {
        /* Error */
        return (RSSI_THRESHOLD_VALUE-1);
    }

    /* Enable scan */
    policyOpt = SL_SCAN_POLICY(1);

    /* Set scan policy - this starts the scan */
    retVal = sl_WlanPolicySet(SL_POLICY_SCAN , policyOpt,
                            (_u8 *)(&IntervalVal), sizeof(IntervalVal));
    if(retVal < 0)
    {
        /* Error */
        return (RSSI_THRESHOLD_VALUE-1);
    }

    /* Delay 1 second to verify scan is started */
    Delay(1000);

    pal_Memset(g_NetEntries, '\0', (SCAN_TABLE_SIZE * sizeof(Sl_WlanNetworkEntry_t)));

    /* retVal indicates the valid number of entries
     * The scan results are occupied in g_NetEntries[]
     */
    retVal = sl_WlanGetNetworkList(0, SCAN_TABLE_SIZE, &g_NetEntries[0]);

    /* Stop the scan */
    policyOpt = SL_SCAN_POLICY(0);

    /* Set scan policy - this will stop the scan the scan */
    if(sl_WlanPolicySet(SL_POLICY_SCAN , policyOpt, 0, 0) < 0)
        return (RSSI_THRESHOLD_VALUE-1);

    for(idx = 0; idx < retVal; idx++)
    {
        if(pal_Strcmp(g_NetEntries[idx].ssid, SSID_NAME) == 0)
            return (g_NetEntries[idx].rssi);
    }

    return (RSSI_THRESHOLD_VALUE-1);
}


/*
 * Application's entry point
 */
void wifi_recv_thread(void *parameter)
{
    _i32 retVal = -1;
//    SlNetCfgIpV4Args_t ipV4;
	Cdma_Info.conErrPlace = WIFI_NETWORK_START_ERROR;
    retVal = initializeAppVariables();
    retVal = configureSimpleLinkToDefaultState();
    if(retVal < 0)
    {
        if (DEVICE_NOT_IN_STATION_MODE == retVal)
        {
            at_printf(" Failed to configure the device in its default state \n\r");
        }

        LOOP_FOREVER();
    }

    at_printf(" Device is configured in default state \n\r");

    /*
     * Assumption is that the device is configured in station mode already
     * and it is in its default state
     */
    /* Initializing the CC3100 device */
    retVal = sl_Start(0, 0, 0);
    if ((retVal < 0) ||
        (ROLE_STA != retVal) )
    {
        at_printf(" Failed to start the device \n\r");
        LOOP_FOREVER();
    }

    at_printf(" Device started as STATION \n\r");


	/* DHCP determine */
#if 0
	e_clientConnectStatus cStatus = returnConnectConfig(CLIENT_WIFI_CONNECT);
	switch(cStatus)
	{

		case DHCP_OPEN:
			at_printf(" Configuring device to connect using dhnamic IP \n\r");
		break;
		
		case DHCP_CLOSE:
		{

			ipV4.ipV4 = CONFIG_IP;
		    ipV4.ipV4Mask = AP_MASK;
		    ipV4.ipV4Gateway = AP_GATEWAY;
		    ipV4.ipV4DnsServer = AP_DNS;
		    at_printf(" Configuring device to connect using static IP \n\r");
		    /* After calling this API device will be configure for static IP address.*/
		    retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_STATIC_ENABLE,1,
		                    sizeof(SlNetCfgIpV4Args_t), (_u8 *)&ipV4);
		    if(retVal < 0)
		        LOOP_FOREVER();
		}
		break;

		default:
			break;
			
	}
#endif

    /* Connecting to WLAN AP - Set with static parameters defined at the top
    After this call we will be connected and have IP address */
    while(1)
    {
		retVal = establishConnectionWithAP();
   		if(retVal < 0)
        {
       		at_printf(" Failed to establish connection w/ an AP \n\r");
            LOOP_FOREVER();
        }
		

	    at_printf(" Connection established w/ AP and IP is acquired,sign : %d  \n\r",getSignalStrength());

	    at_printf(" Establishing connection with TCP server \n\r");
	    /*Before proceeding, please make sure to have a server waiting on PORT_NUM*/
	    retVal = BsdTcpClient(PORT_NUM);
	    if(retVal < 0)
	        at_printf(" Failed to establishing connection with TCP server \n\r");
	    else
	        at_printf(" Connection with TCP server established successfully \n\r");

	    at_printf(" Starting TCP server\r\n");

	}
    



    /*After calling this function, you can start sending data to CC3100 IP
    * address on PORT_NUM */
 /*   
    retVal = BsdTcpServer(PORT_NUM);
    if(retVal < 0)
           at_printf(" Failed to start TCP server \n\r");
    else
           at_printf(" TCP client connected successfully \n\r");
*/


    /* Stop the CC3100 device */
    retVal = sl_Stop(SL_STOP_TIMEOUT);
    if(retVal < 0)
        LOOP_FOREVER();


}




/* wifi 模式启动 */
void wifiStyleStart(void)
{
	
	wifiEvent = rt_event_create("AboxEvent",RT_IPC_FLAG_FIFO);
 	if(!wifiEvent)
 	{
		rt_kprintf("wifi event is error !\r\n");
	}


	rt_thread_t tid;

    tid = rt_thread_create("wifiRecv",
							wifi_recv_thread, //send data
							RT_NULL,
							1024*2,
							0x10,//TCP_RECV_PRI, 
							20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);

}



