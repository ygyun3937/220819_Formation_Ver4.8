#pragma once

#include "CStandardModbus.h"
#include "CAdvantechModbus.h"

struct TPid
{
	CString strTime;
	CString strData;

	int	nDeviceNo;
	int nSize;

};

class CCommPid : public CWnd
{
public:
	CCommPid();
	virtual ~CCommPid();

private:
	void	Initialize();
	void	InitializeVariables();

	bool	Dispose();

public:
	void	StartQueueThread();
	void	StopQueueThread();

	void	AddQueue(TPid* pNewElement);
	TPid*	GetQueue();

	// ----------------------------------------------------------------------------

	int		OpenDevice();
	int		OpenDevice(CString strIpAddress, int nPort);
	int		CloseDevice();

	int		ConnectDevice();
	int		DisconnectDevice();

	int		GetDeviceChannelNo(int sourceChannelIndex);

	// ----------------------------------------------------------------------------

	long	SetAITypeCode(int nChannelNo, int nTypeCode);

	long	SetAOValue(int nChannelNo, float fValue);
	long	SetAOTypeCode(int nChannelNo, int nTypeCode);
	long	SetAOStartupValue(int nChannelNo, float fValue);

	// ----------------------------------------------------------------------------

	long	SetAOValueAll(float fValue);

	long	SetAOValue_Select_All(float fValue);

public:
	void	SetCommType(int value) { m_nCommType = value; }
	int		GetCommType() { return m_nCommType; }

	void	SetDeviceType(CString value) { m_strDeviceType = value; }
	CString	GetDeviceType() { return m_strDeviceType; }

	void	SetDeviceIndex(int value) { m_nDeviceIndex = value; }
	int		GetDeviceIndex() { return m_nDeviceIndex; }

	void	SetPrimaryKey(int value) { m_nPrimaryKey = value; }
	int		GetPrimaryKey() { return m_nPrimaryKey; }

	void			SetChannelArray(CStringArray& value) { m_strChannelArray.Copy(value); }
	CStringArray&	GetChannelArray() { return m_strChannelArray; }

	void	SetIpAddress(CString value) { m_strIpAddress = value; }
	CString	GetIpAddress() { return m_strIpAddress; }

	void	SetPortNo(int value) { m_nPortNo = value; }
	int		GetPortNo() { return m_nPortNo; }

	// ----------------------------------------------------------------------------

	long	GetCurrentAIValue(int nChannelNo)		{ return m_AdvantechModbus->GetAIValue(nChannelNo); }
	long	GetCurrentAIStatus(int nChannelNo)		{ return m_AdvantechModbus->GetAIStatus(nChannelNo); }
	long	GetCurrentAITypeCode(int nChannelNo)	{ return m_AdvantechModbus->GetAITypeCode(nChannelNo); }
	long	GetCurrentAOValue(int nChannelNo)		{ return m_AdvantechModbus->GetAOValue(nChannelNo); }
	long	GetCurrentAOTypeCode(int nChannelNo)	{ return m_AdvantechModbus->GetAOTypeCode(nChannelNo); }

private:
	static UINT	QueuingThread(LPVOID pParam);

	// ----------------------------------------------------------------
	// STANDARD

	int		OpenStandardModbus();
	int		CloseStandardModbus();

	int		ConnectStandardModbus();
	int		DisconnectStandardModbus();

	// ----------------------------------------------------------------
	// ADVANTECH

	int		OpenAdvantechModbus();
	int		CloseAdvantechModbus();

	int		ConnectAdvantechModbus();
	int		DisconnectAdvantechModbus();

private:
	CStandardModbus*	m_pStandardModbus;		// STANDARD
	CAdvantechModbus*	m_AdvantechModbus;		// ADVANTECH

	CWinThread*			m_pQueuingThread;
	bool				m_bLoopQueue;

	CList< TPid*, TPid* >	m_Queue;
	CCriticalSection		m_csPush;
	CCriticalSection		m_csPop;
	
	// ----------------------------------------------------------------------------

	bool		m_bInitialize;
	
	int			m_nCommType;
	int			m_nPrimaryKey;
	CStringArray m_strChannelArray;
	CString		m_strDeviceType;
	int			m_nDeviceIndex;
	CString		m_strIpAddress;
	int			m_nPortNo;

	bool		m_bFirstReceive;
};

