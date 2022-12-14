// CDlgMainTitleFormation.cpp: 구현 파일
//

//#include "pch.h"
#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgMainTitleFormation.h"
#include "afxdialogex.h"

#include "MgrChannel.h"
#include "MgrConfig.h"

#include "CDlgViewLogIn.h"
#include "CDlgEditChannelView.h"
#include "CDlgPogoPin.h"

#include "CDlgViewEqpSystemStatus.h"
#include "CDlgViewEqpGroupStatus.h"
#include "CDlgViewEqpChannelStatus.h"
#include "DlgViewAlarm.h"
#include "CDlgViewIoMap.h"
#include "CDlgViewIoManual.h"
#include "CDlgViewPlcManual.h"
#include "CDlgViewEqpLog.h"
#include "DlgViewDAQ.h"
#include "CDlgViewChamber.h"
#include "CDlgCalibration.h"

// CDlgMainTitleFormation 대화 상자

IMPLEMENT_DYNAMIC(CDlgMainTitleFormation, CDialogEx)

CDlgMainTitleFormation::CDlgMainTitleFormation(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_MAIN_TITLE_FORMATION, pParent)
{

}

CDlgMainTitleFormation::~CDlgMainTitleFormation()
{
}

void CDlgMainTitleFormation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);


	for (int i = 0; i < eViewTitleObjectMaxCount; i++)
	{
		DDX_Control(pDX, IDC_STATIC_VIEW_TITLE_TEXT_1 + i, m_ctrlText[i]);
	}
}

BEGIN_MESSAGE_MAP(CDlgMainTitleFormation, CDialogEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_COMMAND_RANGE(IDC_BTN_MAIN_TITLE_EXIT, IDC_BTN_MAIN_TITLE_USER, OnClickTitleButton)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_WARNING, &CDlgMainTitleFormation::OnBnClickedButtonAlarmWarning)
END_MESSAGE_MAP()

// CDlgMainTitleFormation 메시지 처리기

// -------------------------------------------------------------------------------------------------

void CDlgMainTitleFormation::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (m_btnTitle[eViewTitleObjectQuit].GetSafeHwnd() == NULL)
		return;

	CRect rcParent;
	CRect rcCtrl;
	CRect rcText;
	GetClientRect(&rcParent);
	m_btnTitle[eViewTitleObjectQuit].GetClientRect(&rcCtrl);
	m_ctrlText[eViewTitleObjectQuit].GetClientRect(&rcText);

	int nWidth = rcCtrl.Width();
	int nHeight = rcCtrl.Height();
	int nSpace = 20;

	for (int i = 0; i < eViewTitleObjectMaxCount; ++i)
	{
		if (i == 0)
		{
			rcCtrl.right = rcParent.right - nSpace;
			rcCtrl.left = rcCtrl.right - nWidth;
			rcCtrl.top = rcParent.top + 2;// +nSpace;
			rcCtrl.bottom = rcCtrl.top + nHeight;
		}
		else
			rcCtrl.OffsetRect(-(nWidth + nSpace), 0);

		m_btnTitle[i].MoveWindow(rcCtrl);
		rcText = rcCtrl;
		rcText.top = rcCtrl.bottom + 2;
		rcText.bottom = rcText.top + 12;
		m_ctrlText[i].MoveWindow(rcText);
		InvalidateRect(rcCtrl, false);
	}
}

void CDlgMainTitleFormation::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 __super::OnPaint()을(를) 호출하지 마십시오.
}

BOOL CDlgMainTitleFormation::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CRect rc;
	pDC->GetClipBox(&rc);
	pDC->FillSolidRect(&rc, ColorWhite);

	return __super::OnEraseBkgnd(pDC);
}

HBRUSH CDlgMainTitleFormation::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	else if (CTLCOLOR_DLG == nCtlColor)
	{
		pDC->SetBkColor(ColorWhite);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

// -------------------------------------------------------------------------------------------------

//BOOL CDlgMainTitleFormation::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
//{
//	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//
//	return __super::Create(lpszTemplateName, pParentWnd);
//}
BOOL CDlgMainTitleFormation::Create(CWnd * pParentWnd)
{
	//return 0;
	return __super::Create(IDD, pParentWnd);
}

void CDlgMainTitleFormation::SetLoginStatus(sAccount * lpAccount)
{
	if (lpAccount != nullptr)
	{
		SetDlgItemText(IDC_STATIC_VIEW_TITLE_TEXT_10, lpAccount->szUserID);
	
	}
	else
	{
		SetDlgItemText(IDC_STATIC_VIEW_TITLE_TEXT_10, "LOGIN" );
	}
	Invalidate();
}

BOOL CDlgMainTitleFormation::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	InitStatic();
	InitButton();

	SetWindowPos(NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), 70, SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgMainTitleFormation::InitStatic()
{
	//LPCTSTR lpszText[] = { "QUIT", "SETTING", "LOG", "CALIBRAT", "MANUAL", "IO MAP","CHAMBER", "ANALYZE", "ALARM", "EDITOR", "SYSTEM", "CHANNEL", "EQ GROUP", "USER" };
	LPCTSTR lpszText[] = { "QUIT","Pogo Pin", "CALIBRAT", "MANUAL","ANALYZE", "ALARM", "EDITOR", "SYSTEM", "CHANNEL", "USER" };

	for (int i = 0; i < eViewTitleObjectMaxCount; ++i)
	{
		m_ctrlText[i].SetBkColor(ColorWhite, ColorWhite);
		m_ctrlText[i].SetFont("Verdana", 10, 1400);
		m_ctrlText[i].SetTextColor(COLOR_BLACK);

		m_ctrlText[i].SetWindowText(lpszText[i]);
	}
}

void CDlgMainTitleFormation::InitButton()
{
	SKINBTN(m_btnLogo, IDC_BTN_MAIN_TITLE_LOGO, this, GetExePath() + _T("\\res\\logo.png"), GetExePath() + _T("\\res\\logo.png"));
	SKINBTN(m_btnAlarm, IDC_BUTTON_ALARM_WARNING, this, GetExePath() + _T("\\res\\warning.png"), GetExePath() + _T("\\res\\warning.png"));

	m_btnAlarm.ShowWindow(false);

	CRect rtMain;

	GetWindowRect(&rtMain);
	ScreenToClient(&rtMain);

	LPCTSTR lpszName[] = { _T("\\res\\PowerOff.png")
							, _T("\\res\\Contact.png")
							, _T("\\res\\Calibration.png")
							, _T("\\res\\IoManual.png")
							, _T("\\res\\AnalyzeTool.png")
							, _T("\\res\\Alarm.png")
							, _T("\\res\\PatEditor.png")
							, _T("\\res\\EqpSystem.png")
							, _T("\\res\\EqpChannel.png")
							, _T("\\res\\User.png") };
	for (int i = 0; i < eViewTitleObjectMaxCount; ++i)
	{
		SKINBTN(m_btnTitle[i], IDC_BTN_MAIN_TITLE_EXIT + i, this, GetExePath() + lpszName[i], GetExePath() + lpszName[i]);
	}
}

// -------------------------------------------------------------------------------------------------

void CDlgMainTitleFormation::OnClickTitleButton(UINT nID)
{

	int nType = nID - IDC_BTN_MAIN_TITLE_EXIT;
	switch (nType)
	{
	case eViewTitleObjectQuit:				ExecProgramQuit();		break;
	case eViewTitlePogoPinTest:				ExecProgramSetting();		break;
	case eViewTitleObjectCalibration:		ExecCalibration();		break;
	case eViewTitleObjectIoManual:			EcecIoManual();			break;
	case eViewTitleObjectAnalyzeTool:		ExecAnalyzeTool();		break;
	case eViewTitleObjectAlarm:				ExecAlarm();			break;
	case eViewTitleObjectPtnEditor:			ExecPatternEditor();	break;
	case eViewTitleObjectEqpSystemStatus:	ExecEqpSystem();		break;
	case eViewTitleObjectEqpChannelStatus:	ExecEqpChannel();		break;
	case eViewTitleObjectUser:				ExecUserLogIn();		break;
	}
}

bool CDlgMainTitleFormation::ExecProgramQuit()
{
	if (AfxMessageBox("Program Exit?", MB_YESNO) == IDNO)
	{
		return false;
	}
	//return false;
	SYSLOG(Operation, "User Click[Exit]");

	bool bExecuteCyclerChannel = false;

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

		auto Pos = listCyclerChannel.GetHeadPosition();

		while (Pos)
		{
			CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

			if (!lpCyclerChannel)
				continue;

			if (lpCyclerChannel->GetScheduleStart())
			{
				bExecuteCyclerChannel = true;

				break;
			}
		}

		if (true == bExecuteCyclerChannel)
		{
			if (AfxMessageBox("There is a channel in progress. Are you sure you want to quit?", MB_YESNO) == IDNO) {
				return false;
			}
		}
	}

	CDlgViewPlc1* pDlgViewPlc1 = CDlgViewPlc1::GetMgr();
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();

	if (pDlgViewPlc1 != NULL)
	{
		pDlgViewPlc1->SetPlcThreadStop();
	}
	if (pDlgViewPlc2 != NULL)
	{
		pDlgViewPlc2->SetPlcThreadStop();
	}

	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
		::PostMessage(pWnd->GetSafeHwnd(), WM_PROGRAM_EXIT, 0, 0);

	return true;
}

bool CDlgMainTitleFormation::ExecProgramSetting()
{
	CDlgPogoPin* lpDlg = CDlgPogoPin::GetMgr();
	CMgrAccount* pMgr = CMgrAccount::GetMgr();
	sAccount * lpAccount;
	lpAccount = pMgr->GetLoginedAccount();
	if (lpDlg)
	{
		if (lpAccount == nullptr)
		{
			AfxMessageBox("Login Please");
			return false;
		}
		if (_tcsicmp(lpAccount->szUserLevel, "Admin") != 0)
		{
			AfxMessageBox("Program Seeting Admin User Level만 할 수 있습니다.");
			return false;
		}
		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);
	}

	return true;
}

bool CDlgMainTitleFormation::ExecCalibration()
{
	CDlgCalibration* lpDlg = CDlgCalibration::GetMgr();

	CMgrAccount* pMgr = CMgrAccount::GetMgr();
	sAccount * lpAccount;
	
	lpAccount = pMgr->GetLoginedAccount();
	if (lpDlg)
	{
		if (lpAccount == nullptr)
		{
			AfxMessageBox("Login Please");
			return false;
		}
		if (_tcsicmp(lpAccount->szUserLevel, "Admin") != 0)
		{
			AfxMessageBox("Calibration Admin User Level만 할 수 있습니다.");
			return false;
		}
		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);
	}
	return true;
}

bool CDlgMainTitleFormation::EcecIoManual()
{
	bool bPLCUse;									//2022-05-11 : YYG PLC 통신 사용 유무 추가 (DFS 포메이션 장비 PLC 제어로 인한 추가)
	CString strIOType;
	CString strPLCType;
	strIOType = CMgrConfig::GetMgr()->m_strDioType;		//2021-05-10 : pnx_hr DIO Make 추가
	strPLCType = CMgrConfig::GetMgr()->m_strPLCControlType;		//2021-05-10 : pnx_hr DIO Make 추가
	bPLCUse = CMgrConfig::GetMgr()->m_bPLCControlUse;

	if (bPLCUse == true)
	{
		if (strPLCType == "MOXA")
		{
			CDlgViewIoManual* lpDlg = CDlgViewIoManual::GetMgr();
			CMgrAccount* pMgr = CMgrAccount::GetMgr();
			sAccount * lpAccount;
			lpAccount = pMgr->GetLoginedAccount();
			if (lpDlg)
			{
				if (lpAccount == nullptr)
				{
					AfxMessageBox("Login Please");
					return false;
				}
				if (_tcsicmp(lpAccount->szUserLevel, "Operator") == 0)
				{
					AfxMessageBox("IO Manual Admin 또는 Engineer User Level만 할 수 있습니다.");
					return false;
				}
				lpDlg->ShowWindow(SW_HIDE);
				lpDlg->ShowWindow(SW_SHOW);
			}
		}
		else if(strPLCType == "Mitsubishi")
		{
			CDlgViewPlcManual* lpDlg = CDlgViewPlcManual::GetMgr();
			CMgrAccount* pMgr = CMgrAccount::GetMgr();
			sAccount * lpAccount;
			lpAccount = pMgr->GetLoginedAccount();
			if (lpDlg)
			{
				if (lpAccount == nullptr)
				{
					AfxMessageBox("Login Please");
					return false;
				}
				if (_tcsicmp(lpAccount->szUserLevel, "Operator") == 0)
				{
					AfxMessageBox("Plc Manual Admin 또는 Engineer User Level만 할 수 있습니다.");
					return false;
				}
				lpDlg->ShowWindow(SW_HIDE);
				lpDlg->ShowWindow(SW_SHOW);
			}
		}	

		if (strIOType == "AJIN")
		{
			CDlgViewIoMap* lpDlg = CDlgViewIoMap::GetMgr();
			CMgrAccount* pMgr = CMgrAccount::GetMgr();
			sAccount * lpAccount;
			lpAccount = pMgr->GetLoginedAccount();
			if (lpDlg)
			{
				if (lpAccount == nullptr)
				{
					AfxMessageBox("Login Please");
					return false;
				}
				if (_tcsicmp(lpAccount->szUserLevel, "Operator") == 0)
				{
					AfxMessageBox("IO Manual Admin 또는 Engineer User Level만 할 수 있습니다.");
					return false;
				}
				lpDlg->ShowWindow(SW_HIDE);
				lpDlg->ShowWindow(SW_SHOW);
			}
		}
	}
	else
	{
		if (strIOType == "AJIN")
		{
			CDlgViewIoMap* lpDlg = CDlgViewIoMap::GetMgr();
			CMgrAccount* pMgr = CMgrAccount::GetMgr();
			sAccount * lpAccount;
			lpAccount = pMgr->GetLoginedAccount();
			if (lpDlg)
			{
				if (lpAccount == nullptr)
				{
					AfxMessageBox("Login Please");
					return false;
				}
				if (_tcsicmp(lpAccount->szUserLevel, "Operator") == 0)
				{
					AfxMessageBox("IO Manual Admin 또는 Engineer User Level만 할 수 있습니다.");
					return false;
				}
				lpDlg->ShowWindow(SW_HIDE);
				lpDlg->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			CDlgViewIoManual* lpDlg = CDlgViewIoManual::GetMgr();
			CMgrAccount* pMgr = CMgrAccount::GetMgr();
			sAccount * lpAccount;
			lpAccount = pMgr->GetLoginedAccount();
			if (lpDlg)
			{
				if (lpAccount == nullptr)
				{
					AfxMessageBox("Login Please");
					return false;
				}
				if (_tcsicmp(lpAccount->szUserLevel, "Operator") == 0)
				{
					AfxMessageBox("IO Manual Admin 또는 Engineer User Level만 할 수 있습니다.");
					return false;
				}
				lpDlg->ShowWindow(SW_HIDE);
				lpDlg->ShowWindow(SW_SHOW);
			}
		}
	}	
	return true;
}

bool CDlgMainTitleFormation::ExecIoMap()
{
	CDlgViewIoMap* lpDlg = CDlgViewIoMap::GetMgr();

	if (lpDlg)
	{
		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);
	}

	return true;
}
bool CDlgMainTitleFormation::ExecUserLogIn()
{
	CDlgViewLogIn* lpDlg = CDlgViewLogIn::GetMgr();

	if (lpDlg)
	{
		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);
	}

	return true;
}

bool CDlgMainTitleFormation::ExecAnalyzeTool()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (pMgr)
	{
		CMgrAccount* pMgr = CMgrAccount::GetMgr();
		sAccount * lpAccount;
		lpAccount = pMgr->GetLoginedAccount();
		if (lpAccount == nullptr)
		{
			AfxMessageBox("Login Please");
			return false;
		}
		if (_tcsicmp(lpAccount->szUserLevel, "Operator") == 0)
		{
			AfxMessageBox("Only Analyze Tool Admin or Engineer User Level can do this.");
			return false;
		}


		CString strFilePath;
		strFilePath = "C:\\Phoenixon Controls\\AnalyseViewer\\Analyse_Viewer.exe";

		ULONG lCode = (ULONG)ShellExecute(NULL, "open", strFilePath, NULL, NULL, SW_SHOW);

		if (lCode <= 32)
		{
			CString strErrorText;
			strErrorText.Format("The analysis tool executable file does not exist in the specified path %s.", (LPCSTR)strFilePath);

			AfxMessageBox(strErrorText);

			return false;
		}
	}

	return true;
}

bool CDlgMainTitleFormation::ExecAlarm()
{
	SYSLOG(Operation, _T("User Click Menu[Alarm]"));

	CDlgViewAlarm::GetMgr()->ShowWindow(SW_SHOW);

	return true;
}
bool CDlgMainTitleFormation::ExecPatternEditor()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (pMgr)
	{
		CMgrAccount* pMgr = CMgrAccount::GetMgr();
		sAccount * lpAccount;
		lpAccount = pMgr->GetLoginedAccount();
		if (lpAccount == nullptr)
		{
			AfxMessageBox("Login Please");
			return false;
		}
		if (_tcsicmp(lpAccount->szUserLevel, "Operator") == 0)
		{
			AfxMessageBox("Only Pattern Editor Admin or Engineer User Level can do this.");
			return false;
		}
		CString strFilePath = "C:\\Phoenixon Controls\\PNXEditorPro2\\PNXEditorPro.exe";
		ULONG lCode = (ULONG)ShellExecute(NULL, "open", strFilePath, NULL, NULL, SW_SHOW);

		if (lCode <= 32)
		{
			CString strErrorText;
			strErrorText.Format("The pattern editor executable does not exist in the specified path %s", (LPCSTR)strFilePath);

			AfxMessageBox(strErrorText);

			return false;
		}
	}

	return true;
}
bool CDlgMainTitleFormation::ExecEqLog()
{
	SYSLOG(Operation, _T("User Click Menu[EQP LOG]"));

	CDlgViewEqpGroupStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpChannelStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpSystemStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewChamber::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpLog::GetMgr()->ShowWindow(SW_SHOW);
	return true;
}

bool CDlgMainTitleFormation::ExecDaq()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();
	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (!pMgr)
		return false;

	if (false == pMgr->IsLogin())
		return false;

	sAccount* lpAccount = pMgr->GetLoginedAccount();

	if (nullptr == lpAccount)
		return false;
	CDlgViewDAQ* lpDlg = CDlgViewDAQ::GetMgr();

	if (!lpDlg)
		return false;

	if (true == lpAccount->bUseDAQView)
	{
		CDlgViewDAQ::GetMgr()->ShowWindow(SW_SHOW);
	}
	else
	{
		CDlgViewDAQ::GetMgr()->ShowWindow(SW_HIDE);
	}
	return true;
}

void CDlgMainTitleFormation::UI_Alarm(bool bShow)
{
	CDlgViewAlarm* pDlgAlarm = CDlgViewAlarm::GetMgr();

	if (NULL == pDlgAlarm)
		return;

	if (bShow == TRUE)
	{
		if (FALSE == m_btnAlarm.IsWindowVisible())
		{
			if (FALSE == pDlgAlarm->IsWindowVisible())
			{
				pDlgAlarm->UpdateStatus(false);
				pDlgAlarm->ShowWindow(true);
				m_btnAlarm.ShowWindow(true);
			}
		}
	}
	else
	{
		pDlgAlarm->ShowWindow(false);
		m_btnAlarm.ShowWindow(false);
	}
}

bool CDlgMainTitleFormation::ExecChamber()
{
	SYSLOG(Operation, _T("User Click Menu[EQP SYSTEM]"));

	CDlgViewEqpGroupStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpChannelStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpLog::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpSystemStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewChamber::GetMgr()->ShowWindow(SW_SHOW);


	return true;
}

bool CDlgMainTitleFormation::ExecEqpSystem()
{
	SYSLOG(Operation, _T("User Click Menu[EQP SYSTEM]"));

	CDlgViewEqpGroupStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpChannelStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpLog::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewChamber::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpSystemStatus::GetMgr()->ShowWindow(SW_SHOW);

	return true;
}

bool CDlgMainTitleFormation::ExecEqpChannel()
{
	SYSLOG(Operation, _T("User Click Menu[EQP CHANNEL]"));

	CDlgViewEqpGroupStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpSystemStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpLog::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewChamber::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpChannelStatus::GetMgr()->ShowWindow(SW_SHOW);

	return true;
}

bool CDlgMainTitleFormation::ExecEqpGroup()
{
	SYSLOG(Operation, _T("User Click Menu[EQP GROUP]"));

	CDlgViewEqpChannelStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpSystemStatus::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpLog::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewChamber::GetMgr()->ShowWindow(SW_HIDE);
	CDlgViewEqpGroupStatus::GetMgr()->ShowWindow(SW_SHOW);

	return true;
}






BOOL CDlgMainTitleFormation::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}




void CDlgMainTitleFormation::OnBnClickedButtonAlarmWarning()
{
	CDlgViewAlarm::GetMgr()->ShowWindow(SW_SHOW);
}
