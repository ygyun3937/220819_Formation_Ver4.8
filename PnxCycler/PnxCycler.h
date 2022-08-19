
// PnxCycler.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <TlHelp32.h>


// CPnxCyclerApp:
// See PnxCycler.cpp for the implementation of this class
//

class CPnxCyclerApp : public CWinApp
{
public:
	CPnxCyclerApp();

// Overrides
public:
	virtual BOOL InitInstance();

	BOOL killProcess();
private:
	ULONG_PTR m_gpToken;
// Implementation


	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CPnxCyclerApp theApp;
