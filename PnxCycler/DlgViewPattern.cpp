// DlgViewPattern.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "DlgViewPattern.h"
#include "afxdialogex.h"
#include "MgrCommData.h"
#include "PnxUtil.h"
// CDlgViewPattern 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgViewPattern, CDialogEx)

CDlgViewPattern::CDlgViewPattern(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgViewPattern::IDD, pParent)
{
	m_pCurChannel = NULL;
	m_pCurStep = NULL;
	m_nPrevRow = -1;
}

CDlgViewPattern::~CDlgViewPattern()
{
}

void CDlgViewPattern::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID_PATTERN, m_GridPattern);
	DDX_Control(pDX, IDC_GRID_CONDITION, m_GridCondition);
}


BEGIN_MESSAGE_MAP(CDlgViewPattern, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_PATTERN, OnGridEndSelChange)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgViewPattern 메시지 처리기입니다.


BOOL CDlgViewPattern::Create(CWnd* pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}

BOOL CDlgViewPattern::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

BOOL CDlgViewPattern::OnInitDialog()
{
	__super::OnInitDialog();

	CenterWindow();

	LoadDrawINI();

	CRect rt;
	
	m_GridPattern.GetWindowRect(&rt);

	ScreenToClient(&rt);

	CST_DEFAULT(m_ctrlSchText, IDC_STATIC_SCHEDULE_TEXT, this, 12, 0, TRUE, ColorWhite, ColorLightBlack, _T("Schedule Progress Status"));
	SKINBTN(m_imgSchedule, IDC_BTN_SCHEDULE, this, GetExePath() + _T("\\res\\Schedule.png"), GetExePath() + _T("\\res\\Schedule.png"));

	m_GridPattern.SetBkColor(ColorWhite);
	m_GridCondition.SetBkColor(ColorWhite);
	SetFont(&m_GridPattern, "굴림", 9, FALSE);
	SetFont(&m_GridCondition, "굴림", 9, FALSE);

	CRect rect;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height() - rc.top - 3, SWP_NOZORDER);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgViewPattern::LoadDrawINI()
{
	CStringArray strAry;
	CString strFilePath; strFilePath.Empty();
	CString strAppName; strAppName.Empty();

	strFilePath.Format(_T("%s\\Config\\%s"), GetExePath(), _T("Draw.ini"));
	strAppName.Format(_T("ViewPattern"));

	m_nGridRowSize = INIReadInt(strAppName, _T("ListCtrlRowSize"), strFilePath);

	m_strArrPattern = INIReadStr(strAppName, _T("GridTitle"), strFilePath);
	CPnxUtil::GetMgr()->SplitString(m_strArrPattern, ',', m_strArrTitle);

	CPnxUtil::GetMgr()->SplitString(_T(INIReadStr(strAppName, _T("GridTitleSize"), strFilePath)), ',', strAry);
	for (int i = 0; i < strAry.GetSize(); ++i)
		m_ArrTitleSize.Add(atoi(strAry.GetAt(i)));
	InitListCtrl(&m_GridPattern, m_strArrPattern);

	strAppName.Format(_T("ViewCondition"));
	m_InitWidth = INIReadInt(strAppName, _T("GridTitleSizeFix"), strFilePath);

	m_ArrTitleSize.RemoveAll();
	m_strArrCondition = INIReadStr(strAppName, _T("GridTitle"), strFilePath);
	CPnxUtil::GetMgr()->SplitString(_T(INIReadStr(strAppName, _T("GridTitleSize"), strFilePath)), ',', strAry);
	m_StrArrCondSize = INIReadStr(strAppName, _T("GridTitleSize"), strFilePath);
	for (int i = 0; i < strAry.GetSize(); ++i)
		m_ArrTitleSize.Add(atoi(strAry.GetAt(i)));

	InitListCtrl(&m_GridCondition, m_strArrCondition);
}

void CDlgViewPattern::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if (TRUE == bShow)
	{
		SetTimer(PTNViewTimerID, PTNViewRefreshTime, NULL);		//추가
	}
}

BOOL CDlgViewPattern::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;

	pDC->GetClipBox(&rt);
	pDC->FillSolidRect(&rt, ColorWhite);

	return TRUE;
}

void	CDlgViewPattern::ShowViewPattern(int nChannelNumber)
{
	m_iCurChannelStepInfo = nChannelNumber;
	LoadPattern(nChannelNumber);
	this->ShowWindow(SW_SHOW);
}

void CDlgViewPattern::SetFont(CGridCtrl* pGrid, CString strFontName, int nSize, bool bBold)
{
	LOGFONT lf;
	pGrid->GetFont()->GetLogFont(&lf);

	HWND hWnd = ::GetActiveWindow();
	HDC hdc = ::GetDC(hWnd);

	// nFontSize            : 일반 크기(pt)
	// LOGFONT::lfHeight    : 논리 크기

	// 일반 크기(pt) 를 논리 크기로 변환	
	lf.lfHeight = -MulDiv(nSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	// 논리크기 = -((일반크기 * GetDeviceCaps(pDC, LOGPIXELSY)) / 72)

	// 논리 크기를 일반 크기로 변환
	// nFontSize = -MulDiv(LOGFONT::lfHeight, 72, GetDeviceCaps(pDC, LOGPIXELSY));
	// 일반크기 = -((논리크기 * 72) / GetDeviceCaps(pDC, LOGPIXELSY))
	if (bBold)
		lf.lfWeight = 1000;
	else
		lf.lfWeight = 100;

	CFont Font;
	memcpy(lf.lfFaceName, (LPSTR)(LPCTSTR)strFontName, sizeof(lf.lfFaceName));
	Font.CreateFontIndirect(&lf);
	pGrid->SetFont(&Font);
	//pGrid->AutoSize();
	Font.DeleteObject();
}

void	CDlgViewPattern::InitListCtrl(CGridCtrl* pGrid, CString columnNames)
{
	pGrid->SetEditable(FALSE);
	pGrid->SetVirtualMode(FALSE);
	pGrid->SetAutoSizeStyle();

	CString rString;

	int		nSubString = 0;
	int		nSubString2 = 0;
	CStringArray strArray;
	CStringArray strSize;

	int nRowIndex = 0;
	int nRowIndex2 = 0;
	while (AfxExtractSubString(rString, columnNames, nSubString++, _T(',')))
	{
		strArray.SetAtGrow(nRowIndex, rString.Trim());
		nRowIndex++;
	}

	int nColCount = strArray.GetCount();

	TRY{
		pGrid->SetColumnCount(nColCount);
		pGrid->SetRowCount(1);
		pGrid->SetFixedRowCount(1);
		pGrid->SetFixedColumnCount(0);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return;
	}
	END_CATCH

		for (int col = 0; col < pGrid->GetColumnCount(); col++)
		{
			CString str;

			GV_ITEM Item;

			Item.mask = GVIF_TEXT;
			Item.row = 0;
			Item.col = col;

			str.Format(_T("%s"), strArray[col]);

			Item.strText = str;

			pGrid->SetItem(&Item);
			pGrid->SetItemFormat(0, col, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

	for (int col = 0; col < pGrid->GetColumnCount(); col++)
	{
		pGrid->SetColumnWidth(col, m_ArrTitleSize[col]);
	}

	pGrid->SetListMode(TRUE);
}

void	CDlgViewPattern::LoadPattern(int nChannelNumber)
{
	m_GridPattern.DeleteNonFixedRows();
	m_GridPattern.Invalidate();
	m_GridCondition.DeleteNonFixedRows();
	m_GridCondition.Invalidate();

	CMgrChannel* pMgrChannel = CMgrChannel::GetMgr();
	if (!pMgrChannel)
		return;
	CChannel* pChannel = pMgrChannel->GetAt(nChannelNumber);
	if (pChannel == NULL)
		return;

	CList< CCycle*, CCycle* >& listCycle = pChannel->GetMgr()->GetCycleList();
	if (listCycle.GetCount() == 0)
		return;

	m_pCurChannel = pChannel;
	CMgrSchedule* pMgr = m_pCurChannel->GetMgr();
	if (pMgr == NULL)
		return;

	//m_nCurrentStep = -1;
	CList< CStep*, CStep* >& listStep = pMgr->GetStepList();

	int nRowCount = m_GridPattern.GetRowCount();
	bool bChangedRowCount = false;
	if (listStep.GetCount() + 1 != nRowCount)
		bChangedRowCount = true;

	if (bChangedRowCount)
		m_GridPattern.SetRowCount(listStep.GetCount() + 1);

	auto Pos = listStep.GetHeadPosition();
	int nRow = 1;
	CStep* pStep = NULL;
	while (Pos)
	{
		 pStep = listStep.GetNext(Pos);

		if (NULL == pStep)
			continue;

		if (bChangedRowCount)
			m_GridPattern.SetRowHeight(nRow, 25);

		CStringArray arr;
		m_GridPattern.SetItemData(nRow, StepListCtrlTypeCycleNo, (DWORD)pStep);

		pStep->Step2Array(arr, m_pCurChannel->GetChannelNumber(), m_strArrTitle);
		for (int i = 0; i < m_GridPattern.GetColumnCount(); ++i)
		{
			m_GridPattern.SetItemFormat(nRow, i, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			DrawPattern(nRow, arr);
		}
		nRow++;
	}
	m_GridPattern.Invalidate();

	KillTimer(PTNViewTimerID);
	SetTimer(PTNViewTimerID, PTNViewRefreshTime, NULL);		//추가
}

void	CDlgViewPattern::ShowDrawPattern()
{
	CStep* pStep = NULL;

	CString strStatus;
	CList< CCycle*, CCycle* >& listCycle = m_pCurChannel->GetMgr()->GetCycleList();

	CString strData;
	for (int nRow = 1; nRow < m_GridPattern.GetRowCount(); nRow++)
	{
		pStep = (CStep*)m_GridPattern.GetItemData(nRow, StepListCtrlTypeCycleNo);//StepListCtrlTypeExecute
		if (pStep == NULL)
			continue;

		if (m_pCurChannel == NULL)
			continue;

		if (m_pCurChannel->GetStepNumber() == pStep->GetStepNumber() &&
			m_pCurChannel->GetCycleNumber() == pStep->GetCycleNumber())
		{
			if (m_pCurStep == NULL || pStep->GetStepNumber() != m_pCurStep->GetStepNumber() || pStep->GetCycleNumber() != m_pCurStep->GetCycleNumber())
			{
				m_pCurStep = pStep;
				DrawConditionData(nRow);
			}
			auto pos = listCycle.FindIndex(pStep->GetCycleNumber() - 1/*가져오는 사이클 넘버는 1base 이다..*/);
			if (pos == NULL)
				continue;

			CCycle* pCycle = listCycle.GetAt(pos);
			if (pCycle == NULL)
				continue;

			CString rString;
			for (int nCol = 0; nCol < m_strArrTitle.GetCount(); nCol++)
			{
				if (m_strArrTitle[nCol].CompareNoCase(_T("Schedule State")) == 0 ||
					(m_strArrTitle[nCol].CompareNoCase(_T("Curr Volt(V)")) == 0) ||
					m_strArrTitle[nCol].CompareNoCase(_T("Curr Ampare(A)")) == 0)
				{
					rString = pStep->GetTitleToValue(m_strArrTitle[nCol], m_pCurChannel->GetChannelNumber());
					m_GridPattern.SetItemText(nRow, nCol, rString);
					
					if (m_strArrTitle[nCol].CompareNoCase(_T("Schedule State")) == 0)
						m_GridPattern.SetItemBkColour(nRow, nCol, ColorLightBlue);
				}				
			}


			m_GridPattern.EnsureVisible(nRow, 1);

			if (m_nPrevRow != nRow)
			{
				m_nPrevRow = nRow;
				DrawConditionData(nRow);
			}
		}
		else
		{
			for (int nCol = 0; nCol < m_strArrTitle.GetCount(); nCol++)
			{
				if (m_strArrTitle[nCol].CompareNoCase(_T("Schedule State")) == 0 ||
					(m_strArrTitle[nCol].CompareNoCase(_T("Curr Volt(V)")) == 0) ||
					m_strArrTitle[nCol].CompareNoCase(_T("Curr Ampare(A)")) == 0)
				{
					m_GridPattern.SetItemText(nRow, nCol, "");
					m_GridPattern.SetItemBkColour(nRow, nCol, ColorWhite);
				}
			}
		}
	}

	for (int nCol = 0; nCol < m_strArrTitle.GetCount(); nCol++)
	{
		if (m_strArrTitle[nCol].CompareNoCase(_T("Schedule State")) == 0 ||
			(m_strArrTitle[nCol].CompareNoCase(_T("Curr Volt(V)")) == 0) ||
			m_strArrTitle[nCol].CompareNoCase(_T("Curr Ampare(A)")) == 0)
		{
			m_GridPattern.RedrawColumn(nCol);			
		}
	}
}

void	CDlgViewPattern::DrawPattern(int nRow, CStringArray& arr)
{

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		m_GridPattern.SetItemText(nRow, i, arr.GetAt(i));
		CString str = arr.GetAt(i);
		
		//if (StepListCtrlTypeStepType == i)//검증필요 0430
		if (m_strArrTitle[i].CompareNoCase("Type"))
		{
			if (str.Find(_T("Rest")) >= 0 ||
				str.Find(_T("OCV")) >= 0)
				m_GridPattern.SetItemBkColour(nRow, i, ColorLightGray);
			else if (str.Find(_T("Charge")) >= 0)
				m_GridPattern.SetItemBkColour(nRow, i, ColorBlue);
			else if (str.Find(_T("Discharge")) >= 0)
				m_GridPattern.SetItemBkColour(nRow, i, ColorLightRed);
			else if (str.Find(_T("Pattern")) >= 0)
				m_GridPattern.SetItemBkColour(nRow, i, ColorLightBlack);
			else
				m_GridPattern.SetItemBkColour(nRow, i, ColorWhite);
		}
	}
	m_GridPattern.RedrawRow(nRow);
}

void	CDlgViewPattern::DrawCondition(int nIndex, CStringArray& arr)
{
	m_GridCondition.DeleteNonFixedRows();

	int nRow = 1;
	m_GridCondition.SetRowCount(arr.GetSize() / m_GridCondition.GetColumnCount() + 1);
	for (int i = 0; i < arr.GetSize();)
	{
		for (int nCol = 0; nCol < m_GridCondition.GetColumnCount(); nCol++)
		{
			m_GridCondition.SetItemText(nRow, nCol, arr.GetAt(i++));
			m_GridCondition.SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		m_GridCondition.SetRowHeight(nRow, 25);

		nRow++;
	}
}
void	CDlgViewPattern::DrawCycleCondition(CStep* pStep)
{
	if (nullptr == m_pCurChannel)
		return;

	CMgrSchedule* pMgr = m_pCurChannel->GetMgr();

	if (!pMgr)
		return;

	int nGroup = 0;
	CCycle* pCycle = pMgr->GetAtCycle(pStep->GetCycleNumber());
	if (pCycle)
	{
		CStringArray arr;
		pCycle->Condition2Array(arr);

		int nRow = m_GridCondition.GetRowCount();
		int nAddRow = arr.GetSize() / m_GridCondition.GetColumnCount();
		m_GridCondition.SetRowCount(nRow + nAddRow);

		for (int i = 0; i < arr.GetSize();)
		{
			for (int nCol = 0; nCol < m_GridCondition.GetColumnCount(); nCol++)
			{
				m_GridCondition.SetItemText(nRow, nCol, arr.GetAt(i++));
				m_GridCondition.SetItemFormat(nRow, nCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			m_GridCondition.SetRowHeight(nRow, 25);
			nRow++;
		}
	}
}

//kw----------------- 추가 -------------------------//
void CDlgViewPattern::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == PTNViewTimerID)
	{
		if (m_pCurChannel != NULL)
		{
			if (m_pCurChannel->GetScheduleStart())
				ShowDrawPattern();
			else
			{
				switch (m_pCurChannel->GetChannelStatus())
				{
				case	eCyclerChannelStateComplete:
				case 	eCyclerChannelStateError:
				{
					CStep* pStep = NULL;
					for (int nRow = 1; nRow < m_GridPattern.GetRowCount(); nRow++)
					{
						pStep = (CStep*)m_GridPattern.GetItemData(nRow, StepListCtrlTypeCycleNo);
						if (pStep == NULL)
							continue;

						for (int nCol = 0; nCol < m_strArrTitle.GetCount(); nCol++)
						{
							if (m_strArrTitle[nCol].CompareNoCase(_T("Schedule State")) == 0)
							{
								if (m_pCurChannel->GetStepNumber() == pStep->GetStepNumber() &&
									m_pCurChannel->GetCycleNumber() == pStep->GetCycleNumber())
								{
									if (m_pCurChannel->GetChannelStatus() == eCyclerChannelStateComplete)
									{
										m_GridPattern.SetItemText(nRow, nCol, "Complete");
										m_GridPattern.SetItemBkColour(nRow, nCol, ColorBlue);
									}
									else
									{
										m_GridPattern.SetItemText(nRow, nCol, "Error");
										m_GridPattern.SetItemBkColour(nRow, nCol, ColorRed);
									}
								}
								else
								{
									m_GridPattern.SetItemText(nRow, nCol, "");
									m_GridPattern.SetItemBkColour(nRow, nCol, ColorWhite);
								}
							}
						}

					}

					for (int nCol = 0; nCol < m_strArrTitle.GetCount(); nCol++)
					{
						if (m_strArrTitle[nCol].CompareNoCase(_T("Schedule State")) == 0)
						{
							m_GridPattern.RedrawColumn(nCol);
							break;
						}
					}
				}
				break;
				}
			}
		}
		__super::OnTimer(nIDEvent);
	}
}
//kw-------------------------------------------//

void CDlgViewPattern::OnPaint()
{
	CPaintDC dc(this);
	// 
	// 	m_imgSchedule.SetBkDC(&dc);
}

void CDlgViewPattern::OnGridEndSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	//Trace(_T("Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	DrawConditionData(pItem->iRow);
}

void CDlgViewPattern::DrawConditionData(int nRow)
{
	CStep* pStep = (CStep*)m_GridPattern.GetItemData(nRow, StepListCtrlTypeCycleNo);
	if (pStep == NULL || m_pCurChannel == NULL)
		return;

	CStringArray strArray;
	pStep->Condition2Array(strArray);
	DrawCondition(nRow, strArray);
	DrawCycleCondition(pStep);
}

void CDlgViewPattern::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if (m_GridCondition.GetSafeHwnd() == NULL)
		return;

	CRect rtIcon;
	CRect rtCtrl;
	CRect rtCtrl2;
	CRect rtCtrl3;

	GetClientRect(&rtCtrl);

	m_imgSchedule.GetWindowRect(&rtIcon);
	ScreenToClient(&rtIcon);

	m_GridPattern.SetWindowPos(NULL, rtIcon.left, rtIcon.bottom, rtCtrl.Width()*0.75, rtCtrl.Height() - rtIcon.bottom - 5, SWP_NOZORDER);
	m_GridPattern.GetWindowRect(&rtCtrl2);
	ScreenToClient(&rtCtrl2);

	m_GridCondition.SetWindowPos(NULL, rtCtrl2.right + 10, rtIcon.bottom + 0.1, (rtCtrl.Width()*0.25) - 28, rtCtrl.Height() - rtIcon.bottom - 5, SWP_NOZORDER);

	m_GridCondition.GetWindowRect(&rtCtrl3);
	ScreenToClient(&rtCtrl3);

	if (m_InitWidth)
	{
		for (int col = 0; col < m_GridPattern.GetColumnCount(); col++)
		{
			m_GridPattern.SetColumnWidth(col, (rtCtrl2.Width()) / m_GridPattern.GetColumnCount());
		}
		for (int col = 0; col < m_GridCondition.GetColumnCount(); col++)
		{
			m_GridCondition.SetColumnWidth(col, (rtCtrl3.Width()) / m_GridCondition.GetColumnCount());
		}
		m_InitWidth = false;
	}
}
