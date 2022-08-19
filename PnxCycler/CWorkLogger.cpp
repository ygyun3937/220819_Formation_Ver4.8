#include "stdafx.h"
#include "CWorkLogger.h"


CWorkLogger::CWorkLogger()
{	
}

CWorkLogger::~CWorkLogger()
{
	_regKey.Close();
}


int CWorkLogger::Initialize(int ch)
{	
	CString keyName;
	keyName.Format("SOFTWARE\\PnxCycler\\%02d", ch);

	if (_regKey.Create(HKEY_CURRENT_USER, keyName) == ERROR_SUCCESS) {
		//_regKey.Close();
	}
	return 0;
}

CString CWorkLogger::GetStringValue(LPCSTR valueName)
{		
	CString strValue;
	DWORD dwSize = 1024;
	
	if (_regKey.QueryStringValue(valueName, strValue.GetBuffer(dwSize), &dwSize) == ERROR_SUCCESS) {
		strValue.ReleaseBuffer();
	}

	return strValue;
}

int CWorkLogger::GetIntValue(LPCSTR valueName)
{
	DWORD dwValue = 0;

	if (_regKey.QueryDWORDValue(valueName, dwValue) == ERROR_SUCCESS) {
		;
	}

	return (int)dwValue;
}

void CWorkLogger::WriteIntValue(LPCSTR valueName, int value)
{
	_regKey.SetDWORDValue(valueName, (DWORD)value);
}

void CWorkLogger::WriteStringValue(LPCSTR valueName, LPCSTR value)
{
	_regKey.SetStringValue(valueName, value);
}