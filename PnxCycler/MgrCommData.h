#pragma once

#include "Singleton.h"
#include "CommData.h"
#include <map>

// Warning : ���α׷� ���� �� �ݵ�� Queue �� �ִ� ���� ����� �����ؾ� ��..

class CMgrCommData : public CSingleton< CMgrCommData >
{
public:
	CMgrCommData();
	virtual ~CMgrCommData();

	void MappingDAQ();
	void	RemoveAllDAQ();

	CCommData*	GetAt(int nChannelNumber);	// ���� �̰͸� ȣ���ϸ� ��

	std::map< int, CCommData* >&	GetCommMap();
private:
	std::map< int, CCommData* >	m_mapCommData;
private:
	void	RemoveAll();

};

