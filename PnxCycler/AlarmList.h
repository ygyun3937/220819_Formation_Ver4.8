#pragma once

#include "Alarm.h"

class CAlarmList
{
public:
	CAlarmList();
	virtual ~CAlarmList();

	void	LoadAlarm(CString strType, CString strAlarmCount, CString strFileName);
	void	SetAllAlarm(BOOL bAlarm);

	CAlarm*	GetAlarm(int nIndex);

	BOOL	ExistAlarm();
	void	RemoveAll();
private:
	// Type �� Alarm Data �� ����
	CList< CAlarm*, CAlarm* > m_listAlarm;

	int		m_nAlarmType;
private:
};

