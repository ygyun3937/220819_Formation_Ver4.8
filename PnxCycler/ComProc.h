#pragma once

#define ASCII_LF					0x0a
#define ASCII_ETX					0x03
#define	PORT_DATA_BIT				8
#define	PORT_STOP_BIT				ONESTOPBIT
#define	PORT_PARITY					NOPARITY
#define	PORT_CLEAR_BUF				TRUE
#define PORT_TERMINATE				ASCII_LF
#define PORT_TERMINATE_CYCLER		ASCII_ETX

#include "Singleton.h"
#include "SerialCom.h"
#include <vector>

class CComProc :public CSingleton< CComProc >
{
public:
	CComProc();
	~CComProc();

	void	StopThread();
	void	AddDaqData(int nPrimaryKey, CString strTime, byte* pData, int nDataLenth);
	void	AddChamberData(int nPrimaryKey, CString strTime, float fData);
	void	AddChamberDataTemi(int nPrimaryKey, CString strTime, float fData, int nType);
	CSerialCom*	GetAtComm(int nPrimaryKey);
private:
	std::vector<CSerialCom*> m_vecDevice;
private:
	void	LoadIni();		//Port정보 얻어서 Open까지
	void	RemoveAll();
};

