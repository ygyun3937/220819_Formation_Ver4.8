#pragma once

#include "Singleton.h"
#include "CommEthernet.h"
#include <map>

class CMgrEthernet : public CSingleton< CMgrEthernet >
{
public:
	CMgrEthernet();
	virtual ~CMgrEthernet();

	void	StartThread();
	void	StopThread();

	void	AddQueue(int nPrimaryKey, CString strTime, BYTE* pszData, int nSize);
	void	AddQueue(int nPrimaryKey, CString strTime, CString strData, int nSize);

	CCommEthernet*	GetAt(int nPrimaryKey);
private:
	std::map< int, CCommEthernet* > m_mapEthernet;
private:
	void	RemoveAll();
};

