#include <conio.h>
#include <stdint.h>

#include "stdafx.h"
#include "CAdvantechModbus.h"
#include "CMgrPid.h"

#include "PnxUtil.h"

#define bytes_to_u16(MSB,LSB) (((unsigned int) ((unsigned char) MSB)) & 255)<<8 | (((unsigned char) LSB) & 255) 

// -------------------------------------------------------------------------------------------------
// EVENTS

void	ConnectTcpServerCompletedEventHandler(long lResult, char *i_szIp, void *i_Param);
void	DisconnectTcpServerCompletedEventHandler(long lResult, char *i_szIp, void *i_Param);
void	ModbusWriteCompletedEventHandler(long lResult, void *i_Param);

// -----------------------------------------------------------------------------

void	ClientReadCoilHandler(long lResult, unsigned char *i_byData, int i_iLen, void *i_Param);
void	ClientReadHoldRegHandler(long lResult, unsigned char *i_byData, int i_iLen, void *i_Param);

void	ReadHoldRegAIValueHandler(long lResult, unsigned char *i_byData, int i_iLen, void *i_Param);
void	ReadHoldRegAIStatusHandler(long lResult, unsigned char *i_byData, int i_iLen, void *i_Param);
void	ReadHoldRegAITypeCodeHandler(long lResult, unsigned char *i_byData, int i_iLen, void *i_Param);

void	ReadHoldRegAOValueHandler(long lResult, unsigned char *i_byData, int i_iLen, void *i_Param);
void	ReadHoldRegAOTypeCodeHandler(long lResult, unsigned char *i_byData, int i_iLen, void *i_Param);
void	ReadHoldRegAOStartupValueHandler(long lResult, unsigned char *i_byData, int i_iLen, void *i_Param);

// -----------------------------------------------------------------------------

int m_nDeviceIndexNo = -1;

//DWORD m_dwReadFail;
//DWORD m_dwWriteFail;

// -----------------------------------------------------------------------------

const unsigned short ADAM6024_AI_CHANNEL_MAX_COUNT			= 6;
const unsigned short ADAM6024_AO_CHANNEL_MAX_COUNT			= 2;
const unsigned char  ADAM6024_DEVICE_ADDRESS				= 1;

const unsigned short ADAM6024_START_INDEX_AI_VALUE			= 1;
const unsigned short ADAM6024_START_INDEX_AO_VALUE			= 11;
const unsigned short ADAM6024_START_INDEX_AI_STATUS			= 21;
const unsigned short ADAM6024_START_INDEX_AI_TYPE_CODE		= 201;
const unsigned short ADAM6024_START_INDEX_AO_TYPE_CODE		= 209;
const unsigned short ADAM6024_START_INDEX_AO_STARTUP_VALUE	= 401;

long	m_CurrentAIValue[ADAM6024_AI_CHANNEL_MAX_COUNT];
long	m_CurrentAIStatus[ADAM6024_AI_CHANNEL_MAX_COUNT];
long	m_CurrentAITypeCode[ADAM6024_AI_CHANNEL_MAX_COUNT];

long	m_CurrentAOValue[ADAM6024_AO_CHANNEL_MAX_COUNT];
long	m_CurrentAOTypeCode[ADAM6024_AO_CHANNEL_MAX_COUNT];
long	m_CurrentAOStartup[ADAM6024_AO_CHANNEL_MAX_COUNT];

// -------------------------------------------------------------------------------------------------

CAdvantechModbus::CAdvantechModbus()
{
	Initialize();
}

CAdvantechModbus::~CAdvantechModbus()
{
	Dispose();
}

bool CAdvantechModbus::Initialize()
{
	InitializeVariables();

	return true;
}

void CAdvantechModbus::Dispose()
{
	StopThread();
}

bool CAdvantechModbus::InitializeVariables()
{
	m_pMeasureThread = NULL;
	m_bLoopMeasure = false;

	return true;
}

// -------------------------------------------------------------------------------------------------
// EVENT

void ConnectTcpServerCompletedEventHandler(long lResult, char * i_szIp, void * i_Param)
{
	CString logMessage;
	logMessage.Format("%s [MODBUS][COMM I/F][Advantech][DEVICE_NO:%d] Connect to '%s' result = %d\n", CPnxUtil::GetMgr()->GetAllDate(), m_nDeviceIndexNo, i_szIp, lResult);
	//SYSLOG(System, logMessage);
	printf(logMessage);
}

void DisconnectTcpServerCompletedEventHandler(long lResult, char * i_szIp, void * i_Param)
{
	CString logMessage;
	logMessage.Format("%s [MODBUS][COMM I/F][Advantech][DEVICE_NO:%d] Disconnect from '%s' result = %d\n", CPnxUtil::GetMgr()->GetAllDate(), m_nDeviceIndexNo, i_szIp, lResult);
	//SYSLOG(System, logMessage);
	printf(logMessage);
}

void ModbusWriteCompletedEventHandler(long lResult, void * i_Param)
{
	CString logMessage;
	logMessage.Format("%s [MODBUS][COMM I/F][Advantech][DEVICE_NO:%d][RECV] Write Completed. RESULT='%d'\n", CPnxUtil::GetMgr()->GetAllDate(), m_nDeviceIndexNo, lResult);
	printf(logMessage);

	if (lResult != MODERR_SUCCESS)
	{
		
	}
}

// -----------------------------------------------------------------------------

/**
 * .
 * 
 * METHOD_NAME : ReadHoldRegAIValueHandler
 * METHOD_TYPE : -
 * DESCRIPTION : 
 */
void ReadHoldRegAIValueHandler(long lResult, unsigned char * i_byData, int i_iLen, void * i_Param)
{
	CString currentTime = CPnxUtil::GetMgr()->GetAllDate();
	CString logMessage;
	

	if (lResult != MODERR_SUCCESS) {
		logMessage.Format(_T("%s [MODBUS][COMM I/F][RECV][READHOLDREG][AI_VALUE   ] Failed to read err = '%d'\n"), currentTime, lResult);
		printf(logMessage);
	}

	// ----------------------------------------------------------------------------

	CString tempLogString;

	int HEX_VALUE_SIZE_16BIT = 2;
	int nChannelCount = -1;
	CString strPrevValue;
	CString strResultValue;
	int nResultValue = 0;
	
	for (int dataCount = 0; dataCount < i_iLen; dataCount++)
	{
		CString sTempValue;
		sTempValue.Format("%02x", i_byData[dataCount]);
		long nTempValue = CPnxUtil::GetMgr()->String2Hex(sTempValue);

		if (dataCount % HEX_VALUE_SIZE_16BIT == 0) {
			strPrevValue = sTempValue;
		}
		else if (dataCount % HEX_VALUE_SIZE_16BIT > 0) {
			strResultValue = strPrevValue + sTempValue;
			nResultValue = CPnxUtil::GetMgr()->String2Hex(strResultValue);

			nChannelCount++;
			m_CurrentAIValue[nChannelCount] = nResultValue;

			tempLogString.Format(_T("CH:%02d='%d'"), nChannelCount, nResultValue);
			logMessage += tempLogString;
			if (nChannelCount < ADAM6024_AI_CHANNEL_MAX_COUNT) {
				logMessage += ", ";
			}
		}
	}

	printf(_T("%s [MODBUS][COMM I/F][DEVICE_NO:%d][RECV][READHOLDREG][AI_VALUE   ][LENGTH='%02d'] %s\n"), currentTime, m_nDeviceIndexNo, i_iLen, logMessage);
}

/**
 * .
 * 
 * METHOD_NAME : ReadHoldRegAIStatusHandler
 * METHOD_TYPE : -
 * DESCRIPTION : AI status
?*					Bit value = 0: normal
?*					Bit value = 1: over high
?*					Bit value = 2: over low
?*					Bit value = 0: invalid calibration
 */
void ReadHoldRegAIStatusHandler(long lResult, unsigned char * i_byData, int i_iLen, void * i_Param)
{
	CString currentTime = CPnxUtil::GetMgr()->GetAllDate();
	CString logMessage;

	if (lResult != MODERR_SUCCESS) {
		logMessage.Format(_T("%s [MODBUS][COMM I/F][RECV][READHOLDREG][AI_STATUS  ] Failed to read err = '%d'\n"), currentTime, lResult);
		printf(logMessage);
	}

	// ----------------------------------------------------------------------------

	CString tempLogString;

	int HEX_VALUE_SIZE_16BIT = 2;
	int nChannelCount = -1;
	CString strPrevValue;
	CString strResultValue;
	int nResultValue = 0;

	for (int dataCount = 0; dataCount < i_iLen; dataCount++)
	{
		CString sTempValue;
		sTempValue.Format("%02x", i_byData[dataCount]);
		long nTempValue = CPnxUtil::GetMgr()->String2Hex(sTempValue);

		if (dataCount % HEX_VALUE_SIZE_16BIT == 0) {
			strPrevValue = sTempValue;
		}
		else if (dataCount % HEX_VALUE_SIZE_16BIT > 0) {
			strResultValue = strPrevValue + sTempValue;
			nResultValue = CPnxUtil::GetMgr()->String2Hex(strResultValue);

			nChannelCount++;
			m_CurrentAIStatus[nChannelCount] = nResultValue;

			tempLogString.Format(_T("CH:%02d='%d'"), nChannelCount, nResultValue);
			logMessage += tempLogString;
			if (nChannelCount < ADAM6024_AI_CHANNEL_MAX_COUNT) {
				logMessage += ", ";
			}
		}
	}

	printf(_T("%s [MODBUS][COMM I/F][DEVICE_NO:%d][RECV][READHOLDREG][AI_STATUS  ][LENGTH='%02d'] %s\n"), currentTime, m_nDeviceIndexNo, i_iLen, logMessage);
}

/**
 * .
 * 
 * METHOD_NAME : ReadHoldRegAITypeCodeHandler
 * METHOD_TYPE : -
 * DESCRIPTION : 
 */
void ReadHoldRegAITypeCodeHandler(long lResult, unsigned char * i_byData, int i_iLen, void * i_Param)
{
	CString currentTime = CPnxUtil::GetMgr()->GetAllDate();
	CString logMessage;

	if (lResult != MODERR_SUCCESS) {
		logMessage.Format(_T("%s [MODBUS][COMM I/F][RECV][READHOLDREG][AI_TYPECODE] Failed to read err = '%d'\n"), currentTime, lResult);
		printf(logMessage);
	}

	// ----------------------------------------------------------------------------

	CString tempLogString;

	int HEX_VALUE_SIZE_16BIT = 2;
	int nChannelCount = -1;
	CString strPrevValue;
	CString strResultValue;
	int nResultValue = 0;

	for (int dataCount = 0; dataCount < i_iLen; dataCount++)
	{
		CString sTempValue;
		sTempValue.Format("%02x", i_byData[dataCount]);
		long nTempValue = CPnxUtil::GetMgr()->String2Hex(sTempValue);

		if (dataCount % HEX_VALUE_SIZE_16BIT == 0) {
			strPrevValue = sTempValue;
		}
		else if (dataCount % HEX_VALUE_SIZE_16BIT > 0) {
			strResultValue = strPrevValue + sTempValue;
			nResultValue = CPnxUtil::GetMgr()->String2Hex(strResultValue);

			nChannelCount++;
			m_CurrentAITypeCode[nChannelCount] = nResultValue;

			tempLogString.Format(_T("CH:%02d='%d'"), nChannelCount, nResultValue);
			logMessage += tempLogString;
			if (nChannelCount < ADAM6024_AI_CHANNEL_MAX_COUNT) {
				logMessage += ", ";
			}
		}
	}

	printf(_T("%s [MODBUS][COMM I/F][DEVICE_NO:%d][RECV][READHOLDREG][AI_TYPECODE][LENGTH='%02d'] %s\n"), currentTime, m_nDeviceIndexNo, i_iLen, logMessage);
}

/**
 * .
 * 
 * METHOD_NAME : ReadHoldRegAOValueHandler
 * METHOD_TYPE : -
 * DESCRIPTION : 
 */
void ReadHoldRegAOValueHandler(long lResult, unsigned char * i_byData, int i_iLen, void * i_Param)
{
	CString currentTime = CPnxUtil::GetMgr()->GetAllDate();
	CString logMessage;

	if (lResult != MODERR_SUCCESS) {
		logMessage.Format(_T("%s [MODBUS][COMM I/F][RECV][READHOLDREG][AO_VALUE   ] Failed to read err = '%d'\n"), currentTime, lResult);
		printf(logMessage);
	}

	// ----------------------------------------------------------------------------

	CString tempLogString;

	int HEX_VALUE_SIZE_16BIT = 2;
	int nChannelCount = -1;
	CString strPrevValue;
	CString strResultValue;
	int nResultValue = 0;

	for (int dataCount = 0; dataCount < i_iLen; dataCount++)
	{
		CString sTempValue;
		sTempValue.Format("%02x", i_byData[dataCount]);
		long nTempValue = CPnxUtil::GetMgr()->String2Hex(sTempValue);

		if (dataCount % HEX_VALUE_SIZE_16BIT == 0) {
			strPrevValue = sTempValue;
		}
		else if (dataCount % HEX_VALUE_SIZE_16BIT > 0) {
			strResultValue = strPrevValue + sTempValue;
			nResultValue = CPnxUtil::GetMgr()->String2Hex(strResultValue);

			float fVoltageValue = nResultValue / 409.5;

			nChannelCount++;
			m_CurrentAOValue[nChannelCount] = nResultValue;

			tempLogString.Format(_T("CH:%02d='%d=V:%03f'"), nChannelCount, nResultValue, fVoltageValue);
			logMessage += tempLogString;
			if (nChannelCount < ADAM6024_AI_CHANNEL_MAX_COUNT) {
				logMessage += ", ";
			}
		}
	}

	printf(_T("%s [MODBUS][COMM I/F][DEVICE_NO:%d][RECV][READHOLDREG][AO_VALUE   ][LENGTH='%02d'] %s\n"), currentTime, m_nDeviceIndexNo, i_iLen, logMessage);
}

/**
 * .
 * 
 * METHOD_NAME : ReadHoldRegAOTypeCodeHandler
 * METHOD_TYPE : -
 * DESCRIPTION : 
 */
void ReadHoldRegAOTypeCodeHandler(long lResult, unsigned char * i_byData, int i_iLen, void * i_Param)
{
	CString currentTime = CPnxUtil::GetMgr()->GetAllDate();
	CString logMessage;

	if (lResult != MODERR_SUCCESS) {
		logMessage.Format(_T("%s [MODBUS][COMM I/F][RECV][READHOLDREG][AO_TYPECODE] Failed to read err = '%d'\n"), currentTime, lResult);
		printf(logMessage);
	}

	// ----------------------------------------------------------------------------

	CString tempLogString;

	int HEX_VALUE_SIZE_16BIT = 2;
	int nChannelCount = -1;
	CString strPrevValue;
	CString strResultValue;
	int nResultValue = 0;

	for (int dataCount = 0; dataCount < i_iLen; dataCount++)
	{
		CString sTempValue;
		sTempValue.Format("%02x", i_byData[dataCount]);
		long nTempValue = CPnxUtil::GetMgr()->String2Hex(sTempValue);

		if (dataCount % HEX_VALUE_SIZE_16BIT == 0) {
			strPrevValue = sTempValue;
		}
		else if (dataCount % HEX_VALUE_SIZE_16BIT > 0) {
			strResultValue = strPrevValue + sTempValue;
			nResultValue = CPnxUtil::GetMgr()->String2Hex(strResultValue);

			nChannelCount++;
			m_CurrentAOTypeCode[nChannelCount] = nResultValue;

			tempLogString.Format(_T("CH:%02d='%d'"), nChannelCount, nResultValue);
			logMessage += tempLogString;
			if (nChannelCount < ADAM6024_AI_CHANNEL_MAX_COUNT) {
				logMessage += ", ";
			}
		}
	}

	printf(_T("%s [MODBUS][COMM I/F][DEVICE_NO:%d][RECV][READHOLDREG][AO_CODETYPE][LENGTH='%02d'] %s\n"), currentTime, m_nDeviceIndexNo, i_iLen, logMessage);
}

/**
 * .
 * 
 * METHOD_NAME : ReadHoldRegAOStartupValueHandler
 * METHOD_TYPE : -
 * DESCRIPTION : 
 */
void ReadHoldRegAOStartupValueHandler(long lResult, unsigned char * i_byData, int i_iLen, void * i_Param)
{
	CString currentTime = CPnxUtil::GetMgr()->GetAllDate();
	CString logMessage;

	if (lResult != MODERR_SUCCESS) {
		logMessage.Format(_T("%s [MODBUS][COMM I/F][RECV][READHOLDREG][AO_STARTUP ] Failed to read err = '%d'\n"), currentTime, lResult);
		printf(logMessage);
	}

	// ----------------------------------------------------------------------------

	CString tempLogString;

	int HEX_VALUE_SIZE_16BIT = 2;
	int nChannelCount = -1;
	CString strPrevValue;
	CString strResultValue;
	int nResultValue = 0;

	for (int dataCount = 0; dataCount < i_iLen; dataCount++)
	{
		CString sTempValue;
		sTempValue.Format("%02x", i_byData[dataCount]);
		long nTempValue = CPnxUtil::GetMgr()->String2Hex(sTempValue);

		if (dataCount % HEX_VALUE_SIZE_16BIT == 0) {
			strPrevValue = sTempValue;
		}
		else if (dataCount % HEX_VALUE_SIZE_16BIT > 0) {
			strResultValue = strPrevValue + sTempValue;
			nResultValue = CPnxUtil::GetMgr()->String2Hex(strResultValue);

			nChannelCount++;
			m_CurrentAOStartup[nChannelCount] = nResultValue;

			tempLogString.Format(_T("CH:%02d='%d'"), nChannelCount, nResultValue);
			logMessage += tempLogString;
			if (nChannelCount < ADAM6024_AI_CHANNEL_MAX_COUNT) {
				logMessage += ", ";
			}
		}
	}

	printf(_T("%s [MODBUS][COMM I/F][DEVICE_NO:%d][RECV][READHOLDREG][AO_STARTUP ][LENGTH='%02d'] %s\n"), currentTime, m_nDeviceIndexNo, i_iLen, logMessage);
}

// -------------------------------------------------------------------------------------------------

int CAdvantechModbus::OpenModbus(CString strIpAddress, int nPort, int deviceIndexNo)
{
	m_strIpAddress = strIpAddress;
	m_nPortNo = nPort;
	m_nDeviceIndexNo = deviceIndexNo;

	// MOD_Initialize
	// Initialize the MODBUS library resource.
	long resultValue = MOD_Initialize();

	printf(_T("[PID][COMM I/F][IP:%s][PORT:%d] OPEN MODBUS. REULST='%d'\n"), m_strIpAddress, m_nPortNo, resultValue);

	return resultValue;
}

int CAdvantechModbus::CloseModbus()
{
	// MOD_ReleaseTcpClientResource
	// Release all the allocated memory for clients. This function has to be called after calling MOD_StopTcpClient
	long resultValue = MOD_ReleaseTcpClientResource();
	if (MODERR_SUCCESS != resultValue)
	{ }

	// MOD_Terminate
	// Terminate the MODBUS library and release resource.
	// Before calling this function, all MODBUS clients and servers have to be stopped.
	resultValue = MOD_Terminate();
	if (MODERR_SUCCESS != resultValue)
	{ }

	return resultValue;
}

int CAdvantechModbus::ConnectServer()
{
	//unsigned long ulClientHandle;

	int SCAN_INTERVAL = 300;
	int TIMEOUT_CONNECTION = 3000;
	int TIMEOUT_TRANSACTION = 100;

	// MOD_AddTcpClientConnect
	// Add a MODBUS - TCP client into the polling list.This function has to be called before calling MOD_StartTcpClient.
	long resultValue = MOD_AddTcpClientConnect(LPSTR(LPCTSTR(m_strIpAddress)),	// remote server IP
												SCAN_INTERVAL,					// scan interval (ms)
												TIMEOUT_CONNECTION,				// connection timeout (ms)
												TIMEOUT_TRANSACTION,			// transaction timeout (ms)
												ConnectTcpServerCompletedEventHandler,
												DisconnectTcpServerCompletedEventHandler,
												NULL,
												&m_ulClientHandle);

	if (MODERR_SUCCESS != resultValue) {
		return resultValue;
	}

	CString logMessage;
	logMessage.Format(_T("[PID][COMM I/F][IP:%s][PORT:%d] CONNECTED. SCAN_INTERVAL='%d', CONNECTION_TIMEOUT='%d', TRANSACTION_TIMEOUT='%d'\n")
						, m_strIpAddress, m_nPortNo, SCAN_INTERVAL, TIMEOUT_CONNECTION, TIMEOUT_TRANSACTION);
	printf(logMessage);
	
	// -------------------------------------------------------------------------

	// MOD_AddTcpClientReadTag
	// Add a MODBUS-TCP client tag into the data polling list.
	// This function has to be called after calling MOD_AddTcpClientConnect, and before calling MOD_StartTcpClient.
	//resultValue = MOD_AddTcpClientReadTag(m_ulClientHandle,
	//										1,								// Addr
	//										MODBUS_READCOILSTATUS,			// Type
	//										1,								// StartIndex
	//										32,								// TotalPoint
	//										ClientReadCoil_1_32_Handler);	// EventHandler
	//if (MODERR_SUCCESS != resultValue)
	//	return resultValue;

	//resultValue = MOD_AddTcpClientReadTag(m_ulClientHandle,
	//										1,								// Addr
	//										MODBUS_READCOILSTATUS,			// Type
	//										65,								// StartIndex
	//										32,								// TotalPoint
	//										ClientReadCoil_65_96_Handler);	// EventHandler

	//if (MODERR_SUCCESS != resultValue)
	//	return resultValue;

	// -------------------------------------------------------------------------

	resultValue = SetPollingHoldRegAIValue();
	if (MODERR_SUCCESS != resultValue) {
		return resultValue;
	}

	resultValue = SetPollingHoldRegAIStatus();
	if (MODERR_SUCCESS != resultValue) {
		return resultValue;
	}

	//resultValue = SetPollingHoldRegAITypeCode();
	//if (MODERR_SUCCESS != resultValue) {
	//	return resultValue;
	//}

	resultValue = SetPollingHoldRegAOValue();
	if (MODERR_SUCCESS != resultValue) {
		return resultValue;
	}

	//resultValue = SetPollingHoldRegAOTypeCode();
	//if (MODERR_SUCCESS != resultValue) {
	//	return resultValue;
	//}

	//resultValue = SetPollingHoldRegAOStartupValue();
	//if (MODERR_SUCCESS != resultValue) {
	//	return resultValue;
	//}

	// -------------------------------------------------------------------------

	// MOD_StartTcpClient
	// Start the MODBUS-TCP client. After calling with MODERR_SUCCESS returned, the client thread will start polling all clients and their reading tags.
	resultValue = MOD_StartTcpClient();
	if (MODERR_SUCCESS != resultValue) {
		return resultValue;
	}
	
	// MOD_SetTcpClientPriority
	// Set the priority of the MODBUS-TCP client running thread.
	// Before calling this function, the MODBUS - TCP client has to be successfully started
	resultValue = MOD_SetTcpClientPriority(THREAD_PRIORITY_HIGHEST);
	if (MODERR_SUCCESS != resultValue) {
		return resultValue;
	}

	// -------------------------------------------------------------------------

	return resultValue;
}

int CAdvantechModbus::DisconnectServer()
{
	m_bLoopMeasure = false;

	// MOD_StopTcpClient
	// Stop the MODBUS-TCP client. After calling this function, the client thread will terminate and all polling will stop
	long resultValue = MOD_StopTcpClient();
	if (MODERR_SUCCESS != resultValue)
	{ }

	return 0;
}

// -----------------------------------------------------------------------------

long CAdvantechModbus::GetAIValue(int nChannelNo)
{
	return m_CurrentAIValue[nChannelNo];
}

long CAdvantechModbus::GetAIStatus(int nChannelNo)
{
	return m_CurrentAIStatus[nChannelNo];
}

long CAdvantechModbus::GetAITypeCode(int nChannelNo)
{
	return m_CurrentAITypeCode[nChannelNo];
}

long CAdvantechModbus::GetAOValue(int nChannelNo)
{
	return m_CurrentAOValue[nChannelNo];
}

long CAdvantechModbus::GetAOTypeCode(int nChannelNo)
{
	return m_CurrentAOTypeCode[nChannelNo];
}

long CAdvantechModbus::GetAOStartupValue(int nChannelNo)
{
	return m_CurrentAOStartup[nChannelNo];
}

long CAdvantechModbus::SetForceHoldRegAITypeCode(int nChannelNo, int nTypeCode)
{
	CString tempValue;
	tempValue.Format("%04X", nTypeCode);

	unsigned char byData[2] = { 0 };
	byData[0] = CPnxUtil::GetMgr()->String2Hex(tempValue.Left(2));
	byData[1] = CPnxUtil::GetMgr()->String2Hex(tempValue.Right(2));

	unsigned short TOTAL_POINT = 1;
	unsigned char  TOTAL_BYTE = 2;

	unsigned short tempChannelNo = ADAM6024_START_INDEX_AO_TYPE_CODE + nChannelNo;
	return MOD_ForceTcpClientMultiRegs(m_ulClientHandle, ADAM6024_DEVICE_ADDRESS
										, tempChannelNo, TOTAL_POINT, TOTAL_BYTE, byData, ModbusWriteCompletedEventHandler);
}

long CAdvantechModbus::SetForceHoldRegAOValue(int nChannelNo, long nSetValue)
{
	CString tempValue;
	tempValue.Format("%04X", nSetValue);

	unsigned char byData[2] = { 0 };
	byData[0] = CPnxUtil::GetMgr()->String2Hex(tempValue.Left(2));
	byData[1] = CPnxUtil::GetMgr()->String2Hex(tempValue.Right(2));

	unsigned short TOTAL_POINT = 1;
	unsigned char  TOTAL_BYTE = 2;

	unsigned short tempChannelNo = ADAM6024_START_INDEX_AO_VALUE + nChannelNo;
	return MOD_ForceTcpClientMultiRegs(m_ulClientHandle, ADAM6024_DEVICE_ADDRESS
										, tempChannelNo, TOTAL_POINT, TOTAL_BYTE, byData, ModbusWriteCompletedEventHandler);
}

long CAdvantechModbus::SetForceHoldRegAOTypeCode(int nChannelNo, int nTypeCode)
{
	CString tempValue;
	tempValue.Format("%04X", nTypeCode);

	unsigned char byData[2] = { 0 };
	byData[0] = CPnxUtil::GetMgr()->String2Hex(tempValue.Left(2));
	byData[1] = CPnxUtil::GetMgr()->String2Hex(tempValue.Right(2));

	unsigned short TOTAL_POINT = 1;
	unsigned char  TOTAL_BYTE = 2;

	unsigned short tempChannelNo = ADAM6024_START_INDEX_AO_TYPE_CODE + nChannelNo;
	return MOD_ForceTcpClientMultiRegs(m_ulClientHandle, ADAM6024_DEVICE_ADDRESS
										, tempChannelNo, TOTAL_POINT, TOTAL_BYTE, byData, ModbusWriteCompletedEventHandler);
}

long CAdvantechModbus::SetForceHoldRegAOStartupValue(int nChannelNo, long nSetValue)
{
	CString tempValue;
	tempValue.Format("%04X", nSetValue);

	unsigned char byData[2] = { 0 };
	byData[0] = CPnxUtil::GetMgr()->String2Hex(tempValue.Left(2));
	byData[1] = CPnxUtil::GetMgr()->String2Hex(tempValue.Right(2));

	unsigned short TOTAL_POINT = 1;
	unsigned char  TOTAL_BYTE = 2;

	unsigned short tempChannelNo = ADAM6024_START_INDEX_AO_STARTUP_VALUE + nChannelNo;
	return MOD_ForceTcpClientMultiRegs(m_ulClientHandle, ADAM6024_DEVICE_ADDRESS
										, tempChannelNo, TOTAL_POINT, TOTAL_BYTE, byData, ModbusWriteCompletedEventHandler);
}

long CAdvantechModbus::SetForceHoldRegAOValueAll(int nSetValue)
{
	CString tempValue;
	tempValue.Format("%04X", nSetValue);

	unsigned char byData[4] = { 0 };
	byData[0] = CPnxUtil::GetMgr()->String2Hex(tempValue.Left(2));
	byData[1] = CPnxUtil::GetMgr()->String2Hex(tempValue.Right(2));
	byData[2] = CPnxUtil::GetMgr()->String2Hex(tempValue.Left(2));
	byData[3] = CPnxUtil::GetMgr()->String2Hex(tempValue.Right(2));

	unsigned short TOTAL_POINT = 2;
	unsigned char  TOTAL_BYTE = 4;

	unsigned short tempChannelNo = ADAM6024_START_INDEX_AO_VALUE;
	return MOD_ForceTcpClientMultiRegs(m_ulClientHandle, ADAM6024_DEVICE_ADDRESS
		, tempChannelNo, TOTAL_POINT, TOTAL_BYTE, byData, ModbusWriteCompletedEventHandler);
}

// -------------------------------------------------------------------------------------------------

bool CAdvantechModbus::StartThread()
{
	if (NULL == m_pMeasureThread) {
		m_pMeasureThread = AfxBeginThread(ScanDeviceThread, this);
	}

	return true;
}

bool CAdvantechModbus::StopThread()
{
	m_bLoopMeasure = false;

	if (m_pMeasureThread)
	{
		::WaitForSingleObject(m_pMeasureThread->m_hThread, INFINITE);
		m_pMeasureThread = NULL;
	}

	return true;
}

UINT CAdvantechModbus::ScanDeviceThread(LPVOID pParam)
{
	CAdvantechModbus* pAdvantechModbus = (CAdvantechModbus*)pParam;
	if (!pAdvantechModbus)
	{
		return 0;
	}

	while (pAdvantechModbus->m_bLoopMeasure)
	{
		pAdvantechModbus->ScanDevice();
		Sleep(1);
	}

	return 0;
}

void CAdvantechModbus::ScanDevice()
{

}

// -------------------------------------------------------------------------------------------------

long CAdvantechModbus::SetPollingHoldRegAIValue()
{
	return MOD_AddTcpClientReadTag(m_ulClientHandle, ADAM6024_DEVICE_ADDRESS, MODBUS_READHOLDREG
									, ADAM6024_START_INDEX_AI_VALUE, ADAM6024_AI_CHANNEL_MAX_COUNT, ReadHoldRegAIValueHandler);
}

long CAdvantechModbus::SetPollingHoldRegAIStatus()
{
	return MOD_AddTcpClientReadTag(m_ulClientHandle, ADAM6024_DEVICE_ADDRESS, MODBUS_READHOLDREG
									, ADAM6024_START_INDEX_AI_STATUS, ADAM6024_AI_CHANNEL_MAX_COUNT, ReadHoldRegAIStatusHandler);
}

long CAdvantechModbus::SetPollingHoldRegAITypeCode()
{
	return MOD_AddTcpClientReadTag(m_ulClientHandle, ADAM6024_DEVICE_ADDRESS, MODBUS_READHOLDREG
									, ADAM6024_START_INDEX_AI_TYPE_CODE, ADAM6024_AI_CHANNEL_MAX_COUNT, ReadHoldRegAITypeCodeHandler);
}

long CAdvantechModbus::SetPollingHoldRegAOValue()
{
	return MOD_AddTcpClientReadTag(m_ulClientHandle, ADAM6024_DEVICE_ADDRESS, MODBUS_READHOLDREG
									, ADAM6024_START_INDEX_AO_VALUE, ADAM6024_AO_CHANNEL_MAX_COUNT, ReadHoldRegAOValueHandler);
}

long CAdvantechModbus::SetPollingHoldRegAOTypeCode()
{
	return MOD_AddTcpClientReadTag(m_ulClientHandle, ADAM6024_DEVICE_ADDRESS, MODBUS_READHOLDREG
									, ADAM6024_START_INDEX_AO_TYPE_CODE, ADAM6024_AO_CHANNEL_MAX_COUNT, ReadHoldRegAOTypeCodeHandler);
}

long CAdvantechModbus::SetPollingHoldRegAOStartupValue()
{
	return MOD_AddTcpClientReadTag(m_ulClientHandle, ADAM6024_DEVICE_ADDRESS, MODBUS_READHOLDREG
									, ADAM6024_START_INDEX_AO_STARTUP_VALUE, ADAM6024_AO_CHANNEL_MAX_COUNT, ReadHoldRegAOStartupValueHandler);
}
