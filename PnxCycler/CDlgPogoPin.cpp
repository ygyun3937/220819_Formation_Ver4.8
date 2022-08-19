// CDlgPogoPin.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgPogoPin.h"
#include "afxdialogex.h"
#include "PnxUtil.h"
#include "MgrChannel.h"
#include "MgrCommData.h"
#include "MgrConfig.h"


// CDlgPogoPin 대화 상자
#define Formation_PogoPinUpdateTimerId (2000)
IMPLEMENT_DYNAMIC(CDlgPogoPin, CDialogEx)

CDlgPogoPin::CDlgPogoPin(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_POGOPIIN, pParent)
{

}

CDlgPogoPin::~CDlgPogoPin()
{
}

void CDlgPogoPin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_DAQ, m_Btn_Pogo);
	DDX_Control(pDX, IDC_GRID_POGOPIN1, m_PogoPinGrid1);
	DDX_Control(pDX, IDC_GRID_POGOPIN2, m_PogoPinGrid2);
	DDX_Control(pDX, IDC_TITLE_POGO_STAGE1, m_Title_Stage1);
	DDX_Control(pDX, IDC_TITLE_POGO_STAGE2, m_Title_Stage2);
	DDX_Control(pDX, IDC_BUTTON_STAGE1_APPLY, m_btn_Stage1_Apply);
	DDX_Control(pDX, IDC_BUTTON_STAGE2_APPLY, m_btn_Stage2_Apply);
}

BOOL CDlgPogoPin::Create(CWnd * pParentWnd)
{
	return CDialogEx::Create(IDD, pParentWnd);
}

BOOL CDlgPogoPin::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	LoadDrawINI();

	Init_Control();

	SetTimer(Formation_PogoPinUpdateTimerId, 500, NULL);

	return 0;
}

BOOL CDlgPogoPin::PreTranslateMessage(MSG * pMsg)
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


BEGIN_MESSAGE_MAP(CDlgPogoPin, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgPogoPin::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_STAGE1_APPLY, &CDlgPogoPin::OnBnClickedButtonStage1Apply)
	ON_BN_CLICKED(IDC_BUTTON_STAGE2_APPLY, &CDlgPogoPin::OnBnClickedButtonStage2Apply)
END_MESSAGE_MAP()


// CDlgPogoPin 메시지 처리기


void CDlgPogoPin::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CDlgPogoPin::Init_Control()
{

	CString rString;
	CString PogoPinChName;
	int		nSubString = 0;
	int nColCount = 0;
	bool bSubStage1Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage1Use;
	bool bSubStage2Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage2Use;
	int nWidth = 460;
	int nHeight = 740;
	int nWidthCount = 0;

	nColCount = m_strArrTitle.GetCount();

	if (bSubStage1Use == true)
	{
		nWidthCount++;

		m_PogoPinGrid1.SetEditable();
		m_PogoPinGrid1.SetVirtualMode(FALSE);
		m_PogoPinGrid1.SetAutoSizeStyle();
		m_PogoPinGrid1.SetBkColor(ColorWhite);


		TRY
		{
			m_PogoPinGrid1.SetColumnCount(nColCount);
			m_PogoPinGrid1.SetRowCount(25);
			m_PogoPinGrid1.SetFixedRowCount(1);
			m_PogoPinGrid1.SetFixedColumnCount(1);
		}
			CATCH(CMemoryException, e)
		{
			e->ReportError();
			return;
		}
		END_CATCH

			for (int col = 0; col < m_PogoPinGrid1.GetColumnCount(); col++)
			{
				CString str;

				GV_ITEM Item;

				Item.mask = GVIF_TEXT;
				Item.row = 0;
				Item.col = col;

				str.Format(_T("%s"), m_strArrTitle[col]);

				Item.strText = str;

				m_PogoPinGrid1.SetItem(&Item);
				m_PogoPinGrid1.SetItemFormat(0, col, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

		for (int col = 0; col < m_PogoPinGrid1.GetColumnCount(); col++)
		{
			m_PogoPinGrid1.SetColumnWidth(col, m_ArrTitleSize[col]);
		}

		for (int row = 1; row < m_PogoPinGrid1.GetRowCount(); row++)
		{
			PogoPinChName.Format("Ch%d", row);
			m_PogoPinGrid1.SetItemText(row, 0, PogoPinChName);
			m_PogoPinGrid1.SetItemState(row, 2, m_PogoPinGrid1.GetItemState(row, 2) | GVIS_READONLY);

		}

		m_PogoPinGrid1.SetListMode(TRUE);
		m_PogoPinGrid1.ShowWindow(SW_SHOW);
		m_btn_Stage1_Apply.ShowWindow(SW_SHOW);
		m_Title_Stage1.ShowWindow(SW_SHOW);
	}
	
	if (bSubStage2Use == true)
	{
		nWidthCount++;

		m_PogoPinGrid2.SetEditable();
		m_PogoPinGrid2.SetVirtualMode(FALSE);
		m_PogoPinGrid2.SetAutoSizeStyle();
		m_PogoPinGrid2.SetBkColor(ColorWhite);

		TRY
		{
			m_PogoPinGrid2.SetColumnCount(nColCount);
			m_PogoPinGrid2.SetRowCount(25);
			m_PogoPinGrid2.SetFixedRowCount(1);
			m_PogoPinGrid2.SetFixedColumnCount(1);
		}
			CATCH(CMemoryException, e)
		{
			e->ReportError();
			return;
		}
		END_CATCH

			for (int col = 0; col < m_PogoPinGrid2.GetColumnCount(); col++)
			{
				CString str;

				GV_ITEM Item;

				Item.mask = GVIF_TEXT;
				Item.row = 0;
				Item.col = col;

				str.Format(_T("%s"), m_strArrTitle[col]);

				Item.strText = str;

				m_PogoPinGrid2.SetItem(&Item);
				m_PogoPinGrid2.SetItemFormat(0, col, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

		for (int col = 0; col < m_PogoPinGrid2.GetColumnCount(); col++)
		{
			m_PogoPinGrid2.SetColumnWidth(col, m_ArrTitleSize[col]);
		}

		for (int row = 1; row < m_PogoPinGrid2.GetRowCount(); row++)
		{
			PogoPinChName.Format("Ch%d", row);
			m_PogoPinGrid2.SetItemText(row, 0, PogoPinChName);
			m_PogoPinGrid2.SetItemState(row, 2, m_PogoPinGrid2.GetItemState(row, 2) | GVIS_READONLY);

		}

		m_PogoPinGrid2.SetListMode(TRUE);
		m_Title_Stage2.ShowWindow(SW_SHOW);
		m_PogoPinGrid2.ShowWindow(SW_SHOW);
		m_btn_Stage2_Apply.ShowWindow(SW_SHOW);
	}
	m_Title_Stage1.SetBkColor(COLOR_WAIT_BTN, COLOR_WAIT_BTN);
	m_Title_Stage1.SetFont("Verdana", 11, 1200);
	m_Title_Stage1.SetTextColor(ColorBlack);

	m_Title_Stage2.SetBkColor(COLOR_WAIT_BTN, COLOR_WAIT_BTN);
	m_Title_Stage2.SetFont("Verdana", 11, 1200);
	m_Title_Stage2.SetTextColor(ColorBlack);

	m_btn_Stage1_Apply.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_btn_Stage1_Apply.SetFont("Verdana", 16, 1400);
	m_btn_Stage1_Apply.SetTextColor(COLOR_BLACK);

	m_btn_Stage2_Apply.SetShade(SHS_HARDBUMP, 8, 10, 62, COLOR_WAIT_BTN);
	m_btn_Stage2_Apply.SetFont("Verdana", 16, 1400);
	m_btn_Stage2_Apply.SetTextColor(COLOR_BLACK);


	MoveWindow(0, 0, nWidth * nWidthCount, nHeight);
	CenterWindow();


}

void CDlgPogoPin::LoadDrawINI()
{
	CStringArray strAry;
	CString strFilePath; strFilePath.Empty();
	CString strAppName; strAppName.Empty();

	strFilePath.Format(_T("%s\\Config\\%s"), GetExePath(), _T("Draw.ini"));
	strAppName.Format(_T("ViewPogoPin"));

	m_nGridRowSize = INIReadInt(strAppName, _T("ListCtrlRowSize"), strFilePath);

	CPnxUtil::GetMgr()->SplitString(_T(INIReadStr(strAppName, _T("GridTitle"), strFilePath)), ',', m_strArrTitle);
	CPnxUtil::GetMgr()->SplitString(_T(INIReadStr(strAppName, _T("GridTitleSize"), strFilePath)), ',', strAry);
	for (int i = 0; i < strAry.GetSize(); ++i)
		m_ArrTitleSize.Add(atoi(strAry.GetAt(i)));
}

bool CDlgPogoPin::LoadPogoPin()
{

	CMgrChannel* pMgr = CMgrChannel::GetMgr();
	CList< CChannel*, CChannel* >& listChannel = pMgr->GetChannelList();

	CMgrCommData* pMgrCommData = CMgrCommData::GetMgr();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		CCommData* lpCyclerInfo = pMgrCommData->GetAt(lpCyclerChannel->GetChannelNumber());
			
		if (lpCyclerChannel->GetChannelNumber() < 24)
		{
			if (lpCyclerInfo->IsContactRegUpdate() == true)
			{
				m_PogoPinGrid1.SetItemText(lpCyclerChannel->GetChannelNumber() + 1, 1, lpCyclerInfo->GetPogoOhmSettingstr());
				m_PogoPinGrid1.SetItemText(lpCyclerChannel->GetChannelNumber() + 1, 2, lpCyclerInfo->GetPogoOhmMonitoringstr());
				m_PogoPinGrid1.Invalidate(false);
				lpCyclerInfo->SetContactRegUptate(false);
			}
		}
		else
		{
			if (lpCyclerInfo->IsContactRegUpdate() == true)
			{
				m_PogoPinGrid2.SetItemText(lpCyclerChannel->GetChannelNumber() - 23, 1, lpCyclerInfo->GetPogoOhmSettingstr());
				m_PogoPinGrid2.SetItemText(lpCyclerChannel->GetChannelNumber() - 23, 2, lpCyclerInfo->GetPogoOhmMonitoringstr());
				m_PogoPinGrid2.Invalidate(false);
				lpCyclerInfo->SetContactRegUptate(false);
			}
		}
	}
	return true;
}


void CDlgPogoPin::OnTimer(UINT_PTR nIDEvent)
{
	if (Formation_PogoPinUpdateTimerId == nIDEvent)
	{
		LoadPogoPin();
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDlgPogoPin::OnDestroy()
{
	CDialogEx::OnDestroy();

	KillTimer(Formation_PogoPinUpdateTimerId);
}


void CDlgPogoPin::OnBnClickedButtonStage1Apply()
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();
	CList< CChannel*, CChannel* >& listChannel = pMgr->GetChannelList();
	CString strSettingValue;
	CMgrCommData* pMgrCommData = CMgrCommData::GetMgr();
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\%s"), (LPCSTR)GetExePath(), _T("Common.ini"));

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		CCommData* lpCyclerInfo = pMgrCommData->GetAt(lpCyclerChannel->GetChannelNumber());
		CString strKeyName;

		if (lpCyclerChannel->GetChannelNumber() < 24)
		{
			strSettingValue = m_PogoPinGrid1.GetItemText(lpCyclerChannel->GetChannelNumber() + 1, 1);
			lpCyclerInfo->SetPogoOhmSetting(atof(strSettingValue));
			
			
			strKeyName.Format(_T("ch%d"), lpCyclerChannel->GetChannelNumber());
			INIWriteFloat("ContactResistance", strKeyName, lpCyclerInfo->GetPogoOhmSetting(), strFilePath);

		}
		else
		{
			continue;
		}
	}
	m_PogoPinGrid1.Invalidate(false);

}


void CDlgPogoPin::OnBnClickedButtonStage2Apply()
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();
	CList< CChannel*, CChannel* >& listChannel = pMgr->GetChannelList();
	CString strSettingValue;
	CMgrCommData* pMgrCommData = CMgrCommData::GetMgr();
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\%s"), (LPCSTR)GetExePath(), _T("Common.ini"));

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		CCommData* lpCyclerInfo = pMgrCommData->GetAt(lpCyclerChannel->GetChannelNumber());
		CString strKeyName;

		if (lpCyclerChannel->GetChannelNumber() >=24)
		{
			strSettingValue = m_PogoPinGrid2.GetItemText(lpCyclerChannel->GetChannelNumber() - 24 + 1, 1);
			lpCyclerInfo->SetPogoOhmSetting(atof(strSettingValue));


			strKeyName.Format(_T("ch%d"), lpCyclerChannel->GetChannelNumber());
			INIWriteFloat("ContactResistance", strKeyName, lpCyclerInfo->GetPogoOhmSetting(), strFilePath);

		}
		else
		{
			continue;
		}
	}
	m_PogoPinGrid2.Invalidate(false);	
}


BOOL CDlgPogoPin::PreCreateWindow(CREATESTRUCT& cs)
{
	bool bSubStage1Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage1Use;
	bool bSubStage2Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage2Use;
	




	return TRUE;
}
