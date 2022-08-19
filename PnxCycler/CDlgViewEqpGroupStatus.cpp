// CDlgViewEqpGroupStatus.cpp: 구현 파일
//

#include <ctime> 

//#include "pch.h"
#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewEqpGroupStatus.h"
#include "afxdialogex.h"

#include "MgrConfig.h"
#include "CDlgViewEqpChannelStatus.h"

#define OnDateTimeId	(10001)

// CDlgViewEqpGroupStatus 대화 상자

IMPLEMENT_DYNAMIC(CDlgViewEqpGroupStatus, CDialogEx)

CDlgViewEqpGroupStatus::CDlgViewEqpGroupStatus(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VIEW_EQP_GROUP_STATUS, pParent)
{
}

CDlgViewEqpGroupStatus::~CDlgViewEqpGroupStatus()
{
}

void CDlgViewEqpGroupStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgViewEqpGroupStatus, CDialogEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	
	ON_CONTROL_RANGE(STN_DBLCLK, IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPNAME1, IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPNAME2, OnDoubleClickEquipmentName)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------

// CDlgViewEqpGroupStatus 메시지 처리기

void CDlgViewEqpGroupStatus::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow == TRUE)
	{
		KillTimer(OnDateTimeId);
		SetTimer(OnDateTimeId, 1000, NULL);
	}
	else
	{
		KillTimer(OnDateTimeId);
	}
}

void CDlgViewEqpGroupStatus::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgViewEqpGroupStatus::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 __super::OnPaint()을(를) 호출하지 마십시오.
}

BOOL CDlgViewEqpGroupStatus::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CRect rc;
	pDC->GetClipBox(&rc);
	pDC->FillSolidRect(&rc, ColorWhite);

	return __super::OnEraseBkgnd(pDC);
}

/**
 * .
 * 
 * METHOD_NAME : OnCtlColor
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
HBRUSH CDlgViewEqpGroupStatus::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);


	return hbr;
}

void CDlgViewEqpGroupStatus::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nIDEvent == OnDateTimeId)
	{
		if (this->IsWindowVisible() != TRUE)
			return;

		// ---------------------------------------------------------------------------

		SetDisplayTime();
	}

	__super::OnTimer(nIDEvent);
}

void CDlgViewEqpGroupStatus::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	__super::OnClose();
}

void CDlgViewEqpGroupStatus::OnDestroy()
{
	__super::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

// -------------------------------------------------------------------------------------------------

BOOL CDlgViewEqpGroupStatus::Create(CWnd * pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}

BOOL CDlgViewEqpGroupStatus::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	Initialize();

	SetTimer(OnDateTimeId, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgViewEqpGroupStatus::Initialize()
{
	InitializeColors();
	InitializeControls();
}

void CDlgViewEqpGroupStatus::InitializeControls()
{
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	int nTotalSubEqpCount = pMgrConfig->GetTotalSubEqpCount();

	for (int nSubEqpCount = 0; nSubEqpCount < nTotalSubEqpCount; nSubEqpCount++)
	{
		if (nSubEqpCount > 0)
		{
			GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_INFOGRP1 + nSubEqpCount)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_LBL_EQPTYPE1 + nSubEqpCount)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_LBL_EQPPROCESS1 + nSubEqpCount)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_LBL_EQPSTEP1 + nSubEqpCount)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_LBL_EQPTIME1 + nSubEqpCount)->ShowWindow(TRUE);

			GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPNAME1 + nSubEqpCount)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPSTATUS1 + nSubEqpCount)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPTYPE1 + nSubEqpCount)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPPROCESS1 + nSubEqpCount)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPSTEP1 + nSubEqpCount)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPTIME1 + nSubEqpCount)->ShowWindow(TRUE);
		}

		GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPNAME1 + nSubEqpCount)->SetWindowText(pMgrConfig->GetSubEqpInfo().strSubEqpId);
		SetDisplayFont(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPNAME1 + nSubEqpCount, "Tahoma", 12);
		GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPSTATUS1 + nSubEqpCount)->SetWindowText(_T("IDLE"));
		GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPTYPE1 + nSubEqpCount)->SetWindowText(_T("NONE"));
		GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPPROCESS1 + nSubEqpCount)->SetWindowText(_T("NONE"));
		GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPSTEP1 + nSubEqpCount)->SetWindowText(_T("NONE"));
		GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPTIME1 + nSubEqpCount)->SetWindowText(_T("NONE"));
	}

	SetDisplayFont(IDC_STATIC_VIEW_EQP_GROUP_STATUS_DATE, _T("Tahoma"), 20);
}

void CDlgViewEqpGroupStatus::InitializeColors()
{
}

// -------------------------------------------------------------------------------------------------

void CDlgViewEqpGroupStatus::OnDoubleClickEquipmentName(UINT nID)
{
	int nSubEqpIndexNo = nID - IDC_STATIC_VIEW_EQP_GROUP_STATUS_EQPNAME1;
	CString strSubEqpName = CMgrConfig::GetMgr()->GetSubEqpInfo().strSubEqpId;

	CDlgViewEqpChannelStatus* pDlgViewEqpChannelStatus = CDlgViewEqpChannelStatus::GetMgr();

	pDlgViewEqpChannelStatus->SetSubEqpIndexNo(nSubEqpIndexNo);
	pDlgViewEqpChannelStatus->SetSubEqpId(strSubEqpName);
	pDlgViewEqpChannelStatus->SetDisplayEqpId(strSubEqpName);
	
	this->ShowWindow(SW_HIDE);
	pDlgViewEqpChannelStatus->ShowWindow(SW_SHOW);
}

// -------------------------------------------------------------------------------------------------

void CDlgViewEqpGroupStatus::SetDisplayFont(UINT nID, CString sFontName, int nFontSize)
{
	CFont setFont;
	setFont.CreatePointFont(nFontSize, sFontName);

	GetDlgItem(nID)->SetFont(&setFont);
}

void CDlgViewEqpGroupStatus::SetDisplayTime()
{
	CString sCurrentTime = GetDateTime(_T("TIME"));
	GetDlgItem(IDC_STATIC_VIEW_EQP_GROUP_STATUS_DATE)->SetWindowText(sCurrentTime);
}

/**
 * .
 * 
 * METHOD_NAME : GetDateTime
 * METHOD_TYPE : -
 * DESCRIPTION : Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
 * \return 
 */
CString CDlgViewEqpGroupStatus::GetDateTime(CString sGetType)
{
	//CString returnValue;

	//time_t timer;
	//struct tm* ptm;
	//timer = time(NULL); // 1970년 1월 1일 0시 0분 0초부터 시작하여 현재까지의 초
	//ptm = localtime(&timer); // 포맷팅을 위해 구조체에 넣기

	//if (sGetType == _T("DATE"))
	//{
	//	int year = ptm->tm_year + 1900;
	//	int month = ptm->tm_mon + 1;
	//	returnValue.Format("%d-%d-%d", year, month, ptm->tm_mday);
	//}
	//else if (sGetType == _T("TIME"))
	//{
	//	returnValue.Format("%d:%d:%d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	//}

	// ----------------------------------------------------------------------------

	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d %EX", &tstruct);

	return buf;
}

 BOOL CDlgViewEqpGroupStatus::PreTranslateMessage(MSG* pMsg)
 {
	 if (WM_KEYDOWN == pMsg->message)
	 {
		 if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			 return TRUE;
	 }

	 return __super::PreTranslateMessage(pMsg);
 }

 void CDlgViewEqpGroupStatus::UpdataEQStatus()
 {
	 CString str;

	 for (int nCh = 0; nCh < EQ_STAGE_INDEX; nCh++)
	 {
		 //EQ 상태 가져오는 함수 필요
		 //int nChannelStatus = CMgrChannel::GetMgr()->GetChannel(nCh)->GetChannelStatus();
		 //GetDlgItemText(IDC_STATIC_VIEW_EQP_CHANNEL_DATA_CHANNEL1 + nCh, str);

		 //COLORREF crTempStatusColor = GetDisplayChannelStatusColor(nChannelStatus);
		 //if (_tcsicmp(str, g_lpszCyclerStateString[nChannelStatus]) != 0)
		 //{
			// m_staticControl[nCh].SetBkColor(crTempStatusColor, crTempStatusColor);
			// m_staticControl[nCh].SetFont("Verdana", 11, 1200);
			// m_staticControl[nCh].SetTextColor(COLOR_BLACK);
			// m_staticControl[nCh].SetWindowText(g_lpszCyclerStateString[nChannelStatus]);
		 //}
	 }
 }

 COLORREF CDlgViewEqpGroupStatus::GetDisplayGroupStatusColor(int nEQStatus)
 {
	 COLORREF crStatusColor;
	 return crStatusColor;
 }
