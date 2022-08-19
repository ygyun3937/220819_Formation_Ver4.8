#include "stdafx.h"
#include "LogThread.h"

#include "MgrLog.h"
#include "MgrDAQ.h"
#include "MgrChannel.h"
#include "MgrConfig.h"

#include "CMgrPid.h"

#include "CCalibratorVisaProcess.h"
#include "CDlgEditScheduleInfo.h"

CLogThread::CLogThread()
{
	m_pThread	= nullptr;
	m_bLoop		= true;

	m_nChannelNumber = 0;

	memset(m_nFileCount, 0, sizeof(m_nFileCount));
}


CLogThread::~CLogThread()
{
	for (int i = 0; i < eResultFileTypeMaxCount; ++i)
	{
		if (m_file[i].m_pStream)
			m_file[i].Close();
	}
}

int		CLogThread::GetChannelNumber()
{
	return m_nChannelNumber;
}

void	CLogThread::SetChannelNumber(int nNumber)
{
	m_nChannelNumber = nNumber;
}

void	CLogThread::StartThread()
{
	if (NULL == m_pThread)
		m_pThread = AfxBeginThread(QueuingThread, this);
}

void	CLogThread::StopThread()
{
	m_bLoop = false;

	if (m_pThread)
	{
		::WaitForSingleObject(m_pThread->m_hThread, INFINITE);

		m_pThread = NULL;
	}

	while (!m_Queue.IsEmpty())
		delete m_Queue.RemoveHead();
}

CCommData*	CLogThread::GetQueue()
{
	m_csLock.Lock();

	CCommData* pQueue = NULL;
	
	if (FALSE == m_Queue.IsEmpty())
		pQueue = m_Queue.RemoveHead();
	
	m_csLock.Unlock();

	return pQueue;
}

void	CLogThread::AddQueue(CCommData* pQueue)
{
	m_csLock.Lock();

	m_Queue.AddTail(pQueue);

	m_csLock.Unlock();
}

void CLogThread::LowCalibration(int nCh, int nStep, CString strFilePath)
{
	float f_DAmp1 = 0.0f;
	float f_DAmp2 = 0.0f;
	float f_DAmp3 = 0.0f;
	float f_DAmp4 = 0.0f;
	float f_CAmp1 = 0.0f;
	float f_CAmp2 = 0.0f;
	float f_CAmp3 = 0.0f;
	float f_CAmp4 = 0.0f;	//D = DMM C = Cycler 
	float f_fTargetAmp1 = 0.0f;
	float f_fTargetAmp2 = 0.0f;
	float fY1 = 0.0f;
	float fY2 = 0.0f;
	float fB1 = 0.0f;
	float fB2 = 0.0f;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCh);
	CStep *pStep = pChannel->GetMgr()->GetAtStep(1);

	if (pStep != nullptr)
	{
		f_DAmp1 = pStep->m_fAvgCalibratorAmpere;
		f_CAmp1 = pStep->m_fAvgFormationAmpere;

		if (pStep->GetChargeCurrent() != 0)
			f_fTargetAmp1 = pStep->GetChargeCurrent();
		else
			f_fTargetAmp1 = pStep->GetDisChargeCurrent();

		pStep = pChannel->GetMgr()->GetAtStep(2);
		if (pStep == NULL)
			return;

		// ---------------------------------------------------------------------------

		f_DAmp2 = pStep->m_fAvgCalibratorAmpere;
		f_CAmp2 = pStep->m_fAvgFormationAmpere;

		if (pStep->GetChargeCurrent() != 0)
			f_fTargetAmp2 = pStep->GetChargeCurrent();
		else
			f_fTargetAmp2 = pStep->GetDisChargeCurrent();

		//기울기 계산 
		float fA1 = (f_DAmp2 - f_DAmp1) / (f_fTargetAmp2 - f_fTargetAmp1);
		fB1 = f_DAmp1 - (fA1 * f_fTargetAmp1);
		fY1 = (fA1 * 0) + fB1;
	}

	// ----------------------------------------------------------------------------

	pStep = pChannel->GetMgr()->GetAtStep(nStep - 1);
	if (pStep != nullptr)
	{
		f_DAmp3 = pStep->m_fAvgCalibratorAmpere;
		f_CAmp3 = pStep->m_fAvgFormationAmpere;
		if (pStep->GetChargeCurrent() != 0)
			f_fTargetAmp1 = pStep->GetChargeCurrent();
		else
			f_fTargetAmp1 = pStep->GetDisChargeCurrent();

		pStep = pChannel->GetMgr()->GetAtStep(nStep);
		if (pStep == NULL)
			return;

		// ---------------------------------------------------------------------------

		f_DAmp4 = pStep->m_fAvgCalibratorAmpere;
		f_CAmp4 = pStep->m_fAvgFormationAmpere;

		if (pStep->GetChargeCurrent() != 0)
			f_fTargetAmp2 = pStep->GetChargeCurrent();
		else
			f_fTargetAmp2 = pStep->GetDisChargeCurrent();

		//기울기 계산 
		float fA2 = (f_DAmp4 - f_DAmp3) / (f_fTargetAmp2 - f_fTargetAmp1);
		fB2 = f_DAmp4 - (fA2 * f_fTargetAmp2);
		fY2 = (fA2 *  (f_fTargetAmp2 + (f_fTargetAmp2 - f_fTargetAmp1))) + fB2;

		CString strData = _T("");	//마지막 전압(ex 100A)밑에  110A 추가해달라고하심.
		strData.Format(_T(" , ,%0.3f, , , ,%0.3f\n, ,0, , , ,%0.3f"), f_fTargetAmp2 + (f_fTargetAmp2 - f_fTargetAmp1), fY2, fY1);
		WriteOpenedFile(strFilePath, _T("\n") + strData, eResultFileTypeCalibrationAverage, nCh, NULL);
	}
}

/**
 * .
 * 
 * METHOD_NAME : StartNextChannelCalibration
 * METHOD_TYPE : -
 * DESCRIPTION : 2021.10.26 AutoCalibration 자동으로 다음채널 교정 시작
 */
void CLogThread::StartNextChannelCalibration()
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (!pMgr)
		return;

	if (m_strEndReason.CompareNoCase(g_lpszIFBoardCode[14]) == 0)
		return;

	//if (pMgr->GetCyclerChannelPauseOption() == eCyclerCommandOptionCompleteImmediately)
	//{
	//	return;			//즉시 완료시 다음채널로 넘어가지 않는다. 
	//}
	
	m_bNextCh = false;

	CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();
	
	auto Pos = listCyclerChannel.GetHeadPosition();
	CChannel* lpCyclerChannel = NULL;
	CChannel* lpNextChannel = NULL;
	while (Pos)
	{
		lpCyclerChannel = listCyclerChannel.GetNext(Pos);

		if (!lpCyclerChannel)
			continue;

		if (Pos == NULL)
			return;

		if (lpCyclerChannel->GetSelected())
		{
			lpCyclerChannel->SetSelected(false);

			lpNextChannel = listCyclerChannel.GetNext(Pos);

			if (lpNextChannel != NULL)
				lpNextChannel->SetSelected(true);

			break;
		}
	}

	//?????
	if (lpNextChannel != NULL && lpNextChannel->GetChannelNumber() % 16 != 0) // 16ch만 cal 할수 있다
	{
		CDlgEditScheduleInfo::GetMgr()->ExecProcScheduleStart();
	}
}

UINT CLogThread::QueuingThread(LPVOID pParam)
{
	CLogThread* pMgr = (CLogThread*)pParam;

	if (NULL == pMgr)
		return 0;

	int chNum = pMgr->GetChannelNumber();

	while (pMgr->m_bLoop)
	{
		CCommData* pLog = pMgr->GetQueue();

		if (NULL == pLog)
		{
			Sleep(1);

			continue;
		}

		// AUTO_CALIBRATION
		//if (pLog->GetLogType() == LOG_TYPE_END)
		if (pLog->GetLogType() == LOG_TYPE_END || pLog->GetLogType() == LOG_TYPE_COMPLETE)
		{
			pMgr->WriteEnd(pLog);

			if (pLog->GetLogType() == LOG_TYPE_COMPLETE)
			{
				Sleep(1);

			/*	if (CMgrConfig::GetMgr()->GetMeasureModeNo() > elFBoradStepModeAutocalVoltage && pLog->GetScheduleComplete() && pMgr->m_bNextCh == true)
				{
					if (CMgrConfig::GetMgr()->GetCalibrationAutoMode())
					{
						pMgr->StartNextChannelCalibration();
						pMgr->m_bNextCh = false;
					}				
				}*/
			}
		}
		else if (pLog->GetLogType() == LOG_TYPE_RUN)
		{
			pMgr->WriteRun(pLog);
		}
		else if (pLog->GetLogType() == LOG_TYPE_CALIBRATION_VOLT)
		{
			pMgr->WriteCalVolt(pLog);
		}


		delete pLog;
		pLog = nullptr;

		Sleep(1);
	}

	return 0;
}

/*
bool	CLogThread::WriteOpenedFile(CString strFilePath, CString strCyclerChannelInfo, int nFileType)
{
	bool bExistFile = false;
#ifdef _UNICODE
	bExistFile = (_waccess_s(strFilePath, 0) != -1);
#else 
	bExistFile = (_access_s(strFilePath, 0) != -1);
#endif // _UNICODE

	if (bExistFile == false)
	{
		if (m_file[nFileType].m_pStream)
		{
			m_file[nFileType].Close();
		}

		if (FALSE == m_file[nFileType].Open(strFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
			return false;

		CMgrLog* pMgr = CMgrLog::GetMgr();

		if (pMgr)
		{
			m_file[nFileType].WriteString(pMgr->GetHeaderLogString() + CMgrDAQ::GetMgr()->GetDaQLogHeader() + "\n");
			m_file[nFileType].WriteString(strCyclerChannelInfo);
			m_file[nFileType].Flush();
		}
	}
	else
	{
		if (!m_file[nFileType].m_pStream)
		{
			if (FALSE == m_file[nFileType].Open(strFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
				return false;

			m_file[nFileType].SeekToEnd();
		}

		m_file[nFileType].WriteString(strCyclerChannelInfo);
		m_file[nFileType].Flush();

		if (eResultFileTypeStepTotal == nFileType)
		{
			if (m_file[nFileType].GetLength() >= 50 * 1024 * 1024)
			{
				m_nFileCount[nFileType] += 1;
			}
		}
	}

	// 스케줄 끝난거 체크 되었으면

	if(this->m_bLoop == false)
	{
		if (m_file[nFileType].m_pStream)
			m_file[nFileType].Close();

		m_nFileCount[nFileType] = 0;			
	}

	return true;	
}
*/

bool	CLogThread::WriteOpenedFile(CString strFilePath, CString strCyclerChannelInfo, int nFileType, int nChannelNumber, CCommData* pCommData)
{
	CFileFind kParser;

	// AUTO_CALIBRATION
	CString tempCurrentValue = _T("0.0");
	CCalibratorVisaProcess* pCalibratorVisaProcess = CCalibratorVisaProcess::GetMgr();
	if (pCalibratorVisaProcess != NULL)
	{
		tempCurrentValue.Format(_T(",%.6f\n"), pCalibratorVisaProcess->GetCalibratorChannelDataCurrent(nChannelNumber));
	}

	if (FALSE == kParser.FindFile(strFilePath))
	{
		if (m_file[nFileType].m_pStream) {
			m_file[nFileType].Close();
		}

		if (FALSE == m_file[nFileType].Open(strFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite)) {
			return false;
		}
		
		CMgrLog* pMgr = CMgrLog::GetMgr();

		if (pMgr)
		{
			if (nFileType == eResultFileTypeCalibrationAverage)
			{
				m_file[nFileType].WriteString(pMgr->GetHeaderLogStringForAutoCalibration() + "\n");
				m_file[nFileType].WriteString(strCyclerChannelInfo);
				m_file[nFileType].Flush();
			}
			else
			{
				m_file[nFileType].WriteString(pMgr->GetHeaderLogString() + CMgrDAQ::GetMgr()->GetLogHeaderDAQ(nChannelNumber)+_T("Calibrator") + "\n");
				m_file[nFileType].WriteString(strCyclerChannelInfo+ tempCurrentValue);
				m_file[nFileType].Flush();
			}
		}

		// 작업 명세서
		CString specificationPath = _T("");
		specificationPath.Format(_T("%s\\Config\\%s"), GetExePath(), "Specification");
		::CreateDirectory(specificationPath, NULL);

		CString temp;
		temp.Format(_T("%s\\CH%02d.ini"), specificationPath, nChannelNumber + 1);
		specificationPath = temp;

		// ---------------------------------------------------------------------------

		/*
		if (nFileType == 0) {
			CChannel* pChannel = CMgrChannel::GetMgr()->GetAt(pCommData->GetChannelNumber());
			CMgrSchedule* lpSchedule = pChannel->GetMgr();

			INIWriteInt("Specification", "CycleNumber", pCommData->GetCycleNumber(), specificationPath);

			CCycle* pCycle = lpSchedule->GetAtCycle(pCommData->GetCycleNumber());
			int cycleIndex = 0;
			if (pCycle) {
				cycleIndex = pCycle->GetCycleLoopStartNo() + pCommData->GetCycleIndex();
			}
			else {
				cycleIndex = pCommData->GetCycleIndex() + 1;
			}

			INIWriteInt("Specification", "CycleIndex", cycleIndex, specificationPath);
			INIWriteInt("Specification", "StepNumber", pCommData->GetStepNumber(), specificationPath);
			INIWriteInt("Specification", "StepIndex", pCommData->GetStepIndex(), specificationPath);

			
			INIWriteStr("Specification", "EndCondition", "", specificationPath);
		}
		*/

		//INIWriteInt("Specification", "StepTime", pCommData->GetStepTime(), specificationPath);

		// ---------------------------------------------------------------------------
	}
	else
	{
		kParser.Close();

		if (!m_file[nFileType].m_pStream) {
			if (FALSE == m_file[nFileType].Open(strFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite))
				return false;
			
			m_file[nFileType].SeekToEnd();
		}

		//m_file[nFileType].SeekToEnd();

		// AUTO_CALIBRATION
		if (eResultFileTypeCalibrationAverage == nFileType)
		{
			m_file[nFileType].WriteString(strCyclerChannelInfo);
		}
		else
		{
			m_file[nFileType].WriteString(strCyclerChannelInfo + tempCurrentValue);
		}

		m_file[nFileType].Flush();

		if (eResultFileTypeStepTotal == nFileType)
		{
			if (m_file[nFileType].GetLength() >= 50 * 1024 * 1024)
			{
				m_nFileCount[nFileType] += 1;
			}
		}
	}

	// 스케줄 끝난거 체크 되었으면

	if (this->m_bLoop == false)
	{
		if (m_file[nFileType].m_pStream)
			m_file[nFileType].Close();

		m_nFileCount[nFileType] = 0;
	}

	return true;
}


void	CLogThread::WriteRun(CCommData* lpCycleChannelInfo)
{
	CMgrLog* pMgr = CMgrLog::GetMgr();

	if (!pMgr)
		return;

	// AUTO_CALIBRATION
	 // 교정하지 않는 채널은 로그를 남기지 않는다.
	if (CMgrConfig::GetMgr()->GetMeasureMode().CompareNoCase(_T("NONE")) != 0)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(lpCycleChannelInfo->GetChannelNumber());
		//선택 확인 하는 거랑 교정이랑 먼상관? -> 켈 구현 후 켈 진행 여부 조건으로 수정필요(YGY)
		///if (!pChannel->GetSelected())
		//	return;
	}

	CString strCyclerChannelInfo = lpCycleChannelInfo->GetCyclerChannelInfo(false, false);

	CString strFileName;
	strFileName.Format(_T("STEP_C%02d_L%02d_S%02d.log"),
		lpCycleChannelInfo->GetCycleNumber(),
		lpCycleChannelInfo->GetCycleIndex() + 1,
		lpCycleChannelInfo->GetStepNumber());

	CString strFilePath;
	strFilePath.Format("%s%s", (LPCSTR)pMgr->GetCurrentLogPath(lpCycleChannelInfo->GetLogChannelNumber()), (LPCSTR)strFileName);


	WriteOpenedFile(strFilePath, strCyclerChannelInfo, eResultFileTypeStepRun, lpCycleChannelInfo->GetChannelNumber(), lpCycleChannelInfo);

	strFilePath.Format("%sTOTAL_STEP(%d).log", (LPCSTR)pMgr->GetCurrentLogPath(lpCycleChannelInfo->GetLogChannelNumber()), m_nFileCount[eResultFileTypeStepTotal]);

	WriteOpenedFile(strFilePath, strCyclerChannelInfo, eResultFileTypeStepTotal, lpCycleChannelInfo->GetChannelNumber(), lpCycleChannelInfo);

}

void CLogThread::WriteEnd(CCommData* lpCycleChannelInfo)
{
	CMgrLog* pMgr = CMgrLog::GetMgr();
	if (!pMgr)
		return;

	// AUTO_CALIBRATION
	// 교정하지 않는 채널은 로그를 남기지 않는다.
	if (CMgrConfig::GetMgr()->GetMeasureMode().CompareNoCase(_T("NONE")) != 0)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(lpCycleChannelInfo->GetChannelNumber());
		//선택 확인 하는 거랑 교정이랑 먼상관? -> 켈 구현 후 켈 진행 여부 조건으로 수정필요(YGY)
		//if (!pChannel->GetSelected())
		//	return;
	}

	CString strCyclerChannelInfo = lpCycleChannelInfo->GetCyclerChannelInfo(true, false);

	CString strFileName;
	strFileName.Format(_T("END_C%02d_L%02d_S%02d.log"), lpCycleChannelInfo->GetCycleNumber(), lpCycleChannelInfo->GetCycleIndex() + 1, lpCycleChannelInfo->GetStepNumber());

	CString strFilePath;
	strFilePath.Format("%s%s", (LPCSTR)pMgr->GetCurrentLogPath(lpCycleChannelInfo->GetLogChannelNumber()), (LPCSTR)strFileName);

	WriteOpenedFile(strFilePath, strCyclerChannelInfo, eResultFileTypeEndRun, lpCycleChannelInfo->GetChannelNumber(), lpCycleChannelInfo);

	strFilePath.Format("%sTOTAL_END.log", (LPCSTR)pMgr->GetCurrentLogPath(lpCycleChannelInfo->GetLogChannelNumber()));

	WriteOpenedFile(strFilePath, strCyclerChannelInfo, eResultFileTypeEndTotal, lpCycleChannelInfo->GetChannelNumber(), lpCycleChannelInfo);
	//SYSLOG(Dbg, _T("TestEnd__%s"), strCyclerChannelInfo);

	// ---------------------------------------------------------------------------
	// AUTO_CALIBRATION

	CCalibratorVisaProcess* pCalibratorVisaProcess = CCalibratorVisaProcess::GetMgr();

	// 스텝별 측정 평균값 저장
	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(lpCycleChannelInfo->GetChannelNumber());
	CStep *pStep = pChannel->GetMgr()->GetAtStep(lpCycleChannelInfo->GetStepIndex());

	if (CMgrConfig::GetMgr()->GetMeasureModeNo() != 0)
	{
		strFilePath.Format("%s%s.csv"
			, (LPCSTR)pMgr->GetCurrentLogPath(lpCycleChannelInfo->GetLogChannelNumber())
			, CMgrConfig::GetMgr()->GetMeasureMode());

		CString strData = _T("");
		//Cycle, Step, Mode(Charge,Discharge)
		strData.Format(_T("%02d,%02d,%s,"), lpCycleChannelInfo->GetCycleNumber(), lpCycleChannelInfo->GetStepNumber(), pChannel->StepType2String());

		CString strTemp = _T("");
		strTemp += strData;

		// --------------------------------------------------------------------------
		//Auto Calibration Result Average Value
		float fAvgValue = 0.0f; 

		//Target Value
		if (CMgrConfig::GetMgr()->GetMeasureModeNo() == elFBoradStepModeMasterAutocalCurrent)
		{
			float fTarget = 0.0f;
			if (pStep->GetStepType() == eIFBoardStepTypeCharge)
			{
				fTarget = pStep->GetCurrentAmpere();
			}
			if (pStep->GetStepType() == eIFBoardStepTypeDischarge)
			{
				fTarget = pStep->GetDisCurrentAmpere();
			}
			if (pChannel->GetStepMode() == elFBoradStepModeMasterAutocalCurrent)
			{
				strData.Format(_T("%0.3f,"), fTarget);
				strTemp += strData;

				fAvgValue = pCalibratorVisaProcess->GetCalibratorChannelCurrentAvg(lpCycleChannelInfo->GetChannelNumber(), 5, 7);// / 3;

				pChannel->Set_CurrentCalibrationData(fTarget, fAvgValue);
			}
		}
		//else
		//{
		//	float fTarget = 0.0;

		//	strData.Format(_T("%0.3f,"), fTarget);
		//	strTemp += strData;
		//	fAvgValue = pCalibratorVisaProcess->GetCalibratorChannelVoltageAvg(lpCycleChannelInfo->GetChannelNumber(), 3, 6);// / 5;	// 전압교정때 카운트확인

		//	/*fCalibratorAmp = 0.0;*/
		//}
		strData.Format(_T("%0.6f\n"), fAvgValue);

		//strData.Format(_T("%0.6f, %0.6f, %0.6f, %0.6f\n")
		//	, pChannel->m_fFormationVoltage / pChannel->m_iVoltCnt
		//	, pChannel->m_fFormationAmpere / pChannel->m_iAmpCnt
		//	, fCalibratorVolt, fCalibratorAmp);
		strTemp += strData;

		WriteOpenedFile(strFilePath, strTemp, eResultFileTypeCalibrationAverage, lpCycleChannelInfo->GetChannelNumber(), lpCycleChannelInfo);

		//pStep->m_fAvgCalibratorAmpere = fCalibratorAmp;
		//pStep->m_fAvgFormationAmpere = pChannel->m_fFormationAmpere / pChannel->m_iAmpCnt;

		// --------------------------------------------------------------------

		pChannel->m_iVoltCnt = 0;
		pChannel->m_iAmpCnt = 0;

		pChannel->m_fFormationVoltage = 0.0;
		pChannel->m_fFormationAmpere = 0.0;

		pCalibratorVisaProcess->SetMeasureCount(0);
		//pCalibratorVisaProcess->SetCalibratorChannelDataVoltage(lpCycleChannelInfo->GetChannelNumber(), 0.0f);		 
		//pCalibratorVisaProcess->SetCalibratorChannelDataCurrent(lpCycleChannelInfo->GetChannelNumber(), 0.0f);
	}

	m_strEndReason = lpCycleChannelInfo->GetEndReasonStr();
	if (lpCycleChannelInfo->GetLogType() == LOG_TYPE_COMPLETE)
	{
		// 전류교정일때만 Target 전류
		// 220620 YGY Formation 장비에선 불필요해보임 
	/*	if (CMgrConfig::GetMgr()->GetMeasureModeNo() == elFBoradStepModeAutocalCurrent)
		{
			LowCalibration(lpCycleChannelInfo->GetChannelNumber(), lpCycleChannelInfo->GetStepNumber(), strFilePath);
		}*/

		if (pChannel->GetSelected() == true && pChannel->GetScheduleStart() == false)
		{
			m_bNextCh = true;

			// TODO:
			// Relay All Off 한 채널 교정 끝나고 All off 해줘야함.
			// MgrCalBox::GetMgr()->CalBox_RelayOn(elFBoradStepModeAutocalVoltage, 0, 0);

			CMgrPid* pMgrPid = CMgrPid::GetMgr();
			pMgrPid->GetDevice(0)->SetAOValueAll(0.0f);
		}

		//pCalibratorVisaProcess->SetMeasureModeNo(0);
	}
}

void CLogThread::WriteCalVolt(CCommData * lpCycleChannelInfo)
{
	CMgrLog* pMgr = CMgrLog::GetMgr();
	if (!pMgr)
		return;	
	
	CString strFilePath;
	// ---------------------------------------------------------------------------
	// AUTO_CALIBRATION

	CCalibratorVisaProcess* pCalibratorVisaProcess = CCalibratorVisaProcess::GetMgr();

	// 스텝별 측정 평균값 저장
	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(lpCycleChannelInfo->GetChannelNumber());

	if (CMgrConfig::GetMgr()->GetMeasureModeNo() != 0)
	{
		CString strResuleFilePath;
		strResuleFilePath = CMgrConfig::GetMgr()->GetLogFilePath();
		CString strFolderName;
		SYSTEMTIME tmCur;
		GetLocalTime(&tmCur);
		strFolderName.Format(_T("\\%02d_%02d_%02d\\"), tmCur.wYear, tmCur.wMonth, tmCur.wDay);

		strResuleFilePath += strFolderName;
		CreateDirectory(strResuleFilePath, NULL);

		CString strLogPath = strResuleFilePath;
		strFolderName.Format(_T("%dCH\\"), lpCycleChannelInfo->GetChannelNumber() + 1);

		strResuleFilePath += strFolderName;
		CreateDirectory(strResuleFilePath, NULL);

		strFolderName.Format(_T("%02d%02d%02d\\"),tmCur.wHour, tmCur.wMinute, tmCur.wSecond);
		strResuleFilePath += strFolderName;
		CreateDirectory(strResuleFilePath, NULL);


		strFilePath.Format("%s%s.csv", strResuleFilePath, CMgrConfig::GetMgr()->GetMeasureMode());


		CString strData = _T("");
		//Target Volt
		strData.Format(_T("%0.1f,"), pChannel->Get_CalVoltSetting());

		CString strTemp = _T("");
		strTemp += strData;

		// --------------------------------------------------------------------------
		//Auto Calibration Result Average Value
		float fAvgValue = 0.0f;	

		//Daq Volt
		fAvgValue = pCalibratorVisaProcess->GetCalibratorChannelVoltageAvg(lpCycleChannelInfo->GetChannelNumber(), 3, 6);// / 5;	// 전압교정때 카운트확인
		
		strData.Format(_T("%0.6f\n"), fAvgValue);

		//strData.Format(_T("%0.6f, %0.6f, %0.6f, %0.6f\n")
		//	, pChannel->m_fFormationVoltage / pChannel->m_iVoltCnt
		//	, pChannel->m_fFormationAmpere / pChannel->m_iAmpCnt
		//	, fCalibratorVolt, fCalibratorAmp);
		strTemp += strData;

		WriteOpenedFile(strFilePath, strTemp, eResultFileTypeCalibrationAverage, lpCycleChannelInfo->GetChannelNumber(), lpCycleChannelInfo);

		//pStep->m_fAvgCalibratorAmpere = fCalibratorAmp;
		//pStep->m_fAvgFormationAmpere = pChannel->m_fFormationAmpere / pChannel->m_iAmpCnt;

		// --------------------------------------------------------------------

		pChannel->m_iVoltCnt = 0;
		pChannel->m_iAmpCnt = 0;

		pChannel->m_fFormationVoltage = 0.0;
		pChannel->m_fFormationAmpere = 0.0;

		pCalibratorVisaProcess->SetMeasureCount(0);
		//pCalibratorVisaProcess->SetCalibratorChannelDataVoltage(lpCycleChannelInfo->GetChannelNumber(), 0.0f);		 
		//pCalibratorVisaProcess->SetCalibratorChannelDataCurrent(lpCycleChannelInfo->GetChannelNumber(), 0.0f);
	}
}
