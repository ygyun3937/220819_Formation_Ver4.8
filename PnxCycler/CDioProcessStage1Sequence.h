

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
	 * PROCESS STEP - TYPE A : ��/������, DC/IR.
	 */

	enum enumProcessStepInitType
	{
		//�Ϻ� ���κ� �� �ϰ�
		Init_Step_Probe_Pin_Down,
		//��� ������ ���
		Init_Step_Pusher_Up,
		//Clamp, Centering �Ǹ��� ����		
		Init_Step_Clamp_UnClamp,
		Init_Step_Centering_Backward,
		Init_Step_Job_Model_Check,
		//��
		Init_Step_End,
	};
	enum enumProcessStepAutoType
	{
		//Ʈ���� �� ���� Ȯ��
		AUTO_STAGE_EQ_RUN_TRAY_IN,
		AUTO_STAGE_EQ_RUN_TRAY_IN_CHECK,


		//���͸� �� Ȯ��
		AUTO_STAGE_EQ_RUN_CENTERING_CHECK,

		//���͸� ����
		AUTO_STAGE_EQ_RUN_CENTERING_FORWORD,

		//���͸� Ŭ���� ����
		AUTO_STAGE_EQ_RUN_CENTERING_CLAMP_FORWORD,

		//���
		AUTO_STAGE_EQ_RUN_PUSHER_DOWN,

		//�Ϻ�
		AUTO_STAGE_EQ_RUN_PROBE_UP,
		//////////////////////////////////////

		//������ ���� Ȯ��
		AUTO_STAGE_EQ_SCHEDULE_CHECK,

		//������ ����
		AUTO_STAGE_EQ_SCEDULE_START,

		//������ �Ϸ� Ȯ��
		AUTO_STAGE_EQ_SCEDULE_END_CHECK,
		
		//���� �� ���� ������ ////////////////////////
		//�Ϻ�
		AUTO_STAGE_EQ_RUN_PROBE_DOWN,

		//���
		AUTO_STAGE_EQ_RUN_PUSHER_UP,

		//���͸� Ŭ���� ����
		AUTO_STAGE_EQ_RUN_CENTERING_CLAMP_BACKWORD,

		//���͸� ����
		AUTO_STAGE_EQ_RUN_CENTERING_BACKWORD,

		////Ʈ���� ���� ���� Ȯ��
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


	//�߰�
	enumProcessStepAutoType		m_eStage1PressRunstepPrev;
	enumProcessStepAutoType		m_eStage1PressRunstepCurrent;
	enumProcessStepInitType		m_eInitstepCurrentS1;

	void SetStage1AutoStep(enumProcessStepAutoType estepPress);
	void SetInitS1Step(enumProcessStepInitType estepPress);

	enumProcessStepAutoType GetStage1AutoStep() { return m_eStage1PressRunstepPrev; }

	void Init_Stage1_Seq();

		
	 
};

