// SockProc.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PnxCycler.h"
#include "SockProc.h"
#include "MgrComm.h"
#include "MgrChannel.h"
#include "PnxUtil.h"
#include "CMgrChamber.h"
#include "EthernetDefine.h"
#include "MgrConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma warning(disable:4333)

CSockProc::CSockProc()
{
	CreateSocket();
}

CSockProc::~CSockProc()
{
	CloseSocket();
}

void	CSockProc::CreateSocket()
{
	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return;

	std::vector<SDevice*> vecValue;
	pMgr->GetDeivceFromType(eDeviceTypeIFBoard, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			CClientSocket* pEthernet = new CClientSocket;

			pEthernet->SetServerIP(lpDevice->strIP);
			pEthernet->SetServerPort(lpDevice->nPort);
			pEthernet->SetPrimaryKey(lpDevice->nPrimaryKey);
			pEthernet->SetChannelArray(lpDevice->arrChannel);
			pEthernet->SetBoardIndex(lpDevice->nIndex);

			m_listSocket.AddTail(pEthernet);
		}

		++iter;
	}
	
	//PlcInterface 추가
	pMgr->GetDeivceFromType(eDeviceTypePlcInterface, vecValue);

	iter = vecValue.begin();
	iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			CClientSocket* pEthernet = new CClientSocket;

			pEthernet->SetServerIP(lpDevice->strIP);
			pEthernet->SetServerPort(lpDevice->nPort);
			pEthernet->SetPrimaryKey(lpDevice->nPrimaryKey);
			pEthernet->SetChannelArray(lpDevice->arrChannel);
			pEthernet->SetBoardIndex(lpDevice->nIndex);

			m_listSocket.AddTail(pEthernet);
		}

		++iter;
	}
}

void	CSockProc::CloseSocket()
{
	while (!m_listSocket.IsEmpty())
	{
		CClientSocket* pEthernet = m_listSocket.RemoveHead();

//modfiy by sjh 2019.09.19 memory rick start
		pEthernet->StopLinkThread();
//modfiy by sjh 2019.09.19 memory rick end

		if (NULL == pEthernet)
			continue;

		pEthernet->CloseClientSocket();

		DELETE_POINTER(pEthernet);
	}
}

void	CSockProc::StopThread()
{
	auto Pos = m_listSocket.GetHeadPosition();

	while (Pos)
	{
		CClientSocket* pEthernet = m_listSocket.GetNext(Pos);

		if (NULL == pEthernet)
			continue;

		pEthernet->StopThread();
	}
}

void	CSockProc::SocketConnect()
{
	auto Pos = m_listSocket.GetHeadPosition();

	while (Pos)
	{
		CClientSocket* pEthernet = m_listSocket.GetNext(Pos);

		if (NULL == pEthernet)
			continue;

 		pEthernet->ConnectToServer();

		Sleep(20);
	}
}

#if UPDATE_CP_MODE_TURBY
bool	CSockProc::SendBoardTypeD(int nBoardNumber, BYTE byChannel, int nReserveStepIndex)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (NULL == pSocket)
		return false;

#if TEST_OFF_MODE
	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;
	//if (FALSE == pSocket->GetConnect())
	//	return false;
#endif
	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return false;

	int nChannelNumber = pMgr->GetChannelNumber(eDeviceTypeIFBoard, nBoardNumber, byChannel);

	if (nChannelNumber < 0) 
		return false;

	BYTE bySamplingTime = 0x01;
	BYTE bySendPeriod = 0x03;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

	if (NULL == pChannel)
		return false;

	CList< CStep*, CStep* >& listStep = pChannel->GetMgr()->GetStepList();

	char szData[MAX_BUF_SIZE] = { 0, };

/*	char* szData = NULL;

	try {
		//char szData[MAX_BUF_SIZE*10] = { 0, };
		szData = new char[MAX_BUF_SIZE * 10];
		ZeroMemory(szData, MAX_BUF_SIZE * 10);
	}
	catch (CMemoryException& e) {
		AfxMessageBox(e.ReportError());
		return false;
	}
	
조성복	*/
	UINT  szData_index = 0;
	memset(szData, NULL, MAX_BUF_SIZE);

	//lyw12_1 : 아래 주석처리
	//pChannel->SetSendDataPacket(true);

	/* 여기는 한번만 만들어 주면 되는 곳이다. */
	/* HEAD */
	/* 0 , 1 */
	szData[szData_index] = '@';
	szData_index++;
	szData[szData_index] = 'D';
	szData_index++;

	/* Protocol Byte Length */
	/* 2 , 3 */
	int nStepRepeat = 0;;
	int nStepCount = listStep.GetSize();
	nStepRepeat = nStepCount;

	// 2 를 더해주는 의미를 모르겠다.
	nStepCount = (listStep.GetSize() + 2) >> 8;
	//nStepCount = (listStep.GetSize()) >> 8;

	szData[szData_index] = (char)nStepCount;
	szData_index++;
	szData[szData_index] = (listStep.GetSize() + 2) >> 0 & 0xFF;
	szData_index++;

	/* Channel Index */	
	/* 4,5 */
	szData[szData_index] = (BYTE)byChannel >> 8 & 0xFF;
	szData_index++;
	szData[szData_index] = (BYTE)byChannel >> 0 & 0xFF;
	szData_index++;

	auto Pos = listStep.GetHeadPosition();
	//TRACE(_T(" tx %d  %d\n"), nChannelNumber,byChannel);
	/* STX  관련 문서는 없으나 구두로 처리 하였음.*/
	/* STX */
	/* 누군가의 요청에 의해 이렇게 코딩되어있단다. */
#if TEST_OFF_MODE
	szData_index = 38;
	szData[szData_index] = 0x00;
	szData_index++;
	szData[szData_index] = 0x00;
	szData_index++;
	szData[szData_index] = 0x00;
	szData_index++;
	szData[szData_index] = 0x01;
	
	/* 특정 Start point 를 지정할때 사용한다. */
	if (nReserveStepIndex >= 1)
	{
		szData_index = 44;
		szData[szData_index] = (BYTE)nReserveStepIndex >> 8 & 0xFF;
		szData_index = 45;
		szData[szData_index] = (BYTE)nReserveStepIndex >> 0 & 0xFF;
		szData_index = 104;
	}
	else
	{
		szData_index = 44;
		szData[szData_index] = (BYTE)0 >> 8 & 0xFF;
		szData_index = 45;
		szData[szData_index] = (BYTE)1 >> 0 & 0xFF;
		szData_index = 104;
	}
#endif
	int nStep = 1;
	//20201029 turby Send D Data view point
	while(Pos)
	{
		CStep* pStep = listStep.GetNext(Pos);

		//TRACE(_T(" 12 %d\n"), turby_++);

		if (pStep == NULL)
			continue;

		int nStepType = 0;

		m_nStepType = pStep->GetStepType();

		switch (pStep->GetStepType())
		{
		case eIFBoardStepTypeRest: nStepType = eIFBoardCommandRest;		break;
		case eIFBoardStepTypeOcv: nStepType = eIFBoardCommandRest;		break;
		case eIFBoardStepTypeCharge: nStepType = eIFBoardCommandCharge;	break;
		case eIFBoardStepTypeDischarge: nStepType = eIFBoardCommandDischarge;	break;
		}

		/* I/F Command *//*6,7*/
		szData[szData_index] = nStepType >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = nStepType >> 0 & 0xFF;
		szData_index++;

		/* Step Mode *//*8,9*/
		int nEQMode = pStep->GetStepMode();		//SEND_HEADER_1

		szData[szData_index] = nEQMode >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = nEQMode >> 0 & 0xFF;
		szData_index++;
		
		/* Operation Mode *//*10,11*/
		if (eIFBoardOperationNone == pStep->GetOperationMode())
		{
			switch (pStep->GetStepType())
			{
			case eIFBoardStepTypeCharge:
				szData[szData_index] = eIFBoardOperationChargeCC >> 8 & 0xFF;
				szData_index++;
				szData[szData_index] = eIFBoardOperationChargeCC >> 0 & 0xFF;
				szData_index++;
				break;
			case eIFBoardStepTypeDischarge:
				szData[szData_index] = eIFBoardOperationDischargeCC >> 8 & 0xFF;
				szData_index++;
				szData[szData_index] = eIFBoardOperationDischargeCC >> 0 & 0xFF;
				szData_index++;
				break;
			case eIFBoardStepTypeImpedance:
				szData[szData_index] = eIFBoardOperationDischargeCC >> 8 & 0xFF;
				szData_index++;
				szData[szData_index] = eIFBoardOperationDischargeCC >> 0 & 0xFF;
				szData_index++;
				break;
			case eIFBoardStepTypePogoPinTest:
				szData[szData_index] = eIFBoardOperationChargeCC >> 8 & 0xFF;
				szData_index++;
				szData[szData_index] = eIFBoardOperationChargeCC >> 0 & 0xFF;
				szData_index++;
				break;
			default:
				szData_index += 2;
			}
		}
		else
		{
			szData[szData_index] = pStep->GetOperationMode() >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = pStep->GetOperationMode() >> 0 & 0xFF;
			szData_index++;
		}

		/* Row Data Collection Time Period *//*12,13*/
		szData[szData_index] = bySamplingTime >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = bySamplingTime >> 0 & 0xFF;
		szData_index++;

		/* To Cycler Message Send Time Period *//*14,15*/
		szData[szData_index] = bySendPeriod >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = bySendPeriod >> 0 & 0xFF;
		szData_index++;

		/* Step Index *//*16,17*/
		szData[szData_index] = pStep->GetStepIndex() >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepIndex() >> 0 & 0xFF;
		szData_index++;

		//TRACE(_T("%d %d\n"), szData[szData_index-1], szData[szData_index-2]);

		/* Cycle Loop Count *//*18,19*/
		szData[szData_index] = pStep->GetCycleLoopCount() >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetCycleLoopCount() >> 0 & 0xFF;
		szData_index++;

		/* Go to Step Index *//*20,21*/
		szData[szData_index] = pStep->GetLoopNextCycle() >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetLoopNextCycle() >> 0 & 0xFF;
		szData_index++;

		/* Return Step Index *//*22,23*/
		szData[szData_index] = pStep->GetPrevStepIndex() >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetPrevStepIndex() >> 0 & 0xFF;
		szData_index++;

		/* Ref 와 End Condition Voltage 를 같이 구한다.*/
		/* Ref Voltage 1,2 *//*24,25,26,27*/
		if (eIFBoardStepTypeCharge == pStep->GetStepType())
		{
			szData[szData_index] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;

			// By JSK - 2018.06.19 기존 CC/CV 모드 일때 CV 에 대한 동작 값이 없었다
			//전력전자 미적용
			/* Voltage End Condition 1,2 *//*28,29,30,31*/
			SScheduleComplete* lpScheduleComplete = pStep->GetScheduleCompleteFromName("HV");

			if (lpScheduleComplete)
			{
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else //20201102 turby 추훙에 default 값 적용하자
			{
				szData_index += 4;
			}
		}
		else if (eIFBoardStepTypeDischarge == pStep->GetStepType())   /* Ref Voltage 1,2 *//*24,25,26,27*/
		{
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
			
			//By JSK - 2018.06.19 기존 CC/CV 모드 일때 CV 에 대한 동작 값이 없었다
			//전력전자 미적용
			/* Voltage End Condition 1,2 *//*28,29,30,31*/
			SScheduleComplete* lpScheduleComplete = pStep->GetScheduleCompleteFromName("LV");

			if (lpScheduleComplete)
			{
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else //20201102 turby 추훙에 default 값 적용하자
			{
				szData_index += 4;
			}
		}
		else if (eIFBoardStepTypeImpedance == pStep->GetStepType())
		{
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;

			// By JSK - 2018.06.19 기존 CC/CV 모드 일때 CV 에 대한 동작 값이 없었다
			// 전력전자 미적용
			/* Voltage End Condition 1,2 *//*28,29,30,31*/
			SScheduleComplete* lpScheduleComplete = pStep->GetScheduleCompleteFromName("LV");

			if (lpScheduleComplete)
			{
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else //20201102 turby 추훙에 default 값 적용하자
			{
				szData_index += 4;
			}
		}
		else //20201102 turby 추훙에 default 값 적용하자
		{
			szData_index += 8;
		}

		/* Voltage Step End Move Step Index *//*32,33*/
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteVoltage) >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteVoltage) >> 0 & 0xFF;
		szData_index++;

		CMgrConfig* pMgr = CMgrConfig::GetMgr();
		int nChannelCountInParallel = pMgr->GetParallelChannelCount(); // 병렬에 필요한 Channel 갯수 마스터 + 슬레이브

		//병렬 일때는 나누기 병렬 갯수 만큼 해줘서 보내야 됨 210916
		if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (eIFBoardOperationChargeCP == pStep->GetOperationMode() || eIFBoardOperationDischargeCP == pStep->GetOperationMode()
				|| eIFBoardOperationChargeCPCV == pStep->GetOperationMode() || eIFBoardOperationDischargeCPCV == pStep->GetOperationMode())
			{
				pStep->SetWatt(pStep->GetWatt() / nChannelCountInParallel);
			}
			else
			{
				if (pStep->GetStepType() == 1)
				{
					pStep->SetChargeCurrentAmpere(pStep->GetCurrentAmpere() / nChannelCountInParallel);
				}
				else
				{
					pStep->SetDisChargeCurrentAmpere(pStep->GetDisCurrentAmpere() / nChannelCountInParallel);
				}
			}
		}

		/* Ref Ampere / Ref Power 1,2 *//*34,35,36,37*/		  
		if (eIFBoardOperationChargeCP == pStep->GetOperationMode() || eIFBoardOperationDischargeCP == pStep->GetOperationMode())
		{ 
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
		}
		else if (eIFBoardOperationChargeCPCV == pStep->GetOperationMode() || eIFBoardOperationDischargeCPCV == pStep->GetOperationMode())
		{
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
		}
		else
		{
			if (pStep->GetStepType() == 1)
			{
				szData[szData_index] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else
			{
				szData[szData_index] = ((unsigned int)(pStep->GetDisCurrentAmpere() * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetDisCurrentAmpere() * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetDisCurrentAmpere() * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetDisCurrentAmpere() * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}

			//TRACE(_T("%d] %d %d %d %d\n"), szData_index - 5, szData[szData_index - 4], szData[szData_index - 3], szData[szData_index - 2], szData[szData_index - 1]);
		}
	
		/* Time End Condition 1,2 *//*38,39,40,41*/
		SScheduleComplete* lpScheduleCompleteTime = pStep->GetScheduleCompleteFromName("Ti");

		if (lpScheduleCompleteTime)
		{
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 16 >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 16 >> 0 & 0xFF;
			szData_index++;
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 0 >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 0 >> 0 & 0xFF;
			szData_index++;
		}
		else
		{
			szData_index += 4;
		}

		/* Cycle Index *//*42,43*/
		//20190709 루차장님 요청으로 해당 인덱스는 사이클 번호가 들어간다(페턴 주기 저값 기존에 사용 안했음)
		szData[szData_index] = pStep->GetCycleNumber() >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetCycleNumber() >> 0 & 0xFF;
		szData_index++;

		///////////////////////////////////////////////////////////////////////////////////////
		// Pattern Step 무한 반복 Setting 2021-05-07
		CCommData* commData;
		commData = new CCommData();
		if (commData->CheckPatternLoopStepChange(nStep, 0)==true)//pStep->GetStepNumber()
			pStep->SetStepEndIndex(eIFBoardCompleteTime, g_nPatternStartStep[0]);	

		DELETE_POINTER(commData);
		nStep++;

		///////////////////////////////////////////////////////////////////////////////////////
		/* Time Step End Move Step Index *//*44,45*/
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteTime) >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteTime) >> 0 & 0xFF;
		szData_index++;

		/* Ampere End Condition 1,2 *//*46,47,48, 49*/
		SScheduleComplete* lpScheduleCompleteCurrent = pStep->GetScheduleCompleteFromName("CUR");

		//병렬 일때는 나누기 병렬 갯수 만큼 해줘서 보내야 됨 210916
		if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (lpScheduleCompleteCurrent)
			{
				lpScheduleCompleteCurrent->fValue /= (float)nChannelCountInParallel;
			}
		}

		if (lpScheduleCompleteCurrent)
		{
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
		}
		else
		{
			szData_index += 4;
		}

		/* Ampere Step End Move Step Index *//*50,51*/
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteCurrent) >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteCurrent) >> 0 & 0xFF;
		szData_index++;

		/* Capacity End Condition 1,2 *//*52,53,54,55 */
		SScheduleComplete* lpScheduleCompleteCapacity = pStep->GetScheduleCompleteFromName("Ca");

		//병렬 일때는 나누기 병렬 갯수 만큼 해줘서 보내야 됨 210916
		if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (lpScheduleCompleteCapacity)
			{
				lpScheduleCompleteCapacity->fValue /= (float)nChannelCountInParallel;
			}
		}

		if (lpScheduleCompleteCapacity)
		{
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
		}
		else
		{
			szData_index += 4;
		}

		/* Capacity Step End Move Step Index *//*56,57*/
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteCapacity) >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteCapacity) >> 0 & 0xFF;
		szData_index++;

		/* Watt End Condition 1,2 *//*58,59,60,61*/
		SScheduleComplete* lpScheduleCompleteWatt = pStep->GetScheduleCompleteFromName("Po");

		//병렬 일때는 나누기 병렬 갯수 만큼 해줘서 보내야 됨 210916
		if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (lpScheduleCompleteWatt)
			{
				lpScheduleCompleteWatt->fValue /= (float)nChannelCountInParallel;
			}
		}

		if (lpScheduleCompleteWatt)
		{
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
		}
		else
		{
			szData_index += 4;
		}

		/* Watt Step End Move Step Index *//*62,63*/
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteWatt) >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteWatt) >> 0 & 0xFF;
		szData_index++;

		/* Cycle Voltage End Condition 1,2 *//*64,65, 66, 67*/
		CCycle* pCycle = pChannel->GetMgr()->GetAtCycle(pStep->GetCycleNumber()); //taek sk cycle 종료 조건 test 여기부터
		
		if (pCycle)
		{
			SScheduleComplete* lpScheduleCompleteVoltageMax = pCycle->GetScheduleCompleteFromName(_T("HV"));

			if (lpScheduleCompleteVoltageMax)
			{
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMax->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMax->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMax->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMax->fValue * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else
			{
				szData_index += 4;
			}
		}
		else
		{
			szData_index += 4;
		}
		
		/* Cycle Step End Move Step Index *//*68,69*/
		if (pCycle) 
		{
			szData[szData_index] = pCycle->GetCycleEndIndex(eCycleEndCondtionVoltage) >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = pCycle->GetCycleEndIndex(eCycleEndCondtionVoltage) >> 0 & 0xFF;
			szData_index++;
		}
		else
		{
			szData_index += 2;
		}

		/* Empty value 면 Next Step 의 값을 올리도록 한다. */
		/* Cycle Time End Condition 1,2 *//*70,71, 72, 73*/
		SScheduleComplete* lpScheduleCompleteTimeMax = pCycle->GetScheduleCompleteFromName(_T("Ti"));

		if (lpScheduleCompleteTimeMax)
		{
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 16 >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 16 >> 0 & 0xFF;
			szData_index++;
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 0 >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 0 >> 0 & 0xFF;
			szData_index++;
		}
		else
		{
			szData_index += 4;
		}
		/* Cycle End Move Step Index *//*74,75*/
		szData[szData_index] = pCycle->GetCycleEndIndex(eCycleEndCondtionTime) >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pCycle->GetCycleEndIndex(eCycleEndCondtionTime) >> 0 & 0xFF;
		szData_index++;

		/* Cycle Capacity End Condition 1,2 *//*76,77,78,79*/
		if (pCycle)
		{
			SScheduleComplete* lpScheduleCompleteCapacityMax = pCycle->GetScheduleCompleteFromName(_T("Ca"));

			if (lpScheduleCompleteCapacityMax)
			{
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacityMax->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacityMax->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacityMax->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacityMax->fValue * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else
			{
				szData_index += 4;
			}

			/* Cycle Capacity End Move Step Index *//*80,81*/
			szData[szData_index] = pCycle->GetCycleEndIndex(eCycleEndCondtionCapacity) >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = pCycle->GetCycleEndIndex(eCycleEndCondtionCapacity) >> 0 & 0xFF;
			szData_index++;
		}
		else
		{
			szData_index += 6;
		}

		/* Cycle Watt End Condition 1,2 *//*82,83,84,85*/			
		if (pCycle)
		{
			SScheduleComplete* lpScheduleCompleteWattMax = pCycle->GetScheduleCompleteFromName(_T("Po"));

			if (lpScheduleCompleteWattMax)
			{
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWattMax->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWattMax->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWattMax->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWattMax->fValue * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else
			{
				szData_index += 4;
			}

			/* Cycle End Move Step Index *//*86,87*/
			szData[szData_index] = pCycle->GetCycleEndIndex(eCycleEndCondtionWatt) >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = pCycle->GetCycleEndIndex(eCycleEndCondtionWatt) >> 0 & 0xFF;
			szData_index++;
		}
		else
		{
			szData_index += 6;
		}

//#pragma region Sync Step Temperature Value, Time 사용안함 88,89,90,91,92,93
		///* Sync Step Temperature Value *//*88,89*/
		///* 온도값은 무조건 들어가게 한다. */

		///* 싱글모드로 진행할 시 챔버의 첫번째 채널에만 온도값을 설정하고,
		//   그 이외의 채널에는 0 으로 셋팅하여 온도의 영향을 받지 않도록 한다.
		//   그래서 94,95 번 인덱스의 값을 먼저 사용한다. */

		//int nChamberSetTemp = (int)abs(pStep->GetChamberSetTemp() * 10);

		//// Channel 간 Sync mode 선택시
		//if (true == pChannel->GetTempSync())
		//{
		//	if (pStep->GetChamberSetTemp() < 0.000f)
		//	{
		//		szData[szData_index] = (BYTE)nChamberSetTemp >> 8 & 0xFF; //챔버 셋팅 온도
		//		szData[szData_index] = szData[szData_index] |= (1 << 7);
		//		szData_index++;
		//		szData[szData_index] = (BYTE)nChamberSetTemp >> 0 & 0xFF; //챔버 셋팅 온도
		//		szData_index++;
		//	}
		//	else
		//	{
		//		szData[szData_index] = (BYTE)(nChamberSetTemp >> 8) & 0xFF; //챔버 셋팅 온도
		//		szData_index++;
		//		szData[szData_index] = (BYTE)(nChamberSetTemp & 0xfF);
		//		szData_index++;
		//	}
		//}
		//else // Channel 간 Single mode 선택시
		//{
		//	if (nChannelNumber == 0 || nChannelNumber == 2)
		//	{
		//		if (pStep->GetChamberSetTemp() < 0.000f)
		//		{
		//			szData[szData_index] = (BYTE)nChamberSetTemp >> 8 & 0xFF; //챔버 셋팅 온도
		//			szData[szData_index] = szData[szData_index] |= (1 << 7);
		//			szData_index++;
		//			szData[szData_index] = (BYTE)nChamberSetTemp >> 0 & 0xFF; //챔버 셋팅 온도
		//			szData_index++;
		//		}
		//		else
		//		{
		//			szData[szData_index] = (BYTE)(nChamberSetTemp >> 8) & 0xFF; //챔버 셋팅 온도
		//			szData_index++;
		//			szData[szData_index] = (BYTE)(nChamberSetTemp & 0xfF);
		//			szData_index++;
		//		}
		//	}
		//	else
		//	{
		//		szData[szData_index] = 0x00; //챔버 셋팅 온도
		//		szData_index++;
		//		szData[szData_index] = 0x00;
		//		szData_index++;
		//	}
		//}


		///* Sync Step Temperature Time_1 *//*90,91,92,93*/
		//bool bExistDwellTime = false;

		//CString strDwellTime = pStep->GetChamberDelayTime();

		//if (strcmp(strDwellTime, "") == 0 || strcmp(strDwellTime, "00:00:00:00") == 0)
		//	bExistDwellTime = false;
		//else
		//	bExistDwellTime = true;

		//strDwellTime.Replace(":", "");

		//if (atoi(strDwellTime) > 0)
		//	bExistDwellTime = true;
		//else
		//	bExistDwellTime = false;

		//if (true == bExistDwellTime && true == pChannel->GetTempSync())
		//{
		//	szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 16 >> 8 & 0xFF;
		//	szData_index++;
		//	szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 16 >> 0 & 0xFF;
		//	szData_index++;
		//	szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 0 >> 8 & 0xFF;
		//	szData_index++;
		//	szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 0 >> 0 & 0xFF;
		//	szData_index++;

		//	if ((int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) > 0 && true == pChannel->GetTempSync())
		//	{
		//		pStep->SetEndSync(1);
		//		pChannel->SetEndSync(1);
		//	}
		//	else
		//	{
		//		pStep->SetEndSync(0);
		//		pChannel->SetEndSync(0);
		//	}

		//	if (pChannel->GetSyncChamber())
		//	{
		//		pStep->SetEndSync(0);
		//		pChannel->SetEndSync(0);
		//	}
		//	else
		//	{
		//		pStep->SetEndSync(1);
		//		pChannel->SetEndSync(1);
		//	}
		//}
		//else
		//{
		//	szData_index += 4;
		//}
//#pragma endregion

		/* Cycle Voltage End Condition 1,2 *//*88,89, 90,91 Cycle Low Limit 사용으로 변경 2020.12.10*/
		
		if (pCycle)
		{
			SScheduleComplete* lpScheduleCompleteVoltageMin = pCycle->GetScheduleCompleteFromName(_T("LV"));

			if (lpScheduleCompleteVoltageMin)
			{
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else
			{
				szData_index += 4;
			}
		}
		else
		{
			szData_index += 4;
		}

		/* Cycle Step End Move Step Index *//*92,93*/
		if (pCycle) 
		{
			szData[szData_index] = pCycle->GetCycleEndIndex(eCycleEndCondtionLowVoltage) >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = pCycle->GetCycleEndIndex(eCycleEndCondtionLowVoltage) >> 0 & 0xFF;
			szData_index++;
		}
		else
		{
			szData_index += 2;
		}
		

		/* Sync Step Info *//*94,95*/
		
		// 챔버내 채널간 Sync 를 사용할지 말지를 결정한다. GUI 에서 선택한 값이 들어온다.
		// tempsync 이지만, stepsync 값으로 사용한다.
		if (true == pChannel->GetStepSync() || true == pChannel->GetTempSync())
		{
			szData[szData_index] = (BYTE)eScheduleSyncModeStep >> 8 & 0XFF;
			szData_index++;
			szData[szData_index] = (BYTE)eScheduleSyncModeStep >> 0 & 0XFF;
			szData_index++;
		}
		else
		{
			szData[szData_index] = (BYTE)pChannel->GetSyncType() >> 0 & 0XFF;	// 2020-12-03
			szData_index++;
			szData[szData_index] = (BYTE)pChannel->GetSyncType() >> 0 & 0XFF;
			szData_index++;
		}

		/* Sync Temp Info *//*96,97*/
		/* Editor 에서 받은 temp sync value 이다.*/

		szData[szData_index] = (BYTE)0 >> 8 & 0xFF; //승온
		szData_index++;
		szData[szData_index] = (BYTE)0 >> 0 & 0xFF; //승온
		szData_index++;

		//if (true == pChannel->GetStepSync() || true == pChannel->GetTempSync())
		//{
		//	szData[szData_index] = (BYTE)1 >> 8 & 0xFF; //승온
		//	szData_index++;
		//	szData[szData_index] = (BYTE)1 >> 0 & 0xFF; //승온
		//	szData_index++;
		//}
		//else
		//{
		//	szData[szData_index] = (BYTE)0 >> 8 & 0xFF; //승온
		//	szData_index++;
		//	szData[szData_index] = (BYTE)0 >> 0 & 0xFF; //승온
		//	szData_index++;
		//}

		//TRACE(_T("tx sync ch %d ] %d %d %d %d\n"), byChannel, szData[szData_index - 4], szData[szData_index - 3], szData[szData_index - 2], szData[szData_index - 1]);

		//문의??
		if (true == pStep->GetChamberElevatedTemperature() && true == pChannel->GetTempSync())
		{
			pChannel->SetChamberElevatedTemperature(pStep->GetChamberElevatedTemperature());
		}

		/* Sync Step End Info *//*98,99*/ /* Reserved */
		//int turby_Sync_temp_info = pStep->GetChamberElevatedTemperature();// .m_bChamberElevatedTemperature();
		//int turby_Sync_Step_info = pChannel->GetTempSync();

		szData[szData_index] = 0;
		szData_index++;
		szData[szData_index] = 0;   //test 
		szData_index++;

		/* CP Ampere Limit 1,2 *//*100,101,102,103*/
		if (eIFBoardStepTypeCharge == pStep->GetStepType() || eIFBoardStepTypeDischarge == pStep->GetStepType() || eIFBoardStepTypeExternalPattern == pStep->GetStepType())
		{
			if (eIFBoardStepModeCP == pStep->GetStepMode() || eIFBoardStepModeCPCV == pStep->GetStepMode())
			{  
				if (eIFBoardStepTypeCharge == pStep->GetStepType())
				{
					int turby_curr = (int)(pStep->GetCurrentAmpere() * 1000);

					szData[szData_index] = (BYTE)(turby_curr >> 16 >>8) & 0xFF;
					szData_index++;
					szData[szData_index] = ((BYTE)(turby_curr>>16 >> 0) & 0xFF);
					szData_index++;
					szData[szData_index] = (BYTE)(turby_curr >> 0 >> 8) & 0xFF;
					szData_index++;
					szData[szData_index] = (BYTE)(turby_curr >> 0 >> 0) & 0xFF;
					szData_index++;
				}
				else if (eIFBoardStepTypeDischarge == pStep->GetStepType())
				{
					int turby_curr = (int)(pStep->GetDisCurrentAmpere() * 1000);
					szData[szData_index] = (BYTE)(turby_curr >> 16 >> 8) & 0xFF;
					szData_index++;
					szData[szData_index] = (BYTE)(turby_curr) >> 16 >> 0 & 0xFF;
					szData_index++;
					szData[szData_index] = (BYTE)(turby_curr >> 0 >> 8) & 0xFF;
					szData_index++;
					szData[szData_index] = (BYTE)(turby_curr >> 0 >> 0) & 0xFF;
					szData_index++;
				}
				else
				{
					szData[szData_index] = 0;
					szData_index++;
					szData[szData_index] = 0;
					szData_index++;
					szData[szData_index] = 0;
					szData_index++;
					szData[szData_index] = 0;
					szData_index++;
				}
			}
			else if (eIFBoardStepTypeExternalPattern == pStep->GetStepPattern())
			{
				int hi_curr = (int)(pStep->GetCurrentAmpere() * 1000);
				int low_curr = (int)(pStep->GetDisCurrentAmpere() * 1000);

 				szData[szData_index] = (BYTE)(hi_curr >> 16 >> 8) & 0xFF;
				szData_index++;
				szData[szData_index] = ((BYTE)(hi_curr >> 16 >> 0) & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)(low_curr >> 0 >> 8) & 0xFF;
				szData_index++;
				szData[szData_index] = (BYTE)(low_curr >> 0 >> 0) & 0xFF;
				szData_index++;
			}
			else
			{
				szData[szData_index] = 0;
				szData_index++;
				szData[szData_index] = 0;
				szData_index++;
				szData[szData_index] = 0;
				szData_index++;
				szData[szData_index] = 0;
				szData_index++;
			}
		}
		else
		{
			szData_index += 4;
		}
		//TRACE(_T("%d	%d\n"), rep_cnt, szData_index);
	}
	
	/* ETX */
#if 0 // org
	szData[szData_index + 16] = 0xFF;
	szData[szData_index + 17] = 0xFF;
	szData[szData_index + 18] = 0x00;
	szData[szData_index + 19] = 0x00;

	szData[szData_index + 38] = 0x00;
	szData[szData_index + 39] = 0x00;
	szData[szData_index + 40] = 0x00;
	szData[szData_index + 41] = 0x01;
#else


	szData[szData_index + 10] = 0xFF;
	szData[szData_index + 11] = 0xFF;
	szData[szData_index + 12] = 0x00;
	szData[szData_index + 13] = 0x00;

	szData[szData_index + 32] = 0x00;
	szData[szData_index + 33] = 0x00;
	szData[szData_index + 34] = 0x00;
	szData[szData_index + 35] = 0x01;
#endif

	szData_index += 98;

#if TEST_OFF_MODE
	//int nRet = pSocket->SendToServer(szData, szData_index);
	int nRet = 0;
	// Reconnect 추가. 2021-03-03
#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, szData_index);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else

/* GM 테스트 220203
	vector<char> test;

	nRet = pSocket->SendToServer(reinterpret_cast<char*>(test.data()), szData_index);*/

	nRet = pSocket->SendToServer(szData, szData_index);
    //delete[] szData; 조성복
#endif // 0

	

	//lyw12_1 : 아래 주석처리
	//pChannel->SetSendDataPacket(false);

	if (nRet > 0)
	{
		nChannelNumber = CalcChannelNumber(byChannel, nBoardNumber);

		SYSLOG(Dbg, "[NEW_SEND_BOARD_TYPE_D DATA][BOARD:%d][CH:0x%02x][CH:%d][Length : %d] SEND DATA", nBoardNumber, byChannel, nChannelNumber, szData_index);

		return true;
	}
#endif

	return false;
}
bool CSockProc::SendBoardTypeD_Formation(int nBoardNumber, BYTE byChannel, int nReserveStepIndex)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (NULL == pSocket)
		return false;

#if TEST_OFF_MODE
	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;
	//if (FALSE == pSocket->GetConnect())
	//	return false;
#endif
	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return false;

	int nChannelNumber = pMgr->GetChannelNumber(eDeviceTypeIFBoard, nBoardNumber, byChannel);

	if (nChannelNumber < 0)
		return false;

	BYTE bySamplingTime = 0x01;
	BYTE bySendPeriod = 0x03;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

	if (NULL == pChannel)
		return false;

	CList< CStep*, CStep* >& listStep = pChannel->GetMgr()->GetStepList();

	char szData[MAX_BUF_SIZE] = { 0, };

	/*	char* szData = NULL;

		try {
			//char szData[MAX_BUF_SIZE*10] = { 0, };
			szData = new char[MAX_BUF_SIZE * 10];
			ZeroMemory(szData, MAX_BUF_SIZE * 10);
		}
		catch (CMemoryException& e) {
			AfxMessageBox(e.ReportError());
			return false;
		}

	조성복	*/
	UINT  szData_index = 0;
	memset(szData, NULL, MAX_BUF_SIZE);

	//lyw12_1 : 아래 주석처리
	//pChannel->SetSendDataPacket(true);

	/* 여기는 한번만 만들어 주면 되는 곳이다. */
	/* HEAD */
	/* 0 , 1 */
	szData[szData_index] = '@';
	szData_index++;
	szData[szData_index] = 'D';
	szData_index++;

	/* Protocol Byte Length */
	/* 2 , 3 */
	int nStepRepeat = 0;;
	int nStepCount = listStep.GetSize();
	nStepRepeat = nStepCount;

	// 2 를 더해주는 의미를 모르겠다.
	nStepCount = (listStep.GetSize() + 2) >> 8;
	//nStepCount = (listStep.GetSize()) >> 8;

	szData[szData_index] = (char)nStepCount;
	szData_index++;
	szData[szData_index] = (listStep.GetSize() + 2) >> 0 & 0xFF;
	szData_index++;

	/* Channel Index */
	/* 4,5 */
	szData[szData_index] = (BYTE)byChannel >> 8 & 0xFF;
	szData_index++;
	szData[szData_index] = (BYTE)byChannel >> 0 & 0xFF;
	szData_index++;

	auto Pos = listStep.GetHeadPosition();
	//TRACE(_T(" tx %d  %d\n"), nChannelNumber,byChannel);
/* STX  관련 문서는 없으나 구두로 처리 하였음.*/
/* STX */
/* 누군가의 요청에 의해 이렇게 코딩되어있단다. */
#if TEST_OFF_MODE
	//더미 스텝 
	//만약 X개의 스텝을 진행하는 경우
	//맨 앞에 스텝 1개 와 X개 이후 한개의 스텝의 더미 스텝이 필요하다 (FW팀 확인하였음 220206 YGYUN)
	//------------스텝 시작 전 더미 스텝 시작 ------------------

	//시간 종료 조건 38,39,40,41
	szData_index = 38;
	szData[szData_index] = 0x00; //38
	szData_index++;
	szData[szData_index] = 0x00; //39
	szData_index++;
	szData[szData_index] = 0x00; //40
	szData_index++;
	szData[szData_index] = 0x01; //41

	/* 특정 Start point 를 지정할때 사용한다. */
	//시간 종료 조건 시 Index 44,45
	if (nReserveStepIndex >= 1) 
	{
		szData_index = 44;
		szData[szData_index] = (BYTE)nReserveStepIndex >> 8 & 0xFF; //44
		szData_index = 45;
		szData[szData_index] = (BYTE)nReserveStepIndex >> 0 & 0xFF; //45
		szData_index = 64; // 한개 스텝 길이
	}
	//------------스텝 시작 전 더미 스텝 끝 ------------------

#endif
	int nStep = 1;
	//20201029 turby Send D Data view point
	while (Pos)
	{
		CStep* pStep = listStep.GetNext(Pos);

		//TRACE(_T(" 12 %d\n"), turby_++);

		if (pStep == NULL)
			continue;

		int nStepType = 0;

		m_nStepType = pStep->GetStepType();

		switch (pStep->GetStepType())
		{
		case eIFBoardStepTypeRest: nStepType = eIFBoardCommandRest;		break;
		case eIFBoardStepTypeOcv: nStepType = eIFBoardCommandRest;		break;
		case eIFBoardStepTypeCharge: nStepType = eIFBoardCommandCharge;	break;
		case eIFBoardStepTypeDischarge: nStepType = eIFBoardCommandDischarge;	break;
		case eIFBoardStepTypePogoPinTest: nStepType = eIFBoardCommandPogoPinTestMode;	break;
		}

		/* I/F Command *//*6,7*/
		szData[szData_index] = nStepType >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = nStepType >> 0 & 0xFF;
		szData_index++;

		/* Step Mode *//*8,9*/
		int nEQMode = pStep->GetStepMode();		//SEND_HEADER_1

		szData[szData_index] = nEQMode >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = nEQMode >> 0 & 0xFF;
		szData_index++;

		/* Operation Mode *//*10,11*/
		if (eIFBoardOperationNone == pStep->GetOperationMode())
		{
			switch (pStep->GetStepType())
			{
			case eIFBoardStepTypeCharge:
				szData[szData_index] = eIFBoardOperationChargeCC >> 8 & 0xFF;
				szData_index++;
				szData[szData_index] = eIFBoardOperationChargeCC >> 0 & 0xFF;
				szData_index++;
				break;
			case eIFBoardStepTypeDischarge:
				szData[szData_index] = eIFBoardOperationDischargeCC >> 8 & 0xFF;
				szData_index++;
				szData[szData_index] = eIFBoardOperationDischargeCC >> 0 & 0xFF;
				szData_index++;
				break;
			case eIFBoardStepTypeImpedance:
				szData[szData_index] = eIFBoardOperationDischargeCC >> 8 & 0xFF;
				szData_index++;
				szData[szData_index] = eIFBoardOperationDischargeCC >> 0 & 0xFF;
				szData_index++;
				break;
			default:
				szData_index += 2;
			}
		}
		else
		{
			szData[szData_index] = pStep->GetOperationMode() >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = pStep->GetOperationMode() >> 0 & 0xFF;
			szData_index++;
		}

		/* Row Data Collection Time Period *//*12,13*/
		szData[szData_index] = bySamplingTime >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = bySamplingTime >> 0 & 0xFF;
		szData_index++;

		/* To Cycler Message Send Time Period *//*14,15*/
		szData[szData_index] = bySendPeriod >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = bySendPeriod >> 0 & 0xFF;
		szData_index++;

		/* Step Index *//*16,17*/
		szData[szData_index] = pStep->GetStepIndex() >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepIndex() >> 0 & 0xFF;
		szData_index++;

		//TRACE(_T("%d %d\n"), szData[szData_index-1], szData[szData_index-2]);

		/* Cycle Loop Count *//*18,19*/
		szData[szData_index] = pStep->GetCycleLoopCount() >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetCycleLoopCount() >> 0 & 0xFF;
		szData_index++;

		/* Go to Step Index *//*20,21*/
		szData[szData_index] = pStep->GetLoopNextCycle() >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetLoopNextCycle() >> 0 & 0xFF;
		szData_index++;

		/* Return Step Index *//*22,23*/
		szData[szData_index] = pStep->GetPrevStepIndex() >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetPrevStepIndex() >> 0 & 0xFF;
		szData_index++;

		/* Ref 와 End Condition Voltage 를 같이 구한다.*/
		/* Ref Voltage 1,2 *//*24,25,26,27*/
		if (eIFBoardStepTypeCharge == pStep->GetStepType()|| eIFBoardStepTypePogoPinTest == pStep->GetStepType())
		{
			szData[szData_index] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;

			// By JSK - 2018.06.19 기존 CC/CV 모드 일때 CV 에 대한 동작 값이 없었다
			//전력전자 미적용
			/* Voltage End Condition 1,2 *//*28,29,30,31*/
			SScheduleComplete* lpScheduleComplete = pStep->GetScheduleCompleteFromName("HV");

			if (lpScheduleComplete)
			{
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else //20201102 turby 추훙에 default 값 적용하자
			{
				szData_index += 4;
			}
		}
		else if (eIFBoardStepTypeDischarge == pStep->GetStepType())   /* Ref Voltage 1,2 *//*24,25,26,27*/
		{
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;

			//By JSK - 2018.06.19 기존 CC/CV 모드 일때 CV 에 대한 동작 값이 없었다
			//전력전자 미적용
			/* Voltage End Condition 1,2 *//*28,29,30,31*/
			SScheduleComplete* lpScheduleComplete = pStep->GetScheduleCompleteFromName("LV");

			if (lpScheduleComplete)
			{
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else //20201102 turby 추훙에 default 값 적용하자
			{
				szData_index += 4;
			}
		}
		else if (eIFBoardStepTypeImpedance == pStep->GetStepType())
		{
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;

			// By JSK - 2018.06.19 기존 CC/CV 모드 일때 CV 에 대한 동작 값이 없었다
			// 전력전자 미적용
			/* Voltage End Condition 1,2 *//*28,29,30,31*/
			SScheduleComplete* lpScheduleComplete = pStep->GetScheduleCompleteFromName("LV");

			if (lpScheduleComplete)
			{
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else //20201102 turby 추훙에 default 값 적용하자
			{
				szData_index += 4;
			}
		}
		else //20201102 turby 추훙에 default 값 적용하자
		{
			szData_index += 8;
		}

		/* Voltage Step End Move Step Index *//*32,33*/
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteVoltage) >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteVoltage) >> 0 & 0xFF;
		szData_index++;

		CMgrConfig* pMgr = CMgrConfig::GetMgr();
		int nChannelCountInParallel = pMgr->GetParallelChannelCount(); // 병렬에 필요한 Channel 갯수 마스터 + 슬레이브

		//병렬 일때는 나누기 병렬 갯수 만큼 해줘서 보내야 됨 210916
		if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (eIFBoardOperationChargeCP == pStep->GetOperationMode() || eIFBoardOperationDischargeCP == pStep->GetOperationMode()
				|| eIFBoardOperationChargeCPCV == pStep->GetOperationMode() || eIFBoardOperationDischargeCPCV == pStep->GetOperationMode())
			{
				pStep->SetWatt(pStep->GetWatt() / nChannelCountInParallel);
			}
			else
			{
				if (pStep->GetStepType() == 1)
				{
					pStep->SetChargeCurrentAmpere(pStep->GetCurrentAmpere() / nChannelCountInParallel);
				}
				else
				{
					pStep->SetDisChargeCurrentAmpere(pStep->GetDisCurrentAmpere() / nChannelCountInParallel);
				}
			}
		}

		/* Ref Ampere / Ref Power 1,2 *//*34,35,36,37*/
		if (eIFBoardOperationChargeCP == pStep->GetOperationMode() || eIFBoardOperationDischargeCP == pStep->GetOperationMode())
		{
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
		}
		else if (eIFBoardOperationChargeCPCV == pStep->GetOperationMode() || eIFBoardOperationDischargeCPCV == pStep->GetOperationMode())
		{
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
		}
		else
		{
			if (pStep->GetStepType() == 1|| pStep->GetStepType() == 6)
			{
				szData[szData_index] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}
			else
			{
				szData[szData_index] = ((unsigned int)(pStep->GetDisCurrentAmpere() * 1000) >> 16 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetDisCurrentAmpere() * 1000) >> 16 >> 0 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetDisCurrentAmpere() * 1000) >> 0 >> 8 & 0xFF);
				szData_index++;
				szData[szData_index] = ((unsigned int)(pStep->GetDisCurrentAmpere() * 1000) >> 0 >> 0 & 0xFF);
				szData_index++;
			}

			//TRACE(_T("%d] %d %d %d %d\n"), szData_index - 5, szData[szData_index - 4], szData[szData_index - 3], szData[szData_index - 2], szData[szData_index - 1]);
		}

		/* Time End Condition 1,2 *//*38,39,40,41*/
		SScheduleComplete* lpScheduleCompleteTime = pStep->GetScheduleCompleteFromName("Ti");

		if (lpScheduleCompleteTime)
		{
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 16 >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 16 >> 0 & 0xFF;
			szData_index++;
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 0 >> 8 & 0xFF;
			szData_index++;
			szData[szData_index] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 0 >> 0 & 0xFF;
			szData_index++;
		}
		else
		{
			szData_index += 4;
		}

		/* Cycle Index *//*42,43*/
		//20190709 루차장님 요청으로 해당 인덱스는 사이클 번호가 들어간다(페턴 주기 저값 기존에 사용 안했음)
		szData[szData_index] = pStep->GetCycleNumber() >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetCycleNumber() >> 0 & 0xFF;
		szData_index++;

		///////////////////////////////////////////////////////////////////////////////////////
		// Pattern Step 무한 반복 Setting 2021-05-07
		CCommData* commData;
		commData = new CCommData();
		if (commData->CheckPatternLoopStepChange(nStep, 0) == true)//pStep->GetStepNumber()
			pStep->SetStepEndIndex(eIFBoardCompleteTime, g_nPatternStartStep[0]);

		DELETE_POINTER(commData);
		nStep++;

		///////////////////////////////////////////////////////////////////////////////////////
		/* Time Step End Move Step Index *//*44,45*/
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteTime) >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteTime) >> 0 & 0xFF;
		szData_index++;

		/* Ampere End Condition 1,2 *//*46,47,48, 49*/
		SScheduleComplete* lpScheduleCompleteCurrent = pStep->GetScheduleCompleteFromName("CUR");

		//병렬 일때는 나누기 병렬 갯수 만큼 해줘서 보내야 됨 210916
		if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (lpScheduleCompleteCurrent)
			{
				lpScheduleCompleteCurrent->fValue /= (float)nChannelCountInParallel;
			}
		}

		if (lpScheduleCompleteCurrent)
		{
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
		}
		else
		{
			szData_index += 4;
		}

		/* Ampere Step End Move Step Index *//*50,51*/
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteCurrent) >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteCurrent) >> 0 & 0xFF;
		szData_index++;

		/* Capacity End Condition 1,2 *//*52,53,54,55 */
		SScheduleComplete* lpScheduleCompleteCapacity = pStep->GetScheduleCompleteFromName("Ca");

		//병렬 일때는 나누기 병렬 갯수 만큼 해줘서 보내야 됨 210916
		if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (lpScheduleCompleteCapacity)
			{
				lpScheduleCompleteCapacity->fValue /= (float)nChannelCountInParallel;
			}
		}

		if (lpScheduleCompleteCapacity)
		{
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
		}
		else
		{
			szData_index += 4;
		}

		/* Capacity Step End Move Step Index *//*56,57*/
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteCapacity) >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteCapacity) >> 0 & 0xFF;
		szData_index++;

		/* Watt End Condition 1,2 *//*58,59,60,61*/
		SScheduleComplete* lpScheduleCompleteWatt = pStep->GetScheduleCompleteFromName("Po");

		//병렬 일때는 나누기 병렬 갯수 만큼 해줘서 보내야 됨 210916
		if (pChannel->GetEqMode() == EQModeParallel)
		{
			if (lpScheduleCompleteWatt)
			{
				lpScheduleCompleteWatt->fValue /= (float)nChannelCountInParallel;
			}
		}

		if (lpScheduleCompleteWatt)
		{
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 16 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			szData[szData_index] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
		}
		else
		{
			szData_index += 4;
		}

		/* Watt Step End Move Step Index *//*62,63*/
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteWatt) >> 8 & 0xFF;
		szData_index++;
		szData[szData_index] = pStep->GetStepEndIndex(eIFBoardCompleteWatt) >> 0 & 0xFF;
		szData_index++;

	}

	/* ETX */
#if 0 // org
	szData[szData_index + 16] = 0xFF;
	szData[szData_index + 17] = 0xFF;
	szData[szData_index + 18] = 0x00;
	szData[szData_index + 19] = 0x00;

	szData[szData_index + 38] = 0x00;
	szData[szData_index + 39] = 0x00;
	szData[szData_index + 40] = 0x00;
	szData[szData_index + 41] = 0x01;
#else
	//------------------스텝 종료 시 더미 스텝 시작 -----------------

	szData[szData_index + 10] = 0xFF; //164
	szData[szData_index + 11] = 0xFF;//165
	szData[szData_index + 12] = 0x00;//166
	szData[szData_index + 13] = 0x00;//167

	szData[szData_index + 32] = 0x00; //186
	szData[szData_index + 33] = 0x00;//187
	szData[szData_index + 34] = 0x00;//188
	szData[szData_index + 35] = 0x01;//189
#endif

	szData_index += 58; //65
	//------------------스텝 종료 시 더미 스텝 끝 -----------------
#if TEST_OFF_MODE
	//int nRet = pSocket->SendToServer(szData, szData_index);
	int nRet = 0;
	// Reconnect 추가. 2021-03-03
#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, szData_index);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else

/* GM 테스트 220203
	vector<char> test;

	nRet = pSocket->SendToServer(reinterpret_cast<char*>(test.data()), szData_index);*/

	nRet = pSocket->SendToServer(szData, szData_index);
	//delete[] szData; 조성복
#endif // 0



	//lyw12_1 : 아래 주석처리
	//pChannel->SetSendDataPacket(false);

	if (nRet > 0)
	{
		nChannelNumber = CalcChannelNumber(byChannel, nBoardNumber);

		SYSLOG(Dbg, "[NEW_SEND_BOARD_TYPE_D DATA][BOARD:%d][CH:0x%02x][CH:%d][Length : %d] SEND DATA", nBoardNumber, byChannel, nChannelNumber, szData_index);

		return true;
	}
#endif

	return false;
}
bool CSockProc::SendBoardTypePLC_Formation(bool bAlarm)
{
	char szData[MAX_BUF_SIZE] = { 0, };
	int nRet = 0;
	CClientSocket* pSocket = GetAtKey(1200);

	if (bAlarm)
	{
		szData[0] = 1;
	}
	else {
		szData[0] = 0;

	}
	nRet = pSocket->SendToServer_PLCINTERFACE(szData, 1);


	return false;
}
#else
bool	CSockProc::SendBoardTypeD(int nBoardNumber, BYTE byChannel, int nReserveStepIndex)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (NULL == pSocket)
		return false;

 	if (FALSE == pSocket->GetConnect())
		return false;

	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return false;

	int nChannelNumber = pMgr->GetChannelNumber(eDeviceTypeIFBoard, nBoardNumber, byChannel);

	if (nChannelNumber < 0)
		return false;

	BYTE bySamplingTime = 0x01;
	BYTE bySendPeriod	= 0x03;	

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

	if (NULL == pChannel)
		return false;

	CList< CStep*, CStep* >& listStep = pChannel->GetMgr()->GetStepList();

	char szData[MAX_BUF_SIZE] = { 0, };

	memset(szData, NULL, MAX_BUF_SIZE);
		
	pChannel->SetSendDataPacket(true);

	szData[SEND_HEADER_1]	= '@';
	szData[SEND_HEADER_2]	= 'D';
	szData[SEND_CH_INDEX1]	= (BYTE)byChannel >> 8 & 0xFF;
	szData[SEND_CH_INDEX2]  = (BYTE)byChannel >> 0 & 0xFF;

	// Add By JSK - 2020.06.30 Schedule 내 시작 번지를 위해 권혁주 책임님과 협의하여 End Time 에 대한 Step Move 자리에 시작 번지 적음
	if (nReserveStepIndex >= 2)
	{
		szData[SEND_END_TIME_INDEX_1] = (BYTE)nReserveStepIndex >> 8 & 0xFF;
		szData[SEND_END_TIME_INDEX_2] = (BYTE)nReserveStepIndex >> 0 & 0xFF;
	}
	else
	{
		szData[SEND_END_TIME_INDEX_1] = (BYTE)0 >> 8 & 0xFF;
		szData[SEND_END_TIME_INDEX_2] = (BYTE)1 >> 0 & 0xFF;
	}
	

	int nStepCount = listStep.GetSize();

	// 원래 0이였으나 전력전자팀 조경진 과장님 요청으로, 맨 앞의 Data 를 Dummy 로 넣는다.
	//int nindex = 94;//58; //82
//	int nindex = 58;

	// Modify By JSK - 2020.06.30 D Flame 내부 마지막 4Byte 가 CP, CP/CV 에서의 전류 Limit 값이 들어간다.
	int nindex = 98;

	auto Pos = listStep.GetHeadPosition();

	int cnt = 0;

	while (Pos)
	{
		//TRACE(_T("%d\n"), cnt++);
		CStep* pStep = listStep.GetNext(Pos);

		if (pStep == NULL)
			continue;

		int nStepType = 0;

		m_nStepType = pStep->GetStepType();

		switch (pStep->GetStepType())
		{
		case eIFBoardStepTypeRest		: nStepType = eIFBoardCommandRest;		break;
		case eIFBoardStepTypeOcv		: nStepType = eIFBoardCommandRest;		break;
		case eIFBoardStepTypeCharge		: nStepType = eIFBoardCommandCharge;	break;
		case eIFBoardStepTypeDischarge	: nStepType = eIFBoardCommandDischarge;	break;
		}

		szData[SEND_COMMAND1 + nindex] = nStepType >> 8 & 0xFF;
		szData[SEND_COMMAND2 + nindex] = nStepType >> 0 & 0xFF;

		int nEQMode = EthernetStepModeNormal;

		szData[SEND_MODE_STEP1 + nindex] = nEQMode >> 8 & 0xFF;
		szData[SEND_MODE_STEP2 + nindex] = nEQMode >> 0 & 0xFF;

		if (eIFBoardOperationNone == pStep->GetOperationMode())
		{
			switch (pStep->GetStepType())
			{
			case eIFBoardStepTypeCharge:
				szData[SEND_MODE_OP1 + nindex] = eIFBoardOperationChargeCC >> 8 & 0xFF;
				szData[SEND_MODE_OP2 + nindex] = eIFBoardOperationChargeCC >> 0 & 0xFF;
				break;
			case eIFBoardStepTypeDischarge:
				szData[SEND_MODE_OP1 + nindex] = eIFBoardOperationDischargeCC >> 8 & 0xFF;
				szData[SEND_MODE_OP2 + nindex] = eIFBoardOperationDischargeCC >> 0 & 0xFF;
				break;
			case eIFBoardStepTypeImpedance:
				szData[SEND_MODE_OP1 + nindex] = eIFBoardOperationDischargeCC >> 8 & 0xFF;
				szData[SEND_MODE_OP2 + nindex] = eIFBoardOperationDischargeCC >> 0 & 0xFF;
				break;
			}
		}
		else
		{
			szData[SEND_MODE_OP1 + nindex] = pStep->GetOperationMode() >> 8 & 0xFF;
			szData[SEND_MODE_OP2 + nindex] = pStep->GetOperationMode() >> 0 & 0xFF;
		}

		szData[SEND_SAMPLING1 + nindex]			= bySamplingTime >> 8 & 0xFF;
		szData[SEND_SAMPLING2 + nindex]			= bySamplingTime >> 0 & 0xFF;
		szData[SEND_TIME_PERIOD1 + nindex]		= bySendPeriod >> 8 & 0xFF;
		szData[SEND_TIME_PERIOD2 + nindex]		= bySendPeriod >> 0 & 0xFF;
		szData[SEND_STEP_INDEX_1 + nindex]		= pStep->GetStepIndex() >> 8 & 0xFF;
		szData[SEND_STEP_INDEX_2 + nindex]		= pStep->GetStepIndex() >> 0 & 0xFF;
		szData[SEND_CYCLE_LOOP1 + nindex]		= pStep->GetCycleLoopCount() >> 8 & 0xFF;
		szData[SEND_CYCLE_LOOP2 + nindex]		= pStep->GetCycleLoopCount() >> 0 & 0xFF;
		szData[SEND_GOTO_INDEX_1 + nindex]		= pStep->GetLoopNextCycle() >> 8 & 0xFF;
		szData[SEND_GOTO_INDEX_2 + nindex]		= pStep->GetLoopNextCycle() >> 0 & 0xFF;
		szData[SEND_RETURN_STEP_1 + nindex]		= pStep->GetPrevStepIndex() >> 8 & 0xFF;
		szData[SEND_RETURN_STEP_2 + nindex]		= pStep->GetPrevStepIndex() >> 0 & 0xFF;
			

		if (eIFBoardStepTypeCharge == pStep->GetStepType())
		{			
			szData[SEND_REF_VOL_1 + nindex] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 16 >> 8 & 0xFF);
			szData[SEND_REF_VOL_2 + nindex] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 16 >> 0 & 0xFF);
			szData[SEND_REF_VOL_3 + nindex] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 0 >> 8 & 0xFF);
			szData[SEND_REF_VOL_4 + nindex] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 0 >> 0 & 0xFF);

			// By JSK - 2018.06.19 기존 CC/CV 모드 일때 CV 에 대한 동작 값이 없었다
			//전력전자 미적용
			SScheduleComplete* lpScheduleComplete = pStep->GetScheduleCompleteFromName("HV");

			if (lpScheduleComplete)
			{
				szData[SEND_END_VOL_1 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData[SEND_END_VOL_2 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData[SEND_END_VOL_3 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData[SEND_END_VOL_4 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 0 & 0xFF);
			}
		}
		else if (eIFBoardStepTypeDischarge == pStep->GetStepType())
		{
			szData[SEND_REF_VOL_1 + nindex] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 8 & 0xFF);
			szData[SEND_REF_VOL_2 + nindex] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 0 & 0xFF);
			szData[SEND_REF_VOL_3 + nindex] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 8 & 0xFF);
			szData[SEND_REF_VOL_4 + nindex] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 0 & 0xFF);

// 			By JSK - 2018.06.19 기존 CC/CV 모드 일때 CV 에 대한 동작 값이 없었다
			//전력전자 미적용
			SScheduleComplete* lpScheduleComplete = pStep->GetScheduleCompleteFromName("LV");
			
			if (lpScheduleComplete)
			{
				szData[SEND_END_VOL_1 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData[SEND_END_VOL_2 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData[SEND_END_VOL_3 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData[SEND_END_VOL_4 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 0 & 0xFF);
			}
		}
		else if (eIFBoardStepTypeImpedance == pStep->GetStepType())
		{
			szData[SEND_REF_VOL_1 + nindex] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 8 & 0xFF);
			szData[SEND_REF_VOL_2 + nindex] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 16 >> 0 & 0xFF);
			szData[SEND_REF_VOL_3 + nindex] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 8 & 0xFF);
			szData[SEND_REF_VOL_4 + nindex] = ((unsigned int)(pStep->GetDisChargeVoltage() * 1000) >> 0 >> 0 & 0xFF);

			// 			By JSK - 2018.06.19 기존 CC/CV 모드 일때 CV 에 대한 동작 값이 없었다
			//전력전자 미적용
			SScheduleComplete* lpScheduleComplete = pStep->GetScheduleCompleteFromName("LV");

			if (lpScheduleComplete)
			{
				szData[SEND_END_VOL_1 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData[SEND_END_VOL_2 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData[SEND_END_VOL_3 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData[SEND_END_VOL_4 + nindex] = (BYTE)((unsigned int)(lpScheduleComplete->fValue * 1000) >> 0 >> 0 & 0xFF);
			}
		}

		szData[SEND_END_VOL_INDEX_1 + nindex]	= pStep->GetStepEndIndex(eIFBoardCompleteVoltage) >> 8 & 0xFF;
		szData[SEND_END_VOL_INDEX_2 + nindex]	= pStep->GetStepEndIndex(eIFBoardCompleteVoltage) >> 0 & 0xFF;
		
		if (eIFBoardOperationChargeCP == pStep->GetOperationMode() || eIFBoardOperationDischargeCP == pStep->GetOperationMode())
		{
			szData[SEND_REF_CUR_1 + nindex] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 8 & 0xFF);
			szData[SEND_REF_CUR_2 + nindex] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 0 & 0xFF);
			szData[SEND_REF_CUR_3 + nindex] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 8 & 0xFF);
			szData[SEND_REF_CUR_4 + nindex] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 0 & 0xFF);
		}
		else if (eIFBoardOperationChargeCPCV == pStep->GetOperationMode() || eIFBoardOperationDischargeCPCV == pStep->GetOperationMode())
		{
			szData[SEND_REF_CUR_1 + nindex] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 8 & 0xFF);
			szData[SEND_REF_CUR_2 + nindex] = ((unsigned int)(pStep->GetWatt() * 1000) >> 16 >> 0 & 0xFF);
			szData[SEND_REF_CUR_3 + nindex] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 8 & 0xFF);
			szData[SEND_REF_CUR_4 + nindex] = ((unsigned int)(pStep->GetWatt() * 1000) >> 0 >> 0 & 0xFF);
		}
		else
		{
			szData[SEND_REF_CUR_1 + nindex] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 16 >> 8 & 0xFF);
			szData[SEND_REF_CUR_2 + nindex] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 16 >> 0 & 0xFF);
			szData[SEND_REF_CUR_3 + nindex] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 0 >> 8 & 0xFF);
			szData[SEND_REF_CUR_4 + nindex] = ((unsigned int)(pStep->GetCurrentAmpere() * 1000) >> 0 >> 0 & 0xFF);
		}
		
		SScheduleComplete* lpScheduleCompleteTime = pStep->GetScheduleCompleteFromName("Ti");

		if (lpScheduleCompleteTime)
		{
			szData[SEND_END_TIME_1 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 16 >> 8 & 0xFF;
			szData[SEND_END_TIME_2 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 16 >> 0 & 0xFF;
			szData[SEND_END_TIME_3 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 0 >> 8 & 0xFF;
			szData[SEND_END_TIME_4 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 0 >> 0 & 0xFF;

			if (szData[SEND_END_TIME_1] == 0 && szData[SEND_END_TIME_2] == 0 && szData[SEND_END_TIME_3] == 0 && szData[SEND_END_TIME_4] == 0)
			{
				szData[SEND_END_TIME_1] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 16 >> 8 & 0xFF;
				szData[SEND_END_TIME_2] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 16 >> 0 & 0xFF;
				szData[SEND_END_TIME_3] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 0 >> 8 & 0xFF;
				szData[SEND_END_TIME_4] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTime->strValue, ':') * 0.01f) >> 0 >> 0 & 0xFF;
			}				
		}

// 		szData[SEND_PATT_PERIOD1 + nindex]		= 0x00;
// 		szData[SEND_PATT_PERIOD2 + nindex]		= 0x00;

		//20190709 루차장님 요청으로 해당 인덱스는 사이클 번호가 들어간다(페턴 주기 저값 기존에 사용 안했음)
		szData[SEND_PATT_PERIOD1 + nindex]		= pStep->GetCycleNumber() >> 8 & 0xFF;
		szData[SEND_PATT_PERIOD2 + nindex]		= pStep->GetCycleNumber() >> 0 & 0xFF;
		szData[SEND_END_TIME_INDEX_1 + nindex]	= pStep->GetStepEndIndex(eIFBoardCompleteTime) >> 8 & 0xFF;
		szData[SEND_END_TIME_INDEX_2 + nindex]	= pStep->GetStepEndIndex(eIFBoardCompleteTime) >> 0 & 0xFF;

		SScheduleComplete* lpScheduleCompleteCurrent = pStep->GetScheduleCompleteFromName("CUR");

		if (lpScheduleCompleteCurrent)
		{
			szData[SEND_END_CUR_1 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 16 >> 8 & 0xFF);
			szData[SEND_END_CUR_2 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 16 >> 0 & 0xFF);
			szData[SEND_END_CUR_3 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 0 >> 8 & 0xFF);
			szData[SEND_END_CUR_4 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCurrent->fValue * 1000) >> 0 >> 0 & 0xFF);
		}

		szData[SEND_END_CUR_INDEX_1 + nindex] = pStep->GetStepEndIndex(eIFBoardCompleteCurrent) >> 8 & 0xFF;
		szData[SEND_END_CUR_INDEX_2 + nindex] = pStep->GetStepEndIndex(eIFBoardCompleteCurrent) >> 0 & 0xFF;

		SScheduleComplete* lpScheduleCompleteCapacity = pStep->GetScheduleCompleteFromName("Ca");

		if (lpScheduleCompleteCapacity)
		{
			szData[SEND_END_CAP_1 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 16 >> 8 & 0xFF);
			szData[SEND_END_CAP_2 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 16 >> 0 & 0xFF);
			szData[SEND_END_CAP_3 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 0 >> 8 & 0xFF);
			szData[SEND_END_CAP_4 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCapacity->fValue * 1000) >> 0 >> 0 & 0xFF);
		}

		szData[SEND_END_CAP_INDEX_1 + nindex] = pStep->GetStepEndIndex(eIFBoardCompleteCapacity) >> 8 & 0xFF;
		szData[SEND_END_CAP_INDEX_2 + nindex] = pStep->GetStepEndIndex(eIFBoardCompleteCapacity) >> 0 & 0xFF;

		SScheduleComplete* lpScheduleCompleteWatt = pStep->GetScheduleCompleteFromName("Po");

		if (lpScheduleCompleteWatt)  
		{
			szData[SEND_END_WAT_1 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 16 >> 8 & 0xFF);
			szData[SEND_END_WAT_2 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 16 >> 0 & 0xFF);
			szData[SEND_END_WAT_3 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 0 >> 8 & 0xFF);
			szData[SEND_END_WAT_4 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteWatt->fValue * 1000) >> 0 >> 0 & 0xFF);
		}

		szData[SEND_END_WAT_INDEX_1 + nindex] = pStep->GetStepEndIndex(eIFBoardCompleteWatt) >> 8 & 0xFF;
		szData[SEND_END_WAT_INDEX_2 + nindex] = pStep->GetStepEndIndex(eIFBoardCompleteWatt) >> 0 & 0xFF;

 		CCycle* pCycle = pChannel->GetMgr()->GetAtCycle(pStep->GetCycleNumber()); //taek sk cycle 종료 조건 test 여기부터

 		if (eIFBoardStepTypeCharge == m_nStepType)
 		{
			if (pCycle)
			{
				SScheduleComplete* lpScheduleCompleteVoltageMax = pCycle->GetScheduleCompleteFromName(_T("HV"));

				if (lpScheduleCompleteVoltageMax)
				{
					szData[SEND_CYCLE_END_VOL_1 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMax->fValue * 1000) >> 16 >> 8 & 0xFF);
					szData[SEND_CYCLE_END_VOL_2 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMax->fValue * 1000) >> 16 >> 0 & 0xFF);
					szData[SEND_CYCLE_END_VOL_3 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMax->fValue * 1000) >> 0 >> 8 & 0xFF);
					szData[SEND_CYCLE_END_VOL_4 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMax->fValue * 1000) >> 0 >> 0 & 0xFF);
				}
			} 			
 		}
 		else if (eIFBoardStepTypeDischarge == m_nStepType)
 		{
			if (pCycle)
			{
				SScheduleComplete* lpScheduleCompleteVoltageMin = pCycle->GetScheduleCompleteFromName(_T("LV"));

				if (lpScheduleCompleteVoltageMin)
				{
					szData[SEND_CYCLE_END_VOL_1 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 16 >> 8 & 0xFF);
					szData[SEND_CYCLE_END_VOL_2 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 16 >> 0 & 0xFF);
					szData[SEND_CYCLE_END_VOL_3 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 0 >> 8 & 0xFF);
					szData[SEND_CYCLE_END_VOL_4 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 0 >> 0 & 0xFF);
				}
			}			
		}
		else if (eIFBoardStepTypeImpedance == m_nStepType)
		{
			if (pCycle)
			{
				SScheduleComplete* lpScheduleCompleteVoltageMin = pCycle->GetScheduleCompleteFromName(_T("LV"));

				if (lpScheduleCompleteVoltageMin)
				{
					szData[SEND_CYCLE_END_VOL_1 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 16 >> 8 & 0xFF);
					szData[SEND_CYCLE_END_VOL_2 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 16 >> 0 & 0xFF);
					szData[SEND_CYCLE_END_VOL_3 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 0 >> 8 & 0xFF);
					szData[SEND_CYCLE_END_VOL_4 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteVoltageMin->fValue * 1000) >> 0 >> 0 & 0xFF);
				}
			}			
		}

		if (pCycle)
		{
			szData[SEND_CYCLE_END_VOL_INDEX_1 + nindex] = pCycle->GetCycleEndIndex(eCycleEndCondtionVoltage) >> 8 & 0xFF;
			szData[SEND_CYCLE_END_VOL_INDEX_2 + nindex] = pCycle->GetCycleEndIndex(eCycleEndCondtionVoltage) >> 0 & 0xFF;
		}		

		SScheduleComplete* lpScheduleCompleteTimeMax = pCycle->GetScheduleCompleteFromName(_T("Ti"));

		if (lpScheduleCompleteTimeMax)
		{
			szData[SEND_CYCLE_END_TIME_1 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 16 >> 8 & 0xFF;
			szData[SEND_CYCLE_END_TIME_2 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 16 >> 0 & 0xFF;
			szData[SEND_CYCLE_END_TIME_3 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 0 >> 8 & 0xFF;
			szData[SEND_CYCLE_END_TIME_4 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 0 >> 0 & 0xFF;

			if (szData[SEND_CYCLE_END_TIME_1] == 0 && szData[SEND_CYCLE_END_TIME_2] == 0 && szData[SEND_CYCLE_END_TIME_3] == 0 && szData[SEND_CYCLE_END_TIME_4] == 0)
			{
				szData[SEND_CYCLE_END_TIME_1] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 16 >> 8 & 0xFF;
				szData[SEND_CYCLE_END_TIME_2] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 16 >> 0 & 0xFF;
				szData[SEND_CYCLE_END_TIME_3] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 0 >> 8 & 0xFF;
				szData[SEND_CYCLE_END_TIME_4] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleCompleteTimeMax->strValue, ':') * 0.01f) >> 0 >> 0 & 0xFF;
			}
		}

		szData[SEND_CYCLE_END_TIME_INDEX_1 + nindex] = pCycle->GetCycleEndIndex(eCycleEndCondtionTime) >> 8 & 0xFF;
		szData[SEND_CYCLE_END_TIME_INDEX_2 + nindex] = pCycle->GetCycleEndIndex(eCycleEndCondtionTime) >> 0 & 0xFF;

		if (pCycle)
		{
			SScheduleComplete* lpScheduleCompleteCapacityMax = pCycle->GetScheduleCompleteFromName(_T("Ca"));

			if (lpScheduleCompleteCapacityMax)
			{
				szData[SEND_CYCLE_END_CAP_1 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCapacityMax->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData[SEND_CYCLE_END_CAP_2 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCapacityMax->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData[SEND_CYCLE_END_CAP_3 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCapacityMax->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData[SEND_CYCLE_END_CAP_4 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteCapacityMax->fValue * 1000) >> 0 >> 0 & 0xFF);
			}

			szData[SEND_CYCLE_END_CAP_INDEX_1 + nindex] = pCycle->GetCycleEndIndex(eCycleEndCondtionCapacity) >> 8 & 0xFF;
			szData[SEND_CYCLE_END_CAP_INDEX_2 + nindex] = pCycle->GetCycleEndIndex(eCycleEndCondtionCapacity) >> 0 & 0xFF;
		}
		
		if (pCycle)
		{
			SScheduleComplete* lpScheduleCompleteWattMax = pCycle->GetScheduleCompleteFromName(_T("Po"));

			if (lpScheduleCompleteWattMax)
			{
				szData[SEND_CYCLE_END_WAT_1 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteWattMax->fValue * 1000) >> 16 >> 8 & 0xFF);
				szData[SEND_CYCLE_END_WAT_2 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteWattMax->fValue * 1000) >> 16 >> 0 & 0xFF);
				szData[SEND_CYCLE_END_WAT_3 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteWattMax->fValue * 1000) >> 0 >> 8 & 0xFF);
				szData[SEND_CYCLE_END_WAT_4 + nindex] = (BYTE)((unsigned int)(lpScheduleCompleteWattMax->fValue * 1000) >> 0 >> 0 & 0xFF);
			}

			szData[SEND_CYCLE_END_WAT_INDEX_1 + nindex] = pCycle->GetCycleEndIndex(eCycleEndCondtionWatt) >> 8 & 0xFF;
			szData[SEND_CYCLE_END_WAT_INDEX_2 + nindex] = pCycle->GetCycleEndIndex(eCycleEndCondtionWatt) >> 0 & 0xFF;
		}

		if (false == pChannel->GetTempSync())
		{
			szData[SEND_TEMP_SETTING_1 + nindex] = 0x00;
			szData[SEND_TEMP_SETTING_2 + nindex] = 0x00;
		}
		else
		{
			if (pStep->GetChamberSetTemp() < 0.000f)
			{
				int nChamberSetTemp = (int)abs(pStep->GetChamberSetTemp() * 10);

				szData[SEND_TEMP_SETTING_1 + nindex] = (BYTE)nChamberSetTemp >> 8 & 0xFF; //챔버 셋팅 온도
				szData[SEND_TEMP_SETTING_1 + nindex] = szData[SEND_TEMP_SETTING_1 + nindex] |= (1 << 7);
				szData[SEND_TEMP_SETTING_2 + nindex] = (BYTE)nChamberSetTemp >> 0 & 0xFF; //챔버 셋팅 온도
			}
			else
			{
				szData[SEND_TEMP_SETTING_1 + nindex] = (BYTE)pStep->GetChamberSetTemp() * 10 >> 8 & 0xFF; //챔버 셋팅 온도
				szData[SEND_TEMP_SETTING_2 + nindex] = (BYTE)pStep->GetChamberSetTemp() * 10 >> 0 & 0xFF;
			}
		}
 
 //dwell time
		bool bExistDwellTime = false;

		CString strDwellTime = pStep->GetChamberDelayTime();

		if (strcmp(strDwellTime, "") == 0 || strcmp(strDwellTime, "00:00:00:00") == 0)
			bExistDwellTime = false;
		else
			bExistDwellTime = true;

		strDwellTime.Replace(":", "");

		if (atoi(strDwellTime) > 0)
			bExistDwellTime = true;
		else
			bExistDwellTime = false;

 		if (true == bExistDwellTime && true == pChannel->GetTempSync())
 		{
 			szData[SEND_DWELL_TIME_1 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 16 >> 8 & 0xFF;
 			szData[SEND_DWELL_TIME_2 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 16 >> 0 & 0xFF;
 			szData[SEND_DWELL_TIME_3 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 0 >> 8 & 0xFF;
 			szData[SEND_DWELL_TIME_4 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 0 >> 0 & 0xFF;
 
 			if (szData[SEND_DWELL_TIME_1] == 0 && szData[SEND_DWELL_TIME_2] == 0 && szData[SEND_DWELL_TIME_3] == 0 && szData[SEND_DWELL_TIME_4] == 0)
 			{
				szData[SEND_DWELL_TIME_1 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 16 >> 8 & 0xFF;
				szData[SEND_DWELL_TIME_2 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 16 >> 0 & 0xFF;
				szData[SEND_DWELL_TIME_3 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 0 >> 8 & 0xFF;
				szData[SEND_DWELL_TIME_4 + nindex] = (unsigned int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) >> 0 >> 0 & 0xFF;
 			}

			if ((int)(CPnxUtil::GetMgr()->GetSplitTime(pStep->GetChamberDelayTime(), ':') * 0.01f) > 0 && true == pChannel->GetTempSync())
			{
				pStep->SetEndSync(1);
				pChannel->SetEndSync(1);
			}
			else
			{
				pStep->SetEndSync(0);
				pChannel->SetEndSync(0);
			}

			if (pChannel->GetSyncChamber())
			{
				pStep->SetEndSync(0);
				pChannel->SetEndSync(0);
			}
		}

		//채널이 두개 이상 구동 되면
		// By JSK - 동기화 이거 나중에 원복해라
		/*
		if (pChannel->GetRunChannel().GetLength() > 2 || true == pChannel->GetTempSync()) //taek 20200616
		{
			if (pStep->GetSyncType() != 0)
			{
				if (pChannel->GetRunChannel().Find(pChannel->GetChamberChannel()) < 0)
				{
					pStep->SetSyncType(0);
				}
			}
		}

		if (pChannel->GetSyncChamber())
		{
			pStep->SetSyncType(0);
		}

		if (true == pChannel->GetStepSync())
		{
			pStep->SetSyncType(1);
		}
		else if (true == pChannel->GetTempSync()) //taek 20200616
		{
			pStep->SetSyncType(2);
		}
		else
		{
			pStep->SetSyncType(0);
		}
		

		szData[SEND_SYNC_TYPE1 + nindex] = (BYTE)pStep->GetSyncType() >> 8 & 0XFF;
		szData[SEND_SYNC_TYPE2 + nindex] = (BYTE)pStep->GetSyncType() >> 0 & 0XFF;

		if (pStep->GetSyncType() > 0)
		{
			pChannel->SetSyncType(pStep->GetSyncType());
		}
		*/

		// By JSK - 2020.07.01 다시 정리함
		// 1. 스텝 동기화는 모니터링 프로그램에서 설정하면 그 값을 그대로 쓴다.
		// 2. 온도 동기화는 모니터링에서 설정 하되, 스텝별로 챔버 온도값 설정이 있으면 그렇게 쓴다.

		// 즉, 온도 동기화 일때는 스케줄 시작 전, 챔버 설정이 있는지 검사를 하고, 해당 스텝에 미리 동기화 여부를 넣어주는 예외처리를 한다.

		if (true == pChannel->GetTempSync())
		{			
			if (pStep->GetChamberSetTemp() >= -30.000f)
			{
				szData[SEND_SYNC_TYPE1 + nindex] = (BYTE)eScheduleSyncModeStep >> 8 & 0XFF;
				szData[SEND_SYNC_TYPE2 + nindex] = (BYTE)eScheduleSyncModeStep >> 0 & 0XFF;
			}
		}
		else
		{
			szData[SEND_SYNC_TYPE1 + nindex] = (BYTE)pChannel->GetSyncType() >> 8 & 0XFF;
			szData[SEND_SYNC_TYPE2 + nindex] = (BYTE)pChannel->GetSyncType() >> 0 & 0XFF;
		}		
 
		if (pStep->GetChamberElevatedTemperature())
		{
			szData[SEND_TEMP_START1 + nindex] = (BYTE)1 >> 8 & 0xFF; //승온
			szData[SEND_TEMP_START2 + nindex] = (BYTE)1 >> 0 & 0xFF; //승온
		}
		else
		{
			szData[SEND_TEMP_START1 + nindex] = (BYTE)0 >> 8 & 0xFF; //승온
			szData[SEND_TEMP_START2 + nindex] = (BYTE)0 >> 0 & 0xFF; //승온
		} 		

		if (true == pStep->GetChamberElevatedTemperature() && true == pChannel->GetTempSync())
		{
			pChannel->SetChamberElevatedTemperature(pStep->GetChamberElevatedTemperature());
		}

		szData[SEND_END_SYNC1 + nindex] = (BYTE)pStep->GetEndSync() >> 8 & 0XFF;
		szData[SEND_END_SYNC2 + nindex] = (BYTE)pStep->GetEndSync() >> 0 & 0XFF;   //test 

		// Add By JSK - 2020.08.06 SKI 요청한 전류 Limit 값
		// 20201029  turby 주석 처리 지움
		//if (eIFBoardStepTypeCharge == pStep->GetStepType() || eIFBoardStepTypeDischarge == pStep->GetStepType() || eIFBoardStepTypeExternalPattern == pStep->GetStepType())
		if (eIFBoardStepTypeCharge == pStep->GetStepType() || eIFBoardStepTypeDischarge == pStep->GetStepType() || eIFBoardStepTypeExternalPattern == pStep->GetStepType())
		{
			if (eIFBoardStepModeCP == pStep->GetStepMode())
			{
				if (eIFBoardStepTypeCharge == pStep->GetStepType())
				{
#if UPDATE_CP_MODE_TURBY
					szData[SEND_CURRENT_LIMIT_1 + nindex] = (BYTE)((((int)(pStep->GetCurrentAmpere() * 1000)) >> 8) & 0xFF);
					szData[SEND_CURRENT_LIMIT_2 + nindex] = ((BYTE)(pStep->GetCurrentAmpere() * 1000) & 0xFF);
					szData[SEND_CURRENT_LIMIT_3 + nindex] = (BYTE)pStep->GetCurrentAmpere() * 1000 >> 0 >> 8 & 0xFF;
					szData[SEND_CURRENT_LIMIT_4 + nindex] = (BYTE)pStep->GetCurrentAmpere() * 1000 >> 0 >> 0 & 0xFF;
#else
					szData[SEND_CURRENT_LIMIT_1 + nindex] = (BYTE)pStep->GetChargeCurrentLimit() * 1000 >> 16 >> 8 & 0xFF;
					szData[SEND_CURRENT_LIMIT_2 + nindex] = (BYTE)pStep->GetChargeCurrentLimit() * 1000 >> 16 >> 0 & 0xFF;
					szData[SEND_CURRENT_LIMIT_3 + nindex] = (BYTE)pStep->GetChargeCurrentLimit() * 1000 >> 0 >> 8 & 0xFF;
					szData[SEND_CURRENT_LIMIT_4 + nindex] = (BYTE)pStep->GetChargeCurrentLimit() * 1000 >> 0 >> 0 & 0xFF;
#endif
				}
				else if (eIFBoardStepTypeDischarge == pStep->GetStepType())
				{
					szData[SEND_CURRENT_LIMIT_1 + nindex] = (BYTE)pStep->GetDischargeCurrentLimit() * 1000 >> 16 >> 8 & 0xFF;
					szData[SEND_CURRENT_LIMIT_2 + nindex] = (BYTE)pStep->GetDischargeCurrentLimit() * 1000 >> 16 >> 0 & 0xFF;
					szData[SEND_CURRENT_LIMIT_3 + nindex] = (BYTE)pStep->GetDischargeCurrentLimit() * 1000 >> 0 >> 8 & 0xFF;
					szData[SEND_CURRENT_LIMIT_4 + nindex] = (BYTE)pStep->GetDischargeCurrentLimit() * 1000 >> 0 >> 0 & 0xFF;
				}
				else
				{
					szData[SEND_CURRENT_LIMIT_1 + nindex] = 0;
					szData[SEND_CURRENT_LIMIT_2 + nindex] = 0;
					szData[SEND_CURRENT_LIMIT_3 + nindex] = 0;
					szData[SEND_CURRENT_LIMIT_4 + nindex] = 0;
				}
			}
			else
			{
				szData[SEND_CURRENT_LIMIT_1 + nindex] = 0;
				szData[SEND_CURRENT_LIMIT_2 + nindex] = 0;
				szData[SEND_CURRENT_LIMIT_3 + nindex] = 0;
				szData[SEND_CURRENT_LIMIT_4 + nindex] = 0;
			}
		}


		// Add By JSK - 2020.06.30 SKI 요청한 전류 Limit 값
// 		szData[SEND_CURRENT_LIMIT_1 + nindex] = 0;
// 		szData[SEND_CURRENT_LIMIT_2 + nindex] = 0;
// 		szData[SEND_CURRENT_LIMIT_3 + nindex] = 0;
// 		szData[SEND_CURRENT_LIMIT_4 + nindex] = 0;

		// 실제 획득하고 여기다가 수정
// 		szData[SEND_CURRENT_LIMIT_1 + nindex] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 16 >> 8 & 0xFF);
// 		szData[SEND_CURRENT_LIMIT_2 + nindex] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 16 >> 0 & 0xFF);
// 		szData[SEND_CURRENT_LIMIT_3 + nindex] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 0 >> 8 & 0xFF);
// 		szData[SEND_CURRENT_LIMIT_4 + nindex] = ((unsigned int)(pStep->GetChargeVoltage() * 1000) >> 0 >> 0 & 0xFF);


// 		szData[SEND_STEP_PASUE + nindex] = pChannel->GetTEMPSync(); //예약 정지
// 
// 
// 		szData[SEND_SYNC_CHANNEL_1 + nindex] = pChannel->GetSyncChannel1() >> 8 & 0xFF;
// 		szData[SEND_SYNC_CHANNEL_2 + nindex] = pChannel->GetSyncChannel1() >> 0 & 0xFF;
// 		szData[SEND_SYNC_CHANNEL_3 + nindex] = pChannel->GetSyncChannel2() >> 8 & 0xFF;
// 		szData[SEND_SYNC_CHANNEL_4 + nindex] = pChannel->GetSyncChannel2() >> 0 & 0xFF;

//		nindex += 58;
		//nindex += 94; //82;
		nindex += 98;
	}


	szData[SEND_END_TIME_1] = 0x00;
	szData[SEND_END_TIME_2] = 0x00;
	szData[SEND_END_TIME_3] = 0x00;
	szData[SEND_END_TIME_4] = 0x01;
// 	szData[SEND_END_TIME_INDEX_1] = 0x00;
// 	szData[SEND_END_TIME_INDEX_2] = 0x01;

	szData[SEND_END_TIME_1 + nindex] = 0x00;
	szData[SEND_END_TIME_2 + nindex] = 0x00;
	szData[SEND_END_TIME_3 + nindex] = 0x00;
	szData[SEND_END_TIME_4 + nindex] = 0x01;

	// Modify By JSK - 2018.07.22 김과장님 요청으로 마지막 Dummy 에 Loop Count 를 넣는다.
	szData[SEND_CYCLE_LOOP1 + nindex] = 0x00;
	szData[SEND_CYCLE_LOOP2 + nindex] = 0x00;

	szData[SEND_STEP_INDEX_1 + nindex] = 0xFF;
	szData[SEND_STEP_INDEX_2 + nindex] = 0xFF;
		
	//int nByteLength = ((nStepCount + 2) * 94) + 6; //82

	int nByteLength = ((nStepCount + 2) * 98) + 6;

//	int nByteLength = ((nStepCount + 2) * 58) + 6;
 
 	nStepCount = (listStep.GetSize() +2 ) >> 8;

 	szData[SEND_STEP_CNT1] = nStepCount;
 	szData[SEND_STEP_CNT2] = (listStep.GetSize() + 2 ) >> 0 & 0xFF;

	//modfy by sjh 20190709 lg ochang2 fab
	//루차장님 요청으로 첫번째 더미의 사이클 인덱스 값을 '1' 로 적용
	szData[SEND_PATT_PERIOD1/*Cycle index*/] = 0 >> 8 & 0xFF;
	szData[SEND_PATT_PERIOD2/*Cycle index*/] = 0 >> 0 & 0xFF;

	//end
	
	int nRet = pSocket->SendToServer(szData, nByteLength);

	pChannel->SetSendDataPacket(false);

	if (nRet > 0)
	{
		nChannelNumber = CalcChannelNumber(byChannel, nBoardNumber);

 		LOG(Dbg, "[OLD_SEND_BOARD_TYPE_D DATA][BOARD:%d][CH:0x%02x][CH:%d][Sizd:%d] SEND DATA", nBoardNumber, byChannel, nChannelNumber,nByteLength);

		return true;
	}

	return false;
}
#endif

char szDataS[15] = { 0, };
#define SEND_1000	1
#define SEND_5000	2
#define SEND_10000	3
#define SEND_20000	4
#define SEND_30000	5
#define SEND_40000	6
#define SEND_50000	7
#define SEND_60000	8
#define SEND_65000	9

bool	CSockProc::SendBoardTypeS(int nBoardNumber, BYTE byChannel, int length)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (NULL == pSocket)
		return false;

	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;
	//if (FALSE == pSocket->GetConnect())
	//	return false;

	char szData[15] = { 0, };

	memset(szDataS, NULL, 15);

	int nByteLength = 14;

	int nChannelNumber = CMgrComm::GetMgr()->GetChannelNumber(eDeviceTypeIFBoard, nBoardNumber, byChannel);

	if (nChannelNumber < 0)
		return false;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

	szData[0] = '@';
	szData[1] = 'S';
	szData[2] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[3] = (BYTE)nByteLength >> 0 & 0xFF;
	szData[4] = (BYTE)byChannel >> 8 & 0xFF;
	szData[5] = (BYTE)byChannel >> 0 & 0xFF;
	szData[6] = (BYTE)pChannel->GetSyncType() >> 8 & 0xFF;
	szData[7] = (BYTE)pChannel->GetSyncType() >> 0 & 0xFF;

	if (true == pChannel->GetChamberElevatedTemperature())
	{
		szData[8] = (BYTE)1 >> 8 & 0xFF;
		szData[9] = (BYTE)1 >> 0 & 0xFF;
	}
	else
	{
		szData[8] = (BYTE)0 >> 8 & 0xFF;
		szData[9] = (BYTE)0 >> 0 & 0xFF;
	}

	szData[10] = (BYTE)0;
	szData[11] = (BYTE)0;
	szData[12] = (BYTE)0;
	szData[13] = (BYTE)0;

#if 0
	if (length < 3000)
	{
		TRACE(_T("0 %d \n"), length);
		Sleep(2000);
		TRACE(_T("0 %d \n"),length);
	}
	else if (length < 5010)
	{
		TRACE(_T("1 %d \n"), length);
		Sleep(6000);
		TRACE(_T("1 %d \n"), length);
	}
	else if (length < 10010)
	{
		TRACE(_T("2 %d \n"), length);
		Sleep(15000);
		TRACE(_T("2 %d \n"), length);
	}
	else if (length < 20010)
	{
		TRACE(_T("3 %d \n"), length);
		Sleep(30000);
		TRACE(_T("3 %d \n"), length);
	}
	else if (length < 30010)
	{
		TRACE(_T("4 %d \n"), length);
		Sleep(80000);
		TRACE(_T("4 %d \n"), length);
	}
	else if (length < 40010)
	{
		TRACE(_T("5 %d \n"), length);
		Sleep(200000);
		TRACE(_T("5 %d \n"), length);
	}
	else if (length < 50010)
	{
		TRACE(_T("6 %d \n"), length);
		Sleep(400000);
		TRACE(_T("6 %d \n"), length);
	}
	else if (length < 60010)
	{
		TRACE(_T("7 %d \n"), length);
		Sleep(800000);
		TRACE(_T("7 %d \n"), length);
	}
	else if (length < 65010)
	{
		TRACE(_T("8 %d \n"), length);
		Sleep(1200000);
		TRACE(_T("8 %d \n"), length);
	} 

	//SetTimer(SEND_1000, 1000, NULL);
#endif
	int nRet = 0;
	// Reconnect 추가. 2021-03-03

#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(szData, nByteLength);
#endif
	if (nRet > 0)
	{
		nChannelNumber = CalcChannelNumber(byChannel, nBoardNumber);

		//		LOG(Dbg, "SEND START SUCCESS..Channel(%d), Length(%d), Return(%d)", nChannelNumber, nByteLength, nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < nByteLength; ++i)
		{
			strTemp.Format("0x%02x   ", szData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_S_NOMAL DATA][BOARD:%d][CH:0x%02x][CH:%d] %s", nBoardNumber, byChannel, nChannelNumber, strDbug);

		return true;
	}

	return false;
}

bool	CSockProc::SendTimerTypeS()
{
	//int nRet = pSocket->SendToServer(szDataS, nByteLength);
	return 0;
}

bool	CSockProc::SendBoardTypeS(int nBoardNumber, BYTE byChannel)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (NULL == pSocket)
		return false;

	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;
	//if (FALSE == pSocket->GetConnect())
	//	return false;

	char szData[15] = { 0, };

	memset(szData, NULL, 15);

	int nByteLength = 14;

	int nChannelNumber = CMgrComm::GetMgr()->GetChannelNumber(eDeviceTypeIFBoard, nBoardNumber, byChannel);

	if (nChannelNumber < 0)
		return false;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

	szData[0] = '@';
	szData[1] = 'S';
	szData[2] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[3] = (BYTE)nByteLength >> 0 & 0xFF;
	szData[4] = (BYTE)byChannel >> 8 & 0xFF;
	szData[5] = (BYTE)byChannel >> 0 & 0xFF;
 	szData[6] = (BYTE)pChannel->GetSyncType() >> 8 & 0xFF;
 	szData[7] = (BYTE)pChannel->GetSyncType() >> 0 & 0xFF;

	if (true == pChannel->GetChamberElevatedTemperature())
	{
		szData[8] = (BYTE)1 >> 8 & 0xFF;
		szData[9] = (BYTE)1 >> 0 & 0xFF;
	}
	else
	{
		szData[8] = (BYTE)0 >> 8 & 0xFF;
		szData[9] = (BYTE)0 >> 0 & 0xFF;
	}

	szData[10] = (BYTE)0;
	szData[11] = (BYTE)0;
	szData[12] = (BYTE)0;
	szData[13] = (BYTE)0;

	//int nRet = pSocket->SendToServer(szData, nByteLength);
	// Reconnect 추가. 2021-03-03
	int nRet=0;

#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(szData, nByteLength);
#endif
	if (nRet > 0)
	{
		nChannelNumber = CalcChannelNumber(byChannel, nBoardNumber);

//		LOG(Dbg, "SEND START SUCCESS..Channel(%d), Length(%d), Return(%d)", nChannelNumber, nByteLength, nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < nByteLength; ++i)
		{
			strTemp.Format("0x%02x   ", szData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_S_NOMAL DATA][BOARD:%d][CH:0x%02x][CH:%d] %s", nBoardNumber, byChannel, nChannelNumber, strDbug);

		return true;
	}		
	
	return false;
}

bool	CSockProc::SendBoardTypeS(int nBoardNumber, BYTE byChannel1, BYTE byChannel2)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (NULL == pSocket)
		return false;

	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;
	//if (FALSE == pSocket->GetConnect())
	//	return false;

	char szData[28] = { 0, };

	memset(szData, NULL, 28);

	int nByteLength = 14;

	szData[0] = '@';
	szData[1] = 'S';
	szData[2] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[3] = (BYTE)nByteLength >> 0 & 0xFF;

	//(병렬일때 slave에 맞춘다 조경진K요청으로 의미 없는 값 셋팅)
// 	szData[4] = (BYTE)byChannel1 >> 8 & 0xFF;
// 	szData[5] = (BYTE)byChannel1 >> 0 & 0xFF;
	szData[4] = 0 >> 8 & 0xFF;
	szData[5] = 0 >> 0 & 0xFF;
	szData[6] = (BYTE)0;
	szData[7] = (BYTE)0;
	szData[8] = (BYTE)0;
	szData[9] = (BYTE)0;
	szData[10] = (BYTE)0;
	szData[11] = (BYTE)0;
	szData[12] = (BYTE)0;
	szData[13] = (BYTE)0;
	szData[14] = '@';
	szData[15] = 'S';
	szData[16] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[17] = (BYTE)nByteLength >> 0 & 0xFF;
	szData[18] = (BYTE)byChannel2 >> 8 & 0xFF;
	szData[19] = (BYTE)byChannel2 >> 0 & 0xFF;
	szData[20] = (BYTE)0;
	szData[21] = (BYTE)0;
	szData[22] = (BYTE)0;
	szData[23] = (BYTE)0;
	szData[24] = (BYTE)0;
	szData[25] = (BYTE)0;
	szData[26] = (BYTE)0;
	szData[27] = (BYTE)0;

	//int nRet = pSocket->SendToServer(szData, nByteLength*2);
	// Reconnect 추가. 2021-03-03
	int nRet = 0;

#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength * 2);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(szData, nByteLength * 2);
#endif

	if (nRet > 0)
	{
		SYSLOG(Dbg, "Send Start Success Channel1(0x%02x), Channel2(0x%02x), Length(%d), Return(%d)", byChannel1, byChannel2, nByteLength*2, nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < nByteLength * 2; ++i)
		{
			strTemp.Format("%x   ", szData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_S_DOUBLE DATA][BOARD:%d][CH:0x%02x][CH:0x%02x] SEND DATA : %s", nBoardNumber, byChannel1, byChannel2,strDbug);

		return true;
	}

	return false;
}

bool CSockProc::SendBoardTypeS(int nBoardNumber, BYTE byChannel, int nStepIndex, int nStepTime, int nCycleTime, int nLoopCount, float fStepAh, float fStepWh, float fCycleAh, float fCycleWh)
{	
	CClientSocket* pSocket = GetAt(nBoardNumber);
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	if (NULL == pSocket)
		return false;

	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;
	
	if (pMgrConfig == NULL)
		return false;
	//if (FALSE == pSocket->GetConnect())
	//	return false;

	char szData[15] = { 0, };

	memset(szData, NULL, sizeof(szData));

	int nByteLength = 14;

	int nChannelNumber = CMgrComm::GetMgr()->GetChannelNumber(eDeviceTypeIFBoard, nBoardNumber, byChannel);

	if (nChannelNumber < 0)
		return false;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

	szData[0] = '@';
	szData[1] = 'S';
	szData[2] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[3] = (BYTE)nByteLength >> 0 & 0xFF;
	szData[4] = (BYTE)byChannel >> 8 & 0xFF;
	szData[5] = (BYTE)byChannel >> 0 & 0xFF;

	//Calibration Mode 
	//0 : Default
	//10 : Volt Calibration
	//11 : Master Calibration
	//12 : Slave Calibration
	szData[6] = (BYTE)pChannel->GetStepMode() >> 8 & 0xFF;
	szData[7] = (BYTE)pChannel->GetStepMode() >> 0 & 0xFF;


	////SyncType
	//szData[6] = (BYTE)pChannel->GetSyncType() >> 8 & 0xFF;
	//szData[7] = (BYTE)pChannel->GetSyncType() >> 0 & 0xFF;

	////Stepindex
	//szData[8] = (BYTE)2 >> 8 & 0xFF;
	//szData[9] = (BYTE)2 >> 0 & 0xFF;

	//////StepTime
	//szData[10] = (BYTE)3 >> 8 & 0xFF;
	//szData[11] = (BYTE)3 >> 0 & 0xFF;

	//////CycleTime
	//szData[12] = NULL;
	//szData[13] = NULL;

	//////Cycle Loop Count
	//szData[13] = (BYTE)5 >> 8 & 0xFF;
	//szData[14] = (BYTE)5 >> 0 & 0xFF;


	if (true == pChannel->GetChamberElevatedTemperature())
	{
		szData[8] = (BYTE)1 >> 8 & 0xFF;
		szData[9] = (BYTE)1 >> 0 & 0xFF;
	}
	else
	{
		szData[8] = (BYTE)0 >> 8 & 0xFF;
		szData[9] = (BYTE)0 >> 0 & 0xFF;
	}

	szData[10] = (BYTE)0;
	szData[11] = (BYTE)0;
	szData[12] = (BYTE)0;
	szData[13] = (BYTE)0;

	////Stepindex
	//szData[14] = nStepIndex >> 8 & 0xFF;
	//szData[15] = nStepIndex >> 0 & 0xFF;

	////StepTime
	//szData[16] = nStepTime >> 16 >> 8 & 0xFF;
	//szData[17] = nStepTime >> 16 >> 0 & 0xFF;
	//szData[18] = nStepTime >> 0 >> 8 & 0xFF;
	//szData[19] = nStepTime >> 0 >> 0 & 0xFF;

	////StepAh
	//szData[20] = (BYTE)((unsigned int)(abs(fStepAh) * 1000) >> 16 >> 8 & 0xFF);
	//szData[21] = (BYTE)((unsigned int)(abs(fStepAh) * 1000) >> 16 >> 0 & 0xFF);
	//szData[22] = (BYTE)((unsigned int)(abs(fStepAh) * 1000) >> 0 >> 8 & 0xFF);
	//szData[23] = (BYTE)((unsigned int)(abs(fStepAh) * 1000) >> 0 >> 0 & 0xFF);

	////StepWh
	//szData[24] = (BYTE)((unsigned int)(abs(fStepWh) * 1000) >> 16 >> 8 & 0xFF);
	//szData[25] = (BYTE)((unsigned int)(abs(fStepWh) * 1000) >> 16 >> 0 & 0xFF);
	//szData[26] = (BYTE)((unsigned int)(abs(fStepWh) * 1000) >> 0 >> 8 & 0xFF);
	//szData[27] = (BYTE)((unsigned int)(abs(fStepWh) * 1000) >> 0 >> 0 & 0xFF);
	////

	////CycleTime
	//szData[28] = nCycleTime >> 16 >> 8 & 0xFF;
	//szData[29] = nCycleTime >> 16 >> 0 & 0xFF;
	//szData[30] = nCycleTime >> 0 >> 8 & 0xFF;
	//szData[31] = nCycleTime >> 0 >> 0 & 0xFF;

	////CycleAh
	//szData[32] = (BYTE)((unsigned int)(abs(fCycleAh) * 1000) >> 16 >> 8 & 0xFF);
	//szData[33] = (BYTE)((unsigned int)(abs(fCycleAh) * 1000) >> 16 >> 0 & 0xFF);
	//szData[34] = (BYTE)((unsigned int)(abs(fCycleAh) * 1000) >> 0 >> 8 & 0xFF);
	//szData[35] = (BYTE)((unsigned int)(abs(fCycleAh) * 1000) >> 0 >> 0 & 0xFF);

	////CycleWh
	//szData[36] = (BYTE)((unsigned int)(abs(fCycleWh) * 1000) >> 16 >> 8 & 0xFF);
	//szData[37] = (BYTE)((unsigned int)(abs(fCycleWh) * 1000) >> 16 >> 0 & 0xFF);
	//szData[38] = (BYTE)((unsigned int)(abs(fCycleWh) * 1000) >> 0 >> 8 & 0xFF);
	//szData[39] = (BYTE)((unsigned int)(abs(fCycleWh) * 1000) >> 0 >> 0 & 0xFF);
	////											   

	////Cycle Loop Count
	//szData[40] = nLoopCount >> 8 & 0xFF;
	//szData[41] = nLoopCount >> 0 & 0xFF;

	//int nRet = pSocket->SendToServer(szData, nByteLength);
	// Reconnect 추가. 2021-03-03
	int nRet = 0;

#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(szData, nByteLength);
#endif
	if (nRet > 0)
	{
		nChannelNumber = CalcChannelNumber(byChannel, nBoardNumber);

		//		LOG(Dbg, "SEND START SUCCESS..Channel(%d), Length(%d), Return(%d)", nChannelNumber, nByteLength, nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < nByteLength; ++i)
		{
			strTemp.Format("0x%02x   ", szData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_S_NOMAL DATA][BOARD:%d][CH:0x%02x][CH:%d] %s", nBoardNumber, byChannel, nChannelNumber, strDbug);

		return true;
	}

	return false;
}

//Cycler -> I/F (Modify)
bool	CSockProc::SendBoardTypeM(int nBoardNumber, BYTE byChannel, int nStepIndex, float fRefVolt, float fRefWatt)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (pSocket == NULL)
		return false;
	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;
	//if (FALSE == pSocket->GetConnect())
	//	return false;

	char szData[20] = { 0, };
	memset(szData, NULL, sizeof(char)* 20);

	int nByteLength = 15;

	szData[0] = '@';
	szData[1] = 'M';
	szData[2] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[3] = (BYTE)nByteLength >> 0 & 0xFF;
	szData[4] = (BYTE)byChannel >> 8 & 0xFF;// | byChannel2 >> 8 & 0xFF;
	szData[5] = (BYTE)byChannel & 0xFF;// | byChannel2 >> 0 & 0xFF;
	szData[6] = (BYTE)nStepIndex >> 8 & 0xFF;
	szData[7] = (BYTE)nStepIndex & 0xFF;
	szData[8] = (BYTE)((unsigned int)(fRefVolt * 1000) >> 16 >> 8 & 0xFF);
	szData[9] = (BYTE)((unsigned int)(fRefVolt * 1000) >> 16 >> 0 & 0xFF);
	szData[10] = (BYTE)((unsigned int)(fRefVolt * 1000) >> 0 >> 8 & 0xFF);
	szData[11] = (BYTE)((unsigned int)(fRefVolt * 1000) >> 0 >> 0 & 0xFF);
	szData[12] = (BYTE)((unsigned int)(fRefWatt * 1000) >> 16 >> 8 & 0xFF);
	szData[13] = (BYTE)((unsigned int)(fRefWatt * 1000) >> 16 >> 0 & 0xFF);
	szData[14] = (BYTE)((unsigned int)(fRefWatt * 1000) >> 0 >> 8 & 0xFF);
	szData[15] = (BYTE)((unsigned int)(fRefWatt * 1000) >> 0 >> 0 & 0xFF);

	//int nRet = pSocket->SendToServer(szData, nByteLength);
	// Reconnect 추가. 2021-03-03
	int nRet = 0;

#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(szData, nByteLength);
#endif

	if (nRet > 0)
	{
		SYSLOG(Dbg, "Send Modify Start Success Channel(0x%02x),Length(%d), Return(%d)", byChannel,nByteLength, nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < nByteLength; ++i)
		{
			strTemp.Format("%x   ", szData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_M DATA] SEND DATA : %s", strDbug);

		return true;
	}

	return false;
}

bool CSockProc::SendBoardTypeI(int nBoardnumber, BYTE byChannel, BOOL bZeroOffset)
{
	CClientSocket* pSocket = GetAt(nBoardnumber);

	int nChannelNumber = CMgrComm::GetMgr()->GetChannelNumber(eDeviceTypeIFBoard, nBoardnumber, byChannel);

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	if (pSocket == NULL)
		return false;	
	if (pMgrConfig == NULL)
		return false;

	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;

	vector<char > vecData;
	vector<float> vecTargetValue;

	int nByteLength = 0;
	int nZeroOffset = 0;
	float fMeasureTarget = 0.0f;
	float fMeasureValue = 0.0f;
	int szData_index = 0;

	if (bZeroOffset == true)
	{
		nByteLength = 12;
		nZeroOffset = 1;
	}
	else
	{
		nByteLength = 88;
		nZeroOffset = 0;
	}

	//Command #1,#2
	vecData.push_back('@');
	szData_index++;
	vecData.push_back('I');
	szData_index++;

	//Protocol Length #3,#4
	vecData.push_back((BYTE)nByteLength >> 8 & 0xFF);
	szData_index++;
	vecData.push_back((BYTE)nByteLength >> 0 & 0xFF);
	szData_index++;

	//Channel Index #5,#6
	vecData.push_back((BYTE)byChannel >> 8 & 0xFF);
	szData_index++;
	vecData.push_back((BYTE)byChannel & 0xFF);
	szData_index++;

	//Zero Offset #7,#8
	vecData.push_back((BYTE)nZeroOffset >> 8 & 0xFF);
	szData_index++;
	vecData.push_back((BYTE)nZeroOffset & 0xFF);
	szData_index++;


	//0A만
	if (bZeroOffset == true)
	{
		vecTargetValue = pMgrConfig->GetCurrentCalibrationTargetValue();
		if (vecTargetValue[0] != 0.0f)
			return false;

		fMeasureValue = pChannel->Get_CurrentCalibrationData(vecTargetValue[0]);

		if (fMeasureValue >= 0)
		{
			//Value Sign #9
			vecData.push_back((BYTE)0);
			szData_index++;
		}
		else
		{
			//Value Sign #9
			vecData.push_back((BYTE)1);
			szData_index++;
		}
		//0A Reference Current #10,#11,#12
		vecData.push_back((unsigned int)(fMeasureValue * 1000) >> 16 >> 0 & 0xFF);
		szData_index++;
		vecData.push_back((unsigned int)(fMeasureValue * 1000) >> 0 >> 8 & 0xFF);
		szData_index++;
		vecData.push_back((unsigned int)(fMeasureValue * 1000) >> 0 >> 0 & 0xFF);
		szData_index++;
	}
	//1A ~ 100A
	else
	{
		vecTargetValue = pMgrConfig->GetCurrentCalibrationTargetValue();

		for (int nTargetValue = 0; nTargetValue = vecTargetValue.size(); nTargetValue++)
		{
			fMeasureValue = pChannel->Get_CurrentCalibrationData(vecTargetValue[nTargetValue]);

			if (fMeasureValue >= 0)
			{
				//Value Sign #9,#13,#17,....
				vecData.push_back((BYTE)0);
				szData_index++;
			}
			else
			{
				//Value Sign #9,#13,#17,....
				vecData.push_back((BYTE)1);
				szData_index++;
			}
			//1A Reference Current #10,#11,#12, #14,#15,#16,,
			vecData.push_back((unsigned int)(fMeasureValue * 1000) >> 16 >> 0 & 0xFF);
			szData_index++;
			vecData.push_back((unsigned int)(fMeasureValue * 1000) >> 0 >> 8 & 0xFF);
			szData_index++;
			vecData.push_back((unsigned int)(fMeasureValue * 1000) >> 0 >> 0 & 0xFF);
			szData_index++;
		}

		
	}

	//int nRet = pSocket->SendToServer(szData, nByteLength);
	// Reconnect 추가. 2021-03-03
	int nRet = 0;

#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(reinterpret_cast<char*>(vecData.data()), vecData.size());
#endif

	if (nRet > 0)
	{
		SYSLOG(Dbg, "Send Modify Start Success Channel(0x%02x),Length(%d), Return(%d)", byChannel, vecData.size(), nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < vecData.size(); ++i)
		{
			strTemp.Format("%x   ", vecData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_M DATA] SEND DATA : %s", strDbug);

		return true;
	}

	return false;
}

bool CSockProc::SendBoardTypeV(int nBoardnumber, BYTE byChannel)
{
	CClientSocket* pSocket = GetAt(nBoardnumber);

	int nChannelNumber = CMgrComm::GetMgr()->GetChannelNumber(eDeviceTypeIFBoard, nBoardnumber, byChannel);

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	if (pSocket == NULL)
		return false;
	if (pMgrConfig == NULL)
		return false;

	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;

	vector<char > vecData;

	int nByteLength = 32;
	float fMeasureTarget = 0.0f;
	float fMeasureValue = 0.0f;
	int szData_index = 0;


	//Command #1,#2
	vecData.push_back('@');
	szData_index++;
	vecData.push_back('V');
	szData_index++;

	//Protocol Length #3,#4
	vecData.push_back((BYTE)nByteLength >> 8 & 0xFF);
	szData_index++;
	vecData.push_back((BYTE)nByteLength >> 0 & 0xFF);
	szData_index++;

	//Channel Index #5,#6
	vecData.push_back((BYTE)byChannel >> 8 & 0xFF);
	szData_index++;
	vecData.push_back((BYTE)byChannel & 0xFF);
	szData_index++;

	//Reserve #7,#8
	vecData.push_back((BYTE)0);
	szData_index++;
	vecData.push_back((BYTE)0);
	szData_index++;	

	for (int nTargetValue = 0; nTargetValue < 5; nTargetValue++)
	{
		fMeasureTarget = fMeasureTarget + (1.0f*nTargetValue);
		fMeasureValue = pChannel->Get_VoltCalibrationData(fMeasureTarget);

		if (fMeasureValue >= 0)
		{
			//Value Sign #9,#13,#17,....
			vecData.push_back((BYTE)0);
			szData_index++;
		}
		else
		{
			//Value Sign #9,#13,#17,....
			vecData.push_back((BYTE)1);
			szData_index++;
		}
		//Reference Volt #10,#11,#12, #14,#15,#16,,
		vecData.push_back((unsigned int)(fMeasureValue * 1000) >> 16 >> 0 & 0xFF);
		szData_index++;
		vecData.push_back((unsigned int)(fMeasureValue * 1000) >> 0 >> 8 & 0xFF);
		szData_index++;
		vecData.push_back((unsigned int)(fMeasureValue * 1000) >> 0 >> 0 & 0xFF);
		szData_index++;
	}

	//int nRet = pSocket->SendToServer(szData, nByteLength);
	// Reconnect 추가. 2021-03-03
	int nRet = 0;

#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(reinterpret_cast<char*>(vecData.data()), vecData.size());
#endif

	if (nRet > 0)
	{
		SYSLOG(Dbg, "Send Volt Calibration Data Channel(0x%02x),Length(%d), Return(%d)", byChannel, vecData.size(), nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < vecData.size(); ++i)
		{
			strTemp.Format("%x   ", vecData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_V DATA] SEND DATA : %s", strDbug);

		return true;
	}

	return false;
}

/**
 * .
 * 
 * METHOD_NAME : SendToRemote
 * METHOD_TYPE : -
 * DESCRIPTION : PCAN
 */
void CSockProc::SendToRemote(char * pBuff, int nSize)
{
	CClientSocket* pSocket = GetAtKey(CMgrComm::GetMgr()->GetPrimaryKey(CommObjRemote));

	if (pSocket == NULL) {
		return;
	}

	//if (pSocket->GetConnect() == FALSE) {
	if (pSocket->GetConnect(pSocket->GetPrimaryKey())) 
	{
		return;
	}

	//pSocket->SendToClient(pBuff, nSize);
	pSocket->SendToServer(pBuff, nSize);
}

//Cycler -> I/F (Pause)
bool	CSockProc::SendBoardTypeP(int nBoardNumber, BYTE byChannel, int nStepIndex, int nType)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (NULL == pSocket)
		return false;

	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;
	//if (FALSE == pSocket->GetConnect())
	//	return false;

	char szData[11] = { 0, };

	memset(szData, NULL, 11);

	int nByteLength = 10;

	szData[0] = '@';
	szData[1] = 'P';
	szData[2] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[3] = (BYTE)nByteLength >> 0 & 0xFF;
	szData[4] = (BYTE)byChannel >> 8 & 0xFF;
	szData[5] = (BYTE)byChannel >> 0 & 0xFF;
	szData[6] = (BYTE)nStepIndex >> 8 & 0xFF;
	szData[7] = (BYTE)nStepIndex >> 0 & 0xFF;
	szData[8] = (BYTE)nType >> 8 & 0xFF;
	szData[9] = (BYTE)nType >> 0 & 0xFF;

	//int nRet = pSocket->SendToServer(szData, nByteLength);
	// Reconnect 추가. 2021-03-03
	int nRet = 0;

#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(szData, nByteLength);
#endif
	if (nRet > 0)
	{
		int nChannelNumber = CalcChannelNumber(byChannel, nBoardNumber);

		//LOG(Dbg, "Send Pause Success Channel(%d), Type(%d), Length(%d), Return(%d)", nChannelNumber, nType, nByteLength, nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < nByteLength; ++i)
		{
			strTemp.Format("0x%02x   ", szData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_P DATA][BOARD:%d][CH:0x%02x][CH:%d] SEND DATA : %s", nBoardNumber, byChannel, nChannelNumber, strDbug);

		return true;
	}	

	return false;
}

bool	CSockProc::SendBoardTypeP(int nBoardNumber, CArray< int, int >& arrChannel, CArray< int, int >& arrStepIndex, int nType)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (NULL == pSocket)
		return false;

	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;
	//if (FALSE == pSocket->GetConnect())
	//	return false;

	char szData[200] = { 0, };

	memset(szData, NULL, 200);

	int nByteLength = 10;
	int nIndex		= 0;

	CString strChannel;

	for (int i = 0; i < arrChannel.GetSize(); ++i)
	{
		BYTE byChannel = 0;

			 if (arrChannel.GetAt(i) % 4 == 0) byChannel = 0x01;
		else if (arrChannel.GetAt(i) % 4 == 1) byChannel = 0x02;
		else if (arrChannel.GetAt(i) % 4 == 2) byChannel = 0x04;
		else if (arrChannel.GetAt(i) % 4 == 3) byChannel = 0x08;

		szData[0 + nIndex] = '@';
		szData[1 + nIndex] = 'P';
		szData[2 + nIndex] = nByteLength >> 8 & 0xFF;
		szData[3 + nIndex] = nByteLength >> 0 & 0xFF;
		szData[4 + nIndex] = byChannel >> 8 & 0xFF;
		szData[5 + nIndex] = byChannel >> 0 & 0xFF;
		szData[6 + nIndex] = arrStepIndex.GetAt(i) >> 8 & 0xFF;
		szData[7 + nIndex] = arrStepIndex.GetAt(i) >> 0 & 0xFF;
		szData[8 + nIndex] = nType >> 8 & 0xFF;
		szData[9 + nIndex] = nType >> 0 & 0xFF;

		nIndex += 10;
		CString str;

		str.Format("%d", arrChannel.GetAt(i) + 1);

		strChannel += str;
	}

	//int nRet = pSocket->SendToServer(szData, nByteLength * arrChannel.GetSize());
	// Reconnect 추가. 2021-03-03
	int nRet = 0;

#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength * arrChannel.GetSize());
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(szData, nByteLength * arrChannel.GetSize());
#endif

	if (nRet > 0)
	{
		SYSLOG(Dbg, "Send Pause Success Channel(%s), Type(%d), Length(%d), Return(%d)", strChannel, nType, nByteLength, nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < nByteLength * arrChannel.GetSize(); ++i)
		{
			strTemp.Format("%x   ", szData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_P][BOARD:%d] SEND DATA : %s", nBoardNumber, strDbug);

		return true;
	}

	return false;
}

bool	CSockProc::SendBoardTypeP(int nType)
{
	auto Pos = m_listSocket.GetHeadPosition();

	while (Pos)
	{
		CClientSocket* pSocket = m_listSocket.GetNext(Pos);

		if (NULL == pSocket)
			continue;

		if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
			return false;
		//if (FALSE == pSocket->GetConnect())
		//	return false;

		char szData[200] = { 0, };

		memset(szData, NULL, 200);

		int nByteLength = 10;
		int nIndex		= 0;

		CStringArray& arr = pSocket->GetChannelArray();

		for (int i = 0; i < arr.GetSize(); ++i)
		{
			int nChannelNumber = atoi(arr.GetAt(i));

			BYTE byChannel = 0;

				 if (nChannelNumber % 4 == 0) byChannel = 0x01;
			else if (nChannelNumber % 4 == 1) byChannel = 0x02;
			else if (nChannelNumber % 4 == 2) byChannel = 0x04;
			else if (nChannelNumber % 4 == 3) byChannel = 0x08;

			szData[nIndex++] = '@';
			szData[nIndex++] = 'P';
			szData[nIndex++] = nByteLength >> 8 & 0xFF;
			szData[nIndex++] = nByteLength >> 0 & 0xFF;
			szData[nIndex++] = byChannel >> 8 & 0xFF;
			szData[nIndex++] = byChannel >> 0 & 0xFF;
			szData[nIndex++] = 0x00;
			szData[nIndex++] = 0x00;
			szData[nIndex++] = nType >> 8 & 0xFF;
			szData[nIndex++] = nType >> 0 & 0xFF;
		}

		//int nRet = pSocket->SendToServer(szData, nByteLength * arr.GetSize());
		// Reconnect 추가. 2021-03-03
		int nRet = 0;
	
#if 0
		for (int retry = 0; retry < 3; retry++)
		{
			if (pSocket->GetConnect() == TRUE)
				nRet = pSocket->SendToServer(szData, nByteLength * arr.GetSize());
			if (nRet > 0)
				break;
			pSocket->CloseClientSocket();	delayMs(1000);
			pSocket->ConnectToServer();
		}
#else
		nRet = pSocket->SendToServer(szData, nByteLength * arr.GetSize());
#endif

		if (nRet > 0)
		{
			SYSLOG(Dbg, "Send Pause All Success Type(%d), Length(%d), Return(%d)", nType, nByteLength, nRet);

			CString strDbug = _T("");

			CString strTemp;
			for (int i = 0; i < nByteLength * arr.GetSize(); ++i)
			{
				strTemp.Format("%x   ", szData[i]);
				strDbug += strTemp;
			}
			SYSLOG(Dbg, "[SEND_BOARD_TYPE_P] SEND DATA : %s", strDbug);

			return true;
		}
	}

	return false;
}

//Cycler -> I/F (Jump)
bool	CSockProc::SendBoardTypeJ(int nBoardNumber, BYTE byChannel, int nStepIndex)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (NULL == pSocket)
		return false;

	//if (FALSE == pSocket->GetConnect())
		if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
			return false;

	char szData[9] = { 0, };

	memset(szData, NULL, 9);

	int nByteLength = 8;

	szData[0] = '@';
	szData[1] = 'J';
	szData[2] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[3] = (BYTE)nByteLength >> 0 & 0xFF;
	szData[4] = (BYTE)byChannel >> 8 & 0xFF;
	szData[5] = (BYTE)byChannel >> 0 & 0xFF;
	szData[6] = (BYTE)nStepIndex >> 8 & 0xFF;
	szData[7] = (BYTE)nStepIndex >> 0 & 0xFF;

	//int nRet = pSocket->SendToServer(szData, nByteLength);
	// Reconnect 추가. 2021-03-03
	int nRet = 0;

#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(szData, nByteLength);
#endif
	
	if (nRet > 0) 
	{
		int nChannelNumber = CalcChannelNumber(byChannel, nBoardNumber);

		//LOG(Dbg, "SEND JUMP SUCCESS..Channel(%d), Step Index(%d), Length(%d), Return(%d)", nChannelNumber, nStepIndex, nByteLength, nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < nByteLength; ++i)
		{
			strTemp.Format("%x   ", szData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_J_NOMAL DATA][BOARD:%d][CH:0x%02x][CH:%d] SEND DATA : %s", nBoardNumber, byChannel, nChannelNumber, strDbug);

		return true;
	}

	return false;
}

bool	CSockProc::SendBoardTypeC(int nBoardNumber, BYTE byChannel, BYTE byCommand)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (NULL == pSocket)
		return false;

	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;
	
	char szData[9] = { 0, };

	memset(szData, NULL, 9);

	int nByteLength = 8;

	szData[0] = '@';
	szData[1] = 'C';
	szData[2] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[3] = (BYTE)nByteLength >> 0 & 0xFF;
	szData[4] = (BYTE)byChannel >> 8 & 0xFF;
	szData[5] = (BYTE)byChannel >> 0 & 0xFF;
	szData[6] = (BYTE)byCommand >> 8 & 0xFF;
	szData[7] = (BYTE)byCommand >> 0 & 0xFF;
	
	/*
	char szData[16] = { 0, };

	memset(szData, NULL, 16);

	int nByteLength = 8;

	szData[0] = '@';
	szData[1] = 'C';
	szData[2] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[3] = (BYTE)nByteLength >> 0 & 0xFF;
	szData[4] = (BYTE)byChannel >> 8 & 0xFF;
	szData[5] = (BYTE)byChannel >> 0 & 0xFF;
	szData[6] = (BYTE)byCommand >> 8 & 0xFF;
	szData[7] = (BYTE)byCommand >> 0 & 0xFF;
	szData[8] = '@';
	szData[9] = 'C';
	szData[10] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[11] = (BYTE)nByteLength >> 0 & 0xFF;
	szData[12] = (BYTE)byChannel >> 8 & 0xFF;
	szData[13] = (BYTE)byChannel >> 0 & 0xFF;
	szData[14] = (BYTE)byCommand >> 8 & 0xFF;
	szData[15] = (BYTE)byCommand >> 0 & 0xFF;
	*/

	//int nRet = pSocket->SendToServer(szData, nByteLength);
	// Reconnect 추가. 2021-03-03
	int nRet = 0;
	
#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(szData, nByteLength);
#endif
	if (nRet > 0)
	{
		int nChannelNumber = CalcChannelNumber(byChannel, nBoardNumber);
		//LOG(Dbg, "Send Command Success Channel(%d), Command(0x%02x), Length(%d), Return(%d)", nChannelNumber, byCommand, nByteLength, nRet);

		CString strDbug = _T("");
		CString strTemp;
		for (int i = 0; i < nByteLength; ++i)
		{
			strTemp.Format("0x%02x   ", szData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_C DATA][BOARD:%d][CH:0x%02x][CH:%d] SEND DATA : %s", nBoardNumber, byChannel, nChannelNumber, strDbug);

		return true;
	}

	return false;
}

bool	CSockProc::SendBoardTypeC(int nBoardNumber, CArray< int, int >& arr, int nCommand)
{
	CClientSocket* pSocket = GetAt(nBoardNumber); 

	if (NULL == pSocket)
		return false;

		//if (FALSE == pSocket->GetConnect())
	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;

	char szData[200] = { 0, };

	memset(szData, NULL, 200);

	int nByteLength = 8;
	int nIndex		= 0;

	CString strChannel;

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		BYTE byChannel = 0;

			 if (arr.GetAt(i) % 4 == 0)	byChannel = 0x01;
		else if (arr.GetAt(i) % 4 == 1) byChannel = 0x02;
		else if (arr.GetAt(i) % 4 == 2) byChannel = 0x04;
		else if (arr.GetAt(i) % 4 == 3) byChannel = 0x08;

		szData[nIndex++] = '@';
		szData[nIndex++] = 'C';
		szData[nIndex++] = nByteLength >> 8 & 0xFF;
		szData[nIndex++] = nByteLength >> 0 & 0xFF;
		szData[nIndex++] = byChannel >> 8 & 0xFF;
		szData[nIndex++] = byChannel >> 0 & 0xFF;
		szData[nIndex++] = nCommand >> 8 & 0xFF;
		szData[nIndex++] = nCommand >> 0 & 0xFF;

		CString str;

		str.Format("%d", arr.GetAt(i));

		strChannel += str;
	}

	//int nRet = pSocket->SendToServer(szData, nByteLength * arr.GetSize());
	// Reconnect 추가. 2021-03-03
	int nRet = 0;
	
#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength * arr.GetSize());
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(szData, nByteLength * arr.GetSize());
#endif

	if (nRet > 0)
	{
		SYSLOG(Dbg, "Send Command Success Channel(%s), Command(0x%02x), Length(%d), Return(%d)", strChannel, nCommand, nByteLength, nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < nByteLength * arr.GetSize(); ++i)
		{
			strTemp.Format("%x   ", szData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_C_PARALLEL DATA][BOARD:%d] SEND DATA : %s", nBoardNumber, strDbug);

		return true;
	}

	return false;
}

CClientSocket*	CSockProc::GetAt(int nBoardNumber)
{
	auto Pos = m_listSocket.FindIndex(nBoardNumber);

	if (Pos)
	{
		return m_listSocket.GetAt(Pos);
	}

	return NULL;
}

CClientSocket*	CSockProc::GetAtChannel(int nChannelNumber)
{
	auto Pos = m_listSocket.GetHeadPosition();

	while (Pos)
	{
		CClientSocket* pSocket = m_listSocket.GetNext(Pos);

		if (NULL == pSocket)
			continue;

		CStringArray& arr = pSocket->GetChannelArray();

		for (int i = 0; i < arr.GetSize(); ++i)
		{
			if (atoi(arr.GetAt(i)) == nChannelNumber)
				return pSocket;
		}
	}

	return NULL;
}

CClientSocket*	CSockProc::GetAtKey(int nPrimaryKey)
{
	auto Pos = m_listSocket.GetHeadPosition();

	while (Pos)
	{
		CClientSocket* pSocket = m_listSocket.GetNext(Pos);

		if (NULL == pSocket)
			continue;

		if (pSocket->GetPrimaryKey() == nPrimaryKey)
			return pSocket;
	}

	return NULL;
}

int		CSockProc::CalcChannelNumber(BYTE byChannelNumber, int nBoardIndex)
{
	// Board 0 : 0, 1, 2, 3
	// Board 1 : 4, 5, 6, 7
	// Board 2 : 8, 9, 10, 11
	// Board 3 : 12, 13, 14, 15

	int nChannelNumber = 0;

	if (byChannelNumber == 0x01)
		nChannelNumber = 1;
	else if (byChannelNumber == 0x02)
		nChannelNumber = 2;
	else if (byChannelNumber == 0x04)
		nChannelNumber = 3;
	else if (byChannelNumber == 0x08)
		nChannelNumber = 4;

	int nRet = (nBoardIndex * 4) + nChannelNumber;

	return nRet;
}

bool	CSockProc::SendBoardTypeT(int nBoardNumber, BYTE byChannel)
{
	CClientSocket* pSocket = GetAt(nBoardNumber);

	if (NULL == pSocket)
		return false;

	//if (FALSE == pSocket->GetConnect())
	if (FALSE == pSocket->GetConnect(pSocket->GetPrimaryKey()))
		return false;

	int nChannelNumber = CMgrComm::GetMgr()->GetChannelNumber(eDeviceTypeIFBoard, nBoardNumber, byChannel);

	if (nChannelNumber < 0)
		return false;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);

	if (NULL == pChannel)
		return false;

	//CList< CStep*, CStep* >& listStep = pChannel->GetMgr()->GetStepList();

	char szData[15] = { 0, };

	memset(szData, NULL, 15);

	int nByteLength = 8;

	szData[0] = '@';
	szData[1] = 'T';
	szData[2] = (BYTE)nByteLength >> 8 & 0xFF;
	szData[3] = (BYTE)nByteLength >> 0 & 0xFF;
	szData[4] = (BYTE)byChannel >> 8 & 0xFF;
	szData[5] = (BYTE)byChannel >> 0 & 0xFF;

	//taek 현재 온도 값
	//chamber 연결 되어 있으면 현재 온도 값을 보낸다 1초 주기
	CMgrChamber* pMgrChamber = CMgrChamber::GetMgr();

	if (pMgrChamber)
	{
		SChamber* lpDeviceChamber = pMgrChamber->GetDeivceCyclerChannel(pChannel->GetChannelNumber());

		if (lpDeviceChamber)
		{
			if (abs(lpDeviceChamber->fCurrentValue) > 0.f)
			{
				if (lpDeviceChamber->fCurrentValue < 0.0f)
				{
					float fValue = abs(lpDeviceChamber->fCurrentValue * 10);

					szData[6] = (BYTE)fValue >> 8 & 0xFF;
					szData[6] = szData[6] |= (1 << 7);
					szData[7] = (BYTE)fValue >> 0 & 0xFF;
				}
				else
				{
					float fValue = lpDeviceChamber->fCurrentValue;
					int ivalue = (int)(fValue * 10);

					szData[6] = (BYTE)(ivalue >> 8) & 0xFF;
					szData[7] = (BYTE)ivalue >> 0 & 0xFF;
				}
			}
			else
			{
				szData[6] = (BYTE)0;
				szData[7] = (BYTE)0;
			}
		}
	}

	/* 20201102 turby 여기서 I/F board 와 sequence 상 문제가 있어 보인다. */
	/* Data Delay 현상이 발생하는 이유이다. */
#if 1
	//int nRet = pSocket->SendToServer(szData, nByteLength);
	int nRet = 0;
	
#if 0
	for (int retry = 0; retry < 3; retry++)
	{
		if (pSocket->GetConnect() == TRUE)
			nRet = pSocket->SendToServer(szData, nByteLength);
		if (nRet > 0)
			break;
		pSocket->CloseClientSocket();	delayMs(1000);
		pSocket->ConnectToServer();
	}
#else
	nRet = pSocket->SendToServer(szData, nByteLength );
#endif
	if (nRet > 0)
	{
		nChannelNumber = CalcChannelNumber(byChannel, nBoardNumber);

		SYSLOG(Dbg, "SEND START SUCCESS..Channel(%d), Length(%d), Return(%d)", nChannelNumber, nByteLength, nRet);

		CString strDbug = _T("");

		CString strTemp;
		for (int i = 0; i < nByteLength; ++i)
		{
			strTemp.Format("0x%02x", szData[i]);
			strDbug += strTemp;
		}
		SYSLOG(Dbg, "[SEND_BOARD_TYPE_T_NOMAL DATA][BOARD:%d][CH:0x%02x][CH:%d] %s", nBoardNumber, byChannel, nChannelNumber, strDbug);

		return true;
	}
#endif
	return false;
}