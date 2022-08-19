#include "stdafx.h"
#include "Channel.h"

#include "MgrConfig.h"
#include "MgrCommData.h"
#include "MgrComm.h"
#include "PnxUtil.h"
#include "SockProc.h"
#include "ComProc.h"
#include "MgrDAQ.h"
#include "MgrLog.h"
#include "CMgrAccount.h"
#include "CMgrChamber.h"
#include "MgrStatus.h"
#include "MgrAlarm.h" //lyw4_1
#include "DlgViewAlarm.h" //lyw4_1
#include <algorithm>
#include "SerialCom.h"
#include "MgrChannel.h"
#include "MgrOption.h"

#include "CCalibratorVisaProcess.h"		// AUTO_CALIBRATION
#include "CMgrPid.h"					// AUTO_CALIBRATION
#include "MgrDio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define	NextStep		( 65533 )
#define NextPause		( 65534 )
#define NextComplete	( 65535 )

CChannel::CChannel()
{
	//m_nChannelStatus				= eCyclerChannelStateError;
	m_nChannelStatus = eCyclerChannelStateIdle;
	m_nChannelNumber = 0;
	m_nCycleIndex = 0;
	m_nCycleNumber = 0;
	m_nStepNumber = 0;
	m_nScheduleMode = -1;
	m_nOperationMode = 0;
	m_nChamberDelayTime = 0;
	m_nAccurateReason = 0;
	m_nProcessStatus = 0;
	m_nRunningCount = 0;
	m_nEndSync = 0;
	m_nSyncChannel1 = 0;
	m_nSyncChannel2 = 0;
	m_nSyncType = 0;
	m_nSafetyNumber = 0;
	m_nSystemSafetyReason = 0;	//lyw3_1
	m_bChamberElevatedTemperature = false;
	m_nReservePauseCycleNumber = 0;
	m_nReservePauseStepNumber = 0;
	m_nReserveCompleteCycleNumber = 0;
	m_nReserveCompleteStepNumer = 0;
	m_bStepSync = false;
	m_bTempSync = false;

	m_strChamberChannel.Empty();
	m_strRunChannel.Empty();
	m_strEndCondition.Empty();
	m_strFileName.Empty();
	m_strScheduleName.Empty();
	m_strRating = _T("");

	m_bSync = false;
	m_bChamberChage = false;
	m_bChannelStart = false;
	m_bScheduleStart = false;
	m_bPause = false;
	m_bSelected = false;
	m_bPauseCycle = false;
	m_bPauseStep = false;
	m_bCompleteCycle = false;
	m_bCompleteStep = false;
	m_bPattern = false;
	m_bAlarm = false;
	m_bIsSendCommandJump = false;
	m_bIsCycleChange = false;
	m_bIsStepChange = false;
	m_bSafetyPause = false;
	m_bDataSendPacket = false;
	m_bChannelEnd = false;

	m_bExternPatternCheck = false;

	m_pMgr = new CMgrSchedule();

	m_nVPCheckTime = 0;
	m_nVPCheckCnt = 0;
	m_nIPCheckTime = 0;

	m_eMasterChannel = ParallelMaster; // ���� ��� ��� �⺻ ���� ������ Master�� �Ѵ�.
	m_eEQModeSingle = EqModeSerial;
	m_pStepTarget = NULL;

	sprintf_s(m_szFileID, _T("-1"));
	sprintf_s(m_szSchStart, _T("-1"));

	m_ulBoardTime = 0;

	Remove_MapCurrentCalData();
	Remove_MapVoltCalData();
}


CChannel::~CChannel()
{
	DELETE_POINTER(m_pMgr);
}

int		CChannel::GetSafetyNumber()
{
	return m_nSafetyNumber;
}

//���� �Ͻ����� 
//��Ȱ��ȭ : nCycleNumber(0), nStepNumber(0)
void	CChannel::SetReservePause(int nCycleNumber, int nStepNumber)
{
	m_nReservePauseCycleNumber = nCycleNumber;
	m_nReservePauseStepNumber = nStepNumber;
}

//���� ����
//��Ȱ��ȭ : nCycleNumber(0), nStepNumber(0)
void	CChannel::SetReserveComplete(int nCycleNumber, int nStepNumber)
{
	m_nReserveCompleteCycleNumber = nCycleNumber;
	m_nReserveCompleteStepNumer = nStepNumber;
}

//Standard Specification Applicable Channel Set
//CMgrChannel::CMgrChannel()
void	CChannel::SetChannelNumber(int nChannelNumber)
{
	m_nChannelNumber = nChannelNumber;

	_workLogger.Initialize(nChannelNumber);

	m_specificationPath = _T("");
	m_specificationPath.Format(_T("%s\\Config\\%s"), GetExePath(), "Specification");

	CString temp;
	temp.Format(_T("%s\\CH%02d.ini"), m_specificationPath, m_nChannelNumber + 1);
	m_specificationPath = temp;
}

//Schedule Mode Setting
//CCommEthernet::ParseBoardDataFrame(const TEthernet* pComm)
void	CChannel::SetScheduleMode(int nScheduleMode)
{
	int nCommand = 0;

	switch (nScheduleMode)
	{
	case eIFBoardCommandRest: nCommand = eIFBoardStepTypeRest;		break;
	case eIFBoardCommandCharge: nCommand = eIFBoardStepTypeCharge;	break;
	case eIFBoardCommandDischarge: nCommand = eIFBoardStepTypeDischarge;	break;
	default: nCommand = -1; break;
	}

	m_nScheduleMode = nCommand;
}
//Multi Schedule
//CMgrChannel::ScheduleAppend(SStartSchedule& kParser) 
void	CChannel::AddMultiSchedule(SMultiSchedule* lpMultiSchedule)
{
	lpMultiSchedule->nScheduleSerial = (int)m_vecMultiSchedule.size();

	m_vecMultiSchedule.push_back(lpMultiSchedule);

	sort(m_vecMultiSchedule.begin(), m_vecMultiSchedule.end(), SMultiSchedule::SCompareGreater());
}

CMgrSchedule*	CChannel::GetMgr()
{
	return m_pMgr;
}

//Get schedule information by selecting index among multi - schedules(index)
SMultiSchedule*	CChannel::GetMultiScheduleInfo(int nIndex)
{
	if (nIndex >= 0 && nIndex < (int)m_vecMultiSchedule.size())
	{
		return m_vecMultiSchedule[nIndex];
	}

	return nullptr;
}

//currently disabled
void	CChannel::InitStepStart(int length)
{
	CList< CCycle*, CCycle* >& listCycle = m_pMgr->GetCycleList();

	CCycle* pCycle = listCycle.GetHead();

	if (pCycle)
	{
		CList< CStep*, CStep* >& listStep = pCycle->GetStepList();

		CStep* pStep = listStep.GetHead();

		if (pStep)
		{
			m_nCycleNumber = pStep->GetCycleNumber();
			m_nStepNumber = pStep->GetStepNumber();

			int nCycleIndex = pStep->GetCycleIndex();

			nCycleIndex++;
			pStep->SetCycleIndex(nCycleIndex);

			CMgrChamber* pMgr = CMgrChamber::GetMgr();

			if (pMgr)
			{
				SChamber* lpDeviceChamber = pMgr->GetDeivceCyclerChannel(GetChannelNumber());

				if (lpDeviceChamber)
					lpDeviceChamber->fSettingValue = pStep->GetChamberSetTemp();
			}

			m_nScheduleMode = pStep->GetStepType();
		}
	}

	SetChannelStatus(eCyclerChannelStateStart);

	//������ ��ŸƮ�� S��ȣ �ް� True�� �����ϵ��� ���� YJK
	//SetScheduleStart(TRUE);
	//SetChannelStart(TRUE);
	//SetChannelEnd(FALSE);

	CMgrCommData* pMgrComm = CMgrCommData::GetMgr();

	CCommData* pComm = pMgrComm->GetAt(m_nChannelNumber);

	if (NULL == pComm)
		return;

	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		m_bScheduleStart = true;

		SendStart(length);
	}
}

//220125 YGYUN Current application schedule start function
void CChannel::InitStepStart(int nStepIndex, int nStepTime, int nCycleTime, int nLoopCount, float fStepAh, float fStepWh, float fCycleAh, float fCycleWh)
{
	CList< CCycle*, CCycle* >& listCycle = m_pMgr->GetCycleList();

	CCycle* pCycle = listCycle.GetHead();

	if (pCycle)
	{
		CList< CStep*, CStep* >& listStep = pCycle->GetStepList();

		CStep* pStep = listStep.GetHead();

		if (pStep)
		{
			m_nCycleNumber = pStep->GetCycleNumber();
			m_nStepNumber = pStep->GetStepNumber();

			int nCycleIndex = pStep->GetCycleIndex();

			nCycleIndex++;
			pStep->SetCycleIndex(nCycleIndex);

			CMgrChamber* pMgr = CMgrChamber::GetMgr();

			if (pMgr)
			{
				SChamber* lpDeviceChamber = pMgr->GetDeivceCyclerChannel(GetChannelNumber());

				if (lpDeviceChamber)
					lpDeviceChamber->fSettingValue = pStep->GetChamberSetTemp();
			}

			m_nScheduleMode = pStep->GetStepType();
		}
	}

	CMgrCommData* pMgrComm = CMgrCommData::GetMgr();

	CCommData* pComm = pMgrComm->GetAt(m_nChannelNumber);

	if (NULL == pComm)
		return;

	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		//��Ŷ���� ���н� �����Ҷ����� ��ŸƮ ��ȣ ������, ScheduleStart ��ȣ ����������
		while (true)
		{
			if (SendStart(nStepIndex, nStepTime, nCycleTime, nLoopCount, fStepAh, fStepWh, fCycleAh, fCycleWh) == true && m_bScheduleStart == true)
			{
				//������ ��ŸƮ�� S��ȣ �ް� True�� �����ϵ��� ���� YJK
				SetChannelStatus(eCyclerChannelStateStart);
				//SetScheduleStart(TRUE);
				SetChannelStart(TRUE);
				SetChannelEnd(FALSE);
				Sleep(100);
				break;
			}
			delayMs(1000);
		}
	}
}

void CChannel::InitStepStart_UnCheck(int nStepIndex, int nStepTime, int nCycleTime, int nLoopCount, float fStepAh, float fStepWh, float fCycleAh, float fCycleWh)
{
	CList< CCycle*, CCycle* >& listCycle = m_pMgr->GetCycleList();

	CCycle* pCycle = listCycle.GetHead();

	if (pCycle)
	{
		CList< CStep*, CStep* >& listStep = pCycle->GetStepList();

		CStep* pStep = listStep.GetHead();

		if (pStep)
		{
			m_nCycleNumber = pStep->GetCycleNumber();
			m_nStepNumber = pStep->GetStepNumber();

			int nCycleIndex = pStep->GetCycleIndex();

			nCycleIndex++;
			pStep->SetCycleIndex(nCycleIndex);

			CMgrChamber* pMgr = CMgrChamber::GetMgr();

			if (pMgr)
			{
				SChamber* lpDeviceChamber = pMgr->GetDeivceCyclerChannel(GetChannelNumber());

				if (lpDeviceChamber)
					lpDeviceChamber->fSettingValue = pStep->GetChamberSetTemp();
			}

			m_nScheduleMode = pStep->GetStepType();
		}
	}

	CMgrCommData* pMgrComm = CMgrCommData::GetMgr();

	CCommData* pComm = pMgrComm->GetAt(m_nChannelNumber);

	if (NULL == pComm)
		return;

	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		//��Ŷ���� ���н� �����Ҷ����� ��ŸƮ ��ȣ ������
		while (true)
		{
			if (SendStart(nStepIndex, nStepTime, nCycleTime, nLoopCount, fStepAh, fStepWh, fCycleAh, fCycleWh) == true)
			{
				break;
			}
			delayMs(30);
		}
	}
}

void CChannel::InitStepStart_Check(int nStepIndex, int nStepTime, int nCycleTime, int nLoopCount, float fStepAh, float fStepWh, float fCycleAh, float fCycleWh)
{

	CMgrCommData* pMgrComm = CMgrCommData::GetMgr();

	CCommData* pComm = pMgrComm->GetAt(m_nChannelNumber);

	if (NULL == pComm)
		return;

	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		//��Ŷ���� ���н� �����Ҷ����� ��ŸƮ ��ȣ ������, ScheduleStart ��ȣ ����������
		while (true)
		{
			if (m_bScheduleStart == true)
			{
				//������ ��ŸƮ�� S��ȣ �ް� True�� �����ϵ��� ���� YJK
				SetChannelStatus(eCyclerChannelStateStart);
				//SetScheduleStart(TRUE);
				SetChannelStart(TRUE);
				SetChannelEnd(FALSE);
				break;
			}
			else
			{
				SendStart(nStepIndex, nStepTime, nCycleTime, nLoopCount, fStepAh, fStepWh, fCycleAh, fCycleWh);
			}
			Sleep(500);
		}
	}
}

//PnxFormationDlg 
//Schedule ���� ���� ���� �ʱ�ȭ
//CMgrChannel::StartSchedule(SStartSchedule& kParser)
//CMgrChannel::StartErrorChannelSchedule(int chNum, int cycle, int step, int loop)
//CPnxCyclerDlg::OnReceiveMultiScheduleStart(WPARAM wParam, LPARAM lParam)
//CPnxFormationDlg::OnReceiveMultiScheduleStart(WPARAM wParam, LPARAM lParam)
//220125 YGYUN ���� ���� ����� ������ ��ŸƮ �ȵ�
void	CChannel::InitStepStart()
{
	CList< CCycle*, CCycle* >& listCycle = m_pMgr->GetCycleList();

	CCycle* pCycle = listCycle.GetHead();

	if (pCycle)
	{
		CList< CStep*, CStep* >& listStep = pCycle->GetStepList();

		CStep* pStep = listStep.GetHead();

		if (pStep)
		{
			m_nCycleNumber = pStep->GetCycleNumber();
			m_nStepNumber = pStep->GetStepNumber();

			int nCycleIndex = pStep->GetCycleIndex();

			nCycleIndex++;
			pStep->SetCycleIndex(nCycleIndex);

			//Chamber Data Loading
			CMgrChamber* pMgr = CMgrChamber::GetMgr();

			if (pMgr)
			{
				SChamber* lpDeviceChamber = pMgr->GetDeivceCyclerChannel(GetChannelNumber());

				//Applied to the current step's setting chamber temperature chamber(SerialCom.cpp - RunChamber())
				if (lpDeviceChamber)
					lpDeviceChamber->fSettingValue = pStep->GetChamberSetTemp();
			}

			//Schedule Mode Check
			m_nScheduleMode = pStep->GetStepType();
		}
	}

	CMgrCommData* pMgrComm = CMgrCommData::GetMgr();

	CCommData* pComm = pMgrComm->GetAt(m_nChannelNumber);

	if (NULL == pComm)
		return;

	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		//��Ŷ���� ���н� �����Ҷ����� ��ŸƮ ��ȣ ������, ScheduleStart ��ȣ ����������
		while (true)
		{
			if (SendStart() == true && m_bScheduleStart == true)
			{
				//������ ��ŸƮ�� S��ȣ �ް� True�� �����ϵ��� ���� YJK
				SetChannelStatus(eCyclerChannelStateStart);
				//SetScheduleStart(TRUE);
				SetChannelStart(TRUE);
				SetChannelEnd(FALSE);
				Sleep(100);
				break;
			}
			delayMs(100);
		}
	}
}

//������ �Ϸ� 
//CCommEthernet::ParseBoardDataFrame(const TEthernet* pComm)
void	CChannel::ScheduleFinish(bool bIsScheduleComplete, int nEndReason)
{
	if (true == bIsScheduleComplete && nEndReason > 0)
	{
		// By JSK - ���� �������� ���⼭ ��������??

		m_bScheduleStart = false;

		SetOperationMode(eIFBoardOpCodeUnknown);

		SetScheduleMode(-1);

		SetStepSync(false);
		SetTempSync(false);
		SetNormalSync(false);
		SetSyncChannel1(0);
		SetSyncChannel2(0);
		SetChannelStart(FALSE);
		//SetChannelEnd(TRUE);

		SetChamberElevatedTemperature(false);
		SetChannelEnd(TRUE);
		SetPause(false);

		CMgrStatus* pMgrStatus = CMgrStatus::GetMgr();

		if (pMgrStatus)
		{
			SCylerChannelDump* lpCyclerDump = pMgrStatus->GetCyclerChannelDump(GetChannelNumber());

			if (lpCyclerDump)
				lpCyclerDump->Init();

			pMgrStatus->SaveFile(GetChannelNumber());
		}

		CMgrCommData* pMgrCommData = CMgrCommData::GetMgr();

		if (!pMgrCommData)
			return;

		CCommData* lpCyclerDataInfo = pMgrCommData->GetAt(GetChannelNumber());

		if (!lpCyclerDataInfo)
			return;

		std::vector<SScheduleGrade*>& vecGrade = GetMgr()->GetScheduleGrade();

		auto iter = vecGrade.begin();
		auto iterE = vecGrade.end();

		//ù�����̶� ������ ������ ����������
		while (iter != iterE)
		{
			SScheduleGrade* lpScheduleGrade = (*iter);

			if (lpScheduleGrade)
			{
				float fValue = 0.000f;

				switch (lpScheduleGrade->nType)
				{
				case eScheduleGradeTypeVoltage: fValue = lpCyclerDataInfo->GetVoltage();					break;
				case eScheduleGradeTypeCapacity: fValue = (float)lpCyclerDataInfo->GetStepAmpereHour();	break;			//Capacity 
				case eScheduleGradeTypeWattHour: fValue = (float)lpCyclerDataInfo->GetStepWattHour();		break;
				}

				bool bGradeMin = false;

				switch (lpScheduleGrade->nMinSign)
				{
				case 0: bGradeMin = (lpScheduleGrade->fMinValue < fValue) ? true : false; break;
				case 1: bGradeMin = (lpScheduleGrade->fMinValue >= fValue) ? true : false; break;
				}

				bool bGradeMax = false;

				switch (lpScheduleGrade->nMaxSign)
				{
				case 0: bGradeMax = (fValue < lpScheduleGrade->fMaxValue) ? true : false; break;
				case 1: bGradeMax = (fValue >= lpScheduleGrade->fMaxValue) ? true : false; break;
				}
			}

			++iter;
		}
		//Channel Status Complete
		SetChannelStatus(eCyclerChannelStateComplete);
	}

	//�˶� �����϶�
	if (nEndReason == eIFBoardCyclerCodeAlarm)
	{
		m_bScheduleStart = false;
		//����??
		SetChamberElevatedTemperature(false);

		CMgrStatus* pMgrStatus = CMgrStatus::GetMgr();

		if (pMgrStatus)
		{
			SCylerChannelDump* lpCyclerDump = pMgrStatus->GetCyclerChannelDump(GetChannelNumber());

			if (lpCyclerDump)
				lpCyclerDump->Init();

			pMgrStatus->SaveFile(GetChannelNumber());
		}

		if (this->GetChannelNumber() == 1)
			SetChannelStatus(eCyclerChannelStateError);
	}

	CMgrComm* pMgrComm = CMgrComm::GetMgr();

	if (!pMgrComm)
		return;

	//Chamber ���� Ȯ�� 
	std::vector<SDevice*> vecValue;
	pMgrComm->GetDeivceFromType(eDeviceTypeChamber, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			for (int i = 0; i < lpDevice->arrChannel.GetSize(); ++i)
			{
				int nChannel = atoi(lpDevice->arrChannel.GetAt(i));

				if (nChannel == m_nChannelNumber)
				{
					//�ΰ� ���� ���� �� ���� ���ʴ� ���� �ɶ� ���� ��ٸ���.
					if (GetRunChannel().GetLength() > 2)
					{
						if (!GetSyncChamber())
						{
							if (GetChannelEnd())
							{
								CMgrChamber* pMgr = CMgrChamber::GetMgr();

								if (pMgr)
								{
									SChamber* lpDeviceChamber = pMgr->GetDeivceCyclerChannel(m_nChannelNumber);

									if (lpDeviceChamber)
									{
										lpDeviceChamber->bSendOffCommand = true;
										lpDeviceChamber->bChamberExec = false;
									}
								}

								SetChamberChange(FALSE);
							}
						}
					}
					else
					{
						if (GetChamberChange())
						{
							if (GetChannelEnd())
							{
								CMgrChamber* pMgr = CMgrChamber::GetMgr();

								if (pMgr)
								{
									SChamber* lpDeviceChamber = pMgr->GetDeivceCyclerChannel(m_nChannelNumber);

									if (lpDeviceChamber)
									{
										lpDeviceChamber->bSendOffCommand = true;
										lpDeviceChamber->bChamberExec = false;
									}
								}

								SetChamberChange(FALSE);
							}
						}
					}
				}
			}
		}

		++iter;
	}
}

//Manual Pause
//Current Disable
void	CChannel::ManualPause()
{
	m_bScheduleStart = false; //���ڹް� ó���ؾ� ���� �ʳ� ?

	SetAccurateReason(eIFBoardCyclerCodeComplete);
}

//Get Channel Status String
CString	CChannel::GetChannelStatusStr()
{
	if (m_nChannelStatus >= 0 && m_nChannelStatus < eCyclerChannelStateMaxCount)
	{
		//lyw3_1 : �ý��� ���������� ���� ���
		if (m_nChannelStatus == eCyclerChannelStatePause && m_nSystemSafetyReason != 0)
		{
			return "Pause(Safety Cond)";
		}
		else
		{
			// �߿�� �Ͻ����� ���¸� ���������ʰ� ����ڰ� ������ ��� ���� ��...JK
			if (m_nChannelStatus == 5)
			{
				if (GetTempSync() == true)
					return g_lpszCyclerStateString[7];	//�Ͻ�����(�µ�����ȭ)
				else
					return g_lpszCyclerStateString[8];	//�Ͻ�����(���ܵ���ȭ)
			}

			return g_lpszCyclerStateString[m_nChannelStatus];
		}
	}

	return _T("");
}

//Step End Reason Wirte
//CCommEthernet::ParseBoardDataFrame(const TEthernet* pComm)
void CChannel::SetEndCondition()
{
	CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

	if (pCommData == NULL)
	{
		return;
	}

	if (pCommData->GetEndReason() < 1)
	{
		return;
	}

	CString strEnd;
	strEnd.Empty();
	if (true == GetSafetyPause())
	{
		//Safety �� EndReason �����ͷ� ���
		pCommData->SetEndReason(GetSafetyNumber());
		strEnd.Format(_T("%s"), (LPCSTR)pCommData->GetEndReasonStr());
	}
	else
	{
		strEnd.Format(_T("%s"), (LPCSTR)pCommData->GetEndReasonStr());
	}

	m_strEndCondition.Format(_T("%s"), strEnd.GetString());
}


//��� ���� ���� Ȯ��
// turby ���⼭ ���̾�αװ� �߸� ������ϱ� ���⼭ ã�Ƴ��� �����ϸ� �ȴ�. 
void	CChannel::CheckAllCondition()
{
	if (false == m_bScheduleStart/* || this->m_bPause*/)
	{
		return;
	}

	ENDCOND stEnd;

	// �������� ���� ��ÿϷ� �ؾ� �ϴ� �͵�
	if (stEnd.bEvent == FALSE)
	{
		//CheckSystemSatefyCondition(&stEnd);	//�ý��� �������� Ȯ��

		CheckSystemSatefyCondition_Formation(&stEnd);	//220316 YGYUN �ý��� �������� Ȯ�� �����̼ǿ� �°� ���� (Daq ������) Temp Cut ���Ե� �Լ�
	}

	//220622 YGYUN Formation Contact Registant check Function
	if (stEnd.bEvent == FALSE)
	{
		CheckContactRegistantConditon_Formation(&stEnd);
	}	
	//220704 YGYUN Formation Eq Safety Alarm check Function
	if (stEnd.bEvent == FALSE)
	{
		CheckEQCondition_Formation(&stEnd);
	}
#if TOTAL_SAFETY_ENABLE 
	// Total Safety �߰�. 2021-03-04 cnz
	// 2022-01-14 YGY Total Safety Ȯ�� �Լ� ����
	if (stEnd.bEvent == FALSE)
	{
		CheckStepTotalSafetyCondeition(&stEnd);
	}
#else
	if (stEnd.bEvent == FALSE)
	{
		CheckScheduleSatefyCondition(&stEnd);	//��� ������ �������� Ȯ��(VOLT)
	}

	if (stEnd.bEvent == FALSE)
	{
		CheckCycleSafetyCondition(&stEnd);		//����Ŭ �������� Ȯ��
	}

	if (stEnd.bEvent == FALSE)
	{
		CheckStepSafetyCondition(&stEnd);

		//CheckStepSafetyCondition_Formation(&stEnd);	//220316 YGYUN step �������� Ȯ�� �����̼ǿ� �°� ���� (Daq ������)
	}
#endif // 0

	// ��Ÿ ��� ���� ��ÿϷ�, �Ͻ�����, ���� �ؾ� �ϴ� �͵�
	if (stEnd.bEvent == FALSE)
	{
		CheckStepEndCondition(&stEnd);
		// ���⼭ stEnd.bEvent value �� �ٲ����.
	}

	if (stEnd.bEvent == FALSE)
	{
		CheckUserEvent(&stEnd);
	}

#if 1
	// ACTION
	if (stEnd.bEvent == TRUE)
	{
		CString strLog;
		strLog.Empty();
		strLog.Format(_T("[SW_CHECK_CONDITION] stEnd.nEndReason(%d), stEnd.nEndType(%d), stEnd.nNexCycle(%d), stEnd.nNextStep(%d)"),
			stEnd.nEndReason, stEnd.nEndType, stEnd.nNexCycle, stEnd.nNextStep);
		SYSLOG(Dbg, _T("%s"), strLog.GetString());

		{			
			if (stEnd.nEndType == ENDREASON_EVENT_TYPE_COMPLETE)
			{
				this->SetAccurateReason(stEnd.nEndReason);

				this->SendPause(eIFBoardPauseCommandComplete);
		/*		if (stEnd.nEndReason == eMonitroingCyclerCodeSystemSafetyTemperatureMax)
				{
					CMgrChannel* pMgr = CMgrChannel::GetMgr();
					int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;
					if (!pMgr)
						return;

					pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionCompleteImmediately, m_nChannelNumber / nStageChCount);

				}*/
			}
			if (stEnd.nEndType == ENDREASON_EVENT_TYPE_PAUSE)
			{
				this->SetAccurateReason(stEnd.nEndReason);
				this->SendPause(eIFBoardPauseCommandPause);
			}
			if (stEnd.nEndType == ENDREASON_EVENT_TYPE_JUMP)
			{
				if (true == this->ManualNextStep(stEnd.nNexCycle, stEnd.nNextStep))
				{
					this->SetAccurateReason(stEnd.nEndReason);
				}
			}

			//lyw4_1 : �˶�â ǥ��
			if (stEnd.nEndReason != eIFBoardCyclerCodeTimeCut &&
				stEnd.nEndReason != eMonitroingCyclerCodeCycleChargeCapacity &&
				stEnd.nEndReason != eMonitroingCyclerCodeSoC &&
				stEnd.nEndReason != eMonitroingCyclerCodeSoE &&
				stEnd.nEndReason != eMonitroingCyclerCodeStepPause &&
				stEnd.nEndReason != eMonitroingCyclerCodeCyclePause &&
				stEnd.nEndReason != eMonitroingCyclerCodeStepComplete &&
				stEnd.nEndReason != eMonitroingCyclerCodecycleComplete &&
				stEnd.nEndReason != eMonitroingCyclerCodeDaQVoltage &&
				stEnd.nEndReason != eMonitroingCyclerCodeDaQTemperature
				)
			{

				// �ٸ� �˶��� �߻� �ǿ������� �˶�â ������Ʈ �Ѵ�..
				CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);
				if (pCommData->GetSystemSafetyEndReason() != stEnd.nEndReason)
				{
					//stEnd.nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					pCommData->SetSystemSafetyEndReason(stEnd.nEndReason);
					this->SetSystemSafetyReason(stEnd.nEndReason);

					OutputDebugString(_T("Alarm pop-up display\r\n")); //lyw log

					CDlgViewAlarm* pDlg = CDlgViewAlarm::GetMgr();

					if (NULL == pDlg)
						return;

					pDlg->UpdateStatus(false);
					if (FALSE == pDlg->IsWindowVisible())
					{

						pDlg->ShowWindow(SW_SHOW);
					}
					Sleep(100);
				}
				else
				{
					int a = 1;
				}
			}

			//lyw4_1 --------------------------------
		}
	}
#endif
}

void CChannel::Check_Cycle_TotalSafetyCondition()
{
	if (false == m_bScheduleStart/* || this->m_bPause*/)
	{
		return;
	}

	ENDCOND stEnd;

#if TOTAL_SAFETY_ENABLE // Total Safety �߰�. 2021-03-04 cnz
	if (stEnd.bEvent == FALSE)
	{
		CheckCycleTotalSafetyCondition(&stEnd);
	}
#endif


}

bool	CChannel::SendStep(int nReserveStepIndex)
{
	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		int nBoardNumber = CMgrComm::GetMgr()->GetBoardNumber(m_nChannelNumber);

		if (nBoardNumber >= 0)
		{
			BYTE byChannel = 0;

			if (m_nChannelNumber % 4 == 0)	byChannel = 0x01;
			else if (m_nChannelNumber % 4 == 1) byChannel = 0x02;
			else if (m_nChannelNumber % 4 == 2) byChannel = 0x04;
			else if (m_nChannelNumber % 4 == 3) byChannel = 0x08;

			return CSockProc::GetMgr()->SendBoardTypeD_Formation(nBoardNumber, byChannel, nReserveStepIndex);
		}
	}

	return false;
}

int ChamberStartType[4][2];
int ChannelSyncIndex = 0;
bool SyncTypeInit = false;
void  CChannel::SetChannelSync(int pos, int channel, bool stepsync)
{
	int chamber_index = 0;

	if (!SyncTypeInit)
	{
		SyncTypeInit = TRUE;
		memset(ChamberStartType, 0, sizeof(ChamberStartType));
	}

	if (stepsync)
	{
		if (!pos)		ChamberStartType[chamber_index][0] = channel;
		else			ChamberStartType[chamber_index][1] = channel;
	}
	else
	{
		if (!pos)		ChamberStartType[chamber_index][0] = channel;
		else			ChamberStartType[chamber_index][1] = channel;
	}
}

int   CChannel::SendChannelChoice(int channel)
{
	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		int nBoardNumber = CMgrComm::GetMgr()->GetBoardNumber(m_nChannelNumber);

		int index = m_nChannelNumber % 4;
		switch (nBoardNumber)
		{
		case 0:
#if 1
			if (index == 0) channel = 0;
			else if (index == 1) channel = 1;
			else if (index == 2) channel = 2;
			else if (index == 3) channel = 3;
#else
			if (index == 0) channel += 0x01;
			else if (index == 1) channel += 0x02;
			else if (index == 2) channel += 0x04;
			else if (index == 3) channel += 0x08;
#endif
			break;
		case 1:
#if 1
			if (index == 0) channel = 4;
			else if (index == 1) channel = 5;
			else if (index == 2) channel = 6;
			else if (index == 3) channel = 7;
#else
			if (index == 0) channel += 0x10;
			else if (index == 1) channel += 0x20;
			else if (index == 2) channel += 0x40;
			else if (index == 3) channel += 0x80;
#endif
			break;
		case 2:
#if 1
			if (index == 0) channel = 8;
			else if (index == 1) channel = 9;
			else if (index == 2) channel = 10;
			else if (index == 3) channel = 11;
#else
			if (index == 0) channel += 0x01 << 8;
			else if (index == 1) channel += 0x02 << 8;
			else if (index == 2) channel += 0x04 << 8;
			else if (index == 3) channel += 0x08 << 8;
#endif
			break;
		case 3:
#if 1
			if (index == 0) channel = 12;
			else if (index == 1) channel = 13;
			else if (index == 2) channel = 14;
			else if (index == 3) channel = 15;
			break;
#else
			if (index == 0) channel += 0x10 << 8;
			else if (index == 1) channel += 0x20 << 8;
			else if (index == 2) channel += 0x40 << 8;
			else if (index == 3) channel += 0x80 << 8;
			break;
#endif
		}
	}

	return channel;
}

bool	CChannel::SendStart(int length)
{
	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		int nBoardNumber = CMgrComm::GetMgr()->GetBoardNumber(m_nChannelNumber);

		if (nBoardNumber >= 0)
		{
			BYTE byChannel = 0;

			if (m_nChannelNumber % 4 == 0)	byChannel = 0x01;
			else if (m_nChannelNumber % 4 == 1) byChannel = 0x02;
			else if (m_nChannelNumber % 4 == 2) byChannel = 0x04;
			else if (m_nChannelNumber % 4 == 3) byChannel = 0x08;
			return CSockProc::GetMgr()->SendBoardTypeS(nBoardNumber, byChannel, length);
		}
	}

	return false;
}

bool CChannel::SendStart(int nStepIndex, int nStepTime, int nCycleTime, int nLoopCount, float fStepAh, float fStepWh, float fCycleAh, float fCycleWh)
{
	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		int nBoardNumber = CMgrComm::GetMgr()->GetBoardNumber(m_nChannelNumber);

		if (nBoardNumber >= 0)
		{
			BYTE byChannel = 0;

			if (m_nChannelNumber % 4 == 0)	byChannel = 0x01;
			else if (m_nChannelNumber % 4 == 1) byChannel = 0x02;
			else if (m_nChannelNumber % 4 == 2) byChannel = 0x04;
			else if (m_nChannelNumber % 4 == 3) byChannel = 0x08;
			return CSockProc::GetMgr()->SendBoardTypeS(nBoardNumber, byChannel, nStepIndex, nStepTime, nCycleTime, nLoopCount, fStepAh, fStepWh, fCycleAh, fCycleWh);
		}
	}

	return false;
}

bool	CChannel::SendStart()
{
	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		int nBoardNumber = CMgrComm::GetMgr()->GetBoardNumber(m_nChannelNumber);

		if (nBoardNumber >= 0)
		{
			BYTE byChannel = 0;

			if (m_nChannelNumber % 4 == 0)	byChannel = 0x01;
			else if (m_nChannelNumber % 4 == 1) byChannel = 0x02;
			else if (m_nChannelNumber % 4 == 2) byChannel = 0x04;
			else if (m_nChannelNumber % 4 == 3) byChannel = 0x08;
			return CSockProc::GetMgr()->SendBoardTypeS(nBoardNumber, byChannel);
		}
	}

	return false;
}

bool CChannel::SendTemp()
{
	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		int nBoardNumber = CMgrComm::GetMgr()->GetBoardNumber(m_nChannelNumber);

		if (nBoardNumber >= 0)
		{
			BYTE byChannel = 0;

			if (m_nChannelNumber % 4 == 0)	byChannel = 0x01;
			else if (m_nChannelNumber % 4 == 1) byChannel = 0x02;
			else if (m_nChannelNumber % 4 == 2) byChannel = 0x04;
			else if (m_nChannelNumber % 4 == 3) byChannel = 0x08;

			return CSockProc::GetMgr()->SendBoardTypeT(nBoardNumber, byChannel);
		}
	}

	return false;
}

bool	CChannel::SendPause(int nType)
{
	int nStepIndex = 0;

	CCycle* pCycle = m_pMgr->GetAtCycle(m_nCycleNumber);

	if (NULL != pCycle)
	{
		CStep* pStep = pCycle->GetAtStep(m_nStepNumber);

		if (NULL != pStep)
			nStepIndex = pStep->GetStepIndex();
	}

	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		int nBoardNumber = CMgrComm::GetMgr()->GetBoardNumber(m_nChannelNumber);

		if (nBoardNumber >= 0)
		{
			BYTE byChannel = 0;

			if (m_nChannelNumber % 4 == 0)	byChannel = 0x01;
			else if (m_nChannelNumber % 4 == 1) byChannel = 0x02;
			else if (m_nChannelNumber % 4 == 2) byChannel = 0x04;
			else if (m_nChannelNumber % 4 == 3) byChannel = 0x08;

			bool result = CSockProc::GetMgr()->SendBoardTypeP(nBoardNumber, byChannel, nStepIndex, nType);
			//if (result == true && nType == eIFBoardPauseCommandComplete)
			//	ChamberStop(m_nChannelNumber);

			return result;
		}
	}

	return false;
}

bool	CChannel::SendPause(int nStep, int nType)
{

	TRACE("%s Send Step(%d)\n", __FUNCTION__, nStep);

	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		int nBoardNumber = CMgrComm::GetMgr()->GetBoardNumber(m_nChannelNumber);

		if (nBoardNumber >= 0)
		{
			BYTE byChannel = 0;

			if (m_nChannelNumber % 4 == 0)	byChannel = 0x01;
			else if (m_nChannelNumber % 4 == 1) byChannel = 0x02;
			else if (m_nChannelNumber % 4 == 2) byChannel = 0x04;
			else if (m_nChannelNumber % 4 == 3) byChannel = 0x08;

			return CSockProc::GetMgr()->SendBoardTypeP(nBoardNumber, byChannel, nStep, nType);
		}
	}

	return false;
}

bool	CChannel::SendJump(int nStepIndex/*, int nCloneEndReason*/)
{
	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		int nBoardNumber = CMgrComm::GetMgr()->GetBoardNumber(m_nChannelNumber);

		if (nBoardNumber >= 0)
		{
			BYTE byChannel = 0;

			if (m_nChannelNumber % 4 == 0)	byChannel = 0x01;
			else if (m_nChannelNumber % 4 == 1) byChannel = 0x02;
			else if (m_nChannelNumber % 4 == 2) byChannel = 0x04;
			else if (m_nChannelNumber % 4 == 3) byChannel = 0x08;

			{
				BOOL bSendCheck = FALSE;

				if (false == this->IsSendCommandJump())
				{
					bSendCheck = TRUE;
				}

				if (bSendCheck)
				{
					if (CSockProc::GetMgr()->SendBoardTypeJ(nBoardNumber, byChannel, nStepIndex) == TRUE)
					{
						//LOG(Dbg, _T("CH(%d) Jump command to Step(%d)"), m_nChannelNumber + 1, nStepIndex);

						SetFlagSendCommandJump(true);

						return true;
					}
					else
					{
						SYSLOG(Dbg, _T("CH(%d) Step(%d) fail! no send jump command"), m_nChannelNumber + 1, nStepIndex);

						return false;
					}
				}
				else
				{
					SYSLOG(Dbg, _T("CH(%d) Step(%d) Waiting Jump.."), m_nChannelNumber + 1, nStepIndex);

					return false;
				}
			}
		}
	}

	return false;
}

bool	CChannel::SendCommand(int nCommand)
{
	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		int nBoardNumber = CMgrComm::GetMgr()->GetBoardNumber(m_nChannelNumber);

		if (nBoardNumber >= 0)
		{
			BYTE byChannel = 0;

			if (m_nChannelNumber % 4 == 0)	byChannel = 0x01;
			else if (m_nChannelNumber % 4 == 1) byChannel = 0x02;
			else if (m_nChannelNumber % 4 == 2) byChannel = 0x04;
			else if (m_nChannelNumber % 4 == 3) byChannel = 0x08;

			return CSockProc::GetMgr()->SendBoardTypeC(nBoardNumber, byChannel, (BYTE)nCommand);
		}
	}

	return false;
}

CString	CChannel::Int2Time(int nTime)
{
	if (nTime == 0)
	{
		CString strTime;

		strTime.Format(_T("D'%d %02d:%02d:%02d"), 0, 0, 0, 0);

		return strTime;
	}

	int nDay = nTime / 10 / 60 / 60 / 24;
	int nHour = (nTime / 10 / 60 / 60) % 24;
	int nMinute = (nTime / 10 / 60) % 60;
	int nSecond = (nTime / 10) % 60;

	CString strTime;

	strTime.Format(_T("D'%d %02d:%02d:%02d"), nDay, nHour, nMinute, nSecond);

	return strTime;
}

void	CChannel::ManualNextStep()
{
	ENDCOND stEnd;
	stEnd.bEvent = TRUE;
	stEnd.nEndReason = eIFBoardCyclerCodeJump;

	this->CheckNextStep(0, NextStep, &stEnd);	//���� ���� ���� ��ȣ�� ������

	{
		// ACTION
		if (stEnd.bEvent == TRUE)
		{
			if (stEnd.nEndType == ENDREASON_EVENT_TYPE_COMPLETE)
			{
				this->SetAccurateReason(stEnd.nEndReason);

				this->SendPause(eIFBoardPauseCommandComplete);
			}
			if (stEnd.nEndType == ENDREASON_EVENT_TYPE_PAUSE)
			{
				this->SetAccurateReason(stEnd.nEndReason);

				this->SendPause(eIFBoardPauseCommandPause);
			}
			if (stEnd.nEndType == ENDREASON_EVENT_TYPE_JUMP)
			{
				if (true == this->ManualNextStep(stEnd.nNexCycle, stEnd.nNextStep))
				{
					this->SetAccurateReason(stEnd.nEndReason);
				}
			}
		}
	}
}

bool	CChannel::ManualNextStep(int nCycleNumber, int nStepNumber)
{
	CCycle* pCycle = m_pMgr->GetAtCycle(nCycleNumber);
	if (NULL == pCycle)
	{
		SYSLOG(Dbg, _T("%s fail..cycle is null"), __FUNCTION__);
		return false;
	}

	CStep* pStep = pCycle->GetAtStep(nStepNumber);
	if (NULL == pStep)
	{
		SYSLOG(Dbg, _T("%s fail..step is null"), __FUNCTION__);
		return false;
	}

	if (this->GetCycleNumber() == nCycleNumber &&
		this->GetStepNumber() == nStepNumber)
	{
		SYSLOG(Dbg, _T("%s fail..it;s the current step cycle(%d) step(%d)"), __FUNCTION__, nCycleNumber, nStepNumber);
		return false;
	}

	if (eCommTypeEthernet == CMgrComm::GetMgr()->GetCommType(eDeviceTypeIFBoard))
	{
		SendJump(pStep->GetStepIndex());
	}

	return true;
}

void   CChannel::StartTempSync(int nEndReason, int channel, int nStepIndex)
{
	// �µ������� üũ �ȵ������� ����
	if (GetTempSync() != true)	return;

	// �µ� ���� ��� ���� ���� ���          /** GM������ ��� ����. **/
	//if (GetCancelTempSync() == true)
	//{
	//	SendPause(nStepIndex, eIFBoardPauseCommandRestart);	 // ���� Step ���� ��.
	//	return;
	//}

	CStep* pStep = m_pMgr->GetAtStep(nStepIndex);
	float fNowSettingTemp = 0.0;

	if (pStep != NULL)
	{
		fNowSettingTemp = pStep->GetChamberSetTemp(); // Temperature set in the current step
	}

	pStep = m_pMgr->GetAtStep(nStepIndex - 1);
	float fBeforSettingTemp = 0.0;
	if (pStep != NULL)
	{
		fBeforSettingTemp = pStep->GetChamberSetTemp(); // The temperature set in the previous step
	}

	if (fBeforSettingTemp == fNowSettingTemp) // The temperature of the current step is the same as the temperature of the previous step
	{
		SendPause(nStepIndex, eIFBoardPauseCommandRestart);	 // Proceed to the next step.
		return;
	}
	else
	{
		//Check the channel in which the step is in progress
		BOOL bIsFinishStepInChannel = CheckStepSync(nEndReason, channel, nStepIndex);

		if (bIsFinishStepInChannel == TRUE) // When all steps are completed, temperature is controlled.
		{
			SendChamberControl(nStepIndex, eChamberSet); // Chamber ���� ��.

			//�¿� ��� üũ �� 
			if (CheckTempSync(GetChannelNumber(), nStepIndex, nEndReason) == false)
			{
				// �µ� Setting �� ������ ���.	
				//SetEndCondition("Chamber TempSync"); 
				return;
			}
			else
			{
				SendPause(nStepIndex, eIFBoardPauseCommandRestart);	 // ���� Step ���� ��.
				return;
			}
		}
		else
		{
			return; //��� Step�� ���������� ���.
		}
	}

	return;
}

BOOL	CChannel::CheckStepSync(int nEndReason, int channel, int nStepIndex)
{
	//���ܸ�� üũ �ȵ������� ����
	//if (GetStepSync() != true)	return TRUE;

	//���� ü���̶� ���� ������ ������ ü�� No Ȯ�� �� Ȯ��
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();
	CChannel* pStandardChannel = CMgrChannel::GetMgr()->GetChannel(channel);

	int nChannelCount = listChannel.GetCount();
	CStringArray arrChannelNo;
	int nCount = 0;

	while (Pos)
	{
		CChannel* pCompareChannel = listChannel.GetNext(Pos);

		if (NULL == pCompareChannel)
			continue;

		//�񱳴�� ü���� Schedule ������ �ƴϸ� ����.
		if (pCompareChannel->GetScheduleStart() == false)
		{
			continue;
		}

		//���� ������� ���� ü���̶� ���� ���� �ִ� �ٸ� ü���� �� �� �ִ� ����� Log ��� �ۿ� ����.
		if (pStandardChannel->GetLogFolderName() == pCompareChannel->GetLogFolderName())
		{
			if (pStandardChannel->GetCycleNumber() == pCompareChannel->GetCycleNumber() &&
				pStandardChannel->GetStepNumber() == pCompareChannel->GetStepNumber())
			{
				arrChannelNo[nCount] = "SAME_STEP"; // ���� Step
				nCount++;
			}
			else
			{
				arrChannelNo[nCount] = "ANOTHER_STEP"; // �ٸ� Step
				nCount++;
			}
		}
	}

	for (int i = 0; i < nCount; i++)
	{
		if (arrChannelNo[i] == "ANOTHER_STEP")
		{
			return FALSE; // �Ѱ��� ü���̶� �ٸ� Step�̸� FALSE Return
		}
	}

	return TRUE;
}

void	CChannel::StartStepSync(int nEndReason, int channel, int nStepIndex)
{
	//���ܸ�� üũ �ȵ������� ����
	if (GetStepSync() != true)	return;

	//���� ü���̶� ���� ������ ������ ü�� No Ȯ�� �� Ȯ��
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();
	CChannel* pStandardChannel = CMgrChannel::GetMgr()->GetChannel(channel);

	int nChannelCount = listChannel.GetCount();
	CStringArray arrChannelNo;

	int nCount = 0;

	while (Pos)
	{
		CChannel* pCompareChannel = listChannel.GetNext(Pos);

		if (NULL == pCompareChannel)
			continue;

		//�񱳴�� ü���� Schedule ������ �ƴϸ� ����.
		if (pCompareChannel->GetScheduleStart() == false)
		{
			continue;
		}

		//���� ������� ���� ü���̶� ���� ���� �ִ� �ٸ� ü���� �� �� �ִ� ����� Log ��� �ۿ� ����.
		if (pStandardChannel->GetLogFolderName() == pCompareChannel->GetLogFolderName())
		{
			if (pStandardChannel->GetCycleNumber() == pCompareChannel->GetCycleNumber() &&
				pStandardChannel->GetStepNumber() == pCompareChannel->GetStepNumber())
			{
				arrChannelNo[nCount] = "SAME_STEP"; // ���� Step
				nCount++;
			}
			else
			{
				arrChannelNo[nCount] = "ANOTHER_STEP"; // �ٸ� Step
				nCount++;
			}
		}
	}

	for (int i = 0; i < nCount; i++)
	{
		if (arrChannelNo[i] == "SAME_STEP")
		{
			SetEndCondition("Chamber StepSync");
			return;
		}
	}

	SendChamberControl(nStepIndex, eChamberSet); // Chamber ���� ��.

	//�¿� Check
	if (CheckTempSync(GetChannelNumber(), nStepIndex, nEndReason) == false)
	{
		// �µ� Setting �� ������ ���.	
		//SetEndCondition("Chamber TempSync"); 
		return;
	}

	SendPause(nStepIndex, eIFBoardPauseCommandRestart);	 // ���� Step ���� ��.

	return;
}

bool	CChannel::setPatternTemp(int channel, int nStepIndex)
{
	CStep* pStep = m_pMgr->GetAtStep(nStepIndex);
	if (pStep == NULL)	return false;
	CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();
	if (pMgrChamber == NULL)	return false;
	SChamber* lpDeviceChamber = pMgrChamber->GetDeivceCyclerChannel(channel);
	if (lpDeviceChamber == NULL)	return false;

	float fChamberTemp = pStep->GetChamberSetTemp();

	if (fChamberTemp == 0)	return false;

	if (nStepIndex == 1)
	{
		// ù step�� �µ� ���� skip
		m_fLastTempSetValue[GetChamberIndex(channel)] = fChamberTemp;
		return true;
	}

	// false:Enable true:Disable

	//if (m_fLastTempSetValue[GetChamberIndex(channel)] != fChamberTemp)
	{
		ChamberTempSet(channel, fChamberTemp);

		m_fLastTempSetValue[GetChamberIndex(channel)] = fChamberTemp;
	}
	return true;
}

void CChannel::Set_CurrentCalibrationData(float fTargetValue, float fMeasureValue)
{
	auto it = m_mapCurrentCalibrationData.find(fTargetValue);

	if (it == m_mapCurrentCalibrationData.end())
	{
		m_mapCurrentCalibrationData.insert(std::pair<float, float>(fTargetValue, fMeasureValue));
	}	
}

float CChannel::Get_CurrentCalibrationData(float fTargetValue)
{
	auto it = m_mapCurrentCalibrationData.find(fTargetValue);

	if (it == m_mapCurrentCalibrationData.end())
		return NULL;

	return it->second;
}

void CChannel::Set_VoltCalibrationData(float fTargetValue, float fMeasureValue)
{
	auto it = m_mapVoltCalibrationData.find(fTargetValue);

	if (it == m_mapVoltCalibrationData.end())
	{
		m_mapVoltCalibrationData.insert(std::pair<float, float>(fTargetValue, fMeasureValue));
	}
}

float CChannel::Get_VoltCalibrationData(float fTargetValue)
{
	auto it = m_mapVoltCalibrationData.find(fTargetValue);

	if (it == m_mapVoltCalibrationData.end())
		return NULL;

	return it->second;
}

void CChannel::Remove_MapCurrentCalData()
{	
	/*for (auto it = m_mapCurrentCalibrationData.begin(); it != m_mapCurrentCalibrationData.end(); ++it)
	{
		delete it->second;
	}*/
	m_mapCurrentCalibrationData.clear();
}
void CChannel::Remove_MapVoltCalData()
{
	m_mapVoltCalibrationData.clear();
}
bool	CChannel::CheckTempSync(int channel, int nStepIndex, int nEndReason)
{
	CStep* pStep = m_pMgr->GetAtStep(nStepIndex);
	if (pStep == NULL)	return false;
	CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();
	if (pMgrChamber == NULL)	return false;
	SChamber* lpDeviceChamber = pMgrChamber->GetDeivceCyclerChannel(channel);
	if (lpDeviceChamber == NULL)	return false;

	float fChamberTemp = pStep->GetChamberSetTemp();

	if (pStep->GetChamberElevatedTemperature() == false)
		return true;
	if (nStepIndex == 1)
	{
		m_fLastTempSetValue[GetChamberIndex(channel)] = fChamberTemp;
	}
	// Pattern�� '�¿�/�ð���' üũ �������� �µ� ������ ���� step ����


	float fTempGap;
	fTempGap = lpDeviceChamber->fCurrentValue - fChamberTemp;
	// +- 0.3***���� ���.
	if ((fTempGap <= 0.39) && (fTempGap >= -0.39))
	{
		return true;
	}

	return false;
}

bool CChannel::ChamberTempSet(int channel, float temp)
{
	int chamberNum = GetChamberIndex(channel);
	CMgrChamber* pMgr = CMgrChamber::GetMgr();
	SChamber* lpDeviceChamber = pMgr->GetDevice(chamberNum);

	lpDeviceChamber->fSettingValue = temp;
	lpDeviceChamber->bSendChangeValue = true;

	return true;
}

bool CChannel::ChamberRun(int channel)
{
	int chamberNum = GetChamberIndex(channel);
	CMgrChamber* pMgr = CMgrChamber::GetMgr();
	SChamber* lpDeviceChamber = pMgr->GetDevice(chamberNum);

	if (lpDeviceChamber == NULL)
		return true;

	// �����׽�Ʈ�� ���� �ӽ� �ּ� YJK ���忡���� Ǯ�����
	lpDeviceChamber->bSendChangeValue = true;
	lpDeviceChamber->bSendOnCommand = true;

	//lpDeviceChamber->bChamberExec = true;

	return true;
}

bool CChannel::ChamberStop(int channel)
{
	int chamberNum = GetChamberIndex(channel);
	CMgrChamber* pMgr = CMgrChamber::GetMgr();
	if (pMgr == NULL) return false;
	SChamber* lpDeviceChamber = pMgr->GetDevice(chamberNum);
	if (lpDeviceChamber == NULL) return false;

	lpDeviceChamber->bSendOffCommand = true;
	//lpDeviceChamber->bChamberExec = false;
	return true;
}

void	CChannel::SendChamberControl(int nStepIndex, int chamberindex)
{
	CStep* pStep = m_pMgr->GetAtStep(nStepIndex);

	if (NULL == pStep || GetChamberElevatedTemperature() == false)
		return;

	float fChamberTemp = pStep->GetChamberSetTemp();

	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return;

	std::vector<SDevice*> vecValue;
	pMgr->GetDeivceFromType(eDeviceTypeChamber, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();

	if (pMgrChamber)
	{
		SChamber* lpDeviceChamber = pMgrChamber->GetDeivceCyclerChannel(chamberindex);

		if (lpDeviceChamber)
		{
			lpDeviceChamber->fSettingValue = fChamberTemp;

			lpDeviceChamber->bChamberExec = TRUE;
			lpDeviceChamber->bSendChangeValue = TRUE;
			lpDeviceChamber->bSendOnCommand = TRUE;
		}
	}
}

void	CChannel::SendChamberControl(int nStepIndex, eChamberOperationType eChamberOperationType)
{
	CStep* pStep = m_pMgr->GetAtStep(nStepIndex);

	if (NULL == pStep)
		return;

	float fChamberTemp = pStep->GetChamberSetTemp();

	if (fChamberTemp == ChamberDefaultSetting)
		return;

	std::vector<SDevice*> vecValue;
	CMgrComm::GetMgr()->GetDeivceFromType(eDeviceTypeChamber, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* pComm = (*iter);
		iter++;
		if (NULL == pComm)
			continue;

		for (int i = 0; i < pComm->arrChannel.GetSize(); ++i)
		{
			int nChannel = atoi(pComm->arrChannel.GetAt(i));

			if (nChannel == m_nChannelNumber)
			{
				CSerialCom*	pSerial = CComProc::GetMgr()->GetAtComm(pComm->nPrimaryKey);

				if (pSerial)
				{
					if (eChamberOperationType == eChamberOn)
					{
						pSerial->SetChamberEvent(EVENT_CHAMBER_ON, EVENT_CHAMBER_ON);
					}
					else if (eChamberOperationType == eChamberOff)
					{
						pSerial->SetChamberEvent(EVENT_CHAMBER_OFF, EVENT_CHAMBER_OFF);
					}
					else if (eChamberOperationType == eChamberSet)
					{
						pSerial->m_fnSetSV(fChamberTemp);
						pSerial->SetChamberEvent(EVENT_CHAMBER_SET_TEMP, EVENT_CHAMBER_SET_TEMP);
					}
				}

				break;
			}
		}

	}
}

bool	CChannel::IsSocTarget()
{
	CCycle* pCycle = this->GetMgr()->GetAtCycle(this->GetCycleNumber());

	CStep* pStep = NULL;

	if (pCycle != NULL)
	{
		pStep = pCycle->GetAtStep(this->GetStepNumber());
	}

	if (pStep != NULL)
	{
		if (pStep->GetSocTarget() == TRUE)
			return true;
	}

	return false;
}

bool	CChannel::IsSoHTarget()
{
	CCycle* pCycle = this->GetMgr()->GetAtCycle(this->GetCycleNumber());

	CStep* pStep = NULL;

	if (pCycle != NULL)
	{
		pStep = pCycle->GetAtStep(this->GetStepNumber());
	}

	if (pStep != NULL)
	{
		if (pStep->GetSoHTarget() == TRUE)
			return true;
	}

	return false;
}

bool	CChannel::IsSoHTarget2()
{
	CCycle* pCycle = this->GetMgr()->GetAtCycle(this->GetCycleNumber());

	CStep* pStep = NULL;

	if (pCycle != NULL)
	{
		pStep = pCycle->GetAtStep(this->GetStepNumber());
	}

	if (pStep != NULL)
	{
		if (pStep->GetSoHTarget2() == TRUE)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CChannel::SetSoc()
{
	if (true == this->IsSocTarget())
	{
		CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

		if (pCommData != NULL)
		{
			int nStepIndex = pCommData->GetStepIndex();

			if (nStepIndex > 0)
			{
				this->SetSocTargetValue((float)pCommData->GetStepAmpereHour(), nStepIndex);
				this->SetSocWattTargetValue((float)pCommData->GetStepWattHour(), nStepIndex);
			}
		}
	}
}

void CChannel::SetSoH()
{
	if (true == this->IsSoHTarget())
	{
		CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

		if (pCommData != NULL)
		{
			int nStepIndex = pCommData->GetStepIndex();

			if (nStepIndex > 0)
			{
				this->SetSoHTargetValue((float)pCommData->GetStepAmpereHour(), nStepIndex);
			}
		}
	}
}

void CChannel::SetSoH2()
{
	if (true == this->IsSoHTarget2())
	{
		CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

		if (pCommData != NULL)
		{
			int nStepIndex = pCommData->GetStepIndex();

			if (nStepIndex > 0)
			{
				this->SetSoHTargetValue2((float)pCommData->GetStepAmpereHour(), nStepIndex);
			}
		}
	}
}

void	CChannel::SetSoHTargetValue(float fValue, int nStepIndex)
{
	CCycle* pCycle = this->GetMgr()->GetAtCycle(this->GetCycleNumber());
	CStep* pStep = NULL;

	if (pCycle != NULL)
	{
		pStep = pCycle->GetAtStep(this->GetStepNumber());
	}

	if (pStep != NULL)
	{
		if (nStepIndex == pStep->GetStepIndex())
		{
			pStep->SetSoHTargetValue(fValue);
			//TRACE(_T("SetSocTargetValue : Channel(%d) : %.3f\n"), GetChannelNumber(),fValue);
		}
	}
}

void	CChannel::SetSoHTargetValue2(float fValue, int nStepIndex)
{
	CCycle* pCycle = this->GetMgr()->GetAtCycle(this->GetCycleNumber());
	CStep* pStep = NULL;

	if (pCycle != NULL)
	{
		pStep = pCycle->GetAtStep(this->GetStepNumber());
	}

	if (pStep != NULL)
	{
		if (nStepIndex == pStep->GetStepIndex())
		{
			pStep->SetSoHTargetValue2(fValue);
			//TRACE(_T("SetSocTargetValue : Channel(%d) : %.3f\n"), GetChannelNumber(),fValue);
		}
	}
}

void	CChannel::SetSocTargetValue(float fValue, int nStepIndex)
{
	CCycle* pCycle = this->GetMgr()->GetAtCycle(this->GetCycleNumber());
	CStep* pStep = NULL;

	if (pCycle != NULL)
	{
		pStep = pCycle->GetAtStep(this->GetStepNumber());
	}

	if (pStep != NULL)
	{
		if (nStepIndex == pStep->GetStepIndex())
		{
			pStep->SetSocTargetValue(fValue);
			//TRACE(_T("step(%d)  SetSocTargetValue : Channel(%d) : %.6f\n"),GetStepNumber(), GetChannelNumber(),fValue);
		}
	}
}

void	CChannel::SetSocWattTargetValue(float fValue, int nStepIndex)
{
	CCycle* pCycle = this->GetMgr()->GetAtCycle(this->GetCycleNumber());
	CStep* pStep = NULL;

	if (pCycle != NULL)
	{
		pStep = pCycle->GetAtStep(this->GetStepNumber());
	}

	if (pStep != NULL)
	{
		if (nStepIndex == pStep->GetStepIndex())
		{
			pStep->SetSocWattTargetValue(fValue);
			//TRACE(_T("step(%d)  SetSocTargetValue : Channel(%d) : %.6f\n"),GetStepNumber(), GetChannelNumber(),fValue);
		}
	}
}

bool	CChannel::IsSoHUse()
{
	CCycle* pCycle = this->GetMgr()->GetAtCycle(this->GetCycleNumber());
	CStep* pStep = NULL;

	if (pCycle != NULL)
	{
		pStep = pCycle->GetAtStep(this->GetStepNumber());
	}

	if (pStep != NULL)
	{
		if (pStep->GetRatioSoH() <= 0)
			return false;
		else if (pStep->GetRatioSoH() > 0) //taek target ���� �ִٴ� ���� �߰�
			return true;
	}

	return false;
}

bool	CChannel::IsSocUse()
{
	CCycle* pCycle = this->GetMgr()->GetAtCycle(this->GetCycleNumber());
	CStep* pStep = NULL;

	if (pCycle != NULL)
	{
		pStep = pCycle->GetAtStep(this->GetStepNumber());
	}

	if (pStep != NULL)
	{
		if (pStep->GetRatioSoc() <= 0)
			return false;
		else if (pStep->GetRatioSoc() > 0)
			return true;
	}

	return false;
}

void	CChannel::SetFlagSendCommandJump(bool bIsStepChange, bool bFlag)
{
	//Ÿ�̹� ������.. ��� ������ FW���� ó�� �ð��� �ִ�..�� ���̿� ���� �����°� ���ƾ��t..
	//���� ������� ����� �غ��� ��¶�� ���� ���� ���;� �ٽ� ���� �������ְ� ���ٰ�
	//������, �����̺� �����°� �������� ������ ���Ҷ� ����� �ϴ°����� �ٲ㺽 ���ٰ�
	//������ ���Ҷ�..������..............

	if (this->GetStepNumber() == 0)
	{
		return;
	}

	if (true == bIsStepChange)
	{
		this->SetFlagSendCommandJump(bFlag);
	}
}

void	CChannel::SetSyncMode(int nSyncMode)
{
	if (eScheduleSyncModeNotUse == nSyncMode)
	{
		SetStepSync(false);
		SetTempSync(false);
		SetNormalSync(true);
	}
	else if (eScheduleSyncModeTemperature == nSyncMode)
	{
		SetStepSync(false);
		SetTempSync(true);
		SetNormalSync(false);
	}
	else if (eScheduleSyncModeStep == nSyncMode)
	{
		SetStepSync(true);
		SetTempSync(false);
		SetNormalSync(false);
	}

	m_nSyncType = nSyncMode;
	INIWriteInt("Specification", "WorkName", m_nSyncType, m_specificationPath);
}

void CChannel::CheckNextStep(int nNextCycle, int nNextStep, ENDCOND* pCond)
{
	//���� ������ JUMP, PAUSE, COMPLETE ���� ���
	//JUMP �϶� �̵��� ���� ���
	//void	CChannel::ManualNextStep() ���� ��� 						--> ������ �޴���� ����� ��
	//void	CChannel::CheckStepEndCondition(ENDCOND* pCond) ���� ���	--> ���������� ��� �ʿ��� ��

	CStep* pStep = NULL;

	CCommData* pCommdata = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

	if (pCommdata == NULL)
	{
		SYSLOG(Dbg, _T("[FATAR ERROR] %s.. pCommdata is null"), __FUNCTION__);

		return;
	}

	//�����ٿ����� ���׷� ���� ���ǰ� ������ ���������� ���� �� �ִ� ���� ��Ű��
	if (pCommdata->GetStepNumber() == nNextStep && pCommdata->GetCycleNumber() == nNextCycle)
	{
		pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
		SYSLOG(Dbg, _T("[FATAR ERROR] %s.. can;t not jump..check schedule xml file -> next cycle, next step"), __FUNCTION__);
		return;
	}

	pCommdata->RefreshMsgCount();

	if (nNextStep == NextStep)
	{//case1. ������ ���� �����̸� ���� ���� ������ ������ ��¡

		//���� ������ ����Ŭ�� ���� ������ �����̰� ������ �ִ��� ���� Ȯ�� �ؾ� �Ѵ�

		pStep = m_pMgr->GetAtStep(pCommdata->GetStepIndex());

		if (pStep != NULL)
		{
			int nSetLoop = pStep->GetCycleLoopCount/*this value 1base*/();

			if (nSetLoop > 1 &&
				nSetLoop > pCommdata->GetCycleIndex() + 1)
			{//���� �Ҹ����� �ش� ����Ŭ�� ù���� ���� ���� �������� ��

				CCycle* pCycle = m_pMgr->GetAtCycle(pStep->GetCycleNumber());

				pStep = NULL;

				if (pCycle != NULL)
				{
					pStep = pCycle->GetHeadData();
				}
			}
			else
			{//���� ���ų� ���� �Ҹ����̸� �� ���� ����(����Ŭ) ���� �������� ��

				int nNextIndex = -1;

				//������ ������ ��쵵 �ִ�...���� ������ ���� ������ ����Ŭ ����Ʈ�� ����...�Ѥ�

				CCycle* pCycle = m_pMgr->GetAtCycle(pCommdata->GetCycleNumber()/*xml base*/);

				pStep = pCycle->GetAtStep(pCommdata->GetStepNumber()/*xml base*/);

				if (pStep->GetSize() > 0)
				{//pattern step �ϰ��..
					int nNextCycleNumber = -1;

					CStep* pTempStep = pCycle->GetTailData();

					if (pTempStep->GetStepIndex() == pStep->GetStepIndex())
					{//����Ŭ�� ���� ������ �������� Ȯ�� �Ͽ� �������̸� ���� ����Ŭ ��ȣ�� ���� ù���� ������ ��ȣ�� �����´�(xml base��..)

						nNextCycleNumber = pTempStep->GetCycleNumber() + 1;

						pStep = m_pMgr->GetAtStep(nNextCycleNumber, 1);
					}
					else
					{//����Ŭ�� ���� ������ ������ �ƴϸ� �״��� ������ ��ȣ�� �����´�(xml base��..)
						nNextCycleNumber = pCommdata->GetCycleNumber();

						pStep = m_pMgr->GetAtStep(nNextCycleNumber, pCommdata->GetStepNumber() + 1);
					}

					if (pStep == NULL)
					{
						nNextIndex = -1;
					}
					else
					{
						nNextIndex = pStep->GetStepIndex();
					}
					//������ ��� ���� ���� ���뵵 �������� ó���ϱ� ������(0base)
					//xml base�� ��ȣ�� ������ ���������� �����;� �ϴµ�.. �� ���� ����� ������ ���� ���ּ��� ���� �Ӹ��� �ȵ��ư�
				}
				else
				{//normal step �� ���� �����ִ� �ٷ� ���� ���� ���� �ε��� ������
					nNextIndex = pCommdata->GetStepIndex() + 1/* +next*/;
				}

				pStep = NULL;
				pStep = m_pMgr->GetAtStep(nNextIndex);

				if (pStep == NULL)
				{
					pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
					return;
				}
			}

			pCond->nEndType = ENDREASON_EVENT_TYPE_JUMP;
		}
		//no return;
	}

	else if (nNextStep == NextPause)
	{//case2. ���� ���� ���� ���� �� �Ͻ� ������
		pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
		return;
	}

	else if (nNextStep == NextComplete)
	{//case3. ���� ���� ���� ���� ��� �Ϸ� ���� ������
		pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
		return;
	}

	else if (nNextCycle > 0 && nNextStep > 0)
	{//case4. ����Ŭ, ������ ���������� �� ��ȣ�� �ִ�
		pStep = m_pMgr->GetAtStep(nNextCycle, nNextStep);
		if (pStep != NULL)
		{
			pCond->nEndType = ENDREASON_EVENT_TYPE_JUMP;
		}
		//no return;
	}

	if (pStep == NULL)
	{//case1,4�� �ش� --> ������ ���������� ������� ��ÿϷ�� ó��
		//pCond->nEndReason = ??
		//pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
		SYSLOG(Dbg, _T("[FATAR ERROR] %s.. pStep is null"), __FUNCTION__);
		return;
	}
	else
	{//���� ���� �̵��� ������ ����
		pCond->nNexCycle = pStep->GetCycleNumber();
		pCond->nNextStep = pStep->GetStepNumber();
		return;
	}

}

void CChannel::CheckCycleTotalSafetyCondition(ENDCOND * pCond)
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();
	CCommData* pComm = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

	//yuri 20201206
	static char count_AmpereHour_error_check = 0,
		count_voltageMAX_error_check = 0,
		count_voltageMIN_error_check = 0,
		count_curruntMAX_error_check = 0,
		count_curruntMIN_error_check = 0;

	static int totalTime[32] = { 0, };

	char countMax = 0;


	if (NULL == pComm || NULL == pMgr) {
		SYSLOG(Dbg, _T("[FATAR ERROR] void CChannel::CheckSystemSatefyCondition(ENDCOND* pCond).. pComm, pMgr is null"));
		return;
	}

	TOTAL_SAFETY totalSafety = pMgr->GetTotalSafety(m_nChannelNumber);

	int nSrcValue = 0;
	int nDstValue = 0;
	int nSetValue = 0;

	//yuri ��å�Ӵ� ����ó�� ���� 
	//if (GetPause() == TRUE || GetScheduleStart() == FALSE || GetChannelStatus() != "START")
	//	return;
	if (/*GetPause() == TRUE || */GetScheduleStart() == FALSE || GetChannelStatus() != eCyclerChannelStateStart)
		return;

	// �뷮���� üũ
	if (pComm->GetOPCode() == OpCodeChargeCC || pComm->GetOPCode() == OpCodeChargeCP || pComm->GetOPCode() == OpCodeChargeCV ||
		pComm->GetOPCode() == OpCodeDischargeCC || pComm->GetOPCode() == OpCodeDischargeCP || pComm->GetOPCode() == OpCodeDischargeCV)
	{
		nSrcValue = static_cast<int>(abs(pComm->GetStepAmpereHour()) * 1000 * 1000);
		nDstValue = static_cast<int>(totalSafety.fCapacity * 1000);

		//TRACE("nSrcValue = %d, nDstValue = %d \n", nSrcValue, nDstValue);

		//yuri 20201206
		if (nSrcValue >= nDstValue) {
			if (count_AmpereHour_error_check >= countMax)
			{
				pCond->bEvent = TRUE;
				pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_CAPACITY_MAX;
				pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;							//eIFBoardPauseCommandComplete
				//				lpCyclerChannel->SetPauseCycle(TRUE)

				//����
				pCond->bEvent = TRUE;
				pCond->nEndReason = eMonitroingCyclerCodeCyclePause;
				pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				return;
			}
			else
			{
				count_AmpereHour_error_check++;
			}
		}
		else
		{
			count_AmpereHour_error_check = 0;
		}
	}

	// ���� ������
	if (totalSafety.fVoltage >= 0) {
		if (pComm->GetOPCode() == OpCodeChargeCV ||
			pComm->GetOPCode() == OpCodeDischargeCV)
		{
			CStep* step = GetMgr()->GetAtStep(pComm->GetCycleNumber(), pComm->GetStepNumber());

			nSrcValue = static_cast<int>(pComm->GetVoltage() * 1000 * 10);
			nDstValue = static_cast<int>(totalSafety.fVoltage * 1000 * 10);

			if (pComm->GetOPCode() == OpCodeChargeCV)
			{
				nSetValue = static_cast<int>(step->GetChargeVoltage() * 1000 * 10);

				//yuri 20201206
				if (nSrcValue > (nSetValue + nDstValue)) {
					if ((pComm->GetStepTime()) > 30) {
						//yuri 20201206
						if (count_voltageMAX_error_check >= countMax)
						{
							pCond->bEvent = TRUE;
							pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_VOLTAGE_MAX;
							pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
							return;
						}
						else
						{
							count_voltageMAX_error_check++;
						}
					}
					else
					{
						count_voltageMAX_error_check = 0;
					}
				}
			}
			else
			{
				nSetValue = static_cast<int>(step->GetDischargeVoltage() * 1000 * 10);

				if (nSrcValue < (nSetValue - nDstValue)) {
					if ((pComm->GetStepTime()) > 30) {
						//yuri 20201206
						if (count_voltageMIN_error_check >= countMax)
						{
							pCond->bEvent = TRUE;
							pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_VOLTAGE_MIN;
							pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
							return;
						}
						else
						{
							count_voltageMIN_error_check++;
						}
					}
					else
					{
						count_voltageMIN_error_check = 0;
					}
				}
			}
		}
	}

	// ���� ������ 
	if (totalSafety.fCurrent >= 0) {
		if (pComm->GetOPCode() == OpCodeChargeCC ||
			pComm->GetOPCode() == OpCodeDischargeCC) {

			CStep* step = GetMgr()->GetAtStep(pComm->GetCycleNumber(), pComm->GetStepNumber());

			nSrcValue = static_cast<int>(abs(pComm->GetAmpere()) * 1000 * 10);
			nDstValue = static_cast<int>(totalSafety.fCurrent * 10);

			// True �� �ʱ�ȭ�ϴ� �κ� ����
			/*if (m_bRestarting == TRUE)
			{
				if (nSrcValue == 0)
					return;
				else {
					m_bRestarting = FALSE;
				}
			}*/


			if (pComm->GetOPCode() == OpCodeChargeCC) {

				nSetValue = static_cast<int>(step->GetCurrentAmpere() * 1000 * 10);

				// test code

				/*if (nSrcValue > (nSetValue + 100) || nSrcValue < (nSetValue - 100)) {
					TRACE("nSrcValue = %d, nDstValue = %d, nSetValue = %d\n", nSrcValue, nDstValue, nSetValue);
					}*/

				if (nSrcValue > (nSetValue + nDstValue)) {
					if (count_curruntMAX_error_check >= countMax) {
						pCond->bEvent = TRUE;
						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_CURRENT_MAX;
						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
						return;
					}
					else {
						count_curruntMAX_error_check++;
					}
				}
				else {
					count_curruntMAX_error_check = 0;
				}
			}
			else {
				if (nSrcValue < (nSetValue - nDstValue)) {
					if (count_curruntMIN_error_check >= countMax) {
						pCond->bEvent = TRUE;
						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_CURRENT_MAX;
						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
						return;
					}
					else {
						count_curruntMIN_error_check++;
					}
				}
				else {
					count_curruntMIN_error_check = 0;
				}
			}
			//	}
		}
	}

	static int beforeVoltage[32] = { 0, };
	static int noVoltageVarState[32] = { 0, };
	int nDeviation = 0;
	if (totalSafety.nNoVoltageVariation >= 0) {
		if (pComm->GetOPCode() == OpCodeDischargeCC || pComm->GetOPCode() == OpCodeDischargeCP ||
			pComm->GetOPCode() == OpCodeChargeCC || pComm->GetOPCode() == OpCodeChargeCP)
		{
			int nStepTime = pComm->GetStepTime();
			nSrcValue = static_cast<int>(pComm->GetVoltage() * 1000 * 10);

			if (nStepTime == 1) // ���� ó�� 0.1�ʰ� �Ǹ� ����
			{
				noVoltageVarState[m_nChannelNumber] = 0;
				beforeVoltage[m_nChannelNumber] = nSrcValue;
			}
			else
			{
				nDeviation = abs(nSrcValue - beforeVoltage[m_nChannelNumber]);
				if (nDeviation > 10) // ����� ���� ���簪 ��   ������ 0.1mv
				{
					beforeVoltage[m_nChannelNumber] = nSrcValue;
					noVoltageVarState[m_nChannelNumber] = 0;
				}
				else
				{
					noVoltageVarState[m_nChannelNumber] += 100;
				}

				if (noVoltageVarState[m_nChannelNumber] >= totalSafety.nNoVoltageVariation) {
					pCond->bEvent = TRUE;
					pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_NO_VOLTAGE_VARIATION;
					pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
				}
			}
		}
	}

	// Voltage variation (CC ���� ���� ��ȭ ��)
	static int beforeVoltage_var[32] = { 0, };

	if (totalSafety.fVoltageVariation >= 0) {
		int nStepTime_var = pComm->GetStepTime() * 100;

		if (nStepTime_var > 3000) {
			if (pComm->GetOPCode() == OpCodeDischargeCC) {
				nSrcValue = static_cast<int>(pComm->GetVoltage() * 1000 * 10);
				nSrcValue = beforeVoltage_var[m_nChannelNumber] - nSrcValue;
			}

			if (pComm->GetOPCode() == OpCodeChargeCC) {
				nSrcValue = static_cast<int>(pComm->GetVoltage() * 1000 * 10);
				nSrcValue = nSrcValue - beforeVoltage_var[m_nChannelNumber];
			}

			if (pComm->GetOPCode() == OpCodeDischargeCC || pComm->GetOPCode() == OpCodeChargeCC) {
				if (nSrcValue > (totalSafety.fVoltageVariation * 10)) {
					pCond->bEvent = TRUE;
					pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_CC_VOLTAGE_VARIATION;
					pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
				}
			}
		}
		beforeVoltage_var[m_nChannelNumber] = static_cast<int>(pComm->GetVoltage() * 1000 * 10);
	}


	// charge capacity variation (�⺻ false)
	static int ChargeCapacity[32][1024][1024];

	if (totalSafety.fChargeCapacityVariation >= 0) {
		if (pComm->GetOPCode() == OpCodeChargeCC ||
			pComm->GetOPCode() == OpCodeChargeCV ||
			pComm->GetOPCode() == OpCodeChargeCP) {

			int nCh = m_nChannelNumber;
			int nStep = pComm->GetStepNumber();
			int nLoop = pComm->GetcycleIndex();

			if ((pComm->GetEndReason() > 0) && (nLoop > 1)) { //step ����� �� ��° loop �϶�   

				int currentCapacity = ChargeCapacity[nCh][nLoop][nStep]; //���� �뷮
				int beforCapacity = ChargeCapacity[nCh][nLoop - 1][nStep]; //���� ����Ŭ�� �뷮
				int range = currentCapacity * static_cast<int>(totalSafety.fChargeCapacityVariation / 100.f);

				if (currentCapacity > 0) {
					if ((beforCapacity + range) < currentCapacity ||
						(beforCapacity - range) > currentCapacity)
					{
						pCond->bEvent = TRUE;
						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_CHARGE_CAPA_VARIATION;
						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
						return;
					}
				}
			}
			else
			{
				if (pComm->GetEndReason() == 0)
				{
					ChargeCapacity[nCh][nLoop][nStep] = static_cast<int>(abs(pComm->GetChargeAmpereHour()) * 1000 * 1000);
				}
			}
		}
	}

	/*yuri 20201201 ���� end*/

	// discharge capacity variation
	static int DischargeCapacity[32][1024][1024];

	if (totalSafety.fDischargeCapacityVariation >= 0) {
		if (pComm->GetOPCode() == OpCodeDischargeCC ||
			pComm->GetOPCode() == OpCodeDischargeCV ||
			pComm->GetOPCode() == OpCodeDischargeCP) {

			int nCh = m_nChannelNumber;
			int nStep = pComm->GetStepNumber();
			int nLoop = pComm->GetcycleIndex();

			if ((pComm->GetEndReason() > 0) && (nLoop > 1)) {
				int currentCapacity = DischargeCapacity[nCh][nLoop][nStep];
				int   beforCapacity = DischargeCapacity[nCh][nLoop - 1][nStep];
				int range = currentCapacity * static_cast<int>(totalSafety.fDischargeCapacityVariation / 100.f);

				if (currentCapacity > 0) {
					if ((beforCapacity + range) < currentCapacity ||
						(beforCapacity - range) > currentCapacity)
					{
						pCond->bEvent = TRUE;
						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_DCHARGE_CAPA_VARIATION;
						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
					}
				}
			}
			else
			{
				if (pComm->GetEndReason() <= 0)
				{
					DischargeCapacity[nCh][nLoop][nStep] = static_cast<int>(abs(pComm->GetDischargeAmpereHour()) * 1000 * 1000);
				}
			}
		}
	}

	// chamber max temp (�̻��, è�� ���� �Ǿ� ���� ����.)

	// sample max temp
	if (totalSafety.fSampleMaxTemp >= 0) {
		if (pComm->GetTemp() > totalSafety.fSampleMaxTemp) {
			pCond->bEvent = TRUE;
			pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_TEMP_MAX;
			pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
			return;
		}
	}

	// SOH (�⺻ false)
	static int nSOH_SrcValue[32];
	if (totalSafety.fSOH >= 0) {
		if (pComm->GetcycleIndex() > 0) {
			if (pComm->GetOPCode() == OpCodeDischargeCC || pComm->GetOPCode() == OpCodeDischargeCV || pComm->GetOPCode() == OpCodeDischargeCP) {
				if (pComm->GetEndReason() > 0) {
					if (nSOH_SrcValue[m_nChannelNumber] <= (totalSafety.fSOH)) {
						pCond->bEvent = TRUE;
						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_SOH;
						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
						return;
					}
				}
				else if (pComm->GetEndReason() <= 0) {
					//get = 0.000 Ah, totalSafety.fSOH = mAh
					nSOH_SrcValue[m_nChannelNumber] = static_cast<int>(abs(pComm->GetDischargeAmpereHour()) * 1000);

					//TRACE("nSOH_SrcValue= %d, totalSafety.fSOH = %f, GetDischargeAmpereHour =  %f\n", nSOH_SrcValue, totalSafety.fSOH, abs(pComm->GetDischargeAmpereHour()));
				}
			}
		}
	}

	/*
	static int nSOH_SrcValue;
	if (totalSafety.fSOH >= 0){
		if (pComm->GetcycleIndex() > 0){
			if (pComm->GetOPCode() == OpCodeDischargeCC || pComm->GetOPCode() == OpCodeDischargeCV || pComm->GetOPCode() == OpCodeDischargeCP){
				//get = 0.000 Ah, totalSafety.fSOH = mAh
				nSOH_SrcValue = abs(pComm->GetDischargeAmpereHour()) * 1000;

				//TRACE("nSOH_SrcValue= %d, totalSafety.fSOH = %f, GetDischargeAmpereHour =  %f\n", nSOH_SrcValue, totalSafety.fSOH, abs(pComm->GetDischargeAmpereHour()));

				if (nSOH_SrcValue >= (totalSafety.fSOH)){
						pCond->bEvent = TRUE;
						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_SOH;
						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
						return;
				}
			}
		}
	}
	*/
}

void CChannel::CheckStepTotalSafetyCondeition(ENDCOND * pCond)
{
	CMgrConfig* pMgr = CMgrConfig::GetMgr();
	CCommData* pComm = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

	//yuri 20201206
	static char count_AmpereHour_error_check = 0,
		count_voltageMAX_error_check = 0,
		count_voltageMIN_error_check = 0,
		count_curruntMAX_error_check = 0,
		count_curruntMIN_error_check = 0;

	static int totalTime[32] = { 0, };

	char countMax = 0;


	if (NULL == pComm || NULL == pMgr) {
		SYSLOG(Dbg, _T("[FATAR ERROR] void CChannel::CheckSystemSatefyCondition(ENDCOND* pCond).. pComm, pMgr is null"));
		return;
	}

	TOTAL_SAFETY totalSafety = pMgr->GetTotalSafety(m_nChannelNumber);

	int nSrcValue = 0;
	int nDstValue = 0;
	int nSetValue = 0;

	//yuri ��å�Ӵ� ����ó�� ���� 
	//if (GetPause() == TRUE || GetScheduleStart() == FALSE || GetChannelStatus() != "START")
	//	return;
	if (/*GetPause() == TRUE || */GetScheduleStart() == FALSE || GetChannelStatus() != eCyclerChannelStateStart)
		return;

	// ����� �ð� ����
	//	���� : test end time
	//	����
	//	CC����
	//	C - Rate    ���� �ð� ����
	//	1C   �ѽð����� ������
	//	��갪   �뷮 / ��갪
	//	����   C - Rate + ������ �ð�
	//	����(�뷮 : 1000mAh ������ 500mAh) 3�ð� 30�� : �뷮 / ��갪(2�ð�)+ ������ �ð�(������ �ð� : 1�ð� 30��)
	if (totalSafety.nTestEndTime > 0) {
		CStep* step = GetMgr()->GetAtStep(pComm->GetCycleNumber(), pComm->GetStepNumber());

		if (pComm->GetOPCode() == OpCodeDischargeCC ||
			pComm->GetOPCode() == OpCodeChargeCC ||
			pComm->GetOPCode() == OpCodeDischargeCV ||
			pComm->GetOPCode() == OpCodeChargeCV)
		{


			float nSetCurrent = step->GetCurrentAmpere() * 1000; //�������� ����� �Է� ��
			float nInputCurrent = totalSafety.fInputCapacity;
			float nCRate = nInputCurrent / nSetCurrent;

			auto c_rate = static_cast<int>(nCRate * 60 * 60 * 1000);
			auto test_end_time = static_cast<int>(totalSafety.nTestEndTime);
			totalTime[m_nChannelNumber] = c_rate + test_end_time;

			if ((pComm->GetStepTime() * 100) > totalTime[m_nChannelNumber]) {
				pCond->bEvent = TRUE;
				pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_TEST_END_TIME;
				pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
				return;
			}
		}
	}
}

void CChannel::CheckSystemSatefyCondition(ENDCOND* lpCompleteCondition)
{
	CMgrCommData* pMgrComm = CMgrCommData::GetMgr();

	if (!pMgrComm)
		return;

	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	if (!pMgrConfig)
		return;

	CCommData* lpCyclerChannelInfo = pMgrComm->GetAt(m_nChannelNumber);

	if (!lpCyclerChannelInfo)
		return;

	SetSafetyPause(false);

	CStep* pStep = m_pMgr->GetAtStep(m_nCycleNumber, m_nStepNumber);

	{
		// Volage Max
		float fCyclerValue = lpCyclerChannelInfo->GetVoltage();
		float fSafetyValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectVoltage);

		if (fSafetyValue != 0.000f && fSafetyValue < fCyclerValue)
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyVoltageMax;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "System safety conditions Satisfied with the high limit of voltage"); //lyw4_1

			SYSLOG(System, "System safety conditions Satisfied with the high limit of voltage");

			return;
		}
	}

	{
		// Voltage Min
		float fCyclerValue = lpCyclerChannelInfo->GetVoltage();
		float fSafetyValue = pMgrConfig->GetSystemSafetyMin(eSyatemSafetyObjectVoltage);

		if (fSafetyValue != 0.000f && fCyclerValue != 0.000f && fSafetyValue > fCyclerValue)
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyVoltageMin;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "System safety conditions Satisfied with the low limit of voltage"); //lyw4_1

			SYSLOG(System, "System safety conditions Satisfied with the low limit of voltage");

			return;
		}
	}

	{
		// Current Max
		float fCyclerValue = abs(lpCyclerChannelInfo->GetAmpere());
		float fSafetyValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCurrent);

		if (fSafetyValue != 0.000f && fSafetyValue < fCyclerValue)
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCurrentMax;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "System safety conditions Satisfied with the high limit of current"); //lyw4_1

			SYSLOG(System, "System safety conditions Satisfied with the high limit of current");

			return;
		}
	}

	{
		// Current Min

		// By JSK - 2020.06.30 �ƴ�.. ���� �����̸� rest ���� ��/�����϶� �ؾ� �ϴ°� �ƴϳ�??

		float fCyclerValue = abs(lpCyclerChannelInfo->GetAmpere());
		float fSafetyValue = pMgrConfig->GetSystemSafetyMin(eSyatemSafetyObjectCurrent);

		if (fSafetyValue != 0.000f && fSafetyValue > fCyclerValue && true == lpCyclerChannelInfo->IsUsingCyclerCurrent())
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCurrentMin;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "System safety conditions Satisfied with the low limit of current"); //lyw4_1

			SYSLOG(System, "System safety conditions Satisfied with the low limit of current");

			return;
		}
	}

	{
		// Temperature Max

		CMgrDAQ* pMgrDaQ = CMgrDAQ::GetMgr();

		if (pMgrDaQ)
		{
			float fCyclerValue = pMgrDaQ->GetDAQTemperatureMax(GetChannelNumber());
			float fSafetyValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectTemperature);

			if (fSafetyValue != 0.000f && fSafetyValue < fCyclerValue)
			{
				lpCompleteCondition->bEvent = TRUE;
				lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyTemperatureMax;
				lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;

				// �µ� ���� �˶� Ȯ���� ���� Error Value �߰�. 2021-02-09
				CString strmsg;
				strmsg.Format(_T("Satisfaction with the high limit of the system safety Cond Temp DAQ[%.3f] Safety[%.3f]"), fCyclerValue, fSafetyValue);
				CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", strmsg, ALARM_HEAVY); //lyw4_1

				SYSLOG(System, strmsg);

				return;
			}
		}
	}

	{
		// Capacity Max
		float fCyclerValue = lpCyclerChannelInfo->GetCapacity();
		float fSafetyValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCapacity);

		if (fSafetyValue != 0.000f && fSafetyValue < fCyclerValue)
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCapacityMax;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "System safety conditions Satisfied with the high limit of capacity"); //lyw4_1

			SYSLOG(System, "System safety conditions Satisfied with the high limit of capacity");

			return;
		}
	}

	//{
	//	// Capacity Min
	//	float fCyclerValue = lpCyclerChannelInfo->GetCapacity();
	//	float fSafetyValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCapacity);

	//	if (fSafetyValue != 0.000f && fSafetyValue > fCyclerValue)
	//	{
	//		lpCompleteCondition->bEvent		= TRUE;
	//		lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCapacityMax;
	//		lpCompleteCondition->nEndType	= ENDREASON_EVENT_TYPE_PAUSE;

	//		LOG(Normal, "�ý��� ���� ���� �뷮 ���ѿ� ����");

	//		return;
	//	}		
	//}

	if (pMgrConfig->GetIsDeltaIsUse() == TRUE)
	{
		float fDeltaTime = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectDeltaTime);

		if ((lpCyclerChannelInfo->GetStepTime() - lpCyclerChannelInfo->GetBeforStepTimeV()) / 10.f >= fDeltaTime)				// fDeltaTime = 100
		{
			lpCyclerChannelInfo->SetBeforStepTimeV(lpCyclerChannelInfo->GetStepTime());

			float fCyclerDeltaV = lpCyclerChannelInfo->GetDVBefor();

			float fSystemDeltaV = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectDeltaV);

			if (fCyclerDeltaV == 0.f)
			{
				lpCyclerChannelInfo->SetDVBefor(lpCyclerChannelInfo->GetVoltage());

				lpCompleteCondition->bEvent = FALSE;

				return;
			}
			else
			{
				lpCyclerChannelInfo->SetDVAfter(lpCyclerChannelInfo->GetVoltage());
			}

			fCyclerDeltaV = lpCyclerChannelInfo->GetDVAfter();

			float fDeltaV = (float)(abs(fCyclerDeltaV) * (fSystemDeltaV * 0.01));

			if (lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeChargeCC || lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeDischargeCC)
			{
				if ((fCyclerDeltaV + fDeltaV) < fCyclerDeltaV || (fCyclerDeltaV - fDeltaV) > fCyclerDeltaV)
				{
					lpCompleteCondition->bEvent = TRUE;
					lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyDeltaV;
					lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;	// SKI ��û�� ���� Pause �� ����
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "Satisfying System safety Cond DeltaV"); //lyw4_1

					return;
				}

				lpCyclerChannelInfo->SetDVBefor(lpCyclerChannelInfo->GetVoltage());
			}
		}

		if ((lpCyclerChannelInfo->GetStepTime() - lpCyclerChannelInfo->GetBeforStepTimeI()) / 10.f >= fDeltaTime)
		{
			lpCyclerChannelInfo->SetBeforStepTimeI(lpCyclerChannelInfo->GetStepTime());

			float fCyclerDeltaI = lpCyclerChannelInfo->GetDIBefor();

			float fSystemDeltaI = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectDeltaI);

			if (fCyclerDeltaI == 0.f)
			{
				lpCyclerChannelInfo->SetDIBefor(lpCyclerChannelInfo->GetAmpere());

				lpCompleteCondition->bEvent = FALSE;

				return;
			}
			else
			{
				lpCyclerChannelInfo->SetDIAfter(lpCyclerChannelInfo->GetAmpere());
			}

			fCyclerDeltaI = lpCyclerChannelInfo->GetDIAfter();

			float fDeltaI = (float)(abs(fCyclerDeltaI) * (fSystemDeltaI * 0.01));

			if (lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeChargeCV || lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeDischargeCV)
			{
				if ((fCyclerDeltaI + fDeltaI) < fSystemDeltaI || (fCyclerDeltaI - fDeltaI) > fSystemDeltaI)
				{
					lpCompleteCondition->bEvent = TRUE;
					lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyDeltaI;
					lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "Satisfying System safety Cond Delta"); //lyw4_1
					return;
				}

				lpCyclerChannelInfo->SetDIBefor(lpCyclerChannelInfo->GetAmpere());
			}
		}
	}

	if (pMgrConfig->GetIsCvDeltaIsUse() == TRUE)
	{
		if (lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeChargeCV || lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeDischargeCV)
		{
			float fSystemDeltaV = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCvDeltaV);

			float fTemp = abs((lpCyclerChannelInfo->GetVoltage() - lpCyclerChannelInfo->GetPreVoltage()) * 1000);

			if (fTemp > fSystemDeltaV)
			{
				lpCompleteCondition->bEvent = TRUE;
				lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCvDeltaV;
				lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "Satisfying System safety Cond CV DeltaV"); //lyw4_1
				return;
			}

			float fSystemDeltaI = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCvDeltaA);

			fTemp = abs(lpCyclerChannelInfo->GetAmpere() - lpCyclerChannelInfo->GetPreAmpere());

			if (fTemp > fSystemDeltaI)
			{
				lpCompleteCondition->bEvent = TRUE;
				lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCvDeltaI;
				lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "Satisfying System safety Cond CV Delta I"); //lyw4_1
				return;
			}
		}
	}

	//SVDF Interlock �߰�.
	if (GetMgr()->GetSVDF()->bUseSVDF == true && pStep->GetStepType() == eIFBoardStepTypeRest && (lpCyclerChannelInfo->GetStepTime() % 10 == 0))
	{
		if (lpCyclerChannelInfo->GetStepTime() == 10)
			GetMgr()->GetSVDF()->nCurrErrCount = 0;

		GetMgr()->m_queueSvdfCircula.enQueue(lpCyclerChannelInfo->GetVoltage() * 1000);

		if (lpCyclerChannelInfo->GetStepTime() < 160)
			return;

		int nLoopCnt = 0;
		float fDeviationVolt = 0.0;
		float fCurrVolt = 0;
		int nSVDFMode = 0;

		if (lpCyclerChannelInfo->GetStepTime() == 160)
			nLoopCnt = 1;
		else if (lpCyclerChannelInfo->GetStepTime() == 170)
			nLoopCnt = 2;
		else
			nLoopCnt = 3;

		for (int n = 0; n < nLoopCnt; n++)
		{
			fDeviationVolt += (GetMgr()->m_queueSvdfCircula.deQueue(n) - (lpCyclerChannelInfo->GetVoltage() * 1000));
			//	strLog.Format(_T("SVDF DeviationVolt - %.3f, SVDF MaxVolt - %.3f"), fDeviationVolt, GetMgr()->GetSVDF()->fMaxVolt);
			//	LOG(Dbg, strLog);
		}

		if (fDeviationVolt < GetMgr()->GetSVDF()->fMaxVolt)
		{
			GetMgr()->GetSVDF()->AddErrorCount();
			SYSLOG(System, _T("Resting Voltage Rise ErrCnt - %d, SVDF MaxVolt %.3f"), GetMgr()->GetSVDF()->GetCurrErrCount(), GetMgr()->GetSVDF()->fMaxVolt);
			nSVDFMode = 1;
		}
		else if ((fCurrVolt = GetMgr()->m_queueSvdfCircula.deQueue(0)) > GetMgr()->GetSVDF()->fAvgVolt)
		{
			//	strLog.Format(_T("�ϴ����� Drop fCurrVolt - %.3f, SVDF fAvgVolt - %.3f, fDeviationVolt - %.3f, fSOCLowerDrop - %.3f"), fCurrVolt, GetMgr()->GetSVDF()->fAvgVolt, fDeviationVolt, GetMgr()->GetSVDF()->fSOCLowerDrop);
			//	LOG(Dbg, strLog);
			if (fDeviationVolt > GetMgr()->GetSVDF()->fSOCLowerDrop)
			{
				GetMgr()->GetSVDF()->AddErrorCount();
				SYSLOG(System, _T("Bottom Voltage Drop SVDF ErrCnt - %d, SVDF fSOCLowerDrop %.3f"), GetMgr()->GetSVDF()->GetCurrErrCount(), GetMgr()->GetSVDF()->fSOCLowerDrop);
				nSVDFMode = 2;
			}
		}
		else
		{
			//	strLog.Format(_T("��� ���� Drop fDeviationVolt - %.3f, fSOCUpperDrop - %.3f"), fDeviationVolt, GetMgr()->GetSVDF()->fSOCUpperDrop);
			//	LOG(Dbg, strLog);

			if (fDeviationVolt > GetMgr()->GetSVDF()->fSOCUpperDrop)
			{
				GetMgr()->GetSVDF()->AddErrorCount();
				SYSLOG(System, _T("Top Voltage Drop ErrCnt - %d, fDeviationVolt - %.3f, SVDF fSOCUpperDrop %.3f"), GetMgr()->GetSVDF()->GetCurrErrCount(), fDeviationVolt, GetMgr()->GetSVDF()->fSOCUpperDrop);
				nSVDFMode = 3;
			}
		}

		if (GetMgr()->GetSVDF()->GetCurrErrCount() > GetMgr()->GetSVDF()->nErrCount)
		{
			if (nSVDFMode == 1)
			{
				lpCompleteCondition->bEvent = TRUE;
				lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSvdfRestCellDevDetect;
				lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "SVDF resting deviation detection"); //lyw4_1
				return;
			}
			else if (nSVDFMode == 2)
			{
				lpCompleteCondition->bEvent = TRUE;
				lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSvdfUpperCellDevDetect;
				lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "SVDF upper deviation detection"); //lyw4_1
				return;
			}
			else if (nSVDFMode == 3)
			{
				lpCompleteCondition->bEvent = TRUE;
				lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSvdfLowerCellDevDetect;
				lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "SVDF bottom deviation detection"); //lyw4_1
				return;
			}
		}
	}
}

void CChannel::CheckSystemSatefyCondition_Formation(ENDCOND * lpCompleteCondition)
{
	CMgrCommData* pMgrComm = CMgrCommData::GetMgr();

	if (!pMgrComm)
		return;

	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	if (!pMgrConfig)
		return;

	CCommData* lpCyclerChannelInfo = pMgrComm->GetAt(m_nChannelNumber);

	if (!lpCyclerChannelInfo)
		return;

	SetSafetyPause(false);

	CStep* pStep = m_pMgr->GetAtStep(m_nCycleNumber, m_nStepNumber);

	{
		// Volage Max
		float fCyclerValue = lpCyclerChannelInfo->GetVoltage();
		float fSafetyValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectVoltage);

		if (fSafetyValue != 0.000f && fSafetyValue < fCyclerValue)
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyVoltageMax;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "System safety conditions Satisfied with the high limit of voltage"); //lyw4_1

			SYSLOG(System, "System safety conditions Satisfied with the high limit of voltage");

			return;
		}
	}

	{
		// Voltage Min
		float fCyclerValue = lpCyclerChannelInfo->GetVoltage();
		float fSafetyValue = pMgrConfig->GetSystemSafetyMin(eSyatemSafetyObjectVoltage);

		if (fSafetyValue != 0.000f && fCyclerValue != 0.000f && fSafetyValue > fCyclerValue)
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyVoltageMin;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "System safety conditions Satisfied with the low limit of voltage"); //lyw4_1

			SYSLOG(System, "System safety conditions Satisfied with the low limit of voltage");

			return;
		}
	}

	{
		// Current Max
		float fCyclerValue = abs(lpCyclerChannelInfo->GetAmpere());
		float fSafetyValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCurrent);

		if (fSafetyValue != 0.000f && fSafetyValue < fCyclerValue)
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCurrentMax;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "System safety conditions Satisfied with the high limit of current"); //lyw4_1

			SYSLOG(System, "System safety conditions Satisfied with the high limit of current");

			return;
		}
	}

	{
		// Current Min

		// By JSK - 2020.06.30 �ƴ�.. ���� �����̸� rest ���� ��/�����϶� �ؾ� �ϴ°� �ƴϳ�??

		float fCyclerValue = abs(lpCyclerChannelInfo->GetAmpere());
		float fSafetyValue = pMgrConfig->GetSystemSafetyMin(eSyatemSafetyObjectCurrent);

		if (fSafetyValue != 0.000f && fSafetyValue > fCyclerValue && true == lpCyclerChannelInfo->IsUsingCyclerCurrent())
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCurrentMin;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "System safety conditions Satisfied with the low limit of current"); //lyw4_1

			SYSLOG(System, "System safety conditions Satisfied with the low limit of current");

			return;
		}
	}

	{
		// Temperature Max
		float fCyclerValue = abs(lpCyclerChannelInfo->GetTemp());
		float fSafetyValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectTemperature);

		if (fSafetyValue != 0.000f && fSafetyValue < fCyclerValue)
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyTemperatureMax;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;

			// �µ� ���� �˶� Ȯ���� ���� Error Value �߰�. 2021-02-09
			CString strmsg;
			strmsg.Format(_T("Satisfaction with the high limit of the system safety Cond Temp [%.3f] Safety[%.3f]"), fCyclerValue, fSafetyValue);
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", strmsg, ALARM_HEAVY); //lyw4_1

			SYSLOG(System, strmsg);

			return;
		}
	}
	{
		// Temperature Min
		float fCyclerValue = abs(lpCyclerChannelInfo->GetTemp());
		float fSafetyValue = pMgrConfig->GetSystemSafetyMin(eSyatemSafetyObjectTemperature);

		if (fSafetyValue != 0.000f && fCyclerValue != 0.000f && fSafetyValue > fCyclerValue)
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyTemperatureMin;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;

			// �µ� ���� �˶� Ȯ���� ���� Error Value �߰�. 2021-02-09
			CString strmsg;
			strmsg.Format(_T("Satisfaction with the low limit of the system safety Cond Temp [%.3f] Safety[%.3f]"), fCyclerValue, fSafetyValue);
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", strmsg); //lyw4_1

			SYSLOG(System, strmsg);

			return;
		}
	}
	{
		// Capacity Max
		float fCyclerValue = lpCyclerChannelInfo->GetCapacity();
		float fSafetyValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCapacity);

		if (fSafetyValue != 0.000f && fSafetyValue < fCyclerValue)
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCapacityMax;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "System safety conditions Satisfied with the high limit of capacity"); //lyw4_1

			SYSLOG(System, "System safety conditions Satisfied with the high limit of capacity");

			return;
		}
	}

	//{
	//	// Capacity Min
	//	float fCyclerValue = lpCyclerChannelInfo->GetCapacity();
	//	float fSafetyValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCapacity);

	//	if (fSafetyValue != 0.000f && fSafetyValue > fCyclerValue)
	//	{
	//		lpCompleteCondition->bEvent		= TRUE;
	//		lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCapacityMax;
	//		lpCompleteCondition->nEndType	= ENDREASON_EVENT_TYPE_PAUSE;

	//		LOG(Normal, "�ý��� ���� ���� �뷮 ���ѿ� ����");

	//		return;
	//	}		
	//}

	if (pMgrConfig->GetIsDeltaIsUse() == TRUE)
	{
		float fDeltaTime = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectDeltaTime);

		if ((lpCyclerChannelInfo->GetStepTime() - lpCyclerChannelInfo->GetBeforStepTimeV()) / 10.f >= fDeltaTime)	//fDeltaTime 5 (����)
		{
			lpCyclerChannelInfo->SetBeforStepTimeV(lpCyclerChannelInfo->GetStepTime());

			float fCyclerBeforDeltaV = lpCyclerChannelInfo->GetDVBefor();

			float fSystemDeltaV = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectDeltaV);

			if (fCyclerBeforDeltaV == 0.f)
			{
				lpCyclerChannelInfo->SetDVBefor(lpCyclerChannelInfo->GetVoltage());

				lpCompleteCondition->bEvent = FALSE;

				return;
			}
			else
			{
				lpCyclerChannelInfo->SetDVAfter(lpCyclerChannelInfo->GetVoltage());
			}

			float fCyclerAfterDeltaV = lpCyclerChannelInfo->GetDVAfter();

			//220407 YGY ���� �纯ȯ ���� (0.01 ->0.001)
			//���д� CV����ϰ�츸 
			//float fDeltaV = (float)(abs(fCyclerDeltaV) * (fSystemDeltaV * 0.01));//����
	/*		float fDeltaV_H = (float)(abs(fCyclerBeforDeltaV) + (fSystemDeltaV * 0.001));
			float fDeltaV_L = (float)(abs(fCyclerBeforDeltaV) - (fSystemDeltaV * 0.001));*/	//��1
			float fDeltaV = (float)(fSystemDeltaV * 0.001);	//��2

			if (lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeChargeCV || lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeDischargeCV)
			{
				//if ((fCyclerDeltaV + fDeltaV) < fCyclerDeltaV || (fCyclerDeltaV - fDeltaV) > fCyclerDeltaV)	//����

				//if (fDeltaV_H < fCyclerAfterDeltaV || fDeltaV_L > fCyclerAfterDeltaV)	// ��1
				if (fCyclerBeforDeltaV + fDeltaV < fCyclerAfterDeltaV || fCyclerBeforDeltaV - fDeltaV > fCyclerAfterDeltaV) //��2
				{
					lpCompleteCondition->bEvent = TRUE;
					lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyDeltaV;
					lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;	// SKI ��û�� ���� Pause �� ����
					SYSLOG(Dbg, "[DeltaV]  Before : %.3fV, After : %.3fV", fCyclerBeforDeltaV, fCyclerAfterDeltaV);
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "Satisfying System safety Cond DeltaV"); //lyw4_1

					return;
				}

				lpCyclerChannelInfo->SetDVBefor(lpCyclerChannelInfo->GetVoltage());
			}
		}

		if ((lpCyclerChannelInfo->GetStepTime() - lpCyclerChannelInfo->GetBeforStepTimeI()) / 10.f >= fDeltaTime)
		{
			lpCyclerChannelInfo->SetBeforStepTimeI(lpCyclerChannelInfo->GetStepTime());

			float fCyclerBeforeDeltaI = lpCyclerChannelInfo->GetDIBefor();

			float fSystemDeltaI = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectDeltaI);

			if (fCyclerBeforeDeltaI == 0.f)
			{
				lpCyclerChannelInfo->SetDIBefor(lpCyclerChannelInfo->GetAmpere());

				lpCompleteCondition->bEvent = FALSE;

				return;
			}
			else
			{
				lpCyclerChannelInfo->SetDIAfter(lpCyclerChannelInfo->GetAmpere());
			}

			float fCyclerAfterDeltaI = lpCyclerChannelInfo->GetDIAfter();

			
			//220407 YGY ���� �纯ȯ ���� (0.01 -> ����)
			//������ CC����ϰ�츸 
			//float fDeltaI = (float)(abs(fCyclerDeltaI) * (fSystemDeltaI * 0.01));				//����
			//float fDeltaI_H = (float)(abs(fCyclerBeforeDeltaI) + (fSystemDeltaI * 0.001));
			//float fDeltaI_L = (float)(abs(fCyclerBeforeDeltaI) - (fSystemDeltaI * 0.001));//��1
			
			float fDeltaI = (float)((fSystemDeltaI));				//��2


			if (lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeChargeCC || lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeDischargeCC)
			{
				//if ((fCyclerDeltaI + fDeltaI) < fSystemDeltaI || (fCyclerDeltaI - fDeltaI) > fSystemDeltaI)	//����
				//if ((fDeltaI_H) < fCyclerAfterDeltaI || (fDeltaI_L) > fCyclerAfterDeltaI)//��1
				if ((fCyclerBeforeDeltaI + fDeltaI) < fSystemDeltaI || (fCyclerBeforeDeltaI - fDeltaI) > fSystemDeltaI)	//��2

				{
					lpCompleteCondition->bEvent = TRUE;
					lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyDeltaI;
					lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					SYSLOG(Dbg, "[DeltaV]  Before : %.3fI, After : %.3fI", fCyclerBeforeDeltaI, fCyclerAfterDeltaI);
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "Satisfying System safety Cond Delta"); //lyw4_1
					return;
				}

				lpCyclerChannelInfo->SetDIBefor(lpCyclerChannelInfo->GetAmpere());
			}
		}
	}

	if (pMgrConfig->GetIsCvDeltaIsUse() == TRUE)
	{
		if (lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeChargeCV || lpCyclerChannelInfo->GetOPCode() == eIFBoardOpCodeDischargeCV)
		{
			float fSystemDeltaV = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCvDeltaV);

			float fTemp = abs((lpCyclerChannelInfo->GetVoltage() - lpCyclerChannelInfo->GetPreVoltage()) * 1000);	// V -> mV ���� ��ȯ

			if (fTemp > fSystemDeltaV)
			{
				lpCompleteCondition->bEvent = TRUE;
				lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCvDeltaV;
				lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "Satisfying System safety Cond CV DeltaV"); //lyw4_1
				return;
			}

			float fSystemDeltaI = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCvDeltaA);

			fTemp = abs(lpCyclerChannelInfo->GetAmpere() - lpCyclerChannelInfo->GetPreAmpere());

			if (fTemp > fSystemDeltaI)
			{
				lpCompleteCondition->bEvent = TRUE;
				lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyCvDeltaI;
				lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "Satisfying System safety Cond CV Delta I"); //lyw4_1
				return;
			}
		}
	}

	//SVDF Interlock �߰�.
	if (GetMgr()->GetSVDF()->bUseSVDF == true && pStep->GetStepType() == eIFBoardStepTypeRest && (lpCyclerChannelInfo->GetStepTime() % 10 == 0))
	{
		if (lpCyclerChannelInfo->GetStepTime() == 10)
			GetMgr()->GetSVDF()->nCurrErrCount = 0;

		GetMgr()->m_queueSvdfCircula.enQueue(lpCyclerChannelInfo->GetVoltage() * 1000);

		if (lpCyclerChannelInfo->GetStepTime() < 160)
			return;

		int nLoopCnt = 0;
		float fDeviationVolt = 0.0;
		float fCurrVolt = 0;
		int nSVDFMode = 0;

		if (lpCyclerChannelInfo->GetStepTime() == 160)
			nLoopCnt = 1;
		else if (lpCyclerChannelInfo->GetStepTime() == 170)
			nLoopCnt = 2;
		else
			nLoopCnt = 3;

		for (int n = 0; n < nLoopCnt; n++)
		{
			fDeviationVolt += (GetMgr()->m_queueSvdfCircula.deQueue(n) - (lpCyclerChannelInfo->GetVoltage() * 1000));
			//	strLog.Format(_T("SVDF DeviationVolt - %.3f, SVDF MaxVolt - %.3f"), fDeviationVolt, GetMgr()->GetSVDF()->fMaxVolt);
			//	LOG(Dbg, strLog);
		}

		if (fDeviationVolt < GetMgr()->GetSVDF()->fMaxVolt)
		{
			GetMgr()->GetSVDF()->AddErrorCount();
			SYSLOG(System, _T("Resting Voltage Rise ErrCnt - %d, SVDF MaxVolt %.3f"), GetMgr()->GetSVDF()->GetCurrErrCount(), GetMgr()->GetSVDF()->fMaxVolt);
			nSVDFMode = 1;
		}
		else if ((fCurrVolt = GetMgr()->m_queueSvdfCircula.deQueue(0)) > GetMgr()->GetSVDF()->fAvgVolt)
		{
			//	strLog.Format(_T("�ϴ����� Drop fCurrVolt - %.3f, SVDF fAvgVolt - %.3f, fDeviationVolt - %.3f, fSOCLowerDrop - %.3f"), fCurrVolt, GetMgr()->GetSVDF()->fAvgVolt, fDeviationVolt, GetMgr()->GetSVDF()->fSOCLowerDrop);
			//	LOG(Dbg, strLog);
			if (fDeviationVolt > GetMgr()->GetSVDF()->fSOCLowerDrop)
			{
				GetMgr()->GetSVDF()->AddErrorCount();
				SYSLOG(System, _T("Bottom Voltage Drop SVDF ErrCnt - %d, SVDF fSOCLowerDrop %.3f"), GetMgr()->GetSVDF()->GetCurrErrCount(), GetMgr()->GetSVDF()->fSOCLowerDrop);
				nSVDFMode = 2;
			}
		}
		else
		{
			//	strLog.Format(_T("��� ���� Drop fDeviationVolt - %.3f, fSOCUpperDrop - %.3f"), fDeviationVolt, GetMgr()->GetSVDF()->fSOCUpperDrop);
			//	LOG(Dbg, strLog);

			if (fDeviationVolt > GetMgr()->GetSVDF()->fSOCUpperDrop)
			{
				GetMgr()->GetSVDF()->AddErrorCount();
				SYSLOG(System, _T("Top Voltage Drop ErrCnt - %d, fDeviationVolt - %.3f, SVDF fSOCUpperDrop %.3f"), GetMgr()->GetSVDF()->GetCurrErrCount(), fDeviationVolt, GetMgr()->GetSVDF()->fSOCUpperDrop);
				nSVDFMode = 3;
			}
		}

		if (GetMgr()->GetSVDF()->GetCurrErrCount() > GetMgr()->GetSVDF()->nErrCount)
		{
			if (nSVDFMode == 1)
			{
				lpCompleteCondition->bEvent = TRUE;
				lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSvdfRestCellDevDetect;
				lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "SVDF resting deviation detection"); //lyw4_1
				return;
			}
			else if (nSVDFMode == 2)
			{
				lpCompleteCondition->bEvent = TRUE;
				lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSvdfUpperCellDevDetect;
				lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "SVDF upper deviation detection"); //lyw4_1
				return;
			}
			else if (nSVDFMode == 3)
			{
				lpCompleteCondition->bEvent = TRUE;
				lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSvdfLowerCellDevDetect;
				lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "SVDF bottom deviation detection"); //lyw4_1
				return;
			}
		}
	}
}

void CChannel::CheckContactRegistantConditon_Formation(ENDCOND* lpCompleteCondition)
{
	CMgrCommData* pMgrComm = CMgrCommData::GetMgr();
	CCommData* pComm = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

	if (!pMgrComm)
		return;

	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	if (!pMgrConfig)
		return;

	CCommData* lpCyclerChannelInfo = pMgrComm->GetAt(m_nChannelNumber);

	if (!lpCyclerChannelInfo)
		return;

	if (lpCyclerChannelInfo->GetCommCommand() != eIFBoardCommandPogoPinTestMode)
		return;

	SetSafetyPause(false);

	CStep* pStep = m_pMgr->GetAtStep(m_nCycleNumber, m_nStepNumber);
	{
		// Contact Registant Max
		float fPogoOhmMonitoring = lpCyclerChannelInfo->GetPogoOhmMonitoring();
		float fSafetyValue = lpCyclerChannelInfo->GetPogoOhmSetting();

		if (fSafetyValue != 0.000f && fSafetyValue < fPogoOhmMonitoring)
		{
			lpCompleteCondition->bEvent = TRUE;
			lpCompleteCondition->nEndReason = eMonitroingCyclerCodeSystemSafetyContactReg;
			lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
			CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", "System safety conditions Satisfied with the high limit of Contact Registant"); //220622 YGYUN

			return;
		}
	}



}

void CChannel::CheckEQCondition_Formation(ENDCOND * lpCompleteCondition)
{
	CMgrCommData* pMgrComm = CMgrCommData::GetMgr();
	CCommData* pComm = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);
	ALARM_ITEM item;
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	CString strTemp = "";
	CString strComment = "";
	int nStageNo = 0;
	if (!pMgrComm)
		return;

	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	if (!pMgrConfig)
		return;

	CCommData* lpCyclerChannelInfo = pMgrComm->GetAt(m_nChannelNumber);

	if (!lpCyclerChannelInfo)
		return;


	nStageNo = m_nChannelNumber / CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;
	if (pMgrDio->GetFireCheckSensor(nStageNo) == TRUE)
	{
		lpCompleteCondition->bEvent = TRUE;
		lpCompleteCondition->nEndReason = eMonitoringCyclerCodeStage1FireSensor + nStageNo;
		lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
		strTemp.Format(_T("(STAGE %d) Fire Sensor Check Please"), nStageNo);
		strComment.Format(_T("(STAGE %d) Fire Sensor Check Please"), nStageNo);
		CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_EQ_STAGE1_FIRE_SENSOR_ON + nStageNo, strTemp, ALARM_HEAVY, strComment, &item);
		return;
	}

	if (pMgrDio->GetSmokeCheckSensor1(nStageNo) == TRUE)
	{
		lpCompleteCondition->bEvent = TRUE;
		lpCompleteCondition->nEndReason = eMonitoringCyclerCodeStage1SmokeSensor1+ nStageNo;
		lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
		strTemp.Format(_T("(STAGE %d) Smoke Sensor1 Check Please"), nStageNo);
		strComment.Format(_T("(STAGE %d) Smoke Sensor1 Check Please"), nStageNo);
		CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_EQ_STAGE1_SMOKE_SENSOR1_ON + nStageNo, strTemp, ALARM_HEAVY, strComment, &item);

		return;
	}

	if (pMgrDio->GetSmokeCheckSensor2(nStageNo) == TRUE)
	{
		lpCompleteCondition->bEvent = TRUE;
		lpCompleteCondition->nEndReason = eMonitoringCyclerCodeStage1SmokeSensor2 + nStageNo;
		lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
		strTemp.Format(_T("(STAGE %d) Smoke Sensor2 Check Please"), nStageNo);
		strComment.Format(_T("(STAGE %d) Smoke Sensor2 Check Please"), nStageNo);
		CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_EQ_STAGE1_SMOKE_SENSOR2_ON + nStageNo, strTemp, ALARM_HEAVY, strComment, &item);

		return;
	}
	/*if (pMgrDio->Get_Alarm_FanStatus(nStageNo, 3) == true)
	{
		lpCompleteCondition->bEvent = TRUE;
		lpCompleteCondition->nEndReason = eMonitoringCyclerCodeStage1FanAlarm+ nStageNo;
		lpCompleteCondition->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
		strTemp.Format(_T("(STAGE %d) Fan Status Check Please"), nStageNo);
		CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_EQ_STAGE1_FAN_STATUS_ALARM + nStageNo, strTemp, ALARM_HEAVY, &item);

		return;
	}*/

	


}


void CChannel::CheckScheduleSatefyCondition(ENDCOND* pCond)
{
	CCommData* pComm = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

	if (NULL == pComm)
	{
		SYSLOG(Dbg, _T("[FATAR ERROR] void CChannel::CheckScheduleSatefyCondition(ENDCOND* pCond).. pComm is null"));

		return;
	}

	SetSafetyPause(false);

	std::vector<SScheduleSafety*>& vecScheduleSafety = m_pMgr->GetScheduleSafety();

	auto iter = vecScheduleSafety.begin();
	auto iterE = vecScheduleSafety.end();

	while (iter != iterE)
	{
		SScheduleSafety* lpScheduleSafety = (*iter);

		if (lpScheduleSafety)
		{
			//Voltage Max 
			if (strcmp(lpScheduleSafety->strName, "VoMax") == 0)
			{
				//220126 YGYUN �Ǽ��� ���� ó�� ��� ����
				/*
				float fSrcValue = pComm->GetVoltage();
				float fDstValue = lpScheduleSafety->fValue;*/
				float fSrcValue = abs(pComm->GetVoltage());
				float fDstValue = abs(lpScheduleSafety->fValue);

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeSystemSafetyVoltageMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Schedule safety", "Satisfying schedule safety Cond Voltage Max"); //lyw4_1

					return;
				}
			}
			//Voltage Min
			else if (strcmp(lpScheduleSafety->strName, _T("VoMin")) == 0)
			{
				//220126 YGYUN �Ǽ��� ���� ó�� ��� ����
				/*
				float fSrcValue = pComm->GetVoltage();
				float fDstValue = lpScheduleSafety->fValue;*/
				float fSrcValue = abs(pComm->GetVoltage());
				float fDstValue = abs(lpScheduleSafety->fValue);

				if (fSrcValue <= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeSystemSafetyVoltageMin;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Schedule safety", "Satisfying schedule safety Cond Voltage Min"); //lyw4_1

					return;
				}
			}
		}

		++iter;
	}
}


//����Ŭ �������� (ParseBoardDataFrame_Formation - CheckCondtion Function���� ����Ͽ� ����Ŭ ���� ���� Ȯ��
void CChannel::CheckCycleSafetyCondition(ENDCOND* pCond)
{
	CCommData* pComm = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

	CCycle* pCycle = m_pMgr->GetAtCycle(m_nCycleNumber);

	if (NULL == pComm || NULL == pCycle)
	{
		SYSLOG(Dbg, _T("[FATAR ERROR] void CChannel::CheckCycleSafetyCondition(ENDCOND* pCond).. pComm, pCycle is null"));

		return;
	}

	SetSafetyPause(false);

	//�� ����Ŭ �� �������� 
	std::vector<SScheduleSafety*>& vecScheduleSafety = pCycle->GetScheduleSafety();

	auto iter = vecScheduleSafety.begin();
	auto iterE = vecScheduleSafety.end();

	while (iter != iterE)
	{
		SScheduleSafety* lpScheduleSafety = (*iter);

		if (lpScheduleSafety)
		{
			//Voltage Max ����Ŭ ���� ����
			if (strcmp(lpScheduleSafety->strName, _T("VoMax")) == 0)
			{
				//220126 YGYUN �Ǽ��� ���� ó�� ��� ����
/*				float fSrcValue = pComm->GetVoltage();
				float fDstValue = lpScheduleSafety->fValue;		*/
				float fSrcValue = abs(pComm->GetVoltage());
				float fDstValue = abs(lpScheduleSafety->fValue);

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeCycleSafetyVoltageMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Cycle Safety", "Satisfying cycle safety Cond Voltage Max"); //lyw4_1

					return;
				}
			}
			//Voltage Min ����Ŭ ���� ����
			else if (strcmp(lpScheduleSafety->strName, _T("VoMin")) == 0)
			{
				//220126 YGYUN �Ǽ��� ���� ó�� ��� ����
/*				float fSrcValue = pComm->GetVoltage();
				float fDstValue = lpScheduleSafety->fValue;		*/
				float fSrcValue = abs(pComm->GetVoltage());
				float fDstValue = abs(lpScheduleSafety->fValue);

				if (fSrcValue <= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeCycleSafetyVoltageMin;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Cycle Safety", "Satisfying cycle safety Cond Voltage Min"); //lyw4_1

					return;
				}
			}
			//Current Max ����Ŭ ���� ����
			else if (strcmp(lpScheduleSafety->strName, _T("CuMax")) == 0)
			{
				float fSrcValue = abs(pComm->GetAmpere());
				float fDstValue = abs(lpScheduleSafety->fValue);

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeCycleSafetyCurrentMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Cycle Safety", "Satisfying cycle safety Cond Current Max"); //lyw4_1

					return;
				}
			}
			//Capacity Max ����Ŭ ���� ����
			else if (strcmp(lpScheduleSafety->strName, _T("CaMax")) == 0)
			{
				float fSrcValue = (float)abs(pComm->GetStepAmpereHour());
				float fDstValue = (float)abs(lpScheduleSafety->fValue);

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeCycleSafetyCapacityMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Cycle Safety", "Satisfying cycle safety Cond Capacity Max"); //lyw4_1

					return;
				}
			}
			//Watt Max ����Ŭ ���� ����
			else if (strcmp(lpScheduleSafety->strName, _T("PoMax")) == 0&& strcmp(pComm->GetStepTypeStr(), _T("POGO PIN TEST")) != 0)
			{
				float fSrcValue = (float)abs(pComm->GetPower());
				float fDstValue = (float)abs(lpScheduleSafety->fValue);

				if (m_nChannelNumber == 4)
				{
					int a = 0;
				}

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeCycleSafetyWattMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Cycle Safety", "Satisfying cycle safety Cond Watt Max"); //lyw4_1

					return;
				}
			}
			//WattHour Max ����Ŭ ���� ����
			else if (strcmp(lpScheduleSafety->strName, _T("WHMax")) == 0)
			{
				float fSrcValue = (float)abs(pComm->GetStepWattHour());
				float fDstValue = (float)abs(lpScheduleSafety->fValue);

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeCycleSafetyWatthourMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Cycle Safety", "Satisfying cycle safety Cond WattHour Max"); //lyw4_1

					return;
				}
			}
		}
		++iter;
	}
}


//���� ��������
void CChannel::CheckStepSafetyCondition(ENDCOND* pCond)
{
	CCommData* pComm = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

	CStep* pStep = m_pMgr->GetAtStep(m_nCycleNumber, m_nStepNumber);

	if (NULL == pComm || NULL == pStep)
	{
		SYSLOG(Dbg, _T("[FATAR ERROR] void CChannel::CheckStepSafetyCondition(ENDCOND* pCond).. pComm, pStep is null"));

		return;
	}

	SetSafetyPause(false);

	std::vector<SScheduleSafety*>& vecScheduleSafety = pStep->GetScheduleSafety();

	auto iter = vecScheduleSafety.begin();
	auto iterE = vecScheduleSafety.end();

	while (iter != iterE)
	{
		SScheduleSafety* lpScheduleSafety = (*iter);

		if (lpScheduleSafety)
		{
			//Voltage Max
			if (strcmp(lpScheduleSafety->strName, _T("VoMax")) == 0)
			{
				float fSrcValue = pComm->GetVoltage();
				float fDstValue = lpScheduleSafety->fValue;

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeStepSafetyVoltageMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Step Safety", "Satisfying step safety Cond Voltage Max"); //lyw4_1

					return;
				}
			}
			//Voltage Min
			if (strcmp(lpScheduleSafety->strName, _T("VoMin")) == 0)
			{
				float fSrcValue = pComm->GetVoltage();
				float fDstValue = lpScheduleSafety->fValue;

				if (fSrcValue <= fDstValue && fDstValue != 0.000 && fSrcValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeStepSafetyVoltageMin;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Step Safety", "Satisfying step safety Cond Voltage Min"); //lyw4_1

					return;
				}
			}

			//Current Max
			if (strcmp(lpScheduleSafety->strName, _T("CuMax")) == 0)
			{
				float fSrcValue = abs(pComm->GetAmpere());
				float fDstValue = abs(lpScheduleSafety->fValue);

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeStepSafetyCurrentMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Step Safety", "Satisfying step safety Cond Current Max"); //lyw4_1

					return;
				}
			}

			//Current Min
			if (strcmp(lpScheduleSafety->strName, _T("CuMin")) == 0)
			{
				float fSrcValue = abs(pComm->GetAmpere());
				float fDstValue = abs(lpScheduleSafety->fValue);

				if (fSrcValue <= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeStepSafetyCurrentMin;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Step Safety", "Satisfying step safety Cond Current Min"); //lyw4_1

					return;
				}
			}

			//Capacity Max
			if (strcmp(lpScheduleSafety->strName, _T("CaMax")) == 0)
			{
				float fSrcValue = (float)abs(pComm->GetStepAmpereHour());
				float fDstValue = (float)abs(lpScheduleSafety->fValue);

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeStepSafetyCapacityMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Step Safety", "Satisfying step safety Cond Capacity Max"); //lyw4_1

					return;
				}
			}

			//Temperature Max
			if (strcmp(lpScheduleSafety->strName, _T("CeTeMaxH")) == 0)
			{
				CMgrDAQ* pMgrDaQ = CMgrDAQ::GetMgr();

				if (pMgrDaQ)
				{
					float fSrcValue = abs(pMgrDaQ->GetDAQTemperatureMax(GetChannelNumber()));

					float fDstValue = abs(lpScheduleSafety->fValue);

					if (fSrcValue >= fDstValue && fDstValue != 0.000)
					{
						pCond->bEvent = TRUE;
						pCond->nEndReason = eMonitroingCyclerCodeStepSafetyTemperatureMax;
						pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
						CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Step Safety", "Satisfying step safety Cond Temp Max"); //lyw4_1
						return;
					}
				}


			}
		}

		++iter;
	}
}

void CChannel::CheckStepSafetyCondition_Formation(ENDCOND * pCond)
{
	CCommData* pComm = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

	CStep* pStep = m_pMgr->GetAtStep(m_nCycleNumber, m_nStepNumber);

	if (NULL == pComm || NULL == pStep)
	{
		SYSLOG(Dbg, _T("[FATAR ERROR] void CChannel::CheckStepSafetyCondition(ENDCOND* pCond).. pComm, pStep is null"));

		return;
	}

	SetSafetyPause(false);

	std::vector<SScheduleSafety*>& vecScheduleSafety = pStep->GetScheduleSafety();

	auto iter = vecScheduleSafety.begin();
	auto iterE = vecScheduleSafety.end();

	while (iter != iterE)
	{
		SScheduleSafety* lpScheduleSafety = (*iter);

		if (lpScheduleSafety)
		{
			//Voltage Max
			if (strcmp(lpScheduleSafety->strName, _T("VoMax")) == 0)
			{
				float fSrcValue = pComm->GetVoltage();
				float fDstValue = lpScheduleSafety->fValue;

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeStepSafetyVoltageMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Step Safety", "Satisfying step safety Cond Voltage Max"); //lyw4_1

					return;
				}
			}
			//Voltage Min
			if (strcmp(lpScheduleSafety->strName, _T("VoMin")) == 0)
			{
				float fSrcValue = pComm->GetVoltage();
				float fDstValue = lpScheduleSafety->fValue;

				if (fSrcValue <= fDstValue && fDstValue != 0.000 && fSrcValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeStepSafetyVoltageMin;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Step Safety", "Satisfying step safety Cond Voltage Min"); //lyw4_1

					return;
				}
			}

			//Current Max
			if (strcmp(lpScheduleSafety->strName, _T("CuMax")) == 0)
			{
				float fSrcValue = abs(pComm->GetStepAmpereHour());
				float fDstValue = abs(lpScheduleSafety->fValue);

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeStepSafetyCurrentMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Step Safety", "Satisfying step safety Cond Current Max"); //lyw4_1

					return;
				}
			}

			//Current Min
			if (strcmp(lpScheduleSafety->strName, _T("CuMin")) == 0)
			{
				float fSrcValue = abs(pComm->GetAmpere());
				float fDstValue = abs(lpScheduleSafety->fValue);

				if (fSrcValue <= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeStepSafetyCurrentMin;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Step Safety", "Satisfying step safety Cond Current Min"); //lyw4_1

					return;
				}
			}

			//Capacity Max
			if (strcmp(lpScheduleSafety->strName, _T("CaMax")) == 0)
			{
				float fSrcValue = (float)abs(pComm->GetStepAmpereHour());
				float fDstValue = (float)abs(lpScheduleSafety->fValue);

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeStepSafetyCapacityMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "Step Safety", "Satisfying step safety Cond Capacity Max"); //lyw4_1

					return;
				}
			}

			//Temperature Max
			if (strcmp(lpScheduleSafety->strName, _T("CeTeMaxH")) == 0)
			{
				//220316 YGY Formation Temp Cut Add
				float fSrcValue = (float)abs(pComm->GetTemp());
				float fDstValue = (float)abs(lpScheduleSafety->fValue);

				if (fSrcValue >= fDstValue && fDstValue != 0.000)
				{
					pCond->bEvent = TRUE;
					pCond->nEndReason = eMonitroingCyclerCodeStepSafetyTemperatureMax;
					pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
					CString strmsg;
					strmsg.Format(_T("Satisfaction with the high limit of the system safety Cond Temp [%.3f] Safety[%.3f]"), fSrcValue, fDstValue);
					CMgrAlarm::GetMgr()->AddSysAlarm(m_nChannelNumber, "System safety", strmsg); //lyw4_1
					return;
				}
			}
		}

		++iter;
	}
}

//void CChannel::CheckTotalSafetyCondition(ENDCOND* pCond)
//{
//	CMgrConfig* pMgr = CMgrConfig::GetMgr();
//	CCommData* pComm = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);
//
//	//yuri 20201206
//	static char count_AmpereHour_error_check = 0,
//		count_voltageMAX_error_check = 0,
//		count_voltageMIN_error_check = 0,
//		count_curruntMAX_error_check = 0,
//		count_curruntMIN_error_check = 0;
//
//	static int totalTime[32] = { 0, };
//
//	char countMax = 0;
//
//
//	if (NULL == pComm || NULL == pMgr) {
//		SYSLOG(Dbg, _T("[FATAR ERROR] void CChannel::CheckSystemSatefyCondition(ENDCOND* pCond).. pComm, pMgr is null"));
//		return;
//	}
//
//	TOTAL_SAFETY totalSafety = pMgr->GetTotalSafety(m_nChannelNumber);
//
//	int nSrcValue = 0;
//	int nDstValue = 0;
//	int nSetValue = 0;
//
//	//yuri ��å�Ӵ� ����ó�� ���� 
//	//if (GetPause() == TRUE || GetScheduleStart() == FALSE || GetChannelStatus() != "START")
//	//	return;
//	if (/*GetPause() == TRUE || */GetScheduleStart() == FALSE || GetChannelStatus() != eCyclerChannelStateStart)
//		return;
//
//	// �뷮���� üũ
//	if (pComm->GetOPCode() == OpCodeChargeCC || pComm->GetOPCode() == OpCodeChargeCP || pComm->GetOPCode() == OpCodeChargeCV ||
//		pComm->GetOPCode() == OpCodeDischargeCC || pComm->GetOPCode() == OpCodeDischargeCP || pComm->GetOPCode() == OpCodeDischargeCV)
//	{
//		nSrcValue = static_cast<int>(abs(pComm->GetStepAmpereHour()) * 1000 * 1000);
//		nDstValue = static_cast<int>(totalSafety.fCapacity * 1000);
//
//		//TRACE("nSrcValue = %d, nDstValue = %d \n", nSrcValue, nDstValue);
//
//		//yuri 20201206
//		if (nSrcValue >= nDstValue) {
//			if (count_AmpereHour_error_check >= countMax)
//			{
//				pCond->bEvent = TRUE;
//				pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_CAPACITY_MAX;
//				pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//				return;
//			}
//			else
//			{
//				count_AmpereHour_error_check++;
//			}
//		}
//		else
//		{
//			count_AmpereHour_error_check = 0;
//		}
//	}
//
//	// ���� ������
//	if (totalSafety.fVoltage >= 0) {
//		if (pComm->GetOPCode() == OpCodeChargeCV ||
//			pComm->GetOPCode() == OpCodeDischargeCV)
//		{
//			CStep* step = GetMgr()->GetAtStep(pComm->GetCycleNumber(), pComm->GetStepNumber());
//
//			nSrcValue = static_cast<int>(pComm->GetVoltage() * 1000 * 10);
//			nDstValue = static_cast<int>(totalSafety.fVoltage * 1000 * 10);
//
//			if (pComm->GetOPCode() == OpCodeChargeCV)
//			{
//				nSetValue = static_cast<int>(step->GetChargeVoltage() * 1000 * 10);
//
//				//yuri 20201206
//				if (nSrcValue > (nSetValue + nDstValue)) {
//					if ((pComm->GetStepTime()) > 30) {
//						//yuri 20201206
//						if (count_voltageMAX_error_check >= countMax)
//						{
//							pCond->bEvent = TRUE;
//							pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_VOLTAGE_MAX;
//							pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//							return;
//						}
//						else
//						{
//							count_voltageMAX_error_check++;
//						}
//					}
//					else
//					{
//						count_voltageMAX_error_check = 0;
//					}
//				}
//			}
//			else
//			{
//				nSetValue = static_cast<int>(step->GetDischargeVoltage() * 1000 * 10);
//
//				if (nSrcValue < (nSetValue - nDstValue)) {
//					if ((pComm->GetStepTime()) > 30) {
//						//yuri 20201206
//						if (count_voltageMIN_error_check >= countMax)
//						{
//							pCond->bEvent = TRUE;
//							pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_VOLTAGE_MIN;
//							pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//							return;
//						}
//						else
//						{
//							count_voltageMIN_error_check++;
//						}
//					}
//					else
//					{
//						count_voltageMIN_error_check = 0;
//					}
//				}
//			}
//		}
//	}
//
//	// ���� ������ 
//	if (totalSafety.fCurrent >= 0) {
//		if (pComm->GetOPCode() == OpCodeChargeCC ||
//			pComm->GetOPCode() == OpCodeDischargeCC) {
//
//			CStep* step = GetMgr()->GetAtStep(pComm->GetCycleNumber(), pComm->GetStepNumber());
//
//			nSrcValue = static_cast<int>(abs(pComm->GetAmpere()) * 1000 * 10);
//			nDstValue = static_cast<int>(totalSafety.fCurrent * 10);
//
//			// True �� �ʱ�ȭ�ϴ� �κ� ����
//			/*if (m_bRestarting == TRUE) 
//			{
//				if (nSrcValue == 0)
//					return;
//				else {
//					m_bRestarting = FALSE;
//				}
//			}*/
//
//
//			if (pComm->GetOPCode() == OpCodeChargeCC) {
//
//				nSetValue = static_cast<int>(step->GetCurrentAmpere() * 1000 * 10);
//
//				// test code
//
//				/*if (nSrcValue > (nSetValue + 100) || nSrcValue < (nSetValue - 100)) {
//					TRACE("nSrcValue = %d, nDstValue = %d, nSetValue = %d\n", nSrcValue, nDstValue, nSetValue);
//					}*/
//
//				if (nSrcValue > (nSetValue + nDstValue)) {
//					if (count_curruntMAX_error_check >= countMax) {
//						pCond->bEvent = TRUE;
//						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_CURRENT_MAX;
//						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//						return;
//					}
//					else {
//						count_curruntMAX_error_check++;
//					}
//				}
//				else {
//					count_curruntMAX_error_check = 0;
//				}
//			}
//			else {
//				if (nSrcValue < (nSetValue - nDstValue)) {
//					if (count_curruntMIN_error_check >= countMax) {
//						pCond->bEvent = TRUE;
//						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_CURRENT_MAX;
//						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//						return;
//					}
//					else {
//						count_curruntMIN_error_check++;
//					}
//				}
//				else {
//					count_curruntMIN_error_check = 0;
//				}
//			}
//			//	}
//		}
//	}
//
//
//	// ����� �ð� ����
//	if (totalSafety.nTestEndTime > 0) {
//		CStep* step = GetMgr()->GetAtStep(pComm->GetCycleNumber(), pComm->GetStepNumber());
//
//		if (pComm->GetOPCode() == OpCodeDischargeCC ||
//			pComm->GetOPCode() == OpCodeChargeCC ||
//			pComm->GetOPCode() == OpCodeDischargeCV ||
//			pComm->GetOPCode() == OpCodeChargeCV)
//		{
//
//
//			float nSetCurrent = step->GetCurrentAmpere() * 1000; //�������� ����� �Է� ��
//			float nInputCurrent = totalSafety.fInputCapacity;
//			float nCRate = nInputCurrent / nSetCurrent;
//
//			auto c_rate = static_cast<int>(nCRate * 60 * 60 * 1000);
//			auto test_end_time = static_cast<int>(totalSafety.nTestEndTime);
//			totalTime[m_nChannelNumber] = c_rate + test_end_time;
//
//			if ((pComm->GetStepTime() * 100) > totalTime[m_nChannelNumber]) {
//				pCond->bEvent = TRUE;
//				pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_TEST_END_TIME;
//				pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//				return;
//			}
//		}
//	}
//
//
//	static int beforeVoltage[32] = { 0, };
//	static int noVoltageVarState[32] = { 0, };
//	int nDeviation = 0;
//	if (totalSafety.nNoVoltageVariation >= 0) {
//		if (pComm->GetOPCode() == OpCodeDischargeCC || pComm->GetOPCode() == OpCodeDischargeCP ||
//			pComm->GetOPCode() == OpCodeChargeCC || pComm->GetOPCode() == OpCodeChargeCP)
//		{
//			int nStepTime = pComm->GetStepTime();
//			nSrcValue = static_cast<int>(pComm->GetVoltage() * 1000 * 10);
//
//			if (nStepTime == 1) // ���� ó�� 0.1�ʰ� �Ǹ� ����
//			{
//				noVoltageVarState[m_nChannelNumber] = 0;
//				beforeVoltage[m_nChannelNumber] = nSrcValue;
//			}
//			else
//			{
//				nDeviation = abs(nSrcValue - beforeVoltage[m_nChannelNumber]);
//				if (nDeviation > 10) // ����� ���� ���簪 ��   ������ 0.1mv
//				{
//					beforeVoltage[m_nChannelNumber] = nSrcValue;
//					noVoltageVarState[m_nChannelNumber] = 0;
//				}
//				else
//				{
//					noVoltageVarState[m_nChannelNumber] += 100;
//				}
//
//				if (noVoltageVarState[m_nChannelNumber] >= totalSafety.nNoVoltageVariation) {
//					pCond->bEvent = TRUE;
//					pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_NO_VOLTAGE_VARIATION;
//					pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//				}
//			}
//		}
//	}
//
//	// Voltage variation (CC ���� ���� ��ȭ ��)
//	static int beforeVoltage_var[32] = { 0, };
//
//	if (totalSafety.fVoltageVariation >= 0) {
//		int nStepTime_var = pComm->GetStepTime() * 100;
//
//		if (nStepTime_var > 3000) {
//			if (pComm->GetOPCode() == OpCodeDischargeCC) {
//				nSrcValue = static_cast<int>(pComm->GetVoltage() * 1000 * 10);
//				nSrcValue = beforeVoltage_var[m_nChannelNumber] - nSrcValue;
//			}
//
//			if (pComm->GetOPCode() == OpCodeChargeCC) {
//				nSrcValue = static_cast<int>(pComm->GetVoltage() * 1000 * 10);
//				nSrcValue = nSrcValue - beforeVoltage_var[m_nChannelNumber];
//			}
//
//			if (pComm->GetOPCode() == OpCodeDischargeCC || pComm->GetOPCode() == OpCodeChargeCC) {
//				if (nSrcValue > (totalSafety.fVoltageVariation * 10)) {
//					pCond->bEvent = TRUE;
//					pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_CC_VOLTAGE_VARIATION;
//					pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//				}
//			}
//		}
//		beforeVoltage_var[m_nChannelNumber] = static_cast<int>(pComm->GetVoltage() * 1000 * 10);
//	}
//
//#if 0
//	// charge capacity variation (�⺻ false)
//	static CMap<int, int, int, int> beforeChargeCapacity[32];
//
//	if (totalSafety.fChargeCapacityVariation >= 0) {
//		if (pComm->GetcycleIndex() > 0) {
//
//			if (pComm->GetEndReason() == 0)
//			{
//				if (pComm->GetOPCode() == OpCodeChargeCC ||
//					pComm->GetOPCode() == OpCodeChargeCV ||
//					pComm->GetOPCode() == OpCodeChargeCP)
//				{
//					int nCh = m_nChannelNumber;
//					int nStep = pComm->GetStepNumber();
//
//					beforeChargeCapacity[nCh][nStep] = abs(pComm->GetChargeAmpereHour()) * 1000 * 1000;
//				}
//
//			}
//			else
//			{
//				int currentCapacity = abs(pComm->GetChargeAmpereHour()) * 1000 * 1000;
//				int range = currentCapacity * (totalSafety.fDischargeCapacityVariation / 100.f);
//
//				int nCh = m_nChannelNumber;
//				int nStep = pComm->GetStepNumber();
//
//				if ((beforeChargeCapacity[nCh][nStep] + range) < currentCapacity ||
//					(beforeChargeCapacity[nCh][nStep] - range) > currentCapacity)
//				{
//					pCond->bEvent = TRUE;
//					pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_DCHARGE_CAPA_VARIATION;
//					pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//					return;
//				}
//			}
//		}
//	}
//#endif
//
//
//	/*yuri 20201201 ���� start*/
//#if 1
//
//	// charge capacity variation (�⺻ false)
//	static int ChargeCapacity[32][1024][1024];
//
//	if (totalSafety.fChargeCapacityVariation >= 0) {
//		if (pComm->GetOPCode() == OpCodeChargeCC ||
//			pComm->GetOPCode() == OpCodeChargeCV ||
//			pComm->GetOPCode() == OpCodeChargeCP) {
//
//			int nCh = m_nChannelNumber;
//			int nStep = pComm->GetStepNumber();
//			int nLoop = pComm->GetcycleIndex();
//
//			if ((pComm->GetEndReason() > 0) && (nLoop > 1)) { //step ����� �� ��° loop �϶�   
//
//				int currentCapacity = ChargeCapacity[nCh][nLoop][nStep]; //���� �뷮
//				int beforCapacity = ChargeCapacity[nCh][nLoop - 1][nStep]; //���� ����Ŭ�� �뷮
//				int range = currentCapacity * static_cast<int>(totalSafety.fChargeCapacityVariation / 100.f);
//
//				if (currentCapacity > 0) {
//					if ((beforCapacity + range) < currentCapacity ||
//						(beforCapacity - range) > currentCapacity)
//					{
//						pCond->bEvent = TRUE;
//						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_CHARGE_CAPA_VARIATION;
//						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//						return;
//					}
//				}
//			}
//			else
//			{
//				if (pComm->GetEndReason() == 0)
//				{
//					ChargeCapacity[nCh][nLoop][nStep] = static_cast<int>(abs(pComm->GetChargeAmpereHour()) * 1000 * 1000);
//				}
//			}
//		}
//	}
//#endif
//	/*yuri 20201201 ���� end*/
//
//	// discharge capacity variation
//	static int DischargeCapacity[32][1024][1024];
//
//	if (totalSafety.fDischargeCapacityVariation >= 0) {
//		if (pComm->GetOPCode() == OpCodeDischargeCC ||
//			pComm->GetOPCode() == OpCodeDischargeCV ||
//			pComm->GetOPCode() == OpCodeDischargeCP) {
//
//			int nCh = m_nChannelNumber;
//			int nStep = pComm->GetStepNumber();
//			int nLoop = pComm->GetcycleIndex();
//
//			if ((pComm->GetEndReason() > 0) && (nLoop > 1)) {
//				int currentCapacity = DischargeCapacity[nCh][nLoop][nStep];
//				int   beforCapacity = DischargeCapacity[nCh][nLoop - 1][nStep];
//				int range = currentCapacity * static_cast<int>(totalSafety.fDischargeCapacityVariation / 100.f);
//
//				if (currentCapacity > 0) {
//					if ((beforCapacity + range) < currentCapacity ||
//						(beforCapacity - range) > currentCapacity)
//					{
//						pCond->bEvent = TRUE;
//						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_DCHARGE_CAPA_VARIATION;
//						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//					}
//				}
//			}
//			else
//			{
//				if (pComm->GetEndReason() <= 0)
//				{
//					DischargeCapacity[nCh][nLoop][nStep] = static_cast<int>(abs(pComm->GetDischargeAmpereHour()) * 1000 * 1000);
//				}
//			}
//		}
//	}
//
//	// chamber max temp (�̻��, è�� ���� �Ǿ� ���� ����.)
//
//	// sample max temp
//	if (totalSafety.fSampleMaxTemp >= 0) {
//		if (pComm->GetTemp() > totalSafety.fSampleMaxTemp) {
//			pCond->bEvent = TRUE;
//			pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_TEMP_MAX;
//			pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//			return;
//		}
//	}
//
//	// SOH (�⺻ false)
//	static int nSOH_SrcValue[32];
//	if (totalSafety.fSOH >= 0) {
//		if (pComm->GetcycleIndex() > 0) {
//			if (pComm->GetOPCode() == OpCodeDischargeCC || pComm->GetOPCode() == OpCodeDischargeCV || pComm->GetOPCode() == OpCodeDischargeCP) {
//				if (pComm->GetEndReason() > 0) {
//					if (nSOH_SrcValue[m_nChannelNumber] <= (totalSafety.fSOH)) {
//						pCond->bEvent = TRUE;
//						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_SOH;
//						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//						return;
//					}
//				}
//				else if (pComm->GetEndReason() <= 0) {
//					//get = 0.000 Ah, totalSafety.fSOH = mAh
//					nSOH_SrcValue[m_nChannelNumber] = static_cast<int>(abs(pComm->GetDischargeAmpereHour()) * 1000);
//
//					//TRACE("nSOH_SrcValue= %d, totalSafety.fSOH = %f, GetDischargeAmpereHour =  %f\n", nSOH_SrcValue, totalSafety.fSOH, abs(pComm->GetDischargeAmpereHour()));
//				}
//			}
//		}
//	}
//
//	/*
//	static int nSOH_SrcValue;
//	if (totalSafety.fSOH >= 0){
//		if (pComm->GetcycleIndex() > 0){
//			if (pComm->GetOPCode() == OpCodeDischargeCC || pComm->GetOPCode() == OpCodeDischargeCV || pComm->GetOPCode() == OpCodeDischargeCP){
//				//get = 0.000 Ah, totalSafety.fSOH = mAh
//				nSOH_SrcValue = abs(pComm->GetDischargeAmpereHour()) * 1000;
//
//				//TRACE("nSOH_SrcValue= %d, totalSafety.fSOH = %f, GetDischargeAmpereHour =  %f\n", nSOH_SrcValue, totalSafety.fSOH, abs(pComm->GetDischargeAmpereHour()));
//
//				if (nSOH_SrcValue >= (totalSafety.fSOH)){
//						pCond->bEvent = TRUE;
//						pCond->nEndReason = ENDREASON_CODE_TOTAL_SAFETY_SOH;
//						pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
//						return;
//				}
//			}
//		}
//	}
//	*/
//}
void CChannel::CheckStepEndCondition(ENDCOND* pCond)
{
	CCommData* pComm = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

	CStep* pStep = m_pMgr->GetAtStep(m_nCycleNumber, m_nStepNumber);

	if (NULL == pComm || NULL == pStep)
	{
		SYSLOG(Dbg, _T("[FATAR ERROR] void CChannel::CheckStepEndCondition(ENDCOND* pCond).. pComm, pStep is null"));

		return;
	}

	//taek ���� Ÿ�� ���� ���� �� �ܺ� �����̶�� ���� ����	
	int nPatternTime = pStep->GetPattern()->GetPatternTime();
	int nTimeCondition = 0, nFinishTime = 0;

	if (nPatternTime > 0)
	{
		if (pComm->GetPatternTotalTime() == pStep->GetPatternTime()) //taek �ð� ���� ������
		{
			CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();

			if (pMgrChamber)
			{
				SChamber* lpDeviceChamber = pMgrChamber->GetDevice(m_nChannelNumber);

				if (lpDeviceChamber)
				{
					lpDeviceChamber->bSendChangeValue = true;
					lpDeviceChamber->bSendOnCommand = true;
					lpDeviceChamber->bChamberExec = true;
					lpDeviceChamber->bSendOffCommand = false;
				}
			}
		}
		else
		{
			CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();

			if (pMgrChamber)
			{
				SChamber* lpDeviceChamber = pMgrChamber->GetDevice(m_nChannelNumber);

				if (lpDeviceChamber)
				{
					lpDeviceChamber->bSendChangeValue = false;
				}
			}
		}

		//�ð� ����
		SScheduleComplete* lpScheduleCompleteTime = pStep->GetScheduleCompleteFromName(_T("Ti"));

		if (lpScheduleCompleteTime)
		{
			nTimeCondition = (int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f);
			nFinishTime = pComm->GetPatternTotalTime();

			if ((0.f != nTimeCondition) && (nFinishTime >= (nTimeCondition)))
			{
				pCond->bEvent = TRUE;
			}

			if (pCond->bEvent == TRUE)
			{
				CheckNextStep(lpScheduleCompleteTime->nCycle, lpScheduleCompleteTime->nStep, pCond);

				pCond->nEndReason = eIFBoardCyclerCodeTimeCut;
			}
		}

		SScheduleComplete* lpScheduleCompleteCapacity = pStep->GetScheduleCompleteFromName(_T("Ca")); //taek �ϴ� FW���� �ȵǴ� ���⿡���� ����

		if (lpScheduleCompleteCapacity)
		{
			if ((0.f != lpScheduleCompleteCapacity->fValue) && (pComm->GetStepAmpereHourTotal() >= lpScheduleCompleteCapacity->fValue))
			{
				pCond->bEvent = TRUE;
			}

			if (pCond->bEvent == TRUE)
			{
				CheckNextStep(lpScheduleCompleteCapacity->nCycle, lpScheduleCompleteCapacity->nStep, pCond);

				pCond->nEndReason = eMonitroingCyclerCodeCycleChargeCapacity;
			}
		}

		//SOC
		if (true == IsSocUse())
		{
			CCycle* pCycleTarget = NULL;

			CStep* pStepTarget = NULL;

			pCycleTarget = GetMgr()->GetAtCycle(pStep->GetRatioCycle());

			if (pCycleTarget != NULL)
			{
				if ((GetCycleIndex() + 1) == pStep->GetRatioLoop()) //taek loop ��ȸ ���� Ȯ�� �ʿ�
				{
					pStepTarget = pCycleTarget->GetAtStep(pStep->GetRatioStep());
				}
			}

			if (pStepTarget != NULL)
			{
				int nPercent = pStep->GetRatioSoc();
				g_nRatioSpec[m_nChannelNumber] = nPercent;
				float fTargetValue = 0.f;

				if (pComm->GetCommCommand() == eIFBoardCommandCharge || pComm->GetCommCommand() == eIFBoardCommandDischarge)
				{
					int nRationType = pStep->GetRatioType(); //taek

					if (nRationType == 1)
					{
						fTargetValue = pStepTarget->GetSocTargetValue() * nPercent / 100;

						if (abs(pComm->GetPatternAmpereHour()) >= abs(fTargetValue))
						{
							pComm->SetSOCValue((int)(pComm->GetPatternAmpereHour() / pStepTarget->GetSocTargetValue() * 100));
							pCond->bEvent = TRUE;

							SScheduleComplete* lpScheduleCompleteSoc = pStep->GetScheduleCompleteFromName("RatioSoc");

							if (lpScheduleCompleteSoc)
							{
								if (pCond->bEvent == TRUE)
								{
									CheckNextStep(lpScheduleCompleteSoc->nCycle, lpScheduleCompleteSoc->nStep, pCond);

									pCond->nEndReason = eMonitroingCyclerCodeSoC;

									return;
								}
							}
						}
					}
					else if (nRationType == 2)
					{
						fTargetValue = pStepTarget->GetSocWattTargetValue() * nPercent / 100;
						//TRACE("Target : %.3f     Get : %.3f   \n", fTargetValue, pComm->GetStepWattHour());
						if (abs(pComm->GetStepWattHour()) >= abs(fTargetValue))
						{
							pComm->SetSOCValue((int)(pComm->GetPatternWattHour() / pStepTarget->GetSocWattTargetValue() * 100));
							pCond->bEvent = TRUE;

							SScheduleComplete* lpScheduleCompleteSoc = pStep->GetScheduleCompleteFromName("RatioSoc");

							if (lpScheduleCompleteSoc)
							{
								if (pCond->bEvent == TRUE)
								{
									CheckNextStep(lpScheduleCompleteSoc->nCycle, lpScheduleCompleteSoc->nStep, pCond);

									pCond->nEndReason = eMonitroingCyclerCodeSoE;

									//TRACE("2.step(%d)  Target : %.5f     Get : %.5f   \n", GetStepNumber(), fTargetValue, pComm->GetPatternWattHour());

									return;
								}
							}
						}
					}
					else
					{
						pComm->SetSOCValue(0);
					}
				}
			}
		}
	}

	SScheduleComplete* lpScheduleCompletePattern = pStep->GetScheduleCompleteFromName(_T("PTah")); //�̰� �����ٷ� ������

	if (lpScheduleCompletePattern)
	{
		if ((0.f != lpScheduleCompletePattern->fValue) && (pComm->GetPatternTotalAmpereHour() >= lpScheduleCompletePattern->fValue))
		{
			pCond->bEvent = TRUE;
		}

		if (pCond->bEvent == TRUE)
		{
			CheckNextStep(lpScheduleCompletePattern->nCycle, lpScheduleCompletePattern->nStep, pCond);

			pCond->nEndReason = eMonitroingCyclerCodeCycleChargeCapacity;
		}
	} //taek SW �󿡼� Total ah ã�´�.

	//Total Ah
	SScheduleComplete* lpScheduleCompleteTotalCapacity = pStep->GetScheduleCompleteFromName(_T("Tah")); //�̰� �����ٷ� ������

	if (lpScheduleCompleteTotalCapacity)
	{
		if ((0.f != lpScheduleCompleteTotalCapacity->fValue) && (abs(pComm->GetScheduleAmpereHour()) >= lpScheduleCompleteTotalCapacity->fValue)) //taek ���밪���� Ȯ�� �ؾ� �� 200731
		{
			pCond->bEvent = TRUE;
		}

		if (pCond->bEvent == TRUE)
		{
			CheckNextStep(lpScheduleCompleteTotalCapacity->nCycle, lpScheduleCompleteTotalCapacity->nStep, pCond);

			pCond->nEndReason = eMonitroingCyclerCodeCycleChargeCapacity;
		}
	} 
	
	////Step Ah
	//SScheduleComplete* lpScheduleCompleteStepCapacity = pStep->GetScheduleCompleteFromName(_T("Ca")); //�̰� �����ٷ� ������

	//if (lpScheduleCompleteStepCapacity)
	//{
	//	if ((0.f != lpScheduleCompleteStepCapacity->fValue) && (abs(pComm->GetStepAmpereHourTotal()) >= lpScheduleCompleteStepCapacity->fValue)) //taek ���밪���� Ȯ�� �ؾ� �� 200731
	//	{
	//		pCond->bEvent = TRUE;
	//	}

	//	if (pCond->bEvent == TRUE)
	//	{
	//		CheckNextStep(lpScheduleCompleteStepCapacity->nCycle, lpScheduleCompleteStepCapacity->nStep, pCond);

	//		pCond->nEndReason = eMonitroingCyclerCodeCycleChargeCapacity;
	//	}
	//}
	//taek SW �󿡼� Total ah ã�´�.
	BOOL bRetCheckSafetyCondition = FALSE;
	//DAQ
	std::vector<SDaQComplete*>& vecDaQComplete = pStep->GetDaQComplete();

	auto iter = vecDaQComplete.begin();
	auto iterE = vecDaQComplete.end();

	while (iter != iterE)
	{
		SDaQComplete* lpDaQComplete = (*iter);

		if (lpDaQComplete)
		{
			if (lpDaQComplete->nUniqueID == 0)
				continue;

			CDAQList* mapDAQ = CMgrDAQ::GetMgr()->GetDAQMap(lpDaQComplete->nUniqueID);

			if (NULL == mapDAQ)
				continue;

			auto PosDAQ = mapDAQ->GetDAQList().GetHeadPosition();

			int nDaqSensorType = 0;
			int nChannelNum = 0;
			int nDaqChannelNum = 0;
			float fDaqValue = 0;
			CString strmsg = "";
			CString strErrType = "";
			while (PosDAQ)
			{
				CDAQ* pDAQ = mapDAQ->GetDAQList().GetNext(PosDAQ);

				if (NULL == pDAQ)
					continue;

				if (pDAQ->GetDaqUse() == FALSE)
					continue;

				nChannelNum = this->GetChannelNumber();
				nDaqSensorType = pDAQ->GetSensorType();
				nDaqChannelNum = pDAQ->GetChannelNumber();

				if (nChannelNum == nDaqChannelNum)
				{
					if (nDaqSensorType == READ_TYPE_VOLT)
					{
						fDaqValue = pDAQ->GetRealVoltage();
						strErrType = "Volt";
					}
					else
					{
						fDaqValue = pDAQ->GetRealTemperature();
						strErrType = "Temp";
					}

					switch (lpDaQComplete->nOperator)
					{
					case eDaQOperatorLessThan:
						pCond->bEvent = (fDaqValue < lpDaQComplete->fValue) ? TRUE : FALSE;
						strmsg.Format(_T("DAQ %s Min Arlam"), strErrType);
						break; //���� ����
					case eDaQOperatorLessEqual:
						pCond->bEvent = (fDaqValue <= lpDaQComplete->fValue) ? TRUE : FALSE;
						strmsg.Format(_T("DAQ %s Min Arlam"), strErrType);
						break;
					case eDaQOperatorGreaterThan:
						pCond->bEvent = (fDaqValue > lpDaQComplete->fValue) ? TRUE : FALSE;
						strmsg.Format(_T("DAQ %s Max Arlam"), strErrType);
						break;
					case eDaQOperatorGraterEqual:
						pCond->bEvent = (fDaqValue >= lpDaQComplete->fValue) ? TRUE : FALSE;
						strmsg.Format(_T("DAQ %s Max Arlam"), strErrType);
						break;
					case eDaQOperatorEqual:
						pCond->bEvent = (fDaqValue == lpDaQComplete->fValue) ? TRUE : FALSE;
						strmsg.Format(_T("DAQ %s is equal Arlam"), strErrType);
						break;
					case eDaQOperatorDifferent:
						pCond->bEvent = (fDaqValue != lpDaQComplete->fValue) ? TRUE : FALSE;
						strmsg.Format(_T("DAQ %s is not equal Arlam"), strErrType);
						break;
					}
				}
				else {/*SKIP..*/ }

				if (pCond->bEvent == TRUE)
				{
					CheckNextStep(lpDaQComplete->nNextCycle, lpDaQComplete->nNextStep, pCond);

					if (nDaqSensorType == READ_TYPE_VOLT)
					{
						m_strEndCondition = _T("DAQ Voltage");
						pCond->nEndReason = eMonitroingCyclerCodeDaQVoltage;

					}
					else
					{
						m_strEndCondition = _T("DAQ Temperature");
						pCond->nEndReason = eMonitroingCyclerCodeDaQTemperature;
					}

					return;
				}
			}
		}

		++iter;
	}

	//SOC
	if (true == IsSocUse())
	{
		CCycle* pCycleTarget = NULL;

		CStep* pStepTarget = NULL;

		pCycleTarget = GetMgr()->GetAtCycle(pStep->GetRatioCycle());

		if (pCycleTarget != NULL)
		{
			if ((GetCycleIndex() + 1) == pStep->GetRatioLoop()) //taek loop ��ȸ ���� Ȯ�� �ʿ�
			{
				pStepTarget = pCycleTarget->GetAtStep(pStep->GetRatioStep());
			}
		}

		if (pStepTarget != NULL)
		{
			int nPercent = pStep->GetRatioSoc();
			g_nRatioSpec[m_nChannelNumber] = nPercent;
			float fTargetValue = 0.f;

			if (pComm->GetCommCommand() == eIFBoardCommandCharge || pComm->GetCommCommand() == eIFBoardCommandDischarge)
			{
				int nRationType = pStep->GetRatioType(); //taek

				if (nRationType == 1)
				{
					fTargetValue = pStepTarget->GetSocTargetValue() * nPercent / 100;

					if (abs(pComm->GetStepAmpereHour()) >= abs(fTargetValue))
					{
						pComm->SetSOCValue((int)(pComm->GetStepAmpereHour() / pStepTarget->GetSocTargetValue() * 100));
						pCond->bEvent = TRUE;

						SScheduleComplete* lpScheduleCompleteSoc = pStep->GetScheduleCompleteFromName("RatioSoc");

						if (lpScheduleCompleteSoc)
						{
							if (pCond->bEvent == TRUE)
							{
								CheckNextStep(lpScheduleCompleteSoc->nCycle, lpScheduleCompleteSoc->nStep, pCond);

								pCond->nEndReason = eMonitroingCyclerCodeSoC;

								TRACE("1.step(%d)  Target : %.5f     Get : %.5f   \n", GetStepNumber(), fTargetValue, pComm->GetStepAmpereHour());

								return;
							}
						}
					}
				}
				else if (nRationType == 2)
				{
					fTargetValue = pStepTarget->GetSocWattTargetValue() * nPercent / 100;
					//TRACE("Target : %.3f     Get : %.3f   \n", fTargetValue, pComm->GetStepWattHour());
					if (abs(pComm->GetStepWattHour()) >= abs(fTargetValue))
					{
						pComm->SetSOCValue((int)(pComm->GetStepWattHour() / pStepTarget->GetSocWattTargetValue() * 100));
						pCond->bEvent = TRUE;

						SScheduleComplete* lpScheduleCompleteSoc = pStep->GetScheduleCompleteFromName("RatioSoc");

						if (lpScheduleCompleteSoc)
						{
							if (pCond->bEvent == TRUE)
							{
								CheckNextStep(lpScheduleCompleteSoc->nCycle, lpScheduleCompleteSoc->nStep, pCond);

								pCond->nEndReason = eMonitroingCyclerCodeSoE;

								TRACE("2.step(%d)  Target : %.5f     Get : %.5f   \n", GetStepNumber(), fTargetValue, pComm->GetStepWattHour());

								return;
							}
						}
					}
				}
				else
				{
					pComm->SetSOCValue(0);
				}
			}
		}
	}

	//SOH
	if (true == IsSoHUse())
	{
		CCycle* pCycleTarget = NULL;
		CStep* pStepTarget = NULL;

		CCycle* pCycleTarget2 = NULL;
		CStep* pStepTarget2 = NULL;

		pCycleTarget = GetMgr()->GetAtCycle(pStep->GetRatioCycleSOH());
		pCycleTarget2 = GetMgr()->GetAtCycle(pStep->GetRatioCycleSOH2());

		if (pCycleTarget != NULL)
		{
			if ((GetCycleIndex() + 1) == pStep->GetRatioSOHLoop1()) //taek loop ��ȸ ���� Ȯ�� �ʿ�
			{
				pStepTarget = pCycleTarget->GetAtStep(pStep->GetRatioStepSOH());
				SetSOHTargetStep(pStepTarget);
				//pStepTarget ���� channel���� ������ �ִٰ� �� �ؾ� �ҵ�
			}
		}

		if (pCycleTarget2 != NULL)
		{
			if ((GetCycleIndex() + 1) == pStep->GetRatioSOHLoop2()) //taek loop ��ȸ ���� Ȯ�� �ʿ�
			{
				pStepTarget2 = pCycleTarget2->GetAtStep(pStep->GetRatioStepSOH2());
			}
		}

		if (m_pStepTarget != NULL && pStepTarget2 != NULL)
		{
			int nPercent = pStep->GetRatioSoH();
			float fTargetValue = 0.f;

			// 			if (pComm->GetCommCommand() == IFCommCmdTypeDischarge ||
			// 				pComm->GetCommCommand() == IFCommCmdTypeCharge) //taek ���� ���� ������ �ּ� ó��
			{
				if (pStep->GetSOHReference() > 0) // ���� ������ ���� �� //�𸣰ڴ� ����ν�� rest �߰� ��� �ۿ� ������ �ȳ�... //soh�� ������ 1ȸ �ǰ� �� ������ �Ǵ��ؾ���
				{
					fTargetValue = abs(/*pComm->GetStepAmpereHour()*/ pStepTarget2->GetSoHTargetValue2() / pStep->GetSOHReference() * 100); //before TargetValue�� ã�Ƽ� �� ���̶� ���� ���̶� ��
				}
				else
				{
					fTargetValue = abs(/*pComm->GetStepAmpereHour()*/ pStepTarget2->GetSoHTargetValue2() / m_pStepTarget->GetSoHTargetValue() * 100); //before TargetValue�� ã�Ƽ� �� ���̶� ���� ���̶� ��
				}

				if (nPercent <= fTargetValue) // ä�� �Ϸ� �Ǿ��� ��
				{
					pComm->SetSOHValue((int)fTargetValue);
					pCond->bEvent = TRUE;

					SScheduleComplete* lpScheduleCompleteSoh = pStep->GetScheduleCompleteFromName("RatioSoc");

					if (lpScheduleCompleteSoh)
					{
						if (pCond->bEvent == TRUE)
						{
							CheckNextStep(lpScheduleCompleteSoh->nCycle, lpScheduleCompleteSoh->nStep, pCond);

							pCond->nEndReason = eMonitroingCyclerCodeSoH;

							return;
						}
					}
				}
			}
			// 			else
			// 			{
			// 				pComm->SetSOHValue(0);
			// 			} //taek ���̽��� ������ �ϴ� �ּ� ó��
		}
	}
#if 0 // ���� ����� Normal �˶� �߻�. 2020-120-17 cnz

	// lpScheduleCompleteChamber->fValu �� ������ 0
	SScheduleComplete* lpScheduleCompleteChamber = pStep->GetScheduleCompleteFromName("Chamber");

	if (lpScheduleCompleteChamber)
	{
		if (pComm->GetChamberTemperature() >= lpScheduleCompleteChamber->fValue)
		{
			//pCond->bEvent = TRUE;
			pCond->nEndReason = eMonitroingCyclerCodeChamber;
		}

		if (pCond->bEvent == TRUE)
		{
			//CheckNextStep(lpScheduleCompleteChamber->nCycle, lpScheduleCompleteChamber->nStep, pCond);

			pCond->nEndReason = eMonitroingCyclerCodeChamber;
		}
	}

#endif // 0
}

void CChannel::CheckUserEvent(ENDCOND* pCond)
{
	CCommData* pCommdata = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);

	if (pCommdata == NULL)
	{
		pCond->bEvent = FALSE;
		return;
	}

	if (pCommdata->GetStepIndex() > 0)
	{
		if (GetPauseStep())		//���� �Ϸ��� �Ͻ� ����
		{
			if (true == GetIsStepChange())
			{
				pCond->bEvent = TRUE;
				pCond->nEndReason = eMonitroingCyclerCodeStepPause;
				pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				SetPauseStep(FALSE);
				return;
			}
		}
		if (GetPauseCycle())	//����Ŭ �Ϸ��� �Ͻ� ����
		{
			if (true == GetIsCycleChange())
			{
				pCond->bEvent = TRUE;
				pCond->nEndReason = eMonitroingCyclerCodeCyclePause;
				pCond->nEndType = ENDREASON_EVENT_TYPE_PAUSE;
				SetPauseCycle(FALSE);
				return;
			}
		}

		if (GetCompleteStep())	//���� �Ϸ��� ��� �Ϸ�
		{
			if (true == GetIsStepChange())
			{
				pCond->bEvent = TRUE;
				pCond->nEndReason = eMonitroingCyclerCodeStepComplete;
				pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
				SetCompleteStep(FALSE);
				return;
			}
		}
		if (GetCompleteCycle())	//����Ŭ �Ϸ��� ��� �Ϸ�
		{
			if (true == GetIsCycleChange())
			{
				pCond->bEvent = TRUE;
				pCond->nEndReason = eMonitroingCyclerCodecycleComplete;
				pCond->nEndType = ENDREASON_EVENT_TYPE_COMPLETE;
				SetCompleteCycle(FALSE);
				return;
			}

		}
	}
}

void CChannel::CheckFlowStatus(CCommData* pCommData)
{
	int nNextProc = eCyclerChannelStateIdle;

	auto chNum = pCommData->GetChannelNumber();

	if (GetChannelStatus() == eCyclerChannelStateIdle)
	{
		while (TRUE)
		{
			if (true == pCommData->IsStatusAlaram())
			{
				nNextProc = eCyclerChannelStateError;
				break;
			}
			if (true == pCommData->IsStatusReady())
			{
				nNextProc = eCyclerChannelStateReady;
				break;
			}
			if (true == pCommData->IsStatusPause())
			{
				nNextProc = eCyclerChannelStatePause;
				break;
			}
			// nothing
			nNextProc = eCyclerChannelStateIdle;
			break;
		}
	}

	if (GetChannelStatus() == eCyclerChannelStateReady)
	{// possible next proc (error, run, idle)

		while (TRUE)
		{
			if (true == pCommData->IsStatusAlaram())
			{
				nNextProc = eCyclerChannelStateError;
				break;
			}
			if (true == pCommData->IsStatusStart())
			{
				nNextProc = eCyclerChannelStateStart;
				break;
			}
			if (true == pCommData->IsStatusIdle())
			{
				nNextProc = eCyclerChannelStateIdle;
				break;
			}
			if (true == pCommData->IsStatusPause())
			{
				nNextProc = eCyclerChannelStatePause;
				break;
			}
			// nothing
			nNextProc = eCyclerChannelStateReady;
			break;
		}
	}

	if (GetChannelStatus() == eCyclerChannelStateStart)
	{// possible next proc (error, complete, pause)

		while (TRUE)
		{
			if (true == pCommData->IsStatusAlaram())
			{
				nNextProc = eCyclerChannelStateError;
				break;
			}
			if (true == pCommData->IsStatusComplete())
			{
				nNextProc = eCyclerChannelStateComplete;
				break;
			}
			if (true == pCommData->IsStatusPause())
			{
				nNextProc = eCyclerChannelStatePause;
				break;
			}
			if (true == pCommData->IsStatusPauseDelay())
			{
				nNextProc = eCyclerChannelStatePauseFromDwell;
				break;
			}
			if (true == pCommData->IsStatusPauseStep())
			{
				nNextProc = eCyclerChannelStatePauseFromStepSync;
				break;
			}
			//18�� Flag ���̻� �����������. Complete ó�� �ϵ��� ���� 2020.12.17
			if (true == pCommData->IsStatusPauseSungOn())
			{
				//nNextProc = eCyclerChannelStatePauseFromWaitChamber;
				nNextProc = eCyclerChannelStateComplete;
				break;
			}
			if (true == pCommData->IsStatusPauseTemp())
			{
				nNextProc = eCyclerChannelStatePauseFromTempSync;
				break;
			}
			// nothing
			nNextProc = eCyclerChannelStateStart;
			break;
		}
	}

	if (GetChannelStatus() == eCyclerChannelStatePause)
	{// possible next proc (error, complete (user), run)

		while (TRUE)
		{
			if (true == pCommData->IsStatusAlaram())
			{
				nNextProc = eCyclerChannelStateError;
				break;
			}
			if (true == pCommData->IsStatusComplete())
			{
				nNextProc = eCyclerChannelStateComplete;
				break;
			}
			if (true == pCommData->IsStatusStart())
			{
				nNextProc = eCyclerChannelStateStart;
				break;
			}
			// nothing
			nNextProc = eCyclerChannelStatePause;
			break;
		}
	}

	if (GetChannelStatus() == eCyclerChannelStatePauseFromDwell)
	{// possible next proc (error, complete (user), run)

		while (TRUE)
		{
			if (true == pCommData->IsStatusAlaram())
			{
				nNextProc = eCyclerChannelStateError;
				break;
			}
			if (true == pCommData->IsStatusComplete())
			{
				nNextProc = eCyclerChannelStateComplete;
				break;
			}
			if (true == pCommData->IsStatusStart())
			{
				nNextProc = eCyclerChannelStateStart;
				break;
			}
			// nothing
			nNextProc = eCyclerChannelStatePauseFromDwell;
			break;
		}
	}

	if (GetChannelStatus() == eCyclerChannelStatePauseFromStepSync)
	{// possible next proc (error, complete (user), run)

		while (TRUE)
		{
			if (true == pCommData->IsStatusAlaram())
			{
				nNextProc = eCyclerChannelStateError;
				break;
			}
			if (true == pCommData->IsStatusComplete())
			{
				nNextProc = eCyclerChannelStateComplete;
				break;
			}
			if (true == pCommData->IsStatusStart())
			{
				nNextProc = eCyclerChannelStateStart;
				break;
			}
			// nothing
			nNextProc = eCyclerChannelStatePauseFromStepSync;
			break;
		}
	}

	if (GetChannelStatus() == eCyclerChannelStatePauseFromWaitChamber)
	{// possible next proc (error, complete (user), run)

		while (TRUE)
		{
			if (true == pCommData->IsStatusAlaram())
			{
				nNextProc = eCyclerChannelStateError;
				break;
			}
			if (true == pCommData->IsStatusComplete())
			{
				nNextProc = eCyclerChannelStateComplete;
				break;
			}
			if (true == pCommData->IsStatusStart())
			{
				nNextProc = eCyclerChannelStateStart;
				break;
			}
			// nothing
			nNextProc = eCyclerChannelStatePauseFromWaitChamber;
			break;
		}
	}

	if (GetChannelStatus() == eCyclerChannelStatePauseFromTempSync)
	{// possible next proc (error, complete (user), run)

		while (TRUE)
		{
			if (true == pCommData->IsStatusAlaram())
			{
				nNextProc = eCyclerChannelStateError;
				break;
			}
			if (true == pCommData->IsStatusComplete())
			{
				nNextProc = eCyclerChannelStateComplete;
				break;
			}
			if (true == pCommData->IsStatusStart())
			{
				nNextProc = eCyclerChannelStateStart;
				break;
			}
			// nothing
			nNextProc = eCyclerChannelStatePauseFromTempSync;
			break;
		}
	}


	if (GetChannelStatus() == eCyclerChannelStateComplete)
	{// possible next proc (error, run, idle, ���� �Ϸ�� ��谡 ��ȣ�ϴ� ready�� �ϴ� ����)

		while (TRUE)
		{
			if (true == pCommData->IsStatusAlaram())
			{
				nNextProc = eCyclerChannelStateError;
				break;
			}
			//220208 YGYUN Check ~~~ ???
			//if (true == pCommData->IsStatusStart())
			//{
			//	
			//	nNextProc = eCyclerChannelStateStart;
			//	break;
			//}
			if (true == pCommData->IsStatusIdle())
			{
				nNextProc = eCyclerChannelStateIdle;
				break;
			}
			// nothing
			nNextProc = eCyclerChannelStateComplete;
			break;
		}
	}

	if (GetChannelStatus() == eCyclerChannelStateError)
	{// possible next proc (error, idle, ready)

		while (TRUE)
		{
			if (true == pCommData->IsStatusAlaram())
			{
				nNextProc = eCyclerChannelStateError;
				break;
			}
			if (true == pCommData->IsStatusIdle())
			{
				nNextProc = eCyclerChannelStateIdle;
				break;
			}
			if (true == pCommData->IsStatusStart())
			{
				nNextProc = eCyclerChannelStateStart;;
				break;
			}
			if (true == pCommData->IsStatusReady())
			{
				nNextProc = eCyclerChannelStateReady;
				break;
			}
			if (true == pCommData->IsStatusPause())
			{
				nNextProc = eCyclerChannelStatePause;
				break;
			}
			if (true == pCommData->IsStatusPauseDelay())
			{
				nNextProc = eCyclerChannelStatePauseFromDwell;
				break;
			}
			if (true == pCommData->IsStatusPauseStep())
			{
				nNextProc = eCyclerChannelStatePauseFromStepSync;
				break;
			}
			nNextProc = eCyclerChannelStateError;
			break;
		}
	}

	if (GetChannelStatus() != nNextProc)
	{//���� ���� ��
		SetChannelStatus(nNextProc);

		if (GetChannelStatus() == eCyclerChannelStateComplete || GetChannelStatus() == eCyclerChannelStateError)
		{
			IncreaseRunningCount();
		}

		// Add By JSK - 2020.07.02 ���⼭ ��Ƽ�� �ִٸ� �����غ���? ��� Ÿ�̹����� ������ �ָ��ϴ�.
		if (GetChannelStatus() == eCyclerChannelStateComplete)
		{
			if ((int)m_vecMultiSchedule.size() > 0)
			{
				CWnd* pWnd = AfxGetApp()->GetMainWnd();

				if (pWnd)
					::PostMessage(pWnd->GetSafeHwnd(), WM_MULTI_SCHEDULE_START, GetChannelNumber(), 0);
			}
		}
	}
}


bool CChannel::ProcessingResultFile(bool bIsPause, CCommData* pCommData)
{
	//Schedule ���� �߿� Error �� Pause �϶� Logging �ؾ� ��.
	CChannel* pChannel = CMgrChannel::GetMgr()->GetAt(pCommData->GetChannelNumber());
	if (pChannel->GetScheduleStart() == true)
	{
		int n = pChannel->GetAccurateReason();
		if (pChannel->GetChannelStatus() == eCyclerChannelStateError || pChannel->GetChannelStatus() == eCyclerChannelStatePause)
		{
			CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

			if (pCommData->GetWritelogCount() == pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectErrorLoggingTime) * 10)
			{
				pCommData->WriteLogCount(0);
				CMgrLog::GetMgr()->WriteLogRun(m_nChannelNumber, pCommData);
			}
		}
	}

	if (true == bIsPause)
	{
		return false;
	}


	if (pCommData == NULL)
		return false;

	if (pCommData->GetStepIndex() <= 0)
		return false;

	CStep *pStep = this->GetMgr()->GetAtStep(pCommData->GetStepIndex());

	if (pStep == NULL)
		return false;

	//SScheduleLog* lpScheduleLog2 = pStep->GetScheduleLogFromName("Log");

	//if (!lpScheduleLog2)
	//	return false;

	BOOL bWriteLog = TRUE;

	SScheduleLog* lpScheduleLog = pStep->GetScheduleLogFromName("Ti");

	if (!lpScheduleLog)
		return false;

	if (lpScheduleLog->nTime != pCommData->GetWritelogCount())
		bWriteLog = FALSE;

	//Schedule ���� �߿� ����, ����, �µ� ��ȭ �� Logging
	float fTemp = 0.0;
	if (pChannel->GetScheduleStart() == true)
	{
		//������
		SScheduleLog* lpScheduleLogVolt = pStep->GetScheduleLogFromName("Vo");
		fTemp = abs(pCommData->GetVoltage() - pCommData->GetPreVoltage());

		if (lpScheduleLogVolt->fTime != 0 && fTemp >= lpScheduleLogVolt->fTime)
		{
			CMgrLog::GetMgr()->WriteLogRun(m_nChannelNumber, pCommData);
		}

		//������ 
		SScheduleLog* lpScheduleLogCur = pStep->GetScheduleLogFromName("Cu");
		fTemp = abs(pCommData->GetAmpere() - pCommData->GetPreAmpere());

		if (lpScheduleLogCur->fTime != 0 && fTemp >= lpScheduleLogCur->fTime)
		{
			CMgrLog::GetMgr()->WriteLogRun(m_nChannelNumber, pCommData);
		}

		//�µ���
		SScheduleLog* lpScheduleLogTemp = pStep->GetScheduleLogFromName("Te");

		if (lpScheduleLogTemp->fTime != 0)
		{
			CList< CDAQ*, CDAQ* >& listDAQ = pCommData->GetDAQList();

			CDAQ* pDAQ = NULL;
			CString strValue;

			POSITION Pos = listDAQ.GetHeadPosition();
			for (int i = 0; i < listDAQ.GetCount(); i++)
			{
				pDAQ = listDAQ.GetNext(Pos);

				if (pDAQ == NULL)
				{
					continue;
				}

				if (m_nChannelNumber == pDAQ->GetChannelNumber())
				{
					if (pDAQ->GetSensorType() == READ_TYPE_TEMPERATURE && pDAQ->GetDaqUse())
					{
						fTemp = abs(pDAQ->GetRealTemperature() - pDAQ->GetPreRealTemperature());
						if (fTemp >= lpScheduleLogTemp->fTime)
						{
							CMgrLog::GetMgr()->WriteLogRun(m_nChannelNumber, pCommData);
						}
					}
				}
			}
		}
	}
	/////////////////////////////////////////////////////

	//write result file
	if (bWriteLog == TRUE || pCommData->GetEndReason() > 0 && bIsPause == FALSE)
	{
		//lyw - �α� ī��Ʈ �ʱ�ȭ ����
		pCommData->WriteLogCount(0);

		CMgrLog::GetMgr()->WriteLogRun(m_nChannelNumber, pCommData);	//������ ���常 ������..

		if (pCommData->GetEndReason() > 0)
		{
			//pnx_hr DCIR  2021.05.26 ����� ���� �������� ������ ���� ��
			CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(m_nChannelNumber);
			float fVolt = pCommData->GetVoltage();
			pCommData->SetStepEndVoltage(fVolt);
			pCommData->m_fStepTotalAmpere = 0; //���� ���� �� ������� 0
			CMgrLog::GetMgr()->WriteLogEnd(m_nChannelNumber, pCommData);

			pCommData->InitWH();
			pCommData->SetBeforStepTimeV(0);
			pCommData->SetBeforStepTimeI(0);
			pCommData->SetDIBefor(0);
			pCommData->SetDVBefor(0);
			pCommData->WriteLogCount(0);

			this->SetAccurateReason(0);
			this->SetSystemSafetyReason(0); //lyw3_1
		}
	}

	return true;
}

void CChannel::InitalizeReady()
{
	SetSampleName("");
	SetDescription("");
	SetEndCondition(_T(""));
	SetScheduleStart(FALSE);
	SetChannelStart(FALSE);
	SetChannelEnd(FALSE);
	SetPauseStep(FALSE);
	SetPauseCycle(FALSE);
	SetCompleteCycle(FALSE);
	SetCompleteStep(FALSE);
	SetCycleIndex(0);
	SetFlagSendCommandJump(false);
	SetAccurateReason(0);
	SetSystemSafetyReason(0); //lyw3_1
}

void	CChannel::SetTotalCycleLoop(int nCurrnetLoop, int nStepIndex, bool bCycleChange)
{
	CCycle* pCycle = m_pMgr->GetAtCycle(GetCycleNumber());

	if (pCycle == NULL || nStepIndex == 0)
		return;

	if (m_nCycleIndex == -1)
	{
		//cycle change -> fisrt loop -> do notihg...
	}
	else
	{
		//cycle running -> not fisrt loop -> ++

		if (m_nCycleIndex != nCurrnetLoop)
		{
			pCycle->SetTotalCycleLoop(pCycle->GetTotalCycleLoop() + 1);

			return;
		}
	}

	if (true == bCycleChange)
	{
		//cycle change -> ++

		pCycle->SetTotalCycleLoop(pCycle->GetTotalCycleLoop() + 1);
	}
}

void	CChannel::FinalUpdate(CCommData* pCommdata)
{
	//copy channel to commdata
	//use immediately before queue(commdata)

	//total loop count
	CCycle* pCycle = m_pMgr->GetAtCycle(GetCycleNumber());

	if (pCommdata != NULL && pCycle != NULL)
	{
		pCommdata->SetTotalCycleLoop(pCycle->GetTotalCycleLoop());
		pCommdata->SetTotalCount(GetRunningCount());
	}

	//�ʿ�� �߰�
}

void CChannel::SetSOHTargetStep(CStep* pStepTarget)
{
	m_pStepTarget = pStepTarget;
}

CString CChannel::GetChamberChannel()
{
	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return "";

	std::vector<SDevice*> vecValue;
	pMgr->GetDeivceFromType(eDeviceTypeChamber, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			CString strCyclerChannelNumber;
			strCyclerChannelNumber.Format("%d", GetChannelNumber());

			if (lpDevice->strChamberChannel.Find(strCyclerChannelNumber) >= 0)
			{
				m_strChamberChannel = lpDevice->strChamberChannel;

				return m_strChamberChannel;
			}
		}

		++iter;
	}

	return m_strChamberChannel;
}

std::vector<SViewData>	CChannel::GetViewData()
{
	m_vecViewData.clear();

	CMgrCommData* pMgr = CMgrCommData::GetMgr();

	if (!pMgr)
		return m_vecViewData;

	CCommData* lpCyclerInfo = pMgr->GetAt(m_nChannelNumber);

	if (!lpCyclerInfo)
		return m_vecViewData;

	SViewData ViewData;

	// 0. ä�� ��ȣ
	_tcscpy_s(ViewData.szName, "Ch");
	_stprintf_s(ViewData.szData, "CH %d", m_nChannelNumber + 1);
	m_vecViewData.push_back(ViewData);

	// 1. ����
	_tcscpy_s(ViewData.szName, "Status");
	_stprintf_s(ViewData.szData, "%s", (LPCSTR)GetChannelStatusStr());
	m_vecViewData.push_back(ViewData);

	// 2. ���
	_tcscpy_s(ViewData.szName, "Mode");
	_tcscpy_s(ViewData.szData, StepType2String());
	m_vecViewData.push_back(ViewData);

	// 3. �����ID
	_tcscpy_s(ViewData.szName, "User");

	CMgrAccount* pMgrAccount = CMgrAccount::GetMgr();

	if (pMgrAccount)
		_tcscpy_s(ViewData.szData, pMgrAccount->GetLoginID());

	m_vecViewData.push_back(ViewData);

	// 4. OP Code
	_tcscpy_s(ViewData.szName, "OpCode");
	_tcscpy_s(ViewData.szData, OpCode2String());
	m_vecViewData.push_back(ViewData);

	// 5. ���� ����
	_tcscpy_s(ViewData.szName, "EndReason");
	_tcscpy_s(ViewData.szData, m_strEndCondition);
	m_vecViewData.push_back(ViewData);

	// 6. ����
	_tcscpy_s(ViewData.szName, "Voltage");
	_stprintf_s(ViewData.szData, _T("%.3f"), lpCyclerInfo->GetVoltage());
	m_vecViewData.push_back(ViewData);

	// 7. ����
	_tcscpy_s(ViewData.szName, "Current");
	_stprintf_s(ViewData.szData, _T("%.3f"), lpCyclerInfo->GetAmpere());
	m_vecViewData.push_back(ViewData);

	// 8. ����
	_tcscpy_s(ViewData.szName, "Watt");
	_stprintf_s(ViewData.szData, _T("%.3f"), lpCyclerInfo->GetPower());
	m_vecViewData.push_back(ViewData);

	// Watt Hour �߰� - Ah * V = Wh
	_tcscpy_s(ViewData.szName, "WattHour");
	if (GetExternPatternStatus() == true)
		_stprintf_s(ViewData.szData, _T("%.3f"), lpCyclerInfo->GetPatternWattHour());
	else
		_stprintf_s(ViewData.szData, _T("%.3f"), lpCyclerInfo->GetStepWattHour()/* lpCyclerInfo->GetStepAmpereHour() * lpCyclerInfo->GetVoltage()*/);
	m_vecViewData.push_back(ViewData);

	// 9. Ah
	_tcscpy_s(ViewData.szName, "Ah");
	if (GetExternPatternStatus() == true)
		_stprintf_s(ViewData.szData, _T("%.3f"), lpCyclerInfo->GetPatternTotalAmpereHour());
	else
		_stprintf_s(ViewData.szData, _T("%.3f"), lpCyclerInfo->GetStepAmpereHour());

	m_vecViewData.push_back(ViewData);

	// 10. ���� �ð�
	_tcscpy_s(ViewData.szName, "StepTime");
	_tcscpy_s(ViewData.szData, Int2Time(lpCyclerInfo->GetStepTime()));
	m_vecViewData.push_back(ViewData);

	// 11. �� �ð�
	_tcscpy_s(ViewData.szName, "TotalTime");
	_tcscpy_s(ViewData.szData, Int2Time(lpCyclerInfo->GetSchTotalTime()));
	m_vecViewData.push_back(ViewData);

	// 12. è�� �µ�
	_tcscpy_s(ViewData.szName, "ChamberValue");
	_stprintf_s(ViewData.szData, _T("%.3f"), lpCyclerInfo->GetChamberTemperature());
	m_vecViewData.push_back(ViewData);

	// 13. è�� ����
	_tcscpy_s(ViewData.szName, "ChamberSetting");
	_stprintf_s(ViewData.szData, _T("%.3f"), lpCyclerInfo->GetChamberTemperatureSP());
	m_vecViewData.push_back(ViewData);

	// 13. Cycle Count
	_tcscpy_s(ViewData.szName, "CycleCount");
	CCycle* pCycle = m_pMgr->GetAtCycle(m_nCycleNumber);
	if (pCycle)
	{
		int nStartNo = pCycle->GetCycleLoopStartNo();
		_stprintf_s(ViewData.szData, _T("%d"), nStartNo + m_nCycleIndex);
	}
	else
	{
		_stprintf_s(ViewData.szData, _T("%d"), m_nCycleIndex + 1);
	}
	m_vecViewData.push_back(ViewData);

	// 14. Cycle Number
	_tcscpy_s(ViewData.szName, "CycleNumber");
	_stprintf_s(ViewData.szData, _T("%d"), m_nCycleNumber);
	m_vecViewData.push_back(ViewData);

	// 15. Step Number
	_tcscpy_s(ViewData.szName, "StepNumber");
	_stprintf_s(ViewData.szData, _T("%d"), m_nStepNumber);
	m_vecViewData.push_back(ViewData);

	// 16. Schedule File Path
	_tcscpy_s(ViewData.szName, "ScheduleFilePath");
	_stprintf_s(ViewData.szData, "%s", (LPCSTR)GetFileName());
	m_vecViewData.push_back(ViewData);


	// 17. Schedule Name
	_tcscpy_s(ViewData.szName, "ScheduleName");
	_tcscpy_s(ViewData.szData, m_strScheduleName);
	m_vecViewData.push_back(ViewData);

	// 18. ���
	_tcscpy_s(ViewData.szName, "Rating");
	_tcscpy_s(ViewData.szData, GetRating());
	m_vecViewData.push_back(ViewData);

	// 19. SOC
	_tcscpy_s(ViewData.szName, "SOC");
	_stprintf_s(ViewData.szData, "%d", 0);
	m_vecViewData.push_back(ViewData);

	// 20. SOH
	_tcscpy_s(ViewData.szName, "SOH");
	_stprintf_s(ViewData.szData, "%d", 0);
	m_vecViewData.push_back(ViewData);

	// 21. DAQ Temperature
	// 2021-02-08 ����.
	//_tcscpy_s(ViewData.szName, "DAQTemp");
	//CMgrDAQ* pMgrDaQ = CMgrDAQ::GetMgr();
	//if (pMgrDaQ)
	//	_stprintf_s(ViewData.szData, _T("%.3f"), pMgrDaQ->GetDaQMainTemperature(GetChannelNumber()));
	//m_vecViewData.push_back(ViewData);

	// 22. Sample Name
	_tcscpy_s(ViewData.szName, "SampleName");
	//_tcscpy_s(ViewData.szData, m_strSampleName);
	_tcscpy_s(ViewData.szData, m_strWorkName);
	m_vecViewData.push_back(ViewData);

	// 23. Description
	_tcscpy_s(ViewData.szName, "Description");
	_tcscpy_s(ViewData.szData, m_strDescription);
	m_vecViewData.push_back(ViewData);

	return m_vecViewData;
}

bool	CChannel::IsReserved()
{
	if (true == IsReservedPause() || true == IsReservedComplete())
		return true;

	return false;
}

bool	CChannel::IsMultiSchedule()
{
	if (m_vecMultiSchedule.size() > 0)
		return true;

	return false;
}

bool	CChannel::IsReservedPause()
{
	if (m_nReservePauseCycleNumber > 0 && m_nReservePauseStepNumber > 0)
		return true;

	return false;
}

bool	CChannel::IsReservedComplete()
{
	if (m_nReserveCompleteCycleNumber > 0 && m_nReserveCompleteStepNumer > 0)
		return true;

	return false;
}

CString	CChannel::StepType2String()
{
	CString str;

	switch (m_nScheduleMode)
	{
	case eIFBoardStepTypeRest: str.Format(_T("Rest"));			break;
	case eIFBoardStepTypeCharge: str.Format(_T("Charge"));			break;
	case eIFBoardStepTypeDischarge: str.Format(_T("Discharge"));		break;
	case eIFBoardStepTypeExternalPattern: str.Format(_T("Pattern"));		break;
	case elFBoradStepModeMasterAutocalCurrent: str.Format(_T("AutoCal_Cur"));	break;	// AUTO_CALIBRATION
	case elFBoradStepModeAutocalVoltage: str.Format(_T("AutoCal_Vol"));	break;	// AUTO_CALIBRATION
	}

	return str;
}

CString CChannel::OpCode2String()
{
	int nCode = m_nOperationMode;

	CString strProc;
	strProc.Empty();

	switch (nCode)
	{
	case eIFBoardOpCodeUnknown:			strProc.Format(_T("RECV_NONE"));		break;
	case eIFBoardOpCodeInputRelayOn:	strProc.Format(_T("INTPUT READY"));	break;
	case eIFBoardOpCodeOutputReplyOn:	strProc.Format(_T("REST")); break;
	case eIFBoardOpCodeChargeCC:		strProc.Format(_T("CHARGE CC"));   break;
	case eIFBoardOpCodeChargeCV:		strProc.Format(_T("CHARGE CC/CV")); break;
	case eIFBoardOpCodeDischargeCC:		strProc.Format(_T("DISCHARGE CC"));	break;
	case eIFBoardOpCodeDischargeCV:		strProc.Format(_T("DISCHARGE CC/CV")); break;
	case eIFBoardOpCodeChargeCP:		strProc.Format(_T("CHARGE CP"));	break;
	case eIFBoardOpCodeDischargeCP:		strProc.Format(_T("DISCHARGE CP"));	break;
	}

	return strProc.GetString();
}

void	CChannel::GetReserveInfo(std::vector<CString>& vecValue)
{
	vecValue.clear();

	{
		CString strValue;
		strValue.Format("Channel%d", m_nChannelNumber + 1);

		vecValue.push_back(strValue);
	}

	{
		CString strValue = "Not reserved";

		if (true == IsReservedPause())
			strValue = "Reserved";

		vecValue.push_back(strValue);
	}

	{
		CString strValue;
		strValue.Format("%d", m_nReservePauseCycleNumber);

		vecValue.push_back(strValue);
	}

	{
		CString strValue;
		strValue.Format("%d", m_nReservePauseStepNumber);

		vecValue.push_back(strValue);
	}

	{
		CString strValue = "Not reserved";

		if (true == IsReservedComplete())
			strValue = "Reserved";

		vecValue.push_back(strValue);
	}

	{
		CString strValue;
		strValue.Format("%d", m_nReserveCompleteCycleNumber);

		vecValue.push_back(strValue);
	}

	{
		CString strValue;
		strValue.Format("%d", m_nReserveCompleteStepNumer);

		vecValue.push_back(strValue);
	}
}

void	CChannel::GetCyclerChannelInfo(std::vector<CString>& vecValue)
{
	vecValue.clear();

	{
		CString strValue;
		strValue.Format("Channel%d", m_nChannelNumber + 1);

		vecValue.push_back(strValue);
	}

	{
		CString strValue;
		strValue.Format("%d", (int)m_vecMultiSchedule.size());

		vecValue.push_back(strValue);
	}
}

void	CChannel::GetViewPatternInfo(std::vector<SViewSchedule>& vecSchedule)
{
	vecSchedule.clear();

	CMgrCommData* pMgr = CMgrCommData::GetMgr();

	CCommData* lpCyclerInfo = pMgr->GetAt(m_nChannelNumber);

	if (!lpCyclerInfo)
		return;

	if (m_pMgr)
	{
		CList< CStep*, CStep* >& listStep = m_pMgr->GetStepList();

		auto Pos = listStep.GetHeadPosition();

		while (Pos)
		{
			CStep* lpStep = listStep.GetNext(Pos);

			if (!lpStep)
				continue;

			int nCyclerChannelState = eCyclerChannelRunStateUnknown;

			if (lpStep->GetStepIndex() == lpCyclerInfo->GetStepIndex())
				nCyclerChannelState = eCyclerChannelRunStateRun;
			else if (lpStep->GetStepIndex() > lpCyclerInfo->GetStepIndex())
				nCyclerChannelState = eCyclerChannelRunStateComplete;
			else if (lpStep->GetStepIndex() > lpCyclerInfo->GetStepIndex())
				nCyclerChannelState = eCyclerChannelRunStateWait;

			SViewSchedule kParser;

			{
				// 1. ����Ŭ
				CString strValue;
				strValue.Format("%d", lpStep->GetCycleNumber());
				kParser.Add(strValue);
			}

			{
				// 2. ����
				CString strValue;
				strValue.Format("%d", lpStep->GetStepNumber());
				kParser.Add(strValue);
			}

			{
				// 3. Ÿ��
				CString strValue;
				strValue.Format("%s", (LPCSTR)lpStep->StepType2String());
				kParser.Add(strValue);
			}

			{
				// 4. ���
				CString strValue;
				strValue.Format("%s", (LPCSTR)lpStep->StepMode2Str());
				kParser.Add(strValue);
			}

			{
				// 5. ����

				CString strValue;

				switch (nCyclerChannelState)
				{
				case eCyclerChannelRunStateRun: strValue = "Proceeding";	break;
				case eCyclerChannelRunStateComplete: strValue = "complete";	break;
				case eCyclerChannelRunStateWait: strValue = "Waiting";	break;
				}

				kParser.Add(strValue);
			}

			{
				// 6. ���� ����
				CString strValue;
				//lyw9_1 : system safety 
				if (m_nSystemSafetyReason != 0)
				{
					strValue.Format("%s", (LPCSTR)lpStep->StepEndReason2String(m_nSystemSafetyReason));
				}
				else
				{
					strValue.Format("%s", (LPCSTR)lpStep->StepEndReason2String());
				}
				//lyw3_1
				kParser.Add(strValue);
			}

			{
				// 7. ���� ����
				CString strValue;
				strValue.Format("%.3f", lpStep->GetChargeVoltage());
				kParser.Add(strValue);
			}

			{
				// 8. ���� ����
				CString strValue;
				strValue.Format("%.3f", lpStep->GetDisChargeVoltage());
				kParser.Add(strValue);
			}

			{
				// 9. ���� ����
				CString strValue;

				if (eCyclerChannelRunStateRun == nCyclerChannelState)
					strValue.Format("%.3f", lpCyclerInfo->GetVoltage());

				kParser.Add(strValue);
			}

			{
				// 10. ���� ����
				CString strValue;

				if (lpStep->GetStepType() == eIFBoardStepTypeCharge)
					strValue.Format("%.3f", lpStep->GetCurrentAmpere());

				kParser.Add(strValue);
			}

			{
				// 11. ���� ����
				CString strValue;

				if (lpStep->GetStepType() == eIFBoardStepTypeDischarge)
					strValue.Format("%.3f", lpStep->GetCurrentAmpere());

				kParser.Add(strValue);
			}

			{
				// 12. ���� ����
				CString strValue;

				if (eCyclerChannelRunStateRun == nCyclerChannelState)
					strValue.Format("%.3f", lpCyclerInfo->GetAmpere());

				kParser.Add(strValue);
			}

			{
				// 13. ����
				CString strValue;
				strValue.Format("%.3f", lpStep->GetWatt());
				kParser.Add(strValue);
			}

			{
				// 14. ���� ����
				CString strValue;

				if (eCyclerChannelRunStateRun == nCyclerChannelState)
					strValue.Format("%.3f", lpCyclerInfo->GetPower());

				kParser.Add(strValue);
			}

			{
				// 15. ���� ���ǵ�
				CString strValue;
				strValue.Format("%s", (LPCSTR)lpStep->StepCompleteCondition2String());

				kParser.Add(strValue);
			}

			vecSchedule.push_back(kParser);
		}
	}
}

void	CChannel::RemoveMultiSchedule(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_vecMultiSchedule.size())
		return;

	SMultiSchedule* lpMultiSchedule = m_vecMultiSchedule[nIndex];

	if (lpMultiSchedule)
	{
		delete lpMultiSchedule;
		lpMultiSchedule = nullptr;
	}

	auto iter = m_vecMultiSchedule.begin() + nIndex;

	m_vecMultiSchedule.erase(iter);
}

//Channel ���°� ��������
//CDlgViewEqpChannelStatus::UpdateEqpChannelStatus()
//
void CChannel::ChannelPack2Array(__out CStringArray& arr, CStringArray& arrRef)
{
	CMgrCommData* pMgr = CMgrCommData::GetMgr();
	if (!pMgr)
		return;
	CCommData* pComm = pMgr->GetAt(m_nChannelNumber);

	CString str;

	for (int i = 0; i < arrRef.GetSize(); ++i)
	{
		// 0. DAQ : FORMATION
		if (arrRef.GetAt(i).CompareNoCase(_T("Ch")) == 0)
		{
			str.Empty();
			str.Format(_T("DAQ %d"), m_nChannelNumber + 1);
			arr.Add(str);
		}
		// 1. ä��
		//if (arrRef.GetAt(i).CompareNoCase(_T("Ch")) == 0)
		else if (arrRef.GetAt(i).CompareNoCase(_T("Ch")) == 0)
		{
			str.Empty();
			str.Format(_T("Ch %d"), m_nChannelNumber + 1);
			arr.Add(str);
		}
		// 2. ����
		else if (arrRef.GetAt(i).CompareNoCase(_T("Equip Ch")) == 0)
		{
			str.Empty();
			str.Format(_T("%s-%s-%d"), CMgrConfig::GetMgr()->GetCyclerEQPID(), m_fnGetChamberName(m_nChannelNumber), m_nChannelNumber + 1);
			arr.Add(str);
		}
		// 2. ����
		else if (arrRef.GetAt(i).CompareNoCase(_T("Status")) == 0)
		{
			str.Empty();
			str.Format(_T("%s"), GetChannelStatusStr());
			arr.Add(str);
		}
		// 3. ����Ÿ��
		else if (arrRef.GetAt(i).CompareNoCase(_T("Mode")) == 0)
		{
			str.Empty();
			str.Format(_T("%s"), StepType2String());
			arr.Add(str);
		}
		// 4. DSP Ŀ�ǵ�
		else if (arrRef.GetAt(i).CompareNoCase(_T("OpCode")) == 0)
		{
			str.Empty();
			str.Format(_T("%s"), OpCode2String());
			arr.Add(str);

		}
		// 6. ����(V)
		else if (arrRef.GetAt(i).CompareNoCase(_T("User")) == 0)
		{
			str.Empty();
			CMgrAccount* pMgrAccount = CMgrAccount::GetMgr();
			if (pMgrAccount)
				str.Format(_T("%s"), pMgrAccount->GetLoginID());
			arr.Add(str);
		}
		else if (arrRef.GetAt(i).CompareNoCase(_T("EndReason")) == 0)
		{
			str.Empty();
			str.Format("%s", m_strEndCondition);
			arr.Add(str);
		}
		else if (arrRef.GetAt(i).CompareNoCase(_T("Voltage")) == 0)
		{
			if (EQModeParallel == m_eEQModeSingle)
			{
				if (ParallelMaster == m_eMasterChannel)
				{
					if (pComm)
					{
						if (VoltageExpressionUnitV == CMgrOption::GetMgr()->GetVoltageExpUnit())
						{
							str.Format(_T("%.3f"), pComm->GetVoltage());
						}
						else
						{
							str.Format(_T("%.3f"), pComm->GetVoltage() * 0.001f);
						}
					}
				}
			}
			else
			{
				if (pComm)
				{
					if (VoltageExpressionUnitV == CMgrOption::GetMgr()->GetVoltageExpUnit())
					{
						str.Format(_T("%.3f"), pComm->GetVoltage());
					}
					else
					{
						str.Format(_T("%.3f"), pComm->GetVoltage() * 0.001f);
					}
				}
			}

			arr.Add(str);
		}
		// 7. ����(A)
		else if (arrRef.GetAt(i).CompareNoCase(_T("Current")) == 0)
		{
			str.Empty();

			if (EQModeParallel == m_eEQModeSingle)
			{
				if (ParallelMaster == m_eMasterChannel)
				{
					if (pComm)
					{
						if (AmpereExpressionUnitA == CMgrOption::GetMgr()->GetAmpereExpUnit())
						{
							str.Format(_T("%.3f"), pComm->GetAmpere());
						}
						else
						{
							str.Format(_T("%.3f"), pComm->GetAmpere() * 0.001f);
						}
					}
				}
			}
			else
			{
				if (pComm)
				{
					if (AmpereExpressionUnitA == CMgrOption::GetMgr()->GetAmpereExpUnit())
					{
						str.Format(_T("%.3f"), pComm->GetAmpere());
					}
					else
					{
						str.Format(_T("%.3f"), pComm->GetAmpere() * 0.001f);
					}
				}
			}

			arr.Add(str);
		}
		// 8. ����(W)
		else if (arrRef.GetAt(i).CompareNoCase(_T("Watt")) == 0)
		{
			str.Empty();
			if (pComm)
				str.Format(_T("%.3f"), pComm->GetPower());

			arr.Add(str);
		}
		// 9. WattHour
		else if (arrRef.GetAt(i).CompareNoCase(_T("Wh")) == 0)
		{
			str.Empty();
			if (GetExternPatternStatus() == true)
				str.Format(_T("%.3f"), pComm->GetPatternWattHour());
			if (pComm)
				str.Format(_T("%.3f"), pComm->GetStepWattHour());
			arr.Add(str);
		}
		// 10. �뷮(AH)
		else if (arrRef.GetAt(i).CompareNoCase(_T("Ah")) == 0)
		{
			str.Empty();
			if (GetExternPatternStatus() == true)
				str.Format(_T("%.3f"), pComm->GetPatternTotalAmpereHour());
			else
				str.Format(_T("%.3f"), pComm->GetStepAmpereHour());
			arr.Add(str);
		}
		// 11. Step �ð�
		else if (arrRef.GetAt(i).CompareNoCase(_T("StepTime")) == 0)
		{
			str.Empty();
			str = Int2Time(pComm->GetStepTime());
			arr.Add(str);
		}
		// 12. �� �ð�
		else if (arrRef.GetAt(i).CompareNoCase(_T("TotalTime")) == 0)
		{
			str.Empty();
			str = Int2Time(pComm->GetSchTotalTime());
			arr.Add(str);
		}
		// 13. è�� �µ�
		else if (arrRef.GetAt(i).CompareNoCase(_T("ChamberValue")) == 0)
		{
			str.Empty();
			str.Format(_T("%.1f"), pComm->GetChamberTemperature());
			arr.Add(str);
		}
		// 14. �µ� PV
		else if (arrRef.GetAt(i).CompareNoCase(_T("ChamberSetting")) == 0)
		{
			str.Empty();
			str.Format(_T("%.1f"), pComm->GetChamberTemperatureSP());
			arr.Add(str);
		}
		// 15. Cycle Count
		else if (arrRef.GetAt(i).CompareNoCase(_T("CycleCount")) == 0 || arrRef.GetAt(i).CompareNoCase(_T("Loop")) == 0)
		{
			str.Empty();
			CCycle* pCycle = m_pMgr->GetAtCycle(m_nCycleNumber);
			if (pCycle)
			{
				int nStartNo = pCycle->GetCycleLoopStartNo();
				str.Format(_T("%d"), nStartNo + m_nCycleIndex);
			}
			else
			{
				str.Format(_T("%d"), m_nCycleIndex + 1);
			}
			str.Format(_T("%d"), m_nCycleIndex + 1);
			arr.Add(str);
		}
		// 16. Cycle No
		else if (arrRef.GetAt(i).CompareNoCase(_T("CycleNumber")) == 0 || arrRef.GetAt(i).CompareNoCase(_T("Cycle")) == 0)
		{
			str.Empty();
			str.Format(_T("%d"), m_nCycleNumber);
			arr.Add(str);
		}
		// 17. Step No
		else if (arrRef.GetAt(i).CompareNoCase(_T("StepNumber")) == 0 || (arrRef.GetAt(i).CompareNoCase(_T("Step")) == 0))
		{
			str.Empty();
			str.Format(_T("%d"), m_nStepNumber);
			arr.Add(str);
		}
		// 18. �����
		else if (arrRef.GetAt(i).CompareNoCase(_T("ScheduleFilePath")) == 0)
		{
			str.Empty();
			str.Format("%s", m_strFileName);
			arr.Add(str);
		}
		// 19. ���Ǹ�
		else if (arrRef.GetAt(i).CompareNoCase(_T("ScheduleName")) == 0)
		{
			str.Empty();
			str.Format("%s", m_strScheduleName);
			arr.Add(str);
		}
		// 20. ��������
		else if (arrRef.GetAt(i).CompareNoCase(_T("Rating")) == 0)
		{
			str.Empty();
			str.Format("%s", GetRating());
			arr.Add(str);
		}
		// 21. Master, Slave
		else if (arrRef.GetAt(i).CompareNoCase(_T("equipMode")) == 0)
		{
			str.Empty();
			if (ParallelMaster == IsMasterChannel())
				arr.Add("MASTER");
			else
				arr.Add("SLAVE");

		}
		else
		{
			//24.Null
			str.Empty();
			arr.Add(str);
		}
	}
}

/**
 * .
 *
 * METHOD_NAME : ChannelFormation2Array
 * METHOD_TYPE : -
 * DESCRIPTION : FORMATION
 */
void CChannel::ChannelFormation2Array(__out CStringArray& arr, CStringArray& arrRef)
{
	CMgrCommData* pMgr = CMgrCommData::GetMgr();
	if (!pMgr)
		return;
	CCommData* pComm = pMgr->GetAt(m_nChannelNumber);

	
	// FORMATION
	int nEqpChannel1SetMaxCount;
	if (CMgrConfig::GetMgr()->GetTotalSubEqpCount() > 0)
	{
		// TODO : FORMATION ���� ���� �ʿ�
		nEqpChannel1SetMaxCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;
	}

	CString str;

	for (int i = 0; i < arrRef.GetSize(); ++i)
	{
		// 0. MACHINE : FORMATION
		if (arrRef.GetAt(i).CompareNoCase(_T("Stage")) == 0)
		{
			str.Empty();
			str.Format(_T("Stage %d"), m_nChannelNumber / nEqpChannel1SetMaxCount + 1);
			arr.Add(str);
		}
		// 1. ä��
		//if (arrRef.GetAt(i).CompareNoCase(_T("Ch")) == 0)
		else if (arrRef.GetAt(i).CompareNoCase(_T("Ch")) == 0)
		{
			str.Empty();
			str.Format(_T("Ch %d"), m_nChannelNumber + 1);
			arr.Add(str);
		}
		// 2. ��� ä��
		else if (arrRef.GetAt(i).CompareNoCase(_T("Equip Ch")) == 0)
		{
			str.Empty();
			str.Format(_T("%s-%s-%d"), CMgrConfig::GetMgr()->GetCyclerEQPID(), m_fnGetChamberName(m_nChannelNumber), m_nChannelNumber + 1);
			arr.Add(str);
		}
		// 2. ����
		else if (arrRef.GetAt(i).CompareNoCase(_T("Status")) == 0)
		{
			str.Empty();
			str.Format(_T("%s"), GetChannelStatusStr());
			arr.Add(str);
		}
		// 3. ����Ÿ��
		else if (arrRef.GetAt(i).CompareNoCase(_T("Mode")) == 0)
		{
			str.Empty();
			str.Format(_T("%s"), StepType2String());
			arr.Add(str);
		}
		// 4. DSP Ŀ�ǵ�
		else if (arrRef.GetAt(i).CompareNoCase(_T("OpCode")) == 0)
		{
			str.Empty();
			str.Format(_T("%s"), OpCode2String());
			arr.Add(str);

		}
		// 5. User ����
		else if (arrRef.GetAt(i).CompareNoCase(_T("User")) == 0)
		{
			str.Empty();
			CMgrAccount* pMgrAccount = CMgrAccount::GetMgr();
			if (pMgrAccount)
				str.Format(_T("%s"), pMgrAccount->GetLoginID());
			arr.Add(str);
		}
		// 6. Step ���� ����
		else if (arrRef.GetAt(i).CompareNoCase(_T("EndReason")) == 0)
		{
			str.Empty();
			str.Format("%s", m_strEndCondition);
			arr.Add(str);
		}
		// 7. ���� (V)
		else if (arrRef.GetAt(i).CompareNoCase(_T("Voltage")) == 0)
		{
			if (EQModeParallel == m_eEQModeSingle)
			{
				if (ParallelMaster == m_eMasterChannel)
				{
					if (pComm)
					{
						if (pComm->GetStepTypeStr() == "POGO PIN TEST")
						{
							str.Format(_T("Contact resistance test in progress"));
						}
						else
						{
							if (VoltageExpressionUnitV == CMgrOption::GetMgr()->GetVoltageExpUnit())
							{
								str.Format(_T("%.3f"), pComm->GetVoltage());
							}
							else
							{
								str.Format(_T("%.3f"), pComm->GetVoltage() * 0.001f);
							}
						}
						
					}
				}
			}
			else
			{
				if (pComm)
				{	
					if (pComm->GetStepTypeStr() == "POGO PIN TEST")
					{
							str.Format(_T("Contact resistance test in progress"));
					}
					else
					{
						if (VoltageExpressionUnitV == CMgrOption::GetMgr()->GetVoltageExpUnit())
						{
							str.Format(_T("%.3f"), pComm->GetVoltage());
						}
						else
						{
							str.Format(_T("%.3f"), pComm->GetVoltage() * 0.001f);
						}
					}
				}
			}

			arr.Add(str);
		}
		// 7. ����(A)
		else if (arrRef.GetAt(i).CompareNoCase(_T("Current")) == 0)
		{
			str.Empty();

			if (EQModeParallel == m_eEQModeSingle)
			{
				if (ParallelMaster == m_eMasterChannel)
				{
					if (pComm)
					{
						if (pComm->GetStepTypeStr() == "POGO PIN TEST")
						{
							str.Format(_T("Contact resistance test in progress"));
						}
						else
						{

							if (AmpereExpressionUnitA == CMgrOption::GetMgr()->GetAmpereExpUnit())
							{
								str.Format(_T("%.3f"), pComm->GetAmpere());
							}
							else
							{
								str.Format(_T("%.3f"), pComm->GetAmpere() * 0.001f);
							}
						}
					}
				}
			}
			else
			{
				if (pComm)
				{
					if (pComm)
					{
						if (pComm->GetStepTypeStr() == "POGO PIN TEST")
						{
							str.Format(_T("Contact resistance test in progress"));
						}
						else
						{
							if (AmpereExpressionUnitA == CMgrOption::GetMgr()->GetAmpereExpUnit())
							{
								str.Format(_T("%.3f"), pComm->GetAmpere());
							}
							else
							{
								str.Format(_T("%.3f"), pComm->GetAmpere() * 0.001f);
							}
						}
					}
				}

				arr.Add(str);
			}
		}
		// 8. Temp
		else if (arrRef.GetAt(i).CompareNoCase(_T("Temperature")) == 0)
		{
			str.Empty();

			if (pComm)
			{
				str.Format(_T("%.1f"), pComm->GetTemp());
			}

			arr.Add(str);
		}
		// 9. ����(W)
		else if (arrRef.GetAt(i).CompareNoCase(_T("Watt")) == 0)
		{
			str.Empty();
			if (pComm)
				str.Format(_T("%.3f"), pComm->GetPower());

			arr.Add(str);
		}
		// 10. WattHour
		else if (arrRef.GetAt(i).CompareNoCase(_T("Wh")) == 0)
		{
			str.Empty();
			if (GetExternPatternStatus() == true)
				str.Format(_T("%.3f"), pComm->GetPatternWattHour());
			if (pComm)
				str.Format(_T("%.3f"), pComm->GetStepWattHour());
			arr.Add(str);
		}
		// 11. �뷮(AH)
		else if (arrRef.GetAt(i).CompareNoCase(_T("TotalAh")) == 0)
		{
			str.Empty();

			if (GetExternPatternStatus() == true)
				str.Format(_T("%.3f"), pComm->GetPatternTotalAmpereHour());
			else
				str.Format(_T("%.3f"), pComm->GetScheduleAmpereHour());
			arr.Add(str);
		}
		// 12. Step �ð�
		else if (arrRef.GetAt(i).CompareNoCase(_T("StepTime")) == 0)
		{
			str.Empty();
			str = Int2Time(pComm->GetStepTime());
			arr.Add(str);
		}
		// 13. �� �ð�
		else if (arrRef.GetAt(i).CompareNoCase(_T("TotalTime")) == 0)
		{
			str.Empty();
			str = Int2Time(pComm->GetSchTotalTime());
			arr.Add(str);
		}
		// 14. è�� �µ�
		else if (arrRef.GetAt(i).CompareNoCase(_T("ChamberValue")) == 0)
		{
			str.Empty();
			str.Format(_T("%.1f"), pComm->GetChamberTemperature());
			arr.Add(str);
		}
		// 15. �µ� PV
		else if (arrRef.GetAt(i).CompareNoCase(_T("ChamberSetting")) == 0)
		{
			str.Empty();
			str.Format(_T("%.1f"), pComm->GetChamberTemperatureSP());
			arr.Add(str);
		}
		// 16. Cycle Count
		else if (arrRef.GetAt(i).CompareNoCase(_T("CycleCount")) == 0 || arrRef.GetAt(i).CompareNoCase(_T("Loop")) == 0)
		{
			str.Empty();
			CCycle* pCycle = m_pMgr->GetAtCycle(m_nCycleNumber);
			if (pCycle)
			{
				int nStartNo = pCycle->GetCycleLoopStartNo();
				str.Format(_T("%d"), nStartNo + m_nCycleIndex);
			}
			else
			{
				str.Format(_T("%d"), m_nCycleIndex + 1);
			}
			str.Format(_T("%d"), m_nCycleIndex + 1);
			arr.Add(str);
		}
		// 17. Cycle No
		else if (arrRef.GetAt(i).CompareNoCase(_T("CycleNumber")) == 0 || arrRef.GetAt(i).CompareNoCase(_T("Cycle")) == 0)
		{
			str.Empty();
			str.Format(_T("%d"), m_nCycleNumber);
			arr.Add(str);
		}
		// 18. Step No
		else if (arrRef.GetAt(i).CompareNoCase(_T("StepNumber")) == 0 || (arrRef.GetAt(i).CompareNoCase(_T("Step")) == 0))
		{
			str.Empty();
			str.Format(_T("%d"), m_nStepNumber);
			arr.Add(str);
		}
		// 19. �����
		else if (arrRef.GetAt(i).CompareNoCase(_T("ScheduleFilePath")) == 0)
		{
			str.Empty();
			str.Format("%s", m_strFileName);
			arr.Add(str);
		}
		// 20. ���Ǹ�
		else if (arrRef.GetAt(i).CompareNoCase(_T("ScheduleName")) == 0)
		{
			str.Empty();
			str.Format("%s", m_strScheduleName);
			arr.Add(str);
		}
		// 21. ��������
		else if (arrRef.GetAt(i).CompareNoCase(_T("Rating")) == 0)
		{
			str.Empty();
			str.Format("%s", GetRating());
			arr.Add(str);
		}
		// 22. Master, Slave
		else if (arrRef.GetAt(i).CompareNoCase(_T("equipMode")) == 0)
		{
			str.Empty();
			if (ParallelMaster == IsMasterChannel())
				arr.Add("MASTER");
			else
				arr.Add("SLAVE");

		}
		else
		{
			//24.Null
			str.Empty();
			arr.Add(str);
		}
	}
}

/**
 * .
 *
 * METHOD_NAME : AutoCalibration
 * METHOD_TYPE : -
 * DESCRIPTION : AUTO_CALIBRATION
 */
void CChannel::AutoCalibration(float fVolt, float fAmp, int nStep)
{
	CCalibratorVisaProcess* pCalibratorVisaProcess = CCalibratorVisaProcess::GetMgr();
	if (pCalibratorVisaProcess == nullptr) {
		return;
	}

	// ----------------------------------------------------------------------------
		
	if (this->GetSelected())
	{
		//case : Volt Calibration , Schedule start x
		if (CMgrConfig::GetMgr()->GetMeasureModeNo() == elFBoradStepModeAutocalVoltage)
		{
			CStep* pStep = m_pMgr->GetAtStep(nStep);
			if (m_bIsStepChange == false)
			{
				pCalibratorVisaProcess->SetMeasureModeNo(elFBoradStepModeAutocalVoltage);
				if (pCalibratorVisaProcess->GetMeasureCount() > 3 && pCalibratorVisaProcess->GetMeasureCount() < 8)
				{
					m_iVoltCnt++;								// Curr�� �޸� �������� ��û���� ���´�. 
					m_fFormationVoltage += fVolt;
				}
				else if (pCalibratorVisaProcess->GetMeasureCount() == 0)
				{
					m_fFormationVoltage = 0;
				}

				if (pCalibratorVisaProcess->GetMeasureCount() == 1)
				{	
					CMgrPid* pMgrPid = CMgrPid::GetMgr();

					if (pStep != nullptr) {
						pMgrPid->GetDevice(m_nChannelNumber)->SetAOValue(m_nChannelNumber, pStep->GetChargeVoltage());
					}
				}
			}
		}
		//case : Current Calibration , Schedule start o		
		if (CMgrConfig::GetMgr()->GetMeasureModeNo() == elFBoradStepModeMasterAutocalCurrent)
		{
			if (m_bScheduleStart)
			{
				if (m_bIsStepChange == false)
				{
					pCalibratorVisaProcess->SetMeasureModeNo(elFBoradStepModeMasterAutocalCurrent);

					//if (DMMData->m_iMeasCnt > 3 && DMMData->m_iMeasCnt < 8)
					if (pCalibratorVisaProcess->GetMeasureCount() > 3 && pCalibratorVisaProcess->GetMeasureCount() < 8)
					{
						m_iAmpCnt++;
						m_fFormationAmpere += fAmp;
					}
					else if (pCalibratorVisaProcess->GetMeasureCount() == 0)
					{
						m_fFormationAmpere = 0;
					}
				}
			}
		}
	}		
}


//Master Channel ���� Ȯ��
ParallelMode	CChannel::IsMasterChannel()
{
	return m_eMasterChannel;
}

void	CChannel::SetMasterChannel(ParallelMode nObj)
{
	m_eMasterChannel = nObj;
}

EQMode	CChannel::GetEqMode()
{
	return m_eEQModeSingle;
}

void	CChannel::SetEqMode(EQMode nObj)
{
	m_eEQModeSingle = nObj;
}

bool	CChannel::IsChamberUseInStartedChannel(int nChannelNo)
{
	bool bChamberUseInStartedChannel = false;

	std::vector<SDevice*> vecValue;
	CMgrComm::GetMgr()->GetDeivceFromType(eDeviceTypeChamber, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	CString strChamberChannel = "";

	while (iter != iterE)
	{
		SDevice* pComm = (*iter);
		iter++;
		if (NULL == pComm)
			continue;

		for (int i = 0; i < pComm->arrChannel.GetSize(); ++i)
		{
			int nCompareChannelNo = atoi(pComm->arrChannel.GetAt(i));

			if (nChannelNo == nCompareChannelNo)
			{
				strChamberChannel = pComm->strChamberChannel;
			}
		}
		Sleep(1);
	}

	int		nSubString = 0;
	CStringArray strArrChannelNumbers;

	CPnxUtil::GetMgr()->SplitString(strChamberChannel, ',', strArrChannelNumbers);
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		//���� è���� �ִ� ü�θ� ��.
		for (int i = 0; i < strArrChannelNumbers.GetCount(); i++)
		{
			if (strArrChannelNumbers[i] != lpCyclerChannel->GetChannelNumber())
			{
				continue;
			}
		}

		if (true == lpCyclerChannel->GetScheduleStart() && (true == lpCyclerChannel->GetTempSync()
			|| lpCyclerChannel->GetStepSync()))
		{
			bChamberUseInStartedChannel = true;
		}

		Sleep(1);
	}

	return bChamberUseInStartedChannel;
}

CString	CChannel::m_fnGetChamberName(int nChannelNo)
{
	bool bChamberUseInStartedChannel = false;

	std::vector<SDevice*> vecValue;
	CMgrComm::GetMgr()->GetDeivceFromType(eDeviceTypeChamber, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	CString strChamberName = "";

	while (iter != iterE)
	{
		SDevice* pComm = (*iter);
		iter++;
		if (NULL == pComm)
			continue;
		for (int i = 0; i < pComm->arrChannel.GetSize(); i++)
		{
			if (nChannelNo == atoi(pComm->arrChannel.GetAt(i)))
			{
				strChamberName = pComm->strName;
				break;
			}
		}
	}

	return strChamberName;
}

void	CChannel::SetLastAckTime(ULONGLONG ulTime)
{
	if (GetChannelStatusStr() != _T("NONE") && GetChannelStatusStr() != _T("NOT CONNECTED(SAMPLE)"))
	{
		if (ulTime - m_ulLastAckTime >= 15)
		{
			SetChannelStatus(eCyclerChannelStateCommuicationError);
			SYSLOG(Dbg, "[%dCH] ��� ���� : ���� - %d, ���� - %d", m_nChannelNumber, m_ulLastAckTime, ulTime);
		}
	}

	m_ulLastAckTime = ulTime;
}

void	CChannel::SetBoardTime(ULONGLONG ulTime)
{
	// (m_ulBoardTime != 0 && m_ulBoardTime - 150 > ulTime)
	//SYSLOG(Dbg, "[%dCH] BoardDframe Data ������ : ���� - %d, ���� - %d", m_nChannelNumber, m_ulBoardTime, ulTime);
	//m_ulBoardTime = ulTime;
}

// -------------------------------------------------------------------------------------------------
// PCAN
//
//CString CChannel::GetChannelStatusToString()
//{
//	int nStatus = m_nStatus;
//
//	CString strStatus;
//	strStatus.Empty();
//
//	switch (nStatus)
//	{
//	case ChannelStatusUnknown: strStatus.Format(_T("")); break;
//	case ChannelStatusReady: strStatus.Format(_T("READY")); break;
//	case ChannelStatusStart: strStatus.Format(_T("START")); break;
//	case ChannelStatusComplete: strStatus.Format(_T("COMPLETE")); break;
//	case ChannelStatusError: strStatus.Format(_T("ERROR")); break;
//	case ChannelStatusStop: strStatus.Format(_T("")); break;
//	case ChannelStatusStepStop: strStatus.Format(_T("STEP END->STOP")); break;
//	case ChannelStatusCycleStop: strStatus.Format(_T("CYCLE END->STOP")); break;
//	case ChannelStatusPause: strStatus.Format(_T("PAUSE")); break;
//	case ChannelStatusStepPause: strStatus.Format(_T("STEP END->PAUSE")); break;
//	case ChannelStatusCyclePause: strStatus.Format(_T("CYCLE END->PAUSE")); break;
//	case ChannelStatusSafety: strStatus.Format(_T("SAFETY")); break;
//	case ChannelStatusEMO: strStatus.Format(_T("EMO")); break;
//	case ChannelStatePauseFromStepSync: strStatus.Format(_T("StepSync")); break;
//	case ChannelStatePauseFromTempSync: strStatus.Format(_T("TempSync")); break;
//	}
//
//	return strStatus.GetString();
//}
