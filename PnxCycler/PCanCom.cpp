#include "stdafx.h"
#include "PCanCom.h"

#include "PnxUtil.h"

#include "MgrCommData.h"
#include "MgrChannel.h"
#include "MgrSchedule.h"
#include "MgrConfig.h"
#include "MgrAlarm.h"
#include "MgrLog.h"
#include "MgrPCan.h"
#include "DlgViewAlarm.h"

#include "./include/PCAN/ProtocolCan.h"

CPCanCom::CPCanCom()
{
	m_pThread = NULL;
	m_bLoop = true;
	m_nPrimaryKey = 0;
	m_nChannelNo = 0;
	m_nBaurdrate = 0;
	m_nSetCommand = 0;
	m_nCanSystemMode = 0;
	m_bIsSetConnetUI = false;
	m_nLiveBitSend = 0;
	m_nLiveBitRecv = 0;
	m_bManual = false;
	m_pCanDsp.m_objPCANBasic = nullptr;

	Initialize();

	m_nInitUSBChannel = 0;
	m_nInitBaudrate = 0;
	m_bFirstRecvOK = false;

	SetInverInit(false);
	SetProgramClose(false);

	SetAlarmStatus(false);
	SetInverterStatus(eInverterStatusDisconnect);

	SetCloseMode(false);
	SetCloseStep(CloseStep_DisConnect);

	SetStartStep(StartStep_ErrorClear_Check);

	m_nTimeCount = 0;
}

CPCanCom::~CPCanCom()
{
	Dispose();
}

void CPCanCom::Initialize()
{
	InitializeVariables();
}
void CPCanCom::InitializeVariables()
{
	memset(&m_CanMsgSend, 0, sizeof(&m_CanMsgSend));
	m_CanMsgSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	m_CanMsgSend.LEN = 8;

	memset(&m_CanMsgCVSend, 0, sizeof(&m_CanMsgCVSend));
	m_CanMsgCVSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	m_CanMsgCVSend.LEN = 8;
	memset(&m_CANMsg, 0, sizeof(&m_CANMsg));
	memset(&m_CANMsgPara, 0, sizeof(&m_CANMsgPara));

	for (int nNo = 0; nNo < 7; nNo++) {
		m_CANMsg[nNo].MSGTYPE = PCAN_MESSAGE_STANDARD;
		m_CANMsg[nNo].LEN = 8;
		m_CANMsg[nNo].DATA[0] = 0x80;
		m_CANMsg[nNo].DATA[1] = 0x00;
		m_CANMsg[nNo].DATA[2] = 0x00;
		m_CANMsg[nNo].DATA[3] = 0x00;
		m_CANMsg[nNo].DATA[4] = 0x00;
		m_CANMsg[nNo].DATA[5] = 0x00;
		m_CANMsg[nNo].DATA[6] = 0x00;
		m_CANMsg[nNo].DATA[7] = 0x00;

		m_CANMsgPara[nNo].MSGTYPE = PCAN_MESSAGE_STANDARD;
		m_CANMsgPara[nNo].LEN = 8;
		m_CANMsgPara[nNo].DATA[0] = 0x80;
		m_CANMsgPara[nNo].DATA[1] = 0x01;
		m_CANMsgPara[nNo].DATA[2] = 0x00;
		m_CANMsgPara[nNo].DATA[3] = 0x00;
		m_CANMsgPara[nNo].DATA[4] = 0x00;
		m_CANMsgPara[nNo].DATA[5] = 0x00;
		m_CANMsgPara[nNo].DATA[6] = 0x00;
		m_CANMsgPara[nNo].DATA[7] = 0x00;
	}
	m_bCanOpenOk = false;
}
void CPCanCom::Dispose()
{
	if (m_pCanDsp.m_objPCANBasic)
	{
		delete m_pCanDsp.m_objPCANBasic;
		m_pCanDsp.m_objPCANBasic = nullptr;
	}
}

void CPCanCom::StopAllThread()
{
	StopThread();
}

bool CPCanCom::OpenCan()
{
	if (false == m_bCanOpenOk)
	{
		int channelNo = GetChannelNo(); //DSP1;

		// ------------------------------------------------

		//CString strTemp;
		//strTemp = _T("500K");
		//if (strTemp == "500K")
		//	m_pCanDsp.m_Baudrate = PCAN_BAUD_500K;
		//else if (strTemp == "250K")
		//	m_pCanDsp.m_Baudrate = PCAN_BAUD_250K;
		//else if (strTemp == "125K")
		//	m_pCanDsp.m_Baudrate = PCAN_BAUD_125K;
		//else if (strTemp == "100K")
		//	m_pCanDsp.m_Baudrate = PCAN_BAUD_100K;
		//else if (strTemp == "1M")

		if (500 == m_nInitBaudrate)
			m_pCanDsp.m_Baudrate = PCAN_BAUD_500K;
		else if (250 == m_nInitBaudrate)
			m_pCanDsp.m_Baudrate = PCAN_BAUD_250K;
		else if (125 == m_nInitBaudrate)
			m_pCanDsp.m_Baudrate = PCAN_BAUD_125K;
		else if (100 == m_nInitBaudrate)
			m_pCanDsp.m_Baudrate = PCAN_BAUD_100K;
		else if (1000 == m_nInitBaudrate)
			m_pCanDsp.m_Baudrate = PCAN_BAUD_1M;
		else
			m_pCanDsp.m_Baudrate = PCAN_BAUD_1M;

		// ------------------------------------------------

		if (nullptr == m_pCanDsp.m_objPCANBasic) {
			m_pCanDsp.m_objPCANBasic = new PCANBasicClass(); //32bit dll use
		}

		m_pCanDsp.m_ActiveReadingMode = 0;
		m_pCanDsp.m_PcanHandle = PCAN_USBBUS_DEFAULT + m_nInitUSBChannel;

		if (NULL == m_pCanDsp.m_hEvent) {
			m_pCanDsp.m_hEvent = CreateEvent(NULL, false, false, "");
		}

		TPCANStatus stsResult;
		int selectedIO = 256;
		int selectedInterrupt = 3;
		stsResult = m_pCanDsp.m_objPCANBasic->Initialize(m_pCanDsp.m_PcanHandle
			, m_pCanDsp.m_Baudrate
			, m_pCanDsp.m_HwType
			, selectedIO
			, (WORD)selectedInterrupt);

		if (stsResult != 0)
		{
			m_bCanOpenOk = false;
			return false;
		}

		printf("[PCAN][COMM I/F][CH:%d][BAUDRATE:%d][H/W_TYPE:%d][SELECTIO:%d]\n", channelNo, m_pCanDsp.m_Baudrate, m_pCanDsp.m_HwType, selectedIO);		// TEST_LOG
	}

	// ----------------------------------------------------------------------------

	for (int nNo = 0; nNo < DSP_CMD_MAX_COUNT; nNo++)


	{
		m_CANMsg[nNo].MSGTYPE = PCAN_MESSAGE_STANDARD;
		if (GetChannelNo() == 1) {
			m_CANMsg[nNo].ID = 0x10;
		}
		else if (GetChannelNo() == 2) {
			m_CANMsg[nNo].ID = 0x20;
		}
		else if (GetChannelNo() == 3) {
			m_CANMsg[nNo].ID = 0x30;
		}
		else if (GetChannelNo() == 4) {
			m_CANMsg[nNo].ID = 0x40;
		}
		else if (GetChannelNo() == 5) {
			m_CANMsg[nNo].ID = 0x50;
		}
		else {
			m_CANMsg[nNo].ID = 0x99;
		}
		
		if (nNo == DSP_CMD_00) {
			m_CANMsg[nNo].DATA[0] = 0x00;
			m_CANMsgPara[nNo].DATA[0] = 0x00;
		}
		else if (nNo == DSP_CMD_80) {
			m_CANMsg[nNo].DATA[0] = 0x80;
			m_CANMsgPara[nNo].DATA[0] = 0x80;
		}
		else if (nNo == DSP_CMD_81) {
			m_CANMsg[nNo].DATA[0] = 0x81;
			m_CANMsgPara[nNo].DATA[0] = 0x81;
		}
		else if (nNo == DSP_CMD_82) {
			m_CANMsg[nNo].DATA[0] = 0x82;
			m_CANMsgPara[nNo].DATA[0] = 0x82;
		}
		else if (nNo == DSP_CMD_83) {
			m_CANMsg[nNo].DATA[0] = 0x83;
			m_CANMsgPara[nNo].DATA[0] = 0x83;
		}
		else if (nNo == DSP_CMD_84) {
			m_CANMsg[nNo].DATA[0] = 0x84;
			m_CANMsgPara[nNo].DATA[0] = 0x84;
		}
		else if (nNo == DSP_CMD_85) {
			m_CANMsg[nNo].DATA[0] = 0x85;
			m_CANMsgPara[nNo].DATA[0] = 0x85;
		}
		else if (nNo == DSP_CMD_86) {
			m_CANMsg[nNo].DATA[0] = 0x86;
			m_CANMsgPara[nNo].DATA[0] = 0x86;
		}
		else if (nNo == DSP_CMD_87) {
			m_CANMsg[nNo].DATA[0] = 0x87;
			m_CANMsgPara[nNo].DATA[0] = 0x87;
		}
		else if (nNo == DSP_CMD_88) {
			m_CANMsg[nNo].DATA[0] = 0x88;
			m_CANMsgPara[nNo].DATA[0] = 0x88;
		}
		else if (nNo == DSP_CMD_89) {
			m_CANMsg[nNo].DATA[0] = 0x89;
			m_CANMsgPara[nNo].DATA[0] = 0x89;
		}

		// ----------------------------------------------------------------------------

		m_CANMsg[nNo].DATA[1] = 0x00;
		m_CANMsg[nNo].DATA[2] = 0x00;
		m_CANMsg[nNo].DATA[3] = 0x00;
		m_CANMsg[nNo].DATA[4] = 0x00;
		m_CANMsg[nNo].DATA[5] = 0x00;
		m_CANMsg[nNo].DATA[6] = 0x00;
		m_CANMsg[nNo].DATA[7] = 0x00;

		m_CANMsgPara[nNo].DATA[1] = 0x01;
		m_CANMsgPara[nNo].DATA[2] = 0x00;
		m_CANMsgPara[nNo].DATA[3] = 0x00;
		m_CANMsgPara[nNo].DATA[4] = 0x00;
		m_CANMsgPara[nNo].DATA[5] = 0x00;
		m_CANMsgPara[nNo].DATA[6] = 0x00;
		m_CANMsgPara[nNo].DATA[7] = 0x00;
	}

	m_bCanOpenOk = true;
	m_bLoop = true;

	SendDspCanMsgInitiate();
	return true;
}

bool CPCanCom::CloseCan()
{
	if (m_pCanDsp.m_hEvent != NULL) {
		m_pCanDsp.m_Terminated = true;
		m_pCanDsp.m_hEvent = NULL;
	}

	if (m_pCanDsp.m_objPCANBasic)
		m_pCanDsp.m_objPCANBasic->Uninitialize(m_pCanDsp.m_PcanHandle);

	m_bCanOpenOk = false;
	m_bFirstRecvOK = false;

	return true;
}

// -------------------------------------------------------------------------------------------------

void CPCanCom::StartThread()
{
	m_pThread = AfxBeginThread(ReadThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
}

void CPCanCom::StopThread()
{
	m_bLoop = false;

	if (m_pThread)
	{
		::WaitForSingleObject(m_pThread->m_hThread, INFINITE);

		m_pThread = NULL;
	}
}


void CPCanCom::Function_PcanStart(TPCANMsg * pCanMsg)
{
	int nStep = GetStartStep();

	switch (nStep)
	{
	case StartStep_ErrorClear:
		SendDisplayCanMegErrorClear();

		SetStartStep(StartStep_ErrorClear_Check);
		break;
	case StartStep_ErrorClear_Check:
		if (pCanMsg->DATA[0] == 0x99)
		{
			if (CDlgViewAlarm::GetMgr() != NULL)
			{
				CDlgViewAlarm::GetMgr()->UpdateStatus(true);
			}
			SendDspCanMsgInitiate();
			SetInverterStatus(eInverterStatusInit);
			SetStartStep(StartStep_Init_Check);
		}
		/////------ Test ????-----
		else
		{
			if (m_nTimeCount > 500)
			{
				m_nTimeCount = 0;
				SetStartStep(StartStep_Disconnect);
			}
			else
			{
				m_nTimeCount++;
			}
		}
		break;
	case StartStep_Disconnect:
		SendDisplayCanMsgDisConnect();
		SetStartStep(StartStep_Disconnect_Check);
		break;
	case StartStep_Disconnect_Check:
		if (pCanMsg->DATA[0] == 0x94)
		{
			if (pCanMsg->DATA[1] == 0x10)
			{
				SendDisplayCanMsgClose();
				SetStartStep(StartStep_Close_Check);
			}
		}
		break;
	case StartStep_Close_Check:
		if (pCanMsg->DATA[0] == 0x95)
		{
			if (pCanMsg->DATA[1] == 0x00)
			{
				SetStartStep(StartStep_ErrorClear);
			}
		}
		break;
		/////------ Test ????-----
	case StartStep_Init_Check:
		if (pCanMsg->DATA[0] == 0x10)
		{
			//??????????
			//DSP_SEND_CMD_INPUT_MC_ON
			//SendDisplayCanMessage();
			if (GetInverInit() == false)
			{
				SendDisplayCanMsgConnectionInput();
				SetInverterStatus(eInverterStatusInit);
				SetStartStep(StartStep_Input_On_Check);
			}
		}
		break;
	case StartStep_Input_On_Check:
		if (pCanMsg->DATA[0] == 0x90)
		{
			if (pCanMsg->DATA[1] == 0x10 && GetInverInit() == false)
			{
				if (SendDisplayCanMsgVoltCalcStart() == true)
				{
					Sleep(500);
					SendDisplayCanMsgConectionCmdStart();
					SetInverterStatus(eInverterStatusInputReadyOn);
					SetStartStep(StartStep_Start_Connecting);
					return;
				}
			}
		}
		break;
	case StartStep_Start_Connecting:
		if (pCanMsg->DATA[0] == 0x91)
		{

			if (pCanMsg->DATA[1] != 0x10)
			{
				SendDspCanMsgInitiate();
				SetInverterStatus(eInverterStatusInit);
				return;
			}

			Sleep(500);
			SetInverterStatus(eInverterStatusVoltCalcStart);
			SetStartStep(StartStep_Start_Connect_Complete);

		}
		break;
	case StartStep_Start_Connect_Complete:
		if (pCanMsg->DATA[0] == 0x91)
		{
			//220331 YGYUN ???????? ?? 20?? ???? ???????? ???? ???? --> ???? ????
			if (pCanMsg->DATA[1] != 0x20)
			{
				SendDisplayCanMsgVoltCalcStart();				
				SetInverterStatus(eInverterStatusInit);
				return;
			}

			//Sleep(500);
			SetInverterStatus(eInverterStatusConnectComplete);
			SetStartStep(StartStep_ErrorClear_Check);
			SetInverInit(true);

		}
		break;
	}
}

void CPCanCom::Function_PcanClose(TPCANMsg* pCanMsg)
{
	int nStep = GetCloseStep();
	
	switch (nStep)
	{
	case CloseStep_DisConnect:
		SendDisplayCanMsgDisConnect();
		SetCloseStep(CloseStep_ChargeDisCharge_DisConnect_Check);
		break;
	case CloseStep_ChargeDisCharge_DisConnect_Check:
		if (pCanMsg->DATA[0] == 0x94)
		{
			if (pCanMsg->DATA[1] == 0x10)
			{
				SendDisplayCanMsgClose();
				SetCloseStep(CloseStep_Program_Close_Check);
			}
		}
		break;
	case CloseStep_Program_Close_Check:
		if (pCanMsg->DATA[0] == 0x95)
		{
			if (pCanMsg->DATA[1] == 0x00)
			{
				SetCloseStep(CloseStep_DisConnect);
				SetProgramClose(true);
			}
		}
		break;
	}
}

void CPCanCom::Function_AlarmClear()
{
	SendDisplayCanMegErrorClear();
}

UINT CPCanCom::ReadThread(LPVOID pParam)
{
	CPCanCom*	pCom = (CPCanCom*)pParam;

	if (pCom->m_bLoop == NULL)
	{
		return 0;
	}


	while (pCom->m_bLoop)
	{
		
		pCom->DispatchReceiveData();
		Sleep(1);
	}
	return 0;
}

void CPCanCom::DispatchReceiveData()
{
	if (false == m_bCanOpenOk)
		return;

	TPCANMsg CANMsg;
	TPCANTimestamp CANTimeStamp;
	TPCANStatus recvResult;
	//CPCanCom* pPcan = new CPCanCom;
	CString strAlarmText;
	CMgrPCan* pMgr = CMgrPCan::GetMgr();
	CString str = "";
	recvResult = m_pCanDsp.m_objPCANBasic->Read(m_pCanDsp.m_PcanHandle, &CANMsg, &CANTimeStamp);
	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;


	auto Pos = listChannel.GetHeadPosition();

	if (recvResult == PCAN_ERROR_OK)
	{
		CString strDebug;
		strDebug.Format(_T("RECEIVE CAN MSG [ID:%d] : %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x"),
			CANMsg.ID
			, CANMsg.DATA[0], CANMsg.DATA[1], CANMsg.DATA[2], CANMsg.DATA[3]
			, CANMsg.DATA[4], CANMsg.DATA[5], CANMsg.DATA[6], CANMsg.DATA[7]);
		printf("[PCAN][COMM I/F][CAN_MSG_RESV:%s]\n", strDebug);		// TEST_LOG

	/*	if (pChannel == NULL) {
			return;
		}*/

		int nRecvDataType = 0;

		if (m_nCheckID != 0 && m_bFirstRecvOK == false)
		{
			if (m_nCheckID == CANMsg.ID) {
				nRecvDataType = REV_CAN_ID_DATA_OFFSET;
			}
		}
		else
		{
			for (int nIndex = 0; nIndex < m_strRecvCanIdArray.GetCount(); nIndex++)
			{
				long temp = atol(m_strRecvCanIdArray[nIndex]);

				if (CANMsg.ID == atol(m_strRecvCanIdArray[nIndex]) + REV_CAN_ID_DATA_OFFSET)
				{
					nRecvDataType = REV_CAN_ID_DATA_OFFSET;
					break;
				}
				if (CANMsg.ID == atol(m_strRecvCanIdArray[nIndex]) + REV_CAN_ID_ERRCODE_OFFSET)
				{
					nRecvDataType = REV_CAN_ID_ERRCODE_OFFSET;
					break;
				}
				if (CANMsg.ID == atol(m_strRecvCanIdArray[nIndex]) + REV_CAN_ID_PATCOUNT_OFFSET)
				{
					nRecvDataType = REV_CAN_ID_PATCOUNT_OFFSET;
					break;
				}
			}
		}

		if (nRecvDataType == 0) {
			return;
		}

		// ---------------------------------------------------------------------------

		if (nRecvDataType == REV_CAN_ID_DATA_OFFSET)
		{
			m_bFirstRecvOK = true;
			if (m_bCloseMode == true)
			{
				Function_PcanClose(&CANMsg);
			}
			else if (m_bManual == false)
			{
				//SendDisplayCanMessage();

				ReceiveCanDataProcess(&CANMsg);
				//SendDspCanMsg();
			}
			else
			{
				ManualCanReceive(&CANMsg);
				ManualCanSend(m_stManualRef);
			}
		}
		else if (nRecvDataType == REV_CAN_ID_ERRCODE_OFFSET)
		{
			int nIGBT, nErr1, nErr2, nErr3, nErr4;

			nIGBT = (CANMsg.DATA[0] << 8 & 0xF00) | (CANMsg.DATA[1]);				// IGBT //20190409 ???? 0000
			nErr1 = (CANMsg.DATA[2] << 4 & 0xFF0) | (CANMsg.DATA[3] >> 4 & 0xF);	// nWarn 0000
			nErr2 = (CANMsg.DATA[3] << 8 & 0xF00) | (CANMsg.DATA[4] & 0xFF);		// nGrid 0001
			nErr3 = (CANMsg.DATA[5] << 4 & 0xFF0) | (CANMsg.DATA[6] >> 4 & 0xF);	// nDc 0000
			nErr4 = (CANMsg.DATA[6] << 8 & 0xF00) | (CANMsg.DATA[7] & 0xFF);		// nFault 0000

			//20220225 YGY ?????????? ???? ?????? ??????
			for (int AlarmType = eAlarmInverterTypeCANIGBT; AlarmType < eAlarmInverterTypeMaxCount; ++AlarmType)
			{
				//Channel :4,5
				//Step : 6,7			
				//Waring : 8,9			CanAlarmWaring, IFAlarmWaring,	SerialAlarmWaring	16bit
				//Grid : 10,11			CanAlarmGrid,	IFAlarmGrid,	SerialAlarmGrid		16bit		
				//DC : 12,13			CanAlarmDC,		IFAlarmDC,		SearialAlarmDC		16bit
				//Fault : 14,15			CanAlarmFault,	IFAlarmFault,	SerialAlarmFault	16bit
				//???? ???? ???? ???????? ???? ???? ?????? ?????? ???? ????????

				if (AlarmType < eAlarmInverterTypeCANGrid)
				{
					str = CMgrAlarm::GetMgr()->AddInverterAlarmCan(GetChannelNo(), AlarmType, (BYTE)CANMsg.DATA[(AlarmType * 2)], (BYTE)CANMsg.DATA[(AlarmType * 2) + 1]);
				}
				else if (AlarmType > eAlarmInverterTypeCANGrid)
				{
					str = CMgrAlarm::GetMgr()->AddInverterAlarmCan(GetChannelNo(), AlarmType, (BYTE)CANMsg.DATA[(AlarmType + 2)], (BYTE)CANMsg.DATA[(AlarmType + 2) + 1]);
				}
				else
				{
					str = CMgrAlarm::GetMgr()->AddInverterAlarmCan(GetChannelNo(), AlarmType, (BYTE)CANMsg.DATA[(AlarmType + 1)], (BYTE)CANMsg.DATA[(AlarmType + 2)]);
				}

			}
			CAlarmInverterMap* pMapAlarm = CMgrAlarm::GetMgr()->GetInverterAlarmMap(GetChannelNo());

			if (pMapAlarm)
			{
				if (true == pMapAlarm->ExistAlarm())
				{
					SetAlarmStatus(true);
					SetInverInit(false);
					SetInverterStatus(eInverterStatusError);
				
					//???? ?? Channel?? ???? ?????? ???? ???? ????????.
					while (Pos)
					{
						CChannel* pChannel = listChannel.GetNext(Pos);

						if (NULL == pChannel)
							continue;

						if (pChannel->GetChannelNumber() / nStageChCount != GetChannelNo())
							continue;

						pChannel->SetChannelStatus(eCyclerChannelStateError);
						pChannel->SetPause(true);
						pChannel->SetScheduleStart(false);//hsm 20190601


						if (CMgrConfig::GetMgr()->GetEQMode() == EQModeParallel)
						{
							CChannel* pParallelCh = NULL;
							if (pChannel->GetParallelObj() == ParallelMaster) // ?????? ?? ????
								pParallelCh = CMgrChannel::GetMgr()->GetChannel(pChannel->GetSlaveChannel());
							else
								pParallelCh = CMgrChannel::GetMgr()->GetChannel(pChannel->GetMasterChannel());

							if (pParallelCh != NULL)
							{
								pParallelCh->SetChannelStatus(eCyclerChannelStateError);
								pParallelCh->SetPause(true);
							}
						}

						if (false == strAlarmText.IsEmpty())
							pChannel->SetEndCondition(strAlarmText);


					}
				}
				else
				{//???? ?? Channel?? ???? ?????? ???? ???? ????????.
					while (Pos)
					{
						CChannel* pChannel = listChannel.GetNext(Pos);

						if (NULL == pChannel)
							continue;

						if (pChannel->GetChannelNumber() / nStageChCount != GetChannelNo())
							continue;
						//220302 YGY ???? Alarm ?????? ?????????? ???? FW?? ?????? ???? ?????? ???? ???? ?????????????? ???? ????????
					//???? ???? ???? : ???? ???? ?? ???? ?????? ???? ?????? ?????? FW ???? ????
						//hsm 20190601
						if (pChannel->GetChannelStatus() == eCyclerChannelStateError)
						{
							//pChannel->SetChannelStatus(ChannelStatusReady);
							//// 						pChannel->SetEndCondition("");
							//pChannel->SetPause(false);
						}
					}
				}
			}
		}
		else
		{
			//TRACE(_T("ID : 0x%x\n"), CANMsg.ID);
		}
	}

	// ------------------------------------------------------------------------------------------------

	CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(0);
	int nSendGap = m_nLiveBitSend - pCommData->GetSendMessageCount();
	int nRecvGap = m_nLiveBitSend - pCommData->GetRecvMessageCount();
	const int nDelay = 3000;

	if (nSendGap >= nDelay || nRecvGap >= nDelay)
	{
		if (m_bIsSetConnetUI == true)
		{
			CWnd* pWnd = AfxGetApp()->GetMainWnd();

			if (pWnd)
			{
				::PostMessage(pWnd->GetSafeHwnd(), WM_DSP_PORT_STATUS, 0, false);

				SYSLOG(Dbg, _T("void	CPCanCom::ReceiveDataProcess() -> PCAN ???? ??????"));
			}
			m_bIsSetConnetUI = false;
		}
	}
	else
	{
		if (m_bIsSetConnetUI == false)
		{
			CWnd* pWnd = AfxGetApp()->GetMainWnd();

			if (pWnd) {
				::PostMessage(pWnd->GetSafeHwnd(), WM_DSP_PORT_STATUS, 0, true);
			}

			m_bIsSetConnetUI = true;
		}
	}
	m_nLiveBitSend++;
	m_nLiveBitRecv++;

}
/**
 * .
 *
 * METHOD_NAME : ReceiveCanDataProcess
 * METHOD_TYPE : -
 * DESCRIPTION :
 */
void CPCanCom::ReceiveCanDataProcess(TPCANMsg * pCanMsg)
{
	//CPCanCom* pPcan = new CPCanCom;
	int nIGBT, nErr1, nErr2, nErr3, nErr4;
	CMgrPCan* pMgrPCan = CMgrPCan::GetMgr();


	MSG  Message;
	if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
	{
		if (Message.message == WM_QUIT)
		{
			PostQuitMessage(0);
			return;
		}
		else
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
	CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(GetChannelNo());

	pCommData->SetSendMessageCount(0, FuncAgrsIncrease);

	m_nLiveBitSend = pCommData->GetSendMessageCount();

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(GetChannelNo());

	// -----------------------------------------------------------------------------------------

	if (m_bManual == true) {
		ManualCanSend(m_stManualRef);
	}
	if (pCanMsg->DATA[0] == 0x99)
	{
		if (GetAlarmStatus() == true)
		{
			Sleep(100);
			SendDspCanMsgInitiate();
			SetAlarmStatus(false);
			SetStartStep(StartStep_ErrorClear);

		}
	}
	if (pChannel->GetScheduleStart() == true && pChannel->GetPause() == FALSE
		|| (pChannel->GetPause() == true && (pChannel->GetChannelStatus() == eCyclerChannelStatePauseFromTempSync
			|| pChannel->GetChannelStatus() == eCyclerChannelStatePauseFromStepSync)))
	{
		//SendSCH();
		return;
	}
	else
	{
		if (GetInverInit() == false)
		{
			Function_PcanStart(pCanMsg);
		}

	}
		// ????????
}

// -------------------------------------------------------------------------------------------------


bool CPCanCom::SendDisplayCanMessage()
{
	CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(GetChannelNo());

	pCommData->SetSendMessageCount(0, FuncAgrsIncrease);

	m_nLiveBitSend = pCommData->GetSendMessageCount();

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(GetChannelNo());

	int nEQMode = CMgrConfig::GetMgr()->GetEQMode();
	m_CanMsgSend.ID = (DWORD)_ttoi((LPCTSTR)m_strRecvCanIdArray[nEQMode]);

	// ???????????? ?????????? ???????? ?????? ??????.
	if (nEQMode == EQModeParallel && GetChannelNo() == pChannel->GetSlaveChannel()) {
		return true;
	}

	// -----------------------------------------------------------------------------------------

	if (m_bManual == true) {
		ManualCanSend(m_stManualRef);
	}

	if (pChannel->GetScheduleStart() == true && pChannel->GetPause() == FALSE
		|| (pChannel->GetPause() == true && (pChannel->GetChannelStatus() == eCyclerChannelStatePauseFromStepSync
			|| pChannel->GetChannelStatus() == eCyclerChannelStatePauseFromTempSync)))
	{
		//SendSCH();
		return true;
	}
	else
	{
		int nCommand = GetSendCanCommand();
		//hsm 20190601
		if (pChannel->GetPause() == true)
		{
			// ???? ?????? ????????????..
			if (pChannel->GetChannelStatus() == eCyclerChannelStateError)
			{
				if (nCommand == DSP_SEND_CMD_ERROR_CLEAR) {
					nCommand = DSP_SEND_CMD_ERROR_CLEAR;
				}
			}
			else if (nCommand != DSP_SEND_CMD_STOP_OUTPUT_OFF || nCommand != DSP_SEND_CMD_OUTPUT_OFF)
			{
				// DSP ?????? DSP_CMD_86 ???? ?????? ????. 
				// ???????? ?????? DSP_CMD_86?? ???????? ???????? ??????????
				nCommand = DSP_SEND_CMD_STOP;

				// ????????????
				if (pChannel->GetChannelStatus() != eCyclerChannelStatePauseFromStepSync
					&& pChannel->GetChannelStatus() != eCyclerChannelStatePauseFromTempSync) {
					pChannel->SetChannelStatus(eCyclerChannelStatePause);
				}
			}
		}

		// ---------------------------------------------------------------------------

		switch (nCommand) //???? ????=1, ???? ????=2, ???? ????=3
		{
		case DSP_SEND_CMD_INITIATE:
			//00 : ???? ???? ???? ?????? ????
			for (int dataCount = 0; dataCount < 8; dataCount++) {
				if (CMgrConfig::GetMgr()->GetEQMode() == EqModeSerial) {
					m_CanMsgSend.DATA[dataCount] = m_CANMsg[DSP_CMD_00].DATA[dataCount];		//SEND_CAN_ID_CH4;
				}
				else {
					m_CanMsgSend.DATA[dataCount] = m_CANMsgPara[DSP_CMD_00].DATA[dataCount];	//SEND_CAN_ID_CH4;	
				}
			}
			break;

		case 0x92:
		case DSP_SEND_CMD_INPUT_ON:
			//80 //???? MC ON  ---> ???? ???? ???? MC ON :: 0:0
			for (int dataCount = 0; dataCount < 8; dataCount++) {
				if (CMgrConfig::GetMgr()->GetEQMode() == EqModeSerial) {

					m_CanMsgSend.DATA[dataCount] = m_CANMsg[DSP_CMD_80].DATA[dataCount];		//SEND_CAN_ID_CH4;
				}
				else {
					m_CanMsgSend.DATA[dataCount] = m_CANMsgPara[DSP_CMD_80].DATA[dataCount];	//SEND_CAN_ID_CH4;	
				}
			}
			break;

		case DSP_SEND_CMD_OUTPUT_ON:
			//???? MC ON :: 4:1
			for (int dataCount = 0; dataCount < 8; dataCount++) {
				if (CMgrConfig::GetMgr()->GetEQMode() == EqModeSerial) {
					//m_CanMsgSend.DATA[dataCount] = m_CANMsg[DSP_CMD_81].DATA[dataCount];		//SEND_CAN_ID_CH4;	
				}
				else {
					//m_CanMsgSend.DATA[dataCount] = m_CANMsgPara[DSP_CMD_81].DATA[dataCount];	//SEND_CAN_ID_CH4;	
				}
			}
			break;

		case DSP_SEND_CMD_OUTPUT_OFF:
			//???? MC OFF :: 5:3
			for (int dataCount = 0; dataCount < 8; dataCount++) {
				if (CMgrConfig::GetMgr()->GetEQMode() == EqModeSerial) {
					//m_CanMsgSend.DATA[dataCount] = m_CANMsg[DSP_CMD_84].DATA[dataCount];		//SEND_CAN_ID_CH4;	
				}
				else {
					//m_CanMsgSend.DATA[dataCount] = m_CANMsgPara[DSP_CMD_84].DATA[dataCount];	//SEND_CAN_ID_CH4;	
				}
			}
			break;

		case DSP_SEND_CMD_ALL_OFF:
			//?????? MC OFF :: 5:3
			for (int dataCount = 0; dataCount < 8; dataCount++) {
				if (CMgrConfig::GetMgr()->GetEQMode() == EqModeSerial) {
					//m_CanMsgSend.DATA[dataCount] = m_CANMsg[DSP_CMD_85].DATA[dataCount];		//SEND_CAN_ID_CH4;	
				}
				else {
					//m_CanMsgSend.DATA[dataCount] = m_CANMsgPara[DSP_CMD_85].DATA[dataCount];	//SEND_CAN_ID_CH4;	
				}
			}
			break;

		case DSP_SEND_CMD_STOP_OUTPUT_OFF:
			//???? ????->?????? ???? ?????? ????????
			for (int dataCount = 0; dataCount < 8; dataCount++) {
				if (CMgrConfig::GetMgr()->GetEQMode() == EqModeSerial) {
					//m_CanMsgSend.DATA[dataCount] = m_CANMsg[DSP_CMD_86].DATA[dataCount];		//SEND_CAN_ID_CH4;	
				}
				else {
					//m_CanMsgSend.DATA[dataCount] = m_CANMsgPara[DSP_CMD_86].DATA[dataCount];	//SEND_CAN_ID_CH4;	
				}
			}

			//?????? ???????????? ?????? ???? ???????? ???????? ?????? ?????? ???? 
			//pCom->SetSendCanCmd(DSP_SEND_CMD_STOP_OUTPUT_OFF); ????..
			SetSendCanCommand(DSP_SEND_CMD_OUTPUT_OFF);
			break;

		case DSP_SEND_CMD_STOP:
			//???? ???? or ??????
			for (int dataCount = 0; dataCount < 8; dataCount++) {
				if (CMgrConfig::GetMgr()->GetEQMode() == EqModeSerial) {
					//m_CanMsgSend.DATA[dataCount] = m_CANMsg[DSP_CMD_86].DATA[dataCount];		//SEND_CAN_ID_CH4;	
				}
				else {
					//m_CanMsgSend.DATA[dataCount] = m_CANMsgPara[DSP_CMD_86].DATA[dataCount];	//SEND_CAN_ID_CH4;	
				}

			}
			break;

		case DSP_SEND_CMD_ERROR_CLEAR:
			//Error Clear :: 7:6
			for (int dataCount = 0; dataCount < 8; dataCount++) {
				if (CMgrConfig::GetMgr()->GetEQMode() == EqModeSerial) {
					m_CanMsgSend.DATA[dataCount] = m_CANMsg[DSP_CMD_89].DATA[dataCount];			//SEND_CAN_ID_CH4;
				}
				else {
					m_CanMsgSend.DATA[dataCount] = m_CANMsgPara[DSP_CMD_89].DATA[dataCount];		//SEND_CAN_ID_CH4;	
				}
			}
			break;

		default:
			//sch
			break;
		}

	}

	// ------------------------------------------------------------------------------------------------

	m_nSetCommand = m_CanMsgSend.DATA[0];
	m_stsResult = m_pCanDsp.m_objPCANBasic->Write(m_pCanDsp.m_PcanHandle, &m_CanMsgSend);

	printf("[PCAN][COMM I/F][SET_COMMAND:%d][RESULT:%s]\n", m_nSetCommand, m_stsResult);		// TEST_LOG

	// ------------------------------------------------------------------------------------------------

	CString strDebug;
	strDebug.Format(_T("SEND CAN MSG [ID:%d] : %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x"),
		m_CanMsgSend.ID
		, m_CanMsgSend.DATA[0], m_CanMsgSend.DATA[1], m_CanMsgSend.DATA[2], m_CanMsgSend.DATA[3]
		, m_CanMsgSend.DATA[4], m_CanMsgSend.DATA[5], m_CanMsgSend.DATA[6], m_CanMsgSend.DATA[7]);
	//LOG(Dbg, strDebug.GetString());
	printf("[PCAN][COMM I/F][CAN_MSG_SEND:%s]\n", strDebug);		// TEST_LOG

	if (m_stsResult == PCAN_ERROR_OK) {}
	else
	{
		CString strTemp = _T("");
		strTemp.Format("CAN%d:ERROR(write) - Message was successfully ERR1.\n", m_nChannelNo);
		TRACE(strTemp);
	}

	return true;
}

bool CPCanCom::SendDspCanMsgInitiate()
{
	CString strTemp = _T("");
	TPCANMsg CanMsgSend;
	TPCANStatus stsResult;
	memset(&CanMsgSend, 0, sizeof(&CanMsgSend));
	CanMsgSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CanMsgSend.LEN = 8;

	int nEQMode = CMgrConfig::GetMgr()->GetEQMode();
	CanMsgSend.ID = 0x0100;

	CanMsgSend.DATA[0] = 0x00;
	CanMsgSend.DATA[1] = 0x00;
	CanMsgSend.DATA[2] = 0x00;
	CanMsgSend.DATA[3] = 0x00;
	CanMsgSend.DATA[4] = 0x00;
	CanMsgSend.DATA[5] = 0x00;
	CanMsgSend.DATA[6] = 0x00;
	CanMsgSend.DATA[7] = 0x00;

	stsResult = m_pCanDsp.m_objPCANBasic->Write(m_pCanDsp.m_PcanHandle, &CanMsgSend);

	CString strDebug;
	strDebug.Format(_T("SEND CAN MSG [ID:%d] : %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x"),
		CanMsgSend.ID
		, CanMsgSend.DATA[0], CanMsgSend.DATA[1], CanMsgSend.DATA[2], CanMsgSend.DATA[3]
		, CanMsgSend.DATA[4], CanMsgSend.DATA[5], CanMsgSend.DATA[6], CanMsgSend.DATA[7]);
	printf("[PCAN][COMM I/F][SendCanMsgInitiate:%s]\n", strDebug);

	if (PCAN_ERROR_OK == stsResult)
	{
		CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(GetChannelNo());
		if (pCommData) {
			pCommData->SetSendMessageCount(0, FuncAgrsIncrease);
		}

		return true;
	}

	return false;
}

bool CPCanCom::SendDisplayCanMsgConnectionInput()
{
	CString strTemp = _T("");
	TPCANMsg CanMsgSend;
	TPCANStatus stsResult;
	memset(&CanMsgSend, 0, sizeof(&CanMsgSend));
	CanMsgSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CanMsgSend.LEN = 8;

	CanMsgSend.ID = 0x0100;

	CanMsgSend.DATA[0] = 0x80;
	CanMsgSend.DATA[1] = 0x00;
	CanMsgSend.DATA[2] = 0x00;
	CanMsgSend.DATA[3] = 0x00;
	CanMsgSend.DATA[4] = 0x00;
	CanMsgSend.DATA[5] = 0x00;
	CanMsgSend.DATA[6] = 0x00;
	CanMsgSend.DATA[7] = 0x00;

	stsResult = m_pCanDsp.m_objPCANBasic->Write(m_pCanDsp.m_PcanHandle, &CanMsgSend);

	CString strDebug;
	strDebug.Format(_T("SEND CAN MSG [ID:%d] : %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x"),
		CanMsgSend.ID
		, CanMsgSend.DATA[0], CanMsgSend.DATA[1], CanMsgSend.DATA[2], CanMsgSend.DATA[3]
		, CanMsgSend.DATA[4], CanMsgSend.DATA[5], CanMsgSend.DATA[6], CanMsgSend.DATA[7]);
	printf("[PCAN][COMM I/F][SendDisplayCanMsgConnectionInput:%s]\n", strDebug);

	if (PCAN_ERROR_OK == stsResult)
	{
		CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(GetChannelNo());
		if (pCommData)
			pCommData->SetSendMessageCount(0, FuncAgrsIncrease);

		return true;
	}

	return false;
}

bool CPCanCom::SendDisplayCanMsgVoltCalcStart()
{
	CString strTemp = _T("");
	TPCANMsg CanMsgSend;
	TPCANStatus stsResult;
	memset(&CanMsgSend, 0, sizeof(&CanMsgSend));
	CanMsgSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CanMsgSend.LEN = 8;

	CanMsgSend.ID = 0xAA;

	CanMsgSend.DATA[0] = 0x00;
	CanMsgSend.DATA[1] = 0x00;
	CanMsgSend.DATA[2] = 0x00;
	CanMsgSend.DATA[3] = 0x00;
	CanMsgSend.DATA[4] = 0x00;
	CanMsgSend.DATA[5] = 0x00;
	CanMsgSend.DATA[6] = 0x00;
	CanMsgSend.DATA[7] = 0x00;

	stsResult = m_pCanDsp.m_objPCANBasic->Write(m_pCanDsp.m_PcanHandle, &CanMsgSend);

	CString strDebug;
	strDebug.Format(_T("SEND CAN MSG [ID:%d] : %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x"),
		CanMsgSend.ID
		, CanMsgSend.DATA[0], CanMsgSend.DATA[1], CanMsgSend.DATA[2], CanMsgSend.DATA[3]
		, CanMsgSend.DATA[4], CanMsgSend.DATA[5], CanMsgSend.DATA[6], CanMsgSend.DATA[7]);
	printf("[PCAN][COMM I/F][SendDisplayCanMsgVoltCalcStart:%s]\n", strDebug);

	if (PCAN_ERROR_OK == stsResult)
	{
		CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(GetChannelNo());
		if (pCommData)
			pCommData->SetSendMessageCount(0, FuncAgrsIncrease);

		return true;
	}
	return false;
}

bool CPCanCom::SendDisplayCanMsgConectionCmdStart()
{
	CString strTemp = _T("");
	TPCANMsg CanMsgSend;
	TPCANStatus stsResult;
	memset(&CanMsgSend, 0, sizeof(&CanMsgSend));
	CanMsgSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CanMsgSend.LEN = 8;

	CanMsgSend.ID = 0x0100;

	CanMsgSend.DATA[0] = 0x81;
	CanMsgSend.DATA[1] = 0x00;
	CanMsgSend.DATA[2] = 0x00;
	CanMsgSend.DATA[3] = 0x00;
	CanMsgSend.DATA[4] = 0x00;
	CanMsgSend.DATA[5] = 0x00;
	CanMsgSend.DATA[6] = 0x00;
	CanMsgSend.DATA[7] = 0x00;

	stsResult = m_pCanDsp.m_objPCANBasic->Write(m_pCanDsp.m_PcanHandle, &CanMsgSend);

	CString strDebug;
	strDebug.Format(_T("SEND CAN MSG [ID:%d] : %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x"),
		CanMsgSend.ID
		, CanMsgSend.DATA[0], CanMsgSend.DATA[1], CanMsgSend.DATA[2], CanMsgSend.DATA[3]
		, CanMsgSend.DATA[4], CanMsgSend.DATA[5], CanMsgSend.DATA[6], CanMsgSend.DATA[7]);
	printf("[PCAN][COMM I/F][SendDisplayCanMsgVoltCalcStart:%s]\n", strDebug);

	if (PCAN_ERROR_OK == stsResult)
	{
		CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(GetChannelNo());
		if (pCommData)
			pCommData->SetSendMessageCount(0, FuncAgrsIncrease);

		return true;
	}

	return false;
}

bool CPCanCom::SendDisplayCanMegErrorClear()
{
	CString strTemp = _T("");
	TPCANMsg CanMsgSend;
	TPCANStatus stsResult;
	memset(&CanMsgSend, 0, sizeof(&CanMsgSend));
	CanMsgSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CanMsgSend.LEN = 8;


 	CanMsgSend.ID = 0x0100;
	CanMsgSend.DATA[0] = 0x89;
	CanMsgSend.DATA[1] = 0x00;
	CanMsgSend.DATA[2] = 0x00;
	CanMsgSend.DATA[3] = 0x00;
	CanMsgSend.DATA[4] = 0x00;
	CanMsgSend.DATA[5] = 0x00;
	CanMsgSend.DATA[6] = 0x00;
	CanMsgSend.DATA[7] = 0x00;

	stsResult = m_pCanDsp.m_objPCANBasic->Write(m_pCanDsp.m_PcanHandle, &CanMsgSend);

	CString strDebug;
	strDebug.Format(_T("SEND CAN MSG [ID:%d] : %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x"),
		CanMsgSend.ID
		, CanMsgSend.DATA[0], CanMsgSend.DATA[1], CanMsgSend.DATA[2], CanMsgSend.DATA[3]
		, CanMsgSend.DATA[4], CanMsgSend.DATA[5], CanMsgSend.DATA[6], CanMsgSend.DATA[7]);
	printf("[PCAN][COMM I/F][SendDisplayCanMegErrorClear:%s]\n", strDebug);

	if (PCAN_ERROR_OK == stsResult)
	{
		CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(GetChannelNo());
		if (pCommData)
			pCommData->SetSendMessageCount(0, FuncAgrsIncrease);

		return true;
	}

	return false;
}

bool CPCanCom::SendDisplayCanMsgRest()
{
	CString strTemp = _T("");
	TPCANMsg CanMsgSend;
	TPCANStatus stsResult;
	memset(&CanMsgSend, 0, sizeof(&CanMsgSend));
	CanMsgSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CanMsgSend.LEN = 8;

	CanMsgSend.ID = 0x0100;

	CanMsgSend.DATA[0] = 0x86;
	CanMsgSend.DATA[1] = 0x00;
	CanMsgSend.DATA[2] = 0x00;
	CanMsgSend.DATA[3] = 0x00;
	CanMsgSend.DATA[4] = 0x00;
	CanMsgSend.DATA[5] = 0x00;
	CanMsgSend.DATA[6] = 0x00;
	CanMsgSend.DATA[7] = 0x00;

	stsResult = m_pCanDsp.m_objPCANBasic->Write(m_pCanDsp.m_PcanHandle, &CanMsgSend);

	CString strDebug;
	strDebug.Format(_T("SEND CAN MSG [ID:%d] : %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x"),
		CanMsgSend.ID
		, CanMsgSend.DATA[0], CanMsgSend.DATA[1], CanMsgSend.DATA[2], CanMsgSend.DATA[3]
		, CanMsgSend.DATA[4], CanMsgSend.DATA[5], CanMsgSend.DATA[6], CanMsgSend.DATA[7]);
	printf("[PCAN][COMM I/F][SendDisplayCanMsgRest:%s]\n", strDebug);

	if (PCAN_ERROR_OK == stsResult)
	{
		CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(GetChannelNo());
		if (pCommData)
			pCommData->SetSendMessageCount(0, FuncAgrsIncrease);

		return true;
	}

	return false;
}

bool CPCanCom::SendDisplayCanMsgDisConnect()
{
	CString strTemp = _T("");
	TPCANMsg CanMsgSend;
	TPCANStatus stsResult;
	memset(&CanMsgSend, 0, sizeof(&CanMsgSend));
	CanMsgSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CanMsgSend.LEN = 8;

	CanMsgSend.ID = 0x0100;

	CanMsgSend.DATA[0] = 0x84;
	CanMsgSend.DATA[1] = 0x00;
	CanMsgSend.DATA[2] = 0x00;
	CanMsgSend.DATA[3] = 0x00;
	CanMsgSend.DATA[4] = 0x00;
	CanMsgSend.DATA[5] = 0x00;
	CanMsgSend.DATA[6] = 0x00;
	CanMsgSend.DATA[7] = 0x00;

	stsResult = m_pCanDsp.m_objPCANBasic->Write(m_pCanDsp.m_PcanHandle, &CanMsgSend);

	CString strDebug;
	strDebug.Format(_T("SEND CAN MSG [ID:%d] : %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x"),
		CanMsgSend.ID
		, CanMsgSend.DATA[0], CanMsgSend.DATA[1], CanMsgSend.DATA[2], CanMsgSend.DATA[3]
		, CanMsgSend.DATA[4], CanMsgSend.DATA[5], CanMsgSend.DATA[6], CanMsgSend.DATA[7]);
	printf("[PCAN][COMM I/F][SendDisplayCanMsgDisConnect:%s]\n", strDebug);

	if (PCAN_ERROR_OK == stsResult)
	{
		CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(GetChannelNo());
		if (pCommData)
			pCommData->SetSendMessageCount(0, FuncAgrsIncrease);

		return true;
	}

	return false;
}

bool CPCanCom::SendDisplayCanMsgClose()
{
	CString strTemp = _T("");
	TPCANMsg CanMsgSend;
	TPCANStatus stsResult;
	memset(&CanMsgSend, 0, sizeof(&CanMsgSend));
	CanMsgSend.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CanMsgSend.LEN = 8;

	CanMsgSend.ID = 0x0100;

	CanMsgSend.DATA[0] = 0x85;
	CanMsgSend.DATA[1] = 0x00;
	CanMsgSend.DATA[2] = 0x00;
	CanMsgSend.DATA[3] = 0x00;
	CanMsgSend.DATA[4] = 0x00;
	CanMsgSend.DATA[5] = 0x00;
	CanMsgSend.DATA[6] = 0x00;
	CanMsgSend.DATA[7] = 0x00;

	stsResult = m_pCanDsp.m_objPCANBasic->Write(m_pCanDsp.m_PcanHandle, &CanMsgSend);

	CString strDebug;
	strDebug.Format(_T("SEND CAN MSG [ID:%d] : %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x"),
		CanMsgSend.ID
		, CanMsgSend.DATA[0], CanMsgSend.DATA[1], CanMsgSend.DATA[2], CanMsgSend.DATA[3]
		, CanMsgSend.DATA[4], CanMsgSend.DATA[5], CanMsgSend.DATA[6], CanMsgSend.DATA[7]);
	printf("[PCAN][COMM I/F][SendDisplayCanMsgClose:%s]\n", strDebug);

	if (PCAN_ERROR_OK == stsResult)
	{
		CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(GetChannelNo());
		if (pCommData)
			pCommData->SetSendMessageCount(0, FuncAgrsIncrease);

		return true;
	}

	return false;
}

// -------------------------------------------------------------------------------------------------

void CPCanCom::ManualCanReceive(TPCANMsg* pCanMsg)
{
}

void CPCanCom::ManualCanSend(ManualRef m_stManualRef)
{
}