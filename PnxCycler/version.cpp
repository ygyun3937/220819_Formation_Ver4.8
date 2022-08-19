#include "stdafx.h"

#include "version.h"
#pragma comment( lib, "version.lib" )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


bool GetAppVersion(	IN	LPCTSTR	lpszAppName,
					OUT LPDWORD	pdwVerMajor, 
					OUT LPDWORD	pdwVerMinor, 
					OUT LPDWORD	pdwVerRelease, 
					OUT LPDWORD	pdwVerTest,
					OUT LPTSTR	lpszPrivateBuild,
					OUT	LPTSTR	lpszProductName,
					OUT LPTSTR	lpszCompanyName,
					OUT LPTSTR	lpszCopyright,
					OUT LPTSTR  lpszEndUserName )
{
	// START
	DWORD dwHandle	= 0;
	DWORD dwSize	= GetFileVersionInfoSize( (LPTSTR)lpszAppName, &dwHandle );

	if ( dwSize == 0 )
	{
		SYSLOG(Dbg, "GetFileVersionInfoSize call Failed.\n" );
		return false;
	}

	char* pVerData = new char[ dwSize ];

	if ( GetFileVersionInfo( (LPTSTR)lpszAppName, NULL, dwSize, pVerData ) == FALSE )
	{
		SYSLOG(Dbg, "GetFileVersionInfo call Failed.\n");

		delete[] pVerData;
		return false;
	}

	//--------------------------------------------

	LPVOID	pBuf	= NULL;
	UINT	nSize	= 0;

	//--------------------------------------------

	VS_FIXEDFILEINFO vsf;

	if ( VerQueryValue( pVerData, "\\", &pBuf, &nSize ) == 0 )
	{
		SYSLOG(Dbg, "VerQueryValue call 1 Failed.\n");
		delete[] pVerData;
		return false;
	}

	memcpy( &vsf, pBuf, sizeof( VS_FIXEDFILEINFO ) );
	
	if ( pdwVerMajor != NULL )
		*pdwVerMajor	= HIWORD( vsf.dwFileVersionMS );
	
	if ( pdwVerMinor != NULL )
		*pdwVerMinor	= LOWORD( vsf.dwFileVersionMS );
	
	if ( pdwVerRelease != NULL )
		*pdwVerRelease	= HIWORD( vsf.dwFileVersionLS );
	
	if ( pdwVerTest != NULL )
		*pdwVerTest		= LOWORD( vsf.dwFileVersionLS );

	//--------------------------------------------
	if ( VerQueryValue( pVerData, "\\VarFileInfo\\Translation", &pBuf, &nSize ) == 0 )
	{
		SYSLOG(Dbg, "VerQueryValue call 0 Failed.\n");
		delete[] pVerData;
		return false;
	}

	DWORD dwLangCode;

	memcpy( &dwLangCode, pBuf, sizeof( DWORD ) );

	CString strLangCodePage;
	strLangCodePage.Format( "%04x%04x", LOWORD( dwLangCode ), HIWORD( dwLangCode ) );

	char szSubBlock[ MAX_PATH ] = { 0, };

	//--------------------------------------------

	if ( lpszProductName != NULL )
	{
#if _MSC_VER >= 1300
		sprintf_s( szSubBlock, "StringFileInfo\\%s\\ProductName", strLangCodePage );
#else
		sprintf( szSubBlock, "StringFileInfo\\%s\\ProductName", strLangCodePage );
#endif

		
		if ( VerQueryValue( pVerData, szSubBlock, &pBuf, &nSize ) == 0 )
		{
			SYSLOG(Dbg, "VerQueryValue call 2 Failed.\n");
			delete[] pVerData;
			return false;
		}
		
		memcpy( lpszProductName, pBuf, nSize );
	}

	//--------------------------------------------
	if ( lpszCompanyName != NULL )
	{
#if _MSC_VER >= 1300
		sprintf_s( szSubBlock, "StringFileInfo\\%s\\CompanyName", strLangCodePage );
#else			
		sprintf( szSubBlock, "StringFileInfo\\%s\\CompanyName", strLangCodePage );
#endif
		if ( VerQueryValue( pVerData, szSubBlock, &pBuf, &nSize ) == 0 )
		{
			SYSLOG(Dbg, "VerQueryValue call 2 Failed.\n");
			delete[] pVerData;
			return false;
		}

		memcpy( lpszCompanyName, pBuf, nSize );
	}
		
	//--------------------------------------------
	if ( lpszCopyright != NULL )
	{
#if _MSC_VER >= 1300
		sprintf_s( szSubBlock, "StringFileInfo\\%s\\LegalCopyright", strLangCodePage );
#else			
		sprintf( szSubBlock, "StringFileInfo\\%s\\LegalCopyright", strLangCodePage );
#endif
		
		if ( VerQueryValue( pVerData, szSubBlock, &pBuf, &nSize ) == 0 )
		{
			SYSLOG(Dbg, "VerQueryValue call 3 Failed.\n");
			delete[] pVerData;
			return false;
		}
		
		memcpy( lpszCopyright, pBuf, nSize );
	}

	//--------------------------------------------
	if ( lpszPrivateBuild != NULL )
	{
#if _MSC_VER >= 1300
		sprintf_s( szSubBlock, "StringFileInfo\\%s\\PrivateBuild", strLangCodePage );
#else			
		sprintf( szSubBlock, "StringFileInfo\\%s\\PrivateBuild", strLangCodePage );
#endif
		
		if ( VerQueryValue( pVerData, szSubBlock, &pBuf, &nSize ) == 0 )
		{
			SYSLOG(Dbg, "VerQueryValue call 3 Failed.\n");
			delete[] pVerData;
			return false;
		}
		
		memcpy( lpszPrivateBuild, pBuf, nSize );
	}

	//--------------------------------------------
	if (lpszEndUserName != NULL)
	{
#if _MSC_VER >= 1300
		sprintf_s(szSubBlock, "StringFileInfo\\%s\\ProgramEndUser", strLangCodePage);
#else			
		sprintf(szSubBlock, "StringFileInfo\\%s\\ProgramEndUser", strLangCodePage);
#endif

		if (VerQueryValue(pVerData, szSubBlock, &pBuf, &nSize) == 0)
		{
			SYSLOG(Dbg, "VerQueryValue call 3 Failed.\n");
			delete[] pVerData;
			return false;
		}

		memcpy(lpszEndUserName, pBuf, nSize);
	}

	//--------------------------------------------
	delete[] pVerData;

	return true;
}


//----------------------------------------------------------------
CString GetVersionString()
{
	ULONG		ulVerMajor;
	ULONG		ulVerMinor;
	ULONG		ulVerRel;
	ULONG		ulVerTest;

	GetAppVersion(	__argv[ 0 ],&ulVerMajor,	
								&ulVerMinor,	
								&ulVerRel,		
								&ulVerTest, 
								NULL, NULL, NULL, NULL, NULL );

	CString strAppName;
	strAppName.Format( "%d.%d.%d.%d", ulVerMajor, ulVerMinor, ulVerRel, ulVerTest );

	return strAppName;
}

//----------------------------------------------------------------
CString GetVersionPrivateBuild()
{
	char szPrivateBuild	[ MAX_PATH ] = { 0, };

	GetAppVersion(	__argv[ 0 ],
					NULL, NULL, NULL, NULL, 
					szPrivateBuild,	NULL, NULL, NULL, NULL );

	return szPrivateBuild;
}

CString GetVersionEndUserName()
{
	char szEndUserName[MAX_PATH] = { 0, };

	GetAppVersion(	__argv[0], 
					NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, szEndUserName );

	return szEndUserName;
}