#include "stdafx.h"
#include "MgrPlcInterface.h"

#include "PnxUtil.h"
#include "PlcInterface.h"

CPlcInterface::CPlcInterface()
{


	Initialize();
}

CPlcInterface::~CPlcInterface()
{
}

void CPlcInterface::Initialize()
{
	InitializeVariables();
}

void CPlcInterface::InitializeVariables()
{
	m_nPrimaryKey = 0;
	m_strIpAddress = "";
	m_nPortNo = 0;
	m_bInitialize = false;
}

bool CPlcInterface::Dispose()
{
	return true;
}

void CPlcInterface::AddQueue(stPlcData * pNewElement)
{
	m_csPush.Lock();

	if (m_Queue.GetCount() != 0 && m_Queue.GetCount() % 10 == 0) {
		//SYSLOG(Dbg, "CPlcInterface::GetQueue - %d", m_Queue.GetCount());
	}

	m_Queue.AddTail(pNewElement);

	m_csPush.Unlock();
}

stPlcData * CPlcInterface::GetQueue()
{
	m_csPush.Lock();

	stPlcData* pPlcData = NULL;

	if (FALSE == m_Queue.IsEmpty()) {
		pPlcData = m_Queue.RemoveHead();
	}

	m_csPush.Unlock();

	return pPlcData;
}
