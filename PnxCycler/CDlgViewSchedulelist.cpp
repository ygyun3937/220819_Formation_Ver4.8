// CDlgViewSchedulelist.cpp: 구현 파일
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewSchedulelist.h"
#include "afxdialogex.h"
#include "MgrConfig.h"
#include "MgrChannel.h"
#include "CMgrModel.h"
#include "CDlgViewScheduleInfo.h"
#include "CDlgEditScheduleInfo.h"

//#include "CDlgViewProcessFormation.h"
#include "CDlgViewEqpChannelStatus.h"

#include "DlgTotalSafetyCondition.h"

// CDlgViewSchedulelist 대화 상자

IMPLEMENT_DYNAMIC(CDlgViewSchedulelist, CDialogEx)

CDlgViewSchedulelist::CDlgViewSchedulelist(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VIEW_SCHEDULE_LIST, pParent)
{
	//m_nModelIndex			= -1;
	//m_nScheduleIndex		= -1;

	memset(m_nArrayModelIndex, -1, sizeof(m_nArrayModelIndex));
	memset(m_nArrayScheduleIndex, -1, sizeof(m_nArrayScheduleIndex));

	m_nStageNo = -1;
	m_nScheduleLoadingType	= eScheduleLoadingTypeAdd;

	
}

CDlgViewSchedulelist::~CDlgViewSchedulelist()
{
}

void CDlgViewSchedulelist::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_VIEW_SCHEDULE_MODEL, m_listModel);
	DDX_Control(pDX, IDC_LIST_VIEW_SCHEDULE_SCHEDULE, m_listSchedule);
}


BEGIN_MESSAGE_MAP(CDlgViewSchedulelist, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_VIEW_SCHEDULE_DETAIL, &CDlgViewSchedulelist::OnBnClickedBtnViewScheduleDetail)
	ON_BN_CLICKED(IDC_BTN_VIEW_SCHEDULE_APPLY, &CDlgViewSchedulelist::OnBnClickedBtnViewScheduleApply)
	ON_BN_CLICKED(IDC_BTN_VIEW_SCHEDULE_CANCEL, &CDlgViewSchedulelist::OnBnClickedBtnViewScheduleCancel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_VIEW_SCHEDULE_MODEL, &CDlgViewSchedulelist::OnClickListViewScheduleModel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_VIEW_SCHEDULE_SCHEDULE, &CDlgViewSchedulelist::OnClickListViewScheduleSchedule)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------

// CDlgViewSchedulelist 메시지 처리기


BOOL CDlgViewSchedulelist::Create(CWnd* pParentWnd)
{
	return CDialogEx::Create(IDD, pParentWnd);
}

BOOL CDlgViewSchedulelist::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam)
		{
			ExecProcEditSchedule();

			return TRUE;
		}

		if (VK_SPACE == pMsg->wParam)
		{
			ExecProcScheduleView();
		}

		if (VK_ESCAPE == pMsg->wParam)
		{
			ShowWindow(SW_HIDE);

			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CDlgViewSchedulelist::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitStatic();

	InitListCtrl();

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgViewSchedulelist::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (TRUE == bShow)
	{
		LoadSchedule();

		//m_nModelIndex		= -1;
		//m_nScheduleIndex	= -1;

		memset(m_nArrayModelIndex, -1, sizeof(m_nArrayModelIndex));
		memset(m_nArrayScheduleIndex, -1, sizeof(m_nArrayScheduleIndex));

		DrawListFromSchedule();

		GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_VALUE_1)->SetWindowText("");
		GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_VALUE_2)->SetWindowText("");

		if (eScheduleLoadingTypeAdd == m_nScheduleLoadingType)
		{
			//GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_TITLE_1)->SetWindowText("다음 목록에서 적용할 스케줄을 선택 해주세요");
			GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_TITLE_1)->SetWindowText("Please select a schedule to apply from the following list.");
		}
		else
		{
			//GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_TITLE_1)->SetWindowText("다음 목록에서 추가 예약 할 스케줄을 선택 해주세요");
			GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_TITLE_1)->SetWindowText("Please select a schedule to make additional reservations from the list below.");
		}
			
	}
	else
	{
		CMgrModel* pMgrModel = CMgrModel::GetMgr();
		if (pMgrModel)
		{
			CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
			//220124YGY Test 위해 임시 주석 처리 예외처리 다시 봐야됨

			//if (pMgrConfig)
			//{
			//	pMgrModel->SetModelIndex(-1);
			//	pMgrModel->SetScheduleIndex(-1);
			//}
			//else
			//{
			//	pMgrModel->SetModelIndex(-1);
			//	pMgrModel->SetScheduleIndex(-1);
			//}
		}
	}
}

void CDlgViewSchedulelist::OnClose()
{
	CDialogEx::OnClose();
}

void CDlgViewSchedulelist::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

HBRUSH CDlgViewSchedulelist::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(ColorWhite);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	else if (CTLCOLOR_DLG == nCtlColor)
	{
		pDC->SetBkColor(ColorWhite);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return hbr;
}

// -------------------------------------------------------------------------------------------------

void CDlgViewSchedulelist::OnBnClickedBtnViewScheduleDetail()
{
	ExecProcScheduleView();
}

void CDlgViewSchedulelist::OnBnClickedBtnViewScheduleApply()
{
	ExecProcEditSchedule();
	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (m_nArrayModelIndex[GetStageNo()] >= 0 && m_nArrayScheduleIndex[GetStageNo()] >= 0)
	{
		if (pMgr)
		{
			SModel* lpModel = pMgr->GetModel(m_nArrayModelIndex[GetStageNo()]);

			if (lpModel)
			{
				SSchedule* lpSchedule = lpModel->GetSchedule(m_nArrayScheduleIndex[GetStageNo()]);
				if (lpSchedule)
				{
					CDlgViewEqpChannelStatus* pDlgViewEqpChannelStatus = CDlgViewEqpChannelStatus::GetMgr();
					if (GetStageNo() == 0)
					{
						pDlgViewEqpChannelStatus->m_Status_Schedule.SetWindowText(lpSchedule->strScheduleName);
						pDlgViewEqpChannelStatus->m_Status_Schedule.SetBkColor(ColorBlue, ColorBlue);
						pDlgViewEqpChannelStatus->m_Status_Schedule.SetFont("Verdana", 14, 1400);
						pDlgViewEqpChannelStatus->m_Status_Schedule.SetTextColor(ColorWhite);
						pDlgViewEqpChannelStatus->m_Status_Schedule.Invalidate();
					}
					else
					{
						pDlgViewEqpChannelStatus->m_Status_Stage2_Schedule.SetWindowText(lpSchedule->strScheduleName);
						pDlgViewEqpChannelStatus->m_Status_Stage2_Schedule.SetBkColor(ColorBlue, ColorBlue);
						pDlgViewEqpChannelStatus->m_Status_Stage2_Schedule.SetFont("Verdana", 14, 1400);
						pDlgViewEqpChannelStatus->m_Status_Stage2_Schedule.SetTextColor(ColorWhite);
						pDlgViewEqpChannelStatus->m_Status_Stage2_Schedule.Invalidate();
					}
					pMgr->SetStageScheduleSelect(GetStageNo(), true);
				}
			}
		}
	}

}


void CDlgViewSchedulelist::OnBnClickedBtnViewScheduleCancel()
{
	// 고민해봐야 한다. 취소이기 때문에 불러온 스케줄 정보를 여기서 삭제 해주나?
	ShowWindow(SW_HIDE);
}


void CDlgViewSchedulelist::OnClickListViewScheduleModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_nArrayModelIndex[GetStageNo()] = pNMItemActivate->iItem;
	//m_nScheduleIndex = -1;

	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		pMgr->SetModelIndex(GetStageNo(),m_nArrayModelIndex[GetStageNo()]);
		//pMgr->SetScheduleIndex(m_nScheduleIndex);

		// ---------------------------------------------------------------------
		//Stage 별 관리 필요
		SModel* lpModel = pMgr->GetModel(m_nArrayModelIndex[GetStageNo()]);
		if (lpModel)
		{
			CDlgViewEqpChannelStatus* pDlgViewEqpChannelStatus = CDlgViewEqpChannelStatus::GetMgr();
			if (GetStageNo() == 0)
			{
				pDlgViewEqpChannelStatus->m_Status_Schedule_Model.SetWindowText(lpModel->strModelName);
				pDlgViewEqpChannelStatus->m_Status_Schedule_Model.SetBkColor(ColorBlue, ColorBlue);
				pDlgViewEqpChannelStatus->m_Status_Schedule_Model.SetFont("Verdana", 14, 1400);
				pDlgViewEqpChannelStatus->m_Status_Schedule_Model.SetTextColor(ColorWhite);
				pDlgViewEqpChannelStatus->m_Status_Schedule_Model.Invalidate();
			}
			else
			{
				pDlgViewEqpChannelStatus->m_Status_Schedule_Stage2_Model.SetWindowText(lpModel->strModelName);
				pDlgViewEqpChannelStatus->m_Status_Schedule_Stage2_Model.SetBkColor(ColorBlue, ColorBlue);
				pDlgViewEqpChannelStatus->m_Status_Schedule_Stage2_Model.SetFont("Verdana", 14, 1400);
				pDlgViewEqpChannelStatus->m_Status_Schedule_Stage2_Model.SetTextColor(ColorWhite);
				pDlgViewEqpChannelStatus->m_Status_Schedule_Stage2_Model.Invalidate();
			}
		}
	}

	DrawListFromSchedule();

	*pResult = 0;
}

void CDlgViewSchedulelist::OnClickListViewScheduleSchedule(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_nArrayScheduleIndex[GetStageNo()] = pNMItemActivate->iItem;

	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		pMgr->SetScheduleIndex(GetStageNo(),m_nArrayScheduleIndex[GetStageNo()]);
	}

	if (m_nArrayModelIndex[GetStageNo()] >= 0 && m_nArrayScheduleIndex[GetStageNo()] >= 0)
	{
		if (pMgr)
		{
			SModel* lpModel = pMgr->GetModel(m_nArrayModelIndex[GetStageNo()]);

			if (lpModel)
			{
				//Stage 별 관리 필요
				SSchedule* lpSchedule = lpModel->GetSchedule(m_nArrayScheduleIndex[GetStageNo()]);
				if (lpSchedule)
				{
					GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_VALUE_2)->SetWindowText(lpSchedule->strScheduleName);
					GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_VALUE_2)->Invalidate();

					// ---------------------------------------------------------
				}
			}
		}
	}
	else
	{
		GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_VALUE_2)->SetWindowText("");
	}

	*pResult = 0;
}

// -------------------------------------------------------------------------------------------------

void	CDlgViewSchedulelist::InitStatic()
{
	//LPCTSTR lpszText[] = {"다음 목록에서 적용할 스케줄을 선택 해주세요", "선택 모델", "선택 스케줄", "구 분", "1. 모델 목록 선택", "2. 스케줄 목록 선택", "", "" };
	LPCTSTR lpszText[] = { "Please select a schedule to apply from the following list.", "SELECTED MODEL", "SELECTED SCHUEDULE", "DIVISION", "1. SELECT MODEL LIST", "2. SELECT SCHEDULE LIST", "", "" };

	for (int i = 0; i < eViewScheduleStaticObjectMaxCount; ++i)
		CST_DEFAULT(m_ctrlText[i], IDC_STATIC_VIEW_SCHEDULE_TITLE_1 + i, this, 12, 0, TRUE, ColorWhite, ColorBlack, lpszText[i]);

	// ----------------------------------------------------------------------------

	GetDlgItem(IDC_GROUP_VIEW_SHCEUDLE)->SetWindowText(_T("SELECT SCHEDHULE LIST"));
	GetDlgItem(IDC_BTN_VIEW_SCHEDULE_DETAIL)->SetWindowText(_T("DETAIL"));
	GetDlgItem(IDC_BTN_VIEW_SCHEDULE_APPLY)->SetWindowText(_T("APPLY"));
	GetDlgItem(IDC_BTN_VIEW_SCHEDULE_CANCEL)->SetWindowText(_T("CANCEL"));
}

void	CDlgViewSchedulelist::InitListCtrl()
{
	//LPCTSTR lpszText[eViewScheduleListObejctMaxCount] = { "순번", "이름", "설명", "등록일"};
	LPCTSTR lpszText[eViewScheduleListObejctMaxCount] = { "No.", "NAME", "DESCRIPTION", "MODIFY DATE" };

	m_listModel.InsertColumn(0, "", LVCFMT_CENTER, 0);
	m_listSchedule.InsertColumn(0, "", LVCFMT_CENTER, 0);

	for (int i = 0; i < eViewScheduleListObejctMaxCount; ++i)
	{
		m_listModel.InsertColumn(i + 1, lpszText[i], LVCFMT_CENTER, GetListCtrlColumnWidth(i));
		m_listSchedule.InsertColumn(i + 1, lpszText[i], LVCFMT_CENTER, GetListCtrlColumnWidth(i));
	}

	m_listModel.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listSchedule.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
}

void	CDlgViewSchedulelist::LoadSchedule()
{
	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		pMgr->LoadModel();

		DrawListFromModel();
	}
}

void	CDlgViewSchedulelist::DrawListFromModel()
{
	m_listModel.DeleteAllItems();

	CMgrModel* pMgr = CMgrModel::GetMgr();

	std::vector<SModel*>& vecModel = pMgr->GetModelVector();

	auto iter	= vecModel.begin();
	auto iterE	= vecModel.end();

	int nIndex = 0;

	while (iter != iterE)
	{
		SModel* lpModel = (*iter);

		if (lpModel)
		{
			LVITEM Item;

			Item.iItem		= nIndex;
			Item.mask		= LVIF_TEXT | LVIF_PARAM;
			Item.stateMask	= LVIS_STATEIMAGEMASK;
			Item.state		= INDEXTOSTATEIMAGEMASK(1);
			Item.pszText	= _T("");
			Item.iSubItem	= 0;

			m_listModel.InsertItem(&Item);

			m_listModel.SetItemText(nIndex, 0, "");
			m_listModel.SetItemText(nIndex, 1, lpModel->strSerialNumber);
			m_listModel.SetItemText(nIndex, 2, lpModel->strModelName);
			m_listModel.SetItemText(nIndex, 3, lpModel->strModelDescription);
			m_listModel.SetItemText(nIndex, 4, lpModel->strWriteTime);

			nIndex++;
		}		

		++iter;
	}
}

void	CDlgViewSchedulelist::DrawListFromSchedule()
{
	m_listSchedule.DeleteAllItems();

	GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_VALUE_1)->SetWindowText("");
	GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_VALUE_2)->SetWindowText("");

	if (m_nArrayModelIndex[GetStageNo()] >= 0)
	{
		CMgrModel* pMgr = CMgrModel::GetMgr();

		if (pMgr)
		{
			SModel* lpModel = pMgr->GetModel(m_nArrayModelIndex[GetStageNo()]);

			if (!lpModel)
				return;

			GetDlgItem(IDC_STATIC_VIEW_SCHEDULE_VALUE_1)->SetWindowText(lpModel->strModelName);

			vector<SSchedule*>& vecSchedule = lpModel->GetScheduleVector();

			auto iter	= vecSchedule.begin();
			auto iterE	= vecSchedule.end();

			int nIndex = 0;

			while (iter != iterE)
			{
				SSchedule* lpSchedule = (*iter);

				if (lpSchedule)
				{
					LVITEM Item;

					Item.iItem		= nIndex;
					Item.mask		= LVIF_TEXT | LVIF_PARAM;
					Item.stateMask	= LVIS_STATEIMAGEMASK;
					Item.state		= INDEXTOSTATEIMAGEMASK(1);
					Item.pszText	= _T("");
					Item.iSubItem	= 0;

					m_listSchedule.InsertItem(&Item);

					m_listSchedule.SetItemText(nIndex, 0, "");
					m_listSchedule.SetItemText(nIndex, 1, lpSchedule->strSerialNumber);
					m_listSchedule.SetItemText(nIndex, 2, lpSchedule->strScheduleName);
					m_listSchedule.SetItemText(nIndex, 3, lpSchedule->strScheduleDescription);
					m_listSchedule.SetItemText(nIndex, 4, lpSchedule->strModifyTime);

					nIndex++;
				}				

				++iter;
			}
		}		
	}
}

void	CDlgViewSchedulelist::ExecProcScheduleView()
{
	if (m_nArrayModelIndex[GetStageNo()] < 0)
	{
		AfxMessageBox("Can't proceed because no model has been selected");

		return;
	}
	if (m_nArrayScheduleIndex[GetStageNo()] < 0)
	{
		AfxMessageBox("Can't proceed because no schedule has been selected");

		return;
	}

	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		SModel* lpModel = pMgr->GetModel(m_nArrayModelIndex[GetStageNo()]);

		if (lpModel)
		{
			SSchedule* lpSchedule = lpModel->GetSchedule(m_nArrayScheduleIndex[GetStageNo()]);

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
}

void	CDlgViewSchedulelist::ExecProcEditSchedule()
{
	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		int nModelIndex = pMgr->GetModelIndex(GetStageNo());

		if (nModelIndex < 0)
		{
			AfxMessageBox("Can't proceed because no model has been selected");

			return;
		}

		int nScheduleIndex = pMgr->GetScheduleIndex(GetStageNo());

		if (nScheduleIndex < 0)
		{
			AfxMessageBox("Can't proceed because no schedule has been selected");

			return;
		}

		SModel* lpModel = pMgr->GetModel(nModelIndex);
		if (!lpModel)
		{
			AfxMessageBox("Unable to load registered model information");

			return;
		}

		SSchedule* lpSchedule = lpModel->GetSchedule(nScheduleIndex);
		if (!lpSchedule)
		{
			AfxMessageBox("Unable to load registered schedule information");
			return;
		}


		// 만약 상세보기 없이 진행한거라면.. 여기서 다시 읽어야 다음 화면에서 정상적으로 처리 된다.
		/*
		{
			CMgrChannel* pMgrChannel = CMgrChannel::GetMgr();

			if (pMgrChannel)
			{
				CChannel* lpCyclerChannel = pMgrChannel->GetSelectedChannel();

				if (lpCyclerChannel)
				{
					CMgrSchedule* lpScheduleInfo = lpCyclerChannel->GetMgr();

					if (lpScheduleInfo)
					{
						lpScheduleInfo->ClearSchedule();
						lpScheduleInfo->LoadScheduleXml(lpSchedule->strScheduleFilePath);
					}
				}
			}
		}
		*/
#if TOTAL_SAFETY_ENABLE	// Total safety 일단 주석, 검증 후 활성화 2021-03-08
		// 통합 안전조건
		CDlgTotalSafetyCondition dlg;
		if (dlg.DoModal() == IDCANCEL) {
			return;
		}
#endif // 0

		// ---------------------------------------------------------------------------

		// CYCLER 
		/*CDlgEditScheduleInfo* lpDlg = CDlgEditScheduleInfo::GetMgr();

		if (lpDlg)
		{
			lpDlg->SetScheduleLoadingType(m_nScheduleLoadingType);

			BOOL bIsResult = lpDlg->LoadScheduleCombo();
			if (bIsResult == FALSE)
			{
				return;
			}

			lpDlg->ShowWindow(SW_SHOW);
		}*/

		this->ShowWindow(SW_HIDE);
	}
}

int		CDlgViewSchedulelist::GetListCtrlColumnWidth(int nColumnIndex)
{
	switch (nColumnIndex)
	{
	case eViewScheduleListObejctSerial		: return 50;
	case eViewScheduleListObejctName		: return 335;
	case eViewScheduleListObejctDescription	: return 150;
	case eViewScheduleListObejctWriteTime	: return 210;
	}

	return 0;
}


