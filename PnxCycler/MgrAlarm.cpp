#include "stdafx.h"
#include "MgrAlarm.h"

#include "MgrChannel.h"
#include "PnxUtil.h"
#include <bitset>
#include "MgrCommData.h"
#include "MgrConfig.h"
#include "DlgViewAlarm.h"
#include "CDlgViewEqpChannelStatus.h"
#include "MgrStatus.h"
#include "SockProc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define	OneByte	( 8 )
#define	AlarmBitMaxCount	( 16 )

CMgrAlarm::CMgrAlarm()
{
	m_pThread = NULL;
	m_bLoop = FALSE;


	InitSysAlarmList(); //lyw4_1 : 알람 리스트 초기화 호출
	LoadAlarmList();
	LoadAlarmInverterList();
}


CMgrAlarm::~CMgrAlarm()
{
	RemoveAll();
	RemoveAll_Inverter();
	//<!--20210507 add by swseo
	for (auto it = m_mapSysAlarm.begin(); it != m_mapSysAlarm.end(); ++it)
	{
		delete it->second;
	}
	m_mapSysAlarm.clear();//-->
	for (auto it = m_mapInverterAlarm.begin(); it != m_mapInverterAlarm.end(); ++it)
	{
		delete it->second;
	}
	m_mapSysAlarm.clear();//-->




}

//lyw4_1 : 알람 리스트 초기화
void	CMgrAlarm::InitSysAlarmList()
{
	for (auto it = m_mapSysAlarm.begin(); it != m_mapSysAlarm.end(); ++it)
	{
		delete it->second;
	}
	m_mapSysAlarm.clear();

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		CList<CSysAlarm>* list = new CList<CSysAlarm>();
		m_mapSysAlarm.insert(std::pair<int, CList<CSysAlarm>*>(pChannel->GetChannelNumber(), list));
	}
}

void	CMgrAlarm::AddSysAlarm(int nChannelNumber, CString strTitle, CString strContents, int nAlarmlevel)
{
	auto it = m_mapSysAlarm.find(nChannelNumber);
	if (it == m_mapSysAlarm.end())
		return;

	CList<CSysAlarm>* pListSysAlarm = it->second;

	if (NULL == pListSysAlarm)
		return;

	//Avoid duplicate alarms
	if (pListSysAlarm->GetCount() > 0)
	{
		if (pListSysAlarm->GetTail().GetAlarmTitle() == strTitle)
			return;
	}

	CSysAlarm sysAlarm ;
	sysAlarm.SetAlarm(strTitle, strContents);
	pListSysAlarm->AddTail(sysAlarm);

	//CChannel* pChannel = CMgrChannel::GetMgr()->GetChannel(nChannelNumber);
	//pChannel->SetChannelStatus(eCyclerChannelStateError);
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;
	int nStageNo = nChannelNumber / nStageChCount;

	//220812 YGY Add system alarm condition ------------(Start)
	//■ In case of Heavy Alarm
	//		1. Schedule end (All channels for that station)
	//		2. Alarm occurrence
	//		3. release battery contact
	//
	//■ In case of Light Alarm
	//		1. The corresponding channel system alarm occurs(Other channels work normally.)
	if (nAlarmlevel == ALARM_HEAVY)
	{
		//Schedule end (All channels for that station)
		CMgrChannel* pMgr = CMgrChannel::GetMgr();
		if (!pMgr)
			return;
		pMgr->SetCyclerChannelPauseOption(eCyclerCommandOptionCompleteImmediately, nStageNo);
	}

	//Alarm occurrence
	CMgrStatus::GetMgr()->SetEqp_SystemAlarm(nChannelNumber / nStageChCount, nAlarmlevel);
	
	//--------------------------------------------------(End)

	WriteStsAlarmHistory(nChannelNumber, sysAlarm);

	//OutputDebugString(_T("Add sys alarm\r\n")); //lyw log
}

void	CMgrAlarm::ClearSysAlarm(int nChannelNumber)
{
	auto it = m_mapSysAlarm.find(nChannelNumber);
	if (it == m_mapSysAlarm.end())
		return;

	CList<CSysAlarm>* pListSysAlarm = it->second;

	if (NULL == pListSysAlarm)
		return;

	pListSysAlarm->RemoveAll();

	CCommData*	pCommData = NULL;
	pCommData = CMgrCommData::GetMgr()->GetAt(nChannelNumber);
	pCommData->SetSystemSafetyEndReason(0);
	int nStageChCount = CMgrConfig::GetMgr()->GetSubEqpInfo().nSubEqpStageChannelCount;

	CMgrStatus::GetMgr()->SetEqp_SystemAlarm(nChannelNumber / nStageChCount, false);

}


BOOL CMgrAlarm::AddEqAlarm(int nCode, CString text, int level,  CString comment, ALARM_ITEM * item)
{
	//Avoid duplicate alarms
	CString strMsg;
	int nSize = m_veEqAlarm.size();
	for (int i = 0; i < nSize; i++)
	{
		if (m_veEqAlarm[i].code == nCode)
			return TRUE;
	}
	item->code = nCode;
	item->text.Format(text);
	item->Comment.Format(comment);
	item->level = level;
	item->time = CTime::GetCurrentTime();

	m_veEqAlarm.push_back(*item);


	ALARM_ITEM alarm;
	alarm.code = nCode;
	alarm.text.Format("%s", text);
	alarm.level = level;

	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (pWnd)
		::PostMessage(pWnd->GetSafeHwnd(), WM_UPDATE_VIEW_ALARM, (WPARAM)&alarm, 0);

	WriteEQAlarmHistory(nCode, text);


	return TRUE;
}

tagVEALARM CMgrAlarm::GetEqAlarm()
{
	return m_veEqAlarm;
}

void CMgrAlarm::ClearEqAlarm()
{
	int nSize = m_veEqAlarm.size();
	if (nSize == 0)
		return;
	for (int i = 0; i < nSize; i++)
	{
		m_veEqAlarm.erase(m_veEqAlarm.begin());
	}
}

CList<CSysAlarm>* CMgrAlarm::GetSysAlarmList(int nChannelNumber)
{
	auto it = m_mapSysAlarm.find(nChannelNumber);
	if (it == m_mapSysAlarm.end())
		return NULL;

	CList<CSysAlarm>* pListSysAlarm = it->second;

	return pListSysAlarm;
}


void	CMgrAlarm::LoadAlarmList()
{
	RemoveAll();

	CList< CChannel*, CChannel* >& listChannel = CMgrChannel::GetMgr()->GetChannelList();

	auto Pos = listChannel.GetHeadPosition();

	while (Pos)
	{
		CChannel* pChannel = listChannel.GetNext(Pos);

		if (NULL == pChannel)
			continue;

		CAlarmMap* pAlarmMap = new CAlarmMap;

		pAlarmMap->LoadAlarm();

		Add(pChannel->GetChannelNumber(), pAlarmMap);
	}
}

void CMgrAlarm::LoadAlarmInverterList()
{
	RemoveAll_Inverter();
	bool bSubStage1Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage1Use;
	bool bSubStage2Use = CMgrConfig::GetMgr()->GetSubEqpInfo().bStage2Use;


	if (bSubStage1Use == true)
	{
		CAlarmInverterMap* pAlarmMap = new CAlarmInverterMap;

		pAlarmMap->LoadAlarm();

		Add_Inverter(0, pAlarmMap);
	}
	if (bSubStage2Use == true)
	{
		CAlarmInverterMap* pAlarmMap = new CAlarmInverterMap;

		pAlarmMap->LoadAlarm();

		Add_Inverter(1, pAlarmMap);
	}

	

}

void	CMgrAlarm::Add(int nChannelNumber, CAlarmMap* pNewElement)
{
	auto it = m_mapAlarm.find(nChannelNumber);

	// 못찾았음
	if (it == m_mapAlarm.end())
	{
		m_mapAlarm.insert(std::pair<int, CAlarmMap*>(nChannelNumber, pNewElement));
	}
}

void CMgrAlarm::Add_Inverter(int nStationNo, CAlarmInverterMap* pNewElement)
{
	auto it = m_mapInverterAlarm.find(nStationNo);

	// 못찾았음
	if (it == m_mapInverterAlarm.end())
	{
		m_mapInverterAlarm.insert(std::pair<int, CAlarmInverterMap*>(nStationNo, pNewElement));
	}
}

void	CMgrAlarm::RemoveAll()
{
	for (auto it = m_mapAlarm.begin(); it != m_mapAlarm.end(); ++it)
	{
		delete it->second;
	}

	m_mapAlarm.clear();
}

void CMgrAlarm::RemoveAll_Inverter()
{
	for (auto it = m_mapInverterAlarm.begin(); it != m_mapInverterAlarm.end(); ++it)
	{
		delete it->second;
	}

	m_mapInverterAlarm.clear();
}

void	CMgrAlarm::GetChannelAlarm(int nChannel, int nIndex, CStringArray& arr)
{
	auto it = m_mapAlarm.find(nChannel);

	if (it == m_mapAlarm.end())
		return;

	// 실제 List 에 뿌려야 할 각 Row 는 AlarmType1 의 Str,AlarmType2 의 Str,AlarmType3 의 Str,AlarmType4 의 Str,AlarmType1 의 Str 이다.
	// 즉, 각 Type 별 가장 상위의 Data 를 하나씩 빼와서 어레이에 뿌려줘야 함..

	CAlarmMap* pAlarmMap = it->second;

	if (NULL == pAlarmMap)
		return;

	CString str;

	str.Format(_T("BIT %d"), nIndex);

	arr.Add(str);

	for (int i = eAlarmType::eAlarmTypeIFWarning; i < eAlarmType::eAlarmTypeMaxCount; ++i)
	{
		CAlarmList* pAlarmList = pAlarmMap->GetAlarmList(i);

		if (NULL == pAlarmList)
			continue;

		CAlarm* pAlarm = pAlarmList->GetAlarm(nIndex);

		if (NULL == pAlarm)
			continue;

		arr.Add(pAlarm->GetAlarmStr());
	
	}
}

void CMgrAlarm::GetChannelAlarmComment(int nChannel, int nIndex, CStringArray & arr)
{
	auto it = m_mapAlarm.find(nChannel);

	if (it == m_mapAlarm.end())
		return;

	// 실제 List 에 뿌려야 할 각 Row 는 AlarmType1 의 Str,AlarmType2 의 Str,AlarmType3 의 Str,AlarmType4 의 Str,AlarmType1 의 Str 이다.
	// 즉, 각 Type 별 가장 상위의 Data 를 하나씩 빼와서 어레이에 뿌려줘야 함..

	CAlarmMap* pAlarmMap = it->second;

	if (NULL == pAlarmMap)
		return;

	CString str;

	//str.Format(_T("BIT %d"), nIndex);

	//arr.Add(str);

	for (int i = eAlarmType::eAlarmTypeIFWarning; i < eAlarmType::eAlarmTypeMaxCount; ++i)
	{
		CAlarmList* pAlarmList = pAlarmMap->GetAlarmList(i);

		if (NULL == pAlarmList)
			continue;

		CAlarm* pAlarm = pAlarmList->GetAlarm(nIndex);

		if (NULL == pAlarm)
			continue;

		arr.Add(pAlarm->GetAlarmStr());
		if (pAlarm->GetAlarmComment().IsEmpty() == FALSE)
		{
			arr.Add(pAlarm->GetAlarmComment());
		}
	}

	
}

void CMgrAlarm::GetStationInverterAlarm(int nStationNo, int nIndex, CStringArray & arr)
{
	auto it = m_mapInverterAlarm.find(nStationNo);

	if (it == m_mapInverterAlarm.end())
		return;

	// 실제 List 에 뿌려야 할 각 Row 는 AlarmType1 의 Str,AlarmType2 의 Str,AlarmType3 의 Str,AlarmType4 의 Str,AlarmType1 의 Str 이다.
	// 즉, 각 Type 별 가장 상위의 Data 를 하나씩 빼와서 어레이에 뿌려줘야 함..

	CAlarmInverterMap* pInverterAlarmMap = it->second;

	if (NULL == pInverterAlarmMap)
		return;

	CString str;

	str.Format(_T("BIT %d"), nIndex);

	arr.Add(str);

	for (int i = eAlarmInverterType::eAlarmInverterTypeCANIGBT; i < eAlarmInverterType::eAlarmInverterTypeMaxCount; ++i)
	{
		CAlarmInverterList* pAlarmList = pInverterAlarmMap->GetAlarmList(i);

		if (NULL == pAlarmList)
			continue;

		CAlarmInverter* pAlarm = pAlarmList->GetAlarm(nIndex);

		if (NULL == pAlarm)
			continue;

		arr.Add(pAlarm->GetAlarmStr());
		//arr.Add(pAlarm->GetAlarmComment());
	}


}

void CMgrAlarm::GetStationInverterAlarmComment(int nStationNo, int nIndex, CStringArray & arr)
{
	auto it = m_mapInverterAlarm.find(nStationNo);

	if (it == m_mapInverterAlarm.end())
		return;

	// 실제 List 에 뿌려야 할 각 Row 는 AlarmType1 의 Str,AlarmType2 의 Str,AlarmType3 의 Str,AlarmType4 의 Str,AlarmType1 의 Str 이다.
	// 즉, 각 Type 별 가장 상위의 Data 를 하나씩 빼와서 어레이에 뿌려줘야 함..

	CAlarmInverterMap* pInverterAlarmMap = it->second;

	if (NULL == pInverterAlarmMap)
		return;

	CString str;

	//str.Format(_T("BIT %d"), nIndex);

	//arr.Add(str);

	for (int i = eAlarmInverterType::eAlarmInverterTypeCANIGBT; i < eAlarmInverterType::eAlarmInverterTypeMaxCount; ++i)
	{
		CAlarmInverterList* pAlarmList = pInverterAlarmMap->GetAlarmList(i);

		if (NULL == pAlarmList)
			continue;

		CAlarmInverter* pAlarm = pAlarmList->GetAlarm(nIndex);

		if (NULL == pAlarm)
			continue;

		arr.Add(pAlarm->GetAlarmStr());
		arr.Add(pAlarm->GetAlarmComment());
	}
}

void	CMgrAlarm::GetChannelAlarm(int nChannel, int nIndex, __int16* pBuff)
{
	auto it = m_mapAlarm.find(nChannel);
	bitset<16> bitAlarm;
	bitAlarm.reset();

	if (it == m_mapAlarm.end())
		return;

	// 실제 List 에 뿌려야 할 각 Row 는 AlarmType1 의 Str,AlarmType2 의 Str,AlarmType3 의 Str,AlarmType4 의 Str,AlarmType1 의 Str 이다.
	// 즉, 각 Type 별 가장 상위의 Data 를 하나씩 빼와서 어레이에 뿌려줘야 함..

	CAlarmMap* pAlarmMap = it->second;

	if (NULL == pAlarmMap)
		return;

	CAlarmList* pAlarmList = pAlarmMap->GetAlarmList(nIndex);

	for (int i = 0; i < AlarmBitMaxCount; ++i)
	{
		if (NULL == pAlarmList)
			continue;

		CAlarm* pAlarm = pAlarmList->GetAlarm(i);

		if (NULL == pAlarm)
			continue;
		bitAlarm[i] = pAlarm->GetAlarmSet();
	}

	*pBuff = (__int16)bitAlarm.to_ulong();

	*pBuff = _byteswap_ushort(*pBuff);
}

CAlarmMap*	CMgrAlarm::GetAlarmMap(int nChannelNo)
{
	auto it = m_mapAlarm.find(nChannelNo);

	if (it == m_mapAlarm.end())
		return NULL;

	return it->second;
}

CAlarmInverterMap * CMgrAlarm::GetInverterAlarmMap(int nStationNo)
{
	auto it = m_mapInverterAlarm.find(nStationNo);

	if (it == m_mapInverterAlarm.end())
		return NULL;

	return it->second;
}

BOOL	CMgrAlarm::ExistChannelAlarm(int nChannel)
{
	auto it = m_mapAlarm.find(nChannel);

	if (it == m_mapAlarm.end())
		return FALSE;

	CAlarmMap* pAlarmMap = it->second;

	if (NULL == pAlarmMap)
		return FALSE;

	return pAlarmMap->ExistAlarm();
}

BOOL CMgrAlarm::ExistInverterAlarm(int nStagionNo)
{
	auto it = m_mapInverterAlarm.find(nStagionNo);

	if (it == m_mapInverterAlarm.end())
		return FALSE;

	CAlarmInverterMap* pAlarmInverterMap = it->second;

	if (NULL == pAlarmInverterMap)
		return FALSE;

	return pAlarmInverterMap->ExistAlarm();
}

void	CMgrAlarm::StartThread()
{
	/*
	if (!m_pThread)
		m_pThread = AfxBeginThread(OnDB2AlarmThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	*/

}

void	CMgrAlarm::StopThread()
{
	m_bLoop = FALSE;

	if (m_pThread)
	{
		::WaitForSingleObject(m_pThread->m_hThread, INFINITE);

		m_pThread = NULL;
	}
}

UINT	CMgrAlarm::OnDB2AlarmThread(LPVOID pParam)
{
	CMgrAlarm* pMgr = (CMgrAlarm*)pParam;
	//2021-02-23 cnz
	if (pMgr == NULL)
		return 0;

	while (pMgr->m_bLoop)
	{
		Sleep(1);
	}

	return 0;
}

void	CMgrAlarm::AddAlarm(int nChannelNumber, int nType, BYTE byAlarm)
{
	CAlarmMap* pAlarmMap = GetAlarmMap(nChannelNumber);

	if (NULL == pAlarmMap)
		return;

	CAlarmList* pAlarmList = pAlarmMap->GetAlarmList(nType);

	if (NULL == pAlarmList)
		return;

	// Alarm 을 Byte 단위로 받음 만약 넘으면 Size 값 늘리면 됨 ㅎㅎ
	BOOL arr[OneByte] = { FALSE, };

	BOOL bAlarmClear = FALSE;

	for (int i = 0; i < OneByte; ++i)
	{
		arr[i] = (byAlarm & (1 << i)) != 0;

		if (i == 3 && arr[i] == TRUE)
		{
			bAlarmClear = TRUE;

			break;
		}

		// 기존에 Grid Alarm 의 0, 1 은 인식 하지 않도록 설정 되어 있었음.
		/*
		if (AlarmTypeGrid == nType)
		{
			if (i == 0 || i == 1 )
				continue;
		}
		*/

		CAlarm* pAlarm = pAlarmList->GetAlarm(i);

		if (pAlarm)
		{
			pAlarm->SetAlarmSet(arr[i]);
		}
	}

	// 현재 Serial Module 에서는 0x08 값이 TRUE 면 Alarm Clear 로 인식한다.
	if (TRUE == bAlarmClear)
	{
		pAlarmList->SetAllAlarm(FALSE);
	}
}

// CString	CMgrAlarm::AddAlarmCan(int nChannelNumber, int nWr, int nGrd, int nDc, int nFault)
// {
// 	
// }

CString	CMgrAlarm::AddAlarm(int nChannelNumber, int nType, BYTE byAlarm1, BYTE byAlarm2)
{
	CAlarmMap* pAlarmMap = GetAlarmMap(nChannelNumber);

	if (NULL == pAlarmMap)
		return "";

	CAlarmList* pAlarmList = pAlarmMap->GetAlarmList(nType);

	if (NULL == pAlarmList)
		return "";

	

	CString strAlarmText;

	pAlarmList->SetAllAlarm(FALSE);

	CArray< int, int > arr;

	CPnxUtil::GetMgr()->SplitByte(byAlarm1, arr);

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nIndex = arr.GetAt(i);

		CAlarm* pAlarm = pAlarmList->GetAlarm(nIndex);

		if (pAlarm)
		{
			pAlarm->SetAlarmSet(TRUE);

			strAlarmText.Format("%s", (LPCSTR)pAlarm->GetAlarmText());

			WriteAlarmHistory(nChannelNumber, pAlarm);
		}
	}

	arr.RemoveAll();

	CPnxUtil::GetMgr()->SplitByte(byAlarm2, arr);

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nIndex = arr.GetAt(i);

		CAlarm* pAlarm = pAlarmList->GetAlarm(nIndex + 8);

		if (pAlarm)
		{
			pAlarm->SetAlarmSet(TRUE);

			strAlarmText.Format("%s", (LPCSTR)pAlarm->GetAlarmText());

			WriteAlarmHistory(nChannelNumber, pAlarm);
		}
	}

	if (TRUE == pAlarmList->ExistAlarm())
	{
		CWnd* pWnd = AfxGetApp()->GetMainWnd();

		if (pWnd)
			::PostMessage(pWnd->GetSafeHwnd(), WM_UPDATE_VIEW_ALARM, 0, 0);
			//::PostMessage(pWnd->GetSafeHwnd(), WM_UPDATE_VIEW_ALARM_STATUS, 0, 0);
	}

	return strAlarmText;
}

CString CMgrAlarm::AddInverterAlarmCan(int nStationNumber, int nAlarmType, BYTE byteAlarm2, BYTE byteAlarm1)
{
	CAlarmInverterMap* pAlarmMap = GetInverterAlarmMap(nStationNumber);

	if (NULL == pAlarmMap)
		return "";

	CAlarmInverterList* pAlarmList = pAlarmMap->GetAlarmList(nAlarmType);

	if (NULL == pAlarmList)
		return "";

	CString strAlarmText;

	pAlarmList->SetAllAlarmInverter(FALSE);

	CArray< int, int > arr;

	CPnxUtil::GetMgr()->SplitByte(byteAlarm1, arr);

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nIndex = arr.GetAt(i);

		CAlarmInverter* pAlarm = pAlarmList->GetAlarm(nIndex);

		if (pAlarm)
		{
			pAlarm->SetAlarmSet(TRUE);

			strAlarmText.Format("%s", (LPCSTR)pAlarm->GetAlarmText());

			WriteInverterAlarmHistory(nStationNumber, pAlarm);
		}
	}

	arr.RemoveAll();

	CPnxUtil::GetMgr()->SplitByte(byteAlarm2, arr);

	for (int i = 0; i < arr.GetSize(); ++i)
	{
		int nIndex = arr.GetAt(i);

		CAlarmInverter* pAlarm = pAlarmList->GetAlarm(nIndex + 8);

		if (pAlarm)
		{
			pAlarm->SetAlarmSet(TRUE);

			strAlarmText.Format("%s", (LPCSTR)pAlarm->GetAlarmText());

			WriteInverterAlarmHistory(nStationNumber, pAlarm);
		}
	}

	if (TRUE == pAlarmList->ExistAlarm())
	{
		CWnd* pWnd = AfxGetApp()->GetMainWnd();

		if (pWnd)
			::PostMessage(pWnd->GetSafeHwnd(), WM_UPDATE_VIEW_ALARM, 0, 0);
			//::PostMessage(pWnd->GetSafeHwnd(), WM_UPDATE_VIEW_ALARM_STATUS, 0, 0);
	}

	return strAlarmText;
}


void	CMgrAlarm::WriteAlarmHistory(int nChannelNumber, CAlarm* pAlarm)
{
	if (NULL == pAlarm)
		return;

	CString strFilePath;

	CTime tm = CTime::GetCurrentTime();

	strFilePath.Format("%s\\AlarmHistory\\%s.txt", (LPCSTR)GetExePath(), tm.Format("%Y_%m_%d").GetString());

	FILE* fp = NULL;

	fopen_s(&fp, strFilePath, "a+");

	if (NULL == fp)
		return;

	fprintf(fp, "[Alarm] %s, CH(%d), %s\n", (LPCSTR)CPnxUtil::GetMgr()->GetTime(), nChannelNumber + 1, (LPCSTR)pAlarm->GetAlarmText());

	fclose(fp);
}

void CMgrAlarm::WriteStsAlarmHistory(int nChannelNumber, CSysAlarm pAlarm)
{
	
	CString strFilePath;

	CTime tm = CTime::GetCurrentTime();

	strFilePath.Format("%s\\AlarmHistory\\%s.txt", (LPCSTR)GetExePath(), tm.Format("%Y_%m_%d").GetString());

	FILE* fp = NULL;

	fopen_s(&fp, strFilePath, "a+");

	if (NULL == fp)
		return;

	fprintf(fp, "[System Alarm] %s, CH(%d), %s\n", (LPCSTR)CPnxUtil::GetMgr()->GetTime(), nChannelNumber + 1, (LPCSTR)pAlarm.GetAlarmContents());

	fclose(fp);
}

void CMgrAlarm::WriteInverterAlarmHistory(int nStationNumber, CAlarmInverter * pAlarm)
{
	if (NULL == pAlarm)
		return;

	CString strFilePath;

	CTime tm = CTime::GetCurrentTime();

	strFilePath.Format("%s\\AlarmHistory\\%s.txt", (LPCSTR)GetExePath(), tm.Format("%Y_%m_%d").GetString());

	FILE* fp = NULL;

	fopen_s(&fp, strFilePath, "a+");

	if (NULL == fp)
		return;

	fprintf(fp, "[Inverter Alarm] %s, Station(%d), %s\n", (LPCSTR)CPnxUtil::GetMgr()->GetTime(), nStationNumber + 1, (LPCSTR)pAlarm->GetAlarmText());

	fclose(fp);
}

void CMgrAlarm::WriteEQAlarmHistory(int nCode, CString text)
{
	CString strFilePath;

	CTime tm = CTime::GetCurrentTime();

	strFilePath.Format("%s\\AlarmHistory\\%s.txt", (LPCSTR)GetExePath(), tm.Format("%Y_%m_%d").GetString());

	FILE* fp = NULL;

	fopen_s(&fp, strFilePath, "a+");

	if (NULL == fp)
		return;

	fprintf(fp, "[EQ Alarm] %s, Alarm Code(%d), %s\n", (LPCSTR)CPnxUtil::GetMgr()->GetTime(), nCode, (LPCSTR)text);

	fclose(fp);
}
