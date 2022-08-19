#include "stdafx.h"
#include "AlarmInverterMap.h"

#define	AlarmListFileName	(_T("MgrInverterAlarm.ini"))

CAlarmInverterMap::CAlarmInverterMap()
{
}

CAlarmInverterMap::~CAlarmInverterMap()
{
	RemoveAll();

}
void	CAlarmInverterMap::LoadAlarm()
{
	RemoveAll();

	CString strFilePath = _T("");

	strFilePath.Format(_T("%s\\Config\\%s"), (LPCSTR)GetExePath(), AlarmListFileName);

	int nAlarmCount = INIReadInt(_T("Default"), _T("AlarmTypeCount"), strFilePath);

	//Alarm ¸ÊÇÎ (IFº¸µå)
	for (int i = 0; i < nAlarmCount; ++i)
	{
		CString strAppName;

		strAppName.Format(_T("Alarm%d"), i);

		CString strAlarmType = INIReadStr(strAppName, _T("Type"), strFilePath);
		CString strAlarmFilePath = INIReadStr(strAppName, _T("Name"), strFilePath);
		CString strAlarmCount = INIReadStr(strAppName, _T("Count"), strFilePath);

		CAlarmInverterList* pAlarmList = new CAlarmInverterList;

		pAlarmList->LoadAlarmInverter(strAlarmType, strAlarmCount, strAlarmFilePath);


		Add(strAlarmType, pAlarmList);
	}
}

void	CAlarmInverterMap::Add(CString strType, CAlarmInverterList* pNewElement)
{
	int nKey = 0;

	if (strType.CompareNoCase(_T("IGBT_Can")) == 0)	nKey = eAlarmInverterTypeCANIGBT;
	else if (strType.CompareNoCase(_T("Warning_Can")) == 0)	nKey = eAlarmInverterTypeCANWarning;
	else if (strType.CompareNoCase(_T("Grid_Can")) == 0)	nKey = eAlarmInverterTypeCANGrid;
	else if (strType.CompareNoCase(_T("DC_Can")) == 0)		nKey = eAlarmInverterTypeCANDC;
	else if (strType.CompareNoCase(_T("Fault_Can")) == 0)	nKey = eAlarmInverterTypeCANFault;

	auto it = m_mapAlarm.find(nKey);

	if (it == m_mapAlarm.end())
	{
		m_mapAlarm.insert(std::pair<int, CAlarmInverterList*>(nKey, pNewElement));
	}

}

void	CAlarmInverterMap::RemoveAll()
{
	for (auto it = m_mapAlarm.begin(); it != m_mapAlarm.end(); ++it)
	{
		delete it->second;
	}

	m_mapAlarm.clear();
}

std::map< int, CAlarmInverterList* >&	CAlarmInverterMap::GetAlarmMap()
{
	return m_mapAlarm;
}

CAlarmInverterList*	CAlarmInverterMap::GetAlarmList(int nAlarmType)
{
	auto it = m_mapAlarm.find(nAlarmType);

	if (it == m_mapAlarm.end())
		return NULL;

	return it->second;
}

BOOL	CAlarmInverterMap::ExistAlarm()
{
	for (auto it = m_mapAlarm.begin(); it != m_mapAlarm.end(); ++it)
	{
		CAlarmInverterList* pAlarmList = it->second;

		if (NULL == pAlarmList)
			continue;

		if (TRUE == pAlarmList->ExistAlarm())
			return TRUE;
	}

	return FALSE;
}


