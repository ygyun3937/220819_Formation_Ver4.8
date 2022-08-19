#include "stdafx.h"
#include "CPeakCanFD.h"


CPeakCanFD::CPeakCanFD()
{
}


CPeakCanFD::~CPeakCanFD()
{
}

bool CPeakCanFD::InitPeakCan(void* pSetting)
{
	SBMSLOAD_PEAKCANFD* pCanSetting = (SBMSLOAD_PEAKCANFD*)pSetting;

	m_sSetting = *pCanSetting;

	// Peak Handle
	for (int i = 0; i < ePeakCH_MAX; ++i)
	{
		int nChannel = 0;
		//pCanSetting->nMasterIndex
		if (ePeakCH01 == i)
			nChannel = pCanSetting->nMasterIndex;
		else if (ePeakCH02 == i)
			nChannel = pCanSetting->nSlaveIndex;

		m_PCanHandle[i] = this->GetPCanandle((ePeakType)pCanSetting->nPeakConnectType, nChannel);

		if (PCAN_NONEBUS == m_PCanHandle[i])
			return false;

		m_PCanIndex[i] = nChannel;

		TPCANStatus stsResult;
		//int selectedIO = 256;
		//int selectedInterrupt = 3;


		m_strCanFDSetting.Format(_T("f_clock_mhz=%d, nom_brp=%d, nom_tseg1=%d, nom_tseg2=%d, nom_sjw=%d, data_brp=%d, data_tseg1=%d, data_tseg2=%d, data_sjw=%d"),
			pCanSetting->nClock / 1000,
			pCanSetting->sNominal.nBrp,
			pCanSetting->sNominal.nTseg1,
			pCanSetting->sNominal.nTseg2,
			pCanSetting->sNominal.nSjw,
			pCanSetting->sData.nBrp,
			pCanSetting->sData.nTseg1,
			pCanSetting->sData.nTseg2,
			pCanSetting->sData.nSjw
			);

		LOG_DEBUG(_T("CanFD Index[%d] Channel[%d] String[%s]"), m_sSetting.nCanIndex, nChannel, m_strCanFDSetting.GetString());

		stsResult = m_pPeakBasic->InitializeFD(m_PCanHandle[i], m_strCanFDSetting.GetBuffer());

		if (stsResult != 0)
		{
			LOG_DEBUG(_T("PeakCANFD CH[Index:%d] Initialize Error\n%s"), nChannel, GetErrorEnum(stsResult));
			return false;
		}
		
	}

	return true;
}

void CPeakCanFD::Release()
{
	for (int i = 0; i < ePeakCH_MAX; ++i)
	{
		if (m_pPeakBasic)
			m_pPeakBasic->Uninitialize(m_PCanHandle[i]);
	}
}

void CPeakCanFD::Recv(int nIdx /*= 0*/)
{
	if (nIdx >= ePeakCH_MAX)
		return;

	TPCANMsgFD CANMsgFD;
	TPCANTimestampFD CANTimeStampFD;
	TPCANStatus recvResult;

	recvResult = m_pPeakBasic->ReadFD(m_PCanHandle[nIdx], &CANMsgFD, &CANTimeStampFD);

	if (recvResult == PCAN_ERROR_OK)
	{
		m_sBMSPacket.dwID = CANMsgFD.ID;
		m_sBMSPacket.byDLC = CANMsgFD.DLC;
		m_sBMSPacket.eBMSMessageType = eBMS_MsgCanFD;
		m_sBMSPacket.nChannelNum = m_sSetting.nCanIndex;
		m_sBMSPacket.byChannelIndex = m_PCanIndex[nIdx];

		memcpy(m_sBMSPacket.unMsg.can, CANMsgFD.DATA, sizeof(CANMsgFD.DATA));
	}
}

void CPeakCanFD::Send(void* p)
{
	if (m_PCanHandle == PCAN_NONEBUS)
		return;

	SBMSPacket* pBmsData = (SBMSPacket*)p;

	if (pBmsData->byChannelIndex >= ePeakCH_MAX)
		return;

	TPCANMsgFD CanMsgFDSend;
	TPCANStatus stsResult;
	memset(&CanMsgFDSend, 0, sizeof(&CanMsgFDSend));
	CanMsgFDSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CanMsgFDSend.MSGTYPE |= PCAN_MESSAGE_FD;
	
	CanMsgFDSend.DLC = pBmsData->byDLC;
	CanMsgFDSend.ID = pBmsData->dwID;

	memcpy(CanMsgFDSend.DATA, pBmsData->unMsg.fdcan, sizeof(pBmsData->unMsg.fdcan));
	
	stsResult = m_pPeakBasic->WriteFD(m_PCanHandle[pBmsData->byChannelIndex], &CanMsgFDSend);

	if (stsResult != 0)
		m_pPeakBasic->Reset(m_PCanHandle[pBmsData->byChannelIndex]);
}

void CPeakCanFD::GetCanData(SBMSPacket& data)
{
	data = m_sBMSPacket;
	m_sBMSPacket.InitBMSPacket();
}
