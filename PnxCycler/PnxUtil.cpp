#include "stdafx.h"
#include "PnxUtil.h"

#include <vector>

CPnxUtil::CPnxUtil()
{
	
}


CPnxUtil::~CPnxUtil()
{
}

void CPnxUtil::SplitString(LPCTSTR lpszFullString, TCHAR chSep, CStringArray& arr, BOOL bEmptyData /*= FALSE*/)
{
	// By JSK - strtok 대신 해당 함수를 쓰는 이유에 대하여 간략히 기술
	// 1. strtok 함수는 내부적으로 token 을 static 으로 관리하기 때문에 Thread 환경에서 안전하지 않음
	// 2. strtok 함수는 원본 Data 가 파괴될 가능성이 존재 함
	// 3. MFC 에서는 기본적으로 Afx 계열의 함수를 권장하고 있음
	arr.RemoveAll();

	CString rString;
	int		nSubString = 0;

	while (AfxExtractSubString(rString, lpszFullString, nSubString++, chSep))
	{
		if (TRUE == rString.IsEmpty() && FALSE == bEmptyData)
			continue;

		arr.Add(rString);
	}
}

void CPnxUtil::SplitStringToHex(LPCTSTR lpszFullString, TCHAR chSep, CStringArray& arr, BOOL bEmptyData /*= FALSE*/)
{
	// By JSK - strtok 대신 해당 함수를 쓰는 이유에 대하여 간략히 기술
	// 1. strtok 함수는 내부적으로 token 을 static 으로 관리하기 때문에 Thread 환경에서 안전하지 않음
	// 2. strtok 함수는 원본 Data 가 파괴될 가능성이 존재 함
	// 3. MFC 에서는 기본적으로 Afx 계열의 함수를 권장하고 있음
	arr.RemoveAll();

	CString rString;
	int		nSubString = 0;

	while (AfxExtractSubString(rString, lpszFullString, nSubString++, chSep))
	{
		if (TRUE == rString.IsEmpty() && FALSE == bEmptyData)
			continue;

		CString TempValue;
		TempValue.Format(_T("%d"), String2Hex(rString));

		arr.Add(TempValue);
	}
}

void	CPnxUtil::SplitByte(BYTE byData, CArray< int, int >& arr)
{
	BOOL arrBit[8] = { FALSE, };

	for (int i = 0; i < 8; ++i)
	{
		arrBit[i] = (byData & (1 << i)) != 0;

		if (FALSE == arrBit[i])
			continue;

		arr.Add(i);
	}
}

void	CPnxUtil::SplitWord(BYTE* byData, CArray< int, int >& arr)
{
	BOOL arrBit[16] = { FALSE, };

	for (int i = 0; i < 16; ++i)
	{
		arrBit[i] = (*byData & (1 << i)) != 0;

		if (FALSE == arrBit[i])
			continue;

		arr.Add(i);
	}
}

void CPnxUtil::SplitWord(WORD byData, CArray<int, int>& arr)
{
	BOOL arrBit[16] = { FALSE, };

	for (int i = 0; i < 16; ++i)
	{
		arrBit[i] = (byData & (1 << i)) != 0;

		if (FALSE == arrBit[i])
			continue;

		arr.Add(i);
	}
}

BOOL	CPnxUtil::IsExistFile(CString strFilePath)
{
	CFileFind findfile;

	BOOL bFindFile = findfile.FindFile(strFilePath);

	findfile.Close();

	return bFindFile;
}

void CPnxUtil::DeleteAllFile(LPCSTR szDir)
{
	CString strName;
	strName.Format("%s\\*.*", szDir);
	CFileFind ff;
	BOOL bFind = ff.FindFile(strName);
	while (bFind)
	{
		bFind = ff.FindNextFile();
		if (ff.IsDots() == TRUE || ff.IsDirectory() == TRUE)
		{
			continue;
		}
		DeleteFile(ff.GetFilePath());
	}
	ff.Close();
}

int		CPnxUtil::Char2Hex(TCHAR psz)
{
	if (psz >= _T('0') && psz <= _T('9'))   //--> '0' ~ '9' => 0 ~ 9
		return psz - _T('0');

	if (psz >= _T('a') && psz <= _T('f'))  //--> 'A' ~ 'F' => 10 ~ 15
		return psz - _T('a') + 10;

	if (psz >= _T('A') && psz <= _T('F'))  //--> 'A' ~ 'F' => 10 ~ 15
		return psz - _T('A') + 10;

	return -1;   // 문자가 숫자를 벗어난 경우
}

int		CPnxUtil::String2Hex(CString str)
{
	TCHAR pszStr[1024] = {0,};

	memset(pszStr, NULL, 1024);

	int nSize = str.GetLength();

	_tcscpy_s(pszStr, nSize + 1, str);

	LPSTR lpsz = pszStr;

	int nRetHex = 0;

	for (int i = 0; i < nSize; ++i)
	{
		int nHex = Char2Hex(*lpsz++);

		int nShift = 4 * (nSize - i - 1);

		nRetHex = nRetHex + (nHex << nShift);

		if (nHex == -1)
			return -1;
	}

	return nRetHex;
}

CString	CPnxUtil::GetTime()
{
	SYSTEMTIME tmCur;

	GetLocalTime(&tmCur);

	CString strTime;

	strTime.Format("%02d:%02d:%02d:%03d", tmCur.wHour, tmCur.wMinute, tmCur.wSecond, tmCur.wMilliseconds);

	return strTime;
}

CString CPnxUtil::GetAllDate()
{
	SYSTEMTIME tmCur;

	GetLocalTime(&tmCur);

	CString strTime;

	strTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d",
		tmCur.wYear,
		tmCur.wMonth,
		tmCur.wDay,
		tmCur.wHour,
		tmCur.wMinute,
		tmCur.wSecond,
		tmCur.wMilliseconds);

	return strTime;
}

CString	CPnxUtil::Integer2Str(int nTime)
{
	int nHour	= (nTime / 1000 / 60 / 60) % 60;
	int nMinute = (nTime / 1000 / 60) % 60;
	int nSecond = (nTime / 1000) % 60;
	int nMilSec = nTime % 1000;

	CString str;

	str.Format(_T("%02d:%02d:%02d.%03d"), nHour, nMinute, nSecond, nMilSec);

	return str;
}

int		CPnxUtil::GetSplitTime(LPCTSTR lpszFullString, TCHAR chSep)
{
	CStringArray arr;

	SplitString(lpszFullString, chSep, arr);

	// MilSec 을 대응 하려면 SYSTEMTIME 을 써야 하는데, SYSTEMTIME 은 TimeSpan 을 이용하지 못한다..
	int nTime = 0;

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		CString str = arr.GetAt(i);

		switch (i)
		{
		case 0: nTime += atoi(str) * 60 * 60 * 24 * 1000;		break;
		case 1: nTime += atoi(str) * 60 * 60 * 1000;			break;
		case 2: nTime += atoi(str) * 60 * 1000;					break;
		case 3: nTime += atoi(str) * 1000;						break;
		case 4: nTime += atoi(str);								break;
		}
	}

	return nTime;
}

DWORD	CPnxUtil::HexTextToInt(CString ToConvert)
{
	DWORD iToReturn = 0;
	int iExp = 0;
	char chByte;

	if (ToConvert == "")
		return 0;
	
	if (ToConvert.GetLength() > 8)
		return 0;
	
	ToConvert = ToConvert.MakeUpper();

	try
	{
		for (int i = ToConvert.GetLength() - 1; i >= 0; i--){
			chByte = ToConvert[i];
			switch (int(chByte)){
			case 65:
				iToReturn += (DWORD)(10 * pow(16.0f, iExp));
				break;
			case 66:
				iToReturn += (DWORD)(11 * pow(16.0f, iExp));
				break;
			case 67:
				iToReturn += (DWORD)(12 * pow(16.0f, iExp));
				break;
			case 68:
				iToReturn += (DWORD)(13 * pow(16.0f, iExp));
				break;
			case 69:
				iToReturn += (DWORD)(14 * pow(16.0f, iExp));
				break;
			case 70:
				iToReturn += (DWORD)(15 * pow(16.0f, iExp));
				break;
			default:
				if ((int(chByte) <48) || (int(chByte)>57))
					return 0;
				iToReturn += (DWORD)(atoi(&chByte)*pow(16.0f, iExp));
				break;

			}
			iExp++;
		}
	}
	catch (...)
	{
		return 0;
	}

	return iToReturn;
}

CString	CPnxUtil::Int2Hex(int nData, int nSize)
{
	vector< int > vtHex;

	while (nData)
	{
		if (nData & 1)
		{
			vtHex.push_back(1);
		}
		else
		{
			vtHex.push_back(0);
		}

		nData >>= 1;
	}

	for (int i = vtHex.size(); i < nSize; ++i)
	{
		vtHex.push_back(0);
	}

	reverse(vtHex.begin(), vtHex.end());

	CString strHex;

	for (auto it = vtHex.begin(); it != vtHex.end(); ++it)
	{
		CString str;

		str.Format("%d", *it);

		strHex += str;
	}

	return strHex;
}

int		CPnxUtil::Hex2Int(BYTE byData)
{
	char pszHex[20] = {0,};
	_itoa_s(byData, pszHex, 16 );

	return atoi(pszHex);
}
char* CPnxUtil::ANSItoUTF8(char* pszCode)
{
	BSTR bstrCode;
	char* pszUTFCode = NULL;
	int  nLength, nLength2;

	nLength = MultiByteToWideChar(CP_ACP, 0, pszCode, lstrlen(pszCode), NULL, NULL);
	bstrCode = SysAllocStringLen(NULL, nLength);
	if (bstrCode != 0) {
		MultiByteToWideChar(CP_ACP, 0, pszCode, lstrlen(pszCode), bstrCode, nLength);

		nLength2 = WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, pszUTFCode, 0, NULL, NULL);
		pszUTFCode = (char*)malloc(nLength2 + 1);
		WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, pszUTFCode, nLength2, NULL, NULL);

		return pszUTFCode;
	}

	return NULL;
}