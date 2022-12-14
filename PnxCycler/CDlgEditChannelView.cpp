// CDlgEditChannelView.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgEditChannelView.h"
#include "afxdialogex.h"
#include "CMgrAccount.h"


// CDlgEditChannelView 대화 상자

IMPLEMENT_DYNAMIC(CDlgEditChannelView, CDialogEx)

CDlgEditChannelView::CDlgEditChannelView(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_EDIT_HEADER, pParent)
{
	m_nCurSel = -1;
}

CDlgEditChannelView::~CDlgEditChannelView()
{
}

void CDlgEditChannelView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EDIT_HEADER, m_listChannel);
	DDX_Text(pDX, IDC_EDIT_EDIT_HEADER_1, m_strSerialNo);
	DDX_Text(pDX, IDC_EDIT_EDIT_HEADER_2, m_strName);
	DDX_Text(pDX, IDC_EDIT_EDIT_HEADER_3, m_strWidth);
}


BEGIN_MESSAGE_MAP(CDlgEditChannelView, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_EDIT_HEADER_FUNC_1, &CDlgEditChannelView::OnBnClickedBtnEditHeaderFunc1)
	ON_BN_CLICKED(IDC_BTN_EDIT_HEADER_FUNC_2, &CDlgEditChannelView::OnBnClickedBtnEditHeaderFunc2)
	ON_BN_CLICKED(IDC_BTN_EDIT_HEADER_FUNC_3, &CDlgEditChannelView::OnBnClickedBtnEditHeaderFunc3)
	ON_BN_CLICKED(IDC_CHK_EDIT_DAQ_VIEW, &CDlgEditChannelView::OnBnClickedChkEditDaqView)
	ON_NOTIFY(NM_CLICK, IDC_LIST_EDIT_HEADER, &CDlgEditChannelView::OnClickListEditHeader)
END_MESSAGE_MAP()


// CDlgEditChannelView 메시지 처리기

BOOL CDlgEditChannelView::Create(CWnd* pParentWnd)
{
	return CDialogEx::Create(IDD, pParentWnd);
}


BOOL CDlgEditChannelView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CDlgEditChannelView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CenterWindow();

	InitStatic();

	InitListCtrl();

	InitCheckBox();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgEditChannelView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (bShow)
	{
		LoadListCtrl();

		LoadCheckBox();
	}
}


void CDlgEditChannelView::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void	CDlgEditChannelView::InitStatic()
{
	LPCTSTR lpszText[] = { "The order", "Category", "Size" };

	for (int i = 0; i < eEditChannelObjectMaxCount; ++i)
		CST_DEFAULT(m_ctrlText[i], IDC_STATIC_EDIT_HEADER_TITLE_1 + i, this, 16, 0, TRUE, ColorLightGray, ColorWhite, lpszText[i]);
}

void	CDlgEditChannelView::InitListCtrl()
{
	CRect rt;

	m_listChannel.GetClientRect(&rt);

	m_listChannel.InsertColumn(0, "", LVCFMT_CENTER, 0);
	m_listChannel.InsertColumn(1, "The order", LVCFMT_CENTER, rt.Width() / 3);
	m_listChannel.InsertColumn(2, "Category", LVCFMT_CENTER, rt.Width() / 3);
	m_listChannel.InsertColumn(3, "Size", LVCFMT_CENTER, rt.Width() / 3);

	m_listChannel.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
}

void	CDlgEditChannelView::InitCheckBox()
{
	((CButton*)GetDlgItem(IDC_CHK_EDIT_DAQ_VIEW))->SetCheck(FALSE);
}

void	CDlgEditChannelView::LoadListCtrl()
{
	m_listChannel.DeleteAllItems();

	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (!pMgr)
		return;

	sCustomView* lpCustomView = pMgr->GetCustomView();

	if (!lpCustomView)
		return;

	int nIndex = 0;

	auto iter	= lpCustomView->vecViewItem.begin();
	auto iterE	= lpCustomView->vecViewItem.end();

	while (iter != iterE)
	{
		sChannelView* lpView = (*iter);
		//2021-02-23 cnz
		if (lpView == NULL)
			continue;

		LVITEM Item;

		Item.iItem		= nIndex;
		Item.mask		= LVIF_TEXT | LVIF_PARAM;
		Item.stateMask	= LVIS_STATEIMAGEMASK;
		Item.state		= INDEXTOSTATEIMAGEMASK(1);
		Item.pszText	= _T("");
		Item.iSubItem	= 0;

		m_listChannel.InsertItem(&Item);

		m_listChannel.SetItemText(nIndex, 0, "");

		CString strSerialNo;
		strSerialNo.Format("%d", lpView->nSerialNo);

		CString strWidth;
		strWidth.Format("%d", lpView->nWidth);

		m_listChannel.SetItemText(nIndex, 1, strSerialNo);
		m_listChannel.SetItemText(nIndex, 2, lpView->szName);
		m_listChannel.SetItemText(nIndex, 3, strWidth);

		nIndex++;

		++iter;
	}

	ListCtrl2EditBox();
}

void	CDlgEditChannelView::LoadCheckBox()
{
	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsLogin())
		return;

	sAccount* lpAccount = pMgr->GetLoginedAccount();

	if (nullptr == lpAccount)
		return;

	if (true == lpAccount->bUseDAQView)
		((CButton*)GetDlgItem(IDC_CHK_EDIT_DAQ_VIEW))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_CHK_EDIT_DAQ_VIEW))->SetCheck(FALSE);
}

void	CDlgEditChannelView::ListCtrl2EditBox()
{
	if (m_nCurSel >= 0)
	{
		CMgrAccount* pMgr = CMgrAccount::GetMgr();

		if (!pMgr)
			return;

		sCustomView* lpCustomView = pMgr->GetCustomView();

		if (lpCustomView)
		{
			sChannelView* lpView = lpCustomView->vecViewItem[m_nCurSel];

			if (lpView)
			{
				m_strSerialNo.Format("%d", lpView->nSerialNo);
				m_strName.Format("%s", lpView->szName);
				m_strWidth.Format("%d", lpView->nWidth);
			}
		}
	}
	else
	{
		m_strSerialNo.Empty();
		m_strName.Empty();
		m_strWidth.Empty();
	}

	UpdateData(FALSE);
}

void CDlgEditChannelView::OnClickListEditHeader(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_nCurSel = pNMItemActivate->iItem;

	ListCtrl2EditBox();

	*pResult = 0;
}


void CDlgEditChannelView::OnBnClickedBtnEditHeaderFunc1()
{
	ShowWindow(SW_HIDE);
}


void CDlgEditChannelView::OnBnClickedBtnEditHeaderFunc2()
{
	if (m_nCurSel < 0)
	{
		AfxMessageBox("Choose the actual list");

		return;
	}

	if (m_nCurSel == 0)
	{
		AfxMessageBox("It cannot be raised any higher");

		return;
	}

	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (pMgr)
	{
		pMgr->SortCustomView(m_nCurSel, eCustomViewSortTypeUp);

		// Up 버튼이기 때문에 실제 CurSel 은 하나 올라갔을 거다. 즉 --;
		m_nCurSel--;
	}

	LoadListCtrl();
}


void CDlgEditChannelView::OnBnClickedBtnEditHeaderFunc3()
{
	if (m_nCurSel < 0)
	{
		AfxMessageBox("Choose the actual list");

		return;
	}

	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (nullptr == pMgr)
		return;

	if (pMgr->GetCustomViewSize() <= m_nCurSel + 1)
	{
		AfxMessageBox("it cannot lower it any more");

		return;
	}

	pMgr->SortCustomView(m_nCurSel, eCustomViewSortTypeDown);

	m_nCurSel++;

	LoadListCtrl();
}


void CDlgEditChannelView::OnBnClickedChkEditDaqView()
{
	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsLogin())
		return;

	sAccount* lpAccount = pMgr->GetLoginedAccount();

	if (nullptr == lpAccount)
		return;

	UpdateData(TRUE);

	if (TRUE == ((CButton*)GetDlgItem(IDC_CHK_EDIT_DAQ_VIEW))->GetCheck())
		lpAccount->bUseDAQView = true;
	else
		lpAccount->bUseDAQView = false;

	pMgr->SaveAccount();

	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
		::PostMessage(pWnd->GetSafeHwnd(), WM_UPDATE_DAQ_VIEW, 0, 0);
}