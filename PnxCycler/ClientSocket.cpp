// ClientSocket.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "ClientSocket.h"

#include "PnxUtil.h"
#include "MgrEthernet.h"
#include "MgrComm.h"
#include "MgrChannel.h"
#include "MgrAlarm.h"
#include "CMgrChamber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma warning(disable:4996)


// CClientSocket

CClientSocket::CClientSocket()
{
	m_bConnect		= FALSE;
	m_bConnect_PLCInterface = FALSE;
	m_nPrimaryKey	= -1;
	m_nBoardIndex	= -1;
	m_SockClient	= NULL;
	m_SockConnectClient = NULL;
	m_pThread		= NULL;
	m_bLoop			= FALSE;
	m_bLoop_PLCInterface = FALSE;

	ZeroMemory(m_pszRecv, MAX_BUF_SIZE);
	ZeroMemory(m_pszRecvSwap, MAX_BUF_SIZE);	
	ZeroMemory(m_pszRecv_PLCInterface, MAX_BUF_SIZE);
	ZeroMemory(m_pszRecvSwap_PLCInterface, MAX_BUF_SIZE);
	ZeroMemory(m_pszRecvTemp, RECV_BUF_SIZE);
	
	m_nRecvSize = 0;
	m_nRecvSize_PLCInterface = 0;

	m_pLinkThread = NULL;
	m_bThreadLink = FALSE;
	m_dwAliveTick = ::GetTickCount64();

	m_nSearchIndex = 0;
}

CClientSocket::~CClientSocket()
{
	if (TRUE == m_bConnect || TRUE == m_bConnect_PLCInterface)
		CloseClientSocket();
	
}

CString	CClientSocket::GetServerIP()
{
	return m_strServerIP;
}

void	CClientSocket::SetServerIP(CString strIP)
{
	m_strServerIP = strIP;
}

int		CClientSocket::GetServerPort()
{
	return m_nServerPort;
}

void	CClientSocket::SetServerPort(int nPort)
{
	m_nServerPort = nPort;
}

int		CClientSocket::GetPrimaryKey()
{
	return m_nPrimaryKey;
}

void	CClientSocket::SetPrimaryKey(int nKey)
{
	m_nPrimaryKey = nKey;
}

int		CClientSocket::GetBoardIndex()
{
	return m_nBoardIndex;
}

void	CClientSocket::SetBoardIndex(int nValue)
{
	m_nBoardIndex = nValue;
}

CStringArray&	CClientSocket::GetChannelArray()
{
	return m_arrChannel;
}

void	CClientSocket::SetChannelArray(CStringArray& arr)
{
	m_arrChannel.RemoveAll();

	for (int i = 0 ; i < arr.GetSize() ; ++i)
	{
		m_arrChannel.Add(arr.GetAt(i));
	}
}

BOOL	CClientSocket::GetConnect()
{
	return m_bConnect;
}

BOOL CClientSocket::GetConnect_PLCInterface()
{
	return m_bConnect_PLCInterface;
}

void	CClientSocket::SetConnect(BOOL bConnect)
{
	//로컬 프로그램 종료 되면 알람 보내는 기능인가?
	m_bConnect = bConnect;
}

void CClientSocket::SetConnect_PLCINTERFACE(BOOL bConnect)
{
	m_bConnect_PLCInterface = bConnect;
}

// CClientSocket 멤버 함수

void CClientSocket::Add(int nKey, BOOL bConnect)
{
	auto it = m_mapConnect.find(nKey);

	if (it == m_mapConnect.end())
	{
		m_mapConnect.insert(std::pair<int, BOOL>(nKey, bConnect));
	}
}

BOOL CClientSocket::GetConnect(int nKey)
{
	auto it = m_mapConnect.find(nKey);
	
	if (it == m_mapConnect.end())
	{
		return false;
	}
	
	return it->second;
}

void	CClientSocket::StartThread()
{

	if (!m_pThread)
	{
		m_pThread = AfxBeginThread(OnReceive, this);
	}		
}

void	CClientSocket::StopThread()
{
	m_bLoop = FALSE;
	m_bLoop_PLCInterface = FALSE;

	if (m_pThread)
	{
		::WaitForSingleObject(m_pThread->m_hThread, INFINITE);

		m_pThread = NULL;

		SetConnect(FALSE);
		SetConnect_PLCINTERFACE(FALSE);
		closesocket(m_SockClient);
		closesocket(m_SockConnectClient);
		WSACleanup();
	}
}

void	CClientSocket::CloseSocket()
{
	closesocket(m_SockClient);
	closesocket(m_SockConnectClient);
	WSACleanup();
	m_SockClient = NULL;
	m_SockConnectClient = NULL;
}

UINT CClientSocket::OnReceive(LPVOID pParam)
{
	CClientSocket* pSocket = (CClientSocket*)pParam;
	CCommEthernet* pMgr = (CCommEthernet*)pParam;

	if (!pMgr)
		return 0;

	if (!pSocket)
		return 0;
	int i = 0;
	while (pSocket->m_bLoop)
	{
		pSocket->IsAliveTickOver();
		int nPrimaryKey = pSocket->GetPrimaryKey();
		if (nPrimaryKey == 1200)
		{
			pSocket->ReceivePlcInterface();
		}
		if (nPrimaryKey != 1200)// (nPrimaryKey == 0)
		{
			if (pSocket->GetBoardIndex() >= 0)
				//SYSLOG(Dbg, "%d",i++);
				pSocket->ReceiveBoard();
		}
		
		Sleep(1);
	}

	return 0;
}

BOOL CClientSocket::IsAliveTickOver()
{
	if (GetTickCount() > m_dwAliveTick)
	{
		m_dwAliveTick = ::GetTickCount64() + 1000;
		return true;
	}

	return FALSE;
}

void CClientSocket::ReceiveBoard()
{
	int nRecvSize = recv(m_SockClient, m_pszRecvTemp, RECV_BUF_SIZE, MSG_PEEK);
	if ((nRecvSize <= 0))
		return;

	ZeroMemory(m_pszRecvTemp, RECV_BUF_SIZE);
	int nRecvSize2 = recv(m_SockClient, m_pszRecvTemp, nRecvSize, 0);
	if ((nRecvSize2 <= 0))
	{
		return;
	}
	//printf("%s \r\n",m_pszRecvTemp);

	AddBuffer(m_pszRecvTemp, nRecvSize2);
}

void CClientSocket::ReceivePlcInterface()
{
	int nRecvSize = recv(m_SockConnectClient, m_pszRecvTemp, RECV_BUF_SIZE, MSG_PEEK);
	if ((nRecvSize <= 0))
		return;

	ZeroMemory(m_pszRecvTemp, RECV_BUF_SIZE);
	int nRecvSize2 = recv(m_SockConnectClient, m_pszRecvTemp, nRecvSize, 0);
	if ((nRecvSize2 <= 0))
	{
		return;
	}
	//printf("%s \r\n",m_pszRecvTemp);

	AddBuffer_PLCInterface(m_pszRecvTemp, nRecvSize2);
}

void	CClientSocket::ConnectToServer()
{
	if (TRUE == m_strServerIP.IsEmpty())
		return;

	if (m_nServerPort <= 0)
		return;

	CMgrComm* pMgr = CMgrComm::GetMgr();
	std::vector<SDevice*> vecValue;
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (!pMgr)
		return;
	if (m_nPrimaryKey != 1200)
	{
		m_SockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		memset(&m_AddrClient, 0, sizeof(m_AddrClient));

		m_AddrClient.sin_family = AF_INET;
		m_AddrClient.sin_addr.s_addr = inet_addr(m_strServerIP);  // 서버의 주소
		m_AddrClient.sin_port = htons((u_short)m_nServerPort);


		pMgr->GetDeivceFromType(eDeviceTypeIFBoard, vecValue);

		auto iter = vecValue.begin();
		auto iterE = vecValue.end();

		while (iter != iterE)
		{
			SDevice* lpDevice = (*iter);

			if (lpDevice)
			{
				if (lpDevice->nPrimaryKey == m_nPrimaryKey)
				{
					switch (lpDevice->nIndex % 6)
					{
					case 0: WSAAsyncSelect(m_SockClient, pWnd->GetSafeHwnd(), WM_SOCKET_EVENT_CH1, FD_CONNECT | FD_CLOSE); break;
					case 1: WSAAsyncSelect(m_SockClient, pWnd->GetSafeHwnd(), WM_SOCKET_EVENT_CH2, FD_CONNECT | FD_CLOSE); break;
					case 2: WSAAsyncSelect(m_SockClient, pWnd->GetSafeHwnd(), WM_SOCKET_EVENT_CH3, FD_CONNECT | FD_CLOSE); break;
					case 3: WSAAsyncSelect(m_SockClient, pWnd->GetSafeHwnd(), WM_SOCKET_EVENT_CH4, FD_CONNECT | FD_CLOSE); break;
					case 4: WSAAsyncSelect(m_SockClient, pWnd->GetSafeHwnd(), WM_SOCKET_EVENT_CH5, FD_CONNECT | FD_CLOSE); break;
					case 5: WSAAsyncSelect(m_SockClient, pWnd->GetSafeHwnd(), WM_SOCKET_EVENT_CH6, FD_CONNECT | FD_CLOSE); break;
					}

					m_bLoop = TRUE;

					break;
				}
			}

			++iter;
		}
		if (SOCKET_ERROR == connect(m_SockClient, (struct sockaddr*)&m_AddrClient, sizeof(m_AddrClient)))
		{
			int nError = WSAGetLastError();

			if (nError != WSAEWOULDBLOCK)
			{
				Add(m_nPrimaryKey, false);
				//SetConnect(FALSE);
				return;
			}
		}
	}
	else
	{
		m_SockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		memset(&m_AddrClient, 0, sizeof(m_AddrClient));

		m_AddrClient.sin_family = AF_INET;
		m_AddrClient.sin_addr.s_addr = inet_addr(m_strServerIP);  // 서버의 주소
		m_AddrClient.sin_port = htons((u_short)m_nServerPort);

		pMgr->GetDeivceFromType(eDeviceTypePlcInterface, vecValue);

		auto iter = vecValue.begin();
		auto iterE = vecValue.end();

		while (iter != iterE)
		{
			SDevice* lpDevice = (*iter);

			if (lpDevice)
			{
				if (lpDevice->nPrimaryKey == m_nPrimaryKey)
				{
					//WSAAsyncSelect(m_SockClient, pWnd->GetSafeHwnd(), WM_SOCKET_EVENT_PLC_INTERFACE, FD_CONNECT | FD_CLOSE);

					
					m_bLoop = TRUE;
					break;
				}
			}

			++iter;
		}

		if (bind(m_SockClient, (sockaddr*)&m_AddrClient, sizeof(m_AddrClient)) == SOCKET_ERROR)
		{
			return;
		}
		if (listen(m_SockClient, SOMAXCONN) == SOCKET_ERROR)
		{
			return;
		}

		int nClientSize = sizeof(m_AddrClient);
		struct timeval timeout = {};
		fd_set reads = {};

		FD_ZERO(&reads);
		FD_SET(m_SockClient, &reads);

		timeout.tv_sec = 5;
		int ret = select(m_SockClient + 1, &reads, 0, 0, &timeout);
		if (ret < 0)
		{
			Add(m_nPrimaryKey, false);
			AfxMessageBox("PLC Interface Connect Fail");
			return;
		}
		else if (ret == 0)
		{
			Add(m_nPrimaryKey, false);
			AfxMessageBox("PLC Interface Connect Fail");
			return;
		}
		m_SockConnectClient = accept(m_SockClient, (sockaddr*)&m_AddrClient, &nClientSize);
		/*if (SOCKET_ERROR == connect(m_SockClient, (struct sockaddr*)&m_AddrClient, sizeof(m_AddrClient)))
		{
			int nError = WSAGetLastError();

			SYSLOG(Dbg, "Socket PrimaryKey(%d), WSAGetLastError(%d)", m_nPrimaryKey, nError);

			if (nError != WSAEWOULDBLOCK)
			{
				SetConnect_PLCINTERFACE(FALSE);

				return;
			}
		}*/
		Add(m_nPrimaryKey, TRUE);
		SetConnect_PLCINTERFACE(TRUE);

	}	

	if (m_bLoop)
	{
		StartThread();
	}
}

void	CClientSocket::CloseClientSocket()
{
	closesocket(m_SockClient);
	closesocket(m_SockConnectClient);
	WSACleanup();
	SetConnect(FALSE);
	SetConnect_PLCINTERFACE(FALSE);
}

int		CClientSocket::SendToServer(CString strMsg) 
{
	if (FALSE == m_bConnect)
		return SEND_RET_FAIL;

	int nRetSend = send(m_SockClient, strMsg.GetBuffer(0), strMsg.GetLength(), 0);

	return nRetSend;
}


int		CClientSocket::SendToServer(char* pszMsg, int nSize)
{
	//if (FALSE == m_bConnect)
	if(FALSE == GetConnect(m_nPrimaryKey))
		return SEND_RET_FAIL;

	int nRetSend = send(m_SockClient, pszMsg, nSize, 0);

	return nRetSend;
}

int CClientSocket::SendToServer_PLCINTERFACE(char * pszMsg, int nSize)
{
	if (FALSE == m_bConnect_PLCInterface)
		return SEND_RET_FAIL;

	int nRetSend = send(m_SockConnectClient, pszMsg, nSize, 0);

	return nRetSend;
}

void	CClientSocket::AddBuffer(char* pszRecv, int nRecvSize)
{
	m_csLock.Lock();

	// 이전 데이터와 새로 들어온 데이터의 합이 MAX보다 크면 이전 데이터 지우자
	if (m_nRecvSize + nRecvSize > MAX_BUF_SIZE)
	{					
		//SYSLOG(Dbg, "[CClientSocket::AddBuffer] Over Buffer");
		return;
	}

	memcpy(m_pszRecv + m_nRecvSize, pszRecv, nRecvSize);
	m_nRecvSize += nRecvSize;

	CCommEthernet* pCommEthernet = NULL;
	int nFlameSize = 0;
	while (m_nRecvSize > 0)
	{
		if (FindSTX(m_nSearchIndex) == -1) //STX를 못찾으면 버퍼를 모두 지우자.
		{
			if (m_nSearchIndex >= m_nRecvSize)
			{
				ZeroMemory(m_pszRecv, MAX_BUF_SIZE);
				m_nSearchIndex = 0;
				m_nRecvSize = 0;
				break;
			}
		}

		if (m_nSearchIndex + 4 > m_nRecvSize) // STX를 찾았으나 FrameSize를 못 구하면 
			break;

		nFlameSize = m_pszRecv[m_nSearchIndex + 2] << 8 | m_pszRecv[m_nSearchIndex + 3] & 0xFF;

		if (nFlameSize > 4000)
		{
			SYSLOG(Dbg, "[CClientSocket::AddBuffer] nFlameSize is greater than 4000");
			m_nSearchIndex += 1;
			continue;
		}
		else if (nFlameSize < 6)
		{
			SYSLOG(Dbg, "[CClientSocket::AddBuffer] nFlameSize is less than 6");
			m_nSearchIndex += 1;
			continue;
		}
		/*	else if (m_pszRecv[m_nSearchIndex + nFlameSize] != '@' || m_pszRecv[m_nSearchIndex + nFlameSize + 1] != '!') @A는 연속으로 올 때 STX STX ETX로 날라옴
			{
				SYSLOG(Dbg, "[CClientSocket::AddBuffer] ETX 찾지 못함");
				m_nSearchIndex += nFlameSize;
				continue;
			}*/
		else if (nFlameSize > m_nRecvSize)
		{
			return;
		}
		else if (m_nSearchIndex + nFlameSize > m_nRecvSize)
			break;

		int nBoardChannelNumber = m_pszRecv[m_nSearchIndex + 4] << 8 | m_pszRecv[m_nSearchIndex + 5] & 0xFF;

		switch (nBoardChannelNumber)
		{
		case 1: nBoardChannelNumber = 0; break;
		case 2: nBoardChannelNumber = 1; break;
		case 4: nBoardChannelNumber = 2; break;
		case 8: nBoardChannelNumber = 3; break;
		default:
		{
			//SYSLOG(Dbg, "[CClientSocket::AddBuffer] Board Index Error - %d", nBoardChannelNumber);
			break;
		}
		}

		pCommEthernet = CMgrEthernet::GetMgr()->GetAt(m_nPrimaryKey);
		if (pCommEthernet == NULL)
		{
			SYSLOG(Dbg, "[CClientSocket::AddBuffer] pCommEthernet is NULL");
			CWnd* pWnd = AfxGetApp()->GetMainWnd();
			if (pWnd)
				pWnd->PostMessage(WM_COMMAND, ID_MENU_MAIN_CMD_PAUSE_1_KOR, 0);
		}
		else if (pCommEthernet->IsQueueRun() == false)
		{
			SYSLOG(Dbg, "[CClientSocket::AddBuffer] pCommEthernet Queue is Stopped");
			CWnd* pWnd = AfxGetApp()->GetMainWnd();
			if (pWnd)
				pWnd->PostMessage(WM_COMMAND, ID_MENU_MAIN_CMD_PAUSE_1_KOR, 0);
		}
		else
		{
			CMgrEthernet::GetMgr()->AddQueue(m_nPrimaryKey, CPnxUtil::GetMgr()->GetAllDate(), (BYTE*)(m_pszRecv + m_nSearchIndex), nFlameSize);
		}

		if (GetChannelArray().GetSize() <= nBoardChannelNumber)
		{
			SYSLOG(Dbg, "[CClientSocket::AddBuffer] Chamber Channel matching error - BoardNo: %d", nBoardChannelNumber);
			return;
		}
		if (m_nSearchIndex + nFlameSize > m_nRecvSize)
			int a = 0;

		m_nSearchIndex += nFlameSize;


	}

	int nCopySize = m_nRecvSize - m_nSearchIndex;
	if (nCopySize == 0)
	{
		ZeroMemory(m_pszRecv, MAX_BUF_SIZE);
		m_nSearchIndex = 0;
		m_nRecvSize = 0;
	}
	else if (nCopySize < 0)
	{
		//SYSLOG(Dbg, "[CClientSocket::AddBuffer] nCopySize가 0보다 작다.");
		m_nSearchIndex = 0;
		return;
	}
	else
	{
		memcpy(m_pszRecvSwap, m_pszRecv + m_nSearchIndex, nCopySize);
		ZeroMemory(m_pszRecv, MAX_BUF_SIZE);
		memcpy(m_pszRecv, m_pszRecvSwap, nCopySize);
		m_nSearchIndex = 0;
		m_nRecvSize = nCopySize;
	}
	m_csLock.Unlock();
}

void CClientSocket::AddBuffer_PLCInterface(char * pszRecv, int nRecvSize)
{
	m_csLock_PLCInterface.Lock();

	// 이전 데이터와 새로 들어온 데이터의 합이 MAX보다 크면 이전 데이터 지우자
	if (m_nRecvSize_PLCInterface + nRecvSize > MAX_BUF_SIZE)
	{
		//SYSLOG(Dbg, "[CClientSocket::AddBuffer] Over Buffer");
		return;
	}

	memcpy(m_pszRecv_PLCInterface + m_nRecvSize_PLCInterface, pszRecv, nRecvSize);
	m_nRecvSize_PLCInterface += nRecvSize;

	CCommEthernet* pCommEthernet = NULL;
	int nFlameSize = 0;
	while (m_nRecvSize_PLCInterface > 0)
	{
		if (FindSTX_PLCInterface(m_nSearchIndex) == -1) //STX를 못찾으면 버퍼를 모두 지우자.
		{
			if (m_nSearchIndex >= m_nRecvSize_PLCInterface)
			{
				ZeroMemory(m_pszRecv_PLCInterface, MAX_BUF_SIZE);
				m_nSearchIndex = 0;
				m_nRecvSize_PLCInterface = 0;
				break;
			}
		}

		//if (m_nSearchIndex + 4 > m_nRecvSize_PLCInterface) // STX를 찾았으나 FrameSize를 못 구하면 
		//	break;

		//nFlameSize = m_pszRecv_PLCInterface[m_nSearchIndex + 2] << 8 | m_pszRecv_PLCInterface[m_nSearchIndex + 3] & 0xFF;

		//if (nFlameSize > 4000)
		//{
		//	SYSLOG(Dbg, "[CClientSocket::AddBuffer] nFlameSize is greater than 4000");
		//	m_nSearchIndex += 1;
		//	continue;
		//}
		//else if (nFlameSize < 6)
		//{
		//	SYSLOG(Dbg, "[CClientSocket::AddBuffer] nFlameSize is less than 6");
		//	m_nSearchIndex += 1;
		//	continue;
		//}
		/*	else if (m_pszRecv[m_nSearchIndex + nFlameSize] != '@' || m_pszRecv[m_nSearchIndex + nFlameSize + 1] != '!') @A는 연속으로 올 때 STX STX ETX로 날라옴
			{
				SYSLOG(Dbg, "[CClientSocket::AddBuffer] ETX 찾지 못함");
				m_nSearchIndex += nFlameSize;
				continue;
			}*/
	/*	else if (nFlameSize > m_nRecvSize_PLCInterface)
		{
			return;
		}
		else if (m_nSearchIndex + nFlameSize > m_nRecvSize_PLCInterface)
			break;	*/

		pCommEthernet = CMgrEthernet::GetMgr()->GetAt(m_nPrimaryKey);
		if (pCommEthernet == NULL)
		{
			SYSLOG(Dbg, "[CClientSocket::AddBuffer] pCommEthernet is NULL");
			CWnd* pWnd = AfxGetApp()->GetMainWnd();
			if (pWnd)
				pWnd->PostMessage(WM_COMMAND, ID_MENU_MAIN_CMD_PAUSE_1_KOR, 0);
		}
		else if (pCommEthernet->IsQueueRun() == false)
		{
			SYSLOG(Dbg, "[CClientSocket::AddBuffer] pCommEthernet Queue is Stopped");
			CWnd* pWnd = AfxGetApp()->GetMainWnd();
			if (pWnd)
				pWnd->PostMessage(WM_COMMAND, ID_MENU_MAIN_CMD_PAUSE_1_KOR, 0);
		}
		else
		{
			nFlameSize = 50;
			CMgrEthernet::GetMgr()->AddQueue(m_nPrimaryKey, CPnxUtil::GetMgr()->GetAllDate(), (BYTE*)(m_pszRecv_PLCInterface + m_nSearchIndex), nFlameSize);
		}

		if (m_nSearchIndex + nFlameSize > m_nRecvSize_PLCInterface)
			int a = 0;

		m_nSearchIndex += nFlameSize;


	}

	int nCopySize = m_nRecvSize_PLCInterface - m_nSearchIndex;
	if (nCopySize == 0)
	{
		ZeroMemory(m_pszRecv_PLCInterface, MAX_BUF_SIZE);
		m_nSearchIndex = 0;
		m_nRecvSize_PLCInterface = 0;
	}
	else if (nCopySize < 0)
	{
		//SYSLOG(Dbg, "[CClientSocket::AddBuffer] nCopySize가 0보다 작다.");
		m_nSearchIndex = 0;
		return;
	}
	else
	{
		memcpy(m_pszRecvSwap_PLCInterface, m_pszRecv_PLCInterface + m_nSearchIndex, nCopySize);
		ZeroMemory(m_pszRecv_PLCInterface, MAX_BUF_SIZE);
		memcpy(m_pszRecv_PLCInterface, m_pszRecvSwap_PLCInterface, nCopySize);
		m_nSearchIndex = 0;
		m_nRecvSize_PLCInterface = nCopySize;
	}
	m_csLock_PLCInterface.Unlock();
}

int CClientSocket::FindSTX(int nIndex)
{
	for (; nIndex < m_nRecvSize-1; nIndex++)
	{
		if (m_pszRecv[nIndex] == '@' && (m_pszRecv[nIndex + 1] == 'D' || m_pszRecv[nIndex + 1] == 'A' || m_pszRecv[nIndex + 1] == 'E' ||
			m_pszRecv[nIndex + 1] == 'S' || m_pszRecv[nIndex + 1] == 'J' || m_pszRecv[nIndex + 1] == 'P' || m_pszRecv[nIndex + 1] == 'C' || m_pszRecv[nIndex + 1] == 'T'))
		{
			m_nSearchIndex = nIndex;
			//SYSLOG(Dbg, "index%d", nIndex);
		/*	if (m_nSearchIndex >= m_nRecvSize)
				int a = 0;*/
			return nIndex;
		}
	}

	return -1;
}

int CClientSocket::FindSTX_PLCInterface(int nIndex)
{
	for (; nIndex < m_nRecvSize_PLCInterface - 1; nIndex++)
	{
		if (m_pszRecv_PLCInterface[nIndex] == 0 && (m_pszRecv_PLCInterface[nIndex + 1] == 2))
		{
			m_nSearchIndex = nIndex;
			//SYSLOG(Dbg, "index%d", nIndex);
		/*	if (m_nSearchIndex >= m_nRecvSize)
				int a = 0;*/
			return nIndex;
		}
	}

	return -1;
}

void	CClientSocket::StartLinkThread()
{
	m_bThreadLink = TRUE;
	if (!m_pLinkThread)
	{
//modfiy by sjh 2019.09.19 memory rick start
		m_pLinkThread = AfxBeginThread(LinkThread, this);
//modfiy by sjh 2019.09.19 memory rick end
	}
		
}
void	CClientSocket::StopLinkThread()
{
	m_bThreadLink = FALSE;
	if (m_pLinkThread)
	{
		::WaitForSingleObject(m_pLinkThread->m_hThread, INFINITE);
		m_pLinkThread = NULL;
	}

}

UINT    CClientSocket::LinkThread(LPVOID pParam)
{
//	CClientSocket* pMgrSock = (CClientSocket*)pParam;

#if 0
	while (pMgrSock->m_bThreadLink)
	{
		CMgrChannel* pMgr = CMgrChannel::GetMgr();

		if (pMgr)
		{
			CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

			auto Pos = listCyclerChannel.GetHeadPosition();

			while (Pos)
			{
				CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

				if(!lpCyclerChannel)
					continue;

				CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();

				if (!pMgrChamber)
					continue;

				SChamber* lpDeviceChamber = pMgrChamber->GetDeivceCyclerChannel(lpCyclerChannel->GetChannelNumber());

				if(!lpDeviceChamber)
					continue;

				if (abs(lpDeviceChamber->fCurrentValue) < 0 || false == lpCyclerChannel->GetSendDataPacket())
				{
					lpCyclerChannel->SendTemp();
				}
			}
		}

		Sleep(1000);
	}

#endif
	return 0;
}
