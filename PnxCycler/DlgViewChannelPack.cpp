// DlgViewChannelPack.cpp : 구현 파일입니다.
//

// DlgViewChannelCell.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "DlgViewChannelPack.h"
#include "afxdialogex.h"

#include "MgrChannel.h"
#include "PnxUtil.h"
#include "MgrConfig.h"
#include "DlgViewDAQ.h"
#include "MgrOption.h"
#include "MgrLog.h"
#include "MgrCommData.h"

#include "NewCellTypes/GridURLCell.h"
#include "NewCellTypes/GridCellCombo.h"
#include "NewCellTypes/GridCellCheck.h"
#include "NewCellTypes/GridCellNumeric.h"
#include "NewCellTypes/GridCellDateTime.h"

#define		CHANNEL_STATUS_UPDATE_TIMER_ID	( 1025 )
#define		TIMER_SET_1S					( 1000 )
#define     COMPLETE_INIT_DIALOG			(WM_USER + 1000)

#pragma warning(disable:4100)

// CDlgViewChannelCell 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgViewChannelPack, CDialogEx)

CDlgViewChannelPack::CDlgViewChannelPack(CWnd* pParent /*=NULL*/)
: CDialogEx(CDlgViewChannelPack::IDD, pParent)
{

}

CDlgViewChannelPack::~CDlgViewChannelPack()
{
}

void CDlgViewChannelPack::SetChannelDisconnect(int idx, bool bDisconnect)		// 연결 끊긴 채널 빨간색 처리
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();
	if (idx >= pMgr->GetSize())
		return;
	
	
	for (int nCol = 0; nCol < m_strArrTitle.GetCount(); nCol++)
	{
		if (m_strArrTitle[nCol] == "Ch")
		{
			if (bDisconnect)
				m_Grid.SetItemBkColour(idx + 1, 0, ColorRed);
			else
				m_Grid.SetItemBkColour(idx + 1, 0, ColorGreen);

			m_Grid.RedrawColumn(nCol);
			break;
		}
	}
}


void CDlgViewChannelPack::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CDlgViewChannelPack, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
	ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
	ON_NOTIFY(NM_RCLICK, IDC_GRID, OnGridRClick)
	ON_NOTIFY(GVN_BEGINLABELEDIT, IDC_GRID, OnGridStartEdit)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
	ON_NOTIFY(GVN_SELCHANGING, IDC_GRID, OnGridStartSelChange)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID, OnGridEndSelChange)
	ON_NOTIFY(GVN_BEGINDRAG, IDC_GRID, OnGridBeginDrag)
	ON_WM_SIZE()
	ON_MESSAGE(COMPLETE_INIT_DIALOG, CompleteInitDialog)
END_MESSAGE_MAP()


// CDlgViewChannelCell 메시지 처리기입니다.


BOOL CDlgViewChannelPack::Create(CWnd* pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}


BOOL CDlgViewChannelPack::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


BOOL CDlgViewChannelPack::OnInitDialog()
{
	__super::OnInitDialog();

	LoadDrawINI();

	InitCtrl();

	InitListCtrlChannel();

	LoadChannel();

	PostMessage(COMPLETE_INIT_DIALOG);

	CRect rect;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	CRect rc;
	m_btnChannel.GetWindowRect(&rc);
	SetWindowPos(NULL, 0, 0, rect.Width(), m_Grid.GetFixedRowHeight()*m_nGridRowSize + rc.Height(), SWP_NOZORDER);
	SetTimer(CHANNEL_STATUS_UPDATE_TIMER_ID, TIMER_SET_1S / 10, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgViewChannelPack::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;

	pDC->GetClipBox(&rt);
	pDC->FillSolidRect(&rt, ColorWhite);

	return TRUE;
}

void CDlgViewChannelPack::OnTimer(UINT_PTR nIDEvent)
{
	if (CHANNEL_STATUS_UPDATE_TIMER_ID == nIDEvent)
	{
		DrawChannel();
	}

	__super::OnTimer(nIDEvent);
}


void CDlgViewChannelPack::OnDestroy()
{
	__super::OnDestroy();

	KillTimer(CHANNEL_STATUS_UPDATE_TIMER_ID);
}

void CDlgViewChannelPack::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	m_btnChannel.SetBkDC(&dc);
}

void CDlgViewChannelPack::LoadDrawINI()
{
	CStringArray strAry;
	CString strFilePath; strFilePath.Empty();
	CString strAppName; strAppName.Empty();

	strFilePath.Format(_T("%s\\Config\\%s"), GetExePath(), _T("Draw.ini"));
	strAppName.Format(_T("ViewChannel"));

	m_nGridRowSize = INIReadInt(strAppName, _T("ListCtrlRowSize"), strFilePath);

	CPnxUtil::GetMgr()->SplitString(_T(INIReadStr(strAppName, _T("GridTitle"), strFilePath)), ',', m_strArrTitle);
	CPnxUtil::GetMgr()->SplitString(_T(INIReadStr(strAppName, _T("GridTitleSize"), strFilePath)), ',', strAry);
	for (int i = 0; i < strAry.GetSize(); ++i)
		m_ArrTitleSize.Add(atoi(strAry.GetAt(i)));

}

void	CDlgViewChannelPack::InitCtrl()
{
	// BUTTON
	SKINBTN(m_btnChannel, IDC_BTN_VIEW_CHANNEL_SELECT_CHANNEL, this, GetExePath() + _T("\\res\\Channel.png"), GetExePath() + _T("\\res\\Channel.png"));

	CString strName;

#ifdef _LANG_KOR
	strName = _T("선택 채널");
#endif
#ifdef _LANG_ENG
	strName = _T("Selected Channel");
#endif

	//TEXT
	CST_DEFAULT(m_ctrlChannelDesc, IDC_STATIC_VIEW_CHANNEL_CHANNEL_DESC, this, 12, 0, TRUE, ColorWhite, ColorLightBlack, strName);
	CST_DEFAULT(m_ctrlChannelName, IDC_STATIC_VIEW_CHANNEL_CHANNEL_NAME, this, 12, 0, TRUE, ColorWhite, ColorLightBlack, _T(""));
}

void	CDlgViewChannelPack::InitListCtrlChannel()
{
	m_Grid.SetEditable(FALSE);
	m_Grid.SetVirtualMode(FALSE);
	m_Grid.SetAutoSizeStyle();
	m_Grid.SetBkColor(ColorWhite);
	CString rString;

	int		nSubString = 0;

	int nColCount = m_strArrTitle.GetCount();

	TRY
	{
		m_Grid.SetColumnCount(nColCount);
		m_Grid.SetRowCount(1);
		m_Grid.SetFixedRowCount(1);
		m_Grid.SetFixedColumnCount(0);
	}
	CATCH(CMemoryException, e)
	{
		e->ReportError();
		return;
	}
	END_CATCH

	for (int col = 0; col < m_Grid.GetColumnCount(); col++)
	{
		CString str;

		GV_ITEM Item;

		Item.mask = GVIF_TEXT;
		Item.row = 0;
		Item.col = col;
		str.Format(_T("%s"), m_strArrTitle[col]);
			
		if (str.CompareNoCase(_T("Voltage")) == 0)
		{
			if (VoltageExpressionUnitV == CMgrOption::GetMgr()->GetVoltageExpUnit())
				str.Format(_T("Voltage(V)"));
			else
				str.Format(_T("Voltage(mV)"));
		}
		else if (str.CompareNoCase(_T("Current")) == 0)
		{
			if (AmpereExpressionUnitA == CMgrOption::GetMgr()->GetAmpereExpUnit())
				str.Format(_T("Current(A)"));
			else
				str.Format(_T("Current(mA)"));
		}
		else if (str.CompareNoCase(_T("Watt")) == 0)
		{
			if (VoltageExpressionUnitV == CMgrOption::GetMgr()->GetWattExpUnit())
				str.Format(_T("Watt(W)"));
			else
				str.Format(_T("Watt(Kw)"));
		}			
		Item.strText = str;

		m_Grid.SetItem(&Item);
		m_Grid.SetItemFormat(0, col, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	for (int col = 0; col < m_Grid.GetColumnCount(); col++)
	{
		m_Grid.SetColumnWidth(col, m_ArrTitleSize[col]);
	}
	
	m_Grid.SetListMode(TRUE);
}

void	CDlgViewChannelPack::LoadChannel()
{
	m_Grid.DeleteNonFixedRows(); //Fix된 Row는 빼고 Delete

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	int nChannelCnt = listChannel.GetCount();
	POSITION Pos = listChannel.GetHeadPosition();
	m_Grid.SetRowCount(nChannelCnt + 1);

	int nRow = 1;
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		CStringArray arr;
		pChannel->ChannelPack2Array(arr, this->m_strArrTitle);

		for (int i = 0; i < arr.GetSize(); ++i)
		{
			m_Grid.SetItemText(nRow, i, arr.GetAt(i));
			m_Grid.SetItemFormat(nRow, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		m_Grid.SetItemData(nRow, 0, (DWORD)pChannel);

		nRow++;

	}

	m_Grid.Invalidate();	
}

void	CDlgViewChannelPack::DrawChannel()
{
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	auto Pos = listChannel.GetHeadPosition();
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	int nChannelNo = 1;
	int nState = 0;
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (pChannel->GetEqMode() == EqModeSerial)
		{
			CStringArray arr;
			pChannel->ChannelPack2Array(arr, this->m_strArrTitle);
			nChannelNo = pChannel->GetChannelNumber() + 1;
			nState = pChannel->GetChannelStatus();

			for (int i = 0; i < arr.GetSize(); ++i)
			{
				if (nState == eCyclerChannelStateError)
				{
					if (m_Grid.GetItemText(0, i) != "StepTime" && m_Grid.GetItemText(0, i) != "TitalTime")  // step time과  total time
						m_Grid.SetItemText(nChannelNo, i, arr.GetAt(i));

					if (m_Grid.GetItemText(0, i) == "Status")
						m_Grid.SetItemBkColour(nChannelNo, i, Status2Color(_T("ERROR")));
				}
				else {
					m_Grid.SetItemText(nChannelNo, i, arr.GetAt(i));
					if (m_strArrTitle[i] == "Ch") {}
					else if (m_strArrTitle[i] == "Status")
						m_Grid.SetItemBkColour(nChannelNo, i, Status2Color(arr.GetAt(i)));
					else if (m_strArrTitle[i] == "Mode")
						m_Grid.SetItemBkColour(nChannelNo, i, RunMode2Color(arr.GetAt(i)));
					else if (m_strArrTitle[i] == "OpCode")
						m_Grid.SetItemBkColour(nChannelNo, i, OpCode2Color(arr.GetAt(i)));
					else
						m_Grid.SetItemBkColour(nChannelNo, i, ColorWhite);
				}
			}
		}
		else if (pChannel->GetEqMode() == EQModeParallel)
		{
			int nCurChannelNo = pChannel->GetChannelNumber() + 1;
			CString strChannelNo;

			int nChannelCountInParallel = pMgr->GetParallelChannelCount(); // 병렬에 필요한 Channel 갯수 마스터 + 슬레이브

			int nChannelStartNumber = nChannelCountInParallel * (pChannel->GetChannelNumber() / nChannelCountInParallel);

			strChannelNo.Format(_T("CH %d"), nChannelStartNumber + 1);

			if (pChannel->IsMasterChannel() == ParallelMaster)
			{
				m_Grid.SetItemText(nCurChannelNo, 0, _T(strChannelNo));

				CStringArray arr;
				pChannel->ChannelPack2Array(arr, this->m_strArrTitle);
				nChannelNo = pChannel->GetChannelNumber() + 1;
				nState = pChannel->GetChannelStatus();

				for (int i = 0; i < arr.GetSize(); ++i)
				{
					if (nState == eCyclerChannelStateError)
					{
						if (m_Grid.GetItemText(0, i) != "StepTime" && m_Grid.GetItemText(0, i) != "TitalTime")  // step time과  total time
							m_Grid.SetItemText(nChannelNo, i, arr.GetAt(i));

						if (m_Grid.GetItemText(0, i) == "Status")
							m_Grid.SetItemBkColour(nChannelNo, i, Status2Color(_T("ERROR")));
					}
					else {
						m_Grid.SetItemText(nChannelNo, i, arr.GetAt(i));
						if (m_strArrTitle[i] == "Ch") {}
						else if (m_strArrTitle[i] == "Status")
							m_Grid.SetItemBkColour(nChannelNo, i, Status2Color(arr.GetAt(i)));
						else if (m_strArrTitle[i] == "Mode")
							m_Grid.SetItemBkColour(nChannelNo, i, RunMode2Color(arr.GetAt(i)));
						else if (m_strArrTitle[i] == "OpCode")
							m_Grid.SetItemBkColour(nChannelNo, i, OpCode2Color(arr.GetAt(i)));
						else
							m_Grid.SetItemBkColour(nChannelNo, i, ColorWhite);
					}
				}
			}
			else
			{
				for (int i = 0; i < m_Grid.GetColumnCount(); i++)
				{
					//m_Grid.SetItemState(nCurChannelNo, i, m_Grid.GetItemState(i, i) | GVIS_FIXED | GVIS_FIXEDROW);
					//m_Grid.SetItemBkColour(nCurChannelNo, i, CLR_DEFAULT);
					//m_Grid.SetItemFgColour(nCurChannelNo, i, CLR_DEFAULT);

					if (i == 0)
					{
						m_Grid.SetItemText(nCurChannelNo, i, _T(strChannelNo));
					}
					else
					{
						m_Grid.SetItemText(nCurChannelNo, i, "");
					}
				}
			}		
		}
		
	}

	m_Grid.Invalidate();
}

COLORREF	CDlgViewChannelPack::Status2Color(CString strStatus)
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


COLORREF	CDlgViewChannelPack::RunMode2Color(CString strRunMode)
{
	if (strRunMode.CompareNoCase(_T("REST")) == 0)			return ColorLightBlack;
	else if (strRunMode.CompareNoCase(_T("CHARGE")) == 0)		return ColorSlateblue;
	else if (strRunMode.CompareNoCase(_T("DISCHARGE")) == 0)	return ColorWhiteYellow;
	else if (strRunMode.CompareNoCase(_T("PATTERN")) == 0)		return ColorYellow;
	else if (strRunMode.CompareNoCase(_T("IMPADANCE")) == 0)	return ColorYellow;

	return ColorWhite;
}

COLORREF	CDlgViewChannelPack::OpCode2Color(CString strOpCode)
{
	if (strOpCode.CompareNoCase(_T("None")) == 0)				return ColorGray;
	else if (strOpCode.CompareNoCase(_T("Input Relay On")) == 0)	return ColorBlue;
	else if (strOpCode.CompareNoCase(_T("Output Relay On")) == 0)	return ColorYellow;
	else if (strOpCode.CompareNoCase(_T("Charge CC")) == 0)			return ColorLightGreen;
	else if (strOpCode.CompareNoCase(_T("CHARGE CC/CV")) == 0)			return ColorLightGreen;
	else if (strOpCode.CompareNoCase(_T("Charge CP")) == 0)			return ColorLightGreen;
	else if (strOpCode.CompareNoCase(_T("Discharge CC")) == 0)		return ColorLightRed;
	else if (strOpCode.CompareNoCase(_T("DISCHARGE CC/CV")) == 0)		return ColorLightRed;
	else if (strOpCode.CompareNoCase(_T("Discharge CP")) == 0)		return ColorLightRed;
	else if (strOpCode.CompareNoCase(_T("Rest")) == 0)				return ColorBlue;

	return ColorWhite;
}

void CDlgViewChannelPack::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if (m_Grid.GetSafeHwnd() == NULL)
		return;

	CRect rtCtrl;
	CRect rtParent;
	CRect rtIcon;

	this->GetClientRect(&rtCtrl);
	m_btnChannel.GetWindowRect(&rtIcon);
	ScreenToClient(&rtIcon);

	m_Grid.SetWindowPos(NULL, rtIcon.left, rtIcon.bottom, rtCtrl.Width() - rtIcon.left - 8, (rtCtrl.Height() - rtIcon.bottom) - 5, SWP_NOZORDER);
}

// NM_RCLICK
void CDlgViewChannelPack::OnGridRClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	//Trace(_T("Right button click on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	if (FALSE == CMgrChannel::GetMgr()->GetChannelSelected())
		return;

	// 현재 마우스 커서 위치 받아옴
	CPoint point;
	GetCursorPos(&point);

	// 메뉴 중 2번째 Column 즉, Command 부분만 추출
	CMenu* pMenu = AfxGetApp()->GetMainWnd()->GetMenu();

	if (pMenu)
	{
		CMenu* pSubMenu = pMenu->GetSubMenu(0);

		if (pSubMenu)
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

// GVN_BEGINLABELEDIT
void CDlgViewChannelPack::OnGridStartEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	//Trace(_T("Start Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	//*pResult = (m_bRejectEditAttempts) ? -1 : 0;
}

// GVN_ENDLABELEDIT
void CDlgViewChannelPack::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	//Trace(_T("End Edit on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	//*pResult = (m_bRejectEditChanges) ? -1 : 0;
}

// GVN_SELCHANGING
void CDlgViewChannelPack::OnGridStartSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	//Trace(_T("Start Selection Change on row %d, col %d (%d Selected)\n"),
	//	pItem->iRow, pItem->iColumn, m_Grid.GetSelectedCount());
}

// GVN_SELCHANGED
void CDlgViewChannelPack::OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	CList<int>& lstSelected = m_Grid.GetSelectedRows();

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	auto Pos = listChannel.GetHeadPosition();
	CChannel* pChannel = NULL;
	int nChannel = -1;
	POSITION posFind = (POSITION)0;
	CString strTemp = "";
	CString strCh = "";
	int nLastSelectCh = 0;
	while (Pos)
	{
		pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		nChannel = pChannel->GetChannelNumber()+1;
		posFind = lstSelected.Find(nChannel);
		if (posFind != (POSITION)0)
		{
			pChannel->SetSelected(TRUE);
			strTemp.Format(_T("Ch %d,"), nChannel);
			strCh += strTemp;
			CDlgViewDAQ::GetMgr()->SetChannelNumber(pChannel->GetChannelNumber());
			nLastSelectCh = pChannel->GetChannelNumber();
		}
		else
			pChannel->SetSelected(FALSE);
	}

	strCh = strCh.TrimRight(',');

	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
		::PostMessage(pWnd->GetSafeHwnd(), WM_VIEW_PATTERN, 0, nLastSelectCh);

	m_ctrlChannelName.SetWindowText(strCh);
}

// GVN_BEGINDRAG
void CDlgViewChannelPack::OnGridBeginDrag(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	//Trace(_T("Begin Drag row %d, col %d\n"), pItem->iRow, pItem->iColumn);
}

void CDlgViewChannelPack::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(pItem->iRow - 1);

	/*if (pChannel != NULL)
		CDlgViewPattern::GetMgr()->Refresh(pChannel->GetChannelNumber());*/
		//Trace(_T("Double Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
}

void CDlgViewChannelPack::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	//Trace(_T("Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
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

LRESULT CDlgViewChannelPack::CompleteInitDialog(WPARAM w, LPARAM l)
{
	CString specificationPath = _T("");
	CString findChannel = "";
	BOOL bFind = FALSE;

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	auto Pos = listChannel.GetHeadPosition();

	while (Pos) {
		CChannel* pChannel = listChannel.GetNext(Pos);
		if (NULL == pChannel) {
			continue;
		}

		int dChannelNum = pChannel->GetChannelNumber();
		specificationPath.Format(_T("%s\\Config\\Specification\\CH%02d.ini"), GetExePath(), dChannelNum + 1);

		if (!FileExist(specificationPath)) {
			continue;
		}

		/*CString endCondition = INIReadStr("Specification", "EndCondition", specificationPath);
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
			if (!pMgrComm)
			{
				CCommData* pComm = pMgrComm->GetAt(dChannelNum);
				pComm->SetStepTime(stepTime);
				pComm->SetTotalTime(totalTime);
			}
			auto cycle = INIReadInt("Specification", "CycleNumber", specificationPath);
			auto cycleIndex = INIReadInt("Specification", "CycleIndex", specificationPath);
			auto step = INIReadInt("Specification", "StepNumber", specificationPath);
			auto stepIndex = INIReadInt("Specification", "StepIndex", specificationPath);
			g_nCycleLoopStartSet[cycle - 1] = cycleIndex;
		}*/
	}

	//if (bFind) {
	//	CString message;
	//	message.Format("%s채널에서 비정상 종료 발견", findChannel);
	//	AfxMessageBox(message);
	//}


	return 0;
}


