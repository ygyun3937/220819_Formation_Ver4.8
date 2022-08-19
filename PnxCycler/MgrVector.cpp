#include "stdafx.h"
#include "MgrVector.h"

#include "MgrComm.h"
#include "MgrChannel.h"

CMgrVector::CMgrVector()
{
	if (CommTypeCan == CMgrComm::GetMgr()->GetCommType(CommObjVector) || CommTypeLin == CMgrComm::GetMgr()->GetCommType(CommObjVector))
	{
		CList< TComm*, TComm* >* pListComm = CMgrComm::GetMgr()->GetCommList(CommObjVector);

		if (pListComm)
		{
			auto Pos = pListComm->GetHeadPosition();

			while (Pos)
			{
				TComm* pComm = pListComm->GetNext(Pos);

				if (NULL == pComm)
					continue;

				CCommVector* pVector = new CCommVector;

				pVector->SetPrimaryKey(pComm->nPrimaryKey);
				pVector->SetChannelNumber(pComm->nChannelNumber);

				auto it = m_mapVector.find(pComm->nChannelNumber);

				if (it == m_mapVector.end())
				{
					m_mapVector.insert(std::pair<int, CCommVector*>(pComm->nChannelNumber, pVector));
				}
			}
		}
	}
}

CMgrVector::~CMgrVector()
{
	RemoveAll();
}

void	CMgrVector::StartThread()
{
	for (auto it = m_mapVector.begin(); it != m_mapVector.end(); ++it)
	{
		CCommVector* pComm = it->second;

		if (pComm)
			pComm->StartThread();
	}
}

void	CMgrVector::StopThread()
{
	for (auto it = m_mapVector.begin(); it != m_mapVector.end(); ++it)
	{
		CCommVector* pComm = it->second;

		if (pComm)
			pComm->StopThread();
	}
}

void	CMgrVector::AddQueue(int nCommType, int nChannelNumber, CString strTime, int nBmsID, int nByteOrder, int nStartBit, int nBitLength, float fFactor, float fOffset, TPCANMsg& msg)
{
	CCommVector* pVector = GetAt(nChannelNumber);

	if (pVector)
	{
		TVector* pComm = new TVector;

		pComm->nChannelNumber	= nChannelNumber;
		pComm->nBmsID			= nBmsID;
		pComm->strTime			= strTime;
		pComm->nByteOrder		= nByteOrder;
		pComm->nStartBit		= nStartBit;
		pComm->nBitLength		= nBitLength;
		pComm->fFactor			= fFactor;
		pComm->fOffset			= fOffset;

		if (VectorCommTypeCan == nCommType)
			memcpy(&pComm->stBit, &msg.DATA, sizeof(pComm->stBit));
		else if (VectorCommTypeLin == nCommType)
			memcpy(&pComm->stBit, &msg.DATA, sizeof(pComm->stBit));

		pVector->AddQueue(pComm);
	}
}

CCommVector*	CMgrVector::GetAt(int nChannelNumber)
{
	auto it = m_mapVector.find(nChannelNumber);

	if (it == m_mapVector.end())
		return NULL;

	return it->second;
}

void	CMgrVector::RemoveAll()
{
	for (auto it = m_mapVector.begin(); it != m_mapVector.end(); ++it)
	{
		delete it->second;
	}

	m_mapVector.clear();
}