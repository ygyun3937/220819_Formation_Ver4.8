#include "stdafx.h"
#include "CCommPid.h"
#include "Channel.h"
#include "MgrChannel.h"
#include "CMgrPid.h"
#include "PnxUtil.h"
#include "MgrCommData.h"

CCommPid::CCommPid()
{
	Initialize();
}

CCommPid::~CCommPid()
{
	Dispose();
}

// -------------------------------------------------------------------------------------------------

void CCommPid::Initialize()
{
	InitializeVariables();
}

void CCommPid::InitializeVariables()
{
	m_pStandardModbus = new CStandardModbus();
	m_AdvantechModbus = new CAdvantechModbus();

	m_bFirstReceive = false;

	m_pQueuingThread = NULL;
}

bool CCommPid::Dispose()
{
	CloseDevice();

	return true;
}

// -----------------------------------------------------------------------------

void CCommPid::StartQueueThread()
{
	if (NULL == m_pQueuingThread) {
		m_pQueuingThread = AfxBeginThread(QueuingThread, this);
	}
}

void CCommPid::StopQueueThread()
{
	m_bLoopQueue = FALSE;

	if (m_pQueuingThread)
	{
		::WaitForSingleObject(m_pQueuingThread->m_hThread, INFINITE);

		m_pQueuingThread = NULL;
	}

	// Thread 종료 후 Queue 비우자
	while (!m_Queue.IsEmpty()) {
		delete m_Queue.RemoveHead();
	}
}

UINT CCommPid::QueuingThread(LPVOID pParam)
{
	CCommPid* pMgr = (CCommPid*)pParam;

	if (!pMgr) {
		return 0;
	}

	TPid tPid;

	while (pMgr->m_bLoopQueue)
	{
		TPid* pQueuePid = pMgr->GetQueue();
		if (NULL == pQueuePid)
		{
			Sleep(1);
			continue;
		}

		tPid.strTime = pQueuePid->strTime;
		tPid.strData = pQueuePid->strData;
		tPid.nDeviceNo = pQueuePid->nDeviceNo;
		tPid.nSize = pQueuePid->nSize;

		//memcpy(&tEthernet.pszComm, pComm->pszComm, sizeof(pComm->pszComm));		// 용도는??

		DELETE_POINTER(pQueuePid);

		if (eDeviceTypePid == pMgr->GetCommType())
		{

		}

		Sleep(1);
	}

	return 0;
}

// -----------------------------------------------------------------------------

void CCommPid::AddQueue(TPid * pNewElement)
{
	m_csPush.Lock();

	if (m_Queue.GetCount() != 0 && m_Queue.GetCount() % 10 == 0) {
	//	SYSLOG(Dbg, "CCommPid::GetQueue - %d", m_Queue.GetCount());
	}

	m_Queue.AddTail(pNewElement);

	m_csPush.Unlock();
}

TPid* CCommPid::GetQueue()
{
	m_csPush.Lock();

	TPid* pPid = NULL;

	if (FALSE == m_Queue.IsEmpty()) {
		pPid = m_Queue.RemoveHead();
	}

	m_csPush.Unlock();

	return pPid;
}

// -------------------------------------------------------------------------------------------------

int CCommPid::OpenDevice()
{
	int nResultValue = -1;
	m_strDeviceType = "ADVANTECH";
	if (m_strDeviceType.CompareNoCase(_T("ADVANTECH")) == 0)
	{
		nResultValue = OpenAdvantechModbus();
	}
	else if (m_strDeviceType.CompareNoCase(_T("STANDARD")) == 0)
	{
		nResultValue = ConnectStandardModbus();
	}

	if (nResultValue == 0)
	{
		m_bInitialize = true;
	}

	return nResultValue;
}

int CCommPid::OpenDevice(CString strIpAddress, int nPort)
{
	int nResultValue = -1;

	m_strDeviceType = _T("ADVANTECH");
	m_strIpAddress = strIpAddress;
	m_nPortNo = nPort;
	
	if (m_strDeviceType.CompareNoCase(_T("ADVANTECH")) == 0)
	{
		nResultValue = OpenAdvantechModbus();
	}
	else if (m_strDeviceType.CompareNoCase(_T("STANDARD")) == 0)
	{
		nResultValue = ConnectStandardModbus();
	}

	if (nResultValue == 0)
	{
		m_bInitialize = true;
	}
	
	return nResultValue;
}

int CCommPid::CloseDevice()
{
	int nResultValue = -1;

	if (m_strDeviceType.CompareNoCase(_T("ADVANTECH")) == 0)
	{
		nResultValue = DisconnectAdvantechModbus();
	}
	else if (m_strDeviceType.CompareNoCase(_T("STANDARD")) == 0)
	{
		nResultValue = DisconnectStandardModbus();
	}

	return nResultValue;
}

int CCommPid::ConnectDevice()
{
	int nResultValue = -1;

	if (m_strDeviceType.CompareNoCase(_T("ADVANTECH")) == 0)
	{
		nResultValue = ConnectAdvantechModbus();
	}
	else if (m_strDeviceType.CompareNoCase(_T("STANDARD")) == 0)
	{
		nResultValue = ConnectStandardModbus();
	}

	return nResultValue;
}

int CCommPid::DisconnectDevice()
{
	return 0;
}

int CCommPid::GetDeviceChannelNo(int sourceChannelIndex)
{
	for (int count = 0; count < m_strChannelArray.GetCount(); count++)
	{
		CString tempValue = m_strChannelArray.GetAt(count);
		if ( sourceChannelIndex == atoi(tempValue) )
		{
			return count;
		}
	}
	return -1;
}

// -------------------------------------------------------------------------------------------------
// STANDARD

int CCommPid::OpenStandardModbus()
{
	if (m_pStandardModbus == nullptr) {
		return -1;
	}

	return m_pStandardModbus->OpenModbus(m_strIpAddress, m_nPortNo, 0);
}

int CCommPid::CloseStandardModbus()
{
	if (m_pStandardModbus == nullptr) {
		return -1;
	}

	return 0;
}

int CCommPid::ConnectStandardModbus()
{
	if (m_pStandardModbus == nullptr) {
		return -1;
	}

	return m_pStandardModbus->ConnectServer();
}

int CCommPid::DisconnectStandardModbus()
{
	if (m_pStandardModbus == nullptr) {
		return -1;
	}

	return m_pStandardModbus->DisconnectServer();
}

// -------------------------------------------------------------------------------------------------
// ADVANTECH

int CCommPid::OpenAdvantechModbus()
{
	if (m_AdvantechModbus == nullptr) {
		return -1;
	}
	
	return m_AdvantechModbus->OpenModbus(m_strIpAddress, m_nPortNo, m_nDeviceIndex);	
}

int CCommPid::CloseAdvantechModbus()
{
	if (m_AdvantechModbus == nullptr) {
		return -1;
	}

	return m_AdvantechModbus->CloseModbus();
}

int CCommPid::ConnectAdvantechModbus()
{
	if (m_AdvantechModbus == nullptr) {
		return -1;
	}

	return m_AdvantechModbus->ConnectServer();
}

int CCommPid::DisconnectAdvantechModbus()
{
	if (m_AdvantechModbus == nullptr) {
		return -1;
	}
	
	return m_AdvantechModbus->DisconnectServer();
}

// -----------------------------------------------------------------------------

/**
 * .
 *
 * METHOD_NAME : SetAITypeCode
 * METHOD_TYPE : -
 * DESCRIPTION : 2. AI type
 *				   7 in hex → 4~20mA
 *				   8 in hex → +/- 10V
 *				   D in hex → 0~20mA
 * \return
 */
long CCommPid::SetAITypeCode(int nChannelNo, int nTypeCode)
{
	return m_AdvantechModbus->SetForceHoldRegAITypeCode(nChannelNo, nTypeCode);
}

/**
 * .
 * 
 * METHOD_NAME : SetAOValue
 * METHOD_TYPE : -
 * DESCRIPTION : 
 * \return 
 */
long CCommPid::SetAOValue(int nChannelNo, float fValue)
{
	// CAUTION : ADAM-6024 - AO CHANNEL : 0, 1
	INT16 setValue = fValue * 409.5;
	return m_AdvantechModbus->SetForceHoldRegAOValue(nChannelNo, setValue);
}

/**
 * .
 * 
 * METHOD_NAME : SetAOTypeCode
 * METHOD_TYPE : -
 * DESCRIPTION : 3. AO type
*					0 in hex → 0~20mA
*					1 in hex → 4~20mA
*					2 in hex → 0~10V
 * \return 
 */
long CCommPid::SetAOTypeCode(int nChannelNo, int nTypeCode)
{
	return m_AdvantechModbus->SetForceHoldRegAOTypeCode(nChannelNo, nTypeCode);
}

/**
 * .
 * 
 * METHOD_NAME : SetAOStartupValue
 * METHOD_TYPE : -
 * DESCRIPTION :  ADAM-6024 - AO CHANNEL : 0, 1
 * \return 
 */
long CCommPid::SetAOStartupValue(int nChannelNo, float fValue)
{
	INT16 setValue = fValue * 409.5;
	return m_AdvantechModbus->SetForceHoldRegAOStartupValue(nChannelNo, setValue);
}

long CCommPid::SetAOValueAll(float fValue)
{
	// CAUTION : ADAM-6024 - AO CHANNEL : 0, 1
	INT16 setValue = fValue * 409.5;
	return m_AdvantechModbus->SetForceHoldRegAOValueAll(setValue);
}

long CCommPid::SetAOValue_Select_All(float fValue)
{
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();
	auto Pos = listChannel.GetHeadPosition();


	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		//선택된 채널이면
		if (pChannel->GetSelected() == true)
		{
			pChannel->Set_CalVoltSetting(fValue);
			SetAOValue(0, fValue);
		}
	}

	return 0;
}
