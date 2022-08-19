#pragma once

#include "AlarmList.h"
#include <map>

// Alarm Type(4EA) ���� �� Alarm Bit( 8EA, 12EA) �� ����
// �� Class �ϳ��� 1Channel ���� ������ �ȴٰ� ���� ��
class CAlarmMap
{
public:
	CAlarmMap();
	virtual ~CAlarmMap();

	void	LoadAlarm();

	BOOL	ExistAlarm();

	std::map< int, CAlarmList* >&	GetAlarmMap();

	CAlarmList*	GetAlarmList( int nAlarmType );
private:
	// KEY = Alarm Type, Data = Type �� Bit �� Alarm ����
	std::map< int, CAlarmList* > m_mapAlarm;

	CString m_strFilePath;
private:
	void	Add(CString strType, CAlarmList* pNewElement);
	void	RemoveAll();
};