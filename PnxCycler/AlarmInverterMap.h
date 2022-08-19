#pragma once

#include "AlarmInverterList.h"
#include <map>

// Alarm Type(4EA) 별로 각 Alarm Bit( 8EA, 12EA) 를 관리
// 즉 Class 하나는 1Channel 별로 생성이 된다고 보면 됨
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
	// KEY = Alarm Type, Data = Type 별 Bit 당 Alarm 정보
	std::map< int, CAlarmInverterList* > m_mapAlarm;

	CString m_strFilePath;
private:
	void	Add(CString strType, CAlarmInverterList* pNewElement);
	void	RemoveAll();
};