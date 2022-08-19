#pragma once

// CClientSocket ��� ����Դϴ�.
#include <queue>// CClientSocket ��� ����Դϴ�.
#include <map>

#define MAX_BUF_SIZE	(1024*128)  // �⺻ ���� ������� 16384 �̴�
#define RECV_BUF_SIZE	(1024*16)

#define SEND_RET_FAIL	(-1)

using namespace std;

class CClientSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();

	CString	GetServerIP();
	void	SetServerIP(CString strIP);

	int		GetServerPort();
	void	SetServerPort(int nPort);

	int		GetPrimaryKey();
	void	SetPrimaryKey(int nKey);

	int		GetBoardIndex();
	void	SetBoardIndex(int nValue);

	// I/F Board �� ��� 1���� Socket �� Max 4���� ä���� ������ ������, GUI ǥ���� ���ؼ��� ������ �־�� �Ѵ�.
	CStringArray&	GetChannelArray();
	void	SetChannelArray(CStringArray& arr);

	void	ConnectToServer();
	void	ConnectToServer_PLC();
	void	CloseClientSocket();

	BOOL	GetConnect();
	BOOL	GetConnect(int nKey);				//220816 YGY Socket ���� ���� Ȯ��
	BOOL	GetConnect_PLCInterface();

	void	ReceiveBoard();
	void	ReceivePlcInterface();

	int		SendToServer(CString strMsg);

	// char[], char* �� ��� NULL ���ڸ� ������ ���ڿ� ����� �ν� �ϱ� ������.. ���̸� �ȳ־��ָ� �̻��ϰ� ������ -_-
	int		SendToServer(char* pszMsg, int nSize);
	int		SendToServer_PLCINTERFACE(char* pszMsg, int nSize);

	void	SetConnect(BOOL bConnect);
	void	SetConnect_PLCINTERFACE(BOOL bConnect);



	void Add(int nKey, BOOL bConnect);


	void	StartThread();
	void	StopThread();
	void	CloseSocket();

	static UINT OnReceive(LPVOID pParam);

	BOOL	IsAliveTickOver();

	void	StartLinkThread();
	void	StopLinkThread();
	void	AddBuffer(char* pszRecv, int nRecvSize);
	void	AddBuffer_PLCInterface(char* pszRecv, int nRecvSize);
	int	    FindSTX(int nIndex);
	int	    FindSTX_PLCInterface(int nIndex);
private:
	CCriticalSection	m_csLock;
	CCriticalSection	m_csLock_PLCInterface;

	CStringArray	m_arrChannel;

	CString	m_strServerIP;
	int		m_nServerPort;

	BOOL	m_bConnect;
	BOOL	m_bConnect_PLCInterface;

	int		m_nPrimaryKey;
	int		m_nBoardIndex;

	SOCKET			m_SockClient;
	SOCKET			m_SockConnectClient;

	SOCKADDR_IN		m_AddrClient;

	CWinThread*	m_pThread;
	BOOL		m_bLoop;
	BOOL		m_bLoop_PLCInterface;

	char		m_pszRecv[MAX_BUF_SIZE];
	char		m_pszRecvSwap[MAX_BUF_SIZE];
	int			m_nRecvSize;

	char		m_pszRecv_PLCInterface[MAX_BUF_SIZE];
	char		m_pszRecvSwap_PLCInterface[MAX_BUF_SIZE];
	int			m_nRecvSize_PLCInterface;

	BOOL	m_bThreadLink;

	ULONGLONG	m_dwAliveTick;		//Alive Packet Tick

	CWinThread* m_pLinkThread;	
	ULONGLONG   m_dLastAckTime[4]; // board�� 4ä���̹Ƿ� 4�� �迭�� ����
	char		m_pszRecvTemp[RECV_BUF_SIZE];

	int			m_nSearchIndex;

	std::map<int, BOOL> m_mapConnect;
private:
	static UINT LinkThread(LPVOID pParam);	
};