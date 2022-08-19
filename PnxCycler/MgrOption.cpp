#include "stdafx.h"
#include "MgrOption.h"


CMgrOption::CMgrOption()
{
	m_nCanDelayTime		= 0;
	m_nCanDelayOption	= 0;
	m_nVoltageExpUnit	= 0;
	m_nAmpereExpUnit	= 0;
	m_bUseAutoCal		= FALSE;

	LoadFile();
}


CMgrOption::~CMgrOption()
{
}

int		CMgrOption::GetCanDelayTime()
{
	return m_nCanDelayTime;
}

void	CMgrOption::SetCanDelayTime(int nValue)
{
	m_nCanDelayTime = nValue;
}

int		CMgrOption::GetCanDelayOption()
{
	return m_nCanDelayOption;
}

void	CMgrOption::SetCanDelayOption(int nValue)
{
	m_nCanDelayOption = nValue;
}

int		CMgrOption::GetVoltageExpUnit()
{
	return m_nVoltageExpUnit;
}

void	CMgrOption::SetVoltageExpUnit(int nValue)
{
	m_nVoltageExpUnit = nValue;
}

int		CMgrOption::GetWattExpUnit()
{
	return m_nWattExpUnit;
}

void	CMgrOption::SetWattExpUnit(int nValue)
{
	m_nWattExpUnit = nValue;
}

int		CMgrOption::GetAmpereExpUnit()
{
	return m_nAmpereExpUnit;
}

void	CMgrOption::SetAmpereExpUnit(int nValue)
{
	m_nAmpereExpUnit = nValue;
}

BOOL	CMgrOption::GetAutoCal()
{
	return m_bUseAutoCal;
}

void	CMgrOption::SetAutoCal(int nValue)
{
	if (0 == nValue)
		m_bUseAutoCal = FALSE;
	else
		m_bUseAutoCal = TRUE;
}

void	CMgrOption::SaveFile()
{
	CString strFilePath;

	strFilePath.Format(_T("%s\\Config\\Option.ini"), GetExePath());

	CString strAppName = _T("Option");

	INIWriteInt(strAppName, _T("DelayTime")		, m_nCanDelayTime	, strFilePath);
	INIWriteInt(strAppName, _T("DelayOption")	, m_nCanDelayOption	, strFilePath);
	INIWriteInt(strAppName, _T("VoltageExp")	, m_nVoltageExpUnit	, strFilePath);
	INIWriteInt(strAppName, _T("AmpereExp")		, m_nAmpereExpUnit	, strFilePath);
	INIWriteInt(strAppName, _T("WattExp")		, m_nWattExpUnit	, strFilePath);
	INIWriteInt(strAppName, _T("AutoCal")		, m_bUseAutoCal		, strFilePath);
}

void	CMgrOption::LoadFile()
{
	CString strFilePath;

	strFilePath.Format(_T("%s\\Config\\Option.ini"), GetExePath());

	CString strAppName = _T("Option");

	m_nCanDelayTime		= INIReadInt(strAppName, _T("DelayTime")	, strFilePath);
	m_nCanDelayOption	= INIReadInt(strAppName, _T("DelayOption")	, strFilePath);
	m_nVoltageExpUnit	= INIReadInt(strAppName, _T("VoltageExp")	, strFilePath);
	m_nAmpereExpUnit	= INIReadInt(strAppName, _T("AmpereExp")	, strFilePath);
	m_nWattExpUnit		= INIReadInt(strAppName, _T("WattExp")		, strFilePath);
	m_bUseAutoCal		= INIReadInt(strAppName, _T("AutoCal")		, strFilePath);
}