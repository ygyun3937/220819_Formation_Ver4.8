#pragma once

#include "Pattern.h"

struct SCurrentMap
{
	float	fMinTemp;
	float	fMaxTemp;
	float	fMinAmpere;
	float	fMaxAmpere;

	SCurrentMap()
	{
		fMinTemp = 0.0;
		fMaxTemp = 0.0;
		fMinAmpere = 0.0;
		fMaxAmpere = 0.0;
	}

	void	CopyData(SCurrentMap* pSrc)
	{
		this->fMinTemp = pSrc->fMinTemp;
		this->fMaxTemp = pSrc->fMaxTemp;
		this->fMinAmpere = pSrc->fMinAmpere;
		this->fMaxAmpere = pSrc->fMaxAmpere;
	}

	void	Parse(CStringArray& arr)
	{
		for (int i = 0; i < arr.GetSize(); ++i)
		{
			CString strValue = arr.GetAt(i);

			switch (i)
			{
			case 0: fMinTemp = (float)atof(strValue);	break;
			case 1: fMinAmpere = (float)atof(strValue);	break;
			case 2: fMaxAmpere = (float)atof(strValue);	break;
			case 3: fMaxTemp = (float)atof(strValue);	break;
			}
		}
	}
};

struct SScheduleGrade
{
	float	fMinValue;
	int		nMinSign;
	int		nType;
	int		nMaxSign;
	float	fMaxValue;
	CString strRating;

	SScheduleGrade()
	{
		fMinValue = 0.0;
		nMinSign = 0;
		nType = 0;
		nMaxSign = 0;
		fMaxValue = 0;
		strRating.Empty();
	}

	void	CopyData(SScheduleGrade* pSrc)
	{
		this->fMinValue = pSrc->fMinValue;
		this->nMinSign	= pSrc->nMinSign;
		this->nType		= pSrc->nType;
		this->nMaxSign	= pSrc->nMaxSign;
		this->fMaxValue = pSrc->fMaxValue;
		this->strRating = pSrc->strRating;
	}

	void	Parse(CStringArray& arr)
	{
		for (int i = 0; i < arr.GetSize(); ++i)
		{
			CString strValue = arr.GetAt(i);

			switch (i)
			{
			case 0: fMinValue	= (float)atof(strValue);	break;
			case 1: nMinSign	= atoi(strValue);			break;
			case 2: nType		= atoi(strValue);			break;
			case 3: nMaxSign	= atoi(strValue);			break;
			case 4: fMaxValue	= (float)atof(strValue);	break;
			case 5: strRating	= strValue;					break;
			}
		}
	}
};

struct SScheduleComplete
{
	CString strName;
	CString strValue;	// 해당 Data 의 모든 변수가 float 는 아니다. 시간 같은 경우 해당 변수로 처리
	float	fValue;
	int		nCycle;
	int		nStep;

	SScheduleComplete()
	{
		strName.Empty();
		strValue.Empty();

		fValue = 0.000;
		nCycle = 0;
		nStep = 0;
	}

	void	CopyData(SScheduleComplete* pSrc)
	{
		this->strName = pSrc->strName;
		this->strValue = pSrc->strValue;
		this->fValue = pSrc->fValue;
		this->nCycle = pSrc->nCycle;
		this->nStep = pSrc->nStep;
	}

	void	ParseComplete(CStringArray& arr)
	{
		for (int i = 0; i < arr.GetSize(); ++i)
		{
			CString str = arr.GetAt(i);

			switch (i)
			{
				case 0: strName = str; break;
				case 1:
				{
					strValue	= str;
					fValue		= (float)abs(atof(str));
					break;
				}
				case 2:
				{
					int nSepIndex = str.Find(_T(":"));

					if (nSepIndex >= 0)
					{
						nCycle	= atoi(str.Left(nSepIndex));
						nStep	= atoi(str.Mid(nSepIndex + 1));
					}
					else
					{
						int nSep1 = str.Find(_T("C"));
						int nSep2 = str.Find(_T("S"));

						nCycle	= atoi(str.Mid(nSep1 + 1, nSep2));
						nStep	= atoi(str.Mid(nSep2 + 1));
					}
					break;
				}
			}
		}
	}

	void	ParseCapacity(CStringArray& arr)
	{
		strName = "PatternCap";

		for (int i = 0; i < arr.GetSize(); ++i)
		{
			CString str = arr.GetAt(i);

			switch (i)
			{
				case 0:
				{
					strValue	= str;
					fValue		= (float)atof(str);

					break;
				}
				case 1:
				{
					int nSepIndex = str.Find(_T(":"));

					if (nSepIndex >= 0)
					{
						nCycle = atoi(str.Left(nSepIndex));
						nStep = atoi(str.Mid(nSepIndex + 1));
					}
				}
			}
		}
	}
};

struct SScheduleSafety
{
	CString strName;
	float	fValue;

	SScheduleSafety()
	{
		strName.Empty();

		fValue = 0.000;
	}

	void	CopyData(SScheduleSafety* pSrc)
	{
		this->strName	= pSrc->strName;
		this->fValue	= pSrc->fValue;
	}

	void	Parse(CStringArray& arr)
	{
		for (int i = 0; i < arr.GetSize(); ++i)
		{
			CString strValue = arr.GetAt(i);

			switch (i)
			{
			case 0: strName = strValue;					break;
			case 1: fValue	= (float)atof(strValue);	break;
			}
		}
	}
};

struct SScheduleLog
{
	CString strName;
	float	fTime;	// GUI 용 Data
	int		nTime;
	bool	bLog;

	SScheduleLog()
	{
		strName.Empty();

		fTime = 0.000;
		nTime = 0;
		bLog = false;
	}

	void	CopyData(SScheduleLog* pSrc)
	{
		this->strName	= pSrc->strName;
		this->fTime		= pSrc->fTime;
		this->nTime		= pSrc->nTime;
		this->bLog		= pSrc->bLog;
	}

	void	Parse(CStringArray& arr)
	{
		bool bExistLog = false;

		for (int i = 0; i < arr.GetSize(); ++i)
		{
			CString strValue = arr.GetAt(i);

			switch (i)
			{
				case 0:
				{
					strName = strValue;

					if (strcmp(strName, "Log") == 0)
						bExistLog = true;
					else
						bExistLog = false;

					break;
				}
				case 1:
				{
					if (bExistLog)
					{
						if ((int)atoi(strValue) == 0)
							bLog = false;
						else
							bLog = true;
					}

					
					fTime = (float)atof(strValue);
					nTime = (int)(fTime * 10);

					if (nTime == 0)
						nTime = 1;

					break;
				}
			}
		}
	}
};

struct SDCIRLog
{
	int		nDCIR1;
	int		nDCIR2;
	int		nDCIR3;

	SDCIRLog()
	{
		nDCIR1 = 0;
		nDCIR2 = 0;
		nDCIR3 = 0;
	}

	void	CopyData(SDCIRLog* pSrc)
	{
		this->nDCIR1 = pSrc->nDCIR1;
		this->nDCIR2 = pSrc->nDCIR2;
		this->nDCIR3 = pSrc->nDCIR3;
	}

	void	Parse(CStringArray& arr)
	{
		for (int i = 0; i < arr.GetSize(); ++i)
		{
			CString str = arr.GetAt(i);

			switch (i)
			{
			case 0: nDCIR1 = atoi(str); break;
			case 1: nDCIR2 = atoi(str); break;
			case 2: nDCIR3 = atoi(str); break;
			}
		}
	}
};

struct SDaQComplete
{
	int		nUniqueID;
	int		nOperator;
	int		nNextCycle;
	int		nNextStep;
	float	fValue;

	SDaQComplete()
	{
		nUniqueID	= 0;
		nOperator	= 0;
		fValue		= 0.000;
		nNextCycle	= 0;
		nNextStep	= 0;
	}

	void	CopyData(SDaQComplete* pSrc)
	{
		this->nUniqueID		= pSrc->nUniqueID;
		this->nOperator		= pSrc->nOperator;
		this->fValue		= pSrc->fValue;
		this->nNextCycle	= pSrc->nNextCycle;
		this->nNextStep		= pSrc->nNextStep;
	}

	void	Parse(CStringArray& arr)
	{
		for (int i = 0; i < arr.GetSize(); ++i)
		{
			CString str = arr.GetAt(i);

			switch (i)
			{
			case 0: nUniqueID = atoi(str);		break;
			case 1: nOperator = atoi(str);		break;
			case 2: fValue = (float)atof(str);	break;
			case 3: nNextCycle = atoi(str);		break;
			case 4: nNextStep = atoi(str);		break;
			}
		}
	}
};

#define	ChamberDefaultSetting	( -100.000 )
#define	ChillerDefaultSetting	( -100.000 )

#include <vector>

class CStep
{
public:
	CStep();
	virtual ~CStep();

	void	SetStepType(CString strStepType);
	void	SetStepMode(CString strStepMode);
	void	SetStepPattern();
	void	SetChargeVol_limit();
	void	SetDisChargeVol_limit();
	void	SetChargeCurr_limit();
	void	SetDisChargeCurr_limit();
	void	SetChargeVoltage(CString strVoltage);
	void	SetDischargeVoltage(CString strVoltage);
	float GetDischargeVoltage();
	void	SetChargeCurrentAmpere(CString strAmpere);
	void	SetDisChargeCurrentAmpere(CString strAmpere);
	void	SetWatt(CString strWatt);
	void	SetCycleLoopCount(int nCount);
	void	SetStepEndIndex(int nType, int nIndex);
	void	SetPatternFilePath(CString strFilePath);
	void	SetFinishCondition(LPCTSTR lpszString);
	void	SetSafetyCondition(LPCTSTR lpszString);
	void	SetLogRecodeTerm(LPCTSTR lpszString);
	void	SetChamberCondition(LPCTSTR lpszString);
	void	SetRatioCondition(LPCTSTR lpszString);
	void	SetRatioConditionSOH(LPCTSTR lpszString);
	void	SetPatternCapacity(LPCTSTR lpszString);
	void	SetDAQCondition(LPCTSTR lpszString);
	void	SetDCIR(LPCTSTR lpszString);
	void	GetStepInfo(std::vector<CString>& vecValue);
	void	GetCompleteInfo(std::vector<CString>& vecValue);
	void	SetChargeVoltage(float fVoltage) { m_fChargeVoltage = fVoltage; }
	void	SetDischargeVoltage(float fVoltage) { m_fDischageVoltage = fVoltage; }
	void	SetChargeCurrentAmpere(float fAmpere) { m_fChargeCurrent = fAmpere; }
	void	SetDisChargeCurrentAmpere(float fAmpere) { m_fDisChargeCurrent = fAmpere; }
	void	SetPatternTemp(float fTemp) { m_fPatternTemp = fTemp; }
	void	SetWatt(float fWatt) { m_fWatt = fWatt; }
	void	SetCycleIndex(int nCycleIndex) { m_nCycleIndex = nCycleIndex; }
	void	SetCycleNumber(int nCycleNumber) { m_nCycleNumber = nCycleNumber; }
	void	SetStepNumber(int nStepNumber) { m_nStepNumber = nStepNumber; }
	void	SetPatternTime(int nTime) { m_nPatternTime = nTime; }
	void	SetRatioType(int nSoc) { m_nRatioType = nSoc; }
	void	SetStepIndex(int nIndex) { m_nStepIndex = nIndex; }
	void	SetPrevStepIndex(int nIndex) { m_nPrevStepIndex = nIndex; }
	void	SetPatternPeriod(int nPeriod) { m_nPatternPeriod = nPeriod; }
	void	SetLoopNextCycle(int nCycle) { m_nLoopNextCycle = nCycle; }
	void	SetSyncType(int nSyncType) { m_nSyncType = nSyncType; }
	void	SetPTNEndTime(int nTime) { m_nPTNEndTime = nTime; }
	void	SetIsPattern(BOOL bPattern) { m_bPattern = bPattern; }
	void	SetChamberSetTemp(float fChamberSetTemp) { m_fChamberSettingTemperature = fChamberSetTemp; }
	void	SetSocTarget(BOOL bTarget) { m_bSocTarget = bTarget; }
	void	SetSoHTarget(BOOL bTarget) { m_bSoHTarget = bTarget; }
	void	SetSocTargetValue(float fValue) { m_fSocTargetValue = fValue; }
	void	SetSoHTarget2(BOOL bTarget) { m_bSoHTarget2 = bTarget; }
	void	SetSocWattTargetValue(float fValue) { m_fSocWattTargetValue = fValue; }
	void	SetSoHTargetValue(float fValue) { m_fSoHTargetValue = fValue; }
	void	SetSoHTargetValue2(float fValue) { m_fSoHTargetValue2 = fValue; }
	void	SetEndSync(int nEndSync) { m_nEndSync = nEndSync; }
	float	GetChargeCurrentLimit() { return m_fChargeCurr_limit; }
	float	GetDischargeCurrentLimit() { return m_fDisChargeCurr_limit; }
	float	GetChargeVol_limit() { return m_fChargeVol_limit; }
	float 	GetDisChargeVol_limit() { return m_fDisChargeVol_limit; }
	void	SetStepEndReason(int nEndReason) { m_nStepEndReason = nEndReason; }


	int		GetStepEndIndex(int nType);
	int		GetOperationMode();	
	int		GetCycleIndex() { return m_nCycleIndex; }
	int		GetCycleNumber() { return m_nCycleNumber; }
	int		GetStepNumber() { return m_nStepNumber; }
	int		GetStepType() { return m_nStepType; }
	int		GetStepPattern() { return m_nStepPattern; }
	int		GetStepMode() { return m_nStepMode; }
	int		GetPatternTime() { return m_nPatternTime; }
	int		GetRatioLoop() { return m_nRatioLoop; }
	int		GetRatioSOHLoop1() { return m_nSOHLoop1; }
	int		GetRatioSOHLoop2() { return m_nSOHLoop2; }
	int		GetRatioCycleSOH2() { return m_nRatioCycleSOH2; }
	int		GetRatioStepSOH2() { return m_nRatioStepSOH2; }
	int		GetRatioCycleSOH() { return m_nRatioCycleSOH; }
	int		GetRatioStepSOH() { return m_nRatioStepSOH; }
	int		GetRatioType() { return m_nRatioType; }
	int		GetSOHReference() { return m_nSOHReference; }
	int		GetRatioSoH() { return m_nRatioSoH; }
	int		GetRatioSoc() { return m_nRatioSoc; }
	int		GetRatioCycle() { return m_nRatioCycle; }
	int		GetRatioStep() { return m_nRatioStep; }
	int		GetStepIndex() { return m_nStepIndex; }
	int		GetPrevStepIndex() { return m_nPrevStepIndex; }
	int		GetPatternPeriod() { return m_nPatternPeriod; }
	int		GetCycleLoopCount() { return m_CycleLoopCount; }
	int		GetLoopNextCycle() { return m_nLoopNextCycle; }
	int		GetSyncType() { return m_nSyncType; }	
	int		GetSize() { return m_pPattern->GetSize(); }
	int 	GetEndSync() { return m_nEndSync; }

	bool	GetChamberElevatedTemperature() { return m_bChamberElevatedTemperature; }
	bool	GetChamberStart() { return m_bChamberStart; }

	float	GetChargeVoltage() { return m_fChargeVoltage; }
	float	GetDisChargeVoltage() { return m_fDischageVoltage; }
	float	GetCurrentAmpere() { return m_fChargeCurrent; }
	float	GetDisCurrentAmpere() { return m_fDisChargeCurrent; }
	float	GetPatternTemp() { return m_fPatternTemp; }
	float	GetWatt() { return m_fWatt; }
	float	GetChamberSetTemp() { return m_fChamberSettingTemperature; }
	float	GetSocTargetValue() { return m_fSocTargetValue; }
	float	GetSocWattTargetValue() { return m_fSocWattTargetValue; }
	float	GetSoHTargetValue() { return m_fSoHTargetValue; }
	float	GetSoHTargetValue2() { return m_fSoHTargetValue2; }
	float	GetChargeCurr() { return m_fChargeCurr_limit; }
	float	GetDisChargeCurr() { return m_fDisChargeCurr_limit; }
	float	GetChargeVol() { return m_fChargeVol_limit; }
	float	GetDisChargeVol() { return m_fDisChargeVol_limit; }
	int		GetPTNEndTime() { return m_nPTNEndTime; }
	CString	GetTitleToValue(CString strTitle, int nCurChannel);

	BOOL	CopyData(CStep* pSrc, int nIndex = -1);
	BOOL	IsEmptyValue() { return m_bIsEmptyValue; }
	BOOL	GetIsPattern() { return m_bPattern; }
	BOOL	GetSocTarget() { return m_bSocTarget; }
	BOOL	GetSoHTarget() { return m_bSoHTarget; }
	BOOL	GetSoHTarget2() { return m_bSoHTarget2; }

	CString StepType2String();
	CString StepEndReason2String();
	CString StepEndReason2String(int nEndReason); //lyw3_1
	CString	StepType2Str();
	CString StepMode2Str();
	CString StepCompleteCondition2String();
	CString	GetPatternFilePath() { return m_strPatternFilePath; }
	CString	GetChamberDelayTime() { return m_strChamberDelayTime; }
	void	Condition2Array(__out CStringArray& arr);
	void	Step2Array(__out CStringArray& arr, int nCurChannel, CStringArray& arrRef);

	CPattern*	GetPattern();

	SScheduleComplete*	GetScheduleCompleteFromName(CString strName);
	SScheduleSafety*	GetScheduleSafetyFromName(CString strName);
	SScheduleLog*		GetScheduleLogFromName(CString strName);

	std::vector<SScheduleComplete*>&	GetScheduleComplete() { return m_vecScheduleComplete; }
	std::vector<SScheduleSafety*>&		GetScheduleSafety() { return m_vecScheduleSafety; }
	std::vector<SScheduleLog*>&			GetScheduleLog() { return m_vecScheduleLog; }
	std::vector<SDaQComplete*>&			GetDaQComplete() { return m_vecDaQComplete; }
	std::vector<SDCIRLog*>&				GetDCIRLog() { return m_vecDCIRLog; }

public:
	// AUTO_CALIBRATION
	float	GetChargeCurrent() { return m_fChargeCurrent; }
	float	GetDisChargeCurrent() { return m_fDisChargeCurrent; }

	// AUTO_CALIBRATION_AVERAGE
	CList<float, float> lstAvgCalibratorAmpere;
	CList<float, float> lstAvgFormationAmpere;
	float	m_fAvgCalibratorAmpere;
	float	m_fAvgFormationAmpere;

private:
	int		m_nCycleIndex;
	int		m_nCycleNumber;	//xml base
	int		m_nStepNumber;	//xml base
	int		m_nStepPattern;
	int		m_nStepType;
	int		m_nStepMode;
	int		m_nRatioSoH;
	int		m_nRatioCycleSOH;
	int		m_nRatioStepSOH;
	int		m_nRatioCycleSOH2;
	int		m_nRatioStepSOH2;
	int		m_nSOHReference;
	int		m_nSOHLoop1;
	int		m_nSOHLoop2;
	int		m_nRatioSoc;
	int		m_nRatioLoop;
	int		m_nRatioCycle;
	int		m_nRatioStep;
	int		m_nRatioType;
	int		m_nEndSync;
	int		m_nStepIndex;		//fw base
	int		m_nPrevStepIndex;	// Loop Count 조건 비 만족 시 돌아가야 할 Index 정보
	int		m_nPatternPeriod;	// Pattern 일 경우 진행 해야 하는 시간
	int		m_CycleLoopCount;	// Cycle 반복 횟수
	int		m_nLoopNextCycle;	// 조건 만족시 가야할곳..
	int		m_nPTNTotalTime;	// PTN 돌릴 시 전체 시간
	int		m_nPTNEndTime;		// PTN 돌릴 시 Step 에 대한 시간 종료 조건
	int		m_nPatternTime;	
	int		m_nChamberDelayTime;
	int		m_nChamberStartEnd;
	int		m_nSyncType;
	int		m_nStepEndReason;
	int		m_nStepEndIndex[eIFBoardCompleteMaxCount]; // 조건 별 이동해야 하는 곳..

	// Add By JSK - 2020.08.06
	// Add By turby 20201103
	int		m_nChargeCurrentLimit;
	int		m_nDischargeCurrentLimit;

	bool	m_bChamberElevatedTemperature;

	bool	m_bChamberStart;

	float	m_fSocTargetValue;
	float	m_fSoHTargetValue;
	float	m_fSoHTargetValue2;
	float	m_fSocWattTargetValue;
	float	m_fChargeVoltage;
	float	m_fDischageVoltage;
	float	m_fChargeCurrent;
	float	m_fDisChargeCurrent;
	float	m_fPatternTemp;
	float	m_fWatt;
	float	m_fChamberSettingTemperature;
	float	m_fPatternCapacity;
	
	float	m_fChargeVol_limit;
	float	m_fDisChargeVol_limit;
	float	m_fChargeCurr_limit;
	float	m_fDisChargeCurr_limit;

	BOOL	m_bSocTarget;
	BOOL	m_bSoHTarget;
	BOOL	m_bSoHTarget2;
	BOOL	m_bPattern;
	BOOL	m_bIsEmptyValue;

	CString	m_strChamberDelayTime;
	CString	m_strPatternFilePath;

	CPattern*	m_pPattern;

	std::vector<SScheduleSafety*>	m_vecScheduleSafety;
	std::vector<SScheduleComplete*>	m_vecScheduleComplete;
	std::vector<SScheduleLog*>		m_vecScheduleLog;
	std::vector<SDaQComplete*>		m_vecDaQComplete;
	std::vector<SDCIRLog*>			m_vecDCIRLog;

private:
	void	RemoveScheduleComplete();
	void	RemoveScheduleSafety();
	void	RemoveScheduleLog();
	void	RemoveDaQComplete();
	void	RemoveDCIRLog();
};

