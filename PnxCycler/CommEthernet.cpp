#include "stdafx.h"
#include "CommEthernet.h"
#include "MgrCommData.h"
#include "MgrComm.h"
#include "MgrAlarm.h"
#include "MgrChannel.h"
#include "PnxUtil.h"
#include "MgrConfig.h"
#include "MgrDio.h"
#include "MgrDAQ.h"
#include "MgrPlcInterface.h"
#include "DlgViewAlarm.h" //lyw4_1

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FRAME_HEADER	0x40
#define FRAME_ALRAM		0x41
#define FARAM_COMMAND	0x43
#define FRAME_DATA		0x44
#define FRAME_END		0x45
#define FRAME_SAFETY	0x46
#define FRAME_MODIFY	0x4D
#define FRAME_START		0x53
#define FRAME_JUMP		0x4A
#define FRAME_PAUSE		0x50
#define FRAME_EQREQ		0x51
#define FRAME_STATUS	0x54


enum FRAME_TYPE
{
	FRAME_TYPE_UNKNOWN = -1,
	FRAME_TYPE_DATA,
	FRAME_TYPE_END,
	FRAME_TYPE_ALRAM,
	FRAME_TYPE_START,
	FRAME_TYPE_COMMAND,
	FRAME_TYPE_JUMP,
	FRAME_TYPE_PAUSE,
	FRAME_TYPE_STATUS,
};

enum FRAME_GAP
{
	FRAME_GAP_DATA = 38 //taek 기존 34
};

enum ALRAM_FRAME_INDEX
{
	ALRAM_CHANNEL1 = 4,
	ALRAM_CHANNEL2,
	ALRAM_STEP1,
	ALRAM_STEP2,
	ALRAM_WARING1,
	ALRAM_WARING2,
	ALRAM_GRID1,
	ALRAM_GRID2,
	ALRAM_DC1,
	ALRAM_DC2,
	ALRAM_FAULT1,
	ALRAM_FAULT2,
	ALRAM_END1,
	ALRAM_END2
};

enum END_FRAME_INDEX
{
	END_CHANNEL1 = 4,
	END_CHANNEL2
};

enum DATA_FRAME_INDEX
{
	DATA_COUNT1 = 2,
	DATA_COUNT2,
	DATA_CHANNEL1,
	DATA_CHANNEL2,
	DATA_TIME_PERIOD1,
	DATA_TIME_PERIOD2,
	DATA_STEP_INDEX1,
	DATA_STEP_INDEX2,
	DATA_COMMAND1,
	DATA_COMMAND2,
	DATA_END_REASON1,
	DATA_END_REASON2,
	DATA_OP1,
	DATA_OP2,
	DATA_MODE1,
	DATA_MODE2,
	DATA_VOLTAGE1,
	DATA_VOLTAGE2,
	DATA_VOLTAGE3,
	DATA_VOLTAGE4,
	DATA_AMPERE1,
	DATA_AMPERE2,
	DATA_AMPERE3,
	DATA_AMPERE4,
	DATA_TIME_SPAN_LSB1,
	DATA_TIME_SPAN_LSB2,
	DATA_TIME_SPAN_LSB3,
	DATA_TIME_SPAN_LSB4,
	DATA_TIME_SPAN_MSB1,
	DATA_TIME_SPAN_MSB2,
	DATA_TIME_SPAN_MSB3,
	DATA_TIME_SPAN_MSB4,
	DATA_END_VALUE1,
	DATA_END_VALUE2,
	DATA_END_VALUE3,
	DATA_END_VALUE4,
	DATA_LOOP_CNT1,
	DATA_LOOP_CNT2,
	DATA_CAPACITY1, //taek 아래로 추가 항목
	DATA_CAPACITY2,
	DATA_CAPACITY3,
	DATA_CAPACITY4,
	DATA_TEMP1,		//220216 Temp Protocol Add
	DATA_TEMP2,
	DATA_TEMP3,
	DATA_TEMP4,
};

enum EthernetCmdType
{
	EthernetCmdTypeRest = 0x81,
	EthernetCmdTypeCharge = 0x82,
	EthernetCmdTypeDischarge = 0x83,
	EthernetCmdTypeAlarmClear = 0x84,
	EthernetCmdTypeInputRelayOff = 0x85,
	EthernetCmdTypeInputRelayOn = 0x86,
	EthernetCmdTypeOutputRelayOff = 0x87,
	EthernetCmdTypeOutputRelayOn = 0x88,
	EthernetCmdTypeAllRelayOff = 0x89,
	EthernetCmdTypeAlarmRequest = 0x90,
	EthernetCmdTypeModeParallel = 0x91,
	EthernetCmdTypeModeSerial = 0x92,
	EthernetCmdTypeChargeCV = 0x98,
};

CCommEthernet::CCommEthernet()
{
	m_pQueuingThread = NULL;
	m_pCheckTimeThread = NULL;
	m_bLoopQueue = TRUE;
	m_bTimeCheck = TRUE;
	m_nBoardIndex = -1;
	m_nCommType = -1;
	m_bFirstReceive = FALSE;
	m_nAbnormalCount = 0;
	m_nAbnormalCheckTime = 0;
	m_fBeforCur = 0;
	m_nUIChangeBefore = 0;

	m_nDmmCount = 0;
	m_nOldEndReason = 0;
	m_pChannel = new CChannel;

	m_bChamberSendMessage = TRUE;
}

CCommEthernet::~CCommEthernet()
{
	if (m_pChannel != NULL)//<!--20210507 add by swseo
	{
		delete m_pChannel;
		m_pChannel = NULL;
	}//-->>
}

int		CCommEthernet::GetBoardIndex()
{
	return m_nBoardIndex;
}

void	CCommEthernet::SetBoardIndex(int nIndex)
{
	m_nBoardIndex = nIndex;
}

int		CCommEthernet::GetCommType()
{
	return m_nCommType;
}

void	CCommEthernet::SetCommType(int nCommType)
{
	m_nCommType = nCommType;
}

void	CCommEthernet::SetBeforCur(float fBeforCur)
{
	m_fBeforCur = fBeforCur;
}
float	CCommEthernet::GetBeforCur()
{
	return m_fBeforCur;
}

void	CCommEthernet::StartThread()
{
	if (NULL == m_pQueuingThread)
		m_pQueuingThread = AfxBeginThread(QueuingThread, this);

	if (NULL == m_pCheckTimeThread)
		m_pCheckTimeThread = AfxBeginThread(TimeCheckThread, this);
}

void	CCommEthernet::StopThread()
{
	m_bLoopQueue = FALSE;

	if (m_pQueuingThread)
	{
		::WaitForSingleObject(m_pQueuingThread->m_hThread, INFINITE);

		m_pQueuingThread = NULL;
	}

	m_bTimeCheck = FALSE;

	if (m_pCheckTimeThread)
	{
		::WaitForSingleObject(m_pCheckTimeThread->m_hThread, INFINITE);

		m_pCheckTimeThread = NULL;
	}

	// Thread 종료 후 Queue 비우자
	while (!m_Queue.IsEmpty())
		delete m_Queue.RemoveHead();
}

void	CCommEthernet::AddQueue(TEthernet* pNewElement)
{

	m_csPush.Lock();

	if (m_Queue.GetCount() != 0 && m_Queue.GetCount() % 10 == 0)
		m_Queue.GetCount();

	m_Queue.AddTail(pNewElement);

	m_csPush.Unlock();
}

TEthernet*	CCommEthernet::GetQueue()
{
	m_csPush.Lock();

	TEthernet* pComm = NULL;
	//SYSLOG(Dbg, "GetQueue Before - CCommEthernet::GetQueue - %d", m_Queue.GetCount());

	if (FALSE == m_Queue.IsEmpty()) {
		pComm = m_Queue.RemoveHead();
		//SYSLOG(Dbg, "GetQueue After - CCommEthernet::GetQueue - %d", m_Queue.GetCount());

	}

	m_csPush.Unlock();

	return pComm;
}

UINT	CCommEthernet::QueuingThread(LPVOID pParam)
{
	CCommEthernet* pMgr = (CCommEthernet*)pParam;

	if (!pMgr)
		return 0;

	TEthernet tEthernet;

	while (pMgr->m_bLoopQueue)
	{
		TEthernet* pComm = pMgr->GetQueue();

		if (NULL == pComm)
		{
			Sleep(1);

			continue;
		}
		/*	if (pMgr->GetFrameType(&tEthernet) == FRAME_TYPE_START)
			{
				continue;
			}*/
		tEthernet.strTime = pComm->strTime;
		tEthernet.strData = pComm->strData;
		tEthernet.nBoardNumber = pComm->nBoardNumber;
		tEthernet.nSize = pComm->nSize;

		memcpy(&tEthernet.pszComm, pComm->pszComm, sizeof(pComm->pszComm));

		DELETE_POINTER(pComm);

		if (eDeviceTypeIFBoard == pMgr->GetCommType())
		{
			switch (pMgr->GetFrameType(&tEthernet))
			{

			//220124 YGYUN Formation 적용 함수 ( 기존 데이터 취득 방식 : 100msec당 100개 데이터 취득 후 처리 , 변경 데이터 취득 방식 : 100msec당 데이터 1개 취득 후 처리)
			//case FRAME_TYPE_DATA	: pMgr->ParseBoardDataFrame(&tEthernet);		break;
			case FRAME_TYPE_DATA: pMgr->ParseBoardDataFrame_Formation(&tEthernet);		break;
			case FRAME_TYPE_END: pMgr->ParseBoardEndFrame(&tEthernet);		break;
			case FRAME_TYPE_ALRAM: pMgr->ParseBoardAlarmFrame(&tEthernet);	break;
			case FRAME_TYPE_START: pMgr->ParseBoardStart(&tEthernet);			break;
			case FRAME_TYPE_COMMAND: pMgr->ParseBoardCommand(&tEthernet);		break;
			case FRAME_TYPE_JUMP: pMgr->ParseBoardJump(&tEthernet);			break;
			case FRAME_TYPE_PAUSE: pMgr->ParseBoardPause(&tEthernet);			break;
			case FRAME_TYPE_STATUS: pMgr->ParseBoardStatus(&tEthernet);		break;  // 상태보고 Restart 구분
			}
		}

		if (eDeviceTypePlcInterface == pMgr->GetCommType())
		{
			pMgr->ReceivePlcInterfaceData(&tEthernet);
		}

		Sleep(1);
	}

	return 0;
}

UINT CCommEthernet::TimeCheckThread(LPVOID pParam)
{
	CCommEthernet* pMgr = (CCommEthernet*)pParam;

	if (!pMgr)
		return 0;

	while (pMgr->m_bTimeCheck)
	{
		for (int ch = 0; ch < CMgrConfig::GetMgr()->GetChamberChannelCount(); ch++)
		{
			CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(ch);

			ULONGLONG ulNowTime = 0;
			ulNowTime = ::GetTickCount64() / 1000;


			if (pChannel->GetChannelStatusStr() != _T("None") && pChannel->GetChannelStatusStr() != _T("Sample not connected"))
			{
				if (llabs(pChannel->m_ulLastAckTime - ulNowTime) > 10)
				{
					if (pChannel->GetChannelStatus() != eCyclerChannelStateError)
					{
						pChannel->SetChannelStatus(eCyclerChannelStateError);
						CMgrAlarm::GetMgr()->AddSysAlarm(ch, "Communication Alarm", "Countine Click");

						CDlgViewAlarm* pDlg = CDlgViewAlarm::GetMgr();

						if (pDlg != NULL)
						{
							if (FALSE == pDlg->IsWindowVisible())
							{
								pDlg->UpdateStatus(false);
								pDlg->ShowWindow(SW_SHOW);
							}
						}
					}

				}
			}
		}

		Sleep(1);
	}

	return 0;
}

int	CCommEthernet::GetFrameType(const TEthernet* pComm)
{
	if (pComm->pszComm == NULL)
		return FRAME_TYPE_UNKNOWN;

	if (pComm->pszComm[0] != FRAME_HEADER)
		return FRAME_TYPE_UNKNOWN;

	if (pComm->pszComm[1] == FRAME_DATA)			return FRAME_TYPE_DATA;
	else if (pComm->pszComm[1] == FRAME_END)		return FRAME_TYPE_END;
	else if (pComm->pszComm[1] == FRAME_ALRAM)		return FRAME_TYPE_ALRAM;
	else if (pComm->pszComm[1] == FRAME_START)		return FRAME_TYPE_START;
	else if (pComm->pszComm[1] == FRAME_JUMP)		return FRAME_TYPE_JUMP;
	else if (pComm->pszComm[1] == FRAME_PAUSE)		return FRAME_TYPE_PAUSE;
	else if (pComm->pszComm[1] == FARAM_COMMAND)	return FRAME_TYPE_COMMAND;
	else if (pComm->pszComm[1] == FRAME_STATUS)		return FRAME_TYPE_STATUS;

	return FRAME_TYPE_UNKNOWN;
}

void CCommEthernet::ParseBoardDataFrame_Formation(const TEthernet * pComm)
{
	int		nCommand = -1;
	int		nOpMode = -1;
	int		nStepMode = -1;
	int		nSamplingTime = -1;
	int		nStepIndex = -1;
	int		nChannelNum = -1;
	int		nEndReason = 0;
	int		nLoopcont = 0;
	int		nTimeSpan1 = 0;
	int		nTimeSpan2 = 0;
	int		nTimeSpan3 = 0;
	int		nTimeSpan4 = 0;

	int		nFlameCnt = 0;

	float	fAvgVoltage = 0.0f;
	float	fAvgAmpere = 0.0f;
	float	fAvgCapacity = 0.0f; //taek
	float	fStepEndValue = 0.0f;
	float	fTemp = 0.0f;

	CChannel*	pChannel = NULL;
	CCommData*	pCommData = NULL;
	
	int nBoardIndex = pComm->pszComm[DATA_CHANNEL1] << 8 | pComm->pszComm[DATA_CHANNEL2];
	nChannelNum = CMgrComm::GetMgr()->GetChannelNumber(eDeviceTypeIFBoard, pComm->nBoardNumber, nBoardIndex);
	pCommData = CMgrCommData::GetMgr()->GetAt(nChannelNum);
	pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNum);

	CStep* pStep = pChannel->GetMgr()->GetAtStep(pChannel->GetCycleNumber(), pChannel->GetStepNumber());

	if (pCommData == NULL || pChannel == NULL)
	{
		SYSLOG(Dbg, "[PK_ERROR]  pCommData, pChannel is null return");
		return;
	}

	if (FALSE == m_bFirstReceive)
	{
		CMgrChannel::GetMgr()->SendInitial(pComm->nBoardNumber);
		m_bFirstReceive = TRUE;
	}

	if (nFlameCnt < 0)
	{
		SYSLOG(Dbg, "[PK_ERROR] flmecnt(%d) return", nFlameCnt);
		return;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////
	// IF 통신상태 확인 2021-02-04 CNZ
	// 일단 조건 2개만 .필요시 상태 조건 추가.

		//Sampling Time 데이터 
	int nRowDataSmpTime = (int)(pComm->pszComm[DATA_TIME_PERIOD1] << 8 | pComm->pszComm[DATA_TIME_PERIOD2]);

	switch (nRowDataSmpTime)
	{
	case 1: nSamplingTime = 1;		break;
	case 2: nSamplingTime = 10;		break;
	case 3: nSamplingTime = 100;	break;
	case 4: nSamplingTime = 1000;	break;
	}

	nStepIndex = (int)(pComm->pszComm[DATA_STEP_INDEX1] << 8 | pComm->pszComm[DATA_STEP_INDEX2]);
	nTimeSpan1 = (int)(pComm->pszComm[DATA_TIME_SPAN_LSB1] << 8 | pComm->pszComm[DATA_TIME_SPAN_LSB2]);
	nTimeSpan2 = (int)(pComm->pszComm[DATA_TIME_SPAN_LSB3] << 8 | pComm->pszComm[DATA_TIME_SPAN_LSB4]);
	nTimeSpan3 = (int)(pComm->pszComm[DATA_TIME_SPAN_MSB1] << 8 | pComm->pszComm[DATA_TIME_SPAN_MSB2]);
	nTimeSpan4 = (int)(pComm->pszComm[DATA_TIME_SPAN_MSB3] << 8 | pComm->pszComm[DATA_TIME_SPAN_MSB4]);

	nLoopcont = (int)(pComm->pszComm[DATA_LOOP_CNT1] << 8 | pComm->pszComm[DATA_LOOP_CNT2]);
	nEndReason = (int)(pComm->pszComm[DATA_END_REASON1] << 8 | pComm->pszComm[DATA_END_REASON2]);
	nCommand = (int)(pComm->pszComm[DATA_COMMAND1] << 8 | pComm->pszComm[DATA_COMMAND2]);

	nStepMode = (int)(pComm->pszComm[DATA_MODE1] << 8 | pComm->pszComm[DATA_MODE2]);
	nOpMode = (int)(pComm->pszComm[DATA_OP1] << 8 | pComm->pszComm[DATA_OP2]);

	fStepEndValue = (float)(pComm->pszComm[DATA_END_VALUE1] << 24 |
		pComm->pszComm[DATA_END_VALUE2] << 16 |
		pComm->pszComm[DATA_END_VALUE3] << 8 |
		pComm->pszComm[DATA_END_VALUE4]);

	if (CMgrConfig::GetMgr()->GetMicroV()) //taek 추가 0319
		fStepEndValue *= 0.0001f;
	else
		fStepEndValue *= 0.001f;

	long lVoltage = pComm->pszComm[DATA_VOLTAGE1] << 24 |
		pComm->pszComm[DATA_VOLTAGE2] << 16 |
		pComm->pszComm[DATA_VOLTAGE3] << 8 |
		pComm->pszComm[DATA_VOLTAGE4];


	if (pChannel->GetEqMode() == EqModeSerial)
	{
		fAvgVoltage = (float)lVoltage;
		if (CMgrConfig::GetMgr()->GetMicroV())
			fAvgVoltage *= 0.0001f;
		else
			fAvgVoltage *= 0.001f;
	}
	else if (pChannel->GetEqMode() == EQModeParallel)
	{
		if (CMgrConfig::GetMgr()->GetMicroV())
		{
			fAvgVoltage = (float)lVoltage * 0.0001f;
		}
		else
		{
			fAvgVoltage = (float)lVoltage * 0.001f;
		}
	}

	long lAmpere = pComm->pszComm[DATA_AMPERE1] << 24 |
		pComm->pszComm[DATA_AMPERE2] << 16 |
		pComm->pszComm[DATA_AMPERE3] << 8 |
		pComm->pszComm[DATA_AMPERE4];

	//Pogo Pin
	if (nCommand == 0x93)
	{
		fAvgAmpere = (float)(lAmpere);
		if (CMgrConfig::GetMgr()->GetMicroA())
			fAvgAmpere *= 0.0001f;
		else
			fAvgAmpere *= 0.001f;

	}
	else
	{
		if (pChannel->GetEqMode() == EqModeSerial)
		{
			fAvgAmpere = (float)(lAmpere);
			if (CMgrConfig::GetMgr()->GetMicroA())
				fAvgAmpere *= 0.0001f;
			else
				fAvgAmpere *= 0.001f;
		}
		else if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (CMgrConfig::GetMgr()->GetMicroA())
				fAvgAmpere = (float)(lAmpere) *  0.0001f;
			else
				fAvgAmpere = (float)(lAmpere) * 0.001f;
		}
	}


	long lCapacity = pComm->pszComm[DATA_CAPACITY1] << 24 |
		pComm->pszComm[DATA_CAPACITY2] << 16 |
		pComm->pszComm[DATA_CAPACITY3] << 8 |
		pComm->pszComm[DATA_CAPACITY4]; //taek 추가

	fAvgCapacity = (float)(lCapacity);
	fAvgCapacity *= 0.0000000001f;


	////220511 YGY Formation Temp Daq 추가 
	CMgrDAQ* pMgrDaQ = CMgrDAQ::GetMgr();

	if (pMgrDaQ)
	{
		CString vecValue;

		fTemp = pMgrDaQ->GetDAQTemperature(nChannelNum);
		vecValue = CMgrConfig::GetMgr()->GetSystemSafeInfoTempMax();

		//220809 YGY North Volt 배터리 내부 쇼트 현상으로 온도 급상승 발생 대비 안전 조건 추가
		if (atof(vecValue) < fTemp)
		{
			CMgrAlarm::GetMgr()->AddSysAlarm(nChannelNum, "System Safety", "Satisfying System safety Cond Temp Max", ALARM_HEAVY);

		}
	}


	pChannel->SetLastAckTime(::GetTickCount64() / 1000);
	ULONGLONG a = (nTimeSpan1 << 24) | (nTimeSpan2 << 16) | (nTimeSpan3 << 8) | nTimeSpan4;
	pChannel->SetBoardTime((nTimeSpan1 << 24) | (nTimeSpan2 << 16) | (nTimeSpan3 << 8) | nTimeSpan4);

	if (pChannel->GetEqMode() == EqModeSerial)
	{
		fAvgVoltage = ROUNDING(fAvgVoltage, 4);

		fAvgAmpere = ROUNDING(fAvgAmpere, 4);

		//220216 YGY Formation Temp Protocol Add 
		fTemp = ROUNDING(fTemp, 4);
	}

	////pnx_hr dcir 평균전류 
	CString strDcirTIme = CMgrConfig::GetMgr()->m_strDcirTime;
	if (!strDcirTIme.IsEmpty())
	{
		pCommData->m_fStepTotalAmpere += fAvgAmpere;
	}

	//debug start
	//original fw data
	CString strVolDebug, strCurDebug, strTempDebug;
	strVolDebug.Format("%.3f", fAvgVoltage);
	strCurDebug.Format("%.3f", fAvgAmpere);
	strTempDebug.Format("%.3f", fTemp);


	//승온 조건 일 때와 아닐 때의 구분

	//if (nStepIndex > 0 && false == pChannel->GetScheduleStart())
	//{
	//	if (true == pChannel->GetTempSync() && pChannel->GetEndSync() == TRUE) //딜레이 일때
	//	{
	//		pChannel->SetChannelStatus(eCyclerChannelStatePauseFromDwell);
	//	}
	//	else if (true == pChannel->GetStepSync())//else if (true == pChannel->GetTempSync()) //스텝	
	//	{
	//		pChannel->SetChannelStatus(eCyclerChannelStatePauseFromStepSync);
	//	}
	//	else if (true == pChannel->GetTempSync() && false == pChannel->GetChamberElevatedTemperature()) //온도
	//	{
	//		pChannel->SetChannelStatus(eCyclerChannelStatePauseFromTempSync);
	//	}
	//	else if (true == pChannel->GetTempSync() && true == pChannel->GetChamberElevatedTemperature()) //승온
	//	{
	//		pChannel->SetChannelStatus(eCyclerChannelStatePauseFromWaitChamber);
	//	}
	//}


	//if (nStepIndex > 0/* && pCommData->GetStepTime() % 2 == 0*/)
	if ((pCommData->GetEndReason() > 0 || nEndReason > 0) && nEndReason != m_nOldEndReason)
	{
		SYSLOG(Dbg, "[rcv] ch(%d)  step(%d)  endr(%d)  rcvtm(%s)  cmd(%x)  opmode(%d)  stepmode(%d)  volt(%s)  cur(%s)	temp(%s)  tmsp4(%d)  frame(%d), loop(%d), endreason(%d)",
			nChannelNum + 1, nStepIndex, nEndReason, pComm->strTime, nCommand, nOpMode, nStepMode, strVolDebug, strCurDebug, strTempDebug, nTimeSpan4, nFlameCnt, nLoopcont, pCommData->GetEndReason());

		m_nOldEndReason = nEndReason;
		//	TRACE("StepIndex(%d)\n", nStepIndex);
	}
	/*TRACE()*/
//end


//update 이전 현재 sw가 계산 해줘야 하는 것들 (즉 나중에 fw개선 필요한 것들)
	bool blsUIStepChange = false;
	int  nUIStepNumber = 0;				//XML 1base
	int  nUICycleNumber = 0;			//XML 1base 

	nLoopcont = pCommData->CalcLoopCount(nEndReason, nStepIndex, nLoopcont);

	bool bIsStepChange = pCommData->CheckEQStepChange(nStepIndex, nLoopcont);

	bool bIsCycleChange = pCommData->CheckEQCycleChange(nStepIndex);

	if (bIsStepChange)
	{
		pCommData->RefreshMsgCount();
		m_nOldEndReason = 0;
	}
	// Add By JSK - 2020.06.27 Step 에 End Reason 을 추가하여 역추적하게 하자
	CMgrSchedule* lpScheduleInfo = pChannel->GetMgr();

	CList< CCycle*, CCycle* >& listCycle = lpScheduleInfo->GetCycleList();
	int cycleNum = pCommData->GetCycleNumber();

	bool bIsScheduleComplete;

	if (listCycle.GetCount() == cycleNum)
		bIsScheduleComplete = pCommData->CheckEQScheduleComplete(nStepIndex, nEndReason, nLoopcont, true);
	else
		bIsScheduleComplete = pCommData->CheckEQScheduleComplete(nStepIndex, nEndReason, nLoopcont, false);

	bool bIsPause = pCommData->CheckEQPause(nEndReason);

	nEndReason = pCommData->CalcEndReason(nEndReason, nStepIndex);

	nUIStepNumber = pCommData->CalcUIStepNumber(nStepIndex);

	nUICycleNumber = pCommData->CalcUICycleNumber(nStepIndex);

	fAvgVoltage *= pCommData->IsReverseVoltage();


	if (lpScheduleInfo)
	{
		CStep* lpStep = lpScheduleInfo->GetAtStep(nStepIndex);

		if (lpStep)
		{
			lpStep->SetStepEndReason(nEndReason);
		}
	}
	
	pCommData->SetScheduleComplete(bIsScheduleComplete); //taek 수정

	pCommData->SetLogTime(pComm->strTime);

	pCommData->SetCommCommand(nCommand);

	pCommData->SetOPCode(nOpMode);

	pCommData->SetDataFrameCount(nFlameCnt);

	pCommData->CalcRunningTime(nStepIndex, bIsStepChange, bIsPause);

	m_nUIChangeBefore = pCommData->GetStepNumber(); //taek sk

	pCommData->SetStepNumber(nUIStepNumber);		

	pCommData->SetCycleNumber(nUICycleNumber);

	pCommData->SetCycleIndex/*this is loop count*/(nLoopcont);

	pCommData->SetStepIndex(nStepIndex);

	if (pCommData->GetSystemSafetyEndReason() > 0)
	{
		pCommData->SetEndReason(pCommData->GetSystemSafetyEndReason());
	}
	else
	{
		pCommData->SetEndReason(nEndReason);
	}

	pCommData->SetRestVoltage(fAvgVoltage);

	//Contact Registant Mode
	if (nCommand == 0x93)
	{
		pCommData->SetPogoOhmMonitoring(fAvgAmpere);			// Contact Test 인 경우 전류값이 저항값으로 받음 
		pCommData->SetContactRegUptate(true);

		pCommData->PreSetVoltage(pCommData->GetVoltage());
		pCommData->SetVoltage(fAvgVoltage);

		pCommData->SetPreAmpere(pCommData->GetAmpere());
		pCommData->SetAmpere(fAvgAmpere);
	}
	else
	{
		pCommData->PreSetVoltage(pCommData->GetVoltage());
		pCommData->SetVoltage(fAvgVoltage);

		//220216 YGY Formation OpCode가 3이상일경우만 표시 (FW 요청)
		if (pCommData->GetOPCode() <= eIFBoardOpCodeOutputReplyOn && CMgrConfig::GetMgr()->GetDebugLogUse() != true)
		{
			pCommData->SetPreAmpere(pCommData->GetAmpere());
			pCommData->SetAmpere(0);
		}
		else
		{
			pCommData->SetPreAmpere(pCommData->GetAmpere());
			pCommData->SetAmpere(fAvgAmpere);
		}
	}


	pCommData->SetPreTemp(pCommData->GetTemp());
	pCommData->SetTemp(fTemp);

	//220216 YGY Formation OpCode가 3이상일경우만  표시 (FW 요청)
	if (pCommData->GetOPCode() <= eIFBoardOpCodeOutputReplyOn && CMgrConfig::GetMgr()->GetDebugLogUse() != true)
	{
		pCommData->SetStepAmpereHour(0, FuncAgrsEqual);
	}
	else
	{
		pCommData->SetStepAmpereHour(fAvgCapacity, FuncAgrsEqual);
	}
	pCommData->ConversionCommData(bIsStepChange, bIsCycleChange);

	pCommData->CalcAmpere(nStepIndex);

	//pCommData->CalcDCIR();

	pCommData->CalcStepDCIR(nStepIndex);

	pCommData->SetProcessStatuInt(pChannel->GetProcessStatusInt());

	// CommData 에 추가

	pCommData->SetUserID(pChannel->GetUserID());
	pCommData->SetCellID(pChannel->GetCellID());
	pCommData->SetWorkName(pChannel->GetWorkName());

	//channel
	pChannel->SetIsStepChange(bIsStepChange);

	//채널에서 사이클 바꿔줌
	pChannel->SetIsCycleChange(bIsCycleChange);

	if (pCommData->GetEndReason() != 14 && nStepIndex > 0)
	{
		pChannel->SetStepNumber(nUIStepNumber);
		pChannel->SetCycleNumber(nUICycleNumber);
	}

	pChannel->SetTotalCycleLoop(nLoopcont, nStepIndex, bIsCycleChange);

	pChannel->SetCycleIndex/*this is loop count*/(nLoopcont);

	pChannel->SetOperationMode(nOpMode);

	pChannel->SetScheduleMode(nCommand);

	//Channel 스텝이 멀로 끝낫는지 기록
	pChannel->SetEndCondition();

	pChannel->SetSoc();

	pChannel->SetSoH();

	pChannel->SetSoH2();

#if 0
	if (bIsStepChange != 0)
		TRACE(_T("%d  ch%d	step%d	EndR%d\n"), bIsStepChange, nChannelNum, nStepIndex, nEndReason);
#endif

	//온도 및 Step 동기화
	if (true == pChannel->GetStepSync() || true == pChannel->GetTempSync())
	{
		if (nEndReason == 17 || nEndReason == 18) // Step 시작 과 끝에서 Pause 상태 일때 날라오는 EndReason
		{
			pChannel->StartTempSync(nEndReason, nChannelNum, nStepIndex);
			//스텝 동기화 모드
			pChannel->StartStepSync(nEndReason, nChannelNum, nStepIndex);
		}
		else
		{
			//주기적으로 Chamber제어하기 위해서 Step을 날려준다. (고객요청 사항으로 수동으로 온도 바꾸는거 방지)
			if (bIsScheduleComplete == true)
			{
				pChannel->SendChamberControl(nStepIndex, eChamberOff);
			}
			else if (pChannel->GetScheduleStart() == TRUE)
			{
				pChannel->SendChamberControl(nStepIndex, eChamberSet);
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////

	pChannel->CheckFlowStatus(pCommData);

	pChannel->FinalUpdate(pCommData);

	pChannel->SetFlagSendCommandJump(bIsStepChange, FALSE);

	pChannel->ScheduleFinish(bIsScheduleComplete, nEndReason);

	//this->CheckStepCondition(nChannelNum, nStepIndex);
	this->CheckCondition(nChannelNum, nStepIndex, bIsCycleChange);

	pChannel->ProcessingResultFile(bIsPause, pCommData);

	// 어디선가는 예약 정지, 종료에 대한 부분을 책임져야 한다.
	{
		if (pChannel->IsReservedComplete())
		{
			if (lpScheduleInfo)
			{
				CStep* lpStep = lpScheduleInfo->GetAtStep(pChannel->GetReserveCompleteCycle(), pChannel->GetReserveCompleteStep());

				if (lpStep)
				{
					if (lpStep->GetStepIndex() == nStepIndex)
					{
						pChannel->SendPause(eIFBoardPauseCommandComplete);

						// 음 보내고 나서 Ack 체크하고 초기화 해야 하지 않으까??
						pChannel->SetReserveComplete(0, 0);
					}
				}
			}
		}

		if (pChannel->IsReservedPause())
		{
			if (lpScheduleInfo)
			{
				CStep* lpStep = lpScheduleInfo->GetAtStep(pChannel->GetReservePauseCycle(), pChannel->GetReservePauseStep());

				if (lpStep)
				{
					if (lpStep->GetStepIndex() == nStepIndex)
					{
						pChannel->SendPause(eIFBoardPauseCommandPause);

						// 음 보내고 나서 Ack 체크하고 초기화 해야 하지 않으까??
						pChannel->SetReservePause(0, 0);
					}
				}
			}
		}
	}
}

void	CCommEthernet::ParseBoardDataFrame(const TEthernet* pComm)
{
	int		nCommand = -1;
	int		nOpMode = -1;
	int		nStepMode = -1;
	int		nSamplingTime = -1;
	int		nStepIndex = -1;
	int		nChannelNum = -1;
	int		nEndReason = 0;
	int		nLoopcont = 0;
	int		nTimeSpan1 = 0;
	int		nTimeSpan2 = 0;
	int		nTimeSpan3 = 0;
	int		nTimeSpan4 = 0;
	int		nFlameCnt = 0;

	float	fAvgVoltage = 0.0;
	float	fAvgAmpere = 0.0;
	float	fAvgCapacity = 0.0; //taek
	float	fStepEndValue = 0.0;

	float*	pRawVoltage = NULL;
	float*	pRawAmpere = NULL;
	float*	pRawCapacity = NULL; //taek

	CChannel*	pChannel = NULL;
	CCommData*	pCommData = NULL;

	nFlameCnt = (int)(pComm->pszComm[DATA_COUNT1] << 8 | pComm->pszComm[DATA_COUNT2]);
	nFlameCnt -= 6;  // 왜 6바이트를 뺄까? Stx, length, channel index 각 2byte씩 6바이트를 제외한 길이이다. 
					 // Byte #7 ~ Byte #44 까지가  1ms 단위로 100개 들어온다.
	nFlameCnt /= FRAME_GAP_DATA;//38;

	int nBoardIndex = pComm->pszComm[DATA_CHANNEL1] << 8 | pComm->pszComm[DATA_CHANNEL2];
	nChannelNum = CMgrComm::GetMgr()->GetChannelNumber(eDeviceTypeIFBoard, pComm->nBoardNumber, nBoardIndex);
	pCommData = CMgrCommData::GetMgr()->GetAt(nChannelNum);
	pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNum);

	CStep* pStep = pChannel->GetMgr()->GetAtStep(pChannel->GetCycleNumber(), pChannel->GetStepNumber());

	if (pCommData == NULL || pChannel == NULL)
	{
		SYSLOG(Dbg, "[PK_ERROR]  pCommData, pChannel is null return");
		return;
	}

	if (FALSE == m_bFirstReceive)
	{
		CMgrChannel::GetMgr()->SendInitial(pComm->nBoardNumber);
		m_bFirstReceive = TRUE;
	}

	if (nFlameCnt < 0)
	{
		SYSLOG(Dbg, "[PK_ERROR] flmecnt(%d) return", nFlameCnt);
		return;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// IF 통신상태 확인 2021-02-04 CNZ
	// 일단 조건 2개만 .필요시 상태 조건 추가.



	pRawVoltage = new float[nFlameCnt];
	pRawAmpere = new float[nFlameCnt];
	pRawCapacity = new float[nFlameCnt]; //taek 추가


	int nGarbegeVoltCount = nFlameCnt / 2;		//오창2공장 이슈, FW팀 요청으로 50ms 전압 버림
	int nGarbegeAmpereCount = nFlameCnt * 3 / 10;	//오창2공장 이슈, FW팀 요청으로 30ms 전류 버림

	for (int i = 0; i < nFlameCnt; ++i)
	{
		//Sampling Time 데이터 
		int nRowDataSmpTime = (int)(pComm->pszComm[DATA_TIME_PERIOD1 + (i*FRAME_GAP_DATA)] << 8 | pComm->pszComm[DATA_TIME_PERIOD2 + (i*FRAME_GAP_DATA)]);

		switch (nRowDataSmpTime)
		{
		case 1: nSamplingTime = 1;		break;
		case 2: nSamplingTime = 10;		break;
		case 3: nSamplingTime = 100;	break;
		case 4: nSamplingTime = 1000;	break;
		}

		nStepIndex = (int)(pComm->pszComm[DATA_STEP_INDEX1 + (i*FRAME_GAP_DATA)] << 8 | pComm->pszComm[DATA_STEP_INDEX2 + (i*FRAME_GAP_DATA)]);
		nTimeSpan1 = (int)(pComm->pszComm[DATA_TIME_SPAN_LSB1 + (i*FRAME_GAP_DATA)] << 8 | pComm->pszComm[DATA_TIME_SPAN_LSB2 + (i*FRAME_GAP_DATA)]);
		nTimeSpan2 = (int)(pComm->pszComm[DATA_TIME_SPAN_LSB3 + (i*FRAME_GAP_DATA)] << 8 | pComm->pszComm[DATA_TIME_SPAN_LSB4 + (i*FRAME_GAP_DATA)]);
		nTimeSpan3 = (int)(pComm->pszComm[DATA_TIME_SPAN_MSB1 + (i*FRAME_GAP_DATA)] << 8 | pComm->pszComm[DATA_TIME_SPAN_MSB2 + (i*FRAME_GAP_DATA)]);
		nTimeSpan4 = (int)(pComm->pszComm[DATA_TIME_SPAN_MSB3 + (i*FRAME_GAP_DATA)] << 8 | pComm->pszComm[DATA_TIME_SPAN_MSB4 + (i*FRAME_GAP_DATA)]);
		nLoopcont = (int)(pComm->pszComm[(DATA_LOOP_CNT1)+(i*FRAME_GAP_DATA)] << 8 | pComm->pszComm[(DATA_LOOP_CNT2)+(i*FRAME_GAP_DATA)]);
		nEndReason = (int)(pComm->pszComm[DATA_END_REASON1 + (i*FRAME_GAP_DATA)] << 8 | pComm->pszComm[DATA_END_REASON2 + (i*FRAME_GAP_DATA)]);

		nCommand = (int)(pComm->pszComm[DATA_COMMAND1 + (i*FRAME_GAP_DATA)] << 8 | pComm->pszComm[DATA_COMMAND2 + (i*FRAME_GAP_DATA)]);
		nStepMode = (int)(pComm->pszComm[DATA_MODE1 + (i*FRAME_GAP_DATA)] << 8 | pComm->pszComm[DATA_MODE2 + (i*FRAME_GAP_DATA)]);
		nOpMode = (int)(pComm->pszComm[DATA_OP1 + (i*FRAME_GAP_DATA)] << 8 | pComm->pszComm[DATA_OP2 + (i*FRAME_GAP_DATA)]);

		fStepEndValue = (float)(pComm->pszComm[DATA_END_VALUE1 + (i*FRAME_GAP_DATA)] << 24 |
			pComm->pszComm[DATA_END_VALUE2 + (i*FRAME_GAP_DATA)] << 16 |
			pComm->pszComm[DATA_END_VALUE3 + (i*FRAME_GAP_DATA)] << 8 |
			pComm->pszComm[DATA_END_VALUE4 + (i*FRAME_GAP_DATA)]);

		if (CMgrConfig::GetMgr()->GetMicroV()) //taek 추가 0319
			fStepEndValue *= 0.0001f;
		else
			fStepEndValue *= 0.001f;

		long lVoltage = pComm->pszComm[DATA_VOLTAGE1 + (i*FRAME_GAP_DATA)] << 24 |
			pComm->pszComm[DATA_VOLTAGE2 + (i*FRAME_GAP_DATA)] << 16 |
			pComm->pszComm[DATA_VOLTAGE3 + (i*FRAME_GAP_DATA)] << 8 |
			pComm->pszComm[DATA_VOLTAGE4 + (i*FRAME_GAP_DATA)];


		if (pChannel->GetEqMode() == EqModeSerial)
		{
			pRawVoltage[i] = (float)lVoltage;
			if (CMgrConfig::GetMgr()->GetMicroV())
				pRawVoltage[i] *= 0.0001f;
			else
				pRawVoltage[i] *= 0.001f;
			// 첫 50ms Voltage 버림
			if (i >= nGarbegeVoltCount)
			{
				fAvgVoltage += pRawVoltage[i];
			}
		}
		else if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (CMgrConfig::GetMgr()->GetMicroV())
			{
				fAvgVoltage = (float)lVoltage * 0.0001f;
			}
			else
			{
				fAvgVoltage = (float)lVoltage * 0.001f;
			}
		}

		long lAmpere = pComm->pszComm[DATA_AMPERE1 + (i*FRAME_GAP_DATA)] << 24 |
			pComm->pszComm[DATA_AMPERE2 + (i*FRAME_GAP_DATA)] << 16 |
			pComm->pszComm[DATA_AMPERE3 + (i*FRAME_GAP_DATA)] << 8 |
			pComm->pszComm[DATA_AMPERE4 + (i*FRAME_GAP_DATA)];


		if (pChannel->GetEqMode() == EqModeSerial)
		{
			pRawAmpere[i] = (float)(lAmpere);
			if (CMgrConfig::GetMgr()->GetMicroA())
				pRawAmpere[i] *= 0.00001f;
			else
				pRawAmpere[i] *= 0.001f;

			if (i >= nGarbegeAmpereCount)
			{
				fAvgAmpere += pRawAmpere[i];
			}
		}
		else if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (CMgrConfig::GetMgr()->GetMicroA())
				fAvgAmpere = (float)(lAmpere) *  0.00001f;
			else
				fAvgAmpere = (float)(lAmpere) * 0.001f;
		}

		long lCapacity = pComm->pszComm[DATA_CAPACITY1 + (i*FRAME_GAP_DATA)] << 24 |
			pComm->pszComm[DATA_CAPACITY2 + (i*FRAME_GAP_DATA)] << 16 |
			pComm->pszComm[DATA_CAPACITY3 + (i*FRAME_GAP_DATA)] << 8 |
			pComm->pszComm[DATA_CAPACITY4 + (i*FRAME_GAP_DATA)]; //taek 추가

		pRawCapacity[i] = (float)(lCapacity);
		pRawCapacity[i] *= 0.0001f;
		fAvgCapacity = pRawCapacity[i]; //taek 추가
	}

	pChannel->SetLastAckTime(::GetTickCount64() / 1000);
	ULONGLONG a = (nTimeSpan1 << 24) | (nTimeSpan2 << 16) | (nTimeSpan3 << 8) | nTimeSpan4;
	pChannel->SetBoardTime((nTimeSpan1 << 24) | (nTimeSpan2 << 16) | (nTimeSpan3 << 8) | nTimeSpan4);

	if (pChannel->GetEqMode() == EqModeSerial)
	{
		fAvgVoltage /= (nFlameCnt - nGarbegeVoltCount);
		fAvgVoltage = ROUNDING(fAvgVoltage, 4);

		fAvgAmpere /= ((nFlameCnt)-nGarbegeAmpereCount);

		fAvgAmpere = ROUNDING(fAvgAmpere, 5);
	}

	////pnx_hr dcir 평균전류 
	CString strDcirTIme = CMgrConfig::GetMgr()->m_strDcirTime;
	if (!strDcirTIme.IsEmpty())
	{
		pCommData->m_fStepTotalAmpere += fAvgAmpere;
	}

	//debug start
	//original fw data
	CString strVolDebug, strCurDebug;
	strVolDebug.Format("%.3f", fAvgVoltage);
	strCurDebug.Format("%.3f", fAvgAmpere);

	//승온 조건 일 때와 아닐 때의 구분

	//if (nStepIndex > 0 && false == pChannel->GetScheduleStart())
	//{
	//	if (true == pChannel->GetTempSync() && pChannel->GetEndSync() == TRUE) //딜레이 일때
	//	{
	//		pChannel->SetChannelStatus(eCyclerChannelStatePauseFromDwell);
	//	}
	//	else if (true == pChannel->GetStepSync())//else if (true == pChannel->GetTempSync()) //스텝	
	//	{
	//		pChannel->SetChannelStatus(eCyclerChannelStatePauseFromStepSync);
	//	}
	//	else if (true == pChannel->GetTempSync() && false == pChannel->GetChamberElevatedTemperature()) //온도
	//	{
	//		pChannel->SetChannelStatus(eCyclerChannelStatePauseFromTempSync);
	//	}
	//	else if (true == pChannel->GetTempSync() && true == pChannel->GetChamberElevatedTemperature()) //승온
	//	{
	//		pChannel->SetChannelStatus(eCyclerChannelStatePauseFromWaitChamber);
	//	}
	//}







	//if (nStepIndex > 0/* && pCommData->GetStepTime() % 2 == 0*/)
	if ((pCommData->GetEndReason() > 0 || nEndReason > 0) && nEndReason != m_nOldEndReason)
	{
		SYSLOG(Dbg, "[rcv] ch(%d)  step(%d)  endr(%d)  rcvtm(%s)  cmd(%x)  opmode(%d)  stepmode(%d)  volt(%s)  cur(%s)  tmsp4(%d)  frame(%d), loop(%d), endreason(%d)",
			nChannelNum + 1, nStepIndex, nEndReason, pComm->strTime, nCommand, nOpMode, nStepMode, strVolDebug, strCurDebug, nTimeSpan4, nFlameCnt, nLoopcont, pCommData->GetEndReason());

		m_nOldEndReason = nEndReason;
		//	TRACE("StepIndex(%d)\n", nStepIndex);
	}
	/*TRACE()*/
//end


//update 이전 현재 sw가 계산 해줘야 하는 것들 (즉 나중에 fw개선 필요한 것들)
	bool blsUIStepChange = false;
	int  nUIStepNumber = 0;				//XML 1base
	int  nUICycleNumber = 0;			//XML 1base 

	nLoopcont = pCommData->CalcLoopCount(nEndReason, nStepIndex, nLoopcont);

	bool bIsStepChange = pCommData->CheckEQStepChange(nStepIndex, nLoopcont);

	bool bIsCycleChange = pCommData->CheckEQCycleChange(nStepIndex);

	if (bIsStepChange)
	{
		pCommData->RefreshMsgCount();
		m_nOldEndReason = 0;
	}
	// Add By JSK - 2020.06.27 Step 에 End Reason 을 추가하여 역추적하게 하자
	CMgrSchedule* lpScheduleInfo = pChannel->GetMgr();

	CList< CCycle*, CCycle* >& listCycle = lpScheduleInfo->GetCycleList();
	int cycleNum = pCommData->GetCycleNumber();

	bool bIsScheduleComplete;

	if (listCycle.GetCount() == cycleNum)
		bIsScheduleComplete = pCommData->CheckEQScheduleComplete(nStepIndex, nEndReason, nLoopcont, true);
	else
		bIsScheduleComplete = pCommData->CheckEQScheduleComplete(nStepIndex, nEndReason, nLoopcont, false);

	bool bIsPause = pCommData->CheckEQPause(nEndReason);

	nEndReason = pCommData->CalcEndReason(nEndReason, nStepIndex);

	nUIStepNumber = pCommData->CalcUIStepNumber(nStepIndex);

	nUICycleNumber = pCommData->CalcUICycleNumber(nStepIndex);

	fAvgVoltage *= pCommData->IsReverseVoltage();


	if (lpScheduleInfo)
	{
		CStep* lpStep = lpScheduleInfo->GetAtStep(nStepIndex);

		if (lpStep)
		{
			lpStep->SetStepEndReason(nEndReason);
		}
	}

	//if (pChannel->GetScheduleStart() == TRUE && pStep->GetStepIndex() != nStepIndex && pChannel->GetPause() == FALSE)
	//{
	//	pChannel->SendChamberControl(nStepIndex, false);
	//}
	///////////////////////////////////////////////////////////////////

	pCommData->SetScheduleComplete(bIsScheduleComplete); //taek 수정

	pCommData->SetLogTime(pComm->strTime);

	pCommData->SetCommCommand(nCommand);

	pCommData->SetOPCode(nOpMode);

	pCommData->SetDataFrameCount(nFlameCnt);

	pCommData->CalcRunningTime(nStepIndex, bIsStepChange, bIsPause);

	m_nUIChangeBefore = pCommData->GetStepNumber(); //taek sk

	pCommData->SetStepNumber(nUIStepNumber);

	/*static bool first = false;
	if (first) {
		CString specificationPath;
		specificationPath.Format(_T("%s\\Config\\Specification\\CH%02d.ini"), GetExePath(), nChannelNum + 1);

		CString endCondition = INIReadStr("Specification", "EndCondition", specificationPath);

		if (!endCondition.IsEmpty()) {
			if (nUIStepNumber == m_nUIChangeBefore + 1)
			{
				blsUIStepChange = true;
				pCommData->RefreshMsgCount();
			}
			else
			{
				blsUIStepChange = false;
			}
		}
		first = false;
	}*/

	pCommData->SetCycleNumber(nUICycleNumber);

	pCommData->SetCycleIndex/*this is loop count*/(nLoopcont);

	pCommData->SetStepIndex(nStepIndex);

	if (pCommData->GetSystemSafetyEndReason() > 0)
	{
		pCommData->SetEndReason(pCommData->GetSystemSafetyEndReason());
	}
	else
	{
		pCommData->SetEndReason(nEndReason);
	}

	pCommData->SetRestVoltage(fAvgVoltage);

	pCommData->PreSetVoltage(pCommData->GetVoltage());
	pCommData->SetVoltage(fAvgVoltage);

	pCommData->SetPreAmpere(pCommData->GetAmpere());
	pCommData->SetAmpere(fAvgAmpere);

	pCommData->SetStepAmpereHour(fAvgCapacity, FuncAgrsEqual);

	pCommData->ConversionCommData(bIsStepChange, bIsCycleChange);
	//pCommData->ConversionCommData(blsUIStepChange, bIsCycleChange); //taek sk ui 상 스텝 변화 하는 부분 판단

	pCommData->CalcAmpere(nStepIndex);

	//pCommData->CalcDCIR();

	pCommData->CalcStepDCIR(nStepIndex);

	pCommData->SetProcessStatuInt(pChannel->GetProcessStatusInt());

	// CommData 에 추가

	pCommData->SetUserID(pChannel->GetUserID());
	pCommData->SetCellID(pChannel->GetCellID());
	pCommData->SetWorkName(pChannel->GetWorkName());

	//channel
	pChannel->SetIsStepChange(bIsStepChange);

	//채널에서 사이클 바꿔줌
	pChannel->SetIsCycleChange(bIsCycleChange);

	if (pCommData->GetEndReason() != 14 && nStepIndex > 0)
	{
		pChannel->SetStepNumber(nUIStepNumber);
		pChannel->SetCycleNumber(nUICycleNumber);
	}




	pChannel->SetTotalCycleLoop(nLoopcont, nStepIndex, bIsCycleChange);

	pChannel->SetCycleIndex/*this is loop count*/(nLoopcont);

	pChannel->SetOperationMode(nOpMode);

	pChannel->SetScheduleMode(nCommand);

	//Channel 스텝이 멀로 끝낫는지 기록
	pChannel->SetEndCondition();

	pChannel->SetSoc();

	pChannel->SetSoH();

	pChannel->SetSoH2();

#if 0
	if (bIsStepChange != 0)
		TRACE(_T("%d  ch%d	step%d	EndR%d\n"), bIsStepChange, nChannelNum, nStepIndex, nEndReason);
#endif

	//온도 및 Step 동기화
	if (true == pChannel->GetStepSync() || true == pChannel->GetTempSync())
	{
		if (nEndReason == 17 || nEndReason == 18) // Step 시작 과 끝에서 Pause 상태 일때 날라오는 EndReason
		{
			pChannel->StartTempSync(nEndReason, nChannelNum, nStepIndex);
			//스텝 동기화 모드
			pChannel->StartStepSync(nEndReason, nChannelNum, nStepIndex);
		}
		else
		{
			//주기적으로 Chamber제어하기 위해서 Step을 날려준다. (고객요청 사항으로 수동으로 온도 바꾸는거 방지)
			if (bIsScheduleComplete == true)
			{
				pChannel->SendChamberControl(nStepIndex, eChamberOff);
			}
			else if (pChannel->GetScheduleStart() == TRUE)
			{
				pChannel->SendChamberControl(nStepIndex, eChamberSet);
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////

	pChannel->CheckFlowStatus(pCommData);

	pChannel->FinalUpdate(pCommData);

	pChannel->SetFlagSendCommandJump(bIsStepChange, FALSE);

	if (bIsScheduleComplete == true)
	{
		int i = 0;
	}
	pChannel->ScheduleFinish(bIsScheduleComplete, nEndReason);

	//this->CheckStepCondition(nChannelNum, nStepIndex);
	this->CheckCondition(nChannelNum, nStepIndex, bIsCycleChange);

	pChannel->ProcessingResultFile(bIsPause, pCommData);

	delete[] pRawVoltage;
	delete[] pRawAmpere;
	delete[] pRawCapacity; //taek 추가

	// 어디선가는 예약 정지, 종료에 대한 부분을 책임져야 한다.
	{
		if (pChannel->IsReservedComplete())
		{
			if (lpScheduleInfo)
			{
				CStep* lpStep = lpScheduleInfo->GetAtStep(pChannel->GetReserveCompleteCycle(), pChannel->GetReserveCompleteStep());

				if (lpStep)
				{
					if (lpStep->GetStepIndex() == nStepIndex)
					{
						pChannel->SendPause(eIFBoardPauseCommandComplete);

						// 음 보내고 나서 Ack 체크하고 초기화 해야 하지 않으까??
						pChannel->SetReserveComplete(0, 0);
					}
				}
			}
		}

		if (pChannel->IsReservedPause())
		{
			if (lpScheduleInfo)
			{
				CStep* lpStep = lpScheduleInfo->GetAtStep(pChannel->GetReservePauseCycle(), pChannel->GetReservePauseStep());

				if (lpStep)
				{
					if (lpStep->GetStepIndex() == nStepIndex)
					{
						pChannel->SendPause(eIFBoardPauseCommandPause);

						// 음 보내고 나서 Ack 체크하고 초기화 해야 하지 않으까??
						pChannel->SetReservePause(0, 0);
					}
				}
			}
		}
	}
}


//해당 IF보드 엔드 처리
void	CCommEthernet::ParseBoardEndFrame(const TEthernet* pComm)
{
	BYTE bData = pComm->pszComm[END_CHANNEL1] << 8 | pComm->pszComm[END_CHANNEL2];

	CArray< int, int > arr;

	CPnxUtil::GetMgr()->SplitByte(bData, arr);

	int nChannelNumber = 0;
	for (int i = 0; i < arr.GetSize(); ++i)
	{
		nChannelNumber = CalcChannelNumber(arr.GetAt(i), pComm->nBoardNumber);
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);
		if (pChannel)
		{
			pChannel->SetChannelEnd(TRUE);
			SYSLOG(Dbg, "[END EHCO][BOARD:%d][CH:0x%02x][CH:%d] End Frame Receive", pComm->nBoardNumber, bData, nChannelNumber + 1);
		}
	}
}

//I/F 보드로 알람 정보 가져와 데이터 가공 하는 곳
void	CCommEthernet::ParseBoardAlarmFrame(const TEthernet* pComm)
{
	BYTE bData = pComm->pszComm[ALRAM_CHANNEL1] << 8 | pComm->pszComm[ALRAM_CHANNEL2];

	CArray< int, int > arr;

	CPnxUtil::GetMgr()->SplitByte(bData, arr);

	int nIndex = 0;

	CString strAlarmText;

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nChannelNumber = CalcChannelNumber(arr.GetAt(i), pComm->nBoardNumber);

		for (int j = 0; j < eAlarmTypeMaxCount; ++j)
		{
			//Channel :4,5
			//Step : 6,7			
			//Waring : 8,9			CanAlarmWaring, IFAlarmWaring,	SerialAlarmWaring	16bit
			//Grid : 10,11			CanAlarmGrid,	IFAlarmGrid,	SerialAlarmGrid		16bit		
			//DC : 12,13			CanAlarmDC,		IFAlarmDC,		SearialAlarmDC		16bit
			//Fault : 14,15			CanAlarmFault,	IFAlarmFault,	SerialAlarmFault	16bit
			//알람 상태 정보 불러와서 알람 상태 플래그 활성화 또는 비활성화
			CString str = CMgrAlarm::GetMgr()->AddAlarm(nChannelNumber, j, (BYTE)pComm->pszComm[ALRAM_WARING2 + (j * 2 + nIndex)], (BYTE)pComm->pszComm[ALRAM_WARING1 + (j * 2 + nIndex)]);

			if (FALSE == str.IsEmpty() && TRUE == strAlarmText.IsEmpty())
				strAlarmText.Format("%s", str.GetString());
		}

		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);
		CCommData*	pCommData = NULL;
		pCommData = CMgrCommData::GetMgr()->GetAt(nChannelNumber);
		//채널 정보 존재시
		if (pChannel)
		{
			CAlarmMap* pMapAlarm = CMgrAlarm::GetMgr()->GetAlarmMap(nChannelNumber);
			//알람 맵 정보 존재시 
			if (pMapAlarm)
			{
				if (TRUE == pMapAlarm->ExistAlarm())
				{
					pChannel->SetChannelStatus(eCyclerChannelStateError);
					pChannel->SetScheduleStart(FALSE);
				}
				else
				{
					//220302 YGY 현재 Alarm 정보도 실시간으로 계속 FW에 통해서 받는 구조로 변경 하여 아래조건에서의 동작 주석처리
					//구조 변경 이유 : 알람 발생 후 알람 클리어 상태 확인을 위해서 FW 구조 변경
					if (false == pChannel->GetScheduleStart())
					{
						

					}
				}

				//알람 창 띄우기
				CWnd* pWnd = AfxGetApp()->GetMainWnd();

				if (pWnd)
					::PostMessage(pWnd->GetSafeHwnd(), WM_UPDATE_VIEW_ALARM_STATUS, 0, 0);
			}
		}

		nIndex += 8;

		CString strDbug;
		strDbug.Format("");

		int nFlameCnt = (int)(pComm->pszComm[2] << 8 | pComm->pszComm[3]);

		CString strTemp;
		for (int j = 0; j < nFlameCnt; ++j)
		{
			strTemp.Format("0x%02x   ", pComm->pszComm[j]);
			strDbug += strTemp;
		}
		if (TRUE != strAlarmText.IsEmpty())
		{
			SYSLOG(Dbg, "[RECV_BOARD_TYPE_A][BOARD:%d][CH:0x%02x][CH:%d] %s", pComm->nBoardNumber, bData, nChannelNumber + 1, strDbug.GetString());
		}
		//send to server test start
		TCHAR szBuff[4096];
		memset(szBuff, 0x00, sizeof(szBuff));
		memcpy(szBuff, pComm->pszComm, nFlameCnt + 2);
		szBuff[ALRAM_CHANNEL1] = (TCHAR)nChannelNumber;
		szBuff[ALRAM_CHANNEL2] = 0;
		szBuff[ALRAM_END1] = '$';
		szBuff[ALRAM_END2] = '$';
	}
}

//I/F 보드 Start 신호 
void	CCommEthernet::ParseBoardStart(const TEthernet* pComm)
{
	BYTE bData = pComm->pszComm[4] << 8 | pComm->pszComm[5];

	CArray< int, int > arr;

	CPnxUtil::GetMgr()->SplitByte(bData, arr);

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nChannelNumber = CalcChannelNumber(arr.GetAt(i), pComm->nBoardNumber);

		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

		if (pChannel)
		{
			pChannel->SetScheduleStart(true);
			pChannel->SetChannelStatus(eCyclerChannelStateStart);
		}

		SYSLOG(Dbg, "[STRAT EHCO][BOARD:%d][CH:0x%02x][CH:%d] Start Receive Time(%s)", pComm->nBoardNumber, bData, nChannelNumber + 1, pComm->strTime);
	}
}

//I/F 보드 건너뛰기
void	CCommEthernet::ParseBoardJump(const TEthernet* pComm)
{
	BYTE bData = pComm->pszComm[4] << 8 | pComm->pszComm[5];

	int nStepIndex = pComm->pszComm[6] << 8 | pComm->pszComm[7] & 0xFF;

	CArray< int, int > arr;

	CPnxUtil::GetMgr()->SplitByte(bData, arr);

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nChannelNumber = CalcChannelNumber(arr.GetAt(i), pComm->nBoardNumber);

		SYSLOG(Dbg, "[JUMP EHCO][BOARD:%d][CH:0x%02x][CH:%d] Receive StepIndex(%d), Time(%s)", pComm->nBoardNumber, bData, nChannelNumber + 1, nStepIndex, pComm->strTime);
	}
}

//I/F 보드 병렬 모드 직렬모드 명령어 
void	CCommEthernet::ParseBoardCommand(const TEthernet* pComm)
{
	BYTE bData = pComm->pszComm[4] << 8 | pComm->pszComm[5];

	int nCommand = pComm->pszComm[6] << 8 | pComm->pszComm[7] & 0xFF;

	CArray< int, int > arr;

	CPnxUtil::GetMgr()->SplitByte(bData, arr);

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nChannelNumber = CalcChannelNumber(arr.GetAt(i), pComm->nBoardNumber);


		SYSLOG(Dbg, "[COMMAND EHCO][BOARD:%d][CH:0x%02x][CH:%d] 0x%02x Time(%s)", pComm->nBoardNumber, bData, nChannelNumber + 1, nCommand, pComm->strTime);

		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);
		if (pChannel == NULL)
		{
			SYSLOG(Dbg, _T("%s channel is null, return"), __FUNCTION__);
			return;
		}

		CMgrConfig* pMgr = CMgrConfig::GetMgr();
		int nChannelCountInParallel = pMgr->GetParallelChannelCount(); // 병렬에 필요한 Channel 갯수 마스터 + 슬레이브

		if (nCommand == EthernetCmdTypeModeSerial)
		{
			pChannel->SetMasterChannel(ParallelMaster);
			//병렬 관련 전류 및 전압 I/O 제어.
			if (pChannel->GetEqMode() == EQModeParallel) // 모드 변할 때만 딱 한번 제어
			{
				//병렬 I/O No 0~ 15, 32 ~ 48 
				if (pChannel->GetChannelNumber() < 16)
				{
					CMgrDio::GetMgr()->SetDIO(pChannel->GetChannelNumber(), FALSE); //Mode Relay Off
				}
				else
				{
					CMgrDio::GetMgr()->SetDIO(pChannel->GetChannelNumber() + 16, FALSE); //Mode Relay Off
				}
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////

			pChannel->SetEqMode(EqModeSerial);
		}

		if (nCommand == EthernetCmdTypeModeParallel)
		{
			//병렬 관련 전류 및 전압 I/O 제어.
			if (pChannel->GetEqMode() == EqModeSerial) // 모드 변할 때만 딱 한번 제어
			{
				//병렬 I/O No 0~ 15, 32 ~ 48 
				if (pChannel->GetChannelNumber() < 16)
				{
					CMgrDio::GetMgr()->SetDIO(pChannel->GetChannelNumber(), TRUE); //Mode Relay On
				}
				else
				{
					CMgrDio::GetMgr()->SetDIO(pChannel->GetChannelNumber() + 16, TRUE); //Mode Relay On
				}
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////

			pChannel->SetEqMode(EQModeParallel);

			if (nChannelNumber % nChannelCountInParallel == 0)
			{
				pChannel->SetMasterChannel(ParallelMaster);
			}
			else
			{
				pChannel->SetMasterChannel(ParallelSlave);
			}
		}
	}
}

//I/F Board Pause Command Receive
void	CCommEthernet::ParseBoardPause(const TEthernet* pComm)
{
	BYTE bData = pComm->pszComm[4] << 8 | pComm->pszComm[5];

	int nPause = pComm->pszComm[8] << 8 | pComm->pszComm[9] & 0xFF;

	CArray< int, int > arr;

	CPnxUtil::GetMgr()->SplitByte(bData, arr);

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nChannelNumber = CalcChannelNumber(arr.GetAt(i), pComm->nBoardNumber);

		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

		if (pChannel)
		{
			if (eCyclerChannelStateError != pChannel->GetChannelStatus())
			{
				switch (nPause)
				{
				case 0x01: pChannel->SetChannelStatus(eCyclerChannelStatePause);		pChannel->SetPause(TRUE);										break;
				case 0x02: pChannel->SetScheduleStart(FALSE);	pChannel->SetChannelStart(FALSE);	pChannel->SetChannelEnd(TRUE);	break;
					//Pending Debug 필요
				case 0x03: pChannel->SetChannelStatus(eCyclerChannelStateStart);		pChannel->SetPause(false);	pChannel->SetScheduleStart(TRUE);	pChannel->SetChannelStart(TRUE);	pChannel->SetChannelEnd(FALSE);	break;
				}
			}
		}

		CString strDbug = _T("");
		CString strTemp;
		for (int j = 0; j < 10; ++j)
		{
			strTemp.Format("0x%02x   ", pComm->pszComm[j]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[PAUSE EHCO][BOARD:%d][CH:0x%02x][CH:%d] Time(%s) %s", pComm->nBoardNumber, bData, nChannelNumber + 1, pComm->strTime, strDbug);
	}
}

void	CCommEthernet::CheckCondition(int nChannelNumber, int nStepIndex, bool bIsCycleChange)
{
	if (nStepIndex <= 0)
	{
		return;
	}

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	CChannel* pChannel = pMgr->GetChannel(nChannelNumber);

	if (NULL == pChannel)
	{
		SYSLOG(Dbg, _T("[FATAR ERROR] void	CCommEthernet::CheckStepCondition(int nChannelNumber).. channel is null"));
		return;
	}
	//사이클 완료 한 경우 통합안전 조건 확인
	if (bIsCycleChange)
	{
		pChannel->Check_Cycle_TotalSafetyCondition();
	}
	//시스템 안전 조건, 통합 안전 조건 확인
	pChannel->CheckAllCondition();
}

int		CCommEthernet::CalcChannelNumber(int nChannelNumber, int nBoardIndex)
{
	// Board 0 : 0, 1, 2, 3
	// Board 1 : 4, 5, 6, 7
	// Board 2 : 8, 9, 10, 11
	// Board 3 : 12, 13, 14, 15

	int nRet = (nBoardIndex * 4) + nChannelNumber;

	return nRet;
}


void	CCommEthernet::ParseBoardStatus(const TEthernet* pComm)
{
	BYTE bData = pComm->pszComm[4] << 8 | pComm->pszComm[5];

	int nPause = pComm->pszComm[6] << 8 | pComm->pszComm[7] & 0xFF;

	CArray< int, int > arr;

	CPnxUtil::GetMgr()->SplitByte(bData, arr);

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nChannelNumber = CalcChannelNumber(arr.GetAt(i), pComm->nBoardNumber);

		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

		if (pChannel)
		{
			if (eCyclerChannelStateError != pChannel->GetChannelStatus())
			{
				switch (nPause)
				{
				case 0x01:
					pChannel->SetChannelStatus(eCyclerChannelStatePause);
					pChannel->SetPause(TRUE);
					break;

				}
			}
		}

		CString strDbug = _T("");
		CString strTemp;
		for (int j = 0; j < 10; ++j)
		{
			strTemp.Format("0x%02x   ", pComm->pszComm[j]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[RECEIVE STATUS][BOARD:%d][CH:0x%02x][CH:%d] Time(%s) %s", pComm->nBoardNumber, bData, nChannelNumber + 1, pComm->strTime, strDbug);
	}
}

void CCommEthernet::ReceivePlcInterfaceData(const TEthernet * pComm)
{
	BYTE bData = pComm->pszComm[4] << 8 | pComm->pszComm[5];

	//Vaccum 정수부
	float fVaccumValueMSB = pComm->pszComm[5]-48;
	//Vaccum 소수부
	float fVaccumValueLSB = pComm->pszComm[7] - 48;
	float fVaccumValueLSB2 = pComm->pszComm[8] - 48;

	if (fVaccumValueLSB2 >= 5)
	{
		fVaccumValueLSB = fVaccumValueLSB + 1;
	}
	//Vaccum 단위
	float fVaccumUnit = pComm->pszComm[25] - 48;

	float fVaccumValue = (fVaccumValueMSB + fVaccumValueLSB /10)*pow(10, fVaccumUnit);			//Vacuum Value
	float fVaccumPercent = 0;
	float fVaccumPercent100 =0;
	float fVaccumPercent10 = 0;
	float fVaccumPercent1 = 0;

	if (pComm->pszComm[42] ==100)
	{
		fVaccumPercent = 100;
	}
	else
	{
		fVaccumPercent100 = 0;
		fVaccumPercent10 = (pComm->pszComm[43] - 48);
		fVaccumPercent1 = (pComm->pszComm[44] - 48);
		fVaccumPercent = fVaccumPercent100 * 100 + fVaccumPercent10 * 10 + fVaccumPercent1;
	}
 	
	bool bAutoMode;
	if ((pComm->pszComm[45]) == 0x01)
	{
		bAutoMode = true;
	}
	else
	{
		bAutoMode = false;
	}
	if (pComm == NULL)
		return;
	CArray< int, int > arr;

	CPnxUtil::GetMgr()->SplitByte(bData, arr);

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		//int nChannelNumber = CalcChannelNumber(arr.GetAt(i), pComm->nBoardNumber);

		CMgrPlcInterface* pMgr = CMgrPlcInterface::GetMgr();

		pMgr->Set_VaccumValue(fVaccumValue);
		pMgr->Set_AutoMode(bAutoMode);
		pMgr->Set_PumpOpenPercent(fVaccumPercent);

		CString strDbug = _T("");
		CString strTemp;
		for (int j = 0; j < 10; ++j)
		{
			strTemp.Format("0x%02x   ", pComm->pszComm[j]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[RECEIVE PLC Interface]Time(%s) %s", pComm->strTime, strDbug);
	}
}
