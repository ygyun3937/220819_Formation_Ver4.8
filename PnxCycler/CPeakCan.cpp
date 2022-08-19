#include "stdafx.h"
#include "CPeakCan.h"


CPeakCan::CPeakCan()
{
}


CPeakCan::~CPeakCan()
{
}

bool CPeakCan::InitPeakCan(void* pSetting)
{
	SBMSLOAD_PEAKCAN* pCanSetting = (SBMSLOAD_PEAKCAN*)pSetting;


	m_sSetting = *pCanSetting;

	// Baudrate Setting
	if (5 == m_sSetting.nBaudrate)				m_PCanBaudrate = PCAN_BAUD_5K;
	else if (10 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_10K;
	else if (20 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_20K;
	else if (33 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_33K;
	else if (47 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_47K;
	else if (50 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_50K;
	else if (83 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_83K;
	else if (95 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_95K;
	else if (100 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_100K;
	else if (125 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_125K;
	else if (250 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_250K;
	else if (500 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_500K;
	else if (800 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_800K;
	else if (1000 == m_sSetting.nBaudrate)		m_PCanBaudrate = PCAN_BAUD_1M;
	else
		return false;

	m_sSetting.nBaudrate *= eByteMul;
	int nChannel = 0;
	// Peak Handle
	for (int i = 0; i <= ePeakCH02; ++i)
	{
		if (ePeakCH01 == i)
			nChannel = pCanSetting->nMasterIndex;
		else if (ePeakCH02 == i)
			nChannel = pCanSetting->nSlaveIndex;

		if (nChannel == -1)
			continue;

		m_PCanHandle[i] = this->GetPCanandle((ePeakType)pCanSetting->nPeakConnectType, nChannel);

		if (PCAN_NONEBUS == m_PCanHandle[i])
			return false;

		m_PCanIndex[i] = nChannel;

		TPCANStatus stsResult;
		//int selectedIO = 256;
		//int selectedInterrupt = 3;

		int selectedIO = 256;
		int selectedInterrupt = 3;
		BYTE hwType = 0;
		if (m_pPeakBasic)
			stsResult = m_pPeakBasic->Initialize(m_PCanHandle[i], m_PCanBaudrate, hwType, selectedIO, (WORD)selectedInterrupt);

		if (stsResult != 0)
		{
			LOG_DEBUG(_T("PeakCAN CH[Index:%d] Initialize Error\n%s"), nChannel, GetErrorEnum(stsResult));
			return false;
		}
	}

	return true;
}

void CPeakCan::Release()
{
	for (int i = 0; i < ePeakCH_MAX; ++i)
	{
		if (m_pPeakBasic)
			m_pPeakBasic->Uninitialize(m_PCanHandle[i]);
	}
}

void CPeakCan::Recv(int nIdx /*= 0*/)
{
	if (nIdx >= ePeakCH_MAX)
		return;

	TPCANMsg CANMsg;
	TPCANTimestamp CANTimeStamp;
	TPCANStatus recvResult;

	recvResult = m_pPeakBasic->Read(m_PCanHandle[nIdx], &CANMsg, &CANTimeStamp);

	if (recvResult == PCAN_ERROR_OK)
	{
		m_sBMSPacket.dwID = CANMsg.ID;
		m_sBMSPacket.byDLC = 8;
		m_sBMSPacket.eBMSMessageType = eBMS_MsgCan;
		m_sBMSPacket.nChannelNum = m_sSetting.nCanIndex;
		m_sBMSPacket.byChannelIndex = m_PCanIndex[nIdx];
		memcpy(m_sBMSPacket.unMsg.can, CANMsg.DATA, sizeof(CANMsg.DATA));
	}
}

void CPeakCan::Send(void* p)
{
	if (m_PCanHandle == PCAN_NONEBUS)
		return;
	
	SBMSPacket* pBmsData = (SBMSPacket*)p;

	if (pBmsData->byChannelIndex >= ePeakCH_MAX)
		return;

	TPCANMsg CanMsgSend;
	TPCANStatus stsResult;
	memset(&CanMsgSend, 0, sizeof(&CanMsgSend));
	if(pBmsData->dwID < 0x1000)
		CanMsgSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	else
		CanMsgSend.MSGTYPE = PCAN_MESSAGE_EXTENDED;
	CanMsgSend.LEN = 8;
	CanMsgSend.ID = pBmsData->dwID;
	
	// PCan은 최대 8개짜리를 사용할 수 있네
	memcpy(CanMsgSend.DATA, pBmsData->unMsg.can, sizeof(pBmsData->unMsg.can));
	stsResult = m_pPeakBasic->Write(m_PCanHandle[pBmsData->byChannelIndex], &CanMsgSend);

	if (stsResult != PCAN_ERROR_OK)
	{

	}
}

void CPeakCan::GetCanData(SBMSPacket& data)
{
	data = m_sBMSPacket;
	m_sBMSPacket.InitBMSPacket();
}
