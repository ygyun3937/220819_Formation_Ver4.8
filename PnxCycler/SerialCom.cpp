#include "stdafx.h"
#include "SerialCom.h"

#include "ComProc.h"
#include "MgrComm.h"

#include "PnxUtil.h"
#include "CMgrChamber.h"

#include "MgrCommData.h"
#include "MgrChannel.h"
#include "MgrConfig.h"

#include "MgrDAQ.h"

static const int crc16tab[256] = {
	0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
	0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
	0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
	0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
	0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
	0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
	0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
	0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
	0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
	0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
	0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
	0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
	0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
	0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
	0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
	0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
	0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
	0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
	0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
	0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
	0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
	0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
	0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
	0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
	0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
	0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
	0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
	0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
	0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
	0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
	0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
	0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };

#define ChamberDefaultPrimaryKey_1	(81)
#define ChamberDefaultPrimaryKey_2	(82)
#define ChamberDefaultPrimaryKey_3	(83)

IMPLEMENT_DYNAMIC(CSerialCom, CWnd)

CSerialCom::CSerialCom(ST_SerialINI& refParam)
{
	m_hCom			= INVALID_HANDLE_VALUE;

	m_bLoop			= TRUE;
	m_bRecvCheck	= TRUE;

	m_pThread		= NULL;

	m_nDaqReadType	= READ_TYPE_VOLT;
	m_nFlowControl	= FC_NONE;
	m_nTimeOut		= 100;

	ZeroMemory(m_pszRecv, 1024);

	m_nRecvSize = 0;

	m_stSerialOptionValue = refParam;

	SetPrimaryKey(m_stSerialOptionValue.nPrimaryKey);
	SetPortNumberStr(m_stSerialOptionValue.nPortNumber);
	SetChamberChannel(m_stSerialOptionValue.nChamberChannel);
	SetChamberControlType(m_stSerialOptionValue.nChamberControlType);

	SetDeviceName(m_stSerialOptionValue.strName);
	SetDaqDataType(m_stSerialOptionValue.nDaqType);

	for (int i = 0; i < EVENT_CHAMBER_SIZE; ++i)
		SetChamberEvent(i);
}


CSerialCom::~CSerialCom()
{
	if (m_hCom != INVALID_HANDLE_VALUE)
	{
		Close();
	}
}

BEGIN_MESSAGE_MAP(CSerialCom, CWnd)
END_MESSAGE_MAP()
int		CSerialCom::GetChamberControlType()
{
	return m_nChamberControlType;
}

void	CSerialCom::SetChamberControlType(int nChamberControlType)
{
	m_nChamberControlType = nChamberControlType;
}

void	CSerialCom::SetChamberChannel(int nChamberNumber)
{
	m_nChamberChannelNum = nChamberNumber;
}

int		CSerialCom::GetChamberChannel()
{
	return m_nChamberChannelNum;
}

void CSerialCom::ClosePort()
{
	// ???? Event mask?? ??????.
	SetCommMask(m_hCom, 0);
	// ???? Queue???? ?????? ????.
	PurgeComm(m_hCom, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	// ???? ?????? ??????.
	CloseHandle(m_hCom);
}


BOOL CSerialCom::OpenPort()
{
 	//TRACE("[ INFO ] CSerialCom::OpenPort: %s, %d\n", GetPortNumberStr(), m_stSerialOptionValue.dwBaud);
	if (m_hCom != INVALID_HANDLE_VALUE)
	{
		TRACE("[ WARNING ] CSerialCom::OpenPort: already opened port \n");
		Close();
	}

	m_hCom = CreateFile(GetPortNumberStr(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (m_hCom == INVALID_HANDLE_VALUE)
	{
		if (m_hCom != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hCom);
		}
		m_hCom = INVALID_HANDLE_VALUE;

		return FALSE;
 	}

	SetupComm(m_hCom, 8192, 8192);
	PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
 
 	DCB  dcb;
	if (!GetCommState(m_hCom, &dcb))
	{	
		if (m_hCom != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hCom);
		}
		m_hCom = INVALID_HANDLE_VALUE;

		return FALSE;
 	}

// ???? flow control ????
 	dcb.BaudRate	= m_stSerialOptionValue.dwBaud;
	dcb.ByteSize	= m_stSerialOptionValue.bDataBit;				// Number of bits/byte, 4-8     
	dcb.StopBits	= m_stSerialOptionValue.bStopBit;				// 0,1,2 = 1, 1.5, 2               
	dcb.Parity		= m_stSerialOptionValue.bParity;				// 0-4=None,Odd,Even,Mark,Space    
	dcb.fParity		= m_stSerialOptionValue.bParity ? TRUE : FALSE;	// Enable parity checking          


// flow control ?????? ???? ?????? ?????? Enable
 	dcb.fOutxCtsFlow	= FALSE;									// CTS(Clear To Send) handshaking on output       
	dcb.fOutxDsrFlow	= FALSE;									// DSR(Data Set Ready) handshaking on output       
 	dcb.fDtrControl		= DTR_CONTROL_ENABLE;						// DTR(Data Terminal Ready) Flow control                
 	dcb.fRtsControl		= RTS_CONTROL_ENABLE;						// RTS(Ready To Send) Flow control                

// Xon, Xoff flow control ?? ???? ?? ?????? Enable ????.
	dcb.fOutX		= FALSE;										// Enable output X-ON/X-OFF        
	dcb.fInX		= FALSE;										// Enable input X-ON/X-OFF         

//Setup the flow control
 	dcb.fDsrSensitivity = FALSE;

 	switch (m_nFlowControl)
	{
	case FC_XONXOFF:				// Software Handshaking 
		dcb.fOutX		= true;
		dcb.fInX		= true;
		dcb.XonChar		= 0x11;		// Tx and Rx X-ON character      
		dcb.XoffChar	= 0x13;		// Tx and Rx X-OFF character   
		dcb.XoffLim		= 100;		// Transmit X-ON threshold 
		dcb.XonLim		= 100;		// Transmit X-OFF threshold    
		break;
	case FC_RTSCTS:
		dcb.fOutxCtsFlow	= true;
		dcb.fRtsControl		= RTS_CONTROL_HANDSHAKE;
		break;
	case FC_DTRDSR:
		dcb.fOutxDsrFlow	= true;
		dcb.fDtrControl		= DTR_CONTROL_HANDSHAKE;
		break;
	case FC_FULLHARDWARE:			// Hardware Handshaking
		dcb.fOutxCtsFlow	= true;
		dcb.fOutxDsrFlow	= true;
		dcb.fDtrControl		= DTR_CONTROL_HANDSHAKE;
		dcb.fRtsControl		= RTS_CONTROL_HANDSHAKE;
		break;
 	}

	dcb.fBinary				= TRUE;			// Binary Mode (skip EOF check)    
	dcb.fNull				= FALSE;		// Enable Null stripping           
	dcb.fAbortOnError		= FALSE;		// Abort all reads and writes on Error 
	dcb.fDsrSensitivity		= FALSE;		// DSR Sensitivity              
	dcb.fTXContinueOnXoff	= TRUE;			// Continue TX when Xoff sent 
	dcb.fErrorChar			= FALSE;		// Enable Err Replacement          
	dcb.ErrorChar			= 0;			// Error replacement char          
	dcb.EofChar				= 0;			// End of Input character          
	dcb.EvtChar				= 0;			// Received Event character        

	if (!SetCommState(m_hCom, &dcb))
	{
		if (m_hCom != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hCom);
		}
		m_hCom = INVALID_HANDLE_VALUE;
		
		return FALSE;
	}
 
	SetTimeout(m_nTimeOut, m_nTimeOut, 1);
 
// connection success
 	return true;
}

BOOL CSerialCom::Connect()
{
	if (m_hCom == INVALID_HANDLE_VALUE)
		OpenPort();
	return (m_hCom != INVALID_HANDLE_VALUE);
}

BOOL CSerialCom::Close()
{
	if (m_hCom != INVALID_HANDLE_VALUE) 
	{
		if (eCommTypeRs485 != CMgrComm::GetMgr()->GetCommType(eDeviceTypeChamber))
		{
			CloseHandle(m_hCom);
		}
		m_hCom = INVALID_HANDLE_VALUE;
		return TRUE;
	}
	return FALSE;
}

BOOL CSerialCom::SetTimeout(int readTimeout, int writeTimeout, int readIntervalTimeout)
{
	COMMTIMEOUTS commTimeout;
	GetCommTimeouts(m_hCom, &commTimeout);

	commTimeout.ReadIntervalTimeout = readIntervalTimeout;
	commTimeout.ReadTotalTimeoutMultiplier = 0;
	commTimeout.ReadTotalTimeoutConstant = readTimeout;
	commTimeout.WriteTotalTimeoutMultiplier = 0;
	commTimeout.WriteTotalTimeoutConstant = writeTimeout;

	if (SetCommTimeouts(m_hCom, &commTimeout)) {
		return TRUE;
	}

	return FALSE;
}

WORD		CSerialCom::CRC16(const BYTE *nData, WORD wLength)
{
	WORD temp, temp2, flag;
	temp = 0xFFFF;

	for (unsigned char i = 0; i < wLength; i++)
	{
		temp = temp ^ nData[i];
		for (unsigned char j = 1; j <= 8; j++)
		{
			flag = temp & 0x0001;
			temp >>= 1;
			if (flag)
				temp ^= 0xA001;
		}
	}
	
	temp2 = temp >> 8;
	temp = (temp << 8) | temp2;
	temp &= 0xFFFF;

	return temp;
}

void CSerialCom::SetPortNumberStr(int nPortNumber)
{
	if (nPortNumber > 4)
		m_strPortNumber.Format("//./COM%d", nPortNumber);
	else
		m_strPortNumber.Format("COM%d", nPortNumber);
}

CString CSerialCom::GetPortNumberStr()
{
	return m_strPortNumber;
}

void CSerialCom::StartThread()
{
	if (NULL == m_pThread)
		m_pThread = AfxBeginThread(SerialThread, this);
 
 	//assert(m_pThread);
 
	// Auto Delete ????????..
 	//m_pThread->m_bAutoDelete = FALSE;
 	//m_pThread->ResumeThread();
}

//20201102 turby Chamber ?????? ?????? ??????????.
UINT CSerialCom::SerialThread(LPVOID pParam)
{
	CSerialCom* lpDeviceSerial = (CSerialCom*)pParam;

	if (!lpDeviceSerial)
		return 0;

	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return 0;

	bool bExistDeviceChamber = false;
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
				if (lpDevice->nPrimaryKey == lpDeviceSerial->GetPrimaryKey())
					bExistDeviceChamber = true;
			}

			++iter;
		}
	}

	bool bExistDeviceDaQ = false;
	{
		if (lpDeviceSerial->GetPrimaryKey() == pMgr->GetPrimaryKey(eDeviceTypeDaQ))
			bExistDeviceDaQ = true;
	}
	
	while (lpDeviceSerial->m_bLoop)
	{
		if (false == bExistDeviceChamber && true == bExistDeviceDaQ)
		{
			if (lpDeviceSerial->IsConnected())
			{
				int nItem=0;
				nItem = lpDeviceSerial->GetDaqDataType();
				lpDeviceSerial->RunDaq(nItem);
			}
		}
		else if (true == bExistDeviceChamber)
		{
			int nDeviceTypeChamber = pMgr->GetCommType(eDeviceTypeChamber);

			if (eCommTypeRs485 != nDeviceTypeChamber)
			{
				lpDeviceSerial->RunChamber();
			}
			else
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
						lpDeviceSerial->SetPrimaryKey(lpDevice->nPrimaryKey);
					}

					if (lpDeviceSerial->IsConnected())
						lpDeviceSerial->RunChamber();
					++iter;
				}

				//lpDeviceSerial->OpenPort();
// 				if (lpDeviceSerial->IsConnected())
// 					lpDeviceSerial->RunChamber();
			}
		}		

		Sleep(100);
	}

	return 0;
}

void CSerialCom::StopThread()
{
	m_bLoop = FALSE;

	if (m_pThread)
	{
		::WaitForSingleObject(m_pThread->m_hThread, INFINITE);

		m_pThread = NULL;
	}
}

BOOL CSerialCom::IsConnected()
{
	if (m_hCom != INVALID_HANDLE_VALUE)
	{
		return TRUE;
	}
	return FALSE;
}

DWORD CSerialCom::Send(const char *msg, int len)
{
	if (m_hCom == INVALID_HANDLE_VALUE)
		return 0;

	DWORD writtenBytes = 0;
	if (WriteFile(m_hCom, msg, len, &writtenBytes, NULL)) {
		return writtenBytes;
	}

	//TRACE("[SERIAL SEND FAIL] BOOL CSerialCom::Send(const char *msg, int len)");

	Close();
	return 0;
}

int CSerialCom::Receive(char *buff, int length)
{
	if (m_hCom == INVALID_HANDLE_VALUE) return -1;

	DWORD readBytes = 0;
	if (ReadFile(m_hCom, buff, length, &readBytes, NULL))
	{
		return readBytes;
	}

	Close();

	return -1;
}

int CSerialCom::CountRx()
{
	if (m_hCom == INVALID_HANDLE_VALUE) return -1;

	DWORD  comError = 0;
	COMSTAT comStat;
	if (ClearCommError(m_hCom, &comError, &comStat)) {
		if (comError) {

		}
		return comStat.cbInQue;
	}

	return -1;
}

void CSerialCom::RunDaq(int DataType)
{
	CString strRequest;

	char pData[Size_Total_Frame];
	memset(pData, 0x00, sizeof(pData));

	int nReciveLenth = 0;

	int key = m_nPrimaryKey;
//SEND
	//if (m_nDaqReadType == eDaqDataTypeTemp)
	//{
	//	strRequest.Format("$TEMP?\r\n");
	//}

	//if (m_nDaqReadType == eDaQDataTypeVolt)
	//{
	//	strRequest.Format("$VOLT?");
	//}
	strRequest.Format("$ALLX?");

	Send(strRequest, strRequest.GetLength()+1 /*1 is NULL*/ );
	Sleep(100);

//RECIVE
	int nRet = (m_bRecvCheck) ? CountRx() : 1;

	if (nRet > 0)
	{
		nReciveLenth = Receive(pData, sizeof(pData));
	}
	else if (nRet == 0)
	{
		m_nDaqReadType = !(m_nDaqReadType);

		return;
	}
	else
	{
		Close();

		return;
	}

	Sleep(100);
//EXCEPTION
//'$'x							: SKIP
//'$'o					'@'x	: SKIP
//'$'o	DATA LENTH x	'@'o	: SKIP
//'$'o	DATA LENTH o	'@'o	: ADD

	int nDataIndex = 0;
	int nParseCount = 0;

	while (nDataIndex < nReciveLenth)
	{
		if (pData[nDataIndex] == '$')
		{
			while (nDataIndex < nReciveLenth)
			{
				if (pData[nDataIndex] == '@')
				{
					++nParseCount;

					if ( (nParseCount - Size_Byte_STX - Size_Byte_ETX) % 4 == 0)
					{
						char pDivisionData[1024];
						memset(pDivisionData, 0x00, sizeof(pData));

						int pCopyIndex = nDataIndex- (nParseCount-1);
						for (int i = 0; i < nParseCount; ++i)
						{
							pDivisionData[i] = pData[pCopyIndex];
							++pCopyIndex;
						}

						pDivisionData[nParseCount] = (char)m_nDaqReadType;

						int nTotalLenth = nParseCount + 1;
						//ADD QUEUE
						CComProc* pCom = CComProc::GetMgr();
						//2021-02-23 cnz
						if (pCom == NULL)
							continue;
						pCom->AddDaqData(m_nPrimaryKey, CPnxUtil::GetMgr()->GetTime(), (BYTE*)pDivisionData, nTotalLenth);

						nParseCount = 0;

						break;
					}
					else
					{
						//<'$'o> <DATA LENTH x>	<'@'o>	: SKIP
						m_nDaqReadType = !(m_nDaqReadType);
						return;
					}
				}

				if (nDataIndex == nReciveLenth -1)
				{
					//<'@'x>	: SKIP
					m_nDaqReadType = !(m_nDaqReadType);
					return;
				}

				++nDataIndex;
				++nParseCount;
			//	Sleep(5); // 2021-04-02
			}
		}

		++nDataIndex;
	//	Sleep(5); // 2021-04-02
	}

	//<'$'x> SKIP OR FINISH WORK..
	m_nDaqReadType = !(m_nDaqReadType);
	return;
}

void	CSerialCom::SendChamberTemp(float fValue)
{
//20181228 CHABMER SET (????2???? NEX1000???? VALUE * 10 ???? ?? START
	fValue *= 10;
//20181228 CHABMER SET (????2???? NEX1000???? VALUE * 10 ???? ?? END

	WORD CRC_return = 0;
	//BYTE bByte[1024] = {0,};
	BYTE bByte[64] = { 0, };
	CString sTempSp = _T("");

	float fTemp = fValue;
	if (fTemp == 0)
		fTemp = 25.0 * 10;
	bByte[0] = 0x01;
	bByte[1] = 0x06;
	bByte[2] = 0x00;
	bByte[3] = 0x67;	//	SetTemp CH1
	bByte[4] = 0x00;
	
	bByte[4] = (int)fTemp >> 8 & 0xff;
	bByte[5] = (int)fTemp & 0xff;

	CRC_return = CRC16((BYTE*)bByte, 6);

	bByte[6] = CRC_return >> 8 & 0xFF;
	bByte[7] = CRC_return & 0xFF;

	int	 nRequestLenth = 8;

	char pRequest[1024];
	memset(pRequest, 0x00, sizeof(pRequest));


	for (int i = 0; i < nRequestLenth; ++i)
	{
		pRequest[i] = bByte[i];
	}

	//SEND
	Send(pRequest, nRequestLenth);
	Sleep(100);
}

void	CSerialCom::m_fnSetSV(float	fTemp)
{
	m_fSetSV = fTemp;
}



void	CSerialCom::SendChamberSetTempValue()
{
	float fValue = m_fSetSV;

	//20181228 CHABMER SET (????2???? NEX1000???? VALUE * 10 ???? ?? START
	fValue *= 10;
	//20181228 CHABMER SET (????2???? NEX1000???? VALUE * 10 ???? ?? END

	WORD CRC_return = 0;
	//BYTE bByte[1024] = {0,};
	BYTE bByte[64] = { 0, };
	CString sTempSp = _T("");

	float fTemp = fValue;

	bByte[0] = 0x01;
	bByte[1] = 0x06;
	bByte[2] = 0x00;
	bByte[3] = 0x67;	//	SetTemp CH1
	bByte[4] = 0x00;

	bByte[4] = (int)fTemp >> 8 & 0xff;
	bByte[5] = (int)fTemp & 0xff;

	CRC_return = CRC16((BYTE*)bByte, 6);

	bByte[6] = CRC_return >> 8 & 0xFF;
	bByte[7] = CRC_return & 0xFF;

	int	 nRequestLenth = 8;

	char pRequest[1024];
	memset(pRequest, 0x00, sizeof(pRequest));


	for (int i = 0; i < nRequestLenth; ++i)
	{
		pRequest[i] = bByte[i];
	}

	//SEND
	Send(pRequest, nRequestLenth);
	Sleep(100);

	//TEST
	char pData[1024];
	memset(pData, 0, sizeof(pData));

	while (TRUE)
	{
		int nRecvSize = Receive(pData, 1024);

		if (nRecvSize <= 0)
			break;
	}

	SetChamberEvent(EVENT_CHAMBER_SET_TEMP, -1);
}

void	CSerialCom::SetDeviceName(CString strName)
{
	m_strDeviceName = strName;
}
CString CSerialCom::GetDeviceName()
{
	return m_strDeviceName;
}
void	CSerialCom::SetDaqDataType(int nType)
{
	m_nDaqDataType = nType;
}
int CSerialCom::GetDaqDataType()
{
	return m_nDaqDataType;
}

void		CSerialCom::SetChamberEvent(int nEventIndex, int nEventValue/*Default -1*/)
{
	m_nChamberEvent[nEventIndex] = nEventValue;
}

int*		CSerialCom::GetChamberEvent()
{
	return m_nChamberEvent;
}

void	CSerialCom::SendChamberOnOff(BOOL bOn)
{
	WORD CRC_return;

	int	 nRequestLenth = 8;

	char pCheck[1024];
	char pCRCByte[2];
	char pRequest[1024];

	memset(pCheck, 0x00, sizeof(pCheck));
	memset(pCRCByte, 0x00, sizeof(pCRCByte));
	memset(pRequest, 0x00, sizeof(pRequest));

	pCheck[0] = 0x01;
	pCheck[1] = 0x06;
	pCheck[2] = 0x00;

	if (GetChamberChannel() == 0)	//CH_1
	{
		pCheck[3] = 0x65;
	}

	if (GetChamberChannel() == 1)	//CH_2
	{
		pCheck[3] = 0x66;
	}

	pCheck[4] = 0x00;
	if (bOn)
		pCheck[5] = 0x01;
	else
		pCheck[5] = 0x04;

	CRC_return = CRC16((BYTE*)pCheck, 6);

	pCheck[6] = CRC_return >> 8 & 0xFF;
	pCheck[7] = CRC_return & 0xFF;

	for (int i = 0; i < nRequestLenth; ++i)
	{
		pRequest[i] = pCheck[i];
	}

	//SEND
	Send(pRequest, nRequestLenth);
	Sleep(100);
	char pData[1024];
	memset(pData, 0x00, sizeof(pData));

	while (TRUE)
	{
		int nReciveSize = Receive(pData, 1024);

		if (nReciveSize <= 0)
			break;
	}
	//RECIVE
	if (bOn == TRUE)
		SetChamberEvent(EVENT_CHAMBER_ON, -1);
	if (bOn == FALSE)
		SetChamberEvent(EVENT_CHAMBER_OFF, -1);
}


BOOL	CSerialCom::RunChamberNEX1000()
{
	//--> ?????? ???? ?????? ???? ???? ??????
	for (int i = 0; i < EVENT_CHAMBER_SIZE; ++i)
	{
		switch (GetChamberEvent()[i])
		{
		case EVENT_CHAMBER_SET_TEMP:	SendChamberSetTempValue(); break;
		case EVENT_CHAMBER_ON:			SendChamberOnOff(TRUE);	break;
		case EVENT_CHAMBER_OFF:			SendChamberOnOff(FALSE); break;
		}
	}

	Sleep(300);

	//--> ?????? ???? ?????? ???? ?????? ?????? RECV ????
	//MAKE COMMAND
	WORD CRC_return;

	int	 nRequestLenth = 8;

	char pCheck[1024];
	char pCRCByte[2];
	char pRequest[1024];

	memset(pCheck, 0x00, sizeof(pCheck));
	memset(pCRCByte, 0x00, sizeof(pCRCByte));
	memset(pRequest, 0x00, sizeof(pRequest));

	pCheck[0] = 0x01;
	pCheck[1] = 0x03;
	pCheck[2] = 0x00;

	//if (GetChamberChannel() == 0)	//CH_1
	{
		pCheck[3] = 0x00;
	}

	if (GetChamberChannel() == 1)	//CH_2
	{
		pCheck[3] = 0x0A;
	}

	pCheck[4] = 0x00;
	pCheck[5] = 0x10;

	CRC_return = CRC16((BYTE*)pCheck, 6);

	pCheck[6] = CRC_return >> 8 & 0xFF;
	pCheck[7] = CRC_return & 0xFF;

	for (int i = 0; i < nRequestLenth; ++i)
	{
		pRequest[i] = pCheck[i];
	}

	//SEND
	Send(pRequest, nRequestLenth);
	Sleep(100);

	//RECIVE
	int nReadSize = 0x51;

	char pData[1024];
	memset(pData, 0x00, sizeof(pData));

	int nReciveSize = Receive(pData, nReadSize);

	CString strRef;
	CString strTemp;

	if (nReciveSize > 0)
	{
		for (int i = 21; i < 28; ++i)
		{
			strTemp.Format("%02X", (BYTE)pData[i]);
			strRef += strTemp;
		}
	}
	else
	{
		//TRACE("[RECIVE 0] void	CSerialCom::RunChamber()");
		return FALSE;
	}

	//CRC CHECK
	memset(pCheck, 0x00, sizeof(pCheck));

	if (strRef.GetLength() < 12)		//???? READ???????? SIZE
	{
		return FALSE;
	}
	else
	{
		int hexDecTemp = CPnxUtil::GetMgr()->String2Hex(strRef.Mid(0, 4));
		int hexDecSet = CPnxUtil::GetMgr()->String2Hex(strRef.Mid(8, 4));

		float fChamberTemp = hexDecTemp * 0.1;
		float fChamberSet = hexDecSet * 0.1;
		if (hexDecTemp > 32767)
		{
			fChamberTemp = ((0xFFFF - hexDecTemp) + 1)*0.1;

			fChamberTemp *= -1;

		}
		if (hexDecSet > 32767)
		{
			fChamberSet = ((0xFFFF - hexDecSet) + 1)*0.1;

			fChamberSet *= -1;
		}
		CComProc* pCom = CComProc::GetMgr();
		pCom->AddChamberDataTemi(m_nPrimaryKey, CPnxUtil::GetMgr()->GetTime(), fChamberTemp, eChamberDataTypeValue);
		pCom->AddChamberDataTemi(m_nPrimaryKey, CPnxUtil::GetMgr()->GetTime(), fChamberSet, eChamberDataTypeSetting);
	}

	return TRUE;
}

//Chamber Set Function
void	CSerialCom::RunChamber()
{
	const int nCtrolType = GetChamberControlType();

	if (eChamberModelTypeTemi2500 == nCtrolType)
	{

//#if TURBY_CHAMBER_INITIAL_TEMP
#if 0
		Command_RRD();
#else
		RunChamberTemi2500(); //taek ?????? ???? ?????? ?????? ??????....
#endif
	}
	else if (eChamberModelTypeNex == nCtrolType)
	{
		RunChamberNEX1000();
	}
	else if (eChamberModelTT5166 == nCtrolType)
	{
		RunChamberTT5166();
	}
	else if (eChamberModelTypeOther == nCtrolType)
	{
		//MAKE COMMAND
		WORD CRC_return;

		int	 nRequestLenth = 8;

		char pCheck[1024];
		char pCRCByte[2];
		char pRequest[1024];

		memset(pCheck, 0x00, sizeof(pCheck));
		memset(pCRCByte, 0x00, sizeof(pCRCByte));
		memset(pRequest, 0x00, sizeof(pRequest));

		pCheck[0] = 0x01;
		pCheck[1] = 0x03;
		pCheck[2] = 0x00;

		if (GetChamberChannel() == 0)	//CH_1
		{
			pCheck[3] = 0x09;
		}

		if (GetChamberChannel() == 1)	//CH_2
		{
			pCheck[3] = 0x0A;
		}

		pCheck[4] = 0x00;
		pCheck[5] = 0x01;

		CRC_return = CRC16((BYTE*)pCheck, 6);

		pCheck[6] = CRC_return >> 8 & 0xFF;
		pCheck[7] = CRC_return & 0xFF;

		for (int i = 0; i < nRequestLenth; ++i)
		{
			pRequest[i] = pCheck[i];
		}

		//SEND
		Send(pRequest, nRequestLenth);
		Sleep(100);

		//RECIVE
		int nReadSize = 0x51;

		char pData[1024];
		memset(pData, 0x00, sizeof(pData));

		int nReciveSize = Receive(pData, nReadSize);

		CString strRef;
		CString strTemp;

		if (nReciveSize > 0)
		{
			for (int i = 0; i < nReciveSize; ++i)
			{
				strTemp.Format("%02X", (BYTE)pData[i]);
				strRef += strTemp;
			}
		}
		else
		{
			//TRACE("[RECIVE 0] void	CSerialCom::RunChamber()");
			return;
		}

		//CRC CHECK
		memset(pCheck, 0x00, sizeof(pCheck));

		if (strRef.GetLength() < 14)		//???? READ???????? SIZE
		{
			return;
		}

		for (int i = 8; i < nReciveSize; ++i)
		{
			pCheck[i - 8] = (char)CPnxUtil::GetMgr()->String2Hex(strRef.Mid((i)* 2, 2));
		}

		CRC_return = CRC16((BYTE*)pCheck, 5);

		pCRCByte[0] = CRC_return >> 8 & 0xFF;
		pCRCByte[1] = CRC_return & 0xFF;

		if ((pCheck[5] == pCRCByte[0]) && (pCheck[6] == pCRCByte[1]))
		{
			//ADD QUEUE
			float fValue = (float)(pCheck[3] << 8 | pCheck[4] & 0xFF);

			fValue *= 0.1f;

			CComProc* pCom = CComProc::GetMgr();
			//2021-02-23 cnz
			if (pCom == NULL)
				return;
			pCom->AddChamberData(m_nPrimaryKey, CPnxUtil::GetMgr()->GetTime(), fValue);
		}
		else
		{
			//TRACE("[RECIVE ERROR] void	CSerialCom::RunChamber() \t CH(%d) MSG(%s)", GetChamberChannel(), pCheck);
			return;
		}
	}
}

int		nchamber_temp = TURBY_CHAMBER_INITIAL_TEMP;
SChamber*	m_plpDeviceChamber[4];
bool		m_bconnectflag = false;
bool	CSerialCom::IsChamberExecute()
{
	CMgrChamber* pMgr = CMgrChamber::GetMgr();

	if (!pMgr)
		return false;

#if TURBY_CHAMBER_UPDATE
	if (!m_bconnectflag)
	{
		m_bconnectflag = TRUE;

		m_plpDeviceChamber[0] = pMgr->GetDevice(0);
		m_plpDeviceChamber[1] = pMgr->GetDevice(1);
		m_plpDeviceChamber[2] = pMgr->GetDevice(2);
		m_plpDeviceChamber[3] = pMgr->GetDevice(3);

		for (int i = 0; i < 4; i++)
		{
			if (!m_plpDeviceChamber[i])
				m_bconnectflag = false;
		}
	}
#else
	SChamber* lpDeviceChamber = pMgr->GetDevice(m_nChamberChannelNum);

	if (!lpDeviceChamber)
		return false;
#endif
	

#if TURBY_CHAMBER_UPDATE
#else
	if (lpDeviceChamber->fSettingValue == -100)
	{
		lpDeviceChamber->bSendChangeValue = false;

		return false;
	}
#endif
	// By JSK - ????????.. ???? ???? ?????? ???? \./ ??~~~?????? ????
	std::vector<CString> vecValue;
	{
		// 1. STX
		CString strValue;
		strValue.Format("%c", 0x02);

		vecValue.push_back(strValue);
	}

	{
		// 2. ADDRESS
		CString strValue = "01";

		if (GetChamberChannel() == 1)
			strValue = "02";
		else if (GetChamberChannel() == 2)
			strValue = "03";
		else if (GetChamberChannel() == 3)
			strValue = "04";

		vecValue.push_back(strValue);
	}

	{
		// 3. COMMAND
		vecValue.push_back("WRD");
	}

	{
		// 4. SEPERATOR
		vecValue.push_back(",");
	}

	{
		//5. LENGTH
		vecValue.push_back("01");
	}

	{
		// 6. SEPERATOR
		vecValue.push_back(",");
	}

	{
		// 7. REGISTER
		vecValue.push_back("0102");
	}

	{
		// 8. SEPERATOR
		vecValue.push_back(",");
	}

	{
		int nValue = (int)(m_plpDeviceChamber[m_nChamberChannelNum]->fSettingValue * 10.0f);

		if (nValue == 0)
		{
			nValue = nchamber_temp * 10;
			//TRACE(_T("33  %d %d  %d %f\n"), GetChamberChannel(), nValue, m_nChamberChannelNum, m_plpDeviceChamber[m_nChamberChannelNum]->fSettingValue);
		}
		else
		{
			nchamber_temp = nValue;
		}

		CString strValue;
		strValue.Format("%04X", 0xFFFF & nValue);

		vecValue.push_back(strValue);
	}

	Sleep(100); //???? ?????? ?????? ?????? ????

	CString strModbusPacket;

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		strModbusPacket += (*iter);
		++iter;
	}

	char lpszCheckSum[128] = { 0, };
	ZeroMemory(lpszCheckSum, 128);

	wsprintf(lpszCheckSum, "%s", strModbusPacket);

	int nCheckSum = 0;
	   	 
	int nLength = strModbusPacket.GetLength();

	for (int i = 1; i < nLength; ++i)
		nCheckSum = nCheckSum + lpszCheckSum[i];
	
	CString strCheckSum;
	strCheckSum.Format("%02X", nCheckSum & 0xFF);

	CString strETX;
	strETX.Format("%c%c", 0x0D, ASCII_LF);

	CString strValue;
	strValue = strModbusPacket + strCheckSum + strETX;

	char lpszPacket[128] = { 0, };
	ZeroMemory(lpszPacket, 128);

	wsprintf(lpszPacket, "%s", strValue);

	int nPacketSize = strValue.GetLength();

	DWORD dwSize = Send(lpszPacket, nPacketSize);

	if (dwSize > 0)
	{
		// ?????? ?????? Send ?? ???? ???????? ???? ???? ???? ????????
	}

	if (ReadChamberTypeTemi())
		m_plpDeviceChamber[m_nChamberChannelNum]->bSendChangeValue = false;

	return true;
}

bool	CSerialCom::IsChamberOn()
{
	CMgrChamber* pMgr = CMgrChamber::GetMgr();

	if (!pMgr)
		return false;

	SChamber* lpDeviceChamber = pMgr->GetDevice(m_nChamberChannelNum);

	if (!lpDeviceChamber)
		return false;

	if (lpDeviceChamber->fSettingValue == -100)
	{
		lpDeviceChamber->bSendOnCommand = false;
		lpDeviceChamber->bSendOffCommand = true;

		return false;
	}

	// By JSK - ????????.. ???? ???? ?????? ???? \./ ??~~~?????? ????
	std::vector<CString> vecValue;
	{
		// 1. STX
		CString strValue;
		strValue.Format("%c", 0x02);

		vecValue.push_back(strValue);
	}

	{
		// 2. ADDRESS
		CString strValue = "01";

		if (GetChamberChannel() == 1)
			strValue = "02";
		else if (GetChamberChannel() == 2)
			strValue = "03";
		else if (GetChamberChannel() == 3)
			strValue = "04";

		

		vecValue.push_back(strValue);
	}

	{
		// 3. COMMAND
		vecValue.push_back("WRD");
	}

	{
		// 4. SEPERATOR
		vecValue.push_back(",");
	}

	{
		//5. LENGTH
		vecValue.push_back("02");
	}

	{
		// 6. SEPERATOR
		vecValue.push_back(",");
	}

	{
		// 7. REGISTER
		vecValue.push_back("0101");
	}

	{
		// 8. SEPERATOR
		vecValue.push_back(",");
	}

	{
		// 9. On Command
		vecValue.push_back("0001");
	}

	{
		// 10. SEPERATOR
		vecValue.push_back(",");
	}

	{
		// 11. REGISTER 1
		vecValue.push_back("0104");
	}

	{
		// 12. SEPERATOR
		vecValue.push_back(",");
	}

	{
		// 13. ????
		vecValue.push_back("0001");
	}

	CString strModbusPacket;

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		strModbusPacket += (*iter);
		++iter;
	}

	char lpszCheckSum[128] = { 0, };
	ZeroMemory(lpszCheckSum, 128);

	wsprintf(lpszCheckSum, "%s", strModbusPacket);

	int nCheckSum = 0;

	int nLength = strModbusPacket.GetLength();

	for (int i = 1; i < nLength; ++i)
		nCheckSum = nCheckSum + lpszCheckSum[i];

	CString strCheckSum;
	strCheckSum.Format("%02X", nCheckSum & 0xFF);

	CString strETX;
	strETX.Format("%c%c", 0x0D, ASCII_LF);

	CString strValue;
	strValue = strModbusPacket + strCheckSum + strETX;

	char lpszPacket[128] = { 0, };
	ZeroMemory(lpszPacket, 128);

	wsprintf(lpszPacket, "%s", strValue);

	int nPacketSize = strValue.GetLength();

	DWORD dwSize = Send(lpszPacket, nPacketSize);

	if (dwSize > 0)
	{
		// ?????? ?????? Send ?? ???? ???????? ???? ???? ???? ????????
	}

	if (ReadChamberTypeTemi()) 
	{
		lpDeviceChamber->bSendOnCommand = false;
		lpDeviceChamber->bSendOffCommand = false;
		lpDeviceChamber->bChamberExec = true;
	}

	return true;
}

bool	CSerialCom::IsChamberOff()
{
	CMgrChamber* pMgr = CMgrChamber::GetMgr();

	if (!pMgr)
		return false;

	SChamber* lpDeviceChamber = pMgr->GetDevice(m_nChamberChannelNum);

	if (!lpDeviceChamber)
		return false;

	if (lpDeviceChamber->fSettingValue == -100)
	{
		lpDeviceChamber->bSendOffCommand = false;

		return false;
	}

	// By JSK - ????????.. ???? ???? ?????? ???? \./ ??~~~?????? ????
	std::vector<CString> vecValue;
	{
		// 1. STX
		CString strValue;
		strValue.Format("%c", 0x02);

		vecValue.push_back(strValue);
	}

	{
		// 2. ADDRESS
		CString strValue = "01";

		if (GetChamberChannel() == 1)
			strValue = "02";
		else if (GetChamberChannel() == 2)
			strValue = "03";
		else if (GetChamberChannel() == 3)
			strValue = "04";

		vecValue.push_back(strValue);
	}

	{
		// 3. COMMAND
		vecValue.push_back("WRD");
	}

	{
		// 4. SEPERATOR
		vecValue.push_back(",");
	}

	{
		//5. LENGTH
		vecValue.push_back("02");
	}

	{
		// 6. SEPERATOR
		vecValue.push_back(",");
	}

	{
		// 7. REGISTER
		vecValue.push_back("0101");
	}

	{
		// 8. SEPERATOR
		vecValue.push_back(",");
	}

	{
		// 9. Off Command
		vecValue.push_back("0004");
	}

	{
		// 10. SEPERATOR
		vecValue.push_back(",");
	}

	{
		// 11. REGISTER 1
		vecValue.push_back("0104");
	}

	{
		// 12. SEPERATOR
		vecValue.push_back(",");
	}

	{
		// 13. ????
		vecValue.push_back("0001");
	}

	CString strModbusPacket;

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		strModbusPacket += (*iter);
		++iter;
	}

	char lpszCheckSum[128] = { 0, };
	ZeroMemory(lpszCheckSum, 128);

	wsprintf(lpszCheckSum, "%s", strModbusPacket);

	int nCheckSum = 0;

	int nLength = strModbusPacket.GetLength();

	for (int i = 1; i < nLength; ++i)
		nCheckSum = nCheckSum + lpszCheckSum[i];

	CString strCheckSum;
	strCheckSum.Format("%02X", nCheckSum & 0xFF);

	CString strETX;
	strETX.Format("%c%c", 0x0D, ASCII_LF);

	CString strValue;
	strValue = strModbusPacket + strCheckSum + strETX;

	char lpszPacket[128] = { 0, };
	ZeroMemory(lpszPacket, 128);

	wsprintf(lpszPacket, "%s", strValue);

	int nPacketSize = strValue.GetLength();

	DWORD dwSize = Send(lpszPacket, nPacketSize);

	if (dwSize > 0)
	{
		// ?????? ?????? Send ?? ???? ???????? ???? ???? ???? ????????
	}

	if (ReadChamberTypeTemi())
	{
		lpDeviceChamber->bSendOnCommand = false;
		lpDeviceChamber->bSendOffCommand = false;
		lpDeviceChamber->bChamberExec = false;
	}

	return true;
}

bool	CSerialCom::ReadChamberTypeTemi()
{
	char lpszReceivePacket[1024] = { 0, };
	ZeroMemory(lpszReceivePacket, 1024);

	int nReceiveSize = Receive(lpszReceivePacket, 1024);

	//TRACE(_T("%d\n"), nReceiveSize);
	if (nReceiveSize <= 0)
		return false;

	CString strReceivePacket;
	strReceivePacket.Format("%s", lpszReceivePacket);

	// ???? ?????? ?????? Size ?? ??????????.. ???? ??????????
	//if (nReceiveSize == 11)
	if(strReceivePacket.GetLength() == 11)
	{
		CString strResult = strReceivePacket.Mid(3, 2);

		if (strcmp(strResult, "NG") == 0)
			return false;
	}
	
	if (strcmp(strReceivePacket, "Error") != 0)
	{
		// STX, ETX ????

		// Debug
		if (lpszReceivePacket[0] != 0x02 || lpszReceivePacket[nReceiveSize - 2] != 0x0D || lpszReceivePacket[nReceiveSize - 1] != 0x0A)
			return false;

		CString strReceiveCommand;
		CString strResult;
		CString strMergePacket;
		CString strValue1;
		CString strValue2;
		CString strValue3;

		int nIndexCount	= 0;

		for (int i = 3; i < nReceiveSize - 4; ++i)
		{
			CString strReceiveCRC16;
			strReceiveCRC16.Format("%c%c", lpszReceivePacket[nReceiveSize - 4], lpszReceivePacket[nReceiveSize - 3]);

			int nCheckSum = 0;

			for (int j = 1; j < nReceiveSize - 4; ++j)
			{
				CString strValue;				
				strValue = lpszReceivePacket[j];

				for (int k = 0; k < 127; ++k)
				{
					if (strcmp(strValue, g_lpszAsciiCode[k]) == 0)
					{
						nCheckSum = nCheckSum + k;
						break;
					}
				}
			}

			CString strCheckSum;
			strCheckSum.Format("%02X", nCheckSum & 0xFF);

			// Debug
			if(strcmp(strCheckSum, strReceiveCRC16) != 0)
				break;

			CString strPacket;
			strPacket.Format("%c", lpszReceivePacket[i]);

			if (strReceiveCommand.IsEmpty() || strResult.IsEmpty())
			{
				if (lpszReceivePacket[i] >= 0x44 && lpszReceivePacket[i] <= 0x5A)
				{
					strMergePacket = strMergePacket + strPacket;

					if (strcmp(strMergePacket, "RSD") == 0 || strcmp(strMergePacket, "RRD") == 0 || strcmp(strMergePacket, "WSD") == 0 || strcmp(strMergePacket, "WRD") == 0)
					{
						strReceiveCommand = strMergePacket;
						strMergePacket.Empty();
					}
					else if (strcmp(strMergePacket, "OK") == 0)
					{
						strResult = strMergePacket;
						strMergePacket.Empty();
					}

					continue;
				}				
			}
			else if (lpszReceivePacket[i] == 0x02 || strcmp(strPacket, ",") == 0)
			{
				continue;
			}
			else if (lpszReceivePacket[i] == 0x0D || lpszReceivePacket[i] == 0x0A)
			{
				break;
			}

			if (strcmp(strReceiveCommand, "RRD") == 0 && strcmp(strResult, "OK") == 0)
			{
				strMergePacket += strPacket;

				nIndexCount++;

				if (strValue1.IsEmpty() && nIndexCount >= 4)
				{
					strValue1 = strMergePacket;

					int nValue = CPnxUtil::GetMgr()->HexTextToInt(strValue1);

					if (nValue >> 15 == 0x1)
						nValue = (0xFFFF - nValue + 1) * -1;

					strMergePacket.Empty();
					nIndexCount = 0;

					float fValue = nValue * 0.1f;

					CComProc* lpQueue = CComProc::GetMgr();

					if (lpQueue)
						lpQueue->AddChamberDataTemi(m_nPrimaryKey, CPnxUtil::GetMgr()->GetTime(), fValue, eChamberDataTypeValue);
				}
				else if (strValue2.IsEmpty() && nIndexCount >= 4)
				{
					strValue2 = strMergePacket;

					int nValue = CPnxUtil::GetMgr()->HexTextToInt(strValue2);

					if (nValue >> 15 == 0x1)
						nValue = (0xFFFF - nValue + 1) * -1;

					strMergePacket.Empty();
					nIndexCount = 0;

					float fValue = nValue * 0.1f;

					CComProc* lpQueue = CComProc::GetMgr();

					if (lpQueue)
						lpQueue->AddChamberDataTemi(m_nPrimaryKey, CPnxUtil::GetMgr()->GetTime(), fValue, eChamberDataTypeSetting);
				}
				else if (strValue3.IsEmpty() && nIndexCount >= 4)
				{
					strValue3 = strMergePacket;

					int nValue = CPnxUtil::GetMgr()->HexTextToInt(strValue3);

					if (nValue >> 15 == 0x1)
						nValue = (0xFFFF - nValue + 1) * -1;

					strMergePacket.Empty();
					nIndexCount = 0;

					float fValue = nValue * 0.1f;

					CComProc* lpQueue = CComProc::GetMgr();

					if (lpQueue)
						lpQueue->AddChamberDataTemi(m_nPrimaryKey, CPnxUtil::GetMgr()->GetTime(), fValue, 2);
				}
			}
			else if (lpszReceivePacket[i] == 0x03)
			{
				continue;
			}
		}

		return true;
	}

	return false;
}

bool	CSerialCom::RunChamberTemi2500()
{
	CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();

 	if (pMgrChamber)
	{
		for (int i = 0; i < 4; ++i) // chamber ?????? 4??
		{
			SChamber* lpDeviceChamber = pMgrChamber->GetDevice(i);

			if (lpDeviceChamber)
			{
				SetChamberChannel(i);

				if (true == lpDeviceChamber->bSendChangeValue)
				{
					IsChamberExecute();
					Sleep(500);
					return true;
				}

				if (true == lpDeviceChamber->bSendOnCommand)
				{
					IsChamberOn();
					Sleep(500);
					return true;
				}

				if (true == lpDeviceChamber->bSendOffCommand)
				{
					IsChamberOff();
					Sleep(500);
					return true;
				}
			}
		}
	}

	Command_RRD();

	return true;
}

//Formation PreCharger
bool CSerialCom::RunChamberTT5166()
{
	CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();

	if (pMgrChamber)
	{
		SChamber* lpDeviceChamber = pMgrChamber->GetDevice(GetChamberChannel());
		if (lpDeviceChamber)
		{
			ChamberTT5166_IsAlarm();

			ChamberTT5166_IsChamberStatus();


			ChamberTT5166_IsTemp();


			Sleep(300);



			return true;
		}
	}

	return true;
}

void CSerialCom::SendChamber_TT5166_SetTempValue()
{
	float fValue = m_fSetSV;

	//20181228 CHABMER SET (????2???? NEX1000???? VALUE * 10 ???? ?? START
	//fValue *= 10;
	//20181228 CHABMER SET (????2???? NEX1000???? VALUE * 10 ???? ?? END

	WORD CRC_return = 0;
	//BYTE bByte[1024] = {0,};
	BYTE bByte[64] = { 0, };
	CString sTempSp = _T("");

	float fTemp = fValue;

	//TT-5166 Address
	bByte[0] = 0x00;
	//Function Code 06 (Writes Register Data)
	bByte[1] = 0x06;
	//Register Address (0x0026)
	bByte[2] = 0x00;
	bByte[3] = 0x26;	
	//Set Temp 
	bByte[4] = (int)fTemp >> 8 & 0xff;
	bByte[5] = (int)fTemp & 0xff;

	CRC_return = CRC16((BYTE*)bByte, 6);

	bByte[6] = CRC_return >> 8 & 0xFF;
	bByte[7] = CRC_return & 0xFF;

	int	 nRequestLenth = 8;

	char pRequest[1024];
	memset(pRequest, 0x00, sizeof(pRequest));


	for (int i = 0; i < nRequestLenth; ++i)
	{
		pRequest[i] = bByte[i];
	}

	//SEND
	Send(pRequest, nRequestLenth);
	Sleep(100);

	//TEST
	char pData[1024];
	memset(pData, 0, sizeof(pData));

	while (TRUE)
	{
		int nRecvSize = Receive(pData, 1024);

		if (nRecvSize <= 0)
			break;
	}

	SetChamberEvent(EVENT_CHAMBER_SET_TEMP, -1);
}

void CSerialCom::SendChamber_TT5166_OnOff(BOOL bOn)
{
	WORD CRC_return;

	int	 nRequestLenth = 8;

	char pCheck[1024];
	char pCRCByte[2];
	char pRequest[1024];

	memset(pCheck, 0x00, sizeof(pCheck));
	memset(pCRCByte, 0x00, sizeof(pCRCByte));
	memset(pRequest, 0x00, sizeof(pRequest));

	pCheck[0] = 0x00;	//Address
	pCheck[1] = 0x06;	//Function 
	pCheck[2] = 0x00;	//Start register high byte

	if (bOn)
	{	
		pCheck[3] = 0x00;	//Start register low byte
	}
	else
	{
		pCheck[3] = 0x01;	//Start register low byte
	}

	pCheck[4] = 0x00;
	pCheck[5] = 0x01;

	CRC_return = CRC16((BYTE*)pCheck, 6);

	pCheck[6] = CRC_return >> 8 & 0xFF;
	pCheck[7] = CRC_return & 0xFF;

	for (int i = 0; i < nRequestLenth; ++i)
	{
		pRequest[i] = pCheck[i];
	}

	//SEND
	Send(pRequest, nRequestLenth);
	Sleep(100);
	char pData[1024];
	memset(pData, 0x00, sizeof(pData));

	while (TRUE)
	{
		int nReciveSize = Receive(pData, 1024);

		if (nReciveSize <= 0)
			break;
	}
	//RECIVE
	if (bOn == TRUE)
		SetChamberEvent(EVENT_CHAMBER_ON, -1);
	if (bOn == FALSE)
		SetChamberEvent(EVENT_CHAMBER_OFF, -1);




}


bool CSerialCom::ChamberTT5166_IsChamberStatus()
{
	CMgrChamber* pMgr = CMgrChamber::GetMgr();

	if (!pMgr)
		return false;
	WORD CRC_return;
	SChamber* lpDeviceChamber = pMgr->GetDevice(m_nChamberChannelNum);

	int	 nRequestLenth = 8;

	char pCheck[1024];
	char pCRCByte[2];
	char pRequest[1024];

	memset(pCheck, 0x00, sizeof(pCheck));
	memset(pCRCByte, 0x00, sizeof(pCRCByte));
	memset(pRequest, 0x00, sizeof(pRequest));

	//pCheck[0] = 0x00;			//Address

	// 1. ADDRESS
	if (GetChamberChannel() == 0)
		pCheck[0] = 0x01;
	else if (GetChamberChannel() == 1)
		pCheck[0] = 0x02;
	else if (GetChamberChannel() == 2)
		pCheck[0] = 0x03;
	else if (GetChamberChannel() == 3)
		pCheck[0] = 0x04;
	else if (GetChamberChannel() == 4)
		pCheck[0] = 0x05;

	//Function code 03
	//Function code 03 reads the current value of the output register. 03 can be read at a single address or multiple addresses.
	pCheck[1] = 0x03;			//Function

	//Run/Stop State (00 18)
	pCheck[2] = 0x00;			//Start Register High Byte	
	pCheck[3] = 0x18;			//Start Register Low Byte

	pCheck[4] = 0x00;			//High Start Register Count in bytes (Register Count)
	pCheck[5] = 0x01;			//Low Start Registger Count (Register Count)

	CRC_return = CRC16((BYTE*)pCheck, 6);

	pCheck[6] = CRC_return >> 8 & 0xFF;
	pCheck[7] = CRC_return & 0xFF;

	for (int i = 0; i < nRequestLenth; ++i)
	{
		pRequest[i] = pCheck[i];
	}

	//SEND
	Send(pRequest, nRequestLenth);
	Sleep(100);

	//RECIVE
	int nReadSize = 0x51;

	char pData[1024];
	memset(pData, 0x00, sizeof(pData));

	int nReciveSize = Receive(pData, nReadSize);

	CString strRef;
	CString strTemp;

	if (nReciveSize > 0)
	{
		for (int i = 0; i < nReciveSize; ++i)
		{
			strTemp.Format("%02X", (BYTE)pData[i]);
			strRef += strTemp;
		}
	}
	else
	{
		//TRACE("[RECIVE 0] void	CSerialCom::RunChamber()");
		return false;
	}

	//CRC CHECK
	memset(pCheck, 0x00, sizeof(pCheck));

	if (strRef.GetLength() < 7)		//???? READ???????? SIZE
	{
		return false;
	}

	for (int i = 0; i < nReciveSize; ++i)
	{
		pCheck[i] = (char)CPnxUtil::GetMgr()->String2Hex(strRef.Mid((i) * 2, 2));
	}
		//ADD QUEUE
		float fOnOff = (float)(pCheck[4] & 0x01);
		if (fOnOff != 0x00)
		{
			lpDeviceChamber->bChamberExec = true;
		}
		else{

			lpDeviceChamber->bChamberExec = false;
		}



	return true;
}

bool CSerialCom::ChamberTT5166_IsTemp()
{
	CMgrChamber* pMgr = CMgrChamber::GetMgr();

	if (!pMgr)
		return false;
	WORD CRC_return;
	SChamber* lpDeviceChamber = pMgr->GetDevice(m_nChamberChannelNum);

	int	 nRequestLenth = 8;

	char pCheck[1024];
	char pCRCByte[2];
	char pRequest[1024];

	memset(pCheck, 0x00, sizeof(pCheck));
	memset(pCRCByte, 0x00, sizeof(pCRCByte));
	memset(pRequest, 0x00, sizeof(pRequest));

	//pCheck[0] = 0x00;			//Address

	// 1. ADDRESS
	if (GetChamberChannel() == 0)
		pCheck[0] = 0x01;
	else if (GetChamberChannel() == 1)
		pCheck[0] = 0x02;
	else if (GetChamberChannel() == 2)
		pCheck[0] = 0x03;
	else if (GetChamberChannel() == 3)
		pCheck[0] = 0x04;
	else if (GetChamberChannel() == 4)
		pCheck[0] = 0x05;
	//Function code 03
	//Function code 03 reads the current value of the output register. 03 can be read at a single address or multiple addresses.
	pCheck[1] = 0x03;			//Function 
	pCheck[1] = 0x03;			//Function

	//Temperature PV (00 00)
	pCheck[2] = 0x00;			//Start Register High Byte	
	pCheck[3] = 0x00;			//Start Register Low Byte


	pCheck[4] = 0x00;			//High Start Register Count in bytes (Register Count)
	pCheck[5] = 0x01;			//Low Start Registger Count (Register Count)

	CRC_return = CRC16((BYTE*)pCheck, 6);

	pCheck[6] = CRC_return >> 8 & 0xFF;
	pCheck[7] = CRC_return & 0xFF;

	for (int i = 0; i < nRequestLenth; ++i)
	{
		pRequest[i] = pCheck[i];
	}

	//SEND
	Send(pRequest, nRequestLenth);
	Sleep(100);

	//RECIVE
	int nReadSize = 0x51;

	char pData[1024];
	memset(pData, 0x00, sizeof(pData));

	int nReciveSize = Receive(pData, nReadSize);

	CString strRef;
	CString strTemp;

	if (nReciveSize > 0)
	{
		for (int i = 0; i < nReciveSize; ++i)
		{
			strTemp.Format("%02X", (BYTE)pData[i]);
			strRef += strTemp;
		}
	}
	else
	{
		//TRACE("[RECIVE 0] void	CSerialCom::RunChamber()");
		return false;
	}

	//CRC CHECK
	memset(pCheck, 0x00, sizeof(pCheck));

	if (strRef.GetLength() < 7)		//???? READ???????? SIZE
	{
		return false;
	}

	for (int i = 0; i < nReciveSize; ++i)
	{
		pCheck[i] = (char)CPnxUtil::GetMgr()->String2Hex(strRef.Mid((i) * 2, 2));
	}

	//ADD QUEUE
	float fValue = (float)(pCheck[3] << 8 | pCheck[4] & 0xFF);

	fValue *= 0.01f;

	CComProc* pCom = CComProc::GetMgr();
	//2021-02-23 cnz
	if (pCom == NULL)
		return false;
	lpDeviceChamber->fCurrentValue = fValue;

	return TRUE;
}

bool CSerialCom::ChamberTT5166_IsAlarm()
{
	CMgrChamber* pMgr = CMgrChamber::GetMgr();

	if (!pMgr)
		return false;
	WORD CRC_return;
	SChamber* lpDeviceChamber = pMgr->GetDevice(m_nChamberChannelNum);

	int	 nRequestLenth = 8;

	char pCheck[1024];
	char pCRCByte[2];
	char pRequest[1024];

	memset(pCheck, 0x00, sizeof(pCheck));
	memset(pCRCByte, 0x00, sizeof(pCRCByte));
	memset(pRequest, 0x00, sizeof(pRequest));

	//pCheck[0] = 0x00;			//Address

	// 1. ADDRESS
	if (GetChamberChannel() == 0)
		pCheck[0] = 0x01;
	else if (GetChamberChannel() == 1)
		pCheck[0] = 0x02;
	else if (GetChamberChannel() == 2)
		pCheck[0] = 0x03;
	else if (GetChamberChannel() == 3)
		pCheck[0] = 0x04;
	else if (GetChamberChannel() == 4)
		pCheck[0] = 0x05;

	//Function code 03
	//Function code 03 reads the current value of the output register. 03 can be read at a single address or multiple addresses.
	pCheck[1] = 0x03;			//Function 

	//Fault information (00 1B)
	pCheck[2] = 0x00;			//Start Register High Byte	
	pCheck[3] = 0x1B;			//Start Register Low Byte

	pCheck[4] = 0x00;			//High Start Register Count in bytes (Register Count)
	pCheck[5] = 0x01;			//Low Start Registger Count (Register Count)

	CRC_return = CRC16((BYTE*)pCheck, 6);

	pCheck[6] = CRC_return >> 8 & 0xFF;
	pCheck[7] = CRC_return & 0xFF;

	for (int i = 0; i < nRequestLenth; ++i)
	{
		pRequest[i] = pCheck[i];
	}

	//SEND
	Send(pRequest, nRequestLenth);
	Sleep(100);

	//RECIVE
	int nReadSize = 0x51;

	char pData[1024];
	memset(pData, 0x00, sizeof(pData));

	int nReciveSize = Receive(pData, nReadSize);

	CString strRef;
	CString strTemp;

	if (nReciveSize > 0)
	{
		for (int i = 0; i < nReciveSize; ++i)
		{
			strTemp.Format("%02X", (BYTE)pData[i]);
			strRef += strTemp;
		}
	}
	else
	{
		//TRACE("[RECIVE 0] void	CSerialCom::RunChamber()");
		return false;
	}

	//CRC CHECK
	memset(pCheck, 0x00, sizeof(pCheck));

	if (strRef.GetLength() < 7)		//???? READ???????? SIZE
	{
		return false;
	}

	for (int i = 0; i < nReciveSize; ++i)
	{
		pCheck[i] = (char)CPnxUtil::GetMgr()->String2Hex(strRef.Mid((i) * 2, 2));
	}

	//ADD QUEUE
	int nAlarm = (int)(pCheck[3] << 8 | pCheck[4] & 0xFF);
	if (nAlarm != 0x00&& nAlarm<65 && nAlarm >0)
	{
		lpDeviceChamber->bChamberAlarm = true;
		lpDeviceChamber->nAlarmCode = nAlarm;
	}
	else
	{
		lpDeviceChamber->bChamberAlarm = false;
	}
	return true;
}


bool CSerialCom::Command_RRD()
{
	// ????
	std::vector<CString> vecValue;

	{
		CString strValue;
		strValue.Format("%c", 0x02);

		vecValue.push_back(strValue);
	}

	{
		// 2. ADDRESS
		CString strValue = "01";

		if (ChamberDefaultPrimaryKey_1 == m_nPrimaryKey)
			strValue = "02";
		else if (ChamberDefaultPrimaryKey_2 == m_nPrimaryKey)
			strValue = "03";
		else if (ChamberDefaultPrimaryKey_3 == m_nPrimaryKey)
			strValue = "04";

		vecValue.push_back(strValue);
	}

	{
		// 3. COMMAND
		vecValue.push_back("RRD");
	}

	{
		// 4. SEPERATOR
		vecValue.push_back(",");
	}

	{
		//5. LENGTH
		vecValue.push_back("02");
	}

	{
		// 6. SEPERATOR
		vecValue.push_back(",");
	}

	{
		// 7. REGISTER
		vecValue.push_back("0001");
	}

	{
		// 8. SEPERATOR
		vecValue.push_back(",");
	}

	{
		// 9. On Command
		vecValue.push_back("0003");
	}

	CString strModbusPacket;

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		strModbusPacket += (*iter);
		++iter;
	}

	char lpszCheckSum[128] = { 0, };
	ZeroMemory(lpszCheckSum, 128);

	wsprintf(lpszCheckSum, "%s", strModbusPacket);

	int nCheckSum = 0;

	int nLength = strModbusPacket.GetLength();

	for (int i = 1; i < nLength; ++i)
		nCheckSum = nCheckSum + lpszCheckSum[i];

	CString strCheckSum;
	strCheckSum.Format("%02X", nCheckSum & 0xFF);

	CString strETX;
	strETX.Format("%c%c", 0x0D, ASCII_LF);

	CString strValue;
	strValue = strModbusPacket + strCheckSum + strETX;

	char lpszPacket[128] = { 0, };
	ZeroMemory(lpszPacket, 128);

	wsprintf(lpszPacket, "%s", strValue);

	int nPacketSize = strValue.GetLength();

	DWORD dwSize = Send(lpszPacket, nPacketSize);

	Sleep(100);

	if (dwSize > 0)
	{
		// ?????? ?????? Send ?? ???? ???????? ???? ???? ???? ????????
	}

	ReadChamberTypeTemi();

	return false;
}