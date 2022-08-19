
// PnxCyclerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "PnxCyclerDlg.h"
#include "afxdialogex.h"

#include "MgrSystemLog.h"

#include "PnxUtil.h"
#include "ComProc.h"
#include "SockProc.h"

#include "MgrConfig.h"
#include "MgrChannel.h"
#include "MgrAlarm.h"
#include "MgrLog.h"
#include "MgrComm.h"
#include "MgrCommData.h"
#include "MgrEthernet.h"
#include "MgrSerial.h"
#include "MgrDAQ.h"
#include "MgrChannel.h"

#include "DlgMainTitle.h"
#include "DlgViewPattern.h"
//#include "DlgViewChannelCell.h"
#include "DlgViewChannelPack.h"
#include "DlgViewAlarm.h"
#include "DlgViewDAQ.h"
#include "DlgSystemSafety.h"
#include "DlgViewChamberAlarm.h"

#include "CDlgViewSchedulelist.h"
#include "CDlgViewScheduleInfo.h"
#include "CDlgEditScheduleInfo.h"
#include "CDlgViewLogIn.h"
#include "CDlgEditUser.h"
#include "CDlgEditChannelView.h"
#include "CDlgNextPause.h"
#include "CDlgNextComplete.h"
#include "CDlgNextStep.h"
#include "CDlgViewMaterial.h"
#include "CDlgReserveCancel.h"
#include "CDlgEditCycler.h"
#include "CDlgEditMultiSchedule.h"
#include "CDlgViewChamber.h"

#include "CMgrModel.h"
#include "CMgrAccount.h"
#include "CMgrScheduleHistory.h"
#include "CMgrMaterial.h"
#include "CMgrChamber.h"
#include "MgrStatus.h"
#include "MgrOption.h"
#include "MgrDio.h"

#pragma warning(disable:4100)

#define LogReset			(8888)
#define UsageEquipTime		(7989)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CPnxCyclerDlg dialog
CPnxCyclerDlg::CPnxCyclerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPnxCyclerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nDialogWidth = 0;	
}

void CPnxCyclerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPnxCyclerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(ID_MENU_MAIN_CMD_START_KOR, ID_CANCEL_SYNC, OnClickCommand)
	ON_COMMAND_RANGE(100, 101, OnReconnect)
	ON_MESSAGE(WM_PROGRAM_EXIT, OnReceiveProgramExit)
	ON_MESSAGE(WM_UPDATE_VIEW_ALARM, OnReceiveAlarm)
	ON_MESSAGE(WM_UPDATE_VIEW_ALARM_STATUS, OnReceiveAlarmUpdate)
	ON_MESSAGE(WM_DAQ_SAFETY_WARNING, OnReceiveDAQSafety)
	ON_MESSAGE(WM_DAQ_COMPLETE_WARNING, OnReceiveDAQComplete)
	ON_MESSAGE(WM_NEXT_PAUSE_RESERVE, OnReceiveReservePause)
	ON_MESSAGE(WM_NEXT_COMPLETE_RESERVE, OnReceiveReserveComplete)
	ON_MESSAGE(WM_NEXT_STEP_RESERVE, OnReceiveReserveStep)
	ON_MESSAGE(WM_HIDE_SCHEDULE_GUI, OnReceiveHideScheduleGui)
	ON_MESSAGE(WM_MULTI_SCHEDULE_START, OnReceiveMultiScheduleStart)
	ON_MESSAGE(WM_UPDATE_DAQ_VIEW, OnReceiveDaQView)
	ON_MESSAGE(WM_VIEW_PATTERN, OnReceiveSelectChannel)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPnxCyclerDlg message handlers
BOOL CPnxCyclerDlg::PreTranslateMessage(MSG* pMsg)
{

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CPnxCyclerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetMonitorResolution();

	CreateUserDirectory();

	CreateUserClass();

	InitUI();

	InitMenu();

	SetTimer(UsageEquipTime, 1000, NULL);

	//파일 지우기
	OpenDeleteMainLog();

	//하루에 1 번? 86400000ms
	SetTimer(LogReset, 86400000, NULL);

	PostMessage(WM_UPDATE_DAQ_VIEW, 0, 0);

	//프로그램 시작 시 자동 로그인..
	CMgrAccount* pMgr = CMgrAccount::GetMgr();
	//CString m_strLoginID = pMgr->GetLoginID();
	//CString m_strLoginPW = pMgr->GetLoginPW();
	//sAccount* lpAccount = pMgr->LogInAccount(m_strLoginID, m_strLoginPW);
	CDlgMainTitle* pDlgMainTitle = CDlgMainTitle::GetMgr();
	pDlgMainTitle->SetLoginStatus(pMgr->GetLoginedAccount());
	//

	//시작 시 I/O 읽어서 직렬, 병렬 구분하여서 Mode change
	m_fnInitEqMode();
	//

	CMgrDio::GetMgr()->SetDIO(17, TRUE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPnxCyclerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPnxCyclerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPnxCyclerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	KillTimer(LogReset);
	KillTimer(UsageEquipTime);

	CMgrDio::GetMgr()->SetDIO(16, FALSE);
	CMgrDio::GetMgr()->SetDIO(17, FALSE);
	CMgrDio::GetMgr()->SetDIO(18, FALSE);
	CMgrDio::GetMgr()->SetDIO(19, FALSE);

	CMgrDio::GetMgr()->StopThread();
	DestroyUserClass();

	CloseMenu();
}

BOOL CPnxCyclerDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;

	GetClientRect(&rt);

	pDC->FillSolidRect(&rt, ColorWhite);

	return TRUE;
}

void	CPnxCyclerDlg::SetMonitorResolution()
{
	// 작업 표시줄의 Size 를 구함.
	HWND hWorkDisplay = ::FindWindow(_T("Shell_TrayWnd"), _T(""));

	int nWorkDisplayHeight = 0;

	if (hWorkDisplay)
	{
		CRect rt;

		::GetWindowRect(hWorkDisplay, &rt);

		nWorkDisplayHeight = rt.bottom - rt.top;
	}

	m_nDialogWidth = GetSystemMetrics(SM_CXSCREEN);

	SetWindowPos(NULL, 0, 0, m_nDialogWidth, GetSystemMetrics(SM_CYSCREEN) - nWorkDisplayHeight, SWP_NOZORDER);
}

void	CPnxCyclerDlg::InitUI()
{
	//// Dialog 의 Size 를 미리 Define 하지 말고, 모니터 해상도에 따라서 뿌리도록 해보자.
	//CRect rtTitle;
	//CRect rtWindow;

	//CDlgMainTitle* pDlgMainTitle = CDlgMainTitle::GetMgr();

	//pDlgMainTitle->GetClientRect(&rtTitle);
	//pDlgMainTitle->SetWindowPos(&CWnd::wndTop, 0, 0, rtTitle.Width(), rtTitle.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	//ScreenToClient(&rtTitle);

	//CRect rtViewChannel;
	//{
	//	CDlgViewChannelPack* pDlgViewChannel = CDlgViewChannelPack::GetMgr();

	//	pDlgViewChannel->GetClientRect(&rtViewChannel);

	//	ScreenToClient(&rtViewChannel);

	//	pDlgViewChannel->SetWindowPos(&CWnd::wndTop, 0, rtTitle.bottom + 40, rtViewChannel.Width(), rtViewChannel.Height()/*-244*/, SWP_NOZORDER | SWP_SHOWWINDOW);
	//	pDlgViewChannel->GetWindowRect(&rtWindow);

	//	ScreenToClient(&rtWindow);
	//}

	//CDlgViewDAQ* lpViewDaQ = CDlgViewDAQ::GetMgr();

	//if(lpViewDaQ)
	//	lpViewDaQ->SetWindowPos(&CWnd::wndTop, 0, rtWindow.bottom + 10, rtTitle.Width() / 2, 235, SWP_NOZORDER | SWP_SHOWWINDOW);

	//CDlgViewChamber* lpViewChamber = CDlgViewChamber::GetMgr();

	//if(lpViewChamber)
	//	lpViewChamber->SetWindowPos(&CWnd::wndTop, rtTitle.Width() / 2, rtWindow.bottom + 10, rtTitle.Width(), 235, SWP_NOZORDER | SWP_SHOWWINDOW);

	//CString strPGMTitle;
	//char D_String[15] = { 0, };

	//char* Date[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	//CString str;
	//sprintf_s(D_String, "%s", __DATE__);
	//for (int i = 12; i; i--)
	//{
	//	for (int j = 3; j; j--)
	//	{
	//		if (D_String[j - 1] != *(Date[i - 1] + (j - 1)))
	//			break;
	//		if (j == 1)
	//		{
	//			if (D_String[4] == 0x20)	D_String[4] = 0x30;
	//			//sprintf_s(Date_String, "%c%c%c%c/%02d/%c%c", D_String[7], D_String[8], D_String[9], D_String[10], i, D_String[4], D_String[5]);
	//			
	//			str.Format(_T("%c%c%c%c/%02d/%c%c"), D_String[7], D_String[8], D_String[9], D_String[10], i, D_String[4], D_String[5]);
	//			i = 1; j = 1;
	//			break;
	//		}
	//	}
	//}
	//strPGMTitle.Format(_T("SK Innovation PNX Cell Cycler %s [V 1.1.1]"), (LPCSTR)str);
	//SetWindowText(strPGMTitle);

	CRect rtParent;  // 부모 윈도우 크기
	this->GetClientRect(&rtParent);

	CRect rtClient;  // ctrl 윈도우 크기
	CRect rtWindow;  // ctrl 윈도우를 부모 윈도우 기준으로의 위치


	// Dialog 의 Size 를 미리 Define 하지 말고, 모니터 해상도에 따라서 뿌리도록 해보자.
	//CRect rtTitle;
	//CRect rtWindow;

	CDlgMainTitle* pDlgMainTitle = CDlgMainTitle::GetMgr();

	pDlgMainTitle->GetClientRect(&rtClient);
	pDlgMainTitle->SetWindowPos(&CWnd::wndTop, 0, 0, rtParent.Width(), rtClient.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);
	pDlgMainTitle->GetWindowRect(&rtWindow);
	ScreenToClient(&rtWindow);


	CDlgViewChannelPack* pDlgViewChannel = CDlgViewChannelPack::GetMgr();

	pDlgViewChannel->GetClientRect(&rtClient);
	pDlgViewChannel->SetWindowPos(&CWnd::wndTop, 0, rtWindow.bottom, rtParent.Width(), rtClient.Height(), SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
	pDlgViewChannel->GetWindowRect(&rtWindow);
	ScreenToClient(&rtWindow);

	int nBottom = rtWindow.bottom;

	// 동일 x 축에 2개 client가 들어가야 하므로
	int nCyclerType = CMgrConfig::GetMgr()->GetCyclerType();
	CDlgViewDAQ* pDlgViewDAQ = CDlgViewDAQ::GetMgr();

	if (pDlgViewDAQ)
	{
		pDlgViewDAQ->GetClientRect(&rtClient);
		pDlgViewDAQ->SetWindowPos(&CWnd::wndTop, 0, rtWindow.bottom, rtParent.Width() / 2, rtClient.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);
		pDlgViewDAQ->GetWindowRect(&rtWindow);
		ScreenToClient(&rtWindow);
	}

	CDlgViewPattern* pDlgViewPattern = CDlgViewPattern::GetMgr();
	if (pDlgViewPattern)
	{
		CRect rcPattern;
		pDlgViewPattern->GetClientRect(&rcPattern);
		if (CyclerTypeCell != CMgrConfig::GetMgr()->GetCyclerType())
			pDlgViewPattern->SetWindowPos(&CWnd::wndTop, 0, rtWindow.bottom, rtParent.Width() - rtWindow.right, rtParent.Height() - rcPattern.Height() - 8, SWP_NOZORDER |  SWP_SHOWWINDOW);
		else
			pDlgViewPattern->SetWindowPos(&CWnd::wndTop, 0, rtWindow.bottom, rtParent.Width(), rtParent.Height() - rtWindow.bottom - 8, SWP_NOZORDER | SWP_SHOWWINDOW);
		pDlgViewPattern->GetWindowRect(&rtWindow);
		ScreenToClient(&rtWindow);
	}

	// Title 추가
	SetWindowText(CMgrConfig::GetMgr()->GetVersionStr());
}

void	CPnxCyclerDlg::InitMenu()
{
	// 내가 생각해도 소스 그지같다.. 시간 없으니 나중에 정리..
	for (int i = 0; i < BitmapFileMaxCount; ++i)
		m_bitmapFile[i].LoadBitmap(IDB_BITMAP_NEW + i);

	for (int i = 0; i < BitmapEditMaxCount; ++i)
		m_bitmapEdit[i].LoadBitmap(IDB_BITMAP_CANCEL + i);

	for (int i = 0; i < BitmapPauseMaxCount; ++i)
		m_bitmapPause[i].LoadBitmap(IDB_BITMAP_PAUSE_OP1 + i);

	for (int i = 0; i < BitmapCompMaxCount; ++i)
		m_bitmapComp[i].LoadBitmap(IDB_BITMAP_COMPLETE_OP_1 + i);

	for (int i = 0; i < BitmapNextMaxCount; ++i)
		m_bitmapNext[i].LoadBitmap(IDB_BITMAP_NEXT_OP1 + i);

	for (int i = 0; i < BitmapControlMaxCount ; ++i)
		m_bitmapControl[i].LoadBitmap(IDB_BITMAP_START + i);

	for (int i = 0; i < BitmapSetMaxCount; ++i)
		m_bitmapSetting[i].LoadBitmap(IDB_BITMAP_SAFETY + i);

	for (int i = 0; i < BitmapViewMaxCount; ++i)
		m_bitmapView[i].LoadBitmap(IDB_BITMAP_LOG + i);

	{
		m_Menu.CreateMenu();

		//CMenu* pSub1 = new CMenu();
		//pSub1->CreatePopupMenu();
		//{
		//	pSub1->AppendMenu(MF_STRING, ID_MENU_MAIN_NEW_KOR, _T("새로 만들기(&N)"));
		//	pSub1->AppendMenu(MF_STRING, ID_MENU_MAIN_OPEN_KOR, _T("열기(&O).."));
		//	pSub1->AppendMenu(MF_STRING, ID_MENU_MAIN_SAVE_KOR, _T("저장(&S)"));
		//	pSub1->AppendMenu(MF_STRING, ID_MENU_MAIN_SAVEAS_KOR, _T("다른 이름으로 저장(&A)..."));
		//	pSub1->AppendMenu(MF_STRING, ID_MENU_MAIN_RECENT_KOR, _T("최근에 사용한 파일"));
		//	pSub1->AppendMenu(MF_STRING, ID_MENU_MAIN_EXIT_KOR, _T("종료(&X)"));

		//	for (int i = 0; i < BitmapFileMaxCount; ++i)
		//		pSub1->SetMenuItemBitmaps(ID_MENU_MAIN_NEW_KOR + i, MF_BYCOMMAND, &m_bitmapFile[i], &m_bitmapFile[i]); //test
		//}

		//CMenu* pSub2 = new CMenu();
		//pSub2->CreatePopupMenu();
		//{
		//	pSub2->AppendMenu(MF_STRING, ID_MENU_MAIN_UNDO_KOR, _T("실행취소(&U)"));
		//	pSub2->AppendMenu(MF_STRING, ID_MENU_MAIN_CUT_KOR, _T("잘라내기(&T)"));
		//	pSub2->AppendMenu(MF_STRING, ID_MENU_MAIN_COPY_KOR, _T("복사(&C)"));
		//	pSub2->AppendMenu(MF_STRING, ID_MENU_MAIN_PASTE_KOR, _T("붙여넣기(&P)"));

		//	for (int i = 0; i < BitmapEditMaxCount; ++i)
		//		pSub2->SetMenuItemBitmaps(ID_MENU_MAIN_UNDO_KOR + i, MF_BYCOMMAND, &m_bitmapEdit[i], &m_bitmapEdit[i]); //test
		//}

		CMenu* pSubMenu1 = new CMenu();
		pSubMenu1->CreatePopupMenu();

		pSubMenu1->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_PAUSE_1_KOR, _T("Immediately stop"));
		pSubMenu1->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_PAUSE_2_KOR, _T("After completion of the current Step"));
		pSubMenu1->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_PAUSE_3_KOR, _T("After completion of the current Cycle"));
		pSubMenu1->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_PAUSE_4_KOR, _T("Etc"));

		for (int i = 0; i < BitmapPauseMaxCount; ++i)
			pSubMenu1->SetMenuItemBitmaps(ID_MENU_MAIN_CMD_PAUSE_1_KOR + i, MF_BYCOMMAND, &m_bitmapPause[i], &m_bitmapPause[i]);

		CMenu* pSubMenu2 = new CMenu();
		pSubMenu2->CreatePopupMenu();

		pSubMenu2->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_COMPLETE_1_KOR, _T("Immediately Complete"));
		pSubMenu2->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_COMPLETE_2_KOR, _T("After completion of the current Step"));
		pSubMenu2->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_COMPLETE_3_KOR, _T("After completion of the current Cycle"));
		pSubMenu2->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_COMPLETE_4_KOR, _T("Etc"));

		for (int i = 0; i < BitmapCompMaxCount; ++i)
			pSubMenu2->SetMenuItemBitmaps(ID_MENU_MAIN_CMD_COMPLETE_1_KOR + i, MF_BYCOMMAND, &m_bitmapComp[i], &m_bitmapComp[i]);

		CMenu* pSubMenu3 = new CMenu();
		pSubMenu3->CreatePopupMenu();
		{
			pSubMenu3->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_NEXT_1_KOR, _T("Next Step"));
			pSubMenu3->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_NEXT_2_KOR, _T("Etc"));

			for (int i = 0; i < BitmapNextMaxCount; ++i)
				pSubMenu3->SetMenuItemBitmaps(ID_MENU_MAIN_CMD_NEXT_1_KOR + i, MF_BYCOMMAND, &m_bitmapNext[i], &m_bitmapNext[i]); //test
		}


		CMenu* pSub3 = new CMenu();
		pSub3->CreatePopupMenu();

		pSub3->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_START_KOR, _T("Process Start"));
		pSub3->InsertMenuA(1, MF_BYPOSITION | MF_POPUP, (UINT)pSubMenu1->m_hMenu, _T("Process Stop"));
		pSub3->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_CONTINUE_KOR, _T("Continue"));
		pSub3->InsertMenuA(3, MF_BYPOSITION | MF_POPUP, (UINT)pSubMenu3->m_hMenu, _T("Next Step"));
		pSub3->InsertMenuA(4, MF_BYPOSITION | MF_POPUP, (UINT)pSubMenu2->m_hMenu, _T("Process Complete"));
		pSub3->AppendMenu(MF_STRING, ID_CANCEL_SYNC, _T("Sync cancel"));
		pSub3->AppendMenu(MF_STRING, ID_MENU_MAIN_CMD_ALARM_CLEAR_KOR, _T("Error Clear"));
		pSub3->AppendMenu(MF_STRING, ID_COMM_RECONNECT, _T("Communication reconnection"));
		pSub3->AppendMenu(MF_STRING, ID_ERROR_CHANNEL_RESTART, _T("Abnormally terminated channel restart"));		
		//pSub3->AppendMenu(MF_STRING, ID_MENU_MAIN_VIEW_PATTERN_KOR, _T("패턴 정보"));		
		pSub3->AppendMenu(MF_STRING, ID_MENU_MAIN_ANALYZETOOL_SIMPLE_KOR, _T("General analysis"));
		pSub3->AppendMenu(MF_STRING, ID_MENU_MAIN_ANALYZETOOL_DETAILE_KOR, _T("Detailed analysis"));
		//pSub3->AppendMenu(MF_STRING, ID_MENU_MAIN_VIEW_RESERVE_KOR, _T("예약 정지/종료 보기"));
		pSub3->AppendMenu(MF_STRING, ID_MENU_MAIN_VIEW_RESERVE_SCHEDULE_KOR, _T("Reservation Schedule Edit"));
		pSub3->AppendMenu(MF_STRING, ID_MENU_MAIN_VIEW_SCHEDULE_APPEND_KOR, _T("Reservation Schedule add"));
		pSub3->AppendMenu(MF_STRING, ID_MENU_MAIN_SET_MODE_CHANGE_SINGLE, _T("Single Mode"));
		pSub3->AppendMenu(MF_STRING, ID_MENU_MAIN_SET_MODE_CHANGE_PARALLEL, _T("Parallel Mode"));

		pSub3->SetMenuItemBitmaps(ID_MENU_MAIN_CMD_START_KOR, MF_BYCOMMAND, &m_bitmapControl[0], &m_bitmapControl[0]);
		pSub3->SetMenuItemBitmaps(ID_MENU_MAIN_CMD_CONTINUE_KOR, MF_BYCOMMAND, &m_bitmapControl[1], &m_bitmapControl[1]);
		pSub3->SetMenuItemBitmaps(ID_MENU_MAIN_CMD_ALARM_CLEAR_KOR, MF_BYCOMMAND, &m_bitmapControl[2], &m_bitmapControl[2]);
		//pSub3->SetMenuItemBitmaps(ID_MENU_MAIN_VIEW_PATTERN_KOR, MF_BYCOMMAND, &m_bitmapControl[3], &m_bitmapControl[3]);
		pSub3->SetMenuItemBitmaps(ID_MENU_MAIN_ANALYZETOOL_SIMPLE_KOR, MF_BYCOMMAND, &m_bitmapControl[4], &m_bitmapControl[4]);
		pSub3->SetMenuItemBitmaps(ID_MENU_MAIN_ANALYZETOOL_DETAILE_KOR, MF_BYCOMMAND, &m_bitmapControl[5], &m_bitmapControl[5]);
		//pSub3->SetMenuItemBitmaps(ID_MENU_MAIN_VIEW_RESERVE_KOR, MF_BYCOMMAND, &m_bitmapControl[6], &m_bitmapControl[6]);
		pSub3->SetMenuItemBitmaps(ID_MENU_MAIN_VIEW_RESERVE_SCHEDULE_KOR, MF_BYCOMMAND, &m_bitmapControl[7], &m_bitmapControl[7]);
		pSub3->SetMenuItemBitmaps(ID_MENU_MAIN_VIEW_SCHEDULE_APPEND_KOR, MF_BYCOMMAND, &m_bitmapControl[8], &m_bitmapControl[8]);
		//	pSub3->SetMenuItemBitmaps(ID_MENU_MAIN_SET_MODE_CHANGE_KOR, MF_BYCOMMAND, &m_bitmapControl[9], &m_bitmapControl[9]);

		for (int i = 0; i < BitmapPauseMaxCount; ++i)
			pSubMenu1->SetMenuItemBitmaps(ID_MENU_MAIN_CMD_PAUSE_1_KOR + i, MF_BYCOMMAND, &m_bitmapPause[i], &m_bitmapPause[i]); //test

		for (int i = 0; i < BitmapCompMaxCount; ++i)
			pSubMenu2->SetMenuItemBitmaps(ID_MENU_MAIN_CMD_COMPLETE_1_KOR + i, MF_BYCOMMAND, &m_bitmapComp[i], &m_bitmapComp[i]); //test

		for (int i = 0; i < BitmapNextMaxCount; ++i)
			pSubMenu3->SetMenuItemBitmaps(ID_MENU_MAIN_CMD_NEXT_1_KOR + i, MF_BYCOMMAND, &m_bitmapNext[i], &m_bitmapNext[i]); //test

		CMenu* pSub4 = new CMenu();
		pSub4->CreatePopupMenu();

		pSub4->AppendMenu(MF_STRING, ID_MENU_MAIN_SET_SAFETY_CONDITION_KOR, "Safety Cond Setting");
		pSub4->AppendMenu(MF_STRING, ID_MENU_MAIN_VIEW_DAQ_SETTING, "DAQ MAP Edit");
		pSub4->AppendMenu(MF_STRING, ID_MENU_MAIN_VIEW_DAQ_LOAD, "DAQ MAP Load");

		for (int i = 0; i < BitmapSetMaxCount; ++i)
			pSub4->SetMenuItemBitmaps(ID_MENU_MAIN_SET_SAFETY_CONDITION_KOR + i, MF_BYCOMMAND, &m_bitmapSetting[i], &m_bitmapSetting[i]);

		CMenu* pSub5 = new CMenu();
		pSub5->CreatePopupMenu();

		pSub5->AppendMenu(MF_STRING, ID_MENU_MAIN_VIEW_LOG_KOR, "Log");
		pSub5->AppendMenu(MF_STRING, ID_MENU_MAIN_VIEW_ALARM_KOR, "Alarm Info");
		pSub5->AppendMenu(MF_STRING, ID_MENU_MAIN_VIEW_RELAY_KOR, _T("View material management"));

		for (int i = 0; i < BitmapViewMaxCount; ++i)
			pSub5->SetMenuItemBitmaps(ID_MENU_MAIN_VIEW_LOG_KOR + i, MF_BYCOMMAND, &m_bitmapView[i], &m_bitmapView[i]);

		//	m_Menu.InsertMenu(0, MF_BYPOSITION | MF_POPUP, (UINT)pSub1->m_hMenu, _T("파일(&F)"));
		//	m_Menu.InsertMenu(1, MF_BYPOSITION | MF_POPUP, (UINT)pSub2->m_hMenu, _T("편집(&E)"));
		m_Menu.InsertMenu(0, MF_BYPOSITION | MF_POPUP, (UINT)pSub3->m_hMenu, _T("Control command(&C)"));
		m_Menu.InsertMenu(1, MF_BYPOSITION | MF_POPUP, (UINT)pSub4->m_hMenu, _T("Setting(&S)"));
		m_Menu.InsertMenu(2, MF_BYPOSITION | MF_POPUP, (UINT)pSub5->m_hMenu, _T("View(&V)"));

		SetMenu(&m_Menu);
	}

}

void	CPnxCyclerDlg::CloseMenu()
{
	CMenu* pMenu = GetMenu();

	if (pMenu)
	{
		int nCount = pMenu->GetMenuItemCount();

		for (int i = 0; i < nCount; ++i)
		{
			CMenu* pSub = pMenu->GetSubMenu(i);

			if (pSub)
			{
				int nSubCount = pSub->GetMenuItemCount();

				for (int j = 0; j < nSubCount; ++j)
				{
					CMenu* pSubMenu = pSub->GetSubMenu(j);

					DELETE_POINTER(pSubMenu);
				}

				DELETE_POINTER(pSub);
			}
		}
	}
}

void	CPnxCyclerDlg::CreateUserClass()
{
	CMgrSystemLog::CreateClass();

	CPnxUtil::CreateClass();

	CMgrConfig::CreateClass();

	CMgrOption::CreateClass();

	CMgrMaterial::CreateClass();

	CMgrAccount::CreateClass();
	CMgrAccount::GetMgr()->LogInAccount("Admin", "1");

	CMgrComm::CreateClass();

	CMgrChannel::CreateClass();

	CMgrStatus::CreateClass();

	CMgrConfig::GetMgr()->GetChannelRunningTotal();

	CMgrChamber::CreateClass();

	CMgrDAQ::CreateClass();

	CMgrScheduleHistory::CreateClass();

	CMgrAlarm::CreateClass();

	CMgrCommData::CreateClass();

	CMgrConfig::GetMgr()->GetChannelContactResistance();
	CMgrLog::CreateClass();

	CMgrSerial::CreateClass();
	CMgrSerial::GetMgr()->StartThread();

	CComProc::CreateClass();

	CMgrEthernet::CreateClass();
	CMgrEthernet::GetMgr()->StartThread();

	CMgrModel::CreateClass();

	CSockProc::CreateClass();

	CDlgViewChamberAlarm::CreateClass();
	CDlgViewChamberAlarm::GetMgr()->Create(this);

	CDlgMainTitle::CreateClass();
	CDlgMainTitle::GetMgr()->Create(this);
	CDlgMainTitle::GetMgr()->ShowWindow(SW_SHOW);

	CDlgViewChannelPack::CreateClass();
	CDlgViewChannelPack::GetMgr()->Create(this);
	CDlgViewChannelPack::GetMgr()->ShowWindow(SW_SHOW);


	CDlgViewAlarm::CreateClass();
	CDlgViewAlarm::GetMgr()->Create(this);

	CDlgSystemSafety::CreateClass();
	CDlgSystemSafety::GetMgr()->Create(this);

	CDlgViewDAQ::CreateClass();
	CDlgViewDAQ::GetMgr()->Create(this);

	CDlgViewSchedulelist::CreateClass();
	CDlgViewSchedulelist::GetMgr()->Create(this);

	CDlgViewScheduleInfo::CreateClass();
	CDlgViewScheduleInfo::GetMgr()->Create(this);

	CDlgEditScheduleInfo::CreateClass();
	CDlgEditScheduleInfo::GetMgr()->Create(this);

	CDlgViewLogIn::CreateClass();
	CDlgViewLogIn::GetMgr()->Create(this);

	CDlgEditUser::CreateClass();
	CDlgEditUser::GetMgr()->Create(this);

	CDlgEditChannelView::CreateClass();
	CDlgEditChannelView::GetMgr()->Create(this);

	CDlgNextPause::CreateClass();
	CDlgNextPause::GetMgr()->Create(this);

	CDlgNextComplete::CreateClass();
	CDlgNextComplete::GetMgr()->Create(this);

	CDlgNextStep::CreateClass();
	CDlgNextStep::GetMgr()->Create(this);

	CDlgViewMaterial::CreateClass();
	CDlgViewMaterial::GetMgr()->Create(this);

	CDlgReserveCancel::CreateClass();
	CDlgReserveCancel::GetMgr()->Create(this);

	CDlgViewPattern::CreateClass();
	CDlgViewPattern::GetMgr()->Create(this);

	CDlgEditMultiSchedule::CreateClass();
	CDlgEditMultiSchedule::GetMgr()->Create(this);

	CDlgViewChamber::CreateClass();
	CDlgViewChamber::GetMgr()->Create(this);

	//CMgrDAQ::GetMgr()->LoadDAQCell();

	CString strDaqPath = CMgrConfig::GetMgr()->GetDAQFilePath();

	if (strDaqPath.IsEmpty() == false)
	{
		CFileFind findfile;
		BOOL bFindFile = findfile.FindFile(strDaqPath);
		if (bFindFile == TRUE)
		{
			//		CMgrDAQ::GetMgr()->setDapFilePath(CMgrConfig::GetMgr()->GetDAQFilePath());
			CMgrDAQ::GetMgr()->LoadDAQ(CMgrConfig::GetMgr()->GetDAQFilePath());
		}
	}


	CSockProc::GetMgr()->SocketConnect();

	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	//pnx_hr 20210510 DIO Make 추가
	m_strDioType = CMgrConfig::GetMgr()->m_strDioType;

	CMgrDio::CreateClass();
	CMgrDio::GetMgr()->m_fnDioOpen(m_strDioType);

	if (pMgrConfig)
	{
		if (true == pMgrConfig->IsErrorSystemSafety())
		{
			CDlgSystemSafety* lpDlg = CDlgSystemSafety::GetMgr();

			if (lpDlg)
				lpDlg->ShowWindow(SW_SHOW);
		}

		if (true == pMgrConfig->IsErrorFilePath())
		{
			CDlgEditCycler dlg;

			dlg.DoModal();
		}
	}


}

void	CPnxCyclerDlg::DestroyUserClass()
{
	// Thread 는 무조건 가장 먼저 종료시킨다.
	CMgrAlarm::GetMgr()->StopThread();

	CComProc::GetMgr()->StopThread();

	CMgrSerial::GetMgr()->StopThread();

	CSockProc::GetMgr()->StopThread();

	CMgrEthernet::GetMgr()->StopThread();

	CMgrLog::DestroyClass();

	CDlgViewChannelPack::GetMgr()->DestroyWindow();
	CDlgViewChannelPack::DestroyClass();

	CDlgMainTitle::GetMgr()->DestroyWindow();
	CDlgMainTitle::DestroyClass();

	CDlgViewAlarm::GetMgr()->DestroyWindow();
	CDlgViewAlarm::DestroyClass();

	CDlgSystemSafety::GetMgr()->DestroyWindow();
	CDlgSystemSafety::DestroyClass();

	CDlgViewDAQ::GetMgr()->DestroyWindow();
	CDlgViewDAQ::DestroyClass();

	CDlgViewChamberAlarm::GetMgr()->DestroyWindow();
	CDlgViewChamberAlarm::DestroyClass();

	CDlgViewScheduleInfo::GetMgr()->DestroyWindow();
	CDlgViewScheduleInfo::DestroyClass();

	CDlgViewSchedulelist::GetMgr()->DestroyWindow();
	CDlgViewSchedulelist::DestroyClass();

	CDlgEditScheduleInfo::GetMgr()->DestroyWindow();
	CDlgEditScheduleInfo::DestroyClass();

	CDlgEditUser::GetMgr()->DestroyWindow();
	CDlgEditUser::DestroyClass();

	CDlgViewLogIn::GetMgr()->DestroyWindow();
	CDlgViewLogIn::DestroyClass();

	CDlgEditChannelView::GetMgr()->DestroyWindow();
	CDlgEditChannelView::DestroyClass();

	CDlgNextPause::GetMgr()->DestroyWindow();
	CDlgNextPause::DestroyClass();

	CDlgNextComplete::GetMgr()->DestroyWindow();
	CDlgNextComplete::DestroyClass();

	CDlgNextStep::GetMgr()->DestroyWindow();
	CDlgNextStep::DestroyClass();

	CDlgViewMaterial::GetMgr()->DestroyWindow();
	CDlgViewMaterial::DestroyClass();

	CDlgReserveCancel::GetMgr()->DestroyWindow();
	CDlgReserveCancel::DestroyClass();

	CDlgViewPattern::GetMgr()->DestroyWindow();
	CDlgViewPattern::DestroyClass();

	CDlgEditMultiSchedule::GetMgr()->DestroyWindow();
	CDlgEditMultiSchedule::DestroyClass();

	CDlgViewChamber::GetMgr()->DestroyWindow();
	CDlgViewChamber::DestroyClass();

	CMgrSerial::DestroyClass();

	CMgrEthernet::DestroyClass();

	CSockProc::DestroyClass();

	CComProc::DestroyClass();

	CMgrAccount::DestroyClass();

	CMgrConfig::DestroyClass();

	CMgrScheduleHistory::DestroyClass();

	CMgrChannel::DestroyClass();

	CMgrStatus::DestroyClass();

	CMgrDAQ::DestroyClass();

	CMgrCommData::DestroyClass();

	CMgrComm::DestroyClass();

	CMgrAlarm::DestroyClass();

	CMgrModel::DestroyClass();

	CMgrMaterial::DestroyClass();

	CMgrOption::DestroyClass();

	CMgrChamber::DestroyClass();

	CPnxUtil::DestroyClass();
	CMgrSystemLog::DestroyClass();//<!--20210507 add by swseo-->
}

void	CPnxCyclerDlg::CreateUserDirectory()
{
	CString str;

	str.Format(_T("%s\\Config\\"), (LPCSTR)GetExePath());
	CreateDirectory(str, NULL);

	str.Format(_T("%s\\MainLog\\"), (LPCSTR)GetExePath());
	CreateDirectory(str, NULL);

	str.Format(_T("%s\\AlarmHistory\\"), (LPCSTR)GetExePath());
	CreateDirectory(str, NULL);

	str.Format(_T("%s\\UserHistory\\"), (LPCSTR)GetExePath());
	CreateDirectory(str, NULL);

	str.Format(_T("%s\\Material\\"), (LPCSTR)GetExePath());
	CreateDirectory(str, NULL);
}

void CPnxCyclerDlg::OnReconnect(UINT nID)
{
	TRACE("");
}

void	CPnxCyclerDlg::OnClickCommand(UINT nID)
{
	int nCmdType = nID - ID_MENU_MAIN_CMD_START_KOR;

	switch (nCmdType)
	{
	case CommandTypeStart					: StartSchedule();				break;
 	case CommandTypePauseOption1			: PauseOption1();				break;
 	case CommandTypePauseOption2			: PauseOption2();				break;
 	case CommandTypePauseOption3			: PauseOption3();				break;
	case CommandTypePauseOption4			: PauseOption4();				break;
	case CommandTypeContinue				: RestartSchedule();			break;
	case CommandTypeNextOption1				: NextOption1();				break;
	case CommandTypeNextOption2				: NextOption2();				break;
	case CommandTypeCompleteOption1			: CompleteOption1();			break;
 	case CommandTypeCompleteOption2			: CompleteOption2();			break;
 	case CommandTypeCompleteOption3			: CompleteOption3();			break;
	case CommandTypeCompleteOption4			: CompleteOption4();			break;
	case CommandTypeAlarmClear				: AlarmClear();					break;
	case CommandTypeSafetyCondition			: ViewSystemSafetyCondition();	break;
	case CommandTypeViewAlarm				: ViewAlarm();					break;
	case CommandTypeViewPattern				: ViewPattern();				break;
	case CommandTypeViewLog					: ViewLogOpen();				break;
	case CommandTypeAnalyze_Simple			: ViewAnalyzeSimple();			break;
	case CommandTypeAnalyze_Detail			: ViewAnalyzeDetail();			break;
	case CommandTypeViewRelayCount			: ViewMaterial();				break;
	case CommandTypeViewReserve				: ViewReserve();				break;
	case CommandTypeViewReserveSchedule		: ViewReserveSchedule();		break;
	case CommandTypeViewScheduleAppend		: ViewScheduleAppend();			break;
	case CommandTypeModeChangeSingle	    : SetModeChangeSingle();		break;
	case CommandTypeModeChangeParallel		: SetModeChangeParallel();		break;
	case CommandTypeExcuteDAQEditor			: ExecuteDAQEditor();			break;
	case CommandTypeExcuteDAQLoading		: ViewDAQ();					break;
	case CommandTypeReconnect				: OnCommReconnect();			break;
	case CommandTypeChannelRestart			: OnErrorChannelRestart();		break;
	case CommandTypeCancelSync				: OnCancelSync();				break;
		
	}
}

void	CPnxCyclerDlg::StartSchedule()
{
	SYSLOG(Operation, _T("The operator chose to start the schedule."));

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	if (true == IsSystemError())
	{
		AfxMessageBox("Unable to proceed with system safety Cond not set");

		return;
	}

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}

// 	if (true == pMgr->IsCyclerChannelRestartEnable())
// 	{
// 		AfxMessageBox("이전 진행 이력이 남이 있어, 이어서 진행 합니다");
// 
// 		pMgr->ReStart();
// 
// 		return;		
// 	}

	if (false == pMgr->IsCyclerChannelStartEnable())
		return;

	CDlgViewSchedulelist* lpDlg = CDlgViewSchedulelist::GetMgr();

	//병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	m_fnSelectMasterSlave();

	if (lpDlg)
	{
		lpDlg->SetScheduleLoadingType(eScheduleLoadingTypeAdd);

		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);
	}
}

void	CPnxCyclerDlg::PauseOption1()
{
	SYSLOG(Operation, _T("The operator has selected an immediate stop command"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}

	if (false == pMgr->IsCyclerChannelStarted())
	{
		AfxMessageBox("Immediate stop command cannot be executed because the schedule is not in progress.");

		return;
	}


	//병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	m_fnSelectMasterSlave();

	pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionPauseImmediately);
}

void	CPnxCyclerDlg::PauseOption2()
{
	SYSLOG(Operation, _T("The operator selected the stop command after the step was finished"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}

	if (false == pMgr->IsCyclerChannelStarted())
	{
		AfxMessageBox("Reservation stop command cannot be executed because the schedule is not in progress.");

		return;
	}


	//병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	m_fnSelectMasterSlave();

	pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionPauseAfterStepComplete);
}

void	CPnxCyclerDlg::PauseOption3()
{
	SYSLOG(Operation, _T("The operator selected the stop command after the cycle was finished"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}

	if (false == pMgr->IsCyclerChannelStarted())
	{
		AfxMessageBox("Reservation stop command cannot be executed because the schedule is not in progress.");

		return;
	}


	//병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	m_fnSelectMasterSlave();

	pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionPauseAfterCycleComplete);
}

void	CPnxCyclerDlg::PauseOption4()
{
	SYSLOG(Operation, _T("The operator has selected a reservation stop command"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}

	CDlgNextPause* lpDlg = CDlgNextPause::GetMgr();

	if (lpDlg)
		lpDlg->ShowWindow(SW_SHOW);
}

void	CPnxCyclerDlg::RestartSchedule()
{
	SYSLOG(Operation, _T("The operator has chosen the restart command"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}

	std::vector<SRestartInfo> vecValue;
	if (false == pMgr->IsCyclerChannelRestartEnable(vecValue))
	{
		AfxMessageBox("The status of the selected channel is not stopped and cannot be restarted");

		return;
	}

	auto iter	= vecValue.begin();
	auto iterE	= vecValue.end();

	while (iter != iterE)
	{
		SRestartInfo sRestartInfo = (*iter);

		if (sRestartInfo.nIFBoardNumber >= 0)
		{
			CSockProc* lpProcedure = CSockProc::GetMgr();

			lpProcedure->SendBoardTypeP(sRestartInfo.nIFBoardNumber, (BYTE)sRestartInfo.nCyclerChannel, sRestartInfo.nRestartStep, eIFBoardPauseCommandRestart);
		}

		++iter;
	}
}

void	CPnxCyclerDlg::NextOption1()
{
	SYSLOG(Operation, _T("The operator selected the next step command"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	m_fnSelectMasterSlave(); //병렬 일 때 체널 선택하기

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}


	//병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	m_fnSelectMasterSlave();

	pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionJumpNext);
}

void	CPnxCyclerDlg::NextOption2()
{
	SYSLOG(Operation, _T("The operator selected the next step reservation command"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	m_fnSelectMasterSlave(); //병렬 일 때 체널 선택하기

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}

	CDlgNextStep* lpDlg = CDlgNextStep::GetMgr();

	if (lpDlg)
		lpDlg->ShowWindow(SW_SHOW);
}

void	CPnxCyclerDlg::CompleteOption1()
{
	SYSLOG(Operation, _T("The operator has selected an immediate End command"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}


	//병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	m_fnSelectMasterSlave();

	pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionCompleteImmediately);
}

void	CPnxCyclerDlg::CompleteOption2()
{
	SYSLOG(Operation, _T("The operator selected the End command after the step was finished"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}

	if (false == pMgr->IsCyclerChannelStarted())
	{
		AfxMessageBox("Reservation End command cannot be executed because the schedule is not in progress.");

		return;
	}


	//병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	m_fnSelectMasterSlave();

	pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionCompleteAfterStepComplete);
}

void	CPnxCyclerDlg::CompleteOption3()
{
	SYSLOG(Operation, _T("The operator selected the End command after the cycle was finished"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}

	if (false == pMgr->IsCyclerChannelStarted())
	{
		AfxMessageBox("Reservation End command cannot be executed because the schedule is not in progress.");

		return;
	}


	//병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	m_fnSelectMasterSlave();

	pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionCompleteAfterCycleComplete);
}

void	CPnxCyclerDlg::CompleteOption4()
{
	SYSLOG(Operation, _T("The operator has selected a reservation End command"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}

	CDlgNextComplete* lpDlg = CDlgNextComplete::GetMgr();

	if (lpDlg)
		lpDlg->ShowWindow(SW_SHOW);
}

void	CPnxCyclerDlg::AlarmClear()
{
	SYSLOG(Operation, _T("The operator has selected the alarm clear command"));

	CDlgViewAlarm* lpDlg = CDlgViewAlarm::GetMgr();

	if (lpDlg)
		lpDlg->ShowWindow(SW_HIDE);

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;
	// 선택한 체널만 클리어 2021-02-04 cnz
	int nYesNo = AfxMessageBox(_T("Do you want to clear all channels' alarms?\r\n[Yes] Clear all\r\n[No] Clear only selected channels"), MB_YESNO);
	pMgr->IsCyclerChannelExistAlarm(nYesNo);


}

void	CPnxCyclerDlg::ViewSystemSafetyCondition()
{
	SYSLOG(Operation, _T("The operator selected the system safety Cond setting command"));

	//스케쥴이 진행 중인 Channel이 있으면 Popup 안되게 함.
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	auto Pos = listChannel.GetHeadPosition();
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);
		if (NULL == pChannel)
			continue;

		if (true == pChannel->GetScheduleStart())
		{
			AfxMessageBox(_T("Schedule is running"));
			SYSLOG(Dbg, _T("%s return"), __FUNCTION__);
			return;
		}

		Sleep(1);
	}

	CDlgSystemSafety::GetMgr()->ShowWindow(SW_SHOW);
}


void	CPnxCyclerDlg::ViewAlarm()
{
	SYSLOG(Operation, _T("Operator selected alarm viewer command"));

	CDlgViewAlarm::GetMgr()->ShowWindow(SW_SHOW);
}

void	CPnxCyclerDlg::ViewPattern()
{
	SYSLOG(Operation, _T("The operator selected the channel progress command"));

	CDlgViewPattern* lpDlg = CDlgViewPattern::GetMgr();

	if (lpDlg)
		lpDlg->ShowWindow(SW_SHOW);
}

void	CPnxCyclerDlg::ViewLogOpen()
{
	SYSLOG(Operation, _T("The operator selected the command to open the log folder"));

	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (pMgr)
	{
		CString strFilePath;
		strFilePath.Format(_T("%s"), (LPCSTR)pMgr->GetLogFilePath());

		ULONG lCode = (ULONG)ShellExecute(NULL, "open", _T(strFilePath), NULL, NULL, SW_SHOW);

		if (lCode <= 32)
		{
			AfxMessageBox("Failed to open log path");
		}
	}
}

void	CPnxCyclerDlg::ViewAnalyzeSimple()
{
	SYSLOG(Operation, _T("The operator selected the analysis tool general analysis command"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		CChannel* lpCyclerChannel = pMgr->GetSelectedChannel();

		if (lpCyclerChannel)
		{
			CString strAnalzeToolPath = "C:\\Phoenixon Controls\\AnalyseViewer\\Analyse_Viewer.exe";
			//strAnalzeToolPath.Format("%s\\AnalyseTool\\Analyse_Viewer.exe", GetExePath());
			CString strFilePath;
			strFilePath = strAnalzeToolPath + " w " + "\"" + CString(lpCyclerChannel->GetLogFolderName()).Trim('\\') + "\"";

			WinExec(strFilePath, SW_SHOW);//swseo WinExec->CreateProcess 로 변경 추천
		}
		else
		{
			AfxMessageBox("Analysis tool cannot be linked because there is no selected channel Info");
		}
	}
}

void	CPnxCyclerDlg::ViewAnalyzeDetail()
{
	SYSLOG(Operation, _T("The operator selected the analysis tool detailed analysis command"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		CChannel* lpCyclerChannel = pMgr->GetSelectedChannel();

		if (lpCyclerChannel)
		{
			CString strAnalzeToolPath = "C:\\Phoenixon Controls\\AnalyseViewer\\Analyse_Viewer.exe";
			CString strFilePath;
			strFilePath = strAnalzeToolPath + " m " + "\"" + CString(lpCyclerChannel->GetLogFolderName()).Trim('\\') + "\"";

			WinExec(strFilePath, SW_SHOW);
		}
		else
		{
			AfxMessageBox("Analysis tool cannot be linked because there is no selected channel Info");
		}
	}
}

void	CPnxCyclerDlg::ViewMaterial()
{
	SYSLOG(Operation, _T("The operator selected the material management view command"));

	CDlgViewMaterial* lpDlg = CDlgViewMaterial::GetMgr();

	if (lpDlg)
	{
		lpDlg->ShowWindow(SW_SHOW);
	}
}

void	CPnxCyclerDlg::ViewReserve()
{
	SYSLOG(Operation, _T("The operator selected the stop reservation, view shutdown command"));

	CDlgReserveCancel* lpDlg = CDlgReserveCancel::GetMgr();

	if (lpDlg)
	{
		lpDlg->ShowWindow(SW_SHOW);
	}
}

void	CPnxCyclerDlg::ViewReserveSchedule()
{
	SYSLOG(Operation, _T("The operator selected the reservation schedule management command"));

	CDlgEditMultiSchedule* lpDlg = CDlgEditMultiSchedule::GetMgr();

	if (lpDlg)
		lpDlg->ShowWindow(SW_SHOW);
}

void	CPnxCyclerDlg::ViewScheduleAppend()
{
	SYSLOG(Operation, _T("The operator has chosen to start the schedule"));

	if (true == IsSystemError())
	{
		AfxMessageBox("Unable to proceed with system safety condition not set");

		return;
	}

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel Info.");

		return;
	}

	if (false == pMgr->IsCyclerChannelScheduleAppendEnable())
		return;

	CDlgViewSchedulelist* lpDlg = CDlgViewSchedulelist::GetMgr();

	if (lpDlg)
	{
		lpDlg->SetScheduleLoadingType(eScheduleLoadingTypeAppend);

		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);
	}
}

LRESULT	CPnxCyclerDlg::OnReceiveProgramExit(WPARAM wParam, LPARAM lParam)
{
	EndDialog(0);

	return 0;
}

LRESULT	CPnxCyclerDlg::OnReceiveSelectChannel(WPARAM wParam, LPARAM lParam)
{
	CDlgViewPattern* pDlgViewPattern = CDlgViewPattern::GetMgr();
	if (pDlgViewPattern)
	{
		pDlgViewPattern->ShowViewPattern((int)lParam);
	}

	return 0;
}

LRESULT CPnxCyclerDlg::OnReceiveAlarm(WPARAM wParam, LPARAM lParam)
{
	CDlgViewAlarm* pDlg = CDlgViewAlarm::GetMgr();

	if (NULL == pDlg)
		return 0;

	if (FALSE == pDlg->IsWindowVisible())
		pDlg->ShowWindow(SW_SHOW);

	return 0;
}


LRESULT CPnxCyclerDlg::OnReceiveAlarmUpdate(WPARAM wParam, LPARAM lParam)
{
	CDlgViewAlarm* pDlg = CDlgViewAlarm::GetMgr();

	if (NULL == pDlg)
		return 0;
	if (FALSE == pDlg->IsWindowVisible())
		pDlg->UpdateStatus(false);
	return 0;
}

LRESULT CPnxCyclerDlg::OnReceiveDAQSafety(WPARAM wParam, LPARAM lParam)
{
	int nChannelNumber	= (int)wParam;
	int nDAQWarning		= (int)lParam;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

	if (pChannel)
	{
		pChannel->SendPause(eIFBoardPauseCommandPause);
	}

	switch (nDAQWarning)
	{
	case 1: SYSLOG(Dbg, "DAQ Temperature Safety Min Occur Channel(%d)", nChannelNumber); break;
	case 2: SYSLOG(Dbg, "DAQ Temperature Safety Max Occur Channel(%d)", nChannelNumber); break;
	case 3: SYSLOG(Dbg, "DAQ Voltage Safety Min Occur Channel(%d)", nChannelNumber); break;
	case 4: SYSLOG(Dbg, "DAQ Voltage Safety Max Occur Channel(%d)", nChannelNumber); break;
	}

	return 0;
}

LRESULT CPnxCyclerDlg::OnReceiveDAQComplete(WPARAM wParam, LPARAM lParam)
{
	int nChannelNumber = (int)wParam;
	int nDAQWarning = (int)lParam;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

	if (pChannel)
	{
		pChannel->SendPause(eIFBoardPauseCommandComplete);
	}

	switch (nDAQWarning)
	{
	case 1: SYSLOG(Dbg, "DAQ Temperature Complete Min Occur Channel(%d)", nChannelNumber); break;
	case 2: SYSLOG(Dbg, "DAQ Temperature Complete Max Occur Channel(%d)", nChannelNumber); break;
	case 3: SYSLOG(Dbg, "DAQ Voltage Complete Min Occur Channel(%d)", nChannelNumber); break;
	case 4: SYSLOG(Dbg, "DAQ Voltage Complete Max Occur Channel(%d)", nChannelNumber); break;
	}

	return 0;
}

LRESULT CPnxCyclerDlg::OnReceiveReservePause(WPARAM wParam, LPARAM lParam)
{
	int nCycleNumber	= (int)wParam;
	int nStepNumber		= (int)lParam;

	// 해당 함수를 통해 들어오는 Cycler, Step 정보는 예약 Pause 이다.	

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionPauseReserved, nCycleNumber, nStepNumber);
	}	

	return 0;
}

LRESULT CPnxCyclerDlg::OnReceiveReserveComplete(WPARAM wParam, LPARAM lParam)
{
	int nCycleNumber	= (int)wParam;
	int nStepNumber		= (int)lParam;

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		if (true == pMgr->IsCyclerChannelExistReserve(nCycleNumber, nStepNumber))
		{
			AfxMessageBox("There is already a reserved stop/end channel.");

			return 0;
		}

		pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionCompleteReserved, nCycleNumber, nStepNumber);
	}

	return 0;
}

LRESULT CPnxCyclerDlg::OnReceiveReserveStep(WPARAM wParam, LPARAM lParam)
{
	int nCycleNumber = (int)wParam;
	int nStepNumber = (int)lParam;

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		if (true == pMgr->IsCyclerChannelExistReserve(nCycleNumber, nStepNumber))
		{
			AfxMessageBox("There is already a reserved stop/end channel.");

			return 0;
		}

		pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionJumpReserved, nCycleNumber, nStepNumber);
	}

	return 0;
}

LRESULT CPnxCyclerDlg::OnReceiveHideScheduleGui(WPARAM wParam, LPARAM lParam)
{
	CDlgViewSchedulelist* lpDlg = CDlgViewSchedulelist::GetMgr();

	if (lpDlg)
	{
		lpDlg->ShowWindow(SW_HIDE);
	}

	return 0;
}

LRESULT CPnxCyclerDlg::OnReceiveMultiScheduleStart(WPARAM wParam, LPARAM lParam)
{
	int nCyclerChannelNumber = (int)wParam;

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		CChannel* lpCyclerChannel = pMgr->GetChannel(nCyclerChannelNumber);

		if (lpCyclerChannel)
		{
			SYSTEMTIME tmCur;
			GetLocalTime(&tmCur);

			CString strLogFilePath;

			if (false == lpCyclerChannel->GetScheduleStart())
			{
				CMgrLog* pMgrLog = CMgrLog::GetMgr();

				if (!pMgrLog)
					return 0;

				SMultiSchedule* lpMultiSchedule = lpCyclerChannel->GetMultiScheduleInfo(0);

				if (!lpMultiSchedule)
					return 0;

				if (false == pMgrLog->CreateLogDirectory(lpCyclerChannel->GetChannelNumber(),  lpCyclerChannel->GetChBarcodeName(), lpMultiSchedule->strScheduleFilePath, tmCur, lpMultiSchedule->strEQPID, lpMultiSchedule->strScheduleFileName, strLogFilePath, lpMultiSchedule->strLogFilePath, lpMultiSchedule->strWorkName, true, true))
				{
					AfxMessageBox("Unable to proceed with the scheduled schedule because the file directory creation failed during the schedule progress.");

					return 0;
				}

				CMgrSchedule* lpSchedule = lpCyclerChannel->GetMgr();

				if (!lpSchedule)
					return 0;

				if (FALSE == lpSchedule->LoadScheduleXml(lpMultiSchedule->strScheduleFilePath))
				{
					AfxMessageBox("Unable to proceed because a problem was detected between loading schedules");

					return 0;
				}

				lpCyclerChannel->InitalizeReady();
				lpCyclerChannel->SetFileName(lpMultiSchedule->strScheduleFilePath);
				lpCyclerChannel->SetScheduleName(lpMultiSchedule->strScheduleFileName);
				lpCyclerChannel->SetLogFolderName(strLogFilePath);
				lpCyclerChannel->SetSyncMode(lpMultiSchedule->nSyncMode);
				lpCyclerChannel->SetUserID(lpMultiSchedule->strUserID);
				lpCyclerChannel->SetWorkName(lpMultiSchedule->strWorkName);
				lpCyclerChannel->SetCellID(lpMultiSchedule->strCellID);

				CMgrCommData* pMgrCommData = CMgrCommData::GetMgr();

				if (pMgrCommData)
				{
					CCommData* lpCyclerInfo = pMgrCommData->GetAt(lpCyclerChannel->GetChannelNumber());

					if (lpCyclerInfo)
						lpCyclerInfo->InitalizeReady();
				}

				CMgrSchedule* lpScheduleInfo = lpCyclerChannel->GetMgr();

				if (lpScheduleInfo)
				{
					CStep* lpStartReserve = lpScheduleInfo->GetAtStep(lpMultiSchedule->nStartCycle, lpMultiSchedule->nStartStep);

					if (lpStartReserve)
					{
						if (true == lpCyclerChannel->SendStep(lpStartReserve->GetStepIndex()))
						{
							Sleep(100);
						}
					}
				}

				lpCyclerChannel->InitStepStart();

				lpCyclerChannel->RemoveMultiSchedule(0);
			}
		}
	}

	return 0;
}


LRESULT CPnxCyclerDlg::OnReceiveDaQView(WPARAM WParam, LPARAM lParam)
{
	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (!pMgr)
		return 0;

	if (false == pMgr->IsLogin())
		return 0;

	sAccount* lpAccount = pMgr->GetLoginedAccount();

	if (nullptr == lpAccount)
		return 0;

	CDlgViewDAQ* lpDlg = CDlgViewDAQ::GetMgr();

	if (!lpDlg)
		return 0;

	if (true == lpAccount->bUseDAQView)
		lpDlg->ShowWindow(SW_SHOW);
	else
		lpDlg->ShowWindow(SW_HIDE);

	return 0;
}

void CPnxCyclerDlg::OnClose()
{
	SYSLOG(Operation, "User Click[Exit]");

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	auto Pos = listChannel.GetHeadPosition();
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);
		if (NULL == pChannel)
			continue;

		if (true == pChannel->GetScheduleStart())
		{
			//AfxMessageBox(_T("스케줄이 실행중 입니다"));
			SYSLOG(Dbg, _T("%s return"), __FUNCTION__);
			return;
		}
	}

// 	if (AfxMessageBox(_T("You Want Really Program Exit?"), MB_YESNO) == IDNO)
// 		return;

	CDialogEx::OnClose();
}

LRESULT CPnxCyclerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//WM_SOCKET_EVENT_CH1 ~ 6 까지 IF BOARD 1개로 묶어도 되니 정리하시오
	//AddBuffer1~n 까지가 필요가 없습니다

	switch( message )
	{
	case WM_SOCKET_EVENT_CH1:
		{
			CClientSocket* pSocket = CSockProc::GetMgr()->GetAt(0);

			if (NULL == pSocket)
				break;

			if ( WSAGETSELECTERROR( lParam ) )
			{
				if ( WSAEWOULDBLOCK != WSAGetLastError() )
				{
					pSocket->SetConnect(FALSE);
					TRACE(_T("GetLastError %d\n"), WSAGetLastError());
				}

				break;
			}

			switch( WSAGETSELECTEVENT( lParam ) )
			{
				case FD_CONNECT: pSocket->SetConnect(TRUE); pSocket->StartLinkThread();		break;
				case FD_CLOSE	: pSocket->CloseClientSocket(); break;
			}

			break;
		}
	case WM_SOCKET_EVENT_CH2:
		{
			CClientSocket* pSocket = CSockProc::GetMgr()->GetAt(1);

			if (NULL == pSocket)
				break;

			if (WSAGETSELECTERROR(lParam))
			{
				if (WSAEWOULDBLOCK != WSAGetLastError())
				{
					pSocket->SetConnect(FALSE);
					TRACE(_T("GetLastError %d\n"), WSAGetLastError());
				}

				break;
			}

			switch( WSAGETSELECTEVENT( lParam ) )
			{
				case FD_CONNECT: pSocket->SetConnect(TRUE);	pSocket->StartLinkThread();	break;
				case FD_CLOSE	: pSocket->CloseClientSocket(); break;
			}

			break;
		}
	case WM_SOCKET_EVENT_CH3:
		{
			CClientSocket* pSocket = CSockProc::GetMgr()->GetAt(2);

			if (NULL == pSocket)
				break;

			if (WSAGETSELECTERROR(lParam))
			{
				if (WSAEWOULDBLOCK != WSAGetLastError())
				{
					pSocket->SetConnect(FALSE);
					TRACE(_T("GetLastError %d\n"), WSAGetLastError());
				}

				break;
			}

			switch( WSAGETSELECTEVENT( lParam ) )
			{
				case FD_CONNECT: pSocket->SetConnect(TRUE);	pSocket->StartLinkThread();	break;
				case FD_CLOSE	: pSocket->CloseClientSocket(); break;
			}

			break;
		}
	case WM_SOCKET_EVENT_CH4:
		{
			CClientSocket* pSocket = CSockProc::GetMgr()->GetAt(3);

			if (NULL == pSocket)
				break;

			if (WSAGETSELECTERROR(lParam))
			{
				if (WSAEWOULDBLOCK != WSAGetLastError())
				{
					pSocket->SetConnect(FALSE);
					TRACE(_T("GetLastError %d\n"), WSAGetLastError());
				}

				break;
			}

			switch( WSAGETSELECTEVENT( lParam ) )
			{
				case FD_CONNECT: pSocket->SetConnect(TRUE);	pSocket->StartLinkThread();	break;
				case FD_CLOSE	: pSocket->CloseClientSocket(); break;
			}

			break;
		}
	case WM_SOCKET_EVENT_CH5:
	{
		CClientSocket* pSocket = CSockProc::GetMgr()->GetAt(4);

		if (NULL == pSocket)
			break;

		if (WSAGETSELECTERROR(lParam))
		{
			if (WSAEWOULDBLOCK != WSAGetLastError())
			{
				pSocket->SetConnect(FALSE);
				TRACE(_T("GetLastError %d\n"), WSAGetLastError());
			}

			break;
		}

		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_CONNECT: pSocket->SetConnect(TRUE);		pSocket->StartLinkThread(); break;
		case FD_CLOSE: pSocket->CloseClientSocket(); break;
		}

		break;
	}
	case WM_SOCKET_EVENT_CH6:
	{
		CClientSocket* pSocket = CSockProc::GetMgr()->GetAt(5);

		if (NULL == pSocket)
			break;

		if (WSAGETSELECTERROR(lParam))
		{
			if (WSAEWOULDBLOCK != WSAGetLastError())
			{
				pSocket->SetConnect(FALSE);
				TRACE(_T("GetLastError %d\n"), WSAGetLastError());
			}

			break;
		}

		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_CONNECT: pSocket->SetConnect(TRUE);	pSocket->StartLinkThread();	break;
		case FD_CLOSE: pSocket->CloseClientSocket(); break;
		}

		break;
	}
	}	

	return CDialogEx::WindowProc(message, wParam, lParam);
}

void CPnxCyclerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (LogReset == nIDEvent)
	{
		OpenDeleteMainLog();
	}

	if (UsageEquipTime == nIDEvent)
	{
		CMgrMaterial* pMgr = CMgrMaterial::GetMgr();

		if (pMgr)
		{
			pMgr->IncreaseEquipTime();
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void	CPnxCyclerDlg::OpenDeleteMainLog()
{
	CString strFileName;
	strFileName.Format("%s\\Delete File.bat", (LPCSTR)GetExePath());

	ShellExecute(NULL, "open", strFileName, NULL, NULL, SW_SHOW);
}

bool	CPnxCyclerDlg::IsSystemError()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (!pMgr)
		return true;

	return pMgr->IsErrorSystemSafety();
}

bool	CPnxCyclerDlg::isSafetyCheck()
{
	bool bIsSafety = true;

	CMgrChannel* pMgr = CMgrChannel::GetMgr();
	CList< CChannel*, CChannel* >& listChannel = pMgr->GetChannelList();

	CMgrCommData* pMgrCommData = CMgrCommData::GetMgr();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		CCommData* lpCyclerInfo = pMgrCommData->GetAt(lpCyclerChannel->GetChannelNumber());

		if (lpCyclerChannel->GetSelected() == true)
		{
			if (lpCyclerInfo->GetVoltage() > 0.3)
			{
				bIsSafety = false;
			}
		}
	}

	return bIsSafety;
}

void	CPnxCyclerDlg::ExecuteDAQEditor()
{
	SYSLOG(Operation, _T("User Click Menu[Execute DAQEditor]"));

	CString strFileName;
	strFileName.Format("%s\\DAQEditor\\DAQ.exe", (LPCSTR)GetExePath());

	ULONG Code = (ULONG)ShellExecute(NULL, "open", strFileName, NULL, NULL, SW_SHOW);
	if (Code <= 32)
	{
		CString str;
		str.Format(_T("Unable to run %s . Please check the route."), (LPCSTR)strFileName);
		MessageBox(str, _T("Unable to check route"), MB_OK);
		return;
	}
}

void	CPnxCyclerDlg::ViewDAQ()
{
	SYSLOG(Operation, _T("User Click Menu[View DAQ]"));

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	CList< CChannel*, CChannel* >& listChannel = pMgr->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (pChannel->GetScheduleStart() == TRUE)
		{
			SYSLOG(Dbg, _T("Start schedule..return ViewDAQ()"));
			return;
		}
	}

	CFileDialog dlg(TRUE, "sch", NULL,
		OFN_LONGNAMES | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY,
		_T("DAQ Files(*.csv)|*.csv"));

	CString strFilePath = CMgrConfig::GetMgr()->GetDAQFilePath();

	// 초기 경로 지정
	dlg.m_ofn.lpstrInitialDir = strFilePath;

	if (IDOK != dlg.DoModal())
		return;

	CString strDAQPath = dlg.GetPathName();

	
	CMgrDAQ::GetMgr()->LoadDAQ(strDAQPath);
	//CMgrDAQ::GetMgr()->LoadDAQCell();

	CDlgViewDAQ::GetMgr()->TimerStart();

	CString strIniFilePath;
	strIniFilePath.Format(_T("%s\\Config\\%s"), (LPCSTR)GetExePath(), _T("Common.ini"));
	INIWriteStr(_T("FilePath"), _T("DAQ"), strDAQPath, strIniFilePath);

}

void CPnxCyclerDlg::OnCommReconnect()
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr) {
		CChannel* lpCyclerChannel = pMgr->GetSelectedChannel();
		if (lpCyclerChannel == NULL)
			return;
		CSockProc* lpDevice = CSockProc::GetMgr();

		if (lpDevice) {
			CClientSocket* lpIFBoard = lpDevice->GetAtChannel(lpCyclerChannel->GetChannelNumber());
			if (lpIFBoard) {
				if (lpCyclerChannel->GetChannelStatus() == eCyclerChannelStateError) {
					lpIFBoard->StopThread();
					Sleep(100);
					lpIFBoard->ConnectToServer();
				}
			}
		}
	}
}

void CPnxCyclerDlg::OnErrorChannelRestart()
{
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	auto Pos = listChannel.GetHeadPosition();

	while (Pos) {
		CChannel* pChannel = listChannel.GetNext(Pos);
		if (NULL == pChannel || !pChannel->GetSelected() || pChannel->GetChannelStatus() != eCyclerChannelStateReady) {
			continue;
		}

		int chNum = pChannel->GetChannelNumber();
		CString specificationPath;
		specificationPath.Format(_T("%s\\Config\\Specification\\CH%02d.ini"), GetExePath(), chNum + 1);

		/*CString endCondition = INIReadStr("Specification", "EndCondition", specificationPath);
		if (endCondition.IsEmpty()) {
			CMgrChannel* pMgr = CMgrChannel::GetMgr();

			auto cycle = INIReadInt("Specification", "CycleNumber", specificationPath);
			auto cycleIndex = INIReadInt("Specification", "CycleIndex", specificationPath);
			auto step = INIReadInt("Specification", "StepNumber", specificationPath);
			auto stepIndex = INIReadInt("Specification", "StepIndex", specificationPath);

			pMgr->StartErrorChannelSchedule(chNum, cycle, step, cycleIndex);
		}*/
	}
}

void CPnxCyclerDlg::OnCancelSync()
{
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	auto Pos = listChannel.GetHeadPosition();

	while (Pos) {
		CChannel* pChannel = listChannel.GetNext(Pos);
		if (pChannel->GetSelected() == true) {
			pChannel->SetCancelStepSync(true);
			pChannel->SetCancelTempSync(true);
		}		
	}
}


void CPnxCyclerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CDlgMainTitle* pDlgMainTitle = CDlgMainTitle::GetMgr();

	if (pDlgMainTitle == NULL)
		return;

	InitUI();
}

void CPnxCyclerDlg::SetModeChangeSingle()
{
	SYSLOG(Operation, _T("Operator has chosen to change mode"));

	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	int nChannelCountInParallel = pMgr->GetParallelChannelCount(); // 병렬에 필요한 Channel 갯수 마스터 + 슬레이브
	int nChannelCountInOneBoard = pMgr->GetCChannelCountInOneBoard(); // 한개의 인터페이스 보드당 체널 갯수

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	//선택 돤 Channel에 속한 보드에 모든 체널 선택하기.
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (true == pChannel->GetScheduleStart())
		{
			continue;
		}

		if (pChannel->GetSelected() == true)
		{
			int nChannelStartNumber = nChannelCountInParallel * (pChannel->GetChannelNumber() / nChannelCountInParallel); //현재 선택 된 체널에 첫번째 체널 넘버

			//체널 No는 0 부터 시작. 
			for (int i = nChannelStartNumber; i < (nChannelStartNumber + nChannelCountInParallel); i++)
			{
				CChannel* pChannel_1 = CMgrChannel::GetMgr()->GetChannel(i);

				if (pChannel_1->GetScheduleStart() != true)
				{
					pChannel_1->SetSelected(true);
				}
			}
		}

		Sleep(1);
	}

	//모드 변환 시 Cell 연결이 되어 있으면 안된다.
	//if (isSafetyCheck() == false)
	//{
	//	AfxMessageBox("Cell 연결을 해제 후 Mode 전환 해주세요.");
	//	return;
	//}

	Pos = listChannel.GetHeadPosition();

	//선택 돤 Channel에 병렬 Command 날리기.
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (true == pChannel->GetScheduleStart())
		{
			continue;
		}

		
		if (pChannel->GetSelected() == true)
		{
			int nCurrentBoardNo = (pChannel->GetChannelNumber()) / nChannelCountInOneBoard; // 보드 넘버

			//Interface 보드에 Command 날리기.
			CSockProc::GetMgr()->SendBoardTypeC(nCurrentBoardNo, pow(2, ((pChannel->GetChannelNumber() - (nChannelCountInOneBoard * nCurrentBoardNo)))), 0x92);
			Sleep(100);
		}

		Sleep(1);
	}
}

void CPnxCyclerDlg::SetModeChangeParallel()
{
	SYSLOG(Operation, _T("Operator has chosen to change mode"));

	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	int nChannelCountInParallel = pMgr->GetParallelChannelCount(); // 병렬에 필요한 Channel 갯수 마스터 + 슬레이브
	int nChannelCountInOneBoard = pMgr->GetCChannelCountInOneBoard(); // 한개의 인터페이스 보드당 체널 갯수

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	//선택 돤 Channel에 속한 보드에 모든 체널 선택하기.
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (true == pChannel->GetScheduleStart())
		{
			continue;
		}

		if (pChannel->GetSelected() == true)
		{
			int nChannelStartNumber= nChannelCountInParallel * (pChannel->GetChannelNumber() / nChannelCountInParallel); //현재 선택 된 체널에 첫번째 체널 넘버

			//체널 No는 0 부터 시작. 
			for (int i = nChannelStartNumber; i < (nChannelStartNumber + nChannelCountInParallel); i++)
			{			
				CChannel* pChannel_1 = CMgrChannel::GetMgr()->GetChannel(i);

				if (pChannel_1->GetScheduleStart() != true)
				{
					pChannel_1->SetSelected(true);
				}
			}
		}

		Sleep(1);
	}

	//모드 변환 시 Cell 연결이 되어 있으면 안된다.
	//if (isSafetyCheck() == false)
	//{
	//	AfxMessageBox("Cell 연결을 해제 후 Mode 전환 해주세요.");
	//	return;
	//}

	//선택 돤 Channel에 병렬 Command 날리기.
	Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (true == pChannel->GetScheduleStart())
		{
			continue;
		}

		if (pChannel->GetSelected() == true)
		{
			int nCurrentBoardNo = (pChannel->GetChannelNumber()) / nChannelCountInOneBoard; // 보드 넘버
		
			//Interface 보드에 Command 날리기.
			CSockProc::GetMgr()->SendBoardTypeC(nCurrentBoardNo, pow(2,((pChannel->GetChannelNumber() - (nChannelCountInOneBoard * nCurrentBoardNo)))), 0x91);
			Sleep(500);
		}

		Sleep(1);
	}
}

void CPnxCyclerDlg::m_fnSelectMasterSlave()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	int nChannelCountInParallel = pMgr->GetParallelChannelCount(); // 병렬에 필요한 Channel 갯수 마스터 + 슬레이브
	int nChannelCountInOneBoard = pMgr->GetCChannelCountInOneBoard(); // 한개의 인터페이스 보드당 체널 갯수

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	//선택 돤 Channel에 속한 보드에 모든 체널 선택하기.
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;
		  
		if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (pChannel->GetSelected() == true)
			{
				int nChannelStartNumber = nChannelCountInParallel * (pChannel->GetChannelNumber() / nChannelCountInParallel); //현재 선택 된 체널에 첫번째 체널 넘버

				//체널 No는 0 부터 시작. 
				for (int i = nChannelStartNumber; i < (nChannelStartNumber + nChannelCountInParallel); i++)
				{
					CChannel* pChannel_1 = CMgrChannel::GetMgr()->GetChannel(i);

					pChannel_1->SetSelected(true);
				}
			}
		}

		Sleep(1);
	}
}

void CPnxCyclerDlg::m_fnInitEqMode()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	int nChannelCountInParallel = pMgr->GetParallelChannelCount(); // 병렬에 필요한 Channel 갯수 마스터 + 슬레이브
	int nChannelCountInOneBoard = pMgr->GetCChannelCountInOneBoard(); // 한개의 인터페이스 보드당 체널 갯수

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	//선택 돤 Channel에 속한 보드에 모든 체널 선택하기.
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		bool bBit = false;

		int nChannelStartNumber = pChannel->GetChannelNumber(); //현재 선택 된 체널에 첫번째 체널 넘버

		if (pChannel->GetChannelNumber() < 16)
		{
			CMgrDio::GetMgr()->m_fnGetDioOutput(nChannelStartNumber, &bBit);
		}
		else
		{
			CMgrDio::GetMgr()->m_fnGetDioOutput(nChannelStartNumber + 16, &bBit);
		}

		int nCurrentBoardNo = (pChannel->GetChannelNumber()) / nChannelCountInOneBoard; // 보드 넘버
		if (bBit == true)
		{
			CSockProc::GetMgr()->SendBoardTypeC(nCurrentBoardNo, pow(2, ((pChannel->GetChannelNumber() - (nChannelCountInOneBoard * nCurrentBoardNo)))), 0x91);
		}
		else
		{
			pChannel->SetSelected(true);
			CSockProc::GetMgr()->SendBoardTypeC(nCurrentBoardNo, pow(2, ((pChannel->GetChannelNumber() - (nChannelCountInOneBoard * nCurrentBoardNo)))), 0x92);
		}

		Sleep(1);
	}
}