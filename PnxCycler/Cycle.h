#pragma once

#include "Step.h"

enum eCycleEndCondtion
{
	eCycleEndCondtionVoltage = 0,
	eCycleEndCondtionLowVoltage,
	eCycleEndCondtionTime,
	eCycleEndCondtionAmpere,
	eCycleEndCondtionCapacity,
	eCycleEndCondtionWatt,
	eCycleEndCondtionMaxCount
};

class CCycle
{
public:
	CCycle();
	virtual ~CCycle();

	// 변수 접근 함수
	void	SetTotalCycleLoop(int nTotalCycleLoop);
	int		GetTotalCycleLoop();

	void	SetCycleNumber( int nCycleNumber );
	int		GetCycleNumber();

	void	SetLoopCount( int nLoopCount );
	int		GetLoopCount();

	void	SetGotoCycle( int nGotoCycle );
	int		GetGotoCycle();

	void	SetGotoLoopCount( int nGotoLoopCount );
	int		GetGotoLoopCount();

	void	SetGotoNextCycle( int nGotoNextCycle );
	int		GetGotoNextCycle();

	void	SetCycleEndTime( int nCycleEndTime );
	int		GetCycleEndTime();

	int		GetCycleEndIndex(int nType);
	void	SetCycleEndIndex(int nType, int nIndex);

	void	SetFinishCondition(LPCTSTR lpszString);

	void	SetSafetyCondition( LPCTSTR lpszString );
	void	SetLogRecodeTerm();

	void	SetCycleLoopStartNo(int nStartNo) { m_nCycleLoopStart = nStartNo; }
	int		GetCycleLoopStartNo() { return m_nCycleLoopStart; }

	void	Condition2Array(__out CStringArray& arr);
	// List 접근 함수
	void	Add( CStep* pNewElement );
	void	RemoveAll();

	int		GetSize();

	CStep*	GetHeadData();
	CStep*	GetTailData();

	CStep*	GetAtStep(int nStepNumber);

	BOOL	IsExist(int nStepNumber);

	CList< CStep*, CStep* >&	GetStepList();

	SScheduleComplete*	GetScheduleCompleteFromName(CString strName);
	SScheduleSafety*	GetScheduleSafetyFromName(CString strName);
	SScheduleLog*		GetScheduleLogFromName(CString strName);

	std::vector<SScheduleComplete*>&	GetScheduleComplete() { return m_vecScheduleComplete; }
	std::vector<SScheduleSafety*>&		GetScheduleSafety() { return m_vecScheduleSafety; }
	std::vector<SScheduleLog*>&			GetScheduleLog() { return m_vecScheduleLog; }
private:
	int		m_nCycleNumber;
	int		m_nLoopCount;
	int		m_nGotoCycle;
	int		m_nGotoLoopCount;
	int		m_nGotoNextCycle;
	int		m_nCycleEndTime;
	int		m_nTotalCycleLoop;
	int		m_nCycleEndIndex[eCycleEndCondtionMaxCount];
	int		m_nCycleLoopStart;

	std::vector<SScheduleSafety*>	m_vecScheduleSafety;
	std::vector<SScheduleComplete*>	m_vecScheduleComplete;
	std::vector<SScheduleLog*>		m_vecScheduleLog;

	CList< CStep*, CStep* > m_listStep;
private:
	void	RemoveScheduleComplete();
	void	RemoveScheduleSafety();
	void	RemoveScheduleLog();
};

