// CDlgReserveCancel.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgReserveCancel.h"
#include "afxdialogex.h"
#include "MgrChannel.h"


// CDlgReserveCancel 대화 상자

IMPLEMENT_DYNAMIC(CDlgReserveCancel, CDialogEx)

CDlgReserveCancel::CDlgReserveCancel(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_RESERVE_CANCLE, pParent)
{
	m_nCurSel = -1;
}

CDlgReserveCancel::~CDlgReserveCancel()
{
}

void CDlgReserveCancel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESERVE_CANCEL, m_listReserve);
}


BEGIN_MESSAGE_MAP(CDlgReserveCancel, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESERVE_CANCEL, &CDlgReserveCancel::OnItemChangedListReserveCancel)
	ON_COMMAND_RANGE(IDC_BTN_RESERVE_CANCEL_1, IDC_BTN_RESERVE_CANCEL_5, OnClickReserveCancel)
END_MESSAGE_MAP()


// CDlgReserveCancel 메시지 처리기
BOOL CDlgReserveCancel::Create(CWnd* pParentWnd)
{
	return CDialogEx::Create(IDD, pParentWnd);
}

BOOL CDlgReserveCancel::PreTranslateMessage(MSG* pMsg)
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


BOOL CDlgReserveCancel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitListCtrl();

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgReserveCancel::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (TRUE == bShow)
	{
		LoadListCtrl();
	}
}


void CDlgReserveCancel::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL CDlgReserveCancel::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;
	GetWindowRect(&rt);

	pDC->FillSolidRect(&rt, ColorWhite);

	return TRUE;
}

void CDlgReserveCancel::OnItemChangedListReserveCancel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	m_nCurSel = pNMLV->iItem;

	*pResult = 0;
}

void	CDlgReserveCancel::InitListCtrl()
{
	CRect rt;

	m_listReserve.GetClientRect(&rt);
	
	m_listReserve.InsertColumn(0, "", LVCFMT_CENTER, 0);
	m_listReserve.InsertColumn(1, "Channel", LVCFMT_CENTER, rt.Width() / 7);
	m_listReserve.InsertColumn(2, "Stop reservation", LVCFMT_CENTER, rt.Width() / 7);
	m_listReserve.InsertColumn(3, "Stop reservation Cycle", LVCFMT_CENTER, rt.Width() / 7);
	m_listReserve.InsertColumn(4, "Stop reservation Step", LVCFMT_CENTER, rt.Width() / 7);
	m_listReserve.InsertColumn(5, "End reservation", LVCFMT_CENTER, rt.Width() / 7);
	m_listReserve.InsertColumn(6, "End reservation Cycle", LVCFMT_CENTER, rt.Width() / 7);
	m_listReserve.InsertColumn(7, "End reservation Step", LVCFMT_CENTER, rt.Width() / 7);

	m_listReserve.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
}

void	CDlgReserveCancel::LoadListCtrl()
{
	m_listReserve.DeleteAllItems();

	int nIndex = 0;

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

			std::vector<CString> vecValue;
			lpCyclerChannel->GetReserveInfo(vecValue);			

			LVITEM Item;

			Item.iItem		= nIndex;
			Item.mask		= LVIF_TEXT | LVIF_PARAM;
			Item.stateMask	= LVIS_STATEIMAGEMASK;
			Item.state		= INDEXTOSTATEIMAGEMASK(1);
			Item.pszText	= _T("");
			Item.iSubItem	= 0;

			m_listReserve.InsertItem(&Item);
			m_listReserve.SetItemText(nIndex, 0, "");

			int nSubItem = 1;

			auto iter = vecValue.begin();
			auto iterE = vecValue.end();

			while (iter != iterE)
			{
				CString strValue = (*iter);

				m_listReserve.SetItemText(nIndex, nSubItem, strValue);

				nSubItem++;

				++iter;
			}

			nIndex++;
		}
	}

	m_nCurSel = -1;
}

void	CDlgReserveCancel::OnClickReserveCancel(UINT nID)
{
	int nType = nID - IDC_BTN_RESERVE_CANCEL_1;

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (eReserveCancleObjectPause == nType)
	{
		if (m_nCurSel < 0)
		{
			AfxMessageBox("Unable to cancel reservation stop because there is no selected channel");

			return;
		}

		CChannel* lpCyclerChannel =pMgr->GetChannel(m_nCurSel);

		if (!lpCyclerChannel)
			return;

		if (false == lpCyclerChannel->IsReservedPause())
		{
			AfxMessageBox("It cannot be canceled because it is not reserved");

			return;
		}

		lpCyclerChannel->SetReservePause(0, 0);

		LoadListCtrl();
	}
	else if (eReserveCancleObjectPauseAll == nType)
	{
		if (IDYES != AfxMessageBox("Do you want to cancel the suspension for all channels?", MB_YESNO))
			return;

		CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

		auto Pos = listCyclerChannel.GetHeadPosition();

		while (Pos)
		{
			CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

			if(!lpCyclerChannel)
				continue;

			lpCyclerChannel->SetReservePause(0, 0);
		}

		LoadListCtrl();
	}
	else if (eReserveCancleObjectComplete == nType)
	{
		if (m_nCurSel < 0)
		{
			AfxMessageBox("Can't cancel reservation end because there is no selected channel");

			return;
		}

		CChannel* lpCyclerChannel = pMgr->GetChannel(m_nCurSel);

		if (nullptr == lpCyclerChannel)
			return;

		if (false == lpCyclerChannel->IsReservedComplete())
		{
			AfxMessageBox("Can't cancel it because it's not reserved");

			return;
		}

		lpCyclerChannel->SetReserveComplete(0, 0);

		LoadListCtrl();
	}
	else if (eReserveCancleObjectCompleteAll == nType)
	{
		if (IDYES != AfxMessageBox("Do you want to cancel the reservation for all channels?", MB_YESNO))
			return;

		CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

		auto Pos = listCyclerChannel.GetHeadPosition();

		while (Pos)
		{
			CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

			if (!lpCyclerChannel)
				continue;

			lpCyclerChannel->SetReserveComplete(0, 0);
		}

		LoadListCtrl();
	}
	else if (eReserveCancleObjectClose == nType)
	{
		ShowWindow(SW_HIDE);
	}
}