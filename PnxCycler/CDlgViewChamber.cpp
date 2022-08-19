// CDlgViewChamber.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewChamber.h"
#include "afxdialogex.h"
#include "CMgrChamber.h"
#include "PnxUtil.h"

#define ChamberUpdateTimer	(1122)


// CDlgViewChamber 대화 상자

IMPLEMENT_DYNAMIC(CDlgViewChamber, CDialogEx)

CDlgViewChamber::CDlgViewChamber(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VIEW_CHAMBER, pParent)
{

}

CDlgViewChamber::~CDlgViewChamber()
{
}

void CDlgViewChamber::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgViewChamber, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_GROUP_EDIT_CHANGE, OnReceiveEditCtrlChange)
	ON_MESSAGE(WM_GROUP_SELECT, OnReceiveListSelect)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDlgViewChamber 메시지 처리기


BOOL CDlgViewChamber::Create(CWnd* pParentWnd)
{
	return CDialogEx::Create(IDD, pParentWnd);
}


BOOL CDlgViewChamber::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CDlgViewChamber::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitListCtrl();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgViewChamber::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;

	GetClientRect(&rt);

	pDC->FillSolidRect(&rt, ColorWhite);

	return TRUE;
}


void CDlgViewChamber::OnTimer(UINT_PTR nIDEvent)
{
	if (ChamberUpdateTimer == nIDEvent)
	{
		LoadChamber();
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDlgViewChamber::OnDestroy()
{
	CDialogEx::OnDestroy();

	KillTimer(ChamberUpdateTimer);
}

void	CDlgViewChamber::InitListCtrl()
{
	m_listChamber.Create(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_GROUP_LIST_VIEW_CHAMBER, NULL);
	m_listChamber.CreateScrollBar(IDC_GROUP_SCROLL_VIEW_CHAMBER);
	m_listChamber.CreateEditCtrl(IDC_EDIT_VIEW_CHAMBER_TEMPERATURE);
	m_listChamber.SetEditEnableColumn(5);
	m_listChamber.SetExplorerColumn(true);

	CRect rt;
	GetDlgItem(IDC_STATIC_LIST_CHAMBER)->GetWindowRect(&rt);

	// 실제 모니터 해상도를 구해서 좌, 우측에 List Control 이 들어갈 수 있도록 동일한 Gap 을 두기 위함.
	int nWidth = GetSystemMetrics(SM_CXSCREEN);

	SetWindowPos(NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN) - rt.left, 375, SWP_NOZORDER);

	{
		LPCTSTR lpszColumn[] = { _T("Chamber Number"), _T("Interlink Channel"), _T("State"), _T("Current Temp"), _T("Recipe Linked Temp"), _T("Manual setting Temp"), _T("Chamber action"), nullptr };

		for (int i = 0;; ++i)
		{
			if (nullptr == lpszColumn[i])
				break;

			m_listChamber.InsertColumn(lpszColumn[i], GetListCtrlColumn(i), TEXT_CENTER);
			m_listChamber.SetItemHeight(50);
		}
	}

	// List 는 Dialog 의 좌, 우측 Gap 사이에 넣는다.
	CRect rtScreen = rt;

	ScreenToClient(&rtScreen);

	m_listChamber.MoveWindow(rtScreen.left, rtScreen.top, nWidth - rt.left, 25);
	m_listChamber.SetGroupHeight(0);

	m_rect.left = rtScreen.left;
	m_rect.top = rtScreen.top;
	m_rect.right = nWidth - rt.left;
}

void	CDlgViewChamber::LoadChamber()
{
	m_listChamber.RemoveAllItems();

	sGroupInfo stGroupInfo;

	stGroupInfo.bExpand = TRUE;
	stGroupInfo.strText = "";
	stGroupInfo.bDrawExpand = FALSE;

	int nIndex = m_listChamber.InsertGroup(&stGroupInfo);

	CMgrChamber* pMgr = CMgrChamber::GetMgr();

	if (!pMgr)
		return;

	std::vector<SChamber*>& vecDevice = pMgr->GetDeviceChamber();

	auto iter = vecDevice.begin();
	auto iterE = vecDevice.end();
	
	while (iter != iterE)
	{
		SChamber* lpDeviceChamber = (*iter);

		if (lpDeviceChamber)
		{
			int nItem = m_listChamber.InsertItem(nIndex, _T(""));

			std::vector<CString> vecValue;
			lpDeviceChamber->GetChamberInfo(vecValue);

			int nSubItem = 0;

			auto it = vecValue.begin();
			auto itE = vecValue.end();

			while (it != itE)
			{
				m_listChamber.SetItemText(nIndex, nItem, nSubItem, (*it));

				nSubItem++;

				++it;
			}
		}

		++iter;
	}	

	m_listChamber.MoveWindow(m_rect.left, m_rect.top, m_rect.Width(), (vecDevice.capacity() * 50) + 25);
}

int		CDlgViewChamber::GetListCtrlColumn(int nIndex)
{
	switch (nIndex)
	{
	case eViewChamberObjectSerial: return 135;
	case eViewChamberObjectCyclerChannel: return 135;
	case eViewChamberObjectStatus: return 135;
	case eViewChamberObjectCurrentTemp: return 135;
	case eViewChamberObjectSettingAutoTemp: return 130;
	case eViewChamberObjectSettingManualTemp: return 130;
	case eViewChamberObjectOnOff: return 130;
	}

	return 50;
}

LRESULT CDlgViewChamber::OnReceiveEditCtrlChange(WPARAM wParam, LPARAM lParam)
{
	CString strEditText		= (LPSTR)(LPCTSTR)wParam;
	int		nChamberChannel	= (int)lParam;

	CMgrChamber* pMgr = CMgrChamber::GetMgr();

	if (pMgr)
	{
		SChamber* lpDeviceChamber = pMgr->GetDevice(nChamberChannel);

		if (lpDeviceChamber)
		{
			lpDeviceChamber->fManualSettingValue = (float)atof(strEditText);
			lpDeviceChamber->fSettingValue = lpDeviceChamber->fManualSettingValue;
			lpDeviceChamber->bSendChangeValue = true;
			//값이 바뀌었다고 알려 주자
		}
	}

	return 0;
}

LRESULT CDlgViewChamber::OnReceiveListSelect(WPARAM wParam, LPARAM lParam)
{
	CString strChamberIndex = (LPSTR)(LPCTSTR)wParam;
	int		nColumnIndex	= (int)lParam;

	if (eViewChamberObjectOnOff != nColumnIndex)
		return 0;

	CStringArray arr;
	CPnxUtil::GetMgr()->SplitString(strChamberIndex, ',', arr);

	CMgrChamber* pMgr = CMgrChamber::GetMgr();

	if (pMgr)
	{
		for (int i = 0; i < arr.GetSize(); ++i)
		{
			CString strChamberChannel = arr.GetAt(i);

			SChamber* lpDeviceChamber = pMgr->GetDevice(atoi(strChamberChannel));

			if (lpDeviceChamber)
			{
				if (false == lpDeviceChamber->bChamberExec)
				{
					// 요건 키는거야.
					if (lpDeviceChamber->fManualSettingValue > 0.f)
					{
						lpDeviceChamber->bSendOnCommand = true;
						lpDeviceChamber->bSendOffCommand = false;
						//Sleep(500) 또는 버튼 DIsable 필요;
						//lpDeviceChamber->bChamberExec	= true;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					lpDeviceChamber->bSendOnCommand		= false;
					lpDeviceChamber->bSendOffCommand	= true;
					//Sleep(500) 또는 버튼 DIsable 필요 -> 눌렀을때 일정 시간 버튼 Disable 하는 방향으로 구현하는게 좋을듯
					//문제는 Flag를 변환시켜 체크하고 챔버를 구동하는데 있어 Flag체크한는데 Delay발생함. 그사이 데이터 보내고받는
					//부분에있어 문제가생김;
					//lpDeviceChamber->bChamberExec		= false;
				}
			}
		}		
	}

	return 0;
}

void CDlgViewChamber::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
	if (bShow == TRUE)
	{
		KillTimer(ChamberUpdateTimer);
		SetTimer(ChamberUpdateTimer, 500, NULL);
	}
	else
	{
		KillTimer(ChamberUpdateTimer);
	}
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
