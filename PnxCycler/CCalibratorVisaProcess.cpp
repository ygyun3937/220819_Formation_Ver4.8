#include "stdafx.h"
#include "CCalibratorVisaProcess.h"

#include "PnxUtil.h"
#include "Math.h"
#include "Channel.h"
#include "MgrChannel.h"
#include "CMgrPid.h"
#include "MgrLog.h"

#include "MgrConfig.h"

#define	DEVICE_SLOT_CHANNEL_MAX_COUNT	16
#define	DEVICE_MEASURE_MAX_COUNT		65535

CCalibratorVisaProcess::CCalibratorVisaProcess()
{
	Initialize();
}

CCalibratorVisaProcess::~CCalibratorVisaProcess()
{
	Dispose();
}

// -------------------------------------------------------------------------------------------------

bool CCalibratorVisaProcess::Initialize()
{
	InitializeVariables();

	if (m_strCalibratorIP != "" && m_nCalibratorPort != 0)
	{
		OpenDevice();
	}

	StartThread();

	return true;
}

bool CCalibratorVisaProcess::InitializeVariables()
{
	m_bIsConnected = false;
	m_bIsInitialized = false;

	m_strCalibratorIP = CMgrConfig::GetMgr()->GetCalibratorIp();
	m_nCalibratorPort = CMgrConfig::GetMgr()->GetCalibratorPort();
	
	m_nMeasureMode = CMgrConfig::GetMgr()->GetMeasureModeNo();

	m_pMeasureThread = NULL;
	m_bLoopMeasure = true;
	m_bIsMeasureRun = false;
	m_nDelayTime = 0;
	// ----------------------------------------------------------------------------

	// BUILD_CHANNEL_DATA
	m_nUseChannelCount = CMgrConfig::GetMgr()->GetCalibratorUseChannelCount();
	for (int channelIndexCount = 0; channelIndexCount < m_nUseChannelCount; channelIndexCount++)
	{
		CCalibratorChannelData* pCalibratorChannelData = new CCalibratorChannelData();

		pCalibratorChannelData->SetChannelIndex(channelIndexCount);						// CHANNEL_INDEX
		pCalibratorChannelData->SetChannelId(MakeDeviceChannelId(channelIndexCount));	// CHANNEL_ID
		
		m_CalibratorChannelData.SetAt(channelIndexCount, pCalibratorChannelData);
	}

	// ----------------------------------------------------------------------------

	// DEVICE - VOLTAGE_CHANNEL
	CString tempVoltageChannel = CMgrConfig::GetMgr()->GetCalibratorVoltageChannel();
	if (tempVoltageChannel.IsEmpty() != true) {
		if (tempVoltageChannel.Find(_T("-")) >= 0) {
			CStringArray tempListChannelNo;
			CPnxUtil::GetMgr()->SplitString(tempVoltageChannel, '-', tempListChannelNo);

			m_nVoltageStartChannelNo = atoi(tempListChannelNo.GetAt(0));
			m_nVoltageEndChannelNo = atoi(tempListChannelNo.GetAt(1));
		}
	}

	// ----------------------------------------------------------------------------

	// DEVICE - CURRENT_CHANNEL
	CString tempCurrentChannel = CMgrConfig::GetMgr()->GetCalibratorCurrentChannel();
	if (tempCurrentChannel.IsEmpty() != true) {
		if (tempCurrentChannel.Find(_T("-")) >= 0) {
			CStringArray tempListChannelNo;
			CPnxUtil::GetMgr()->SplitString(tempCurrentChannel, '-', tempListChannelNo);

			m_nCurrentStartChannelNo = atoi(tempListChannelNo.GetAt(0));
			m_nCurrentEndChannelNo = atoi(tempListChannelNo.GetAt(1));
		}
	}

	// ----------------------------------------------------------------------------
	
	// SHUNT_VALUE
	CString tempShuntValues = CMgrConfig::GetMgr()->GetShuntValues();
	if (tempShuntValues.IsEmpty() != true) {
		if (tempShuntValues.Find(_T(",")) >= 0) {
			CPnxUtil::GetMgr()->SplitString(tempShuntValues, ',', m_sShuntValueArray);
		}
	}
	else {
		m_fShuntValue = CMgrConfig::GetMgr()->GetShuntValue();
	}

	// ----------------------------------------------------------------------------

	m_strTotalDeviceChannelId = MakeDeviceUseChannelIdGroup();

	m_nMeasureCurChIndexNo = -1;
	m_bIsMeasureModeEach = true;
	m_eProcessAutoCalibrationVoltCurrentStep = ProcessStep_AutoCalibrationVolt_Init;
	return true;
}

bool CCalibratorVisaProcess::Dispose()
{
	return true;
}

// -------------------------------------------------------------------------------------------------

int CCalibratorVisaProcess::OpenDevice()
{
	CString logText;

	// Change VISA_ADDRESS to a SOCKET address, e.g. "TCPIP::169.254.104.59::5025::SOCKET"
	// "Your instrument's VISA address goes here!";
	//ViRsrc VISA_ADDRESS = "TCPIP0::192.168.2.101::hislip0::INSTR";			
	CString tempIpAddress;
	tempIpAddress.Format("TCPIP0::%s::inst0::INSTR", m_strCalibratorIP);
	ViRsrc VISA_ADDRESS = (ViString)(LPCSTR)tempIpAddress;

	//LPCSTR

	// Create a connection (session) to the TCP/IP socket on the instrument. 	
	viOpenDefaultRM(&m_resourceManager);
	ViStatus error = viOpen(m_resourceManager, VISA_ADDRESS, VI_NO_LOCK, 0, &m_session);

	if (error >= VI_SUCCESS)
	{
		m_bIsConnected = true;

		// The first thing you should do with a SOCKET connection is enable the Termination Character. 
		// Otherwise all of your read's will fail
		viSetAttribute(m_session, VI_ATTR_TERMCHAR_EN, VI_TRUE);

		// We can find out details of the connection
		ViChar ipAddress[100];
		ViChar hostname[100];
		ViUInt16 port;
		ViUInt16 statusByte;
		viGetAttribute(m_session, VI_ATTR_TCPIP_ADDR, ipAddress);
		viGetAttribute(m_session, VI_ATTR_TCPIP_HOSTNAME, hostname);
		viGetAttribute(m_session, VI_ATTR_TCPIP_PORT, &port);

		printf("IP: %s\nHostname: %s\nPort: %d\n", ipAddress, hostname, port);
		logText.Format(_T("[CALIBRATOR][COMM I/F][VISA] Open Failed. IP_ADDRESS='%s', HOST_NAME='%s', PORT='%d'"), ipAddress, hostname, port);
		SYSLOG(System, logText);
		
		if (m_bIsInitialized == false)
		{
			InitiateDeviceStartup();
			m_bIsInitialized = true;
		}
	}
	else
	{
		// error = -1073807343
		AfxMessageBox(_T("[CALIBRATOR][COMM I/F][VISA] Open Failed"));
		logText.Format("[CALIBRATOR][COMM I/F][VISA] Open Failed. IP_ADDRESS='%s', PORT='%d'", m_strCalibratorIP, m_nCalibratorPort);
		SYSLOG(Err, logText);
	}

	return 0;
}

int CCalibratorVisaProcess::CloseDevice()
{
	// Close the connection to the instrument
	viClose(m_session);

	m_bIsConnected = false;
	m_bIsInitialized = false;

	return 0;
}

// -------------------------------------------------------------------------------------------------

bool CCalibratorVisaProcess::StartThread()
{
	if (m_pMeasureThread == NULL)
	{
		m_pMeasureThread = AfxBeginThread(MeasureDeviceThread, this);
	}

	return true;
}

bool CCalibratorVisaProcess::StopThread()
{
	m_bLoopMeasure = false;

	if (m_pMeasureThread)
	{
		::WaitForSingleObject(m_pMeasureThread->m_hThread, INFINITE);
		m_pMeasureThread = NULL;
	}

	return true;
}

// -------------------------------------------------------------------------------------------------

UINT CCalibratorVisaProcess::MeasureDeviceThread(LPVOID pParam)
{
	CCalibratorVisaProcess* pCalibratorVisaProcess = (CCalibratorVisaProcess*)pParam;
	if (!pCalibratorVisaProcess)
	{
		return 0;
	}

	while (pCalibratorVisaProcess->m_bLoopMeasure)
	{
		if (CMgrConfig::GetMgr()->GetMeasureModeNo() == elFBoradStepModeAutocalVoltage)
		{
			pCalibratorVisaProcess->Auto_Calibration_Volt();
			Sleep(1);
		}
		else
		{
			pCalibratorVisaProcess->MeasureDevice();
			Sleep(1);
		}

		
	}

	return 0;
}

void CCalibratorVisaProcess::MeasureDevice()
{
	if (m_bIsConnected == false)
	{
		return;
	}

	if (Get_MeasureRun() == false)
	{
		return;
	}	// ---------------------------------------------------------------------------

	ViChar idnResponse[5000];
	ViStatus error;
	CString logText;


	//error = viPrintf(m_session, "READ?\n");
	//error = viPrintf(m_session, "MEAS:VOLT:DC? 1,0.003,(@101:116)\n");
	if (m_bIsMeasureModeEach == true)
	{
		
		if (m_nMeasureCurChIndexNo < CMgrConfig::GetMgr()->GetCalibratorUseCurrentChannelCount()) {
			m_nMeasureCurChIndexNo++;
		}
		else {
			m_nMeasureCurChIndexNo = 0;
			Set_MeasureRun(false);
			CMgrConfig::GetMgr()->SetMeasureMode("None");
		}

		m_strEachDeviceChannelId = MakeDeviceChannelId(m_nMeasureCurChIndexNo);		
		error = viPrintf(m_session, "MEAS:VOLT:DC? AUTO,1,(@%s)\n", m_strEachDeviceChannelId);
	}
	else {
		error = viPrintf(m_session, "MEAS:VOLT:DC?  AUTO,1,(%s)\n", m_strTotalDeviceChannelId);
	}

	if (error == VI_ERROR_CONN_LOST)
	{
		//VI_ERROR_CONN_LOST (-1073807194)
		CloseDevice();
	}

	viScanf(m_session, "%t", idnResponse);
	if (error == VI_ERROR_CONN_LOST)
	{
		//VI_ERROR_CONN_LOST (-1073807194)
		CloseDevice();
	}

	//printf(idnResponse);
	logText.Format(_T("[CALIBRATOR][COMM I/F][VISA] RAW_DATA='%s'"), idnResponse);
	//SYSLOG(System, logText);

	// ---------------------------------------------------------------------

	ParseRawData(idnResponse);

	m_nMeasureCount++;
}

BOOL CCalibratorVisaProcess::MeasureDevice_Voltage()
{
	// ---------------------------------------------------------------------------

	ViChar idnResponse[5000];
	ViStatus error;
	CString logText;

	//error = viPrintf(m_session, "READ?\n");
	//error = viPrintf(m_session, "MEAS:VOLT:DC? 1,0.003,(@101:116)\n");
	if (m_bIsMeasureModeEach == true)
	{
		if ((m_nMeasureCurChIndexNo + 1) < 24) {
			m_nMeasureCurChIndexNo++;
		}
		else {
			m_nMeasureCurChIndexNo = 0;
			return true;
		}

		m_strEachDeviceChannelId = MakeDeviceChannelId(m_nMeasureCurChIndexNo);
		error = viPrintf(m_session, "MEAS:VOLT:DC? Auto,1,(@%s)\n", m_strEachDeviceChannelId);
	}
	else {
		error = viPrintf(m_session, "MEAS:VOLT:DC? Auto,1,(%s)\n", m_strTotalDeviceChannelId);
	}

	if (error == VI_ERROR_CONN_LOST)
	{
		//VI_ERROR_CONN_LOST (-1073807194)
		CloseDevice();
	}

	viScanf(m_session, "%t", idnResponse);
	if (error == VI_ERROR_CONN_LOST)
	{
		//VI_ERROR_CONN_LOST (-1073807194)
		CloseDevice();
	}

	//printf(idnResponse);
	//logText.Format(_T("[CALIBRATOR][COMM I/F][VISA] RAW_DATA='%s'"), idnResponse);
	//SYSLOG(System, logText);

	// ---------------------------------------------------------------------

	ParseRawData(idnResponse);

	
	m_nMeasureCount++;

	return false;
}

void CCalibratorVisaProcess::SetProcessAutoCalibrationVoltAutoStep(enumProcessAutoCalibrationVolt estepCurrent)
{
	m_eProcessAutoCalibrationVoltCurrentStep = estepCurrent;
}

void CCalibratorVisaProcess::Init_ProcessAutoCalibrationVolt()
{
	SetProcessAutoCalibrationVoltAutoStep(ProcessStep_AutoCalibrationVolt_Init);
}

void CCalibratorVisaProcess::Auto_Calibration_Volt()
{
	if (m_bIsConnected == false)
	{
		return;
	}

	if (Get_MeasureRun() == false)
	{
		return;
	}

	m_nMeasureMode = CMgrConfig::GetMgr()->GetMeasureModeNo();
	CMgrPid* pMgrPid = CMgrPid::GetMgr();
	CCommData* pMgrCommdata;
	switch (m_eProcessAutoCalibrationVoltCurrentStep)
	{
	case ProcessStep_AutoCalibrationVolt_Init:

		if (pMgrPid != nullptr) {			
			pMgrPid->GetDevice(0)->SetAOValue_Select_All(m_fSettingVoltValue);
			Sleep(Get_DelayTime());
			SetProcessAutoCalibrationVoltAutoStep(ProcessStep_AutoCalibrationVolt_Measure_Start);
		}
		break;
	case ProcessStep_AutoCalibrationVolt_Measure_Start:		
		if (MeasureDevice_Voltage() == true)
		{			
			SetProcessAutoCalibrationVoltAutoStep(ProcessStep_AutoCalibrationVolt_Measure_Average_Write);
		}
		break;
	case ProcessStep_AutoCalibrationVolt_Measure_Average_Write:
		if ((m_nMeasureCurChIndexNo + 1) < 25) {
			SYSTEMTIME tmCur;
			GetLocalTime(&tmCur);
			CMgrLog::GetMgr()->CreateVoltCalibrationDir(m_nMeasureCurChIndexNo, tmCur, m_fSettingVoltValue, GetCalibratorChannelDataVoltage(m_nMeasureCurChIndexNo+1));
			CChannel* pChannel = CMgrChannel::GetMgr()->GetAt(m_nMeasureCurChIndexNo);

			//Data 쌓기
			pChannel->Set_VoltCalibrationData(m_fSettingVoltValue, GetCalibratorChannelDataVoltage(m_nMeasureCurChIndexNo + 1));

			m_nMeasureCurChIndexNo++;
		}
		else {
			m_nMeasureCurChIndexNo = 0;
			SetProcessAutoCalibrationVoltAutoStep(ProcessStep_AutoCalibrationVolt_Measure_Next_Volt_Setting);
		}
		
		break;
	case ProcessStep_AutoCalibrationVolt_Measure_Next_Volt_Setting:
		if (m_fSettingVoltValue == 5.0f)
		{
			m_fSettingVoltValue = 0.0f;
			pMgrPid->GetDevice(0)->SetAOValue_Select_All(m_fSettingVoltValue);
			SetProcessAutoCalibrationVoltAutoStep(ProcessStep_AutoCalibrationVolt_Measure_End);
		}
		else
		{
			m_fSettingVoltValue += 1.0f;
			SetProcessAutoCalibrationVoltAutoStep(ProcessStep_AutoCalibrationVolt_Init);
		}
		break;
	case ProcessStep_AutoCalibrationVolt_Measure_End:
		m_fSettingVoltValue = 1.0f;
		Set_MeasureRun(false);
		CMgrConfig::GetMgr()->SetMeasureMode("None");
		//FW로 데이터 전송


		SetProcessAutoCalibrationVoltAutoStep(ProcessStep_AutoCalibrationVolt_Init);
		break;
	case ProcessStep_AutoCalibrationVolt_Measure_Error:

		SetProcessAutoCalibrationVoltAutoStep(ProcessStep_AutoCalibrationVolt_Init);
		break;
	}
}

void CCalibratorVisaProcess::ParseRawData(CString strRawData)
{
	int	nSubString = 0;
	int nRowIndex = 0;

	CString resultString;
	CStringArray sRawDataArray;
	
	while (AfxExtractSubString(resultString, strRawData, nSubString++, _T(',')))
	{
		sRawDataArray.SetAtGrow(nRowIndex, resultString.Trim());
		nRowIndex++;
	}

	// ----------------------------------------------------------------------------

	CString tempString;
	CString resultValue;

	m_sResultDataArray.RemoveAll();
	int nTotalDataCount = sRawDataArray.GetCount();
	
	for (int dataIndexCount = 0; dataIndexCount < nTotalDataCount; dataIndexCount++)
	{
		tempString.Format(_T("%s"), sRawDataArray[dataIndexCount]);

		CString tempValue = tempString.Left(11);
		CString tempPointValue = tempString.Mid(11, 4);

		float ftempValue = atof(tempValue) / DotPositon(tempPointValue);
		resultValue.Format("%f", ftempValue);

		// ---------------------------------------------------------------------------

		int tempChannelIndexNo = -1;
		if (m_bIsMeasureModeEach == true) {
			tempChannelIndexNo = m_nMeasureCurChIndexNo;
		}
		else {
			tempChannelIndexNo = dataIndexCount;
		}

		// ---------------------------------------------------------------------------

		// TEST
		m_sResultDataArray.SetAtGrow(dataIndexCount, resultValue);
		printf("[CH:%02d][INDEX:%02d] %s = %s\n", m_nMeasureCurChIndexNo, dataIndexCount, tempString, resultValue);

		// ---------------------------------------------------------------------------
		if (resultValue.CompareNoCase("inf") != 0)
		{
			if (CMgrConfig::GetMgr()->GetMeasureMode().CompareNoCase(_T("VoltCalibration")) == 0)
			{
				SetCalibratorChannelDataVoltage(tempChannelIndexNo, ftempValue);
			}
			else if (CMgrConfig::GetMgr()->GetMeasureMode().CompareNoCase(_T("CurrCalibration")) == 0)
			{
				SetCalibratorChannelDataCurrent(tempChannelIndexNo, ftempValue);
			}

		}
	}
}

int CCalibratorVisaProcess::DotPositon(CString nScale)
{
	if (nScale == "E-00"|| nScale == "E+00")
	{
		return 1;
	}
	else if (nScale == "E-01")
	{
		return 10;
	}
	else if (nScale == "E-02")
	{
		return 100;
	}
	else if (nScale == "E-03")
	{
		return 1000;
	}
	else if (nScale == "E-04")
	{
		return 10000;
	}
	else if (nScale == "E-05")
	{
		return 100000;
	}
	else if (nScale == "E-06")
	{
		return 1000000;
	}

	return 0;
}

// -----------------------------------------------------------------------------

void CCalibratorVisaProcess::InitiateDeviceStartup()
{
	InitiateDeviceVoltage();
}

void CCalibratorVisaProcess::InitiateDeviceVoltage()
{
	viPrintf(m_session, "*RST\n");													// FACTORY_RESET

	//viPrintf(m_session, "CONF:VOLT:DC 10,0.003,(@101:116,201:216,301:316)\n");	// 1~3 SLOT
	//viPrintf(m_session, "CONF:VOLT:DC 10,0.003,(@101:116)\n");					// 1 SLOT
	//viPrintf(m_session, "CONF:VOLT:DC 10,0.003,(@101)\n");						// 1 CHANNEL
	viPrintf(m_session, "CONF:VOLT:DC 10,1,(%s)\n", m_strTotalDeviceChannelId);
	
	//viPrintf(m_session, "TRIG:SOUR EXT\n");										// ???
	viPrintf(m_session, "INIT\n");
	viPrintf(m_session, "FETC?\n");
}

void CCalibratorVisaProcess::InitiateDeviceCurrent()
{
	viPrintf(m_session, "*RST\n");													// FACTORY_RESET

																					// ???
	
	//viPrintf(m_session, "TRIG:SOUR EXT\n");										// ???
	viPrintf(m_session, "INIT\n");
	viPrintf(m_session, "FETC?\n");
}

CString CCalibratorVisaProcess::MakeDeviceUseChannelIdGroup()
{
	CString returnValue;

	int nTotalSlotCount = m_nUseChannelCount / DEVICE_SLOT_CHANNEL_MAX_COUNT;	// 1_SLOT = 16_CHANNEL
	
	bool isModValue = false;
	int nTempSlotNo = m_nUseChannelCount % DEVICE_SLOT_CHANNEL_MAX_COUNT;
	if (nTempSlotNo != 0)
	{
		nTotalSlotCount++;
		isModValue = true;
	}

	for (int nSlotCount = 1; nSlotCount <= nTotalSlotCount; nSlotCount++)
	{
		CString tempChannelId;

		if (nSlotCount != nTotalSlotCount)
			tempChannelId.Format("%d%02d:%d%02d", nSlotCount, 1, nSlotCount, DEVICE_SLOT_CHANNEL_MAX_COUNT);
		else
		{
			if (isModValue == false)
				tempChannelId.Format("%d%02d:%d%02d", nSlotCount, 1, nSlotCount, DEVICE_SLOT_CHANNEL_MAX_COUNT);
			else
				tempChannelId.Format("%d%02d:%d%02d", nSlotCount, 1, nSlotCount, nTempSlotNo);
		}

		if (returnValue.IsEmpty() == true) 
			returnValue += "@";
		else
			returnValue += ",";

		returnValue += tempChannelId;
	}

	return returnValue;
}

CString CCalibratorVisaProcess::MakeDeviceChannelId(int nChannelIndexNo)
{
	int tempSlotNo = ((nChannelIndexNo + 1) / 12) + 1;	
	int tempChannelNo = ((nChannelIndexNo + 1) % 12);	

	if (tempSlotNo != 0 && tempChannelNo == 0)
	{
		tempSlotNo -= 1;
		tempChannelNo = DEVICE_SLOT_CHANNEL_MAX_COUNT;
	}

	CString tempChannelId;
	tempChannelId.Format("%d%02d", tempSlotNo, tempChannelNo);

	return tempChannelId;
}

void CCalibratorVisaProcess::ClearChannelVoltageValueArray(int nChannelIndexNo)
{
	CCalibratorChannelData* pCalibratorChannelData;
	if (m_CalibratorChannelData.Lookup(nChannelIndexNo, pCalibratorChannelData) == true)
	{
		pCalibratorChannelData->GetVoltageValueArray()->RemoveAll();
	}
}

void CCalibratorVisaProcess::ClearChannelCurrentValueArray(int nChannelIndexNo)
{
	CCalibratorChannelData* pCalibratorChannelData;
	if (m_CalibratorChannelData.Lookup(nChannelIndexNo, pCalibratorChannelData) == true)
	{
		pCalibratorChannelData->GetCurrentValueArray()->RemoveAll();
	}
}

// -------------------------------------------------------------------------------------------------

void CCalibratorVisaProcess::SetCalibratorChannelDataVoltage(int nChannelIndexNo, float value)
{
	CCalibratorChannelData* pCalibratorChannelData;
	if (m_CalibratorChannelData.Lookup(nChannelIndexNo, pCalibratorChannelData) == true)
	{
		if (m_nMeasureCount == 0) {
			pCalibratorChannelData->SetCurrentVoltageValue(0.0f);
		}
		else {
			pCalibratorChannelData->SetCurrentVoltageValue(value);
		}

		// ---------------------------------------------------------------------

		if (m_nMeasureCount == 0) {
			pCalibratorChannelData->GetVoltageValueArray()->RemoveAll();
		}
		
		if (m_nMeasureCount <= DEVICE_MEASURE_MAX_COUNT)
		{
			CString strtempValue;
			LPCTSTR tempValue;
			strtempValue.Format(_T("%f"), value);
			 
			tempValue = strtempValue;
			pCalibratorChannelData->GetVoltageValueArray()->SetAtGrow(m_nMeasureCount, tempValue);
		}
	}
}

float CCalibratorVisaProcess::GetCalibratorChannelDataVoltage(int nChannelIndexNo)
{
	float returnValue = 0.0f;

	CCalibratorChannelData* pCalibratorChannelData;
	if (m_CalibratorChannelData.Lookup(nChannelIndexNo, pCalibratorChannelData) == true)
	{
		returnValue = pCalibratorChannelData->GetCurrentVoltageValue();
	}

	return returnValue;
}

void CCalibratorVisaProcess::SetCalibratorChannelDataCurrent(int nChannelIndexNo, float value)
{
	CCalibratorChannelData* pCalibratorChannelData;
	if (m_CalibratorChannelData.Lookup(nChannelIndexNo, pCalibratorChannelData) == true)
	{
		float tempCurrentValue = 0.0f;

		if (m_sShuntValueArray.IsEmpty() == true) {
			tempCurrentValue = (value * m_fShuntValue) * 1000;			// 1000 곱해준이유?
		}
		else 
		{
			float tempShuntValue = atof(m_sShuntValueArray.GetAt(nChannelIndexNo));
			tempCurrentValue = (value * tempShuntValue) * 1000;			// 1000 곱해준이유?
		}
		
		// ---------------------------------------------------------------------

		if (m_nMeasureCount == 0) {
			pCalibratorChannelData->SetCurrentCurrentValue(0.0f);
		}
		else {
			pCalibratorChannelData->SetCurrentCurrentValue(tempCurrentValue);
		}

		// ---------------------------------------------------------------------

		if (m_nMeasureCount == 0) {
			pCalibratorChannelData->GetCurrentValueArray()->RemoveAll();
		}

		if (m_nMeasureCount <= DEVICE_MEASURE_MAX_COUNT)
		{
			CString strtempValue;
			strtempValue.Format(_T("%f"), tempCurrentValue);
			LPCTSTR tempValue;

			tempValue = strtempValue;
			pCalibratorChannelData->GetCurrentValueArray()->SetAtGrow(nChannelIndexNo, tempValue);
		}
	}
}

float CCalibratorVisaProcess::GetCalibratorChannelDataCurrent(int nChannelIndexNo)
{
	float returnValue = 0.0f;

	CCalibratorChannelData* pCalibratorChannelData;
	if (m_CalibratorChannelData.Lookup(nChannelIndexNo, pCalibratorChannelData) == true) {
		returnValue = pCalibratorChannelData->GetCurrentCurrentValue();
	}

	return returnValue;
}

float CCalibratorVisaProcess::GetCalibratorChannelVoltageAvg(int nChannelIndexNo, int nDataStartIndex, int nDataEndIndex)
{
	float returnValue = 0.0f;

	CCalibratorChannelData* pCalibratorChannelData;
	if (m_CalibratorChannelData.Lookup(nChannelIndexNo, pCalibratorChannelData) == true) {
		returnValue = pCalibratorChannelData->GetVoltageArrayAverage(nDataStartIndex, nDataEndIndex);
	}

	return returnValue;
}

float CCalibratorVisaProcess::GetCalibratorChannelCurrentAvg(int nChannelIndexNo, int nDataStartIndex, int nDataEndIndex)
{
	float returnValue = 0.0f;

	CCalibratorChannelData* pCalibratorChannelData;
	if (m_CalibratorChannelData.Lookup(nChannelIndexNo, pCalibratorChannelData) == true) {
		returnValue = pCalibratorChannelData->GetCurrentArrayAverage(nDataStartIndex, nDataEndIndex);
	}

	return returnValue;
}