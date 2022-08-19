#pragma once

#include <map>

#include "CCommPid.h"

class CMgrPid : public CSingleton<CMgrPid>
{
public:
	CMgrPid();
	virtual ~CMgrPid();

private:
	void Initialize();
	void InitializeVariables();
	void InitializeDevices();

	void Dispose();

public:
	void AddQueue(int nPrimaryKey, CString strTime, CString strData, int nSize);

	CCommPid* GetDeviceMapAt(int nPrimaryKey);
	CCommPid* GetDevice(int nSourceChannlNo);

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

private:
	CList< CCommPid*, CCommPid* > m_listCom;
	std::map< int, CCommPid* > m_mapCom;
};

