#include "stdafx.h"
#include "MgrSerial.h"

#include "MgrComm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMgrSerial::CMgrSerial()
{
	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return;

	std::vector<SDevice*> vecValue;
	pMgr->GetDeivceFromType(eDeviceTypeDaQ, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			CQueuingSerial* lpQueue = new CQueuingSerial;

			int nPrimaryKey = lpDevice->nPrimaryKey;

			lpQueue->SetPrimaryKey(nPrimaryKey);
			lpQueue->SetCommType(lpDevice->nCommObj);

			m_mapSerial.insert(std::pair<int, CQueuingSerial*>(nPrimaryKey, lpQueue));
		}

		++iter;
	}

	pMgr->GetDeivceFromType(eDeviceTypeChamber, vecValue);

	iter = vecValue.begin();
	iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			CQueuingSerial* lpQueue = new CQueuingSerial;

			int nPrimaryKey = lpDevice->nPrimaryKey;

			lpQueue->SetPrimaryKey(nPrimaryKey);
			lpQueue->SetCommType(lpDevice->nCommObj);

			for (int i = 0; i < lpDevice->arrChannel.GetSize(); ++i)
			{
				lpQueue->AddChannelArray(lpDevice->arrChannel.GetAt(i));
			}

			m_mapSerial.insert(std::pair<int, CQueuingSerial*>(nPrimaryKey, lpQueue));
		}

		++iter;
	}
	

	pMgr->GetDeivceFromType(eDeivceTypeBarcode, vecValue);

	 iter = vecValue.begin();
	 iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			CZebraBCRControl* lPBarcode = new CZebraBCRControl;

			int nStationNo = lpDevice->nChannelNumber;
			lPBarcode->m_nPort = lpDevice->nCommPort;

			m_mapBarcode.insert(std::pair<int, CZebraBCRControl*>(nStationNo, lPBarcode));

		}

		++iter;
	}
		

	for (auto it = m_mapBarcode.begin(); it != m_mapBarcode.end(); ++it)
	{
		CZebraBCRControl* pBarcode = it->second;

		if (pBarcode)
		{
			pBarcode->SetInit(pBarcode->m_nPort, 9600, 8, NOPARITY, 0, 0);
			pBarcode->CreateThread();

		}
	}

	Set_ChBarcodeCheckMode(true);
}


CMgrSerial::~CMgrSerial()
{
	RemoveAll();
}

void	CMgrSerial::StartThread()
{
	for (auto it = m_mapSerial.begin(); it != m_mapSerial.end(); ++it)
	{
		CQueuingSerial* pComm = it->second;

		if (pComm)
			pComm->StartThread();
	}



}

void	CMgrSerial::StopThread()
{
	for (auto it = m_mapSerial.begin(); it != m_mapSerial.end(); ++it)
	{
		CQueuingSerial* pComm = it->second;

		if (pComm)
			pComm->StopThread();
	}

	for (auto it = m_mapBarcode.begin(); it != m_mapBarcode.end(); ++it)
	{
		CZebraBCRControl* pBarcode = it->second;

		if (pBarcode)
		{			
			pBarcode->PostThreadMessage(WM_QUIT,0,0);

		}
	}
}

void	CMgrSerial::AddQueue(int nPrimaryKey, CString strTime, BYTE* pszData, int nDataLenth /* nDataLenth defult 0*/)
{
	CQueuingSerial* pSerial = GetAt(nPrimaryKey);

	if (pSerial)
	{
		TSerial* pComm = new TSerial;

		memset(pComm->pszComm, NULL, 1024);

		pComm->nPrimaryKey	= nPrimaryKey;
		pComm->strTime		= strTime;
		pComm->nDataLenth	= nDataLenth;

		memcpy(pComm->pszComm, pszData, sizeof(pComm->pszComm));

		pSerial->AddQueue(pComm);
	}
}

void	CMgrSerial::AddQueue(int nPrimaryKey, CString strTime, float fValue)
{
	CQueuingSerial* pSerial = GetAt(nPrimaryKey);

	if (pSerial)
	{
		TSerial* pComm = new TSerial;

		memset(pComm->pszComm, NULL, 1024);

		pComm->nPrimaryKey = nPrimaryKey;
		pComm->strTime = strTime;
		pComm->fValue = fValue;

		pSerial->AddQueue(pComm);
	}
}

void	CMgrSerial::AddQueue(int nPrimaryKey, CString strTime, float fValue, int nType)
{
	CQueuingSerial* pSerial = GetAt(nPrimaryKey);

	if (pSerial)
	{
		TSerial* pComm = new TSerial;

		memset(pComm->pszComm, NULL, 1024);

		pComm->nPrimaryKey = nPrimaryKey;
		pComm->strTime = strTime;
		pComm->fValue = fValue;
		pComm->nChamberDataType = nType;

		pSerial->AddQueue(pComm);
	}
}

void	CMgrSerial::AddQueue(int nPrimaryKey, CString strTime, int nElecType, float fValue)
{
	CQueuingSerial* pSerial = GetAt(nPrimaryKey);

	if (pSerial)
	{
		TSerial* pComm = new TSerial;
		memset(pComm->pszComm, NULL, 1024);

		pComm->nPrimaryKey = nPrimaryKey;
		pComm->strTime = strTime;
		pComm->nElecType = nElecType;
		pComm->fValue = fValue;

		pSerial->AddQueue(pComm);
	}
}

void	CMgrSerial::AddQueue(int nPrimaryKey, CString strTime, char* pData)
{
	CQueuingSerial* pSerial = GetAt(nPrimaryKey);

	if (pSerial)
	{
		TSerial* pComm = new TSerial;

		memset(pComm->pszComm, NULL, 1024);

		pComm->nPrimaryKey	= nPrimaryKey;
		pComm->strTime		= strTime;

		memcpy(pComm->pszComm, pData, sizeof(pComm->pszComm));

		pSerial->AddQueue(pComm);
	}
}

CQueuingSerial*	CMgrSerial::GetAt(int nPrimaryKey)
{
	auto it = m_mapSerial.find(nPrimaryKey);

	if (it == m_mapSerial.end())
		return NULL;

	return it->second;
}

CZebraBCRControl * CMgrSerial::GetAtBarcode(int nStationNo)
{
	auto it = m_mapBarcode.find(nStationNo);

	if (it == m_mapBarcode.end())
		return NULL;

	return it->second;
}

void CMgrSerial::Set_ChBarcodeCheckMode(bool bChBarcodeCheckMode)
{
	m_bChBarcodeCheck = bChBarcodeCheckMode;

}


void	CMgrSerial::RemoveAll()
{
	for (auto it = m_mapSerial.begin(); it != m_mapSerial.end(); ++it)
	{
		delete it->second;
	}
	/*for (auto it = m_mapBarcode.begin(); it != m_mapBarcode.end(); ++it)
	{	
		delete it->second;
	}*/
	m_mapSerial.clear();
	m_mapBarcode.clear();
}
