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

// 1. DAQ csv ������ Load �Ѵ�.
// 2. DAQ csv Name �� �ش� �ϴ� Balance ������ Load �Ѵ�.

class CDAQ
{
public:
	CDAQ();
	virtual ~CDAQ();

	// ����� ���� DAQ Editor ���� �޾ƿ��� �����..
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

	// ����� ���� Balance ���� ���Ǵ� �Լ���
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

	void	Balance2Array(CStringArray& arr);	// Balance Setting ��
	void	DAQ2Array(CStringArray& arr, BOOL bUseBalance);		// DAQ Display ��
	void	DAQ2Array(__out CStringArray& arr, /*BOOL bUseBalance,*/ BOOL bUseSchedule, CStringArray& arrRef);

	//Daq Befor Data
	void	SetDaqCheckVoltage(float fVolt);
	float	GetDaqCheckVoltage();

	void	SetDeltaTime(int nTime = -1);
	int		GetDeltaTime();


	int		m_nErrCount;

	int     m_nPreTempCount;

private:
	// ����� ���� DAQ Editor ���� �޾ƿ��� �����..
	int		m_nIndex;	// File �� ���� ����
	int		m_nChannelNumber;	// Mapping �Ǿ� �ִ� ä�� ����(Ch1~Ch16)
	int		m_nGroupIndex;
	int		m_nUniqueCode;		// �з� ��
	int		m_nSensorType;		// DAQ SENSOR TYPE
	int		m_nSensorIndex;		// DAQ SENSOR INDEX

	float	m_fSafetyConditionMin;
	float	m_fSafetyConditionMax;

	float	m_fFinishConditionMin;
	float	m_fFinishConditionMax;

	BOOL	m_bUse;	// ��� ����

	CString	m_strDAQName;

	// ����� ���� Balance ���� ���Ǵ� ������

	int		m_nBalanceType;
	int		m_nBalanceResistance;

	float	m_fBalanceVoltage;
	float	m_fStartCondition;	// mV ���� ������ > ����
	float	m_fEndCondition;	// mV ���� ������ < ����

	BOOL	m_bUseBalance;

	BOOL	m_bStartBalance;

	// DAQ ��� Data
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

// 1. DAQ csv ������ Load �Ѵ�.
// 2. DAQ csv Name �� �ش� �ϴ� Balance ������ Load �Ѵ�.

class CDAQ
{
public:
	CDAQ();
	virtual ~CDAQ();

	// ����� ���� DAQ Editor ���� �޾ƿ��� �����..
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

	// ����� ���� Balance ���� ���Ǵ� �Լ���
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

	void	Balance2Array(CStringArray& arr);	// Balance Setting ��
	void	DAQ2Array(CStringArray& arr, BOOL bUseBalance);		// DAQ Display ��

private:
	// ����� ���� DAQ Editor ���� �޾ƿ��� �����..
	int		m_nIndex;	// File �� ���� ����
	int		m_nChannelNumber;	// Mapping �Ǿ� �ִ� ä�� ����(Ch1~Ch16)
	int		m_nGroupIndex;
	int		m_nUniqueCode;		// �з� ��

	float	m_fSafetyConditionMin;
	float	m_fSafetyConditionMax;

	float	m_fFinishConditionMin;
	float	m_fFinishConditionMax;

	BOOL	m_bUse;	// ��� ����

	// ����� ���� Balance ���� ���Ǵ� ������

	int		m_nBalanceType;
	int		m_nBalanceResistance;

	float	m_fBalanceVoltage;
	float	m_fStartCondition;	// mV ���� ������ > ����
	float	m_fEndCondition;	// mV ���� ������ < ����

	BOOL	m_bUseBalance;

	BOOL	m_bStartBalance;

	// DAQ ��� Data
	float	m_fRealVoltage;
	float	m_fRealTemperature;
};
*/