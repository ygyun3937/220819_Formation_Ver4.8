#include "stdafx.h"
#include "Daq.h"
#include "MgrDAQ.h"


CDAQ::CDAQ()
{
	m_nIndex = 0;
	m_nChannelNumber = -1;
	m_nSensorType = 0;
	m_nSensorIndex = 0;
	m_nGroupIndex = 0;

	m_fSafetyConditionMin = 0.000;
	m_fSafetyConditionMax = 0.000;
	m_fFinishConditionMin = 0.000;
	m_fFinishConditionMax = 0.000;
	m_nUniqueCode = 0;
	m_bUse = FALSE;

	m_nBalanceType = 0;
	m_nBalanceResistance = -1;
	m_fBalanceVoltage = 0.000;
	m_fStartCondition = 0.000;
	m_fEndCondition = 0.000;
	m_bUseBalance = FALSE;
	m_bStartBalance = FALSE;

	// DAQ 통신 Data
	m_fRealVoltage = 0.000;
	m_fRealTemperature = 0.000;
	m_fPreRealTemperature = 0.000;
	m_fAccumulateVoltage = 0.f;
	m_strDAQName.Empty();

	//Daq Befor Data
	m_fCheckVoltage = 0.000;
	m_nDeltaTime = 0;
	m_nErrCount = 0;
	m_nPreTempCount = 0;
}


CDAQ::~CDAQ()
{
}

int		CDAQ::GetDaqIndex()
{
	return m_nIndex;
}

void	CDAQ::SetDaqIndex(CString str)
{
	m_nIndex = atoi(str) - 1;
}

int		CDAQ::GetChannelNumber()
{
	return m_nChannelNumber;
}

void	CDAQ::SetChannelNumber(CString str)
{
	CString strChannelNumber = str;

	if (strChannelNumber.Find(_T("CH")) < 0)
		return;

	strChannelNumber.Replace(_T("CH"), _T(""));

	m_nChannelNumber = atoi(strChannelNumber);

	// 0Base 로 맞추자
	m_nChannelNumber -= 1;
}

int		CDAQ::GetSensorType()
{
	return m_nSensorType;
}

void	CDAQ::SetSensorType(CString str)
{
	m_nSensorType = atoi(str);
}

int		CDAQ::GetSensorIndex()
{
	return m_nSensorIndex;
}

void	CDAQ::SetSensorIndex(CString str)
{
	m_nSensorIndex = atoi(str);
}

int		CDAQ::GetGroupIndex()
{
	return m_nGroupIndex;
}

void	CDAQ::SetGroupIndex(CString str)
{
	m_nGroupIndex = atoi(str);
}

float	CDAQ::GetSafetyConditionMin()
{
	return m_fSafetyConditionMin;
}

void	CDAQ::SetSafetyConditionMin(CString str)
{
	m_fSafetyConditionMin = (float)atof(str);
}

float	CDAQ::GetSafetyConditionMax()
{
	return m_fSafetyConditionMax;
}

void	CDAQ::SetSafetyConditionMax(CString str)
{
	m_fSafetyConditionMax = (float)atof(str);
}

float	CDAQ::GetFinishConditionMin()
{
	return m_fFinishConditionMin;
}

void	CDAQ::SetFinishConditionMin(CString str)
{
	m_fFinishConditionMin = (float)atof(str);
}

float	CDAQ::GetFinishConditionMax()
{
	return m_fFinishConditionMax;
}

void	CDAQ::SetFinishConditionMax(CString str)
{
	m_fFinishConditionMax = (float)atof(str);
}

int		CDAQ::GetUniqueCode()
{
	return m_nUniqueCode;
}

void	CDAQ::SetUniqueCode(CString str)
{
	m_nUniqueCode = atoi(str);
}

BOOL	CDAQ::GetDaqUse()
{
	return m_bUse;
}

void	CDAQ::SetDaqUse(CString str)
{
	if (str.CompareNoCase(_T("1")) == 0)
		m_bUse = TRUE;
	else
		m_bUse = FALSE;
}

CString	CDAQ::GetDAQName()
{
	return m_strDAQName;
}

void	CDAQ::SetDAQName(CString str)
{
	m_strDAQName = str;
}

int		CDAQ::GetBalanceType()
{
	return m_nBalanceType;
}

void	CDAQ::SetBalanceType(CString str)
{
	m_nBalanceType = atoi(str);
}

int		CDAQ::GetBalanceResistance()
{
	return m_nBalanceResistance;
}

void	CDAQ::SetBalanceResistance(CString str)
{
	m_nBalanceResistance = atoi(str);
}

void	CDAQ::SetBalanceResistance(int nValue)
{
	m_nBalanceResistance = nValue;
}

float	CDAQ::GetBalanceVoltage()
{
	return m_fBalanceVoltage;
}

void	CDAQ::SetBalanceVoltage(CString str)
{
	m_fBalanceVoltage = (float)atof(str);
}

float	CDAQ::GetRealVoltage()
{
	return m_fRealVoltage;
}

void	CDAQ::SetRealVoltage(float fData)
{
	m_fRealVoltage = fData;
}

float CDAQ::GetAccumulateVoltage()
{
	return m_fAccumulateVoltage;
}

void CDAQ::SetAccumulateVoltage(float fData)
{
	m_fAccumulateVoltage = fData;
}

float	CDAQ::GetPreRealTemperature()
{
	return m_fPreRealTemperature;
}

float	CDAQ::GetRealTemperature()
{
	return m_fRealTemperature;
}

void	CDAQ::SetRealTemperature(float fData)
{
	m_fRealTemperature = fData;
}

void	CDAQ::SetPreRealTemperature(float fData)
{
	m_fPreRealTemperature = fData;
}

float	CDAQ::GetStartCondition()
{
	return m_fStartCondition;
}

void	CDAQ::SetStartCondition(CString str)
{
	m_fStartCondition = (float)atof(str);
}

float	CDAQ::GetEndCondition()
{
	return m_fEndCondition;
}

void	CDAQ::SetEndCondition(CString str)
{
	m_fEndCondition = (float)atof(str);
}

BOOL	CDAQ::GetUseBalance()
{
	return m_bUseBalance;
}

void	CDAQ::SetUseBalance(CString str)
{
	if (str.CompareNoCase(_T("1")) == 0)
		m_bUseBalance = TRUE;
	else
		m_bUseBalance = FALSE;
}

void	CDAQ::SetUseBalance(BOOL bUse)
{
	m_bUseBalance = bUse;
}

BOOL	CDAQ::GetStartBalance()
{
	return m_bStartBalance;
}

void	CDAQ::SetStartBalance(BOOL bStart)
{
	m_bStartBalance = bStart;
}

void	CDAQ::CopyData(CDAQ* pSrc, BOOL bBalance/* = FALSE*/)
{
	if (bBalance)
	{
		this->m_nIndex = pSrc->m_nIndex;
		this->m_nBalanceType = pSrc->m_nBalanceType;
		this->m_nBalanceResistance = pSrc->m_nBalanceResistance;
		this->m_fBalanceVoltage = pSrc->m_fBalanceVoltage;
		this->m_bUseBalance = pSrc->m_bUseBalance;
		this->m_fStartCondition = pSrc->m_fStartCondition;
		this->m_fEndCondition = pSrc->m_fEndCondition;
	}
	else
	{
		this->m_nIndex = pSrc->m_nIndex;
		this->m_bUse = pSrc->m_bUse;
		this->m_nChannelNumber = pSrc->m_nChannelNumber;
		this->m_nSensorType = pSrc->m_nSensorType;
		this->m_nSensorIndex = pSrc->m_nSensorIndex;
		this->m_strDAQName = pSrc->m_strDAQName;
		this->m_fSafetyConditionMin = pSrc->m_fSafetyConditionMin;
		this->m_fSafetyConditionMax = pSrc->m_fSafetyConditionMax;
		this->m_fFinishConditionMin = pSrc->m_fFinishConditionMin;
		this->m_fFinishConditionMax = pSrc->m_fFinishConditionMax;
		this->m_nUniqueCode = pSrc->m_nUniqueCode;
		this->m_nGroupIndex = pSrc->m_nGroupIndex;

		this->m_nBalanceType = pSrc->m_nBalanceType;
		this->m_nBalanceResistance = pSrc->m_nBalanceResistance;
		this->m_fBalanceVoltage = pSrc->m_fBalanceVoltage;
		this->m_fStartCondition = pSrc->m_fStartCondition;
		this->m_fEndCondition = pSrc->m_fEndCondition;
		this->m_bUseBalance = pSrc->m_bUseBalance;
		this->m_fRealVoltage = pSrc->m_fRealVoltage;
		this->m_fRealTemperature = pSrc->m_fRealTemperature;
		this->m_fCheckVoltage = pSrc->m_fCheckVoltage;
		this->m_nDeltaTime = pSrc->m_nDeltaTime;
	}
}

void	CDAQ::Balance2Array(CStringArray& arr)
{
	CString str;

	str.Format(_T("DAQ %d"), m_nIndex + 1);
	arr.Add(str);

	str.Format(_T("CH %d"), m_nChannelNumber + 1);
	arr.Add(str);

	if (TRUE == m_bUseBalance)
		str = _T("Use");
	else
		str = _T("Not Use");

	arr.Add(str);

	str.Format(_T("%s"), _T("Voltage"));
	arr.Add(str);

	str = _T("");

	switch (m_nBalanceResistance)
	{
	case BalanceResistance20: str = _T("20Ω"); break;
	case BalanceResistance40: str = _T("40Ω"); break;
	case BalanceResistance60: str = _T("60Ω"); break;
	}

	arr.Add(str);

	str.Format(_T("%.3f"), m_fBalanceVoltage);
	arr.Add(str);

	str.Format(_T("%.3f"), m_fStartCondition);
	arr.Add(str);

	str.Format(_T("%.3f"), m_fEndCondition);
	arr.Add(str);
}

void	CDAQ::DAQ2Array(CStringArray& arr, BOOL bUseBalance)
{
	CString str;

	// 1. Group
	str.Format(_T("%d"), m_nUniqueCode);
	arr.Add(str);

	// 2. DAQ Index

	//str.Format(_T("DAQ %d"), m_nIndex + 1);
	//arr.Add(str);

	arr.Add(m_strDAQName);

	// 3. DAQ Voltage
	str.Format(_T("%f"), m_fRealVoltage);
	arr.Add(str);

	// 4. DAQ Temperature
 	str.Format(_T("%.3f"), m_fRealTemperature);
 	arr.Add(str);


	// 6. DAQ Safety Min
	str.Format(_T("%.3f"), m_fSafetyConditionMin);
	arr.Add(str);

	// 5. DAQ Safety Max
	str.Format(_T("%.3f"), m_fSafetyConditionMax);
	arr.Add(str);


//
//	// 7. DAQ Channel
//	str.Format(_T("CH %d"), m_nChannelNumber + 1);
//	arr.Add(str);
//
//	// 6. DAQ Use
//
//	if (TRUE == m_bUse)
//		str = _T("Use");
//	else
//		str = _T("Not Use");
//
//	arr.Add(str);
//
//	
//
//// 	// 8. DAQ Safety Max
//// 	str.Format(_T("%.3f"), m_fSafetyConditionMax);
//// 	arr.Add(str);
//
//	// 9. DAQ Complete Min
//	str.Format(_T("%.3f"), m_fFinishConditionMin);
//	arr.Add(str);
//
//	// 10. DAQ Complete Max
//	str.Format(_T("%.3f"), m_fFinishConditionMax);
//	arr.Add(str);
//
//	if (FALSE == bUseBalance)
//		return;
//
//	// 11. Balance Use
//	if (TRUE == m_bUseBalance)
//		str = _T("Use");
//	else
//		str = _T("Not Use");
//
//	arr.Add(str);
//
//	// 12. Balance Type
//	str.Format(_T("%s"), _T("Voltage"));
//	arr.Add(str);
//
//	// 13. Balance Resistance
//	str = _T("");
//
//	switch (m_nBalanceResistance)
//	{
//	case BalanceResistance20: str = _T("20Ω"); break;
//	case BalanceResistance40: str = _T("40Ω"); break;
//	case BalanceResistance60: str = _T("60Ω"); break;
//	}
//
//	arr.Add(str);
//
//	// 14. Balance Value
//	str.Format(_T("%.3f"), m_fBalanceVoltage);
//	arr.Add(str);
//
//	// 15. Balance Start Condition
//	str.Format(_T("%.3f"), m_fStartCondition);
//	arr.Add(str);
//
//	// 16. Valance End Condition
//	str.Format(_T("%.3f"), m_fEndCondition);
//	arr.Add(str);
//
//	// 17. Balance Started
//	if (TRUE == m_bStartBalance)
//		str = _T("Start");
//	else
//		str = _T("Stop");
//
//	arr.Add(str);
}

void	CDAQ::DAQ2Array(__out CStringArray& arr,/* BOOL bUseBalance,*/ BOOL bUseSchedule, CStringArray& arrRef)
{


	CString str;
	int i = 0;
	for (int i = 0; i < arrRef.GetSize(); ++i)
	{
		// 1. Group
		if (arrRef.GetAt(i).CompareNoCase(_T("Unique Code")) == 0)
		{
			str.Empty();
			str.Format(_T("%d"), m_nUniqueCode);
			arr.Add(str);
		}
		// 2. DAQ Channel
		else if (arrRef.GetAt(i).CompareNoCase(_T("Ch")) == 0)
		{
			str.Empty();
			str.Format(_T("CH %d"), m_nChannelNumber + 1);
			arr.Add(str);
		}
		// 3. DAQ Index
		else if (arrRef.GetAt(i).CompareNoCase(_T("DAQ")) == 0)
		{
			str.Empty();
			arr.Add(m_strDAQName);
		}
		// 4. DAQ Voltage
		else if (arrRef.GetAt(i).CompareNoCase(_T("Voltage")) == 0)
		{
			str.Empty();
			str.Format(_T("%f"), m_fRealVoltage);
			arr.Add(str);
		}
		// 5. Balance
		else if (arrRef.GetAt(i).CompareNoCase(_T("Balance")) == 0)
		{
			str.Empty();
			if (bUseSchedule) //bUseBalance
			{
				switch (m_nBalanceResistance + 1)
				{
				case BalanceResistanceOFF: str = _T("OFF"); break;
				case BalanceResistance20: str = _T("20Ω"); break;
				case BalanceResistance40: str = _T("40Ω"); break;
				case BalanceResistance60: str = _T("60Ω"); break;
				default: str = _T("OFF"); break;
				}
				arr.Add(str);
			}
			//else if (bUseSchedule)	//pnx_hr 2021.03.10 DAQ BalanceScheduleRes 
			//{
			//	switch (m_nBalanceScheduleRes)
			//	{
			//	case BalanceResistanceOFF: str = _T("OFF"); break;
			//	case BalanceResistance20: str = _T("20Ω"); break;
			//	case BalanceResistance40: str = _T("40Ω"); break;
			//	case BalanceResistance60: str = _T("60Ω"); break;
			//	default: str = _T("OFF"); break;
			//	}
			//	arr.Add(str);
			//}
			else
			{
				str = _T("OFF");
				arr.Add(str);
			}
		}
		// 6. DAQ Temperature
		else if (arrRef.GetAt(i).CompareNoCase(_T("Temperature")) == 0)
		{
			str.Empty();
			str.Format(_T("%.3f"), m_fRealTemperature);
			arr.Add(str);
		}
		// 7. DAQ Safety Max
		else if (arrRef.GetAt(i).CompareNoCase(_T("SafetyCondition Upper Limit")) == 0)
		{
			str.Empty();
			str.Format(_T("%.3f"), m_fSafetyConditionMax);
			arr.Add(str);
		}

		// 8. DAQ Safety Min
		else if (arrRef.GetAt(i).CompareNoCase(_T("SafetyCondition Lower Limit")) == 0)
		{
			str.Empty();
			str.Format(_T("%.3f"), m_fSafetyConditionMin);
			arr.Add(str);
		}
	}
}
CString	CDAQ::GetBalanceVoltageStr()
{
	CString str;

	str.Format(_T("%.3f"), m_fBalanceVoltage);

	return str;
}

CString	CDAQ::GetStartConditionStr()
{
	CString str;

	str.Format(_T("%.3f"), m_fStartCondition);

	return str;
}

CString	CDAQ::GetEndConditionStr()
{
	CString str;

	str.Format(_T("%.3f"), m_fEndCondition);

	return str;
}

CString	CDAQ::GetRealVoltageStr()
{
	CString str;

	str.Format(_T("%.7f"), m_fRealVoltage);

	return str;
}

CString	CDAQ::GetRealTemperatureStr()
{
	CString str;

	str.Format(_T("%.3f"), m_fRealTemperature);

	return str;
}

void	CDAQ::SetDaqCheckVoltage(float fVolt)
{
	m_fCheckVoltage = fVolt;
}

float	CDAQ::GetDaqCheckVoltage()
{
	return m_fCheckVoltage;
}

void	CDAQ::SetDeltaTime(int nTime /*= -1*/)
{
	if (nTime == -1)
		m_nDeltaTime++;
	else if (nTime == 0)
		m_nDeltaTime = 0;
	else if (nTime > 0)
		m_nDeltaTime = nTime;
	/*m_nDeltaTime = nTime;*/
}
int		CDAQ::GetDeltaTime()
{
	return m_nDeltaTime;
}

/*
#include "stdafx.h"
#include "Daq.h"


CDAQ::CDAQ()
{
	m_nIndex				= 0;
	m_nChannelNumber		= 0;
	m_nGroupIndex			= 0;
	m_fSafetyConditionMin	= 0.000;
	m_fSafetyConditionMax	= 0.000;
	m_fFinishConditionMin	= 0.000;
	m_fFinishConditionMax	= 0.000;
	m_nUniqueCode			= 0;
	m_bUse					= FALSE;

	m_nBalanceType			= 0;
	m_nBalanceResistance	= -1;
	m_fBalanceVoltage		= 0.000;
	m_fStartCondition		= 0.000;
	m_fEndCondition			= 0.000;
	m_bUseBalance			= FALSE;
	m_bStartBalance			= FALSE;

// DAQ 통신 Data
	m_fRealVoltage			= 0.000;
	m_fRealTemperature		= 0.000;
}


CDAQ::~CDAQ()
{
}

int		CDAQ::GetDaqIndex()
{
	return m_nIndex;
}

void	CDAQ::SetDaqIndex(CString str)
{
	m_nIndex = atoi(str);

	m_nIndex -= 1;
}

int		CDAQ::GetChannelNumber()
{
	return m_nChannelNumber;
}

void	CDAQ::SetChannelNumber(CString str)
{
	CString strChannelNumber = str;

	if (strChannelNumber.Find(_T("CH")) < 0)
		return;

	strChannelNumber.Replace(_T("CH"), _T(""));

	m_nChannelNumber = atoi(strChannelNumber);

	// 0Base 로 맞추자
	m_nChannelNumber -= 1;
}

int		CDAQ::GetGroupIndex()
{
	return m_nGroupIndex;
}

void	CDAQ::SetGroupIndex(CString str)
{
	m_nGroupIndex = atoi(str);
}

float	CDAQ::GetSafetyConditionMin()
{
	return m_fSafetyConditionMin;
}

void	CDAQ::SetSafetyConditionMin(CString str)
{
	m_fSafetyConditionMin = (float)atof(str);
}

float	CDAQ::GetSafetyConditionMax()
{
	return m_fSafetyConditionMax;
}

void	CDAQ::SetSafetyConditionMax(CString str)
{
	m_fSafetyConditionMax = (float)atof(str);
}

float	CDAQ::GetFinishConditionMin()
{
	return m_fFinishConditionMin;
}

void	CDAQ::SetFinishConditionMin(CString str)
{
	m_fFinishConditionMin = (float)atof(str);
}

float	CDAQ::GetFinishConditionMax()
{
	return m_fFinishConditionMax;
}

void	CDAQ::SetFinishConditionMax(CString str)
{
	m_fFinishConditionMax = (float)atof(str);
}

int		CDAQ::GetUniqueCode()
{
	return m_nUniqueCode;
}

void	CDAQ::SetUniqueCode(CString str)
{
	m_nUniqueCode = atoi(str);
}

BOOL	CDAQ::GetDaqUse()
{
	return m_bUse;
}

void	CDAQ::SetDaqUse(CString str)
{
	if (str.CompareNoCase(_T("1")) == 0)
		m_bUse = TRUE;
	else
		m_bUse = FALSE;
}

int		CDAQ::GetBalanceType()
{
	return m_nBalanceType;
}

void	CDAQ::SetBalanceType(CString str)
{
	m_nBalanceType = atoi(str);
}

int		CDAQ::GetBalanceResistance()
{
	return m_nBalanceResistance;
}

void	CDAQ::SetBalanceResistance(CString str)
{
	m_nBalanceResistance = atoi(str);
}

void	CDAQ::SetBalanceResistance(int nValue)
{
	m_nBalanceResistance = nValue;
}

float	CDAQ::GetBalanceVoltage()
{
	return m_fBalanceVoltage;
}

void	CDAQ::SetBalanceVoltage(CString str)
{
	m_fBalanceVoltage = (float)atof(str);
}

float	CDAQ::GetRealVoltage()
{
	return m_fRealVoltage;
}

void	CDAQ::SetRealVoltage(float fData)
{
	m_fRealVoltage = fData;
}

float	CDAQ::GetRealTemperature()
{
	return m_fRealTemperature;
}

void	CDAQ::SetRealTemperature(float fData)
{
	m_fRealTemperature = fData;
}

float	CDAQ::GetStartCondition()
{
	return m_fStartCondition;
}

void	CDAQ::SetStartCondition(CString str)
{
	m_fStartCondition = (float)atof(str);
}

float	CDAQ::GetEndCondition()
{
	return m_fEndCondition;
}

void	CDAQ::SetEndCondition(CString str)
{
	m_fEndCondition = (float)atof(str);
}

BOOL	CDAQ::GetUseBalance()
{
	return m_bUseBalance;
}

void	CDAQ::SetUseBalance(CString str)
{
	if (str.CompareNoCase(_T("1")) == 0)
		m_bUseBalance = TRUE;
	else
		m_bUseBalance = FALSE;
}

void	CDAQ::SetUseBalance(BOOL bUse)
{
	m_bUseBalance = bUse;
}

BOOL	CDAQ::GetStartBalance()
{
	return m_bStartBalance;
}

void	CDAQ::SetStartBalance(BOOL bStart)
{
	m_bStartBalance = bStart;
}

void	CDAQ::CopyData(CDAQ* pSrc)
{
	this->m_nIndex				= pSrc->m_nIndex;
	this->m_nChannelNumber		= pSrc->m_nChannelNumber;
	this->m_nGroupIndex			= pSrc->m_nGroupIndex;
	this->m_nUniqueCode			= pSrc->m_nUniqueCode;
	this->m_fSafetyConditionMin = pSrc->m_fSafetyConditionMin;
	this->m_fSafetyConditionMax = pSrc->m_fSafetyConditionMax;
	this->m_fFinishConditionMin = pSrc->m_fFinishConditionMin;
	this->m_fFinishConditionMax = pSrc->m_fFinishConditionMax;
	this->m_bUse				= pSrc->m_bUse;

	this->m_nBalanceType		= pSrc->m_nBalanceType;
	this->m_nBalanceResistance	= pSrc->m_nBalanceResistance;
	this->m_fBalanceVoltage		= pSrc->m_fBalanceVoltage;
	this->m_fStartCondition		= pSrc->m_fStartCondition;
	this->m_fEndCondition		= pSrc->m_fEndCondition;
	this->m_bUseBalance			= pSrc->m_bUseBalance;
	this->m_fRealVoltage		= pSrc->m_fRealVoltage;
	this->m_fRealTemperature	= pSrc->m_fRealTemperature;
}

void	CDAQ::Balance2Array(CStringArray& arr)
{
	CString str;

	str.Format(_T("DAQ %d"), m_nIndex + 1);
	arr.Add(str);

	str.Format(_T("CH %d"), m_nChannelNumber + 1);
	arr.Add(str);

	if(TRUE == m_bUseBalance)
		str = _T("Use");
	else
		str = _T("Not Use");

	arr.Add(str);

	str.Format(_T("%s"), _T("Voltage"));
	arr.Add(str);

	str = _T("");

	switch (m_nBalanceResistance)
	{
	case BalanceResistance20: str = _T("20Ω"); break;
	case BalanceResistance40: str = _T("40Ω"); break;
	case BalanceResistance60: str = _T("60Ω"); break;
	}

	arr.Add(str);

	str.Format(_T("%.3f"), m_fBalanceVoltage);
	arr.Add(str);

	str.Format(_T("%.3f"), m_fStartCondition);
	arr.Add(str);

	str.Format(_T("%.3f"), m_fEndCondition);
	arr.Add(str);
}

void	CDAQ::DAQ2Array(CStringArray& arr, BOOL bUseBalance)
{
	CString str;

	// 1. Group
	str.Format(_T("%d"), m_nUniqueCode);
	arr.Add(str);

	// 2. DAQ Index
	str.Format(_T("DAQ %d"), m_nIndex + 1);
	arr.Add(str);

	// 3. DAQ Voltage
	str.Format(_T("%f"), m_fRealVoltage);
	arr.Add(str);

	// 4. DAQ Temperature
	str.Format(_T("%.3f"), m_fRealTemperature);
	arr.Add(str);

	// 5. DAQ Channel
	str.Format(_T("CH %d"), m_nChannelNumber + 1);
	arr.Add(str);

	// 6. DAQ Use

	if (TRUE == m_bUse)
		str = _T("Use");
	else
		str = _T("Not Use");

	arr.Add(str);

	// 7. DAQ Safety Min
	str.Format(_T("%.3f"), m_fSafetyConditionMin);
	arr.Add(str);

	// 8. DAQ Safety Max
	str.Format(_T("%.3f"), m_fSafetyConditionMax);
	arr.Add(str);

	// 9. DAQ Complete Min
	str.Format(_T("%.3f"), m_fFinishConditionMin);
	arr.Add(str);

	// 10. DAQ Complete Max
	str.Format(_T("%.3f"), m_fFinishConditionMax);
	arr.Add(str);

	if (FALSE == bUseBalance)
		return;

	// 11. Balance Use
	if (TRUE == m_bUseBalance)
		str = _T("Use");
	else
		str = _T("Not Use");

	arr.Add(str);

	// 12. Balance Type
	str.Format(_T("%s"), _T("Voltage"));
	arr.Add(str);

	// 13. Balance Resistance
	str = _T("");

	switch (m_nBalanceResistance)
	{
	case BalanceResistance20: str = _T("20Ω"); break;
	case BalanceResistance40: str = _T("40Ω"); break;
	case BalanceResistance60: str = _T("60Ω"); break;
	}

	arr.Add(str);

	// 14. Balance Value
	str.Format(_T("%.3f"), m_fBalanceVoltage);
	arr.Add(str);

	// 15. Balance Start Condition
	str.Format(_T("%.3f"), m_fStartCondition);
	arr.Add(str);

	// 16. Valance End Condition
	str.Format(_T("%.3f"), m_fEndCondition);
	arr.Add(str);

	// 17. Balance Started
	if (TRUE == m_bStartBalance)
		str = _T("Start");
	else
		str = _T("Stop");

	arr.Add(str);
}

CString	CDAQ::GetBalanceVoltageStr()
{
	CString str;

	str.Format(_T("%.3f"), m_fBalanceVoltage);

	return str;
}

CString	CDAQ::GetStartConditionStr()
{
	CString str;

	str.Format(_T("%.3f"), m_fStartCondition);

	return str;
}

CString	CDAQ::GetEndConditionStr()
{
	CString str;

	str.Format(_T("%.3f"), m_fEndCondition);

	return str;
}

CString	CDAQ::GetRealVoltageStr()
{
	CString str;

	str.Format(_T("%.3f"), m_fRealVoltage);

	return str;
}

CString	CDAQ::GetRealTemperatureStr()
{
	CString str;

	str.Format(_T("%.3f"), m_fRealTemperature);

	return str;
}
*/