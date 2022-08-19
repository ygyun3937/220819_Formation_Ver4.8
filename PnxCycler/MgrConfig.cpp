#include "stdafx.h"
#include "MgrConfig.h"
#include "MgrChannel.h"
#include "MgrCommData.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CommonFilePath	( _T("Common.ini") )

CMgrConfig::CMgrConfig()
{
	m_strLogFilePath.Empty();
	m_strSchedulePath.Empty();
	m_strDAQFilePath.Empty();

	m_fEQVoltageMin			= 0.000;
	m_fEQVoltageMax			= 0.000;
	m_fEQAmpereMin			= 0.000;
	m_fEQAmpereMax			= 0.000;
	m_bLogUse				= FALSE;
	m_bDAQType				= FALSE;
	m_bErrorFilePath		= false;
	m_bErrorSystemSafety	= false;
	m_bUseDeltaV			= false;
	m_bUseCvDeltaV			= false;
	m_nMicroV				= 0;
	m_nMicroA				= 0;

	m_strCalibrationMode = "None";
	LoadCommon();
}

CMgrConfig::~CMgrConfig()
{
	SaveRunningCount();

	{
		auto iter = m_vecSystemSafetyMin.begin();
		auto iterE = m_vecSystemSafetyMin.end();

		while (iter != iterE)
		{
			SSystemSafety* lpSystemSafety = (*iter);

			if (lpSystemSafety)
			{
				delete lpSystemSafety;
				lpSystemSafety = nullptr;
			}

			++iter;
		}

		m_vecSystemSafetyMin.clear();
	}

	{
		auto iter = m_vecSystemSafetyMax.begin();
		auto iterE = m_vecSystemSafetyMax.end();

		while (iter != iterE)
		{
			SSystemSafety* lpSystemSafety = (*iter);

			if (lpSystemSafety)
			{
				delete lpSystemSafety;
				lpSystemSafety = nullptr;
			}

			++iter;
		}

		m_vecSystemSafetyMax.clear();
	}
}

//Common.ini Save Function
void	CMgrConfig::SaveCommon()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\%s"), GetExePath(), CommonFilePath);

	//CString strAppName = _T("Cycler");
	CString strAppName = _T("EqpInfo");

	INIWriteInt(strAppName, _T("Type"), m_nCyclerType, strFilePath);
	INIWriteInt(strAppName, _T("Number"), m_nEQNumber, strFilePath);
	INIWriteStr(strAppName, _T("Name"), m_strProjectName, strFilePath);

	LPCTSTR lpszSystemSafetyMin[] = { "VoltageMin", "AmpereMin", "CapacityMin", "TemperatureMin", "", "", "", "", "", "" };

	for (int i = 0; i < eSyatemSafetyObjectMaxCount; ++i)
	{
		SSystemSafety* lpSystemSafety = m_vecSystemSafetyMin[i];

		if(lpSystemSafety)
			INIWriteStr("SafetyCondition", lpszSystemSafetyMin[i], lpSystemSafety->strValue, strFilePath);
	}

	m_bErrorSystemSafety = false;

	LPCTSTR lpszSystemSafetyMax[] = { "VoltageMax", "AmpereMax", "CapacityMax", "TemperatureMax", "DeltaV", "DeltaI", "TargetT", "ErrorLoggingTime", "CvDeltaV", "CvDeltaA" };

	for (int i = 0; i < eSyatemSafetyObjectMaxCount; ++i)
	{
		SSystemSafety* lpSystemSafety = m_vecSystemSafetyMax[i];

		if (lpSystemSafety)
		{
			INIWriteStr("SafetyCondition", lpszSystemSafetyMax[i], lpSystemSafety->strValue, strFilePath);

			if (true == lpSystemSafety->bIsWarning)
				m_bErrorSystemSafety = true;
		}			
	}

	if(m_bUseDeltaV)
		INIWriteInt("SafetyCondition", _T("DeltaUse"), 1, strFilePath);
	else
		INIWriteInt("SafetyCondition", _T("DeltaUse"), 0, strFilePath);

	if(m_bUseCvDeltaV)
		INIWriteInt("SafetyCondition", _T("CvDeltaUse"), 1, strFilePath);
	else
		INIWriteInt("SafetyCondition", _T("CvDeltaUse"), 0, strFilePath);

	INIWriteStr("FilePath", "Log", m_strLogFilePath, strFilePath);
	INIWriteStr("FilePath", "Schedule", m_strSchedulePath, strFilePath);
}

//Common.ini Load Function
void	CMgrConfig::LoadCommon()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\%s"), (LPCSTR)GetExePath(), CommonFilePath);

	//CString strAppName = _T("Cycler");
	CString strAppName = _T("EqpInfo");

	m_strCyclerEQPID	= INIReadStr(strAppName, _T("EQPID")	, strFilePath);
	m_nCyclerType = INIReadInt(strAppName, _T("Type"), strFilePath);
	m_nEQNumber = INIReadInt(strAppName, _T("Number"), strFilePath);
	m_strProjectName = INIReadStr(strAppName, _T("Name"), strFilePath);
	m_nTotalSubEqpCount = INIReadInt(strAppName, _T("TotalSubEqpCount"), strFilePath);			// FORMATION

	if (m_strCyclerEQPID.IsEmpty())
	{
		m_strCyclerEQPID = "PNX_001";
		SYSLOG(Dbg, "EQPID is not set during CONFIGURE information LOADING, so it is applied as a default value.");
	}

	// ----------------------------------------------------------------------------
	// SUBEQP INFO
/*
	for (int subEqpCount = 0; subEqpCount < m_nTotalSubEqpCount; subEqpCount++)
	{*/

		CString tempAppName;
		tempAppName.Format("SubEqpId%d", 1);

		m_SubEqpInfo.nSubEqpNo = 0;
		m_SubEqpInfo.strSubEqpId = INIReadStr(tempAppName, _T("SubEqpId"), strFilePath);
		m_SubEqpInfo.strStage1Name = INIReadStr(tempAppName, _T("Stage1Name"), strFilePath);
		m_SubEqpInfo.strStage2Name = INIReadStr(tempAppName, _T("Stage2Name"), strFilePath);

		CString strUseStage1 = INIReadStr(tempAppName, "Stage1Use", strFilePath);

		m_SubEqpInfo.nSubEqpStageMaxCount = 0;
		if (strcmp(strUseStage1, "1") == 0)
		{
			m_SubEqpInfo.bStage1Use = true;
			m_SubEqpInfo.nSubEqpStageMaxCount++;
		}
		else
		{
			m_SubEqpInfo.bStage1Use = false;
		}
		CString strUseStage2 = INIReadStr(tempAppName, "Stage2Use", strFilePath);

		if (strcmp(strUseStage2, "1") == 0)
		{
			m_SubEqpInfo.bStage2Use = true;
			m_SubEqpInfo.nSubEqpStageMaxCount++;
		}
		else
		{
			m_SubEqpInfo.bStage2Use = false;
		}

		m_SubEqpInfo.nSubEqpStageChannelCount = INIReadInt(tempAppName, _T("StageChannelCount"), strFilePath);
		m_SubEqpInfo.nSubEqpStageChannelTotalCount = INIReadInt(tempAppName, _T("SubEqpStageChannelTotalCount"), strFilePath);
	//}
	
	// ----------------------------------------------------------------------------

	LPCTSTR lpszSystemSafetyMin[] = { "VoltageMin", "AmpereMin", "CapacityMin", "TemperatureMin", "", "", "", "",  "", ""};

	for (int i = 0; i < eSyatemSafetyObjectMaxCount ; ++i)
	{
		CString strValue = INIReadStr("SafetyCondition", lpszSystemSafetyMin[i], strFilePath);

		SSystemSafety* lpSystemSafety = new SSystemSafety;
		lpSystemSafety->SetValue(false, strValue);

		m_vecSystemSafetyMin.push_back(lpSystemSafety);
	}	

	LPCTSTR lpszSystemSafetyMax[] = { "VoltageMax", "AmpereMax", "CapacityMax", "TemperatureMax", "DeltaV", "DeltaI", "TargetT", "ErrorLoggingTime", "CvDeltaV", "CvDeltaA" };

	for (int i = 0; i < eSyatemSafetyObjectMaxCount ; ++i)
	{
		CString strValue = INIReadStr("SafetyCondition", lpszSystemSafetyMax[i], strFilePath);

		SSystemSafety* lpSystemSafety = new SSystemSafety;
		lpSystemSafety->SetValue(true, strValue);

		m_vecSystemSafetyMax.push_back(lpSystemSafety);

		if (true == lpSystemSafety->bIsWarning)
			m_bErrorSystemSafety = true;
	}

	strAppName = _T("SafetyCondition");

	CString strUseDeltaV = INIReadStr("SafetyCondition", "DeltaUse", strFilePath);

	if (strcmp(strUseDeltaV, "1") == 0)
		m_bUseDeltaV = true;
	else
		m_bUseDeltaV = false;

	CString strUseCvDeltaV = INIReadStr("SafetyCondition", "CvDeltaUse", strFilePath);

	if (strcmp(strUseCvDeltaV, "1") == 0)
		m_bUseCvDeltaV = true;
	else
		m_bUseCvDeltaV = false;

	strAppName = _T("FilePath");

	m_strLogFilePath	 = INIReadStr(strAppName, _T("Log"), strFilePath);					//로그 파일 경로
	m_strSchedulePath	 = INIReadStr(strAppName, _T("Schedule"), strFilePath);				//스케줄 파일 경로

	if (m_strLogFilePath.IsEmpty() || m_strSchedulePath.IsEmpty())
		m_bErrorFilePath = true;

	m_bLogUse	= INIReadInt(strAppName, _T("SaveLog"), strFilePath);
	m_bDAQType	= INIReadInt(strAppName, _T("DAQType"), strFilePath);

	CString strUseDubugLog = INIReadStr("FilePath", "DebugLogUse", strFilePath);

	if (strcmp(strUseDubugLog, "1") == 0)
		m_bDebugLog = true;
	else
		m_bDebugLog = false;
	

	m_strDAQFilePath = INIReadStr(strAppName, _T("DAQ"), strFilePath);

	strAppName = _T("EQProperty");

	m_fEQVoltageMin	= INIReadFloat(strAppName, _T("VoltageMin"), strFilePath);
	m_fEQVoltageMax	= INIReadFloat(strAppName, _T("VoltageMax"), strFilePath);
	m_fEQAmpereMin	= INIReadFloat(strAppName, _T("AmpereMin"), strFilePath);
	m_fEQAmpereMax	= INIReadFloat(strAppName, _T("AmpereMax"), strFilePath);

	m_fConnectVolt = INIReadFloat(strAppName, _T("ConnectVoltage"), strFilePath);

	strAppName = _T("ETC");

	m_nMicroV = INIReadInt(strAppName, _T("MicroV"), strFilePath);
	m_nMicroA = INIReadInt(strAppName, _T("MicroA"), strFilePath);

	strAppName = _T("DCIR Time");
	m_strDcirTime = INIReadStr(strAppName, _T("Time(sec)"), strFilePath);

	strAppName = _T("Parallel");
	m_nParallelChannelCount = INIReadInt(strAppName, _T("ParallelChannelCount"), strFilePath);
	m_nChannelCountInOneBoard = INIReadInt(strAppName, _T("ChannelCountInOneBoard"), strFilePath);

	strAppName = _T("DIO");
	m_strDioType = INIReadStr(strAppName, _T("DioType"), strFilePath);
	m_strDioIniName = INIReadStr(strAppName, _T("DioIniName"), strFilePath);
	m_nDioCount = INIReadInt(strAppName, _T("DioCount"), strFilePath);

	//Moxa Board
	if (m_strDioType == "MOXA")
	{
		for (int i = 0; i < m_nDioCount; i++)
		{

			MoxaIOInfo* lpMoxaIOInfo = new MoxaIOInfo;
			strAppName.Format(_T("MOXADIO%d"), i + 1);
			lpMoxaIOInfo->strIP = INIReadStr(strAppName, _T("IPAddress"), strFilePath);
			lpMoxaIOInfo->nChCount = INIReadInt(strAppName, _T("ChCount"), strFilePath);
			lpMoxaIOInfo->nIOType = INIReadStr(strAppName, _T("IOType"), strFilePath);

			m_vecMoxaDevice.push_back(lpMoxaIOInfo);

		}
	}


	strAppName = _T("PLC");

	CString strUsePLCControlUse = INIReadStr(strAppName, _T("Use"), strFilePath);
	if (strcmp(strUsePLCControlUse, "1") == 0)
		m_bPLCControlUse = true;
	else
		m_bPLCControlUse = false;

	m_strPLCControlType = INIReadStr(strAppName, _T("Type"), strFilePath);

	// CAN
	strAppName = _T("EQMode");
	m_nEQMode = INIReadInt(strAppName, _T("Mode"), strFilePath);

	// AUTO_CALIBRATION
	strAppName = _T("AutoCalibration");
	m_nCalibratorUseChannelCount = INIReadInt(strAppName, _T("UseChannelCount"), strFilePath);
	m_nCalibratorUseVoltChannelCount = INIReadInt(strAppName, _T("UseVoltChannelCount"), strFilePath);
	m_nCalibratorUseCurrentChannelCount = INIReadInt(strAppName, _T("UseCurrentChannelCount"), strFilePath);
	//m_nInitialEQVoltV = INIReadInt(strAppName, _T("InitialEQVolt(V)"), strFilePath);
	//m_nInitialEQCurrA = INIReadInt(strAppName, _T("InitialEQCurr(A)"), strFilePath);
	m_fShuntValue = INIReadFloat(strAppName, _T("ShuntValue"), strFilePath);
	//m_strCalibrationMode = INIReadStr(strAppName, _T("CalibrationMode"), strFilePath); 
	//m_fCalibrationVoltV = INIReadFloat(strAppName, _T("CalibrationVolt(V)"), strFilePath);
	//m_fCalibrationCurrA = INIReadFloat(strAppName, _T("CalibrationCurr(A)"), strFilePath);
	int tempValue = INIReadInt(strAppName, _T("AutoMode"), strFilePath);
	m_bAutoMode = tempValue;

	// Comm.ini 파일
	strFilePath.Format(_T("%s\\Config\\Comm.ini"), GetExePath());
	m_nChamberChannelCount = INIReadInt(_T("CommCount"), _T("ChamberChannelCount"), strFilePath);
}

//Common.ini - Channel Running Count Save Function
void CMgrConfig::SaveRunningCount()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\%s"), (LPCSTR)GetExePath(), CommonFilePath);

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

		auto Pos = listCyclerChannel.GetHeadPosition();

		while (Pos)
		{
			CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

			if (!lpCyclerChannel)
				continue;

			CString strKeyName;
			strKeyName.Format(_T("ch%d"), lpCyclerChannel->GetChannelNumber());

			INIWriteInt("ChannelRunningTotal", strKeyName, lpCyclerChannel->GetRunningCount(), strFilePath);
		}
	}
}

//Common.ini - Channel Running Count Load Function
void CMgrConfig::GetChannelRunningTotal()
{
	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\%s"), (LPCSTR)GetExePath(), CommonFilePath);

	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	if (pMgr)
	{
		CList< CChannel*, CChannel* >& listCyclerChannel = pMgr->GetChannelList();

		auto Pos = listCyclerChannel.GetHeadPosition();

		while (Pos)
		{
			CChannel* lpCyclerChannel = listCyclerChannel.GetNext(Pos);

			if(!lpCyclerChannel)
				continue;

			CString strKeyName;
			strKeyName.Format(_T("ch%d"), lpCyclerChannel->GetChannelNumber());

			lpCyclerChannel->SetRunningCount(INIReadInt("ChannelRunningTotal", strKeyName, strFilePath));
		}
	}
}

//LoadCommon에서 불러온 시스텝 안전조건 중 Min값 Get (ALL)
void	CMgrConfig::GetSystemSafeInfoMin(std::vector<CString>& vecValue)
{
	vecValue.clear();

	auto iter = m_vecSystemSafetyMin.begin();
	auto iterE = m_vecSystemSafetyMin.end();

	while (iter != iterE)
	{
		SSystemSafety* lpSystemSafety = (*iter);

		if (lpSystemSafety)
			vecValue.push_back(lpSystemSafety->strValue);
		else
			vecValue.push_back("");

		++iter;
	}
}

//LoadCommon에서 불러온 시스텝 안전조건 중 Max값 Get (ALL)
void	CMgrConfig::GetSystemSafeInfoMax(std::vector<CString>& vecValue)
{
	vecValue.clear();

	auto iter = m_vecSystemSafetyMax.begin();
	auto iterE = m_vecSystemSafetyMax.end();

	while (iter != iterE)
	{
		SSystemSafety* lpSystemSafety = (*iter);

		if (lpSystemSafety)
			vecValue.push_back(lpSystemSafety->strValue);
		else
			vecValue.push_back("");

		++iter;
	}
}

CString CMgrConfig::GetSystemSafeInfoTempMax()
{
	vector<CString> vecSystemMax;
	GetSystemSafeInfoMax(vecSystemMax);
	return vecSystemMax[3];
}

//시스템 안전조건 Set (SaveCommon에서 이 함수를 통해 Set 값을 저장) (ALL)
void	CMgrConfig::SetSystemSafeInfoMin(int nIndex, CString strValue)
{
	if (nIndex >= 0 && (int)m_vecSystemSafetyMin.size() > nIndex)
	{
		SSystemSafety* lpSystemSafety = m_vecSystemSafetyMin[nIndex];

		if (lpSystemSafety)
			lpSystemSafety->SetValue(false, strValue);
	}
}

//시스템 안전조건 Set (SaveCommon에서 이 함수를 통해 Set 값을 저장) (ALL)
void	CMgrConfig::SetSystemSafeInfoMax(int nIndex, CString strValue)
{
	if (nIndex >= 0 && (int)m_vecSystemSafetyMax.size() > nIndex)
	{
		SSystemSafety* lpSystemSafety = m_vecSystemSafetyMax[nIndex];

		if (lpSystemSafety)
			lpSystemSafety->SetValue(true, strValue);
	}
}
//LoadCommon에서 불러온 시스텝 안전조건 중 Min값 Get (해당 Index만)
float	CMgrConfig::GetSystemSafetyMin(int nIndex)
{
	if (nIndex < 0 || (int)m_vecSystemSafetyMin.size() <= nIndex)
		return 0.000f;

	SSystemSafety* lpSystemSafety = m_vecSystemSafetyMin[nIndex];

	if (!lpSystemSafety)
		return 0.000f;

	return lpSystemSafety->fValue;
}

//LoadCommon에서 불러온 시스텝 안전조건 중 Max값 Get (해당 Index만)
float	CMgrConfig::GetSystemSafetyMax(int nIndex)
{
	if (nIndex < 0 || (int)m_vecSystemSafetyMax.size() <= nIndex)
		return 0.000f;

	SSystemSafety* lpSystemSafety = m_vecSystemSafetyMax[nIndex];

	if (!lpSystemSafety)
		return 0.000f;

	return lpSystemSafety->fValue;
}

CString CMgrConfig::GetConnectVoltStr()
{
	CString csfConnectVolt;
	csfConnectVolt.Format("%.3f", GetConnectVolt());
	return csfConnectVolt.GetString();
}

// 통합 안전조건
TOTAL_SAFETY CMgrConfig::GetTotalSafety(int nChNum)
{
	return m_stTotalSafety[nChNum];
}

void CMgrConfig::SetTotalSafety(int nChNum, TOTAL_SAFETY totalSafety)
{
	m_stTotalSafety[nChNum] = totalSafety;
}

CString	CMgrConfig::GetVersionStr()
{
	CString strType;

	switch (m_nCyclerType)
	{
	case CyclerTypeCell: strType = _T("Cell");		break;
	case CyclerTypeModule: strType = _T("Module");	break;
	case CyclerTypePack: strType = _T("Pack");		break;
	}

	CString str;

	str.Format(_T("%s %s Cycler[EQ Number:%d](Version:%s)(Build:%s)"), m_strProjectName.GetString(), strType.GetString(), m_nEQNumber, GetVersionString(), GetVersionPrivateBuild());

	return str.GetString();
}

void CMgrConfig::GetChannelContactResistance()
{
	CMgrChannel* pMgr = CMgrChannel::GetMgr();

	CString strFilePath;
	strFilePath.Format(_T("%s\\Config\\%s"), (LPCSTR)GetExePath(), CommonFilePath);
	CList< CChannel*, CChannel* >& listChannel = pMgr->GetChannelList();

	CMgrCommData* pMgrCommData = CMgrCommData::GetMgr();

	if (pMgrCommData)
	{
		auto Pos = listChannel.GetHeadPosition();		

		while (Pos)
		{
			CChannel* lpCyclerChannel = listChannel.GetNext(Pos);

			if (!lpCyclerChannel)
				continue;

			CString strKeyName;
			CCommData* lpCyclerInfo = pMgrCommData->GetAt(lpCyclerChannel->GetChannelNumber());
			strKeyName.Format(_T("ch%d"), lpCyclerChannel->GetChannelNumber());

			lpCyclerInfo->SetPogoOhmSetting(INIReadFloat("ContactResistance", strKeyName, strFilePath));

		}
	}
}

// -------------------------------------------------------------------------------------------------
// FORMATION 

void CMgrConfig::SetSubEqpInfo( SUBEQP_INFO subeqpInfo)
{
	m_SubEqpInfo = subeqpInfo;
}

SUBEQP_INFO CMgrConfig::GetSubEqpInfo()
{
	return m_SubEqpInfo;
}

// -----------------------------------------------------------------------------

int CMgrConfig::GetMeasureModeNo()
{
	if (m_strCalibrationMode.CompareNoCase(_T("None")) == 0)
	{
		m_nCalibrationModeNo = 0;
	}
	else if (m_strCalibrationMode.CompareNoCase(_T("VoltCalibration")) == 0)
	{
		m_nCalibrationModeNo = elFBoradStepModeAutocalVoltage;
	}
	else if (m_strCalibrationMode.CompareNoCase(_T("CurrCalibration")) == 0)
	{
		m_nCalibrationModeNo = elFBoradStepModeMasterAutocalCurrent;
	}

	return m_nCalibrationModeNo;
}
void CMgrConfig::GetMoxaIoInfo(std::vector<MoxaIOInfo*>& vecValue)
{
	vecValue.clear();

	auto iter = m_vecMoxaDevice.begin();
	auto iterE = m_vecMoxaDevice.end();

	while (iter != iterE)
	{
		MoxaIOInfo* lpMoxaIOInfo = (*iter);

		if (lpMoxaIOInfo)
			vecValue.push_back(lpMoxaIOInfo);
		else
			vecValue.push_back(NULL);

		++iter;
	}
}

void CMgrConfig::SetMoxaIoInfor(int nModuleNo, std::vector<MoxaIOInfo*> vecValue)
{
	if (nModuleNo >= 0 && (int)m_vecMoxaDevice.size() > nModuleNo)
	{
		MoxaIOInfo* lpMoxaIOInfo = vecValue[nModuleNo];

		m_vecMoxaDevice[nModuleNo] = lpMoxaIOInfo;
	}
}

void CMgrConfig::SetCurrentCalibrationTargetValue(float fTargetValue)
{
	m_vecCurrentCalibrationTargetValue.push_back(fTargetValue);

}
