// CDlgNextPause.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgNextPause.h"
#include "afxdialogex.h"
#include "MgrChannel.h"

// CDlgNextPause 대화 상자

IMPLEMENT_DYNAMIC(CDlgNextPause, CDialogEx)

CDlgNextPause::CDlgNextPause(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_NEXT_PAUSE, pParent)
{

}

CDlgNextPause::~CDlgNextPause()
{
}

void CDlgNextPause::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgNextPause, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_PAUSE_MOVE_NEXT_EXIT, &CDlgNextPause::OnBnClickedBtnPasueMoveNextExit)
	ON_BN_CLICKED(IDC_BTN_PAUSE_MOVE_NEXT_CYCLE, &CDlgNextPause::OnBnClickedBtnPasueMoveNextCycle)
END_MESSAGE_MAP()


// CDlgNextPause 메시지 처리기

BOOL CDlgNextPause::Create(CWnd* pParentWnd)
{
	return CDialogEx::Create(IDD, pParentWnd);
}

BOOL CDlgNextPause::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam)
			return TRUE;

		if (VK_ESCAPE == pMsg->wParam)
		{
			ShowWindow(SW_HIDE);

			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

BOOL CDlgNextPause::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CenterWindow();

	InitStatic();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgNextPause::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (TRUE == bShow)
	{
		GetDlgItem(IDC_EDIT_PAUSE_MOVE_CYCLE)->SetWindowText("1");
		GetDlgItem(IDC_EDIT_PAUSE_MOVE_STEP)->SetWindowText("1");
	}
}

void CDlgNextPause::InitStatic()
{
	LPCTSTR lpszName[] = { _T("Move Cycle number"), _T("Move Step number"), nullptr };

	for (int i = 0; i < eNextPauseObjectMaxCount; ++i)
	{
		CST_DEFAULT(m_ctrlText[i], IDC_STATIC_PAUSE_MOVE_CYCLE + i, this, 16, 0, TRUE, ColorLightGray, ColorWhite, lpszName[i]);
	}
}

void CDlgNextPause::OnBnClickedBtnPasueMoveNextExit()
{
	ShowWindow(SW_HIDE);
}

void CDlgNextPause::OnBnClickedBtnPasueMoveNextCycle()
{
	UpdateData(TRUE);

	CString strCycleNumber;
	GetDlgItem(IDC_EDIT_PAUSE_MOVE_CYCLE)->GetWindowText(strCycleNumber);

	CString strStepNumber;
	GetDlgItem(IDC_EDIT_PAUSE_MOVE_STEP)->GetWindowText(strStepNumber);

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		if (false == pMgr->GetCyclerChannelExistStepInfo(atoi(strCycleNumber), atoi(strStepNumber)))
		{
			CString strErrorText;
			strErrorText.Format("Selected Cycle[%s] and Step[%s] do not exist in the current schedule", (LPCSTR)strCycleNumber, (LPCSTR)strStepNumber);

			AfxMessageBox(strErrorText);

			return;
		}
	}

	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
		::PostMessage(pWnd->GetSafeHwnd(), WM_NEXT_PAUSE_RESERVE, atoi(strCycleNumber), atoi(strStepNumber));

	ShowWindow(SW_HIDE);
}
