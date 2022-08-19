#include "Singleton.h"
#include <vector>

#define EQP_RUN_MODE_IDLE							0
#define EQP_RUN_MODE_PAUSE						1
#define EQP_RUN_MODE_INIT							2
#define EQP_RUN_MODE_MANUAL						3
#define EQP_RUN_MODE_AUTO							4


#define EQP_SUBEQP_MAX_COUNT 10
#define EQP_SUBEQP_STAGE_MAX_COUNT 10

#pragma once

struct SCylerChannelDump
{
	int		nCyclerChannelNumber;

	bool	bScheduleStart;

	CString	strScheduleFilePath;
	CString strLogFilePath;
	CString	strScheduleName; // .xml 제외
	CString	strScheduleSubName;	

	void	Init()
	{
		bScheduleStart = false;

		strScheduleFilePath.Empty();
		strLogFilePath.Empty();
		strScheduleName.Empty();
		strScheduleSubName.Empty();
	}
};
typedef struct _StageStepStatusInfo
{
	int nCurrentStep;
	int nStatus;	//0 : Wait 1: Processing 2: Complete
	bool bInit;

	void Init()
	{
		nCurrentStep = -1;
		nStatus = 0;
		bInit = false;
	}
};

// -----------------------------------------------------------------------------

typedef struct _EquipmentStageStatusInfo
{
	int nStageIndexNo;
	int nStageNo;

	CString strStageRunMode;//

	CString strStageRunStatus;
} EQP_STAGE_STATUS_INFO;


typedef struct _SubEquipmentStatusInfo
{
	int		nSubEqpIndexNo;
	int		nSubEqpNo;
	CString strSubEqpId;

	CString strRunMode;
	CString strRunStatus;

	CString strDioConnectionStatus;

	BOOL bScheudleFinish;

	EQP_STAGE_STATUS_INFO m_SubEqpStageStatusInfo[EQP_SUBEQP_STAGE_MAX_COUNT];

} SUBEQP_STATUS_INFO, *pSubEqp_Status_Info;

class CMgrStatus : public CSingleton< CMgrStatus >
{
public:
	CMgrStatus();
	virtual ~CMgrStatus();

public:
	SCylerChannelDump*	GetCyclerChannelDump(int nCyclerChannelNumber);
	void	SaveFile(int nCyclerChannelNumber);

public:
	// MAIN EQP STATUS
	//void	SetEqp_Stage1_RunMode(int runMode) { m_nEqp_Stage1_RunMode = runMode; }
	//int		GetEqp_Stage1_RunMode();
	//void	SetEqp_Stage2_RunMode(int runMode) { m_nEqp_Stage2_RunMode = runMode; }
	//int		GetEqp_Stage2_RunMode();

	void SetEQp_Finish(int nStageNo, bool bFinish);
	bool GetEQp_Finish(int nStageNo) { return m_SubEqpStatuInfo[nStageNo].bScheudleFinish; }


	void	SetEqp_RunMode(int nStageNo,int nrunMode) { m_nEqp_RunMode[nStageNo] = nrunMode; }
	int		GetEqp_RunMode(int nStageNo);
	
	void SetEqp_SystemAlarm(int nStageNo, int nAlarm);
	int GetEqp_SystemAlarm(int nStageNo);

	int GetEqpRunStatus(int nSubeqNo);
	int GetEqpRunProcessEndCheck(int nSubeqNo);
	BOOL GetEqpRunProcessingCheck(int nSubeqNo);
	BOOL GetEqpRunProcessErrorCheck(int nSubeqNo);
	// SUBEQP STATUS
	void	SetSubEqpStatusInfo(int nSubEqpIndexNo, SUBEQP_STATUS_INFO subEqpStatusInfo);
	SUBEQP_STATUS_INFO GetSubEqpStatusInfo(int nSubEqpIndexNo);

	CString GetSubEqpDioConnectionStatus(int nSubEqpIndexNo);
	CString GetSubEqpRunStatus(int nSubEqpIndexNo);
	CString GetSubEqpStageRunMode(int nSubEqpIndexNo, int nSubEqpStageIndexNo);

	//nRunStatus : 0 : Wait 1: Processing 2: Complete
	void Set_Status_Stage1(int nCurrentStep, int nRunStatus);

	//nRunStatus : 0 : Wait 1: Processing 2: Complete
	void Set_Status_Stage2(int nCurrentStep, int nRunStatus);

	_StageStepStatusInfo Get_Status_Stage1() { return m_Stage1StatusInfo; }
	_StageStepStatusInfo Get_Status_Stage2() { return m_Stage2StatusInfo; }


	void	SetEqInitFlag(int nStage, BOOL bInitFlag) { m_bStageInitFlag[nStage] = bInitFlag; }
	BOOL	GetEqInitFlag(int nStage) { return m_bStageInitFlag[nStage]; }

	void	SetEqStartFlag(int nStage, BOOL bStartFlag);
	BOOL	GetEqStartFlag(int nStage) { return m_bStageStartFlag[nStage]; }

	void	SetEqAutoMode(int nStage, BOOL bAutoMode) { m_bStageAutoMode[nStage] = bAutoMode; }
	BOOL	GetEqAutoMode(int nStage) { return m_bStageAutoMode[nStage]; }




private:
	void	Initialize();
	void	InitializeVariables();
	void	LoadFile();

	void	Terminate();

	BOOL m_bStageInitFlag[EQ_STAGE_INDEX];
	BOOL m_bStageStartFlag[EQ_STAGE_INDEX];
	BOOL m_bStageAutoMode[EQ_STAGE_INDEX];
private:
	std::vector<SCylerChannelDump*> m_vecCyclerDump;	// 이력관리

	// MAIN EQP

	int		m_nEqp_RunMode[2];
	int	m_nEqp_Alarm[2];		// 0: Normal 1: Light 2: Heavy
	_StageStepStatusInfo m_Stage1StatusInfo;
	_StageStepStatusInfo m_Stage2StatusInfo;

	// SUB EQP
	SUBEQP_STATUS_INFO m_SubEqpStatuInfo[EQP_SUBEQP_MAX_COUNT];
};

