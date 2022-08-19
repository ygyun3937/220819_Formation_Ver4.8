#pragma once

/************************************************************************/
/* Greend */
/************************************************************************/

class CZebraBCRControl : public CWinThread
{
	DECLARE_DYNCREATE(CZebraBCRControl)

public:
	CZebraBCRControl();
	virtual ~CZebraBCRControl();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	void WaitTime(DWORD dwMillisecond);
	BOOL GetConnect();
protected:
	DECLARE_MESSAGE_MAP()

private:
	BOOL GetMsg();
	BOOL OpenComPort();
	BOOL OpenComPortCheck();

	HANDLE	m_hSerial;
	BOOL m_nConnected;
	int m_nChannelCnt;
	BOOL m_bTempUse;

	CString m_strBarcode;
	CString m_strTrayBarcode;
public:
	CString strReceive;
	BOOL GetReceiveData();

	BOOL IsConnect()	{ return m_nConnected; };
	void SetInit(int nPort, int nBaudRate, int nByteSize, int nParity, int nStopBit, int nChannelCnt, BOOL nTempUse=FALSE);

	int m_nPort;
	int m_nBaudRate;
	int m_nByteSize;
	int m_nParity;
	int m_nStopBits;
	//220331 YGYUN Barcode 관련 추가 
	void SetBarcode(CString strBarcode);
	CString GetBarcode();

	CString	GetTrayBarcode() { return m_strTrayBarcode; }
	void	SetTrayBarcode(CString str) { m_strTrayBarcode = str; }

};


