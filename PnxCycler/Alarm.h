#pragma once

enum eAlarmType
{
	eAlarmTypeUnknown = -1,
	eAlarmTypeIFWarning,
	eAlarmTypeIFDC,
	eAlarmTypeIFBattery,
	eAlarmTypeIFFault,
	eAlarmTypeMaxCount
};

class CAlarm
{
public:
	CAlarm();
	virtual ~CAlarm();

	int		GetAlarmType();
	void	SetAlarmType(CString strType);

	CString	GetAlarmText();
	void	SetAlarmText(CString strAlarmText);

	CString	GetAlarmComment();
	void	SetAlarmComment(CString strAlarmComment);

	BOOL	GetAlarmSet();
	void	SetAlarmSet(BOOL bAlarm);

	CString	GetAlarmStr();

	//��ġ���
	void SetAlarmTroubleShootingManual(CString strMsg);
	CString GetAlarmTroubleShootingManual();
private:
	int		m_nAlarmType;

	CString	m_strAlarmText;
	CString	m_strAlarmComment;

	BOOL	m_bAlarmSet;


	//��ġ ���
	CString m_strAlarmTroubleShootingManual;
};

