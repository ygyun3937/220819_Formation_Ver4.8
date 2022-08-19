#include "stdafx.h"
#include "PlcInterface.h"
#include <list>
#include "MgrComm.h"
#include "MgrPlcInterface.h"


CMgrPlcInterface::CMgrPlcInterface()
{
	Initialize();
}

CMgrPlcInterface::~CMgrPlcInterface()
{
	Dispose();
}

void CMgrPlcInterface::Initialize()
{
	InitializeVariables();

	InitializeDevices();

	//OpenDevices();

	//StartThread();
}

void CMgrPlcInterface::InitializeVariables()
{
	m_fVacuumValue = 0.0;
	m_fPumpOpenPercent = 0.0;
}

void CMgrPlcInterface::InitializeDevices()
{
	CMgrComm* pMgr = CMgrComm::GetMgr();
	if (!pMgr) {
		return;
	}

	std::vector<SDevice*> vecValue;
	pMgr->GetDeivceFromType(eDeviceTypePlcInterface, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			CPlcInterface* pCommPlcInterface = new CPlcInterface;

			pCommPlcInterface->SetCommType(eDeviceTypePlcInterface);
			pCommPlcInterface->SetDeviceType(lpDevice->strDeviceType);
			pCommPlcInterface->SetDeviceIndex(lpDevice->nIndex);
			pCommPlcInterface->SetPrimaryKey(lpDevice->nPrimaryKey);
			pCommPlcInterface->SetChannelArray(lpDevice->arrChannel);
			pCommPlcInterface->SetIpAddress(lpDevice->strIP);
			pCommPlcInterface->SetPortNo(lpDevice->nPort);

			//if (pCommPid->OpenDevice())
			//{
			//}

			m_listPlcInterface.AddTail(pCommPlcInterface);
			//m_mapCom.insert(std::pair< int, CCommPid* >(lpDevice->nPrimaryKey, pCommPid));
		}

		++iter;
	}
}

void CMgrPlcInterface::Dispose()
{
	//StopThread();

	//CloseDevices();
}

void CMgrPlcInterface::AddQueue(int nPrimaryKey, CString strTime, CString strData, int nSize)
{
	CPlcInterface* pCommPlcInterface = GetDeviceMapAt(nPrimaryKey);

	if (pCommPlcInterface)
	{
		stPlcData* pComm = new stPlcData;

		pComm->nDeviceNo = pCommPlcInterface->GetDeviceIndex();
		pComm->strTime = strTime;
		pComm->strData = strData;
		pComm->nSize = nSize;

		pCommPlcInterface->AddQueue(pComm);
	}
}

CPlcInterface * CMgrPlcInterface::GetDeviceMapAt(int nPrimaryKey)
{
	auto it = m_mapPlcInterface.find(nPrimaryKey);

	if (it == m_mapPlcInterface.end()) {
		return NULL;
	}

	return it->second;
}

CPlcInterface * CMgrPlcInterface::GetDevice(int nSourceChannlNo)
{
	POSITION position = m_listPlcInterface.GetHeadPosition();
	while (position)
	{
		CPlcInterface* pPlcInterface = m_listPlcInterface.GetNext(position);
		if (NULL == pPlcInterface) {
			continue;
		}

		for (int nChannelCount = 0; nChannelCount < pPlcInterface->GetChannelArray().GetSize(); nChannelCount++)
		{
			CString tempValue = pPlcInterface->GetChannelArray().GetAt(nChannelCount);
			if (nSourceChannlNo == atoi(tempValue)) {
				return pPlcInterface;
			}
		}
	}

	return NULL;
}
void CMgrPlcInterface::Set_VaccumValue(float fValue)
{ 
	m_fVacuumValue = fValue;
}
CString CMgrPlcInterface::Get_VaccumValueKPAStr()
{
	CString csValue;
		csValue.Format("%.1f", Get_VacuumValue() / pow(10, 3));
	return csValue.GetString();
}
CString CMgrPlcInterface::Get_VaccumValuePAStr()
{
	CString csValue;
	csValue.Format("%.1f", Get_VacuumValue() / pow(10, 3));

	return csValue.GetString();
}
void CMgrPlcInterface::Set_PumpOpenPercent(float fOpenPercent)
{
	m_fPumpOpenPercent = fOpenPercent;
}
CString CMgrPlcInterface::Get_PumpOpenPercentStr()
{
	CString csValue;
	csValue.Format("%.1f", Get_PumpOpenPercent());

	return csValue.GetString();
}
void CMgrPlcInterface::Set_AutoMode(bool bAutoMode)
{
	m_bAutoMode = bAutoMode;
}
// -------------------------------------------------------------------------------------------------



