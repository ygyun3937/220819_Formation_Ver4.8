// CDlgNextStep.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgNextStep.h"
#include "afxdialogex.h"
#include "MgrChannel.h"

// CDlgNextStep 대화 상자

IMPLEMENT_DYNAMIC(CDlgNextStep, CDialogEx)

CDlgNextStep::CDlgNextStep(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_NEXT_STEP, pParent)
{

}

CDlgNextStep::~CDlgNextStep()
{
}

void CDlgNextStep::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgNextStep, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_STEP_MOVE_NEXT_CYCLE, &CDlgNextStep::OnBnClickedBtnStepMoveNextCycle)
	ON_BN_CLICKED(IDC_BTN_STEP_MOVE_NEXT_EXIT, &CDlgNextStep::OnBnClickedBtnStepMoveNextExit)	
END_MESSAGE_MAP()


// CDlgNextStep 메시지 처리기

BOOL CDlgNextStep::Create(CWnd* pParentWnd)
{
	return CDialogEx::Create(IDD, pParentWnd);
}

BOOL CDlgNextStep::PreTranslateMessage(MSG* pMsg)
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

BOOL CDlgNextStep::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CenterWindow();

	InitStatic();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgNextStep::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (TRUE == bShow)
	{
		GetDlgItem(IDC_EDIT_STEP_MOVE_CYCLE)->SetWindowText("1");
		GetDlgItem(IDC_EDIT_STEP_MOVE_STEP)->SetWindowText("1");
	}
}


void CDlgNextStep::InitStatic()
{
	LPCTSTR lpszName[] = { _T("Move Cycle number."), _T("Move Step number"), nullptr };

	for (int i = 0; i < eNextStepObjectMaxCount; ++i)
	{
		CST_DEFAULT(m_ctrlText[i], IDC_STATIC_STEP_MOVE_CYCLE + i, this, 16, 0, TRUE, ColorLightGray, ColorWhite, lpszName[i]);
	}
}

void CDlgNextStep::OnBnClickedBtnStepMoveNextCycle()
{
	UpdateData(TRUE);

	CString strCycleNumber;
	GetDlgItem(IDC_EDIT_STEP_MOVE_CYCLE)->GetWindowText(strCycleNumber);

	CString strStepNumber;
	GetDlgItem(IDC_EDIT_STEP_MOVE_STEP)->GetWindowText(strStepNumber);

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		if (false == pMgr->GetCyclerChannelExistStepInfo(atoi(strCycleNumber), atoi(strStepNumber)))
		{
			CString strErrorText;
			strErrorText.Format("Selected Cycle[%s] and Step[%s] do not exist in the current schedule.", (LPCSTR)strCycleNumber, (LPCSTR)strStepNumber);

			AfxMessageBox(strErrorText);

			return;
		}
	}

	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
		::PostMessage(pWnd->GetSafeHwnd(), WM_NEXT_STEP_RESERVE, atoi(strCycleNumber), atoi(strStepNumber));

	ShowWindow(SW_HIDE);
}


void CDlgNextStep::OnBnClickedBtnStepMoveNextExit()
{
	ShowWindow(SW_HIDE);
}