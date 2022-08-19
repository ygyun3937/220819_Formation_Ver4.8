#include "stdafx.h"
#include "CCalibratorChannelData.h"

CCalibratorChannelData::CCalibratorChannelData()
{
	Initialize();
}

CCalibratorChannelData::~CCalibratorChannelData()
{
	Dispose();
}

// -----------------------------------------------------------------------------

bool CCalibratorChannelData::Initialize()
{
	InitializeVariables();

	return true;
}

bool CCalibratorChannelData::InitializeVariables()
{
	m_fCurrentVoltageValue = 0.0f;
	m_fCurrentCurrentValue = 0.0f;

	return true;
}

bool CCalibratorChannelData::Dispose()
{
	return true;
}

// -----------------------------------------------------------------------------

float CCalibratorChannelData::GetVoltageArrayAverage(int nDataStartIndex, int nDataEndIndex)
{
	float sumValue = 0.0f;
	int dataCount = 0;

	int temp = m_sVoltageValueArray.GetCount();

	for (int dataIndex = 0; dataIndex < m_sVoltageValueArray.GetCount(); dataIndex++)
	{
		if (nDataStartIndex >= nDataStartIndex && dataIndex <= nDataEndIndex)
		{
			float tempValue = atof(m_sVoltageValueArray.GetAt(dataIndex));
			sumValue += tempValue;
			dataCount++;
		}
	}

	float returnValue = sumValue / dataCount;

	return returnValue;
}

float CCalibratorChannelData::GetCurrentArrayAverage(int nDataStartIndex, int nDataEndIndex)
{
	float sumValue = 0.0f;
	int dataCount = 0;

	for (int dataIndex = 0; dataIndex < m_sCurrentValueArray.GetCount(); dataIndex++)
	{
		if (nDataStartIndex >= nDataStartIndex && dataIndex <= nDataEndIndex)
		{
			float tempValue = atof(m_sCurrentValueArray.GetAt(dataIndex));

			// m_fDmmAmpereAvg += fabs(m_fDmmAmpere);
			sumValue += fabs(tempValue);
			dataCount++;
		}
	}

	float returnValue = sumValue / dataCount;

	return returnValue;
}
