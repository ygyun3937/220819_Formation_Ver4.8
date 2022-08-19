#include "stdafx.h"
#include "SysAlarm.h"

CSysAlarm::CSysAlarm()
{
	m_strAlarmTitle.Empty();
	m_strAlarmContents.Empty();
}

CSysAlarm::~CSysAlarm()
{
}

void		CSysAlarm::SetAlarm(CString strTitle, CString strContents)
{
	CTime cTime = CTime::GetCurrentTime(); // ���� �ý������κ��� ��¥ �� �ð��� ��� �´�.

	CString strDate, strTime; // ��ȯ�Ǵ� ��¥�� �ð��� ������ CString ���� ����
	strDate.Format("%04dY %02dM %02dD", cTime.GetYear(), // ���� �⵵ ��ȯ
		cTime.GetMonth(), // ���� �� ��ȯ
		cTime.GetDay()); // ���� �� ��ȯ


	strTime.Format("%02dH %02dM %02dS", cTime.GetHour(), // ���� �ð� ��ȯ
		cTime.GetMinute(), // ���� �� ��ȯ
		cTime.GetSecond()); // ���� �� ��ȯ

	m_strCreatedAt = strDate + " " + strTime;
	m_strAlarmTitle = strTitle;
	m_strAlarmContents = strContents;
}

CString		CSysAlarm::GetAlarmTitle()
{
	return m_strAlarmTitle.GetString();
}

CString	CSysAlarm::GetAlarmContents()
{
	return m_strAlarmContents.GetString();
}

CString	CSysAlarm::GetAlarmTime()
{
	return m_strCreatedAt.GetString();
}

