#pragma once

#include "Singleton.h"
#include "SerialCom.h"
class CChamberProc : public CSingleton<CChamberProc>
{
public:
	CChamberProc();
	~CChamberProc();

	int		m_fnInit();		//초기화 
	int		m_fnDeInit();

	BOOL	m_fnPortOpen(int nCh, int nPortNo, int nBaurd);	//열기!
	BOOL	m_fnPoerClose(int nCh);		//닫기!

	CSerialCom*	GetAt(int nIndex);	//시리얼 얻어오기
	CSerialCom*	GetAtComm(int nChannelNumber);
	int			GetSize();

	CList< CSerialCom*, CSerialCom* >&	GetComList();

	void	LoadIni();		//Port정보 얻어서 Open까지

	void	m_fnStartThread();
	void	m_fnStopThread();

private:

	CList<CSerialCom*, CSerialCom*> m_listCom;
	int			m_nPortNo;
	int			m_nBaurdrate;
	BOOL		m_bLoop;

};

