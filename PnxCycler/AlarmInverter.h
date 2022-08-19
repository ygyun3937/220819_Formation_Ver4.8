#pragma once

enum eAlarmInverterType
{
	eAlarmInverterTypeUnknown = -1,
	eAlarmInverterTypeCANIGBT,
	eAlarmInverterTypeCANWarning,
	eAlarmInverterTypeCANGrid,
	eAlarmInverterTypeCANDC,
	eAlarmInverterTypeCANFault,
	eAlarmInverterTypeMaxCount
};

class CAlarmInverter
{
public:
	CAlarmInverter();
	virtual ~CAlarmInverter();

	int		GetAlarmType();
	void	SetAlarmType(CString strType);

	CString	GetAlarmText();
	void	SetAlarmText(CString strAlarmText);

	CString	GetAlarmComment();
	void	SetAlarmComment(CString strAlarmComment);

	BOOL	GetAlarmSet();
	void	SetAlarmSet(BOOL bAlarm);

	CString	GetAlarmStr();
private:
	int		m_nAlarmType;

	CString	m_strAlarmText;
	
	CString m_strAlarmComment;

	BOOL	m_bAlarmSet;
};

