#pragma once

#include "DAQList.h"
#include "SerialCom.h"
#include <map>

//enum DaqReadType
//{
//	READ_TYPE_VOLT = 0,
//	READ_TYPE_TEMPERATURE,
//	READ_TYPE_THERMISTOR
//};

class CMgrDAQ : public CSingleton< CMgrDAQ >
{
public:
	CMgrDAQ();
	virtual ~CMgrDAQ();

	void	LoadDAQCell();
	void	LoadDAQ(CString lpszFilePath);
	void	SaveBalance();
	void	ClearDAQ();

	int		GetSize();

	CString	GetFileName();

	float	GetDAQVoltageMax(int nChannelNumber);
	float	GetDAQTemperatureMax(int nChannelNumber);
	float	GetDAQTemperature(int nChannelNumber);
	float	GetDAQTemperature_Convert(float fReg);
	CString	GetDAQTemperaturestr(int nChannelNumber);
	CString		GetLogHeaderDAQ(int nChannelNumber);

	CDAQList*	GetDAQMap(int nUniqueCode = 0);

	CDAQ*	GetDAQ(int nDAQIndex);

	CList< CDAQ*, CDAQ* >&	GetDAQList();
	CList< TTempTable*, TTempTable* >&	GetTempTable() { return m_listTempTable;  }
	bool IsWriteDAQ(){ return m_bWriteDAQ; }
private:
	// Key<Group>
	std::map< int, CDAQList* > m_mapDAQ;
	CList< CDAQ*, CDAQ* > m_listDAQ;	// 순차적 Data
	CList< TTempTable*, TTempTable* > m_listTempTable;	// 순차적 Data
	CString	m_strFileName;

	CWinThread*	m_pThread;
	CRITICAL_SECTION	m_cs;
	bool			m_bWriteDAQ;
	CString		m_strDAQWritePath;

private:
	void	AddDAQ(LPCTSTR lpszFullString);
	void	AddBalance(LPCTSTR lpszFullString);

	void	LoadBalance(CString strPath);
	void	AddTempTable(LPCTSTR lpszFullString);
	void	RemoveAll();
};

/*

#pragma once

#include "Singleton.h"
#include "DAQList.h"

#include <map>

class CMgrDAQ : public CSingleton< CMgrDAQ >
{
public:
	CMgrDAQ();
	virtual ~CMgrDAQ();

	void	LoadDAQCell();
	void	LoadDAQ(CString lpszFilePath);
	void	SaveBalance();

	int		GetSize();

	CString	GetFileName();

	float	GetDAQVoltageMax(int nChannelNumber);
	float	GetDAQTemperatureMax(int nChannelNumber);
	
	CDAQList*	GetDAQMap(int nUniqueCode = 0);

	CDAQ*	GetDAQ(int nDAQIndex);

	CList< CDAQ*, CDAQ* >&	GetDAQList();
private:
	// Key<Group>
	std::map< int, CDAQList* > m_mapDAQ;

	CList< CDAQ*, CDAQ* > m_listDAQ;	// 순차적 Data

	CString	m_strFileName;
private:
	void	AddDAQ(LPCTSTR lpszFullString);
	void	AddBalance(LPCTSTR lpszFullString);

	void	LoadBalance(CString strPath);

	void	RemoveAll();
};

*/