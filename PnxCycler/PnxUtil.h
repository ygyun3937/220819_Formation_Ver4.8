#pragma once

#include "Singleton.h"

// 필요한 Util 성 기능들 모음

using namespace std;

class CPnxUtil : public CSingleton< CPnxUtil >
{
private:
	int		Char2Hex(TCHAR psz);
public:
	CPnxUtil();
	virtual ~CPnxUtil();

	void	SplitString(LPCTSTR lpszFullString, TCHAR chSep, CStringArray& arr, BOOL bEmptyData = FALSE );	// 특정 문자열에서 구분자를 통해 문자열 추출
	void	SplitStringToHex(LPCTSTR lpszFullString, TCHAR chSep, CStringArray& arr, BOOL bEmptyData = FALSE);
	void	SplitByte(BYTE byData, CArray< int, int >& arr);
	void	SplitWord(BYTE* byData, CArray< int, int >& arr);
	void	SplitWord(WORD byData, CArray< int, int >& arr);		// PCAN

	int		String2Hex(CString str);
	int		GetSplitTime(LPCTSTR lpszFullString, TCHAR chSep);

	int		Hex2Int(BYTE byData);

	DWORD	HexTextToInt(CString ToConvert);

	BOOL	IsExistFile( CString strFilePath );

	void	DeleteAllFile(LPCSTR szDir);

	CString	GetTime();
	CString GetAllDate();
	CString	Integer2Str(int nTime);

	CString	Int2Hex(int nData, int nSize);

	char* ANSItoUTF8(char* pszCode);	// 2021-02-02
};

