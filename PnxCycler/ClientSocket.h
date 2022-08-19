#pragma once

// CClientSocket 명령 대상입니다.
#include <queue>// CClientSocket 명령 대상입니다.
#include <map>

#define MAX_BUF_SIZE	(1024*128)  // 기본 스텍 사이즈는 16384 이다
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

	// I/F Board 의 경우 1개의 Socket 에 Max 4개의 채널이 물리기 때문에, GUI 표현을 위해서라도 가지고 있어야 한다.
	CStringArray&	GetChannelArray();
	void	SetChannelArray(CStringArray& arr);

	void	ConnectToServer();
	void	ConnectToServer_PLC();
	void	CloseClientSocket();

	BOOL	GetConnect();
	BOOL	GetConnect(int nKey);				//220816 YGY Socket 연결 각각 확인
	BOOL	GetConnect_PLCInterface();

	void	ReceiveBoard();
	void	ReceivePlcInterface();

	int		SendToServer(CString strMsg);

	// char[], char* 의 경우 NULL 문자를 만나면 문자열 종료로 인식 하기 때문에.. 길이를 안넣어주면 이상하게 나간다 -_-
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
	ULONGLONG   m_dLastAckTime[4]; // board당 4채널이므로 4개 배열만 쓰자
	char		m_pszRecvTemp[RECV_BUF_SIZE];

	int			m_nSearchIndex;

	std::map<int, BOOL> m_mapConnect;
private:
	static UINT LinkThread(LPVOID pParam);	
};