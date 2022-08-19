// DlgSystemSafety.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "DlgSystemSafety.h"
#include "afxdialogex.h"

#include "MgrConfig.h"

// CDlgSystemSafety 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSystemSafety, CDialogEx)

CDlgSystemSafety::CDlgSystemSafety(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSystemSafety::IDD, pParent)
{
	
}

CDlgSystemSafety::~CDlgSystemSafety()
{
	
}

void CDlgSystemSafety::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSystemSafety, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_SYSTEM_SAFETY_SAVE, &CDlgSystemSafety::OnBnClickedBtnSystemSafetySave)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_SAFETY_CLOSE, &CDlgSystemSafety::OnBnClickedBtnSystemSafetyClose)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlgSystemSafety 메시지 처리기입니다.

BOOL CDlgSystemSafety::Create(CWnd* pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}


BOOL CDlgSystemSafety::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return __super::PreTranslateMessage(pMsg);
}


BOOL CDlgSystemSafety::OnInitDialog()
{
	__super::OnInitDialog();

	CenterWindow();

	InitStatic();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgSystemSafety::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if (TRUE == bShow)
	{
		LoadSystemSafety();
	}
}


BOOL CDlgSystemSafety::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;

	GetClientRect(&rt);

	pDC->FillSolidRect(&rt, ColorWhite);

	return TRUE;
}

HBRUSH CDlgSystemSafety::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	if (IDC_CHK_SYSTEM_SAFETY_DELTA_USE == pWnd->GetDlgCtrlID())
	{
		pDC->SetBkColor(ColorWhite);

		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	else if (IDC_CHK_SYSTEM_SAFETY_CV_DELTA_USE == pWnd->GetDlgCtrlID())
	{
		pDC->SetBkColor(ColorWhite);

		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return hbr;
}

void	CDlgSystemSafety::InitStatic()
{
	LPCTSTR lpszTitle[] = { "Voltage", "Current", "Capacity", "Temp", "Voltage Change", "Current Change", "Check Change Time", "Error log time", "CV Voltage Change", "CV Current Change" };

	for (int i = 0; i < eSystemSafetyObjectMaxCount; ++i)
		CST_DEFAULT(m_ctrlTitle[i], IDC_STATIC_SYSTEM_SAFETY_TITLE_1 + i, this, 16, 0, TRUE, ColorWhite, ColorBlack, lpszTitle[i]);

	LPCSTR lpszUnits[] = { "V", "A", "Ah", "°C", "%", "%", "sec", "sec", "mV", "A"};

	for (int i = 0; i < eSystemSafetyObjectMaxCount; ++i)
		CST_DEFAULT(m_ctrlUnits[i], IDC_STATIC_SYSTEM_SAFETY_UNIT_1 + i, this, 14, 0, TRUE, ColorWhite, ColorBlack, lpszUnits[i]);

	LPCTSTR lpszType[] = { "Min", "Max" };

	for (int i = 0; i < eSystemSafetyTypeMaxCount; ++i)
		CST_DEFAULT(m_ctrlValue[i], IDC_STATIC_SYSTEM_SAFETY_TYPE_1 + i, this, 16, 0, TRUE, ColorWhite, ColorBlack, lpszType[i]);
}

void	CDlgSystemSafety::LoadSystemSafety()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (pMgr)
	{
		{
			std::vector<CString> vecValue;
			pMgr->GetSystemSafeInfoMin(vecValue);

			int nIndex = 0;

			auto iter	= vecValue.begin();
			auto iterE	= vecValue.end();

			while (iter != iterE)
			{
				CString strValue = (*iter);
				GetDlgItem(IDC_EDIT_SYSTEM_SAFETY_MIN_1 + nIndex)->SetWindowText(strValue);

				nIndex++;

				++iter;
			}
		}

		{
			std::vector<CString> vecValue;
			pMgr->GetSystemSafeInfoMax(vecValue);

			int nIndex = 0;

			auto iter	= vecValue.begin();
			auto iterE	= vecValue.end();

			while (iter != iterE)
			{
				CString strValue = (*iter);
				GetDlgItem(IDC_EDIT_SYSTEM_SAFETY_MAX_1 + nIndex)->SetWindowText(strValue);

				nIndex++;

				++iter;
			}
		}	

		bool bUseDeltaV = pMgr->GetIsDeltaIsUse();

		if (true == bUseDeltaV)
			((CButton*)GetDlgItem(IDC_CHK_SYSTEM_SAFETY_DELTA_USE))->SetCheck(TRUE);
		else
			((CButton*)GetDlgItem(IDC_CHK_SYSTEM_SAFETY_DELTA_USE))->SetCheck(FALSE);

		bool bUseCvDeltaV = pMgr->GetIsCvDeltaIsUse();

		if (true == bUseCvDeltaV)
			((CButton*)GetDlgItem(IDC_CHK_SYSTEM_SAFETY_CV_DELTA_USE))->SetCheck(TRUE);
		else
			((CButton*)GetDlgItem(IDC_CHK_SYSTEM_SAFETY_CV_DELTA_USE))->SetCheck(FALSE);
	}

	
}

void CDlgSystemSafety::OnBnClickedBtnSystemSafetySave()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (!pMgr)
		return;

	for (int i = 0; i < eSystemSafetyObjectMaxCount; ++i)
	{
		{
			CString strValue;
			GetDlgItem(IDC_EDIT_SYSTEM_SAFETY_MIN_1 + i)->GetWindowText(strValue);

			pMgr->SetSystemSafeInfoMin(i, strValue);
		}

		{
			CString strValue;
			GetDlgItem(IDC_EDIT_SYSTEM_SAFETY_MAX_1 + i)->GetWindowText(strValue);

			pMgr->SetSystemSafeInfoMax(i, strValue);
		}
	}

	if (TRUE == ((CButton*)GetDlgItem(IDC_CHK_SYSTEM_SAFETY_DELTA_USE))->GetCheck())
		pMgr->SetDeltaIsUse(true);
	else
		pMgr->SetDeltaIsUse(false);

	if (TRUE == ((CButton*)GetDlgItem(IDC_CHK_SYSTEM_SAFETY_CV_DELTA_USE))->GetCheck())
		pMgr->SetCvDeltaIsUse(true);
	else
		pMgr->SetCvDeltaIsUse(false);

	pMgr->SaveCommon();
}


void CDlgSystemSafety::OnBnClickedBtnSystemSafetyClose()
{
	ShowWindow(SW_HIDE);
}
