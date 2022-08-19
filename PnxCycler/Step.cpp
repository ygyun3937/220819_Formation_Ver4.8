#include "stdafx.h"
#include "Step.h"

#include "PnxUtil.h"
#include "MgrConfig.h"
#include "MgrCommData.h"
#include "MgrChannel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CStep::CStep()
{
	m_pPattern = new CPattern;

	m_nCycleNumber				= 0;
	m_nStepNumber				= 0;
	m_nStepType					= 0;
	m_nStepMode					= 0;
	m_nRatioType				= 0;
	m_nRatioSoc					= -1;
	m_nRatioCycle				= 0;
	m_nRatioStep				= 0;
	m_nRatioLoop				= 0;
	m_nRatioSoH					= -1;
	m_nSOHReference				= 0;
	m_nSOHLoop1					= 0;
	m_nSOHLoop2					= 0;
	m_nRatioCycleSOH			= 0;
	m_nRatioStepSOH				= 0;
	m_nRatioCycleSOH2			= 0;
	m_nRatioStepSOH2			= 0;
	m_nPrevStepIndex			= 0;
	m_nPatternPeriod			= 0;
	m_CycleLoopCount			= 0;
	m_nLoopNextCycle			= 0;
	m_nPTNTotalTime				= 0;
	m_nPTNEndTime				= 0;
	m_nCycleIndex				= 0;
	m_nEndSync					= 0;
	m_nPatternTime				= 0;
	m_nStepEndReason			= 0;
	m_nChargeCurrentLimit		= 0;
	m_nDischargeCurrentLimit	= 0;

	m_fChargeVoltage				= 0.000;
	m_fDischageVoltage				= 0.000;
	m_fChargeCurrent				= 0.000;
	m_fDisChargeCurrent				= 0.000;
	m_fPatternTemp					= 0.000;
	m_fWatt							= 0.000;
	m_fChamberSettingTemperature	= 0.000;// 0.000;// ChamberDefaultSetting;
	m_bChamberElevatedTemperature	= false;
	m_fPatternCapacity				= 0.000;
	m_nChamberDelayTime				= 0;
	m_nSyncType						= 0;
	m_strChamberDelayTime.Empty();

	m_bPattern		= FALSE;
	m_bIsEmptyValue = FALSE;

	m_strPatternFilePath.Empty();

	for (int i = 0; i < eIFBoardCompleteMaxCount; ++i)
		m_nStepEndIndex[i] = 0;

//MODIFY BY SJH 20180824 SOC START
	m_bSocTarget = FALSE;
	m_bSoHTarget = FALSE;
	m_fSocTargetValue = 0.000;	
//MODIFY BY SJH 20180824 SOC END

}

CStep::~CStep()
{
	if (m_pPattern)
	{
		delete m_pPattern;

		m_pPattern = NULL;
	}

	RemoveScheduleComplete();

	RemoveScheduleSafety();

	RemoveScheduleLog();

	RemoveDaQComplete();

	RemoveDCIRLog();
}

void	CStep::SetStepType(CString strStepType)
{
	int nStepType = eIFBoardStepTypeRest;

		 if (strStepType.CompareNoCase(_T("Rest")) == 0)		nStepType = eIFBoardStepTypeRest;
	else if (strStepType.CompareNoCase(_T("Charge")) == 0)		nStepType = eIFBoardStepTypeCharge;
	else if (strStepType.CompareNoCase(_T("Discharge")) == 0)	nStepType = eIFBoardStepTypeDischarge;
	else if (strStepType.CompareNoCase(_T("Pattern")) == 0)		nStepType = eIFBoardStepTypeExternalPattern;
	else if (strStepType.CompareNoCase(_T("OCV")) == 0)			nStepType = eIFBoardStepTypeOcv;
	else if (strStepType.CompareNoCase(_T("Impedance")) == 0)	nStepType = eIFBoardStepTypeImpedance;
	else if (strStepType.CompareNoCase(_T("ohm")) == 0)	nStepType = eIFBoardStepTypePogoPinTest;

	m_nStepType = nStepType;
}

void	CStep::SetStepPattern()
{
	m_nStepPattern = eIFBoardStepTypeExternalPattern;
}

void	CStep::SetChargeVol_limit()
{
	m_fChargeVol_limit = m_fChargeVoltage;
}

void	CStep::SetDisChargeVol_limit()
{
	m_fDisChargeVol_limit = m_fDischageVoltage;
}

void	CStep::SetChargeCurr_limit()
{
	m_fChargeCurr_limit = m_fChargeCurrent;
}

void	CStep::SetDisChargeCurr_limit()
{
	m_fDisChargeCurr_limit = m_fDisChargeCurrent;
}

void	CStep::SetStepMode(CString strStepMode)
{
	int nStepMode = eIFBoardStepModeDefault;

		 if (strStepMode.CompareNoCase(_T("CC")) == 0)		nStepMode = eIFBoardStepModeCC;
	else if (strStepMode.CompareNoCase(_T("CC/CV")) == 0)	nStepMode = eIFBoardStepModeCCCV;
	else if (strStepMode.CompareNoCase(_T("CP")) == 0)		nStepMode = eIFBoardStepModeCP;
	else if (strStepMode.CompareNoCase(_T("CPCV")) == 0)	nStepMode = eIFBoardStepModeCPCV;
	else if (strStepMode.CompareNoCase(_T("CP/CV")) == 0)	nStepMode = eIFBoardStepModeCPCV;
	else if (strStepMode.CompareNoCase(_T("10")) == 0)		nStepMode = elFBoradStepModeAutocalVoltage;
	else if (strStepMode.CompareNoCase(_T("11")) == 0)		nStepMode = elFBoradStepModeMasterAutocalCurrent;
	else if (strStepMode.CompareNoCase(_T("12")) == 0)		nStepMode = elFBoradStepModeSlaveAutocalCurrent;


	m_nStepMode = nStepMode;

	// Add By JSK - 2018.07.04 간헐적으로 PTN Editor 에서 해당 기능이 동작하지 않을 때가 있다.
	if ((eIFBoardStepTypeCharge == m_nStepType || eIFBoardStepTypeDischarge == m_nStepType || eIFBoardStepTypeImpedance == m_nStepType || eIFBoardStepTypePogoPinTest == m_nStepType) && TRUE == strStepMode.IsEmpty())
		m_nStepMode = eIFBoardStepModeCC;
}

void	CStep::SetChargeVoltage(CString strVoltage)
{	
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	//220516 YGY PogoPin Test Step Type 추가 
	if (eIFBoardStepTypeCharge == m_nStepType || eIFBoardStepTypePogoPinTest == m_nStepType)
	{
		m_fChargeVoltage = (float)atof(strVoltage);

		if (strVoltage.IsEmpty() == TRUE || m_fChargeVoltage == 0)
		{
			m_bIsEmptyValue = TRUE;
		}
 	}		
	else if (eIFBoardStepTypeRest == m_nStepType)// || StepTypePattern == m_nStepType) //taek 패턴일 때 값이 있으면 CPCV다
		m_fChargeVoltage = 0.000f;

	if (m_nStepMode == elFBoradStepModeMasterAutocalCurrent)
	{
		pMgrConfig->SetCurrentCalibrationTargetValue(m_fChargeVoltage);
	}

	if (eIFBoardStepTypeExternalPattern == m_nStepType)
	{//패턴값은 여기서 셋팅 하지 않음..그래서 스킵
		//m_bIsEmptyValue = FALSE;
		m_fChargeVoltage = (float)atof(strVoltage);

		if (strVoltage.IsEmpty() == TRUE || m_fChargeVoltage == 0)
		{
			m_bIsEmptyValue = TRUE;
		}
	}
}

void	CStep::SetCycleLoopCount(int nCount)
{
	//에디터 수정되면 바꿔야함
	if (nCount == 0)
		nCount = 1;
	//
	m_CycleLoopCount = nCount;
}

int		CStep::GetStepEndIndex(int nType)
{
	if (nType >= eIFBoardCompleteVoltage && nType < eIFBoardCompleteMaxCount)
		return m_nStepEndIndex[ nType ];

	return 0;
}

void	CStep::SetStepEndIndex(int nType, int nIndex)
{
	if (nType >= eIFBoardCompleteVoltage && nType < eIFBoardCompleteMaxCount)
		m_nStepEndIndex[ nType ] = nIndex;
}
float CStep::GetDischargeVoltage()
{
	return m_fDischageVoltage;
}
void	CStep::SetDischargeVoltage(CString strVoltage)
{
	if (eIFBoardStepTypeDischarge == m_nStepType || eIFBoardStepTypeImpedance == m_nStepType)
	{
		m_fDischageVoltage = (float)atof(strVoltage);
		
		if (strVoltage.IsEmpty() == TRUE || m_fDischageVoltage == 0)
		{
			m_bIsEmptyValue = TRUE;
		}
	}
		
	else if (eIFBoardStepTypeRest == m_nStepType)// || StepTypePattern == m_nStepType) //taek cpcv 때문에 패턴에서도 값 필요
		m_fDischageVoltage = 0.000;

	if (eIFBoardStepTypeExternalPattern == m_nStepType)
	{//패턴값은 여기서 셋팅 하지 않음..그래서 스킵
		//m_bIsEmptyValue = FALSE;
		m_fDischageVoltage = (float)atof(strVoltage);

		if (strVoltage.IsEmpty() == TRUE || m_fDischageVoltage == 0)
		{
			m_bIsEmptyValue = TRUE;
		}
	}
}

void	CStep::SetDisChargeCurrentAmpere(CString strAmpere)
{
	if (TRUE == strAmpere.IsEmpty())
	{
		m_fDisChargeCurrent = 0.000;
		m_bIsEmptyValue = TRUE;
	}
	else
	{
		m_fDisChargeCurrent = (float)atof(strAmpere);

		if (m_fDisChargeCurrent <= 0)
		{
			m_fDisChargeCurrent = 0.000;
			m_bIsEmptyValue = TRUE;
		}
	}

	if (GetStepMode() == eIFBoardStepModeCP)
	{//CP는 전류 필요 없음
		m_bIsEmptyValue = FALSE;
	}

	if (GetStepMode() == eIFBoardStepModeCPCV)
	{//CP는 전류 필요 없음
		m_bIsEmptyValue = FALSE;
	}

	if (eIFBoardStepTypeExternalPattern == m_nStepType)
	{//패턴값은 여기서 셋팅 하지 않음..그래서 스킵
		m_bIsEmptyValue = FALSE;
	}
}



void	CStep::SetChargeCurrentAmpere(CString strAmpere)
{
	if (TRUE == strAmpere.IsEmpty())
	{
		m_fChargeCurrent = 0.000;
		m_bIsEmptyValue = TRUE;
	}
	else
	{
		m_fChargeCurrent = (float)atof(strAmpere);

		if (m_fChargeCurrent <= 0)
		{
			m_fChargeCurrent = 0.000;
			m_bIsEmptyValue = TRUE;
		}
	}

	if (GetStepMode() == eIFBoardStepModeCP)
	{//CP는 전류 필요 없음
		m_bIsEmptyValue = FALSE;
	}

	if (GetStepMode() == eIFBoardStepModeCPCV)
	{//CP는 전류 필요 없음
		m_bIsEmptyValue = FALSE;
	}

	if (eIFBoardStepTypeExternalPattern == m_nStepType)
	{//패턴값은 여기서 셋팅 하지 않음..그래서 스킵
		m_bIsEmptyValue = FALSE;
	}
}

void	CStep::SetWatt(CString strWatt)
{
	if (TRUE == strWatt.IsEmpty())
		m_fWatt = 0.000;
	else
		m_fWatt = (float)atof(strWatt);

	if (GetStepMode() == eIFBoardStepModeCP && m_fWatt == 0)
	{
		m_bIsEmptyValue = TRUE;
	}
}

void	CStep::SetPatternFilePath(CString strFilePath)
{
	if (strFilePath.GetLength() <= 0)
		return;

	m_strPatternFilePath = strFilePath;

	if (FALSE == m_strPatternFilePath.IsEmpty())
		m_pPattern->LoadPatternCsv(m_strPatternFilePath);
}

float temp_his = 25;
void	CStep::SetChamberCondition(LPCTSTR lpszString)
{
#if 0	// 20201117 챔버 온도가 0 이라도 초기값 적용 하기로 협의함.
	if (strcmp(lpszString, "") == 0)
	{
		m_fChamberSettingTemperature = ChamberDefaultSetting;
		return;
	}
#endif
	CStringArray arrCondition;
	CPnxUtil::GetMgr()->SplitString(lpszString, '=', arrCondition);

	// Xml 순서 상, Step 의 종료 조건이 우선, Chamber 종료 조건이 이후이다.
	SScheduleComplete* lpScheduleComplete = new SScheduleComplete;

	// 같은 종료조건에 포함시키기 위해 강제로 Name 을 지정 해준다.
	lpScheduleComplete->strName = _T("Chamber");

	for (int i = 0; i < arrCondition.GetSize(); ++i)
	{
		CString str = arrCondition.GetAt(i);

		switch (i)
		{
			case 0: 
				if (str == "")
				{
					m_fChamberSettingTemperature = temp_his;
				}
				else
				{
					m_fChamberSettingTemperature = (float)atof(str);
					temp_his = m_fChamberSettingTemperature;
				}
				break;	// 설정 온도
			case 1:	m_nChamberDelayTime			 = (int)atoi(str);		break;	// 확인 시간  hsm add
			case 2:																// 온도 차이
			case 3:																// 패턴 번호
			case 4:																// 챔버동작 후 충방전 대기
			case 5: lpScheduleComplete->fValue = (float)atof(str);		break;	// 종료 온도
			case 6:																// Cycle, Step
			{
				// 0:65533 분리
				int nSepIndex = str.Find(_T(":"));

				if (nSepIndex >= 0)
				{
					lpScheduleComplete->nCycle	= atoi(str.Left(nSepIndex));
					lpScheduleComplete->nStep		= atoi(str.Mid(nSepIndex + 1));
				}

				break;
			}
			case 7: m_bChamberElevatedTemperature = (strcmp(str, "1") == 0) ? true : false;//m_nChamberElevatedTemperature = (int)atoi(str);		break;
			case 8: m_strChamberDelayTime = str;							break; //taek 챔버 종료 시간
		}
	}

	m_vecScheduleComplete.push_back(lpScheduleComplete);
}

void	CStep::SetRatioCondition(LPCTSTR lpszString)
{
	m_nRatioSoc = -1;
	m_nRatioLoop = 0;

	if (strcmp(lpszString, "") == 0)
		return;

	CStringArray arrCondition;
	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	for (int i = 0; i < arrCondition.GetSize(); ++i)
	{
		CString str = arrCondition.GetAt(i);

		switch (i)
		{
			case 0:	// Parameter
			{
				m_nRatioType = atoi(str); //taek ah인지 wh 인지 구분
				SetRatioType(m_nRatioType);
			}
			case 1:	// Cycle Step
			{
				// 1:2 분리
				int nSepIndex = str.Find(_T(":"));

				if (nSepIndex >= 0)
				{
					m_nRatioCycle	= atoi(str.Left(nSepIndex));
					m_nRatioStep	= atoi(str.Mid(nSepIndex + 1));
				}

				break;
			}
			case 2: m_nRatioSoc = atoi(str); break;	// SOC Value
			case 3: // Cycle Step
			{
				SScheduleComplete* lpScheduleComplete = new SScheduleComplete;

				lpScheduleComplete->strName = _T("RatioSoc");

				// 0:65533 분리
				int nSepIndex = str.Find(_T(":"));

				if (nSepIndex >= 0)
				{
					lpScheduleComplete->nCycle = atoi(str.Left(nSepIndex));
					lpScheduleComplete->nStep = atoi(str.Mid(nSepIndex + 1));
				}

				m_vecScheduleComplete.push_back(lpScheduleComplete);

				break;
			}
			case 4: m_nRatioLoop = atoi(str); break;
		}
	}
}

void	CStep::SetRatioConditionSOH(LPCTSTR lpszString)
{
	m_nRatioSoH = -1;
	m_nSOHReference = 0;
	m_nSOHLoop1 = 0;
	m_nSOHLoop2 = 0;

	if (strcmp(lpszString, "") == 0)
		return;

	CStringArray arrCondition;
	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	for (int i = 0; i < arrCondition.GetSize(); ++i)
	{
		CString str = arrCondition.GetAt(i);

		switch (i)
		{
			case 0:	break;// Parameter
			case 1:	// Cycle Step
			{
				// 1:2 분리
				int nSepIndex = str.Find(_T(":"));

				if (nSepIndex >= 0)
				{
					m_nRatioCycleSOH = atoi(str.Left(nSepIndex));
					m_nRatioStepSOH = atoi(str.Mid(nSepIndex + 1));
				}

				break;
			}
			case 2:
			{
				// 1:2 분리
				int nSepIndex = str.Find(_T(":"));

				if (nSepIndex >= 0)
				{
					m_nRatioCycleSOH2 = atoi(str.Left(nSepIndex));
					m_nRatioStepSOH2 = atoi(str.Mid(nSepIndex + 1));
				}

				break;
			}
			case 3: m_nRatioSoH = atoi(str); break;	// SOC Value
			case 4: // Cycle Step
			{
				SScheduleComplete* lpScheduleComplete = new SScheduleComplete;

				lpScheduleComplete->strName = _T("RatioSoH");

				// 0:65533 분리
				int nSepIndex = str.Find(_T(":"));

				if (nSepIndex >= 0)
				{
					lpScheduleComplete->nCycle = atoi(str.Left(nSepIndex));
					lpScheduleComplete->nStep = atoi(str.Mid(nSepIndex + 1));
				}

				m_vecScheduleComplete.push_back(lpScheduleComplete);

				break;
			}
			case 5: m_nSOHReference = atoi(str); break;
			case 6: m_nSOHLoop1 = atoi(str); break;
			case 7: m_nSOHLoop2 = atoi(str); break;
		}
	}
}

void	CStep::SetPatternCapacity(LPCTSTR lpszString)
{
	if (strcmp(lpszString, "") == 0)
		return;

	CStringArray arrCondition;
	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	SScheduleComplete* lpScheduleComplete = new SScheduleComplete;
	lpScheduleComplete->ParseCapacity(arrCondition);

	m_vecScheduleComplete.push_back(lpScheduleComplete);
}

void	CStep::SetFinishCondition(LPCTSTR lpszString)
{
	RemoveScheduleComplete();

	CStringArray arrCondition;
	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	for (int i = 0; i < arrCondition.GetSize(); ++i)
	{
		CStringArray arr;
		CPnxUtil::GetMgr()->SplitString(arrCondition.GetAt(i), '=', arr);

		SScheduleComplete* lpScheduleComplete = new SScheduleComplete;
		lpScheduleComplete->ParseComplete(arr);

		m_vecScheduleComplete.push_back(lpScheduleComplete);
	}
}

void	CStep::SetSafetyCondition( LPCTSTR lpszString )
{
	RemoveScheduleSafety();

	CStringArray arrCondition;
	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	for (int i = 0; i < arrCondition.GetSize(); ++i)
	{
		CStringArray arr;
		CPnxUtil::GetMgr()->SplitString(arrCondition.GetAt(i), '=', arr);

		SScheduleSafety* lpScheduleSafety = new SScheduleSafety;
		lpScheduleSafety->Parse(arr);

		m_vecScheduleSafety.push_back(lpScheduleSafety);		
	}
}

void	CStep::SetLogRecodeTerm( LPCTSTR lpszString )
{
	RemoveScheduleLog();

	CStringArray arrCondition;
	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	for (int i = 0; i < arrCondition.GetSize(); ++i)
	{
		CStringArray arr;
		CPnxUtil::GetMgr()->SplitString(arrCondition.GetAt(i), '=', arr);

		SScheduleLog* lpScheduleLog = new SScheduleLog;
		lpScheduleLog->Parse(arr);

		m_vecScheduleLog.push_back(lpScheduleLog);
	}
}

void	CStep::SetDCIR(LPCTSTR lpszString)
{
	if (strcmp(lpszString, "") == 0)
		return;
	
	RemoveDCIRLog();

	CStringArray arrCondition;
	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	SDCIRLog* lpDCIRLog = new SDCIRLog;
	lpDCIRLog->Parse(arrCondition);

	m_vecDCIRLog.push_back(lpDCIRLog);
}

void	CStep::SetDAQCondition(LPCTSTR lpszString)
{
	RemoveDaQComplete();

	CStringArray arrCondition;
	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	for (int i = 0; i < arrCondition.GetSize(); ++i)
	{
		CStringArray arr;
		CPnxUtil::GetMgr()->SplitString(arrCondition.GetAt(i), ':', arr);

		SDaQComplete* lpDaQComplete = new SDaQComplete;
		lpDaQComplete->Parse(arr);

		m_vecDaQComplete.push_back(lpDaQComplete);
	}
}

SScheduleComplete*	CStep::GetScheduleCompleteFromName(CString strName)
{
	auto iter = m_vecScheduleComplete.begin();
	auto iterE = m_vecScheduleComplete.end();

	while (iter != iterE)
	{
		SScheduleComplete* lpScheduleComplete = (*iter);

		if (lpScheduleComplete)
		{
			if (strcmp(lpScheduleComplete->strName, strName) == 0)
				return lpScheduleComplete;
		}

		++iter;
	}

	return nullptr;
}
SScheduleSafety*	CStep::GetScheduleSafetyFromName(CString strName)
{
	auto iter = m_vecScheduleSafety.begin();
	auto iterE = m_vecScheduleSafety.end();

	while (iter != iterE)
	{
		SScheduleSafety* lpScheduleSafety = (*iter);

		if (lpScheduleSafety)
		{
			if (strcmp(lpScheduleSafety->strName, strName) == 0)
				return lpScheduleSafety;
		}

		++iter;
	}

	return nullptr;
}
SScheduleLog*	CStep::GetScheduleLogFromName(CString strName)
{
	auto iter = m_vecScheduleLog.begin();
	auto iterE = m_vecScheduleLog.end();

	while (iter != iterE)
	{
		SScheduleLog* lpScheduleLog = (*iter);

		if (lpScheduleLog)
		{
			if (strcmp(lpScheduleLog->strName, strName) == 0)
				return lpScheduleLog;
		}

		++iter;
	}

	return nullptr;
}

CPattern*	CStep::GetPattern()
{
	return m_pPattern;
}

void	CStep::RemoveScheduleComplete()
{
	auto iter = m_vecScheduleComplete.begin();
	auto iterE = m_vecScheduleComplete.end();

	while (iter != iterE)
	{
		SScheduleComplete* lpValue = (*iter);

		if (lpValue)
		{
			delete lpValue;
			lpValue = nullptr;
		}

		++iter;
	}

	m_vecScheduleComplete.clear();
}

void	CStep::RemoveScheduleSafety()
{
	auto iter = m_vecScheduleSafety.begin();
	auto iterE = m_vecScheduleSafety.end();

	while (iter != iterE)
	{
		SScheduleSafety* lpValue = (*iter);

		if (lpValue)
		{
			delete lpValue;
			lpValue = nullptr;
		}

		++iter;
	}

	m_vecScheduleSafety.clear();
}

void	CStep::RemoveScheduleLog()
{
	auto iter = m_vecScheduleLog.begin();
	auto iterE = m_vecScheduleLog.end();

	while (iter != iterE)
	{
		SScheduleLog* lpValue = (*iter);

		if (lpValue)
		{
			delete lpValue;
			lpValue = nullptr;
		}

		++iter;
	}

	m_vecScheduleLog.clear();
}

void	CStep::RemoveDaQComplete()
{
	auto iter = m_vecDaQComplete.begin();
	auto iterE = m_vecDaQComplete.end();

	while (iter != iterE)
	{
		SDaQComplete* lpValue = (*iter);

		if (lpValue)
		{
			delete lpValue;
			lpValue = nullptr;
		}

		++iter;
	}

	m_vecDaQComplete.clear();
}

void	CStep::RemoveDCIRLog()
{
	auto iter = m_vecDCIRLog.begin();
	auto iterE = m_vecDCIRLog.end();

	while (iter != iterE)
	{
		SDCIRLog* lpValue = (*iter);

		if (lpValue)
		{
			delete lpValue;
			lpValue = nullptr;
		}

		++iter;
	}

	m_vecDCIRLog.clear();
}

BOOL	CStep::CopyData(CStep* pSrc, int nIndex /*= -1*/)
{
	BOOL bExistTime = FALSE;

	this->m_nCycleNumber				= pSrc->m_nCycleNumber;
	this->m_nStepNumber					= pSrc->m_nStepNumber;
	this->m_nCycleIndex					= pSrc->m_nCycleIndex;
	this->m_nStepType					= pSrc->m_nStepType;
	this->m_nStepMode					= pSrc->m_nStepMode;
	this->m_nRatioType					= pSrc->m_nRatioType;
	this->m_nRatioSoc					= pSrc->m_nRatioSoc;
	this->m_nRatioSoH					= pSrc->m_nRatioSoH;
	this->m_nSOHReference				= pSrc->m_nSOHReference;
	this->m_nRatioCycle					= pSrc->m_nRatioCycle;
	this->m_nRatioStep					= pSrc->m_nRatioStep;
	this->m_nRatioLoop					= pSrc->m_nRatioLoop;
	this->m_nRatioCycleSOH2				= pSrc->m_nRatioCycleSOH2;
	this->m_nRatioStepSOH2				= pSrc->m_nRatioStepSOH2;
	this->m_nRatioCycleSOH				= pSrc->m_nRatioCycleSOH;
	this->m_nRatioStepSOH				= pSrc->m_nRatioStepSOH;
	this->m_nSOHLoop1					= pSrc->m_nSOHLoop1;
	this->m_nSOHLoop2					= pSrc->m_nSOHLoop2;
	this->m_fChargeVoltage				= pSrc->m_fChargeVoltage;
	this->m_fDischageVoltage			= pSrc->m_fDischageVoltage;
	this->m_fChargeCurrent				= pSrc->m_fChargeCurrent;
	this->m_fDisChargeCurrent			= pSrc->m_fDisChargeCurrent;
	this->m_fPatternTemp				= pSrc->m_fPatternTemp;
	this->m_nPatternTime				= pSrc->m_nPatternTime;
	this->m_fWatt						= pSrc->m_fWatt;
	this->m_bChamberElevatedTemperature = pSrc->m_bChamberElevatedTemperature;
	this->m_fChamberSettingTemperature	= pSrc->m_fChamberSettingTemperature;
	this->m_nChamberDelayTime			= pSrc->m_nChamberDelayTime;
	this->m_strChamberDelayTime			= pSrc->m_strChamberDelayTime;
	this->m_nSyncType					= pSrc->m_nSyncType;
	this->m_fPatternCapacity			= pSrc->m_fPatternCapacity;
	this->m_bPattern					= pSrc->m_bPattern;
	this->m_bIsEmptyValue				= pSrc->m_bIsEmptyValue;
	this->m_nEndSync					= pSrc->m_nEndSync;
	this->m_fChargeVol_limit			= pSrc->m_fChargeVol_limit;
	this->m_fDisChargeVol_limit			= pSrc->m_fDisChargeVol_limit;
	this->m_nChargeCurrentLimit			= pSrc->m_nChargeCurrentLimit;
	this->m_nDischargeCurrentLimit		= pSrc->m_nDischargeCurrentLimit;
	this->m_nStepPattern				= pSrc->m_nStepPattern;

	std::vector<SScheduleComplete*>& vecScheduleComplete = pSrc->GetScheduleComplete();
	{
		auto iter = vecScheduleComplete.begin();
		auto iterE = vecScheduleComplete.end();

		while (iter != iterE)
		{
			SScheduleComplete* lpScheduleComplete = (*iter);

			if (lpScheduleComplete)
			{
				SScheduleComplete* lpNewElement = new SScheduleComplete;
				lpNewElement->CopyData(lpScheduleComplete);

				if (nIndex >= 0 && strcmp(lpNewElement->strName, "Ti") == 0)
				{
					if (CPnxUtil::GetMgr()->GetSplitTime(lpNewElement->strValue, ':') > 0)
					{
						TPattern* lpExternalStep = pSrc->m_pPattern->GetAt(nIndex);

						if (lpExternalStep)
						{
							lpNewElement->strValue.Format("00:00:00:00:%d00", lpExternalStep->nTimeGap);

							this->m_nPTNTotalTime = lpExternalStep->nTimeGap;

							bExistTime = TRUE;
						}
					}
					else
					{
						TPattern* lpExternalStep = pSrc->m_pPattern->GetAt(nIndex);

						if (lpExternalStep)
						{
							lpNewElement->strValue.Format("00:00:00:00:%d00", lpExternalStep->nTimeGap);

							this->m_nPTNTotalTime = lpExternalStep->nTimeGap;
						}
					}
				}

				m_vecScheduleComplete.push_back(lpNewElement);
			}

			++iter;
		}
	}	

	std::vector<SScheduleSafety*>& vecScheduleSafety = pSrc->GetScheduleSafety();
	{
		auto iter = vecScheduleSafety.begin();
		auto iterE = vecScheduleSafety.end();

		while (iter != iterE)
		{
			SScheduleSafety* lpScheduleSafety = (*iter);

			if (lpScheduleSafety)
			{
				SScheduleSafety* lpNewElement = new SScheduleSafety;
				lpNewElement->CopyData(lpScheduleSafety);

				m_vecScheduleSafety.push_back(lpNewElement);

				++iter;
			}
		}
	}

	std::vector<SScheduleLog*>& vecScheduleLog = pSrc->GetScheduleLog();
	{
		auto iter = vecScheduleLog.begin();
		auto iterE = vecScheduleLog.end();

		while (iter != iterE)
		{
			SScheduleLog* lpScheduleLog = (*iter);

			if (lpScheduleLog)
			{
				SScheduleLog* lpNewElement = new SScheduleLog;
				lpNewElement->CopyData(lpScheduleLog);

				m_vecScheduleLog.push_back(lpNewElement);

				++iter;
			}
		}
	}
	std::vector<SDaQComplete*>& vecDaQComplete = pSrc->GetDaQComplete();
	{
		auto iter = vecDaQComplete.begin();
		auto iterE = vecDaQComplete.end();

		while (iter != iterE)
		{
			SDaQComplete* lpDaQComplete = (*iter);

			if (lpDaQComplete)
			{
				SDaQComplete* lpNewElement = new SDaQComplete;
				lpNewElement->CopyData(lpDaQComplete);

				m_vecDaQComplete.push_back(lpNewElement);

				++iter;
			}
		}
	}
	std::vector<SDCIRLog*>& vecDCIRLog = pSrc->GetDCIRLog();
	{
		auto iter = vecDCIRLog.begin();
		auto iterE = vecDCIRLog.end();

		while (iter != iterE)
		{
			SDCIRLog* lpDCIRLog = (*iter);

			if (lpDCIRLog)
			{
				SDCIRLog* lpNewElement = new SDCIRLog;
				lpNewElement->CopyData(lpDCIRLog);

				m_vecDCIRLog.push_back(lpNewElement);

				++iter;
			}
		}
	}

	return bExistTime;
}

CString	CStep::StepType2Str()
{
	switch (m_nStepType)
	{
		case eIFBoardStepTypeRest			: return _T("Rest");
		case eIFBoardStepTypeCharge			: return _T("Charge");
		case eIFBoardStepTypeDischarge		: return _T("Discharge");
		case eIFBoardStepTypeExternalPattern: return _T("Pattern");
		case eIFBoardStepTypeOcv			: return _T("OCV");
		case eIFBoardStepTypeImpedance		: return _T("Impedance");
		case eIFBoardStepTypePogoPinTest: return _T("ohm");
	}

	return _T("*");
}

CString CStep::StepType2String()
{
	switch (m_nStepType)
	{
	case eIFBoardStepTypeRest			: return _T("휴지");
	case eIFBoardStepTypeCharge			: return _T("충전");
	case eIFBoardStepTypeDischarge		: return _T("방전");
	case eIFBoardStepTypeExternalPattern: return _T("패턴");
	}

	return _T("");
}

CString CStep::StepEndReason2String()
{
	if(m_nStepEndReason < 0)
		return _T("");

	if(m_nStepEndReason >= eIFBoardCyclerCodeEmpty && m_nStepEndReason < eIFBoardCyclerCodeMaxCount)
		return g_lpszIFBoardCode[m_nStepEndReason];
	else if(m_nStepEndReason >= eMonitroingCyclerCodeSystemSafetyVoltageMax && m_nStepEndReason < eMonitroingCyclerCodeMaxCount)
		return g_lpszMonitoringCode[m_nStepEndReason - eMonitroingCyclerCodeSystemSafetyVoltageMax];

	return _T("");
}

//lyw3_1
CString CStep::StepEndReason2String(int nEndReason)
{
	return g_lpszMonitoringCode[nEndReason - eMonitroingCyclerCodeSystemSafetyVoltageMax];
}

CString CStep::StepMode2Str()
{
	switch (m_nStepMode)
	{
		case eIFBoardStepModeCC		: return _T("CC");
		case eIFBoardStepModeCCCV	: return _T("CC/CV");
		case eIFBoardStepModeCP		: return _T("CP");
		case eIFBoardStepModeCPCV	: return _T("CP/CV");
	}

	return _T("");
}

int		CStep::GetOperationMode()
{
	if (eIFBoardStepTypeCharge == m_nStepType)
	{
			 if (eIFBoardStepModeCC == m_nStepMode)		return eIFBoardOperationChargeCC;
		else if (eIFBoardStepModeCCCV == m_nStepMode)	return eIFBoardOperationChargeCCCV;
		else if (eIFBoardStepModeCP == m_nStepMode)		return eIFBoardOperationChargeCP;
		else if (eIFBoardStepModeCPCV == m_nStepMode)	return eIFBoardOperationChargeCPCV;
	}
	else if (eIFBoardStepTypeDischarge == m_nStepType)
	{
		if (eIFBoardStepModeCC == m_nStepMode)		return eIFBoardOperationDischargeCC;
		else if (eIFBoardStepModeCCCV == m_nStepMode)	return eIFBoardOperationDischargeCCCV;
		else if (eIFBoardStepModeCP == m_nStepMode)		return eIFBoardOperationDischargeCP;
		else if (eIFBoardStepModeCPCV == m_nStepMode)	return eIFBoardOperationDischargeCPCV;
	}
	else if (eIFBoardStepTypeImpedance == m_nStepType)
	{
		return eIFBoardOperationDischargeCC;
	}
	else if (eIFBoardStepTypePogoPinTest == m_nStepType)
	{
		return eIFBoardOperationChargeCC;
	}

	return eIFBoardOperationNone;
}

void	CStep::GetStepInfo(std::vector<CString>& vecValue)
{
	vecValue.clear();

	{
		// 1. 스텝 번호
		CString strValue;
		strValue.Format("%d", m_nStepIndex);
		
		vecValue.push_back(strValue);
	}

	{
		// 2. 스텝 타입
		CString strValue;
		strValue.Format("%s", StepType2Str());

		vecValue.push_back(strValue);
	}

	{
		// 3. 충전 전압
		CString strValue;
		strValue.Format("%.3f", m_fChargeVoltage);

		vecValue.push_back(strValue);
	}

	{
		// 4. 방전 전압
		CString strValue;
		strValue.Format("%.3f", m_fDischageVoltage);

		vecValue.push_back(strValue);
	}

	{
		// 5. 충전 전류
		CString strValue;
		if(eIFBoardStepTypeCharge == m_nStepType|| eIFBoardStepTypePogoPinTest == m_nStepType)
			strValue.Format("%.3f", m_fChargeCurrent);

		vecValue.push_back(strValue);
	}

	{
		// 6. 방전 전류
		CString strValue;
		if (eIFBoardStepTypeDischarge == m_nStepType)
			strValue.Format("%.3f", m_fDisChargeCurrent);

		vecValue.push_back(strValue);
	}

	{
		// 7. 파워
		CString strValue;
		strValue.Format("%.3f", m_fWatt);

		vecValue.push_back(strValue);
	}

	{
		// 8. 저항
		CString strValue;
		strValue.Format("%s", "");

		vecValue.push_back(strValue);
	}

	{
		// 9. 온도
		CString strValue;
		strValue.Format("%.3f", m_fChamberSettingTemperature);

		vecValue.push_back(strValue);
	}
}

void	CStep::GetCompleteInfo(std::vector<CString>& vecValue)
{
	vecValue.clear();

	LPCTSTR lpszValue[] = { "HV", "CUR", "Ca", "Ti", "", "Po", "WHMax", "", nullptr };

	for (int i = 0; ; ++i)
	{
		if(nullptr == lpszValue[i])
			break;

		SScheduleComplete* lpScheduleComplete = GetScheduleCompleteFromName(lpszValue[i]);

		if (lpScheduleComplete)
		{
			CString strValue;

			if (strcmp(lpszValue[i], "Ti") == 0)
				strValue.Format("%s", lpScheduleComplete->strValue);
			else
				strValue.Format("%.3f", lpScheduleComplete->fValue);

			vecValue.push_back(strValue);
		}
		else
		{
			vecValue.push_back("");
		}
	}
}

CString CStep::StepCompleteCondition2String()
{
	std::vector<CString> vecValue;

	LPCTSTR lpszCompleteName[] = { "HV", "LV", "CUR", "Ti", "Tah", "Ca", "WH", "Po", "RatioSoc", "RatocSoh", "Chamber", nullptr };

	for (int i = 0; ; ++i)
	{
		if (nullptr == lpszCompleteName[i])
			break;

		SScheduleComplete* lpScheduleComplete = GetScheduleCompleteFromName(lpszCompleteName[i]);

		if (lpScheduleComplete)
		{
			CString strCompleteValue;

			if (strcmp(lpszCompleteName[i], "Ti") == 0)
			{
				if (lpScheduleComplete->strValue.Compare("00:00:00:00") != 0)
					strCompleteValue.Format("시간 >= %s", lpScheduleComplete->strValue);
			}
			else if (strcmp(lpszCompleteName[i], "RatioSoc") == 0)
			{
				strCompleteValue.Format("SOC > %d%s", GetRatioSoc(), "%");
			}
			else if (strcmp(lpszCompleteName[i], "RatioSoh") == 0)
			{
				strCompleteValue.Format("SOH > %d%s", GetRatioSoH(), "%");
			}
			else if (lpScheduleComplete->fValue != 0.f)
			{
				if (strcmp(lpszCompleteName[i], "HV") == 0)
					strCompleteValue.Format("Voltage > %.3f", lpScheduleComplete->fValue);
				else if (strcmp(lpszCompleteName[i], "LV") == 0)
					strCompleteValue.Format("Voltage < %.3f", lpScheduleComplete->fValue);
				else if (strcmp(lpszCompleteName[i], "CUR") == 0)
					strCompleteValue.Format("Current > %.3f", lpScheduleComplete->fValue);
				else if (strcmp(lpszCompleteName[i], "Tah") == 0)
					strCompleteValue.Format("Cumulative Capacity > %.3f", lpScheduleComplete->fValue);
				else if (strcmp(lpszCompleteName[i], "Ca") == 0)
					strCompleteValue.Format("Capacity > %.3f", lpScheduleComplete->fValue);
				else if (strcmp(lpszCompleteName[i], "WH") == 0)
					strCompleteValue.Format("Wattage > %.3f", lpScheduleComplete->fValue);
				else if (strcmp(lpszCompleteName[i], "Po") == 0)
					strCompleteValue.Format("Power > %.3f", lpScheduleComplete->fValue);
				else if (strcmp(lpszCompleteName[i], "Chamber") == 0)
					strCompleteValue.Format("Chamber Temp == %.1f", GetChamberSetTemp());
			}

			if (FALSE == strCompleteValue.IsEmpty())
				vecValue.push_back(strCompleteValue);
		}
	}

	CString strValue;

	auto iter	= vecValue.begin();
	auto iterE	= vecValue.end();

	while (iter != iterE)
	{
		strValue += (*iter);
		strValue += ",";

		++iter;
	}

	strValue.TrimRight(",");

	return strValue;
}


CString	CStep::GetTitleToValue(CString strTitle, int nCurChannel)
{
	CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(nCurChannel);
	if (pCommData == NULL)
		return _T("");

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCurChannel);
	if (pChannel == NULL)
		return _T("");

	CList< CCycle*, CCycle* >& listCycle = pChannel->GetMgr()->GetCycleList();
	if (listCycle.GetCount() == 0)
		return _T("");

	auto pos = listCycle.FindIndex(GetCycleNumber() - 1/*가져오는 사이클 넘버는 1base 이다..*/);
	if (pos == NULL)
		return _T("");

	CCycle* pCycle = listCycle.GetAt(pos);
	if (pCycle == NULL)
		return _T("");

	const int nSetRoop = pCycle->GetLoopCount();
	const float fCurVoltage = pCommData->GetVoltage();
	const float fCurAmpere = pCommData->GetAmpere();

	int nCyclerChannelState = eCyclerChannelRunStateUnknown;
	bool bRunStep = false;
	if (GetStepIndex() == pCommData->GetStepIndex())
	{
		nCyclerChannelState = eCyclerChannelRunStateRun;
		bRunStep = true;
	}
	else if (GetStepIndex() > pCommData->GetStepIndex())
		nCyclerChannelState = eCyclerChannelRunStateComplete;
	else if (GetStepIndex() > pCommData->GetStepIndex())
		nCyclerChannelState = eCyclerChannelRunStateWait;

	CString str = "";
	// 8. Current Voltage
	if (strTitle.CompareNoCase(_T("Curr Volt(V)")) == 0)
	{
		str.Empty();
		if (bRunStep)
			str.Format(_T("%.3f"), fCurVoltage);
		return str;
	}

	// 9. Current Ampere
	else if (strTitle.CompareNoCase(_T("Curr Ampare(A)")) == 0)
	{
		str.Empty();
		if (bRunStep)
			str.Format(_T("%.3f"), fCurAmpere);
		return str;
	}
	// 10. IsRun
	else if (strTitle.CompareNoCase(_T("Schedule State")) == 0)
	{
		if (pChannel->GetPause() == TRUE)
		{
			str.Empty();
			str.Format(_T("Pause[ LOOP : %d / %d ]"), pChannel->GetCycleIndex() + 1, nSetRoop);
			return str;
		}
		else
		{
			str.Empty();
			if (bRunStep)
				str.Format(_T("▶ [ LOOP : %d / %d ]"), pChannel->GetCycleIndex() + 1, nSetRoop);
			return str;
		}
	}

	return str;
}

void	CStep::Condition2Array(__out CStringArray& arr)
{
	// 해당 GUI 에서 표현 해야 하는 Data 는 Cycle 정보 + Step 정보이다.

	CMgrConfig* pMgr = CMgrConfig::GetMgr();
	if (pMgr == NULL)
		return;
	CString str;

	auto iter = m_vecScheduleComplete.begin();
	auto iterE = m_vecScheduleComplete.end();

	while (iter != iterE)
	{
		SScheduleComplete* lpValue = (*iter);

		if (lpValue)
		{
			if (lpValue->fValue != 0)
			{
				arr.Add(_T(lpValue->strName));
				str.Format(_T("%.3f"), lpValue->fValue);
				arr.Add(str);
				arr.Add(_T("END"));
			}
		}
		++iter;
	}
}


void	CStep::Step2Array(__out CStringArray& arr, int nCurChannel, CStringArray& arrRef)
{
	/* GUI 에 뿌려야줘야 하는 Data 들을 Array 형태로 담아버리자 초안
	MgrViewPattern.ini 참고
	cycle, step, type, mode, mode, set volt, set ampere, cur volt, cur ampere, state
	*/

	CCommData* pCommData = CMgrCommData::GetMgr()->GetAt(nCurChannel);
	if (pCommData == NULL)
		return;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nCurChannel);
	if (pChannel == NULL)
		return;

	CList< CCycle*, CCycle* >& listCycle = pChannel->GetMgr()->GetCycleList();
	if (listCycle.GetCount() == 0)
		return;

	auto pos = listCycle.FindIndex(GetCycleNumber() - 1/*가져오는 사이클 넘버는 1base 이다..*/);
	if (pos == NULL)
		return;

	CCycle* pCycle = listCycle.GetAt(pos);
	if (pCycle == NULL)
		return;

	const int nSetRoop = pCycle->GetLoopCount();
	const float fCurVoltage = pCommData->GetVoltage();
	const float fCurAmpere = pCommData->GetAmpere();

	int nCyclerChannelState = eCyclerChannelRunStateUnknown;
	bool bRunStep = false;
	if (GetStepIndex() == pCommData->GetStepIndex())
	{
		nCyclerChannelState = eCyclerChannelRunStateRun;
		bRunStep = true;
	}
	else if (GetStepIndex() > pCommData->GetStepIndex())
		nCyclerChannelState = eCyclerChannelRunStateComplete;
	else if (GetStepIndex() > pCommData->GetStepIndex())
		nCyclerChannelState = eCyclerChannelRunStateWait;

	CString str;

	for (int i = 0; i < arrRef.GetSize(); ++i)
	{
		// 1. Cycle
		if (arrRef.GetAt(i).CompareNoCase(_T("Cycle")) == 0)
		{
			if (GetStepNumber() == 1/*1번째 스탭만 뿌려줍시다 */)
			{
				str.Empty();
				str.Format(_T("%d"), GetCycleNumber());
				arr.Add(str.GetString());
			}
			else
			{
				arr.Add(_T(""));
			}
		}
		// 2. Step
		else if (arrRef.GetAt(i).CompareNoCase(_T("Step")) == 0)
		{
			str.Empty();
			str.Format(_T("%d"), GetStepNumber());
			arr.Add(str.GetString());
		}
		// 3. Step Type
		else if (arrRef.GetAt(i).CompareNoCase(_T("Type")) == 0)
		{
			arr.Add(StepType2Str());
		}
		// 4. Step Mode
		else if (arrRef.GetAt(i).CompareNoCase(_T("Mode")) == 0)
		{
			arr.Add(StepMode2Str());
		}
		// 5. Voltage
		else if (arrRef.GetAt(i).CompareNoCase(_T("Set(V)")) == 0)
		{
			str.Empty();
			if (GetStepType() == eIFBoardStepTypeCharge)
			{
				str.Format(_T("%.3f"), m_fChargeVoltage);
			}
			else if (GetStepType() == eIFBoardStepTypeDischarge)
			{
				str.Format(_T("%.3f"), m_fDischageVoltage);
			}
			else
			{
				str.Format(_T("0.000"));
			}
			arr.Add(str.GetString());
		}
		// 6. Ampere
		else if (arrRef.GetAt(i).CompareNoCase(_T("Set(A)")) == 0)
		{
			if (GetStepType() == eIFBoardStepTypeCharge)
			{
				str.Format(_T("%.3f"), m_fChargeCurrent);
			}
			else if (GetStepType() == eIFBoardStepTypeDischarge)
			{
				str.Format(_T("%.3f"), m_fDisChargeCurrent);
			}
			else
			{
				str.Format(_T("0.000"));
			}
			arr.Add(str.GetString());

		}
		// 7. Watt
		else if (arrRef.GetAt(i).CompareNoCase(_T("Set(kW)")) == 0 || arrRef.GetAt(i).CompareNoCase(_T("Set(W)")) == 0)
		{
			str.Empty();
			float fWatt = (m_nStepMode == eIFBoardStepModeCP ? m_fWatt : 0.000f);
			if (fWatt > 0.000)
				str.Format(_T("%.3f"), fWatt);
			else
				str.Format(_T("0.000"));
			arr.Add(str.GetString());
		}
		// 8. Current Voltage
		if (arrRef.GetAt(i).CompareNoCase(_T("Curr Volt(V)")) == 0)
		{
			str.Empty();
			if (bRunStep)
				str.Format(_T("%.3f"), fCurVoltage);
			arr.Add(str.GetString());
		}

		// 9. Current Ampere
		else if (arrRef.GetAt(i).CompareNoCase(_T("Curr Ampare(A)")) == 0)
		{
			str.Empty();
			if (bRunStep)
				str.Format(_T("%.3f"), fCurAmpere);
			arr.Add(str.GetString());
		}
		// 10. IsRun
		else if (arrRef.GetAt(i).CompareNoCase(_T("Schedule State")) == 0)
		{
			if (pChannel->GetPause() == TRUE)
			{
				str.Empty();
				str.Format(_T("일시정지[ LOOP : %d / %d ]"), pChannel->GetCycleIndex() + 1, nSetRoop);
				arr.Add(str.GetString());
			}
			else
			{
				str.Empty();
				if (bRunStep)
					str.Format(_T("▶ [ LOOP : %d / %d ]"), pChannel->GetCycleIndex() + 1, nSetRoop);
				arr.Add(str.GetString());
			}
		}
	}
}
