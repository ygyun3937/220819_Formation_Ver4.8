// CDlgEditMultiSchedule.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgEditMultiSchedule.h"
#include "afxdialogex.h"
#include "MgrChannel.h"

#include "CDlgViewScheduleInfo.h"


// CDlgEditMultiSchedule 대화 상자

IMPLEMENT_DYNAMIC(CDlgEditMultiSchedule, CDialogEx)

CDlgEditMultiSchedule::CDlgEditMultiSchedule(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_EDIT_MULTE_SCHEDULE, pParent)
{
	m_nCyclerChannel	= -1;
	m_nSchedule			= -1;
}

CDlgEditMultiSchedule::~CDlgEditMultiSchedule()
{
}

void CDlgEditMultiSchedule::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EDIT_MULTISCHEDULE_CHANNEL, m_listCyclerChannel);
	DDX_Control(pDX, IDC_LIST_EDIT_MULTISCHEDULE_SCHEDULE, m_listSchedule);
}


BEGIN_MESSAGE_MAP(CDlgEditMultiSchedule, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_LIST_EDIT_MULTISCHEDULE_CHANNEL, &CDlgEditMultiSchedule::OnClickListEditMultischeduleChannel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_EDIT_MULTISCHEDULE_SCHEDULE, &CDlgEditMultiSchedule::OnClickListEditMultischeduleSchedule)
	ON_BN_CLICKED(IDC_BTN_EDIT_MULTISCHEDULE_DELETE, &CDlgEditMultiSchedule::OnBnClickedBtnEditMultischeduleDelete)
	ON_BN_CLICKED(IDC_BTN_EDIT_MULTISCHEDULE_CLOSE, &CDlgEditMultiSchedule::OnBnClickedBtnEditMultischeduleClose)
	ON_BN_CLICKED(IDC_BTN_EDIT_MULTISCHEDULE_UP, &CDlgEditMultiSchedule::OnBnClickedBtnEditMultischeduleUp)
	ON_BN_CLICKED(IDC_BTN_EDIT_MULTISCHEDULE_DOWN, &CDlgEditMultiSchedule::OnBnClickedBtnEditMultischeduleDown)
END_MESSAGE_MAP()


// CDlgEditMultiSchedule 메시지 처리기


BOOL CDlgEditMultiSchedule::Create(CWnd* pParentWnd)
{
	return CDialogEx::Create(IDD, pParentWnd);
}


BOOL CDlgEditMultiSchedule::PreTranslateMessage(MSG* pMsg)
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
		if (VK_F4 == pMsg->wParam)
		{
			ExecProcScheduleView();

			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CDlgEditMultiSchedule::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitListCtrl();

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgEditMultiSchedule::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (TRUE == bShow)
	{
		m_nCyclerChannel	= -1;
		m_nSchedule			= -1;

		LoadCyclerChannel();

		LoadSchedule();
	}
}


void CDlgEditMultiSchedule::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CDlgEditMultiSchedule::OnClickListEditMultischeduleChannel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_nCyclerChannel	= pNMItemActivate->iItem;
	m_nSchedule			= -1;

	LoadSchedule();

	*pResult = 0;
}


void CDlgEditMultiSchedule::OnClickListEditMultischeduleSchedule(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_nSchedule = pNMItemActivate->iItem;

	*pResult = 0;
}


void CDlgEditMultiSchedule::OnBnClickedBtnEditMultischeduleDelete()
{
	if (m_nCyclerChannel < 0 || m_nSchedule < 0)
	{
		AfxMessageBox("The schedule cannot be deleted because it has not been selected.");

		return;
	}

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		CChannel* lpCyclerChannel = pMgr->GetAt(m_nCyclerChannel);

		if (lpCyclerChannel)
		{
			lpCyclerChannel->RemoveMultiSchedule(m_nSchedule);
		}
	}

	LoadCyclerChannel();

	LoadSchedule();

	m_nSchedule = -1;
}


void CDlgEditMultiSchedule::OnBnClickedBtnEditMultischeduleClose()
{
	ShowWindow(SW_HIDE);
}


void CDlgEditMultiSchedule::OnBnClickedBtnEditMultischeduleUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgEditMultiSchedule::OnBnClickedBtnEditMultischeduleDown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void	CDlgEditMultiSchedule::InitListCtrl()
{
	{
		CRect rt;
		m_listCyclerChannel.GetClientRect(&rt);

		LPCTSTR lpszText[] = {"", "Channel", "Count", nullptr};

		m_listCyclerChannel.InsertColumn(0, "", LVCFMT_CENTER, 0);

		for (int i = 1; ; ++i)
		{
			if(nullptr == lpszText[i])
				break;

			m_listCyclerChannel.InsertColumn(i, lpszText[i], LVCFMT_CENTER, rt.Width() / 2);
		}

		m_listCyclerChannel.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	}

	{
		LPCTSTR lpszText[] = { "", "The order", "Schedule", "Result Save Path", "Worker", "Process name", "Sync", "Cell ID", "Start Cycle", "Start Step", nullptr };

		m_listSchedule.InsertColumn(0, "", LVCFMT_CENTER, 0);

		for (int i = 1; ; ++i)
		{
			if (nullptr == lpszText[i])
				break;

			m_listSchedule.InsertColumn(i, lpszText[i], LVCFMT_CENTER, GetColumnWidth(i));
		}

		m_listSchedule.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	}
}

void	CDlgEditMultiSchedule::LoadCyclerChannel()
{
	m_listCyclerChannel.DeleteAllItems();

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	int nIndex = 0;

	CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

	auto Pos = listCyclerChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

		if(!lpCyclerChannel)
			continue;

		std::vector<CString> vecValue;
		lpCyclerChannel->GetCyclerChannelInfo(vecValue);

		LVITEM Item;

		Item.iItem		= nIndex;
		Item.mask		= LVIF_TEXT | LVIF_PARAM;
		Item.stateMask	= LVIS_STATEIMAGEMASK;
		Item.state		= INDEXTOSTATEIMAGEMASK(1);
		Item.pszText	= _T("");
		Item.iSubItem	= 0;

		m_listCyclerChannel.InsertItem(&Item);
		m_listCyclerChannel.SetItemText(nIndex, 0, "");

		int nSubItem = 1;

		auto iter = vecValue.begin();
		auto iterE = vecValue.end();

		while (iter != iterE)
		{
			CString strValue = (*iter);

			m_listCyclerChannel.SetItemText(nIndex, nSubItem, strValue);

			nSubItem++;

			++iter;
		}

		nIndex++;
	}
}

void	CDlgEditMultiSchedule::LoadSchedule()
{
	m_listSchedule.DeleteAllItems();

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (m_nCyclerChannel < 0)
		return;

	int nIndex = 0;

	CChannel* lpCyclerChannel = pMgr->GetAt(m_nCyclerChannel);

	if (!lpCyclerChannel)
		return;

	std::vector<SMultiSchedule*>& vecMultiSchedule = lpCyclerChannel->GetMultiSchedule();

	auto iter = vecMultiSchedule.begin();
	auto iterE = vecMultiSchedule.end();

	while (iter != iterE)
	{
		SMultiSchedule* lpMultiSchedule = (*iter);

		if (lpMultiSchedule)
		{
			std::vector<CString> vecValue;
			lpMultiSchedule->GetScheduleInfo(vecValue);

			LVITEM Item;

			Item.iItem		= nIndex;
			Item.mask		= LVIF_TEXT | LVIF_PARAM;
			Item.stateMask	= LVIS_STATEIMAGEMASK;
			Item.state		= INDEXTOSTATEIMAGEMASK(1);
			Item.pszText	= _T("");
			Item.iSubItem	= 0;

			m_listSchedule.InsertItem(&Item);
			m_listSchedule.SetItemText(nIndex, 0, "");

			auto it = vecValue.begin();
			auto itE = vecValue.end();

			int nSubItem = 1;

			while (it != itE)
			{
				CString strValue = (*it);

				m_listSchedule.SetItemText(nIndex, nSubItem, strValue);

				nSubItem++;

				++it;
			}		
		}

		++iter;

		nIndex++;
	}
}

int		CDlgEditMultiSchedule::GetColumnWidth(int nIndex)
{
	switch (nIndex)
	{
	case eEditMultiScheduleObjectSerial: return 50;
	case eEditMultiScheduleObjectName: return 100;
	case eEditMultiScheduleObjectLogFilePath: return 200;
	case eEditMultiScheduleObjectUserID: return 100;
	case eEditMultiScheduleObjectWorkName: return 200;
	case eEditMultiScheduleObjectSyncType: return 95;
	case eEditMultiScheduleObjectCellID: return 100;
	case eEditMultiScheduleObjectCycle: return 100;
	case eEditMultiScheduleObjectStep: return 100;
	}

	return 50;
}
void	CDlgEditMultiSchedule::ExecProcScheduleView()
{
#if 0

	//m_listSchedule; m_nSchedule;
	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		SModel* lpModel = pMgr->GetModel(pMgr->GetModelIndex());

		if (lpModel)
		{
			SSchedule* lpSchedule = lpModel->GetSchedule(pMgr->GetScheduleIndex());

			if (lpSchedule)
			{
				CDlgViewScheduleInfo* lpScheduleInfo = CDlgViewScheduleInfo::GetMgr();

				if (lpScheduleInfo)
				{
					lpScheduleInfo->ShowWindow(SW_HIDE);
					lpScheduleInfo->ShowViewScheduleInfo(lpModel, lpSchedule);
				}
			}
		}
	}
#endif // 0

}