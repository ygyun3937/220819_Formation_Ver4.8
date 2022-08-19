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
	CTime cTime = CTime::GetCurrentTime(); // 현재 시스템으로부터 날짜 및 시간을 얻어 온다.

	CString strDate, strTime; // 반환되는 날짜와 시간을 저장할 CString 변수 선언
	strDate.Format("%04dY %02dM %02dD", cTime.GetYear(), // 현재 년도 반환
		cTime.GetMonth(), // 현재 월 반환
		cTime.GetDay()); // 현재 일 반환


	strTime.Format("%02dH %02dM %02dS", cTime.GetHour(), // 현재 시간 반환
		cTime.GetMinute(), // 현재 분 반환
		cTime.GetSecond()); // 현재 초 반환

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
