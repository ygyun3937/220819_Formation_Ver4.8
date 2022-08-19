#include "stdafx.h"
#include "MgrStatus.h"
#include "MgrChannel.h"
#include "MgrConfig.h"
#include "MgrSerial.h"
#include "MgrCommData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define StatusFilePath	( _T("Status.ini") )

CMgrStatus::CMgrStatus()
{
	Initialize();
}

CMgrStatus::~CMgrStatus()
{
	Terminate();
}

// -------------------------------------------------------------------------------------------------

CString CMgrStatus::GetSubEqpDioConnectionStatus(int nSubEqpIndexNo)
{
	if (nSubEqpIndexNo < EQP_SUBEQP_MAX_COUNT)
	{
		return  m_SubEqpStatuInfo[nSubEqpIndexNo].strDioConnectionStatus;
	}
	return _T("NONE");
}

CString CMgrStatus::GetSubEqpRunStatus(int nSubEqpIndexNo)
{
	if (nSubEqpIndexNo < EQP_SUBEQP_MAX_COUNT)
	{
		return  m_SubEqpStatuInfo[nSubEqpIndexNo].strRunStatus;
	}
	return _T("NONE");
}

CString CMgrStatus::GetSubEqpStageRunMode(int nSubEqpIndexNo, int nSubEqpStageIndexNo)
{
	if ((nSubEqpIndexNo < EQP_SUBEQP_MAX_COUNT) || (nSubEqpStageIndexNo < EQP_SUBEQP_STAGE_MAX_COUNT))
	{
		m_SubEqpStatuInfo[nSubEqpIndexNo].m_SubEqpStageStatusInfo[nSubEqpStageIndexNo].strStageRunMode;
	}
	return _T("NONE");
}

/**
 * .
 * 
 * METHOD_NAME : Initialize
 * METHOD_TYPE : -
 * DESCRIPTION : 
 */
void CMgrStatus::Initialize()
{
	InitializeVariables();

	LoadFile();
}

/**
 * .
 * 
 * METHOD_NAME : InitializeVariables
 * METHOD_TYPE : -
 * DESCRIPTION : 
 */
void CMgrStatus::InitializeVariables()
{
	memset(m_nEqp_RunMode, EQP_RUN_MODE_IDLE, sizeof(m_nEqp_RunMode));
	// �ʱ�ȭ
	for (int nSubEqpCount = 0; nSubEqpCount < EQP_SUBEQP_MAX_COUNT; nSubEqpCount++)
	{
		m_SubEqpStatuInfo[nSubEqpCount].nSubEqpIndexNo = nSubEqpCount;
		m_SubEqpStatuInfo[nSubEqpCount].nSubEqpNo = nSubEqpCount + 1;
		m_SubEqpStatuInfo[nSubEqpCount].strRunStatus = _T("NONE");
		m_SubEqpStatuInfo[nSubEqpCount].strRunMode = _T("NONE");
		m_SubEqpStatuInfo[nSubEqpCount].bScheudleFinish = FALSE;

		m_SubEqpStatuInfo[nSubEqpCount].strDioConnectionStatus = _T("DISCONNECT");
		
		for (int nSubEqpStageCount = 0; nSubEqpStageCount < EQP_SUBEQP_STAGE_MAX_COUNT; nSubEqpStageCount++)
		{
			m_SubEqpStatuInfo[nSubEqpCount].m_SubEqpStageStatusInfo[nSubEqpStageCount].nStageIndexNo = nSubEqpStageCount;
			m_SubEqpStatuInfo[nSubEqpCount].m_SubEqpStageStatusInfo[nSubEqpStageCount].nStageNo = nSubEqpStageCount + 1;

			m_SubEqpStatuInfo[nSubEqpCount].m_SubEqpStageStatusInfo[nSubEqpStageCount].strStageRunMode = _T("MANUAL");
			m_SubEqpStatuInfo[nSubEqpCount].m_SubEqpStageStatusInfo[nSubEqpStageCount].strStageRunStatus = _T("NONE");
		}
	}

	m_Stage1StatusInfo.Init();
	m_Stage2StatusInfo.Init();

	memset(m_bStageAutoMode, FALSE, sizeof(m_bStageAutoMode));
	memset(m_bStageInitFlag, FALSE, sizeof(m_bStageInitFlag));
	memset(m_bStageStartFlag, FALSE, sizeof(m_bStageStartFlag));

}

/**
 * .
 * 
 * METHOD_NAME : LoadFile
 * METHOD_TYPE : -
 * DESCRIPTION : 
 */
void CMgrStatus::LoadFile()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\%s"), GetExePath(), StatusFilePath);

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

		auto Pos = listCyclerChannel.GetHeadPosition();

		while (Pos)
		{
			CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

			if (lpCyclerChannel)
			{
				SCylerChannelDump* lpCyclerDump = new SCylerChannelDump;

				CString strAppName;
				strAppName.Format(_T("Shcedule%d"), lpCyclerChannel->GetChannelNumber() + 1);

				lpCyclerDump->nCyclerChannelNumber	= INIReadInt(strAppName, "Channel", strFilePath);
				lpCyclerDump->strLogFilePath		= INIReadStr(strAppName, "LogPath", strFilePath);
				lpCyclerDump->strScheduleFilePath	= INIReadStr(strAppName, "SchPath", strFilePath);
				lpCyclerDump->strScheduleName		= INIReadStr(strAppName, "SchName", strFilePath);
				lpCyclerDump->strScheduleSubName	= INIReadStr(strAppName, "SchSubName", strFilePath);

				CString strScheduleStart = INIReadStr(strAppName, "SchStart", strFilePath);

				if (strcmp(strScheduleStart, "1") == 0)
					lpCyclerDump->bScheduleStart = true;
				else
					lpCyclerDump->bScheduleStart = false;

				m_vecCyclerDump.push_back(lpCyclerDump);
			}
		}
	}
}

// -----------------------------------------------------------------------------

void CMgrStatus::Terminate()
{
	auto iter = m_vecCyclerDump.begin();
	auto iterE = m_vecCyclerDump.end();

	while (iter != iterE)
	{
		SCylerChannelDump* lpCyclerDump = (*iter);

		if (lpCyclerDump)
		{
			delete lpCyclerDump;
			lpCyclerDump = nullptr;
		}

		++iter;
	}
}

void CMgrStatus::Set_Status_Stage1(int nCurrentStep, int nRunStatus)
{
	//step�� �ٲ�ų� �Ϸ���°� �ٲ��쿡�� ���� Set
	if (m_Stage1StatusInfo.nCurrentStep != nCurrentStep || m_Stage1StatusInfo.nStatus != nRunStatus)
	{
		m_Stage1StatusInfo.nCurrentStep = nCurrentStep;
		m_Stage1StatusInfo.nStatus = nRunStatus;
		m_Stage1StatusInfo.bInit = true;
	}
}

void CMgrStatus::Set_Status_Stage2(int nCurrentStep, int nRunStatus)
{
	if (m_Stage2StatusInfo.nCurrentStep != nCurrentStep || m_Stage2StatusInfo.nStatus != nRunStatus)
	{
		m_Stage2StatusInfo.nCurrentStep = nCurrentStep;
		m_Stage2StatusInfo.nStatus = nRunStatus;
		m_Stage2StatusInfo.bInit = true;
	}
}

void CMgrStatus::SetEqStartFlag(int nStage, BOOL bStartFlag)
{
	if (bStartFlag)
	{
		CMgrStatus::GetMgr()->SetEqp_RunMode(nStage, EQP_RUN_MODE_AUTO);
	}
	else
	{
		CMgrStatus::GetMgr()->SetEqp_RunMode(nStage, EQP_RUN_MODE_MANUAL);
	}
	m_bStageStartFlag[nStage] = bStartFlag;
}



// -------------------------------------------------------------------------------------------------

SCylerChannelDump*	CMgrStatus::GetCyclerChannelDump(int nCyclerChannelNumber)
{
	auto iter = m_vecCyclerDump.begin();
	auto iterE = m_vecCyclerDump.end();

	while (iter != iterE)
	{
		SCylerChannelDump* lpCyclerDump = (*iter);

		if (lpCyclerDump)
		{
			if (lpCyclerDump->nCyclerChannelNumber == nCyclerChannelNumber)
				return lpCyclerDump;
		}

		++iter;
	}

	return nullptr;
}

void	CMgrStatus::SaveFile(int nCyclerChannelNumber)
{
	SCylerChannelDump* lpCyclerDump = GetCyclerChannelDump(nCyclerChannelNumber);

	if (lpCyclerDump)
	{
		CString strFilePath;
		strFilePath.Format(_T("%s\\Config\\%s"), (LPCSTR)GetExePath(), StatusFilePath);

		CString strAppName;
		strAppName.Format(_T("Shcedule%d"), nCyclerChannelNumber + 1);

		INIWriteStr(strAppName, _T("LogPath"), lpCyclerDump->strLogFilePath, strFilePath);
		INIWriteStr(strAppName, _T("SchName"), lpCyclerDump->strScheduleName, strFilePath);
		INIWriteStr(strAppName, _T("SchSubName"), lpCyclerDump->strScheduleSubName, strFilePath);
		INIWriteStr(strAppName, _T("SchPath"), lpCyclerDump->strScheduleFilePath, strFilePath);

		if (true == lpCyclerDump->bScheduleStart)
			INIWriteStr(strAppName, _T("SchStart"), "1", strFilePath);
		else
			INIWriteStr(strAppName, _T("SchStart"), "0", strFilePath);
	}
}

BOOL CMgrStatus::GetEqpRunProcessingCheck(int nSubeqNo)
{
	if (GetEqpRunProcessErrorCheck(nSubeqNo) == TRUE)
	{
		return false;
	}
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	int nSubEqpStageMaxCount = pMgrConfig->GetTotalSubEqpCount();
	int nSubEqpStageChannelMaxCount = pMgrConfig->GetSubEqpInfo().nSubEqpStageChannelCount;

	//��ü ä�� ���� Ȯ�� ( Stage1, Stage2 ���� ����ϴ� ��� ä��)
	for (int nCh = nSubeqNo * nSubEqpStageChannelMaxCount; nCh < (nSubeqNo + 1)*nSubEqpStageChannelMaxCount; nCh++)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCh);
		if (pChannel->GetSelected() == true)
		{
			int nChannelStatus = CMgrChannel::GetMgr()->GetChannel(nCh)->GetChannelStatus();
			if (nChannelStatus == eCyclerChannelStateStart)
			{
				return true;
			}
		}
	}
	return false;
}

//��ü ä�� Error TRUE
//�ݴ��� FALSE
BOOL CMgrStatus::GetEqpRunProcessErrorCheck(int nSubeqNo)
{
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	int nSubEqpStageChannelMaxCount = pMgrConfig->GetSubEqpInfo().nSubEqpStageChannelCount;
	bool bSelected = false;
	bool bError = true;

	//��ü ä�� ���� Ȯ�� ( Stage1, Stage2 ���� ����ϴ� ��� ä��)
	for (int nCh = nSubeqNo * nSubEqpStageChannelMaxCount; nCh < (nSubeqNo + 1)*nSubEqpStageChannelMaxCount; nCh++)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCh);
		if (pChannel->GetSelected() == true)
		{
			int nChannelStatus = CMgrChannel::GetMgr()->GetChannel(nCh)->GetChannelStatus();
			bSelected = true;
			if (nChannelStatus != eCyclerChannelStateError|| nChannelStatus != eCyclerChannelStateCommuicationError)
			{
				bError = false;
			}			
		}
	}

	if (bSelected == true && bError == true)
	{
		return true;
	}
	return false;
}

void CMgrStatus::SetEQp_Finish(int nStageNo, bool bFinish)
{
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	CMgrSerial* pMgrSerial = CMgrSerial::GetMgr();

	//Stage Barcode ���� �ʱ�ȭ
	if (bFinish == true)
	{
		if (GetEQp_Finish(nStageNo) == true)
			return;

		int nSubEqpStageMaxCount = pMgrConfig->GetTotalSubEqpCount();
		int nSubEqpStageChannelMaxCount = pMgrConfig->GetSubEqpInfo().nSubEqpStageChannelCount;
		bool bSelected = false;
		bool bComplete = true;

		//Tray Barcode �ʱ�ȭ
		pMgrSerial->GetAtBarcode(nStageNo)->SetTrayBarcode("");

		for (int nCh = nStageNo * nSubEqpStageChannelMaxCount; nCh < (nStageNo + 1)*nSubEqpStageChannelMaxCount; nCh++)
		{
			CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCh);

			pChannel->SetChBarcode("");
			CCommData*	pCommData;

			pCommData = CMgrCommData::GetMgr()->GetAt(nCh);
			pCommData->SetChBarcode("");

		}

	}

	m_SubEqpStatuInfo[nStageNo].bScheudleFinish = bFinish;
}

int CMgrStatus::GetEqp_RunMode(int nStageNo)
{
	return m_nEqp_RunMode[nStageNo];
}

void CMgrStatus::SetEqp_SystemAlarm(int nStageNo, int nAlarm)
{
	m_nEqp_Alarm[nStageNo] = nAlarm;
}

int CMgrStatus::GetEqp_SystemAlarm(int nStageNo)
{
	return m_nEqp_Alarm[nStageNo];
}

int CMgrStatus::GetEqpRunStatus(int nSubEqNo)
{
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	int nSubEqpStageMaxCount = pMgrConfig->GetTotalSubEqpCount();
	int nSubEqpStageChannelMaxCount = pMgrConfig->GetSubEqpInfo().nSubEqpStageChannelCount;

	if (GetEqpRunProcessErrorCheck(nSubEqNo) == true)
	{
		return eCyclerChannelStateError;
	}
	 
	//Start Ȯ��
	for (int nCh = nSubEqNo * nSubEqpStageChannelMaxCount; nCh < (nSubEqNo + 1)*nSubEqpStageChannelMaxCount; nCh++)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCh);
		if (pChannel->GetSelected() == true)
		{
			int nChannelStatus = CMgrChannel::GetMgr()->GetChannel(nCh)->GetChannelStatus();
			if (nChannelStatus == eCyclerChannelStateStart)
			{
				return eCyclerChannelStateStart;
			}
		}
	}
	//Ready Ȯ��
	for (int nCh = nSubEqNo * nSubEqpStageChannelMaxCount; nCh < (nSubEqNo + 1)*nSubEqpStageChannelMaxCount; nCh++)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCh);
		if (pChannel->GetSelected() == true)
		{
			int nChannelStatus = CMgrChannel::GetMgr()->GetChannel(nCh)->GetChannelStatus();
			if (nChannelStatus == eCyclerChannelStateReady)
			{
				return eCyclerChannelStateReady;
			}
		}
	}

	//End Ȯ��
	return GetEqpRunProcessEndCheck(nSubEqNo);


}

//������ ä�� �ϳ��� �Ϸ���°� �ƴϸ� FALSE
//���� ä���� �Ϸ���¸� TRUE
int CMgrStatus::GetEqpRunProcessEndCheck(int nSubeqNo)
{
	if (GetEqpRunProcessErrorCheck(nSubeqNo) == TRUE)
	{
		return false;
	}	

	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	int nSubEqpStageMaxCount = pMgrConfig->GetTotalSubEqpCount();
	int nSubEqpStageChannelMaxCount = pMgrConfig->GetSubEqpInfo().nSubEqpStageChannelCount;
	bool bSelected = false;
	bool bComplete = true;

	//��ü ä�� ���� Ȯ�� ( Stage���� ����ϴ� ��� ä��)
	for (int nCh = nSubeqNo * nSubEqpStageChannelMaxCount; nCh < (nSubeqNo + 1)*nSubEqpStageChannelMaxCount; nCh++)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCh);
		if (pChannel->GetSelected() == true)
		{
			int nChannelStatus = CMgrChannel::GetMgr()->GetChannel(nCh)->GetChannelStatus();
			bSelected = true;
			if (nChannelStatus != eCyclerChannelStateComplete)
			{
				bComplete = false;
			}

		}
	}
	//�ʱ�ȭ�� Init����
	if ((bSelected == true &&bComplete == true)|| GetEQp_Finish(nSubeqNo)==true)
	{
		SetEQp_Finish(nSubeqNo, true);
		return eCyclerChannelStateComplete;
	}

	//IDLE Ȯ��
	bool bIDLE = true;

	//��ü ä�� ���� Ȯ�� ( Stage���� ����ϴ� ��� ä��)
	for (int nCh = nSubeqNo * nSubEqpStageChannelMaxCount; nCh < (nSubeqNo + 1)*nSubEqpStageChannelMaxCount; nCh++)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCh);
		if (pChannel->GetSelected() == true)
		{
			int nChannelStatus = CMgrChannel::GetMgr()->GetChannel(nCh)->GetChannelStatus();
			bSelected = true;
			if (nChannelStatus != eCyclerChannelStateIdle)
			{
				bIDLE = false;
			}

		}
	}

	if (bSelected == true && bIDLE == true)
	{
		return eCyclerChannelStateIdle;
	}
}

// -------------------------------------------------------------------------------------------------

void CMgrStatus::SetSubEqpStatusInfo(int nSubEqpIndexNo, SUBEQP_STATUS_INFO subEqpStatusInfo)
{
	m_SubEqpStatuInfo[nSubEqpIndexNo] = subEqpStatusInfo;
}

SUBEQP_STATUS_INFO CMgrStatus::GetSubEqpStatusInfo(int nSubEqpIndexNo)
{
	return m_SubEqpStatuInfo[nSubEqpIndexNo];
}


