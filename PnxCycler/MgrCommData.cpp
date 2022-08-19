#include "stdafx.h"
#include "MgrCommData.h"

#include "MgrChannel.h"
#include "MgrConfig.h"

#include "MgrDAQ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMgrCommData::CMgrCommData()
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	CList< CChannel*, CChannel* >& listChannel = pMgr->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		auto it = m_mapCommData.find(pChannel->GetChannelNumber());
		
		if (it == m_mapCommData.end())
		{
			CCommData* pComm = new CCommData;		
			pComm->SetChannelNumber(pChannel->GetChannelNumber());

			m_mapCommData.insert(std::pair<int, CCommData*>(pChannel->GetChannelNumber(), pComm));
		}
	}
}

CMgrCommData::~CMgrCommData()
{
	RemoveAll();
}

CCommData*	CMgrCommData::GetAt(int nChannelNumber)
{
	auto it = m_mapCommData.find(nChannelNumber);

	if (it == m_mapCommData.end())
		return NULL;

	return it->second;
}

std::map< int, CCommData* >&	CMgrCommData::GetCommMap()
{
	return m_mapCommData;
}

void	CMgrCommData::RemoveAll()
{
	for (auto it = m_mapCommData.begin(); it != m_mapCommData.end(); ++it)
	{
		delete it->second;
	}

	m_mapCommData.clear();
}
void	CMgrCommData::MappingDAQ()
{
	// 1. DAQ ������ ȹ���Ѵ�.

	// 2. DAQ �� ����� Balance ������ �ڵ� Ȥ�� �������� ȹ�� �ȴ�.

	// 3. Channel �� DAQ ������ �ִ´�.

	CList< CDAQ*, CDAQ* >& listDAQ = CMgrDAQ::GetMgr()->GetDAQList();

	auto Pos = listDAQ.GetHeadPosition();

	while (Pos)
	{
		CDAQ* pDAQ = listDAQ.GetNext(Pos);

		if (NULL == pDAQ)
			continue;

		CCommData* pComm = GetAt(pDAQ->GetChannelNumber());

		if (NULL == pComm)
			continue;

		if (pDAQ->GetDaqUse() == TRUE)
		{
			pComm->AddDAQ(pDAQ);
		}
		else
		{
			continue;
		}
	}
}

void	CMgrCommData::RemoveAllDAQ()
{
	for (auto it = m_mapCommData.begin(); it != m_mapCommData.end(); ++it)
	{
		it->second->RemoveAllDAQ();
	}
}
