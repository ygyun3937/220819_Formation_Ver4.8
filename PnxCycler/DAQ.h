#pragma once

enum BalanceType
{
	BalanceTypeVoltage = 0
};

enum BalanceResistance
{
	BalanceResistanceOFF,
	BalanceResistance20,
	BalanceResistance40,
	BalanceResistance60
};

// 1. DAQ csv 파일을 Load 한다.
// 2. DAQ csv Name 에 해당 하는 Balance 파일을 Load 한다.

class CDAQ
{
public:
	CDAQ();
	virtual ~CDAQ();

	// 여기는 실제 DAQ Editor 에서 받아오는 내용들..
	int		GetDaqIndex();
	void	SetDaqIndex(CString str);

	int		GetChannelNumber();
	void	SetChannelNumber(CString str);

	int		GetSensorType();
	void	SetSensorType(CString str);

	int		GetSensorIndex();
	void	SetSensorIndex(CString str);

	int		GetGroupIndex();
	void	SetGroupIndex(CString str);

	float	GetSafetyConditionMin();
	void	SetSafetyConditionMin(CString str);

	float	GetSafetyConditionMax();
	void	SetSafetyConditionMax(CString str);

	float	GetFinishConditionMin();
	void	SetFinishConditionMin(CString str);

	float	GetFinishConditionMax();
	void	SetFinishConditionMax(CString str);

	int		GetUniqueCode();
	void	SetUniqueCode(CString str);

	BOOL	GetDaqUse();
	void	SetDaqUse(CString str);

	CString	GetDAQName();
	void	SetDAQName(CString str);

	// 여기는 실제 Balance 에서 사용되는 함수들
	int		GetBalanceType();
	void	SetBalanceType(CString str);

	int		GetBalanceResistance();
	void	SetBalanceResistance(CString str);
	void	SetBalanceResistance(int nValue);

	float	GetBalanceVoltage();
	void	SetBalanceVoltage(CString str);

	float	GetRealVoltage();
	void	SetRealVoltage(float fData);

	float GetAccumulateVoltage();
	void SetAccumulateVoltage(float fData);
	

	float	GetRealTemperature();
	void	SetRealTemperature(float fData);

	float	GetPreRealTemperature();
	void	SetPreRealTemperature(float fData);

	
	float	GetStartCondition();
	void	SetStartCondition(CString str);

	float	GetEndCondition();
	void	SetEndCondition(CString str);

	BOOL	GetUseBalance();
	void	SetUseBalance(CString str);
	void	SetUseBalance(BOOL bUse);

	BOOL	GetStartBalance();
	void	SetStartBalance(BOOL bStart);

	void	CopyData(CDAQ* pSrc, BOOL bBalance = FALSE);

	CString	GetBalanceVoltageStr();
	CString	GetStartConditionStr();
	CString	GetEndConditionStr();
	CString	GetRealVoltageStr();
	CString	GetRealTemperatureStr();

	void	Balance2Array(CStringArray& arr);	// Balance Setting 용
	void	DAQ2Array(CStringArray& arr, BOOL bUseBalance);		// DAQ Display 용
	void	DAQ2Array(__out CStringArray& arr, /*BOOL bUseBalance,*/ BOOL bUseSchedule, CStringArray& arrRef);

	//Daq Befor Data
	void	SetDaqCheckVoltage(float fVolt);
	float	GetDaqCheckVoltage();

	void	SetDeltaTime(int nTime = -1);
	int		GetDeltaTime();


	int		m_nErrCount;

	int     m_nPreTempCount;

private:
	// 여기는 실제 DAQ Editor 에서 받아오는 내용들..
	int		m_nIndex;	// File 내 적힌 순번
	int		m_nChannelNumber;	// Mapping 되어 있는 채널 정보(Ch1~Ch16)
	int		m_nGroupIndex;
	int		m_nUniqueCode;		// 분류 값
	int		m_nSensorType;		// DAQ SENSOR TYPE
	int		m_nSensorIndex;		// DAQ SENSOR INDEX

	float	m_fSafetyConditionMin;
	float	m_fSafetyConditionMax;

	float	m_fFinishConditionMin;
	float	m_fFinishConditionMax;

	BOOL	m_bUse;	// 사용 여부

	CString	m_strDAQName;

	// 여기는 실제 Balance 에서 사용되는 변수들

	int		m_nBalanceType;
	int		m_nBalanceResistance;

	float	m_fBalanceVoltage;
	float	m_fStartCondition;	// mV 단위 실제값 > 설정
	float	m_fEndCondition;	// mV 단위 실제값 < 설정

	BOOL	m_bUseBalance;

	BOOL	m_bStartBalance;

	// DAQ 통신 Data
	float	m_fRealVoltage;
	float	m_fRealTemperature;
	float   m_fPreRealTemperature;

	float	m_fAccumulateVoltage;

	//DAQ befor Data
	float	m_fCheckVoltage;
	int		m_nDeltaTime;


};


struct TTempTable
{
	float	fFahrenheit;
	float	fCelsius;
	float	fThermistor;
	TTempTable()
	{
		fFahrenheit = 0;
		fCelsius = 0;
		fThermistor = 0;
	}
};
/*
#pragma once

enum BalanceType
{
	BalanceTypeVoltage = 0
};

enum BalanceResistance
{
	BalanceResistance20 = 0,
	BalanceResistance40,
	BalanceResistance60
};

// 1. DAQ csv 파일을 Load 한다.
// 2. DAQ csv Name 에 해당 하는 Balance 파일을 Load 한다.

class CDAQ
{
public:
	CDAQ();
	virtual ~CDAQ();

	// 여기는 실제 DAQ Editor 에서 받아오는 내용들..
	int		GetDaqIndex();
	void	SetDaqIndex(CString str);

	int		GetChannelNumber();
	void	SetChannelNumber(CString str);

	int		GetGroupIndex();
	void	SetGroupIndex(CString str);

	float	GetSafetyConditionMin();
	void	SetSafetyConditionMin(CString str);

	float	GetSafetyConditionMax();
	void	SetSafetyConditionMax(CString str);

	float	GetFinishConditionMin();
	void	SetFinishConditionMin(CString str);

	float	GetFinishConditionMax();
	void	SetFinishConditionMax(CString str);

	int		GetUniqueCode();
	void	SetUniqueCode(CString str);

	BOOL	GetDaqUse();
	void	SetDaqUse(CString str);

	// 여기는 실제 Balance 에서 사용되는 함수들
	int		GetBalanceType();
	void	SetBalanceType(CString str);

	int		GetBalanceResistance();
	void	SetBalanceResistance(CString str);
	void	SetBalanceResistance(int nValue);

	float	GetBalanceVoltage();
	void	SetBalanceVoltage(CString str);

	float	GetRealVoltage();
	void	SetRealVoltage(float fData);

	float	GetRealTemperature();
	void	SetRealTemperature(float fData);

	float	GetStartCondition();
	void	SetStartCondition(CString str);

	float	GetEndCondition();
	void	SetEndCondition(CString str);

	BOOL	GetUseBalance();
	void	SetUseBalance(CString str);
	void	SetUseBalance(BOOL bUse);

	BOOL	GetStartBalance();
	void	SetStartBalance(BOOL bStart);

	void	CopyData(CDAQ* pSrc);

	CString	GetBalanceVoltageStr();
	CString	GetStartConditionStr();
	CString	GetEndConditionStr();
	CString	GetRealVoltageStr();
	CString	GetRealTemperatureStr();

	void	Balance2Array(CStringArray& arr);	// Balance Setting 용
	void	DAQ2Array(CStringArray& arr, BOOL bUseBalance);		// DAQ Display 용

private:
	// 여기는 실제 DAQ Editor 에서 받아오는 내용들..
	int		m_nIndex;	// File 내 적힌 순번
	int		m_nChannelNumber;	// Mapping 되어 있는 채널 정보(Ch1~Ch16)
	int		m_nGroupIndex;
	int		m_nUniqueCode;		// 분류 값

	float	m_fSafetyConditionMin;
	float	m_fSafetyConditionMax;

	float	m_fFinishConditionMin;
	float	m_fFinishConditionMax;

	BOOL	m_bUse;	// 사용 여부

	// 여기는 실제 Balance 에서 사용되는 변수들

	int		m_nBalanceType;
	int		m_nBalanceResistance;

	float	m_fBalanceVoltage;
	float	m_fStartCondition;	// mV 단위 실제값 > 설정
	float	m_fEndCondition;	// mV 단위 실제값 < 설정

	BOOL	m_bUseBalance;

	BOOL	m_bStartBalance;

	// DAQ 통신 Data
	float	m_fRealVoltage;
	float	m_fRealTemperature;
};
*/
