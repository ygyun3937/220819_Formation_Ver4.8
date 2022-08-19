
// PnxCycler.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "PnxCyclerDlg.h"

#include "PnxFormationDlg.h"

#include <mutex>
#include <iostream>

#include <dbghelp.h>

#pragma warning(pop)
#pragma comment(lib, "Dbghelp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class exception_handler {

private:
    static exception_handler* volatile _instance;
    static std::mutex _mutex;

    exception_handler() {}
    exception_handler(const exception_handler& other);
    ~exception_handler() {
        delete _instance;
    }

public:
    static exception_handler* volatile instance() {
        if (_instance == nullptr) {

            std::lock_guard<std::mutex> lock(_mutex);

            if (_instance == nullptr) {
                _instance = new exception_handler();
            }
        }
        return _instance;
    }

public:
    DWORD initialize(
        __in LPCTSTR dump_file_name,
        __in const MINIDUMP_TYPE dump_type = MINIDUMP_TYPE::MiniDumpNormal
    );

    DWORD run();

    static LONG WINAPI exception_callback(
        __in struct _EXCEPTION_POINTERS* exceptioninfo
    );

private:
    std::string _dump_file_name;
    MINIDUMP_TYPE _dump_type;
    LPTOP_LEVEL_EXCEPTION_FILTER _prev_filter;
};

//
// static members
//

exception_handler* volatile exception_handler::_instance = nullptr;
std::mutex exception_handler::_mutex;

//
// exception_handler.
//

DWORD exception_handler::initialize(
    __in LPCTSTR dump_file_name,
    __in const MINIDUMP_TYPE dump_type
) {

    DWORD error = ERROR_SUCCESS;

    do {

        if (nullptr == dump_file_name) {
            error = ERROR_INVALID_PARAMETER;
            break;
        }

        _dump_file_name.assign(dump_file_name);
        _dump_type = dump_type;

    } while (false);

    return error;
}

DWORD exception_handler::run() {
    _prev_filter = ::SetUnhandledExceptionFilter(exception_callback);
    return ERROR_SUCCESS;
}

LONG exception_handler::exception_callback(
    __in struct _EXCEPTION_POINTERS* exceptioninfo
) {

    do {

        if (nullptr == exceptioninfo) {
            break;
        }

        SYSTEMTIME st = { 0 };
        ::GetLocalTime(&st);

        std::string dump_file_name;
        dump_file_name.assign(exception_handler::instance()->_dump_file_name);

        //
        // create dump file.
        //

        HANDLE dump_file_handle = ::CreateFile(
            CString(dump_file_name.c_str()),
            GENERIC_WRITE,
            0,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );

        if (INVALID_HANDLE_VALUE == dump_file_handle) {
            break;
        }

        MINIDUMP_EXCEPTION_INFORMATION ex_info = { 0 };

        ex_info.ThreadId = ::GetCurrentThreadId(); // Threae ID ????
        ex_info.ExceptionPointers = exceptioninfo; // Exception ???? ????
        ex_info.ClientPointers = FALSE;

        //
        // write dump file.
        //

        if (FALSE == ::MiniDumpWriteDump(
            ::GetCurrentProcess(),
            ::GetCurrentProcessId(),
            dump_file_handle,
            exception_handler::instance()->_dump_type,
            &ex_info,
            nullptr, nullptr
        )) {

            break;
        }

    } while (false);

    return (exception_handler::instance()->_prev_filter) ? exception_handler::instance()->_prev_filter(exceptioninfo) : EXCEPTION_EXECUTE_HANDLER;
}


// CPnxCyclerApp

BEGIN_MESSAGE_MAP(CPnxCyclerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPnxCyclerApp construction

CPnxCyclerApp::CPnxCyclerApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	GdiplusStartupInput gdiplus;

	if (GdiplusStartup(&m_gpToken, &gdiplus, NULL) != Ok)
	{
		return;
	}

	if (!AfxSocketInit())
		return;
}


// The one and only CPnxCyclerApp object

CPnxCyclerApp theApp;

// CPnxCyclerApp initialization

BOOL CPnxCyclerApp::InitInstance()
{
    exception_handler::instance()->initialize("handler.dmp");
    exception_handler::instance()->run();

	HANDLE hMutex = NULL;
	hMutex = CreateMutex(
		NULL,
		TRUE,
		_T("CYCLER_RUNNGING"));

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{

		if (killProcess() != TRUE)
		{
			//AfxMessageBox(_T("프로그램이 이미 실행 중 입니다."));
			SYSLOG(Dbg, _T("Fail!! Running return"));
			return FALSE;
		}
		InitInstance();
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	//CPnxCyclerDlg dlg;			// PnxCyclerDlg
	CPnxFormationDlg dlg;			// FORMATION

	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CPnxCyclerApp::killProcess()
{
	HANDLE         hProcessSnap = NULL;
	DWORD          Return = FALSE;
	PROCESSENTRY32 pe32 = { 0 };

	CString ProcessName = "PnxFormation.exe";
	ProcessName.MakeLower();


	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return (DWORD)INVALID_HANDLE_VALUE;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32))
	{
		DWORD Code = 0;
		DWORD dwPriorityClass;

		do {
			HANDLE hProcess;
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			dwPriorityClass = GetPriorityClass(hProcess);

			CString Temp = pe32.szExeFile;
			Temp.MakeLower();

			if (Temp == ProcessName)
			{
				if (TerminateProcess(hProcess, 0))
					GetExitCodeProcess(hProcess, &Code);
				else
					return GetLastError();
			}
			CloseHandle(hProcess);
		} while (Process32Next(hProcessSnap, &pe32));
		Return = TRUE;
	}
	else
		Return = FALSE;

	CloseHandle(hProcessSnap);

	return Return;
}



int CPnxCyclerApp::ExitInstance()
{
	GdiplusShutdown(m_gpToken);

	return CWinApp::ExitInstance();
}
