#pragma once

#include "Singleton.h"
#include "Channel.h"

struct SRestartInfo
{
	int	nCyclerChannel;
	int	nRestartStep;
	int nIFBoardNumber;
};

struct SStartSchedule
{
	int nStartCycleNumber;
	int nStartStepNumber;
	int nSyncMode;

	CString strScheduleFilePath;
	CString strScheduleName;
	CString strEQPID;
	CString strUserID;
	CString strWorkName;
	CString strLogFilePath;

	std::vector<CString> vecCellID;
	std::vector<int> vecCyclerChannelNumber;

	BOOL bScheduleName;
	BOOL bTime;
};

class CMgrChannel : public CSingleton< CMgrChannel >
{
public:
	CMgrChannel();
	virtual ~CMgrChannel();

	// Channel STL ฐüทร
	CChannel*	GetAt(int nIndex);
	CChannel*	GetChannel(int nChannelNo);
	CChannel*	GetSelectedChannel();

	int			GetSelectCyclerChannelCount();
	int			GetSize();

	CList< CChannel*, CChannel* >&	GetChannelList();

	BOOL	GetChannelSelected();
	void	SetChannelSelected(CStringArray& arr/*, int nIdx*/);
	void	SetCyclerChannelSelect(int nCyclerChannel);

	void	SendAllChannelCommandFrame(BYTE byCommand);
	void	StartSchedule(SStartSchedule& kParser);
	void	StartErrorChannelSchedule(int chNum, int cycle, int step, int loop);
	void	StartErrorChannelSchedule(int chNum, int cycle, int step, int loop, int steptime, int cycletime, float StepAh, float StepWh, float CycleAh, float CycleWh);
	void	ScheduleAppend(SStartSchedule& kParser);
	void	NextStep();
	void	MoveStep(int nCycleNumber, int nStepNumber);

	//use remote
	void	NextStep(CChannel* pChannel);
	void	MoveStep(CChannel* pChannel, int nCycleNumber, int nStepNumber);

	void	SendInitial(int nBoardNumber);

	void	SetCyclerChannelReservePause(int nCycleNumber, int nStepNumber);
	void	SetCyclerChannelReserveComplete(int nCycleNumber, int nStepNumber);
	bool	GetCyclerChannelExistStepInfo(int nCycleNumber, int nStepNumber);

	bool	IsCyclerChannelRestartEnable();
	bool	IsCyclerChannelRestartEnable(std::vector<SRestartInfo>& vevValue);
	bool	IsCyclerChannelStartEnable();
	bool	IsCyclerChannelStartEnable(int nStageNo);//220526 Added function for selecting by stage
	bool	IsCyclerChannelScheduleAppendEnable();
	//bool	IsCyclerChannelStarted();
	bool	IsCyclerChannelStarted(int nStageNo = 0);			//220526 Added function for selecting by stage
	bool	IsCyclerChannelFinished(int nStageNo = 0);			//220314 YGYUN Schedule Finish Check Function
	//bool	IsCyclerChannelSelected();
	bool	IsCyclerChannelSelected(int nStageNo = 0);	//220526 Added function for selecting by stage
	bool	IsChannelBarcodeCheck(int nStageNo = 0);
	bool    IsChamberUseInStartedChannel(int nChannelNo);
	bool	IsCyclerChannelExistAlarm(int nYesNo);
	bool	IsCyclerChannelExistReserve(int nCycleNumber, int nStepNumber);

	//void	SetCyclerChannelPauseOption(int nCommandType, int nCycleNumber = 0, int nStepNumber = 0);
	void	SetCyclerChannelPauseOption(int nCommandType, int nStageNo = 0, int nCycleNumber = 0, int nStepNumber = 0); //220526 Added function for selecting by stage
	void	GetSelectedCyclerChannel(std::vector<CString>& vecValue);
	bool	ChangeMode(bool bSingle);
	


private:
	CList< CChannel*, CChannel* > m_listChannel;


private:
	void	RemoveAll();
};

