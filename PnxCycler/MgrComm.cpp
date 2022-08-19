#include "stdafx.h"
#include "MgrComm.h"

#include "MgrConfig.h"
#include "PnxUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define OneByte	( 8 )

CMgrComm::CMgrComm()
{
	LoadDevice();
}


CMgrComm::~CMgrComm()
{
	RemoveAll();
}

void	CMgrComm::LoadDevice()
{
	CString strFilePath;

	strFilePath.Format(_T("%s\\Config\\Comm.ini"), (LPCSTR)GetExePath());

	LPCTSTR lpszCommObj[] = { _T("InterfaceBoard"), _T("DAQ"), _T("Chamber"), _T("Chiller"), _T("BalanceBoard"), _T("Vector")
								, _T("DmmAnalyzer"), _T("DmmPower"), _T("BmsPower"), _T("AnalyzeTool"), _T("PCAN"), _T("Remote")
								, _T("Calibrator"), _T("PID"), _T("PLCInterface"), _T("BARCODE"),nullptr };

	for (int i = 0;; ++i)
	{
		if (nullptr == lpszCommObj[i])
			break;

		int nCommType	= INIReadInt(_T("CommType"), lpszCommObj[i], strFilePath);
		int nCommCount	= INIReadInt(_T("CommCount"), lpszCommObj[i], strFilePath);

		for (int j = 0; j < nCommCount; ++j)
		{
			CString strAppName;
			strAppName.Format(_T("%s%d"), lpszCommObj[i], j+1);

			SDevice* lpDevice = new SDevice;

			lpDevice->nType					= nCommType;
			lpDevice->nCommObj				= i;
			lpDevice->nIndex				= j;	// Board Number
			lpDevice->strName				= INIReadStr(strAppName, _T("Name"), strFilePath);
			lpDevice->strIP					= INIReadStr(strAppName, _T("TCP-IP"), strFilePath);
			lpDevice->nPort					= INIReadInt(strAppName, _T("TCP-Port"), strFilePath);
			lpDevice->nPort2				= INIReadInt(strAppName, _T("TCP-Port2"), strFilePath);
			lpDevice->nCommPort				= INIReadInt(strAppName, _T("RS-Port"), strFilePath);
			lpDevice->nBaudrate				= INIReadInt(strAppName, _T("RS-Baudrate"), strFilePath);
			lpDevice->nSamplingTime			= INIReadInt(strAppName, _T("SamplingTime"), strFilePath);
			lpDevice->nSendPeriod			= INIReadInt(strAppName, _T("SendPeriod"), strFilePath);
			lpDevice->nPrimaryKey			= INIReadInt(strAppName, _T("PrimaryKey"), strFilePath);
			lpDevice->nChamberChannel		= INIReadInt(strAppName, _T("ChamberNumber"), strFilePath);
			lpDevice->nChamberControlType	= INIReadInt(strAppName, _T("ChamberControlType"), strFilePath);
			lpDevice->strChamberChannel		= INIReadStr(strAppName, _T("Channel"), strFilePath);
			lpDevice->nDaqDataType			= INIReadInt(strAppName, _T("DATA_TYPE"), strFilePath);
			lpDevice->strDeviceType			= INIReadStr(strAppName, _T("DeviceType"), strFilePath);
			lpDevice->nUsbChannel			= INIReadInt(strAppName, _T("USBChannel"), strFilePath);
			CString strCanID				= INIReadStr(strAppName, _T("CanID"), strFilePath);
			CString strCheckCanID			= INIReadStr(strAppName, _T("CheckID"), strFilePath);
			if (strCanID != "") {
				CPnxUtil::GetMgr()->SplitStringToHex(strCanID, ',', lpDevice->arrCanId);
			}

			lpDevice->nCheckCanID = CPnxUtil::GetMgr()->String2Hex(strCheckCanID);

			// --------------------------------------------------------------------------

			CString strCalCnt = _T("");
			strCalCnt.Format(_T("Calibrator%d"), j + 1);
			if (strAppName.CompareNoCase(strCalCnt) == 0)
			{
				if (nCommCount == 0)
				{
					CMgrConfig::GetMgr()->SetCalibratorIP("");
					CMgrConfig::GetMgr()->SetCalibratorPort(0);
				}
				else
				{
					CMgrConfig::GetMgr()->SetCalibratorIP(lpDevice->strIP);
					CMgrConfig::GetMgr()->SetCalibratorPort(lpDevice->nPort);
				}
			}

			// --------------------------------------------------------------------------

			CString strChannel = INIReadStr(strAppName, _T("Channel"), strFilePath);

			if (strChannel.Find(_T(",")) < 0) {
				lpDevice->nChannelNumber = atoi(strChannel);
			}
			else {
				CPnxUtil::GetMgr()->SplitString(strChannel, ',', lpDevice->arrChannel);
			}
			
			AddDevice(lpDevice);
		}

		m_arrCommType[i] = nCommType;
	}
}

void	CMgrComm::AddDevice(SDevice* lpDevice)
{
	auto iter = m_mapDevice.find(lpDevice->nCommObj);

	if (iter == m_mapDevice.end())
	{
		std::vector<SDevice*> vecValue;

		vecValue.push_back(lpDevice);

		m_mapDevice.insert(std::make_pair(lpDevice->nCommObj, vecValue));
	}
	else
	{
		std::vector<SDevice*> vecValue = iter->second;

		vecValue.push_back(lpDevice);

		m_mapDevice[lpDevice->nCommObj] = vecValue;
	}
}

int		CMgrComm::GetCommType(int nCommObj)
{
	if (nCommObj >= eDeviceTypeIFBoard && nCommObj < eDeviceTypeMaxCount)
		return m_arrCommType[nCommObj];

	return eCommTypeUnknown;
}

bool	CMgrComm::GetDeivceFromType(int nCommObj, std::vector<SDevice*>& vecValue)
{
	vecValue.clear();

	auto it = m_mapDevice.find(nCommObj);

	if (it != m_mapDevice.end())
	{
		std::vector<SDevice*> vecDevice = it->second;

		auto iter = vecDevice.begin();
		auto iterE = vecDevice.end();

		while (iter != iterE)
		{
			SDevice* lpDevice = (*iter);

			if (lpDevice)
				vecValue.push_back(lpDevice);

			++iter;
		}
	}

	return true;
}

int		CMgrComm::GetChannelNumber(int nCommObj, int nBoardNumber, BYTE byChannel)
{
	auto it = m_mapDevice.find(nCommObj);

	if (it == m_mapDevice.end())
		return -1;

	std::vector<SDevice*> vecDevice = it->second;

	int nIndex = 0;
	auto iter = vecDevice.begin();
	auto iterE = vecDevice.end();
	int nChannel = 1;
	switch ((int)byChannel)
	{
	case 1: nChannel = 0; break;
	case 2: nChannel = 1; break;
	case 4: nChannel = 2; break;
	case 8: nChannel = 3; break;
	}

	SDevice* lpDevice = NULL;
	while (iter != iterE)
	{
		lpDevice = (*iter);
		if (nBoardNumber == nIndex && lpDevice)
		{
			return atoi(lpDevice->arrChannel.GetAt(nChannel));
			break;
		}
		++iter;
		nIndex++;
	}

	return -1;
/*
	BOOL arrBit[OneByte] = { FALSE, };

	for (int i = 0; i < OneByte; ++i)
	{
		arrBit[i] = (byChannel & (1 << i)) != 0;

		if (FALSE == arrBit[i])
			continue;

		auto iter = vecDevice.begin();
		auto iterE = vecDevice.end();

		while (iter != iterE)
		{
			SDevice* lpDevice = (*iter);

			if (lpDevice)
			{
				if (nBoardNumber == lpDevice->nIndex)
				{
					for (int j = 0; j < lpDevice->arrChannel.GetSize(); ++j)
					{
						if (j == i)
						{
							return atoi(lpDevice->arrChannel.GetAt(j));
						}
					}
				}
			}

			++iter;
		}
	}

	return -1;*/
}

int		CMgrComm::GetBoardNumber(int nChannelNumber)
{
	auto it = m_mapDevice.find(eDeviceTypeIFBoard);

	if (it == m_mapDevice.end())
		return -1;

	std::vector<SDevice*> vecDevice = it->second;

	auto iter = vecDevice.begin();
	auto iterE = vecDevice.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			for (int i = 0; i < lpDevice->arrChannel.GetSize(); ++i)
			{
				if (atoi(lpDevice->arrChannel.GetAt(i)) == nChannelNumber)
					return lpDevice->nIndex;
			}
		}

		++iter;
	}

	return -1;
}

int		CMgrComm::GetPrimaryKey(int nCommObj)
{
	auto it = m_mapDevice.find(nCommObj);

	if (it == m_mapDevice.end())
		return -1;

	std::vector<SDevice*> vecDevice = it->second;

	auto iter = vecDevice.begin();
	auto iterE = vecDevice.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
			return lpDevice->nPrimaryKey;

		++iter;
	}

	return -1;
}

int		CMgrComm::GetDeviceKeyFromType(int nCommObj, int nCyclerChannel)
{
	auto it = m_mapDevice.find(nCommObj);

	if (it == m_mapDevice.end())
		return -1;

	std::vector<SDevice*> vecDevice = it->second;

	auto iter	= vecDevice.begin();
	auto iterE	= vecDevice.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			for (int i = 0; i < lpDevice->arrChannel.GetSize(); ++i)
			{
				CString strCycleChannel = lpDevice->arrChannel.GetAt(i);

				if (atoi(strCycleChannel) == nCyclerChannel)
					return lpDevice->nPrimaryKey;
			}
		}

		++iter;
	}

	return -1;
}

int		CMgrComm::GetCyclerChannelFromDevice(int nCommObj, int nCyclerChannel)
{
	auto it = m_mapDevice.find(nCommObj);

	if (it == m_mapDevice.end())
		return -1;

	CString strCyclerChannel;
	strCyclerChannel.Format("%d", nCyclerChannel);

	std::vector<SDevice*> vecDevice = it->second;

	auto iter	= vecDevice.begin();
	auto iterE	= vecDevice.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			if (lpDevice->strChamberChannel.Find(strCyclerChannel) >= 0)
			{
				for (int i = 0; i < lpDevice->arrChannel.GetSize(); ++i)
				{
					CString str = lpDevice->arrChannel.GetAt(i);

					if (atoi(str) != nCyclerChannel)
						return atoi(str);
				}
			}			
		}

		++iter;
	}

	return -1;
}

void	CMgrComm::RemoveAll()
{
	auto iter = m_mapDevice.begin();
	auto iterE = m_mapDevice.end();

	while (iter != iterE)
	{
		std::vector<SDevice*> vecDevice = iter->second;

		auto it = vecDevice.begin();
		auto itE = vecDevice.end();

		while (it != itE)
		{
			SDevice* lpDevice = (*it);

			if (lpDevice)
			{
				delete lpDevice;
				lpDevice = nullptr;
			}

			++it;
		}

		vecDevice.clear();

		++iter;
	}

	m_mapDevice.clear();
}
