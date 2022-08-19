#pragma once

#include "CommData.h"

#define DB_WAIT_COUNT 5

class CLogThread
{
public:
	CLogThread();
	virtual ~CLogThread();

	void	StartThread();
	void	StopThread();

	int		GetChannelNumber();
	void	SetChannelNumber(int nNumber);

	CCommData*	GetQueue();
	void		AddQueue(CCommData* pQueue);

public:
	void	LowCalibration(int nCh, int nStep, CString strFilePath);
	void	StartNextChannelCalibration();

private:
	enum eResultFileType
	{
		eResultFileTypeStepRun = 0,
		eResultFileTypeStepTotal,
		eResultFileTypeEndRun,
		eResultFileTypeEndTotal,
		eResultFileTypeCalibrationAverage,
		eResultFileTypeMaxCount
	};

private:
	CCriticalSection	m_csLock;
	CWinThread*			m_pThread;

	bool	m_bLoop;
	int		m_nChannelNumber;

	CList< CCommData*, CCommData* > m_Queue;

	CStdioFile	m_file[eResultFileTypeMaxCount];
	int	m_nFileCount[eResultFileTypeMaxCount];

	// AUTO_CALIBRATION
	int		m_nTotalEnd = 0;
	CString m_strEndReason;
	bool	m_bNextCh = false;

private:
	static UINT QueuingThread(LPVOID pParam);

	void	WriteRun(CCommData* lpCycleChannelInfo);
	void	WriteEnd(CCommData* lpCycleChannelInfo);
	void	WriteCalVolt(CCommData* lpCycleChannelInfo);




	bool	WriteOpenedFile(CString strFilePath, CString strCyclerChannelInfo, int nFileType, int nChannelNumber, CCommData* lpCycleChannelInfo);
};

