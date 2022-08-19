#include "stdafx.h"
#include "PnxCycler.h"
#include "TcpManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TcpInfo::TcpInfo()
{
	sock = NULL;
	bConnect = FALSE;
	strIP = "127.0.0.1";
	nHandle = 0;
	nPort = 0;
	AfxBeginThread(ThreadTcpSend, this);
}

TcpInfo::~TcpInfo()
{
}

UINT TcpInfo::ThreadTcpSend(LPVOID lParam)
{
	TcpInfo* pParent = (TcpInfo*)lParam;
	CString strSndCmd;

#pragma pack(push, 1)

	while (TRUE)
	{
		Sleep(1);

		if (!pParent->bConnect){
			Sleep(100);
			continue;
		}

		if (pParent->m_pSendList.IsEmpty()){
			Sleep(1);
			continue;
		}

		if (pParent->sock!= NULL){
			strSndCmd = pParent->m_pSendList.Pop();
			pParent->sock->Send((unsigned char*)strSndCmd.GetBuffer(0), strSndCmd.GetLength());
		}
	}
#pragma pack(pop)

	return 0;
}

CTcpManager::CTcpManager()
{
}

CTcpManager::~CTcpManager()
{
}

BEGIN_MESSAGE_MAP(CTcpManager, CWnd)
END_MESSAGE_MAP()