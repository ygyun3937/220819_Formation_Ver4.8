#include "stdafx.h"
#include "ComProc.h"
#include "MgrSerial.h"
#include "MgrComm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CComProc::CComProc()
{
	LoadIni();
}


CComProc::~CComProc()
{
	RemoveAll();
}

void	CComProc::RemoveAll()
{
	auto iter = m_vecDevice.begin();
	auto iterE = m_vecDevice.end();

	while (iter != iterE)
	{
		CSerialCom* lpDevice = (*iter);

		if (lpDevice)
		{
			lpDevice->Close();

			delete lpDevice;
			lpDevice = nullptr;
		}

		++iter;
	}

	m_vecDevice.clear();
}

void CComProc::LoadIni()
{
	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return;

	int nDaQDeviceType = pMgr->GetCommType(eDeviceTypeDaQ);

	if (eCommTypeRs232 == nDaQDeviceType || eCommTypeRs422 == nDaQDeviceType || eCommTypeRs485 == nDaQDeviceType)
	{
		std::vector<SDevice*> vecValue;
		pMgr->GetDeivceFromType(eDeviceTypeDaQ, vecValue);

		auto iter = vecValue.begin();
		auto iterE = vecValue.end();

		while (iter != iterE)
		{
			SDevice* lpDevice = (*iter);

			if (lpDevice)
			{
				ST_SerialINI SerialParam;

				SerialParam.strName = lpDevice->strName;
				SerialParam.nDaqType = lpDevice->nDaqDataType;

				SerialParam.nPrimaryKey = lpDevice->nPrimaryKey;
				SerialParam.nPortNumber = lpDevice->nCommPort;
				SerialParam.dwBaud		= lpDevice->nBaudrate;
				SerialParam.bDataBit	= PORT_DATA_BIT;
				SerialParam.bStopBit	= PORT_STOP_BIT;
				SerialParam.bParity		= PORT_PARITY;
				SerialParam.nDelimiter	= PORT_TERMINATE_CYCLER;
				SerialParam.bBufClear	= PORT_CLEAR_BUF;

				CSerialCom* lpSerialDevice = new CSerialCom(SerialParam);

				m_vecDevice.push_back(lpSerialDevice);

				if (lpSerialDevice->OpenPort())
				{
					lpSerialDevice->Connect();
					lpSerialDevice->StartThread();
				}
				else
				{
#ifndef _DEBUG
					AfxMessageBox("[SERIAL PORT] DAQ CAN NOT OPEN");
#endif
				}
			}

			++iter;
		}
	}

	int nChamberDeviceType = pMgr->GetCommType(eDeviceTypeChamber);

	if (eCommTypeRs232 == nChamberDeviceType || eCommTypeRs422 == nChamberDeviceType || eCommTypeRs485 == nChamberDeviceType)
	{
		std::vector<SDevice*> vecValue;
		pMgr->GetDeivceFromType(eDeviceTypeChamber, vecValue);

		auto iter = vecValue.begin();
		auto iterE = vecValue.end();

		while (iter != iterE)
		{
			SDevice* lpDevice = (*iter);

			if (lpDevice)
			{
				ST_SerialINI SerialParam;
				
				SerialParam.nPrimaryKey			= lpDevice->nPrimaryKey;
				SerialParam.nChamberChannel		= lpDevice->nChamberChannel;
				SerialParam.nChamberControlType = lpDevice->nChamberControlType;
				SerialParam.nPortNumber			= lpDevice->nCommPort;
				SerialParam.dwBaud				= lpDevice->nBaudrate;
				SerialParam.bDataBit			= PORT_DATA_BIT;
				SerialParam.bStopBit			= PORT_STOP_BIT;
				SerialParam.bParity				= EVENPARITY;
				SerialParam.nDelimiter			= PORT_TERMINATE_CYCLER;
				SerialParam.bBufClear			= PORT_CLEAR_BUF;

				CSerialCom* lpSerialDevice = new CSerialCom(SerialParam);
				
				m_vecDevice.push_back(lpSerialDevice);

				if (eCommTypeRs485 == nChamberDeviceType)
				{
					// Add By JSK - 2020.07.04 Ãß°¡
					if (lpSerialDevice->OpenPort())
					{
						lpSerialDevice->Connect();
						lpSerialDevice->StartThread();
					}					
				}
				else
				{
					if (lpSerialDevice->OpenPort())
					{
						lpSerialDevice->Connect();
						lpSerialDevice->StartThread();
					}
				}
			}

			++iter;
		}	
	}	
}

void CComProc::StopThread()
{
	auto iter = m_vecDevice.begin();
	auto iterE = m_vecDevice.end();

	while (iter != iterE)
	{
		CSerialCom* lpDevice = (*iter);

		if (lpDevice)
			lpDevice->StopThread();

		++iter;
	}
}

void CComProc::AddDaqData(int nPrimaryKey, CString strTime, byte* pData, int nDataLenth)
{
	CMgrSerial::GetMgr()->AddQueue(nPrimaryKey, strTime, pData, nDataLenth);
}

void CComProc::AddChamberData(int nPrimaryKey, CString strTime, float fData)
{
	CMgrSerial::GetMgr()->AddQueue(nPrimaryKey, strTime, fData);
}

void CComProc::AddChamberDataTemi(int nPrimaryKey, CString strTime, float fData, int nType)
{
	CMgrSerial::GetMgr()->AddQueue(nPrimaryKey, strTime, fData, nType);
}

CSerialCom*	CComProc::GetAtComm(int nPrimaryKey)
{
	auto iter = m_vecDevice.begin();
	auto iterE = m_vecDevice.end();

	while (iter != iterE)
	{
		CSerialCom* lpDevice = (*iter);

		if (lpDevice)
		{
			if (lpDevice->GetPrimaryKey() == nPrimaryKey)
				return lpDevice;
		}

		++iter;
	}

	return nullptr;
}
