#pragma once

#include <map>

#include "PlcInterface.h"

class CMgrPlcInterface : public CSingleton<CMgrPlcInterface>
{
public:
	CMgrPlcInterface();
	virtual ~CMgrPlcInterface();

private:
	void Initialize();
	void InitializeVariables();
	void InitializeDevices();

	void Dispose();

public:
	void AddQueue(int nPrimaryKey, CString strTime, CString strData, int nSize);

	CPlcInterface* GetDeviceMapAt(int nPrimaryKey);
	CPlcInterface* GetDevice(int nSourceChannlNo);


	void Set_VaccumValue(float fValue);
	float Get_VacuumValue() { return m_fVacuumValue; }
	//nUnit : ???? 0 : ?⺻ 3: K 
	CString Get_VaccumValueKPAStr();
	CString Get_VaccumValuePAStr();

	void Set_PumpOpenPercent(float fOpenPercent);
	float Get_PumpOpenPercent() { return m_fPumpOpenPercent; }

	CString Get_PumpOpenPercentStr();

	void Set_AutoMode(bool bAutoMode);
	bool Get_AutoMode() {return m_bAutoMode;}


private:
	void StartThread();
	void StartThreadListType();
	void StartThreatMapType();

	void StopThread();
	void StopThreadListType();
	void StopThreadMapType();

	// -------------------------------------------------------------------------

	void OpenDevices();
	void CloseDevices();

	float	m_fVacuumValue;
	float	m_fPumpOpenPercent;

	bool	m_bAutoMode;

private:
	CList< CPlcInterface*, CPlcInterface* > m_listPlcInterface;
	std::map< int, CPlcInterface* > m_mapPlcInterface;
};

