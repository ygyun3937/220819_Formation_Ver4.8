// DlgViewChannelCell.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "DlgViewChannelCell.h"
#include "afxdialogex.h"

#include "MgrChannel.h"
#include "PnxUtil.h"
#include "CMgrAccount.h"
#include "SockProc.h"
#include "MgrLog.h"
#include "MgrComm.h"
#include "MgrCommData.h"

#define		ListCtrlRowSize			( 30 )
#define		ChannelUpdateTimerId	( 1025 )
#define		OneSecond				( 1000 )
#define     COMPLETE_INIT_DIALOG	(WM_USER + 1000)

#pragma warning(disable:4100)

// CDlgViewChannelCell 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgViewChannelCell, CDialogEx)

CDlgViewChannelCell::CDlgViewChannelCell(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgViewChannelCell::IDD, pParent)
{
	m_nWindowWidth = 0;
}

CDlgViewChannelCell::~CDlgViewChannelCell()
{
}

void CDlgViewChannelCell::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgViewChannelCell, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_MESSAGE(WM_GROUP_SELECT, OnReceiveListCtrlSelect)
	ON_MESSAGE(WM_GROUP_RCLICK, OnReceiveListCtrlRClick)
	ON_MESSAGE(COMPLETE_INIT_DIALOG, CompleteInitDialog)
END_MESSAGE_MAP()


// CDlgViewChannelCell 메시지 처리기입니다.


BOOL CDlgViewChannelCell::Create(CWnd* pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}


BOOL CDlgViewChannelCell::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


BOOL CDlgViewChannelCell::OnInitDialog()
{
	__super::OnInitDialog();

	InitCtrl();

	InitListCtrlChannel();

	LoadChannel();

	SetTimer(ChannelUpdateTimerId, OneSecond / 10, NULL);

	PostMessage(COMPLETE_INIT_DIALOG);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgViewChannelCell::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;

	GetClientRect(&rt);

	pDC->FillSolidRect(&rt, ColorWhite);

	return TRUE;
}

void CDlgViewChannelCell::OnTimer(UINT_PTR nIDEvent)
{
	if (ChannelUpdateTimerId == nIDEvent)
	{
		DrawChannel();
	}

	__super::OnTimer(nIDEvent);
}


void CDlgViewChannelCell::OnDestroy()
{
	__super::OnDestroy();

	KillTimer(ChannelUpdateTimerId);
}

void CDlgViewChannelCell::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	m_btnChannel.SetBkDC(&dc);
}


void	CDlgViewChannelCell::InitCtrl()
{
	SKINBTN(m_btnChannel, IDC_BTN_VIEW_CHANNEL_SELECT_CHANNEL, this, GetExePath() + _T("\\res\\Channel.png"), GetExePath() + _T("\\res\\Channel.png"));

	CST_DEFAULT(m_ctrlChannelDesc, IDC_STATIC_VIEW_CHANNEL_CHANNEL_DESC, this, 12, 0, TRUE, ColorWhite, ColorLightBlack, "선택 채널");
	CST_DEFAULT(m_ctrlChannelName, IDC_STATIC_VIEW_CHANNEL_CHANNEL_NAME, this, 12, 0, TRUE, ColorWhite, ColorLightBlack, _T(""));
}

void	CDlgViewChannelCell::InitListCtrlChannel()
{
	// List Control 생성
	m_listCtrlChannel.Create(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_GROUP_LIST_VIEW_CHANNEL_CELL, NULL);
	m_listCtrlChannel.CreateScrollBar(IDC_GROUP_SCROLL_VIEW_CHANNEL_CELL);
	//m_listCtrlChannel.SetUserInterlock(true);

	CRect rtOriginal;
	GetWindowRect(&rtOriginal);

	CRect rtScreen = rtOriginal;
	ScreenToClient(&rtScreen);

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{		
		int nHeight = (ListCtrlRowSize * (pMgr->GetSize() + 1)) + rtScreen.top + 50;
		int nWidth = GetSystemMetrics(SM_CXSCREEN);

		SetWindowPos(NULL, 0, 0, GetSystemMetrics(SM_CXSCREEN) - rtOriginal.left, nHeight, SWP_NOZORDER);

		m_listCtrlChannel.MoveWindow(rtScreen.left + 2, rtScreen.top + 50, nWidth - (rtOriginal.left * 2) - 2, nHeight);
		m_listCtrlChannel.SetGroupHeight(0);
		m_listCtrlChannel.SetFontSizeTitle(16);
		m_listCtrlChannel.SetFontSizeEx(18);

		m_nWindowWidth = nWidth - (rtOriginal.left * 2) - 2;
		m_nWindowWidth -= ::GetSystemMetrics(SM_CXVSCROLL);
	}

	int nColumnMaxSize = GetColumnMaxSize();

	CMgrAccount* pMgrAccount = CMgrAccount::GetMgr();

	if (pMgrAccount)
	{
		sCustomView* lpView = pMgrAccount->GetCustomView();

		if (!lpView)
			return;

		int nColumnIndex = 0;
		int nColumnTotalWidth = 0;

		auto iter = lpView->vecViewItem.begin();
		auto iterE = lpView->vecViewItem.end();

		while (iter != iterE)
		{
			const sChannelView* lpChanelView = (*iter);

			if (lpChanelView && lpChanelView->nWidth > 0)
			{
				int nColumnWidth = ((int)lpView->vecViewItem.size() <= nColumnIndex + 1) ? m_nWindowWidth - nColumnTotalWidth - 2 : (m_nWindowWidth * lpChanelView->nWidth) / nColumnMaxSize;

				//m_listCtrlChannel.InsertColumn(lpChanelView->szName, lpChanelView->nWidth, TEXT_CENTER);
				m_listCtrlChannel.InsertColumn(lpChanelView->szName, nColumnWidth, TEXT_CENTER);
				m_listCtrlChannel.SetItemHeight(ListCtrlRowSize);

				//nColumnTotalWidth += nColumnWidth;
				nColumnIndex++;
			}

			++iter;
		}
	}
}

int		CDlgViewChannelCell::GetColumnMaxSize()
{
	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (pMgr)
	{
		sCustomView* pView = pMgr->GetCustomView();

		if (nullptr == pView)
			return 0;

		auto iter = pView->vecViewItem.begin();
		auto iterE = pView->vecViewItem.end();

		int nTotalWidth = 0;

		while (iter != iterE)
		{
			const sChannelView* lpChanelView = (*iter);

			if (lpChanelView)
				nTotalWidth += lpChanelView->nWidth;

			++iter;
		}

		return nTotalWidth;
	}

	return 0;
}

int		CDlgViewChannelCell::GetItemIndex(TCHAR* lpszName)
{
	CMgrAccount* pMgr = CMgrAccount::GetMgr();

	if (pMgr)
	{
		sCustomView* pView = pMgr->GetCustomView();

		if (nullptr == pView)
			return -1;

		auto iter = pView->vecViewItem.begin();
		auto iterE = pView->vecViewItem.end();

		while (iter != iterE)
		{
			const sChannelView* lpChanelView = (*iter);

			if (lpChanelView)
			{
				if (lpChanelView->nWidth > 0 && strcmp(lpChanelView->szHeader, lpszName) == 0)
				{
					return lpChanelView->nSerialNo - 1;
				}
			}

			++iter;
		}
	}

	return -1;
}

void	CDlgViewChannelCell::LoadChannel()
{
	m_listCtrlChannel.RemoveAllItems();

	sGroupInfo stGroupInfo;

	stGroupInfo.bExpand = TRUE;
	stGroupInfo.strText = "";
	stGroupInfo.bDrawExpand = FALSE;
	stGroupInfo.bBattery = TRUE;

	int nIndex = m_listCtrlChannel.InsertGroup(&stGroupInfo);

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	CList< CChannel*, CChannel* >& listChannel = pMgr->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listChannel.GetNext(Pos);

		if(!lpCyclerChannel)
			continue;

		int nItem = m_listCtrlChannel.InsertItem(nIndex, _T(""));

		std::vector<SViewData> vecViewData = lpCyclerChannel->GetViewData();

		auto iter = vecViewData.begin();
		auto iterE = vecViewData.end();

		while (iter != iterE)
		{
			SViewData sData = (*iter);

			int nSubItem = GetItemIndex(sData.szName);

			if (nSubItem >= 0)
				m_listCtrlChannel.SetItemText(nIndex, nItem, nSubItem, sData.szData);

			if (strcmp(sData.szName, "Channel") == 0)
				m_listCtrlChannel.SetBatteryStatusIndex(nSubItem);

			++iter;
		}
	}
}

void	CDlgViewChannelCell::DrawChannel()
{
	m_csLock.Lock();

	CMgrAccount* pMgrAccount = CMgrAccount::GetMgr();

	if (pMgrAccount && true == pMgrAccount->IsModified())
	{
		pMgrAccount->SetModified(false);

		sCustomView* pView = pMgrAccount->GetCustomView();

		if (nullptr == pView)
			return;

		int nColumnMaxSize = GetColumnMaxSize();

		int nColumnIndex = 0;
		int nColumnTotalWidth = 0;

		auto iter = pView->vecViewItem.begin();
		auto iterE = pView->vecViewItem.end();

		while (iter != iterE)
		{
			const sChannelView* lpChanelView = (*iter);

			if (lpChanelView && lpChanelView->nWidth > 0)
			{
				int nColumnWidth = ((int)pView->vecViewItem.size() <= nColumnIndex + 1) ? m_nWindowWidth - nColumnTotalWidth - 2 : (m_nWindowWidth * lpChanelView->nWidth) / nColumnMaxSize;

				m_listCtrlChannel.EditColumn(lpChanelView->szName, nColumnWidth, TEXT_CENTER, nColumnIndex);

				nColumnTotalWidth += nColumnWidth;
				nColumnIndex++;
			}

			++iter;
		}
	}


	int nCyclerChannelIndex = 0;

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	CList< CChannel*, CChannel* >& listChannel = pMgr->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		std::vector<SViewData> vecViewData = lpCyclerChannel->GetViewData();

		m_listCtrlChannel.SetItemFlag(0, nCyclerChannelIndex, lpCyclerChannel->IsReserved(), lpCyclerChannel->IsMultiSchedule());

		auto it = vecViewData.begin();
		auto itE = vecViewData.end();

		COLORREF colorConnect = ColorGray;

		while (it != itE)
		{
			SViewData sData = (*it);

			// Add By JSK - 2020.06.11
			int nSubItem = GetItemIndex(sData.szName);

			if (nSubItem >= 0)
			{
				if (strcmp(sData.szName, "Channel") == 0)
				{
					colorConnect = ColorGray;

					CSockProc* lpDevice = CSockProc::GetMgr();

					if (lpDevice)
					{
						CClientSocket* lpIFBoard = lpDevice->GetAtChannel(lpCyclerChannel->GetChannelNumber());

						if (lpIFBoard)
						{
							if (lpIFBoard->GetConnect())
								colorConnect = ColorWhite;
						}
					}

					m_listCtrlChannel.SetSubItemColor(0, nCyclerChannelIndex, nSubItem, colorConnect, TRUE, lpCyclerChannel->GetChannelStatus());
					m_listCtrlChannel.SetBatteryStatusIndex(nSubItem);
				}
				else if (strcmp(sData.szName, "Status") == 0)
				{
					m_listCtrlChannel.SetSubItemColor(0, nCyclerChannelIndex, nSubItem, Status2Color(sData.szData), TRUE);
				}
				else if (strcmp(sData.szName, "Mode") == 0)
				{
					m_listCtrlChannel.SetSubItemColor(0, nCyclerChannelIndex, nSubItem, RunMode2Color(sData.szData), TRUE);
				}

				m_listCtrlChannel.SetItemText(0, nCyclerChannelIndex, nSubItem, sData.szData);
			}

			++it;
		}

		nCyclerChannelIndex++;
	}

	m_csLock.Unlock();
}

COLORREF	CDlgViewChannelCell::Status2Color(CString strStatus)
{

	for (int i = 0; i < eCyclerChannelStateMaxCount; ++i)
	{
		if (_tcsicmp(strStatus, g_lpszCyclerStateString[i]) == 0)
		{
			switch (i)
			{
			case eCyclerChannelStateIdle: return ColorWhite;
			case eCyclerChannelStateReady: return ColorLightGray;
			case eCyclerChannelStateStart: return ColorLightGreen;
			case eCyclerChannelStateComplete: return ColorLightBlue;
			case eCyclerChannelStateError: return ColorLightRed;
			case eCyclerChannelStatePause:
			case eCyclerChannelStatePauseFromDwell:
			case eCyclerChannelStatePauseFromWaitChamber:
			case eCyclerChannelStatePauseFromTempSync:
			case eCyclerChannelStatePauseFromStepSync: return ColorPurple;
			}
		}
	}

	return ColorWhite;
}

COLORREF	CDlgViewChannelCell::OpCode2Color(CString strOpCode)
{
		 if (strOpCode.CompareNoCase(_T("None")) == 0)				return ColorGray;
	else if (strOpCode.CompareNoCase(_T("Input Relay On")) == 0)	return ColorBlue;
	else if (strOpCode.CompareNoCase(_T("Output Relay On")) == 0)	return ColorYellow;
	else if (strOpCode.CompareNoCase(_T("Charge CC")) == 0)			return ColorLightGreen;
	else if (strOpCode.CompareNoCase(_T("Charge CV")) == 0)			return ColorLightGreen;
	else if (strOpCode.CompareNoCase(_T("Charge CP")) == 0)			return ColorLightGreen;
	else if (strOpCode.CompareNoCase(_T("Discharge CC")) == 0)		return ColorLightRed;
	else if (strOpCode.CompareNoCase(_T("Discharge CV")) == 0)		return ColorLightRed;
	else if (strOpCode.CompareNoCase(_T("Discharge CP")) == 0)		return ColorLightRed;
	else if (strOpCode.CompareNoCase(_T("Rest")) == 0)				return ColorBlue;

	return ColorWhite;
}

COLORREF	CDlgViewChannelCell::RunMode2Color(CString strRunMode)
{
		 if (strRunMode.CompareNoCase(_T("REST")) == 0)			return ColorLightBlack;
	else if (strRunMode.CompareNoCase(_T("CHARGE")) == 0)		return ColorSlateblue;
	else if (strRunMode.CompareNoCase(_T("DISCHARGE")) == 0)	return ColorWhiteYellow;
	else if (strRunMode.CompareNoCase(_T("PATTERN")) == 0)		return ColorYellow;
	else if (strRunMode.CompareNoCase(_T("IMPADANCE")) == 0)	return ColorYellow;

	return ColorWhite;
}

LRESULT	CDlgViewChannelCell::OnReceiveListCtrlSelect(WPARAM wParam, LPARAM lParam)
{
	CString strSelectChannel = (LPSTR)(LPCTSTR)wParam;
	
	CStringArray arr;

	CPnxUtil::GetMgr()->SplitString(strSelectChannel, ',', arr);

	CString strText;

	int nChannelNumber = -1;

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nSelectChannel = atoi(arr.GetAt(i))/* + nIdx*/;

		CString str;
		str.Format(_T("채널 %d,"), nSelectChannel+1);		

		strText += str;

		if (nChannelNumber < 0)
			nChannelNumber = nSelectChannel;
	}

	strText.TrimRight(_T(","));

	CMgrChannel::GetMgr()->SetChannelSelected(arr/*, nIdx*/);

	m_ctrlChannelName.SetWindowText(strText);

	wParam = NULL;

	return 0;
}

LRESULT CDlgViewChannelCell::OnReceiveListCtrlRClick(WPARAM wParam, LPARAM lParam)
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return 0;

	int nSelectedCyclerChannel = pMgr->GetSelectCyclerChannelCount();

	if (nSelectedCyclerChannel >= 2)
	{
		CPoint point;

		GetCursorPos(&point);

		// 메뉴 중 2번째 Column 즉, Command 부분만 추출
		CMenu* pMenu = AfxGetApp()->GetMainWnd()->GetMenu();

		if (pMenu)
		{
			CMenu* pSubMenu = pMenu->GetSubMenu(eMenuTypeCommand);

			if (pSubMenu)
				pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		}

		return 0;
	}

	int nSelectCyclerChannel = (int)wParam;
	//nSelectCyclerChannel += 1;

	pMgr->SetCyclerChannelSelect(nSelectCyclerChannel);

	CString strSelectedCyclerChannel;
	strSelectedCyclerChannel.Format("채널 %d", nSelectCyclerChannel + 1);

	m_ctrlChannelName.SetWindowText(strSelectedCyclerChannel);


	// 현재 마우스 커서 위치 받아옴
	CPoint point;

	GetCursorPos(&point);

	// 메뉴 중 2번째 Column 즉, Command 부분만 추출
	CMenu* pMenu = AfxGetApp()->GetMainWnd()->GetMenu();

	if (pMenu)
	{
		CMenu* pSubMenu = pMenu->GetSubMenu(eMenuTypeCommand);

		if (pSubMenu)
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}

	return 0;
}

BOOL FileExist(CString str)
{
	WIN32_FIND_DATAA FindData;
	HANDLE file = FindFirstFile(str, &FindData);

	if (file == INVALID_HANDLE_VALUE) {
		FindClose(file);
		return FALSE;
	}
	FindClose(file);
	return TRUE;

}

LRESULT CDlgViewChannelCell::CompleteInitDialog(WPARAM w, LPARAM l)
{
	CString specificationPath = _T("");
	CString findChannel = "";
	BOOL bFind = FALSE;
	
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	auto Pos = listChannel.GetHeadPosition();
	

	while (Pos)	{
		CChannel* pChannel = listChannel.GetNext(Pos);
		if (NULL == pChannel) {
			continue;
		}
			
		int dChannelNum = pChannel->GetChannelNumber();
		specificationPath.Format(_T("%s\\Config\\Specification\\CH%02d.ini"), GetExePath(), dChannelNum + 1);

		if (!FileExist(specificationPath)) {
			continue;
		}

		CString endCondition = INIReadStr("Specification", "EndCondition", specificationPath);
		if (endCondition.IsEmpty()) {
			findChannel.AppendFormat("%02d ", dChannelNum + 1);
			bFind = TRUE;
				
			auto fileName = INIReadStr("Specification", "FileName", specificationPath);
			auto scheduleName = INIReadStr("Specification", "ScheduleName", specificationPath);
			auto logFolderName = INIReadStr("Specification", "LogFolderName", specificationPath);
			auto syncMode = INIReadInt("Specification", "SyncMode", specificationPath);
			auto userID = INIReadStr("Specification", "UserID", specificationPath);
			auto workName = INIReadStr("Specification", "WorkName", specificationPath);

			pChannel->SetFileName(fileName);
			pChannel->SetScheduleName(scheduleName);
			pChannel->SetLogFolderName(logFolderName);
			pChannel->SetSyncMode(syncMode);
			pChannel->SetUserID(userID);
			pChannel->SetWorkName(workName);
						
			CString strLogPath;
			strLogPath.Format("%s%dCH\\", logFolderName, dChannelNum + 1);
			
			CMgrLog* pMgr = CMgrLog::GetMgr();
			pMgr->SetCurrentLogPath(dChannelNum, strLogPath);

			auto stepTime = INIReadInt("Specification", "StepTime", specificationPath);
			auto totalTime = INIReadInt("Specification", "TotalTime", specificationPath);
			CMgrCommData* pMgrComm = CMgrCommData::GetMgr();
			CCommData* pComm = pMgrComm->GetAt(dChannelNum);
			pComm->SetStepTime(stepTime);
			pComm->SetTotalTime(totalTime);

			auto cycle = INIReadInt("Specification", "CycleNumber", specificationPath);
			auto cycleIndex = INIReadInt("Specification", "CycleIndex", specificationPath);
			auto step = INIReadInt("Specification", "StepNumber", specificationPath);
			auto stepIndex = INIReadInt("Specification", "StepIndex", specificationPath);
			g_nCycleLoopStartSet[cycle - 1] = cycleIndex;
		}
	}

	//if (bFind) {
	//	CString message;
	//	message.Format("%s채널에서 비정상 종료 발견", findChannel);
	//	AfxMessageBox(message);
	//}
	

	return 0;
}


