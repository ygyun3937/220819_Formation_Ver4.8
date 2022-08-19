// CellT7_1.h: interface for the CCellT7_1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__VERSION_H__INCLUDED_)
#define __VERSION_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


bool	GetAppVersion(	IN	LPCTSTR	lpszAppName,
						OUT	LPDWORD pdwVerMajor,		OUT	LPDWORD pdwVerMinor, 
						OUT	LPDWORD pdwVerRelease,		OUT	LPDWORD pdwVerTest, 
						OUT	LPTSTR	lpszPrivateBuild,	OUT	LPTSTR	lpszProductName,	
						OUT	LPTSTR	lpszCompanyName, 	OUT	LPTSTR	lpszCopyright,
						OUT LPTSTR lpszEndUserName );

CString GetVersionString();
CString GetVersionPrivateBuild();
CString GetVersionEndUserName();

#endif // !defined(__VERSION_H__INCLUDED_)
