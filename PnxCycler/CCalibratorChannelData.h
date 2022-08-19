#pragma once

class CCalibratorChannelData
{
public:
	CCalibratorChannelData();
	virtual ~CCalibratorChannelData();

public:
	void	SetChannelIndex(int value) { m_nChannelIndex = value; }
	int		GetChannelIndex() { return m_nChannelIndex; }

	void	SetChannelId(CString value) { m_strChannelId = value; }
	CString	GetChannelId() { return m_strChannelId; }

	void	SetCurrentVoltageValue(float value) { m_fCurrentVoltageValue = value; }
	float	GetCurrentVoltageValue() { return m_fCurrentVoltageValue; }

	void	SetCurrentCurrentValue(float value) { m_fCurrentCurrentValue = value; }
	float	GetCurrentCurrentValue() { return m_fCurrentCurrentValue; }

	CStringArray* GetVoltageValueArray() { return &m_sVoltageValueArray; }
	CStringArray* GetCurrentValueArray() { return &m_sCurrentValueArray; }

	// ----------------------------------------------------------------------------

	float GetVoltageArrayAverage(int nDataStartIndex, int nDataEndIndex);
	float GetCurrentArrayAverage(int nDataStartIndex, int nDataEndIndex);

private:
	bool Initialize();
	bool InitializeVariables();
	bool Dispose();

private:
	int		m_nChannelIndex;
	CString m_strChannelId;

	float	m_fCurrentVoltageValue;
	float	m_fCurrentCurrentValue;

	CStringArray m_sVoltageValueArray;
	CStringArray m_sCurrentValueArray;
};

