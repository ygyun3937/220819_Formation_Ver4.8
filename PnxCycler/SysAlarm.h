#pragma once
//lyw4_1 : �ý��� �˶� ��ü
class CSysAlarm
{
public:
	CSysAlarm();
	virtual ~CSysAlarm();

	void SetAlarm(CString strTitle, CString strContents);
	
	CString	GetAlarmTitle();
	CString	GetAlarmContents();
	CString GetAlarmTime();

private:
	CString m_strAlarmTitle;
	CString	m_strAlarmContents;
	CString m_strCreatedAt; //�˶� ���� �ð�
};

