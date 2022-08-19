
// stdafx.cpp : source file that includes just the standard includes
// PnxCycler.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "MgrSystemLog.h"
#include "MgrConfig.h"

CCriticalSection g_criticalExe;
CCriticalSection g_criticalLog;

bool g_bPatternStep[32];
int g_nPatternStepCount;
int g_nPatternStartStep[32];

int g_nScheduleCycleLoop[1000];
int g_nCycleLoopStartSet[1000];

double g_dTotalCapLog[32];
double g_dStepCapLog[32][3];
int g_nTotalCapCalIndex[32];

int g_nAlarmStatus[32];

int g_nRatioSpec[32];

//
//void LOG(int nType, const char* fmt, ...)
//{
//	//g_criticalLog.Lock();
//
//	static _TCHAR szLog[5096];
//
//	FILE*		fp = NULL;
//	CTime		tm = CTime::GetCurrentTime();
//	CString		strLog = _T("");
//	CString		strPath = _T("");
//	va_list		args;
//	SYSTEMTIME	cur_time;
//
//	if (fmt == NULL)
//	{
//		//g_criticalLog.Unlock();
//
//		return;
//	}
//
//	ZeroMemory(szLog, 5096);
//	va_start(args, fmt);
//	wvsprintf(szLog, fmt, args);
//	va_end(args);
//
//	strPath.Format("%s\\MainLog\\%s", GetExePath(), tm.Format("%Y_%m_%d_%H.log").GetString());
//
//	fopen_s(&fp, (LPSTR)(LPCSTR)strPath, "a+");
//
//	GetLocalTime(&cur_time);
//
//	strLog.Format("%04d-%02d-%02d %02d:%02d:%02d:%03ld : ",
//		cur_time.wYear,
//		cur_time.wMonth,
//		cur_time.wDay,
//		cur_time.wHour,
//		cur_time.wMinute,
//		cur_time.wSecond,
//		cur_time.wMilliseconds);
//
//	if (fp != NULL)
//	{
//		switch (nType)
//		{
//		case Normal		: strLog += _T("[NORMAL    ] : ");	break;
//		case Dbg		: strLog += _T("[DEBUG     ] : ");	break;
//		case Operation	: strLog += _T("[OPERATION ] : ");	break;
//		case Err		: strLog += _T("[ERROR     ] : ");  break;
//		}
//
//		strLog += szLog;
//
//		fprintf(fp, "%s\r\n", (LPSTR)(LPCSTR)strLog);
//		fflush(fp);
//		fclose(fp);
//	}
////	Sleep(2);
//	//g_criticalLog.Unlock();
//}

CString	GetExePath()
{
	g_criticalExe.Lock();

	static TCHAR pBuf[256] = { 0, };

	memset(pBuf, NULL, sizeof(pBuf));

	if (AfxGetApp())
		GetModuleFileName(AfxGetApp()->m_hInstance, pBuf, sizeof(pBuf));
	else
		GetModuleFileName(NULL, pBuf, sizeof(pBuf));

	CString strFilePath;

	strFilePath.Format(_T("%s"), pBuf);

	strFilePath = strFilePath.Left(strFilePath.ReverseFind(_T('\\')));

	g_criticalExe.Unlock();

	return strFilePath.GetString();
}

void	INIWriteStr(CString strAppName, CString strKeyName, CString strValue, CString strFilePath)
{
	WritePrivateProfileString(strAppName, strKeyName, _T(strValue), strFilePath);
}

void	INIWriteInt(CString strAppName, CString strKeyName, int nValue, CString strFilePath)
{
	CString str;

	str.Format(_T("%d"), nValue);

	WritePrivateProfileString(strAppName, strKeyName, str, strFilePath);
}

void	INIWriteFloat(CString strAppName, CString strKeyName, float fValue, CString strFilePath)
{
	CString str;

	str.Format(_T("%.3f"), fValue);

	WritePrivateProfileString(strAppName, strKeyName, str, strFilePath);
}

CString	INIReadStr(CString strAppName, CString strKeyName, CString strFilePath)
{
	TCHAR szReturnString[1024] = { 0, };

	memset(szReturnString, NULL, 1024);

	GetPrivateProfileString(strAppName, strKeyName, _T(""), szReturnString, 1024, strFilePath);

	CString str;

	str.Format(_T("%s"), szReturnString);

	return str;
}

CString	INIReadStrW(CString strAppName, CString strKeyName, CString strFilePath)
{
	WCHAR szReturnString[1024] = { 0, };

	memset(szReturnString, NULL, 1024);

	LPCWSTR tempAppName = (LPCWSTR)(LPCTSTR)strAppName;
	LPCWSTR tempKeyName = (LPCWSTR)(LPCTSTR)strKeyName;
	LPCWSTR tempFilePath = (LPCWSTR)(LPCTSTR)strFilePath;

	GetPrivateProfileStringW(tempAppName, tempKeyName, (LPCWSTR)"", szReturnString, 1024, tempFilePath);

	CString str;

	str.Format(_T("%s"), szReturnString);

	return str;
}

int		INIReadInt(CString strAppName, CString strKeyName, CString strFilePath)
{
	TCHAR szReturnString[1024] = { 0, };

	memset(szReturnString, NULL, 1024);

	GetPrivateProfileString(strAppName, strKeyName, _T(""), szReturnString, 1024, strFilePath);

	int nRet = atoi(szReturnString);

	return nRet;
}

float	INIReadFloat(CString strAppName, CString strKeyName, CString strFilePath)
{
	TCHAR szReturnString[1024] = { 0, };

	memset(szReturnString, NULL, 1024);

	GetPrivateProfileString(strAppName, strKeyName, _T(""), szReturnString, 1024, strFilePath);

	float fRet = (float)atof(szReturnString);

	return fRet;
}

float	AbsFunc(float f)
{
	if( f < 0.0)
		f *= -1;

	return f;
}

//Camber Type 구별 8채널 16채널
int GetChamberType() 
{
	int type = CMgrConfig::GetMgr()->GetChamberChannelCount();

	//8채널 -> Chamber 4개 return 0
	if (type == 8)
		return 0;
	//16채널 -> Chamber 2개 return 1
	else if (type == 16)
		return 1;
	//2채널
	else if (type == 2)
		return 2;
	//4채널
	else if (type == 4)
		return 3;

	return 0;
}

// channel 에 따른 Chamber Index 를 반환해 준다.
int GetChamberIndex(int channel)
{
	//8채널이면 0 - 챔버 4개
	if (GetChamberType() == 0)
	{
		switch (channel)
		{
		case 0: case 1:
			return 0;
			break;
		case 2: case 3:
			return 1;
			break;
		case 4: case 5:
			return 2;
			break;
		case 6: case 7:
			return 3;
			break;
		}
	}
	//16채널이면 1 - 챔버 2개
	else if (GetChamberType() == 1)
	{
		switch (channel)
		{
		case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
			return 0;
			break;
		case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
			return 1;
			break;
		}
	}
	else if (GetChamberType() == 2)
	{
		return 0;
	}
	else if (GetChamberType() == 3)
	{
		switch (channel)
		{
		case 0: case 1:
			return 0;
			break;
		case 2: case 3:
			return 1;
			break;
		}
	}
	
	return 0;
}

//pnx_hr 2021.05.12 
void SetAdLinkType(bool bAdLink)
{
	m_bAdLink = bAdLink;
}

BOOL GetAdLinkType()
{
	return m_bAdLink;
}


