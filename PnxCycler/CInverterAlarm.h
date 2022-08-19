#pragma once
//lyw4_1 : 시스템 알람 객체
class CInverterAlarm
{
public:
	CInverterAlarm();
	virtual ~CInverterAlarm();

	void SetAlarm(CString strTitle, CString strContents);

	CString	GetAlarmTitle();
	CString	GetAlarmContents();
	CString GetAlarmTime();

private:
	CString m_strAlarmTitle;
	CString	m_strAlarmContents;
	CString m_strCreatedAt; //알람 생성 시간
};
