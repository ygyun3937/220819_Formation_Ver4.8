#include "stdafx.h"
#include "CMgrChamber.h"
#include "MgrComm.h"
#include "PnxUtil.h"
#include "MgrAlarm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMgrChamber::CMgrChamber()
{
	LoadChamber();
}

CMgrChamber::~CMgrChamber()
{
	auto iter = m_vecDeviceChamber.begin();
	auto iterE = m_vecDeviceChamber.end();

	while (iter != iterE)
	{
		SChamber* lpDeviceChamber = (*iter);

		if (lpDeviceChamber)
		{
			delete lpDeviceChamber;
			lpDeviceChamber = nullptr;
		}

		++iter;
	}

	m_vecDeviceChamber.clear();
}


void	CMgrChamber::LoadChamber()
{
	CMgrComm* pMgr = CMgrComm::GetMgr();

	if (!pMgr)
		return;

	std::vector<SDevice*> vecValue;
	pMgr->GetDeivceFromType(eDeviceTypeChamber, vecValue);

	auto iter = vecValue.begin();
	auto iterE = vecValue.end();

	while (iter != iterE)
	{
		SDevice* lpDevice = (*iter);

		if (lpDevice)
		{
			SChamber* lpDeviceChamber = new SChamber;

			lpDeviceChamber->strName.Format("Chamber%d", lpDevice->nIndex + 1);

			lpDeviceChamber->nStatus = 0;
			lpDeviceChamber->nNumber = lpDevice->nIndex;
			lpDeviceChamber->fCurrentValue = lpDeviceChamber->fCurrentValue;
			lpDeviceChamber->fSettingValue = lpDeviceChamber->fSettingValue;
			lpDeviceChamber->fManualSettingValue = 30.0;
			lpDeviceChamber->bChamberExec = false;
			//lpDeviceChamber->bSendOffCommand		= false;
			//lpDeviceChamber->bSendOnCommand		= false;
			//lpDeviceChamber->bSendChangeValue		= false;

			for (int i = 0; i < lpDevice->arrChannel.GetSize(); ++i)
			{
				CString strCyclerChannelNumber = lpDevice->arrChannel.GetAt(i);

				lpDeviceChamber->vecCyclerChannel.push_back(strCyclerChannelNumber);
			}

			m_vecDeviceChamber.push_back(lpDeviceChamber);
		}

		++iter;
	}
}

SChamber*	CMgrChamber::GetDevice(int nChamberNumber)
{
	auto iter = m_vecDeviceChamber.begin();
	auto iterE = m_vecDeviceChamber.end();

	while (iter != iterE)
	{
		SChamber* lpDeviceChamber = (*iter);

		if (lpDeviceChamber)
		{
			if (lpDeviceChamber->nNumber == nChamberNumber)
				return lpDeviceChamber;
		}

		++iter;
	}

	return nullptr;
}

SChamber*	CMgrChamber::GetDeivceCyclerChannel(int nCyclerChannelNumber)
{
	auto iter = m_vecDeviceChamber.begin();
	auto iterE = m_vecDeviceChamber.end();

	while (iter != iterE)
	{
		SChamber* lpDeviceChamber = (*iter);

		if (lpDeviceChamber)
		{
			auto it = lpDeviceChamber->vecCyclerChannel.begin();
			auto itE = lpDeviceChamber->vecCyclerChannel.end();

			while (it != itE)
			{
				if (nCyclerChannelNumber == atoi(*it))
					return lpDeviceChamber;

				++it;
			}
		}

		++iter;
	}

	return nullptr;
}

void SChamber::GetChamberInfo(std::vector<CString>& vecValue, bool bViewFlag)
{
	vecValue.clear();
	{
		vecValue.push_back(strName);
	}

	{
		if (bViewFlag != true)
		{
			auto iter = vecCyclerChannel.begin();
			auto iterE = vecCyclerChannel.end();

			CString str;

			while (iter != iterE)
			{
				int chNum = _ttoi(*iter) + 1;
				CString strChNum;
				strChNum.Format(_T("%d"), chNum);

				//str += *iter;
				str += strChNum;
				str += ",";

				++iter;
			}

			str.TrimRight(",");

			vecValue.push_back(str);
		}

	}

	{
		// 상태값 정의 필요.. 고민좀 더 해봐야할듯
		CString strValue;

		if (false == bChamberExec)
			strValue = "Offline";
		else
			strValue = "Running";

		vecValue.push_back(strValue);
	}

	{
		CString strValue;
		strValue.Format("%.3f", fCurrentValue);

		vecValue.push_back(strValue);
	}

	{
		if (bViewFlag != true)
		{
			CString strValue;
			strValue.Format("%.3f", fSettingValue);

			vecValue.push_back(strValue);
		}
	}

	{
		if (bViewFlag != true)
		{
			CString strValue;
			strValue.Format("%.3f", fManualSettingValue);

			vecValue.push_back(strValue);
		}
	}

	{
		// 현재 상태값을 보고 텍스트를 변경하자. 구동/해제
		CString strValue;
		if (true == bChamberAlarm)
		{
			strValue = "Alarm";
			ALARM_ITEM item;
			CString strAlarmCode;
			CString strAlarmMsg;
			strAlarmCode.Format("Chamber Alarm (%s (Code : %d))", g_lpszChamberAlarmString[nAlarmCode], nAlarmCode);
			strAlarmMsg.Format("Chamber Alarm Check Please");
			CMgrAlarm::GetMgr()->AddEqAlarm(ALARM_CHAMBER_ALARM_ERROR, strAlarmCode, ALARM_LIGHT, strAlarmMsg, &item);
		}
		else
		{
			if (false == bChamberExec)
			{
				strValue = "Chamber Off";
			}
			else
			{
				strValue = "Chamber On";
			}
		}
		vecValue.push_back(strValue);
	}


}
