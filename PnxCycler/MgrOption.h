#pragma once

enum
{
	CanDelayOptionPause = 0,
	CanDelayOptionComplete
};

enum
{
	VoltageExpressionUnitV = 0,
	VoltageExpressionUnitmV
};

enum
{
	AmpereExpressionUnitA = 0,
	AmpereExpressionUnitmA
};

class CMgrOption : public CSingleton< CMgrOption >
{
public:
	CMgrOption();
	virtual ~CMgrOption();

	int		GetCanDelayTime();
	void	SetCanDelayTime(int nValue);

	int		GetCanDelayOption();
	void	SetCanDelayOption(int nValue);

	int		GetVoltageExpUnit();
	void	SetVoltageExpUnit(int nValue);

	int		GetAmpereExpUnit();
	void	SetAmpereExpUnit(int nValue);

	int		GetWattExpUnit();
	void	SetWattExpUnit(int nValue);

	BOOL	GetAutoCal();
	void	SetAutoCal(int nValue);

	void	SaveFile();
	void	LoadFile();
private:
	int		m_nCanDelayTime;	// CAN 통신 지연 시간
	int		m_nCanDelayOption;	// 통신 지연 후 처리할 Event

	int		m_nVoltageExpUnit;	// Voltage Display 단위
	int		m_nAmpereExpUnit;	// Ampere Display 단위
	int		m_nWattExpUnit;	// Watt Display 단위

	BOOL	m_bUseAutoCal;
};

