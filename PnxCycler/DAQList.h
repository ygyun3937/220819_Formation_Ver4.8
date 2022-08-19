#pragma once

#include "DAQ.h"

class CDAQList
{
public:
	CDAQList();
	virtual ~CDAQList();

	void	Add(CDAQ* pNewElement);

	CList< CDAQ*, CDAQ* >&	GetDAQList();
private:
	CList< CDAQ*, CDAQ* >	m_listDAQ;
private:
	void	RemoveAll();
};

