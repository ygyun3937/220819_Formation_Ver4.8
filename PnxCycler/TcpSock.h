#if !defined(AFX_TCPSOCK_H__399A3E02_328E_46AE_B15B_A70B088E158B__INCLUDED_)
#define AFX_TCPSOCK_H__399A3E02_328E_46AE_B15B_A70B088E158B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TcpInfo;
class CTcpSock : public CAsyncSocket
{
public:
	CWnd* m_pParent;
	long m_TcpClientNum;
	TcpInfo* m_TcpInfo;
	void InitSock(TcpInfo* tcp_info);

public:
	CTcpSock(CWnd* pParent = NULL, long nClientNum = -1);
	virtual ~CTcpSock();
		
typedef enum
	{
		EEventUnknown  	   = -1,			
		EEventNone  	   ,				
		EEventError 	   ,				
		EEventReceive 	   ,				
		EEventAccept 	   ,				
		EEventClose 	   ,
		EEventConnect 	   
	} 
	Event;
typedef enum
	{
		CCodeUnknown  	   = -1,			
		CCodeNone  		   = 0,				
		CCodeSucess		   ,
		CCodeFail		   
	} 
	Code;
	
public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
};

#endif // !defined(AFX_TCPSOCK_H__399A3E02_328E_46AE_B15B_A70B088E158B__INCLUDED_)
