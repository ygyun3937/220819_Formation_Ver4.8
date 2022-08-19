#include "stdafx.h"
#include "MgrSchedule.h"
#include "Channel.h"
#include "PnxUtil.h"
#include "MgrConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMgrSchedule::CMgrSchedule()
{
	m_nStepIndex	= 1;
	m_nLastIndex	= 1;

	m_stSVDF = new	stSVDF();
	m_queueSvdfCircula.CreateQueue(3);
	m_queueCircula.CreateQueue(2);


	
}


CMgrSchedule::~CMgrSchedule()
{
	ClearSchedule();

	delete m_stSVDF;
	m_stSVDF = NULL;

	m_queueSvdfCircula.DestroyQueue();
	m_queueCircula.DestroyQueue();
}
int	CMgrSchedule::getScheduleStepCountXml(LPCTSTR lpszFilePath , int Cycle)
{
	int retCount=0;
	TiXmlDocument XmlDoc;

	if (false == XmlDoc.LoadFile(lpszFilePath))
		return false;
	TiXmlNode* pXmlSchdule = XmlDoc.FirstChild(_T("SCHEDULE"));

	TiXmlNode* pXmlCycleList = pXmlSchdule->FirstChild(_T("CYCLE_LIST"));
	if (nullptr == pXmlCycleList)
		return false;

	TiXmlNode* pXmlCycle = pXmlCycleList->FirstChild(_T("CYCLE"));
	if (pXmlCycle == NULL)
		return false;

	CString sCycleNoFile, sCycle;
	sCycle.Format(_T("%d"), Cycle);
	while (pXmlCycle)
	{
		CTiXmlHelper::GetXmlData(pXmlCycle, _T("CYCLE_NO"), &sCycleNoFile);
		if (sCycleNoFile == sCycle)
		{
			TiXmlNode* pXmlStepList = pXmlCycle->FirstChild(_T("STEP_LIST"));
			if (nullptr == pXmlStepList)
				return false;
			TiXmlNode* pXmlStep = pXmlStepList->FirstChild(_T("STEP"));

			while (pXmlStep)
			{
				CTiXmlHelper::GetXmlData(pXmlStep, _T("CYCLE_NO"), &sCycleNoFile);
				if (sCycleNoFile != sCycle)
					break;
				pXmlStep = pXmlStep->NextSibling(_T("STEP"));

				retCount++;
			}
			return retCount;
		}
		else
		{
			pXmlCycle = pXmlCycle->NextSibling(_T("CYCLE"));
			if (nullptr == pXmlCycle)
				return retCount;
		}
	}
	return retCount;
}
bool	CMgrSchedule::LoadScheduleXml(LPCTSTR lpszFilePath)
{
	ClearSchedule();

	m_nStepIndex = 1;

	TiXmlDocument XmlDoc;

	if (false == XmlDoc.LoadFile(lpszFilePath))
		return false;

	TiXmlNode* pXmlSchdule = XmlDoc.FirstChild(_T("SCHEDULE"));
	TiXmlNode* pXmlSchdule2 = XmlDoc.FirstChild(_T("SCHEDULE"));

	if (nullptr == pXmlSchdule)
		return false;

	CString strScheduleId;
	CTiXmlHelper::GetXmlData(pXmlSchdule, _T("SCH_ID"), &strScheduleId);

	CString strScheduleDesc;
	CTiXmlHelper::GetXmlData(pXmlSchdule, _T("SCH_DESC"), &strScheduleDesc);
	
	CString strSVDFSafety;
	CTiXmlHelper::GetXmlData(pXmlSchdule, _T("SVDF"), &strSVDFSafety);
	ParseSVDF(strSVDFSafety);

	while (pXmlSchdule2)
	{
		bool bLoadXml = LoadScheduleSafetyXml(pXmlSchdule2);

		pXmlSchdule2 = pXmlSchdule2->NextSibling(_T("SCHEDULE"));

		if (false == bLoadXml)
			return false;
	}

	TiXmlNode* pXmlStepList = pXmlSchdule->FirstChild(_T("GRADE_SETTING"));

	if (nullptr != pXmlStepList)
	{
		RemoveScheduleGrade();

		TiXmlElement* pXmlOptionList = pXmlStepList->FirstChildElement(_T("OPTION"));//"GRADE"));

		if (nullptr != pXmlOptionList)
		{
			while (pXmlOptionList)
			{
				bool bLoadXml = LoadGradeXml(pXmlOptionList);

				pXmlOptionList = pXmlOptionList->NextSiblingElement(_T("OPTION"));//"GRADE"));

				if (false == bLoadXml)
					return false;
			}
		}
	}

	TiXmlNode* pXmlTempList = pXmlSchdule->FirstChild(_T("TEMPAMPERE_SETTING"));

	if (nullptr != pXmlTempList)
	{
		RemoveCurrentMap(); //taek 어떻게 할지 고민 중...

		TiXmlElement* pXmlOptionList = pXmlTempList->FirstChildElement(_T("OPTION"));//"GRADE"));

		if (nullptr != pXmlOptionList)
		{
			while (pXmlOptionList)
			{
				bool bLoadXml = LoadTempXml(pXmlOptionList);

				pXmlOptionList = pXmlOptionList->NextSiblingElement(_T("OPTION"));//"GRADE"));

				if (false == bLoadXml)
					return false;
			}
		}
	} //taek sk

	TiXmlNode* pXmlCycleList = pXmlSchdule->FirstChild(_T("CYCLE_LIST"));

	if (nullptr == pXmlCycleList)
		return false;

	TiXmlNode* pXmlCycle = pXmlCycleList->FirstChild(_T("CYCLE"));

	if (pXmlCycle == NULL)
	{
		//AfxMessageBox(_T("스케줄 정보가 없습니다 확인 해주세요"));
		SYSLOG(Dbg, _T("%s return"), __FUNCTION__);

		return false;
	}

	while (pXmlCycle)
	{
		bool bLoadXml = LoadCycleXml(pXmlCycle);

		pXmlCycle = pXmlCycle->NextSibling(_T("CYCLE"));

		if (false == bLoadXml)
			return false;
	}

	SScheduleValidate kParser;
	if (false == IsScheduleValidation(kParser))
	{
		CString strValue;

		if (eScheduleValidateStepSize == kParser.nValidateCode)
			strValue = "There is no registered step information in the schedule.";
		else
		{
			switch (kParser.nValidateCode)
			{
				case eScheduleValidateRestBasic		: strValue.Format("In Schedule Cycle(%d), Step(%d), the end condition between REST in progress is not set.", kParser.nCycleNumber, kParser.nStepNumber); break;
				case eScheduleValidateCCBasic		: strValue.Format("In Schedule Cycle(%d), Step(%d), there is no set condition for the end of CC progress.", kParser.nCycleNumber, kParser.nStepNumber); break;
				case eScheduleValidateCCCVBasic		: strValue.Format("In Schedule Cycle(%d), Step(%d), there are no conditions for the end of CC/CV in progress.", kParser.nCycleNumber, kParser.nStepNumber); break;
				case eScheduleValidateCPBasic		: strValue.Format("In Schedule Cycle(%d), Step(%d), there are no Cond for the end of CP in progress.", kParser.nCycleNumber, kParser.nStepNumber); break;
				case eScheduleValidateCPCVBasic		: strValue.Format("In Schedule Cycle (%d) and Step (%d), there are no Cond for the end of CP/CV in progress.", kParser.nCycleNumber, kParser.nStepNumber); break;
			}

			AfxMessageBox(strValue);
		}

		return false;
	}

	//TODO: 시스템 안전조건과 스케줄의 안전조건 비교하기
	SScheduleSafetyCheck kSafetyCheck;
	if (false == CheckScheduleSafetyCondition(kSafetyCheck))
	{
		//오류에 대한 알람 처리
		CString strValue;
		switch (kSafetyCheck.nErrorCode)
		{
		case eScheduleSafetyCheckConfig: strValue = "The system safety Cond could not be verified."; break;
		case eScheduleSafetyCheckVoltMax: strValue = "The safety Cond [voltage upper limit] value of the schedule is greater than the system safety condition [maximum voltage]."; break;
		case eScheduleSafetyCheckVoltMin: strValue = "The safety Cond [voltage lower limit] value of the schedule is less than the system safety condition [minimum voltage]."; break;
		case eScheduleSafetyCheckCurrMax: strValue = "The safety Cond [upper current limit] value of the schedule is greater than the system safety condition [maximum current]."; break;
		case eScheduleSafetyCheckWHMax: strValue = "The safe Cond [capacity] value of the schedule is greater than the system safe condition [capacity]."; break;
		}

		AfxMessageBox(strValue);

		return false;
	}

	//20201103 turby 여기서 pattern 을 parsing 하는것 같다.
	if (false == SortStep())
		return false;
	
	if (false == SortCycle())
		return false;

	return true;
}

bool	CMgrSchedule::LoadScheduleSafetyXml(TiXmlNode* pXmlSchedulesafety)
{
	LPCTSTR lpszXmlName[] = { "SCH_SAFETY_LIMIT_COND", nullptr };

	for (int i = 0;; ++i)
	{
		if (nullptr == lpszXmlName[i])
			break;

		CString str;
		CTiXmlHelper::GetXmlData(pXmlSchedulesafety, lpszXmlName[i], &str);

		switch (i)
		{
			case XmlScheduleOption:SetSchedulesafety(str); 	break;
		}
	}

	return true;
}

bool	CMgrSchedule::LoadGradeXml(TiXmlElement* pXmlCycleGrade)//TiXmlNode* pXmlCycleGrade)
{
	LPCTSTR lpszXmlName[] = { "OPTION", nullptr };

	for (int i = 0;; ++i)
	{
		if (nullptr == lpszXmlName[i])
			break;

		CString str;
		CTiXmlHelper::GetXmlData(pXmlCycleGrade, lpszXmlName[i], &str);

		str=pXmlCycleGrade->GetText();

		switch (i)
		{
			case XmlScheduleOption: SetGradeSetting(str); break;
		}
	}

	return true;
}

bool	CMgrSchedule::LoadTempXml(TiXmlElement* pXmlCycleGrade)
{
	LPCTSTR lpszXmlName[] = { "OPTION", nullptr };

	for (int i = 0;; ++i)
	{
		if (nullptr == lpszXmlName[i])
			break;

		CString str;
		CTiXmlHelper::GetXmlData(pXmlCycleGrade, lpszXmlName[i], &str);

		str = pXmlCycleGrade->GetText();

		switch (i)
		{
			case XmlScheduleOption: SetTempSetting(str);		break;
		}
	}

	return true;
}

bool	CMgrSchedule::LoadCycleXml(TiXmlNode* pXmlCycle)
{
	LPCTSTR lpszXmlName[] = { "CYCLE_NO", "FINISH_COND" , "SAFETY_LIMIT_COND", "LOG_WRITE_COND", "CAN_COND", "LOOP_COUNT", "GOTO_CYCLE", "GOTO_LOOP_COUNT", "GOTO_NEXT_CYCLE", "CYCLE_END_TIME", nullptr };

	CCycle* pCycle = new CCycle;

	for (int i = 0;; ++i)
	{
		if (nullptr == lpszXmlName[i])
			break;

		CString str;
		CTiXmlHelper::GetXmlData(pXmlCycle, lpszXmlName[i], &str);

		switch (i)
		{
		case XmlCycleNumber			:pCycle->SetCycleNumber(atoi(str));		break;
		case XmlCycleComplete		:pCycle->SetFinishCondition(str);		break;
		case XmlCycleSafety			:pCycle->SetSafetyCondition(str);		break;
		case XmlCycleLogPeriod		:pCycle->SetLogRecodeTerm();			break;
		case XmlCycleLoopCount		:pCycle->SetLoopCount(atoi(str));		break;
		case XmlCycleGotoCycle		:pCycle->SetGotoCycle(atoi(str));		break;
 		case XmlCycleGotoLoopCount	:pCycle->SetGotoLoopCount(atoi(str));	break;
 		case XmlCycleGotoNextCycle	:pCycle->SetGotoNextCycle(atoi(str));	break;
		case XmlCycleEndTime		:pCycle->SetCycleEndTime(atoi(str));	break;
		}
	}	

	TiXmlNode* pXmlStepList = pXmlCycle->FirstChild(_T("STEP_LIST"));

	if (nullptr == pXmlStepList)
	{
		m_listCycle.AddTail(pCycle);

		return false;
	}

	TiXmlNode* pXmlStep = pXmlStepList->FirstChild(_T("STEP"));

	while (pXmlStep)
	{
		bool bLoadXml = LoadStepXml(pXmlStep, pCycle);

		pXmlStep = pXmlStep->NextSibling(_T("STEP"));

		if (false == bLoadXml)
		{
			if (pCycle)
			{
				delete pCycle;
				pCycle = nullptr;
			}

			return false;
		}
	}

	// choo 패턴이 하나일때는 Cycler 종료 종건에 에디터의 종료 조건 값을 넣자. 패턴에디터의 종료 조건에 따라 멈출수 있게..
	// Cycle 종료 조건 자체를 체크 안한다. 그런 함수 자체가 없음.
	//if (pCycle->GetStepList().GetCount() == 1 && pCycle->GetCycleEndTime() == 0)
	//{
	//	CStep* pStep = pCycle->GetAtStep(1);
	//	if (pStep != NULL && pStep->GetStepType() == eIFBoardStepTypeExternalPattern)
	//	{
	//		/*std::vector<SScheduleComplete*>& GetScheduleComplete() { return m_vecScheduleComplete; }
	//		pStep->get*/
	//		pCycle->SetCycleEndTime(pStep->GetPTNEndTime());
	//	}
	//}

	m_listCycle.AddTail(pCycle);

	return true;
}

bool	CMgrSchedule::LoadStepXml( TiXmlNode* pXmlStep, CCycle* pCycle )
{
	LPCTSTR lpszXmlName[] = { "STEP_NO", "STEP_TYPE", "STEP_MODE","MODE_TYPE", "VOLTAGE", "DISCHARGE_VOLTAGE", "CURRENT_A", "DISCHARGE_CURRENT_A" , "WATT", "FINISH_COND", "SAFETY_LIMIT_COND",
								"LOG_WRITE_COND", "PATTERN_ADDR","SYNC_TYPE", "CHAMBER_COND", "RATIO_COND", "SOH_COND", "CAN_COND", "CAN_SEND", "POWER_SUPPLY_VOLTAGE", "CHILLER_COND","PATTERN_CAP", "DAQ_COND","DCIR", nullptr };

	CStep* pStep = new CStep;

	pStep->SetCycleNumber(pCycle->GetCycleNumber());

	//pnx_hr dcir 2021.05.26 
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();
	if (pMgrConfig->GetDcirTimeINI() != "")
		pStep->SetDCIR(pMgrConfig->m_strDcirTime);

	for (int i = 0;; ++i)
	{
		if (nullptr == lpszXmlName[i])
			break;

		CString str;
		CTiXmlHelper::GetXmlData(pXmlStep, lpszXmlName[i], &str);
	//==> Address 에 한글이 있을 때에는 주소가 읽히지 않는다. 추책임에게 전달하자.

		switch (i)
		{
		case XmlStepNumber:					pStep->SetStepNumber(atoi(str));			break;
		case XmlStepType:					pStep->SetStepType(str);					break;
		case XmlStepMode:					pStep->SetStepMode(str);					break;
		case XmlModeType:					pStep->SetStepMode(str);					break; //AUTOCAL
		case XmlStepChargeVoltage:			pStep->SetChargeVoltage(str);				break;
		case XmlStepDischargeVoltage:		pStep->SetDischargeVoltage(str);			break;
		case XmlStepChargeAmpere:			pStep->SetChargeCurrentAmpere(str);			break;
		case XmlStepDisChargeAmpere:		pStep->SetDisChargeCurrentAmpere(str);		break;
		case XmlStepWatt:					pStep->SetWatt(str);						break;
		case XmlStepComplete:				pStep->SetFinishCondition(str);				break;
		case XmlStepSafety:					pStep->SetSafetyCondition(str);				break;
		case XmlStepLogPeriod:				pStep->SetLogRecodeTerm(str);				break;
		case XmlStepPatternPath:			pStep->SetPatternFilePath(str);				break;
		case XmlStepChamber:				pStep->SetChamberCondition(str);			break;
		case XmlStepRatio:					pStep->SetRatioCondition(str);				break;
		case XmlStepRatioSOH:				pStep->SetRatioConditionSOH(str);			break;
		case XmlStepPatternCap:				pStep->SetPatternCapacity(str);				break;
		case XmlStepDAQ:					pStep->SetDAQCondition(str);				break;
		case XmlDCIR:						pStep->SetDCIR(str);						break;
		}
	}
	
	//pattern 인지 확인해서 vol, curr 값을 limit 값으로 변환한다.
	if (pStep->GetStepType() == eIFBoardStepTypeExternalPattern)
	{
		CString str;

		pStep->SetStepPattern();
		pStep->SetChargeVol_limit();
		pStep->SetDisChargeVol_limit();
		pStep->SetChargeCurr_limit();
		pStep->SetDisChargeCurr_limit();
	}
	//20201102 turby pattern parsing 부분인데 모르겠다.
	
	pCycle->Add(pStep);		
	
	// 먼저 Pattern 정보가 있는지 확인 하자
	int nIndex = 0;

	//사이클 패턴정보 있음 밑에는 뭐 계산 해주긴 하는데 결국 사이즈 정보는 없음
	if (pStep->GetSize() > 0)
	{
		int nPatternTime = pStep->GetPattern()->GetPatternTime();
		int nTimeCondition = 0;

		SScheduleComplete* lpScheduleComplete = pStep->GetScheduleCompleteFromName("Ti");

		if (lpScheduleComplete) //taek pattern 타임 종료 확인 해 보자
		{
			nTimeCondition = (int)(CPnxUtil::GetMgr()->GetSplitTime(lpScheduleComplete->strValue, ':') * 0.01f);

			if (nTimeCondition == 0)
			{
				nTimeCondition = nPatternTime;
			}
		}

		// Pattern 의 총 시간에서 Pattern 의 갯수를 빼서 Loop Count 로 돌려버리자.
		int nLoopCount = 0;

		if (nTimeCondition > 0 && nPatternTime > 0)
		{
			if (nTimeCondition < nPatternTime)
			{
				nLoopCount = nPatternTime / nTimeCondition;

				if (nPatternTime % nTimeCondition != 0)
					nLoopCount++;					
			}
			else
			{
				nLoopCount = nTimeCondition / nPatternTime;
				if (nTimeCondition % nPatternTime != 0)						
					nLoopCount++;					
			}
		}			

		int nStepIndex = m_nStepIndex;

		int nPTNMode = 0;
		int nPTNTEMP = 0;

		CPattern* pMasterPTN = pStep->GetPattern();

		if (NULL != pMasterPTN)
		{
			nPTNMode = pMasterPTN->GetPatternMode();
			nPTNTEMP = pMasterPTN->GetPatternTemp();

#if 0	//20201120 by choo pattern mode 값 변경한다.
			if (pStep->GetChargeVoltage() > 0 || pStep->GetDisChargeVoltage() > 0)
			{
				nPTNMode = 4;
				pMasterPTN->SetPatternMode(nPTNMode);
			}
#endif
		}

		CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

		// 여기서 Pattern 정보를 가지고 있던 Step 을 미리 추가 하느냐 마느냐..

		pStep->SetStepIndex(m_nStepIndex);	//bug fix

		CList< TPattern*, TPattern* >& listPattern = pStep->GetPattern()->GetPatternList();

		TPattern* pTail = listPattern.GetTail();

 		auto Pos = listPattern.GetHeadPosition();
 
 		while (Pos)
 		{
 			CStep* pStepPattern = new CStep;
 
 			BOOL bRet = pStepPattern->CopyData(pStep, nIndex);
 
 			TPattern* pPattern = listPattern.GetNext(Pos);
 
 			if (NULL == pPattern)
 			{
 				DELETE_POINTER(pStepPattern);
 
 				nIndex++;
 
 				continue;
 			}
 
#if 1
			if (nPTNMode == 1)
			{
				pStepPattern->SetStepMode("CC");

				float fCurrent = abs(pPattern->fCurr);

				if (pPattern->fCurr > 0.000f)
				{
					if (pPattern->fCurr > pStep->GetChargeCurrentLimit())
						fCurrent = pStep->GetChargeCurrentLimit();
					pStepPattern->SetStepType("Charge");
					pStepPattern->SetChargeVoltage(pMgrConfig->GetEQVoltageMax());
					pStepPattern->SetChargeCurrentAmpere(fCurrent);
				}
				else
				{
					float dis_limit = pStep->GetDischargeCurrentLimit();
					dis_limit *= -1;

					if (pPattern->fCurr < dis_limit)
						fCurrent = dis_limit*(-1);
					pStepPattern->SetStepType("Discharge");
					pStepPattern->SetDischargeVoltage(pMgrConfig->GetEQVoltageMin());
					pStepPattern->SetDisChargeCurrentAmpere(fCurrent);
				}
			}
			else if (nPTNMode == 2)
			{
				pStepPattern->SetStepMode("CP");

				float fValue = abs(pPattern->fWatt);

				if (pPattern->fWatt > 0.000f)
				{
					pStepPattern->SetStepType("Charge");
					pStepPattern->SetChargeVoltage(pMgrConfig->GetEQVoltageMax());
					pStepPattern->SetWatt(fValue);
				}
				else
				{
					pStepPattern->SetStepType("Discharge");
					pStepPattern->SetDischargeVoltage(pMgrConfig->GetEQVoltageMin());
					pStepPattern->SetWatt(fValue);
				}
			}
			else if (nPTNMode == 3)
			{
				pStepPattern->SetStepMode("CPCV");

				float fPower = abs(pPattern->fWatt);

				if (pPattern->fWatt > 0.000f)
				{
					pStepPattern->SetStepType("Charge");
					if (pStep->GetChargeVol_limit() > pPattern->fVol)
						pStepPattern->SetChargeVoltage(pPattern->fVol);
					else
						pStepPattern->SetChargeVoltage(pStep->GetChargeVol_limit());
					pStepPattern->SetWatt(fPower);
				}
				else
				{
					pStepPattern->SetStepType("Discharge");
					if (pStep->GetDisChargeVol_limit() > pPattern->fVol)
						pStepPattern->SetChargeVoltage(pPattern->fVol);
					else
						pStepPattern->SetChargeVoltage(pStep->GetChargeVol_limit());
					pStepPattern->SetWatt(fPower);
		}
	}
#else
 			// Voltage 는 무조건 해당 설비당 최고, 최저를 넣고 Ampere 값만변경한다.
 			// Add By JSK - 2018.06.22
 			if (1 == nPTNMode)
 			{
 				pStepPattern->SetStepMode("CC");
 
 				float fCurrent = abs(pPattern->fValue);
 
 				if (pPattern->fValue > 0.000f)
 				{
 					pStepPattern->SetStepType("Charge");
 					pStepPattern->SetChargeVoltage(pMgrConfig->GetEQVoltageMax());
 					pStepPattern->SetChargeCurrentAmpere(fCurrent);
 				}
 				else
 				{
 					pStepPattern->SetStepType("Discharge");
 					pStepPattern->SetDischargeVoltage(pMgrConfig->GetEQVoltageMin());
 					pStepPattern->SetDisChargeCurrentAmpere(fCurrent);
 				}
 			}
 			else if (3 == nPTNMode)
 			{
 				pStepPattern->SetStepMode("CP");
 
 				float fCurrent = abs(pPattern->fValue);
 
 				if (pPattern->fValue > 0.000f)
 				{
 					pStepPattern->SetStepType("Charge");
 					pStepPattern->SetChargeVoltage(pMgrConfig->GetEQVoltageMax());
 					pStepPattern->SetWatt(fCurrent * 0.001f);
 				}
 				else
 				{
 					pStepPattern->SetStepType("Discharge");
 					pStepPattern->SetDischargeVoltage(pMgrConfig->GetEQVoltageMin());
 					pStepPattern->SetWatt(fCurrent * 0.001f);
 				}
 			}
			else if (4 == nPTNMode)
			{
				pStepPattern->SetStepMode("CPCV");

				float fCurrent = abs(pPattern->fValue);

				if (pPattern->fValue > 0.000f)
				{
					pStepPattern->SetStepType("Charge");
					pStepPattern->SetChargeVoltage(pStep->GetChargeVoltage());	//taek 여기에 pattern 전류값 기입
					pStepPattern->SetChargeCurrentAmpere(fCurrent * 0.001f);
				}
				else
				{
					pStepPattern->SetStepType("Discharge");
					pStepPattern->SetDischargeVoltage(pStep->GetDisChargeVoltage()); //taek 여기에 pattern 전류값 기입
					pStepPattern->SetDisChargeCurrentAmpere(fCurrent * 0.001f);
				}
			}
#endif
			//TEMP 관련 하여서 여기에서 작성하자
			//TEmp 값이 있으면 시간 값 계산 후에 넣고 없으면 빼고
			if (nPTNTEMP == 1)
			{
				if (pPattern->fTemp != NULL)
				{
					pStepPattern->SetPatternTemp(pPattern->fTemp);
					pStepPattern->SetChamberSetTemp(pPattern->fTemp);
					pStepPattern->SetPatternTime(pPattern->nTime);
				}
			}

 			pStepPattern->SetPatternPeriod(pPattern->nTime);
 			pStepPattern->SetStepIndex(m_nStepIndex);

			if(nTimeCondition < nPatternTime)
				pStep->SetPTNEndTime(nTimeCondition);
			else
				pStep->SetPTNEndTime(nPatternTime);

 			pStepPattern->SetIsPattern(TRUE);
 
 			if (TRUE == bRet && pTail && pTail->nIndex == nIndex)
 			{
 				pStepPattern->SetCycleLoopCount(nLoopCount);
 
 				if (nLoopCount > 1)
 					pStepPattern->SetPrevStepIndex(nStepIndex);
 				else
 				{
 					pStepPattern->SetCycleLoopCount(0);
 					pStepPattern->SetPrevStepIndex(0);
 				}						
 			}
 
 			m_listStep.AddTail(pStepPattern);
 
 			m_nStepIndex++;				
 
 			nIndex++;
 		}

		//pStep->SetStepIndex(m_nStepIndex);	//다음 스탭 인덱스랑 겹친다
	}
	else
	{
		CStep* pStepPattern = new CStep;

		pStepPattern->CopyData(pStep);

		pStepPattern->SetStepIndex(m_nStepIndex);
		pStep->SetStepIndex(m_nStepIndex);

		m_listStep.AddTail(pStepPattern);
		//m_listStepView.AddTail(pStepPattern);

		m_nStepIndex++;
	}
	

	//
	m_listStepView.AddTail(pStep);


	return true;
}

void	CMgrSchedule::RemoveAll()
{
	while (!m_listStep.IsEmpty())
		delete m_listStep.RemoveHead();

	while (!m_listCycle.IsEmpty())
		delete m_listCycle.RemoveHead();
}

CList< CCycle*, CCycle* >&	CMgrSchedule::GetCycleList()
{
	return m_listCycle;
}

CList< CStep*, CStep* >&	CMgrSchedule::GetStepList()
{
	return m_listStep;
}

CCycle*	CMgrSchedule::GetAtCycle(int nCycleNumber)
{
	auto Pos = m_listCycle.GetHeadPosition();

	while (Pos)
	{
		CCycle* pCycle = m_listCycle.GetNext(Pos);

		if (NULL == pCycle)
			continue;

		if (pCycle->GetCycleNumber() == nCycleNumber)
			return pCycle;
	}

	return NULL;
}

CStep*	CMgrSchedule::GetAtStep(int nCycleNumber, int nStepNumber)
{
	auto PosCycle = m_listCycle.GetHeadPosition();

	while (PosCycle)
	{
		CCycle* pCycle = m_listCycle.GetNext(PosCycle);

		if (NULL == pCycle)
			continue;

		CList< CStep*, CStep* >& listStep = pCycle->GetStepList();

		auto PosStep = listStep.GetHeadPosition();

		while (PosStep)
		{
			CStep* pStep = listStep.GetNext(PosStep);

			if (NULL == pStep)
				continue;

			if (pStep->GetCycleNumber() == nCycleNumber && pStep->GetStepNumber() == nStepNumber)
				return pStep;
		}
	}

	return NULL;
}

CStep*	CMgrSchedule::GetAtStep(int nStepIndex)
{
	auto Pos = m_listStep.GetHeadPosition();

	while (Pos)
	{
		CStep* pStep = m_listStep.GetNext(Pos);

		if (NULL == pStep)
			continue;

		if (pStep->GetStepIndex() == nStepIndex)
			return pStep;
	}

	return NULL;
}

int		CMgrSchedule::GetStepIndex(int nCycleNumber, int nStepNumber)
{
	auto Pos = m_listStep.GetHeadPosition();

	while (Pos)
	{
		CStep* pStep = m_listStep.GetNext(Pos);

		if (NULL == pStep)
			continue;

		if (pStep->GetCycleNumber() == nCycleNumber && pStep->GetStepNumber() == nStepNumber)
			return pStep->GetStepIndex();
	}

	return 0;
}

bool	CMgrSchedule::IsScheduleValidation(SScheduleValidate& kParser)
{
	if (m_listStep.GetSize() <= 0)
	{
		kParser.nValidateCode = eScheduleValidateStepSize;
		return false;
	}

	auto Pos = m_listStep.GetHeadPosition();

	while (Pos)
	{
		CStep* lpStep = m_listStep.GetNext(Pos);

		if(!lpStep)
			continue;

		kParser.nCycleNumber	= lpStep->GetCycleNumber();
		kParser.nStepNumber		= lpStep->GetStepNumber();

		if (eIFBoardStepTypeRest == lpStep->GetStepType())
		{
			// CC 일때 걸수 있는 종료 조건 : 전압상한, 시간, 용량, 파워이다. 전부 없으면 Baaaaaaaaaaaaaam
			bool bIsExistCompleteCondition = false;

			LPCTSTR lpszCondition[] = { "HV", "LV", "Ti", nullptr };

			for (int i = 0; ; ++i)
			{
				if (nullptr == lpszCondition[i])
					break;

				SScheduleComplete* lpScheduleComplete = lpStep->GetScheduleCompleteFromName(lpszCondition[i]);

				if (!lpScheduleComplete)
					continue;

				if (strcmp(lpszCondition[i], "Ti") == 0)
				{
					if (strcmp(lpScheduleComplete->strValue, "00:00:00:00") != 0)
						bIsExistCompleteCondition = true;
				}

				if (bIsExistCompleteCondition)
					break;
			}

			if (false == bIsExistCompleteCondition)
			{
				kParser.nValidateCode = eScheduleValidateRestBasic;
				return false;
			}
		}
		else if (eIFBoardStepTypeCharge == lpStep->GetStepType() || eIFBoardStepTypeDischarge == lpStep->GetStepType())
		{
			if (eIFBoardStepModeCC == lpStep->GetStepMode())
			{
				bool bIsExistCompleteCondition = false;

				LPCTSTR lpszCondition[] = {"HV", "LV", "Ti", "Ca", "Po", nullptr};

				for (int i = 0; ; ++i)
				{
					if(nullptr == lpszCondition[i])
						break;

					SScheduleComplete* lpScheduleComplete = lpStep->GetScheduleCompleteFromName(lpszCondition[i]);

					if(!lpScheduleComplete)
						continue;

					if (strcmp(lpszCondition[i], "Ti") == 0)
					{
						if (strcmp(lpScheduleComplete->strValue, "00:00:00:00") != 0)
							bIsExistCompleteCondition = true;
					}
					else if (eIFBoardStepTypeCharge == lpStep->GetStepType() && strcmp(lpszCondition[i], "HV") == 0)
					{
						// 나도 일단 넣자. 상세 정보 체크 기능 넣어야 함.
						bIsExistCompleteCondition = true;
					}
					else if (eIFBoardStepTypeDischarge == lpStep->GetStepType() && strcmp(lpszCondition[i], "LV") == 0)
					{
						// 나도 일단 넣자. 상세 정보 체크 기능 넣어야 함.
						bIsExistCompleteCondition = true;
					}
					else
					{
						if (lpScheduleComplete->fValue != 0.000f)
							bIsExistCompleteCondition = true;
					}

					if(bIsExistCompleteCondition)
						break;
				}

				if (false == bIsExistCompleteCondition)
				{
					kParser.nValidateCode = eScheduleValidateCCBasic;
					return false;
				}
			}
			else if (eIFBoardStepModeCCCV == lpStep->GetStepMode())
			{
				bool bIsExistCompleteCondition = false;

				LPCTSTR lpszCondition[] = { "CUR", "Ti", "Ca", "Po", nullptr };

				for (int i = 0; ; ++i)
				{
					if (nullptr == lpszCondition[i])
						break;

					SScheduleComplete* lpScheduleComplete = lpStep->GetScheduleCompleteFromName(lpszCondition[i]);

					if (!lpScheduleComplete)
						continue;

					if (strcmp(lpszCondition[i], "Ti") == 0)
					{
						if (strcmp(lpScheduleComplete->strValue, "00:00:00:00") != 0)
							bIsExistCompleteCondition = true;
					}
					else
					{
						if (lpScheduleComplete->fValue != 0.000f)
							bIsExistCompleteCondition = true;
					}

					if (bIsExistCompleteCondition)
						break;
				}

				if (false == bIsExistCompleteCondition)
				{
					kParser.nValidateCode = eScheduleValidateCCCVBasic;
					return false;
				}
			}
			else if (eIFBoardStepModeCP == lpStep->GetStepMode())
			{
				bool bIsExistCompleteCondition = false;

				LPCTSTR lpszCondition[] = { "HV", "LV", "Ti", "Po", "Ca", nullptr };

				for (int i = 0; ; ++i)
				{
					if (nullptr == lpszCondition[i])
						break;

					SScheduleComplete* lpScheduleComplete = lpStep->GetScheduleCompleteFromName(lpszCondition[i]);

					if (!lpScheduleComplete)
						continue;

					if (strcmp(lpszCondition[i], "Ti") == 0)
					{
						if (strcmp(lpScheduleComplete->strValue, "00:00:00:00") != 0)
							bIsExistCompleteCondition = true;
					}
					else
					{
						if (lpScheduleComplete->fValue != 0.000f)
							bIsExistCompleteCondition = true;
					}

					if (bIsExistCompleteCondition)
						break;
				}

				if (false == bIsExistCompleteCondition)
				{
					kParser.nValidateCode = eScheduleValidateCPBasic;
					return false;
				}
			}
			else if (eIFBoardStepModeCPCV == lpStep->GetStepMode())
			{
				bool bIsExistCompleteCondition = false;

				LPCTSTR lpszCondition[] = { "HV", "LV", "Ti", "Po", "Ca", nullptr };

				for (int i = 0; ; ++i)
				{
					if (nullptr == lpszCondition[i])
						break;

					SScheduleComplete* lpScheduleComplete = lpStep->GetScheduleCompleteFromName(lpszCondition[i]);

					if (!lpScheduleComplete)
						continue;

					if (strcmp(lpszCondition[i], "Ti") == 0)
					{
						if (strcmp(lpScheduleComplete->strValue, "00:00:00:00") != 0)
							bIsExistCompleteCondition = true;
					}
					else
					{
						if (lpScheduleComplete->fValue != 0.000f)
							bIsExistCompleteCondition = true;
					}

					if (bIsExistCompleteCondition)
						break;
				}

				if (false == bIsExistCompleteCondition)
				{
					kParser.nValidateCode = eScheduleValidateCPCVBasic;
					return false;
				}
			}
		}
	}

	return true;
}


//TODO: 스케줄 Load 시 시스템의 안전조건과 스케줄의 안전조건 비교
bool	CMgrSchedule::CheckScheduleSafetyCondition(SScheduleSafetyCheck& kParser)
{	
	//프로그램 조건
	CMgrConfig* pMgrConfig = CMgrConfig::GetMgr();

	if (!pMgrConfig)
	{
		kParser.nErrorCode = eScheduleSafetyCheckConfig;
		return false;
	}
	// Volage Max
	float fSafetyMaxVoltValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectVoltage);
	// Voltage Min
	float fSafetyMinVoltValue = pMgrConfig->GetSystemSafetyMin(eSyatemSafetyObjectVoltage);
	// Current Max
	float fSafetyMaxCurrValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCurrent);
	// Capacity Max
	float fSafetyMaxCapValue = pMgrConfig->GetSystemSafetyMax(eSyatemSafetyObjectCapacity);


	//스케줄러 조건
	std::vector<SScheduleSafety*>& vecScheduleSafety = GetScheduleSafety();

	auto iter = vecScheduleSafety.begin();
	auto iterE = vecScheduleSafety.end();

	while (iter != iterE)
	{
		SScheduleSafety* lpScheduleSafety = (*iter);

		if (lpScheduleSafety)
		{
			if (strcmp(lpScheduleSafety->strName, "VoMax") == 0)
			{
				float fDstValue = lpScheduleSafety->fValue;
				if (fDstValue > fSafetyMaxVoltValue)
				{
					kParser.nErrorCode = eScheduleSafetyCheckVoltMax;
					return false;
				}
			}
			else if (strcmp(lpScheduleSafety->strName, _T("VoMin")) == 0)
			{
				float fDstValue = lpScheduleSafety->fValue;
				if (fDstValue < fSafetyMinVoltValue)
				{
					kParser.nErrorCode = eScheduleSafetyCheckVoltMin;
					return false;
				}
			}
			else if (strcmp(lpScheduleSafety->strName, _T("CuMax")) == 0)	//전류
			{
				float fDstValue = lpScheduleSafety->fValue;
				if (fDstValue > fSafetyMaxCurrValue)
				{
					kParser.nErrorCode = eScheduleSafetyCheckCurrMax;
					return false;
				}
			}			
			else if (strcmp(lpScheduleSafety->strName, _T("WHMax")) == 0)	//전력량
			{
				float fDstValue = lpScheduleSafety->fValue;
				if (fDstValue > fSafetyMaxCapValue)
				{
					kParser.nErrorCode = eScheduleSafetyCheckWHMax;
					return false;
				}
			}
			/*
			else if (strcmp(lpScheduleSafety->strName, _T("CaMax")) == 0)	//용량
			{
				float fDstValue = lpScheduleSafety->fValue;
			}
			*/
		}

		++iter;
	}	

	return true;
}


bool	CMgrSchedule::SortStep()
{
 	int nPatternIndex	= -1;
 	int nStepIndex		= 0;

	auto Pos = m_listStep.GetHeadPosition();

	// Add By JSK - 2020.07.01 사고나여
	CStep* pTail = m_listStep.GetTail();

	if (!pTail)
		return false;

	while (Pos)
	{
		CStep* pStep = m_listStep.GetNext(Pos);

		if (NULL == pStep)
			continue;

		//SOC
		if (pStep->GetRatioSoc() > 0)
		{
			CStep* pTargetSoc = GetAtStep(pStep->GetRatioCycle(), pStep->GetRatioStep());

			if(pTargetSoc)
				pTargetSoc->SetSocTarget(TRUE);
		}

		//SOH
		if (pStep->GetRatioSoH() > 0)
		{
			CStep* pTargetSoH = GetAtStep(pStep->GetRatioCycleSOH(), pStep->GetRatioStepSOH());

			if(pTargetSoH)
				pTargetSoH->SetSoHTarget(TRUE);

			CStep* pTargetSoH2 = GetAtStep(pStep->GetRatioCycleSOH2(), pStep->GetRatioStepSOH2());

			if(pTargetSoH2)
				pTargetSoH2->SetSoHTarget2(TRUE);
		}

		// 1. 각 종료 조건 별로 Next Cycle, Step 정보를 구하고 거기에 맞는 Step Index를 구한다.
		std::vector<SScheduleComplete*>& vecScheduleComplete = pStep->GetScheduleComplete();

		auto iter = vecScheduleComplete.begin();
		auto iterE = vecScheduleComplete.end();

		while (iter != iterE)
		{
			SScheduleComplete* lpScheduleComplete = (*iter);

			if (lpScheduleComplete)
			{
				if (lpScheduleComplete->nCycle > 0 && lpScheduleComplete->nStep > 0)
				{
					if (lpScheduleComplete->nStep != 65533 && lpScheduleComplete->nStep != 65534 && lpScheduleComplete->nStep != 65535)
					{
						int nNextStep = GetStepIndex(lpScheduleComplete->nCycle, lpScheduleComplete->nStep);

							 if (strcmp(lpScheduleComplete->strName, _T("Ti")) == 0)	pStep->SetStepEndIndex(eIFBoardCompleteTime, nNextStep);
						else if (strcmp(lpScheduleComplete->strName, _T("HV")) == 0)	pStep->SetStepEndIndex(eIFBoardCompleteVoltage, nNextStep);
						else if (strcmp(lpScheduleComplete->strName, _T("LV")) == 0 && nNextStep > 0)	pStep->SetStepEndIndex(eIFBoardCompleteVoltage, nNextStep);
						else if (strcmp(lpScheduleComplete->strName, _T("CUR")) == 0)	pStep->SetStepEndIndex(eIFBoardCompleteCurrent, nNextStep);
						else if (strcmp(lpScheduleComplete->strName, _T("Ca")) == 0)	pStep->SetStepEndIndex(eIFBoardCompleteCapacity, nNextStep);
						else if (strcmp(lpScheduleComplete->strName, _T("Po")) == 0)	pStep->SetStepEndIndex(eIFBoardCompleteWatt, nNextStep);
					}
				}
				else if (lpScheduleComplete->nStep == 65533)
				{
					int nNewStepIndex = 0;

					if (pTail->GetStepIndex() != pStep->GetStepIndex())
					{
						nNewStepIndex = nStepIndex + 2;
					}
					else
					{
						//종료조건 더미 데이터 번지수 이동 할 수 있도록 바꿈
						nNewStepIndex = pTail->GetStepIndex() + 1;
						m_nLastIndex = nNewStepIndex;
					}
					// 전력 전자 요청으로 수정.. 
						 if (strcmp(lpScheduleComplete->strName, _T("Ti")) == 0)	pStep->SetStepEndIndex(eIFBoardCompleteTime, nNewStepIndex);
					else if (strcmp(lpScheduleComplete->strName, _T("HV")) == 0)	pStep->SetStepEndIndex(eIFBoardCompleteVoltage, nNewStepIndex);
					else if (strcmp(lpScheduleComplete->strName, _T("CUR")) == 0)	pStep->SetStepEndIndex(eIFBoardCompleteCurrent, nNewStepIndex);
					else if (strcmp(lpScheduleComplete->strName, _T("Ca")) == 0)	pStep->SetStepEndIndex(eIFBoardCompleteCapacity, nNewStepIndex);
					else if (strcmp(lpScheduleComplete->strName, _T("Po")) == 0)	pStep->SetStepEndIndex(eIFBoardCompleteWatt, nNewStepIndex);
					else if (strcmp(lpScheduleComplete->strName, _T("LV")) == 0)	pStep->SetStepEndIndex(eIFBoardCompleteVoltage, nNewStepIndex); // add turby
					//else if (strcmp(pSrc->strName, _T("Tah")) == 0)	pStep->SetStepEndIndex(StepEndCondtionTah, nNewStepIndex); //taek 200113 봉인
				}
			}

			++iter;
		}

		// 3. Step 의 Pattern 의 마지막에 Return Step Index 구해야 함
		if (nPatternIndex == -1 && pStep->GetPatternPeriod() > 0)
		{
			//최초 Pattern 정보가 있다면..
			nPatternIndex = pStep->GetStepIndex();
		}
		else if (nPatternIndex > 0 && pStep->GetPatternPeriod() <= 0)
		{
			// Pattern 의 마지막 정보인 경우
			nPatternIndex = -1;

			// 해당 시점에서의 Step 정보는 이미 Pattern 정보가 끝난 이후다.
			CStep* pPrevStep = GetAtStep(nStepIndex);

			if (pPrevStep && pPrevStep->GetPrevStepIndex() <= 0)
			{
				pPrevStep->SetPrevStepIndex(0);
			}
		}

		nStepIndex = pStep->GetStepIndex();
	}


	//Cycle 의 마지막 Step 마다 Loop Count, Goto Step Index, Return Step Index 구해야 함
	Pos = m_listCycle.GetHeadPosition();

	while (Pos)
	{
		CCycle* pCycle = m_listCycle.GetNext(Pos); //일단 사이클 마다 돌면서

		if (!pCycle)
			continue;
		
		CStep* pStepFisrt = pCycle->GetHeadData();		//사이클의 제일 첫번째 스탭

		CStep* pStep = pCycle->GetTailData();			//마지막 스탭만 빼오자	//하..사이클에 있는 스탭 리스트가 패턴정보를 가지고있음... 이걸로 계산하고

		//Todo : 패턴 마지막에 go to 값이 없어서 문제가발생함. 이부분 수정중...
		//패턴의 마지막을 찾는다
		int cycleCount = 1;
		int patternLastIndex = 0;
		bool findPattern = false;
		while (true)
		{
			//마지막 스탭이 패턴인경우는 아래쪽에 조건이 있음
			if (pStep->GetStepPattern() == eIFBoardStepTypeExternalPattern)
				break;

			CStep* pStepFind = pCycle->GetAtStep(cycleCount);

			//패턴을 찾음
			if (pStepFind->GetStepPattern() == eIFBoardStepTypeExternalPattern && findPattern == false)
			{
				//패턴찾았음
				findPattern = true;
			}
			//패턴을 찾은상태에서 패턴 아닌게 나오면 -1 해당 인덱스가 패턴의 마지막인덱스
			if (findPattern == true && pStepFind->GetStepPattern() != eIFBoardStepTypeExternalPattern)
			{
				patternLastIndex = pStepFind->GetStepIndex() - 1;

				//패턴 마지막 스텝에 Go to, Return Step Index 넣기
				int nFisrtIndex = pStepFisrt->GetStepIndex();

				CStep* pSetStep = GetAtStep(patternLastIndex);
				pSetStep->SetLoopNextCycle(patternLastIndex + 1);
				pSetStep->SetPrevStepIndex(nFisrtIndex);
				findPattern = false;

				// -> Set후 다음 또 있는지 검색
			}

			//마지막 스텝에 도착하면 break
			if (pStep->GetStepIndex() == pStepFind->GetStepIndex())
				break;

			cycleCount++;
		}
		////////////////////여기까지

		if(!pStep)
			continue;
																				//스탭데이터 보낼때의 스탭은 채널에 있는 스탭리스트가 함..
		CStep* pStepResult = GetAtStep(pStep->GetStepIndex());					//결국 이쪽 Step에 셋해줘야함..

		if (pStep && pStepResult && pStepFisrt)
		{
			if (pStep == NULL)
			{
				SYSLOG(Dbg, _T("%s step is null"), __FUNCTION__);
				continue;
			}

			if (pStep->GetSize() > 0)	//사이클 마지막 스탭이 패턴일경우
			{							//현재 스탭 인덱스+패턴사이즈 한게 패턴의 fw base의 실제 마지막 인덱스가 되겠다
										//솔직히 내부적으로 너무 꼬여서 주석 달아놔도 많이 햇갈릴테니 일단 믿고 쓰시길 바랍니다
										//pStep->GetStepIndex() 이 패턴일때 제일 첫번째 fw base 인덱스가 되겠다

				int nNextStepIndex = 0;	//이동할 다음 인덱스

				//int nFisrtIndex = pStep->GetStepIndex(); //패턴의 처음 인덱스

				int nFisrtIndex = pStepFisrt->GetStepIndex(); //루프 불만족시 이동 인덱스

				int nLastIndex = pStep->GetStepIndex() + pStep->GetSize(); //패턴의 마지막 인덱스
				
				nLastIndex -= 1; // 시작하는 자기자신은 빼야된다

				CStep* pPatternLastStep = GetAtStep(nLastIndex);

				pStepResult = GetAtStep(nLastIndex);

				if (pPatternLastStep)
				{
					CCycle* pCycleNext = NULL;
//  					TRACE(_T("P Y: pCycle->GetLoopCount(%d)\n"), pCycle->GetLoopCount()); //루프설정
//  					TRACE(_T("P Y: nFisrtIndex(%d)\n"), nFisrtIndex); //루프 불만족시 이동 인덱스
//  					TRACE(_T("P Y: nLastIndex(%d)\n"), nLastIndex);
//  					TRACE(_T("P Y: pCycle->GetGotoCycle(%d)\n"), pCycle->GetGotoCycle()); //다음이동 사이클 번호
					
					if (pCycle->GetGotoCycle() == 0)
					{//다음 사이클 설정이 0이라면
						pCycleNext = GetAtCycle(pStep->GetCycleNumber() + 1); //다음 사이클을 가져와
					}
					else
					{//지정 사이클이 있으면
						pCycleNext = GetAtCycle(pCycle->GetGotoCycle()); //지정 사이클을 가져와

						if (pCycle->GetCycleNumber() == pCycle->GetGotoCycle()) //지정사이클이 자기 자신 이라면
						{
							pCycleNext = GetAtCycle(pStep->GetCycleNumber());//요렇게 하면 될듯 NULL; //널참조로 밑에 +1타게끔 //taek 자기 자신을 참조 하여 무한 루프
						}
					}

					if (pCycleNext)
					{//사이클정보 있는지보고
						CStep* pStepNext = pCycleNext->GetHeadData();

						if (pStepNext)
						{//제일 첫번재 스탭을 빼온다
							nNextStepIndex = pStepNext->GetStepIndex();
							//TRACE(_T("P Y: nNextStepIndex(%d)\n"), nNextStepIndex);
						}
					}
					else 
					{//없으면 마지막 사이클이란 얘기인데..
						nNextStepIndex = nLastIndex + 1; //일단 마지막스탭인덱스 + 1 해보자(fw팀이랑 확인필요)
						//TRACE(_T("P Y: nNextStepIndex(%d)\n"), nNextStepIndex);
					}
					
					//셋팅..
					int nLoopCount = nNextStepIndex;
					if (g_nCycleLoopStartSet[pCycle->GetCycleNumber() - 1] > 1)
						nLoopCount = pCycle->GetLoopCount() - g_nCycleLoopStartSet[pCycle->GetCycleNumber() - 1];
					else
						nLoopCount = pPatternLastStep->GetCycleLoopCount();
						//nLoopCount = pCycle->GetLoopCount();

					pStepResult->SetCycleLoopCount(nLoopCount);
					pStepResult->SetPrevStepIndex(nFisrtIndex);
					pStepResult->SetLoopNextCycle(nNextStepIndex);

					//TRACE(_T("\n"));
				}
				else
				{
					SYSLOG(Dbg, _T("%s pPatternLastStep is null"), __FUNCTION__);
					continue;
				}
			}
			else 
			{//패턴이 아닐때

				int nFisrtIndex = pStepFisrt->GetStepIndex(); //루프 불만족시 이동 인덱스

				int nNextStepIndex = 0;	//이동할 다음 인덱스
				// 					TRACE(_T("P N: pCycle->GetLoopCount(%d)\n"), pCycle->GetLoopCount()); //루프설정
				// 					TRACE(_T("P N: nFisrtIndex(%d)\n"), nFisrtIndex); //루프 불만족시 이동 인덱스
				// 					TRACE(_T("P N: pCycle->GetGotoCycle(%d)\n"), pCycle->GetGotoCycle()); //다음이동 사이클 번호
				CCycle* pCycleNext = NULL;

				if (pCycle->GetGotoCycle() == 0)
				{//다음 사이클 설정이 0이라면
					pCycleNext = GetAtCycle(pStep->GetCycleNumber() + 1); //다음 사이클을 가져와
				}
				else
				{//지정 사이클이 있으면
					pCycleNext = GetAtCycle(pCycle->GetGotoCycle()); //지정 사이클을 가져와 

					if (pCycle->GetCycleNumber() == pCycle->GetGotoCycle()) //지정사이클이 자기 자신 이라면
					{
						pCycleNext = GetAtCycle(pStep->GetCycleNumber());//요렇게 하면 될듯 NULL; //널참조로 밑에 +1타게끔 //taek 자기 자신을 참조 하여 무한 루프
					}
				}

				if (pCycleNext)
				{//사이클정보 있는지보고
					CStep* pStepNext = pCycleNext->GetHeadData();

					if (pStepNext)
					{//제일 첫번재 스탭을 빼온다
						nNextStepIndex = pStepNext->GetStepIndex();
						//TRACE(_T("P N: nNextStepIndex(%d)\n"), nNextStepIndex);
					}
				}
				else
				{//없으면 마지막 사이클이란 얘기인데..
					nNextStepIndex = pStep->GetStepIndex() + 1; //일단 마지막스탭인덱스 + 1 해보자(fw팀이랑 확인필요)
					//TRACE(_T("P N: nNextStepIndex(%d)\n"), nNextStepIndex);
				}

				//셋팅..
				int nLoopCount;
				if (g_nCycleLoopStartSet[pCycle->GetCycleNumber() - 1] > 1)
				{
					nLoopCount = pCycle->GetLoopCount() - g_nCycleLoopStartSet[pCycle->GetCycleNumber() - 1] + 1;
					pCycle->SetCycleLoopStartNo(g_nCycleLoopStartSet[pCycle->GetCycleNumber() - 1]);
				}
				else
				{
					nLoopCount = pCycle->GetLoopCount();
					pCycle->SetCycleLoopStartNo(1);
				}

				pStepResult->SetCycleLoopCount(nLoopCount);
				pStepResult->SetPrevStepIndex(nFisrtIndex);
				pStepResult->SetLoopNextCycle(nNextStepIndex);

				

				//TRACE(_T("\n"));
			}
		}
		else
		{
		SYSLOG(Dbg, _T("%s pStep, pStepResult is null"), __FUNCTION__);
		}
	}

	//20190731 debug start
	//단일 스탭 루프카운트 점프 문제로 일시적 으로 다음의 코드사용
	//루프카운트 있을시 최소 스탭 갯수는 2개다
	//fw 개선후 삭제바람
	Pos = m_listCycle.GetHeadPosition();

	while (Pos)
	{
		CCycle* pCycle = m_listCycle.GetNext(Pos);

		if (pCycle)
		{
			if (pCycle->GetLoopCount() > 1 && pCycle->GetSize() <= 1)
			{	
				AfxMessageBox("When setting the cycle loop, the minimum number of steps must be 2 or more.");

				return false;
			}
		}
	}
	//20190731 debug end

	return true;
}

int		CMgrSchedule::GetLastStepIndex()
{
	return m_nLastIndex;
}

int		CMgrSchedule::GetCycleLastIndex(int nStepIndex)
{
	CStep* pStep = GetAtStep(nStepIndex);
	if (pStep == NULL)
		return 999;

	CCycle* pCycle = GetAtCycle(pStep->GetCycleNumber());
	if (pCycle == NULL)
		return 999;

	CStep* pTailStep = pCycle->GetTailData();
	if (pTailStep == NULL)
		return 999;

	int nLastIndex = 999;

	if (pTailStep->GetSize() > 0) //패턴일때
	{
		nLastIndex = pStep->GetStepIndex() + pStep->GetSize(); //패턴의 마지막 인덱스
	}
	else
	{
		nLastIndex = pTailStep->GetStepIndex();
	}

	return nLastIndex;
}

bool	CMgrSchedule::SortCycle()
{ 
 	auto Pos = m_listCycle.GetHeadPosition();
 
 	CCycle* pTail = m_listCycle.GetTail(); //taek 아직 사용 안함

	if (!pTail)
		return false;
 
	while (Pos)
	{
		CCycle* pCycle = m_listCycle.GetNext(Pos);

		if (NULL == pCycle)
			continue;

		// 1. 각 종료 조건 별로 Next Cycle, Step 정보를 구하고 거기에 맞는 Step Index를 구한다.
		std::vector<SScheduleComplete*>& vecScheduleComplete = pCycle->GetScheduleComplete();

		auto iter = vecScheduleComplete.begin();
		auto iterE = vecScheduleComplete.end();

		while (iter != iterE)
		{
			SScheduleComplete* lpScheduleComplete = (*iter);

			if (lpScheduleComplete)
			{
				if (lpScheduleComplete->nCycle > 0 && lpScheduleComplete->nStep > 0)
				{
					if (lpScheduleComplete->nStep != 65533 && lpScheduleComplete->nStep != 65534 && lpScheduleComplete->nStep != 65535)
					{
						int nNextStep = GetStepIndex(lpScheduleComplete->nCycle, lpScheduleComplete->nStep);

							 if (strcmp(lpScheduleComplete->strName, _T("Ti")) == 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionTime, nNextStep);
						else if (strcmp(lpScheduleComplete->strName, _T("HV")) == 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionVoltage, nNextStep);
						else if (strcmp(lpScheduleComplete->strName, _T("LV")) == 0 && nNextStep > 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionLowVoltage, nNextStep);
						else if (strcmp(lpScheduleComplete->strName, _T("CUR")) == 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionAmpere, nNextStep);
						else if (strcmp(lpScheduleComplete->strName, _T("Ca")) == 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionCapacity, nNextStep);
						else if (strcmp(lpScheduleComplete->strName, _T("Po")) == 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionWatt, nNextStep);
					}
				}
				else if (lpScheduleComplete->nStep == 65533)
				{
					int nNewStepIndex = 0;

					//종료조건 더미 데이터 번지수 이동 할 수 있도록 바꿈
					if (pCycle->GetCycleNumber() == pTail->GetCycleNumber())
						nNewStepIndex = m_nLastIndex;
					else
					{
						CCycle* pNextCycle = GetAtCycle(pCycle->GetCycleNumber() + 1);
						if (NULL == pNextCycle)
						{
							nNewStepIndex = m_nLastIndex;
							SYSLOG(Dbg, _T("%s cycle is null"), __FUNCTION__);

						}
						else
						{
							CStep* pNextStep = pNextCycle->GetAtStep(1);

							if (NULL == pNextStep)
							{
								nNewStepIndex = m_nLastIndex;// pTail->GetCycleNumber() + 1; //taek 이거 맞는지 확인 테스트 test
							}
							else
							{
								nNewStepIndex = pNextStep->GetStepIndex();
							}
						}
					}
					//nNewStepIndex = m_nLastIndex;// pTail->GetCycleNumber() + 1; //taek 이거 맞는지 확인 테스트 test
					//					m_nLastIndex = nNewStepIndex;
					/*				}*/
					// 전력 전자 요청으로 수정.. 
						 if (strcmp(lpScheduleComplete->strName, _T("Ti")) == 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionTime, nNewStepIndex);
					else if (strcmp(lpScheduleComplete->strName, _T("HV")) == 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionVoltage, nNewStepIndex);
					else if (strcmp(lpScheduleComplete->strName, _T("LV")) == 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionLowVoltage, nNewStepIndex);
					else if (strcmp(lpScheduleComplete->strName, _T("CUR")) == 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionAmpere, nNewStepIndex);
					else if (strcmp(lpScheduleComplete->strName, _T("Ca")) == 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionCapacity, nNewStepIndex);
					else if (strcmp(lpScheduleComplete->strName, _T("Po")) == 0)	pCycle->SetCycleEndIndex(eCycleEndCondtionWatt, nNewStepIndex);
				}
			}

			++iter;
		}
	}

 	return true;
}

void CMgrSchedule::SetSchedulesafety(LPCTSTR lpszString)
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

		m_vecSafety.push_back(lpScheduleSafety);
	}
}


void CMgrSchedule::SetGradeSetting(LPCTSTR lpszString)
{
	CStringArray arrCondition;

	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	for (int i = 0; i < arrCondition.GetSize(); ++i)
	{
		CStringArray arr;
		CPnxUtil::GetMgr()->SplitString(arrCondition.GetAt(i), '=', arr);

		SScheduleGrade* lpScheduleGrade = new SScheduleGrade;
		lpScheduleGrade->Parse(arr);

		m_vecGrade.push_back(lpScheduleGrade);
	}
}

void CMgrSchedule::RemoveScheduleGrade()
{
	auto iter = m_vecGrade.begin();
	auto iterE = m_vecGrade.end();

	while (iter != iterE)
	{
		SScheduleGrade* lpScheduleGrade = (*iter);

		if (lpScheduleGrade)
		{
			delete lpScheduleGrade;
			lpScheduleGrade = nullptr;
		}

		++iter;
	}

	m_vecGrade.clear();
}

void CMgrSchedule::SetTempSetting(LPCTSTR lpszString)
{
	CStringArray arrCondition;

	// HV=0.000=0:65533/LV=0.000=0:65533/Ti=00:00:00:05=0:65533/RT=00:00:00:00=0:65533/
	CPnxUtil::GetMgr()->SplitString(lpszString, '/', arrCondition);

	for (int i = 0; i < arrCondition.GetSize(); ++i)
	{
		CStringArray arr;
		CPnxUtil::GetMgr()->SplitString(arrCondition.GetAt(i), '=', arr);

		SCurrentMap* lpCurrentMap = new SCurrentMap;
		lpCurrentMap->Parse(arr);

		m_vecCurrent.push_back(lpCurrentMap);
	}
}

void CMgrSchedule::RemoveCurrentMap()
{
	auto iter = m_vecCurrent.begin();
	auto iterE = m_vecCurrent.end();

	while (iter != iterE)
	{
		SCurrentMap* lpCurrentMap = (*iter);

		if (lpCurrentMap)
		{
			delete lpCurrentMap;
			lpCurrentMap = nullptr;
		}

		++iter;
	}

	m_vecCurrent.clear();
}

void CMgrSchedule::RemoveScheduleSafety()
{
	auto iter = m_vecSafety.begin();
	auto iterE = m_vecSafety.end();

	while (iter != iterE)
	{
		SScheduleSafety* lpScheduleSafety = (*iter);

		if (lpScheduleSafety)
		{
			delete lpScheduleSafety;
			lpScheduleSafety = nullptr;
		}

		++iter;
	}

	m_vecSafety.clear();
}

SScheduleSafety*	CMgrSchedule::GetScheduleSafetyFromName(CString strName)
{
	auto iter = m_vecSafety.begin();
	auto iterE = m_vecSafety.end();

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

void	CMgrSchedule::GetScheduleSafetyInfo(std::vector<CString>& vecValue)
{
	vecValue.clear();

	LPCTSTR lpszValue[] = {"VoMax", "VoMin", "CuMax", "CuMin", "CaMax", "CeTeMaxH", nullptr};

	for (int i = 0; ; ++i)
	{
		if(nullptr == lpszValue[i])
			break;

		SScheduleSafety* lpScheduleSafety = GetScheduleSafetyFromName(lpszValue[i]);

		if (lpScheduleSafety)
		{
			CString strValue;
			strValue.Format("%.3f", lpScheduleSafety->fValue);

			vecValue.push_back(strValue);
		}
		else
		{
			vecValue.push_back("0.000");
		}
	}
}

void	CMgrSchedule::ClearSchedule()
{
	while (!m_listStep.IsEmpty())
		delete m_listStep.RemoveHead();

	while (!m_listCycle.IsEmpty())
		delete m_listCycle.RemoveHead();

	RemoveScheduleSafety();

	RemoveScheduleGrade();

	RemoveCurrentMap();
}

void CMgrSchedule::ParseSVDF(CString strData)
{
	m_stSVDF->Clear();
	// -1000:4.0:1000:5:3:1
	CStringArray arr;
	CPnxUtil::GetMgr()->SplitString(strData, ':', arr);

	CString str;
	for (int i = 0; i < arr.GetSize(); ++i)
	{
		str = arr.GetAt(i);
		switch (i)
		{
		case SVDF_MAX_VOLT:			m_stSVDF->fMaxVolt = atof(str);					break;
		case SVDF_AVG_VOLT:			m_stSVDF->fAvgVolt = atof(str);					break;
		case SVDF_SOC_UPPER_DROP:	m_stSVDF->fSOCUpperDrop = atof(str);				break;
		case SVDF_SOC_LOWER_DROP:	m_stSVDF->fSOCLowerDrop = atof(str);				break;
		case SVDF_ERR_COUNT:		m_stSVDF->nErrCount = atoi(str);					break;
		case SVDF_VENT_OPEN:		m_stSVDF->bVentOpen = ((str == 1) ? true : false);	break;
		}
	}

	if (m_stSVDF->fMaxVolt != 0 && m_stSVDF->fAvgVolt != 0 && m_stSVDF->fSOCUpperDrop != 0 && m_stSVDF->fSOCLowerDrop != 0 && m_stSVDF->nErrCount != 0)
		m_stSVDF->bUseSVDF = true;
}