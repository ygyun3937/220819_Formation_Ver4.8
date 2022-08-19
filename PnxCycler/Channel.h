#pragma once

// 1. 해당 Class 에서 Schedule Xml 파일을 관리.( 실제로는 관리 하는 Class 를 멤버로 잡음 )
// 2. Firmware 에서 올라오는 Data 를 메모리로 관리( 실제로 Firmware 정보는 DB 에 존재하고, Data 관리하는 Class 를 멤버로 잡음 )
// 3. 각 Channel 별 Schedule 관리를 함. Step 정보 찾거나 상태 Check 하거나 등등..

#include "MgrSchedule.h"
#include "CommData.h"
#include "SerialCom.h"
#include "CWorkLogger.h"
#include <map>

enum eScheduleGradeType
{
	eScheduleGradeTypeVoltage = 0,
	eScheduleGradeTypeCapacity,
	eScheduleGradeTypeWattHour
};

enum ENDREASON_CODE
{
	//USE FW
	ENDREASON_CODE_TIME = 1,
	ENDREASON_CODE_AMPERE,
	ENDREASON_CODE_CAPACITY,
	ENDREASON_CODE_WATT,
	ENDREASON_CODE_VOLTAGE,

	ENDREASON_CODE_GOTO,		//사이클 루프 만족시		//SW에서 사용할까?
	ENDREASON_CODE_RETURN,		//사이클 루프 불만족시		//하지말까? 고민필요
	ENDREASON_CODE_JUMP,
	ENDREASON_CODE_COMPLETE,
	ENDREASON_CODE_ALARAM,
	ENDREASON_CODE_PAUSE,

	//USE SW
		//SAFETY
		ENDREASON_CODE_SYSTEM_SAFETY_VOLTAGE_MAX = 100,
		ENDREASON_CODE_SYSTEM_SAFETY_VOLTAGE_MIN,
		ENDREASON_CODE_SYSTEM_SAFETY_AMPERE_MAX,
		ENDREASON_CODE_SYSTEM_SAFETY_TEMP_MAX,
		ENDREASON_CODE_SYSTEM_SAFETY_DELTA_V,
		ENDREASON_CODE_SYSTEM_SAFETY_DELTA_I,
		ENDREASON_CODE_SYSTEM_CYCLE_VOLTAGE_MAX,
		ENDREASON_CODE_SYSTEM_CYCLE_VOLTAGE_MIN,
		ENDREASON_CODE_SYSTEM_CYCLE_WATT_MAX,
		ENDREASON_CODE_SYSTEM_CYCLE_AH_MAX,
		ENDREASON_CODE_SYSTEM_CYCLE_AMPERE_MAX,
		ENDREASON_CODE_SYSTEM_CYCLE_WATTHOUR_MAX,
		ENDREASON_CODE_SYSTEM_STEP_VOLTAGE_MAX,
		ENDREASON_CODE_SYSTEM_STEP_VOLTAGE_MIN,
		ENDREASON_CODE_SYSTEM_STEP_CURRENT_MAX,
		ENDREASON_CODE_SYSTEM_STEP_CURRENT_MIN,
		ENDREASON_CODE_SYSTEM_STEP_AH_MAX,
		ENDREASON_CODE_SYSTEM_STEP_TEMP_MAX,
		//USER STOP
		ENDREASON_CODE_STEP_COMPLETE,
		ENDREASON_CODE_CYCLE_COMPLETE,
		ENDREASON_CODE_STEP_PAUSE,
		ENDREASON_CODE_CYCLE_PAUSE,
		//UTIL DATA
		ENDREASON_CODE_SOC,
		ENDREASON_CODE_SOH,
		ENDREASON_CODE_DAQ_VOLTAGE,
		ENDREASON_CODE_DAQ_TEMP,
		ENDREASON_CODE_BMS,
		ENDREASON_CODE_CHAMBER,

		ENDREASON_CODE_TOTAL_SAFETY_TEST_END_TIME,
		ENDREASON_CODE_TOTAL_SAFETY_TEMP_MAX,
		ENDREASON_CODE_TOTAL_SAFETY_CURRENT_MAX,
		ENDREASON_CODE_TOTAL_SAFETY_CURRENT_MIN,
		ENDREASON_CODE_TOTAL_SAFETY_VOLTAGE_MIN,
		ENDREASON_CODE_TOTAL_SAFETY_VOLTAGE_MAX,
		ENDREASON_CODE_TOTAL_SAFETY_CAPACITY_MAX,
		ENDREASON_CODE_TOTAL_SAFETY_NO_VOLTAGE_VARIATION,
		ENDREASON_CODE_TOTAL_SAFETY_CC_VOLTAGE_VARIATION,
		ENDREASON_CODE_TOTAL_SAFETY_CHARGE_CAPA_VARIATION, //yuri 20201201 추가
		ENDREASON_CODE_TOTAL_SAFETY_DCHARGE_CAPA_VARIATION,
		ENDREASON_CODE_TOTAL_SAFETY_SOH,
};
enum ENDREASON_EVENT_TYPE
{
	ENDREASON_EVENT_TYPE_COMPLETE = 1,
	ENDREASON_EVENT_TYPE_PAUSE,
	ENDREASON_EVENT_TYPE_JUMP,
};
enum
{
	OpCodeUnknown = 0,
	OpCodeInputRelayOn,
	OpCodeOutputReplyOn,
	OpCodeChargeCC,
	OpCodeChargeCV,
	OpCodeDischargeCC,
	OpCodeDischargeCV,
	OpCodeChargeCP,
	OpCodeDischargeCP
};

enum ParallelMode
{
	ParallelMaster = 0,
	ParallelSlave
};

enum EQMode
{
	EqModeSerial = 0,
	EQModeParallel
};

enum eCyclerChannelRunState
{
	eCyclerChannelRunStateUnknown = -1,
	eCyclerChannelRunStateRun,
	eCyclerChannelRunStateComplete,
	eCyclerChannelRunStateWait
};

enum eChamberOperationType
{
	eChamberOn = 0,
	eChamberOff,
	eChamberSet
};

typedef struct ST_END_CONDITON_INFO
{
	BOOL bEvent = FALSE;	// 뭐가 됬든 조건 만족시 TRUE

	int nNexCycle = 0;		// 종료시 다음 이동할 사이클
	int nNextStep = 0;		// 종료시 다음 이동할 스탭
	int nEndReason = 0;		// SW에서 알 수 있는 종료 이유
	int nEndType = 0;		// 종료 타입
}ENDCOND;

struct SViewData
{
	TCHAR szName[MAX_PATH];
	TCHAR szData[MAX_PATH];
};

struct SViewSchedule
{
	std::vector<CString> vecValue;

	void	Init()
	{
		vecValue.clear();
	}

	void	Add(CString strValue)
	{
		vecValue.push_back(strValue);
	}
};

struct SMultiSchedule
{
	CString strScheduleFileName;
	CString strScheduleFilePath;
	CString strLogFilePath;
	CString strUserID;
	CString strWorkName;
	CString strCellID;
	CString strEQPID;

	int		nSyncMode;
	int		nScheduleSerial;

	int		nStartCycle;
	int		nStartStep;

	void	GetScheduleInfo(std::vector<CString>& vecValue)
	{
		vecValue.clear();

		{
			CString strValue;
			strValue.Format("%d", nScheduleSerial+1);

			vecValue.push_back(strValue);
		}

		{
			vecValue.push_back(strScheduleFileName);
		}

		{
			vecValue.push_back(strLogFilePath);
		}

		{
			vecValue.push_back(strUserID);
		}

		{
			vecValue.push_back(strWorkName);
		}

		{
			CString strValue = "Not Use";

			if (eScheduleSyncModeTemperature == nSyncMode)
				strValue = "Temp Sync";
			else if (eScheduleSyncModeStep == nSyncMode)
				strValue = "Step Sync";

			vecValue.push_back(strValue);
		}

		{
			vecValue.push_back(strCellID);
		}

		{
			CString strValue;
			strValue.Format("%d", nStartCycle);

			vecValue.push_back(strValue);
		}

		{
			CString strValue;
			strValue.Format("%d", nStartStep);

			vecValue.push_back(strValue);
		}
	}

	struct SCompareGreater
	{
		bool operator()(const SMultiSchedule* lhs, const SMultiSchedule* rhs)
		{
			return (lhs->nScheduleSerial < rhs->nScheduleSerial);
		}
	};
};

// -----------------------------------------------------------------------------
//PCAN
enum ChannelStatus
{
	/*ChannelStatusUnknown = -1,
	ChannelStatusReady,
	ChannelStatusStart,
	ChannelStatusComplete,
	ChannelStatusError,
	ChannelStatusStop,
	ChannelStatusStepStop,
	ChannelStatusCycleStop,
	ChannelStatusPause,
	ChannelStatusStepPause,
	ChannelStatusCyclePause,
	ChannelStatusSafety,
	ChannelStatePauseFromStepSync,
	ChannelStatePauseFromTempSync,
	ChannelStatusEMO*/
};

// -------------------------------------------------------------------------------------------------

class CChannel
{
public:
	CChannel();
	virtual ~CChannel();

	void	InitStepStart();
	void	InitStepStart(int length);
	void	InitStepStart(int nStepIndex, int nStepTime, int nCycleTime, int nLoopCount, float fStepAh, float fStepWh, float fCycleAh, float fCycleWh);
	////220221 YGY 채널 동시 시작을 위해 수정 -- 시작 --
	void	InitStepStart_UnCheck(int nStepIndex, int nStepTime, int nCycleTime, int nLoopCount, float fStepAh, float fStepWh, float fCycleAh, float fCycleWh);
	void	InitStepStart_Check(int nStepIndex, int nStepTime, int nCycleTime, int nLoopCount, float fStepAh, float fStepWh, float fCycleAh, float fCycleWh);	
	////-- 끝 --
	void	ScheduleFinish(bool bIsScheduleComplete, int nEndReason);
	void	CheckAllCondition();					//220114 YGYUN 안전조건 확인 + 통합안전조건(Test End Time)
	void	Check_Cycle_TotalSafetyCondition();		//220114 YGYUN  통합안전조건확인 함수 : Test End Time은 CheckAllCondtion 함수에서 확인(다른 안전조건과 달리 사이클 정지가 아님)
	void	ManualNextStep();	//지정스탭 계산 후 동작 (메뉴얼 동작시 사용) 즉시 커멘드 요청
	void	ManualPause();
	void	FinalUpdate(CCommData* pCommdata);
	void	CheckFlowStatus(CCommData* pCommData);				//채널 상태 설정
	//int		GetChamberIndex(int Channel);
	void	SetReservePause(int nCycleNumber, int nStepNumber);
	void	SetReserveComplete(int nCycleNumber, int nStepNumber);
	void	SetChannelNumber(int nChannelNumber);
	void	GetReserveInfo(std::vector<CString>& vecValue);
	void	GetCyclerChannelInfo(std::vector<CString>& vecValue);
	void	GetViewPatternInfo(std::vector<SViewSchedule>& vecSchedule);	
	void	RemoveMultiSchedule(int nIndex);
	void	SetEndCondition();
	void	SetSoc();
	void	SetSoH();
	void	SetSoH2();
	void	InitalizeReady();
	void	SetFlagSendCommandJump(bool bIsStepChange, bool bFlag);
	void	SetSyncMode(int nSyncMode);
	void	SetSyncChannel1(int nSyncChannel1) { m_nSyncChannel1 = nSyncChannel1; }
	void	SetSyncChannel2(int nSyncChannel2) { m_nSyncChannel2 = nSyncChannel2; }
	
	void	SetStepSync(bool bStepSync) { 
		SetCancelStepSync(false);
		m_bStepSync = bStepSync; 
	}
	
	void	SetTempSync(bool bTempSync) {
		SetCancelTempSync(false);
		m_bTempSync = bTempSync; 
	}
	
	void	SetNormalSync(bool bNormalSync) { m_bNormalSync = bNormalSync; }
	//void	SetSyncType(int nSyncType) { m_nSyncType = nSyncType; }
	void	SetChamberElevatedTemperature(bool bElavatedTemperature) { m_bChamberElevatedTemperature = bElavatedTemperature; }
	void	SetEndSync(int nEndSync) { m_nEndSync = nEndSync; }
	void	SetChamberDelayTime(int nChamberDelayTime) { m_nChamberDelayTime = nChamberDelayTime; }
	void	SetCycleNumber(int nCycleNum) { m_nCycleNumber = nCycleNum; }
	void	SetStepNumber(int nStepNum) { m_nStepNumber = nStepNum;  }
	void	SetCycleIndex(int nIndex) { m_nCycleIndex = nIndex; }
	void	SetTotalCycleLoop(int nCurrnetLoop, int nStepIndex, bool bCycleChange);
	void	SetScheduleMode(int nScheduleMode);
	void	AddMultiSchedule(SMultiSchedule* lpMultiSchedule);
	void	ChannelPack2Array(__out CStringArray& arr, CStringArray& arrRef);
	void	SetChannelStatus(int nStatus) 	{ 	m_nChannelStatus = nStatus;	}	

	void	SetCancelTempSync(bool sync) {
		m_bCancelTempSync = sync;
	}

	bool	GetCancelTempSync() {
		return m_bCancelTempSync;
	}

	void	SetCancelStepSync(bool sync) {
		m_bCancelStepSync = sync;
	}

	bool	GetCancelStepSync() {
		return m_bCancelStepSync;
	}

	ULONGLONG m_ulLastAckTime;

	CWorkLogger _workLogger;

	void	SetSendDataPacket(bool bSendDataPacket) { m_bDataSendPacket = bSendDataPacket; }
	
	void	SetChannelEnd(bool bChannelEnd) { 
		//if (bChannelEnd == true) {
		//	// 스텝이 종료될 경우 작업 명세서 EndCondition 기록
		//	INIWriteStr("Specification", "EndCondition", "true", m_specificationPath);
		//}

		m_bChannelEnd = bChannelEnd; 
	}

	void	SetChannelStart(bool bChannelStart) { m_bChannelStart = bChannelStart; }
	void	SetChamberChange(bool bChamberChange) { m_bChamberChage = bChamberChange; }

	void	SetFileName(CString strFileName) { 
		m_strFileName.Format(_T("%s"), strFileName.GetString()); 
		INIWriteStr("Specification", "FileName", m_strFileName, m_specificationPath);
	}
	
	void	SetScheduleName(CString strScheduleName) { 
		m_strScheduleName.Format(_T("%s"), strScheduleName.GetString()); 
		INIWriteStr("Specification", "ScheduleName", m_strScheduleName, m_specificationPath);
	}
	
	void	SetSampleName(CString str) { m_strSampleName = str; }
	void	SetDescription(CString str) { m_strDescription = str; }
	
	void	SetLogFolderName(CString str) { 
		m_strLogFolderName = str; 
		INIWriteStr("Specification", "LogFolderName", str, m_specificationPath);
	}
	
	void	SetScheduleStart(bool bStart) { m_bScheduleStart = bStart; }
	void	SetEndCondition(CString strEndReason) { m_strEndCondition.Format(_T("%s"), strEndReason.GetString()); }
	void	SetPause(bool bPause) { m_bPause = bPause; }
	void	SetPauseStep(bool bPause) { m_bPauseStep = bPause; }
	void	SetPauseCycle(bool bPause) { m_bPauseCycle = bPause; }
	void	SetCompleteStep(bool bComplete) { m_bCompleteStep = bComplete; }
	void	SetCompleteCycle(bool bComplete) { m_bCompleteCycle = bComplete; }
	void	SetSelected(bool bSelect) { m_bSelected = bSelect; }
	void	SetOperationMode(int nMode) { m_nOperationMode = nMode; }	
	void	SetAccurateReason(int nReason) { m_nAccurateReason = nReason; } //lyw3_1
	void	SetIsStepChange(bool bIsStepChange) { m_bIsStepChange = bIsStepChange; }
	void	SetIsCycleChange(bool bIsCycleChange) { m_bIsCycleChange = bIsCycleChange; }
	void	SetSystemSafetyReason(int nReason) { m_nSystemSafetyReason = nReason; }
	void	SetRunningCount(int nCount) { m_nRunningCount = nCount; }
	void	SetFlagSendCommandJump(bool bFlag) { m_bIsSendCommandJump = bFlag; }
	
	void	SetUserID(CString str) { 
		m_strUserID = str; 
		INIWriteStr("Specification", "UserID", str, m_specificationPath);
	}

	void	SetCellID(CString str) { m_strCellID = str; }
	
	void	SetWorkName(CString str) { 
		m_strWorkName = str; 
		INIWriteStr("Specification", "WorkName", str, m_specificationPath);
	}

	void	SetChBarcode(CString str) { m_strChBarCode = str; }
	
	void	SetExternPatternStatus(bool bStatus) { m_bExternPatternCheck = bStatus; }

	void	SetStepMode(int nStepMode) { m_nStepMode = nStepMode; }

	int		GetStepMode() {	return m_nStepMode;	}



	int		GetSyncChannel1() { return m_nSyncChannel1; }
	int		GetSyncChannel2() { return m_nSyncChannel2; }	
	int		GetSyncType() { return m_nSyncType; }
	int		GetChannelNumber() { return m_nChannelNumber; }	
	int		GetEndSync() { return m_nEndSync; }
	int		GetChamberDelayTime() { return m_nChamberDelayTime; }
	int		GetCycleNumber() { return m_nCycleNumber; }
	int		GetStepNumber() { return m_nStepNumber; }
	int		GetCycleIndex() { return m_nCycleIndex; }
	int		GetChannelStatus() { return m_nChannelStatus; }
	int		GetProcessStatusInt() { return m_nProcessStatus; }
	int		GetSystemSafetyReason() { return m_nSystemSafetyReason; } //lyw3_1
	int		GetOperationMode() { return m_nOperationMode; }

	int		GetAccurateReason() { return m_nAccurateReason; }
	int		GetRunningCount() { return m_nRunningCount; }
	int		GetReserveCompleteCycle() { return m_nReserveCompleteCycleNumber; }
	int		GetReserveCompleteStep() { return m_nReserveCompleteStepNumer; }
	int		GetReservePauseCycle() { return m_nReservePauseCycleNumber; }
	int		GetReservePauseStep() { return m_nReservePauseStepNumber; }
	int		GetMultiScheduleSize() { return (int)m_vecMultiSchedule.size(); }

	bool	IsReserved();
	bool	IsMultiSchedule();
	bool	IsReservedPause();
	bool	IsReservedComplete();
	bool	GetChamberElevatedTemperature() { return m_bChamberElevatedTemperature; }
	bool	GetStepSync() { return m_bStepSync; }
	bool	GetTempSync() { return m_bTempSync; }
	bool	GetNormalSync() { return m_bNormalSync; }
	bool    GetExternPatternStatus() { return m_bExternPatternCheck; }

	char*	GetFileID() { return m_szFileID; }

public:
	bool	SendStep(int nReserveStepIndex = 1);
	int		SendChannelChoice(int channel);
	void    SetChannelSync(int pos, int channel, bool stepsync);
	bool	SendStart();
	bool	SendStart(int length);
	bool	SendStart(int nStepIndex, int nStepTime, int nCycleTime, int nLoopCount, float fStepAh, float fStepWh, float fCycleAh, float fCycleWh);
	bool	SendTemp();
	bool	ProcessingResultFile(bool bIsPause, CCommData* pCommdata);
	bool	SendPause(int nType);
	bool	SendPause(int nType, int nStep);
	bool	SendJump(int nStepIndex);
	bool	SendCommand(int nCommand);
	bool	ManualNextStep(int nCycleNumber, int nStepNumber);	//지정스탭을 알고 있을시 즉시 커멘드 요청
	bool	GetSendDataPacket() { return m_bDataSendPacket; }
	bool	GetChannelEnd() { return m_bChannelEnd; }			//채널 스텝 종료 플래그 Get
	bool	GetScheduleStart() { return m_bScheduleStart; }
	bool	GetPause() { return m_bPause; }
	bool	GetSelected() { return m_bSelected; }
	bool	GetSyncChamber() { return m_bSync; }

	bool    ChamberTempSet(int channel, float temp);
	bool	ChamberRun(int channel);
	bool	ChamberStop(int channel);

	CString	GetChannelStatusStr();
	CString GetChamberChannel();
	CString GetRunChannel() { return m_strRunChannel; }	
	CString	GetFileName() { return m_strFileName.GetString(); }
	CString	GetScheduleName() { return m_strScheduleName.GetString(); }
	CString	GetLogFolderName() { return m_strLogFolderName; }
	CString GetUserID() { return m_strUserID; }
	CString GetCellID() { return m_strCellID; }
	CString GetWorkName() { return m_strWorkName; }
	CString GetChBarcodeName() { return m_strChBarCode; }

	ParallelMode	IsMasterChannel();
	void			SetMasterChannel(ParallelMode nObj);
	EQMode			GetEqMode();
	void            SetEqMode(EQMode nObj);
	//////////////////////////////////////////////////////////////////////////
	void StartTempSync(int nEndReason, int channel, int nStepIndex);
	BOOL CheckStepSync(int nEndReason, int channel, int nStepIndex);
	void StartStepSync(int nEndReason, int channel, int nStepIndex);
	//////////////////////////////////////////////////////////////////////////

	CMgrSchedule*	GetMgr();

	SMultiSchedule*	GetMultiScheduleInfo(int nIndex);

	std::vector<SViewData>	GetViewData();
	std::vector<SMultiSchedule*>&	GetMultiSchedule() { return m_vecMultiSchedule; }


	int		m_nChannelSyncType[32];

	int		m_nChanelSequence[4];	// Interface Channel 만큼
	int		m_nOldStep[4];
	float	m_fLastTempSetValue[4];
	float	m_fStepTempValue[100];

	CString m_specificationPath;

	void	SendChamberControl(int nStepIndex, eChamberOperationType eChamberOperationType);
	void	SendChamberControl(int nStepIndex, int chamberindex);

	bool    IsChamberUseInStartedChannel(int nChannelNo);
	CString m_fnGetChamberName(int nChannelNo);
	void	SetLastAckTime(ULONGLONG ulTime);
	void	SetBoardTime(ULONGLONG ulTime);

	CString	StepType2String();

	CString m_strChBarCode;



public:
	// FORMATION
	void	ChannelFormation2Array(__out CStringArray& arr, CStringArray& arrRef);
	void	AutoCalibration(float fVolt, float fAmp, int nStep);

	void	Set_CalVoltSetting(float fVolt) { m_fFormationCalSettingVolt = fVolt; }
	float	Get_CalVoltSetting() { return m_fFormationCalSettingVolt; }
	// AUTO_CALIBRATION
	float	m_fFormationAmpere = 0.0;
	float	m_fFormationVoltage = 0.0;
	int		m_iVoltCnt = 0;
	int		m_iAmpCnt = 0;

	float	m_fFormationCalSettingVolt = 0.0f;

	//220818 YGY Calibration Data Map Data 
	void Set_CurrentCalibrationData(float fTargetValue, float fMeasureValue);
	float Get_CurrentCalibrationData(float fTargetValue);

	void Set_VoltCalibrationData(float fTargetValue, float fMeasureValue);
	float Get_VoltCalibrationData(float fTargetValue);

	void Remove_MapCurrentCalData();
	void Remove_MapVoltCalData();
private:
	bool	m_bCancelTempSync = false;
	bool	m_bCancelStepSync = false;
	   	 
	int		m_nProcessStatus;
	int		m_nSyncChannel1;
	int		m_nSyncChannel2;	
	int		m_nSyncType;
	int		m_nSafetyNumber;
	int		m_nChannelNumber;
	int		m_nChannelStatus;		//channel status(main)
	int		m_nCycleIndex;		//변수이름..현재 사이클의 루프 카운트 이다..
	int		m_nCycleNumber;		//xml base
	int		m_nStepNumber;		//xml base
	int		m_nScheduleMode;
	int		m_nOperationMode;
	int		m_nChamberDelayTime;
	int		m_nEndSync;	
	int		m_nVPCheckTime;
	int		m_nVPCheckCnt;
	int		m_nIPCheckTime;
	int		m_nAccurateReason;	//SW에서 알 수 있는 조건들 enum ENDREASON_CODE 참고
	int		m_nRunningCount;		//total running count
	int		m_nReservePauseCycleNumber;
	int		m_nReservePauseStepNumber;
	int		m_nReserveCompleteCycleNumber;
	int		m_nReserveCompleteStepNumer;	// 예약 정지, 종료 근데 이 변수들.. 보내고 난 다음에 초기화 해야 하지 않나?
	int		m_nSystemSafetyReason; //lyw3_1 : 시스템 안전조건으로 종료 원인

	char	m_szFileID[64];
	char	m_szSchStart[64];

	bool	m_bNormalSync;
	bool	m_bStepSync;
	bool	m_bTempSync;
	bool	m_bChamberElevatedTemperature;
	bool	m_bScheduleStart;	// 처음 Schedule Loading 해서 Start 시키면 TRUE 로 변경
	bool	m_bPause;			// DSP 메시지에서 특정 전류, 전압 등의 Data 가 유효하지 않은 경우 TRUE 로 변경
	bool	m_bSelected;		// List Control 에서 현재 선택 되었는지 여부
	bool	m_bSafetyPause;
	bool	m_bAlarm;
	bool	m_bPauseCycle;
	bool	m_bPauseStep;
	bool	m_bCompleteCycle;
	bool	m_bCompleteStep;
	bool	m_bPattern;
	bool	m_bIsSendCommandJump;	//jump중복 되어 나가는 경우 때문에 한번만 보내도록
	bool	m_bIsCycleChange;
	bool	m_bIsStepChange;
	bool	m_bDataSendPacket;
	bool	m_bChannelEnd;
	bool	m_bChannelStart;
	bool	m_bChamberChage;
	bool	m_bSync;


	bool    m_bExternPatternCheck;

	int		m_nStepMode;
	int		g_nChannelStep[32];

	CString	m_strEndCondition; // 어떤 원인으로 종료 되었는지 Display
	CString m_strFileName;
	CString m_strScheduleName;
	CString m_strSampleName;
	CString m_strDescription;
	CString m_strRating;
	CString m_strLogFolderName;
	CString m_strRunChannel;
	CString m_strChamberChannel;
	CString m_strUserID;
	CString m_strCellID;
	CString m_strWorkName;
	ParallelMode	m_eMasterChannel;	// 병렬 모드에서 Master 인지 Slave 인지..
	EQMode	m_eEQModeSingle;

	CStep*			m_pStepTarget;
	CMgrSchedule*	m_pMgr;

	std::vector<SViewData>			m_vecViewData;
	std::vector<SMultiSchedule*>	m_vecMultiSchedule;

	//Key: Targer Current
	//Data : Measure Current

	std::map<float, float> m_mapCurrentCalibrationData;
	std::map<float, float> m_mapVoltCalibrationData;


	ULONGLONG m_ulBoardTime;

private:
	bool	CheckTempSync(int channel, int nStepIndex, int bIsStepChange);

	void	CheckNextStep(int nNextCycle, int nNextStep, ENDCOND* pCond);
	//void	CheckTotalSafetyCondition(ENDCOND* pCond);	// 2021-03-03 cnz --> 2022-01-14 YGY 사이클과 스텝 통합안전조건 구분을 위해 함수 수정 아래 함수 2개
	void	CheckCycleTotalSafetyCondition(ENDCOND* pCond);	// 2022-01-14 YGY Cycle 통합 안전 조건 즉시완료
	void	CheckStepTotalSafetyCondeition(ENDCOND* pCond); // 2022-01-14 YGY Step 통합 안전 조건 즉시완료
	void	CheckSystemSatefyCondition(ENDCOND* lpCompleteCondition);	// System 안전 조건 즉시완료
	void	CheckSystemSatefyCondition_Formation(ENDCOND* lpCompleteCondition);	//220316YGYUN System 안전 조건 즉시완료 (Formation)
	void	CheckContactRegistantConditon_Formation(ENDCOND* lpCompleteCondition);	//220614YGYUN Contact Registant 조건 즉시완료 (Formation)
	void	CheckEQCondition_Formation(ENDCOND* lpCompleteCondition);
	void	CheckScheduleSatefyCondition(ENDCOND* pCond);	// Schedule 안전 조건 즉시완료
	void	CheckCycleSafetyCondition(ENDCOND* pCond);	// Cycle 안전 조건 즉시완료
	void	CheckStepSafetyCondition(ENDCOND* pCond);	// Step 안전 조건 즉시완료
	void	CheckStepSafetyCondition_Formation(ENDCOND* pCond);	//220316YGYUN  Step 안전 조건 즉시완료
	void	CheckStepEndCondition(ENDCOND* pCond);		// 외부조건 즉시완료 or 일시정지 or 점프
	void	CheckUserEvent(ENDCOND* pCond);				// 유저가 조작 스탭 완료후 ~ 사이클 완료후 ~ 뭐뭐뭐
	void	SetSocTargetValue(float fValue, int nStepIndex);		//SOC 조건 비교를 위한 STEP의 현재 STEP AMPERE 값을 저장
	void	SetSocWattTargetValue(float fValue, int nStepIndex);	//SOC 조건 비교를 위한 STEP의 현재 STEP Watt 값을 저장
	void	SetSoHTargetValue(float fValue, int nStepIndex);		//SOC 조건 비교를 위한 STEP의 현재 STEP AMPERE 값을 저장
	void	SetSoHTargetValue2(float fValue, int nStepIndex);		//SOC 조건 비교를 위한 STEP의 현재 STEP AMPERE 값을 저장
	void	SetSOHTargetStep(CStep* pStepTarget);
	void	SetSafetyPause(bool bPause) { m_bSafetyPause = bPause; }	
	void	IncreaseRunningCount() { m_nRunningCount++; }
	void	SetVPCheckTime(int nTime) { m_nVPCheckTime = nTime; }
	void	SetVPCheckCnt(int nCount) { m_nVPCheckCnt = nCount; }
	void	SetIPCheckTime(int nTime) { m_nIPCheckTime = nTime; }
	bool	setPatternTemp(int channel, int nStepIndex);

	int		GetSafetyNumber();
	int		GetVPCheckTime() { return m_nVPCheckTime; }
	int		GetVPCheckCnt() { return m_nVPCheckCnt; }
	int		GetIPCheckTime() { return m_nIPCheckTime; }

	char*	GetSchStart() { return m_szSchStart; }

	bool	IsSocTarget();											//SOC 계산에 필요한 용량(기준 값) 을 가지는 STEP 인지 판단
	bool	IsSocUse();												//SOC 사용 유무 판단
	bool	IsSoHTarget();											//SOC 계산에 필요한 용량(기준 값) 을 가지는 STEP 인지 판단
	bool	IsSoHTarget2();											//SOC 계산에 필요한 용량(기준 값) 을 가지는 STEP 인지 판단
	bool	IsSoHUse();												//SOC 사용 유무 판단
	bool	GetChannelStart() { return m_bChannelStart; }
	bool	GetChamberChange() { return m_bChamberChage; }
	bool	GetSafetyPause() { return m_bSafetyPause; }
	bool	GetPauseStep() { return m_bPauseStep; }
	bool	GetPauseCycle() { return m_bPauseCycle; }
	bool	GetCompleteStep() { return m_bCompleteStep; }
	bool	GetCompleteCycle() { return m_bCompleteCycle; }	
	bool	IsSendCommandJump() { return m_bIsSendCommandJump; }
	bool	GetIsStepChange() { return m_bIsStepChange; }
	bool	GetIsCycleChange() { return m_bIsCycleChange; }

	CString	Int2Time(int nTime);
	
	CString OpCode2String();
	CString	GetRating() { return m_strRating; }	

// -------------------------------------------------------------------------------------------------
// PCAN
public:
	void	SetParallelObj(int nObj) { m_nParallelObj = nObj; }
	int		GetParallelObj() { return m_nParallelObj;  }
	
	void	SetMasterChannel(int nChannelNumber) { m_nMasterChannel = nChannelNumber; }
	int		GetMasterChannel() { return m_nMasterChannel; }

	void	SetSlaveChannel(int nChannelNumber) { m_nSlaveChannel = nChannelNumber; }
	int		GetSlaveChannel() { return m_nSlaveChannel; }

	void	SetMasterPercent(int nValue) { m_nMasterPercent = nValue; }
	int		GetMasterPercent() { return m_nMasterPercent; }

	void	SetSlavePercent(int nValue) { m_nSlavePercent = nValue; }
	int		GetSlavePercent() { return m_nSlavePercent; }

public:
	//CString	GetChannelStatusToString();

private:
	int		m_nParallelObj;		// 병렬 모드에서 Master 인지 Slave 인지..
	int		m_nMasterChannel;
	int		m_nMasterPercent;
	int		m_nSlaveChannel;
	int		m_nSlavePercent;
	float	m_fSplitCondition;	// 조과장님 말로는, 특정 Ampere 이상에서 나눠야 한다는데?

	int		m_nStatus;
};

