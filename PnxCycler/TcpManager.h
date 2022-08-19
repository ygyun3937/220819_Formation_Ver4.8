#if !defined(AFX_TCPMANAGER_H__96DA77D6_99A9_4FED_A649_425EF57ECB92__INCLUDED_)
#define AFX_TCPMANAGER_H__96DA77D6_99A9_4FED_A649_425EF57ECB92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcpSock.h"
#include "safequeue.h"

class TcpInfo
{
public:
	TcpInfo();
	~TcpInfo();
	CString strIP;
	int nHandle;
	int	nPort;
	CTcpSock	*sock;
	BOOL		bConnect;
	SafeQueue<CString>	m_pSendList;
	static UINT ThreadTcpSend(LPVOID lParam);

	//Moxa Board 
	int nChCount;
	CString nIOType;
};

class CData
{
public:
	CData()
	{
		memset(data, 0x00, 4095); 
		len = 0;
		sendtime = 0;
		retrycnt = 0;
	}

public:
	char	data[4095];
	int		len;
	clock_t sendtime;
	int		retrycnt;
};

class CTcpManager : public CWnd
{
public:
	CTcpManager();
	virtual ~CTcpManager();

protected:
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_TCPMANAGER_H__96DA77D6_99A9_4FED_A649_425EF57ECB92__INCLUDED_)
