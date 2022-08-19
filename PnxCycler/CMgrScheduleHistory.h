#pragma once

enum eScheduleHistoryType
{
	eScheduleHistoryTypeUserName = 0,
	eScheduleHistoryTypeSampleName,
	eScheduleHistoryTypeEquipName,
	eScheduleHistoryTypeScheduleName,
	eScheduleHistoryTypeSchedulePath,
	eScheduleHistoryTypeLogFilePath,
	eScheduleHistoryTypeDescription,
	eScheduleHistoryTypeMaxCount
};

#include "Singleton.h"
#include <list>
#include <map>

typedef std::list<CString> StringList;

#define HistoryMaxCount	(10)

class CMgrScheduleHistory : public CSingleton< CMgrScheduleHistory >
{
public:
	CMgrScheduleHistory();
	virtual ~CMgrScheduleHistory();

	void	SetNewItem(eScheduleHistoryType nType, CString strValue);

	StringList*	GetItem(eScheduleHistoryType nType);
private:
	std::map<int, StringList*> m_mapSchedule;
private:
	void	SaveScheduleHistory(eScheduleHistoryType nType);
	void	LoadScheduleHistory(eScheduleHistoryType nType);

	CString	ScheduleHistory2String(eScheduleHistoryType nType);
};

