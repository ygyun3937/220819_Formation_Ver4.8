#include "stdafx.h"
#include "CMgrPid.h"

#include <list>
#include "MgrComm.h"


CMgrPid::CMgrPid()
{
	Initialize();
}

CMgrPid::~CMgrPid()
{
	Dispose();
}

// -------------------------------------------------------------------------------------------------

void CMgrPid::Initialize()
{
	InitializeVariables();

	InitializeDevices();

	OpenDevices();

	StartThread();
}

void CMgrPid::InitializeVariables()
{
}

void CMgrPid::InitializeDevices()
{
	CMgrComm* pMgr = CMgrComm::GetMgr();
	if (!pMgr) {
		return;
	}
	
	std::vector<SDevice*> vecValue;
	pMgr->GetDeivceFromType(eDeviceTypePid, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			CCommPid* pCommPid = new CCommPid;

			pCommPid->SetCommType(eDeviceTypePid);
			pCommPid->SetDeviceType(lpDevice->strDeviceType);
			pCommPid->SetDeviceIndex(lpDevice->nIndex);
			pCommPid->SetPrimaryKey(lpDevice->nPrimaryKey);
			pCommPid->SetChannelArray(lpDevice->arrChannel);
			pCommPid->SetIpAddress(lpDevice->strIP);
			pCommPid->SetPortNo(lpDevice->nPort);

			//if (pCommPid->OpenDevice())
			//{
			//}

			m_listCom.AddTail(pCommPid);
			//m_mapCom.insert(std::pair< int, CCommPid* >(lpDevice->nPrimaryKey, pCommPid));
		}

		++iter;
	}
}

void CMgrPid::Dispose()
{
	StopThread();

	CloseDevices();

}

// -------------------------------------------------------------------------------------------------

void CMgrPid::AddQueue(int nPrimaryKey, CString strTime, CString strData, int nSize)
{
	CCommPid* pCommPid = GetDeviceMapAt(nPrimaryKey);

	if (pCommPid)
	{
		TPid* pComm = new TPid;

		pComm->nDeviceNo = pCommPid->GetDeviceIndex();
		pComm->strTime = strTime;
		pComm->strData = strData;
		pComm->nSize = nSize;

		pCommPid->AddQueue(pComm);
	}
}

CCommPid * CMgrPid::GetDeviceMapAt(int nPrimaryKey)
{
	auto it = m_mapCom.find(nPrimaryKey);

	if (it == m_mapCom.end()) {
		return NULL;
	}
	
	return it->second;
}

// -------------------------------------------------------------------------------------------------

void CMgrPid::StartThread()
{
	StartThreadListType();
	//StartThreatMapType();
}

void CMgrPid::StartThreadListType()
{
	auto Pos = m_listCom.GetHeadPosition();
	while (Pos)
	{
		CCommPid* pCommPid = m_listCom.GetNext(Pos);
		if (pCommPid) {
			pCommPid->StartQueueThread();
		}
	}
}

void CMgrPid::StartThreatMapType()
{
	for (auto it = m_mapCom.begin(); it != m_mapCom.end(); ++it)
	{
		CCommPid* pCommPid = it->second;
		if (pCommPid) {
			pCommPid->StartQueueThread();
		}
	}
}

void CMgrPid::StopThread()
{
	StopThreadListType();
	// StopThreadMapType();
}

void CMgrPid::StopThreadListType()
{
	auto Pos = m_listCom.GetHeadPosition();
	while (Pos)
	{
		CCommPid* pCommPid = m_listCom.GetNext(Pos);
		if (pCommPid) {
			pCommPid->StopQueueThread();
		}
	}
}

void CMgrPid::StopThreadMapType()
{
	for (auto it = m_mapCom.begin(); it != m_mapCom.end(); ++it)
	{
		CCommPid* pCommPid = it->second;
		if (pCommPid) {
			pCommPid->StopQueueThread();
		}
	}
}

// -------------------------------------------------------------------------------------------------

void CMgrPid::OpenDevices()
{
	auto Pos = m_listCom.GetHeadPosition();
	while (Pos)
	{
		CCommPid* pCommPid = m_listCom.GetNext(Pos);
		if (pCommPid) {
			if (pCommPid->OpenDevice() == 0) {
				pCommPid->ConnectDevice();
			}

			Sleep(20);
		}
	}
}

void CMgrPid::CloseDevices()
{
	auto Pos = m_listCom.GetHeadPosition();
	while (Pos)
	{
		CCommPid* pCommPid = m_listCom.GetNext(Pos);
		if (pCommPid) {
			pCommPid->CloseDevice();
			Sleep(20);
		}
	}
}

// -----------------------------------------------------------------------------

CCommPid* CMgrPid::GetDevice(int nSourceChannlNo)
{
	POSITION position = m_listCom.GetHeadPosition();
	while (position)
	{
		CCommPid* pCommPid = m_listCom.GetNext(position);
		if (NULL == pCommPid) {
			continue;
		}
		
		for (int nChannelCount = 0; nChannelCount < pCommPid->GetChannelArray().GetSize(); nChannelCount++)
		{
			CString tempValue = pCommPid->GetChannelArray().GetAt(nChannelCount);
			if (nSourceChannlNo == atoi(tempValue)) {
				return pCommPid;
			}
		}
	}

	return NULL;
}




