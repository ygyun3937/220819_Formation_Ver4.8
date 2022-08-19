#pragma once
#include <map>
#include <vector>

enum eSyatemSafetyObject
{
	eSyatemSafetyObjectVoltage = 0,
	eSyatemSafetyObjectCurrent,
	eSyatemSafetyObjectCapacity,
	eSyatemSafetyObjectTemperature,
	eSyatemSafetyObjectDeltaV,
	eSyatemSafetyObjectDeltaI,
	eSyatemSafetyObjectDeltaTime,
	eSyatemSafetyObjectErrorLoggingTime,
	eSyatemSafetyObjectCvDeltaV,
	eSyatemSafetyObjectCvDeltaA,
	eSyatemSafetyObjectMaxCount
};

enum CyclerType
{
	CyclerTypeCell = 0,
	CyclerTypeModule,
	CyclerTypePack
};


struct MoxaIOInfo
{
	CString strIP;
	int nChCount;
	CString nIOType;
	int nHandle;
	BOOL		bConnect;

	MoxaIOInfo()
	{
		strIP = "";
		nChCount = 0;
		nIOType = "";
		nHandle = 0;
		bConnect = FALSE;
	}
};

//System 안전조건 Get Set
//안전조건 수정 시 Set Value 조건 확인 후 Set 진행
struct SSystemSafety
{
	CString strName;
	CString strValue;

	bool	bIsWarning;

	float	fValue;

	SSystemSafety()
	{
		bIsWarning = false;

		fValue = 0.000f;
	}

	void	SetValue(bool bValidatate, CString strCondition)
	{
		fValue = (float)atof(strCondition);

		bIsWarning = false;

		if (true == bValidatate && strCondition.IsEmpty())
			bIsWarning = true;		

		strValue.Format("%.3f", fValue);

		if (true == bValidatate && strValue.Compare("0.000") == 0)
			bIsWarning = true;
	}
};

//통합 안전조건 구조체
typedef struct _totalSafety 
{
	float fInputCapacity;
	float fCapacity;
	float fVoltage;
	float fCurrent;
	INT64 nTestEndTime;
	INT64 nNoVoltageVariation;
	float fVoltageVariation;
	float fChargeCapacityVariation;
	float fDischargeCapacityVariation;
	float fChamberMaxTemp;
	float fSampleMaxTemp;
	float fSOH;
	CString fCapPercent;	//yuri xml 통합안전조건 값을 xml파일에 추가
	CString fsohPercent;	//yuri xml 통합안전조건 값을 xml파일에 추가
	CString strTestEndTime; //yuri xml 통합안전조건 값을 xml파일에 추가

} TOTAL_SAFETY, * pTOTAL_SAFETY;

// -----------------------------------------------------------------------------

//장비 정보 관련 구조체
typedef struct _SubEquipmentInfo
{
	int		nSubEqpNo;
	CString strSubEqpId;
	CString strStage1Name;
	CString strStage2Name;

	bool bStage1Use;
	bool bStage2Use;
	
	int		nSubEqpChannelMaxCount;
	int		nSubEqpStageMaxCount;
	int		nSubEqpStageChannelCount;
	int		nSubEqpStageChannelTotalCount;
} SUBEQP_INFO, *pSubEqp_Info;

// -----------------------------------------------------------------------------

#include "Singleton.h"
#include <vector>

class CMgrConfig : public CSingleton< CMgrConfig >
{
public:
	CMgrConfig();
	virtual ~CMgrConfig();

public:
	void	SaveCommon();
	void	GetChannelRunningTotal();
	CString	GetVersionStr();
	void	GetChannelContactResistance();
public:
	void	GetSystemSafeInfoMin(std::vector<CString>& vecValue);
	void	GetSystemSafeInfoMax(std::vector<CString>& vecValue);

	CString	GetSystemSafeInfoTempMax();

	void	SetSystemSafeInfoMin(int nIndex, CString strValue);
	void	SetSystemSafeInfoMax(int nIndex, CString strValue);

	float	GetSystemSafetyMin(int nIndex);
	float	GetSystemSafetyMax(int nIndex);

	void	SetDeltaIsUse(bool bUseDeltaV) { m_bUseDeltaV = bUseDeltaV; }
	void	SetCvDeltaIsUse(bool bUseCvDeltaV) { m_bUseCvDeltaV = bUseCvDeltaV; }
	void	SetCyclerLogFilePath(CString strValue) { m_strLogFilePath = strValue; }
	void	SetCyclerSchedulePath(CString strValue) { m_strSchedulePath = strValue; }

	bool	GetIsDeltaIsUse() { return m_bUseDeltaV; }
	bool	GetIsCvDeltaIsUse() { return m_bUseCvDeltaV; }

	int		GetMicroV() { return m_nMicroV; }
	int		GetMicroA() { return m_nMicroA; }

	float	GetEQVoltageMin() { return m_fEQVoltageMin; }
	float	GetEQVoltageMax() { return m_fEQVoltageMax; }
	float	GetEQAmpereMin() { return m_fEQAmpereMin; }
	float	GetEQAmpereMax() { return m_fEQAmpereMax; }

	float	GetConnectVolt() { return m_fConnectVolt; }
	void	SetConnectVolt(float fConnectVolt) { m_fConnectVolt = fConnectVolt; }
	CString GetConnectVoltStr();

	void	SetEQMode(int nMode) { m_nEQMode = nMode; }
	int		GetEQMode() { return m_nEQMode; }
	
	BOOL	GetLogUse() { return m_bLogUse; }
	BOOL	GetDAQType() { return m_bDAQType; }

	BOOL	GetDebugLogUse() { return m_bDebugLog; }

	CString	GetLogFilePath() { return m_strLogFilePath.GetString(); }
	CString GetSchedulePath() { return m_strSchedulePath.GetString(); }
	CString GetCyclerEQPID() { return m_strCyclerEQPID.GetString(); }
	CString	GetDAQFilePath() { return m_strDAQFilePath.GetString(); }

	bool	IsErrorFilePath() { return m_bErrorFilePath; }
	bool	IsErrorSystemSafety() { return m_bErrorSystemSafety; }

	void	SetErrorFilePath(bool bError) { m_bErrorFilePath = bError; }

	TOTAL_SAFETY	GetTotalSafety(int nChNum);
	void			SetTotalSafety(int nChNum, TOTAL_SAFETY totalSafety);

	int		GetChamberChannelCount() { return m_nChamberChannelCount;  }

	CString GetDcirTimeINI() { return m_strDcirTime; }

	int		GetCyclerType() { return m_nCyclerType; }
	int		GetParallelChannelCount() { return m_nParallelChannelCount; }
	int		GetCChannelCountInOneBoard() { return m_nChannelCountInOneBoard; }

	CString	GetDioName() { return m_strDioType; }
	CString	GetDioIniName() { return m_strDioIniName; }
	void	SetDioIniName(CString strName) { m_strDioIniName = strName; }

	int		GetDioCount() { return m_nDioCount; }
	int		GetTotalSubEqpCount() { return m_nTotalSubEqpCount; }

	// SUBEQP_INFO
	void		SetSubEqpInfo(SUBEQP_INFO subeqpInfo);
	SUBEQP_INFO GetSubEqpInfo();

	// AUTO_CALIBRATION

	void	SetCalibratorIP(CString ipAddress) { m_strCalibratorIP = ipAddress; }
	CString	GetCalibratorIp() { return m_strCalibratorIP; }

	void	SetCalibratorPort(int nPortNo) { m_nCalibratorPort = nPortNo; }
	int		GetCalibratorPort() { return m_nCalibratorPort; }

	void	SetCalibratorUseChannelCount(int value) { m_nCalibratorUseChannelCount = value; }
	int		GetCalibratorUseChannelCount() { return m_nCalibratorUseChannelCount; }

	void	SetCalibratorUseVoltChannelCount(int value) { m_nCalibratorUseVoltChannelCount = value; }
	int		GetCalibratorUseVoltChannelCount() { return m_nCalibratorUseVoltChannelCount; }

	void	SetCalibratorUseCurrentChannelCount(int value) { m_nCalibratorUseCurrentChannelCount = value; }
	int		GetCalibratorUseCurrentChannelCount() { return m_nCalibratorUseCurrentChannelCount; }

	void	SetCalibratorVoltageChannel(CString value) { m_strVoltageChannel = value; }
	CString	GetCalibratorVoltageChannel() { return m_strVoltageChannel; }

	void	SetCalibratorCurrentChannel(CString value) { m_strCurrentChannel = value; }
	CString	GetCalibratorCurrentChannel() { return m_strCurrentChannel; }

	CString	GetMeasureMode() { return m_strCalibrationMode; }
	void	SetMeasureMode(CString strMeasureMode) { m_strCalibrationMode= strMeasureMode; }
	int		GetMeasureModeNo();

	void	SetCalibrationAutoMode(bool value) { m_bAutoMode = value; }
	bool	GetCalibrationAutoMode() { return m_bAutoMode; }
	
	void	SetShuntValue(float fValue) { m_fShuntValue = fValue; }
	float	GetShuntValue() { return m_fShuntValue; }

	void	SetShuntValues(CString sValue) { m_strShuntValues = sValue; }
	CString	GetShuntValues() { return m_strShuntValues; }


	void GetMoxaIoInfo(std::vector<MoxaIOInfo*>& vecValue);
	void SetMoxaIoInfor(int nModuleNo,std::vector<MoxaIOInfo*> vecValue);


	std::vector<float>	GetCurrentCalibrationTargetValue() { return m_vecCurrentCalibrationTargetValue; }
	void	SetCurrentCalibrationTargetValue(float fTargetValue);
	std::vector<float> m_vecCurrentCalibrationTargetValue;

public:
	CString m_strDcirTime;

	CString m_strDioType;			//IO Type 

	bool m_bPLCControlUse;
	CString m_strPLCControlType;

	int		m_nDioCount;

	CString m_strDioIniName;

private:
	void	LoadCommon();
	void	SaveRunningCount();

private:
	CString	m_strProjectName;		// 고객사 명칭
	int		m_nCyclerType;			// Cell = 0, Module = 1, Pack = 2
	int		m_nEQNumber;			// 설비 호기

	int		m_nMicroV;
	int		m_nMicroA;

	bool	m_bErrorFilePath;		// 설정 중 파일 경로에 대한 항목이 이상할 경우 팝업
	bool	m_bErrorSystemSafety;
	bool	m_bUseDeltaV;
	bool    m_bUseCvDeltaV;

	// EQ Property -> Pattern 시 사용
	float	m_fEQVoltageMin;
	float	m_fEQVoltageMax;
	float	m_fEQAmpereMin;
	float	m_fEQAmpereMax;

	//220529 YGY Battery Connect 기준 Volt 값
	float	m_fConnectVolt;

	int		m_nEQMode;

	BOOL	m_bLogUse;
	BOOL	m_bDAQType;

	bool	m_bDebugLog;			//220318 FW Debug용 (0 : Op Code 3이하인 경우 전류, 용량 값 0으로 저장, 1 : 전체 데이터 Raw Data로 기록)

	int		m_nChamberChannelCount;
	int		m_nParallelChannelCount;
	int		m_nChannelCountInOneBoard;


	CString m_strLogFilePath;		// Local PC 어디에 저장될 것인지
	CString m_strSchedulePath;		// Schedule 저장되어 있는 경로
	CString m_strCyclerEQPID;
	CString m_strDAQFilePath;		// 2021-03-02

	// ----------------------------------------------------------------------------

	TOTAL_SAFETY m_stTotalSafety[100];					// 통합 안전조건
	std::vector<SSystemSafety*> m_vecSystemSafetyMin;
	std::vector<SSystemSafety*> m_vecSystemSafetyMax;

	std::vector<MoxaIOInfo*> m_vecMoxaDevice;


	// ----------------------------------------------------------------------------

	// SUB EQUIPMENT
	SUBEQP_INFO m_SubEqpInfo;
	int		m_nTotalSubEqpCount;

	// CALIBRATION
	CString	m_strCalibratorIP;
	int		m_nCalibratorPort;
	int		m_nCalibratorUseVoltChannelCount;
	int		m_nCalibratorUseCurrentChannelCount;
	int		m_nCalibratorUseChannelCount;
	CString m_strVoltageChannel;
	CString m_strCurrentChannel;

	CString	m_strCalibrationMode;
	int		m_nCalibrationModeNo;

	float	m_fShuntValue;
	CString m_strShuntValues;

	int		m_nInitialEQVoltV;
	int		m_nInitialEQCurrA;
	float	m_fCalibrationVoltV;
	float	m_fCalibrationCurrA;
	bool	m_bAutoMode;
};

