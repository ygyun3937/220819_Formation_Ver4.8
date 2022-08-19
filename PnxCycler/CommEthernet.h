#pragma once
#include "Channel.h"
struct TEthernet
{
	CString strTime;
	CString strData;

	int	nBoardNumber;
	int nSize;

	BYTE	pszComm[4096] = { 0, };
};

class CCommEthernet
{
public:
	CCommEthernet();
	virtual ~CCommEthernet();

	void	StartThread();
	void	StopThread();

	void	AddQueue(TEthernet* pNewElement);

	int		GetBoardIndex();
	void	SetBoardIndex(int nIndex);

	int		GetCommType();
	void	SetCommType(int nCommType);

	float	GetBeforCur();
	void	SetBeforCur(float fBeforCur);

	bool	IsQueueRun() {		return m_bLoopQueue;	}
private:
	CList< TEthernet*, TEthernet* > m_Queue;

	CWinThread*	m_pQueuingThread;
	CWinThread*	m_pCheckTimeThread;

	CCriticalSection	m_csPush;
	CCriticalSection	m_csPop;

	BOOL	m_bLoopQueue;
	BOOL	m_bTimeCheck;
	BOOL	m_bFirstReceive;

	int		m_nUIChangeBefore;

	int		m_nBoardIndex;
	int		m_nCommType;

	int		m_nAbnormalCount;
	int		m_nAbnormalCheckTime;

	int		m_nDmmCount;

	float	m_fBeforCur;
	int		m_nOldEndReason;
	ULONGLONG		m_lBoardTickCount[4]{ 0, };

private:
	TEthernet*	GetQueue();
	
	static UINT	QueuingThread(LPVOID pParam);
	static UINT	TimeCheckThread(LPVOID pParam);

public:
	CChannel* m_pChannel;
	int		GetFrameType(const TEthernet* pComm);
	int		CalcChannelNumber(int nChannelNumber, int nBoardIndex);

	//220124 YGYUN
	void	ParseBoardDataFrame_Formation(const TEthernet* pComm);	
	void	ParseBoardDataFrame(const TEthernet* pComm);
	void	ParseBoardEndFrame(const TEthernet* pComm);
	void	ParseBoardAlarmFrame(const TEthernet* pComm); //�м� 220126
	void	ParseBoardStart(const TEthernet* pComm);
	void	ParseBoardJump(const TEthernet* pComm);
	void	ParseBoardCommand(const TEthernet* pComm);
	void	ParseBoardPause(const TEthernet* pComm);
	void	ParseBoardStatus(const TEthernet* pComm);

	void	ReceivePlcInterfaceData(const TEthernet* pComm);


	//void	CheckStepCondition(int nChannelNumber, int nStepIndex);	//naming..->CheckCondition
	void	CheckCondition(int nChannelNumber, int nStepIndex, bool bIsCycleChange);	//

	BOOL    m_bChamberSendMessage;
};

