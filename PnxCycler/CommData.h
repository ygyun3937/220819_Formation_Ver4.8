#pragma once

enum FuncAgrs
{
	FuncAgrsEqual = 0,
	FuncAgrsPlus,
	FuncAgrsIncrease,
	FuncAgrsMinus,
};

enum RECE_OPCODE {
	CHA_START = 10,
	CHA_CC = 11,
	CHA_CV = 12,
	CHA_CP = 13,
	CHA_END = 14,

	DIS_START = 20,
	DIS_CC = 21,
	DIS_CV = 22,
	DIS_CP = 23,
	DIS_END = 24,
};

enum LOG_TYPE
{
	LOG_TYPE_RUN =0,
	LOG_TYPE_END =10,
	LOG_TYPE_COMPLETE = 20,
	//220621 YGYUN Calibration Log Add
	LOG_TYPE_CALIBRATION_VOLT = 30,
	LOG_TYPE_CALIBRATION_CURRENT = 40,
};

#include <vector>
#include "DAQ.h"

class CCommData
{
public:
	CCommData();
	virtual ~CCommData();

	void	CalcAmpere(int nStepIndex);
	void	ConversionCommData(bool blsUIStepChange, bool bIsCycleChange); //taek sk
	void	InitWH();
	void	InitCalcData();
	void	CopyData(CCommData* pSrc, BOOL bCopyTime = TRUE);
	void	CalcRunningTime(int nStepIndex, bool bIsStepChange, bool bIsPause);
	//void	CalcDCIR();
	void	CalcStepDCIR(int nStepIndex);
	void	WriteLogCount(int nCount = -1);
	void	InitalizeReady();
	void	SetRestVoltage(float fValue);

	void	SetStepIndex(int nIndex) { m_nStepIndex = nIndex; }
	void	SetChannelNumber(int nChannelNumber) { m_nChannel = nChannelNumber; }
	void	SetSOCValue(int nSOC) { m_nSOCValue = nSOC; }
	void	SetSOHValue(int nSOH) { m_nSOHValue = nSOH; }	
	void	SetChamberTemperature(float fChamberTemperature) { m_fChamberTemp = fChamberTemperature; }
	void	SetChamberTemperatureSP(float fChamberTemperature) { m_fChamberTempSp = fChamberTemperature; }
	void	SetPower(float fPower) { m_fPower = fPower; }
	void	SetBeforStepTimeV(int nStepTime) { m_nBeforStepTimeV = nStepTime; }
	void	SetBeforStepTimeI(int nStepTime) { m_nBeforStepTimeI = nStepTime; }
	void	SetCommCommand(int nCommand) { m_nCommCmd = nCommand; }
	void	SetOPCode(int nCode) { m_nOpcode = nCode; }
	void	SetDataFrameCount(int nFlameCnt) { m_nDataFrameCount = nFlameCnt; }
	void	SetEndReason(int nEndReason) { m_nEndReason = nEndReason; }
	void	SetSystemSafetyEndReason(int n_SystemSafetyEndReason) { m_nSystemSafetyEndReason = n_SystemSafetyEndReason; }
	void	SetInverterEndReason(int n_InverterEndReason) { m_nInverterEndReason = n_InverterEndReason; }
	void	PreSetVoltage(float fPreVoltage) { m_fPreVolt = fPreVoltage; }
	void	SetVoltage(float fVoltage) { m_fVolt = fVoltage; }
	void	SetPreAmpere(float fPreAmpere) { m_fPreCurr = fPreAmpere; }
	void	SetAmpere(float fAmpere) { m_fCurr = fAmpere; }
	void	SetPreTemp(float fTemp) { m_fPreTemp = fTemp;}
	void	SetTemp(float fTemp) { m_fTemp = fTemp;}
	//220516 Pogo Pin Ohm Add
	void SetPogoOhmMonitoring(float fPogoPinOhmMonitoring);
	void SetPogoOhmSetting(float fPogoPinOhmSetting) { m_fPogoPinOhm_Setting = fPogoPinOhmSetting; }



	void	SetStepAmpereHour(double fAmpereHour, int nFuncAgrs = FuncAgrsEqual);
	void    SetStepAmpereHourTotal(double fAmpereHourTotal);
	void	SetCycleNumber(int nNumber) { m_nCycleNo = nNumber; }
	void	SetStepNumber(int nNumber) { m_nStepNo = nNumber; }
	void	SetProcessStatuInt(int nVluae) { m_nProcessStatus = nVluae; }
	void	SetLogType(int nType) { m_nLogType = nType; }
	void	SetLogChannelNumber(int nChannelNumber) { m_nLogChannelNumber = nChannelNumber; }
	void	SetTotalCycleLoop(int nTotalCount) { m_nTotalCycleLoop = nTotalCount; }
	void	SetTotalCount(int nTotalCount) { m_nTotalCount = nTotalCount; }
	void	SetDVBefor(float fDV1) { m_fDeltaVBefor = fDV1; }
	void	SetDVAfter(float fDV2) { m_fDeltaVAfter = fDV2; }
	void	SetDIBefor(float fDI1) { m_fDeltaIBefor = fDI1; }
	void	SetDIAfter(float fDI2) { m_fDeltaIAfter = fDI2; }	
	void	SetScheduleComplete(bool bFlag) { m_bScheduleComplete = bFlag; }
	void	SetUserID(CString str) { m_strUserID = str; }
	void	SetCellID(CString str) { m_strCellID = str; }
	void	SetWorkName(CString str) { m_strWorkName = str; }
	void	SetChBarcode(CString str) { m_strChBarCode = str; }
	void	SetCycleIndex(int nCycleIndex) { m_nCycleIndex = nCycleIndex; }
	int	GetcycleIndex() { return	m_nCycleIndex; }
	
	void	RefreshMsgCount() { 
		m_nStepTime = 0; 
	}

	void	SetLogTime(CString csData) { m_strTime = csData; }

	void	SetRatioType(int nRatioType) { m_nRatioType = nRatioType; }

	int		CalcLoopCount(int nEndReason, int nStepIndex, int nRecvLoopCount);//FW에서 주는 LoopCount SW입장에 맞게 계산(개선 후 필요 없음)
	int		CalcUIStepNumber(int nStepIndex);
	int		CalcUICycleNumber(int nStepIndex);
	int		CalcEndReason(int nEndReason, int nStepIndex);
	int		GetPatternTotalTime() { return m_nPatternTime; }
	int		GetStepTime() { return m_nStepTime; }
	int		GetBeforStepTimeV() { return m_nBeforStepTimeV; }
	int		GetBeforStepTimeI() { return m_nBeforStepTimeI; }
	int		GetSchTotalTime() { return m_nTotalTime; }
	int		GetCommCommand() { return m_nCommCmd; }								//ParseBoardDataFrame_FormationI/F Command 상태 가져옴	0[ Default ], 81 [ Rest ], 82[ Charge ], 83[ Dischage ], 84[ Alarm Clear ], 85[ Input Relay Off ], 86[ Input Relay On ], 87[ Output Relay Off ], 88[ Output Relay On ], 89[ All Relay Off ]
	int		GetOPCode() { return m_nOpcode; }									// ParseBoardDataFrame_Formation /Operation Mode : 0[None], 1[Input On], 2[Output On], 3[Charge CC], 4[Charge CV], 5[Discharge CC], 6[Discharge CV], 7[Charge CC_CP], 8[Dischage CC_CP], 9[Chage CV_CP], 10[Dischage CV_CP], 11[Chage CR], 12[Dischage CR]
	int		GetEndReason() { return m_nEndReason; }								// ParseBoardDataFrame_Formation /1[Time], 2[Ampere], 3[Capacity], 4[Watt], 5[Voltage]
	int		GetSystemSafetyEndReason() { return m_nSystemSafetyEndReason; }
	int		GetInverterEndReason() { return m_nInverterEndReason; }
	int		GetCycleNumber() { return m_nCycleNo; }
	int		GetStepNumber() { return m_nStepNo; }	
	int		GetProcessStatusInt() { return m_nProcessStatus; }
	int		GetLogChannelNumber() { return m_nLogChannelNumber; }
	int		GetLogType() { return m_nLogType; }	
	int		GetCycleIndex() { return m_nCycleIndex; }
	int		GetTotalCycleLoop() { return m_nTotalCycleLoop; }	
	int		GetTotalCount() { return m_nTotalCount; }	
	int		GetWritelogCount() { return m_nWriteLogCount; }
	int		GetStepIndex() { return m_nStepIndex; }

	float	IsReverseVoltage();
	float	GetDVBefor() { return m_fDeltaVBefor; }
	float	GetDVAfter() { return m_fDeltaVAfter; }
	float	GetDIBefor() { return m_fDeltaIBefor; }
	float	GetDIAfter() { return m_fDeltaIAfter; }
	float	GetPreVoltage() { return m_fPreVolt; }
	float	GetVoltage() { return m_fVolt; }
	float	GetPreAmpere() { return m_fPreCurr; }
	float	GetAmpere() { return m_fCurr; }
	float	GetPreTemp() { return m_fPreTemp; }
	float	GetTemp() { return m_fTemp; }
	float	GetCapacity() { return (float)abs(m_fStepAh); }
	float	GetChamberTemperature() { return m_fChamberTemp; }
	float	GetChamberTemperatureSP() { return m_fChamberTempSp; }

	double	GetStepAmpereHour();
	double  GetStepAmpereHourTotal();
	double	GetStepWattHour() { return m_fStepWh; }
	double	GetPatternAmpereHour() { return m_fPatternAh; }
	double	GetPatternWattHour() { return m_fPatternWh; }
	double	GetPatternTotalAmpereHour() { return m_fPatternTotalAh; }
	double	GetScheduleAmpereHour() { return m_fSchAh; }
	double	GetPower() { return m_fPower; }	

	//220516 Pogo Pin Ohm Add
	double GetPogoOhmMonitoring() { return m_fPogoPinOhm_Monitoring; }
	double GetPogoOhmSetting() { return m_fPogoPinOhm_Setting; }
	CString GetPogoOhmSettingstr();
	CString	GetChBarcode();

	bool	IsUsingCyclerCurrent();
	bool	CheckEQStepChange(int nStepIndexIFBoard, int nLoopCount);
	bool	CheckEQCycleChange(int nStepIndexIFBoard);
	bool	CheckEQScheduleComplete(int nStepIndexIFBoard, int nEndReason, int nLoopCount, bool bLastCycle);
	bool	CheckEQPause(int nEndReason);
	bool	IsStatusIdle();
	bool	IsStatusReady();
	bool	IsStatusStart();
	bool	IsStatusPause();
	bool	IsStatusPauseTemp();
	bool	IsStatusPauseDelay();
	bool	IsStatusPauseStep();
	bool	IsStatusPauseSungOn();
	bool	IsStatusComplete();
	bool	IsStatusAlaram();
	bool	IsUpdateDaQInfo();
	bool	IsContactRegUpdate();
	void	SetContactRegUptate(bool bUpdate);

	bool	m_bContactRegUpdate;

	bool	CheckPatternLoopStepChange(int nStepIndexIFBoard, int nChamberIndex);


	CString GetPogoOhmMonitoringstr();

	CString	GetEndReasonStr();
	CString	GetCyclerChannelInfo(bool bStepEnd, bool nTotalCarType);
	CString	GetStepTypeStr();
	CString	GetStepTypeFromDaQ();

	double	GetScheduleWattHour() { return m_fSchWh; }
	double	GetChargeAmpereHour() { return m_fChaAh; }
	double	GetChargeWattHour() { return m_fChaWh; }
	double	GetDischargeAmpereHour() { return m_fDisAh; }
	double	GetDischargeWattHour() { return m_fDisWh; }
	int		GetChannelNumber();	// 확인

	// DAQ
	void	AddDAQ(CDAQ* pDAQ);
	void	RemoveAllDAQ();
	CList< CDAQ*, CDAQ* >& GetDAQList();

	//pnx_hr DCIR 
	float   m_fStepEndVoltage;
	void	SetStepEndVoltage(float fVoltage) { m_fStepEndVoltage = fVoltage; }
	float	GetStepEndVoltage() { return m_fStepEndVoltage; }
	float   m_fStepTotalAmpere;

	bool _bFirst;
	bool m_bSKipStepTimeInit;
	bool m_bSKipCycleTimeInit;

private:
	enum eDCIRType
	{
		eDCIRTypeSection1 = 0,
		eDCIRTypeSection2,
		eDCIRTypeSection3,
		eDCIRStepEnd1,
		eDCIRStepEnd2,
		eDCIRStepEnd3
	};

	int	m_nType;
	int	m_nCycleNo;			//sw에서 표시되는 xml base 사이클 번호
	int	m_nStepNo;			//sw에서 표시되는 xml base의 스탭 번호
	int	m_nStepIndex;		//fw서 주는 스탭 인덱스 0base
	int	m_nCycleIndex;		//변수이름..현재 사이클의 루프 카운트 이다..
	int	m_nTotalCycleLoop;
	int	m_nTotalCount;
	int	m_nPatternTime;
	int	m_nSOCValue;
	int	m_nSOHValue;
	int	m_nLogType;
	int	m_nLogChannelNumber;
	int	m_nChannel;
	int	m_nOpcode;
	int	m_nCommCmd;
	int	m_nDataFrameCount;
	int	m_nEndReason;
	int	m_nSystemSafetyEndReason;
	int m_nInverterEndReason;
	int	m_nTimeData;
	int	m_nTimeData2;
	int	m_nProcessStatus;

	int m_nRatioType;

	unsigned long	m_nTotalTime;
	unsigned long	m_nStepTime;
	unsigned long	m_nCycleTime;
	unsigned long	m_nBeforStepTimeV;
	unsigned long	m_nBeforStepTimeI;
	unsigned long	m_nWriteLogCount;

	float	m_fChamberTemp;
	float	m_fChamberTempSp;
	float	m_fRestVoltage;
	//float	m_fDcir;			//REST 일때는 0이어야 한다 f
	float	m_fDcir1;
	float	m_fDcir2;
	float	m_fDcir3;
	
	float	m_fLastDcir1;
	float	m_fLastDcir2;
	float	m_fLastDcir3;

	float   m_fPreVolt;
	float	m_fVolt;
	float   m_fPreCurr;
	float	m_fCurr;
	float	m_fPreTemp;
	float	m_fTemp;
	float	m_fDeltaVBefor;
	float	m_fDeltaVAfter;
	float	m_fDeltaIBefor;
	float	m_fDeltaIAfter;

	//220516 Pogo Pin Ohm Add
	float	m_fPogoPinOhm_Monitoring;
	float	m_fPogoPinOhm_Setting;


	double	m_fPower;
	double	m_fPatternWh;
	double	m_fPatternAh;
	double	m_fPatternTotalAh;
	double	m_fCycleAh;
	double	m_fStepAh;
	double  m_fStepAhTotal;
	double	m_fDisStepAh;
	double	m_fCharStepAh;
	double	m_fSchAh;
	double	m_fChaAh;
	double	m_fDisAh;
	double	m_fStepWh;
	double	m_fSchWh;
	double	m_fChaWh;
	double	m_fDisWh;

	bool	m_bScheduleComplete;

	CString	m_strTime;
	CString	m_strUserID;
	CString	m_strCellID;
	CString	m_strWorkName;

	CString m_strChBarCode;


	CList< CDAQ*, CDAQ* > m_listDAQ;
private:
	void	SetDcir1(float fvalue);
	void	SetDcir2(float fvalue);
	void	SetDcir3(float fvalue);	
	void	SetTimeData(int nValue);
	void	IncreaseMsgCount();
	void	SetPatternTotalTime(int nTime, int nFuncAgrs = FuncAgrsEqual);
	void	SetPatternWattHour(double fWattHour, int nFuncAgrs = FuncAgrsEqual); //요거를 넣는다 taek
	void	SetPatternAmpereHour(double fAmpereHour, int nFuncAgrs = FuncAgrsEqual); //요거를 넣는다 taek
	void	SetPatternTotalAmpereHour(double fAmpereHour, int nFuncAgrs = FuncAgrsEqual); //taek
	void	SetCycleAmperHour(double fAmpereHour, int nFuncAgrs = FuncAgrsEqual); //taek
	void	SetStepWattHour(double fWattHour, int nFuncAgrs = FuncAgrsEqual);
	void	SetScheduleAmpereHour(double fAmpereHour, int nFuncAgrs = FuncAgrsEqual);
	void	SetScheduleWattHour(double fWattHour, int nFuncAgrs = FuncAgrsEqual);
	void	SetChargeAmpereHour(double fAmpereHour, int nFuncAgrs = FuncAgrsEqual);
	void	SetChargeWattHour(double fWattHour, int nFuncAgrs = FuncAgrsEqual);
	void	SetDischargeAmpereHour(double fAmpereHour, int nFuncAgrs = FuncAgrsEqual);
	void	SetDischargeWattHour(double fWattHour, int nFuncAgrs = FuncAgrsEqual);	
	void	SetStepDisAmperHour(double fAmpereHour) { m_fDisStepAh = fAmpereHour; }
	void	SetStepCharAmperHour(double fAmpereHour) { m_fCharStepAh = fAmpereHour; }	

public:
	void	SetCycleTime(int nCycleTime) { m_nCycleTime = nCycleTime; }
	void	SetStepTime(int nStepTime) { m_nStepTime = nStepTime; }
	void	SetTotalTime(int nTotalTime) { m_nTotalTime = nTotalTime; }

	bool	GetScheduleComplete() { return m_bScheduleComplete; }

private:
	void	RefreshMsgTotalCount() { 
		m_nTotalTime = 0; 
	}	
	
	int		GetStepType();	// 확인		
	int		GetTimeData() { return m_nTimeData; }

	float	GetRestVoltage() { return m_fRestVoltage; }
//	float	GetDcir() { return m_fDcir; }
	
	
	CString GetChannelBarcodeStr();
	CString	GetChannelNumberStr();	
	CString GetChamberTemperatureStr();
	CString	GetPowerStr();
	CString	GetStepAmpereHourStr();
	CString	GetStepWattHourstr();
	CString GetDCIR(int nType);
	CString	GetScheduleAmpereHourStr();
	CString	GetScheduleWattHourStr();
	CString	GetChargeAmpereHourStr();
	CString	GetChargeWattHourStr();
	CString	GetDischargeAmpereHourStr();
	CString	GetDischargeWattHourStr();
	CString	GetStepTimeStr();
	CString	GetSchTotalTimeStr();
	CString GetTempStr();	//220405 YGYUN Temp Log Data 추가
	CString	GetVoltageStr();
	CString	GetAmpereStr();
	CString	GetCycleNumberStr();
	CString	GetStepNumberStr();
	CString	GetLogTimeStr() { return m_strTime.GetString(); }
	CString GetStepAmpereHourTotalStr();

// -------------------------------------------------------------------------------------------------
// PCAN

public:
	void	SetSendMessageCount(int nCount, int nFuncAgrs = FuncAgrsEqual);
	int		GetSendMessageCount();

	void	SetRecvMessageCount(int nCount, int nFuncAgrs = FuncAgrsEqual);
	int		GetRecvMessageCount();
	
private:
	int		m_nSendMessageCount;
	int		m_nRecvMessageCount;
};


