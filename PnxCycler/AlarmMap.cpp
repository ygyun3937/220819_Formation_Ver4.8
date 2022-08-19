#include "stdafx.h"
#include "AlarmMap.h"

#define	AlarmListFileName	(_T("MgrAlarm.ini"))

CAlarmMap::CAlarmMap()
{
}


CAlarmMap::~CAlarmMap()
{
	RemoveAll();
}

void	CAlarmMap::LoadAlarm()
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

		CString strAlarmType		= INIReadStr(strAppName, _T("Type"), strFilePath);
		CString strAlarmFilePath	= INIReadStr(strAppName, _T("Name"), strFilePath);
		CString strAlarmCount		= INIReadStr(strAppName, _T("Count"), strFilePath);

		CAlarmList* pAlarmList = new CAlarmList;

		pAlarmList->LoadAlarm(strAlarmType, strAlarmCount, strAlarmFilePath);

		
		Add(strAlarmType, pAlarmList);
	}	
}

void	CAlarmMap::Add(CString strType, CAlarmList* pNewElement)
{
	int nKey = 0;

		 if (strType.CompareNoCase(_T("Warning_IF")) == 0) nKey = eAlarmTypeIFWarning;
	else if (strType.CompareNoCase(_T("DC_IF")) == 0)		nKey = eAlarmTypeIFDC;
	else if (strType.CompareNoCase(_T("Battery_IF")) == 0)	nKey = eAlarmTypeIFBattery;
	else if (strType.CompareNoCase(_T("Fault_IF")) == 0)	nKey = eAlarmTypeIFFault;

		 auto it = m_mapAlarm.find(nKey);

		 if (it == m_mapAlarm.end())
		 {
			 m_mapAlarm.insert(std::pair<int, CAlarmList*>(nKey, pNewElement));
		 }

}

void	CAlarmMap::RemoveAll()
{
	for (auto it = m_mapAlarm.begin(); it != m_mapAlarm.end(); ++it)
	{
		delete it->second;
	}

	m_mapAlarm.clear();
}

std::map< int, CAlarmList* >&	CAlarmMap::GetAlarmMap()
{
	return m_mapAlarm;
}

CAlarmList*	CAlarmMap::GetAlarmList(int nAlarmType)
{
	auto it = m_mapAlarm.find(nAlarmType);

	if (it == m_mapAlarm.end())
		return NULL;

	return it->second;
}

BOOL	CAlarmMap::ExistAlarm()
{
	for (auto it = m_mapAlarm.begin(); it != m_mapAlarm.end(); ++it)
	{
		CAlarmList* pAlarmList = it->second;

		if (NULL == pAlarmList)
			continue;

		if (TRUE == pAlarmList->ExistAlarm())
			return TRUE;
	}

	return FALSE;
}