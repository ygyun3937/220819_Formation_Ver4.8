#include "stdafx.h"
#include "CInverterAlarm.h"

CInverterAlarm::CInverterAlarm()
{
	m_strAlarmTitle.Empty();
	m_strAlarmContents.Empty();
}

CInverterAlarm::~CInverterAlarm()
{
}

void CInverterAlarm::SetAlarm(CString strTitle, CString strContents)
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

CString CInverterAlarm::GetAlarmTitle()
{
	return m_strAlarmTitle.GetString();
}

CString CInverterAlarm::GetAlarmContents()
{
	return m_strAlarmTitle.GetString();
}

CString CInverterAlarm::GetAlarmTime()
{
	return m_strCreatedAt.GetString();
}
