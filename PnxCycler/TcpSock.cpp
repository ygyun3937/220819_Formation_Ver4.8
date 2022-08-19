#include "stdafx.h"
#include "TcpSock.h"
#include "TcpManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTcpSock::CTcpSock(CWnd* pParent, long nClientNum)
{
	
	m_TcpClientNum = nClientNum;
	
	m_pParent = pParent;
	m_TcpInfo = NULL;
}

CTcpSock::~CTcpSock()
{
}

#if 0
BEGIN_MESSAGE_MAP(CTcpSock, CAsyncSocket)
	//{{AFX_MSG_MAP(CTcpSock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

void CTcpSock::OnAccept(int nErrorCode) 
{
	LONG m_lParam = m_TcpClientNum;
	::SendMessage(m_pParent->m_hWnd,WM_TCP_SOCKET_MSG, MAKEWPARAM(EEventAccept,CCodeNone),LPARAM(m_lParam));

	CAsyncSocket::OnAccept(nErrorCode);
}

void CTcpSock::OnReceive(int nErrorCode) 
{
	LONG m_lParam = m_TcpClientNum;
	::SendMessage(m_pParent->m_hWnd,WM_TCP_SOCKET_MSG, MAKEWPARAM(EEventReceive,CCodeNone),LPARAM(m_lParam));

	CAsyncSocket::OnReceive(nErrorCode);
}

void CTcpSock::InitSock(TcpInfo* tcp_info)
{
	m_TcpInfo = tcp_info;
	Connect(m_TcpInfo->strIP, m_TcpInfo->nPort);
}

void CTcpSock::OnClose(int nErrorCode) 
{
	LONG m_lParam = m_TcpClientNum;
	::SendMessage(m_pParent->m_hWnd,WM_TCP_SOCKET_MSG, MAKEWPARAM(EEventClose,CCodeNone),LPARAM(m_lParam));

	CAsyncSocket::OnClose(nErrorCode);
}

void CTcpSock::OnConnect(int nErrorCode) 
{
	LONG m_lParam = m_TcpClientNum;
	if(!nErrorCode)
	{
		::SendMessage(m_pParent->m_hWnd,WM_TCP_SOCKET_MSG, MAKEWPARAM(EEventConnect,CCodeSucess),LPARAM(m_lParam));	
	}
	else
	{
		::SendMessage(m_pParent->m_hWnd,WM_TCP_SOCKET_MSG, MAKEWPARAM(EEventConnect,CCodeFail),LPARAM(m_lParam));	
	}
	CAsyncSocket::OnConnect(nErrorCode);
}

