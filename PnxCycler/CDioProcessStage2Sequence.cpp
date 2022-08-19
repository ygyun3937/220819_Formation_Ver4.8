
#include "stdafx.h"
#include "PnxCycler.h"
#include "CDioProcessStage2Sequence.h"
#include "CDlgViewEqpChannelStatus.h"

#include "MgrDio.h"
#include "MgrStatus.h"
#include "MgrConfig.h"
#include "CMgrModel.h"
#include "CMgrAccount.h"
#include "MgrAlarm.h"
#include "MgrPCan.h"

CDioProcessStage2Sequence::CDioProcessStage2Sequence()
{
	m_eStage2PressRunstepCurrent = (enumProcessStepAutoType)AUTO_STAGE_EQ_RUN_TRAY_IN;
	m_eInitstepCurrentS2 = (enumProcessStepInitType)Init_Step_Probe_Pin_Down;

	m_bThreadLife = FALSE;


	nTimeOutCnt = 0;

	m_pThread = NULL;
}

CDioProcessStage2Sequence::~CDioProcessStage2Sequence()
{
}

void CDioProcessStage2Sequence::StartThread()
{
	if (m_pThread == NULL)
	{
		m_bThreadLife = TRUE;
		m_pThread = AfxBeginThread(threadFunction, this);
	}
}

void CDioProcessStage2Sequence::StopThread()
{
	m_bThreadLife = FALSE;
	m_pThread = NULL;

	if (m_pThread != NULL)
	{
		DWORD exitcode = ::WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		if (exitcode == WAIT_TIMEOUT)
			::TerminateThread(m_pThread->m_hThread, exitcode);
		m_pThread = NULL;
	}
}

// -----------------------------------------------------------------------------

UINT CDioProcessStage2Sequence::threadFunction(LPVOID pParam)
{
	CDioProcessStage2Sequence* pThis = (CDioProcessStage2Sequence*)pParam;

	while (pThis->m_bThreadLife)
	{
		pThis->threadJob();
		Sleep(10);
	}

	return 0;
}

void CDioProcessStage2Sequence::threadJob(void)
{
	doRunStep();
}

// -----------------------------------------------------------------------------

void CDioProcessStage2Sequence::doRunStep()
{
	Run_Stage2();
}

void CDioProcessStage2Sequence::Run_Stage2()
{
	CMgrStatus* pCMgrStatus = CMgrStatus::GetMgr();
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	CMgrPCan* pMgrPCan = CMgrPCan::GetMgr();
	int nStageNo = 1;
	int nMgrStage2Alarm = CMgrStatus::GetMgr()->GetEqp_SystemAlarm(nStageNo);

	if (pCMgrStatus == NULL)
	{
		return;
	}
	if (pMgrPCan == NULL)
	{
		return;
	}	
	if (pMgrDio == NULL)
	{
		return;
	}
	//PreCharger의 경우 DIO 제어권이 PLC에 있고 PC에서는 스위치, 스위치램프, PLC와 송수신 비트,  
	if (pMgrDio->GetEQType() == enumEQType_Precharger)
		return;

	if (pCMgrStatus->GetEqp_RunMode(1) == EQP_RUN_MODE_INIT || pMgrDio->GetEMOCheckSensor(1) || nMgrStage2Alarm == ALARM_HEAVY || pMgrPCan->GetStation(nStageNo)->GetAlarmStatus() == TRUE)
	{
		Init_Stage2();
	}
	else if (pCMgrStatus->GetEqp_RunMode(1) == EQP_RUN_MODE_AUTO)
	{
		Auto_Stage2();
	}
}

void CDioProcessStage2Sequence::Auto_Stage2()
{
	CString strMsg("");
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	CMgrAlarm* pMgrAlarm = CMgrAlarm::GetMgr();
	CMgrModel* pMgrModel = CMgrModel::GetMgr();
	CMgrPCan* pMgrPCan = CMgrPCan::GetMgr();

	int nStageNo = 1;
	int nMgrStatuS2 = CMgrStatus::GetMgr()->GetEqpRunStatus(nStageNo);
	ALARM_ITEM item;
	if (pMgrPCan == NULL)
		return;
	//Alarm Check
	if (nMgrStatuS2 == eCyclerChannelStateError)
		return;

	//Init Check
	if (CMgrStatus::GetMgr()->GetEqInitFlag(nStageNo) == FALSE)
		return;

	//Start Check
	if (CMgrStatus::GetMgr()->GetEqStartFlag(nStageNo) == FALSE)
		return;

	//220527 YGY IO 수정필요
	switch (m_eStage2PressRunstepCurrent)
	{
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_RUN_TRAY_IN:
	{
		//220627 YGYUN Detect On = true? false? Check 
		//Tray In Sensor 
		if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CHECK_SENSOR) == true)
		{
			Sleep(200);
			SetStage2AutoStep(AUTO_STAGE_EQ_RUN_TRAY_IN_CHECK);
			break;
		}
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_RUN_TRAY_IN_CHECK:
	{
		//220627 YGYUN Detect On = true? false? Check 

		//DCIR .. IO Type : MOXA	
		if (pMgrDio->GetEQType() == enumEQType_DCIR)
		{
			if (pMgrDio->GetIOStatus(1, 0) == true)
			{
				nTimeOutCnt = 0;
				SetStage2AutoStep(AUTO_STAGE_EQ_RUN_CENTERING_FORWORD);
				break;
			}
		}
		//Formation .. IO Type : Ajin
		else
		{
			//Tray In Sensor 
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CHECK_SENSOR) == true)
			{
				nTimeOutCnt = 0;
				SetStage2AutoStep(AUTO_STAGE_EQ_RUN_CENTERING_FORWORD);
				break;
			}
			else
			{
				if (nTimeOutCnt > 200)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_TRAY_SENSOR_CHECK_OVER_TIME, "Stage2 Tray Input Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 tray input detection sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		}
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_RUN_CENTERING_FORWORD:
	{
		//DCIR .. IO Type : MOXA
		if (pMgrDio->GetEQType() == enumEQType_DCIR)
		{
			pMgrDio->SetCenteringCylinder(nStageNo, TRUE);
			//Cylinder Foward Input Sensor Check
			if (pMgrDio->GetIOStatus(1, 1) == true && pMgrDio->GetIOStatus(1, 3) == true)
			{
				nTimeOutCnt = 0;
				SetStage2AutoStep(AUTO_STAGE_EQ_RUN_CENTERING_CLAMP_FORWORD);
			}
			else
			{
				if (nTimeOutCnt > 200)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CENTERING_CYLINDER_FOWRRD_SENSOR_CHECK_OVER_TIME, "Stage2 Centering Cylinder Foward Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Centering Cylinder Foward Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}

		}
		//Formation .. IO Type : Ajin
		else
		{
			pMgrDio->SetCenteringCylinder(nStageNo, TRUE);
			//Cylinder Foward Input Sensor Check
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CENTERING_CYLINDER_1_FORWARD) == true && pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CENTERING_CYLINDER_2_FORWARD) == true)
			{
				nTimeOutCnt = 0;
				SetStage2AutoStep(AUTO_STAGE_EQ_RUN_CENTERING_CLAMP_FORWORD);
			}
			else
			{
				if (nTimeOutCnt > 200)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CENTERING_CYLINDER_FOWRRD_SENSOR_CHECK_OVER_TIME, "Stage2 Centering Cylinder Foward Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Centering Cylinder Foward Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		}
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_RUN_CENTERING_CLAMP_FORWORD:
	{
		//DCIR .. IO Type : MOXA
		if (pMgrDio->GetEQType() == enumEQType_DCIR)
		{
			pMgrDio->SetClampCylinder(nStageNo, TRUE);
			//Clamp Foward Input Sensor Check
			if (pMgrDio->GetIOStatus(1, 9) == true && pMgrDio->GetIOStatus(1, 11) == true)
			{
				nTimeOutCnt = 0;
				SetStage2AutoStep(AUTO_STAGE_EQ_RUN_PUSHER_DOWN);
			}
			else
			{
				if (nTimeOutCnt > 200)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CLAMP_CYLINDER_FOWRRD_SENSOR_CHECK_OVER_TIME, "Stage2 Clamp Cylinder Foward Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Clamp Cylinder Foward Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}

		}
		//Formation .. IO Type : Ajin
		else
		{
			pMgrDio->SetClampCylinder(nStageNo, TRUE);
			//Clamp Foward Input Sensor Check
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CLAMP_CYLINDER_1_CLAMP) == true && pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CLAMP_CYLINDER_2_CLAMP) == true)
			{
				//InterLock
				if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_INTERLOCK_CHECK_SENSOR) == TRUE)
				{
					ALARM_ITEM item;

					CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_STAGE2_TRAY_INTERLOCK_SENSOR_DETECT, "STAGE2 Tray Interlock Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Tray Interlock Sensor and the tray input status.", &item);
					CMgrStatus::GetMgr()->SetEqStartFlag(1, FALSE);
					return;
				}
				else
				{
					nTimeOutCnt = 0;
					SetStage2AutoStep(AUTO_STAGE_EQ_RUN_PUSHER_DOWN);
				}
			}
			else
			{
				if (nTimeOutCnt > 200)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CLAMP_CYLINDER_FOWRRD_SENSOR_CHECK_OVER_TIME, "Stage2 Clamp Cylinder Foward Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Clamp Cylinder Foward Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		}
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_RUN_PUSHER_DOWN:
	{
		////DCIR .. IO Type : MOXA
		//if (pMgrDio->GetEQType() == enumEQType_DCIR)
		//{
		//	pMgrDio->SetPusherCylinder(nStageNo, FALSE);
		//	//Pusher Down Sensor Check
		//	if (pMgrDio->GetIOStatus(1, 14) == true)
		//	{
		//		nTimeOutCnt = 0;
		//		SetStage2AutoStep(AUTO_STAGE_EQ_RUN_PROBE_UP);
		//	}
		//	else
		//	{
		//		if (nTimeOutCnt > 200)
		//		{
		//			nTimeOutCnt = 0;
		//			pMgrAlarm->AddEqAlarm(ALARM_STAGE2_PUSHER_CYLINDER_DOWN_SENSOR_CHECK_OVER_TIME, "Stage2 Pusher Cylinder Down Sensor Check Please", ALARM_LIGHT, &item);
		//		}
		//		else
		//		{
		//			nTimeOutCnt += 1;
		//		}
		//	}

		//}
		////Formation .. IO Type : Ajin
		//else
		//{
			pMgrDio->SetPusherCylinder(nStageNo, FALSE);
			//Pusher Down Sensor Check
			if (nTimeOutCnt > 100)
			{
				//InterLock
				if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_INTERLOCK_CHECK_SENSOR) == TRUE)
				{
					ALARM_ITEM item;

					CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_STAGE2_TRAY_INTERLOCK_SENSOR_DETECT, "STAGE2 Tray Interlock Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Tray Interlock Sensor and the tray input status.", &item);
					CMgrStatus::GetMgr()->SetEqStartFlag(1, FALSE);
					return;
				}
				else
				{
					nTimeOutCnt = 0;
					SetStage2AutoStep(AUTO_STAGE_EQ_RUN_PROBE_UP);
				}
			}
			else
			{
				nTimeOutCnt += 1;
			}
		//}
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_RUN_PROBE_UP:
	{
		////DCIR .. IO Type : MOXA
		//if (pMgrDio->GetEQType() == enumEQType_DCIR)
		//{
		//	pMgrDio->SetProbeCylinder(nStageNo, TRUE);
		//	//Probe Up Sensor Check
		//	if (pMgrDio->GetIOStatus(2, 0) == true)
		//	{
		//		nTimeOutCnt = 0;
		//		pMgrModel->SetModelIndex(nStageNo, -1);
		//		pMgrModel->SetScheduleIndex(nStageNo, -1);
		//		pMgrModel->SetStageScheduleSelect(nStageNo, false);
		//		CDlgViewEqpChannelStatus::GetMgr()->SelectSchedule(nStageNo);
		//		SetStage2AutoStep(AUTO_STAGE_EQ_SCHEDULE_CHECK);
		//	}
		//	else
		//	{
		//		if (nTimeOutCnt > 1000)
		//		{
		//			nTimeOutCnt = 0;
		//			pMgrAlarm->AddEqAlarm(ALARM_STAGE2_PROBE_CYLINDER_UP_SENSOR_CHECK_OVER_TIME, "Stage2 Probe Cylinder Up Sensor Check Please", ALARM_LIGHT, &item);
		//		}
		//		else
		//		{
		//			nTimeOutCnt += 1;
		//		}
		//	}

		//}
		////Formation .. IO Type : Ajin
		//else
		//{
			pMgrDio->SetProbeCylinder(nStageNo, TRUE);
			//Pusher Down Sensor Check
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_PROBE_UP) == true)// && pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_PROBE_DOWN) == false)
			{
				nTimeOutCnt = 0;
				pMgrModel->SetModelIndex(nStageNo, -1);
				pMgrModel->SetScheduleIndex(nStageNo, -1);
				pMgrModel->SetStageScheduleSelect(nStageNo, false);
				CDlgViewEqpChannelStatus::GetMgr()->SelectSchedule(nStageNo);
				SetStage2AutoStep(AUTO_STAGE_EQ_SCHEDULE_CHECK);
			}
			else
			{
				if (nTimeOutCnt > 1000)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_PROBE_CYLINDER_UP_SENSOR_CHECK_OVER_TIME, "Stage2 Probe Cylinder Up Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Probe Cylinder Up Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		//}
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_SCHEDULE_CHECK:
	{	//모델 선택 확인
		SModel* lpModel = pMgrModel->GetModel(pMgrModel->GetModelIndex(nStageNo));
		if (lpModel != NULL)
		{
			//스케줄 선택 확인
			SSchedule* lpSchedule = lpModel->GetSchedule(pMgrModel->GetScheduleIndex(nStageNo));
			if (lpSchedule != NULL)
			{
				if (lpSchedule&& lpModel&&pMgrModel->GetStageScheduleSelect(nStageNo))
				{
					nTimeOutCnt = 0;
					Sleep(500);
					SetStage2AutoStep(AUTO_STAGE_EQ_SCEDULE_START);
				}
				else
				{
					if (nTimeOutCnt > 7000)
					{
						nTimeOutCnt = 0;
						pMgrAlarm->AddEqAlarm(ALARM_STAGE2_SCHDULE_SELECT_OVER_TIME, "Stage2 Schdule Select Check Please", ALARM_LIGHT, "Stage2 Schdule Select Check Please", &item);
					}
					else
					{
						nTimeOutCnt += 1;
					}
				}
			}			
		}	
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_SCEDULE_START:
	{
		CDlgViewEqpChannelStatus::GetMgr()->ExecProcScheduleStart(nStageNo);
		Sleep(500);
		nTimeOutCnt = 0;
		SetStage2AutoStep(AUTO_STAGE_EQ_SCEDULE_END_CHECK);
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_SCEDULE_END_CHECK:
	{
		nMgrStatuS2 = CMgrStatus::GetMgr()->GetEqpRunStatus(nStageNo);

		if (nMgrStatuS2 == eCyclerChannelStateComplete)
		{
			nTimeOutCnt = 0;
			SetStage2AutoStep(AUTO_STAGE_EQ_RUN_PROBE_DOWN);
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
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_RUN_PROBE_DOWN:
	{
		////DCIR .. IO Type : MOXA
		//if (pMgrDio->GetEQType() == enumEQType_DCIR)
		//{
		//	pMgrDio->SetProbeCylinder(nStageNo, FALSE);
		//	//Probe Down Sensor Check
		//	if (pMgrDio->GetIOStatus(2, 1) == true)
		//	{
		//		nTimeOutCnt = 0;
		//		SetStage2AutoStep(AUTO_STAGE_EQ_RUN_PUSHER_UP);
		//	}
		//	else
		//	{
		//		if (nTimeOutCnt > 1000)
		//		{
		//			nTimeOutCnt = 0;
		//			pMgrAlarm->AddEqAlarm(ALARM_STAGE2_PROBE_CYLINDER_DOWN_SENSOR_CHECK_OVER_TIME, "Stage2 Probe Cylinder Down Sensor Check Please", ALARM_LIGHT, &item);
		//		}
		//		else
		//		{
		//			nTimeOutCnt += 1;
		//		}
		//	}

		//}
		////Formation .. IO Type : Ajin
		//else
		//{
			pMgrDio->SetProbeCylinder(nStageNo, FALSE);
			//Probe Down Sensor Check
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_PROBE_DOWN) == true)// && pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_PROBE_DOWN) == false)
			{//InterLock
				if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_INTERLOCK_CHECK_SENSOR) == TRUE)
				{
					ALARM_ITEM item;
					CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_STAGE2_TRAY_INTERLOCK_SENSOR_DETECT, "STAGE2 Tray Interlock Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Tray Interlock Sensor and the tray input status.", &item);
					CMgrStatus::GetMgr()->SetEqStartFlag(1, FALSE);
					return;
				}
				else
				{
					nTimeOutCnt = 0;
					SetStage2AutoStep(AUTO_STAGE_EQ_RUN_PUSHER_UP);
				}
			}
			else
			{
				if (nTimeOutCnt > 1000)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_PROBE_CYLINDER_DOWN_SENSOR_CHECK_OVER_TIME, "Stage2 Probe Cylinder Down Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Probe Cylinder Down Sensor and the tray input status.", &item);					
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		//}
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_RUN_PUSHER_UP:
	{
		////DCIR .. IO Type : MOXA
		//if (pMgrDio->GetEQType() == enumEQType_DCIR)
		//{
		//	pMgrDio->SetPusherCylinder(nStageNo, TRUE);
		//	//Pusher Up Sensor Check
		//	if (pMgrDio->GetIOStatus(1, 13) == true)
		//	{
		//		nTimeOutCnt = 0;
		//		SetStage2AutoStep(AUTO_STAGE_EQ_RUN_CENTERING_CLAMP_BACKWORD);
		//	}
		//	else
		//	{
		//		if (nTimeOutCnt > 200)
		//		{
		//			nTimeOutCnt = 0;
		//			pMgrAlarm->AddEqAlarm(ALARM_STAGE2_PUSHER_CYLINDER_UP_SENSOR_CHECK_OVER_TIME, "Stage2 Pusher Cylinder Up Sensor Check Please", ALARM_LIGHT, &item);
		//		}
		//		else
		//		{
		//			nTimeOutCnt += 1;
		//		}
		//	}

		//}
		////Formation .. IO Type : Ajin
		//else
		//{
			pMgrDio->SetPusherCylinder(nStageNo, TRUE);
			//Pusher Up Sensor Check
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_PUSHER_UP) == true)// && pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_PROBE_DOWN) == false)
			{
				nTimeOutCnt = 0;
				SetStage2AutoStep(AUTO_STAGE_EQ_RUN_CENTERING_CLAMP_BACKWORD);
			}
			else
			{
				if (nTimeOutCnt > 200)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_PUSHER_CYLINDER_UP_SENSOR_CHECK_OVER_TIME, "Stage2 Pusher Cylinder Up Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Pusher Cylinder Up Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		//}
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_RUN_CENTERING_CLAMP_BACKWORD:
	{
		////DCIR .. IO Type : MOXA
		//if (pMgrDio->GetEQType() == enumEQType_DCIR)
		//{
		//	pMgrDio->SetClampCylinder(nStageNo, FALSE);
		//	//Clamp Backward Sensor Check
		//	if (pMgrDio->GetIOStatus(1, 10) == true && pMgrDio->GetIOStatus(1, 12) == true)
		//	{
		//		nTimeOutCnt = 0;
		//		SetStage2AutoStep(AUTO_STAGE_EQ_RUN_CENTERING_BACKWORD);
		//	}
		//	else
		//	{
		//		if (nTimeOutCnt > 200)
		//		{
		//			nTimeOutCnt = 0;
		//			pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CLAMP_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME, "Stage2 Clamp Cylinder Backward Sensor Check Please", ALARM_LIGHT, &item);
		//		}
		//		else
		//		{
		//			nTimeOutCnt += 1;
		//		}
		//	}

		//}
		////Formation .. IO Type : Ajin
		//else
		//{
			pMgrDio->SetClampCylinder(nStageNo, FALSE);
			//Clamp Backward Sensor Check
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CLAMP_CYLINDER_1_UNCLAMP) == true && pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CLAMP_CYLINDER_2_UNCLAMP) == true)
			{
				nTimeOutCnt = 0;
				SetStage2AutoStep(AUTO_STAGE_EQ_RUN_CENTERING_BACKWORD);
			}
			else
			{
				if (nTimeOutCnt > 200)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CLAMP_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME, "Stage2 Clamp Cylinder Backward Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Clamp Cylinder Backward Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		//}
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_EQ_RUN_CENTERING_BACKWORD:
	{
		////DCIR .. IO Type : MOXA
		//if (pMgrDio->GetEQType() == enumEQType_DCIR)
		//{
		//	pMgrDio->SetCenteringCylinder(nStageNo, FALSE);
		//	//Centering Backward Sensor Check
		//	if (pMgrDio->GetIOStatus(1, 2) == true && pMgrDio->GetIOStatus(1, 4) == true)
		//	{
		//		nTimeOutCnt = 0;
		//		SetStage2AutoStep(AUTO_STAGE_RUN_STEP_END);
		//	}
		//	else
		//	{
		//		if (nTimeOutCnt > 200)
		//		{
		//			nTimeOutCnt = 0;
		//			pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CENTERING_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME, "Stage2 Centering Cylinder Backward Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Centering Cylinder Backward Sensor and the tray input status.", &item);
		//		}
		//		else
		//		{
		//			nTimeOutCnt += 1;
		//		}
		//	}

		//}
		////Formation .. IO Type : Ajin
		//else
		//{
			pMgrDio->SetCenteringCylinder(nStageNo, FALSE);
			//Centering Backward Sensor Check
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CENTERING_CYLINDER_1_BACKWARD) == true && pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CENTERING_CYLINDER_2_BACKWARD) == true)
			{
				nTimeOutCnt = 0;
				SetStage2AutoStep(AUTO_STAGE_RUN_STEP_END);
			}
			else
			{
				if (nTimeOutCnt > 200)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CENTERING_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME, "Stage2 Centering Cylinder Backward Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Centering Cylinder Backward Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		//}
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_RUN_STEP_END:
	{
		nTimeOutCnt = 0;
		CMgrStatus::GetMgr()->SetEqStartFlag(nStageNo, FALSE);
		CMgrStatus::GetMgr()->SetEqInitFlag(nStageNo, FALSE);
		SetStage2AutoStep(AUTO_STAGE_EQ_RUN_TRAY_IN);
		break;
	}
	case CDioProcessStage2Sequence::AUTO_STAGE_ERROR_Check:
	{
		break;
	}
	default:
	{
		break;
	}
	}

}
void CDioProcessStage2Sequence::Init_Stage2()
{
	CMgrDio* pMgrDio = CMgrDio::GetMgr();
	CMgrAlarm* pMgrAlarm = CMgrAlarm::GetMgr();
	int nMgrStatuS2 = CMgrStatus::GetMgr()->GetEqpRunStatus(1);
	int nStageNo =1;
	ALARM_ITEM item;

	////Alarm Check
	//if (CMgrPCan::GetMgr()->GetStation(1)->GetAlarmStatus() == true || nMgrStatuS2 == eCyclerChannelStateError)
	//	return;

	//Start Check
	if (CMgrStatus::GetMgr()->GetEqStartFlag(1) == FALSE && pMgrDio->GetEMOCheckSensor(1) == FALSE)
		return;



	switch (m_eInitstepCurrentS2)
	{
	case enumProcessStepInitType::Init_Step_Probe_Pin_Down:
	{
		////DCIR .. IO Type : MOXA		
		//if (pMgrDio->GetEQType() == enumEQType_DCIR)
		//{
		//	pMgrDio->SetProbeCylinder(nStageNo, FALSE);
		//	//Probe Down Sensor Check
		//	if (pMgrDio->GetIOStatus(2, 1) == true)
		//	{
		//		nTimeOutCnt = 0;
		//		SetInitS2Step(Init_Step_Pusher_Up);
		//	}
		//	else
		//	{
		//		if (nTimeOutCnt > 1000)
		//		{
		//			nTimeOutCnt = 0;
		//			pMgrAlarm->AddEqAlarm(ALARM_STAGE2_PROBE_CYLINDER_DOWN_SENSOR_CHECK_OVER_TIME, "Stage2 Probe Cylinder Down Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Probe Cylinder Down Sensor and the tray input status.", &item);
		//		}
		//		else
		//		{
		//			nTimeOutCnt += 1;
		//		}
		//	}

		//}
		////Formation .. IO Type : Ajin
		//else
		//{
			pMgrDio->SetProbeCylinder(nStageNo, FALSE);
			//Pusher Down Sensor Check
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_PROBE_DOWN) == true)// && pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_PROBE_DOWN) == false)
			{
				nTimeOutCnt = 0;
				SetInitS2Step(Init_Step_Pusher_Up);
			}
			else
			{
				if (nTimeOutCnt > 1000)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_PROBE_CYLINDER_DOWN_SENSOR_CHECK_OVER_TIME, "Stage2 Probe Cylinder Down Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Probe Cylinder Down Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		//}
		break;
	}
	case enumProcessStepInitType::Init_Step_Pusher_Up:
	{
		////DCIR .. IO Type : MOXA
		//if (pMgrDio->GetEQType() == enumEQType_DCIR)
		//{
		//	pMgrDio->SetPusherCylinder(nStageNo, TRUE);
		//	//Pusher Up Sensor Check
		//	if (pMgrDio->GetIOStatus(1, 13) == true)
		//	{
		//		nTimeOutCnt = 0;
		//		SetInitS2Step(Init_Step_Clamp_UnClamp);
		//	}
		//	else
		//	{
		//		if (nTimeOutCnt > 200)
		//		{
		//			nTimeOutCnt = 0;
		//			pMgrAlarm->AddEqAlarm(ALARM_STAGE2_PUSHER_CYLINDER_UP_SENSOR_CHECK_OVER_TIME, "Stage2 Pusher Cylinder Up Sensor Check Please", ALARM_LIGHT, &item);
		//		}
		//		else
		//		{
		//			nTimeOutCnt += 1;
		//		}
		//	}

		//}
		////Formation .. IO Type : Ajin
		//else
		//{
			pMgrDio->SetPusherCylinder(nStageNo, TRUE);
			//Pusher Up Sensor Check
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_PUSHER_UP) == true)// && pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_PROBE_DOWN) == false)
			{
				nTimeOutCnt = 0;
				SetInitS2Step(Init_Step_Clamp_UnClamp);
			}
			else
			{
				if (nTimeOutCnt > 200)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_PUSHER_CYLINDER_UP_SENSOR_CHECK_OVER_TIME, "Stage2 Pusher Cylinder Up Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Pusher Cylinder Up Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		//}
		break;
	}
	case enumProcessStepInitType::Init_Step_Clamp_UnClamp:
	{
		////DCIR .. IO Type : MOXA
		//if (pMgrDio->GetEQType() == enumEQType_DCIR)
		//{
		//	pMgrDio->SetClampCylinder(nStageNo, FALSE);
		//	//Clamp Backward Sensor Check
		//	if (pMgrDio->GetIOStatus(1, 10) == true && pMgrDio->GetIOStatus(1, 12) == true)
		//	{
		//		nTimeOutCnt = 0;
		//		SetInitS2Step(Init_Step_Centering_Backward);
		//	}
		//	else
		//	{
		//		if (nTimeOutCnt > 200)
		//		{
		//			nTimeOutCnt = 0;
		//			pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CLAMP_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME, "Stage2 Clamp Cylinder Backward Sensor Check Please", ALARM_LIGHT, &item);
		//		}
		//		else
		//		{
		//			nTimeOutCnt += 1;
		//		}
		//	}

		//}
		////Formation .. IO Type : Ajin
		//else
		//{
			pMgrDio->SetClampCylinder(nStageNo, FALSE);
			//Clamp Backward Sensor Check
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CLAMP_CYLINDER_1_UNCLAMP) == true && pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CLAMP_CYLINDER_2_UNCLAMP) == true)
			{
				nTimeOutCnt = 0;
				SetInitS2Step(Init_Step_Centering_Backward);
			}
			else
			{
				if (nTimeOutCnt > 200)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CLAMP_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME, "Stage2 Clamp Cylinder Backward Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Clamp Cylinder Backward Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		//}
		break;
	}
	case enumProcessStepInitType::Init_Step_Centering_Backward:
	{
		////DCIR .. IO Type : MOXA
		//if (pMgrDio->GetEQType() == enumEQType_DCIR)
		//{
		//	pMgrDio->SetCenteringCylinder(nStageNo, FALSE);
		//	//Centering Backward Sensor Check
		//	if (pMgrDio->GetIOStatus(1, 2) == true && pMgrDio->GetIOStatus(1, 4) == true)
		//	{
		//		nTimeOutCnt = 0;
		//		SetInitS2Step(Init_Step_End);
		//	}
		//	else
		//	{
		//		if (nTimeOutCnt > 200)
		//		{
		//			nTimeOutCnt = 0;
		//			pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CENTERING_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME, "Stage2 Centering Cylinder Backward Sensor Check Please", ALARM_LIGHT, &item);
		//		}
		//		else
		//		{
		//			nTimeOutCnt += 1;
		//		}
		//	}

		//}
		////Formation .. IO Type : Ajin
		//else
		//{
			pMgrDio->SetCenteringCylinder(nStageNo, FALSE);
			//Centering Backward Sensor Check
			if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CENTERING_CYLINDER_1_BACKWARD) == true && pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_CENTERING_CYLINDER_2_BACKWARD) == true)
			{
				nTimeOutCnt = 0;
				SetInitS2Step(Init_Step_Job_Model_Check);
			}
			else
			{
				if (nTimeOutCnt > 200)
				{
					nTimeOutCnt = 0;
					pMgrAlarm->AddEqAlarm(ALARM_STAGE2_CENTERING_CYLINDER_BACKWARD_SENSOR_CHECK_OVER_TIME, "Stage2 Centering Cylinder Backward Sensor Check Please", ALARM_LIGHT, "Please check the connection status of the stage 2 Centering Cylinder Backward Sensor and the tray input status.", &item);
				}
				else
				{
					nTimeOutCnt += 1;
				}
			}
		//}
		break;
	}
	case enumProcessStepInitType::Init_Step_Job_Model_Check:
	{
		if (AfxMessageBox("Model 46*96 or 46*120?", MB_YESNO) == IDYES)
		{
			nTimeOutCnt = 0;
			pMgrDio->SetJobChangeCylinder(nStageNo, TRUE);
			SetInitS2Step(Init_Step_End);
			break;
		}
		else if (IDNO)
		{
			nTimeOutCnt = 0;
			pMgrDio->SetJobChangeCylinder(nStageNo, false);
			SetInitS2Step(Init_Step_End);
			break;
		}
	}
	case enumProcessStepInitType::Init_Step_End:
	{
		nTimeOutCnt = 0;
		SetStage2AutoStep(AUTO_STAGE_EQ_RUN_TRAY_IN);
		CMgrStatus::GetMgr()->SetEqStartFlag(nStageNo, FALSE);
		CMgrStatus::GetMgr()->SetEqInitFlag(nStageNo, TRUE);
		SetInitS2Step(Init_Step_Probe_Pin_Down);
		break;
	}
	default:
		break;
	}

}
/**
 * .
 *
 * METHOD_NAME : WaitTime
 * METHOD_TYPE : -
 * DESCRIPTION :
 */
void CDioProcessStage2Sequence::WaitTime(DWORD dwMillisecond)
{
	DWORD dwStart;
	dwStart = GetTickCount();

	while (GetTickCount() - dwStart < dwMillisecond)
	{
		Sleep(10);
	}
}


void CDioProcessStage2Sequence::SetStage2AutoStep(enumProcessStepAutoType estepPress)
{
	m_eStage2PressRunstepPrev = m_eStage2PressRunstepCurrent;
	m_eStage2PressRunstepCurrent = estepPress;	// 새로 Step으로 전환
}

void CDioProcessStage2Sequence::SetInitS2Step(enumProcessStepInitType estepPress)
{
	m_eInitstepCurrentS2 = estepPress;	// 새로 Step으로 전환

}

void CDioProcessStage2Sequence::Init_Stage2_Seq()
{
	SetStage2AutoStep(AUTO_STAGE_EQ_RUN_TRAY_IN);
	SetInitS2Step(Init_Step_Probe_Pin_Down);
}

BOOL CDioProcessStage2Sequence::InterLockCheck()
{	
	CMgrDio* pMgrDio = CMgrDio::GetMgr();

	//InterLock
	if (pMgrDio->GetAjinInputData(INPUT_Hanwha_STAGE2_TRAY_INTERLOCK_CHECK_SENSOR) == TRUE)
	{		
		return false;
	}
	return true;
}

