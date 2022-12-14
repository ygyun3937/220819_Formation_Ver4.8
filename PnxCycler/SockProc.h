#pragma once

enum TURBY_D_INDEX
{
	SEND_D_HEADER_1 = 0,
	SEND_D_HEADER_2,
	SEND_D_LENGTH_1,
	SEND_D_LENGTH_2,
	SEND_D_CHANNEL_1,
	SEND_D_CHANNEL_2,
	SEND_D_COMMAND_1,
	SEND_D_COMMAND_2,
	SEND_D_STEP_MODE_1,
	SEND_D_STEP_MODE_2,
	SEND_D_OP_MODE_1,
	SEND_D_OP_MODE_2,
	SEND_D_TIME_PERIOD_1,
	SEND_D_TIME_PERIOD_2,
	SEND_D_SEND_TIME_1,
	SEND_D_SEND_TIME_2
};

enum SENDINDEX
{
	SEND_HEADER_1 = 0,
	SEND_HEADER_2,
	SEND_STEP_CNT1,
	SEND_STEP_CNT2,
	SEND_CH_INDEX1,
	SEND_CH_INDEX2,
	SEND_COMMAND1,
	SEND_COMMAND2,
	SEND_MODE_STEP1,
	SEND_MODE_STEP2,
	SEND_MODE_OP1,
	SEND_MODE_OP2,
	SEND_SAMPLING1,
	SEND_SAMPLING2,
	SEND_TIME_PERIOD1,
	SEND_TIME_PERIOD2,
	SEND_STEP_INDEX_1,
	SEND_STEP_INDEX_2,
	SEND_CYCLE_LOOP1,
	SEND_CYCLE_LOOP2,
	SEND_GOTO_INDEX_1,
	SEND_GOTO_INDEX_2,
	SEND_RETURN_STEP_1,
	SEND_RETURN_STEP_2,
	SEND_REF_VOL_1,	// 실제 센싱 값
	SEND_REF_VOL_2,
	SEND_REF_VOL_3,
	SEND_REF_VOL_4,
	//hsm 전력전자 미적용
	SEND_END_VOL_1,	// 종료 조건
	SEND_END_VOL_2,
	SEND_END_VOL_3,
	SEND_END_VOL_4,	
	SEND_END_VOL_INDEX_1,
	SEND_END_VOL_INDEX_2,
	SEND_REF_CUR_1, // 병렬일떄 나누기 병렬갯수
	SEND_REF_CUR_2, // 병렬일떄 나누기 병렬갯수
	SEND_REF_CUR_3,	// 병렬일떄 나누기 병렬갯수
	SEND_REF_CUR_4, // 병렬일떄 나누기 병렬갯수
	SEND_END_TIME_1,
	SEND_END_TIME_2,
	SEND_END_TIME_3,
	SEND_END_TIME_4,
	SEND_PATT_PERIOD1,
	SEND_PATT_PERIOD2,
	SEND_END_TIME_INDEX_1,
	SEND_END_TIME_INDEX_2,
	SEND_END_CUR_1, // 병렬일떄 나누기 병렬갯수
	SEND_END_CUR_2, // 병렬일떄 나누기 병렬갯수
	SEND_END_CUR_3, // 병렬일떄 나누기 병렬갯수
	SEND_END_CUR_4, // 병렬일떄 나누기 병렬갯수
	SEND_END_CUR_INDEX_1,
	SEND_END_CUR_INDEX_2,
	SEND_END_CAP_1, // 병렬일떄 나누기 병렬갯수
	SEND_END_CAP_2, // 병렬일떄 나누기 병렬갯수
	SEND_END_CAP_3, // 병렬일떄 나누기 병렬갯수
	SEND_END_CAP_4, // 병렬일떄 나누기 병렬갯수
	SEND_END_CAP_INDEX_1,
	SEND_END_CAP_INDEX_2,
// 	SEND_END_TAH_1,
// 	SEND_END_TAH_2,
// 	SEND_END_TAH_3,
// 	SEND_END_TAH_4,
// 	SEND_END_TAH_INDEX_1,
// 	SEND_END_TAH_INDEX_2, //taek Tah Total ah 없어서 추가함 현재 봉인 200113
	SEND_END_WAT_1, // 병렬일떄 나누기 병렬갯수
	SEND_END_WAT_2, // 병렬일떄 나누기 병렬갯수
	SEND_END_WAT_3, // 병렬일떄 나누기 병렬갯수
	SEND_END_WAT_4, // 병렬일떄 나누기 병렬갯수
	SEND_END_WAT_INDEX_1,
	SEND_END_WAT_INDEX_2,
// 	SEND_END_WATT_HOUR_1,
// 	SEND_END_WATT_HOUR_2,
// 	SEND_END_WATT_HOUR_3,
// 	SEND_END_WATT_HOUR_4,
// 	SEND_END_WATT_HOUR_INDEX_1,
// 	SEND_END_WATT_HOUR_INDEX_2,
	SEND_CYCLE_END_VOL_1,	//taek sk cycle 종료 조건
	SEND_CYCLE_END_VOL_2,
	SEND_CYCLE_END_VOL_3,
	SEND_CYCLE_END_VOL_4,
	SEND_CYCLE_END_VOL_INDEX_1,
	SEND_CYCLE_END_VOL_INDEX_2,
	SEND_CYCLE_END_TIME_1,
	SEND_CYCLE_END_TIME_2,
	SEND_CYCLE_END_TIME_3,
	SEND_CYCLE_END_TIME_4,
	SEND_CYCLE_END_TIME_INDEX_1,
	SEND_CYCLE_END_TIME_INDEX_2,
	SEND_CYCLE_END_CAP_1,
	SEND_CYCLE_END_CAP_2,
	SEND_CYCLE_END_CAP_3,
	SEND_CYCLE_END_CAP_4,
	SEND_CYCLE_END_CAP_INDEX_1,
	SEND_CYCLE_END_CAP_INDEX_2,
	SEND_CYCLE_END_WAT_1,
	SEND_CYCLE_END_WAT_2,
	SEND_CYCLE_END_WAT_3,
	SEND_CYCLE_END_WAT_4,
	SEND_CYCLE_END_WAT_INDEX_1,
	SEND_CYCLE_END_WAT_INDEX_2,
 	SEND_TEMP_SETTING_1,
 	SEND_TEMP_SETTING_2,
	SEND_DWELL_TIME_1,
	SEND_DWELL_TIME_2,
	SEND_DWELL_TIME_3,
	SEND_DWELL_TIME_4,
	SEND_SYNC_TYPE1,
	SEND_SYNC_TYPE2,
 	SEND_TEMP_START1,
	SEND_TEMP_START2,
	SEND_END_SYNC1,
	SEND_END_SYNC2,
	SEND_CURRENT_LIMIT_1,
	SEND_CURRENT_LIMIT_2,
	SEND_CURRENT_LIMIT_3,
	SEND_CURRENT_LIMIT_4
// 	SEND_STEP_PASUE,
// 	SEND_SYNC_CHANNEL_1,
// 	SEND_SYNC_CHANNEL_2,
// 	SEND_SYNC_CHANNEL_3,
// 	SEND_SYNC_CHANNEL_4,
};
enum RECEIVEINDEX
{
	REC_HEADER_1 = 0,
	REC_HEADER_2,
	REC_DATA_CNT,
	REC_CH_INDEX,
	REC_COMMAND,
	REC_MODE_OP,
	REC_SAMPLING,
	REC_STEP_INDEX_1,
	REC_STEP_INDEX_2,
	REC_END_TYPE,
	REC_VOL_1,
	REC_VOL_2,
	REC_VOL_3,
	REC_CUR_1,
	REC_CUR_2,
	REC_CUR_3,
	REC_TIME_1,
	REC_TIME_2,
	REC_TIME_3,
	REC_TIME_4,
	REC_TIME_5,
	REC_END_VALUE_1,
	REC_END_VALUE_2,
	REC_END_VALUE_3,
};

#include "Singleton.h"
#include "ClientSocket.h"

class CChannel;

class CSockProc : public CWnd, public CSingleton< CSockProc >
{
public:
	CSockProc();
	virtual ~CSockProc();

	void	CreateSocket();
	void	CloseSocket();

	void	SocketConnect();

	void	StopThread();
	bool	SendTimerTypeS();
	bool	SendBoardTypeD(int nBoardNumber, BYTE byChannel, int nReserveStepIndex = 1);
	bool	SendBoardTypeD_Formation(int nBoardNumber, BYTE byChannel, int nReserveStepIndex = 1);
	bool	SendBoardTypePLC_Formation(bool bAlarm);	//PLC Alarm Send 220608YGY
	bool	SendBoardTypeT(int nBoardNumber, BYTE byChannel);
	bool	SendBoardTypeS(int nBoardNumber, BYTE byChannel);
	bool	SendBoardTypeS(int nBoardNumber, BYTE byChannel, int length);
	bool	SendBoardTypeS(int nBoardNumber, BYTE byChannel1, BYTE byChannel2);	
	bool	SendBoardTypeS(int nBoardNumber, BYTE byChannel, int nStepIndex, int nStepTime, int nCycleTime, int nLoopCount, float fStepAh, float fStepWh, float fCycleAh, float fCycleWh);

	bool	SendBoardTypeP(int nBoardNumber, BYTE byChannel, int nStepIndex, int nType);	// 개별 채널 
	bool	SendBoardTypeP(int nBoardNumber, CArray< int, int >& arrChannel, CArray< int, int >& arrStepIndex, int nType);	// 2개 이상 채널
	bool	SendBoardTypeP(int nType); // 모든 채널
	bool	SendBoardTypeJ(int nBoardNumber, BYTE byChannel, int nStepIndex);
	bool	SendBoardTypeC(int nBoardNumber, BYTE byChannel, BYTE byCommand);
	bool	SendBoardTypeC(int nBoardNumber, CArray< int, int >& arr, int nCommand);
	bool	SendBoardTypeM(int nBoardNumber, BYTE byChannel, int nStepIndex, float fRefVolt, float fRefWatt);

	//bZeroOffset : 1 -> 0A Calibration 0 -> 1A Calibration
	bool	SendBoardTypeI(int nBoardnumber, BYTE byChannel, BOOL bZeroOffset);
	bool	SendBoardTypeV(int nBoardnumber, BYTE byChannel);


	void	SendToRemote(char* pBuff, int nSize);		// PCAN

	CClientSocket*	GetAt(int nBoardNumber);
	CClientSocket*	GetAtChannel(int nChannelNumber);
	CClientSocket*	GetAtKey(int nPrimaryKey);
private:
	int m_nStepType;

	CList< CClientSocket*, CClientSocket* > m_listSocket;
private:
	int		CalcChannelNumber(BYTE byChannelNumber, int nBoardIndex);	
};


