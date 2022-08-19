#include "stdafx.h"
#include "QueuingSerial.h"

#include "MgrCommData.h"
#include "MgrChannel.h"
#include "MgrDAQ.h"
#include "CMgrChamber.h"
#include "SerialCom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CQueuingSerial::CQueuingSerial()
{
	m_pQueuingThread	= NULL;
	m_bLoopQueue		= TRUE;
	m_nPrimaryKey		= -1;
	m_nCommType			= -1;
}


CQueuingSerial::~CQueuingSerial()
{
}

void	CQueuingSerial::StartThread()
{
	if (NULL == m_pQueuingThread)
		m_pQueuingThread = AfxBeginThread(QueuingThread, this);
}

void	CQueuingSerial::StopThread()
{
	m_bLoopQueue = FALSE;

	if (m_pQueuingThread)
	{
		::WaitForSingleObject(m_pQueuingThread->m_hThread, INFINITE);

		m_pQueuingThread = NULL;
	}

	// Thread 종료 후 Queue 비우자
	while (!m_Queue.IsEmpty())
		delete m_Queue.RemoveHead();
}

void	CQueuingSerial::SetPrimaryKey(int nPrimaryKey)
{
	m_nPrimaryKey = nPrimaryKey;
}

void	CQueuingSerial::AddChannelArray(CString str)
{
	m_arrChannel.Add(str);
}

CStringArray&	CQueuingSerial::GetChannelArray()
{
	return m_arrChannel;
}

void	CQueuingSerial::AddQueue(TSerial* pNewElement)
{
	m_csPush.Lock();

	m_Queue.AddTail(pNewElement);

	m_csPush.Unlock();
}

TSerial*	CQueuingSerial::GetQueue()
{
	m_csPush.Lock();

	TSerial* pComm = NULL;

	if (FALSE == m_Queue.IsEmpty())
		pComm = m_Queue.RemoveHead();

	m_csPush.Unlock();

	return pComm;
}

void	CQueuingSerial::ParseChamber(const TSerial* pComm)
{
	CMgrCommData* pMgrCommData = CMgrCommData::GetMgr();

	if (!pMgrCommData)
		return;

	CMgrChannel* pMgrChannel = CMgrChannel::GetMgr();
	
	if (!pMgrChannel)
		return;

	CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();

	if (!pMgrChamber)
		return;

	for (int i = 0; i < m_arrChannel.GetSize(); ++i)
	{
		int nCyclerChannelNumber = atoi(m_arrChannel.GetAt(i));

		CCommData* lpCyclerInfo = pMgrCommData->GetAt(nCyclerChannelNumber);

		if(!lpCyclerInfo)
			continue;

		CChannel* lpCyclerChannel = pMgrChannel->GetAt(nCyclerChannelNumber);

		if(!lpCyclerChannel)
			continue;

		SChamber* lpDeviceChamber = pMgrChamber->GetDeivceCyclerChannel(nCyclerChannelNumber);

		if (lpDeviceChamber)
		{
			switch (pComm->nChamberDataType)
			{
				case eChamberDataTypeValue:				
				{
					lpCyclerInfo->SetChamberTemperature(pComm->fValue);
					lpDeviceChamber->fCurrentValue = pComm->fValue;
				
					break;
				}
				case eChamberDataTypeSetting:
				{
	#if TURBY_CHAMBER_UPDATE
					lpCyclerInfo->SetChamberTemperatureSP(pComm->fValue);
	#else
					lpCyclerInfo->SetChamberTemperatureSP(pComm->fValue);

					lpDeviceChamber->fSettingValue = pComm->fValue;
	#endif

					break;
				}
			}
		}
		else
		{
			;
		}
	} 
}
float	CQueuingSerial::CalcThermistor2Temp(double dData)
{
	CList< TTempTable*, TTempTable* >& listDAQ = CMgrDAQ::GetMgr()->GetTempTable();
	float fResult = 0.f;
	POSITION pos = listDAQ.GetHeadPosition();
	TTempTable* pTemp = NULL;
	TTempTable* pPrevTemp = NULL;
	while (pos)
	{
		pTemp = listDAQ.GetNext(pos);
		if (pTemp == NULL)
			continue;

		if (dData > pTemp->fThermistor)
			break;
		else
			pPrevTemp = pTemp;
	}
	float dCelsius_1 = (pPrevTemp->fThermistor - pTemp->fThermistor) / (pTemp->fCelsius - pPrevTemp->fCelsius);
	float dValue2 = (float)((dData - pTemp->fThermistor) / dCelsius_1);
	fResult = (float)(pTemp->fCelsius - dValue2);

	return fResult;
}
void	CQueuingSerial::ParseDAQ(const TSerial* pComm)
{
	// csv 파일에서 불러온 DAQ 리스트
	CList< CDAQ*, CDAQ* >& listDAQ = CMgrDAQ::GetMgr()->GetDAQList();

	if (listDAQ.GetSize() <= 0)
		return;

	int nDataAmount = (pComm->nDataLenth - Size_Byte_STX - Size_Byte_ETX - Size_Byte_Type) / Size_Byte_Data;
	if (nDataAmount <= 0)
		return;

	float fAccumulateVoltage = 0.f;
	CDAQ* pDAQ = NULL;
	int nStartDaqTotalIndex = 0;
	int nEndDaqTotalIndex = 0;

	POSITION Pos = NULL;

	int nIndex, nSign, nConstant, nDecimel;
	nIndex = nSign = nConstant = nDecimel = 0;
	CString strTemp = "";
	float dData = 0;
	float fOldValue = 0;
	Pos = listDAQ.GetHeadPosition();
	CString str;

	while (Pos != NULL)
	{
		pDAQ = listDAQ.GetNext(Pos);
		if (pDAQ == NULL)
			continue;

		nIndex = pDAQ->GetDaqIndex();

		if (nIndex < 0 || nIndex >= nDataAmount)
			break;

		//PARSE
		nSign = (int)(pComm->pszComm[Index_SIGN + (nIndex * Size_Byte_Data)] == 0x00) ? 1 : -1;

		nConstant = (int)(pComm->pszComm[Index_DATA1 + (nIndex * Size_Byte_Data)]);
		nDecimel = (int)(pComm->pszComm[Index_DATA2 + (nIndex * Size_Byte_Data)] << 8 | pComm->pszComm[Index_DATA3 + (nIndex * Size_Byte_Data)]);

		strTemp.Format("%d.%04d", nConstant, nDecimel);
		dData = (float)atof(strTemp);

		if (nSign != 1)
			dData *= (-1.0f);

		if (pDAQ->GetSensorType() == READ_TYPE_THERMISTOR)
			dData *= 1000; // k옴으로 들어옴.

		if (pDAQ->GetSensorType() == READ_TYPE_VOLT)
			fOldValue = pDAQ->GetRealVoltage();
		else if (pDAQ->GetSensorType() == READ_TYPE_TEMPERATURE)
			fOldValue = pDAQ->GetRealTemperature();
		else if (pDAQ->GetSensorType() == READ_TYPE_THERMISTOR)
		{
			dData = CalcThermistor2Temp(dData);
			fOldValue = pDAQ->GetRealTemperature();
			str.Format(_T("%.3f"), dData);

		}
		if (abs(fOldValue - dData) > 5)
			pDAQ->m_nErrCount++;
		else
			pDAQ->m_nErrCount = 0;

		//DAQ UPDATE, 에러가 아니거나 연속 5회 이상 5V도는 5도 이상 차이나면 값을 넣음
		if (pDAQ->m_nErrCount == 0 || pDAQ->m_nErrCount >= 5)
		{
			if (pDAQ->GetSensorType() == READ_TYPE_VOLT)
			{
				pDAQ->SetRealVoltage(dData);
				fAccumulateVoltage += dData;
				pDAQ->SetAccumulateVoltage(fAccumulateVoltage);
			}
			else if (pDAQ->GetSensorType() == READ_TYPE_TEMPERATURE)
			{
				if (str == "-nan(ind)")
				{
					pDAQ->SetRealTemperature(pDAQ->GetPreRealTemperature());
				}
				else
				{
					pDAQ->SetPreRealTemperature(pDAQ->GetRealTemperature());
					pDAQ->SetRealTemperature(dData);
				}
			}
			else if (pDAQ->GetSensorType() == READ_TYPE_THERMISTOR)
			{
				if (str == "-nan(ind)")
				{
					pDAQ->SetRealTemperature(pDAQ->GetPreRealTemperature());
				}
				else
				{
					pDAQ->SetPreRealTemperature(pDAQ->GetRealTemperature());
					pDAQ->SetRealTemperature(dData);
				}
			}
		}
	}


	//CWnd* pWnd = AfxGetApp()->GetMainWnd();

	//if (pWnd)
	//	::PostMessage(pWnd->GetSafeHwnd(), WM_DAQ_UPDATE, 0, 0);
	
}

int		CQueuingSerial::GetCommType()
{
	return m_nCommType;
}

void	CQueuingSerial::SetCommType(int nCommType)
{
	m_nCommType = nCommType;
}

UINT	CQueuingSerial::QueuingThread(LPVOID pParam)
{
	CQueuingSerial* pMgr = (CQueuingSerial*)pParam;

	if (!pMgr)
		return 0;

	while (pMgr->m_bLoopQueue)
	{
		TSerial* pComm = pMgr->GetQueue();

		if (NULL == pComm)
		{
			Sleep(1);

			continue;
		}

		int nCommType = pMgr->GetCommType();

		//TRACE(_T("%d\n"), nCommType);
		if (eDeviceTypeDaQ == nCommType)			pMgr->ParseDAQ(pComm);
		if (eDeviceTypeChamber == nCommType)	pMgr->ParseChamber(pComm);

		if (pComm)
		{
			delete pComm;

			pComm = NULL;
		}

		Sleep(1);
	}

	return 0;
}