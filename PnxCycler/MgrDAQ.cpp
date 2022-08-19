#include "stdafx.h"
#include "MgrDAQ.h"

#include "PnxUtil.h"
#include "MgrCommData.h"
#include "MgrChannel.h"

// Group 추가 되면 여기다가 ㄱㄱ
enum DAQContent
{
	DAQContentIndex = 0,
	DAQContentUse,
	DAQContentChannel,
	DAQContentSensorType,
	DAQContentSensorChannel,
	DAQContentName,
	DAQContentSafetyMax,
	DAQContentSafetyMin,
	DAQContentFinishMax,		//NOT USE
	DAQContentFinishMin,		//NOT USE
	DAQContentUniqueCode
};

enum BalanceContent
{
	BalanceContentIndex = 0,
	BalanceContentUse,
	BalanceContentType,
	BalanceContentResistance,
	BalanceContentValue,
	BalanceContentStart,
	BalanceContentEnd
};

CMgrDAQ::CMgrDAQ()
{
	m_strFileName.Empty();
	m_pThread = nullptr;
	InitializeCriticalSection(&m_cs);
	m_bWriteDAQ = false;
}


CMgrDAQ::~CMgrDAQ()
{
	RemoveAll();
	EnterCriticalSection(&m_cs);
	if (m_pThread)
	{
		WaitForSingleObject(m_pThread->m_hThread, INFINITY);
		m_pThread = NULL;
	}
	LeaveCriticalSection(&m_cs);
	SYSLOG(Dbg, _T("MgrDAQ Destroy"));

	DeleteCriticalSection(&m_cs);
}


void CMgrDAQ::ClearDAQ()
{
	RemoveAll();
	EnterCriticalSection(&m_cs);
	if (m_pThread)
	{
		WaitForSingleObject(m_pThread->m_hThread, INFINITY);
		m_pThread = NULL;
	}
	LeaveCriticalSection(&m_cs);
}


void	CMgrDAQ::LoadDAQCell()
{
	CString str;

	for (int i = 0; i < CMgrChannel::GetMgr()->GetSize(); ++i)
	{
		CDAQ* pDAQ = new CDAQ;

		// 1. DAQ Index
		str.Format("%d", i + 1);
		pDAQ->SetDaqIndex(str);

		// 2. DAQ Use
		pDAQ->SetDaqUse("1");

		// 3. Channel
		str.Format("CH%d", i + 1);
		pDAQ->SetChannelNumber(str);

		// 4. DAQ Name
		str.Format("Temp Sensor%d", i + 1);
		pDAQ->SetDAQName(str);

		// 5. DAQ SensorType
		str.Format("1");
		pDAQ->SetSensorType(str);

		// 6. DAQ SensorIndex
		str.Format("%d", i + 1);
		pDAQ->SetSensorIndex(str);

		pDAQ->SetSafetyConditionMax("100.000");
		pDAQ->SetSafetyConditionMin("-100.000");
		pDAQ->SetFinishConditionMax("100.000");
		pDAQ->SetFinishConditionMin("-100.000");

		auto it = m_mapDAQ.find(pDAQ->GetUniqueCode());

		if (it == m_mapDAQ.end())
		{
			CDAQList* listDAQ = new CDAQList;

			listDAQ->Add(pDAQ);

			m_mapDAQ.insert(std::pair< int, CDAQList* >(pDAQ->GetUniqueCode(), listDAQ));
		}
		else
		{
			CDAQList* listDAQ = it->second;

			if (listDAQ)
				listDAQ->Add(pDAQ);
		}

		m_listDAQ.AddTail(pDAQ);
	}

	CMgrCommData::GetMgr()->MappingDAQ();
}

void	CMgrDAQ::LoadDAQ(CString lpszFilePath)
{
	CString strFilePath = lpszFilePath;

	RemoveAll();

	CStdioFile file(strFilePath, CFile::modeRead | CFile::typeText);
	CString strContent = "";
	CStringArray arrTempTable;
	//file.ReadString(strContent); // 첫번째줄은 헤더이므로 버리자
	int nMode = 0;
	while (file.ReadString(strContent))
	{
		if (strContent.Find(_T("DataStart")) >= 0)
		{
			file.ReadString(strContent);  // 헤더값은 버리자
			file.ReadString(strContent);
			nMode = 1;
		}
		else if (strContent.Find(_T("DataEnd")) >= 0)
			nMode = 2;
		else if (strContent.Find(_T("TempStart")) >= 0)
		{
			file.ReadString(strContent);  // 헤더값은 버리자
			file.ReadString(strContent);
			nMode = 3;
		}
		else if (strContent.Find(_T("TempEnd")) >= 0)
			nMode = 4;

		if (nMode == 1)
		{
			AddDAQ(strContent);
		}
		else if (nMode == 3)
		{
			AddTempTable(strContent);
		}
	}
	file.Close();
	LoadBalance(strFilePath);

	CMgrCommData::GetMgr()->MappingDAQ();
}

void	CMgrDAQ::AddDAQ(LPCTSTR lpszFullString)
{
	CStringArray arr;

	CPnxUtil::GetMgr()->SplitString(lpszFullString, ',', arr);

	CDAQ* pDAQ = new CDAQ;

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		CString str = arr.GetAt(i);

		switch (i)
		{
		case DAQContentIndex:			pDAQ->SetDaqIndex(str);				break;
		case DAQContentUse:				pDAQ->SetDaqUse(str);				break;
		case DAQContentChannel:			pDAQ->SetChannelNumber(str);		break;
		case DAQContentSensorType:		pDAQ->SetSensorType(str);			break;
		case DAQContentSensorChannel:	pDAQ->SetSensorIndex(str);			break;
		case DAQContentName:			pDAQ->SetDAQName(str);				break;
		case DAQContentSafetyMax:		pDAQ->SetSafetyConditionMax(str);	break;
		case DAQContentSafetyMin:		pDAQ->SetSafetyConditionMin(str);	break;
		case DAQContentFinishMax:		pDAQ->SetFinishConditionMax(str);	break;
		case DAQContentFinishMin:		pDAQ->SetFinishConditionMin(str);	break;
		case DAQContentUniqueCode:		pDAQ->SetUniqueCode(str);			break;
		}
	}

	// Data 가 없는 것들도 있다..
	if (arr.GetSize() <= 0 || pDAQ->GetChannelNumber() < 0)
	{
		DELETE_POINTER(pDAQ);

		return;
	}

	auto it = m_mapDAQ.find(pDAQ->GetUniqueCode());

	if (it == m_mapDAQ.end())
	{
		CDAQList* listDAQ = new CDAQList;

		listDAQ->Add(pDAQ);

		m_mapDAQ.insert(std::pair< int, CDAQList* >(pDAQ->GetUniqueCode(), listDAQ));
	}
	else
	{
		CDAQList* listDAQ = it->second;

		if (listDAQ)
			listDAQ->Add(pDAQ);
	}

	m_listDAQ.AddTail(pDAQ);
}


void	CMgrDAQ::AddTempTable(LPCTSTR lpszFullString)
{
	CStringArray arr;

	CPnxUtil::GetMgr()->SplitString(lpszFullString, ',', arr);

	if (arr.GetCount() != 3)
		return;

	TTempTable* pTemp = new TTempTable;
	pTemp->fFahrenheit = (float)atof(arr[0]);
	pTemp->fCelsius = (float)atof(arr[1]);
	pTemp->fThermistor = (float)atof(arr[2]);

	m_listTempTable.AddTail(pTemp);
}

void	CMgrDAQ::LoadBalance(CString strPath)
{
	m_strFileName = strPath.Mid(strPath.ReverseFind('\\') + 1);

	CString strFilePath;

	strFilePath.Format(_T("%s\\Config\\Balance\\%s"), GetExePath(), m_strFileName);

	FILE* fp = NULL;

	fopen_s(&fp, strFilePath, _T("r"));

	if (NULL == fp)
		return;

	char szContent[1024] = { 0, };

	memset(szContent, NULL, 1024);

	while (fgets(szContent, 1024, fp))
	{
		CString strContent;

		strContent.Format(_T("%s"), szContent);

		strContent.Remove('\r');
		strContent.Remove('\n');

		memset(szContent, NULL, 1024);

		AddBalance(strContent);
	}

	fclose(fp);
}

void	CMgrDAQ::AddBalance(LPCTSTR lpszFullString)
{
	CStringArray arr;

	CPnxUtil::GetMgr()->SplitString(lpszFullString, ',', arr);

	CDAQ* pSrc = new CDAQ;

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		CString str = arr.GetAt(i);

		switch (i)
		{
		case BalanceContentIndex: pSrc->SetDaqIndex(str);			break;
		case BalanceContentUse: pSrc->SetUseBalance(str);			break;
		case BalanceContentType: pSrc->SetBalanceType(str);		break;
		case BalanceContentResistance: pSrc->SetBalanceResistance(str);	break;
		case BalanceContentValue: pSrc->SetBalanceVoltage(str);		break;
		case BalanceContentStart: pSrc->SetStartCondition(str);		break;
		case BalanceContentEnd: pSrc->SetEndCondition(str);		break;
		}
	}

	auto Pos = m_listDAQ.FindIndex(pSrc->GetDaqIndex());

	if (Pos)
	{
		CDAQ* pDst = m_listDAQ.GetAt(Pos);

		if (pDst)
		{
			pDst->CopyData(pSrc, TRUE);
		}
	}

	delete pSrc;

	pSrc = NULL;
}

CDAQList*	CMgrDAQ::GetDAQMap(int nUniqueCode /*= 0*/)
{
	auto it = m_mapDAQ.find(nUniqueCode);

	if (it == m_mapDAQ.end())
		return NULL;

	return it->second;
}

CDAQ*	CMgrDAQ::GetDAQ(int nDAQIndex)
{
	for (auto it = m_mapDAQ.begin(); it != m_mapDAQ.end(); ++it)
	{
		CDAQList* mapDAQ = it->second;

		if (NULL == mapDAQ)
			continue;

		CList< CDAQ*, CDAQ* >& listDAQ = mapDAQ->GetDAQList();

		auto Pos = listDAQ.GetHeadPosition();

		while (Pos)
		{
			CDAQ* pDAQ = listDAQ.GetNext(Pos);

			if (NULL == pDAQ)
				continue;

			if (pDAQ->GetDaqIndex() == nDAQIndex)
				return pDAQ;
		}
	}

	return NULL;
}

CList< CDAQ*, CDAQ* >&	CMgrDAQ::GetDAQList()
{
	return m_listDAQ;
}

int		CMgrDAQ::GetSize()
{
	return m_mapDAQ.size();
}

CString	CMgrDAQ::GetFileName()
{
	return m_strFileName;
}

void	CMgrDAQ::RemoveAll()
{
	for (auto it = m_mapDAQ.begin(); it != m_mapDAQ.end(); ++it)
	{
		delete it->second;
	}

	m_mapDAQ.clear();

	m_listDAQ.RemoveAll();

	while (!m_listTempTable.IsEmpty())
		delete m_listTempTable.RemoveTail();

	m_listTempTable.RemoveAll();

	CMgrCommData::GetMgr()->RemoveAllDAQ();
}

float	CMgrDAQ::GetDAQVoltageMax(int nChannelNumber)
{
	float fValue = 0.000;

	for (auto it = m_mapDAQ.begin(); it != m_mapDAQ.end(); ++it)
	{
		CList< CDAQ*, CDAQ* >& listDAQ = it->second->GetDAQList();

		auto Pos = listDAQ.GetHeadPosition();

		while (Pos)
		{
			CDAQ* pDAQ = listDAQ.GetNext(Pos);

			if (NULL == pDAQ)
				continue;

			if (nChannelNumber != pDAQ->GetChannelNumber())
				continue;

			if (fValue <= pDAQ->GetRealVoltage())
				fValue = pDAQ->GetRealVoltage();
		}
	}

	return fValue;
}

float	CMgrDAQ::GetDAQTemperatureMax(int nChannelNumber)
{
	float fValue = 0.000;

	for (auto it = m_mapDAQ.begin(); it != m_mapDAQ.end(); ++it)
	{
		CList< CDAQ*, CDAQ* >& listDAQ = it->second->GetDAQList();

		auto Pos = listDAQ.GetHeadPosition();

		while (Pos)
		{
			CDAQ* pDAQ = listDAQ.GetNext(Pos);

			if (NULL == pDAQ)
				continue;

			if (nChannelNumber != pDAQ->GetChannelNumber())
				continue;

			if (fValue <= pDAQ->GetRealTemperature())
				fValue = pDAQ->GetRealTemperature();
		}
	}

	return fValue;
}

float CMgrDAQ::GetDAQTemperature(int nChannelNumber)
{
	float fValue = 0.000;

	for (auto it = m_mapDAQ.begin(); it != m_mapDAQ.end(); ++it)
	{
		CList< CDAQ*, CDAQ* >& listDAQ = it->second->GetDAQList();

		auto Pos = listDAQ.GetHeadPosition();

		while (Pos)
		{
			CDAQ* pDAQ = listDAQ.GetNext(Pos);

			if (NULL == pDAQ)
				continue;

			if (nChannelNumber != pDAQ->GetChannelNumber())
				continue;

			fValue = pDAQ->GetRealTemperature();
			



			//fValue = fValue*100 / 8.19175;
		}
	}

	return fValue;
}

float CMgrDAQ::GetDAQTemperature_Convert(float fReg)
{
	auto Pos = m_listTempTable.GetHeadPosition();
	float fValue = 0.0;
	double dHighTemp = 0;
	double dHighReg = 0;
	double dLowTemp = 0;
	double dLowReg = 0;
	while (Pos)
	{
		TTempTable*  pTempTable = m_listTempTable.GetNext(Pos);
		if (NULL == pTempTable)
			continue;
		if (m_listTempTable.GetAt(Pos)->fThermistor > fReg)
		{
			dHighReg = m_listTempTable.GetAt(Pos)->fThermistor;
			dHighTemp = m_listTempTable.GetAt(Pos)->fFahrenheit;
		}
		else
		{
			dLowReg = m_listTempTable.GetAt(Pos)->fThermistor;
			dLowTemp = m_listTempTable.GetAt(Pos)->fFahrenheit;
			break;
		}
	}

	fValue = (dHighTemp - dLowTemp) / (dLowReg - dHighReg)*(fReg - dHighReg) + dLowTemp;



	return fValue;
}

CString CMgrDAQ::GetDAQTemperaturestr(int nChannelNumber)
{
	CString str;

	str.Format(_T("%.1f"), GetDAQTemperature(nChannelNumber));

	return str;
}


void	CMgrDAQ::SaveBalance()
{
	CString strFilePath;

	strFilePath.Format(_T("%s\\Config\\Balance\\%s"), GetExePath(), m_strFileName);

	DeleteFile(strFilePath);

	FILE* fp = NULL;

	fopen_s(&fp, strFilePath, _T("w"));

	if (NULL == fp)
		return;

	auto Pos = m_listDAQ.GetHeadPosition();

	while (Pos)
	{
		CDAQ* pDAQ = m_listDAQ.GetNext(Pos);

		if (NULL == pDAQ)
			continue;

		fprintf(fp, _T("%d,%d,%d,%d,%s,%s,%s\n"),
			pDAQ->GetDaqIndex() + 1,
			pDAQ->GetUseBalance(),
			pDAQ->GetBalanceType(),
			pDAQ->GetBalanceResistance(),
			pDAQ->GetBalanceVoltageStr(),
			pDAQ->GetStartConditionStr(),
			pDAQ->GetEndConditionStr());
	}

	fclose(fp);
}



CString	CMgrDAQ::GetLogHeaderDAQ(int nChannelNumber)
{
	CString strHeader;

	auto Pos = m_listDAQ.GetHeadPosition();
	CString str;
	int nVoltIndex = 1;
	int nTempIndex = 1;

	while (Pos)
	{
		CDAQ* pDAQ = m_listDAQ.GetNext(Pos);

		if (NULL == pDAQ)
			continue;

		if (pDAQ->GetChannelNumber() != nChannelNumber)
			continue;

		if (pDAQ->GetSensorType() == READ_TYPE_VOLT)
		{
			str.Format(_T("DAQ_Volt%d,"), nVoltIndex);
			nVoltIndex++;
		}
		else if (pDAQ->GetSensorType() == READ_TYPE_TEMPERATURE)
		{
			str.Format(_T("DAQ_Temp%d,"), nTempIndex);
			nTempIndex++;
		}

		strHeader += str;
	}

	return strHeader;
}