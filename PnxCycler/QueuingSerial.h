#pragma once

struct TSerial
{
	CString strTime;

	int	nPrimaryKey;

	int nDataLenth;
	int nElecType;
	int nChamberDataType;

	float fValue;

	BYTE	pszComm[1024];
};

class CQueuingSerial
{
public:
	CQueuingSerial();
	virtual ~CQueuingSerial();

	void	StartThread();
	void	StopThread();

	void	SetPrimaryKey(int nPrimaryKey);
	void	AddQueue(TSerial* pNewElement);

	int		GetCommType();
	void	SetCommType(int nCommType);

	void	AddChannelArray(CString str);
	CStringArray&	GetChannelArray();
private:
	CList< TSerial*, TSerial* > m_Queue;

	CStringArray	m_arrChannel;	// 1���� ��� Type �� �������� Channel ������ �ʿ��� �� ���

	CWinThread*	m_pQueuingThread;

	CCriticalSection	m_csPush;
	CCriticalSection	m_csPop;

	BOOL	m_bLoopQueue;

	int		m_nPrimaryKey;
	int		m_nCommType;	// DSP, Chamber, Chiller ����
private:
	void	ParseChamber(const TSerial* pComm);
	void	ParseDAQ(const TSerial* pComm);
	float	CalcThermistor2Temp(double dData);

	TSerial*	GetQueue();

	static UINT	QueuingThread(LPVOID pParam);
};
