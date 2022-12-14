#include "stdafx.h"
#include "CommData.h"

#include "MgrChannel.h"
#include "MgrConfig.h"
#include "MgrAlarm.h"
#include "MgrDAQ.h"
#include "MgrDio.h"
#include "MgrPlcInterface.h"
#include "PnxUtil.h"
#include "MgrSerial.h"

CCommData::CCommData()
{
	m_strTime.Empty();

	m_nChannel			= 0;
	m_nTotalCount		= 0;
	m_nTotalCycleLoop	= 0;
	m_nCycleNo			= -1;
	m_nStepNo			= -1;
	m_nCycleTime		= 0;
	m_nStepTime			= 0;
	m_nWriteLogCount	= 0;
	m_nCommCmd			= 0;
	m_nOpcode			= 0;
	m_nType				= eIFBoardStepTypeUnknown;
	m_nTotalTime		= 0;
	m_nDataFrameCount	= 0;
	m_nEndReason		= 0;
	m_nStepIndex		= 0;
	m_nLogType			= 0;
	m_nLogChannelNumber	= 0;
	m_nTimeData			= 0;
	m_nTimeData2		= 0;
	m_nPatternTime		= 0;
	m_fCycleAh			= 0.0;
	m_nSOCValue			= 0;
	m_nSOHValue			= 0;
	m_fPatternWh		= 0.0;
	m_fSchAh			= 0.0;
	m_fPreVolt			= 0.0;
	m_fVolt				= 0.0;
	m_fPreCurr			= 0.0;
	m_fCurr				= 0.0;
	m_fChamberTemp		= 0.0;
	m_fChamberTempSp	= 0.0;
	m_fStepAh			= 0.0;
	m_fStepAhTotal		= 0.0;
	m_fDisStepAh		= 0.0;
	m_fCharStepAh		= 0.0;
	m_fStepWh			= 0.0;
	m_fPower			= 0.0;
	m_fRestVoltage		= 0.0;
	//m_fDcir				= 0.0;
	m_fDcir1			= 0.0;
	m_fDcir2			= 0.0;
	m_fDcir3			= 0.0;
	m_fLastDcir1 = 0.0;
	m_fLastDcir2 = 0.0;
	m_fLastDcir3 = 0.0;
	m_nCycleIndex		= 0;
	m_fDeltaVBefor		= 0.0;
	m_fDeltaVAfter		= 0.0;
	m_fDeltaIBefor		= 0.0;
	m_fDeltaIAfter		= 0.0;
	m_nBeforStepTimeV	= 0;
	m_nBeforStepTimeI	= 0;
	m_nProcessStatus	= 0;	
	m_fTemp = 0.0f;
	m_fPogoPinOhm_Monitoring = 0.0f;
	m_fPogoPinOhm_Setting = 0.0f;
	m_nRatioType = 0;
	m_fStepEndVoltage = 0;

	m_bSKipStepTimeInit = false;
	m_bSKipCycleTimeInit = false;

	m_bContactRegUpdate = true;
	
}


CCommData::~CCommData()
{
}

void	CCommData::CalcAmpere(int nStepIndex)
{
	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);

	if (NULL == pChannel)
		return;

	if (nStepIndex == 0)
	{
		this->m_nType = 0;
	}

	CStep* pStep = pChannel->GetMgr()->GetAtStep(nStepIndex);

	if (NULL == pStep)
		return;

	this->m_nType = pStep->GetStepType();

	float fCurrTemp = this->m_fCurr;

	if (GetStepType() == eIFBoardStepTypeCharge)
	{
		//SKIP
		
	}
	else if (GetStepType() == eIFBoardStepTypeDischarge)
	{
		//20190227 TEST START
// 		if (GetOPCode() > DIS_START && GetOPCode() < DIS_END)
// 		{
// 			fCurrTemp = (fCurrTemp)* (-1.0f);
// 		}

		if (fCurrTemp > 0.0f)
		{
			fCurrTemp = (fCurrTemp)* (-1.0f);
		}

		//20190227 TEST END

		//fCurrTemp = (fCurrTemp)* (-1.0f);	//abs
	}

	else if (GetStepType() == eIFBoardStepTypeExternalPattern)
	{
		if (GetOPCode() > DIS_START && GetOPCode() < DIS_END)
		{
			fCurrTemp = (fCurrTemp)* (-1.0f);
		}
		else if (GetOPCode() > CHA_START)
		{
			fCurrTemp = fCurrTemp;
		}
		else if (GetOPCode() == CHA_START)
		{
			fCurrTemp = 0;
		}
	}
	else
	{
		fCurrTemp = 0;
	}

	this->m_fCurr = fCurrTemp;
}

void CCommData::ConversionCommData(bool blsUIStepChange, bool bIsCycleChange)
{
	//주의 : 전단에 사용 값 들이 업데이트 먼저 되어야 함

	if (blsUIStepChange == true)
	{
		SetPatternAmpereHour(0.0f, FuncAgrsEqual);	//test
		SetPatternWattHour(0.0f, FuncAgrsEqual);
		SetPatternTotalTime(0, FuncAgrsEqual);
		SetPatternTotalAmpereHour(0, FuncAgrsEqual);
	}

	if (true == bIsCycleChange)
	{
		SetCycleAmperHour(0.0f, FuncAgrsEqual);
	}

	int nMsgCount = GetStepTime();

	//TRACE("%s = %d\n", __FUNCTION__, nMsgCount);

	if (nMsgCount <= 0)
		nMsgCount = 1; 

	int nMilSec = 100 * nMsgCount;

	double fCalcComm = 1.0;

	if (nMilSec > 0)
	{
		/*
		if (nMilSec > 3600000)
		fCalcComm = nMilSec / 3600000;
		else
		fCalcComm = 3600000 / nMilSec;
		*/

		fCalcComm = 3600000 / 100;
	}

	float fVoltage = GetVoltage();
	float fAmpere = GetAmpere();

	// 	if (fAmpere == 0.000)
	// 		fAmpere = 1.0f;

	float fWatt = fVoltage * fAmpere;
	double fAmpereHour = GetStepAmpereHour();
	double fWattHour = fWatt / fCalcComm;

	if (GetCommCommand() == eIFBoardCommandDischarge)
	{
		if (fAmpereHour > 0.000)
			fAmpereHour *= (-1.0f);

		if (fWattHour > 0.000)
			fWattHour *= (-1.0f);

		if (fWatt > 0.000)
			fWatt *= (-1.0f);
	}

	SetPower(fWatt);
	SetStepAmpereHour(fAmpereHour, FuncAgrsEqual);

	float fAmperHourTotal = 0.0;

	if (GetStepAmpereHour() < 0)
		fAmperHourTotal += GetStepAmpereHour()*-1 + GetStepAmpereHourTotal();
	else
		fAmperHourTotal += GetStepAmpereHour() + GetStepAmpereHourTotal();


	SetStepAmpereHourTotal(fAmperHourTotal);
	
	SetStepWattHour(fWattHour, FuncAgrsPlus);

	SetTimeData(nMsgCount);
	//TRACE("MsgCount : %d \n", nMsgCount);
	double fAbsAh = (double)abs(fAmpereHour);
	switch (GetCommCommand())
	{
	case eIFBoardCommandCharge: 
		SetScheduleAmpereHour(fAbsAh, FuncAgrsPlus);
		SetPatternAmpereHour(fAbsAh, FuncAgrsPlus);
		SetPatternTotalAmpereHour(fAbsAh, FuncAgrsPlus);
		SetCycleAmperHour(fAbsAh, FuncAgrsPlus);
		SetPatternTotalTime(GetTimeData(), FuncAgrsPlus);
		break; //패턴 일때 누적 충방 계산  break;
	case eIFBoardCommandDischarge: 
		SetScheduleAmpereHour(fAbsAh, FuncAgrsMinus);
		SetPatternAmpereHour(fAbsAh, FuncAgrsPlus);
		SetPatternTotalAmpereHour(fAbsAh, FuncAgrsPlus);
		SetCycleAmperHour(fAbsAh, FuncAgrsPlus);
		SetPatternTotalTime(GetTimeData(), FuncAgrsPlus); break;//패턴 일때 누적 충방 계산 break;
	default:break;
	}

	SetScheduleWattHour((double)abs(fWattHour), FuncAgrsEqual);

	switch (GetCommCommand())
	{
	case eIFBoardCommandCharge: 
		SetChargeAmpereHour(fAmpereHour, FuncAgrsEqual); 
		SetChargeWattHour(fWattHour, FuncAgrsEqual);
		SetPatternWattHour(fWattHour, FuncAgrsEqual); 
		break;
	case eIFBoardCommandDischarge: 
		SetDischargeAmpereHour(fAmpereHour, FuncAgrsEqual); 
		SetDischargeWattHour(fWattHour, FuncAgrsEqual); 
		SetPatternWattHour(fWattHour, FuncAgrsEqual); 
		break;
	default:break;
	}

	/*CString strMsg = "";
	strMsg.Format(_T("PatternAh - %.4f, PatternTotalAh - %.4f,  PatternWh - %.4f"), GetPatternAmpereHour(), GetPatternTotalAmpereHour(), GetPatternWattHour());
	if(GetPatternTotalAmpereHour() > 0)
	SYSLOG(Dbg, strMsg);*/

}

void	CCommData::InitWH()
{//스탭 종료시 초기화 해줘야 하는것들
	SetStepAmpereHour(0.0f, FuncAgrsEqual);
	SetPower(0.0f);
	SetStepWattHour(0.0f, FuncAgrsEqual);
	SetChargeAmpereHour(0.0f, FuncAgrsEqual);
	SetChargeWattHour(0.0f, FuncAgrsEqual);
	SetDischargeAmpereHour(0.0f, FuncAgrsEqual);
	SetDischargeWattHour(0.0f, FuncAgrsEqual);
	SetStepAmpereHourTotal(0.0f);
	SetDcir1(0);
	SetDcir2(0);
	SetDcir3(0);
	SetTimeData(0);

	m_fLastDcir1 = m_fLastDcir2 = m_fLastDcir3 = 0;
	
}

void	CCommData::InitCalcData()
{//스케줄 시작시 초기화 해줘야 하는 것들
	SetPower(0.0f);
	SetStepAmpereHour(0.0f, FuncAgrsEqual);
	SetPatternAmpereHour(0.0f, FuncAgrsPlus);
	SetPatternWattHour(0.0f, FuncAgrsPlus);
	SetCycleAmperHour(0.0f, FuncAgrsPlus);
	SetPatternTotalAmpereHour(0.0f, FuncAgrsEqual);
	SetPatternTotalTime(0, FuncAgrsPlus);
	SetStepWattHour(0.0f, FuncAgrsEqual);
	SetScheduleAmpereHour(0.0f, FuncAgrsEqual);
	SetChargeAmpereHour(0.0f, FuncAgrsEqual);
	SetChargeWattHour(0.0f, FuncAgrsEqual);
	SetDischargeAmpereHour(0.0f, FuncAgrsEqual);
	SetDischargeWattHour(0.0f, FuncAgrsEqual);
	SetScheduleWattHour(0.0f, FuncAgrsEqual);
	SetBeforStepTimeV(0);
	SetBeforStepTimeI(0);
	SetDIBefor(0);
	SetDVBefor(0);
	SetSOCValue(0);
	SetSOHValue(0);
}

int		CCommData::GetChannelNumber()
{
	return m_nChannel;
}

CString CCommData::GetChannelBarcodeStr()
{
	CString csChannel;
	csChannel.Format("%s", m_strChBarCode);
	return csChannel.GetString();
}

CString CCommData::GetChannelNumberStr()
{
	CString csChannel;
	csChannel.Format("%d",GetChannelNumber()+1);
	return csChannel.GetString();
}

CString	CCommData::GetStepTimeStr()
{
	CString csTime;
	csTime.Format("%.1f", float((GetStepTime())/10.0f));
	return csTime.GetString();
}

CString	CCommData::GetSchTotalTimeStr()
{
	CString csTime;
	csTime.Format("%.1f", float((GetSchTotalTime())/10.0f));
	return csTime.GetString();
}

CString CCommData::GetTempStr()
{
	CString csTemp;
	
	csTemp.Format("%.3f", GetTemp());

	return csTemp.GetString();
}

CString	CCommData::GetVoltageStr()
{
	CString csVol;

	if (CMgrConfig::GetMgr()->GetMicroV())
		csVol.Format("%.4f", GetVoltage());
	else
		csVol.Format("%.3f", GetVoltage());
	return csVol.GetString();
}

CString	CCommData::GetAmpereStr()
{
	CString csAmpere;
	if (CMgrConfig::GetMgr()->GetMicroA())
		csAmpere.Format("%.5f", GetAmpere());
	else
		csAmpere.Format("%.3f", GetAmpere());

	return csAmpere.GetString();
}

int		CCommData::GetStepType()
{
	return m_nType;
}

CString	CCommData::GetStepTypeStr()
{
	CString csStepType;

	switch (m_nType)
	{
	case eIFBoardStepTypeUnknown		: csStepType.Format("%s", _T("")); break;
	case eIFBoardStepTypeRest			: csStepType.Format("%s", _T("REST")); break;
	case eIFBoardStepTypeCharge			: csStepType.Format("%s", _T("CHARGE")); break;
	case eIFBoardStepTypeDischarge		: csStepType.Format("%s", _T("DISCHARGE")); break;
	case eIFBoardStepTypeExternalPattern: csStepType.Format("%s", _T("PATTERN")); break;
	case eIFBoardStepTypeOcv			: csStepType.Format("%s", _T("OCV")); break;
	case eIFBoardStepTypeImpedance		: csStepType.Format("%s", _T("Impedance"));	break;
	case eIFBoardStepTypePogoPinTest	: csStepType.Format("%s", _T("POGO PIN TEST"));	break;
	}

	return csStepType.GetString();
}

CString	CCommData::GetStepTypeFromDaQ()
{
	// By JSK - 2020.08.06 DAQ 화면에서 어떤 스텝인지 띄워달라는데.. 아무것도 진행 안해도 rest 로 뜬다..
	if (m_nStepIndex <= 0)
		return _T("");
	
	return GetStepTypeStr();
}

void	CCommData::SetPatternWattHour(double fWattHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fPatternWh = fWattHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fPatternWh += fWattHour;
}

void	CCommData::SetPatternAmpereHour(double fAmpereHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fPatternAh = fAmpereHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fPatternAh += fAmpereHour;
}	

void	CCommData::SetPatternTotalAmpereHour(double fAmpereHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fPatternTotalAh = fAmpereHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fPatternTotalAh += fAmpereHour;
}

void	CCommData::SetCycleAmperHour(double fAmpereHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fCycleAh = fAmpereHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fCycleAh += fAmpereHour;
}

void	CCommData::SetPatternTotalTime(int nTime, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_nPatternTime = nTime;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_nPatternTime += nTime; //TRACE("Time : %d\n", m_nPatternTime);
}

double	CCommData::GetStepAmpereHour()
{
	if (GetCommCommand() == eIFBoardCommandDischarge)
	{
		SetStepDisAmperHour(m_fStepAh);
	}
	else if (GetCommCommand() == eIFBoardCommandCharge)
	{
		SetStepCharAmperHour(m_fStepAh);
	}

	return m_fStepAh;

}

double	CCommData::GetStepAmpereHourTotal()
{
	return m_fStepAhTotal;
}

CString	CCommData::GetStepAmpereHourTotalStr()
{
	CString csAh;

	//오창2공장 셀장비는 데이터 로깅시 AH 양수표현(방전/충전 상관없이)
	if (GetStepAmpereHourTotal() < 0)
	{
		csAh.Format("%.6f", (GetStepAmpereHourTotal()*-1));
	}
	else
	{
		csAh.Format("%.6f", GetStepAmpereHourTotal());
	}

	return csAh.GetString();
}

CString	CCommData::GetStepAmpereHourStr()
{
	CString csAh;

	//오창2공장 셀장비는 데이터 로깅시 AH 양수표현(방전/충전 상관없이)
	if (GetStepAmpereHour() < 0)
	{
		csAh.Format("%.6f", (GetStepAmpereHour()*-1) );
	}
	else
	{
		csAh.Format("%.6f", GetStepAmpereHour());
	}
	
	return csAh.GetString();
}

void CCommData::SetPogoOhmMonitoring(float fPogoPinOhmMonitoring)
{
	m_fPogoPinOhm_Monitoring = fPogoPinOhmMonitoring;

	SetContactRegUptate(true);
}

void	CCommData::SetStepAmpereHour(double fAmpereHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fStepAh = fAmpereHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fStepAh += fAmpereHour;

// 	if (this->GetStepIndex() > 0)
// 	{
// //		CString strDebug;
// //		strDebug.Format(_T("step(%d) value(%f)\n"), this->GetStepIndex(), m_fStepAh);
// //		TRACE(strDebug.GetString());
// 	}
}

void	CCommData::SetStepAmpereHourTotal(double fAmpereHourTotal)
{
	m_fStepAhTotal = fAmpereHourTotal;
}

CString	CCommData::GetPowerStr()
{
	CString csWatt;
	csWatt.Format("%.6f", GetPower());
	return csWatt.GetString();
}

CString	CCommData::GetStepWattHourstr()
{
	CString csStepWh;
	csStepWh.Format("%.6f", GetStepWattHour());
	return csStepWh.GetString();
}

void	CCommData::SetStepWattHour(double fWattHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fStepWh = fWattHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fStepWh += fWattHour;
}

void	CCommData::SetRestVoltage(float fValue)
{
	if (eIFBoardCommandRest == GetCommCommand())
	{
		m_fRestVoltage = fValue;
	}
}

CString CCommData::GetDCIR(int nType)
{
	CString strValue;

	switch (nType)
	{
		case eDCIRTypeSection1	: strValue.Format("%.4f", m_fDcir1);	break;		//
		case eDCIRTypeSection2	: strValue.Format("%.4f", m_fDcir2); break;
		case eDCIRTypeSection3	: strValue.Format("%.4f", m_fDcir3); break;
		case eDCIRStepEnd1: strValue.Format("%.4f", m_fLastDcir1);	break;
		case eDCIRStepEnd2: strValue.Format("%.4f", m_fLastDcir2); break;
		case eDCIRStepEnd3: strValue.Format("%.4f", m_fLastDcir3); break;

	}
	
	return strValue;
}

void	CCommData::SetDcir3(float fvalue)
{
	if (GetOPCode() == eIFBoardOpCodeDischargeCC || GetOPCode() == eIFBoardOpCodeDischargeCV || GetOPCode() == eIFBoardOpCodeDischargeCP)
	{
		m_fDcir3 = fvalue*(-1);
	}
	else
	{
		m_fDcir3 = fvalue;
	}
}

void	CCommData::SetDcir2(float fvalue)
{
	if (GetOPCode() == eIFBoardOpCodeDischargeCC || GetOPCode() == eIFBoardOpCodeDischargeCV || GetOPCode() == eIFBoardOpCodeDischargeCP)
	{
		m_fDcir2 = fvalue*(-1);
	}
	else
	{
		m_fDcir2 = fvalue;
	}
}

void	CCommData::SetDcir1(float fvalue)
{
	if (GetOPCode() == eIFBoardOpCodeDischargeCC || GetOPCode() == eIFBoardOpCodeDischargeCV || GetOPCode() == eIFBoardOpCodeDischargeCP)
	{
		m_fDcir1 = fvalue*(-1);
	}
	else
	{
		m_fDcir1 = fvalue;
	}
}


CString	CCommData::GetScheduleAmpereHourStr()
{
	CString csScAmpere;
	csScAmpere.Format("%.6f", GetScheduleAmpereHour());
	return csScAmpere.GetString();
}

void	CCommData::SetScheduleAmpereHour(double fAmpereHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fSchAh = fAmpereHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fSchAh += fAmpereHour;
	else if (FuncAgrsMinus == nFuncAgrs)
		m_fSchAh -= fAmpereHour;
}

CString	CCommData::GetScheduleWattHourStr()
{
	CString csSchWh;
	csSchWh.Format("%.6f", GetScheduleWattHour());
	return csSchWh.GetString();
}

void	CCommData::SetScheduleWattHour(double fWattHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fSchWh = fWattHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fSchWh += fWattHour;
}

CString	CCommData::GetChargeAmpereHourStr()
{
	CString csChAh;
	csChAh.Format("%.6f", GetChargeAmpereHour());
	return csChAh.GetString();
}


void	CCommData::SetChargeAmpereHour(double fAmpereHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fChaAh = fAmpereHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fChaAh += fAmpereHour;
}

CString	CCommData::GetChargeWattHourStr()
{
	CString csfChaWh;
	csfChaWh.Format("%.6f", GetChargeWattHour());
	return csfChaWh.GetString();
}

void	CCommData::SetChargeWattHour(double fWattHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fChaWh = fWattHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fChaWh += fWattHour;
}

CString	CCommData::GetDischargeAmpereHourStr()
{
	CString csDisAh;
	csDisAh.Format("%.6f", GetDischargeAmpereHour());
	return csDisAh.GetString();
}

void	CCommData::SetDischargeAmpereHour(double fAmpereHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fDisAh = fAmpereHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fDisAh += fAmpereHour;
}

CString	CCommData::GetDischargeWattHourStr()
{
	CString str;
	str.Format("%.6f", GetDischargeWattHour());
	return str.GetString();
}

void	CCommData::SetDischargeWattHour(double fWattHour, int nFuncAgrs /*= FuncAgrsEqual*/)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_fDisWh = fWattHour;
	else if (FuncAgrsPlus == nFuncAgrs)
		m_fDisWh += fWattHour;
}


CString CCommData::GetChamberTemperatureStr()
{
	CString csChamber;
	csChamber.Format("%.3f", GetChamberTemperature());
	return csChamber.GetString();
}

CString	CCommData::GetCyclerChannelInfo(bool bStepEnd, bool bTotalCarType)
{
	std::vector<CString> vecValue;

	{
		// 1.시간
		vecValue.push_back(GetLogTimeStr());
	}

	{
		// 2. 채널
		vecValue.push_back(GetChannelNumberStr());
	}
		// Tray Barcode
	{
		if (CMgrSerial::GetMgr() != NULL)
		{
			vecValue.push_back(CMgrSerial::GetMgr()->GetAtBarcode(GetChannelNumber()/24)->GetTrayBarcode());
		}

	}
	//Channel Barcode
	{
		
			vecValue.push_back(GetChannelBarcodeStr());
	}


	{
		// 3. 종료 원인
		vecValue.push_back(GetEndReasonStr());
	}

	{
		// 4. 루프 횟수
		CString strValue;
		strValue.Format("%d", GetCycleIndex() + 1);

		vecValue.push_back(strValue);
	}

	{
		// 5. 총 루프 횟수
		CString strValue;
		strValue.Format("%d", GetTotalCycleLoop());

		vecValue.push_back(strValue);
	}

	{
		// 6. 사이클 번호
		vecValue.push_back(GetCycleNumberStr());
	}

	{
		// 7. 스텝 번호
		vecValue.push_back(GetStepNumberStr());
	}

	{
		// 8. 스텝 타입
		vecValue.push_back(GetStepTypeStr());
	}

	{
		// 9. 스텝 시간
		vecValue.push_back(GetStepTimeStr());
	}

	{
		// 10. 토탈 시간
		vecValue.push_back(GetSchTotalTimeStr());
	}

	{
		// 11. 온도	//220405YGYUN Log Data Temp 추가
		vecValue.push_back(GetTempStr());
	}


	{	// 12. 전압
		vecValue.push_back(GetVoltageStr());
	}

	{
		// 13. 전류
		vecValue.push_back(GetAmpereStr());
	}

	{
		// 14. 용량
		vecValue.push_back(GetStepAmpereHourStr());
	}

	{
		//////////////////////////////////////////////////////////////////////////////////
		//// 2021-05-14
		//CString strAmpere;
		//int ch = GetChannelNumber();

		//// 모든 Step Cap의 ?
		//if (GetStepAmpereHour() < 0)
		//	g_dTotalCapLog[ch] += GetStepAmpereHour()*-1;
		//else
		//	g_dTotalCapLog[ch] += GetStepAmpereHour();

		//strAmpere.Format(_T("%.6f"), g_dTotalCapLog[ch]);

		// Total End Log
		//if (bTotalCarType == true)
		//{
		//	// 모든 Step Cap의 ?
		//	if (GetStepAmpereHour()<0)
		//		g_dTotalCapLog[ch] += GetStepAmpereHour()*-1;
		//	else
		//		g_dTotalCapLog[ch] += GetStepAmpereHour();
		//	strAmpere.Format(_T("%.6f"), g_dTotalCapLog[ch]);
		//}
		// Total Step Log
		//else
		//{
		//	// Step 1,2,3 Cap의 합
		//	if (GetStepAmpereHour() < 0)
		//		g_dStepCapLog[ch][g_nTotalCapCalIndex[ch]] = GetStepAmpereHour() * -1;
		//	else
		//		g_dStepCapLog[ch][g_nTotalCapCalIndex[ch]] = GetStepAmpereHour();
		//	strAmpere.Format(_T("%.6f"), g_dStepCapLog[ch][0] + g_dStepCapLog[ch][1] + g_dStepCapLog[ch][2]);
		//	g_nTotalCapCalIndex[ch]++;
		//	if (g_nTotalCapCalIndex[ch] == 3)
		//		g_nTotalCapCalIndex[ch] = 0;
		//}

		// 15. 스케줄 용량
		vecValue.push_back(GetScheduleAmpereHourStr());
		//vecValue.push_back(GetStepAmpereHourTotalStr());
	}

	{
		// 16. 누적 충전 용량
		vecValue.push_back(GetChargeAmpereHourStr());
	}

	{
		// 17. 누적 방전 용량
		vecValue.push_back(GetDischargeAmpereHourStr());
	}

	{
		// 18. 전력
		vecValue.push_back(GetPowerStr());
	}

	{
		// 19. 전력량
		vecValue.push_back(GetStepWattHourstr());
	}

	{
		// 20. 스케줄 전력량
		vecValue.push_back(GetScheduleWattHourStr());
	}

	{
		// 21. 누적 충전 전력량
		vecValue.push_back(GetChargeWattHourStr());
	}

	{
		// 22. 누적 방전 전력량
		vecValue.push_back(GetDischargeWattHourStr());
	}

	{
		// 23. 챔버 온도 센싱 값
		vecValue.push_back(GetChamberTemperatureStr());
	}

	{
		// 24. DCIR 값
		if(bStepEnd == false)
			vecValue.push_back(GetDCIR(eDCIRTypeSection1));
		else
			vecValue.push_back(GetDCIR(eDCIRStepEnd1));
	}

	{
		// 25. 작업자 ID
		vecValue.push_back(m_strUserID);
	}

	{
		// 26. Cell ID
		vecValue.push_back(m_strCellID);
	}

	{
		// 27. 작업명
		vecValue.push_back(m_strWorkName);
	}

	{
		// 28. OpCode 
		CString strValue ="";
		switch (GetOPCode())
		{
			case eIFBoardOpCodeUnknown		: strValue = _T("Waiting");		break;
			case eIFBoardOpCodeInputRelayOn	: strValue = _T("Input MC Connect");	break;
			case eIFBoardOpCodeOutputReplyOn: strValue = _T("Output MC Connect");	break;
			case eIFBoardOpCodeChargeCC		: strValue = _T("Charge CC");		break;
			case eIFBoardOpCodeChargeCV		: strValue = _T("Charge CV");		break;
			case eIFBoardOpCodeDischargeCC	: strValue = _T("Discharge CC");		break;
			case eIFBoardOpCodeDischargeCV	: strValue = _T("Discharge CV");		break;
			case eIFBoardOpCodeChargeCP		: strValue = _T("Charge CP");		break;
			case eIFBoardOpCodeDischargeCP	: strValue = _T("Discharge CP");		break;
		}
		if (strValue.IsEmpty())
			strValue = _T(",");
		vecValue.push_back(strValue);
	}

	//220609 YGYUN Formation PLC 통신 Vaccum Log Law Data 추가
	//29.VaccumValue (PLC Interface 사용 안 할 경우 조건 처리 필)
	CMgrPlcInterface* pMgrPlcInterface = CMgrPlcInterface::GetMgr();

	if (pMgrPlcInterface != NULL)
	{
		vecValue.push_back(pMgrPlcInterface->Get_VaccumValueKPAStr());
	}
	
	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	CString strCyclerChannelInfo = "";

	while (iter != iterE)
	{
		strCyclerChannelInfo += (*iter);
		strCyclerChannelInfo += ",";

		++iter;
	}

	strCyclerChannelInfo.TrimRight(",");
	//strCyclerChannelInfo += "\n";



	return strCyclerChannelInfo;
}

CString	CCommData::GetCycleNumberStr()
{
	CString csCycleNo;
	csCycleNo.Format("%d", GetCycleNumber());
	return csCycleNo.GetString();
}

CString	CCommData::GetStepNumberStr()
{
	CString csStepNo;
	csStepNo.Format("%d", GetStepNumber());
	return csStepNo.GetString();
}

void	CCommData::IncreaseMsgCount()
{

	m_nStepTime++;
	m_nTotalTime++;
	m_nCycleTime++;




	//TRACE("StepTime : %d\n", m_nStepTime);

	//// 작업 명세서
	//CString specificationPath = _T("");
	//specificationPath.Format(_T("%s\\Config\\%s"), GetExePath(), "Specification");
	//::CreateDirectory(specificationPath, NULL);

	//CString temp;
	//temp.Format(_T("%s\\CH%02d.ini"), specificationPath, m_nChannel + 1);
	//specificationPath = temp;

	//INIWriteInt("Specification", "StepTime", m_nStepTime, specificationPath);
	//INIWriteInt("Specification", "TotalTime", m_nTotalTime, specificationPath);

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);
	pChannel->_workLogger.WriteIntValue("CycleTime", m_nCycleTime);
	pChannel->_workLogger.WriteIntValue("StepTime", m_nStepTime);
	pChannel->_workLogger.WriteIntValue("TotalTime", m_nTotalTime);
}

void CCommData::WriteLogCount(int nCount /*= -1*/)
{
	if (nCount == -1)
		m_nWriteLogCount++;
	else if (nCount == 0)
		m_nWriteLogCount = 0;
	else if (nCount > 0)
		m_nWriteLogCount = nCount;
}

CString CCommData::GetPogoOhmMonitoringstr()
{
	CString strPogoOhmMonitoring;

	strPogoOhmMonitoring.Format("%.3f", GetPogoOhmMonitoring());

	return strPogoOhmMonitoring;
}
CString CCommData::GetPogoOhmSettingstr()
{
	CString strPogoOhmSetting;

	strPogoOhmSetting.Format("%.3f", GetPogoOhmSetting());

	return strPogoOhmSetting;
}
CString CCommData::GetChBarcode()
{ 
	return m_strChBarCode; 
}
CString	CCommData::GetEndReasonStr()
{
	if (m_nEndReason < 0)
		return _T("");

	if (m_nEndReason >= eIFBoardCyclerCodeEmpty && m_nEndReason < eIFBoardCyclerCodeMaxCount)
	{
		/*if (m_nEndReason == eIFBoardCyclerCodePause)
		{
			return g_lpszMonitoringCode[g_nAlarmStatus[m_nChannel] - eMonitroingCyclerCodeSystemSafetyVoltageMax];
		}
		else*/
			return g_lpszIFBoardCode[m_nEndReason];
	}

	else if (m_nEndReason >= eMonitroingCyclerCodeSystemSafetyVoltageMax && m_nEndReason < eMonitroingCyclerCodeMaxCount)
	{
		CString retString;
		if (m_nEndReason == eMonitroingCyclerCodeSoC || m_nEndReason == eMonitroingCyclerCodeSoE)
		{
			CStringArray arr;
			CPnxUtil::GetMgr()->SplitString(g_lpszMonitoringCode[m_nEndReason - eMonitroingCyclerCodeSystemSafetyVoltageMax], '/', arr);
			if (arr.GetCount() > 2)
			{
				int nStepType = GetStepType();

				if (nStepType == eIFBoardStepTypeCharge)
					retString.Format(_T("%s %d"), arr.GetAt(0), g_nRatioSpec[m_nChannel]);
				else if(nStepType == eIFBoardStepTypeDischarge)
					retString.Format(_T("%s %d"), arr.GetAt(1), g_nRatioSpec[m_nChannel]);
				else
					retString.Format(_T("%s %d"), arr.GetAt(2), g_nRatioSpec[m_nChannel]);
			}
			else
				retString.Format(_T("%s %d"), g_lpszMonitoringCode[m_nEndReason - eMonitroingCyclerCodeSystemSafetyVoltageMax], g_nRatioSpec[m_nChannel]);
		}
		else
			retString = g_lpszMonitoringCode[m_nEndReason - eMonitroingCyclerCodeSystemSafetyVoltageMax];

		return retString;
	}

	return _T("");
}

void	CCommData::CopyData(CCommData* pSrc, BOOL bCopyTime /*= TRUE*/)
{
	m_nType				= pSrc->m_nType;
	m_nCycleNo			= pSrc->m_nCycleNo;
	m_nStepNo			= pSrc->m_nStepNo;
	m_nCycleIndex		= pSrc->m_nCycleIndex;
	m_nStepIndex		= pSrc->m_nStepIndex;
	m_nTotalCycleLoop	= pSrc->m_nTotalCycleLoop;
	m_nTotalCount		= pSrc->m_nTotalCount;
	m_nPatternTime		= pSrc->m_nPatternTime;
	m_nSOCValue			= pSrc->m_nSOCValue;
	m_nSOHValue			= pSrc->m_nSOHValue;
	m_nLogType			= pSrc->m_nLogType;
	m_nLogChannelNumber = pSrc->m_nLogChannelNumber;
	m_nChannel			= pSrc->m_nChannel;
	m_nOpcode			= pSrc->m_nOpcode;
	m_nCommCmd			= pSrc->m_nCommCmd;
	m_nDataFrameCount	= pSrc->m_nDataFrameCount;
	m_nEndReason		= pSrc->m_nEndReason;
	m_nTimeData			= pSrc->m_nTimeData;
	m_nTimeData2		= pSrc->m_nTimeData2;
	m_nProcessStatus	= pSrc->m_nProcessStatus;
	m_fChamberTemp		= pSrc->m_fChamberTemp;
	m_fChamberTempSp	= pSrc->m_fChamberTempSp;
	m_fRestVoltage		= pSrc->m_fRestVoltage;
	//m_fDcir				= pSrc->m_fDcir;
	m_fDcir1			= pSrc->m_fDcir1;
	m_fDcir2			= pSrc->m_fDcir2;
	m_fDcir3			= pSrc->m_fDcir3;
	m_fLastDcir1		= pSrc->m_fLastDcir1;
	m_fLastDcir2		= pSrc->m_fLastDcir2;
	m_fLastDcir3		= pSrc->m_fLastDcir3;
	m_fVolt				= pSrc->m_fVolt;
	m_fCurr				= pSrc->m_fCurr;
	m_fTemp				= pSrc->m_fTemp;
	m_fPower			= pSrc->m_fPower;
	m_fPatternWh		= pSrc->m_fPatternWh;
	m_fPatternAh		= pSrc->m_fPatternAh;
	m_fPatternTotalAh	= pSrc->m_fPatternTotalAh;
	m_fCycleAh			= pSrc->m_fCycleAh;
	m_fStepAh			= pSrc->m_fStepAh;
	m_fStepAhTotal      = pSrc->m_fStepAhTotal;
	m_fDisStepAh		= pSrc->m_fDisStepAh;
	m_fCharStepAh		= pSrc->m_fCharStepAh;
	m_fSchAh			= pSrc->m_fSchAh;
	m_fChaAh			= pSrc->m_fChaAh;
	m_fDisAh			= pSrc->m_fDisAh;	
	m_fStepWh			= pSrc->m_fStepWh;
	m_fSchWh			= pSrc->m_fSchWh;
	m_fChaWh			= pSrc->m_fChaWh;
	m_fDisWh			= pSrc->m_fDisWh;
	m_strTime			= pSrc->m_strTime;
	m_strUserID			= pSrc->m_strUserID;
	m_strCellID			= pSrc->m_strCellID;
	m_strWorkName		= pSrc->m_strWorkName;
	m_strChBarCode		= pSrc->m_strChBarCode;

    m_nRatioType = pSrc->m_nRatioType;
	if (bCopyTime)
	{
		m_nCycleTime	= pSrc->m_nCycleTime;
		m_nStepTime		= pSrc->m_nStepTime;
		m_nTotalTime	= pSrc->m_nTotalTime;
		m_strTime		= pSrc->m_strTime;
	}

	POSITION pos = NULL;
	pos = pSrc->m_listDAQ.GetHeadPosition();
	while (pos)
	{
		this->m_listDAQ.AddTail(pSrc->m_listDAQ.GetNext(pos));
	}
	
}

bool	CCommData::CheckEQStepChange(int nStepIndexIFBoard, int nLoopCount)
{
//#pragma region 패턴 시간 누적 기능
	//스탭이 변경 됬을시 return true;
	if (this->m_nStepIndex != nStepIndexIFBoard)
	{
		//스탭이 패턴인지 구분하여야한다. 
		//패턴이라면 시간 누적, 아니라면 시간초기화 및 스탭변경 YJK
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);
		if (pChannel != NULL)
		{
			CStep* pStep = pChannel->GetMgr()->GetAtStep(nStepIndexIFBoard);

			//만약 외부 패턴이라면
			if (pStep != NULL && pStep->GetStepPattern() == eIFBoardStepTypeExternalPattern)
			{
				//첫번째 패턴일 경우 시간 한번 초기화 후 변수 true로 변경
				if (pChannel->GetExternPatternStatus() == false)
				{
					pChannel->SetExternPatternStatus(true);
					return true;
				}
				//두번째부터 패턴일경우 시간 초기화 하지않음
				else
				{
					return false;
				}

			}
			else
			{
				pChannel->SetExternPatternStatus(false);
				return true;
			}
		}

		return true;
	}
//#pragma endregion

	//스탭이 변경 됬을시 return true;
	/*if (this->m_nStepIndex != nStepIndexIFBoard)
	{
		return true;
	}*/
	//단일 사이클, 단일 스탭인데 루프가 있고 루프 돌때마다 스탭이 변경 됬다고 봐야 한다
	//였다가 단일 스탭인 경우 루프 도는거 시험시작 못하도록 막음 (FW에서 아직 대응이 안됨)
	else if (this->m_nStepIndex == nStepIndexIFBoard)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);
		
		if (pChannel != NULL)
		{
			CStep* pStep = pChannel->GetMgr()->GetAtStep(nStepIndexIFBoard);

			if (pStep != NULL)
			{
				if (pStep->GetCycleLoopCount() > 1 && (nLoopCount != pChannel->GetCycleIndex()))
				{
					SYSLOG(Dbg, _T("CH(%d) Step Change..before(%d) after(%d)"), m_nChannel + 1, this->m_nStepIndex, nStepIndexIFBoard);
					return true;
				}
			}
		}
	}

	return false;
}

bool	CCommData::CheckEQCycleChange(int nStepIndexIFBoard)
{
	//사이클이 변경 됬을시 return true;
	int nPrvCycleNumber = CalcUICycleNumber(nStepIndexIFBoard);

	if (this->m_nCycleNo != nPrvCycleNumber)
	{
		return true;
	}

	return false;
}

bool	CCommData::CheckEQScheduleComplete(int nStepIndexIFBoard, int nEndReason, int nLoopCount, bool bLastCycle)
{
	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);

	if (pChannel == NULL)
	{
		return false;
	}

	if (nEndReason < 0)
	{
		return false;
	}

	if (nStepIndexIFBoard == 0)
	{
		return false;	//주석 이었는데.. 확인 필요
	}

	if ((eIFBoardCyclerCodeMaxCount < nEndReason || nEndReason >= eIFBoardCyclerCodePause))
	{
		return false;
	}

	if (nEndReason == eIFBoardCyclerCodeComplete ||  nEndReason == eIFBoardCyclerCodeAlarm)
	{//왜 즉시완료가 됬는지 세분화 필요 --> CalcEndReason() 에서 진행
		SYSLOG(Dbg, _T("ch(%d) schedule complete(complete9 or alarm) curstep(%d) endreason(%d)"), m_nChannel + 1, nStepIndexIFBoard, nEndReason);
		return true;
	}
	//추가 사이클이 있는지 확인필요
	if (nEndReason == eIFBoardCyclerCodeCycleVoltageCut || nEndReason == eIFBoardCyclerCodeCycleLowVoltageCut || nEndReason == eIFBoardCyclerCodeCycleTimeCut || nEndReason == eIFBoardCyclerCodeCycleCapacityCut || nEndReason == eIFBoardCyclerCodeCycleWattCut)
	{
		//추가사이클이없다면 종료 retrun true YJK
		
		if (bLastCycle ==true)
		{
			return true;
		}
		
	}
	if (nEndReason > 0)
	{
		//get current step info
		CStep* pStep = pChannel->GetMgr()->GetAtStep(nStepIndexIFBoard);

		if (pStep == NULL)
		{
			SYSLOG(Dbg, _T("[FATAL ERROR ]CH(%d) void CCommData::CheckEQScheduleComplete... pstep is null"), m_nChannel + 1);
			return false;
		}

		//serch schedule last index(step)
		int nScheduleLastIndex = pChannel->GetMgr()->GetLastStepIndex()/*return i/fBoard index*/ - 1/*-1 is dummy index..OTL*/;

		if (nStepIndexIFBoard == nScheduleLastIndex)
		{
			//check loop count
			if (nLoopCount/*fw use 0base*/ >= pStep->GetCycleLoopCount() - 1/*fw use 0base*/)
			{
				//check go to cycle
				if (pStep->GetLoopNextCycle() < nStepIndexIFBoard)
				{
					return false;
				}

				SYSLOG(Dbg, _T("CH(%d) Schedule Complete(normal) curstep(%d) xmllaststep(%d)"), m_nChannel + 1, nStepIndexIFBoard, nScheduleLastIndex);
				return true;
			}
		}
		if (pChannel->GetChannelEnd())
		{
			return false;
		}
 		//if (nStepIndexIFBoard == 65535)
 		//{
 		//	//check loop count
 		//	if (nLoopCount/*fw use 0base*/ >= pStep->GetCycleLoopCount() - 1/*fw use 0base*/)
 		//	{
 		//		//check go to cycle
 		//		if (pStep->GetLoopNextCycle() < nStepIndexIFBoard)
 		//		{
 		//			return FALSE;
 		//		}
			//	SYSLOG(Dbg, _T("CH(%d) Schedule Complete(normal) curstep(%d) xmllaststep(%d)"), m_nChannel + 1, nStepIndexIFBoard, nScheduleLastIndex);

 		//		//LOG(Dbg, _T("CH(%d) Schedule Complete(normal) curstep(%d) xmllaststep(%d)"), m_nChannel + 1, nStepIndexIFBoard, nScheduleLastIndex);
 		//		return TRUE;
 		//	}
 		//}
	}

	return false;
}

bool CCommData::CheckEQPause(int nEndReason)
{
	if ((eIFBoardCyclerCodeMaxCount < nEndReason || nEndReason >= eIFBoardCyclerCodePause))
	{
		return true;
	}

	return false;
}

int CCommData::CalcLoopCount(int nEndReason, int nStepIndex, int nRecvLoopCount)
{
	//	기존 스탭 피니시에서 했던 작업들임..
	//	스탭 피니시는 엔드리즌 들어올때마다 하니 여기서 계산 해줘야함 (같은 스탭인데 loopcount가 다르게 들어오기때문..)
	//	fw으로 부터 recv 되는 해당 값이 개선이 되면 이함수는 쓸 필요가 없다
	//	fw가 스탭 끝날때 마다 주는 정보 (EX 스케줄)
	// 	1cycle (setting loop 3)
	// 		1step
	// 		2step
	// 		3step
	// 	2cycle (setting loop 2)
	// 		1step
	// 		2step
	// 		3step
	//	loop count가 사이클의 제일 마지막 스탭에서 +1되서 들어온다..
	//	loop count 조건 만족시 다시 0으로...후,,
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(1) nCurrentLoopCount(0)	0
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(2) nCurrentLoopCount(0)	0
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(3) nCurrentLoopCount(1)	0
	// 
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(1) nCurrentLoopCount(1)	1
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(2) nCurrentLoopCount(1)	1
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(3) nCurrentLoopCount(2)	1
	// 
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(1) nCurrentLoopCount(2)	2
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(2) nCurrentLoopCount(2)	2
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(3) nCurrentLoopCount(0)	2
	// 
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(4) nCurrentLoopCount(0)	0
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(5) nCurrentLoopCount(0)	0
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(6) nCurrentLoopCount(1)	0
	// 
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(4) nCurrentLoopCount(1)	1
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(5) nCurrentLoopCount(1)	1
	// 	StepFinish() : Ch(1) nReasonCode(1) nStepIndex(6) nCurrentLoopCount(0)	1

	int nCalcData = 0;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);

	if (pChannel == NULL)
	{
		return nCalcData;
	}

	nCalcData = nRecvLoopCount;

	if (nStepIndex > 0)
	{
		if (pChannel->GetMgr()->GetCycleLastIndex(nStepIndex) == nStepIndex)
		{//사이클의 제일 마지막 스탭일경우만 검사

			CStep* pStep = pChannel->GetMgr()->GetAtStep(nStepIndex);

			if (pStep != NULL)
			{
				if ((pStep->GetCycleLoopCount() - 1/*fw use 0base..*/ == pChannel->GetCycleIndex()) &&
					nEndReason > 0 &&
					nCalcData == 0)
				{	// 1. 사이클 제일 마지막 스탭이 루프 만족에 해당, 즉 사이클 완전 끝
					//	  fw에서 nLoopcount를 0 으로 초기화 해서 줌

					nCalcData = pChannel->GetCycleIndex();	//TRACE("1  %d\n", nCalcData);
				}
				else if ((pStep->GetCycleLoopCount() - 1/*fw use 0base..*/ > pChannel->GetCycleIndex()) &&
					nEndReason > 0 &&
					nCalcData != 0)
				{	// 2. 사이클 제일 마지막 스탭이 루프 불만족에 해당, 즉 사이클 진행 중
					//    fw에서 nLoopcount +1 해서줌

					nCalcData -= 1;	//TRACE("2  %d\n", nCalcData);
				}
				else
				{	// 경우3. 종료되지 않은 스탭의 데이터는 그대로 쓴다
					// fw에서 nLoopcount 그대로 줌
					//				TRACE("3  %d skip\n", nCalcData);
				}
			}
			else
			{
				int i = 0;
				i = 1;
			}
		}
	}

	return  nCalcData;	//0base
}

int CCommData::CalcUIStepNumber(int nStepIndex)
{
	int nStepNumber = 0;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);

	if (pChannel == NULL)
	{
		return nStepNumber;
	}

	CMgrSchedule* pSchedule = pChannel->GetMgr();

	if (pSchedule == NULL)
	{
		return nStepNumber;
	}

	CStep* pStep = pChannel->GetMgr()->GetAtStep(nStepIndex);

	if (pStep == NULL)
	{
		return nStepNumber;
	}
	
	nStepNumber = pStep->GetStepNumber();
	
	return nStepNumber;
}

int CCommData::CalcUICycleNumber(int nStepIndex)
{
	int nCycleNumber = 0;

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);

	if (pChannel == NULL)
	{
		return nCycleNumber;
	}

	CMgrSchedule* pSchedule = pChannel->GetMgr();

	if (pSchedule == NULL)
	{
		return nCycleNumber;
	}

	CStep* pStep = pChannel->GetMgr()->GetAtStep(nStepIndex);

	if (pStep == NULL)
	{
		return nCycleNumber;
	}

	nCycleNumber = pStep->GetCycleNumber();

	return nCycleNumber;
}

int CCommData::CalcEndReason(int nEndReason, int nStepIndex)
{
	int nResult = nEndReason;

	if (nResult == eIFBoardCyclerCodeJump ||
		nResult == eIFBoardCyclerCodeComplete/*||
		nResult == ENDREASON_CODE_ALARAM	||	nResult == ENDREASON_CODE_PAUSE*/)
	{
		//SW에서 체킹하여 스탭을 종료 시키거나 점프시켜야 하는 상황이 있을시
		//그 이유에 대해 밖아(?) 주자

		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);

		if (pChannel == NULL)
		{
			SYSLOG(Dbg, _T("[FATAL ERROR] int CCommData::CalcEndReason(int nEndReason, int nStepIndex)"));
			return nResult;
		}

		if (pChannel->GetAccurateReason() != 0)
		{
			nResult = pChannel->GetAccurateReason();
		}

		SYSLOG(Dbg, _T("ch(%d) calc endreason before(%d) after(%d)"), m_nChannel + 1, nEndReason, nResult);
	}

	if (nStepIndex == 0 && nEndReason == 1)
	{//스탭인덱스가 0인데 시험 시작시 엔드리즌이 1로해서 들어온다ㅋㅋ
		nResult = 0;
	}

	return nResult;
}

void CCommData::CalcRunningTime(int nStepIndex, bool bIsStepChange, bool bIsPause)
{
	int CycleTime = -1;
	int stepTime = -1;
	int totalTime = -1;
	int CycleNo = -1;
	int CycleIndex = -1;
	int StepNo = -1;
	int StepIndex = -1;

	if (_bFirst)
	{
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);
		auto endCondition = pChannel->_workLogger.GetStringValue("EndCondition");

		if (endCondition.IsEmpty()) 
		{
			CycleTime = pChannel->_workLogger.GetIntValue("CycleTime");
			stepTime = pChannel->_workLogger.GetIntValue("StepTime");
			totalTime = pChannel->_workLogger.GetIntValue("TotalTime");
			CycleNo = pChannel->_workLogger.GetIntValue("CycleNumber");
			CycleIndex = pChannel->_workLogger.GetIntValue("CycleIndex");

			StepNo = pChannel->_workLogger.GetIntValue("StepNumber");
			StepIndex = pChannel->_workLogger.GetIntValue("StepIndex");




			SetCycleTime(CycleTime);
			SetStepTime(stepTime);
			SetTotalTime(totalTime);
			SetCycleNumber(CycleNo);
			SetCycleIndex(CycleIndex);

			SetStepNumber(StepNo);
			SetStepIndex(StepIndex);
		}

		_bFirst = false;
	}
	if (true == bIsPause)
	{
		return;
	}
	if (nStepIndex > 0 && true == bIsStepChange)
	{
		if (stepTime > 0)
		{

		}
		else
		{
			if (m_bSKipStepTimeInit == true)
			{
				m_bSKipStepTimeInit = false;
			}
			else
			{
				CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);
				if (pChannel->GetChannelStatus() != eCyclerChannelStatePause)
				{
					SetStepTime(0);
				}
			}

		}
	}

	if (nStepIndex > 0 && nStepIndex != 65535)
	{
		IncreaseMsgCount();		//step time, total time

		WriteLogCount();
	}
}

float CCommData::IsReverseVoltage(/*float fAvgVoltage*/)
{
	// Add By JSK - 2018.07.23 역삽입 시에는 Voltage 값이 마이너스로..
	if (TRUE == CMgrAlarm::GetMgr()->ExistChannelAlarm(m_nChannel))
	{
		CAlarmMap* mapAlarm = CMgrAlarm::GetMgr()->GetAlarmMap(m_nChannel/*nChannelNum*/);

		if (mapAlarm)
		{
			CAlarmList* pAlarmList = mapAlarm->GetAlarmList(eAlarmTypeIFWarning);

			if (pAlarmList != NULL)
			{
				CAlarm* pAlarm = pAlarmList->GetAlarm(0);

				if (pAlarm != NULL)
				{
					if (pAlarm->GetAlarmStr().CompareNoCase(_T("Cell Reverse")) == 0)
					{
						/*fAvgVoltage *= */return -1.0f;
					}
				}
			}
		}
	}
	else
	{
		//fAvgVoltage = abs(fAvgVoltage);
	}

	return 1.0;
}
//
//void CCommData::CalcDCIR()
//{
////주의 : 전단에 사용 값 들이 업데이트 먼저 되어야 함
// 	if (eIFBoardCommandCharge == GetCommCommand()|| eIFBoardCommandDischarge == GetCommCommand())
//	{
//		//20181213 hsm DCIR 무조건 구해야함
//
//		//taek 일단 10의 배수 일 때만 구하는 걸로
//		//지금은 해당 시간 까지의 값에서 나누는 걸로 했음
//
//		int nCount = 0;
//		nCount = int((GetStepTime()) / 10.0f);
//		if (nCount % 10 == 0)
//		{
//			if (abs(GetAmpere()) > 0)
//			{
//				double dAvg = m_fAvgAmpere / GetStepTime();
//				SetDcir(abs((GetStepEndVoltage() - GetVoltage()) / dAvg));
//				//SetDcir(abs((GetRestVoltage() - GetVoltage()) / GetAmpere()));
//			}
//			else
//			{
//				SetDcir(0);
//			}
//		}
//		else
//		{
//			SetDcir(0);
//		} //taek 20200520 구하지 않음
//	}
//}

void CCommData::CalcStepDCIR(int nStepIndex) 
{
	//주의 : 전단에 사용 값 들이 업데이트 먼저 되어야 함
	if (eIFBoardCommandCharge == GetCommCommand()|| eIFBoardCommandDischarge == GetCommCommand())
	{
		//20181213 hsm DCIR 무조건 구해야함
					
		CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);
		 
		if (NULL == pChannel)
			return;
		 
		CStep* pStep = pChannel->GetMgr()->GetAtStep(nStepIndex);
		
		if (NULL == pStep)
			return;

		std::vector<SDCIRLog*>& vecDCIRLog = pStep->GetDCIRLog();

		auto iter = vecDCIRLog.begin();
		auto iterE = vecDCIRLog.end();

		while (iter != iterE)
		{
			SDCIRLog* lpDCIR = (*iter);

			if (lpDCIR)
			{
				if (lpDCIR->nDCIR1 > 0)
				{
					double dCount = 0;
					dCount = double((GetStepTime()) / 10.0f); //pnx_hr 2021/05/26 평균전류 구해야함! 					

					if (dCount == lpDCIR->nDCIR1)
					{
						if (abs(GetAmpere()) > 0)
						{
							float  fAvg = m_fStepTotalAmpere / GetStepTime() ;
														
							if (fAvg == 0 || GetStepEndVoltage() == 0)
								m_fDcir1 = 0;
							else
							{
								m_fDcir1 = abs((GetStepEndVoltage() - GetVoltage()) / fAvg); //pnx_hr 2021.05.26 전류는 평균전류 RestVoltage는 이전스텝의 마지막 전압 
								m_fDcir1 *= 1000;
								m_fLastDcir1 = m_fDcir1;
							}

						/*	CString strTemp;
							strTemp.Format("CalcStepDCIR : endVolt - %f, volt - %f, fAvg - %f, dcir - %f", GetStepEndVoltage(), GetVoltage(), fAvg, m_fDcir1);

							SYSLOG(Dbg, strTemp);*/
						}
						else
						{
							SetDcir1(0);
						}
					}
					else
					{
						SetDcir1(0);
					}
				}
				if (lpDCIR->nDCIR2 > 0)
				{
					int nCount = 0;
					nCount = int((GetStepTime()) / 10.0f);
					if (nCount == lpDCIR->nDCIR2)
					{
						if (abs(GetAmpere()) > 0)
						{
							float  fAvg = m_fStepTotalAmpere / GetStepTime();
							//SetDcir2(abs((GetRestVoltage() - GetVoltage()) / GetAmpere()));
							m_fDcir2 = abs((GetStepEndVoltage() - GetVoltage()) / fAvg); //pnx_hr 2021.05.26 전류는 평균전류 RestVoltage는 이전스텝의 마지막 전압 
							m_fDcir2 *= 1000;
							m_fLastDcir2 = m_fDcir2;
						}
						else
						{
							SetDcir2(0);
						}
					}
					else
					{
						SetDcir2(0);
					}
				}
				if (lpDCIR->nDCIR3 > 0)
				{
					int nCount = 0;
					nCount = int((GetStepTime()) / 10.0f);
					if (nCount == lpDCIR->nDCIR3)
					{
						if (abs(GetAmpere()) > 0)
						{
							float  fAvg = m_fStepTotalAmpere / GetStepTime();
							//SetDcir3(abs((GetRestVoltage() - GetVoltage()) / GetAmpere()));
							m_fDcir3 = abs((GetStepEndVoltage() - GetVoltage()) / fAvg); //pnx_hr 2021.05.26 전류는 평균전류 RestVoltage는 이전스텝의 마지막 전압 
							m_fDcir3 *= 1000;
							m_fLastDcir3 = m_fDcir3;
						}
						else
						{
							SetDcir3(0);
						}
					}
					else
					{
						SetDcir3(0);
					}
				} //taek 200520
			}

			++iter;
		}
	}
}

void CCommData::InitalizeReady()
{
	InitWH();
	InitCalcData();
	SetCycleIndex(0);
	WriteLogCount(0);
	SetStepTime(0);
	RefreshMsgTotalCount();
	SetCycleTime(0);
	m_bSKipStepTimeInit = false;
	m_bSKipCycleTimeInit = false;
}

bool CCommData::IsStatusIdle()
{
	if (GetVoltage() <= CMgrConfig::GetMgr()->GetConnectVolt() &&
		(GetStepIndex() == 0|| GetStepIndex() == 65535) &&
		GetEndReason() == 0)
	{
		return true;
	}

	return false;
}

bool CCommData::IsStatusReady()
{
	if (GetVoltage() >= CMgrConfig::GetMgr()->GetConnectVolt() &&
		(GetStepIndex() == 0|| GetStepIndex() == 65535) &&
		GetEndReason() == 0)
	{
		////20220325 YGYUN Moxa 로 PLC와 통신하는 경우 
		//if (CMgrConfig::GetMgr()->m_strDioType == "MOXA")
		//{
		//	//Ready Bit On
		//	CMgrDio* pMgrDio = CMgrDio::GetMgr();

		//	pMgrDio->SetMoxa_PC_Ready(true);
		//}
		return true;
	}

	return false;
}

bool CCommData::IsStatusStart()
{
	if (GetVoltage() >= CMgrConfig::GetMgr()->GetConnectVolt() &&
		GetStepIndex() > 0 && GetStepIndex() != 65535&&
		(eIFBoardCyclerCodePause > GetEndReason() && GetEndReason() < eIFBoardCyclerCodePause))
	{
		////20220325 YGYUN Moxa 로 PLC와 통신하는 경우 
		//if (CMgrConfig::GetMgr()->m_strDioType == "MOXA")
		//{
		//	//Busy Bit On
		//	CMgrDio* pMgrDio = CMgrDio::GetMgr();
		//	pMgrDio->SetMoxa_PC_Busy(true);
		//}
		return true;
	}

	return false;
}

bool CCommData::IsStatusPause()
{
	/*
	if (GetStepIndex() > 0 &&
		GetEndReason() == eIFBoardCyclerCodePause && (GetEndReason() <= eIFBoardCyclerCode26 || GetEndReason() >= eIFBoardCyclerCode21))
		*/
	if(GetStepIndex() > 0 && eIFBoardCyclerCodePause == GetEndReason())
	{
		////20220325 YGYUN Moxa 로 PLC와 통신하는 경우 
		//if (CMgrConfig::GetMgr()->m_strDioType == "MOXA")
		//{
		//	//Ready Bit On
		//	CMgrDio* pMgrDio = CMgrDio::GetMgr();

		//	pMgrDio->SetMoxa_PC_Busy(false);
		//}
		return true;
	}

	return false;
}

bool CCommData::IsStatusPauseSungOn()
{
	if (GetStepIndex() > 0 &&
		GetEndReason() == eIFBoardCyclerCodeRisingChamberSingle)
	{
		return true;
	}

	return false;
}

bool CCommData::IsStatusPauseStep()
{
	if (GetStepIndex() > 0 &&
		GetEndReason() == eIFBoardCyclerCodeStepSync)
	{
		return true;
	}

	return false;
}

bool CCommData::IsStatusPauseTemp()
{
	if (GetStepIndex() > 0 &&
		(eIFBoardCyclerCodeRisingChamberMulti == GetEndReason()))
	{
		return true;
	}

	return false;
}

bool CCommData::IsStatusPauseDelay()
{
	if (GetStepIndex() > 0 &&
		((eIFBoardCyclerCodeDwellSingle == GetEndReason() || eIFBoardCyclerCodeDwellMulti == GetEndReason())))
	{
		return true;
	}

	return false;
}

bool CCommData::IsStatusComplete()
{
	if (GetStepIndex() > 0 &&
		GetEndReason() != eIFBoardCyclerCodeAlarm &&
		true == GetScheduleComplete()&& GetVoltage() >= CMgrConfig::GetMgr()->GetConnectVolt())
	{
		////20220325 YGYUN Moxa 로 PLC와 통신하는 경우 
		//if (CMgrConfig::GetMgr()->m_strDioType == "MOXA")
		//{
		//	//Ready Bit On
		//	CMgrDio* pMgrDio = CMgrDio::GetMgr();

		//	pMgrDio->SetMoxa_PC_WorkComplete(true);
		//	Sleep(500);
		//	pMgrDio->SetMoxa_PC_WorkComplete(false);
		//	Sleep(50);
		//	pMgrDio->SetMoxa_PC_Busy(false);

		//}
		return true;
	}

	return false;
}

bool CCommData::IsStatusAlaram()
{
	if (GetEndReason() == eIFBoardCyclerCodeAlarm)
	{
		////20220325 YGYUN Moxa 로 PLC와 통신하는 경우 
		//if (CMgrConfig::GetMgr()->m_strDioType == "MOXA")
		//{
		//	//Ready Bit On
		//	CMgrDio* pMgrDio = CMgrDio::GetMgr();

		//	pMgrDio->SetMoxa_PC_Ready(false);
		//	pMgrDio->SetMoxa_PC_Busy(false);
		//}
		return true;
	}

	return false;
}

bool CCommData::IsContactRegUpdate()
{
	return m_bContactRegUpdate;
}

void CCommData::SetContactRegUptate(bool bUpdate)
{
	m_bContactRegUpdate = bUpdate;
}

void CCommData::SetTimeData(int nValue)
{
	if (nValue == 0)
	{
		m_nTimeData = 0;
	}
	m_nTimeData = nValue - m_nTimeData2;
	m_nTimeData2 = nValue;
}

bool	CCommData::IsUsingCyclerCurrent()
{
	// 실제 충방전기에서 Current 를 쓰는 모드인지 확인
	if (eIFBoardOperationChargeCC == m_nOpcode || eIFBoardOperationChargeCCCV == m_nOpcode || eIFBoardOperationChargeCP == m_nOpcode || eIFBoardOperationChargeCPCV == m_nOpcode ||
		eIFBoardOperationDischargeCC == m_nOpcode || eIFBoardOperationDischargeCCCV == m_nOpcode || eIFBoardOperationDischargeCP == m_nOpcode || eIFBoardOperationDischargeCPCV == m_nOpcode)
	{
		return true;
	}
	return false;
}
void	CCommData::AddDAQ(CDAQ* pDAQ)
{
	m_listDAQ.AddTail(pDAQ);
}

void	CCommData::RemoveAllDAQ()
{
	m_listDAQ.RemoveAll();
}

CList< CDAQ*, CDAQ* >& CCommData::GetDAQList()
{
	return m_listDAQ;
}

bool	CCommData::CheckPatternLoopStepChange(int nStepIndexIFBoard, int nChamberIndex)
{
	//스탭이 패턴인지 구분하여야한다. 

	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(0);
	if (pChannel != NULL)
	{
		CStep* pStep = pChannel->GetMgr()->GetAtStep(nStepIndexIFBoard);
		if (pStep == NULL)
		{
			return false;
		}

		//만약 외부 패턴이라면
		if (pStep->GetStepPattern() == eIFBoardStepTypeExternalPattern)
		{
			SScheduleComplete* lpScheduleCompleteTime = pStep->GetScheduleCompleteFromName(_T("Ti"));

			if (g_bPatternStep[0] != true && lpScheduleCompleteTime->fValue >= 999.0)
			{
				g_bPatternStep[0] = true;
				g_nPatternStartStep[0] = nStepIndexIFBoard;
			}
			if (g_bPatternStep[0] == true)
			{
				if ((nStepIndexIFBoard - (g_nPatternStartStep[0] - 1)) == g_nPatternStepCount)
				{
					g_bPatternStep[0] = false;
					return true;
				}
			}
		}
	}
	return false;
#if 0
	CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(m_nChannel);
	if (pChannel != NULL)
	{
		CStep* pStep = pChannel->GetMgr()->GetAtStep(nStepIndexIFBoard);
		if (pStep == NULL)
		{
			return false;
		}

		//만약 외부 패턴이라면
		if (pStep->GetStepPattern() == eIFBoardStepTypeExternalPattern)
		{
			SScheduleComplete* lpScheduleCompleteTime = pStep->GetScheduleCompleteFromName(_T("Ti"));
			
			if (g_bPatternStep[m_nChannel] != true && lpScheduleCompleteTime->fValue >= 999.0)
			{
				g_bPatternStep[m_nChannel] = true;
				g_nPatternStartStep[m_nChannel] = nStepIndexIFBoard;
			}

		}
		if (g_bPatternStep[m_nChannel] == true)
		{
			if ((nStepIndexIFBoard - (g_nPatternStartStep[m_nChannel]-1)) == g_nPatternStepCount)
			{
				pChannel->SendJump(g_nPatternStartStep[m_nChannel]);
				g_bPatternStep[m_nChannel] = false;
				Sleep(500);
			}
		}
	}
	return true;
#endif // 0
	
}

// -------------------------------------------------------------------------------------------------
// PCAN

void CCommData::SetSendMessageCount(int nCount, int nFuncAgrs)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_nSendMessageCount = nCount;
	else if (FuncAgrsIncrease == nFuncAgrs)
	{
		m_nSendMessageCount++;

		if (m_nSendMessageCount == 1000)
		{
			m_nSendMessageCount = 0;
		}
	}
}

int CCommData::GetSendMessageCount()
{
	return m_nSendMessageCount;
}

void CCommData::SetRecvMessageCount(int nCount, int nFuncAgrs)
{
	if (FuncAgrsEqual == nFuncAgrs)
		m_nRecvMessageCount = nCount;
	else if (FuncAgrsIncrease == nFuncAgrs)
	{
		m_nRecvMessageCount++;

		if (m_nRecvMessageCount == 1000)
		{
			m_nRecvMessageCount = 0;
		}
	}
}

int CCommData::GetRecvMessageCount()
{
	return m_nRecvMessageCount;
}


