#pragma once

#include "Singleton.h"
#include "SerialCom.h"
class CChamberProc : public CSingleton<CChamberProc>
{
public:
	CChamberProc();
	~CChamberProc();

	int		m_fnInit();		//�ʱ�ȭ 
	int		m_fnDeInit();

	BOOL	m_fnPortOpen(int nCh, int nPortNo, int nBaurd);	//����!
	BOOL	m_fnPoerClose(int nCh);		//�ݱ�!

	CSerialCom*	GetAt(int nIndex);	//�ø��� ������
	CSerialCom*	GetAtComm(int nChannelNumber);
	int			GetSize();

	CList< CSerialCom*, CSerialCom* >&	GetComList();

	void	LoadIni();		//Port���� �� Open����

	void	m_fnStartThread();
	void	m_fnStopThread();

private:

	CList<CSerialCom*, CSerialCom*> m_listCom;
	int			m_nPortNo;
	int			m_nBaurdrate;
	BOOL		m_bLoop;

};

