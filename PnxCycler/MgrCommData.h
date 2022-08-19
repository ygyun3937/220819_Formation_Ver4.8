#pragma once

#include "Singleton.h"
#include "CommData.h"
#include <map>

// Warning : 프로그램 종료 시 반드시 Queue 에 있는 내용 지우고 삭제해야 함..

class CMgrCommData : public CSingleton< CMgrCommData >
{
public:
	CMgrCommData();
	virtual ~CMgrCommData();

	void MappingDAQ();
	void	RemoveAllDAQ();

	CCommData*	GetAt(int nChannelNumber);	// 나는 이것만 호출하면 됨

	std::map< int, CCommData* >&	GetCommMap();
private:
	std::map< int, CCommData* >	m_mapCommData;
private:
	void	RemoveAll();

};

