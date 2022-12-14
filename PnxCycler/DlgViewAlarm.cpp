// DlgViewAlarm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "DlgViewAlarm.h"
#include "CDlgMainTitleFormation.h"
#include "afxdialogex.h"
#include "resource.h"

#include "MgrChannel.h"
#include "MgrAlarm.h"
#include "PnxUtil.h"
#include "MgrConfig.h"
#include "MgrPCan.h"
#include "PCanCom.h"
#include "SockProc.h"

#define	AlarmBitMaxCount	( 16 )
#define AlarmTimerId		( 1026 )
#define TimerElapse			( 1000 )

// CDlgViewAlarm 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgViewAlarm, CDialogEx)

CDlgViewAlarm::CDlgViewAlarm(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgViewAlarm::IDD, pParent)
{

}

CDlgViewAlarm::~CDlgViewAlarm()
{
}

void CDlgViewAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TITLE_SYSTEMALARM, m_Title_SystemAlarm);
	DDX_Control(pDX, IDC_BUTTON_ALARM_CLEAR_ALL, m_Btn_AllAlarmClear);
	DDX_Control(pDX, IDC_BUTTON_ALARM_CLEAR, m_Btn_SelectAlarmClear);
	DDX_Control(pDX, IDC_TITLE_SYSTEMALARM_HISTORY_STAGE1, m_SubTitle_Stage1_AlarmHistory);
	DDX_Control(pDX, IDC_TITLE_SYSTEMALARM_HISTORY_STAGE2, m_SubTitle_Stage2_AlarmHistory);
	DDX_Control(pDX, IDC_TITLE_INVERTERALARM, m_SubTitle_InvertAlarm);
	DDX_Control(pDX, IDC_TITLE_EQALARM, m_SubTitle_EQAlarm);
	DDX_Control(pDX, IDC_TITLE_ALARM_COMMENT, m_SubTitle_AlarmComment);
}


BEGIN_MESSAGE_MAP(CDlgViewAlarm, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CHANNEL, &CDlgViewAlarm::OnSelchangeTabChannel)
	ON_MESSAGE(WM_GROUP_SELECT, OnReceiveListCtrlSelect)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_ALARM_CLEAR_ALL, &CDlgViewAlarm::OnBnClickedButtonAlarmClearAll)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_CLEAR, &CDlgViewAlarm::OnBnClickedButtonAlarmClear)
END_MESSAGE_MAP()


// CDlgViewAlarm 메시지 처리기입니다.


BOOL CDlgViewAlarm::Create(CWnd* pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}


BOOL CDlgViewAlarm::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYUP == pMsg->message)
	{
		switch (pMsg->wParam)

		{
		case VK_ESCAPE:
			this->ShowWindow(HIDE_WINDOW);
			break;
		}
		//return TRUE;
	}

	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


BOOL CDlgViewAlarm::OnInitDialog()
{
	__super::OnInitDialog();

	//	InitTabCtrl();

	InitListCtrl();

	CenterWindow();

	// 2020-12-14 cnz
	//SetTimer(AlarmTimerId, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgViewAlarm::OnShowWindow(BOOL bShow, UINT nStatus)
{
	//__super::OnShowWindow(bShow, nStatus);
}


BOOL CDlgViewAlarm::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;

	GetClientRect(&rt);

	pDC->FillSolidRect(&rt, ColorWhite);

	return TRUE;
}


void CDlgViewAlarm::OnDestroy()
{
	__super::OnDestroy();

	KillTimer(AlarmTimerId);
}

void CDlgViewAlarm::OnSelchangeTabChannel(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nCurSel = m_tabCtrl.GetCurSel();

	if (nCurSel < 0)
		return;

	LoadAlarm(nCurSel);
	LoadSysAlarm(nCurSel); //lyw4_1	
	LoadInverterAlarm(nCurSel);

	LoadAlarmComment();
	*pResult = 0;
}


void CDlgViewAlarm::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == AlarmTimerId)
	{
		CAlarm* pAlarm;
		if (pAlarm->GetAlarmSet() == true)
		{
			LoadAlarm();
			LoadSysAlarm();

			LoadAlarmComment();
		}

		CMgrPCan* pMgrPCan = CMgrPCan::GetMgr();


		if (pMgrPCan->GetStation(0) != NULL)
		{
			if (CMgrPCan::GetMgr()->GetStation(0)->GetAlarmStatus() == true)
			{
				LoadInverterAlarm();
			}
		}

		if (pMgrPCan->GetStation(1) != NULL)
		{
			if (CMgrPCan::GetMgr()->GetStation(1)->GetAlarmStatus() == true)
			{
				LoadInverterAlarm();
			}
		}
	}

	__super::OnTimer(nIDEvent);
}

LRESULT CDlgViewAlarm::OnReceiveListCtrlSelect(WPARAM wParam, LPARAM lParam)
{
	CString strSelectChannel;
	m_nCurSel = 0;
	strSelectChannel = (LPSTR)(LPCTSTR)wParam;

	CStringArray arr;

	CPnxUtil::GetMgr()->SplitString(strSelectChannel, ',', arr);

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		m_nCurSel = atoi(arr.GetAt(i));
	}

	LoadAlarm(m_nCurSel);
	LoadSysAlarm(m_nCurSel); //lyw4_1
	LoadInverterAlarm(m_nCurSel);
	LoadAlarmComment();
	return 0;
}

void	CDlgViewAlarm::InitTabCtrl()
{
	int nCount = CMgrChannel::GetMgr()->GetSize();

	for (int i = 0; i < nCount; ++i)
	{
		CString strTabName;

		strTabName.Format("Ch %d", i + 1);

		TCITEM tab_item;

		tab_item.mask = TCIF_TEXT;
		tab_item.pszText = (LPSTR)(LPCTSTR)strTabName;
		tab_item.iImage = 0;

		m_tabCtrl.InsertItem(i, &tab_item);
	}

	m_tabCtrl.SetTabCtrlEnd();
	m_tabCtrl.ModifyStyle(0, TCS_OWNERDRAWFIXED);
}

void	CDlgViewAlarm::InitListCtrl()
{
	CRect rt;

	CST_DEFAULT(m_ctrlDesc, IDC_STATIC_DESC, this, 12, 0, TRUE, ColorWhite, ColorLightRed, _T("* When an alarm occurs, select a channel and check it on the right"));

	//ALARM
	m_listCtrlAlarm.Create(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_GROUP_LIST_VIEW_ALARM, NULL);
	m_listCtrlAlarm.CreateScrollBar(IDC_GROUP_SCROLL_VIEW_ALARM);

	GetDlgItem(IDC_STATIC_LIST_ALARM)->GetWindowRect(&rt);

	LPCTSTR lpszColumnAlarm[] = { _T("BIT"), _T("WARNNING"), _T("DC"), _T("BATTERY"), _T("FAULT"), NULL };

	for (int i = 0;; ++i)
	{
		if (NULL == lpszColumnAlarm[i])
			break;

		if (i == 0)
			m_listCtrlAlarm.InsertColumn(lpszColumnAlarm[i], 60, TEXT_CENTER);
		else
			m_listCtrlAlarm.InsertColumn(lpszColumnAlarm[i], ((rt.Width() - 60) / 4) - 3, TEXT_CENTER);

		m_listCtrlAlarm.SetItemHeight(25);
	}

	ScreenToClient(&rt);

	m_listCtrlAlarm.MoveWindow(rt.left, rt.top, rt.Width(), rt.Height());
	m_listCtrlAlarm.SetGroupHeight(0);
	m_listCtrlAlarm.SetUNQID(0);


	//lyw4_1 : System ALARM
	m_listCtrlSysAlarm.Create(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_GROUP_LIST_VIEW_ALARM, NULL);
	m_listCtrlSysAlarm.CreateScrollBar(IDC_GROUP_SCROLL_VIEW_ALARM);

	GetDlgItem(IDC_STATIC_LIST_SYS_ALARM)->GetWindowRect(&rt);


	LPCTSTR lpszColumnSysAlarm[] = { _T("TIME"), _T("TITLE"), _T("CONTENTS"), NULL };

	for (int i = 0;; ++i)
	{
		if (NULL == lpszColumnSysAlarm[i])
			break;

		if (i == 0)
			m_listCtrlSysAlarm.InsertColumn(lpszColumnSysAlarm[i], 200, TEXT_CENTER);
		else if (i == 1)
			m_listCtrlSysAlarm.InsertColumn(lpszColumnSysAlarm[i], 120, TEXT_CENTER);
		else
			m_listCtrlSysAlarm.InsertColumn(lpszColumnSysAlarm[i], rt.Width() - 320, TEXT_CENTER);

		m_listCtrlSysAlarm.SetItemHeight(25);
	}

	ScreenToClient(&rt);

	m_listCtrlSysAlarm.MoveWindow(rt.left, rt.top, rt.Width(), rt.Height());
	m_listCtrlSysAlarm.SetGroupHeight(0);
	m_listCtrlSysAlarm.SetUNQID(2);

	//220726 System Alarm History
	m_listCtrlSysAlarmHistory_Stage1.Create(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_GROUP_LIST_VIEW_ALARM, NULL);
	m_listCtrlSysAlarmHistory_Stage1.CreateScrollBar(IDC_GROUP_SCROLL_VIEW_ALARM);

	GetDlgItem(IDC_STATIC_LIST_SYS_ALARM_HISTORY_STAGE1)->GetWindowRect(&rt);


	LPCTSTR lpszColumnSysAlarm_History_Stage1[] = { _T("TIME"), _T("TITLE"),_T("CH"),_T("CONTENTS"), NULL };

	for (int i = 0;; ++i)
	{
		if (NULL == lpszColumnSysAlarm_History_Stage1[i])
			break;

		if (i == 0)
			m_listCtrlSysAlarmHistory_Stage1.InsertColumn(lpszColumnSysAlarm_History_Stage1[i], 130, TEXT_CENTER);
		else if (i == 1)
			m_listCtrlSysAlarmHistory_Stage1.InsertColumn(lpszColumnSysAlarm_History_Stage1[i], 120, TEXT_CENTER);
		else if (i == 2)
			m_listCtrlSysAlarmHistory_Stage1.InsertColumn(lpszColumnSysAlarm_History_Stage1[i], 40, TEXT_CENTER);
		else
			m_listCtrlSysAlarmHistory_Stage1.InsertColumn(lpszColumnSysAlarm_History_Stage1[i], rt.Width() - 320, TEXT_CENTER);

		m_listCtrlSysAlarmHistory_Stage1.SetItemHeight(25);
	}

	ScreenToClient(&rt);

	m_listCtrlSysAlarmHistory_Stage1.MoveWindow(rt.left, rt.top, rt.Width(), rt.Height());
	m_listCtrlSysAlarmHistory_Stage1.SetGroupHeight(0);
	m_listCtrlSysAlarmHistory_Stage1.SetUNQID(2);



	m_listCtrlSysAlarmHistory_Stage2.Create(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_GROUP_LIST_VIEW_ALARM, NULL);
	m_listCtrlSysAlarmHistory_Stage2.CreateScrollBar(IDC_GROUP_SCROLL_VIEW_ALARM);

	GetDlgItem(IDC_STATIC_LIST_SYS_ALARM_HISTORY_STAGE2)->GetWindowRect(&rt);


	LPCTSTR lpszColumnSysAlarm_History_Stage2[] = { _T("TIME"), _T("TITLE"),_T("CH"),_T("CONTENTS"), NULL };

	for (int i = 0;; ++i)
	{
		if (NULL == lpszColumnSysAlarm_History_Stage2[i])
			break;

		if (i == 0)
			m_listCtrlSysAlarmHistory_Stage2.InsertColumn(lpszColumnSysAlarm_History_Stage2[i], 130, TEXT_CENTER);
		else if (i == 1)
			m_listCtrlSysAlarmHistory_Stage2.InsertColumn(lpszColumnSysAlarm_History_Stage2[i], 120, TEXT_CENTER);
		else if (i == 2)
			m_listCtrlSysAlarmHistory_Stage2.InsertColumn(lpszColumnSysAlarm_History_Stage2[i], 40, TEXT_CENTER);
		else
			m_listCtrlSysAlarmHistory_Stage2.InsertColumn(lpszColumnSysAlarm_History_Stage2[i], rt.Width() - 320, TEXT_CENTER);

		m_listCtrlSysAlarmHistory_Stage2.SetItemHeight(25);
	}

	ScreenToClient(&rt);

	m_listCtrlSysAlarmHistory_Stage2.MoveWindow(rt.left, rt.top, rt.Width(), rt.Height());
	m_listCtrlSysAlarmHistory_Stage2.SetGroupHeight(0);
	m_listCtrlSysAlarmHistory_Stage2.SetUNQID(2);


	m_listCtrlStation.Create(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_GROUP_LIST_VIEW_ALARM_CHANNEL, NULL);
	m_listCtrlStation.CreateScrollBar(IDC_GROUP_SCROLL_VIEW_ALARM_CHANNEL);

	GetDlgItem(IDC_STATIC_LIST_INVERTER)->GetWindowRect(&rt);

	LPCTSTR lpszColumnInverter[] = { _T("STATION"), _T("STATUS"), NULL };

	for (int i = 0;; ++i)
	{
		if (NULL == lpszColumnInverter[i])
			break;

		m_listCtrlStation.InsertColumn(lpszColumnInverter[i], ((rt.Width()) / 2) - 8, TEXT_CENTER);

		m_listCtrlStation.SetItemHeight(25);
	}

	ScreenToClient(&rt);

	m_listCtrlStation.MoveWindow(rt.left, rt.top, rt.Width(), rt.Height());
	m_listCtrlStation.SetGroupHeight(0);
	m_listCtrlStation.SetUNQID(1);

	m_listCtrlInverterAlarm.Create(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_GROUP_LIST_VIEW_ALARM, NULL);
	m_listCtrlInverterAlarm.CreateScrollBar(IDC_GROUP_SCROLL_VIEW_ALARM);

	GetDlgItem(IDC_STATIC_LIST_INVERTER_ALARM)->GetWindowRect(&rt);
	// 
	LPCTSTR lpszColumnInverterAlarm[] = { _T("BIT"), _T("IGBT"),_T("WARNNING"), _T("GRID"), _T("DC"), _T("FAULT"), NULL };

	for (int i = 0;; ++i)
	{
		if (NULL == lpszColumnInverterAlarm[i])
			break;
		if (i == 0)
			m_listCtrlInverterAlarm.InsertColumn(lpszColumnInverterAlarm[i], 60, TEXT_CENTER);
		else
			m_listCtrlInverterAlarm.InsertColumn(lpszColumnInverterAlarm[i], ((rt.Width() - 60) / 5) - 3, TEXT_CENTER);

		m_listCtrlInverterAlarm.SetItemHeight(25);
	}

	ScreenToClient(&rt);

	m_listCtrlInverterAlarm.MoveWindow(rt.left, rt.top, rt.Width(), rt.Height());
	m_listCtrlInverterAlarm.SetGroupHeight(0);
	m_listCtrlInverterAlarm.SetUNQID(2);
	


	//Eq Alarm
	m_listCtrlEqAlarm.Create(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_GROUP_LIST_VIEW_ALARM, NULL);
	m_listCtrlEqAlarm.CreateScrollBar(IDC_GROUP_SCROLL_VIEW_ALARM);

	GetDlgItem(IDC_STATIC_LIST_EQ_ALARM)->GetWindowRect(&rt);
	// 
	LPCTSTR lpszColumnEQAlarm[] = { _T("TIME"), _T("Alarm Code"), _T("CONTENTS"), NULL };

	for (int i = 0;; ++i)
	{
		if (NULL == lpszColumnEQAlarm[i])
			break;

		if (i == 0)
			m_listCtrlEqAlarm.InsertColumn(lpszColumnEQAlarm[i], 200, TEXT_CENTER);
		else if (i == 1)
			m_listCtrlEqAlarm.InsertColumn(lpszColumnEQAlarm[i], 80, TEXT_CENTER);
		else
			m_listCtrlEqAlarm.InsertColumn(lpszColumnEQAlarm[i], rt.Width() - 320, TEXT_CENTER);

		m_listCtrlEqAlarm.SetItemHeight(25);
	}

	ScreenToClient(&rt);

	m_listCtrlEqAlarm.MoveWindow(rt.left, rt.top, rt.Width(), rt.Height());
	m_listCtrlEqAlarm.SetGroupHeight(0);
	m_listCtrlEqAlarm.SetUNQID(2);
	//lyw4_1 ----------------------------------------------------

	//CHANNEL
	m_listCtrlChannel.Create(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_GROUP_LIST_VIEW_ALARM_CHANNEL, NULL);
	m_listCtrlChannel.CreateScrollBar(IDC_GROUP_SCROLL_VIEW_ALARM_CHANNEL);

	GetDlgItem(IDC_STATIC_LIST_CHANNEL)->GetWindowRect(&rt);

	LPCTSTR lpszColumnChannel[] = { _T("CHANNEL"), _T("STATUS"), NULL };

	for (int i = 0;; ++i)
	{
		if (NULL == lpszColumnChannel[i])
			break;

		m_listCtrlChannel.InsertColumn(lpszColumnChannel[i], ((rt.Width()) / 2) - 8, TEXT_CENTER);

		m_listCtrlChannel.SetItemHeight(25);
	}

	ScreenToClient(&rt);

	m_listCtrlChannel.MoveWindow(rt.left, rt.top, rt.Width(), rt.Height());
	m_listCtrlChannel.SetGroupHeight(0);
	m_listCtrlChannel.SetUNQID(1);	

	//Alarm Comment
	m_listCtrlAlarmComment.Create(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_GROUP_LIST_VIEW_ALARM, NULL);
	m_listCtrlAlarmComment.CreateScrollBar(IDC_GROUP_SCROLL_VIEW_ALARM);

	GetDlgItem(IDC_STATIC_LIST_ALARM_COMMENT)->GetWindowRect(&rt);
	// 
	LPCTSTR lpszColumnAlarmComment[] = { _T("Alarm Unit"), _T("Content"), _T("COMMENT"), NULL };

	for (int i = 0;; ++i)
	{
		if (NULL == lpszColumnAlarmComment[i])
			break;

		if (i == 0)
			m_listCtrlAlarmComment.InsertColumn(lpszColumnAlarmComment[i], 100, TEXT_CENTER);
		else if (i == 1)
			m_listCtrlAlarmComment.InsertColumn(lpszColumnAlarmComment[i], 200, TEXT_CENTER);
		else
			m_listCtrlAlarmComment.InsertColumn(lpszColumnAlarmComment[i], rt.Width() - 320, TEXT_CENTER);

		m_listCtrlAlarmComment.SetItemHeight(25);
	}

	ScreenToClient(&rt);

	m_listCtrlAlarmComment.MoveWindow(rt.left, rt.top, rt.Width(), rt.Height());
	m_listCtrlAlarmComment.SetGroupHeight(0);
	m_listCtrlAlarmComment.SetUNQID(2);
	
	m_Title_SystemAlarm.SetBkColor(ColorWhite, ColorWhite);
	m_Title_SystemAlarm.SetFont("Verdana", 14, 1200);
	m_Title_SystemAlarm.SetTextColor(COLOR_BLACK);

	m_SubTitle_Stage1_AlarmHistory.SetBkColor(ColorWhite, ColorWhite);
	m_SubTitle_Stage1_AlarmHistory.SetFont("Verdana", 14, 1200);
	m_SubTitle_Stage1_AlarmHistory.SetTextColor(COLOR_BLACK);

	m_SubTitle_Stage2_AlarmHistory.SetBkColor(ColorWhite, ColorWhite);
	m_SubTitle_Stage2_AlarmHistory.SetFont("Verdana", 14, 1200);
	m_SubTitle_Stage2_AlarmHistory.SetTextColor(COLOR_BLACK);

	m_SubTitle_InvertAlarm.SetBkColor(ColorWhite, ColorWhite);
	m_SubTitle_InvertAlarm.SetFont("Verdana", 14, 1200);
	m_SubTitle_InvertAlarm.SetTextColor(COLOR_BLACK);

	m_SubTitle_EQAlarm.SetBkColor(ColorWhite, ColorWhite);
	m_SubTitle_EQAlarm.SetFont("Verdana", 14, 1200);
	m_SubTitle_EQAlarm.SetTextColor(COLOR_BLACK);

	m_SubTitle_AlarmComment.SetBkColor(ColorWhite, ColorWhite);
	m_SubTitle_AlarmComment.SetFont("Verdana", 14, 1200);
	m_SubTitle_AlarmComment.SetTextColor(COLOR_BLACK);

	m_Btn_AllAlarmClear.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_Btn_AllAlarmClear.SetFont("Verdana", 10, 1200);
	m_Btn_AllAlarmClear.SetTextColor(COLOR_BLACK);

	m_Btn_SelectAlarmClear.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_Btn_SelectAlarmClear.SetFont("Verdana", 10, 1200);
	m_Btn_SelectAlarmClear.SetTextColor(COLOR_BLACK);

	LoadAlarm();

	

	UpdateStatus(true);
}

void	CDlgViewAlarm::LoadAlarm(int nChannelNo /*= 0*/)
{
	m_csLock.Lock();

	m_listCtrlAlarm.RemoveAllItems();

	sGroupInfo stGroupInfo;

	stGroupInfo.bExpand = TRUE;
	stGroupInfo.strText = "";
	stGroupInfo.bDrawExpand = FALSE;

	int nIndex = m_listCtrlAlarm.InsertGroup(&stGroupInfo);

	CMgrAlarm* pMgr = CMgrAlarm::GetMgr();
	if (pMgr == NULL)	
		return;

	for (int i = 0; i < AlarmBitMaxCount; ++i)
	{
		CStringArray arr;

		pMgr->GetChannelAlarm(nChannelNo, i, arr);

		int nItem = m_listCtrlAlarm.InsertItem(nIndex, _T(""));

		for (int j = 0; j < arr.GetSize(); ++j)
		{
	
			CString str = arr.GetAt(j);

			m_listCtrlAlarm.SetItemText(nIndex, nItem, j, str);

			if (j > 0 && FALSE == str.IsEmpty())
			{
				m_listCtrlAlarm.SetSubItemColor(nIndex, nItem, j, ColorLightRed, TRUE);
			}
			else if (j > 0 && TRUE == str.IsEmpty())
			{
				m_listCtrlAlarm.SetSubItemColor(nIndex, nItem, j, ColorWhite, TRUE);
			}
		}
	}

	m_csLock.Unlock();
}
void CDlgViewAlarm::LoadInverterAlarm(int nStationNo)
{	
	m_csLockInverterAlarm.Lock();

	m_listCtrlInverterAlarm.RemoveAllItems();

	sGroupInfo stGroupInfo;

	stGroupInfo.bExpand = TRUE;
	stGroupInfo.strText = "";
	stGroupInfo.bDrawExpand = FALSE;

	int nIndex = m_listCtrlInverterAlarm.InsertGroup(&stGroupInfo);

	CMgrAlarm* pMgr = CMgrAlarm::GetMgr();
	if (pMgr == NULL)
		return;

	for (int i = 0; i < AlarmBitMaxCount; ++i)
	{
		CStringArray arr;

		pMgr->GetStationInverterAlarm(nStationNo,i, arr);

		int nItem = m_listCtrlInverterAlarm.InsertItem(nIndex, _T(""));

		for (int j = 0; j < arr.GetSize(); ++j)
		{
			CString str = arr.GetAt(j);

			m_listCtrlInverterAlarm.SetItemText(nIndex, nItem, j, str);

			if (j > 0 && FALSE == str.IsEmpty())
			{
				m_listCtrlInverterAlarm.SetSubItemColor(nIndex, nItem, j, ColorLightRed, TRUE);
			}
			else if (j > 0 && TRUE == str.IsEmpty())
			{
				m_listCtrlInverterAlarm.SetSubItemColor(nIndex, nItem, j, ColorWhite, TRUE);
			}
		}
	}
	m_csLockInverterAlarm.Unlock();

}
void	CDlgViewAlarm::LoadAllChannelAlarm()
{
	m_csLock.Lock();

	//m_listCtrlAlarm.RemoveAllItems();

	sGroupInfo stGroupInfo;

	stGroupInfo.bExpand = TRUE;
	stGroupInfo.strText = "";
	stGroupInfo.bDrawExpand = FALSE;

	int nIndex = m_listCtrlAlarm.InsertGroup(&stGroupInfo);

	CMgrAlarm* pMgr = CMgrAlarm::GetMgr();
	if (pMgr == NULL)
		return;

	for (int ch = 0; ch < CMgrConfig::GetMgr()->GetChamberChannelCount(); ch++)
	{
		for (int i = 0; i < AlarmBitMaxCount; ++i)
		{
			CStringArray arr;

			pMgr->GetChannelAlarm(ch, i, arr);

			int nItem = m_listCtrlAlarm.InsertItem(nIndex, _T(""));

			for (int j = 0; j < arr.GetSize(); ++j)
			{
				CString str = arr.GetAt(j);

				m_listCtrlAlarm.SetItemText(nIndex, nItem, j, str);

				if (j > 0 && FALSE == str.IsEmpty())
				{
					m_listCtrlAlarm.SetSubItemColor(nIndex, nItem, j, ColorLightRed, TRUE);
				}
				else if (j > 0 && TRUE == str.IsEmpty())
				{
					m_listCtrlAlarm.SetSubItemColor(nIndex, nItem, j, ColorWhite, TRUE);
				}
			}
		}
	}
	m_csLock.Unlock();
}

void	CDlgViewAlarm::LoadSysAlarm(int nChannelNo/* = 0*/)
{
	m_csLockSysAlarm.Lock();

	m_listCtrlSysAlarm.RemoveAllItems();

	sGroupInfo stGroupInfo;

	stGroupInfo.bExpand = TRUE;
	stGroupInfo.strText = "";
	stGroupInfo.bDrawExpand = FALSE;

	int nIndex = m_listCtrlSysAlarm.InsertGroup(&stGroupInfo);
	int Stage1_nIndex = m_listCtrlSysAlarmHistory_Stage1.InsertGroup(&stGroupInfo);

	CMgrAlarm* pMgr = CMgrAlarm::GetMgr();
	CList<CSysAlarm>* pList = pMgr->GetSysAlarmList(nChannelNo);

	if (pList != NULL)
	{
		auto Pos = pList->GetHeadPosition();

		while (Pos)
		{
			CSysAlarm alarm = pList->GetNext(Pos);

			int nItem = m_listCtrlSysAlarm.InsertItem(nIndex, _T(""));
			if (nChannelNo / CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount == 0)
			{
				int nItem_Stage1 = m_listCtrlSysAlarmHistory_Stage1.InsertItem(Stage1_nIndex, _T(""));
				CString strCh = "";

				strCh.Format(_T("%d"), nChannelNo);
				m_listCtrlSysAlarmHistory_Stage1.SetItemText(Stage1_nIndex, nItem_Stage1, 0, alarm.GetAlarmTime());
				m_listCtrlSysAlarmHistory_Stage1.SetItemText(Stage1_nIndex, nItem_Stage1, 1, alarm.GetAlarmTitle());
				m_listCtrlSysAlarmHistory_Stage1.SetItemText(Stage1_nIndex, nItem_Stage1, 2, strCh);
				m_listCtrlSysAlarmHistory_Stage1.SetItemText(Stage1_nIndex, nItem_Stage1, 3, alarm.GetAlarmContents());

			}
			m_listCtrlSysAlarm.SetItemText(nIndex, nItem, 0, alarm.GetAlarmTime());
			m_listCtrlSysAlarm.SetItemText(nIndex, nItem, 1, alarm.GetAlarmTitle());
			m_listCtrlSysAlarm.SetItemText(nIndex, nItem, 2, alarm.GetAlarmContents());
		}
	}

	m_csLockSysAlarm.Unlock();
}


void CDlgViewAlarm::LoadEqAlarm()
{
	m_csLockEQAlarm.Lock();

	m_listCtrlEqAlarm.RemoveAllItems();

	sGroupInfo stGroupInfo;

	stGroupInfo.bExpand = TRUE;
	stGroupInfo.strText = "";
	stGroupInfo.bDrawExpand = FALSE;

	CString strTime;
	CString strCode;

	int nIndex = m_listCtrlEqAlarm.InsertGroup(&stGroupInfo);

	//Eq
	tagVEALARM m_EqAlarm = CMgrAlarm::GetMgr()->GetEqAlarm();
	if (m_EqAlarm.size() != 0)
	{

		for (int i = 0; i < m_EqAlarm.size(); i++)
		{
			int nItem = m_listCtrlEqAlarm.InsertItem(nIndex, _T(""));
			strTime.Format("%s", m_EqAlarm[i].time.Format("%Y.%m.%d %H:%M:%S"));
			strCode.Format("%d", m_EqAlarm[i].code);
			m_listCtrlEqAlarm.SetItemText(nIndex, nItem, 0, strTime);
			m_listCtrlEqAlarm.SetItemText(nIndex, nItem, 1, strCode);
			m_listCtrlEqAlarm.SetItemText(nIndex, nItem, 2, m_EqAlarm[i].text);
			m_listCtrlEqAlarm.SetItemText(nIndex, nItem, 3, m_EqAlarm[i].Comment);
		}
	}

	m_csLockEQAlarm.Unlock();

}

//알람 상태에 따른 업데이트 함수 (Channel, FormationDlg, 
void CDlgViewAlarm::UpdateStatus(bool bInitialize)
{
	m_csLockChannel.Lock();

	if (bInitialize == true)
	{
		m_listCtrlChannel.RemoveAllItems();
		m_listCtrlStation.RemoveAllItems();
	}

	sGroupInfo stGroupInfo;

	stGroupInfo.bExpand = TRUE;
	stGroupInfo.strText = "";
	stGroupInfo.bDrawExpand = FALSE;

	//Channel
	int nIndex = 0;
	if (bInitialize == true)
	{
		/*if (m_listCtrlChannel.empty == NULL)
			return;*/
		nIndex = m_listCtrlChannel.InsertGroup(&stGroupInfo);

	}
	
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (bInitialize == true)
		{
			m_listCtrlChannel.InsertItem(nIndex, _T(""));
		}

		//CHANNEL
		CString strTemp;
		strTemp.Empty();
		strTemp.Format(_T("%d"), pChannel->GetChannelNumber() + 1);
		m_listCtrlChannel.SetItemText(nIndex, pChannel->GetChannelNumber(), 0, _T(strTemp.GetString()));


		//ALARM STATUS
		CAlarmMap* pMapAlarm = CMgrAlarm::GetMgr()->GetAlarmMap(pChannel->GetChannelNumber());
		strTemp.Empty();
		if (pMapAlarm)
		{
			if (TRUE == pMapAlarm->ExistAlarm())
			{
				strTemp.Format(_T("ERROR"));
				m_listCtrlChannel.SetSubItemColor(nIndex, pChannel->GetChannelNumber(), 0, ColorLightRed, TRUE);
				m_listCtrlChannel.SetSubItemColor(nIndex, pChannel->GetChannelNumber(), 1, ColorLightRed, TRUE);
			}
			else
			{
				strTemp.Format(_T("NORMAL"));
			}
		}

		//lyw4_1
		//SYSTEM ALARM STATUS
		CList< CSysAlarm >* pListSysAlarm = CMgrAlarm::GetMgr()->GetSysAlarmList(pChannel->GetChannelNumber());
		if (pListSysAlarm != NULL && pListSysAlarm->GetCount() > 0)
		{
			strTemp.Format(_T("SYSTEM"));
			m_listCtrlChannel.SetSubItemColor(nIndex, pChannel->GetChannelNumber(), 0, ColorPurple, TRUE);
			m_listCtrlChannel.SetSubItemColor(nIndex, pChannel->GetChannelNumber(), 1, ColorPurple, TRUE);
		}
		else
		{
			if (strTemp == _T("NORMAL"))
			{
				m_listCtrlChannel.SetSubItemColor(nIndex, pChannel->GetChannelNumber(), 0, ColorWhite, TRUE);
				m_listCtrlChannel.SetSubItemColor(nIndex, pChannel->GetChannelNumber(), 1, ColorWhite, TRUE);
			}
		}
		//lyw4_1--------------------------------------------------
				
		m_listCtrlChannel.SetItemText(nIndex, pChannel->GetChannelNumber(), 1, _T(strTemp.GetString()));
	}
	m_csLockChannel.Unlock();
	m_csLockInverterAlarm.Lock();

	CList< CPCanCom*, CPCanCom* >& listPCan = CMgrPCan::GetMgr()->GetComList();

	nIndex = m_listCtrlStation.InsertGroup(&stGroupInfo);

	auto Pos_Can = listPCan.GetHeadPosition();

	while (Pos_Can)
	{
		CPCanCom* pPCanCom = listPCan.GetNext(Pos_Can);

		if (NULL == pPCanCom)
			continue;

		if (bInitialize == true)
			m_listCtrlStation.InsertItem(nIndex, _T(""));

		CString strTemp;
		strTemp.Empty();
		strTemp.Format(_T("%d"), pPCanCom->GetChannelNo() + 1);
		m_listCtrlStation.SetItemText(nIndex, pPCanCom->GetChannelNo(), 0, _T(strTemp.GetString()));


		//ALARM STATUS
		CAlarmInverterMap* pMapInverterAlarm = CMgrAlarm::GetMgr()->GetInverterAlarmMap(pPCanCom->GetChannelNo());

		strTemp.Empty();
		if (pMapInverterAlarm)
		{
			if (TRUE == pMapInverterAlarm->ExistAlarm())
			{
				strTemp.Format(_T("ERROR"));
				m_listCtrlStation.SetSubItemColor(nIndex, pPCanCom->GetChannelNo(), 0, ColorLightRed, TRUE);
				m_listCtrlStation.SetSubItemColor(nIndex, pPCanCom->GetChannelNo(), 1, ColorLightRed, TRUE);
			}
			else
			{
				strTemp.Format(_T("NORMAL"));
			}
		}		

		m_listCtrlStation.SetItemText(nIndex, pPCanCom->GetChannelNo(), 1, _T(strTemp.GetString()));
	}

	m_csLockInverterAlarm.Unlock();
	LoadEqAlarm();
	LoadSysAlarm(m_nCurSel); //lyw4_1

	LoadAlarmComment();
	//LoadInverterAlarm(0);

	m_listCtrlChannel.Invalidate(false);
	m_listCtrlStation.Invalidate(false);

	ShowWindow(SW_HIDE);
}

void CDlgViewAlarm::LoadAlarmComment()
{
	m_csLockAlarmComment.Lock();

	m_listCtrlAlarmComment.RemoveAllItems();

	sGroupInfo stGroupInfo;

	stGroupInfo.bExpand = TRUE;
	stGroupInfo.strText = "";
	stGroupInfo.bDrawExpand = FALSE;

	int nIndex = m_listCtrlAlarmComment.InsertGroup(&stGroupInfo);
	CMgrAlarm* pMgr = CMgrAlarm::GetMgr();
	if (pMgr == NULL)
		return;

	int nChannelStart = 0;
	int nChannelEnd = 0;
	int nChannelCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;


	bool bSubStage1Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage1Use;
	bool bSubStage2Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage2Use;

	if (bSubStage1Use == true && bSubStage2Use == true)
	{
		nChannelStart = 0;
		nChannelEnd = nChannelCount * 2;
	}
	else if (bSubStage1Use == true)
	{
		nChannelStart = 0;
		nChannelEnd = nChannelCount;
	}
	else if (bSubStage2Use == true)
	{
		nChannelStart = nChannelCount;
		nChannelEnd = nChannelCount+ nChannelStart;
	}

	//Channel
	for (int i = 0; i < AlarmBitMaxCount; ++i)
	{
		CStringArray arr;
		for (int nChannelNo = nChannelStart; nChannelNo < nChannelEnd; nChannelNo++)
		{
			if (pMgr->ExistChannelAlarm(nChannelNo) != true)
				continue;
			
			pMgr->GetChannelAlarmComment(nChannelNo, i, arr);

			int nItem = m_listCtrlAlarmComment.InsertItem(nIndex, _T(""));

			for (int j = 0; j < arr.GetSize(); ++j)
			{
				CString str = arr.GetAt(j);
				if (j == 0)
				{
					str = "IF";
				}

				m_listCtrlAlarmComment.SetItemText(nIndex, nItem, j, str);				

				if (j > 0 && FALSE == str.IsEmpty())
				{
					m_listCtrlAlarmComment.SetSubItemColor(nIndex, nItem, j, ColorLightRed, TRUE);
				}
				else if (j > 0 && TRUE == str.IsEmpty())
				{
					m_listCtrlAlarmComment.SetSubItemColor(nIndex, nItem, j, ColorWhite, TRUE);
				}
			}
		}
	}		

	nIndex = m_listCtrlAlarmComment.InsertGroup(&stGroupInfo);
	//Inverter
	for (int i = 0; i < AlarmBitMaxCount; ++i)
	{
		CStringArray arr;
		for (int nStationNo = nChannelStart / nChannelCount; nStationNo < (nChannelStart / nChannelCount) + 1; nStationNo++)
		{
			if (pMgr->ExistInverterAlarm(nStationNo) != true)
				continue;
			pMgr->GetStationInverterAlarmComment(nStationNo, i, arr);

			int nItem = m_listCtrlAlarmComment.InsertItem(nIndex, _T(""));

			for (int j = 0; j < arr.GetSize(); ++j)
			{
				CString str = arr.GetAt(j);
				if (j == 0)
				{
					str = "Inverter";
				}
				m_listCtrlAlarmComment.SetItemText(nIndex, nItem, j, str);

				if (j > 0 && FALSE == str.IsEmpty())
				{
					m_listCtrlAlarmComment.SetSubItemColor(nIndex, nItem, j, ColorLightRed, TRUE);
				}
				else if (j > 0 && TRUE == str.IsEmpty())
				{
					m_listCtrlAlarmComment.SetSubItemColor(nIndex, nItem, j, ColorWhite, TRUE);
				}
			}
		}
	
	}
	nIndex = m_listCtrlAlarmComment.InsertGroup(&stGroupInfo);

	//System
	for (int nChannelNo = nChannelStart; nChannelNo < nChannelEnd; nChannelNo++)
	{
		CList<CSysAlarm>* pList = pMgr->GetSysAlarmList(nChannelNo);

		if (pList != NULL)
		{
			auto Pos = pList->GetHeadPosition();

			while (Pos)
			{
				CSysAlarm alarm = pList->GetNext(Pos);

				int nItem = m_listCtrlAlarmComment.InsertItem(nIndex, _T(""));

				m_listCtrlAlarmComment.SetItemText(nIndex, nItem, 0, "System");
				m_listCtrlAlarmComment.SetItemText(nIndex, nItem, 1, alarm.GetAlarmTitle());
				m_listCtrlAlarmComment.SetItemText(nIndex, nItem, 2, alarm.GetAlarmContents());
			}
		}
	}
	//EQ
	nIndex = m_listCtrlAlarmComment.InsertGroup(&stGroupInfo);

	//Eq
	tagVEALARM m_EqAlarm = CMgrAlarm::GetMgr()->GetEqAlarm();
	if (m_EqAlarm.size() != 0)
	{
		for (int i = 0; i < m_EqAlarm.size(); i++)
		{
			int nItem = m_listCtrlAlarmComment.InsertItem(nIndex, _T(""));
			m_listCtrlAlarmComment.SetItemText(nIndex, nItem, 0, "EQ");
			m_listCtrlAlarmComment.SetItemText(nIndex, nItem, 1, m_EqAlarm[i].text);
			m_listCtrlAlarmComment.SetItemText(nIndex, nItem, 2, m_EqAlarm[i].Comment);
		}
	}


	m_csLockAlarmComment.Unlock();
}

void	CDlgViewAlarm::CheckAlarmExist()
{

}

void CDlgViewAlarm::ClearSysAlarmHisotry(int nStageNo)
{
	if (nStageNo == 0)
	{
		m_listCtrlSysAlarmHistory_Stage1.RemoveAllItems();
	}
}

void CDlgViewAlarm::OnBnClickedButtonAlarmClearAll()
{
	//lyw4_1 : 모든 채널 시스템 알람 초기화
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	CMgrAlarm* pMgr = CMgrAlarm::GetMgr();
	auto Pos = listChannel.GetHeadPosition();	
	CMgrPCan* pMgrPCan = CMgrPCan::GetMgr();
	CDlgMainTitleFormation* pDlg = CDlgMainTitleFormation::GetMgr();

	if (NULL == pDlg)
		return ;

	pDlg->UI_Alarm(false);
	
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		CAlarmMap* lpAlarmMap = pMgr->GetAlarmMap(pChannel->GetChannelNumber());

		if (!lpAlarmMap)
			continue;

		if (pChannel->GetChannelStatus() == eCyclerChannelStateError)
		{
			pChannel->SendCommand(eIFBoardCommandAlarmRequest);
			Sleep(100);

			// 알람종류중 4가지 종류중에 하나라도 알람이 켜져야 알람 클리어
			CMgrAlarm* pMgr = CMgrAlarm::GetMgr();
			if (pMgr == NULL)
				continue;

			for (int bitNumber = 0; bitNumber < 16/*AlarmBitMaxCount*/; ++bitNumber) {
				CStringArray arr;
				auto chNum = pChannel->GetChannelNumber();

				pMgr->GetChannelAlarm(chNum, bitNumber, arr);
				for (int alarmKind = 1; alarmKind < arr.GetSize(); ++alarmKind) {
					CString str = arr.GetAt(alarmKind);
					if (!str.IsEmpty()) {
						pChannel->SendCommand(eIFBoardCommandAlarmClear);
						Sleep(100);
						pChannel->SendCommand(eIFBoardCommandAlarmRequest);
						break;
					}
				}
			}
		}		

		
		CMgrAlarm::GetMgr()->ClearSysAlarm(pChannel->GetChannelNumber());
	}
	
	if (pMgrPCan->GetStation(0) != NULL)
	{
		if (CMgrPCan::GetMgr()->GetStation(0)->GetAlarmStatus() == true)
		{
			CMgrPCan::GetMgr()->GetStation(0)->Function_AlarmClear();
		}
	}
	
	if (pMgrPCan->GetStation(1) != NULL)
	{
		if (CMgrPCan::GetMgr()->GetStation(1)->GetAlarmStatus() == true)
		{
			CMgrPCan::GetMgr()->GetStation(1)->Function_AlarmClear();
		}
	}

	CMgrAlarm::GetMgr()->ClearEqAlarm();


	UpdateStatus(true);
}


void CDlgViewAlarm::OnBnClickedButtonAlarmClear()
{
	//lyw4_1 : 현재 채널 시스템 알람 초기화
	CMgrAlarm::GetMgr()->ClearSysAlarm(m_nCurSel);
	CMgrAlarm::GetMgr()->ClearEqAlarm();
	//CSockProc::GetMgr()->SendBoardTypePLC_Formation(false);

	ShowWindow(SW_HIDE);

	UpdateStatus(true);
}

void CDlgViewAlarm::Destory_UI()
{
	//m_Btn_AllAlarmClear.
}
