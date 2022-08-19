
#include "stdafx.h"
#include "PnxCycler.h"
#include "CPlcProcessSequence2.h"

#include "MgrStatus.h"
#include "MgrConfig.h"
#include "CMgrModel.h"
#include "CMgrAccount.h"
#include "MgrPCan.h"
#include "MgrAlarm.h"
#include "MgrDio.h"

#include "MgrChannel.h"
#include "CDlgViewPlc2.h"
#include "CDlgViewPlcManual.h"
#include "CDlgViewEqpChannelStatus.h"

//IMPLEMENT_DYNAMIC(CPlcProcessSequence2, CDialogEx)

CPlcProcessSequence2::CPlcProcessSequence2()
{
	m_ePressRunStepCurrent = (enumProcessStepAuto2)AUTO_INIT;

	ZeroMemory(m_PlcAndPcStatus, 12);

	m_bThreadLife = FALSE;

	m_SendBlockTimeCheck = 0;
	m_SendTimeCheck = 0;
	m_ReadTimeCheck = 0;
	m_ReadAlarmCount = 0;
	m_bPLCHeartBitAlarmFlag = FALSE;

	//Test
	//m_RunStopFlag = FALSE;
	m_PlcWaitTime = 0;


}

CPlcProcessSequence2::~CPlcProcessSequence2()
{
}

void CPlcProcessSequence2::ThreadRun(void)
{
	if (m_bThreadLife)
		return;

	m_bThreadLife = true;

	m_pThread = AfxBeginThread(threadFunction, this);
}

void CPlcProcessSequence2::ThreadStop(void)
{
	m_bThreadLife = FALSE;
	//m_pThread = NULL;

	if (m_pThread != NULL)
	{
		DWORD exitcode = ::WaitForSingleObject(m_pThread->m_hThread, 1000);
		if (exitcode == WAIT_TIMEOUT)
			::TerminateThread(m_pThread->m_hThread, exitcode);
		m_pThread = NULL;
	}
}

// -----------------------------------------------------------------------------

UINT CPlcProcessSequence2::threadFunction(LPVOID pParam)
{
	CPlcProcessSequence2* pThis = (CPlcProcessSequence2*)pParam;

	while (pThis->m_bThreadLife)
	{
		pThis->threadJob();
		Sleep(10);
	}

	return 0;
}

void CPlcProcessSequence2::threadJob(void)
{
	doRunStep();
}

// -----------------------------------------------------------------------------

void CPlcProcessSequence2::doRunStep()
{
	CMgrStatus* pCMgrStatus = CMgrStatus::GetMgr();
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	int nStageNo = 1;
	int nMgrStage2Alarm = CMgrStatus::GetMgr()->GetEqp_SystemAlarm(nStageNo);

	if (pCMgrStatus == NULL)
		return;
	if (pDlgViewPlc2 == NULL)
		return;
	if (pMgrDio == NULL)
		return;

	//PLC Check 1Sec
	SetSendHeartBit();
	GetReadHeartBit();

	//if (m_PLCAlarmFlag != TRUE)
	//{
	//	SendPlcBlockData();
	//	AutoRun();
	//}
	if (pMgrDio->GetEQType() != enumEQType_DFS)
		return;

	if (CMgrPCan::GetMgr()->GetStation(nStageNo)->GetAlarmStatus() == true || nMgrStage2Alarm == ALARM_HEAVY || pMgrDio->GetEMOCheckSensor(nStageNo))
	{
		CString str;
		long lBitStatus;
		
		//Ready Signal Off (EMO == Ready)
		//Ready Signal Off -> 배터리 연결 초기화 확인
		lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_EMO].lPlcData;

		lBitStatus = BIT_ON;

		pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_EMO].lPlcData = lBitStatus;
		pDlgViewPlc2->PLC_SendProcessData();
	}
	//else
	//{
	//	CString str;
	//	long lBitStatus;

	//	lBitStatus = pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_EMO].lPlcData;

	//	lBitStatus = BIT_OFF;
	//	pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_EMO].lPlcData = lBitStatus;
	//	pDlgViewPlc2->PLC_SendProcessData();
	//}

	if (pCMgrStatus->GetEqp_RunMode(nStageNo) == EQP_RUN_MODE_AUTO)
	{
		SendPlcBlockData();
		AutoRun();
	}
}

void CPlcProcessSequence2::AutoRun()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	CMgrModel* pMgrModel = CMgrModel::GetMgr();
	CMgrChannel* pMgrChannel = CMgrChannel::GetMgr();
	CMgrPCan* pMgrPCan = CMgrPCan::GetMgr();
	CMgrAlarm* pMgrAlarm = CMgrAlarm::GetMgr();
	int nStageNo = 1;
	int nMgrStage2Alarm = CMgrStatus::GetMgr()->GetEqp_SystemAlarm(nStageNo);
	int nPlcWaitTimeMax = 1000;
	int nMgrStatuS2 = CMgrStatus::GetMgr()->GetEqpRunStatus(nStageNo);
	ALARM_ITEM item;

	if (pMgrPCan == NULL)
		return;
	if (pMgrAlarm == NULL)
		return;

	if (pMgrPCan->GetStation(nStageNo) == NULL)
		return;

	//Heart Bit Check
	if (Get_PLCHeartBitAlarm() != TRUE)
	{
		pMgrAlarm->AddEqAlarm(ALARM_DFS_STAGE2_HEART_BIT_RECEIVE_ERR, "Stage2 PLC Heart Bit Check Please", ALARM_LIGHT, "Stage2 PLC Heart Bit Check Please", &item);
		return;
	}

	//Alarm Check 
	if (nMgrStatuS2 == eCyclerChannelStateError || pDlgViewPlc2->m_StPlcDataParam2[TYPE_PLC_EMO].lPlcData == BIT_ON || pDlgViewPlc2->m_StPlcDataParam2[TYPE_READ_PLC_ALARM_OCCUR].lPlcData == BIT_ON)
		return;

	//Init Check
	if (CMgrStatus::GetMgr()->GetEqInitFlag(nStageNo) == FALSE)
		return;

	//Start Check
	if (CMgrStatus::GetMgr()->GetEqStartFlag(nStageNo) == FALSE)
		return;

	////TEST
	//if (m_RunStopFlag == FALSE)
	//	return;

	switch (m_ePressRunStepCurrent)
	{
	//초기화
	case AUTO_INIT:
	{
		m_PlcWaitTime = 0;
		pDlgViewPlc2->PlcDataInit();
		pDlgViewPlc2->PlcDataBlockInit();
		pDlgViewPlc2->PLC_SendProcessData();

		SetAutoStep(AUTO_PLC_READY);
		break;
	}
	//PLC -> PC READY
	case AUTO_PLC_READY:
	{
		if (m_PlcWaitTime > 200)
		{
			if (pDlgViewPlc2->m_StPlcDataParam2[TYPE_READ_READY].lPlcData == BIT_ON)
			{
				m_PlcWaitTime = 0;
				SetAutoStep(AUTO_PC_PROCESS_START);
				break;
			}
			else
			{
				pMgrAlarm->AddEqAlarm(ALARM_DFS_STAGE1_READY_BIT_RECEIVE_ERR, "Stage1 PLC Ready Bit Check Please", ALARM_LIGHT, "Stage1 PLC Ready Bit Check Please", &item);
				break;
			}
		}
		else
		{
			m_PlcWaitTime++;
		}
		break;
	}
	//PC -> PLC PROCESS_START	
	case AUTO_PC_PROCESS_START:
	{
		//모델 선택 확인
		SModel* lpModel = pMgrModel->GetModel(pMgrModel->GetModelIndex(nStageNo));
		if (lpModel != NULL)
		{
			//스케줄 선택 확인
			SSchedule* lpSchedule = lpModel->GetSchedule(pMgrModel->GetScheduleIndex(nStageNo));
			if (lpSchedule != NULL)
			{
				m_PlcWaitTime = 0;
				pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_PROCESS_START].lPlcData = BIT_ON;
				pDlgViewPlc2->PLC_SendProcessData();
				SetAutoStep(AUTO_PLC_LOAD_COMPLETE);
			}
			else
			{
				if (m_PlcWaitTime > 7000)
				{
					m_PlcWaitTime = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_SCHDULE_SELECT_OVER_TIME, "Stage2 Schdule Select Check Please", ALARM_LIGHT, "Stage2 Schdule Select Check Please", &item);
				}
				else
				{
					m_PlcWaitTime++;
				}

			}
		}
		break;
	}
	//PLC -> PC LOAD_COMPLETE	
	case AUTO_PLC_LOAD_COMPLETE:
	{
		//PLC Load Complete Signal Check
		if (pDlgViewPlc2->m_StPlcDataParam2[TYPE_PLC_LOAD_COMPLETE].lPlcData == BIT_ON)
		{
			m_PlcWaitTime = 0;
			SetAutoStep(AUTO_PC_SCHEDULE_START);
		}
		else
		{
			if (m_PlcWaitTime > 500)
			{
				m_PlcWaitTime = 0;

				//PC Bit 확인
				if (pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_PROCESS_START].lPlcData != BIT_ON)
				{
					SetAutoStep(AUTO_PC_PROCESS_START);
				}
				else
				{

					pMgrAlarm->AddEqAlarm(ALARM_DFS_STAGE2_PLC_LOAD_COMPLETE_BIT_RECEIVE_ERR, "Stage2 PLC Load Complete Bit Check Please", ALARM_LIGHT, "Stage2 PLC Load Complete Bit Check Please", &item);
				}

			}
			else
			{
				m_PlcWaitTime++;
			}
		}
		break;
	}
	//PC -> PLC SCHEDULE_START
	case AUTO_PC_SCHEDULE_START:
	{
		CDlgViewEqpChannelStatus::GetMgr()->ExecProcScheduleStart(nStageNo);
		Sleep(500);
		m_PlcWaitTime = 0;
		SetAutoStep(AUTO_PC_SCHEDULE_START_CHECK);
		break;
	}
	case AUTO_PC_SCHEDULE_START_CHECK:
	{
		//Start Check
		if (pMgrChannel->IsCyclerChannelStarted(nStageNo) == true)
		{
			m_PlcWaitTime = 0;

			pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_SCHEDULE_START].lPlcData = BIT_ON;
			pDlgViewPlc2->PLC_SendProcessData();
			SetAutoStep(AUTO_PC_SCHEDULE_COMPLETE);
		}
		break;
	}
	//PC -> PLC DATA_SEND V, A , T 
	case AUTO_PC_SCHEDULE_COMPLETE:
	{
		nMgrStatuS2 = CMgrStatus::GetMgr()->GetEqpRunStatus(nStageNo);

		if (nMgrStatuS2 == eCyclerChannelStateComplete)
		{
			m_PlcWaitTime = 0;
			pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_RESULT_PASS].lPlcData = BIT_ON;
			pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_SCHEDULE_COMPLETE].lPlcData = BIT_ON;
			pDlgViewPlc2->PLC_SendProcessData();
			SetAutoStep(AUTO_PLC_UNLOAD_COMPLETE);
		}
		else
		{
			if (CMgrPCan::GetMgr()->GetStation(nStageNo)->GetAlarmStatus() == true || nMgrStatuS2 == eCyclerChannelStateError)
			{

				pMgrAlarm->AddEqAlarm(ALARM_STAGE2_STATUS_ALARM, "Stage2 Inverter && Schedule Status Check Please", ALARM_LIGHT, "Stage2 Inverter && Schedule Status Check Please", &item);
			}
		}
		break;
	}
	//PLC -> PC UNLOAD_COMPLETE	
	case AUTO_PLC_UNLOAD_COMPLETE:
	{
		if (pDlgViewPlc2->m_StPlcDataParam2[TYPE_PLC_UNLOAD_COMPLETE].lPlcData == BIT_ON)
		{
			m_PlcWaitTime = 0;
			SetAutoStep(AUTO_PLC_PROCESS_END);
			break;

		}
		else
		{
			if (m_PlcWaitTime > 200)
			{
				m_PlcWaitTime = 0;
				pMgrAlarm->AddEqAlarm(ALARM_DFS_STAGE2_PLC_UNLOAD_COMPLETE_BIT_RECEIVE_ERR, "Stage2 PLC Unload Complete Bit Check Please", ALARM_LIGHT, "Stage2 PLC Unload Complete Bit Check Please", &item);

			}
			else
			{
				m_PlcWaitTime++;
			}
		}
		break;
	}
	//PC -> PLC PROCESS_END
	case AUTO_PLC_PROCESS_END:
	{
		//pDlgViewPlc1->m_StPlcDataParam[TYPE_SEND_SCHEDULE_COMPLETE].lPlcData = BIT_OFF;
		pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_PROCESS_END].lPlcData = BIT_ON;
		pDlgViewPlc2->PLC_SendProcessData();

		SetAutoStep(AUTO_PLC_PROCESS_END_CHECK);
		break;
	}
	case AUTO_PLC_PROCESS_END_CHECK:
	{
		Sleep(1000);

		pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_PROCESS_START].lPlcData = BIT_OFF;
		pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_SCHEDULE_START].lPlcData = BIT_OFF;
		pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_SCHEDULE_COMPLETE].lPlcData = BIT_OFF;
		pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_RESULT_PASS].lPlcData = BIT_OFF;
		pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_PROCESS_END].lPlcData = BIT_OFF;

		pDlgViewPlc2->PLC_SendProcessData();

		CMgrStatus::GetMgr()->SetEqInitFlag(nStageNo, FALSE);
		CMgrStatus::GetMgr()->SetEqStartFlag(nStageNo, FALSE);
		SetAutoStep(AUTO_INIT);
		break;
	}
	//Alarm
	}
}


void CPlcProcessSequence2::WaitTime(DWORD dwMillisecond)
{
	DWORD dwStart;
	dwStart = GetTickCount();

	while (GetTickCount() - dwStart < dwMillisecond)
	{
		Sleep(10);
	}
}

void CPlcProcessSequence2::SetAutoStep(enumProcessStepAuto2 estepPress)
{
	m_ePressRunStepCurrent = estepPress;	// 새로 Step으로 전환
}


void CPlcProcessSequence2::Init_Seq()
{
	SetAutoStep(AUTO_INIT);
}

void  CPlcProcessSequence2::SetSendHeartBit()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();

	int nMaxTime = 100;

	m_SendTimeCheck++;

	if (m_SendTimeCheck > nMaxTime)
	{
		m_SendTimeCheck = 0;
		static BOOL bFlk = FALSE;

		if (!bFlk) {
			bFlk = TRUE;
			m_PlcAndPcStatus[TYPE_SEND_HEART_BIT] = BIT_ON;							//[JMW:220314]
			pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_HEART_BIT].lPlcData = 1;
			pDlgViewPlc2->PLC_WriteDevice(SEND_HEART_BIT, TYPE_SEND_HEART_BIT);


			////TEST Bit
			//m_PlcAndPcStatus[TYPE_READ_HEART_BIT] = BIT_OFF;						//[JMW:220314]
			//pDlgViewPlc2->m_StPlcDataParam2[TYPE_READ_HEART_BIT].lPlcData = 0;
			//pDlgViewPlc2->PLC_WriteDevice(READ_HEART_BIT, TYPE_READ_HEART_BIT);
			//bFlk = TRUE;
		}
		else {
			bFlk = FALSE;
			m_PlcAndPcStatus[TYPE_SEND_HEART_BIT] = BIT_OFF;						//[JMW:220314]
			pDlgViewPlc2->m_StPlcDataParam2[TYPE_SEND_HEART_BIT].lPlcData = 0;
			pDlgViewPlc2->PLC_WriteDevice(SEND_HEART_BIT, TYPE_SEND_HEART_BIT);

			////TEST Bit
			//m_PlcAndPcStatus[TYPE_READ_HEART_BIT] = BIT_ON;							//[JMW:220314]
			//pDlgViewPlc2->m_StPlcDataParam2[TYPE_READ_HEART_BIT].lPlcData = 1;
			//pDlgViewPlc2->PLC_WriteDevice(READ_HEART_BIT, TYPE_READ_HEART_BIT);

		}
	}
}

void  CPlcProcessSequence2::GetReadHeartBit()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	static BOOL bFlk = FALSE;
	int nMaxTime = 50;
	int nMaxAlarmCount = 500;

	m_ReadTimeCheck++;
	if (m_ReadTimeCheck > nMaxTime)
	{
		//Read Process
		pDlgViewPlc2->PLC_ReadProcessData();

		m_ReadTimeCheck = 0;

		if (!bFlk) {
			if (pDlgViewPlc2->m_StPlcDataParam2[TYPE_READ_HEART_BIT].lPlcData == 1)
			{
				bFlk = TRUE;
				m_ReadAlarmCount = 0;
				m_ReadTimeCheck = 0;
			}
		}
		else {
			if (pDlgViewPlc2->m_StPlcDataParam2[TYPE_READ_HEART_BIT].lPlcData == 0)
			{
				bFlk = FALSE;
				m_ReadAlarmCount = 0;
				m_ReadTimeCheck = 0;
			}
		}
	}

	m_ReadAlarmCount++;
	if (m_ReadAlarmCount >= nMaxAlarmCount)
	{
		//Alarm Flag On
		Set_PLCHeartBitAlarm(TRUE);
	}
}

void  CPlcProcessSequence2::SendPlcBlockData()
{
	CDlgViewPlc2* pDlgViewPlc2 = CDlgViewPlc2::GetMgr();
	int nMaxTime = 10;
	m_SendBlockTimeCheck++;
	if (m_SendBlockTimeCheck > nMaxTime)
	{
		pDlgViewPlc2->PLC_RealNumberWriteDevice2(SEND_V_START_24, PLC_SEND_V_24);//V
		pDlgViewPlc2->PLC_RealNumberWriteDevice2(SEND_A_START_24, PLC_SEND_A_24);//A
		pDlgViewPlc2->PLC_RealNumberWriteDevice2(SEND_Ah_START_24, PLC_SEND_Ah_24);//Ah
		pDlgViewPlc2->PLC_RealNumberWriteDevice2(SEND_T_START_24, PLC_SEND_T_24);//T

		m_SendBlockTimeCheck = 0;
	}
}