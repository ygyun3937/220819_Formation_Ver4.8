#pragma once
struct stPlcData
{
	CString strTime;
	CString strData;
		
	int	nDeviceNo;
	int nSize;

};
class CPlcInterface
{
public:
	CPlcInterface();
	virtual ~CPlcInterface();

private:
	void	Initialize();
	void	InitializeVariables();

	bool	Dispose();

public:
	void	AddQueue(stPlcData* pNewElement);
	stPlcData*	GetQueue();


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


	

private:
	static UINT	QueuingThread(LPVOID pParam);

	CList< stPlcData*, stPlcData* >	m_Queue;
	CCriticalSection		m_csPush;
	CCriticalSection		m_csPop;

private:
	static	UINT ReadThread(LPVOID pParam);
	bool				m_bLoopQueue;


	// ----------------------------------------------------------------------------

	bool		m_bInitialize;

	int			m_nCommType;
	int			m_nPrimaryKey;
	CStringArray m_strChannelArray;
	CString		m_strDeviceType;
	int			m_nDeviceIndex;
	CString		m_strIpAddress;
	int			m_nPortNo;


};

