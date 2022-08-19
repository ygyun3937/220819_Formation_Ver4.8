// DlgViewChamberAlarm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "DlgViewChamberAlarm.h"
#include "afxdialogex.h"


// CDlgViewChamberAlarm 대화 상자입니다.

#define TimerIdChamber	(3012)

IMPLEMENT_DYNAMIC(CDlgViewChamberAlarm, CDialogEx)

CDlgViewChamberAlarm::CDlgViewChamberAlarm(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgViewChamberAlarm::IDD, pParent)
{
	m_bBlink = FALSE;
}

CDlgViewChamberAlarm::~CDlgViewChamberAlarm()
{
}

void CDlgViewChamberAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgViewChamberAlarm, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgViewChamberAlarm 메시지 처리기입니다.


BOOL CDlgViewChamberAlarm::Create(CWnd* pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}


BOOL CDlgViewChamberAlarm::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


BOOL CDlgViewChamberAlarm::OnInitDialog()
{
	__super::OnInitDialog();

	CenterWindow();

	InitStatic();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgViewChamberAlarm::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if (TRUE == bShow)
	{
		SetTimer(TimerIdChamber, 1000, NULL);
	}
	else
	{
		KillTimer(TimerIdChamber);
	}
}


BOOL CDlgViewChamberAlarm::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;

	GetClientRect(&rt);

	pDC->FillSolidRect(&rt, ColorWhite);

	return TRUE;
}


void CDlgViewChamberAlarm::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}


void CDlgViewChamberAlarm::OnDestroy()
{
	__super::OnDestroy();

	KillTimer(TimerIdChamber);
}


void CDlgViewChamberAlarm::OnTimer(UINT_PTR nIDEvent)
{
	if (TimerIdChamber == nIDEvent)
	{
		SetBlink();
	}

	__super::OnTimer(nIDEvent);
}

void	CDlgViewChamberAlarm::InitStatic()
{
	CString strText;
	strText = _T("Chamber Alarm");
	CST_DEFAULT(m_ctrlText, IDC_STATIC_CHAMBER_ALARM, this, 36, 0, TRUE, RGB(255, 0, 0), ColorWhite, strText);
}

void	CDlgViewChamberAlarm::SetBlink()
{
	m_bBlink = !m_bBlink;

	if (TRUE == m_bBlink)
	{
		m_ctrlText.SetBkColor(ColorBlack);
		m_ctrlText.SetTextColor(ColorYellow);
	}
	else
	{
		m_ctrlText.SetBkColor(RGB(255, 0, 0));
		m_ctrlText.SetTextColor(ColorWhite);
	}
}