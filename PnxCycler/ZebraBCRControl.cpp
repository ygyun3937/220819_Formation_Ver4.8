#include "stdafx.h"
#include "PnxCycler.h"
#include "ZebraBCRControl.h"
#include "CDlgViewEqpChannelStatus.h"

#include "MgrSerial.h"
#include "MgrStatus.h"
#include "MgrConfig.h"
#include "MgrComm.h"
#include "CMgrModel.h"
#include "CMgrAccount.h"

IMPLEMENT_DYNCREATE(CZebraBCRControl, CWinThread)

CZebraBCRControl::CZebraBCRControl()
{
	m_nConnected = FALSE;
	m_nChannelCnt = 0;

	m_hSerial = NULL;
	strReceive = _T("");
	m_strBarcode = "";
}

CZebraBCRControl::~CZebraBCRControl()
{
}

BOOL CZebraBCRControl::InitInstance()
{
	m_nConnected = FALSE;

	return TRUE;
}

int CZebraBCRControl::ExitInstance()
{
	m_nConnected = FALSE;

	if (m_hSerial != NULL)
	{
		CloseHandle(m_hSerial);
		m_hSerial = NULL;
	}

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CZebraBCRControl, CWinThread)
END_MESSAGE_MAP()

int CZebraBCRControl::Run()
{
	DWORD dwTime = GetTickCount();

	while(TRUE)
	{
		Sleep(10);

		if (!GetMsg()){
			ExitInstance(); 
			return 0;
		}

		if (!OpenComPortCheck()){
			WaitTime(3000);
			continue;
		}
		else
		{
			GetReceiveData();
		}
	}

	return CWinThread::Run();
}

BOOL CZebraBCRControl::GetMsg()
{
	MSG	msg;
	
	while(TRUE)
	{
		if(!PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
			break;

		switch(msg.message)	
		{
		case WM_QUIT:
			{
				return FALSE;
			}
		//case WM_MSG_THREAD_CONTROLLER_TEMP_GREEN_SETDIM:
			{

				break;
			}
		//case WM_MSG_THREAD_CONTROLLER_TEMP_GREEN_SETONOFF:
			{
				break;
			}
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		Sleep(1);
	}

	return TRUE;
}

BOOL CZebraBCRControl::OpenComPortCheck()
{
	try
	{
		if (GetConnect() == false)
		{
			if (!OpenComPort())
			{
				return FALSE;
			}
		}

		return TRUE;
	}
	catch (const std::exception&)
	{
		return false;
	}
	
}



BOOL CZebraBCRControl::OpenComPort()
{
	TCHAR szPort[15] ;
	COMMTIMEOUTS commTimeOuts;

	if (m_nPort >= 10)	
	{
		_stprintf(szPort, _T("\\\\.\\COM%d"), m_nPort);
	}
	else
	{
		_stprintf(szPort, _T("COM%d"), m_nPort);
	}
	
	m_hSerial = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(m_hSerial == (HANDLE) -1)
	{
		m_nConnected = FALSE;
		return FALSE;
	}

	SetupComm(m_hSerial, 4096, 4096);
	PurgeComm(m_hSerial, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	commTimeOuts.ReadIntervalTimeout = 100; //0xFFFFFFFF;
	commTimeOuts.ReadTotalTimeoutMultiplier = 0;
	commTimeOuts.ReadTotalTimeoutConstant = 100;
	commTimeOuts.WriteTotalTimeoutMultiplier = 0;
	commTimeOuts.WriteTotalTimeoutConstant = 100;


	SetCommTimeouts(m_hSerial, &commTimeOuts);

	BOOL bRet = FALSE;
	DCB dcb;
	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hSerial, &dcb);
	dcb.BaudRate	= m_nBaudRate;
	dcb.ByteSize	= m_nByteSize;
	dcb.Parity		= m_nParity;
	dcb.StopBits	= m_nStopBits;
	dcb.fDtrControl	= DTR_CONTROL_DISABLE;
	dcb.fBinary		= TRUE;
	dcb.fParity		= TRUE;




	bRet = SetCommState(m_hSerial, &dcb);

	if (bRet)
	{
		m_nConnected = TRUE;
	}
	else
	{
		m_nConnected = FALSE;

		if (m_hSerial != NULL){
			CloseHandle(m_hSerial);
			m_hSerial = NULL;
		}

		return FALSE;
	}

	return TRUE;
}



void CZebraBCRControl::WaitTime(DWORD dwMillisecond)
{
	DWORD dwStart;
	dwStart = GetTickCount();

	while (GetTickCount() - dwStart < dwMillisecond)
	{
		Sleep(10);
	}
}

BOOL CZebraBCRControl::GetConnect()
{
	return m_nConnected; 
}

void CZebraBCRControl::SetInit(int nPort, int nBaudRate, int nByteSize, int nParity, int nStopBit, int nChannelCnt, BOOL nTempUse)
{
	m_nPort = nPort;
	m_nBaudRate = nBaudRate;
	m_nByteSize = nByteSize;
	m_nParity = nParity;
	m_nStopBits = nStopBit;
	m_nChannelCnt = nChannelCnt;
}

void CZebraBCRControl::SetBarcode(CString strBarcode)
{
	m_strBarcode = strBarcode;

}

CString CZebraBCRControl::GetBarcode()
{
	return m_strBarcode;
}

//Read Data
BOOL CZebraBCRControl::GetReceiveData()
{
	if (!m_nConnected) {
		return FALSE;
	}

	char	szBufRcv[256];
	DWORD	dwLen;
	CMgrSerial* pMgrSerial = CMgrSerial::GetMgr();
	

	memset(szBufRcv, 0, sizeof(szBufRcv));
	ReadFile(m_hSerial, szBufRcv, 1024, &dwLen, 0);
	{
		//받은 데이터 
		CString strReceive = _T("");

		if (dwLen <= 0) {
			return FALSE;
		}
		else
		{
			strReceive.Format(_T("%s"), szBufRcv);
			SetBarcode(strReceive);

			memset(szBufRcv, 0, sizeof(szBufRcv));
			return TRUE;
		}
	}

	return FALSE;
}
