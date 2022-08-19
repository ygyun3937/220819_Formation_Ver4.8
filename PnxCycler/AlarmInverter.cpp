#include "stdafx.h"
#include "AlarmInverter.h"


CAlarmInverter::CAlarmInverter()
{
	m_nAlarmType = eAlarmInverterTypeUnknown;
	m_bAlarmSet = FALSE;

	m_strAlarmText.Empty();
}

CAlarmInverter::~CAlarmInverter()
{
}

int		CAlarmInverter::GetAlarmType()
{
	return m_nAlarmType;
}

void	CAlarmInverter::SetAlarmType(CString strType)
{
	if (strType.CompareNoCase(_T("IGBT_Can")) == 0)	m_nAlarmType = eAlarmInverterTypeCANIGBT;
	else if (strType.CompareNoCase(_T("Warning_Can")) == 0) m_nAlarmType = eAlarmInverterTypeCANWarning;
	else if (strType.CompareNoCase(_T("Grid_Can")) == 0)	m_nAlarmType = eAlarmInverterTypeCANGrid;
	else if (strType.CompareNoCase(_T("DC_Can")) == 0)		m_nAlarmType = eAlarmInverterTypeCANDC;
	else if (strType.CompareNoCase(_T("Fault_Can")) == 0)	m_nAlarmType = eAlarmInverterTypeCANFault;
}

CString	CAlarmInverter::GetAlarmText()
{
	return m_strAlarmText.GetString();
}

void	CAlarmInverter::SetAlarmText(CString strAlarmText)
{
	m_strAlarmText = strAlarmText;
}

CString CAlarmInverter::GetAlarmComment()
{
	if (m_bAlarmSet)
	{
		return m_strAlarmComment.GetString();
	}

	return _T("");
}

void CAlarmInverter::SetAlarmComment(CString strAlarmComment)
{
	m_strAlarmComment = strAlarmComment;
}

BOOL	CAlarmInverter::GetAlarmSet()
{
	return m_bAlarmSet;
}

void	CAlarmInverter::SetAlarmSet(BOOL bAlarm)
{
	m_bAlarmSet = bAlarm;
}

CString	CAlarmInverter::GetAlarmStr()
{
	if (m_bAlarmSet)
	{
		return m_strAlarmText.GetString();
	}

	return _T("");
}

