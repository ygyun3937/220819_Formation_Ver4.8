// DlgViewDAQ.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "PnxCycler.h"
#include "DlgViewDAQ.h"
#include "afxdialogex.h"

#include "MgrDAQ.h"
#include "MgrCommData.h"
#include "MgrConfig.h"
#include "PnxUtil.h"

#include "DlgViewPattern.h"
#include "NewCellTypes/GridURLCell.h"
#include "NewCellTypes/GridCellCombo.h"
#include "NewCellTypes/GridCellCheck.h"
#include "NewCellTypes/GridCellNumeric.h"
#include "NewCellTypes/GridCellDateTime.h"

#define		DAQUpdateTimerId	( 1028 )
#define		TIMER_SET_1S			( 1000 )

// CDlgViewDAQ 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgViewDAQ, CDialogEx)

CDlgViewDAQ::CDlgViewDAQ(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgViewDAQ::IDD, pParent)
{
	m_nChannelNumer = -1;
}

CDlgViewDAQ::~CDlgViewDAQ()
{
}

void CDlgViewDAQ::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRIDDAQ, m_DaqGrid);
}


BEGIN_MESSAGE_MAP(CDlgViewDAQ, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgViewDAQ 메시지 처리기입니다.


BOOL CDlgViewDAQ::Create(CWnd* pParentWnd)
{
	return __super::Create(IDD, pParentWnd);
}


BOOL CDlgViewDAQ::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
			return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


BOOL CDlgViewDAQ::OnInitDialog()
{
	__super::OnInitDialog();
	LoadDrawINI();

	InitCtrl();

	InitListCtrl();	

	CRect rect;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	CRect rc;
	m_btnDAQ.GetWindowRect(&rc);
	int nHeight = m_DaqGrid.GetFixedRowHeight()*m_nGridRowSize + rc.Height();
	SetWindowPos(NULL, 300,300, rect.Width(), nHeight, SWP_NOZORDER);
	//SetTimer(DAQUpdateTimerId, OneSecond / 10, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgViewDAQ::OnEraseBkgnd(CDC* pDC)
{
	CRect rt;

	pDC->GetClipBox(&rt);
	pDC->FillSolidRect(&rt, ColorWhite);

	return TRUE;
}

void CDlgViewDAQ::OnTimer(UINT_PTR nIDEvent)
{
	if (DAQUpdateTimerId == nIDEvent)
	{
		LoadDAQ();
	}

	__super::OnTimer(nIDEvent);
}

void CDlgViewDAQ::OnDestroy()
{
	__super::OnDestroy();

	KillTimer(DAQUpdateTimerId);
}


void CDlgViewDAQ::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	m_btnDAQ.SetBkDC(&dc);
}

void	CDlgViewDAQ::SetChannelNumber(int nChannelNumber)
{
	m_nChannelNumer = nChannelNumber;
}

void	CDlgViewDAQ::TimerStart()
{
	KillTimer(DAQUpdateTimerId);
	SetTimer(DAQUpdateTimerId, TIMER_SET_1S / 2, NULL);
}

void	CDlgViewDAQ::TimerEnd()
{
	m_DaqGrid.DeleteNonFixedRows();
	m_DaqGrid.Invalidate();

	KillTimer(DAQUpdateTimerId);
}

void	CDlgViewDAQ::InitCtrl()
{
	SKINBTN(m_btnDAQ, IDC_BTN_DAQ, this, GetExePath() + _T("\\res\\Daq.png"), GetExePath() + _T("\\res\\Daq.png"));

	CST_DEFAULT(m_ctrlDAQText, IDC_STATIC_DAQ_TEXT, this, 12, 0, TRUE, ColorWhite, ColorLightBlack, _T("DAQ Name"));
	CST_DEFAULT(m_ctrlDAQName, IDC_STATIC_DAQ_NAME, this, 12, 0, TRUE, ColorWhite, ColorLightBlack, _T("Not Load"));
}

void CDlgViewDAQ::LoadDrawINI()
{
	CStringArray strAry;
	CString strFilePath; strFilePath.Empty();
	CString strAppName; strAppName.Empty();

	strFilePath.Format(_T("%s\\Config\\%s"), GetExePath(), _T("Draw.ini"));
	strAppName.Format(_T("ViewDAQ"));

	m_nGridRowSize = INIReadInt(strAppName, _T("ListCtrlRowSize"), strFilePath);

	CPnxUtil::GetMgr()->SplitString(_T(INIReadStr(strAppName, _T("GridTitle"), strFilePath)), ',', m_strArrTitle);
	CPnxUtil::GetMgr()->SplitString(_T(INIReadStr(strAppName, _T("GridTitleSize"), strFilePath)), ',', strAry);
	for (int i = 0; i < strAry.GetSize(); ++i)
		m_ArrTitleSize.Add(atoi(strAry.GetAt(i)));
}

void	CDlgViewDAQ::InitListCtrl()
{
	m_DaqGrid.SetEditable(FALSE);
	m_DaqGrid.SetVirtualMode(FALSE);
	m_DaqGrid.SetAutoSizeStyle();
	m_DaqGrid.SetBkColor(ColorWhite);

	CString rString;

	int		nSubString = 0;

	int nColCount = m_strArrTitle.GetCount();

	TRY
	{
		m_DaqGrid.SetColumnCount(nColCount);
		m_DaqGrid.SetRowCount(1);
		m_DaqGrid.SetFixedRowCount(1);
		m_DaqGrid.SetFixedColumnCount(0);
	}
		CATCH(CMemoryException, e)
	{
		e->ReportError();
		return;
	}
	END_CATCH

		for (int col = 0; col < m_DaqGrid.GetColumnCount(); col++)
		{
			CString str;

			GV_ITEM Item;

			Item.mask = GVIF_TEXT;
			Item.row = 0;
			Item.col = col;

			str.Format(_T("%s"), m_strArrTitle[col]);

			Item.strText = str;

			m_DaqGrid.SetItem(&Item);
			m_DaqGrid.SetItemFormat(0, col, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

	for (int col = 0; col < m_DaqGrid.GetColumnCount(); col++)
	{
		m_DaqGrid.SetColumnWidth(col, m_ArrTitleSize[col]);
	}

	m_DaqGrid.SetListMode(TRUE);
}

void	CDlgViewDAQ::LoadDAQ()
{
	if (m_nChannelNumer < 0)
		return;

	CCommData* pComm = CMgrCommData::GetMgr()->GetAt(m_nChannelNumer);

	if (NULL == pComm)
		return;

	BOOL bUseBalance = FALSE;

	CList< CDAQ*, CDAQ* >& listDAQ = pComm->GetDAQList();

	m_DaqGrid.SetRowCount(listDAQ.GetCount() 
		+ m_DaqGrid.GetFixedRowCount());
	auto Pos = listDAQ.GetHeadPosition();

	int nRow = 1;
	while (Pos)
	{
		CDAQ* pDAQ = listDAQ.GetNext(Pos);

		if (NULL == pDAQ)
			continue;

		CStringArray arr;
		pDAQ->DAQ2Array(arr, pDAQ->GetUseBalance(), m_strArrTitle);

		for (int i = 0; i < arr.GetSize(); ++i)
		{
			m_DaqGrid.SetItemText(nRow, i, arr.GetAt(i));
		}
		nRow++;	
	}

	m_DaqGrid.Invalidate();
	
	CMgrDAQ* pMgr = CMgrDAQ::GetMgr();

	m_ctrlDAQName.SetWindowText(pMgr->GetFileName());
}

void CDlgViewDAQ::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (m_DaqGrid.GetSafeHwnd() == NULL)
		return;

	CRect rtDAQIcon;

	m_btnDAQ.GetWindowRect(&rtDAQIcon);
	ScreenToClient(&rtDAQIcon);

	m_DaqGrid.SetWindowPos(NULL, rtDAQIcon.left, rtDAQIcon.bottom, cx - 10, cy - rtDAQIcon.Height() - 5, SWP_NOZORDER);
}
