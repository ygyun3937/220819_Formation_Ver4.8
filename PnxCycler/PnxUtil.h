#pragma once

#include "Singleton.h"

// �ʿ��� Util �� ��ɵ� ����

using namespace std;

class CPnxUtil : public CSingleton< CPnxUtil >
{
private:
	int		Char2Hex(TCHAR psz);
public:
	CPnxUtil();
	virtual ~CPnxUtil();

	void	SplitString(LPCTSTR lpszFullString, TCHAR chSep, CStringArray& arr, BOOL bEmptyData = FALSE );	// Ư�� ���ڿ����� �����ڸ� ���� ���ڿ� ����
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
