#pragma once

// Was generated IDispatch wrapper class was created in Microsoft Visual C + +.

// Note : Please do not change the contents of this file.  
// If this class is regenerated in the Microsoft Visual C + +, the changes will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// CActprogtype  wrapper class

class CActprogtype : public CWnd
{
protected:
	DECLARE_DYNCREATE(CActprogtype)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x78703D55, 0x4042, 0x4046, { 0x84, 0x5D, 0x7B, 0xBE, 0x83, 0xFB, 0x2F, 0x44 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// Attribute
public:


// Operation
public:

// IActProgType

// Functions
//

	long get_ActUnitType()
	{
		long result;
		InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActUnitType(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActProtocolType()
	{
		long result;
		InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActProtocolType(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActNetworkNumber()
	{
		long result;
		InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActNetworkNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActStationNumber()
	{
		long result;
		InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActStationNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActUnitNumber()
	{
		long result;
		InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActUnitNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActConnectUnitNumber()
	{
		long result;
		InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActConnectUnitNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActIONumber()
	{
		long result;
		InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActIONumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActCpuType()
	{
		long result;
		InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActCpuType(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActPacketType()
	{
		long result;
		InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActPacketType(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActPortNumber()
	{
		long result;
		InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActPortNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActBaudRate()
	{
		long result;
		InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActBaudRate(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActDataBits()
	{
		long result;
		InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActDataBits(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xc, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActParity()
	{
		long result;
		InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActParity(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xd, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActStopBits()
	{
		long result;
		InvokeHelper(0xe, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActStopBits(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xe, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActControl()
	{
		long result;
		InvokeHelper(0xf, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActControl(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xf, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_ActHostAddress()
	{
		CString result;
		InvokeHelper(0x10, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_ActHostAddress(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x10, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActCpuTimeOut()
	{
		long result;
		InvokeHelper(0x11, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActCpuTimeOut(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x11, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActTimeOut()
	{
		long result;
		InvokeHelper(0x12, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActTimeOut(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x12, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActSumCheck()
	{
		long result;
		InvokeHelper(0x13, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActSumCheck(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x13, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActSourceNetworkNumber()
	{
		long result;
		InvokeHelper(0x14, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActSourceNetworkNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x14, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActSourceStationNumber()
	{
		long result;
		InvokeHelper(0x15, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActSourceStationNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x15, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActDestinationPortNumber()
	{
		long result;
		InvokeHelper(0x16, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActDestinationPortNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x16, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActDestinationIONumber()
	{
		long result;
		InvokeHelper(0x17, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActDestinationIONumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x17, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActMultiDropChannelNumber()
	{
		long result;
		InvokeHelper(0x18, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActMultiDropChannelNumber(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x18, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActThroughNetworkType()
	{
		long result;
		InvokeHelper(0x19, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActThroughNetworkType(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x19, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActIntelligentPreferenceBit()
	{
		long result;
		InvokeHelper(0x1a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActIntelligentPreferenceBit(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActDidPropertyBit()
	{
		long result;
		InvokeHelper(0x1b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActDidPropertyBit(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActDsidPropertyBit()
	{
		long result;
		InvokeHelper(0x1c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActDsidPropertyBit(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_ActPassword()
	{
		CString result;
		InvokeHelper(0x1d, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_ActPassword(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x1d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActTargetSimulator()
	{
		long result;
		InvokeHelper(0x1e, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActTargetSimulator(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActConnectWay()
	{
		long result;
		InvokeHelper(0x1f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActConnectWay(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_ActATCommand()
	{
		CString result;
		InvokeHelper(0x20, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_ActATCommand(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x20, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_ActDialNumber()
	{
		CString result;
		InvokeHelper(0x21, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_ActDialNumber(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x21, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_ActOutsideLineNumber()
	{
		CString result;
		InvokeHelper(0x22, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_ActOutsideLineNumber(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x22, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_ActCallbackNumber()
	{
		CString result;
		InvokeHelper(0x23, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_ActCallbackNumber(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x23, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActLineType()
	{
		long result;
		InvokeHelper(0x24, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActLineType(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x24, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActConnectionCDWaitTime()
	{
		long result;
		InvokeHelper(0x25, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActConnectionCDWaitTime(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x25, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActConnectionModemReportWaitTime()
	{
		long result;
		InvokeHelper(0x26, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActConnectionModemReportWaitTime(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x26, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActDisconnectionCDWaitTime()
	{
		long result;
		InvokeHelper(0x27, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActDisconnectionCDWaitTime(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x27, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActDisconnectionDelayTime()
	{
		long result;
		InvokeHelper(0x28, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActDisconnectionDelayTime(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x28, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActTransmissionDelayTime()
	{
		long result;
		InvokeHelper(0x29, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActTransmissionDelayTime(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x29, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActATCommandResponseWaitTime()
	{
		long result;
		InvokeHelper(0x2a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActATCommandResponseWaitTime(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x2a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActPasswordCancelResponseWaitTime()
	{
		long result;
		InvokeHelper(0x2b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActPasswordCancelResponseWaitTime(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x2b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActATCommandPasswordCancelRetryTimes()
	{
		long result;
		InvokeHelper(0x2c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActATCommandPasswordCancelRetryTimes(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x2c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActCallbackCancelWaitTime()
	{
		long result;
		InvokeHelper(0x2d, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActCallbackCancelWaitTime(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x2d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActCallbackDelayTime()
	{
		long result;
		InvokeHelper(0x2e, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActCallbackDelayTime(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x2e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_ActCallbackReceptionWaitingTimeOut()
	{
		long result;
		InvokeHelper(0x2f, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_ActCallbackReceptionWaitingTimeOut(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x2f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long Open()
	{
		long result;
		InvokeHelper(0x30, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long Close()
	{
		long result;
		InvokeHelper(0x31, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetCpuType(BSTR * szCpuName, long * lplCpuCode)
	{
		long result;
		static BYTE parms[] = VTS_PBSTR VTS_PI4 ;
		InvokeHelper(0x32, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szCpuName, lplCpuCode);
		return result;
	}
	long SetCpuStatus(long lOperation)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x33, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lOperation);
		return result;
	}
	long ReadDeviceBlock(LPCTSTR szDevice, long lSize, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI4 ;
		InvokeHelper(0x34, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lSize, lplData);
		return result;
	}
	long WriteDeviceBlock(LPCTSTR szDevice, long lSize, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI4 ;
		InvokeHelper(0x35, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lSize, lplData);
		return result;
	}
	long ReadDeviceRandom(LPCTSTR szDeviceList, long lSize, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI4 ;
		InvokeHelper(0x36, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDeviceList, lSize, lplData);
		return result;
	}
	long WriteDeviceRandom(LPCTSTR szDeviceList, long lSize, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI4 ;
		InvokeHelper(0x37, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDeviceList, lSize, lplData);
		return result;
	}
	long ReadBuffer(long lStartIO, long lAddress, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_PI2 ;
		InvokeHelper(0x38, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lStartIO, lAddress, lSize, lpsData);
		return result;
	}
	long WriteBuffer(long lStartIO, long lAddress, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_PI2 ;
		InvokeHelper(0x39, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lStartIO, lAddress, lSize, lpsData);
		return result;
	}
	long GetClockData(short * lpsYear, short * lpsMonth, short * lpsDay, short * lpsDayOfWeek, short * lpsHour, short * lpsMinute, short * lpsSecond)
	{
		long result;
		static BYTE parms[] = VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2 VTS_PI2 ;
		InvokeHelper(0x3a, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lpsYear, lpsMonth, lpsDay, lpsDayOfWeek, lpsHour, lpsMinute, lpsSecond);
		return result;
	}
	long SetClockData(short sYear, short sMonth, short sDay, short sDayOfWeek, short sHour, short sMinute, short sSecond)
	{
		long result;
		static BYTE parms[] = VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 VTS_I2 ;
		InvokeHelper(0x3b, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sYear, sMonth, sDay, sDayOfWeek, sHour, sMinute, sSecond);
		return result;
	}
	long SetDevice(LPCTSTR szDevice, long lData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0x3c, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lData);
		return result;
	}
	long GetDevice(LPCTSTR szDevice, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_PI4 ;
		InvokeHelper(0x3d, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lplData);
		return result;
	}
	long CheckDeviceString(LPCTSTR szDevice, long lCheckType, long lSize, long * lplDeviceType, BSTR * lpszDeviceName, long * lplDeviceNumber, long * lplDeviceRadix)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_PI4 VTS_PBSTR VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x3e, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lCheckType, lSize, lplDeviceType, lpszDeviceName, lplDeviceNumber, lplDeviceRadix);
		return result;
	}
	long EntryDeviceStatus(LPCTSTR szDeviceList, long lSize, long lMonitorCycle, long * lplData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_PI4 ;
		InvokeHelper(0x3f, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDeviceList, lSize, lMonitorCycle, lplData);
		return result;
	}
	long FreeDeviceStatus()
	{
		long result;
		InvokeHelper(0x40, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long ReadDeviceBlock2(LPCTSTR szDevice, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI2 ;
		InvokeHelper(0x41, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lSize, lpsData);
		return result;
	}
	long WriteDeviceBlock2(LPCTSTR szDevice, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI2 ;
		InvokeHelper(0x42, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lSize, lpsData);
		return result;
	}
	long ReadDeviceRandom2(LPCTSTR szDeviceList, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI2 ;
		InvokeHelper(0x43, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDeviceList, lSize, lpsData);
		return result;
	}
	long WriteDeviceRandom2(LPCTSTR szDeviceList, long lSize, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PI2 ;
		InvokeHelper(0x44, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDeviceList, lSize, lpsData);
		return result;
	}
	long GetDevice2(LPCTSTR szDevice, short * lpsData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_PI2 ;
		InvokeHelper(0x45, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, lpsData);
		return result;
	}
	long SetDevice2(LPCTSTR szDevice, short sData)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I2 ;
		InvokeHelper(0x46, DISPATCH_METHOD, VT_I4, (void*)&result, parms, szDevice, sData);
		return result;
	}
	long Connect()
	{
		long result;
		InvokeHelper(0x47, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long Disconnect()
	{
		long result;
		InvokeHelper(0x48, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetCpuListOnEther(long lTimeOut, long lCpuListSize, long * lplCpuList, long * lplCpuListCount)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x49, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lTimeOut, lCpuListSize, lplCpuList, lplCpuListCount);
		return result;
	}

// Properties
//



};
