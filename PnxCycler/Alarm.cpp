#include "stdafx.h"
#include "Alarm.h"


CAlarm::CAlarm()
{
	m_nAlarmType	= eAlarmTypeUnknown;
	m_bAlarmSet		= FALSE;

	m_strAlarmText.Empty();
}

CAlarm::~CAlarm()
{
}

int		CAlarm::GetAlarmType()
{
	return m_nAlarmType;
}

void	CAlarm::SetAlarmType(CString strType)
{
		 if (strType.CompareNoCase(_T("Warning_IF")) == 0) m_nAlarmType = eAlarmTypeIFWarning;
	else if (strType.CompareNoCase(_T("DC_IF")) == 0)		m_nAlarmType = eAlarmTypeIFDC;
	else if (strType.CompareNoCase(_T("Battery_IF")) == 0)	m_nAlarmType = eAlarmTypeIFBattery;
	else if (strType.CompareNoCase(_T("Fault_IF")) == 0)	m_nAlarmType = eAlarmTypeIFFault;
}

CString	CAlarm::GetAlarmText()
{
	return m_strAlarmText.GetString();
}

void	CAlarm::SetAlarmText(CString strAlarmText)
{
	m_strAlarmText = strAlarmText;
}

CString CAlarm::GetAlarmComment()
{
	if (m_bAlarmSet)
	{
		return m_strAlarmComment.GetString();
	}
	return _T("");
}

void CAlarm::SetAlarmComment(CString strAlarmComment)
{
	m_strAlarmComment = strAlarmComment;
}

BOOL	CAlarm::GetAlarmSet()
{
	return m_bAlarmSet;
}

void	CAlarm::SetAlarmSet(BOOL bAlarm)
{
	m_bAlarmSet = bAlarm;
}

CString	CAlarm::GetAlarmStr()
{
	if (m_bAlarmSet)
	{
		return m_strAlarmText.GetString();
	}

	return _T("");
}

void CAlarm::SetAlarmTroubleShootingManual(CString strMsg)
{
	m_strAlarmTroubleShootingManual = strMsg;
}

CString CAlarm::GetAlarmTroubleShootingManual()
{

	return m_strAlarmTroubleShootingManual;
}
