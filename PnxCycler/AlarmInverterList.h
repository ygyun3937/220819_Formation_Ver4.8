#pragma once

#include "AlarmInverter.h"

class CAlarmInverterList
{
public:
	CAlarmInverterList();
	virtual ~CAlarmInverterList();

	void	LoadAlarmInverter(CString strType, CString strAlarmCount, CString strFileName);
	void	SetAllAlarmInverter(BOOL bAlarm);

	CAlarmInverter*	GetAlarm(int nIndex);

	BOOL	ExistAlarm();
private:
	// Type 별 Alarm Data 를 관리
	CList< CAlarmInverter*, CAlarmInverter* > m_listAlarm;

	int		m_nAlarmType;
private:
	void	RemoveAll();
};

