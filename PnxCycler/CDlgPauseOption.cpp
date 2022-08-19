// CDlgPauseOption.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgPauseOption.h"
#include "afxdialogex.h" 
#include "MgrChannel.h"
#include "CDlgNextPause.h"


// CDlgPauseOption 대화 상자

IMPLEMENT_DYNAMIC(CDlgPauseOption, CDialogEx)

CDlgPauseOption::CDlgPauseOption(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_PAUSE_OPTION, pParent)
{

}

CDlgPauseOption::~CDlgPauseOption()
{
}

void CDlgPauseOption::DoDataExchange(CDataExchange* pDX)
{
	for (int i = 0; i < 4; i++)
	{
		DDX_Control(pDX, IDC_BTN_IMMEDIATELY_PAUSE + i, m_BtnPauseOption[i]);
	}



	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPauseOption, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_IMMEDIATELY_PAUSE, &CDlgPauseOption::OnBnClickedBtnImmediatelyPause)
	ON_BN_CLICKED(IDC_BTN_STEPCOMPLETE_PAUSE, &CDlgPauseOption::OnBnClickedBtnStepcompletePause)
	ON_BN_CLICKED(IDC_BTN_CYCLECOMPLETE_PAUSE, &CDlgPauseOption::OnBnClickedBtnCyclecompletePause)
	ON_BN_CLICKED(IDC_BTN_NEXT_PAUSE, &CDlgPauseOption::OnBnClickedBtnNextPause)
END_MESSAGE_MAP()


// CDlgPauseOption 메시지 처리기


BOOL CDlgPauseOption::Create(CWnd * pParentWnd)
{
	return CDialogEx::Create(IDD, pParentWnd);
}

void CDlgPauseOption::OnBnClickedBtnImmediatelyPause()
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


	////병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	//m_fnSelectMasterSlave();

	pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionPauseImmediately);

	ShowWindow(SW_HIDE);
}


void CDlgPauseOption::OnBnClickedBtnStepcompletePause()
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


	////병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	//m_fnSelectMasterSlave();

	pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionPauseAfterStepComplete);
}


void CDlgPauseOption::OnBnClickedBtnCyclecompletePause()
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


	////병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	//m_fnSelectMasterSlave();

	pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionPauseAfterCycleComplete);
}


void CDlgPauseOption::OnBnClickedBtnNextPause()
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


BOOL CDlgPauseOption::OnInitDialog()
{
	__super::OnInitDialog();
	CenterWindow();


	for (int i = 0; i < 4; i++)
	{
		m_BtnPauseOption[i].SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
		m_BtnPauseOption[i].SetFont("Verdana", 16, 1400);
		m_BtnPauseOption[i].SetTextColor(COLOR_BLACK);
	}
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgPauseOption::PreTranslateMessage(MSG* pMsg)
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
