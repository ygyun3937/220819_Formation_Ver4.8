// CDlgViewProcessFormation.cpp: 구현 파일
//

//#include "pch.h"
#include "stdafx.h"
#include "PnxCycler.h"
#include "CDlgViewProcessFormation.h"
#include "afxdialogex.h"

#include "MgrChannel.h"
#include "MgrConfig.h"
#include "MgrComm.h"

//#include "MgrAlarm.h"
//#include "MgrLog.h"
//#include "MgrCommData.h"
//#include "MgrEthernet.h"
//#include "MgrSerial.h"
//#include "MgrDAQ.h"

#include "CMgrAccount.h"
#include "CMgrModel.h"
#include "CMgrScheduleHistory.h"

#include "CDlgViewSchedulelist.h"

// CDlgViewProcessFormation 대화 상자

IMPLEMENT_DYNAMIC(CDlgViewProcessFormation, CDialogEx)

CDlgViewProcessFormation::CDlgViewProcessFormation(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_VIEW_PROCESS_FORMATION, pParent)
{
	m_nScheduleLoadingType = eScheduleLoadingTypeAdd;
}

CDlgViewProcessFormation::~CDlgViewProcessFormation()
{
}

void CDlgViewProcessFormation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgViewProcessFormation, CDialogEx)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()

	ON_BN_CLICKED(IDC_BUTTON_VIEW_PROCESS_FORMATION_SELECT_SCHEDULE, &CDlgViewProcessFormation::OnBnClickedButtonViewProcessFormationSelectSchedule)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_PROCESS_FORMATION_START, &CDlgViewProcessFormation::OnBnClickedButtonViewProcessFormationStart)
	
END_MESSAGE_MAP()


// CDlgViewProcessFormation 메시지 처리기

// -------------------------------------------------------------------------------------------------

void CDlgViewProcessFormation::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CDlgViewProcessFormation::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 __super::OnPaint()을(를) 호출하지 마십시오.
}


BOOL CDlgViewProcessFormation::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return __super::OnEraseBkgnd(pDC);
}


void CDlgViewProcessFormation::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	__super::OnTimer(nIDEvent);
}


void CDlgViewProcessFormation::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	__super::OnClose();
}


void CDlgViewProcessFormation::OnDestroy()
{
	__super::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

// -----------------------------------------------------------------------------

void CDlgViewProcessFormation::OnBnClickedButtonViewProcessFormationSelectSchedule()
{
	SelectSchedule();
}

void CDlgViewProcessFormation::OnBnClickedButtonViewProcessFormationStart()
{
	CMgrModel* pMgr = CMgrModel::GetMgr();
	if (pMgr)
	{
		int nModelIndex = pMgr->GetModelIndex();
		if (nModelIndex < 0)
		{
			AfxMessageBox("모델을 선택하지 않아 진행할 수 없습니다");

			return;
		}
	}

	ExecProcScheduleStart();
}

// -------------------------------------------------------------------------------------------------

//BOOL CDlgViewProcessFormation::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
//{
//	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//
//	return __super::Create(lpszTemplateName, pParentWnd);
//}
BOOL CDlgViewProcessFormation::Create(CWnd * pParentWnd)
{
	//return 0;
	return __super::Create(IDD, pParentWnd);
}

BOOL CDlgViewProcessFormation::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CDlgViewProcessFormation::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return __super::PreTranslateMessage(pMsg);
}

// -------------------------------------------------------------------------------------------------

void CDlgViewProcessFormation::SelectSchedule()
{
	SYSLOG(Operation, _T("작업자가 스케줄 시작을 선택 하였습니다"));

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	if (true == IsSystemError())
	{
		AfxMessageBox("시스템 안전 조건 미 설정으로 진행할 수 없습니다");

		return;
	}

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (false == pMgr->IsCyclerChannelSelected())
	{
		//AfxMessageBox("선택된 채널 정보가 없습니다.");
		//return;

		AfxMessageBox("No channel is selected in that list.\n[ TARGET CHANNEL -> ALL CHANNEL ]");
		SetSelectChannelAll();
	}

	if (false == pMgr->IsCyclerChannelStartEnable())
		return;

	CDlgViewSchedulelist* lpDlg = CDlgViewSchedulelist::GetMgr();

	//병렬이면 Master 또는 Slave 체널까지 선택하는 함수
	SelectMasterSlave();

	if (lpDlg)
	{
		lpDlg->SetEqpProcessMode("");
		lpDlg->SetScheduleLoadingType(eScheduleLoadingTypeAdd);
		
		lpDlg->ShowWindow(SW_HIDE);
		lpDlg->ShowWindow(SW_SHOW);
	}
}

/**
 * .
 * 
 * METHOD_NAME : SelectMasterSlave
 * METHOD_TYPE : -
 * DESCRIPTION : 병렬이면 Master 또는 Slave 체널까지 선택하는 함수
 */
void CDlgViewProcessFormation::SelectMasterSlave()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	int nChannelCountInParallel = pMgr->GetParallelChannelCount();		// 병렬에 필요한 Channel 갯수 마스터 + 슬레이브
	int nChannelCountInOneBoard = pMgr->GetCChannelCountInOneBoard();	// 한개의 인터페이스 보드당 체널 갯수

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	//선택 돤 Channel에 속한 보드에 모든 체널 선택하기.
	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (pChannel->GetSelected() == true)
			{
				int nChannelStartNumber = nChannelCountInParallel * (pChannel->GetChannelNumber() / nChannelCountInParallel); //현재 선택 된 체널에 첫번째 체널 넘버

				//체널 No는 0 부터 시작. 
				for (int i = nChannelStartNumber; i < (nChannelStartNumber + nChannelCountInParallel); i++)
				{
					CChannel* pChannel_1 = CMgrChannel::GetMgr()->GetChannel(i);

					pChannel_1->SetSelected(true);
				}
			}
		}

		Sleep(1);
	}
}

void CDlgViewProcessFormation::SaveHistory()
{
	CMgrScheduleHistory* pMgrScheduleHistory = CMgrScheduleHistory::GetMgr();

	if (pMgrScheduleHistory)
	{
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeUserName, GetUserName());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeSampleName, GetWorkName());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeEquipName, GetEQPID());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeScheduleName, GetScheduleName());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeSchedulePath, GetScheduleFilePath());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeLogFilePath, GetLogFilePath());
		pMgrScheduleHistory->SetNewItem(eScheduleHistoryTypeDescription, GetDescription());
	}
}

/**
 * .
 * 
 * METHOD_NAME : ExecProcScheduleStart
 * METHOD_TYPE : -
 * DESCRIPTION : 
 */
void CDlgViewProcessFormation::ExecProcScheduleStart()
{
	SYSLOG(Operation, _T("작업자 스케줄 시작 명령 수행"));

#ifndef _DEBUG
	if (false == IsEnableScheduleStart())
	{
		AfxMessageBox("충방전기가 연결되어 있지 않아 시작 할 수 없습니다.");

		return;
	}
#endif

	// 없어서는 안되는 항목들 체크 하고 넘어가자

	int nStartCycle = GetStartCycle();		// FIX=1
	int nStartStep = GetStartStep();		// FIX=1

	if (nStartCycle <= 0 || nStartStep <= 0)
	{
		AfxMessageBox("레시피의 시작 위치를 지정 해주지 않아 시작 할 수 없습니다.");
		return;
	}

	// 여기서 동기화 선택 여부에 따라서 첫번째 예외처리를 한다.
	int nSyncMode = GetSyncMode();

	if (eScheduleSyncModeStep == nSyncMode)
	{
		if (false == IsStepSyncValidate())
		{
			AfxMessageBox("스텝 동기화 시에는 2개 이상의 채널을 선택한 상태에서 진행 가능 합니다.");
			return;
		}
	}
	else if (eScheduleSyncModeTemperature == nSyncMode)
	{
		if (false == IsTemperatureSyncValidate())
		{
			AfxMessageBox("온도 동기화 시에는 챔버 온도 변경 내역이 있는 상태에서 진행 가능 합니다.");
			return;
		}
	}

	CMgrAccount* pMgrAccount = CMgrAccount::GetMgr();
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	if (pMgrAccount)
	{
		sAccount* lpAccount = pMgrAccount->GetLoginedAccount();

		if (lpAccount)
		{
			CString strFilePath;
			
			//GetDlgItem(IDC_EDIT_EDIT_SCHEDULE_INFO_FILE_PATH)->GetWindowText(strFilePath);
			strFilePath = pMgrConfig->GetLogFilePath();		// FORMATION 

			_tcscpy_s(lpAccount->szSaveFilePath, strFilePath);

			pMgrAccount->SaveAccount();
		}
	}

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		SaveHistory();

		SStartSchedule kParser;

		kParser.nStartCycleNumber = nStartCycle;
		kParser.nStartStepNumber = nStartStep;
		kParser.nSyncMode = nSyncMode;
		kParser.strEQPID = GetEQPID();
		kParser.strScheduleFilePath = GetScheduleFilePath();
		kParser.strScheduleName = GetScheduleName();
		kParser.strUserID = GetUserName();
		kParser.strWorkName = GetWorkName();
		kParser.strLogFilePath = GetLogFilePath();
		kParser.bScheduleName = IsDlgButtonChecked(IDC_CHECK_SCHNAME);
		kParser.bTime = IsDlgButtonChecked(IDC_CHECK_TIME);

		GetCellID(kParser.vecCellID);

		GetCyclerChannelNumber(kParser.vecCyclerChannelNumber);

		//중복 불가하게 수정해야함 pnx_hr 2021.04.29
		if (eScheduleLoadingTypeAdd == m_nScheduleLoadingType)
		{
			pMgr->StartSchedule(kParser);		// START PROCESS
		}
		else
			pMgr->ScheduleAppend(kParser);
		TRACE("m_nScehduleLoadingType = %d", m_nScheduleLoadingType);

		CWnd* pWnd = AfxGetApp()->GetMainWnd();
		if (pWnd)
		{
			auto iterE = kParser.vecCyclerChannelNumber.end();
			iterE--;
			::PostMessage(pWnd->GetSafeHwnd(), WM_VIEW_PATTERN, 0, *iterE);
		}

		//ShowWindow(SW_HIDE);
	}

}

void CDlgViewProcessFormation::GetCellID(std::vector<CString>& vecValue)
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		int nSelectedCyclerChannelCount = pMgr->GetSelectCyclerChannelCount();

		for (int i = 0; i < nSelectedCyclerChannelCount; ++i)
		{
			CString strValue;
			//strValue = m_listMaterial.GetItemText(i, 2); CELL_ID		// 확인필요
			strValue.Format(_T("TempCellId_%d"), i);
			vecValue.push_back(strValue);
		}
	}
}

void CDlgViewProcessFormation::GetCyclerChannelNumber(std::vector<int>& vecValue)
{
	vecValue.clear();

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

	auto Pos = listCyclerChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if (!lpCyclerChannel->GetSelected())
			continue;

		vecValue.push_back(lpCyclerChannel->GetChannelNumber());
	}
}

// -----------------------------------------------------------------------------

/**
 * .
 * 
 * METHOD_NAME : IsStepSyncValidate
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
bool CDlgViewProcessFormation::IsStepSyncValidate()
{
	// 스텝 동기화를 할때는 2개 이상의 채널이 선택되어 있어야 한다.
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return false;

	int nSelectCyclerChannelCount = pMgr->GetSelectCyclerChannelCount();

	if (nSelectCyclerChannelCount <= 1)
		return false;

	int nChamberKey = -1;

	CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

	auto Pos = listCyclerChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

		if (nullptr == lpCyclerChannel)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		CMgrComm* pMgrComm = CMgrComm::GetMgr();

		if (!pMgrComm)
			continue;

		if (pMgrComm->GetDeviceKeyFromType(eDeviceTypeChamber, lpCyclerChannel->GetChannelNumber()) < 0)
			return false;

		if (nChamberKey == -1)
			nChamberKey = pMgrComm->GetDeviceKeyFromType(eDeviceTypeChamber, lpCyclerChannel->GetChannelNumber());
		else
		{
			if (nChamberKey != pMgrComm->GetDeviceKeyFromType(eDeviceTypeChamber, lpCyclerChannel->GetChannelNumber()))
				return true;
		}
	}

	return true;
}

bool CDlgViewProcessFormation::IsTemperatureSyncValidate()
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return false;

	CChannel* lpCyclerChannel = pMgr->GetSelectedChannel();

	if (!lpCyclerChannel)
		return false;

	CMgrSchedule* lpScheduleInfo = lpCyclerChannel->GetMgr();

	if (!lpScheduleInfo)
		return false;

	// 지금 선택한 채널이 1개인 경우 -> 나머지 채널이 진행중인지 체크
	// 3. 나머지 채널 진행중이다 and 지금 채널이 온도 변경이 있다 = false;		ok
	// 4. 나머지 채널 진행중이다 and 직므 채널이 온도 변경이 없다  = true;		ok

	int nSelectCyclerChannelCount = pMgr->GetSelectCyclerChannelCount();

	if (1 == nSelectCyclerChannelCount)
	{
		CMgrComm* pMgrComm = CMgrComm::GetMgr();

		if (!pMgrComm)
			return false;

		int nExecCyclerChannel = pMgrComm->GetCyclerChannelFromDevice(eDeviceTypeChamber, lpCyclerChannel->GetChannelNumber());

		if (nExecCyclerChannel < 0)
			return false;

		CChannel* lpExeCyclerChannel = pMgr->GetChannel(nExecCyclerChannel);

		if (!lpExeCyclerChannel)
			return false;

		if (true == lpExeCyclerChannel->GetScheduleStart())
		{
			{
				CMgrSchedule* lpExeSchedule = lpExeCyclerChannel->GetMgr();

				if (!lpExeSchedule)
					return false;

				CList< CStep*, CStep* >& listExeStep = lpExeSchedule->GetStepList();

				auto Pos = listExeStep.GetHeadPosition();

				while (Pos)
				{
					CStep* lpStep = listExeStep.GetNext(Pos);

					if (!lpStep)
						continue;

					float fValue = lpStep->GetChamberSetTemp();

					// 얘는 이미 돌고 있는 스케줄이 온도 변경하는 놈이다.
					if (fValue != 0.000f)
						return false;
				}
			}

			{
				CMgrSchedule* lpModifyScheduleInfo = new CMgrSchedule;
				lpModifyScheduleInfo->LoadScheduleXml(GetScheduleFilePath());

				CList< CStep*, CStep* >& listStep = lpModifyScheduleInfo->GetStepList();

				if (listStep.GetSize() <= 0)
				{
					delete lpModifyScheduleInfo;
					lpModifyScheduleInfo = nullptr;

					return false;
				}

				auto Pos = listStep.GetHeadPosition();

				while (Pos)
				{
					CStep* lpStep = listStep.GetNext(Pos);

					if (!lpStep)
						continue;

					float fValue = lpStep->GetChamberSetTemp();

					if (fValue != 0.000f)
					{
						delete lpModifyScheduleInfo;
						lpModifyScheduleInfo = nullptr;

						return false;
					}
				}

				delete lpModifyScheduleInfo;
				lpModifyScheduleInfo = nullptr;
			}
		}
	}

	{
		CMgrSchedule* lpModifyScheduleInfo = new CMgrSchedule;
		lpModifyScheduleInfo->LoadScheduleXml(GetScheduleFilePath());

		CList< CStep*, CStep* >& listStep = lpModifyScheduleInfo->GetStepList();

		if (listStep.GetSize() <= 0)
		{
			delete lpModifyScheduleInfo;
			lpModifyScheduleInfo = nullptr;

			return false;
		}

		auto Pos = listStep.GetHeadPosition();

		while (Pos)
		{
			CStep* lpStep = listStep.GetNext(Pos);

			if (!lpStep)
				continue;

			float fValue = lpStep->GetChamberSetTemp();

			if (fValue != 0.000f)
			{
				delete lpModifyScheduleInfo;
				lpModifyScheduleInfo = nullptr;

				return true;
			}
		}

		delete lpModifyScheduleInfo;
		lpModifyScheduleInfo = nullptr;

		return false;
	}
}

/**
 * .
 *
 * METHOD_NAME : GetStartCycle
 * METHOD_TYPE : -
 * DESCRIPTION :
 * \return
 */
int CDlgViewProcessFormation::GetStartCycle()
{
	CString strValue;
	strValue = "1";
	return atoi(strValue);
}

/**
 * .
 * 
 * METHOD_NAME : GetStartStep
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
int CDlgViewProcessFormation::GetStartStep()
{
	CString strValue;
	strValue = "1";
	return atoi(strValue);	
}

/**
 * .
 * 
 * METHOD_NAME : GetSyncMode
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
int CDlgViewProcessFormation::GetSyncMode()
{
	return eScheduleSyncModeNotUse;
}

// -----------------------------------------------------------------------------

CString CDlgViewProcessFormation::GetDescription()
{
	CString strValue;
	//GetDlgItem(IDC_EDIT_EDIT_SCHEDULE_INFO_DESCRIPTION)->GetWindowText(strValue);
	return strValue;
}

/**
 * .
 * 
 * METHOD_NAME : GetEQPID
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
CString CDlgViewProcessFormation::GetEQPID()
{
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	if (pMgrConfig)
	{
		return pMgrConfig->GetCyclerEQPID();
	}

	return "";
}

/**
 * .
 * 
 * METHOD_NAME : GetLogFilePath
 * METHOD_TYPE : -
 * DESCRIPTION : SCHEDULE
 * \return 
 */
CString CDlgViewProcessFormation::GetLogFilePath()
{
	CString strValue;

	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	if (pMgrConfig)
	{
		strValue = pMgrConfig->GetLogFilePath();
	}

	return strValue;
}

/**
 * .
 * 
 * METHOD_NAME : GetScheduleName
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
CString CDlgViewProcessFormation::GetScheduleName()
{
	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		SModel* lpModel = pMgr->GetModel(pMgr->GetModelIndex());

		if (!lpModel)
			return "";

		SSchedule* lpSchedule = lpModel->GetSchedule(pMgr->GetScheduleIndex());

		if (!lpSchedule)
			return "";

		return lpSchedule->strScheduleName;
	}

	return "";
}

CString CDlgViewProcessFormation::GetScheduleFilePath()
{
	CMgrModel* pMgr = CMgrModel::GetMgr();

	if (pMgr)
	{
		SModel* lpModel = pMgr->GetModel(pMgr->GetModelIndex());

		if (!lpModel)
			return "";

		SSchedule* lpSchedule = lpModel->GetSchedule(pMgr->GetScheduleIndex());

		if (!lpSchedule)
			return "";

		return lpSchedule->strScheduleFilePath;
	}

	return "";
}

/**
 * .
 * 
 * METHOD_NAME : GetUserName
 * METHOD_TYPE : -
 * DESCRIPTION : MES연동?, 현재 공백(EMPTY)로 처리
 * \return 
 */
CString CDlgViewProcessFormation::GetUserName()
{
	CString strValue;
	//GetDlgItem(IDC_EDIT_EDIT_SCHEDULE_INFO_WORKER)->GetWindowText(strValue);
	
	if (strValue.IsEmpty())
	{
		// MES 연동 부분에 작업자, 설명이 있다. 이걸로 넣어야 하나??
		CMgrAccount* pMgrAccount = CMgrAccount::GetMgr();
		if (pMgrAccount)
		{
			return pMgrAccount->GetLoginID();
		}
	}

	return "";
}

/**
 * .
 * 
 * METHOD_NAME : GetWorkName
 * METHOD_TYPE : -
 * DESCRIPTION : 작업명(?)
 * \return 
 */
CString CDlgViewProcessFormation::GetWorkName()
{
	CString strValue;
	//GetDlgItem(IDC_EDIT_EDIT_SCHEDULE_INFO_WORK_NAME)->GetWindowText(strValue);

	return strValue;
}


// -----------------------------------------------------------------------------
/**
 * .
 * 
 * METHOD_NAME : IsSystemError
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
bool CDlgViewProcessFormation::IsSystemError()
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();

	if (!pMgr)
	{
		return true;
	}

	return pMgr->IsErrorSystemSafety();
}

// -------------------------------------------------------------------------------------------------

void CDlgViewProcessFormation::SetSelectChannelAll()
{
	CMgrChannel* pMgrChannel = CMgrChannel::GetMgr();
	int nTotalChannelCount = CMgrChannel::GetMgr()->GetChannelList().GetCount();

	//체널 No는 0 부터 시작. 
	for (int nChannelCount = 0; nChannelCount < nTotalChannelCount; nChannelCount++)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelCount);
		pChannel->SetSelected(true);
	}
}








