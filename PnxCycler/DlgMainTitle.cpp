// DlgMainTitle.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "DlgMainTitle.h"
#include "afxdialogex.h"

#include "MgrChannel.h"
#include "MgrConfig.h"
#include "CDlgViewLogIn.h"
#include "CDlgEditChannelView.h"

// CDlgMainTitle 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgMainTitle, CDialogEx)

CDlgMainTitle::CDlgMainTitle(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMainTitle::IDD, pParent)
{
}

CDlgMainTitle::~CDlgMainTitle()
{
}

void CDlgMainTitle::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgMainTitle, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_COMMAND_RANGE(IDC_BTN_MAIN_TITLE_EXIT, IDC_BTN_MAIN_TITLE_USER, OnClickTitleButton)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgMainTitle 메시지 처리기입니다.


BOOL CDlgMainTitle::Create(CWnd* pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}


BOOL CDlgMainTitle::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


BOOL CDlgMainTitle::OnInitDialog()
{
	__super::OnInitDialog();

	InitStatic();
	InitButton();

	SetWindowPos(NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN), 70, SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgMainTitle::OnDestroy()
{
	__super::OnDestroy();
}

void CDlgMainTitle::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	m_btnLogo.SetBkDC(&dc);

	for (int i = 0; i < eViewTitleObjectMaxCount; ++i)
	{
		if(m_btnTitle[i])
			m_btnTitle[i].SetBkDC(&dc);
	}
}

void	CDlgMainTitle::InitStatic()
{
	LPCTSTR lpszText[] = { "End", "Setting", "Analyse", "Editor" };

	for (int i = 0; i < eViewTitleObjectMaxCount; ++i)
		CST_DEFAULT(m_ctrlText[i], IDC_STATIC_VIEW_TITLE_TEXT_1 + i, this, 12, 0, TRUE, ColorWhite, ColorBlack, lpszText[i]);
}

void	CDlgMainTitle::InitButton()
{
	SKINBTN(m_btnLogo, IDC_BTN_MAIN_TITLE_LOGO, this, GetExePath() + _T("\\res\\logo.png"), GetExePath() + _T("\\res\\logo.png"));

	CRect rtMain;

	GetWindowRect(&rtMain);
	ScreenToClient(&rtMain);

	LPCTSTR lpszName[] = { _T("\\res\\PowerOff.png"), _T("\\res\\Setting.png"), _T("\\res\\AnalyzeTool.png"), _T("\\res\\PatEditor.png"), _T("\\res\\User.png") };

	for (int i = 0; i < eViewTitleObjectMaxCount; ++i)
	{
		SKINBTN(m_btnTitle[i], IDC_BTN_MAIN_TITLE_EXIT + i, this, GetExePath() + lpszName[i], GetExePath() + lpszName[i]);
	}
}

void	CDlgMainTitle::OnClickTitleButton(UINT nID)
{
	int nType = nID - IDC_BTN_MAIN_TITLE_EXIT;

	switch (nType)
	{
	case eViewTitleObjectQuit		: ExecProgramQuit();	break;
	case eViewTitleObjectSetting	: ExecProgramSetting(); break;
	case eViewTitleObjectAnalyzeTool: ExecAnalyzeTool();	break;
	case eViewTitleObjectPtnEditor	: ExecPatternEditor();	break;
	}
}

bool	CDlgMainTitle::ExecProgramQuit()
{
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

			if(!lpCyclerChannel)
				continue;

			if (lpCyclerChannel->GetScheduleStart())
			{
				bExecuteCyclerChannel = true;

				break;
			}
		}

		if (true == bExecuteCyclerChannel)
		{
			if (AfxMessageBox("Channel in progress of the schedule. want to end it?", MB_YESNO) == IDNO) {
				return false;
			}
		}
	}

	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
		::PostMessage(pWnd->GetSafeHwnd(), WM_PROGRAM_EXIT, 0, 0);

	return true;
}

bool	CDlgMainTitle::ExecProgramSetting()
{
	CDlgEditChannelView* lpDlg = CDlgEditChannelView::GetMgr();

	if (lpDlg)
	{
		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);
	}

	return true;
}

bool	CDlgMainTitle::ExecAnalyzeTool()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (pMgr)
	{
		CString strFilePath;
		strFilePath = "C:\\Phoenixon Controls\\AnalyseViewer\\Analyse_Viewer.exe";

		ULONG lCode = (ULONG)ShellExecute(NULL, "open", strFilePath, NULL, NULL, SW_SHOW);

		if (lCode <= 32)
		{
			CString strErrorText;
			strErrorText.Format("Analyse executable does not exist in the specified path %s.", (LPCSTR)strFilePath);

			AfxMessageBox(strErrorText);

			return false;
		}
	}

	return true;
}
bool	CDlgMainTitle::ExecPatternEditor()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (pMgr)
	{
		CString strFilePath =  "C:\\Phoenixon Controls\\PNXEditorPro2\\PNXEditorPro.exe";
		ULONG lCode = (ULONG)ShellExecute(NULL, "open", strFilePath, NULL, NULL, SW_SHOW);

		if (lCode <= 32)
		{
			CString strErrorText;
			strErrorText.Format("Editor executable does not exist in the specified path %s.", (LPCSTR)strFilePath);

			AfxMessageBox(strErrorText);

			return false;
		}
	}

	return true;

}

bool	CDlgMainTitle::ExecUserLogIn()
{
	CDlgViewLogIn* lpDlg = CDlgViewLogIn::GetMgr();

	if (lpDlg)
	{
		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);

		return true;
	}

	return false;
}

void	CDlgMainTitle::SetLoginStatus(sAccount* lpAccount)
{
	if (lpAccount != nullptr)
	{
		//SetDlgItemText(IDC_STATIC_VIEW_LOGIN_STATUS, lpAccount->szUserID);
	}
	else
	{
		//SetDlgItemText(IDC_STATIC_VIEW_LOGIN_STATUS, "LOGIN");
	}
}


void CDlgMainTitle::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

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
			rcCtrl.top = rcParent.top+2;// +nSpace;
			rcCtrl.bottom = rcCtrl.top + nHeight;
		}
		else
			rcCtrl.OffsetRect(-(nWidth + nSpace), 0);

		m_btnTitle[i].MoveWindow(rcCtrl);	
		rcText = rcCtrl;
		rcText.top = rcCtrl.bottom+2;
		rcText.bottom = rcText.top + 12;
		m_ctrlText[i].MoveWindow(rcText);
		InvalidateRect(rcCtrl, false);
	}
}


HBRUSH CDlgMainTitle::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

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
	return hbr;
}


BOOL CDlgMainTitle::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	pDC->GetClipBox(&rc);
	pDC->FillSolidRect(&rc, ColorWhite);

	return __super::OnEraseBkgnd(pDC);
}
