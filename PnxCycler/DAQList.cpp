#include "stdafx.h"
#include "DaqList.h"


CDAQList::CDAQList()
{
}


CDAQList::~CDAQList()
{
	RemoveAll();
}

void	CDAQList::Add(CDAQ* pNewElement)
{
	m_listDAQ.AddTail(pNewElement);
}

CList< CDAQ*, CDAQ* >&	CDAQList::GetDAQList()
{
	return m_listDAQ;
}

void	CDAQList::RemoveAll()
{
	while (!m_listDAQ.IsEmpty())
		delete m_listDAQ.RemoveHead();
}
