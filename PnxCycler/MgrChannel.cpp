#include "stdafx.h"
#include "MgrChannel.h"

#include "PnxUtil.h"
#include "MgrLog.h"
#include "MgrComm.h"
#include "MgrCommData.h"
#include "MgrAlarm.h"
#include "CMgrChamber.h"
#include "MgrPCan.h"
#include "PCanCom.h"
#include "MgrConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMgrChannel::CMgrChannel()
{
	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return;

	std::vector<SDevice*> vecValue;
	pMgr->GetDeivceFromType(eDeviceTypeIFBoard, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			for (int i = 0; i < lpDevice->arrChannel.GetSize(); ++i)
			{
				CChannel* pChannel = new CChannel;

				pChannel->SetChannelNumber(atoi(lpDevice->arrChannel.GetAt(i)));

				m_listChannel.AddTail(pChannel);
			}
		}

		++iter;
	}
}

CMgrChannel::~CMgrChannel()
{
	RemoveAll();
}

CChannel*	CMgrChannel::GetAt(int nIndex)
{
	auto Pos = m_listChannel.FindIndex(nIndex);

	if (Pos)
	{
		return m_listChannel.GetAt(Pos);
	}

	return NULL;
}

CChannel*	CMgrChannel::GetChannel(int nChannelNo)
{
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = m_listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (pChannel->GetChannelNumber() == nChannelNo)
			return pChannel;
	}

	return NULL;
}

CChannel*	CMgrChannel::GetSelectedChannel()
{
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = m_listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (true == pChannel->GetSelected())
		{
			return pChannel;
		}
	}

	return nullptr;
}

int		CMgrChannel::GetSelectCyclerChannelCount()
{
	int nSelectedCyclerChannel = 0;

	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if (true == lpCyclerChannel->GetSelected())
			nSelectedCyclerChannel++;
	}

	return nSelectedCyclerChannel;
}

int		CMgrChannel::GetSize()
{
	return m_listChannel.GetSize();
}


void	CMgrChannel::RemoveAll()
{
	while (!m_listChannel.IsEmpty())
	{
		delete m_listChannel.RemoveHead();
	}		
}

CList< CChannel*, CChannel* >&	CMgrChannel::GetChannelList()
{
	return m_listChannel;
}

BOOL	CMgrChannel::GetChannelSelected()
{
	// ???????? ???????? ?????? TRUE
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = m_listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (true == pChannel->GetSelected())
			return TRUE;
	}

	return FALSE;
}

void	CMgrChannel::SetChannelSelected(CStringArray& arr/*, int nIdx*/)
{
	// ?????? ????
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = m_listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		pChannel->SetSelected(FALSE);
	}

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nChannel = atoi(arr.GetAt(i))/* + nIdx*/;

		CChannel* pChannel = GetAt(nChannel);

		if (NULL == pChannel)
			continue;

		pChannel->SetSelected(TRUE);
	}
}

void	CMgrChannel::SetCyclerChannelSelect(int nCyclerChannel)
{
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		lpCyclerChannel->SetSelected(FALSE);
	}

	{
		CChannel* lpCyclerChannel = GetAt(nCyclerChannel);

		if (lpCyclerChannel)
			lpCyclerChannel->SetSelected(TRUE);
	}	
}

void	CMgrChannel::SendAllChannelCommandFrame(BYTE byCommand)
{
	CList< CChannel*, CChannel* >& listChannel = GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		pChannel->SendCommand(byCommand);
	}
}

void	CMgrChannel::StartSchedule(SStartSchedule& kParser)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
		::PostMessage(pWnd->GetSafeHwnd(), WM_HIDE_SCHEDULE_GUI, 0, 0);

	SYSTEMTIME tmCur;
  	GetLocalTime(&tmCur);

	int nCyclerChannelIndex = 0;

	auto iter = kParser.vecCyclerChannelNumber.begin();
	auto iterE = kParser.vecCyclerChannelNumber.end();
	CString strCh = "";
	while (iter != iterE)
	{
		CChannel* lpCyclerChannel = GetChannel(*iter);

		if (!lpCyclerChannel)
		{
			AfxMessageBox("You cannot proceed with the schedule by selecting the wrong channel Info.");

			return;
		}

		//if (lpCyclerChannel->GetChannelStatusStr() != "Possible to Proceed"&& lpCyclerChannel->GetChannelStatusStr() != "End")  //&&lpCyclerChannel->GetChannelStatusStr() != "End")
		//{			
		//	CString strSubStageName;
		//	if ((lpCyclerChannel->GetChannelNumber() + 1) / 24 == 0)
		//	{
		//		strSubStageName = CMgrConfig::GetMgr()->GetSubEqpInfo().strStage1Name;
		//	}
		//	if ((lpCyclerChannel->GetChannelNumber() + 1) / 24 == 1)
		//	{
		//		strSubStageName = CMgrConfig::GetMgr()->GetSubEqpInfo().strStage2Name;
		//	}
		//	strCh.Format(" %s - Channel%d is not available.",strSubStageName, (lpCyclerChannel->GetChannelNumber() + 1) % 24);
		//	AfxMessageBox(strCh);
		//	return;
		//}

		if (true == lpCyclerChannel->GetScheduleStart())
		{
			AfxMessageBox("The selected channel is already in progress.");

			return;
		}

		if (true == lpCyclerChannel->GetStepSync() || true == lpCyclerChannel->GetTempSync())
		{
			if (true == lpCyclerChannel->IsChamberUseInStartedChannel(lpCyclerChannel->GetChannelNumber()))
			{
				AfxMessageBox("A schedule is already in progress in the same chamber.");

				return;
			}
		}

		++iter;
	}

	iter = kParser.vecCyclerChannelNumber.begin();
	iterE = kParser.vecCyclerChannelNumber.end();

	while (iter != iterE)
	{
		CChannel* lpCyclerChannel = GetChannel(*iter);

		if (!lpCyclerChannel)
			return;

		CMgrLog* pMgrLog = CMgrLog::GetMgr();

		if (!pMgrLog)
			return;

		CString strLogFilePath;

		//???? ?????? Slave ?????? Logging ?? ?????? ????. ?????? ???? ???? ????.
		if (lpCyclerChannel->GetEqMode() == EQModeParallel && lpCyclerChannel->IsMasterChannel() == ParallelSlave)
		{

		}
		else
		{
			if (false == pMgrLog->CreateLogDirectory(lpCyclerChannel->GetChannelNumber(), lpCyclerChannel->GetChBarcodeName(),kParser.strScheduleFilePath, tmCur, kParser.strEQPID, kParser.strScheduleName, strLogFilePath, kParser.strLogFilePath, kParser.strWorkName, kParser.bScheduleName, kParser.bTime))
			{
				AfxMessageBox("Unable to proceed due to failure to create result file directory between schedule progress");

				return;
			}
		}

		CMgrSchedule* lpSchedule = lpCyclerChannel->GetMgr();

		if (!lpSchedule)
			return;

		if (FALSE == lpSchedule->LoadScheduleXml(kParser.strScheduleFilePath))
		{
			AfxMessageBox("Unable to proceed because a problem was detected between loading schedules");

			return;
		}

		lpCyclerChannel->InitalizeReady();
		lpCyclerChannel->SetFileName(kParser.strScheduleFilePath);
		lpCyclerChannel->SetScheduleName(kParser.strScheduleName);
		lpCyclerChannel->SetLogFolderName(strLogFilePath);
		lpCyclerChannel->SetSyncMode(kParser.nSyncMode);
		lpCyclerChannel->SetUserID(kParser.strUserID);
		lpCyclerChannel->SetWorkName(kParser.strWorkName);

		if ((int)kParser.vecCellID.size() > nCyclerChannelIndex)
		{
			lpCyclerChannel->SetCellID(kParser.vecCellID[nCyclerChannelIndex]);
		}
		CMgrCommData* pMgrCommData = CMgrCommData::GetMgr();


		if (pMgrCommData)
		{
			CCommData* lpCyclerInfo = pMgrCommData->GetAt(lpCyclerChannel->GetChannelNumber());

			if (lpCyclerInfo)
			{
				lpCyclerInfo->InitalizeReady();
				lpCyclerInfo->SetLogChannelNumber(lpCyclerChannel->GetChannelNumber());
			}

			lpCyclerInfo->SetSystemSafetyEndReason(0);
		}

		nCyclerChannelIndex++;

		Sleep(10);

		++iter;
	}

	iter = kParser.vecCyclerChannelNumber.begin();
	iterE = kParser.vecCyclerChannelNumber.end();

	
	while (iter != iterE)
	{
		CChannel* lpCyclerChannel = GetChannel(*iter);

		if (!lpCyclerChannel)
			continue; //lyw12_1 : return ?? continue?? ????

		CMgrSchedule* lpScheduleInfo = lpCyclerChannel->GetMgr();

		if (iter == kParser.vecCyclerChannelNumber.begin())
		{
			if(true == lpCyclerChannel->GetTempSync() || true == lpCyclerChannel->GetStepSync())
			{
				lpCyclerChannel->SendChamberControl(kParser.nStartStepNumber, eChamberOn);
				Sleep(200);
				lpCyclerChannel->SendChamberControl(kParser.nStartStepNumber, eChamberSet);
				Sleep(200);
			}
		}

		if (lpScheduleInfo)
		{
			CStep* lpStartReserve = lpScheduleInfo->GetAtStep(kParser.nStartCycleNumber, kParser.nStartStepNumber);

			if (lpStartReserve)
			{
				//lyw12_1 : Setting during packet transmission
				lpCyclerChannel->SetSendDataPacket(true);
				//OutputDebugString(_T("SendStep ????\r\n")); //lyw log

				//In case of packet transmission failure, repeat until normal transmission.
				int nRetryCount=0;
				while (true)
				{
					if (lpCyclerChannel->SendStep(lpStartReserve->GetStepIndex()) == true)
					{
						Sleep(50);
						break;
					}
					nRetryCount++;
					if (nRetryCount >= 3)
						break;
					delayMs(100);
				}
			}
		}

		++iter;
	}

	Sleep(50); //lyw12_1 : additional start delay

	iter = kParser.vecCyclerChannelNumber.begin();
	iterE = kParser.vecCyclerChannelNumber.end();

	while (iter != iterE)
	{
		CChannel* lpCyclerChannel = GetChannel(*iter);

		if (!lpCyclerChannel)
			continue; //lyw12_1 : return ?? continue?? ????

		//Sleep(500); //lyw : ???? ???? ??????
		//lpCyclerChannel->InitStepStart();
		////220125 YGY Test
		//lpCyclerChannel->InitStepStart(1, 0, 0, 0, 0.0, 0.0, 0.0, 0.0);
		lpCyclerChannel->InitStepStart_UnCheck(1, 0, 0, 0, 0.0, 0.0, 0.0, 0.0);

		//lpCyclerChannel->InitStepStart(step_max);


		++iter;
	}

	iter = kParser.vecCyclerChannelNumber.begin();
	iterE = kParser.vecCyclerChannelNumber.end();

	while (iter != iterE)
	{
		CChannel* lpCyclerChannel = GetChannel(*iter);

		if (!lpCyclerChannel)         
			continue; //lyw12_1 : return ?? continue?? ????

		//Sleep(500); //lyw : ???? ???? ??????
		//lpCyclerChannel->InitStepStart();
		////220125 YGY Test
		//lpCyclerChannel->InitStepStart(1, 0, 0, 0, 0.0, 0.0, 0.0, 0.0);
		lpCyclerChannel->InitStepStart_Check(1, 0, 0, 0, 0.0, 0.0, 0.0, 0.0);


		//lyw12_1 : Packet transmission off setting
		lpCyclerChannel->SetSendDataPacket(false);

		++iter;
	}
}

void	CMgrChannel::StartErrorChannelSchedule(int chNum, int cycle, int step, int loop)
{	
	CChannel* lpCyclerChannel = GetChannel(chNum);

	if (!lpCyclerChannel) {
		AfxMessageBox("You cannot proceed with the schedule by selecting the wrong channel information.");
		return;
	}
		
	CMgrSchedule* lpSchedule = lpCyclerChannel->GetMgr();
	if (!lpSchedule) {
		return;
	}
		
	g_nCycleLoopStartSet[cycle - 1] = loop;
	if (FALSE == lpSchedule->LoadScheduleXml(lpCyclerChannel->GetFileName())) {
		AfxMessageBox("Unable to proceed because there was a problem with the loading schedule");
		return;
	}

	//lpCyclerChannel->InitalizeReady();
	//lpCyclerChannel->SetFileName(kParser.strScheduleFilePath);
	//lpCyclerChannel->SetScheduleName(kParser.strScheduleName);
	//lpCyclerChannel->SetLogFolderName(strLogFilePath);
	//lpCyclerChannel->SetSyncMode(kParser.nSyncMode);
	//lpCyclerChannel->SetUserID(kParser.strUserID);
	//lpCyclerChannel->SetWorkName(kParser.strWorkName);

	//if ((int)kParser.vecCellID.size() > nCyclerChannelIndex)
	//{
	//	lpCyclerChannel->SetCellID(kParser.vecCellID[nCyclerChannelIndex]);
	//}

	CMgrCommData* pMgrCommData = CMgrCommData::GetMgr();
	if (pMgrCommData) {
		CCommData* lpCyclerInfo = pMgrCommData->GetAt(lpCyclerChannel->GetChannelNumber());
		if (lpCyclerInfo) {
			lpCyclerInfo->InitalizeReady();
			lpCyclerInfo->SetLogChannelNumber(lpCyclerChannel->GetChannelNumber());
		}
	}
	
	CStep* lpStartReserve = lpSchedule->GetAtStep(cycle, step);
	if (lpStartReserve)	{
		//lyw12_1 : Setting during packet transmission
		lpCyclerChannel->SetSendDataPacket(true);
		//OutputDebugString(_T("SendStep ????\r\n")); //lyw log

		//When packet transmission fails, repeat until normal transmission
		int nRetryCount = 0;
		while (true)
		{
			if (lpCyclerChannel->SendStep(lpStartReserve->GetStepIndex()) == true)
			{
				Sleep(100);
				break;
			}
			nRetryCount++;
			if (nRetryCount >= 3)
				break;
			delayMs(1000);
		}
	}

	//Sleep(500); //lyw : additional start delay
	lpCyclerChannel->InitStepStart();
	//lpCyclerChannel->InitStepStart(step_max);

	//lyw12_1 : Packet transmission off setting
	lpCyclerChannel->SetSendDataPacket(false);
}

void CMgrChannel::StartErrorChannelSchedule(int chNum, int cycle, int step, int loop, int steptime, int cycletime, float StepAh, float StepWh, float CycleAh, float CycleWh)
{
	CChannel* lpCyclerChannel = GetChannel(chNum);

	if (!lpCyclerChannel) {
		AfxMessageBox("You cannot proceed with the schedule by selecting the wrong channel Info.");
		return;
	}

	CMgrSchedule* lpSchedule = lpCyclerChannel->GetMgr();
	if (!lpSchedule) {
		return;
	}

	g_nCycleLoopStartSet[cycle - 1] = loop;
	if (FALSE == lpSchedule->LoadScheduleXml(lpCyclerChannel->GetFileName())) {
		AfxMessageBox("Unable to proceed because there was a problem with the loading schedule");					//?????? ???? off?? ?????????????? GetFileName == NULL
		return;
	}

	//lpCyclerChannel->InitalizeReady();
	//lpCyclerChannel->SetFileName(kParser.strScheduleFilePath);
	//lpCyclerChannel->SetScheduleName(kParser.strScheduleName);
	//lpCyclerChannel->SetLogFolderName(strLogFilePath);
	//lpCyclerChannel->SetSyncMode(kParser.nSyncMode);
	//lpCyclerChannel->SetUserID(kParser.strUserID);
	//lpCyclerChannel->SetWorkName(kParser.strWorkName);

	//if ((int)kParser.vecCellID.size() > nCyclerChannelIndex)
	//{
	//	lpCyclerChannel->SetCellID(kParser.vecCellID[nCyclerChannelIndex]);
	//}

	CMgrCommData* pMgrCommData = CMgrCommData::GetMgr();
	if (pMgrCommData) {
		CCommData* lpCyclerInfo = pMgrCommData->GetAt(lpCyclerChannel->GetChannelNumber());
		if (lpCyclerInfo) {
			//lpCyclerInfo->InitalizeReady();
			lpCyclerInfo->SetLogChannelNumber(lpCyclerChannel->GetChannelNumber());
		}
	}

	CStep* lpStartReserve = lpSchedule->GetAtStep(cycle, step);
	if (lpStartReserve) {
		//lyw12_1 : Setting during packet transmission
		lpCyclerChannel->SetSendDataPacket(true);
		//OutputDebugString(_T("SendStep Function\r\n")); //lyw log

		//When packet transmission fails, repeat until normal transmission
		int nRetryCount = 0;
		while (true)
		{
			if (lpCyclerChannel->SendStep(lpStartReserve->GetStepIndex()) == true)
			{
				Sleep(100);
				break;
			}
			nRetryCount++;
			if (nRetryCount >= 3)
				break;
			delayMs(1000);
		}
	}

	//Sleep(500); //lyw : additional start delay
	lpCyclerChannel->InitStepStart(lpStartReserve->GetStepIndex(), steptime, cycletime, loop, StepAh, StepWh, CycleAh, CycleWh);
	//lpCyclerChannel->InitStepStart(step_max);

	//lyw12_1 : Packet transmission off setting
	lpCyclerChannel->SetSendDataPacket(false);




}

//eScheduleLoadingTypeAppend Mode Case 
//Schedule reservation function
void	CMgrChannel::ScheduleAppend(SStartSchedule& kParser)
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
		::PostMessage(pWnd->GetSafeHwnd(), WM_HIDE_SCHEDULE_GUI, 0, 0);

	int nCyclerChannelIndex = 0;

	auto iter = kParser.vecCyclerChannelNumber.begin();
	auto iterE = kParser.vecCyclerChannelNumber.end();

	while (iter != iterE)
	{
		CChannel* lpCyclerChannel = GetChannel(*iter);

		if (!lpCyclerChannel)
			return;

		SMultiSchedule* lpMultiSchedule = new SMultiSchedule;

		lpMultiSchedule->strScheduleFilePath	= kParser.strScheduleFilePath;
		lpMultiSchedule->strScheduleFileName	= kParser.strScheduleName;
		lpMultiSchedule->strUserID				= kParser.strUserID;
		lpMultiSchedule->strWorkName			= kParser.strWorkName;
		lpMultiSchedule->strEQPID				= kParser.strEQPID;
		lpMultiSchedule->strLogFilePath			= kParser.strLogFilePath;
		lpMultiSchedule->nSyncMode				= kParser.nSyncMode;
		lpMultiSchedule->nStartCycle			= kParser.nStartCycleNumber;
		lpMultiSchedule->nStartStep				= kParser.nStartStepNumber;

		if ((int)kParser.vecCellID.size() > nCyclerChannelIndex)
		{
			lpMultiSchedule->strCellID = kParser.vecCellID[nCyclerChannelIndex];
		}

		nCyclerChannelIndex++;

		lpCyclerChannel->AddMultiSchedule(lpMultiSchedule);

		++iter;
	}
}

void	CMgrChannel::NextStep()
{
	CChannel* pChannel = NULL;

	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		pChannel = m_listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (true == pChannel->GetSelected() && true == pChannel->GetScheduleStart())
		{
			break;
		}
		else
		{
			pChannel = NULL;
		}
	}

	if (pChannel == NULL)
	{
		SYSLOG(Dbg, _T("[ERROR]%s pChannel is Null"), __FUNCTION__);

		return;
	}

	pChannel->ManualNextStep();

	SYSLOG(Dbg, _T("NEXT STEP MOVE"));
}

void	CMgrChannel::MoveStep(int nCycleNumber, int nStepNumber)
{
	auto Pos = m_listChannel.GetHeadPosition();

	CChannel* pChannel = NULL;

	while (Pos)
	{
		pChannel = m_listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		if (true == pChannel->GetSelected() && true == pChannel->GetScheduleStart())
		{
			break;
		}
		else
		{
			pChannel = NULL;
		}
	}

	if (pChannel == NULL)
	{
		SYSLOG(Dbg, _T("[ERROR]%s pChannel is Null"), __FUNCTION__);

		return;
	}

	ENDCOND stEnd;
	stEnd.bEvent = TRUE;
	stEnd.nEndReason = eIFBoardCyclerCodeJump;

	{
		if (stEnd.bEvent == TRUE)
		{
			if (true == pChannel->ManualNextStep(nCycleNumber, nStepNumber))
			{
				pChannel->SetAccurateReason(stEnd.nEndReason);
			}
		}
	}	

	SYSLOG(Dbg, _T("SELECTED STEP MOVE"));
}

void	CMgrChannel::NextStep(CChannel* pChannel)
{
	if (pChannel == NULL)
	{
		SYSLOG(Dbg, _T("%s channel is null"), __FUNCTION__);

		return;
	}

	pChannel->ManualNextStep();

	SYSLOG(Dbg, _T("NEXT STEP MOVE"));
}

void	CMgrChannel::MoveStep(CChannel* pChannel, int nCycleNumber, int nStepNumber)
{
	if (pChannel == NULL)
	{
		SYSLOG(Dbg, _T("%s channel is null"), __FUNCTION__);

		return;
	}

	ENDCOND stEnd;
	stEnd.bEvent = TRUE;
	stEnd.nEndReason = eIFBoardCyclerCodeJump;

	{
		if (stEnd.bEvent == TRUE)
		{
			if (true == pChannel->ManualNextStep(nCycleNumber, nStepNumber))
			{
				pChannel->SetAccurateReason(stEnd.nEndReason);
			}
		}
	}

	SYSLOG(Dbg, _T("SELECTED STEP MOVE"));
}

void	CMgrChannel::SendInitial(int nBoardNumber)
{
	//It should be sent for each channel, but it is currently sending based on the board tied to it.
	//4-channel applied board side 4*4 send 16 times
	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return;

	std::vector<SDevice*> vecValue;
	pMgr->GetDeivceFromType(eDeviceTypeIFBoard, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			if (lpDevice->nIndex == nBoardNumber)
			{
				for (int i = 0; i < lpDevice->arrChannel.GetSize(); ++i)
				{
					int nChannelNumber = atoi(lpDevice->arrChannel.GetAt(i));

					CChannel* lpCyclerChannel = GetChannel(nChannelNumber);

					if (lpCyclerChannel)
					{
						lpCyclerChannel->SendCommand(eIFBoardCommandAlarmClear);

						lpCyclerChannel->SendCommand(eIFBoardCommandSerialMode);
					}						
				}

				break;
			}
		}

		++iter;
	}

	// Modify By JSK - 2020.07.03 ???? ?? ???????? ??????.. ?????? ????????
	/*
	iter = vecValue.begin();
	iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			if (lpDevice->nIndex == nBoardNumber)
			{
				for (int i = 0; i < lpDevice->arrChannel.GetSize(); ++i)
				{
					int nChannelNumber = atoi(lpDevice->arrChannel.GetAt(i));

					CChannel* lpCyclerChannel = GetChannel(nChannelNumber);

					if (lpCyclerChannel)
						lpCyclerChannel->SendCommand(eIFBoardCommandSerialMode);
				}

				break;
			}
		}

		++iter;
	}	
	*/
}

void	CMgrChannel::SetCyclerChannelReservePause(int nCycleNumber, int nStepNumber)
{
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		lpCyclerChannel->SetReservePause(nCycleNumber, nStepNumber);
	}
}

void	CMgrChannel::SetCyclerChannelReserveComplete(int nCycleNumber, int nStepNumber)
{
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		lpCyclerChannel->SetReserveComplete(nCycleNumber, nStepNumber);
	}
}

bool	CMgrChannel::GetCyclerChannelExistStepInfo(int nCycleNumber, int nStepNumber)
{
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if(false == lpCyclerChannel->GetSelected())
			continue;

		CMgrSchedule* lpSchedule = lpCyclerChannel->GetMgr();

		if (!lpSchedule)
			continue;

		CStep* lpStep = lpSchedule->GetAtStep(nCycleNumber, nStepNumber);

		if (!lpStep)
		{
			return false;
		}
	}

	return true;
}

bool	CMgrChannel::IsCyclerChannelRestartEnable()
{
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if (eCyclerChannelStatePause == lpCyclerChannel->GetChannelStatus())
			return true;
	}

	return false;
}

bool	CMgrChannel::IsCyclerChannelRestartEnable(std::vector<SRestartInfo>& vevValue)
{
	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return false;

	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

 		if (eCyclerChannelStatePause != lpCyclerChannel->GetChannelStatus())
 			continue;

		CMgrSchedule* lpScheduleInfo = lpCyclerChannel->GetMgr();

		if(!lpScheduleInfo)
			continue;		


		lpScheduleInfo->LoadScheduleXml(lpCyclerChannel->GetFileName());

		SRestartInfo kParser;

		if (lpCyclerChannel->GetChannelNumber() % 4 == 0)	kParser.nCyclerChannel = 0x01;
		else if (lpCyclerChannel->GetChannelNumber() % 4 == 1) kParser.nCyclerChannel = 0x02;
		else if (lpCyclerChannel->GetChannelNumber() % 4 == 2) kParser.nCyclerChannel = 0x04;
		else if (lpCyclerChannel->GetChannelNumber() % 4 == 3) kParser.nCyclerChannel = 0x08;

		//kParser.nCyclerChannel	= lpCyclerChannel->GetChannelNumber();
		kParser.nRestartStep	= lpScheduleInfo->GetStepIndex(lpCyclerChannel->GetCycleNumber(), lpCyclerChannel->GetStepNumber());
		kParser.nIFBoardNumber	= pMgr->GetBoardNumber(lpCyclerChannel->GetChannelNumber());

		vevValue.push_back(kParser);
	}

	return true;
}

bool	CMgrChannel::IsCyclerChannelStartEnable()
{
	if (false == IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel information.");

		return false;
	}

	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if(false == lpCyclerChannel->GetSelected())
			continue;		

		if (eCyclerChannelStateError == lpCyclerChannel->GetChannelStatus())
		{
			AfxMessageBox("The channel you are trying to schedule has an alarm history and cannot proceed.");

			return false;
		}
			
		if (true == lpCyclerChannel->GetScheduleStart())
		{
			AfxMessageBox("The channel you want to schedule is already in progress.");
			return false;
		}
	}

	return true;
}

bool CMgrChannel::IsCyclerChannelStartEnable(int nStageNo)
{
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;

	if (false == IsCyclerChannelSelected(nStageNo))
	{
		AfxMessageBox("There is no selected channel information.");

		return false;
	}

	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;
		// 0 : 0 ~ 23
		// 1 : 24 ~ 47
		// 2 : 48 ~ 71
		// 3 : 72 ~ 95
		if (lpCyclerChannel->GetChannelNumber() / nStageChCount != nStageNo)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		if (eCyclerChannelStateError == lpCyclerChannel->GetChannelStatus())
		{
			AfxMessageBox("The channel you are trying to schedule has an alarm history and cannot proceed.");

			return false;
		}

		if (true == lpCyclerChannel->GetScheduleStart())
		{
			AfxMessageBox("The channel you want to schedule is already in progress.");
			return false;
		}
	}

	return true;
}

bool	CMgrChannel::IsCyclerChannelScheduleAppendEnable()
{
	if (false == IsCyclerChannelSelected())
	{
		AfxMessageBox("There is no selected channel information.");

		return false;
	}

	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		if (eCyclerChannelStateError == lpCyclerChannel->GetChannelStatus())
		{
			AfxMessageBox("The channel you are trying to schedule has an alarm history and cannot proceed.");

			return false;
		}

		if (false == lpCyclerChannel->GetScheduleStart())
		{
			AfxMessageBox("The channel you are trying to schedule has an alarm history and cannot proceed.");
			return false;
		}
	}

	return true;
}

//bool	CMgrChannel::IsCyclerChannelStarted()
//{
//
//	auto Pos = m_listChannel.GetHeadPosition();
//
//	while (Pos)
//	{
//		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);
//
//		if (!lpCyclerChannel)
//			continue;
//
//		if (false == lpCyclerChannel->GetSelected())
//			continue;
//
//		if (false == lpCyclerChannel->GetScheduleStart())
//			return false;
//	}
//
//	return true;
//}

bool CMgrChannel::IsCyclerChannelStarted(int nStageNo)
{
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;
		// 0 : 0 ~ 23
		// 1 : 24 ~ 47
		// 2 : 48 ~ 71
		// 3 : 72 ~ 95
		if (lpCyclerChannel->GetChannelNumber() / nStageChCount != nStageNo)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		if (false == lpCyclerChannel->GetScheduleStart())
			return false;
	}

	return true;
}

bool CMgrChannel::IsCyclerChannelFinished(int nStageNo)
{
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;

	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;
		// 0 : 0 ~ 23
		// 1 : 24 ~ 47
		// 2 : 48 ~ 71
		// 3 : 72 ~ 95
		if (lpCyclerChannel->GetChannelNumber() / nStageChCount != nStageNo)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		if (false == lpCyclerChannel->GetChannelEnd())
			return false;
	}

	return true;
}

//bool	CMgrChannel::IsCyclerChannelSelected()
//{
//	bool bCyclerChannelSelected = false;
//
//	auto Pos = m_listChannel.GetHeadPosition();
//
//	while (Pos)
//	{
//		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);
//
//		if (!lpCyclerChannel)
//			continue;
//
//		if (false == lpCyclerChannel->GetSelected())
//			continue;
//
//		bCyclerChannelSelected = true;
//	}
//
//	return bCyclerChannelSelected;
//}

bool CMgrChannel::IsCyclerChannelSelected(int nStageNo)
{
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;
	bool bCyclerChannelSelected = false;

	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;
		// 0 : 0 ~ 23
		// 1 : 24 ~ 47
		// 2 : 48 ~ 71
		// 3 : 72 ~ 95
		if (lpCyclerChannel->GetChannelNumber() / nStageChCount != nStageNo)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		bCyclerChannelSelected = true;
	}

	return bCyclerChannelSelected;
}

bool CMgrChannel::IsChannelBarcodeCheck(int nStageNo)
{
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;
	bool bChannelBarcodeCheck = false;

	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;
		// 0 : 0 ~ 23
		// 1 : 24 ~ 47
		// 2 : 48 ~ 71
		// 3 : 72 ~ 95
		if (lpCyclerChannel->GetChannelNumber() / nStageChCount != nStageNo)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		if (true == lpCyclerChannel->GetChBarcodeName().IsEmpty())
		{
			bChannelBarcodeCheck = false;
			break;
		}

		bChannelBarcodeCheck = true;
	}

	return bChannelBarcodeCheck;
}

bool	CMgrChannel::IsCyclerChannelExistAlarm(int nYesNo)
{
	CMgrAlarm* pMgr = CMgrAlarm::GetMgr();
	CMgrPCan* pMgrPCan = CMgrPCan::GetMgr();
	CPCanCom* pPcan = new CPCanCom;

	if (!pMgr)
		return false;

	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;
		
		if (nYesNo == IDNO)
		{
			if (false == lpCyclerChannel->GetSelected())
				continue;
		}

		CAlarmMap* lpAlarmMap = pMgr->GetAlarmMap(lpCyclerChannel->GetChannelNumber());

		if(!lpAlarmMap)
			continue;

		if (lpCyclerChannel->GetChannelStatus() == eCyclerChannelStateError) {

			lpCyclerChannel->SendCommand(eIFBoardCommandAlarmRequest);
			Sleep(100);

			//One of the 4 types of alarms must be turned on to clear the alarm.
			CMgrAlarm* pMgr = CMgrAlarm::GetMgr();
			if (pMgr == NULL)
				continue;

			for (int bitNumber = 0; bitNumber < 16/*AlarmBitMaxCount*/; ++bitNumber) {
				CStringArray arr;
				auto chNum = lpCyclerChannel->GetChannelNumber();
				
				pMgr->GetChannelAlarm(chNum, bitNumber, arr);			
				for (int alarmKind = 1; alarmKind < arr.GetSize(); ++alarmKind)	{
					CString str = arr.GetAt(alarmKind);
					if (!str.IsEmpty()) {
						lpCyclerChannel->SendCommand(eIFBoardCommandAlarmClear);

						Sleep(100);

						lpCyclerChannel->SendCommand(eIFBoardCommandAlarmRequest);

						break;
					}
				}
			}
		}
	/*	if (pMgrPCan->GetAlarmStatus())
		{
			pPcan->SendDisplayCanMegErrorClear();
		}*/


		//g_nAlarmStatus[lpCyclerChannel->GetChannelNumber()] = 0;
		//Sleep(10);
	}

	return true;
}

bool	CMgrChannel::IsCyclerChannelExistReserve(int nCycleNumber, int nStepNumber)
{
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		if (true == lpCyclerChannel->IsReserved())
		{
			if (lpCyclerChannel->GetReserveCompleteCycle() == nCycleNumber && lpCyclerChannel->GetReserveCompleteStep() == nStepNumber)
				return true;

			if (lpCyclerChannel->GetReservePauseCycle() == nCycleNumber && lpCyclerChannel->GetReservePauseStep() == nStepNumber)
				return true;
		}		
	}

	return false;
}

//void	CMgrChannel::SetCyclerChannelPauseOption(int nCommandType, int nCycleNumber, int nStepNumber)
//{
//	auto Pos = m_listChannel.GetHeadPosition();
//
//	while (Pos)
//	{
//		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);
//
//		if (!lpCyclerChannel)
//			continue;
//
//		if (false == lpCyclerChannel->GetSelected())
//			continue;
//
//		switch (nCommandType)
//		{
//		case eCyclerCommandOptionPauseImmediately			: lpCyclerChannel->SendPause(eIFBoardPauseCommandPause); break;
//		case eCyclerCommandOptionPauseAfterStepComplete		: lpCyclerChannel->SetPauseStep(TRUE); break;
//		case eCyclerCommandOptionPauseAfterCycleComplete	: lpCyclerChannel->SetPauseCycle(TRUE); break;	
//		case eCyclerCommandOptionPauseReserved				: lpCyclerChannel->SetReservePause(nCycleNumber, nStepNumber); break;
//		case eCyclerCommandOptionJumpNext					: lpCyclerChannel->ManualNextStep(); break;
//		case eCyclerCommandOptionJumpReserved				: lpCyclerChannel->ManualNextStep(nCycleNumber, nStepNumber); break;
//		case eCyclerCommandOptionCompleteImmediately		: lpCyclerChannel->SendPause(eIFBoardPauseCommandComplete); break; ///*lpCyclerChannel->SendCommand(eIFBoardPauseCommandComplete);*/  //taek ????????
//		case eCyclerCommandOptionCompleteAfterStepComplete	: lpCyclerChannel->SetCompleteStep(TRUE); break;
//		case eCyclerCommandOptionCompleteAfterCycleComplete	: lpCyclerChannel->SetCompleteCycle(TRUE); break;
//		case eCyclerCommandOptionCompleteReserved			: lpCyclerChannel->SetReserveComplete(nCycleNumber, nStepNumber); break;
//		}
//	}
//}

void CMgrChannel::SetCyclerChannelPauseOption(int nCommandType, int nStageNo, int nCycleNumber, int nStepNumber)
{
	auto Pos = m_listChannel.GetHeadPosition();
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;
		// 0 : 0 ~ 23
		// 1 : 24 ~ 47
		// 2 : 48 ~ 71
		// 3 : 72 ~ 95
		if (lpCyclerChannel->GetChannelNumber() / nStageChCount != nStageNo)
			continue;
		if (false == lpCyclerChannel->GetSelected())
			continue;

		switch (nCommandType)
		{
		case eCyclerCommandOptionPauseImmediately: lpCyclerChannel->SendPause(eIFBoardPauseCommandPause); break;
		case eCyclerCommandOptionPauseAfterStepComplete: lpCyclerChannel->SetPauseStep(TRUE); break;
		case eCyclerCommandOptionPauseAfterCycleComplete: lpCyclerChannel->SetPauseCycle(TRUE); break;
		case eCyclerCommandOptionPauseReserved: lpCyclerChannel->SetReservePause(nCycleNumber, nStepNumber); break;
		case eCyclerCommandOptionJumpNext: lpCyclerChannel->ManualNextStep(); break;
		case eCyclerCommandOptionJumpReserved: lpCyclerChannel->ManualNextStep(nCycleNumber, nStepNumber); break;
		case eCyclerCommandOptionCompleteImmediately: lpCyclerChannel->SendPause(eIFBoardPauseCommandComplete); break; ///*lpCyclerChannel->SendCommand(eIFBoardPauseCommandComplete);*/  //taek ????????
		case eCyclerCommandOptionCompleteAfterStepComplete: lpCyclerChannel->SetCompleteStep(TRUE); break;
		case eCyclerCommandOptionCompleteAfterCycleComplete: lpCyclerChannel->SetCompleteCycle(TRUE); break;
		case eCyclerCommandOptionCompleteReserved: lpCyclerChannel->SetReserveComplete(nCycleNumber, nStepNumber); break;
		case eCyclerCommandOptionCalibration_Volt: lpCyclerChannel->SendPause(eIFBoardPauseCommandVoltCalibrationEnd); break;	//220818 YGYUN Calibration End Command (Volt)
		case eCyclerCommandOptionCalibration_MasterCalibration: lpCyclerChannel->SendPause(eIFBoardPauseCommandMasterCurrentCalibrationEnd); break;	//220818 YGYUN Calibration End Command (Master Current)
		case eCyclerCommandOptionCalibration_SlaveCalibration: lpCyclerChannel->SendPause(eIFBoardPauseCommandSlaveCurrentCalibrationEnd); break;	//220818 YGYUN Calibration End Command (Slave Current)

		}
	}
}

void	CMgrChannel::GetSelectedCyclerChannel(std::vector<CString>& vecValue)
{
	vecValue.clear();
	
	auto Pos = m_listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* lpCyclerChannel = m_listChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if (false == lpCyclerChannel->GetSelected())
			continue;

		CString strValue;
		strValue.Format("Channel%d", lpCyclerChannel->GetChannelNumber() + 1);

		vecValue.push_back(strValue);
	}
}

bool	CMgrChannel::ChangeMode(bool bSingle)
{
	auto Pos = m_listChannel.GetHeadPosition();
	CChannel* pChannel = NULL;
	while (Pos)
	{
		pChannel = m_listChannel.GetNext(Pos);
		if (!pChannel)
			continue;
		if (false == pChannel->GetSelected())
			continue;
		if (true == pChannel->GetScheduleStart())
			continue;
	//	pChannel->
	}

	return true;
}

