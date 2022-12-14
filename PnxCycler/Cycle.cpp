#include "stdafx.h"
#include "Cycle.h"
#include "MgrConfig.h"
#include "PnxUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCycle::CCycle()
{
	m_nCycleNumber		= 0;
	m_nLoopCount		= 0;
	m_nGotoCycle		= 0;
	m_nGotoLoopCount	= 0;
	m_nGotoNextCycle	= 0;
	m_nCycleEndTime		= 0;
	m_nTotalCycleLoop	= 0;
	m_nCycleLoopStart   = 0;
}


CCycle::~CCycle()
{
	RemoveAll();

	RemoveScheduleComplete();

	RemoveScheduleSafety();

	RemoveScheduleLog();
}

void CCycle::SetTotalCycleLoop(int nTotalCycleLoop)
{
	m_nTotalCycleLoop = nTotalCycleLoop;
}

int	CCycle::GetTotalCycleLoop()
{
	return m_nTotalCycleLoop;
}

int		CCycle::GetCycleEndIndex(int nType)
{
	if (nType >= eCycleEndCondtionVoltage && nType < eCycleEndCondtionMaxCount)
		return m_nCycleEndIndex[nType];

	return 0;
}

void	CCycle::SetCycleEndIndex(int nType, int nIndex)
{
	if (nType >= eCycleEndCondtionVoltage && nType < eCycleEndCondtionMaxCount)
		m_nCycleEndIndex[nType] = nIndex;
}

void	CCycle::SetCycleNumber( int nCycleNumber )
{
	/*
	if ( nCycleNumber > 0 )
		nCycleNumber -= 1;
	else
		nCycleNumber = 0;
	*/

	m_nCycleNumber = nCycleNumber;
}

int		CCycle::GetCycleNumber()
{
	return m_nCycleNumber;
}

void	CCycle::SetLoopCount( int nLoopCount )
{
	//에디터 수정되면 바꿔야함
	if (nLoopCount == 0)
		nLoopCount = 1;
	//
	m_nLoopCount = nLoopCount;

	int nCycleNo = GetCycleNumber()-1;
	g_nScheduleCycleLoop[nCycleNo] = m_nLoopCount;
}

int		CCycle::GetLoopCount()
{
	return m_nLoopCount;
}

void	CCycle::SetGotoCycle( int nGotoCycle )
{
	m_nGotoCycle = nGotoCycle;
}

int		CCycle::GetGotoCycle()
{
	return m_nGotoCycle;
}

void	CCycle::SetGotoLoopCount( int nGotoLoopCount )
{
	m_nGotoLoopCount = nGotoLoopCount;
}

int		CCycle::GetGotoLoopCount()
{
	return m_nGotoLoopCount;
}

void	CCycle::SetGotoNextCycle( int nGotoNextCycle )
{
	m_nGotoNextCycle = nGotoNextCycle;
}

int		CCycle::GetGotoNextCycle()
{
	return m_nGotoNextCycle;
}

void	CCycle::SetCycleEndTime(int nCycleEndTime)
{
	m_nCycleEndTime = nCycleEndTime;
}

int		CCycle::GetCycleEndTime()
{
	return m_nCycleEndTime;
}

void	CCycle::SetFinishCondition(LPCTSTR lpszString)
{
	RemoveScheduleComplete();

	CStringArray arrCondition;
	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	for (int i = 0; i < arrCondition.GetSize(); ++i)
	{
		CStringArray arr;
		CPnxUtil::GetMgr()->SplitString(arrCondition.GetAt(i), '=', arr);

		SScheduleComplete* lpScheduleComplete = new SScheduleComplete;
		lpScheduleComplete->ParseComplete(arr);

		m_vecScheduleComplete.push_back(lpScheduleComplete);
	}
}

void	CCycle::SetSafetyCondition( LPCTSTR lpszString )
{
	RemoveScheduleSafety();

	CStringArray arrCondition;
	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	for (int i = 0; i < arrCondition.GetSize(); ++i)
	{
		CStringArray arr;
		CPnxUtil::GetMgr()->SplitString(arrCondition.GetAt(i), '=', arr);

		SScheduleSafety* lpScheduleSafety = new SScheduleSafety;
		lpScheduleSafety->Parse(arr);

		m_vecScheduleSafety.push_back(lpScheduleSafety);
	}
}

void	CCycle::SetLogRecodeTerm()
{
	RemoveScheduleLog();
}

void	CCycle::Add( CStep* pNewElement )
{
	m_listStep.AddTail( pNewElement );
}

int		CCycle::GetSize()
{
	return m_listStep.GetSize();
}

CStep*	CCycle::GetHeadData()
{
	return m_listStep.GetHead();
}

CStep*	CCycle::GetTailData()
{
	return m_listStep.GetTail();
}

BOOL	CCycle::IsExist(int nStepNumber)
{
	auto Pos = m_listStep.GetHeadPosition();

	while (Pos)
	{
		CStep* pStep = m_listStep.GetNext(Pos);

		if (NULL == pStep)
			continue;

		if (pStep->GetStepNumber() == nStepNumber)
			return TRUE;
	}

	return FALSE;
}

CStep*	CCycle::GetAtStep(int nStepNumber)
{
	auto Pos = m_listStep.GetHeadPosition();

	while (Pos)
	{
		CStep* pStep = m_listStep.GetNext(Pos);

		if (NULL == pStep)
			continue;

		if (pStep->GetStepNumber() == nStepNumber)
			return pStep;
	}

	return NULL;
}

void	CCycle::RemoveAll()
{
	while (!m_listStep.IsEmpty())
		delete m_listStep.RemoveHead();
}

void	CCycle::RemoveScheduleComplete()
{
	auto iter = m_vecScheduleComplete.begin();
	auto iterE = m_vecScheduleComplete.end();

	while (iter != iterE)
	{
		SScheduleComplete* lpValue = (*iter);

		if (lpValue)
		{
			delete lpValue;
			lpValue = nullptr;
		}

		++iter;
	}

	m_vecScheduleComplete.clear();
}

void	CCycle::RemoveScheduleSafety()
{
	auto iter = m_vecScheduleSafety.begin();
	auto iterE = m_vecScheduleSafety.end();

	while (iter != iterE)
	{
		SScheduleSafety* lpValue = (*iter);

		if (lpValue)
		{
			delete lpValue;
			lpValue = nullptr;
		}

		++iter;
	}

	m_vecScheduleSafety.clear();
}

void	CCycle::RemoveScheduleLog()
{
	auto iter = m_vecScheduleLog.begin();
	auto iterE = m_vecScheduleLog.end();

	while (iter != iterE)
	{
		SScheduleLog* lpValue = (*iter);

		if (lpValue)
		{
			delete lpValue;
			lpValue = nullptr;
		}

		++iter;
	}

	m_vecScheduleLog.clear();
}

CList< CStep*, CStep* >&	CCycle::GetStepList()
{
	return m_listStep;
}

SScheduleComplete*	CCycle::GetScheduleCompleteFromName(CString strName)
{
	auto iter = m_vecScheduleComplete.begin();
	auto iterE = m_vecScheduleComplete.end();

	while (iter != iterE)
	{
		SScheduleComplete* lpScheduleComplete = (*iter);

		if (lpScheduleComplete)
		{
			if (strcmp(lpScheduleComplete->strName, strName) == 0)
				return lpScheduleComplete;
		}

		++iter;
	}

	return nullptr;
}
SScheduleSafety*	CCycle::GetScheduleSafetyFromName(CString strName)
{
	auto iter = m_vecScheduleSafety.begin();
	auto iterE = m_vecScheduleSafety.end();

	while (iter != iterE)
	{
		SScheduleSafety* lpScheduleSafety = (*iter);

		if (lpScheduleSafety)
		{
			if (strcmp(lpScheduleSafety->strName, strName) == 0)
				return lpScheduleSafety;
		}

		++iter;
	}

	return nullptr;
}
SScheduleLog*	CCycle::GetScheduleLogFromName(CString strName)
{
	auto iter = m_vecScheduleLog.begin();
	auto iterE = m_vecScheduleLog.end();

	while (iter != iterE)
	{
		SScheduleLog* lpScheduleLog = (*iter);

		if (lpScheduleLog)
		{
			if (strcmp(lpScheduleLog->strName, strName) == 0)
				return lpScheduleLog;
		}

		++iter;
	}

	return nullptr;
}

void	CCycle::Condition2Array(__out CStringArray& arr)
{
	// 해당 GUI 에서 표현 해야 하는 Data 는 Cycle 정보 + Step 정보이다.

	CMgrConfig* pMgr = CMgrConfig::GetMgr();
	CString str;

	if (pMgr)
	{
#ifdef _LANG_KOR
		str.Format(_T("System Safety Max"));
#endif
#ifdef _LANG_ENG
		str.Format(_T("System safety upper limit"));
#endif
		arr.Add(str);
		str.Empty();

		str.Format(_T("%.3f"), pMgr->GetSystemSafetyMax(eSyatemSafetyObjectVoltage));
		arr.Add(str);
		str.Empty();

		str.Format(_T("END"));
		arr.Add(str);
		str.Empty();

#ifdef _LANG_KOR
		str.Format(_T("System Safety Min"));
#endif
#ifdef _LANG_ENG
		str.Format(_T("Lower system safety limit"));
#endif
		arr.Add(str);
		str.Empty();

		str.Format(_T("%.3f"), pMgr->GetSystemSafetyMin(eSyatemSafetyObjectVoltage));
		arr.Add(str);
		str.Empty();

		str.Format(_T("END"));
		arr.Add(str);
		str.Empty();
	}

	auto iter = m_vecScheduleSafety.begin();
	auto iterE = m_vecScheduleSafety.end();

	while (iter != iterE)
	{
		SScheduleSafety* lpValue = (*iter);

		if (lpValue)
		{
			if (lpValue->fValue != 0)
			{
				arr.Add(lpValue->strName);
				str.Format(_T("%.3f"), lpValue->fValue);
				arr.Add(str);
				arr.Add(_T("END"));
			}
		}
		++iter;
	}
}