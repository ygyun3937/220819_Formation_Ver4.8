#pragma once

#include "PCANBasicClass.h"
#include "PCANUser.h"
#include "./include/PCAN/ProtocolCan.h"

#include "Channel.h"

#define SEND_CAN_ID_CV_MODE						0x101 //0x101		//CV MODE ID
#define SEND_CAN_ID_CV_MODE_CH2					0x201 //0x101		//CV MODE ID
#define SEND_CAN_ID_CH_1_SELECT					0xAA //0x101		//PACK SENSING ID
#define SEND_SENSOR_OFF							0xFF //0x101		//PACK SENSING ID

struct ManualRef
{
	int nMode;
	float fVolt;
	float fCurr;
	int nCommand;
};

enum {
	DSP_SEND_CMD_INITIATE = 0,
	DSP_SEND_CMD_INPUT_ON,
	DSP_SEND_CMD_OUTPUT_ON,
	DSP_SEND_CMD_OUTPUT_OFF,
	DSP_SEND_CMD_ALL_OFF,
	DSP_SEND_CMD_STOP_OUTPUT_OFF,
	DSP_SEND_CMD_STOP,
	DSP_SEND_CMD_ERROR_CLEAR,
	DSP_SEND_CMD_MAX_COUNT
};

enum enDspCommand {
	DSP_CMD_00 = 0,
	DSP_CMD_80,
	DSP_CMD_81,
	DSP_CMD_82,
	DSP_CMD_83,
	DSP_CMD_84,
	DSP_CMD_85,
	DSP_CMD_86,
	DSP_CMD_87,
	DSP_CMD_88,
	DSP_CMD_89,
	DSP_CMD_MAX_COUNT		// NO_OF_DSP_CMD
};

enum eDSP_COMMAND_VALUE {
	DSP_CMD_CHA = 0x82,
	DSP_CMD_DIS = 0x83,
};

enum eDSP_RECE_OPCODE {
	DSP_OP_NONE = 0,		//대기상태
	DSP_OP_INPUT_ON = 1,	//입력단 준비 완료
	DSP_OP_OUPUT_ON = 2,	//충방 시험 준비 완료
	DSP_OP_CHA_CC = 3,		//충전 CC 진행 중
	DSP_OP_CHA_CV = 4,		//충전 CV 진행 중
	DSP_OP_DIS_CC = 5,		//방전 CC 진행 중
	DSP_OP_DIS_CV = 6,		//방전 CV 진행 중
	DSP_OP_CHA_CP = 7,		//충전 CP 진행 중
	DSP_OP_DIS_CP = 8,		//방전 CP 진행 중
};

enum eType {
	Rest = 0,
	Charge = 1,
	Discharge = 2,
	Pattern = 3,
	Balance = 4,
	NO_TYPE_END,
};
enum ePComStartStep
{
	StartStep_ErrorClear,
	StartStep_ErrorClear_Check,
	StartStep_Disconnect,
	StartStep_Disconnect_Check,
	StartStep_Close_Check,
	StartStep_Init_Check,
	StartStep_Input_On_Check,
	StartStep_Start_Connecting,
	StartStep_Start_Connect_Complete,
};
enum ePComCloseStep
{
	CloseStep_DisConnect,
	CloseStep_ChargeDisCharge_DisConnect_Check,
	CloseStep_Program_Close_Check,
};

#define LIMIT_PACK_CHA_CV_VOLT 89.0
#define LIMIT_PACK_DIS_CV_VOLT 4.0

class CPCanCom
{
public:
	CPCanCom();
	~CPCanCom();

private:
	void	Initialize();
	void	InitializeVariables();
	void	Dispose();

public:
	void	StopAllThread();

	void	StartThread();
	void	StopThread();

	bool	OpenCan();
	bool	CloseCan();

	// ----------------------------------------------------------------------------

	bool	SendDisplayCanMessage();

	bool	SendDspCanMsgInitiate();
	bool	SendDisplayCanMsgConnectionInput();
	bool	SendDisplayCanMsgVoltCalcStart();
	bool	SendDisplayCanMsgConectionCmdStart();

	bool	SendDisplayCanMegErrorClear();


	bool	SendDisplayCanMsgRest();
	bool	SendDisplayCanMsgDisConnect();
	bool	SendDisplayCanMsgClose();



	// ----------------------------------------------------------------------------

	void	ManualCanReceive(TPCANMsg* pCanMsg);
	void	ManualCanSend(ManualRef m_stManualRef);

public:
	BOOL	IsFirstRecvOK() { return m_bFirstRecvOK; }
	BOOL	IsOpen() { return m_bCanOpenOk; }

	void	SetChannelNo(int nCh) { m_nChannelNo = nCh; }
	int		GetChannelNo() { return m_nChannelNo; }

	void	SetBaurdrate(int nBaurd) { m_nBaurdrate = nBaurd; }
	int		GetBaurdrate() { return m_nBaurdrate; }

	void	SetPrimaryKey(int nPrimaryKey) { m_nPrimaryKey = nPrimaryKey; }
	int		GetPrimaryKey() { return m_nPrimaryKey; }
		 
	void	SetInitBaudrate(int nBaudrate) { m_nInitBaudrate = nBaudrate; }
	void	SetInitUSBChannel(int nChannel) { m_nInitUSBChannel = nChannel; }		// 1Base

	CStringArray& GetRecvCanIdArray() { return m_strRecvCanIdArray; }
	void	SetResvCanIdArray(CStringArray& nValues) { m_strRecvCanIdArray.Copy(nValues); }

	void	SetCheckID(int nID) { m_nCheckID = nID; }

	void	SetSetCommand(int nCmd) { m_nSetCommand = nCmd; }
	int		GetSetCommand() { return m_nSetCommand; }

	void	SetManualCount(int nCount) { m_nManualCount = nCount; }
	void	SetManualTest(bool bManual) { m_bManual = bManual; }

	void	SetSendCanCommand(int nValue) { m_nSendCanCommand = nValue; }
	int		GetSendCanCommand() { return m_nSendCanCommand; }

	void	SetInverInit(bool bInit) { m_bInverInit = bInit; }
	int		GetInverInit() { return m_bInverInit; }



	void	SetProgramClose(bool bClose) { m_bProgramClose = bClose; }
	bool		GetProgramClose() { return m_bProgramClose; }
	void	SetCloseMode(bool bClose) { m_bCloseMode = bClose; }
	bool	GetCloseMode() { return  m_bCloseMode; }

	void Function_PcanStart(TPCANMsg* pCanMsg);

	int m_nStartStep;

	void SetStartStep(int nStartStep) { m_nStartStep = nStartStep; }
	int GetStartStep() { return m_nStartStep; }
	
	void Function_PcanClose(TPCANMsg* pCanMsg);

	int m_nCloseStep;

	void SetCloseStep(int nCloseStep) {m_nCloseStep = nCloseStep;}
	int GetCloseStep() { return m_nCloseStep; }

	void	SetAlarmStatus(bool bAlarm) { m_bAlarm = bAlarm; }
	bool		GetAlarmStatus() { return m_bAlarm; }

	void	SetInverterStatus(int nStatus) { m_nInverterStatus = nStatus; }
	int		GetInverterStatus() { return m_nInverterStatus; }

	void Function_AlarmClear();


private:
	// Thread
	static	UINT ReadThread(LPVOID pParam);
	void	DispatchReceiveData();
	void	ReceiveCanDataProcess(TPCANMsg* pCanMsg);

private:
	bool			m_bInverInit;
	bool			m_bProgramClose;


	CWinThread*		m_pThread;
	bool			m_bLoop;

	ManualRef		m_stManualRef;
	
	int				m_nPrimaryKey;
	int				m_nChannelNo;
	int				m_nBaurdrate;

	int				m_nCanSystemMode;

	BOOL			m_bIsSetConnetUI;
	int				m_nLiveBitSend;
	int				m_nLiveBitRecv;

	BOOL			m_bManual;
	int				m_nManualCount;
	
	BOOL			m_bCloseMode;
	
	int				m_nInitUSBChannel;
	int				m_nInitBaudrate;

	bool			m_bFirstRecvOK;

	CStringArray	m_strRecvCanIdArray;

	int				m_nCheckID = 0;

	int				m_nSendCanCommand;
	int				m_nSetCommand;

	int	m_nInverterStatus;

	// -------------------------------------------------------------------------

	BOOL			m_bCanOpenOk;
	PCANUser		m_pCanDsp;

	TPCANMsg		m_CanMsgSend;
	TPCANMsg		m_CanMsgCVSend;
	TPCANStatus		m_stsResult;

	TPCANMsg		m_CANMsg[11];		// 00(0),80(1),81(2),82(3),83(4),84(5),85(6),86(7),87(8),88(9),89(10)
	TPCANMsg		m_CANMsgPara[11];	// 00(0),80(1),81(2),82(3),83(4),84(5),85(6),86(7),87(8),88(9),89(10)

	bool			m_bAlarm;

	int				m_nTimeCount;

};					

