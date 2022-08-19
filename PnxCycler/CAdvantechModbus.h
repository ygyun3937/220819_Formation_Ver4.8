#pragma once

#include "./include/ADSMOD.h"

class CAdvantechModbus
{
public:
	CAdvantechModbus();
	virtual ~CAdvantechModbus();

private:
	bool	Initialize();
	void	Dispose();

	bool	InitializeVariables();

public:
	int		OpenModbus(CString strIpAddRess, int nPort, int deviceIndexNo);
	int		CloseModbus();

	int		ConnectServer();
	int		DisconnectServer();

public:
	void	SetDeviceIndexNo(int value) { m_nDeviceIndex = value; }
	int		GetDeviceIndexNo() { return m_nDeviceIndex; }

	long 	GetAIValue(int nChannelNo);
	long	GetAIStatus(int nChannelNo);
	long	GetAITypeCode(int nChannelNo);

	long	GetAOValue(int nChannelNo);
	long	GetAOTypeCode(int nChannelNo);
	long	GetAOStartupValue(int nChannelNo);

	long	SetForceHoldRegAITypeCode(int nChannelNo, int nTypeCode);

	long    SetForceHoldRegAOValue(int nChannelNo, long nSetValue);
	long	SetForceHoldRegAOTypeCode(int nChannelNo, int nTypeCode);
	long	SetForceHoldRegAOStartupValue(int nChannelNo, long nSetValue);

	long	SetForceHoldRegAOValueAll(int nSetValue);

private:
	bool	StartThread();
	bool	StopThread();

	static UINT	ScanDeviceThread(LPVOID pParam);
	void		ScanDevice();

	// ----------------------------------------------------------------------------

	long	SetPollingHoldRegAIValue();
	long	SetPollingHoldRegAIStatus();
	long	SetPollingHoldRegAITypeCode();

	long	SetPollingHoldRegAOValue();
	long	SetPollingHoldRegAOTypeCode();
	long	SetPollingHoldRegAOStartupValue();

private:
	CWinThread*		m_pMeasureThread;
	bool			m_bLoopMeasure;

	int				m_nDeviceIndex;
	CString			m_strIpAddress;
	int				m_nPortNo;

	CStringArray	m_strChannelArray;

	unsigned long	m_ulClientHandle;
};



