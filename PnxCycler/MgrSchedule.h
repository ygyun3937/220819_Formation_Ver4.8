#pragma once

#include "Singleton.h"

#include "TinyXml.h"
#include "TiXmlHelper.h"

#include "Cycle.h"
#include "CircularQueue.h"

struct SScheduleValidate
{
	int		nCycleNumber;
	int		nStepNumber;
	int		nValidateCode;

	SScheduleValidate()
	{
		nCycleNumber	= 0;
		nStepNumber		= 0;
		nValidateCode	= eScheduleValidateNoError;
	}
};


struct SScheduleSafetyCheck
{
	int		nErrorCode;

	SScheduleSafetyCheck()
	{
		nErrorCode = eScheduleSafetyCheckNoError;
	}
};

class stSVDF
{
public:
	float fMaxVolt = 0;
	float fAvgVolt = 0;
	float fSOCUpperDrop = 0.00f;
	float fSOCLowerDrop = 0.00f;
	int   nErrCount = 3;
	bool  bVentOpen = false;
	bool  bUseSVDF = false;
	int	  nCurrErrCount = 0;

	void Clear()
	{
		fMaxVolt = 0;
		fAvgVolt = 0;
		fSOCUpperDrop = 0.00f;
		fSOCLowerDrop = 0.00f;
		bVentOpen = false;
		nCurrErrCount = 0;
		bUseSVDF = false;
		nErrCount = 3;
	}

	void	AddErrorCount() { nCurrErrCount++; }
	int		GetCurrErrCount() { return nCurrErrCount; }
};

enum eSVDFField
{
	SVDF_MAX_VOLT = 0,
	SVDF_AVG_VOLT,
	SVDF_SOC_UPPER_DROP,
	SVDF_SOC_LOWER_DROP,
	SVDF_ERR_COUNT,
	SVDF_VENT_OPEN
};

class CMgrSchedule
{
public:
	CMgrSchedule();
	virtual ~CMgrSchedule();

	void	GetScheduleSafetyInfo(std::vector<CString>& vecValue);
	void	ClearSchedule();

	int		GetLastStepIndex();
	int		GetCycleLastIndex(int nStepIndex);
	int		GetStepIndex(int nCycleNumber, int nStepNumber);

	bool	LoadScheduleXml(LPCTSTR lpszFilePath);

	CCycle*	GetAtCycle(int nCycleNumber);

	CStep*	GetAtStep(int nCycleNumber, int nStepNumber);
	CStep*	GetAtStep(int nStepIndex);

	int getScheduleStepCountXml(LPCTSTR lpszFilePath, int Cycle);

	std::vector<SScheduleGrade*>&	GetScheduleGrade() { return m_vecGrade; }
	std::vector<SCurrentMap*>&		GetCurrentMap() { return m_vecCurrent; }
	std::vector<SScheduleSafety*>&	GetScheduleSafety() { return m_vecSafety; }

	CList< CCycle*, CCycle* >&	GetCycleList();
	CList< CStep*, CStep* >&	GetStepList();
	CList< CStep*, CStep* >&	GetStepView() { return m_listStepView; }

	stSVDF* GetSVDF() { return m_stSVDF; }
	CirculaQuque m_queueSvdfCircula;
	CirculaQuque m_queueCircula;
private:
	void	SetGradeSetting(LPCTSTR lpszString);
	void	SetTempSetting(LPCTSTR lpszString);
	void	SetSchedulesafety(LPCTSTR lpszString);	

	SScheduleSafety*	GetScheduleSafetyFromName(CString strName);
private:
	enum XmlSchdule
	{
		XmlScheduleOption = 0
	};

	enum XmlCycle
	{
		XmlCycleNumber = 0,
		XmlCycleComplete,
		XmlCycleSafety,
		XmlCycleLogPeriod,
		XmlCycleBmsSafety,
		XmlCycleLoopCount,
		XmlCycleGotoCycle,
		XmlCycleGotoLoopCount,
		XmlCycleGotoNextCycle,
		XmlCycleEndTime,
	};

	enum XmlStep
	{
		XmlStepNumber = 0,
		XmlStepType,
		XmlStepMode,
		XmlModeType,
		XmlStepChargeVoltage,
		XmlStepDischargeVoltage,
		XmlStepChargeAmpere,
		XmlStepDisChargeAmpere,
		XmlStepWatt,
		XmlStepComplete,
		XmlStepSafety,
		XmlStepLogPeriod,
		XmlStepPatternPath,
		XmlStepSyncType,
		XmlStepChamber,
		XmlStepRatio,
		XmlStepRatioSOH,
		XmlStepBmsEnd,
		XmlStepBmsTx,
		XmlStepPowerSupplyVoltage,
		XmlStepChiller,
		XmlStepPatternCap,
		XmlStepDAQ,
		XmlDCIR,
	};

	CList< CCycle*, CCycle* >	m_listCycle;
	CList< CStep*, CStep* >		m_listStep;	
	CList< CStep*, CStep* >		m_listStepView;	// 화면 디스플레이 용

	std::vector<SScheduleGrade*>	m_vecGrade;
	std::vector<SCurrentMap*>		m_vecCurrent;
	std::vector<SScheduleSafety*>	m_vecSafety;
public:
	int		m_nStepIndex;
	int		m_nLastIndex;
private:
	void	RemoveAll();
	void	RemoveScheduleGrade();
	void	RemoveCurrentMap();
	void	RemoveScheduleSafety();

	bool	LoadGradeXml(TiXmlElement* pXmlCycleGrade);
	bool	LoadTempXml(TiXmlElement* pXmlCycleGrade);
	bool	LoadCycleXml( TiXmlNode* pXmlCycle );
	bool	LoadScheduleSafetyXml(TiXmlNode* pXmlSchedulesafety);
	bool	LoadStepXml( TiXmlNode* pXmlStep, CCycle* pCycle );
	bool	SortStep();
	bool	SortCycle();
	bool	IsScheduleValidation(SScheduleValidate& kParser);
	bool	CheckScheduleSafetyCondition(SScheduleSafetyCheck& kSafetyCheck);

	void	ParseSVDF(CString strData);

	stSVDF*  m_stSVDF;
};