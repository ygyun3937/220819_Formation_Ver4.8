

// -----------------------------------------------------------------------------

#pragma once
#include "include\PLC\ActDefine.h"	

class CPlcProcessSequence1: public CSingleton< CPlcProcessSequence1 >	//[JMW:220315] Csingleton add
{
public:

	enum enumProcessStepAuto
	{
		//초기화
		AUTO_INIT,

		//PLC -> PC READY
		AUTO_PLC_READY,

		//PC -> PLC PROCESS_START	
		AUTO_PC_PROCESS_START,

		//PLC -> PC LOAD_COMPLETE	
		AUTO_PLC_LOAD_COMPLETE,

		AUTO_PC_SCHEDULE_START,
		AUTO_PC_SCHEDULE_START_CHECK,

		//PC -> PLC SCHEDULE_COMPLETE
		AUTO_PC_SCHEDULE_COMPLETE,
		//PLC -> PC UNLOAD_COMPLETE	
		AUTO_PLC_UNLOAD_COMPLETE,

		//PC -> PLC PROCESS_END
		AUTO_PLC_PROCESS_END,
		AUTO_PLC_PROCESS_END_CHECK,

	};
public:
	//CPlcProcessSequence1(CWnd* pParent = nullptr);
	CPlcProcessSequence1();
	virtual ~CPlcProcessSequence1();

public:


public:
	void ThreadRun(void);
	void ThreadStop(void);
	BOOL Create();
	//virtual BOOL Create(CWnd* pParentWnd = NULL);

	// ----------------------------------------------------------------------------

private:
	static UINT threadFunction(LPVOID pParam);
	void threadJob(void);
	void doRunStep();

	void SendPlcBlockData();
	void AutoRun();

	void WaitTime(DWORD dwMillisecond);
private:
	BOOL			m_bThreadLife;
	CWinThread*		m_pThread;


	int m_SendTimeCheck;
	int m_ReadTimeCheck;
	int m_SendBlockTimeCheck;
	long m_PlcAndPcStatus[TYPE_PLC_MAX];	//[JMW:220314]
	BOOL m_bPLCHeartBitAlarmFlag;

public:
	//추가
	enumProcessStepAuto		m_ePressRunStepCurrent;
	void SetAutoStep(enumProcessStepAuto estepPress);

	void Init_Seq();

	void SetSendHeartBit();
	void GetReadHeartBit();

	int m_ReadAlarmCount;

	//BOOL m_RunStopFlag;


	//PLC TEST 
	int m_PlcWaitTime;

	void Set_PLCHeartBitAlarm(BOOL bAlarm) { m_bPLCHeartBitAlarmFlag = bAlarm; }
	BOOL Get_PLCHeartBitAlarm() { return m_bPLCHeartBitAlarmFlag; }


};

