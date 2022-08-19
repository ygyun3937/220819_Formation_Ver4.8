#include "stdafx.h"
#include "CMgrScheduleHistory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMgrScheduleHistory::CMgrScheduleHistory()
{
	for (int i = 0; i < eScheduleHistoryTypeMaxCount; ++i)
	{
		LoadScheduleHistory((eScheduleHistoryType)i);
	}
}

CMgrScheduleHistory::~CMgrScheduleHistory()
{
	auto iter	= m_mapSchedule.begin();
	auto iterE	= m_mapSchedule.end();

	while (iter != iterE)
	{
		StringList* listValue = iter->second;

		if (listValue)
		{
			delete listValue;

			listValue = nullptr;
		}

		++iter;
	}

	m_mapSchedule.clear();
}

void	CMgrScheduleHistory::SetNewItem(eScheduleHistoryType nType, CString strValue)
{
	if (TRUE == strValue.IsEmpty())
		return;

	auto it = m_mapSchedule.find(nType);

	if (it == m_mapSchedule.end())
		return;

	StringList* listValue = it->second;

	// 첫번째 중복된 정보가 있는지 확인 해야 한다.
	auto iter	= listValue->begin();
	auto iterE	= listValue->end();

	while (iter != iterE)
	{
		CString strPrevValue = (*iter);

		if (strcmp(strPrevValue, strValue) == 0)
			return;

		++iter;
	}

	// 두번째 추가했을 때 개수가 최대치인지 확인해야 한다.
	if (listValue->size() >= HistoryMaxCount)
	{
		listValue->pop_back();
	}

	listValue->push_front(strValue);

	SaveScheduleHistory(nType);
}

StringList*	CMgrScheduleHistory::GetItem(eScheduleHistoryType nType)
{
	auto it = m_mapSchedule.find(nType);

	if (it == m_mapSchedule.end())
		return nullptr;

	return it->second;
}

void	CMgrScheduleHistory::SaveScheduleHistory(eScheduleHistoryType nType)
{
	auto it = m_mapSchedule.find(nType);

	if (it == m_mapSchedule.end())
		return;

	CString strFilePath;
	strFilePath.Format("%s\\UserHistory\\%s.ini", (LPCSTR)GetExePath(), (LPCSTR)ScheduleHistory2String(nType));

	DeleteFile(strFilePath);

	StringList* listValue = it->second;

	INIWriteInt("Base", "Count", listValue->size(), strFilePath);

	int nIndex = 0;

	auto iter = listValue->begin();
	auto iterE = listValue->end();

	while (iter != iterE)
	{
		CString strAppName;
		strAppName.Format("History%d", nIndex++);

		CString strValue = (*iter);

		INIWriteStr(strAppName, "Value", strValue, strFilePath);

		++iter;
	}
}

void	CMgrScheduleHistory::LoadScheduleHistory(eScheduleHistoryType nType)
{
	auto it = m_mapSchedule.find(nType);

	if (it != m_mapSchedule.end())
		return;

	StringList* listValue = new StringList;

	m_mapSchedule.insert(std::make_pair(nType, listValue));

	CString strFilePath;
	strFilePath.Format("%s\\UserHistory\\%s.ini", (LPCSTR)GetExePath(), (LPCSTR)ScheduleHistory2String(nType));

	int nContentSize = INIReadInt("Base", "Count", strFilePath);

	int nIndex = 0;

	for (int i = 0; i < nContentSize; ++i)
	{
		CString strAppName;
		strAppName.Format("History%d", nIndex++);

		CString strValue = INIReadStr(strAppName, "Value", strFilePath);

		if (FALSE == strValue.IsEmpty())
			listValue->push_back(strValue);
	}
}

CString	CMgrScheduleHistory::ScheduleHistory2String(eScheduleHistoryType nType)
{
	switch (nType)
	{
	case eScheduleHistoryTypeUserName		: return "HistoryUserName";
	case eScheduleHistoryTypeSampleName		: return "HistorySampleName";
	case eScheduleHistoryTypeEquipName		: return "HistoryEquipName";
	case eScheduleHistoryTypeScheduleName	: return "HistoryScheduleName";
	case eScheduleHistoryTypeSchedulePath	: return "HistorySchedulePath";
	case eScheduleHistoryTypeLogFilePath	: return "HistoryLogFilePath";
	case eScheduleHistoryTypeDescription	: return "HistoryDescription";
	}

	return "";
}