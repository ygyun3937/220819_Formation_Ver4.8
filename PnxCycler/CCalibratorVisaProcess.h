#pragma once

//#include <map>

#include "Singleton.h"

#include "./include/visa.h"
#include "./include/visatype.h"

#include "CCalibratorChannelData.h"

class CCalibratorVisaProcess : public CSingleton< CCalibratorVisaProcess >
{
public:
	CCalibratorVisaProcess();
	virtual ~CCalibratorVisaProcess();

public:
	enum enumProcessAutoCalibrationVolt
	{
		ProcessStep_AutoCalibrationVolt_Init,
		ProcessStep_AutoCalibrationVolt_Measure_Start,
		ProcessStep_AutoCalibrationVolt_Measure_Average_Write,
		ProcessStep_AutoCalibrationVolt_Measure_Next_Volt_Setting,
		ProcessStep_AutoCalibrationVolt_Measure_End,
		ProcessStep_AutoCalibrationVolt_Measure_Error,
	};
	void	SetCalibratorIP(CString ipAddress) { m_strCalibratorIP = ipAddress; }
	void	SetCalibratorPort(int nPortNo) { m_nCalibratorPort = nPortNo; }

	void	SetMeasureModeNo(int nMode) { m_nMeasureMode = nMode; }
	int		GetMeasureModeNo() { return m_nMeasureMode; }

	void	SetMeasureCount(int nMeasureCount) { m_nMeasureCount = nMeasureCount; }
	int		GetMeasureCount() { return m_nMeasureCount; }

	// -------------------------------------------------------------------------
	
	void	SetCalibratorChannelDataVoltage(int nChannelIndexNo, float value);
	float	GetCalibratorChannelDataVoltage(int nChannelIndexNo);
	
	void	SetCalibratorChannelDataCurrent(int nChannelIndexNo, float value);
	float	GetCalibratorChannelDataCurrent(int nChannelIndexNo);

	float	GetCalibratorChannelVoltageAvg(int nChannelIndexNo, int nDataStartIndex, int nDataEndIndex);
	float	GetCalibratorChannelCurrentAvg(int nChannelIndexNo, int nDataStartIndex, int nDataEndIndex);

	void	Set_MeasureRun(bool bMeasureRun) { m_bIsMeasureRun = bMeasureRun; }
	BOOL	Get_MeasureRun() { return m_bIsMeasureRun; }

	void	Set_DelayTime(int nDelayTime) { m_nDelayTime = nDelayTime; }
	int		Get_DelayTime() { return m_nDelayTime; }


	bool	StartThread();
	bool	StopThread();

private:
	bool	Initialize();
	bool	InitializeVariables();
	bool	Dispose();

	int		OpenDevice();
	int		CloseDevice();


	static	UINT MeasureDeviceThread(LPVOID pParam);

	void	ParseRawData(CString strRawData);
	int		DotPositon(CString nScale);

	// -------------------------------------------------------------------------

	void	InitiateDeviceStartup();
	void	InitiateDeviceVoltage();
	void	InitiateDeviceCurrent();

	CString	MakeDeviceUseChannelIdGroup();
	CString	MakeDeviceChannelId(int nChannelIndexNo);

	void	ClearChannelVoltageValueArray(int nChannelIndexNo);
	void	ClearChannelCurrentValueArray(int nChannelIndexNo);
	

	void	MeasureDevice();
	BOOL	MeasureDevice_Voltage();

	enumProcessAutoCalibrationVolt		m_eProcessAutoCalibrationVoltCurrentStep;
	void SetProcessAutoCalibrationVoltAutoStep(enumProcessAutoCalibrationVolt estepCurrent);

	void Init_ProcessAutoCalibrationVolt();

	void  Auto_Calibration_Volt();
private:
	CString			m_strCalibratorIP;
	int				m_nCalibratorPort;
	int				m_nUseChannelCount;

	int				m_nVoltageStartChannelNo;
	int				m_nVoltageEndChannelNo;
	int				m_nCurrentStartChannelNo;
	int				m_nCurrentEndChannelNo;

	CString			m_strTotalDeviceChannelId;
	CString			m_strEachDeviceChannelId;

	CWinThread*		m_pMeasureThread;
	bool			m_bLoopMeasure;

	bool			m_bIsConnected;
	bool			m_bIsInitialized;
	bool			m_bIsMeasureRun;

	int				m_nDelayTime;

	bool			m_bIsMeasureModeEach;			// TRUE : ?????? ???? ???? / FALSE : ?????? ???? ????????
	int				m_nMeasureCurChIndexNo;

	CStringArray	m_sResultDataArray;		// TEST


	CStringArray	m_sResultDataVoltage;
	CStringArray	m_sResultDataCurrent;

	// -------------------------------------------------------------------------

	ViSession m_resourceManager;
	ViSession m_session;

	// -------------------------------------------------------------------------

	CMap<int, int, CCalibratorChannelData*, CCalibratorChannelData*> m_CalibratorChannelData;

	int		m_nMeasureMode;
	int		m_nMeasureCount = 0;

	float	m_fShuntValue = 0.0f;
	CStringArray m_sShuntValueArray;


	CList<CCalibratorVisaProcess*, CCalibratorVisaProcess*> m_listCom;

	float m_fSettingVoltValue = 1.0f;
};

