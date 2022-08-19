
#ifndef __ADSMOD_H__
#define __ADSMOD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef APAXMOD_XP_EXPORTS 
#define ADS_API __declspec(dllexport)
#elif defined APAXMOD_CE_EXPORTS 
#define ADS_API __declspec(dllexport)
#else
#define ADS_API
#endif

#define MODBUS_READCOILSTATUS	1
#define MODBUS_READINPUTSTATUS	2
#define MODBUS_READHOLDREG		3
#define MODBUS_READINPUTREG		4
#define MODBUS_FORCESINGLECOIL	5
#define MODBUS_PRESETSINGLEREG	6
#define MODBUS_FORCEMULTICOIL	15
#define MODBUS_PRESETMULTIREGS	16

#define MODERR_SUCCESS			0
#define MODERR_BASE				800
#define MODEXCEPT_BASE			900

#define MODERR_WSASTART_FAILED		(MODERR_BASE + 1) 
#define MODERR_WSACLEAN_FAILED		(MODERR_BASE + 2) 
#define MODERR_CREATESOCK_FAILED	(MODERR_BASE + 3)
#define MODERR_CREATESVR_FAILED		(MODERR_BASE + 4)
#define MODERR_PARAMETER_INVALID	(MODERR_BASE + 5)
#define MODERR_WAIT_TIMEOUT			(MODERR_BASE + 6)
#define MODERR_CONNECT_FAILED		(MODERR_BASE + 7)
#define MODERR_CLOSESOCKET_FAILED	(MODERR_BASE + 8)
#define MODERR_MEMALLOC_FAILED		(MODERR_BASE + 9)
#define MODERR_CREATECLN_FAILED		(MODERR_BASE + 10)
#define MODERR_TCPCLIENT_INVALID	(MODERR_BASE + 11)
#define MODERR_TCPSEND_FAILED		(MODERR_BASE + 12)
#define MODERR_TCPRECV_FAILED		(MODERR_BASE + 13)
#define MODERR_SOCKET_CLOSED		(MODERR_BASE + 14)
#define MODERR_THREAD_RUNNING		(MODERR_BASE + 15)
#define MODERR_THREAD_STOP			(MODERR_BASE + 16)
#define MODERR_THREAD_BUSY			(MODERR_BASE + 17)
#define MODERR_RTUCLIENT_INVALID	(MODERR_BASE + 18)
#define MODERR_RTUCOM_INUSED		(MODERR_BASE + 19)
#define MODERR_RTUCRC_INVALID		(MODERR_BASE + 20)
#define MODERR_SETPRIO_FAILED		(MODERR_BASE + 21)
#define MODERR_RTUSEND_FAILED		(MODERR_BASE + 22)
#define MODERR_RTURECV_FAILED		(MODERR_BASE + 23)

#define MODERR_MODEXCEPT_01		(MODEXCEPT_BASE + 1)		/* ILLEGAL FUNCTION */
#define MODERR_MODEXCEPT_02		(MODEXCEPT_BASE + 2)		/* ILLEGAL DATA ADDRESS */
#define MODERR_MODEXCEPT_03		(MODEXCEPT_BASE + 3)		/* ILLEGAL DATA VALUE */
#define MODERR_MODEXCEPT_04		(MODEXCEPT_BASE + 4)		/* SLAVE DEVICE FAILURE */
#define MODERR_MODEXCEPT_05		(MODEXCEPT_BASE + 5)		/* ACKNOWLEDGE */
#define MODERR_MODEXCEPT_06		(MODEXCEPT_BASE + 6)		/* SLAVE DEVICE BUSY */
#define MODERR_MODEXCEPT_07		(MODEXCEPT_BASE + 7)		/* NEGATIVE ACKNOWLEDGE */
#define MODERR_MODEXCEPT_08		(MODEXCEPT_BASE + 8)		/* MEMORY PARITY ERROR */
#define MODERR_MODEXCEPT_99		(MODEXCEPT_BASE + 99)		/* Invalid packet */

typedef void (*OnCoilChangedEvent)(int i_iStart, int i_iLen);
typedef void (*OnHoldRegChangedEvent)(int i_iStart, int i_iLen);
typedef void (*OnRemoteTcpClientConnectEvent)(char *i_szIp);
typedef void (*OnRemoteTcpClientDisconnectEvent)(char *i_szIp);

typedef void (*OnConnectTcpServerCompletedEvent)(long i_lResult, char *i_szIp, void *i_Param);
typedef void (*OnDisconnectTcpServerCompletedEvent)(long i_lResult, char *i_szIp, void *i_Param);
typedef void (*OnConnectRtuServerCompletedEvent)(long i_lResult, int i_iCom, void *i_Param);
typedef void (*OnDisconnectRtuServerCompletedEvent)(long i_lResult, int i_iCom, void *i_Param);
typedef void (*OnModbusReadCompletedEvent)(long i_lResult, unsigned char *i_byData, int i_iLen, void *i_Param);
typedef void (*OnModbusWriteCompletedEvent)(long i_lResult, void *i_Param);

LONG ADS_API MOD_Initialize();
LONG ADS_API MOD_Terminate();
LONG ADS_API MOD_GetVersion();

// MODBUS-TCP server functions
LONG ADS_API MOD_StartTcpServer();
LONG ADS_API MOD_StopTcpServer();
LONG ADS_API MOD_SetTcpServerPort(unsigned short i_iPort);
LONG ADS_API MOD_SetTcpServerPriority(int i_iPriority);
LONG ADS_API MOD_SetTcpServerClientIpRestrict(bool i_bRestrict);
LONG ADS_API MOD_GetTcpServerClientIp(int i_iIndex, char *o_szIp);
LONG ADS_API MOD_SetTcpServerClientIp(int i_iIndex, char *i_szIp);
LONG ADS_API MOD_GetTcpServerClientIdle(int *o_iIdleSec);
LONG ADS_API MOD_SetTcpServerClientIdle(int i_iIdleSec);

// MODBUS-RTU server functions
LONG ADS_API MOD_StartRtuServer(int i_iServerIndex, int i_iComIndex, unsigned char i_bySlaveAddr);
LONG ADS_API MOD_StopRtuServer(int i_iServerIndex);
LONG ADS_API MOD_SetRtuServerPriority(int iServerIndex, int iPriority);
LONG ADS_API MOD_SetRtuServerComm(int i_iServerIndex, long i_lBaudrate, int i_iDataBits, int i_iParity, int i_iStop);
LONG ADS_API MOD_SetRtuServerTimeout(int i_iServerIndex, 
									 int i_iReadIntervalTimeout,
									 int i_iReadTotalTimeoutConstant,
									 int i_iReadTotalTimeoutMultiplier,
									 int i_iWriteTotalTimeoutConstant,
									 int i_iWriteTotalTimeoutMultiplier);

// MODBUS server common functions 
LONG ADS_API MOD_GetServerCoil(int i_iStart, int i_iLen, unsigned char *o_byBuf, int *o_iRetLen);
LONG ADS_API MOD_GetServerInput(int i_iStart, int i_iLen, unsigned char *o_byBuf, int *o_iRetLen);
LONG ADS_API MOD_GetServerInputReg(int i_iStart, int i_iLen, unsigned char *o_byBuf, int *o_iRetLen);
LONG ADS_API MOD_GetServerHoldReg(int i_iStart, int i_iLen, unsigned char *o_byBuf, int *o_iRetLen);

LONG ADS_API MOD_SetServerCoil(int i_iStart, int i_iLen, unsigned char *i_byBuf);
LONG ADS_API MOD_SetServerInput(int i_iStart, int i_iLen, unsigned char *i_byBuf);
LONG ADS_API MOD_SetServerInputReg(int i_iStart, int i_iLen, unsigned char *i_byBuf);
LONG ADS_API MOD_SetServerHoldReg(int i_iStart, int i_iLen, unsigned char *i_byBuf);

// MODBUS server, set remote client write event handler
LONG ADS_API MOD_SetServerCoilChangedEventHandler(OnCoilChangedEvent i_evtHandle);
LONG ADS_API MOD_SetServerHoldRegChangedEventHandler(OnHoldRegChangedEvent i_evtHandle);

// MODBUS-TCP server, remote client connect to/disconnect from server event handler
LONG ADS_API MOD_SetTcpServerClientConnectEventHandler(OnRemoteTcpClientConnectEvent i_evtHandle);
LONG ADS_API MOD_SetTcpServerClientDisconnectEventHandler(OnRemoteTcpClientDisconnectEvent i_evtHandle);

// MODBUS server, remote client write to server event
LONG ADS_API MOD_WaitServerCoilChangedEvent(unsigned long i_ulMilliseconds, int *o_iStart, int *o_iLen);
LONG ADS_API MOD_WaitServerHoldRegChangedEvent(unsigned long i_ulMilliseconds, int *o_iStart, int *o_iLen);

// MODBUS-TCP server, remote client connect to/disconnect from server event
LONG ADS_API MOD_WaitTcpServerClientConnectEvent(unsigned long i_ulMilliseconds, char *o_szIP);
LONG ADS_API MOD_WaitTcpServerClientDisconnectEvent(unsigned long i_ulMilliseconds, char *o_szIP);

// ============================================================================================
// MODBUS-TCP client functions
// ============================================================================================
LONG ADS_API MOD_AddTcpClientConnect(char *i_szServerIp, 
									 int i_iScanInterval, 
									 int i_iConnectTimeout, 
									 int i_iTransactTimeout, 
									 OnConnectTcpServerCompletedEvent connEvtHandle,
									 OnDisconnectTcpServerCompletedEvent DisconnEvtHandle,
									 void *i_Param,
									 unsigned long *o_ulClientHandle);
LONG ADS_API MOD_AddTcpClientConnectEx(char *i_szServerIp, 
									   unsigned short i_usPort,
									   int i_iScanInterval, 
									   int i_iConnectTimeout, 
									   int i_iTransactTimeout, 
									   OnConnectTcpServerCompletedEvent connEvtHandle,
									   OnDisconnectTcpServerCompletedEvent DisconnEvtHandle,
									   void *i_Param,
									   unsigned long *o_ulClientHandle);
LONG ADS_API MOD_AddTcpClientReadTag(unsigned long i_ulClientHandle,
									 unsigned char i_byAddr,
									 unsigned char i_byType,
									 unsigned short i_iStartIndex,
									 unsigned short i_iTotalPoint,
									 OnModbusReadCompletedEvent evtHandle);

LONG ADS_API MOD_StartTcpClient();
LONG ADS_API MOD_StopTcpClient();
LONG ADS_API MOD_SetTcpClientPriority(int iPriority);
LONG ADS_API MOD_ReleaseTcpClientResource();

LONG ADS_API MOD_ForceTcpClientSingleCoil(unsigned long i_ulClientHandle,
										  unsigned char i_byAddr,
										  unsigned short i_iIndex,
										  unsigned short i_iData,
										  OnModbusWriteCompletedEvent evtHandle);
LONG ADS_API MOD_PresetTcpClientSingleReg(unsigned long i_ulClientHandle,
										  unsigned char i_byAddr,
										  unsigned short i_iIndex,
										  unsigned short i_iData,
										  OnModbusWriteCompletedEvent evtHandle);
LONG ADS_API MOD_ForceTcpClientMultiCoils(unsigned long i_ulClientHandle,
										  unsigned char i_byAddr,
										  unsigned short i_iStartIndex,
										  unsigned short i_iTotalPoint,
										  unsigned char i_byTotalByte,
										  unsigned char *i_byData,
										  OnModbusWriteCompletedEvent evtHandle);
LONG ADS_API MOD_ForceTcpClientMultiRegs( unsigned long i_ulClientHandle,
										  unsigned char i_byAddr,
										  unsigned short i_iStartIndex,
										  unsigned short i_iTotalPoint,
										  unsigned char i_byTotalByte,
										  unsigned char *i_byData,
										  OnModbusWriteCompletedEvent evtHandle);

// ============================================================================================
// MODBUS-RTU client functions
// ============================================================================================
LONG ADS_API MOD_AddRtuClientConnect(int i_iCom,
							 		 unsigned long i_iBaudrate,
							 		 unsigned char i_byDataBits,
							 		 unsigned char i_byParity,
							 		 unsigned char i_byStopBits,
							 		 int i_iScanInterval, 
							 		 int i_iTransactTimeout,
							 		 OnConnectRtuServerCompletedEvent connEvtHandle,
							 		 OnDisconnectRtuServerCompletedEvent disconnevtHandle,
									 void *i_Param,
							 		 unsigned long *o_ulClientHandle);
LONG ADS_API MOD_AddRtuClientReadTag(unsigned long i_ulClientHandle,
									 unsigned char i_byAddr,
									 unsigned char i_byType,
									 unsigned short i_iStartIndex,
									 unsigned short i_iTotalPoint,
									 OnModbusReadCompletedEvent evtHandle);
									 
LONG ADS_API MOD_StartRtuClient();
LONG ADS_API MOD_StopRtuClient();
LONG ADS_API MOD_SetRtuClientPriority(int iPriority);
LONG ADS_API MOD_ReleaseRtuClientResource();

LONG ADS_API MOD_ForceRtuClientSingleCoil(unsigned long i_ulClientHandle,
										  unsigned char i_byAddr,
										  unsigned short i_iIndex,
										  unsigned short i_iData,
										  OnModbusWriteCompletedEvent evtHandle);
LONG ADS_API MOD_PresetRtuClientSingleReg(unsigned long i_ulClientHandle,
										  unsigned char i_byAddr,
										  unsigned short i_iIndex,
										  unsigned short i_iData,
										  OnModbusWriteCompletedEvent evtHandle);
LONG ADS_API MOD_ForceRtuClientMultiCoils(unsigned long i_ulClientHandle,
										  unsigned char i_byAddr,
										  unsigned short i_iStartIndex,
										  unsigned short i_iTotalPoint,
										  unsigned char i_byTotalByte,
										  unsigned char *i_byData,
										  OnModbusWriteCompletedEvent evtHandle);
LONG ADS_API MOD_ForceRtuClientMultiRegs( unsigned long i_ulClientHandle,
										  unsigned char i_byAddr,
										  unsigned short i_iStartIndex,
										  unsigned short i_iTotalPoint,
										  unsigned char i_byTotalByte,
										  unsigned char *i_byData,
										  OnModbusWriteCompletedEvent evtHandle);

#ifdef __cplusplus
}
#endif

#endif