

// -----------------------------------------------------------------------------

#pragma once

class CDioProcessStage1Sequence:public CSingleton< CDioProcessStage1Sequence >
{
public:
	enum enumProcessStepType
	{
		PROCESS_STEP_INIT
		, PROCESS_STEP_AUTO_A
	};



	/**
	 * PROCESS STEP - TYPE A : 충/방전기, DC/IR.
	 */

	enum enumProcessStepInitType
	{
		//하부 프로브 핀 하강
		Init_Step_Probe_Pin_Down,
		//상부 프래스 상승
		Init_Step_Pusher_Up,
		//Clamp, Centering 실린더 후진		
		Init_Step_Clamp_UnClamp,
		Init_Step_Centering_Backward,
		Init_Step_Job_Model_Check,
		//끝
		Init_Step_End,
	};
	enum enumProcessStepAutoType
	{
		//트레이 인 감지 확인
		AUTO_STAGE_EQ_RUN_TRAY_IN,
		AUTO_STAGE_EQ_RUN_TRAY_IN_CHECK,


		//센터링 전 확인
		AUTO_STAGE_EQ_RUN_CENTERING_CHECK,

		//센터링 전진
		AUTO_STAGE_EQ_RUN_CENTERING_FORWORD,

		//센터링 클램프 전진
		AUTO_STAGE_EQ_RUN_CENTERING_CLAMP_FORWORD,

		//상부
		AUTO_STAGE_EQ_RUN_PUSHER_DOWN,

		//하부
		AUTO_STAGE_EQ_RUN_PROBE_UP,
		//////////////////////////////////////

		//스케줄 선택 확인
		AUTO_STAGE_EQ_SCHEDULE_CHECK,

		//스케줄 시작
		AUTO_STAGE_EQ_SCEDULE_START,

		//스케줄 완료 확인
		AUTO_STAGE_EQ_SCEDULE_END_CHECK,
		
		//측정 후 동작 시퀀스 ////////////////////////
		//하부
		AUTO_STAGE_EQ_RUN_PROBE_DOWN,

		//상부
		AUTO_STAGE_EQ_RUN_PUSHER_UP,

		//센터링 클램프 후진
		AUTO_STAGE_EQ_RUN_CENTERING_CLAMP_BACKWORD,

		//센터링 후진
		AUTO_STAGE_EQ_RUN_CENTERING_BACKWORD,

		////트레이 배출 감지 확인
		//AUTO_STAGE_EQ_RUN_TRAY_OUT,
		//AUTO_STAGE_EQ_RUN_TRAY_OUT_CHECK,
		///////////////////////////////////////////////

		//Run Step Check
		AUTO_STAGE_RUN_STEP_END,


		//Error Check
		AUTO_STAGE_ERROR_Check,
	};
public:
	CDioProcessStage1Sequence();
	~CDioProcessStage1Sequence();

public:


public:
	void StartThread();
	void StopThread();

	// ----------------------------------------------------------------------------
	
public:

	//int GetCurrentProcessStepAutoTypeAS1() { return m_eCurrentProcessStepAutoTypeAS1; }
	//int GetCurrentProcessStepAutoTypeAS1() { return m_eCurrentProcessStepAutoTypeAS2; }

private:
	static UINT threadFunction(LPVOID pParam);
	void threadJob(void);
	void doRunStep();

	void Run_Stage1();


	void Auto_Stage1();
	void Init_Stag1();

	void WaitTime(DWORD dwMillisecond);
private:
	BOOL			m_bThreadLife;
	CWinThread*		m_pThread;

	
	int m_nManualProcessEqpNo;
	int m_nManualProcessEqpStageNo;
	   

public:
	

	int nTimeOutCnt;


	//추가
	enumProcessStepAutoType		m_eStage1PressRunstepPrev;
	enumProcessStepAutoType		m_eStage1PressRunstepCurrent;
	enumProcessStepInitType		m_eInitstepCurrentS1;

	void SetStage1AutoStep(enumProcessStepAutoType estepPress);
	void SetInitS1Step(enumProcessStepInitType estepPress);

	enumProcessStepAutoType GetStage1AutoStep() { return m_eStage1PressRunstepPrev; }

	void Init_Stage1_Seq();

		
	 
};

