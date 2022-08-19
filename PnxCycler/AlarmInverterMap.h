#pragma once

#include "AlarmInverterList.h"
#include <map>

// Alarm Type(4EA) ���� �� Alarm Bit( 8EA, 12EA) �� ����
// �� Class �ϳ��� 1Channel ���� ������ �ȴٰ� ���� ��
class CAlarmInverterMap
{
public:
	CAlarmInverterMap();
	virtual ~CAlarmInverterMap();

	void	LoadAlarm();

	BOOL	ExistAlarm();

	std::map< int, CAlarmInverterList* >&	GetAlarmMap();

	CAlarmInverterList*	GetAlarmList(int nAlarmType);
private:
	// KEY = Alarm Type, Data = Type �� Bit �� Alarm ����
	std::map< int, CAlarmInverterList* > m_mapAlarm;

	CString m_strFilePath;
private:
	void	Add(CString strType, CAlarmInverterList* pNewElement);
	void	RemoveAll();
};